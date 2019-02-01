// Forsight_BASINT.cpp : Defines the entry point for the console application.
//
/* A tiny BASIC interpreter */
#include "forsight_basint.h"
#include "forsight_innercmd.h"
#include "forsight_innerfunc.h"
#include "forsight_inter_control.h"
#include "forsight_xml_reader.h"
#include "forsight_io_controller.h"
#include "forsight_program_property.h"

#ifndef WIN32
#include "error_code.h"
#else
#include "macro_instr_mgr.h"
#endif

#ifdef USE_FORSIGHT_REGISTERS_MANAGER
#include "reg_manager/forsight_registers_manager.h"
#else
#include "reg-shmi/forsight_registers.h"
#endif

#ifndef WIN32
#define TPI_SUCCESS				(0)
#endif

// #define NUM_LAB 100
#define LINE_CONTENT_LEN   2046

#define ILLTOK     0
#define PRINT      1
#define INPUT      2
#define IF         3
#define THEN       4
#define ELSE       5
#define FOR        6
#define NEXT       7
#define TO         8
#define GOTO       9
#define EOL       10
#define FINISHED  11
#define GOSUB     12
#define RETURN    13
#define BREAK     14
#define CONTINUE  15
#define SELECT    16
#define CASE      17
#define WHILE     18
#define WEND      19
#define ELSEIF    20
#define ENDIF     21
#define LOOP      22
#define ENDLOOP   23
#define SUB       24
#define CALL      25
#define END       26
#define IMPORT    27
#define DEFAULT   28
#define WAIT      29
#define CALLMACRO 30
#define NOP       31

enum var_inner_type { FORSIGHT_CHAR, FORSIGHT_INT, FORSIGHT_FLOAT };

#define FORSIGHT_RETURN_VALUE   "forsight_return_value"
#define FORSIGHT_CURRENT_JOINT  "j_pos"
#define FORSIGHT_CURRENT_POS    "c_pos"
	
#define FORSIGHT_REGISTER_ON    "on"
#define FORSIGHT_REGISTER_OFF   "off"

#define FORSIGHT_TIMER          "timer"
#define FORSIGHT_TIMER_START    "start"
#define FORSIGHT_TIMER_STOP     "stop"
#define FORSIGHT_TIMER_RESET    "reset"

#define FORSIGHT_PULSE          "pulse"

// #define FORSIGHT_REGISTER_UF    "uf"
// #define FORSIGHT_REGISTER_TF    "tf"

#define FORSIGHT_TF_NO    "tf_no."
#define FORSIGHT_UF_NO    "uf_no."
#define FORSIGHT_OVC      "ovc."
#define FORSIGHT_OAC      "oac."

#define STR_AND   "and"
#define STR_OR    "or"

// Enumeration of two-character operators, such as <=.

//	int variables[26]= {    /* 26 user variables,  A-Z */
//	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	  0, 0, 0, 0, 0, 0
//	};

struct commands { /* keyword lookup table */
  char command[20];
  char tok;
};

static struct commands table[] = { /* Commands must be entered lowercase */
  "print",       PRINT, /* in this table. */
  "input",       INPUT,
  "if",          IF,
  "then",        THEN,
  "else",        ELSE,
  "goto",        GOTO,
  "for",         FOR,
  "next",        NEXT,
  "to",          TO,
  "gosub",       GOSUB,
  "return",      RETURN,
  "break",       BREAK,
  "continue",    CONTINUE,
  "select",      SELECT,
  "case",        CASE,
  "default",     DEFAULT,
  "while",       WHILE,
  "wend",        WEND,
  "elseif",      ELSEIF,
  "endif",       ENDIF,
  "loop",        LOOP,
  "endloop",     ENDLOOP,
  "sub",         SUB,
  "call",        CALL,
  "end",         END,
  "import",      IMPORT,
  "callmacro",   CALLMACRO,
  "nop",         NOP,
  "", END  /* mark end of table */
};

typedef struct _ErrInfo {
#ifdef WIN32
	__int64         warn;
#else
    long long int   warn;
#endif
    char desc[1024];
} ErrInfo;


char * gosub_pop(struct thread_control_block * objThreadCntrolBlock);

struct thread_control_block g_thread_control_block[NUM_THREAD + 1];
// jmp_buf e_buf; /* hold environment for longjmp() */

struct select_and_cycle_stack select_and_cycle_pop(
	struct thread_control_block * objThreadCntrolBlock);

void print(struct thread_control_block * objThreadCntrolBlock),
	 find_eol(struct thread_control_block * objThreadCntrolBlock),
	 exec_goto(struct thread_control_block * objThreadCntrolBlock);
void exec_if(struct thread_control_block * objThreadCntrolBlock),
	 exec_else(struct thread_control_block * objThreadCntrolBlock),
	 exec_for(struct thread_control_block * objThreadCntrolBlock),
	 exec_next(struct thread_control_block * objThreadCntrolBlock),
	 exec_while(struct thread_control_block * objThreadCntrolBlock),
	 exec_wend(struct thread_control_block * objThreadCntrolBlock),
	 select_and_cycle_push(struct thread_control_block * objThreadCntrolBlock,
	 							struct select_and_cycle_stack obj_stack),
	 input(struct thread_control_block * objThreadCntrolBlock),
	 exec_break(struct thread_control_block * objThreadCntrolBlock),
	 exec_continue(struct thread_control_block * objThreadCntrolBlock),
	 exec_loop(struct thread_control_block * objThreadCntrolBlock),
	 exec_endloop(struct thread_control_block * objThreadCntrolBlock),
	 exec_select(struct thread_control_block * objThreadCntrolBlock),
	 exec_case(struct thread_control_block * objThreadCntrolBlock),
	 exec_elseif(struct thread_control_block * objThreadCntrolBlock),
	 exec_endif(struct thread_control_block * objThreadCntrolBlock);

int  exec_end(struct thread_control_block * objThreadCntrolBlock);
int  gosub(struct thread_control_block * objThreadCntrolBlock);
void greturn(struct thread_control_block * objThreadCntrolBlock),
	 gosub_push(struct thread_control_block * objThreadCntrolBlock, char *s),
	 label_init(struct thread_control_block * objThreadCntrolBlock),
//	 exec_call(struct thread_control_block * objThreadCntrolBlock), 
	 exec_import(struct thread_control_block * objThreadCntrolBlock);
void get_exp(struct thread_control_block * objThreadCntrolBlock, eval_value * result, int* boolValue),
	 putback(struct thread_control_block * objThreadCntrolBlock);
