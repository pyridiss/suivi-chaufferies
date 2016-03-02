#include <QSettings>

#include "qcustomplot/qcustomplot.h"
#include "filedownloader.h"

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

    connect(mConfigurationDialog,     SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));
    connect(mAddMetersRecordDialog,   SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));
    connect(mShowMetersRecordsDialog, SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));

    mDJU.load();

    /*
     * Set Energy Consumption chart design
     */

    ui->chart_EnergyConsumption->setLocale(QLocale(QLocale::French, QLocale::France));
    ui->chart_EnergyConsumption->plotLayout()->insertRow(0);
    ui->chart_EnergyConsumption->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->chart_EnergyConsumption, "Total des consommations de chaleur des sous-stations"));

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

    //Y-Axis
    ui->chart_EnergyConsumption->yAxis->setLabel("Consommation (MWh)");
    ui->chart_EnergyConsumption->yAxis->setRangeLower(0);
    ui->chart_EnergyConsumption->yAxis->setAutoTickStep(false);

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

    /*
     * Set menu for heating systems
     */

    //Get the QToolButton
    QToolButton *toolButton = static_cast<QToolButton*>(ui->mainToolBar->widgetForAction(ui->actionHeatingSystems));
    toolButton->setPopupMode(QToolButton::InstantPopup);

    //Construct a menu for this button
    QMenu* menu = new QMenu(this);

    QAction *actionNewHeatingSystem = menu->addAction("Créer une nouvelle chaufferie");

    menu->addSection("Chaufferies existantes");

    QActionGroup *heatingSystemsGroup = new QActionGroup(this);

    QString lastHeatingSystem = settings.value("lastHeatingSystem").toString();

    int size = settings.beginReadArray("heatingSystems");
    for (int i = 0 ; i < size ; ++i)
    {
        settings.setArrayIndex(i);

        QString name = settings.value("name").toString();
        QString filename = settings.value("filename").toString();

        //Load the heating system if not done yet
        if (mHeatingSystems.find(filename) == mHeatingSystems.end())
        {
            HeatingSystem *newHeatingSystem = new HeatingSystem(this);
            newHeatingSystem->load(filename);
            mHeatingSystems.insert(filename, newHeatingSystem);
        }

        QAction *heatingSystem = heatingSystemsGroup->addAction(name);
        heatingSystem->setCheckable(true);
        heatingSystem->setProperty("filename", filename);
    }
    settings.endArray();

    menu->addActions(heatingSystemsGroup->actions());

    toolButton->setMenu(menu);

    foreach(QAction* action, heatingSystemsGroup->actions())
    {
        connect(action, SIGNAL(triggered(bool)), this, SLOT(changeCurrentHeatingSystem()));

        if (action->text() == lastHeatingSystem)
        {
            action->setChecked(true);
            action->trigger();
        }
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;

    //Write heating systems
    settings.remove("heatingSystems");
    settings.beginWriteArray("heatingSystems");

    QMap<QString, HeatingSystem*>::iterator it = mHeatingSystems.begin();
    for (int i = 0 ; i < mHeatingSystems.size() ; ++i)
    {
        settings.setArrayIndex(i);

        settings.setValue("name", it.value()->mName);
        settings.setValue("filename", it.value()->mFileName);
        ++it;
    }
    settings.endArray();

    //Write current heating system
    settings.setValue("lastHeatingSystem", mHeatingSystems[mCurrentHeatingSystem]->mName);
}

void MainWindow::changeCurrentHeatingSystem()
{
    QString newHeatingSystem = sender()->property("filename").toString();

    if (newHeatingSystem == mCurrentHeatingSystem) return;

    mCurrentHeatingSystem = newHeatingSystem;

    updateEnergyConsumptionChart();

    saveSettings();
}

