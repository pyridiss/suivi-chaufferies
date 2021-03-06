#include "addfueldelivery.h"
#include "ui_addfueldelivery.h"

AddFuelDelivery::AddFuelDelivery(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AddFuelDelivery)
{
    ui->setupUi(this);

    ui->radioButton_None->hide();

    connect(ui->radioButton_None,        SIGNAL(toggled(bool)), this, SLOT(hideAllFrames()));
    connect(ui->radioButton_WoodChips,   SIGNAL(clicked(bool)), this, SLOT(showWoodChipsGroup()));
    connect(ui->radioButton_Pellets,     SIGNAL(clicked(bool)), this, SLOT(showPelletsGroup()));
    connect(ui->radioButton_FuelOil,     SIGNAL(clicked(bool)), this, SLOT(showFuelOilGroup()));
    connect(ui->radioButton_Propane,     SIGNAL(clicked(bool)), this, SLOT(showPropaneGroup()));
    connect(ui->radioButton_NaturalGas,  SIGNAL(clicked(bool)), this, SLOT(showNaturalGasGroup()));
    connect(ui->radioButton_Electricity, SIGNAL(clicked(bool)), this, SLOT(showElectricityGroup()));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));

    hideAllFrames();
}

AddFuelDelivery::~AddFuelDelivery()
{
    delete ui;
}

void AddFuelDelivery::resetValues()
{
    ui->radioButton_None->setChecked(true);

    ui->editDeliveryDate->setDate(QDate::currentDate());

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

    hideAllFrames();
}

void AddFuelDelivery::hideAllFrames()
{
    ui->frame_WoodChips->hide();
    ui->frame_Pellets->hide();
    ui->frame_FuelOil->hide();
    ui->frame_Propane->hide();
    ui->frame_NaturalGas->hide();
    ui->frame_Electricity->hide();
    ui->frame_General->hide();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::setHeatingSystem(HeatingSystem *system)
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

    if (system->mElectricMeter)
        ui->radioButton_Electricity->show();

    resetValues();
}

void AddFuelDelivery::showWoodChipsGroup()
{
    hideAllFrames();
    ui->frame_WoodChips->show();
    ui->frame_General->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::showPelletsGroup()
{
    hideAllFrames();
    ui->frame_Pellets->show();
    ui->frame_General->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::showFuelOilGroup()
{
    hideAllFrames();
    ui->frame_FuelOil->show();
    ui->frame_General->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::showPropaneGroup()
{
    hideAllFrames();
    ui->frame_Propane->show();
    ui->frame_General->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::showNaturalGasGroup()
{
    hideAllFrames();
    ui->frame_NaturalGas->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::showElectricityGroup()
{
    hideAllFrames();
    ui->frame_Electricity->show();

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDelivery::buttonBoxClicked(QAbstractButton* button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
        resetValues();
}

void AddFuelDelivery::on_buttonBox_accepted()
{
    //1. Wood chips
    if (ui->editWoodChipsQuantity->value() > 0)
    {
        HeatingSystem::FuelUnits unit;
        if (ui->editWoodChipsUnit->currentIndex() == 0) unit = HeatingSystem::ApparentCubicMeters;
        if (ui->editWoodChipsUnit->currentIndex() == 1) unit = HeatingSystem::Tons;
        if (ui->editWoodChipsUnit->currentIndex() == 2) unit = HeatingSystem::MWh;

        HeatingSystem::FuelDelivery delivery(HeatingSystem::WoodChips,
                                             ui->editDeliveryDate->date(),
                                             ui->editWoodChipsQuantity->value(),
                                             unit,
                                             ui->editWoodChipsBill->value(),
                                             0,
                                             ui->editWoodChipsMoisture->value());
        mHeatingSystem->mFuelDeliveries.push_back(delivery);
    }

    //2. Pellets
    if (ui->editPelletsQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::Pellets,
                                             ui->editDeliveryDate->date(),
                                             ui->editPelletsQuantity->value(),
                                             HeatingSystem::Tons,
                                             ui->editPelletsBill->value(),
                                             ui->editPelletsLHV->value());
        mHeatingSystem->mFuelDeliveries.push_back(delivery);
    }

    //3. Fuel oil
    if (ui->editFuelOilQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::FuelOil,
                                             ui->editDeliveryDate->date(),
                                             ui->editFuelOilQuantity->value(),
                                             HeatingSystem::Liters,
                                             ui->editFuelOilBill->value());
        mHeatingSystem->mFuelDeliveries.push_back(delivery);
    }

    //4. Propane
    if (ui->editPropaneQuantity->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(HeatingSystem::Propane,
                                             ui->editDeliveryDate->date(),
                                             ui->editPropaneQuantity->value(),
                                             HeatingSystem::Kilograms,
                                             ui->editPropaneBill->value());
        mHeatingSystem->mFuelDeliveries.push_back(delivery);
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
