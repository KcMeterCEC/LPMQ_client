#ifndef MEM_H
#define MEM_H

#include <QObject>
#include <QMap>
#include <QString>

typedef enum
{
    MEM_STAT,
}mem_cmd;
class Mem : public QObject
{
    Q_OBJECT
public:
    explicit Mem(QObject *parent = nullptr);

    quint8 requestMemStat(char *buf);

    void  execMemCmd(const char *buf);
private:
    void  execMemStat(const QString &ret);

    QMap<QString, double> statBk;
signals:
    void  resultMemUsage(const QMap<QString, qulonglong> &info);
};

#endif // MEM_H
