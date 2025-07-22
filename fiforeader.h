#ifndef FIFOREADER_H
#define FIFOREADER_H
#include <stdint.h>

struct GpioInfo{
    int pin;
    int fd;
};

class FifoReader
{
public:
    FifoReader();
    ~FifoReader();
    int test_gpio();
    //return succeed
    bool get_fifo_A_value(uint16_t *value);
    bool get_fifo_B_value(uint16_t *value);

    int gpio_init();
    int fifo_init();
    int set_single_gpio_out(GpioInfo *pin,int out);//
    int get_single_gpio_in(GpioInfo *pin);

    unsigned short read_fifo_A();
    unsigned short read_fifo_B();

    int set_fifo_r_out(int out);
    //link to mcu
    GpioInfo m_mcu_CMN[5];// 4 for transf_led

    GpioInfo m_fifo_pin[32];
    GpioInfo m_fifo_ff_A2;
    GpioInfo m_fifo_R_A;
    GpioInfo m_fifo_rt_A;
    GpioInfo m_fifo_ef_A2;
    GpioInfo m_fifo_ff_B2;
    GpioInfo m_fifo_R_B;
    GpioInfo m_fifo_EF_B2;
    GpioInfo m_fifo_RT_B;

    GpioInfo m_save_led;//save led
private:
    int gpio_register();
    int gpio_unregister();
    int gpio_direct_init();
    int gpio_all_open();
    int gpio_all_close();


    int config_single_gpio_out(int pin, bool isOut);

};

#endif // FIFOREADER_H
