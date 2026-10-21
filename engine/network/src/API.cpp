


// third time's the charm
#include "Network/API.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "ServerReplicator.hpp"
#include "ClientReplicator.hpp"
#include "Network/Players.hpp"
#include "Network/Player.hpp"
#include "Marker.hpp"

#include "NetworkSettings.hpp"
#include "DataModel/DataModel.hpp"

#include "DataModel/GlobalSettings.hpp"

#include "DataModel/HackDefines.hpp"

#include "DataModel/Workspace.hpp"

#include "GuidRegistryService.hpp"
#include "RakNet/RakNetVersion.hpp"
#include "Utility/Statistics.hpp"
#include "Utility/URL.hpp"
#include "Utility/Utilities.hpp"

#include "FastLog.hpp"

// RakNet
#include "RakNet/StringCompressor.hpp"
#include "RakNet/StringTable.hpp"

#include <boost/algorithm/string.hpp>

FASTSTRINGVARIABLE(ClientExternalBrowserUserAgent, "Roblox/WinInet")

std::string Aya::Network::versionB;
std::string Aya::Network::securityKey;
unsigned int Aya::initialProgramHash = 0;

#if RAKNET_PROTOCOL_VERSION != 5
#error
#endif


RBX_REGISTER_CLASS(Aya::Network::Client);
RBX_REGISTER_CLASS(Aya::Network::Server);
RBX_REGISTER_CLASS(Aya::Network::Player);
RBX_REGISTER_CLASS(Aya::Network::Players);
RBX_REGISTER_CLASS(Aya::NetworkSettings);
RBX_REGISTER_CLASS(Aya::Network::Peer);
RBX_REGISTER_CLASS(Aya::Network::Marker);
RBX_REGISTER_CLASS(Aya::Network::Replicator);
RBX_REGISTER_CLASS(Aya::Network::ServerReplicator);
RBX_REGISTER_CLASS(Aya::Network::ClientReplicator);
RBX_REGISTER_CLASS(Aya::Network::GuidRegistryService);

RBX_REGISTER_ENUM(PacketPriority);
RBX_REGISTER_ENUM(PacketReliability);
RBX_REGISTER_ENUM(Aya::Network::FilterResult);
RBX_REGISTER_ENUM(Aya::Network::Player::MembershipType);
RBX_REGISTER_ENUM(Aya::Network::Player::ChatMode);
RBX_REGISTER_ENUM(Aya::Network::Players::PlayerChatType);
RBX_REGISTER_ENUM(Aya::Network::Players::ChatOption);
RBX_REGISTER_ENUM(Aya::NetworkSettings::PhysicsSendMethod);
RBX_REGISTER_ENUM(Aya::NetworkSettings::PhysicsReceiveMethod);

namespace Aya
{
namespace Network
{
// Prevent string compressors from being created at the same time
class SafeInitFree
{
public:
    SafeInitFree()
    {
        RakNet::StringCompressor::AddReference();
        RakNet::StringTable::AddReference();
    }
    ~SafeInitFree()
    {
        RakNet::StringCompressor::RemoveReference();
        RakNet::StringTable::RemoveReference();
    }
};
} // namespace Network
} // namespace Aya

static bool _isPlayerAuthenticationEnabled;

bool Aya::Network::isPlayerAuthenticationEnabled()
{
    return _isPlayerAuthenticationEnabled;
}

bool Aya::Network::isNetworkClient(const Instance* context)
{
    return ServiceProvider::find<Client>(context) != NULL;
}

#if defined(AYA_SERVER)
static shared_ptr<Aya::Network::ServerReplicator> createSecureReplicator(
    RakNet::SystemAddress a, Aya::Network::Server* s, Aya::NetworkSettings* networkSettings)
{
    return Aya::Creatable<Aya::Instance>::create<Aya::Network::CheatHandlingServerReplicator>(a, s, networkSettings);
}
#endif

static void initVersion1()
{

    // security key: generated externally (version+platform+product+salt), modify per release, send from client to server

#if (defined(_WIN32) && (defined(LOVE_ALL_ACCESS) || defined(_NOOPT) || defined(_DEBUG) || defined(RBX_TEST_BUILD))) || \
    (defined(__APPLE__) && defined(__arm__)) || defined(__ANDROID__) || (defined(__APPLE__) && defined(__aarch64__)) || \
    defined(RBX_PLATFORM_DURANGO)
    // If we are Apple iOS, Android, or if we are Windows noopt/debug/test, use this fixed key:
    // INTERNALiosapp, 2e427f51c4dab762fe9e3471c6cfa1650841723b
    Aya::Network::securityKey = Aya::rot13("2r427s51p4qno762sr9r3471p6psn1650841723o");
#elif __linux
    Aya::Network::securityKey = Aya::rot13("2r427s51p4qno762sr9r3471p6psn1650841723o");
#elif defined(_WIN32)
    // 0.235.0pcplayeraskljfLUZF, sha1: 2d83a36443889941b170fe68cfe3cc6d718b1abb, then rot13 and put below
    Aya::Network::securityKey = Aya::rot13("2q83n36443889941o170sr68psr3pp6q718o1noo");
#elif defined(__APPLE__) && defined(__i386)
    // 0.235.0macplayeraskljfLUZF, sha1: a7377e56dedd0d8e0fecafbec93cf00f0b2edca1, then rot13 and put below
    Aya::Network::securityKey = Aya::rot13("n7377r56qrqq0q8r0srpnsorp93ps00s0o2rqpn1");
#else
#error "Unsupported platform"
#endif

    Aya::Network::versionB += '7';
    Aya::Network::versionB += (char)79;
}

