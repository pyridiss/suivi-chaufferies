#include <QSettings>

#include "qcustomplot/qcustomplot.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mConfigurationDialog = new ConfigurationDialog(this);
    mAddFullDeliveryDialog = new AddFuelDeliveryDialog(this);
    mAddMetersRecordDialog = new AddMetersRecordDialog(this);
    mShowMetersRecordsDialog = new ShowMetersRecordsDialog(this);

    connect(mConfigurationDialog, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
    connect(mAddMetersRecordDialog, SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));
    connect(mShowMetersRecordsDialog, SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));

    /*
     * Set Energy Consumption chart design
     */

    //X-Axis: X-Axis is based on time and range match the heating season.
    ui->chart_EnergyConsumption->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    double beginningHeatingSeason = QDateTime(QDate(2015, 07, 01), QTime(0, 0, 0)).toTime_t();
    double endHeatingSeason = QDateTime(QDate(2016, 06, 30), QTime(0, 0, 0)).toTime_t();
    ui->chart_EnergyConsumption->xAxis->setRange(beginningHeatingSeason, endHeatingSeason);

    //X-Axis: Set a format for dates
    ui->chart_EnergyConsumption->xAxis->setDateTimeFormat("der MMM\nyyyy");
    ui->chart_EnergyConsumption->xAxis->setTickLabelFont(QFont(QFont().family(), 7));

    //X-Axis: manually add ticks
    ui->chart_EnergyConsumption->xAxis->setAutoTicks(false);
    QVector<double> ticks;
    for (int i = 7 ; i <= 7+12 ; ++i)
    {
        ticks.push_back(QDateTime(QDate(2015+(i-1)/12, (i-1)%12+1, 01), QTime(0, 0, 0)).toTime_t());
    }
    ui->chart_EnergyConsumption->xAxis->setTickVector(ticks);

    //Y-Axis: set range lower
    ui->chart_EnergyConsumption->yAxis->setRangeLower(0);

    //Add graphs. 0 will be 'sum of meters in substations'.
    ui->chart_EnergyConsumption->addGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSettings()
{
    QSettings settings;

    QString name = "<p align=\"center\"><span style=\"font-weight:600;\">";
    name += settings.value("boilerRoom/boilerRoomName", "Nom de la chaufferie").toString();
    name += "</span></p>";

    ui->labelBoilerRoomName->setText(name);

    updateEnergyConsumptionChart();
}

void MainWindow::updateEnergyConsumptionChart()
{
    QSettings settings;
    QMap<QDate, int> meterRecords;

    int substationsNumber = settings.value("substations/size").toInt();

    for (int i = 0 ; i < substationsNumber ; ++i)
    {
        int recordsNumber = settings.beginReadArray("substation" + QVariant(i+1).toString());

        for (int j = 0 ; j < recordsNumber ; ++j)
        {
            //Get the date and the index of the record 'j' for the substation 'i'.
            settings.setArrayIndex(j);
            QDate date = settings.value("date").toDate();
            int index = settings.value("index").toInt();

            if (meterRecords.find(date) == meterRecords.end())
                meterRecords.insert(date, index);
            else meterRecords[date] += index;
        }
        settings.endArray();
    }

    QVector<double> x(meterRecords.size()), y(meterRecords.size());

    QMap<QDate, int>::iterator it = meterRecords.begin();
    for (int i = 0 ; i < meterRecords.size() ; ++i)
    {
        x[i] = QDateTime(it.key()).toTime_t();
        y[i] = it.value();
        ++it;
    }

    ui->chart_EnergyConsumption->graph(0)->setData(x, y);
    ui->chart_EnergyConsumption->graph(0)->rescaleValueAxis();

    ui->chart_EnergyConsumption->replot();
}

void MainWindow::on_actionConfigureBoilerRoom_triggered()
{
    mConfigurationDialog->readSettings();
    mConfigurationDialog->show();
}

void MainWindow::on_actionExport_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionSend_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionDownloadData_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionShowDeliveries_triggered()
{
    QMessageBox::information(this, "Information", "Cette fonction n'est pas encore implémentée.");
}

void MainWindow::on_actionShowMetersRecord_triggered()
{
    mShowMetersRecordsDialog->resetValues();
    mShowMetersRecordsDialog->readSettings();
    mShowMetersRecordsDialog->show();
}

void MainWindow::on_actionAboutSoftware_triggered()
{
    QMessageBox::about(this, "À propos du logiciel", "Logiciel développé par l'ALE 08.");
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, "À propos de Qt");
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void MainWindow::on_pushButton_AddFuelDelivery_clicked()
{
    mAddFullDeliveryDialog->resetValues();
    mAddFullDeliveryDialog->readSettings();
    mAddFullDeliveryDialog->show();
}

void MainWindow::on_pushButton_AddMetersRecord_clicked()
{
    mAddMetersRecordDialog->readSettings();
    mAddMetersRecordDialog->show();
}
