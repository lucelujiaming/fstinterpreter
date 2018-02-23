// Forsight_BASINT.cpp : Defines the entry point for the console application.
//
/* A tiny BASIC interpreter */

#include "forsight_basint.h"
#include "forsight_innercmd.h"
#include "forsight_innerfunc.h"
#include "forsight_inter_control.h"
#include "forsight_auto_lock.h"
#include "forsight_xml_reader.h"

// #define NUM_LAB 100

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

enum var_inner_type { FORSIGHT_CHAR, FORSIGHT_INT, FORSIGHT_FLOAT };

#define FORSIGHT_RETURN_VALUE   "forsight_return_value_"
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
  "", END  /* mark end of table */
};


char * gosub_pop(struct thread_control_block * objThreadCntrolBlock);

struct thread_control_block g_thread_control_block[NUM_THREAD];
jmp_buf e_buf; /* hold environment for longjmp() */

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
void serror(int error),
	 get_exp(struct thread_control_block * objThreadCntrolBlock, float * result, int* boolValue),
	 putback(struct thread_control_block * objThreadCntrolBlock);
void    level1(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        level2(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        level3(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        level4(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        level5(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        level6(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        level7(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue),
        primitive(struct thread_control_block * objThreadCntrolBlock, float *result);
void unary(char, float *r),
	arith(char o, float *r, float *h);

int load_program(char *p, char *pname);
void assignment(struct thread_control_block * objThreadCntrolBlock) ;
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
HANDLE    g_basic_interpreter_handle[NUM_THREAD];
#else
pthread_t g_basic_interpreter_handle[NUM_THREAD];
#endif

#ifdef WIN32
unsigned __stdcall basic_interpreter(void* arg)
#else
void* basic_interpreter(void* arg)
#endif
{
  int iIdx = 0;
  int iRet = 0;
  struct thread_control_block * objThreadCntrolBlock
  				= (struct thread_control_block*)arg;
  
  setPrgmState(EXECUTE_R);

  iRet = call_interpreter(objThreadCntrolBlock, 1);
  setPrgmState(IDLE_R);

  free(objThreadCntrolBlock->instrSet);
  objThreadCntrolBlock->instrSet = 0 ;
#ifdef WIN32
  iIdx = g_thread_control_block[0].iThreadIdx ;
  CloseHandle( g_basic_interpreter_handle[iIdx] );  
  g_basic_interpreter_handle[iIdx] = NULL; 
  return NULL;
#else
  pthread_join(g_basic_interpreter_handle[iIdx], NULL);
  g_basic_interpreter_handle[iIdx] = 0;
#endif // WIN32
}

void setLinenum(struct thread_control_block* objThreadCntrolBlock, int iLinenum)
{
#ifdef WIN32
	win_cs_lock tempBaseLock ;
#else
	linux_mutex_lock tempBaseLock ;
#endif 
	auto_lock temp(&tempBaseLock) ;
	if(objThreadCntrolBlock->stateLineNum == LINENUM_CONSUMED)
	{
		objThreadCntrolBlock->stateLineNum = LINENUM_PRODUCED;
		objThreadCntrolBlock->iLineNum = iLinenum;
	}
	else
	{
		printf("setLinenum failure\n");
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
    int num ;
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

int call_interpreter(struct thread_control_block* objThreadCntrolBlock, int mode)
{
  int iRet = 0;
  int iLinenum;
  char * cLineContentPtr = 0 ;
  char * cLineContentProgPtr = 0 ;
  char cLineContent[128];
  int iScan = 0 ;
  // char in[80];
  // char *p_buf;
  char *t = 0;
  string strFileName ;

  objThreadCntrolBlock->ret_value = 0.0 ;
  if(mode == 1)
  {
	  /* allocate memory for the program */
	  if(!(objThreadCntrolBlock->p_buf=(char *) malloc(PROG_SIZE))) {
		printf("allocation failure");
		exit(1);
	  }
	  objThreadCntrolBlock->iSubProgNum = 0 ;
	  memset(objThreadCntrolBlock->sub_prog, 0x00, sizeof(char *) * NUM_SUBROUTINE);
	  
      label_init(objThreadCntrolBlock);  /* zero all labels */
	  /* load the program to execute */
	  if(!load_program(objThreadCntrolBlock->p_buf,objThreadCntrolBlock->project_name)) exit(1);
      generateXPathVector(objThreadCntrolBlock->project_name);
	  
	  objThreadCntrolBlock->prog = objThreadCntrolBlock->p_buf;
	  objThreadCntrolBlock->prog_end = objThreadCntrolBlock->prog + strlen(objThreadCntrolBlock->prog);
	  memset(objThreadCntrolBlock->prog_jmp_line, 0x00, sizeof(objThreadCntrolBlock->prog_jmp_line));
	  scan_labels(objThreadCntrolBlock, INSIDE_FUNC, objThreadCntrolBlock->project_name); /* find the labels in the program */
	  objThreadCntrolBlock->select_and_cycle_tos = 0; /* initialize the FOR stack index */
	  objThreadCntrolBlock->gosub_tos = 0; /* initialize the GOSUB stack index */
	  
	  get_token(objThreadCntrolBlock);
	  while(objThreadCntrolBlock->tok == IMPORT)
	  {
		  exec_import(objThreadCntrolBlock);
		  get_token(objThreadCntrolBlock);
	  }

// 	  for(vector<sub_label>::iterator it
// 		  = objThreadCntrolBlock->sub_label_table.begin();
// 	  it != objThreadCntrolBlock->sub_label_table.end(); ++it)
// 	  {
// 		  printf("%d %s %08X\n", (int)it->type, it->name, (int)it->p);
// 	  }
      
      memset(cLineContent, 0x00, 128);
	  strFileName = objThreadCntrolBlock->project_name ;
	  sprintf(cLineContent, "%s::main", objThreadCntrolBlock->project_name);
      char * loc = find_label(objThreadCntrolBlock, cLineContent); // "main");
	  if(loc=='\0')
	  {
		  serror(7); /* label not defined */
		  return 1;
	  }
	  // Save local var stack index.
	  int lvartemp = objThreadCntrolBlock->local_var_stack.size();
	  gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
	  objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
	  
	  while(*(objThreadCntrolBlock->token) != ')')
	  {
		  get_token(objThreadCntrolBlock);
	  }
	  
	  iRet = call_interpreter(objThreadCntrolBlock, 0);
	  printf("Execute over.");
	  free(objThreadCntrolBlock->p_buf);
	  for(int i = 0; i < objThreadCntrolBlock->iSubProgNum; i++)
	  {
		  if (objThreadCntrolBlock->sub_prog[i])
			 free(objThreadCntrolBlock->sub_prog[i]);
	  }
	  return iRet;
  }

  do {
  	if(objThreadCntrolBlock->prog_mode == 1)
  	{
	    memset(cLineContent, 0x00, 128);
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
		    setPrgmState(PAUSED_R);
  			printf("Line number(%s) -> ", cLineContent);
			int iOldLinenum = iLinenum ;
			
			// iScan = scanf("%d", &iLinenum);
			InterpreterState interpreterState  = PAUSED_R ;
			while(interpreterState == PAUSED_R)
			{
#ifdef WIN32
				Sleep(100);
				// interpreterState = EXECUTE_R ;
#else
				sleep(1);
				interpreterState = getPrgmState();
#endif
			}
			iLinenum = objThreadCntrolBlock->iLineNum ;
			// 
			if(iScan == 0)
			{
				printf("illegal line number.");
				exit(1);
			}
			if(iOldLinenum != iLinenum - 1)
			{
			    //
				int iRet = call_internal_cmd(
						find_internal_cmd((char *)"movej"), -1,
						objThreadCntrolBlock);
				if(iRet == END_COMMND_RET)
					return END_COMMND_RET;
			}
			// scanf("%s", cLinenum);
			// iLinenum = atoi(cLinenum);
			if((iLinenum >0) && (iLinenum < 1024))
			{
				if(objThreadCntrolBlock->prog_jmp_line[iLinenum - 1].prog_pos!= 0)
				{
					objThreadCntrolBlock->prog =
						objThreadCntrolBlock->prog_jmp_line[iLinenum - 1].prog_pos;
				}
			}
		    setPrgmState(EXECUTE_R);
		}
  	}
	
    objThreadCntrolBlock->token_type = get_token(objThreadCntrolBlock);
    /* check for assignment statement */
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
	    int iIdx = find_internal_cmd(objThreadCntrolBlock->token) ;
		int iLineNum = calc_line_from_prog(objThreadCntrolBlock);
		if(objThreadCntrolBlock->is_main_thread == 0)
		{
			if(call_internal_cmd_exec_sub_thread(iIdx) == 0)
			{
				printf("Non execution permissions : %s\n", objThreadCntrolBlock->token);
			}
			else
			{
				int iRet = call_internal_cmd(iIdx, iLineNum, 
					objThreadCntrolBlock);
				// find_eol(objThreadCntrolBlock);
				if(iRet == END_COMMND_RET)
					return END_COMMND_RET;
			}
		}
		else
		{
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
		  find_eol(objThreadCntrolBlock); // exec_default(objThreadCntrolBlock);
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
  } while (objThreadCntrolBlock->tok != FINISHED);
  return 0 ; // NULL ;
}


int  calc_line_from_prog(struct thread_control_block * objThreadCntrolBlock)
{
	for(int i = 0 ; i < 1024 ; i++)
	{
		if(objThreadCntrolBlock->prog < objThreadCntrolBlock->prog_jmp_line[i].prog_pos)
			return i ;
	}
	return 0;
}

/* Load a program. */
int load_program(char *p, char *pname)
{
  char fname[128];
  FILE *fp = 0 ;
  int i=0;
  
  sprintf(fname, "%s.xml", pname);
  parse_xml_file_wrapper(fname);
  sprintf(fname, "%s.bas", pname);
  if(!(fp=fopen(fname, "r"))) return 0;

  i = 0;
  do {
    *p = getc(fp);
    p++; i++;
  } while(!feof(fp) && i<PROG_SIZE);
  *(p-2) = '\0'; /* null terminate the program */
  fclose(fp);
  return 1;
}

int release_array_element(struct thread_control_block * objThreadCntrolBlock)
{
    float value;
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
		 serror(4);
		 return -1;
	  }
	  return (int)value ;
	}
	get_token(objThreadCntrolBlock);
	if(objThreadCntrolBlock->token[0] == '['){
	  value = release_array_element(objThreadCntrolBlock);
      sprintf(val_name, "%s[%d]", val_name, (int)value);
	  value = find_var(objThreadCntrolBlock, val_name);
	  // Jump ']'
      get_token(objThreadCntrolBlock);
	  if(objThreadCntrolBlock->token[0] != ']'){
		 putback(objThreadCntrolBlock);
		 // Simulate left value
		 objThreadCntrolBlock->token_type = NUMBER ;
		 sprintf(objThreadCntrolBlock->token, "%d", (int)value);
		 // Calc
	     level3(objThreadCntrolBlock, &value, &boolValue);
	     putback(objThreadCntrolBlock);
	  }
	  return (int)value;
	}
	else {
	  objThreadCntrolBlock->prog = temp_prog;
	  get_exp(objThreadCntrolBlock, &value, &boolValue);

	  get_token(objThreadCntrolBlock);
	  if(objThreadCntrolBlock->token[0] != ']') {
	    serror(4);
	    return -1;
	  }
	  else {
	    return (int)value;
	  }
	}
}


void deal_array_element(struct thread_control_block * objThreadCntrolBlock)
{
    int array_value;
    char array_variable[128];
	char * temp_prog = 0 ;
	char * start_pos = 0 ;
	memset(array_variable, 0x00, 128);
    start_pos = array_variable ;

	temp_prog = objThreadCntrolBlock->prog;
	get_token(objThreadCntrolBlock);
    strcpy(array_variable, objThreadCntrolBlock->token) ;
	objThreadCntrolBlock->prog = temp_prog;

//	while(1)
//	{
       get_token(objThreadCntrolBlock);
	   if(!isalpha(objThreadCntrolBlock->token[0])) {
	     serror(4);
	     return;
	   }
	   get_token(objThreadCntrolBlock);
	   if(objThreadCntrolBlock->token[0] == '['){
	   	  array_value = release_array_element(objThreadCntrolBlock);
		  if(array_value < 0) {
	   	     putback(objThreadCntrolBlock);
		     serror(4);
		     return;
		  }
          sprintf(objThreadCntrolBlock->token, "%s[%d]", array_variable, (int)array_value);
  	      objThreadCntrolBlock->prog--;
	   }
	   else
	   	  putback(objThreadCntrolBlock);
//	   // return ']' to the prog
//	   objThreadCntrolBlock->prog--;
//	}
}

/* Assign a variable a value. */
void assignment(struct thread_control_block * objThreadCntrolBlock)
{
  float value;
  int boolValue;
  char var[80];
  char * tempProg ;

  /* get the variable name */
  tempProg = objThreadCntrolBlock->prog ;
  get_token(objThreadCntrolBlock);
  if(!isalpha(objThreadCntrolBlock->token[0])) {
    serror(4);
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
	// var = toupper(*token)-'A';
  }
  else
  {
  	putback(objThreadCntrolBlock);
  }

  /* get the equals sign */
  get_token(objThreadCntrolBlock);
  if(*(objThreadCntrolBlock->token)!=EQ) {
    serror(3);
    return;
  }

  /* get the value to assign to var */
  get_exp(objThreadCntrolBlock, &value, &boolValue);

  /* assign the value */
  // variables[var] = value;
  assign_var(objThreadCntrolBlock, var, value) ;
}

/* Execute a simple version of the BASIC PRINT statement */
void print(struct thread_control_block * objThreadCntrolBlock)
{
  float answer;
  int boolValue;

  int len=0, spaces;
  char last_delim;

  do {
    get_token(objThreadCntrolBlock); /* get next list item */
    if(objThreadCntrolBlock->tok==EOL || objThreadCntrolBlock->tok==FINISHED) break;
    if(objThreadCntrolBlock->token_type==QUOTE) { /* is string */
      printf("%s", objThreadCntrolBlock->token);
      len += strlen(objThreadCntrolBlock->token);
      get_token(objThreadCntrolBlock);
    }
    else { /* is expression */
      putback(objThreadCntrolBlock);
      get_exp(objThreadCntrolBlock, &answer, &boolValue);
      get_token(objThreadCntrolBlock);
      len += printf("%d", (int)answer);
    }
    last_delim = *(objThreadCntrolBlock->token);

    if(*(objThreadCntrolBlock->token)==';') {
      /* compute number of spaces to move to next tab */
      spaces = 8 - (len % 8);
      len += spaces; /* add in the tabbing position */
      while(spaces) {
	printf(" ");
        spaces--;
      }
    }
    else if(*(objThreadCntrolBlock->token)==',') /* do nothing */;
    else if(objThreadCntrolBlock->tok!=EOL
		&& objThreadCntrolBlock->tok!=FINISHED) serror(0);
  } while (*(objThreadCntrolBlock->token)==';'
  			|| *(objThreadCntrolBlock->token)==',');

  if(objThreadCntrolBlock->tok==EOL || objThreadCntrolBlock->tok==FINISHED) {
    if(last_delim != ';' && last_delim!=',') printf("\n");
  }
  else serror(0); /* error is not , or ; */

}

/* Find all labels. */
void scan_labels(struct thread_control_block * objThreadCntrolBlock, 
				SubLabelType type, char * pname)
{
  struct sub_label  objLabel ;
   int iLineNum = 0 ;
  int addr;
  char *temp;

  temp = objThreadCntrolBlock->prog;   /* save pointer to top of program */

  /* if the first token in the file is a label */
  objThreadCntrolBlock->prog_jmp_line[iLineNum].prog_pos = objThreadCntrolBlock->prog ;
  iLineNum++ ;
  get_token(objThreadCntrolBlock);
  if(objThreadCntrolBlock->token_type==NUMBER) {
    // strcpy(label_table[0].name,token);
    // label_table[0].p=prog;
    objLabel.type = type ;
	
	sprintf(objLabel.name, "%s::%s", pname, 
       			objThreadCntrolBlock->token);
	
	objLabel.p = objThreadCntrolBlock->prog;
	addr = add_label(objThreadCntrolBlock, objLabel);
      if(addr==-1 || addr==-2) {
          (addr==-1) ?serror(5):serror(6);
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
		  addr = add_label(objThreadCntrolBlock, objLabel);
		  if(addr==-1 || addr==-2) {
			  (addr==-1) ?serror(5):serror(6);
		  }
	  }
  }
  else
	  putback(objThreadCntrolBlock);

  
  objThreadCntrolBlock->prog_jmp_line[iLineNum].type = COMMON ;
  if(objThreadCntrolBlock->token_type==INNERCMD) 
  {
	  int iIdx = find_internal_cmd(objThreadCntrolBlock->token) ;
	  if((iIdx >= 0) && (iIdx <= 2))  // movel - 0 ; movej - 1 ; movec - 2
		  objThreadCntrolBlock->prog_jmp_line[iLineNum].type = MOTION ;
  }
  find_eol(objThreadCntrolBlock);
  
  do {
    objThreadCntrolBlock->prog_jmp_line[iLineNum].prog_pos = objThreadCntrolBlock->prog ;
    get_token(objThreadCntrolBlock);
    if(objThreadCntrolBlock->token_type==NUMBER) {
      // strcpy(label_table[addr].name, token);
      // label_table[addr].p = prog;  /* current point in program */
          objLabel.type = type ;
		  sprintf(objLabel.name, "%s::%s", pname, 
	       			objThreadCntrolBlock->token);
		  objLabel.p = objThreadCntrolBlock->prog;

		  addr = add_label(objThreadCntrolBlock, objLabel);
		  if(addr==-1 || addr==-2) {
			  (addr==-1) ?serror(5):serror(6);
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
	        addr = add_label(objThreadCntrolBlock, objLabel);
	        if(addr==-1 || addr==-2) {
	          (addr==-1) ?serror(5):serror(6);
	        }
		}
	}

	objThreadCntrolBlock->prog_jmp_line[iLineNum].type = COMMON ;
	if(objThreadCntrolBlock->token_type==INNERCMD) 
	{
	    int iIdx = find_internal_cmd(objThreadCntrolBlock->token) ;
		if((iIdx >= 0) && (iIdx <= 2))  // movel - 0 ; movej - 1 ; movec - 2
			objThreadCntrolBlock->prog_jmp_line[iLineNum].type = MOTION ;
	}
    /* if not on a blank line, find next line */
    if(objThreadCntrolBlock->tok!=EOL)
		find_eol(objThreadCntrolBlock);
	
    iLineNum++ ;
  } while(objThreadCntrolBlock->tok!=FINISHED);
  objThreadCntrolBlock->prog = temp;  /* restore to original */
}

/* Find the start of the next line. */
void find_eol(struct thread_control_block * objThreadCntrolBlock)
{
  while(*(objThreadCntrolBlock->prog)!='\n'
  	&& *(objThreadCntrolBlock->prog)!='\0') ++(objThreadCntrolBlock->prog);
  if(*(objThreadCntrolBlock->prog))
  	(objThreadCntrolBlock->prog)++;
}

/* Return index of next free position in label array.
   A -1 is returned if the array is full.
   A -2 is returned when duplicate label is found.
*/

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

/* Find location of given label.  A null is returned if
   label is not found; otherwise a pointer to the position
   of the label is returned.
*/
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
	    if(!strcmp(it->name, name))  {
	        return it->p;
	    }
	}
	return '\0';
}
SubLabelType find_label_type(struct thread_control_block * objThreadCntrolBlock, char *name)
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
			  serror(1);
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

/* Execute a GOTO statement. */
void exec_goto(struct thread_control_block * objThreadCntrolBlock)
{
  char *loc;

  get_token(objThreadCntrolBlock); /* get label to go to */
  /* find the location of the label */
  loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
  if(loc=='\0')
  {
     serror(7); /* label not defined */
  }
  else
  {
  	 jumpout_blocks_in_loc(objThreadCntrolBlock, loc);
  	 objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
  }
}

/* Initialize the array that holds the labels.
   By convention, a null label name indicates that
   array position is unused.
*/
void label_init(struct thread_control_block * objThreadCntrolBlock)
{
  // register int t;

  // for(t=0; t<NUM_LAB; ++t) label_table[t].name[0]='\0';
}

int calc_conditions(
		struct thread_control_block * objThreadCntrolBlock)
{
  float x ;
  int boolValue = 1; // true - 0; // false
  get_exp(objThreadCntrolBlock, &x, &boolValue); /* get left expression */
//	return calc_single_condition(objThreadCntrolBlock);
  return boolValue ;
}

/* Execute an IF statement. */
void exec_if(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack if_stack ;
  int cond;

  cond = calc_conditions(objThreadCntrolBlock);

  if(cond) { /* is true so process target of IF */
    get_token(objThreadCntrolBlock);
    if(objThreadCntrolBlock->tok!=THEN) {
      serror(8);
      return;
    }/* else program execution starts on next line */
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
              printf("%s  out range", objThreadCntrolBlock->token);
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
			break ;
	    }
     }
  }
}

/* Execute an ELSE statement. */
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
          printf("%s  out range", objThreadCntrolBlock->token);
		  return;
	   }
	}
	else if(objThreadCntrolBlock->tok==ELSE)  // Execute else
	{
	    serror(1); /* not a legal operator */
		break ;
    }
	else if(objThreadCntrolBlock->tok==ELSEIF)  // Execute else
	{
	    serror(1); /* not a legal operator */
		break ;
    }
 	else if (objThreadCntrolBlock->tok==ENDIF) // Finish if
	{
 	    if_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
 	    if(if_stack.itokentype != IF){
		   serror(4);
		   return;
 	    }
		break ;
    }
  }
}

