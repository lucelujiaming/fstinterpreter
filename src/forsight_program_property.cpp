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

#include "forsight_program_property.h"
#include "forsight_cJSON.h"
#include "forsight_basint.h"
#include "forsight_inter_control.h"

#define POSE_NONE      0
#define POSE_CART      1
#define POSE_JOINT     2

// -------------------------------- libxml --------------------------------
int parseHomePoseElement(xmlNodePtr nodeHead, char * home_pose_exp)
{
    xmlNodePtr nodeElement;
    xmlChar *type, *value;
	
    for(nodeElement = nodeHead->children; 
			nodeElement; nodeElement = nodeElement->next){
		if(xmlStrcasecmp(nodeElement->name,BAD_CAST"element")==0){ 
			type = xmlGetProp(nodeElement, BAD_CAST"type");
			if(xmlStrcasecmp(type, BAD_CAST"num")==0){
				value = xmlNodeGetContent(nodeElement);
			//	sprintf(home_pose_exp, "%s HP[%s]", 
				sprintf(home_pose_exp, "%s %s", 
							home_pose_exp, (char *)value);
			}
        }
    }
	return 1 ;
}

int parseHomePose(xmlNodePtr nodeRoot, char * home_pose_exp)
{
    xmlNodePtr nodeHead ;
    xmlChar *type, *value;
    for(nodeHead = nodeRoot->children; 
				nodeHead; nodeHead = nodeHead->next){
		if(xmlStrcasecmp(nodeHead->name, BAD_CAST"element")==0)
		{
			type = xmlGetProp(nodeHead, BAD_CAST"type");
			if(xmlStrcasecmp(type, BAD_CAST"home_pose")==0)
			{
				parseHomePoseElement(nodeHead, home_pose_exp);
			}
			else if(xmlStrcasecmp(type, BAD_CAST"boolean_operation")==0)
			{
				value = xmlNodeGetContent(nodeHead);
				if(xmlStrcasecmp(value, BAD_CAST"AND")==0)
				{
					sprintf(home_pose_exp, "%s AND", home_pose_exp);
				}
				else if(xmlStrcasecmp(value, BAD_CAST"OR")==0)
				{
					sprintf(home_pose_exp, "%s OR", home_pose_exp);
				}
			}
			else if(xmlStrcasecmp(type, BAD_CAST"bracket")==0)
			{
				sprintf(home_pose_exp, "%s (", home_pose_exp);
				parseHomePose(nodeHead, home_pose_exp);
				sprintf(home_pose_exp, "%s )", home_pose_exp);
			}
			else if(xmlStrcasecmp(type, BAD_CAST"num")==0)
			{
				value = xmlNodeGetContent(nodeHead);
				if(xmlStrcasecmp(value, BAD_CAST"-1")==0)
				{
					return 1;
				}
			}
		}
	}
	return 1 ;
}

int parse_home_pose_exp(char *valuestring, char * home_pose_exp)
{	
    xmlDocPtr doc;
    xmlNodePtr rootProg ;
	
	string strValue = string(valuestring); 

    doc = xmlRecoverDoc((const unsigned char *)strValue.c_str());
    rootProg = xmlDocGetRootElement(doc);
	
	parseHomePose(rootProg, home_pose_exp);
	
	xmlFreeDoc(doc);
//	xmlCleanupParser();
	return 1 ;
}

// -------------------------------- cJSON --------------------------------

