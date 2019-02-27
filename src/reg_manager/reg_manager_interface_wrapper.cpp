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

/************************************************* 
	Function:		load_register_data
	Description:	Initialize ProcessComm.
	Input:			NULL
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getPr
	Description:	get PR.
	Input:			num        -  Index of PR
	Ouput:			ptr        -  PR register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getPr(PrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
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
		ptr->value.pos_type = PR_REG_POS_TYPE_CARTESIAN ;
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
	memset(ptr->comment, 0x00, MAX_REG_COMMENT_LENGTH);
	ptr->value.cartesian_pos.position.x = 1.0;
	ptr->value.cartesian_pos.position.y = 2.0;
	ptr->value.cartesian_pos.position.z = 3.0;
	ptr->value.cartesian_pos.orientation.a = 4.0;
	ptr->value.cartesian_pos.orientation.b = 5.0;
	ptr->value.cartesian_pos.orientation.c = 6.0;
	ptr->value.pos_type = PR_REG_POS_TYPE_CARTESIAN ;
	bRet = true ;
#endif
	return bRet ;
}

/************************************************* 
	Function:		reg_manager_interface_setPr
	Description:	set PR.
	Input:			num        -  Index of PR
	Input:			ptr        -  PR register Data
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_delPr
	Description:	delete PR.
	Input:			num        -  Index of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getPosePr
	Description:	get Pose info of PR.
	Input:			num        -  Index of PR
	Ouput:			ptr        -  PoseEuler Data
	Return: 		1 - success
*************************************************/
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
#ifndef WIN32
		   ptr->point_.x_    = objPrRegDataIpc.pos[0];
		   ptr->point_.y_    = objPrRegDataIpc.pos[1];
		   ptr->point_.z_    = objPrRegDataIpc.pos[2];
		   ptr->euler_.a_    = objPrRegDataIpc.pos[3];
		   ptr->euler_.b_    = objPrRegDataIpc.pos[4];
		   ptr->euler_.c_    = objPrRegDataIpc.pos[5];
#else
		   ptr->position.x    = objPrRegDataIpc.pos[0];
		   ptr->position.y    = objPrRegDataIpc.pos[1];
		   ptr->position.z    = objPrRegDataIpc.pos[2];
		   ptr->orientation.a = objPrRegDataIpc.pos[3];
		   ptr->orientation.b = objPrRegDataIpc.pos[4];
		   ptr->orientation.c = objPrRegDataIpc.pos[5];
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

/************************************************* 
	Function:		reg_manager_interface_setPosePr
	Description:	set Pose info of PR.
	Input:			num        -  Index of PR
	Input:			ptr        -  PoseEuler Data of PR
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setPosePr(PoseEuler *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		PrRegDataIpc objPrRegDataIpc ;
		objPrRegDataIpc.id     = num ;
#ifndef WIN32
		objPrRegDataIpc.pos[0] = ptr->point_.x_   ;
		objPrRegDataIpc.pos[1] = ptr->point_.y_   ;
		objPrRegDataIpc.pos[2] = ptr->point_.z_   ;
		objPrRegDataIpc.pos[3] = ptr->euler_.a_   ;
		objPrRegDataIpc.pos[4] = ptr->euler_.b_   ;
		objPrRegDataIpc.pos[5] = ptr->euler_.c_   ;
#else
		objPrRegDataIpc.pos[0] = ptr->position.x   ;
		objPrRegDataIpc.pos[1] = ptr->position.y   ;
		objPrRegDataIpc.pos[2] = ptr->position.z   ;
		objPrRegDataIpc.pos[3] = ptr->orientation.a;
		objPrRegDataIpc.pos[4] = ptr->orientation.b;
		objPrRegDataIpc.pos[5] = ptr->orientation.c;
#endif
		objPrRegDataIpc.pos[6] = 0.0;
		objPrRegDataIpc.pos[7] = 0.0;
		objPrRegDataIpc.pos[8] = 0.0;
		bRet = g_objRegManagerInterface->setPrReg(&objPrRegDataIpc);
		FST_INFO("setPr: id = %d (%f, %f, %f, %f, %f, %f) at %d with %s ", num, 
			objPrRegDataIpc.pos[0], objPrRegDataIpc.pos[1], 
			objPrRegDataIpc.pos[2], objPrRegDataIpc.pos[3], 
			objPrRegDataIpc.pos[4], objPrRegDataIpc.pos[5], num, bRet?"TRUE":"FALSE");
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

/************************************************* 
	Function:		reg_manager_interface_getJointPr
	Description:	get Joint info of PR.
	Input:			num        -  Index of PR
	Ouput:			ptr        -  Joint Data of PR
	Return: 		1 - success
*************************************************/
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
#ifndef WIN32
		   ptr->j1_ = objPrRegDataIpc.pos[0];
		   ptr->j2_ = objPrRegDataIpc.pos[1];
		   ptr->j3_ = objPrRegDataIpc.pos[2];
		   ptr->j4_ = objPrRegDataIpc.pos[3];
		   ptr->j5_ = objPrRegDataIpc.pos[4];
		   ptr->j6_ = objPrRegDataIpc.pos[5];   
