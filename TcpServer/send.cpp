// send.cpp: implementation of the CTCPCustom class.
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
#include <time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "send.h"
#include "../TcpServer/tcpserver.h"
#include "../maintask.h"

extern int flag_save;

// 构造函数
CTCPCustom::CTCPCustom()
{
   m_SocketEnable = 0;
   m_ExitThreadFlag = 0;
}

// 析构函数
CTCPCustom::~CTCPCustom()
{
   //关闭线程退出事件
}

/*--------------------------------------------------------------------
【函数介绍】: 此线程用于监听与客户端连接的socket通讯的事件，例如当接收到数据、
			   连接断开和通讯过程发生错误等事件
【入口参数】: lparam:无类型指针，可以通过此参数，向线程中传入需要用到的资源。
			   在这里我们将CTCPCustom类实例指针传进来
【出口参数】:  (无)
【返回  值】:   无
---------------------------------------------------------------------*/
void* CTCPCustom::SocketDataThread(void* lparam)
{
	CTCPCustom *pSocket;

	struct msg  Msg3;
	char TxBuf[4000];
	unsigned int TxCounter = 0;
	// 得到CTCPCustom类实例指针
	pSocket = (CTCPCustom*)lparam;

	// 定义读事件集合
	fd_set fdRead;
	int ret;
	struct timeval	aTime;
	aTime.tv_sec = 2;
	aTime.tv_usec = 0;

	Msg3.mytype = 1;         //?===================

	pSocket->m_pTCPServer->m_ClientEnable = 1;
	printf("SocketDataThread created\n");

    while( 1 ){
        // 收到退出事件，结束线程
        if( pSocket->m_pTCPServer->m_ExitClientThread ){
			pSocket->m_pTCPServer->m_ExitClientThread = 0;
			close( pSocket->m_socketfd );
			break;
		}

		// 置空读事件集合
		FD_ZERO(&fdRead);
		// 给pSocket设置读事件
		FD_SET(pSocket->m_socketfd, &fdRead );
		// 调用select函数，判断是否有读事件发生
		aTime.tv_sec = 0;
		aTime.tv_usec = 0;
		ret = select(pSocket->m_socketfd + 1, &fdRead, NULL, NULL, &aTime);
		//printf("CTCPCustom select ret = %d\n", ret);

        if( ret < 0 ){
			// 关闭socket
			close( pSocket->m_socketfd );
			break;
        }else{
			// 判断是否读事件
            if(FD_ISSET(pSocket->m_socketfd, &fdRead)){
				pSocket->RecvLen = recv(pSocket->m_socketfd, pSocket->RecvBuf, 1024, 0);
                if (pSocket->RecvLen < 0 ){
					// 与客户端端连接的Socket错误
					printf( "Socket error\n");
					// 关闭socket
					close(pSocket->m_socketfd);
					break;
                }else if( pSocket->RecvLen == 0){// 表示连接已经从容关闭
					printf( "Close socket %d\n", pSocket->m_socketfd );
					// 关闭socket
					close(pSocket->m_socketfd);
					break;
                }else{
					// 对接收的数据进行处理，这里为简单处理数据回发
					pSocket->RecvBuf[pSocket->RecvLen] = 0;
					printf( "RCV:%s\n", pSocket->RecvBuf);

					send(pSocket->m_socketfd, pSocket->RecvBuf, pSocket->RecvLen, 0);
				}
			}
		}

        if(flag_save){
            msgrcv(msqid2, &Msg3, sizeof(Msg3), 1, 0);         // 阻塞读取

            if(TxCounter < 100){
				memcpy(TxBuf + TxCounter * 2 * DATAPIPE, Msg3.AD, 2 * DATAPIPE);
				TxCounter++;
				//printf("ctr1 = %d\n", TxCounter);
			}
            if(TxCounter >= 100){
				TxCounter = 0;
				send(pSocket->m_socketfd, TxBuf, 2 * DATAPIPE * 100, MSG_NOSIGNAL);
			}
        }else{
			TxCounter = 0;
		}
	}

	pSocket->m_pTCPServer->m_ClientEnable = 0;
	// 释放TCPCustom指针
	pSocket->m_pTCPServer->RemoveClientSocketObject( (void*)pSocket );

	printf( "SocketDataThread finished\n");
	pthread_exit( NULL );
//	// 释放TCPCustom指针
//	pSocket->m_pTCPServer->RemoveClientSocketObject( (void*)pSocket );
//	pSocket->m_pTCPServer->m_ClientEnable = 0;
}

/*--------------------------------------------------------------------
【函数介绍】: 打开socket，创建通讯线程
【入口参数】:  pTCPServer指向服务器端监听socket
【出口参数】:  (无)
【返回  值】:  1:打开成功;0:打开失败
---------------------------------------------------------------------*/
int CTCPCustom::Open(void* lparam)
{
	int					res, flags;
	pthread_attr_t 		attr;
	struct sched_param schedule_param;

	m_pTCPServer = (CTCPServer*)lparam;
	// 创建通讯线程, 把侦听服务器类指针带入线程
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

	res = pthread_create( &m_thread, &attr, SocketDataThread, this );
    if( res!=0 ){
		close( m_socketfd );
		return 0;
	}

	pthread_attr_destroy( &attr );

	// 设置通讯模式为异步模式
	flags = fcntl(m_socketfd, F_GETFL);
	//if( (flags < 0) || ((fcntl(m_socketfd, F_SETFL, flags | O_ASYNC)) < 0) )     ///////
    if( (flags < 0) || ((fcntl(m_socketfd, F_SETFL, flags | O_NONBLOCK)) < 0) ){
		perror("fcntl");
	}

	m_SocketEnable = 1;
	return 1;
}

/*--------------------------------------------------------------------
【函数介绍】: 关闭socket，关闭线程，释放Socket资源
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】:  1:成功关闭;0:关闭失败
---------------------------------------------------------------------*/
int CTCPCustom::Close()
{
	m_ExitThreadFlag = 1;

   // 关闭Socket，释放资源
   int err = close(m_socketfd);
   if( err < 0 ){
	   return 0;
   }
   return 1;
}
