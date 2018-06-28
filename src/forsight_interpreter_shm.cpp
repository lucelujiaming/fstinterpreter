#include <string.h>

#ifndef WIN32
#include <unistd.h>
#include <sys/mman.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>  
#include <fcntl.h>
#include "forsight_interpreter_shm.h"

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include <map>
#include <string>

using namespace std;

#define MEM_FALSE 0
#define MEM_TRUE 1
#define MEM_WRITE_TURN 11
#define MEM_READ_TURN 12
#define MEM_READ_ALREADY 0
#define MEM_WRITE_ALREADY 1

map<string, ShmData> g_mapper;
FuncTable table;

int createShm(const char* name, int size)
{
#ifdef WIN32
	FILE * fd = fopen(name, "w");
    if (fd == 0)
#else
    int fd = shm_open(name,  O_CREAT|O_RDWR, 00777);
    if (fd == -1)
#endif
    {
		perror("failed on creating sharedmem\n");
	#ifdef WIN32
		fclose(fd);
	#else
		close(fd);
	#endif
        return -1;
    }
    ShmData data;
#ifdef WIN32
	printf("ftruncate(fd, size);\n");
    data.ptr = malloc(size);
#else
    ftruncate(fd, size);
    data.ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (data.ptr == MAP_FAILED) 
    {
	#ifdef WIN32
		fclose(fd);
	#else
		close(fd);
	#endif
        perror("failed on mapping process sharedmem\n");
        return -1;
    }
#endif
 
    memset(data.ptr, 0, size);
    g_mapper.insert(map<string, ShmData>::value_type(name, data));

    return 0;
}

int openShm(const char* name, int size)
{
#ifdef WIN32
	FILE * fd = fopen(name, "w");
    if (fd == 0)
#else
    int fd = shm_open(name,  O_RDWR, 00777);
    // printf("shm_open(%s) return %d\n",name, fd);
    if (fd == -1)
#endif
    {
        perror("failed on creating sharedmem\n");
	#ifdef WIN32
		fclose(fd);
	#else
		close(fd);
	#endif
        return -1;
    }
    ShmData data;
#ifdef WIN32
	printf("ftruncate(fd, size);\n");
    data.ptr = malloc(size);
#else
    data.ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (data.ptr == MAP_FAILED) 
    {
	#ifdef WIN32
		fclose(fd);
	#else
		close(fd);
	#endif
        perror("failed on mapping process sharedmem\n");
        return -1;
    }
#endif

    g_mapper.insert(map<string, ShmData>::value_type(name, data));

    return 0;   
}

ShmData *getShm(const char* name)
{
    map<string, ShmData>::iterator it = g_mapper.find(name);
    if (it == g_mapper.end())
        return NULL;

    ShmData *data = &it->second;
    return data;
}

void readShm(const char* name, int offset,  void*buffer, int size)
{
    void* ptr = getShm(name)->ptr;

    memcpy(buffer, (char*)ptr+offset, size);
}


void writeShm(const char* name, int offset, void*buffer, int size)
{
    void* ptr = getShm(name)->ptr;
    memcpy((char*)ptr+offset, buffer, size); 
}

bool tryWrite(const char* name, int offset, void*buffer, int size)
{
    ShmData *data = getShm(name);
    if (data == NULL)
    {
        printf("can't find name:%s\n", name);
        return false;
    }
    volatile unsigned int *ptr_read, *ptr_write, *ptr_turn, *ptr_latest;
    AcceFlag *flag = (AcceFlag *)&(((FuncTable*)data->ptr)->offset_flag);
    ptr_read = &(flag->read);
    ptr_write = &(flag->write);
    ptr_turn = &(flag->turn);
    ptr_latest = &(flag->latest);

    // printf("read:%d, write:%d, turn:%d, latest:%d\n", flag->read, flag->write, flag->turn, flag->latest);

    if (*(ptr_latest) == MEM_WRITE_ALREADY){return false;}

    if (*(ptr_read) == MEM_FALSE && *(ptr_write)  == MEM_FALSE)
    {
        *(ptr_write) = MEM_TRUE;  //Indicating that it is going to be the writing state
        *(ptr_turn) = MEM_WRITE_TURN; //mark that which process is using the memory
        if (*(ptr_read) == MEM_TRUE &&  *(ptr_turn) != MEM_WRITE_TURN )
        {
           //  printf("WARN: ptr_read:%d, ptr_turn:%d, \n", ptr_read, ptr_turn);   
            *(ptr_write) = MEM_FALSE; 
            return false;
        } //check if writing process is working on.
        memcpy(((FuncTable*)data->ptr)->param + offset, buffer, size);
        *(ptr_latest) = MEM_WRITE_ALREADY;
        *(ptr_write) = MEM_FALSE;     
    } 
    else
    {
    //    printf("WARN: ptr_read:%d, ptr_write:%d, \n", ptr_read, ptr_write);   
        return false;
    }
    return true;
}


