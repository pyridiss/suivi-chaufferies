#include <QDate>
#include <QColor>

#include "showmetersrecordsdialog.h"
#include "ui_showmetersrecordsdialog.h"

ShowMetersRecordsDialog::ShowMetersRecordsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowMetersRecordsDialog)
{
    ui->setupUi(this);

    ui->tableWidget->insertRow(0);
}

ShowMetersRecordsDialog::~ShowMetersRecordsDialog()
{
    delete ui;
}

void ShowMetersRecordsDialog::resetValues()
{
    disconnect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged(int, int)));
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount() > 1) ui->tableWidget->removeRow(1);
}

void ShowMetersRecordsDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    /*
     * The table will be filled with meters records.
     * The first column (number 0) will contain dates and will be hidden (for the user, dates are in the header)
     * but we need dates in a column to make findItems() and sort() functions work.
     * The first row (number 0) will contain substation number (substationX) to easily get the corresponding section
     * in settings file. It will be hidden too.
     */

    //1. Add substations in columns header ( = Horizontal header) and in first row.
    //Be careful: we must start at the second column (column == 1) because the first one (column == 0) will be used to store dates.

    ui->tableWidget->setColumnCount(system->mSubstations.length() + 3);

    for (int i = 0 ; i < system->mSubstations.length() ; ++i)
    {
        ui->tableWidget->setHorizontalHeaderItem(i+1, new QTableWidgetItem(system->mSubstations[i].first));
        ui->tableWidget->setItem(0, i+1, new QTableWidgetItem(system->mSubstations[i].first));
    }

    //2. For each substation, iterate to find each record and add it to the table.
    foreach (HeatingSystem::Record record, system->mRecords)
    {
        //Find the corresponding column
        int column = 1;
        while (ui->tableWidget->item(0, column)->text() != record.mSubstation && column < ui->tableWidget->columnCount())
            ++column;

        //If a row has already to added for this date, we must use the same. Else, we create a new one.
        if (ui->tableWidget->findItems(record.mDate.toString("yyyy-MM-dd"), Qt::MatchExactly).empty())
        {
            //Insert a new row at position 1, and store the date in the first column.
            ui->tableWidget->insertRow(1);
            ui->tableWidget->setItem(1, 0, new QTableWidgetItem(record.mDate.toString("yyyy-MM-dd")));

            //Then we store the index.
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setData(Qt::EditRole, record.mValue);
            item->setTextAlignment(Qt::AlignCenter);
            item->setData(Qt::UserRole, record.getHash());
            ui->tableWidget->setItem(1, column, item);
        }
        else
        {
            //A row exists for this date; we get it and add the index in it.
            int row = ui->tableWidget->findItems(record.mDate.toString("yyyy-MM-dd"), Qt::MatchExactly).first()->row();

            QTableWidgetItem *item = new QTableWidgetItem;
            item->setData(Qt::EditRole, record.mValue);
            item->setTextAlignment(Qt::AlignCenter);
            item->setData(Qt::UserRole, record.getHash());
            ui->tableWidget->setItem(row, column, item);
        }
    }

    //3. We add a 'sum' column
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 2, new QTableWidgetItem("Somme"));
    for (int i = 1 ; i < ui->tableWidget->rowCount() ; ++i)
    {
        int sum = 0;
        for (int j = 1 ; j < ui->tableWidget->columnCount() - 2 ; ++j)
        {
            if (ui->tableWidget->item(i, j) != 0)
                sum += ui->tableWidget->item(i, j)->data(Qt::EditRole).toInt();
        }
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(sum));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        ui->tableWidget->setItem(i, ui->tableWidget->columnCount() - 2, item);
    }

    //4. We add a column for the main meter
    if (system->mMainHeatMeter)
    {
        ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Compteur principal"));
        ui->tableWidget->setItem(0, ui->tableWidget->columnCount() - 1, new QTableWidgetItem("mainHeatMeterRecords"));

        foreach (HeatingSystem::Record record, system->mMainHeatMeterRecords)
        {
            //If a row has already to added for this date, we must use the same. Else, we create a new one.
            if (ui->tableWidget->findItems(record.mDate.toString("yyyy-MM-dd"), Qt::MatchExactly).empty())
            {
                //Insert a new row at position 1, and store the date in the first column.
                ui->tableWidget->insertRow(1);
                ui->tableWidget->setItem(1, 0, new QTableWidgetItem(record.mDate.toString("yyyy-MM-dd")));

                //Then we store the index.
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setData(Qt::EditRole, record.mValue);
                item->setTextAlignment(Qt::AlignCenter);
                item->setTextColor(Qt::red);
                item->setData(Qt::UserRole, record.getHash());
                ui->tableWidget->setItem(1, ui->tableWidget->columnCount() - 1, item);
            }
            else
            {
                //A row exists for this date; we get it and add the index in it.
                int row = ui->tableWidget->findItems(record.mDate.toString("yyyy-MM-dd"), Qt::MatchExactly).first()->row();

                QTableWidgetItem *item = new QTableWidgetItem;
                item->setData(Qt::EditRole, record.mValue);
                item->setTextAlignment(Qt::AlignCenter);
                item->setTextColor(Qt::red);
                ui->tableWidget->setItem(row, ui->tableWidget->columnCount() - 1, item);
            }
        }
    }
    else
    {
        //There is no main meter; we can remove the last column.
        ui->tableWidget->removeColumn(ui->tableWidget->columnCount() - 1);
    }

    //5. All indexes are stored in the table. We must sort them now (by date).
    //A '0' is stored in the cell (0, 0) to prevent the first row to be moved.
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("0"));
    ui->tableWidget->sortItems(0);

    //6. Rows are sorted. We copy the first column in vertical header.
    QStringList headers;
    headers.push_back("0"); //First row with substations numbers.

    for (int i = 1 ; i < ui->tableWidget->rowCount() ; ++i)
    {
        QString q = ui->tableWidget->item(i, 0)->text();
        q = QDate::fromString(q, "yyyy-MM-dd").toString("dd MMMM yyyy");
        headers.push_back(q);
    }

    ui->tableWidget->setVerticalHeaderLabels(headers);

    //7. Empty cells should be non-editable
    for (int i = 0 ; i < ui->tableWidget->rowCount() ; ++i)
    {
        for (int j = 0 ; j < ui->tableWidget->columnCount() ; ++j)
        {
            if (ui->tableWidget->item(i, j) == 0)
            {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setFlags(Qt::NoItemFlags);
                item->setBackgroundColor(QColor::fromRgb(224, 224, 224));
                ui->tableWidget->setItem(i, j, item);
            }
        }
    }

    //8. We can hide first row and first column.
    ui->tableWidget->hideRow(0);
    ui->tableWidget->hideColumn(0);

    //9. We connect the table to the method recordChanged() to automatically save any change made be the user.
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged(int, int)));
}

void ShowMetersRecordsDialog::recordChanged(int x, int y)
{
    //1. First, we get the item changed.
    QTableWidgetItem* item = ui->tableWidget->item(x, y);

    //2. We enter the records (substations and main heat meter)
    for (HeatingSystem::Record &record : mHeatingSystem->mRecords)
    {
        if (record.getHash() == item->data(Qt::UserRole))
        {
            record.mValue = item->data(Qt::EditRole).toDouble();
        }
    }
    for (HeatingSystem::Record &record : mHeatingSystem->mMainHeatMeterRecords)
    {
        if (record.getHash() == item->data(Qt::UserRole))
        {
            record.mValue = item->data(Qt::EditRole).toDouble();
        }
    }

    //3. Update sums
    for (int i = 1 ; i < ui->tableWidget->rowCount() ; ++i)
    {
        int sum = 0;
        for (int j = 1 ; j < ui->tableWidget->columnCount() - 2 ; ++j)
        {
            sum += ui->tableWidget->item(i, j)->data(Qt::EditRole).toInt();
        }
        if (sum != 0) ui->tableWidget->item(i, ui->tableWidget->columnCount() - 2)->setText(QString::number(sum));
    }

    emit settingsChanged();
    mHeatingSystem->save();
}
