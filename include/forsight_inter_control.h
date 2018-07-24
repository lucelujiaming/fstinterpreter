#ifndef FORSIGHT_INTER_CONTROL_H
#define FORSIGHT_INTER_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#pragma warning(disable : 4786)
#endif

#include <vector>
#include <string>
#include <string.h>
#include <fstream>

#ifndef WIN32
#include <unistd.h>
#endif
#include "interpreter_common.h"
#include "reg_manager/reg_manager_interface_wrapper.h"

#include "forsight_interpreter_shm.h"
#include "forsight_basint.h"

// #define USE_WAITING_R

bool parseScript(const char* fname);
// void findLoopEnd(int index);
InterpreterState getPrgmState();
void setPrgmState(InterpreterState state);
void setCurLine(char * line);
#ifdef WIN32
void setWarning(__int64 warn);
#else
void setWarning(long long int warn);
#endif 
void setSendPermission(bool flag);
void getSendPermission();
void returnRegInfo(RegMap info);
void returnDIOInfo(IOPathInfo& info);
bool setInstruction(struct thread_control_block * objThdCtrlBlockPtr, Instruction * instruction);
bool getIntprtCtrl();
void executeBlock();
void executeLoop(int loop_cnt);
void executeScript();
#ifdef WIN32
unsigned __stdcall script_func(void* arg);
#else
void* script_func(void* arg);
#endif
void parseCtrlComand(); // (struct thread_control_block * objThdCtrlBlock);
void initShm();
void forgesight_load_programs_path();
char * forgesight_get_programs_path();

void updateIOError();
vector<string> split(string str,string pattern);
void startFile(struct thread_control_block * objThdCtrlBlockPtr, 
	char * proj_name, int idx);

void waitInterpreterStateleftWaiting(
	struct thread_control_block * objThdCtrlBlockPtr);
void waitInterpreterStateleftPaused(
	struct thread_control_block * objThdCtrlBlockPtr);

void waitInterpreterStateToPaused(
	struct thread_control_block * objThdCtrlBlockPtr);

void setMoveCommandDestination(MoveCommandDestination movCmdDst);
void getMoveCommandDestination(MoveCommandDestination& movCmdDst);
void copyMoveCommandDestination(MoveCommandDestination& movCmdDst);

#endif







