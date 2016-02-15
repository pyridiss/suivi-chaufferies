#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ALE 08");
    QCoreApplication::setOrganizationDomain("ale08.org");
    QCoreApplication::setApplicationName("SuiviChaufferies");

    MainWindow w;
    w.readSettings();
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - w.width()) / 2;
    int y = (screenGeometry.height() - w.height()) / 2;
    w.move(x, y);
    w.show();

    return a.exec();
}
