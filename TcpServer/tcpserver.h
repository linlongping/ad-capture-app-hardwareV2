#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <pthread.h>

/**
 * @file tcpserver.h
 * @brief TCP服务器类定义
 * @details 基于POSIX线程库实现的TCP服务器框架，负责监听客户端连接、管理客户端对象
 *          及处理线程生命周期，提供基础的TCP服务端功能接口
 */

/**
 * @class CTCPServer
 * @brief TCP服务器核心类
 * @details 封装TCP服务器的监听、连接管理及线程控制功能，通过独立线程处理客户端连接请求，
 *          维护客户端连接状态并提供资源释放机制
 */
class CTCPServer
{
private:
    /**
     * @brief 通讯线程标识符
     * @details 用于标识服务器监听线程的POSIX线程ID，用于线程管理和控制
     */
    pthread_t 	m_thread;

    /**
     * @brief Socket监听线程函数
     * @details 静态成员函数，作为服务器监听线程的入口点，负责接受客户端连接请求
     * @param[in] lparam 线程参数，指向CTCPServer类实例的指针
     * @return 线程退出状态码（通常为0）
     */
    static int SocketListenThread( void*lparam );

public:
    /**
     * @brief TCP服务监听socket描述符
     * @details 用于绑定服务器端口并监听客户端连接的socket文件描述符
     */
    int			m_sockfd;

    /**
     * @brief 线程退出标志
     * @details 用于控制服务器监听线程的退出状态：非0值表示触发线程退出
     */
    int			m_ExitThreadFlag;

    /**
     * @brief 本地服务端口号
     * @details 服务器绑定的本地端口号，用于客户端连接
     */
    int			m_LocalPort;

    /**
     * @brief 客户端使能标志
     * @details 标识当前是否有客户端连接：非0值表示存在活跃客户端连接
     */
    int         m_ClientEnable;

    /**
     * @brief 客户端线程退出标志
     * @details 用于控制客户端处理线程的退出状态：非0值表示触发客户端线程退出
     */
    int         m_ExitClientThread;

    /**
     * @brief 构造函数
     * @details 初始化TCP服务器实例，设置默认端口和线程状态标志
     */
    CTCPServer();

    /**
     * @brief 析构函数
     * @details 清理TCP服务器资源，确保所有线程和socket正确关闭
     */
    virtual ~CTCPServer();

    /**
     * @brief 打开TCP服务
     * @details 初始化监听socket，绑定端口并启动监听线程，开始接受客户端连接
     * @return 操作结果：0表示成功，非0表示失败
     */
    int Open();

    /**
     * @brief 关闭TCP服务
     * @details 设置线程退出标志，关闭监听socket，终止所有相关线程
     * @return 操作结果：0表示成功，非0表示失败
     */
    int Close();

    /**
     * @brief 移除客户端socket对象
     * @details 释放指定客户端连接的资源，关闭客户端socket并清理相关数据
     * @param[in] lparam 指向客户端对象的指针
     * @return 操作结果：0表示成功，非0表示失败
     */
    int RemoveClientSocketObject( void* lparam );
};

#endif
