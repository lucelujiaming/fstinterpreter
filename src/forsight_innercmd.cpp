// #include "stdafx.h"

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innercmd.h"

#ifndef WIN32
#include "error_code.h"
#include <execinfo.h>
#endif

#define FILE_PATH_LEN       1024
#define MAX_WAIT_SECOND     30
#define MAX_STOPWATCH_NUM   128

#define   MOVJ_COMMAND_PARAM_MIN     8
#define   MOVL_COMMAND_PARAM_MIN     8
#define   MOVC_COMMAND_PARAM_MIN     14

typedef struct _StopWatch
{
    time_t start_time;
    int    diff_time;
}StopWatch;

StopWatch g_structStopWatch[MAX_STOPWATCH_NUM];

int call_MoveL(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_MoveJ(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_MoveC(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_Timer(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_UserAlarm(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_Wait(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_Pause(int iLineNum, struct thread_control_block* objThreadCntrolBlock);
int call_Abort(int iLineNum, struct thread_control_block* objThreadCntrolBlock);

// This structure links a library function name   
// with a pointer to that function.   
struct intern_cmd_type {   
    char *f_name; // function name
    int can_exec_sub_thread ;
    int (*p)(int , struct thread_control_block* );   // pointer to the function   
} intern_cmd[] = {   
    // move on the top
    (char *)"movel",      0, call_MoveL,
    (char *)"movej",      0, call_MoveJ,
    (char *)"movec",      0, call_MoveC,
    // left
    (char *)"timer",      1, call_Timer,
    (char *)"useralarm",  1, call_UserAlarm,
    (char *)"wait",       1, call_Wait,
    (char *)"pause",      1, call_Pause,
    (char *)"abort",      1, call_Abort,
    (char *)"", 0  // null terminate the list   
};

vector<string> g_vecXPath ;

/* Return true if c is a delimiter. */
static int isdelim(char c)
{
  if(strchr(" ;,+-<>/*%^=()[]", c) || c==9 || c=='\r' || c=='\n' || c==0) 
    return 1;  
  return 0;
}

/* Return 1 if c is space or tab. */
static int iswhite(char c)
{
  if(c==' ' || c=='\t') return 1;
  else return 0;
}

/* Find the start of the next line. */
static char * find_copy_eol(char * prog, char* temp)
{
  while(*prog!='\n'  && *prog!='\0') 
	  *temp++=*prog++;
  if(*prog) prog++;
  return prog;
}

static char * get_cmd_param(char * prog, char* temp)
{
  while(iswhite(*prog)) ++prog;  /* skip over white space */
    while(!isdelim(*prog)) 
		*temp++=*prog++;
  if(*prog) prog++;
  return prog;
}

int find_internal_cmd(char *s)   
{   
    int i;
    for(i=0; intern_cmd[i].f_name[0]; i++) {
        if(!strcmp(intern_cmd[i].f_name, s))  return i;   
    }   
    return -1;   
}   

int call_internal_cmd_exec_sub_thread(int index)   
{   
    // int i; 
    if(index >= 0)
       return intern_cmd[index].can_exec_sub_thread;
	else 
	   return -1 ;
}

int call_internal_cmd(int index, int iLineNum, 
					 struct thread_control_block* objThreadCntrolBlock)   
{   
    // int i; 
    if(index >= 0)
       return (*intern_cmd[index].p)(iLineNum, objThreadCntrolBlock);
	else 
	   return -1 ;
}

int getAditionalInfomation(struct thread_control_block* objThreadCntrolBlock, 
						   char * instrSetPtr)
{
	int iCount = 0 ;
	eval_value value;
	int boolValue;

	AdditionalInfomation additionalInfomation ;

	while(*(objThreadCntrolBlock->token) != '\r')
	{
		get_token(objThreadCntrolBlock);
		// 1.	ACC加速度倍率指令
		if(strcmp(objThreadCntrolBlock->token, "acc") == 0)
		{
			additionalInfomation.type = ACC ;
			get_exp(objThreadCntrolBlock, &value, &boolValue);
			int speed = (int)value.getFloatValue();
			additionalInfomation.acc_speed = speed ;
			iCount++ ;
		}
		// 2.	 Condition <case>（低优先级）
		// 3.	批量位置补偿指令（低优先级）
		// 4.	单句位置补偿指令
		// 5.	Tool_Offset工具坐标补偿指令
		else if((strcmp(objThreadCntrolBlock->token, "offset") == 0)
		      ||(strcmp(objThreadCntrolBlock->token, "tool_offset") == 0))
		{
			if(strcmp(objThreadCntrolBlock->token, "offset") == 0)
				additionalInfomation.type = OFFSET ;
			else if(strcmp(objThreadCntrolBlock->token, "tool_offset") == 0)
				additionalInfomation.type = TOOL_OFFSET ;

			get_token(objThreadCntrolBlock);
			if(strncmp(objThreadCntrolBlock->token, "pr", strlen("pr")) == 0)
			{
				if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
					additionalInfomation.offset.type = PR;
				else if(strcmp(objThreadCntrolBlock->token, "pr_uf") == 0)
					additionalInfomation.offset.type = PR_UF;
				
				get_token(objThreadCntrolBlock);
				if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
					additionalInfomation.offset.pr_reg.type = POSE_REG;
				else if(strcmp(objThreadCntrolBlock->token, "ur") == 0)
					additionalInfomation.offset.pr_reg.type = NUM_REG;
				
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != '[') {
					return 0 ;
				}
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pr_reg.index = (int)value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ']') {
					return 0 ;
				}
				
				if(additionalInfomation.offset.type == PR_UF)
				{
	  				get_token(objThreadCntrolBlock);
					if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
						additionalInfomation.offset.uf_reg.type = POSE_REG;
					else if(strcmp(objThreadCntrolBlock->token, "uf") == 0)
						additionalInfomation.offset.uf_reg.type = NUM_REG;
					
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != '[') {
						return 0 ;
					}
					get_exp(objThreadCntrolBlock, &value, &boolValue);
					additionalInfomation.offset.uf_reg.index = (int)value.getFloatValue();
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != ']') {
						return 0 ;
					}
				}
			}
			else if(strncmp(objThreadCntrolBlock->token, "c_vec", strlen("c_vec")) == 0)
			{
				if(strcmp(objThreadCntrolBlock->token, "c_vec") == 0)
					additionalInfomation.offset.type = C_VEC;
				else if(strcmp(objThreadCntrolBlock->token, "c_vec_uf") == 0)
					additionalInfomation.offset.type = C_VEC_UF;
				
				// additionalInfomation.offset.pose_target ;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != '(')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.position.x = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.position.y = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.position.z = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.orientation.a = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.orientation.b = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.orientation.c = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ')')
					return 0 ;
				
				if(additionalInfomation.offset.type == C_VEC_UF)
				{
	  				get_token(objThreadCntrolBlock);
					if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
						additionalInfomation.offset.uf_reg.type = POSE_REG;
					else if(strcmp(objThreadCntrolBlock->token, "ur") == 0)
						additionalInfomation.offset.uf_reg.type = NUM_REG;
					
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != '[') {
						return 0 ;
					}
					get_exp(objThreadCntrolBlock, &value, &boolValue);
					additionalInfomation.offset.uf_reg.index = (int)value.getFloatValue();
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != ']') {
						return 0 ;
					}
				}	
			}
			else if(strncmp(objThreadCntrolBlock->token, "j_vec", strlen("j_vec")) == 0)
			{
				if(strcmp(objThreadCntrolBlock->token, "j_vec") == 0)
					additionalInfomation.offset.type = J_VEC;
				else if(strcmp(objThreadCntrolBlock->token, "j_vec_uf") == 0)
					additionalInfomation.offset.type = J_VEC_UF;
				
				// additionalInfomation.offset.joint_target ;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != '(')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j1 = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j2 = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j3 = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j4 = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j5 = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j6 = value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ')')
					return 0 ;
				
				if(additionalInfomation.offset.type == J_VEC_UF)
				{
	  				get_token(objThreadCntrolBlock);
					if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
						additionalInfomation.offset.uf_reg.type = POSE_REG;
					else if(strcmp(objThreadCntrolBlock->token, "ur") == 0)
						additionalInfomation.offset.uf_reg.type = NUM_REG;
					
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != '[') {
						return 0 ;
					}
					get_exp(objThreadCntrolBlock, &value, &boolValue);
					additionalInfomation.offset.uf_reg.index = (int)value.getFloatValue();
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != ']') {
						return 0 ;
					}
				}
			}
			iCount++ ;
		}
		// 6.	EV同步附加轴速度指令（低优先级）
		else if(strncmp(objThreadCntrolBlock->token, "ev", 2) == 0)
		{
			additionalInfomation.type = EV ;
			get_exp(objThreadCntrolBlock, &value, &boolValue);
		//	int speed = (int)value.getFloatValue();
			iCount++ ;
		}
		// 7.	Ind_EV非同步附加轴速度指令（低优先级）
		else if(strncmp(objThreadCntrolBlock->token, "ind_ev", 6) == 0)
		{
			additionalInfomation.type = IND_EV ;
			get_exp(objThreadCntrolBlock, &value, &boolValue);
		//	int speed = (int)value.getFloatValue();
			iCount++ ;
		}
		// 8.	TB先执行指令
		else if((strcmp(objThreadCntrolBlock->token, "tb") == 0)
			  ||(strcmp(objThreadCntrolBlock->token, "ta") == 0)
			  ||(strcmp(objThreadCntrolBlock->token, "db") == 0))
		{
			if(strcmp(objThreadCntrolBlock->token, "tb") == 0)
				additionalInfomation.type = TB ;
			else if(strcmp(objThreadCntrolBlock->token, "ta") == 0)
				additionalInfomation.type = TA ;
			else if(strcmp(objThreadCntrolBlock->token, "db") == 0)
				additionalInfomation.type = DB ;
			
			get_exp(objThreadCntrolBlock, &value, &boolValue);
			additionalInfomation.execute.range = value.getFloatValue() ;
			
			get_token(objThreadCntrolBlock);
			if(strcmp(objThreadCntrolBlock->token, "assign") == 0)
			{
  				get_token(objThreadCntrolBlock);
				if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
					additionalInfomation.execute.assignment.type = POSE_REG;
				else if(strcmp(objThreadCntrolBlock->token, "r") == 0)
					additionalInfomation.execute.assignment.type = NUM_REG;
				
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != '[') {
					return 0 ;
				}
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.execute.assignment.index = (int)value.getFloatValue();
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ']') {
					return 0 ;
				}
				
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token)!=EQ) {
					return 0 ;
				}
                get_exp(objThreadCntrolBlock, &value, &boolValue);
				sprintf(additionalInfomation.execute.assignment.value, "%f", value.getFloatValue());
			}
			else if(strcmp(objThreadCntrolBlock->token, "call") == 0)
			{
				// prog_demo_dec::prog_1 (1, 2)
				memset(additionalInfomation.execute.fname, 0x00, 128);
				get_token(objThreadCntrolBlock);
				sprintf(additionalInfomation.execute.fname, 
					"%s%s", additionalInfomation.execute.fname, 
						objThreadCntrolBlock->token);
				while (strcmp(objThreadCntrolBlock->token, ")") != 0)
				{
					get_token(objThreadCntrolBlock);
					sprintf(additionalInfomation.execute.fname, 
						"%s%s", additionalInfomation.execute.fname, 
						objThreadCntrolBlock->token);
				}
			}
			iCount++ ;
		}
		else if (*(objThreadCntrolBlock->token) == '\r')
		{
			break;
		}
	    get_token(objThreadCntrolBlock);
		memcpy(instrSetPtr, &additionalInfomation, sizeof(AdditionalInfomation));
		instrSetPtr += sizeof(AdditionalInfomation);
	}
	return iCount;
}


