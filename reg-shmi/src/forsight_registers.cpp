// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "time.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "../../forsight_innercmd.h"
#include "forsight_op_regs_shmi.h"
#include "forsight_registers.h"

// Register name
#define STR_PR    "pr"
#define STR_SR    "sr"
#define STR_R     "r"
#define STR_MR    "mr"

#define STR_UF    "uf"
#define STR_TF    "tf"

#define STR_PL    "pl"

// member name of Register
#define STR_REG_TYPE    "type"
#define STR_REG_ID      "id"
#define STR_REG_COMMENT "comment"
#define STR_REG_VALUE   "value"

// member name of PR
#define STR_PL_POSE    "pose"
#define STR_PL_JOINT   "joint"
// member name of UF/TF
#define STR_UF_TF_COORDINATE    "coordinate"
// member name of PL
#define STR_PL_POSE       "pose"
#define STR_PL_PALLET     "pallet"
#define STR_PL_FLAG       "flag"


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

int forgesight_get_register(struct thread_control_block* objThreadCntrolBlock, 
							char *name, void * value)
{
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
	if(namePtr)
	{
		namePtr++ ;
		memset(reg_member, 0x00, 16);
		temp = reg_member ;
		get_char_token(namePtr, temp);
	}

	if(!strcmp(reg_name, STR_PR))
	{
		getPr(value, iRegIdx);
	}
	else if(!strcmp(reg_name, STR_SR))
	{
		getSr(value, iRegIdx);
	}
	else if(!strcmp(reg_name, STR_R))
	{
		getR(value, iRegIdx);
	}
	else if(!strcmp(reg_name, STR_MR))
	{
		getMr(value, iRegIdx);
	}
	else if(!strcmp(reg_name, STR_UF))
	{
		getUf(value, iRegIdx);
	}
	else if(!strcmp(reg_name, STR_TF))
	{
		getTf(value, iRegIdx);
	}
	else if(!strcmp(reg_name, STR_PL))
	{
		getPl(value, iRegIdx);
	}
	return 0 ;
}

int forgesight_set_register(struct thread_control_block* objThreadCntrolBlock, 
							char *name, void * value)
{
	char reg_name[16] ;
	char reg_idx[16] ;
	char reg_member[16] ;
	int  iRegIdx = 0 ;
	char * namePtr = name ;
	char *temp = NULL ;
	
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

	if(!strcmp(reg_name, STR_PR))
	{
		if(strlen(reg_member) == 0)
		{
			setPr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_PL_POSE))
		{
			setPosePr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_PL_JOINT))
		{
			setJointPr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_TYPE))
		{
			setTypePr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdPr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentPr(value, iRegIdx);
		}
	}
	else if(!strcmp(reg_name, STR_SR))
	{
		if(strlen(reg_member) == 0)
		{
			setSr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_VALUE))
		{
			setValueSr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdSr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentSr(value, iRegIdx);
		}
	}
	else if(!strcmp(reg_name, STR_R))
	{
		if(strlen(reg_member) == 0)
		{
			setR(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_VALUE))
		{
			setValueR(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdR(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentR(value, iRegIdx);
		}
	}
	else if(!strcmp(reg_name, STR_MR))
	{
		if(strlen(reg_member) == 0)
		{
			setMr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_VALUE))
		{
			setValueMr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdMr(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentMr(value, iRegIdx);
		}
	}
	else if(!strcmp(reg_name, STR_UF))
	{
		if(strlen(reg_member) == 0)
		{
			setUf(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_UF_TF_COORDINATE))
		{
			setCoordinateUf(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdUf(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentUf(value, iRegIdx);
		}
	}
	else if(!strcmp(reg_name, STR_TF))
	{
		if(strlen(reg_member) == 0)
		{
			setTf(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_UF_TF_COORDINATE))
		{
			setCoordinateTf(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdTf(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentTf(value, iRegIdx);
		}
	}
	else if(!strcmp(reg_name, STR_PL))
	{
		if(strlen(reg_member) == 0)
		{
			setPl(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_PL_POSE))
		{
			setPosePl(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_PL_PALLET))
		{
			setPalletPl(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_PL_FLAG))
		{
			setFlagPl(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_ID))
		{
			setIdPl(value, iRegIdx);
		}
		else if (!strcmp(reg_member, STR_REG_COMMENT))
		{
			setCommentPl(value, iRegIdx);
		}
	}
	return 0 ;
}
