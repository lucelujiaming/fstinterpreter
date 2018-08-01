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

#include "forsight_program_property.h"
#include "forsight_cJSON.h"
#include "forsight_basint.h"
#include "forsight_inter_control.h"

#define POSE_NONE      0
#define POSE_CART      1
#define POSE_JOINT     2

int parseCart(struct thread_control_block * objThreadCntrolBlock, 
			   cJSON *jsonCart, PoseEuler & cart)
{
	int numentries=0,i=0,fail=0;
	cJSON *child=jsonCart->child;

	while (child && !fail)
	{
	//	printf("parsePosesContent: cJSON_Array %s\n", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// printf("cJSON_Number %f\n", child->valuedouble); 
			if(strcmp(child->string, "a") == 0)
				cart.orientation.a = child->valuedouble;
			else if(strcmp(child->string, "b") == 0)
				cart.orientation.b = child->valuedouble;
			else if(strcmp(child->string, "c") == 0)
				cart.orientation.c = child->valuedouble;
			else if(strcmp(child->string, "x") == 0)
				cart.position.x = child->valuedouble;
			else if(strcmp(child->string, "y") == 0)
				cart.position.y = child->valuedouble;
			else if(strcmp(child->string, "z") == 0)
				cart.position.z = child->valuedouble;
			break;
		case cJSON_String:	
			printf("cJSON_String %s\n", child->valuestring); break;
		case cJSON_Object:	
			// printf("cJSON_Object\n"); 
			break;
		}
		child=child->next;
	}
	return 1;
}

int parseJoint(struct thread_control_block * objThreadCntrolBlock, 
			  cJSON *jsonJoint, Joint & joint)
{
	int numentries=0,i=0,fail=0;
	cJSON *child=jsonJoint->child;
	
	while (child && !fail)
	{
	//	printf("parsePosesContent: cJSON_Array %s\n", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// printf("cJSON_Number %f\n", child->valuedouble); 
			if(strcmp(child->string, "j1") == 0)
				joint.j1 = child->valuedouble;
			else if(strcmp(child->string, "j2") == 0)
				joint.j2 = child->valuedouble;
			else if(strcmp(child->string, "j3") == 0)
				joint.j3 = child->valuedouble;
			else if(strcmp(child->string, "j4") == 0)
				joint.j4 = child->valuedouble;
			else if(strcmp(child->string, "j5") == 0)
				joint.j5 = child->valuedouble;
			else if(strcmp(child->string, "j6") == 0)
				joint.j6 = child->valuedouble;
			break;
		case cJSON_String:	
			printf("cJSON_String %s\n", child->valuestring); break;
		case cJSON_Object:	
			// printf("cJSON_Object\n"); 
			break;
		}
		child=child->next;
	}
	return 1;
}

int parseAdditionalE(struct thread_control_block * objThreadCntrolBlock, 
			   cJSON *jsonAdditionalE, AdditionalE & additionalE)
{
	int numentries=0,i=0,fail=0;
	cJSON *child=jsonAdditionalE->child;
	
	while (child && !fail)
	{
	//	printf("parsePosesContent: cJSON_Array %s\n", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// printf("cJSON_Number %f\n", child->valuedouble); 
			if(strcmp(child->string, "e1") == 0)
				additionalE.e1 = child->valuedouble;
			else if(strcmp(child->string, "e2") == 0)
				additionalE.e2 = child->valuedouble;
			else if(strcmp(child->string, "e3") == 0)
				additionalE.e3 = child->valuedouble;
			break;
		case cJSON_String:	
			printf("cJSON_String %s\n", child->valuestring); break;
		case cJSON_Object:	
			// printf("cJSON_Object\n"); 
			break;
		}
		child=child->next;
	}
	return 1;
}

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

	int numentries=0,i=0,fail=0;
	cJSON *child=jsonPoseContent->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonPoseContent->child;
	while (child && !fail)
	{
	//	printf("parsePosesContent: cJSON_Array %s\n", child->string);
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// printf("cJSON_Number %d\n", child->valueint);
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
			// printf("cJSON_Object\n"); 
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
	assign_var(objThreadCntrolBlock, var, value);
	return 1;
}

int parsePoses(struct thread_control_block * objThreadCntrolBlock, 
			 cJSON *jsonPose)
{
	int numentries=0,i=0,fail=0;
	cJSON *child=jsonPose->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonPose->child;
	// include NULL check
	while (child && !fail)
	{
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			// printf("cJSON_Number %s : %d\n", 
			//   	child->string, child->valueint); 
			break;
		case cJSON_String:	
			printf("cJSON_String %s : %s\n", 
				child->string, child->valuestring); 
			break;
		case cJSON_Object:	
			// printf("cJSON_Object %s \n", child->string); 
			parsePosesContent(objThreadCntrolBlock, child);
			break;
		}
		child=child->next;
	}
	return 1;
}

int parseProgramProp(struct thread_control_block * objThreadCntrolBlock, char * data)
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
			printf("cJSON_True"); break;
		case cJSON_Number:		
			{
				if(strcmp(child->string, "launchCode") == 0)
				{
					printf("cJSON_Number %s : %d\n", 
						child->string, child->valueint);
				}
			}
			break;
		case cJSON_String:	
			{
				if(strcmp(child->string, "programType") == 0)
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
			break;
		case cJSON_Array:	
			{
				printf("parseDIOMap: cJSON_Array %s\n", child->string);
				if(strcmp(child->string, "poses") == 0)
					parsePoses(objThreadCntrolBlock, child);
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

int print_program_prop_poses(struct thread_control_block * objThreadCntrolBlock)
{
    for(unsigned i=0; i < objThreadCntrolBlock->global_vars.size(); i++)
    {
		printf("%d : %s \n", i, objThreadCntrolBlock->global_vars[i].var_name);
	}
	return 1;
}

int append_program_prop_mapper(struct thread_control_block * objThreadCntrolBlock, char *filename)
{
	char fname[128];
	FILE *f;long len;char *data;
	
#ifdef WIN32
    sprintf(fname, "%s.json", filename);
#else
    sprintf(fname, "%s\/programs\/%s.json", 
    		forgesight_get_programs_path(), filename);
#endif
	f=fopen(fname,"rb"); 
	if(f)
	{
	    fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
	    data=(char*)malloc(len+1); fread(data,1,len,f); 
		fclose(f);
		parseProgramProp(objThreadCntrolBlock, data);
		free(data);
	}
	
#ifdef WIN32
      // NOTICE: It would sometimes Segmentation fault.
      print_program_prop_poses(objThreadCntrolBlock);
#endif
	return 1;
}



