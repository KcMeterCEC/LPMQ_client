#ifndef DISPIECHART_H
#define DISPIECHART_H

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QObject>

QT_CHARTS_USE_NAMESPACE

class DonutBreakdownChart;
class DisPieChart : public QObject
{
    Q_OBJECT
public:
    explicit DisPieChart(QObject *parent = nullptr);

    QChartView *createPsChart();
    void  refreshPsChart(const QMap<QString, double> &info);

    QChartView *createMemChart();
    void  refreshMemChart(const QMap<QString, double> &info);
private:
    DonutBreakdownChart     *psChart;
    QPieSeries              *psUsed;
    QPieSeries              *psFree;

    DonutBreakdownChart     *memChart;
    QPieSeries              *memUsed;
    QPieSeries              *memFree;

signals:

};

#endif // DISPIECHART_H
