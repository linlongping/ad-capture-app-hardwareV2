#include "tcpserver.h"
#include <thread>
#include <QDebug>

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
        // 创建一个QTcpServer对象
        server = new QTcpServer(this);

        // 当有新的连接时，发出newConnection信号
        connect(server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
        // 监听指定端口，这里使用1001
        if (!server->listen(QHostAddress::Any, 1001)) {
        //if (!server->listen(QHostAddress::Any, 8888)) {
            qDebug() << "Server could not start!";
        } else {
            qDebug() << "Server started!";
        }
        startTimer(5);//5ms
}
TcpServer::~TcpServer()
{
    server->close();
    qDeleteAll(clients);
}
void TcpServer::enable_send(bool send)
{
   m_send = send;
}
int TcpServer::add_data(int16_t *data, int count)
{
    memcpy((char*)m_send_buff+m_send_data_len,data,count*sizeof(int16_t));
    m_send_data_len += count*sizeof(int16_t);
    if(m_send_data_len>3800){
       m_send_data_len = 3800;
    }
    return 0;
}

void TcpServer::timerEvent(QTimerEvent *)
{
    if(m_send_data_len == 0){
        return ;
    }
    if(!m_send) return;//dont send

    for(auto client:clients){
       client->write((char*)m_send_buff,m_send_data_len);
    }
    m_send_data_len = 0;
}

void TcpServer::onNewConnection()
{
    // 当有新的连接时，接受它
    QTcpSocket *socket = server->nextPendingConnection();
    // 将新的socket添加到clients列表中
    clients.append(socket);
    // 当有数据可读时，调用onReadyRead
    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);

    // 当连接断开时，调用onDisconnected
    connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);
    qDebug() << "New client connected." << socket->peerAddress().toString();
}
void TcpServer::onReadyRead()
{
    // 获取发出信号的socket
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket && socket->canReadLine()) {
        // 读取一行数据
        QByteArray line = socket->readLine();
        qDebug() << "Received:" << line;
        // 发送回应
        socket->write("Message received: " + line);
        socket->flush();
    }
}

void TcpServer::onDisconnected()
{
    // 获取断开的socket
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        qDebug() << "Client disconnected." << socket->peerAddress().toString();
        // 从clients列表中移除断开的socket
        clients.removeAll(socket);
        // 删除socket
        socket->deleteLater();
    }
}
