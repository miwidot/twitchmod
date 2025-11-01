#include "twitchauth.h"
#include "oauthserver.h"
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QTimer>

TwitchAuth::TwitchAuth(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_oauthServer(new OAuthServer(this))
    , m_authenticated(false)
    , m_pollingInterval(5000)  // Default 5 seconds
{
    // OAuthServer not used in Device Code Grant Flow
}

QString TwitchAuth::getClientId()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString clientId = env.value("TWITCH_CLIENT_ID");

    if (clientId.isEmpty()) {
        qWarning("TWITCH_CLIENT_ID environment variable not set!");
        return QString();
    }

    return clientId;
}


QStringList TwitchAuth::getRequiredScopes()
{
    // All scopes needed for full moderator functionality
    return {
        // Chat
        "chat:read",
        "chat:edit",

        // Moderation
        "moderator:manage:banned_users",
        "moderator:read:banned_users",
        "moderator:manage:chat_messages",
        "moderator:read:chat_messages",
        "moderator:manage:chat_settings",
        "moderator:read:chat_settings",
        "moderator:read:chatters",

        // AutoMod
        "moderator:manage:automod",
        "moderator:read:automod_settings",
        "moderator:manage:automod_settings",
        "moderator:manage:blocked_terms",
        "moderator:read:blocked_terms",

        // Predictions & Polls
        "channel:manage:predictions",
        "channel:read:predictions",
        "channel:manage:polls",
        "channel:read:polls",

        // User info
        "user:read:email",
        "moderator:read:followers",
        "moderator:read:moderators",
        "moderator:read:vips",

        // Warnings & Unban requests
        "moderator:manage:warnings",
        "moderator:read:warnings",
        "moderator:manage:unban_requests",
        "moderator:read:unban_requests",

        // Shield mode
        "moderator:read:shield_mode",
        "moderator:manage:shield_mode",

        // Announcements
        "moderator:manage:announcements",

        // Shoutouts
        "moderator:read:shoutouts",
        "moderator:manage:shoutouts",
    };
}

void TwitchAuth::startAuthentication()
{
    emit authenticationStarted();

    // Device Code Grant Flow - Step 1: Request device code
    QString clientId = getClientId();
    if (clientId.isEmpty()) {
        emit authenticationFailed("TWITCH_CLIENT_ID environment variable not set");
        return;
    }

    QUrl url("https://id.twitch.tv/oauth2/device");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("client_id", clientId);
    params.addQueryItem("scopes", getRequiredScopes().join(" "));

    QNetworkReply *reply = m_networkManager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, &TwitchAuth::onDeviceCodeReceived);
}

void TwitchAuth::onDeviceCodeReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit authenticationFailed("Failed to get device code: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    QString deviceCode = obj["device_code"].toString();
    QString userCode = obj["user_code"].toString();
    QString verificationUri = obj["verification_uri"].toString();
    int expiresIn = obj["expires_in"].toInt();
    int interval = obj["interval"].toInt(5);

    reply->deleteLater();

    if (deviceCode.isEmpty() || userCode.isEmpty()) {
        emit authenticationFailed("Invalid device code response");
        return;
    }

    // Store device code and polling interval
    m_deviceCode = deviceCode;
    m_pollingInterval = interval * 1000; // Convert to milliseconds

    // Emit signal with user code and verification URI for UI to display
    emit deviceCodeReady(userCode, verificationUri);

    // Start polling for token
    startTokenPolling();
}

void TwitchAuth::startTokenPolling()
{
    QTimer::singleShot(m_pollingInterval, this, &TwitchAuth::pollForToken);
}

void TwitchAuth::pollForToken()
{
    QString clientId = getClientId();
    if (clientId.isEmpty() || m_deviceCode.isEmpty()) {
        return;
    }

    QUrl url("https://id.twitch.tv/oauth2/token");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("client_id", clientId);
    params.addQueryItem("device_code", m_deviceCode);
    params.addQueryItem("grant_type", "urn:ietf:params:oauth:grant-type:device_code");

    QNetworkReply *reply = m_networkManager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, &TwitchAuth::onTokenPollResponse);
}

void TwitchAuth::onTokenPollResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode == 200) {
        // Success! We got the token
        m_accessToken = obj["access_token"].toString();
        m_refreshToken = obj["refresh_token"].toString();

        reply->deleteLater();

        if (m_accessToken.isEmpty()) {
            emit authenticationFailed("Invalid token response");
            return;
        }

        // Validate token to get user info
        validateToken();

    } else if (statusCode == 400) {
        QString error = obj["error"].toString();

        if (error == "authorization_pending") {
            // User hasn't authorized yet, keep polling
            reply->deleteLater();
            startTokenPolling();

        } else if (error == "slow_down") {
            // We're polling too fast, increase interval
            m_pollingInterval += 1000;
            reply->deleteLater();
            startTokenPolling();

        } else if (error == "expired_token") {
            // Device code expired
            reply->deleteLater();
            emit authenticationFailed("Device code expired. Please try again.");

        } else if (error == "access_denied") {
            // User denied authorization
            reply->deleteLater();
            emit authenticationFailed("Authorization denied by user");

        } else {
            // Other error
            reply->deleteLater();
            emit authenticationFailed("Authorization failed: " + error);
        }
    } else {
        reply->deleteLater();
        emit authenticationFailed("Unexpected response from Twitch");
    }
}

void TwitchAuth::validateToken()
{
    QUrl url("https://id.twitch.tv/oauth2/validate");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("OAuth " + m_accessToken).toUtf8());

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &TwitchAuth::onValidateReplyFinished);
}

void TwitchAuth::onValidateReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit authenticationFailed("Token validation failed: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    m_userId = obj["user_id"].toString();
    m_username = obj["login"].toString();
    m_authenticated = true;

    reply->deleteLater();

    emit authenticationSucceeded(m_username);
}

bool TwitchAuth::isAuthenticated() const
{
    return m_authenticated;
}

QString TwitchAuth::getAccessToken() const
{
    return m_accessToken;
}

QString TwitchAuth::getUserId() const
{
    return m_userId;
}

QString TwitchAuth::getUsername() const
{
    return m_username;
}
