#ifndef FORSIGHT_INNERCMD_H
#define FORSIGHT_INNERCMD_H
#include "forsight_inter_control.h"
#include "forsight_basint.h"

#define   TIMER_START_VALUE      0.0
#define   TIMER_STOP_VALUE       1.0
#define   TIMER_RESET_VALUE      2.0

#define   END_COMMND_RET      0x400    // 1024
extern vector<string> g_vecXPath ;

int find_internal_cmd(char *s);
int call_internal_cmd_exec_sub_thread(int index);
int call_internal_cmd(int index, int iLineNum, 
					  struct thread_control_block* objThreadCntrolBlock);
void mergeImportXPathToProjectXPath(
		struct thread_control_block* objThreadCntrolBlock, char * fname);
void generateXPathVector(char * fname);
int getLineNumFromXPathVector(char * xPath);
int getMaxLineNum();

int set_global_TF(int iLineNum, int iTFNum, struct thread_control_block* objThreadCntrolBlock);
int set_global_UF(int iLineNum, int iUFNum, struct thread_control_block* objThreadCntrolBlock);
int set_OVC(int iLineNum, double dUFNum, struct thread_control_block* objThreadCntrolBlock);
int set_OAC(int iLineNum, double dUFNum, struct thread_control_block* objThreadCntrolBlock);

int execute_Timer(struct thread_control_block* objThreadCntrolBlock, char *vname, eval_value& value);

#endif
