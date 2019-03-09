// xmlReader.cpp : Defines the entry point for the console application.
// gcc -o xmlReader xmlReader.cpp  -I /usr/local/include/libxml2/ -L /usr/local/lib -lxml2  -std=c++11 -lsupc++ -lstdc++

// #include "stdafx.h"

#include<stdio.h>
#include<string.h>
#include<libxml/parser.h>
#include<libxml/tree.h>
#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#include<vector>
#include <algorithm>
using namespace std;

#include "forsight_basint.h"
#include "forsight_xml_reader.h"

#define PROG_HEAD         "head"
#define LINE_LAB_LEN      256
#define FILE_PATH_LEN     1024

typedef enum _WaitType
{
    WAIT_NONE, 
	WAIT_TIME, 
	WAIT_COND ,
	WAIT_COND_OUTTIME ,
} WaitType;

typedef struct label_t {
	char name[LINE_LAB_LEN];
} Label;

typedef struct line_info_t {
	int  indentValue ;
	char xPath[LINE_LAB_LEN];
	int  xPathIdx;
	char fileName[FILE_PATH_LEN];
} LineInfo;

int g_lineNum = 0 ;
char   g_xml_file_name[FILE_PATH_LEN];
char g_xpath_file_name[FILE_PATH_LEN];
char g_basic_file_name[FILE_PATH_LEN];
char g_mix_file_name[FILE_PATH_LEN];

int generateFunctionBody(xmlNodePtr nodeFunctionBody, LineInfo objLineInfo);
int generateFunctionCall(xmlNodePtr nodeFunctionCall, LineInfo objLineInfo);
int generateElementStr(xmlNodePtr nodeValueElement, LineInfo objLineInfo, char * label_str);

/************************************************* 
	Function:		exportBASCode
	Description:	Append one line .
	Input:			objLineInfo      - Line Info
	Input:			format           - Line format string, such as "%s "
	Input:			value            - Line content
	Return: 		1 - success
*************************************************/ 
int printBASCode(LineInfo objLineInfo, char *format, char * value)
{
	FILE * fpMix   = fopen(g_mix_file_name,"a");
	// FST_INFO(format, value);
	fprintf(fpMix, format, value);
	fclose(fpMix);
	
	FILE * fpBasic = fopen(g_basic_file_name,"a");
	// FST_INFO(format, value);
	fprintf(fpBasic, format, value);
	fclose(fpBasic);
	return 1;
}

#define PRINT_XPATH 
// /bookstore/book[1]	选取属于 bookstore 子元素的第一个 book 元素。
/************************************************* 
	Function:		exportBASCode
	Description:	Output one line basic code .
	Input:			objLineInfo      - Line Info
	Input:			title            - Line title, such as "EXPORT:"
	Input:			format           - Line format string, such as "%s "
	Input:			value            - Line content
	Return: 		1 - success
*************************************************/ 
int exportBASCode(LineInfo objLineInfo, char *title, char *format, char * value)
{
	int iPathLen = 0 ;
	FILE * fpMix   = fopen(g_mix_file_name,"a");
	FILE * fpXPath = fopen(g_xpath_file_name,"a");
	FILE * fpBasic = fopen(g_basic_file_name,"a");

	// FST_INFO(title);
	fprintf(fpMix,"%s", title);
	
	// FST_INFO("(%03d)", g_lineNum);
	fprintf(fpMix,"(%03d)", g_lineNum);
	fprintf(fpXPath,"%03d:%s:", g_lineNum, objLineInfo.fileName);

#ifdef PRINT_XPATH
	if(strlen(title) > 0)
	{
		iPathLen =  strlen(objLineInfo.xPath) ;
		// FST_INFO("(%03d)%s", iPathLen, objLineInfo.xPath);
		fprintf(fpMix,"(%03d)%s", iPathLen, objLineInfo.xPath);
		fprintf(fpXPath,"%s\n", objLineInfo.xPath);
		for(int i = 100 ; i > iPathLen; i--)
		{
			// FST_INFO(" ");
			fprintf(fpMix, " ");
		}
	}
#endif
	for(int i = 0 ; i < objLineInfo.indentValue ; i++)
	{
		// FST_INFO("    ");
		fprintf(fpMix, "    ");
	    fprintf(fpBasic, "    ");
	}
	// FST_INFO(format, value);
	fprintf(fpMix, format, value);
	fprintf(fpBasic, format, value);
	fclose(fpMix);
	fclose(fpXPath);
	fclose(fpBasic);
	g_lineNum++ ;
	return 1;
}

/************************************************* 
	Function:		generateElementStr
	Description:	parse <head> node and output basic code .
	Input:			nodehead         - <head> node
	Input:			objLineInfo      - Line Info
	Return: 		1 - success
*************************************************/ 
int generateIncludeFile(xmlNodePtr nodehead, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeInclude ;
    xmlChar *value;
    for(nodeInclude = nodehead->children; 
		nodeInclude; nodeInclude = nodeInclude->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeInclude->name);
        if(xmlStrcasecmp(nodeInclude->name,BAD_CAST"include")==0){ 
			value=xmlNodeGetContent(nodeInclude);
			sprintf(objLineInfoTemp.xPath, "%s", (char *)xmlGetNodePath(nodeInclude));
            exportBASCode(objLineInfoTemp, (char *)"EXPORT: ", (char *)"IMPORT %s\n", (char*)value);
        }
    }
	return 1 ;
}

/************************************************* 
	Function:		serializeFunctionParam
	Description:    generate parameters of function .
	Input:			labels         - parameter vector
	Output:			result         - Output String as parse result
	Return: 		1 - success
*************************************************/ 
void serializeFunctionParam(vector<Label> labels, char * result)
{
	if(labels.size()== 0)        // No Param
	{
		return ;
	}
	else if(labels.size() == 1)  // One Param
	{
		sprintf(result, "%s", labels[0].name);
		return ;
	}
	else                        // One Param
	{
		sprintf(result, "%s", labels[0].name);
		for(int i = 1 ; i< labels.size() ; i++)
			sprintf(result, "%s, %s", result, labels[i].name);
	}
}

