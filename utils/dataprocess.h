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

    double m_data[18];//转换之后的数据
   // double m_effective_value[16];//有效值 只要1，3，5，7，9，11，12的有效值
    int16_t data[18];
    uint8_t m_bit[16];//16位_数字量
};

class DataProcess
{
public:
    DataProcess();
    //添加数据
    void add_data(DataUnit data);
    std::deque<DataUnit> m_data_buf;//缓存数据
    //找一个周期的开始点和结束点，上一个值小于等于0，当前值大于0即为起点，同时也是结束点
    void find_period_start_end(int channel,int *start_index,int *end_index,
                               double &period,double &effect_value);


    QMutex m_mutex;
};

#endif // DATAPROCESS_H
