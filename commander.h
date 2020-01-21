#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>

class QTcpSocket;
class Rb;

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
}cmd_status;

struct Header
{
    std::uint8_t        ck;
    std::uint8_t        cmd;
    std::int16_t        status;
    std::uint16_t       payload_len;
};

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
private:
    QTcpSocket          *socket = nullptr;
    bool                hasConnected = false;
    Header              head;
    char                socketBuf[16384];
    Rb                  *recvBuf;
    enum
    {
        GET_HEAD,
        GET_CONTENTS,
    }status;

    bool    send2Server(void);

    void    execSysInfo(const char *buf);
private slots:
    void    connectSlot(void);
    void    errorSlot(QAbstractSocket::SocketError err);
    void    recvSlot(void);
signals:
    void    connectStatus(bool status, const QString & errStr);

    void    resultSysInfo(const QString &result);
};

#endif // COMMANDER_H
