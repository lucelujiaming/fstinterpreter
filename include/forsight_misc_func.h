#ifndef FORSIGHT_MISC_FUNC_H
#define FORSIGHT_MISC_FUNC_H

#include "forsight_basint.h"
#include "forsight_innercmd.h"
#include "forsight_innerfunc.h"
#include "forsight_inter_control.h"

#define STR_MISC_FUNC    "substr;replace;replaceall;"
#define STR_SUBSTR_FUNC        "substr"
#define STR_REPLACE_FUNC       "replace"
#define STR_REPLACEALL_FUNC    "replaceall"

void deal_misc_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result);

#endif
