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
bool load_register_data()
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
	return true ;
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
		ptr->value = objPrRegDataIpc.value;
		
		FST_INFO("getPrReg: id = (%f, %f, %f, %f, %f, %f) at %d with %s ", 
			objPrRegDataIpc.value.pos[0], objPrRegDataIpc.value.pos[1], 
			objPrRegDataIpc.value.pos[2], objPrRegDataIpc.value.pos[3], 
			objPrRegDataIpc.value.pos[4], objPrRegDataIpc.value.pos[5], num, bRet?"TRUE":"FALSE");
		
		FST_INFO("getPrReg: id = (%f, %f, %f, %f, %f, %f) at %d with %s ", 
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
		objPrRegDataIpc.value = ptr->value;
		
		bRet = g_objRegManagerInterface->setPrReg(&objPrRegDataIpc);
		FST_INFO("setPr: id = %d (%f, %f, %f, %f, %f, %f) at %d with %s ", num, 
			objPrRegDataIpc.value.pos[0], objPrRegDataIpc.value.pos[1], 
			objPrRegDataIpc.value.pos[2], objPrRegDataIpc.value.pos[3], 
			objPrRegDataIpc.value.pos[4], objPrRegDataIpc.value.pos[5], num, bRet?"TRUE":"FALSE");
		
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
		   ptr->point_.x_    = objPrRegDataIpc.value.pos[0];
		   ptr->point_.y_    = objPrRegDataIpc.value.pos[1];
		   ptr->point_.z_    = objPrRegDataIpc.value.pos[2];
		   ptr->euler_.a_    = objPrRegDataIpc.value.pos[3];
		   ptr->euler_.b_    = objPrRegDataIpc.value.pos[4];
		   ptr->euler_.c_    = objPrRegDataIpc.value.pos[5];
#else
		   ptr->position.x    = objPrRegDataIpc.value.pos[0];
		   ptr->position.y    = objPrRegDataIpc.value.pos[1];
		   ptr->position.z    = objPrRegDataIpc.value.pos[2];
		   ptr->orientation.a = objPrRegDataIpc.value.pos[3];
		   ptr->orientation.b = objPrRegDataIpc.value.pos[4];
		   ptr->orientation.c = objPrRegDataIpc.value.pos[5];
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
        objPrRegDataIpc.value.pos_type     = PR_REG_POS_TYPE_CARTESIAN ;
#ifndef WIN32
		objPrRegDataIpc.value.pos[0] = ptr->point_.x_   ;
		objPrRegDataIpc.value.pos[1] = ptr->point_.y_   ;
		objPrRegDataIpc.value.pos[2] = ptr->point_.z_   ;
		objPrRegDataIpc.value.pos[3] = ptr->euler_.a_   ;
		objPrRegDataIpc.value.pos[4] = ptr->euler_.b_   ;
		objPrRegDataIpc.value.pos[5] = ptr->euler_.c_   ;
#else
		objPrRegDataIpc.value.pos[0] = ptr->position.x   ;
		objPrRegDataIpc.value.pos[1] = ptr->position.y   ;
		objPrRegDataIpc.value.pos[2] = ptr->position.z   ;
		objPrRegDataIpc.value.pos[3] = ptr->orientation.a;
		objPrRegDataIpc.value.pos[4] = ptr->orientation.b;
		objPrRegDataIpc.value.pos[5] = ptr->orientation.c;
#endif
		objPrRegDataIpc.value.pos[6] = 0.0;
		objPrRegDataIpc.value.pos[7] = 0.0;
		objPrRegDataIpc.value.pos[8] = 0.0;
		bRet = g_objRegManagerInterface->setPrReg(&objPrRegDataIpc);
		FST_INFO("setPr: id = %d (%f, %f, %f, %f, %f, %f) at %d with %s ", num, 
			objPrRegDataIpc.value.pos[0], objPrRegDataIpc.value.pos[1], 
			objPrRegDataIpc.value.pos[2], objPrRegDataIpc.value.pos[3], 
			objPrRegDataIpc.value.pos[4], objPrRegDataIpc.value.pos[5], num, bRet?"TRUE":"FALSE");
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
		   ptr->j1_ = objPrRegDataIpc.value.pos[0];
		   ptr->j2_ = objPrRegDataIpc.value.pos[1];
		   ptr->j3_ = objPrRegDataIpc.value.pos[2];
		   ptr->j4_ = objPrRegDataIpc.value.pos[3];
		   ptr->j5_ = objPrRegDataIpc.value.pos[4];
		   ptr->j6_ = objPrRegDataIpc.value.pos[5];   
#else
		   ptr->j1 = objPrRegDataIpc.value.pos[0];
		   ptr->j2 = objPrRegDataIpc.value.pos[1];
		   ptr->j3 = objPrRegDataIpc.value.pos[2];
		   ptr->j4 = objPrRegDataIpc.value.pos[3];
		   ptr->j5 = objPrRegDataIpc.value.pos[4];
		   ptr->j6 = objPrRegDataIpc.value.pos[5];   
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
            objPrRegDataIpc.value.pos_type     = PR_REG_POS_TYPE_JOINT;
#ifndef WIN32 
			objPrRegDataIpc.value.pos[0] = ptr->j1_;
			objPrRegDataIpc.value.pos[1] = ptr->j2_;
			objPrRegDataIpc.value.pos[2] = ptr->j3_;
			objPrRegDataIpc.value.pos[3] = ptr->j4_;
			objPrRegDataIpc.value.pos[4] = ptr->j5_;
			objPrRegDataIpc.value.pos[5] = ptr->j6_;
#else
			objPrRegDataIpc.value.pos[0] = ptr->j1;
			objPrRegDataIpc.value.pos[1] = ptr->j2;
			objPrRegDataIpc.value.pos[2] = ptr->j3;
			objPrRegDataIpc.value.pos[3] = ptr->j4;
			objPrRegDataIpc.value.pos[4] = ptr->j5;
			objPrRegDataIpc.value.pos[5] = ptr->j6; 
#endif
			objPrRegDataIpc.value.pos[6] = 0.0;
			objPrRegDataIpc.value.pos[7] = 0.0;
			objPrRegDataIpc.value.pos[8] = 0.0;
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
bool reg_manager_interface_getSr(string &ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		ptr = "";
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegDataIpc);
		ptr = string(objSrRegDataIpc.value) ;
		FST_INFO("getSr[%d]:(%s) at %d with %s", num, ptr.c_str(), num, bRet?"TRUE":"FALSE");
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
    ptr = string("Test");
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
bool reg_manager_interface_setSr(string &ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		SrRegDataIpc objSrRegDataIpc ;
		// memcpy(&objSrRegData, ptr, sizeof(objSrRegData));
		objSrRegDataIpc.id = num ;
		strcpy(objSrRegDataIpc.value, ptr.c_str()) ;
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
bool reg_manager_interface_getR(double *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		*ptr = 0.0;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegDataIpc);
		FST_INFO("getR: value = (%f) at %d with %s", 
			objRRegDataIpc.value, num, bRet?"TRUE":"FALSE");
		*ptr = objRRegDataIpc.value;
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	*ptr = 1.0 ;
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
bool reg_manager_interface_setR(double *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		RRegDataIpc objRRegDataIpc ;
		objRRegDataIpc.id    = num;
		objRRegDataIpc.value = *ptr;
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
bool reg_manager_interface_getMr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		*ptr = 0.0;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegDataIpc);
		FST_INFO("getMR: value = (%d) at %d with %s", 
			objMrRegDataIpc.value, num, bRet?"TRUE":"FALSE");
		*ptr = objMrRegDataIpc.value ;
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	*ptr = 1 ;
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
bool reg_manager_interface_setMr(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		MrRegDataIpc objMrRegDataIpc ;
		objMrRegDataIpc.id = num ;
		objMrRegDataIpc.value = *ptr ;
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
		FST_INFO("setValueMr:(%d) at %d with %s", objMrRegDataIpc.value, num, bRet?"TRUE":"FALSE");
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
bool reg_manager_interface_getMI(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		bRet = g_objRegManagerInterface->getMi(num, &objMiDataIpc);
		FST_INFO("getMI: value = (%d) at %d with %s", 
			objMiDataIpc.value, num, bRet?"TRUE":"FALSE");
		if(bRet)
		{
		   *ptr = objMiDataIpc.value;
		}
		else 
		{
			bRet = true ;
		   *ptr = 0.0;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	*ptr = 1 ;
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
bool reg_manager_interface_setMI(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MiDataIpc objMiDataIpc ;
		objMiDataIpc.id    = num;
		objMiDataIpc.value = *ptr;
		bRet = g_objRegManagerInterface->setMi(&objMiDataIpc);
		FST_INFO("setR:(%f) at %d with %s", objMiDataIpc.value, num, bRet?"TRUE":"FALSE");
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addMi(objMiDataIpc);
		}
#endif
		if(!bRet)
		{
			bRet = true ;
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
		else 
		{
			bRet = true ;
		   *ptr = 0.0;
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
		if(!bRet)
		{
			bRet = true ;
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
bool reg_manager_interface_getMH(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
	    FST_INFO("getMh at TXT_MH = %d", num);
		bRet = g_objRegManagerInterface->getMh(num, &objMhDataIpc);
		FST_INFO("getMH: value = (%d) at %d with %s", 
			objMhDataIpc.value, num, bRet?"TRUE":"FALSE");
		if(bRet)
		{
		   *ptr = objMhDataIpc.value;
		}
		else 
		{
			bRet = true ;
		   *ptr = 0.0;
		}
	}
	else
	{
		FST_ERROR("g_objRegManagerInterface is NULL");
	}
#else
	*ptr = 1 ;
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
bool reg_manager_interface_setMH(int *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		fst_base::MhDataIpc objMhDataIpc ;
		objMhDataIpc.id    = num;
		objMhDataIpc.value = *ptr;
		bRet = g_objRegManagerInterface->setMh(&objMhDataIpc);
		FST_INFO("setR:(%f) at %d with %s", objMhDataIpc.value, num, bRet?"TRUE":"FALSE");
#ifdef USE_LOCAL_REG_MANAGER_INTERFACE
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addMh(objMhDataIpc);
		}
#endif
		if(!bRet)
		{
			bRet = true ;
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
		else 
		{
			bRet = true ;
		   *ptr = 0.0;
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
		if(!bRet)
		{
			bRet = true ;
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

bool reg_manager_interface_getUserOpMode(int& mode)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		bRet = g_objRegManagerInterface->getUserOpMode(mode);
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


