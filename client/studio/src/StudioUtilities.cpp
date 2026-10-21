


#include "StudioUtilities.hpp"

// Qt Headers
#include <QNetworkInterface>
#include <QFile>
#include <QFileInfo>
#include <QDir>

// 3rd Party Headers
#include "boost/iostreams/copy.hpp"

// Roblox Headers
#include "Network/API.hpp"

#include "Script/script.hpp"
#include "Script/ScriptContext.hpp"
#include "Utility/FileSystem.hpp"

#include "Utility/Hash.hpp"

#include "Utility/RobloxGoogleAnalytics.hpp"

#include "Utility/RunStateOwner.hpp"

#include "Utility/Statistics.hpp"

#include "DataModel/Camera.hpp"

#include "DataModel/ContentProvider.hpp"

#include "DataModel/ChangeHistory.hpp"

#include "DataModel/DataModel.hpp"

#include "DataModel/PartInstance.hpp"

#include "DataModel/Tool.hpp"

#include "DataModel/Workspace.hpp"

#include "World/World.hpp"
#include "DataModel/Visit.hpp"

#include "Xml/Serializer.hpp"
#include "Xml/XmlSerializer.hpp"
#include "Reflection/Type.hpp"
#include "Client.hpp"
#include "ClientReplicator.hpp"
#include "Network/Players.hpp"

#include "Marker.hpp"

// Roblox Studio Headers
#include "QtUtilities.hpp"
#include "RobloxMainWindow.hpp"
#include "UpdateUIManager.hpp"
#include "MobileDevelopmentDeployer.hpp"
#include "RobloxUser.hpp"
#include "RobloxDocManager.hpp"
#include "RobloxIDEDoc.hpp"
#include "RobloxGameExplorer.hpp"
#include "RobloxSettings.hpp"

DYNAMIC_FASTFLAG(MaterialPropertiesEnabled)
DYNAMIC_FASTFLAG(CloudEditGARespectsThrottling)
FASTFLAGVARIABLE(TeamCreateOptimizeRemoteSelection, false)

static const char* kCloudEditScriptUrlPrefix = "CloudEditPlace:";

namespace StudioUtilities
{
static bool sIsVideoUploading = false;
static std::string sVideoFileName("");
static bool IsAvatarMode = false;
static bool IsTestMode = false;
static Aya::atomic<int> sNumScreenShotUploads;
static bool IsFirstTimeOpeningStudio = false;

bool isFirstTimeOpeningStudio()
{
    return IsFirstTimeOpeningStudio;
}

void setIsFirstTimeOpeningStudio(bool value)
{
    IsFirstTimeOpeningStudio = value;
}

bool isConnectedToNetwork()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if (iface.flags().testFlag(QNetworkInterface::IsUp) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {

#ifdef _DEBUG
            // details of connection
            qDebug() << "name:" << iface.name() << Qt::endl << "ip addresses:" << Qt::endl << "mac:" << iface.hardwareAddress() << Qt::endl;
#endif

            // this loop is important
            for (int j = 0; j < iface.addressEntries().count(); j++)
            {
#ifdef _DEBUG
                qDebug() << iface.addressEntries().at(j).ip().toString() << " / " << iface.addressEntries().at(j).netmask().toString() << Qt::endl;
#endif

                // we have an interface that is up, and has an ip address
                // therefore the link is present

                // we will only enable this check on first positive,
                // all later results are incorrect
                if (result == false)
                    result = true;
            }
        }
    }
    return result;
}

bool isAvatarMode()
{
    return IsAvatarMode;
}

void setAvatarMode(bool isAvatarMode)
{
    IsAvatarMode = isAvatarMode;
}

bool isTestMode()
{
    return IsTestMode;
}

void setTestMode(bool isTestMode)
{
    IsTestMode = isTestMode;
}

void setScreenShotUploading(bool state)
{
    state ? ++sNumScreenShotUploads : --sNumScreenShotUploads;
}

bool isScreenShotUploading()
{
    return sNumScreenShotUploads > 0;
}

void setVideoUploading(bool state)
{
    sIsVideoUploading = state;
}

bool isVideoUploading()
{
    return sIsVideoUploading;
}

