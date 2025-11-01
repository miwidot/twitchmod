#ifndef TWITCHWEBSOCKET_H
#define TWITCHWEBSOCKET_H

#include <QObject>
#include <QWebSocket>
#include <QString>

class TwitchWebSocket : public QObject
{
    Q_OBJECT

public:
    explicit TwitchWebSocket(QObject *parent = nullptr);
    ~TwitchWebSocket();

    void connect(const QString &accessToken);
    void disconnect();
    bool isConnected() const;

    // Subscribe to EventSub topics
    void subscribeToChat(const QString &broadcasterId);
    void subscribeToChatMessages(const QString &broadcasterId);

signals:
    void connected();
    void disconnected();
    void error(const QString &error);

    // Chat events
    void chatMessageReceived(const QString &channelName, const QString &username,
                            const QString &message, const QString &userId);
    void userBanned(const QString &channelName, const QString &username);
    void userTimedOut(const QString &channelName, const QString &username, int seconds);
    void messageDeleted(const QString &channelName, const QString &messageId);

    // Prediction/Poll events
    void predictionStarted(const QString &channelName, const QString &title);
    void predictionEnded(const QString &channelName, const QString &winningOutcome);
    void pollStarted(const QString &channelName, const QString &title);
    void pollEnded(const QString &channelName);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);

private:
    void sendWelcome();
    void sendKeepAlive();
    void handleMessage(const QJsonObject &messageObj);

    QWebSocket *m_webSocket;
    QString m_sessionId;
    bool m_isConnected;
};

#endif // TWITCHWEBSOCKET_H
