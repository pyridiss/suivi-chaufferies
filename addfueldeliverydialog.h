#ifndef ADDFUELDELIVERYDIALOG_H
#define ADDFUELDELIVERYDIALOG_H

#include <QDialog>

namespace Ui {
class AddFuelDeliveryDialog;
}

class AddFuelDeliveryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFuelDeliveryDialog(QWidget *parent = 0);
    ~AddFuelDeliveryDialog();

private:
    Ui::AddFuelDeliveryDialog *ui;

public slots:
    void readSettings();
};

#endif // ADDFUELDELIVERYDIALOG_H
