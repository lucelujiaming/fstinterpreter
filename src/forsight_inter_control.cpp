#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include "forsight_inter_control.h"
#include "forsight_innercmd.h"
#include "forsight_program_property.h"
#include "forsight_io_mapping.h"
#include "forsight_io_controller.h"
#ifndef WIN32
#include "io_interface.h"
#include "error_code.h"
#endif

#ifdef USE_FORSIGHT_REGISTERS_MANAGER
#ifndef WIN32
#include "reg_manager/reg_manager_interface.h"
using namespace fst_reg ;
#endif
#include "reg_manager/forsight_registers_manager.h"

#else
#include "reg-shmi/forsight_registers.h"
#include "reg-shmi/forsight_op_regs_shmi.h"
#endif

#define VELOCITY    (500)
using namespace std;

static std::vector<Instruction> g_script;
static int block_start, block_end;
bool terminated = false;

bool is_backward= false;
InterpreterState prgm_state = IDLE_R;
//bool send_flag = false;
// int target_line;
static IntprtStatus intprt_status;
static Instruction instruction;
static CtrlStatus ctrl_status;
static InterpreterControl intprt_ctrl;

static InterpreterState g_privateInterpreterState;

extern jmp_buf e_buf; /* hold environment for longjmp() */
extern struct thread_control_block g_thread_control_block[NUM_THREAD];

#ifdef WIN32
extern HANDLE    g_basic_interpreter_handle[NUM_THREAD];
#else
extern pthread_t g_basic_interpreter_handle[NUM_THREAD];
#endif
int  g_iCurrentThreadSeq = -1 ;  // minus one add one equals to zero

vector<string> split(string str,string pattern)
{
	string::size_type pos;
	vector<string> result;
	str+=pattern;
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

bool parseScript(const char* fname)
{
    Instruction instr;
    char command[256];
    string path = "/home/fst/Program/";
    path = path + fname;
    ifstream fin(path.c_str());
    int count = 0;
    while(fin.getline(command, sizeof(command)))
    {
        printf("command:%s\n",command);
#ifdef USE_XPATH
	    // itoa(++count, instr.line, 10);
        sprintf(instr.line, "%d", ++count);
#else
        instr.line = ++count;
#endif
        vector<string> result=split(command, " "); //use space to split
        if (result[0] == "LOOP")
        {
            instr.type = LOGIC_TOK; // instr.type = LOOP;
            if (result.size() == 1)
                instr.loop_cnt = -1;
            else 
                instr.loop_cnt = atoi(result[1].c_str());
            printf("loop_cnt:%d\n", instr.loop_cnt);
        }
        else if (result[0] == "END")
        {
            instr.type = END_PROG; // END;
        }
        else
        {
            instr.type = MOTION;
            if (result[0] == "moveJ")
            {
                if(result.size() < 7)
                {
                    printf("Error:moveJ j1 j2 j3 j4 j5 j6 [smooth]\n");
                    return false;
                }
                instr.target.type = MOTION_JOINT;
                instr.target.joint_target = *(Joint*)&result[1];
                if (result.size() == 7)
                    instr.target.cnt = -1;
                else
                    instr.target.cnt = atoi(result[7].c_str());
            }
            if (result[0] == "moveL")
            {
                if(result.size() < 7)
                {
                    printf("Error:moveL x y z a b c [smooth]\n");
                    return false;
                }
                instr.target.type = MOTION_LINE;
                instr.target.pose_target = *(PoseEuler*)&result[1];
                if (result.size() == 7)
                    instr.target.cnt = -1;
                else
                    instr.target.cnt = atoi(result[7].c_str());

            }
            if (result[0] == "moveC")
            {
                if(result.size() < 13)
                {
                    perror("Error:moveC x1 y1 z1 a1 b1 c1 x2 y2 z2 a2 b2 c2 [smooth]\n");
                    return false;
                }
                instr.target.type = MOTION_CIRCLE;
                instr.target.circle_target = *(CircleTarget*)&result[1];
                if (result.size() == 7)
                    instr.target.cnt = -1;
                else
                    instr.target.cnt = atoi(result[7].c_str());
            }            
        }
        g_script.push_back(instr);
        memset((void*)&instr, 0, sizeof(instr));
        memset(command, 0, sizeof(command));
    }
    fin.close();

    return true;
}

void findLoopEnd(int index)
{
    for(int i = index; i < g_script.size(); i++)
    {
        if (g_script[i].type == END_PROG) // END)
        {
            block_end = i -1;
            return;
        }
    }
    block_end = g_script.size() - 1;
}

void setMoveCommandDestination(MoveCommandDestination movCmdDst)
{ 
    writeShm(SHM_INTPRT_DST, 0, (void*)&movCmdDst, sizeof(movCmdDst));
}

void getMoveCommandDestination(MoveCommandDestination& movCmdDst)
{
    readShm(SHM_INTPRT_DST, 0, (void*)&movCmdDst, sizeof(movCmdDst));
}

void copyMoveCommandDestination(MoveCommandDestination& movCmdDst)
{
    getMoveCommandDestination(movCmdDst);
    setMoveCommandDestination(movCmdDst);
}

void setIntprtDataFlag(bool flag)
{
#ifdef WIN32
	CtrlStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->is_data_ready) - (int)tempPtr ;
#else
    int offset = &((CtrlStatus*)0)->is_data_ready;
#endif  
    writeShm(SHM_CTRL_STATUS, offset, (void*)&flag, sizeof(flag));
}

