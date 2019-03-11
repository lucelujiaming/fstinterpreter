#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include<libxml/parser.h>

#include "forsight_inter_control.h"
#include "forsight_innercmd.h"
#include "forsight_io_controller.h"
#ifndef WIN32
// #include "io_interface.h"
#include "error_code.h"
#endif

#ifdef USE_FORSIGHT_REGISTERS_MANAGER
#ifndef WIN32
#include "reg_manager/reg_manager_interface_wrapper.h"
using namespace fst_ctrl ;
#else
#include "forsight_io_mapping.h"
#include "forsight_launch_code_startup.h"
#include "forsight_macro_instr_startup.h"
#endif
#include "reg_manager/forsight_registers_manager.h"
#else
#include "reg-shmi/forsight_registers.h"
#include "reg-shmi/forsight_op_regs_shmi.h"
#endif
#include "launch_code_mgr.h"

#define VELOCITY    (500)
using namespace std;

#define MAX_WAIT_SECOND     30

// bool is_backward= false;
// InterpreterState prgm_state = INTERPRETER_IDLE;
// static IntprtStatus intprt_status;
// static Instruction instruction;
// static CtrlStatus ctrl_status;


// extern jmp_buf e_buf; /* hold environment for longjmp() */
extern struct thread_control_block g_thread_control_block[NUM_THREAD + 1];

#ifdef WIN32
extern HANDLE    g_basic_interpreter_handle[NUM_THREAD + 1];
#else
extern pthread_t g_basic_interpreter_handle[NUM_THREAD + 1];
#endif
int  g_iCurrentThreadSeq = -1 ;  // minus one add one equals to zero

std::string g_files_manager_data_path = "";
int         g_wait_time_out_config    = -1;

vector<key_variable> g_vecKeyVariables ;

static InterpreterState g_privateInterpreterState;
InterpreterPublish  g_interpreter_publish; 

LaunchCodeMgr *    g_launch_code_mgr_ptr; 
HomePoseMgr *      g_home_pose_mgr_ptr; 

