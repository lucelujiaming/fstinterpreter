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
#else
#include "forsight_interpreter_shm.h"
#endif
#include "interpreter_common.h"

#include "forsight_basint.h"
#include "forsight_home_pose.h"

#include "forsight_external_resource.h"

// #define USE_WAITING_R

void resetProgramNameAndLineNum(struct thread_control_block * objThdCtrlBlockPtr);

char * getProgramName();
void setProgramName(struct thread_control_block * objThdCtrlBlockPtr, char * program_name);

InterpreterState getPrgmState();
void setPrgmState(struct thread_control_block * objThdCtrlBlockPtr, InterpreterState state);

ProgMode getProgMode(struct thread_control_block * objThdCtrlBlockPtr);
void setProgMode(struct thread_control_block * objThdCtrlBlockPtr, ProgMode progMode);

void setCurLine(struct thread_control_block * objThdCtrlBlockPtr, char * line, int lineNum);
#ifdef WIN32
void setWarning(__int64 warn);
#else
void setWarning(long long int warn);
#endif 
void setMessage(int warn);
bool setInstruction(struct thread_control_block * objThdCtrlBlockPtr, Instruction * instruction);
// bool getIntprtCtrl(InterpreterControl& intprt_ctrl);
void executeBlock();
void executeLoop(int loop_cnt);
void executeScript();
#ifdef WIN32
unsigned __stdcall script_func(void* arg);
#else
void* script_func(void* arg);
#endif
void parseCtrlComand(InterpreterControl intprt_ctrl, void * requestDataPtr); 
							// (struct thread_control_block * objThdCtrlBlock);
void initInterpreter();
void uninitInterpreter();
void forgesight_load_programs_path();
char * forgesight_get_programs_path();

bool forgesight_find_external_resource(char *vname, key_variable& keyVar);
bool forgesight_find_external_resource_by_xmlname(char *xml_name, key_variable& keyVar);

void forgesight_load_wait_time_out_config();
int forgesight_get_wait_time_out_config();

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

void getMoveCommandDestination(MoveCommandDestination& movCmdDst);

struct thread_control_block *  getThreadControlBlock(bool isUploadError = true);
int  getCurrentThreadSeq(bool isUploadError = true);
void incCurrentThreadSeq();
void decCurrentThreadSeq();

void updateHomePoseMgr();
checkHomePoseResult checkSingleHomePoseByCurrentJoint(int idx, Joint currentJoint);
#endif







