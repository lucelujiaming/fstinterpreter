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

typedef struct
{
	std::string memberName;
	std::string memberTypeStr;
}eval_struct_member;

typedef enum _EvalValueType
{
	TYPE_NONE   = 0x00,
	TYPE_INT    = 0x01,
	TYPE_DOUBLE = 0x02,
	TYPE_STRING = 0x04,
	TYPE_POSE   = 0x08,
	TYPE_JOINT  = 0x10,
	TYPE_PL     = 0x20,
	TYPE_STRUCT = 0x40,
		
}EvalValueType;

#define EVAL_VALUE_TYPE_NONE      "none"
#define EVAL_VALUE_TYPE_INT       "integer"
#define EVAL_VALUE_TYPE_FLOAT     "double"
#define EVAL_VALUE_TYPE_STRING    "string"
#define EVAL_VALUE_TYPE_POSE      "pose"
#define EVAL_VALUE_TYPE_JOINT     "joint"
#define EVAL_VALUE_TYPE_PL        "pl"
#define EVAL_VALUE_TYPE_STRUCT    "struct"

// variable of eval_struct_mem_layout
typedef struct
{
	std::string memberName;
	std::string memberTypeStr;
//	union {
		int    iMember ;
		double dMember ;
		std::string strMember ;
//	};
}eval_struct_var;

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
		dValue = -1 ;
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
	// TYPE_DOUBLE
	void setDoubleValue(double fVal){
		evalType |= TYPE_DOUBLE ;
		dValue = fVal ;
	}
	void increaseDoubleValue(){
		dValue++ ;
	}
	double getDoubleValue(){
		int iType = evalType & TYPE_DOUBLE;
		if(iType != 0)
			return dValue ;
		else {
			noticeErrorType(TYPE_DOUBLE) ;
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
	
	// TYPE_STRUCTURE
	void setStructData(std::vector<eval_struct_var>& structVal){
		evalType |= TYPE_STRUCT ;
		vecStructVar = structVal;
	}
	
	std::vector<eval_struct_var> getStructData(){
		int iType = evalType & TYPE_STRUCT;
		if(iType != 0) {
			return vecStructVar ;
		}
		else {
			noticeErrorType(TYPE_STRING) ;
			return std::vector<eval_struct_var>();
		}
	}
public:
	void calcAdd(eval_value * operand)
	{
		if(hasType(TYPE_INT) == TYPE_INT){
			iValue = iValue + operand->getIntValue();
		}else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			dValue = dValue + operand->getDoubleValue();
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
#else
				joint.j1_ += jointOperand.j1_;
				joint.j2_ += jointOperand.j2_;
				joint.j3_ += jointOperand.j3_;
				joint.j4_ += jointOperand.j4_;
				joint.j5_ += jointOperand.j5_;
				joint.j6_ += jointOperand.j6_;
#endif
		    }
			else if(operand->hasType(TYPE_POSE) == TYPE_POSE)
		    {
		    	PoseEuler poseOperand = operand->getPoseValue();
		    	pose = calcCartesianPosAdd(pose, poseOperand);
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
		    }
			else if(operand->hasType(TYPE_JOINT) == TYPE_JOINT)
		    {
		    	Joint jointOperand = operand->getJointValue();
#ifdef WIN32
				joint.j1 += jointOperand.j1;
				joint.j2 += jointOperand.j2;
				joint.j3 += jointOperand.j3;
				joint.j4 += jointOperand.j4;
				joint.j5 += jointOperand.j5;
				joint.j6 += jointOperand.j6;
#else
				joint.j1_ += jointOperand.j1_;
				joint.j2_ += jointOperand.j2_;
				joint.j3_ += jointOperand.j3_;
				joint.j4_ += jointOperand.j4_;
				joint.j5_ += jointOperand.j5_;
				joint.j6_ += jointOperand.j6_;
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
			else if(operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
		    {
		    	char strTmp[256];
				memset(strTmp, 0x00, 256);
		    	sprintf(strTmp, "%.3f", operand->getDoubleValue());
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
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			dValue = dValue - operand->getDoubleValue();
			return ;
		}
		else if(hasType(TYPE_JOINT) == TYPE_JOINT){
		    if(operand->hasType(TYPE_POSE) == TYPE_POSE)
		    {
		    	PoseEuler poseOperand = operand->getPoseValue();
		    	pose = calcCartesianPosSubtract(pose, poseOperand);
		    }
			else if(operand->hasType(TYPE_JOINT) == TYPE_JOINT)
		    {
		    	Joint jointOperand = operand->getJointValue();
#ifdef WIN32
				joint.j1 -= jointOperand.j1;
				joint.j2 -= jointOperand.j2;
				joint.j3 -= jointOperand.j3;
				joint.j4 -= jointOperand.j4;
				joint.j5 -= jointOperand.j5;
				joint.j6 -= jointOperand.j6;
#else
				joint.j1_ -= jointOperand.j1_;
				joint.j2_ -= jointOperand.j2_;
				joint.j3_ -= jointOperand.j3_;
				joint.j4_ -= jointOperand.j4_;
				joint.j5_ -= jointOperand.j5_;
				joint.j6_ -= jointOperand.j6_;
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
		    }
			else if(operand->hasType(TYPE_JOINT) == TYPE_JOINT)
		    {
		    	Joint jointOperand = operand->getJointValue();
#ifdef WIN32
				joint.j1 -= jointOperand.j1;
				joint.j2 -= jointOperand.j2;
				joint.j3 -= jointOperand.j3;
				joint.j4 -= jointOperand.j4;
				joint.j5 -= jointOperand.j5;
				joint.j6 -= jointOperand.j6;
#else
				joint.j1_ -= jointOperand.j1_;
				joint.j2_ -= jointOperand.j2_;
				joint.j3_ -= jointOperand.j3_;
				joint.j4_ -= jointOperand.j4_;
				joint.j5_ -= jointOperand.j5_;
				joint.j6_ -= jointOperand.j6_;
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
		    //	std::string testKey;
		    //	testKey = operand->getStringValue();
			//	std::string testFind = std::string(strContent);
				
			//	if (strContent.rfind(testKey) != std::string::npos)
			//	{
			//		strContent = strContent.substr(0, strContent.length() - testKey.length());
			//	}
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
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			dValue = dValue * operand->getDoubleValue();
			return ;
		}
		else if(hasType(TYPE_JOINT) == TYPE_JOINT){
		    if(operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
		    {
#ifdef WIN32
				joint.j1   = joint.j1 * operand->getDoubleValue();
				joint.j2   = joint.j2 * operand->getDoubleValue();
				joint.j3   = joint.j3 * operand->getDoubleValue();
				joint.j4   = joint.j4 * operand->getDoubleValue();
				joint.j5   = joint.j5 * operand->getDoubleValue();
				joint.j6   = joint.j6 * operand->getDoubleValue();
#else
				joint.j1_  = joint.j1_ * operand->getDoubleValue();
				joint.j2_  = joint.j2_ * operand->getDoubleValue();
				joint.j3_  = joint.j3_ * operand->getDoubleValue();
				joint.j4_  = joint.j4_ * operand->getDoubleValue();
				joint.j5_  = joint.j5_ * operand->getDoubleValue();
				joint.j6_  = joint.j6_ * operand->getDoubleValue();
#endif
		    }
		}
		else if(hasType(TYPE_POSE) == TYPE_POSE){
		    if(operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
		    {
#ifdef WIN32
				pose.position.x    = pose.position.x    * operand->getDoubleValue();
				pose.position.y    = pose.position.y    * operand->getDoubleValue();
				pose.position.z    = pose.position.z    * operand->getDoubleValue();
				pose.orientation.a = pose.orientation.a * operand->getDoubleValue();
				pose.orientation.b = pose.orientation.b * operand->getDoubleValue();
				pose.orientation.c = pose.orientation.c * operand->getDoubleValue();
#else
				pose.point_.x_ = pose.point_.x_ * operand->getDoubleValue();
				pose.point_.y_ = pose.point_.y_ * operand->getDoubleValue();
				pose.point_.z_ = pose.point_.z_ * operand->getDoubleValue();
				
				pose.euler_.a_ = pose.euler_.a_ * operand->getDoubleValue();
				pose.euler_.b_ = pose.euler_.b_ * operand->getDoubleValue();
				pose.euler_.c_ = pose.euler_.c_ * operand->getDoubleValue();
#endif
		    }
		}
		else if(hasType(TYPE_STRING) == TYPE_STRING){
		    if(operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
		    {
		    	std::string strOpt;
		    	strOpt = strContent;
				
		    	int iTmp = (int)operand->getDoubleValue();
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
		    if(operand->getIntValue() != 0)
				iValue = iValue / operand->getIntValue();
			return ;
		}
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
		    if(operand->getDoubleValue() != 0.0)
				dValue = dValue / operand->getDoubleValue();
			return ;
		}
		else if(hasType(TYPE_JOINT) == TYPE_JOINT){
		    if(operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
		    {
		    	if(operand->getDoubleValue() != 0.0)
			    {
#ifdef WIN32
					joint.j1   = joint.j1 / operand->getDoubleValue();
					joint.j2   = joint.j2 / operand->getDoubleValue();
					joint.j3   = joint.j3 / operand->getDoubleValue();
					joint.j4   = joint.j4 / operand->getDoubleValue();
					joint.j5   = joint.j5 / operand->getDoubleValue();
					joint.j6   = joint.j6 / operand->getDoubleValue();
#else
					joint.j1_  = joint.j1_ / operand->getDoubleValue();
					joint.j2_  = joint.j2_ / operand->getDoubleValue();
					joint.j3_  = joint.j3_ / operand->getDoubleValue();
					joint.j4_  = joint.j4_ / operand->getDoubleValue();
					joint.j5_  = joint.j5_ / operand->getDoubleValue();
					joint.j6_  = joint.j6_ / operand->getDoubleValue();
#endif
			    }
		    }
		}
		else if(hasType(TYPE_POSE) == TYPE_POSE){
		    if(operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
		    {
		    	if(operand->getDoubleValue() != 0.0)
			    {
#ifdef WIN32
					pose.position.x    = pose.position.x    / operand->getDoubleValue();
					pose.position.y    = pose.position.y    / operand->getDoubleValue();
					pose.position.z    = pose.position.z    / operand->getDoubleValue();
					pose.orientation.a = pose.orientation.a / operand->getDoubleValue();
					pose.orientation.b = pose.orientation.b / operand->getDoubleValue();
					pose.orientation.c = pose.orientation.c / operand->getDoubleValue();
#else
					pose.point_.x_ = pose.point_.x_ / operand->getDoubleValue();
					pose.point_.y_ = pose.point_.y_ / operand->getDoubleValue();
					pose.point_.z_ = pose.point_.z_ / operand->getDoubleValue();
					
					pose.euler_.a_ = pose.euler_.a_ / operand->getDoubleValue();
					pose.euler_.b_ = pose.euler_.b_ / operand->getDoubleValue();
					pose.euler_.c_ = pose.euler_.c_ / operand->getDoubleValue();
#endif
			    }
		    }
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
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			dValue = dValue / operand->getDoubleValue();
			dValue = (int)dValue ;
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
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			dValue    = fmodf(dValue, operand->getDoubleValue());
		}
		else {
			noticeErrorType(operand->getIntType()) ;
			return ;
		}
	}
 
	void calcPower(eval_value * operand)
	{
		int t = 0 ;
		int ex = (int)operand->getDoubleValue();
		if(hasType(TYPE_INT) == TYPE_INT){
			if(ex == 0)
				iValue = 1 ;
			else {
				for(t=ex-1; t>0; --t) 
					iValue = (iValue) * ex;
			}
		}
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			if(ex == 0)
				dValue = 1 ;
			else {
				for(t=ex-1; t>0; --t) 
					dValue = (dValue) * ex;
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
		else if(hasType(TYPE_DOUBLE) == TYPE_DOUBLE){
			dValue = -(dValue);
		}
		else {
			noticeErrorType(TYPE_DOUBLE | TYPE_INT) ;
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
		else if((hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			&& (operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE))
		{
			if(getDoubleValue() < operand->getDoubleValue()) 
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
		else if((hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			&& (operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE))
		{
			if(getDoubleValue() <= operand->getDoubleValue()) 
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
		else if((hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			&& (operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE))
		{
			if(getDoubleValue() > operand->getDoubleValue()) 
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
		else if((hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			&& (operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE))
		{
			if(getDoubleValue() >= operand->getDoubleValue()) 
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
		else if((hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			&& (operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE))
		{
			if(getDoubleValue() == operand->getDoubleValue()) 
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
		else if((hasType(TYPE_DOUBLE) == TYPE_DOUBLE)
			&& (operand->hasType(TYPE_DOUBLE) == TYPE_DOUBLE))
		{
			if(getDoubleValue() != operand->getDoubleValue()) 
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
		double dValue ;
		std::string strContent;
		
		// All member of register
		PoseEuler pose;
		PoseEuler poseFake;
		Joint     joint;
		Joint     jointFake;

		std::vector<eval_struct_var> vecStructVar ;
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
		
		// All of register
        PrRegData reg_pr ;
		PrRegData     prRegDataFake;
#endif
	// };
};

#endif