/************************************************* 
	Function:		split
	Description:	split string to vector
	Input:			str               - soruce string
	Input:			pattern           - seperator
	Return: 		vector<string>
*************************************************/ 
vector<string> split(string str,string pattern)
{
	string::size_type pos;
	vector<string> result;
	str+=pattern;
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if((int)pos<size)
		{
			string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

/************************************************* 
	Function:		getMoveCommandDestination
	Description:	get start position of MOV* (Used in the BACKWARD)
	Input:			movCmdDst        - start position of MOV*
	Return: 		NULL
*************************************************/ 
void getMoveCommandDestination(MoveCommandDestination& movCmdDst)
{
	  forgesight_registers_manager_get_joint(movCmdDst.joint_target);
	  forgesight_registers_manager_get_cart(movCmdDst.pose_target);
//    readShm(SHM_INTPRT_DST, 0, (void*)&movCmdDst, sizeof(movCmdDst));
}

/************************************************* 
	Function:		resetProgramNameAndLineNum
	Description:	clear Line number and ProgramName
	Input:			thread_control_block   - interpreter info
	Return: 		NULL
*************************************************/ 
void resetProgramNameAndLineNum(struct thread_control_block * objThdCtrlBlockPtr)
{
	setCurLine(objThdCtrlBlockPtr, (char *)"", 0);
	setProgramName(objThdCtrlBlockPtr, (char *)""); 
}

/************************************************* 
	Function:		getProgramName
	Description:	get running Program Name
	Return: 		Running Program Name
*************************************************/ 
char * getProgramName()
{
	return g_interpreter_publish.program_name; 
}

/************************************************* 
	Function:		setProgramName
	Description:	set running Program Name
	Input:			thread_control_block   - interpreter info
	Input:			program_name           - Running Program Name
	Return: 		NULL
*************************************************/ 
void setProgramName(struct thread_control_block * objThdCtrlBlockPtr, char * program_name)
{
    FST_INFO("setProgramName to %s at %d", program_name, objThdCtrlBlockPtr->is_main_thread);
	if(objThdCtrlBlockPtr->is_main_thread == MAIN_THREAD)
	{
		strcpy(g_interpreter_publish.program_name, program_name); 
	}
	else
	{
		FST_INFO("setProgramName Failed to %s", program_name);
	}
}

/************************************************* 
	Function:		getThreadControlBlock
	Description:	Find a free thread_control_block object in the g_thread_control_block
	Input:			NULL
	Return: 		a free thread_control_block object 
*************************************************/ 
struct thread_control_block *  getThreadControlBlock()
{
	if(getCurrentThreadSeq() < 0)
    {
        FST_ERROR("getThreadControlBlock failed from %d", getCurrentThreadSeq());
		setWarning(INFO_INTERPRETER_THREAD_NOT_EXIST);
		return NULL;
	}
	else
    {
    	FST_INFO("getThreadControlBlock at %d", getCurrentThreadSeq());
		return &g_thread_control_block[getCurrentThreadSeq()] ;
	}
}

/************************************************* 
	Function:		getCurrentThreadSeq
	Description:	get current running thread_control_block index
	Input:			NULL
	Return: 		a free thread_control_block object index
*************************************************/ 
int getCurrentThreadSeq()
{
	if(g_iCurrentThreadSeq < 0)
		setWarning(INFO_INTERPRETER_THREAD_NOT_EXIST);
	return g_iCurrentThreadSeq ;
}

/************************************************* 
	Function:		incCurrentThreadSeq
	Description:	increment current running thread_control_block index
	Input:			NULL
	Return: 		NULL
*************************************************/ 
void incCurrentThreadSeq()
{
	if(g_iCurrentThreadSeq < NUM_THREAD)
		g_iCurrentThreadSeq++ ;
	else
		g_iCurrentThreadSeq = 0 ;
}

/************************************************* 
	Function:		decCurrentThreadSeq
	Description:	decrement current running thread_control_block index
	Input:			NULL
	Return: 		NULL
*************************************************/ 
void decCurrentThreadSeq()
{
	if(g_iCurrentThreadSeq == 0)
    	FST_ERROR("g_iCurrentThreadSeq == 0");
	g_iCurrentThreadSeq-- ;
}

/************************************************* 
	Function:		getPrgmState
	Description:	get current Program State
	Input:			NULL
	Return: 		current Program State
*************************************************/ 
InterpreterState getPrgmState()
{
	return g_privateInterpreterState ;
}

/************************************************* 
	Function:		setPrgmState
	Description:	set current Program State
	Input:			thread_control_block   - interpreter info
	Input:			state                  - Program State
	Return: 		NULL
*************************************************/ 
void setPrgmState(struct thread_control_block * objThdCtrlBlockPtr, InterpreterState state)
{
    FST_INFO("setPrgmState to %d at %d", (int)state, objThdCtrlBlockPtr->is_main_thread);
	if(objThdCtrlBlockPtr->is_main_thread == MAIN_THREAD)
	{
	 	g_privateInterpreterState = state ;
		g_interpreter_publish.status = state ;
	}
	else
	{
		FST_INFO("setPrgmState Failed to %d", (int)state);
	}
}

/************************************************* 
	Function:		getProgMode
	Description:	get current Program Mode
	Input:			NULL
	Return: 		current Program Mode
*************************************************/ 
ProgMode getProgMode(struct thread_control_block * objThdCtrlBlockPtr)
{
	return objThdCtrlBlockPtr->prog_mode ;
}

/************************************************* 
	Function:		setPrgmState
	Description:	set current Program State
	Input:			thread_control_block   - interpreter info
	Input:			state                  - Program State
	Return: 		NULL
*************************************************/ 
void setProgMode(struct thread_control_block * objThdCtrlBlockPtr, ProgMode progMode)
{
    FST_INFO("setProgMode to %d at %d", (int)progMode, objThdCtrlBlockPtr->is_main_thread);
	if(objThdCtrlBlockPtr->is_main_thread == MAIN_THREAD)
	{
	 	objThdCtrlBlockPtr->prog_mode   = progMode ;
		g_interpreter_publish.progMode = progMode ;
	}
	else
	{
		FST_INFO("setProgMode Failed to %d", (int)progMode);
	}
}


/************************************************* 
	Function:		setCurLine
	Description:	set current Line info
	Input:			thread_control_block   - interpreter info
	Input:			line                   - Line XPath
	Input:			lineNum                - Line number
	Return: 		NULL
*************************************************/ 
void setCurLine(struct thread_control_block * objThdCtrlBlockPtr, char * line, int lineNum)
{
// #ifdef WIN32
//  	Instruction temp,  * tempPtr = &temp;
//      int offset = (int)&(tempPtr->line) - (int)tempPtr ;
// #else
//   // int offset = (int)&intprt_status.line - (int)&intprt_status;
//      int offset = (int)&((Instruction*)0)->line;   
// #endif  
//     FST_INFO("setCurLine %s(%d) at %d", line, strlen(line), offset);
//  //    writeShm(SHM_INTPRT_STATUS, offset, (void*)&line, sizeof(line));
//  	writeShm(SHM_INTPRT_STATUS, offset, (void*)line, strlen(line) + 1); 

	if(objThdCtrlBlockPtr->is_main_thread == MAIN_THREAD)
	{
		if(objThdCtrlBlockPtr->is_in_macro == false)
		{
			strcpy(g_interpreter_publish.current_line_path, line); 
			g_interpreter_publish.current_line_num = lineNum; 
		}
		else
		{
			FST_INFO("setCurLine Failed to %d in the macro", (int)lineNum);
		}
	}
	else
	{
		FST_INFO("setCurLine Failed to %d", (int)lineNum);
	}
}

/************************************************* 
	Function:		setWarning
	Description:	upload Error code
	Input:			warn                   - Error code
	Return: 		NULL
*************************************************/ 
#ifdef WIN32
void setWarning(__int64 warn)
#else
void setWarning(long long int warn)
#endif  
{
#ifdef WIN32
	IntprtStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->warn) - (int)tempPtr ;
    writeShm(SHM_INTPRT_STATUS, offset, (void*)&warn, sizeof(warn));
#else
	g_objInterpreterServer->sendEvent(INTERPRETER_EVENT_TYPE_ERROR, &warn);
#endif  
}

/************************************************* 
	Function:		setMessage
	Description:	upload Message code
	Input:			warn                   - Message code
	Return: 		NULL
*************************************************/ 
void setMessage(int warn)
{
#ifdef WIN32
	IntprtStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->warn) - (int)tempPtr ;
    writeShm(SHM_INTPRT_STATUS, offset, (void*)&warn, sizeof(warn));
#else
	g_objInterpreterServer->sendEvent(INTERPRETER_EVENT_TYPE_MESSAGE, &warn);
#endif  
}

