

// Precompiled Header


// Qt Headers
#include <QApplication>
#include <QFile>
#include <QStringList>
#include <QMap>
#include <QApplication>

// Roblox Headers
#include "FastLog.hpp"
#include "RobloxServicesTools.hpp"


// Roblox Studio Headers
#include "RobloxApplicationManager.hpp"
#include "RobloxMainWindow.hpp"
#include "RobloxSettings.hpp"
#include "StudioUtilities.hpp"
#include "Utility/standardout.hpp"

// d9mz - logmanager will consistently be a huge pain in the ass
//        and we should make an alternative for it - a cli tool to view all flog output within a client?
#ifdef _WIN32
#include "VersionInfo.hpp"
#include "DumpErrorUploader.hpp"
// I can't fucking stand this header
#include "SharedLauncher.hpp"
#endif

#ifdef QT_ROBLOX_STUDIO
#include "SharedLauncher.hpp"
#endif

#ifdef Q_WS_MAC
#ifndef _DEBUG
#define ENABLE_BREAKPAD_CRASH_REPORTER
#endif
#endif

#ifdef ENABLE_APPLE_CRASH_REPORTER
#include "AppleCrashReporter.hpp"
#endif

#ifdef ENABLE_BREAKPAD_CRASH_REPORTER
#include "BreakpadCrashReporter.hpp"
#endif

#include "aya/Profiler.hpp"

#if ENABLE_CHROMIUM_FRAMES
#include "Utility/CefIntegration.hpp"
#endif

LOGGROUP(RobloxWndInit)

static bool UploadDmp = false;
static bool showVersion = false;

/**
 * Given an argument name, get the parameter for the argument.
 */
QString getArgParameter(const QStringList& args, const QString& arg)
{
    const int index = args.indexOf(arg) + 1;
    return index > 0 && index < args.size() ? args[index] : "";
}

/**
 * Gets the file name and script name from the commandline arguments.
 */
void parseCommandLineArgs(QMap<QString, QString>& argMap)
{
    const QStringList args = QApplication::arguments();

    // Helper lambda to check for an argument and optionally get its next value
    auto getArgValue = [&args](const QString& arg, bool nextValue = true) -> QString
    {
        int index = args.indexOf(arg);
        if (index != -1 && nextValue && index + 1 < args.size())
        {
            return args.at(index + 1);
        }
        return index != -1 ? "TRUE" : nullptr;
    };

    if (args.indexOf("--nochromium") == -1)
    {
#if defined(ENABLE_CHROMIUM_FRAMES)
        Aya::CefIntegration::initialize();
#endif
    }

    bool fetchLocalClientSettings = !(args.indexOf("--httpClientSettings") != -1 || args.indexOf("-S") != -1);
    SetFetchLocalClientSettings(fetchLocalClientSettings);
    Aya::StandardOut::singleton()->printf(Aya::MESSAGE_INFO, "SetFetchLocalClientSettings = %i", fetchLocalClientSettings);

#ifdef WIN32
    // Parsing arguments and filling the map
    argMap[SharedLauncher::FileLocationArgument] = getArgValue(SharedLauncher::FileLocationArgument);
    argMap[SharedLauncher::ScriptArgument] = getArgValue(SharedLauncher::ScriptArgument);
    argMap[SharedLauncher::BrowserTrackerId] = getArgValue(SharedLauncher::BrowserTrackerId);
    argMap[SharedLauncher::TestModeArgument] = getArgValue(SharedLauncher::TestModeArgument, false);
    argMap[SharedLauncher::IDEArgument] = getArgValue(SharedLauncher::IDEArgument, false);
    argMap[SharedLauncher::BuildArgument] = getArgValue(SharedLauncher::BuildArgument, false);
    argMap[SharedLauncher::AvatarModeArgument] = getArgValue(SharedLauncher::AvatarModeArgument, false);
    argMap[SharedLauncher::DebuggerArgument] = getArgValue(SharedLauncher::DebuggerArgument, false);
    argMap[SharedLauncher::StartEventArgument] = getArgValue(SharedLauncher::StartEventArgument);
    argMap[SharedLauncher::ReadyEventArgument] = getArgValue(SharedLauncher::ReadyEventArgument);
    argMap[SharedLauncher::ShowEventArgument] = getArgValue(SharedLauncher::ShowEventArgument);
    argMap[SharedLauncher::AuthUrlArgument] = getArgValue(SharedLauncher::AuthUrlArgument);
    argMap[SharedLauncher::AuthTicketArgument] = getArgValue(SharedLauncher::AuthTicketArgument);

    // Special handling for cases with one or two arguments which might be a file
    if (args.size() == 2 && QFile::exists(args[1]))
    {
        argMap[SharedLauncher::IDEArgument] = "TRUE";
        argMap[SharedLauncher::FileLocationArgument] = args[1];
    }
    else if (args.size() == 3 && (args[1] == SharedLauncher::IDEArgument || args[1] == SharedLauncher::FileLocationArgument) &&
             QFile::exists(args[2]))
    {
        argMap[SharedLauncher::IDEArgument] = "TRUE";
        argMap[SharedLauncher::FileLocationArgument] = args[2];
    }
#endif
}

#ifdef _WIN32

bool attachCurrentProcessToDebugger()
{
    return true;
}

#endif

class RobloxApplication : public QApplication
{
public:
    RobloxApplication(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        QPalette pal = this->palette();
        pal.setColor(QPalette::Window, Qt::white);
        this->setPalette(pal);

        QFont font = this->font();
        font.setHintingPreference(QFont::PreferNoHinting);
        setFont(font);
    }

    bool notify(QObject* receiver, QEvent* event) override
    {
        RBXPROFILER_SCOPE("Qt", "$Event");
        RBXPROFILER_LABELF("Qt", "%s::%d", receiver->objectName().toStdString().c_str(), event->type());

        return QApplication::notify(receiver, event);
    }
};

int main(int argc, char* argv[])
{
#ifdef _WIN32
    // This was compiled with sse2.  If this is placed too late, there is a good
    // chance some float operation will occur.
    if (!G3D::System::hasSSE2())
    {
        MessageBoxA(NULL, "This platform lacks SSE2 support.", "Aya", MB_OK);
        return false;
    }
    // CoInitialize(NULL);
#endif

    RobloxApplication app(argc, argv);
    Q_INIT_RESOURCE(RobloxStudio);

    // Platform independent settings
    QApplication::setOrganizationName("Kiseki");
    QApplication::setOrganizationDomain("kiseki.lol");
    QApplication::setApplicationName("KisekiStudio");
    QApplication::setApplicationVersion(RobloxSettings::getVersionString());

    // add plugins path for loading images in webpage
#ifdef _WIN32
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/imageformats");
#endif

    int retVal = -1;

    try
    {
        QMap<QString, QString> argMap;
        parseCommandLineArgs(argMap);

        RobloxApplicationManager::instance().registerApplication();
        RobloxMainWindow rbxMainWin(argMap);

        // Following is required so that when we launch from QProcess new instance of RobloxStudio should come to the top of Z-Order
        rbxMainWin.activateWindow();
        rbxMainWin.raise();

        retVal = app.exec();
    }

    catch (std::runtime_error const& exp)
    {
        FASTLOG1(FLog::RobloxWndInit, "Application closed due to an exception: '%s'", exp.what());
    }

#if defined(ENABLE_CHROMIUM_FRAMES)
    Aya::CefIntegration::shutdown();
#endif

    RobloxApplicationManager::instance().unregisterApplication();
    return retVal;
}