#else
		   ptr->j1 = objPrRegDataIpc.pos[0];
		   ptr->j2 = objPrRegDataIpc.pos[1];
		   ptr->j3 = objPrRegDataIpc.pos[2];
		   ptr->j4 = objPrRegDataIpc.pos[3];
		   ptr->j5 = objPrRegDataIpc.pos[4];
		   ptr->j6 = objPrRegDataIpc.pos[5];   
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

/************************************************* 
	Function:		reg_manager_interface_setJointPr
	Description:	set Joint info of PR.
	Input:			num        -  Index of PR
	Input:			ptr        -  Joint Data of PR
	Return: 		1 - success
*************************************************/
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
#ifndef WIN32 
			objPrRegDataIpc.pos[0] = ptr->j1_;
			objPrRegDataIpc.pos[1] = ptr->j2_;
			objPrRegDataIpc.pos[2] = ptr->j3_;
			objPrRegDataIpc.pos[3] = ptr->j4_;
			objPrRegDataIpc.pos[4] = ptr->j5_;
			objPrRegDataIpc.pos[5] = ptr->j6_;
#else
			objPrRegDataIpc.pos[0] = ptr->j1;
			objPrRegDataIpc.pos[1] = ptr->j2;
			objPrRegDataIpc.pos[2] = ptr->j3;
			objPrRegDataIpc.pos[3] = ptr->j4;
			objPrRegDataIpc.pos[4] = ptr->j5;
			objPrRegDataIpc.pos[5] = ptr->j6; 
#endif
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

