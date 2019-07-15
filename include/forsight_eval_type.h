#ifndef FORSIGHT_EVAL_TYPE_H
#define FORSIGHT_EVAL_TYPE_H
#include <stdlib.h>
#include <string>

#ifdef WIN32
#include "fst_datatype.h" 
using namespace fst_controller;
#include "tp_reg_manager_interface.h"
// #include "reg-shmi/forsight_regs_shmi.h"
#else
#include "basic_alg_datatype.h" 
using namespace basic_alg;
#include "process_comm.h"
using namespace fst_ctrl ;
#endif

#define EVAL_CMP_TRUE            1
#define EVAL_CMP_FALSE           0
#define EVAL_CMP_ERROR           -1

typedef struct
{
	int row;
	int column;
	int layer;
}pl_t;

typedef enum _EvalValueType
{
	TYPE_NONE   = 0x00,
	TYPE_INT    = 0x01,
	TYPE_FLOAT  = 0x02,
	TYPE_STRING = 0x04,
	TYPE_POSE   = 0x08,
	TYPE_JOINT  = 0x10,
	TYPE_PL     = 0x20,
	
}EvalValueType;

typedef struct _AdditionalE {
    double e1;
    double e2;
    double e3;
} AdditionalE;

#define STR_VALUE_SIZE 1024

class eval_value {
public:
	eval_value() 
	{
		resetNoneValue() ;
	}
	int hasType(int type){
		return evalType & type ;
	}
	int getIntType(){
		return evalType ;
	}

private:
	// Not support TYPE_INT
	void setIntValue(int iVal){
		evalType |= TYPE_INT ;
		iValue = iVal ;
	}
	void increaseIntValue(){
		iValue++ ;
	}
	int getIntValue(){
		int iType = evalType & TYPE_INT;
		if(iType != 0)
			return iValue ;
		else {
			noticeErrorType(TYPE_INT) ;
			return -1;
		}
	}

public:
	void resetNoneValue(){
		evalType = TYPE_NONE ;
		fValue = -1 ;
		strContent = "";
		
#ifdef WIN32
		pose.position.x	  = 0.0;
		pose.position.y	  = 0.0;
		pose.position.z	  = 0.0;
		pose.orientation.a = 0.0;
		pose.orientation.b = 0.0;
		pose.orientation.c = 0.0;
		joint.j1 = 0.0;
		joint.j2 = 0.0;
		joint.j3 = 0.0;
		joint.j4 = 0.0;
		joint.j5 = 0.0;
		joint.j6 = 0.0;
#else
		pose.point_.x_	  = 0.0;
		pose.point_.y_	  = 0.0;
		pose.point_.z_	  = 0.0;
		pose.euler_.a_    = 0.0;
		pose.euler_.b_    = 0.0;
		pose.euler_.c_    = 0.0;
	   
		joint.j1_ = 0.0;
		joint.j2_ = 0.0;
		joint.j3_ = 0.0;
		joint.j4_ = 0.0;
		joint.j5_ = 0.0;
		joint.j6_ = 0.0;
#endif
	   
		tfIndex = -1 ;
		ufIndex = -1 ;
		
		postureInfo.arm = postureInfo.elbow =
			postureInfo.flip = postureInfo.wrist = 0 ;
		postureFake.arm = postureFake.elbow =
			postureFake.flip = postureFake.wrist = 0 ;
		
		turnInfo.j1 = turnInfo.j2 = turnInfo.j3 = 
			turnInfo.j4 = turnInfo.j5 = turnInfo.j6 = 
			turnInfo.j7 = turnInfo.j8 = turnInfo.j9 = 0;
		turnFake.j1 = turnFake.j2 = turnFake.j3 = 
			turnFake.j4 = turnFake.j5 = turnFake.j6 = 
			turnFake.j7 = turnFake.j8 = turnFake.j9 = 0;
		isPulse = false ;
	}
	// TYPE_FLOAT
	void setFloatValue(float fVal){
		evalType |= TYPE_FLOAT ;
		fValue = fVal ;
	}
	void increaseFloatValue(){
		fValue++ ;
	}
	float getFloatValue(){
		int iType = evalType & TYPE_FLOAT;
		if(iType != 0)
			return fValue ;
		else {
			noticeErrorType(TYPE_FLOAT) ;
			return -1;
		}
	}
	
	// TYPE_STRING
	void setStringValue(std::string& strVal){
		evalType |= TYPE_STRING ;
		strContent = strVal;
	}

	std::string getStringValue(){
		int iType = evalType & TYPE_STRING;
		if(iType != 0) {
			return strContent ;
		}
		else {
			noticeErrorType(TYPE_STRING) ;
			return std::string("");
		}
	}
	
