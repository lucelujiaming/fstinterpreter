#ifndef PRG_INTERPRETER_COMMON_H_
#define PRG_INTERPRETER_COMMON_H_

#ifdef WIN32
#include "fst_datatype.h"
#include "stdint.h"
using namespace fst_controller;
#else
#include "motion_control_datatype.h"
#include "stdint.h"
using namespace fst_mc;
#include "basic_alg_datatype.h" 
using namespace basic_alg;
#endif

#define ADD_INFO_NUM    10

#define TP_XPATH_LEN     512

#define USE_XPATH


typedef enum _InterpreterEventType
{
    INTERPRETER_EVENT_TYPE_WARING = 1,
    INTERPRETER_EVENT_TYPE_ERROR,
    INTERPRETER_EVENT_TYPE_MESSAGE,
}InterpreterEventType;


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
    SET_UF,
    SET_TF,
    SET_OVC,
    SET_OAC,
    END_PROG,
}InstType;

typedef enum
{
    INTERPRETER_IDLE      = 0,    
    INTERPRETER_EXECUTE   = 1,
    INTERPRETER_PAUSED    = 2,
    INTERPRETER_IDLE_TO_EXECUTE   = 101,
    INTERPRETER_EXECUTE_TO_PAUSE  = 102,
    INTERPRETER_PAUSE_TO_IDLE     = 103,
    INTERPRETER_PAUSE_TO_EXECUTE  = 104    
}InterpreterState;


typedef enum _RegDIOType
{
    NOTDEFINE = 0,
    REG = 1,
    DIO = 2,
}RegDIOType;


typedef enum _DIOType
{
    UNDEFINE = 0,
    DI = 1,
    DO = 2,
}DIOType;

typedef enum _RegOperateType
{
    POSE_REG           = 101,
    POSE_REG_POSE      = 102,
    POSE_REG_JOINT     = 103,
    POSE_REG_TYPE      = 104,
    POSE_REG_ID        = 105,
    POSE_REG_COMMENT   = 106,
	// 6
    STR_REG            = 201,
    STR_REG_VALUE      = 202,
    STR_REG_ID         = 203,
    STR_REG_COMMENT    = 204,
    // 6 + 4 = 10
    NUM_REG            = 301,
    NUM_REG_VALUE      = 302,
    NUM_REG_ID         = 303,
    NUM_REG_COMMENT    = 304,
	// 10 + 4 = 14
    MOT_REG            = 401,
    MOT_REG_VALUE      = 402,
    MOT_REG_ID         = 403,
    MOT_REG_COMMENT    = 404,
	// 14 + 4 = 18
    UF_REG             = 501,
    UF_REG_COORD       = 502,
    UF_REG_ID          = 503,
    UF_REG_COMMENT     = 504,
	// 18 + 4 = 22
    TF_REG             = 601,
    TF_REG_COORD       = 602,
    TF_REG_ID          = 603,
    TF_REG_COMMENT     = 604,
	// 22 + 4 = 26
    PL_REG             = 701,
    PL_REG_POSE        = 702,
    PL_REG_PALLET      = 703,
    PL_REG_FLAG        = 704,
    PL_REG_ID          = 705,
    PL_REG_COMMENT     = 706,
	// 26 + 6 = 32
    HOME_REG           = 801,
    HOME_REG_JOINT     = 802,
    HOME_REG_ID        = 803,
    HOME_REG_COMMENT   = 804,
	// 32 + 4 = 36
}RegOperateType;

#define REG_TYPE_NUM    32

/*
typedef struct _DIOMap
{
    int     card_seq;
    DIOType type;
    int     port_seq;
    bool has_value;
    int     value;
}DIOMap;
*/

typedef struct _IOMapPortInfo
{
    uint32_t    msg_id;
    uint32_t    dev_id;
    int         port_type;
    int         port_index;
    int         bytes_len;
    char        value;
}IOMapPortInfo;

typedef struct _IOPathInfo
{
    char        dio_path[128];
    char        value;
}IOPathInfo;

typedef struct _Reg
{
    RegOperateType type;
    int     index;
}Reg;

typedef struct _RegMap
{
    RegOperateType type;
    int     index;
    char    value[1024];
}RegMap;

typedef enum _UserOpMode
{
    NONE_U                  = 0,
    AUTO_MODE_U             = 1,
    SLOWLY_MANUAL_MODE_U    = 2,
    UNLIMITED_MANUAL_MODE_U = 3,
}UserOpMode;

typedef enum _AutoMode
{
    AUTOMODE_NONE_U     = 0,
    LOCAL_TRIGGER_U     = 1,
    LAUNCH_CODE_U       = 2,
    MACRO_TRIGGER_U     = 3,
}AutoMode;

typedef struct _InterpreterControl
{
    int cmd;
    // void* request_data_ptr;
    union
    {
        char      start_ctrl[256];
		AutoMode    autoMode ;
        // int         id;
        // int            jump_line;    // Jump 
        char           jump_line[256];
        int            step_mode;       // auto or debug 
        int            program_code;    // CODE_START
    };
}InterpreterControl;


typedef struct _CtrlStatus
{
    bool        is_permitted;  //if sending next move command is permitted
    UserOpMode  user_op_mode;
    bool        is_data_ready;  //if sending next move command is permitted
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
//    union {
        Reg             pr_reg;
        PoseEuler       pose_target;
        Joint           joint_target;
//    };
    Reg                 uf_reg;
} AdditionalOffsetInfomation;

typedef struct _AdditionalExecuteInfomation
{
    AdditionalExecuteInfomationType type;
    double    range ;
//    union {
        RegMap          assignment;
        char            fname[128];
//    };
} AdditionalExecuteInfomation;

typedef struct _AdditionalInfomation
{
    AdditionalInfomationType        type;
//    union 
//    {
    	int acc_speed ;                         // used for AAC, EV, IND_EV
    	AdditionalOffsetInfomation offset ; // used for OFFSET, TOOL_OFFSET
    	AdditionalExecuteInfomation execute ; // used for TB, TA, DB
//    };
} AdditionalInfomation;

typedef struct _Instruction
{
#ifdef USE_XPATH
    char            line[TP_XPATH_LEN];
#else
    int             line;
#endif
    InstType        type;

    MotionTarget    target;
    int  loop_cnt;
	
	int  current_uf ;
	int  current_tf ;
	double  current_ovc ;
	double  current_oac ;
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

typedef struct _MoveCommandDestination
{
    MotionType        type;
    PoseEuler       pose_target;
    Joint           joint_target;
} MoveCommandDestination;


typedef struct _RegChgList
{
    int  command;
    int             count;
#ifdef WIN32
    char additional; //malloc other memory
#else
    char additional[0]; //malloc other memory
#endif
}RegChgList;

typedef struct _ChgFrameSimple
{
	int id;
	char comment[32];
} ChgFrameSimple;

// This is output info.
typedef struct _IODeviceInfoShm
{
    char path[128];
    unsigned int id;
    char communication_type[128];
    int device_number;
    int    device_type;
    unsigned int input;
    unsigned int output;
} IODeviceInfoShm;

typedef struct
{
    int current_line_num;
    InterpreterState status;
    char program_name[256];
    char current_line_path[256];
}InterpreterPublish;


#endif
