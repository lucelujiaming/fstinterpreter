// #include "stdafx.h"

#include<stdio.h>
#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include<string>
#include<vector>
#include<map>
#include <algorithm>
using namespace std;

#include "forsight_io_mapping.h"
#include "forsight_cJSON.h"
#include "forsight_inter_control.h"

#ifndef WIN32
#define stricmp     strcasecmp
#endif

typedef struct _IOMapJsonInfo
{
    int         from;
    int         index;
    char        module[128];
    int         to;
}IOMapJsonInfo;

typedef struct _IOMapVarInfo
{
    char        in[8];
    char        out[8];
}IOMapVarInfo;

map<string, string> g_io_mapper;

int generateIOInfo(IOMapJsonInfo &objInfo, char * strIOType)
{
	char cTemp[128];
	char cUpperType[16];
    char *pUpper;
	string strKey, strValue ;
	
    strcpy(cUpperType, strIOType);
	pUpper = cUpperType;
	while(*pUpper){ 
		*pUpper = toupper(*pUpper); 
		pUpper++; 
	}
  
	int iDIRange = objInfo.to - objInfo.from + 1 ;
	for (int i = objInfo.from ; i <= objInfo.to ; i++)
	{
		memset(cTemp, 0x00, 128);
		sprintf(cTemp, "%s[%d]", strIOType, i);
		strKey.assign(cTemp);
		
        vector<string> result=split(objInfo.module, "/"); //use space to split
		sprintf(cTemp, "root/IO/%s/%s/%s/%d", 
			result[0].c_str(), result[2].c_str(), cUpperType, 
			objInfo.index + i - objInfo.from);
		strValue.assign(cTemp);

		g_io_mapper.insert(
			map<string, string>::value_type(strKey, strValue));
		// Add revert element
		g_io_mapper.insert(
			map<string, string>::value_type(strValue, strKey));
	}
	return 1;
}

int parseIOObject(cJSON *jsonIObject, char * strIOType)
{
	IOMapJsonInfo objInfo ;
	int numentries=0,i=0,fail=0;
	cJSON *child=jsonIObject->child;
	
	// printf("parseDI: cJSON_Array %s\n", jsonIObject->string);
	while (child) 
	{
		numentries++; 
		// printf("parseIOObject: cJSON_Array %s\n", child->string);
		if(strcmp(child->string, "from") == 0)
		{
			objInfo.from = child->valueint ;
		}
		else if(strcmp(child->string, "index") == 0)
		{
			objInfo.index = child->valueint ;
		}
		else if(strcmp(child->string, "module") == 0)
		{
			strcpy(objInfo.module, child->valuestring) ;
		}
		else if(strcmp(child->string, "to") == 0)
		{
			objInfo.to = child->valueint ;
		}
		child=child->next;
	}
	
	generateIOInfo(objInfo, strIOType);
	return 1;
}

int parseIO(cJSON *jsonDI, char * strIOType)
{
	int numentries=0,i=0,fail=0;
	cJSON *child=jsonDI->child;
	while (child) 
		numentries++, child=child->next;

	child=jsonDI->child;
	while (child && !fail)
	{
		// printf("parseIO: cJSON_Array %s\n", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// printf("cJSON_Number %d\n", child->valueint); 
			break;
		case cJSON_String:	
			// printf("cJSON_String %s\n", child->valuestring); 
			break;
		case cJSON_Object:	
			// printf("cJSON_Object\n"); 
			parseIOObject(child, strIOType);
			break;
		}
		child=child->next;
	}
	return 1;
}

