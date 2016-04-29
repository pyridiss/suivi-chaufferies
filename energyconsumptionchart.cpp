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

    //Set chart properties
    setChartTitle(djuState);
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
     * Data needed to plot the graphs
     */

    double theoreticAnnualConsumption = 0;
    for (QPair<QString, double> &substation : heatingSystem->mSubstations)
        theoreticAnnualConsumption += substation.second;

    QMap<QDate, int> meterRecords;
    for (const HeatingSystem::Record &record : heatingSystem->mRecords)
    {
        if (meterRecords.find(record.mDate) == meterRecords.end())
            meterRecords.insert(record.mDate, record.mValue);
        else meterRecords[record.mDate] += record.mValue;
    }

    QDate heatingSeasonBegin(2015, 7, 1);
    QDate heatingSeasonEnd  (2016, 6, 30);
    QDate lastDJU = QDate::fromString(dju->getLastDataDate(), "yyyy-MM-dd");
    QDate lastRecord = (meterRecords.empty() == false) ? meterRecords.lastKey() : QDate();
    QDate d;

    double averageDJUOfHeatingSeason = dju->getAverageDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

    //Coordinates
    QVector<double> x0, y0, x1, y1, x2, y2, x3, y3;

    /*
     * Graph #0: Sum of meters in substations
     */

    //Add a new layer to keep this graph on the top
    mChart->addLayer("mainGraph", mChart->layer("main"));

    mChart->setCurrentLayer("mainGraph");
    QCPGraph* graph0 = mChart->addGraph();
    graph0->setPen(greenPen);
    graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
    graph0->setName("Relevés des compteurs de chaleur");

    x0.resize(meterRecords.size());
    y0.resize(meterRecords.size());

    QMap<QDate, int>::iterator it = meterRecords.begin();
    for (int i = 0 ; i < meterRecords.size() ; ++i)
    {
        x0[i] = QDateTime(it.key()).toTime_t();
        y0[i] = it.value() / 1000.f;
        ++it;
    }

    graph0->setData(x0, y0);

    /*
     * Graph #1: Extension of the current data
     */

    if ((djuState == DJU_OK || djuState == DJU_LESSTHAN3YEARS) && meterRecords.empty() == false)
    {
        QCPGraph* graph1 = mChart->addGraph();

        greenPen.setStyle(Qt::DotLine);

        graph1->setPen(greenPen);
        graph1->setName("Estimation des relevés futurs");

        x1.resize(lastRecord.daysTo(heatingSeasonEnd) + 1);
        y1.resize(lastRecord.daysTo(heatingSeasonEnd) + 1);

        x1[0] = QDateTime(lastRecord).toTime_t();
        y1[0] = y0[meterRecords.size() - 1];

        double DJUcovered = 0;

        d = heatingSeasonBegin;
        while (!meterRecords.isEmpty() && d < meterRecords.lastKey())
        {
            if (d <= lastDJU)
                DJUcovered += dju->getDJU(d.toString("yyyy-MM-dd"));
            else
                DJUcovered += dju->getAverageDJU(d.toString("yyyy-MM-dd"));

            d = d.addDays(1);
        }

        d = lastRecord;
        for (int i = 1 ; i < x1.size() ; ++i)
        {
            d = d.addDays(1);

            x1[i] = QDateTime(d).toTime_t();
            y1[i] = y1[i-1] + meterRecords.last() /1000.f / DJUcovered * dju->getAverageDJU(d.toString("yyyy-MM-dd"));
        }

        graph1->setData(x1, y1);

        //Result
        greenPen.setStyle(Qt::SolidLine);

        QCPItemTracer *tracer1 = new QCPItemTracer(mChart);
        mChart->addItem(tracer1);
        tracer1->setGraph(graph1);
        tracer1->setGraphKey(x1[x1.size() - 3]);
        tracer1->setStyle(QCPItemTracer::tsCircle);
        tracer1->setPen(greenPen);
        tracer1->setBrush(greenPen.color());
        tracer1->setSize(7);

        QCPItemText *label1 = new QCPItemText(mChart);
        mChart->addItem(label1);
        label1->position->setType(QCPItemPosition::ptAxisRectRatio);
        label1->setPositionAlignment(Qt::AlignRight);
        label1->position->setCoords(0.95, 0.6);
        label1->setText("Trajectoire actuelle :\n" + locale.toString(y1[y1.size() - 1], 'g', 3) + " MWh");
        label1->setTextAlignment(Qt::AlignHCenter);
        label1->setFont(QFont("sans", 8));

        QCPItemCurve *arrow1 = new QCPItemCurve(mChart);
        mChart->addItem(arrow1);
        arrow1->start->setParentAnchor(label1->right);
        arrow1->startDir->setParentAnchor(arrow1->start);
        arrow1->startDir->setCoords(10, 0);
        arrow1->end->setParentAnchor(tracer1->position);
        arrow1->end->setCoords(-5, 5);
        arrow1->endDir->setParentAnchor(arrow1->end);
        arrow1->endDir->setCoords(-30, 30);
        arrow1->setHead(QCPLineEnding::esSpikeArrow);
    }

    /*
     * Graph #2 and #3: Theoretic sums
     */

    if (djuState == DJU_OK || djuState == DJU_LESSTHAN3YEARS)
    {
        // #1: theoretic sum based on real DJU
        mChart->setCurrentLayer("main");
        QCPGraph* graph2 = mChart->addGraph();
        graph2->setPen(redPen);
        graph2->setName("Estimation à partir des données réelles de température");

        x2.resize(qMax(qint64(1), heatingSeasonBegin.daysTo(lastDJU) + 1));
        y2.resize(qMax(qint64(1), heatingSeasonBegin.daysTo(lastDJU) + 1));

        d = heatingSeasonBegin;

        x2[0] = QDateTime(d).toTime_t();
        y2[0] = dju->getDJU(d.toString("yyyy-MM-dd")) / averageDJUOfHeatingSeason * theoreticAnnualConsumption;

        for (int i = 1 ; i < x2.size() ; ++i)
        {
            d = d.addDays(1);

            x2[i] = QDateTime(d).toTime_t();
            y2[i] = y2[i-1] + dju->getDJU(d.toString("yyyy-MM-dd")) / averageDJUOfHeatingSeason * theoreticAnnualConsumption;
        }

        graph2->setData(x2, y2);

        // #2: theoretic sum based on average DJU
        QCPGraph* graph3 = mChart->addGraph();

        redPen.setStyle(Qt::DotLine);

        graph3->setPen(redPen);
        graph3->setName("Estimation à partir de données théoriques de température");

        double DJUSinceBeginningOfHeatingSeason = dju->getDJU(heatingSeasonBegin.toString("yyyy-MM-dd"), lastDJU.toString("yyyy-MM-dd"));
        double DJURestOfHeatingSeason           = dju->getAverageDJU(lastDJU.toString("yyyy-MM-dd"), heatingSeasonEnd.toString("yyyy-MM-dd"));

        x3.resize(lastDJU.daysTo(heatingSeasonEnd) + 1);
        y3.resize(lastDJU.daysTo(heatingSeasonEnd) + 1);

        x3[0] = QDateTime(d).toTime_t();
        y3[0] = y2[y2.size() - 1];

        d = lastDJU;

        for (int i = 1 ; i < x3.size() ; ++i)
        {
            d = d.addDays(1);

            x3[i] = QDateTime(d).toTime_t();
            y3[i] = y3[i-1] + dju->getAverageDJU(d.toString("yyyy-MM-dd")) / (DJUSinceBeginningOfHeatingSeason + DJURestOfHeatingSeason) * theoreticAnnualConsumption;
        }

        graph3->setData(x3, y3);

        //Result
        redPen.setStyle(Qt::SolidLine);

        QCPItemTracer *tracer3 = new QCPItemTracer(mChart);
        mChart->addItem(tracer3);
        tracer3->setGraph(graph3);
        tracer3->setGraphKey(x3[x3.size() - 3]);
        tracer3->setStyle(QCPItemTracer::tsCircle);
        tracer3->setPen(redPen);
        tracer3->setBrush(redPen.color());
        tracer3->setSize(7);

        QCPItemText *label3 = new QCPItemText(mChart);
        mChart->addItem(label3);
        label3->position->setType(QCPItemPosition::ptAxisRectRatio);
        label3->setPositionAlignment(Qt::AlignRight);
        label3->position->setCoords(0.95, 0.02);
        label3->setText("Consommation théorique cette année :\n" + locale.toString(y3[y3.size() - 1], 'g', 3) + " MWh");
        label3->setTextAlignment(Qt::AlignHCenter);
        label3->setFont(QFont("sans", 8));

        QCPItemCurve *arrow3 = new QCPItemCurve(mChart);
        mChart->addItem(arrow3);
        arrow3->start->setParentAnchor(label3->right);
        arrow3->startDir->setParentAnchor(arrow3->start);
        arrow3->startDir->setCoords(10, 0);
        arrow3->end->setParentAnchor(tracer3->position);
        arrow3->end->setCoords(-5, -5);
        arrow3->endDir->setParentAnchor(arrow3->end);
        arrow3->endDir->setCoords(-30, -30);
        arrow3->setHead(QCPLineEnding::esSpikeArrow);
    }

    /*
     * End
     */

    setChartAxis(theoreticAnnualConsumption * 1.2);
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
