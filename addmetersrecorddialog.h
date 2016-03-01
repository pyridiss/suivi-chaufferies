#ifndef ADDMETERSRECORDDIALOG_H
#define ADDMETERSRECORDDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <heatingsystem.h>

namespace Ui {
class AddMetersRecordDialog;
}

class AddMetersRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMetersRecordDialog(QWidget *parent = 0);
    ~AddMetersRecordDialog();

private:
    Ui::AddMetersRecordDialog *ui;
    QList<QSpinBox*> spinBoxes;
    HeatingSystem *mHeatingSystem;

public slots:
    void setHeatingSystem(HeatingSystem *system);

private slots:
    void on_buttonBox_accepted();

signals:
    void settingsChanged();
};

#endif // ADDMETERSRECORDDIALOG_H
