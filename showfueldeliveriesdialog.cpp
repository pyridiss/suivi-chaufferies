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

    //Moisture
    DoubleSpinBoxDelegate *delegateMoisture = new DoubleSpinBoxDelegate(this);
    delegateMoisture->setSuffix(" %");
    delegateMoisture->setMaximum(100);
    ui->tableFuels->setItemDelegateForColumn(Column_Moisture, delegateMoisture);

    //Energy
    DoubleSpinBoxDelegate *delegateEnergy = new DoubleSpinBoxDelegate(this);
    delegateEnergy->setSuffix(" MWh");
    delegateEnergy->setPrecision(4);
    ui->tableFuels->setItemDelegateForColumn(Column_Energy, delegateEnergy);

    //Bill
    DoubleSpinBoxDelegate *delegateBill = new DoubleSpinBoxDelegate(this);
    delegateBill->setSuffix(" €");
    delegateBill->setPrecision(6);
    ui->tableFuels->setItemDelegateForColumn(Column_Bill, delegateBill);

    //Energy Price
    DoubleSpinBoxDelegate *delegateEnergyPrice = new DoubleSpinBoxDelegate(this);
    delegateEnergyPrice->setSuffix(" € / MWh");
    delegateEnergyPrice->setPrecision(4);
    ui->tableFuels->setItemDelegateForColumn(Column_EnergyPrice, delegateEnergyPrice);

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
        double quantity               = ui->tableFuels->item(i, Column_Quantity)->data(Qt::EditRole).toDouble();
        HeatingSystem::FuelUnits unit = (HeatingSystem::FuelUnits) ui->tableFuels->item(i, Column_Unit)    ->text().toInt();
        double moisture               = ui->tableFuels->item(i, Column_Moisture)->data(Qt::EditRole).toDouble() / 100;
        double bill                   = ui->tableFuels->item(i, Column_Bill)    ->data(Qt::EditRole).toDouble();

        //Assuming 20% softwood + 80% hardwood
        double energy  = 0;
        double lhv     = (0.2 * (5.1 - moisture * 100 / 16.4) + 0.8 * (4.9 - moisture * 100 / 18.34));
        double density = (0.2 * ((160 * moisture * 100) / (100 - moisture * 100) + 160) + 0.8 * ((220 * moisture * 100) / (100 - moisture * 100) + 220)) / 1000;

        switch (unit)
        {
            break; //TODO
        }

        if (unit == HeatingSystem::ApparentCubicMeters) energy = quantity * lhv * density;
        if (unit == HeatingSystem::Tons) energy = quantity * lhv;
        if (unit == HeatingSystem::MWh) energy = quantity;

        if (energy == 0) energy = 1;

        ui->tableFuels->item(i, Column_Energy)->setData(Qt::DisplayRole, energy);
        ui->tableFuels->item(i, Column_EnergyPrice)->setData(Qt::DisplayRole, bill / energy);
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

        //Date
        ui->tableFuels->setItem(0, 0, new QTableWidgetItem(delivery.mDate.toString("yyyy-MM-dd")));

        //Quantity
        QTableWidgetItem *itemQuantity = new QTableWidgetItem;
        itemQuantity->setData(Qt::EditRole, delivery.mValue);
        itemQuantity->setTextAlignment(Qt::AlignCenter);
        ui->tableFuels->setItem(0, Column_Quantity, itemQuantity);

        //Unit
        QTableWidgetItem *itemUnit = new QTableWidgetItem(QString::number(delivery.mUnit));
        ui->tableFuels->setItem(0, Column_Unit, itemUnit);

        //Moisture
        QTableWidgetItem *itemMoisture = new QTableWidgetItem;
        itemMoisture->setData(Qt::EditRole, delivery.mWoodMoisture);
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
    ui->tableFuels->sortItems(0);

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
    QTableWidgetItem* itemDate = ui->tableFuels->item(x, 0);

    //2. We enter the records for the changed substation.
    for (HeatingSystem::FuelDelivery &delivery : mHeatingSystem->mFuelDeliveries)
    {
        //For each saved record, we check if the date is the same as the modified index.
        if (delivery.mDate.toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            switch (y)
            {
                case 1: //Quantity
                    delivery.mValue = item->data(Qt::EditRole).toDouble();
                    break;
                case 2: //Unit
//                    delivery.mUnit = item->data(Qt::EditRole).toInt();
                    break;
                case 3: //Moisture
                    delivery.mWoodMoisture = item->data(Qt::EditRole).toDouble();
                    break;
                case 5: //Bill
                    delivery.mBill = item->data(Qt::EditRole).toDouble();
                    break;
            }
        }
    }

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
    //The corresponding date has been saved in the properties of the sender.
    QString hash = sender()->property("record").toString();
    QString date; //TODO: Remove this workaround

    foreach (const HeatingSystem::FuelDelivery &delivery, mHeatingSystem->mFuelDeliveries)
    {
        if (delivery.getHash() == hash)
        {
            date = delivery.mDate.toString("yy-MM-dd");
            mHeatingSystem->mFuelDeliveries.removeAll(delivery);
            break;
        }
    }

    //Update table
    for (int i = 0 ; i < ui->tableFuels->rowCount() ; ++i)
    {
        if (ui->tableFuels->item(i, Column_Date)->text() == date)
        {
            ui->tableFuels->removeRow(i);
            break;
        }
    }

    emit settingsChanged();
    mHeatingSystem->save();
}