/************************************************* 
	Function:		generateElementStr
	Description:	parse <element> node .
	Input:			nodeValueElement         - <element> node
	Input:			objLineInfo              - Line Info
	Output:			label_str                - Output String as parse result
	Return: 		1 - success
*************************************************/ 
int generateElementStr(xmlNodePtr nodeValueElement, LineInfo objLineInfo, char * label_str)
{
    char label_output[1024];
    char label_elemnt[128];
    Label labelParam ;
	vector<Label> label_vector;

	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeSubValueElement ;
    xmlChar *name, *file, *type, *value;
	value = xmlNodeGetContent(nodeValueElement);
			// FST_INFO("\t\t\t\t  --debug-- (nodeValueElement) %s = %s\n", 
			//		(char*)nodeValueElement->name, (char *)value);
		
	if(xmlStrcasecmp(nodeValueElement->name, BAD_CAST"element")==0){ 
	    name = xmlGetProp(nodeValueElement,BAD_CAST"type");
		// FST_INFO("%s, ", (char*)nodeStatement->name);
		if(xmlStrcasecmp(name, BAD_CAST"num")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"param")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"operation")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			if(xmlStrcasecmp(value, BAD_CAST"MOD")==0)
			{
				sprintf(label_str, "%s %% ", label_str);
			}
			else if(xmlStrcasecmp(value, BAD_CAST"DIV")==0)
			{
				sprintf(label_str, "%s @ ", label_str);
			}
			else
				sprintf(label_str, "%s%s", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"pose_element")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"boolean_operation")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s %s ", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"system")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"entity")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s", label_str, "");
			
			for(nodeSubValueElement = nodeValueElement->children; 
				nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
					objLineInfoTemp.indentValue = objLineInfo.indentValue ;
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_str);
			}
			sprintf(label_str, "%s", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"bracket")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s( ", label_str, "");
			
			for(nodeSubValueElement = nodeValueElement->children; 
				nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
					objLineInfoTemp.indentValue = objLineInfo.indentValue ;
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_str);
			}
			sprintf(label_str, "%s )", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"function")==0){ 
			name = xmlGetProp(nodeValueElement,BAD_CAST"name");
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s(", label_str, (char*)name);
			
// 			for(nodeSubValueElement = nodeValueElement->children; 
// 				nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
// 				generateElementStr(nodeSubValueElement, objLineInfoTemp, label_str);
// 			}
// 			sprintf(label_str, "%s) ", label_str);
			
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				// generateElement(nodeSubValueElement, objLineInfoTemp);
				value = xmlNodeGetContent(nodeSubValueElement);
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					memset(&labelParam, 0x00, sizeof(labelParam));
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					strcpy(labelParam.name, (char*)label_output);
					label_vector.push_back(labelParam);
				}
			}
			// printBASCode(objLineInfo, ") ", "");
			memset(label_output, 0x00, 1024);
			serializeFunctionParam(label_vector, label_output);
			sprintf(label_str, "%s%s)", label_str, label_output);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"argument")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s", label_str, "");
			
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				objLineInfoTemp.indentValue = objLineInfo.indentValue ;
				generateElementStr(nodeSubValueElement, objLineInfoTemp, label_str);
			}
			sprintf(label_str, "%s", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"subroutine")==0){ 
			file = xmlGetProp(nodeValueElement, BAD_CAST"file");
			name = xmlGetProp(nodeValueElement, BAD_CAST"name");

			value = xmlNodeGetContent(nodeValueElement);
			// FST_INFO(label_str, "%s( ", (char*)name);

			if(xmlStrlen(file)==0){        // Inside function
				memset(label_output, 0x00, 1024);
				sprintf(label_output, "%s::%s", objLineInfo.fileName, (char*)name);
				sprintf(label_str, "%s%s(", label_str, label_output);
			}
			else if(xmlStrlen(name)==0){   // Outside main
				memset(label_output, 0x00, 1024);
				sprintf(label_output, "%s::main", (char*)file);
				sprintf(label_str, "%s%s(", label_str, label_output);
			}
			else {                            // Outside function
				memset(label_output, 0x00, 1024);
				sprintf(label_output, "%s::%s", (char*)file, (char*)name);
				sprintf(label_str, "%s%s(", label_str, label_output);
			}
			
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				// generateElement(nodeSubValueElement, objLineInfoTemp);
				value = xmlNodeGetContent(nodeSubValueElement);
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					memset(&labelParam, 0x00, sizeof(labelParam));
					// strcpy(labelParam.name, (char*)value);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					strcpy(labelParam.name, (char*)label_output);
					label_vector.push_back(labelParam);
				}
			}
			// printBASCode(objLineInfo, ") ", "");
			memset(label_output, 0x00, 1024);
			serializeFunctionParam(label_vector, label_output);
			sprintf(label_str, "%s%s)", label_str, label_output);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"pose")==0){
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%sP[%s] ", label_str, (char*)label_output);
				}
			}
		}
		else if(xmlStrcasecmp(name, BAD_CAST"pose_register")==0){
			int iCount = 0 ;
			memset(label_elemnt, 0x00, 128);
			sprintf(label_str, "%sPR[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					type = xmlGetProp(nodeSubValueElement, BAD_CAST"type");
					if(xmlStrcasecmp(type, BAD_CAST"param")==0){
						memset(label_elemnt, 0x00, 128);
						generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
						sprintf(label_elemnt, "%s", (char*)label_output);
					}
					else if(xmlStrcasecmp(type, BAD_CAST"pose_element")==0)
					{
						memset(label_output, 0x00, 1024);
						generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
						sprintf(label_elemnt, "PJ%s", (char*)label_output);
					}
					else 
					{
						memset(label_output, 0x00, 1024);
						generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
						sprintf(label_str, "%s%s", label_str, (char*)label_output);
					}
					iCount++;
				}
			}
			if(strlen(label_elemnt) == 0)
				sprintf(label_str, "%s] ", label_str);
			else
				sprintf(label_str, "%s].%s ", label_str, label_elemnt);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"home_register")==0){ 
			int iCount = 0 ;
			memset(label_elemnt, 0x00, 128);
			sprintf(label_str, "%sHR[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					type = xmlGetProp(nodeSubValueElement, BAD_CAST"type");
					if(xmlStrcasecmp(type, BAD_CAST"param")==0){
						memset(label_elemnt, 0x00, 128);
						generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
						sprintf(label_elemnt, "%s", (char*)label_output);
					}
					else if(xmlStrcasecmp(type, BAD_CAST"pose_element")==0)
					{
						memset(label_output, 0x00, 1024);
						generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
						sprintf(label_elemnt, "HJ%s", (char*)label_output);
					}
					else 
					{
						memset(label_output, 0x00, 1024);
						generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
						sprintf(label_str, "%s%s", label_str, (char*)label_output);
					}
					iCount++;
				}
			}
			if(strlen(label_elemnt) == 0)
				sprintf(label_str, "%s] ", label_str);
			else
				sprintf(label_str, "%s].%s ", label_str, label_elemnt);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"motion_register")==0){
			sprintf(label_str, "%sMR[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
					nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%s%s", label_str, (char*)label_output);
				}
			}
			sprintf(label_str, "%s] ", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"register")==0){ 
			sprintf(label_str, "%sR[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
					nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%s%s", label_str, (char*)label_output);
				}
			}
			sprintf(label_str, "%s] ", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"string_register")==0){
			sprintf(label_str, "%sSR[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
					nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%s%s", label_str, (char*)label_output);
				}
			}
			sprintf(label_str, "%s] ", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"modbus_input_register")==0){ 
			sprintf(label_str, "%sMI[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
					nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%s%s", label_str, (char*)label_output);
				}
			}
			sprintf(label_str, "%s] ", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"modbus_holding_register")==0){ 
			sprintf(label_str, "%sMH[", label_str); 
			for(nodeSubValueElement = nodeValueElement->children; 
					nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%s%s", label_str, (char*)label_output);
				}
			}
			sprintf(label_str, "%s] ", label_str);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"timer")==0){  
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					//	value = xmlNodeGetContent(nodeSubValueElement);
					memset(label_output, 0x00, 1024);
					generateElementStr(nodeSubValueElement, objLineInfoTemp, label_output);
					sprintf(label_str, "%sTIMER[%s] ", label_str, (char*)label_output);
				}
			}
		}
		else if(xmlStrcasecmp(name, BAD_CAST"timer_status")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s", label_str, (char*)value);
		}
		else if(  (xmlStrcasecmp(name, BAD_CAST"ai")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"ao")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"di")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"do")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"ri")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"ro")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"si")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"so")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"ui")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"uo")==0) )  
		{ 
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeSubValueElement);
					sprintf(label_str, "%s%s[%s] ", label_str, name, (char*)value);
					// toupper
					string strRet = string(label_str);
					transform(strRet.begin(), strRet.end(), strRet.begin(), ::toupper);  
					sprintf(label_str, "%s", strRet.c_str());
				}
			}
		}
		else if(xmlStrcasecmp(name, BAD_CAST"io_val")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			sprintf(label_str, "%s%s ", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"string")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			// printBASCode(objLineInfo, " \"%s\" ", (char*)value);
			sprintf(label_str, "%s\"%s\"", label_str, (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"command")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, " %s ", (char*)value);
		}
		else { 
			FST_ERROR("Wrong Type (%s) in element. \n", (char *)name);
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generatePoseRegElement
	Description:	Wapper: call generateElementStr.
	Input:			nodeValueElement         - <element> node
	Input:			objLineInfo              - Line Info
	Return: 		1 - success
*************************************************/ 
int generateElement(xmlNodePtr nodeValueElement, LineInfo objLineInfo)
{
    char label_params[1024];
	memset(label_params, 0x00, 1024);
	generateElementStr(nodeValueElement, objLineInfo, label_params);
	if(strcmp(label_params, "Overall_Velocity_Coefficient") == 0)
		printBASCode(objLineInfo, "%s", "OVC.");
	else
		printBASCode(objLineInfo, "%s", (char*)label_params);
	return 1 ;
}

void ltrim ( char *s )
{
    char *p;
    p = s;
    while ( *p == ' ' || *p == '\t' ) {*p++;}
    strcpy ( s,p );
}

void rtrim ( char *s )
{
    int i;
	
    i = strlen ( s )-1;
    while ( ( s[i] == ' ' || s[i] == '\t' ) && i >= 0 ) {i--;};
    s[i+1] = '\0';
}

void trim ( char *s )
{
    ltrim ( s );
    rtrim ( s );
}

/************************************************* 
	Function:		generatePoseRegElement
	Description:	parse <pose_register> node and output basic code.
	Input:			nodeAssignmentStatement  - <pose_register> node
	Input:			objLineInfo              - Line Info
	Return: 		1 - success
*************************************************/ 
int generatePoseRegElement(xmlNodePtr nodeInstructionParam, LineInfo objLineInfo)
{
    xmlNodePtr nodeElement;
    char label_params[1024];
    char label_param[32];
	memset(label_params, 0x00, 1024);
	
	for(nodeElement = nodeInstructionParam->children; 
			nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name, BAD_CAST"element")==0){
			//	value = xmlNodeGetContent(nodeElement);
			//	printBASCode(objLineInfo, "%s ", (char*)value);
			memset(label_param, 0x00, 32);
			generateElementStr(nodeElement, objLineInfo, label_param);
			sprintf(label_params, "%s%s ", label_params, label_param);
		}
	}
    trim(label_params);
	printBASCode(objLineInfo, "PR[%s]", (char*)label_params);
	return 1 ;
}

/************************************************* 
	Function:		generatePoseElement
	Description:	parse <pose> node and output basic code.
	Input:			nodeAssignmentStatement  - <pose> node
	Input:			objLineInfo              - Line Info
	Return: 		1 - success
*************************************************/ 
int generatePoseElement(xmlNodePtr nodeInstructionParam, LineInfo objLineInfo)
{
    xmlNodePtr nodeElement;
    char label_params[1024];
    char label_param[32];
	memset(label_params, 0x00, 1024);
	
	for(nodeElement = nodeInstructionParam->children; 
		nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name, BAD_CAST"element")==0){
			//	value = xmlNodeGetContent(nodeElement);
			//	printBASCode(objLineInfo, "%s ", (char*)value);
			memset(label_param, 0x00, 32);
			generateElementStr(nodeElement, objLineInfo, label_params);
			sprintf(label_params, "%s%s ", label_params, label_param);
		}
	}
    trim(label_params);
	printBASCode(objLineInfo, "P[%s]", (char*)label_params);
	return 1 ;
}

