/**
 * @file reg_interface.cpp
 * @brief 
 * @author Lujiaming
 * @version 1.0.0
 * @date 2018-04-12
 */
	 
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#include "common.h"
#include "error_code.h"
#include <boost/algorithm/string.hpp>
#endif

#include "macro_instr_mgr.h"
#define EMULATE_GENERATE_CONFIG

MacroInstrMgr::MacroInstrMgr()
{
	int result = 0 ;
#ifdef EMULATE_GENERATE_CONFIG
	int i = 0 ;
	result = generateEmptyJsonConfig();
    result = initial();
	
	macro_instr objMacroInstr ;
	for(i = 0; i < 10 ;i++)
	{	
		memset(&objMacroInstr, 0x00, sizeof(objMacroInstr));
		sprintf(objMacroInstr.program_name, "Program%d", i);
		sprintf(objMacroInstr.io_name     , "DI[%d]",    i);
		sprintf(objMacroInstr.macro_name  , "Macro%d",   i);
		AddSingleJsonConfig(objMacroInstr);
	}
	delSingleJsonConfig((char *)"Macro3");
	i = 7 ;
	sprintf(objMacroInstr.program_name, "FFFF%d",    i);
	sprintf(objMacroInstr.io_name     , "DI[%d]",    i);
	sprintf(objMacroInstr.macro_name  , "Macro%d",   i);
	updateSingleJsonConfig(objMacroInstr);
	printMacroInstrList();
#else
    result = initial();
#endif
    if (result != 0)
    {
        // rcs::Error::instance()->add(result);
    }
}

MacroInstrMgr::~MacroInstrMgr()
{
	if (macro_instr_root)
	{
		cJSON_Delete(macro_instr_root);
	}
	clearMacroInstrList();
}


int MacroInstrMgr::parseSingleMacroInstr(cJSON * item)
{
	macro_instr objMacroInstr ;
	memset(&objMacroInstr, 0x00, sizeof(objMacroInstr));
	strcpy(objMacroInstr.macro_name, item->string);

	cJSON *child=item->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); break;
		case cJSON_Number:
			break;
		case cJSON_String:
			if(strcmp(child->string, "MacroProgram") == 0)
				strcpy(objMacroInstr.program_name, child->valuestring);
			else if(strcmp(child->string, "IO") == 0)
				strcpy(objMacroInstr.io_name, child->valuestring);
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			printf("cJSON_Object\n");
			break;
		}
		child = child->next ;
	}
	addToMacroInstrList(objMacroInstr);
	return 1;
}

int MacroInstrMgr::parseAllMacroInstr(char * data)
{
	macro_instr_root = cJSON_Parse(data);
	if(macro_instr_root == NULL)
		return -1;
	
	cJSON *child=macro_instr_root->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); break;
		case cJSON_Number:
			break;
		case cJSON_String:
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			// printf("cJSON_Object\n");
			parseSingleMacroInstr(child);
			break;
		}
		child = child->next ;
	}
	return -1;
}


int MacroInstrMgr::parseMacroInstrFile(char *filename, char * progName)
{
//	int iCode = 0;
	FILE *f;long len;char *data;

	f=fopen(filename,"rb"); 
	if(f)
	{
	    fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
	    data=(char*)malloc(len+1); fread(data,1,len,f); 
		fclose(f);
		// iCode = 
		parseAllMacroInstr(data);
		free(data);
	}
	return 1;
}


int MacroInstrMgr::initial()
{
    char base[1000];
#ifndef WIN32
	parseMacroInstrFile(base, (char *)"/data/programs/filename");
#else
	parseMacroInstrFile("macro_instr_config.json", base);
#endif
	// printLaunchCodeList();
	return 0;
}
	
int MacroInstrMgr::updateAll()
{	
	if (macro_instr_root)
	{
		cJSON_Delete(macro_instr_root);
	}
	clearMacroInstrList();
    return initial();
}

