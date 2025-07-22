#include "maintask.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include "utils/utils.h"
#include <string.h>

#include "TcpServer/tcpserver.h"

class CTCPServer  m_TCPServer;

#define  SERVERPORT    1001                 // TCP服务器端口号=========================

#define DEBUG 0
int fd=0;
struct msg Msg1;
struct msg Msg2;
unsigned  int state=0;
int msqid, msqid2;                          //////
int flag_save=0;
key_t key;

//void *thread_handler1(void *arg)
//{
//    int flag=0;
//    int i=0;
//    Msg1.mytype=1;
//    AD_INIT();
//    int ret=0;
//    while(1)
//    {
//        do
//        {
//            while(GPIO.GPIO_PinState3()) //ef
//            {
//                GPIO.GPIO_OutClear(GPIO8); //R
//                Msg1.AD[i]=GPIO.GPIO_PinState1();
//                GPIO.GPIO_OutSet(GPIO8);
//                if( (Msg1.AD[0]&0xFFFF) != 0xA55A)
//                {
//                    i=0;
//                    printf("Msg=%x\n",Msg1.AD[0]);
//                }
//                else
//                {
//                    i++;
//                }
//                if(i == DATAPIPE)
//                {
//                    ret = msgsnd(msqid, &Msg1, sizeof(Msg1), IPC_NOWAIT);
//                    if(ret < 0)
//                    {
//                        perror("msg is full");
//                    }

//                    if(m_TCPServer.m_ClientEnable == 1)
//                    {
//						ret = msgsnd(msqid2, &Msg1, sizeof(Msg1), IPC_NOWAIT);     // 发送给tcp通信线程
//						if(ret < 0)
//						{
//							perror("msg2 is full");
//						}
//                    }

//                    i=0;
//                }
//            }
//        }while(i!=0);

//        flag_save=!GPIO.GPIO_PinState4();
//        if(flag_save)
//        {
//            GPIO.GPIO_OutClear(GPIO14);
//            GPIO.GPIO_OutClear(GPIO10);
//        }else
//        {
//            GPIO.GPIO_OutSet(GPIO14);
//            GPIO.GPIO_OutSet(GPIO10);
//        }
//        SLEEP_TIME;
//    }
//}
void *thread_handler2(void *arg)
{
    int flag_file=0;
    signed short buf1[DATAPIPE*DATAPACK_WRITE]={0};
    Msg2.mytype=1;
    int i=0;
    struct  timeval   tv_begin,tv_end;
    int now_time,last_time;

    while(1)
    {
        gettimeofday(&tv_end,NULL);
        now_time=tv_end.tv_sec;
        if(flag_file)
        {
            if(now_time-last_time>=FILE_TO_FILE_COST_SEC)
            {
                close(fd);
              //  printf("%d\n",fd);
                flag_file=0;
            }
        }

        if(!flag_file)
        {
            gettimeofday(&tv_end,NULL);
            last_time=tv_end.tv_sec;
            get_new_fd();
            flag_file=1;
        }
        for(i = 0; i < DATAPACK_WRITE; i++)
        {
            msgrcv(msqid, &Msg2, sizeof(Msg2), 1, 0);
            memcpy(buf1 + DATAPIPE * i, Msg2.AD, 2 * DATAPIPE);
        }

        if((i == DATAPACK_WRITE) && (flag_file))
        {
            write(fd, buf1, sizeof(buf1));
        }
    }
}


void main_task()
{
    pthread_attr_t thread_attr;
    struct sched_param schedule_param;
    pthread_attr_init(&thread_attr);
    schedule_param.sched_priority=11;
    pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&thread_attr,SCHED_FIFO);
    pthread_attr_setschedparam(&thread_attr,&schedule_param);
    pthread_t thread1,thread2;
}

