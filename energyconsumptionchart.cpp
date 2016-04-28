#include "energyconsumptionchart.h"
#include <heatingsystem.h>
#include <dju.h>

EnergyConsumptionChart::EnergyConsumptionChart(QWidget *parent) :
    QFrame(parent)
{
    mChart = new QCustomPlot(this);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(mChart);
    setLayout(layout);

    mChart->setLocale(QLocale(QLocale::French, QLocale::France));
    mChart->plotLayout()->insertRow(0);
    mChart->plotLayout()->addElement(0, 0, new QCPPlotTitle(mChart, "Total des consommations de chaleur des sous-stations"));

    //Legend
    mChart->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(7);
    mChart->legend->setFont(legendFont);
    mChart->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);

    //X-Axis: X-Axis is based on time and range match the heating season.
    mChart->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    double beginningHeatingSeason = QDateTime(QDate(2015, 07, 01), QTime(0, 0, 0)).toTime_t();
    double endHeatingSeason = QDateTime(QDate(2016, 06, 30), QTime(0, 0, 0)).toTime_t();
    mChart->xAxis->setRange(beginningHeatingSeason, endHeatingSeason);

    //X-Axis: Set a format for dates
    mChart->xAxis->setDateTimeFormat("der MMM\nyyyy");
    mChart->xAxis->setTickLabelFont(QFont(QFont().family(), 7));

    //X-Axis: manually add ticks
    mChart->xAxis->setAutoTicks(false);
    QVector<double> ticks;
    for (int i = 7 ; i <= 7+12 ; ++i)
    {
        ticks.push_back(QDateTime(QDate(2015+(i-1)/12, (i-1)%12+1, 01), QTime(0, 0, 0)).toTime_t());
    }
    mChart->xAxis->setTickVector(ticks);

    //Y-Axis
    mChart->yAxis->setLabel("Consommation (MWh)");
    mChart->yAxis->setRangeLower(0);
    mChart->yAxis->setAutoTickStep(false);

    //Add graphs. 0 will be 'real sum of meters in substations', 1 will be 'theoretic sum based on real DJU', 2 will be 'theoretic sum based on theoric DJU'
    mChart->addGraph();
    mChart->graph(0)->setName("Relevés des compteurs de chaleur");

    mChart->addGraph();
    mChart->graph(1)->setPen(QPen(Qt::red));
    mChart->graph(1)->setName("Estimation à partir des données réelles de température");

    mChart->addGraph();
    QPen redDotPen;
    redDotPen.setColor(Qt::red);
    redDotPen.setStyle(Qt::DotLine);
    mChart->graph(2)->setPen(redDotPen);
    mChart->graph(2)->setName("Estimation à partir de données théoriques de température");
}

EnergyConsumptionChart::~EnergyConsumptionChart()
{

}

