#ifndef ENERGYCONSUMPTIONCHART_H
#define ENERGYCONSUMPTIONCHART_H

#include <QFrame>
#include "qcustomplot/qcustomplot.h"

class HeatingSystem;
class DJU;

class EnergyConsumptionChart : public QFrame
{
    Q_OBJECT

public:
    explicit EnergyConsumptionChart(QWidget *parent = 0);
    ~EnergyConsumptionChart();

public:
    void updateChart(HeatingSystem* heatingSystem, DJU *dju);
    void setResultsTable(QTableWidget *t);
    void setExpectedLabel(QLabel* l);
    void setShiftLabel(QLabel* l);
    void setExpectedCorrectedLabel(QLabel* l);

private:
    QCustomPlot *mChart;

    QTableWidget *resultsTable;
    QLabel* expectedLabel;
    QLabel* shiftLabel;
    QLabel* expectedCorrectedLabel;
};

#endif // ENERGYCONSUMPTIONCHART_H
