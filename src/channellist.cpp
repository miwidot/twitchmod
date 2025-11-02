#include "channellist.h"
#include <QHeaderView>
#include <QMenu>

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
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

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

    // Connections
    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &ChannelList::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &ChannelList::showContextMenu);
    connect(m_joinButton, &QPushButton::clicked, this, &ChannelList::onJoinChannel);

    // Load saved channels on startup
    loadChannels();

    // If no channels were loaded, add default
    if (m_moderatingRoot->childCount() == 0 && m_watchingRoot->childCount() == 0) {
        addChannel("miwitv", true);
    }
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

    // Save channels whenever a new one is added
    saveChannels();
}

void ChannelList::removeChannel(const QString &channelName)
{
    // Search in both categories
    for (int i = 0; i < m_moderatingRoot->childCount(); ++i) {
        QTreeWidgetItem *item = m_moderatingRoot->child(i);
        if (item->data(0, Qt::UserRole).toString() == channelName) {
            delete m_moderatingRoot->takeChild(i);
            saveChannels(); // Save after removing
            return;
        }
    }

    for (int i = 0; i < m_watchingRoot->childCount(); ++i) {
        QTreeWidgetItem *item = m_watchingRoot->child(i);
        if (item->data(0, Qt::UserRole).toString() == channelName) {
            delete m_watchingRoot->takeChild(i);
            saveChannels(); // Save after removing
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

void ChannelList::showContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = m_treeWidget->itemAt(pos);

    // Only show menu for channel items, not category headers
    if (!item || item == m_moderatingRoot || item == m_watchingRoot) {
        return;
    }

    QMenu contextMenu(this);
    QAction *removeAction = contextMenu.addAction("Remove Channel");

    connect(removeAction, &QAction::triggered, this, &ChannelList::onRemoveChannel);

    contextMenu.exec(m_treeWidget->mapToGlobal(pos));
}

void ChannelList::onRemoveChannel()
{
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    if (!item || item == m_moderatingRoot || item == m_watchingRoot) {
        return;
    }

    QString channelName = item->data(0, Qt::UserRole).toString();
    removeChannel(channelName);
}

void ChannelList::saveChannels()
{
    QSettings settings("TwitchMod", "TwitchMod");

    // Clear existing channels
    settings.beginGroup("channels");
    settings.remove("");
    settings.endGroup();

    // Save moderating channels
    QStringList moderatingChannels;
    for (int i = 0; i < m_moderatingRoot->childCount(); ++i) {
        QTreeWidgetItem *item = m_moderatingRoot->child(i);
        QString channelName = item->data(0, Qt::UserRole).toString();
        moderatingChannels.append(channelName);
    }
    settings.setValue("channels/moderating", moderatingChannels);

    // Save watching channels
    QStringList watchingChannels;
    for (int i = 0; i < m_watchingRoot->childCount(); ++i) {
        QTreeWidgetItem *item = m_watchingRoot->child(i);
        QString channelName = item->data(0, Qt::UserRole).toString();
        watchingChannels.append(channelName);
    }
    settings.setValue("channels/watching", watchingChannels);

    qDebug() << "Saved" << moderatingChannels.size() << "moderating and"
             << watchingChannels.size() << "watching channels";
}

void ChannelList::loadChannels()
{
    QSettings settings("TwitchMod", "TwitchMod");

    // Load moderating channels
    QStringList moderatingChannels = settings.value("channels/moderating").toStringList();
    for (const QString &channelName : moderatingChannels) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_moderatingRoot);
        item->setText(0, "#" + channelName);
        item->setData(0, Qt::UserRole, channelName);
        item->setForeground(0, QBrush(QColor(0, 200, 0))); // Green for mod channels
    }

    // Load watching channels
    QStringList watchingChannels = settings.value("channels/watching").toStringList();
    for (const QString &channelName : watchingChannels) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_watchingRoot);
        item->setText(0, "#" + channelName);
        item->setData(0, Qt::UserRole, channelName);
    }

    qDebug() << "Loaded" << moderatingChannels.size() << "moderating and"
             << watchingChannels.size() << "watching channels";
}
