// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innercmd.h"
#include "reg_manager/reg_manager_interface_wrapper.h"
#include "interpreter_common.h"


// Register name
#define TXT_PR    "pr"
#define TXT_SR    "sr"
#define TXT_R     "r"
#define TXT_MR    "mr"

#define TXT_UF    "uf"
#define TXT_TF    "tf"

#define TXT_PL    "pl"


#ifndef WIN32
extern InterpreterPublish  g_interpreter_publish; 
fst_base::ProcessComm* g_process_comm_ptr       = NULL;
fst_base::InterpreterClient* g_objRegManagerInterface = NULL;
fst_base::InterpreterServer* g_objInterpreterServer   = NULL;
using namespace fst_ctrl ;
#endif

/* Return true if c is a delimiter. */
static int isdelim(char c)
{
  if(strchr(" ;,+-<>/*%^=()[]", c) || c==9 || c=='\r' || c=='\n' || c==0) 
    return 1;  
  return 0;
}

static int get_char_token(char * src, char * dst)
{
	char * tmp = src ;
	if(isalpha(*src)) { /* var or command */
		while(!isdelim(*(src))) 
			*dst++=*(src)++;
	}
	return src - tmp ;
}

static int get_num_token(char * src, char * dst)
{
	char * tmp = src ;
	if(isdigit(*src)) { /* var or command */
		while(!isdelim(*(src))) 
			*dst++=*(src)++;
	}
	return src - tmp ;
}


void load_register_data()
{
#ifndef WIN32

	g_process_comm_ptr = fst_base::ProcessComm::getInstance();
    if(fst_base::ProcessComm::getInitErrorCode() != SUCCESS)
    {
        FST_ERROR("load_register_data getInitErrorCode return failed");
        return false;
    }
	if(g_process_comm_ptr->getInterpreterClientPtr()->init() != SUCCESS)
	{
        FST_ERROR("load_register_data getInterpreterClientPtr return false");
		return false;
	}
    if(g_process_comm_ptr->getInterpreterServerPtr()->init() != SUCCESS)
	{
        FST_ERROR("load_register_data getInterpreterServerPtr init return false");
		return false;
	}
    if(g_process_comm_ptr->getInterpreterServerPtr()->open() != SUCCESS)
	{
        FST_ERROR("load_register_data getInterpreterServerPtr open return false");
		return false;
	}
	usleep(10);
	g_objRegManagerInterface = g_process_comm_ptr->getInterpreterClientPtr();
	g_objInterpreterServer   = g_process_comm_ptr->getInterpreterServerPtr();
	g_objRegManagerInterface->setInterpreterServerStatus(true);
	usleep(10);
	memset(&g_interpreter_publish, 0x00, sizeof(InterpreterPublish));
	g_objInterpreterServer->addPublishTask(300, &g_interpreter_publish);

#endif

}