void MainWindow::updateEnergyConsumptionChart()
{
    //Update name of the heating system
    QString name = "<p align=\"center\"><span style=\"font-weight:600;\">";
    name += mHeatingSystems[mCurrentHeatingSystem]->mName;
    name += "</span></p>";

    ui->labelBoilerRoomName->setText(name);

    double theoreticAnnualConsumption = 0;

    //Remove data from ui->tableWidget_chartResults
    while (ui->tableWidget_chartResults->rowCount() > 0)
        ui->tableWidget_chartResults->removeRow(0);

    for (QPair<QString, double> &substation : mHeatingSystems[mCurrentHeatingSystem]->mSubstations)
    {
        theoreticAnnualConsumption += substation.second;

        int i = ui->tableWidget_chartResults->rowCount();

        QTableWidgetItem *newItem = new QTableWidgetItem();
        newItem->setData(Qt::EditRole, substation.second);
        newItem->setFlags(Qt::NoItemFlags);
        newItem->setTextAlignment(Qt::AlignCenter);
        newItem->setTextColor(Qt::black);

        //ui->tableWidget_chartResults will be filled with consumptions. These values will be used later.
        ui->tableWidget_chartResults->insertRow(i);
        ui->tableWidget_chartResults->setItem(i, 0, newItem);
        ui->tableWidget_chartResults->setVerticalHeaderItem(i, new QTableWidgetItem(substation.first));
    }

    //1. Graph 0: 'real sum of meters in substations'.
    QMap<QDate, int> meterRecords;

    for (const HeatingSystem::Record &record : mHeatingSystems[mCurrentHeatingSystem]->mRecords)
    {
        if (meterRecords.find(record.mDate) == meterRecords.end())
            meterRecords.insert(record.mDate, record.mValue);
        else meterRecords[record.mDate] += record.mValue;
    }

    QVector<double> x0(meterRecords.size()), y0(meterRecords.size());

    QMap<QDate, int>::iterator it = meterRecords.begin();
    for (int i = 0 ; i < meterRecords.size() ; ++i)
    {
        x0[i] = QDateTime(it.key()).toTime_t();
        y0[i] = it.value() / 1000.f;
        ++it;
    }

    ui->chart_EnergyConsumption->graph(0)->setData(x0, y0);

    //2. Graph 1: 'theoretic sum based on real DJU'

    QDate heatingSeasonBegin(2015, 7, 1);
    QDate heatingSeasonEnd  (2016, 6, 30);
    QDate lastKnownTemperature = QDate::fromString(mDJU.getLastDataDate(), "yyyy-MM-dd");

    double averageDJUOfHeatingSeason = mDJU.getAverageDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

    int size1 = qMax(qint64(1), heatingSeasonBegin.daysTo(lastKnownTemperature) + 1);
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

    double step = theoreticAnnualConsumption * 1.2;
    int power = 0;
    while (step > 10)
    {
        step /= 10;
        ++power;
    }
    step = round(step) * pow(10, power);
    ui->chart_EnergyConsumption->yAxis->setTickStep(step / 10);

    ui->chart_EnergyConsumption->replot();

    //5. Results

    QLocale locale;

    //Expected consumption
    double expectedConsumptionMWh = y2[size2 - 1];
    ui->label_ExpectedConsumption->setText("<b>" + locale.toString(expectedConsumptionMWh, 'g', 3) + " MWh</b>");

    //Shift
    double theoreticConsumption = 0;
    if (!meterRecords.isEmpty() && meterRecords.lastKey() > heatingSeasonBegin && meterRecords.lastKey() < heatingSeasonEnd)
    {
        if (meterRecords.lastKey() <= lastKnownTemperature)
            theoreticConsumption = y1[heatingSeasonBegin.daysTo(meterRecords.lastKey()) - 1];
        else
            theoreticConsumption = y2[lastKnownTemperature.daysTo(meterRecords.lastKey()) - 1];
    }

    double shift = 0;
    if (!meterRecords.isEmpty())
        shift = (meterRecords.last() / 1000.f - theoreticConsumption) / theoreticConsumption * 100.f;
    ui->label_Shift->setText(((shift >= 0) ? "<b>+ " : "<b>- ") + locale.toString((shift >= 0) ? shift : -shift, 'g', 3) + " %</b>");

    //Expected corrected consumption
    double DJUcovered = 0;

    d = heatingSeasonBegin;
    while (!meterRecords.isEmpty() && d < meterRecords.lastKey())
    {
        if (d <= lastKnownTemperature)
            DJUcovered += mDJU.getDJU(d.toString("yyyy-MM-dd"));
        else
            DJUcovered += mDJU.getAverageDJU(d.toString("yyyy-MM-dd"));

        d = d.addDays(1);
    }

    double expectedCorrectedConsumptionMWh = 0;
    if (!meterRecords.isEmpty())
        expectedCorrectedConsumptionMWh = meterRecords.last() / 1000.f / DJUcovered * mDJU.getAverageDJU("2010-01-01", "2010-12-31");
    ui->label_ExpectedCorrectedConsumption->setText("<b>" + locale.toString(expectedCorrectedConsumptionMWh, 'g', 3) + " MWh</b>");
}

void MainWindow::on_actionConfigureBoilerRoom_triggered()
{
    mConfigurationDialog->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
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
    QUrl DJUFile("http://www.ale08.org/dju.xml");
    FileDownloader *downloader = new FileDownloader(DJUFile, this);

    QMessageBox *box = new QMessageBox(QMessageBox::Information,
                                       "Téléchargement en cours…",
                                       "Le fichier est en cours de téléchargement");
    box->show();

    connect(downloader, SIGNAL(downloaded(QByteArray*)), box, SLOT(close()));
    connect(downloader, SIGNAL(downloaded(QByteArray*)), this, SLOT(fileDownloaded(QByteArray*)));
}

void MainWindow::on_actionShowDeliveries_triggered()
{
    if (mHeatingSystems[mCurrentHeatingSystem] == NULL)
    {
        QMessageBox::critical(this, "Erreur", "Aucune chaufferie n'est définie.");
        return;
    }
    mShowFuelDeliveriesDialog->resetValues();
    mShowFuelDeliveriesDialog->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
    mShowFuelDeliveriesDialog->show();
}

void MainWindow::on_actionShowMetersRecord_triggered()
{
    if (mHeatingSystems[mCurrentHeatingSystem] == NULL)
    {
        QMessageBox::critical(this, "Erreur", "Aucune chaufferie n'est définie.");
        return;
    }
    mShowMetersRecordsDialog->resetValues();
    mShowMetersRecordsDialog->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
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
    if (mHeatingSystems[mCurrentHeatingSystem] == NULL)
    {
        QMessageBox::critical(this, "Erreur", "Aucune chaufferie n'est définie.");
        return;
    }
    mAddFullDeliveryDialog->resetValues();
    mAddFullDeliveryDialog->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
    mAddFullDeliveryDialog->show();
}

void MainWindow::on_pushButton_AddMetersRecord_clicked()
{
    if (mHeatingSystems[mCurrentHeatingSystem] == NULL)
    {
        QMessageBox::critical(this, "Erreur", "Aucune chaufferie n'est définie.");
        return;
    }
    mAddMetersRecordDialog->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
    mAddMetersRecordDialog->show();
}

void MainWindow::fileDownloaded(QByteArray* file)
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");

    QFile outFile(dir.filePath("dju.xml"));
    outFile.open(QIODevice::WriteOnly);
    outFile.write(*file);

    QMessageBox::information(this, "Information", "Le fichier a été téléchargé.");

    mDJU.load();
}
