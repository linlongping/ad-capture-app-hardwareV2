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

// ���캯��
CTCPCustom::CTCPCustom()
{
   m_SocketEnable = 0;
   m_ExitThreadFlag = 0;
}

// ��������
CTCPCustom::~CTCPCustom()
{
   //�ر��߳��˳��¼�
}

/*--------------------------------------------------------------------
���������ܡ�: ���߳����ڼ�����ͻ������ӵ�socketͨѶ���¼������統���յ����ݡ�
			   ���ӶϿ���ͨѶ���̷���������¼�
����ڲ�����: lparam:������ָ�룬����ͨ���˲��������߳��д�����Ҫ�õ�����Դ��
			   ���������ǽ�CTCPCustom��ʵ��ָ�봫����
�����ڲ�����:  (��)
������  ֵ��:   ��
---------------------------------------------------------------------*/
void* CTCPCustom::SocketDataThread(void* lparam)
{
	CTCPCustom *pSocket;

	struct msg  Msg3;
	char TxBuf[4000];
	unsigned int TxCounter = 0;
	// �õ�CTCPCustom��ʵ��ָ��
	pSocket = (CTCPCustom*)lparam;

	// ������¼�����
	fd_set fdRead;
	int ret;
	struct timeval	aTime;
	aTime.tv_sec = 2;
	aTime.tv_usec = 0;

	Msg3.mytype = 1;         //?===================

	pSocket->m_pTCPServer->m_ClientEnable = 1;
	printf("SocketDataThread created\n");

    while( 1 ){
        // �յ��˳��¼��������߳�
        if( pSocket->m_pTCPServer->m_ExitClientThread ){
			pSocket->m_pTCPServer->m_ExitClientThread = 0;
			close( pSocket->m_socketfd );
			break;
		}

		// �ÿն��¼�����
		FD_ZERO(&fdRead);
		// ��pSocket���ö��¼�
		FD_SET(pSocket->m_socketfd, &fdRead );
		// ����select�������ж��Ƿ��ж��¼�����
		aTime.tv_sec = 0;
		aTime.tv_usec = 0;
		ret = select(pSocket->m_socketfd + 1, &fdRead, NULL, NULL, &aTime);
		//printf("CTCPCustom select ret = %d\n", ret);

        if( ret < 0 ){
			// �ر�socket
			close( pSocket->m_socketfd );
			break;
        }else{
			// �ж��Ƿ���¼�
            if(FD_ISSET(pSocket->m_socketfd, &fdRead)){
				pSocket->RecvLen = recv(pSocket->m_socketfd, pSocket->RecvBuf, 1024, 0);
                if (pSocket->RecvLen < 0 ){
					// ��ͻ��˶����ӵ�Socket����
					printf( "Socket error\n");
					// �ر�socket
					close(pSocket->m_socketfd);
					break;
                }else if( pSocket->RecvLen == 0){// ��ʾ�����Ѿ����ݹر�
					printf( "Close socket %d\n", pSocket->m_socketfd );
					// �ر�socket
					close(pSocket->m_socketfd);
					break;
                }else{
					// �Խ��յ����ݽ��д�������Ϊ�򵥴������ݻط�
					pSocket->RecvBuf[pSocket->RecvLen] = 0;
					printf( "RCV:%s\n", pSocket->RecvBuf);

					send(pSocket->m_socketfd, pSocket->RecvBuf, pSocket->RecvLen, 0);
				}
			}
		}

        if(flag_save){
            msgrcv(msqid2, &Msg3, sizeof(Msg3), 1, 0);         // ������ȡ

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
	// �ͷ�TCPCustomָ��
	pSocket->m_pTCPServer->RemoveClientSocketObject( (void*)pSocket );

	printf( "SocketDataThread finished\n");
	pthread_exit( NULL );
//	// �ͷ�TCPCustomָ��
//	pSocket->m_pTCPServer->RemoveClientSocketObject( (void*)pSocket );
//	pSocket->m_pTCPServer->m_ClientEnable = 0;
}

/*--------------------------------------------------------------------
���������ܡ�: ��socket������ͨѶ�߳�
����ڲ�����:  pTCPServerָ��������˼���socket
�����ڲ�����:  (��)
������  ֵ��:  1:�򿪳ɹ�;0:��ʧ��
---------------------------------------------------------------------*/
int CTCPCustom::Open(void* lparam)
{
	int					res, flags;
	pthread_attr_t 		attr;
	struct sched_param schedule_param;

	m_pTCPServer = (CTCPServer*)lparam;
	// ����ͨѶ�߳�, ��������������ָ������߳�
	res = pthread_attr_init(&attr);
    if( res!=0 ){
		printf("Create attribute failed\n" );
	}
	// �����̰߳�����
	res = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
	// �����̷߳�������
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

	// ����ͨѶģʽΪ�첽ģʽ
	flags = fcntl(m_socketfd, F_GETFL);
	//if( (flags < 0) || ((fcntl(m_socketfd, F_SETFL, flags | O_ASYNC)) < 0) )     ///////
    if( (flags < 0) || ((fcntl(m_socketfd, F_SETFL, flags | O_NONBLOCK)) < 0) ){
		perror("fcntl");
	}

	m_SocketEnable = 1;
	return 1;
}

/*--------------------------------------------------------------------
���������ܡ�: �ر�socket���ر��̣߳��ͷ�Socket��Դ
����ڲ�����:  (��)
�����ڲ�����:  (��)
������  ֵ��:  1:�ɹ��ر�;0:�ر�ʧ��
---------------------------------------------------------------------*/
int CTCPCustom::Close()
{
	m_ExitThreadFlag = 1;

   // �ر�Socket���ͷ���Դ
   int err = close(m_socketfd);
   if( err < 0 ){
	   return 0;
   }
   return 1;
}
