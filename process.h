#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QMap>
#include <QString>

typedef enum
{
    CPU_STAT,
    TASK_LIST,
}ps_cmd;

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskListHeader
{
    std::uint16_t focus;
    std::uint16_t number;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

class Process : public QObject
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr);

    quint8  requestCpuStat(char *buf);
    quint8  requestTaskList(char *buf, quint16 fcs, quint16 num);

    void    execCpuCmd(const char *buf);
private:
    void    execCpuStat(const QString &ret);

    QMap<QString, double> statBk;
    bool isCpuStatMore = false;
signals:
    void    resultCpuUsage(const QMap<QString, double> &info);

};

#endif // PROCESS_H
