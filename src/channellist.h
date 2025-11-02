#ifndef CHANNELLIST_H
#define CHANNELLIST_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>

class ChannelList : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelList(QWidget *parent = nullptr);

    void addChannel(const QString &channelName, bool isModerator = false);
    void removeChannel(const QString &channelName);
    void clearChannels();

    // Channel persistence
    void saveChannels();
    void loadChannels();

signals:
    void channelSelected(const QString &channelName);
    void channelJoinRequested();

private slots:
    void onItemClicked(QTreeWidgetItem *item, int column);
    void onJoinChannel();
    void showContextMenu(const QPoint &pos);
    void onRemoveChannel();

private:
    QTreeWidget *m_treeWidget;
    QPushButton *m_joinButton;

    QTreeWidgetItem *m_moderatingRoot;
    QTreeWidgetItem *m_watchingRoot;
};

#endif // CHANNELLIST_H
