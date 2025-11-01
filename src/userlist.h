#ifndef USERLIST_H
#define USERLIST_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>

class UserList : public QWidget
{
    Q_OBJECT

public:
    explicit UserList(QWidget *parent = nullptr);

    void addUser(const QString &username, bool isModerator = false, bool isVip = false);
    void removeUser(const QString &username);
    void clearUsers();
    void setUserCount(int count);

signals:
    void userBanRequested(const QString &username);
    void userTimeoutRequested(const QString &username, int seconds);
    void userInfoRequested(const QString &username);

private slots:
    void onUserContextMenu(const QPoint &pos);

private:
    QLabel *m_headerLabel;
    QListWidget *m_listWidget;

    QString getSelectedUsername() const;
};

#endif // USERLIST_H
