#include "widget.h"
#include "ui_widget.h"
#include "util.h"

#include <QMessageBox>
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    _server = QSharedPointer<Server>(new Server());
    _client = QSharedPointer<Client>(new Client());

    connect(_client->_tcp_client, &QTcpSocket::connected, this, [&]() {
        QDateTime currentTime = QDateTime::currentDateTime();
        QString currentTimeString = currentTime.toString();
        qDebug() << "TcpSocket: Connected to server success\t" << currentTimeString;
    });
    connect(_client->_tcp_client, &QTcpSocket::errorOccurred, this, [&]() {
        QDateTime currentTime = QDateTime::currentDateTime();
        QString currentTimeString = currentTime.toString();
        qDebug() << "Error: " << _client->_tcp_client->errorString() << "\t" << currentTimeString;
    });

    connect(_server->_tcp_server, &QTcpServer::newConnection, this, &Widget::onNewConnection);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_send_file_clicked()
{
    ui->progressBar_send->setValue(0);
    ui->lineEdit_send_status->setText("");

    QString filename = ui->lineEdit_send_file->text();
    if(filename==""){
        QMessageBox::information(this, "Information", "请先选择要发送的文件！");
        return;
    }

    QString address = ui->lineEdit_server_ip->text();
    quint16 port = ui->lineEdit_server_port->text().toUShort();

    if(!_client->setAddressPort(address,port)){
        QMessageBox::information(this, "Information", _client->getTcpError());
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Open file error:" << file.errorString();
        QMessageBox::information(this, "Information", "打开文件失败");
        return;
    }

    QByteArray buffer;
    buffer.resize(file.size());

    // 读取文件内容
    QFileInfo fileinfo(filename);
    int filesize = fileinfo.size();

    // 发送文件名
    QString send_filename = Util::getFilename(filename);
    qDebug()<<"发送端解析文件名称:"<<send_filename;
    send_filename.append('\0');
    _client->sendBytes(send_filename.toUtf8());

    _client->sendBytes((char*)&filesize, 4); //用4个字节表示文件大小

    int curSize = 0;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        _client->sendBytes(line); //向套接字中写入数据
        curSize += line.size();
        int progressValue = (curSize/filesize)*100; //进度值
        ui->progressBar_send->setValue(progressValue);
        if(progressValue==100){
            ui->lineEdit_send_status->setText("发送文件["+filename+"]成功!");
        }
    }
}

void Widget::on_pushButton_select_file_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("open a file."),
        "D:/",
        tr("All files(*.*);;images(*.png *jpeg *bmp *.jpg);;video files(*.avi *.mp4 *.wmv);;files(*.txt;;*.doxc)")
    );
    if (filepath.isEmpty()) {
        return;
    }
    ui->lineEdit_send_file->setText(filepath);
}


void Widget::on_pushButton_start_listen_clicked()
{
    quint16 port = ui->lineEdit_listen_port->text().toUShort();

    if(!_server->startListen(port)){
        QMessageBox::information(this, "Information",_server->getTcpError());
        return;
    }
    QMessageBox::information(this, "Information","start listen port "+QString::number(port));
}

void Widget::onNewConnection(){
    QTcpSocket* client = _server->onNewConnection();
    connect(client, &QTcpSocket::readyRead, this, std::bind(&Widget::recvFile, this, client));
}

void Widget::recvFile(QTcpSocket* client){
    ui->progressBar_recv->setValue(0);
    ui->lineEdit_recv_status->setText("");

    int receivedSize = _server->recvData(client,ui->lineEdit_save_dir->text());

    int progressValue = (receivedSize * 100) / _server->_filesize;
    ui->progressBar_recv->setValue(progressValue);

    if(progressValue==100){
        ui->lineEdit_recv_status->setText("接受文件["+_server->_filename+"]成功!");
        _server->_filename="";
        _server-> _filesize=0;
        _server-> _recved_filesize=0;
        _server->_recv_data=nullptr;
    }
}



void Widget::on_pushButton_save_dir_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(
        nullptr,
        "选择保存目录",
        "C:/",
        QFileDialog::ShowDirsOnly
    );

    // 检查用户是否选择了目录
    if (directory.isEmpty()) {
        // 用户没有选择任何目录
        QMessageBox::information(this, "提示", "未选择保存目录，默认D盘");
        ui->lineEdit_save_dir->setText("D:/");
    } else {
        // 用户选择了目录，可以在这里进行进一步的操作
        QMessageBox::information(this, "提示", "选择的保存目录为：" + directory);
        ui->lineEdit_save_dir->setText(directory);
    }
}

