#if defined(__APPLE__)
#undef ENABLE_CHROMIUM_FRAMES
#endif

#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
#pragma once

#include "DataModel/ChromiumFrame.hpp"

// libcef
#include <include/base/cef_bind.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_process_message.h>

#include <filesystem>
#include <future>

namespace Aya
{

std::filesystem::path getModuleParentPath();

class ChromiumFrame;

class CefIntegration
{
public:
    static void initialize();
    static void shutdown();
};

class AyaCefApp : public CefApp
{
public:
    AyaCefApp(){};

private:
    IMPLEMENT_REFCOUNTING(AyaCefApp);
};

class AyaCefRenderer : public CefRenderHandler
{
public:
    AyaCefRenderer(ChromiumFrame* frame);
    ~AyaCefRenderer();

    void UpdateResolution(int height, int width);
    bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect);
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);
    bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info);
    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects,
        const void* buffer, int w, int h);
    void OnCursorChange(CefRefPtr< CefBrowser > browser, CefCursorHandle handle, cef_cursor_type_t cursor);
    void OnScrollOffsetChanged( CefRefPtr< CefBrowser > browser, double x, double y );

    bool StartDragging(
        CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> drag_data, CefRenderHandler::DragOperationsMask allowed_ops, int x, int y)
    {
        return false;
    }

    // These methods are purposefully left unimplemented
    CefRefPtr<CefAccessibilityHandler> GetAccessibilityHandler()
    {
        return nullptr;
    }


    void UpdateDragCursor(CefRefPtr<CefBrowser> browser, CefRenderHandler::DragOperation operation) {}
    void OnImeCompositionRangeChanged(
        CefRefPtr<CefBrowser> browser, const CefRange& selected_range, const CefRenderHandler::RectList& character_bounds)
    {
    }

private:
    IMPLEMENT_REFCOUNTING(AyaCefRenderer);
    int height, width;
    ChromiumFrame* frame;

    std::mutex paintMutex;
};

class AyaCefClient
    : public CefClient
    , public CefLifeSpanHandler
    , public CefLoadHandler
    , public CefDisplayHandler
{

public:
    AyaCefClient(ChromiumFrame* frame);
    ~AyaCefClient();

    void OnAfterCreated(CefRefPtr<CefBrowser> browser);
    bool DoClose(CefRefPtr<CefBrowser> browser);

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()
    {
        return this;
    }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler()
    {
        return this;
    }
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
    {
        return this->renderHandler;
    }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler()
    {
        return this;
    }
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) {}

    bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name,
        WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo,
        CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
    {
        return true;
    }

    void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type);
    void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode);
    void OnLoadError(
        CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl);
    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line);
    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {}
    bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);
    const bool closeAllowed()
    {
        return this->closing;
    }
    const bool isLoaded()
    {
        return this->loaded;
    }

    // promises for js
    // should we use boost future? it's what studio uses
    std::promise<std::string>* resultPromise = nullptr;
    void SetPromise(std::promise<std::string>* prom)
    {
        resultPromise = prom;
    }

private:
    int browser_id = 0;
    bool closing = false;
    bool loaded = false;
    CefRefPtr<CefRenderHandler> renderHandler;
    ChromiumFrame* frame;

    IMPLEMENT_REFCOUNTING(AyaCefClient);
};

} // namespace Aya
#endif