/**
 * @file main.cpp
 * @brief :main function of gtest
 * @author Lujiaming
 * @version 1.0.0
 * @date 2017-11-08
 */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include<iostream>

#include <cmath>
#include "forsight_eval_type.h"

#include "reg_manager/reg_manager_interface_wrapper.h"
#include "forsight_innerfunc.h"
#include "forsight_io_controller.h"

#ifndef WIN32
void signalInterrupt(int signo) 
{
	if(log_ptr_ != NULL)
	{
        FST_INFO("Free log_ptr_ in the signalInterrupt");
		
		delete log_ptr_;
		log_ptr_ = NULL ;
	}
    _exit(0);
}
#endif

void test_basic_pr_calculation()
{
    char * cPR1Str = (char *)"pr[1]" ;
    char * cPR2Str = (char *)"pr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;
	
	thread_control_block objLocalThreadCntrolBlock;
		
	// PR Calculation
	result_fst.resetNoneValue();
	forgesight_registers_manager_get_register(&objLocalThreadCntrolBlock, cPR1Str, &result_fst);
	result_snd.resetNoneValue();
	forgesight_registers_manager_get_register(&objLocalThreadCntrolBlock, cPR2Str, &result_snd);
	result_fst.calcAdd(&result_snd);
	if(result_fst.hasType(TYPE_POSE) == TYPE_POSE)
	{
#ifndef WIN32
	    FST_INFO("PR Add :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().point_.x_, result_fst.getPoseValue().point_.y_, 
			result_fst.getPoseValue().point_.z_, result_fst.getPoseValue().euler_.a_, 
			result_fst.getPoseValue().euler_.b_, result_fst.getPoseValue().euler_.c_);
#else
	    FST_INFO("PR Add :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().position.x,    result_fst.getPoseValue().position.y, 
			result_fst.getPoseValue().position.z,    result_fst.getPoseValue().orientation.a, 
			result_fst.getPoseValue().orientation.b, result_fst.getPoseValue().orientation.c);
#endif
	}
	else if(result_fst.hasType(TYPE_JOINT) == TYPE_JOINT)
	{
	    FST_INFO("PR Add::(%f, %f, %f, %f, %f, %f) in MovJ", 
#ifndef WIN32
			result_fst.getJointValue().j1_, result_fst.getJointValue().j2_, 
			result_fst.getJointValue().j3_, result_fst.getJointValue().j4_, 
			result_fst.getJointValue().j5_, result_fst.getJointValue().j6_);
#else
			result_fst.getJointValue().j1, result_fst.getJointValue().j2, 
			result_fst.getJointValue().j3, result_fst.getJointValue().j4, 
			result_fst.getJointValue().j5, result_fst.getJointValue().j6);
#endif		
	}

	result_fst.resetNoneValue();
	forgesight_registers_manager_get_register(&objLocalThreadCntrolBlock, cPR1Str, &result_fst);
	result_snd.resetNoneValue();
	forgesight_registers_manager_get_register(&objLocalThreadCntrolBlock, cPR2Str, &result_snd);
	result_fst.calcSubtract(&result_snd);
	if(result_fst.hasType(TYPE_POSE) == TYPE_POSE)
	{
#ifndef WIN32
	    FST_INFO("PR Add :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().point_.x_, result_fst.getPoseValue().point_.y_, 
			result_fst.getPoseValue().point_.z_, result_fst.getPoseValue().euler_.a_, 
			result_fst.getPoseValue().euler_.b_, result_fst.getPoseValue().euler_.c_);
#else
	    FST_INFO("PR Add :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().position.x,    result_fst.getPoseValue().position.y, 
			result_fst.getPoseValue().position.z,    result_fst.getPoseValue().orientation.a, 
			result_fst.getPoseValue().orientation.b, result_fst.getPoseValue().orientation.c);
#endif
	}
	else if(result_fst.hasType(TYPE_JOINT) == TYPE_JOINT)
	{
	    FST_INFO("PR Add::(%f, %f, %f, %f, %f, %f) in MovJ", 
#ifndef WIN32
			result_fst.getJointValue().j1_, result_fst.getJointValue().j2_, 
			result_fst.getJointValue().j3_, result_fst.getJointValue().j4_, 
			result_fst.getJointValue().j5_, result_fst.getJointValue().j6_);
#else
			result_fst.getJointValue().j1, result_fst.getJointValue().j2, 
			result_fst.getJointValue().j3, result_fst.getJointValue().j4, 
			result_fst.getJointValue().j5, result_fst.getJointValue().j6);
#endif		
	}
}