/************************************************* 
	Function:		reg_manager_interface_getTypePr
	Description:	get Type info of PR.
	Input:			num        -  Index of PR
	Ouput:			ptr        -  Type info of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setTypePr
	Description:	set Type info of PR.
	Input:			num        -  Index of PR
	Input:			ptr        -  Type info of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getIdPr
	Description:	get Id info of PR.
	Input:			num        -  Index of PR
	Ouput:			ptr        -  Id info of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setIdPr
	Description:	set Id info of PR.
	Input:			num        -  Index of PR
	Input:			ptr        -  Id info of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getCommentPr
	Description:	get Comment info of PR.
	Input:			num        -  Index of PR
	Ouput:			ptr        -  Comment info of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setCommentPr
	Description:	set Comment info of PR.
	Input:			num        -  Index of PR
	Input:			ptr        -  Comment info of PR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getSr
	Description:	get SR.
	Input:			num        -  Index of SR
	Ouput:			ptr        -  SR register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getSr(SrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
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
	memset(ptr->comment, 0x00, MAX_REG_COMMENT_LENGTH);
    ptr->value = string("Test");
	bRet = true ;
#endif
	return bRet ;
}

/************************************************* 
	Function:		reg_manager_interface_setSr
	Description:	set SR.
	Input:			num        -  Index of SR
	Input:			ptr        -  SR register Data
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_delSr
	Description:	delete SR.
	Input:			num        -  Index of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getValueSr
	Description:	get string info of SR.
	Input:			num        -  Index of SR
	Ouput:			ptr        -  string info of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setValueSr
	Description:	set string info of SR.
	Input:			num        -  Index of SR
	Input:			ptr        -  string info of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getIdSr
	Description:	get Id info of SR.
	Input:			num        -  Index of SR
	Ouput:			ptr        -  Id info of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setIdSr
	Description:	set Id info of SR.
	Input:			num        -  Index of SR
	Input:			ptr        -  Id info of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getCommentSr
	Description:	get Comment info of SR.
	Input:			num        -  Index of SR
	Ouput:			ptr        -  Comment info of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setCommentPr
	Description:	set Comment info of SR.
	Input:			num        -  Index of SR
	Input:			ptr        -  Comment info of SR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getR
	Description:	get R.
	Input:			num        -  Index of R
	Ouput:			ptr        -  R register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getR(RRegData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
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
	memset(ptr->comment, 0x00, MAX_REG_COMMENT_LENGTH);
	ptr->value = 1 ;
	bRet = true ;
#endif
	return bRet ;
}

/************************************************* 
	Function:		reg_manager_interface_setR
	Description:	set R.
	Input:			num        -  Index of R
	Input:			ptr        -  R register Data
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_delR
	Description:	delete R.
	Input:			num        -  Index of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getValueR
	Description:	get number info of R.
	Input:			num        -  Index of R
	Ouput:			ptr        -  number info of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setValueR
	Description:	set number info of R.
	Input:			num        -  Index of R
	Input:			ptr        -  number info of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getIdR
	Description:	get Id info of R.
	Input:			num        -  Index of R
	Ouput:			ptr        -  Id info of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setIdR
	Description:	set Id info of R.
	Input:			num        -  Index of R
	Input:			ptr        -  Id info of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getCommentR
	Description:	get Comment info of R.
	Input:			num        -  Index of R
	Ouput:			ptr        -  Comment info of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setCommentR
	Description:	set Comment info of R.
	Input:			num        -  Index of R
	Input:			ptr        -  Comment info of R
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getMr
	Description:	get MR.
	Input:			num        -  Index of MR
	Ouput:			ptr        -  MR register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getMr(MrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
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
	memset(ptr->comment, 0x00, MAX_REG_COMMENT_LENGTH);
	ptr->value = 1 ;
	bRet = true ;
#endif
	return bRet ;
}

/************************************************* 
	Function:		reg_manager_interface_setMr
	Description:	set MR.
	Input:			num        -  Index of MR
	Input:			ptr        -  MR register Data
	Return: 		1 - success
*************************************************/
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
 
/************************************************* 
	 Function:		 reg_manager_interface_delMr
	 Description:	 delete MR.
	 Input: 		 num		-  Index of MR
	 Return:		 1 - success
 *************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getValueMr
	Description:	get number info of MR.
	Input:			num        -  Index of MR
	Ouput:			ptr        -  number info of MR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setValueMr
	Description:	set number info of MR.
	Input:			num        -  Index of MR
	Input:			ptr        -  number info of MR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getIdMr
	Description:	get Id info of MR.
	Input:			num        -  Index of MR
	Ouput:			ptr        -  Id info of MR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setIdMr
	Description:	set Id info of MR.
	Input:			num        -  Index of MR
	Input:			ptr        -  Id info of MR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getCommentMr
	Description:	get Comment info of MR.
	Input:			num        -  Index of MR
	Ouput:			ptr        -  Comment info of MR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setCommentMr
	Description:	set Comment info of MR.
	Input:			num        -  Index of MR
	Input:			ptr        -  Comment info of MR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getHr
	Description:	get HR.
	Input:			num        -  Index of HR
	Ouput:			ptr        -  HR register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getHr(HrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
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
	memset(ptr->comment, 0x00, MAX_REG_COMMENT_LENGTH);
	bRet = true ;
#endif
	return bRet ;
}

/************************************************* 
	Function:		reg_manager_interface_setHr
	Description:	set HR.
	Input:			num        -  Index of HR
	Input:			ptr        -  HR register Data
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	 Function:		 reg_manager_interface_delHr
	 Description:	 delete HR.
	 Input: 		 num		-  Index of HR
	 Return:		 1 - success
 *************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getJointHr
	Description:	get Joint info of HR.
	Input:			num        -  Index of HR
	Ouput:			ptr        -  Joint Data of HR
	Return: 		1 - success
*************************************************/
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
#ifndef WIN32
			ptr->j1_ = objHrRegDataIpc.joint_pos[0];
			ptr->j2_ = objHrRegDataIpc.joint_pos[1];
			ptr->j3_ = objHrRegDataIpc.joint_pos[2];
			ptr->j4_ = objHrRegDataIpc.joint_pos[3];
			ptr->j5_ = objHrRegDataIpc.joint_pos[4];
			ptr->j6_ = objHrRegDataIpc.joint_pos[5];
