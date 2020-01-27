#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QMap>
#include <QString>

typedef enum
{
    CPU_STAT,
}ps_cmd;
class Process : public QObject
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr);

    quint8  requestCpuStat(char *buf);

    void    execCpuCmd(const char *buf);
private:
    void    execCpuStat(const QString &ret);

    QMap<QString, double> statBk;
    bool isCpuStatMore = false;
signals:
    void    resultCpuUsage(const QMap<QString, double> &info);

};

#endif // PROCESS_H