void setVideoFileName(const std::string& fileName)
{
    sVideoFileName = fileName;
}

std::string getVideoFileName()
{
    return sVideoFileName;
}

bool containsEditScript(const QString& url)
{
    return url.contains("edit.ashx", Qt::CaseInsensitive);
}

bool containsJoinScript(const QString& url)
{
    return url.contains("join.ashx", Qt::CaseInsensitive);
}

bool containsVisitScript(const QString& url)
{
    return url.contains("visit.ashx", Qt::CaseInsensitive);
}

bool containsGameServerScript(const QString& url)
{
    return url.contains("gameserver.ashx", Qt::CaseInsensitive);
}

std::string authenticate(std::string& domain, std::string& url, std::string& ticket)
{
    if (domain.empty() || url.empty() || ticket.empty())
        return "";

    try
    {
        // Post our ticket back to Roblox to suggest a re-authentication
        std::string result;
        {
            std::string compound = url;
            compound += "?suggest=";
            compound += ticket;

            Aya::Http http(compound.c_str());
#ifdef Q_WS_MAC
            http.setAuthDomain(::GetBaseURL());
#else
            http.additionalHeaders["RBXAuthenticationNegotiation:"] = ::GetBaseURL();
#endif

            // http.additionalHeaders["RBXAuthenticationNegotiation:"] = domain;
            http.get(result);
        }

        // The http content is the new ticket
        return result;
    }
    catch (std::exception& e)
    {
        // Report Error!
        Aya::StandardOut::singleton()->print(Aya::MESSAGE_ERROR, e.what());
        return "";
    }
}

Aya::ProtectedString fetchAndValidateScript(Aya::DataModel* pDataModel, const std::string& urlScript)
{
    if (!pDataModel)
        return Aya::ProtectedString();

    Aya::Security::Impersonator impersonate(Aya::Security::COM);

    std::ostringstream data;
    if (!Aya::ContentProvider::isUrl(urlScript))
        return Aya::ProtectedString();

    std::string dataString;
    {
        try
        {
            Aya::DataModel::LegacyLock lock(pDataModel, Aya::DataModelJob::Write);
            std::auto_ptr<std::istream> stream(
                Aya::ServiceProvider::create<Aya::ContentProvider>(pDataModel)->getContent(Aya::ContentId(urlScript.c_str())));
            dataString = std::string(static_cast<std::stringstream const&>(std::stringstream() << stream->rdbuf()).str());
        }

        catch (std::exception& e)
        {
            Aya::StandardOut::singleton()->printf(Aya::MESSAGE_ERROR, "ContentProvider while executing url script failed because %s", e.what());
        }
    }

    Aya::ProtectedString verifiedSource;

    try
    {
        verifiedSource = Aya::ProtectedString::fromTrustedSource(dataString);
        Aya::ContentProvider::verifyScriptSignature(verifiedSource, true);
    }
    catch (std::bad_alloc&)
    {
        throw;
    }
    catch (std::exception& e)
    {
        Aya::StandardOut::singleton()->printf(Aya::MESSAGE_ERROR, "Script signature verification failed -- %s", e.what());
        return Aya::ProtectedString();
    }

    return verifiedSource;
}

void executeURLJoinScript(boost::shared_ptr<Aya::Game> pGame, std::string urlScript)
{
    RBXASSERT(containsJoinScript(QString::fromStdString(urlScript)));

    boost::shared_ptr<Aya::DataModel> pDataModel = pGame->getDataModel();
    if (!pDataModel)
        return;

    Aya::ProtectedString verifiedSource = fetchAndValidateScript(pDataModel.get(), urlScript);
    if (verifiedSource.empty())
        return;

    try
    {
        Aya::DataModel::LegacyLock lock(pDataModel, Aya::DataModelJob::Write);
        if (pDataModel->isClosed())
            return;

        std::string dataString = verifiedSource.getSource();
        int firstNewLineIndex = dataString.find("\r\n");
        if (dataString[firstNewLineIndex + 2] == '{')
        {
            pGame->configurePlayer(Aya::Security::COM, dataString.substr(firstNewLineIndex + 2));
            if (RobloxIDEDoc* ideDoc = RobloxDocManager::Instance().getPlayDoc())
            {
                QMetaObject::invokeMethod(ideDoc, "refreshDisplayName", Qt::QueuedConnection);
            }

            return;
        }

        boost::shared_ptr<Aya::ScriptContext> pContext = Aya::shared_from(pDataModel->create<Aya::ScriptContext>());
        if (pContext)
            pContext->executeInNewThread(Aya::Security::COM, verifiedSource, "Start Game");
    }
    catch (std::exception& exp)
    {
        std::string s(verifiedSource.getSource());
        std::string msg = Aya::format("executeURLJoinScript() failed because %s.  %s", exp.what(), s.substr(0, 64).c_str());
        Aya::StandardOut::singleton()->print(Aya::MESSAGE_ERROR, msg.c_str());
    }
}

