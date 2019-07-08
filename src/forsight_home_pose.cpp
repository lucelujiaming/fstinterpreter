/**
 * @file forsight_home_pose.cpp
 * @brief 
 * @author Lujiaming
 * @version 1.0.0
 * @date 2018-04-12
 */
	 
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#include <boost/algorithm/string.hpp>
#include "common.h"
#include "error_code.h"
#else
#pragma warning(disable : 4786)
#endif
#include <string>

#include "forsight_home_pose.h"

#define INVALID_LAUNCHCODE   -1

HomePoseMgr::HomePoseMgr(std::string path)
{
	program_path = path;
#ifdef WIN32
	program_path += std::string("\\home\\custom\\");
#else
	program_path += std::string("/home/custom/");
#endif
    int result = initial(program_path);
    if (result != 0)
    {
        // rcs::Error::instance()->add(result);
    }
}

HomePoseMgr::~HomePoseMgr()
{
	homePoseList.clear();
}

int HomePoseMgr::parseJointValueAndFloat(
										 cJSON *jsonJoint, double& value, double& floatValue)
{
	int numentries=0; // ,i=0,fail=0;
	cJSON *child=jsonJoint->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonJoint->child;
	// include NULL check
	while (child) //  && !fail)
	{
		switch ((child->type)&255)
		{
		case cJSON_Number:	
			if(strcmp(child->string, "value") == 0)
			{
				value = child->valuedouble;
			}
			else if(strcmp(child->string, "float") == 0)
			{
				floatValue = child->valuedouble;
			}
			break;
		case cJSON_String:	
			// FST_INFO("cJSON_String %s : %s", 
			//		child->string, child->valuestring); 
			break;
		case cJSON_Object:
			break;
		}
		child=child->next;
	}
	return 1;
}

int HomePoseMgr::parseAdditional(cJSON *jsonJoint, HomePose& homePos)
{
	int numentries=0; // ,i=0,fail=0;
	cJSON *child=jsonJoint->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonJoint->child;
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
			// FST_INFO("cJSON_String %s : %s", 
			//		child->string, child->valuestring); 
			break;
		case cJSON_Object:	
			if(strcmp(child->string, "e1") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j7_, homePos.jointFloat.j7_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j7, homePos.jointFloat.j7);
#endif
			}
			else if(strcmp(child->string, "e2") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j8_, homePos.jointFloat.j8_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j8, homePos.jointFloat.j8);
#endif
			}
			else if(strcmp(child->string, "e3") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j9_, homePos.jointFloat.j9_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j9, homePos.jointFloat.j9);
#endif
			}
			break;
		}
		child=child->next;
	}
	return 1;
}

int HomePoseMgr::parseJoint(cJSON *jsonJoint, HomePose& homePos)
{
	int numentries=0; // ,i=0,fail=0;
	cJSON *child=jsonJoint->child;
	while (child) 
		numentries++, child=child->next;
	
	child=jsonJoint->child;
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
			// FST_INFO("cJSON_String %s : %s", 
			//		child->string, child->valuestring); 
			break;
		case cJSON_Object:
			if(strcmp(child->string, "j1") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j1_, homePos.jointFloat.j1_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j1, homePos.jointFloat.j1);
#endif
			}
			else if(strcmp(child->string, "j2") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j2_, homePos.jointFloat.j2_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j2, homePos.jointFloat.j2);
#endif
			}
			else if(strcmp(child->string, "j3") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j3_, homePos.jointFloat.j3_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j3, homePos.jointFloat.j3);
#endif
			}
			else if(strcmp(child->string, "j4") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j4_, homePos.jointFloat.j4_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j4, homePos.jointFloat.j4);
#endif
			}
			else if(strcmp(child->string, "j5") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j5_, homePos.jointFloat.j5_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j5, homePos.jointFloat.j5);
#endif
			}
			else if(strcmp(child->string, "j6") == 0)
			{
#ifndef WIN32
				parseJointValueAndFloat(child, 
					homePos.joint.j6_, homePos.jointFloat.j6_);
#else
				parseJointValueAndFloat(child, 
					homePos.joint.j6, homePos.jointFloat.j6);
#endif
			}
			break;
		}
		child=child->next;
	}
	return 1;
}

int HomePoseMgr::parseHomePose(char * data, HomePose& homePos)
{
	int iRet = 0 ;
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
			break;
		case cJSON_String:
			if(strcmp(child->string, "name") == 0)
			{
				strcpy(homePos.name, 
					child->valuestring);
			}
			else if(strcmp(child->string, "groupNumber") == 0)
			{
				homePos.groupNumber = 
					(int)atoi(child->valuestring) ;
			}
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:	
			if(strcmp(child->string, "joint") == 0)
			{
				parseJoint(child, homePos);
			}
			else if(strcmp(child->string, "additional") == 0)
			{
				parseAdditional(child, homePos);
			}
			break;
		}
		child = child->next ;
	}
	cJSON_Delete(json);

	return 1;
}