bool reg_manager_interface_getPr(PrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		ptr->value.pos[0] = 0.0;
		ptr->value.pos[1] = 0.0;
		ptr->value.pos[2] = 0.0;
		ptr->value.pos[3] = 0.0;
		ptr->value.pos[4] = 0.0;
		ptr->value.pos[5] = 0.0;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		ptr->value.pos[0] = objPrRegDataIpc.pos[0];
		ptr->value.pos[1] = objPrRegDataIpc.pos[1];
		ptr->value.pos[2] = objPrRegDataIpc.pos[2];
		ptr->value.pos[3] = objPrRegDataIpc.pos[3];
		ptr->value.pos[4] = objPrRegDataIpc.pos[4];
		ptr->value.pos[5] = objPrRegDataIpc.pos[5];
		
		FST_INFO("getPr: id = (%f, %f, %f, %f, %f, %f) at %d with %s ", 
			ptr->value.pos[0], ptr->value.pos[1], 
			ptr->value.pos[2], ptr->value.pos[3], 
			ptr->value.pos[4], ptr->value.pos[5], num, bRet?"TRUE":"FALSE");
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setPr(PrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		objPrRegDataIpc.id     = num;
		objPrRegDataIpc.pos[0] = ptr->value.pos[0];
		objPrRegDataIpc.pos[1] = ptr->value.pos[1];
		objPrRegDataIpc.pos[2] = ptr->value.pos[2];
		objPrRegDataIpc.pos[3] = ptr->value.pos[3];
		objPrRegDataIpc.pos[4] = ptr->value.pos[4];
		objPrRegDataIpc.pos[5] = ptr->value.pos[5];
		objPrRegDataIpc.pos[6] = 0.0;
		objPrRegDataIpc.pos[7] = 0.0;
		objPrRegDataIpc.pos[8] = 0.0;
		
		bRet = g_objRegManagerInterface->setPrReg(&objPrRegDataIpc);
		FST_INFO("setPr: id = %d (%f, %f, %f, %f, %f, %f) at %d with %s ", num, 
			objPrRegDataIpc.pos[0], objPrRegDataIpc.pos[1], 
			objPrRegDataIpc.pos[2], objPrRegDataIpc.pos[3], 
			objPrRegDataIpc.pos[4], objPrRegDataIpc.pos[5], num, bRet?"TRUE":"FALSE");
		
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addPrReg(&objPrRegDataIpc);
		}
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_delPr(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deletePrReg(num);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

/*
 * The operated object is an individual member of PR.
 */
bool reg_manager_interface_getPosePr(PoseEuler *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
		   ptr->position.x    = objPrRegDataIpc.pos[0];
		   ptr->position.y    = objPrRegDataIpc.pos[1];
		   ptr->position.z    = objPrRegDataIpc.pos[2];
		   ptr->orientation.a = objPrRegDataIpc.pos[3];
		   ptr->orientation.b = objPrRegDataIpc.pos[4];
		   ptr->orientation.c = objPrRegDataIpc.pos[5];
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setPosePr(PoseEuler *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		
		objPrRegDataIpc.pos[0] = ptr->position.x   ;
		objPrRegDataIpc.pos[1] = ptr->position.y   ;
		objPrRegDataIpc.pos[2] = ptr->position.z   ;
		objPrRegDataIpc.pos[3] = ptr->orientation.a;
		objPrRegDataIpc.pos[4] = ptr->orientation.b;
		objPrRegDataIpc.pos[5] = ptr->orientation.c;
		objPrRegDataIpc.pos[6] = 0.0;
		objPrRegDataIpc.pos[7] = 0.0;
		objPrRegDataIpc.pos[8] = 0.0;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		FST_INFO("setPr: id = %d (%f, %f, %f, %f, %f, %f) at %d with %s ", num, 
			objPrRegDataIpc.pos[0], objPrRegDataIpc.pos[1], 
			objPrRegDataIpc.pos[2], objPrRegDataIpc.pos[3], 
			objPrRegDataIpc.pos[4], objPrRegDataIpc.pos[5], num, bRet?"TRUE":"FALSE");
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    memcpy(&(objPrRegDataIpc.pos), ptr, 
				sizeof(objPrRegDataIpc.pos));
			reg_manager_interface_setPr(&objPrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getJointPr(Joint *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
		   ptr->j1 = objPrRegDataIpc.pos[0];
		   ptr->j2 = objPrRegDataIpc.pos[1];
		   ptr->j3 = objPrRegDataIpc.pos[2];
		   ptr->j4 = objPrRegDataIpc.pos[3];
		   ptr->j5 = objPrRegDataIpc.pos[4];
		   ptr->j6 = objPrRegDataIpc.pos[5];   
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setJointPr(Joint *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
			objPrRegDataIpc.pos[0] = ptr->j1;
			objPrRegDataIpc.pos[1] = ptr->j2;
			objPrRegDataIpc.pos[2] = ptr->j3;
			objPrRegDataIpc.pos[3] = ptr->j4;
			objPrRegDataIpc.pos[4] = ptr->j5;
			objPrRegDataIpc.pos[5] = ptr->j6;
			objPrRegDataIpc.pos[6] = 0.0;
			objPrRegDataIpc.pos[7] = 0.0;
			objPrRegDataIpc.pos[8] = 0.0;
			bRet = g_objRegManagerInterface->setPrReg(&objPrRegDataIpc);
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getTypePr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   *ptr = objPrRegDataIpc.value.pos_type;
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setTypePr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objPrRegDataIpc.value.pos_type = *ptr;
			reg_manager_interface_setPr(&objPrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getIdPr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   *ptr = objPrRegDataIpc.id;
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setIdPr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objPrRegDataIpc.id = *ptr;
			reg_manager_interface_setPr(&objPrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getCommentPr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objPrRegDataIpc.comment.c_str(), objPrRegDataIpc.comment.length());
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setCommentPr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objPrRegDataIpc.comment = string(ptr);
			reg_manager_interface_setPr(&objPrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

/**********************
 ********* SR *********
 **********************/

bool reg_manager_interface_getSr(SrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		ptr->value = "";
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		ptr->value = string(objSrRegDataIpc.value) ;
		FST_INFO("getSr[%d]:(%s) at %d with %s", num, ptr->value.c_str(), num, bRet?"TRUE":"FALSE");
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
    ptr->value = string("");
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setSr(SrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		// memcpy(&objSrRegData, ptr, sizeof(objSrRegData));
		objSrRegDataIpc.id = num ;
		strcpy(objSrRegDataIpc.value, ptr->value.c_str()) ;
		FST_INFO("setSr:(%s) at %d with %s", objSrRegDataIpc.value, num, bRet?"TRUE":"FALSE");
		
		bRet = g_objRegManagerInterface->setSrReg(&objSrRegDataIpc);
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addSrReg(&objSrRegDataIpc);
		}
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_delSr(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deleteSrReg(num);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getValueSr(string &strVal, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		strVal = string(objSrRegDataIpc.value);
		if(bRet)
		{
		   strVal = objSrRegDataIpc.value;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setValueSr(string &strVal, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		objSrRegDataIpc.id    = num;
		strcpy(objSrRegDataIpc.value, strVal.c_str());
		FST_INFO("setValueSr:(%s)", objSrRegDataIpc.value);
		bRet = g_objRegManagerInterface->setSrReg(&objSrRegDataIpc);
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getIdSr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		if(bRet)
		{
		   *ptr = objSrRegDataIpc.id;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setIdSr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		if(bRet)
		{
		    objSrRegDataIpc.id = *ptr;
			bRet = g_objRegManagerInterface->setSrReg(&objSrRegDataIpc);
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getCommentSr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objSrRegDataIpc.comment.c_str(), objSrRegDataIpc.comment.length());
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setCommentSr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objSrRegData.comment = string(ptr);
			reg_manager_interface_setSr(&objSrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

/**********************
 ********* R **********
 **********************/
bool reg_manager_interface_getR(RRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		ptr->value = 0.0;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegDataIpc);
		FST_INFO("getR: value = (%f) at %d with %s", 
			objRRegDataIpc.value, num, bRet?"TRUE":"FALSE");
		ptr->value = objRRegDataIpc.value;
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	ptr->value = 1 ;
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setR(RRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		objRRegDataIpc.id    = num;
		objRRegDataIpc.value = ptr->value;
		// memcpy(&objRRegData, ptr, sizeof(objRRegData));
		bRet = g_objRegManagerInterface->setRReg(&objRRegDataIpc);
		FST_INFO("setR:(%f) at %d with %s", objRRegDataIpc.value, num, bRet?"TRUE":"FALSE");
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addRReg(objRRegDataIpc);
		}
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_delR(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deleteRReg(num);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getValueR(double *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegDataIpc);
		if(bRet)
		{
		   *ptr = objRRegDataIpc.value;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setValueR(double *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		objRRegDataIpc.id    = num;
		objRRegDataIpc.value = *ptr;
		bRet = g_objRegManagerInterface->setRReg(&objRRegDataIpc);
		FST_INFO("setValueR:(%f) at %d with %s", 
			objRRegDataIpc.value, num, bRet?"TRUE":"FALSE");
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getIdR(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegDataIpc);
		if(bRet)
		{
		   *ptr = objRRegDataIpc.id;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setIdR(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
	    objRRegDataIpc.id = *ptr;
		bRet = g_objRegManagerInterface->setRReg(&objRRegDataIpc);
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getCommentR(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objRRegDataIpc.comment.c_str(), objRRegDataIpc.comment.length());
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setCommentR(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objRRegDataIpc.comment = string(ptr);
			reg_manager_interface_setR(&objRRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

/**********************
 ********* MR *********
 **********************/
bool reg_manager_interface_getMr(MrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		ptr->value = 0.0;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		FST_INFO("getMR: value = (%f) at %d with %s", 
			objMrRegDataIpc.value, num, bRet?"TRUE":"FALSE");
		ptr->value = objMrRegDataIpc.value ;
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	ptr->value = 1 ;
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setMr(MrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		objMrRegDataIpc.id = num ;
		objMrRegDataIpc.value = ptr->value ;
		FST_INFO("setR:(%f) at %d with %s", objMrRegDataIpc.value, num, bRet?"TRUE":"FALSE");
		bRet = g_objRegManagerInterface->setMrReg(&objMrRegDataIpc);
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addMrReg(&objMrRegDataIpc);
		}
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}
 
bool reg_manager_interface_delMr(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deleteMrReg(num);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getValueMr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		if(bRet)
		{
		   *ptr = objMrRegDataIpc.value;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setValueMr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		objMrRegDataIpc.id    = num;
		objMrRegDataIpc.value = *ptr;
		bRet = g_objRegManagerInterface->setMrReg(&objMrRegDataIpc);
		FST_INFO("setValueMr:(%f) at %d with %s", objMrRegDataIpc.value, num, bRet?"TRUE":"FALSE");
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getIdMr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		if(bRet)
		{
		   *ptr = objMrRegDataIpc.id;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setIdMr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objMrRegData.id = *ptr;
			reg_manager_interface_setMr(&objMrRegData, num);
#endif
		}

	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getCommentMr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objMrRegDataIpc.comment.c_str(), objMrRegDataIpc.comment.length());
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setCommentMr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objMrRegDataIpc.comment = string(ptr);
			reg_manager_interface_setMr(&objMrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

/**********************
 ********* HR *********
 **********************/

bool reg_manager_interface_getHr(HrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		ptr->value.joint_pos[0] = 0.0;
		ptr->value.joint_pos[1] = 0.0;
		ptr->value.joint_pos[2] = 0.0;
		ptr->value.joint_pos[3] = 0.0;
		ptr->value.joint_pos[4] = 0.0;
		ptr->value.joint_pos[5] = 0.0;
		bRet = g_objRegManagerInterface->getHrReg(num, &objHrRegDataIpc);
		
		ptr->value.joint_pos[0] = objHrRegDataIpc.joint_pos[0];
		ptr->value.joint_pos[1] = objHrRegDataIpc.joint_pos[1];
		ptr->value.joint_pos[2] = objHrRegDataIpc.joint_pos[2];
		ptr->value.joint_pos[3] = objHrRegDataIpc.joint_pos[3];
		ptr->value.joint_pos[4] = objHrRegDataIpc.joint_pos[4];
		ptr->value.joint_pos[5] = objHrRegDataIpc.joint_pos[5];
		
		FST_INFO("getHr: id = (%f, %f, %f, %f, %f, %f) at %d with %s", 
			objHrRegDataIpc.joint_pos[0], objHrRegDataIpc.joint_pos[1], 
			objHrRegDataIpc.joint_pos[2], objHrRegDataIpc.joint_pos[3], 
			objHrRegDataIpc.joint_pos[4], objHrRegDataIpc.joint_pos[5], 
			num, bRet?"TRUE":"FALSE");
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setHr(HrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		objHrRegDataIpc.joint_pos[0] = ptr->value.joint_pos[0];
		objHrRegDataIpc.joint_pos[1] = ptr->value.joint_pos[1];
		objHrRegDataIpc.joint_pos[2] = ptr->value.joint_pos[2];
		objHrRegDataIpc.joint_pos[3] = ptr->value.joint_pos[3];
		objHrRegDataIpc.joint_pos[4] = ptr->value.joint_pos[4];
		objHrRegDataIpc.joint_pos[5] = ptr->value.joint_pos[5];
		
		bRet = g_objRegManagerInterface->setHrReg(&objHrRegDataIpc);
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addHrReg(&objHrRegDataIpc);
		}
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_delHr(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deleteHrReg(num);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getJointHr(Joint *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		bRet = g_objRegManagerInterface->getHrReg(num, &objHrRegDataIpc);
		if(bRet)
		{
			ptr->j1 = objHrRegDataIpc.joint_pos[0];
			ptr->j2 = objHrRegDataIpc.joint_pos[1];
			ptr->j3 = objHrRegDataIpc.joint_pos[2];
			ptr->j4 = objHrRegDataIpc.joint_pos[3];
			ptr->j5 = objHrRegDataIpc.joint_pos[4];
			ptr->j6 = objHrRegDataIpc.joint_pos[5];
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setJointHr(Joint *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		objHrRegDataIpc.joint_pos[0] = ptr->j1;
		objHrRegDataIpc.joint_pos[1] = ptr->j2;
		objHrRegDataIpc.joint_pos[2] = ptr->j3;
		objHrRegDataIpc.joint_pos[3] = ptr->j4;
		objHrRegDataIpc.joint_pos[4] = ptr->j5;
		objHrRegDataIpc.joint_pos[5] = ptr->j6;
		
		bRet = g_objRegManagerInterface->setHrReg(&objHrRegDataIpc);
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getIdHr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		bRet = g_objRegManagerInterface->getHrReg(num, &objHrRegDataIpc);
		if(bRet)
		{
		   *ptr = objHrRegDataIpc.id;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setIdHr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		bRet = g_objRegManagerInterface->getHrReg(num, &objHrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objHrRegData.id = *ptr;
			reg_manager_interface_setHr(&objHrRegData, num);;
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_getCommentHr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		bRet = g_objRegManagerInterface->getHrReg(num, &objHrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objHrRegDataIpc.comment.c_str(), objHrRegDataIpc.comment.length());
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}

bool reg_manager_interface_setCommentHr(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
		bRet = g_objRegManagerInterface->getHrReg(num, &objHrRegDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objHrRegDataIpc.comment = string(ptr);
			reg_manager_interface_setHr(&objHrRegDataIpc, num);
#endif
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return bRet ;
}


/**********************
 ********* UF *********
 **********************/
bool reg_manager_interface_getUf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setUf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getCoordinateUf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setCoordinateUf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getIdUf(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setIdUf(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getCommentUf(char *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setCommentUf(char *ptr, uint16_t num)
{
	return true ;
}

/**********************
 ********* TF *********
 **********************/
bool reg_manager_interface_getTf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setTf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getCoordinateTf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setCoordinateTf(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getIdTf(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setIdTf(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getCommentTf(char *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setCommentTf(char *ptr, uint16_t num)
{
	return true ;
}

/**********************
 ********* PL *********
 **********************/
bool reg_manager_interface_getPl(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setPl(void *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getPosePl(PoseEuler* pose, int index)
{
	return true ;
}

bool reg_manager_interface_setPosePl(PoseEuler* pose, int index)
{
	return true ;
}

bool reg_manager_interface_getPalletPl(pl_t *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setPalletPl(pl_t *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getFlagPl(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setFlagPl(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getIdPl(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setIdPl(int *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_getCommentPl(char *ptr, uint16_t num)
{
	return true ;
}

bool reg_manager_interface_setCommentPl(char *ptr, uint16_t num)
{
	return true ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_pr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	bool bRet = false ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		vecRet = g_objRegManagerInterface->getPrRegValidIdList(0, 255);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_sr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		vecRet = g_objRegManagerInterface->getSrRegValidIdList(0, 255);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_r_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		vecRet = g_objRegManagerInterface->getRRegValidIdList(0, 255);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_mr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		vecRet = g_objRegManagerInterface->getMrRegValidIdList(0, 255);
	bRet = true ;
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_hr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		vecRet = g_objRegManagerInterface->getHrRegValidIdList(0, 255);
#endif
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#endif
	return vecRet ;
}




