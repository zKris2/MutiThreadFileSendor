#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QSharedPointer>

const quint16 PORT = 6928;

class Server
{
public:
    using ptr = QSharedPointer<Server>;
    Server();
    ~Server();

    bool startListen(quint16 port);
    QString getTcpError();

    QTcpSocket* onNewConnection();
    quint64 recvData(QTcpSocket* client,QString directory);

public:
    QTcpServer* _tcp_server;
    QTcpSocket* _tcp_client;

    QString _filename;
    QByteArray _recv_data;
    quint64 _filesize;
    quint64 _recved_filesize;
};

#endif // SERVER_H