void test_basic_r_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cR1Str = (char *)"r[1]" ;
    char * cR2Str = (char *)"r[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;
	
	thread_control_block objLocalThreadCntrolBlock;

	// R Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"r", keyVar))
	{
	//	if(strchr(cR1Str, '['))
	//	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("R Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcSubtract(&result_snd);
		FST_INFO("R Sub: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcMultiply(&result_snd);
		FST_INFO("R Mul: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcDivide(&result_snd);
		FST_INFO("R Div: result_fst = %f \n", result_fst.getFloatValue());
	}
}

void test_peak_r_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cR1Str = (char *)"r[1]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"mr", keyVar))
	{
		result_fst.resetNoneValue();
		result_fst.setFloatValue(123.456);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		FST_INFO("R Add: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(0.0);
		result_fst.calcDivide(&result_snd);
		FST_INFO("R DivToZero: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		FST_INFO("R MR1: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(999241609382799593429992484890476544.456);
		for(int i = 0 ; i < 5000 ; i++)
		{
			result_fst.calcAdd(&result_snd);
		}
		FST_INFO("R AddOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		FST_INFO("R MR1: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(999241609382799593429992484890476544.456);
		for(int i = 0 ; i < 5000 ; i++)
		{
			result_fst.calcSubtract(&result_snd);
		}
		FST_INFO("R SubOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		FST_INFO("R Add: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(64373313662928145088512.456);
		for(int i = 0 ; i < 5 ; i++)
		{
			result_fst.calcMultiply(&result_snd);
		}
		FST_INFO("R MulOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		FST_INFO("R Add: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(64373313662928145088512.456);
		for(int i = 0 ; i < 5 ; i++)
		{
			result_fst.calcDivide(&result_snd);
		}
		FST_INFO("MR DivOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());
	}
}

void test_basic_mr_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cMR1Str = (char *)"mr[1]" ;
    char * cMR2Str = (char *)"mr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"mr", keyVar))
	{
	//	if(strchr(cMR1Str, '['))
	//	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcSubtract(&result_snd);
		FST_INFO("MR Sub: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcMultiply(&result_snd);
		FST_INFO("MR Mul: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcDivide(&result_snd);
		FST_INFO("MR Div: result_fst = %f \n", result_fst.getFloatValue());
		
	//	}
	}
}

void test_peak_mr_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cMR1Str = (char *)"mr[1]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"mr", keyVar))
	{
		result_fst.resetNoneValue();
		result_fst.setFloatValue(123.456);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(0.0);
		result_fst.calcDivide(&result_snd);
		FST_INFO("MR DivToZero: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		FST_INFO("MR MR1: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(999241609382799593429992484890476544.456);
		for(int i = 0 ; i < 5000 ; i++)
		{
			result_fst.calcAdd(&result_snd);
		}
		FST_INFO("MR AddOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		FST_INFO("MR MR1: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(999241609382799593429992484890476544.456);
		for(int i = 0 ; i < 5000 ; i++)
		{
			result_fst.calcSubtract(&result_snd);
		}
		FST_INFO("MR SubOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(64373313662928145088512.456);
		for(int i = 0 ; i < 5 ; i++)
		{
			result_fst.calcMultiply(&result_snd);
		}
		FST_INFO("MR MulOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
		result_snd.resetNoneValue();
		result_snd.setFloatValue(64373313662928145088512.456);
		for(int i = 0 ; i < 5 ; i++)
		{
			result_fst.calcDivide(&result_snd);
		}
		FST_INFO("MR DivOverflow: result_fst = %f \n", 
			result_fst.getFloatValue());
	}
}

void test_r2mr_combine_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cR1Str  = (char *)"r[1]" ;
    char * cMR2Str = (char *)"mr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;
	
	thread_control_block objLocalThreadCntrolBlock;

	// R Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"r", keyVar))
	{
	//	if(strchr(cR1Str, '['))
	//	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("R2MR Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcSubtract(&result_snd);
		FST_INFO("R2MR Sub: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcMultiply(&result_snd);
		FST_INFO("R2MR Mul: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcDivide(&result_snd);
		FST_INFO("R2MR Div: result_fst = %f \n", result_fst.getFloatValue());
	}
}


void test_mr2r_combine_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cMR1Str = (char *)"mr[1]" ;
    char * cR2Str = (char *)"r[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;
	
	thread_control_block objLocalThreadCntrolBlock;

	// R Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"r", keyVar))
	{
	//	if(strchr(cR1Str, '['))
	//	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("R Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcSubtract(&result_snd);
		FST_INFO("R Sub: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcMultiply(&result_snd);
		FST_INFO("R Mul: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcDivide(&result_snd);
		FST_INFO("R Div: result_fst = %f \n", result_fst.getFloatValue());
	}
}

void test_basic_sr_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cSR1Str = (char *)"sr[1]" ;
    char * cSR2Str = (char *)"sr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"sr", keyVar))
	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("SR Add: result_fst = %s \n", result_fst.getStringValue().c_str());
	}
}

void test_sr2r_combine_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cSR1Str = (char *)"sr[1]" ;
    char * cR2Str = (char *)"r[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"sr", keyVar))
	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("SR Add: result_fst = %s \n", result_fst.getStringValue().c_str());
	//	}
	}
}

void test_r2sr_combine_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cR1Str = (char *)"r[1]" ;
    char * cSR2Str = (char *)"sr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;
	
	thread_control_block objLocalThreadCntrolBlock;

	// R Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"r", keyVar))
	{
	//	if(strchr(cR1Str, '['))
	//	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("R Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcSubtract(&result_snd);
		FST_INFO("R Sub: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcMultiply(&result_snd);
		FST_INFO("R Mul: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcDivide(&result_snd);
		FST_INFO("R Div: result_fst = %f \n", result_fst.getFloatValue());
	}
}

void test_sr2mr_combine_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cSR1Str = (char *)"sr[1]" ;
    char * cMR2Str = (char *)"mr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"sr", keyVar))
	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("SR Add: result_fst = %s \n", result_fst.getStringValue().c_str());
	//	}
	}
}

void test_mr2sr_combine_calculation()
{
    int iRet = 0;
	key_variable keyVar ;
    char * cMR1Str = (char *)"mr[1]" ;
    char * cSR2Str = (char *)"sr[2]" ;
	
    eval_value result_fst ;
    eval_value result_snd ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"mr", keyVar))
	{
	//	if(strchr(cMR1Str, '['))
	//	{
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcAdd(&result_snd);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcSubtract(&result_snd);
		FST_INFO("MR Sub: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcMultiply(&result_snd);
		FST_INFO("MR Mul: result_fst = %f \n", result_fst.getFloatValue());

		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMR1Str, keyVar, &result_fst);
		result_snd.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSR2Str, keyVar, &result_snd);
		result_fst.calcDivide(&result_snd);
		FST_INFO("MR Div: result_fst = %f \n", result_fst.getFloatValue());
		
	//	}
	}
}

bool isEqual(float fFst, float fSnd, double valve = 0.001) 
{
    if(fabs(fFst - fSnd) < valve)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void test_basic_internal_func()
{
    eval_value result;
	char temp[NUM_OF_PARAMS][LAB_LEN];
	int iCount = get_internal_func_count();
	
	//	000 	(char *)"rand", 	  0, call_rand ,
	result.resetNoneValue();
	call_internal_func(0, &result);
	FST_INFO("FUNC         rand: result = %f ", result.getFloatValue());
	
	//	001 	(char *)"sin",		  1, call_sin  ,
	result.resetNoneValue();
	call_internal_func(1, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), sin(5)))
		FST_INFO("FUNC          sin: result = %f and cmath = %f "
			, result.getFloatValue(), sin(5));
	else 
		FST_ERROR("FUNC          sin: result = %f and cmath = %f "
			, result.getFloatValue(), sin(5));
	
	//	002 	(char *)"cos",		  1, call_cos  ,
	result.resetNoneValue();
	call_internal_func(2, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), cos(5)))
		FST_INFO("FUNC          cos: result = %f and cmath = %f "
			, result.getFloatValue(), cos(5));
	else 
		FST_ERROR("FUNC          cos: result = %f and cmath = %f "
			, result.getFloatValue(), cos(5));
	
	//	003 	(char *)"tan",		  1, call_tan  ,
	result.resetNoneValue();
	call_internal_func(3, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), tan(5)))
		FST_INFO("FUNC          tan: result = %f and cmath = %f "
			, result.getFloatValue(), tan(5));
	else 
		FST_ERROR("FUNC          tan: result = %f and cmath = %f "
			, result.getFloatValue(), tan(5));
	
	//	004 	(char *)"asin", 	  1, call_asin ,
	result.resetNoneValue();
	call_internal_func(4, &result, (char *)"0.5");
	if(isEqual(result.getFloatValue(), asin(0.5)))
		FST_INFO("FUNC         asin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(0.5));
	else 
		FST_ERROR("FUNC         asin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(0.5));
	
	//	005 	(char *)"arcsin",	  1, call_asin ,
	result.resetNoneValue();
	call_internal_func(5, &result, (char *)"0.5");
	if(isEqual(result.getFloatValue(), asin(0.5)))
		FST_INFO("FUNC       arcsin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(0.5));
	else 
		FST_ERROR("FUNC       arcsin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(0.5));
	
	//	006 	(char *)"acos", 	  1, call_acos ,
	result.resetNoneValue();
	call_internal_func(6, &result, (char *)"0.5");
	if(isEqual(result.getFloatValue(), acos(0.5)))
		FST_INFO("FUNC         acos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(0.5));
	else 
		FST_ERROR("FUNC         acos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(0.5));
	
	//	007 	(char *)"arccos",	  1, call_acos ,
	result.resetNoneValue();
	call_internal_func(7, &result, (char *)"0.5");
	if(isEqual(result.getFloatValue(), acos(0.5)))
		FST_INFO("FUNC       arccos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(0.5));
	else 
		FST_ERROR("FUNC       arccos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(0.5));
	
	//	008 	(char *)"atan", 	  1, call_atan ,
	result.resetNoneValue();
	call_internal_func(8, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), atan(5)))
		FST_INFO("FUNC         atan: result = %f and cmath = %f "
			, result.getFloatValue(), atan(5));
	else 
		FST_ERROR("FUNC         atan: result = %f and cmath = %f "
			, result.getFloatValue(), atan(5));
	
	//	009 	(char *)"arctan",	  1, call_atan ,
	result.resetNoneValue();
	call_internal_func(9, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), atan(5)))
		FST_INFO("FUNC       arctan: result = %f and cmath = %f "
			, result.getFloatValue(), atan(5));
	else 
		FST_ERROR("FUNC      arctan: result = %f and cmath = %f "
			, result.getFloatValue(), atan(5));
	
	//	010 	(char *)"atan2",	  2, call_atan2,
	result.resetNoneValue();
	call_internal_func(10, &result, (char *)"5", (char *)"6");
	if(isEqual(result.getFloatValue(), atan2(5, 6)))
		FST_INFO("FUNC        atan2: result = %f and cmath = %f "
			, result.getFloatValue(), atan2(5, 6));
	else 
		FST_ERROR("FUNC        atan2: result = %f and cmath = %f "
			, result.getFloatValue(), atan2(5, 6));
	
	//	011 	(char *)"sinh", 	  1, call_sinh ,
	result.resetNoneValue();
	call_internal_func(11, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), sinh(5)))
		FST_INFO("FUNC         sinh: result = %f and cmath = %f "
			, result.getFloatValue(), sinh(5));
	else 
		FST_ERROR("FUNC         sinh: result = %f and cmath = %f "
			, result.getFloatValue(), sinh(5));
	
	//	012 	(char *)"cosh", 	  1, call_cosh ,
	result.resetNoneValue();
	call_internal_func(12, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), cosh(5)))
		FST_INFO("FUNC         cosh: result = %f and cmath = %f "
			, result.getFloatValue(), cosh(5));
	else 
		FST_ERROR("FUNC         cosh: result = %f and cmath = %f "
			, result.getFloatValue(), cosh(5));
	
	//	013 	(char *)"tanh", 	  1, call_tanh ,
	result.resetNoneValue();
	call_internal_func(13, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), tanh(5)))
		FST_INFO("FUNC         tanh: result = %f and cmath = %f "
			, result.getFloatValue(), tanh(5));
	else 
		FST_ERROR("FUNC         tanh: result = %f and cmath = %f "
			, result.getFloatValue(), tanh(5));
	
	//	014 	(char *)"exp",		  1, call_exp  ,
	result.resetNoneValue();
	call_internal_func(14, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), exp(5)))
		FST_INFO("FUNC          exp: result = %f and cmath = %f "
			, result.getFloatValue(), exp(5));
	else 
		FST_ERROR("FUNC          exp: result = %f and cmath = %f "
			, result.getFloatValue(), exp(5));
	
	//	015 	(char *)"pow",		  2, call_pow  ,
	result.resetNoneValue();
	call_internal_func(15, &result, (char *)"5", (char *)"3");  
	if(isEqual(result.getFloatValue(), pow(5, 3)))
		FST_INFO("FUNC          pow: result = %f and cmath = %f "
			, result.getFloatValue(), pow(5, 3));
	else 
		FST_ERROR("FUNC          pow: result = %f and cmath = %f "
			, result.getFloatValue(), pow(5, 3));
	
	//	016 	(char *)"sqrt", 	  1, call_sqrt ,
	result.resetNoneValue();
	call_internal_func(16, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), sqrt(5)))
		FST_INFO("FUNC         sqrt: result = %f and cmath = %f "
			, result.getFloatValue(), sqrt(5));
	else 
		FST_ERROR("FUNC         sqrt: result = %f and cmath = %f "
			, result.getFloatValue(), sqrt(5));
	
	//	017 	(char *)"log",		  1, call_log  ,
	result.resetNoneValue();
	call_internal_func(17, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), log(5)))
		FST_INFO("FUNC          log: result = %f and cmath = %f "
			, result.getFloatValue(), log(5));
	else 
		FST_ERROR("FUNC          log: result = %f and cmath = %f "
			, result.getFloatValue(), log(5));
	
	//	018 	(char *)"log10",	  1, call_log10,
	result.resetNoneValue();
	call_internal_func(18, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), log10(5)))
		FST_INFO("FUNC        log10: result = %f and cmath = %f "
			, result.getFloatValue(), log10(5));
	else 
		FST_ERROR("FUNC        log10: result = %f and cmath = %f "
			, result.getFloatValue(), log10(5));
	
	//	019 	(char *)"ceil", 	  1, call_ceil ,
	result.resetNoneValue();
	call_internal_func(19, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), ceil(5)))
		FST_INFO("FUNC         ceil: result = %f and cmath = %f "
			, result.getFloatValue(), ceil(5));
	else 
		FST_ERROR("FUNC         ceil: result = %f and cmath = %f "
			, result.getFloatValue(), ceil(5));
	
	//	020 	(char *)"floor",	  1, call_floor,
	result.resetNoneValue();
	call_internal_func(20, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), floor(5)))
		FST_INFO("FUNC        floor: result = %f and cmath = %f "
			, result.getFloatValue(), floor(5));
	else 
		FST_ERROR("FUNC        floor: result = %f and cmath = %f "
			, result.getFloatValue(), floor(5));
	
	//	021 	(char *)"fabs", 	  1, call_fabs ,
	result.resetNoneValue();
	call_internal_func(21, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), fabs(5)))
		FST_INFO("FUNC         fabs: result = %f and cmath = %f "
			, result.getFloatValue(), fabs(5));
	else 
		FST_ERROR("FUNC         fabs: result = %f and cmath = %f "
			, result.getFloatValue(), fabs(5));
	
	//	022 	(char *)"abs",		  1, call_fabs ,
	result.resetNoneValue();
	call_internal_func(22, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), fabs(5)))
		FST_INFO("FUNC          abs: result = %f and cmath = %f "
			, result.getFloatValue(), fabs(5));
	else 
		FST_ERROR("FUNC          abs: result = %f and cmath = %f "
			, result.getFloatValue(), fabs(5));
	
	//		//	(char *)"frexp",	  2, call_frexp,
	//	023 	(char *)"ldexp",	  2, call_ldexp,
	result.resetNoneValue();
	call_internal_func(23, &result, (char *)"5", (char *)"5");
	if(isEqual(result.getFloatValue(), ldexp(5, 5)))
		FST_INFO("FUNC        ldexp: result = %f and cmath = %f "
			, result.getFloatValue(), ldexp(5, 5));
	else 
		FST_ERROR("FUNC        ldexp: result = %f and cmath = %f "
			, result.getFloatValue(), ldexp(5, 5));
	
	//	024 	(char *)"modf", 	  1, call_modf ,
	result.resetNoneValue();
	call_internal_func(24, &result, (char *)"5");
	double valTwo ;
	if(isEqual(result.getFloatValue(), (float)modf(5, &valTwo)))
		FST_INFO("FUNC         modf: result = %f and cmath = %f "
			, result.getFloatValue(), modf(5, &valTwo));
	else 
		FST_ERROR("FUNC         modf: result = %f and cmath = %f "
			, result.getFloatValue(), modf(5, &valTwo));
	
	//	025 	(char *)"fmod", 	  2, call_fmod ,
	result.resetNoneValue();
	call_internal_func(25, &result, (char *)"5", (char *)"5");
	if(isEqual(result.getFloatValue(), fmod(5, 5)))
		FST_INFO("FUNC         fmod: result = %f and cmath = %f "
			, result.getFloatValue(), fmod(5, 5));
	else 
		FST_ERROR("FUNC         fmod: result = %f and cmath = %f "
			, result.getFloatValue(), fmod(5, 5));
	
	//	026 	(char *)"hypot",	  2, call_hypot,
	result.resetNoneValue();
	call_internal_func(26, &result, (char *)"5", (char *)"5");
	if(isEqual(result.getFloatValue(), hypot(5, 5)))
		FST_INFO("FUNC        hypot: result = %f and cmath = %f "
			, result.getFloatValue(), hypot(5, 5));
	else 
		FST_ERROR("FUNC        hypot: result = %f and cmath = %f "
			, result.getFloatValue(), hypot(5, 5));
	
	//	027 	(char *)"gcd",		  2, call_gcd,
	result.resetNoneValue();                                       
	call_internal_func(27, &result, (char *)"5", (char *)"5");
	if(isEqual(result.getFloatValue(), Stein_GCD(5, 5)))
		FST_INFO("FUNC          gcd: result = %f and cmath = %f "
			, result.getFloatValue(), Stein_GCD(5, 5));
	else 
		FST_ERROR("FUNC          gcd: result = %f and cmath = %f "
			, result.getFloatValue(), Stein_GCD(5, 5));
	
	//	028 	(char *)"lcm",		  2, call_lcm,
	result.resetNoneValue();
	call_internal_func(28, &result, (char *)"150", (char *)"5");
	if(isEqual(result.getFloatValue(), 150 * 5 / Stein_GCD(150, 5)))
		FST_INFO("FUNC          lcm: result = %f and cmath = %f "
			, result.getFloatValue(), 150 * 5 / Stein_GCD(150, 5));
	else 
		FST_ERROR("FUNC          lcm: result = %f and cmath = %f "
			, result.getFloatValue(), 150 * 5 / Stein_GCD(150, 5));
	
	//			// Convert function
	//	029 	(char *)"degrees",	  1, call_degrees,
	result.resetNoneValue();
	call_internal_func(29, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), RAD2DEG(5)))
		FST_INFO("FUNC      degrees: result = %f and cmath = %f "
			, result.getFloatValue(), RAD2DEG(5));
	else 
		FST_ERROR("FUNC      degrees: result = %f and cmath = %f "
			, result.getFloatValue(), RAD2DEG(5));
	
	//	030 	(char *)"radians",	  1, call_radians,
	result.resetNoneValue();
	call_internal_func(30, &result, (char *)"5");
	if(isEqual(result.getFloatValue(), DEG2RAD(5)))
		FST_INFO("FUNC      radians: result = %f and cmath = %f "
			, result.getFloatValue(), DEG2RAD(5));
	else 
		FST_ERROR("FUNC      radians: result = %f and cmath = %f "
			, result.getFloatValue(), DEG2RAD(5));
	
	//			// Convert function
	//	031 	(char *)"isleapyear", 1, call_isleapyear,
	result.resetNoneValue();
	call_internal_func(31, &result, (char *)"1990");
	if((int)result.getFloatValue() == IsLeapYear(1990))
		FST_INFO("FUNC   isleapyear: result = %f and cmath = %d "
			, result.getFloatValue(), IsLeapYear(1990));
	else 
		FST_ERROR("FUNC   isleapyear: result = %f and cmath = %d "
			, result.getFloatValue(), IsLeapYear(1990));
	
	//	032 	(char *)"getmaxday",  2, call_getmaxday,
	result.resetNoneValue();
	call_internal_func(32, &result, (char *)"1980", (char *)"5");
	if((int)result.getFloatValue() == GetMaxDay(1980, 5))
		FST_INFO("FUNC    getmaxday: result = %f and cmath = %d "
			, result.getFloatValue(), GetMaxDay(1980, 5));
	else 
		FST_ERROR("FUNC    getmaxday: result = %f and cmath = %d "
			, result.getFloatValue(), GetMaxDay(1980, 5));
	
	//	033 	(char *)"getdays",	  3, call_getdays,
	result.resetNoneValue();
	call_internal_func(33, &result, (char *)"1980", (char *)"5", (char *)"5");
	if((int)result.getFloatValue() == GetDays(1980, 5, 5))
		FST_INFO("FUNC      getdays: result = %f and cmath = %d "
			, result.getFloatValue(), GetDays(1980, 5, 5));
	else 
		FST_ERROR("FUNC      getdays: result = %f and cmath = %d "
			, result.getFloatValue(), GetDays(1980, 5, 5));
	
	//			// String function
	//	034 	(char *)"strlen",	  1, call_strlen ,
	result.resetNoneValue();
	call_internal_func(34, &result, (char *)"123456789");
	if((int)result.getFloatValue() == strlen("123456789"))
		FST_INFO("FUNC       strlen: result = %f and cmath = %d "
			, result.getFloatValue(), strlen("123456789"));
	else 
		FST_ERROR("FUNC       strlen: result = %f and cmath = %d "
			, result.getFloatValue(), strlen("123456789"));
	
	//	035 	(char *)"findstr",	  2, call_findstr ,
	result.resetNoneValue();
	char cString[32];
	memset(cString, 0x00, 32); strcpy(cString, "123456789");
	call_internal_func(35, &result, cString, (char *)"456");
	char * cRet = strstr(cString, (char *)"456");
	if(cRet != NULL)
	{
		FST_INFO("FUNC      findstr: result = %f and cmath = %d "
			, result.getFloatValue(), (int)(cRet - cString));
	}
	else 
	{
		FST_ERROR("FUNC      findstr: result = %f and cmath = NG "
			, result.getFloatValue());
	}
	
	//	036 	(char *)"substr",	  3, call_substr ,
	result.resetNoneValue();
	call_internal_func(36, &result, (char *)"123456789", (char *)"1", (char *)"2");
	string strRet = string((char *)"123456789").substr(1, 2);
	if(result.getStringValue() == strRet)
		FST_INFO("FUNC       substr: result = %s and cmath = %s "
			, result.getStringValue().c_str(), strRet.c_str());
	else 
		FST_ERROR("FUNC       substr: result = %s and cmath = %s "
			, result.getStringValue().c_str(), strRet.c_str());
	
	//	037 	(char *)"replace",	  3, call_replace ,
	result.resetNoneValue();
	call_internal_func(37, &result, (char *)"123456789", (char *)"1", (char *)"2");
	FST_INFO("FUNC      replace: result = %s ", result.getStringValue().c_str());
	
	//	038 	(char *)"replaceall", 3, call_replaceall ,
	result.resetNoneValue();
	call_internal_func(38, &result, (char *)"123456789", (char *)"1", (char *)"2");
	FST_INFO("FUNC   replaceall: result = %s ", result.getStringValue().c_str());
	
	//	039 	(char *)"replacealliteration", 
	//								  3, call_replaceall_iteration ,
	result.resetNoneValue();
	call_internal_func(39, &result, (char *)"123456789", (char *)"1", (char *)"2");
	FST_INFO("FUNC replceallitt: result = %s ", result.getStringValue().c_str());
	
	//	040 	(char *)"lower",	  1, call_lower ,
	result.resetNoneValue();
	call_internal_func(40, &result, (char *)"AbcJhHGgFFdDEFGijk");
	FST_INFO("FUNC        lower: result = %s ", result.getStringValue().c_str());
	
	//	041 	(char *)"upper",	  1, call_upper ,
	result.resetNoneValue();
	call_internal_func(41, &result, (char *)"AbcJhHGgFFdDEFGijk");
	FST_INFO("FUNC        upper: result = %s ", result.getStringValue().c_str());
	
	//	042 	(char *)"revert",	  1, call_revert ,
	result.resetNoneValue();
	call_internal_func(42, &result, (char *)"AbcJhHGgFFdDEFGijk");
	FST_INFO("FUNC       revert: result = %s ", result.getStringValue().c_str());
	
	//			// String convert function of <stdlib.h>
	//	043 	(char *)"atoi", 	 1, call_atoi ,
	result.resetNoneValue();
	call_internal_func(43, &result, (char *)"123.456");
	if((int)result.getFloatValue() == 123)
		FST_INFO("FUNC         atoi: result = %f ", result.getFloatValue());
	else 
		FST_ERROR("FUNC         atoi: result = %f ", result.getFloatValue());
	
	//	044 	(char *)"atof", 	 1, call_atof ,
	result.resetNoneValue();
	call_internal_func(44, &result, (char *)"123.456");
	if(isEqual(result.getFloatValue(), 123.456))
		FST_INFO("FUNC         atof: result = %f ", result.getFloatValue());
	else 
		FST_ERROR("FUNC         atof: result = %f ", result.getFloatValue());
	
	//	045 	(char *)"itoa", 	 1, call_itoa ,
	result.resetNoneValue();
	call_internal_func(45, &result, (char *)"123.456");
	if(result.getStringValue() == (char *)"123")
		FST_INFO("FUNC         itoa: result = %s ", result.getStringValue().c_str());
	else 
		FST_ERROR("FUNC         itoa: result = %s ", result.getStringValue().c_str());
	
	//	046 	(char *)"ftoa", 	 1, call_ftoa ,
	result.resetNoneValue();
	call_internal_func(46, &result, (char *)"123.456");
	if(result.getStringValue() == (char *)"123.456")
		FST_INFO("FUNC         ftoa: result = %s ", result.getStringValue().c_str());
	else 
		FST_ERROR("FUNC         ftoa: result = %s ", result.getStringValue().c_str());
}