void    level1(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        level2(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        level3(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        level4(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        level5(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        level6(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        level7(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue),
        primitive(struct thread_control_block * objThreadCntrolBlock, eval_value *result);
void unary(char, eval_value *r),
	arith(struct thread_control_block * objThreadCntrolBlock, char o, eval_value *r, eval_value *h);

int load_program(struct thread_control_block * objThreadCntrolBlock, char *p, char *pname);
void scan_labels(struct thread_control_block * objThreadCntrolBlock, 
				SubLabelType type, char * pname);
int add_label(struct thread_control_block * objThreadCntrolBlock, struct sub_label objLabel);
char *find_label(struct thread_control_block * objThreadCntrolBlock, char *s);

int look_up(char *s);
int isdelim(char c), isdelim_with_array(char c);
int iswhite(char c);

// int basic_interpreter(struct thread_control_block * objThreadCntrolBlock);

void get_args(struct thread_control_block * objThreadCntrolBlock);
void get_params(struct thread_control_block * objThreadCntrolBlock);
int  calc_line_from_prog(struct thread_control_block * objThreadCntrolBlock);

#ifdef WIN32
HANDLE    g_basic_interpreter_handle[NUM_THREAD + 1];
extern MacroInstrMgr  *  g_macro_instr_mgr_ptr; 
#else
pthread_t g_basic_interpreter_handle[NUM_THREAD + 1];
fst_log::Logger * log_ptr_ = NULL;
#endif

/************************************************* 
	Function:       basic_interpreter
	Description:    interpreter thread wrapper.
	Input:          struct thread_control_block pointer
	Output:         NULL
	Return:         NULL
*************************************************/ 
#ifdef WIN32
unsigned __stdcall basic_interpreter(void* arg)
#else
void* basic_interpreter(void* arg)
#endif
{
	int iIdx = 0;
	//  int iRet = 0;
	struct thread_control_block * objThreadCntrolBlock
					= (struct thread_control_block*)arg;
	// Set this state outside according prog_mode
	// setPrgmState(objThreadCntrolBlock, EXECUTE_R); 
	FST_INFO("Enter call_interpreter.");
	//  if(objThreadCntrolBlock->is_in_macro)
	//  {
	//  	setRunningMacroInstr(objThreadCntrolBlock->project_name);
	//  }
	// iRet = 
	call_interpreter(objThreadCntrolBlock, 1);
	FST_INFO("Left  call_interpreter.");
	//  if(objThreadCntrolBlock->is_in_macro)
	//  {
	//  	resetRunningMacroInstr(objThreadCntrolBlock->project_name);
	//  }
	setPrgmState(objThreadCntrolBlock, INTERPRETER_IDLE);
	// clear ProgramName and leave line path
  	setProgramName(objThreadCntrolBlock, (char *)""); 

	// free(objThreadCntrolBlock->instrSet);
	objThreadCntrolBlock->instrSet = 0 ;

	// iIdx = g_thread_control_block[0].iThreadIdx ;
	iIdx = objThreadCntrolBlock->iThreadIdx ;

#ifdef WIN32
	CloseHandle( g_basic_interpreter_handle[iIdx] );  
	g_basic_interpreter_handle[iIdx] = NULL; 
	return NULL;
#else
	FST_INFO("Enter pthread_join.");
	pthread_join(g_basic_interpreter_handle[iIdx], NULL);
	FST_INFO("Left  pthread_join.");
	fflush(stdout);
	g_basic_interpreter_handle[iIdx] = 0;
	return NULL ;
#endif // WIN32
}

/*
void setLinenum(struct thread_control_block* objThreadCntrolBlock, int iLinenum)
{
#ifdef WIN32
	win_cs_lock tempBaseLock ;
#else
	linux_mutex_lock tempBaseLock ;
#endif 
	auto_lock temp(&tempBaseLock) ;
    FST_INFO("setLinenum : %d", iLinenum);
	if(objThreadCntrolBlock->stateLineNum == LINENUM_CONSUMED)
	{
		objThreadCntrolBlock->stateLineNum = LINENUM_PRODUCED;
		objThreadCntrolBlock->iLineNum = iLinenum;
	}
	else
	{
		FST_ERROR("setLinenum failure");
	}
}

LineNumState getLinenumInternal(
	struct thread_control_block* objThreadCntrolBlock, int & num)
{
#ifdef WIN32
	win_cs_lock tempBaseLock ;
#else
	linux_mutex_lock tempBaseLock ;
#endif 
	auto_lock temp(&tempBaseLock) ;
	if(objThreadCntrolBlock->stateLineNum == LINENUM_PRODUCED)
	{
		objThreadCntrolBlock->stateLineNum = LINENUM_CONSUMED;
		num = objThreadCntrolBlock->iLineNum ;
		return LINENUM_PRODUCED ;
	}
	else 
	{
		return objThreadCntrolBlock->stateLineNum ;
	}
}

int getLinenum(
	struct thread_control_block* objThreadCntrolBlock)
{
    int num = 0;
	LineNumState state = getLinenumInternal(objThreadCntrolBlock, num);
	while(state == LINENUM_CONSUMED)
	{
#ifdef WIN32
	  Sleep(100);
#else
	  sleep(1);
#endif
		state = getLinenumInternal(objThreadCntrolBlock, num);
	}
	return num ;  // objThreadCntrolBlock->iLineNum ;
}
*/
	
/************************************************* 
	Function:		setRunningMacroInstr
	Description:	set Macro Instruction startup .
	Input:			program_name
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
void setRunningMacroInstr(char* program_name)
{
#ifdef WIN32
  if (g_macro_instr_mgr_ptr)
  {
	  g_macro_instr_mgr_ptr->setRunningInMacroInstrList(program_name);
  }
#endif
}

/************************************************* 
	Function:		resetRunningMacroInstr
	Description:	reset Macro Instruction startup .
	Input:			program_name
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
void resetRunningMacroInstr(char* program_name)
{
#ifdef WIN32
  if (g_macro_instr_mgr_ptr)
  {
	  g_macro_instr_mgr_ptr->resetRunningInMacroInstrList(program_name);
  }
#endif
}
	
/************************************************* 
	Function:		setLinenum
	Description:	set Line number.
	Input:			thread_control_block  - interpreter info
	                iLinenum              - Line number
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
void setLinenum(struct thread_control_block* objThreadCntrolBlock, int iLinenum)
{
    FST_INFO("setLinenum : %d at the %dth thread", 
		iLinenum, objThreadCntrolBlock->iThreadIdx);
	objThreadCntrolBlock->iLineNum = iLinenum;
	if(iLinenum < (int)objThreadCntrolBlock->vector_XPath.size())
	{
		FST_INFO("setLinenum: setCurLine (%d) at %s", 
			iLinenum, objThreadCntrolBlock->vector_XPath[iLinenum].c_str());
		setCurLine(objThreadCntrolBlock, 
			(char *)objThreadCntrolBlock->vector_XPath[iLinenum].c_str(), iLinenum);
	}
	else
		FST_INFO("OutRange with %d", iLinenum);
}

/************************************************* 
	Function:		getLinenum
	Description:	get Line number.
	Input:			thread_control_block  - interpreter info
	Output: 		NULL
	Return: 		Line number
*************************************************/ 
int getLinenum(
	struct thread_control_block* objThreadCntrolBlock)
{
    FST_INFO("getLinenum : %d", objThreadCntrolBlock->iLineNum);
	return objThreadCntrolBlock->iLineNum ;
}

/************************************************* 
	Function:		printCurrentLine
	Description:	print Current program Line.
	Input:			thread_control_block  - interpreter info
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
void printCurrentLine(struct thread_control_block* objThreadCntrolBlock)
{
	char cLineContent[LINE_CONTENT_LEN];
	char * cLineContentPtr = 0 ;
	char * cLineContentProgPtr = 0 ;

	memset(cLineContent, 0x00, LINE_CONTENT_LEN);
	cLineContentPtr = cLineContent ;
	cLineContentProgPtr = objThreadCntrolBlock->prog ;
	while(*cLineContentProgPtr!='\n'
		&& *cLineContentProgPtr!='\r'
		&& *cLineContentProgPtr!='\0')
	{
		*cLineContentPtr++=*cLineContentProgPtr++;
	}
	FST_INFO("\t(%d): (%s)",  // -(%08X), objThreadCntrolBlock->prog, 
		objThreadCntrolBlock->iLineNum, cLineContent);
}

/************************************************* 
	Function:		printProgJmpLine
	Description:	print all of program Line.
	Input:			thread_control_block  - interpreter info
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
void printProgJmpLine(struct thread_control_block* objThreadCntrolBlock)
{
	int iLineNumTemp = 0 ; 
	char *proglabelsScan; 
	// Scan the labels in the import files
	proglabelsScan = objThreadCntrolBlock->prog ;
	iLineNumTemp   = objThreadCntrolBlock->iLineNum ;
	for(unsigned i=0; i < objThreadCntrolBlock->prog_jmp_line.size(); i++)
	{
	    objThreadCntrolBlock->iLineNum = i; 
		objThreadCntrolBlock->prog = objThreadCntrolBlock->prog_jmp_line[i].start_prog_pos;
		printCurrentLine(objThreadCntrolBlock);
	}
	objThreadCntrolBlock->iLineNum = iLineNumTemp;
	objThreadCntrolBlock->prog     = proglabelsScan;
}

/************************************************* 
	Function:		call_interpreter
	Description:	load program or execute interpreter process.
	Input:			thread_control_block  - interpreter info
	     			mode =   1   - load program
	     			     =   0   - execute interpreter process.
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
int call_interpreter(struct thread_control_block* objThreadCntrolBlock, int mode)
{
  int isExecuteEmptyLine ;
  int iRet = 0;
  int iLinenum;
  char * cLineContentPtr = 0 ;
  char * cLineContentProgPtr = 0 ;
  char cLineContent[LINE_CONTENT_LEN];
  // int iScan = 0 ;
  // char in[80];
  // char *p_buf;
  // char *t = 0;

  objThreadCntrolBlock->ret_value = 0.0 ;
//  objThreadCntrolBlock->stateLineNum = LINENUM_PRODUCED ;
  if(mode == 1)
  {
	  /* allocate memory for the program */
	  if(!(objThreadCntrolBlock->p_buf=(char *) malloc(PROG_SIZE))) {
		FST_ERROR("allocation failure");
		exit(1);
	  }
	  if(!(objThreadCntrolBlock->instrSet   
		= (Instruction * )malloc(sizeof(Instruction) + 
		  sizeof(AdditionalInfomation) * ADD_INFO_NUM))) {
		FST_ERROR("allocation failure");
		exit(1);
	  }
	
	  objThreadCntrolBlock->iSubProgNum = 0 ;
	  memset(objThreadCntrolBlock->sub_prog, 0x00, sizeof(char *) * NUM_SUBROUTINE);
	  
	  if(objThreadCntrolBlock->start_mov_position.size() > 0)
	  {
	      FST_INFO("start_mov_position = %d", objThreadCntrolBlock->start_mov_position.size());
		  // objThdCtrlBlockPtr->start_mov_position.clear();
	  }
      label_init(objThreadCntrolBlock);  /* zero all labels */
	  /* load the program to execute */
	  if(!load_program(objThreadCntrolBlock, objThreadCntrolBlock->p_buf,objThreadCntrolBlock->project_name)) 
	  {
		FST_ERROR("no project : %s", objThreadCntrolBlock->project_name);
	  	// exit(1);
		return -1;
	  }
	  // Call in the first time to load major P[*]
  	  append_program_prop_mapper(objThreadCntrolBlock, objThreadCntrolBlock->project_name, true);
	  
	  objThreadCntrolBlock->prog = objThreadCntrolBlock->p_buf;
	  objThreadCntrolBlock->prog_end = objThreadCntrolBlock->prog + strlen(objThreadCntrolBlock->prog);
	  objThreadCntrolBlock->prog_jmp_line.clear();
	  scan_labels(objThreadCntrolBlock, INSIDE_FUNC, objThreadCntrolBlock->project_name); /* find the labels in the program */
	  objThreadCntrolBlock->select_and_cycle_tos = 0; /* initialize the FOR stack index */
	  objThreadCntrolBlock->gosub_tos = 0; /* initialize the GOSUB stack index */
	  objThreadCntrolBlock->is_abort  = false;
	  objThreadCntrolBlock->is_paused = false;
	  
	  get_token(objThreadCntrolBlock);
	  while(objThreadCntrolBlock->tok == IMPORT)
	  {
		  exec_import(objThreadCntrolBlock);
		  get_token(objThreadCntrolBlock);
	  }
      generateXPathVector(objThreadCntrolBlock, objThreadCntrolBlock->project_name);
	  struct prog_line_info_t objProgLineInfo ;
	  objProgLineInfo.start_prog_pos = objThreadCntrolBlock->prog_end ;
	  objProgLineInfo.end_prog_pos   = objThreadCntrolBlock->prog_end ;
	  objProgLineInfo.type     = END_PROG ;
	  objThreadCntrolBlock->prog_jmp_line.push_back(objProgLineInfo);

      memset(cLineContent, 0x00, LINE_CONTENT_LEN);
	  sprintf(cLineContent, "%s::main", objThreadCntrolBlock->project_name);
      char * loc = find_label(objThreadCntrolBlock, cLineContent); // "main");
	  if(loc=='\0')
	  {
		  serror(objThreadCntrolBlock, 7); /* label not defined */
		  FST_ERROR("label not defined.");
		  return 1;
	  }
	  // Save local var stack index.
	  // int lvartemp = objThreadCntrolBlock->local_var_stack.size();
	  gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
	  objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
	  
	  while(*(objThreadCntrolBlock->token) != ')')
	  {
		  get_token(objThreadCntrolBlock);
	  }
	  FST_INFO("Execute call_interpreter begin at call_interpreter.");
	  iRet = call_interpreter(objThreadCntrolBlock, 0);
	  FST_INFO("Execute call_interpreter over at call_interpreter.");
	  if(objThreadCntrolBlock->p_buf)
	  {
	    FST_INFO("free(objThreadCntrolBlock->p_buf);");
		free(objThreadCntrolBlock->p_buf);
		objThreadCntrolBlock->p_buf = NULL ;
		objThreadCntrolBlock->prog = NULL ;
		objThreadCntrolBlock->prog_end = NULL ;
	  }
	  FST_INFO("free(objThreadCntrolBlock->instrSet);");
	  if(objThreadCntrolBlock->instrSet)
	  {
	    FST_INFO("free(objThreadCntrolBlock->instrSet);");
		free(objThreadCntrolBlock->instrSet);
		objThreadCntrolBlock->instrSet = NULL ;
	  }
	  FST_INFO("free(objThreadCntrolBlock->iSubProgNum);");
	  for(int i = 0; i < objThreadCntrolBlock->iSubProgNum; i++)
	  {
	      FST_INFO("objThreadCntrolBlock->sub_prog[%d]", i);
		  if (objThreadCntrolBlock->sub_prog[i])
		  {
	          FST_INFO("free objThreadCntrolBlock->sub_prog[%d]", i);
			  free(objThreadCntrolBlock->sub_prog[i]);
			  objThreadCntrolBlock->sub_prog[i] = NULL ;
		  }
	  }
	  objThreadCntrolBlock->global_vars.clear();
	  objThreadCntrolBlock->local_var_stack.clear();
	  // objThreadCntrolBlock->func_call_stack.clear();
	  while(!objThreadCntrolBlock->func_call_stack.empty()) 
	  	objThreadCntrolBlock->func_call_stack.pop();
	  
	  objThreadCntrolBlock->prog_jmp_line.clear();
	  objThreadCntrolBlock->sub_label_table.clear();
	  objThreadCntrolBlock->start_mov_position.clear();
	  objThreadCntrolBlock->vector_XPath.clear();
	  FST_INFO("return %d", iRet);
	  return iRet;
  }

  // printProgJmpLine(objThreadCntrolBlock);
  objThreadCntrolBlock->iLineNum = calc_line_from_prog(objThreadCntrolBlock);
  FST_INFO("Start Interaptor : Line number = %d ", objThreadCntrolBlock->iLineNum);
  iLinenum = objThreadCntrolBlock->iLineNum;
  
// #ifdef WIN32
//   objThreadCntrolBlock->prog_mode = STEP_MODE;
// #endif
  isExecuteEmptyLine = 0 ;
  do {
	// Deal PAUSED_R at the beginning on 190125 
	InterpreterState interpreterState  = getPrgmState();
	while(interpreterState == INTERPRETER_PAUSED)
	{
		FST_INFO("interpreterState is PAUSED_R.");
#ifdef WIN32
		interpreterState =  INTERPRETER_EXECUTE;
		Sleep(1000);
#else
		interpreterState  = getPrgmState();
		sleep(1);
#endif
	}
  	if((objThreadCntrolBlock->prog_mode == STEP_MODE)
		&& (isExecuteEmptyLine == 0))
  	{
  	    // Get curent Line
	    memset(cLineContent, 0x00, LINE_CONTENT_LEN);
		cLineContentPtr = cLineContent ;
		cLineContentProgPtr = objThreadCntrolBlock->prog ;
        while(*cLineContentProgPtr!='\n'
			&& *cLineContentProgPtr!='\r'
			&& *cLineContentProgPtr!='\0')
		{
			*cLineContentPtr++=*cLineContentProgPtr++;
		}

		if(strlen(cLineContent) != 0)
		{
//		    setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSED);
  			FST_INFO("PAUSED: Line number(%s) at %d", cLineContent, iLinenum);
			int iOldLinenum = iLinenum ;
			// iScan = scanf("%d", &iLinenum);
			
			setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSED) ; // WAITING_R ;
            FST_INFO("call_interpreter : Enter waitInterpreterStateleftPaused %d ", iLinenum);
			// Set the iLineNum as the number of executed line
			// and prog had point to the next line. 
			// So Calling calc_line_from_prog to get the lineNum is forbidden here
			setLinenum(objThreadCntrolBlock, iLinenum);
			waitInterpreterStateleftPaused(objThreadCntrolBlock);
            FST_INFO("call_interpreter : Left  waitInterpreterStateleftPaused %d ", iLinenum);
			
			// use the iLineNum which had been set in the BACKWARD/FORWARD/JUMP
			iLinenum = getLinenum(objThreadCntrolBlock) ; // objThreadCntrolBlock->iLineNum ;
			// 
  			FST_INFO("interpreterState : Line number(%d) with %d", iLinenum, iOldLinenum);
			if(iLinenum == 0)
			{
				FST_INFO("illegal line number: %d.", iLinenum);
				// serror(objThreadCntrolBlock, 17); // exit(1);
				continue ;
			}
			if(iOldLinenum != iLinenum - 1)
			{
  			    // FST_INFO("Insert movej at (%d) with %d", iLinenum, iOldLinenum);
			}
			// scanf("%s", cLinenum);
			// iLinenum = atoi(cLinenum);
			// Set prog at the iLineNum which had been set in the BACKWARD/FORWARD/JUMP
			if(iLinenum > 0) // ((iLinenum > 0) && (iLinenum < 1024))
			{
				if(objThreadCntrolBlock->prog_jmp_line[iLinenum - 1].start_prog_pos!= 0)
				{
					if(objThreadCntrolBlock->prog > 
						objThreadCntrolBlock->prog_jmp_line[iLinenum - 1].start_prog_pos)
					{
						FST_INFO("objThreadCntrolBlock->prog : (%08X) and start_prog_pos (%08X) ",
							(unsigned int)objThreadCntrolBlock->prog, 
							(unsigned int)objThreadCntrolBlock->prog_jmp_line[iLinenum - 1].start_prog_pos);
					}
					objThreadCntrolBlock->prog =
						objThreadCntrolBlock->prog_jmp_line[iLinenum - 1].start_prog_pos;
  			        FST_INFO("objThreadCntrolBlock->prog : Line number(%d) ", iLinenum);
				}
			}
			// Not need to execute this state
		    // setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSED);
  			FST_INFO("setPrgmState(EXECUTE_TO_PAUSE_T).");
		    setPrgmState(objThreadCntrolBlock, INTERPRETER_EXECUTE_TO_PAUSE);
#ifdef WIN32
			Sleep(1);
#else
	        usleep(1000);
#endif
  			// FST_INFO("interpreterState : Line number(%d) ", iLinenum);
  			printCurrentLine(objThreadCntrolBlock);
  			FST_INFO("setPrgmState(EXECUTE_R).");
		    setPrgmState(objThreadCntrolBlock, INTERPRETER_EXECUTE);
		}
  	}
	else if(objThreadCntrolBlock->prog_mode == ERROR_MODE)
    {
	    iLinenum = calc_line_from_prog(objThreadCntrolBlock);
	    FST_ERROR("objThreadCntrolBlock crash at line %d ", iLinenum);
        return 0 ; // NULL ;
	}
	isExecuteEmptyLine = 0 ;
    objThreadCntrolBlock->token_type = get_token(objThreadCntrolBlock);
	// Deal abort
	if(objThreadCntrolBlock->is_abort == true)
	{
		// setPrgmState(objThreadCntrolBlock, PAUSE_TO_IDLE_T) ;
  		FST_INFO("objThreadCntrolBlock->is_abort == true.");
        break ; // return 0 ; // NULL ;
	}
	// Deal PAUSED_R opration had moved to the beginning on 190125 
    /* check for assignment statement */
	iLinenum = calc_line_from_prog(objThreadCntrolBlock);
    FST_INFO("objThreadCntrolBlock->token_type = %d at line %d ", 
    	objThreadCntrolBlock->token_type, iLinenum);
	setLinenum(objThreadCntrolBlock, iLinenum);
    if(objThreadCntrolBlock->token_type==VARIABLE) {
      putback(objThreadCntrolBlock); /* return the var to the input stream */
      assignment(objThreadCntrolBlock); /* must be assignment statement */
    }
	else if(objThreadCntrolBlock->token_type==INSIDEFUNC) /* is INSIDEFUNC */
	{
		putback(objThreadCntrolBlock); /* return the var to the input stream */
		int iRet = exec_call(objThreadCntrolBlock);
		if(iRet == END_COMMND_RET)
			return END_COMMND_RET;
	}
	else if(objThreadCntrolBlock->token_type==OUTSIDEFUNC) /* is OUTSIDEFUNC */
	{
		putback(objThreadCntrolBlock); /* return the var to the input stream */
		int iRet = exec_call(objThreadCntrolBlock);
		if(iRet == END_COMMND_RET)
			return END_COMMND_RET;
	}
	else if(objThreadCntrolBlock->token_type==INNERCMD) /* is command */
	{
	    int iIdx = find_internal_cmd(objThreadCntrolBlock->token);
		int iLineNum = calc_line_from_prog(objThreadCntrolBlock);
		// We had eaten MOV* as token. 
		// iLineNum = iLineNum - 1 ;
		if(objThreadCntrolBlock->is_main_thread == MONITOR_THREAD)  // not main
		{
			if(call_internal_cmd_exec_sub_thread(iIdx) == 0) // 0 - mov 1 - nonmov
			{
				FST_INFO("Non execution permissions : %s", objThreadCntrolBlock->token);
				find_eol(objThreadCntrolBlock);
			}
			else
			{
    				FST_INFO("call_internal_cmd objThreadCntrolBlock at %d", iIdx);
				int iRet = call_internal_cmd(iIdx, iLineNum, 
					objThreadCntrolBlock);
				// find_eol(objThreadCntrolBlock);
				if(iRet == END_COMMND_RET)
					return END_COMMND_RET;
			}
		}
		else // main
		{
			// if(call_internal_cmd_exec_sub_thread(iIdx) == 0) // 0 - mov 1 - nonmov
			// {
			//	if (isInstructionEmpty(SHM_INTPRT_CMD))
		    //    {
		    //        FST_INFO("check if step is done in call_interpreter");
		    //        // setPrgmState(objThreadCntrolBlock, INTERPRETER_PAUSED);
		    //    }
			// } 
    		FST_INFO("call_internal_cmd execution : %s at %d, iLineNum = %d", 
						objThreadCntrolBlock->token, iIdx, iLineNum);
			printCurrentLine(objThreadCntrolBlock);
			int iRet = call_internal_cmd(iIdx, iLineNum, 
				objThreadCntrolBlock);
			// find_eol(objThreadCntrolBlock);
			if(iRet == END_COMMND_RET)
				return END_COMMND_RET;
		}
    }
    else if(objThreadCntrolBlock->token_type==COMMAND) /* is command */
	{
      switch(objThreadCntrolBlock->tok) {
        case SUB:
		  // Skip Function declaration by jiaming.lu at 180717
		  find_eol(objThreadCntrolBlock);
  		  break;
        case NOP:
		  // Skip Function declaration by jiaming.lu at 180717
		  if(objThreadCntrolBlock->prog_mode == STEP_MODE)
		  {
		     FST_INFO("Meet NOP ");
		     setLinenum(objThreadCntrolBlock, getLinenum(objThreadCntrolBlock) + 1);
		  }
		  find_eol(objThreadCntrolBlock);
  		  break;
        case PRINT:
		  print(objThreadCntrolBlock);
  		  break;
		case GOTO:
		  exec_goto(objThreadCntrolBlock);
		  break;
		case IF:
		  exec_if(objThreadCntrolBlock);
		  break;
		case ELSE:
		  exec_else(objThreadCntrolBlock);
		  break;
		case ELSEIF:
		  exec_elseif(objThreadCntrolBlock);
		  break;
		case ENDIF:
		  exec_endif(objThreadCntrolBlock);
		  break;
		case FOR:
		  exec_for(objThreadCntrolBlock);
		  break;
		case NEXT:
		  exec_next(objThreadCntrolBlock);
		  break;
  		case INPUT:
		  input(objThreadCntrolBlock);
		  break;
		case GOSUB:
		  iRet = gosub(objThreadCntrolBlock);
		  if(iRet == END_COMMND_RET)
		     return END_COMMND_RET;;
		  break;
		case RETURN:
		  greturn(objThreadCntrolBlock);
		  // break;
		  return 0 ; // NULL ;
		case BREAK:
		  exec_break(objThreadCntrolBlock);
		  break;
		case CONTINUE:
		  exec_continue(objThreadCntrolBlock);
		  break;
		case SELECT:
		  exec_select(objThreadCntrolBlock);
		  break;
		case CASE:
		  exec_case(objThreadCntrolBlock);
		  break;
		case DEFAULT:
		  // exec_default(objThreadCntrolBlock);
		  find_eol(objThreadCntrolBlock); 
		  break;
		case WHILE:
		  exec_while(objThreadCntrolBlock);
		  break;
		case WEND:
		  exec_wend(objThreadCntrolBlock);
		  break;
		case LOOP:
		  exec_loop(objThreadCntrolBlock);
		  break;
		case ENDLOOP:
		  exec_endloop(objThreadCntrolBlock);
		  break;
		case CALL:
		  iRet = exec_call(objThreadCntrolBlock);
		  if(iRet == END_COMMND_RET)
		     return END_COMMND_RET;
		  break;
		case CALLMACRO:
//			objThreadCntrolBlock->is_in_macro = true ;
			if(objThreadCntrolBlock->prog_mode == STEP_MODE)
			{
				objThreadCntrolBlock->prog_mode = FULL_MODE;
				iRet = exec_call(objThreadCntrolBlock, true);
				objThreadCntrolBlock->prog_mode = STEP_MODE;
			}
			else
			{
				iRet = exec_call(objThreadCntrolBlock, true);
			}
//			objThreadCntrolBlock->is_in_macro = false ;
			if(iRet == END_COMMND_RET)
				return END_COMMND_RET;
		  break;
		case END:
		  // exit(0);
		  iRet = exec_end(objThreadCntrolBlock);
		  if(iRet == 1)
		  {
             return 0 ; // NULL ;
		  }
		  break;
		case IMPORT:
		  exec_import(objThreadCntrolBlock);
		  break;
      }
	}
	else {
		isExecuteEmptyLine = 1 ;
	}
#ifdef JMP_NOP
    // Skip NOP and set new line number
	char tokenType = get_token(objThreadCntrolBlock);
	while((tokenType == DELIMITER)
		||( (objThreadCntrolBlock->token_type==COMMAND) 
		  &&(objThreadCntrolBlock->tok == NOP)))
	{
		tokenType = get_token(objThreadCntrolBlock);
	}
	// Get normal token
	putback(objThreadCntrolBlock);
	// Skip NOP over and set new line number
	setLinenum(objThreadCntrolBlock, calc_line_from_prog(objThreadCntrolBlock));
    // Skip NOP over and set new line number over
#endif
  } while (objThreadCntrolBlock->tok != FINISHED);
  
  FST_INFO("call_interpreter execution over");
  return 0 ; // NULL ;
}

/************************************************* 
	Function:		jump_prog_from_line
	Description:	process INTERPRETER_SERVER_CMD_JUMP.
	Input:			thread_control_block  - interpreter info
	                iLinenum              - Line number
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
int  jump_prog_from_line(struct thread_control_block * objThreadCntrolBlock, int iNum)
{
	if(iNum < (int)objThreadCntrolBlock->prog_jmp_line.size())
	{
		objThreadCntrolBlock->prog = objThreadCntrolBlock->prog_jmp_line[iNum].start_prog_pos;
		return iNum ;
	}
	return 0;
}

/************************************************* 
	Function:		calc_line_from_prog
	Description:	get lineNum by interpreter position.
	Input:			thread_control_block  - interpreter info
	Return: 		Line number (starts from one)
*************************************************/ 
int  calc_line_from_prog(struct thread_control_block * objThreadCntrolBlock)
{
	for(int i = 0 ; i < (int)objThreadCntrolBlock->prog_jmp_line.size() ; i++)
	{
		if(objThreadCntrolBlock->prog < objThreadCntrolBlock->prog_jmp_line[i].end_prog_pos)
		{
			prog_line_info tmpDbg = objThreadCntrolBlock->prog_jmp_line[i];
			FST_INFO("calc_line_from_prog get %d at (%08X, %08X) ", 
		   	    i+1, (unsigned int)tmpDbg.start_prog_pos,
		   	    (unsigned int)objThreadCntrolBlock->prog_jmp_line[i-1].start_prog_pos);
		//   FST_INFO("calc_line_from_prog get %d at (%08X, %08X) ", 
		//   	    i, objThreadCntrolBlock->prog,
		//   	    objThreadCntrolBlock->prog_jmp_line[i-1].start_prog_pos);
		   printCurrentLine(objThreadCntrolBlock);
		// When I found prog is less than end_prog_pos address of lineN in the first time
		// We got the right line. we add one for the line_num starts from one .
		   return i + 1;
		}
	}
	// FST_INFO("calc_line_from_prog Failed return ");
	return 0;
}

/************************************************* 
	Function:		load_program
	Description:	Load a program.
	Input:			thread_control_block  - interpreter info
	                p                     - program buffer
	                iLinenum              - program name
	Output: 		NULL
	Return: 		INT 
*************************************************/ 
int load_program(struct thread_control_block * objThreadCntrolBlock, char *p, char *pname)
{
  char fXMLName[128];
  char fBASName[128];
  FILE *fp = 0 ;
  int i=0;
#ifdef WIN32
  sprintf(fXMLName, "%s.xml", pname);
  sprintf(fBASName, "%s.bas", pname);
  // use bas directly 
  // if((access(fBASName,F_OK))==-1)
  // use XML directly 
  parse_xml_file_wrapper(fXMLName);
#else
  sprintf(fXMLName, "%s/programs/%s.xml", forgesight_get_programs_path(), pname);
  sprintf(fBASName, "%s/programs/%s.bas", forgesight_get_programs_path(), pname);
  // use bas directly 
  // if((access(fBASName,F_OK))==-1)
  // use XML directly 
  if((access(fXMLName, F_OK))==0)
  {   
      parse_xml_file_wrapper(fXMLName);
  }
#endif
  
  if(!(fp=fopen(fBASName, "r"))) 
  {
      FST_ERROR("load_program failed : %s from %s", fBASName, fXMLName);
      serror(objThreadCntrolBlock, 14);
      return 0;
  }

  i = 0;
  do {
    *p = getc(fp);
    p++; i++;
  } while(!feof(fp) && i<PROG_SIZE);
  *(p-2) = '\0'; /* null terminate the program */
  fclose(fp);
  return 1;
}

/************************************************* 
	Function:		release_array_element
	Description:	calculate the index of array.
	Input:			thread_control_block  - interpreter info
	Output: 		NULL
	Return: 		index value
*************************************************/ 
// This function would always left the ']'.
int release_array_element(struct thread_control_block * objThreadCntrolBlock)
{
    eval_value value;
	int boolValue;
	char val_name[128];
	char * temp_prog = 0 ;

	temp_prog = objThreadCntrolBlock->prog;
    get_token(objThreadCntrolBlock);
	strcpy(val_name, objThreadCntrolBlock->token);
	if(!isalpha(*objThreadCntrolBlock->token)) {
	  putback(objThreadCntrolBlock);
	  get_exp(objThreadCntrolBlock, &value, &boolValue);
	  // Jump ']'
      get_token(objThreadCntrolBlock);
	  if(objThreadCntrolBlock->token[0] != ']'){
		 serror(objThreadCntrolBlock, 4);
		 return -1;
	  }
	  // putback ']'
	  putback(objThreadCntrolBlock);
	  return (int)value.getFloatValue() ;
	}
	get_token(objThreadCntrolBlock);
	if(objThreadCntrolBlock->token[0] == '['){
		value.setFloatValue(release_array_element(objThreadCntrolBlock));
		// Jump ']'
		get_token(objThreadCntrolBlock);
		sprintf(val_name, "%s[%d]", val_name, (int)value.getFloatValue());
		value = find_var(objThreadCntrolBlock, val_name);
		// Jump ']'
		get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->token[0] != ']'){
			putback(objThreadCntrolBlock);
			// Simulate left value
			objThreadCntrolBlock->token_type = NUMBER ;
			sprintf(objThreadCntrolBlock->token, "%d", (int)value.getFloatValue());
			// Calc
			level3(objThreadCntrolBlock, &value, &boolValue);
			putback(objThreadCntrolBlock);
		}
		// putback ']'
		putback(objThreadCntrolBlock);
		return (int)value.getFloatValue();
	}
	else {
	  objThreadCntrolBlock->prog = temp_prog;
	  get_exp(objThreadCntrolBlock, &value, &boolValue);

	  get_token(objThreadCntrolBlock);
	  if(objThreadCntrolBlock->token[0] != ']') {
	    serror(objThreadCntrolBlock, 4);
	    return -1;
	  }
	  else {
		// putback ']'
		putback(objThreadCntrolBlock);
	    return (int)value.getFloatValue();
	  }
	}
}


/************************************************* 
	Function:		deal_array_element
	Description:	deal array variable.
	Input:			thread_control_block  - interpreter info
	Output: 		objThreadCntrolBlock->token
	Return: 		NULL
*************************************************/ 
void deal_array_element(struct thread_control_block * objThreadCntrolBlock)
{
    int array_value;
    char array_variable[128];
	char * temp_prog = 0 ;
//	char * start_pos = 0 ;
	memset(array_variable, 0x00, 128);
//    start_pos = array_variable ;

	temp_prog = objThreadCntrolBlock->prog;
	get_token(objThreadCntrolBlock);
    strcpy(array_variable, objThreadCntrolBlock->token) ;
	objThreadCntrolBlock->prog = temp_prog;

//	while(1)
//	{
       get_token(objThreadCntrolBlock);
	   if(!isalpha(objThreadCntrolBlock->token[0])) {
	     serror(objThreadCntrolBlock, 4);
	     return;
	   }
	   get_token(objThreadCntrolBlock);
	   if(objThreadCntrolBlock->token[0] == '['){
	   	  array_value = release_array_element(objThreadCntrolBlock);
		  if(array_value < 0) {
	   	     putback(objThreadCntrolBlock);
		     serror(objThreadCntrolBlock, 4);
		     return;
		  }
          sprintf(objThreadCntrolBlock->token, "%s[%d]", array_variable, (int)array_value);
  	   //   objThreadCntrolBlock->prog--;
	   }
	   else
	   	  putback(objThreadCntrolBlock);
//	   // return ']' to the prog
//	   objThreadCntrolBlock->prog--;
//	}
}

/************************************************* 
	Function:		assignment
	Description:	Assign a variable a value.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL
*************************************************/ 
void assignment(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value value;
  int boolValue;
  char var[80];
  char * tempProg ;

  /* get the variable name */
  tempProg = objThreadCntrolBlock->prog ;
  get_token(objThreadCntrolBlock);
  if(!isalpha(objThreadCntrolBlock->token[0])) {
    serror(objThreadCntrolBlock, 4);
    return;
  }

  // var = toupper(*token)-'A';
  memset(var, 0x00, 80);
  strcpy(var, objThreadCntrolBlock->token);

  get_token(objThreadCntrolBlock);
  if(objThreadCntrolBlock->token[0] == '['){
    objThreadCntrolBlock->prog = tempProg ;
	deal_array_element(objThreadCntrolBlock);
	memset(var, 0x00, 80);
	strcpy(var, objThreadCntrolBlock->token);
	// Eat ]
	get_token(objThreadCntrolBlock);
	if(objThreadCntrolBlock->token[0] != ']') { /* is string */
      serror(objThreadCntrolBlock, 3);
      return;
	}
	// Check the Member of the structure
	get_token(objThreadCntrolBlock);
	if(objThreadCntrolBlock->token[0] == '.') {
	// var = toupper(*token)-'A';
		get_token(objThreadCntrolBlock);
		sprintf(var, "%s.%s", var, objThreadCntrolBlock->token);
	}
	else
	{
	  	putback(objThreadCntrolBlock);
	}
  }
  else
  {
  	putback(objThreadCntrolBlock);
  }

  /* get the equals sign */
  get_token(objThreadCntrolBlock);
  if(*(objThreadCntrolBlock->token)!=EQ) {
    serror(objThreadCntrolBlock, 3);
    return;
  }

  /* get the value to assign to var */
  get_exp(objThreadCntrolBlock, &value, &boolValue);

  /* assign the value */
  // variables[var] = value;
  assign_var(objThreadCntrolBlock, var, value) ;
  find_eol(objThreadCntrolBlock);
}

/************************************************* 
	Function:		print
	Description:	Execute a simple version of the BASIC PRINT statement
	Input:			thread_control_block  - interpreter info
	Return: 		NULL
*************************************************/ 
void print(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value answer;
  int boolValue;

  int len=0, spaces;
  char last_delim;

  do {
    get_token(objThreadCntrolBlock); /* get next list item */
    if(objThreadCntrolBlock->tok==EOL || objThreadCntrolBlock->tok==FINISHED) break;
    if(objThreadCntrolBlock->token_type==QUOTE) { /* is string */
      FST_INFO("%s", objThreadCntrolBlock->token);
      len += strlen(objThreadCntrolBlock->token);
      get_token(objThreadCntrolBlock);
    }
    else { /* is expression */
      putback(objThreadCntrolBlock);
      get_exp(objThreadCntrolBlock, &answer, &boolValue);
      get_token(objThreadCntrolBlock);
      len += printf("%d", (int)answer.getFloatValue());
    }
    last_delim = *(objThreadCntrolBlock->token);

    if(*(objThreadCntrolBlock->token)==';') {
      /* compute number of spaces to move to next tab */
      spaces = 8 - (len % 8);
      len += spaces; /* add in the tabbing position */
      while(spaces) {
	  FST_INFO(" ");
        spaces--;
      }
    }
    else if(*(objThreadCntrolBlock->token)==',') /* do nothing */;
    else if(objThreadCntrolBlock->tok!=EOL
		&& objThreadCntrolBlock->tok!=FINISHED) serror(objThreadCntrolBlock, 0);
  } while (*(objThreadCntrolBlock->token)==';'
  			|| *(objThreadCntrolBlock->token)==',');

  if(objThreadCntrolBlock->tok==EOL || objThreadCntrolBlock->tok==FINISHED) {
    if(last_delim != ';' && last_delim!=',') FST_INFO("");
  }
  else serror(objThreadCntrolBlock, 0); /* error is not , or ; */

}

/************************************************* 
	Function:		scan_labels
	Description:	Find all labels in current objThreadCntrolBlock->prog.
	                Generate line info which records the start/end position and type of each line
	Input:			thread_control_block  - interpreter info
	    			type:      INSIDE_FUNC  - main subroutine
	    			           OUTSIDE_FUNC - import subroutine
	    			pname:     subroutine name
	Output: 		objThreadCntrolBlock->sub_label_table
	         		objThreadCntrolBlock->prog_jmp_line
	Return: 		NULL
*************************************************/ 
void scan_labels(struct thread_control_block * objThreadCntrolBlock, 
				SubLabelType type, char * pname)
{
  int bisFindEOL = 0 ;
  struct prog_line_info_t objProgLineInfo ;
  struct sub_label  objLabel ;
  // int iLineNum = 0 ;
  int addr;
  char *temp;

  temp = objThreadCntrolBlock->prog;   /* save pointer to top of program */

  /* if the first token in the file is a label */
  objProgLineInfo.start_prog_pos = objThreadCntrolBlock->prog ;
  // iLineNum++ ;
  get_token(objThreadCntrolBlock);
  bisFindEOL = 0 ;
  if(objThreadCntrolBlock->token_type==NUMBER) {
    // strcpy(label_table[0].name,token);
    // label_table[0].p=prog;
    objLabel.type = type ;
	
	sprintf(objLabel.name, "%s::%s", pname, 
       			objThreadCntrolBlock->token);
	
	objLabel.p = objThreadCntrolBlock->prog;
	addr = add_label(objThreadCntrolBlock, objLabel);
      if(addr==-1 || addr==-2) {
          (addr==-1) ?serror(objThreadCntrolBlock, 5):serror(objThreadCntrolBlock, 6);
      }
  }
  else if(objThreadCntrolBlock->token_type==COMMAND) {
	  if(objThreadCntrolBlock->tok == SUB)
	  {
		  get_token(objThreadCntrolBlock);
		  objLabel.type = type ;
		  
		  sprintf(objLabel.name, "%s::%s", pname, 
       			objThreadCntrolBlock->token);
			  
		  objLabel.p = objThreadCntrolBlock->prog;
		  // Jump parameter
		  find_eol(objThreadCntrolBlock);
		  bisFindEOL = 1 ;
		  addr = add_label(objThreadCntrolBlock, objLabel);
		  if(addr==-1 || addr==-2) {
			  (addr==-1) ?serror(objThreadCntrolBlock, 5):serror(objThreadCntrolBlock, 6);
		  }
	  }
  }
  else
	  putback(objThreadCntrolBlock);

  
  objProgLineInfo.type = COMMON ;
  if(objThreadCntrolBlock->token_type==INNERCMD) 
  {
	  int iIdx = find_internal_cmd(objThreadCntrolBlock->token) ;
	  if((iIdx >= 0) && (iIdx <= 2))  // movel - 0 ; movej - 1 ; movec - 2
		  objProgLineInfo.type = MOTION ;
  }
  else if(objThreadCntrolBlock->token_type==COMMAND) 
  {
  	  if(objThreadCntrolBlock->tok == END)
	  	objProgLineInfo.type = END_TOK ;
	  else
	  	objProgLineInfo.type = LOGIC_TOK ;
  }
  if (bisFindEOL == 0)
  {
     find_eol(objThreadCntrolBlock);
  }
  objProgLineInfo.end_prog_pos = objThreadCntrolBlock->prog ;
  objThreadCntrolBlock->prog_jmp_line.push_back(objProgLineInfo);
  // iLineNum++ ;
  
  do {
    objProgLineInfo.start_prog_pos = objThreadCntrolBlock->prog ;
    char token_type = get_token(objThreadCntrolBlock);
	if(COMMENT == token_type)
	{
		continue ;
	}
    bisFindEOL = 0 ;
    if(objThreadCntrolBlock->token_type==NUMBER) {
      // strcpy(label_table[addr].name, token);
      // label_table[addr].p = prog;  /* current point in program */
          objLabel.type = type ;
		  sprintf(objLabel.name, "%s::%s", pname, 
	       			objThreadCntrolBlock->token);
		  objLabel.p = objThreadCntrolBlock->prog;

		  addr = add_label(objThreadCntrolBlock, objLabel);
		  if(addr==-1 || addr==-2) {
			  (addr==-1) ?serror(objThreadCntrolBlock, 5):serror(objThreadCntrolBlock, 6);
		  }
    }
	else if(objThreadCntrolBlock->token_type==COMMAND) {
		if(objThreadCntrolBlock->tok == SUB)
		{
			get_token(objThreadCntrolBlock);
            objLabel.type = type ;
			sprintf(objLabel.name, "%s::%s", pname, 
		       			objThreadCntrolBlock->token);
		    objLabel.p = objThreadCntrolBlock->prog;
			// Jump parameter
			find_eol(objThreadCntrolBlock);
		    bisFindEOL = 1 ;
	        addr = add_label(objThreadCntrolBlock, objLabel);
	        if(addr==-1 || addr==-2) {
	          (addr==-1) ?serror(objThreadCntrolBlock, 5):serror(objThreadCntrolBlock, 6);
	        }
		}
	}
	else if(objThreadCntrolBlock->token_type==DELIMITER) {
		if(objThreadCntrolBlock->tok==EOL || objThreadCntrolBlock->tok==FINISHED)
		{
		    bisFindEOL = 1 ;
		}
	}

	objProgLineInfo.type = COMMON ;
	if(objThreadCntrolBlock->token_type==INNERCMD) 
	{
		int iIdx = find_internal_cmd(objThreadCntrolBlock->token) ;
		if((iIdx >= 0) && (iIdx <= 2))	// movel - 0 ; movej - 1 ; movec - 2
			objProgLineInfo.type = MOTION ;
	}
	else if(objThreadCntrolBlock->token_type==COMMAND) 
	{
		if(objThreadCntrolBlock->tok == END)
		  objProgLineInfo.type = END_TOK ;
		else
		  objProgLineInfo.type = LOGIC_TOK ;
	}

    /* if not on a blank line, find next line */
	if (bisFindEOL == 0)
	{
		find_eol(objThreadCntrolBlock);
	}
    objProgLineInfo.end_prog_pos = objThreadCntrolBlock->prog ;
	// Omit invalidated line - Lu.jiaming modified at 181225
	if(objThreadCntrolBlock->tok!=FINISHED)
	{
		objThreadCntrolBlock->prog_jmp_line.push_back(objProgLineInfo);
	}
    // iLineNum++ ;
  } while(objThreadCntrolBlock->tok!=FINISHED);
  objThreadCntrolBlock->prog = temp;  /* restore to original */
}

/************************************************* 
	Function:		find_eol
	Description:	Find the start of the next line and make prog move to the next line.
	Input:			thread_control_block  - interpreter info
	Output: 		objThreadCntrolBlock->prog
	Return: 		NULL
*************************************************/ 
void find_eol(struct thread_control_block * objThreadCntrolBlock)
{
  while(*(objThreadCntrolBlock->prog)!='\n'
  	&& *(objThreadCntrolBlock->prog)!='\0') ++(objThreadCntrolBlock->prog);
  if(*(objThreadCntrolBlock->prog))
  	(objThreadCntrolBlock->prog)++;
}


/************************************************* 
	Function:		add_label
	Description:	Return index of next free position in label array.
	Input:			thread_control_block  - interpreter info
	    			objLabel:               sub_label info
	Output: 		NULL
	Return: 		A -1 is returned if the array is full.
					A -2 is returned when duplicate label is found.
*************************************************/ 
int add_label(struct thread_control_block * objThreadCntrolBlock, struct sub_label objLabel)
{
//  register int t;

//  for(t=0;t<NUM_LAB;++t) {
//    if(label_table[t].name[0]==0) return t;
//    if(!strcmp(label_table[t].name,s)) return -2; /* dup */
//  }
	for(vector<sub_label>::iterator it
		= objThreadCntrolBlock->sub_label_table.begin();
		it != objThreadCntrolBlock->sub_label_table.end(); ++it)
	{
	    if(!strcmp(it->name, objLabel.name))  {
	        return -2; /* dup */
	    }
	}
	objThreadCntrolBlock->sub_label_table.push_back(objLabel);
	return 0 ;

//  return -1;  /* full */
}


/************************************************* 
	Function:		find_label
	Description:	Find location of given label.  A null is returned if
				    label is not found; otherwise a pointer to the position
				    of the label is returned.
	Input:			thread_control_block  - interpreter info
	    			name:                 - label name
	Return: 		program position.
*************************************************/ 
char *find_label(struct thread_control_block * objThreadCntrolBlock, char *name)
{
//  register int t;

//  for(t=0; t<NUM_LAB; ++t)
//    if(!strcmp(label_table[t].name,s)) return label_table[t].p;
//  return '\0'; /* error condition */
	for(vector<sub_label>::iterator it
		= objThreadCntrolBlock->sub_label_table.begin();
	   it != objThreadCntrolBlock->sub_label_table.end(); ++it)
	{
#ifndef WIN32
        // case-insensitive.
	    if(!strcasecmp(it->name, name))  {
#else
        // case-insensitive.
	    if(!stricmp(it->name, name))  {
#endif
	        return it->p;
	    }
	}
	return '\0';
}

/************************************************* 
	Function:		find_label_by_type
	Description:	Find location of given label.  A null is returned if
				    label is not found; otherwise a pointer to the position
				    of the label is returned.
	Input:			thread_control_block  - interpreter info
	    			name:                 - label name
	Return: 		label_type.
*************************************************/ 
SubLabelType find_label_by_type(struct thread_control_block * objThreadCntrolBlock, char *name)
{
//  register int t;

//  for(t=0; t<NUM_LAB; ++t)
//    if(!strcmp(label_table[t].name,s)) return label_table[t].p;
//  return '\0'; /* error condition */
	for(vector<sub_label>::iterator it
		= objThreadCntrolBlock->sub_label_table.begin();
	   it != objThreadCntrolBlock->sub_label_table.end(); ++it)
	{
	    if(!strcmp(it->name, name))  {
	        return it->type;
	    }
	}
	return SUBLABEL_NONE;
}


/************************************************* 
	Function:		jumpout_one_block_in_loc
	Description:	jumpout a program block when determine condition is not false. 
	Input:			thread_control_block  - interpreter info
	    			startFlag:            - the program block type: such as if/while
	    			endFlag:              - the end type of startFlag : such as endif/wend
	Output: 		loc                   - program location
	Return: 		SUCCESS or FAILED.
*************************************************/ 
#define JUMP_OUT_INIT    0
#define JUMP_OUT_OK      1
#define JUMP_OUT_RANGE   2
static int jumpout_one_block_in_loc(struct thread_control_block * objThreadCntrolBlock,
		char * loc,int startFlag, int endFlag)
{
     while(1)
	 {
	    get_token(objThreadCntrolBlock);
	 	if (objThreadCntrolBlock->tok==startFlag) // Finish if
		{
			jumpout_one_block_in_loc(objThreadCntrolBlock, loc, startFlag, endFlag);
	    }
		else if (objThreadCntrolBlock->tok==endFlag) // Finish if
		{
			if(objThreadCntrolBlock->tok == END)
			{
			  get_token(objThreadCntrolBlock);
			  if(objThreadCntrolBlock->tok == SELECT)
			  {
				return JUMP_OUT_OK ;
			  }
			  serror(objThreadCntrolBlock, 1);
			  exit(0);
			}
			return JUMP_OUT_OK ;
	    }

		if(objThreadCntrolBlock->prog > loc)
		{
			return JUMP_OUT_RANGE ;
		}
     }
}

/************************************************* 
	Function:		jumpout_one_block_in_loc
	Description:	jumpout a program block when determine condition is not false. 
	Input:			thread_control_block  - interpreter info
	Output: 		loc                   - program location
	Return: 		SUCCESS or FAILED.
*************************************************/ 
static int jumpout_blocks_in_loc(struct thread_control_block * objThreadCntrolBlock,
			char * loc)
{
  int iRet = JUMP_OUT_INIT ;
  select_and_cycle_stack objCloseBlock ;
  while(objThreadCntrolBlock->select_and_cycle_tos > 0)
  {
  	 objCloseBlock = select_and_cycle_pop(objThreadCntrolBlock);
	 iRet = JUMP_OUT_INIT ;
	 if(objCloseBlock.itokentype == IF)
	 {
	     iRet = jumpout_one_block_in_loc(objThreadCntrolBlock, loc,
		 			objCloseBlock.itokentype, ENDIF);
	 }
	 else if(objCloseBlock.itokentype == WHILE)
	 {
	     iRet = jumpout_one_block_in_loc(objThreadCntrolBlock, loc,
		 			objCloseBlock.itokentype, WEND);
	 }
	 else if(objCloseBlock.itokentype == SELECT)
	 {
	     iRet = jumpout_one_block_in_loc(objThreadCntrolBlock, loc,
		 			objCloseBlock.itokentype, END);
	 }

	 if(iRet == JUMP_OUT_RANGE)
	 {
	 	select_and_cycle_push(objThreadCntrolBlock, objCloseBlock);
	 }
  }
  return 1 ;
}

/************************************************* 
	Function:		exec_goto
	Description:	Execute a GOTO statement.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_goto(struct thread_control_block * objThreadCntrolBlock)
{
  char *loc;

  get_token(objThreadCntrolBlock); /* get label to go to */
  /* find the location of the label */
  loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
  if(loc=='\0')
  {
     serror(objThreadCntrolBlock, 7); /* label not defined */
  }
  else
  {
  	 jumpout_blocks_in_loc(objThreadCntrolBlock, loc);
  	 objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
  }
}


/************************************************* 
	Function:		label_init (legacy)
	Description:	Initialize the array that holds the labels.
				    By convention, a null label name indicates that
				    array position is unused.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void label_init(struct thread_control_block * objThreadCntrolBlock)
{
  // register int t;

  // for(t=0; t<NUM_LAB; ++t) label_table[t].name[0]='\0';
}


/************************************************* 
	Function:		calc_conditions
	Description:	calculate the condition result in if/while statement.
	Input:			thread_control_block  - interpreter info
	Return: 		condition result.
*************************************************/ 
int calc_conditions(
		struct thread_control_block * objThreadCntrolBlock)
{
  eval_value x ;
  int boolValue = 1; // true - 0; // false
  get_exp(objThreadCntrolBlock, &x, &boolValue); /* get left expression */
//	return calc_single_condition(objThreadCntrolBlock);
  return boolValue ;
}

/************************************************* 
	Function:		exec_if
	Description:	Execute an IF statement.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_if(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value x ;
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack if_stack ;
  int cond;

  cond = calc_conditions(objThreadCntrolBlock);
  x.setFloatValue(cond);

  if(cond) { /* is true so process target of IF */
    get_token(objThreadCntrolBlock);
    if(objThreadCntrolBlock->tok!=THEN) {
      serror(objThreadCntrolBlock, 8);
      return;
    }/* else program execution starts on next line */
    else
    {
       find_eol(objThreadCntrolBlock);
    }
    if_stack.itokentype = IF ;
	if_stack.target = x;
    select_and_cycle_push(objThreadCntrolBlock, if_stack);
  }
  // else find_eol(); /* find start of next line */
  else
  {
     while(1)
	 {
	    get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->tok==IF)
		{
		   iRet = jumpout_one_block_in_loc(objThreadCntrolBlock,
			        objThreadCntrolBlock->prog_end, IF, ENDIF);

		   if(iRet == JUMP_OUT_RANGE)
		   {
              FST_ERROR("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if(objThreadCntrolBlock->tok==ELSE)  // Execute else
		{
		    if_stack.itokentype = IF ;
			if_stack.target = x;
            select_and_cycle_push(objThreadCntrolBlock, if_stack);
			break ;
	    }
		else if(objThreadCntrolBlock->tok==ELSEIF)  // Execute else
		{
		    /* is false so process target of ELSEIF */
		    if_stack.itokentype = IF ;
			if_stack.target = x;
            select_and_cycle_push(objThreadCntrolBlock, if_stack);
			putback(objThreadCntrolBlock);
			break ;
	    }
	 	else if (objThreadCntrolBlock->tok==ENDIF) // Finish if
		{
  	        find_eol(objThreadCntrolBlock);
			break ;
	    }
     }
  }
}

/************************************************* 
	Function:		exec_else
	Description:	Execute an ELSE statement.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_else(struct thread_control_block * objThreadCntrolBlock)
{
  select_and_cycle_stack if_stack ;
  int iRet = JUMP_OUT_INIT ;
  while(1)
  {
    get_token(objThreadCntrolBlock);
	if(objThreadCntrolBlock->tok==IF)
	{
	   iRet = jumpout_one_block_in_loc(objThreadCntrolBlock,
		   objThreadCntrolBlock->prog_end, IF, ENDIF);

	   if(iRet == JUMP_OUT_RANGE)
	   {
          FST_ERROR("%s  out range", objThreadCntrolBlock->token);
		  return;
	   }
	}
	else if(objThreadCntrolBlock->tok==ELSE)  // Execute else
	{
	    serror(objThreadCntrolBlock, 1); /* not a legal operator */
		break ;
    }
	else if(objThreadCntrolBlock->tok==ELSEIF)  // Execute else
	{
	    serror(objThreadCntrolBlock, 1); /* not a legal operator */
		break ;
    }
 	else if (objThreadCntrolBlock->tok==ENDIF) // Finish if
	{
 	    if_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
 	    if(if_stack.itokentype != IF){
		   serror(objThreadCntrolBlock, 4);
		   return;
 	    }
		break ;
    }
  }
}

