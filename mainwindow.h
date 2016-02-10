#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <configurationdialog.h>

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

private slots:
    void on_actionConfigureBoilerRoom_triggered();
    void on_actionExport_triggered();
    void on_actionSend_triggered();
    void on_actionDownloadData_triggered();
    void on_actionExit_triggered();
    void on_pushButton_AddFuelDelivery_clicked();
    void on_pushButton_AddMetersRecord_clicked();

public slots:
    void readSettings();
};

#endif // MAINWINDOW_H
