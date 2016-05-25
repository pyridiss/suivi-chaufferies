#ifndef ADDMETERSRECORD_H
#define ADDMETERSRECORD_H

#include <QFrame>
#include <QSpinBox>
#include <QAbstractButton>
#include <heatingsystem.h>

namespace Ui {
class AddMetersRecord;
}

class AddMetersRecord : public QFrame
{
    Q_OBJECT

public:
    explicit AddMetersRecord(QWidget *parent = 0);
    ~AddMetersRecord();

public:
    void resetValues();

private:
    Ui::AddMetersRecord *ui;
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

#endif // ADDMETERSRECORD_H
