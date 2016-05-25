#include <QSettings>

#include "qcustomplot/qcustomplot.h"
#include "filedownloader.h"
#include "tabstyle.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mConfigurationDialog = new ConfigurationDialog(this);
    mAddFullDelivery = new AddFuelDelivery(ui->dataFrame);
    mAddMetersRecordDialog = new AddMetersRecordDialog(ui->dataFrame);
    mShowMetersRecordsDialog = new ShowMetersRecordsDialog(this);
    mShowFuelDeliveriesDialog = new ShowFuelDeliveriesDialog(this);

    connect(mConfigurationDialog,     SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));
    connect(mAddMetersRecordDialog,   SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));
    connect(mShowMetersRecordsDialog, SIGNAL(settingsChanged()), this, SLOT(updateEnergyConsumptionChart()));

    mAddFullDelivery->hide();
    mAddMetersRecordDialog->hide();
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
    connect(actionNewHeatingSystem, SIGNAL(triggered(bool)), this, SLOT(actionNewHeatingSystem_triggered()));

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
            QMap<QString, HeatingSystem*>::iterator i = mHeatingSystems.insert(filename, newHeatingSystem);
            DJU* newDJU = new DJU();
            newDJU->loadFromFile(i.value()->mWeatherStation);
            mDJU.insert(i.value()->mWeatherStation, newDJU);
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

    ui->tabWidget->tabBar()->setStyle(new TabStyle);

    ui->pushButton_AddFuelDelivery->setFocus();
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
    QString name = "<p align=\"center\"><span style=\"font-weight:600;font-size:12pt;\">";
    name += mHeatingSystems[mCurrentHeatingSystem]->mName;
    name += "</span></p>";

    ui->labelBoilerRoomName->setText(name);

    ui->energyConsumptionChart->updateChart(mHeatingSystems[mCurrentHeatingSystem], mDJU[mHeatingSystems[mCurrentHeatingSystem]->mWeatherStation]);
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

    QUrl DJUFile("http://www.ale08.org/suivi-chaufferies/" + mHeatingSystems[mCurrentHeatingSystem]->mWeatherStation + ".xml");
    FileDownloader *downloader = new FileDownloader(DJUFile, this);

    QMessageBox *box = new QMessageBox(QMessageBox::Information,
                                       "Téléchargement en cours…",
                                       "Le fichier est en cours de téléchargement");
    box->show();

    connect(downloader, SIGNAL(downloaded(QByteArray*)), box, SLOT(close()));
    connect(downloader, SIGNAL(downloaded(QByteArray*)), this, SLOT(fileDownloaded(QByteArray*)));
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

void MainWindow::fileDownloaded(QByteArray* file)
{
    QString& weatherStation = mHeatingSystems[mCurrentHeatingSystem]->mWeatherStation;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");

    QFile* outFile = new QFile(dir.filePath(weatherStation + ".xml"));
    outFile->open(QIODevice::WriteOnly);
    outFile->write(*file);
    outFile->close();

    QMessageBox::information(this, "Information", "Le fichier a été téléchargé.");

    if (mDJU[weatherStation] == NULL)
    {
        DJU* newDJU = new DJU;
        mDJU.insert(weatherStation, newDJU);
    }
    mDJU[weatherStation]->load(weatherStation, *file);
}

void MainWindow::actionNewHeatingSystem_triggered()
{
    QString userText = QInputDialog::getText(this, "Nom de la nouvelle chaufferie",
                                             "Veuillez choisir un nom pour la nouvelle chaufferie (ce nom pourra être modifié par la suite)");

    //Remove non-ASCII characters from userText to be able to use it as a file name.
    QString name = userText;
    QString filename = userText.replace(QRegExp("[^A-Za-z0-9]"), "_");

    //Create a new HeatingSystem
    HeatingSystem* system = new HeatingSystem();
    mHeatingSystems.insert(filename, system);

    system->mName = name;
    system->mFileName = filename;
    system->save();

    mCurrentHeatingSystem = filename;

    //Open configuration dialog
    on_actionConfigureBoilerRoom_triggered();

    //Save the new heating system in settings file
    saveSettings();

    //Update menu
    readSettings();
}

void MainWindow::on_pushButton_AddFuelDelivery_toggled(bool checked)
{
    if (mHeatingSystems[mCurrentHeatingSystem] == NULL)
    {
        QMessageBox::critical(this, "Erreur", "Aucune chaufferie n'est définie.");
        return;
    }

    mAddFullDelivery->resetValues();

    if (checked)
    {
        mAddFullDelivery->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
        mAddFullDelivery->setMinimumWidth(ui->dataFrame->width());
        mAddFullDelivery->show();
        ui->pushButton_AddMetersRecord->setChecked(false);
        ui->pushButton_AddIntervention->setChecked(false);
    }
    else
        mAddFullDelivery->hide();
}

void MainWindow::on_pushButton_AddMetersRecord_toggled(bool checked)
{
    if (mHeatingSystems[mCurrentHeatingSystem] == NULL)
    {
        QMessageBox::critical(this, "Erreur", "Aucune chaufferie n'est définie.");
        return;
    }

    if (checked)
    {
        mAddMetersRecordDialog->setHeatingSystem(mHeatingSystems[mCurrentHeatingSystem]);
        mAddMetersRecordDialog->setMinimumWidth(ui->dataFrame->width());
        mAddMetersRecordDialog->show();
        ui->pushButton_AddFuelDelivery->setChecked(false);
        ui->pushButton_AddIntervention->setChecked(false);
    }
    else
        mAddMetersRecordDialog->hide();
}

void MainWindow::on_setScreen_FuelDeliveries_clicked()
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

void MainWindow::on_setScreen_MetersRecords_clicked()
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

void MainWindow::on_setScreen_Interventions_clicked()
{

}
