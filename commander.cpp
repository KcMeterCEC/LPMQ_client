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
}
void    Commander::connect2Server(const QString &ip, quint16 port)
{
    qDebug() << "connecting to server, ip " << ip << " port " << port;

    socket->connectToHost(ip, port);

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
