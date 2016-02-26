#ifndef HEATINGSYSTEM_H
#define HEATINGSYSTEM_H

#include <QObject>
#include <QDate>

typedef QList< QPair<QString, double> > SubstationsList;

class HeatingSystem : public QObject
{
    Q_OBJECT

public:
    enum Fuel
    {
        NoFuel,
        WoodChips,
        Pellets,
        GeothermalPower,
        NaturalGas,
        FuelOil,
        Propane,
        Electricity,
        Other
    };
    Q_ENUM(Fuel)

    struct Record
    {
        QString mSubstation;
        QDate mDate;
        double mValue;

        Record()
        {
        }

        Record(QString substation, QDate date, double value)
        {
            mSubstation = substation;
            mDate = date;
            mValue = value;
        }
    };

    struct FuelDelivery
    {
        Fuel mFuel;
        QDate mDate;
        double mValue;

        FuelDelivery()
        {
        }

        FuelDelivery(Fuel fuel, QDate date, double value)
        {
            mFuel = fuel;
            mDate = date;
            mValue = value;
        }
    };

public:
    explicit HeatingSystem(QObject *parent = 0);

public:
    void load(QString fileName);

signals:

public slots:

public:
    QString mName;

    Fuel mMainHeatSource;
    Fuel mSecondHeatSource;

    SubstationsList mSubstations;

    double mInvestment;
    double mSubsidies;
    double mLoan;
    int mLoanPeriod;
    double mLoanRate;

    double mAnnualWoodConsumption;
    double mBoilerEfficiency;
    double mNetworkEfficiency;
    bool mMainHeatMeter;

    bool mHeatSellActivated;
    bool mHeatSellElectricity;
    bool mHeatSellRoutineMaintenance;
    bool mHeatSellMajorMaintenance;
    bool mHeatSellLoanAmortization;
    bool mHeatSellLoanInterest;

    QList<Record> mRecords;
    QList<FuelDelivery> mFuelDeliveries;
};

#endif // HEATINGSYSTEM_H
