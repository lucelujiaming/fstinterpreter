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
#include "forsight_io_mapping.h"
#include "forsight_io_controller.h"
#ifndef WIN32
#include "io_interface.h"
#include <parameter_manager/parameter_manager_param_group.h>
#else
#include<string>
#include<vector>
#include<map>
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

#define SMLT_TRUE    1
#define SMLT_FLASE   0

#define IO_EMULATOR_YAML    "/share/io_manager/config/io_emulator.yaml"

typedef struct  
{
	bool is_aio_emulated ;
	bool is_dio_emulated ;
	bool is_rio_emulated ;
	bool is_sio_emulated ;
	bool is_uio_emulated ;
} io_emulate_config;

#ifndef WIN32
fst_parameter::ParamGroup * g_io_config_;
#else
std::map<std::string, int> g_io_config_;
#endif
io_emulate_config     g_io_config_emulated ;

int forgesight_io_config_get_value(string key, int& value)
{
#ifndef WIN32
    g_io_config_->clearLastError();
	value = DEFAULT_IO_VALUE;
	g_io_config_->getParam(key, value);
//    printf("forgesight_io_config_get_value:: %s:%d with 0x%llx\n", 
//    				key.c_str(), value, 
//    				g_io_config_->getLastError());
#else
    std::map<std::string, int>::iterator it = g_io_config_.find(key);
	if (it != g_io_config_.end())
		value = g_io_config_[key];
	else
		value = DEFAULT_IO_VALUE;
#endif
	return 1;
}

int forgesight_io_config_set_value(string key, eval_value& value)
{
#ifndef WIN32
    g_io_config_->clearLastError();
	g_io_config_->setParam(key, (int)value.getFloatValue());
    printf("forgesight_io_config_set_value:: %s:%d with 0x%llx\n", 
    				key.c_str(), (int)value.getFloatValue(), 
    				g_io_config_->getLastError());
	g_io_config_->dumpParamFile(IO_EMULATOR_YAML);
#else
    std::map<std::string, int>::iterator it = g_io_config_.find(key);
	if (it != g_io_config_.end())
		g_io_config_.insert(
			std::map<std::string, int>::value_type(
			key, (int)value.getFloatValue() ));
	else
		g_io_config_[key] = (int)value.getFloatValue() ;
#endif
	return 1;
}

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

