#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>

#include "delegates/doublespinboxdelegate.h"

#include "configurationdialog.h"
#include "ui_configurationdialog.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    //setup fuels selections
    ui->editMainHeatSource->addItem("Bois déchiqueté", HeatingSystem::WoodChips);
    ui->editMainHeatSource->addItem("Granulés",        HeatingSystem::Pellets);
    ui->editMainHeatSource->addItem("Géothermie",      HeatingSystem::GeothermalPower);
    ui->editSecondHeatSource->addItem("Aucune",                           HeatingSystem::NoFuel);
    ui->editSecondHeatSource->addItem("Appoint / Secours au gaz naturel", HeatingSystem::NaturalGas);
    ui->editSecondHeatSource->addItem("Appoint / Secours au fioul",       HeatingSystem::FuelOil);
    ui->editSecondHeatSource->addItem("Appoint / Secours au propane",     HeatingSystem::Propane);
    ui->editSecondHeatSource->addItem("Appoint / Secours électrique",     HeatingSystem::Electricity);
    ui->editSecondHeatSource->addItem("Autre",                            HeatingSystem::Other);

    //Substations consumptions will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateSubstationsConsumptions = new DoubleSpinBoxDelegate(this);
    delegateSubstationsConsumptions->setSuffix(" MWh");
    delegateSubstationsConsumptions->setMaximum(100000);
    ui->editSubstations->setItemDelegateForColumn(1, delegateSubstationsConsumptions);

    //Setup weather station selection
    QFile* file = new QFile(":/resources/cities.xml");
    bool fileOpened = file->open(QIODevice::ReadOnly);
    if (!fileOpened)
    {
        QMessageBox::warning(0, "Information", "Aucun fichier de données des stations météo n'a été trouvé.");
        return;
    }

    QXmlStreamReader xml(file);
    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }

        if(token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "city")
            {
                QString code = xml.attributes().first().value().toString();
                QString name = xml.readElementText();
                ui->editWeatherStation->addItem(name, code);
            }
        }
    }

    if(xml.hasError())
    {
        QMessageBox::critical(0, "xml Parse Error", xml.errorString(), QMessageBox::Ok);
        return;
    }

    file->close();
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    //Set the 'general' tab
    ui->editHeatingSystemName->setText(system->mName);
    for (int i = 0 ; i < ui->editMainHeatSource->count() ; ++i)
    {
        if (ui->editMainHeatSource->itemData(i) == system->mMainHeatSource)
            ui->editMainHeatSource->setCurrentIndex(i);
    }
    for (int i = 0 ; i < ui->editSecondHeatSource->count() ; ++i)
    {
        if (ui->editSecondHeatSource->itemData(i) == system->mSecondHeatSource)
            ui->editSecondHeatSource->setCurrentIndex(i);
    }
    for (int i = 0 ; i < ui->editWeatherStation->count() ; ++i)
    {
        if (ui->editWeatherStation->itemData(i).toString() == system->mWeatherStation)
            ui->editWeatherStation->setCurrentIndex(i);
    }

    //Clear substations table
    while (ui->editSubstations->rowCount() > 0)
    {
        ui->editSubstations->removeRow(0);
    }

    //Set the 'substations' tab
    for(int i = 0 ; i < system->mSubstations.size() ; ++i)
    {
        const QPair<QString, double>& substation = system->mSubstations.at(i);

        ui->editSubstations->insertRow(i);

        ui->editSubstations->setItem(i, 0, new QTableWidgetItem(substation.first));

        QTableWidgetItem *newTableItem = new QTableWidgetItem();
        newTableItem->setData(Qt::EditRole, substation.second);
        ui->editSubstations->setItem(i, 1, newTableItem);
    }

    //Set the 'heat sell' tab
    ui->groupBox_HeatSell              ->setChecked(system->mHeatSellActivated);
    ui->editHeatSell_Electricity       ->setChecked(system->mHeatSellElectricity);
    ui->editHeatSell_RoutineMaintenance->setChecked(system->mHeatSellRoutineMaintenance);
    ui->editHeatSell_MajorMaintenance  ->setChecked(system->mHeatSellMajorMaintenance);
    ui->editHeatSell_LoanAmortization  ->setChecked(system->mHeatSellLoanAmortization);
    ui->editHeatSell_LoanInterest      ->setChecked(system->mHeatSellLoanInterest);

    //Set the 'economic data' tab
    ui->doubleSpinBox_Investment->setValue(system->mInvestment);
    ui->doubleSpinBox_Subsidies ->setValue(system->mSubsidies);
    ui->doubleSpinBox_Loan      ->setValue(system->mLoan);
    ui->spinBox_LoanPeriod      ->setValue(system->mLoanPeriod);
    ui->doubleSpinBox_LoanRate  ->setValue(system->mLoanRate);

    //Set the 'technic data' tab
    ui->editBoilerEfficiency     ->setValue(system->mBoilerEfficiency);
    ui->editNetworkEfficiency    ->setValue(system->mNetworkEfficiency);
    ui->editAnnualWoodConsumption->setValue(system->mAnnualWoodConsumption);
    ui->editElectricMeter        ->setChecked(system->mElectricMeter);
    ui->editMainHeatMeter        ->setChecked(system->mMainHeatMeter);
}

