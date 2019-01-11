#ifndef FORSIGHT_BASINT_H
#define FORSIGHT_BASINT_H

#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h"
#include "common.h"

// #ifndef WIN32
#define USE_FORSIGHT_REGISTERS_MANAGER
// #endif

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include <vector>
#include <map>
#include <stack>

#include "interpreter_common.h"
#include "forsight_eval_type.h"

#ifdef WIN32
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
// #include "motion_plan_arm_group.h"
#endif


using namespace std;

#ifdef WIN32
#define DATA_PATH        "\\root\\files_manager_python27\\data"
#else
#define DATA_PATH        "\/root\/files_manager_python27\/data"
#endif

#define NUM_SUBROUTINE 64
#define NUM_THREAD     32

#define PROGRAM_START_LINE_NUM     2

#define LAB_LEN 128
#define SELECT_AND_CYCLE_NEST 64
#define SUB_NEST 16
#define PROG_SIZE 40960   // 4096
#define  NUM_OF_PARAMS  31    // max number of parameters

#define DELIMITER    1
#define VARIABLE     2
#define NUMBER       3
#define COMMAND      4
#define STRING	     5
#define QUOTE	     6
#define INNERCMD     7
#define INSIDEFUNC   8
#define BUILTINFUNC  9
#define OUTSIDEFUNC  10
#define COMMENT      11

#define MONITOR_THREAD     0
#define MAIN_THREAD        1

enum double_ops {
	LT=1,    // value < partial_value
	LE,      // value <= partial_value
	GT,      // value > partial_value
	GE,      // value >= partial_value
	EQ,      // value == partial_value
	NE,      // value <> partial_value
	AND,     // value AND partial_value
	OR ,     // value OR  partial_value
};

typedef enum _LineNumState
{
    LINENUM_NONE,
    LINENUM_PRODUCED, 
    LINENUM_CONSUMED,
}LineNumState;

typedef enum _SubLabelType
{
    SUBLABEL_NONE,
    INSIDE_FUNC, 
    OUTSIDE_FUNC,
}SubLabelType;

typedef enum _ExecuteDirection
{
    EXECUTE_FORWARD = 0,
    EXECUTE_BACKWARD
}ExecuteDirection;

typedef enum _ProgMode
{
    FULL_MODE = 0,
    STEP_MODE,
    ERROR_MODE,
}ProgMode;

struct sub_label {
	SubLabelType  type ;
	char name[LAB_LEN];
	char *p;  /* points to place to go in source file*/
};

struct var_type {
    char var_name[80]; // name
    // var_inner_type v_type; // data type
	eval_value value ;
};

// This structure encapsulates the info
// associated with variables.
typedef struct prog_line_info_t {
    char*     start_prog_pos; // name
    char*     end_prog_pos; // name
	InstType  type;
} prog_line_info ;

typedef struct select_and_cycle_stack {
	int itokentype ;
	// For and Select use now
	eval_value target ;
	// For
	char var[80];     /* counter or selector variable  */
	char *loc;
	// While uses it
	char *while_loc;
	// While temply and discard now
	// int compare_op ;
} select_and_cycle_stack_struct ;

struct thread_control_block {
	int iThreadIdx ;              // Thread Idx
	char project_name[128];       // project_name
	// This vector holds info for global variables.
	vector<var_type> global_vars;
	
	// This vector holds info for local variables
	// and parameters.
	vector<var_type> local_var_stack;
	
	// Stack for managing function scope.
	stack<int> func_call_stack;
	
	int g_variable_error ; // = 0 ;
	char *p_buf;		// program buffer
	char *prog;  		/* prog pointer holds expression to be analyzed */
	char *prog_end;     /* prog end position */

	int    iSubProgNum ;                    // sub programs number
	char * sub_prog[NUM_SUBROUTINE];		// sub programs buffer
	
	vector<prog_line_info> prog_jmp_line;   // jmp_line info
	ProgMode  prog_mode ; 					// = 0;   /* 0 - run to end, 1 - step  */
	ExecuteDirection  execute_direction ;  	/* 0 - FORWARD, 1 - BACKWARD  */
	bool is_abort , is_paused, is_in_macro; 
	int  is_main_thread ; 					// = 0;   /* 0 - Monitor, 1 - Main  */
	
	char token[80];					// Current token
	char token_type, tok;			// 
	
	vector<sub_label> sub_label_table; // [NUM_LAB];
	float ret_value ;               // retrun values of function
	
	/* stack for FOR/NEXT loop */
	struct select_and_cycle_stack  selcyclstack[SELECT_AND_CYCLE_NEST];
	
	char *gosub_stack[SUB_NEST];			/* stack for gosub */
	
	int select_and_cycle_tos;  				/* index to top of FOR/WHILE/IF/SELECT stack */
	int gosub_tos;  						/* index to top of GOSUB stack */

	Instruction * instrSet ;               // used by MOV*
	// LineNum and Update flag
	int               iLineNum ;           // Current LineNum
//	LineNumState      stateLineNum ;
//    MotionTarget      currentMotionTarget ;
    map<int, MoveCommandDestination>  start_mov_position ;  // iLineNum :: movCmdDst
    
	vector<string> vector_XPath ;
} ;
#ifndef WIN32
extern fst_log::Logger* log_ptr_;
#endif
void setLinenum(struct thread_control_block* objThreadCntrolBlock, int iLinenum);
// LineNumState getLinenum(struct thread_control_block* objThreadCntrolBlock, int & num);
int getLinenum(struct thread_control_block* objThreadCntrolBlock) ;

int call_interpreter(struct thread_control_block* objThreadCntrolBlock, int mode);
#ifdef WIN32
unsigned __stdcall basic_interpreter(void* arg);
#else
void* basic_interpreter(void* arg);
#endif

bool basic_thread_create(int iIdx, void * args);
void basic_thread_destroy(int iIdx);

int get_token(struct thread_control_block * objThreadCntrolBlock);
void get_exp(struct thread_control_block * objThreadCntrolBlock, eval_value * result, int* boolValue);
void putback(struct thread_control_block * objThreadCntrolBlock);
int calc_conditions(
		struct thread_control_block * objThreadCntrolBlock);
int exec_call(struct thread_control_block * objThreadCntrolBlock, bool isMacro = false);

void assign_var(struct thread_control_block * objThreadCntrolBlock, char *vname, eval_value value);
eval_value find_var(struct thread_control_block * objThreadCntrolBlock, char *s, int raise_unkown_error = 0);

void find_eol(struct thread_control_block * objThreadCntrolBlock);
int  jump_prog_from_line(struct thread_control_block * objThreadCntrolBlock, int iNum);
int  calc_line_from_prog(struct thread_control_block * objThreadCntrolBlock);
void serror(struct thread_control_block * objThreadCntrolBlock, int error);

void assignment(struct thread_control_block * objThreadCntrolBlock) ;

#endif

