/**
 * @file maintask.h
 * @brief 主任务相关宏定义、结构体及函数声明
 * @details 包含数据存储任务中使用的常量定义、消息结构体和主任务函数声明，
 *          用于协调数据采集、存储等核心功能的运行
 * @author （可补充作者信息）
 * @version 1.15.03
 * @date （可补充日期信息）
 */
#ifndef MAINTASK_H
#define MAINTASK_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/**
 * @def DATAPIPE
 * @brief 单次数据帧长度
 * @details 定义每帧AD采集数据的通道数量（数据点个数）
 */
#define DATAPIPE 18

/**
 * @def DATAPACK_WRITE
 * @brief 单次写入文件的数据帧数量
 * @details 定义每次向文件写入数据时包含的数据帧总数
 */
#define DATAPACK_WRITE 50

/**
 * @def SLEEP_TIME
 * @brief 休眠宏定义
 * @details 封装usleep函数，实现5000微秒（5毫秒）的休眠操作
 */
#define SLEEP_TIME do{ usleep(5000);}while(0);

/**
 * @def FILE_MAX
 * @brief 最大文件数量阈值
 * @details 定义存储文件的最大数量限制，用于控制存储空间占用
 */
#define FILE_MAX   300

/**
 * @def FILE_TO_FILE_COST_SEC
 * @brief 文件切换时间间隔（秒）
 * @details 定义数据文件的切换周期，单位为秒，到达该时间后创建新文件存储数据
 */
#define FILE_TO_FILE_COST_SEC   60

/**
 * @def VERSION
 * @brief 版本信息宏
 * @details 打印当前程序版本信息（JLY_test1.15.03）
 */
#define VERSION do{printf("This version is JLY_test1.15.03");}while(0);

/**
 * @struct msg
 * @brief 消息队列数据结构体
 * @details 用于在进程/线程间通过消息队列传递AD采集数据
 */
struct msg
{
    long mytype;                ///< 消息类型，用于消息队列中区分不同消息
    unsigned short AD[DATAPIPE];///< AD采集数据数组，长度由DATAPIPE定义
};

/**
 * @var extern int msqid2
 * @brief 外部消息队列ID
 * @details 声明外部定义的消息队列标识符，用于跨文件访问消息队列
 */
extern int msqid2;

/**
 * @fn void main_task()
 * @brief 主任务函数
 * @details 负责初始化系统资源、启动数据采集和存储线程、协调各模块运行的入口函数
 */
void main_task();

#endif // MAINTASK_H
