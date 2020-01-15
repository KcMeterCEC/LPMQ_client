#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>

class QTcpSocket;

class Commander : public QObject
{
    Q_OBJECT
public:
    explicit    Commander(QObject *parent = nullptr);

    void    connect2Server(const QString &ip, quint16 port);

private:
    QTcpSocket          *socket = nullptr;
    bool                hasConnected = false;
private slots:
    void    connectSlot(void);
    void    errorSlot(QAbstractSocket::SocketError err);
    void    recvSlot(void);
signals:
    void    connectStatus(bool status);
};

#endif // COMMANDER_H
