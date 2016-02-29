#include <QDate>
#include <QPushButton>

#include <delegates/comboboxdelegate.h>
#include <delegates/doublespinboxdelegate.h>

#include "showfueldeliveriesdialog.h"
#include "ui_showfueldeliveriesdialog.h"

ShowFuelDeliveriesDialog::ShowFuelDeliveriesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowFuelDeliveriesDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(settingsChanged()), this, SLOT(updateSums()));

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
    delegateEnergy_Wood->setPrecision(4);
    ui->tableWidget_Wood->setItemDelegateForColumn(4, delegateEnergy_Wood);

    //Wood bill will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateBill_Wood = new DoubleSpinBoxDelegate(this);
    delegateBill_Wood->setSuffix(" €");
    delegateBill_Wood->setPrecision(6);
    ui->tableWidget_Wood->setItemDelegateForColumn(5, delegateBill_Wood);

    //Wood price per MWh will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegatePricePerMWh_Wood = new DoubleSpinBoxDelegate(this);
    delegatePricePerMWh_Wood->setSuffix(" € / MWh");
    delegatePricePerMWh_Wood->setPrecision(4);
    ui->tableWidget_Wood->setItemDelegateForColumn(6, delegatePricePerMWh_Wood);

    //Last column allows user to delete records
    ui->tableWidget_Wood->setColumnWidth(7, 34);

    /*
     * Configure table 'Secondary fuel' columns
     */

    //Date shouldn't be shown
    ui->tableWidget_SecondaryFuel->hideColumn(0);

    //Fuel energy will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateEnergy_Fuel = new DoubleSpinBoxDelegate(this);
    delegateEnergy_Fuel->setSuffix(" MWh");
    delegateEnergy_Fuel->setPrecision(4);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(2, delegateEnergy_Fuel);

    //Fuel bill will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateBill_Fuel = new DoubleSpinBoxDelegate(this);
    delegateBill_Fuel->setSuffix(" €");
    delegateBill_Fuel->setPrecision(6);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(3, delegateBill_Fuel);

    //Fuel price per MWh will be shown by a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegatePricePerMWh_Fuel = new DoubleSpinBoxDelegate(this);
    delegatePricePerMWh_Fuel->setSuffix(" € / MWh");
    delegatePricePerMWh_Fuel->setPrecision(4);
    ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(4, delegatePricePerMWh_Fuel);

    //Last column allows user to delete records
    ui->tableWidget_SecondaryFuel->setColumnWidth(5, 34);

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
    disconnect(ui->tableWidget_Wood, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Wood(int, int)));
    while (ui->tableWidget_Wood->rowCount() > 0) ui->tableWidget_Wood->removeRow(0);

    disconnect(ui->tableWidget_SecondaryFuel, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_SecondaryFuel(int, int)));
    while (ui->tableWidget_SecondaryFuel->rowCount() > 0) ui->tableWidget_SecondaryFuel->removeRow(0);

    disconnect(ui->tableWidget_NaturalGas, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_NaturalGas(int, int)));
    while (ui->tableWidget_NaturalGas->columnCount() > 0) ui->tableWidget_NaturalGas->removeColumn(0);

    disconnect(ui->tableWidget_Electricity, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Electricity(int, int)));
    while (ui->tableWidget_Electricity->columnCount() > 0) ui->tableWidget_Electricity->removeColumn(0);
}

void ShowFuelDeliveriesDialog::updateSums()
{
    if (mHeatingSystem->mMainHeatSource == HeatingSystem::WoodChips || mHeatingSystem->mMainHeatSource == HeatingSystem::Pellets)
    {
        for (int i = 0 ; i < ui->tableWidget_Wood->rowCount() ; ++i)
        {
            double quantity = ui->tableWidget_Wood->item(i, 1)->data(Qt::EditRole).toDouble();
            int unit        = ui->tableWidget_Wood->item(i, 2)->text().toInt();
            double moisture = ui->tableWidget_Wood->item(i, 3)->data(Qt::EditRole).toDouble() / 100;
            double bill     = ui->tableWidget_Wood->item(i, 5)->data(Qt::EditRole).toDouble();

            //Assuming 20% softwood + 80% hardwood
            double energy  = 0;
            double lhv     = (0.2 * (5.1 - moisture * 100 / 16.4) + 0.8 * (4.9 - moisture * 100 / 18.34));
            double density = (0.2 * ((160 * moisture * 100) / (100 - moisture * 100) + 160) + 0.8 * ((220 * moisture * 100) / (100 - moisture * 100) + 220)) / 1000;

            if (unit == 0) energy = quantity * lhv * density;
            if (unit == 1) energy = quantity * lhv;
            if (unit == 2) energy = quantity;

            if (energy == 0) energy = 1;

            ui->tableWidget_Wood->item(i, 4)->setData(Qt::DisplayRole, energy);
            ui->tableWidget_Wood->item(i, 6)->setData(Qt::DisplayRole, bill / energy);
        }
    }

    if (mHeatingSystem->mSecondHeatSource == HeatingSystem::FuelOil || mHeatingSystem->mSecondHeatSource == HeatingSystem::Propane)
    {
        for (int i = 0 ; i < ui->tableWidget_SecondaryFuel->rowCount() ; ++i)
        {
            double quantity = ui->tableWidget_SecondaryFuel->item(i, 1)->data(Qt::EditRole).toDouble();
            double bill     = ui->tableWidget_SecondaryFuel->item(i, 3)->data(Qt::EditRole).toDouble();

            double lhv    = (mHeatingSystem->mSecondHeatSource == HeatingSystem::FuelOil) ? 9.96 : 12.9;
            double energy = quantity * lhv / 1000;

            if (energy == 0) energy = 1;

            ui->tableWidget_SecondaryFuel->item(i, 2)->setData(Qt::DisplayRole, energy);
            ui->tableWidget_SecondaryFuel->item(i, 4)->setData(Qt::DisplayRole, bill / energy);
        }
    }
}

void ShowFuelDeliveriesDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    /*
     * Wood (wood chips and pellets)
     */

    if (system->mMainHeatSource == HeatingSystem::WoodChips || system->mMainHeatSource == HeatingSystem::Pellets)
    {
        ui->groupBox_Wood->show();

        foreach (const HeatingSystem::FuelDelivery &delivery, system->mWoodDeliveries)
        {
            ui->tableWidget_Wood->insertRow(0);

            //Date
            ui->tableWidget_Wood->setItem(0, 0, new QTableWidgetItem(delivery.mDate.toString("yyyy-MM-dd")));

            //Quantity
            QTableWidgetItem *itemQuantity = new QTableWidgetItem;
            itemQuantity->setData(Qt::EditRole, delivery.mValue);
            itemQuantity->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Wood->setItem(0, 1, itemQuantity);

            //Unit
            QTableWidgetItem *itemUnit = new QTableWidgetItem(QString::number(delivery.mWoodUnit));
            ui->tableWidget_Wood->setItem(0, 2, itemUnit);

            //Moisture
            QTableWidgetItem *itemMoisture = new QTableWidgetItem;
            itemMoisture->setData(Qt::EditRole, delivery.mWoodMoisture);
            ui->tableWidget_Wood->setItem(0, 3, itemMoisture);

            //Energy
            QTableWidgetItem *itemEnergy = new QTableWidgetItem();
            itemEnergy->setFlags(Qt::NoItemFlags);
            ui->tableWidget_Wood->setItem(0, 4, itemEnergy);

            //Bill
            QTableWidgetItem *itemBill = new QTableWidgetItem;
            itemBill->setData(Qt::EditRole, delivery.mBill);
            ui->tableWidget_Wood->setItem(0, 5, itemBill);

            //Price per MWh
            QTableWidgetItem *itemPrice = new QTableWidgetItem();
            itemPrice->setFlags(Qt::NoItemFlags);
            ui->tableWidget_Wood->setItem(0, 6, itemPrice);

            //Delete button
            QPushButton* itemDelete = new QPushButton(ui->tableWidget_Wood);
            itemDelete->setIcon(QIcon::fromTheme("edit-delete"));
            itemDelete->setProperty("record", delivery.getHash());
            ui->tableWidget_Wood->setCellWidget(0, 7, itemDelete);
            connect(itemDelete, SIGNAL(clicked(bool)), this, SLOT(deleteWoodDelivery()));
        }

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
    else
    {
        ui->groupBox_Wood->hide();
        resize(size().width(), layout()->minimumSize().height());
    }

    /*
     * Oil fuel and propane
     */

    if (system->mSecondHeatSource == HeatingSystem::FuelOil || system->mSecondHeatSource == HeatingSystem::Propane)
    {
        ui->groupBox_SecondaryFuel->show();

        //Delegate quantity column to show a suffix
        DoubleSpinBoxDelegate *delegateQuantity_SecondaryFuel = new DoubleSpinBoxDelegate(this);
        delegateQuantity_SecondaryFuel->setSuffix((system->mSecondHeatSource == HeatingSystem::FuelOil) ? " litres" : " kg");
        delegateQuantity_SecondaryFuel->setPrecision(4);
        ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(1, delegateQuantity_SecondaryFuel);

        foreach (const HeatingSystem::FuelDelivery &delivery, system->mFossilFuelDeliveries)
        {
            ui->tableWidget_SecondaryFuel->insertRow(0);

            //Date
            ui->tableWidget_SecondaryFuel->setItem(0, 0, new QTableWidgetItem(delivery.mDate.toString("yyyy-MM-dd")));

            //Quantity
            QTableWidgetItem *itemQuantity = new QTableWidgetItem;
            itemQuantity->setData(Qt::EditRole, delivery.mValue);
            itemQuantity->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_SecondaryFuel->setItem(0, 1, itemQuantity);

            //Energy
            QTableWidgetItem *itemEnergy = new QTableWidgetItem();
            itemEnergy->setFlags(Qt::NoItemFlags);
            ui->tableWidget_SecondaryFuel->setItem(0, 2, itemEnergy);

            //Bill
            QTableWidgetItem *itemBill = new QTableWidgetItem;
            itemBill->setData(Qt::EditRole, delivery.mBill);
            ui->tableWidget_SecondaryFuel->setItem(0, 3, itemBill);

            //Price per MWh
            QTableWidgetItem *itemPrice = new QTableWidgetItem();
            itemPrice->setFlags(Qt::NoItemFlags);
            ui->tableWidget_SecondaryFuel->setItem(0, 4, itemPrice);

            //Delete button
            QPushButton* itemDelete = new QPushButton(ui->tableWidget_Wood);
            itemDelete->setIcon(QIcon::fromTheme("edit-delete"));
            itemDelete->setProperty("record", delivery.getHash());
            ui->tableWidget_SecondaryFuel->setCellWidget(0, 5, itemDelete);
            connect(itemDelete, SIGNAL(clicked(bool)), this, SLOT(deleteSecondaryFuelDelivery()));
        }

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
    else // No secondary fuel, or natural gas, or elecrticity
    {
        ui->groupBox_SecondaryFuel->hide();
    }

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
    connect(ui->tableWidget_Wood, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Wood(int,int)));
    connect(ui->tableWidget_SecondaryFuel, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_SecondaryFuel(int,int)));
    connect(ui->tableWidget_NaturalGas, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_NaturalGas(int,int)));
    connect(ui->tableWidget_Electricity, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged_Electricity(int,int)));
}