/************************************************* 
	Function:		getUserOpMode (Legacy)
	Description:	get User Opration Mode
	Input:			NULL
	Return: 		User Opration Mode
*************************************************/ 
// 	UserOpMode getUserOpMode()
// 	{
// 	#ifdef WIN32
// 		CtrlStatus temp,  * tempPtr = &temp;
// 		int offset = (int)&(tempPtr->user_op_mode) - (int)tempPtr ;
// 		readShm(SHM_CTRL_STATUS, offset, (void*)&ctrl_status.user_op_mode, sizeof(ctrl_status.user_op_mode));
// 	#else
// 		int offset = (int)&((CtrlStatus*)0)->user_op_mode;
// 	#endif  
// 
// 		return ctrl_status.user_op_mode;

/************************************************* 
	Function:		setInstruction
	Description:	Send MOV* Instruction to controller
	Input:			thread_control_block   - interpreter info
	Input:			instruction            - Instruction Object
	Return: 		true - success ; false - failed
*************************************************/ 
bool setInstruction(struct thread_control_block * objThdCtrlBlockPtr, Instruction * instruction)
{
    bool ret = true;
//    int iLineNum = 0;
	// We had eaten MOV* as token. 
    if (objThdCtrlBlockPtr->is_abort)
    {
        return false;
    }
	// Speed up at 0930
//    ret = g_objRegManagerInterface->isNextInstructionNeeded();
//    if (ret == false)
 //   {
 //       FST_INFO("not permitted");
 //       return false;
 //   }
//	// setSendPermission(false);
	
//    int count = 0;
    //FST_INFO("cur state:%d", prgm_state);
//    if (objThdCtrlBlockPtr->prog_mode == STEP_MODE) 
//	//	&& (prgm_state == INTERPRETER_EXECUTE_TO_PAUSE))
//    {
//		// FST_INFO("cur state:%d in STEP_MODE ", prgm_state);
//        return false;
//    }

    do
    {
		if (instruction->is_additional == false)
		{
	     	FST_INFO("setInstruction:: instr.target.cnt = %f .", instruction->target.cnt);
#ifndef WIN32
			ret = g_objRegManagerInterface->setInstruction(instruction);
#endif
		}
		else
		{
	     	FST_INFO("setInstruction:: instr.target.cnt = %f .", instruction->target.cnt);
#ifndef WIN32
			ret = g_objRegManagerInterface->setInstruction(instruction);
#endif
		}
#ifndef WIN32
	    if (ret)
#endif
	    {
//	        if (is_backward)
//	        {
//	            is_backward = false;
//		        //    iLineNum--;
//		        //    setCurLine(iLineNum);
//	        }
	        //else
	        //{
	        //    iLineNum++;
	        //    setCurLine(iLineNum);
	        //}   
			//	iLineNum = calc_line_from_prog(objThdCtrlBlockPtr);
			//  FST_INFO("set line in setInstruction");
	        //    setLinenum(objThdCtrlBlockPtr, iLineNum);

	        if (objThdCtrlBlockPtr->prog_mode == STEP_MODE)
	        {
			    FST_INFO("In STEP_MODE, it seems that it does not need to wait");
	            // setPrgmState(objThreadCntrolBlock, INTERPRETER_EXECUTE_TO_PAUSE);   //wait until this Instruction end
            }
	    }

#ifdef WIN32
		Sleep(1);
		break ;
#else
        usleep(1000);
#endif
     //   if (count++ > 500)
     //       return false;
    }while(!ret);

    // Wait until finish 
#ifndef WIN32
    ret = g_objRegManagerInterface->isNextInstructionNeeded();
#else
    ret = true;
#endif
    FST_INFO("wait ctrl_status.is_permitted == false");
    while (ret == false)
    {
#ifdef WIN32
		Sleep(1);
		break ;
#else
        usleep(1000);
#endif
#ifndef WIN32
    	ret = g_objRegManagerInterface->isNextInstructionNeeded();
#endif
    }
    FST_INFO("ctrl_status.is_permitted == true");

    return true;
}

/*
bool getIntprtCtrl(InterpreterControl& intprt_ctrl)
{
    bool iRet = tryRead(SHM_CTRL_CMD, 0, (void*)&intprt_ctrl, sizeof(intprt_ctrl));
	if(g_lastcmd != intprt_ctrl.cmd)
    {
       FST_INFO("getIntprtCtrl = %d", intprt_ctrl.cmd);
	   g_lastcmd = (fst_base::InterpreterServerCmd)intprt_ctrl.cmd ;
	}
	return iRet ;
}
*/
	
