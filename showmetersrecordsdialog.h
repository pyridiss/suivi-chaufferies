#ifndef SHOWMETERSRECORDSDIALOG_H
#define SHOWMETERSRECORDSDIALOG_H

#include <QDialog>

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

public slots:
    void readSettings();
    void recordChanged(int, int);

signals:
    void settingsChanged();
};

#endif // SHOWMETERSRECORDSDIALOG_H