#else
			ptr->j1 = objHrRegDataIpc.joint_pos[0];
			ptr->j2 = objHrRegDataIpc.joint_pos[1];
			ptr->j3 = objHrRegDataIpc.joint_pos[2];
			ptr->j4 = objHrRegDataIpc.joint_pos[3];
			ptr->j5 = objHrRegDataIpc.joint_pos[4];
			ptr->j6 = objHrRegDataIpc.joint_pos[5];
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

/************************************************* 
	Function:		reg_manager_interface_setJointHr
	Description:	set Pose info of HR.
	Input:			num        -  Index of HR
	Input:			ptr        -  PoseEuler Data of HR
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setJointHr(Joint *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		HrRegDataIpc objHrRegDataIpc ;
#ifndef WIN32
		objHrRegDataIpc.joint_pos[0] = ptr->j1_;
		objHrRegDataIpc.joint_pos[1] = ptr->j2_;
		objHrRegDataIpc.joint_pos[2] = ptr->j3_;
		objHrRegDataIpc.joint_pos[3] = ptr->j4_;
		objHrRegDataIpc.joint_pos[4] = ptr->j5_;
		objHrRegDataIpc.joint_pos[5] = ptr->j6_;
#else
		objHrRegDataIpc.joint_pos[0] = ptr->j1;
		objHrRegDataIpc.joint_pos[1] = ptr->j2;
		objHrRegDataIpc.joint_pos[2] = ptr->j3;
		objHrRegDataIpc.joint_pos[3] = ptr->j4;
		objHrRegDataIpc.joint_pos[4] = ptr->j5;
		objHrRegDataIpc.joint_pos[5] = ptr->j6;
#endif
		
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

/************************************************* 
	Function:		reg_manager_interface_getIdHr
	Description:	get Id info of HR.
	Input:			num        -  Index of HR
	Ouput:			ptr        -  Id info of HR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setIdHr
	Description:	set Id info of HR.
	Input:			num        -  Index of HR
	Input:			ptr        -  Id info of HR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_getCommentHr
	Description:	get Comment info of HR.
	Input:			num        -  Index of HR
	Ouput:			ptr        -  Comment info of HR
	Return: 		1 - success
*************************************************/
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

/************************************************* 
	Function:		reg_manager_interface_setCommentHr
	Description:	set Comment info of HR.
	Input:			num        -  Index of HR
	Input:			ptr        -  Comment info of HR
	Return: 		1 - success
*************************************************/
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
 ********* MI **********
 **********************/

