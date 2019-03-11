// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innercmd.h"

#include "interpreter_common.h"
#ifdef WIN32
#include "reg_manager/reg_manager_interface_wrapper.h"
#else
#include "reg_manager_interface_wrapper.h"
using namespace fst_ctrl ;
enum {MAX_REG_COMMENT_LENGTH = 32,};
#endif

// Register name
#define TXT_PR    "pr"
#define TXT_SR    "sr"
#define TXT_R     "r"
#define TXT_MR    "mr"
#define TXT_HR    "hr"
#define TXT_MI    "mi"
#define TXT_MH    "mh"

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
/************************************************* 
	Function:		forgesight_registers_manager_get_register
	Description:	Get the register info from controller .
	Input:			thread_control_block  - interpreter info
	Input:			name             - register name, like PR[1]
	Output:			value            - register value
	Return: 		1 - success
*************************************************/
int forgesight_registers_manager_get_register(
			struct thread_control_block* objThreadCntrolBlock, 
							char *name, eval_value * value)
{	
	bool bRet = false ;
	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	// char reg_content_buffer[512] ;
	
	PrRegData objPrRegData ;
	SrRegData objSrRegData ;
	HrRegData objHrRegData ;

    int       iID ;
    char      cComment[MAX_REG_COMMENT_LENGTH];
    std::string    strComment;

	PoseEuler objPoseEuler ;
    Joint     objJoint;
    int       iType ;
    
    std::string    strSrValue;
	
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
 
	FST_INFO("forgesight_registers_manager_get_register at %s[%d] ", reg_name, iRegIdx);
	value->resetNoneValue();
	if(!strcmp(reg_name, TXT_PR))
	{
		if(strlen(reg_member) == 0)
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			// PrRegData * ptr = (PrRegData *)reg_content_buffer ;
#ifndef WIN32
			if(bRet == false)
			{
				serror(objThreadCntrolBlock, 4) ;
			}
			else
			{
				value->setPrRegDataValue(&objPrRegData);
			}
#else
			value->setPrRegDataValue(&objPrRegData);
#endif
		}
		// Implement for intergretion
		else if (!strcmp(reg_member, TXT_POSE))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setPoseValue(&objPoseEuler);
		}
		else if (!strcmp(reg_member, TXT_POSE_X))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objPoseEuler.point_.x_);
#else
				value->setFloatValue(objPoseEuler.position.x);
#endif
		}
		else if (!strcmp(reg_member, TXT_POSE_Y))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objPoseEuler.point_.y_);
#else
				value->setFloatValue(objPoseEuler.position.y);
#endif
		}
		else if (!strcmp(reg_member, TXT_POSE_Z))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objPoseEuler.point_.z_);
#else
				value->setFloatValue(objPoseEuler.position.z);
#endif
		}
		else if (!strcmp(reg_member, TXT_POSE_A))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objPoseEuler.euler_.a_);
#else
				value->setFloatValue(objPoseEuler.orientation.a);
#endif
		}
		else if (!strcmp(reg_member, TXT_POSE_B))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objPoseEuler.euler_.b_);
#else
				value->setFloatValue(objPoseEuler.orientation.b);
#endif
		}
		else if (!strcmp(reg_member, TXT_POSE_C))
		{
			bRet = reg_manager_interface_getPosePr(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objPoseEuler.euler_.c_);
#else
				value->setFloatValue(objPoseEuler.orientation.c);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setJointValue(&objJoint);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j1_);
#else
				value->setFloatValue(objJoint.j1);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j2_);
#else
				value->setFloatValue(objJoint.j2);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j3_);
#else
				value->setFloatValue(objJoint.j3);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j4_);
#else
				value->setFloatValue(objJoint.j4);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j5_);
#else
				value->setFloatValue(objJoint.j5);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j6_);
#else
				value->setFloatValue(objJoint.j6);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J7))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j7_);
#else
				value->setFloatValue(objJoint.j7);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J8))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j8_);
#else
				value->setFloatValue(objJoint.j8);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J9))
		{
			bRet = reg_manager_interface_getJointPr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j9_);
#else
				value->setFloatValue(objJoint.j9);