void dealCodeStart(int program_code)
{
	struct thread_control_block * objThdCtrlBlockPtr ;
	g_launch_code_mgr_ptr->updateAll();
	std::string program_name = g_launch_code_mgr_ptr->getProgramByCode(program_code);
	if(program_name != "")
	{
        FST_INFO("start run %s ...", program_name.c_str());
		if(strcmp(getProgramName(), program_name.c_str()) == 0)
        {
        	FST_INFO("Duplicate to trigger and omit it while %s is executing ...", program_name.c_str());
        	return;
		}
		incCurrentThreadSeq();
	    // objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
	    objThdCtrlBlockPtr = getThreadControlBlock();
		if(objThdCtrlBlockPtr == NULL) return ;
		// Clear last lineNum
		setCurLine(objThdCtrlBlockPtr, (char *)"", 0);
		
//      objThdCtrlBlockPtr->prog_mode   = FULL_MODE;
//  	g_interpreter_publish.progMode  = FULL_MODE;
  		setProgMode(objThdCtrlBlockPtr, FULL_MODE);
		objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
		startFile(objThdCtrlBlockPtr, 
			(char *)program_name.c_str(), getCurrentThreadSeq());
        setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);
	}
	else 
	{
  		setWarning(FAIL_INTERPRETER_FILE_NOT_FOUND); 
	}
}

/************************************************* 
	Function:		startFile
	Description:	start running Program File
	Input:			thread_control_block   - interpreter info
	Input:			proj_name              - Program File name
	Input:			idx                    - Thread Index
	Return: 		NULL
*************************************************/ 
void startFile(struct thread_control_block * objThdCtrlBlockPtr, 
	char * proj_name, int idx)
{
	if(strlen(proj_name) < LAB_LEN)
	{
		strcpy(objThdCtrlBlockPtr->project_name, proj_name); // "prog_demo_dec"); // "BAS-EX1.BAS") ; // 
	}
	else
	{
		serror(objThdCtrlBlockPtr, 23);
		return;
	}
	// Just set to default value and it will change in the append_program_prop_mapper
	objThdCtrlBlockPtr->is_main_thread = MAIN_THREAD ;
	objThdCtrlBlockPtr->is_in_macro    = false ;
	objThdCtrlBlockPtr->iThreadIdx = idx ;
	// Refresh InterpreterPublish project_name
	
	setProgramName(objThdCtrlBlockPtr, proj_name); 
	// Start thread
	basic_thread_create(idx, objThdCtrlBlockPtr);
	// intprt_ctrl.cmd = LOAD ;
}

/*
void waitInterpreterStateleftWaiting(
	struct thread_control_block * objThdCtrlBlockPtr)
{
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState == WAITING_R)
	{
#ifdef WIN32
		Sleep(100);
		interpreterState = EXECUTE_R ;
#else
		sleep(1);
		interpreterState = getPrgmState();
		if(objThdCtrlBlockPtr->is_abort == true)
		{
			// setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSE_TO_IDLE) ;
			break;
		}
#endif
	}
}

void waitInterpreterStateToWaiting(
	struct thread_control_block * objThdCtrlBlockPtr)
{
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState != WAITING_R)
	{
#ifdef WIN32
		Sleep(100);
		// interpreterState = EXECUTE_R ;
#else
		sleep(1);
		interpreterState = getPrgmState();
		if(objThdCtrlBlockPtr->is_abort == true)
		{
			// setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSE_TO_IDLE) ;
			break;
		}
#endif
	}
}	
*/

/************************************************* 
	Function:		waitInterpreterStateleftPaused
	Description:	wait Interpreter State left Paused
	Input:			thread_control_block   - interpreter info
	Return: 		NULL
*************************************************/ 
void waitInterpreterStateleftPaused(
	struct thread_control_block * objThdCtrlBlockPtr)
{
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState == INTERPRETER_PAUSED)
	{
#ifdef WIN32
		Sleep(100);
		interpreterState = INTERPRETER_EXECUTE ;
#else
		sleep(1);
		interpreterState = getPrgmState();
		if(objThdCtrlBlockPtr->is_abort == true)
		{
			// setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSE_TO_IDLE) ;
			break;
		}
#endif
	}
}

/************************************************* 
	Function:		waitInterpreterStateleftPaused
	Description:	wait Interpreter State enter Paused
	Input:			thread_control_block   - interpreter info
	Return: 		NULL
*************************************************/ 
void waitInterpreterStateToPaused(
	struct thread_control_block * objThdCtrlBlockPtr)
{
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState != INTERPRETER_PAUSED)
	{
#ifdef WIN32
		Sleep(100);
		// interpreterState = INTERPRETER_EXECUTE ;
#else
		sleep(1);
		interpreterState = getPrgmState();
		if(objThdCtrlBlockPtr->is_abort == true)
		{
			// setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSE_TO_IDLE) ;
   			FST_INFO("waitInterpreterStateToPaused abort");
			break;
		}
		if(interpreterState == INTERPRETER_IDLE)
		{
   			FST_INFO("waitInterpreterStateToPaused = IDLE_R");
			break;
		}
#endif
	}
}	

