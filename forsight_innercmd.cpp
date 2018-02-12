// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innercmd.h"

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
	float value;
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
			int speed = (int)value;
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
					additionalInfomation.offset.pr_reg.type = POSE;
				else if(strcmp(objThreadCntrolBlock->token, "ur") == 0)
					additionalInfomation.offset.pr_reg.type = NUM;
				
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != '[') {
					return 0 ;
				}
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pr_reg.index = (int)value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ']') {
					return 0 ;
				}
				
				if(additionalInfomation.offset.type == PR_UF)
				{
	  				get_token(objThreadCntrolBlock);
					if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
						additionalInfomation.offset.uf_reg.type = POSE;
					else if(strcmp(objThreadCntrolBlock->token, "uf") == 0)
						additionalInfomation.offset.uf_reg.type = NUM;
					
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != '[') {
						return 0 ;
					}
					get_exp(objThreadCntrolBlock, &value, &boolValue);
					additionalInfomation.offset.uf_reg.index = (int)value;
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
				additionalInfomation.offset.pose_target.position.x = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.position.y = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.position.z = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.orientation.a = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.orientation.b = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.pose_target.orientation.c = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ')')
					return 0 ;
				
				if(additionalInfomation.offset.type == C_VEC_UF)
				{
	  				get_token(objThreadCntrolBlock);
					if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
						additionalInfomation.offset.uf_reg.type = POSE;
					else if(strcmp(objThreadCntrolBlock->token, "ur") == 0)
						additionalInfomation.offset.uf_reg.type = NUM;
					
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != '[') {
						return 0 ;
					}
					get_exp(objThreadCntrolBlock, &value, &boolValue);
					additionalInfomation.offset.uf_reg.index = (int)value;
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
				additionalInfomation.offset.joint_target.j1 = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j2 = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j3 = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j4 = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j5 = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ',')
					return 0 ;
				
			    get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.offset.joint_target.j6 = value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ')')
					return 0 ;
				
				if(additionalInfomation.offset.type == J_VEC_UF)
				{
	  				get_token(objThreadCntrolBlock);
					if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
						additionalInfomation.offset.uf_reg.type = POSE;
					else if(strcmp(objThreadCntrolBlock->token, "ur") == 0)
						additionalInfomation.offset.uf_reg.type = NUM;
					
					get_token(objThreadCntrolBlock);
					if(*(objThreadCntrolBlock->token) != '[') {
						return 0 ;
					}
					get_exp(objThreadCntrolBlock, &value, &boolValue);
					additionalInfomation.offset.uf_reg.index = (int)value;
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
			int speed = (int)value;
			iCount++ ;
		}
		// 7.	Ind_EV非同步附加轴速度指令（低优先级）
		else if(strncmp(objThreadCntrolBlock->token, "ind_ev", 6) == 0)
		{
			additionalInfomation.type = IND_EV ;
			get_exp(objThreadCntrolBlock, &value, &boolValue);
			int speed = (int)value;
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
			additionalInfomation.execute.range = value ;
			
			get_token(objThreadCntrolBlock);
			if(strcmp(objThreadCntrolBlock->token, "assign") == 0)
			{
  				get_token(objThreadCntrolBlock);
				if(strcmp(objThreadCntrolBlock->token, "pr") == 0)
					additionalInfomation.execute.assignment.type = POSE;
				else if(strcmp(objThreadCntrolBlock->token, "r") == 0)
					additionalInfomation.execute.assignment.type = NUM;
				
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != '[') {
					return 0 ;
				}
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				additionalInfomation.execute.assignment.index = (int)value;
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token) != ']') {
					return 0 ;
				}
				
				get_token(objThreadCntrolBlock);
				if(*(objThreadCntrolBlock->token)!=EQ) {
					return 0 ;
				}
                get_exp(objThreadCntrolBlock, &value, &boolValue);
				sprintf(additionalInfomation.execute.assignment.value, "%f", value);
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
	    get_token(objThreadCntrolBlock);
		memcpy(instrSetPtr, &additionalInfomation, sizeof(AdditionalInfomation));
		instrSetPtr += sizeof(AdditionalInfomation);
	}
	return iCount;
}

// MovJ P[1] P[1] 30% Fine +附加参数
int call_MoveJ(int iLineNum, struct thread_control_block* objThreadCntrolBlock)
{  
	float value;
	int boolValue;
    
	char commandParam[1024];
    Instruction instr;
    char * commandParamPtr = commandParam;
	
	instr.target.type = MOTION_JOINT;
	if(iLineNum <= g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.joint_target.j1 = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.joint_target.j2 = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.joint_target.j3 = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.joint_target.j4 = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.joint_target.j5 = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.joint_target.j6 = value;

    get_exp(objThreadCntrolBlock, &value, &boolValue);
    instr.target.velocity        = value;
	
	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cnt") == 0)
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
    	if(objThreadCntrolBlock->prog_mode == 1)
    	{
        	instr.target.cnt = -1;
    	}
	    else
	    {
	        instr.target.cnt = value;
	    }
    }
    else
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
        instr.target.cnt = value;
    }
	
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
	
	bool bRet = setInstruction(objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock->instrSet);
	}
    return 1;   
}

