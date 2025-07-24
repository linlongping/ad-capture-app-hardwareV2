#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QTime>
#include <thread>

/**
 * @file tcpserver.h
 * @brief TCP服务器类定义
 * @details 基于Qt框架实现的TCP服务器，支持客户端连接管理、数据接收和定时发送功能，
 *          用于处理设备数据的网络传输，维护客户端连接列表并提供数据缓冲区管理
 */

/**
 * @class TcpServer
 * @brief TCP服务器核心类
 * @details 继承自QObject，封装QTcpServer功能，实现客户端连接监听、数据收发及连接状态管理，
 *          提供数据添加接口用于缓存待发送数据，并通过定时事件实现批量数据发送
 */
class TcpServer : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @details 初始化TCP服务器实例，创建QTcpServer对象并设置信号槽关联，启动服务器监听
     * @param[in] parent 父对象指针，用于Qt对象树管理
     */
    explicit TcpServer(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     * @details 释放服务器资源，关闭所有客户端连接并销毁QTcpServer对象
     */
    ~TcpServer();

    /**
     * @brief 启用/禁用数据发送功能
     * @details 控制定时发送数据的开关状态
     * @param[in] send 发送开关状态：true启用发送，false禁用发送
     */
    void enable_send(bool send);

    /**
     * @brief 添加数据到发送缓冲区
     * @details 将待发送的16位整数数据存入缓冲区，用于定时批量发送，超过缓冲区长度时截断
     * @param[in] data 待添加的16位整数数据数组
     * @param[in] count 数据元素数量
     * @return 操作结果：0表示成功
     */
    int add_data(int16_t *data,int count);

protected:
    /**
     * @brief 定时事件处理函数
     * @details 重写QObject的定时事件，用于周期性检查发送缓冲区并向客户端发送数据
     * @param[in] event 定时事件对象
     */
    void timerEvent(QTimerEvent *);

    int16_t m_send_buff[4000] = {0};  ///< 发送缓冲区，最多存储4000个16位整数
    int m_send_data_len=0;            ///< 当前缓冲区中数据的字节数

private slots:
    /**
     * @brief 新客户端连接处理槽函数
     * @details 当有新客户端连接时触发，接受连接并将客户端socket添加到连接列表
     */
    void onNewConnection();

    /**
     * @brief 数据接收处理槽函数
     * @details 当客户端发送数据时触发，读取并处理接收的数据
     */
    void onReadyRead();

    /**
     * @brief 客户端断开连接处理槽函数
     * @details 当客户端断开连接时触发，从连接列表中移除对应的socket并释放资源
     */
    void onDisconnected();

private:
    QTcpServer *server;               ///< TCP服务器对象，用于监听客户端连接
    QList<QTcpSocket *> clients;      ///< 客户端连接列表，存储所有当前连接的客户端socket
    bool m_send=false;                ///< 发送使能标志：true表示允许发送数据，false表示禁止
};

#endif // TCPSERVER_H
