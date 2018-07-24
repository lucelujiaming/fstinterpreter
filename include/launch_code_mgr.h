/**
 * @file launch_code_mgr.h
 * @brief 
 * @author Lujiaming
 * @version 1.0.0
 * @date 2018-04-12
 */
#ifndef LAUNCH_CODE_MGR_H_
#define LAUNCH_CODE_MGR_H_

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#ifndef WIN32
#include <atomic>
#include <string>
#include <pb_encode.h>
#include "common.h"
#include "common/interpreter_common.h"
#include "common/error_code.h"
#endif

#include <vector>
#include <string>
#include <map>

class LaunchCodeMgr
{
  public:	 
    LaunchCodeMgr();
    ~LaunchCodeMgr();
    int initial();
    
    /**
     * @brief: get Program by Code
     *
     * @param iLaunchCode: Launch Code
     *
     * @return: Program Name
     */
    std::string getProgramByCode(int iLaunchCode);


    /**
     * @brief: get all Launch Code again
     *
     *
     * @return: 0 if success 
     */
    int updateAll();

    std::map<int, std::string>  getAllProgramByCode(){ return launchCodeList; }
	int printLaunchCodeList();

  private:
  	int readFileList(char *basePath);
	int parseProgramPropFile(char *filename, char * progName);
	int parseLaunchCode(char * data);
	
    std::map<int, std::string>  launchCodeList; 
};

#endif