int call_MoveL(int iLineNum, struct thread_control_block* objThreadCntrolBlock)
{  
	float value;
	int boolValue;
	
	char commandParam[1024];
    Instruction instr;
    char * commandParamptr = commandParam;
	
	instr.target.type = MOTION_LINE;
	if(iLineNum <= g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
	
    // result.size() == MOVJ_COMMAND_PARAM_MIN
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.pose_target.position.x    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.pose_target.position.y    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.pose_target.position.z    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.pose_target.orientation.a = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.pose_target.orientation.b = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.pose_target.orientation.c = value;

    get_exp(objThreadCntrolBlock, &value, &boolValue);
    instr.target.velocity                  = value;

	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cnt") == 0)
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
    	if(objThreadCntrolBlock->prog_mode == 1)
    	{
        	instr.target.cnt = -1;
    	}
	    else
	    {
	        instr.target.cnt = value;
	    }
    }
    else
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
        instr.target.cnt = value;
    }
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
	
	bool bRet = setInstruction(objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock->instrSet);
	}
    return 1;   
}

int call_MoveC(int iLineNum, struct thread_control_block* objThreadCntrolBlock)
{  
	float value;
	int boolValue;
	
	char commandParam[1024];
    Instruction instr;
    char * commandParamptr = commandParam;
		
	instr.target.type = MOTION_CIRCLE;
	if(iLineNum <= g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);

    // result.size() == MOVJ_COMMAND_PARAM_MIN
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose1.position.x    = value;

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose1.position.y    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose1.position.z    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose1.orientation.a = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose1.orientation.b = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose1.orientation.c = value;

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose2.position.x    = value;

    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose2.position.y    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose2.position.z    = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose2.orientation.a = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose2.orientation.b = value;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	instr.target.circle_target.pose2.orientation.c = value;

	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
    instr.target.velocity                  = value;

	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cnt") == 0)
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
    	if(objThreadCntrolBlock->prog_mode == 1)
    	{
        	instr.target.cnt = -1;
    	}
	    else
	    {
	        instr.target.cnt = value;
	    }
    }
    else
    {
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
        instr.target.cnt = value;
    }
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
	
	bool bRet = setInstruction(objThreadCntrolBlock->instrSet);
	while(bRet == false)
	{
		bRet = setInstruction(objThreadCntrolBlock->instrSet);
	}
    return 1;     
}

int call_UserAlarm(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	float value;
	int boolValue;
    int alarmNumber;
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	alarmNumber = (int)value ;
	setWarning(alarmNumber);
    return 1;
}

int call_Timer(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	float value;
	int boolValue;
    int timerNumber;
    char var[80];
	
    get_exp(objThreadCntrolBlock, &value, &boolValue);
	timerNumber = (int)value ;
	if(timerNumber >= MAX_STOPWATCH_NUM)
    	return 0;
	
	sprintf(var, "Timer[%d]", timerNumber);
	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "start") != 0)
    {
		g_structStopWatch[timerNumber].start_time = time(0);
		assign_var(objThreadCntrolBlock, var, 0.0);
	}
	else if(strcmp(objThreadCntrolBlock->token, "stop") != 0)
    {
		g_structStopWatch[timerNumber].diff_time = time(0) - 
			g_structStopWatch[timerNumber].start_time ;
		assign_var(objThreadCntrolBlock, var, 
			g_structStopWatch[timerNumber].diff_time);
	}
	else 
    {
    	return 0;
	}
    return 1;   
}

int call_Wait(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	float value;
	int boolValue;
	time_t timeStart, now ;
	int timeWaitSeconds ;
    int cond, outTime;
    struct select_and_cycle_stack wait_stack;
    
    Instruction instr;
	
	instr.target.type = MOTION_JOINT;
	if(iLineNum <= g_vecXPath.size())
		sprintf(instr.line, "%s", g_vecXPath[iLineNum].c_str());
	else
		sprintf(instr.line, "OutRange with %d", iLineNum);
	
	get_token(objThreadCntrolBlock);
	if(strcmp(objThreadCntrolBlock->token, "cond") != 0)
    {
		putback(objThreadCntrolBlock);
    	get_exp(objThreadCntrolBlock, &value, &boolValue);
		timeWaitSeconds = (int)value ;
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
			outTime = (int)value ;
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
    }
    return 1;   
}

int call_Pause(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	InterpreterState interpreterState  = PAUSED_R ;
	setPrgmState(PAUSED_R);

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
    return 1;   
}

int call_Abort(int iLineNum, struct thread_control_block* objThreadCntrolBlock) 
{  
	setPrgmState(IDLE_R);
#ifdef WIN32
	Sleep(100);
    return 0; 
#else
	sleep(1);
    return END_COMMND_RET;   
#endif
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
	sprintf(xpath_file_name, "%s_xpath.txt", fname);

	if((xpath_file = fopen(xpath_file_name, "r"))==NULL){
		perror("open file failed\n");  
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
			strcpy(contentXPath,   contentSepPtr + 1);
			iLineNum = atoi(contentLineNum);
			g_vecXPath[iLineNum] = string(contentXPath) ;
		}
	}
	g_vecXPath.resize(iLineNum + 2);
	
//	for(int i =1 ; i < g_vecXPath.size() ; i++)
//	{
//		printf("%d - %s" , i, g_vecXPath[i].c_str());
//	}
}

int getLineNumFromXPathVector(char * xPath)
{
    for(unsigned int i = 0; i < g_vecXPath.size(); ++i)  
    {  
        if(g_vecXPath[i] == xPath)
			return i ;
    }
	return -1 ;
}

int getMaxLineNum()
{
	return g_vecXPath.size() ;
}

