// #include "stdafx.h"
#ifdef WIN32
#pragma warning(disable : 4786)
#endif
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
#define TXT_P    "p"

#define TXT_PR    "pr"
#define TXT_SR    "sr"
#define TXT_R     "r"
#define TXT_MR    "mr"

#define TXT_MI    "mi"
#define TXT_MH    "mh"

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
	
	Posture posture ;
    Turn    turn;
//    int       iID ;
//    char      cComment[MAX_REG_COMMENT_LENGTH];
    std::string    strComment;

	PoseEuler objPoseEuler ;
    Joint     objJoint;
//    int       iType ;
    
    std::string    strSrValue;
	
//    int       iPlFlag ;
//  	pl_t      pltValue ;
	
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
			//	value->setPrRegDataValue(&objPrRegData);
				if(objPrRegData.value.pos_type == PR_REG_POS_TYPE_CARTESIAN)
				{
					objPoseEuler.point_.x_  = objPrRegData.value.pos[0];
                    objPoseEuler.point_.y_  = objPrRegData.value.pos[1];
                    objPoseEuler.point_.z_  = objPrRegData.value.pos[2];
                    objPoseEuler.euler_.a_  = objPrRegData.value.pos[3];
                    objPoseEuler.euler_.b_  = objPrRegData.value.pos[4];
                    objPoseEuler.euler_.c_  = objPrRegData.value.pos[5];
					
					value->setPoseValue(&objPoseEuler);
	       			FST_INFO("setPoseValue PR[%d].pos_type = %d at CART:(%f, %f, %f, %f, %f, %f)", 
						iRegIdx, value->getIntType(), 
						value->getPoseValue().point_.x_, value->getPoseValue().point_.y_, 
						value->getPoseValue().point_.z_, value->getPoseValue().euler_.a_, 
						value->getPoseValue().euler_.b_, value->getPoseValue().euler_.c_);
				}
				else if(objPrRegData.value.pos_type == PR_REG_POS_TYPE_JOINT)
				{
					objJoint.j1_ = objPrRegData.value.pos[0];
                    objJoint.j2_ = objPrRegData.value.pos[1];
                    objJoint.j3_ = objPrRegData.value.pos[2];
                    objJoint.j4_ = objPrRegData.value.pos[3];
                    objJoint.j5_ = objPrRegData.value.pos[4];
                    objJoint.j6_ = objPrRegData.value.pos[5];
					value->setJointValue(&objJoint);
	       			FST_INFO("setJointValue PR[%d].pos_type = %d at HOINT:(%f, %f, %f, %f, %f, %f)",
						iRegIdx, value->getIntType(), 
						value->getJointValue().j1_, value->getJointValue().j2_, 
						value->getJointValue().j3_, value->getJointValue().j4_, 
						value->getJointValue().j5_, value->getJointValue().j6_);
				}
				
				posture.flip   = objPrRegData.value.posture[0];	
				posture.arm = objPrRegData.value.posture[1];	
				posture.elbow = objPrRegData.value.posture[2];	
				posture.wrist  = objPrRegData.value.posture[3];
				value->setPosture(posture);
							
				turn.j1   = objPrRegData.value.turn[0];
				turn.j2   = objPrRegData.value.turn[1];
				turn.j3   = objPrRegData.value.turn[2];
				turn.j4   = objPrRegData.value.turn[3];
				turn.j5   = objPrRegData.value.turn[4];
				turn.j6   = objPrRegData.value.turn[5];
				turn.j7   = objPrRegData.value.turn[6];
				turn.j8   = objPrRegData.value.turn[7];
				turn.j9   = objPrRegData.value.turn[8];
				value->setTurn(turn);
			}
#else
			// value->setPrRegDataValue(&objPrRegData);
			if(objPrRegData.value.pos_type == PR_REG_POS_TYPE_JOINT)
			{
				objJoint.j1 = objPrRegData.value.joint_pos[0];
                objJoint.j2 = objPrRegData.value.joint_pos[1];
                objJoint.j3 = objPrRegData.value.joint_pos[2];
                objJoint.j4 = objPrRegData.value.joint_pos[3];
                objJoint.j5 = objPrRegData.value.joint_pos[4];
                objJoint.j6 = objPrRegData.value.joint_pos[5];
				value->setJointValue(&objJoint);
			}
			else if(objPrRegData.value.pos_type == PR_REG_POS_TYPE_CARTESIAN)
			{
				objPoseEuler.position    = objPrRegData.value.cartesian_pos.position;
                objPoseEuler.orientation = objPrRegData.value.cartesian_pos.orientation;
				value->setPoseValue(&objPoseEuler);
			}
				
			posture.flip   = objPrRegData.value.posture[0];	
			posture.arm = objPrRegData.value.posture[1];	
			posture.elbow = objPrRegData.value.posture[2];	
			posture.wrist  = objPrRegData.value.posture[3];
			value->setPosture(posture);
						
			turn.j1   = objPrRegData.value.turn[0];
			turn.j2   = objPrRegData.value.turn[1];
			turn.j3   = objPrRegData.value.turn[2];
			turn.j4   = objPrRegData.value.turn[3];
			turn.j5   = objPrRegData.value.turn[4];
			turn.j6   = objPrRegData.value.turn[5];
			turn.j7   = objPrRegData.value.turn[6];
			turn.j8   = objPrRegData.value.turn[7];
			turn.j9   = objPrRegData.value.turn[8];
			value->setTurn(turn);
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
				value->setDoubleValue(objPoseEuler.point_.x_);
