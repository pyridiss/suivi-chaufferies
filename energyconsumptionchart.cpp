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

    //Set pens
    QPen greenPen, redPen;
    greenPen.setColor(QColor(55, 164, 44));
    greenPen.setWidthF(2);
    redPen.setColor(QColor(232, 87, 82));
    redPen.setWidthF(1.5);

    //Other things
    QLocale locale;

    /*
     * Graph #0: Sum of meters in substations
     */

    //Add a new layer to keep this graph on the top
    mChart->addLayer("mainGraph", mChart->layer("main"));

    mChart->setCurrentLayer("mainGraph");
    mChart->addGraph();
    mChart->graph(0)->setPen(greenPen);
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
     * Graph #1 to #3: Theoretic sums
     */

    if (djuState == DJU_OK || djuState == DJU_LESSTHAN3YEARS)
    {
        // #1: theoretic sum based on real DJU
        mChart->setCurrentLayer("main");
        mChart->addGraph();
        mChart->graph(1)->setPen(redPen);
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

        // #2: theoretic sum based on average DJU
        mChart->addGraph();

        redPen.setStyle(Qt::DotLine);

        mChart->graph(2)->setPen(redPen);
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

        // #3: extension of the current data
        //TODO: Remove this graph if no data is recorded.
        mChart->setCurrentLayer("mainGraph");
        mChart->addGraph();

        greenPen.setStyle(Qt::DotLine);

        mChart->graph(3)->setPen(greenPen);
        mChart->graph(3)->setName("Estimation des relevés futurs");

        QDate lastRecord = meterRecords.lastKey();

        int size3 = lastRecord.daysTo(heatingSeasonEnd) + 1;
        QVector <double> x3(size3), y3(size3);

        x3[0] = QDateTime(lastRecord).toTime_t();
        y3[0] = y0[meterRecords.size() - 1];

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

        d = lastRecord;
        for (int i = 1 ; i < size3 ; ++i)
        {
            d = d.addDays(1);

            x3[i] = QDateTime(d).toTime_t();
            y3[i] = y3[i-1] + meterRecords.last() /1000.f / DJUcovered * dju->getAverageDJU(d.toString("yyyy-MM-dd"));
        }

        mChart->graph(3)->setData(x3, y3);

        /*
         * Results
         */

        greenPen.setStyle(Qt::SolidLine);
        redPen.setStyle(Qt::SolidLine);

        //Expected consumption
        QCPItemTracer *tracer1 = new QCPItemTracer(mChart);
        mChart->addItem(tracer1);
        tracer1->setGraph(mChart->graph(2));
        tracer1->setGraphKey(x2[size2-3]);
        tracer1->setStyle(QCPItemTracer::tsCircle);
        tracer1->setPen(redPen);
        tracer1->setBrush(redPen.color());
        tracer1->setSize(7);

        QCPItemText *label1 = new QCPItemText(mChart);
        mChart->addItem(label1);
        label1->position->setType(QCPItemPosition::ptAxisRectRatio);
        label1->setPositionAlignment(Qt::AlignRight);
        label1->position->setCoords(0.95, 0.02);
        label1->setText("Consommation théorique cette année :\n" + locale.toString(y2[size2-1], 'g', 3) + " MWh");
        label1->setTextAlignment(Qt::AlignHCenter);
        label1->setFont(QFont("sans", 8));

        QCPItemCurve *arrow1 = new QCPItemCurve(mChart);
        mChart->addItem(arrow1);
        arrow1->start->setParentAnchor(label1->right);
        arrow1->startDir->setParentAnchor(arrow1->start);
        arrow1->startDir->setCoords(10, 0);
        arrow1->end->setParentAnchor(tracer1->position);
        arrow1->end->setCoords(-5, -5);
        arrow1->endDir->setParentAnchor(arrow1->end);
        arrow1->endDir->setCoords(-30, -30);
        arrow1->setHead(QCPLineEnding::esSpikeArrow);

        //Current trajectory
        QCPItemTracer *tracer2 = new QCPItemTracer(mChart);
        mChart->addItem(tracer2);
        tracer2->setGraph(mChart->graph(3));
        tracer2->setGraphKey(x3[size3-3]);
        tracer2->setStyle(QCPItemTracer::tsCircle);
        tracer2->setPen(greenPen);
        tracer2->setBrush(greenPen.color());
        tracer2->setSize(7);

        QCPItemText *label2 = new QCPItemText(mChart);
        mChart->addItem(label2);
        label2->position->setType(QCPItemPosition::ptAxisRectRatio);
        label2->setPositionAlignment(Qt::AlignRight);
        label2->position->setCoords(0.95, 0.6);
        label2->setText("Trajectoire actuelle :\n" + locale.toString(y3[size3-1], 'g', 3) + " MWh");
        label2->setTextAlignment(Qt::AlignHCenter);
        label2->setFont(QFont("sans", 8));

        QCPItemCurve *arrow2 = new QCPItemCurve(mChart);
        mChart->addItem(arrow2);
        arrow2->start->setParentAnchor(label2->right);
        arrow2->startDir->setParentAnchor(arrow2->start);
        arrow2->startDir->setCoords(10, 0);
        arrow2->end->setParentAnchor(tracer2->position);
        arrow2->end->setCoords(-5, 5);
        arrow2->endDir->setParentAnchor(arrow2->end);
        arrow2->endDir->setCoords(-30, 30);
        arrow2->setHead(QCPLineEnding::esSpikeArrow);
    }

    /*
     * End
     */

    mChart->replot();

}

void EnergyConsumptionChart::clearChart()
{
    //We start with a brand new QCustomPlot... not really efficient, but it works!
    delete mChart;
    mChart = new QCustomPlot(this);
    layout()->addWidget(mChart);
    mChart->setLocale(QLocale(QLocale::French, QLocale::France));
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