void EnergyConsumptionChart::updateChart(HeatingSystem *heatingSystem, DJU* dju)
{
    if (heatingSystem == NULL)
        return;

    //Remove data from ui->tableWidget_chartResults
    while (resultsTable->rowCount() > 0)
        resultsTable->removeRow(0);

    double theoreticAnnualConsumption = 0;

    for (QPair<QString, double> &substation : heatingSystem->mSubstations)
    {
        theoreticAnnualConsumption += substation.second;

        int i = resultsTable->rowCount();

        QTableWidgetItem *newItem = new QTableWidgetItem();
        newItem->setData(Qt::EditRole, substation.second);
        newItem->setFlags(Qt::NoItemFlags);
        newItem->setTextAlignment(Qt::AlignCenter);
        newItem->setTextColor(Qt::black);

        //ui->tableWidget_chartResults will be filled with consumptions. These values will be used later.
        resultsTable->insertRow(i);
        resultsTable->setItem(i, 0, newItem);
        resultsTable->setVerticalHeaderItem(i, new QTableWidgetItem(substation.first));
    }

    //1. Graph 0: 'real sum of meters in substations'.
    QMap<QDate, int> meterRecords;

    for (const HeatingSystem::Record &record : heatingSystem->mRecords)
    {
        if (meterRecords.find(record.mDate) == meterRecords.end())
            meterRecords.insert(record.mDate, record.mValue);
        else meterRecords[record.mDate] += record.mValue;
    }

    QVector<double> x0(meterRecords.size()), y0(meterRecords.size());

    QMap<QDate, int>::iterator it = meterRecords.begin();
    for (int i = 0 ; i < meterRecords.size() ; ++i)
    {
        x0[i] = QDateTime(it.key()).toTime_t();
        y0[i] = it.value() / 1000.f;
        ++it;
    }

    mChart->graph(0)->setData(x0, y0);

    //2. Graph 1: 'theoretic sum based on real DJU'

    if (dju == NULL) return;

    QDate heatingSeasonBegin(2015, 7, 1);
    QDate heatingSeasonEnd  (2016, 6, 30);
    QDate lastKnownTemperature = QDate::fromString(dju->getLastDataDate(), "yyyy-MM-dd");

    double averageDJUOfHeatingSeason = dju->getAverageDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

    int size1 = qMax(qint64(1), heatingSeasonBegin.daysTo(lastKnownTemperature) + 1);
    QVector <double> x1(size1), y1(size1);

    QDate d = heatingSeasonBegin;

    x1[0] = QDateTime(d).toTime_t();
    y1[0] = dju->getDJU(d.toString("yyyy-MM-dd")) / averageDJUOfHeatingSeason * theoreticAnnualConsumption;

    for (int i = 1 ; i < size1 ; ++i)
    {
        d = d.addDays(1);

        x1[i] = QDateTime(d).toTime_t();
        y1[i] = y1[i-1] + dju->getDJU(d.toString("yyyy-MM-dd")) / averageDJUOfHeatingSeason * theoreticAnnualConsumption;
    }

    mChart->graph(1)->setData(x1, y1);

    //3. Graph 2: 'theoretic sum based on theoric DJU'

    double DJUSinceBeginningOfHeatingSeason = dju->getDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), lastKnownTemperature.toString("yyyy-MM-dd"));
    double DJURestOfHeatingSeason           = dju->getAverageDJU(lastKnownTemperature.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

    int size2 = lastKnownTemperature.daysTo(heatingSeasonEnd) + 1;
    QVector <double> x2(size2), y2(size2);

    d = lastKnownTemperature;

    x2[0] = QDateTime(d).toTime_t();
    y2[0] = y1[size1 - 1];

    for (int i = 1 ; i < size2 ; ++i)
    {
        d = d.addDays(1);

        x2[i] = QDateTime(d).toTime_t();
        y2[i] = y2[i-1] + dju->getAverageDJU(d.toString("yyyy-MM-dd")) / (DJUSinceBeginningOfHeatingSeason + DJURestOfHeatingSeason) * theoreticAnnualConsumption;
    }

    mChart->graph(2)->setData(x2, y2);

    //4. Replot
    mChart->yAxis->setRangeUpper(theoreticAnnualConsumption * 1.2);

    double step = theoreticAnnualConsumption * 1.2;
    int power = 0;
    while (step > 10)
    {
        step /= 10;
        ++power;
    }
    step = round(step) * pow(10, power);
    mChart->yAxis->setTickStep(step / 10);

    mChart->replot();

    //5. Results

    QLocale locale;

    //Expected consumption
    double expectedConsumptionMWh = y2[size2 - 1];
    expectedLabel->setText("<b>" + locale.toString(expectedConsumptionMWh, 'g', 3) + " MWh</b>");

    //Shift
    double theoreticConsumption = 0;
    if (!meterRecords.isEmpty() && meterRecords.lastKey() > heatingSeasonBegin && meterRecords.lastKey() < heatingSeasonEnd)
    {
        if (meterRecords.lastKey() <= lastKnownTemperature)
            theoreticConsumption = y1[heatingSeasonBegin.daysTo(meterRecords.lastKey()) - 1];
        else
            theoreticConsumption = y2[lastKnownTemperature.daysTo(meterRecords.lastKey()) - 1];
    }

    double shift = 0;
    if (!meterRecords.isEmpty())
        shift = (meterRecords.last() / 1000.f - theoreticConsumption) / theoreticConsumption * 100.f;
    shiftLabel->setText(((shift >= 0) ? "<b>+ " : "<b>- ") + locale.toString((shift >= 0) ? shift : -shift, 'g', 3) + " %</b>");

    //Expected corrected consumption
    double DJUcovered = 0;

    d = heatingSeasonBegin;
    while (!meterRecords.isEmpty() && d < meterRecords.lastKey())
    {
        if (d <= lastKnownTemperature)
            DJUcovered += dju->getDJU(d.toString("yyyy-MM-dd"));
        else
            DJUcovered += dju->getAverageDJU(d.toString("yyyy-MM-dd"));

        d = d.addDays(1);
    }

    double expectedCorrectedConsumptionMWh = 0;
    if (!meterRecords.isEmpty())
        expectedCorrectedConsumptionMWh = meterRecords.last() / 1000.f / DJUcovered * dju->getAverageDJU("2010-01-01", "2010-12-31");
    expectedCorrectedLabel->setText("<b>" + locale.toString(expectedCorrectedConsumptionMWh, 'g', 3) + " MWh</b>");
}

void EnergyConsumptionChart::setResultsTable(QTableWidget *t)
{
    resultsTable = t;
}

void EnergyConsumptionChart::setExpectedLabel(QLabel* l)
{
    expectedLabel = l;
}

void EnergyConsumptionChart::setShiftLabel(QLabel* l)
{
    shiftLabel = l;
}

void EnergyConsumptionChart::setExpectedCorrectedLabel(QLabel* l)
{
    expectedCorrectedLabel = l;
}