void executeURLScript(boost::shared_ptr<Aya::DataModel> pDataModel, std::string urlScript)
{
    if (!pDataModel)
        return;

    Aya::ProtectedString verifiedSource = fetchAndValidateScript(pDataModel.get(), urlScript);
    if (verifiedSource.empty())
        return;

    try
    {
        Aya::DataModel::LegacyLock lock(pDataModel, Aya::DataModelJob::Write);
        if (pDataModel->isClosed())
            return;

        boost::shared_ptr<Aya::ScriptContext> pContext = Aya::shared_from(pDataModel->create<Aya::ScriptContext>());
        if (pContext)
            pContext->executeInNewThread(Aya::Security::COM, verifiedSource, "Start Game");
    }
    catch (std::exception& exp)
    {
        std::string s(verifiedSource.getSource());
        std::string msg = Aya::format("executeURLScript() failed because %s.  %s", exp.what(), s.substr(0, 64).c_str());
        Aya::StandardOut::singleton()->print(Aya::MESSAGE_ERROR, msg.c_str());
    }
}

void reportCloudEditJoinEvent(const char* label)
{
    if (DFFlag::CloudEditGARespectsThrottling)
    {
        Aya::RobloxGoogleAnalytics::trackEvent(GA_CATEGORY_STUDIO, "CloudEdit", label);
    }
    else
    {
        Aya::RobloxGoogleAnalytics::trackEventWithoutThrottling(GA_CATEGORY_STUDIO, "CloudEdit", label);
    }
}

static void notifyCloudEditConnectionClosed(const char* category, std::string message)
{
    std::string label = Aya::format("Connection Closed: %s | %s", category, message.c_str());
    reportCloudEditJoinEvent(label.c_str());
    QMetaObject::invokeMethod(&(UpdateUIManager::Instance().getMainWindow()), "notifyCloudEditConnectionClosed", Qt::QueuedConnection);
}

static void updateCameraLocation(Aya::DataModel* dm)
{
    if (Aya::Network::Player* p = Aya::Network::Players::findLocalPlayer(dm))
    {
        p->setCloudEditCameraCoordinateFrame(dm->getWorkspace()->getCamera()->getCameraCoordinateFrame());
    }
}

static void doUpdateSelection(shared_ptr<bool> debouncer, Aya::DataModel* dm)
{
    if (Aya::Network::Player* p = Aya::Network::Players::findLocalPlayer(dm))
    {
        if (Aya::Selection* s = dm->find<Aya::Selection>())
        {
            shared_ptr<Aya::Reflection::ValueArray> va(new Aya::Reflection::ValueArray);
            for (auto& i : *s)
            {
                va->push_back(i);
            }
            Aya::Network::Player::event_cloudEditSelectionChanged.replicateEvent(p, shared_ptr<const Aya::Reflection::ValueArray>(va));
        }
    }
    if (FFlag::TeamCreateOptimizeRemoteSelection)
    {
        *debouncer = false;
    }
}

static void updateSelection(shared_ptr<bool> debouncer, Aya::DataModel* dm)
{
    if (FFlag::TeamCreateOptimizeRemoteSelection)
    {
        if (*debouncer)
            return;
        *debouncer = true;
        dm->submitTask(boost::bind(&doUpdateSelection, debouncer, _1), Aya::DataModelJob::Write);
    }
    else
    {
        doUpdateSelection(debouncer, dm);
    }
}

