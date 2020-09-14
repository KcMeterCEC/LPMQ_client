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
/**
 * @brief TargetIo::execIoStat
 * @param ret
 * @note
 *   1 - major number
     2 - minor mumber
     3 - device name
     4 - reads completed successfully
      + This is the total number of reads completed successfully
     5 - reads merged
      + Reads and writes which are adjacent to each other may be merged for
      + efficiency.  Thus two 4K reads may become one 8K read before it is
      + ultimately handed to the disk, and so it will be counted (and queued)
      + as only one I/O.  This field lets you know how often this was done.
     6 - sectors read
      + This is the total number of sectors read successfully.
     7 - time spent reading (ms)
      + This is the total number of milliseconds spent by all reads
     8 - writes completed
      + same as fieid 4
     9 - writes merged
      + same as fieid 5
    10 - sectors written
      + same as fieid 6
    11 - time spent writing (ms)
      + same as fieid 7
    12 - I/Os currently in progress
      + The only field that should go to zero. Incremented as requests are
      + given to appropriate struct request_queue and decremented as they finish.
    13 - time spent doing I/Os (ms)
      + This field increases so long as field 12 is nonzero.
    14 - weighted time spent doing I/Os (ms)
      + This field is incremented at each I/O start, I/O completion, I/O
      + merge, or read of these stats by the number of I/Os in progress
      + (field 12) times the number of milliseconds spent doing I/O since the
      + last update of this field.  This can provide an easy measure of both
      + I/O completion time and the backlog that may be accumulating.

    [Kernel 4.18+ appends four more fields for discard tracking putting the total at 18]:

    15 - discards completed successfully
      + same as fieid 4
    16 - discards merged
      + same as fieid 5
    17 - sectors discarded
      + same as fieid 6
    18 - time spent discarding
      + same as fieid 7

    [Kernel 5.5+ appends two more fields for flush requests]:

    19 - flush requests completed successfully
    20 - time spent flushing
 */
void TargetIo::execIoStat(const QString &ret)
{
    const char *rPrefix = " read";
    const char *wPrefix = " write";
    QMap<QString, double> stat;

    //! 1. get line
    QStringList list = ret.split(QRegExp("[\n]"));
    list.removeAll("");
    //! 2. name filter
    for(auto &l : list)
    {
        if(partFilter(l))
        {
            QStringList ioList = l.simplified().split(' ');

            if(statBk.contains(QString(ioList[2] + rPrefix)))
            {
                stat[ioList[2] + rPrefix] = static_cast<double>
                        ((ioList[5].toInt() - statBk.value(ioList[2] + rPrefix))) / 2048;
                stat[ioList[2] + wPrefix] = static_cast<double>
                        ((ioList[9].toInt() - statBk.value(ioList[2] + wPrefix))) / 2048;
            }
            else
            {
                stat[ioList[2] + rPrefix] = 0;
                stat[ioList[2] + wPrefix] = 0;
            }

            statBk[ioList[2] + rPrefix] = ioList[5].toInt();
            statBk[ioList[2] + wPrefix] = ioList[9].toInt();
        }
    }

    if(!statBk.isEmpty())
    {
        emit resultIoUsage(stat);
    }
}
bool TargetIo::partFilter(const QString &str)
{
    bool ret = false;
    if(str.contains("hd") || str.contains("sd"))
    {
        int idx = str.indexOf('d');
        if(str[idx + 2].isSpace())
        {
            ret = true;
        }
    }
    else if(str.contains("mmcblk"))
    {
        int idx = str.indexOf('k');
        if(str[idx + 2].isSpace())
        {
            ret = true;
        }
    }

    return ret;
}