/************************************************* 
	Function:		exec_else
	Description:	Execute an ELSEIF statement.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_elseif(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack if_stack ;
  // float x , y;
  int cond;
  
  if_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(if_stack.itokentype != IF){
	serror(objThreadCntrolBlock, 4);
	return;
  }

  if(if_stack.target.getFloatValue() != 0.0)  // if statement is true
  	cond = 0 ;
  else
  	cond = calc_conditions(objThreadCntrolBlock);

  if(cond) { /* is true so process target of IF */
    get_token(objThreadCntrolBlock);
    if(objThreadCntrolBlock->tok!=THEN) {
      serror(objThreadCntrolBlock, 8);
      return;
    }/* else program execution starts on next line */
    else
    {
      find_eol(objThreadCntrolBlock);
    }
    if_stack.itokentype = IF ;
    select_and_cycle_push(objThreadCntrolBlock, if_stack);
  }
  // else find_eol(); /* find start of next line */
  else
  {
     while(1)
	 {
	    get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->tok==IF)
		{
		   iRet = jumpout_one_block_in_loc(objThreadCntrolBlock,
		   			objThreadCntrolBlock->prog_end, IF, ENDIF);

		   if(iRet == JUMP_OUT_RANGE)
		   {
              FST_ERROR("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if(objThreadCntrolBlock->tok==ELSE)  // Execute else
		{
		    if_stack.itokentype = IF ;
            select_and_cycle_push(objThreadCntrolBlock, if_stack);
			break ;
	    }
		else if(objThreadCntrolBlock->tok==ELSEIF)  // Execute else
		{
			putback(objThreadCntrolBlock);
			break ;
	    }
	 	else if (objThreadCntrolBlock->tok==ENDIF) // Finish if
		{
			find_eol(objThreadCntrolBlock);
			break ;
	    }
     }
  }
}

/************************************************* 
	Function:		exec_else
	Description:	Execute an FOR loop.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_for(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack for_stack;
  eval_value value, tmp;
  int boolValue;

  get_token(objThreadCntrolBlock); /* read the control variable */
  if(!isalpha(*(objThreadCntrolBlock->token))) {
    serror(objThreadCntrolBlock, 4);
    return;
  }

  // for_stack.var=toupper(*token)-'A'; /* save its index */
  memset(for_stack.var, 0x00, 80);
  strcpy(for_stack.var, objThreadCntrolBlock->token);

  get_token(objThreadCntrolBlock); /* read the equals sign */
  if(*(objThreadCntrolBlock->token)!=EQ) {
    serror(objThreadCntrolBlock, 3);
    return;
  }

  get_exp(objThreadCntrolBlock, &value, &boolValue); /* get initial value */

  // variables[for_stack.var]=value;
  assign_var(objThreadCntrolBlock, for_stack.var, value) ;

  get_token(objThreadCntrolBlock);
  if(objThreadCntrolBlock->tok!=TO) serror(objThreadCntrolBlock, 9); /* read and discard the TO */

  get_exp(objThreadCntrolBlock, &for_stack.target, &boolValue); /* get target value */

  /* if loop can execute at least once, push info on stack */
  // if(value>=variables[for_stack.var]) {
  tmp = find_var(objThreadCntrolBlock, for_stack.var);
  if(value.getFloatValue() >= tmp.getFloatValue()) {
  	for_stack.itokentype = FOR ;
	find_eol(objThreadCntrolBlock);
    for_stack.loc = objThreadCntrolBlock->prog;
    select_and_cycle_push(objThreadCntrolBlock, for_stack);
  }
  else  /* otherwise, skip loop code altogether */
  {
     while(1)
	 {
	 	get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->tok==FOR)
		{
		   jumpout_one_block_in_loc(objThreadCntrolBlock,
			   objThreadCntrolBlock->prog_end, FOR, NEXT);

		   if(iRet == JUMP_OUT_RANGE)
		   {
              FST_ERROR("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if (objThreadCntrolBlock->tok==NEXT)
		{
			find_eol(objThreadCntrolBlock);
			break ;
	    }
     }
  }
}

/************************************************* 
	Function:		exec_else
	Description:	Execute an NEXT statement.
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_next(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value value;
  struct select_and_cycle_stack for_stack;

  for_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(for_stack.itokentype != FOR){
     serror(objThreadCntrolBlock, 4);
     return;
  }
  // variables[for_stack.var]++; /* increment control variable */
  value = find_var(objThreadCntrolBlock, for_stack.var);
  // int iVar = find_var(objThreadCntrolBlock, for_stack.var);
  // iVar++ ;
  value.increaseFloatValue();
  assign_var(objThreadCntrolBlock, for_stack.var, value); // iVar);

  // if(variables[for_stack.var]>for_stack.target) return;  /* all done */
  value = find_var(objThreadCntrolBlock, for_stack.var);
  if(value.getFloatValue() > for_stack.target.getFloatValue()) 
  {
  	  find_eol(objThreadCntrolBlock);
      return;  /* all done */
  }
  select_and_cycle_push(objThreadCntrolBlock, for_stack);  /* otherwise, restore the info */
  objThreadCntrolBlock->prog = for_stack.loc;  /* loop */
}

/************************************************* 
	Function:		exec_loop
	Description:	Execute an loop statement. such like "loop 5"
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_loop(struct thread_control_block * objThreadCntrolBlock)
{
  // int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack loop_stack;
  // int value;
  eval_value value;
  int boolValue;

  // for_stack.var=toupper(*token)-'A'; /* save its index */
  memset(loop_stack.var, 0x00, 80);
  strcpy(loop_stack.var, "loop_var");
  value.setFloatValue( 1 ) ;
  assign_var(objThreadCntrolBlock, loop_stack.var, value) ;

  // deal target such like "5"
  get_exp(objThreadCntrolBlock, &loop_stack.target, &boolValue); /* get target value */

  /* if loop can execute at least once, push info on stack */
  loop_stack.itokentype = LOOP ;
  find_eol(objThreadCntrolBlock);
  loop_stack.loc = objThreadCntrolBlock->prog;
  select_and_cycle_push(objThreadCntrolBlock, loop_stack);
}

