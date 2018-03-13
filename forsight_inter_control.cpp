#include "forsight_inter_control.h"
#include "forsight_innercmd.h"

#define VELOCITY    (500)
using namespace std;

static std::vector<Instruction> g_script;
static int block_start, block_end;
bool terminated = false;

bool is_backward= false;
InterpreterState prgm_state = IDLE_R;
//bool send_flag = false;
int target_line;
static IntprtStatus intprt_status;
static Instruction instruction;
static CtrlStatus ctrl_status;
static InterpreterControl intprt_ctrl;

static InterpreterState g_privateInterpreterState;

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
            instr.type = LOOP;
            if (result.size() == 1)
                instr.loop_cnt = -1;
            else 
                instr.loop_cnt = atoi(result[1].c_str());
            printf("loop_cnt:%d\n", instr.loop_cnt);
        }
        else if (result[0] == "END")
        {
            instr.type = END;
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
        if (g_script[i].type == END)
        {
            block_end = i -1;
            return;
        }
    }
    block_end = g_script.size() - 1;
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
    writeShm(SHM_INTPRT_STATUS, offset, (void*)&state, sizeof(state));
}

void setCurLine(int line)
{
#ifdef WIN32
	Instruction temp,  * tempPtr = &temp;
    int offset = (int)&(tempPtr->line) - (int)tempPtr ;
#else
   // int offset = (int)&intprt_status.line - (int)&intprt_status;
    int offset = &((Instruction*)0)->line;   
#endif  
    printf("setCurLine (%d) at %d\n", line, offset);
    writeShm(SHM_INTPRT_STATUS, offset, (void*)&line, sizeof(line));    
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
    if (objThdCtrlBlockPtr->is_abort)
    {
        target_line++;
        return false;
    }
    getSendPermission();
    if (!ctrl_status.is_permitted)
    {
        printf("not permitted\n");
        return false;
    }
    int count = 0;
    bool ret;
    //printf("cur state:%d\n", prgm_state);
    if ((objThdCtrlBlockPtr->prog_mode == 1) && (prgm_state == EXECUTE_TO_PAUSE_T))
    {
        if (isInstructionEmpty(SHM_INTPRT_CMD))
        {
            //printf("check if step is done\n");
            setPrgmState(PAUSED_R);
        }
        return false;
    }

    // if (prgm_state != EXECUTE_R)
    //    return false;
    do
    {
		if (instruction->is_additional == false)
		{
			ret = tryWrite(SHM_INTPRT_CMD, 0, 
				(void*)instruction, sizeof(Instruction));
		}
		else
		{
			ret = tryWrite(SHM_INTPRT_CMD, 0, 
				(void*)instruction, 
				sizeof(Instruction) 
					+ sizeof(AdditionalInfomation) * instruction->add_num);
		}

	    if (ret)
	    {
	        if (is_backward)
	        {
	            is_backward = false;
	            target_line--;
	            setCurLine(target_line);
	        }
	        else
	        {
	            target_line++;
	            setCurLine(target_line);
	        }   

	        if (objThdCtrlBlockPtr->prog_mode == 1)
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

    return true;
}

bool getIntprtCtrl()
{
    return tryRead(SHM_CTRL_CMD, 0, (void*)&intprt_ctrl, sizeof(intprt_ctrl));
}

void startFile(struct thread_control_block * objThdCtrlBlockPtr, char * proj_name)
{
	strcpy(objThdCtrlBlockPtr->project_name, proj_name); // "prog_demo_dec"); // "BAS-EX1.BAS") ; // 
	objThdCtrlBlockPtr->is_main_thread = 1 ;
	objThdCtrlBlockPtr->iThreadIdx = 0 ;
	objThdCtrlBlockPtr->instrSet   
		= (Instruction * )malloc(sizeof(Instruction) + 
		  sizeof(AdditionalInfomation) * ADD_INFO_NUM);
	base_thread_create(0, objThdCtrlBlockPtr);
	intprt_ctrl.cmd = LOAD ;
}


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

void parseCtrlComand(struct thread_control_block * objThdCtrlBlockPtr)
{
#ifndef WIN32
    printf("parseCtrlComand: %d\n", intprt_ctrl.cmd);
#endif
    switch (intprt_ctrl.cmd)
    {
        case LOAD:
            // printf("load file_name\n");
            break;
        case JUMP:
#ifdef USE_XPATH
            printf("jump to line:%s\n", intprt_ctrl.line);
			target_line = getLineNumFromXPathVector(intprt_ctrl.line);
#else
            printf("jump to line:%d\n", intprt_ctrl.line);
			target_line = intprt_ctrl.line;
            setLinenum(objThdCtrlBlockPtr, target_line);
#endif
            // setPrgmState(EXECUTE_R);
			fflush(stdout);
			break;
        case DEBUG:
            printf("debug...\n");
            objThdCtrlBlockPtr->prog_mode = 1 ;
            setPrgmState(PAUSED_R);
			if(strlen(intprt_ctrl.start_ctrl.file_name) == 0)
			{
			   strcpy(intprt_ctrl.start_ctrl.file_name, "prog_demo_dec");
			}
            startFile(objThdCtrlBlockPtr, intprt_ctrl.start_ctrl.file_name);
            break;
        case START:
            printf("start run...\n");
            setPrgmState(EXECUTE_R);
			if(strlen(intprt_ctrl.start_ctrl.file_name) == 0)
			{
			   strcpy(intprt_ctrl.start_ctrl.file_name, "prog_demo_dec");
			}
			startFile(objThdCtrlBlockPtr, intprt_ctrl.start_ctrl.file_name);
            break;
        case FORWARD:
            printf("step forward\n");
            objThdCtrlBlockPtr->prog_mode = 1 ;
            // target_line++;
            target_line = getLinenum(objThdCtrlBlockPtr);
            printf("step forward to %d \n", target_line);
            setPrgmState(EXECUTE_R);
			
            printf("Enter waitInterpreterStateToPaused %d \n", target_line);
			waitInterpreterStateToPaused(objThdCtrlBlockPtr);
			// target_line++ in setInstruction
            printf("Left  waitInterpreterStateToPaused %d \n", target_line);
			
//           printf("Enter waitInterpreterStateToWaiting %d \n", target_line);
//			waitInterpreterStateToWaiting(objThdCtrlBlockPtr);
//			// target_line++ in setInstruction
//           printf("Left  waitInterpreterStateToWaiting %d \n", target_line);
			

            setLinenum(objThdCtrlBlockPtr, target_line);
            break;
        case BACKWARD:
            printf("backward\n");
           // setPrgmState(EXECUTE_R);  
            target_line = getLinenum(objThdCtrlBlockPtr);          
            if (target_line < 1)
                break;
            if (objThdCtrlBlockPtr->prog_jmp_line[target_line-1].type == MOTION)
                is_backward = true;
            else
            {
                perror("can't back\n");
                break;
            }
            objThdCtrlBlockPtr->prog_mode = 1 ;
	    	// target_line--;
            target_line = getLinenum(objThdCtrlBlockPtr);
            setPrgmState(EXECUTE_R);

            printf("Enter waitInterpreterStateToPaused %d \n", target_line);
			waitInterpreterStateToPaused(objThdCtrlBlockPtr);
			// target_line-- in setInstruction
            printf("Left  waitInterpreterStateToPaused %d \n", target_line);
			
//            printf("Enter waitInterpreterStateToWaiting %d \n", target_line);
//			waitInterpreterStateToWaiting(objThdCtrlBlockPtr);
//			// target_line-- in setInstruction
//            printf("Left  waitInterpreterStateToWaiting %d \n", target_line);
			
		    setLinenum(objThdCtrlBlockPtr, target_line);
		    break;
		case CONTINUE:
			if(getPrgmState() == PAUSED_R)
	        {
	            printf("continue move..\n");
	            objThdCtrlBlockPtr->prog_mode = 0 ;
	            setPrgmState(EXECUTE_R);
			}
			else
			    setWarning(1);

            break;
        case PAUSE:
        {
            UserOpMode mode = getUserOpMode();
            if ((mode == SLOWLY_MANUAL_MODE_U)
            || (mode == UNLIMITED_MANUAL_MODE_U))
            {
                objThdCtrlBlockPtr->prog_mode = 1 ;
            }
            setPrgmState(PAUSED_R); 
            break;
        }
        case ABORT:
            printf("abort motion\n");
	        objThdCtrlBlockPtr->is_abort = true;
            // target_line = getMaxLineNum();
            target_line = 0;
			
		    setPrgmState(PAUSE_TO_IDLE_T);
#ifdef WIN32
			Sleep(1);
#else
	        usleep(1000);
#endif
  			printf("setPrgmState(IDLE_R).\n");
		    setPrgmState(IDLE_R);
            break;
        case MOD_REG:
            break;
        default:
            break;

    }
}

void initShm()
{
    openShm(SHM_INTPRT_CMD, 1024);
    openShm(SHM_INTPRT_STATUS, 1024);
    openShm(SHM_CTRL_CMD, 1024);
    openShm(SHM_CTRL_STATUS, 1024);
    intprt_ctrl.cmd = START;
	g_privateInterpreterState = IDLE_R ;
}

