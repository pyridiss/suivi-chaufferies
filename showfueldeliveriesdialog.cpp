#include <QSettings>
#include <QDate>

#include <delegates/comboboxdelegate.h>
#include <delegates/doublespinboxdelegate.h>

#include "showfueldeliveriesdialog.h"
#include "ui_showfueldeliveriesdialog.h"

ShowFuelDeliveriesDialog::ShowFuelDeliveriesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowFuelDeliveriesDialog)
{
    ui->setupUi(this);

    /*
     * Configure table 'Wood' columns
     */

    //Date shouldn't be shown
    ui->tableWidget_Wood->hideColumn(0);

    //Wood units will be edited with a ComboBox
    ComboBoxDelegate *delegateUnit_Wood = new ComboBoxDelegate(this);
    QVector<QString> items_Wood;
    items_Wood.push_back("MAP");
    items_Wood.push_back("tonnes");
    items_Wood.push_back("MWh");
    delegateUnit_Wood->setItems(items_Wood);
    ui->tableWidget_Wood->setItemDelegateForColumn(2, delegateUnit_Wood);

    //Wood moisture will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateMoisture_Wood = new DoubleSpinBoxDelegate(this);
    delegateMoisture_Wood->setSuffix(" %");
    delegateMoisture_Wood->setMaximum(100);
    ui->tableWidget_Wood->setItemDelegateForColumn(3, delegateMoisture_Wood);

    //Wood energy will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateEnergy_Wood = new DoubleSpinBoxDelegate(this);
    delegateEnergy_Wood->setSuffix(" MWh");
    delegateEnergy_Wood->setPrecision(5);
    ui->tableWidget_Wood->setItemDelegateForColumn(4, delegateEnergy_Wood);

    //Wood bill will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateBill_Wood = new DoubleSpinBoxDelegate(this);
    delegateBill_Wood->setSuffix(" €");
    delegateBill_Wood->setPrecision(6);
    ui->tableWidget_Wood->setItemDelegateForColumn(5, delegateBill_Wood);

    //Wood price per MWh will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegatePricePerMWh_Wood = new DoubleSpinBoxDelegate(this);
    delegatePricePerMWh_Wood->setSuffix(" € / MWh");
    delegatePricePerMWh_Wood->setPrecision(5);
    ui->tableWidget_Wood->setItemDelegateForColumn(6, delegatePricePerMWh_Wood);

    /*
     * Configure table 'Secondary fuel' columns
     */

    //Date shouldn't be shown
    ui->tableWidget_SecondaryFuel->hideColumn(0);

    //Fuel units will be edited with a ComboBox
    ComboBoxDelegate *delegateUnit_Fuel = new ComboBoxDelegate(this);
    QVector<QString> items_Fuel;
    items_Fuel.push_back("litres");
    items_Fuel.push_back("tonnes");
    items_Fuel.push_back("mètres-cube");
    items_Fuel.push_back("kWh");
    delegateUnit_Fuel->setItems(items_Fuel);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(2, delegateUnit_Fuel);

    //Fuel energy will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateEnergy_Fuel = new DoubleSpinBoxDelegate(this);
    delegateEnergy_Fuel->setSuffix(" MWh");
    delegateEnergy_Fuel->setPrecision(5);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(3, delegateEnergy_Fuel);

    //Fuel bill will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateBill_Fuel = new DoubleSpinBoxDelegate(this);
    delegateBill_Fuel->setSuffix(" €");
    delegateBill_Fuel->setPrecision(6);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(4, delegateBill_Fuel);

    //Fuel price per MWh will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegatePricePerMWh_Fuel = new DoubleSpinBoxDelegate(this);
    delegatePricePerMWh_Fuel->setSuffix(" € / MWh");
    delegatePricePerMWh_Fuel->setPrecision(5);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(5, delegatePricePerMWh_Fuel);

    /*
     * Configure table 'Natural gas' columns
     */

    //Date shouldn't be shown
    ui->tableWidget_NaturalGas->hideRow(0);

    //Gas index will be edited by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateIndex_NaturalGas = new DoubleSpinBoxDelegate(this);
    delegateIndex_NaturalGas->setSuffix(" kWh");
    delegateIndex_NaturalGas->setPrecision(5);
    ui->tableWidget_NaturalGas->setItemDelegateForRow(1, delegateIndex_NaturalGas);

    /*
     * Configure table 'Electricity' columns
     */

    //Date shouldn't be shown
    ui->tableWidget_Electricity->hideRow(0);

    //Gas index will be edited by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateIndex_Electricity = new DoubleSpinBoxDelegate(this);
    delegateIndex_Electricity->setSuffix(" kWh");
    delegateIndex_Electricity->setPrecision(5);
    ui->tableWidget_Electricity->setItemDelegateForRow(1, delegateIndex_Electricity);
}