void test_overflow_internal_func()
{
    eval_value result;
	char temp[NUM_OF_PARAMS][LAB_LEN];
	int iCount = get_internal_func_count();
	
	//	000 	(char *)"rand", 	  0, call_rand ,
	//	001 	(char *)"sin",		  1, call_sin  ,
	//	002 	(char *)"cos",		  1, call_cos  ,
	
	//	003 	(char *)"tan",		  1, call_tan  ,
	result.resetNoneValue();
	call_internal_func(3, &result, (char *)"1.5707963267948966192313216916398");
	if(isEqual(result.getFloatValue(), tan(1.5707963267948966192313216916398)))
		FST_INFO("FUNC          tan: result = %f and cmath = %f "
			, result.getFloatValue(), tan(1.5707963267948966192313216916398));
	else 
		FST_ERROR("FUNC          tan: result = %f and cmath = %f "
			, result.getFloatValue(), tan(5));
	
	//	004 	(char *)"asin", 	  1, call_asin ,
	result.resetNoneValue();
	call_internal_func(4, &result, (char *)"10");
	if(isEqual(result.getFloatValue(), asin(10)))
		FST_INFO("FUNC         asin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(10));
	else 
		FST_ERROR("FUNC         asin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(10));
	
	//	005 	(char *)"arcsin",	  1, call_asin ,
	result.resetNoneValue();
	call_internal_func(5, &result, (char *)"10");
	if(isEqual(result.getFloatValue(), asin(10)))
		FST_INFO("FUNC       arcsin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(10));
	else 
		FST_ERROR("FUNC       arcsin: result = %f and cmath = %f "
			, result.getFloatValue(), asin(10));
	
	//	006 	(char *)"acos", 	  1, call_acos ,
	result.resetNoneValue();
	call_internal_func(6, &result, (char *)"10");
	if(isEqual(result.getFloatValue(), acos(10)))
		FST_INFO("FUNC         acos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(10));
	else 
		FST_ERROR("FUNC         acos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(10));
	
	//	007 	(char *)"arccos",	  1, call_acos ,
	result.resetNoneValue();
	call_internal_func(7, &result, (char *)"10");
	if(isEqual(result.getFloatValue(), acos(10)))
		FST_INFO("FUNC       arccos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(10));
	else 
		FST_ERROR("FUNC       arccos: result = %f and cmath = %f "
			, result.getFloatValue(), acos(10));
	
	//	008 	(char *)"atan", 	  1, call_atan ,
	
	//	009 	(char *)"arctan",	  1, call_atan ,
	
	//	010 	(char *)"atan2",	  2, call_atan2,
	
	//	011 	(char *)"sinh", 	  1, call_sinh ,
	
	//	012 	(char *)"cosh", 	  1, call_cosh ,
	
	//	013 	(char *)"tanh", 	  1, call_tanh ,
	
	//	014 	(char *)"exp",		  1, call_exp  ,
	
	//	015 	(char *)"pow",		  2, call_pow  ,
	result.resetNoneValue();
	call_internal_func(15, &result, (char *)"5", (char *)"3");  
	if(isEqual(result.getFloatValue(), pow(-5, 3)))
		FST_INFO("FUNC          pow: result = %f and cmath = %f "
			, result.getFloatValue(), pow(-5, 3));
	else 
		FST_ERROR("FUNC          pow: result = %f and cmath = %f "
			, result.getFloatValue(), pow(-5, 3));
	
	//	016 	(char *)"sqrt", 	  1, call_sqrt ,
	result.resetNoneValue();
	call_internal_func(16, &result, (char *)"-5");
	if(isEqual(result.getFloatValue(), sqrt(-5)))
		FST_INFO("FUNC         sqrt: result = %f and cmath = %f "
			, result.getFloatValue(), sqrt(-5));
	else 
		FST_ERROR("FUNC         sqrt: result = %f and cmath = %f "
			, result.getFloatValue(), sqrt(5));
	
	//	017 	(char *)"log",		  1, call_log  ,
	result.resetNoneValue();
	call_internal_func(17, &result, (char *)"-5");
	if(isEqual(result.getFloatValue(), log(-5)))
		FST_INFO("FUNC          log: result = %f and cmath = %f "
			, result.getFloatValue(), log(-5));
	else 
		FST_ERROR("FUNC          log: result = %f and cmath = %f "
			, result.getFloatValue(), log(-5));
	
	//	018 	(char *)"log10",	  1, call_log10,
	result.resetNoneValue();
	call_internal_func(18, &result, (char *)"-5");
	if(isEqual(result.getFloatValue(), log10(-5)))
		FST_INFO("FUNC        log10: result = %f and cmath = %f "
			, result.getFloatValue(), log10(-5));
	else 
		FST_ERROR("FUNC        log10: result = %f and cmath = %f "
			, result.getFloatValue(), log10(-5));
	
	//	019 	(char *)"ceil", 	  1, call_ceil ,
	
	//	020 	(char *)"floor",	  1, call_floor,
	
	//	021 	(char *)"fabs", 	  1, call_fabs ,
	
	//	022 	(char *)"abs",		  1, call_fabs ,
	
	//		//	(char *)"frexp",	  2, call_frexp,
	//	023 	(char *)"ldexp",	  2, call_ldexp,
	
	//	024 	(char *)"modf", 	  1, call_modf ,
	
	//	025 	(char *)"fmod", 	  2, call_fmod ,
	
	//	026 	(char *)"hypot",	  2, call_hypot,
	
	//	027 	(char *)"gcd",		  2, call_gcd,
	result.resetNoneValue();                                       
	call_internal_func(27, &result, (char *)"5", (char *)"5");
	if(isEqual(result.getFloatValue(), Stein_GCD(-5, 5)))
		FST_INFO("FUNC          gcd: result = %f and cmath = %f "
			, result.getFloatValue(), Stein_GCD(-5, 5));
	else 
		FST_ERROR("FUNC          gcd: result = %f and cmath = %f "
			, result.getFloatValue(), Stein_GCD(5, 5));
	
	//	028 	(char *)"lcm",		  2, call_lcm,
	result.resetNoneValue();
	call_internal_func(28, &result, (char *)"150", (char *)"5");
	if(isEqual(result.getFloatValue(), 150 * 5 / Stein_GCD(-150, 5)))
		FST_INFO("FUNC          lcm: result = %f and cmath = %f "
			, result.getFloatValue(), 150 * 5 / Stein_GCD(-150, 5));
	else 
		FST_ERROR("FUNC          lcm: result = %f and cmath = %f "
			, result.getFloatValue(), 150 * 5 / Stein_GCD(-150, 5));
	
	//			// Convert function
	//	029 	(char *)"degrees",	  1, call_degrees,
	
	//	030 	(char *)"radians",	  1, call_radians,
	
	//			// Convert function
	//	031 	(char *)"isleapyear", 1, call_isleapyear,
	result.resetNoneValue();
	call_internal_func(31, &result, (char *)"-1990");
	if((int)result.getFloatValue() == IsLeapYear(-1990))
		FST_INFO("FUNC   isleapyear: result = %f and cmath = %d "
			, result.getFloatValue(), IsLeapYear(-1990));
	else 
		FST_ERROR("FUNC   isleapyear: result = %f and cmath = %d "
			, result.getFloatValue(), IsLeapYear(-1990));
	
	//	032 	(char *)"getmaxday",  2, call_getmaxday,
	result.resetNoneValue();
	call_internal_func(32, &result, (char *)"-1980", (char *)"5");
	if((int)result.getFloatValue() == GetMaxDay(-1980, 5))
		FST_INFO("FUNC    getmaxday: result = %f and cmath = %d "
			, result.getFloatValue(), GetMaxDay(-1980, 5));
	else 
		FST_ERROR("FUNC    getmaxday: result = %f and cmath = %d "
			, result.getFloatValue(), GetMaxDay(-1980, 5));
	
	//	033 	(char *)"getdays",	  3, call_getdays,
	result.resetNoneValue();
	call_internal_func(33, &result, (char *)"-1980", (char *)"5", (char *)"5");
	if((int)result.getFloatValue() == GetDays(-1980, 5, 5))
		FST_INFO("FUNC      getdays: result = %f and cmath = %d "
			, result.getFloatValue(), GetDays(-1980, 5, 5));
	else 
		FST_ERROR("FUNC      getdays: result = %f and cmath = %d "
			, result.getFloatValue(), GetDays(-1980, 5, 5));
	
	//			// String function
	//	034 	(char *)"strlen",	  1, call_strlen ,
	
	//	035 	(char *)"findstr",	  2, call_findstr ,
	result.resetNoneValue();
	char cString[32];
	memset(cString, 0x00, 32); strcpy(cString, "123456789");
	call_internal_func(35, &result, cString, (char *)"FFF");
	char * cRet = strstr(cString, (char *)"FFF");
	if(cRet != NULL)
	{
		FST_INFO("FUNC      findstr: result = %f and cmath = %d "
			, result.getFloatValue(), (int)(cRet - cString));
	}
	else 
	{
		FST_ERROR("FUNC      findstr: result = %f and cmath = NG "
			, result.getFloatValue());
	}
	
	//	036 	(char *)"substr",	  3, call_substr ,
	result.resetNoneValue();
	call_internal_func(36, &result, (char *)"123456789", (char *)"-11", (char *)"2");
	FST_INFO("FUNC       substr: result = %s "
		, result.getStringValue().c_str());
	
	//	037 	(char *)"replace",	  3, call_replace ,
	result.resetNoneValue();
	call_internal_func(37, &result, (char *)"123456789", (char *)"FF", (char *)"2");
	FST_INFO("FUNC      replace: result = %s ", result.getStringValue().c_str());
	
	//	038 	(char *)"replaceall", 3, call_replaceall ,
	result.resetNoneValue();
	call_internal_func(38, &result, (char *)"123456789", (char *)"FF", (char *)"2");
	FST_INFO("FUNC   replaceall: result = %s ", result.getStringValue().c_str());
	
	//	039 	(char *)"replacealliteration", 
	//								  3, call_replaceall_iteration ,
	result.resetNoneValue();
	call_internal_func(39, &result, (char *)"123456789", (char *)"FF", (char *)"2");
	FST_INFO("FUNC replceallitt: result = %s ", result.getStringValue().c_str());
	
	//	040 	(char *)"lower",	  1, call_lower ,
	
	//	041 	(char *)"upper",	  1, call_upper ,
	
	//	042 	(char *)"revert",	  1, call_revert ,
	result.resetNoneValue();
	call_internal_func(42, &result, (char *)"");
	FST_INFO("FUNC       revert: result = %s ", result.getStringValue().c_str());
	
	//			// String convert function of <stdlib.h>
	//	043 	(char *)"atoi", 	 1, call_atoi ,
	result.resetNoneValue();
	call_internal_func(43, &result, (char *)"abcd");
	FST_INFO("FUNC         atoi: result = %f ", result.getFloatValue());
	
	//	044 	(char *)"atof", 	 1, call_atof ,
	result.resetNoneValue();
	call_internal_func(44, &result, (char *)"abcd");
	FST_INFO("FUNC         atof: result = %f ", result.getFloatValue());
	
	//	045 	(char *)"itoa", 	 1, call_itoa ,
	result.resetNoneValue();
	call_internal_func(45, &result, (char *)"abcd");
	if(result.getStringValue() == (char *)"abcd")
		FST_INFO("FUNC         itoa: result = %s ", result.getStringValue().c_str());
	else 
		FST_ERROR("FUNC         itoa: result = %s ", result.getStringValue().c_str());
	
	//	046 	(char *)"ftoa", 	 1, call_ftoa ,
	result.resetNoneValue();
	call_internal_func(46, &result, (char *)"abcd");
	if(result.getStringValue() == (char *)"abcd")
		FST_INFO("FUNC         ftoa: result = %s ", result.getStringValue().c_str());
	else 
		FST_ERROR("FUNC         ftoa: result = %s ", result.getStringValue().c_str());
}

void test_basic_pr_register(int index)
{
	char cPRStr[1024];

	memset(cPRStr, 0x00, 1204);
	sprintf(cPRStr, "pr[%d]", index); ;

	eval_value result_fst ;

	thread_control_block objLocalThreadCntrolBlock;

	Joint jointValFst = {1,2,3,4,5,6};
	Joint jointValSnd = {6,5,4,3,2,1};

	
	result_fst.resetNoneValue();
	result_fst.setJointValue(&jointValFst);
	forgesight_registers_manager_set_register(&objLocalThreadCntrolBlock, cPRStr, &result_fst);
	// PR Calculation
	result_fst.resetNoneValue();
	forgesight_registers_manager_get_register(&objLocalThreadCntrolBlock, cPRStr, &result_fst);
	
	if(result_fst.hasType(TYPE_POSE) == TYPE_POSE)
	{
#ifndef WIN32
	    FST_INFO("PR Get :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().point_.x_, result_fst.getPoseValue().point_.y_, 
			result_fst.getPoseValue().point_.z_, result_fst.getPoseValue().euler_.a_, 
			result_fst.getPoseValue().euler_.b_, result_fst.getPoseValue().euler_.c_);
#else
	    FST_INFO("PR Get :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().position.x,    result_fst.getPoseValue().position.y, 
			result_fst.getPoseValue().position.z,    result_fst.getPoseValue().orientation.a, 
			result_fst.getPoseValue().orientation.b, result_fst.getPoseValue().orientation.c);
#endif
	}
	else if(result_fst.hasType(TYPE_JOINT) == TYPE_JOINT)
	{
	    FST_INFO("PR Get :(%f, %f, %f, %f, %f, %f) in MovJ", 
#ifndef WIN32
			result_fst.getJointValue().j1_, result_fst.getJointValue().j2_, 
			result_fst.getJointValue().j3_, result_fst.getJointValue().j4_, 
			result_fst.getJointValue().j5_, result_fst.getJointValue().j6_);
#else
			result_fst.getJointValue().j1, result_fst.getJointValue().j2, 
			result_fst.getJointValue().j3, result_fst.getJointValue().j4, 
			result_fst.getJointValue().j5, result_fst.getJointValue().j6);
#endif		
	}
	
	result_fst.resetNoneValue();
	result_fst.setJointValue(&jointValSnd);
	forgesight_registers_manager_set_register(&objLocalThreadCntrolBlock, cPRStr, &result_fst);
	// PR Calculation
	result_fst.resetNoneValue();
	forgesight_registers_manager_get_register(&objLocalThreadCntrolBlock, cPRStr, &result_fst);
	
	if(result_fst.hasType(TYPE_POSE) == TYPE_POSE)
	{
#ifndef WIN32
		FST_INFO("PR Get :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().point_.x_, result_fst.getPoseValue().point_.y_, 
			result_fst.getPoseValue().point_.z_, result_fst.getPoseValue().euler_.a_, 
			result_fst.getPoseValue().euler_.b_, result_fst.getPoseValue().euler_.c_);
#else
		FST_INFO("PR Get :(%f, %f, %f, %f, %f, %f) in MovL", 
			result_fst.getPoseValue().position.x,	 result_fst.getPoseValue().position.y, 
			result_fst.getPoseValue().position.z,	 result_fst.getPoseValue().orientation.a, 
			result_fst.getPoseValue().orientation.b, result_fst.getPoseValue().orientation.c);
#endif
	}
	else if(result_fst.hasType(TYPE_JOINT) == TYPE_JOINT)
	{
		FST_INFO("PR Get :(%f, %f, %f, %f, %f, %f) in MovJ", 
#ifndef WIN32
			result_fst.getJointValue().j1_, result_fst.getJointValue().j2_, 
			result_fst.getJointValue().j3_, result_fst.getJointValue().j4_, 
			result_fst.getJointValue().j5_, result_fst.getJointValue().j6_);
#else
			result_fst.getJointValue().j1, result_fst.getJointValue().j2, 
			result_fst.getJointValue().j3, result_fst.getJointValue().j4, 
			result_fst.getJointValue().j5, result_fst.getJointValue().j6);
#endif		
	}
}

void test_basic_r_register(int index)
{
    int iRet = 0;
	key_variable keyVar ;
    char cRStr[1024];

	memset(cRStr, 0x00, 1204);
	sprintf(cRStr, "r[%d]", index);
	
    eval_value result_fst ;
	
	thread_control_block objLocalThreadCntrolBlock;

	// R Calculation
	if(forgesight_find_external_resource((char *)"r", keyVar))
	{
		result_fst.resetNoneValue();
		result_fst.setFloatValue(123.456);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cRStr, keyVar, &result_fst);
		// PR Calculation
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cRStr, keyVar, &result_fst);
		FST_INFO("R Get: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		result_fst.setFloatValue(456.123);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cRStr, keyVar, &result_fst);
		// PR Calculation
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cRStr, keyVar, &result_fst);
		FST_INFO("R Get: result_fst = %f \n", result_fst.getFloatValue());
	}
}

void test_basic_mr_register(int index)
{
    int iRet = 0;
	key_variable keyVar ;
    char cMRStr[1024];

	memset(cMRStr, 0x00, 1204);
	sprintf(cMRStr, "mr[%d]", index); 
	
    eval_value result_fst ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"mr", keyVar))
	{
		result_fst.resetNoneValue();
		result_fst.setFloatValue(123.456);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cMRStr, keyVar, &result_fst);
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMRStr, keyVar, &result_fst);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
		
		result_fst.resetNoneValue();
		result_fst.setFloatValue(456.123);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cMRStr, keyVar, &result_fst);
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cMRStr, keyVar, &result_fst);
		FST_INFO("MR Add: result_fst = %f \n", result_fst.getFloatValue());
	}
}

