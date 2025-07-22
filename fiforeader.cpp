#include "fiforeader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>  //define O_WRONLY and O_RDONLY
#include <iostream>
#include <thread>

#define GPIO_PIN(bank,port,index) (bank*32+(port-1)*8+index)
#define GPIO_DIR_OUT "out"
#define GPIO_DIR_IN  "in"

enum ERR_CODE{
  ERR_REGISTER_GPIO = -1,
  ERR_GPIO_SET_DIR = -2,
};

//#define FIFO_PIN(index)
#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"
#define SYSFS_GPIO_RST_PIN_VAL      "121"
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio121/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "out"
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio121/value"
#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"


int FifoReader::test_gpio()
{
        int fd = open("/sys/class/gpio/export", O_WRONLY);
          if(fd == -1)
            {
             printf("ERR: Radio hard reset pin open error.\n");
              return EXIT_FAILURE;
            }

        printf("open sys/gpio/class/gpio/export\r\n");
        std::cout << SYSFS_GPIO_RST_PIN_VAL<< "size"<< sizeof(SYSFS_GPIO_RST_PIN_VAL)<<std::endl;
          write(fd, SYSFS_GPIO_RST_PIN_VAL ,sizeof(SYSFS_GPIO_RST_PIN_VAL));
            close(fd);
             //设置端口方向/sys/class/gpio/gpio137# echo out > direction
            fd = open(SYSFS_GPIO_RST_DIR, O_WRONLY);
            if(fd == -1)
                {
                  printf("ERR: Radio hard reset pin direction open error.\n");
                  return EXIT_FAILURE;
            }
            printf("echo out direction\r\n");
            write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
            close(fd);

       //     输出复位信号: 拉高>100ns
            fd = open(SYSFS_GPIO_RST_VAL, O_RDWR);
            if(fd == -1)
                {
                  printf("ERR: Radio hard reset pin value open error.\n");
                 return EXIT_FAILURE;
                }
        while(1)
        {
            write(fd, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_H));
            printf("echo 1 > gpio121\r\n");
            sleep(1);
            write(fd, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
            printf("echo 0 > gpio121\r\n");
            sleep(1);
        }
        close(fd);
      //  loop
        return 0;
}

FifoReader::FifoReader()
{
    m_mcu_CMN[0].pin = GPIO_PIN(4,1,0);
    m_mcu_CMN[1].pin = GPIO_PIN(4,1,1);
    m_mcu_CMN[2].pin = GPIO_PIN(4,1,3);
    m_mcu_CMN[3].pin = GPIO_PIN(4,1,2);
    m_mcu_CMN[4].pin = GPIO_PIN(4,1,4);//led
    //port A:1 B:2 C:3 D:4
    m_fifo_pin[0].pin = GPIO_PIN(1,1,3);
    m_fifo_pin[1].pin = GPIO_PIN(1,1,2);
    m_fifo_pin[2].pin = GPIO_PIN(1,2,0);
    m_fifo_pin[3].pin = GPIO_PIN(1,1,1);
    m_fifo_pin[4].pin = GPIO_PIN(1,1,5);
    m_fifo_pin[5].pin = GPIO_PIN(1,1,0);
    m_fifo_pin[6].pin = GPIO_PIN(1,1,4);
    m_fifo_pin[7].pin = GPIO_PIN(1,3,2);
    m_fifo_pin[8].pin = GPIO_PIN(1,3,4);
    m_fifo_pin[9].pin = GPIO_PIN(1,4,3);
    m_fifo_pin[10].pin = GPIO_PIN(1,4,4);
    m_fifo_pin[11].pin = GPIO_PIN(1,4,5);
    m_fifo_pin[12].pin = GPIO_PIN(1,3,3);
    m_fifo_pin[13].pin = GPIO_PIN(1,3,5);
    m_fifo_pin[14].pin = GPIO_PIN(1,3,6);
    m_fifo_pin[15].pin = GPIO_PIN(1,3,7);
    m_fifo_pin[16].pin = GPIO_PIN(1,2,3);
    m_fifo_pin[17].pin = GPIO_PIN(1,2,6);
    m_fifo_pin[18].pin = GPIO_PIN(1,3,0);
    m_fifo_pin[19].pin = GPIO_PIN(1,2,7);
    m_fifo_pin[20].pin = GPIO_PIN(1,4,2);
    m_fifo_pin[21].pin = GPIO_PIN(1,4,6);
    m_fifo_pin[22].pin = GPIO_PIN(1,3,1);
    m_fifo_pin[23].pin = GPIO_PIN(1,4,1);
    m_fifo_pin[24].pin = GPIO_PIN(1,4,0);
    m_fifo_pin[25].pin = GPIO_PIN(1,2,2);
    m_fifo_pin[26].pin = GPIO_PIN(1,1,6);
    m_fifo_pin[27].pin = GPIO_PIN(1,1,7);
    m_fifo_pin[28].pin = GPIO_PIN(1,2,1);
    m_fifo_pin[29].pin = GPIO_PIN(1,2,4);
    m_fifo_pin[30].pin = GPIO_PIN(1,2,5);
    m_fifo_pin[31].pin = GPIO_PIN(1,4,7);

    m_fifo_ff_A2.pin = GPIO_PIN(3,3,6);
    m_fifo_R_A.pin = GPIO_PIN(3,4,1);
    m_fifo_rt_A.pin = GPIO_PIN(3,4,2);
    m_fifo_ef_A2.pin = GPIO_PIN(3,4,3);

    m_fifo_ff_B2.pin = GPIO_PIN(0,3,0);
    m_fifo_R_B.pin = GPIO_PIN(0,3,6);
    m_fifo_EF_B2.pin = GPIO_PIN(0,2,7);
    m_fifo_RT_B.pin = GPIO_PIN(0,3,4);

    m_save_led.pin = GPIO_PIN(4,2,0);//gpio4_B0
}
FifoReader::~FifoReader()
{
   // gpio_all_close();
   // gpio_unregister();
}



