#include "DummyWindow.hpp"
#include "Utility/standardout.hpp"
#include <SDL_video.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <boost/format.hpp>

Display* display;
Window window;

int _XError_Handler(Display* display, XErrorEvent* event)
{
    std::string str = boost::str(boost::format("XError: Type %1%, Serial: %2%, Request Code: %3%, Minor Code: %4%, Error Code: %5%") % event->type % event->serial % event->request_code % event->minor_code % event->error_code);
    printf("THERE WAS AN XERROR %s", str.c_str());
    throw std::runtime_error(str.c_str());
}
#endif

#ifdef _WIN32
WNDCLASS DefineWindow(WNDPROC winProc, LPCTSTR className, HINSTANCE hInst)
{
    WNDCLASS wndClass;

    wndClass.style = 0;                 // Windows Style (future article)
    wndClass.lpfnWndProc = winProc;     // The Window Procedure
    wndClass.cbClsExtra = 0;            // Extra memory for this class
    wndClass.cbWndExtra = 0;            // Extra memory for this window
    wndClass.hInstance = hInst;         // The definition of the instance of the application
    wndClass.hIcon = 0;                 // The icon in the upper left corner
    wndClass.hCursor = 0;               // ::LoadCursor(0, IDC_ARROW);   // The cursor for this window
    wndClass.hbrBackground = 0;         // (HBRUSH)(COLOR_WINDOW + 1);   // The background color for this window
    wndClass.lpszMenuName = 0;          // The name of the class for the menu for this window
    wndClass.lpszClassName = className; // The name of this class

    return wndClass;
}

HWND CreateDummyWindow(LPCTSTR caption, LPCTSTR className, HINSTANCE hInstance, int width, int height)
{
    return ::CreateWindow(className, caption, WS_DISABLED, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, unsigned int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hwnd, message, wParam, lParam);
}
#endif


DummyWindow::DummyWindow(int width, int height)
    : handle(0)
{
#ifdef _WIN32
    HINSTANCE hInst = GetModuleHandle(0);
    TCHAR className[] = "DummyWindow";

    WNDCLASS wndClass = DefineWindow(WindowProcedure, className, hInst);
    ::RegisterClass(&wndClass);

    handle = (HWND)(::CreateDummyWindow("", className, hInst, width, height));
#elif defined(__linux__)
    XSetErrorHandler(_XError_Handler);
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
    }

    handle = SDL_CreateWindow("Aya Render Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!handle)
    {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
#endif
}

DummyWindow::~DummyWindow(void)
{
#ifdef _WIN32
    ::DestroyWindow((HWND)handle);
#elif defined(__linux__)
    if (handle)
    {
        SDL_DestroyWindow((SDL_Window*)handle);
        handle = nullptr;
    }
#endif
}