int set_global_TF(int iLineNum, int iTFNum, struct thread_control_block* objThreadCntrolBlock)
{
    Instruction instr;
	instr.type = SET_TF ;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
#endif
	instr.current_tf = iTFNum ;
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	FST_INFO("set_global_TF XPATH: %s", g_vecXPath[iLineNum].c_str());
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	}
    return 1;
}

int set_global_UF(int iLineNum, int iUFNum, struct thread_control_block* objThreadCntrolBlock)
{
    Instruction instr;
	instr.type = SET_UF ;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
#endif
	instr.current_uf = iUFNum ;
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	}
    return 1;
}

int set_OVC(int iLineNum, double dOVCNum, struct thread_control_block* objThreadCntrolBlock)
{
    Instruction instr;
	instr.type = SET_OVC ;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
#endif
	instr.current_ovc = dOVCNum / 100 ;
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	}
    return 1;
}

int set_OAC(int iLineNum, double dOACNum, struct thread_control_block* objThreadCntrolBlock)
{
    Instruction instr;
	instr.type = SET_OVC ;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
#endif
	instr.current_oac = dOACNum / 100 ;
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	FST_INFO("set_global_UF XPATH: %s", g_vecXPath[iLineNum].c_str());
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	}
    return 1;
}


// MovJ P[1] P[1] 30% Fine +附加参数
int call_MoveJ(int iLineNum, struct thread_control_block* objThreadCntrolBlock)
{  
    MoveCommandDestination movCmdDst ;
	eval_value value;
	int boolValue;
    
	char commandParam[1024];
    Instruction instr;
    char * commandParamPtr = commandParam;
	instr.type = MOTION ;
	instr.target.type = MOTION_JOINT;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
#endif
	// Save start position
	memset(&movCmdDst ,0x00, sizeof(MoveCommandDestination));
	getMoveCommandDestination(movCmdDst);
	if(iLineNum < (int)g_vecXPath.size())
	{
	    FST_INFO("call_MoveJ Run XPATH: %d: %s", iLineNum, g_vecXPath[iLineNum].c_str());
	    // FST_INFO("call_MoveJ Run movCmdDst: %08X with(%08X, %08X, %08X, %08X)", 
		//  	movCmdDst.type, MOTION_NONE, MOTION_JOINT, MOTION_LINE, MOTION_CIRCLE);
		if(movCmdDst.type != MOTION_NONE)
		{
			if(objThreadCntrolBlock->start_mov_position.find(iLineNum)
				==objThreadCntrolBlock->start_mov_position.end())
			{
			//    FST_INFO("Forward move to JOINT and Insert:(%f, %f, %f, %f, %f, %f) in MovJ", 
			//		movCmdDst.joint_target.j1, movCmdDst.joint_target.j2, 
			//		movCmdDst.joint_target.j3, movCmdDst.joint_target.j4, 
			//		movCmdDst.joint_target.j5, movCmdDst.joint_target.j6);
				try
				{
					objThreadCntrolBlock->start_mov_position.insert(
						map<int, MoveCommandDestination>::value_type(iLineNum, 
										movCmdDst));
				}
				catch (std::exception& e)
				{
				    std::cerr << "Exception catched : " << e.what() << std::endl;
				}
			}
			else
			{
	            FST_INFO("call_MoveJ Run XPATH: %s exists ", g_vecXPath[iLineNum].c_str());
			}
		}
		else
		{
			FST_INFO("call_MoveJ XPATH without movCmdDst");
		}
	}
	else
	{
		; // FST_INFO("call_MoveJ XPATH out of range at %d", iLineNum);
	}
	// FST_INFO("call_MoveJ XPATH: %s", g_vecXPath[iLineNum].c_str());

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	FST_INFO("call_MoveJ value.getType() = %d", value.getType());
	if(value.getType() == TYPE_NONE)
	{
		find_eol(objThreadCntrolBlock);
		return 0;
	}
	else if((value.getType() == TYPE_INT) || (value.getType() == TYPE_FLOAT))
	{
		instr.target.joint_target.j1 = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.joint_target.j2 = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.joint_target.j3 = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.joint_target.j4 = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.joint_target.j5 = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.joint_target.j6 = value.getFloatValue();
	}
	else if(value.getType() == TYPE_POSE)
	{
		// instr.target.pose_target = value.getPoseValue();
	    FST_ERROR("value.getType() == TYPE_POSE in MovJ");
		serror(objThreadCntrolBlock, 16);
		find_eol(objThreadCntrolBlock);
    	return 0;
	}
	else if(value.getType() == TYPE_JOINT)
	{
		instr.target.joint_target = value.getJointValue();
		
	    FST_INFO("Forward move to JOINT:(%f, %f, %f, %f, %f, %f) in MovJ", 
			instr.target.joint_target.j1, instr.target.joint_target.j2, 
			instr.target.joint_target.j3, instr.target.joint_target.j4, 
			instr.target.joint_target.j5, instr.target.joint_target.j6);
	}
	// Use start point in revert mode  
	if(objThreadCntrolBlock->execute_direction == EXECUTE_BACKWARD)
	{
	     FST_INFO("Use start point in revert mode.");
		 
		if(iLineNum < (int)g_vecXPath.size())
		{
		    FST_INFO("call_MoveJ XPATH: %s", g_vecXPath[iLineNum].c_str());
			if(objThreadCntrolBlock->start_mov_position.find(iLineNum)
				!=objThreadCntrolBlock->start_mov_position.end())
			{
			    instr.target.joint_target
				 	= objThreadCntrolBlock->start_mov_position[iLineNum].joint_target;
			}
			else
			{
				FST_INFO("call_MoveL XPATH without StartJoint");
			}
		}
		else
		{
			FST_ERROR("call_MoveJ XPATH out of range at %d", iLineNum);
		}
		 
	    FST_INFO("Backward move to JOINT:(%f, %f, %f, %f, %f, %f) in MovJ", 
			instr.target.joint_target.j1, instr.target.joint_target.j2, 
			instr.target.joint_target.j3, instr.target.joint_target.j4, 
			instr.target.joint_target.j5, instr.target.joint_target.j6);
	}
	get_token(objThreadCntrolBlock);

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	// Divide 100 as percent.
	FST_INFO("get_token =  '%f'", value.getFloatValue());
    instr.target.vel        = value.getFloatValue() / 100;
	
	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cnt") == 0)
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
    	if(objThreadCntrolBlock->prog_mode == STEP_MODE)
    	{
        	instr.target.cnt = -1;
    	}
	    else
	    {
	        if(value.getFloatValue() < 0) // == -1
	    	{
	        	instr.target.cnt = -1;
	     		FST_INFO("instr.target.cnt = %f in the FINE.", instr.target.cnt);
	    	}
	        else 
				instr.target.cnt = value.getFloatValue() / 100;
	    }
    }
    else
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
        instr.target.cnt = value.getFloatValue() / 100;
    }
	// instr.target.acc = -1 ;
	// Set to instrSet
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	
	get_token(objThreadCntrolBlock);
	// result.size() > MOVJ_COMMAND_PARAM_MIN
	if(objThreadCntrolBlock->token_type == DELIMITER)
	{
		if(*(objThreadCntrolBlock->token) == ';')
		{
			char * instrSetPtr = 
				(char *)objThreadCntrolBlock->instrSet
				+ sizeof(Instruction) - sizeof(char) ;
			objThreadCntrolBlock->instrSet->is_additional = true ;
			objThreadCntrolBlock->instrSet->add_num    =  
				getAditionalInfomation(objThreadCntrolBlock, instrSetPtr);
		}
		else
		{
			objThreadCntrolBlock->instrSet->is_additional = false ;
			objThreadCntrolBlock->instrSet->add_num    = 0 ;
		}
	}
	
	// objThreadCntrolBlock->currentMotionTarget = instr.target ;
