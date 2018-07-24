// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innercmd.h"
#ifndef WIN32
#include "motion_plan_frame_manager.h"
#include "reg_manager_interface.h"
#endif

#include "interpreter_common.h"
#ifdef WIN32
#include "reg_manager/reg_manager_interface_wrapper.h"
#else
#include "reg_manager_interface_wrapper.h"
using namespace fst_reg ;
#endif

// Register name
#define TXT_PR    "pr"
#define TXT_SR    "sr"
#define TXT_R     "r"
#define TXT_MR    "mr"
#define TXT_HR    "hr"

#define TXT_UF    "uf"
#define TXT_TF    "tf"

#define TXT_PL    "pl"

// member name of Register
#define TXT_REG_TYPE    "type"
#define TXT_REG_ID      "id"
#define TXT_REG_COMMENT "comment"
#define TXT_REG_VALUE   "value"

// member name of PR
#define TXT_POSE    "pose"
#define TXT_POSE_X  "x"
#define TXT_POSE_Y  "y"
#define TXT_POSE_Z  "z"
#define TXT_POSE_A  "a"
#define TXT_POSE_B  "b"
#define TXT_POSE_C  "c"

#define TXT_JOINT      "joint"
#define TXT_JOINT_J1   "j1"
#define TXT_JOINT_J2   "j2"
#define TXT_JOINT_J3   "j3"
#define TXT_JOINT_J4   "j4"
#define TXT_JOINT_J5   "j5"
#define TXT_JOINT_J6   "j6"
#define TXT_JOINT_J7   "j7"
#define TXT_JOINT_J8   "j8"
#define TXT_JOINT_J9   "j9"

#define TXT_PR_POSE_JOINT_J1   "pj1"
#define TXT_PR_POSE_JOINT_J2   "pj2"
#define TXT_PR_POSE_JOINT_J3   "pj3"
#define TXT_PR_POSE_JOINT_J4   "pj4"
#define TXT_PR_POSE_JOINT_J5   "pj5"
#define TXT_PR_POSE_JOINT_J6   "pj6"

#define TXT_HR_JOINT_J1   "hj1"
#define TXT_HR_JOINT_J2   "hj2"
#define TXT_HR_JOINT_J3   "hj3"
#define TXT_HR_JOINT_J4   "hj4"
#define TXT_HR_JOINT_J5   "hj5"
#define TXT_HR_JOINT_J6   "hj6"

// member name of UF/TF
#define TXT_UF_TF_COORDINATE    "coordinate"
// member name of PL
#define TXT_PL_POSE       "pose"
#define TXT_PL_PALLET     "pallet"
#define TXT_PL_FLAG       "flag"


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

