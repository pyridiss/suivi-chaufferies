#include <QDir>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QDate>

#include "heatingsystem.h"

HeatingSystem::HeatingSystem(QObject *parent) : QObject(parent)
{

}

void HeatingSystem::load(QString fileName)
{
    mFileName = fileName;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");

    QFile* file = new QFile(dir.filePath(fileName + ".xml"));

    bool fileOpened = file->open(QIODevice::ReadOnly);
    if (!fileOpened)
    {
        QMessageBox::warning(0, "Information", "La chaufferie demandée n'existe pas.\n(fichier : " + fileName + ")");
        return;
    }

    QXmlStreamReader xml(file);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }

        if(token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "heating_system")
            {
                continue;
            }
            if (xml.name() == "properties")
            {
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "name")
                        mName = a.value().toString();
                    if (a.name() == "weatherStation")
                        mWeatherStation = a.value().toString();
                }
            }
            if (xml.name() == "heat_sources")
            {
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "first")
                        mMainHeatSource = (HeatingSystem::Fuel) a.value().toInt();
                    if (a.name() == "second")
                        mSecondHeatSource = (HeatingSystem::Fuel) a.value().toInt(); //TODO: Error here!
                }
            }
            if (xml.name() == "add_substation")
            {
                double consumption = 0;
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "consumption")
                        consumption = a.value().toDouble();
                }
                QString substation = xml.readElementText();
                mSubstations.push_back(QPair<QString, double>(substation, consumption));
            }
            if (xml.name() == "economy")
            {
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "investment")
                        mInvestment = a.value().toDouble();
                    if (a.name() == "subsidies")
                        mSubsidies = a.value().toDouble();
                    if (a.name() == "loan")
                        mLoan = a.value().toDouble();
                    if (a.name() == "loanPeriod")
                        mLoanPeriod = a.value().toInt();
                    if (a.name() == "loanRate")
                        mLoanRate = a.value().toDouble();
                }
            }
            if (xml.name() == "technic")
            {
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "annualWoodConsumption")
                        mAnnualWoodConsumption = a.value().toDouble();
                    if (a.name() == "boilerEfficiency")
                        mBoilerEfficiency = a.value().toDouble();
                    if (a.name() == "networkEfficiency")
                        mNetworkEfficiency = a.value().toDouble();
                    if (a.name() == "electricMeter")
                        mElectricMeter = a.value().toInt();
                    if (a.name() == "mainHeatMeter")
                        mMainHeatMeter = a.value().toInt();
                }
            }
            if (xml.name() == "heat_sell")
            {
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "activated")
                        mHeatSellActivated = a.value().toInt();
                    if (a.name() == "electricity")
                        mHeatSellElectricity = a.value().toInt();
                    if (a.name() == "routineMaintenance")
                        mHeatSellRoutineMaintenance = a.value().toInt();
                    if (a.name() == "majorMaintenance")
                        mHeatSellMajorMaintenance = a.value().toInt();
                    if (a.name() == "loanAmortization")
                        mHeatSellLoanAmortization = a.value().toInt();
                    if (a.name() == "loanInterest")
                        mHeatSellLoanInterest = a.value().toInt();
                }
            }
            if (xml.name() == "add_record")
            {
                Record newRecord;

                QXmlStreamAttributes xmlAttributes = xml.attributes();
                newRecord.mSubstation = xmlAttributes.value("", "substation").toString();
                newRecord.mDate = QDate::fromString(xmlAttributes.value("", "date").toString(), "yyyy-MM-dd");
                newRecord.mValue = xml.readElementText().toDouble();

                mRecords.push_back(newRecord);
            }
            if (xml.name() == "add_mainheatmeter_record")
            {
                Record newRecord;

                QXmlStreamAttributes xmlAttributes = xml.attributes();
                newRecord.mSubstation = "MainHeatMeter";
                newRecord.mDate = QDate::fromString(xmlAttributes.value("", "date").toString(), "yyyy-MM-dd");
                newRecord.mValue = xml.readElementText().toDouble();

                mMainHeatMeterRecords.push_back(newRecord);
            }
        }
    }

    if(xml.hasError())
    {
        QMessageBox::critical(0, "xml Parse Error", xml.errorString(), QMessageBox::Ok);
        return;
    }

    file->close();
}

