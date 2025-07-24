/**
 * @file main.cpp
 * @brief 程序主入口文件，负责AD数据采集、处理、存储及网络通信功能
 * @details 该文件包含主函数及核心线程逻辑，负责初始化AD数据的采集、通过消息队列传递、存储到SD卡、TCPCANTCP服务器发送及CAN总线通信等功能
 * @author （可补充作者）
 * @date （可补充日期）
 * @version 1.0
 */

#include <iostream>
#include "fiforeader.h"
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>  //define O_WRONLY and O_RDONLY
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "global_def.h"
#include "utils/utils.h"

#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <device/tcpserver.h>
#include <thread>
#include <deque>
#include <QTimer>
#include "dataprocess/dataprocess.h"
#include "device/cantask.h"

/**
 * @def CAN0_INIT
 * @brief CAN0接口初始化命令
 * @details 设置CAN0波特率为125000并启用三重采样
 */
#define CAN0_INIT "ip link set can0 type can bitrate 125000 triple-sampling on"

/**
 * @def CAN0_UP
 * @brief 启动CAN0接口命令
 * @details 激活CAN0网络接口
 */
#define CAN0_UP "ifconfig can0 up"

/**
 * @enum ERR_CODE
 * @brief 系统错误代码枚举
 * @var ERR_CODE::ERR_OK
 *      无错误
 * @var ERR_CODE::ERR_NO_SD
 *      SD卡不存在错误
 * @var ERR_CODE::ERR_DATE_TIME
 *      日期时间错误（年份小于2024）
 */
enum ERR_CODE{
    ERR_OK=0,        /**< 无错误 */
    ERR_NO_SD=-1,    /**< SD卡不存在错误 */
    ERR_DATE_TIME=-2 /**< 日期时间错误 */
};

/**
 * @var g_err_code
 * @brief 全局错误代码变量
 * @details 用于存储系统当前的错误状态，参考ERR_CODE枚举
 */
int32_t g_err_code = ERR_OK;

/**
 * @var Msg1
 * @brief 消息队列1的数据结构实例
 * @details 用于存储和传递AD采集数据
 */
struct msg Msg1;

/**
 * @var Msg2
 * @brief 消息队列2的数据结构实例
 * @details 用于数据存储线程的数据传递
 */
struct msg Msg2;

/**
 * @var state
 * @brief 系统状态标志位
 * @details 预留用于表示系统运行状态
 */
unsigned  int state=0;

/**
 * @var msqid
 * @brief 第一个消息队列ID
 * @details 用于AD采集数据的进程间通信
 */
int msqid;

/**
 * @var msqid2
 * @brief 第二个消息队列ID
 * @details 用于TCP通信线程的数据传递
 */
int msqid2;                          //////

/**
 * @var flag_save
 * @brief 数据保存标志位
 * @details 1表示启用数据保存，0表示禁用
 */
int flag_save=1;

/**
 * @var key
 * @brief 消息队列键值
 * @details 用于创建和获取消息队列
 */
key_t key;

/**
 * @var m_pServer
 * @brief TCP服务器实例指针
 * @details 用于处理网络连接和数据发送
 */
TcpServer *m_pServer = nullptr;

/**
 * @var m_data_process
 * @brief 数据处理实例
 * @details 用于频率计算等数据处理操作
 */
DataProcess m_data_process;//calc freq

/**
 * @var g_freq_115V
 * @brief 115V abc相的频率数组
 * @details 存储计算得到的115V三相电的频率值
 */
double g_freq_115V[3];//115V abc freq

/**
 * @brief 数据采集线程处理函数
 * @details 从FIFO读取AD数据，进行数据解析、封装，发送到消息队列、TCP服务器和CAN设备
 */
