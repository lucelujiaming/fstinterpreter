/**********************************************************************
    Copyright:	Foresight-Robotics
    File:   	fst_datatype.h
    Author:	Feng Yun
    Data:	Aug. 1 2016
    Modify:	Jun. 1 2017
    Description:Define all datatypes used in ArmGroup
**********************************************************************/

#ifndef FST_DATATYPE_H
#define FST_DATATYPE_H


#define FAIL_INTERPRETER_BASE                       (__int64)0x0001000900B50000   /*fail to dump parameter into a file*/
#define FAIL_INTERPRETER_SYNTAX_ERROR               (__int64)0x0001000900B50001 
#define FAIL_INTERPRETER_UNBALANCED_PARENTHESES     (__int64)0x0001000900B50002 
#define FAIL_INTERPRETER_NO_EXPRESSION_PRESENT      (__int64)0x0001000900B50003 
#define FAIL_INTERPRETER_EQUALS_SIGN_EXPECTED       (__int64)0x0001000900B50004 
#define FAIL_INTERPRETER_NOT_A_VARIABLE             (__int64)0x0001000900B50005 
#define FAIL_INTERPRETER_LABEL_TABLE_FULL           (__int64)0x0001000900B50006 
#define FAIL_INTERPRETER_DUPLICATE_SUB_LABEL        (__int64)0x0001000900B50007 
#define FAIL_INTERPRETER_UNDEFINED_SUB_LABEL        (__int64)0x0001000900B50008
#define FAIL_INTERPRETER_THEN_EXPECTED              (__int64)0x0001000900B50009 
#define FAIL_INTERPRETER_TO_EXPECTED                (__int64)0x0001000900B5000A 
#define FAIL_INTERPRETER_TOO_MANY_NESTED_FOR_LOOPS  (__int64)0x0001000900B5000B 
#define FAIL_INTERPRETER_NEXT_WITHOUT_FOR           (__int64)0x0001000900B5000C 
#define FAIL_INTERPRETER_TOO_MANY_NESTED_GOSUB      (__int64)0x0001000900B5000D 
#define FAIL_INTERPRETER_RETURN_WITHOUT_GOSUB       (__int64)0x0001000900B5000E 
#define FAIL_INTERPRETER_FILE_NOT_FOUND             (__int64)0x0001000900B5000F 
#define FAIL_INTERPRETER_MOVL_WITH_JOINT            (__int64)0x0001000900B50010
#define FAIL_INTERPRETER_MOVJ_WITH_POINT            (__int64)0x0001000900B50011 
#define FAIL_INTERPRETER_ILLEGAL_LINE_NUMBER        (__int64)0x0001000900B50012 
#define FAIL_INTERPRETER_FUNC_PARAMS_MISMATCH       (__int64)0x0001000900B50013 
#define FAIL_INTERPRETER_DUPLICATE_EXEC_MACRO       (__int64)0x0001000900B50014 

#define INFO_INTERPRETER_BACK_TO_BEGIN              (__int64)0x0001000200B50015
#define FAIL_INTERPRETER_ALARM_EXEC_BASE            (__int64)0x0001000900B50100 
#define FAIL_INTERPRETER_USER_ALARM1                (__int64)0x0001000900B50101
#define FAIL_INTERPRETER_USER_ALARM2                (__int64)0x0001000900B50102
#define FAIL_INTERPRETER_USER_ALARM3                (__int64)0x0001000900B50103
#define FAIL_INTERPRETER_USER_ALARM4                (__int64)0x0001000900B50104
#define FAIL_INTERPRETER_USER_ALARM5                (__int64)0x0001000900B50105
#define FAIL_INTERPRETER_USER_ALARM6                (__int64)0x0001000900B50106
#define FAIL_INTERPRETER_USER_ALARM7                (__int64)0x0001000900B50107
#define FAIL_INTERPRETER_USER_ALARM8                (__int64)0x0001000900B50108
#define FAIL_INTERPRETER_USER_ALARM9                (__int64)0x0001000900B50109
#define FAIL_INTERPRETER_USER_ALARM10               (__int64)0x0001000900B5010A
#define FAIL_INTERPRETER_NOT_IN_PAUSE               (__int64)0x0001000900B5010B


#define                 PI      3.1415926535897932384626433832795
#define       NUM_OF_JOINT      9
#define  AXIS_IN_ALGORITHM      6