bool getIntprtDataFlag()
{
    bool is_data_ready;
#ifdef WIN32
	CtrlStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->is_data_ready) - (int)tempPtr ;
#else
    int offset = &((CtrlStatus*)0)->is_data_ready;  
#endif     
    readShm(SHM_CTRL_STATUS, offset, (void*)&is_data_ready, sizeof(is_data_ready));
    return is_data_ready;
}

void returnRegInfo(RegMap info)
{
    printf("returnRegInfo to %d\n", (int)info.type);
	
	printf("reg.value = (");
	for(int iRet = 0 ; iRet < 100 ; iRet++)
	{
	    printf("%08X, ", info.value[iRet]);
	}
	printf(") \n");
				
    writeShm(SHM_REG_IO_INFO, 0, (void*)&info, sizeof(RegMap));
	setIntprtDataFlag(true);
}

void returnDIOInfo(IOPathInfo& info)
{
    printf("returnDIOInfo to %s:%d\n", info.dio_path, (int)info.value);
    writeShm(SHM_REG_IO_INFO, 0, (char*)&info.value, sizeof(char));
	setIntprtDataFlag(true);
}

void returnIODeviceInfo(char * info, int iNum)
{
    writeShm(SHM_REG_IO_INFO, 0, (char*)info, sizeof(IODeviceInfoShm) * iNum);
	setIntprtDataFlag(true);
}

InterpreterState getPrgmState()
{
	return g_privateInterpreterState ;
}

void setPrgmState(InterpreterState state)
{
	g_privateInterpreterState = state ;
#ifdef WIN32
    IntprtStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->state) - (int)tempPtr ;
#else
    int offset = &((IntprtStatus*)0)->state;
#endif
    printf("setPrgmState to %d\n", (int)state);
    writeShm(SHM_INTPRT_STATUS, offset, (void*)&state, sizeof(state));
}

void setCurLine(char * line)
{
#ifdef WIN32
	Instruction temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->line) - (int)tempPtr ;
#else
   // int offset = (int)&intprt_status.line - (int)&intprt_status;
    int offset = &((Instruction*)0)->line;   
#endif  
    printf("setCurLine %s(%d) at %d\n", line, strlen(line), offset);
//    writeShm(SHM_INTPRT_STATUS, offset, (void*)&line, sizeof(line));
	writeShm(SHM_INTPRT_STATUS, offset, (void*)line, strlen(line) + 1); 
}

#ifdef WIN32
void setWarning(__int64 warn)
#else
void setWarning(long long int warn)
#endif  
{
#ifdef WIN32
	IntprtStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->warn) - (int)tempPtr ;
#else
    int offset = &((IntprtStatus*)0)->warn;
#endif  
    writeShm(SHM_INTPRT_STATUS, offset, (void*)&warn, sizeof(warn));
}

void setSendPermission(bool flag)
{
#ifdef WIN32
	CtrlStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->is_permitted) - (int)tempPtr ;
#else
    int offset = &((CtrlStatus*)0)->is_permitted;
#endif  
    writeShm(SHM_CTRL_STATUS, offset, (void*)&flag, sizeof(flag));
}

void getSendPermission()
{
#ifdef WIN32
	CtrlStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->is_permitted) - (int)tempPtr ;
#else
    int offset = &((CtrlStatus*)0)->is_permitted;
#endif  
    readShm(SHM_CTRL_STATUS, offset, (void*)&ctrl_status.is_permitted, sizeof(ctrl_status.is_permitted));
}

