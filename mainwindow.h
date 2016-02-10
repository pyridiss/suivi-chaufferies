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
    void on_actionExit_triggered();
    void on_actionConfigureBoilerRoom_triggered();

public slots:
    void readSettings();
};

#endif // MAINWINDOW_H
