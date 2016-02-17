#ifndef DJU_H
#define DJU_H

#include <QMap>
#include <QList>

class DJU
{
public:
    DJU();

public:
    void load();
    double getDJU(QString date);
    double getDJU(QString date1, QString date2);
    QString getLastDataDate();
    double getAverageDJU(QString date);
    double getAverageDJU(QString date1, QString date2);

private:
    QMap<QString, double> mDJU;
    QMap< std::pair<QString, QString>, double> mPeriodicDJU;
    QList<int> mCompleteYears;
};

#endif // DJU_H