/************************************************* 
	Function:		exec_endloop
	Description:	Execute an exec_endloop statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_endloop(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value value;
  struct select_and_cycle_stack loop_stack;

  loop_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(loop_stack.itokentype != LOOP){
     serror(objThreadCntrolBlock, 4);
     return;
  }
  // variables[for_stack.var]++; /* increment control variable */
  value = find_var(objThreadCntrolBlock, loop_stack.var);
  // int iVar = find_var(objThreadCntrolBlock, loop_stack.var);
  // iVar++ ;
  value.increaseFloatValue();
  assign_var(objThreadCntrolBlock, loop_stack.var, value); // iVar);
  value = find_var(objThreadCntrolBlock, loop_stack.var) ;
  // if(variables[for_stack.var]>for_stack.target) return;  /* all done */
  if(value.getFloatValue() > loop_stack.target.getFloatValue())
  {
  	  find_eol(objThreadCntrolBlock);
      return;  /* all done */
  }
  select_and_cycle_push(objThreadCntrolBlock, loop_stack);  /* otherwise, restore the info */
  objThreadCntrolBlock->prog = loop_stack.loc;  /* loop */
}

/************************************************* 
	Function:		exec_while
	Description:	Execute a WHILE loop. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_while(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack while_stack;
  // float x, y ;
  int cond;

  // Save th position after the while
  while_stack.while_loc = objThreadCntrolBlock->prog;

  cond = calc_conditions(objThreadCntrolBlock);

  /* if loop can execute at least once, push info on stack */
  if(cond) { /* is true so process target of IF */
  	// while_stack.compare_op = EQ ;
	
    while_stack.itokentype = WHILE ;
	// use the next line as the start point of while entity
  	find_eol(objThreadCntrolBlock);
    while_stack.loc = objThreadCntrolBlock->prog;
    select_and_cycle_push(objThreadCntrolBlock, while_stack);
  }
  else
  {
     while(1)
	 {
	    get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->tok==WHILE)
		{
		   iRet = jumpout_one_block_in_loc(objThreadCntrolBlock,
			   objThreadCntrolBlock->prog_end, WHILE, WEND);

		   if(iRet == JUMP_OUT_RANGE)
		   {
              FST_ERROR("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if (objThreadCntrolBlock->tok==WEND) // Finish if
		{
  	        find_eol(objThreadCntrolBlock);
			break ;
	    }
     }
  }
}

/************************************************* 
	Function:		exec_wend
	Description:	Execute a WEND statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_wend(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  // float x, y ;
  int cond;
  // char op_char;

  struct select_and_cycle_stack while_stack;

  while_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(while_stack.itokentype != WHILE){
     serror(objThreadCntrolBlock, 4);
     return;
  }
  objThreadCntrolBlock->prog = while_stack.while_loc;  /* loop */

  cond = calc_conditions(objThreadCntrolBlock);

  if(cond) { /* is true so process target of IF */
	  find_eol(objThreadCntrolBlock);
	  select_and_cycle_push(objThreadCntrolBlock, while_stack);  /* otherwise, restore the info */
	  objThreadCntrolBlock->prog = while_stack.loc;  /* loop */
  }
  else
  {
     while(1)
	 {
	    get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->tok==WHILE)
		{
		   iRet = jumpout_one_block_in_loc(objThreadCntrolBlock,
			   objThreadCntrolBlock->prog_end, WHILE, WEND);

		   if(iRet == JUMP_OUT_RANGE)
		   {
              FST_ERROR("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if (objThreadCntrolBlock->tok==WEND) // Finish if
		{
  	        find_eol(objThreadCntrolBlock);
			break ;
	    }
     }
  }
}

/************************************************* 
	Function:		exec_continue
	Description:	Execute a CONTINUE statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_continue(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value value;
  int cond;
  struct select_and_cycle_stack cycle_stack;

  cycle_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  while(cycle_stack.itokentype == IF)
  {
	cycle_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  }

  if(cycle_stack.itokentype == FOR){
      // variables[cycle_stack.var]++; /* increment control variable */
	  value = find_var(objThreadCntrolBlock, cycle_stack.var);
	  // int iVar = find_var(objThreadCntrolBlock, cycle_stack.var);
	  // iVar++ ;
	  assign_var(objThreadCntrolBlock, cycle_stack.var, value); // iVar);

	 // if(variables[cycle_stack.var]>cycle_stack.target)
	 value = find_var(objThreadCntrolBlock, cycle_stack.var);
     if(value.getFloatValue() > cycle_stack.target.getFloatValue())
	 {
         while (objThreadCntrolBlock->tok!=NEXT) 
		 	get_token(objThreadCntrolBlock);
         find_eol(objThreadCntrolBlock);
	     return;  /* all done */
     }
	 select_and_cycle_push(objThreadCntrolBlock, cycle_stack);  /* otherwise, restore the info */
	 objThreadCntrolBlock->prog = cycle_stack.loc;  /* loop */
  }
  else if(cycle_stack.itokentype == WHILE){
	  objThreadCntrolBlock->prog = cycle_stack.while_loc;  /* loop */
      cond = calc_conditions(objThreadCntrolBlock);

	  if(cond) { /* is true so process target of IF */
	      find_eol(objThreadCntrolBlock);
		  select_and_cycle_push(objThreadCntrolBlock, cycle_stack);  /* otherwise, restore the info */
		  objThreadCntrolBlock->prog = cycle_stack.loc;  /* loop */
	  }
	  else
	  {
  		  while (objThreadCntrolBlock->tok!=WEND)
		  	get_token(objThreadCntrolBlock);
          find_eol(objThreadCntrolBlock);
	  	  return ;
	  }
  }
  return ;
}