#ifdef WIN32
#define USE_FAKE_DATA 
#endif
int forgesight_registers_manager_get_register(
			struct thread_control_block* objThreadCntrolBlock, 
							char *name, eval_value * value)
{	
	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	// char reg_content_buffer[512] ;
	
	PrRegData objPrRegData ;
	SrRegData objSrRegData ;
	MrRegData objMrRegData ;
	RRegData objRRegData ;
	HrRegData objHrRegData ;

    int       iID ;
    char      cComment[MAX_REG_COMMENT_LENGTH];
    std::string    strComment;

	PoseEuler objPoseEuler ;
    Joint     objJoint;
    int       iType ;
    
    std::string    strSrValue;
    double    dRValue;
    int       iMrValue;
	
    int       iPlFlag ;
	pl_t      pltValue ;
	
	memset(reg_name, 0x00, 16);
	temp = reg_name ;
	get_char_token(namePtr, temp);
	
	namePtr += strlen(reg_name) ;
	if(namePtr[0] != '['){
		return -1 ;
	}
	namePtr++ ;
	
	memset(reg_idx, 0x00, 16);
	temp = reg_idx ;
	get_num_token(namePtr, temp);
	iRegIdx = atoi(reg_idx);
	// namePtr += strlen(reg_idx) ;

	namePtr += strlen(reg_idx) ;
	if(namePtr[0] != ']'){
		return -1 ;
	}
	namePtr++ ;

	namePtr = strchr(namePtr, '.');
	memset(reg_member, 0x00, 16);
	if(namePtr)
	{
		namePtr++ ;
		temp = reg_member ;
		get_char_token(namePtr, temp);
	}
	// memset(reg_content_buffer, 0x00, sizeof(reg_content_buffer));
 
	printf("forgesight_registers_manager_get_register at %s \n", reg_name);

	if(!strcmp(reg_name, TXT_PR))
	{
		if(strlen(reg_member) == 0)
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			// PrRegData * ptr = (PrRegData *)reg_content_buffer ;
			value->setPrRegDataValue(&objPrRegData);
		}
		// Implement for intergretion
		else if (!strcmp(reg_member, TXT_POSE))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setPoseValue(&objPoseEuler);
		}
		else if (!strcmp(reg_member, TXT_POSE_X))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setFloatValue(objPoseEuler.position.x);
		}
		else if (!strcmp(reg_member, TXT_POSE_Y))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setFloatValue(objPoseEuler.position.y);
		}
		else if (!strcmp(reg_member, TXT_POSE_Z))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setFloatValue(objPoseEuler.position.z);
		}
		else if (!strcmp(reg_member, TXT_POSE_A))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setFloatValue(objPoseEuler.orientation.a);
		}
		else if (!strcmp(reg_member, TXT_POSE_B))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setFloatValue(objPoseEuler.orientation.b);
		}
		else if (!strcmp(reg_member, TXT_POSE_C))
		{
			reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setFloatValue(objPoseEuler.orientation.c);
		}
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setJointValue(&objJoint);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j1);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j2);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j3);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j4);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j5);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j6);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J7))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j7);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J8))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j8);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J9))
		{
			reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j9);
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J1))
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.cartesian_pos.position.x);
			else if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.joint_pos[0]);
			else
	       		printf("PR[%d].pos_type = %d \n", 
	       			iRegIdx, objPrRegData.value.pos_type);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.cartesian_pos.position.y);
			else if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.joint_pos[1]);
			else
	       		printf("PR[%d].pos_type = %d \n", 
	       			iRegIdx, objPrRegData.value.pos_type);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.cartesian_pos.position.z);
			else if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.joint_pos[2]);
			else
	       		printf("PR[%d].pos_type = %d \n", 
	       			iRegIdx, objPrRegData.value.pos_type);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.cartesian_pos.orientation.a);
			else if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.joint_pos[3]);
			else
	       		printf("PR[%d].pos_type = %d \n", 
	       			iRegIdx, objPrRegData.value.pos_type);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.cartesian_pos.orientation.b);
			else if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.joint_pos[4]);
			else
	       		printf("PR[%d].pos_type = %d \n", 
	       			iRegIdx, objPrRegData.value.pos_type);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.cartesian_pos.orientation.c);
			else if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				value->setFloatValue(objPrRegData.value.joint_pos[5]);
			else
	       		printf("PR[%d].pos_type = %d \n", 
	       			iRegIdx, objPrRegData.value.pos_type);
		}
		else if (!strcmp(reg_member, TXT_REG_TYPE))
		{
			reg_manager_interface_getTypePr(&iType, iRegIdx);
			value->setFloatValue(iType);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdPr(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentPr(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_SR))
	{
		if(strlen(reg_member) == 0)
		{
			printf("TXT_SR at (%d), (%s), (%s) \n", 
				objSrRegData.id, objSrRegData.comment, objSrRegData.value.c_str());
			reg_manager_interface_getSr(&objSrRegData, iRegIdx);
			// SrRegData * ptr = (SrRegData *)reg_content_buffer ;
			// value->setStringValue(ptr->value);
			value->setSrRegDataValue(&objSrRegData);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			reg_manager_interface_getValueSr(strSrValue, iRegIdx);
			value->setStringValue(strSrValue);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdSr(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentSr(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_R))
	{
	    printf("forgesight_registers_manager_get_register at TXT_R \n");
		if(strlen(reg_member) == 0)
		{
	        printf("reg_manager_interface_getR at TXT_R \n");
            // Use TXT_REG_VALUE
			reg_manager_interface_getR(&objRRegData, iRegIdx);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setFloatValue(ptr->value);
			value->setRRegDataValue(&objRRegData);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			reg_manager_interface_getValueR(&dRValue, iRegIdx);
			value->setFloatValue(dRValue);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdR(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentR(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_MR))
	{
		if(strlen(reg_member) == 0)
		{
			reg_manager_interface_getMr(&objMrRegData, iRegIdx);
			// MrRegData * ptr = (MrRegData *)reg_content_buffer ;
	    	printf("Get at TXT_MR with %d (%s) %d \n", 
	    			objMrRegData.id, objMrRegData.comment, objMrRegData.value);
			// value->setFloatValue(ptr->value);
			value->setMrRegDataValue(&objMrRegData);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			reg_manager_interface_getValueMr(&iMrValue, iRegIdx);
			value->setFloatValue(iMrValue);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdMr(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentMr(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_HR))
	{
		if(strlen(reg_member) == 0)
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			// PrRegData * ptr = (PrRegData *)reg_content_buffer ;
			value->setHrRegDataValue(&objHrRegData);
		}
		// Implement for intergretion
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setJointValue(&objJoint);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j1);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j2);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j3);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j4);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j5);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j6);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J7))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j7);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J8))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j8);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J9))
		{
			reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			value->setFloatValue(objJoint.j9);
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_HR_JOINT_J1))
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			value->setFloatValue(objHrRegData.value.joint_pos[0]);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			value->setFloatValue(objHrRegData.value.joint_pos[1]);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			value->setFloatValue(objHrRegData.value.joint_pos[2]);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			value->setFloatValue(objHrRegData.value.joint_pos[3]);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			value->setFloatValue(objHrRegData.value.joint_pos[4]);
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			value->setFloatValue(objHrRegData.value.joint_pos[5]);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdPr(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentPr(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_UF))
	{
//		if(strlen(reg_member) == 0)
//		{
//            // Use TXT_UF_TF_COORDINATE
//			reg_manager_interface_getUf(reg_content_buffer, iRegIdx);
//			uf_shmi_t * ptr = (uf_shmi_t *)reg_content_buffer ;
//			value->setPoseValue(&(ptr->c));
//		}
//		else 
		if (!strcmp(reg_member, TXT_UF_TF_COORDINATE))
		{
			reg_manager_interface_getCoordinateUf(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setPoseValue(&objPoseEuler);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdUf(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentUf(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_TF))
	{
//		if(strlen(reg_member) == 0)
//		{
//			reg_manager_interface_getTf(reg_content_buffer, iRegIdx);
//			tf_shmi_t * ptr = (tf_shmi_t *)reg_content_buffer ;
//			value->setPoseValue(&(ptr->c));
//		}
//		else 
		if (!strcmp(reg_member, TXT_UF_TF_COORDINATE))
		{
			reg_manager_interface_getCoordinateTf(&objPoseEuler, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setPoseValue(&objPoseEuler);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdTf(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentTf(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	else if(!strcmp(reg_name, TXT_PL))
	{
//		if(strlen(reg_member) == 0)
//		{
//            // Use TXT_PL_POSE
//			reg_manager_interface_getPl(reg_content_buffer, iRegIdx);
//			pl_shmi_t * ptr = (pl_shmi_t *)reg_content_buffer ;
//			value->setPLValue(&(ptr->pallet));
//		}
//		else 
		if (!strcmp(reg_member, TXT_PL_POSE))
		{
			reg_manager_interface_getPalletPl(&pltValue, iRegIdx);
			// PoseEuler * ptr = (PoseEuler *)reg_content_buffer ;
			value->setPoseValue(&objPoseEuler);
		}
//		else if (!strcmp(reg_member, TXT_PL_PALLET))
//		{
//			reg_manager_interface_getPalletPl(reg_content_buffer, iRegIdx);
//			pl_t * ptr = (pl_t *)reg_content_buffer ;
//			value->setPLValue(ptr);
//		}
		else if (!strcmp(reg_member, TXT_PL_FLAG))
		{
			reg_manager_interface_getFlagPl(&iPlFlag, iRegIdx);
			value->setFloatValue(iPlFlag);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			reg_manager_interface_getIdPl(&iID, iRegIdx);
			value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			reg_manager_interface_getCommentPl(cComment, iRegIdx);
			strComment = std::string(cComment);
			value->setStringValue(strComment);
		}
	}
	return 0 ;
}

int forgesight_registers_manager_set_register(
		struct thread_control_block* objThreadCntrolBlock, 
		char *name, eval_value * valueStart)
{
	eval_value value;
	int boolValue;

	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	PrRegData objPrRegData ;
	HrRegData objHrRegData ;
	
	PoseEuler pose ;
	Joint joint ;
	pl_t pltValue ;
	
	std::string strValue;
	
	memset(reg_name, 0x00, 16);
	temp = reg_name ;
	get_char_token(namePtr, temp);
	if(name[strlen(reg_name)] != '['){
		return -1 ;
	}
	namePtr += strlen(reg_name) ;
	namePtr++ ;
	
	memset(reg_idx, 0x00, 16);
	temp = reg_idx ;
	get_num_token(namePtr, temp);
	iRegIdx = atoi(reg_idx);
	// namePtr += strlen(reg_idx) ;
	
	namePtr = strchr(namePtr, '.');
	memset(reg_member, 0x00, 16);
	if(namePtr)
	{
		namePtr++ ;
		temp = reg_member ;
		get_char_token(namePtr, temp);
	}

	if(!strcmp(reg_name, TXT_PR))
	{
		if(strlen(reg_member) == 0)
		{
			reg_manager_interface_setPr(&(valueStart->getPrRegDataValue()), iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
				pose.position.x = (double)valueStart->getFloatValue();

				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.y = (double)value.getFloatValue();

				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.z = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.a = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.b = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.c = (double)value.getFloatValue();

				printf("Set POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setPosePr(&pose, iRegIdx);
	 	      	return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				printf("Set POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setPosePr(&pose, iRegIdx);
	    	   	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_POSE_X))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.position.x = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set X:(%f) but PR[%d] is not CARTESIAN\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set X:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_Y))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.position.y = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set Y:(%f) but PR[%d] is not CARTESIAN\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set Y:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_Z))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.position.z = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set Z:(%f) but PR[%d] is not CARTESIAN\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set Z:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_A))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.orientation.a = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set A:(%f) but PR[%d] is not CARTESIAN\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set A:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_B))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.orientation.b = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set B:(%f) but PR[%d] is not CARTESIAN\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set B:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_C))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.orientation.c = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set C:(%f) but PR[%d] is not CARTESIAN\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set C:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
				joint.j1 = (double)valueStart->getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j2 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j3 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j4 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j5 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j6 = (double)value.getFloatValue();
				
				printf("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
					reg_idx);
				reg_manager_interface_setJointPr(&joint, iRegIdx);
	    	   	return 0 ;
			}
			else if (valueStart->getType() == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				printf("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
					reg_idx);
				reg_manager_interface_setJointPr(&joint, iRegIdx);
	    	  	return 0 ;
			}
			
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[0] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set J1:(%f) but PR[%d] is not POS_TYPE_JOINT\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set J1:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[1] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set J2:(%f) but PR[%d] is not POS_TYPE_JOINT\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set J2:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[2] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set J3:(%f) but PR[%d] is not POS_TYPE_JOINT\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set J3:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[3] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set J4:(%f) but PR[%d] is not POS_TYPE_JOINT\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set J4:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[4] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set J5:(%f) but PR[%d] is not POS_TYPE_JOINT\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set J5:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[5] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					printf("Set J6:(%f) but PR[%d] is not POS_TYPE_JOINT\n", 
						fValue, iRegIdx);
				}
			}
			else
			{
				printf("Set J6:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J1))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.position.x = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[0] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		printf("Set PJ1:(%f) PR[%d].pos_type = %d \n", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				printf("Set PJ1:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.position.y = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[1] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		printf("Set PJ2:(%f) PR[%d].pos_type = %d \n", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				printf("Set PJ2:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.position.z = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[2] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		printf("Set PJ3:(%f) PR[%d].pos_type = %d \n", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				printf("Set PJ3:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.orientation.a = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[3] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		printf("Set PJ4:(%f) PR[%d].pos_type = %d \n", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				printf("Set PJ4:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.orientation.b = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[4] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		printf("Set PJ5:(%f) PR[%d].pos_type = %d \n", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				printf("Set PJ5:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
					objPrRegData.value.cartesian_pos.orientation.c = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				if(POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
					objPrRegData.value.joint_pos[5] = fValue ;
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		printf("Set PJ6:(%f) PR[%d].pos_type = %d \n", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				printf("Set PJ6:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_TYPE))
		{
			int iType = (int)valueStart->getFloatValue();
			printf("Set TYPE:(%d) to PR[%s]\n", iType, reg_idx);
			reg_manager_interface_setTypePr(&iType, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			printf("Set ID:(%d) to PR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdPr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to PR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentPr(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_SR))
	{
		if(strlen(reg_member) == 0)
		{
			strValue = valueStart->getStringValue() ;
			if(valueStart->getType() == TYPE_STRING)
			{
				printf("Set TYPE_STRING token:(%s) to %s\n", 
						objThreadCntrolBlock->token, strValue.c_str());
			   reg_manager_interface_setValueSr(strValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				printf("Set TYPE_SR token:(%s) to %s\n", 
						objThreadCntrolBlock->token, strValue.c_str());
				reg_manager_interface_setSr(&(valueStart->getSrRegDataValue()), iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_FLOAT | TYPE_MR))
			{
			    char cStringValue[64];
			    sprintf(cStringValue, "%d", (int)valueStart->getFloatValue());
				strValue = std::string(cStringValue);
				printf("Set TYPE_STRING token:(%s) to %s\n", 
						objThreadCntrolBlock->token, cStringValue);
			    reg_manager_interface_setValueSr(strValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_FLOAT | TYPE_R))
			{
			    char cStringValue[64];
			    sprintf(cStringValue, "%f", valueStart->getFloatValue());
				strValue = std::string(cStringValue);
				printf("Set TYPE_STRING token:(%s) to %s\n", 
						objThreadCntrolBlock->token, cStringValue);
			    reg_manager_interface_setValueSr(strValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			get_token(objThreadCntrolBlock);
			printf("Set VALUE:(%s) to SR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			strValue = std::string(objThreadCntrolBlock->token);
			reg_manager_interface_setValueSr(strValue, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			printf("Set ID:(%d) to SR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdSr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to SR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentSr(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_R))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->getType() == TYPE_FLOAT)
			{    
				double fValue = valueStart->getFloatValue();
			    reg_manager_interface_setValueR(&fValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_FLOAT | TYPE_MR))
			{    
				double dValue = valueStart->getFloatValue();
			    reg_manager_interface_setValueR(&dValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_FLOAT | TYPE_R))
			{    
				reg_manager_interface_setR(&(valueStart->getRRegDataValue()), iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				double fValue = atof(strValue.c_str());
			    reg_manager_interface_setValueR(&fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			double fValue = valueStart->getFloatValue();
			printf("Set VALUE:(%f) to SR[%s]\n", fValue, reg_idx);
			reg_manager_interface_setValueR(&fValue, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			printf("Set ID:(%d) to SR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdR(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to SR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentR(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_MR))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->getType() == TYPE_FLOAT)
			{    
				int iValue = (int)valueStart->getFloatValue();
			    reg_manager_interface_setValueMr(&iValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_FLOAT | TYPE_R))
			{    
				int iValue = (int)valueStart->getFloatValue();
			    reg_manager_interface_setValueMr(&iValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_FLOAT | TYPE_MR))
			{    
				reg_manager_interface_setMr(&(valueStart->getMrRegDataValue()), iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				printf("Set TYPE_SR token:(%s) to %s\n", 
						objThreadCntrolBlock->token, strValue.c_str());
				int iValue = (int)atof(strValue.c_str());
			    reg_manager_interface_setValueMr(&iValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getFloatValue();
			printf("Set VALUE:(%f) to MR[%s]\n", iValue, reg_idx);
			reg_manager_interface_setValueMr(&iValue, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			printf("Set ID:(%d) to MR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdMr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to MR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentMr(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_HR))
	{
		if(strlen(reg_member) == 0)
		{
			reg_manager_interface_setHr(&(valueStart->getHrRegDataValue()), iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
				joint.j1 = (double)valueStart->getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j2 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j3 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j4 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j5 = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				joint.j6 = (double)value.getFloatValue();
				
				printf("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
					reg_idx);
				reg_manager_interface_setJointHr(&joint, iRegIdx);
	    	   	return 0 ;
			}
			else if (valueStart->getType() == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				printf("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
					reg_idx);
				reg_manager_interface_setJointHr(&joint, iRegIdx);
	    	  	return 0 ;
			}
			
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[0] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set J1:(%f) but PR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[1] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set J2:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[2] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set J3:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[3] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set J4:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[4] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set J5:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[5] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set J6:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_HR_JOINT_J1))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[0] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set HJ1:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J2))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[1] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set HJ2:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J3))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[2] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set HJ3:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J4))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[3] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set HJ4:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J5))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[4] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set HJ5:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J6))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getHr(&objHrRegData, iRegIdx))
			{
				objHrRegData.value.joint_pos[5] = fValue ;
				reg_manager_interface_setHr(&objHrRegData, iRegIdx);
			}
			else
			{
				printf("Set HJ6:(%f) but HR[%d] not exist\n", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			printf("Set ID:(%d) to HR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdHr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to HR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentHr(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_UF))
	{
		if(strlen(reg_member) == 0)
		{
			// reg_manager_interface_setUf(valueStart->getUFRegValue(), iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_UF_TF_COORDINATE))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
				// pose.position.x = (double)(atof((char *)valueStart));
				pose.position.x = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.y = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.z = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.a = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.b = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.c = (double)value.getFloatValue();
				
				printf("Set COORDINATE:(%f, %f, %f, %f, %f, %f) to UF[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setCoordinateUf(&pose, iRegIdx);
	       		return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				printf("Set POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setCoordinateUf(&pose, iRegIdx);
	        	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)value.getFloatValue();
			printf("Set ID:(%d) to MR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdUf(&iID, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to MR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentUf(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	        return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_TF))
	{
		if(strlen(reg_member) == 0)
		{
			// reg_manager_interface_setTf(valueStart->getTFRegValue(), iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_UF_TF_COORDINATE))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
				// pose.position.x = (double)(atof((char *)valueStart));
				pose.position.x = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.y = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.z = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.a = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.b = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.c = (double)value.getFloatValue();
				
				printf("Set COORDINATE:(%f, %f, %f, %f, %f, %f) to TF[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setCoordinateTf(&pose, iRegIdx);
	        	return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				printf("Set POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setCoordinateTf(&pose, iRegIdx);
	        	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)value.getFloatValue();
			printf("Set ID:(%d) to MR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdTf(&iID, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to MR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentTf(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	        return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_PL))
	{
		if(strlen(reg_member) == 0)
		{
			// reg_manager_interface_setPl(valueStart->getPLRegValue(), iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PL_POSE))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
				// pose.position.x = (double)(atof((char *)valueStart));
				pose.position.x = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.y = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.position.z = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.a = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.b = (double)value.getFloatValue();
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
				pose.orientation.c = (double)value.getFloatValue();
				
				printf("Set COORDINATE:(%f, %f, %f, %f, %f, %f) to TF[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setPosePl(&pose, iRegIdx);
	        	return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				printf("Set POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]\n", 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
					reg_idx);
				reg_manager_interface_setPosePl(&pose, iRegIdx);
	        	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_PL_PALLET))
		{
			pltValue = valueStart->getPLValue();
			reg_manager_interface_setPalletPl(&pltValue, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PL_FLAG))
		{
			int iFlagPl = (int)value.getFloatValue();
			printf("Set ID:(%d) to MR[%s]\n", iFlagPl, reg_idx);
			reg_manager_interface_setFlagPl(&iFlagPl, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)value.getFloatValue();
			printf("Set ID:(%d) to MR[%s]\n", iID, reg_idx);
			reg_manager_interface_setIdPl(&iID, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			printf("Set COMMENT:(%s) to MR[%s]\n", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentPl(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	        return 0 ;
		}
	}
	return 0 ;
}

int forgesight_read_reg(RegMap & reg)
{

	PrRegData objPrRegData ;
	PrRegData * objPrRegDataPtr ;
	
	SrRegData objSrRegData ;
	// SrRegData * objSrRegDataPtr ;
	
	MrRegData objMrRegData ;
	MrRegData * objMrRegDataPtr ;
	
	RRegData objRRegData ;
	RRegData * objRRegDataPtr ;

	HrRegData objHrRegData ;
	HrRegData * objHrRegDataPtr ;

    int       iID ;
 //   char      cComment[MAX_REG_COMMENT_LENGTH];
    std::string    strComment;

	PoseEuler objPoseEuler ;
    Joint     objJoint;
    int       iType ;
    
    std::string    strSrValue;
    double    dRValue;
    int       iMrValue;
	
    int       iPlFlag ;
	pl_t      pltValue ;
	
	 switch(reg.type)
	 {
	 // pose register
	 case POSE_REG:
		 reg_manager_interface_getPr(&objPrRegData, reg.index);
		 objPrRegDataPtr = (PrRegData *)reg.value;
		 objPrRegDataPtr->id = objPrRegData.id;
		 memcpy(objPrRegDataPtr->comment, 
		 		objPrRegData.comment, sizeof(objPrRegData.comment));
 		 objPrRegDataPtr->value = objPrRegData.value;
		 break;
	 case POSE_REG_POSE:
		 reg_manager_interface_getPosePr(&objPoseEuler, reg.index);
		 memcpy(reg.value, &objPoseEuler, sizeof(objPoseEuler));
		 break;
	 case POSE_REG_JOINT:
		 reg_manager_interface_getJointPr(&objJoint, reg.index);
		 memcpy(reg.value, &objJoint, sizeof(objJoint));
		 break;
	 case POSE_REG_TYPE:
		 reg_manager_interface_getTypePr(&iType, reg.index);
		 memcpy(reg.value, &iType, sizeof(iType));
		 break;
	 case POSE_REG_ID:
		 reg_manager_interface_getIdPr(&iID, reg.index);
		 memcpy(reg.value, &iID, sizeof(iID));
		 break;
	 case POSE_REG_COMMENT:
		 reg_manager_interface_getCommentPr(reg.value, reg.index);
		 // memcpy(reg.value, cComment, sizeof(cComment));
		 break;
	 // string register
	 case STR_REG:
	     reg_manager_interface_getSr(&objSrRegData, reg.index);
		 memcpy(reg.value, &objSrRegData.id, sizeof(objSrRegData.id));
		 memcpy(reg.value + sizeof(objSrRegData.id), 
		 		objSrRegData.comment, sizeof(objSrRegData.comment));
		 memcpy(reg.value + sizeof(objSrRegData.id) + sizeof(objPrRegData.comment), 
		 		objSrRegData.value.c_str(), objSrRegData.value.length());
	 	break;
	 case STR_REG_VALUE:
	     reg_manager_interface_getValueSr(strSrValue, reg.index);
		 memcpy(reg.value, strSrValue.c_str(), strSrValue.length());
	 	 break;
	 case STR_REG_ID:
	     reg_manager_interface_getIdSr(&iID, reg.index);
		 memcpy(reg.value, &iID, sizeof(iID));
	 	 break;
	 case STR_REG_COMMENT:
	    reg_manager_interface_getCommentSr(reg.value, reg.index);
		// memcpy(reg.value, cComment, sizeof(cComment));
	 	break;
	 // number register
	 case NUM_REG:
	    reg_manager_interface_getR(&objRRegData, reg.index);
		objRRegDataPtr = (RRegData *)reg.value;
	    objRRegDataPtr->id = objRRegData.id;
		memcpy(objRRegDataPtr->comment, objRRegData.comment, 
						sizeof(objRRegData.comment));
		objRRegDataPtr->value  = objRRegData.value;
	 	break;
	 case NUM_REG_VALUE:
	    reg_manager_interface_getValueR(&dRValue, reg.index);
		memcpy(reg.value, &dRValue, sizeof(dRValue));
	 	break;
	 case NUM_REG_ID:
	    reg_manager_interface_getIdR(&iID, reg.index);
		memcpy(reg.value, &iID, sizeof(iID));
	 	break;
	 case NUM_REG_COMMENT:
	    reg_manager_interface_getCommentR(reg.value, reg.index);
		// memcpy(reg.value, cComment, sizeof(cComment));
	 	break;
	 // Special register for motion instruction
	 case MOT_REG:
	    reg_manager_interface_getMr(&objMrRegData, reg.index);
		objMrRegDataPtr = (MrRegData *)reg.value;
	    objMrRegDataPtr->id = objMrRegData.id;
		memcpy(objMrRegDataPtr->comment, objMrRegData.comment, 
						sizeof(objMrRegData.comment));
		objMrRegDataPtr->value  = objMrRegData.value;
	 	break;
	 case MOT_REG_VALUE:
	    reg_manager_interface_getValueMr(&iMrValue, reg.index);
		memcpy(reg.value, &iMrValue, sizeof(iMrValue));
	 	break;
	 case MOT_REG_ID:
	    reg_manager_interface_getIdMr(&iID, reg.index);
		memcpy(reg.value, &iID, sizeof(iID));
	 	break;
	 case MOT_REG_COMMENT:
	    reg_manager_interface_getCommentMr(reg.value, reg.index);
		// memcpy(reg.value, cComment, sizeof(cComment));
	 	break;
	 // home register
	 case HOME_REG:
		 reg_manager_interface_getHr(&objHrRegData, reg.index);
		 objHrRegDataPtr = (HrRegData *)reg.value;
		 objHrRegDataPtr->id = objHrRegData.id;
		 memcpy(objHrRegDataPtr->comment, 
		 		objHrRegData.comment, sizeof(objHrRegData.comment));
 		 objHrRegDataPtr->value = objHrRegData.value;
		 break;
	 case HOME_REG_JOINT:
		 reg_manager_interface_getJointHr(&objJoint, reg.index);
		 memcpy(reg.value, &objJoint, sizeof(objJoint));
		 break;
	 case HOME_REG_ID:
		 reg_manager_interface_getIdHr(&iID, reg.index);
		 memcpy(reg.value, &iID, sizeof(iID));
		 break;
	 case HOME_REG_COMMENT:
		 reg_manager_interface_getCommentHr(reg.value, reg.index);
		 // memcpy(reg.value, cComment, sizeof(cComment));
		 break;
	 // register of user coordinate offset
	 case UF_REG:
	    reg_manager_interface_getUf(reg.value, reg.index);
	 	break;
	 case UF_REG_COORD:
	    reg_manager_interface_getCoordinateUf(reg.value, reg.index);
	 	break;
	 case UF_REG_ID:
	    reg_manager_interface_getIdUf(&iID, reg.index);
	    memcpy(reg.value, &iID, sizeof(iID));
	 	break;
	 case UF_REG_COMMENT:
	    reg_manager_interface_getCommentUf(reg.value, reg.index);
	 	break;
	 // register of tool coordinate offset
	 case TF_REG:
	    reg_manager_interface_getTf(reg.value, reg.index);
	 	break;
	 case TF_REG_COORD:
	    reg_manager_interface_getCoordinateTf(reg.value, reg.index);
	 	break;
	 case TF_REG_ID:
	    reg_manager_interface_getIdTf(&iID, reg.index);
	    memcpy(reg.value, &iID, sizeof(iID));
	 	break;
	 case TF_REG_COMMENT:
	    reg_manager_interface_getCommentTf(reg.value, reg.index);
	 	break;
	 // stack register
	 case PL_REG:
	    reg_manager_interface_getPl(reg.value, reg.index);
	 	break;
	 case PL_REG_POSE:
	    reg_manager_interface_getPosePl(&objPoseEuler, reg.index);
		 memcpy(reg.value, &objPoseEuler, sizeof(objPoseEuler));
	 	break;
	 case PL_REG_PALLET:
	    reg_manager_interface_getPalletPl(&pltValue, reg.index);
		memcpy(reg.value, &pltValue, sizeof(pltValue));
	 	break;
	 case PL_REG_FLAG:
	    reg_manager_interface_getFlagPl(&iPlFlag, reg.index);
		 memcpy(reg.value, &iPlFlag, sizeof(iPlFlag));
	 	break;
	 case PL_REG_ID:
	    reg_manager_interface_getIdPl(&iID, reg.index);
	    memcpy(reg.value, &iID, sizeof(iID));
	 	break;
	 case PL_REG_COMMENT:
	    reg_manager_interface_getCommentPl(reg.value, reg.index);
	 	break;
 	 }
	 return 1;
}

int forgesight_mod_reg(RegMap & reg)
{
	PrRegData objPrRegData ;
	PrRegData * objPrRegDataPtr ;
	
	SrRegData objSrRegData ;
//	SrRegData * objSrRegDataPtr ;
	
	MrRegData objMrRegData ;
	MrRegData * objMrRegDataPtr ;
	
	RRegData objRRegData ;
	RRegData * objRRegDataPtr ;

	HrRegData objHrRegData ;
	HrRegData * objHrRegDataPtr ;
	

    int       iID ;
//    char      cComment[MAX_REG_COMMENT_LENGTH];

	PoseEuler objPoseEuler ;
    Joint     objJoint;
    int       iType ;
    
    std::string    strSrValue;
    double    dRValue;
    int       iMrValue;
	
    int       iPlFlag ;
	pl_t      pltValue ;
	
  	 printf("reg.type = %d.\n", reg.type);
	 
	 switch(reg.type)
	 {
	 // pose register
	 case POSE_REG:
	 	 objPrRegDataPtr = (PrRegData *)reg.value;
		 objPrRegData.id = objPrRegDataPtr->id;
		 memcpy(objPrRegData.comment, objPrRegDataPtr->comment, 
		 		sizeof(objPrRegData.comment));
		 objPrRegData.value = objPrRegDataPtr->value;
		 reg_manager_interface_setPr(&objPrRegData, reg.index);
		 break;
	 case POSE_REG_POSE:
		 memcpy(&objPoseEuler, reg.value, sizeof(objPoseEuler));
		 reg_manager_interface_setPosePr(&objPoseEuler, reg.index);
		 break;
	 case POSE_REG_JOINT:
		 memcpy(&objJoint, reg.value, sizeof(objJoint));
		 reg_manager_interface_setJointPr(&objJoint, reg.index);
		 break;
	 case POSE_REG_TYPE:
		 memcpy(&iType, reg.value, sizeof(iType));
		 reg_manager_interface_setTypePr(&iType, reg.index);
		 break;
	 case POSE_REG_ID:
		 memcpy(&iID, reg.value, sizeof(iID));
		 reg_manager_interface_setIdPr(&iID, reg.index);
		 break;
	 case POSE_REG_COMMENT:
		 // memcpy(cComment, reg.value, sizeof(cComment));
		 reg_manager_interface_setCommentPr(reg.value, reg.index);
		 break;
	 // string register
	 case STR_REG:
		 memcpy(&objSrRegData.id, reg.value, sizeof(objSrRegData.id));
		 memcpy(objSrRegData.comment, 
		 	    reg.value + sizeof(objSrRegData.id), 
		 		sizeof(objSrRegData.comment));
		 objSrRegData.value = string(reg.value + 
		 						sizeof(objSrRegData.id) + 
		 						sizeof(objPrRegData.comment)); 
	    reg_manager_interface_setSr(&objSrRegData, reg.index);
	 	break;
	 case STR_REG_VALUE:
		strSrValue = string(reg.value);
	    reg_manager_interface_setValueSr(strSrValue, reg.index);
	 	break;
	 case STR_REG_ID:
		memcpy(&iID, reg.value, sizeof(iID));
	    reg_manager_interface_setIdSr(&iID, reg.index);
	 	break;
	 case STR_REG_COMMENT:
		 // memcpy(cComment, reg.value, sizeof(cComment));
	    reg_manager_interface_setCommentSr(reg.value, reg.index);
	 	break;
	 // number register
	 case NUM_REG:
	 	objRRegDataPtr = (RRegData *)reg.value;
		 
		printf("objRRegDataPtr: id = %d, comment = %s\n", objRRegDataPtr->id, objRRegDataPtr->comment);
		printf("objRRegDataPtr: id = (%f) \n", objRRegDataPtr->value);
		 
 		objRRegData.id = objRRegDataPtr->id ;
		memcpy(objRRegData.comment, objRRegDataPtr->comment, 
		 		                 sizeof(objRRegData.comment));
		objRRegData.value = objRRegDataPtr->value ;
		printf("objRRegData: id = (%f) \n", objRRegData.value);

	    reg_manager_interface_setR(&objRRegData, reg.index);
	 	break;
	 case NUM_REG_VALUE:
		memcpy(&dRValue, reg.value, sizeof(dRValue));
	    reg_manager_interface_setValueR(&dRValue, reg.index);
	 	break;
	 case NUM_REG_ID:
		memcpy(&iID, reg.value, sizeof(iID));
	    reg_manager_interface_setIdR(&iID, reg.index);
	 	break;
	 case NUM_REG_COMMENT:
		// memcpy(Comment, reg.value, csizeof(cComment));
	    reg_manager_interface_setCommentR(reg.value, reg.index);
	 	break;
	 // Special register for motion instruction
	 case MOT_REG:
	 	 objMrRegDataPtr = (MrRegData *)reg.value;
		 
		 printf("objMrRegDataPtr: id = %d, comment = %s\n", objMrRegDataPtr->id, objMrRegDataPtr->comment);
		 printf("objMrRegDataPtr: id = (%f) \n", objMrRegDataPtr->value);

		objMrRegData.id = objMrRegDataPtr->id;
		memcpy(objMrRegData.comment, objMrRegDataPtr->comment, 
		 		sizeof(objMrRegData.comment));
		objMrRegData.value = objMrRegDataPtr->value;
		printf("MrRegData: id = (%d) \n", objMrRegData.value);

	    reg_manager_interface_setMr(&objMrRegData, reg.index);
	 	break;
	 case MOT_REG_VALUE:
		memcpy(&iMrValue, reg.value, sizeof(iMrValue));
	    reg_manager_interface_setValueMr(&iMrValue, reg.index);
	 	break;
	 case MOT_REG_ID:
		memcpy(&iID, reg.value, sizeof(iID));
	    reg_manager_interface_setIdMr(&iID, reg.index);
	 	break;
	 case MOT_REG_COMMENT:
		// memcpy(cComment, reg.value, sizeof(cComment));
	    reg_manager_interface_setCommentMr(reg.value, reg.index);
	 	break;
	 // home register
	 case HOME_REG:
	 	 objHrRegDataPtr = (HrRegData *)reg.value;
		 objHrRegData.id = objHrRegDataPtr->id;
		 memcpy(objHrRegData.comment, objHrRegDataPtr->comment, 
		 		sizeof(objHrRegData.comment));
		 objHrRegData.value = objHrRegDataPtr->value;
		 reg_manager_interface_setHr(&objHrRegData, reg.index);
		 break;
	 case HOME_REG_JOINT:
		 memcpy(&objJoint, reg.value, sizeof(objJoint));
		 reg_manager_interface_setJointHr(&objJoint, reg.index);
		 break;
	 case HOME_REG_ID:
		 memcpy(&iID, reg.value, sizeof(iID));
		 reg_manager_interface_setIdHr(&iID, reg.index);
		 break;
	 case HOME_REG_COMMENT:
		 // memcpy(cComment, reg.value, sizeof(cComment));
		 reg_manager_interface_setCommentHr(reg.value, reg.index);
		 break;
	 // register of user coordinate offset
	 case UF_REG:
	    reg_manager_interface_setUf(reg.value, reg.index);
	 	break;
	 case UF_REG_COORD:
	    reg_manager_interface_setCoordinateUf(reg.value, reg.index);
	 	break;
	 case UF_REG_ID:
	    memcpy(&iID, reg.value, sizeof(iID));
	    reg_manager_interface_setIdUf(&iID, reg.index);
	 	break;
	 case UF_REG_COMMENT:
	    reg_manager_interface_setCommentUf(reg.value, reg.index);
	 	break;
	 // register of tool coordinate offset
	 case TF_REG:
	    reg_manager_interface_setTf(reg.value, reg.index);
	 	break;
	 case TF_REG_COORD:
	    reg_manager_interface_setCoordinateTf(reg.value, reg.index);
	 	break;
	 case TF_REG_ID:
	    memcpy(&iID, reg.value, sizeof(iID));
	    reg_manager_interface_setIdTf(&iID, reg.index);
	 	break;
	 case TF_REG_COMMENT:
	    reg_manager_interface_setCommentTf(reg.value, reg.index);
	 	break;
	 // stack register
	 case PL_REG:
	    reg_manager_interface_setPl(reg.value, reg.index);
	 	break;
	 case PL_REG_POSE:
	    // reg_manager_interface_setPosePl(reg.value, reg.index);
	 	break;
	 case PL_REG_PALLET:
		memcpy(&pltValue, reg.value, sizeof(pltValue));
	    reg_manager_interface_setPalletPl(&pltValue, reg.index);
	 	break;
	 case PL_REG_FLAG:
		memcpy(&iPlFlag, reg.value, sizeof(iPlFlag));
	    reg_manager_interface_setFlagPl(&iPlFlag, reg.index);
	 	break;
	 case PL_REG_ID:
	    memcpy(&iID, reg.value, sizeof(iID));
	    reg_manager_interface_setIdPl(&iID, reg.index);
	 	break;
	 case PL_REG_COMMENT:
	    reg_manager_interface_setCommentPl(reg.value, reg.index);
	 	break;
 	 }
  	 printf("reg.type = %d end.\n", reg.type);
	 return 1;
}

int forgesight_del_reg(RegMap & reg)
{	
  	 printf("reg.type = %d.\n", reg.type);
	 
	 switch(reg.type)
	 {
	 // pose register
	 case POSE_REG:
		 reg_manager_interface_delPr(reg.index);
		 break;
	 // string register
	 case STR_REG:
		 reg_manager_interface_delSr(reg.index);
	 	break;
	 // number register
	 case NUM_REG:
	    reg_manager_interface_delR(reg.index);
	 	break;
	 // Special register for motion instruction
	 case MOT_REG:
	    reg_manager_interface_delMr(reg.index);
	 	break;
	 // home register
	 case HOME_REG:
		 reg_manager_interface_delHr(reg.index);
		 break;
 	 }
  	 printf("reg.type = %d end.\n", reg.type);
	 return 1;
}

std::vector<BaseRegData> forgesight_read_valid_pr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_pr_lst(start_id, size);
}

std::vector<BaseRegData> forgesight_read_valid_sr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_sr_lst(start_id, size);
}

std::vector<BaseRegData> forgesight_read_valid_r_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_r_lst(start_id, size);
}

std::vector<BaseRegData> forgesight_read_valid_mr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_mr_lst(start_id, size);
}

std::vector<BaseRegData> forgesight_read_valid_hr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_hr_lst(start_id, size);
}

