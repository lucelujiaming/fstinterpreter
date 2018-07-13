// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innercmd.h"
#include "reg_manager/reg_manager_interface.h"
#include "reg_manager/reg_manager_interface_wrapper.h"
#include "interpreter_common.h"
#ifndef WIN32
#include "motion_plan_frame_manager.h"
using namespace fst_reg ;
#endif


// Register name
#define TXT_PR    "pr"
#define TXT_SR    "sr"
#define TXT_R     "r"
#define TXT_MR    "mr"

#define TXT_UF    "uf"
#define TXT_TF    "tf"

#define TXT_PL    "pl"


#ifndef WIN32
RegManagerInterface * g_objRegManagerInterface = NULL;
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
	g_objRegManagerInterface = new RegManagerInterface("/root/install/share/configuration/machine");
#endif
}

bool reg_manager_interface_getPr(PrRegData *ptr, uint16_t num)
{
	bool bRet = false ;
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		bRet = g_objRegManagerInterface->getPrReg(num, ptr);
		PrRegData* data_ptr = ptr ;
		
		printf("getPr: id = %d, comment = %s\n", data_ptr->id, data_ptr->comment);
		printf("getPr: id = (%f, %f, %f, %f, %f, %f) \n", 
			data_ptr->value.joint_pos[0], data_ptr->value.joint_pos[1], 
			data_ptr->value.joint_pos[2], data_ptr->value.joint_pos[3], 
			data_ptr->value.joint_pos[4], data_ptr->value.joint_pos[5]);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		memcpy(&objPrRegData, ptr, sizeof(PrRegData));
		printf("setPr: id = %d, comment = %s\n", objPrRegData.id, objPrRegData.comment);
		printf("setPr: id = (%f, %f, %f, %f, %f, %f) \n", 
			objPrRegData.value.joint_pos[0], objPrRegData.value.joint_pos[1], 
			objPrRegData.value.joint_pos[2], objPrRegData.value.joint_pos[3], 
			objPrRegData.value.joint_pos[4], objPrRegData.value.joint_pos[5]);
		// objPrRegData.id = num ;
		
		bRet = g_objRegManagerInterface->setPrReg(&objPrRegData);
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addPrReg(&objPrRegData);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		bRet = g_objRegManagerInterface->deletePrReg(num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		   memcpy(ptr, &(objPrRegData.value.cartesian_pos), 
		   	    sizeof(objPrRegData.value.cartesian_pos));
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		    memcpy(&(objPrRegData.value.cartesian_pos), ptr, 

				sizeof(objPrRegData.value.cartesian_pos));
			reg_manager_interface_setPr(&objPrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		   memcpy(ptr, &(objPrRegData.value.joint_pos), 
		   	      sizeof(objPrRegData.value.joint_pos));
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		    memcpy(&(objPrRegData.value.joint_pos), ptr, 

				sizeof(objPrRegData.value.joint_pos));
			reg_manager_interface_setPr(&objPrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		   *ptr = objPrRegData.value.pos_type;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		    objPrRegData.value.pos_type = *ptr;

			reg_manager_interface_setPr(&objPrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		   *ptr = objPrRegData.id;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		    objPrRegData.id = *ptr;
			reg_manager_interface_setPr(&objPrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		   memcpy(ptr, objPrRegData.comment, sizeof(objPrRegData.comment));
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		PrRegData objPrRegData ;
		bRet = g_objRegManagerInterface->getPrReg(num, &objPrRegData);
		if(bRet)
		{
		    memcpy(objPrRegData.comment, ptr, 

				sizeof(objPrRegData.comment));
			reg_manager_interface_setPr(&objPrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		printf("getSr[%d]\n", num);
		bRet = g_objRegManagerInterface->getSrReg(num, ptr);
		printf("getSr[%d]:(%s)\n", num, ptr->value.c_str());
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
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
		
		SrRegData objSrRegData ;
		// memcpy(&objSrRegData, ptr, sizeof(objSrRegData));
		objSrRegData.id = num ;
		memcpy(objSrRegData.comment, ptr->comment, MAX_REG_COMMENT_LENGTH);
		objSrRegData.value = ptr->value ;
		printf("setSr:(%s)\n", objSrRegData.value.c_str());
		
		bRet = g_objRegManagerInterface->setSrReg(&objSrRegData);
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addSrReg(&objSrRegData);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		bRet = g_objRegManagerInterface->deleteSrReg(num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		SrRegData objSrRegData ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegData);
		if(bRet)
		{
		   strVal = objSrRegData.value;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		SrRegData objSrRegData ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegData);
		if(bRet)
		{
		    objSrRegData.value = strVal;
		}
		else    // Not exist
		{
		    objSrRegData.id    = num ;
            strcpy(objSrRegData.comment, "Empty");
		    objSrRegData.value = strVal;
		}
		printf("setValueSr:(%s)\n", objSrRegData.value.c_str());
		reg_manager_interface_setSr(&objSrRegData, num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		SrRegData objSrRegData ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegData);
		if(bRet)
		{
		   *ptr = objSrRegData.id;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		SrRegData objSrRegData ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegData);
		if(bRet)
		{
		    objSrRegData.id = *ptr;
			reg_manager_interface_setSr(&objSrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		SrRegData objSrRegData ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegData);
		if(bRet)
		{
		   memcpy(ptr, objSrRegData.comment, sizeof(objSrRegData.comment));
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		SrRegData objSrRegData ;
		bRet = g_objRegManagerInterface->getSrReg(num, &objSrRegData);
		if(bRet)
		{
		    memcpy(objSrRegData.comment, ptr, sizeof(objSrRegData.comment));
			reg_manager_interface_setSr(&objSrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		printf("reg_manager_interface_getR at %d \n", num);
		bRet = g_objRegManagerInterface->getRReg(num, ptr);
		
		RRegData* data_ptr = ptr ;
		
		printf("getR: id = %d, comment = %s\n", data_ptr->id, data_ptr->comment);
		printf("getR: value = (%f) \n", data_ptr->value);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
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
		
		RRegData * objRRegDataPtr = (RRegData *)ptr;
		// memcpy(&objRRegData, ptr, sizeof(objRRegData));
		
		printf("RRegData: id = %d, comment = %s\n", objRRegDataPtr->id, objRRegDataPtr->comment);
		printf("RRegData: id = (%f) \n", objRRegDataPtr->value);

		objRRegDataPtr->id = num ;
		
		bRet = g_objRegManagerInterface->setRReg(objRRegDataPtr);
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addRReg(objRRegDataPtr);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		bRet = g_objRegManagerInterface->deleteRReg(num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		RRegData objRRegData ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegData);
		if(bRet)
		{
		   *ptr = objRRegData.value;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		RRegData objRRegData ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegData);
		if(bRet)
		{
		    objRRegData.value = *ptr;
		}
		else    // Not exist
		{
		    objRRegData.id    = num ;
            strcpy(objRRegData.comment, "Empty");
		    objRRegData.value = *ptr;
		}
		reg_manager_interface_setR(&objRRegData, num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		RRegData objRRegData ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegData);
		if(bRet)
		{
		   *ptr = objRRegData.id;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		RRegData objRRegData ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegData);
		if(bRet)
		{
		    objRRegData.id = *ptr;
			reg_manager_interface_setR(&objRRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		RRegData objRRegData ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegData);
		if(bRet)
		{
		   memcpy(ptr, objRRegData.comment, sizeof(objRRegData.comment));
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		RRegData objRRegData ;
		bRet = g_objRegManagerInterface->getRReg(num, &objRRegData);
		if(bRet)
		{
		    memcpy(objRRegData.comment, ptr, sizeof(objRRegData.comment));
			reg_manager_interface_setR(&objRRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		bRet = g_objRegManagerInterface->getMrReg(num, ptr);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
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
		MrRegData objMrRegData ;
		memcpy(&objMrRegData, ptr, sizeof(objMrRegData));
		objMrRegData.id = num ;
		
		bRet = g_objRegManagerInterface->setMrReg(&objMrRegData);
		if(bRet == false)
		{
			bRet = g_objRegManagerInterface->addMrReg(&objMrRegData);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		bRet = g_objRegManagerInterface->deleteMrReg(num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		MrRegData objMrRegData ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegData);
		if(bRet)
		{
		   *ptr = objMrRegData.value;
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		MrRegData objMrRegData ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegData);
		if(bRet)  
		{
		    objMrRegData.value = *ptr;
		}
		else    // Not exist
		{
		    objMrRegData.id    = num ;
            strcpy(objMrRegData.comment, "Empty");
		    objMrRegData.value = *ptr;
		}
		reg_manager_interface_setMr(&objMrRegData, num);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		MrRegData objMrRegData ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegData);
		if(bRet)
		{
		   *ptr = objMrRegData.id;
		}
		else
		{
			printf("MrReg[%d] is NULL\n", num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		MrRegData objMrRegData ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegData);
		if(bRet)
		{
		    objMrRegData.id = *ptr;
			reg_manager_interface_setMr(&objMrRegData, num);
		}
		else
		{
			printf("MrReg[%d] is NULL\n", num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		MrRegData objMrRegData ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegData);
		if(bRet)
		{
		   memcpy(ptr, objMrRegData.comment, sizeof(objMrRegData.comment));
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
		MrRegData objMrRegData ;
		bRet = g_objRegManagerInterface->getMrReg(num, &objMrRegData);
		if(bRet)
		{
		    memcpy(objMrRegData.comment, ptr, sizeof(objMrRegData.comment));
			reg_manager_interface_setMr(&objMrRegData, num);
		}
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
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
	return 0 ;
}

bool reg_manager_interface_setUf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getCoordinateUf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setCoordinateUf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getIdUf(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setIdUf(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getCommentUf(char *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setCommentUf(char *ptr, uint16_t num)
{
	return 0 ;
}

/**********************
 ********* TF *********
 **********************/
bool reg_manager_interface_getTf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setTf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getCoordinateTf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setCoordinateTf(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getIdTf(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setIdTf(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getCommentTf(char *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setCommentTf(char *ptr, uint16_t num)
{
	return 0 ;
}

/**********************
 ********* PL *********
 **********************/
bool reg_manager_interface_getPl(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setPl(void *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getPosePl(PoseEuler* pose, int index)
{
	return 0 ;
}

bool reg_manager_interface_setPosePl(PoseEuler* pose, int index)
{
	return 0 ;
}

bool reg_manager_interface_getPalletPl(pl_t *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setPalletPl(pl_t *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getFlagPl(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setFlagPl(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getIdPl(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setIdPl(int *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_getCommentPl(char *ptr, uint16_t num)
{
	return 0 ;
}

bool reg_manager_interface_setCommentPl(char *ptr, uint16_t num)
{
	return 0 ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_pr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	bool bRet = false ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		vecRet = g_objRegManagerInterface->getPrRegValidIdList(0, 255);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
	bRet = true ;
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_sr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	bool bRet = false ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		vecRet = g_objRegManagerInterface->getSrRegValidIdList(0, 255);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
	bRet = true ;
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_r_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	bool bRet = false ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		vecRet = g_objRegManagerInterface->getRRegValidIdList(0, 255);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
	bRet = true ;
#endif
	return vecRet ;
}

std::vector<BaseRegData> reg_manager_interface_read_valid_mr_lst(int start_id, int size)
{
    std::vector<BaseRegData> vecRet ;
	bool bRet = false ;
	vecRet.clear();
#ifndef WIN32
	if(g_objRegManagerInterface)
	{
		vecRet = g_objRegManagerInterface->getMrRegValidIdList(0, 255);
	}
	else
	{
		printf("g_objRegManagerInterface is NULL\n");
	}
#else
	bRet = true ;
#endif
	return vecRet ;
}



