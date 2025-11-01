#include "chatwidget.h"
#include <QDateTime>
#include <QScrollBar>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , m_channelName("Unknown")
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Chat display (read-only text edit with HTML support)
    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setStyleSheet(
        "QTextEdit {"
        "  background-color: #0e0e10;"
        "  color: #efeff1;"
        "  border: none;"
        "  font-family: 'Consolas', 'Courier New', monospace;"
        "  font-size: 13px;"
        "}"
    );

    // Message input (mIRC-style)
    m_messageInput = new QLineEdit(this);
    m_messageInput->setPlaceholderText("Send a message...");
    m_messageInput->setStyleSheet(
        "QLineEdit {"
        "  background-color: #18181b;"
        "  color: #efeff1;"
        "  border: 1px solid #464649;"
        "  padding: 8px;"
        "  font-size: 13px;"
        "}"
    );

    layout->addWidget(m_chatDisplay);
    layout->addWidget(m_messageInput);

    // Add welcome message
    addSystemMessage("Welcome to TwitchMod! Connect to start moderating.");

    // Connections
    connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatWidget::onSendMessage);
}

void ChatWidget::addMessage(const QString &username, const QString &message, const QColor &userColor)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");

    QString colorHex = userColor.name();
    QString html = QString("<span style='color: #999;'>[%1]</span> "
                          "<span style='color: %2; font-weight: bold;'>%3:</span> "
                          "<span style='color: #efeff1;'>%4</span>")
                      .arg(timestamp)
                      .arg(colorHex)
                      .arg(username)
                      .arg(message.toHtmlEscaped());

    m_chatDisplay->append(html);

    // Auto-scroll to bottom
    QScrollBar *scrollBar = m_chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::addSystemMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");

    QString html = QString("<span style='color: #999;'>[%1]</span> "
                          "<span style='color: #9147ff; font-style: italic;'>* %2</span>")
                      .arg(timestamp)
                      .arg(message.toHtmlEscaped());

    m_chatDisplay->append(html);

    // Auto-scroll to bottom
    QScrollBar *scrollBar = m_chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::clearChat()
{
    m_chatDisplay->clear();
}

void ChatWidget::setChannelName(const QString &channelName)
{
    m_channelName = channelName;
}

void ChatWidget::onSendMessage()
{
    QString message = m_messageInput->text().trimmed();

    if (message.isEmpty()) {
        return;
    }

    // Check if it's a mod command
    if (message.startsWith("/")) {
        addSystemMessage("Mod command: " + message);
        // TODO: Handle mod commands (ban, timeout, etc.)
    } else {
        // Send as regular message
        emit messageSent(message);
        // Echo to chat (in real implementation, wait for server confirmation)
        addMessage("You", message, QColor(147, 61, 255)); // Twitch purple
    }

    m_messageInput->clear();
}
