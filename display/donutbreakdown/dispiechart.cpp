#include "dispiechart.h"
#include "donutbreakdownchart.h"

#include <QDebug>

QT_CHARTS_USE_NAMESPACE

DisPieChart::DisPieChart(QObject *parent) : QObject(parent), psChart(new DonutBreakdownChart())
  ,memChart(new DonutBreakdownChart())
{

}
QChartView *DisPieChart::createPsChart()
{
    psUsed = new QPieSeries();
    Q_CHECK_PTR(psUsed);
    psUsed->setName(tr("used"));
    psUsed->append("user", 0.1);
    psUsed->append("nice", 0.1);
    psUsed->append("system", 0.1);
    psUsed->append("iowait", 0.1);
    psUsed->append("irq", 0.1);
    psUsed->append("softirq", 0.1);
//    psUsed->append("steal", 0.1);
//    psUsed->append("guest", 0.1);
//    psUsed->append("guest_nice", 0.1);

    psFree = new QPieSeries();
    Q_CHECK_PTR(psFree);
    psFree->setName(tr("idle"));
    psFree->append("idle", 0.1);
    //animation will take many cpu usage
//    psChart->setAnimationOptions(QChart::AllAnimations);
    psChart->setTitle(tr("process overview"));
    psChart->legend()->setAlignment(Qt::AlignRight);
    psChart->addBreakdownSeries(psUsed, Qt::red);
    psChart->addBreakdownSeries(psFree, Qt::darkBlue);

    QChartView *chartView = new QChartView(psChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}
QChartView *DisPieChart::createMemChart()
{
    memUsed = new QPieSeries();
    Q_CHECK_PTR(memUsed);
    memUsed->setName(tr("using"));
    memUsed->append("used", 0.1);
    memUsed->append("buffers", 0.1);
    memUsed->append("cache", 0.1);

    memFree = new QPieSeries();
    Q_CHECK_PTR(memFree);
    memFree->setName(tr("free"));
    memFree->append("free", 0.1);
    //animation will take many cpu usage
    memChart->setTitle(tr("memory overview"));
    memChart->legend()->setAlignment(Qt::AlignRight);
    memChart->addBreakdownSeries(memUsed, Qt::red);
    memChart->addBreakdownSeries(memFree, Qt::darkBlue);

    QChartView *chartView = new QChartView(memChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}
void  DisPieChart::refreshPsChart(const QMap<QString, double> &info)
{
    auto slices = psUsed->slices();
    for (QPieSlice *slice : slices)
    {
        if(slice->label() == "user")
        {
            slice->setValue(info.value("cpu.user"));
        }
        else if(slice->label() == "nice")
        {
            slice->setValue(info.value("cpu.nice"));
        }
        else if(slice->label() == "system")
        {
            slice->setValue(info.value("cpu.system"));
        }
        else if(slice->label() == "iowait")
        {
            slice->setValue(info.value("cpu.iowait"));
        }
        else if(slice->label() == "irq")
        {
            slice->setValue(info.value("cpu.irq"));
        }
        else if(slice->label() == "softirq")
        {
            slice->setValue(info.value("cpu.softirq"));
        }
//        else if(slice->label() == "steal")
//        {
//            slice->setValue(info.value("cpu.steal"));
//        }
//        else if(slice->label() == "guest")
//        {
//            slice->setValue(info.value("cpu.guest"));
//        }
//        else if(slice->label() == "guest_nice")
//        {
//            slice->setValue(info.value("cpu.guest_nice"));
//        }
    }

    slices = psFree->slices();
    for (QPieSlice *slice : slices)
    {
        if(slice->label() == "idle")
        {
            slice->setValue(info.value("cpu.idle"));
        }
    }

    psChart->refresh();
}
void  DisPieChart::refreshMemChart(const QMap<QString, qulonglong> &info)
{
    auto slices = memUsed->slices();
    for (QPieSlice *slice : slices)
    {
        if(slice->label() == "used")
        {
            slice->setValue(info.value("mem.used"));
        }
        else if(slice->label() == "buffers")
        {
            slice->setValue(info.value("mem.buffers"));
        }
        else if(slice->label() == "cache")
        {
            slice->setValue(info.value("mem.cache"));
        }
    }

    slices = memFree->slices();
    for (QPieSlice *slice : slices)
    {
        if(slice->label() == "free")
        {
            slice->setValue(info.value("mem.free"));
        }
    }

    memChart->refresh();
}
