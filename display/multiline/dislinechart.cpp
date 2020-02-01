#include "dislinechart.h"
#include <QDebug>


QT_CHARTS_USE_NAMESPACE

DisLineChart::DisLineChart(QObject *parent)
: QObject(parent),psLineChart(new QChart()),
  psAxisX (new QValueAxis()), psAxisY (new QValueAxis())
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
bool  DisLineChart::psLineCreated(void)
{
    return hasPsLine;
}
void  DisLineChart::refreshPsChart(const QMap<QString, double> &info)
{
    for(int i = 0; i < info.value("cpu count"); ++i)
    {
        qDebug() << QString("cpu%1").arg(i)
                 << info.value(QString("cpu%1.usage").arg(i));
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
