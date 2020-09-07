#include <QtNetwork/QTcpSocket>

#include "commander.h"

Commander::Commander(QObject *parent) : QObject(parent),
    socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::readyRead, this, &Commander::recvData);
    connect(socket, SIGNAL(connected()), this, SLOT(hasConnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(hasErr(QAbstractSocket::SocketError)));
}
Commander::~Commander()
{
    delete socket;
}
void Commander::recvData(void)
{

}
void Commander::hasConnected(void)
{
    isConnected = true;
    emit connectChanged(isConnected, "");
}
void Commander::hasErr(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);

    disconnectFromServer();
    emit connectChanged(isConnected, socket->errorString());
}
bool Commander::connectStatus(void) const
{
    return isConnected;
}
void Commander::connect2Server(const QString &ip, quint16 port)
{
    socket->connectToHost(ip, port);
}
void Commander::disconnectFromServer(void)
{
    if(isConnected)
    {
        isConnected = false;
        socket->disconnectFromHost();
    }
}
