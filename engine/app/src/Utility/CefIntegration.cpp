#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
#include "CefIntegration.hpp"
#include "DataModel/ChromiumFrame.hpp"
#include "DataModel/DataModel.hpp"
#include "Utility/standardout.hpp"
#include "include/internal/cef_types.h"

#include <filesystem>
#include <mutex>
#include <algorithm>

#define CEF_SUBPROCESS_NAME "Aya.WebHelper"

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#define CEF_SUBPROCESS_PATH CEF_SUBPROCESS_NAME ".exe"
#endif

#ifdef __linux
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#ifndef __aarch64__
#include <x86intrin.h>
#endif
#define CEF_SUBPROCESS_PATH CEF_SUBPROCESS_NAME
#endif

#if !defined(_M_ARM64) && !defined(__aarch64__)
#include <xmmintrin.h>
#else
#include <arm_neon.h>
#endif

std::string ws2s(const wchar_t* ws)
{
    std::string s;
#ifdef _WIN32
    int len = WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL);
    if (len > 0)
    {
        s.resize(len - 1);
        WideCharToMultiByte(CP_ACP, 0, ws, -1, &s[0], len, NULL, NULL);
    }
#else
    s.resize(strlen((char*)ws)); // HACK:
    int len = wcstombs(&s[0], ws, wcslen(ws));
#endif
    return s;
}

// Latest Chrome useragent
#define USER_AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36"

namespace Aya
{

#ifndef AYA_SERVER
bool isRunning = false;

// TODO: Ideally, put this in SystemUtil
std::filesystem::path getModuleParentPath()
{
#ifdef WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
#else
    char buffer[PATH_MAX] = {0};
    readlink("/proc/self/exe", buffer, sizeof(buffer));
#endif

    return std::filesystem::path(buffer).parent_path();
}

void CefIntegration::initialize()
{
    CefMainArgs args;
    CefSettings settings;

    settings.command_line_args_disabled = true;
    settings.no_sandbox = true; // Justification for this: this is non-portable, too bulky, and we'd like access to system resources for faster
                                // rendering We are implementing a URL whitelist anyway, so malicious content is a non-issue.
    settings.windowless_rendering_enabled = true;
    settings.multi_threaded_message_loop = true; // we can enable this since we are app
#ifdef AYA_TEST_BUILD
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_VERBOSE;
#else
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_DISABLE;
#endif

    std::filesystem::path path = getModuleParentPath();
    CefString(&settings.resources_dir_path).FromASCII((path).string().c_str());
    CefString(&settings.locales_dir_path).FromASCII((path / "locales").string().c_str());
    CefString(&settings.browser_subprocess_path).FromASCII((path / CEF_SUBPROCESS_PATH).string().c_str());
    CefString(&settings.root_cache_path).FromASCII((path / "cache").string().c_str());

    CefString(&settings.user_agent).FromASCII(USER_AGENT);

    if (!CefInitialize(args, settings, new AyaCefApp(), nullptr))
    {
        Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "Failed to initialize CEF");
        return;
    }
}

void CefIntegration::shutdown()
{
    CefShutdown();

#ifdef WIN32
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(ws2s(pEntry.szExeFile).c_str(), CEF_SUBPROCESS_PATH) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
#else
    DIR* dir;
    struct dirent* entry;
    dir = opendir("/proc");
    if (dir)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            bool isproc = true;
            for (int i = 0; i < strlen(entry->d_name); i++)
            {
                if (!isdigit(entry->d_name[i]))
                { // process d_name is always its pid so if there are non-digits stop everytihng
                    isproc = false;
                    break;
                }
            }

            if (!isproc)
                continue;

            char processpath[64 + 1];
            snprintf(processpath, 64, "/proc/%s/status",
                entry->d_name); // status file stores Name: which should be the filename of the program without any path

            FILE* status = fopen(processpath, "r");
            char str[128];
            while (fgets(str, sizeof(str), status))
            { // reads status file line by line
                char f_entry[4 + 1], proc[sizeof(CEF_SUBPROCESS_PATH) + 1];
                strncpy(f_entry, str, 4); // copy first 4 characters of line into f_entry, should be 'Name' all the time
                if (strncmp(f_entry, "Name", 4) == 0)
                {
                    strncpy(proc, str + 6, sizeof(CEF_SUBPROCESS_PATH)); // the 6th character should probably be the process name
                    if (strncmp(proc, CEF_SUBPROCESS_PATH, sizeof(CEF_SUBPROCESS_PATH)) == 0)
                    { // we have found the process we want to kill
                        pid_t pid = atoi(entry->d_name);
                        kill(pid, SIGTERM);
                        break;
                    }
                }
            }
            fclose(status);
        }
        closedir(dir);
    }
