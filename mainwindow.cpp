#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mConfigurationDialog = new ConfigurationDialog(this);
    mAddFullDeliveryDialog = new AddFuelDeliveryDialog(this);
    mAddMetersRecordDialog = new AddMetersRecordDialog(this);

    connect(mConfigurationDialog, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSettings()
{
    QSettings settings;

    QString name = "<p align=\"center\"><span style=\"font-weight:600;\">";
    name += settings.value("boilerRoom/boilerRoomName", "Nom de la chaufferie").toString();
    name += "</span></p>";

    ui->labelBoilerRoomName->setText(name);
}

void MainWindow::on_actionConfigureBoilerRoom_triggered()
{
    mConfigurationDialog->readSettings();
    mConfigurationDialog->show();
}

void MainWindow::on_actionExport_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionSend_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionDownloadData_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void MainWindow::on_pushButton_AddFuelDelivery_clicked()
{
    mAddFullDeliveryDialog->resetValues();
    mAddFullDeliveryDialog->readSettings();
    mAddFullDeliveryDialog->show();
}

void MainWindow::on_pushButton_AddMetersRecord_clicked()
{
    mAddMetersRecordDialog->readSettings();
    mAddMetersRecordDialog->show();
}
