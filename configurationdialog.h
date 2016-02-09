#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ~ConfigurationDialog();

private:
    Ui::ConfigurationDialog *ui;

public slots:
    void setHeatSell(int state);
private slots:
    void on_pushButton_AddSubstation_clicked();
    void on_pushButton_RemoveSubstation_clicked();
};

#endif // CONFIGURATIONDIALOG_H
