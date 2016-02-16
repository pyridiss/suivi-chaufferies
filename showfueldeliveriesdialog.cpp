#include <QSettings>
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
    QSettings settings;
    int mainFuel      = settings.value("boilerRoom/mainHeatSource").toInt();
    int secondaryFuel = settings.value("boilerRoom/secondaryHeatSource").toInt();

    //1. Wood
    if (mainFuel == 0 || mainFuel == 1)
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

    //2. Secondary fuel
    if (secondaryFuel == 2 || secondaryFuel == 3)
    {
        for (int i = 0 ; i < ui->tableWidget_SecondaryFuel->rowCount() ; ++i)
        {
            double quantity = ui->tableWidget_SecondaryFuel->item(i, 1)->data(Qt::EditRole).toDouble();
            double bill     = ui->tableWidget_SecondaryFuel->item(i, 3)->data(Qt::EditRole).toDouble();

            double lhv    = (secondaryFuel == 2) ? 9.96 : 12.9;
            double energy = quantity * lhv / 1000;

            if (energy == 0) energy = 1;

            ui->tableWidget_SecondaryFuel->item(i, 2)->setData(Qt::DisplayRole, energy);
            ui->tableWidget_SecondaryFuel->item(i, 4)->setData(Qt::DisplayRole, bill / energy);
        }
    }
}