void refresh_io_config_emulated()
{
	int iValue ;
	forgesight_io_config_get_value("aio_all.emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_aio_emulated = true ;
	else
		g_io_config_emulated.is_aio_emulated = false ;

	forgesight_io_config_get_value("dio_all.emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_dio_emulated = true ;
	else
		g_io_config_emulated.is_dio_emulated = false ;
	
	forgesight_io_config_get_value("rio_all.emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_rio_emulated = true ;
	else
		g_io_config_emulated.is_rio_emulated = false ;
	
	forgesight_io_config_get_value("sio_all.emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_sio_emulated = true ;
	else
		g_io_config_emulated.is_sio_emulated = false ;
	
	forgesight_io_config_get_value("uio_all.emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_uio_emulated = true ;
	else
		g_io_config_emulated.is_uio_emulated = false ;
#if 0	
	printf("is_aio_emulated:: %s , is_dio_emulated:: %s , "
		   "is_rio_emulated:: %s , is_sio_emulated:: %s , "
		   "is_uio_emulated:: %s .\n", 
		g_io_config_emulated.is_aio_emulated ? "true" : "false", 
		g_io_config_emulated.is_dio_emulated ? "true" : "false", 
		g_io_config_emulated.is_rio_emulated ? "true" : "false", 
		g_io_config_emulated.is_sio_emulated ? "true" : "false", 
		g_io_config_emulated.is_uio_emulated ? "true" : "false");
#endif
}

int forgesight_load_io_config()
{	
#ifndef WIN32
	g_io_config_ = new fst_parameter::ParamGroup(IO_EMULATOR_YAML);
#else
	g_io_config_.insert(
		std::map<std::string, int>::value_type("dio_all.emltFlag", SMLT_TRUE));
	g_io_config_.insert(
		std::map<std::string, int>::value_type("di[0].emltFlag", SMLT_TRUE));
	g_io_config_.insert(
		std::map<std::string, int>::value_type("do[0].emltFlag", SMLT_TRUE));
	g_io_config_.insert(
		std::map<std::string, int>::value_type("di[1].emltFlag", SMLT_TRUE));
	g_io_config_.insert(
		std::map<std::string, int>::value_type("do[2].emltFlag", SMLT_TRUE));
	g_io_config_.insert(
		std::map<std::string, int>::value_type("di[3].emltFlag", SMLT_TRUE));
	g_io_config_.insert(
		std::map<std::string, int>::value_type("do[3].emltFlag", SMLT_TRUE));
#endif
	refresh_io_config_emulated();
	return 1 ;
}

#define TPI_SUCCESS				(0)
int set_io_status_to_io_mananger(
	char *vname, eval_value& value)
{
#ifndef WIN32
	IOPortInfo info;
	memset(&info, 0x00, sizeof(IOPortInfo));
	
	if(g_io_mapper.find(vname) != g_io_mapper.end() )
	{
		U64 result = IOInterface::instance()->checkIO(
			g_io_mapper[vname].c_str(), &info);
		if (result != TPI_SUCCESS)
		{
			//    rcs::Error::instance()->add(result);
			//    tp_interface_->setReply(BaseTypes_StatusCode_FAILED);
			printf("IOInterface::instance()->checkIO Failed:: %s\n", 
				g_io_mapper[vname].c_str());
			return ;
		}
	}
	else{
        printf("MOD_IO:: not exist path : %s\n", vname); 
		return ;
    }
#endif
	char val = (char)value.getFloatValue();
#ifndef WIN32
	printf("\t SET:: %s \n", vname);
    printf("\t msg_id: %d\n", info.msg_id);
    printf("\t dev_id: %d\n", info.dev_id);
    printf("\t port_type: %d\n", info.port_type);
    printf("\t port_index: %d\n", info.port_index);
    printf("\t bytes_len: %d\n", info.bytes_len);
    printf("\t val: %d \n", val);
	IOInterface::instance()->setDO(&info, val);
#endif
	return 1 ;
}

int set_io_interface_status(char *vname, char val)
{
#ifndef WIN32
	IOPortInfo info;
	memset(&info, 0x00, sizeof(IOPortInfo));
	
	U64 result = IOInterface::instance()->checkIO(
		vname, &info);
	if (result != TPI_SUCCESS)
	{
		//    rcs::Error::instance()->add(result);
		//    tp_interface_->setReply(BaseTypes_StatusCode_FAILED);
		printf("IOInterface::instance()->checkIO Failed:: %s\n", vname);
		return ;
	}
	printf("\t SET:: %s : %d = %d\n", vname, info.port_index, val);
	IOInterface::instance()->setDO(&info, val);
#endif
	return 1 ;
}

eval_value get_io_status_from_io_mananger(
		char *vname)
{
	eval_value value ;
	int iValue = -1;
#ifndef WIN32
	char valueBuf[8];
	IOPortInfo info;
	memset(&info, 0x00, sizeof(IOPortInfo));
	
	if(g_io_mapper.find(vname) != g_io_mapper.end() )
	{
		U64 result = IOInterface::instance()->checkIO(
			g_io_mapper[vname].c_str(), &info);
		if (result != TPI_SUCCESS)
		{
			printf("IOInterface::instance()->checkIO Failed:: %s\n", 
				g_io_mapper[vname].c_str());
			return ;
		}
	}
	else{
        printf("MOD_IO:: not exist path : %s\n", vname); 
		return ;
    }
	memset(valueBuf, 0x00, 8);
	IOInterface::instance()->getDIO(&info, valueBuf, 8);
	printf("\t get_io_status_from_io_mananger:: %s : (", vname);
	for(int iRet = 0 ; iRet < 8 ; iRet++)
	{
		printf("%04X, ", valueBuf[iRet]);
	}
	printf(") \n");
	
	iValue = atoi(valueBuf);
#endif
	value.setFloatValue(iValue) ;
	return value;
}

int get_io_interface_status(char *vname)
{
	int iValue = -1;
#ifndef WIN32
	char valueBuf[8];
	IOPortInfo info;
	memset(&info, 0x00, sizeof(IOPortInfo));
	
	U64 result = IOInterface::instance()->checkIO(vname, &info);
	if (result != TPI_SUCCESS)
	{
		printf("IOInterface::instance()->checkIO Failed:: %s\n", vname);
		return ;
	}
	
	memset(valueBuf, 0x00, 8);
	IOInterface::instance()->getDIO(&info, valueBuf, 8);
	printf("\t get_io_interface_status:: %s : (", vname);
	for(int iRet = 0 ; iRet < 8 ; iRet++)
	{
		printf("%04X, ", valueBuf[iRet]);
	}
	printf(") \n");
	
	iValue = atoi(valueBuf);
#endif
	return iValue;
}

eval_value forgesight_get_io_status(char *name)
{	
	eval_value value;
	int  iValue = 0;
	char io_name[16] ;
	char io_idx[16] ;
	char io_key_buffer[16] ;

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

	if((!strcmp(io_name, TXT_AI)) || (!strcmp(io_name, TXT_AO)))
	{
		if (g_io_config_emulated.is_aio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(
				io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == 1) || (g_io_config_emulated.is_aio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_DI)) || (!strcmp(io_name, TXT_DO)))
	{
		if (g_io_config_emulated.is_dio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(
				io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_dio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(
				io_key_buffer, iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_RI)) || (!strcmp(io_name, TXT_RO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(
				io_key_buffer, iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_SI)) || (!strcmp(io_name, TXT_SO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(
				io_key_buffer, iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_UI)) || (!strcmp(io_name, TXT_UO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(
				io_key_buffer, iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	// Use actual value
	return get_io_status_from_io_mananger(name);
}

int forgesight_set_io_status(char *name, eval_value& valueStart)
{
	eval_value value;
	int iValue;
	char io_name[16] ;
	char io_idx[16] ;
	char io_key_buffer[16] ;

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
	
	if((!strcmp(io_name, TXT_AI)) || (!strcmp(io_name, TXT_AO)))
	{
		if (g_io_config_emulated.is_aio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_aio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(io_key_buffer, valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_DI)) || (!strcmp(io_name, TXT_DO)))
	{
		printf("set_io status: %s:%d (%s).\n", 
				io_name, iIOIdx, name);
		if (g_io_config_emulated.is_dio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_dio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(io_key_buffer, valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_RI)) || (!strcmp(io_name, TXT_RO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(io_key_buffer, valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_SI)) || (!strcmp(io_name, TXT_SO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(io_key_buffer, valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_UI)) || (!strcmp(io_name, TXT_UO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(io_key_buffer, iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
			memset(io_key_buffer, 0x00, 16);
			sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(io_key_buffer, valueStart);
			return 0;
		}
	}
	// Use io_mananger
	set_io_status_to_io_mananger(name, valueStart);
	return 0;

}

int forgesight_read_io_emulate_status(char * name, int& value)
{
	char io_name[16] ;
	char io_idx[16] ;
	char io_key_buffer[32] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	
	if(namePtr[0] == '_') {
		memset(io_key_buffer, 0x00, 32);
		sprintf(io_key_buffer, "%s.emltFlag", name);
		forgesight_io_config_get_value(io_key_buffer, value);
	    printf("forgesight_read_io_emulate_status: %s:%d .\n", io_key_buffer, (int)value);
		return 1 ;
	}
	else
//	if(namePtr[0] != '_') 
	{
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
	}
	memset(io_key_buffer, 0x00, 32);
	sprintf(io_key_buffer, "%s.emltFlag", name);
	forgesight_io_config_get_value(io_key_buffer, value);
    printf("forgesight_read_io_emulate_status: %s:%d .\n", io_key_buffer, (int)value);
	return 1;
}

int forgesight_mod_io_emulate_status(char * name, char value)
{
	eval_value valueSet;
	char io_name[16] ;
	char io_idx[16] ;
	char io_key_buffer[32] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	
	if(namePtr[0] == '_') {
		memset(io_key_buffer, 0x00, 32);
		sprintf(io_key_buffer, "%s.emltFlag", name);
		valueSet.setFloatValue((int)value);
		forgesight_io_config_set_value(io_key_buffer, valueSet);
	    printf("forgesight_mod_io_emulate_status: %s:%d .\n", io_key_buffer, (int)value);
		refresh_io_config_emulated();
		return 1 ;
	}
	else
//	if(namePtr[0] != '_') 
	{
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
	}
	memset(io_key_buffer, 0x00, 32);
	sprintf(io_key_buffer, "%s.emltFlag", name);
	valueSet.setFloatValue((int)value);
    printf("forgesight_mod_io_emulate_status: %s:%d .\n", io_key_buffer, (int)value);
	forgesight_io_config_set_value(io_key_buffer, valueSet);
	return 1;
}

int forgesight_mod_io_emulate_value(char * name, char value)
{
	eval_value valueSet;
	char io_name[16] ;
	char io_idx[16] ;
	char io_key_buffer[32] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	
//	if(namePtr[0] == '_') {
//		forgesight_io_config_get_value(name, value);
//		return 1 ;
//	}
//	else
	if(namePtr[0] != '_') 
	{
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
	}
	memset(io_key_buffer, 0x00, 32);
	sprintf(io_key_buffer, "%s.value", name);
	valueSet.setFloatValue((int)value);
    printf("forgesight_mod_io_emulate_value: %s:%d .\n", io_key_buffer, (int)value);
	forgesight_io_config_set_value(io_key_buffer, valueSet);
	return 1;
}



