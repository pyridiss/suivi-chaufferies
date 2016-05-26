#include <QDate>
#include <QPushButton>

#include <delegates/comboboxdelegate.h>
#include <delegates/doublespinboxdelegate.h>

#include "showfueldeliveriesdialog.h"
#include "ui_showfueldeliveriesdialog.h"

enum TableFuelColumns
{
    Column_Date,
    Column_UUID,
    Column_Fuel,
    Column_Quantity,
    Column_Unit,
    Column_LHV,
    Column_Moisture,
    Column_Energy,
    Column_Bill,
    Column_EnergyPrice,
    Column_Delete
};

ShowFuelDeliveriesDialog::ShowFuelDeliveriesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowFuelDeliveriesDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(settingsChanged()), this, SLOT(updateSums()));

    /*
     * Configure table 'Fuels' columns
     */

    //A few columns shouldn't be shown
    ui->tableFuels->hideColumn(Column_Date);
    ui->tableFuels->hideColumn(Column_UUID);

    //Fuel
    ComboBoxDelegate *delegateFuel = new ComboBoxDelegate(this);
    QVector<QString> fuels;
    fuels.push_back("Bois déchiqueté");
    fuels.push_back("Granulés");
    fuels.push_back("Fioul");
    fuels.push_back("Propane");
    delegateFuel->setItems(fuels);
    ui->tableFuels->setItemDelegateForColumn(Column_Fuel, delegateFuel);
    ui->tableFuels->setColumnWidth(Column_Fuel, 150);

    //Qauntity
    ui->tableFuels->setColumnWidth(Column_Quantity, 110);

    //Unit
    ComboBoxDelegate *delegateUnit = new ComboBoxDelegate(this);
    QVector<QString> units;
    units.push_back("kg");
    units.push_back("tonnes");
    units.push_back("litres");
    units.push_back("m³");
    units.push_back("MAP");
    units.push_back("kWh");
    units.push_back("MWh");
    delegateUnit->setItems(units);
    ui->tableFuels->setItemDelegateForColumn(Column_Unit, delegateUnit);
    ui->tableFuels->setColumnWidth(Column_Unit, 80);

    //LHV
    DoubleSpinBoxDelegate *delegateLHV = new DoubleSpinBoxDelegate(this);
    delegateLHV->setSuffix(" kWh / unité");
    delegateLHV->setPrecision(4);
    ui->tableFuels->setItemDelegateForColumn(Column_LHV, delegateLHV);
    ui->tableFuels->setColumnWidth(Column_LHV, 140);

    //Moisture
    DoubleSpinBoxDelegate *delegateMoisture = new DoubleSpinBoxDelegate(this);
    delegateMoisture->setSuffix(" %");
    delegateMoisture->setMaximum(100);
    ui->tableFuels->setItemDelegateForColumn(Column_Moisture, delegateMoisture);
    ui->tableFuels->setColumnWidth(Column_Moisture, 80);

    //Energy
    DoubleSpinBoxDelegate *delegateEnergy = new DoubleSpinBoxDelegate(this);
    delegateEnergy->setSuffix(" MWh");
    delegateEnergy->setPrecision(4);
    ui->tableFuels->setItemDelegateForColumn(Column_Energy, delegateEnergy);
    ui->tableFuels->setColumnWidth(Column_Energy, 100);

    //Bill
    DoubleSpinBoxDelegate *delegateBill = new DoubleSpinBoxDelegate(this);
    delegateBill->setSuffix(" €");
    delegateBill->setPrecision(6);
    ui->tableFuels->setItemDelegateForColumn(Column_Bill, delegateBill);
    ui->tableFuels->setColumnWidth(Column_Bill, 110);

    //Energy Price
    DoubleSpinBoxDelegate *delegateEnergyPrice = new DoubleSpinBoxDelegate(this);
    delegateEnergyPrice->setSuffix(" € / MWh");
    delegateEnergyPrice->setPrecision(4);
    ui->tableFuels->setItemDelegateForColumn(Column_EnergyPrice, delegateEnergyPrice);
    ui->tableFuels->setColumnWidth(Column_EnergyPrice, 110);

    //Last column allows user to delete records
    ui->tableFuels->setColumnWidth(Column_Delete, 34);

    /*
     * Configure table 'Natural gas' columns
     */

    //Date shouldn't be shown
    ui->tableWidget_NaturalGas->hideRow(0);

    //Gas index will be edited by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateIndex_NaturalGas = new DoubleSpinBoxDelegate(this);
    delegateIndex_NaturalGas->setSuffix(" m³");
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
    disconnect(ui->tableFuels, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Fuel(int, int)));
    while (ui->tableFuels->rowCount() > 0) ui->tableFuels->removeRow(0);

    disconnect(ui->tableWidget_NaturalGas, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_NaturalGas(int, int)));
    while (ui->tableWidget_NaturalGas->columnCount() > 0) ui->tableWidget_NaturalGas->removeColumn(0);

    disconnect(ui->tableWidget_Electricity, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Electricity(int, int)));
    while (ui->tableWidget_Electricity->columnCount() > 0) ui->tableWidget_Electricity->removeColumn(0);
}

