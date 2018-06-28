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

#define PI 3.1415926535897932384626433832795


/* Define a group of values in joint space */
typedef struct {
    double j1;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;
    double j7;
    double j8;
    double j9;
}Joint;

// Define a point in cartesian space
typedef struct {
    double x;
    double y;
    double z;
}Point;

// Define a quaternion
typedef struct {
    double x;
    double y;
    double z;
    double w;
}Quaternion;

// Define an euler angle
typedef struct {
    double a;
    double b;
    double c;
}Euler;

// Define an additional E
typedef struct _AdditionalE {
    double e1;
    double e2;
    double e3;
} AdditionalE;

// Define a pose in cartesian space
typedef struct {
    Point       position;
    Quaternion  orientation;
}Pose;

// Define another pose type using euler angle
typedef struct {
    Point position;
    Euler orientation;
}PoseEuler;

// Define the home position, upper limit and lower limit of a joint
typedef struct {
    double home;
    double upper;
    double lower;

    double max_omega;
    double max_alpha;
}JointLimit;

// Define all joint limits of a robot
typedef struct {
    JointLimit j1;
    JointLimit j2;
    JointLimit j3;
    JointLimit j4;
    JointLimit j5;
    JointLimit j6;
    JointLimit j7;
    JointLimit j8;
    JointLimit j9;
}JointConstraint;

// Define a transformation type
//struct Transformation {
//    Point position;
//    Euler orientation;
//};
typedef PoseEuler Transformation;

// point level: 0x0 -> middle point
//              0x1 -> start  point
//              0x2 -> ending point
typedef enum {
    POINT_MIDDLE    = 0x0,
    POINT_START     = 0x1,
    POINT_ENDING    = 0x2,
    POINT_LAST      = 0x3,
    POINT_FIRST     = 0x4,
}PointLevel;

// const unsigned int POINT_LEVEL_MASK  = 0x00000003;

// A flag indicating the type of a motion
typedef enum {
    MOTION_NONE,
    MOTION_JOINT,
    MOTION_LINE,
    MOTION_CIRCLE,
}MotionType;

// A flag indicating the smooth type between two motions
typedef enum {
    SMOOTH_NONE,
    SMOOTH_2J,
    SMOOTH_2L,
    SMOOTH_2C,
    //SMOOTH_J2J,
    //SMOOTH_J2L,
    //SMOOTH_J2C,
    //SMOOTH_L2J,
    //SMOOTH_L2L,
    //SMOOTH_L2C,
    //SMOOTH_C2J,
    //SMOOTH_C2L,
    //SMOOTH_C2C,
}SmoothType;

typedef enum 
{
    MODE_VELOCITY,
    MODE_DISTANCE,
}SmoothMode;

typedef enum 
{
    T_CURVE,
    S_CURVE,
}CurveMode;

// Define a angular velocity structure
typedef struct {
    double j1;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;
    double j7;
    double j8;
    double j9;
}JointOmega;


////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    int id;
    Joint       joint;
    PointLevel  level;
}JointOutput;

// Define a group of coordinate offset
typedef struct {
    double alpha;
    double a;
    double d;
    double theta;
}CoordinateOffset;

// Define a group of DH parameter
typedef struct {
    CoordinateOffset j1;
    CoordinateOffset j2;
    CoordinateOffset j3;
    CoordinateOffset j4;
    CoordinateOffset j5;
    CoordinateOffset j6;
}DHGroup;

// target poses of circle motion
typedef struct {
    Pose pose1;
    Pose pose2;
}CircleTarget;

// target structure used in motion command
typedef struct {
    MotionType  type;
    union {
        int     cnt;
        double  smooth_radius;
    };
    union {
        double  linear_velocity;
        double  percent_velocity;
    };
    union {
        Pose            pose_target;
        Joint           joint_target;
        CircleTarget    circle_target;
    };
}MotionTarget;

// initial state of a line motion
typedef struct {
    Pose    pose_prv;
    Pose    pose;
    double  velocity;
    double  vu;
}LInitial;

// initial state of a circle motion
typedef struct {
    Pose    pose;
    double  velocity;
}CInitial;

typedef struct {
    double ArcRot[4][4];
    double radius;
    double omega;
}Arc;

typedef struct {
    double q0[4];
    double q1[4];
    double s1[4];
    double s2[4];
    int modeflag;
    double theta_q;
    double theta_s;
}orienttrans;

typedef struct {
    double time[8];
    double Polynominal[7][4];
}ElementSmooth;

typedef struct {
    double P0[3];
    double v0[3];
    double a0[3];
    double P2[3];
    double v2[3];
    double a2[3];
    double T;
    double lamda[3];
}Parabola;

typedef struct {
    Pose    pose;
    double  velocity;
    union
    {
        int     cnt;
        double  smooth_radius;
    };
    
}LCommand;

typedef struct 
{
    Pose    pose1;
    Pose    pose2;
    double  velocity;
    union
    {
        int     cnt;
        double  smooth_radius;
    };

}CCommand;

typedef struct
{
    Joint       joints;
    double      v_percentage;
    union
    {
        int         cnt;
        double      smooth_radius;
    };

}JCommand;

typedef struct VelCartesian
{
    double vx;
    double vy;
    double vz;
    double omegax;
    double omegay;
    double omegaz;
}VelCartesian;

typedef struct 
{
    Pose pose;
    VelCartesian velocity;
}PoseVel;

enum ManualMode
{
    WORLD,
    USER,
    TOOL,
};







#endif  // #ifndef FST_DATATYPE_H