int HomePoseMgr::parseHomePoseFile(char *filename, char * progName)
{
	HomePose currentHomePose ; 
	int iCode = 0;
	FILE *f;long len;char *data;

	f=fopen(filename,"rb"); 
	if(f)
	{
	    fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
	    data=(char*)malloc(len+1); fread(data,1,len,f); 
		fclose(f);
		iCode = parseHomePose(data, currentHomePose);
		if(iCode == 1)
		{
			iCode = atoi(progName);
			homePoseList.insert(std::pair<int, HomePose>(iCode, currentHomePose));
		}
		free(data);
	}
	return 1;
}

int HomePoseMgr::readHomePoseFileList(char *basePath)
{
    char filename[256];
#ifdef WIN32
    char progName[256];
	for(int i = 0 ; i < 10 ; i++)
	{
		memset(filename, 0x00, 256);
		itoa(i, filename, 10);
		sprintf(progName, "%s\\%d.json", basePath, i);
		if((atoi(filename) != 0)
			|| (strcmp(filename, "0") == 0))
		{
			parseHomePoseFile(progName, filename);
		}
	}
#else
    char base[1000];
    DIR *dir;
    struct dirent *ptr;
	char * strExtPtr ;

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        return -1;
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
        {
        //	printf("d_name:%s/%s\n",basePath,ptr->d_name);
			strExtPtr = strrchr(ptr->d_name, '.');
			if(strExtPtr)
			{
				if(strcmp(strExtPtr, ".json") == 0)
				{
		            memset(base,'\0',sizeof(base));
		            strcpy(base,basePath);
		            strcat(base,"/");
		            strcat(base,ptr->d_name);
					
		            memset(filename,'\0',sizeof(filename));
					strcpy(filename, ptr->d_name);
					strExtPtr = strrchr(filename, '.');
					strExtPtr[0] = '\0';
				//	if(strcmp(filename, "index") != 0)
					if((atoi(filename) != 0)
						|| (strcmp(filename, "0") == 0))
					{
						parseHomePoseFile(base, filename);
					}
				}
			}
        }
        else if(ptr->d_type == 10)    ///link file
        {
            ; // printf("d_name:%s/%s\n",basePath,ptr->d_name);
        }
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readHomePoseFileList(base);
        }
    }
    closedir(dir);
#endif
    return 1;
}

int HomePoseMgr::initial(std::string path)
{
	int iRet = readHomePoseFileList((char *)path.c_str());
	// printHomePoseList();
	return iRet;
}
	
int HomePoseMgr::updateAll()
{	
	homePoseList.clear();
    return initial(program_path);
}

checkHomePoseResult HomePoseMgr::checkSingleHomePoseByJoint(int idx, Joint currentJoint)
{
	std::map<int, HomePose>::iterator it =homePoseList.find(idx);
	if (it==homePoseList.end())
		return HOME_POSE_NOT_EXIST;
	Joint joint      = homePoseList[idx].joint ;
	Joint jointFloat = homePoseList[idx].jointFloat ;
#ifndef WIN32
	if ((currentJoint.j1_ >= joint.j1_ - jointFloat.j1_) &&(currentJoint.j1_ <= joint.j1_ + jointFloat.j1_)
		&&(currentJoint.j2_ >= joint.j2_ - jointFloat.j2_) &&(currentJoint.j2_ <= joint.j2_ + jointFloat.j2_)
		&&(currentJoint.j3_ >= joint.j3_ - jointFloat.j3_) &&(currentJoint.j3_ <= joint.j3_ + jointFloat.j3_)
		&&(currentJoint.j4_ >= joint.j4_ - jointFloat.j4_) &&(currentJoint.j4_ <= joint.j4_ + jointFloat.j4_)
		&&(currentJoint.j5_ >= joint.j5_ - jointFloat.j5_) &&(currentJoint.j5_ <= joint.j5_ + jointFloat.j5_)
		&&(currentJoint.j6_ >= joint.j6_ - jointFloat.j6_) &&(currentJoint.j6_ <= joint.j6_ + jointFloat.j6_)
		&&(currentJoint.j7_ >= joint.j7_ - jointFloat.j7_) &&(currentJoint.j7_ <= joint.j7_ + jointFloat.j7_)
		&&(currentJoint.j8_ >= joint.j8_ - jointFloat.j8_) &&(currentJoint.j8_ <= joint.j8_ + jointFloat.j8_)
		&&(currentJoint.j9_ >= joint.j9_ - jointFloat.j9_) &&(currentJoint.j9_ <= joint.j9_ + jointFloat.j9_))
#else
	if ((currentJoint.j1 >= joint.j1 - jointFloat.j1) &&(currentJoint.j1 <= joint.j1 + jointFloat.j1)
		&&(currentJoint.j2 >= joint.j2 - jointFloat.j2) &&(currentJoint.j2 <= joint.j2 + jointFloat.j2)
		&&(currentJoint.j3 >= joint.j3 - jointFloat.j3) &&(currentJoint.j3 <= joint.j3 + jointFloat.j3)
		&&(currentJoint.j4 >= joint.j4 - jointFloat.j4) &&(currentJoint.j4 <= joint.j4 + jointFloat.j4)
		&&(currentJoint.j5 >= joint.j5 - jointFloat.j5) &&(currentJoint.j5 <= joint.j5 + jointFloat.j5)
		&&(currentJoint.j6 >= joint.j6 - jointFloat.j6) &&(currentJoint.j6 <= joint.j6 + jointFloat.j6)
		&&(currentJoint.j7 >= joint.j7 - jointFloat.j7) &&(currentJoint.j7 <= joint.j7 + jointFloat.j7)
		&&(currentJoint.j8 >= joint.j8 - jointFloat.j8) &&(currentJoint.j8 <= joint.j8 + jointFloat.j8)
		&&(currentJoint.j9 >= joint.j9 - jointFloat.j9) &&(currentJoint.j9 <= joint.j9 + jointFloat.j9))
#endif
	{
		return HOME_POSE_WITHIN_CUR_POS ;
	}
    return HOME_POSE_NOT_WITHIN_CUR_POS ;
}