UserOpMode getUserOpMode()
{
#ifdef WIN32
	CtrlStatus temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->user_op_mode) - (int)tempPtr ;
#else
    int offset = &((CtrlStatus*)0)->user_op_mode;
#endif  
    readShm(SHM_CTRL_STATUS, offset, (void*)&ctrl_status.user_op_mode, sizeof(ctrl_status.user_op_mode));

    return ctrl_status.user_op_mode;
}

/*SysCtrlMode getMotionMode()*/
//{
    //int offset = &((CtrlStatus*)0)->sys_ctrl_mode;
    //readShm(SHM_CTRL_STATUS, offset, (void*)&ctrl_status.sys_ctrl_mode, sizeof(ctrl_status.sys_ctrl_mode));

    //return ctrl_status.sys_ctrl_mode;
//}

bool setInstruction(struct thread_control_block * objThdCtrlBlockPtr, Instruction * instruction)
{
    int iLineNum = 0;
	// We had eaten MOV* as token. 
    if (objThdCtrlBlockPtr->is_abort)
    {
        // target_line++;
        return false;
    }
    getSendPermission();
    if (!ctrl_status.is_permitted)
    {
        printf("not permitted\n");
        return false;
    }
	
	setSendPermission(false);
	
    int count = 0;
    bool ret;
    //printf("cur state:%d\n", prgm_state);
    if ((objThdCtrlBlockPtr->prog_mode == STEP_MODE) 
		&& (prgm_state == EXECUTE_TO_PAUSE_T))
    {
        if (isInstructionEmpty(SHM_INTPRT_CMD))
        {
            printf("check if step is done in setInstruction\n");
            setPrgmState(PAUSED_R);
        }
		// printf("cur state:%d in STEP_MODE \n", prgm_state);
        return false;
    }

    // if (prgm_state != EXECUTE_R)
    //    return false;
    do
    {
		if (instruction->is_additional == false)
		{
	     	printf("instr.target.cnt = %f .\n", instruction->target.cnt);
			ret = tryWrite(SHM_INTPRT_CMD, 0, 
				(void*)instruction, sizeof(Instruction));
		}
		else
		{
	     	printf("instr.target.cnt = %f .\n", instruction->target.cnt);
			ret = tryWrite(SHM_INTPRT_CMD, 0, 
				(void*)instruction, 
				sizeof(Instruction) 
					+ sizeof(AdditionalInfomation) * instruction->add_num);
		}
#ifndef WIN32
	    if (ret)
#endif
	    {
	        if (is_backward)
	        {
	            is_backward = false;
		        //    iLineNum--;
		        //    setCurLine(iLineNum);
	        }
		        //else
		        //{
		        //    iLineNum++;
		        //    setCurLine(iLineNum);
		        //}   
		//	iLineNum = calc_line_from_prog(objThdCtrlBlockPtr);
		//  printf("set line in setInstruction\n");
        //    setLinenum(objThdCtrlBlockPtr, iLineNum);

	        if (objThdCtrlBlockPtr->prog_mode == STEP_MODE)
	            setPrgmState(EXECUTE_TO_PAUSE_T);   //wait until this Instruction end
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
    getSendPermission();
    printf("wait ctrl_status.is_permitted == false\n");
    while (ctrl_status.is_permitted == false)
    {
#ifdef WIN32
		Sleep(1);
		break ;
#else
        usleep(1000);
#endif
        getSendPermission();
    }
    printf("ctrl_status.is_permitted == true\n");

    return true;
}

bool getIntprtCtrl()
{
    return tryRead(SHM_CTRL_CMD, 0, (void*)&intprt_ctrl, sizeof(intprt_ctrl));
}

void startFile(struct thread_control_block * objThdCtrlBlockPtr, 
	char * proj_name, int idx)
{
	strcpy(objThdCtrlBlockPtr->project_name, proj_name); // "prog_demo_dec"); // "BAS-EX1.BAS") ; // 
	objThdCtrlBlockPtr->is_main_thread = MAIN_THREAD ;
	objThdCtrlBlockPtr->iThreadIdx = idx ;
	append_program_prop_mapper(objThdCtrlBlockPtr, proj_name);
	base_thread_create(idx, objThdCtrlBlockPtr);
	intprt_ctrl.cmd = LOAD ;
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
			// setPrgmState(PAUSE_TO_IDLE_T) ;
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
			// setPrgmState(PAUSE_TO_IDLE_T) ;
			break;
		}
#endif
	}
}	
*/