static void onConnectionAccepted(int placeId, std::string url, shared_ptr<Aya::Instance> replicator)
{
    using namespace Aya;
    reportCloudEditJoinEvent("Connection Accepted");
    try
    {
        boost::shared_ptr<Network::ClientReplicator> rep = Instance::fastSharedDynamicCast<Network::ClientReplicator>(replicator);
        rep->disconnectionSignal.connect(boost::bind(&notifyCloudEditConnectionClosed, "ReplicatorDisconnect", _1));
    }
    catch (Aya::base_exception& e)
    {
        StandardOut::singleton()->printf(MESSAGE_ERROR, "onConnectionAccepted failed: %s", e.what());
    }
}

void joinCloudEditSession(shared_ptr<Aya::Game> pGame, shared_ptr<EntityProperties> config)
{
    using namespace Aya;
    try
    {
        boost::shared_ptr<DataModel> dataModel = pGame->getDataModel();
        if (!dataModel)
            return;

        DataModel::LegacyLock lock(dataModel, DataModelJob::Write);
        if (dataModel->isClosed())
            return;

        reportCloudEditJoinEvent("Attempt To Connect");

        Security::Impersonator impersonate(Security::COM);

        Http::gameID = config->get<std::string>("GameId").get();
        int userId = RobloxUser::singleton().getUserId();
        Aya::Analytics::setUserId(userId);

        int placeId = config->get<int>("PlaceId").get();
        dataModel->setPlaceID(placeId, false);
        dataModel->setUniverseId(config->get<int>("UniverseId").get());
        ScriptContext::propScriptsDisabled.setValue(dataModel->create<ScriptContext>(), true);

        dataModel->create<ContentProvider>()->setThreadPool(16);
        Network::Players* players = dataModel->create<Network::Players>();
        // creating a visit helps suppress saving behaviors that don't work well in CloudEdit
        dataModel->create<Visit>();
        if (RobloxIDEDoc* ideDoc = RobloxDocManager::Instance().getPlayDoc())
        {
            ideDoc->enableUndo(true);
            QMetaObject::invokeMethod(ideDoc, "updateFromPlaceID", Qt::QueuedConnection, Q_ARG(int, placeId));
            QMetaObject::invokeMethod(&UpdateUIManager::Instance().getViewWidget<RobloxGameExplorer>(eDW_GAME_EXPLORER), "openGameFromPlaceId",
                Qt::QueuedConnection, Q_ARG(int, placeId));
        }

        Network::Client* client = dataModel->create<Network::Client>();
        client->configureAsCloudEditClient();
        client->connectionAcceptedSignal.connect(boost::bind(&onConnectionAccepted, placeId, _1, _2));
        client->connectionRejectedSignal.connect(boost::bind(&notifyCloudEditConnectionClosed, "Rejected", _1));
        client->connectionFailedSignal.connect(boost::bind(&notifyCloudEditConnectionClosed, "Failed", _3));
        client->setTicket(config->get<std::string>("ClientTicket").get());

        client->setGameSessionID(config->get<std::string>("SessionId").get());

        shared_ptr<Network::Player> player = Instance::fastSharedDynamicCast<Network::Player>(
            client->playerConnect(userId, config->get<std::string>("MachineAddress").get(), config->get<int>("ServerPort").get(), 0, -1));
        player->setName(config->get<std::string>("UserName").get());
        player->setCharacterAppearance(config->get<std::string>("CharacterAppearance").get());
        if (RunService* rs = dataModel->create<RunService>())
        {
            rs->heartbeatSignal.connect(boost::bind(&updateCameraLocation, dataModel.get()));
        }
        shared_ptr<bool> selectionUpdateDebouncer(new bool(false));
        dataModel->create<Selection>()->selectionChanged.connect(boost::bind(&updateSelection, selectionUpdateDebouncer, dataModel.get()));

        // creating a client will reset the mouse command to null, set it back to adv arrow here
        dataModel->getWorkspace()->setDefaultMouseCommand();
    }
    catch (const std::exception& e)
    {
        StandardOut::singleton()->printf(MESSAGE_ERROR, "Error while joining CloudEdit: %s", e.what());
        QtUtilities::RBXMessageBox failureBox;
        failureBox.setText("Exception while trying to join CloudEdit. See Output for details.");
        failureBox.exec();
    }
}

