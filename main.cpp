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

#define CAN0_INIT "ip link set can0 type can bitrate 125000 triple-sampling on"
#define CAN0_UP "ifconfig can0 up"

enum ERR_CODE{
    ERR_OK=0,
    ERR_NO_SD=-1,
    ERR_DATE_TIME=-2,
};

int32_t g_err_code = ERR_OK;

struct msg Msg1;
struct msg Msg2;

unsigned  int state=0;
int msqid, msqid2;                          //////
int flag_save=1;
key_t key;
TcpServer *m_pServer = nullptr;

DataProcess m_data_process;//calc freq
double g_freq_115V[3];//115V abc freq
//void *cap_thread_handler(void *arg)
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
