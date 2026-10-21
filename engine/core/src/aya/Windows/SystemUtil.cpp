#include "aya/SystemUtil.hpp"
#include "aya/ProcessPerfCounter.hpp"
#include "util/RegistryUtil.hpp"
#include "FastLog.hpp"

#include <Windows.h>
#include <setupapi.h>
#include <filesystem>

#include <initguid.h>
#include <d3d9.h>
#include "ddraw.h"
#include <d3d11.h>
#include <dxgi1_4.h>
LOGVARIABLE(DXVideoMemory, 4)


namespace Aya
{
namespace SystemUtil
{
std::string osVer()
{
    OSVERSIONINFO osvi = {0};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning(disable : 4996 28159)
    GetVersionEx(&osvi);
    return Aya::format("%d.%d.%d.%d", osvi.dwOSVersionInfoSize, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
}

bool isCPU64Bit()
{
#if defined(_WIN64)
    return true;
#else
    BOOL result = false;
    return IsWow64Process(GetCurrentProcess(), &result) && result;
#endif
}

int osPlatformId()
{
    return VER_PLATFORM_WIN32_NT;
}

std::string osPlatform()
{
    return "Win32";
}

std::string deviceName()
{
    return "PC";
}

std::string getGPUMake()
{
    DISPLAY_DEVICE displayDevice;
    ZeroMemory(&displayDevice, sizeof(displayDevice));
    displayDevice.cb = sizeof(displayDevice);
    EnumDisplayDevices(NULL, 0, &displayDevice, 0);

    return std::string(displayDevice.DeviceString);
}

uint64_t getVideoMemory()
{
    size_t freeVideoRam = 2 ^ 30; // 2 GiB

    // Create a DXGI Factory
    IDXGIFactory4* pFactory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)(&pFactory));
    if (FAILED(hr))
    {
        return freeVideoRam;
    }

    IDXGIAdapter3* pAdapter = nullptr;
    hr = pFactory->EnumAdapters1(0, (IDXGIAdapter1**)&pAdapter); // Get the first adapter
    if (FAILED(hr))
    {
        pFactory->Release();
        return freeVideoRam;
    }

    DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
    hr = pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo);
    if (FAILED(hr))
    {
        pAdapter->Release();
        pFactory->Release();
        return freeVideoRam;
    }

    freeVideoRam = memoryInfo.Budget;

    pAdapter->Release();
    pFactory->Release();

    return freeVideoRam;
}

std::string getExecutablePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path().string();
}
} // namespace SystemUtil
} // namespace Aya
