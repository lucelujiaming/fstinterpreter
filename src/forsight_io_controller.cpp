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
#else
#include<string>
#include<vector>
#include<map>
#endif
#include "forsight_cJSON.h"

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

#ifdef WIN32
#define IO_EMULATOR_JSON    "io_emulator.json"
#else
#define IO_EMULATOR_JSON    "share/configuration/configurable/io_emulator.json"
#endif

typedef struct  
{
	bool is_aio_emulated ;
	bool is_dio_emulated ;
	bool is_rio_emulated ;
	bool is_sio_emulated ;
	bool is_uio_emulated ;
} io_emulate_config;

cJSON * g_io_config_ = NULL;

io_emulate_config     g_io_config_emulated ;

void refresh_io_config_emulated();

/* use for generating data for test */
static void saveIOConfigToJsonFile(cJSON * ioConfig)
{
	FILE * pFile;
	
	char * cContent = cJSON_Print(ioConfig);
    if((pFile = fopen (IO_EMULATOR_JSON, "w"))==NULL)
    {
        printf("cant open the file");
        exit(0);
    }
	
    if(fwrite(cContent,strlen(cContent),1, pFile)!=1)
        printf("file write error\n");
    fclose (pFile);
}

int addSingleConfig(string name, eval_value& value)
{
	if(g_io_config_)
	{
		cJSON*obj_config = cJSON_CreateObject();
		cJSON* item=cJSON_CreateBool(1);
		cJSON_AddItemToObject(obj_config,"emltFlag",item);
		
		item=cJSON_CreateNumber((int)value.getFloatValue());
		cJSON_AddItemToObject(obj_config,"value",item);
		
		cJSON_AddItemToObject(g_io_config_, name.c_str(), obj_config);
		saveIOConfigToJsonFile(g_io_config_);
		return 1;
	}
	return 0;
}

int getSingleConfig(cJSON * item, string key, int& value)
{
	cJSON *child=item->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); 
			if(strcmp(child->string, key.c_str()) == 0)
			{
				if(strcmp(child->string, "emltFlag") == 0)
				{
					value = child->valueint;
					return 1;
				}
			}
			break;
		case cJSON_Number:
			if(strcmp(child->string, key.c_str()) == 0)
			{
				if(strcmp(child->string, "value") == 0)
				{
					value = child->valuedouble;
					return 1;
				}
			}
			break;
		case cJSON_String:
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			printf("cJSON_Object\n");
			break;
		}
		child = child->next ;
	}
	return 0;
}

int setSingleConfig(cJSON * item, string key, int& value)
{
	cJSON *child=item->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); 
			if(strcmp(child->string, key.c_str()) == 0)
			{
				if(strcmp(child->string, "emltFlag") == 0)
				{
					child->valueint = value;
					saveIOConfigToJsonFile(g_io_config_);
					return 1;
				}
			}
			break;
		case cJSON_Number:
			if(strcmp(child->string, key.c_str()) == 0)
			{
				if(strcmp(child->string, "value") == 0)
				{
					child->valuedouble = value;
					saveIOConfigToJsonFile(g_io_config_);
					return 1;
				}
			}
			break;
		case cJSON_String:
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			printf("cJSON_Object\n");
			break;
		}
		child = child->next ;
	}
	return 0;
}

int forgesight_io_config_get_value(string name, string key, int& value)
{
	value = DEFAULT_IO_VALUE ;
	if(g_io_config_)
	{
		cJSON *child=g_io_config_->child;
		while(child)
		{
			switch ((child->type)&255)
			{
			case cJSON_True:	
				printf("cJSON_True"); break;
			case cJSON_Number:
				break;
			case cJSON_String:
				if(strcmp(child->string, key.c_str()) == 0)
					value = child->valueint;
				break;
			case cJSON_Array:
				break;
			case cJSON_Object:	
				printf("cJSON_Object\n");
				if(strcmp(child->string, name.c_str()) == 0)
				{
					getSingleConfig(child, key, value);
					return 1;
				}
				break;
			}
			child = child->next ;
		}
	}
	return 0;
}

