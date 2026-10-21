#include "aya/SystemUtil.hpp"
#include "util/StreamHelpers.hpp"

#ifdef __ANDROID_API__
#include <android/api-level.h>
#endif 

#include <fstream>
#include <string>

#include <unistd.h>
#include <GL/HeadersGL.hpp>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

using namespace Aya;

namespace Aya
{
namespace SystemUtil
{
std::string mOSVersion; // Set in JNIGLActivity.cpp
std::string mDeviceName;

std::string getCPUMake()
{
#ifdef __arm_
    return "ARM";
#elif __aarch64__
    return "AArch64";
#elif __i386__ || __amd64__
    return "Intel";
#else
#error Unsupported platform.
#endif
}

uint64_t getCPUSpeed()
{
    return 0;
}

uint64_t getCPULogicalCount()
{
    return getCPUCoreCount();
}

uint64_t getCPUCoreCount()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

uint64_t getCPUPhysicalCount()
{
    return getCPUCoreCount();
}

bool isCPU64Bit()
{
#ifdef __arm64__ || __amd64__
    return true;
#else
    return false;
#endif
}

uint64_t getMBSysRAM()
{
    return 900;
}

uint64_t getMBSysAvailableRAM()
{
    return 0; // Does not appear to be used.
}

uint64_t getVideoMemory()
{
    size_t availableVRAM = 2^30; // 2 GiB

    if (glewIsSupported("GL_NVX_gpu_memory_info")) {
        GLint cur_avail_mem_kb = 0;
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);
        availableVRAM = cur_avail_mem_kb * 1024;
    }
    else if (glewIsSupported("GL_ATI_meminfo")) {
        GLint nCurAvailMemoryInKB = 0;
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &nCurAvailMemoryInKB);
        availableVRAM = nCurAvailMemoryInKB * 1024;
    }

    return availableVRAM;
}

std::string osPlatform()
{
#ifdef __ANDROID_API__
    return "Android";
#else
    return "Linux";
#endif
}

int osPlatformId()
{
#ifdef __ANDROID_API__
    return __ANDROID_API__;
#else
    return 0x7999999999;
#endif
}

std::string osVer()
{
    return mOSVersion;
}

std::string deviceName()
{
    return Aya::SystemUtil::mDeviceName;
}

std::string getGPUMake()
{
#ifdef __ANDROID_API__
    return "Android";
#else
    return "Some Linux GPU";
#endif
}

std::string getMaxRes()
{
    return "";
}

std::string getExecutablePath() {
  char buffer[PATH_MAX] = {0};
  readlink("/proc/self/exe", buffer, sizeof(buffer));
  return std::filesystem::path(buffer).parent_path();
}
} // namespace SystemUtil
} // namespace Aya
