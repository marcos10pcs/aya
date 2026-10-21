#pragma once

#include <queue>
#include "aya/Boost.hpp"
#include "aya/Thread.hpp"

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/concepts.hpp> // source

#ifdef _WIN32
// For mkdir
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include "_FindFirst.hpp"
#endif
#include "errno.h"
#include <windows.h>

namespace Aya
{
class ErrorUploader
{
protected:
    struct data
    {
        std::queue<std::string> files;
        boost::recursive_mutex sync; // TODO: Would non-recursive be safe here?
        std::string url;
        int dmpFileCount;
        HANDLE hInterprocessMutex;

        data()
            : dmpFileCount(0)
            , hInterprocessMutex(NULL)
        {
        }
    };
    shared_ptr<data> _data;
    boost::scoped_ptr<Aya::worker_thread> thread;

public:
    ErrorUploader()
        : _data(new data())
    {
    }

    static std::string MoveRelative(LPCTSTR fileName, std::string path);

    void Cancel()
    {
        boost::recursive_mutex::scoped_lock lock(_data->sync);
        while (!_data->files.empty())
            _data->files.pop();
    }
    bool IsUploading()
    {
        return !_data->files.empty();
    }
};
}