#undef min
#undef max


#include "aya/Log.hpp"

#include "DumpErrorUploader.hpp"
#include "LogManager.hpp"
#include "aya/rbxTime.hpp"

#include "Utility/Http.hpp"
#include "Utility/standardout.hpp"
#include "Utility/MemoryStats.hpp"
#include "DataModel/Stats.hpp"

namespace io = boost::iostreams;

boost::scoped_ptr<Aya::Http> DumpErrorUploader::crashEventRequest;
std::istringstream DumpErrorUploader::crashEventData("Crash happened!");
std::string DumpErrorUploader::crashEventResponse;
std::string DumpErrorUploader::crashCounterNamePrefix;

DYNAMIC_FASTINTVARIABLE(RCCInfluxHundredthsPercentage, 1000)
DYNAMIC_FASTFLAGVARIABLE(ExtendedCrashInfluxReporting, false)

DumpErrorUploader::DumpErrorUploader(bool backgroundUpload, const std::string& crashCounterNamePrefix)
{
    if (backgroundUpload)
        thread.reset(new Aya::worker_thread(boost::bind(&DumpErrorUploader::run, _data), "ErrorUploader"));

    this->crashCounterNamePrefix = crashCounterNamePrefix;
}

void DumpErrorUploader::InitCrashEvent(const std::string& url, const std::string& crashEventFileName)
{
    // Setup a HTTP object for crashEvent
    if (!crashEventRequest)
    {
        std::string finalUrl = url + "?filename=" + Aya::Http::urlEncode(crashEventFileName);
        Aya::Log::current()->writeEntry(Aya::Log::Information, Aya::format("Initializing CrashEvent request, url: %s", finalUrl.c_str()).c_str());
        crashEventRequest.reset(new Aya::Http(finalUrl));
        crashEventResponse.reserve(MAX_PATH);
    }
}


void DumpErrorUploader::Upload(const std::string& url)
{
#ifdef WIN32
    HANDLE hMutex;

    hMutex = CreateMutex(NULL,                 // default security descriptor
        TRUE,                                  // own the mutex
        TEXT("RobloxCrashDumpUploaderMutex")); // object name

    if (hMutex == NULL)
    {
        Aya::StandardOut::singleton()->printf(Aya::MESSAGE_ERROR, "CreateMutex error: %d\n", GetLastError());
    }
    else
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            Aya::StandardOut::singleton()->printf(Aya::MESSAGE_INFO, "RobloxCrashDumpUploaderMutex already exists. Not uploading logs.");
            return;
        }
    }

    if (thread)
    {
        thread->wake();
    }
    else
    {
        while (run(_data) != Aya::worker_thread::done)
        {
        }
    }
#endif
}

int LogFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    Aya::StandardOut::singleton()->printf(Aya::MESSAGE_ERROR, "Exception code: %u", code);

    return EXCEPTION_EXECUTE_HANDLER;
}

