#ifndef CANTASK_H
#define CANTASK_H

#include "candevice.h"
#include <stdint.h>
#include <thread>
#include <mutex>
#include "utils/effectvaluecalculator.h"

/**
 * @file cantask.h
 * @brief CAN通信任务类定义
 * @details 封装CAN设备的发送与接收任务管理，包含数据转换系数的加载/保存、
 *          多线程数据收发控制及有效值计算缓冲等功能，实现CAN总线上的数据交互
 */

/**
 * @struct AD_KB
 * @brief 数据转换系数结构体
 * @details 存储AD转换的比例系数(k)和偏移量(b)，用于原始数据到物理量的转换，
 *          公式为：物理量 = 原始值 * k + b
 */
struct AD_KB{
    float k=1.0;  ///< 比例系数，默认值1.0
    float b=0;    ///< 偏移量，默认值0
};

/**
 * @class CanTask
 * @brief CAN通信任务核心类
 * @details 管理CAN设备的初始化、数据收发线程、转换系数配置及有效值计算，
 *          提供线程安全的数据设置接口和发送使能控制，实现CAN数据的高效处理与传输
 */
class CanTask
{
public:
    /**
     * @brief 构造函数
     * @details 初始化CAN任务实例，创建CAN设备对象并初始化相关资源
     */
    CanTask();

    /**
     * @brief 析构函数
     * @details 停止收发线程，释放CAN设备资源及动态分配的内存
     */
    ~CanTask();

    /**
     * @brief 设置待发送的数据
     * @details 将输入的16位整数数据存入内部缓冲区，线程安全操作
     * @param[in] data 待发送的16位整数数据数组
     * @param[in] data_len 数据元素数量
     */
    void set_data(int16_t *data,int data_len);

    /**
     * @brief 启用/禁用CAN数据发送
     * @details 控制发送线程是否执行数据发送操作
     * @param[in] send 发送使能状态：true启用发送，false禁用发送
     */
    void enable_send(bool send);

    /**
     * @brief 加载转换系数
     * @details 从存储介质（如文件或配置区）加载AD转换的k和b系数到m_ad_kb数组
     */
    void load_kb();

    /**
     * @brief 保存转换系数
     * @details 将当前m_ad_kb数组中的转换系数保存到存储介质，持久化配置
     */
    void save_kb();

    int16_t m_data[48];        ///< 待发送的16位整数数据缓冲区（最大48个元素）
    AD_KB m_ad_kb[48];         ///< 48路数据对应的转换系数数组，与m_data一一对应

private:
    CanDevice *m_pCanDev;      ///< CAN设备对象指针，用于底层CAN通信操作

    std::thread *m_pThread;    ///< 发送线程指针，负责周期性发送CAN数据
    /**
     * @brief 发送线程函数
     * @details 线程入口函数，循环检测发送使能状态，当允许发送时通过CAN设备发送数据
     */
    void thread_send_func();

    std::thread *m_pRecvThread;///< 接收线程指针，负责监听并处理接收的CAN数据
    /**
     * @brief 接收线程函数
     * @details 线程入口函数，持续从CAN设备接收数据并进行相应处理
     */
    void thread_recv_func();

    bool quit = false;                 ///< 线程退出标志：true表示触发线程退出
    std::mutex m_mutex;        ///< 互斥锁，保证多线程环境下数据访问的安全性

    bool m_send = false;               ///< 发送使能标志：true表示允许发送数据

    EffectValueCalculator m_effect_buf[15];///< 15路有效值计算器缓冲区，用于数据有效值计算
};

#endif // CANTASK_H
