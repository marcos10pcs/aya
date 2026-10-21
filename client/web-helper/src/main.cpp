#include <include/base/cef_bind.h>
#include <include/cef_app.h>
#include <include/cef_base.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_frame_handler.h>
#include <include/cef_render_process_handler.h>
#include <include/wrapper/cef_closure_task.h>

class AyaCEFApp : public CefApp
{

public:
    AyaCEFApp () {};
private:
    IMPLEMENT_REFCOUNTING(AyaCEFApp);

};

class AyaSubProcess 
    : public AyaCEFApp 
    , public CefRenderProcessHandler
    , public CefV8Handler
{

public:
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) override
    {
        CefRefPtr<CefV8Value> global = context->GetGlobal();
        CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("sendResultToEngine", this);
        global->SetValue("sendResultToEngine", func, V8_PROPERTY_ATTRIBUTE_NONE);
    }

    bool Execute(const CefString& name,
                                CefRefPtr<CefV8Value> object,
                                const CefV8ValueList& arguments,
                                CefRefPtr<CefV8Value>& retval,
                                CefString& exception) override
    {
        // this should start to become a tuple of some kind because this is
        // shit if we want to let users have more control over js w/ lua

        // but this is good enough as a poc & can be used for replication
        // across different clients connected to a server
        if (name == "sendResultToEngine" && arguments.size() == 1)
        {
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("resultMessage");

            CefRefPtr<CefListValue> args = msg->GetArgumentList();
            args->SetString(0, arguments[0]->GetStringValue());

            CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
            CefRefPtr<CefFrame> frame = browser->GetMainFrame();

            frame->SendProcessMessage(PID_BROWSER, msg);

            return true;
        }

        return false;
    }

private:
    IMPLEMENT_REFCOUNTING(AyaSubProcess);

};

#ifdef WIN32
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    CefMainArgs args(hInstance);
    return CefExecuteProcess(args, new AyaSubProcess(), nullptr);
}
#else
int main(int argc, char* argv[])
{
    CefMainArgs args(argc, argv);
    return CefExecuteProcess(args, new AyaSubProcess(), nullptr);
}
#endif