/* Execute an IF statement. */
void exec_elseif(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack if_stack ;
  // float x , y;
  int cond;

  cond = calc_conditions(objThreadCntrolBlock);

  if(cond) { /* is true so process target of IF */
    get_token(objThreadCntrolBlock);
    if(objThreadCntrolBlock->tok!=THEN) {
      serror(8);
      return;
    }/* else program execution starts on next line */
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
              printf("%s  out range", objThreadCntrolBlock->token);
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
			break ;
	    }
     }
  }
}

/* Execute a FOR loop. */
void exec_for(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack for_stack;
  float value;
  int boolValue;

  get_token(objThreadCntrolBlock); /* read the control variable */
  if(!isalpha(*(objThreadCntrolBlock->token))) {
    serror(4);
    return;
  }

  // for_stack.var=toupper(*token)-'A'; /* save its index */
  memset(for_stack.var, 0x00, 80);
  strcpy(for_stack.var, objThreadCntrolBlock->token);

  get_token(objThreadCntrolBlock); /* read the equals sign */
  if(*(objThreadCntrolBlock->token)!=EQ) {
    serror(3);
    return;
  }

  get_exp(objThreadCntrolBlock, &value, &boolValue); /* get initial value */

  // variables[for_stack.var]=value;
  assign_var(objThreadCntrolBlock, for_stack.var, value) ;

  get_token(objThreadCntrolBlock);
  if(objThreadCntrolBlock->tok!=TO) serror(9); /* read and discard the TO */

  get_exp(objThreadCntrolBlock, &for_stack.target, &boolValue); /* get target value */

  /* if loop can execute at least once, push info on stack */
  // if(value>=variables[for_stack.var]) {
  if(value >= find_var(objThreadCntrolBlock, for_stack.var)) {
  	for_stack.itokentype = FOR ;
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
              printf("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if (objThreadCntrolBlock->tok==NEXT)
		{
			break ;
	    }
     }
  }
}

