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
}

EnergyConsumptionChart::~EnergyConsumptionChart()
{

}

void EnergyConsumptionChart::updateChart(HeatingSystem *heatingSystem, DJU* dju)
{
    //Clear everything
    clearChart();

    //Check heating system
    if (heatingSystem == NULL)
        return;

    //Check state of DJU data
    DJUstate djuState;

    if (dju == NULL) djuState = DJU_NOFILE;
    else if (dju->completeMonthesNumber() == 0) djuState = DJU_NOFILE;
    else if (dju->completeMonthesNumber() < 12) djuState = DJU_LESSTHAN1YEAR;
    else if (dju->completeMonthesNumber() < 36) djuState = DJU_LESSTHAN3YEARS;
    else djuState = DJU_OK;

    //Theoretic annual consumption
    double theoreticAnnualConsumption = 0;
    for (QPair<QString, double> &substation : heatingSystem->mSubstations)
        theoreticAnnualConsumption += substation.second;

    //Set chart properties
    setChartTitle(djuState);
    setChartAxis(theoreticAnnualConsumption * 1.2);
    setChartLegend();

    /*
     * Graph #0: Sum of meters in substations
     */

    //Add a new layer to keep this graph on the top
    mChart->addLayer("mainGraph", mChart->layer("main"));
    mChart->setCurrentLayer("mainGraph");
    mChart->addGraph();

    QPen pen;
    pen.setColor(QColor(55, 164, 44));
    pen.setWidthF(2);

    mChart->graph(0)->setPen(pen);
    mChart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
    mChart->graph(0)->setName("Relevés des compteurs de chaleur");

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

    /*
     * Graph #1 and #2: Theoretic sums
     */

    mChart->setCurrentLayer("main");

    if (djuState == DJU_OK || djuState == DJU_LESSTHAN3YEARS)
    {
        // #1: based on real DJU
        mChart->addGraph();

        QPen pen;
        pen.setColor(QColor(232, 87, 82));
        pen.setWidthF(1.5);

        mChart->graph(1)->setPen(pen);
        mChart->graph(1)->setName("Estimation à partir des données réelles de température");

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

        // #2: based on average DJU
        mChart->addGraph();

        pen.setStyle(Qt::DotLine);

        mChart->graph(2)->setPen(pen);
        mChart->graph(2)->setName("Estimation à partir de données théoriques de température");

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

        //Results
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

    /*
     * End
     */

    mChart->replot();

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

void EnergyConsumptionChart::clearChart()
{
    //We start with a brand new QCustomPlot... not really efficient, but it works!
    delete mChart;
    mChart = new QCustomPlot(this);
    layout()->addWidget(mChart);
    mChart->setLocale(QLocale(QLocale::French, QLocale::France));

    //Remove data from labels
    expectedLabel->setText("-");
    shiftLabel->setText("-");
    expectedCorrectedLabel->setText("-");
}

void EnergyConsumptionChart::setChartTitle(DJUstate djuState)
{
    //Main title
    mChart->plotLayout()->insertRow(0);
    mChart->plotLayout()->addElement(0, 0, new QCPPlotTitle(mChart, "Total des consommations de chaleur des sous-stations"));

    //Add subtitle if DJU data is not sufficient
    if (djuState != DJU_OK)
    {
        QCPPlotTitle *subTitle = new QCPPlotTitle(mChart);
        subTitle->setFont(QFont("sans", 8));
        subTitle->setTextColor(Qt::red);

        switch (djuState)
        {
            case DJU_LESSTHAN3YEARS:
                subTitle->setText("Attention : les données de température couvrent moins de 3 années.\nLa consommation théorique sera peu précise.");
                break;
            case DJU_LESSTHAN1YEAR:
                subTitle->setText("Attention : les données de température ne couvrent pas une année complète.\nIl est impossible de déterminer une consommation théorique.");
                break;
            case DJU_NOFILE:
                subTitle->setText("Attention : aucune donnée de DJU n'est disponible.\nVeuillez mettre à jour les données de DJU (Données > Mettre à jour les données).");
                break;
            default:
                break;
        }

        mChart->plotLayout()->insertRow(1);
        mChart->plotLayout()->addElement(1, 0, subTitle);
    }
}

void EnergyConsumptionChart::setChartAxis(double ymax)
{
    //X-Axis is based on time and range matches the heating season.
    double begHeatingSeason = QDateTime(QDate(2015, 07, 01), QTime(0, 0, 0)).toTime_t();
    double endHeatingSeason = QDateTime(QDate(2016, 06, 30), QTime(0, 0, 0)).toTime_t();

    mChart->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    mChart->xAxis->setRange(begHeatingSeason, endHeatingSeason);
    mChart->xAxis->setDateTimeFormat("der MMM\nyyyy");
    mChart->xAxis->setTickLabelFont(QFont("sans", 7));

    //manually add ticks
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
    mChart->yAxis->setRangeUpper(ymax);

    double step = ymax;
    int power = 0;
    while (step > 10)
    {
        step /= 10;
        ++power;
    }
    step = round(step) * pow(10, power);
    mChart->yAxis->setTickStep(step / 10);
}

void EnergyConsumptionChart::setChartLegend()
{
    mChart->legend->setVisible(true);
    mChart->legend->setFont(QFont("sans", 7));
    mChart->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
}
