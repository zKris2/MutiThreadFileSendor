#include "client.h"

#include <QFile>
#include <QFileInfo>

Client::Client() {
    _tcp_client = new QTcpSocket();
}

Client::~Client() {
    _tcp_client->disconnectFromHost();
    _tcp_client->close();
    if(_tcp_client!=nullptr){
        delete _tcp_client;
        _tcp_client = nullptr;
    }
}

bool Client::setAddressPort(QString address,quint16 port){

    if (_tcp_client->state() != QAbstractSocket::ConnectedState) {
        _tcp_client->connectToHost(QHostAddress(address), port);
    }

    if (!_tcp_client->waitForConnected(1000)) {
        return false;
    }
    return true;
}
QString Client::getTcpError(){
    return _tcp_client->errorString();
}

void Client::sendBytes(QByteArray data){
    _tcp_client->write(data);
}
void Client::sendBytes(QByteArray data,int size){
    _tcp_client->write(data,size);
}

