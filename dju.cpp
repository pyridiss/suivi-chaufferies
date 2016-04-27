#include <QStandardPaths>
#include <QString>
#include <QDir>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QLocale>

#include "dju.h"

DJU::DJU()
{
}

void DJU::load(QString weatherStation)
{
    mWeatherStation = weatherStation;

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

    QXmlStreamReader xml(file);
    QLocale locale;
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
            if (xml.name() == "complete_year")
            {
                int date = locale.toDouble(xml.readElementText());
                mCompleteYears.push_back(date);
            }
            if (xml.name() == "dju")
            {
                QString date = xml.attributes().first().value().toString();
                double  dju  = locale.toDouble(xml.readElementText());
                mDJU.insert(date, dju);
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
    foreach (int i, mCompleteYears)
    {
        date.remove(0, 4);
        date.insert(0, QString::number(i));
        averageDJU += getDJU(date);
    }

    averageDJU /= mCompleteYears.size();

    return averageDJU;
}

double DJU::getAverageDJU(QString date1, QString date2)
{
    double averageDJU = 0;
    QString d1 = date1, d2 = date2;
    foreach (int i, mCompleteYears)
    {
        if (date2.left(4) > date1.left(4)) //The period overlaps two years
        {
            if (i == mCompleteYears.last()) continue;
            d1.remove(0, 4);
            d1.insert(0, QString::number(i));
            d2.remove(0, 4);
            d2.insert(0, QString::number(i+1));
        }
        else
        {
            d1.remove(0, 4);
            d1.insert(0, QString::number(i));
            d2.remove(0, 4);
            d2.insert(0, QString::number(i));
        }
        averageDJU += getDJU(d1, d2);
    }

    if (date2.left(4) > date1.left(4))
    {
        averageDJU /= (mCompleteYears.size() - 1);
    }
    else
    {
        averageDJU /= mCompleteYears.size();
    }

    return averageDJU;
}
