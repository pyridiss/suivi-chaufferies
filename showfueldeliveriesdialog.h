#ifndef SHOWFUELDELIVERIESDIALOG_H
#define SHOWFUELDELIVERIESDIALOG_H

#include <QDialog>

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

public slots:
    void readSettings();
    void recordChanged_Wood(int x, int y);
    void recordChanged_SecondaryFuel(int x, int y);
    void recordChanged_NaturalGas(int x, int y);
    void recordChanged_Electricity(int x, int y);
    void deleteWoodDelivery();
    void deleteSecondaryFuelDelivery();

signals:
    void settingsChanged();
};

#endif // SHOWFUELDELIVERIESDIALOG_H