int generateElementOld(xmlNodePtr nodeValueElement, LineInfo objLineInfo)
{
    char label_params[1024];
    Label labelParam ;
	vector<Label> label_vector;

	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeSubValueElement ;
    xmlChar *name, *file, *value;
	value = xmlNodeGetContent(nodeValueElement);
			// FST_INFO("\t\t\t\t  --debug-- (nodeValueElement) %s = %s\n", 
			//		(char*)nodeValueElement->name, (char *)value);
		
	if(xmlStrcasecmp(nodeValueElement->name, BAD_CAST"element")==0){ 
	    name = xmlGetProp(nodeValueElement,BAD_CAST"type");
		// FST_INFO("%s, ", (char*)nodeStatement->name);
		if(xmlStrcasecmp(name, BAD_CAST"num")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, "%s ", (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"operation")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, "%s ", (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"boolean_operation")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, "%s ", (char*)value);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"bracket")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, "( ", "");
			
			for(nodeSubValueElement = nodeValueElement->children; 
				nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
					objLineInfoTemp.indentValue = objLineInfo.indentValue ;
					generateElement(nodeSubValueElement, objLineInfoTemp);
			}
			printBASCode(objLineInfo, ") ", "");
		}
		else if(xmlStrcasecmp(name, BAD_CAST"function")==0){ 
			name = xmlGetProp(nodeValueElement,BAD_CAST"name");
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, "%s( ", (char*)name);
			
			for(nodeSubValueElement = nodeValueElement->children; 
				nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				generateElement(nodeSubValueElement, objLineInfoTemp);
			}
			printBASCode(objLineInfo, ") ", "");
		}
		else if(xmlStrcasecmp(name, BAD_CAST"subroutine")==0){ 
			file = xmlGetProp(nodeValueElement, BAD_CAST"file");
			name = xmlGetProp(nodeValueElement, BAD_CAST"name");

			value = xmlNodeGetContent(nodeValueElement);
			// printBASCode(objLineInfo, "%s( ", (char*)name);

			if(xmlStrlen(file)==0){        // Inside function
				memset(label_params, 0x00, 1024);
				sprintf(label_params, "%s::%s", objLineInfo.fileName, (char*)name);
				printBASCode(objLineInfo, "%s(", label_params);
			}
			else if(xmlStrlen(name)==0){   // Outside main
				memset(label_params, 0x00, 1024);
				sprintf(label_params, "%s::main", (char*)file);
				printBASCode(objLineInfo, "%s(", label_params);
			}
			else {                            // Outside function
				memset(label_params, 0x00, 1024);
				sprintf(label_params, "%s::%s", (char*)file, (char*)name);
				printBASCode(objLineInfo, "%s(", label_params);
			}
			
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				// generateElement(nodeSubValueElement, objLineInfoTemp);
				value = xmlNodeGetContent(nodeSubValueElement);
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					memset(&labelParam, 0x00, sizeof(labelParam));
					strcpy(labelParam.name, (char*)value);
					label_vector.push_back(labelParam);
				}
			}
			// printBASCode(objLineInfo, ") ", "");
			memset(label_params, 0x00, 1024);
			serializeFunctionParam(label_vector, label_params);
			printBASCode(objLineInfo, "%s)", label_params);
		}
		else if(xmlStrcasecmp(name, BAD_CAST"register")==0){ 
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeSubValueElement);
					printBASCode(objLineInfo, "R[%s] ", (char*)value);
				}
			}
		}
		else if(xmlStrcasecmp(name, BAD_CAST"di")==0){ 
			for(nodeSubValueElement = nodeValueElement->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
				if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeSubValueElement);
					printBASCode(objLineInfo, "DI[%s] ", (char*)value);
				}
			}
		}
		else if(xmlStrcasecmp(name, BAD_CAST"io_val")==0){ 
			value = xmlNodeGetContent(nodeValueElement);
			printBASCode(objLineInfo, "%s ", (char*)value);
		}
		else { 
			FST_ERROR("Wrong Type (%s) in element. \n", (char *)name);
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generateAssignment
	Description:	parse <assignment> node and output basic code.
	Input:			nodeAssignmentStatement  - <assignment> node
	Input:			objLineInfo              - Line Info
	Return: 		1 - success
*************************************************/ 
int generateAssignment(xmlNodePtr nodeAssignmentStatement, LineInfo objLineInfo)
{
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeLeftValue ;
    xmlNodePtr nodeRightValue ;
    xmlChar *value;
	
	// sprintf(currentChildPath, "%s", currentXPath);
	// exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"", (char *)"");
    for(nodeStatement = nodeAssignmentStatement->children; 
			nodeStatement; nodeStatement = nodeStatement->next){
						value = xmlNodeGetContent(nodeStatement);
						//	FST_INFO("\t\t\t\t  --debug-- (nodeStatement) %s = %s\n", 
						//		(char*)nodeStatement->name, (char *)value);
        if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"lvalue")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeLeftValue = nodeStatement->children; 
				nodeLeftValue; nodeLeftValue = nodeLeftValue->next){
					value = xmlNodeGetContent(nodeLeftValue);
					//		FST_INFO("\t\t\t\t  --debug-- (nodeLeftValue) %s = %s\n", 
					//		(char*)nodeLeftValue->name, (char *)value);
					generateElement(nodeLeftValue, objLineInfo);
			}
			printBASCode(objLineInfo, " = ", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"rvalue")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeRightValue = nodeStatement->children; 
				nodeRightValue; nodeRightValue = nodeRightValue->next){
				value = xmlNodeGetContent(nodeRightValue);
						//		FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", 
						//			(char*)nodeRightValue->name, (char *)value);
						
				if(xmlStrcasecmp(nodeRightValue->name, BAD_CAST"element")==0){ 
					generateElement(nodeRightValue, objLineInfo);
					printBASCode(objLineInfo, " ", "");
				}
			}
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in Assignment. \n", (char *)nodeStatement->name);
		}
    }
	return 1 ;
}

