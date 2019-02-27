/**
 * @file forsight_home_pose.h
 * @brief 
 * @author Lujiaming
 * @version 1.0.0
 * @date 2018-04-12
 */
#ifndef FORSIGHT_HOME_POSE_H_
#define FORSIGHT_HOME_POSE_H_

#ifdef WIN32
#include "fst_datatype.h" 
using namespace fst_controller;
#pragma warning(disable : 4786)
#else
#include <atomic>
#include <string>
#include <pb_encode.h>
#include "common.h"
#include "interpreter_common.h"
#include "error_code.h"
#include "basic_alg_datatype.h" 
using namespace basic_alg;
#endif

#include <vector>
#include <string>
#include <map>

#include "forsight_cJSON.h"

typedef enum _checkHomePoseResult
{
    HOME_POSE_NONE,
	HOME_POSE_WITHIN_CUR_POS, 
	HOME_POSE_NOT_WITHIN_CUR_POS, 
	HOME_POSE_NOT_EXIST, 
}checkHomePoseResult;

typedef struct home_pose_t
{
	char name[256];
	int  groupNumber ;
	Joint joint ;
	Joint jointFloat ;
} HomePose;

class HomePoseMgr
{
  public:	 
    HomePoseMgr(std::string path);
    ~HomePoseMgr();
    int initial(std::string path);
    
    /**
     * @brief: get Program by Code
     *
     * @param iLaunchCode: Launch Code
     *
     * @return: Program Name
     */
    HomePose getHomePoseByIdx(int iHomePoseIdx);


    /**
     * @brief: get all Launch Code again
     *
     *
     * @return: 0 if success 
     */
    int updateAll();

    std::map<int, HomePose>  getAllHomePose(){ return homePoseList; }
	int printHomePoseList();

	checkHomePoseResult checkSingleHomePoseByJoint(int idx, Joint currentJoint);

  private:
  	int readHomePoseFileList(char *basePath);
	int parseHomePoseFile(char *filename, char * progName);
	int parseHomePose(char * data, HomePose& homePos);
	int parseJoint(cJSON *jsonJoint, HomePose& homePos);
	
  private:
	int parseAdditional(cJSON *jsonAdditional, HomePose& homePos);
	int parseJointValueAndFloat(
			cJSON *jsonJoint, double& value, double& floatValue);
	
  private:
    std::map<int, HomePose>  homePoseList; 
	std::string program_path;
};

#endif