int FifoReader::gpio_register()
{
    int fd = -1;
    fd = open("/sys/class/gpio/export", O_WRONLY);
    if(fd == -1){
        printf("ERR: Radio hard reset pin open error.\n");
        return ERR_REGISTER_GPIO;
     }
#if 0
        std::cout << "44 "<<sizeof("44")<<std::endl;
        write(fd,"44",sizeof("44"));
#else
    for(int i=0;i<32;i++){
        std::string pin_num = std::to_string(m_fifo_pin[i].pin);
        std::cout << pin_num<<"size"<<pin_num.size()<<std::endl;
        write(fd,pin_num.c_str(),pin_num.size());
        usleep(1000);
    }
#endif
    write(fd,std::to_string(m_fifo_ff_A2.pin).c_str(),std::to_string(m_fifo_ff_A2.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_R_A.pin).c_str(),std::to_string(m_fifo_R_A.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_rt_A.pin).c_str(),std::to_string(m_fifo_rt_A.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_ef_A2.pin).c_str(),std::to_string(m_fifo_ef_A2.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_ff_B2.pin).c_str(),std::to_string(m_fifo_ff_B2.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_R_B.pin).c_str(),std::to_string(m_fifo_R_B.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_EF_B2.pin).c_str(),std::to_string(m_fifo_EF_B2.pin).size());
    usleep(1000);
    write(fd,std::to_string(m_fifo_RT_B.pin).c_str(),std::to_string(m_fifo_RT_B.pin).size());
    usleep(1000);

    //mcu 5 pin
    for(int i=0;i<5;i++){
        std::string pin_num = std::to_string(m_mcu_CMN[i].pin);
        write(fd,pin_num.c_str(),pin_num.size());
        usleep(1000);
    }
    close(fd);
    return 0;
}

int FifoReader::gpio_unregister()
{
    int fd = -1;
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if(fd == -1){
        printf("ERR: Radio hard reset pin open error.\n");
        return ERR_REGISTER_GPIO;
     }
    for(int i=0;i<32;i++){
        std::string pin_num = std::to_string(m_fifo_pin[i].pin);
        write(fd,pin_num.c_str(),pin_num.size()-1);
    }

    write(fd,std::to_string(m_fifo_ff_A2.pin).c_str(),std::to_string(m_fifo_ff_A2.pin).size());
    write(fd,std::to_string(m_fifo_R_A.pin).c_str(),std::to_string(m_fifo_R_A.pin).size());
    write(fd,std::to_string(m_fifo_rt_A.pin).c_str(),std::to_string(m_fifo_rt_A.pin).size());
    write(fd,std::to_string(m_fifo_ef_A2.pin).c_str(),std::to_string(m_fifo_ef_A2.pin).size());

    write(fd,std::to_string(m_fifo_ff_B2.pin).c_str(),std::to_string(m_fifo_ff_B2.pin).size());
    write(fd,std::to_string(m_fifo_R_B.pin).c_str(),std::to_string(m_fifo_R_B.pin).size());
    write(fd,std::to_string(m_fifo_EF_B2.pin).c_str(),std::to_string(m_fifo_EF_B2.pin).size());
    write(fd,std::to_string(m_fifo_RT_B.pin).c_str(),std::to_string(m_fifo_RT_B.pin).size());

    //mcu 5 pin
    for(int i=0;i<5;i++){
        std::string pin_num = std::to_string(m_mcu_CMN[i].pin);
        write(fd,pin_num.c_str(),pin_num.size());
    }
    close(fd);
    return 0;
}


