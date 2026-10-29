#include "Application.hpp"
#include "InitializationError.hpp"
#include "FunctionMarshaller.hpp"
#include "Utility/Statistics.hpp"
#include "Utility/Http.hpp"
#include "DataModel/FastLogSettings.hpp"
#include "Utility/MicrophoneInput.hpp"
#include "Utility/CefIntegration.hpp"
#include "Graphics3D/CoordinateFrame.hpp"
#include <boost/format.hpp>
#include <stdlib.h>
#include <QSurfaceFormat>
#include <QApplication>
#include <QSettings>
#include "Window.hpp"

Aya::Application* appPtr;
QApplication* qtAppPtr;
OgreWindow* qtOgreWidget;

int main(int argc, char** argv)
{
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
        "--disable-frame-rate-limit --enable-gpu-rasterization --enable-zero-copy --enable-native-gpu-memory-buffers --disable-gpu-vsync");

#ifdef __linux
    setenv("QMLSCENE_DEVICE", "softwarecontext", 0); // FIXME: workaround
#endif

    QSurfaceFormat format;
    format.setSwapInterval(0);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication::setOrganizationName("Aya");
    QApplication::setOrganizationDomain("clipaai.tech");
    QApplication::setApplicationName("Aya");

#ifdef WIN32
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
#endif
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QApplication qtApp(argc, argv);
    qtAppPtr = &qtApp;

    QSettings siteSettings;
    QString configuredSite = siteSettings.value("JsHelpers/masterServerUrl").toString();
    if (configuredSite.isEmpty() || configuredSite.contains("kiseki.lol", Qt::CaseInsensitive))
        siteSettings.setValue("JsHelpers/masterServerUrl", "https://clipaai.tech");

    Aya::Application app;
    appPtr = &app;

    app.loadAppSettings();

    try
    {
        app.parseCommandLine(argc, argv);
    }
    catch (std::exception& e)
    {
        printf("app.parseCommandLine: %s\n", e.what());
    }

    std::string siteUrl = siteSettings.value("JsHelpers/masterServerUrl").toString().toStdString();
    if (siteUrl.back() != '/') siteUrl += '/';
    SetBaseURL(siteUrl);

    // need client settings here before we create window
    std::string clientSettingsString;
    try
    {
        Aya::Http(siteUrl + "api/clientappsettings/2016").get(clientSettingsString);
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Could not load 2016 client settings: %s\n", e.what());
    }
    // Apply client settings
    LoadClientSettingsFromString(CLIENT_APP_SETTINGS_STRING, clientSettingsString, &Aya::ClientAppSettings::singleton());

    OgreWindow widget(NULL);
    widget.setFocus();
    widget.show();
    widget.setWindowTitle("Aya");

    qtOgreWidget = &widget;

    try
    {
        app.Initialize(widget.getOgreWidget());
    }
    catch (const Aya::initialization_error& e)
    {
        printf("Error initailzie: %s", e.what());
        return -1;
    }

#if defined(ENABLE_CHROMIUM_FRAMES)
    if (app.getVm().count("nochromium") == 0)
    {
        Aya::CefIntegration::initialize();
    }
#endif
#if defined(ENABLE_VOICE_CHAT)
    Aya::MicrophoneInput::initialize();
#endif

    int r = -1;
    try
    {
        if (app.isGameReady())
            widget.enterWidget();
        else
            widget.hideOgreWidget();
        r = qtApp.exec();
    }
    catch (std::runtime_error const& exp)
    {
        printf("Error: %s\n", exp.what());
    }

    if (r != 0)
    {
        printf("Error: %d", r);
    }

    printf("Shutting down...\n");

    widget.shutdownRcc();
    app.shutdown();

#if defined(ENABLE_VOICE_CHAT)
    Aya::MicrophoneInput::shutdown();
#endif
#if defined(ENABLE_CHROMIUM_FRAMES)
    Aya::CefIntegration::shutdown();
#endif

    return r;
}
