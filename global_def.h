#ifndef GLOBAL_DEF_H
#define GLOBAL_DEF_H

#define DATAPIPE 37
#define DATAPACK_WRITE 50
#define SLEEP_TIME do{ usleep(5000);}while(0);
#define FILE_MAX   300
#define FILE_TO_FILE_COST_SEC   60
#define VERSION do{printf("This version is JLY_test1.15.03");}while(0);

#define  SERVERPORT    8885
struct msg
{
    long mytype;
    unsigned short AD[DATAPIPE];

};

extern int msqid2;

#endif // GLOBAL_DEF_H