#else
				value->setDoubleValue(objPoseEuler.position.x);
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
				value->setDoubleValue(objPoseEuler.point_.y_);
#else
				value->setDoubleValue(objPoseEuler.position.y);
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
				value->setDoubleValue(objPoseEuler.point_.z_);
#else
				value->setDoubleValue(objPoseEuler.position.z);
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
				value->setDoubleValue(objPoseEuler.euler_.a_);
#else
				value->setDoubleValue(objPoseEuler.orientation.a);
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
				value->setDoubleValue(objPoseEuler.euler_.b_);
#else
				value->setDoubleValue(objPoseEuler.orientation.b);
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
				value->setDoubleValue(objPoseEuler.euler_.c_);
#else
				value->setDoubleValue(objPoseEuler.orientation.c);
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
				value->setDoubleValue(objJoint.j1_);
#else
				value->setDoubleValue(objJoint.j1);
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
				value->setDoubleValue(objJoint.j2_);
#else
				value->setDoubleValue(objJoint.j2);
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
				value->setDoubleValue(objJoint.j3_);
#else
				value->setDoubleValue(objJoint.j3);
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
				value->setDoubleValue(objJoint.j4_);
#else
				value->setDoubleValue(objJoint.j4);
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
				value->setDoubleValue(objJoint.j5_);
#else
				value->setDoubleValue(objJoint.j5);
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
				value->setDoubleValue(objJoint.j6_);
#else
				value->setDoubleValue(objJoint.j6);
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
				value->setDoubleValue(objJoint.j7_);
#else
				value->setDoubleValue(objJoint.j7);
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
				value->setDoubleValue(objJoint.j8_);
#else
				value->setDoubleValue(objJoint.j8);
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
				value->setDoubleValue(objJoint.j9_);
#else
				value->setDoubleValue(objJoint.j9);
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
					value->setDoubleValue(objPrRegData.value.pos[0]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.pos[0]);
#else

				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.cartesian_pos.position.x);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.joint_pos[0]);
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
					value->setDoubleValue(objPrRegData.value.pos[1]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.pos[1]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.cartesian_pos.position.y);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.joint_pos[1]);
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
					value->setDoubleValue(objPrRegData.value.pos[2]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.pos[2]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.cartesian_pos.position.z);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.joint_pos[2]);
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
					value->setDoubleValue(objPrRegData.value.pos[3]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.pos[3]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.cartesian_pos.orientation.a);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.joint_pos[3]);
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
					value->setDoubleValue(objPrRegData.value.pos[4]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.pos[4]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.cartesian_pos.orientation.b);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.joint_pos[4]);
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
					value->setDoubleValue(objPrRegData.value.pos[5]);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.pos[5]);
#else
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.cartesian_pos.orientation.c);
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
					value->setDoubleValue(objPrRegData.value.joint_pos[5]);
#endif
				else
		       		FST_INFO("PR[%d].pos_type = %d ", 
		       			iRegIdx, objPrRegData.value.pos_type);
			}
		}
	}
	return 0 ;
}

void Split(const std::string& src, const std::string& separator, 
		   std::vector<std::string>& dest)
{
	string str = src;
	string substring;
	string::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator,start);
	do
	{
		if (index != string::npos)
		{    
			substring = str.substr(start,index-start );
			dest.push_back(substring);
			start =index+separator.size();
			index = str.find(separator,start);
			if (start == string::npos) break;
		}
	}while(index != string::npos);
	
	//the last part
	substring = str.substr(start);
	dest.push_back(substring);
}

