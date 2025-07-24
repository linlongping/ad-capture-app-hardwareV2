#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <QMutex>

/**
 * @file dataprocess.h
 * @brief 数据处理相关类定义
 * @details 包含数据单元类DataUnit和数据处理类DataProcess的定义，
 *          用于实现数据转换、周期检测及有效值计算等数据处理功能
 */

/**
 * @class DataUnit
 * @brief 数据单元类
 * @details 封装单组采集数据，包含原始数据、转换后数据及数字量信息，
 *          提供数据转换方法，用于标准化数据处理
 */
class DataUnit
{
public:
    /**
     * @brief 构造函数
     * @details 初始化数据单元，复制原始数据并进行转换，提取16位数字量，
     *          对第9位数字量进行取反处理
     * @param[in] in_data 输入的16位整数数据数组（长度为18）
     */
    DataUnit(int16_t in_data[]){
        memcpy(data,in_data,18*2);
        multiply_data(data,m_data);
        for(int i=0;i<16;i++){
            m_bit[i] = (in_data[17]>>i)&0x01;
        }
        m_bit[9] = m_bit[9] ==1?0:1;
    };

    /**
     * @brief 数据转换方法
     * @details 将16位整数数据转换为双精度浮点数数据，具体转换规则由实现定义
     * @param[in] in_data 输入的16位整数数据数组
     * @param[out] out_data 输出的双精度浮点数数据数组
     */
    void multiply_data(int16_t in_data[],double out_data[]);

    double m_data[18];      ///< 转换之后的双精度浮点数数据（长度为18）
    int16_t data[18];       ///< 原始16位整数数据（长度为18）
    uint8_t m_bit[16];      ///< 16位数字量，其中第9位已做取反处理
};

/**
 * @class DataProcess
 * @brief 数据处理类
 * @details 维护数据缓冲区，提供数据添加和周期检测功能，用于分析数据的周期特性
 *          和计算有效值，支持多线程安全操作
 */
class DataProcess
{
public:
    /**
     * @brief 构造函数
     * @details 初始化数据处理实例，创建空的数据缓冲区
     */
    DataProcess();

    /**
     * @brief 添加数据到缓冲区
     * @details 将新的数据单元存入缓冲区，线程安全操作
     * @param[in] data 待添加的DataUnit类型数据
     */
    void add_data(DataUnit data);

    std::deque<DataUnit> m_data_buf;  ///< 数据缓冲区，用于存储历史数据单元
    QMutex m_mutex;                   ///< 互斥锁，保证多线程环境下的数据操作安全

    /**
     * @brief 查找周期的起止点
     * @details 基于指定通道数据，寻找周期起点（上一个值≤0且当前值>0）和终点，
     *          计算周期长度和有效值
     * @param[in] channel 通道索引
     * @param[out] start_index 周期起点索引
     * @param[out] end_index 周期终点索引
     * @param[out] period 计算得到的周期长度
     * @param[out] effect_value 计算得到的有效值
     */
    void find_period_start_end(int channel,int *start_index,int *end_index,
                               double &period,double &effect_value);
};

#endif // DATAPROCESS_H
