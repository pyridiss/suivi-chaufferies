#ifndef ENERGYCONSUMPTIONCHART_H
#define ENERGYCONSUMPTIONCHART_H

#include <QFrame>
#include "qcustomplot/qcustomplot.h"

class HeatingSystem;
class DJU;

class EnergyConsumptionChart : public QFrame
{
    Q_OBJECT

    enum DJUstate {DJU_OK, DJU_LESSTHAN3YEARS, DJU_LESSTHAN1YEAR, DJU_NOFILE};

public:
    explicit EnergyConsumptionChart(QWidget *parent = 0);
    ~EnergyConsumptionChart();

public:
    void updateChart(HeatingSystem* heatingSystem, DJU *dju);

private:
    void clearChart();
    void setChartTitle(DJUstate djuState);
    void setChartAxis(double ymax);
    void setChartLegend();

private:
    QCustomPlot *mChart;
};

#endif // ENERGYCONSUMPTIONCHART_H