void ShowFuelDeliveriesDialog::readSettings()
{
    QSettings settings;

    int mainFuel      = settings.value("boilerRoom/mainHeatSource").toInt();
    int secondaryFuel = settings.value("boilerRoom/secondaryHeatSource").toInt();

    //1. Wood
    if (mainFuel == 0 || mainFuel == 1)
    {
        ui->groupBox_Wood->show();

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

            //Delete button
            QPushButton* itemDelete = new QPushButton(ui->tableWidget_Wood);
            itemDelete->setIcon(QIcon::fromTheme("edit-delete"));
            itemDelete->setProperty("record", date.toString("yyyy-MM-dd"));
            ui->tableWidget_Wood->setCellWidget(0, 7, itemDelete);
            connect(itemDelete, SIGNAL(clicked(bool)), this, SLOT(deleteWoodDelivery()));

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
    }
    else
    {
        ui->groupBox_Wood->hide();
        resize(size().width(), layout()->minimumSize().height());
    }

    //2. Secondary fuel
    if (secondaryFuel == 2 || secondaryFuel == 3) //2 is fuel oil, 3 is propane
    {
        ui->groupBox_SecondaryFuel->show();

        //Delegate quantity column to show a suffix
        DoubleSpinBoxDelegate *delegateQuantity_SecondaryFuel = new DoubleSpinBoxDelegate(this);
        delegateQuantity_SecondaryFuel->setSuffix((secondaryFuel == 2) ? " litres" : " kg");
        delegateQuantity_SecondaryFuel->setPrecision(4);
        ui->tableWidget_SecondaryFuel->setItemDelegateForColumn(1, delegateQuantity_SecondaryFuel);

        int secondaryFuelDeliviriesNumber = settings.beginReadArray("secondaryFuelDeliveries");
        for (int i = 0 ; i < secondaryFuelDeliviriesNumber ; ++i)
        {
            settings.setArrayIndex(i);
            ui->tableWidget_SecondaryFuel->insertRow(0);
            QDate date = settings.value("date").toDate();
            double quantity = settings.value("quantity").toDouble();
            double bill = settings.value("bill").toDouble();

            //Date
            ui->tableWidget_SecondaryFuel->setItem(0, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));

            //Quantity
            QTableWidgetItem *itemQuantity = new QTableWidgetItem;
            itemQuantity->setData(Qt::EditRole, quantity);
            itemQuantity->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_SecondaryFuel->setItem(0, 1, itemQuantity);

            //Energy
            QTableWidgetItem *itemEnergy = new QTableWidgetItem();
            itemEnergy->setFlags(Qt::NoItemFlags);
            ui->tableWidget_SecondaryFuel->setItem(0, 2, itemEnergy);

            //Bill
            QTableWidgetItem *itemBill = new QTableWidgetItem;
            itemBill->setData(Qt::EditRole, bill);
            ui->tableWidget_SecondaryFuel->setItem(0, 3, itemBill);

            //Price per MWh
            QTableWidgetItem *itemPrice = new QTableWidgetItem();
            itemPrice->setFlags(Qt::NoItemFlags);
            ui->tableWidget_SecondaryFuel->setItem(0, 4, itemPrice);

            //Delete button
            QPushButton* itemDelete = new QPushButton(ui->tableWidget_Wood);
            itemDelete->setIcon(QIcon::fromTheme("edit-delete"));
            itemDelete->setProperty("record", date.toString("yyyy-MM-dd"));
            ui->tableWidget_SecondaryFuel->setCellWidget(0, 5, itemDelete);
            connect(itemDelete, SIGNAL(clicked(bool)), this, SLOT(deleteSecondaryFuelDelivery()));

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
    }
    else // No secondary fuel, or natural gas, or elecrticity
    {
        ui->groupBox_SecondaryFuel->hide();
    }

    //3. Natural gas
    if (secondaryFuel == 1)
    {
        ui->groupBox_NaturalGas->show();

        int naturalGasIndexesNumber = settings.beginReadArray("naturalGasIndex");
        for (int i = 0 ; i < naturalGasIndexesNumber ; ++i)
        {
            settings.setArrayIndex(i);
            QString date = settings.value("date").toDate().toString("yyyy-MM-dd");
            int index = settings.value("index").toInt();

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
            itemIndex->setData(Qt::EditRole, index);
            ui->tableWidget_NaturalGas->setItem(1, column, itemIndex);

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
    }
    else
    {
        ui->groupBox_NaturalGas->hide();
    }

    //4. Electricity
    int electricityIndexesNumber = settings.beginReadArray("electricityIndex");
    for (int i = 0 ; i < electricityIndexesNumber ; ++i)
    {
        settings.setArrayIndex(i);
        QString date = settings.value("date").toDate().toString("yyyy-MM-dd");
        int index = settings.value("index").toInt();

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
        itemIndex->setData(Qt::EditRole, index);
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

    settings.endArray();

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

    QSettings settings;

    //2. We enter the records for the changed substation.
    int size = settings.beginReadArray("woodDeliveries");
    for (int i = 0 ; i < size ; ++i)
    {
        settings.setArrayIndex(i);
        //For each saved record, we check if the date is the same as the modified index.
        if (settings.value("date").toDate().toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            switch (y)
            {
                case 1: //Quantity
                    settings.setValue("quantity", item->data(Qt::EditRole));
                    break;
                case 2: //Unit
                    settings.setValue("unit", item->data(Qt::EditRole));
                    break;
                case 3: //Moisture
                    settings.setValue("moisture", item->data(Qt::EditRole));
                    break;
                case 5: //Bill
                    settings.setValue("bill", item->data(Qt::EditRole));
                    break;
            }
        }
    }

    settings.endArray();

    //3. Update sums

    emit settingsChanged();
}

void ShowFuelDeliveriesDialog::recordChanged_SecondaryFuel(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_SecondaryFuel->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_SecondaryFuel->item(x, 0);

    QSettings settings;

    //2. We enter the records for the changed substation.
    int size = settings.beginReadArray("secondaryFuelDeliveries");
    for (int i = 0 ; i < size ; ++i)
    {
        settings.setArrayIndex(i);
        //For each saved record, we check if the date is the same as the modified index.
        if (settings.value("date").toDate().toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            switch (y)
            {
                case 1: //Quantity
                    settings.setValue("quantity", item->data(Qt::EditRole));
                    break;
                case 3: //Bill
                    settings.setValue("bill", item->data(Qt::EditRole));
                    break;
            }
        }
    }

    settings.endArray();

    emit settingsChanged();
}

void ShowFuelDeliveriesDialog::recordChanged_NaturalGas(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_NaturalGas->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_NaturalGas->item(0, y);

    QSettings settings;

    //2. We enter the records for the changed substation.
    int size = settings.beginReadArray("naturalGasIndex");
    for (int i = 0 ; i < size ; ++i)
    {
        settings.setArrayIndex(i);
        //For each saved record, we check if the date is the same as the modified index.
        if (settings.value("date").toDate().toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            settings.setValue("index", item->data(Qt::EditRole));
        }
    }

    settings.endArray();

    emit settingsChanged();
}

void ShowFuelDeliveriesDialog::recordChanged_Electricity(int x, int y)
{
    //1. First, we get the item changed and corresponding date and substation.
    QTableWidgetItem* item = ui->tableWidget_Electricity->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget_Electricity->item(0, y);

    QSettings settings;

    //2. We enter the records for the changed substation.
    int size = settings.beginReadArray("electricityIndex");
    for (int i = 0 ; i < size ; ++i)
    {
        settings.setArrayIndex(i);
        //For each saved record, we check if the date is the same as the modified index.
        if (settings.value("date").toDate().toString("yyyy-MM-dd") == itemDate->text())
        {
            //Update value
            settings.setValue("index", item->data(Qt::EditRole));
        }
    }

    settings.endArray();

    emit settingsChanged();
}

void ShowFuelDeliveriesDialog::deleteWoodDelivery()
{
    //1. The corresponding date has been saved in the properties of the sender.
    QString date = sender()->property("record").toString();

    QSettings settings;

    //2. QSettings has no good function to remove one entry from arrays; we will write a new array from ui->tableWidget_Wood
    settings.remove("woodDeliveries");
    settings.beginWriteArray("woodDeliveries");
    int index = 0;
    int rowToRemove = -1;
    for (int i = 0 ; i < ui->tableWidget_Wood->rowCount() ; ++i)
    {
        settings.setArrayIndex(index);
        if (ui->tableWidget_Wood->item(i, 0)->text() != date)
        {
            settings.setValue("date",       QDate::fromString(ui->tableWidget_Wood->item(i, 0)->text(), "yyyy-MM-dd"));
            settings.setValue("quantity",   ui->tableWidget_Wood->item(i, 1)->data(Qt::EditRole));
            settings.setValue("unit",       ui->tableWidget_Wood->item(i, 2)->data(Qt::EditRole));
            settings.setValue("moisture",   ui->tableWidget_Wood->item(i, 3)->data(Qt::EditRole));
            settings.setValue("bill",       ui->tableWidget_Wood->item(i, 5)->data(Qt::EditRole));
            ++index;
        }
        else rowToRemove = i;
    }

    settings.endArray();

    //3. Update table
    if (rowToRemove != -1) ui->tableWidget_Wood->removeRow(rowToRemove);

    emit settingsChanged();
}

void ShowFuelDeliveriesDialog::deleteSecondaryFuelDelivery()
{
    //1. The corresponding date has been saved in the properties of the sender.
    QString date = sender()->property("record").toString();

    QSettings settings;

    //2. QSettings has no good function to remove one entry from arrays; we will write a new array from ui->tableWidget_SecondaryFuel
    settings.remove("secondaryFuelDeliveries");
    settings.beginWriteArray("secondaryFuelDeliveries");
    int index = 0;
    int rowToRemove = -1;
    for (int i = 0 ; i < ui->tableWidget_SecondaryFuel->rowCount() ; ++i)
    {
        settings.setArrayIndex(index);
        if (ui->tableWidget_SecondaryFuel->item(i, 0)->text() != date)
        {
            settings.setValue("date",       QDate::fromString(ui->tableWidget_SecondaryFuel->item(i, 0)->text(), "yyyy-MM-dd"));
            settings.setValue("quantity",   ui->tableWidget_SecondaryFuel->item(i, 1)->data(Qt::EditRole));
            settings.setValue("bill",       ui->tableWidget_SecondaryFuel->item(i, 3)->data(Qt::EditRole));
            ++index;
        }
        else rowToRemove = i;
    }

    settings.endArray();

    //3. Update table
    if (rowToRemove != -1) ui->tableWidget_SecondaryFuel->removeRow(rowToRemove);

    emit settingsChanged();
}
