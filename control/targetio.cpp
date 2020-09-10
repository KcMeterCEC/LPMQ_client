#include <QDebug>

#include "targetio.h"

TargetIo::TargetIo(QObject *parent) : QObject(parent)
{

}
TargetIo::~TargetIo()
{

}
void TargetIo::execIoCmd(const char *buf)
{
    switch(buf[0])
    {
    case IO_STAT:
    {
        execIoStat(buf + 1);
    }break;
    default:
    {
        qFatal("unknown IO command!");
    }break;
    }
}
quint8 TargetIo::requestIoStat(char *buf)
{
    buf[0] = IO_STAT;

    return 1;
}
void TargetIo::execIoStat(const QString &ret)
{
    QMap<QString, double> stat;

    emit resultIoUsage(stat);
}