void cap_thread_handler()
{
    int flag=0;
    int i=0;
    Msg1.mytype=1;

    FifoReader fifo_reader;
    fifo_reader.fifo_init();
    int ret=0;
    std::deque<std::uint16_t> bufA;
    std::deque<std::uint16_t> bufB;
    CanTask canTask;

    std::cout << "Adcapture V1.0.2"<<std::endl;

    int first_bit = 0;//io first bit
    int recv_A_count=0;
    int recv_B_count=0;
    while(1){
        uint16_t value_A,value_B;
        if(fifo_reader.get_fifo_A_value(&value_A)){
            bufA.push_back(value_A);
            if(bufA.size()>36){
                bufA.pop_front();
            }
            //   std::cout << "value A is"<< std::to_string(value_A)<<std::endl;
        }
        if(fifo_reader.get_fifo_B_value(&value_B)){
            bufB.push_back(value_B);
            if(bufB.size()>36){
                bufB.pop_front();
            }
            //  std::cout << "value B is"<< std::to_string(value_B)<<std::endl;
        }
        //find head and delete front
        if(bufA.size()>=18 && bufB.size()>=18){
            for(int i=0;i<18;i++){
                if(bufA[0] != 0xA55A){
                    bufA.pop_front();
                }
                if(bufB[0] != 0xA55A){
                    bufB.pop_front();
                }
            }
        }
        //one frame
        if(bufA.size()>=18 && bufB.size()>=18){
            Msg1.AD[0] = 0xA55A;
            Msg1.AD[1] = bufB[2];
            Msg1.AD[2] = bufB[4];
            Msg1.AD[3] = bufB[6];
            Msg1.AD[4] = bufB[8];
            Msg1.AD[5] = bufB[10];
            Msg1.AD[6] = bufB[12];
            Msg1.AD[7] = bufA[3];
            Msg1.AD[8] = bufA[1];
            Msg1.AD[9] = bufA[13];
            Msg1.AD[10] = bufA[11];
            Msg1.AD[11] = bufB[15];
            Msg1.AD[12] = bufB[13];
            Msg1.AD[13] = bufA[5];
            Msg1.AD[14] = bufB[9];
            Msg1.AD[15] = bufA[7];
            Msg1.AD[16] = bufB[11];
            Msg1.AD[17] = bufA[9];
            Msg1.AD[18] = bufA[12];
            Msg1.AD[19] = bufA[8];
            Msg1.AD[20] = bufA[10];
            Msg1.AD[21] = bufA[15];
            Msg1.AD[22] = bufA[14];
            Msg1.AD[23] = bufA[16];
            Msg1.AD[24] = bufA[4];
            Msg1.AD[25] = bufA[2];
            Msg1.AD[26] = bufA[6];
            Msg1.AD[27] = bufB[3];
            Msg1.AD[28] = bufB[1];
            Msg1.AD[29] = bufB[5];
            Msg1.AD[30] = bufB[7];
            Msg1.AD[31] = bufB[16];
            Msg1.AD[32] = bufB[14];

            //            for(int i=0;i<17;i++){
            //                Msg1.AD[i] = bufA[i];
            //            }
            //            for(int i=0;i<16;i++){
            //                Msg1.AD[i+17] = bufB[i+1];
            //            }
            Msg1.AD[33] = g_freq_115V[0]*10;
            Msg1.AD[34] = g_freq_115V[1]*10;
            Msg1.AD[35] = g_freq_115V[2]*10;
            Msg1.AD[36] = bufA[17];// bit
            first_bit = bufA[17]&0x01;
            //            for(int j=0;j<18;j++){
            //                std::cout << std::to_string(j)<<" "<< std::to_string(bufB[j])<<std::endl;
            //            }
            //           std::cout << "end bufA"<<std::endl;
            //delete front
            for(int i=0;i<18;i++){
                bufA.pop_front();
                bufB.pop_front();
            }
            //add Data
            DataUnit dataunit((int16_t*)Msg1.AD);
            //m_data_process.add_data(dataunit);
            m_data_process.add_data1(dataunit.m_data[18],dataunit.m_data[19],dataunit.m_data[20]);
            if(g_err_code != ERR_NO_SD){//no sd card
                ret = msgsnd(msqid, &Msg1, DATAPIPE*2, IPC_NOWAIT);
                if(ret < 0){
                    perror("msg is full");
                }
            }
            m_pServer->add_data((int16_t*)Msg1.AD,37);
            canTask.set_data((int16_t*)(Msg1.AD+1),DATAPIPE-1);
        }
        //empty
        if(!fifo_reader.get_single_gpio_in(&fifo_reader.m_fifo_EF_B2)||
            (!fifo_reader.get_single_gpio_in(&fifo_reader.m_fifo_ef_A2))){
            int flag_save = !fifo_reader.get_single_gpio_in(&fifo_reader.m_mcu_CMN[2]);
            //     std::cout << "flag_save"<< std::to_string(flag_save)<<std::endl;

            if(flag_save){
                fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[1],0);
                fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[4],0);
            }else{
                fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[1],1);
                fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[4],1);
            }
            if(first_bit){
                fifo_reader.set_single_gpio_out(&fifo_reader.m_save_led,0);
            }else{
                fifo_reader.set_single_gpio_out(&fifo_reader.m_save_led,1);
            }
            //set send or not send
            m_pServer->enable_send(flag_save);
            canTask.enable_send(flag_save);

            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    }
}

/**
 * @brief 数据存储线程处理函数
 * @details 从消息队列读取数据，按时间间隔分文件存储到SD卡，控制文件数量
 */
void save_thread_handler()
{
    int fd = 0;
    int flag_file=0;
    int16_t buf1[DATAPIPE*DATAPACK_WRITE]={0};
    Msg2.mytype=1;
    int i=0;
    struct  timeval   tv_begin,tv_end;
    int now_time,last_time;

    bool has_sd = QFile::exists("/dev/mmcblk1p1");
    if(!has_sd) {
        g_err_code = ERR_NO_SD;
        std::cout << "err_no_sd card"<<std::endl;
        return;
    }
    if(QDate::currentDate().year()<2024) g_err_code = ERR_DATE_TIME;

    while(true){
        gettimeofday(&tv_end,NULL);
        now_time=tv_end.tv_sec;
        if(flag_file){
            if(now_time-last_time>=FILE_TO_FILE_COST_SEC){
                close(fd);
                //  printf("%d\n",fd);
                flag_file=0;
            }
        }

        if(!flag_file){
            gettimeofday(&tv_end,NULL);
            last_time=tv_end.tv_sec;
            fd = get_new_fd();
            flag_file=1;
        }
        for(i = 0; i < DATAPACK_WRITE; i++){
            msgrcv(msqid, &Msg2, sizeof(Msg2), 1, 0);
            memcpy(buf1 + DATAPIPE * i, Msg2.AD, 2 * DATAPIPE);
        }

        if((i == DATAPACK_WRITE) && (flag_file)){
            write(fd, buf1, sizeof(buf1));
        }
    }
}