/************************************************* 
	Function:		exec_break
	Description:	Execute a BREAK statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_break(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack cycle_stack;

  cycle_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  while(cycle_stack.itokentype == IF)
  {
	cycle_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  }

  // variables[cycle_stack.var]++; /* increment control variable */
  // if(variables[cycle_stack.var]>cycle_stack.target) return;  /* all done */
  // select_and_cycle_push(cycle_stack);  /* otherwise, restore the info */
  // prog = cycle_stack.loc;  /* loop */
  while(1)
  {
	  get_token(objThreadCntrolBlock);
	  if((cycle_stack.itokentype == FOR) && (objThreadCntrolBlock->tok==NEXT))
	  {
          find_eol(objThreadCntrolBlock);
		  break;
	  }
	  if((cycle_stack.itokentype == WHILE) && (objThreadCntrolBlock->tok==WEND))
	  {
          find_eol(objThreadCntrolBlock);
		  break;
	  }
	  if((cycle_stack.itokentype == SELECT) && (objThreadCntrolBlock->tok==END))
	  {
		  get_token(objThreadCntrolBlock);
		  if(objThreadCntrolBlock->tok != SELECT){
			  serror(objThreadCntrolBlock, 4);
			  return;
		  }
          find_eol(objThreadCntrolBlock);
		  break;
	  }
  }
}

/************************************************* 
	Function:		exec_select
	Description:	Execute a SELECT statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_select(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value x ;
  int boolValue;
  struct select_and_cycle_stack select_stack;

  get_token(objThreadCntrolBlock);
  /* get next list item */
  if(objThreadCntrolBlock->tok != CASE) { /* is string */
      serror(objThreadCntrolBlock, 3);
      return;
  }
  // get exp
  get_exp(objThreadCntrolBlock, &x, &boolValue); /* get expression */
  select_stack.itokentype = SELECT ;
  select_stack.target = x ;
  select_and_cycle_push(objThreadCntrolBlock, select_stack);
  find_eol(objThreadCntrolBlock);
}

/************************************************* 
	Function:		exec_case
	Description:	Execute a CASE statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_case(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack select_stack ;
  eval_value x ; // , y, cond;
  int boolValue;
  // char op;

  select_stack = select_and_cycle_pop(objThreadCntrolBlock);
  // Wait for test
  if(select_stack.itokentype != SELECT){
     serror(objThreadCntrolBlock, 4);
     return;
  }

  get_exp(objThreadCntrolBlock, &x, &boolValue); /* get expression */

  if(x.getFloatValue() == select_stack.target.getFloatValue()) { 
	 /* is true so process target of CASE */
     select_and_cycle_push(objThreadCntrolBlock, select_stack);  // Use of END SELECt
     find_eol(objThreadCntrolBlock);
     return;
  }
  else
  {
     select_and_cycle_push(objThreadCntrolBlock, select_stack);
     while(1)
	 {
	    get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->tok==SELECT)  // Execute else
		{
			iRet = jumpout_one_block_in_loc(objThreadCntrolBlock,
				objThreadCntrolBlock->prog_end, SELECT, END);
		    if(iRet == JUMP_OUT_RANGE)
		    {
               FST_ERROR("%s  out range", objThreadCntrolBlock->token);
			   return;
		    }
		}
		else if(objThreadCntrolBlock->tok==CASE)  // Execute else
		{
			putback(objThreadCntrolBlock);
			break ;
		}
		else if(objThreadCntrolBlock->tok==DEFAULT)  // Execute else
		{
			putback(objThreadCntrolBlock);
			break ;
	    }
	 	else if (objThreadCntrolBlock->tok==END) // Finish if
		{
	        get_token(objThreadCntrolBlock);
	  	    if(objThreadCntrolBlock->tok!=SELECT) {
			   serror(objThreadCntrolBlock, 4);
			   return;
			}
            find_eol(objThreadCntrolBlock);
			break ;
	    }
     }
  }
}

/************************************************* 
	Function:		exec_endif
	Description:	Execute a ENDIF statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
void exec_endif(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack select_stack ;

  select_stack = select_and_cycle_pop(objThreadCntrolBlock);
  if(IF != select_stack.itokentype) {
     serror(objThreadCntrolBlock, 4);
     return;
  }
  find_eol(objThreadCntrolBlock);
  return;
}

/************************************************* 
	Function:		exec_end
	Description:	Execute a END statement. 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL.
*************************************************/ 
int exec_end(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack select_stack ;

  get_token(objThreadCntrolBlock);
  if(objThreadCntrolBlock->tok == SELECT)
  {
    select_stack = select_and_cycle_pop(objThreadCntrolBlock);
	if(objThreadCntrolBlock->tok!=select_stack.itokentype) {
	   serror(objThreadCntrolBlock, 4);
	   return 0;
	}
    find_eol(objThreadCntrolBlock);
	return 0;
  }
  else if(objThreadCntrolBlock->tok == SUB)
  {
    putback(objThreadCntrolBlock);
  	greturn(objThreadCntrolBlock);
	return 1;
  }
  putback(objThreadCntrolBlock);
/*
  if(select_and_cycle_tos>0)
  {
    select_stack = select_and_cycle_pop();
    if(tok == WHILE){
		return ;
	}
	else {
	   serror(objThreadCntrolBlock, 4);
	   return;
	}
  }
 */
  // exit(0);
  return 1 ;
}

/************************************************* 
	Function:		select_and_cycle_push
	Description:	Push function for the FOR stack. 
	Input:			thread_control_block  - interpreter info
	         		i                     - select_and_cycle_stack element
	Return: 		NULL.
*************************************************/ 
void select_and_cycle_push(struct thread_control_block * objThreadCntrolBlock,
		struct select_and_cycle_stack i)
{
   if(objThreadCntrolBlock->select_and_cycle_tos>SELECT_AND_CYCLE_NEST)
    serror(objThreadCntrolBlock, 10);

  objThreadCntrolBlock->selcyclstack[objThreadCntrolBlock->select_and_cycle_tos]=i;
  objThreadCntrolBlock->select_and_cycle_tos++;
//    FST_INFO("\t\t\t\t\t select_and_cycle_push with %d at %d",
//		i.itokentype,
//		objThreadCntrolBlock->select_and_cycle_tos);
}

/************************************************* 
	Function:		select_and_cycle_push
	Description:	Pop function from the FOR stack. 
	Input:			thread_control_block  - interpreter info
	Return: 		Last select_and_cycle_stack element
*************************************************/ 
struct select_and_cycle_stack select_and_cycle_pop(
	struct thread_control_block * objThreadCntrolBlock)
{
  objThreadCntrolBlock->select_and_cycle_tos--;
  if(objThreadCntrolBlock->select_and_cycle_tos<0)
  {
      select_and_cycle_stack fake ;
	  serror(objThreadCntrolBlock, 11);
      // memset(&fake, 0x00, sizeof(select_and_cycle_stack));
      fake.itokentype = ILLTOK ;
	  FST_ERROR("objThreadCntrolBlock->select_and_cycle_tos<0"); 
	  return fake ;
  }
  else 
  {
  	 return(objThreadCntrolBlock->selcyclstack[objThreadCntrolBlock->select_and_cycle_tos]);
  }
//    FST_INFO("\t\t\t\t\t select_and_cycle_pop with %d", objThreadCntrolBlock->select_and_cycle_tos);
  
}

/************************************************* 
	Function:		input
	Description:	Execute a simple form of the BASIC INPUT command . 
	Input:			thread_control_block  - interpreter info
	Return: 		NULL
*************************************************/ 
void input(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value value;
  // char str[80], var;
  char var[80];
  int i;

  get_token(objThreadCntrolBlock); /* see if prompt string is present */
  if(objThreadCntrolBlock->token_type==QUOTE) {
    FST_INFO("%s", objThreadCntrolBlock->token); /* if so, print it and check for comma */
    get_token(objThreadCntrolBlock);
    if(*(objThreadCntrolBlock->token)!=',') serror(objThreadCntrolBlock, 1);
    get_token(objThreadCntrolBlock);
  }
  else FST_INFO("? "); /* otherwise, prompt with / */
  // var = toupper(*token)-'A'; /* get the input var */
  memset(var, 0x00, 80);
  strcpy(var, objThreadCntrolBlock->token);

  scanf("%d", &i); /* read input */

  // variables[var] = i; /* store it */
  value.setFloatValue(i) ; 
  assign_var(objThreadCntrolBlock, var, value); // i) ;
}

/************************************************* 
	Function:		call_inner_func
	Description:	Execute inner defined function . 
	Input:			thread_control_block  - interpreter info
	Ouput:			result                - return value
	Return: 		NULL
*************************************************/ 
bool call_inner_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result)
{
    eval_value value;
    int boolValue;
	int count ;
	char temp[NUM_OF_PARAMS][LAB_LEN];

	int funcIdx = find_internal_func(objThreadCntrolBlock->token);

    count = 0;
    get_token(objThreadCntrolBlock);
	// Fit for circumstance without parameter
	if((*(objThreadCntrolBlock->token) == '\r')
		|| (*(objThreadCntrolBlock->token) == '\n'))
	{
		result->setFloatValue(0.0);
		return false;
	}
    if(*(objThreadCntrolBlock->token) != '(')
    {
		serror(objThreadCntrolBlock, 2);
		result->setFloatValue(0.0);
		return false;
	}

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
		if( (value.getType() == (int)(TYPE_STRING | TYPE_SR))
		  ||(value.getType() == (int)(TYPE_STRING)))
		{
			sprintf(temp[count], "%s", value.getStringValue().c_str()); // save temporarily
		}
		else if( (value.getType() == (int)(TYPE_JOINT | TYPE_PR))
		       ||(value.getType() ==  (int)(TYPE_POSE | TYPE_PR)))
		{
			// sprintf(temp[count], "%f", value.getFloatValue()); // save temporarily
		}
		else // It is number in most times
		{
			sprintf(temp[count], "%f", value.getFloatValue()); // save temporarily
		}
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    // count--;
	
	if(count != get_func_params_num(funcIdx))
	{
		serror(objThreadCntrolBlock, 18);
		return false;
    }
    // Now, push on local_var_stack in reverse order.
    if(count == PARAM_NUM_ONE)
	{
		return call_internal_func(funcIdx, result, temp[0]);
    }
	else if(count == PARAM_NUM_TWO)
	{
		return call_internal_func(funcIdx, result, temp[0], temp[1]);
    }
	else if(count == PARAM_NUM_THR)
	{
		return call_internal_func(funcIdx, result, temp[0], temp[1], temp[2]);
    }
	else 
	{
		serror(objThreadCntrolBlock, 18);
		return false;
    }
	return true;
}

