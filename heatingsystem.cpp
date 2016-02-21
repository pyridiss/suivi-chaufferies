#include <QDir>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QLocale>
#include <QDate>

#include "heatingsystem.h"

HeatingSystem::HeatingSystem(QObject *parent) : QObject(parent)
{

}

void HeatingSystem::load(QString fileName)
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");

    QFile* file = new QFile(dir.filePath(fileName + ".xml"));

    bool fileOpened = file->open(QIODevice::ReadOnly);
    if (!fileOpened)
    {
        QMessageBox::warning(0, "Information", "La chaufferie demandée n'existe pas.");
        return;
    }

    QLocale locale;

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
                }
            }
            if (xml.name() == "heat_sources")
            {
                QXmlStreamAttributes xmlAttributes = xml.attributes();
                foreach (QXmlStreamAttribute a, xmlAttributes)
                {
                    if (a.name() == "first")
                        mMainHeatSource = (HeatingSystem::MainHeatSource) a.value().toInt();
                    if (a.name() == "second")
                        mSecondHeatSource = (HeatingSystem::SecondHeatSource) a.value().toInt();
                }
            }
            if (xml.name() == "add_substation")
            {
                QString substation = xml.readElementText();
                mSubstations.push_back(substation);
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
                }
            }
            if (xml.name() == "heatSell")
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
                newRecord.value = xml.readElementText().toDouble();

                mRecords.push_back(newRecord);
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
