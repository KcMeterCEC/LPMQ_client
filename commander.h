#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>
#include <Qmap>

class QTcpSocket;
class Rb;
class Process;
class Mem;

#define HEAD_CK (0xA5)

typedef enum
{
    CLASS_INFO,
    CLASS_PS,
    CLASS_MEM,
    CLASS_IO,
}cmd_class;

typedef enum
{
    CMD_OK,
    CMD_NO_FILE,
    CMD_FILE_READ_ERR,
}cmd_status;

typedef enum
{
    FOCUS_CPU,
    FOCUS_MEM,
    FOCUS_IO,
}task_list_focus;

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct Header
{
    std::uint8_t        ck = HEAD_CK;
    std::uint8_t        cmd = CLASS_INFO;
    std::int16_t        status = CMD_OK;
    std::uint16_t       payload_len = 0;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((__packed__));
#endif

class Commander : public QObject
{
    Q_OBJECT
public:
    explicit    Commander(QObject *parent = nullptr);
    ~Commander();

    void    connect2Server(const QString &ip, quint16 port);
    void    disconnect2Server(void);
    bool    isConnect() const;

    void    requestSysInfo(void);

    void    requestCpuUsage(void);
    void    requestTaskList(task_list_focus focus, quint16 number);

    void    requestMemUsage(void);
private:
    QTcpSocket          *socket = nullptr;
    bool                hasConnected = false;
    Header              head;
    char                socketBuf[40960];
    char                sendBuf[1024];
    Rb                  *recvBuf;
    enum
    {
        GET_HEAD,
        GET_CONTENTS,
    }status;

    Process             *ps;
    Mem                 *memory;

    bool    send2Server(Header &sendHead);

    void    execSysInfo(const char *buf);
private slots:
    void    connectSlot(void);
    void    errorSlot(QAbstractSocket::SocketError err);
    void    recvSlot(void);
signals:
    void    connectStatus(bool status, const QString & errStr);

    void    resultSysInfo(const QMap<QString, QString> &info);
    void    psResultCpuUsage(const QMap<QString, double> &info);
    void    psResultTaskList(const QVector<QMap<QString, QString>> &info);

    void    memResultMemUsage(const QMap<QString, qulonglong> &info);
};

#endif // COMMANDER_H
