#ifndef SHM_H_
#define SHM_H_

#include <stdio.h>
#ifndef WIN32
#include <sys/shm.h>
#include <semaphore.h>
#endif

typedef struct _ShmData
{
    void*   ptr;
    int     size;
}ShmData;

typedef struct _AcceFlag
{
    unsigned int read;
    unsigned int write;
    unsigned int turn;
    unsigned int latest;
}AcceFlag;


typedef struct _FuncTable
{ 
    unsigned int offset;
    AcceFlag offset_flag;
    unsigned int size;
#ifdef WIN32
    char * param;
#else
    char param[0];
#endif
}FuncTable;


#define SHM_INTPRT_CMD      ("interpreter_command")
#define SHM_INTPRT_STATUS   ("interpreter_status")
// Lujiaming add at 0323
#define SHM_REG_IO_INFO        ("register_io_info")
// Lujiaming add at 0323 end

// Lujiaming add at 0514
#define SHM_CHG_REG_LIST_INFO  ("chg_reg_list_info")
// Lujiaming add at 0514 end

#define SHM_CTRL_CMD        ("controller_command")
#define SHM_CTRL_STATUS     ("constroller_status")

// Lujiaming add at 0528
#define SHM_INTPRT_DST      ("interpreter_cmd_dst")
// Lujiaming add at 0528 end

#define SHM_INTPRTCMD_SIZE  (8192)
#define SHM_INTPRTSTATUS_SIZE (1024)
#define SHM_CTRLCMD_SIZE    (1024)
#define SHM_CTRLSTATUS_SIZE (1024)


int createShm(const char* name, int size);
int openShm(const char* name, int size);
void readShm(const char* name, int offset,  void*buffer, int size);
void writeShm(const char* name, int offset, void*buffer, int size);
bool lockRead(const char* name, int offset, void*buffer, int size);
bool unlockRead(const char* name);
bool tryRead(const char* name, int offset, void*buffer, int size);
bool tryWrite(const char* name, int offset, void*buffer, int size);
bool isInstructionEmpty(const char* name);


#endif
