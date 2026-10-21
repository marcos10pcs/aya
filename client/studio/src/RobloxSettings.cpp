


#include "RobloxSettings.hpp"

// Qt Headers
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QDesktopServices>

// Roblox Headers
#include "DataModel/ContentProvider.hpp"

#include "DataModel/DebugSettings.hpp"

#include "Script/script.hpp"
#include "Utility/Statistics.hpp"

#include <QStandardPaths>

// Roblox Studio Headers
#include "RbxWorkspace.hpp"

#ifdef _WIN32
#include "RobloxStudioVersion.hpp"
#endif

AppSettings& AppSettings::instance()
{
    static AppSettings singleton;
    return singleton;
}

AppSettings::AppSettings()
    : m_bIsScriptAssetUploadEnabled(false)
    , m_bIsAnimationAssetUploadEnabled(false)
    , m_bIsImageModelAssetUploadEnabled(false)
{
    QString executableFolder = QApplication::applicationDirPath();
    QFile file(executableFolder + "/AppSettings.xml");
    if (file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc("appSettings");
        if (doc.setContent(&file))
        {
            QDomElement docElem = doc.documentElement();
            if (docElem.tagName() == "Settings")
            {
                QDomElement content = docElem.firstChildElement("ContentFolder");
                QString dummy = content.text();
                m_contentFolder = executableFolder + "/" + dummy;

                dummy.replace("content", "BuiltInPlugins");
                m_builtInPluginsFolder = executableFolder + "/" + dummy;

                content = docElem.firstChildElement("BaseUrl");
                m_baseURL = content.text();

                content = docElem.firstChildElement("IsScriptAssetUploadEnabled");
                m_bIsScriptAssetUploadEnabled = content.text() == "1";

                content = docElem.firstChildElement("IsAnimationAssetUploadEnabled");
                m_bIsAnimationAssetUploadEnabled = content.text() == "1";

                content = docElem.firstChildElement("IsImageModelAssetUploadEnabled");
                m_bIsImageModelAssetUploadEnabled = content.text() == "1";

                content = docElem.firstChildElement("CrashMenu");
                m_bShowStudioCrashMenu = content.text() == "1";
#ifdef STUDIO_ADMIN_BUILD
                content = docElem.firstChildElement("AdminKey");
                m_adminKey = content.text();
#endif
            }
        }
        file.close();
    }
    else
    {

#ifdef AYA_TEST_BUILD
        m_baseURL = "http://www.kiseki.lol";
#ifdef USE_PRIVATE_RESOURCES
        m_contentFolder = executableFolder + "/../../client/resources/content";
        m_builtInPluginsFolder = executableFolder + "/../../client/studio/resources/BuiltInPlugins";
#else
        m_contentFolder = executableFolder + "/content";
        m_builtInPluginsFolder = executableFolder + "/BuiltInPlugins";
#endif
#else
        m_baseURL = "http://www.kiseki.lol";
        m_contentFolder = executableFolder + "/content";
        m_builtInPluginsFolder = executableFolder + "/BuiltInPlugins";
#endif
    }

#ifdef _WIN32
    m_tempLocation = QDir::homePath() + "/AppData/Local/Roblox";
#else
    QStringList documentationPaths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    m_tempLocation = documentationPaths[0] + "/Roblox";
#endif
    if (!QFile::exists(m_tempLocation))
        QDir().mkpath(m_tempLocation);
}

RobloxSettings::RobloxSettings()
    : QSettings()
{
}

void RobloxSettings::saveAssets()
{
    RobloxSettings settings;
    settings.setValue("BaseUrl", QString(::GetBaseURL().c_str()));
    settings.setValue("ContentFolder", QString(Aya::ContentProvider::assetFolder().c_str()));
}

void RobloxSettings::recoverAssets()
{
    QString baseUrl = AppSettings::instance().baseURL();
    ::SetBaseURL(baseUrl.toStdString());

    QString assetFolder = AppSettings::instance().contentFolder();
    QByteArray assetFolderUtf8 = assetFolder.toUtf8();

    Aya::ContentProvider::setAssetFolder(std::string(assetFolderUtf8.data(), assetFolderUtf8.size()).c_str());
}

QString RobloxSettings::getAssetFolder()
{
    return AppSettings::instance().contentFolder();
}

QString RobloxSettings::getBuiltInPluginsFolder()
{
    return AppSettings::instance().builtInPluginsFolder();
}

QString RobloxSettings::getBaseURL()
{
    return AppSettings::instance().baseURL();
}

QString RobloxSettings::getApiBaseURL()
{
    return QString::fromStdString(Aya::ContentProvider::getUnsecureApiBaseUrl(getBaseURL().toStdString()));
}

#ifdef STUDIO_ADMIN_BUILD
QString RobloxSettings::getAdminKey()
{
    return AppSettings::instance().adminKey();
}
#endif

QString RobloxSettings::getVersionString()
{
    QString version;
#ifdef _WIN32
    version = VER_FILEVERSION_STR;
    version.replace(',', '.');
#else
    QString plistPath = QApplication::applicationDirPath() + "/../info.plist";
    QSettings settings(plistPath, QSettings::NativeFormat);
    version = settings.value("CFBundleShortVersionString").toString();
#endif
    Aya::DebugSettings::robloxVersion = version.toStdString();
    return version;
}

QString RobloxSettings::getTempLocation()
{
    return AppSettings::instance().tempLocation();
}

QString RobloxSettings::getResourcesFolder()
{
    QString resourcesFolder = QApplication::applicationDirPath();
#if defined(USE_PRIVATE_RESOURCES) && defined(AYA_TEST_BUILD)
    resourcesFolder += "/../../client/studio/resources";
#endif

    return resourcesFolder;
}

void RobloxSettings::initWorkspaceSettings()
{
    RbxWorkspace::isScriptAssetUploadEnabled = AppSettings::instance().isScriptAssetUploadEnabled();
    RbxWorkspace::isAnimationAssetUploadEnabled = AppSettings::instance().isAnimationAssetUploadEnabled();
    RbxWorkspace::isImageModelAssetUploadEnabled = AppSettings::instance().isImageModelAssetUploadEnabled();
}

bool RobloxSettings::showCrashMenu()
{
    return AppSettings::instance().showCrashMenu();
}
