#ifndef ADDFUELDELIVERYDIALOG_H
#define ADDFUELDELIVERYDIALOG_H

#include <QDialog>

namespace Ui {
class AddFuelDeliveryDialog;
}

class AddFuelDeliveryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFuelDeliveryDialog(QWidget *parent = 0);
    ~AddFuelDeliveryDialog();

public:
    void resetValues();

private:
    Ui::AddFuelDeliveryDialog *ui;

public slots:
    void readSettings();
    void changeSelectionToWood();
    void changeSelectionToSecondaryFuel();
    void changeSelectionToElectricity();

private slots:
    void on_buttonBox_accepted();
};

#endif // ADDFUELDELIVERYDIALOG_H
