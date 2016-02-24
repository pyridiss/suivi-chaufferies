#include "delegates/doublespinboxdelegate.h"

#include "configurationdialog.h"
#include "ui_configurationdialog.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    //Substations consumptions will be edited with a DoubleSpinBox with extra parameters
    DoubleSpinBoxDelegate *delegateSubstationsConsumptions = new DoubleSpinBoxDelegate(this);
    delegateSubstationsConsumptions->setSuffix(" MWh");
    delegateSubstationsConsumptions->setMaximum(100000);
    ui->editSubstations->setItemDelegateForColumn(1, delegateSubstationsConsumptions);

    connect(ui->editHeatSell_Activated, SIGNAL(stateChanged(int)), this, SLOT(setHeatSell(int)));
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
    ui->editMainHeatSource   ->setCurrentIndex(system->mMainHeatSource);
    ui->editSecondHeatSource ->setCurrentIndex(system->mSecondHeatSource);

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
    ui->editHeatSell_Activated         ->setChecked(system->mHeatSellActivated);
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
    ui->editMainHeatMeter        ->setChecked(system->mMainHeatMeter);
}

void ConfigurationDialog::setHeatSell(int state)
{
    ui->groupBox_HeatSell->setEnabled(state);
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
    mHeatingSystem->mMainHeatSource   = (HeatingSystem::MainHeatSource)ui->editMainHeatSource->currentIndex();
    mHeatingSystem->mSecondHeatSource = (HeatingSystem::SecondHeatSource)ui->editSecondHeatSource->currentIndex();

    mHeatingSystem->mSubstations.clear();

    for (int i = 0 ; i < ui->editSubstations->rowCount() ; ++i)
    {
        QString name       = ui->editSubstations->item(i, 0)->text();
        double consumption = ui->editSubstations->item(i, 1)->data(Qt::EditRole).toDouble();

        mHeatingSystem->mSubstations.push_back(QPair<QString, double>(name, consumption));
    }

    mHeatingSystem->mHeatSellActivated          = ui->editHeatSell_Activated->isChecked();
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
    mHeatingSystem->mMainHeatMeter         = ui->editMainHeatMeter->isChecked();

    emit settingsChanged();
}
