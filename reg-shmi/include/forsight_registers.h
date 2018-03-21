#ifndef FORSIGHT_REGISTERS_H
#define FORSIGHT_REGISTERS_H
#include "../../forsight_inter_control.h"
#include "../../forsight_basint.h"

#define REGSITER_NAMES   "pr;sr;r;mr;uf;tf;pl"

int forgesight_get_register(struct thread_control_block* objThreadCntrolBlock, 
							char *name, void * value);
int forgesight_set_register(struct thread_control_block* objThreadCntrolBlock, 
							char *name, void * value);

#endif