// 	#ifdef USE_XPATH
// 		FST_INFO("setInstruction MOTION_JOINT at %s", instr.line);
// 	#else
// 		FST_INFO("setInstruction MOTION_JOINT at %d", instr.line);
// 	#endif
	FST_INFO("call_MoveJ instr.target.cnt = %f setInstruction.", instr.target.cnt);
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
#ifdef WIN32
		Sleep(1);
#else
        usleep(1000);
#endif
	}
    return 1;   
}

int call_MoveL(int iLineNum, struct thread_control_block* objThreadCntrolBlock)
{  
    MoveCommandDestination movCmdDst ;
	eval_value value;
	int boolValue;
	
	char commandParam[1024];
    Instruction instr;
    char * commandParamptr = commandParam;
	instr.type = MOTION ;
	instr.target.type = MOTION_LINE;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
#endif
	// Save start position
	memset(&movCmdDst ,0x00, sizeof(MoveCommandDestination));
	getMoveCommandDestination(movCmdDst);
	if(iLineNum < (int)g_vecXPath.size())
	{
	    FST_INFO("call_MoveL Run XPATH: %d: %s", iLineNum, g_vecXPath[iLineNum].c_str());
	    // FST_INFO("call_MoveL Run movCmdDst: %08X with(%08X, %08X, %08X, %08X)", 
		//  	movCmdDst.type, MOTION_NONE, MOTION_JOINT, MOTION_LINE, MOTION_CIRCLE);
		if(movCmdDst.type != MOTION_NONE)
		{
			if(objThreadCntrolBlock->start_mov_position.find(iLineNum)
				==objThreadCntrolBlock->start_mov_position.end())
			{
			//    FST_INFO("move from POSE and insert:(%f, %f, %f, %f, %f, %f) in MovL", 
			//		movCmdDst.pose_target.position.x,    movCmdDst.pose_target.position.y, 
			//		movCmdDst.pose_target.position.z,    movCmdDst.pose_target.orientation.a, 
			//		movCmdDst.pose_target.orientation.b, movCmdDst.pose_target.orientation.c);
				try
				{
					objThreadCntrolBlock->start_mov_position.insert(
						map<int, MoveCommandDestination>::value_type(iLineNum, 
											movCmdDst));
				}
				catch (std::exception& e)
				{
				    std::cerr << "Exception catched : " << e.what() << std::endl;
				}
			}
			else
			{
	            FST_INFO("call_MoveL Run XPATH: %s exists ", g_vecXPath[iLineNum].c_str());
			}
		}
		else
		{
			; // FST_INFO("call_MoveL XPATH without movCmdDst");
		}
	}
	else
	{
		FST_ERROR("call_MoveL XPATH out of range at %d", iLineNum);
	}
	// FST_INFO("call_MoveL Run XPATH: %s", g_vecXPath[iLineNum].c_str());
	
    // result.size() == MOVJ_COMMAND_PARAM_MIN
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	FST_INFO("call_MoveL value.getType() = %d", value.getType());
	if(value.getType() == TYPE_NONE)
	{
		find_eol(objThreadCntrolBlock);
		return 0;
	}
	else if((value.getType() == TYPE_INT) || (value.getType() == TYPE_FLOAT))
	{
		instr.target.pose_target.position.x    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
		get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.pose_target.position.y    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
		get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.pose_target.position.z    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
		get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.pose_target.orientation.a = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
		get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.pose_target.orientation.b = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
		get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.pose_target.orientation.c = value.getFloatValue();

		instr.target.user_frame_id = instr.target.tool_frame_id = -1 ;
	}
	else if(value.getType() == TYPE_POSE)
	{
		instr.target.pose_target = value.getPoseValue();
		instr.target.user_frame_id = value.getUFIndex();
		instr.target.tool_frame_id = value.getTFIndex();
		
	    FST_INFO("Forward move to POSE:(%f, %f, %f, %f, %f, %f) in MovL", 
			instr.target.pose_target.position.x, instr.target.pose_target.position.y, 
			instr.target.pose_target.position.z, instr.target.pose_target.orientation.a, 
			instr.target.pose_target.orientation.b, instr.target.pose_target.orientation.c);
	}
	else if(value.getType() == TYPE_JOINT)
	{
		// instr.target.joint_target = value.getJointValue();
	    FST_ERROR("value.getType() == TYPE_JOINT in MovL");
		serror(objThreadCntrolBlock, 15);
		find_eol(objThreadCntrolBlock);
    	return 0;
	}
	// Use start point in revert mode  
	if(objThreadCntrolBlock->execute_direction == EXECUTE_BACKWARD)
	{
	     FST_INFO("Use start point in revert mode.");
		 
		if(iLineNum < (int)g_vecXPath.size())
		{
		    FST_INFO("call_MoveL XPATH: %s", g_vecXPath[iLineNum].c_str());
			if(objThreadCntrolBlock->start_mov_position.find(iLineNum)
				!=objThreadCntrolBlock->start_mov_position.end())
			{
			    instr.target.pose_target
				 	= objThreadCntrolBlock->start_mov_position[iLineNum].pose_target;
			}
			else
			{
				FST_INFO("call_MoveL XPATH without StartPoint");
			}
		}
		else
		{
			FST_INFO("call_MoveL XPATH out of range at %d", iLineNum);
		}
		 
	    FST_INFO("Backward move to POSE:(%f, %f, %f, %f, %f, %f) in MovL", 
			instr.target.pose_target.position.x, instr.target.pose_target.position.y, 
			instr.target.pose_target.position.z, instr.target.pose_target.orientation.a, 
			instr.target.pose_target.orientation.b, instr.target.pose_target.orientation.c);
	}
	
	
	get_token(objThreadCntrolBlock);
    get_exp(objThreadCntrolBlock, &value, &boolValue);
    instr.target.vel                  = value.getFloatValue();

	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cnt") == 0)
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
	     FST_INFO("instr.target.cnt = %f setInstruction.", value.getFloatValue());
    	if(objThreadCntrolBlock->prog_mode == STEP_MODE)
    	{
        	instr.target.cnt = -1;
    	}
	    else
	    {
	        if(value.getFloatValue() < 0) // == -1
	    	{
	        	instr.target.cnt = -1.0000;
	     		FST_INFO("instr.target.cnt = %f in the FINE.", instr.target.cnt);
	    	}
	        else 
	            instr.target.cnt = value.getFloatValue() / 100;
	    }
    }
    else
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
        instr.target.cnt = value.getFloatValue() / 100;
    }
	// instr.target.acc = -1 ;
	// Set to instrSet
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	
	get_token(objThreadCntrolBlock);
	// result.size() > MOVJ_COMMAND_PARAM_MIN
	if(objThreadCntrolBlock->token_type == DELIMITER)
	{
		if(*(objThreadCntrolBlock->token) == ';')
		{
			char * instrSetPtr = 
				(char *)objThreadCntrolBlock->instrSet
				+ sizeof(Instruction) - sizeof(char) ;
			objThreadCntrolBlock->instrSet->is_additional = true ;
			objThreadCntrolBlock->instrSet->add_num    =  
				getAditionalInfomation(objThreadCntrolBlock, instrSetPtr);
		}
		else
		{
			objThreadCntrolBlock->instrSet->is_additional = false ;
			objThreadCntrolBlock->instrSet->add_num    = 0 ;
		}
	}
	// FST_INFO("MOVL: instr.target.accleration = %f .", instr.target.acc);
	
