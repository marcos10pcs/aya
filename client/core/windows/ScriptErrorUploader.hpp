#pragma once

#include "ErrorUploader.hpp"

class ScriptErrorUploader : public Aya::ErrorUploader
{
public:
    ScriptErrorUploader(bool backgroundUpload)
    {
        if (backgroundUpload)
            thread.reset(new Aya::worker_thread(boost::bind(&ScriptErrorUploader::run, _data), "ScriptErrorUploader"));
    }
    /*override*/ void Upload(std::string url);

private:
    static Aya::worker_thread::work_result run(shared_ptr<data> _cseData);
};