/* Execute a NEXT statement. */
void exec_next(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack for_stack;

  for_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(for_stack.itokentype != FOR){
     serror(4);
     return;
  }
  // variables[for_stack.var]++; /* increment control variable */
  int iVar = find_var(objThreadCntrolBlock, for_stack.var);
  iVar++ ;
  assign_var(objThreadCntrolBlock, for_stack.var,iVar);

  // if(variables[for_stack.var]>for_stack.target) return;  /* all done */
  if(find_var(objThreadCntrolBlock, for_stack.var)>for_stack.target) return;  /* all done */
  select_and_cycle_push(objThreadCntrolBlock, for_stack);  /* otherwise, restore the info */
  objThreadCntrolBlock->prog = for_stack.loc;  /* loop */
}

// Deal the COMMAND such like "loop 5"
void exec_loop(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack loop_stack;
  // int value;
  int boolValue;

  // for_stack.var=toupper(*token)-'A'; /* save its index */
  memset(loop_stack.var, 0x00, 80);
  strcpy(loop_stack.var, "loop_var");
  assign_var(objThreadCntrolBlock, loop_stack.var, 1) ;

  // deal target such like "5"
  get_exp(objThreadCntrolBlock, &loop_stack.target, &boolValue); /* get target value */

  /* if loop can execute at least once, push info on stack */
  loop_stack.itokentype = LOOP ;
  loop_stack.loc = objThreadCntrolBlock->prog;
  select_and_cycle_push(objThreadCntrolBlock, loop_stack);
}

