#ifndef FORSIGHT_LAUNCH_CODE_STARTUP_H
#define FORSIGHT_LAUNCH_CODE_STARTUP_H

#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h"

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include <vector>
#include <map>
#include <stack>
using namespace std;

#include "interpreter_common.h"
#include "forsight_eval_type.h"

#ifdef WIN32
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#include "motion_plan_arm_group.h"
#endif

#ifdef WIN32
unsigned __stdcall launch_code_thread(void* arg);
#else
void* launch_code_thread(void* arg);
#endif
void forgesight_simulate_launch_config_values();

bool launch_code_thread_create(void * args);
void launch_code_thread_destroy();	 

#endif