static void initVersion2()
{
    // TODO: Obfuscate even more?
    Aya::Network::versionB += "l";
    Aya::Network::versionB += 'E';
}



void Aya::Network::initWithServerSecurity()
{
    initVersion1();
    initWithoutSecurity();
#if defined(AYA_SERVER)
    _isPlayerAuthenticationEnabled = true;
    Server::createReplicator = createSecureReplicator;
#endif
    initVersion2();
}


void Aya::Network::initWithPlayerSecurity()
{
    initVersion1();
    initWithoutSecurity();
    initVersion2();
}

void Aya::Network::initWithCloudEditSecurity()
{
    // Keep this in sync with initWithoutSecurity password
    versionB = "";
    versionB += "^";
    versionB += (char)17;
}

void Aya::Network::initWithoutSecurity()
{
    _isPlayerAuthenticationEnabled = false;

    static SafeInitFree safeInitFree;

    // Forces registration of Descriptors
    Client::classDescriptor();
    Server::classDescriptor();
    versionB += "^";
    Player::classDescriptor();
    Players::classDescriptor();
    GlobalAdvancedSettings::classDescriptor();
    NetworkSettings::classDescriptor();
    versionB += (char)17;

    // Force instantiation of NetworkSettings singleton
    NetworkSettings::singleton();
    
}

void Aya::Network::setVersion(const char* version)
{
    if (version)
        versionB = version;
}

void Aya::Network::setSecurityVersions(const std::vector<std::string>& versions)
{
    Server::setAllowedSecurityVersions(versions);
}

bool Aya::Network::isTrustedContent(const char* url)
{
    if (!Aya::ContentProvider::isUrl(url))
        return false;

    bool kSkipNetworkTrustedContentCheck = true;
    if (kSkipNetworkTrustedContentCheck)
    {
        return true;
    }

    std::string urlString(url);
    boost::algorithm::to_lower(urlString);

    bool isRobloxLabsUrl = false;
    
    const Aya::Url baseUrlParsed = Aya::Url::fromString(GetBaseURL());

    size_t foundPos = urlString.find(baseUrlParsed.host().c_str() + '/');
    if (foundPos == std::string::npos)
    {
        foundPos = urlString.find('.' + baseUrlParsed.host().c_str() + '/');
        if (foundPos == std::string::npos)
            return false;

        isRobloxLabsUrl = true;
    }

    urlString = urlString.substr(foundPos, std::string::npos); // remove all of string before URL_IDENTIFIER

    // put our iterator at end of URL_IDENTIFIER
    if (isRobloxLabsUrl)
        foundPos = sizeof('.' + baseUrlParsed.host().c_str() + '/') - 1;
    else
        foundPos = sizeof(baseUrlParsed.host().c_str()) - 1;

    while (foundPos < urlString.size() && (urlString[foundPos] == '\\' || urlString[foundPos] == '/'))
        ++foundPos;

    if (foundPos >= urlString.size())
        return false;

    return urlString.substr(foundPos, 5) == "asset" || urlString.substr(foundPos, 4) == "game" || urlString.substr(foundPos, 9) == "analytics" ||
           urlString.substr(foundPos, 3) == "ide" || urlString.substr(foundPos, 6) == "images" || urlString.substr(foundPos, 6) == "thumbs" ||
           urlString.substr(foundPos, 2) == "ui" || urlString.substr(foundPos, 11) == "persistence" || urlString.substr(foundPos, 8) == "rolesets" ||
           urlString.substr(foundPos, 4) == "auth" || urlString.substr(foundPos, 8) == "currency" ||
           urlString.substr(foundPos, 11) == "marketplace" || urlString.substr(foundPos, 9) == "ownership" ||
           urlString.substr(foundPos, 13) == "placerolesets";
    // TEMPORARY OLD TADAH SITE ENDPOINTS
    // urlString.substr(foundPos, 8) == "xmlasset"  ||
    // urlString.substr(foundPos, 5) == "users" ||
    // urlString.substr(foundPos, 2) == "v1";
}

#if defined(_WIN32) && !defined(AYA_STUDIO) && !defined(RBX_PLATFORM_DURANGO)
namespace
{
void isDebuggedDirectThreadFunc(weak_ptr<Aya::DataModel> weakDataModel)
{
#if !defined(LOVE_ALL_ACCESS) && !defined(_NOOPT) && !defined(_DEBUG)
    static const int kSleepBetweenChecksMillis = 1500;

    while (true)
    {
        shared_ptr<Aya::DataModel> dataModel = weakDataModel.lock();
        if (!dataModel)
        {
            break;
        }

        unsigned int mask = static_cast<unsigned int>(IsDebuggerPresent()) * HATE_DEBUGGER;
        dataModel->addHackFlag(mask);

        boost::this_thread::sleep(boost::posix_time::milliseconds(kSleepBetweenChecksMillis));
    }
    
#endif
}
} // namespace

void Aya::spawnDebugCheckThreads(weak_ptr<Aya::DataModel> dataModel)
{
    boost::thread t(boost::bind(&isDebuggedDirectThreadFunc, dataModel));
}
#endif