void exec_endloop(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack loop_stack;

  loop_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(loop_stack.itokentype != LOOP){
     serror(4);
     return;
  }
  // variables[for_stack.var]++; /* increment control variable */
  int iVar = find_var(objThreadCntrolBlock, loop_stack.var);
  iVar++ ;
  assign_var(objThreadCntrolBlock, loop_stack.var,iVar);

  // if(variables[for_stack.var]>for_stack.target) return;  /* all done */
  if(find_var(objThreadCntrolBlock, loop_stack.var)>loop_stack.target)
  	 return;  /* all done */
  select_and_cycle_push(objThreadCntrolBlock, loop_stack);  /* otherwise, restore the info */
  objThreadCntrolBlock->prog = loop_stack.loc;  /* loop */
}

/* Execute a WHILE loop. */
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
              printf("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if (objThreadCntrolBlock->tok==WEND) // Finish if
		{
			break ;
	    }
     }
  }
}

/* Execute a WEND statement. */
void exec_wend(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  // float x, y ;
  int cond;
  // char op_char;

  struct select_and_cycle_stack while_stack;

  while_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  if(while_stack.itokentype != WHILE){
     serror(4);
     return;
  }
  objThreadCntrolBlock->prog = while_stack.while_loc;  /* loop */

  cond = calc_conditions(objThreadCntrolBlock);

  if(cond) { /* is true so process target of IF */
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
              printf("%s  out range", objThreadCntrolBlock->token);
			  return;
		   }
		}
		else if (objThreadCntrolBlock->tok==WEND) // Finish if
		{
			break ;
	    }
     }
  }
}

