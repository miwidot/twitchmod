#include "userlist.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

UserList::UserList(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);

    // Header label
    m_headerLabel = new QLabel("Users (0)", this);
    m_headerLabel->setStyleSheet("font-weight: bold; color: #9147ff;");

    // List widget
    m_listWidget = new QListWidget(this);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listWidget->setSortingEnabled(true);

    layout->addWidget(m_headerLabel);
    layout->addWidget(m_listWidget);

    // Add some example users
    addUser("mod_user", true, false);
    addUser("vip_user", false, true);
    addUser("viewer1", false, false);
    addUser("viewer2", false, false);
    addUser("viewer3", false, false);

    setUserCount(5);

    // Context menu connection
    connect(m_listWidget, &QListWidget::customContextMenuRequested,
            this, &UserList::onUserContextMenu);
}

void UserList::addUser(const QString &username, bool isModerator, bool isVip)
{
    QString displayName = username;

    // Add badges (mIRC-style)
    if (isModerator) {
        displayName = "@" + displayName; // Moderator badge
    } else if (isVip) {
        displayName = "+" + displayName; // VIP badge
    }

    QListWidgetItem *item = new QListWidgetItem(displayName);
    item->setData(Qt::UserRole, username); // Store actual username

    // Color coding
    if (isModerator) {
        item->setForeground(QBrush(QColor(0, 200, 0))); // Green for mods
    } else if (isVip) {
        item->setForeground(QBrush(QColor(255, 0, 255))); // Magenta for VIPs
    }

    m_listWidget->addItem(item);
}

void UserList::removeUser(const QString &username)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem *item = m_listWidget->item(i);
        if (item->data(Qt::UserRole).toString() == username) {
            delete m_listWidget->takeItem(i);
            break;
        }
    }
}

void UserList::clearUsers()
{
    m_listWidget->clear();
}

void UserList::setUserCount(int count)
{
    m_headerLabel->setText(QString("Users (%1)").arg(count));
}

QString UserList::getSelectedUsername() const
{
    QListWidgetItem *item = m_listWidget->currentItem();
    if (item) {
        return item->data(Qt::UserRole).toString();
    }
    return QString();
}

void UserList::onUserContextMenu(const QPoint &pos)
{
    QString username = getSelectedUsername();
    if (username.isEmpty()) {
        return;
    }

    // Create context menu (mIRC-style mod actions)
    QMenu menu(this);

    QAction *viewInfoAction = menu.addAction("View User Info");
    menu.addSeparator();

    QMenu *timeoutMenu = menu.addMenu("Timeout");
    QAction *timeout1m = timeoutMenu->addAction("1 minute");
    QAction *timeout5m = timeoutMenu->addAction("5 minutes");
    QAction *timeout10m = timeoutMenu->addAction("10 minutes");
    QAction *timeout30m = timeoutMenu->addAction("30 minutes");

    QAction *banAction = menu.addAction("Ban User");
    menu.addSeparator();

    QAction *deleteMessagesAction = menu.addAction("Delete Recent Messages");
    menu.addSeparator();

    QAction *copyUsernameAction = menu.addAction("Copy Username");

    // Execute menu
    QAction *selectedAction = menu.exec(m_listWidget->mapToGlobal(pos));

    // Handle actions
    if (selectedAction == viewInfoAction) {
        emit userInfoRequested(username);
        QMessageBox::information(this, "User Info",
                                QString("User: %1\n\n"
                                       "This will show:\n"
                                       "- Account creation date\n"
                                       "- Follow status\n"
                                       "- Previous bans/timeouts\n"
                                       "- Message history").arg(username));
    }
    else if (selectedAction == timeout1m) {
        emit userTimeoutRequested(username, 60);
    }
    else if (selectedAction == timeout5m) {
        emit userTimeoutRequested(username, 300);
    }
    else if (selectedAction == timeout10m) {
        emit userTimeoutRequested(username, 600);
    }
    else if (selectedAction == timeout30m) {
        emit userTimeoutRequested(username, 1800);
    }
    else if (selectedAction == banAction) {
        emit userBanRequested(username);
    }
    else if (selectedAction == deleteMessagesAction) {
        QMessageBox::information(this, "Delete Messages",
                                QString("Deleting recent messages from %1").arg(username));
    }
    else if (selectedAction == copyUsernameAction) {
        QApplication::clipboard()->setText(username);
    }
}
