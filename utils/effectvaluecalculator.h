#ifndef EFFECTVALUECALCULATOR_H
#define EFFECTVALUECALCULATOR_H

#include <queue>
#include <math.h>
#include <stdint.h>
#include <mutex>

/**
 * @file effectvaluecalculator.h
 * @brief 有效值计算器类定义
 * @details 提供基于滑动窗口的实时数据有效值（均方根值）计算功能，
 *          支持线程安全的数据添加和计算操作，适用于需要实时分析的传感器数据处理场景
 */

/**
 * @def BUF_LEN
 * @brief 数据缓冲区长度宏定义
 * @details 定义滑动窗口的最大长度为2000，超过此长度时自动移除最早的数据，
 *          确保计算基于最近的固定数量样本
 */
#define BUF_LEN 2000

/**
 * @class EffectValueCalculator
 * @brief 有效值计算器类
 * @details 采用双端队列维护数据缓冲区，通过互斥锁保证多线程环境下的数据安全性，
 *          提供数据添加和有效值计算的核心接口，用于实时计算输入数据序列的均方根值
 */
class EffectValueCalculator
{
public:
    /**
     * @brief 构造函数
     * @details 初始化有效值计算器实例，创建空的数据缓冲区和互斥锁
     */
    EffectValueCalculator();

    /**
     * @brief 添加数据到缓冲区
     * @details 将新的16位整数数据存入缓冲区，若缓冲区长度超过BUF_LEN则移除最早的数据，
     *          操作过程通过互斥锁保证线程安全
     * @param[in] data 待添加的16位整数数据
     */
    void add_data(int16_t data){
        m_mutex.lock();
        buf.push_back(data);
        if(buf.size()>BUF_LEN){
            buf.pop_front();
        }
        m_mutex.unlock();
    }

    /**
     * @brief 计算有效值（均方根值）
     * @details 基于缓冲区中所有数据计算有效值，公式为：sqrt(Σ(data²)/N)，其中N为缓冲区数据数量，
     *          计算过程通过互斥锁保证线程安全，缓冲区为空时返回0.0
     * @return 计算得到的有效值（双精度浮点数）
     */
    double calc_effect_value(){
        if(buf.empty()) return  0.0;
        //计算有效值
        m_mutex.lock();
        double sum = 0;
        for(int i=0;i<buf.size();i++){
            sum +=  buf.at(i)*buf.at(i);
        }
        m_mutex.unlock();
        double effect_value = sqrt(sum/buf.size());
        return effect_value;
    }

private:
    /**
     * @brief 数据缓冲区
     * @details 采用双端队列存储最近的输入数据，支持高效的首尾插入和删除操作，
     *          最大长度由BUF_LEN宏定义限制
     */
    std::deque<int16_t> buf;

    /**
     * @brief 互斥锁
     * @details 用于保护缓冲区的并发访问，确保多线程环境下add_data和calc_effect_value操作的原子性
     */
    std::mutex  m_mutex;
};

#endif // EFFECTVALUECALCULATOR_H