	// TYPE_POSE
	void setPoseValue(PoseEuler * poseVal){
		evalType |= TYPE_POSE ;
		pose     = * poseVal ;
	}
	
	PoseEuler getPoseValue(){
		int iType = evalType & TYPE_POSE ;
		if(iType != 0) {
			return pose ;
		}
		else {
			noticeErrorType(TYPE_POSE) ;
			return poseFake;
		}
	}

	// TYPE_JOINT
	void setJointValue(Joint * jointVal){
		evalType |= TYPE_JOINT ;
		joint     = * jointVal ;
	}
	
	Joint getJointValue(){
		int iType = evalType & TYPE_JOINT;
		if(iType != 0) {
			return joint ;
		}
		else {
			noticeErrorType(TYPE_JOINT) ;
			return jointFake;
		}
	}
	
	void setUFIndex(int ufParam){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			ufIndex = ufParam ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return ;
		}
	}
	
	int getUFIndex(){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			return ufIndex ;
		}
		else {
		//	noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return -1;
		}
	}
	
	void setTFIndex(int tfParam){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			tfIndex = tfParam ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return ;
		}
	}
	
	int getTFIndex(){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			return tfIndex ;
		}
		else {
		//	noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return -1;
		}
	}
	
	void setPosture(Posture posture){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			postureInfo = posture ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return ;
		}
	}
	
	Posture getPosture(){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			return postureInfo ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return postureFake;
		}
	}

	void setTurn(Turn turn){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			turnInfo = turn ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return ;
		}
	}
	
	Turn getTurn(){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			return turnInfo ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return turnFake;
		}
	}

	void setPulse(bool bValue){
		isPulse = bValue;
	}
	
	bool getPulse(){
		return isPulse ;
	}
	
	void updateAdditionalE(AdditionalE additionParam){
		if((hasType(TYPE_JOINT) == TYPE_JOINT) 
			|| (hasType(TYPE_POSE) == TYPE_POSE)) {
			addition = additionParam ;
		}
		else {
			noticeErrorType(TYPE_JOINT | TYPE_POSE) ;
			return ;
		}
	}

	// TYPE_PL
	void setPLValue(pl_t * jointVal){
		evalType = TYPE_PL ;
		pallet     = * jointVal ;
	}
	
	pl_t getPLValue(){
		if(evalType == TYPE_PL) {
			return pallet ;
		}
		else {
			noticeErrorType(TYPE_PL) ;
			return palletFake;
		}
	}
	
