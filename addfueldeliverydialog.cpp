#include "addfueldeliverydialog.h"
#include "ui_addfueldeliverydialog.h"

AddFuelDeliveryDialog::AddFuelDeliveryDialog(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AddFuelDeliveryDialog)
{
    ui->setupUi(this);

    connect(ui->radioButton_Wood, SIGNAL(clicked(bool)), this, SLOT(changeSelectionToWood()));
    connect(ui->radioButton_SecondaryFuel, SIGNAL(clicked(bool)), this, SLOT(changeSelectionToSecondaryFuel()));
    connect(ui->radioButton_Electricity, SIGNAL(clicked(bool)), this, SLOT(changeSelectionToElectricity()));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}

AddFuelDeliveryDialog::~AddFuelDeliveryDialog()
{
    delete ui;
}

void AddFuelDeliveryDialog::resetValues()
{
    ui->dateEdit_Wood->setDate(QDate::currentDate());
    ui->doubleSpinBox_Wood->setValue(0);
    ui->doubleSpinBox_WoodMoisture->setValue(0);
    ui->doubleSpinBox_WoodBill->setValue(0);

    ui->dateEdit_SecondaryFuel->setDate(QDate::currentDate());
    ui->spinBox_SecondaryFuel->setValue(0);
    ui->doubleSpinBox_SecondaryFuelBill->setValue(0);

    ui->dateEdit_NaturalGas->setDate(QDate::currentDate());
    ui->doubleSpinBox_NaturalGas->setValue(0);

    ui->dateEdit_Electricity->setDate(QDate::currentDate());
    ui->spinBox_Electricity->setValue(0);
}

void AddFuelDeliveryDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    if (system->mMainHeatSource == HeatingSystem::GeothermalPower)
    {
        ui->groupBox_Wood->hide();
        ui->radioButton_Wood->hide();
    }
    else //WoodChips or Pellets
    {
        ui->groupBox_Wood->show();
        ui->radioButton_Wood->show();
    }

    //Second fuel
    ui->groupBox_SecondaryFuel->hide();
    ui->groupBox_NaturalGas->hide();
    ui->radioButton_SecondaryFuel->hide();

    switch (system->mSecondHeatSource)
    {
        case HeatingSystem::NoFuel:
            break;
        case HeatingSystem::NaturalGas:
            ui->groupBox_NaturalGas->show();
            ui->radioButton_SecondaryFuel->show();
            ui->radioButton_SecondaryFuel->setText("Gaz naturel");
            break;
        case HeatingSystem::FuelOil:
            ui->groupBox_SecondaryFuel->show();
            ui->groupBox_SecondaryFuel->setTitle("Livraison de fioul");
            ui->radioButton_SecondaryFuel->show();
            ui->radioButton_SecondaryFuel->setText("Fioul");
            ui->spinBox_SecondaryFuel->setSuffix(" litres");
            break;
        case HeatingSystem::Propane:
            ui->groupBox_SecondaryFuel->show();
            ui->groupBox_SecondaryFuel->setTitle("Livraison de propane");
            ui->radioButton_SecondaryFuel->show();
            ui->radioButton_SecondaryFuel->setText("Propane");
            ui->spinBox_SecondaryFuel->setSuffix(" kilogrammes");
            break;
        case HeatingSystem::Electricity:
            break;
        case HeatingSystem::Other:
            break;
        default:
            break;
    }

    resize(layout()->minimumSize().width(), layout()->minimumSize().height());
}

void AddFuelDeliveryDialog::changeSelectionToWood()
{
    ui->groupBox_Wood->setEnabled(true);
    ui->groupBox_SecondaryFuel->setEnabled(false);
    ui->groupBox_NaturalGas->setEnabled(false);
    ui->groupBox_Electricity->setEnabled(false);
}

void AddFuelDeliveryDialog::changeSelectionToSecondaryFuel()
{
    ui->groupBox_Wood->setEnabled(false);
    ui->groupBox_SecondaryFuel->setEnabled(true);
    ui->groupBox_NaturalGas->setEnabled(true);
    ui->groupBox_Electricity->setEnabled(false);
}

void AddFuelDeliveryDialog::changeSelectionToElectricity()
{
    ui->groupBox_Wood->setEnabled(false);
    ui->groupBox_SecondaryFuel->setEnabled(false);
    ui->groupBox_NaturalGas->setEnabled(false);
    ui->groupBox_Electricity->setEnabled(true);
}

void AddFuelDeliveryDialog::buttonBoxClicked(QAbstractButton* button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
        resetValues();
}

void AddFuelDeliveryDialog::on_buttonBox_accepted()
{
    //1. Wood
    if (ui->doubleSpinBox_Wood->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(mHeatingSystem->mMainHeatSource,
                                             ui->dateEdit_Wood->date(),
                                             ui->doubleSpinBox_Wood->value(),
                                             ui->doubleSpinBox_WoodBill->value(),
                                             ui->comboBox_WoodUnit->currentIndex(),
                                             ui->doubleSpinBox_WoodMoisture->value());
        mHeatingSystem->mWoodDeliveries.push_back(delivery);
    }

    //2. Secondary fuel
    if (ui->spinBox_SecondaryFuel->value() > 0)
    {
        HeatingSystem::FuelDelivery delivery(mHeatingSystem->mSecondHeatSource,
                                             ui->dateEdit_SecondaryFuel->date(),
                                             ui->spinBox_SecondaryFuel->value(),
                                             ui->doubleSpinBox_SecondaryFuelBill->value());
        mHeatingSystem->mFossilFuelDeliveries.push_back(delivery);
    }

    //3. Natural gas
    if (ui->doubleSpinBox_NaturalGas->value() > 0)
    {
        HeatingSystem::FuelIndex delivery(HeatingSystem::NaturalGas,
                                          ui->doubleSpinBox_NaturalGas->value(),
                                          ui->dateEdit_NaturalGas->date());
        mHeatingSystem->mNaturalGasIndexes.push_back(delivery);
    }

    //4. Electricity
    if (ui->spinBox_Electricity->value() > 0)
    {
        HeatingSystem::FuelIndex delivery(HeatingSystem::NaturalGas,
                                          ui->spinBox_Electricity->value(),
                                          ui->dateEdit_Electricity->date());
        mHeatingSystem->mElectricityIndexes.push_back(delivery);
    }

    mHeatingSystem->save();
    resetValues();
}
