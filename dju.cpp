#include <QStandardPaths>
#include <QString>
#include <QDir>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QDate>

#include "dju.h"

DJU::DJU()
{
}

void DJU::loadFromFile(QString weatherStation)
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");

    if (!dir.exists())
    {
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/SuiviChaufferies/");
    }

    QFile* file = new QFile(dir.filePath(weatherStation + ".xml"));
    bool fileOpened = file->open(QIODevice::ReadOnly);
    if (!fileOpened)
    {
        QMessageBox::warning(0, "Information", "Aucun fichier de données des DJU n'a été trouvé.");
        return;
    }
    load(weatherStation, file->readAll());

    file->close();
}

void DJU::load(QString weatherStation, const QByteArray& data)
{
    mWeatherStation = weatherStation;

    QXmlStreamReader xml(data);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }

        if(token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "DJU")
            {
                continue;
            }
            if (xml.name() == "complete-month")
            {
                QString date = xml.readElementText();
                mCompleteMonthes.push_back(date);
            }
            if (xml.name() == "dju")
            {
                QString date = xml.attributes().first().value().toString();
                double  dju  = xml.readElementText().toDouble();
                mDJU.insert(date, dju);
            }
        }
    }

    if(xml.hasError())
    {
        QMessageBox::critical(0, "xml Parse Error", xml.errorString(), QMessageBox::Ok);
        return;
    }

    //Check if data is exploitable and warn about it if necessary
    if (mCompleteMonthes.count() < 12)
        QMessageBox::warning(0, "Attention",
                             "Les données de température ne couvrent pas une année complète ; il est impossible de déterminer une consommation théorique.\n(Station météo : " + mWeatherStation + ").");
    else if (mCompleteMonthes.count() < 36)
        QMessageBox::warning(0, "Attention",
                             "Les données de température couvrent moins de 3 années ; la consommation théorique sera peu précise.\n(Station météo : " + mWeatherStation + ").");
}

double DJU::getDJU(QString date)
{
    QMap<QString, double>::iterator i = mDJU.find(date);
    if (i != mDJU.end()) return i.value();
    return 0;
}

double DJU::getDJU(QString date1, QString date2)
{
    //This function is used to calculate DJU's between two dates.
    //It is often called with the same dates, so we will store values in mPeriodicDJU to avoid repeated calculations.

    double sum = 0;

    //1. Check if the sum has been already calculated.
    QMap< std::pair<QString, QString>, double>::iterator i = mPeriodicDJU.find(std::pair<QString, QString>(date1, date2));
    if (i == mPeriodicDJU.end())
    {
        //No record found; we calculate manually
        QMap<QString, double>::iterator j = mDJU.begin();

        while (j != mDJU.end() && j.key() < date1)
        {
            ++j;
        }

        while (j != mDJU.end() && j.key() <= date2)
        {
            sum += j.value();
            ++j;
        }

        //Add sum in mPeriodicDJU
        mPeriodicDJU.insert(std::pair<QString, QString>(date1, date2), sum);
    }
    else
    {
        sum = i.value();
    }

    return sum;
}

QString DJU::getLastDataDate()
{
    if (mDJU.isEmpty()) return "2000-01-01";
    return mDJU.lastKey();
}

double DJU::getAverageDJU(QString date)
{
    double averageDJU = 0;
    int n = 0;
    foreach (QString i, mCompleteMonthes)
    {
        //Check if we have complete information about the asked month
        if (i[5] == date[5] && i[6] == date[6])
        {
            date.remove(0, 7);
            date.insert(0, i);
            averageDJU += getDJU(date);
            ++n;
        }
    }

    averageDJU /= n;

    return averageDJU;
}

double DJU::getAverageDJU(QString date1, QString date2)
{
    //This function is *really* inefficient; but it works for now...

    double averageDJU = 0;
    QDate d2 = QDate::fromString(date2, "yyyy-MM-dd");

    for (QDate date = QDate::fromString(date1,"yyyy-MM-dd") ; date < d2 ; date = date.addDays(1))
    {
        averageDJU += getAverageDJU(date.toString("yyyy-MM-dd"));
    }

    return averageDJU;
}
