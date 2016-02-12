#include <QGridLayout>
#include <QLabel>
#include <QSettings>
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

void AddMetersRecordDialog::readSettings()
{
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

    QSettings settings;

    ui->spinBox_MainHeatMeter->setEnabled(settings.value("boilerRoom/mainHeatMeter").toBool());

    int size = settings.beginReadArray("substations");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        layout->addWidget(new QLabel(settings.value("name").toString()), i, 0);

        QSpinBox* spinBox = new QSpinBox(ui->groupBox_Records);
        spinBox->setSuffix(" kWh");
        spinBox->setMaximum(1000000);
        layout->addWidget(spinBox, i, 1);

        spinBoxes.push_back(spinBox);
    }
    settings.endArray();

}

void AddMetersRecordDialog::on_buttonBox_accepted()
{
    QSettings settings;

    //1. Main heat meter
    if (ui->spinBox_MainHeatMeter->value() > 0)
    {
        int currentSize = settings.value("mainHeatMeterRecords/size", 0).toInt();

        settings.beginWriteArray("mainHeatMeterRecords");
        settings.setArrayIndex(currentSize);

        settings.setValue("date", ui->dateEdit_RecordDate->date());
        settings.setValue("index", ui->spinBox_MainHeatMeter->value());

        settings.endArray();
    }

    //2. Substation meters
    for (int i = 0 ; i < spinBoxes.length() ; ++i)
    {
        QSpinBox* spinBox = spinBoxes[i];
        if (spinBox->value() > 0)
        {
            //Not to be confused, substation number is i+1 to follow the numbers used in the [substations] section of the settings file.
            QString category = "substation" + QVariant(i+1).toString();
            int currentSize = settings.value(category + "/size", 0).toInt();

            settings.beginWriteArray(category);
            settings.setArrayIndex(currentSize);

            settings.setValue("date", ui->dateEdit_RecordDate->date());
            settings.setValue("index", spinBox->value());

            settings.endArray();
        }
    }

    emit settingsChanged();
}
