#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mConfigurationDialog = new ConfigurationDialog(this);

    connect(mConfigurationDialog, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void MainWindow::on_actionConfigureBoilerRoom_triggered()
{
    mConfigurationDialog->readSettings();
    mConfigurationDialog->show();
}

void MainWindow::readSettings()
{
    QSettings settings;

    QString name = "<p align=\"center\"><span style=\"font-weight:600;\">";
    name += settings.value("boilerRoom/boilerRoomName", "Nom de la chaufferie").toString();
    name += "</span></p>";

    ui->labelBoilerRoomName->setText(name);
}