/* use for generating data for test */
static void saveMacroTriggerJsonFile(cJSON * macroTrigger) // char * cContent)
{
	FILE * pFile;
	
	char * cContent = cJSON_Print(macroTrigger);
	// printf("Out : %s \n", cContent);
    if((pFile = fopen (MACRO_INSTR_CONFIG_JSON, "w"))==NULL)
    {
        printf("cant open the file");
        exit(0);
    }
	
    if(fwrite(cContent,strlen(cContent),1, pFile)!=1)
        printf("file write error\n");
    fclose (pFile);
}

int MacroInstrMgr::generateEmptyJsonConfig()
{
	cJSON*root = cJSON_CreateObject();
	saveMacroTriggerJsonFile(root);
	cJSON_Delete(root);
	return 1;
}

int MacroInstrMgr::AddSingleJsonConfig(macro_instr macroInstr)
{
	cJSON*obj_config = cJSON_CreateObject();
	
    cJSON* item = cJSON_CreateString(macroInstr.program_name);
    cJSON_AddItemToObject(obj_config, "MacroProgram", item);
	
    item = cJSON_CreateString(macroInstr.io_name);
    cJSON_AddItemToObject(obj_config,"IO",item);
	
	cJSON_AddItemToObject(macro_instr_root, 
						macroInstr.macro_name, obj_config);
	
	saveMacroTriggerJsonFile(macro_instr_root);
	addToMacroInstrList(macroInstr);
	return 1;
}

int MacroInstrMgr::delSingleJsonConfig(char* macro_name)
{
	if(macro_instr_root == NULL)
		return -1;
	
	cJSON *child=macro_instr_root->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); break;
		case cJSON_Number:
			break;
		case cJSON_String:
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			// printf("cJSON_Object\n");
			if(strcmp(child->string, macro_name) == 0)
			{	
				cJSON_DeleteItemFromObject(macro_instr_root, macro_name);
				saveMacroTriggerJsonFile(macro_instr_root);
				delFromMacroInstrList(macro_name);
				return 1;
			}
			break;
		}
		child = child->next ;
	}
	return 0;
}

int MacroInstrMgr::updateSingleJsonConfig(macro_instr macroInstr)
{
	if(macro_instr_root == NULL)
		return -1;
	
	cJSON *child=macro_instr_root->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); break;
		case cJSON_Number:
			break;
		case cJSON_String:
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			// printf("cJSON_Object\n");
			if(strcmp(child->string, macroInstr.macro_name) == 0)
			{	
				updateSingleMacroInstr(child, macroInstr);
				saveMacroTriggerJsonFile(macro_instr_root);
				updateMacroInstrList(macroInstr);
				return 1;
			}
			break;
		}
		child = child->next ;
	}
	return 0;
}

int MacroInstrMgr::updateSingleMacroInstr(cJSON * item, macro_instr macroInstr)
{
	cJSON *child=item->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			printf("cJSON_True"); break;
		case cJSON_Number:
			break;
		case cJSON_String:
			if(strcmp(child->string, "MacroProgram") == 0)
			{
				free(child->valuestring);
				child->valuestring = (char *)malloc(
						strlen(macroInstr.program_name) + 1);
				memcpy(child->valuestring, macroInstr.program_name, 
						strlen(macroInstr.program_name) + 1);
			}
			else if(strcmp(child->string, "IO") == 0)
			{
				free(child->valuestring);
				child->valuestring = (char *)malloc(
						strlen(macroInstr.io_name) + 1);
				memcpy(child->valuestring, macroInstr.io_name, 
						strlen(macroInstr.io_name) + 1);
			}
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			printf("cJSON_Object\n");
			break;
		}
		child = child->next ;
	}
	return 1;
}

/**
 *  This function would refresh all of IO Signal
 *  Delete the Signal which had not monitored
 *  Add the Signal which prepared to monitor
 *  Do nothing to the Signal which kept monitoring
 * @return: Program Name
 */
// 	int MacroInstrMgr::refreshAllIOConfig(std::map<std::string, int>& retMap)
// 	{
// 		return 1;
// 	}

std::string MacroInstrMgr::getProgramByMacroName(std::string strMacroName)
{
	std::map<std::string, macro_instr>::iterator it;
	it = macroInstrMgrList.find(strMacroName);
	if (it==macroInstrMgrList.end())
		return std::string("");
    else
		return macroInstrMgrList[strMacroName].program_name;
}

