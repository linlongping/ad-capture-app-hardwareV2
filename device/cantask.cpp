#include "cantask.h"
#include <string.h>
#include <QSettings>
#include <thread>
#include <iostream>
#include <time.h>

extern int g_err_code;
CanTask::CanTask()
{
    load_kb();
    m_pCanDev = new CanDevice(0);
    m_pThread = new std::thread(&CanTask::thread_send_func,this);

    m_pRecvThread = new std::thread(&CanTask::thread_recv_func,this);
}
CanTask::~CanTask()
{
    quit = true;
    m_pThread->join();
    delete m_pThread;
    m_pThread = nullptr;

    m_pRecvThread->join();
    delete m_pRecvThread;
    m_pRecvThread = nullptr;
}
void CanTask::load_kb()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    for(int i=0;i<40;i++){
        setting.beginGroup("kb"+QString::number(i));
        m_ad_kb[i].k = setting.value("k",QVariant("1.0")).toFloat();
        m_ad_kb[i].b = setting.value("b",QVariant("0.0")).toFloat();
        setting.endGroup();
    }
}
void CanTask::save_kb()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    for(int i=0;i<40;i++){
        setting.beginGroup("kb"+QString::number(i));
        setting.setValue("k",m_ad_kb[i].k);
        setting.setValue("b",m_ad_kb[i].b);
        setting.endGroup();
    }
}
void CanTask::thread_recv_func()
{
    while(!quit){
        uint32_t can_id;
        uint8_t can_data[8];

        uint32_t len;
        m_pCanDev->can_read(can_id,can_data,len);
        //
        if(can_id >=0x100 && can_id<= 0x11d){
            uint16_t index = can_id - 0x100;
            float k = *(float*)(can_data);
            float b = *(float*)(can_data+4);
            m_ad_kb[index].k = k;
            m_ad_kb[index].b = b;
            save_kb();
        }else if(can_id == 0x130){//set time
            uint16_t year = (can_data[0]<<8)|(can_data[1]);
            uint16_t month = can_data[2];
            uint16_t date = can_data[3];
            uint16_t hour = can_data[4];
            uint16_t minute = can_data[5];
            uint16_t second = can_data[6];

            std::string cmd = "date -s \""+std::to_string(year)+"-"+std::to_string(month)+"-"+std::to_string(date)+" "+
                    std::to_string(hour)+":"+std::to_string(minute)+":"+std::to_string(second)+"\"";
            std::cout << "system call cmd:"<< cmd <<std::endl;
            system(cmd.data());
            //同步到硬件时钟
            std::string syncCmd = "hwclock -wu";
            system(syncCmd.c_str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void CanTask::set_data(int16_t *data, int data_len)
{
    m_mutex.lock();
    for(int i=0;i<data_len-1;i++){
        m_data[i] = int16_t(data[i]*m_ad_kb[i].k+m_ad_kb[i].b);
    }
    m_data[data_len-1] = data[data_len-1];
    //calc effect value
    //memcpy(m_data,data,data_len*sizeof(int16_t));
    m_mutex.unlock();
    for(int i=0;i<15;i++){
        m_effect_buf[i].add_data(m_data[17+i]);
    }
}
void CanTask::enable_send(bool send)
{
    m_send = send;
}

void CanTask::thread_send_func()
{
    clock_t last_clock = clock();
    int16_t send_buf[48];

    while(!quit){
        m_mutex.lock();
        for(int i=0;i<17;i++){
            send_buf[i]=m_data[i];
        }
        for(int i=32;i<36;i++){
            send_buf[i] = m_data[i];
        }
        m_mutex.unlock();
        clock_t curr_clock = clock();
        if((double(curr_clock-last_clock)/CLOCKS_PER_SEC)>1.0){
            last_clock = clock();
            ////std::cout << "calc effect value every 1 second"<<std::endl;
            for(int i=0;i<15;i++){
                send_buf[17+i] = m_effect_buf[i].calc_effect_value();
            }
        }
        //send_data
        for(int i=0;i<9;i++){
           uint32_t can_id = i;
           uint8_t can_data[8];
           can_data[0] = send_buf[i*4]&0xff;
           can_data[1] = send_buf[i*4]>>8;
           can_data[2] = send_buf[i*4+1]&0xff;
           can_data[3] = send_buf[i*4+1]>>8;
           can_data[4] = send_buf[i*4+2]&0xff;
           can_data[5] = send_buf[i*4+2]>>8;
           can_data[6] = send_buf[i*4+3]&0xff;
           can_data[7] = send_buf[i*4+3]>>8;
           if(m_send){
            m_pCanDev->can_write(can_id,can_data,8);
           }
           std::this_thread::sleep_for(std::chrono::microseconds(2400));
        }
        //send_err_code
        uint32_t can_id = 10;
        uint8_t data[8] = {(uint8_t)g_err_code,0};
        m_pCanDev->can_write(can_id,data,1);
    }
}

