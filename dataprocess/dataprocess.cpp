#include "dataprocess.h"
#include <QDebug>
#include "utils.h"
#include <math.h>
double g_rate[16]={0,
                   0.019937988,
                   //0.009968994,
                   0.305175781,
                   //0.2906436,
                   0.019937988,
                   //0.009968994,
                   0.007629395,
                   //0.003814697,
                   0.019937988,
                   //0.009968994,
                   0.003051758,
                   //0.001525879,
                   0.019937988,
                   //0.009968994,
                   0.007629395,
                   //0.003814697,
                   0.019937988,
                   //0.009968994,
                   0.030517578,
                   //0.015258789,
                   0.019937988,
                   //0.009968994,
                   0.054986572
                   //0.027493286
};
#define MAX_LEN (3*1000)
void DataUnit::multiply_data(int16_t *in_data, double out_data[])
{
    for(int i=1;i<37;i++){
        out_data[i] = in_data[i];//*g_rate[i];
    }
    return;
}

DataProcess::DataProcess()
{

}

//计算一个通道的周期起点和终点，同时计算一下频率和有效值
//void DataProcess::calc_freq(int channel, double &freq)
//{
//    m_mutex.lock();
//    if(m_data_buf.size()<2*1000) {
//        m_mutex.unlock();
//        return;
//    }
//    //使用fft计算频率
//    float tmp_data[2*1000];
//    for(int i=0;i<std::min<int>(2*1000,m_data_buf.size());i++){
//        tmp_data[i] = m_data_buf.at(i).m_data[channel];
//    }
//    m_mutex.unlock();
//    freq = get_fft_freq(std::min<int>(2*1000,m_data_buf.size()),tmp_data,1000);//这个是频率

//    return;
//}
//void DataProcess::add_data(DataUnit data)
//{
//    m_mutex.lock();
//    m_data_buf.push_back(data);
//    if(m_data_buf.size()>MAX_LEN){
//        m_data_buf.pop_front();
//    }
//    m_mutex.unlock();
//}
//------------------
void DataProcess::calc_freq1(int channel, double &freq)
{
        float tmp_data[2*1000];
    switch (channel) {
    case 0:
        for(int i=0;i<2000-m_count;i++){
            tmp_data[i] = m_data_ch1[m_count+i];
        }
        for(int i=0;i<m_count;i++){
            tmp_data[2000-m_count+i]= m_data_ch1[i];
        }
        break;
    case 1:
        for(int i=0;i<2000-m_count;i++){
            tmp_data[i] = m_data_ch2[m_count+i];
        }
        for(int i=0;i<m_count;i++){
            tmp_data[2000-m_count+i]= m_data_ch2[i];
        }
        break;
    case 2:
        for(int i=0;i<2000-m_count;i++){
            tmp_data[i] = m_data_ch3[m_count+i];
        }
        for(int i=0;i<m_count;i++){
            tmp_data[2000-m_count+i]= m_data_ch3[i];
        }
        break;
    default:
        break;
    }
        freq = get_fft_freq(2*1000,tmp_data,1000);//这个是频率
    return;
}
void DataProcess::add_data1(float ch1,float ch2,float ch3)
{
    m_data_ch1[m_count] = ch1;
    m_data_ch2[m_count] = ch2;
    m_data_ch3[m_count] = ch3;
    m_count++;
    if(m_count>=2000){
        m_count = 0;
    }
}
