#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMap>

class ChannelList;
class ChatWidget;
class UserList;
class TwitchAuth;
class TwitchAPI;
class TwitchWebSocket;

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
    void onDeviceCodeReady(const QString &userCode, const QString &verificationUri);
    void onAuthenticationSucceeded(const QString &username);
    void onAuthenticationFailed(const QString &error);

    // Prediction/Poll slots
    void onCreatePrediction();
    void onCreatePoll();

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

    // Channel to ChatWidget mapping
    QMap<QString, ChatWidget*> m_channelWidgets;

    // Current active channel for user list
    QString m_currentChannel;

    // Twitch components
    TwitchAuth *m_twitchAuth;
    TwitchAPI *m_twitchAPI;
    TwitchWebSocket *m_webSocket;

    // Menu actions
    QAction *m_connectAction;
    QAction *m_disconnectAction;
    QAction *m_settingsAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
};

#endif // MAINWINDOW_H
