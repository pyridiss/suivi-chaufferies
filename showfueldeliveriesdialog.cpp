#include "showfueldeliveriesdialog.h"
#include "ui_showfueldeliveriesdialog.h"

ShowFuelDeliveriesDialog::ShowFuelDeliveriesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowFuelDeliveriesDialog)
{
    ui->setupUi(this);
}

ShowFuelDeliveriesDialog::~ShowFuelDeliveriesDialog()
{
    delete ui;
}

void ShowFuelDeliveriesDialog::resetValues()
{

}

void ShowFuelDeliveriesDialog::readSettings()
{

}
void ShowFuelDeliveriesDialog::recordChanged()
{
    emit settingsChanged();
}
