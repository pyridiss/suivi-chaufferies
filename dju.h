#ifndef DJU_H
#define DJU_H

#include <QMap>
#include <QList>

class DJU
{
public:
    DJU();

public:
    void loadFromFile(QString weatherStation);
    void load(QString weatherStation, const QByteArray& data);
    double getDJU(QString date);
    double getDJU(QString date1, QString date2);
    QString getLastDataDate();
    double getAverageDJU(QString date);
    double getAverageDJU(QString date1, QString date2);

private:
    QString mWeatherStation;
    QMap<QString, double> mDJU;
    QMap< std::pair<QString, QString>, double> mPeriodicDJU;
    QList<QString> mCompleteMonthes;
};

#endif // DJU_H
