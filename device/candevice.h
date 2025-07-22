#ifndef CANDEVICE_H
#define CANDEVICE_H
#include <stdint.h>
#include <mutex>

class CanDevice
{
public:
    CanDevice(int id);

    int can_write(uint32_t can_id, uint8_t *data, uint32_t len);

    int can_read(uint32_t &can_id, uint8_t *data, uint32_t &len);
private:
    int can_fd;
 //   std::mutex m_mutex;//读和写可能不在同一个线程
};

#endif // CANDEVICE_H
