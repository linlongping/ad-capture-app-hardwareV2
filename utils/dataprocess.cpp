#include "dataprocess.h"
#include <QDebug>
#include "utils.h"
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
#define MAX_LEN (30*1000)
void DataUnit::multiply_data(int16_t *in_data, double out_data[])
{
    for(int i=1;i<13;i++){
        out_data[i] = in_data[i]*g_rate[i];
    }
    return;
}

DataProcess::DataProcess()
{

}

//计算一个通道的周期起点和终点，同时计算一下频率和有效值
void DataProcess::find_period_start_end(int channel,int *start_index, int *end_index,
                                        double &period,double &effect_value)
{
    m_mutex.lock();
    if(m_data_buf.size()<20*1000) {
        m_mutex.unlock();
        return;
    }
#if 0
    bool found_start = false;
    bool found_one_period = false;
    for(int i=0;i<m_data_buf.size()-1;i++){
        if(m_data_buf[i].m_data[channel]<=0&&
                m_data_buf[i+1].m_data[channel]>0){
      //      qDebug() << QString::number(m_data_buf[i].m_data[channel]);
            if(found_start == false){
                *start_index = i;//第一次为起点
                found_start = true;
            }else{
                *end_index  = i;//第二次为终点
                found_one_period = true;
                break;
            }
        }
    }
    if(found_one_period){
       period = 50*(*end_index-*start_index)/1000.0;//ms,400hz，大概为2.5ms
    }else{
        m_mutex.unlock();
        return;
    }
#endif
    //使用fft计算频率
    float tmp_data[20*1000];
    for(int i=0;i<20*1000;i++){
        tmp_data[i] = m_data_buf[i].m_data[channel];
    }
    period = get_fft_freq(20*1000,tmp_data,20*992);//这个是频率
    //有效值
    double sum = 0;
    *start_index = 0;//直接取2000个点计算有效值
    *end_index = 2000;
    for(int i=*start_index;i<*end_index;i++){
        sum += m_data_buf[i].m_data[channel]*m_data_buf[i].m_data[channel];
    }
    effect_value = sqrt(sum/(*end_index - *start_index)) * 1.0141;
    m_mutex.unlock();
    return;
}

void DataProcess::add_data(DataUnit data)
{
    m_mutex.lock();
    m_data_buf.push_back(data);
    if(m_data_buf.size()>MAX_LEN){
        m_data_buf.pop_front();
    }
    m_mutex.unlock();
}