void waitInterpreterStateleftPaused(
	struct thread_control_block * objThdCtrlBlockPtr)
{
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState == PAUSED_R)
	{
#ifdef WIN32
		Sleep(100);
		interpreterState = EXECUTE_R ;
#else
		sleep(1);
		interpreterState = getPrgmState();
		if(objThdCtrlBlockPtr->is_abort == true)
		{
			// setPrgmState(PAUSE_TO_IDLE_T) ;
			break;
		}
#endif
	}
}

void waitInterpreterStateToPaused(
	struct thread_control_block * objThdCtrlBlockPtr)
{
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState != PAUSED_R)
	{
#ifdef WIN32
		Sleep(100);
		// interpreterState = EXECUTE_R ;
#else
		sleep(1);
		interpreterState = getPrgmState();
		if(objThdCtrlBlockPtr->is_abort == true)
		{
			// setPrgmState(PAUSE_TO_IDLE_T) ;
			break;
		}
#endif
	}
}	

void parseCtrlComand() // (struct thread_control_block * objThdCtrlBlockPtr)
{
#ifdef WIN32
	__int64 result = 0 ;
#else
	U64 result = SUCCESS ;
#endif

	RegMap reg ;
	IOPathInfo  dioPathInfo ;
	
	int iLineNum = 0 ;
	static InterpreterCommand lastCmd ;
	UserOpMode mode ;
    thread_control_block * objThdCtrlBlockPtr = NULL;

#ifndef WIN32
    int iIONum = 0 ;
	fst_io_manager::IODeviceInfo * objIODeviceInfoPtr ;
	char * objCharPtr ;
	IODeviceInfoShm * objIODeviceInfoShmPtr ;
	
	char tempDebug[1024];
	int iSeq = 0 ;
	char * testDebug = 0 ;
	RegChgList  * regChgList ;
	ChgFrameSimple * chgFrameSimple ;
//	RegManagerInterface objRegManagerInterface("share/configuration/machine");
	std::vector<BaseRegData> vecRet ; 
    char * strChgRegLst ;
	// if(intprt_ctrl.cmd != UPDATE_IO_DEV_ERROR)
    printf("parseCtrlComand: %d\n", intprt_ctrl.cmd);
#endif
    switch (intprt_ctrl.cmd)
    {
        case LOAD:
            // printf("load file_name\n");
            break;
        case JUMP:
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
#ifdef USE_XPATH
            printf("jump to line:%s\n", intprt_ctrl.line);
			iLineNum = getLineNumFromXPathVector(intprt_ctrl.line);
#else
            printf("jump to line:%d\n", intprt_ctrl.line);
			iLineNum = intprt_ctrl.line;
            setLinenum(objThdCtrlBlockPtr, iLineNum);
#endif
            // setPrgmState(EXECUTE_R);
			break;
        case DEBUG:
            printf("debug...\n");
			g_iCurrentThreadSeq++ ;
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			
            objThdCtrlBlockPtr->prog_mode = STEP_MODE;
			objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
            setPrgmState(PAUSED_R);
			if(strlen(intprt_ctrl.start_ctrl.file_name) == 0)
			{
			   strcpy(intprt_ctrl.start_ctrl.file_name, "prog_1");
			}
            startFile(objThdCtrlBlockPtr, 
				intprt_ctrl.start_ctrl.file_name, g_iCurrentThreadSeq);
	        // g_iCurrentThreadSeq++ ;
            break;
        case START:
            printf("start run...\n");
			g_iCurrentThreadSeq++ ;
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			
            objThdCtrlBlockPtr->prog_mode = FULL_MODE;
			objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
            setPrgmState(EXECUTE_R);
			if(strlen(intprt_ctrl.start_ctrl.file_name) == 0)
			{
			   strcpy(intprt_ctrl.start_ctrl.file_name, "sr_test");
			}
			startFile(objThdCtrlBlockPtr, 
				intprt_ctrl.start_ctrl.file_name, g_iCurrentThreadSeq);
	        // g_iCurrentThreadSeq++ ;
            break;
        case FORWARD:
            printf("step forward\n");
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			if(objThdCtrlBlockPtr->is_paused == true)
			{
            	printf("Can not FORWARD in PAUSED_R \n");
           		break;
			}
			
            objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
			objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
            // target_line++;
            iLineNum = getLinenum(objThdCtrlBlockPtr);
            printf("step forward to %d \n", iLineNum);
            setPrgmState(EXECUTE_R);
			
            printf("Enter waitInterpreterStateToPaused %d \n", iLineNum);
			waitInterpreterStateToPaused(objThdCtrlBlockPtr);
			// target_line++ in setInstruction
            printf("Left  waitInterpreterStateToPaused %d \n", iLineNum);

			// Use the program pointer to get the current line number.
			// to support logic
			iLineNum = calc_line_from_prog(objThdCtrlBlockPtr);
            setLinenum(objThdCtrlBlockPtr, iLineNum);
            break;
        case BACKWARD:
            printf("backward\n");
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			if(objThdCtrlBlockPtr->is_paused == true)
			{
            	printf("Can not BACKWARD in PAUSED_R \n");
           		break;
			}

			if(lastCmd == FORWARD)
			{
			    // In this circumstance, 
			    // call calc_line_from_prog to get the next FORWARD line.
			    iLineNum = calc_line_from_prog(objThdCtrlBlockPtr);
				if((objThdCtrlBlockPtr->prog_jmp_line[iLineNum - 1].type == LOGIC_TOK)
				 ||(objThdCtrlBlockPtr->prog_jmp_line[iLineNum - 1].type == END_TOK))
				{
            		printf("Can not BACKWARD to %d(%d).\n",
						iLineNum, objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type);
					break ;
				}
				iLineNum-- ;
		    	setLinenum(objThdCtrlBlockPtr, iLineNum);
            	printf("JMP to %d(%d) in the FORWARD -> BACKWARD .\n", 
					iLineNum,    objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type);
				break;
			}
            // setPrgmState(EXECUTE_R);  
			// In this circumstance, 
			// We had jmp to the right line, we should use the iLineNum.
            iLineNum = getLinenum(objThdCtrlBlockPtr);
            if (iLineNum < 1)
                break;
            // if (objThdCtrlBlockPtr->prog_jmp_line[iLineNum].type == MOTION)
            is_backward = true;
            // else {  perror("can't back\n");  break;      }
            objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
			objThdCtrlBlockPtr->execute_direction = EXECUTE_BACKWARD ;
            setPrgmState(EXECUTE_R);

            printf("Enter waitInterpreterStateToPaused %d \n", iLineNum);
			waitInterpreterStateToPaused(objThdCtrlBlockPtr);
			// target_line-- in setInstruction
            printf("Left  waitInterpreterStateToPaused %d \n", iLineNum);
			
			iLineNum-- ;
		    setLinenum(objThdCtrlBlockPtr, iLineNum);
		    break;
		case CONTINUE:
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			
			if(getPrgmState() == PAUSED_R)
	        {
	            printf("continue move..\n");
				// Not Change program mode  
				// objThdCtrlBlockPtr->prog_mode = FULL_MODE;
	            setPrgmState(EXECUTE_R);
			}
			else
			    setWarning(FAIL_INTERPRETER_NOT_IN_PAUSE);
            break;
        case PAUSE:
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			
            mode = getUserOpMode();
            if ((mode == SLOWLY_MANUAL_MODE_U)
            || (mode == UNLIMITED_MANUAL_MODE_U))
            {
                objThdCtrlBlockPtr->prog_mode = STEP_MODE ;
            }
            setPrgmState(PAUSED_R); 
            break;
        case ABORT:
            printf("abort motion\n");
			if(g_iCurrentThreadSeq < 0) break ;
		    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			
	        objThdCtrlBlockPtr->is_abort = true;
            // target_line = getMaxLineNum();
            // target_line = 0;
            // Restore program pointer
            objThdCtrlBlockPtr->prog = objThdCtrlBlockPtr->p_buf ;
			
		    setPrgmState(PAUSE_TO_IDLE_T);
#ifdef WIN32
			Sleep(1);
#else
	        usleep(1000);
#endif
  			printf("setPrgmState(IDLE_R).\n");
		    setPrgmState(IDLE_R);
            break;
        case READ_REG:
			// intprt_ctrl.RegMap.
			reg = intprt_ctrl.reg ;
			memset(reg.value, 0x00, 1024);
			forgesight_read_reg(reg);
			returnRegInfo(reg);
            break;
        case MOD_REG:
			// intprt_ctrl.RegMap.
			reg = intprt_ctrl.reg ;
			forgesight_mod_reg(reg);
  	        printf("reg.type = %d end.\n", reg.type);
            break;
        case DEL_REG:
			// intprt_ctrl.RegMap.
			reg = intprt_ctrl.reg ;
			forgesight_del_reg(reg);
  	        printf("reg.type = %d end.\n", reg.type);
            break;
        case READ_IO:
			// intprt_ctrl.RegMap.
			// dioMap  = intprt_ctrl.dio ;
			// forgesight_read_dio(regIOInfo.dio);
			dioPathInfo = intprt_ctrl.dioPathInfo;
            printf("READ_IO:: path : %s\n", dioPathInfo.dio_path); 

			dioPathInfo.value = get_io_interface_status(dioPathInfo.dio_path);
			returnDIOInfo(dioPathInfo);
            break;
        case MOD_IO:
			dioPathInfo = intprt_ctrl.dioPathInfo;
			printf("MOD_IO: %s:%d.\n", 
						dioPathInfo.dio_path, dioPathInfo.value);
			set_io_interface_status(
					dioPathInfo.dio_path, dioPathInfo.value);
            break;
#ifndef WIN32
        case READ_IO_DEV_INFO:
			iIONum = IOInterface::instance()->getIODevNum();
			printf("READ_IO_DEV_INFO::getIODevNum: start: %d.\n", iIONum);
 			objIODeviceInfoPtr = IOInterface::instance()->getDevInfoPtr();
			objCharPtr = (char *)malloc(sizeof(int) + sizeof(IODeviceInfoShm) * (iIONum + 1));
			objIODeviceInfoShmPtr = (IODeviceInfoShm *)(objCharPtr + sizeof(int));
			iIONum = iIONum + 1;
			memcpy(objCharPtr, &iIONum, sizeof(int));
			iIONum = iIONum - 1;
			memset(objIODeviceInfoShmPtr, 0x00, sizeof(IODeviceInfoShm) * iIONum);
			for(int i = 0 ; i < iIONum ; i++)
			{
			    memcpy(objIODeviceInfoShmPtr[i].path, objIODeviceInfoPtr[i].path.c_str(), 
					objIODeviceInfoPtr[i].path.length());
				objIODeviceInfoShmPtr[i].id = objIODeviceInfoPtr[i].id;
				
			    memcpy(objIODeviceInfoShmPtr[i].communication_type, 
					objIODeviceInfoPtr[i].communication_type.c_str(), 
					objIODeviceInfoPtr[i].communication_type.length());
				objIODeviceInfoShmPtr[i].device_number = objIODeviceInfoPtr[i].device_number;
				objIODeviceInfoShmPtr[i].device_type   = objIODeviceInfoPtr[i].device_type;
				objIODeviceInfoShmPtr[i].input         = objIODeviceInfoPtr[i].input;
				objIODeviceInfoShmPtr[i].output        = objIODeviceInfoPtr[i].output;
		    }

			if(iIONum == 0)
			{
			    memcpy(objIODeviceInfoShmPtr[iIONum].path, 
							"root/IO/RS485/1/DO/1", strlen("root/IO/RS485/1/DO/1"));
				objIODeviceInfoShmPtr[iIONum].id = 1;
			    memcpy(objIODeviceInfoShmPtr[iIONum].communication_type, "RS485", strlen("RS485"));
				objIODeviceInfoShmPtr[iIONum].device_number = 1;
				objIODeviceInfoShmPtr[iIONum].device_type   = 2;
				objIODeviceInfoShmPtr[iIONum].input         = 0x22;
				objIODeviceInfoShmPtr[iIONum].output        = 0x33;
				printf("%d: READ_IO_DEV_INFO::getIODevNum with Fack Data: end: %d.\n", __LINE__, iIONum);
				returnIODeviceInfo(objCharPtr, iIONum + 1);
			}
			else
			{
				printf("%d: READ_IO_DEV_INFO::getIODevNum with Real Data: end: %d.\n", __LINE__, iIONum);
				returnIODeviceInfo(objCharPtr, iIONum);
			}
            break;
//        case UPDATE_IO_DEV_ERROR:
//			result = IOInterface::instance()->updateIOError();
// 			setWarning(result) ; 
//          break;
#endif
        case READ_SMLT_STS:
			dioPathInfo = intprt_ctrl.dioPathInfo;
			printf("READ_SMLT_STS: %s:%d .\n", dioPathInfo.dio_path, dioPathInfo.value);
			objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			forgesight_read_io_emulate_status(dioPathInfo.dio_path, (int &)dioPathInfo.value);
			returnDIOInfo(dioPathInfo);
            break;
        case MOD_SMLT_STS:
			dioPathInfo = intprt_ctrl.dioPathInfo;
			printf("MOD_SMLT_STS: %s:%d .\n", dioPathInfo.dio_path, dioPathInfo.value);
			objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			forgesight_mod_io_emulate_status(dioPathInfo.dio_path, dioPathInfo.value);
            break;
        case MOD_SMLT_VAL:
			dioPathInfo = intprt_ctrl.dioPathInfo;
			printf("MOD_SMLT_VAL: %s:%d .\n", dioPathInfo.dio_path, dioPathInfo.value);
			objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
			forgesight_mod_io_emulate_value(dioPathInfo.dio_path, dioPathInfo.value);
            break;
#ifndef WIN32
        case READ_CHG_PR_LST:
			vecRet.clear(); 
			vecRet = forgesight_read_valid_pr_lst(0, 255);
			
			memset(tempDebug, 0x00, 1024);
			strcpy(tempDebug, "PR:");
			regChgList = (RegChgList *)malloc(sizeof(RegChgList) + MAX_PR_REG_ID * sizeof(ChgFrameSimple));
			chgFrameSimple = (ChgFrameSimple *)((char *)regChgList + sizeof(RegChgList)) ;
			strChgRegLst = (char *)regChgList ;
			memset(strChgRegLst, 0x00, sizeof(RegChgList) + MAX_PR_REG_ID * sizeof(ChgFrameSimple));
			regChgList->command = intprt_ctrl.cmd ;
			regChgList->count   = vecRet.size();
			iSeq = 0 ;
			for(vector<BaseRegData>::iterator it = vecRet.begin(); it != vecRet.end(); ++it)
			{
				sprintf(tempDebug, "%s%d:%s;", tempDebug, it->id, it->comment);
				chgFrameSimple[iSeq].id = it->id ;
				memcpy(chgFrameSimple[iSeq].comment, it->comment, 32);
				printf("             %d:%s;\n", it->id, it->comment);
			    iSeq++ ;
			}
			printf("temp: %s  (%d) .\n", tempDebug, vecRet.size());
			
			for (int i = 0 ; i < sizeof(RegChgList) + 1 * sizeof(ChgFrameSimple) ; i++)
				printf("GET:: data: %d\n", strChgRegLst[i]);
			
            writeShm(SHM_CHG_REG_LIST_INFO, 0, (void*)regChgList, 
				sizeof(RegChgList) + MAX_PR_REG_ID * sizeof(ChgFrameSimple));
	        setIntprtDataFlag(true);
            break;
        case READ_CHG_SR_LST:
			vecRet.clear(); 
			vecRet = forgesight_read_valid_sr_lst(0, 255);
			
			memset(tempDebug, 0x00, 1024);
			strcpy(tempDebug, "SR:");
			regChgList = (RegChgList *)malloc(sizeof(RegChgList) + MAX_SR_REG_ID * sizeof(ChgFrameSimple));
			chgFrameSimple = (ChgFrameSimple *)((char *)regChgList + sizeof(RegChgList)) ;
			strChgRegLst = (char *)regChgList ;
			memset(strChgRegLst, 0x00, sizeof(RegChgList) + MAX_SR_REG_ID * sizeof(ChgFrameSimple));
			regChgList->command = intprt_ctrl.cmd ;
			regChgList->count   = vecRet.size();
			iSeq = 0 ;
			for(vector<BaseRegData>::iterator it = vecRet.begin(); it != vecRet.end(); ++it)
			{
				sprintf(tempDebug, "%s%d:%s;", tempDebug, it->id, it->comment);
				chgFrameSimple[iSeq].id = it->id ;
				memcpy(chgFrameSimple[iSeq].comment, it->comment, 32);
				printf("             %d:%s;\n", it->id, it->comment);
			    iSeq++ ;
			}
			printf("temp: %s  (%d) .\n", tempDebug, vecRet.size());
				
            writeShm(SHM_CHG_REG_LIST_INFO, 0, (void*)regChgList, 
				sizeof(RegChgList) + MAX_SR_REG_ID * sizeof(ChgFrameSimple));;
	        setIntprtDataFlag(true);
            break;
        case READ_CHG_R_LST:
			vecRet.clear(); 
			vecRet = forgesight_read_valid_r_lst(0, 255);
			memset(tempDebug, 0x00, 1024);
			strcpy(tempDebug, "R:");
			regChgList = (RegChgList *)malloc(sizeof(RegChgList) + MAX_R_REG_ID * sizeof(ChgFrameSimple));
			chgFrameSimple = (ChgFrameSimple *)((char *)regChgList + sizeof(RegChgList)) ;
			strChgRegLst = (char *)regChgList ;
			memset(strChgRegLst, 0x00, sizeof(RegChgList) + MAX_R_REG_ID * sizeof(ChgFrameSimple));
			regChgList->command = intprt_ctrl.cmd ;
			regChgList->count   = vecRet.size();
			iSeq = 0 ;
			for(vector<BaseRegData>::iterator it = vecRet.begin(); it != vecRet.end(); ++it)
			{
				sprintf(tempDebug, "%s%d:%s;", tempDebug, it->id, it->comment);
				chgFrameSimple[iSeq].id = it->id ;
				memcpy(chgFrameSimple[iSeq].comment, it->comment, 32);
				printf("             %d:%s;\n", it->id, it->comment);
			    iSeq++ ;
			}
			printf("temp: %s  (%d) .\n", tempDebug, vecRet.size());
			
//		    printf("GET:: data: %d - %d \n\t", sizeof(RegChgList), sizeof(ChgFrameSimple));
//			for (int i = 0 ; i < sizeof(RegChgList) + regChgList->count * sizeof(ChgFrameSimple) ; i++)
//			{
//			    printf("%02X ", strChgRegLst[i]);
//				if(i+1 == sizeof(RegChgList))
//			    	printf("\n-%02X\t", i+1);
//				else if((i+1 - sizeof(RegChgList))%sizeof(ChgFrameSimple) == 0)
//					if(i+1 - sizeof(RegChgList) > 0)
//			    	     printf("\n_%02X\t", i+1);
//			}
//		    printf("\n");
			
            writeShm(SHM_CHG_REG_LIST_INFO, 0, (void*)regChgList, 
				sizeof(RegChgList) + MAX_R_REG_ID * sizeof(ChgFrameSimple));
	        setIntprtDataFlag(true);
            break;
        case READ_CHG_MR_LST:
			vecRet.clear(); 
			vecRet = forgesight_read_valid_mr_lst(0, 255);
			memset(tempDebug, 0x00, 1024);
			strcpy(tempDebug, "MR:");
			regChgList = (RegChgList *)malloc(sizeof(RegChgList) + MAX_MR_REG_ID * sizeof(ChgFrameSimple));
			chgFrameSimple = (ChgFrameSimple *)((char *)regChgList + sizeof(RegChgList)) ;
			strChgRegLst = (char *)regChgList ;
			memset(strChgRegLst, 0x00, sizeof(RegChgList) + MAX_MR_REG_ID * sizeof(ChgFrameSimple));
			regChgList->command = intprt_ctrl.cmd ;
			regChgList->count   = vecRet.size();
			iSeq = 0 ;
			for(vector<BaseRegData>::iterator it = vecRet.begin(); it != vecRet.end(); ++it)
			{
				sprintf(tempDebug, "%s%d:%s;", tempDebug, it->id, it->comment);
				chgFrameSimple[iSeq].id = it->id ;
				memcpy(chgFrameSimple[iSeq].comment, it->comment, 32);
				printf("             %d:%s;\n", it->id, it->comment);
			    iSeq++ ;
			}
			printf("temp: %s  (%d) .\n", tempDebug, vecRet.size());
				
            writeShm(SHM_CHG_REG_LIST_INFO, 0, (void*)regChgList, 
				sizeof(RegChgList) + MAX_MR_REG_ID * sizeof(ChgFrameSimple));
	        setIntprtDataFlag(true);
            break;
#endif
        default:
            break;

    }
	lastCmd = intprt_ctrl.cmd;
  	//		printf("left parseCtrlComand.\n");
}

void initShm()
{
    openShm(SHM_INTPRT_CMD, 1024);
    openShm(SHM_INTPRT_STATUS, 1024);
	
	// Lujiaming add at 0323
    openShm(SHM_REG_IO_INFO, 1024);
	// Lujiaming add at 0323 end
	
	// Lujiaming add at 0514
    openShm(SHM_CHG_REG_LIST_INFO, 1024);
	// Lujiaming add at 0514 end
	
    openShm(SHM_CTRL_CMD, 1024);
    openShm(SHM_CTRL_STATUS, 1024);
    openShm(SHM_INTPRT_DST, 1024);
    intprt_ctrl.cmd = START;
	g_privateInterpreterState = IDLE_R ;
	
	setPrgmState(IDLE_R);
#ifdef WIN32
//	generateFakeData();
#else
	
#ifndef USE_FORSIGHT_REGISTERS_MANAGER
	initRegShmi();
#endif
//	initShmi(1024);
#endif
}


void updateIOError()
{
#ifndef WIN32
	U64 result = SUCCESS ;
	result = IOInterface::instance()->updateIOError();
    if (result != SUCCESS)
    {
		setWarning(result) ; 
    }
#endif
}