/************************************************* 
	Function:		generateOffsetPR
	Description:	parse <tool_offset_pr/offset_pr> node and output basic code.
	Input:			nodeInstructionParam  - <tool_offset_pr/offset_pr> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateOffsetPR(xmlNodePtr nodeInstructionParam, LineInfo objLineInfo)
{
    char label_uf[32];
    xmlNodePtr nodeElement;
    xmlChar *name, *type, *value;
	name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
	type = xmlGetProp(nodeInstructionParam, BAD_CAST"type");
	memset(label_uf, 0x00, 32);
	if(xmlStrcasecmp(type, BAD_CAST"with_uf")==0)
		strcpy(label_uf, "_UF");
	
	if(xmlStrcasecmp(name, BAD_CAST"offset_pr")==0){
		printBASCode(objLineInfo, ";OFFSET PR%s ", label_uf);
	}
	else if(xmlStrcasecmp(name, BAD_CAST"tool_offset_pr")==0){
		printBASCode(objLineInfo, ";TOOL_OFFSET PR%s ", label_uf);
	}
	
	for(nodeElement = nodeInstructionParam->children; 
	nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name, BAD_CAST"argument")==0){
			name = xmlGetProp(nodeElement, BAD_CAST"name");
			value = xmlNodeGetContent(nodeElement);
			
			if(xmlStrcasecmp(name, BAD_CAST"pose")==0){
				printBASCode(objLineInfo, "PR[%s] ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"uf")==0){
				printBASCode(objLineInfo, "UF[%s]", (char*)value);
			}
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generateOffsetVEC
	Description:	parse <offset_XXX> node and output basic code.
	Input:			nodeInstructionParam  - <offset_XXX> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateOffsetVEC(xmlNodePtr nodeInstructionParam, LineInfo objLineInfo)
{
    xmlNodePtr nodeElement;
    char label_uf[32];
    char label_params[1024];
	Label labelParam ;
	vector<Label> label_vector;
    xmlChar *name, *type, *value;
	
	name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
	type = xmlGetProp(nodeInstructionParam, BAD_CAST"type");
	memset(label_uf, 0x00, 32);
	if(xmlStrcasecmp(type, BAD_CAST"with_uf")==0)
		strcpy(label_uf, "_UF");

	if(xmlStrcasecmp(name, BAD_CAST"offset_cvec")==0){
		printBASCode(objLineInfo, ";OFFSET C_VEC%s (", label_uf);
	}
	else if(xmlStrcasecmp(name, BAD_CAST"offset_jvec")==0){
		printBASCode(objLineInfo, ";OFFSET J_VEC%s (", label_uf);
	}
	else if(xmlStrcasecmp(name, BAD_CAST"tool_offset_cvec")==0){
		printBASCode(objLineInfo, ";TOOL_OFFSET C_VEC%s (", label_uf);
	}
	else if(xmlStrcasecmp(name, BAD_CAST"tool_offset_jvec")==0){
		printBASCode(objLineInfo, ";TOOL_OFFSET J_VEC%s (", label_uf);
	}
	
	label_vector.clear();
	for(nodeElement = nodeInstructionParam->children; 
	nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name, BAD_CAST"argument")==0){
			value = xmlNodeGetContent(nodeElement);
			memset(&labelParam, 0x00, sizeof(labelParam));
			strcpy(labelParam.name, (char*)value);
			label_vector.push_back(labelParam);
		}
		else if(xmlStrcasecmp(nodeElement->name, BAD_CAST"element")==0){
			// value = xmlNodeGetContent(nodeElement);
			memset(&labelParam, 0x00, sizeof(labelParam));
			memset(label_params, 0x00, 1024);
			generateElementStr(nodeElement, objLineInfo, label_params);
			strcpy(labelParam.name, (char*)label_params);
			label_vector.push_back(labelParam);
		}
	}
	if(xmlStrcasecmp(type, BAD_CAST"with_uf")==0){
		if (label_vector.size() == 7){
			memset(label_uf, 0x00, 32);
			strcpy(label_uf, label_vector[6].name);
			label_vector.pop_back();
			memset(label_params, 0x00, sizeof(1024));
			serializeFunctionParam(label_vector, label_params);
			printBASCode(objLineInfo, "%s)", label_params);
			printBASCode(objLineInfo, " UF[%s]", label_uf);
		}
		
	}
	else {
		if (label_vector.size() == 6){
			memset(label_params, 0x00, sizeof(1024));
			serializeFunctionParam(label_vector, label_params);
			printBASCode(objLineInfo, "%s)", label_params);
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generateOffsetCondExecute
	Description:	parse <TB/TA/DB> node and output basic code.
	Input:			nodeInstructionParam  - <TB/TA/DB> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateOffsetCondExecute(xmlNodePtr nodeInstructionParam, LineInfo objLineInfo)
{
    xmlNodePtr nodeElement;
    xmlChar *name, *value, *type;
	
	name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
	printBASCode(objLineInfo, ";%s ", (char*)name);

	for(nodeElement = nodeInstructionParam->children; 
			nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name, BAD_CAST"argument")==0){
			value = xmlNodeGetContent(nodeElement);
			type  = xmlGetProp(nodeElement, BAD_CAST"type");
			
			if(xmlStrcasecmp(type, BAD_CAST"register")==0){
				printBASCode(objLineInfo, " R[%s]", (char*)value);
			}
			else if(xmlStrcasecmp(type, BAD_CAST"number_register")==0){
				printBASCode(objLineInfo, " MR[%s]", (char*)value);
			}
			else {
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
		}
		else if(xmlStrcasecmp(nodeElement->name, BAD_CAST"assignment")==0){
			printBASCode(objLineInfo, "ASSIGN ", "");
			generateAssignment(nodeElement, objLineInfo);
		}
		else if(xmlStrcasecmp(nodeElement->name, BAD_CAST"call")==0){
			generateFunctionCall(nodeElement, objLineInfo);
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generateOffsetCondition
	Description:	parse <condition> node and output basic code.
	Input:			nodeInstructionParam  - <condition> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateOffsetCondition(xmlNodePtr nodeInstructionParam, LineInfo objLineInfo)
{
    xmlNodePtr nodeSubValueElement;
    // xmlChar *name, *value;
	
	for(nodeSubValueElement = nodeInstructionParam->children; 
			nodeSubValueElement; nodeSubValueElement = nodeSubValueElement->next){
        if(xmlStrcasecmp(nodeSubValueElement->name,BAD_CAST"element")==0){ 
			generateElement(nodeSubValueElement, objLineInfo);
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generateWaitInstruction
	Description:	parse <wait> node and output basic code.
	Input:			nodeInstructionStatement  - <wait> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateWaitInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
	// char waitParam[1024];
    xmlNodePtr nodeInstructionParam;
    xmlNodePtr nodeInstructionCallParam;
    xmlChar *name, *value, *type;
	name = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");
	
	string strCommandName = (char *)name;
	transform(strCommandName.begin(), strCommandName.end(), 
		strCommandName.begin(), ::toupper); 
	exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)strCommandName.c_str());
	
	// WaitType waitType = WAIT_NONE ;
    for(nodeInstructionParam = nodeInstructionStatement->children; 
	nodeInstructionParam; nodeInstructionParam = nodeInstructionParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- (nodeInstruction) %s \n", (char*)nodeInstruction->name);
		value = xmlNodeGetContent(nodeInstructionParam);
        if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"argument")==0){
			name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
			type = xmlGetProp(nodeInstructionParam, BAD_CAST"type");
			// All of parameters should have spaces after them . 
			// Without the space, it would cause the expression analyzer work failed
			if(xmlStrcasecmp(name, BAD_CAST"condition")==0){
				printBASCode(objLineInfo, "COND ", (char*)value);
				generateOffsetCondition(nodeInstructionParam, objLineInfo);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"timeout")==0){
				if(xmlStrcasecmp(type, BAD_CAST"register")==0){
					printBASCode(objLineInfo, " R[%s]", (char*)value);
				}
				else if(xmlStrcasecmp(type, BAD_CAST"number_register")==0){
					printBASCode(objLineInfo, " MR[%s]", (char*)value);
				}
				else {
					printBASCode(objLineInfo, " %s", (char*)value);
				}
			}
			else if(xmlStrcasecmp(name, BAD_CAST"call_back")==0){
				printBASCode(objLineInfo, " %s", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"handle")==0){
				if(xmlStrcasecmp(value, BAD_CAST"SKIP")==0){
					printBASCode(objLineInfo, " %s ", (char*)value);
				}
				else if(xmlStrcasecmp(value, BAD_CAST"Warning")==0){
					printBASCode(objLineInfo, " %s ", (char*)value);
				}
				else {
					printBASCode(objLineInfo, " %s", "");
					
					for(nodeInstructionCallParam = nodeInstructionParam->children; 
						nodeInstructionCallParam; nodeInstructionCallParam = nodeInstructionCallParam->next){

						if(xmlStrcasecmp(nodeInstructionCallParam->name, BAD_CAST"call")==0){
							generateFunctionCall(nodeInstructionCallParam, objLineInfo);
						}
					}
				}
			}
		}
		else if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in Instruction. \n", (char *)nodeInstructionParam->name);
		}
    }
	printBASCode(objLineInfo, "\n", "");
	return 1 ;
}

/************************************************* 
	Function:		generatePauseInstruction
	Description:	parse <abort> node and output basic code.
	Input:			nodeInstructionStatement  - <abort> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateAbortInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
    xmlChar *name;
	name = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");
	
	string strCommandName = (char *)name;
	transform(strCommandName.begin(), strCommandName.end(), 
		strCommandName.begin(), ::toupper); 
	exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)strCommandName.c_str());

	printBASCode(objLineInfo, "\n", "");
	return 1 ;
}

/************************************************* 
	Function:		generatePauseInstruction
	Description:	parse <pause> node and output basic code.
	Input:			nodeInstructionStatement  - <pause> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generatePauseInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
    xmlChar *name;
	name = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");
	
	string strCommandName = (char *)name;
	transform(strCommandName.begin(), strCommandName.end(), 
		strCommandName.begin(), ::toupper); 
	exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)strCommandName.c_str());
	
	printBASCode(objLineInfo, "\n", "");
	return 1 ;
}

/************************************************* 
	Function:		generateUserAlarmInstruction
	Description:	parse <useralarm> node and output basic code.
	Input:			nodeInstructionStatement  - <useralarm> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateUserAlarmInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
    xmlNodePtr nodeInstructionParam;
    xmlChar *name, *value;
	name = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");
	
	string strCommandName = (char *)name;
	transform(strCommandName.begin(), strCommandName.end(), 
		strCommandName.begin(), ::toupper); 
	exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)strCommandName.c_str());
	
    for(nodeInstructionParam = nodeInstructionStatement->children; 
	nodeInstructionParam; nodeInstructionParam = nodeInstructionParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- (nodeInstruction) %s \n", (char*)nodeInstruction->name);
		value = xmlNodeGetContent(nodeInstructionParam);
        if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"argument")==0){
			name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
			// All of parameters should have spaces after them . 
			// Without the space, it would cause the expression analyzer work failed
			if(xmlStrcasecmp(name, BAD_CAST"useralarm")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
		}
		else if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in Instruction. \n", (char *)nodeInstructionParam->name);
		}
    }
	printBASCode(objLineInfo, "\n", "");
	return 1 ;
}

/************************************************* 
	Function:		generatePrintInstruction
	Description:	parse <print> node and output basic code.
	Input:			nodeInstructionStatement  - <print> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generatePrintInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
	// char timerParam[1024];
    xmlNodePtr nodeInstructionParam;
    xmlChar *type, *value;
	type = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");

	string strCommandName = (char *)type;
	transform(strCommandName.begin(), strCommandName.end(), 
		strCommandName.begin(), ::toupper); 
	exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)strCommandName.c_str());
	
    for(nodeInstructionParam = nodeInstructionStatement->children; 
	nodeInstructionParam; nodeInstructionParam = nodeInstructionParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- (nodeInstruction) %s \n", (char*)nodeInstruction->name);
		value = xmlNodeGetContent(nodeInstructionParam);
        if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"argument")==0){
			type = xmlGetProp(nodeInstructionParam, BAD_CAST"type");
			// All of parameters should have spaces after them . 
			// Without the space, it would cause the expression analyzer work failed
			if(xmlStrcasecmp(type, BAD_CAST"string")==0){
				printBASCode(objLineInfo, "\"%s\", ", (char*)value);
			}
			else if(xmlStrcasecmp(type, BAD_CAST"register")==0){
				printBASCode(objLineInfo, "%s; ", (char*)value);
			}
			else if(xmlStrcasecmp(type, BAD_CAST"variable")==0){
				printBASCode(objLineInfo, "%s; ", (char*)value);
			}
		}
		else if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in Instruction. \n", (char *)nodeInstructionParam->name);
		}
    }
	printBASCode(objLineInfo, "\"\" \n", "");
	return 1 ;
}

/************************************************* 
	Function:		generateTimerInstruction
	Description:	parse <timer> node and output basic code.
	Input:			nodeInstructionStatement  - <timer> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateTimerInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
	// char timerParam[1024];
    xmlNodePtr nodeInstructionParam;
    xmlChar *name, *value;
	name = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");
	
	string strCommandName = (char *)name;
	transform(strCommandName.begin(), strCommandName.end(), 
		strCommandName.begin(), ::toupper); 
	exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)strCommandName.c_str());
	
    for(nodeInstructionParam = nodeInstructionStatement->children; 
	nodeInstructionParam; nodeInstructionParam = nodeInstructionParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- (nodeInstruction) %s \n", (char*)nodeInstruction->name);
		value = xmlNodeGetContent(nodeInstructionParam);
        if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"argument")==0){
			name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
			// All of parameters should have spaces after them . 
			// Without the space, it would cause the expression analyzer work failed
			if(xmlStrcasecmp(name, BAD_CAST"timer")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"timer_status")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
		}
		else if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in Instruction. \n", (char *)nodeInstructionParam->name);
		}
    }
	printBASCode(objLineInfo, "\n", "");
	return 1 ;
}

/************************************************* 
	Function:		generateMoveInstruction
	Description:	parse <move_XXX> node and output basic code.
	Input:			nodeInstructionStatement  - <move_XXX> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateMoveInstruction(xmlNodePtr nodeInstructionStatement, LineInfo objLineInfo)
{
    xmlNodePtr nodeInstructionParam, nodeElement;
    xmlChar *name, *nameType, *value;
	name = xmlGetProp(nodeInstructionStatement,BAD_CAST"type");
	
	// sprintf(currentChildPath, "%s", currentXPath);
	if(xmlStrcasecmp(name,BAD_CAST"move_line")==0){
		exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)"MOVEL");
	}
	else if(xmlStrcasecmp(name,BAD_CAST"move_circle")==0){
		exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)"MOVEC");
	}
	else if(xmlStrcasecmp(name,BAD_CAST"move_joint")==0){
		exportBASCode(objLineInfo, (char *)"EXPORT: ", (char *)"%s ", (char *)"MOVEJ");
	}

    for(nodeInstructionParam = nodeInstructionStatement->children; 
		nodeInstructionParam; nodeInstructionParam = nodeInstructionParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- (nodeInstruction) %s \n", (char*)nodeInstruction->name);
		value = xmlNodeGetContent(nodeInstructionParam);
        if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"argument")==0){
			name = xmlGetProp(nodeInstructionParam, BAD_CAST"name");
			// All of parameters should have spaces after them . 
			// Without the space, it would cause the expression analyzer work failed
			if((xmlStrcasecmp(name, BAD_CAST"pose")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"cirpose")==0) 
				||(xmlStrcasecmp(name, BAD_CAST"topose")==0)) {
				nameType = xmlGetProp(nodeInstructionParam, BAD_CAST"type");
				if(xmlStrcasecmp(nameType, BAD_CAST"num")==0){
					for(nodeElement = nodeInstructionParam->children; 
							nodeElement; nodeElement = nodeElement->next){
						if(xmlStrcasecmp(nodeElement->name, BAD_CAST"element")==0){
						//	value = xmlNodeGetContent(nodeElement);
						//	printBASCode(objLineInfo, "%s ", (char*)value);
							generateElement(nodeElement, objLineInfo);
							printBASCode(objLineInfo, ", ", "");
						}
					}
				}
				else if(xmlStrcasecmp(nameType, BAD_CAST"pose_register")==0){
					generatePoseRegElement(nodeInstructionParam, objLineInfo);
					printBASCode(objLineInfo, ", ", "");
				}
				else if(xmlStrcasecmp(nameType, BAD_CAST"pose")==0){
					generatePoseElement(nodeInstructionParam, objLineInfo);
					printBASCode(objLineInfo, ", ", "");
				}
			}
			else if(xmlStrcasecmp(name, BAD_CAST"velocity")==0){
				nameType = xmlGetProp(nodeInstructionParam, BAD_CAST"type");
				if(xmlStrcasecmp(nameType, BAD_CAST"num")==0){
					printBASCode(objLineInfo, "%s ", (char*)value);
				}
				else if(xmlStrcasecmp(nameType, BAD_CAST"motion_register")==0){
					char label_params[1024];
					char label_param[32];
					memset(label_params, 0x00, 1024);
					
					for(nodeElement = nodeInstructionParam->children; 
								nodeElement; nodeElement = nodeElement->next){
						if(xmlStrcasecmp(nodeElement->name, BAD_CAST"element")==0){
							//	value = xmlNodeGetContent(nodeElement);
							//	printBASCode(objLineInfo, "%s ", (char*)value);
							memset(label_param, 0x00, 32);
							generateElementStr(nodeElement, objLineInfo, label_params);
							sprintf(label_params, "%s%s ", label_params, label_param);
						}
					}
					trim(label_params);
					printBASCode(objLineInfo, "MR[%s] ", (char*)label_params);
				}
			}
			else if(xmlStrcasecmp(name, BAD_CAST"fine")==0){
				if(strcmp((char*)value, "") != 0)
					printBASCode(objLineInfo, "CNT %s ", (char*)value);
				else
				{
					// default is -1 in the FINE for safety.
					printBASCode(objLineInfo, "CNT %s ", (char*)"-1");
				}
			}
			else if(xmlStrcasecmp(name, BAD_CAST"smooth_velocity")==0){
				printBASCode(objLineInfo, "SV %s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"smooth_distance")==0){
				printBASCode(objLineInfo, "SD %s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"acceleration")==0){
				printBASCode(objLineInfo, ";ACC %s ", (char*)value);
			}
			else if((xmlStrcasecmp(name, BAD_CAST"offset_cvec")==0)
			      ||(xmlStrcasecmp(name, BAD_CAST"offset_jvec")==0)
			      ||(xmlStrcasecmp(name, BAD_CAST"tool_offset_jvec")==0)
				  ||(xmlStrcasecmp(name, BAD_CAST"tool_offset_cvec")==0)){
				generateOffsetVEC(nodeInstructionParam, objLineInfo);
				
			}
			else if((xmlStrcasecmp(name, BAD_CAST"offset_pr")==0)
			      ||(xmlStrcasecmp(name, BAD_CAST"tool_offset_pr")==0)){
				generateOffsetPR(nodeInstructionParam, objLineInfo);
			}
			else if((xmlStrcasecmp(name, BAD_CAST"ev")==0)
					||(xmlStrcasecmp(name, BAD_CAST"ind_ev")==0)){
				if(xmlStrcasecmp(name, BAD_CAST"ev")==0){
					printBASCode(objLineInfo, ";EV %s ", (char*)value);
				}
				else if(xmlStrcasecmp(name, BAD_CAST"ind_ev")==0){
					printBASCode(objLineInfo, ";Ind_EV %s ", (char*)value);
				}
			}
			else if((xmlStrcasecmp(name, BAD_CAST"TB")==0)
		          ||(xmlStrcasecmp(name, BAD_CAST"TA")==0)
	              ||(xmlStrcasecmp(name, BAD_CAST"DB")==0)){
				generateOffsetCondExecute(nodeInstructionParam, objLineInfo);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"timer")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"timer_status")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"useralarm")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"condition")==0){
				printBASCode(objLineInfo, "COND ", (char*)value);
				generateOffsetCondition(nodeInstructionParam, objLineInfo);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"timeout")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"call_back")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
			else if(xmlStrcasecmp(name, BAD_CAST"handle")==0){
				printBASCode(objLineInfo, "%s ", (char*)value);
			}
		}
		else if(xmlStrcasecmp(nodeInstructionParam->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in Instruction. \n", (char *)nodeInstructionParam->name);
		}
    }
	printBASCode(objLineInfo, "\n", "");
	return 1 ;
}

// Same with IF without endif
/************************************************* 
	Function:		generateLogicalElseIF
	Description:	parse <vice_logical> node of <if> node and output basic code.
	Input:			nodeLogicalStatement  - <vice_logical> node
	Input:			objLineInfo           - Line Info
	Return: 		1 - success
*************************************************/ 
int generateLogicalElseIF(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *value, *else_type;
	
	else_type = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(else_type,BAD_CAST"elif")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
		           (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, (char *)"EXPORT: ", (char *)"%s ", (char *)"ELSEIF");
	}
	else if(xmlStrcasecmp(else_type,BAD_CAST"else")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
			(char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, (char *)"EXPORT: ", (char *)"%s \n", (char *)"ELSE");
	}
	
    for(nodeStatement = nodeLogicalStatement->children; 
	nodeStatement; nodeStatement = nodeStatement->next){
		// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
			nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//		FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfo);
				}
			}
			if(xmlStrcasecmp(else_type,BAD_CAST"elif")==0){
				printBASCode(objLineInfo, (char *)" THEN \n", (char *)"");
			}
        }
		else if((xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0)
			||(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0)) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			generateFunctionBody(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			generateLogicalElseIF(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in ElseIF. \n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
	// exportBASCode(objLineInfoTemp, (char *)"EXPORT: ", (char *)" ", (char *)"%s \n", (char *)"endif");
	return 1 ;
}

/************************************************* 
	Function:		generateLogicalIF
	Description:	parse <if> node and output basic code.
	Input:			nodeLogicalStatement  - <if> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateLogicalIF(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *name, *value;
	
	name = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(name,BAD_CAST"if")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
						(char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, (char *)"EXPORT: ", (char *)"%s ", (char *)"IF");
	}

    for(nodeStatement = nodeLogicalStatement->children; 
		nodeStatement; nodeStatement = nodeStatement->next){
			// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
				nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//		FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfo);
				}
			}
			printBASCode(objLineInfo, " THEN \n", "");
        }
		else if((xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0)
		      ||(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0)) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			generateFunctionBody(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue ;
			generateLogicalElseIF(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in IF. \n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
    objLineInfoTemp.indentValue = objLineInfo.indentValue ;
    sprintf(objLineInfoTemp.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
	exportBASCode(objLineInfoTemp, "EXPORT: ", "%s \n", "ENDIF");
	return 1 ;
}

/************************************************* 
	Function:		generateLogicalWHILE
	Description:	parse <while> node and output basic code.
	Input:			nodeLogicalStatement  - <while> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateLogicalWHILE(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *name, *value;
	
	name = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(name,BAD_CAST"while")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, "EXPORT: ", "%s ", "WHILE");
	}
	
    for(nodeStatement = nodeLogicalStatement->children; 
	nodeStatement; nodeStatement = nodeStatement->next){
		// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
			nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//		FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfoTemp);
				}
			}
			printBASCode(objLineInfoTemp, " \n", "");
        }
		else if((xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0)
			||(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0)) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			// sprintf(currentChildPath, "%s/%s", currentXPath, (char *)nodeStatement->name);
			generateFunctionBody(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in WHILE. \n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
    objLineInfoTemp.indentValue = objLineInfo.indentValue ;
    sprintf(objLineInfoTemp.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
	exportBASCode(objLineInfoTemp, "EXPORT: ", "%s \n", "WEND");
	return 1 ;
}

/************************************************* 
	Function:		generateCASEInSWITCH
	Description:	parse <vice_logical> node of <switch> node and output basic code.
	Input:			nodeLogicalStatement  - <vice_logical> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateCASEInSWITCH(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *name, *value;
	
	name = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(name,BAD_CAST"case")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, "EXPORT: ", "%s ", "CASE ");
	}
	else if(xmlStrcasecmp(name,BAD_CAST"default")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, "EXPORT: ", "%s ", "DEFAULT ");
	}
	else { 
		FST_ERROR("Wrong type (%s) in CASE. \n", (char *)name);
	}
	
    for(nodeStatement = nodeLogicalStatement->children; 
	nodeStatement; nodeStatement = nodeStatement->next){
		// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
			nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//		FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfoTemp);
				}
			}
			printBASCode(objLineInfoTemp, " \n", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			// generateSwitchBody(nodeStatement);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0) { 
			// printf("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			generateFunctionBody(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in CASE. \n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
	exportBASCode(objLineInfo, "EXPORT: ", "%s \n", "BREAK");
	return 1 ;
}

/************************************************* 
	Function:		generateLogicalSWITCH
	Description:	parse <switch> node and output basic code.
	Input:			nodeLogicalStatement  - <switch> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateLogicalSWITCH(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	int iCaseIdx = 0 ;
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *name, *value;
	
	name = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(name,BAD_CAST"switch")==0){
		// Use the determine node
		sprintf(objLineInfo.xPath, "%s/determine", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfo, "EXPORT: ", "%s ", "SELECT CASE ");
	}
	
    for(nodeStatement = nodeLogicalStatement->children; 
	nodeStatement; nodeStatement = nodeStatement->next){
		// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
			nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//	FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", 
					//    (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfo);
				}
			}
			printBASCode(objLineInfo, " \n", "");
        }
		else if((xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0)
			||(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0)) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			// generateSwitchBody(nodeStatement);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical")==0) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			// objLineInfoTemp.xPathIdx = iCaseIdx++;
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			generateCASEInSWITCH(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in SWITCH\n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
    objLineInfoTemp.indentValue = objLineInfo.indentValue ;
	sprintf(objLineInfo.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
	exportBASCode(objLineInfoTemp, "EXPORT: ", "%s \n", "END SELECT");
	return 1 ;
}

/************************************************* 
	Function:		generateLogicalLOOP
	Description:	parse <call> node and output basic code.
	Input:			nodeFunctionCall  - <call> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateFunctionCall(xmlNodePtr nodeFunctionCall, LineInfo objLineInfo)
{
    char label_instr[32];
    char label_params[1024];
    Label labelParam ;
	vector<Label> label_vector;

    xmlNodePtr nodeFunctionCallParam ;
    xmlChar *nameProp, *fileProp, *typeProp, *value;
	fileProp = xmlGetProp(nodeFunctionCall, BAD_CAST"file");
	nameProp = xmlGetProp(nodeFunctionCall, BAD_CAST"name");
	typeProp = xmlGetProp(nodeFunctionCall, BAD_CAST"type");
	sprintf(objLineInfo.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeFunctionCall));
	
	memset(label_instr, 0x00, 32);
	if(xmlStrcasecmp(typeProp,BAD_CAST"macro")==0){
        sprintf(label_instr, "CALLMACRO");
	}
	else {
        sprintf(label_instr, "CALL");
	}
	if(xmlStrlen(fileProp)==0){        // Inside function
		memset(label_params, 0x00, 1024);
        sprintf(label_params, "%s %s::%s", label_instr, objLineInfo.fileName, (char*)nameProp);
		printBASCode(objLineInfo, "%s (", label_params);
	}
	else if(xmlStrlen(nameProp)==0){   // Outside main
		memset(label_params, 0x00, 1024);
        sprintf(label_params, "%s %s::main", label_instr, (char*)fileProp);
		printBASCode(objLineInfo, "%s (", label_params);
	}
	else {                            // Outside function
		memset(label_params, 0x00, 1024);
        sprintf(label_params, "%s %s::%s", label_instr, (char*)fileProp, (char*)nameProp);
		printBASCode(objLineInfo, "%s (", label_params);
	}
    for(nodeFunctionCallParam = nodeFunctionCall->children; 
		nodeFunctionCallParam; nodeFunctionCallParam = nodeFunctionCallParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- (nodeFunctionCall) %s \n", (char*)nodeInstruction->name);
		value = xmlNodeGetContent(nodeFunctionCallParam);
        if(xmlStrcasecmp(nodeFunctionCallParam->name,BAD_CAST"parameter")==0){ 
            memset(&labelParam, 0x00, sizeof(labelParam));
            strcpy(labelParam.name, (char*)value);
            label_vector.push_back(labelParam);
		}
		else if(xmlStrcasecmp(nodeFunctionCallParam->name,BAD_CAST"element")==0){ 
			memset(&labelParam, 0x00, sizeof(labelParam));
			// strcpy(labelParam.name, (char*)value);
			memset(label_params, 0x00, 1024);
			generateElementStr(nodeFunctionCallParam, objLineInfo, label_params);
			strcpy(labelParam.name, (char*)label_params);
			label_vector.push_back(labelParam);
		}
    }
    memset(label_params, 0x00, 1024);
    serializeFunctionParam(label_vector, label_params);
    printBASCode(objLineInfo, "%s)", label_params);
	return 1 ;
}

/************************************************* 
	Function:		generateLogicalLOOP
	Description:	parse <loop> node and output basic code.
	Input:			nodeLogicalStatement  - <loop> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateLogicalLOOP(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *name, *value;
	
	name = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(name,BAD_CAST"LOOP")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, (char *)"EXPORT: ", (char *)"%s ", (char *)"LOOP");
	}
	
    for(nodeStatement = nodeLogicalStatement->children; 
	nodeStatement; nodeStatement = nodeStatement->next){
		// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
			nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//		FST_ERROR("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfoTemp);
				}
			}
			printBASCode(objLineInfoTemp, " \n", "");
        }
		else if((xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0)
			||(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0)) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			// sprintf(currentChildPath, "%s/%s", currentXPath, (char *)nodeStatement->name);
			generateFunctionBody(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in LOOP. \n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
    objLineInfoTemp.indentValue = objLineInfo.indentValue ;
    sprintf(objLineInfoTemp.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
	exportBASCode(objLineInfoTemp, "EXPORT: ", "%s \n", "ENDLOOP");
	return 1 ;
}

/************************************************* 
	Function:		generateLogicalFOR
	Description:	parse <for> node and output basic code.
	Input:			nodeLogicalStatement  - <for> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateLogicalFOR(xmlNodePtr nodeLogicalStatement, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlNodePtr nodeStatement ;
    xmlNodePtr nodeDetermine ;
    xmlChar *name, *value;
	
	name = xmlGetProp(nodeLogicalStatement,BAD_CAST"type");
	if(xmlStrcasecmp(name,BAD_CAST"for")==0){
		// Use the determine node
		sprintf(objLineInfoTemp.xPath, "%s/determine", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
		exportBASCode(objLineInfoTemp, "EXPORT: ", "%s ", "FOR");
	}
	
    for(nodeStatement = nodeLogicalStatement->children; 
	nodeStatement; nodeStatement = nodeStatement->next){
		// FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
		if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"determine")==0){ 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			for(nodeDetermine = nodeStatement->children; 
			nodeDetermine; nodeDetermine = nodeDetermine->next){
				if(xmlStrcasecmp(nodeDetermine->name, BAD_CAST"element")==0){ 
					value = xmlNodeGetContent(nodeDetermine);
					//		FST_INFO("\t\t\t\t  --debug-- (nodeRightValue) %s = %s\n", (char*)nodeRightValue->name, (char *)value);
					generateElement(nodeDetermine, objLineInfoTemp);
				}
			}
			printBASCode(objLineInfoTemp, " \n", "");
        }
		else if((xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical_body")==0)
			||(xmlStrcasecmp(nodeStatement->name,BAD_CAST"vice_logical_body")==0)) { 
			// FST_INFO("%s, ", (char*)nodeStatement->name);
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
			// sprintf(currentChildPath, "%s/%s", currentXPath, (char *)nodeStatement->name);
			generateFunctionBody(nodeStatement, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Name (%s) in WHILE. \n", (char *)nodeStatement->name);
		}
        // FST_INFO("%s \n", (char*)nodeStatement->name);
    }
    objLineInfoTemp.indentValue = objLineInfo.indentValue ;
    sprintf(objLineInfoTemp.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeLogicalStatement));
	exportBASCode(objLineInfoTemp, "EXPORT: ", "%s \n", "NEXT");
	return 1 ;
}

/************************************************* 
	Function:		generateFunctionBody
	Description:	parse <body> node of function node and output basic code.
	Input:			nodeFunctionBody  - <body> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateFunctionBody(xmlNodePtr nodeFunctionBody, LineInfo objLineInfo)
{
    int iAssignmentIdx = 0 , iInstructionIdx = 0 , iLogicalIdx = 0  , 
		iCommentIdx = 0    , iNopIdx = 0         , iCallIdx = 0     , 
		iReturnIdx = 0     , iBreakIdx = 0       , iContinueIdx = 0 ;
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
    xmlChar *name, *type, *value;
    xmlNodePtr nodeStatement ;

	// EXPORT Subroutine Statement
    for(nodeStatement = nodeFunctionBody->children; 
       nodeStatement; nodeStatement = nodeStatement->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeStatement->name);
        if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"assignment")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iAssignmentIdx++;
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			exportBASCode(objLineInfoTemp, "EXPORT: ", "", "");
			generateAssignment(nodeStatement, objLineInfoTemp);
			printBASCode(objLineInfo, " \n", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"instruction")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iInstructionIdx++;
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));

			type = xmlGetProp(nodeStatement,BAD_CAST"type");
			if((xmlStrcasecmp(type,BAD_CAST"move_line")==0)
					||(xmlStrcasecmp(type,BAD_CAST"move_circle")==0)
					||(xmlStrcasecmp(type,BAD_CAST"move_joint")==0)){
				generateMoveInstruction(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(type,BAD_CAST"timer")==0){
				generateTimerInstruction(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(type,BAD_CAST"useralarm")==0){
				generateUserAlarmInstruction(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(type,BAD_CAST"wait")==0){
				generateWaitInstruction(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(type,BAD_CAST"pause")==0){
				generatePauseInstruction(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(type,BAD_CAST"abort")==0){
				generateAbortInstruction(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(type,BAD_CAST"print")==0){
				generatePrintInstruction(nodeStatement, objLineInfoTemp);
			}
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"logical")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iLogicalIdx++;
			name = xmlGetProp(nodeStatement, BAD_CAST"type");
			if(xmlStrcasecmp(name,BAD_CAST"if")==0){ 
				sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
				generateLogicalIF(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(name,BAD_CAST"while")==0){ 
				sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
				generateLogicalWHILE(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(name,BAD_CAST"switch")==0){ 
				sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
				generateLogicalSWITCH(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(name,BAD_CAST"for")==0){ 
				sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
				generateLogicalFOR(nodeStatement, objLineInfoTemp);
			}
			else if(xmlStrcasecmp(name,BAD_CAST"loop")==0){ 
				sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
				generateLogicalLOOP(nodeStatement, objLineInfoTemp);
			}
			else { 
				FST_ERROR("Wrong Command (%s) in logical Command. \n", (char *)name);
			}
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"comment")==0){ 
        //    // FST_INFO("%s, ", (char*)nodeStatement->name);
        //    // objLineInfoTemp.xPathIdx = iCommentIdx++;
			value = xmlNodeGetContent(nodeStatement);
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			FST_INFO("Omit comment - %s ", (char *)value);
		//	exportBASCode(objLineInfoTemp, "EXPORT: ", "# %s \n", (char*)value);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"nop")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // // objLineInfoTemp.xPathIdx = iNopIdx++;
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			// exportBASCode(objLineInfoTemp, "EXPORT: ", "NOP\n", "");
			// exportBASCode(objLineInfoTemp, "EXPORT: ", "", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"call")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iCallIdx++;
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			exportBASCode(objLineInfoTemp, "EXPORT: ", "", "");
			generateFunctionCall(nodeStatement, objLineInfoTemp);
			printBASCode(objLineInfo, " \n", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"return")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iReturnIdx++;
			value = xmlNodeGetContent(nodeStatement);
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			exportBASCode(objLineInfoTemp, "EXPORT: ", "RETURN %s \n", (char*)value);
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"break")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iBreakIdx++;
			value = xmlNodeGetContent(nodeStatement);
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			exportBASCode(objLineInfoTemp, "EXPORT: ", "BREAK \n", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"continue")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iContinueIdx++;
			value = xmlNodeGetContent(nodeStatement);
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			exportBASCode(objLineInfoTemp, "EXPORT: ", "CONTINUE \n", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"end")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
            // objLineInfoTemp.xPathIdx = iContinueIdx++;
			value = xmlNodeGetContent(nodeStatement);
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeStatement));
			exportBASCode(objLineInfoTemp, "EXPORT: ", "END \n", "");
        }
		else if(xmlStrcasecmp(nodeStatement->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong Command (%s) in FunctionBody. \n", (char *)nodeStatement->name);
		}
	}
	return 1 ;
}

/************************************************* 
	Function:		generateFunction
	Description:	parse <function> node and output basic code.
	Input:			nodeFunction  - <function> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateFunction(xmlNodePtr nodeFunction, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
	int        isNodeExist = 0 ;
    char label_params[1024];
    Label labelParam ;
	vector<Label> label_vector;
    xmlNodePtr nodeFunctionParam ;
    xmlChar *name, *value;
    
    xmlNodePtr nodeFunctionBody ;
	// EXPORT Subroutine name
	name = xmlGetProp(nodeFunction,BAD_CAST"name");
    sprintf(objLineInfo.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeFunction));
    exportBASCode(objLineInfo, "EXPORT: ", "SUB %s (", (char*)name);

    for(nodeFunctionParam = nodeFunction->children; 
		nodeFunctionParam; nodeFunctionParam = nodeFunctionParam->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeFunctionParam->name);
        if(xmlStrcasecmp(nodeFunctionParam->name,BAD_CAST"parameter")==0){ 
            // FST_INFO("%s, ", (char*)nodeFunctionParam->name);
            memset(&labelParam, 0x00, sizeof(labelParam));
			value = xmlNodeGetContent(nodeFunctionParam);
            strcpy(labelParam.name, (char*)value);
            label_vector.push_back(labelParam);
        }
		else if(xmlStrcasecmp(nodeFunctionParam->name,BAD_CAST"element")==0){ 
			memset(&labelParam, 0x00, sizeof(labelParam));
			// strcpy(labelParam.name, (char*)value);
			memset(label_params, 0x00, 1024);
			generateElementStr(nodeFunctionParam, objLineInfo, label_params);
			strcpy(labelParam.name, (char*)label_params);
			label_vector.push_back(labelParam);
		}
    }
    memset(label_params, 0x00, 1024);
    serializeFunctionParam(label_vector, label_params);
    printBASCode(objLineInfo, "%s)\n", label_params);

	// EXPORT Subroutine Body
    for(nodeFunctionBody = nodeFunction->children; 
	nodeFunctionBody; nodeFunctionBody = nodeFunctionBody->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeFunctionBody->name);
        if(xmlStrcasecmp(nodeFunctionBody->name,BAD_CAST"body")==0){ 
			isNodeExist = 1;
			objLineInfoTemp.indentValue = objLineInfo.indentValue + 1;
            generateFunctionBody(nodeFunctionBody, objLineInfoTemp);
        }
    }
    sprintf(objLineInfo.xPath, "%s", 
			 (char *)xmlGetNodePath(nodeFunction));
    exportBASCode(objLineInfo, "EXPORT: ", "END SUB \n", "");
	if(isNodeExist == 0)   {
        FST_ERROR("\t\t\t\t ERROR: no function body\n");
    }
	return 1 ;
}

/************************************************* 
	Function:		generateProgBody
	Description:	parse <prog_body> node and output basic code.
	Input:			nodeProgBody  - <prog_body> node
	Input:			objLineInfo   - Line Info
	Return: 		1 - success
*************************************************/ 
int generateProgBody(xmlNodePtr nodeProgBody, LineInfo objLineInfo)
{
	LineInfo objLineInfoTemp = objLineInfo;
	// char currentChildPath[LAB_LEN];
	int        isNodeExist = 0 ;
    xmlNodePtr nodeFunction ;
    xmlChar *name; // , * xCurrentPath ;
    for(nodeFunction = nodeProgBody->children; 
		nodeFunction; nodeFunction = nodeFunction->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeFunction->name);
        if(xmlStrcasecmp(nodeFunction->name,BAD_CAST"function")==0){ 
			name = xmlGetProp(nodeFunction,BAD_CAST"name");
			if(!strcmp((char *)name, "main"))
			{
				isNodeExist = 1;
			}
			// xCurrentPath = xmlGetNodePath(nodeFunction);
            generateFunction(nodeFunction, objLineInfoTemp);
        }
		else if(xmlStrcasecmp(nodeFunction->name,BAD_CAST"nop")==0){ 
            // FST_INFO("%s, ", (char*)nodeStatement->name);
			sprintf(objLineInfoTemp.xPath, "%s", 
					 (char *)xmlGetNodePath(nodeFunction));
		//	exportBASCode(objLineInfoTemp, "EXPORT: ", "NOP\n", "");
		//	exportBASCode(objLineInfoTemp, "EXPORT: ", "", "");
        }
		else if(xmlStrcasecmp(nodeFunction->name,BAD_CAST"text")==0){
			;
		}
		else { 
			FST_ERROR("Wrong name (%s) in ProgBody. \n", (char *)nodeFunction->name);
		}
    }
	if(isNodeExist == 0)   {
        FST_ERROR("\t\t\t\t ERROR: no function = main\n");
    }
	return 1 ;
}