bool lockRead(const char* name, int offset, void*buffer, int size)
{
    ShmData *data = getShm(name);
    if (data == NULL)
        return false;
    volatile unsigned int *ptr_read, *ptr_write, *ptr_turn, *ptr_latest;
    AcceFlag *flag = (AcceFlag *)&(((FuncTable*)data->ptr)->offset_flag);
    ptr_read = &(flag->read);
    ptr_write = &(flag->write);
    ptr_turn = &(flag->turn);
    ptr_latest = &(flag->latest);
   // printf("read:%d, write:%d, turn:%d, latest:%d\n", flag->read, flag->write, flag->turn, flag->latest);

    if (*(ptr_latest) == MEM_READ_ALREADY){return false;}

    if (*(ptr_read) == MEM_FALSE && *(ptr_write)  ==  MEM_FALSE)
    {
        *(ptr_read) = MEM_TRUE;  //Indicating that it is going to be the reading state
        *(ptr_turn) = MEM_READ_TURN; //mark that which process is using the memory
        if (*(ptr_write) == MEM_TRUE &&  *(ptr_turn) != MEM_READ_TURN )
        {
            *(ptr_read) = MEM_FALSE; 
            return false;
        } //check if writing process is working on.
        memcpy(buffer, ((FuncTable*)data->ptr)->param + offset, size);
        *(ptr_latest) = MEM_READ_ALREADY;
       // *(ptr_read) = MEM_FALSE;
    }

    return true; 
}

bool unlockRead(const char* name)
{
    ShmData *data = getShm(name);
    if (data == NULL)
        return false;
//    volatile unsigned int *ptr_read, *ptr_write, *ptr_turn, *ptr_latest;
    AcceFlag *flag = (AcceFlag *)&(((FuncTable*)data->ptr)->offset_flag);
    if (flag->read == MEM_TRUE)
        flag->read = MEM_FALSE;
	return true;
}

bool tryRead(const char* name, int offset, void*buffer, int size)
{
    ShmData *data = getShm(name);
    if (data == NULL)
    {
		printf("getShm(%s) failed \n", name);
        return false;
    }
    volatile unsigned int *ptr_read, *ptr_write, *ptr_turn, *ptr_latest;
    AcceFlag *flag = (AcceFlag *)&(((FuncTable*)data->ptr)->offset_flag);
    ptr_read = &(flag->read);
    ptr_write = &(flag->write);
    ptr_turn = &(flag->turn);
    ptr_latest = &(flag->latest);
    // printf("read:%d, write:%d, turn:%d, latest:%d\n",
    //      flag->read, flag->write, flag->turn, flag->latest);

    if (*(ptr_latest) == MEM_READ_ALREADY){return false;}

    if (*(ptr_read) == MEM_FALSE && *(ptr_write)  ==  MEM_FALSE)
    {
        *(ptr_read) = MEM_TRUE;  //Indicating that it is going to be the reading state
        *(ptr_turn) = MEM_READ_TURN; //mark that which process is using the memory
        if (*(ptr_write) == MEM_TRUE &&  *(ptr_turn) != MEM_READ_TURN )
        {
            *(ptr_read) = MEM_FALSE; 
            return false;
        } //check if writing process is working on.
        memcpy(buffer, ((FuncTable*)data->ptr)->param + offset, size);
        *(ptr_latest) = MEM_READ_ALREADY;
        *(ptr_read) = MEM_FALSE;
    }

    return true;    
}

bool isInstructionEmpty(const char* name)
{
    ShmData *data = getShm(name);
    if (data == NULL)
    {
        printf("can't find name:%s\n", name);
        return false;
    }
    volatile unsigned int *ptr_read, *ptr_write; // , *ptr_turn, *ptr_latest;
    AcceFlag *flag = (AcceFlag *)&(((FuncTable*)data->ptr)->offset_flag);
    ptr_read = &(flag->read);
    ptr_write = &(flag->write);
    if (*(ptr_read) == MEM_FALSE && *(ptr_write)  == MEM_FALSE)
        return true;

    return false;
}

