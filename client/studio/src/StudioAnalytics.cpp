
#include "StudioAnalytics.hpp"
#include "LuaSourceBuffer.hpp"

#include "Utility/Utilities.hpp"

#include "Utility/RobloxGoogleAnalytics.hpp"


#include "DataModel/Remote.hpp"

#include "DataModel/PartInstance.hpp"

#include "DataModel/DataModel.hpp"


#include "RobloxUser.hpp"


LOGVARIABLE(StudioAnalytics, 1);


PersistentVariable::PersistentVariable(const char* name, StudioAnalytics* owner)
    : name(name)
    , owner(owner)
    , accumulatedValue(0)
{
    owner->registerPersistentVariable(this);
}

StudioAnalytics::StudioAnalytics() {}

void StudioAnalytics::registerPersistentVariable(PersistentVariable* variable)
{
    persistentVariables.push_back(variable);
}

void StudioAnalytics::savePersistentVariables()
{
    for (std::vector<PersistentVariable*>::iterator it = persistentVariables.begin(); it != persistentVariables.end(); ++it)
    {
        const char* name = (*it)->getName();
        int value = storage.value(name).toInt() + (*it)->getAccumulatedValue();

        storage.setValue(name, value);

        FASTLOGS(FLog::StudioAnalytics, "Saving variable %s", name);
        FASTLOG1(FLog::StudioAnalytics, "Value: %i", value);

        (*it)->clearAccumulatedValue();
    }
}

void StudioAnalytics::reportPersistentVariables()
{
    std::string robloxUserId = Aya::StringConverter<int>::convertToString(RobloxUser::singleton().getUserId());

    for (std::vector<PersistentVariable*>::iterator it = persistentVariables.begin(); it != persistentVariables.end(); ++it)
    {
        const char* name = (*it)->getName();
        int value = storage.value(name).toInt() + (*it)->getAccumulatedValue();
        FASTLOGS(FLog::StudioAnalytics, "Reporting variable %s", name);
        FASTLOG1(FLog::StudioAnalytics, "Value: %i", value);
        Aya::RobloxGoogleAnalytics::trackEvent("UsageFeatures", name, robloxUserId.c_str(), value);
    }
}

void StudioAnalytics::reportEvent(const char* name, int value /* = 0 */, const char* label /* = "" */, const char* category /* = "UsageFeatures" */)
{
    Aya::RobloxGoogleAnalytics::trackEvent(category, name, label, value);
}

struct PublishFeatures
{
    bool hasDataStoreService;
    bool hasTeleportService;
    bool hasMarketPlaceService;
    bool hasRemoteFunctions;
    int partCount;

    PublishFeatures()
    {
        memset(this, 0, sizeof(PublishFeatures));
    }
};

void checkScriptCode(const std::string& code, const char* substring, bool* target)
{
    if (!*target && code.find(substring) != std::string::npos)
        *target = true;
}

void fillPublishFeatures(shared_ptr<Aya::Instance> instance, PublishFeatures* publishFeatures)
{
    shared_ptr<Aya::Script> script = Aya::Instance::fastSharedDynamicCast<Aya::Script>(instance);
    if (Aya::Instance::fastDynamicCast<Aya::Script>(instance.get()) || Aya::Instance::fastDynamicCast<Aya::ModuleScript>(instance.get()))
    {
        std::string code;
        try
        {
            code = LuaSourceBuffer::fromInstance(instance).getScriptText();
        }
        catch (const Aya::base_exception&)
        {
            // if the script fails to load, treat it as empty for these signals
            code = "";
        }
        checkScriptCode(code, "DataStoreService", &publishFeatures->hasDataStoreService);
        checkScriptCode(code, "TeleportService", &publishFeatures->hasTeleportService);
        checkScriptCode(code, "MarketPlaceService", &publishFeatures->hasMarketPlaceService);
    }

    if (!publishFeatures->hasRemoteFunctions)
    {
        shared_ptr<Aya::RemoteFunction> remoteFunction = Aya::Instance::fastSharedDynamicCast<Aya::RemoteFunction>(instance);
        shared_ptr<Aya::RemoteEvent> remoteEvent = Aya::Instance::fastSharedDynamicCast<Aya::RemoteEvent>(instance);

        if (remoteFunction || remoteEvent)
            publishFeatures->hasRemoteFunctions = true;
    }

    shared_ptr<Aya::PartInstance> partInstance = Aya::Instance::fastSharedDynamicCast<Aya::PartInstance>(instance);
    if (partInstance)
        publishFeatures->partCount++;
}

void StudioAnalytics::reportPublishStats(Aya::DataModel* dm)
{
    PublishFeatures features;
    dm->visitDescendants(boost::bind(&fillPublishFeatures, _1, &features));

    std::stringstream featuresText;

    if (features.hasDataStoreService)
        featuresText << "DataStoreService ";
    if (features.hasTeleportService)
        featuresText << "TeleportsService ";
    if (features.hasMarketPlaceService)
        featuresText << "MarketPlaceService ";
    if (features.hasRemoteFunctions)
        featuresText << "RemoteFunctions ";

    if (featuresText.rdbuf()->in_avail() == 0)
        featuresText << "None";

    std::string placeId = Aya::StringConverter<int>::convertToString(dm->getPlaceID());
    reportEvent(featuresText.str().c_str(), 0, placeId.c_str(), "PublishFeatures");

    reportEvent("PublishPartCount", features.partCount, placeId.c_str());
    FASTLOG5(FLog::StudioAnalytics, "Report on publish, part count: %u, DataStore: %i, Teleports: %i, MarketPlace: %i, Remote: %i",
        features.partCount, features.hasDataStoreService, features.hasTeleportService, features.hasMarketPlaceService, features.hasRemoteFunctions);
}