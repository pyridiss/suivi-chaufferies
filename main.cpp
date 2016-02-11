#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ALE 08");
    QCoreApplication::setOrganizationDomain("ale08.org");
    QCoreApplication::setApplicationName("SuiviChaufferies");
QList<QSpinBox*> list;
    MainWindow w;
    w.readSettings();
    w.show();

    return a.exec();
}
