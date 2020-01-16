#include "commander.h"

#include <QtNetwork/QTcpSocket>
#include <QDebug>

Commander::Commander(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    Q_CHECK_PTR(socket);

    connect(socket, SIGNAL(readyRead()), this, SLOT(recvSlot));
    connect(socket, SIGNAL(connected()), this, SLOT(connectSlot()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(errorSlot(QAbstractSocket::SocketError)));

    head.ck = HEAD_CK;
}
void    Commander::connect2Server(const QString &ip, quint16 port)
{
    qDebug() << "connecting to server, ip " << ip << " port " << port;

    socket->connectToHost(ip, port);

}
void    Commander::disconnect2Server(void)
{
    qDebug() << "disconnecting to server.";

    if(hasConnected)
    {
        socket->disconnectFromHost();
    }

    hasConnected = false;
}
void    Commander::connectSlot(void)
{
    qDebug() << "connection succeeded!";

    hasConnected = true;

    emit connectStatus(hasConnected);
}
void    Commander::errorSlot(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err)

    qDebug() << "connection succeeded!" << socket->errorString();

    hasConnected = false;

    emit connectStatus(hasConnected);
}
void    Commander::recvSlot(void)
{

}
bool    Commander::isConnect() const
{
    return hasConnected;
}

void    Commander::requestSysInfo(void)
{
    head.cmd = CLASS_INFO;
    head.payload_len = 0;
    send2Server();
}
bool    Commander::send2Server(void)
{
    if(!hasConnected)
    {
        qCritical() << "need connect to server first!";
        return false;
    }

    if(socket->write((const char *)&head, sizeof(Header)) == -1)
    {
        qCritical() << "can't write data to server!";
        return false;
    }
    if(head.payload_len)
    {
        if(socket->write(socketBuf, head.payload_len) == -1)
        {
            qCritical() << "can't write data to server!";
            return false;
        }
    }

    return true;
}
