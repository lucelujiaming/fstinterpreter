#ifndef INTERPRETER_COMMON_H_
#define INTERPRETER_COMMON_H_

#include "fst_datatype.h"
using namespace fst_controller;

#define ADD_INFO_NUM    10

#define TP_XPATH_LEN     1024

typedef enum _AdditionalInfomationType
{
    ACC, 
    CONDITION,
    CONDITION_OFFSET ,
    OFFSET ,
    TOOL_OFFSET, 
    EV,
    IND_EV,
    TB,
    TA,
    DB,
} AdditionalInfomationType;

typedef enum _AdditionalOffsetInfomationType
{
    PR, 
    PR_UF, 
    C_VEC ,
    C_VEC_UF ,
    J_VEC, 
    J_VEC_UF, 
} AdditionalOffsetInfomationType;

typedef enum __AdditionalExecuteInfomationType
{
    ASSIGNMENT, 
    SUBROUTINE ,
} AdditionalExecuteInfomationType;

typedef enum _InstType
{
    COMMON = 1,
    LOGIC_TOK,
    END_TOK,
    MOTION,
    END_PROG,
}InstType;

typedef enum _InterpreterState
{
    IDLE_R      = 0,    
    EXECUTE_R   = 1,
    PAUSED_R    = 2,

    IDLE_TO_EXECUTE_T   = 101,
    EXECUTE_TO_PAUSE_T  = 102,
    PAUSE_TO_IDLE_T     = 103,
    PAUSE_TO_EXECUTE_T  = 104,
}InterpreterState;

typedef enum _InterpreterCommand
{
    LOAD    = 101,
    JUMP    = 102,
    START   = 103,
    DEBUG   = 104,
    FORWARD  = 105,
    BACKWARD = 106,
    CONTINUE = 107,
    PAUSE   = 108,
    ABORT   = 109,

    MOD_REG = 201,
}InterpreterCommand;

typedef enum _DIOType
{
    UNDEFINE = 0,
    DI = 1,
    DO = 2,
}DIOType;

typedef enum _RegType
{
    POSE = 1,
    NUM = 2,
    STR = 3,
    PLT = 4,
}RegType;


typedef struct _DIOMap
{
    int     card_seq;
    DIOType type;
    int     port_seq;
    int     value;
}DIOMap;

typedef struct _Reg
{
    RegType type;
    int     index;
}Reg;

typedef struct _RegMap
{
    RegType type;
    int     index;
    char    value[128];
}RegMap;

typedef enum _UserOpMode
{
    NONE_U                  = 0,
    AUTO_MODE_U             = 1,
    SLOWLY_MANUAL_MODE_U    = 2,
    UNLIMITED_MANUAL_MODE_U = 3,
}UserOpMode;



typedef struct _StartCtrl
{
    char        file_name[128];
}StartCtrl;

typedef struct _InterpreterControl
{
    InterpreterCommand cmd;
    union
    {
        StartCtrl   start_ctrl;
        // int         id;
#ifdef USE_XPATH
        char           line[TP_XPATH_LEN];
#else
    int             line;
#endif
        RegMap      reg;
        DIOMap      dio;
    };
}InterpreterControl;


typedef struct _CtrlStatus
{
    bool        is_permitted;  //if sending next move command is permitted
    UserOpMode  user_op_mode;
}CtrlStatus;

typedef struct _IntprtStatus
{
#ifdef USE_XPATH
    char            line[TP_XPATH_LEN];
#else
    int             line;
#endif
    InterpreterState    state;
#ifdef WIN32
	__int64         warn;
#else
    long long int   warn;
#endif
}IntprtStatus;

typedef struct _AdditionalOffsetInfomation
{
    AdditionalOffsetInfomationType        type;
    union {
        Reg             pr_reg;
        PoseEuler       pose_target;
        Joint           joint_target;
    };
    Reg                 uf_reg;
} AdditionalOffsetInfomation;

typedef struct _AdditionalExecuteInfomation
{
    AdditionalExecuteInfomationType type;
    double    range ;
    union {
        RegMap          assignment;
        char            fname[128];
    };
} AdditionalExecuteInfomation;

typedef struct _AdditionalInfomation
{
    AdditionalInfomationType        type;
    union 
    {
    	int acc_speed ;                         // used for AAC, EV, IND_EV
    	AdditionalOffsetInfomation offset ; // used for OFFSET, TOOL_OFFSET
    	AdditionalExecuteInfomation execute ; // used for TB, TA, DB
    };
} AdditionalInfomation;

typedef struct _Instruction
{
#ifdef USE_XPATH
    char            line[TP_XPATH_LEN];
#else
    int             line;
#endif
    InstType        type;
    union 
    {
        MotionTarget    target;
        int             loop_cnt;
    };
    bool is_additional;
    int add_num;
#ifdef WIN32
    char additional; //malloc other memory
#else
    char additional[0]; //malloc other memory
#endif
    //add some other additional statement
    //.....
    //.....
}Instruction;

#endif