/************************************************* 
	Function:		reg_manager_interface_getMI
	Description:	get MI.
	Input:			num        -  Index of MI
	Ouput:			ptr        -  MI Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getMI(MiData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		ptr->value = 0.0;
		bRet = g_objRegManagerInterface->getMi(num, &objMiDataIpc);
		FST_INFO("getMI: value = (%d) at %d with %s", 
			objMiDataIpc.value, num, bRet?"TRUE":"FALSE");
		ptr->value = objMiDataIpc.value;
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

/************************************************* 
	Function:		reg_manager_interface_setMI
	Description:	set MI.
	Input:			num        -  Index of MI
	Input:			ptr        -  MI register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setMI(MiData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		objMiDataIpc.id    = num;
		objMiDataIpc.value = ptr->value;
		bRet = g_objRegManagerInterface->setMi(&objMiDataIpc);
		FST_INFO("setR:(%f) at %d with %s", objMiDataIpc.value, num, bRet?"TRUE":"FALSE");
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addMi(objMiDataIpc);
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

/************************************************* 
	Function:		reg_manager_interface_delMI
	Description:	delete MI.
	Input:			num        -  Index of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_delMI(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deleteMi(num);
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

/************************************************* 
	Function:		reg_manager_interface_getValueMI
	Description:	get number info of MI.
	Input:			num        -  Index of MI
	Ouput:			ptr        -  number info of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getValueMI(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		bRet = g_objRegManagerInterface->getMi(num, &objMiDataIpc);
		if(bRet)
		{
		   *ptr = objMiDataIpc.value;
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

/************************************************* 
	Function:		reg_manager_interface_setValueMI
	Description:	set number info of MI.
	Input:			num        -  Index of MI
	Input:			ptr        -  number info of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setValueMI(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		objMiDataIpc.id    = num;
		objMiDataIpc.value = *ptr;
		bRet = g_objRegManagerInterface->setMi(&objMiDataIpc);
		FST_INFO("setValueMI:(%f) at %d with %s", 
			objMiDataIpc.value, num, bRet?"TRUE":"FALSE");
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

/************************************************* 
	Function:		reg_manager_interface_getIdMI
	Description:	get Id info of MI.
	Input:			num        -  Index of MI
	Ouput:			ptr        -  Id info of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getIdMI(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		bRet = g_objRegManagerInterface->getMi(num, &objMiDataIpc);
		if(bRet)
		{
		   *ptr = objMiDataIpc.id;
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

/************************************************* 
	Function:		reg_manager_interface_setIdMI
	Description:	set Id info of MI.
	Input:			num        -  Index of MI
	Input:			ptr        -  Id info of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setIdMI(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
	    objMiDataIpc.id = *ptr;
		bRet = g_objRegManagerInterface->setMi(&objMiDataIpc);
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

/************************************************* 
	Function:		reg_manager_interface_getCommentMI
	Description:	get Comment info of MI.
	Input:			num        -  Index of MI
	Ouput:			ptr        -  Comment info of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getCommentMI(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		bRet = g_objRegManagerInterface->getMi(num, &objMiDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objMiDataIpc.comment.c_str(), objMiDataIpc.comment.length());
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

/************************************************* 
	Function:		reg_manager_interface_setCommentMI
	Description:	set Comment info of MI.
	Input:			num        -  Index of MI
	Input:			ptr        -  Comment info of MI
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setCommentMI(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		bRet = g_objRegManagerInterface->getMi(num, &objMiDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objMiDataIpc.comment = string(ptr);
			reg_manager_interface_setMI(&objMiDataIpc, num);
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
 ********* MH **********
 **********************/

