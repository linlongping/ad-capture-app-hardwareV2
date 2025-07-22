#ifndef CANTASK_H
#define CANTASK_H
#include "candevice.h"
#include <stdint.h>
#include <thread>
#include <mutex>
#include "utils/effectvaluecalculator.h"

struct AD_KB{
    float k=1.0;
    float b=0;
};
class CanTask
{
public:
    CanTask();
    ~CanTask();
    void set_data(int16_t *data,int data_len);
    void enable_send(bool send);

    void load_kb();
    void save_kb();

    int16_t m_data[48];
    AD_KB m_ad_kb[48];
private:
    CanDevice *m_pCanDev = nullptr;

    std::thread *m_pThread = nullptr;
    void thread_send_func();
    std::thread *m_pRecvThread = nullptr;
    void thread_recv_func();

    bool quit = false;
    std::mutex m_mutex;

    bool m_send = false;

    EffectValueCalculator m_effect_buf[15];//
};

#endif // CANTASK_H