// 	#ifdef USE_XPATH
// 		FST_INFO("setInstruction MOTION_LINE at %s", instr.line);
// 	#else
// 		FST_INFO("setInstruction MOTION_LINE at %d", instr.line);
// 	#endif
	
	 FST_INFO("instr.target.cnt = %f setInstruction.", instr.target.cnt);
#ifndef WIN32
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
#ifdef WIN32
		Sleep(1);
#else
        usleep(1000);
#endif
	}
#endif

//    FST_INFO("setInstruction return true");
    return 1;   
}

int call_MoveC(int iLineNum, struct thread_control_block* objThreadCntrolBlock)
{  
    MoveCommandDestination movCmdDst ;
	eval_value value;
	int boolValue;
	
	char commandParam[1024];
    Instruction instr;
    char * commandParamptr = commandParam;
	instr.type = MOTION ;
	instr.target.type = MOTION_CIRCLE;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
#endif
	// Save start position
	memset(&movCmdDst ,0x00, sizeof(MoveCommandDestination));
	getMoveCommandDestination(movCmdDst);
	if(iLineNum < (int)g_vecXPath.size())
	{
	    FST_INFO("call_MoveC Run XPATH: %d: %s", iLineNum, g_vecXPath[iLineNum].c_str());
		if(movCmdDst.type != MOTION_NONE)
		{
			if(objThreadCntrolBlock->start_mov_position.find(iLineNum)
				==objThreadCntrolBlock->start_mov_position.end())
			{
				try
				{
					objThreadCntrolBlock->start_mov_position.insert(
						map<int, MoveCommandDestination>::value_type(iLineNum, 
											movCmdDst));
				}
				catch (std::exception& e)
				{
				    std::cerr << "Exception catched : " << e.what() << std::endl;
				}
			}
			else
			{
	            FST_INFO("call_MoveC Run XPATH: %s exists ", g_vecXPath[iLineNum].c_str());
			}
		}
		else
		{
			FST_INFO("call_MoveC XPATH without movCmdDst");
		}
	}
	else
	{
		FST_ERROR("call_MoveC XPATH out of range at %d", iLineNum);
	}

    // result.size() == MOVJ_COMMAND_PARAM_MIN
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	if(value.getType() == TYPE_NONE)
	{
		find_eol(objThreadCntrolBlock);
    	return 0;
	}
	else if((value.getType() == TYPE_INT) || (value.getType() == TYPE_FLOAT))
	{
		instr.target.circle_target.pose1.position.x    = value.getFloatValue();
		get_token(objThreadCntrolBlock);

	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose1.position.y    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose1.position.z    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose1.orientation.a = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose1.orientation.b = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose1.orientation.c = value.getFloatValue();
	}
	else if(value.getType() == TYPE_POSE)
	{
		instr.target.pose_target = value.getPoseValue();
		
	    FST_INFO("move to POSE:(%f, %f, %f, %f, %f, %f) in MovC", 
			instr.target.pose_target.position.x, instr.target.pose_target.position.y, 
			instr.target.pose_target.position.z, instr.target.pose_target.orientation.a, 
			instr.target.pose_target.orientation.b, instr.target.pose_target.orientation.c);
	}
	else if(value.getType() == TYPE_JOINT)
	{
		// instr.target.joint_target = value.getJointValue();
	    FST_INFO("value.getType() == TYPE_JOINT in MovC");
		find_eol(objThreadCntrolBlock);
    	return 0;
	}
	get_token(objThreadCntrolBlock);

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	if(value.getType() == TYPE_NONE)
	{
		find_eol(objThreadCntrolBlock);
		return 0;
	}
	else if((value.getType() == TYPE_INT) || (value.getType() == TYPE_FLOAT))
	{
		instr.target.circle_target.pose2.position.x    = value.getFloatValue();
		get_token(objThreadCntrolBlock);

	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose2.position.y    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose2.position.z    = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose2.orientation.a = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose2.orientation.b = value.getFloatValue();
		get_token(objThreadCntrolBlock);
		
	    get_exp(objThreadCntrolBlock, &value, &boolValue);
		instr.target.circle_target.pose2.orientation.c = value.getFloatValue();
	}
	else if(value.getType() == TYPE_POSE)
	{
		instr.target.pose_target = value.getPoseValue();
	}
	else if(value.getType() == TYPE_JOINT)
	{
		// instr.target.joint_target = value.getJointValue();
		find_eol(objThreadCntrolBlock);
    	return 0;
	}
	// Use start point in revert mode  
	if(objThreadCntrolBlock->execute_direction == EXECUTE_BACKWARD)
	{
	     FST_INFO("Use start point in revert mode.");
		 // Wait for revert
	     // instr.target.circle_target.pose2 
	     //    = objThreadCntrolBlock->start_mov_position[g_vecXPath[iLineNum]];
	}
	get_token(objThreadCntrolBlock);
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
    instr.target.vel                  = value.getFloatValue();

	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cnt") == 0)
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
    //	if(objThreadCntrolBlock->prog_mode == STEP_MODE)
    //	{
        	instr.target.cnt = -1;
    //	}
	//    else
	//    {
	//        instr.target.cnt = value.getFloatValue() / 100;
	//    }
    }
    else
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
        instr.target.cnt = value.getFloatValue() / 100;
    }
	// instr.target.acc = -1 ;
	// Set to instrSet
	memcpy(objThreadCntrolBlock->instrSet, &instr, sizeof(Instruction));
	
	get_token(objThreadCntrolBlock);
	// result.size() > MOVJ_COMMAND_PARAM_MIN
	if(objThreadCntrolBlock->token_type == DELIMITER)
	{
		if(*(objThreadCntrolBlock->token) == ';')
		{
			char * instrSetPtr = 
				(char *)objThreadCntrolBlock->instrSet
				+ sizeof(Instruction) - sizeof(char) ;
			objThreadCntrolBlock->instrSet->is_additional = true ;
			objThreadCntrolBlock->instrSet->add_num    =  
				getAditionalInfomation(objThreadCntrolBlock, instrSetPtr);
		}
		else
		{
			objThreadCntrolBlock->instrSet->is_additional = false ;
			objThreadCntrolBlock->instrSet->add_num    = 0 ;
		}
	}
	
