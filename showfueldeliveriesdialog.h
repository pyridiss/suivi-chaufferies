#ifndef SHOWFUELDELIVERIESDIALOG_H
#define SHOWFUELDELIVERIESDIALOG_H

#include <QDialog>
#include <heatingsystem.h>

namespace Ui {
class ShowFuelDeliveriesDialog;
}

class ShowFuelDeliveriesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowFuelDeliveriesDialog(QWidget *parent = 0);
    ~ShowFuelDeliveriesDialog();

public:
    void resetValues();

private:
    Ui::ShowFuelDeliveriesDialog *ui;
    HeatingSystem* mHeatingSystem;

public slots:
    void setHeatingSystem(HeatingSystem *system);
    void recordChanged_Wood(int x, int y);
    void recordChanged_SecondaryFuel(int x, int y);
    void recordChanged_NaturalGas(int x, int y);
    void recordChanged_Electricity(int x, int y);
    void deleteWoodDelivery();
    void deleteSecondaryFuelDelivery();
    void updateSums();

signals:
    void settingsChanged();
};

#endif // SHOWFUELDELIVERIESDIALOG_H
