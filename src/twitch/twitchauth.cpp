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

TwitchAuth::TwitchAuth(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_oauthServer(new OAuthServer(this))
    , m_authenticated(false)
{
    connect(m_oauthServer, &OAuthServer::authorizationCodeReceived,
            this, &TwitchAuth::onAuthCodeReceived);
    connect(m_oauthServer, &OAuthServer::authorizationError,
            this, &TwitchAuth::onAuthError);
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

QString TwitchAuth::getRedirectUri()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString redirectUri = env.value("TWITCH_REDIRECT_URI", DEFAULT_REDIRECT_URI);
    return redirectUri;
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
    // Start local OAuth callback server
    if (!m_oauthServer->start(8080)) {
        emit authenticationFailed("Failed to start OAuth callback server on port 8080");
        return;
    }

    emit authenticationStarted();

    // Build OAuth URL (Implicit Grant Flow - no client secret needed!)
    QString clientId = getClientId();
    if (clientId.isEmpty()) {
        emit authenticationFailed("TWITCH_CLIENT_ID environment variable not set");
        return;
    }

    QUrl authUrl("https://id.twitch.tv/oauth2/authorize");
    QUrlQuery query;

    query.addQueryItem("client_id", clientId);
    query.addQueryItem("redirect_uri", getRedirectUri());
    query.addQueryItem("response_type", "token");  // Implicit Grant - token comes directly!
    query.addQueryItem("scope", getRequiredScopes().join(" "));

    authUrl.setQuery(query);

    // Open browser for user authentication
    QDesktopServices::openUrl(authUrl);
}

void TwitchAuth::handleAuthorizationCode(const QString &code)
{
    // No longer used with Implicit Grant Flow
    Q_UNUSED(code);
}

void TwitchAuth::onAuthCodeReceived(const QString &tokenOrCode)
{
    // With Implicit Grant, we receive the token directly
    // No need to exchange code for token!
    m_oauthServer->stop();
    m_accessToken = tokenOrCode;

    if (m_accessToken.isEmpty()) {
        emit authenticationFailed("Invalid token received");
        return;
    }

    // Validate token to get user info
    validateToken();
}

void TwitchAuth::onAuthError(const QString &error)
{
    m_oauthServer->stop();
    emit authenticationFailed(error);
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