/************************************************* 
	Function:		parseCtrlComand
	Description:	parse Controller Command
	Input:			intprt_ctrl            - Command
	Input:			requestDataPtr         - Command parameters
	Return: 		NULL
*************************************************/ 
void parseCtrlComand(InterpreterControl intprt_ctrl, void * requestDataPtr) 
	// (struct thread_control_block * objThdCtrlBlockPtr)
{
//	InterpreterState interpreterState  = IDLE_R;
	int iLineNum = 0 ;
#ifdef WIN32
	__int64 result = 0 ;
	static int lastCmd ;
#else
	static fst_base::InterpreterServerCmd lastCmd ;
#endif
//	UserOpMode userOpMode ;
	AutoMode   autoMode ;
	int        program_code ;
    thread_control_block * objThdCtrlBlockPtr = NULL;

	g_launch_code_mgr_ptr->updateAll();
#ifndef WIN32
//	RegMap reg ;
	IOPathInfo  dioPathInfo ;
	// if(intprt_ctrl.cmd != UPDATE_IO_DEV_ERROR)
	if(intprt_ctrl.cmd != fst_base::INTERPRETER_SERVER_CMD_LOAD)
        FST_INFO("parseCtrlComand: %d", intprt_ctrl.cmd);
#endif
    switch (intprt_ctrl.cmd)
    {
        case fst_base::INTERPRETER_SERVER_CMD_LOAD:
            // FST_INFO("load file_name");
            break;
        case fst_base::INTERPRETER_SERVER_CMD_DEBUG:
			memcpy(intprt_ctrl.start_ctrl, requestDataPtr, 256);
            FST_INFO("start debug %s ...", intprt_ctrl.start_ctrl);
			if(strcmp(getProgramName(), intprt_ctrl.start_ctrl) == 0)
            {
            	FST_INFO("Duplicate to execute %s ...", intprt_ctrl.start_ctrl);
				setWarning(FAIL_INTERPRETER_DUPLICATE_EXEC_MACRO) ; 
            	break;
			}
			incCurrentThreadSeq();
		    // objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			// Clear last lineNum
			setCurLine(objThdCtrlBlockPtr, (char *)"", 0);
			
       //     objThdCtrlBlockPtr->prog_mode = STEP_MODE;
  	   //	  g_interpreter_publish.progMode  = STEP_MODE;
  			setProgMode(objThdCtrlBlockPtr, STEP_MODE);
			objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
			if(strlen(intprt_ctrl.start_ctrl) == 0)
			{
			   strcpy(intprt_ctrl.start_ctrl, "lineno_test_2");
			}
            startFile(objThdCtrlBlockPtr, 
				intprt_ctrl.start_ctrl, getCurrentThreadSeq());
            setPrgmState(objThdCtrlBlockPtr, INTERPRETER_PAUSED);
	        // g_iCurrentThreadSeq++ ;
            break;
        case fst_base::INTERPRETER_SERVER_CMD_START:
			memcpy(intprt_ctrl.start_ctrl, requestDataPtr, 256);
            FST_INFO("start run %s ...", intprt_ctrl.start_ctrl);
			if(strcmp(getProgramName(), intprt_ctrl.start_ctrl) == 0)
            {
            	FST_INFO("Duplicate to execute %s ...", intprt_ctrl.start_ctrl);
				setWarning(FAIL_INTERPRETER_DUPLICATE_EXEC_MACRO) ;
            	break;
			}
			incCurrentThreadSeq();
		    // objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			// Clear last lineNum
			setCurLine(objThdCtrlBlockPtr, (char *)"", 0);
			
       //     objThdCtrlBlockPtr->prog_mode   = FULL_MODE;
  	   //	  g_interpreter_publish.progMode  = FULL_MODE;
  			setProgMode(objThdCtrlBlockPtr, FULL_MODE);
			objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
			if(strlen(intprt_ctrl.start_ctrl) == 0)
			{
			   strcpy(intprt_ctrl.start_ctrl, "lineno_test_2");
			}
			startFile(objThdCtrlBlockPtr, 
				intprt_ctrl.start_ctrl, getCurrentThreadSeq());
            setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);
	        // g_iCurrentThreadSeq++ ;
            break;
        case fst_base::INTERPRETER_SERVER_CMD_JUMP:
			memcpy(intprt_ctrl.jump_line, requestDataPtr, 256);
			if(getCurrentThreadSeq() < 0) break ;
			if(g_basic_interpreter_handle[getCurrentThreadSeq()] == 0)
			{
            	FST_ERROR("Thread exits at %d ", getPrgmState());
				break;
			}
			// objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			if(objThdCtrlBlockPtr->is_in_macro == true)
			{
				FST_ERROR("Can not JUMP macro ");
				break;
			}
			if(objThdCtrlBlockPtr->is_paused == true)
			{
            	FST_ERROR("Can not JUMP in calling Pause ");
           		break;
			}
			if(getPrgmState() == INTERPRETER_EXECUTE)
			{
            	FST_ERROR("Can not JUMP in EXECUTE_R ");
           		break;
			}
            FST_INFO("jump to line:%s", intprt_ctrl.jump_line);
			iLineNum = getLineNumFromXPathVector(objThdCtrlBlockPtr, intprt_ctrl.jump_line);
			if(iLineNum > 0)
            {
            	setLinenum(objThdCtrlBlockPtr, iLineNum);
				// Jump prog
				jump_prog_from_line(objThdCtrlBlockPtr, iLineNum);
				// Just Move to line and do not execute
            	// setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);
			}
			else
            {
            	FST_ERROR("Failed to jump to line:%d", iLineNum);
				setWarning(FAIL_INTERPRETER_ILLEGAL_LINE_NUMBER);
			}
			break;
        case fst_base::INTERPRETER_SERVER_CMD_SWITCH_STEP:
			memcpy(&intprt_ctrl.step_mode, requestDataPtr, sizeof(int));
            FST_INFO("switch Step at %d with %d", 
				getCurrentThreadSeq(), intprt_ctrl.step_mode);
			if(getCurrentThreadSeq() < 0) break ;
			if(g_basic_interpreter_handle[getCurrentThreadSeq()] == 0)
			{
            	FST_ERROR("Thread exits at %d ", getPrgmState());
				break;
			}
			// objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
            FST_INFO("SWITCH_STEP with %d", intprt_ctrl.step_mode);
            // objThdCtrlBlockPtr->prog_mode = (ProgMode)intprt_ctrl.step_mode;
  			// g_interpreter_publish.progMode  = (ProgMode)intprt_ctrl.step_mode;
  			setProgMode(objThdCtrlBlockPtr, (ProgMode)intprt_ctrl.step_mode);
            break;
        case fst_base::INTERPRETER_SERVER_CMD_FORWARD:
            FST_INFO("step forward at %d ", getCurrentThreadSeq());
			if(getCurrentThreadSeq() < 0) break ;
			if(g_basic_interpreter_handle[getCurrentThreadSeq()] == 0)
			{
            	FST_ERROR("Thread exits at %d ", getPrgmState());
				break;
			}
			// objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			if(objThdCtrlBlockPtr->is_in_macro == true)
			{
				FST_ERROR("Can not FORWARD macro ");
				break;
			}
			if(objThdCtrlBlockPtr->is_paused == true)
			{
            	FST_ERROR("Can not FORWARD in calling Pause ");
           		break;
			}
			if(getPrgmState() == INTERPRETER_EXECUTE)
			{
            	FST_ERROR("Can not FORWARD in EXECUTE_R ");
           		break;
			}
	        // Checking prog_mode on 190125 
			if(objThdCtrlBlockPtr->prog_mode != STEP_MODE)
			{
            	FST_ERROR("Can not FORWARD in other mode ");
           		break;
			}
			
            // objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
			objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;

			iLineNum = calc_line_from_prog(objThdCtrlBlockPtr);
            setLinenum(objThdCtrlBlockPtr, iLineNum);
            FST_INFO("step forward to %d ", iLineNum);
            setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);

			// Controller use the PrgmState and LineNum to check to execute 
