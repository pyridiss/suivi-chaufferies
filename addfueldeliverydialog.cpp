#include <QSettings>

#include "addfueldeliverydialog.h"
#include "ui_addfueldeliverydialog.h"

AddFuelDeliveryDialog::AddFuelDeliveryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFuelDeliveryDialog)
{
    ui->setupUi(this);

    connect(ui->radioButton_Wood, SIGNAL(clicked(bool)), this, SLOT(changeSelectionToWood()));
    connect(ui->radioButton_SecondaryFuel, SIGNAL(clicked(bool)), this, SLOT(changeSelectionToSecondaryFuel()));
    connect(ui->radioButton_Electricity, SIGNAL(clicked(bool)), this, SLOT(changeSelectionToElectricity()));
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

void AddFuelDeliveryDialog::readSettings()
{
    QSettings settings;

    //1. Wood chips
    if (settings.value("boilerRoom/mainHeatSource").toInt() == 0)
    {
        ui->groupBox_Wood->show();
        ui->radioButton_Wood->show();
    }

    //2. Pellets
    if (settings.value("boilerRoom/mainHeatSource").toInt() == 1)
    {
        ui->groupBox_Wood->show();
        ui->radioButton_Wood->show();
    }

    //3. Geothermal power
    if (settings.value("boilerRoom/mainHeatSource").toInt() == 2)
    {
        ui->groupBox_Wood->hide();
        ui->radioButton_Wood->hide();
    }

    //Secondary fuel
    ui->groupBox_SecondaryFuel->hide();
    ui->groupBox_NaturalGas->hide();
    ui->radioButton_SecondaryFuel->hide();

    switch (settings.value("boilerRoom/secondaryHeatSource").toInt())
    {
        case 0: //None
            break;
        case 1: //Natural Gas
            ui->groupBox_NaturalGas->show();
            ui->radioButton_SecondaryFuel->show();
            ui->radioButton_SecondaryFuel->setText("Gaz naturel");
            break;
        case 2: //Fuel oil
            ui->groupBox_SecondaryFuel->show();
            ui->groupBox_SecondaryFuel->setTitle("Livraison de fioul");
            ui->radioButton_SecondaryFuel->show();
            ui->radioButton_SecondaryFuel->setText("Fioul");
            ui->spinBox_SecondaryFuel->setSuffix(" litres");
            break;
        case 3: //Propane
            ui->groupBox_SecondaryFuel->show();
            ui->groupBox_SecondaryFuel->setTitle("Livraison de propane");
            ui->radioButton_SecondaryFuel->show();
            ui->radioButton_SecondaryFuel->setText("Propane");
            ui->spinBox_SecondaryFuel->setSuffix(" kilogrammes");
            break;
        case 4: //Electricity
            break;
        case 5: //Other
            break;
    }

    //TODO: This line does not work because minimumSize() is not updated yet.
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

void AddFuelDeliveryDialog::on_buttonBox_accepted()
{
    QSettings settings;

    //1. Wood
    if (ui->doubleSpinBox_Wood->value() > 0)
    {
        int currentSize = settings.value("woodDeliveries/size").toInt();

        settings.beginWriteArray("woodDeliveries");
        settings.setArrayIndex(currentSize);

        settings.setValue("date", ui->dateEdit_Wood->date());
        settings.setValue("quantity", ui->doubleSpinBox_Wood->value());
        settings.setValue("unit", ui->comboBox_WoodUnit->currentIndex());
        settings.setValue("moisture", ui->doubleSpinBox_WoodMoisture->value());
        settings.setValue("bill", ui->doubleSpinBox_WoodBill->value());

        settings.endArray();
    }

    //2. Secondary fuel
    if (ui->spinBox_SecondaryFuel->value() > 0)
    {
        int currentSize = settings.value("secondaryFuelDeliveries/size").toInt();

        settings.beginWriteArray("secondaryFuelDeliveries");
        settings.setArrayIndex(currentSize);

        settings.setValue("date", ui->dateEdit_SecondaryFuel->date());
        settings.setValue("quantity", ui->spinBox_SecondaryFuel->value());
        settings.setValue("bill", ui->doubleSpinBox_SecondaryFuelBill->value());

        settings.endArray();
    }

    //3. Natural gas
    if (ui->doubleSpinBox_NaturalGas->value() > 0)
    {
        int currentSize = settings.value("naturalGasIndex/size").toInt();

        settings.beginWriteArray("naturalGasIndex");
        settings.setArrayIndex(currentSize);

        settings.setValue("date", ui->dateEdit_NaturalGas->date());
        settings.setValue("index", ui->doubleSpinBox_NaturalGas->value());

        settings.endArray();
    }

    //4. Electricity
    if (ui->spinBox_Electricity->value() > 0)
    {
        int currentSize = settings.value("electricityIndex/size").toInt();

        settings.beginWriteArray("electricityIndex");
        settings.setArrayIndex(currentSize);

        settings.setValue("date", ui->dateEdit_Electricity->date());
        settings.setValue("index", ui->spinBox_Electricity->value());

        settings.endArray();
    }
}
