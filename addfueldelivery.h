#ifndef ADDFUELDELIVERYDIALOG_H
#define ADDFUELDELIVERYDIALOG_H

#include <QFrame>
#include <QAbstractButton>
#include <heatingsystem.h>

namespace Ui {
class AddFuelDeliveryDialog;
}

class AddFuelDeliveryDialog : public QFrame
{
    Q_OBJECT

public:
    explicit AddFuelDeliveryDialog(QWidget *parent = 0);
    ~AddFuelDeliveryDialog();

public:
    void resetValues();

private:
    Ui::AddFuelDeliveryDialog *ui;
    HeatingSystem* mHeatingSystem;

public slots:
    void setHeatingSystem(HeatingSystem *system);
    void hideAllGroupBoxes();
    void deactivate(bool status);

    void showWoodChipsGroup();
    void showPelletsGroup();
    void showFuelOilGroup();
    void showPropaneGroup();
    void showNaturalGasGroup();
    void showElectricityGroup();

private slots:
    void on_buttonBox_accepted();
    void buttonBoxClicked(QAbstractButton* button);
};

#endif // ADDFUELDELIVERYDIALOG_H
