#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <pthread.h>


class CTCPServer
{
private:
    pthread_t 	m_thread;      						// 通讯线程标识符ID
    // Socket侦听线程
    static int SocketListenThread( void*lparam );
public:
    int			m_sockfd;							// TCP服务监听socket
    int			m_ExitThreadFlag;
    int			m_LocalPort;						// 设置服务端口号

    int         m_ClientEnable;
//////
    int         m_ExitClientThread;
    //pthread_t 	m_Clientthread;

    CTCPServer();
    virtual ~CTCPServer();
    int Open();				    					// 打开TCP服务
    int Close();									// 关闭TCP服务
    // 删除一个客户端对象连接 释放资源
    int RemoveClientSocketObject( void* lparam );
};

#endif
