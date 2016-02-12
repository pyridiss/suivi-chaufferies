#include <QDate>
#include <QSettings>

#include "showmetersrecordsdialog.h"
#include "ui_showmetersrecordsdialog.h"
#include <QDebug>
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

void ShowMetersRecordsDialog::readSettings()
{
    QSettings settings;

    /*
     * The table will be filled with meters records.
     * The first column (number 0) will contain dates and will be hidden (for the user, dates are in the header)
     * but we need dates in a column to make findItems() and sort() function works.
     * The first row (number 0) will contain substation number (substationX) to easily get the corresponding section
     * in settings file.
     */

    //1. Add substations in columns header
    int substationsNumber = settings.beginReadArray("substations");
    ui->tableWidget->setColumnCount(substationsNumber + 1);

    ui->tableWidget->hideRow(0);
    ui->tableWidget->hideColumn(0);

    for (int i = 0 ; i < substationsNumber ; ++i)
    {
        settings.setArrayIndex(i);
        ui->tableWidget->setHorizontalHeaderItem(i+1, new QTableWidgetItem(settings.value("name").toString()));
        ui->tableWidget->setItem(0, i+1, new QTableWidgetItem("substation" + QVariant(i+1).toString()));
    }
    settings.endArray();

    //2. For each substation, iterate to find each record and add it to the table.
    for (int i = 0 ; i < substationsNumber ; ++i)
    {
        int recordsNumber = settings.beginReadArray("substation" + QVariant(i+1).toString());

        for (int j = 0 ; j < recordsNumber ; ++j)
        {
            settings.setArrayIndex(j);
            QDate date = settings.value("date").toDate();
            int index = settings.value("index").toInt();


            if (ui->tableWidget->findItems(date.toString(), Qt::MatchExactly).empty())
            {
                ui->tableWidget->insertRow(1);
                QTableWidgetItem *headerItem = new QTableWidgetItem;
                headerItem->setData(Qt::EditRole, date);
                ui->tableWidget->setVerticalHeaderItem(1, headerItem);
                ui->tableWidget->setItem(1, 0, new QTableWidgetItem(date.toString()));

                QTableWidgetItem *item = new QTableWidgetItem;
                item->setData(Qt::EditRole, index);
                ui->tableWidget->setItem(1, i+1, item);
            }
            else
            {
                int row = ui->tableWidget->findItems(date.toString(), Qt::MatchExactly).first()->row();

                QTableWidgetItem *item = new QTableWidgetItem;
                item->setData(Qt::EditRole, index);
                ui->tableWidget->setItem(row, i+1, item);
            }
        }
        settings.endArray();
    }
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(recordChanged(int, int)));
}

void ShowMetersRecordsDialog::recordChanged(int x, int y)
{
    QTableWidgetItem* item = ui->tableWidget->item(x, y);
    QTableWidgetItem* itemDate = ui->tableWidget->item(x, 0);
    QTableWidgetItem* itemSubstation = ui->tableWidget->item(0, y);

   QSettings settings;

   int size = settings.beginReadArray(itemSubstation->text());
   for (int i = 0 ; i < size ; ++i)
   {
       settings.setArrayIndex(i);
       if (settings.value("date").toDate().toString() == itemDate->text())
       {
           settings.setValue("index", item->data(Qt::EditRole));
       }
   }

   settings.endArray();
}
