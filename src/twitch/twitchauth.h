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

    // TODO: Replace with your actual Twitch App credentials
    static constexpr const char* CLIENT_ID = "YOUR_CLIENT_ID_HERE";
    static constexpr const char* CLIENT_SECRET = "YOUR_CLIENT_SECRET_HERE";
    static constexpr const char* REDIRECT_URI = "http://localhost:8080/callback";

signals:
    void authenticationStarted();
    void authenticationSucceeded(const QString &username);
    void authenticationFailed(const QString &error);

private slots:
    void onAuthCodeReceived(const QString &code);
    void onAuthError(const QString &error);
    void onTokenReplyFinished();
    void onValidateReplyFinished();

private:
    void exchangeCodeForToken(const QString &code);
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
