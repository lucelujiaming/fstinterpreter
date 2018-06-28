#ifndef OP_SHMI_
#define OP_SHMI_

#include "forsight_peterson.h"
#ifndef WIN32
#include <linux/types.h>
#endif
#include "forsight_err_shmi.h"
#include "forsight_regs_shmi.h"

#define REG_SHMI_SIZE (sizeof(pr_shmi_t) * PR_NUM_MAX \
	                 + sizeof(sr_shmi_t) * SR_NUM_MAX \
                     + sizeof(r_shmi_t) * R_NUM_MAX \
                     + sizeof(mr_shmi_t) * MR_NUM_MAX \
                     + sizeof(uf_shmi_t) * UF_NUM_MAX \
                     + sizeof(tf_shmi_t) * TF_NUM_MAX \
                     + sizeof(pl_shmi_t) * PR_NUM_MAX )

#define READ_REG_SHMI READ_PTS
#define WRITE_REG_SHMI WRITE_PTS

#define OK_SHMI OK_PTS

struct shmi_info {
#ifdef WIN32
    void *ptr;
#else
    char *ptr;
#endif
    size_t offset;
    size_t size;
    struct peterson pts;
};

int initShmi(int size);

int transData(void *ptr, struct shmi_info *info, uint8_t op);

#endif
