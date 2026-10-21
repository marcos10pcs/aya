#if defined(__APPLE__)
#undef ENABLE_CHROMIUM_FRAMES
#endif

#ifdef ENABLE_CHROMIUM_FRAMES
#pragma once

#include "DataModel/GuiObject.hpp"
#include "DataModel/GuiMixin.hpp"

#include "Utility/CefIntegration.hpp"

#include <include/cef_app.h>
#include <include/cef_base.h>

namespace Aya
{

enum ChromiumLoadState
{
    Stopped,
    Loaded,
    Loading,
    Error
};

enum ChromiumMouseCursor
{
    Pointer,
    Cross,
    Hand,
    IBeam,
    Wait,
    Help,
    EastResize,
    NorthResize,
    NortheastResize,
    NorthwestResize,
    SouthResize,
    SoutheastResize,
    SouthwestResize,
    WestResize,
    NorthSouthResize,
    EastWestResize,
    NortheastSouthwestResize,
    NorthwestSoutheastResize,
    ColumnResize,
    RowResize,
    MiddlePanning,
    EastPanning,
    NorthPanning,
    NortheastPanning,
    NorthwestPanning,
    SouthPanning,
    SoutheastPanning,
    SouthwestPanning,
    WestPanning,
    Move,
    VerticalText,
    Cell,
    ContextMenu,
    Alias,
    Progress,
    NoDrop,
    Copy,
    None,
    NotAllowed,
    ZoomIn,
    ZoomOut,
    Grab,
    Grabbing,
    Custom
};

// fwd decl
class AyaCefRenderer;
class AyaCefClient;

extern const char* const sChromiumFrame;

class ChromiumFrame
    : public DescribedCreatable<ChromiumFrame, GuiLabel, sChromiumFrame>
    , public GuiImageMixin
{
public:
    ChromiumFrame();
    ~ChromiumFrame();

    void initialize();
    void uninitialize();

    void mouseLeftClick(int x, int y, bool isDown = true);
    void mouseMiddleClick(int x, int y, bool isDown = true);
    void mouseRightClick(int x, int y, bool isDown = true);
    void mouseWheel(int x, int y, int deltaX, int deltaY);
    void keyEvent(int keyCode, int modifiers, bool isKeyDown = true);

    std::string runJS(std::string code);

    void refresh();
    void refreshNoCache();
    void backward();
    void forward();
    void cancel();

    bool getIsLoaded() const
    {
        return (loadState == ChromiumLoadState::Loaded);
    }
    bool getCanPageBackward() const
    {
        return canPageBackward;
    }
    void setCanPageBackward(bool value);
    bool getCanPageForward() const
    {
        return canPageForward;
    }
    void setCanPageForward(bool value);

    bool getInitialized() const
    {
        return initialized;
    }
    void setInitializedInternal(bool value);

    std::string getPageTitle() const
    {
        return pageTitle;
    }
    void setPageTitleInternal(std::string value);

    int getStatusCode() const
    {
        return statusCode;
    }
    void setStatusCodeInternal(int value);

    ChromiumLoadState getLoadState() const
    {
        return loadState;
    }
    void setLoadStateInternal(ChromiumLoadState value);

    Vector2 getResolution() const
    {
        return this->resolution;
    }
    void setResolution(Vector2 resolution);

    Vector2 getMousePosition() const
    {
        return this->mousePosition;
    }
    void setMousePosition(Vector2 position);

    std::string getURL() const
    {
        return this->url;
    }
    void setURL(std::string url);
    void setURLInternal(std::string url); // called by AyaCefClient::OnLoadStart

    ChromiumMouseCursor getMouseCursor() const
    {
        return this->mouseCursor;
    }
    void setMouseCursorInternal(ChromiumMouseCursor cursor);

    Vector2 getScrollOffset() const
    {
        return this->scrollOffset;
    }
    void setScrollOffsetInternal(Vector2 offset);

    Color3 getColorAtPixel(int x, int y);

    // These are public so that AyaCefRenderer::OnPaint may modify them
    bool shouldReuploadTexture = false;
    std::vector<char> buffer;

    Aya::remote_signal<void(bool)> isLoadedSignal;
    Aya::remote_signal<void(ChromiumMouseCursor)> mouseCursorChangedSignal;
    Aya::remote_signal<void(std::string, std::string, int)> onConsoleMessageSignal;
    Aya::remote_signal<void(Vector2)> scrollOffsetChangedSignal;
    static Reflection::RemoteEventDesc<ChromiumFrame, void(std::string, std::string, int)> event_ChromiumFrameConsoleMessage;

#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
    // These are public so that the cefLoop may set them upon initialization
    CefRefPtr<CefBrowser> cefInstance;
    CefRefPtr<AyaCefRenderer> cefRenderer;
    CefRefPtr<AyaCefClient> cefClient;
#endif

    int lastPaintWidth;
    int lastPaintHeight;

private:
#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
    void mouseClick(int x, int y, cef_mouse_button_type_t button, bool isDown = true);
#endif

    std::string url;
    std::string pageTitle;

    bool initialized;
    bool canPageBackward = false;
    bool canPageForward = false;

    int statusCode;

    Vector2 resolution;
    Vector2 mousePosition;
    Vector2 scrollOffset;

    ChromiumLoadState loadState;
    ChromiumMouseCursor mouseCursor;

    shared_ptr<Graphics::Texture> texture;

    /*override*/ void onAncestorChanged(const AncestorChanged& event);

    //
    // IAdornable
    //

    /*override*/ void render2d(Adorn* adorn);
    /*override*/ void renderBackground2d(Adorn* adorn);
};

} // namespace Aya
#endif