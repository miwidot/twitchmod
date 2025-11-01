#include "channellist.h"
#include <QHeaderView>

ChannelList::ChannelList(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);

    // Tree widget for channels (mIRC-style)
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabel("Channels");
    m_treeWidget->setRootIsDecorated(true);
    m_treeWidget->setAnimated(true);

    // Create root categories
    m_moderatingRoot = new QTreeWidgetItem(m_treeWidget);
    m_moderatingRoot->setText(0, "Moderating");
    m_moderatingRoot->setExpanded(true);
    m_moderatingRoot->setForeground(0, QBrush(QColor(147, 61, 255))); // Twitch purple

    m_watchingRoot = new QTreeWidgetItem(m_treeWidget);
    m_watchingRoot->setText(0, "Watching");
    m_watchingRoot->setExpanded(true);

    // Join button
    m_joinButton = new QPushButton("+ Join Channel", this);

    layout->addWidget(m_treeWidget);
    layout->addWidget(m_joinButton);

    // Add some example channels
    addChannel("xqc", true);
    addChannel("shroud", true);
    addChannel("pokimane", false);

    // Connections
    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &ChannelList::onItemClicked);
    connect(m_joinButton, &QPushButton::clicked, this, &ChannelList::onJoinChannel);
}

void ChannelList::addChannel(const QString &channelName, bool isModerator)
{
    QTreeWidgetItem *parent = isModerator ? m_moderatingRoot : m_watchingRoot;

    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, "#" + channelName);
    item->setData(0, Qt::UserRole, channelName);

    if (isModerator) {
        item->setForeground(0, QBrush(QColor(0, 200, 0))); // Green for mod channels
    }
}

void ChannelList::removeChannel(const QString &channelName)
{
    // Search in both categories
    for (int i = 0; i < m_moderatingRoot->childCount(); ++i) {
        QTreeWidgetItem *item = m_moderatingRoot->child(i);
        if (item->data(0, Qt::UserRole).toString() == channelName) {
            delete m_moderatingRoot->takeChild(i);
            return;
        }
    }

    for (int i = 0; i < m_watchingRoot->childCount(); ++i) {
        QTreeWidgetItem *item = m_watchingRoot->child(i);
        if (item->data(0, Qt::UserRole).toString() == channelName) {
            delete m_watchingRoot->takeChild(i);
            return;
        }
    }
}

void ChannelList::clearChannels()
{
    m_moderatingRoot->takeChildren();
    m_watchingRoot->takeChildren();
}

void ChannelList::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    // Ignore clicks on root items
    if (item == m_moderatingRoot || item == m_watchingRoot) {
        return;
    }

    QString channelName = item->data(0, Qt::UserRole).toString();
    if (!channelName.isEmpty()) {
        emit channelSelected(channelName);
    }
}

void ChannelList::onJoinChannel()
{
    emit channelJoinRequested();
}
