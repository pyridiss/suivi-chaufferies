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
    mShowFuelDeliveriesDialog = new ShowFuelDeliveriesDialog(this);

    connect(mConfigurationDialog, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
    connect(mAddMetersRecordDialog, SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));
    connect(mShowMetersRecordsDialog, SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));

    mDJU.load();

    /*
     * Set Energy Consumption chart design
     */

    ui->chart_EnergyConsumption->setLocale(QLocale(QLocale::French, QLocale::France));

    //Legend
    ui->chart_EnergyConsumption->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(7);
    ui->chart_EnergyConsumption->legend->setFont(legendFont);
    ui->chart_EnergyConsumption->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);

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

    //Add graphs. 0 will be 'real sum of meters in substations', 1 will be 'theoretic sum based on real DJU', 2 will be 'theoretic sum based on theoric DJU'
    ui->chart_EnergyConsumption->addGraph();
    ui->chart_EnergyConsumption->graph(0)->setName("Relevés des compteurs de chaleur");

    ui->chart_EnergyConsumption->addGraph();
    ui->chart_EnergyConsumption->graph(1)->setPen(QPen(Qt::red));
    ui->chart_EnergyConsumption->graph(1)->setName("Estimation à partir des données réelles de température");

    ui->chart_EnergyConsumption->addGraph();
    QPen redDotPen;
    redDotPen.setColor(Qt::red);
    redDotPen.setStyle(Qt::DotLine);
    ui->chart_EnergyConsumption->graph(2)->setPen(redDotPen);
    ui->chart_EnergyConsumption->graph(2)->setName("Estimation à partir de données théoriques de température");
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

    double theoreticAnnualConsumption = 2000;

    //1. Graph 0: 'real sum of meters in substations'.
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

    QVector<double> x0(meterRecords.size()), y0(meterRecords.size());

    QMap<QDate, int>::iterator it = meterRecords.begin();
    for (int i = 0 ; i < meterRecords.size() ; ++i)
    {
        x0[i] = QDateTime(it.key()).toTime_t();
        y0[i] = it.value();
        ++it;
    }

    ui->chart_EnergyConsumption->graph(0)->setData(x0, y0);

    //2. Graph 1: 'theoretic sum based on real DJU'

    QDate heatingSeasonBegin(2015, 7, 1);
    QDate heatingSeasonEnd  (2016, 6, 30);
    QDate lastKnownTemperature = QDate::fromString(mDJU.getLastDataDate(), "yyyy-MM-dd");

    double averageDJUOfHeatingSeason = mDJU.getAverageDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

    int size1 = heatingSeasonBegin.daysTo(lastKnownTemperature) + 1;
    QVector <double> x1(size1), y1(size1);

    QDate d = heatingSeasonBegin;

    x1[0] = QDateTime(d).toTime_t();
    y1[0] = mDJU.getDJU(d.toString("yyyy-MM-dd")) / averageDJUOfHeatingSeason * theoreticAnnualConsumption;

    for (int i = 1 ; i < size1 ; ++i)
    {
        d = d.addDays(1);

        x1[i] = QDateTime(d).toTime_t();
        y1[i] = y1[i-1] + mDJU.getDJU(d.toString("yyyy-MM-dd")) / averageDJUOfHeatingSeason * theoreticAnnualConsumption;
    }

    ui->chart_EnergyConsumption->graph(1)->setData(x1, y1);

    //3. Graph 2: 'theoretic sum based on theoric DJU'

    double DJUSinceBeginningOfHeatingSeason = mDJU.getDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), lastKnownTemperature.toString("yyyy-MM-dd"));
    double DJURestOfHeatingSeason           = mDJU.getAverageDJU(lastKnownTemperature.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

    int size2 = lastKnownTemperature.daysTo(heatingSeasonEnd) + 1;
    QVector <double> x2(size2), y2(size2);

    d = lastKnownTemperature;

    x2[0] = QDateTime(d).toTime_t();
    y2[0] = y1[size1 - 1];

    for (int i = 1 ; i < size2 ; ++i)
    {
        d = d.addDays(1);

        x2[i] = QDateTime(d).toTime_t();
        y2[i] = y2[i-1] + mDJU.getAverageDJU(d.toString("yyyy-MM-dd")) / (DJUSinceBeginningOfHeatingSeason + DJURestOfHeatingSeason) * theoreticAnnualConsumption;
    }

    ui->chart_EnergyConsumption->graph(2)->setData(x2, y2);

    //4. Replot
    ui->chart_EnergyConsumption->yAxis->setRangeUpper(theoreticAnnualConsumption * 1.2);
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
    mShowFuelDeliveriesDialog->resetValues();
    mShowFuelDeliveriesDialog->readSettings();
    mShowFuelDeliveriesDialog->show();
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
