#ifndef TARGETMEM_H
#define TARGETMEM_H

#include <QObject>

class TargetMem : public QObject
{
    Q_OBJECT
public:
    static TargetMem *getInstance(void)
    {
        static TargetMem instance;

        return &instance;
    }

    void execMemCmd(const char *buf);

    quint8 requestMemStat(char *buf);
private:
    explicit TargetMem(QObject *parent = nullptr);
    ~TargetMem();

    typedef enum
    {
        MEM_STAT,
    }memCmd;

    void  execMemStat(const QString &ret);
signals:
    void  resultMemUsage(const QMap<QString, double> &info);
};

#endif // TARGETMEM_H
