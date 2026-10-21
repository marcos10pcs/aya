


#include "RobloxTutorials.hpp"

// Qt Headers
#include <QWidget>
#include <QGridLayout>
#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebEnginePage>
#include <QPushButton>
#include <QKeySequence>
#include <QSize>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QUrlQuery>
#include <QWebEngineUrlRequestInterceptor>

// Roblox Headers
#include "DataModel/FastLogSettings.hpp"


// Roblox Studio Headers
#include "RobloxBrowser.hpp"
#include "RobloxMainWindow.hpp"
#include "RobloxWebPage.hpp"
#include "QtUtilities.hpp"
#include "UpdateUIManager.hpp"
#include <QDesktopServices>

#define COLLAPSED_SIZE 142
#define EXPAND_THRESHOLD 300

FASTFLAG(WebkitDeveloperToolsEnabled);
FASTFLAGVARIABLE(StudioShowTutorialsByDefault, false);
FASTFLAGVARIABLE(StudioTutorialSeeAll, false);
FASTSTRINGVARIABLE(StudioTutorialsSeeAllUrl, "http://wiki.roblox.com/index.php?title=AllTutorials");
FASTSTRINGVARIABLE(StudioTutorialsUrl, "http://wiki.roblox.com/index.php?title=StudioTutorials&studiomode=true");
FASTSTRINGVARIABLE(StudioTutorialsTOCUrl, "http://wiki.roblox.com/index.php?title=Studio_Tutorials_Landing&studiomode=true");

class TutorialUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
public:
    void interceptRequest(QWebEngineUrlRequestInfo& info) override
    {
        /*
        if (info.requestUrl().host() != "kiseki.lol") {
            QDesktopServices::openUrl(info.requestUrl());
            info.block(true);
        }
        */
    }
};

RobloxTutorials::RobloxTutorials()
    : m_pWebView(NULL)
    , m_pWrapperWidget(NULL)
    , m_pWebContentsView(NULL)
    , m_firstPaint(true)
    , m_firstClick(true)
{
    if (!FFlag::StudioShowTutorialsByDefault)
        return;

    m_pWrapperWidget = new QWidget(this);

    setupWebView(m_pWrapperWidget);

    QVBoxLayout* layout = new QVBoxLayout(m_pWrapperWidget);
    layout->addWidget(m_pWebContentsView);
    layout->addWidget(m_pWebView);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

void RobloxTutorials::handleHomeClicked()
{
    m_pWebView->load(QUrl(FString::StudioTutorialsUrl.c_str()));
}

void RobloxTutorials::updateButtons()
{
    m_pStopButton->setEnabled(m_pWebView->pageAction(QWebEnginePage::Stop)->isEnabled());
    m_pReloadButton->setEnabled(m_pWebView->pageAction(QWebEnginePage::Reload)->isEnabled());
    m_pBackButton->setEnabled(m_pWebView->pageAction(QWebEnginePage::Back)->isEnabled());
    m_pForwardButton->setEnabled(m_pWebView->pageAction(QWebEnginePage::Forward)->isEnabled());
}

void RobloxTutorials::resizeWidget()
{
    QWidget* dockWidget = dynamic_cast<QWidget*>(this->parent());
    setMaximumHeight(QWIDGETSIZE_MAX);
    dockWidget->setMinimumHeight(COLLAPSED_SIZE);
    dockWidget->setMaximumHeight(QWIDGETSIZE_MAX);
}

void RobloxTutorials::disableLinks(const QUrl& url) {}

void RobloxTutorials::urlChanged(const QUrl& url)
{
    QString urlString = url.toString();

    int strCmp = urlString.compare(QString(FString::StudioTutorialsTOCUrl.c_str()));
    if (strCmp == 0)
    {
        m_firstClick = true;
    }
}

void RobloxTutorials::linkClicked(const QUrl& url)
{
    if (m_firstClick)
    {
        m_firstClick = false;
        m_pWebContentsView->load(QUrl(FString::StudioTutorialsUrl.c_str()));
    }
    else
    {
        // Check if "See All" link is clicked.
        QString urlString = url.toString();
        if (FFlag::StudioTutorialSeeAll && urlString.endsWith("Link="))
        {
            /*
            QUrl allTutorials(FString::StudioTutorialsSeeAllUrl.c_str(), QUrl::TolerantMode);
            QDesktopServices::openUrl(allTutorials);
            return;
            */
        }

        if (height() < EXPAND_THRESHOLD)
        {
            QWidget* dockWidget = dynamic_cast<QWidget*>(parent());
            dockWidget->setFixedHeight(UpdateUIManager::Instance().getMainWindow().size().height() / 2);
            QMetaObject::invokeMethod(this, "resizeWidget", Qt::QueuedConnection);
        }

        QUrl studioModeUrl = url;
        QUrlQuery query(studioModeUrl);
        if (!query.hasQueryItem("studiomode"))
            query.addQueryItem("studiomode", "true");
        studioModeUrl.setQuery(query);
        m_pWebView->load(studioModeUrl);
    }
}

void RobloxTutorials::setupWebView(QWidget* wrapperWidget)
{
    m_pWebView = new RobloxBrowser(wrapperWidget);
    m_pWebView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    m_pWebView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pWebView->setPage(new RobloxWebPage(wrapperWidget));

    m_pWebContentsView = new RobloxBrowser(wrapperWidget);
    m_pWebContentsView->setMaximumSize(QWIDGETSIZE_MAX, COLLAPSED_SIZE);
    m_pWebContentsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pWebContentsView->setPage(new RobloxWebPage(wrapperWidget));

    setMaximumHeight(COLLAPSED_SIZE);

    auto* interceptor = new TutorialUrlRequestInterceptor();
    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();
    profile->setUrlRequestInterceptor(interceptor);

    // Adjust link delegation for QWebEnginePage
    // connect(m_pWebView->page(), &QWebEnginePage::linkClicked, this, &RobloxTutorials::linkClicked);
    // connect(m_pWebContentsView->page(), &QWebEnginePage::linkClicked, this, &RobloxTutorials::linkClicked);

    m_pWebContentsView->load(QUrl(FString::StudioTutorialsTOCUrl.c_str()));

    QWebEngineSettings* globalSettings = QWebEngineProfile::defaultProfile()->settings();
    globalSettings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    globalSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    globalSettings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);

#ifdef _WIN32
    globalSettings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
#endif
}
