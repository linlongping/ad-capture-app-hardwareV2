#include "candevice.h"
#include <linux/can.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include<algorithm>
#include <iostream>

CanDevice::CanDevice(int id)
{
    struct sockaddr_can addr;
    struct ifreq ifr;
    can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW); // 创建 SocketCAN 套接字
    if(id == 0){
        strcpy(ifr.ifr_name, "can0" );
    }else{
        strcpy(ifr.ifr_name, "can1" );
    }
    ioctl(can_fd, SIOCGIFINDEX, &ifr); // 指定 can0 设备
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(can_fd, (struct sockaddr *)&addr, sizeof(addr)); // 将套接字与 can0 绑定
    std::cout << "can inited"<<std::endl;
}

int CanDevice::can_write(uint32_t can_id, uint8_t *data, uint32_t len)
{
    struct can_frame frame;  // 声明 can 帧结构体，can_frame 定义在头文件 can.h 中

    for(int i=0;i<std::min<int>(len,8);i++){
        frame.data[i] = data[i];
    }

    /************ 写数据 ************/
    frame.can_dlc = std::min<int>(len,8);  // 设置数据长度（CAN协议规定一帧最多有八个字节的有效数据）
    frame.can_id = can_id; 	// 设置 ID 号，假设这里 ID 号为1
    // 实际的 ID 号要根据是标准帧（11位）还是拓展帧（29）位来设置
    // eg: frame.can_id = CAN_EFF_FLAG | 0x1;
   // m_mutex.lock();
    write(can_fd, &frame, sizeof(frame));  // 写数据
  //  m_mutex.unlock();
    return 0;
}

int CanDevice::can_read(uint32_t &can_id, uint8_t *data, uint32_t &len)
{
    struct can_frame frame;   // can_frame 结构体定义在头文件 can.h 中
    //m_mutex.lock();
    read(can_fd, &frame, sizeof(frame));  // 读取数据，读取到的有效数据保存在 frame.data[] 数组中
    //m_mutex.unlock();

    can_id = frame.can_id;
    len = frame.can_dlc;
    len = std::max<int>(8,len);
    for(int i=0;i<std::max<int>(len,8);i++){
        data[i] = frame.data[i];
    }
    return 0;
}