void test_basic_sr_register(int index)
{
    int iRet = 0;
	key_variable keyVar ;
    char cSRStr[1024];

	memset(cSRStr, 0x00, 1204);
	sprintf(cSRStr, "sr[%d]", index); 

	string strTmp ;
	
    eval_value result_fst ;

	thread_control_block objLocalThreadCntrolBlock;
	
	// MR Calculation
	// External Type
	if(forgesight_find_external_resource((char *)"sr", keyVar))
	{
		result_fst.resetNoneValue();
		strTmp = string("12345");
		result_fst.setStringValue(strTmp);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cSRStr, keyVar, &result_fst);
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSRStr, keyVar, &result_fst);
		FST_INFO("SR Add: result_fst = %s \n", result_fst.getStringValue().c_str());
		
		result_fst.resetNoneValue();
		strTmp = string("54321");
		result_fst.setStringValue(strTmp);
		iRet = forgesight_registers_manager_set_resource(
			&objLocalThreadCntrolBlock, cSRStr, keyVar, &result_fst);
		result_fst.resetNoneValue();
		iRet = forgesight_registers_manager_get_resource(
			&objLocalThreadCntrolBlock, cSRStr, keyVar, &result_fst);
		FST_INFO("SR Add: result_fst = %s \n", result_fst.getStringValue().c_str());
		
	}
}