// 	#ifdef USE_XPATH
// 		FST_INFO("setInstruction MOTION_CURVE at %s", instr.line);
// 	#else
// 		FST_INFO("setInstruction MOTION_CURVE at %d", instr.line);
// 	#endif
	bool bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock, objThreadCntrolBlock->instrSet);
#ifdef WIN32
		Sleep(1);
#else
        usleep(1000);
#endif
	}
    return 1;     
}

int call_UserAlarm(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	eval_value value;
	int boolValue;
    int alarmNumber;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	alarmNumber = (int)value.getFloatValue() ;
	setMessage(alarmNumber);
    find_eol(objThreadCntrolBlock);
    return 1;
}

int call_Timer(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	eval_value value;
	int boolValue;
    int timerNumber;
    char var[80];
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	timerNumber = (int)value.getFloatValue() ;
        FST_INFO("%d: call_Timer  enter %d ", __LINE__, timerNumber);
	if(timerNumber >= MAX_STOPWATCH_NUM)
    	return 0;
	
	sprintf(var, "Timer[%d]", timerNumber);
	get_token(objThreadCntrolBlock);
        FST_INFO("%d: call_Timer  enter %s ", __LINE__, objThreadCntrolBlock->token);
	if(strcmp(objThreadCntrolBlock->token, "start") == 0)
    {
        FST_INFO("%d: call_Timer  start", __LINE__);
		g_structStopWatch[timerNumber].start_time = time(0);
		value.setFloatValue(0); // 0.0; 
		assign_var(objThreadCntrolBlock, var, value); // 0.0);
	}
	else if(strcmp(objThreadCntrolBlock->token, "stop") == 0)
    {
        FST_INFO("%d: call_Timer  stop", __LINE__);
		g_structStopWatch[timerNumber].diff_time = time(0) - 
			g_structStopWatch[timerNumber].start_time ;
		
    	eval_value value;
		value.setFloatValue(g_structStopWatch[timerNumber].diff_time)  ;
		assign_var(objThreadCntrolBlock, var, value);
	    FST_INFO("Time elapse : %d .", g_structStopWatch[timerNumber].diff_time);
	}
	else 
    {
        find_eol(objThreadCntrolBlock);
    	return 0;
	}
    find_eol(objThreadCntrolBlock);
    return 1;   
}