/************************************************* 
	Function:		get_args
	Description:	Push the arguments to a function onto the local variable stack.
                    By the mean, Just get parameter values of function 
                    and save as the local variable. 
                    Ex. To func1(1+3, 2, 3), save 4, 2, 3 into local_var_stack
	Input:			thread_control_block  - interpreter info
	Ouput:			objThreadCntrolBlock->local_var_stack
	Return: 		NULL
*************************************************/ 
void get_args(struct thread_control_block * objThreadCntrolBlock)
{
    eval_value value;
    int boolValue;
	int count ;
	float temp[NUM_OF_PARAMS];
    var_type vt;

    count = 0;
    get_token(objThreadCntrolBlock);
	// Fit for circumstance without parameter
	if((*(objThreadCntrolBlock->token) == '\r')
		|| (*(objThreadCntrolBlock->token) == '\n'))
		return ;
    if(*(objThreadCntrolBlock->token) != '(')
    {
    	serror(objThreadCntrolBlock, 2);
		return;
	}
    get_token(objThreadCntrolBlock);
    if(*(objThreadCntrolBlock->token) == ')')
    {
		return;
	}
	putback(objThreadCntrolBlock);

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
        temp[count] = value.getFloatValue(); // save temporarily
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    count--;

    // Now, push on local_var_stack in reverse order.
    for(; count>=0; count--) {
        vt.value.setFloatValue(temp[count]);
        // vt.v_type = FORSIGHT_INT;
        objThreadCntrolBlock->local_var_stack.push_back(vt);
    }
}

/************************************************* 
	Function:		get_params
	Description:	Get function parameters.
                    By the mean, Just bing parameter values while were set in the get_args
                    with the parameter variable in the function defination. 
                    Ex. To func1(1+3, 2, 3), 
                        In the func1(a, b, c), a =4, b = 2, c = 3
	Input:			thread_control_block  - interpreter info
	Ouput:			objThreadCntrolBlock->local_var_stack
	Return: 		NULL
*************************************************/ 
void get_params(struct thread_control_block * objThreadCntrolBlock)
{
    var_type *p;
    int i;

    i = objThreadCntrolBlock->local_var_stack.size()-1;
	if(i < 0)
    {
		// No parameter
    	// serror(objThreadCntrolBlock, 2);
		return;
	}

    get_token(objThreadCntrolBlock);
    if(*(objThreadCntrolBlock->token) != '(')
    {
	    serror(objThreadCntrolBlock, 7); /* label not defined */
		return;
    }

    // Process comma-separated list of parameters.
    do {
        get_token(objThreadCntrolBlock);
        p = &(objThreadCntrolBlock->local_var_stack[i]);
        if(*(objThreadCntrolBlock->token) != ')' ) {
            // Link parameter name with argument already on
            // local var stack.
            strcpy(p->var_name, objThreadCntrolBlock->token);
			// Get ','
            get_token(objThreadCntrolBlock);
			if(i < 0)
		    {
		    	serror(objThreadCntrolBlock, 2);
				return;
			}
            i--;
        }
        else
			break;
    } while(*(objThreadCntrolBlock->token) == ',');

    if(*(objThreadCntrolBlock->token) != ')')
    {
	    serror(objThreadCntrolBlock, 7); /* label not defined */
		return;
    }
}

/************************************************* 
	Function:		exec_import
	Description:	Execute a IMPORT statement. 
	Input:			thread_control_block  - interpreter info
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void exec_import(struct thread_control_block * objThreadCntrolBlock)
{
  char *proglabelsScan; 
  
  // int addr;
  struct sub_label  objLabel ;
  char * temp = objLabel.name ;
  memset(objLabel.name, 0x00, LAB_LEN);
  
  while((*objThreadCntrolBlock->prog != '\r') 
	  &&(*objThreadCntrolBlock->prog != '\n'))
  {
	  if(iswhite(*(objThreadCntrolBlock->prog)))
  		++objThreadCntrolBlock->prog;  /* skip over white space */
	  else
        *temp++=*(objThreadCntrolBlock->prog)++;
  }
  objLabel.type = OUTSIDE_FUNC ;
  
  if(objThreadCntrolBlock->iSubProgNum > NUM_SUBROUTINE)
  {
    	serror(objThreadCntrolBlock, 22);
		return;
  }
  if(!(objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum]
	    =(char *) malloc(PROG_SIZE))) {
	  FST_ERROR("allocation failure");
	  exit(1);
  }
  // memset(file_buffer, 0x00, 128);
  // sprintf(file_buffer, "%s.bas", objLabel.name);
  // objLabel.p = objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum] ;
  load_program(objThreadCntrolBlock, 
	  objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum], objLabel.name);
  // NOTICE: We should not call it , it should be loaded as the local variable.
  // append_program_prop_mapper(objThreadCntrolBlock, objLabel.name, false);
  // Scan the labels in the import files
  proglabelsScan = objThreadCntrolBlock->prog ;
  objThreadCntrolBlock->prog = objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum];
  scan_labels(objThreadCntrolBlock, OUTSIDE_FUNC, objLabel.name);
  objThreadCntrolBlock->prog = proglabelsScan;
  objThreadCntrolBlock->iSubProgNum++ ;
  
  mergeImportXPathToProjectXPath(objThreadCntrolBlock, objLabel.name);
/*
 *	  addr = add_label(objThreadCntrolBlock, objLabel);
 *    if(addr==-1 || addr==-2) {
 *        (addr==-1) ?serror(objThreadCntrolBlock, 5):serror(objThreadCntrolBlock, 6);
 *    }
 */
}


// 	int exec_call_submain(struct thread_control_block * objThreadCntrolBlock)
// 	{
// 		int iRet = 0 ;
// 		int lvartemp;
// 		char *loc;
// 		
// 		get_token(objThreadCntrolBlock);
// 		/* find the label to call */
// 		loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
// 		if(loc=='\0')
// 		{
// 			serror(objThreadCntrolBlock, 7); /* label not defined */
// 			return 1;
// 		}
// 		// Save local var stack index.
// 		lvartemp = objThreadCntrolBlock->local_var_stack.size();
// 		
// 		get_args(objThreadCntrolBlock); // get function arguments
// 		
// 		objThreadCntrolBlock->func_call_stack.push(lvartemp); // push local var index
// 		
// 		gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
// 		objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
// 		
// 		get_params(objThreadCntrolBlock); // load the function's parameters with
// 		
// 		FST_INFO("Execute call_interpreter at exec_call_submain.");
// 		return call_interpreter(objThreadCntrolBlock, 0);
// 		FST_INFO("Left   call_interpreter at exec_call_submain.");
// 	}

/************************************************* 
	Function:		exec_call
	Description:	Execute a CALL statement. 
	Input:			thread_control_block   - interpreter info
	        		isMacro       true     - Macro mode 
	        		              false    - Common mode 
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
int exec_call(struct thread_control_block * objThreadCntrolBlock, bool isMacro)
{
  char * fileNamePtr = NULL ;
  char func_name[256];
  char file_name[256];
  int lvartemp;
  char *loc;

  get_token(objThreadCntrolBlock);
  /* find the label to call */
  loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
  if(loc=='\0')
  {
	  memset(func_name, 0x00, 256);
	  sprintf(func_name, "%s::main", objThreadCntrolBlock->token);
	  loc = find_label(objThreadCntrolBlock, func_name);
	  if(loc=='\0')
	  {
		  serror(objThreadCntrolBlock, 7); /* label not defined */
		  return 0;
	  }
  }
  memset(file_name, 0x00, 256);
  sprintf(file_name, "%s", objThreadCntrolBlock->token);
  if(fileNamePtr = strchr(file_name, ':'))
  {
	 *fileNamePtr = '\0';
  }
  
#if 0
  if(isMacro)
  {
	if(g_macro_instr_mgr_ptr)
	{
		if (g_macro_instr_mgr_ptr->getRunningInMacroInstrList(
			objThreadCntrolBlock->token) == true)
		{
			serror(objThreadCntrolBlock, 19);
  			find_eol(objThreadCntrolBlock);
			return 1;
		}
	}
  }
#endif
  // Save local var stack index.
  lvartemp = objThreadCntrolBlock->local_var_stack.size();

  get_args(objThreadCntrolBlock); // get function arguments

  objThreadCntrolBlock->func_call_stack.push(lvartemp); // push local var index

  gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
  objThreadCntrolBlock->prog = loc;  /* start program running at that loc */

  get_params(objThreadCntrolBlock); // load the function's parameters with

  append_program_prop_mapper(objThreadCntrolBlock, file_name, false);
  FST_INFO("Execute call_interpreter at exec_call.");
  int iRet = call_interpreter(objThreadCntrolBlock, 0);
  // find_eol(objThreadCntrolBlock);
  FST_INFO("Left   call_interpreter at exec_call.");
  greturn(objThreadCntrolBlock);
  if(iRet == END_COMMND_RET)
	 return END_COMMND_RET;
  return 1;
}

/************************************************* 
	Function:		gosub
	Description:	Execute a GOSUB command. 
	Input:			thread_control_block   - interpreter info
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
int gosub(struct thread_control_block * objThreadCntrolBlock)
{
  char *loc;

  get_token(objThreadCntrolBlock);
  /* find the label to call */
  loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
  if(loc=='\0')
    serror(objThreadCntrolBlock, 7); /* label not defined */
  else {
    gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
    objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
	
	FST_INFO("Execute call_interpreter at gosub.");
    return call_interpreter(objThreadCntrolBlock, 0);
  }
  return 1 ;
}

/************************************************* 
	Function:		greturn
	Description:	Execute a RETURN command and Return from GOSUB. 
	Input:			thread_control_block   - interpreter info
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void greturn(struct thread_control_block * objThreadCntrolBlock)
{
  eval_value ret_value ;
  int   boolValue ;
  char* progTemp  ;
  progTemp = gosub_pop(objThreadCntrolBlock);
  if(objThreadCntrolBlock->func_call_stack.empty()) {
    // serror(objThreadCntrolBlock, 2);
	objThreadCntrolBlock->prog = progTemp ;
    return;
  }
  objThreadCntrolBlock->local_var_stack.resize(objThreadCntrolBlock->func_call_stack.top());
  objThreadCntrolBlock->func_call_stack.pop();

  
  get_token(objThreadCntrolBlock);
  // Not deal 'end sub'
  if(objThreadCntrolBlock->token_type != COMMAND)
  {
	  if( (objThreadCntrolBlock->token[0] == '\r')
  		||(objThreadCntrolBlock->token[0] == '\n'))
	  {
		putback(objThreadCntrolBlock);
		objThreadCntrolBlock->prog = progTemp ;
		// We should skip the whole line
		find_eol(objThreadCntrolBlock);
  		return ;
	  }
	  putback(objThreadCntrolBlock);
	  get_exp(objThreadCntrolBlock,&ret_value, &boolValue);
	  objThreadCntrolBlock->ret_value = ret_value.getFloatValue() ;
  }
  objThreadCntrolBlock->prog = progTemp ;
  // We should skip the whole line
  find_eol(objThreadCntrolBlock);
}

/************************************************* 
	Function:		gosub_push
	Description:	GOSUB stack push function. 
	Input:			thread_control_block   - interpreter info
	            	s                      - program position
	Ouput:			objThreadCntrolBlock->gosub_stack
	Return: 		NULL
*************************************************/ 
void gosub_push(struct thread_control_block * objThreadCntrolBlock, char *s)
{
  objThreadCntrolBlock->gosub_tos++;

  if(objThreadCntrolBlock->gosub_tos==SUB_NEST) {
    serror(objThreadCntrolBlock, 12);
    return;
  }

  objThreadCntrolBlock->gosub_stack[objThreadCntrolBlock->gosub_tos]=s;
}

/************************************************* 
	Function:		gosub_pop
	Description:	GOSUB stack pop function. 
	Input:			thread_control_block   - interpreter info
	Ouput:			NULL
	Return: 		Last program position
*************************************************/ 
char *gosub_pop(struct thread_control_block * objThreadCntrolBlock)
{
  if(objThreadCntrolBlock->gosub_tos==0) {
    serror(objThreadCntrolBlock, 13);
    return 0;
  }

  return(objThreadCntrolBlock->gosub_stack[objThreadCntrolBlock->gosub_tos--]);
}

/************************************************* 
	Function:		get_exp
	Description:	Expression: Caculating expression. 
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
/* Entry point into parser. */
void get_exp(struct thread_control_block * objThreadCntrolBlock, eval_value * result, int* boolValue)
{
  result->resetNoneValue();
  get_token(objThreadCntrolBlock);
  if(!objThreadCntrolBlock->token[0]) {
    serror(objThreadCntrolBlock, 2);
    return;
  }
  level1(objThreadCntrolBlock, result, boolValue);
  putback(objThreadCntrolBlock); /* return last token read to input stream */
}

/************************************************* 
	Function:		serror
	Description:	display and upload an error message
	Input:			thread_control_block   - interpreter info
	Input:			error                  - error number
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void serror(struct thread_control_block * objThreadCntrolBlock, int error)
{
  static const ErrInfo errInfo[]= {
		FAIL_INTERPRETER_SYNTAX_ERROR             ,     "syntax error",                // 0 
		FAIL_INTERPRETER_UNBALANCED_PARENTHESES   ,     "unbalanced parentheses",      // 1 
		FAIL_INTERPRETER_NO_EXPRESSION_PRESENT    ,     "no expression present",       // 2 
		FAIL_INTERPRETER_EQUALS_SIGN_EXPECTED     ,     "equals sign expected",        // 3 
		FAIL_INTERPRETER_NOT_A_VARIABLE           ,     "not a variable",              // 4 
		FAIL_INTERPRETER_LABEL_TABLE_FULL         ,     "Label table full",            // 5 
		FAIL_INTERPRETER_DUPLICATE_SUB_LABEL      ,     "duplicate sub_label",         // 6 
		FAIL_INTERPRETER_UNDEFINED_SUB_LABEL      ,     "undefined sub_label",         // 7 
		FAIL_INTERPRETER_THEN_EXPECTED            ,     "THEN expected",               // 8 
		FAIL_INTERPRETER_TO_EXPECTED              ,     "TO expected",                 // 9 
		FAIL_INTERPRETER_TOO_MANY_NESTED_FOR_LOOPS,     "too many nested FOR loops",   // 10 
		FAIL_INTERPRETER_NEXT_WITHOUT_FOR         ,     "NEXT without FOR",            // 11
		FAIL_INTERPRETER_TOO_MANY_NESTED_GOSUB    ,     "too many nested GOSUBs",      // 12 
		FAIL_INTERPRETER_RETURN_WITHOUT_GOSUB     ,     "RETURN without GOSUB",        // 13 
		FAIL_INTERPRETER_FILE_NOT_FOUND           ,     "file not found",              // 14
		FAIL_INTERPRETER_MOVL_WITH_JOINT          ,     "movl with joint",             // 15
		FAIL_INTERPRETER_MOVJ_WITH_POINT          ,     "movj with point",             // 16
		FAIL_INTERPRETER_ILLEGAL_LINE_NUMBER      ,     "illegal line number",         // 17
		FAIL_INTERPRETER_FUNC_PARAMS_MISMATCH     ,     "func params mismatching",     // 18
		FAIL_INTERPRETER_DUPLICATE_EXEC_MACRO     ,     "exec macro duplicating",      // 19 - used by setWarning
		INFO_INTERPRETER_BACK_TO_BEGIN            ,     "Backward to begin",           // 20 - used by setWarning
		INFO_INTERPRETER_THREAD_NOT_EXIST         ,     "thread not exist",            // 21 - used by setWarning
		INFO_INTERPRETER_TOO_MANY_IMPORT          ,     "too many import file",        // 22 
		INFO_INTERPRETER_TOO_LONG_PROJECT_NAME    ,     "too long project name",       // 23
		INFO_INTERPRETER_ARITHMETIC_EXCEPTION     ,     "Arithmetic Exception",        // 24
		INFO_INTERPRETER_UNKNOWN_ARITHM     	  , 	"Unknown Arithm"		       // 25
  };
  if(error > (int)(sizeof(errInfo)/sizeof(ErrInfo))) {
  	FST_ERROR("\t NOTICE : Error out of range %d ", error);
    return;
  }
  
  FST_INFO("-----------------ERR:%d----------------------", error);
#ifdef WIN32
  FST_INFO("\t NOTICE : %d -  0x%016I64x  - (%s)", error, errInfo[error].warn, errInfo[error].desc);
#else
  FST_INFO("\t NOTICE : %d -  %llx(%s)", error, errInfo[error].warn, errInfo[error].desc);
#endif
  FST_INFO("-----------------ERR:%d----------------------", error);
  
  setWarning(errInfo[error].warn) ; 
  objThreadCntrolBlock->prog_mode = ERROR_MODE;
  
//  longjmp(e_buf, 1); /* return to save point */
#if 0
	  // iIdx = g_thread_control_block[0].iThreadIdx ;
	  int iIdx = objThreadCntrolBlock->iThreadIdx ;
	  
#ifdef WIN32
	  ExitThread
	  CloseHandle( g_basic_interpreter_handle[iIdx] );  
	  g_basic_interpreter_handle[iIdx] = NULL; 
#else
	  FST_INFO("Enter pthread_join.");
	  pthread_join(g_basic_interpreter_handle[iIdx], NULL);
	  FST_INFO("Left  pthread_join.");
	  fflush(stdout);
	  g_basic_interpreter_handle[iIdx] = 0;
	  pthread_exit(g_basic_interpreter_handle[iIdx]);
#endif // WIN32
#endif
}

