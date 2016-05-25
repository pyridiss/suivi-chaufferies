#include "addfueldeliverydialog.h"
#include "ui_addfueldeliverydialog.h"

AddFuelDeliveryDialog::AddFuelDeliveryDialog(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AddFuelDeliveryDialog)
{
    ui->setupUi(this);

    ui->radioButton_None->hide();

    connect(ui->radioButton_None,        SIGNAL(toggled(bool)), this, SLOT(deactivate(bool)));
    connect(ui->radioButton_WoodChips,   SIGNAL(clicked(bool)), this, SLOT(showWoodChipsGroup()));
    connect(ui->radioButton_Pellets,     SIGNAL(clicked(bool)), this, SLOT(showPelletsGroup()));
    connect(ui->radioButton_FuelOil,     SIGNAL(clicked(bool)), this, SLOT(showFuelOilGroup()));
    connect(ui->radioButton_Propane,     SIGNAL(clicked(bool)), this, SLOT(showPropaneGroup()));
    connect(ui->radioButton_NaturalGas,  SIGNAL(clicked(bool)), this, SLOT(showNaturalGasGroup()));
    connect(ui->radioButton_Electricity, SIGNAL(clicked(bool)), this, SLOT(showElectricityGroup()));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}

AddFuelDeliveryDialog::~AddFuelDeliveryDialog()
{
    delete ui;
}

void AddFuelDeliveryDialog::resetValues()
{
    ui->radioButton_None->setChecked(true);

    ui->editDeliveryDate->setDate(QDate::currentDate());
    ui->editProvider->setCurrentIndex(0);

    ui->editWoodChipsQuantity->setValue(0);
    ui->editWoodChipsMoisture->setValue(0);
    ui->editWoodChipsBill->setValue(0);

    ui->editPelletsQuantity->setValue(0);
    ui->editPelletsLHV->setValue(0);
    ui->editPelletsBill->setValue(0);

    ui->editFuelOilQuantity->setValue(0);
    ui->editFuelOilBill->setValue(0);

    ui->editPropaneQuantity->setValue(0);
    ui->editPropaneBill->setValue(0);

    ui->editNaturalGasIndex->setValue(0);

    ui->editElectricityIndex->setValue(0);

    ui->editComment->setPlainText("");

    hideAllGroupBoxes();
}

void AddFuelDeliveryDialog::hideAllGroupBoxes()
{
    ui->groupBox_WoodChips->hide();
    ui->groupBox_Pellets->hide();
    ui->groupBox_FuelOil->hide();
    ui->groupBox_Propane->hide();
    ui->groupBox_NaturalGas->hide();
    ui->groupBox_Electricity->hide();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::deactivate(bool status)
{
    ui->editDeliveryDate->setEnabled(!status);
    ui->editProvider->setEnabled(!status);
    ui->editComment->setEnabled(!status);
}

void AddFuelDeliveryDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    ui->radioButton_WoodChips->hide();
    ui->radioButton_Pellets->hide();
    ui->radioButton_FuelOil->hide();
    ui->radioButton_Propane->hide();
    ui->radioButton_NaturalGas->hide();
    ui->radioButton_Electricity->hide();

    switch (system->mMainHeatSource)
    {
        case HeatingSystem::WoodChips:
            ui->radioButton_WoodChips->show();
            break;
        case HeatingSystem::Pellets:
            ui->radioButton_Pellets->show();
            break;
        case HeatingSystem::GeothermalPower:
            ui->radioButton_Electricity->show();
            break;
        default:
            break;
    }

    switch (system->mSecondHeatSource)
    {
        case HeatingSystem::NoFuel:
            break;
        case HeatingSystem::NaturalGas:
            ui->radioButton_NaturalGas->show();
            break;
        case HeatingSystem::FuelOil:
            ui->radioButton_FuelOil->show();
            break;
        case HeatingSystem::Propane:
            ui->radioButton_Propane->show();
            break;
        case HeatingSystem::Electricity:
            ui->radioButton_Electricity->show();
            break;
        case HeatingSystem::Other:
            break;
        default:
            break;
    }

    resetValues();
}

void AddFuelDeliveryDialog::showWoodChipsGroup()
{
    hideAllGroupBoxes();
    ui->groupBox_WoodChips->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::showPelletsGroup()
{
    hideAllGroupBoxes();
    ui->groupBox_Pellets->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::showFuelOilGroup()
{
    hideAllGroupBoxes();
    ui->groupBox_FuelOil->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::showPropaneGroup()
{
    hideAllGroupBoxes();
    ui->groupBox_Propane->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::showNaturalGasGroup()
{
    hideAllGroupBoxes();
    ui->groupBox_NaturalGas->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::showElectricityGroup()
{
    hideAllGroupBoxes();
    ui->groupBox_Electricity->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::buttonBoxClicked(QAbstractButton* button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
        resetValues();
}

void AddFuelDeliveryDialog::on_buttonBox_accepted()
{
    //1. Wood chips
    if (ui->editWoodChipsQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::WoodChips,
                                             ui->editDeliveryDate->date(),
                                             ui->editWoodChipsQuantity->value(),
                                             ui->editWoodChipsBill->value(),
                                             ui->editWoodChipsUnit->currentIndex(),
                                             ui->editWoodChipsMoisture->value());
        mHeatingSystem->mWoodDeliveries.push_back(delivery);
    }

    //2. Pellets
    //TODO: Not really well saved.
    if (ui->editPelletsQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::Pellets,
                                             ui->editDeliveryDate->date(),
                                             ui->editPelletsQuantity->value(),
                                             ui->editPelletsBill->value(),
                                             0,
                                             0);
        mHeatingSystem->mWoodDeliveries.push_back(delivery);
    }

    //3. Fuel oil
    if (ui->editFuelOilQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::FuelOil,
                                             ui->editDeliveryDate->date(),
                                             ui->editFuelOilQuantity->value(),
                                             ui->editFuelOilBill->value());
        mHeatingSystem->mFossilFuelDeliveries.push_back(delivery);
    }

    //4. Propane
    if (ui->editFuelOilQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::Propane,
                                             ui->editDeliveryDate->date(),
                                             ui->editPropaneQuantity->value(),
                                             ui->editPropaneBill->value());
        mHeatingSystem->mFossilFuelDeliveries.push_back(delivery);
    }

    //5. Natural gas
    if (ui->editNaturalGasIndex->value() > 0)
    {
        HeatingSystem::FuelIndex delivery(HeatingSystem::NaturalGas,
                                          ui->editNaturalGasIndex->value(),
                                          ui->editDeliveryDate->date());
        mHeatingSystem->mNaturalGasIndexes.push_back(delivery);
    }

    //6. Electricity
    if (ui->editElectricityIndex->value() > 0)
    {
        HeatingSystem::FuelIndex delivery(HeatingSystem::NaturalGas,
                                          ui->editElectricityIndex->value(),
                                          ui->editDeliveryDate->date());
        mHeatingSystem->mElectricityIndexes.push_back(delivery);
    }

    mHeatingSystem->save();
    resetValues();
}
