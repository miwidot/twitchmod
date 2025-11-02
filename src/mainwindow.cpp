#include "mainwindow.h"
#include "channellist.h"
#include "chatwidget.h"
#include "userlist.h"
#include "predictiondialog.h"
#include "polldialog.h"
#include "twitch/twitchauth.h"
#include "twitch/twitchapi.h"
#include "twitch/twitchwebsocket.h"

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_twitchAuth(new TwitchAuth(this))
    , m_twitchAPI(new TwitchAPI(this))
    , m_webSocket(new TwitchWebSocket(this))
{
    setWindowTitle("TwitchMod - Twitch Moderator Client");
    resize(1280, 720);

    createMenuBar();
    createLayout();
    setupConnections();

    // Connect auth signals
    connect(m_twitchAuth, &TwitchAuth::authenticationStarted,
            this, &MainWindow::onAuthenticationStarted);
    connect(m_twitchAuth, &TwitchAuth::deviceCodeReady,
            this, &MainWindow::onDeviceCodeReady);
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

    QAction *createPollAction = modsMenu->addAction("Create Poll");
    connect(createPollAction, &QAction::triggered, this, &MainWindow::onCreatePoll);

    QAction *createPredictionAction = modsMenu->addAction("Create Prediction");
    connect(createPredictionAction, &QAction::triggered, this, &MainWindow::onCreatePrediction);

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

    // Channel selection - join IRC channel and create tab
    connect(m_channelList, &ChannelList::channelSelected, [this](const QString &channelName) {
        qDebug() << "Channel selected:" << channelName;

        // Check if tab already exists
        if (m_channelWidgets.contains(channelName)) {
            // Switch to existing tab
            ChatWidget *existingWidget = m_channelWidgets[channelName];
            for (int i = 0; i < m_chatTabs->count(); ++i) {
                if (m_chatTabs->widget(i) == existingWidget) {
                    m_chatTabs->setCurrentIndex(i);
                    return;
                }
            }
        }

        // Create new chat tab
        ChatWidget *chatWidget = new ChatWidget(this);
        chatWidget->setChannelName(channelName);
        int tabIndex = m_chatTabs->addTab(chatWidget, "#" + channelName);
        m_chatTabs->setCurrentIndex(tabIndex);

        // Store widget mapping
        m_channelWidgets[channelName] = chatWidget;

        // Connect messageSent signal to send IRC message
        connect(chatWidget, &ChatWidget::messageSent, [this, channelName](const QString &message) {
            if (m_webSocket && m_webSocket->isConnected()) {
                m_webSocket->sendMessage(channelName, message);
            }
        });

        // Join IRC channel
        if (m_webSocket && m_webSocket->isConnected()) {
            m_webSocket->joinChannel(channelName);
        }

        // Update current channel for user list
        m_currentChannel = channelName;
        m_userList->clearUsers();
    });

    // Join Channel button handler
    connect(m_channelList, &ChannelList::channelJoinRequested, [this]() {
        bool ok;
        QString channelName = QInputDialog::getText(this, "Join Channel",
                                                     "Enter channel name:",
                                                     QLineEdit::Normal,
                                                     "", &ok);
        if (ok && !channelName.isEmpty()) {
            // Remove # if user typed it
            if (channelName.startsWith("#")) {
                channelName = channelName.mid(1);
            }

            // Add to channel list as "Watching"
            m_channelList->addChannel(channelName, false);

            // Auto-select it to join
            emit m_channelList->channelSelected(channelName);
        }
    });

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
    statusBar()->showMessage("Requesting device code from Twitch...", 0);
}

void MainWindow::onDeviceCodeReady(const QString &userCode, const QString &verificationUri)
{
    statusBar()->showMessage("Waiting for authorization...", 0);

    // Show dialog with activation instructions
    QString message = QString(
        "<h2>Twitch Authorization Required</h2>"
        "<p><b>Step 1:</b> Go to <a href=\"%1\">%1</a></p>"
        "<p><b>Step 2:</b> Enter this code:</p>"
        "<h1 style=\"color: #9147ff; font-family: monospace; letter-spacing: 5px;\">%2</h1>"
        "<p><i>Waiting for you to authorize in your browser...</i></p>"
    ).arg(verificationUri, userCode);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Connect to Twitch");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    // Make links clickable
    msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);

    // Open browser automatically
    QDesktopServices::openUrl(QUrl(verificationUri));

    // Show dialog (non-blocking - user can keep it open while authorizing)
    msgBox.setModal(false);
    msgBox.show();
    msgBox.exec();
}