/**
 * @brief 主任务初始化函数
 * @details 初始化消息队列、启动CAN接口、创建并启动采集和存储线程
 * @return 0（成功）
 */
int main_task()
{
    printf("(*-*)/    Adcapture 1.0 60s   c(*_*) \n");
    system("sysctl -w kernel.msgmnb=6553600");
    system(CAN0_INIT);
    system(CAN0_UP);

    key_t key;
    if((key=ftok(".",'a'))<0){
        perror("ftok error");
        exit(-1);
    }
    if((msqid = msgget(key, IPC_CREAT|IPC_EXCL|0777)) < 0){
        if(errno != EEXIST){
            perror("msgget error");
            exit(-1);
        }else{
            msqid = msgget(key, 0777);
        }
    }

    if((key = ftok(".",'b')) < 0){
        perror("second ftok error");
        exit(-1);
    }
    if((msqid2 = msgget(key, IPC_CREAT|IPC_EXCL|0777)) < 0){
        if(errno != EEXIST){
            perror("second msgget error");
            exit(-1);
        }else{
            msqid2 = msgget(key, 0777);
        }
    }
    std::thread *cap_thread = new std::thread(&cap_thread_handler);
    std::thread *save_thread = new std::thread(&save_thread_handler);


    cap_thread->detach();
    save_thread->detach();

    return 0;
}

/**
 * @brief 频率计算定时函数
 * @details 每1秒调用一次，计算115V各相的频率并更新到全局变量g_freq_115V
 */
void calc_freq_func()
{
    //    m_data_process.calc_freq(18,g_freq_115V[0]);
    //    m_data_process.calc_freq(19,g_freq_115V[1]);
    //    m_data_process.calc_freq(20,g_freq_115V[2]);
    m_data_process.calc_freq1(0,g_freq_115V[0]);
    m_data_process.calc_freq1(1,g_freq_115V[1]);
    m_data_process.calc_freq1(2,g_freq_115V[2]);
    ////std::cout << "calc freq every 1s"<< std::to_string(g_freq_115V[0])<<std::endl;
}

/**
 * @brief 程序主入口函数
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 应用程序退出代码
 * @details 初始化Qt应用、TCP服务器，启动主任务和频率计算定时器，进入Qt事件循环
 */
int main(int argc, char *argv[])
{
    std::cout << "AdCapture Versionn 1.0!" << std::endl;
    QCoreApplication a(argc, argv);
    m_pServer = new TcpServer();
    main_task();

    QTimer timer;
    QObject::connect(&timer,&QTimer::timeout,&calc_freq_func);
    timer.start(1000);

    return a.exec();
    //  system("sysctl -w kernel.msgmnb=6553600");
    //------below is for test,not used----------
    FifoReader fifo_reader;
    fifo_reader.fifo_init();
    std::cout << "after init"<<std::endl;
    std::deque<std::uint16_t> bufA;
    std::deque<std::uint16_t> bufB;

    while(1){
        std::this_thread::sleep_for(std::chrono::microseconds(50));

        uint16_t value_A,value_B;
        if(fifo_reader.get_fifo_A_value(&value_A)){
            bufA.push_back(value_A);
            if(bufA.size()>36){
                bufA.pop_front();
            }
            //    std::cout << "value A is"<< std::to_string(value_A)<<std::endl;
        }
        if(fifo_reader.get_fifo_B_value(&value_B)){
            bufB.push_back(value_B);
            if(bufB.size()>36){
                bufB.pop_front();
            }
            //     std::cout << "value B is"<< std::to_string(value_B)<<std::endl;
        }
        //find head
        if(bufA.size()>=36 && bufB.size()>=36){
            for(int i=0;i<18;i++){
                if(bufA[0] != 0xA55A){
                    bufA.pop_front();
                }
                if(bufB[0] != 0xA55A){
                    bufB.pop_front();
                }
            }
            Msg1.AD[0] = 0xA55A;
            for(int i=0;i<18;i++){
                Msg1.AD[i] = bufA[i];
                Msg1.AD[i+18] = bufB[i];
            }
        }
        int flag_save = !fifo_reader.get_single_gpio_in(&fifo_reader.m_mcu_CMN[2]);
        std::cout << "flag_save"<< std::to_string(flag_save)<<std::endl;

        if(flag_save){
            fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[1],0);
            fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[4],0);
        }else{
            fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[1],1);
            fifo_reader.set_single_gpio_out(&fifo_reader.m_mcu_CMN[4],1);
        }
    }

    return 0;
}
