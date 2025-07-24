/**
 * @file fiforeader.h
 * @brief FIFO数据读取器类的头文件，负责GPIO初始化、FIFO数据读取及相关硬件交互
 * @author
 * @date
 * @version 1.0
 */
#ifndef FIFOREADER_H
#define FIFOREADER_H
#include <stdint.h>

    /**
 * @struct GpioInfo
 * @brief GPIO引脚信息结构体，存储引脚编号和文件描述符
 */
    struct GpioInfo{
    int pin;   ///< GPIO引脚编号
    int fd;    ///< GPIO操作的文件描述符
};

/**
 * @class FifoReader
 * @brief FIFO数据读取器类，负责初始化GPIO、读取FIFO数据及控制相关硬件引脚
 * @details 该类封装了与FIFO相关的GPIO配置、数据读取和硬件控制功能，
 *          支持从FIFO A和FIFO B两个通道读取数据，并提供GPIO输入输出控制接口
 */
class FifoReader
{
public:
    /**
     * @brief 构造函数，初始化GPIO引脚信息
     */
    FifoReader();

    /**
     * @brief 析构函数，释放GPIO资源
     */
    ~FifoReader();

    /**
     * @brief 测试GPIO功能，用于调试GPIO输出控制
     * @return 成功返回0，失败返回非0
     */
    int test_gpio();

    /**
     * @brief 从FIFO A读取一个16位数据
     * @param[out] value 存储读取到的数据
     * @return 成功读取返回true，否则返回false
     */
    bool get_fifo_A_value(uint16_t *value);

    /**
     * @brief 从FIFO B读取一个16位数据
     * @param[out] value 存储读取到的数据
     * @return 成功读取返回true，否则返回false
     */
    bool get_fifo_B_value(uint16_t *value);

    /**
     * @brief 初始化所有GPIO引脚（注册、配置方向、打开文件）
     * @return 成功返回0，失败返回非0
     */
    int gpio_init();

    /**
     * @brief 初始化FIFO相关硬件（复位、使能等）
     * @return 成功返回0，失败返回非0
     */
    int fifo_init();

    /**
     * @brief 设置指定GPIO引脚的输出值
     * @param[in] pin 指向GpioInfo结构体的指针
     * @param[in] out 输出值（0或1）
     * @return 成功返回0，失败返回非0
     */
    int set_single_gpio_out(GpioInfo *pin, int out);

    /**
     * @brief 读取指定GPIO引脚的输入值
     * @param[in] pin 指向GpioInfo结构体的指针
     * @return 成功返回引脚值（0或1），失败返回-1
     */
    int get_single_gpio_in(GpioInfo *pin);

    /**
     * @brief 从FIFO A读取16位数据（原始读取，不检查FIFO状态）
     * @return 读取到的16位数据
     */
    unsigned short read_fifo_A();

    /**
     * @brief 从FIFO B读取16位数据（原始读取，不检查FIFO状态）
     * @return 读取到的16位数据
     */
    unsigned short read_fifo_B();

    /**
     * @brief 设置FIFO读使能输出（预留接口）
     * @param[in] out 输出值（0或1）
     * @return 未实现，默认返回0
     */
    int set_fifo_r_out(int out);

    GpioInfo m_mcu_CMN[5]; ///< 与MCU通信的GPIO引脚数组（含4个传输LED控制）
    GpioInfo m_fifo_pin[32]; ///< FIFO数据引脚数组（32位）
    GpioInfo m_fifo_ff_A2; ///< FIFO A的满标志引脚
    GpioInfo m_fifo_R_A; ///< FIFO A的读控制引脚
    GpioInfo m_fifo_rt_A; ///< FIFO A的复位引脚
    GpioInfo m_fifo_ef_A2; ///< FIFO A的空标志引脚
    GpioInfo m_fifo_ff_B2; ///< FIFO B的满标志引脚
    GpioInfo m_fifo_R_B; ///< FIFO B的读控制引脚
    GpioInfo m_fifo_EF_B2; ///< FIFO B的空标志引脚
    GpioInfo m_fifo_RT_B; ///< FIFO B的复位引脚
    GpioInfo m_save_led; ///< 保存状态LED的控制引脚

private:
    /**
     * @brief 注册所有GPIO引脚（向系统导出GPIO）
     * @return 成功返回0，失败返回非0
     */
    int gpio_register();

    /**
     * @brief 注销所有GPIO引脚（从系统取消导出）
     * @return 成功返回0，失败返回非0
     */
    int gpio_unregister();

    /**
     * @brief 配置所有GPIO引脚的方向（输入/输出）
     * @return 成功返回0，失败返回非0
     */
    int gpio_direct_init();

    /**
     * @brief 打开所有GPIO引脚的操作文件
     * @return 成功返回0，失败返回非0
     */
    int gpio_all_open();

    /**
     * @brief 关闭所有GPIO引脚的操作文件
     * @return 成功返回0，失败返回非0
     */
    int gpio_all_close();

    /**
     * @brief 配置单个GPIO引脚的方向
     * @param[in] pin GPIO引脚编号
     * @param[in] isOut true表示输出，false表示输入
     * @return 成功返回0，失败返回非0
     */
    int config_single_gpio_out(int pin, bool isOut);
};

#endif // FIFOREADER_H
