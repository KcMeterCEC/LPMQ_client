#ifndef TARGETPS_H
#define TARGETPS_H

#include <QObject>
#include <QMap>

class TargetPs : public QObject
{
    Q_OBJECT
public:
    static TargetPs *getInstance(void)
    {
        static TargetPs instance;

        return &instance;
    }

    void execCpuCmd(const char *buf);

    quint8 requestCpuStat(char *buf);
private:
    explicit TargetPs(QObject *parent = nullptr);
    ~TargetPs();

    typedef enum
    {
        CPU_STAT,
        TASK_LIST,
    }psCmd;

    QMap<QString, double> statBk;

    void execCpuStat(const QString &ret);
signals:
    void resultCpuUsage(const QMap<QString, double> &info);
};

#endif // TARGETPS_H