/* Execute a NEXT statement. */
void exec_continue(struct thread_control_block * objThreadCntrolBlock)
{
  int cond;
  struct select_and_cycle_stack cycle_stack;

  cycle_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  while(cycle_stack.itokentype == IF)
  {
	cycle_stack = select_and_cycle_pop(objThreadCntrolBlock); /* read the loop info */
  }

  if(cycle_stack.itokentype == FOR){
      // variables[cycle_stack.var]++; /* increment control variable */
	  int iVar = find_var(objThreadCntrolBlock, cycle_stack.var);
	  iVar++ ;
	  assign_var(objThreadCntrolBlock, cycle_stack.var,iVar);

     // if(variables[cycle_stack.var]>cycle_stack.target)
     if(find_var(objThreadCntrolBlock, cycle_stack.var)>cycle_stack.target)
	 {
         while (objThreadCntrolBlock->tok!=NEXT) get_token(objThreadCntrolBlock);
	     return;  /* all done */
     }
	 select_and_cycle_push(objThreadCntrolBlock, cycle_stack);  /* otherwise, restore the info */
	 objThreadCntrolBlock->prog = cycle_stack.loc;  /* loop */
  }
  else if(cycle_stack.itokentype == WHILE){
	  objThreadCntrolBlock->prog = cycle_stack.while_loc;  /* loop */
      cond = calc_conditions(objThreadCntrolBlock);

	  if(cond) { /* is true so process target of IF */
		  select_and_cycle_push(objThreadCntrolBlock, cycle_stack);  /* otherwise, restore the info */
		  objThreadCntrolBlock->prog = cycle_stack.loc;  /* loop */
	  }
	  else
	  {
  		  while (objThreadCntrolBlock->tok!=WEND)
		  	get_token(objThreadCntrolBlock);
	  	  return ;
	  }
  }
  return ;
}

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
		  break;
	  }
	  if((cycle_stack.itokentype == WHILE) && (objThreadCntrolBlock->tok==WEND))
	  {
		  break;
	  }
	  if((cycle_stack.itokentype == SELECT) && (objThreadCntrolBlock->tok==END))
	  {
		  get_token(objThreadCntrolBlock);
		  if(objThreadCntrolBlock->tok != SELECT){
			  serror(4);
			  return;
		  }
		  break;
	  }
  }
}

void exec_select(struct thread_control_block * objThreadCntrolBlock)
{
  float x ;
  int boolValue;
  struct select_and_cycle_stack select_stack;

  get_token(objThreadCntrolBlock);
  /* get next list item */
  if(objThreadCntrolBlock->tok != CASE) { /* is string */
      serror(3);
      return;
  }
  // get exp
  get_exp(objThreadCntrolBlock, &x, &boolValue); /* get expression */
  select_stack.itokentype = SELECT ;
  select_stack.target = x ;
  select_and_cycle_push(objThreadCntrolBlock, select_stack);
}

void exec_case(struct thread_control_block * objThreadCntrolBlock)
{
  int iRet = JUMP_OUT_INIT ;
  struct select_and_cycle_stack select_stack ;
  float x ; // , y, cond;
  int boolValue;
  // char op;

  select_stack = select_and_cycle_pop(objThreadCntrolBlock);

  get_exp(objThreadCntrolBlock, &x, &boolValue); /* get expression */

  if(x == select_stack.target) { /* is true so process target of CASE */
     select_and_cycle_push(objThreadCntrolBlock, select_stack);  // Use of END SELECt
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
               printf("%s  out range", objThreadCntrolBlock->token);
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
			   serror(4);
			   return;
			}
			break ;
	    }
     }
  }
}

void exec_endif(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack select_stack ;

  select_stack = select_and_cycle_pop(objThreadCntrolBlock);
  if(IF != select_stack.itokentype) {
     serror(4);
     return;
  }
  return;
}

int exec_end(struct thread_control_block * objThreadCntrolBlock)
{
  struct select_and_cycle_stack select_stack ;

  get_token(objThreadCntrolBlock);
  if(objThreadCntrolBlock->tok == SELECT)
  {
    select_stack = select_and_cycle_pop(objThreadCntrolBlock);
	if(objThreadCntrolBlock->tok!=select_stack.itokentype) {
	   serror(4);
	   return 0;
	}
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
	   serror(4);
	   return;
	}
  }
 */
  // exit(0);
  return 1 ;
}