void MainWindow::onAuthenticationSucceeded(const QString &username)
{
    m_connectAction->setEnabled(false);
    m_disconnectAction->setEnabled(true);

    // Set up API with auth token
    m_twitchAPI->setAccessToken(m_twitchAuth->getAccessToken());
    m_twitchAPI->setClientId(TwitchAuth::getClientId());

    // Connect IRC status signals
    QObject::connect(m_webSocket, &TwitchWebSocket::connected,
                    [this]() {
        statusBar()->showMessage("IRC Connected - Ready to chat!", 0);
        qDebug() << "IRC WebSocket connected!";
    });

    QObject::connect(m_webSocket, &TwitchWebSocket::disconnected,
                    [this]() {
        statusBar()->showMessage("IRC Disconnected", 0);
        qDebug() << "IRC WebSocket disconnected!";
    });

    // Connect to IRC chat
    m_webSocket->connect(m_twitchAuth->getAccessToken(), username);
    statusBar()->showMessage("Connecting to IRC...", 0);

    // Connect chat signals to display messages
    QObject::connect(m_webSocket, &TwitchWebSocket::chatMessageReceived,
                    [this](const QString &channel, const QString &user, const QString &message, const QString &) {
        qDebug() << "[" << channel << "]" << user << ":" << message;

        // Find the ChatWidget for this channel
        if (m_channelWidgets.contains(channel)) {
            ChatWidget *chatWidget = m_channelWidgets[channel];
            // Random color for each user (could be improved with persistent color mapping)
            QColor userColor = QColor::fromHsl((qHash(user) % 360), 200, 150);
            chatWidget->addMessage(user, message, userColor);
        } else {
            qDebug() << "WARNING: No ChatWidget found for channel:" << channel;
        }
    });

    // Connect user JOIN/PART signals for user list
    QObject::connect(m_webSocket, &TwitchWebSocket::userJoined,
                    [this](const QString &channel, const QString &username) {
        if (channel == m_currentChannel) {
            m_userList->addUser(username);
            qDebug() << "Added user to list:" << username;
        }
    });

    QObject::connect(m_webSocket, &TwitchWebSocket::userParted,
                    [this](const QString &channel, const QString &username) {
        if (channel == m_currentChannel) {
            m_userList->removeUser(username);
            qDebug() << "Removed user from list:" << username;
        }
    });

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

void MainWindow::onCreatePrediction()
{
    if (!m_twitchAuth->isAuthenticated()) {
        QMessageBox::warning(this, "Not Connected",
                           "Please connect to Twitch first.");
        return;
    }

    PredictionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto data = dialog.getPredictionData();

        // TODO: Get current broadcaster ID (for now use authenticated user)
        QString broadcasterId = m_twitchAuth->getUserId();

        statusBar()->showMessage("Creating prediction...", 0);

        // Create prediction via API
        m_twitchAPI->createPrediction(broadcasterId, data.title,
                                     data.outcomes, data.durationSeconds);

        // TODO: Listen for API response and show success/error
        QMessageBox::information(this, "Prediction Created",
                                QString("Prediction '%1' created!\n\n"
                                       "Duration: %2 seconds\n"
                                       "Outcomes: %3")
                                .arg(data.title)
                                .arg(data.durationSeconds)
                                .arg(data.outcomes.join(", ")));

        statusBar()->showMessage("Prediction created successfully", 3000);
    }
}

void MainWindow::onCreatePoll()
{
    if (!m_twitchAuth->isAuthenticated()) {
        QMessageBox::warning(this, "Not Connected",
                           "Please connect to Twitch first.");
        return;
    }

    PollDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto data = dialog.getPollData();

        // TODO: Get current broadcaster ID (for now use authenticated user)
        QString broadcasterId = m_twitchAuth->getUserId();

        statusBar()->showMessage("Creating poll...", 0);

        // Create poll via API
        m_twitchAPI->createPoll(broadcasterId, data.title,
                               data.choices, data.durationSeconds);

        // TODO: Listen for API response and show success/error
        QMessageBox::information(this, "Poll Created",
                                QString("Poll '%1' created!\n\n"
                                       "Duration: %2 seconds\n"
                                       "Choices: %3\n"
                                       "Channel Points Voting: %4")
                                .arg(data.title)
                                .arg(data.durationSeconds)
                                .arg(data.choices.join(", "))
                                .arg(data.channelPointsVotingEnabled ? "Yes" : "No"));

        statusBar()->showMessage("Poll created successfully", 3000);
    }
}