HomePose HomePoseMgr::getHomePoseByIdx(int iLaunchCode)
{
	HomePose homePoseFake ;
#ifndef WIN32
	homePoseFake.joint.j1_ = homePoseFake.joint.j2_ = 
		homePoseFake.joint.j3_ = homePoseFake.joint.j4_ = 
		homePoseFake.joint.j5_ = homePoseFake.joint.j6_ = 
		homePoseFake.joint.j7_ = homePoseFake.joint.j8_ = 
		homePoseFake.joint.j9_ = 0 ;
#else
	homePoseFake.joint.j1 = homePoseFake.joint.j2 = 
		homePoseFake.joint.j3 = homePoseFake.joint.j4 = 
		homePoseFake.joint.j5 = homePoseFake.joint.j6 = 
		homePoseFake.joint.j7 = homePoseFake.joint.j8 = 
		homePoseFake.joint.j9 = 0 ;
#endif
	std::map<int, HomePose>::iterator it =homePoseList.find(iLaunchCode);
	if (it==homePoseList.end())
		return homePoseFake;
    else
    	return homePoseList[iLaunchCode];
}

int HomePoseMgr::printHomePoseList()
{
	std::map<int, HomePose>::iterator it;

	it = homePoseList.begin();

	while(it != homePoseList.end())
	{
		// it->first;  // it->second;
#ifndef WIN32
		printf("Get JOINT: %d :: (%f, %f, %f, %f, %f, %f, %f, %f, %f) \n", it->first, 
			it->second.joint.j1_, it->second.joint.j2_, it->second.joint.j3_, 
			it->second.joint.j4_, it->second.joint.j5_, it->second.joint.j6_,  
			it->second.joint.j7_, it->second.joint.j8_, it->second.joint.j9_);
		printf("with jointFloat:(%f, %f, %f, %f, %f, %f, %f, %f, %f) \n", 
			it->second.jointFloat.j1_, it->second.jointFloat.j2_, it->second.jointFloat.j3_, 
			it->second.jointFloat.j4_, it->second.jointFloat.j5_, it->second.jointFloat.j6_, 
			it->second.jointFloat.j7_, it->second.jointFloat.j8_, it->second.jointFloat.j9_);
#else
		printf("Get JOINT: %d :: (%f, %f, %f, %f, %f, %f, %f, %f, %f) \n", it->first, 
			it->second.joint.j1, it->second.joint.j2, it->second.joint.j3, 
			it->second.joint.j4, it->second.joint.j5, it->second.joint.j6,  
			it->second.joint.j7, it->second.joint.j8, it->second.joint.j9);
		printf("with jointFloat:(%f, %f, %f, %f, %f, %f, %f, %f, %f) \n", 
			it->second.jointFloat.j1, it->second.jointFloat.j2, it->second.jointFloat.j3, 
			it->second.jointFloat.j4, it->second.jointFloat.j5, it->second.jointFloat.j6, 
			it->second.jointFloat.j7, it->second.jointFloat.j8, it->second.jointFloat.j9);
#endif
		it++;         
	}
	return 1;
}