namespace fst_controller {

class MotionCommand;

typedef unsigned int    Tick;
typedef unsigned int    Size;
typedef double          MotionTime;
typedef double          Angle;
typedef double          Omega;
typedef double          Alpha;



/* Define a group of values in joint space */
struct Joint {
    Angle   j1;
    Angle   j2;
    Angle   j3;
    Angle   j4;
    Angle   j5;
    Angle   j6;
    Angle   j7;
    Angle   j8;
    Angle   j9;
};

// Define a angular velocity structure
struct JointOmega {
    Omega   j1;
    Omega   j2;
    Omega   j3;
    Omega   j4;
    Omega   j5;
    Omega   j6;
    Omega   j7;
    Omega   j8;
    Omega   j9;
};

struct JointAlpha {
    Alpha   j1;
    Alpha   j2;
    Alpha   j3;
    Alpha   j4;
    Alpha   j5;
    Alpha   j6;
    Alpha   j7;
    Alpha   j8;
    Alpha   j9;
};

struct JointState
{
    Angle   joint[NUM_OF_JOINT];
    Omega   omega[NUM_OF_JOINT];
    Alpha   alpha[NUM_OF_JOINT];
};

struct JointGroup {
    Tick        stamp;
    JointState  j1;
    JointState  j2;
    JointState  j3;
    JointState  j4;
    JointState  j5;
    JointState  j6;
    JointState  j7;
    JointState  j8;
    JointState  j9;
};


struct Spline {
    double  coeff[6];
};

struct JointSegment {
    MotionTime  time_from_start;
    MotionTime  duration;
    JointGroup  start;
    JointGroup  end;
    Spline      coeff[NUM_OF_JOINT];
};

// Define a point in cartesian space
struct Point {
    double x;
    double y;
    double z;
};

// Define a quaternion
struct Quaternion {
    double x;
    double y;
    double z;
    double w;
};

// Define an euler angle
struct Euler {
    double a;
    double b;
    double c;
};

// Define a pose in cartesian space
struct Pose {
    Point       position;
    Quaternion  orientation;
};

// Define another pose type using euler angle
struct PoseEuler {
    Point position;
    Euler orientation;
};




// Define the home position, upper limit and lower limit of a joint
struct JointLimit {
    double home;
    double upper;
    double lower;

    //double max_omega;
    //double max_alpha;
};

// Define all joint limits of a robot
struct JointConstraint {
    JointLimit j1;
    JointLimit j2;
    JointLimit j3;
    JointLimit j4;
    JointLimit j5;
    JointLimit j6;
    JointLimit j7;
    JointLimit j8;
    JointLimit j9;
};

// Define a transformation type
//struct Transformation {
//    Point position;
//    Euler orientation;
//};
typedef PoseEuler Transformation;

// point level: 0x0 -> middle point
//              0x1 -> start  point
//              0x2 -> ending point
enum PointLevel {
    POINT_MIDDLE    = 0x0,
    POINT_START     = 0x1,
    POINT_ENDING    = 0x2,
    POINT_LAST      = 0x3,
    POINT_FIRST     = 0x4,
};

const unsigned int POINT_LEVEL_MASK  = 0x00000003;

// A flag indicating the type of a motion
enum MotionType {
    MOTION_NONE,
    MOTION_JOINT,
    MOTION_LINE,
    MOTION_CIRCLE,
};

// A flag indicating the smooth type between two motions
enum SmoothType {
    SMOOTH_NONE,
    SMOOTH_2J,
    SMOOTH_2L,
    SMOOTH_2C,
    SMOOTH_UNKNOWN,
};

enum SmoothMode
{
    MODE_VELOCITY,
    MODE_DISTANCE,
};

// PathPoint structure defines what does path-plan should give out.
struct PathPoint {
    // Command type, joint command or cartesian command*/
    MotionType  type;

    int         id;

    // point stamp
    Tick        stamp;

    // the point generated from which command
    MotionCommand *source;

    // value in cartesian space or joint space
    union {
        Pose    pose;
        Joint   joint;
    };
};

// ControlPoint structure used in trajectory-create.
struct ControlPoint {
    // point from path plan
    PathPoint   path_point;

    // time from start
    // time < 0 means this point has not been converted to a trajectory point
    MotionTime  time_from_start;