void ShowFuelDeliveriesDialog::recordChanged_Wood(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_Wood->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_Wood->item(x, 0);

    //2. We enter the records for the changed substation.
    for (HeatingSystem::FuelDelivery &delivery : mHeatingSystem->mWoodDeliveries)
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
                    delivery.mWoodUnit = item->data(Qt::EditRole).toInt();
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
}

void ShowFuelDeliveriesDialog::recordChanged_SecondaryFuel(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_SecondaryFuel->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_SecondaryFuel->item(x, 0);

    //2. We enter the records for the changed substation.
    for (HeatingSystem::FuelDelivery &delivery : mHeatingSystem->mFossilFuelDeliveries)
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
                case 3: //Bill
                    delivery.mBill = item->data(Qt::EditRole).toDouble();
                    break;
            }
        }
    }

    emit settingsChanged();
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
}

void ShowFuelDeliveriesDialog::deleteWoodDelivery()
{
    //The corresponding date has been saved in the properties of the sender.
    QString hash = sender()->property("record").toString();
    QString date; //TODO: Remove this workaround

    foreach (const HeatingSystem::FuelDelivery &delivery, mHeatingSystem->mWoodDeliveries)
    {
        if (delivery.getHash() == hash)
        {
            date = delivery.mDate.toString("yy-MM-dd");
            mHeatingSystem->mWoodDeliveries.removeAll(delivery);
            break;
        }
    }

    //Update table
    for (int i = 0 ; i < ui->tableWidget_Wood->rowCount() ; ++i)
    {
        if (ui->tableWidget_Wood->item(i, 0)->text() == date)
        {
            ui->tableWidget_Wood->removeRow(i);
            break;
        }
    }

    emit settingsChanged();
}

void ShowFuelDeliveriesDialog::deleteSecondaryFuelDelivery()
{
    //1. The corresponding date has been saved in the properties of the sender.
    QString hash = sender()->property("record").toString();
    QString date; //TODO: Remove this workaround

    foreach (const HeatingSystem::FuelDelivery &delivery, mHeatingSystem->mFossilFuelDeliveries)
    {
        if (delivery.getHash() == hash)
        {
            date = delivery.mDate.toString("yy-MM-dd");
            mHeatingSystem->mFossilFuelDeliveries.removeAll(delivery);
            break;
        }
    }

    //Update table
    for (int i = 0 ; i < ui->tableWidget_SecondaryFuel->rowCount() ; ++i)
    {
        if (ui->tableWidget_SecondaryFuel->item(i, 0)->text() == date)
        {
            ui->tableWidget_SecondaryFuel->removeRow(i);
            break;
        }
    }

    emit settingsChanged();
}
