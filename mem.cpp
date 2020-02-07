#include "mem.h"
#include <QDebug>

Mem::Mem(QObject *parent) : QObject(parent)
{

}
quint8 Mem::requestMemStat(char *buf)
{
    buf[0] = MEM_STAT;

    return 1;
}
void Mem::execMemStat(const QString &ret)
{
    QStringList list = ret.split(QRegExp("(kB){0,1}[:\n]"));
    list.removeAll("");

    qDebug() << "mem stat return:";

    QMap<QString, qulonglong> stat;
    for(auto v = list.cbegin(); v!= list.cend(); ++v)
    {
        //Total installed memory (MemTotal and SwapTotal in /proc/meminfo)
        if(*v == "MemTotal")
        {
            stat["mem.total"] = (*(v + 1)).toULongLong();
        }
        else if(*v == "SwapTotal")
        {
//            stat["mem.total"] += (*(v + 1)).toULongLong();
        }
        //Unused memory (MemFree and SwapFree in /proc/meminfo)
        else if(*v == "MemFree")
        {
            stat["mem.free"] = (*(v + 1)).toULongLong();
        }
        else if(*v == "SwapFree")
        {
//            stat["mem.free"] += (*(v + 1)).toULongLong();
        }
        //Memory used by kernel buffers (Buffers in /proc/meminfo)
        else if(*v == "Buffers")
        {
            stat["mem.buffers"] = (*(v + 1)).toULongLong();
        }
        //Memory  used  by the page cache and slabs (Cached and SReclaimable in /proc/mem‐info)
        else if(*v == "Cached")
        {
            stat["mem.cache"] = (*(v + 1)).toULongLong();
        }
        else if(*v == "SReclaimable")
        {
            stat["mem.cache"] += (*(v + 1)).toULongLong();
        }
    }
    //Used memory (calculated as total - free - buffers - cache)
    stat["mem.used"] = stat["mem.total"] - stat["mem.free"] - stat["mem.buffers"] - stat["mem.cache"];

    emit resultMemUsage(stat);
}
void Mem::execMemCmd(const char *buf)
{
    switch(buf[0])
    {
    case MEM_STAT:
    {
        execMemStat(buf + 1);
    }break;
    default:
    {
        qFatal("unknown MEM command!");
    }break;
    }
}
