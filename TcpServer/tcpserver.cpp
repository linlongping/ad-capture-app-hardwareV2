// tcp_server.cpp: implementation of the CTCPServer class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "tcpserver.h"
#include "send.h"

#define  MAX_CONN_NUM		10               // =======================

//构造函数
CTCPServer::CTCPServer()
{
    m_ExitThreadFlag = 0;
    m_ExitClientThread = 0;
    m_ClientEnable = 0;
}

//析构函数
CTCPServer::~CTCPServer()
{

}

/*--------------------------------------------------------------------
【函数介绍】:  此线程用于监听套接字事件。
【入口参数】:  lparam:无类型指针，可以通过此参数，向线程中传入需要用到的资源。
               在这里我们将CTCPServer类实例指针传进来
【出口参数】:  (无)
【返回  值】:  返回值没有特别的意义，在此我们将返回值设为0。
---------------------------------------------------------------------*/
int CTCPServer::SocketListenThread(void* lparam)
{
    CTCPServer *pSocket;
    // 得到CTCPServer实例指针
    pSocket = (CTCPServer*)lparam;
    // 定义读事件集合
    fd_set fdRead;
    int ret;
    struct timeval	aTime;

    while( 1 ){
        //收到退出事件，结束线程
        if( pSocket->m_ExitThreadFlag ){
            break;
        }

        FD_ZERO(&fdRead);
        FD_SET(pSocket->m_sockfd, &fdRead);

        aTime.tv_sec = 2;
        aTime.tv_usec = 0;
        ret = select( MAX_CONN_NUM, &fdRead, NULL, NULL, &aTime );
        //printf( "select ret = %d\n", ret);

        if (ret < 0 ){
            // 关闭服务器套接字
            close(pSocket->m_sockfd );
            break;
        }
        if (ret > 0){
            // 判断是否读事件
            if (FD_ISSET(pSocket->m_sockfd, &fdRead)){
                ////// 如有发送线程存在，发送线程退出信号发出
                if(pSocket->m_ClientEnable == 1){
                    pSocket->m_ExitClientThread = 1;
                    ////// 等待发送线程退出，回收资源
                    sleep(1);
                }

                struct sockaddr_in clientAddr;
                CTCPCustom* pClientSocket = new CTCPCustom( );
                int namelen = sizeof(clientAddr);
                // 等待，创建与客户端连接的套接字
                pClientSocket->m_socketfd = accept(pSocket->m_sockfd, (struct sockaddr *)&clientAddr, (socklen_t*)&namelen );
                // 接收到客户端连接
                if( pClientSocket->m_socketfd ){
                    strcpy ( pClientSocket->m_RemoteHost, inet_ntoa(clientAddr.sin_addr) );
                    pClientSocket->m_RemotePort = ntohs(clientAddr.sin_port);

                    // 打开pClientSocket服务线程, 把本侦听类的指针传给客户端连接类
                    pClientSocket->Open( pSocket );
                }else{
                    delete  pClientSocket;
                }
            }
        }
    }

    printf( "SocketLisenThread finished\n");
    pthread_exit( NULL );
    return 0;
}

/*--------------------------------------------------------------------
【函数介绍】:  打开TCP服务
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】:  <=0:打开TCP服务失败; =1:打开TCP服务成功
---------------------------------------------------------------------*/
int CTCPServer::Open()
{
    struct sockaddr_in 	server_sockaddr;
    pthread_attr_t 		attr;
    int					res, flags;

    // 1.创建套接字
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_sockfd == -1){
        printf("socket fail!\n");
        return -1;
    }
    printf("socket sucess!, sockfd=%d\n", m_sockfd);
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(m_LocalPort);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(server_sockaddr.sin_zero), 8);
    if((bind(m_sockfd, (struct sockaddr*)&server_sockaddr, sizeof(sockaddr))) == -1){
        printf("bind fail!\n");
        return -2;
    }
    printf("bind sucess\n");

    // 2.设置为侦听套接字，可同时响应MAX_CONN_NUM个客户的连接请求
    if(listen(m_sockfd, MAX_CONN_NUM) != 0){
        return -3;
    }

    // 3.设置监听套接字通讯模式为异步模式
    flags = fcntl(m_sockfd, F_GETFL);
    if( (flags < 0) || ((fcntl(m_sockfd, F_SETFL, flags | O_ASYNC)) < 0) ){
        perror("fcntl");
    }

    // 4.创建侦听通讯线程，在线程里，等待客户端连接请求
    res = pthread_attr_init(&attr);
    if( res!=0 ){
        printf("Create attribute failed\n" );
    }
    // 设置线程绑定属性
    res = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
    // 设置线程分离属性
    res += pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    if( res!=0 ){
        printf( "Setting attribute failed\n" );
    }

    res = pthread_create( &m_thread, &attr, (void *(*) (void *))&SocketListenThread, (void*)this );
    if( res!=0 ){
        close(m_sockfd );
        return -1;
    }

    pthread_attr_destroy( &attr );
    return 1;
}


/*--------------------------------------------------------------------
【函数介绍】:  关闭TCP服务
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】:  <=0:关闭TCP服务失败; =1:关闭TCP服务成功
---------------------------------------------------------------------*/
int CTCPServer::Close()
{
    m_ExitThreadFlag = 1;

    int err = close(m_sockfd);
    if( err < 0 ){
        return -1;
    }
    return 1;
}

int CTCPServer::RemoveClientSocketObject( void* lparam )
{
    CTCPCustom* pSocket = (CTCPCustom*)lparam;
    delete pSocket;
    return 0;
}

