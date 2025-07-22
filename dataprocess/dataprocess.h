#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <vector>
//#include <deque>
#include <mutex>
#include <thread>
#include <string.h>
#include <QMutex>
//#include <queue>
#define DATAUNIT_DATA_LEN 37
class DataUnit
{
public:
    DataUnit(int16_t in_data[]){
        memcpy(data,in_data,DATAUNIT_DATA_LEN*2);
        multiply_data(data,m_data);
        for(int i=0;i<16;i++){
            m_bit[i] = (in_data[36]>>i)&0x01;
        }
        m_bit[9] = m_bit[9] ==1?0:1;
    };
    ~DataUnit(){}

    void multiply_data(int16_t in_data[],double out_data[]);

    double m_data[DATAUNIT_DATA_LEN];//转换之后的数据
    int16_t data[DATAUNIT_DATA_LEN];
    uint8_t m_bit[16];//16位_数字量
};

class DataProcess
{
public:
    DataProcess();
    //添加数据
 //   void add_data(DataUnit data);
    //std::queue<DataUnit> m_data_buf;//缓存数据
//    std::deque<DataUnit> m_data_buf;//缓存数据

    void add_data1(float ch1,float ch2,float ch3);
    //找一个周期的开始点和结束点，上一个值小于等于0，当前值大于0即为起点，同时也是结束点
  //  void calc_freq(int channel,double &freq);
    void calc_freq1(int channel,double &freq);//0,1,2


    std::mutex m_mutex;
private:
    int m_count=0;
    float m_data_ch1[2000];
    float m_data_ch2[2000];
    float m_data_ch3[2000];
};

#endif // DATAPROCESS_H