/************************************************* 
	Function:		reg_manager_interface_getMH
	Description:	get MH.
	Input:			num        -  Index of MH
	Ouput:			ptr        -  MH Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getMH(MhData *ptr, uint16_t num)
{
	bool bRet = false ;
	ptr->id    = num ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		ptr->value = 0.0;
	    FST_INFO("getMh at TXT_MH = %d", num);
		bRet = g_objRegManagerInterface->getMh(num, &objMhDataIpc);
		FST_INFO("getMH: value = (%d) at %d with %s", 
			objMhDataIpc.value, num, bRet?"TRUE":"FALSE");
		ptr->value = objMhDataIpc.value;
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

/************************************************* 
	Function:		reg_manager_interface_setMH
	Description:	set MH.
	Input:			num        -  Index of MH
	Input:			ptr        -  MH register Data
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setMH(MhData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		objMhDataIpc.id    = num;
		objMhDataIpc.value = ptr->value;
		bRet = g_objRegManagerInterface->setMh(&objMhDataIpc);
		FST_INFO("setR:(%f) at %d with %s", objMhDataIpc.value, num, bRet?"TRUE":"FALSE");
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addMh(objMhDataIpc);
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

/************************************************* 
	Function:		reg_manager_interface_delMH
	Description:	delete MH.
	Input:			num        -  Index of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_delMH(uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		bRet = g_objRegManagerInterface->deleteMh(num);
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

/************************************************* 
	Function:		reg_manager_interface_getValueMH
	Description:	get number info of MH.
	Input:			num        -  Index of MH
	Ouput:			ptr        -  number info of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getValueMH(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		bRet = g_objRegManagerInterface->getMh(num, &objMhDataIpc);
		if(bRet)
		{
		   *ptr = objMhDataIpc.value;
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

/************************************************* 
	Function:		reg_manager_interface_setValueMH
	Description:	set number info of MH.
	Input:			num        -  Index of MH
	Input:			ptr        -  number info of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setValueMH(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		objMhDataIpc.id    = num;
		objMhDataIpc.value = *ptr;
		bRet = g_objRegManagerInterface->setMh(&objMhDataIpc);
		FST_INFO("setValueMH:(%f) at %d with %s", 
			objMhDataIpc.value, num, bRet?"TRUE":"FALSE");
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

/************************************************* 
	Function:		reg_manager_interface_getIdMH
	Description:	get Id info of MH.
	Input:			num        -  Index of MH
	Ouput:			ptr        -  Id info of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getIdMH(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		bRet = g_objRegManagerInterface->getMh(num, &objMhDataIpc);
		if(bRet)
		{
		   *ptr = objMhDataIpc.id;
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

/************************************************* 
	Function:		reg_manager_interface_setIdMH
	Description:	set Id info of MH.
	Input:			num        -  Index of MH
	Input:			ptr        -  Id info of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setIdMH(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
	    objMhDataIpc.id = *ptr;
		bRet = g_objRegManagerInterface->setMh(&objMhDataIpc);
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

/************************************************* 
	Function:		reg_manager_interface_getCommentMH
	Description:	get Comment info of MH.
	Input:			num        -  Index of MH
	Ouput:			ptr        -  Comment info of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_getCommentMH(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		bRet = g_objRegManagerInterface->getMh(num, &objMhDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		   memcpy(ptr, objMhDataIpc.comment.c_str(), objMhDataIpc.comment.length());
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

/************************************************* 
	Function:		reg_manager_interface_setCommentMH
	Description:	set Comment info of MH.
	Input:			num        -  Index of MH
	Input:			ptr        -  Comment info of MH
	Return: 		1 - success
*************************************************/
bool reg_manager_interface_setCommentMH(char *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		bRet = g_objRegManagerInterface->getMh(num, &objMhDataIpc);
		if(bRet)
		{
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		    objMhDataIpc.comment = string(ptr);
			reg_manager_interface_setMH(&objMhDataIpc, num);
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

bool reg_manager_interface_getJoint(Joint &joint)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		bRet = g_objRegManagerInterface->getJoint(1, joint);
		if(bRet)
		{
			return bRet ;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	joint.j1 = joint.j2 = joint.j3 = joint.j4 = 
		joint.j5 = joint.j6 = joint.j7 = 
		joint.j8 = joint.j9 = 0;
	bRet = true ;
#endif
	return true ;
}

bool reg_manager_interface_getCart(PoseEuler &pos)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		bRet = g_objRegManagerInterface->getCart(1, pos);
		if(bRet)
		{
			return bRet ;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return true ;
}

bool reg_manager_interface_cartToJoint(PoseEuler pos, Joint &joint)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		bRet = g_objRegManagerInterface->cartToJoint(pos, joint);
		if(bRet)
		{
			return bRet ;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return true ;
}

bool reg_manager_interface_jointToCart(Joint joint, PoseEuler &pos)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		bRet = g_objRegManagerInterface->jointToCart(joint, pos);
		if(bRet)
		{
			return bRet ;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	bRet = true ;
#endif
	return true ;
}


