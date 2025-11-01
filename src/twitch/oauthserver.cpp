#include "oauthserver.h"
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>

OAuthServer::OAuthServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &OAuthServer::onNewConnection);
}

OAuthServer::~OAuthServer()
{
    stop();
}

bool OAuthServer::start(quint16 port)
{
    if (m_server->isListening()) {
        return true;
    }

    if (!m_server->listen(QHostAddress::LocalHost, port)) {
        return false;
    }

    return true;
}

void OAuthServer::stop()
{
    if (m_server->isListening()) {
        m_server->close();
    }
}

bool OAuthServer::isListening() const
{
    return m_server->isListening();
}

void OAuthServer::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (!socket) {
        return;
    }

    connect(socket, &QTcpSocket::readyRead, this, &OAuthServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void OAuthServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QString request = QString::fromUtf8(socket->readAll());

    // Parse HTTP request
    QStringList lines = request.split("\r\n");
    if (lines.isEmpty()) {
        socket->close();
        return;
    }

    QString requestLine = lines.first();
    QStringList parts = requestLine.split(" ");
    if (parts.size() < 2) {
        socket->close();
        return;
    }

    QString path = parts[1];
    QUrl url("http://localhost" + path);
    QUrlQuery query(url);

    // Check for authorization code
    if (query.hasQueryItem("code")) {
        QString code = query.queryItemValue("code");

        QString successHtml = R"(
<!DOCTYPE html>
<html>
<head>
    <title>TwitchMod - Authentication Successful</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background: linear-gradient(135deg, #9147ff 0%, #772ce8 100%);
            color: white;
        }
        .container {
            text-align: center;
            background: rgba(0, 0, 0, 0.3);
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }
        h1 { font-size: 32px; margin-bottom: 20px; }
        p { font-size: 18px; opacity: 0.9; }
        .checkmark {
            font-size: 64px;
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="checkmark">✓</div>
        <h1>Authentication Successful!</h1>
        <p>You can now close this window and return to TwitchMod.</p>
    </div>
</body>
</html>
)";

        sendResponse(socket, successHtml);
        emit authorizationCodeReceived(code);

    } else if (query.hasQueryItem("error")) {
        QString error = query.queryItemValue("error");
        QString errorDescription = query.queryItemValue("error_description", QUrl::FullyDecoded);

        QString errorHtml = R"(
<!DOCTYPE html>
<html>
<head>
    <title>TwitchMod - Authentication Failed</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background: linear-gradient(135deg, #ff4757 0%, #e84118 100%);
            color: white;
        }
        .container {
            text-align: center;
            background: rgba(0, 0, 0, 0.3);
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }
        h1 { font-size: 32px; margin-bottom: 20px; }
        p { font-size: 18px; opacity: 0.9; }
        .error-icon {
            font-size: 64px;
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="error-icon">✗</div>
        <h1>Authentication Failed</h1>
        <p>)" + errorDescription + R"(</p>
        <p>Please close this window and try again.</p>
    </div>
</body>
</html>
)";

        sendResponse(socket, errorHtml);
        emit authorizationError(error + ": " + errorDescription);
    }

    // Close connection after a short delay
    QTimer::singleShot(1000, socket, &QTcpSocket::close);
}

void OAuthServer::sendResponse(QTcpSocket *socket, const QString &html)
{
    if (!socket) {
        return;
    }

    QByteArray response;
    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: text/html; charset=utf-8\r\n");
    response.append("Content-Length: " + QByteArray::number(html.toUtf8().size()) + "\r\n");
    response.append("Connection: close\r\n");
    response.append("\r\n");
    response.append(html.toUtf8());

    socket->write(response);
    socket->flush();
}
