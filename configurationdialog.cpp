#include <QSettings>

#include "configurationdialog.h"
#include "ui_configurationdialog.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    connect(ui->checkBox_HeatSell, SIGNAL(stateChanged(int)), this, SLOT(setHeatSell(int)));
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::readSettings()
{
    QSettings settings;

    ui->lineEdit_BoilerRoomName->setText(settings.value("boilerRoom/boilerRoomName", "").toString());
    ui->comboBox_MainHeatSource->setCurrentIndex(settings.value("boilerRoom/mainHeatSource", 0).toInt());
    ui->comboBox_SecondaryHeatSource->setCurrentIndex(settings.value("boilerRoom/secondaryHeatSource", 0).toInt());
    ui->comboBox_MainHeatMeter->setCurrentIndex(settings.value("boilerRoom/mainHeatMeter", 0).toInt());

    ui->listWidget_Substations->clear();
    int size = settings.beginReadArray("substations");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget_Substations);
        newItem->setText(settings.value("name").toString());
        newItem->setFlags(newItem->flags () | Qt::ItemIsEditable);
    }
    settings.endArray();
}

void ConfigurationDialog::setHeatSell(int state)
{
    ui->groupBox_HeatSell->setEnabled(state);
}

void ConfigurationDialog::on_pushButton_AddSubstation_clicked()
{
   QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget_Substations);
   newItem->setText("Nouvelle sous-station");
   newItem->setFlags(newItem->flags () | Qt::ItemIsEditable);
}

void ConfigurationDialog::on_pushButton_RemoveSubstation_clicked()
{
    if (ui->listWidget_Substations->currentItem() != 0)
    {
        delete ui->listWidget_Substations->currentItem();
    }
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    QSettings settings;

    settings.setValue("boilerRoom/boilerRoomName",      ui->lineEdit_BoilerRoomName->text());
    settings.setValue("boilerRoom/mainHeatSource",      ui->comboBox_MainHeatSource->currentIndex());
    settings.setValue("boilerRoom/secondaryHeatSource", ui->comboBox_SecondaryHeatSource->currentIndex());
    settings.setValue("boilerRoom/mainHeatMeter",       ui->comboBox_MainHeatMeter->currentIndex());

    settings.beginWriteArray("substations");
    for (int i = 0; i < ui->listWidget_Substations->count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", ui->listWidget_Substations->item(i)->text());
    }
    settings.endArray();

    emit settingsChanged();
}
