// send.h: interface for the CTCPCustom class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _SEND_H
#define _SEND_H

#include "tcpserver.h"

/**
 * @file send.h
 * @brief TCP客户端通信类定义
 * @details 定义CTCPCustom类，封装TCP客户端的连接管理、数据收发及线程控制功能，
 *          作为TCP服务器的客户端处理单元，负责与单个客户端的具体通信逻辑
 */

/**
 * @class CTCPCustom
 * @brief TCP客户端通信处理类
 * @details 维护与单个客户端的TCP连接，通过独立线程处理数据收发，
 *          提供连接建立、关闭及数据交互的接口，依赖CTCPServer进行连接管理
 */
class CTCPCustom
{
public:
    /**
     * @brief 构造函数
     * @details 初始化TCP客户端通信实例，设置默认连接状态和线程标志
     */
    CTCPCustom();

    /**
     * @brief 析构函数
     * @details 清理客户端通信资源，确保线程和socket正确关闭
     */
    virtual ~CTCPCustom();

public:
    /**
     * @brief 远程主机IP地址
     * @details 存储连接的客户端IP地址字符串，最大长度100字节
     */
    char  m_RemoteHost[100];

    /**
     * @brief 远程主机端口号
     * @details 存储连接的客户端端口号
     */
    int   m_RemotePort;

    /**
     * @brief 通信socket描述符
     * @details 用于与客户端进行数据传输的socket文件描述符
     */
    int	  m_socketfd;

    /**
     * @brief socket使能标志
     * @details 标识当前socket连接是否有效：非0表示连接已建立
     */
    int   m_SocketEnable;

    /**
     * @brief 线程退出标志
     * @details 用于控制通信线程的退出状态：非0值触发线程退出
     */
    int   m_ExitThreadFlag;

    /**
     * @brief 所属TCP服务器指针
     * @details 指向管理当前客户端连接的CTCPServer实例
     */
    CTCPServer*  m_pTCPServer;

private:
    /**
     * @brief 通信线程标识符
     * @details 用于标识客户端数据处理线程的POSIX线程ID
     */
    pthread_t 	m_thread;

    /**
     * @brief  socket数据处理线程
     * @details 静态成员函数，作为客户端通信线程的入口点，负责数据收发和连接维护
     * @param[in] lparam 线程参数，指向CTCPCustom类实例的指针
     * @return 线程退出状态（通常为NULL）
     */
    static void* SocketDataThread(void* lparam);

public:
    /**
     * @brief 接收数据长度
     * @details 记录最近一次接收的数据字节数
     */
    int  RecvLen;

    /**
     * @brief 接收数据缓冲区
     * @details 存储接收的客户端数据，最大容量1500字节
     */
    char RecvBuf[1500];

    /**
     * @brief 打开客户端通信
     * @details 初始化通信线程，设置socket为非阻塞模式，开始数据交互
     * @param[in] lparam 指向所属CTCPServer实例的指针
     * @return 操作结果：1表示成功，0表示失败
     */
    int Open(void* lparam);

    /**
     * @brief 关闭客户端通信
     * @details 设置线程退出标志，关闭socket并释放相关资源
     * @return 操作结果：1表示成功，0表示失败
     */
    int Close();
};

#endif
