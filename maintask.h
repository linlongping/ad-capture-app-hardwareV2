#ifndef MAINTASK_H
#define MAINTASK_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define DATAPIPE 18
#define DATAPACK_WRITE 50
#define SLEEP_TIME do{ usleep(5000);}while(0);
#define FILE_MAX   300
#define FILE_TO_FILE_COST_SEC   60
#define VERSION do{printf("This version is JLY_test1.15.03");}while(0);

struct msg
{
    long mytype;
    unsigned short AD[DATAPIPE];

};
extern int msqid2;
void main_task();
#endif // MAINTASK_H