/************************************************* 
	Function:		get_token
	Description:	Tokenizer: Get a token.
	Input:			thread_control_block   - interpreter info
	Ouput:			objThreadCntrolBlock->token_type, tok, token
	Return: 		NULL
*************************************************/ 
int get_token(struct thread_control_block * objThreadCntrolBlock)
{

  register char *temp;

  objThreadCntrolBlock->token_type=0; objThreadCntrolBlock->tok=0;
  temp=objThreadCntrolBlock->token;

  if(*(objThreadCntrolBlock->prog)=='\0') { /* end of file */
    *(objThreadCntrolBlock->token)=0;
    objThreadCntrolBlock->tok = FINISHED;
    return(objThreadCntrolBlock->token_type=DELIMITER);
  }

  while(iswhite(*(objThreadCntrolBlock->prog)))
  		++objThreadCntrolBlock->prog;  /* skip over white space */

  if(*(objThreadCntrolBlock->prog)=='\r') { /* crlf */
    ++objThreadCntrolBlock->prog; ++objThreadCntrolBlock->prog;
    objThreadCntrolBlock->tok = EOL; *(objThreadCntrolBlock->token)='\r';
    objThreadCntrolBlock->token[1]='\n'; objThreadCntrolBlock->token[2]=0;
    return (objThreadCntrolBlock->token_type = DELIMITER);
  }
  
  if(*(objThreadCntrolBlock->prog)=='\n') { /* crlf */
	  ++objThreadCntrolBlock->prog; 
	  objThreadCntrolBlock->tok = EOL; *(objThreadCntrolBlock->token)='\r';
	  objThreadCntrolBlock->token[1]='\n'; objThreadCntrolBlock->token[2]=0;
	  return (objThreadCntrolBlock->token_type = DELIMITER);
  }

  if(*(objThreadCntrolBlock->prog)=='#') {
  	find_eol(objThreadCntrolBlock);
  	objThreadCntrolBlock->token[0]='\r';
	objThreadCntrolBlock->token[1]='\n';
	objThreadCntrolBlock->token[2]=0;
    // return (objThreadCntrolBlock->token_type = DELIMITER);
	return (objThreadCntrolBlock->token_type = COMMENT);
  }
  // Add @ as DIV operation
  if(strchr("+-*^/%@=;(),><[].", *objThreadCntrolBlock->prog)){ /* delimiter */
    // *temp=*objThreadCntrolBlock->prog;
  	if(strchr("<>=", *objThreadCntrolBlock->prog)) {
		switch(*objThreadCntrolBlock->prog) {
			case '<':
			    if(*(objThreadCntrolBlock->prog+1) == '=') {
			        objThreadCntrolBlock->prog++;
					objThreadCntrolBlock->prog++;   /* advance to next position */
			        *temp = LE; temp++; *temp = LE;
			    }
			    else if(*(objThreadCntrolBlock->prog+1) == '>') {
			        objThreadCntrolBlock->prog++;
					objThreadCntrolBlock->prog++;   /* advance to next position */
			        *temp = NE; temp++; *temp = NE;
			    }
			    else {
			        objThreadCntrolBlock->prog++;   /* advance to next position */
			        *temp = LT;
			    }
			    break;
			case '>':
			    if(*(objThreadCntrolBlock->prog+1) == '=') {
			        objThreadCntrolBlock->prog++;
				    objThreadCntrolBlock->prog++;
			        *temp = GE; temp++; *temp = GE;
			    }
			    else {
			        objThreadCntrolBlock->prog++;
			        *temp = GT;
			    }
			    break;
			case '=':
				if(*(objThreadCntrolBlock->prog+1) == '=') {
					objThreadCntrolBlock->prog++;
					objThreadCntrolBlock->prog++;
					*temp = EQ; temp++; *temp = EQ;
				}
				else {
					objThreadCntrolBlock->prog++;
					*temp = EQ;
			    }
			    break;
		}
  	}
	else
	{
        *temp=*objThreadCntrolBlock->prog;
        objThreadCntrolBlock->prog++; /* advance to next position */
	}
    temp++;
    *temp=0;
    return (objThreadCntrolBlock->token_type=DELIMITER);
  }

  if(*(objThreadCntrolBlock->prog)=='"') { /* quoted string */
    objThreadCntrolBlock->prog++;
    while(*(objThreadCntrolBlock->prog) != '"'
		&& *objThreadCntrolBlock->prog != '\r'
		&& *objThreadCntrolBlock->prog != '\n') 
		*temp++=*(objThreadCntrolBlock->prog)++;
    if(*(objThreadCntrolBlock->prog)=='\r') 
		serror(objThreadCntrolBlock, 1);
    objThreadCntrolBlock->prog++;*temp=0;
    return(objThreadCntrolBlock->token_type=QUOTE);
  }

  if(isdigit(*(objThreadCntrolBlock->prog))) { /* number */
    while(!isdelim(*(objThreadCntrolBlock->prog))) *temp++=*(objThreadCntrolBlock->prog)++;
    *temp = '\0';
    return(objThreadCntrolBlock->token_type = NUMBER);
  }

  if(isalpha(*(objThreadCntrolBlock->prog))) { /* var or command */
    while(!isdelim(*(objThreadCntrolBlock->prog))) 
		*temp++=*(objThreadCntrolBlock->prog)++;
    objThreadCntrolBlock->token_type=STRING;
  }

  *temp = '\0';

  /* see if a string is a command or a variable */
  if(objThreadCntrolBlock->token_type==STRING) {
    objThreadCntrolBlock->tok=look_up(objThreadCntrolBlock->token); /* convert to internal rep */
    if(objThreadCntrolBlock->tok)
    {
		objThreadCntrolBlock->token_type = COMMAND; /* is a command */
    }
    else if(find_internal_cmd(objThreadCntrolBlock->token) != -1)
    {
	   objThreadCntrolBlock->token_type = INNERCMD;
    }
    else if(find_internal_func(objThreadCntrolBlock->token) != -1)
    {
	   objThreadCntrolBlock->token_type = BUILTINFUNC;
    }
    else if(find_label(objThreadCntrolBlock, objThreadCntrolBlock->token) != '\0')
    {
        SubLabelType labelType = find_label_by_type(
			objThreadCntrolBlock, objThreadCntrolBlock->token);
    	if(labelType == INSIDE_FUNC)
	    	objThreadCntrolBlock->token_type = INSIDEFUNC;
		else if(labelType == OUTSIDE_FUNC)
	    	objThreadCntrolBlock->token_type = OUTSIDEFUNC;
    }
	// Support AND
    else if(!strcmp(objThreadCntrolBlock->token, STR_AND))
    {
       memset(objThreadCntrolBlock->token, 0x00, 80);
	   objThreadCntrolBlock->token[0] = AND;
	   objThreadCntrolBlock->token[1] = '\0';
	   objThreadCntrolBlock->token_type = DELIMITER;
    }
	// Support OR
	else if(!strcmp(objThreadCntrolBlock->token, STR_OR))
    {
       memset(objThreadCntrolBlock->token, 0x00, 80);
	   objThreadCntrolBlock->token[0] = OR;
	   objThreadCntrolBlock->token[1] = '\0';
	   objThreadCntrolBlock->token_type = DELIMITER;
    }
	else
	{
	   objThreadCntrolBlock->token_type = VARIABLE;
	}
  }
  return objThreadCntrolBlock->token_type;
}

/************************************************* 
	Function:		putback
	Description:	Tokenizer: Return a token to input stream..
	Input:			thread_control_block   - interpreter info
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void putback(struct thread_control_block * objThreadCntrolBlock)
{

  char *t;

  t = objThreadCntrolBlock->token;
  for(; *t; t++) objThreadCntrolBlock->prog--;
  if(objThreadCntrolBlock->token_type == QUOTE)
  {
	  objThreadCntrolBlock->prog--; // put back left "
	  objThreadCntrolBlock->prog--; // put back right "
  }
  memset(objThreadCntrolBlock->token, 0x00, 80);
}

/************************************************* 
	Function:		look_up
	Description:	Look up a a token's internal representation in the token table.
	Input:			token
	Return: 		token type
*************************************************/ 
int look_up(char *s)
{
  register int i; // ,j;
  char *p;

  /* convert to lowercase */
  p = s;
  while(*p){ *p = tolower(*p); p++; }

  /* see if token is in table */
  for(i=0; *table[i].command; i++)
      if(!strcmp(table[i].command, s)) return table[i].tok;
  return 0; /* unknown command */
}

/************************************************* 
	Function:		isdelim_with_array
	Description:	Return true if c is a delimiter or '[]'.
	Input:			char c
	Return: 		1 - isdelim ; 0 - notdelim
*************************************************/ 
int isdelim_with_array(char c)
{
  if(strchr(" ;,+-<>/*%^=()", c) || c==9 || c=='\r' || c=='\n' || c==0)
    return 1;
  return 0;
}

/************************************************* 
	Function:		isdelim
	Description:	Return true if c is a delimiter.
	Input:			char c
	Return: 		1 - isdelim ; 0 - notdelim
*************************************************/ 
int isdelim(char c)
{
  if(strchr(" ;,+-<>/*%^=()[]", c) || c==9 || c=='\r' || c=='\n' || c==0)
    return 1;
  return 0;
}

/************************************************* 
	Function:		iswhite
	Description:	Return 1 if c is space or tab.
	Input:			char c
	Return: 		1 - space or tab ; 0 - not space or tab
*************************************************/ 
int iswhite(char c)
{
  if(c==' ' || c=='\t') return 1;
  else return 0;
}


/************************************************* 
	Function:		level1
	Description:	Expression: Process relational operators(AND and OR).
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level1(struct thread_control_block * objThreadCntrolBlock, eval_value *value, int* boolValue)
{
    eval_value partial_value;
    int   another_bool_value = 1; // true
    char op;
    char relops[] = {
        AND, OR, 0
    };

    level2(objThreadCntrolBlock, value, boolValue);

    op = *(objThreadCntrolBlock->token);
    if(strchr(relops, op) && op != 0) {
        get_token(objThreadCntrolBlock);
        level2(objThreadCntrolBlock, &partial_value, &another_bool_value);

        switch(op) { // perform the relational operation
			case AND:
			    *boolValue = (*boolValue && another_bool_value);
			    break;
			case OR:
			    *boolValue = (*boolValue || another_bool_value);
			    break;
        }
    }
}

/************************************************* 
	Function:		level2
	Description:	Expression: Process relational operators(LT, LE, GT, GE, EQ, NE).
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level2(struct thread_control_block * objThreadCntrolBlock, eval_value *value, int* boolValue)
{
    eval_value partial_value;
    char op;
    char relops[] = {
        LT, LE, GT, GE, EQ, NE, 0
    };

    level3(objThreadCntrolBlock, value, boolValue);

    op = *(objThreadCntrolBlock->token);
    if(strchr(relops, op) && op != 0) {
        get_token(objThreadCntrolBlock);
        level3(objThreadCntrolBlock, &partial_value, boolValue);

        switch(op) { // perform the relational operation
			case LT:
			    *boolValue = (value->getFloatValue() < partial_value.getFloatValue());
			    break;
			case LE:
			    *boolValue = (value->getFloatValue() <= partial_value.getFloatValue());
			    break;
			case GT:
			    *boolValue = (value->getFloatValue() > partial_value.getFloatValue());
			    break;
			case GE:
			    *boolValue = (value->getFloatValue() >= partial_value.getFloatValue());
			    break;
			case EQ:
			    *boolValue = (value->getFloatValue() == partial_value.getFloatValue());
			    break;
			case NE:
			    *boolValue = (value->getFloatValue() != partial_value.getFloatValue());
			    break;
        }
    }
}

/************************************************* 
	Function:		level3
	Description:	Expression: Add or subtract two terms.
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level3(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue)
{
  register char  op;
  eval_value hold;

  level4(objThreadCntrolBlock, result, boolValue);
  while((op = *(objThreadCntrolBlock->token)) == '+' || op == '-') {
    get_token(objThreadCntrolBlock);
    level4(objThreadCntrolBlock, &hold, boolValue);
    arith(objThreadCntrolBlock, op, result, &hold);
  }
}

/************************************************* 
	Function:		level4
	Description:	Expression: Multiply or divide two factors.
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level4(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue)
{
  register char  op;
  eval_value hold;

  level5(objThreadCntrolBlock, result, boolValue);
  while((op = *(objThreadCntrolBlock->token))
  	== '*' || op == '/' || op == '%'|| op == '@') {
    get_token(objThreadCntrolBlock);
    level5(objThreadCntrolBlock, &hold, boolValue);
    arith(objThreadCntrolBlock, op, result, &hold);
  }
}

/************************************************* 
	Function:		level5
	Description:	Expression: Process integer exponent.
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level5(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue)
{
  eval_value hold;

  level6(objThreadCntrolBlock, result, boolValue);
  if(*(objThreadCntrolBlock->token)== '^') {
    get_token(objThreadCntrolBlock);
    level5(objThreadCntrolBlock, &hold, boolValue);
    arith(objThreadCntrolBlock, '^', result, &hold);
  }
}

/************************************************* 
	Function:		level6
	Description:	Expression: Process unary + or -.
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level6(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue)
{
  register char  op;

  op = 0;
  if((objThreadCntrolBlock->token_type==DELIMITER) &&
  	(*(objThreadCntrolBlock->token)=='+'
  	|| *(objThreadCntrolBlock->token)=='-')) {
    op = *(objThreadCntrolBlock->token);
    get_token(objThreadCntrolBlock);
  }
  level7(objThreadCntrolBlock, result, boolValue);
  if(op)
    unary(op, result);
}

/************************************************* 
	Function:		level7
	Description:	Expression: Process parenthesized expression.
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Input:			boolValue              - Caculating boolean result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void level7(struct thread_control_block * objThreadCntrolBlock, eval_value *result, int* boolValue)
{
  if((*(objThreadCntrolBlock->token) == '(')
  	&& (objThreadCntrolBlock->token_type == DELIMITER)) {
    get_token(objThreadCntrolBlock);
    level1(objThreadCntrolBlock, result, boolValue);
    if(*(objThreadCntrolBlock->token) != ')')
      serror(objThreadCntrolBlock, 1);
    get_token(objThreadCntrolBlock);
  }
  else
    primitive(objThreadCntrolBlock, result);
}

/************************************************* 
	Function:		primitive
	Description:	Expression: Find value of number or variable.
	Input:			thread_control_block   - interpreter info
	Input:			result                 - Caculating result
	Ouput:			NULL
	Return: 		NULL
*************************************************/ 
void primitive(struct thread_control_block * objThreadCntrolBlock, eval_value *result)
{
  std::string strValue ;
  char var[80];
  int iRet = 0 ;
  // char *progFuncCall; 
  switch(objThreadCntrolBlock->token_type) {
  case INNERCMD:
    // Timer
	if(strcmp(objThreadCntrolBlock->token, "timer") != 0)
    {
		result->setFloatValue(1.0);
		get_token(objThreadCntrolBlock);
		return;
	}
	// if objThreadCntrolBlock->token == "timer",take it as the VARIABLE
  case VARIABLE:
	objThreadCntrolBlock->g_variable_error = 0 ;
    *result = find_var(objThreadCntrolBlock, objThreadCntrolBlock->token);
	if(objThreadCntrolBlock->g_variable_error == 0)
       get_token(objThreadCntrolBlock);
	else
	{
		putback(objThreadCntrolBlock);
		deal_array_element(objThreadCntrolBlock);
		strcpy(var, objThreadCntrolBlock->token);

		get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->token[0] != ']')
		{
			serror(objThreadCntrolBlock, 0);
		}

		get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->token[0] == '.') {
			// var = toupper(*token)-'A';
			get_token(objThreadCntrolBlock);
			sprintf(var, "%s.%s", var, objThreadCntrolBlock->token);
		}
		else
		{
			putback(objThreadCntrolBlock);
		}

		*result = find_var(objThreadCntrolBlock, var, 1);
		get_token(objThreadCntrolBlock);
		// No deal unexist array type element
		// if(objThreadCntrolBlock->g_variable_error == 0)
	}
	objThreadCntrolBlock->g_variable_error = 0 ;
    return;
  case NUMBER:
    result->setFloatValue(atof(objThreadCntrolBlock->token));
    get_token(objThreadCntrolBlock);
    return;
  case INSIDEFUNC:
    // progFuncCall = objThreadCntrolBlock->prog ;
  	putback(objThreadCntrolBlock);
	// exec_call(objThreadCntrolBlock);
	iRet = exec_call(objThreadCntrolBlock);
	if(iRet == END_COMMND_RET)
	   result->setFloatValue(0) ;
	else
	   result->setFloatValue(objThreadCntrolBlock->ret_value);
    // objThreadCntrolBlock->prog = progFuncCall;
	// find_eol(objThreadCntrolBlock);
    get_token(objThreadCntrolBlock);
    return;
  case BUILTINFUNC:
  	// use objThreadCntrolBlock->token
  	call_inner_func(objThreadCntrolBlock, result);
    get_token(objThreadCntrolBlock);
    return;
  case QUOTE:
  	strValue = std::string(objThreadCntrolBlock->token);
	result->setStringValue(strValue);
    get_token(objThreadCntrolBlock);
    return;
  default:
	FST_ERROR("primitive error :: get_token =  '%s'", objThreadCntrolBlock->token);
    serror(objThreadCntrolBlock, 0);
  }
}