void ConfigurationDialog::on_pushButton_AddSubstation_clicked()
{
    int newRow = ui->editSubstations->rowCount();
    ui->editSubstations->insertRow(newRow);

    ui->editSubstations->setItem(newRow, 0, new QTableWidgetItem("Nouvelle sous-station"));
    ui->editSubstations->setItem(newRow, 1, new QTableWidgetItem());
}

void ConfigurationDialog::on_pushButton_RemoveSubstation_clicked()
{
    //TODO: Records should be removed from settings file.

    if (ui->editSubstations->currentItem() != 0)
    {
        ui->editSubstations->removeRow(ui->editSubstations->currentRow());
    }
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    /*
     * Values from the ui will be saved in the heating system
     */

    mHeatingSystem->mName             = ui->editHeatingSystemName->text();
    mHeatingSystem->mMainHeatSource   = (HeatingSystem::Fuel)ui->editMainHeatSource->currentData().toInt();
    mHeatingSystem->mSecondHeatSource = (HeatingSystem::Fuel)ui->editSecondHeatSource->currentData().toInt();
    mHeatingSystem->mWeatherStation   = ui->editWeatherStation->currentData().toString();

    mHeatingSystem->mSubstations.clear();

    for (int i = 0 ; i < ui->editSubstations->rowCount() ; ++i)
    {
        QString name       = ui->editSubstations->item(i, 0)->text();
        double consumption = ui->editSubstations->item(i, 1)->data(Qt::EditRole).toDouble();

        mHeatingSystem->mSubstations.push_back(QPair<QString, double>(name, consumption));
    }

    mHeatingSystem->mHeatSellActivated          = ui->groupBox_HeatSell->isChecked();
    mHeatingSystem->mHeatSellElectricity        = ui->editHeatSell_Electricity->isChecked();
    mHeatingSystem->mHeatSellRoutineMaintenance = ui->editHeatSell_RoutineMaintenance->isChecked();
    mHeatingSystem->mHeatSellMajorMaintenance   = ui->editHeatSell_MajorMaintenance->isChecked();
    mHeatingSystem->mHeatSellLoanAmortization   = ui->editHeatSell_LoanAmortization->isChecked();
    mHeatingSystem->mHeatSellLoanInterest       = ui->editHeatSell_LoanInterest->isChecked();

    mHeatingSystem->mInvestment = ui->doubleSpinBox_Investment->value();
    mHeatingSystem->mSubsidies  = ui->doubleSpinBox_Subsidies->value();
    mHeatingSystem->mLoan       = ui->doubleSpinBox_Loan->value();
    mHeatingSystem->mLoanPeriod = ui->spinBox_LoanPeriod->value();
    mHeatingSystem->mLoanRate   = ui->doubleSpinBox_LoanRate->value();

    mHeatingSystem->mBoilerEfficiency      = ui->editBoilerEfficiency->value();
    mHeatingSystem->mNetworkEfficiency     = ui->editNetworkEfficiency->value();
    mHeatingSystem->mAnnualWoodConsumption = ui->editAnnualWoodConsumption->value();
    mHeatingSystem->mElectricMeter         = ui->editElectricMeter->isChecked();
    mHeatingSystem->mMainHeatMeter         = ui->editMainHeatMeter->isChecked();

    emit settingsChanged();
    mHeatingSystem->save();
}
