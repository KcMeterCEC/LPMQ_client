#include "dislinechart.h"
#include <QDebug>
#include <QtCharts/QAreaSeries>
#include <QPointF>

QT_CHARTS_USE_NAMESPACE

DisLineChart::DisLineChart(QObject *parent)
: QObject(parent),
  psLineChart(new QChart()),
  psAxisX (new QValueAxis()), psAxisY (new QValueAxis()),
  memLineChart(new QChart())
{
    psLineChart->setTitle(tr("history of cpu usage"));

    //animation will take many cpu usage
//    psLineChart->setAnimationOptions(QChart::AllAnimations);
}
void DisLineChart::insertPsChart(quint8 cpuNum)
{
    QLineSeries * line = nullptr;
    for(quint8 i = 0; i < cpuNum; ++i)
    {
        line = new QLineSeries();

        psLine.push_back(line);

        psLineChart->addSeries(line);
        psLineChart->addAxis(psAxisX, Qt::AlignBottom);
        psLineChart->addAxis(psAxisY, Qt::AlignLeft);

        line->attachAxis(psAxisX);
        line->attachAxis(psAxisY);
    }
    hasPsLine = true;
}
QChartView *DisLineChart::createPsChart(void)
{
    QChartView *chartView = new QChartView(psLineChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}
QChartView *DisLineChart::createMemChart(void)
{
    memLineChart->setTitle(tr("history of memory usage"));

    QLineSeries * lineUsed = new QLineSeries(memLineChart);
    QLineSeries * lineUsedAddBuffers = new QLineSeries(memLineChart);
    QLineSeries * lineUsedAddBuffersAndCache = new QLineSeries(memLineChart);
    QLineSeries * lineTotal = new QLineSeries(memLineChart);

    memLine.push_back(lineUsed);
    memLine.push_back(lineUsedAddBuffers);
    memLine.push_back(lineUsedAddBuffersAndCache);
    memLine.push_back(lineTotal);

    QAreaSeries *areaUsed = new QAreaSeries(lineUsed);
    areaUsed->setName("used");
    areaUsed->setColor(Qt::red);

    QAreaSeries *areaBuffers = new QAreaSeries(lineUsedAddBuffers, lineUsed);
    areaBuffers->setName("buffers");

    QAreaSeries *areaCached = new QAreaSeries(lineUsedAddBuffersAndCache, lineUsedAddBuffers);
    areaCached->setName("cached");

    QAreaSeries *areaFree = new QAreaSeries(lineTotal, lineUsedAddBuffersAndCache);
    areaFree->setName("free");
    areaFree->setColor(Qt::darkCyan);

    memLineChart->addSeries(areaUsed);
    memLineChart->addSeries(areaBuffers);
    memLineChart->addSeries(areaCached);
    memLineChart->addSeries(areaFree);

    memLineChart->createDefaultAxes();

    QChartView *chartView = new QChartView(memLineChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}
void  DisLineChart::refreshMemChart(const QMap<QString, qulonglong> &info)
{
    memLine[0]->append(memLine[0]->count(), info.value("mem.used"));
    memLine[1]->append(memLine[1]->count(), info.value("mem.used") + info.value("mem.buffers"));
    memLine[2]->append(memLine[2]->count(), info.value("mem.used") + info.value("mem.buffers") + info.value("mem.cache"));
    memLine[3]->append(memLine[3]->count(), info.value("mem.total"));

    memLineChart->axes(Qt::Vertical).first()->setMax(info.value("mem.total"));
    memLineChart->axes(Qt::Horizontal).first()->setMax(memLine[0]->count());
}
bool  DisLineChart::psLineCreated(void)
{
    return hasPsLine;
}
void  DisLineChart::refreshPsChart(const QMap<QString, double> &info)
{
    for(int i = 0; i < info.value("cpu count"); ++i)
    {
//        qDebug() << QString("cpu%1").arg(i)
//                 << info.value(QString("cpu%1.usage").arg(i));
        psLine[i]->setName(QString("cpu%1").arg(i));
        psLine[i]->append(psLine[i]->count(), info.value(QString("cpu%1.usage").arg(i)));
    }

    psPointAdded(psLine[0]->count() - 1);
}
void  DisLineChart::psPointAdded(int index)
{
    for (const auto *series : psLine)
    {
        qreal y = series->at(index).y();

        if(y > yMax)
        {
            yMax = y;
            psAxisY->setMax(yMax);
        }
    }
    psAxisX->setMax(psLine.at(0)->count());
}
