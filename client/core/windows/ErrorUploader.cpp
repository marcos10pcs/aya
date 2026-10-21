#undef min
#undef max


#include "ErrorUploader.hpp"
#include "LogManager.hpp"
#include "format_string.hpp"
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/concepts.hpp> // source
#include <filesystem>

#include "Utility/Http.hpp"
#include "Utility/standardout.hpp"

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

namespace Aya
{
std::string ErrorUploader::MoveRelative(LPCTSTR fileName, std::string path)
{
    std::filesystem::path filePath(fileName);
    std::filesystem::path dirPath = filePath.parent_path(); // Get directory path of the file
    std::filesystem::path newDirPath = dirPath / path; // Append the new path

    // Create the directory if it doesn't exist
    if (!std::filesystem::exists(newDirPath)) {
        std::filesystem::create_directories(newDirPath);
    }

    // Construct new file path
    std::filesystem::path newFilePath = newDirPath / filePath.filename();

    // Move the file
    std::filesystem::rename(filePath, newFilePath);

    // Convert the result path to std::string for return
    return newFilePath.string();
}
}