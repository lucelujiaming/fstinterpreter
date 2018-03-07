#ifndef FORSIGHT_INNERCMD_H
#define FORSIGHT_INNERCMD_H
#include "forsight_inter_control.h"
#include "forsight_basint.h"

#define   END_COMMND_RET      0x400    // 1024

int find_internal_cmd(char *s);
int call_internal_cmd_exec_sub_thread(int index);
int call_internal_cmd(int index, int iLineNum, 
					  struct thread_control_block* objThreadCntrolBlock);
void mergeImportXPathToProjectXPath(
		struct thread_control_block* objThreadCntrolBlock, char * fname);
void generateXPathVector(char * fname);
int getLineNumFromXPathVector(char * xPath);
int getMaxLineNum();

#endif
