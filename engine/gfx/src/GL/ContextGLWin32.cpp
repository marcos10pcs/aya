#include "ContextGL.hpp"

#include "HeadersGL.hpp"

#include "aya/Debug.hpp"

#include <Windows.h>

#include <glew/wglew.hpp>
#include <glew/glew.hpp>
#include <imgui_impl_opengl3.h>

#include "Utility/standardout.hpp"

GLenum GLEWAPIENTRY glewInit();

FASTFLAG(DebugGraphicsGL)

namespace Aya
{
namespace Graphics
{
class ContextGLWin32 : public ContextGL
{
public:
    ContextGLWin32(void* windowHandle)
        : hwnd(reinterpret_cast<HWND>(windowHandle))
        , hdc(GetDC(hwnd))
        , hglrc(NULL)
    {
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pf = ChoosePixelFormat(hdc, &pfd);

        if (!pf)
            throw Aya::runtime_error("Error choosing pixel format: %x", GetLastError());

        if (!SetPixelFormat(hdc, pf, &pfd))
            throw Aya::runtime_error("Error setting pixel format: %x", GetLastError());

        hglrc = wglCreateContext(hdc);

        if (!hglrc)
            throw Aya::runtime_error("Error creating context: %x", GetLastError());

        if (!wglMakeCurrent(hdc, hglrc))
            throw Aya::runtime_error("Error changing context: %x", GetLastError());

        {
            // We need to initialize the context with wgl function, but you can only get the wgl function after you initialize the context.
            // Let's just reinitialize it.
            glewInit();

            // ImGui initialize
            // by @mdolli
            IMGUI_CHECKVERSION();
            
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            ImGui_ImplOpenGL3_Init();

            if (WGLEW_ARB_create_context)
            {
                wglMakeCurrent(NULL, NULL);
                wglDeleteContext(hglrc);

                const int attribs[] = {
                    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                    WGL_CONTEXT_MINOR_VERSION_ARB, 2,
                    0, 0
                };

                hglrc = wglCreateContextAttribsARB(hdc, NULL, attribs);

                if (!hglrc)
                    throw Aya::runtime_error("Error creating context: %x", GetLastError());

                if (!wglMakeCurrent(hdc, hglrc))
                    throw Aya::runtime_error("Error changing context: %x", GetLastError());
            }
        }

        glewExperimental = true;
        glewInitRBX();

        if (wglSwapIntervalEXT)
            wglSwapIntervalEXT(0);
    }

    ~ContextGLWin32()
    {
        if (wglGetCurrentContext() == hglrc)
            wglMakeCurrent(NULL, NULL);

        wglDeleteContext(hglrc);
    }

    virtual void setCurrent()
    {
        if (wglGetCurrentContext() != hglrc)
        {
            BOOL result = wglMakeCurrent(hdc, hglrc);
            RBXASSERT(result);
        }
    }

    virtual void swapBuffers()
    {
        RBXASSERT(wglGetCurrentContext() == hglrc);

        /*
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Kiseki Aya - Debug");
        ImGui::Text("This will have more stuff eventually!");
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/

        SwapBuffers(hdc);
    }

    virtual unsigned int getMainFramebufferId()
    {
        return 0;
    }

    virtual bool isMainFramebufferRetina()
    {
        return false;
    }

    virtual std::pair<unsigned int, unsigned int> updateMainFramebuffer(unsigned int width, unsigned int height)
    {
        RECT rect = {};
        GetClientRect(hwnd, &rect);

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.DisplaySize = ImVec2((float)width, (float)height);
        
        return std::make_pair(rect.right - rect.left, rect.bottom - rect.top);
    }

private:
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
};

ContextGL* ContextGL::create(void* windowHandle)
{
    return new ContextGLWin32(windowHandle);
}

} // namespace Graphics
} // namespace Aya