/************************************************* 
	Function:		parse_xml_file
	Description:	parse xml file.
	Input:			file_name  - xml file name
	Return: 		0 - Success , -1 -  Failed
*************************************************/ 
int parse_xml_file(char * file_name){
	LineInfo objLineInfo ;
	// char currentChildPath[LAB_LEN];
    xmlDocPtr doc;
    xmlNodePtr rootProg ;
    xmlNodePtr nodeHead, nodeProgBody ;
	int        isNodeExist = 0 ;
    // doc=xmlParseMemory(buf,len);    //parse xml in memory
    doc = xmlReadFile(file_name,"UTF-8",XML_PARSE_RECOVER);
    if(doc == NULL){
        FST_ERROR("\t\t\t\t ERROR: doc == null\n");
        return -1;
    }
    rootProg = xmlDocGetRootElement(doc);

	memset(&objLineInfo, 0x00, sizeof(LineInfo));
	sprintf(objLineInfo.xPath,    "/%s", rootProg->name) ;
	
	memset(objLineInfo.fileName, 0x00, FILE_PATH_LEN);
#ifdef WIN32
	char * fileNameStartPtr = strrchr(file_name, '\\');
	if (fileNameStartPtr == NULL)
		fileNameStartPtr = file_name ;
	else 
		fileNameStartPtr++ ;
#else
	char * fileNameStartPtr = strrchr(file_name, '/');
	if (fileNameStartPtr == NULL)
		fileNameStartPtr = file_name ;
	else 
		fileNameStartPtr++ ;
#endif
	char * fileNameEndPtr = strrchr(file_name, '.');
	memcpy(objLineInfo.fileName, fileNameStartPtr, 
		fileNameEndPtr - fileNameStartPtr);

    g_lineNum = 1 ;
    // generate Include File
	isNodeExist = 0 ;
    for(nodeHead = rootProg->children; 
            nodeHead; nodeHead = nodeHead->next){
        if(xmlStrcasecmp(nodeHead->name,BAD_CAST"head")==0)
        {
			isNodeExist = 1 ;
			sprintf(objLineInfo.xPath, "%s", 
			 		(char *)xmlGetNodePath(nodeHead));
            generateIncludeFile(nodeHead, objLineInfo);
            break;
        }
    }
    // if(nodeHead == NULL){
	if(isNodeExist == 0)   {
        FST_INFO("\t\t\t\t ERROR: no node = head\n");
    }

	// generate prog_body 
	isNodeExist = 0 ;
    for(nodeProgBody = rootProg->children; 
		nodeProgBody; nodeProgBody = nodeProgBody->next){
		//				FST_INFO("\t\t\t\t  --debug-- %s \n", (char*)nodeProgBody->name);
        if(xmlStrcasecmp(nodeProgBody->name,BAD_CAST"prog_body")==0)
        {
			isNodeExist = 1 ;
			sprintf(objLineInfo.xPath, "%s", 
			 		(char *)xmlGetNodePath(nodeProgBody));
            generateProgBody(nodeProgBody, objLineInfo);
            break;
        }
    }
    // if(nodeProgBody == NULL){
	if(isNodeExist == 0)   {
        FST_INFO("\t\t\t\t ERROR: no node = ProgBody\n");
        return -1;
    }
    xmlFreeDoc(doc);
//	xmlCleanupParser();
    return 0;
}

