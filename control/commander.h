#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QtNetwork/QAbstractSocket>

class QTcpSocket;
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
private:
    explicit Commander(QObject *parent = nullptr);
    ~Commander();

    bool        isConnected = false;
    QTcpSocket  *socket = nullptr;
signals:
    void connectChanged(bool isCon, const QString &str);
private slots:
    void recvData(void);
    void hasConnected(void);
    void hasErr(QAbstractSocket::SocketError err);
};

#endif // COMMANDER_H