/************************************************* 
	Function:		parseCart
	Description:	Analyze the PoseEuler info in the properity json file .
	Input:			thread_control_block  - interpreter info
	Input:			jsonCart        - cJSON object
	Input:			cart            - PoseEuler object
	Output: 		NULL
	Return: 		1 - success
*************************************************/
int parseCart(struct thread_control_block * objThreadCntrolBlock, 
			   cJSON *jsonCart, PoseEuler & cart)
{
	// int numentries=0; int i=0; int fail=0;
	cJSON *child=jsonCart->child;

	while (child) //  && !fail)
	{
	//	FST_INFO("parsePosesContent: cJSON_Array %s", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// FST_INFO("cJSON_Number %f", child->valuedouble); 
			if(strcmp(child->string, "a") == 0)
#ifndef WIN32
				cart.euler_.a_ = child->valuedouble;
#else
				cart.orientation.a = child->valuedouble;
#endif
			else if(strcmp(child->string, "b") == 0)
#ifndef WIN32
				cart.euler_.b_ = child->valuedouble;
#else
				cart.orientation.b = child->valuedouble;
#endif
			else if(strcmp(child->string, "c") == 0)
#ifndef WIN32
				cart.euler_.c_ = child->valuedouble;
#else
				cart.orientation.c = child->valuedouble;
#endif
			else if(strcmp(child->string, "x") == 0)
#ifndef WIN32
				cart.point_.x_ = child->valuedouble;
#else
				cart.position.x = child->valuedouble;
#endif
			else if(strcmp(child->string, "y") == 0)
#ifndef WIN32
				cart.point_.y_ = child->valuedouble;
#else
				cart.position.y = child->valuedouble;
#endif
			else if(strcmp(child->string, "z") == 0)
#ifndef WIN32
				cart.point_.z_ = child->valuedouble;
#else
				cart.position.z = child->valuedouble;
#endif
			break;
		case cJSON_String:	
			FST_INFO("cJSON_String %s", child->valuestring); break;
		case cJSON_Object:	
			// FST_INFO("cJSON_Object"); 
			break;
		}
		child=child->next;
	}
	return 1;
}

