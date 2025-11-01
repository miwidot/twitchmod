#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("TwitchMod");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("TwitchMod");

#ifdef Q_OS_WIN
    // Windows-specific: Use Fusion style for better look
    QApplication::setStyle("fusion");
#endif

#ifdef Q_OS_MACOS
    // macOS-specific: Don't show icons in menus (native macOS style)
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

    MainWindow window;
    window.show();

    return app.exec();
}
