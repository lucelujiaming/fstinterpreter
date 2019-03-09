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

#include<libxml/parser.h>
#include<libxml/tree.h>

#include "forsight_external_resource.h"
#include "forsight_cJSON.h"
#include "forsight_basint.h"
#include "forsight_inter_control.h"

#ifdef WIN32
#include "reg_manager/reg_manager_interface_wrapper.h"
#else
#include "reg_manager_interface_wrapper.h"
using namespace fst_ctrl ;
enum {MAX_REG_COMMENT_LENGTH = 32,};
#endif

#define POSE_NONE      0
#define POSE_CART      1
#define POSE_JOINT     2

bool check_type_and_size(KeyVariableType type, int size)
{
	switch(type)
	{
	case KEYTYPE_CHAR: 
		if(size != 1)
			return false ;
		else
			return true ;
    case KEYTYPE_INT16:
		if(size != 2)
			return false ;
		else
			return true ;
    case KEYTYPE_INT32:
		if(size != 4)
			return false ;
		else
			return true ;
    case KEYTYPE_INT64:
		if(size != 8)
			return false ;
		else
			return true ;
    case KEYTYPE_DOUBLE:
		if(size != 8)
			return false ;
		else
			return true ;
    case KEYTYPE_BYTEARRAY:
		return true ;
	default:
		return true ;
	}
	return true ;
}

// -------------------------------- libxml --------------------------------
bool generateKeyWord(xmlNodePtr nodeHead, key_variable& objKeyVariable)
{
    xmlNodePtr nodeElement;
    xmlChar *value;  
    char* str;
	
    for(nodeElement = nodeHead->children; 
			nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name,BAD_CAST"KeyName")==0){ 
			value = xmlNodeGetContent(nodeElement);
			strcpy(objKeyVariable.key_name, (char *)value);
			char *p;
			/* convert to lowercase */
			p = objKeyVariable.key_name;
			while(*p){ *p = tolower(*p); p++; }
        }
		else if(xmlStrcasecmp(nodeElement->name,BAD_CAST"BaseAddress")==0){ 
			value = xmlNodeGetContent(nodeElement);
			objKeyVariable.base_address = strtol((char *)value, &str, 16);
        }
		else if(xmlStrcasecmp(nodeElement->name,BAD_CAST"Type")==0){ 
			value = xmlNodeGetContent(nodeElement);
    
			if(xmlStrcasecmp(value, BAD_CAST"Char")==0){ 
				objKeyVariable.key_type = KEYTYPE_CHAR ;
	        }
			else if(xmlStrcasecmp(value, BAD_CAST"Int16")==0){ 
				objKeyVariable.key_type = KEYTYPE_INT16 ;
	        }
			else if(xmlStrcasecmp(value, BAD_CAST"Int32")==0){ 
				objKeyVariable.key_type = KEYTYPE_INT32 ;
	        }
			else if(xmlStrcasecmp(value, BAD_CAST"Long")==0){ 
				objKeyVariable.key_type = KEYTYPE_INT64 ;
	        }
			else if(xmlStrcasecmp(value, BAD_CAST"Double")==0){ 
				objKeyVariable.key_type = KEYTYPE_DOUBLE ;
	        }
			else if(xmlStrcasecmp(value, BAD_CAST"ByteArray")==0){ 
				objKeyVariable.key_type = KEYTYPE_BYTEARRAY ;
	        }
        }
		else if(xmlStrcasecmp(nodeElement->name,BAD_CAST"ByteSize")==0){ 
			value = xmlNodeGetContent(nodeElement);
			objKeyVariable.key_size = atoi((char *)value);
        }
    }
	
	// return true ;
	return check_type_and_size(objKeyVariable.key_type, objKeyVariable.key_size);
}

/************************************************* 
	Function:		append_program_prop_mapper
	Description:	Analyze the properity json file of program.
	Input:			thread_control_block  - interpreter info
	Input:			filename - program filename
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int import_external_resource(char *file_name,
				vector<key_variable>& vecKeyVariables)
{
	key_variable objKeyVariable;
	char fileName[LAB_LEN];
	char filePath[LAB_LEN];
	char headPath[LAB_LEN];
    xmlDocPtr doc;
    xmlNodePtr rootProg ;
    xmlNodePtr nodeHead, nodeProgBody ;
	
	vecKeyVariables.clear();
#ifdef WIN32
	TCHAR pBuf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pBuf);
    sprintf(fileName, "%s\\%s", pBuf, file_name);
	strcpy(filePath, pBuf);
#else
	char * fileNameStartPtr = strrchr(file_name, '/');
	if (fileNameStartPtr == NULL)
		fileNameStartPtr = fileName ;
	else 
		fileNameStartPtr++ ;
    strcpy(fileName, file_name);
	memcpy(filePath, file_name, 
		fileNameStartPtr - file_name);
#endif

    // doc=xmlParseMemory(buf,len);    //parse xml in memory
    doc = xmlReadFile(fileName, "UTF-8", XML_PARSE_RECOVER);
    if(doc == NULL){
        FST_ERROR("\t\t\t\t ERROR: doc == null\n");
        return -1;
    }
    rootProg = xmlDocGetRootElement(doc);

    // generate Include File
    for(nodeHead = rootProg->children; 
            nodeHead; nodeHead = nodeHead->next){
        if(xmlStrcasecmp(nodeHead->name,BAD_CAST"head")==0)
        {
#ifdef WIN32
			sprintf(headPath, "%s\\%s.xml", filePath, 
			 		(char *)xmlGetNodePath(nodeHead));
#else
			sprintf(headPath, "%s/%s.xml", filePath, 
			 		(char *)xmlGetNodePath(nodeHead));
#endif
            import_external_resource(headPath, vecKeyVariables);
            break;
        }
    }

	// generate prog_body 
    for(nodeProgBody = rootProg->children; 
		nodeProgBody; nodeProgBody = nodeProgBody->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeProgBody->name);
        if(xmlStrcasecmp(nodeProgBody->name,BAD_CAST"KeyWord")==0)
        {
			memset(&objKeyVariable, 0x00, sizeof(key_variable));
			bool bRet = generateKeyWord(nodeProgBody, objKeyVariable);
			if(bRet)
			{
				if(strstr(REGSITER_NAMES, objKeyVariable.key_name))
				{
					FST_INFO("Duplicate %s", objKeyVariable.key_name);
				}
				else
				{
					vecKeyVariables.push_back(objKeyVariable);
				}
			}
        }
    }
    xmlFreeDoc(doc);
//	xmlCleanupParser();
    return 0;
}