int forgesight_io_config_set_value(string name, string key, eval_value& value)
{
	int iVal  = 0 ;
	bool bIsFind = false ;
	cJSON *child=g_io_config_->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); break;
		case cJSON_Number:
			break;
		case cJSON_String:
			printf("cJSON_String"); break;
			break;
		case cJSON_Array:
			printf("cJSON_Array"); break;
			break;
		case cJSON_Object:	
			printf("cJSON_Object\n");
			if(strcmp(child->string, name.c_str()) == 0)
			{
				iVal = (int)value.getFloatValue();
				setSingleConfig(child, key, iVal);
				return 1;
			}
			break;
		}
		child = child->next ;
	}

	if(bIsFind == false)
	{
		addSingleConfig(name, value);
	}
	return 1;
}

int forgesight_reset_uio_config_values()
{
	char cTemp[32];
	eval_value value ;
	value.setFloatValue(0.0);

	for (int i = 0 ; i<= UI_UO_NUM ;i++)
	{
		memset(cTemp, 0x00, 32);
		sprintf(cTemp, "ui[%d]", i);
		forgesight_io_config_set_value(cTemp, "value", value);
		memset(cTemp, 0x00, 32);
		sprintf(cTemp, "uo[%d]", i);
		forgesight_io_config_set_value(cTemp, "value", value);
	}
	return 1;
}

void generate_fake_io_config()
{
	char cTemp[32];
	eval_value value ;
	value.setFloatValue(1.0);

	addSingleConfig("aio_all", value);
	addSingleConfig("dio_all", value);
	addSingleConfig("rio_all", value);
	addSingleConfig("sio_all", value);
	addSingleConfig("uio_all", value);
	for (int i = 0 ; i<= UI_UO_NUM ;i++)
	{
		memset(cTemp, 0x00, 32);
		sprintf(cTemp, "ui[%d]", i);
		addSingleConfig(cTemp, value);
		memset(cTemp, 0x00, 32);
		sprintf(cTemp, "uo[%d]", i);
		addSingleConfig(cTemp, value);
	}

}