/************************************************* 
	Function:		parseJoint
	Description:	Analyze the Joint info in the properity json file .
	Input:			thread_control_block  - interpreter info
	Input:			jsonJoint       - cJSON object
	Input:			joint           - joint object
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int parseJoint(struct thread_control_block * objThreadCntrolBlock, 
			  cJSON *jsonJoint, Joint & joint)
{
	// int numentries=0,i=0,fail=0;
	cJSON *child=jsonJoint->child;
	
	while (child) //  && !fail)
	{
	//	FST_INFO("parsePosesContent: cJSON_Array %s", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// FST_INFO("cJSON_Number %f", child->valuedouble); 
			if(strcmp(child->string, "j1") == 0)
#ifndef WIN32
				joint.j1_ = child->valuedouble;
#else
				joint.j1 = child->valuedouble;
#endif
			else if(strcmp(child->string, "j2") == 0)
#ifndef WIN32
				joint.j2_ = child->valuedouble;
#else
				joint.j2 = child->valuedouble;
#endif
			else if(strcmp(child->string, "j3") == 0)
#ifndef WIN32
				joint.j3_ = child->valuedouble;
#else
				joint.j3 = child->valuedouble;
#endif
			else if(strcmp(child->string, "j4") == 0)
#ifndef WIN32
				joint.j4_ = child->valuedouble;
#else
				joint.j4 = child->valuedouble;
#endif
			else if(strcmp(child->string, "j5") == 0)
#ifndef WIN32
				joint.j5_ = child->valuedouble;
#else
				joint.j5 = child->valuedouble;
#endif
			else if(strcmp(child->string, "j6") == 0)
#ifndef WIN32
				joint.j6_ = child->valuedouble;
#else
				joint.j6 = child->valuedouble;
#endif
			break;
		case cJSON_String:	
			FST_INFO("cJSON_String %s", child->valuestring); break;
		case cJSON_Object:	
			// FST_INFO("cJSON_Object"); 
			break;
		}
		child=child->next;
	}
	return 1;
}

/************************************************* 
	Function:		parseAdditionalE
	Description:	Analyze the Additional info in the properity json file .
	Input:			thread_control_block  - interpreter info
	Input:			jsonAdditionalE       - cJSON object
	Input:			additionalE           - AdditionalE object
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int parseAdditionalE(struct thread_control_block * objThreadCntrolBlock, 
			   cJSON *jsonAdditionalE, AdditionalE & additionalE)
{
	// int numentries=0,i=0,fail=0;
	cJSON *child=jsonAdditionalE->child;
	
	while (child) //  && !fail)
	{
	//	FST_INFO("parsePosesContent: cJSON_Array %s", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// FST_INFO("cJSON_Number %f", child->valuedouble); 
			if(strcmp(child->string, "e1") == 0)
				additionalE.e1 = child->valuedouble;
			else if(strcmp(child->string, "e2") == 0)
				additionalE.e2 = child->valuedouble;
			else if(strcmp(child->string, "e3") == 0)
				additionalE.e3 = child->valuedouble;
			break;
		case cJSON_String:	
			FST_INFO("cJSON_String %s", child->valuestring); break;
		case cJSON_Object:	
			// FST_INFO("cJSON_Object"); 
			break;
		}
		child=child->next;
	}
	return 1;
}

/************************************************* 
	Function:		parsePoses
	Description:	Analyze the P[*] in the properity json file .
	Input:			thread_control_block  - interpreter info
	Input:			jsonPose              - cJSON object
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int parsePosesContent(struct thread_control_block * objThreadCntrolBlock, 
			   cJSON *jsonPoseContent)
{
	int id = -1 , uf = -1 , tf = -1 ;
	int iPoseType = POSE_NONE ;
	Joint joint ;
	PoseEuler cart ;
	AdditionalE additionalE ;
	char var[128];
	eval_value value ;

	int numentries=0; // ,i=0,fail=0;
	cJSON *child=jsonPoseContent->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonPoseContent->child;
	while (child) //  && !fail)
	{
	//	FST_INFO("parsePosesContent: cJSON_Array %s", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// FST_INFO("cJSON_Number %d", child->valueint);
			if(strcmp(child->string, "id") == 0)
			{
				id = child->valueint ;
			}
			else if(strcmp(child->string, "uf") == 0)
			{
				uf = child->valueint ;
			}
			else if(strcmp(child->string, "tf") == 0)
			{
				tf = child->valueint ;
			}
			break;
		case cJSON_String:
			if(strcmp(child->string, "poseType") == 0)
			{
				if(strcmp(child->valuestring, "cart") == 0)
					iPoseType = POSE_CART;
				else if(strcmp(child->valuestring, "joint") == 0)
					iPoseType = POSE_JOINT;
			}
			break;
		case cJSON_Object:	
			// FST_INFO("cJSON_Object"); 
			if(strcmp(child->string, "cart") == 0)
				parseCart(objThreadCntrolBlock, child, cart);
			else if(strcmp(child->string, "joint") == 0)
				parseJoint(objThreadCntrolBlock, child, joint);
			else if(strcmp(child->string, "additionalE") == 0)
				parseAdditionalE(objThreadCntrolBlock, child, additionalE);
			break;
		}
		child=child->next;
	}
	if (id >= 0)
	{
		sprintf(var, "p[%d]", id);
	}
	else
		return 0;

	if(iPoseType == POSE_CART)
	{
		value.setPoseValue(&cart);
	}
	else if(iPoseType == POSE_JOINT)
	{
		value.setJointValue(&joint);
	}
	else
		return 0;
	
	value.setUFIndex(uf);   value.setTFIndex(tf);  
	value.updateAdditionalE(additionalE);
	
	// assign_var(objThreadCntrolBlock, var, value);
    var_type vt;
    memset(vt.var_name, 0x00, LAB_LEN);
    strcpy(vt.var_name, var),  vt.value = value;
    objThreadCntrolBlock->local_var_stack.push_back(vt);
	return 1;
}

/************************************************* 
	Function:		parsePoses
	Description:	Analyze the P[*] in the properity json file .
	Input:			thread_control_block  - interpreter info
	Input:			jsonPose              - cJSON object
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int parsePoses(struct thread_control_block * objThreadCntrolBlock, 
			 cJSON *jsonPose)
{
	int numentries=0; // ,i=0,fail=0;
	cJSON *child=jsonPose->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonPose->child;
	// include NULL check
	while (child) //  && !fail)
	{
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// FST_INFO("cJSON_Number %s : %d", 
			//   	child->string, child->valueint); 
			break;
		case cJSON_String:	
			FST_INFO("cJSON_String %s : %s", 
				child->string, child->valuestring); 
			break;
		case cJSON_Object:	
			// FST_INFO("cJSON_Object %s ", child->string); 
			parsePosesContent(objThreadCntrolBlock, child);
			break;
		}
		child=child->next;
	}
	return 1;
}

/************************************************* 
	Function:		parseProgramProp
	Description:	Analyze the properity json file of program.
	Input:			thread_control_block  - interpreter info
	Input:			data                  - file content
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int parseProgramProp(struct thread_control_block * objThreadCntrolBlock, char * data, bool isMainThread)
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
			FST_INFO("cJSON_True"); break;
		case cJSON_Number:
			if(isMainThread)
			{
				if(strcmp(child->string, "launchCode") == 0)
				{
					FST_INFO("cJSON_Number %s : %d", 
						child->string, child->valueint);
				}
			}
			break;
		case cJSON_String:	
			if(strcmp(child->string, "programType") == 0)
			{
				if(isMainThread)
				{
					if(strcmp(child->valuestring, "Monitor") == 0)
					{
						objThreadCntrolBlock->is_main_thread = MONITOR_THREAD ;
					}
					else {
						objThreadCntrolBlock->is_main_thread = MAIN_THREAD ;
					}
				}
			}
			else if(strcmp(child->string, "homePose") == 0)
			{
				memset(objThreadCntrolBlock->home_pose_exp, 0x00, STR_VALUE_SIZE);

				if(strlen(child->valuestring) < STR_VALUE_SIZE)
				{
					parse_home_pose_exp(child->valuestring, 
						objThreadCntrolBlock->home_pose_exp) ;
				//	FST_INFO("home_pose_exp = %s", objThreadCntrolBlock->home_pose_exp);
				}
				else {
					strcpy(objThreadCntrolBlock->home_pose_exp, "ERROR") ;
				}
			}
			break;
		case cJSON_Array:
			FST_INFO("parseDIOMap: cJSON_Array %s", child->string);
			if(strcmp(child->string, "poses") == 0)
				parsePoses(objThreadCntrolBlock, child);
			break;
		case cJSON_Object:	
			// FST_INFO("cJSON_Object"); 
			break;
		}
		child = child->next ;
	}
	cJSON_Delete(json);
	return 1;
}

/************************************************* 
	Function:		print_program_prop_poses
	Description:	print all of P[*] in the program properity info.
	Input:			thread_control_block  - interpreter info
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int print_program_prop_poses(struct thread_control_block * objThreadCntrolBlock)
{
    for(unsigned i=0; i < objThreadCntrolBlock->local_var_stack.size(); i++)
    {
		FST_INFO("%d : %s ", i, objThreadCntrolBlock->local_var_stack[i].var_name);
	}
	return 1;
}

/************************************************* 
	Function:		append_program_prop_mapper
	Description:	Analyze the properity json file of program.
	Input:			thread_control_block  - interpreter info
	Input:			filename - program filename
	Output: 		NULL
	Return: 		1 - success
*************************************************/ 
int append_program_prop_mapper(struct thread_control_block * objThreadCntrolBlock, 
				char *filename, bool isMainThread)
{
	char fname[128];
	FILE *f;long len;char *data;
	
#ifdef WIN32
    sprintf(fname, "%s.json", filename);
#else
    sprintf(fname, "%s/programs/%s.json", 
    		forgesight_get_programs_path(), filename);
#endif
	f=fopen(fname,"rb"); 
	if(f)
	{
	    fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
	    data=(char*)malloc(len+1); fread(data,1,len,f); 
		fclose(f);
		parseProgramProp(objThreadCntrolBlock, data, isMainThread);
		free(data);
	}
	
#ifdef WIN32
      // NOTICE: It would sometimes Segmentation fault.
      print_program_prop_poses(objThreadCntrolBlock);
#endif
	return 1;
}



