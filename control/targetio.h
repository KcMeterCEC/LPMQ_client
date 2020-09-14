#ifndef TARGETIO_H
#define TARGETIO_H

#include <QObject>
#include <QMap>

class TargetIo : public QObject
{
    Q_OBJECT
public:
    static TargetIo *getInstance(void)
    {
        static TargetIo instance;

        return &instance;
    }

    void execIoCmd(const char *buf);

    quint8 requestIoStat(char *buf);
private:
    explicit TargetIo(QObject *parent = nullptr);
    ~TargetIo();

    typedef enum
    {
        IO_STAT,
    }ioCmd;

    QMap<QString, int> statBk;

    void execIoStat(const QString &ret);
    bool partFilter(const QString &str);
signals:
    void resultIoUsage(const QMap<QString, double> &info);
};

#endif // TARGETIO_H
