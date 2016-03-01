#ifndef SHOWMETERSRECORDSDIALOG_H
#define SHOWMETERSRECORDSDIALOG_H

#include <QDialog>
#include <heatingsystem.h>

namespace Ui {
class ShowMetersRecordsDialog;
}

class ShowMetersRecordsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowMetersRecordsDialog(QWidget *parent = 0);
    ~ShowMetersRecordsDialog();

public:
    void resetValues();

private:
    Ui::ShowMetersRecordsDialog *ui;
    HeatingSystem* mHeatingSystem;

public slots:
    void setHeatingSystem(HeatingSystem *system);
    void recordChanged(int, int);

signals:
    void settingsChanged();
};

#endif // SHOWMETERSRECORDSDIALOG_H
