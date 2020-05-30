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

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskId
{
    uint32_t uid;
    uint32_t gid;
    int32_t  pid;
    int32_t  ppid;
    int32_t  pgid;
    int32_t  sid;
    int32_t  tpgid;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskFault
{
    uint32_t minflt;
    uint32_t cminflt;
    uint32_t majflt;
    uint32_t cmajflt;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskTime
{
    uint32_t utime;
    uint32_t stime;
    int32_t  cutime;
    int32_t  cstime;
    uint64_t starttime;
    uint32_t guest_time;
    int32_t  cguest_time;
    int64_t  ticket;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskMem
{
    uint32_t vsize;
    int32_t  rss;
    uint32_t start_data;
    uint32_t end_data;
    uint32_t start_brk;
    uint32_t arg_start;
    uint32_t arg_end;
    uint32_t env_start;
    uint32_t env_end;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskPolicy
{
    int32_t  processor;
    uint32_t rt_priority;
    uint32_t policy;
    int32_t  priority;
    int32_t  nice;
    int32_t  threads;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskStat
{
    char state;
    int32_t  tty_nr;
    uint32_t flags;
    int32_t  exit_code;
    uint32_t wchan;
    uint32_t exit_signal;
    uint64_t delayacct_blkio_ticks;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct TaskOverview
{
    char                name[50];
    struct TaskId       id;
    struct TaskFault    fault;
    struct TaskTime     time;
    struct TaskMem      mem;
    struct TaskPolicy   policy;
    struct TaskStat     stat;
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
    void    execCpuList(const char *buf);

    QMap<QString, double> statBk;
    bool isCpuStatMore = false;
signals:
    void    resultCpuUsage(const QMap<QString, double> &info);
    void    resultTaskList(const QVector<QMap<QString, QString>> &info);
};

#endif // PROCESS_H
