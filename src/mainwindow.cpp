#include "mainwindow.h"
#include "channellist.h"
#include "chatwidget.h"
#include "userlist.h"
#include "twitch/twitchauth.h"
#include "twitch/twitchapi.h"

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_twitchAuth(new TwitchAuth(this))
    , m_twitchAPI(new TwitchAPI(this))
{
    setWindowTitle("TwitchMod - Twitch Moderator Client");
    resize(1280, 720);

    createMenuBar();
    createLayout();
    setupConnections();

    // Connect auth signals
    connect(m_twitchAuth, &TwitchAuth::authenticationStarted,
            this, &MainWindow::onAuthenticationStarted);
    connect(m_twitchAuth, &TwitchAuth::authenticationSucceeded,
            this, &MainWindow::onAuthenticationSucceeded);
    connect(m_twitchAuth, &TwitchAuth::authenticationFailed,
            this, &MainWindow::onAuthenticationFailed);

    statusBar()->showMessage("Not connected - Click File > Connect to Twitch", 5000);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenuBar()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    m_connectAction = new QAction("&Connect to Twitch", this);
    m_connectAction->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(m_connectAction);

    m_disconnectAction = new QAction("&Disconnect", this);
    m_disconnectAction->setEnabled(false);
    fileMenu->addAction(m_disconnectAction);

    fileMenu->addSeparator();

    m_settingsAction = new QAction("&Settings", this);
    m_settingsAction->setShortcut(QKeySequence("Ctrl+,"));
    fileMenu->addAction(m_settingsAction);

    fileMenu->addSeparator();

    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    fileMenu->addAction(m_exitAction);

    // View Menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    QAction *toggleChannelsAction = viewMenu->addAction("Toggle Channel List");
    QAction *toggleUsersAction = viewMenu->addAction("Toggle User List");

    // Mods Menu
    QMenu *modsMenu = menuBar()->addMenu("&Mods");
    modsMenu->addAction("AutoMod Queue");
    modsMenu->addAction("Banned Users");
    modsMenu->addAction("Mod Actions Log");
    modsMenu->addSeparator();
    modsMenu->addAction("Create Poll");
    modsMenu->addAction("Create Prediction");

    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    m_aboutAction = new QAction("&About TwitchMod", this);
    helpMenu->addAction(m_aboutAction);
    helpMenu->addAction("Documentation");
}

void MainWindow::createLayout()
{
    // Main widget and layout
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Main horizontal splitter (3-panel mIRC style)
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Left panel: Channel list
    m_channelList = new ChannelList(this);

    // Center panel: Chat tabs
    m_chatTabs = new QTabWidget(this);
    m_chatTabs->setTabsClosable(true);
    m_chatTabs->setMovable(true);

    // Add a default chat widget
    ChatWidget *defaultChat = new ChatWidget(this);
    m_chatTabs->addTab(defaultChat, "Welcome");

    // Right panel: User list
    m_userList = new UserList(this);

    // Add widgets to splitter
    m_mainSplitter->addWidget(m_channelList);
    m_mainSplitter->addWidget(m_chatTabs);
    m_mainSplitter->addWidget(m_userList);

    // Set initial splitter sizes (mIRC-style proportions)
    // Left: 200px, Center: flexible, Right: 180px
    m_mainSplitter->setSizes(QList<int>() << 200 << 700 << 180);

    mainLayout->addWidget(m_mainSplitter);
}

void MainWindow::setupConnections()
{
    connect(m_connectAction, &QAction::triggered, this, &MainWindow::onConnectTwitch);
    connect(m_disconnectAction, &QAction::triggered, this, &MainWindow::onDisconnect);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettings);
    connect(m_exitAction, &QAction::triggered, this, &QApplication::quit);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    // Close tab on close button click
    connect(m_chatTabs, &QTabWidget::tabCloseRequested, [this](int index) {
        if (m_chatTabs->count() > 1) { // Keep at least one tab
            QWidget *widget = m_chatTabs->widget(index);
            m_chatTabs->removeTab(index);
            widget->deleteLater();
        }
    });
}

void MainWindow::onConnectTwitch()
{
    m_twitchAuth->startAuthentication();
}

void MainWindow::onDisconnect()
{
    // TODO: Implement disconnect logic
    m_connectAction->setEnabled(true);
    m_disconnectAction->setEnabled(false);
    statusBar()->showMessage("Disconnected from Twitch", 3000);
}

void MainWindow::onSettings()
{
    // TODO: Implement settings dialog
    QMessageBox::information(this, "Settings",
                             "Settings dialog will be implemented here.\n\n"
                             "You'll be able to configure:\n"
                             "- Theme and colors\n"
                             "- Keyboard shortcuts\n"
                             "- Mod action defaults\n"
                             "- Notification settings");
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About TwitchMod",
                      "<h3>TwitchMod v1.0.0</h3>"
                      "<p>A professional Twitch moderator client with mIRC-style interface.</p>"
                      "<p><b>Features:</b></p>"
                      "<ul>"
                      "<li>Multi-channel moderation</li>"
                      "<li>Full mod actions (ban, timeout, delete)</li>"
                      "<li>AutoMod queue management</li>"
                      "<li>Predictions and Polls support</li>"
                      "<li>Cross-platform (Windows/macOS)</li>"
                      "</ul>"
                      "<p>Built with Qt6 and C++</p>");
}

void MainWindow::onAuthenticationStarted()
{
    m_connectAction->setEnabled(false);
    statusBar()->showMessage("Waiting for authentication in browser...", 0);
}

void MainWindow::onAuthenticationSucceeded(const QString &username)
{
    m_connectAction->setEnabled(false);
    m_disconnectAction->setEnabled(true);

    // Set up API with auth token
    m_twitchAPI->setAccessToken(m_twitchAuth->getAccessToken());
    m_twitchAPI->setClientId(TwitchAuth::CLIENT_ID);

    statusBar()->showMessage("Connected as " + username, 5000);

    QMessageBox::information(this, "Success",
                           QString("Successfully authenticated as %1!\n\n"
                                  "You can now moderate your channels.").arg(username));
}

void MainWindow::onAuthenticationFailed(const QString &error)
{
    m_connectAction->setEnabled(true);
    m_disconnectAction->setEnabled(false);

    statusBar()->showMessage("Authentication failed", 3000);

    QMessageBox::warning(this, "Authentication Failed",
                        QString("Failed to authenticate with Twitch:\n\n%1").arg(error));
}
