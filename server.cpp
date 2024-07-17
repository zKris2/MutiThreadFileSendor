#include "server.h"
#include "util.h"

#include <QMessageBox>
#include <QDateTime>
#include <QTcpSocket>

Server::Server() {
    _tcp_server = new QTcpServer();
    _filename="";
    _filesize=0;
    _recved_filesize=0;
    _recv_data = nullptr;
}

Server::~Server() {
    _tcp_client->close();
    if(_tcp_client!=nullptr){
        delete _tcp_client;
        _tcp_client=nullptr;
    }
    _tcp_server->close();
    if(_tcp_server!=nullptr){
        delete _tcp_server;
        _tcp_server = nullptr;
    }
}

bool Server::startListen(quint16 port){
    if(_tcp_server->isListening()){
        return false;
    }
    if (!_tcp_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server listen port error:" << _tcp_server->errorString();
        return false;
    } else {
        qDebug() << "Server start listen port"<< port << Qt::endl;
        return true;
    }
}
QString Server::getTcpError(){
    return _tcp_server->errorString();
}

quint64 Server::recvData(QTcpSocket* client,QString directory){
    QByteArray buffer = client->readAll();
    // 如果我们还没有接收到文件大小，先读取MIME类型和文件大小
    if (_filesize == 0) {
        int nullPos = buffer.indexOf('\0');
        if (nullPos != -1) {
            // 保存FILENAME
            _filename = buffer.left(nullPos);
            //qDebug()<<"接收端文件名称:"<<_filename;
            buffer.remove(0, nullPos + 1);
            // 现在buffer应该包含文件大小信息
            if (buffer.size() >= 4) {
                // 从buffer中读取文件大小
                memcpy(&_filesize, buffer.constData(), 4);
                // 移除已读取的文件大小信息
                buffer.remove(0, 4);
                //qDebug()<<"接收端文件大小:"<<_filesize;
            }
        }
    }
    _recv_data.append(buffer);
    _recved_filesize += buffer.size();
    // 检查是否已经接收了所有数据
    if (_recved_filesize >= _filesize) {
        _filename = directory + _filename;
        qDebug()<<"保存文件目录:"<<_filename;
        qDebug()<<"文件大小:"<<_recv_data.size();
        QPair<bool,QString> result = Util::saveByteArray2File(_recv_data,_filename);
        if(!result.first){
            qDebug()<<"save file failed: "<<result.second;
        }
    }
    return _recved_filesize;
}

QTcpSocket* Server::onNewConnection(){
    QTcpSocket* client = _tcp_server->nextPendingConnection();

    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentTimeString = currentTime.toString();
    qDebug()<< "TcpServer: New connection\t" << currentTimeString;
    _tcp_client = client;
    return client;
}