//            FST_INFO("Enter waitInterpreterStateToPaused %d ", iLineNum);
//            waitInterpreterStateToPaused(objThdCtrlBlockPtr);

			// Use the program pointer to get the current line number.
			// to support logic
            break;
        case fst_base::INTERPRETER_SERVER_CMD_BACKWARD:
            FST_INFO("backward at %d ", getCurrentThreadSeq());
			if(getCurrentThreadSeq() < 0) break ;
			if(g_basic_interpreter_handle[getCurrentThreadSeq()] == 0)
			{
            	FST_ERROR("Thread exits at %d ", getPrgmState());
				break;
			}
			// objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			if(objThdCtrlBlockPtr->is_in_macro == true)
			{
				FST_ERROR("Can not BACKWARD macro ");
				break;
			}
			if(objThdCtrlBlockPtr->is_paused == true)
			{
            	FST_ERROR("Can not BACKWARD in calling Pause ");
           		break;
			}
			if(getPrgmState() == INTERPRETER_EXECUTE)
			{
            	FST_ERROR("Can not BACKWARD in EXECUTE_R ");
           		break;
			}
	        // Checking prog_mode on 190125 
			if(objThdCtrlBlockPtr->prog_mode != STEP_MODE)
			{
            	FST_ERROR("Can not BACKWARD in other mode ");
           		break;
			}
			// Revert checking condition on 190125
			// if(lastCmd == fst_base::INTERPRETER_SERVER_CMD_FORWARD)
			if(lastCmd != fst_base::INTERPRETER_SERVER_CMD_BACKWARD)
			{
			    // In this circumstance, 
			    // we need not call calc_line_from_prog to get the next FORWARD line.
				// Just execute last statemant
			    iLineNum = getLinenum(objThdCtrlBlockPtr);
				// SWitch prog_mode and execute_direction on 190125
                // objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
			    objThdCtrlBlockPtr->execute_direction = EXECUTE_BACKWARD ;
			
				if((objThdCtrlBlockPtr->prog_jmp_line[iLineNum - 1].type == LOGIC_TOK)
				 ||(objThdCtrlBlockPtr->prog_jmp_line[iLineNum - 1].type == END_TOK))
				{
            		FST_ERROR("Can not BACKWARD to %d(%d).",
						iLineNum, objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type);
					break ;
				}
				// In fact, It does nothing
				// iLineNum-- ;
		    	setLinenum(objThdCtrlBlockPtr, iLineNum);
            	FST_INFO("JMP to %d(%d) in the FORWARD -> BACKWARD .", 
					iLineNum,    objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type);
                setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);
				break;
			}
            // setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);  
			// In this circumstance, 
			// We had jmp to the right line, we should use the iLineNum.
            iLineNum = getLinenum(objThdCtrlBlockPtr);
            if (iLineNum < PROGRAM_START_LINE_NUM)
            {
            	FST_ERROR("Can not BACKWARD out of program ");
  				setWarning(INFO_INTERPRETER_BACK_TO_BEGIN) ; 
                break;
            }
            // if (objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type == MOTION)
