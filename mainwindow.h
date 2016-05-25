#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <configurationdialog.h>
#include <addfueldelivery.h>
#include <addmetersrecord.h>
#include <showmetersrecordsdialog.h>
#include <showfueldeliveriesdialog.h>
#include <dju.h>
#include <heatingsystem.h>

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
    QMap<QString, DJU*> mDJU;
    QMap<QString, HeatingSystem*> mHeatingSystems;
    QString mCurrentHeatingSystem;

private slots:
    void on_actionConfigureBoilerRoom_triggered();
    void on_actionExport_triggered();
    void on_actionSend_triggered();
    void on_actionDownloadData_triggered();
    void on_actionExit_triggered();
    void on_actionAboutSoftware_triggered();
    void on_actionAboutQt_triggered();
    void actionNewHeatingSystem_triggered();
    void fileDownloaded(QByteArray *file);

    void on_pushButton_AddFuelDelivery_toggled(bool checked);
    void on_pushButton_AddMetersRecord_toggled(bool checked);

    void on_setScreen_FuelDeliveries_clicked();
    void on_setScreen_MetersRecords_clicked();
    void on_setScreen_Interventions_clicked();

public slots:
    void readSettings();
    void saveSettings();
    void changeCurrentHeatingSystem();
    void updateEnergyConsumptionChart();
};

#endif // MAINWINDOW_H
