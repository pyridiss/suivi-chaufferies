#ifndef ADDMETERSRECORDDIALOG_H
#define ADDMETERSRECORDDIALOG_H

#include <QDialog>
#include <QSpinBox>

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

public slots:
    void readSettings();

private slots:
    void on_buttonBox_accepted();

signals:
    void settingsChanged();
};

#endif // ADDMETERSRECORDDIALOG_H