/* Push function for the FOR stack. */
void select_and_cycle_push(struct thread_control_block * objThreadCntrolBlock,
		struct select_and_cycle_stack i)
{
   if(objThreadCntrolBlock->select_and_cycle_tos>SELECT_AND_CYCLE_NEST)
    serror(10);

  objThreadCntrolBlock->selcyclstack[objThreadCntrolBlock->select_and_cycle_tos]=i;
  objThreadCntrolBlock->select_and_cycle_tos++;
//    printf("\t\t\t\t\t select_and_cycle_push with %d at %d\n",
//		i.itokentype,
//		objThreadCntrolBlock->select_and_cycle_tos);
}

struct select_and_cycle_stack select_and_cycle_pop(
	struct thread_control_block * objThreadCntrolBlock)
{
  objThreadCntrolBlock->select_and_cycle_tos--;
  if(objThreadCntrolBlock->select_and_cycle_tos<0)
  {
	  serror(11);
  }
//    printf("\t\t\t\t\t select_and_cycle_pop with %d\n", objThreadCntrolBlock->select_and_cycle_tos);

  return(objThreadCntrolBlock->selcyclstack[objThreadCntrolBlock->select_and_cycle_tos]);
}

/* Execute a simple form of the BASIC INPUT command */
void input(struct thread_control_block * objThreadCntrolBlock)
{
  // char str[80], var;
  char var[80];
  int i;

  get_token(objThreadCntrolBlock); /* see if prompt string is present */
  if(objThreadCntrolBlock->token_type==QUOTE) {
    printf("%s", objThreadCntrolBlock->token); /* if so, print it and check for comma */
    get_token(objThreadCntrolBlock);
    if(*(objThreadCntrolBlock->token)!=',') serror(1);
    get_token(objThreadCntrolBlock);
  }
  else printf("? "); /* otherwise, prompt with / */
  // var = toupper(*token)-'A'; /* get the input var */
  memset(var, 0x00, 80);
  strcpy(var, objThreadCntrolBlock->token);

  scanf("%d", &i); /* read input */

  // variables[var] = i; /* store it */
  assign_var(objThreadCntrolBlock, var, i) ;
}

double call_inner_func(struct thread_control_block * objThreadCntrolBlock)
{
    float value;
    int boolValue;
	int count ;
	char temp[NUM_OF_PARAMS][LAB_LEN];

	int funcIdx = find_internal_func(objThreadCntrolBlock->token);

    count = 0;
    get_token(objThreadCntrolBlock);
	// Fit for circumstance without parameter
	if((*(objThreadCntrolBlock->token) == '\r')
		|| (*(objThreadCntrolBlock->token) == '\n'))
		return 0.0;
    if(*(objThreadCntrolBlock->token) != '(')
    {
    	serror(2);
		return 0.0;
	}

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
        sprintf(temp[count], "%f", value); // save temporarily
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    // count--;

    // Now, push on local_var_stack in reverse order.
    if(count == 1)
	{
		return call_internal_func(funcIdx, temp[0]);
    }
	else if(count == 2)
	{
		return call_internal_func(funcIdx, temp[0], temp[1]);
    }
	return 0.0;
}

// Push the arguments to a function onto the local
// variable stack.
void get_args(struct thread_control_block * objThreadCntrolBlock)
{
    float value;
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
    	serror(2);
		return;
	}

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
        temp[count] = value; // save temporarily
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    count--;

    // Now, push on local_var_stack in reverse order.
    for(; count>=0; count--) {
        vt.value = temp[count];
        // vt.v_type = FORSIGHT_INT;
        objThreadCntrolBlock->local_var_stack.push_back(vt);
    }
}

// Get function parameters.
void get_params(struct thread_control_block * objThreadCntrolBlock)
{
    var_type *p;
    int i;

    i = objThreadCntrolBlock->local_var_stack.size()-1;
	if(i < 0)
    {
		// No parameter
    	// serror(2);
		return;
	}

    get_token(objThreadCntrolBlock);
    if(*(objThreadCntrolBlock->token) != '(')
    {
	    serror(7); /* label not defined */
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
		    	serror(2);
				return;
			}
            i--;
        }
        else
			break;
    } while(*(objThreadCntrolBlock->token) == ',');

    if(*(objThreadCntrolBlock->token) != ')')
    {
	    serror(7); /* label not defined */
		return;
    }
}

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
  
  if(!(objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum]
	    =(char *) malloc(PROG_SIZE))) {
	  printf("allocation failure");
	  exit(1);
  }
  // memset(file_buffer, 0x00, 128);
  // sprintf(file_buffer, "%s.bas", objLabel.name);
  // objLabel.p = objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum] ;
  load_program(objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum], objLabel.name);
  // Scan the labels in the import files
  proglabelsScan = objThreadCntrolBlock->prog ;
  objThreadCntrolBlock->prog = objThreadCntrolBlock->sub_prog[objThreadCntrolBlock->iSubProgNum];
  scan_labels(objThreadCntrolBlock, OUTSIDE_FUNC, objLabel.name);
  objThreadCntrolBlock->prog = proglabelsScan;
  objThreadCntrolBlock->iSubProgNum++ ;
    
/*
 *	  addr = add_label(objThreadCntrolBlock, objLabel);
 *    if(addr==-1 || addr==-2) {
 *        (addr==-1) ?serror(5):serror(6);
 *    }
 */
}

int exec_call_submain(struct thread_control_block * objThreadCntrolBlock)
{
	int iRet = 0 ;
	int lvartemp;
	char *loc;
	
	get_token(objThreadCntrolBlock);
	/* find the label to call */
	loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
	if(loc=='\0')
	{
		serror(7); /* label not defined */
		return 1;
	}
	// Save local var stack index.
	lvartemp = objThreadCntrolBlock->local_var_stack.size();
	
	get_args(objThreadCntrolBlock); // get function arguments
	
	objThreadCntrolBlock->func_call_stack.push(lvartemp); // push local var index
	
	gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
	objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
	
	get_params(objThreadCntrolBlock); // load the function's parameters with
	
	return call_interpreter(objThreadCntrolBlock, 0);
}