#endif
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J1))
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
#ifndef WIN32
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[0]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[0]);
#else

				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.cartesian_pos.position.x);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.joint_pos[0]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
	       				iRegIdx, objPrRegData.value.pos_type);
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
#ifndef WIN32
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[1]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[1]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.cartesian_pos.position.y);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.joint_pos[1]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
		       			iRegIdx, objPrRegData.value.pos_type);
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
#ifndef WIN32
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[2]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[2]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.cartesian_pos.position.z);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.joint_pos[2]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
		       			iRegIdx, objPrRegData.value.pos_type);
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
#ifndef WIN32
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[3]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[3]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.cartesian_pos.orientation.a);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.joint_pos[3]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
		       			iRegIdx, objPrRegData.value.pos_type);
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
#ifndef WIN32
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[4]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[4]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.cartesian_pos.orientation.b);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.joint_pos[4]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
		       			iRegIdx, objPrRegData.value.pos_type);
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			bRet = reg_manager_interface_getPr(&objPrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
#ifndef WIN32
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[5]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.pos[5]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.cartesian_pos.orientation.c);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setFloatValue(objPrRegData.value.joint_pos[5]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
		       			iRegIdx, objPrRegData.value.pos_type);
			}
		}
		else if (!strcmp(reg_member, TXT_REG_TYPE))
		{
			bRet = reg_manager_interface_getTypePr(&iType, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iType);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			bRet = reg_manager_interface_getIdPr(&iID, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			bRet = reg_manager_interface_getCommentPr(cComment, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				strComment = std::string(cComment);
				value->setStringValue(strComment);
			}
		}
	}
	else if(!strcmp(reg_name, TXT_SR))
	{
		if(strlen(reg_member) == 0)
		{
#ifndef WIN32
			FST_INFO("TXT_SR at (%d), (%s), (%s) ", 
				objSrRegData.id, objSrRegData.comment.c_str(), objSrRegData.value.c_str());
#else
			FST_INFO("TXT_SR at (%d), (%s), (%s) ", 
				objSrRegData.id, objSrRegData.comment, objSrRegData.value.c_str());
#endif
			bRet = reg_manager_interface_getSr(&objSrRegData, iRegIdx);
			// SrRegData * ptr = (SrRegData *)reg_content_buffer ;
			// value->setStringValue(ptr->value);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setSrRegDataValue(&objSrRegData);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueSr(strSrValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setStringValue(strSrValue);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			bRet = reg_manager_interface_getIdSr(&iID, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			bRet = reg_manager_interface_getCommentSr(cComment, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				strComment = std::string(cComment);
				value->setStringValue(strComment);
			}
		}
	}
	else if(!strcmp(reg_name, TXT_HR))
	{
		if(strlen(reg_member) == 0)
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			// PrRegData * ptr = (PrRegData *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setHrRegDataValue(&objHrRegData);
		}
		// Implement for intergretion
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setJointValue(&objJoint);
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j1_);
#else
				value->setFloatValue(objJoint.j1);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j2_);
#else
				value->setFloatValue(objJoint.j2);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j3_);
#else
				value->setFloatValue(objJoint.j3);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j4_);
#else
				value->setFloatValue(objJoint.j4);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j5_);
#else
				value->setFloatValue(objJoint.j5);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j6_);
#else
				value->setFloatValue(objJoint.j6);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J7))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j7_);
#else
				value->setFloatValue(objJoint.j7);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J8))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j8_);
#else
				value->setFloatValue(objJoint.j8);
