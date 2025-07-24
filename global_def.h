#ifndef GLOBAL_DEF_H
#define GLOBAL_DEF_H

/**
 * @file global_def.h
 * @brief 全局常量与数据结构定义
 * @details 定义系统中跨模块使用的常量、宏定义及公共数据结构，
 *          为整个应用程序提供统一的基础配置和数据格式标准
 */

/**
 * @def DATAPIPE
 * @brief 数据通道数量
 * @details 定义系统支持的模拟量输入通道总数，用于数据缓冲区分配和通道管理
 */
#define DATAPIPE 37

/**
 * @def DATAPACK_WRITE
 * @brief 写入文件的数据块大小
 * @details 定义单次写入文件的数据记录数量，用于控制文件写入的批量处理
 */
#define DATAPACK_WRITE 50

/**
 * @def SLEEP_TIME
 * @brief 短延时宏定义
 * @details 通过usleep实现5000微秒（5毫秒）的固定延时，用于循环中的时间控制
 */
#define SLEEP_TIME do{ usleep(5000);}while(0);

/**
 * @def FILE_MAX
 * @brief 最大文件数量限制
 * @details 定义系统允许保存的最大文件数量，用于文件管理和自动清理
 */
#define FILE_MAX   300

/**
 * @def FILE_TO_FILE_COST_SEC
 * @brief 文件切换时间间隔（秒）
 * @details 定义新建文件的时间间隔，控制文件分割的时间粒度，单位为秒
 */
#define FILE_TO_FILE_COST_SEC   60

/**
 * @def VERSION
 * @brief 版本信息打印宏
 * @details 执行时打印当前软件版本号，用于版本标识和调试
 */
#define VERSION do{printf("This version is JLY_test1.15.03");}while(0);

/**
 * @def SERVERPORT
 * @brief TCP服务器监听端口
 * @details 定义系统TCP服务端的监听端口号，用于网络通信的端口配置
 */
#define  SERVERPORT    8885

/**
 * @struct msg
 * @brief 消息队列数据结构
 * @details 用于进程间通过消息队列传递AD采样数据，定义消息类型和数据缓冲区格式
 */
struct msg
{
    long mytype;                 ///< 消息类型标识，用于消息队列中的类型过滤
    unsigned short AD[DATAPIPE]; ///< AD采样数据缓冲区，存储DATAPIPE个通道的16位采样值
};

/**
 * @var extern int msqid2
 * @brief 消息队列标识符（外部声明）
 * @details 跨文件共享的消息队列ID，用于标识进程间通信所使用的消息队列
 */
extern int msqid2;

#endif // GLOBAL_DEF_H