public:
	void calcAdd(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = iValue + operand->getIntValue();
		}else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue = fValue + operand->getFloatValue();
		}else if(hasType(TYPE_JOINT) == TYPE_JOINT){
		    if(operand->hasType(TYPE_JOINT) == TYPE_JOINT)
		    {
		    	Joint jointOperand = operand->getJointValue();
#ifdef WIN32
				joint.j1 += jointOperand.j1;
				joint.j2 += jointOperand.j2;
				joint.j3 += jointOperand.j3;
				joint.j4 += jointOperand.j4;
				joint.j5 += jointOperand.j5;
				joint.j6 += jointOperand.j6;
				
				reg_pr.value.joint_pos[0] += jointOperand.j1;
				reg_pr.value.joint_pos[1] += jointOperand.j2;
				reg_pr.value.joint_pos[2] += jointOperand.j3;
				reg_pr.value.joint_pos[3] += jointOperand.j4;
				reg_pr.value.joint_pos[4] += jointOperand.j5;
				reg_pr.value.joint_pos[5] += jointOperand.j6;
#else
				joint.j1_ += jointOperand.j1_;
				joint.j2_ += jointOperand.j2_;
				joint.j3_ += jointOperand.j3_;
				joint.j4_ += jointOperand.j4_;
				joint.j5_ += jointOperand.j5_;
				joint.j6_ += jointOperand.j6_;
				
				reg_pr.value.pos[0] += jointOperand.j1_;
				reg_pr.value.pos[1] += jointOperand.j2_;
				reg_pr.value.pos[2] += jointOperand.j3_;
				reg_pr.value.pos[3] += jointOperand.j4_;
				reg_pr.value.pos[4] += jointOperand.j5_;
				reg_pr.value.pos[5] += jointOperand.j6_;
				reg_pr.value.pos[6] = 0.0;
				reg_pr.value.pos[7] = 0.0;
				reg_pr.value.pos[8] = 0.0;
#endif
		    }
			else {
				noticeErrorType(operand->getIntType()) ;
			}
		}
		else if(hasType(TYPE_POSE) == TYPE_POSE){
		    if(operand->hasType(TYPE_POSE) == TYPE_POSE)
		    {
		    	PoseEuler poseOperand = operand->getPoseValue();
		    	pose = calcCartesianPosAdd(pose, poseOperand);
#ifdef WIN32
				reg_pr.value.cartesian_pos.position    = pose.position;
				reg_pr.value.cartesian_pos.orientation = pose.orientation;
#else
				reg_pr.value.pos[0] = pose.point_.x_;
				reg_pr.value.pos[1] = pose.point_.y_;
				reg_pr.value.pos[2] = pose.point_.z_;
				reg_pr.value.pos[3] = pose.euler_.a_;
				reg_pr.value.pos[4] = pose.euler_.b_;
				reg_pr.value.pos[5] = pose.euler_.c_;
				reg_pr.value.pos[6] = 0.0;
				reg_pr.value.pos[7] = 0.0;
				reg_pr.value.pos[8] = 0.0;
#endif
		    }
			else {
				noticeErrorType(operand->getIntType()) ;
			}
		}
		else if(hasType(TYPE_STRING) == TYPE_STRING){
		    if(operand->hasType(TYPE_STRING) == TYPE_STRING)
		    {
		    	std::string strTmp;
		    	strTmp = operand->getStringValue();
				strContent   += strTmp;
			}
			else if(operand->hasType(TYPE_FLOAT) == TYPE_FLOAT)
		    {
		    	char strTmp[256];
				memset(strTmp, 0x00, 256);
		    	sprintf(strTmp, "%.3f", operand->getFloatValue());
				strContent   += strTmp;
		    }
			else {
				noticeErrorType(operand->getIntType()) ;
			}
		}
		else {
			noticeErrorType(operand->getIntType()) ;
		}
	}
	
	void calcSubtract(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = iValue - operand->getIntValue();
			return ;
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue = fValue - operand->getFloatValue();
			return ;
		}
		else if(hasType(TYPE_JOINT) == TYPE_JOINT){
		    if(operand->hasType(TYPE_JOINT) == TYPE_JOINT)
		    {
		    	Joint jointOperand = operand->getJointValue();
#ifdef WIN32
				joint.j1 -= jointOperand.j1;
				joint.j2 -= jointOperand.j2;
				joint.j3 -= jointOperand.j3;
				joint.j4 -= jointOperand.j4;
				joint.j5 -= jointOperand.j5;
				joint.j6 -= jointOperand.j6;

				reg_pr.value.joint_pos[0] -= jointOperand.j1;
				reg_pr.value.joint_pos[1] -= jointOperand.j2;
				reg_pr.value.joint_pos[2] -= jointOperand.j3;
				reg_pr.value.joint_pos[3] -= jointOperand.j4;
				reg_pr.value.joint_pos[4] -= jointOperand.j5;
				reg_pr.value.joint_pos[5] -= jointOperand.j6;
#else
				joint.j1_ -= jointOperand.j1_;
				joint.j2_ -= jointOperand.j2_;
				joint.j3_ -= jointOperand.j3_;
				joint.j4_ -= jointOperand.j4_;
				joint.j5_ -= jointOperand.j5_;
				joint.j6_ -= jointOperand.j6_;

				reg_pr.value.pos[0] -= jointOperand.j1_;
				reg_pr.value.pos[1] -= jointOperand.j2_;
				reg_pr.value.pos[2] -= jointOperand.j3_;
				reg_pr.value.pos[3] -= jointOperand.j4_;
				reg_pr.value.pos[4] -= jointOperand.j5_;
				reg_pr.value.pos[5] -= jointOperand.j6_;
				reg_pr.value.pos[6] = 0.0;
				reg_pr.value.pos[7] = 0.0;
				reg_pr.value.pos[8] = 0.0;
#endif
		    }
			else {
				noticeErrorType(operand->getIntType()) ;
				return ;
			}
		}
		else if(hasType(TYPE_POSE) == TYPE_POSE){
		    if(operand->hasType(TYPE_POSE) == TYPE_POSE)
		    {
		    	PoseEuler poseOperand = operand->getPoseValue();
		    	pose = calcCartesianPosSubtract(pose, poseOperand);
#ifdef WIN32
				reg_pr.value.cartesian_pos.position    = pose.position;
				reg_pr.value.cartesian_pos.orientation = pose.orientation;
#else
				reg_pr.value.pos[0] = pose.point_.x_;
				reg_pr.value.pos[1] = pose.point_.y_;
				reg_pr.value.pos[2] = pose.point_.z_;
				reg_pr.value.pos[3] = pose.euler_.a_;
				reg_pr.value.pos[4] = pose.euler_.b_;
				reg_pr.value.pos[5] = pose.euler_.c_;
				reg_pr.value.pos[6] = 0.0;
				reg_pr.value.pos[7] = 0.0;
				reg_pr.value.pos[8] = 0.0;
#endif
		    }
			else {
				noticeErrorType(operand->getIntType()) ;
				return ;
			}
		}
		else if(hasType(TYPE_STRING) == TYPE_STRING){
		    if(operand->hasType(TYPE_STRING) == TYPE_STRING)
		    {
		    	std::string testKey;
		    	testKey = operand->getStringValue();
				std::string testFind = std::string(strContent);
				
				if (strContent.rfind(testKey) != std::string::npos)
				{
					strContent = strContent.substr(0, strContent.length() - testKey.length());
				}
			}
			else {
				noticeErrorType(operand->getIntType()) ;
				return ;
			}
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}
	
	void calcMultiply(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = iValue * operand->getIntValue();
			return ;
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue = fValue * operand->getFloatValue();
			return ;
		}
		else if(hasType(TYPE_STRING) == TYPE_STRING){
		    if(operand->hasType(TYPE_FLOAT) == TYPE_FLOAT)
		    {
		    	std::string strOpt;
		    	strOpt = strContent;
				
		    	int iTmp = (int)operand->getFloatValue();
				for(int i = 0 ; i < iTmp ; i++)
				{
					strContent += strOpt;	
				}
			}
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}

	void calcDivide(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = iValue / operand->getIntValue();
			return ;
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue = fValue / operand->getFloatValue();
			return ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}
	
	void calcDIVToInt(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = iValue / operand->getIntValue();
			return ;
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue = fValue / operand->getFloatValue();
			fValue = (int)fValue ;
			return ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}

	void calcMod(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			int iTmp = 0 ;
			iTmp = iValue / operand->getIntValue();
			iValue = iValue - (iTmp * operand->getIntValue());
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue    = fmodf(fValue, operand->getFloatValue());
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}
 
	void calcPower(eval_value * operand)
	{
		int t = 0 ;
		int ex = (int)operand->getFloatValue();
		if(hasType(TYPE_INT) == TYPE_INT){
			if(ex == 0)
				iValue = 1 ;
			else {
				for(t=ex-1; t>0; --t) 
					iValue = (iValue) * ex;
			}
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			if(ex == 0)
				fValue = 1 ;
			else {
				for(t=ex-1; t>0; --t) 
					fValue = (fValue) * ex;
			}
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}
	
	void calcUnary()
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = -(iValue);
		}
		else if(hasType(TYPE_FLOAT) == TYPE_FLOAT){
			fValue = -(fValue);
		}
		else {
			noticeErrorType(TYPE_FLOAT | TYPE_INT) ;
			return ;
		}
	}

	PoseEuler calcCartesianPosAdd(PoseEuler & opt, PoseEuler & optAnd) 
	{
#ifdef WIN32
		opt.position.x    += optAnd.position.x;
		opt.position.y    += optAnd.position.y;
		opt.position.z    += optAnd.position.z;
		opt.orientation.a += optAnd.orientation.a;
		opt.orientation.b += optAnd.orientation.b;
		opt.orientation.c += optAnd.orientation.c;
#else
		opt.point_.x_ += optAnd.point_.x_ ;
		opt.point_.y_ += optAnd.point_.y_ ;
		opt.point_.z_ += optAnd.point_.z_ ;
		
		opt.euler_.a_ += optAnd.euler_.a_ ;
		opt.euler_.b_ += optAnd.euler_.b_ ;
		opt.euler_.c_ += optAnd.euler_.c_ ;
#endif
		return opt ;
	}
	PoseEuler calcCartesianPosSubtract(PoseEuler & opt, PoseEuler & optAnd) 
	{
#ifdef WIN32
		opt.position.x    -= optAnd.position.x;
		opt.position.y    -= optAnd.position.y;
		opt.position.z    -= optAnd.position.z;
		opt.orientation.a -= optAnd.orientation.a;
		opt.orientation.b -= optAnd.orientation.b;
		opt.orientation.c -= optAnd.orientation.c;
#else
		opt.point_.x_ -= optAnd.point_.x_ ;
		opt.point_.y_ -= optAnd.point_.y_ ;
		opt.point_.z_ -= optAnd.point_.z_ ;
		
		opt.euler_.a_ -= optAnd.euler_.a_ ;
		opt.euler_.b_ -= optAnd.euler_.b_ ;
		opt.euler_.c_ -= optAnd.euler_.c_ ;
#endif
		return opt ;
	}
	
	int calcLT(eval_value * operand)
	{
		if((hasType(TYPE_STRING) == TYPE_STRING)
			&& (operand->hasType(TYPE_STRING) == TYPE_STRING))
		{
			if(getStringValue() < operand->getStringValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else if((hasType(TYPE_FLOAT) == TYPE_FLOAT)
			&& (operand->hasType(TYPE_FLOAT) == TYPE_FLOAT))
		{
			if(getFloatValue() < operand->getFloatValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return EVAL_CMP_ERROR;
		}
	}

	int calcLE(eval_value * operand)
	{
		if((hasType(TYPE_STRING) == TYPE_STRING)
			&& (operand->hasType(TYPE_STRING) == TYPE_STRING))
		{
			if(getStringValue() <= operand->getStringValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else if((hasType(TYPE_FLOAT) == TYPE_FLOAT)
			&& (operand->hasType(TYPE_FLOAT) == TYPE_FLOAT))
		{
			if(getFloatValue() <= operand->getFloatValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return EVAL_CMP_ERROR;
		}
	}

	int calcGT(eval_value * operand)
	{
		if((hasType(TYPE_STRING) == TYPE_STRING)
			&& (operand->hasType(TYPE_STRING) == TYPE_STRING))
		{
			if(getStringValue() > operand->getStringValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else if((hasType(TYPE_FLOAT) == TYPE_FLOAT)
			&& (operand->hasType(TYPE_FLOAT) == TYPE_FLOAT))
		{
			if(getFloatValue() > operand->getFloatValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return EVAL_CMP_ERROR;
		}
	}

	int calcGE(eval_value * operand)
	{
		if((hasType(TYPE_STRING) == TYPE_STRING)
			&& (operand->hasType(TYPE_STRING) == TYPE_STRING))
		{
			if(getStringValue() >= operand->getStringValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else if((hasType(TYPE_FLOAT) == TYPE_FLOAT)
			&& (operand->hasType(TYPE_FLOAT) == TYPE_FLOAT))
		{
			if(getFloatValue() >= operand->getFloatValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return EVAL_CMP_ERROR;
		}
	}

	int calcEQ(eval_value * operand)
	{
		if((hasType(TYPE_STRING) == TYPE_STRING)
			&& (operand->hasType(TYPE_STRING) == TYPE_STRING))
		{
			if(getStringValue() == operand->getStringValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else if((hasType(TYPE_FLOAT) == TYPE_FLOAT)
			&& (operand->hasType(TYPE_FLOAT) == TYPE_FLOAT))
		{
			if(getFloatValue() == operand->getFloatValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return EVAL_CMP_ERROR;
		}
	}
	
	int calcNE(eval_value * operand)
	{
		if((hasType(TYPE_STRING) == TYPE_STRING)
			&& (operand->hasType(TYPE_STRING) == TYPE_STRING))
		{
			if(getStringValue() != operand->getStringValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else if((hasType(TYPE_FLOAT) == TYPE_FLOAT)
			&& (operand->hasType(TYPE_FLOAT) == TYPE_FLOAT))
		{
			if(getFloatValue() != operand->getFloatValue()) 
				return EVAL_CMP_TRUE;
			else
				return EVAL_CMP_FALSE ;
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return EVAL_CMP_ERROR;
		}
	}
private:
	void noticeErrorType(int type){
		printf("TypeError: call %04X in the %04X\n", type, evalType);
	}

private:
	int evalType ;
	// use the union or malloc in the future.
	// union {
		// Basic type ;
		int iValue ;
		float fValue ;
		std::string strContent;
		
		// All member of register
		PoseEuler pose;
		PoseEuler poseFake;
		Joint     joint;
		Joint     jointFake;
//		Coordinate c; 
		pl_t pallet;
		pl_t     palletFake;
		// additionalE
		AdditionalE addition ;
		// tf Index & uf Index
		int tfIndex ;
		int ufIndex ;
		Posture     postureInfo;
		Posture     postureFake;

		Turn        turnInfo ;
		Turn        turnFake ;
		
		bool isPulse ;
#if 0
		// All of register
		pr_shmi_t reg_pr ;
		sr_shmi_t reg_sr ;
		r_shmi_t  reg_r  ;
		mr_shmi_t reg_mr ;
		uf_shmi_t reg_ur ;
		tf_shmi_t reg_tf ;
#else
		// All of register
        PrRegData reg_pr ;
		PrRegData     prRegDataFake;
#endif
	// };
};

#endif