#endif
		}
		else if (!strcmp(reg_member, TXT_JOINT_J9))
		{
			bRet = reg_manager_interface_getJointHr(&objJoint, iRegIdx);
			// Joint * ptr = (Joint *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
#ifndef WIN32
				value->setFloatValue(objJoint.j9_);
#else
				value->setFloatValue(objJoint.j9);
#endif
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_HR_JOINT_J1))
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(objHrRegData.value.joint_pos[0]);
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J2))
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(objHrRegData.value.joint_pos[1]);
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J3))
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(objHrRegData.value.joint_pos[2]);
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J4))
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(objHrRegData.value.joint_pos[3]);
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J5))
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(objHrRegData.value.joint_pos[4]);
		}
		else if (!strcmp(reg_member, TXT_HR_JOINT_J6))
		{
			bRet = reg_manager_interface_getHr(&objHrRegData, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(objHrRegData.value.joint_pos[5]);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			bRet = reg_manager_interface_getIdPr(&iID, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			bRet = reg_manager_interface_getCommentPr(cComment, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				strComment = std::string(cComment);
				value->setStringValue(strComment);
			}
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
			bRet = reg_manager_interface_getCoordinateUf(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setPoseValue(&objPoseEuler);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			bRet = reg_manager_interface_getIdUf(&iID, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			bRet = reg_manager_interface_getCommentUf(cComment, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				strComment = std::string(cComment);
				value->setStringValue(strComment);
			}
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
			bRet = reg_manager_interface_getCoordinateTf(&objPoseEuler, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setPoseValue(&objPoseEuler);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			bRet = reg_manager_interface_getIdTf(&iID, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			bRet = reg_manager_interface_getCommentTf(cComment, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				strComment = std::string(cComment);
				value->setStringValue(strComment);
			}
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
			bRet = reg_manager_interface_getPalletPl(&pltValue, iRegIdx);
			// _PoseEuler * ptr = (_PoseEuler *)reg_content_buffer ;
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
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
			bRet = reg_manager_interface_getFlagPl(&iPlFlag, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iPlFlag);
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			bRet = reg_manager_interface_getIdPl(&iID, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(iID);
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			bRet = reg_manager_interface_getCommentPl(cComment, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				strComment = std::string(cComment);
				value->setStringValue(strComment);
			}
		}
	}
	return 0 ;
}

/************************************************* 
	Function:		forgesight_registers_manager_set_register
	Description:	Set the register info from controller .
	Input:			thread_control_block  - interpreter info
	Input:			name             - register name, like PR[1]
	Input:			valueStart       - register value
	Return: 		1 - success
*************************************************/
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
			if (valueStart->getType() == TYPE_PR)
			{
				reg_manager_interface_setPr(&(valueStart->getPrRegDataValue()), iRegIdx);
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				FST_INFO("Set POSE->PR:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setPosePr(&pose, iRegIdx);
				return 0 ;
			}
			else if (valueStart->getType() == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				FST_INFO("Set JOINT->PR:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
					reg_idx);
				reg_manager_interface_setJointPr(&joint, iRegIdx);
				return 0 ;
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
#ifndef WIN32
				pose.point_.x_ = (double)valueStart->getFloatValue();
#else
				pose.position.x = (double)valueStart->getFloatValue();
#endif

				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.y_ = (double)value.getFloatValue();
#else
				pose.position.y = (double)value.getFloatValue();
#endif

				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.z_ = (double)value.getFloatValue();
#else
				pose.position.z = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.a_ = (double)value.getFloatValue();
#else
				pose.orientation.a = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.b_ = (double)value.getFloatValue();
#else
				pose.orientation.b = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.c_ = (double)value.getFloatValue();
#else
				pose.orientation.c = (double)value.getFloatValue();
#endif

				FST_INFO("Set FLOAT->POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setPosePr(&pose, iRegIdx);
	 	      	return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				FST_INFO("Set POSE->POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
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
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.x = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set X:(%f) but PR[%d] is not CARTESIAN", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set X:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_Y))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.y = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set Y:(%f) but PR[%d] is not CARTESIAN", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set Y:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_Z))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.z = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set Z:(%f) but PR[%d] is not CARTESIAN", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set Z:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_A))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.a = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set A:(%f) but PR[%d] is not CARTESIAN", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set A:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_B))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.b = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set B:(%f) but PR[%d] is not CARTESIAN", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set B:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE_C))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.c = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set C:(%f) but PR[%d] is not CARTESIAN", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set C:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT))
		{
			if (valueStart->getType() == TYPE_FLOAT)
			{
#ifndef WIN32
				joint.j1_ = (double)valueStart->getFloatValue();
#else
				joint.j1 = (double)valueStart->getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j2_ = (double)value.getFloatValue();
#else
				joint.j2 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j3_ = (double)value.getFloatValue();
#else
				joint.j3 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j4_ = (double)value.getFloatValue();
#else
				joint.j4 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j5_ = (double)value.getFloatValue();
#else
				joint.j5 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j6_ = (double)value.getFloatValue();
#else
				joint.j6 = (double)value.getFloatValue();
#endif
				
				FST_INFO("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
					reg_idx);
				reg_manager_interface_setJointPr(&joint, iRegIdx);
	    	   	return 0 ;
			}
			else if (valueStart->getType() == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				FST_INFO("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
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
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.joint_pos[0] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set J1:(%f) but PR[%d] is not PR_REG_POS_TYPE_JOINT", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set J1:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J2))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.joint_pos[1] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set J2:(%f) but PR[%d] is not PR_REG_POS_TYPE_JOINT", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set J2:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J3))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.joint_pos[2] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set J3:(%f) but PR[%d] is not PR_REG_POS_TYPE_JOINT", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set J3:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J4))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.joint_pos[3] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set J4:(%f) but PR[%d] is not PR_REG_POS_TYPE_JOINT", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set J4:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J5))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.joint_pos[4] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set J5:(%f) but PR[%d] is not PR_REG_POS_TYPE_JOINT", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set J5:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_JOINT_J6))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.joint_pos[5] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
					FST_ERROR("Set J6:(%f) but PR[%d] is not PR_REG_POS_TYPE_JOINT", 
						fValue, iRegIdx);
				}
			}
			else
			{
				FST_ERROR("Set J6:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		// General parameters for XML content
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J1))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.x = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.joint_pos[0] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		FST_ERROR("Set PJ1:(%f) PR[%d].pos_type = %d ", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				FST_ERROR("Set PJ1:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.y = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.joint_pos[1] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		FST_ERROR("Set PJ2:(%f) PR[%d].pos_type = %d ", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				FST_ERROR("Set PJ2:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.z = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.joint_pos[2] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		FST_ERROR("Set PJ3:(%f) PR[%d].pos_type = %d ", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				FST_ERROR("Set PJ3:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.a = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.joint_pos[3] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		FST_ERROR("Set PJ4:(%f) PR[%d].pos_type = %d ", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				FST_ERROR("Set PJ4:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.b = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.joint_pos[4] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		FST_ERROR("Set PJ5:(%f) PR[%d].pos_type = %d ", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				FST_ERROR("Set PJ5:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			double fValue = valueStart->getFloatValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.c = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.joint_pos[5] = fValue ;
#endif
					reg_manager_interface_setPr(&objPrRegData, iRegIdx);
				}
				else
				{
		       		FST_ERROR("Set PJ6:(%f) PR[%d].pos_type = %d ", 
						fValue, iRegIdx, objPrRegData.value.pos_type);
				}
			}
			else
			{
				FST_ERROR("Set PJ6:(%f) but PR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_TYPE))
		{
			int iType = (int)valueStart->getFloatValue();
			FST_INFO("Set TYPE:(%d) to PR[%s]", iType, reg_idx);
			reg_manager_interface_setTypePr(&iType, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			FST_INFO("Set ID:(%d) to PR[%s]", iID, reg_idx);
			reg_manager_interface_setIdPr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			FST_INFO("Set COMMENT:(%s) to PR[%s]", 
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
				FST_INFO("Set TYPE_STRING token:(%s) to %s", 
						objThreadCntrolBlock->token, strValue.c_str());
			   reg_manager_interface_setValueSr(strValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				FST_INFO("Set TYPE_SR token:(%s) to %s", 
						objThreadCntrolBlock->token, strValue.c_str());
				reg_manager_interface_setSr(&(valueStart->getSrRegDataValue()), iRegIdx);
			}
			else if(valueStart->getType() == TYPE_FLOAT)
			{
			    char cStringValue[64];
			    sprintf(cStringValue, "%d", (int)valueStart->getFloatValue());
				strValue = std::string(cStringValue);
				FST_INFO("Set TYPE_STRING token:(%s) to %s", 
						objThreadCntrolBlock->token, cStringValue);
			    reg_manager_interface_setValueSr(strValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			get_token(objThreadCntrolBlock);
			FST_INFO("Set VALUE:(%s) to SR[%s]", 
				objThreadCntrolBlock->token, reg_idx);
			strValue = std::string(objThreadCntrolBlock->token);
			reg_manager_interface_setValueSr(strValue, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			FST_INFO("Set ID:(%d) to SR[%s]", iID, reg_idx);
			reg_manager_interface_setIdSr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			FST_INFO("Set COMMENT:(%s) to SR[%s]", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentSr(
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
#ifndef WIN32
				joint.j1_ = (double)valueStart->getFloatValue();
#else
				joint.j1 = (double)valueStart->getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j2_ = (double)value.getFloatValue();
#else
				joint.j2 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j3_ = (double)value.getFloatValue();
#else
				joint.j3 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j4_ = (double)value.getFloatValue();
#else
				joint.j4 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j5_ = (double)value.getFloatValue();
#else
				joint.j5 = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j6_ = (double)value.getFloatValue();
#else
				joint.j6 = (double)value.getFloatValue();
#endif
				
				FST_INFO("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
					reg_idx);
				reg_manager_interface_setJointHr(&joint, iRegIdx);
	    	   	return 0 ;
			}
			else if (valueStart->getType() == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				FST_INFO("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
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
				FST_INFO("Set J1:(%f) but PR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set J2:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set J3:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set J4:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set J5:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set J6:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set HJ1:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set HJ2:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set HJ3:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set HJ4:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set HJ5:(%f) but HR[%d] not exist", fValue, iRegIdx);
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
				FST_ERROR("Set HJ6:(%f) but HR[%d] not exist", fValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)valueStart->getFloatValue();
			FST_INFO("Set ID:(%d) to HR[%s]", iID, reg_idx);
			reg_manager_interface_setIdHr(&iID, iRegIdx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			FST_INFO("Set COMMENT:(%s) to HR[%s]", 
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
#ifndef WIN32
				pose.point_.x_ = (double)value.getFloatValue();
#else
				pose.position.x = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.y_ = (double)value.getFloatValue();
#else
				pose.position.y = (double)value.getFloatValue();
#endif
		
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.z_ = (double)value.getFloatValue();
#else
				pose.position.z = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.z_ = (double)value.getFloatValue();
#else
				pose.orientation.a = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.b_ = (double)value.getFloatValue();
#else
				pose.orientation.b = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.c_ = (double)value.getFloatValue();
#else
				pose.orientation.c = (double)value.getFloatValue();
#endif
				
				FST_INFO("Set COORDINATE:(%f, %f, %f, %f, %f, %f) to UF[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setCoordinateUf(&pose, iRegIdx);
	       		return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				FST_INFO("Set POSE->COORDINATE:(%f, %f, %f, %f, %f, %f) to PR[%s]",
#ifndef WIN32 
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setCoordinateUf(&pose, iRegIdx);
	        	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)value.getFloatValue();
			FST_INFO("Set ID:(%d) to MR[%s]", iID, reg_idx);
			reg_manager_interface_setIdUf(&iID, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			FST_INFO("Set COMMENT:(%s) to MR[%s]", 
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
#ifndef WIN32
				pose.point_.x_ = (double)value.getFloatValue();
#else 
				pose.position.x = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.y_ = (double)value.getFloatValue();
#else 
				pose.position.y = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.z_ = (double)value.getFloatValue();
#else 
				pose.position.z = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.a_ = (double)value.getFloatValue();
#else 
				pose.orientation.a = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.b_ = (double)value.getFloatValue();
#else 
				pose.orientation.b = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.c_ = (double)value.getFloatValue();
#else 
				pose.orientation.c = (double)value.getFloatValue();
#endif
				
				FST_INFO("Set FLOAT->COORDINATE:(%f, %f, %f, %f, %f, %f) to TF[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setCoordinateTf(&pose, iRegIdx);
	        	return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				FST_INFO("Set POSE->COORDINATE:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setCoordinateTf(&pose, iRegIdx);
	        	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)value.getFloatValue();
			FST_INFO("Set ID:(%d) to MR[%s]", iID, reg_idx);
			reg_manager_interface_setIdTf(&iID, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			FST_INFO("Set COMMENT:(%s) to MR[%s]", 
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
#ifndef WIN32
				pose.point_.x_ = (double)value.getFloatValue();
#else 
				pose.position.x = (double)value.getFloatValue();
#endif

				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.y_ = (double)value.getFloatValue();
#else 
				pose.position.y = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.z_ = (double)value.getFloatValue();
#else 
				pose.position.z = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.a_ = (double)value.getFloatValue();
#else 
				pose.orientation.a = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.b_ = (double)value.getFloatValue();
#else 
				pose.orientation.b = (double)value.getFloatValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.c_ = (double)value.getFloatValue();
#else 
				pose.orientation.c = (double)value.getFloatValue();
#endif
				
				FST_INFO("Set FLOAT->PL_POSE:(%f, %f, %f, %f, %f, %f) to TF[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				reg_manager_interface_setPosePl(&pose, iRegIdx);
	        	return 0 ;
			}
			else if (valueStart->getType() == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				FST_INFO("Set POSE->PL_POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else 
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
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
			FST_INFO("Set ID:(%d) to MR[%s]", iFlagPl, reg_idx);
			reg_manager_interface_setFlagPl(&iFlagPl, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_ID))
		{
			int iID = (int)value.getFloatValue();
			FST_INFO("Set ID:(%d) to MR[%s]", iID, reg_idx);
			reg_manager_interface_setIdPl(&iID, iRegIdx);
	        return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_COMMENT))
		{
			// get_token(objThreadCntrolBlock);
			FST_INFO("Set COMMENT:(%s) to MR[%s]", 
				objThreadCntrolBlock->token, reg_idx);
			reg_manager_interface_setCommentPl(
				(char *)valueStart->getStringValue().c_str(), iRegIdx);
	        return 0 ;
		}
	}
	return 0 ;
}

/************************************************* 
	Function:		forgesight_read_valid_pr_lst
	Description:	Get valid PR.
	Input:			start_id         - start index
	Input:			size             - number of PR
	Return: 		std::vector<BaseRegData>
*************************************************/
std::vector<BaseRegData> forgesight_read_valid_pr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_pr_lst(start_id, size);
}

/************************************************* 
	Function:		forgesight_read_valid_sr_lst
	Description:	Get valid SR.
	Input:			start_id         - start index
	Input:			size             - number of SR
	Return: 		std::vector<BaseRegData>
*************************************************/
std::vector<BaseRegData> forgesight_read_valid_sr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_sr_lst(start_id, size);
}

/************************************************* 
	Function:		forgesight_read_valid_r_lst
	Description:	Get valid R.
	Input:			start_id         - start index
	Input:			size             - number of R
	Return: 		std::vector<BaseRegData>
*************************************************/
std::vector<BaseRegData> forgesight_read_valid_r_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_r_lst(start_id, size);
}

/************************************************* 
	Function:		forgesight_read_valid_mr_lst
	Description:	Get valid MR.
	Input:			start_id         - start index
	Input:			size             - number of MR
	Return: 		std::vector<BaseRegData>
*************************************************/
std::vector<BaseRegData> forgesight_read_valid_mr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_mr_lst(start_id, size);
}

/************************************************* 
	Function:		forgesight_read_valid_hr_lst
	Description:	Get valid HR.
	Input:			start_id         - start index
	Input:			size             - number of HR
	Return: 		std::vector<BaseRegData>
*************************************************/
std::vector<BaseRegData> forgesight_read_valid_hr_lst(int start_id, int size)
{
	return reg_manager_interface_read_valid_hr_lst(start_id, size);
}

int forgesight_registers_manager_get_joint(Joint &joint)
{
	return reg_manager_interface_getJoint(joint);
}

int forgesight_registers_manager_get_cart(PoseEuler &pos)
{
	return reg_manager_interface_getCart(pos);
}

int forgesight_registers_manager_cartToJoint(PoseEuler pos, Joint &joint)
{
	return forgesight_registers_manager_cartToJoint(pos, joint);
}

int forgesight_registers_manager_jointToCart(Joint joint, PoseEuler &pos)
{
	return forgesight_registers_manager_jointToCart(joint, pos);
}

int forgesight_registers_manager_get_resource(
							struct thread_control_block* objThreadCntrolBlock, 
							char *name, key_variable keyVar, eval_value * value)
{
	int    iResValue ;
	double dResValue ;
	
	bool bRet = false ;
	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
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
 
	FST_INFO("forgesight_registers_manager_get_resource at %s[%d] ", reg_name, iRegIdx);
	value->resetNoneValue();

	if(!strcmp(reg_name, TXT_MI))
	{
	    FST_INFO("forgesight_registers_manager_get_resource at TXT_MI ");
		if(strlen(reg_member) == 0)
		{
	        FST_INFO("forgesight_registers_manager_get_resource at TXT_MI ");
            // Use TXT_REG_VALUE
			bRet = reg_manager_interface_getMI(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue((float)iResValue);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setFloatValue(ptr->value);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueMI(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue((float)iResValue);
		}
	}
	else if(!strcmp(reg_name, TXT_MH))
	{
	    FST_INFO("forgesight_registers_manager_get_resource at TXT_MH ");
		if(strlen(reg_member) == 0)
		{
	        FST_INFO("forgesight_registers_manager_get_resource at TXT_MH = %d", iRegIdx);
            // Use TXT_REG_VALUE
			bRet = reg_manager_interface_getMH(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue((float)iResValue);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setFloatValue(ptr->value);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueMH(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue((float)iResValue);
		}
	}
	else if(!strcmp(reg_name, TXT_R))
	{
	    FST_INFO("forgesight_registers_manager_get_register at TXT_R ");
		if(strlen(reg_member) == 0)
		{
	        FST_INFO("reg_manager_interface_getR at TXT_R ");
            // Use TXT_REG_VALUE
			bRet = reg_manager_interface_getR(&dResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			    value->setFloatValue(dResValue);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setFloatValue(ptr->value);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueR(&dResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue(dResValue);
		}
	}
	else if(!strcmp(reg_name, TXT_MR))
	{
		if(strlen(reg_member) == 0)
		{
			FST_INFO("getMR at %d", iRegIdx);
			bRet = reg_manager_interface_getMr(&iResValue, iRegIdx);
	    	FST_INFO("Get at TXT_MR with MR[%d] = %d ", iRegIdx, iResValue);
			// value->setFloatValue(ptr->value);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue((float)iResValue);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueMr(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setFloatValue((float)iResValue);
		}
	}
	else
	{
		if((keyVar.key_type == KEYTYPE_CHAR)
			||(keyVar.key_type == KEYTYPE_INT16)
			||(keyVar.key_type == KEYTYPE_INT32)
			||(keyVar.key_type == KEYTYPE_INT64)
			||(keyVar.key_type == KEYTYPE_DOUBLE))
		{
			value->setFloatValue(1.0);
		}
		else if(keyVar.key_type == KEYTYPE_BYTEARRAY)
		{
			std::string strVal = string("TEST KEYTYPE_BYTEARRAY");
			value->setStringValue(strVal);
		}
	}
	
	return 0;
}

int forgesight_registers_manager_set_resource(
							struct thread_control_block* objThreadCntrolBlock, 
							char *name, key_variable keyVar, eval_value * valueStart)
{
	eval_value value;
//	int boolValue;

	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
//	PrRegData objPrRegData ;
//	HrRegData objHrRegData ;
	
//	PoseEuler pose ;
//	Joint joint ;
//	pl_t pltValue ;
	
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
	
	if(!strcmp(reg_name, TXT_MI))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->getType() == TYPE_FLOAT)
			{    
				int iValue = (int)valueStart->getFloatValue();
			    reg_manager_interface_setValueMI(&iValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				int iValue = (int)atof(strValue.c_str());
			    reg_manager_interface_setValueMI(&iValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getFloatValue();
			FST_INFO("Set VALUE:(%f) to MI[%s]", iValue, reg_idx);
			reg_manager_interface_setValueMI(&iValue, iRegIdx);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_MH))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->getType() == TYPE_FLOAT)
			{    
				int iValue = (int)valueStart->getFloatValue();
			    reg_manager_interface_setValueMH(&iValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				int iValue = (int)atof(strValue.c_str());
			    reg_manager_interface_setValueMH(&iValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getFloatValue();
			FST_INFO("Set VALUE:(%d) to MH[%s]", iValue, reg_idx);
			reg_manager_interface_setValueMH(&iValue, iRegIdx);
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
				FST_INFO("Set FLOAT:(%f) to R[%s]", fValue, reg_idx);
			    reg_manager_interface_setValueR(&fValue, iRegIdx);
			}
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				double fValue = atof(strValue.c_str());
			    reg_manager_interface_setValueR(&fValue, iRegIdx);
			}
			FST_INFO("Set to R[%s]", reg_idx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			double fValue = valueStart->getFloatValue();
			FST_INFO("Set VALUE:(%f) to SR[%s]", fValue, reg_idx);
			reg_manager_interface_setValueR(&fValue, iRegIdx);
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
			else if(valueStart->getType() == (int)(TYPE_STRING | TYPE_SR))
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				FST_INFO("Set TYPE_SR token:(%s) to %s", 
						objThreadCntrolBlock->token, strValue.c_str());
				int iValue = (int)atof(strValue.c_str());
			    reg_manager_interface_setValueMr(&iValue, iRegIdx);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getFloatValue();
			FST_INFO("Set VALUE:(%d) to MR[%s]", iValue, reg_idx);
			reg_manager_interface_setValueMr(&iValue, iRegIdx);
	       	return 0 ;
		}
	}
	return 0;
}



