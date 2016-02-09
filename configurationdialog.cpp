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
