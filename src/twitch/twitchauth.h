#ifndef TWITCHAUTH_H
#define TWITCHAUTH_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QNetworkAccessManager>

class OAuthServer;

class TwitchAuth : public QObject
{
    Q_OBJECT

public:
    explicit TwitchAuth(QObject *parent = nullptr);

    // OAuth flow
    void startAuthentication();
    void handleAuthorizationCode(const QString &code);

    // Token management
    bool isAuthenticated() const;
    QString getAccessToken() const;
    QString getUserId() const;
    QString getUsername() const;

    // Scopes needed for moderator client
    static QStringList getRequiredScopes();

    // OAuth credentials - can be set via environment variable:
    // TWITCH_CLIENT_ID (no client secret needed with Implicit Grant!)
    static QString getClientId();
    static QString getRedirectUri();

    // Default redirect URI
    static constexpr const char* DEFAULT_REDIRECT_URI = "http://localhost:8080/callback";

signals:
    void authenticationStarted();
    void authenticationSucceeded(const QString &username);
    void authenticationFailed(const QString &error);

private slots:
    void onAuthCodeReceived(const QString &token);
    void onAuthError(const QString &error);
    void onValidateReplyFinished();

private:
    void validateToken();

    QNetworkAccessManager *m_networkManager;
    OAuthServer *m_oauthServer;

    QString m_accessToken;
    QString m_refreshToken;
    QString m_userId;
    QString m_username;
    bool m_authenticated;
};

#endif // TWITCHAUTH_H