void ShowFuelDeliveriesDialog::updateSums()
{
    for (int i = 0 ; i < ui->tableFuels->rowCount() ; ++i)
    {
        double quantity = ui->tableFuels->item(i, Column_Quantity)->data(Qt::EditRole).toDouble();
        double lhv      = ui->tableFuels->item(i, Column_LHV)     ->data(Qt::EditRole).toDouble();
        double bill     = ui->tableFuels->item(i, Column_Bill)    ->data(Qt::EditRole).toDouble();

        ui->tableFuels->item(i, Column_Energy)     ->setData(Qt::DisplayRole, quantity * lhv / 1000);
        ui->tableFuels->item(i, Column_EnergyPrice)->setData(Qt::DisplayRole, bill / (quantity * lhv / 1000));
    }
}

void ShowFuelDeliveriesDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    /*
     * Fuels
     */

    foreach (const HeatingSystem::FuelDelivery &delivery, system->mFuelDeliveries)
    {
        ui->tableFuels->insertRow(0);

        //UUID
        ui->tableFuels->setItem(0, Column_UUID, new QTableWidgetItem(delivery.getHash()));

        //Date
        ui->tableFuels->setItem(0, Column_Date, new QTableWidgetItem(delivery.mDate.toString("yyyy-MM-dd")));

        //Fuel
        ui->tableFuels->setItem(0, Column_Fuel, new QTableWidgetItem(QString::number(delivery.mFuel)));

        //Quantity
        QTableWidgetItem *itemQuantity = new QTableWidgetItem;
        itemQuantity->setData(Qt::EditRole, delivery.mValue);
        itemQuantity->setTextAlignment(Qt::AlignCenter);
        ui->tableFuels->setItem(0, Column_Quantity, itemQuantity);

        //Unit
        QTableWidgetItem *itemUnit = new QTableWidgetItem(QString::number(delivery.mUnit));
        if (delivery.mFuel != HeatingSystem::WoodChips)
            itemUnit->setFlags(Qt::NoItemFlags);
        ui->tableFuels->setItem(0, Column_Unit, itemUnit);

        //LHV
        QTableWidgetItem *itemLHV = new QTableWidgetItem(QString::number(delivery.mLHV));
        if (delivery.mFuel != HeatingSystem::Pellets)
            itemLHV->setFlags(Qt::NoItemFlags);
        ui->tableFuels->setItem(0, Column_LHV, itemLHV);

        //Moisture
        QTableWidgetItem *itemMoisture = new QTableWidgetItem;
        itemMoisture->setData(Qt::EditRole, delivery.mWoodMoisture);
        if (delivery.mFuel != HeatingSystem::WoodChips)
            itemMoisture->setFlags(Qt::NoItemFlags);
        ui->tableFuels->setItem(0, Column_Moisture, itemMoisture);

        //Energy
        QTableWidgetItem *itemEnergy = new QTableWidgetItem();
        itemEnergy->setFlags(Qt::NoItemFlags);
        ui->tableFuels->setItem(0, Column_Energy, itemEnergy);

        //Bill
        QTableWidgetItem *itemBill = new QTableWidgetItem;
        itemBill->setData(Qt::EditRole, delivery.mBill);
        ui->tableFuels->setItem(0, Column_Bill, itemBill);

        //Energy Price
        QTableWidgetItem *itemPrice = new QTableWidgetItem();
        itemPrice->setFlags(Qt::NoItemFlags);
        ui->tableFuels->setItem(0, Column_EnergyPrice, itemPrice);

        //Delete button
        QPushButton* itemDelete = new QPushButton(ui->tableFuels);
        itemDelete->setIcon(QIcon::fromTheme("edit-delete"));
        itemDelete->setProperty("record", delivery.getHash());
        ui->tableFuels->setCellWidget(0, Column_Delete, itemDelete);
        connect(itemDelete, SIGNAL(clicked(bool)), this, SLOT(deleteFuelDelivery()));
    }

    //Sort deliveries
    ui->tableFuels->sortItems(Column_Date);

    //Copy dates in vertical header
    QStringList headers;
    for (int i = 0 ; i < ui->tableFuels->rowCount() ; ++i)
    {
        QString q = ui->tableFuels->item(i, Column_Date)->text();
        q = QDate::fromString(q, "yyyy-MM-dd").toString("dd MMMM yyyy");
        headers.push_back(q);
    }
    ui->tableFuels->setVerticalHeaderLabels(headers);

    /*
     * Natural gas
     */

    if (system->mSecondHeatSource == HeatingSystem::NaturalGas)
    {
        ui->groupBox_NaturalGas->show();

        foreach (const HeatingSystem::FuelIndex &index, system->mNaturalGasIndexes)
        {
            QString date = index.mDate.toString("yyyy-MM-dd");

            //Find the best column to add the new value
            int column = 0;
            for (int i = 0 ; i < ui->tableWidget_NaturalGas->columnCount() ; ++i)
            {
                if (ui->tableWidget_NaturalGas->item(0, i)->text() < date)
                    ++column;
                else break;
            }
            ui->tableWidget_NaturalGas->insertColumn(column);

            //Date
            ui->tableWidget_NaturalGas->setItem(0, column, new QTableWidgetItem(date));

            //Index
            QTableWidgetItem *itemIndex = new QTableWidgetItem;
            itemIndex->setData(Qt::EditRole, index.mIndex);
            ui->tableWidget_NaturalGas->setItem(1, column, itemIndex);
        }

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
    else
    {
        ui->groupBox_NaturalGas->hide();
    }

    /*
     * Electricity
     */

    foreach (const HeatingSystem::FuelIndex &index, system->mElectricityIndexes)
    {
        QString date = index.mDate.toString("yyyy-MM-dd");

        //Find the best column to add the new value
        int column = 0;
        for (int i = 0 ; i < ui->tableWidget_Electricity->columnCount() ; ++i)
        {
            if (ui->tableWidget_Electricity->item(0, i)->text() < date)
                ++column;
            else break;
        }
        ui->tableWidget_Electricity->insertColumn(column);

        //Date
        ui->tableWidget_Electricity->setItem(0, column, new QTableWidgetItem(date));

        //Index
        QTableWidgetItem *itemIndex = new QTableWidgetItem;
        itemIndex->setData(Qt::EditRole, index.mIndex);
        ui->tableWidget_Electricity->setItem(1, column, itemIndex);

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

    updateSums();

    //Connect's
    connect(ui->tableFuels, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Fuel(int,int)));
    connect(ui->tableWidget_NaturalGas, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_NaturalGas(int,int)));
    connect(ui->tableWidget_Electricity, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Electricity(int,int)));
}