int call_Wait(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	eval_value value;
	int boolValue;
	time_t timeStart, now ;
	int timeWaitSeconds ;
    int cond, outTime;
    struct select_and_cycle_stack wait_stack;
    
    Instruction instr;
	
	instr.target.type = MOTION_JOINT;
#ifdef USE_XPATH
	if(iLineNum <= (int)g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
#else
	FST_INFO("call_MoveJ XPATH at %d", iLineNum);
	instr.line = iLineNum;
	FST_INFO("call_Wait XPATH: %s", g_vecXPath[iLineNum].c_str());
#endif
	
	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cond") != 0)
    {
		putback(objThreadCntrolBlock);
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
		// Need not get_token again . let call_interpreter filter it .
		timeWaitSeconds = (int)value.getFloatValue() ;
	    FST_INFO("call_Wait timeWaitSeconds = %d at %lld.", timeWaitSeconds, time(NULL));
		now = timeStart  = time(0);
		while(now - timeStart < timeWaitSeconds)
		{
#ifdef WIN32
			Sleep(100);
#else
			sleep(1);
#endif
			now = time(0);
		}
	    FST_INFO("call_Wait timeWaitSeconds = %d at %lld.", timeWaitSeconds, time(NULL));
		find_eol(objThreadCntrolBlock);
    }
    else  // Deal wait with condition
    {
        // wait_stack.itokentype = WAIT ;
        wait_stack.while_loc = objThreadCntrolBlock->prog;
		cond = calc_conditions(objThreadCntrolBlock);
		
		get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->token[0] == '\r')
		{
			putback(objThreadCntrolBlock);
			now = timeStart  = time(0);
			while(!cond)
			{
#ifdef WIN32
				Sleep(100);
#else
				sleep(1);
#endif
			    now = time(0);

				objThreadCntrolBlock->prog = wait_stack.while_loc;
				cond = calc_conditions(objThreadCntrolBlock);
				while(now - timeStart > MAX_WAIT_SECOND)
					break ;
			}
		}
		// with dealing method when timeout
		else if(objThreadCntrolBlock->token_type == NUMBER) 
		{
			putback(objThreadCntrolBlock);
			get_exp(objThreadCntrolBlock, &value, &boolValue);
			outTime = (int)value.getFloatValue() ;
			wait_stack.loc = objThreadCntrolBlock->prog;
			now = timeStart  = time(0);
			while(!cond)
			{
#ifdef WIN32
				Sleep(100);
#else
				sleep(1);
#endif
			    now = time(0);
				objThreadCntrolBlock->prog = wait_stack.while_loc;
				cond = calc_conditions(objThreadCntrolBlock);
				if(now - timeStart > outTime)
					break ;
			}
			
			objThreadCntrolBlock->prog = wait_stack.loc;
			if(!cond)
			{
				get_token(objThreadCntrolBlock);
				if(strcmp(objThreadCntrolBlock->token, "skip") == 0)
				{
					; // Do nothing  
				}
				else if(strcmp(objThreadCntrolBlock->token, "call") == 0)
				{
					// exec_call(objThreadCntrolBlock) ;
					int iRet = exec_call(objThreadCntrolBlock);
					if(iRet == END_COMMND_RET)
						return END_COMMND_RET;
				}
				else 
				{
					putback(objThreadCntrolBlock);
				}
			}
			else
				find_eol(objThreadCntrolBlock);
			
		}
		else 
		{
			putback(objThreadCntrolBlock);
		}
		find_eol(objThreadCntrolBlock);
    }
    return 1;   
}