/* Return 1 if c is space or tab. */
static int iswhite(char c)
{
	if(c==' ' || c=='\t') return 1;
	else return 0;
}

// 	void addLineNumber(char * file_name)
// 	{
// 		int iLineNum = 0 ;
// 		char contentLine[LAB_LEN];
// 		char * contentPtr, * contentLinePtr; 
// 		char * contentSpacePtr, * contentSlashPtr, * contentAnglePtr;
// 		char contentLineAddNum[LAB_LEN];
// 
// 		FILE *file_xml, *file_xml_addline ;
// 
// 		file_xml_addline = fopen("data2.xml","w");  
// 		if(file_xml_addline == NULL)  
// 		{  
// 			perror("open file data2.xml");  
// 			exit(0);  
// 		}
// 		
// 		if((file_xml = fopen(file_name, "r"))==NULL){
// 			perror("openf file error");
// 		}
// 
// 		iLineNum = 1 ;
// 		while(fgets(contentLine,sizeof(contentLine),file_xml)!=NULL)  
// 		{  
// 			memset(contentLineAddNum, 0x00, LAB_LEN);
// 			contentPtr = contentLine ;
// 			contentLinePtr = contentLine ;
// 			while(iswhite(*(contentLinePtr)))
//   			  ++contentLinePtr; 
// 			if(contentLinePtr[0] == '<')
// 			{
// 				if((contentLinePtr[1] != '/')
// 					&& (contentLinePtr[1] != '?'))
// 				{
// 					contentSpacePtr = strchr(contentLinePtr, ' ');
// 					contentSlashPtr = strchr(contentLinePtr, '/');
// 					contentAnglePtr = strchr(contentLinePtr, '>');
// 					if(contentSpacePtr)
// 					{
// 						if(contentSlashPtr)
// 						{
// 							if(contentSlashPtr < contentSpacePtr)
// 								contentSpacePtr = contentSlashPtr ;
// 						}
// 						if(contentAnglePtr)
// 						{
// 							if(contentAnglePtr < contentSpacePtr)
// 								contentSpacePtr = contentAnglePtr ;
// 						}
// 					}
// 					else 
// 					{
// 						if(contentSlashPtr)
// 						{
// 							contentSpacePtr = contentSlashPtr ;
// 						}
// 						if(contentAnglePtr)
// 						{
// 							if(contentSpacePtr)
// 							{
// 								if(contentAnglePtr < contentSpacePtr)
// 									contentSpacePtr = contentAnglePtr ;
// 							}
// 							else
// 							{
// 								contentSpacePtr = contentAnglePtr ;
// 							}
// 						}
// 					}
// 
// 					if(contentSpacePtr)
// 					{
// 						memcpy(contentLineAddNum, 
// 							contentPtr, 
// 							contentSpacePtr - contentPtr);
// 						sprintf(contentLineAddNum, "%s line=\"%d\" %s", 
// 							contentLineAddNum, iLineNum, contentSpacePtr);
// 						fputs(contentLineAddNum, file_xml_addline);
// 					}
// 				}
// 				else {
// 					fputs(contentLine, file_xml_addline);
// 				}
// 			}
// 			else {
// 				fputs(contentLine, file_xml_addline);
// 			}
// 			// FST_INFO("%s", contentLine);  
// 			iLineNum++ ;
// 		}
// 		fclose(file_xml_addline);
// 		fclose(file_xml);
// 		// FST_INFO("content:\n%s\n",content);	
// 	}

