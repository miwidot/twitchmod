#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>

class ChannelList;
class ChatWidget;
class UserList;
class TwitchAuth;
class TwitchAPI;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectTwitch();
    void onDisconnect();
    void onSettings();
    void onAbout();

    // Auth slots
    void onAuthenticationStarted();
    void onAuthenticationSucceeded(const QString &username);
    void onAuthenticationFailed(const QString &error);

private:
    void createMenuBar();
    void createLayout();
    void setupConnections();

    // UI Components (mIRC-style layout)
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;

    ChannelList *m_channelList;
    QTabWidget *m_chatTabs;
    UserList *m_userList;

    // Twitch components
    TwitchAuth *m_twitchAuth;
    TwitchAPI *m_twitchAPI;

    // Menu actions
    QAction *m_connectAction;
    QAction *m_disconnectAction;
    QAction *m_settingsAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
};

#endif // MAINWINDOW_H
