#pragma once
#include "aya/intrusive_ptr_target.hpp"

#include <QWindow>
#include <QWidget>
#include <QEvent>
#include <QPoint>

#include "GrayChatBar.hpp"
#include "LauncherView.hpp"
#include "DataModel/GameBasicSettings.hpp"

// This will not work unless if it is made by an OgreWindow
class OgreWidget : public QWidget
{
    Q_OBJECT

    QPoint lastPosition;
    QPoint lastPositionFrac;
    bool firstClosure;
    bool disableClosures;
    Qt::MouseButtons oldMouseButtons;
    QWidget* ogreWindow;

    bool textInputCanGrabFocus;

    void sendMouseButtonEvents(QMouseEvent* event, QPoint delta, QPoint position);
    void onChatBarEnteredText(const QString& text);

public:
    explicit OgreWidget(QWidget* ogreWindow, QWidget* parent);

    bool event(QEvent* event);
    QWidget* getOgreWindow()
    {
        return ogreWindow;
    }

    bool setClosuresDisabled(bool b)
    {
        disableClosures = b;
    };
};

class OgreWindow : public QWidget
{
    Q_OBJECT

    OgreWidget* myOgreWidget;
    GrayChatBar* textInput;
    LauncherView* launcherView;

    std::thread rccThread;
    std::atomic<bool> runRcc;


    void launchGame(const QString& gameConfiguration, const int& virtualVersion);
    void stopAvatarView();
    void startAvatarView();
    void startRcc(const QString& serverConfiguration, int port, int virtualVersion, bool forceVirtualVersion);
    void wearAsset(int64_t assetId, const QString& category);

    static void rccThreadProc(std::atomic<bool>& runAtomic, std::string password, OgreWindow* window, int port, int virtualVersion);

    virtual void closeEvent(QCloseEvent* ev);

public:
    explicit OgreWindow(QWidget* parent);
    ~OgreWindow();
    void shutdownRcc();
    void closeGame(); // called by view gameverbs

    OgreWidget* getOgreWidget()
    {
        return myOgreWidget;
    };
    GrayChatBar* getTextInput()
    {
        return textInput;
    }
    LauncherView* getLauncherView()
    {
        return launcherView;
    }

    bool event(QEvent* event);
    void changeEvent(QEvent* event);
    void enterWidget();
    void hideOgreWidget();
    bool isShowingAvatarPage = false;

    void handleVirtualVersionChange(Aya::GameBasicSettings::VirtualVersion version);
};
