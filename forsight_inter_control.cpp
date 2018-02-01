#include "forsight_inter_control.h"

#define VELOCITY    (500)
using namespace std;

static std::vector<Instruction> g_script;
static int block_start, block_end;
bool terminated = false;


int target_line;
static IntprtStatus intprt_status;
static Instruction instruction;
static CtrlStatus ctrl_status;
static InterpreterControl intprt_ctrl;

static ProgramState g_privateProgramState;

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

bool parseScript(const char* file_name)
{
    Instruction instr;
    char command[256];
    string path = "/home/fst/Program/";
    path = path + file_name;
    ifstream fin(path.c_str());
    int count = 0;
    while(fin.getline(command, sizeof(command)))
    {
        printf("command:%s\n",command);
        // instr.line = ++count;
	    // itoa(++count, instr.line, 10);
        sprintf(instr.line, "%d", ++count);
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


ProgramState getPrgmState()
{
	return g_privateProgramState ;
}

void setPrgmState(ProgramState state)
{
	g_privateProgramState = state ;
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


bool setInstruction(Instruction * instruction)
{
    getSendPermission();
    if (!ctrl_status.is_permitted)
        return false;
    int count = 0;
    bool ret;
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
				(void*)instruction, sizeof(Instruction) + sizeof(AdditionalInfomation));
		}
#ifdef WIN32
		Sleep(1);
#else
        usleep(1000);
#endif
     //   if (count++ > 500)
     //       return false;
    }while(!ret);

    return true;
}

void initShm()
{
    openShm(SHM_INTPRT_CMD, 1024);
    openShm(SHM_INTPRT_STATUS, 1024);
    openShm(SHM_CTRL_CMD, 1024);
    openShm(SHM_CTRL_STATUS, 1024);
    intprt_ctrl.cmd = START;
	g_privateProgramState = IDLE_R ;
}

bool getIntprtCtrl()
{
    return tryRead(SHM_CTRL_CMD, 0, (void*)&intprt_ctrl, sizeof(intprt_ctrl));
}

void parseCtrlComand(struct thread_control_block * objThdCtrlBlockPtr)
{
    switch (intprt_ctrl.cmd)
    {
	case LOAD:
		break;
	case JUMP:
		target_line = intprt_ctrl.id;
		break;
	case START:
		{
			strcpy(objThdCtrlBlockPtr->file_name, "prog_demo_dec.bas"); // "BAS-EX1.BAS") ; // 
			objThdCtrlBlockPtr->is_main_thread = 1 ;
			objThdCtrlBlockPtr->iThreadIdx = 0 ;
			objThdCtrlBlockPtr->instrSet   
				= (Instruction * )malloc(sizeof(Instruction) + 
				  sizeof(AdditionalInfomation) * ADD_INFO_NUM);
			base_thread_create(0, objThdCtrlBlockPtr);
			intprt_ctrl.cmd = LOAD ;
            break;
        }
	case FORWARD:
		break;
	case BACKWARD:
		if (g_script[target_line-1].type == MOTION)
			target_line--;
		else
			perror("can't back\n");
		break;
	case CONTINUE:
		if(getPrgmState() == PAUSED_R)
		   setPrgmState(EXECUTE_R);
		else
		   setWarning(1);
		break;
	case MOD_REG:
		break;
	default:
		break;
    }
}
