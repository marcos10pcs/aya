

#undef min
#undef max

#include "ScriptErrorUploader.hpp"
#include "LogManager.hpp"

#include "Utility/Http.hpp"
#include "Utility/standardout.hpp"
#include "ErrorUploader.hpp"

namespace io = boost::iostreams;

void ScriptErrorUploader::Upload(std::string url)
{
    // Do nothing
}

Aya::worker_thread::work_result ScriptErrorUploader::run(shared_ptr<data> _cseData)
{
    std::string file;
    {
        boost::recursive_mutex::scoped_lock lock(_cseData->sync);
        if (_cseData->files.empty())
        {
            if (_cseData->hInterprocessMutex)
            {
                CloseHandle(_cseData->hInterprocessMutex);
                _cseData->hInterprocessMutex = NULL;
            }
            return Aya::worker_thread::done;
        }
        file = _cseData->files.front();
    }

    try
    {
        printf("Uploading %s\n", file.c_str());
        bool isCseFile = file.substr(file.size() - 4) == ".cse";
        if (isCseFile)
            _cseData->dmpFileCount++;

        // TODO: put the filename in the header rather than the query string??
        std::string url = _cseData->url;
        url += "?filename=";
        url += file;

        if (isCseFile && _cseData->dmpFileCount > 3)
        {
            std::stringstream data("Too many cse files");
            std::string response;
            Aya::Http(url).post(data, Aya::Http::kContentTypeDefaultUnspecified, true, response);
        }
        else
        {
            std::fstream data(file.c_str(), std::ios_base::in | std::ios_base::binary);
            size_t begin = data.tellg();
            data.seekg(0, std::ios::end);
            size_t end = data.tellg();
            if (end > begin)
            {
                data.seekg(0, std::ios::beg);
                std::string response;
                Aya::Http(url).post(data, Aya::Http::kContentTypeDefaultUnspecified, true, response);
            }
            else
            {
                // Some cse files are empty. Post it anyway so that we can report it
                std::stringstream data("Empty!!!");
                std::string response;
                Aya::Http(url).post(data, Aya::Http::kContentTypeDefaultUnspecified, true, response);
            }
        }

        // d9mz - fix soon! linker errors
        // ErrorUploader::MoveRelative((LPCTSTR)file.c_str(), "archive\\");
    }
    catch (std::exception& e)
    {
        Aya::StandardOut::singleton()->print(Aya::MESSAGE_ERROR, e);
    }

    {
        boost::recursive_mutex::scoped_lock lock(_cseData->sync);
        _cseData->files.pop();
    }
    return Aya::worker_thread::more;
}