#endif
}

/// AyaCefRenderer

AyaCefRenderer::AyaCefRenderer(ChromiumFrame* frame)
    : frame(frame)
{
    Vector2 resolution = frame->getResolution();

    this->width = resolution.x;
    this->height = resolution.y;
}

AyaCefRenderer::~AyaCefRenderer()
{
    //
}

void AyaCefRenderer::UpdateResolution(int width, int height)
{
    this->width = width;
    this->height = height;
    this->frame->buffer.resize(width * height * 4);
}

bool AyaCefRenderer::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect = CefRect(0, 0, this->width, this->height);

    return true;
}

void AyaCefRenderer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect = CefRect(0, 0, this->width, this->height);
}

bool AyaCefRenderer::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    screen_info.rect = CefRect(0, 0, this->width, this->height);
    screen_info.device_scale_factor = 1.0;

    return true;
}

void AyaCefRenderer::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height)
{
    const uint8_t* source = static_cast<const uint8_t*>(buffer);
    for (const auto& rect : dirtyRects)
    {
        if (rect.x + rect.width > width || rect.y + rect.height > height)
            continue;

        for (int i = 0; i < rect.height; ++i)
        {
            // Load 4 pixels at once to do processing on with SIMD/SSE2
            int rowOffset = ((rect.y + i) * width + rect.x) * 4;
            for (int j = 0; j < rect.width; j += 4)
            {
                // Calculate the pixel position in the linear buffer
                int sourceOffset = rowOffset + j * 4;

#if defined(_M_ARM64) || defined(__aarch64__)
                // Load 4 pixels at once to do processing on with NEON
                uint8x16_t pixels = vld1q_u8(source + sourceOffset);

                // Shuffle to swap R and B (BGRA -> RGBA)
                uint8x16_t mask = {2, 1, 0, 3, 6, 5, 4, 7, 10, 9, 8, 11, 14, 13, 12, 15};
                uint8x16_t shuffled = vqtbl1q_u8(pixels, mask);

                // Store the shuffled pixels back to the buffer
                vst1q_u8(reinterpret_cast<uint8_t*>(this->frame->buffer.data()) + sourceOffset, shuffled);
#else
                // 16 bytes, since each pixel is BGRA
                __m128i pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(source + sourceOffset));

                // Shuffle to swap R and B (BGRA -> RGBA)
                // ChromiumFrame->buffer expects RGBA data
                __m128i mask = _mm_setr_epi8(2, 1, 0, 3, // Pixel #1 (R, G, B, A) -> (B, G, R, A)  ;  (0, 1, 2, 3)     -> (2, 1, 0, 3)
                    6, 5, 4, 7,                          // Pixel #2 (R, G, B, A) -> (B, G, R, A)  ;  (4, 5, 6, 7)     -> (6, 5, 4, 7)
                    10, 9, 8, 11,                        // Pixel #3 (R, G, B, A) -> (B, G, R, A)  ;  (8, 9, 10, 11)   -> (10, 9, 8, 11)
                    14, 13, 12, 15                       // Pxiel #4 (R, G, B, A) -> (B, G, R, A)  ;  (12, 13, 14, 15) -> (14, 13, 12, 15)
                );
                __m128i shuffled = _mm_shuffle_epi8(pixels, mask);

                // Store the shuffled pixels back to the buffer
                _mm_storeu_si128(reinterpret_cast<__m128i*>(this->frame->buffer.data() + sourceOffset), shuffled);
#endif
            }
        }
    }

    this->frame->lastPaintWidth = width;
    this->frame->lastPaintHeight = height;
    this->frame->shouldReuploadTexture = true;
}