void test_basic_io_value(char * cInput1Str, char * cOutput1Str)
{
	thread_control_block objLocalThreadCntrolBlock;
	eval_value value ;
	
	value.resetNoneValue();
	value.setFloatValue(1);
	forgesight_set_io_status(&objLocalThreadCntrolBlock, cInput1Str, value);
	value.resetNoneValue();
	value = forgesight_get_io_status(&objLocalThreadCntrolBlock, cInput1Str);
	FST_INFO("DI Add: value = %f \n", value.getFloatValue());

	value.resetNoneValue();
	value.setFloatValue(0);
	forgesight_set_io_status(&objLocalThreadCntrolBlock, cInput1Str, value);
	value.resetNoneValue();
	value = forgesight_get_io_status(&objLocalThreadCntrolBlock, cInput1Str);
	FST_INFO("DI Add: value = %f \n", value.getFloatValue());

	value.resetNoneValue();
	value.setFloatValue(1);
	forgesight_set_io_status(&objLocalThreadCntrolBlock, cOutput1Str, value);
	value.resetNoneValue();
	value = forgesight_get_io_status(&objLocalThreadCntrolBlock, cOutput1Str);
	FST_INFO("DI Add: value = %f \n", value.getFloatValue());

	value.resetNoneValue();
	value.setFloatValue(0);
	forgesight_set_io_status(&objLocalThreadCntrolBlock, cOutput1Str, value);
	value.resetNoneValue();
	value = forgesight_get_io_status(&objLocalThreadCntrolBlock, cOutput1Str);
	FST_INFO("DI Add: value = %f \n", value.getFloatValue());
}