    // time duration from prev point to this point
    // duration < 0 means this point has not been converted to a trajectory point
    // command_duration = cycle_step / v_command
    // duration = actual dutation
    MotionTime  command_duration;
    MotionTime  forward_duration;
    MotionTime  backward_duration;

    
    // point is what trajectory-create should give out
    JointState  forward_point;
    JointState  backward_point;

    double  coeff[AXIS_IN_ALGORITHM][6];

    //bool    alpha_valid;
    //double  alpha_upper[6];
    //double  alpha_lower[6];
};

struct TrajSegment
{
    int         id;
    MotionTime  time_from_start;
    MotionTime  duration;

    //bool    quadratic;
    //bool    quintic;
    //bool    t_curve;

    double      coeff[AXIS_IN_ALGORITHM][6];
};

// Define a point structure used in joint trajectory fifo
struct JointPoint {
    // time stamp
    int stamp;
    // point level, start/middle/ending point
    PointLevel  level;
    // the point generated from which command
    MotionCommand *source;
    // joint values of this point
    Joint       joint;
    // angular velocity of this point
    JointOmega  omega;
};



////////////////////////////////////////////////////////////////////////////////////////////
struct JointOutput {
    int id;
    Joint       joint;
    PointLevel  level;
};

// Define a group of coordinate offset
struct CoordinateOffset {
    double alpha;
    double a;
    double d;
    double theta;
};

// Define a group of DH parameter
struct DHGroup {
    CoordinateOffset j1;
    CoordinateOffset j2;
    CoordinateOffset j3;
    CoordinateOffset j4;
    CoordinateOffset j5;
    CoordinateOffset j6;
};

// target poses of circle motion
struct CircleTarget {
    PoseEuler pose1;
    PoseEuler pose2;
};

// target structure used in motion command
struct MotionTarget {
    MotionType  type;

    // 0.0 - 1.0
    double cnt;

    // percent velocity in move Joint, range: 0.0-1.0
    // linear velocity in move cartesian, range 0.0-MAC_VEL
    // velocity < 0 means using default velocity
    double vel;

    // percent accleration in move Joint, range: 0.0-1.0
    // linear accleration in move cartesian, range: 0.0-MAX_ACC
    // accleration < 0 means using default accleration
    double acc;

    union {
        PoseEuler       pose_target;
        Joint           joint_target;
        CircleTarget    circle_target;
    };
};

// initial state of a line motion
struct LInitial {
    Pose    pose_prv;
    Pose    pose;
    double  velocity;
    double  vu;
};

// initial state of a circle motion
struct CInitial {
    Pose    pose;
    double  velocity;
};

struct Arc {
    double ArcRot[4][4];
    double radius;
    double omega;
};

struct orienttrans {
    double q0[4];
    double q1[4];
    double s1[4];
    double s2[4];
    int modeflag;
    double theta_q;
    double theta_s;
};

struct ElementSmooth {
    double time[8];
    double Polynominal[7][4];
};

struct Parabola {
    double P0[3];
    double v0[3];
    double a0[3];
    double P2[3];
    double v2[3];
    double a2[3];
    double T;
    double lamda[3];
};

struct LCommand
{
    Pose    pose;
    double  velocity;
    union
    {
        int     cnt;
        double  smooth_radius;
    };
    
};

struct CCommand
{
    Pose    pose1;
    Pose    pose2;
    double  velocity;
    union
    {
        int     cnt;
        double  smooth_radius;
    };

};

struct JCommand
{
    Joint       joints;
    double      v_percentage;
    union
    {
        int         cnt;
        double      smooth_radius;
    };

};

struct VelCartesian
{
    double vx;
    double vy;
    double vz;
    double omegax;
    double omegay;
    double omegaz;
};

struct PoseVel
{
    Pose pose;
    VelCartesian velocity;
};

enum ManualFrame
{
    JOINT,
    WORLD,
    USER,
    TOOL,
};

enum ManualMode
{
    STEP,
    CONTINUOUS,
    APOINT,
};

enum ManualDirection
{
    STANDBY  = 0,
    INCREASE = 1,
    DECREASE = 2,
};

struct ManualCoeff
{
    double duration_1;
    double duration_2;
    double duration_3;
    double alpha_1;
    double alpha_3;
};





}   // namespace fst_controller



#endif  // #ifndef FST_DATATYPE_H
