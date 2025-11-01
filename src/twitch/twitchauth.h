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
    // TWITCH_CLIENT_ID (Device Code Grant Flow!)
    static QString getClientId();

signals:
    void authenticationStarted();
    void authenticationSucceeded(const QString &username);
    void authenticationFailed(const QString &error);
    void deviceCodeReady(const QString &userCode, const QString &verificationUri);

private slots:
    void onDeviceCodeReceived();
    void onTokenPollResponse();
    void onValidateReplyFinished();

private:
    void validateToken();
    void startTokenPolling();
    void pollForToken();

    QNetworkAccessManager *m_networkManager;
    OAuthServer *m_oauthServer;  // Not used anymore but keeping for compatibility

    QString m_accessToken;
    QString m_refreshToken;
    QString m_userId;
    QString m_username;
    bool m_authenticated;

    // Device Code Grant Flow variables
    QString m_deviceCode;
    int m_pollingInterval;
};

#endif // TWITCHAUTH_H