int exec_call(struct thread_control_block * objThreadCntrolBlock)
{
  char func_name[1024];
  int lvartemp;
  char *loc;

  get_token(objThreadCntrolBlock);
  /* find the label to call */
  loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
  if(loc=='\0')
  {
	  memset(func_name, 0x00, 1024);
	  sprintf(func_name, "%s::main", objThreadCntrolBlock->token);
	  loc = find_label(objThreadCntrolBlock, func_name);
	  if(loc=='\0')
	  {
		  serror(7); /* label not defined */
		  return 0;
	  }
  }
  
  // Save local var stack index.
  lvartemp = objThreadCntrolBlock->local_var_stack.size();

  get_args(objThreadCntrolBlock); // get function arguments

  objThreadCntrolBlock->func_call_stack.push(lvartemp); // push local var index

  gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
  objThreadCntrolBlock->prog = loc;  /* start program running at that loc */

  get_params(objThreadCntrolBlock); // load the function's parameters with

  int iRet = call_interpreter(objThreadCntrolBlock, 0);
  if(iRet == END_COMMND_RET)
	 return END_COMMND_RET;
  return 1;
}

/* Execute a GOSUB command. */
int gosub(struct thread_control_block * objThreadCntrolBlock)
{
  char *loc;

  get_token(objThreadCntrolBlock);
  /* find the label to call */
  loc = find_label(objThreadCntrolBlock, objThreadCntrolBlock->token);
  if(loc=='\0')
    serror(7); /* label not defined */
  else {
    gosub_push(objThreadCntrolBlock, objThreadCntrolBlock->prog); /* save place to return to */
    objThreadCntrolBlock->prog = loc;  /* start program running at that loc */
    return call_interpreter(objThreadCntrolBlock, 0);
  }
  return 1 ;
}

/* Return from GOSUB. */
void greturn(struct thread_control_block * objThreadCntrolBlock)
{
  float ret_value ;
  int   boolValue ;
  char* progTemp  ;
  progTemp = gosub_pop(objThreadCntrolBlock);
  if(objThreadCntrolBlock->func_call_stack.empty()) {
    // serror(2);
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
  		return ;
	  }
	  putback(objThreadCntrolBlock);
	  get_exp(objThreadCntrolBlock,&ret_value, &boolValue);
	  objThreadCntrolBlock->ret_value = ret_value ;
  }
  objThreadCntrolBlock->prog = progTemp ;
}

/* GOSUB stack push function. */
void gosub_push(struct thread_control_block * objThreadCntrolBlock, char *s)
{
  objThreadCntrolBlock->gosub_tos++;

  if(objThreadCntrolBlock->gosub_tos==SUB_NEST) {
    serror(12);
    return;
  }

  objThreadCntrolBlock->gosub_stack[objThreadCntrolBlock->gosub_tos]=s;

}

/* GOSUB stack pop function. */
char *gosub_pop(struct thread_control_block * objThreadCntrolBlock)
{
  if(objThreadCntrolBlock->gosub_tos==0) {
    serror(13);
    return 0;
  }

  return(objThreadCntrolBlock->gosub_stack[objThreadCntrolBlock->gosub_tos--]);
}

/* Entry point into parser. */
void get_exp(struct thread_control_block * objThreadCntrolBlock, float * result, int* boolValue)
{
  get_token(objThreadCntrolBlock);
  if(!objThreadCntrolBlock->token[0]) {
    serror(2);
    return;
  }
  level1(objThreadCntrolBlock, result, boolValue);
  putback(objThreadCntrolBlock); /* return last token read to input stream */
}


/* display an error message */
void serror(int error)
{
  static const char *e[]= {
    "syntax error",
    "unbalanced parentheses",
    "no expression present",
    "equals sign expected",
    "not a variable",
    "Label table full",
    "duplicate sub_label",
    "undefined sub_label",
    "THEN expected",
    "TO expected",
    "too many nested FOR loops",
    "NEXT without FOR",
    "too many nested GOSUBs",
    "RETURN without GOSUB"
    "Use Call in exp"
  };
  printf("%s\n", e[error]);
  longjmp(e_buf, 1); /* return to save point */
}

