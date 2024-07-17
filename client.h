#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include <QSharedPointer>

class Client
{
public:
    using ptr = QSharedPointer<Client>;
    QTcpSocket* _tcp_client;
    Client();
    ~Client();

    bool setAddressPort(QString address,quint16 port);
    QString getTcpError();

    void sendBytes(QByteArray data);
    void sendBytes(QByteArray data,int size);

private:
    QString _ip;
    quint16 _port;
};

#endif // CLIENT_H
