/**
 * @file macro_instr_mgr.h
 * @brief 
 * @author Lujiaming
 * @version 1.0.0
 * @date 2018-07-10
 */
#ifndef MACRO_INSTR_MGR_H_
#define MACRO_INSTR_MGR_H_

#ifdef WIN32
#pragma warning(disable : 4786)
#endif
#ifndef WIN32
#include <atomic>
#include <pb_encode.h>
#include "common.h"
#include "common/interpreter_common.h"
#include "common/error_code.h"
#endif
#include "forsight_cJSON.h"

#include <vector>
#include <string>
#include <map>

#ifdef WIN32
#define MACRO_INSTR_CONFIG_JSON    "macro_instr_config.json"
#else
#define MACRO_INSTR_CONFIG_JSON    "share/configuration/configurable/macro_instr_config.json"
#endif

typedef struct macro_instr_t {
    char macro_name[128]; 
    char program_name[128]; 
    char io_name[32]; 
} macro_instr;

typedef struct io_macro_instr_t {
    char macro_name[128]; 
    char program_name[128]; 
	int  iValue ;
	bool bIsRunning ;
} io_macro_instr;

class MacroInstrMgr
{
  public:	 
    MacroInstrMgr();
    ~MacroInstrMgr();
    int initial();
	/* use for generating data for test */
	int generateEmptyJsonConfig();
    
    /**
     * @brief: get Program by Macro Name
     *
     * @param macroName: Macro Name
     *
     * @return: Program Name
     */
    std::string getProgramByMacroName(std::string macroName);

    /**
     * @brief: Add/del/update JsonConfig
     */
	int AddSingleJsonConfig(macro_instr macroInstr);
	int delSingleJsonConfig(char* macro_name);
	int updateSingleJsonConfig(macro_instr macroInstr);
	
	bool getRunningInMacroInstrList(char* program_name);
	void setRunningInMacroInstrList(char* program_name);
	void resetRunningInMacroInstrList(char* program_name);
    /**
 	 * @brief: get IOConfig
     *
     * @return: Program Name
     */
	// int refreshAllIOConfig(std::map<std::string, int>& retMap);
    /**
     * @brief: get all Launch Code again
     *
     *
     * @return: 0 if success 
     */
    int updateAll();
	
  private:
	int parseMacroInstrFile(char *filename, char * progName);
	int parseAllMacroInstr(char * data);
	int parseSingleMacroInstr(cJSON * item);
	
	int updateSingleMacroInstr(cJSON * item, macro_instr macroInstr);

    /**
     * @brief: operation of macroInstrMgrList
     */
    // std::map<std::string, macro_instr>  getAllMacroInstr(){ return macroInstrMgrList; }
	int printMacroInstrList();
	void clearMacroInstrList();
	void addToMacroInstrList(macro_instr objMacroInstr);
	void updateMacroInstrList(macro_instr objMacroInstr);
	void delFromMacroInstrList(char* macro_name);

	cJSON* macro_instr_root ;

	// This variable can not modify directly
	// in order to maintain the coherence between macroInstrMgrList and ioMgrList
	// It only to be operated by those functions above 
    std::map<std::string, macro_instr>     macroInstrMgrList; 
  public:
    std::map<std::string, io_macro_instr>  ioMgrList;
};

#endif