void DumpErrorUploader::UploadCrashEventFile(struct _EXCEPTION_POINTERS* excInfo)
{
#ifndef AYA_STUDIO
    try
    {
        if (crashEventRequest)
        {
            Aya::StandardOut::singleton()->printf(Aya::MESSAGE_INFO, "Crash Event post");
            crashEventRequest->post(crashEventData, Aya::Http::kContentTypeDefaultUnspecified, true, crashEventResponse);
            Aya::StandardOut::singleton()->printf(Aya::MESSAGE_INFO, "Crash Event posted, response: %s", crashEventResponse.c_str());

            Aya::Analytics::EphemeralCounter::reportCounter(crashCounterNamePrefix + "CrashEvent", 1, true);

            Aya::Analytics::InfluxDb::Points points;
            if (DFFlag::ExtendedCrashInfluxReporting)
            {
                points.addPoint("SessionReport", "AppStatusCrash");
                points.addPoint("PlayTime", Aya::Time::nowFastSec());
                points.addPoint("UsedMemoryKB", Aya::MemoryStats::usedMemoryBytes());

                MainLogManager::GameState gamestate = MainLogManager::getMainLogManager()->getGameState();
                std::string gameStateString =
                    gamestate == MainLogManager::UN_INITIALIZED ? "uninitialized" : (gamestate == MainLogManager::IN_GAME ? "inGame" : "leaveGame");
                points.addPoint("GameState", gameStateString.c_str());

                if (excInfo)
                {
                    points.addPoint("ExceptionCode", (uint32_t)excInfo->ExceptionRecord->ExceptionCode);
                    std::stringstream excepAddress;
                    excepAddress << excInfo->ExceptionRecord->ExceptionAddress;
                    points.addPoint("ExceptionAddress", excepAddress.str().c_str());

                    for (uint32_t i = 0; i < excInfo->ExceptionRecord->NumberParameters; ++i)
                    {
                        std::ostringstream paramName;
                        paramName << "ExceptionParameter-" << i;
                        std::stringstream paramValue;
                        paramValue << excInfo->ExceptionRecord->ExceptionInformation[i];
                        points.addPoint(paramName.str(), paramValue.str().c_str());
                    }
                }
            }
            points.addPoint("SessionID", MainLogManager::getMainLogManager()->getSessionId().c_str());
            points.report(crashCounterNamePrefix + "CrashEvent", DFInt::RCCInfluxHundredthsPercentage);
        }
    }
    catch (const std::exception& e)
    {
        Aya::StandardOut::singleton()->printf(Aya::MESSAGE_ERROR, "Exception during upload crash event: %s", e.what());
    }
    catch (...)
    {
        Aya::StandardOut::singleton()->printf(Aya::MESSAGE_ERROR, "Exception during upload crash event");
    }
#endif
}

Aya::worker_thread::work_result DumpErrorUploader::run(shared_ptr<data> _data)
{
    std::string file;
    {
        boost::recursive_mutex::scoped_lock lock(_data->sync);
        if (_data->files.empty())
        {
            if (_data->hInterprocessMutex)
            {
                CloseHandle(_data->hInterprocessMutex);
                _data->hInterprocessMutex = NULL;
            }

            if (_data->dmpFileCount > 0)
            {
                // report number of dmps uploaded
                Aya::Analytics::EphemeralCounter::reportCounter(crashCounterNamePrefix + "Crash", _data->dmpFileCount, true);
            }

            return Aya::worker_thread::done;
        }
        file = _data->files.front();
    }

    try
    {
        printf("Uploading %s\n", file.c_str());
        bool isDmpFile = file.substr(file.size() - 4) == ".dmp";
        if (isDmpFile)
            _data->dmpFileCount++;

        bool isFullDmp = file.find(".Full.") != std::string::npos;

        // TODO: put the filename in the header rather than the query string??
        std::string url = _data->url;
        url += "?filename=";
        url += Aya::Http::urlEncode(file);

        if (isDmpFile && _data->dmpFileCount > 3)
        {
            std::stringstream data("Too many dmp files");
            std::string response;
            Aya::Http(url).post(data, Aya::Http::kContentTypeDefaultUnspecified, true, response);
        }
        else if (!isFullDmp)
        {
            Aya::StandardOut::singleton()->printf(Aya::MESSAGE_INFO, "Uploading %s\n", file.c_str());
            std::fstream data(file.c_str(), std::ios_base::in | std::ios_base::binary);
            std::streamoff begin = data.tellg();
            data.seekg(0, std::ios::end);
            std::streamoff end = data.tellg();
            if (end > begin)
            {
                data.seekg(0, std::ios::beg);
                std::string response;
                Aya::Http(url).post(data, Aya::Http::kContentTypeDefaultUnspecified, true, response);
            }
            else
            {
                // Some dmp files are empty. Post it anyway so that we can report it
                std::stringstream data("Empty!!!");
                std::string response;
                Aya::Http(url).post(data, Aya::Http::kContentTypeDefaultUnspecified, true, response);
            }
        }

        // done uploading, move to archive.
        // nb: if upload cuts right after uploading .dmp file, we will never upload the other log files associated.
        // we think this is acceptable to keep this code simple at this time.
        ErrorUploader::MoveRelative((LPCTSTR)file.c_str(), "archive\\");
    }
    catch (std::exception& e)
    {
        Aya::StandardOut::singleton()->print(Aya::MESSAGE_ERROR, e);
    }

    {
        boost::recursive_mutex::scoped_lock lock(_data->sync);
        _data->files.pop();
    }
    return Aya::worker_thread::more;
}