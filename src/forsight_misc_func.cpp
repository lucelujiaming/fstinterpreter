// #include "stdafx.h"
#include "stdio.h"
#include <string>
#include "forsight_misc_func.h"

void deal_substr_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result);
void deal_replace_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result);
void deal_repl_all_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result);


void deal_misc_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result)
{
	if(!strcmp(STR_SUBSTR_FUNC, objThreadCntrolBlock->token))
  	{
  		deal_substr_func(objThreadCntrolBlock, result);
  	}
	else if(!strcmp(STR_SUBSTR_FUNC, objThreadCntrolBlock->token))
  	{
  		deal_replace_func(objThreadCntrolBlock, result);
  	}
	else if(!strcmp(STR_REPLACEALL_FUNC, objThreadCntrolBlock->token))
  	{
  		deal_repl_all_func(objThreadCntrolBlock, result);
  	}
	else
  	{
  		serror(objThreadCntrolBlock, 18);
  	}
}

void deal_substr_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result)
{
    eval_value value;
    int boolValue;
	int count ;
	char temp[NUM_OF_PARAMS][LAB_LEN];
	
	string strOpt ;

    count = 0;
    get_token(objThreadCntrolBlock);
	// Fit for circumstance without parameter
	if((*(objThreadCntrolBlock->token) == '\r')
		|| (*(objThreadCntrolBlock->token) == '\n'))
		return ;
    if(*(objThreadCntrolBlock->token) != '(')
    {
    	serror(objThreadCntrolBlock, 2);
		return ;
	}

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
		if(count == 0)
		{
			strOpt = value.getStringValue();
		}
        sprintf(temp[count], "%f", value.getFloatValue()); // save temporarily
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    // count--;

    // Now, push on local_var_stack in reverse order.
    if(count == 3)
	{
		strOpt = strOpt.substr((int)atof(temp[1]), (int)atof(temp[2]));
		// call_internal_func(funcIdx, temp[0], temp[1]);
		return ;
    }
	else 
	{
    	serror(objThreadCntrolBlock, 18);
		return ;
    }
	return ;
	
}

void deal_replace_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result)
{
    eval_value value;
    int boolValue;
	int count ;
	string temp[NUM_OF_PARAMS];
	
	string strOpt ;

    count = 0;
    get_token(objThreadCntrolBlock);
	// Fit for circumstance without parameter
	if((*(objThreadCntrolBlock->token) == '\r')
		|| (*(objThreadCntrolBlock->token) == '\n'))
		return ;
    if(*(objThreadCntrolBlock->token) != '(')
    {
    	serror(objThreadCntrolBlock, 2);
		return ;
	}

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
		if(count == 0)
		{
			strOpt = value.getStringValue();
		}
        // sprintf(temp[count], "%s", value.getFloatValue()); // save temporarily
		temp[count] = value.getStringValue();
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    // count--;

    // Now, push on local_var_stack in reverse order.
    if(count == 3)
	{
		strOpt = strOpt.replace(strOpt.find(temp[2]), 1, temp[2]);
		return ;
    }
	else 
	{
    	serror(objThreadCntrolBlock, 18);
		return ;
    }
	return ;
}

void deal_repl_all_func(struct thread_control_block * objThreadCntrolBlock, eval_value *result)
{
    eval_value value;
    int boolValue;
	int count ;
	string temp[NUM_OF_PARAMS];
	
	string strOpt ;

    count = 0;
    get_token(objThreadCntrolBlock);
	// Fit for circumstance without parameter
	if((*(objThreadCntrolBlock->token) == '\r')
		|| (*(objThreadCntrolBlock->token) == '\n'))
		return ;
    if(*(objThreadCntrolBlock->token) != '(')
    {
    	serror(objThreadCntrolBlock, 2);
		return ;
	}

    // Process a comma-separated list of values.
    do {
        get_exp(objThreadCntrolBlock, &value, &boolValue);
		if(count == 0)
		{
			strOpt = value.getStringValue();
		}
        // sprintf(temp[count], "%s", value.getFloatValue()); // save temporarily
		temp[count] = value.getStringValue();
        get_token(objThreadCntrolBlock);
        count++;
    } while(*(objThreadCntrolBlock->token) == ',');
    // count--;

    // Now, push on local_var_stack in reverse order.
    if(count == 3)
	{
		strOpt = strOpt.replace(strOpt.find(temp[2]), 1, temp[2]);
		return ;
    }
	else 
	{
    	serror(objThreadCntrolBlock, 18);
		return ;
    }
	return ;
}