/* Get a token. */
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
    return (objThreadCntrolBlock->token_type = DELIMITER);
  }

  if(strchr("+-*^/%=;(),><[]", *objThreadCntrolBlock->prog)){ /* delimiter */
    // *temp=*objThreadCntrolBlock->prog;
  	if(strchr("<>=", *objThreadCntrolBlock->prog)) {
		switch(*objThreadCntrolBlock->prog) {
			case '<':
			    if(*(objThreadCntrolBlock->prog+1) == '=') {
			        objThreadCntrolBlock->prog++;
					objThreadCntrolBlock->prog++;   /* advance to next position */
			        *temp = LE; temp++; *temp = LE;
			    }
			    else if(*(objThreadCntrolBlock->prog+1) == '<') {
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
		serror(1);
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
        SubLabelType labelType = find_label_type(
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



/* Return a token to input stream. */
void putback(struct thread_control_block * objThreadCntrolBlock)
{

  char *t;

  t = objThreadCntrolBlock->token;
  for(; *t; t++) objThreadCntrolBlock->prog--;
  memset(objThreadCntrolBlock->token, 0x00, 80);
}

/* Look up a a token's internal representation in the
   token table.
*/
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

/* Return true if c is a delimiter. */
int isdelim_with_array(char c)
{
  if(strchr(" ;,+-<>/*%^=()", c) || c==9 || c=='\r' || c=='\n' || c==0)
    return 1;
  return 0;
}

/* Return true if c is a delimiter. */
int isdelim(char c)
{
  if(strchr(" ;,+-<>/*%^=()[]", c) || c==9 || c=='\r' || c=='\n' || c==0)
    return 1;
  return 0;
}

/* Return 1 if c is space or tab. */
int iswhite(char c)
{
  if(c==' ' || c=='\t') return 1;
  else return 0;
}


// Process relational operators.
void level1(struct thread_control_block * objThreadCntrolBlock, float *value, int* boolValue)
{
    float partial_value;
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

// Process relational operators.
void level2(struct thread_control_block * objThreadCntrolBlock, float *value, int* boolValue)
{
    float partial_value;
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
			    *boolValue = (*value < partial_value);
			    break;
			case LE:
			    *boolValue = (*value <= partial_value);
			    break;
			case GT:
			    *boolValue = (*value > partial_value);
			    break;
			case GE:
			    *boolValue = (*value >= partial_value);
			    break;
			case EQ:
			    *boolValue = (*value == partial_value);
			    break;
			case NE:
			    *boolValue = (*value != partial_value);
			    break;
        }
    }
}

/*  Add or subtract two terms. */
void level3(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue)
{
  register char  op;
  float hold;

  level4(objThreadCntrolBlock, result, boolValue);
  while((op = *(objThreadCntrolBlock->token)) == '+' || op == '-') {
    get_token(objThreadCntrolBlock);
    level4(objThreadCntrolBlock, &hold, boolValue);
    arith(op, result, &hold);
  }
}

/* Multiply or divide two factors. */
void level4(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue)
{
  register char  op;
  float hold;

  level5(objThreadCntrolBlock, result, boolValue);
  while((op = *(objThreadCntrolBlock->token))
  	== '*' || op == '/' || op == '%') {
    get_token(objThreadCntrolBlock);
    level5(objThreadCntrolBlock, &hold, boolValue);
    arith(op, result, &hold);
  }
}

/* Process integer exponent. */
void level5(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue)
{
  float hold;

  level6(objThreadCntrolBlock, result, boolValue);
  if(*(objThreadCntrolBlock->token)== '^') {
    get_token(objThreadCntrolBlock);
    level5(objThreadCntrolBlock, &hold, boolValue);
    arith('^', result, &hold);
  }
}

/* Is a unary + or -. */
void level6(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue)
{
  register char  op;

  op = 0;
  if((objThreadCntrolBlock->token_type==DELIMITER) &&
  	*(objThreadCntrolBlock->token)=='+'
  	|| *(objThreadCntrolBlock->token)=='-') {
    op = *(objThreadCntrolBlock->token);
    get_token(objThreadCntrolBlock);
  }
  level7(objThreadCntrolBlock, result, boolValue);
  if(op)
    unary(op, result);
}

/* Process parenthesized expression. */
void level7(struct thread_control_block * objThreadCntrolBlock, float *result, int* boolValue)
{
  if((*(objThreadCntrolBlock->token) == '(')
  	&& (objThreadCntrolBlock->token_type == DELIMITER)) {
    get_token(objThreadCntrolBlock);
    level1(objThreadCntrolBlock, result, boolValue);
    if(*(objThreadCntrolBlock->token) != ')')
      serror(1);
    get_token(objThreadCntrolBlock);
  }
  else
    primitive(objThreadCntrolBlock, result);
}

/* Find value of number or variable. */
void primitive(struct thread_control_block * objThreadCntrolBlock, float *result)
{
  int iRet = 0 ;
  char *progFuncCall; 
  switch(objThreadCntrolBlock->token_type) {
  case VARIABLE:
	objThreadCntrolBlock->g_variable_error = 0 ;
    *result = find_var(objThreadCntrolBlock, objThreadCntrolBlock->token);
	if(objThreadCntrolBlock->g_variable_error == 0)
       get_token(objThreadCntrolBlock);
	else
	{
		putback(objThreadCntrolBlock);
		deal_array_element(objThreadCntrolBlock);
		*result = find_var(objThreadCntrolBlock, objThreadCntrolBlock->token);
		// No deal unexist array type element
		// if(objThreadCntrolBlock->g_variable_error == 0)
		get_token(objThreadCntrolBlock);
		if(objThreadCntrolBlock->token[0] == ']')
			get_token(objThreadCntrolBlock);
	}
	objThreadCntrolBlock->g_variable_error = 0 ;
    return;
  case NUMBER:
    *result = atof(objThreadCntrolBlock->token);
    get_token(objThreadCntrolBlock);
    return;
  case INSIDEFUNC:
    progFuncCall = objThreadCntrolBlock->prog ;
  	putback(objThreadCntrolBlock);
	// exec_call(objThreadCntrolBlock);
	iRet = exec_call(objThreadCntrolBlock);
	if(iRet == END_COMMND_RET)
	   *result = 0 ;
	else
	   *result = objThreadCntrolBlock->ret_value ;
    // objThreadCntrolBlock->prog = progFuncCall;
	// find_eol(objThreadCntrolBlock);
    get_token(objThreadCntrolBlock);
    return;
  case BUILTINFUNC:
  	// use objThreadCntrolBlock->token
  	*result = call_inner_func(objThreadCntrolBlock);
    get_token(objThreadCntrolBlock);
    return;
  default:
    serror(0);
  }
}

/* Perform the specified arithmetic. */
void arith(char o, float *r, float *h)
{
  register int t, ex;

  switch(o) {
    case '-':
      *r = *r-*h;
      break;
    case '+':
      *r = *r+*h;
      break;
    case '*':
      *r = *r * *h;
      break;
    case '/':
      *r = (*r)/(*h);
      break;
    case '%':
      t = (*r)/(*h);
      *r = *r-(t*(*h));
      break;
    case '^':
      ex = *r;
      if(*h==0) {
        *r = 1;
        break;
      }
      for(t=*h-1; t>0; --t) *r = (*r) * ex;
      break;
  }
}

/* Reverse the sign. */
void unary(char o, float *r)
{
  if(o=='-') *r = -(*r);
}


// Declare a global variable.
void assign_var(struct thread_control_block * objThreadCntrolBlock, char *vname, float value)
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
    strcpy(vt.var_name, vname);
	vt.value = value;
    objThreadCntrolBlock->global_vars.push_back(vt);
}


/* Reverse Find the value of a variable. */
float find_var(struct thread_control_block * objThreadCntrolBlock, char *vname)
{
         vector<var_type>::reverse_iterator it ;
	  objThreadCntrolBlock->g_variable_error = 0 ;
//	  if(!isalpha(*s)){
//	    serror(4); /* not a variable */
//	    return 0;
//	  }
//	  return variables[toupper(*s)-'A'];
	if(!strcmp(vname, FORSIGHT_RETURN_VALUE))
	{
		return objThreadCntrolBlock->ret_value ;
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

	// printf("not defined variable (%s).\n", vname);
    // serror(4);
	objThreadCntrolBlock->g_variable_error = 1 ;
	return -1 ;
}

/* Reverse erase the value of a variable. */

int erase_var(struct thread_control_block * objThreadCntrolBlock, char *vname)
{
        vector<var_type>::iterator it ;
//	  if(!isalpha(*s)){
//	    serror(4); /* not a variable */
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
	printf("not defined variable.");
	return -1 ;
}

bool base_thread_create(int iIdx, void * args)
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
		g_basic_interpreter_handle[iIdx] = 0;
	}
#endif
	return ret;
}

void base_thread_wait(int iIdx)
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


