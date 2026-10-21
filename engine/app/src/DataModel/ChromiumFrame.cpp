#if defined(__APPLE__)
#undef ENABLE_CHROMIUM_FRAMES
#endif

#ifdef ENABLE_CHROMIUM_FRAMES
/* See: https://github.com/kiseki-lol/aya/issues/76 */

#include "DataModel/ChromiumFrame.hpp"
#include "DataModel/GuiLayerCollector.hpp"
#include "GfxRender/AdornRender.hpp"
#include "Utility/standardout.hpp"
#include "Network/Players.hpp"

#include <boost/thread/once.hpp>
#include <filesystem>

RBX_REGISTER_ENUM(ChromiumLoadState);
RBX_REGISTER_ENUM(ChromiumMouseCursor);

DYNAMIC_FASTFLAGVARIABLE(MaximumChromiumFrames, 10);

namespace Aya
{

const char* const sChromiumFrame = "ChromiumFrame";

int nChromiumFrames = 0;

static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_refresh(&ChromiumFrame::refresh, "PageRefresh", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_backward(&ChromiumFrame::backward, "PageBackward", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_forward(&ChromiumFrame::forward, "PageForward", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_cancel(&ChromiumFrame::cancel, "CancelLoading", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_refreshNoCache(&ChromiumFrame::refreshNoCache, "PageRefreshNoCache", Security::None);

static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_initialize(&ChromiumFrame::initialize, "Initialize", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void()> func_uninitialize(&ChromiumFrame::uninitialize, "Uninitialize", Security::None);

static Reflection::BoundFuncDesc<ChromiumFrame, std::string(std::string)> func_runJS(&ChromiumFrame::runJS, "RunJS", "code", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void(int, int, bool)> func_mouseLeftClick(
    &ChromiumFrame::mouseLeftClick, "MouseLeftClick", "x", "y", "isDown", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void(int, int, bool)> func_mouseMiddleClick(
    &ChromiumFrame::mouseMiddleClick, "MouseMiddleClick", "x", "y", "isDown", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void(int, int, bool)> func_mouseRightClick(
    &ChromiumFrame::mouseRightClick, "MouseRightClick", "x", "y", "isDown", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void(int, int, int, int)> func_mouseWheel(
    &ChromiumFrame::mouseWheel, "MouseWheel", "x", "y", "deltaX", "deltaY", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, void(int, int, bool)> func_keyEvent(
    &ChromiumFrame::keyEvent, "KeyEvent", "keyCode", "modifiers", "isKeyDown", Security::None);
static Reflection::BoundFuncDesc<ChromiumFrame, Color3(int, int)> func_getColorAtPixel(
    &ChromiumFrame::getColorAtPixel, "GetColorAtPixel", "x", "y", Security::None);

static Reflection::EnumPropDescriptor<ChromiumFrame, ChromiumLoadState> prop_loadState(
    "LoadState", "Browser", &ChromiumFrame::getLoadState, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, bool> prop_initialized(
    "Initialized", "Browser", &ChromiumFrame::getInitialized, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, bool> prop_canPageBackward(
    "CanPageBackward", "Browser", &ChromiumFrame::getCanPageBackward, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, bool> prop_canPageForward(
    "CanPageForward", "Browser", &ChromiumFrame::getCanPageForward, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, bool> prop_isLoaded(
    "IsLoaded", "Browser", &ChromiumFrame::getIsLoaded, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, Vector2> prop_resolution(
    "Resolution", "Browser", &ChromiumFrame::getResolution, &ChromiumFrame::setResolution, Reflection::PropertyDescriptor::STANDARD);
static Reflection::PropDescriptor<ChromiumFrame, Vector2> prop_mousePosition(
    "MousePosition", "Browser", &ChromiumFrame::getMousePosition, &ChromiumFrame::setMousePosition, Reflection::PropertyDescriptor::STANDARD);
static Reflection::PropDescriptor<ChromiumFrame, bool> prop_mouseCursor(
    "MouseCursor", "Browser", &ChromiumFrame::getMouseCursor, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, Vector2> prop_scrollOffset(
    "ScrollOffset", "Browser", &ChromiumFrame::getScrollOffset, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, int> prop_statusCode(
    "StatusCode", "Browser", &ChromiumFrame::getStatusCode, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);
static Reflection::PropDescriptor<ChromiumFrame, std::string> prop_url(
    "URL", "Browser", &ChromiumFrame::getURL, &ChromiumFrame::setURL, Reflection::PropertyDescriptor::STANDARD);
static Reflection::PropDescriptor<ChromiumFrame, std::string> prop_pageTitle(
    "PageTitle", "Browser", &ChromiumFrame::getPageTitle, NULL, Reflection::PropertyDescriptor::NO_XML_WRITE);

static Reflection::RemoteEventDesc<ChromiumFrame, void(bool)> event_ChromiumFrameLoaded(&ChromiumFrame::isLoadedSignal, "Loaded", "isLoaded",
    Security::None, Reflection::RemoteEventCommon::SCRIPTING, Reflection::RemoteEventCommon::BROADCAST);
static Reflection::RemoteEventDesc<ChromiumFrame, void(ChromiumMouseCursor)> event_ChromiumFrameMouseCursorChanged(&ChromiumFrame::mouseCursorChangedSignal,
    "MouseCursorChanged", "cursor", Security::None, Reflection::RemoteEventCommon::SCRIPTING, Reflection::RemoteEventCommon::BROADCAST);
static Reflection::RemoteEventDesc<ChromiumFrame, void(Vector2)> event_scrollOffsetChanged(&ChromiumFrame::scrollOffsetChangedSignal,
    "ScrollOffsetChanged", "offset", Security::None, Reflection::RemoteEventCommon::SCRIPTING, Reflection::RemoteEventCommon::BROADCAST);

Reflection::RemoteEventDesc<ChromiumFrame, void(std::string, std::string, int)> ChromiumFrame::event_ChromiumFrameConsoleMessage(
    &ChromiumFrame::onConsoleMessageSignal, "ConsoleMessage", "message", "source", "line", Security::None, Reflection::RemoteEventCommon::SCRIPTING,
    Reflection::RemoteEventCommon::BROADCAST);

template<>
Reflection::EnumDesc<ChromiumLoadState>::EnumDesc()
    : EnumDescriptor("ChromiumLoadState")
{
    addPair(ChromiumLoadState::Stopped, "Stopped");
    addPair(ChromiumLoadState::Loaded, "Loaded");
    addPair(ChromiumLoadState::Loading, "Loading");
    addPair(ChromiumLoadState::Error, "Error");
}

template<>
Reflection::EnumDesc<ChromiumMouseCursor>::EnumDesc()
    : EnumDescriptor("ChromiumMouseCursor")
{
    addPair(ChromiumMouseCursor::Pointer, "Pointer");
    addPair(ChromiumMouseCursor::Cross, "Cross");
    addPair(ChromiumMouseCursor::Hand, "Hand");
    addPair(ChromiumMouseCursor::IBeam, "IBeam");
    addPair(ChromiumMouseCursor::Wait, "Wait");
    addPair(ChromiumMouseCursor::Help, "Help");
    addPair(ChromiumMouseCursor::EastResize, "EastResize");
    addPair(ChromiumMouseCursor::NorthResize, "NorthResize");
    addPair(ChromiumMouseCursor::NortheastResize, "NortheastResize");
    addPair(ChromiumMouseCursor::NorthwestResize, "NorthwestResize");
    addPair(ChromiumMouseCursor::SouthResize, "SouthResize");
    addPair(ChromiumMouseCursor::SoutheastResize, "SoutheastResize");
    addPair(ChromiumMouseCursor::SouthwestResize, "SouthwestResize");
    addPair(ChromiumMouseCursor::WestResize, "WestResize");
    addPair(ChromiumMouseCursor::NorthSouthResize, "NorthSouthResize");
    addPair(ChromiumMouseCursor::EastWestResize, "EastWestResize");
    addPair(ChromiumMouseCursor::NortheastSouthwestResize, "NortheastSouthwestResize");
    addPair(ChromiumMouseCursor::NorthwestSoutheastResize, "NorthwestSoutheastResize");
    addPair(ChromiumMouseCursor::ColumnResize, "ColumnResize");
    addPair(ChromiumMouseCursor::RowResize, "RowResize");
    addPair(ChromiumMouseCursor::MiddlePanning, "MiddlePanning");
    addPair(ChromiumMouseCursor::EastPanning, "EastPanning");
    addPair(ChromiumMouseCursor::NorthPanning, "NorthPanning");
    addPair(ChromiumMouseCursor::NortheastPanning, "NortheastPanning");
    addPair(ChromiumMouseCursor::NorthwestPanning, "NorthwestPanning");
    addPair(ChromiumMouseCursor::SouthPanning, "SouthPanning");
    addPair(ChromiumMouseCursor::SoutheastPanning, "SoutheastPanning");
    addPair(ChromiumMouseCursor::SouthwestPanning, "SouthwestPanning");
    addPair(ChromiumMouseCursor::WestPanning, "WestPanning");
    addPair(ChromiumMouseCursor::Move, "Move");
    addPair(ChromiumMouseCursor::VerticalText, "VerticalText");
    addPair(ChromiumMouseCursor::Cell, "Cell");
    addPair(ChromiumMouseCursor::ContextMenu, "ContextMenu");
    addPair(ChromiumMouseCursor::Alias, "Alias");
    addPair(ChromiumMouseCursor::Progress, "Progress");
    addPair(ChromiumMouseCursor::NoDrop, "NoDrop");
    addPair(ChromiumMouseCursor::Copy, "Copy");
    addPair(ChromiumMouseCursor::None, "None");
    addPair(ChromiumMouseCursor::NotAllowed, "NotAllowed");
    addPair(ChromiumMouseCursor::ZoomIn, "ZoomIn");
    addPair(ChromiumMouseCursor::ZoomOut, "ZoomOut");
    addPair(ChromiumMouseCursor::Grab, "Grab");
    addPair(ChromiumMouseCursor::Grabbing, "Grabbing");
    addPair(ChromiumMouseCursor::Custom, "Custom");
}

REFLECTION_END();

ChromiumFrame::ChromiumFrame()
    : DescribedCreatable<ChromiumFrame, GuiLabel, sChromiumFrame>("ChromiumFrame")
    , initialized(false)
    , resolution(Vector2(1920, 1080))
    , mousePosition(Vector2(0, 0))
    , url("")
    , texture(nullptr)
{
#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
    this->buffer.resize(this->resolution.x * this->resolution.y * 4);
    this->lastPaintWidth = this->resolution.x;
    this->lastPaintHeight = this->resolution.y;
#endif
}

ChromiumFrame::~ChromiumFrame()
{
    if (this->initialized)
        this->uninitialize();
}

void ChromiumFrame::render2d(Adorn* adorn)
{
#ifndef AYA_SERVER
#ifdef ENABLE_CHROMIUM_FRAMES
    if (this->initialized && this->shouldReuploadTexture)
    {
        if (this->texture == nullptr || this->texture->getWidth() != this->lastPaintWidth || this->texture->getHeight() != this->lastPaintHeight)
        {
            Aya::StandardOut::singleton()->print(MESSAGE_INFO, "Creating texture for ChromiumFrame");
            TextureProxyBaseRef ref = adorn->createTextureProxy(this->lastPaintWidth, this->lastPaintHeight);
            guiImageDraw.setImageFromRef(ref);
            this->texture = boost::dynamic_pointer_cast<Aya::Graphics::TextureProxy>(ref)->getTexture();
        }

        this->texture->upload(0, 0, Graphics::TextureRegion(0, 0, 0, this->texture->getWidth(), this->texture->getHeight(), 1), this->buffer.data(),
            this->buffer.size());
        this->shouldReuploadTexture = false;
    }
#endif

    GuiObject* clippingObject = firstAncestorClipping();

    if (clippingObject == NULL || !absoluteRotation.empty())
        guiImageDraw.render2d(adorn, true, getRect2D(), Vector2(0, 0), Vector2(1, 1), Color4(1, 1, 1, 1), absoluteRotation, Gui::NOTHING, false);
    else
        guiImageDraw.render2d(
            adorn, true, getRect2D(), Vector2(0, 0), Vector2(1, 1), Color4(1, 1, 1, 1), clippingObject->getClippedRect(), Gui::NOTHING, false);
#endif
}

void ChromiumFrame::renderBackground2d(Adorn* adorn)
{
    if (this->getBackgroundTransparency() < 1.0)
        GuiObject::render2dImpl(adorn, getRenderBackgroundColor4());
}

void ChromiumFrame::initialize()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::Initialize can only be called from the client");

    if (this->initialized)
        throw std::runtime_error("Cannot initialize an already initialized ChromiumFrame");
    
    // broken rn, only lets one max.
    //if (nChromiumFrames >= DFFlag::MaximumChromiumFrames)
    //    throw std::runtime_error("Maximum number of ChromiumFrames reached");

#ifndef AYA_SERVER
    if (this->url.empty())
    {
        std::filesystem::path path = getModuleParentPath() / "content" / "default.html";
        this->url = "file:///" + path.string();
    }

    this->cefRenderer = new AyaCefRenderer(this);
    this->cefClient = new AyaCefClient(this);

    CefBrowserSettings browser_settings;
    CefWindowInfo window_info;

    browser_settings.background_color = CefColorSetARGB(255, 255, 255, 255);
    browser_settings.windowless_frame_rate = 60;
    browser_settings.javascript_access_clipboard = STATE_DISABLED;
    // browser_settings.local_storage = STATE_DISABLED;

    window_info.SetAsWindowless(0);
    CefRefPtr<CefDictionaryValue> emptyDictionary = CefDictionaryValue::Create();
    CefRefPtr<CefRequestContext> globalContext = CefRequestContext::GetGlobalContext();

    CefBrowserHost::CreateBrowser(window_info, this->cefClient.get(), this->getURL(), browser_settings, emptyDictionary, globalContext);

    nChromiumFrames++;
#endif
#endif
}

void ChromiumFrame::uninitialize()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::Uninitalize can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot uninitalized a non-initialized ChromiumFrame");

    Aya::StandardOut::singleton()->printf(MESSAGE_INFO, "Uninitializing ChromiumFrame");

#ifndef AYA_SERVER
    this->cefInstance->GetHost()->CloseBrowser(true);
    this->setInitializedInternal(false);
    this->cefInstance = nullptr;
    this->cefRenderer = nullptr;
    this->texture = nullptr;
    guiImageDraw.setImageFromRef(nullptr);

    nChromiumFrames--;
#endif
#endif
}

void ChromiumFrame::refresh()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::Refresh can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot refresh on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    this->cefInstance->GetMainFrame()->GetBrowser()->Reload();
#endif
#endif
}

void ChromiumFrame::refreshNoCache()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::RefreshNoCache can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot refresh on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    this->cefInstance->GetMainFrame()->GetBrowser()->ReloadIgnoreCache();
#endif
#endif
}

void ChromiumFrame::backward()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::PageBackward can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot go backwards on a non-initialized ChromiumFrame");
#ifndef AYA_SERVER
    if (this->cefInstance->GetMainFrame()->GetBrowser()->CanGoBack())
        this->cefInstance->GetMainFrame()->GetBrowser()->GoBack();
    else
        throw std::runtime_error("Cannot go backwards anymore"); // come up with a better error name. and should this even return void
#endif
#endif
}

void ChromiumFrame::forward()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::PageForward can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot go forwards on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    if (this->cefInstance->GetMainFrame()->GetBrowser()->CanGoForward())
        this->cefInstance->GetMainFrame()->GetBrowser()->GoForward();
    else
        throw std::runtime_error("Cannot go forwards anymore"); // come up with a better error name. and should this even return void
#endif
#endif
}

void ChromiumFrame::cancel()
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::CancelLoading can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot go forwards on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    this->cefInstance->GetMainFrame()->GetBrowser()->StopLoad();
#endif
#endif
}

void ChromiumFrame::onAncestorChanged(const AncestorChanged& event)
{
    GuiObject::onAncestorChanged(event);

#if !defined(AYA_SERVER) || !defined(ENABLE_CHROMIUM_FRAMES)
    // we uninit if it is null and/or if it is not a gui object of importance to us
    // why? because it doesn't make sense to keep playing if it is not being rendered...
    if (this->initialized &&
        !(Aya::Instance::fastDynamicCast<Aya::GuiObject>(event.newParent) || Aya::Instance::fastDynamicCast<Aya::GuiLayerCollector>(event.newParent)))
    {
        this->uninitialize();
    }
#endif
}

void ChromiumFrame::setLoadStateInternal(ChromiumLoadState value)
{
    if (this->loadState == value)
        return;

    loadState = value;

    raisePropertyChanged(prop_loadState);
    raisePropertyChanged(prop_isLoaded);

    if (value == ChromiumLoadState::Loaded)
        event_ChromiumFrameLoaded.fireEvent(this, this->getIsLoaded());
}

void ChromiumFrame::setStatusCodeInternal(int value)
{
    if (this->statusCode == value)
        return;

    statusCode = value;
    raisePropertyChanged(prop_statusCode);
}

void ChromiumFrame::setPageTitleInternal(std::string value)
{
    if (this->pageTitle == value)
        return;

    pageTitle = value;
    raisePropertyChanged(prop_pageTitle);
}

void ChromiumFrame::setInitializedInternal(bool value)
{
    if (this->initialized == value)
        return;

    initialized = value;
    raisePropertyChanged(prop_initialized);
}

void ChromiumFrame::setURL(std::string url)
{
    if (this->url == url)
        return;

    this->url = url;
    raisePropertyChanged(prop_url);

#if !defined(AYA_SERVER) || !defined(ENABLE_CHROMIUM_FRAMES)
    if (this->initialized)
    {
        this->cefInstance->GetMainFrame()->LoadURL(url);
    }
#endif
}

void ChromiumFrame::setURLInternal(std::string url)
{
    if (this->url == url)
        return;

    this->url = url;
    raisePropertyChanged(prop_url);
}

void ChromiumFrame::setCanPageForward(bool value)
{
    if (this->canPageForward == value)
        return;

    this->canPageForward = value;
    raisePropertyChanged(prop_canPageForward);
}

void ChromiumFrame::setCanPageBackward(bool value)
{
    if (this->canPageBackward == value)
        return;

    this->canPageBackward = value;
    raisePropertyChanged(prop_canPageBackward);
}

void ChromiumFrame::setResolution(Vector2 resolution)
{
    if (this->resolution == resolution)
        return;

    if (resolution.x <= 0 || resolution.y <= 0)
        throw std::runtime_error("The x/y resolution of a ChromiumFrame must be a positive integer");

    if (resolution.x * resolution.y > 35389440)
        throw std::runtime_error("ChromiumFrame resolution cannot currently exceed 8K Full Format (8192x4320)");

    this->resolution = resolution;
    raisePropertyChanged(prop_resolution);

#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
    if (this->initialized)
    {
        this->cefRenderer->UpdateResolution(resolution.x, resolution.y);
        this->cefInstance->GetHost()->WasResized();
    }
#endif
}

void ChromiumFrame::setMousePosition(Vector2 position)
{
    if (this->mousePosition == position)
        return;

    this->mousePosition = position;
    raisePropertyChanged(prop_mousePosition);

#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
    if (this->initialized)
    {
        CefMouseEvent mouseEvent;
        mouseEvent.x = this->mousePosition.x;
        mouseEvent.y = this->mousePosition.y;

        this->cefInstance->GetHost()->SendMouseMoveEvent(mouseEvent, false);
    }
#endif
}

void ChromiumFrame::setMouseCursorInternal(ChromiumMouseCursor cursor)
{
    if (this->mouseCursor == cursor)
        return;

    this->mouseCursor = cursor;
    raisePropertyChanged(prop_mouseCursor);

    event_ChromiumFrameMouseCursorChanged.fireEvent(this, cursor);
}

void ChromiumFrame::setScrollOffsetInternal(Vector2 offset)
{
    if (this->scrollOffset == offset)
        return;

    this->scrollOffset = offset;
    raisePropertyChanged(prop_scrollOffset);

    event_scrollOffsetChanged.fireEvent(this, offset);
}

std::string ChromiumFrame::runJS(std::string code)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::RunJS can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot evaluate JavaScript on a non-initialized ChromiumFrame");
#ifndef AYA_SERVER
    std::promise<std::string> prom;
    std::future<std::string> fut = prom.get_future();

    this->cefClient.get()->SetPromise(&prom);

    std::string wrappedCode = "sendResultToEngine((function() { try { " + code + "; } catch(e) { return 'Error: ' + e.message; }})());";
    this->cefInstance->GetMainFrame()->ExecuteJavaScript(wrappedCode, "", 0);

    // bye bye after 5 secs
    std::future_status status = fut.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::timeout)
    {
        throw std::runtime_error("JavaScript execution timed out.");
    }
    else if (status == std::future_status::ready)
    {
        return fut.get();
    }
    else
    {
        throw std::runtime_error("JavaScript execution failed.");
    }
#endif
#endif
}

#if defined(ENABLE_CHROMIUM_FRAMES) && !defined(AYA_SERVER)
void ChromiumFrame::mouseClick(int x, int y, cef_mouse_button_type_t button, bool isDown)
{
    CefMouseEvent mouseEvent;
    mouseEvent.x = x;
    mouseEvent.y = y;

    this->cefInstance->GetHost()->SendMouseClickEvent(mouseEvent, button, isDown, 1);
}
#endif

void ChromiumFrame::mouseLeftClick(int x, int y, bool isDown)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::MouseLeftClick can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot run inputs on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    this->mouseClick(x, y, MBT_LEFT, isDown);
#endif
#endif
}

void ChromiumFrame::mouseMiddleClick(int x, int y, bool isDown)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::MouseMiddleClick can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot run inputs on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    this->mouseClick(x, y, MBT_MIDDLE, isDown);
#endif
#endif
}

void ChromiumFrame::mouseRightClick(int x, int y, bool isDown)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::MouseRightClick can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot run inputs on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    this->mouseClick(x, y, MBT_RIGHT, isDown);
#endif
#endif
}

void ChromiumFrame::mouseWheel(int x, int y, int deltaX, int deltaY)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::MouseWheel can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot run inputs on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    CefMouseEvent mouseEvent;
    mouseEvent.x = x;
    mouseEvent.y = y;

    this->cefInstance->GetHost()->SendMouseWheelEvent(mouseEvent, deltaX, deltaY);
#endif
#endif
}

void ChromiumFrame::keyEvent(int keyCode, int modifiers, bool isKeyDown)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::KeyEvent can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot run inputs on a non-initialized ChromiumFrame");

#ifndef AYA_SERVER
    CefKeyEvent keyEvent;
    keyEvent.windows_key_code = keyCode;
    keyEvent.modifiers = modifiers;
    keyEvent.type = isKeyDown ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;

    this->cefInstance->GetHost()->SendKeyEvent(keyEvent);
#endif
#endif
}

Color3 ChromiumFrame::getColorAtPixel(int x, int y)
{
#ifndef ENABLE_CHROMIUM_FRAMES
    throw std::runtime_error("ChromiumFrames are not enabled in this build of Aya");
#else
    if (Network::Players::serverIsPresent(this))
        throw std::runtime_error("ChromiumFrame::GetColorAtPixel can only be called from the client");

    if (!this->initialized)
        throw std::runtime_error("Cannot get color at pixel on a non-initialized ChromiumFrame");

    if (x < 0 || x >= this->resolution.x || y < 0 || y >= this->resolution.y)
        throw std::runtime_error("Pixel out of bounds");

#ifndef AYA_SERVER
    int offset = (y * this->resolution.x + x) * 4;
    return Color3(this->buffer[offset + 0] / 255.0f, this->buffer[offset + 1] / 255.0f, this->buffer[offset + 2] / 255.0f);
#endif
#endif
}


} // namespace Aya
#endif