/************************************************* 
	Function:		outputXPathVector
	Description:	Print xpath file content.
	Input:			xpath_file_name  - xml file name
	Output: 		NULL
	Return: 		NULL
*************************************************/ 
void outputXPathVector(char * xpath_file_name)
{
	vector<string> vecXPath ;
	int iLineNum = 0 ;
	char contentLine[FILE_PATH_LEN];
	char * contentSepPtr; 
	char contentLineNum[LINE_LAB_LEN];
	char contentXPath[FILE_PATH_LEN];

	FILE *xpath_file ;

	if((xpath_file = fopen(xpath_file_name, "r"))==NULL){
		perror("open xpath_file_name file failed\n");  
		return ;
	}
	
	// Pre-arrange
	// for(int i =0 ; i < g_lineNum + 1 ; i++)
	//  	vecXPath.push_back("");
	vecXPath.resize(g_lineNum + 1);

	memset(contentLine,    0x00, FILE_PATH_LEN);
	while(fgets(contentLine,sizeof(contentLine),xpath_file)!=NULL)  
	{  
		memset(contentLineNum, 0x00, LINE_LAB_LEN);
		memset(contentXPath,   0x00, FILE_PATH_LEN);
		contentSepPtr = strchr(contentLine, ':');
		if(contentSepPtr)
		{
			memcpy(contentLineNum, contentLine, 
				contentSepPtr - contentLine);
			strcpy(contentXPath,   contentSepPtr + 1);
			iLineNum = atoi(contentLineNum);
			vecXPath[iLineNum] = string(contentXPath) ;
		}
	}
	
	for(int i =1 ; i < vecXPath.size() ; i++)
	{
		FST_INFO("%d - %s" , i, vecXPath[i].c_str());
	}
}

