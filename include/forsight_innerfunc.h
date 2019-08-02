#ifndef FORSIGHT_INNERFUNC_H
#define FORSIGHT_INNERFUNC_H
#include "forsight_inter_control.h"

#define PARAM_NUM_ZERO    0
#define PARAM_NUM_ONE     1
#define PARAM_NUM_TWO     2
#define PARAM_NUM_THR     3
#define PARAM_NUM_FOUR    4
#define PARAM_NUM_FIVE    5
#define PARAM_NUM_MAX     5

#define FUNC_FKJ_ONE    "fkj_one"
#define FUNC_FKJ_TWO    "fkj_two"

#define RAD2DEG(x) ((x)*180./PI)  // Convert radians to angles
#define DEG2RAD(x) ((x)*PI/180.)  // Convert angle to radians

int find_internal_func(char *s);
int get_internal_func_count();
int get_func_params_num(int iIdx);
bool call_internal_func(int index, eval_value *result, 
		char * valFirst = 0, char * valSecond = 0, char * valThird = 0, 
		char * valFourth = 0, char * valFiveth = 0);

// White Test
int IsLeapYear(int year);
int GetMaxDay(int year,int month);
int GetDays(int year,int month,int day);
int Stein_GCD(int x, int y);


#endif
