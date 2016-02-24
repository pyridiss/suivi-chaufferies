#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <heatingsystem.h>

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ~ConfigurationDialog();

public:
    void setHeatingSystem(HeatingSystem *system);

private:
    Ui::ConfigurationDialog *ui;
    HeatingSystem *mHeatingSystem;

public slots:
    void setHeatSell(int state);

private slots:
    void on_pushButton_AddSubstation_clicked();
    void on_pushButton_RemoveSubstation_clicked();
    void on_buttonBox_accepted();

signals:
    void settingsChanged();
};

#endif // CONFIGURATIONDIALOG_H
