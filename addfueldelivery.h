#ifndef ADDFUELDELIVERY_H
#define ADDFUELDELIVERY_H

#include <QFrame>
#include <QAbstractButton>
#include <heatingsystem.h>

namespace Ui {
class AddFuelDelivery;
}

class AddFuelDelivery : public QFrame
{
    Q_OBJECT

public:
    explicit AddFuelDelivery(QWidget *parent = 0);
    ~AddFuelDelivery();

public:
    void resetValues();

private:
    Ui::AddFuelDelivery *ui;
    HeatingSystem* mHeatingSystem;

public slots:
    void setHeatingSystem(HeatingSystem *system);
    void hideAllFrames();

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

#endif // ADDFUELDELIVERY_H