//            is_backward = true;
            // else {  perror("can't back");  break;      }
            // objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
			objThdCtrlBlockPtr->execute_direction = EXECUTE_BACKWARD ;
            FST_INFO("step BACKWARD to %d ", iLineNum);
			// set_prog_from_line(objThdCtrlBlockPtr, iLineNum);
			iLineNum-- ;
			
			if((objThdCtrlBlockPtr->prog_jmp_line[iLineNum - 1].type == LOGIC_TOK)
			 ||(objThdCtrlBlockPtr->prog_jmp_line[iLineNum - 1].type == END_TOK))
			{
				FST_ERROR("Can not BACKWARD to %d(%d).",
					iLineNum, objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type);
				break ;
			}
		    setLinenum(objThdCtrlBlockPtr, iLineNum);
            setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);

			// Controller use the PrgmState and LineNum to check to execute 
//            FST_INFO("Enter waitInterpreterStateToPaused %d ", iLineNum);
//			waitInterpreterStateToPaused(objThdCtrlBlockPtr);
		    break;
		case fst_base::INTERPRETER_SERVER_CMD_RESUME:
			if(getCurrentThreadSeq() < 0) break ;
		    // objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			
			if(getPrgmState() == INTERPRETER_PAUSED)
	        {
	            FST_INFO("continue move..");
				// Not Change program mode  
				// objThdCtrlBlockPtr->prog_mode = FULL_MODE;
	            setPrgmState(objThdCtrlBlockPtr, INTERPRETER_EXECUTE);
			}
			else
			    setWarning(FAIL_INTERPRETER_NOT_IN_PAUSE);
            break;
        case fst_base::INTERPRETER_SERVER_CMD_PAUSE:
			if(getCurrentThreadSeq() < 0) break ;
			// objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			if(objThdCtrlBlockPtr->is_in_macro == true)
			{
				FST_ERROR("Can not PAUSE macro ");
				break;
			}
			if(getPrgmState() == INTERPRETER_IDLE)
			{
            	FST_ERROR("Can not PAUSE in INTERPRETER_IDLE ");
           		break;
			}
			
//            userOpMode = getUserOpMode();
//            if ((userOpMode == SLOWLY_MANUAL_MODE_U)
//            || (userOpMode == UNLIMITED_MANUAL_MODE_U))
//            {
//                objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
//            }
            setPrgmState(objThdCtrlBlockPtr, INTERPRETER_PAUSED); 
            break;
        case fst_base::INTERPRETER_SERVER_CMD_ABORT:
            FST_ERROR("abort motion");
			if(getCurrentThreadSeq() < 0) break ;
		    // objThdCtrlBlockPtr = &g_thread_control_block[getCurrentThreadSeq()];
		    objThdCtrlBlockPtr = getThreadControlBlock();
			if(objThdCtrlBlockPtr == NULL) break ;
			
  			FST_INFO("set abort motion flag.");
	        objThdCtrlBlockPtr->is_abort = true;
            // Restore program pointer
  			FST_INFO("reset prog position.");
            objThdCtrlBlockPtr->prog = objThdCtrlBlockPtr->p_buf ;
			
		    setPrgmState(objThdCtrlBlockPtr, INTERPRETER_PAUSE_TO_IDLE);
#ifdef WIN32
			Sleep(1);
#else
	        usleep(1000);
#endif
  			FST_INFO("setPrgmState(IDLE_R).");
		    setPrgmState(objThdCtrlBlockPtr, INTERPRETER_IDLE);
		    // clear line path and ProgramName
  			FST_INFO("reset ProgramName And LineNum.");
			
			resetProgramNameAndLineNum(objThdCtrlBlockPtr);
            break;
        case fst_base::INTERPRETER_SERVER_CMD_SET_AUTO_START_MODE:
			memcpy(&intprt_ctrl.autoMode, requestDataPtr, sizeof(AutoMode));
			// intprt_ctrl.RegMap.
			autoMode = intprt_ctrl.autoMode ;
			// Move to Controller
			// deal_auto_mode(autoMode);
            break;
        case fst_base::INTERPRETER_SERVER_CMD_CODE_START:
			memcpy(&intprt_ctrl.program_code, requestDataPtr, sizeof(AutoMode));
			// intprt_ctrl.RegMap.
			program_code = intprt_ctrl.program_code ;
			// Move to Controller
			dealCodeStart(program_code);
            break;
        default:
            break;

    }
	lastCmd = intprt_ctrl.cmd;
  	//		FST_INFO("left parseCtrlComand.");
}

