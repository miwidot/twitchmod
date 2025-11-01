#ifndef TWITCHAPI_H
#define TWITCHAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QJsonObject>

class TwitchAPI : public QObject
{
    Q_OBJECT

public:
    explicit TwitchAPI(QObject *parent = nullptr);

    void setAccessToken(const QString &token);
    void setClientId(const QString &clientId);

    // Moderation API calls
    void banUser(const QString &broadcasterId, const QString &moderatorId,
                const QString &userId, const QString &reason = "");
    void unbanUser(const QString &broadcasterId, const QString &moderatorId,
                  const QString &userId);
    void timeoutUser(const QString &broadcasterId, const QString &moderatorId,
                    const QString &userId, int durationSeconds, const QString &reason = "");
    void deleteMessage(const QString &broadcasterId, const QString &moderatorId,
                      const QString &messageId);

    // Chat settings
    void getChatSettings(const QString &broadcasterId);
    void updateChatSettings(const QString &broadcasterId, const QString &moderatorId,
                           const QJsonObject &settings);

    // Predictions
    void createPrediction(const QString &broadcasterId, const QString &title,
                         const QStringList &outcomes, int durationSeconds);
    void endPrediction(const QString &broadcasterId, const QString &predictionId,
                      const QString &status, const QString &winningOutcomeId = "");
    void getPredictions(const QString &broadcasterId);

    // Polls
    void createPoll(const QString &broadcasterId, const QString &title,
                   const QStringList &choices, int durationSeconds);
    void endPoll(const QString &broadcasterId, const QString &pollId,
                const QString &status);
    void getPolls(const QString &broadcasterId);

    // User info
    void getUsers(const QStringList &userIds);
    void getModerators(const QString &broadcasterId);
    void getChatters(const QString &broadcasterId, const QString &moderatorId);

signals:
    void requestCompleted(const QString &endpoint, const QJsonObject &response);
    void requestFailed(const QString &endpoint, const QString &error);

private slots:
    void onReplyFinished();

private:
    void makeRequest(const QString &method, const QString &endpoint,
                    const QJsonObject &body = QJsonObject());

    QNetworkAccessManager *m_networkManager;
    QString m_accessToken;
    QString m_clientId;

    static constexpr const char* API_BASE_URL = "https://api.twitch.tv/helix";
};

#endif // TWITCHAPI_H
