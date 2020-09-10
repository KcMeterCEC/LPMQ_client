#include <QtNetwork/QTcpSocket>
#include <QNetworkProxyFactory>
#include <QDebug>

#include "commander.h"
#include "rb.h"
#include "targetps.h"
#include "targetmem.h"

Commander::Commander(QObject *parent) : QObject(parent),
    socket(new QTcpSocket(this)),
    recvBuf(new Rb(sizeof(socketBuf)))
{
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    connect(socket, &QTcpSocket::readyRead, this, &Commander::recvData);
    connect(socket, SIGNAL(connected()), this, SLOT(hasConnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(hasErr(QAbstractSocket::SocketError)));

    ps = TargetPs::getInstance();
    mem = TargetMem::getInstance();
}
Commander::~Commander()
{
    delete socket;
    delete recvBuf;
}
void Commander::recvData(void)
{
    QByteArray recv = socket->readAll();
    if(recv.size() >= static_cast<int>(sizeof(socketBuf)))
    {
        qFatal("Received size of data is out of range: %d\n", recv.size());
    }
    recvBuf->write(recv.constData(), recv.size());

    while(1)
    {
        if(headStatus == GET_HEAD)
        {
            if(recvBuf->read((char *)&head, sizeof(head)))
            {
                if(head.ck == HEAD_CK)
                {
                    if(head.status == CMD_OK)
                    {
                        headStatus = GET_CONTENTS;
                    }
                    else
                    {
                        qDebug() << "something is wrong : " << head.status;
                    }
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
                socketBuf[head.payload_len] = '\0';
                headStatus = GET_HEAD;
                switch(head.cmd)
                {
                case CLASS_INFO:
                {
                    execSysInfo(socketBuf);
                }break;
                case CLASS_PS:
                {
                    ps->execCpuCmd(socketBuf);
                }break;
                case CLASS_MEM:
                {
                    mem->execMemCmd(socketBuf);
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
void Commander::execSysInfo(const char *buf)
{
    const QString &result = buf;
    QStringList list = result.split(QRegExp("[\t\n:]"));
    list.removeAll("");

    QMap<QString, QString> cpuInfo;
    const QString checklist[] =
    {
      "model name", "cpu MHz", "cache size", "bogomips", "clflush size",
      "cache_alignment", "address sizes",
    };

    quint8 coreNum = 1;
    for(auto v = list.cbegin(); v != list.cend(); ++v)
    {
        for(auto item : checklist)
        {
            if(*v == item)
            {
                cpuInfo[item] = *(v + 1);
                break;
            }
        }

        if(*v == "processor")
        {
            cpuInfo["processor"] = QString("%1").arg(coreNum);
            ++coreNum;
        }
    }

    emit resultSysInfo(cpuInfo);
}
void Commander::requestSysInfo(void)
{
    if(!isConnected) return;

    Header sendHead;

    sendHead.cmd = CLASS_INFO;
    sendHead.payload_len = 0;
    send2Server(sendHead);
}
void Commander::requestCpuUsage(void)
{
    if(!isConnected) return;

    Header sendHead;

    sendHead.cmd = CLASS_PS;
    sendHead.payload_len = ps->requestCpuStat(sendBuf);
    send2Server(sendHead);
}
void Commander::requestMemUsage(void)
{
    if(!isConnected) return;
    Header sendHead;

    sendHead.cmd = CLASS_MEM;
    sendHead.payload_len = mem->requestMemStat(sendBuf);
    send2Server(sendHead);
}
bool Commander::send2Server(Header &sendHead)
{
    if(!isConnected)
    {
        qCritical() << "need connect to server first!";
        return false;
    }

    if(socket->write((const char *)&sendHead, sizeof(Header)) == -1)
    {
        qCritical() << "can't write data to server!";
        return false;
    }
    if(sendHead.payload_len)
    {
        if(socket->write(sendBuf, sendHead.payload_len) == -1)
        {
            qCritical() << "can't write data to server!";
            return false;
        }
    }

    return true;
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
