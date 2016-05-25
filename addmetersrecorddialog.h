#ifndef ADDMETERSRECORDDIALOG_H
#define ADDMETERSRECORDDIALOG_H

#include <QFrame>
#include <QSpinBox>
#include <QAbstractButton>
#include <heatingsystem.h>

namespace Ui {
class AddMetersRecordDialog;
}

class AddMetersRecordDialog : public QFrame
{
    Q_OBJECT

public:
    explicit AddMetersRecordDialog(QWidget *parent = 0);
    ~AddMetersRecordDialog();

public:
    void resetValues();

private:
    Ui::AddMetersRecordDialog *ui;
    QList<QSpinBox*> spinBoxes;
    HeatingSystem *mHeatingSystem;

public slots:
    void setHeatingSystem(HeatingSystem *system);

private slots:
    void on_buttonBox_accepted();
    void buttonBoxClicked(QAbstractButton* button);

signals:
    void settingsChanged();
};

#endif // ADDMETERSRECORDDIALOG_H