int parseIOMap(char * data, IOMapVarInfo &varInfo)
{
	cJSON *json;
	json=cJSON_Parse(data);
	if(json == NULL)
		return -1;
	
	cJSON *child=json->child;
	while(child)
	{
		switch ((child->type)&255)
		{
		case cJSON_True:	
			// printf("cJSON_True"); 
			break;
		case cJSON_Number:	
			// printf("cJSON_Number %d\n", child->valueint); 
			break;
		case cJSON_String:	
			// printf("cJSON_String %s\n", child->valuestring);
			break;
		case cJSON_Array:	
			{
				// printf("parseIOMap: cJSON_Array %s\n", child->string);
				if(stricmp(child->string, varInfo.in) == 0)
					parseIO(child, varInfo.in);
				else if(stricmp(child->string, varInfo.out) == 0)
					parseIO(child, varInfo.out);
				break;
			}
		case cJSON_Object:	
			// printf("cJSON_Object\n"); 
			break;
		}
		child = child->next ;
	}
	cJSON_Delete(json);
	return 1;
}

int print_io_mapper()
{
	map<string, string>::iterator it;

	it = g_io_mapper.begin();
	
	printf("\t\tobjThreadCntrolBlock->io_mapper has %d elements \n", 
			g_io_mapper.size());

	while(it != g_io_mapper.end())
	{
		// it->first;  // it->second;
		printf("\t\t%s :: %s \n", 
				it->first.c_str(), it->second.c_str());
		it++;         
	}
	return 1;
}

int append_single_io_mapper(
		char *filename, IOMapVarInfo &varInfo)
{
	FILE *f;long len;char *data;

	f=fopen(filename,"rb"); 
	if(f)
	{
	    fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
	    data=(char*)malloc(len+1); fread(data,1,len,f); 
		fclose(f);
		parseIOMap(data, varInfo);
		free(data);
	}
	return 1;
}

int append_io_mapping()
{
	IOMapVarInfo varInfo ;
	g_io_mapper.clear();
#ifdef WIN32
	// AI/AO
	strcpy(varInfo.in , "ai") ;  strcpy(varInfo.out , "ao") ;
	append_single_io_mapper("data\\io\\io_mapping\\ai_ao_mapping.json", varInfo);
	// DI/DO
	strcpy(varInfo.in , "di") ;  strcpy(varInfo.out , "do") ;
	append_single_io_mapper("data\\io\\io_mapping\\di_do_mapping.json", varInfo);
	// RI/RO
	strcpy(varInfo.in , "ri") ;  strcpy(varInfo.out , "ro") ;
	append_single_io_mapper("data\\io\\io_mapping\\ri_ro_mapping.json", varInfo);
	// SI/SO
	strcpy(varInfo.in , "si") ;  strcpy(varInfo.out , "so") ;
	append_single_io_mapper("data\\io\\io_mapping\\si_so_mapping.json", varInfo);
	// UI/UO
	strcpy(varInfo.in , "ui") ;  strcpy(varInfo.out , "uo") ;
	append_single_io_mapper("data\\io\\io_mapping\\ui_uo_mapping.json", varInfo);
    print_io_mapper();
#else
	// AI/AO
	strcpy(varInfo.in , "ai") ;  strcpy(varInfo.out , "ao") ;
	append_single_io_mapper("\/data\/io\/io_mapping\/ai_ao_mapping.json", varInfo);
	// DI/DO
	strcpy(varInfo.in , "di") ;  strcpy(varInfo.out , "do") ;
	append_single_io_mapper("\/data\/io\/io_mapping\/di_do_mapping.json", varInfo);
	// RI/RO
	strcpy(varInfo.in , "ri") ;  strcpy(varInfo.out , "ro") ;
	append_single_io_mapper("\/data\/io\/io_mapping\/ri_ro_mapping.json", varInfo);
	// SI/SO
	strcpy(varInfo.in , "si") ;  strcpy(varInfo.out , "so") ;
	append_single_io_mapper("\/data\/io\/io_mapping\/si_so_mapping.json", varInfo);
	// UI/UO
	strcpy(varInfo.in , "ui") ;  strcpy(varInfo.out , "uo") ;
	append_single_io_mapper("\/data\/io\/io_mapping\/ui_uo_mapping.json", varInfo);
#endif
	return 1;
}


