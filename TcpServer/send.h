// send.h: interface for the CTCPCustom class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _SEND_H
#define _SEND_H

#include "tcpserver.h"

/**
 * @file send.h
 * @brief TCP�ͻ���ͨ���ඨ��
 * @details ����CTCPCustom�࣬��װTCP�ͻ��˵����ӹ��������շ����߳̿��ƹ��ܣ�
 *          ��ΪTCP�������Ŀͻ��˴���Ԫ�������뵥���ͻ��˵ľ���ͨ���߼�
 */

/**
 * @class CTCPCustom
 * @brief TCP�ͻ���ͨ�Ŵ�����
 * @details ά���뵥���ͻ��˵�TCP���ӣ�ͨ�������̴߳��������շ���
 *          �ṩ���ӽ������رռ����ݽ����Ľӿڣ�����CTCPServer�������ӹ���
 */
class CTCPCustom
{
public:
    /**
     * @brief ���캯��
     * @details ��ʼ��TCP�ͻ���ͨ��ʵ��������Ĭ������״̬���̱߳�־
     */
    CTCPCustom();

    /**
     * @brief ��������
     * @details ����ͻ���ͨ����Դ��ȷ���̺߳�socket��ȷ�ر�
     */
    virtual ~CTCPCustom();

public:
    /**
     * @brief Զ������IP��ַ
     * @details �洢���ӵĿͻ���IP��ַ�ַ�������󳤶�100�ֽ�
     */
    char  m_RemoteHost[100];

    /**
     * @brief Զ�������˿ں�
     * @details �洢���ӵĿͻ��˶˿ں�
     */
    int   m_RemotePort;

    /**
     * @brief ͨ��socket������
     * @details ������ͻ��˽������ݴ����socket�ļ�������
     */
    int	  m_socketfd;

    /**
     * @brief socketʹ�ܱ�־
     * @details ��ʶ��ǰsocket�����Ƿ���Ч����0��ʾ�����ѽ���
     */
    int   m_SocketEnable;

    /**
     * @brief �߳��˳���־
     * @details ���ڿ���ͨ���̵߳��˳�״̬����0ֵ�����߳��˳�
     */
    int   m_ExitThreadFlag;

    /**
     * @brief ����TCP������ָ��
     * @details ָ�����ǰ�ͻ������ӵ�CTCPServerʵ��
     */
    CTCPServer*  m_pTCPServer;

private:
    /**
     * @brief ͨ���̱߳�ʶ��
     * @details ���ڱ�ʶ�ͻ������ݴ����̵߳�POSIX�߳�ID
     */
    pthread_t 	m_thread;

    /**
     * @brief  socket���ݴ����߳�
     * @details ��̬��Ա��������Ϊ�ͻ���ͨ���̵߳���ڵ㣬���������շ�������ά��
     * @param[in] lparam �̲߳�����ָ��CTCPCustom��ʵ����ָ��
     * @return �߳��˳�״̬��ͨ��ΪNULL��
     */
    static void* SocketDataThread(void* lparam);

public:
    /**
     * @brief �������ݳ���
     * @details ��¼���һ�ν��յ������ֽ���
     */
    int  RecvLen;

    /**
     * @brief �������ݻ�����
     * @details �洢���յĿͻ������ݣ��������1500�ֽ�
     */
    char RecvBuf[1500];

    /**
     * @brief �򿪿ͻ���ͨ��
     * @details ��ʼ��ͨ���̣߳�����socketΪ������ģʽ����ʼ���ݽ���
     * @param[in] lparam ָ������CTCPServerʵ����ָ��
     * @return ���������1��ʾ�ɹ���0��ʾʧ��
     */
    int Open(void* lparam);

    /**
     * @brief �رտͻ���ͨ��
     * @details �����߳��˳���־���ر�socket���ͷ������Դ
     * @return ���������1��ʾ�ɹ���0��ʾʧ��
     */
    int Close();
};

#endif
