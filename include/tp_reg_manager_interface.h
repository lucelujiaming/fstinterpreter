/**
 * @file reg_manager_interface.h
 * @brief  Copy from prg_interpretor
 * @author Lujiaming
 * @version 1.0.0
 * @date 2018-05-29
 */
#ifndef REG_MANAGER_INTERFACE_H
#define REG_MANAGER_INTERFACE_H

enum {MAX_REG_COMMENT_LENGTH = 32,};

enum
{
    MAX_PR_REG_ID = 10,//200,
    MAX_SR_REG_ID = 10,//300,
    MAX_MR_REG_ID = 10,//1500,
    MAX_R_REG_ID = 10,//1500,
};

/*
	typedef enum
	{
	    REG_TYPE_PR = 0,
	    REG_TYPE_SR = 1,
	    REG_TYPE_MR = 2,
	    REG_TYPE_R = 3,
	    REG_TYPE_MAX = 4,
	    REG_TYPE_INVALID = 5,
	}RegType;
*/

typedef struct
{
    int id;
    bool is_valid;
    bool is_changed;
    char comment[MAX_REG_COMMENT_LENGTH];
}BaseRegData;


enum
{
    MAX_AXIS_NUM = 6,
    MAX_EXT_AXIS_NUM = 3,
};

typedef enum
{
    POS_TYPE_CARTESIAN = 0,
    POS_TYPE_JOINT = 1,
}PosType;

typedef struct
{
    fst_controller::PoseEuler cartesian_pos;
    double joint_pos[MAX_AXIS_NUM];
    PosType pos_type;
    double ext_pos[MAX_EXT_AXIS_NUM];   // not used
    int group_id;           // not used
    bool posture[4];        // not used
    int multiturn[3];       // not used
}PrValue;

typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    PrValue value;
}PrRegData;


typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    int value;
}MrRegData;


enum{MAX_SR_REG_STRING_LENGTH = 254,};


typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    std::string value;
}SrRegData;


#define MAX_R_REG_VALUE 99999999.999 


typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    double value;
}RRegData;


#endif //#ifndef REG_MANAGER_INTERFACE_H


