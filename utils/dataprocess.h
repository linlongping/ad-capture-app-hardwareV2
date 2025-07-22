#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <QMutex>
class DataUnit
{
public:
    DataUnit(int16_t in_data[]){
        memcpy(data,in_data,18*2);
        multiply_data(data,m_data);
        for(int i=0;i<16;i++){
            m_bit[i] = (in_data[17]>>i)&0x01;
        }
        m_bit[9] = m_bit[9] ==1?0:1;
    };

    void multiply_data(int16_t in_data[],double out_data[]);

    double m_data[18];//ת��֮�������
   // double m_effective_value[16];//��Чֵ ֻҪ1��3��5��7��9��11��12����Чֵ
    int16_t data[18];
    uint8_t m_bit[16];//16λ_������
};

class DataProcess
{
public:
    DataProcess();
    //�������
    void add_data(DataUnit data);
    std::deque<DataUnit> m_data_buf;//��������
    //��һ�����ڵĿ�ʼ��ͽ����㣬��һ��ֵС�ڵ���0����ǰֵ����0��Ϊ��㣬ͬʱҲ�ǽ�����
    void find_period_start_end(int channel,int *start_index,int *end_index,
                               double &period,double &effect_value);


    QMutex m_mutex;
};

#endif // DATAPROCESS_H