int forgesight_load_io_config()
{	
	int iCode = 0;
	FILE *f;long len;char *data;
	
	f=fopen(IO_EMULATOR_JSON,"rb"); 
	if(f)
	{
	    fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
	    data=(char*)malloc(len+1); fread(data,1,len,f); 
		fclose(f);
		g_io_config_ = cJSON_Parse(data);
		if(g_io_config_ == NULL)
		{
			return -1;
		}
		// generate_fake_io_config();
		free(data);
	}
	refresh_io_config_emulated();
	return 1 ;
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
	forgesight_io_config_get_value("aio_all", "emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_aio_emulated = true ;
	else
		g_io_config_emulated.is_aio_emulated = false ;

	forgesight_io_config_get_value("dio_all", "emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_dio_emulated = true ;
	else
		g_io_config_emulated.is_dio_emulated = false ;
	
	forgesight_io_config_get_value("rio_all", "emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_rio_emulated = true ;
	else
		g_io_config_emulated.is_rio_emulated = false ;
	
	forgesight_io_config_get_value("sio_all", "emltFlag", iValue);
	if (iValue == SMLT_TRUE)
		g_io_config_emulated.is_sio_emulated = true ;
	else
		g_io_config_emulated.is_sio_emulated = false ;
	
	forgesight_io_config_get_value("uio_all", "emltFlag", iValue);
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

	if((!strcmp(io_name, TXT_AI)) || (!strcmp(io_name, TXT_AO)))
	{
		if (g_io_config_emulated.is_aio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == 1) || (g_io_config_emulated.is_aio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(name, "value", iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_DI)) || (!strcmp(io_name, TXT_DO)))
	{
		if (g_io_config_emulated.is_dio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_dio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(name, "value", iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_RI)) || (!strcmp(io_name, TXT_RO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(name, "value", iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_SI)) || (!strcmp(io_name, TXT_SO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(name, "value", iValue);
			value.setFloatValue(iValue);
			return value;
		}
	}
	else if((!strcmp(io_name, TXT_UI)) || (!strcmp(io_name, TXT_UO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_get_value(name, "value", iValue);
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
	
	if((!strcmp(io_name, TXT_AI)) || (!strcmp(io_name, TXT_AO)))
	{
		if (g_io_config_emulated.is_aio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_aio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(name, "value", valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_DI)) || (!strcmp(io_name, TXT_DO)))
	{
		printf("set_io status: %s:%d (%s).\n", 
				io_name, iIOIdx, name);
		if (g_io_config_emulated.is_dio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_dio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(name, "value", valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_RI)) || (!strcmp(io_name, TXT_RO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(name, "value", valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_SI)) || (!strcmp(io_name, TXT_SO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(name, "value", valueStart);
			return 0;
		}
	}
	else if((!strcmp(io_name, TXT_UI)) || (!strcmp(io_name, TXT_UO)))
	{
		if (g_io_config_emulated.is_rio_emulated == false)
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.emltFlag", name);
			forgesight_io_config_get_value(name, "emltFlag", iValue);
		}
		// Global emulation is on or single emulation is on
		if ((iValue == SMLT_TRUE) || (g_io_config_emulated.is_rio_emulated == true))
		{
		//	memset(io_key_buffer, 0x00, 16);
		//	sprintf(io_key_buffer, "%s.value", name);
			forgesight_io_config_set_value(name, "value", valueStart);
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
	// char io_key_buffer[32] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	
	if(namePtr[0] == '_') {
	//	memset(io_key_buffer, 0x00, 32);
	//	sprintf(io_key_buffer, "%s.emltFlag", name);
		forgesight_io_config_get_value(name, "emltFlag", value);
	    printf("forgesight_read_io_emulate_status: %s.emltFlag:%d .\n", name, (int)value);
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
	// memset(io_key_buffer, 0x00, 32);
	// sprintf(io_key_buffer, "%s.emltFlag", name);
	forgesight_io_config_get_value(name, "emltFlag", value);
    printf("forgesight_read_io_emulate_status: %s.emltFlag:%d .\n", name, (int)value);
	return 1;
}

int forgesight_mod_io_emulate_status(char * name, char value)
{
	eval_value valueSet;
	char io_name[16] ;
	char io_idx[16] ;
	// char io_key_buffer[32] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	
	if(namePtr[0] == '_') {
	//	memset(io_key_buffer, 0x00, 32);
	//	sprintf(io_key_buffer, "%s.emltFlag", name);
		valueSet.setFloatValue((int)value);
		forgesight_io_config_set_value(name, "emltFlag", valueSet);
	    printf("forgesight_mod_io_emulate_status: %s.emltFlag:%d .\n", name, (int)value);
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
	// memset(io_key_buffer, 0x00, 32);
	// sprintf(io_key_buffer, "%s.emltFlag", name);
	valueSet.setFloatValue((int)value);
    printf("forgesight_mod_io_emulate_status: %s.emltFlag:%d .\n", name, (int)value);
	forgesight_io_config_set_value(name, "emltFlag", valueSet);
	return 1;
}

int forgesight_mod_io_emulate_value(char * name, char value)
{
	eval_value valueSet;
	char io_name[16] ;
	char io_idx[16] ;
	// char io_key_buffer[32] ;

	int  iIOIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	memset(io_name, 0x00, 16);
	memset(io_idx, 0x00, 16);
	
	temp = io_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(io_name) ;
	
//	if(namePtr[0] == '_') {
//		forgesight_io_config_get_value(name, "emltFlag", value);
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
	// memset(io_key_buffer, 0x00, 32);
	// sprintf(io_key_buffer, "%s.value", name);
	valueSet.setFloatValue((int)value);
    printf("forgesight_mod_io_emulate_value: %s.value:%d .\n", name, (int)value);
	forgesight_io_config_set_value(name, "value", valueSet);
	return 1;
}



