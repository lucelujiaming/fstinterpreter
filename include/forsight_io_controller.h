#ifndef FORSIGHT_IO_CONTROLLER_H
#define FORSIGHT_IO_CONTROLLER_H
#include "forsight_inter_control.h"
#include "forsight_basint.h"


#define UI_UO_NUM                   13
#define IO_NAMES   "ai;ao;di;do;ri;ro;si;so;ui;uo"

eval_value forgesight_get_io_status(
			struct thread_control_block* objThreadCntrolBlock, char *name);
int forgesight_set_io_status(
			struct thread_control_block* objThreadCntrolBlock, char *name, eval_value& valueStart);

#endif
