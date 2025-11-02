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

void TwitchWebSocket::connect(const QString &accessToken, const QString &username)
{
    m_accessToken = accessToken;
    m_username = username.toLower(); // IRC requires lowercase

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
    QObject::connect(m_webSocket, &QWebSocket::errorOccurred,
                    this, &TwitchWebSocket::onError);

    // Connect to Twitch IRC WebSocket
    qDebug() << "Connecting to Twitch IRC...";
    m_webSocket->open(QUrl("wss://irc-ws.chat.twitch.tv:443"));
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

void TwitchWebSocket::joinChannel(const QString &channelName)
{
    if (!m_webSocket || !m_isConnected) {
        qWarning() << "Cannot join channel: not connected";
        return;
    }

    // IRC requires lowercase channel names with # prefix
    QString ircChannel = channelName.toLower();
    if (!ircChannel.startsWith("#")) {
        ircChannel = "#" + ircChannel;
    }

    qDebug() << "Joining channel:" << ircChannel;
    m_webSocket->sendTextMessage("JOIN " + ircChannel);
}

void TwitchWebSocket::partChannel(const QString &channelName)
{
    if (!m_webSocket || !m_isConnected) {
        qWarning() << "Cannot part channel: not connected";
        return;
    }

    // IRC requires lowercase channel names with # prefix
    QString ircChannel = channelName.toLower();
    if (!ircChannel.startsWith("#")) {
        ircChannel = "#" + ircChannel;
    }

    qDebug() << "Leaving channel:" << ircChannel;
    m_webSocket->sendTextMessage("PART " + ircChannel);
}

void TwitchWebSocket::sendMessage(const QString &channelName, const QString &message)
{
    if (!m_webSocket || !m_isConnected) {
        qWarning() << "Cannot send message: not connected";
        return;
    }

    // IRC requires lowercase channel names with # prefix
    QString ircChannel = channelName.toLower();
    if (!ircChannel.startsWith("#")) {
        ircChannel = "#" + ircChannel;
    }

    qDebug() << "Sending message to" << ircChannel << ":" << message;
    m_webSocket->sendTextMessage("PRIVMSG " + ircChannel + " :" + message);
}

void TwitchWebSocket::onConnected()
{
    qDebug() << "Connected to Twitch IRC, authenticating...";

    // Send PASS (OAuth token)
    m_webSocket->sendTextMessage("PASS oauth:" + m_accessToken);

    // Send NICK (username)
    m_webSocket->sendTextMessage("NICK " + m_username);

    // Request capabilities for tags, membership, and commands
    m_webSocket->sendTextMessage("CAP REQ :twitch.tv/membership twitch.tv/tags twitch.tv/commands");

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
    qDebug() << "IRC <<" << message;
    parseIrcMessage(message);
}

void TwitchWebSocket::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    QString errorString = m_webSocket ? m_webSocket->errorString() : "Unknown error";
    emit this->error(errorString);
}

