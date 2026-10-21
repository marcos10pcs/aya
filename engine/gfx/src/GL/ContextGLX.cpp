#include "ContextGL.hpp"

#include "HeadersGL.hpp"

#include "aya/Debug.hpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

LOGGROUP(Graphics)

static int _XError_Handler(Display* dsp, XErrorEvent* error)
{
    char errorstring[128];
    XGetErrorText(dsp, error->error_code, errorstring, 128);

    printf("XError: %s\n", errorstring);
    return -1;
}

namespace Aya
{
namespace Graphics
{

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

class ContextGLX : public ContextGL
{
    Display* display;
    GLXContext context;
    GLXDrawable handle;

public:
    ContextGLX(void* windowHandle)
    {
        XSetErrorHandler(_XError_Handler);

        handle = (GLXDrawable)windowHandle;
        display = XOpenDisplay(NULL);
        static int visual_attribs[] = {GLX_X_RENDERABLE, true, GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_X_VISUAL_TYPE,
            GLX_TRUE_COLOR, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_DEPTH_SIZE, 24, GLX_STENCIL_SIZE, 8, GLX_DOUBLEBUFFER, true,
            None};
        int fbcount;
        GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
        if (!fbc)
        {
            printf("IM GOING TO DIE\n");
        }
        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
        for (int i = 0; i < fbcount; i++)
        {
            XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[i]);
            if (vi)
            {
                int samp_buf, samples, visual_id, red_size, blue_size, green_size, alpha_size, stencil_size;
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);
                glXGetFBConfigAttrib(display, fbc[i], GLX_VISUAL_ID, &visual_id);
                glXGetFBConfigAttrib(display, fbc[i], GLX_RED_SIZE, &red_size);
                glXGetFBConfigAttrib(display, fbc[i], GLX_BLUE_SIZE, &blue_size);
                glXGetFBConfigAttrib(display, fbc[i], GLX_GREEN_SIZE, &green_size);
                glXGetFBConfigAttrib(display, fbc[i], GLX_ALPHA_SIZE, &alpha_size);
                printf("  Matching fbconfig %d, visual ID 0x%x , SAMPLE_BUFFERS = %d,"
                       " SAMPLES = %d R:%d G:%d B:%d A%d\n",
                    i, visual_id, samp_buf, samples, red_size, blue_size, green_size, alpha_size);

                if ((best_fbc < 0 || samp_buf) && (samples > best_num_samp))
                    best_fbc = i, best_num_samp = samples;
                if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                    worst_fbc = i, worst_num_samp = samples;
            }
            XFree(vi);
        }

        XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[best_fbc]);
        glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
        GLXContext ctx_old = glXCreateContext(display, vi, 0, true);
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
        if (!glXCreateContextAttribsARB)
        {
            printf("glxCreateContrixAttribsARB not found falling back to old-style");
            context = ctx_old;
        }
        else
        {
            glXMakeCurrent(display, 0, 0);
            glXDestroyContext(display, ctx_old);

            static int context_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3, None};

            context = glXCreateContextAttribsARB(display, fbc[best_fbc], 0, true, context_attribs);
            if (!context)
            {
                context = glXCreateContext(display, vi, 0, true);
            }
        }
        XFree(fbc);

        if (!glXMakeCurrent(display, handle, context))
        {
            printf("glXMakeCurrent failed\n");
        }
        else
        {
            printf("glXMakeCurrent worked!!! :D\n");
        }

        glewExperimental = true;
        glewInitRBX();
    }

    virtual void setCurrent()
    {
        glXMakeCurrent(display, handle, context);
    }

    virtual void swapBuffers()
    {
        glXSwapBuffers(display, handle);
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
        Window root;
        int x, y;
        unsigned int w, h, border, depth;
        XGetGeometry(display, handle, &root, &x, &y, &w, &h, &border, &depth);
        return std::make_pair(w, h);
    }
};

ContextGL* ContextGL::create(void* windowHandle)
{
    return new ContextGLX(windowHandle);
}

} // namespace Graphics
} // namespace Aya
