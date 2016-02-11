#include <QGridLayout>
#include <QLabel>

#include "addfueldeliverydialog.h"
#include "ui_addfueldeliverydialog.h"

AddFuelDeliveryDialog::AddFuelDeliveryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFuelDeliveryDialog)
{
    ui->setupUi(this);

    QGridLayout* layout = new QGridLayout(ui->groupBox_Fuels);

    layout->addWidget(new QLabel("élément 1"), 0, 0);
    layout->addWidget(new QLabel("élément 2"), 0, 1);
    layout->addWidget(new QLabel("élément 3"), 1, 0);
    layout->addWidget(new QLabel("élément 4"), 1, 1);
}

AddFuelDeliveryDialog::~AddFuelDeliveryDialog()
{
    delete ui;
}

void AddFuelDeliveryDialog::readSettings()
{

}
