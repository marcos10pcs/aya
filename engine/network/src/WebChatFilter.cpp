#include "Network/WebChatFilter.hpp"


#include <algorithm>

#include "RobloxServicesTools.hpp"

#include "aya/rbxTime.hpp"

#include "DataModel/Stats.hpp"

#include "DataModel/DataModel.hpp"

#include "Utility/Guid.hpp"

#include "Utility/Http.hpp"

#include "Utility/HttpAsync.hpp"

#include "Utility/standardout.hpp"

#include "Reflection/Type.hpp"
#include "Utility/Statistics.hpp"

#include "Utility/RobloxGoogleAnalytics.hpp"


#include <boost/lexical_cast.hpp>
#include <boost/thread/once.hpp>
#include <rapidjson/document.h>
#include <string>

DYNAMIC_LOGVARIABLE(WebChatFiltering, 0)

DYNAMIC_FASTINTVARIABLE(WebChatFilterHttpTimeoutSeconds, 60)
DYNAMIC_FASTFLAGVARIABLE(ConstructModerationFilterTextParamsAndHeadersUseLegacyFilterParams, true)

using namespace Aya;
using namespace Aya::Network;

namespace
{
static Timer<Aya::Time::Precise> lastFailureTime;

void initFailureTimer()
{
    lastFailureTime.reset();
}

inline void logContent(const std::string& varName, const std::string& msg)
{
    if (DFLog::WebChatFiltering)
    {
        std::stringstream ss;
        ss << varName << " (" << msg.length() << " bytes) " << msg;
        FASTLOGS(DFLog::WebChatFiltering, "%s", ss.str().c_str());
    }
}

inline void logResponseData(const std::string& unfiltered, const ChatFilter::Result& result, const Aya::Time::Interval& webChatFilterResponseDelta)
{
    FASTLOG1F(DFLog::WebChatFiltering, "ResponseLatencyMillis: %f", webChatFilterResponseDelta.msec());
    logContent("FilteredContent (blacklist)", result.blacklistFilteredMessage);
    logContent("FilteredContent (whitelist)", result.whitelistFilteredMessage);
}
} // namespace

void filterMessageHelper(const std::string message, const Aya::Network::ChatFilter::FilteredChatMessageCallback filteredCallback,
    shared_ptr<Aya::Network::Player> playerFilter)
{
    logContent("Unfiltered", message);

    {
        static boost::once_flag flag = BOOST_ONCE_INIT;
        boost::call_once(&initFailureTimer, flag);
    }

    std::stringstream params;
    Aya::HttpAux::AdditionalHeaders headers;    

    Aya::Timer<Aya::Time::Precise> timer;

    std::string response;
    Time::Interval responseDelta = timer.delta();
    std::string failureReason;

    ChatFilter::Result result;

    // I hope whoever looks at this code doesnt make fun of me when we open source aya
    // not sure why the hell i have to do this, params << is not woking, stringstreams are busted?
    // Aya::StandardOut::singleton()->printf(MESSAGE_INFO, "webchatfilter: %s", (GetWebChatFilterURL(GetBaseURL()).erase(GetWebChatFilterURL(GetBaseURL()).length()-9) + "?text=" + Http::urlEncode(message)).c_str());
    // Aya::StandardOut::singleton()->printf(MESSAGE_INFO, "sayer: %s, userId %d", playerFilter->getName().c_str(), playerFilter->getUserID());
    
    try
    {
        Http http((
            GetWebChatFilterURL(GetBaseURL()).erase(GetWebChatFilterURL(GetBaseURL()).length()-9) + "?text=" + Http::urlEncode(message) + "&sender=" + playerFilter->getName() + "&senderId=" + std::to_string(playerFilter->getUserID())
        ));
        http.setResponseTimeout(DFInt::WebChatFilterHttpTimeoutSeconds * 1000);
        
        {
            params << "filters=" << kWebChatWhiteListPolicy;
            params << "&filters=" << kWebChatBlackListPolicy;
            params << "&text=" << Http::urlEncode(message);
        }
        
        http.post(params, Http::kContentTypeUrlEncoded, false, response);

        using namespace rapidjson;
        Document doc;
        doc.Parse<kParseDefaultFlags>(response.c_str());

        RBXASSERT(doc.HasMember("data"));
        Value& data = doc["data"];
        RBXASSERT(data.HasMember(kWebChatWhiteListPolicy.c_str()));
        RBXASSERT(data.HasMember(kWebChatBlackListPolicy.c_str()));

        result.whitelistFilteredMessage = data[kWebChatWhiteListPolicy.c_str()].GetString();
        result.blacklistFilteredMessage = data[kWebChatBlackListPolicy.c_str()].GetString();

        filteredCallback(result);
    }
    catch (const std::exception& e)
    {
        FASTLOGS(DFLog::WebChatFiltering, "Web chat failed: %s", e.what());
        Aya::StandardOut::singleton()->printf(MESSAGE_INFO, "Web chat failed: %s", e.what());
        Aya::StandardOut::singleton()->printf(MESSAGE_INFO, "params: %s", params.str().c_str());
        failureReason = e.what();
        RobloxGoogleAnalytics::trackEvent(GA_CATEGORY_GAME, "ChatFailure", failureReason.c_str());
    }

    logResponseData(message, result, responseDelta);
}

void WebChatFilter::filterMessage(shared_ptr<Aya::Network::Player> sender, shared_ptr<Aya::Instance> receiver, const std::string& message,
    const Aya::Network::ChatFilter::FilteredChatMessageCallback filteredCallback)
{
    if (filterMessageBase(sender, receiver, message, filteredCallback))
    {
        return;
    }

    RBXASSERT(sender);
    boost::function0<void> f = boost::bind(&filterMessageHelper, message, filteredCallback, sender);
    boost::function0<void> g = boost::bind(&StandardOut::print_exception, f, Aya::MESSAGE_ERROR, false);
    boost::thread(thread_wrapper(g, "rbx_webChatFilterHttpPost"));
}