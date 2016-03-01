#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

#include "addmetersrecorddialog.h"
#include "ui_addmetersrecorddialog.h"

AddMetersRecordDialog::AddMetersRecordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMetersRecordDialog)
{
    ui->setupUi(this);
    ui->dateEdit_RecordDate->setDate(QDate::currentDate());
}

AddMetersRecordDialog::~AddMetersRecordDialog()
{
    delete ui;
}

void AddMetersRecordDialog::setHeatingSystem(HeatingSystem *system)
{
    mHeatingSystem = system;

    if (ui->groupBox_Records->layout() != NULL)
    {
        QLayoutItem* item;
        while((item = ui->groupBox_Records->layout()->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
        delete ui->groupBox_Records->layout();
    }
    spinBoxes.clear();

    QGridLayout* layout = new QGridLayout(ui->groupBox_Records);

    ui->spinBox_MainHeatMeter->setEnabled(system->mMainHeatMeter);

    for (int i = 0 ; i < system->mSubstations.size() ; ++i)
    {
        layout->addWidget(new QLabel(system->mSubstations[i].first), i, 0);

        QSpinBox* spinBox = new QSpinBox(ui->groupBox_Records);
        spinBox->setSuffix(" kWh");
        spinBox->setMaximum(1000000);
        spinBox->setProperty("substation", system->mSubstations[i].first);
        layout->addWidget(spinBox, i, 1);

        spinBoxes.push_back(spinBox);
    }
}

void AddMetersRecordDialog::on_buttonBox_accepted()
{
    //1. Main heat meter
    if (ui->spinBox_MainHeatMeter->value() > 0)
    {
        HeatingSystem::Record record("MainHeatMeter",
                                     ui->dateEdit_RecordDate->date(),
                                     ui->spinBox_MainHeatMeter->value());
        mHeatingSystem->mMainHeatMeterRecords.push_back(record);
    }

    //2. Substation meters
    foreach (QSpinBox* spinBox, spinBoxes)
    {
        if (spinBox->value() > 0)
        {
            HeatingSystem::Record record(spinBox->property("substation").toString(),
                                         ui->dateEdit_RecordDate->date(),
                                         spinBox->value());
            mHeatingSystem->mRecords.push_back(record);
        }
    }

    emit settingsChanged();
}
