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
#include "forsight_io_controller.h"
#ifndef WIN32
// #include "io_interface.h"
#else
#include<string>
#include<vector>
#include<map>
#endif

#ifdef USE_FORSIGHT_REGISTERS_MANAGER
#ifndef WIN32
#include "reg_manager/reg_manager_interface_wrapper.h"
using namespace fst_ctrl ;
#endif
#endif

// Register name
#define TXT_AI    "ai"
#define TXT_AO    "ao"
#define TXT_DI    "di"
#define TXT_DO    "do"
#define TXT_RI    "ri"
#define TXT_RO    "ro"
#define TXT_SI    "si"
#define TXT_SO    "so"
#define TXT_UI    "ui"
#define TXT_UO    "uo"

#define DEFAULT_IO_VALUE    0   //  SMLT_FLASE 

/* Return true if c is a delimiter. */
static int isdelim(char c)
{
  if(strchr(" ;,+-<>/*%^=()[]_", c) || c==9 || c=='\r' || c=='\n' || c==0) 
    return 1;  
  return 0;
}

static int get_char_token(char * src, char * dst)
{
	char * tmp = src ;
	if(isalpha(*src)) { /* var or command */
		while(!isdelim(*(src))) 
			*dst++=*(src)++;
	}
	return src - tmp ;
}

static int get_num_token(char * src, char * dst)
{
	char * tmp = src ;
	if(isdigit(*src)) { /* var or command */
		while(!isdelim(*(src))) 
			*dst++=*(src)++;
	}
	return src - tmp ;
}

/************************************************* 
	Function:		forgesight_get_io_status
	Description:	get io status.
	Input:			name  - io name
	Output: 		NULL
	Return: 		io value
*************************************************/ 
eval_value forgesight_get_io_status(
			struct thread_control_block* objThreadCntrolBlock, char *name)
{	
	bool bRet = false ;
	eval_value value;
	uint32_t  iValue = 0;
	char io_name[16] ;
	char io_idx[16] ;
	// char io_key_buffer[16] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	if(namePtr[0] != '['){
		return value ;
	}
	namePtr++ ;
	
	memset(io_idx, 0x00, 16);
	temp = io_idx ;
	get_num_token(namePtr, temp);
	iIOIdx = atoi(io_idx);
	// namePtr += strlen(reg_idx) ;
	
	namePtr += strlen(io_idx) ;
	if(namePtr[0] != ']'){
		return value ;
	}
	namePtr++ ;

	FST_INFO("set_io status: %s:%d (%s).", io_name, iIOIdx, name);
	if(!strcmp(io_name, TXT_AI))
	{
	//	bRet = g_objRegManagerInterface->getAi(iIOIdx, iValue);
	}
	else if(!strcmp(io_name, TXT_AO))
	{
	//	bRet = g_objRegManagerInterface->getAo(iIOIdx, iValue);
	}
	else if(!strcmp(io_name, TXT_DI))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		bRet = g_objRegManagerInterface->getDi(iIOIdx, iValue);
#endif
	}
	else if(!strcmp(io_name, TXT_DO))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		bRet = g_objRegManagerInterface->getDo(iIOIdx, iValue);
#endif
	}
	else if(!strcmp(io_name, TXT_RI))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		bRet = g_objRegManagerInterface->getRi(iIOIdx, iValue);
#endif
	}
	else if(!strcmp(io_name, TXT_RO))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		bRet = g_objRegManagerInterface->getRo(iIOIdx, iValue);
#endif
	}
	else if(!strcmp(io_name, TXT_SI))
	{
	//	bRet = g_objRegManagerInterface->getSi(iIOIdx, iValue);
	}
	else if(!strcmp(io_name, TXT_SO))
	{
	//	bRet = g_objRegManagerInterface->getSo(iIOIdx, iValue);
	}
	else if(!strcmp(io_name, TXT_UI))
	{
	//	bRet = g_objRegManagerInterface->getUi(iIOIdx, iValue);
	}
	else if(!strcmp(io_name, TXT_UO))
	{
	//	bRet = g_objRegManagerInterface->getUo(iIOIdx, iValue);
	}
	FST_INFO("get_io status: %s:%d (%s) = %d.", io_name, iIOIdx, name, iValue);
	
	if(bRet != SUCCESS)
	{
		serror(objThreadCntrolBlock, 4) ; 
	}
	value.setFloatValue(iValue);
	return value;
}