void startMobileDevelopmentDeployer()
{
    MobileDevelopmentDeployer::singleton().startPlaySessionWithRbxDevDevices();
}

void stopMobileDevelopmentDeployer()
{
    MobileDevelopmentDeployer::singleton().disconnectClientsAndShutDown();
}

void convertPhysicalPropertiesIfNeeded(std::vector<boost::shared_ptr<Aya::Instance>> instances, Aya::Workspace* workspace)
{
    if (workspace->getUsingNewPhysicalProperties())
    {
        for (size_t i = 0; i < instances.size(); ++i)
        {
            Aya::PartInstance::convertToNewPhysicalPropRecursive(instances[i].get());
        }
    }
    else
    {
        for (size_t i = 0; i < instances.size(); i++)
        {
            if (Aya::PartInstance::instanceOrChildrenContainsNewCustomPhysics(instances[i].get()))
            {
                if (RobloxIDEDoc::displayAskConvertPlaceToNewMaterialsIfInsertNewModel())
                {
                    Aya::RobloxGoogleAnalytics::trackEvent(
                        GA_CATEGORY_ACTION, "PhysicalProperties: Insert - Conversion", "Workspace->PhysicalPropertiesMode_New");
                    workspace->setPhysicalPropertiesMode(Aya::PhysicalPropertiesMode_NewPartProperties);
                    Aya::PartInstance::convertToNewPhysicalPropRecursive(Aya::DataModel::get(workspace));
                }
                break;
            }
        }
    }
}

void insertModel(boost::shared_ptr<Aya::DataModel> pDataModel, QString fileName, bool insertInto)
{
    if (fileName.isEmpty())
        return;

    if (!pDataModel)
        throw std::runtime_error("Can't insert at this time");

    QByteArray ba = fileName.toUtf8();
    const char* c_str = ba.constData();

    std::ifstream stream(c_str, std::ios_base::in | std::ios_base::binary);

    // The following code happens BEFORE we do a lock because we can't display a message box within a lock
    Aya::Instances instances;
    Serializer().loadInstances(stream, instances);

    {
        // Lock the data model temporarily to make sure insertion is legal
        Aya::DataModel::LegacyLock lock(pDataModel, Aya::DataModelJob::Write);

        if (DFFlag::MaterialPropertiesEnabled)
        {
            convertPhysicalPropertiesIfNeeded(instances, pDataModel->getWorkspace());
        }

        if (Aya::ContentProvider* cp = pDataModel->create<Aya::ContentProvider>())
        {
            Aya::LuaSourceContainer::blockingLoadLinkedScriptsForInstances(cp, instances);
        }
    }

    Aya::PromptMode promptMode = Aya::SUPPRESS_PROMPTS;

    if (!insertInto)
    {
        if (instances.size() == 1)
        {
            Aya::Instance* single = instances[0].get();
            if (dynamic_cast<Aya::Tool*>(single))
            {
                if (QtUtilities::RBXMessageBox::question(&UpdateUIManager::Instance().getMainWindow(), "Insert Model",
                        "Put this tool into the starter pack (otherwise drop into the 3d view)?", QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::No) == QMessageBox::Yes)
                    promptMode = Aya::PUT_TOOL_IN_STARTERPACK;
            }
        }
    }

    // Now lock the datamodel (after MessageBox)
    Aya::DataModel::LegacyLock lock(pDataModel, Aya::DataModelJob::Write);

    shared_ptr<Aya::Workspace> spWorkspace = shared_from(pDataModel ? pDataModel->getWorkspace() : NULL);
    if (!spWorkspace)
        throw std::runtime_error("Can't insert at this time");

    Aya::Selection* sel = Aya::ServiceProvider::create<Aya::Selection>(spWorkspace.get());

    Aya::Instance* requestedParent = (insertInto && (sel->size() == 1)) ? sel->front().get() : spWorkspace.get();

    Aya::InsertMode insertMode = insertInto ? Aya::INSERT_TO_TREE : Aya::INSERT_TO_3D_VIEW;

    try
    {
        spWorkspace->insertInstances(instances, requestedParent, insertMode, promptMode);

        Aya::RobloxGoogleAnalytics::trackEvent(GA_CATEGORY_ACTION, insertInto ? "insertIntoFromFileComplete" : "insertModelComplete");

        boost::shared_ptr<Aya::RunService> runService = shared_from(pDataModel->create<Aya::RunService>());
        // If we're not playing a game then select the content
        if (runService->getRunState() == Aya::RS_STOPPED)
        {
            sel->setSelection(instances.begin(), instances.end());
        }
    }
    catch (std::exception&)
    {
        std::for_each(instances.begin(), instances.end(), boost::bind(&Aya::Instance::setParent, _1, (Aya::Instance*)NULL));
        throw;
    }
}

