#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>

#define HEAD_CK (0xA5)

class QTcpSocket;
class Rb;
class TargetPs;
class TargetMem;
class TargetIo;
class Commander : public QObject
{
    Q_OBJECT
public:
    static Commander *getInstance(void)
    {
        static Commander instance;

        return &instance;
    }

    bool connectStatus(void) const;
    void connect2Server(const QString &ip, quint16 port);
    void disconnectFromServer(void);

    void requestSysInfo(void);

    void requestCpuUsage(void);

    void requestMemUsage(void);

    void requestIoUsage(void);
private:
    explicit Commander(QObject *parent = nullptr);
    ~Commander();

    typedef enum
    {
        CLASS_INFO,
        CLASS_PS,
        CLASS_MEM,
        CLASS_IO,
    }cmdClass;
    typedef enum
    {
        CMD_OK,
        CMD_NO_FILE,
        CMD_FILE_READ_ERR,
    }cmdStatus;
    typedef enum
    {
        GET_HEAD,
        GET_CONTENTS,
    }cmdHeadStatus;
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

    Header          head;
    cmdHeadStatus   headStatus = GET_HEAD;
    bool            isConnected = false;
    QTcpSocket      *socket = nullptr;
    Rb              *recvBuf = nullptr;
    char            socketBuf[40960];
    char            sendBuf[1024];
    TargetPs        *ps;
    TargetMem       *mem;
    TargetIo        *io;

    bool send2Server(Header &sendHead);
    void execSysInfo(const char *buf);
signals:
    void connectChanged(bool isCon, const QString &str);

    void resultSysInfo(const QMap<QString, QString> &info);
private slots:
    void recvData(void);
    void hasConnected(void);
    void hasErr(QAbstractSocket::SocketError err);
};

#endif // COMMANDER_H