void ShowFuelDeliveriesDialog::recordChanged_Fuel(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableFuels->item(x, y);
    QTableWidgetItem* itemUUID = ui->tableFuels->item(x, Column_UUID);

    HeatingSystem::FuelDelivery* delivery = mHeatingSystem->findDelivery(itemUUID->text());

    if      (y == Column_Fuel)     ;
    else if (y == Column_Quantity) delivery->mValue        = item->data(Qt::EditRole).toDouble();
    else if (y == Column_Unit)     delivery->mUnit         = (HeatingSystem::FuelUnits)item->data(Qt::EditRole).toInt();
    else if (y == Column_LHV)      ;
    else if (y == Column_Moisture) delivery->mWoodMoisture = item->data(Qt::EditRole).toDouble();
    else if (y == Column_Bill)     delivery->mBill         = item->data(Qt::EditRole).toDouble();

    //Update LHV
    delivery->computeLHV();
    ui->tableFuels->item(x, Column_LHV)->setData(Qt::EditRole, delivery->mLHV);

    emit settingsChanged();
    mHeatingSystem->save();
}

void ShowFuelDeliveriesDialog::recordChanged_NaturalGas(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_NaturalGas->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_NaturalGas->item(0, y);

    //2. We enter the records for the changed substation.
    for (HeatingSystem::FuelIndex &index : mHeatingSystem->mNaturalGasIndexes)
    {
        //For each saved record, we check if the date is the same as the modified index.
        if (index.mDate.toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            index.mIndex = item->data(Qt::EditRole).toInt();
        }
    }

    emit settingsChanged();
    mHeatingSystem->save();
}

void ShowFuelDeliveriesDialog::recordChanged_Electricity(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_Electricity->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_Electricity->item(0, y);

    //2. We enter the records for the changed substation.
    for (HeatingSystem::FuelIndex &index : mHeatingSystem->mElectricityIndexes)
    {
        //For each saved record, we check if the date is the same as the modified index.
        if (index.mDate.toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            index.mIndex = item->data(Qt::EditRole).toInt();
        }
    }

    emit settingsChanged();
    mHeatingSystem->save();
}

void ShowFuelDeliveriesDialog::deleteFuelDelivery()
{
    //The hash has been saved in the properties of the sender.
    QString hash = sender()->property("record").toString();

    HeatingSystem::FuelDelivery* delivery = mHeatingSystem->findDelivery(hash);

    if (delivery != NULL)
        mHeatingSystem->mFuelDeliveries.removeAll(*delivery);

    //Update table
    for (int i = 0 ; i < ui->tableFuels->rowCount() ; ++i)
    {
        if (ui->tableFuels->item(i, Column_UUID)->text() == hash)
        {
            ui->tableFuels->removeRow(i);
            break;
        }
    }

    emit settingsChanged();
    mHeatingSystem->save();
}
