#include "twitchapi.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>

TwitchAPI::TwitchAPI(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

void TwitchAPI::setAccessToken(const QString &token)
{
    m_accessToken = token;
}

void TwitchAPI::setClientId(const QString &clientId)
{
    m_clientId = clientId;
}

void TwitchAPI::banUser(const QString &broadcasterId, const QString &moderatorId,
                       const QString &userId, const QString &reason)
{
    QJsonObject body;
    body["data"] = QJsonObject{
        {"user_id", userId},
        {"reason", reason}
    };

    QString endpoint = QString("/moderation/bans?broadcaster_id=%1&moderator_id=%2")
                          .arg(broadcasterId, moderatorId);
    makeRequest("POST", endpoint, body);
}

void TwitchAPI::unbanUser(const QString &broadcasterId, const QString &moderatorId,
                         const QString &userId)
{
    QString endpoint = QString("/moderation/bans?broadcaster_id=%1&moderator_id=%2&user_id=%3")
                          .arg(broadcasterId, moderatorId, userId);
    makeRequest("DELETE", endpoint);
}

void TwitchAPI::timeoutUser(const QString &broadcasterId, const QString &moderatorId,
                           const QString &userId, int durationSeconds, const QString &reason)
{
    QJsonObject body;
    body["data"] = QJsonObject{
        {"user_id", userId},
        {"duration", durationSeconds},
        {"reason", reason}
    };

    QString endpoint = QString("/moderation/bans?broadcaster_id=%1&moderator_id=%2")
                          .arg(broadcasterId, moderatorId);
    makeRequest("POST", endpoint, body);
}

void TwitchAPI::deleteMessage(const QString &broadcasterId, const QString &moderatorId,
                             const QString &messageId)
{
    QString endpoint = QString("/moderation/chat?broadcaster_id=%1&moderator_id=%2&message_id=%3")
                          .arg(broadcasterId, moderatorId, messageId);
    makeRequest("DELETE", endpoint);
}

void TwitchAPI::getChatSettings(const QString &broadcasterId)
{
    QString endpoint = QString("/chat/settings?broadcaster_id=%1").arg(broadcasterId);
    makeRequest("GET", endpoint);
}

void TwitchAPI::updateChatSettings(const QString &broadcasterId, const QString &moderatorId,
                                  const QJsonObject &settings)
{
    QString endpoint = QString("/chat/settings?broadcaster_id=%1&moderator_id=%2")
                          .arg(broadcasterId, moderatorId);
    makeRequest("PATCH", endpoint, settings);
}

void TwitchAPI::createPrediction(const QString &broadcasterId, const QString &title,
                                const QStringList &outcomes, int durationSeconds)
{
    QJsonArray outcomesArray;
    for (const QString &outcome : outcomes) {
        outcomesArray.append(QJsonObject{{"title", outcome}});
    }

    QJsonObject body;
    body["broadcaster_id"] = broadcasterId;
    body["title"] = title;
    body["outcomes"] = outcomesArray;
    body["prediction_window"] = durationSeconds;

    makeRequest("POST", "/predictions", body);
}

void TwitchAPI::endPrediction(const QString &broadcasterId, const QString &predictionId,
                             const QString &status, const QString &winningOutcomeId)
{
    QJsonObject body;
    body["broadcaster_id"] = broadcasterId;
    body["id"] = predictionId;
    body["status"] = status; // "RESOLVED", "CANCELED", "LOCKED"

    if (!winningOutcomeId.isEmpty()) {
        body["winning_outcome_id"] = winningOutcomeId;
    }

    makeRequest("PATCH", "/predictions", body);
}

void TwitchAPI::getPredictions(const QString &broadcasterId)
{
    QString endpoint = QString("/predictions?broadcaster_id=%1").arg(broadcasterId);
    makeRequest("GET", endpoint);
}

void TwitchAPI::createPoll(const QString &broadcasterId, const QString &title,
                          const QStringList &choices, int durationSeconds)
{
    QJsonArray choicesArray;
    for (const QString &choice : choices) {
        choicesArray.append(QJsonObject{{"title", choice}});
    }

    QJsonObject body;
    body["broadcaster_id"] = broadcasterId;
    body["title"] = title;
    body["choices"] = choicesArray;
    body["duration"] = durationSeconds;

    makeRequest("POST", "/polls", body);
}

void TwitchAPI::endPoll(const QString &broadcasterId, const QString &pollId,
                       const QString &status)
{
    QJsonObject body;
    body["broadcaster_id"] = broadcasterId;
    body["id"] = pollId;
    body["status"] = status; // "TERMINATED" or "ARCHIVED"

    makeRequest("PATCH", "/polls", body);
}

void TwitchAPI::getPolls(const QString &broadcasterId)
{
    QString endpoint = QString("/polls?broadcaster_id=%1").arg(broadcasterId);
    makeRequest("GET", endpoint);
}

void TwitchAPI::getUsers(const QStringList &userIds)
{
    QString endpoint = "/users?";
    for (const QString &userId : userIds) {
        endpoint += "id=" + userId + "&";
    }
    makeRequest("GET", endpoint);
}

void TwitchAPI::getModerators(const QString &broadcasterId)
{
    QString endpoint = QString("/moderation/moderators?broadcaster_id=%1").arg(broadcasterId);
    makeRequest("GET", endpoint);
}

void TwitchAPI::getChatters(const QString &broadcasterId, const QString &moderatorId)
{
    QString endpoint = QString("/chat/chatters?broadcaster_id=%1&moderator_id=%2")
                          .arg(broadcasterId, moderatorId);
    makeRequest("GET", endpoint);
}

void TwitchAPI::makeRequest(const QString &method, const QString &endpoint,
                           const QJsonObject &body)
{
    QUrl url(API_BASE_URL + endpoint);
    QNetworkRequest request(url);

    // Set headers
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_accessToken).toUtf8());
    request.setRawHeader("Client-Id", m_clientId.toUtf8());

    QNetworkReply *reply = nullptr;

    if (method == "GET") {
        reply = m_networkManager->get(request);
    }
    else if (method == "POST") {
        QByteArray data = QJsonDocument(body).toJson();
        reply = m_networkManager->post(request, data);
    }
    else if (method == "PATCH") {
        QByteArray data = QJsonDocument(body).toJson();
        reply = m_networkManager->sendCustomRequest(request, "PATCH", data);
    }
    else if (method == "DELETE") {
        reply = m_networkManager->deleteResource(request);
    }

    if (reply) {
        reply->setProperty("endpoint", endpoint);
        connect(reply, &QNetworkReply::finished, this, &TwitchAPI::onReplyFinished);
    }
}

void TwitchAPI::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    QString endpoint = reply->property("endpoint").toString();
    QByteArray responseData = reply->readAll();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        emit requestCompleted(endpoint, doc.object());
    } else {
        emit requestFailed(endpoint, reply->errorString());
    }

    reply->deleteLater();
}