int MacroInstrMgr::printMacroInstrList()
{
	std::map<std::string, macro_instr>::iterator it;
	
	it = macroInstrMgrList.begin();
	
	while(it != macroInstrMgrList.end())
	{
		// it->first;  // it->second;
		printf("\t MacroInstrMgr: %s :: (%s, %s, %s) \n", 
			it->first.c_str(), it->second.macro_name, 
			it->second.program_name, it->second.io_name);
		it++;         
	}
	return 1;
}

void MacroInstrMgr::addToMacroInstrList(macro_instr objMacroInstr)
{
	io_macro_instr objIOMacroInstr;
	macroInstrMgrList.insert(std::pair<std::string, macro_instr>(
		std::string(objMacroInstr.macro_name), objMacroInstr));
	
	memcpy(objIOMacroInstr.macro_name, objMacroInstr.macro_name, 
			strlen(objMacroInstr.macro_name) + 1);
	memcpy(objIOMacroInstr.program_name, objMacroInstr.program_name, 
			strlen(objMacroInstr.program_name) + 1);
	// Default is pull up and wait for triggering 
	objIOMacroInstr.iValue = 1 ; 
	objIOMacroInstr.bIsRunning = false ;
	ioMgrList.insert(std::pair<std::string, io_macro_instr>(
		std::string(objMacroInstr.io_name), objIOMacroInstr));
}

bool MacroInstrMgr::getRunningInMacroInstrList(char* program_name)
{
	std::map<std::string, io_macro_instr>::iterator it;
	it = ioMgrList.begin();
	while(it != ioMgrList.end())
	{
		if(strcmp(it->second.program_name, program_name) == 0)
		{
			return it->second.bIsRunning;
		}
		it++;         
	}
	return false ;
}

void MacroInstrMgr::setRunningInMacroInstrList(char* program_name)
{
	std::map<std::string, io_macro_instr>::iterator it;
	it = ioMgrList.begin();
	while(it != ioMgrList.end())
	{
		if(strcmp(it->second.program_name, program_name) == 0)
		{
			it->second.bIsRunning = true;
		}
		it++;         
	}
}

void MacroInstrMgr::resetRunningInMacroInstrList(char* program_name)
{
	std::map<std::string, io_macro_instr>::iterator it;
	it = ioMgrList.begin();
	while(it != ioMgrList.end())
	{
		if(strcmp(it->second.program_name, program_name) == 0)
		{
			it->second.bIsRunning = false;
		}
		it++;         
	}
}

void MacroInstrMgr::updateMacroInstrList(macro_instr objMacroInstr)
{
	std::map<std::string, macro_instr>::iterator key
		= macroInstrMgrList.find(objMacroInstr.macro_name);
	if(key!=macroInstrMgrList.end())
	{
		macroInstrMgrList[objMacroInstr.macro_name] = objMacroInstr;
	}

	std::map<std::string, io_macro_instr>::iterator keyIO
		= ioMgrList.find(objMacroInstr.io_name);
	if(keyIO!=ioMgrList.end())
	{
		// Only update macro_name and program_name
		memcpy(macroInstrMgrList[objMacroInstr.io_name].macro_name, 
			objMacroInstr.macro_name, 
			strlen(objMacroInstr.macro_name) + 1);
		memcpy(macroInstrMgrList[objMacroInstr.io_name].program_name, 
			objMacroInstr.program_name, 
			strlen(objMacroInstr.program_name) + 1);
	}
}

void MacroInstrMgr::delFromMacroInstrList(char* macro_name)
{
	std::map<std::string, macro_instr>::iterator key
		= macroInstrMgrList.find(macro_name);
	if(key!=macroInstrMgrList.end())
	{
		std::map<std::string, io_macro_instr>::iterator keyIO
			= ioMgrList.find(key->second.io_name);
		if(keyIO!=ioMgrList.end())
		{
			ioMgrList.erase(keyIO);
		}
		macroInstrMgrList.erase(key);
	}
}

void MacroInstrMgr::clearMacroInstrList()
{
	macroInstrMgrList.clear();
	ioMgrList.clear();
}

