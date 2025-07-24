/**
 * @file utils.h
 * @brief 通用工具函数及类声明
 * @details 包含工具类Utils的定义，以及文件操作、FFT计算等通用工具函数的声明，
 *          提供文件管理、频率分析等辅助功能
 */
#ifndef UTILS_H
#define UTILS_H

/**
 * @class Utils
 * @brief 通用工具类
 * @details 基础工具类框架，可用于封装各类通用工具方法，目前为默认构造函数实现
 */
class Utils
{
public:
    /**
     * @brief 构造函数
     * @details 初始化Utils类实例，目前无特殊初始化操作
     */
    Utils();
};

/**
 * @brief 统计指定目录下的文件数量
 * @details 查找/run/media/mmcblk1p1/路径下后缀为.txt的文件并统计数量
 * @return 符合条件的文件数量
 */
int find_file_number();

/**
 * @brief 删除过期文件
 * @details 当文件数量超过阈值时，删除最早创建的5个.txt文件，用于控制存储占用
 */
void delete_file();

/**
 * @brief 创建新的文件描述符
 * @details 以当前时间为文件名在/run/media/mmcblk1p1/路径下创建新的.txt文件，
 *          并在创建前执行文件清理操作，确保存储空间充足
 * @return 新创建文件的文件描述符，失败时会触发系统重启
 */
int get_new_fd();

/**
 * @brief 计算FFT得到信号的主频率
 * @param[in] n 采样点数量
 * @param[in] in 输入的采样数据数组（实数）
 * @param[in] sample_rate 采样频率
 * @return 信号的主频率值（Hz）
 */
float get_fft_freq(int n, const float *in, int sample_rate);

/**
 * @brief FFT功能测试函数
 * @details 生成包含特定频率成分的测试信号，通过FFT计算验证频率分析功能的正确性
 */
void fft_test();

#endif // UTILS_H