void HeatingSystem::save()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");

    QFile* file = new QFile(dir.filePath(mFileName + ".xml"));

    bool fileOpened = file->open(QIODevice::WriteOnly);
    if (!fileOpened)
    {
        QMessageBox::critical(0, "Erreur", "Le fichier de données de la chaufferie n'a pas pu être enregistré.");
        return;
    }

    QXmlStreamWriter xml(file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();
    xml.writeStartElement("heating_system");

    xml.writeStartElement("properties");
        xml.writeAttribute("name", mName);
        xml.writeAttribute("weatherStation", mWeatherStation);
    xml.writeEndElement();

    xml.writeStartElement("heat_sources");
        xml.writeAttribute("first",  QString::number(mMainHeatSource));
        xml.writeAttribute("second", QString::number(mSecondHeatSource));
    xml.writeEndElement();

    xml.writeStartElement("economy");
        xml.writeAttribute("investment", QString::number(mInvestment));
        xml.writeAttribute("subsidies",  QString::number(mSubsidies));
        xml.writeAttribute("loan",       QString::number(mLoan));
        xml.writeAttribute("loanPeriod", QString::number(mLoanPeriod));
        xml.writeAttribute("loanRate",   QString::number(mLoanRate));
    xml.writeEndElement();

    xml.writeStartElement("technic");
        xml.writeAttribute("annualWoodConsumption", QString::number(mAnnualWoodConsumption));
        xml.writeAttribute("boilerEfficiency",      QString::number(mBoilerEfficiency));
        xml.writeAttribute("networkEfficiency",     QString::number(mNetworkEfficiency));
        xml.writeAttribute("electricMeter",         QString::number(mElectricMeter));
        xml.writeAttribute("mainHeatMeter",         QString::number(mMainHeatMeter));
    xml.writeEndElement();

    xml.writeStartElement("heat_sell");
        xml.writeAttribute("activated",          QString::number(mHeatSellActivated));
        xml.writeAttribute("electricity",        QString::number(mHeatSellElectricity));
        xml.writeAttribute("routineMaintenance", QString::number(mHeatSellRoutineMaintenance));
        xml.writeAttribute("majorMaintenance",   QString::number(mHeatSellMajorMaintenance));
        xml.writeAttribute("loanAmortization",   QString::number(mHeatSellLoanAmortization));
        xml.writeAttribute("loanInterest",       QString::number(mHeatSellLoanInterest));
    xml.writeEndElement();

    QPair<QString, double> substation;
    foreach (substation, mSubstations)
    {
        xml.writeStartElement("add_substation");
            xml.writeAttribute("consumption", QString::number(substation.second));
            xml.writeCharacters(substation.first);
        xml.writeEndElement();
    }

    foreach (const Record &record, mRecords)
    {
        xml.writeStartElement("add_record");
            xml.writeAttribute("substation", record.mSubstation);
            xml.writeAttribute("date",       record.mDate.toString("yyyy-MM-dd"));
            xml.writeCharacters(QString::number(record.mValue));
        xml.writeEndElement();
    }

    foreach (const Record &record, mMainHeatMeterRecords)
    {
        xml.writeStartElement("add_mainheatmeter_record");
            xml.writeAttribute("date", record.mDate.toString("yyyy-MM-dd"));
            xml.writeCharacters(QString::number(record.mValue));
        xml.writeEndElement();
    }

    xml.writeEndElement(); //heating_system
    xml.writeEndDocument();

    file->close();
}

HeatingSystem::FuelDelivery* HeatingSystem::findDelivery(QString hash)
{
    FuelDelivery* d = NULL;

    for (FuelDelivery &delivery : mFuelDeliveries)
    {
        if (delivery.getHash() == hash)
            d = &delivery;
    }
    return d;
}
