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