/************************************************* 
	Function:		forgesight_set_io_status
	Description:	set io status.
	Input:			name  - io name
	Input:			valueStart  - io value
	Output: 		NULL
	Return: 		0 - success , -1 - failed
*************************************************/ 
int forgesight_set_io_status(
			struct thread_control_block* objThreadCntrolBlock,char *name, eval_value& valueStart)
{
	bool bRet = false ;
//	int iValue;
	char io_name[16] ;
	char io_idx[16] ;
	// char io_key_buffer[16] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	if(namePtr[0] != '['){
		return -1 ;
	}
	namePtr++ ;
	
	memset(io_idx, 0x00, 16);
	temp = io_idx ;
	get_num_token(namePtr, temp);
	iIOIdx = atoi(io_idx);
	// namePtr += strlen(reg_idx) ;
	
	namePtr += strlen(io_idx) ;
	if(namePtr[0] != ']'){
		return -1 ;
	}
	namePtr++ ;
	
	FST_INFO("set_io status: %s:%d (%s) = %d.", io_name, iIOIdx, name, (int)valueStart.getFloatValue());
	if(!strcmp(io_name, TXT_AI))
	{
	//	bRet = g_objRegManagerInterface->setAi(iIOIdx, (int)valueStart.getFloatValue());
	}
	else if(!strcmp(io_name, TXT_AO))
	{
	//	bRet = g_objRegManagerInterface->setAo(iIOIdx, (int)valueStart.getFloatValue());
	}
	else if(!strcmp(io_name, TXT_DI))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		bRet = g_objRegManagerInterface->setDi(iIOIdx, (int)valueStart.getFloatValue());
#endif
	}
	else if(!strcmp(io_name, TXT_DO))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		if(valueStart.getPulse() == true)
			bRet = g_objRegManagerInterface->setDoPulse(iIOIdx, valueStart.getFloatValue());
		else
			bRet = g_objRegManagerInterface->setDo(iIOIdx, (int)valueStart.getFloatValue());
#endif
	}
	else if(!strcmp(io_name, TXT_RI))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		bRet = g_objRegManagerInterface->setRi(iIOIdx, (int)valueStart.getFloatValue());
#endif
	}
	else if(!strcmp(io_name, TXT_RO))
	{
#ifdef WIN32
		bRet = SUCCESS;
#else
		if(valueStart.getPulse() == true)
			bRet = g_objRegManagerInterface->setRoPulse(iIOIdx, valueStart.getFloatValue());
		else
			bRet = g_objRegManagerInterface->setRo(iIOIdx, (int)valueStart.getFloatValue());
#endif
	}
	else if(!strcmp(io_name, TXT_SI))
	{
	//	bRet = g_objRegManagerInterface->setSi(iIOIdx, (int)valueStart.getFloatValue());
	}
	else if(!strcmp(io_name, TXT_SO))
	{
	//	bRet = g_objRegManagerInterface->setSo(iIOIdx, (int)valueStart.getFloatValue());
	}
	else if(!strcmp(io_name, TXT_UI))
	{
	//	bRet = g_objRegManagerInterface->setUi(iIOIdx, (int)valueStart.getFloatValue());
	}
	else if(!strcmp(io_name, TXT_UO))
	{
	//	bRet = g_objRegManagerInterface->setUo(iIOIdx, (int)valueStart.getFloatValue());
	}
	
	if(bRet != SUCCESS)
	{
		serror(objThreadCntrolBlock, 4) ; 
	}
	return 0;

}