ShowFuelDeliveriesDialog::~ShowFuelDeliveriesDialog()
{
    delete ui;
}

void ShowFuelDeliveriesDialog::resetValues()
{
    disconnect(ui->tableWidget_Wood, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged()));
    while (ui->tableWidget_Wood->rowCount() > 0) ui->tableWidget_Wood->removeRow(0);

    disconnect(ui->tableWidget_SecondaryFuel, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged()));
    while (ui->tableWidget_SecondaryFuel->rowCount() > 0) ui->tableWidget_SecondaryFuel->removeRow(0);

    disconnect(ui->tableWidget_NaturalGas, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged()));
    while (ui->tableWidget_NaturalGas->columnCount() > 0) ui->tableWidget_NaturalGas->removeColumn(0);

    disconnect(ui->tableWidget_Electricity, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged()));
    while (ui->tableWidget_Electricity->columnCount() > 0) ui->tableWidget_Electricity->removeColumn(0);
}

void ShowFuelDeliveriesDialog::readSettings()
{
    QSettings settings;

    //1. Wood
    int woodDeliveriesNumber = settings.beginReadArray("woodDeliveries");
    for (int i = 0 ; i < woodDeliveriesNumber ; ++i)
    {
        settings.setArrayIndex(i);
        ui->tableWidget_Wood->insertRow(0);
        QDate date = settings.value("date").toDate();
        double quantity = settings.value("quantity").toDouble();
        int unit = settings.value("unit").toInt();
        double moisture = settings.value("moisture").toDouble();
        double bill = settings.value("bill").toDouble();

        //Date
        ui->tableWidget_Wood->setItem(0, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));

        //Quantity
        QTableWidgetItem *itemQuantity = new QTableWidgetItem;
        itemQuantity->setData(Qt::EditRole, quantity);
        itemQuantity->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_Wood->setItem(0, 1, itemQuantity);

        //Unit
        QTableWidgetItem *itemUnit = new QTableWidgetItem(QString::number(unit));
        ui->tableWidget_Wood->setItem(0, 2, itemUnit);

        //Moisture
        QTableWidgetItem *itemMoisture = new QTableWidgetItem;
        itemMoisture->setData(Qt::EditRole, moisture);
        ui->tableWidget_Wood->setItem(0, 3, itemMoisture);

        //Energy
        QTableWidgetItem *itemEnergy = new QTableWidgetItem();
        itemEnergy->setFlags(Qt::NoItemFlags);
        ui->tableWidget_Wood->setItem(0, 4, itemEnergy);

        //Bill
        QTableWidgetItem *itemBill = new QTableWidgetItem;
        itemBill->setData(Qt::EditRole, bill);
        ui->tableWidget_Wood->setItem(0, 5, itemBill);

        //Price per MWh
        QTableWidgetItem *itemPrice = new QTableWidgetItem();
        itemPrice->setFlags(Qt::NoItemFlags);
        ui->tableWidget_Wood->setItem(0, 6, itemPrice);

        //Sort deliveries
        ui->tableWidget_Wood->sortItems(0);

        //Copy dates in vertical header
        QStringList headers;
        for (int i = 0 ; i < ui->tableWidget_Wood->rowCount() ; ++i)
        {
            QString q = ui->tableWidget_Wood->item(i, 0)->text();
            q = QDate::fromString(q, "yyyy-MM-dd").toString("dd MMMM yyyy");
            headers.push_back(q);
        }
        ui->tableWidget_Wood->setVerticalHeaderLabels(headers);
    }

    settings.endArray();

    //2. Secondary fuel
    int secondaryFuelDeliviriesNumber = settings.beginReadArray("secondaryFuelDeliveries");
    for (int i = 0 ; i < secondaryFuelDeliviriesNumber ; ++i)
    {
        settings.setArrayIndex(i);
        ui->tableWidget_SecondaryFuel->insertRow(0);
        QDate date = settings.value("date").toDate();
        double quantity = settings.value("quantity").toDouble();
        int unit = settings.value("unit").toInt();
        double bill = settings.value("bill").toDouble();

        //Date
        ui->tableWidget_SecondaryFuel->setItem(0, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));

        //Quantity
        QTableWidgetItem *itemQuantity = new QTableWidgetItem;
        itemQuantity->setData(Qt::EditRole, quantity);
        itemQuantity->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_SecondaryFuel->setItem(0, 1, itemQuantity);

        //Unit
        QTableWidgetItem *itemUnit = new QTableWidgetItem(QString::number(unit));
        ui->tableWidget_SecondaryFuel->setItem(0, 2, itemUnit);

        //Energy
        QTableWidgetItem *itemEnergy = new QTableWidgetItem();
        itemEnergy->setFlags(Qt::NoItemFlags);
        ui->tableWidget_SecondaryFuel->setItem(0, 3, itemEnergy);

        //Bill
        QTableWidgetItem *itemBill = new QTableWidgetItem;
        itemBill->setData(Qt::EditRole, bill);
        ui->tableWidget_SecondaryFuel->setItem(0, 4, itemBill);

        //Price per MWh
        QTableWidgetItem *itemPrice = new QTableWidgetItem();
        itemPrice->setFlags(Qt::NoItemFlags);
        ui->tableWidget_SecondaryFuel->setItem(0, 5, itemPrice);

        //Sort deliveries
        ui->tableWidget_SecondaryFuel->sortItems(0);

        //Copy dates in vertical header
        QStringList headers;
        for (int i = 0 ; i < ui->tableWidget_SecondaryFuel->rowCount() ; ++i)
        {
            QString q = ui->tableWidget_SecondaryFuel->item(i, 0)->text();
            q = QDate::fromString(q, "yyyy-MM-dd").toString("dd MMMM yyyy");
            headers.push_back(q);
        }
        ui->tableWidget_SecondaryFuel->setVerticalHeaderLabels(headers);
    }

    settings.endArray();

    //3. Natural gas
    int naturalGasIndexesNumber = settings.beginReadArray("naturalGasIndex");
    for (int i = 0 ; i < naturalGasIndexesNumber ; ++i)
    {
        settings.setArrayIndex(i);
        ui->tableWidget_NaturalGas->insertColumn(0);
        QDate date = settings.value("date").toDate();
        int index = settings.value("index").toInt();

        //Date
        ui->tableWidget_NaturalGas->setItem(0, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));

        //Index
        QTableWidgetItem *itemIndex = new QTableWidgetItem;
        itemIndex->setData(Qt::EditRole, index);
        ui->tableWidget_NaturalGas->setItem(1, 0, itemIndex);

        //Copy dates in horizontal header
        QStringList headers;
        for (int i = 0 ; i < ui->tableWidget_NaturalGas->columnCount() ; ++i)
        {
            QString q = ui->tableWidget_NaturalGas->item(0, i)->text();
            q = QDate::fromString(q, "yyyy-MM-dd").toString("dd/MM/yyyy");
            headers.push_back(q);
        }
        ui->tableWidget_NaturalGas->setHorizontalHeaderLabels(headers);
    }

    settings.endArray();

    //4. Electricity
    int electricityIndexesNumber = settings.beginReadArray("electricityIndex");
    for (int i = 0 ; i < electricityIndexesNumber ; ++i)
    {
        settings.setArrayIndex(i);
        ui->tableWidget_Electricity->insertColumn(0);
        QDate date = settings.value("date").toDate();
        int index = settings.value("index").toInt();

        //Date
        ui->tableWidget_Electricity->setItem(0, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));

        //Index
        QTableWidgetItem *itemIndex = new QTableWidgetItem;
        itemIndex->setData(Qt::EditRole, index);
        ui->tableWidget_Electricity->setItem(1, 0, itemIndex);

        //Copy dates in horizontal header
        QStringList headers;
        for (int i = 0 ; i < ui->tableWidget_Electricity->columnCount() ; ++i)
        {
            QString q = ui->tableWidget_Electricity->item(0, i)->text();
            q = QDate::fromString(q, "yyyy-MM-dd").toString("dd/MM/yyyy");
            headers.push_back(q);
        }
        ui->tableWidget_Electricity->setHorizontalHeaderLabels(headers);
    }

    settings.endArray();
}

void ShowFuelDeliveriesDialog::recordChanged()
{
    emit settingsChanged();
}
