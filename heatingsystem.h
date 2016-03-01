#ifndef HEATINGSYSTEM_H
#define HEATINGSYSTEM_H

#include <QObject>
#include <QDate>
#include <QUuid>

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

    private:
        //mHash will be used to identify records with no doubt.
        QString mHash;

    public:
        Record()
        {
            mHash = QUuid::createUuid().toString();
        }

        Record(QString substation, QDate date, double value)
        {
            mSubstation = substation;
            mDate = date;
            mValue = value;

            mHash = QUuid::createUuid().toString();
        }
        bool operator==(const Record &right)
        {
            return mHash == right.mHash;
        }
        const QString& getHash() const
        {
            return mHash;
        }
    };

    struct FuelDelivery
    {
        Fuel mFuel;
        QDate mDate;
        double mValue;
        double mBill;
        int mWoodUnit;
        double mWoodMoisture;

    private:
        //mHash will be used to identify fuel deliviries with no doubt.
        QString mHash;

    public:
        FuelDelivery()
        {
            mHash = QUuid::createUuid().toString();
        }

        FuelDelivery(Fuel fuel, QDate date, double value, double bill, int woodUnit = 0, double woodMoisture = 0)
        {
            mFuel = fuel;
            mDate = date;
            mValue = value;
            mBill = bill;
            mWoodUnit = woodUnit;
            mWoodMoisture = woodMoisture;

            mHash = QUuid::createUuid().toString();
        }
        bool operator==(const FuelDelivery &right)
        {
            return mHash == right.mHash;
        }
        const QString& getHash() const
        {
            return mHash;
        }
    };

    struct FuelIndex
    {
        Fuel mFuel;
        int mIndex;
        QDate mDate;

    private:
        //mHash will be used to identify fuel indexes with no doubt.
        QString mHash;

    public:
        FuelIndex()
        {
            mHash = QUuid::createUuid().toString();
        }

        FuelIndex(Fuel fuel, int index, QDate date)
        {
            mFuel = fuel;
            mIndex = index;
            mDate = date;

            mHash = QUuid::createUuid().toString();
        }
        bool operator==(const FuelIndex &right)
        {
            return mHash == right.mHash;
        }
        const QString& getHash() const
        {
            return mHash;
        }
    };


public:
    explicit HeatingSystem(QObject *parent = 0);

public:
    void load(QString fileName);
    void save();

signals:

public slots:

public:
    QString mName;
    QString mFileName;

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
    QList<Record> mMainHeatMeterRecords;
    QList<FuelDelivery> mWoodDeliveries;
    QList<FuelDelivery> mFossilFuelDeliveries;
    QList<FuelIndex> mNaturalGasIndexes;
    QList<FuelIndex> mElectricityIndexes;
};

#endif // HEATINGSYSTEM_H
