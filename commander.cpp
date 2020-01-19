#include "commander.h"
#include "rb.h"

#include <QtNetwork/QTcpSocket>
#include <QDebug>

Commander::Commander(QObject *parent) : QObject(parent),status(GET_HEAD)
{
    socket = new QTcpSocket(this);
    Q_CHECK_PTR(socket);

    /**
     * It took me a lot of time,oh my god!
     */
//    connect(socket, SIGNAL(readyRead()), this, SLOT(recvSlot));
    connect(socket, &QIODevice::readyRead, this, &Commander::recvSlot);

    connect(socket, SIGNAL(connected()), this, SLOT(connectSlot()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(errorSlot(QAbstractSocket::SocketError)));

    head.ck = HEAD_CK;

    recvBuf = new Rb(sizeof(socketBuf));

}
Commander::~Commander()
{
    delete recvBuf;
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

    emit connectStatus(hasConnected, "");
}
void    Commander::errorSlot(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err)

    qDebug() << "connection succeeded!" << socket->errorString();

    hasConnected = false;

    emit connectStatus(hasConnected, socket->errorString());
}
void    Commander::recvSlot(void)
{
    QByteArray recv = socket->readAll();

    if((unsigned int)recv.size() >= sizeof(socketBuf))
    {
        qFatal("received size of data is out of range!");
    }
    recvBuf->write(recv.constData(), recv.size());

    while(1)
    {
        if(status == GET_HEAD)
        {
            if(recvBuf->read((char *)&head, sizeof(head)))
            {
                if(head.ck == HEAD_CK)
                {
                    status = GET_CONTENTS;
                }
                else
                {
                    qFatal("header check failed!");
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            if(recvBuf->read(socketBuf, head.payload_len))
            {
                status = GET_HEAD;
                switch(head.cmd)
                {
                case CLASS_INFO:
                {
                    qDebug() << "get CLASS_INFO";
                    execSysInfo(socketBuf);
                }break;
                case CLASS_PS:
                {
                    qDebug() << "get CLASS_PS";
                }break;
                case CLASS_MEM:
                {
                    qDebug() << "get CLASS_MEM";
                }break;
                case CLASS_IO:
                {
                    qDebug() << "get CLASS_IO";
                }break;
                default:qFatal("unknown command!");break;
                }
            }
            else
            {
                break;
            }
        }
    }

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
void    Commander::execSysInfo(const char *buf)
{
    emit resultSysInfo(buf);
}
