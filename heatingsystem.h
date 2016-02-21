#ifndef HEATINGSYSTEM_H
#define HEATINGSYSTEM_H

#include <QObject>
#include <QDate>

class HeatingSystem : public QObject
{
    Q_OBJECT

public:
    enum MainHeatSource
    {
        WoodChips,
        Pellets,
        GeothermalPower
    };
    enum SecondHeatSource
    {
        NoSecondHeatSource,
        NaturalGas,
        FuelOil,
        Propane,
        Electricity,
        Other
    };
    struct Record
    {
        QString mSubstation;
        QDate mDate;
        double value;
    };

public:
    explicit HeatingSystem(QObject *parent = 0);

public:
    void load(QString fileName);

signals:

public slots:

private:
    QString mName;

    MainHeatSource mMainHeatSource;
    SecondHeatSource mSecondHeatSource;

    QList<QString> mSubstations;

    double mInvestment;
    double mSubsidies;
    double mLoan;
    int mLoanPeriod;
    double mLoanRate;

    double mAnnualWoodConsumption;
    double mBoilerEfficiency;
    double mNetworkEfficiency;

    bool mHeatSellActivated;
    bool mHeatSellElectricity;
    bool mHeatSellRoutineMaintenance;
    bool mHeatSellMajorMaintenance;
    bool mHeatSellLoanAmortization;
    bool mHeatSellLoanInterest;

    QList<Record> mRecords;
};

#endif // HEATINGSYSTEM_H