int main(int argc, char **argv)
{
	char cCase[128];
    eval_value result_fst ;
    eval_value result_snd ;
    eval_value result_thd ;
	
	thread_control_block objLocalThreadCntrolBlock;
		
	InterpreterControl intprt_ctrl; 
	memset(&intprt_ctrl, 0x00, sizeof(intprt_ctrl));
	
    if (argc < 2)
    {
        cout << "more parameters are needed" << endl;
        return -1;
    }
#ifndef WIN32
	signal(SIGINT, signalInterrupt);
	if(log_ptr_ == NULL)
	{
		log_ptr_ = new fst_log::Logger();
		FST_LOG_INIT("Interpreter");
	}
#else
	//	append_io_mapping();
	intprt_ctrl.cmd = fst_base::INTERPRETER_SERVER_CMD_LAUNCH ;
#endif

	memset(cCase, 0x00, 128);
	strcpy(cCase, argv[1]);
	
    initInterpreter();

	load_register_data();

	if((strcmp("all", cCase) == 0) || (strcmp("pr_calc", cCase) == 0))
	{
		test_basic_pr_calculation();
	}
 	if((strcmp("all", cCase) == 0) || (strcmp("r_calc", cCase) == 0))
	{
		test_basic_r_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("r_peak", cCase) == 0))
	{
		test_peak_r_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("mr_calc", cCase) == 0))
	{
		test_basic_mr_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("mr_peak", cCase) == 0))
	{
		test_peak_mr_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("r2mr_calc", cCase) == 0))
	{
		test_r2mr_combine_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("mr2r_calc", cCase) == 0))
	{
		test_mr2r_combine_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("sr", cCase) == 0))
	{
		test_basic_sr_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("sr2r_calc", cCase) == 0))
	{
		test_sr2r_combine_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("r2sr_calc", cCase) == 0))
	{
		test_r2sr_combine_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("sr2mr_calc", cCase) == 0))
	{
		test_sr2mr_combine_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("mr2sr_calc", cCase) == 0))
	{
		test_mr2sr_combine_calculation();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("innerfunc", cCase) == 0))
	{
		test_basic_internal_func();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("overflowfunc", cCase) == 0))
	{
		test_overflow_internal_func();
	}
	if((strcmp("all", cCase) == 0) || (strcmp("register", cCase) == 0))
	{
		if (argc < 3)
	    {
	        cout << "Please input index" << endl;
	        return -1;
	    }
		test_basic_pr_register(atoi(argv[2]));
		test_basic_r_register(atoi(argv[2]));
		test_basic_mr_register(atoi(argv[2]));
		test_basic_sr_register(atoi(argv[2]));
	}
	if((strcmp("all", cCase) == 0) || (strcmp("io", cCase) == 0))
	{
		test_basic_io_value((char *)"di[1]", (char *)"do[1]");
		test_basic_io_value((char *)"ri[1]", (char *)"ro[1]");
	}
	
    return 1;
}


