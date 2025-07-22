#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QTime>
#include <thread>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();
    void enable_send(bool send);
    //添加数据,定时发送出去
    int add_data(int16_t *data,int count);
protected:
    void timerEvent(QTimerEvent *);

    int16_t m_send_buff[4000] = {0};
    int m_send_data_len=0;//数据size
private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
private:
    QTcpServer *server;
    QList<QTcpSocket *> clients;
    bool m_send=false;

};

#endif // TCPSERVER_H
