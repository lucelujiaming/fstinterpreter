#include "forsight_op_shmi.h"
#include "forsight_peterson.h"
#include "forsight_err_shmi.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <stdlib.h>

static int fdRegShmi;
static void *ptrRegShmi;

int initShmi() {
	int err = OK_SHMI;
#ifdef WIN32
	FILE * errFile = fopen("reg_shmi", "w");
#else
	err = shm_open("reg_shmi", O_CREAT|O_RDWR, 00777);
#endif
	if(err)
		printf("REG_SHMI: init failed fdRegShmi = %d\n", fdRegShmi);
	
#ifdef WIN32
	ptrRegShmi = malloc(REG_SHMI_SIZE);
	fdRegShmi  = REG_SHMI_SIZE ; 
#else
	ptrRegShmi = mmap(NULL, REG_SHMI_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, fdRegShmi, 0);
#endif

	return err;

}

int transData(void *ptr, struct shmi_info *info, uint8_t op) {
#ifdef WIN32
    char *src;
    char *dst;
#else
    void *src;
    void *dst;
#endif

    switch (op) {
    case READ_REG_SHMI:
#ifdef WIN32
        dst = (char *)ptr;
        src = (char *)info->ptr + info->offset;
#else
        dst = ptr;
        src = (void *)(info->ptr + info->offset);
#endif
        break;
    case WRITE_REG_SHMI:
#ifdef WIN32
        dst = (char *)info->ptr + info->offset;
        src = (char *)ptr;
#else
        dst = (void *)(info->ptr + info->offset);
        src = ptr;
#endif
        break;
    default:
        printf("Error: wrong operation, operation = 0x%u\n", (uint8_t)op);
        return WRONG_OPERATION_SHMI;
    }

    tryLockPts(&info->pts, op);
#ifndef WIN32
    memcpy(dst, src, info->size);
#endif
    unlockPts(&info->pts, op);

    return SUCCESS_SHMI;
}