void AyaCefRenderer::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle handle, cef_cursor_type_t cursor)
{
    this->frame->setMouseCursorInternal(static_cast<ChromiumMouseCursor>(cursor));
}

void AyaCefRenderer::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
    this->frame->setScrollOffsetInternal(Vector2(x, y));
}

/// AyaCefClient

AyaCefClient::AyaCefClient(ChromiumFrame* frame)
    : frame(frame)
{
    this->renderHandler = this->frame->cefRenderer;
}

AyaCefClient::~AyaCefClient()
{
    if (resultPromise)
    {
        resultPromise->set_value("");
    }
}

void AyaCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    if (this->browser_id == 0)
        this->browser_id = browser->GetIdentifier();

    this->frame->cefInstance = browser;
    this->frame->setInitializedInternal(true);
}

bool AyaCefClient::DoClose(CefRefPtr<CefBrowser> browser)
{
    if (browser->GetIdentifier() == this->browser_id)
    {
        this->frame->setLoadStateInternal(ChromiumLoadState::Stopped);
        this->closing = true;
    }

    return false;
}

void AyaCefClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    // todo: this is broken
    // ^^^^ fixed: https://magpcss.org/ceforum/viewtopic.php?f=6&t=14845
    std::string url = frame->GetBrowser()->GetMainFrame()->GetURL(); // this actualy gets the main frame's url
    Aya::StandardOut::singleton()->printf(MESSAGE_INFO, "Loading URL: %s", url.c_str());
    this->frame->setPageTitleInternal("");

    if (frame->IsMain())
    {
        this->frame->setLoadStateInternal(ChromiumLoadState::Loading);
    }

    if (this->frame->getURL() != url)
    {
        this->frame->setURLInternal(url);
    }
}

void AyaCefClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    if (frame->IsMain())
    {
        this->loaded = true;

        std::string url = frame->GetURL();
        if (this->frame->getURL() != url)
        {
            this->frame->setURLInternal(url);
            this->frame->setLoadStateInternal(ChromiumLoadState::Loaded);
        }

        this->frame->setCanPageForward(frame->GetBrowser()->GetMainFrame()->GetBrowser()->CanGoForward()); // ????
        this->frame->setCanPageBackward(frame->GetBrowser()->GetMainFrame()->GetBrowser()->CanGoBack());   // ????

        this->frame->setStatusCodeInternal(httpStatusCode);
        this->frame->setLoadStateInternal(ChromiumLoadState::Loaded);
    }
}

void AyaCefClient::OnLoadError(
    CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    this->loaded = true;
    this->frame->setStatusCodeInternal(400); // ? - errorcode is not http codes so not sure
    this->frame->setLoadStateInternal(ChromiumLoadState::Error);
}

bool AyaCefClient::OnConsoleMessage(
    CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line)
{
    this->frame->event_ChromiumFrameConsoleMessage.fireAndReplicateEvent(this->frame, message.ToString().c_str(), source.ToString().c_str(), line);

    return true;
}

bool AyaCefClient::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    if (message->GetName() == "resultMessage")
    {
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        if (args->GetSize() > 0 && args->GetType(0) == VTYPE_STRING)
        {
            std::string result = args->GetString(0);
            if (resultPromise)
            {
                resultPromise->set_value(result);
                resultPromise = nullptr; // Reset the promise after setting the value
            }
            return true;
        }
    }
    return false;
}

void AyaCefClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    // doesn't work?
    std::string titleStr = title.ToString();
    this->frame->setPageTitleInternal(titleStr);
}
#endif

} // namespace Aya
#endif