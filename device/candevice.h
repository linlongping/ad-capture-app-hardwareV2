#ifndef CANDEVICE_H
#define CANDEVICE_H
#include <stdint.h>
#include <mutex>

/**
 * @file candevice.h
 * @brief CAN设备操作类定义
 * @details 封装CAN总线设备的底层操作接口，提供CAN消息的发送与接收功能，
 *          负责CAN设备的初始化及数据交互的线程安全控制
 */

/**
 * @class CanDevice
 * @brief CAN设备操作核心类
 * @details 实现CAN总线设备的打开、关闭、消息发送和接收等基础功能，
 *          隐藏底层硬件操作细节，为上层应用提供简洁的CAN通信接口
 */
class CanDevice
{
public:
    /**
     * @brief 构造函数
     * @details 初始化CAN设备实例，打开指定ID的CAN设备并进行初始化配置
     * @param[in] id CAN设备标识符，用于指定操作的CAN通道
     */
    CanDevice(int id);

    /**
     * @brief 发送CAN消息
     * @details 向CAN总线发送指定ID和数据的消息
     * @param[in] can_id CAN消息标识符
     * @param[in] data 待发送的消息数据缓冲区
     * @param[in] len 待发送数据的长度（字节数）
     * @return 操作结果：0表示成功，非0表示失败
     */
    int can_write(uint32_t can_id, uint8_t *data, uint32_t len);

    /**
     * @brief 接收CAN消息
     * @details 从CAN总线接收消息，获取消息ID、数据及长度信息
     * @param[out] can_id 接收消息的标识符
     * @param[out] data 接收消息的数据缓冲区
     * @param[out] len 接收数据的实际长度（字节数）
     * @return 操作结果：0表示成功，非0表示失败
     */
    int can_read(uint32_t &can_id, uint8_t *data, uint32_t &len);

private:
    int can_fd;               ///< CAN设备文件描述符，用于底层CAN设备操作
    // std::mutex m_mutex;     ///< 互斥锁，用于保证读写操作的线程安全（注释说明预留）
};

#endif // CANDEVICE_H
