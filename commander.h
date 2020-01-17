#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>

class QTcpSocket;

#define HEAD_CK (0xA5)

typedef enum
{
    CLASS_INFO,
    CLASS_PS,
    CLASS_MEM,
    CLASS_IO,
}cmd_class;

struct Header
{
    std::uint8_t        ck;
    std::uint8_t        cmd;
    std::uint16_t       payload_len;
}__attribute__((__packed__));

class Commander : public QObject
{
    Q_OBJECT
public:
    explicit    Commander(QObject *parent = nullptr);

    void    connect2Server(const QString &ip, quint16 port);
    void    disconnect2Server(void);
    bool    isConnect() const;

    void    requestSysInfo(void);
private:
    QTcpSocket          *socket = nullptr;
    bool                hasConnected = false;
    Header              head;
    char                socketBuf[1024];

    bool    send2Server(void);
private slots:
    void    connectSlot(void);
    void    errorSlot(QAbstractSocket::SocketError err);
    void    recvSlot(void);
signals:
    void    connectStatus(bool status, const QString & errStr);
};

#endif // COMMANDER_H
