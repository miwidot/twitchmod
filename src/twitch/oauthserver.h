#ifndef OAUTHSERVER_H
#define OAUTHSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class OAuthServer : public QObject
{
    Q_OBJECT

public:
    explicit OAuthServer(QObject *parent = nullptr);
    ~OAuthServer();

    bool start(quint16 port = 8080);
    void stop();
    bool isListening() const;

signals:
    void authorizationCodeReceived(const QString &code);
    void authorizationError(const QString &error);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    void sendResponse(QTcpSocket *socket, const QString &html);

    QTcpServer *m_server;
};

#endif // OAUTHSERVER_H