/************************************************* 
	Function:		forgesight_load_programs_path
	Description:	load programs path
	Input:			NULL
	Return: 		NULL
*************************************************/ 
void forgesight_load_programs_path()
{
	std::string data_path = "";
	g_files_manager_data_path = "";
#ifdef WIN32
	TCHAR pBuf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pBuf);
    g_files_manager_data_path = std::string(pBuf) + std::string(DATA_PATH);
#else
	if(getenv("ROBOT_DATA_PREFIX") != NULL)
		g_files_manager_data_path = string(getenv("ROBOT_DATA_PREFIX"));
	
	if(g_files_manager_data_path.length() == 0)
	{
	    fst_parameter::ParamGroup param_;
	    param_.loadParamFile("/root/install/share/configuration/machine/programs_path.yaml");
	    param_.getParam("file_manager/programs_path", g_files_manager_data_path);
	}
	else
	{
	    fst_parameter::ParamGroup param_;
	    param_.loadParamFile("/root/install/share/configuration/machine/programs_path.yaml");
	    param_.getParam("file_manager/data_path", data_path);
		g_files_manager_data_path.append(data_path);
	}
	FST_INFO("forgesight_load_programs_path: %s .", g_files_manager_data_path.c_str());
#endif
	
}

/************************************************* 
	Function:		forgesight_get_programs_path
	Description:	get programs path
	Input:			NULL
	Return: 		programs path
*************************************************/ 
char * forgesight_get_programs_path()
{
	return (char *)g_files_manager_data_path.c_str();
}

/************************************************* 
	Function:		forgesight_load_wait_time_out_config
	Description:	load programs path
	Input:			NULL
	Return: 		NULL
*************************************************/ 
void forgesight_load_wait_time_out_config()
{
#ifdef WIN32
    g_wait_time_out_config = 10;
#else
    fst_parameter::ParamGroup param_;
    param_.loadParamFile("/root/install/share/configuration/machine/prg_interpreter_config.yaml");
    param_.getParam("wait_time/time_out", g_wait_time_out_config);
	FST_INFO("forgesight_load_wait_time_out_config: %d .", g_wait_time_out_config);
	
	if(g_wait_time_out_config <= 0)
	    g_wait_time_out_config = MAX_WAIT_SECOND ;
#endif
	
}

/************************************************* 
	Function:		forgesight_get_programs_path
	Description:	get programs path
	Input:			NULL
	Return: 		programs path
*************************************************/ 
int forgesight_get_wait_time_out_config()
{
	return g_wait_time_out_config;
}

/************************************************* 
	Function:		initInterpreter
	Description:	init interpretor
	Input:			NULL
	Return: 		programs path
*************************************************/ 
void initInterpreter()
{
	xmlInitParser();
	g_privateInterpreterState = INTERPRETER_IDLE ;
	
	forgesight_load_programs_path();
	forgesight_load_wait_time_out_config();
	g_launch_code_mgr_ptr = new LaunchCodeMgr(g_files_manager_data_path);
	g_home_pose_mgr_ptr   = new HomePoseMgr(g_files_manager_data_path);
#ifdef WIN32
	import_external_resource("config\\user_defined_variable.xml", g_vecKeyVariables);
#else
	import_external_resource(
		"/root/install/share/configuration/machine/user_defined_variable.xml", 
		g_vecKeyVariables);
#endif
}

/************************************************* 
	Function:		forgesight_get_programs_path
	Description:	get programs path
	Input:			NULL
	Return: 		programs path
*************************************************/ 
bool forgesight_find_external_resource(char *vname, key_variable& keyVar)
{
	// Otherwise, try global vars.
	for(unsigned i=0; i < g_vecKeyVariables.size(); i++)
	{
	//	FST_INFO("forgesight_find_external_resource: %s .", g_vecKeyVariables[i].key_name);
		if(!strcmp(g_vecKeyVariables[i].key_name, vname)) {
			keyVar = g_vecKeyVariables[i] ;
			return true;
		}
	}
	return false;
}

void updateHomePoseMgr()
{
	g_home_pose_mgr_ptr->updateAll();
}

checkHomePoseResult checkSingleHomePoseByCurrentJoint(int idx, Joint currentJoint)
{
	return g_home_pose_mgr_ptr->checkSingleHomePoseByJoint(idx, currentJoint);
}

/************************************************* 
	Function:		uninitInterpreter
	Description:	uninit interpretor
	Input:			NULL
	Return: 		programs path
*************************************************/ 
void uninitInterpreter()
{
	delete g_launch_code_mgr_ptr;
	delete g_home_pose_mgr_ptr;
	xmlCleanupParser();
}

/************************************************* 
	Function:		updateIOError (Legacy)
	Description:	update IO Error
	Input:			NULL
	Return: 		NULL
*************************************************/ 
void updateIOError()
{
#ifndef WIN32
//	U64 result = SUCCESS ;
//	result = IOInterface::instance()->updateIOError();
//    if (result != SUCCESS)
//    {
//		setWarning(result) ; 
//    }
#endif
}