int call_Pause(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	InterpreterState interpreterState  = INTERPRETER_PAUSED ;
	setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSED);
/*
	while(interpreterState == PAUSED_R)
	{
#ifdef WIN32
		Sleep(100);
		interpreterState = EXECUTE_R ;
#else
		sleep(1);
		interpreterState = getPrgmState();
#endif
	}
*/
    objThreadCntrolBlock->is_paused = true;
    FST_INFO("call_Pause: Enter waitInterpreterStateleftPaused %d ", iLineNum);
	waitInterpreterStateleftPaused(objThreadCntrolBlock);
    FST_INFO("call_Pause: Left  waitInterpreterStateleftPaused %d ", iLineNum);
    objThreadCntrolBlock->is_paused = false;
    find_eol(objThreadCntrolBlock);
    return 1;   
}

int call_Abort(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	setPrgmState(objThreadCntrolBlock, INTERPRETER_IDLE);
  // clear line path and ProgramName
	resetProgramNameAndLineNum(objThreadCntrolBlock);
#ifdef WIN32
	Sleep(100);
    return 0; 
#else
	sleep(1);
    return END_COMMND_RET;   
#endif
}

int getXPathLinenum(char * file_name)
{
	int iLineCount = 0 ;
	char contentLine[FILE_PATH_LEN];
	FILE *xpath_file ;
	if((xpath_file = fopen(file_name, "r"))==NULL){
		perror("open file_name file failed\n");  
		return -1;
	}
	
	memset(contentLine,    0x00, FILE_PATH_LEN);
	while(fgets(contentLine, sizeof(contentLine), xpath_file)!=NULL)  
	{  
		iLineCount++ ;
	}
	fclose(xpath_file);
	return iLineCount ;
}