void TwitchWebSocket::parseIrcMessage(const QString &message)
{
    // Handle PING - must respond with PONG to stay connected
    if (message.startsWith("PING")) {
        QString pongResponse = message;
        pongResponse.replace("PING", "PONG");
        m_webSocket->sendTextMessage(pongResponse);
        qDebug() << "IRC >>" << pongResponse;
        return;
    }

    // Parse IRC message format:
    // @tags :prefix COMMAND params :trailing

    QString tags;
    QString prefix;
    QString command;
    QString params;
    QString trailing;

    QString remaining = message;

    // Extract tags (optional)
    if (remaining.startsWith("@")) {
        int tagEnd = remaining.indexOf(" ");
        if (tagEnd != -1) {
            tags = remaining.mid(1, tagEnd - 1);
            remaining = remaining.mid(tagEnd + 1);
        }
    }

    // Extract prefix (optional)
    if (remaining.startsWith(":")) {
        int prefixEnd = remaining.indexOf(" ");
        if (prefixEnd != -1) {
            prefix = remaining.mid(1, prefixEnd - 1);
            remaining = remaining.mid(prefixEnd + 1).trimmed();
        }
    }

    // Extract trailing message (after :)
    int trailingStart = remaining.indexOf(" :");
    if (trailingStart != -1) {
        trailing = remaining.mid(trailingStart + 2);
        remaining = remaining.left(trailingStart);
    }

    // Extract command and params
    QStringList parts = remaining.split(" ", Qt::SkipEmptyParts);
    if (!parts.isEmpty()) {
        command = parts.first();
        if (parts.size() > 1) {
            params = parts.mid(1).join(" ");
        }
    }

    // Handle IRC commands
    if (command == "PRIVMSG") {
        // Extract username from prefix (user!user@user.tmi.twitch.tv)
        QString username = prefix.split("!").first();

        // Extract channel from params
        QString channel = params.trimmed();
        if (channel.startsWith("#")) {
            channel = channel.mid(1);
        }

        qDebug() << "Chat message in" << channel << "from" << username << ":" << trailing;
        emit chatMessageReceived(channel, username, trailing, "");

    } else if (command == "JOIN") {
        // User joined channel
        QString username = prefix.split("!").first();
        QString channel = params.isEmpty() ? trailing : params;
        if (channel.startsWith("#")) {
            channel = channel.mid(1);
        }
        qDebug() << username << "joined" << channel;
        emit userJoined(channel, username);

    } else if (command == "PART") {
        // User left channel
        QString username = prefix.split("!").first();
        QString channel = params.isEmpty() ? trailing : params;
        if (channel.startsWith("#")) {
            channel = channel.mid(1);
        }
        qDebug() << username << "left" << channel;
        emit userParted(channel, username);

    } else if (command == "CLEARCHAT") {
        // User banned or timed out
        QString channel = params.trimmed();
        if (channel.startsWith("#")) {
            channel = channel.mid(1);
        }

        if (!trailing.isEmpty()) {
            // User banned/timed out
            qDebug() << "User" << trailing << "cleared from" << channel;
            emit userBanned(channel, trailing);
        } else {
            // Entire chat cleared
            qDebug() << "Chat cleared in" << channel;
        }

    } else if (command == "CLEARMSG") {
        // Single message deleted
        QString channel = params.trimmed();
        if (channel.startsWith("#")) {
            channel = channel.mid(1);
        }
        qDebug() << "Message deleted in" << channel;
        emit messageDeleted(channel, "");

    } else if (command == "001") {
        // Welcome message - successfully authenticated
        qDebug() << "IRC authentication successful!";

    } else if (command == "353") {
        // NAMES list (list of users in channel)
        // Format: :server 353 nick = #channel :user1 user2 user3 ...
        // params contains: "nick = #channel"
        // trailing contains: "user1 user2 user3 ..."

        // Extract channel from params
        QStringList paramParts = params.split(" ", Qt::SkipEmptyParts);
        QString channel;
        for (const QString &part : paramParts) {
            if (part.startsWith("#")) {
                channel = part.mid(1); // Remove #
                break;
            }
        }

        if (!channel.isEmpty() && !trailing.isEmpty()) {
            // Parse user list from trailing (space-separated)
            QStringList usernames = trailing.split(" ", Qt::SkipEmptyParts);

            qDebug() << "NAMES for channel" << channel << ":" << usernames.size() << "users";

            // Emit userJoined for each user in the list
            for (const QString &username : usernames) {
                emit userJoined(channel, username);
            }
        }

    } else if (command == "366") {
        // End of NAMES list
        qDebug() << "End of user list";

    } else if (command == "CAP") {
        // Capabilities acknowledgment
        qDebug() << "Capabilities:" << params << trailing;

    } else {
        // Unknown command - log for debugging
        qDebug() << "IRC command:" << command << "params:" << params << "trailing:" << trailing;
    }
}
