#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);

    void addMessage(const QString &username, const QString &message, const QColor &userColor = QColor(255, 255, 255));
    void addSystemMessage(const QString &message);
    void clearChat();
    void setChannelName(const QString &channelName);

signals:
    void messageSent(const QString &message);

private slots:
    void onSendMessage();

private:
    QString m_channelName;

    QTextEdit *m_chatDisplay;
    QLineEdit *m_messageInput;
};

#endif // CHATWIDGET_H