void insertScript(boost::shared_ptr<Aya::DataModel> pDataModel, const QString& fileName)
{
    shared_ptr<Aya::Workspace> spWorkspace = shared_from(pDataModel ? pDataModel->getWorkspace() : NULL);
    if (!pDataModel || !spWorkspace)
        throw std::runtime_error("Script cannot be inserted at this time");

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        throw std::runtime_error("Script file cannot be opened");

    Aya::DataModel::LegacyLock lock(pDataModel, Aya::DataModelJob::Write);

    shared_ptr<Aya::Script> pScriptInstance = Aya::Creatable<Aya::Instance>::create<Aya::Script>();
    if (!pScriptInstance)
        throw std::runtime_error("Script instance cannot be created");

    // set file name as script instance name
    pScriptInstance->setName(QFileInfo(fileName).completeBaseName().toStdString());

    // read and set code from file into script instance
    QTextStream in(&file);
    pScriptInstance->setEmbeddedCode(Aya::ProtectedString::fromTrustedSource(in.readAll().toUtf8().data()));

    // set appropriate parent
    Aya::Selection* pSelection = Aya::ServiceProvider::create<Aya::Selection>(spWorkspace.get());
    Aya::Instance* pRequestedParent = (pSelection && pSelection->size() == 1) ? pSelection->front().get() : spWorkspace.get();
    pScriptInstance->setParent(pRequestedParent);

    // select the created script instance
    if (pSelection)
        pSelection->setSelection(pScriptInstance.get());
}

QString getDebugInfoFile(const QString& fileName, const QString& debuggerFileExt)
{
    QString debugInfoFile;
    if (!fileName.isEmpty())
    {
        static QString filePath = QtUtilities::qstringFromStdString(Aya::FileSystem::getUserDirectory(true, Aya::DirAppData, "debugger").native());
        // get hash of the file path
        unsigned int hash = Aya::Hash::hash(QFileInfo(fileName).absoluteFilePath().toStdString());
        // create debug file path with generated hash
        debugInfoFile = filePath;
        debugInfoFile.append(QString::number(hash));
        if (!debuggerFileExt.isEmpty())
            debugInfoFile.append(debuggerFileExt);
        debugInfoFile.append(".rdbg");
    }
    return debugInfoFile;
}

bool checkNetworkAndUserAuthentication(bool openStartPage)
{
    if (!StudioUtilities::isConnectedToNetwork())
    {
        // we don't have network connect
        UpdateUIManager::Instance().showErrorMessage(QObject::tr("Publish to Roblox"),
            QObject::tr("Feature not available in offline mode.\nPlease check internet connection and restart Roblox."));
        return false;
    }

    if (RobloxUser::singleton().getUserId() <= 0)
    {
        UpdateUIManager::Instance().showErrorMessage(QObject::tr("Log in required"), QObject::tr("You must log in to perform this action!"));
        if (openStartPage)
        {
            UpdateUIManager::Instance().getMainWindow().openStartPage(true, "showlogin=True");
            UpdateUIManager::Instance().getMainWindow().setFocus();
        }
        return false;
    }

    return true;
}

int translateKeyModifiers(Qt::KeyboardModifiers state, const QString& text)
{
    // Convert modifier combos to a single integer.
    int result = 0;
    if ((state & Qt::ShiftModifier) && (text.size() == 0 || !text.at(0).isPrint() || text.at(0).isLetter() || text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}
} // namespace StudioUtilities
