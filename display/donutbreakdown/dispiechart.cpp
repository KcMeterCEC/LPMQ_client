#include "dispiechart.h"
#include "donutbreakdownchart.h"

#include <QDebug>

QT_CHARTS_USE_NAMESPACE

DisPieChart::DisPieChart(QObject *parent) : QObject(parent), psChart(new DonutBreakdownChart())
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
