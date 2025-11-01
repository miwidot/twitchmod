#include "twitchwebsocket.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

TwitchWebSocket::TwitchWebSocket(QObject *parent)
    : QObject(parent)
    , m_webSocket(nullptr)
    , m_isConnected(false)
{
}

TwitchWebSocket::~TwitchWebSocket()
{
    if (m_webSocket) {
        m_webSocket->close();
        delete m_webSocket;
    }
}

void TwitchWebSocket::connect(const QString &accessToken)
{
    Q_UNUSED(accessToken)

    if (m_webSocket) {
        delete m_webSocket;
    }

    m_webSocket = new QWebSocket();

    // Connect signals
    QObject::connect(m_webSocket, &QWebSocket::connected,
                    this, &TwitchWebSocket::onConnected);
    QObject::connect(m_webSocket, &QWebSocket::disconnected,
                    this, &TwitchWebSocket::onDisconnected);
    QObject::connect(m_webSocket, &QWebSocket::textMessageReceived,
                    this, &TwitchWebSocket::onTextMessageReceived);
    QObject::connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                    this, &TwitchWebSocket::onError);

    // Connect to Twitch EventSub WebSocket
    // TODO: Use actual EventSub WebSocket URL
    // m_webSocket->open(QUrl("wss://eventsub.wss.twitch.tv/ws"));

    // For now, stub implementation
    m_isConnected = false;
}

void TwitchWebSocket::disconnect()
{
    if (m_webSocket) {
        m_webSocket->close();
    }
    m_isConnected = false;
}

bool TwitchWebSocket::isConnected() const
{
    return m_isConnected;
}

void TwitchWebSocket::subscribeToChat(const QString &broadcasterId)
{
    // TODO: Subscribe to channel.chat_message EventSub topic
    Q_UNUSED(broadcasterId)
}

void TwitchWebSocket::subscribeToChatMessages(const QString &broadcasterId)
{
    // TODO: Subscribe to channel.chat.message EventSub topic
    Q_UNUSED(broadcasterId)
}

void TwitchWebSocket::onConnected()
{
    m_isConnected = true;
    emit connected();
}

void TwitchWebSocket::onDisconnected()
{
    m_isConnected = false;
    emit disconnected();
}

void TwitchWebSocket::onTextMessageReceived(const QString &message)
{
    // Parse JSON message
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        return;
    }

    QJsonObject messageObj = doc.object();
    handleMessage(messageObj);
}

void TwitchWebSocket::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    QString errorString = m_webSocket ? m_webSocket->errorString() : "Unknown error";
    emit this->error(errorString);
}

void TwitchWebSocket::sendWelcome()
{
    // TODO: Handle welcome message from Twitch EventSub
}

void TwitchWebSocket::sendKeepAlive()
{
    // TODO: Send keepalive message
}

void TwitchWebSocket::handleMessage(const QJsonObject &messageObj)
{
    // TODO: Parse EventSub message types:
    // - session_welcome
    // - session_keepalive
    // - notification (actual events)
    // - session_reconnect
    // - revocation

    QString messageType = messageObj["metadata"].toObject()["message_type"].toString();

    if (messageType == "session_welcome") {
        m_sessionId = messageObj["payload"].toObject()["session"].toObject()["id"].toString();
    }
    else if (messageType == "notification") {
        // Handle notification events
        QJsonObject payload = messageObj["payload"].toObject();
        QJsonObject event = payload["event"].toObject();

        // Example: Chat message event
        // emit chatMessageReceived(channelName, username, message, userId);
        Q_UNUSED(event)
    }
}
