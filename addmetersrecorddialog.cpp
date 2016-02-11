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