int FifoReader::gpio_init()
{
    gpio_register();
    std::cout <<"gpio_register succeed"<< std::endl;

    gpio_direct_init();

    std::cout <<"gpio_direct_init succeed"<< std::endl;
    gpio_all_open();
    return 0;
}
int FifoReader::fifo_init()
{
    gpio_init();

    set_single_gpio_out(&m_fifo_R_A,1);
    set_single_gpio_out(&m_mcu_CMN[0],1);
    set_single_gpio_out(&m_mcu_CMN[1],1);

    set_single_gpio_out(&m_fifo_R_B,1);
    set_single_gpio_out(&m_fifo_rt_A,0);
    set_single_gpio_out(&m_fifo_RT_B,0);

    sleep(1);
    set_single_gpio_out(&m_mcu_CMN[0],1);
    set_single_gpio_out(&m_fifo_rt_A,1);
    set_single_gpio_out(&m_fifo_RT_B,1);
    sleep(1);
    return 0;
}
int FifoReader::set_fifo_r_out(int out)
{

}

bool FifoReader::get_fifo_A_value(uint16_t *value)
{
//#ifdef USE_SIM
//    static int index=0;
//    if(index % 18==0){
//        index=0;
//        *value = 0xA55A;
//    }else{
//        *value = (uint16_t)index;
//    }
//    index++;
//    return true;
//#else
    int gpio_EF_A = get_single_gpio_in(&m_fifo_ef_A2);
    if(gpio_EF_A == 1){//fifo has data
        set_single_gpio_out(&m_fifo_R_A,0);
        std::this_thread::sleep_for(std::chrono::microseconds(2));
        uint16_t value_A = read_fifo_A();
        *value = value_A;
        set_single_gpio_out(&m_fifo_R_A,1);
    }
    return gpio_EF_A == 1;
//#endif
}
bool FifoReader::get_fifo_B_value(uint16_t *value)
{
//#ifdef USE_SIM
//    static int index=0;
//    if(index % 18 == 0){
//        index=0;
//        *value = 0xA55A;
//    }else{
//        *value = (uint16_t)index;
//    }
//    index++;
//    return true;
//#else
    int gpio_ef_B = get_single_gpio_in(&m_fifo_EF_B2);
    if(gpio_ef_B == 1){
        set_single_gpio_out(&m_fifo_R_B,0);
        std::this_thread::sleep_for(std::chrono::microseconds(2));
        uint16_t value_B = read_fifo_B();
        *value = value_B;
        set_single_gpio_out(&m_fifo_R_B,1);
    }
    return gpio_ef_B == 1;
//#endif
}
unsigned short FifoReader::read_fifo_A()
{
    unsigned short value = 0;
    for(int i=0;i<16;i++){
        value |= (get_single_gpio_in(&m_fifo_pin[i])&0x01)<<i;
    }
    return value;
}
unsigned short FifoReader::read_fifo_B()
{
    unsigned short value = 0;
    for(int i=0;i<16;i++){
        int pin_bit = get_single_gpio_in(&m_fifo_pin[i+16])&0x01;
        value |= (pin_bit)<<i;
      //  usleep(2);
    //    std::cout << "pin_bit:"<<std::to_string(i)<<" "<<std::to_string(pin_bit)<<std::endl;
    }
    return value;
}

