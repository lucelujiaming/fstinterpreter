#include "reg-shmi/forsight_op_shmi.h"
#include "reg-shmi/forsight_peterson.h"
#include "reg-shmi/forsight_err_shmi.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <stdlib.h>

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

	printf("tryLockPts: operation = 0x%u\n", (uint8_t)op);
    // tryLockPts(&info->pts, op);
	printf("tryLockPts Over: operation = 0x%u\n", (uint8_t)op);
// #ifndef WIN32
    memcpy(dst, src, info->size);
// #endif
	printf("unlockPts: operation = 0x%u\n", (uint8_t)op);
    // unlockPts(&info->pts, op);
	printf("unlockPts Over: operation = 0x%u\n", (uint8_t)op);

    return SUCCESS_SHMI;
}
