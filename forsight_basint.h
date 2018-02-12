#ifndef FORSIGHT_BASINT_H
#define FORSIGHT_BASINT_H

#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h"
#include <vector>
#include <stack>

#ifdef WIN32
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include "interpreter_common.h"

using namespace std;

#define NUM_SUBROUTINE 128
#define NUM_THREAD     256

#define LAB_LEN 128
#define SELECT_AND_CYCLE_NEST 128
#define SUB_NEST 25
#define PROG_SIZE 10000
const int NUM_OF_PARAMS = 31;    // max number of parameters

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

struct sub_label {
	SubLabelType  type ;
	char name[LAB_LEN];
	char *p;  /* points to place to go in source file*/
};

struct var_type {
    char var_name[80]; // name
    // var_inner_type v_type; // data type
	float value ;
};

// This structure encapsulates the info
// associated with variables.
typedef struct prog_line_info_t {
    char*     prog_pos; // name
	InstType  type; ;
} prog_line_info ;

typedef struct select_and_cycle_stack {
	int itokentype ;
	// For and Select use now
	float target ;
	// For
	char var[80];     /* counter or selector variable  */
	char *loc;
	// While uses it
	char *while_loc;
	// While temply and discard now
	// int compare_op ;
} select_and_cycle_stack_struct ;

struct thread_control_block {
	int iThreadIdx ;
	char project_name[128];
	// This vector holds info for global variables.
	vector<var_type> global_vars;
	
	// This vector holds info for local variables
	// and parameters.
	vector<var_type> local_var_stack;
	
	// Stack for managing function scope.
	stack<int> func_call_stack;
	
	int g_variable_error ; // = 0 ;
	char *p_buf;
	char *prog;  /* holds expression to be analyzed */
	char *prog_end;

	int    iSubProgNum ;
	char * sub_prog[NUM_SUBROUTINE];
	
	prog_line_info prog_jmp_line[1024];
	int  prog_mode ; // = 0;   /* 0 - run to end, 1 - step  */
	int  is_main_thread ; // = 0;   /* 0 - run to end, 1 - step  */
	
	char token[80];
	char token_type, tok;
	
	vector<sub_label> sub_label_table; // [NUM_LAB];
	float ret_value ;
	
	/* stack for FOR/NEXT loop */
	struct select_and_cycle_stack  selcyclstack[SELECT_AND_CYCLE_NEST];
	
	char *gosub_stack[SUB_NEST];	/* stack for gosub */
	
	int select_and_cycle_tos;  /* index to top of FOR/WHILE/IF/SELECT stack */
	int gosub_tos;  /* index to top of GOSUB stack */

	Instruction * instrSet ;
	// LineNum and Update flag
	int               iLineNum ;
	LineNumState      stateLineNum ;
} ;

void setLinenum(struct thread_control_block* objThreadCntrolBlock, int iLinenum);
LineNumState getLinenum(struct thread_control_block* objThreadCntrolBlock, int & num);

int call_interpreter(struct thread_control_block* objThreadCntrolBlock, int mode);
#ifdef WIN32
unsigned __stdcall basic_interpreter(void* arg);
#else
void* basic_interpreter(void* arg);
#endif

bool base_thread_create(int iIdx, void * args);
void base_thread_wait(int iIdx);

int get_token(struct thread_control_block * objThreadCntrolBlock);
void get_exp(struct thread_control_block * objThreadCntrolBlock, float * result, int* boolValue);
void putback(struct thread_control_block * objThreadCntrolBlock);
int calc_conditions(
		struct thread_control_block * objThreadCntrolBlock);
int exec_call(struct thread_control_block * objThreadCntrolBlock);

void assign_var(struct thread_control_block * objThreadCntrolBlock, char *vname, float value);
float find_var(struct thread_control_block * objThreadCntrolBlock, char *s);

void find_eol(struct thread_control_block * objThreadCntrolBlock);
#endif