/************************************************* 
	Function:		arith
	Description:	Expression: Perform the specified arithmetic. 
	Input:			o                 - arithmetic operator
	Input:			r                 - operand1
	Input:			h                 - operand2
	Ouput:			r                 - operand1
	Return: 		NULL
*************************************************/ 
void arith(struct thread_control_block * objThreadCntrolBlock, char o, eval_value *r, eval_value *h)
{
//  register int t, ex;

  switch(o) {
    case '-':
      // r->fValue = r->fValue - h->fValue;
      r->calcSubtract(h);
      break;
    case '+':
      // r->fValue = r->fValue + h->fValue;
      r->calcAdd(h);
      break;
    case '*':
      // r->fValue = r->fValue * h->fValue;
      r->calcMultiply(h);
      break;
    case '/':
      // r->fValue = (r->fValue)/(h->fValue);
	  if(h->getFloatValue() == 0.0)
	  {
		 serror(objThreadCntrolBlock, 24);
	  }
      r->calcDivide(h);
      break;
    case '%':
      // t = (r->fValue)/(h->fValue);
      // r->fValue = r->fValue-(t*(h->fValue));
	  if(h->getFloatValue() == 0.0)
	  {
		 serror(objThreadCntrolBlock, 24);
	  }
      r->calcMod(h);
      break;
    case '^':
      // ex = r->fValue;
      // if(h->fValue==0) {
      //   r->fValue = 1;
      //   break;
      // }
      // for(t=h->fValue-1; t>0; --t) 
	  //    r->fValue = (r->fValue) * ex;
      r->calcPower(h);
      break;
    case '@':
	  if(h->getFloatValue() == 0.0)
	  {
		 serror(objThreadCntrolBlock, 24);
	  }
	  r->calcDIVToInt(h);
      break;
	default:
	  serror(objThreadCntrolBlock, 25);
      break;
  }
}

/************************************************* 
	Function:		unary
	Description:	Expression: Reverse the sign.
	Input:			o                 - arithmetic operator
	Input:			r                 - operand1
	Ouput:			r                 - operand1
	Return: 		NULL
*************************************************/ 
void unary(char o, eval_value *r)
{
  if(o=='-') r->calcUnary(); // r->fValue = -(r->fValue);
}

static int get_char_token(char * src, char * dst)
{
	char * tmp = src ;
	if(isalpha(*src)) { /* var or command */
		while(!isdelim(*(src))) 
			*dst++=*(src)++;
	}
	return tmp - src ;
}


void set_var_value(struct thread_control_block * objThreadCntrolBlock, 
				   char *dst_reg_name, eval_value& valueDst, eval_value& valueSrc)
{
	FST_INFO("set_var_value = %s with %04X and %04X", dst_reg_name, valueSrc.getType(), valueDst.getType());
	if(strcmp(dst_reg_name, "p") == 0) // lvalue is P register
	{
		if (valueSrc.getType() == TYPE_PR)
		{
			if (valueDst.getType() == TYPE_POSE)
			{
				PoseEuler pointEulerVal ;
#ifdef WIN32
				pointEulerVal.position = valueSrc.getPrRegDataValue().value.cartesian_pos.position ;
				pointEulerVal.orientation = valueSrc.getPrRegDataValue().value.cartesian_pos.orientation;
#else
				pointEulerVal.position.x    = valueSrc.getPrRegDataValue().value.pos[0];
				pointEulerVal.position.y    = valueSrc.getPrRegDataValue().value.pos[1];
				pointEulerVal.position.z    = valueSrc.getPrRegDataValue().value.pos[2];
				pointEulerVal.orientation.a = valueSrc.getPrRegDataValue().value.pos[3];
				pointEulerVal.orientation.b = valueSrc.getPrRegDataValue().value.pos[4];
				pointEulerVal.orientation.c = valueSrc.getPrRegDataValue().value.pos[5];
				
				FST_INFO("set_var_value: id = (%f, %f, %f, %f, %f, %f) ", 
					valueSrc.getPrRegDataValue().value.pos[0], valueSrc.getPrRegDataValue().value.pos[1], 
					valueSrc.getPrRegDataValue().value.pos[2], valueSrc.getPrRegDataValue().value.pos[3], 
					valueSrc.getPrRegDataValue().value.pos[4], valueSrc.getPrRegDataValue().value.pos[5]);
#endif
				//	vt.value = value;
				valueDst.setPoseValue(&pointEulerVal);
			}
			else if (valueDst.getType() == TYPE_JOINT)
			{
				Joint jointVal ;
#ifdef WIN32
				jointVal.j1 = valueSrc.getPrRegDataValue().value.joint_pos[0];
				jointVal.j2 = valueSrc.getPrRegDataValue().value.joint_pos[1];
				jointVal.j3 = valueSrc.getPrRegDataValue().value.joint_pos[2];
				jointVal.j4 = valueSrc.getPrRegDataValue().value.joint_pos[3];
				jointVal.j5 = valueSrc.getPrRegDataValue().value.joint_pos[4];
				jointVal.j6 = valueSrc.getPrRegDataValue().value.joint_pos[5];
#else
				jointVal.j1 = valueSrc.getPrRegDataValue().value.pos[0];
				jointVal.j2 = valueSrc.getPrRegDataValue().value.pos[1];
				jointVal.j3 = valueSrc.getPrRegDataValue().value.pos[2];
				jointVal.j4 = valueSrc.getPrRegDataValue().value.pos[3];
				jointVal.j5 = valueSrc.getPrRegDataValue().value.pos[4];
				jointVal.j6 = valueSrc.getPrRegDataValue().value.pos[5];
#endif
				//	vt.value = value;
				valueDst.setJointValue(&jointVal);
			}
		}
		else if (valueSrc.getType() == TYPE_POSE)
		{
			valueDst.setPoseValue(&valueSrc.getPoseValue());
		}
		else if (valueSrc.getType() == TYPE_JOINT)
		{
			valueDst.setJointValue(&valueSrc.getJointValue());
		}
	}
	else
	{
		FST_ERROR("Not support variable...");
	}
}

void assign_global_var(struct thread_control_block * objThreadCntrolBlock, char *vname, eval_value value)
{
	var_type vt;
	// Otherwise, try global vars.
	for(unsigned i=0; i < objThreadCntrolBlock->global_vars.size(); i++)
	{
	    if(!strcmp(objThreadCntrolBlock->global_vars[i].var_name, vname)) {
	        objThreadCntrolBlock->global_vars[i].value = value;
	        return;
	    }
	}
	memset(vt.var_name, 0x00, LAB_LEN);
	strcpy(vt.var_name, vname);
	vt.value = value;
	objThreadCntrolBlock->global_vars.push_back(vt);
}

/************************************************* 
	Function:		assign_var
	Description:	Declare a global variable.
	Input:			thread_control_block   - interpreter info
	Input:			vname           - variable name
	Input:			value           - variable value
	Return: 		NULL
*************************************************/ 
void assign_var(struct thread_control_block * objThreadCntrolBlock, char *vname, eval_value value)
{
    int iLineNum = 0 ;
	char reg_name[256] ;
	char *temp = NULL ;
	
	memset(reg_name, 0x00, 256);
	temp = reg_name ;
	get_char_token(vname, temp);
	// deal "pr;sr;r;mr;uf;tf;pl" except p
    if(strstr(REGSITER_NAMES, reg_name) && (strcmp(reg_name, "p") != 0))
    {
		if(strchr(vname, '['))
		{
			if((value.getType() & TYPE_FLOAT) == TYPE_FLOAT)
			{
				FST_INFO("assign_var vname = %s and value = %f.", vname, value.getFloatValue());
			}
			if ((value.getType() & TYPE_STRING) == TYPE_STRING)
			{
				string strTmp = value.getStringValue() ;
				FST_INFO("assign_var vname = %s and value = (%s).", vname, strTmp.c_str());
			}
#ifdef USE_FORSIGHT_REGISTERS_MANAGER
			int iRet = forgesight_registers_manager_set_register(
				objThreadCntrolBlock, vname, &value);
#else
            int iRet = forgesight_set_register(
				objThreadCntrolBlock, vname, &value);
#endif
			if(iRet == 0)
			{
				return ;
			}
		}
    }
	else if(strstr(IO_NAMES, reg_name))
    {
		if(strchr(vname, '['))
		{
			int iRet = 0 ;
			iRet = forgesight_set_io_status(vname, value);
			if(iRet == 0)
			{
				return ;
			}
		}
    }
	else if(strcmp(FORSIGHT_TIMER, reg_name) == 0)
    {
		if(strchr(vname, '['))
		{
			int iRet = 0 ;
			iRet = execute_Timer(objThreadCntrolBlock, vname, value);
			if(iRet != 1)
			{
				FST_INFO("forgesight_set_timer Failed");
			}
			return ;
		}
    }

	if(strcmp(FORSIGHT_TF_NO, vname) == 0)
    {
        FST_INFO("set_global_TF vname = %s and value = %f.", vname, value.getFloatValue());
		iLineNum = calc_line_from_prog(objThreadCntrolBlock);
		set_global_TF(iLineNum, (int)value.getFloatValue(), objThreadCntrolBlock);
		return ;
    }
	else if(strcmp(FORSIGHT_UF_NO, vname) == 0)
    {
        FST_INFO("set_global_UF vname = %s and value = %f.", vname, value.getFloatValue());
		iLineNum = calc_line_from_prog(objThreadCntrolBlock);
		set_global_UF(iLineNum, (int)value.getFloatValue(), objThreadCntrolBlock);
		return ;
    }
	else if(strcmp(FORSIGHT_OVC, vname) == 0)
    {
        FST_INFO("set_OVC vname = %s and value = %f.", vname, value.getFloatValue());
		iLineNum = calc_line_from_prog(objThreadCntrolBlock);
		set_OVC(iLineNum, value.getFloatValue(), objThreadCntrolBlock);
		return ;
    }
	else if(strcmp(FORSIGHT_OAC, vname) == 0)
    {
        FST_INFO("set_OAC vname = %s and value = %f.", vname, value.getFloatValue());
		iLineNum = calc_line_from_prog(objThreadCntrolBlock);
		set_OAC(iLineNum, value.getFloatValue(), objThreadCntrolBlock);
		return ;
    }
	
	// P register was saved in the local_var_stack
	if(strcmp(reg_name, "p") == 0) // lvalue is P register
	{
	    // Otherwise, try global vars.
	    for(unsigned i=0; i < objThreadCntrolBlock->local_var_stack.size(); i++)
	    {
	        if(!strcmp(objThreadCntrolBlock->local_var_stack[i].var_name, vname)) {
				set_var_value(objThreadCntrolBlock, reg_name, 
					objThreadCntrolBlock->local_var_stack[i].value, value);
	            return;
	        }
		}
		FST_ERROR("The %s does not exist", vname);
	}
	else
	{
		assign_global_var(objThreadCntrolBlock, vname, value);
	}
}

/************************************************* 
	Function:		find_var
	Description:	Reverse Find the value of a variable. 
	Input:			thread_control_block   - interpreter info
	Input:			vname           - variable name
	Input:			raise_unkown_error   1 - serror with not-existed variable
	                                     0 - Omit with not-existed variable
	Return: 		value           - variable value
*************************************************/ 
eval_value find_var(struct thread_control_block * objThreadCntrolBlock, 
					char *vname, int raise_unkown_error)
{
	MoveCommandDestination movCmdDst ;
	eval_value value ;
	int        boolPulseValue;
	char reg_name[256] ;
	char *temp = NULL ;

    vector<var_type>::reverse_iterator it ;
	  objThreadCntrolBlock->g_variable_error = 0 ;
//	  if(!isalpha(*s)){
//	    serror(objThreadCntrolBlock, 4); /* not a variable */
//	    return 0;
//	  }
//	  return variables[toupper(*s)-'A'];

	if(!strcmp(vname, FORSIGHT_RETURN_VALUE))
	{
		value.setFloatValue(objThreadCntrolBlock->ret_value);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_REGISTER_ON))
	{
		value.setFloatValue(1.0);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_REGISTER_OFF))
	{
		value.setFloatValue(0.0);
		return value ;
	}

	if(!strcmp(vname, FORSIGHT_CURRENT_JOINT))
	{
		copyMoveCommandDestination(movCmdDst);
	    value.setJointValue(&movCmdDst.joint_target);
		
		value.setPrRegDataWithJointValue(&movCmdDst.joint_target);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_CURRENT_POS))
	{
		copyMoveCommandDestination(movCmdDst);
		value.setPoseValue(&movCmdDst.pose_target);
		
		value.setPrRegDataWithPoseEulerValue(&movCmdDst.pose_target);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_TIMER_START))
	{
		value.setFloatValue(TIMER_START_VALUE);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_TIMER_STOP))
	{
		value.setFloatValue(TIMER_STOP_VALUE);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_TIMER_RESET))
	{
		value.setFloatValue(TIMER_RESET_VALUE);
		return value ;
	}
	else if(!strcmp(vname, FORSIGHT_PULSE))
	{
		get_exp(objThreadCntrolBlock, &value, &boolPulseValue);
		value.setPulse(true);
		return value ;
	}
	
	memset(reg_name, 0x00, 256);
	temp = reg_name ;
	get_char_token(vname, temp);
	// deal "pr;sr;r;mr;uf;tf;pl" except p
    if(strstr(REGSITER_NAMES, reg_name) && (strcmp(reg_name, "p") != 0))
    {
		if(strchr(vname, '['))
		{
			FST_INFO("find_var vname = %s .", vname);
#ifdef USE_FORSIGHT_REGISTERS_MANAGER
    		int iRet = forgesight_registers_manager_get_register(
				objThreadCntrolBlock, vname, &value);
#else
    		int iRet = forgesight_get_register(
				objThreadCntrolBlock, vname, &value);
#endif
			if(iRet == 0)
			{
				return value ;
			}
		}
    }
	else if(strstr(IO_NAMES, reg_name))
    {
		if(strchr(vname, '['))
		{
			// int iValue = -1;
			value = forgesight_get_io_status(vname);
			return value;
		}
	}
	
	// First, try local vars.
	for(it
		= objThreadCntrolBlock->local_var_stack.rbegin();
		it != objThreadCntrolBlock->local_var_stack.rend(); ++it)
	{
	    if(!strcmp(it->var_name, vname))  {
	        return it->value;
        }
	}

	// Otherwise, try global vars.
	for(it
		= objThreadCntrolBlock->global_vars.rbegin();
		it != objThreadCntrolBlock->global_vars.rend(); ++it)
	{
	    if(!strcmp(it->var_name, vname))  {
	        return it->value;
        }
	}

	if (raise_unkown_error == 1)
	{
		FST_ERROR("not defined variable (%s).", vname);
		serror(objThreadCntrolBlock, 4);
	}
	objThreadCntrolBlock->g_variable_error = 1 ;
	return value ;
}

/************************************************* 
	Function:		erase_var
	Description:	Reverse erase the value of a variable.
	Input:			thread_control_block   - interpreter info
	Input:			vname           - variable name
	Return: 		value           - variable value
*************************************************/ 
int erase_var(struct thread_control_block * objThreadCntrolBlock, char *vname)
{
        vector<var_type>::iterator it ;
//	  if(!isalpha(*s)){
//	    serror(objThreadCntrolBlock, 4); /* not a variable */
//	    return 0;
//	  }
//	  return variables[toupper(*s)-'A'];

	for(it
		= objThreadCntrolBlock->local_var_stack.begin();
		it != objThreadCntrolBlock->local_var_stack.end(); ++it)
	{
	    if(!strcmp(it->var_name, vname))  {
			objThreadCntrolBlock->local_var_stack.erase(it);
	        return 1;
        }
	}

	// Otherwise, try global vars.
	for(it
		= objThreadCntrolBlock->global_vars.begin();
		it != objThreadCntrolBlock->global_vars.end(); ++it)
	{
	    if(!strcmp(it->var_name, vname))  {
			objThreadCntrolBlock->global_vars.erase(it);
	        return 1;
        }
	}
	FST_ERROR("not defined variable.");
	return -1 ;
}


/************************************************* 
	Function:		basic_thread_create
	Description:	Start thread of interpreter.
	Input:			iIdx            - Thread iIdx
	Input:			args            - interpreter info
	Return: 		NULL
*************************************************/ 
bool basic_thread_create(int iIdx, void * args)
{
	bool ret = false;
#ifdef WIN32
	g_basic_interpreter_handle[iIdx] =
		(HANDLE)_beginthreadex(NULL, 0, basic_interpreter, args, 0, NULL);
	if (NULL != g_basic_interpreter_handle[iIdx])
	{
		ret = true;
	}
#else
	if (0 == pthread_create(
		&(g_basic_interpreter_handle[iIdx]), NULL, basic_interpreter, args))
	{
		ret = true;
	}
	else
	{
      FST_ERROR("start basic_thread_create Failed..");
		g_basic_interpreter_handle[iIdx] = 0;
	}
#endif
	return ret;
}

/************************************************* 
	Function:		basic_thread_destroy
	Description:	Destroy thread of interpreter.
	Input:			iIdx            - Thread iIdx
	Return: 		NULL
*************************************************/ 
void basic_thread_destroy(int iIdx)
{
#ifdef WIN32
	WaitForSingleObject(g_basic_interpreter_handle[iIdx], INFINITE);
	if (NULL != g_basic_interpreter_handle[iIdx])
	{
		CloseHandle(g_basic_interpreter_handle[iIdx]);
	}
	g_basic_interpreter_handle[iIdx] = NULL;
#else
	pthread_join(g_basic_interpreter_handle[iIdx], NULL);
	g_basic_interpreter_handle[iIdx] = 0;
#endif // WIN32
}


