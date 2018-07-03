#ifndef FORSIGHT_INNERFUNC_H
#define FORSIGHT_INNERFUNC_H
#include "forsight_inter_control.h"

#define PARAM_NUM_ONE     1
#define PARAM_NUM_TWO     2
#define PARAM_NUM_THR     3
#define PARAM_NUM_MAX     3

int find_internal_func(char *s);
int get_func_params_num(int iIdx);
bool call_internal_func(int index, eval_value *result, 
		char * valFirst, char * valSecond = 0, char * valThird = 0);

#endif
