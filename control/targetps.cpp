#include <QDebug>
#include <QMap>
#include <QStringList>
#include <QString>
#include "targetps.h"

TargetPs::TargetPs(QObject *parent) : QObject(parent)
{

}
TargetPs::~TargetPs()
{

}
quint8 TargetPs::requestCpuStat(char *buf)
{
    buf[0] = CPU_STAT;

    return 1;
}
void TargetPs::execCpuCmd(const char *buf)
{
    switch(buf[0])
    {
    case CPU_STAT:
    {
        execCpuStat(buf + 1);
    }break;
    case TASK_LIST:
    {
//        execCpuList(buf + 1);
    }break;
    default:
    {
        qFatal("unknown PS command!");
    }break;
    }
}
/**
 * @brief TargetPs::execCpuStat
 * @param ret : QString
 * @note <cpu....>
 * The meanings of the columns are as follows, from left to right:
 * user -> Time spent in user mode
 * nice -> Time spent in user mode with low priority(nice)
 * system -> Time spent in system mode
 * idle -> Time spent in the idle task
 * iowait -> Time waiting for I/O to complete
 * irq -> Time servicing interrupts
 * softirq -> Time servicing softirqs
 * steal -> Stolen time, which is the time spent in
 other operating systems when running in a virtualized environment
 * guest -> Time spent running a virtual CPU for guest
 operating systems under the control of the Linux kernel.
 * guest_nice -> Time spent running a niced guest
 */
void TargetPs::execCpuStat(const QString &ret)
{
    QStringList list = ret.split(QRegExp("[\n]"));
    list.removeAll("");

    QMap<QString, double> stat;
    qint8 cpuCnt = -1;

    for(auto &l : list)
    {
        // cpu usage
        QString cpuFind = cpuCnt >= 0 ?
                    QString("cpu%1").arg(cpuCnt) :
                    QString("cpu");

        if(l.contains(cpuFind))
        {
            QStringList cpuList = l.mid(cpuFind.size()).simplified().split(' ');

            double totalTime = 0;
            for(auto &c : cpuList)
            {
                totalTime += c.toInt();
            }
            if(statBk.contains(cpuFind + (".total")))
            {
                stat[cpuFind + (".user")] = (cpuList.at(0).toInt() - statBk.value(cpuFind + (".user"))) * 100 /
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".nice")] = (cpuList.at(1).toInt() - statBk.value(cpuFind + (".nice"))) * 100/
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".system")] = (cpuList.at(2).toInt() - statBk.value(cpuFind + (".system"))) * 100/
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".idle")] = (cpuList.at(3).toInt() - statBk.value(cpuFind + (".idle"))) * 100/
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".iowait")] = (cpuList.at(4).toInt() - statBk.value(cpuFind + (".iowait"))) * 100/
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".irq")] = (cpuList.at(5).toInt() - statBk.value(cpuFind + (".irq"))) * 100/
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".softirq")] = (cpuList.at(6).toInt() - statBk.value(cpuFind + (".softirq"))) * 100/
                        (totalTime - statBk.value(cpuFind + (".total")));
                stat[cpuFind + (".usage")] = 100 - stat.value(cpuFind + (".idle"));
            }
            else
            {
                stat[cpuFind + (".user")] = 0;
                stat[cpuFind + (".nice")] = 0;
                stat[cpuFind + (".system")] = 0;
                stat[cpuFind + (".idle")] = 0;
                stat[cpuFind + (".iowait")] = 0;
                stat[cpuFind + (".irq")] = 0;
                stat[cpuFind + (".softirq")] = 0;
                stat[cpuFind + (".usage")] = 0;
            }

            cpuCnt++;

            statBk[cpuFind + (".total")] = totalTime;
            statBk[cpuFind + (".user")] = cpuList.at(0).toInt();
            statBk[cpuFind + (".nice")] = cpuList.at(1).toInt();
            statBk[cpuFind + (".system")] = cpuList.at(2).toInt();
            statBk[cpuFind + (".idle")] = cpuList.at(3).toInt();
            statBk[cpuFind + (".iowait")] = cpuList.at(4).toInt();
            statBk[cpuFind + (".irq")] = cpuList.at(5).toInt();
            statBk[cpuFind + (".softirq")] = cpuList.at(6).toInt();
        }

        stat["cpu count"] = cpuCnt;
    }

    if(!statBk.isEmpty())
    {
        emit resultCpuUsage(stat);
    }
}