void mergeImportXPathToProjectXPath(
		struct thread_control_block* objThreadCntrolBlock, char * fname)
{
	int iImportLineNum = 0 , iMainLineCount = 0 ;
	char contentImportLine[FILE_PATH_LEN];
	char * contentImportSepPtr; 
	char contentImportLineNum[LAB_LEN];
	char contentImportXPath[FILE_PATH_LEN];
	
	char xpath_import_file_name[FILE_PATH_LEN];
	char xpath_main_file_name[FILE_PATH_LEN];

	FILE *xpath_import_file ;
	FILE *xpath_main_file ;
	
	memset(xpath_import_file_name, 0x00, FILE_PATH_LEN);
	memset(xpath_main_file_name, 0x00, FILE_PATH_LEN);
#ifdef WIN32
	sprintf(xpath_import_file_name, "%s_xpath.txt", fname);
	sprintf(xpath_main_file_name, "%s_xpath.txt", objThreadCntrolBlock->project_name);
#else
	sprintf(xpath_import_file_name, "%s\/programs\/%s_xpath.txt", 
			forgesight_get_programs_path(), fname);
	sprintf(xpath_main_file_name, "%s\/programs\/%s_xpath.txt", 
			forgesight_get_programs_path(), objThreadCntrolBlock->project_name);
#endif
	iMainLineCount = getXPathLinenum(xpath_main_file_name);
	
	if((xpath_import_file = fopen(xpath_import_file_name, "r"))==NULL){
		perror("open xpath_import_file_name file failed\n");  
		return ;
	}
	if((xpath_main_file = fopen(xpath_main_file_name, "a"))==NULL){
		perror("open xpath_import_file_name file failed\n");  
		return ;
	}
	
	memset(contentImportLine,    0x00, FILE_PATH_LEN);
	while(fgets(contentImportLine, 
		sizeof(contentImportLine), xpath_import_file)!=NULL)  
	{  
		memset(contentImportLineNum, 0x00, LAB_LEN);
		memset(contentImportXPath,   0x00, FILE_PATH_LEN);
		contentImportSepPtr = strchr(contentImportLine, ':');
		if(contentImportSepPtr)
		{
			memcpy(contentImportLineNum, contentImportLine, 
				contentImportSepPtr - contentImportLine);
			strcpy(contentImportXPath,   contentImportSepPtr + 1);
			iImportLineNum = atoi(contentImportLineNum);
		//	FST_INFO("%03d:%s", iImportLineNum + iMainLineCount, 
		//		contentImportSepPtr + 1);
			fprintf(xpath_main_file, "%03d:%s", iImportLineNum + iMainLineCount, 
				contentImportSepPtr + 1);
		}
	}
	fclose(xpath_import_file);
	fclose(xpath_main_file);
}

void generateXPathVector(char * fname)
{
	char xpath_file_name[FILE_PATH_LEN];
	int iLineNum = 0 ;
	char contentLine[FILE_PATH_LEN];
	char * contentSepPtr; 
	char contentLineNum[LAB_LEN];
	char contentXPath[FILE_PATH_LEN];

	FILE *xpath_file ;

	memset(xpath_file_name, 0x00, FILE_PATH_LEN);
#ifdef WIN32
	sprintf(xpath_file_name, "%s_xpath.txt", fname);
#else
	sprintf(xpath_file_name, "%s\/programs\/%s_xpath.txt", 
			forgesight_get_programs_path(), fname);
#endif

	if((xpath_file = fopen(xpath_file_name, "r"))==NULL){
		perror("open file failed\n");  
		return ;
	}
	
	// Pre-arrange
	// for(int i =0 ; i < g_lineNum + 1 ; i++)
	//  	vecXPath.push_back("");
	g_vecXPath.resize(1024);

	memset(contentLine,    0x00, FILE_PATH_LEN);
	while(fgets(contentLine,sizeof(contentLine),xpath_file)!=NULL)  
	{  
		memset(contentLineNum, 0x00, LAB_LEN);
		memset(contentXPath,   0x00, FILE_PATH_LEN);
		contentSepPtr = strchr(contentLine, ':');
		if(contentSepPtr)
		{
			memcpy(contentLineNum, contentLine, 
				contentSepPtr - contentLine);
			char * strEnter = strchr(contentSepPtr + 1, '\n');
			if(strEnter)
			{
			    memcpy(contentXPath, contentSepPtr + 1, strlen(contentSepPtr + 1) - 1);
			}
			else
			{
				strcpy(contentXPath,   contentSepPtr + 1);
			}
			iLineNum = atoi(contentLineNum);
			g_vecXPath[iLineNum] = string(contentXPath) ;
		}
	}
	g_vecXPath.resize(iLineNum + 2);
	fclose(xpath_file);
//	for(int i =1 ; i < g_vecXPath.size() ; i++)
//	{
//		FST_INFO("%d - %s" , i, g_vecXPath[i].c_str());
//	}
}

int getLineNumFromXPathVector(char * xPath)
{
    for(unsigned int i = 0; i < (int)g_vecXPath.size(); ++i)  
    {  
        if(g_vecXPath[i] == string(xPath))
        	return i ;
    }
	FST_INFO("getLineNumFromXPathVector Failed:: %s ", xPath);
	return -1 ;
}

int getMaxLineNum()
{
	return (int)g_vecXPath.size() ;
}