/************************************************* 
	Function:		forgesight_registers_manager_set_register
	Description:	Set the register info from controller .
	Input:			thread_control_block  - interpreter info
	Input:			name             - register name, like PR[1]
	Input:			valueStart       - register value
	Return: 		1 - success
*************************************************/
#define  POSE_ELEMENTS_NUM     (1 + 6 + 4)   // 1Type + 6Pose + 4Poseture
int forgesight_registers_manager_set_register(
		struct thread_control_block* objThreadCntrolBlock, 
		char *name, eval_value * valueStart)
{
	bool isSetOK = false ;
	eval_value value;
	int boolValue;

	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	PrRegData objPrRegData ;
	
	PoseEuler pose ;
	Joint joint ;
	Posture posture = {1, 1, -1, 0};
	Turn    turn    = {0, 0, 0, 0, 0, 0, 0, 0, 0};
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

	if(!strcmp(reg_name, TXT_PR))
	{
		if(strlen(reg_member) == 0)
		{
			if (valueStart->hasType(TYPE_POSE) == TYPE_POSE)
			{
				pose    = valueStart->getPoseValue();
				posture = valueStart->getPosture();
				turn    = valueStart->getTurn();
				FST_INFO("Set POSE->PR:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				isSetOK = reg_manager_interface_setPosePr(&pose, &posture, &turn, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
				return 0 ;
			}
			else if (valueStart->hasType(TYPE_JOINT) == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				posture = valueStart->getPosture();
				turn    = valueStart->getTurn();
				FST_INFO("Set JOINT->PR:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
					reg_idx);
				isSetOK = reg_manager_interface_setJointPr(&joint, &posture, &turn, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
				return 0 ;
			}
			else if (valueStart->hasType(TYPE_STRING) == TYPE_STRING)
			{
				vector<string> strArray;
				string Insta;
				Split(valueStart->getStringValue(),"|",strArray);
				
				if(strArray.size() == POSE_ELEMENTS_NUM)
				{
					if(strArray[0] == "POINT")
					{
#ifndef WIN32
						joint.j1_ = atof(strArray[1].c_str()); 
						joint.j2_ = atof(strArray[2].c_str());
						joint.j3_ = atof(strArray[3].c_str()); 
						joint.j4_ = atof(strArray[4].c_str());
						joint.j5_ = atof(strArray[5].c_str()); 
						joint.j6_ = atof(strArray[6].c_str());
#else
						joint.j1  = atof(strArray[1].c_str()); 
						joint.j2  = atof(strArray[2].c_str());
						joint.j3  = atof(strArray[3].c_str()); 
						joint.j4  = atof(strArray[4].c_str());
						joint.j5  = atof(strArray[5].c_str()); 
						joint.j6  = atof(strArray[6].c_str());
#endif
						posture.flip     = atoi(strArray[7].c_str()); 
						posture.arm    = atoi(strArray[8].c_str()); 
						posture.elbow     = atoi(strArray[9].c_str());
						posture.wrist    = atoi(strArray[10].c_str()); 
						isSetOK = reg_manager_interface_setJointPr(&joint, &posture, &turn, iRegIdx);
						if(isSetOK == false)
							serror(objThreadCntrolBlock, 4);
					}
					else if(strArray[0] == "POSE")
					{
#ifndef WIN32
						pose.point_.x_ = atof(strArray[1].c_str()); 
						pose.point_.y_ = atof(strArray[2].c_str());
						pose.point_.z_ = atof(strArray[3].c_str()); 
						pose.euler_.a_ = atof(strArray[4].c_str());
						pose.euler_.b_ = atof(strArray[5].c_str()); 
						pose.euler_.c_ = atof(strArray[6].c_str());
#else
						pose.position.x    = atof(strArray[1].c_str()); 
						pose.position.y    = atof(strArray[2].c_str());
						pose.position.z    = atof(strArray[3].c_str()); 
						pose.orientation.a = atof(strArray[4].c_str());
						pose.orientation.b = atof(strArray[5].c_str()); 
						pose.orientation.c = atof(strArray[6].c_str());
#endif	
						posture.flip      = atoi(strArray[7].c_str()); 
						posture.arm    = atoi(strArray[8].c_str()); 
						posture.elbow    = atoi(strArray[9].c_str());
						posture.wrist     = atoi(strArray[10].c_str()); 
						isSetOK = reg_manager_interface_setPosePr(&pose, &posture, &turn, iRegIdx);
						if(isSetOK == false)
							serror(objThreadCntrolBlock, 4);
					}
					return 0 ;
				}
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_POSE))
		{
			if (valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{
#ifndef WIN32
				pose.point_.x_ = (double)valueStart->getDoubleValue();
#else
				pose.position.x = (double)valueStart->getDoubleValue();
#endif

				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.y_ = (double)value.getDoubleValue();
#else
				pose.position.y = (double)value.getDoubleValue();
#endif

				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.point_.z_ = (double)value.getDoubleValue();
#else
				pose.position.z = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.a_ = (double)value.getDoubleValue();
#else
				pose.orientation.a = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.b_ = (double)value.getDoubleValue();
#else
				pose.orientation.b = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				pose.euler_.c_ = (double)value.getDoubleValue();
#else
				pose.orientation.c = (double)value.getDoubleValue();
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
				isSetOK = reg_manager_interface_setPosePr(&pose, &posture, &turn, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
	 	      	return 0 ;
			}
			else if (valueStart->hasType(TYPE_POSE) == TYPE_POSE)
			{
				pose = valueStart->getPoseValue();
				posture = valueStart->getPosture();
				turn    = valueStart->getTurn();
				FST_INFO("Set POSE->POSE:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					pose.point_.x_, pose.point_.y_, pose.point_.z_, 
					pose.euler_.a_, pose.euler_.b_, pose.euler_.c_,
#else
					pose.position.x, pose.position.y, pose.position.z, 
					pose.orientation.a, pose.orientation.b, pose.orientation.c,
#endif
					reg_idx);
				isSetOK = reg_manager_interface_setPosePr(&pose, &posture, &turn, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
	    	   	return 0 ;
			}
		}
		else if (!strcmp(reg_member, TXT_POSE_X))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.x = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.y = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.z = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.a = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.b = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.c = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			if (valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{
#ifndef WIN32
				joint.j1_ = (double)valueStart->getDoubleValue();
#else
				joint.j1 = (double)valueStart->getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j2_ = (double)value.getDoubleValue();
#else
				joint.j2 = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j3_ = (double)value.getDoubleValue();
#else
				joint.j3 = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j4_ = (double)value.getDoubleValue();
#else
				joint.j4 = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j5_ = (double)value.getDoubleValue();
#else
				joint.j5 = (double)value.getDoubleValue();
#endif
				
				get_exp(objThreadCntrolBlock, &value, &boolValue);
#ifndef WIN32
				joint.j6_ = (double)value.getDoubleValue();
#else
				joint.j6 = (double)value.getDoubleValue();
#endif
				
				FST_INFO("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
					reg_idx);
				isSetOK = reg_manager_interface_setJointPr(&joint, &posture, &turn, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
	    	   	return 0 ;
			}
			else if (valueStart->hasType(TYPE_JOINT) == TYPE_JOINT)
			{
				joint = valueStart->getJointValue();
				posture = valueStart->getPosture();
				turn    = valueStart->getTurn();
				FST_INFO("Set JOINT:(%f, %f, %f, %f, %f, %f) to PR[%s]", 
#ifndef WIN32
					joint.j1_, joint.j2_, joint.j3_, joint.j4_, joint.j5_, joint.j6_, 
#else
					joint.j1, joint.j2, joint.j3, joint.j4, joint.j5, joint.j6, 
#endif
					reg_idx);
				isSetOK = reg_manager_interface_setJointPr(&joint, &posture, &turn, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
	    	  	return 0 ;
			}
			
		}
		else if (!strcmp(reg_member, TXT_JOINT_J1))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.joint_pos[0] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.joint_pos[1] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.joint_pos[2] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.joint_pos[3] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.joint_pos[4] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.joint_pos[5] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.x = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[0] = fValue ;
#else
					objPrRegData.value.joint_pos[0] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.y = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[1] = fValue ;
#else
					objPrRegData.value.joint_pos[1] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.cartesian_pos.position.z = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[2] = fValue ;
#else
					objPrRegData.value.joint_pos[2] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.a = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[3] = fValue ;
#else
					objPrRegData.value.joint_pos[3] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.b = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[4] = fValue ;
#else
					objPrRegData.value.joint_pos[4] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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
			double fValue = valueStart->getDoubleValue();
			
			if(reg_manager_interface_getPr(&objPrRegData, iRegIdx))
			{
				if(PR_REG_POS_TYPE_CARTESIAN == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.cartesian_pos.orientation.c = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
				}
				else if(PR_REG_POS_TYPE_JOINT == objPrRegData.value.pos_type)
				{
#ifndef WIN32
					objPrRegData.value.pos[5] = fValue ;
#else
					objPrRegData.value.joint_pos[5] = fValue ;
#endif
					isSetOK = reg_manager_interface_setPr(&objPrRegData, iRegIdx);
					if(isSetOK == false)
						serror(objThreadCntrolBlock, 4);
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

int forgesight_registers_manager_get_posture(Posture &posture)
{
	return reg_manager_interface_getPosture(posture);
}

int forgesight_registers_manager_get_turn(Turn &turn)
{
	return reg_manager_interface_getTurn(turn);
}

int forgesight_registers_manager_cartToJoint(PoseEuler pos, Joint &joint)
{
	return forgesight_registers_manager_cartToJoint(pos, joint);
}

int forgesight_registers_manager_jointToCart(Joint joint, PoseEuler &pos)
{
	return forgesight_registers_manager_jointToCart(joint, pos);
}

int forgesight_registers_manager_get_user_opmode(int& mode)
{
	return reg_manager_interface_getUserOpMode(mode);
}

int forgesight_registers_manager_get_resource(
							struct thread_control_block* objThreadCntrolBlock, 
							char *name, key_variable keyVar, eval_value * value)
{
	int    iResValue ;
	double dResValue ;
	string strResValue ;
	
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
				value->setDoubleValue((float)iResValue);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setDoubleValue(ptr->value);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueMI(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setDoubleValue((float)iResValue);
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
				value->setDoubleValue((float)iResValue);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setDoubleValue(ptr->value);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueMH(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setDoubleValue((float)iResValue);
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
			    value->setDoubleValue(dResValue);
			// RRegData * ptr = (RRegData *)reg_content_buffer ;
			// value->setDoubleValue(ptr->value);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueR(&dResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setDoubleValue(dResValue);
		}
	}
	else if(!strcmp(reg_name, TXT_MR))
	{
		if(strlen(reg_member) == 0)
		{
			FST_INFO("getMR at %d", iRegIdx);
			bRet = reg_manager_interface_getMr(&iResValue, iRegIdx);
	    	FST_INFO("Get at TXT_MR with MR[%d] = %d ", iRegIdx, iResValue);
			// value->setDoubleValue(ptr->value);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setDoubleValue((float)iResValue);
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueMr(&iResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
				value->setDoubleValue((float)iResValue);
		}
	}
	else if(!strcmp(reg_name, TXT_SR))
	{
		if(strlen(reg_member) == 0)
		{
			FST_INFO("TXT_SR at (%d) ", iRegIdx);
			bRet = reg_manager_interface_getSr(strResValue, iRegIdx);
			// SrRegData * ptr = (SrRegData *)reg_content_buffer ;
			// value->setStringValue(ptr->value);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				value->setStringValue(strResValue);
				value->setDoubleValue(atof(strResValue.c_str()));
			}
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			bRet = reg_manager_interface_getValueSr(strResValue, iRegIdx);
			if(bRet == false)
				serror(objThreadCntrolBlock, 4) ; 
			else
			{
				value->setStringValue(strResValue);
				value->setDoubleValue(atof(strResValue.c_str()));
			}
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
			value->setDoubleValue(1.0);
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
	bool isSetOK = false ;
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
			if(valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{    
				int iValue = (int)valueStart->getDoubleValue();
			    isSetOK = reg_manager_interface_setValueMI(&iValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_STRING) == TYPE_STRING)
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				int iValue = (int)atof(strValue.c_str());
			    isSetOK = reg_manager_interface_setValueMI(&iValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getDoubleValue();
			FST_INFO("Set VALUE:(%f) to MI[%s]", iValue, reg_idx);
			isSetOK = reg_manager_interface_setValueMI(&iValue, iRegIdx);
			if(isSetOK == false)
				serror(objThreadCntrolBlock, 4);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_MH))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{    
				int iValue = (int)valueStart->getDoubleValue();
			    isSetOK = reg_manager_interface_setValueMH(&iValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_STRING) == TYPE_STRING)
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				int iValue = (int)atof(strValue.c_str());
			    isSetOK = reg_manager_interface_setValueMH(&iValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getDoubleValue();
			FST_INFO("Set VALUE:(%d) to MH[%s]", iValue, reg_idx);
			isSetOK = reg_manager_interface_setValueMH(&iValue, iRegIdx);
			if(isSetOK == false)
				serror(objThreadCntrolBlock, 4);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_R))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{    
				double fValue = valueStart->getDoubleValue();
				FST_INFO("Set FLOAT:(%f) to R[%s]", fValue, reg_idx);
			    isSetOK = reg_manager_interface_setValueR(&fValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_STRING) == TYPE_STRING)
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				double fValue = atof(strValue.c_str());
			    isSetOK = reg_manager_interface_setValueR(&fValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			FST_INFO("Set to R[%s]", reg_idx);
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			double fValue = valueStart->getDoubleValue();
			FST_INFO("Set VALUE:(%f) to SR[%s]", fValue, reg_idx);
			isSetOK = reg_manager_interface_setValueR(&fValue, iRegIdx);
			if(isSetOK == false)
				serror(objThreadCntrolBlock, 4);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_MR))
	{
		if(strlen(reg_member) == 0)
		{
			if(valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{    
				int iValue = (int)valueStart->getDoubleValue();
			    isSetOK = reg_manager_interface_setValueMr(&iValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_STRING) == TYPE_STRING)
			{
				std::string strValue;
				strValue = valueStart->getStringValue();
				
				FST_INFO("Set MR token:(%s) to TYPE_STRING %s", 
						objThreadCntrolBlock->token, strValue.c_str());
				int iValue = (int)atof(strValue.c_str());
			    isSetOK = reg_manager_interface_setValueMr(&iValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			int iValue = (int)valueStart->getDoubleValue();
			FST_INFO("Set VALUE:(%d) to MR[%s]", iValue, reg_idx);
			isSetOK = reg_manager_interface_setValueMr(&iValue, iRegIdx);
			if(isSetOK == false)
				serror(objThreadCntrolBlock, 4);
	       	return 0 ;
		}
	}
	else if(!strcmp(reg_name, TXT_SR))
	{
		if(strlen(reg_member) == 0)
		{
			strValue = valueStart->getStringValue() ;
			if(valueStart->hasType(TYPE_STRING) == TYPE_STRING)
			{
				FST_INFO("Set TYPE_STRING token:(%s) to %s", 
						objThreadCntrolBlock->token, strValue.c_str());
			   isSetOK = reg_manager_interface_setValueSr(strValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			{
			    char cStringValue[64];
			    sprintf(cStringValue, "%f", valueStart->getDoubleValue());
				strValue = std::string(cStringValue);
				FST_INFO("Set TYPE_STRING token:(%s) to %s", 
						objThreadCntrolBlock->token, cStringValue);
			    isSetOK = reg_manager_interface_setValueSr(strValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_POSE) == TYPE_POSE)
			{
				char cStringValue[64];
#ifndef WIN32
				sprintf(cStringValue, "POSE|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f", 
					valueStart->getPoseValue().point_.x_,
					valueStart->getPoseValue().point_.y_,
					valueStart->getPoseValue().point_.z_,
					valueStart->getPoseValue().euler_.a_,
					valueStart->getPoseValue().euler_.b_,
					valueStart->getPoseValue().euler_.c_);
#else
				sprintf(cStringValue, "POSE|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f", 
					valueStart->getPoseValue().position.x   ,
					valueStart->getPoseValue().position.y   ,
					valueStart->getPoseValue().position.z   ,
					valueStart->getPoseValue().orientation.a,
					valueStart->getPoseValue().orientation.b,
					valueStart->getPoseValue().orientation.c);
#endif	
				sprintf(cStringValue, "%s|%d|%d|%d|%d",
					cStringValue, 
					valueStart->getPosture().flip  ,
					valueStart->getPosture().arm,
					valueStart->getPosture().elbow,
					valueStart->getPosture().wrist);

				strValue = std::string(cStringValue);
				FST_INFO("Set TYPE_STRING token:(%s) to %s", 
					objThreadCntrolBlock->token, cStringValue);
				isSetOK = reg_manager_interface_setValueSr(strValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
			else if(valueStart->hasType(TYPE_JOINT) == TYPE_JOINT)
			{
				char cStringValue[64];
#ifndef WIN32
				sprintf(cStringValue, "JOINT|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f", 
					valueStart->getJointValue().j1_,
					valueStart->getJointValue().j2_,
					valueStart->getJointValue().j3_,
					valueStart->getJointValue().j4_,
					valueStart->getJointValue().j5_,
					valueStart->getJointValue().j6_);
#else
				sprintf(cStringValue, "POSE|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f|%0.3f", 
					valueStart->getJointValue().j1,
					valueStart->getJointValue().j2,
					valueStart->getJointValue().j3,
					valueStart->getJointValue().j4,
					valueStart->getJointValue().j5,
					valueStart->getJointValue().j6);
#endif
				sprintf(cStringValue, "%s|%d|%d|%d|%d",
					cStringValue, 
					valueStart->getPosture().flip  ,
					valueStart->getPosture().arm,
					valueStart->getPosture().elbow,
					valueStart->getPosture().wrist);
				strValue = std::string(cStringValue);
				FST_INFO("Set TYPE_STRING token:(%s) to %s", 
					objThreadCntrolBlock->token, cStringValue);
				isSetOK = reg_manager_interface_setValueSr(strValue, iRegIdx);
				if(isSetOK == false)
					serror(objThreadCntrolBlock, 4);
			}
	       	return 0 ;
		}
		else if (!strcmp(reg_member, TXT_REG_VALUE))
		{
			get_token(objThreadCntrolBlock);
			FST_INFO("Set VALUE:(%s) to SR[%s]", 
				objThreadCntrolBlock->token, reg_idx);
			strValue = std::string(objThreadCntrolBlock->token);
			isSetOK = reg_manager_interface_setValueSr(strValue, iRegIdx);
			if(isSetOK == false)
				serror(objThreadCntrolBlock, 4);
	       	return 0 ;
		}
	}
	return 0;
}


/************************************************* 
	Function:		forgesight_registers_manager_get_register
	Description:	Get the register info from controller .
	Input:			thread_control_block  - interpreter info
	Input:			name             - register name, like PR[1]
	Output:			value            - register value
	Return: 		1 - success
*************************************************/
int forgesight_registers_manager_get_point(
			struct thread_control_block* objThreadCntrolBlock, 
							char *name, eval_value * valueStart)
{	
    vector<var_type>::reverse_iterator it ;
	eval_value value;
	
	bool bRet = false ;
	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	char reg_nomember_name[16] ;
	
	PoseEuler objPoseEuler ;
    Joint     objJoint;
//    int       iType ;
	
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
 
	FST_INFO("forgesight_registers_manager_get_point at %s[%d] ", reg_name, iRegIdx);
	value.resetNoneValue();
	if(!strcmp(reg_name, TXT_P))
	{
		memset(reg_nomember_name, 0x00, 16);
		sprintf(reg_nomember_name, "%s[%d]", reg_name, iRegIdx);
		// Get P register
		for(it
			= objThreadCntrolBlock->local_var_stack.rbegin();
		it != objThreadCntrolBlock->local_var_stack.rend(); ++it)
		{
			if(!strcmp(it->var_name, reg_nomember_name))  {
				value = it->value;
				break;
			}
		}
			
		if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J1))
		{
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				objPoseEuler = value.getPoseValue();
#ifndef WIN32
				valueStart->setDoubleValue(objPoseEuler.point_.x_);
#else
				valueStart->setDoubleValue(objPoseEuler.position.x);
#endif
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				objJoint = value.getJointValue();
#ifndef WIN32
				valueStart->setDoubleValue(objJoint.j1_);
#else
				valueStart->setDoubleValue(objJoint.j1);
#endif
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				objPoseEuler = value.getPoseValue();
#ifndef WIN32
				valueStart->setDoubleValue(objPoseEuler.point_.y_);
#else
				valueStart->setDoubleValue(objPoseEuler.position.y);
#endif
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				objJoint = value.getJointValue();
#ifndef WIN32
				valueStart->setDoubleValue(objJoint.j2_);
#else
				valueStart->setDoubleValue(objJoint.j2);
#endif
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				objPoseEuler = value.getPoseValue();
#ifndef WIN32
				valueStart->setDoubleValue(objPoseEuler.point_.z_);
#else
				valueStart->setDoubleValue(objPoseEuler.position.z);
#endif
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				objJoint = value.getJointValue();
#ifndef WIN32
				valueStart->setDoubleValue(objJoint.j3_);
#else
				valueStart->setDoubleValue(objJoint.j3);
#endif
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				objPoseEuler = value.getPoseValue();
#ifndef WIN32
				valueStart->setDoubleValue(objPoseEuler.euler_.a_);
#else
				valueStart->setDoubleValue(objPoseEuler.orientation.a);
#endif
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				objJoint = value.getJointValue();
#ifndef WIN32
				valueStart->setDoubleValue(objJoint.j4_);
#else
				valueStart->setDoubleValue(objJoint.j4);
#endif
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				objPoseEuler = value.getPoseValue();
#ifndef WIN32
				valueStart->setDoubleValue(objPoseEuler.euler_.b_);
#else
				valueStart->setDoubleValue(objPoseEuler.orientation.b);
#endif
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				objJoint = value.getJointValue();
#ifndef WIN32
				valueStart->setDoubleValue(objJoint.j5_);
#else
				valueStart->setDoubleValue(objJoint.j5);
#endif
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				objPoseEuler = value.getPoseValue();
#ifndef WIN32
				valueStart->setDoubleValue(objPoseEuler.euler_.c_);
#else
				valueStart->setDoubleValue(objPoseEuler.orientation.c);
#endif
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				objJoint = value.getJointValue();
#ifndef WIN32
				valueStart->setDoubleValue(objJoint.j6_);
#else
				valueStart->setDoubleValue(objJoint.j6);
#endif
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
int forgesight_registers_manager_set_point(
		struct thread_control_block* objThreadCntrolBlock, 
		char *name, eval_value * valueStart)
{
    vector<var_type>::reverse_iterator it ;
	eval_value value;
//	int boolValue;

	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
	char reg_nomember_name[16] ;
	
	PoseEuler pose ;
	Joint joint ;
	
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

	if(!strcmp(reg_name, TXT_P))
	{
		memset(reg_nomember_name, 0x00, 16);
		sprintf(reg_nomember_name, "%s[%d]", reg_name, iRegIdx);
		// Get P register
		for(it
			= objThreadCntrolBlock->local_var_stack.rbegin();
		it != objThreadCntrolBlock->local_var_stack.rend(); ++it)
		{
			if(!strcmp(it->var_name, reg_nomember_name))  {
				value = it->value;
				break;
			}
		}
		
		if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J1))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				pose = value.getPoseValue();
#ifndef WIN32
				pose.point_.x_ = fValue;
#else
				pose.position.x = fValue;
#endif
				value.setPoseValue(&pose);
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				joint = value.getJointValue();
#ifndef WIN32
				joint.j1_ = fValue;
#else
				joint.j1  = fValue;
#endif
				value.setJointValue(&joint);
			}
			else
			{
	       		FST_ERROR("Set PJ1:(%f) P[%d].pos_type = %d ", 
					fValue, iRegIdx, value.getIntType());
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J2))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				pose = value.getPoseValue();
#ifndef WIN32
				pose.point_.y_ = fValue;
#else
				pose.position.y = fValue;
#endif
				value.setPoseValue(&pose);
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				joint = value.getJointValue();
#ifndef WIN32
				joint.j2_ = fValue;
#else
				joint.j2  = fValue;
#endif
				value.setJointValue(&joint);
			}
			else
			{
	       		FST_ERROR("Set PJ1:(%f) P[%d].pos_type = %d ", 
					fValue, iRegIdx, value.getIntType());
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J3))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				pose = value.getPoseValue();
#ifndef WIN32
				pose.point_.z_ = fValue;
#else
				pose.position.z = fValue;
#endif
				value.setPoseValue(&pose);
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				joint = value.getJointValue();
#ifndef WIN32
				joint.j3_ = fValue;
#else
				joint.j3  = fValue;
#endif
				value.setJointValue(&joint);
			}
			else
			{
	       		FST_ERROR("Set PJ1:(%f) P[%d].pos_type = %d ", 
					fValue, iRegIdx, value.getIntType());
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J4))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				pose = value.getPoseValue();
#ifndef WIN32
				pose.euler_.a_ = fValue;
#else
				pose.orientation.a = fValue;
#endif
				value.setPoseValue(&pose);
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				joint = value.getJointValue();
#ifndef WIN32
				joint.j4_ = fValue;
#else
				joint.j4  = fValue;
#endif
				value.setJointValue(&joint);
			}
			else
			{
	       		FST_ERROR("Set PJ1:(%f) P[%d].pos_type = %d ", 
					fValue, iRegIdx, value.getIntType());
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J5))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				pose = value.getPoseValue();
#ifndef WIN32
				pose.euler_.b_ = fValue;
#else
				pose.orientation.b = fValue;
#endif
				value.setPoseValue(&pose);
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				joint = value.getJointValue();
#ifndef WIN32
				joint.j5_ = fValue;
#else
				joint.j5  = fValue;
#endif
				value.setJointValue(&joint);
			}
			else
			{
	       		FST_ERROR("Set PJ1:(%f) P[%d].pos_type = %d ", 
					fValue, iRegIdx, value.getIntType());
			}
		}
		else if (!strcmp(reg_member, TXT_PR_POSE_JOINT_J6))
		{
			double fValue = valueStart->getDoubleValue();
			
			if(TYPE_POSE == value.hasType(TYPE_POSE))
			{
				pose = value.getPoseValue();
#ifndef WIN32
				pose.euler_.c_ = fValue;
#else
				pose.orientation.c = fValue;
#endif
				value.setPoseValue(&pose);
			}
			else if(TYPE_JOINT == value.hasType(TYPE_JOINT))
			{
				joint = value.getJointValue();
#ifndef WIN32
				joint.j6_ = fValue;
#else
				joint.j6  = fValue;
#endif
				value.setJointValue(&joint);
			}
			else
			{
	       		FST_ERROR("Set PJ1:(%f) P[%d].pos_type = %d ", 
					fValue, iRegIdx, value.getIntType());
			}
		}
		
		// Set P register
		for(it
			= objThreadCntrolBlock->local_var_stack.rbegin();
		it != objThreadCntrolBlock->local_var_stack.rend(); ++it)
		{
			if(!strcmp(it->var_name, reg_nomember_name))  {
			    it->value = value;
				break;
			}
		}
	}
	return 0 ;
}