int FifoReader::config_single_gpio_out(int pin,bool isOut)
{
    int fd = -1;
    std::string sys_gpio = "/sys/class/gpio/gpio"+std::to_string(pin)+"/direction";
    //设置端口方向/sys/class/gpio/gpio137# echo out > direction
    fd = open(sys_gpio.c_str(), O_WRONLY);
    if(fd == -1){
        printf("ERR: Radio hard reset pin direction open error %d.\n",pin);
        return ERR_GPIO_SET_DIR;
    }
    if(isOut){
        write(fd, GPIO_DIR_OUT, sizeof(GPIO_DIR_OUT));
    }else{
        write(fd, GPIO_DIR_IN, sizeof(GPIO_DIR_IN));
    }
    close(fd);
    return 0;
}
int FifoReader::gpio_direct_init()
{
    std::cout << "start init direct fifo pin"<<std::endl;
    for(int i=0;i<32;i++){
        config_single_gpio_out(m_fifo_pin[i].pin,false);
        usleep(1000);
    }
    config_single_gpio_out(m_fifo_ff_A2.pin,false);
        usleep(1000);
    config_single_gpio_out(m_fifo_ff_B2.pin,false);
        usleep(1000);
    config_single_gpio_out(m_fifo_ef_A2.pin,false);
        usleep(1000);
    config_single_gpio_out(m_fifo_EF_B2.pin,false);
        usleep(1000);
    config_single_gpio_out(m_fifo_rt_A.pin,true);//reset input?
        usleep(1000);
    config_single_gpio_out(m_fifo_RT_B.pin,true);
        usleep(1000);

    config_single_gpio_out(m_fifo_R_A.pin,true);
        usleep(1000);
    config_single_gpio_out(m_fifo_R_B.pin,true);
        usleep(1000);

    std::cout << "start init mcu pin"<<std::endl;
    //mcu
    config_single_gpio_out(m_mcu_CMN[0].pin,true);
    config_single_gpio_out(m_mcu_CMN[1].pin,true);
    config_single_gpio_out(m_mcu_CMN[2].pin,false);
    config_single_gpio_out(m_mcu_CMN[3].pin,true);
    config_single_gpio_out(m_mcu_CMN[4].pin,true);

    return 0;
}
int FifoReader::gpio_all_open()
{
    //open fd
    for(int i=0;i<32;i++){
        std::string value_path = "/sys/class/gpio/gpio"+std::to_string(
                    m_fifo_pin[i].pin)+"/value";
        m_fifo_pin[i].fd = open(value_path.c_str(), O_RDWR);
        if(m_fifo_pin[i].fd == -1){
            std::cout << "open pin "<<m_fifo_pin->pin<< " failed"<<std::endl;
        }
    }
    std::string value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_ff_A2.pin)+"/value";
    m_fifo_ff_A2.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_ff_B2.pin)+"/value";
    m_fifo_ff_B2.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_ef_A2.pin)+"/value";
    m_fifo_ef_A2.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_EF_B2.pin)+"/value";
    m_fifo_EF_B2.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_rt_A.pin)+"/value";
    m_fifo_rt_A.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_RT_B.pin)+"/value";
    m_fifo_RT_B.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_R_A.pin)+"/value";
    m_fifo_R_A.fd = open(value_path.c_str(), O_RDWR);
    value_path = "/sys/class/gpio/gpio"+std::to_string(
                 m_fifo_R_B.pin)+"/value";
    m_fifo_R_B.fd = open(value_path.c_str(), O_RDWR);
    //mcu
    for(int i=0;i<5;i++){
        std::string value_path = "/sys/class/gpio/gpio"+
                std::to_string(m_mcu_CMN[i].pin)+"/value";
        m_mcu_CMN[i].fd = open(value_path.c_str(), O_RDWR);
        if(m_mcu_CMN[i].fd == -1){
            std::cout << "open pin "<<m_fifo_pin->pin<< " failed"<<std::endl;
        }else{
            std::cout << "open pin "<<m_mcu_CMN[i].pin<< " fd:"<<m_mcu_CMN[i].fd<<std::endl;
        }
    }
    return 0;
}
int FifoReader::gpio_all_close()
{
    for(int i=0;i<32;i++){
        close(m_fifo_pin[i].fd);
    }
    close(m_fifo_ff_A2.fd);
    close(m_fifo_ff_B2.fd);
    close(m_fifo_ef_A2.fd);
    close(m_fifo_EF_B2.fd);
    close(m_fifo_rt_A.fd);
    close(m_fifo_RT_B.fd);
    close(m_fifo_R_A.fd);
    close(m_fifo_R_A.fd);
    //mcu
    for(int i=0;i<5;i++){
        close(m_mcu_CMN[i].fd);
    }
    return 0;
}
int FifoReader::set_single_gpio_out(GpioInfo *pin,int out)
{
    if(out){
        write(pin->fd,"1",sizeof ("1"));
    }else{
        write(pin->fd,"0",sizeof ("0"));
    }
    return 0;
}

int FifoReader::get_single_gpio_in(GpioInfo *pin)
{
    unsigned char a='0';
    int ret = -1;
#if 0
//    std::string value_path = "/sys/class/gpio/gpio"+std::to_string(pin->pin)+"/value";
//    int fd = open(value_path.c_str(), O_RDWR);
//    ret = read(fd,&a,sizeof(a));
#else
    lseek(pin->fd,0,SEEK_SET);
    ret = read(pin->fd,&a,sizeof(a));
#endif
    if(ret == -1){
        std::cout << "get single gpio in err"<<pin->pin <<std::endl;
    }
 //   std::cout << "read pin"<<std::to_string(pin->pin)<<a<<" "<<std::to_string(pin->fd)<<std::endl;
 //   std::cout << std::to_string((int)a)<<std::endl;
  //  close(fd);
    return a=='0'?0:1;
}
