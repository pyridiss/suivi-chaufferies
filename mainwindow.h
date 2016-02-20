#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <configurationdialog.h>
#include <addfueldeliverydialog.h>
#include <addmetersrecorddialog.h>
#include <showmetersrecordsdialog.h>
#include <showfueldeliveriesdialog.h>
#include <dju.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ConfigurationDialog *mConfigurationDialog;
    AddFuelDeliveryDialog *mAddFullDeliveryDialog;
    AddMetersRecordDialog *mAddMetersRecordDialog;
    ShowMetersRecordsDialog *mShowMetersRecordsDialog;
    ShowFuelDeliveriesDialog *mShowFuelDeliveriesDialog;

private:
    DJU mDJU;

private slots:
    void on_actionConfigureBoilerRoom_triggered();
    void on_actionExport_triggered();
    void on_actionSend_triggered();
    void on_actionDownloadData_triggered();
    void on_actionExit_triggered();
    void on_actionShowDeliveries_triggered();
    void on_actionShowMetersRecord_triggered();
    void on_actionAboutSoftware_triggered();
    void on_actionAboutQt_triggered();
    void on_pushButton_AddFuelDelivery_clicked();
    void on_pushButton_AddMetersRecord_clicked();
    void fileDownloaded(QByteArray *file);

public slots:
    void readSettings();
    void updateEnergyConsumptionChart();
};

#endif // MAINWINDOW_H