/************************************************* 
	Function:		parse_xml_file_wrapper
	Description:	Wrapper function: Open file and call parse_xml_file.
	Input:			xml_file_name  - xml file name
	Output: 		NULL
	Return: 		0 - Success , -1 -  Failed
*************************************************/ 
int parse_xml_file_wrapper(char * xml_file_name){
    xmlDocPtr doc;
	char   xmlFileName[FILE_PATH_LEN];
	if(xml_file_name == 0) {
		FST_ERROR("usage: run <filename>\n");
		return 0 ;
	}
	// Not translate unexist xml.
	doc = xmlReadFile(xml_file_name,"UTF-8",XML_PARSE_RECOVER);
    if(doc == NULL){
        FST_ERROR("\t\t\t\t ERROR: doc == null\n");
        return -1;
    }
	xmlFreeDoc(doc);
//	xmlCleanupParser();
	
	memset(g_xml_file_name, 0x00, FILE_PATH_LEN);
	strcpy(g_xml_file_name, xml_file_name);
	
	memset(xmlFileName, 0x00, FILE_PATH_LEN);
	char * fileNamePtr = strrchr(g_xml_file_name, '.');
	memcpy(xmlFileName, g_xml_file_name, 
		fileNamePtr - g_xml_file_name);

	memset(g_xpath_file_name, 0x00, FILE_PATH_LEN);
	sprintf(g_xpath_file_name, "%s_xpath.txt", xmlFileName);
	// sprintf(g_xpath_file_name, "%s_xpath.txt", project_name);
	
	memset(g_basic_file_name, 0x00, FILE_PATH_LEN);
	sprintf(g_basic_file_name, "%s.bas", xmlFileName);

	memset(g_mix_file_name, 0x00, FILE_PATH_LEN);
	sprintf(g_mix_file_name, "%s_mix.txt", xmlFileName);
    
	remove(g_xpath_file_name);
	remove(g_basic_file_name);
	remove(g_mix_file_name);
    if(parse_xml_file(g_xml_file_name)<0){
            perror("parse xml failed");
    }

	// outputXPathVector(g_xpath_file_name);
    return 0;
}

