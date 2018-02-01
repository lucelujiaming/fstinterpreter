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

namespace fst_controller {

class MoveCommand1;

/* Define a group of values in joint space */
struct Joint {
    double j1;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;
    double j7;
    double j8;
    double j9;
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

    double max_omega;
    double max_alpha;
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
    //SMOOTH_J2J,
    //SMOOTH_J2L,
    //SMOOTH_J2C,
    //SMOOTH_L2J,
    //SMOOTH_L2L,
    //SMOOTH_L2C,
    //SMOOTH_C2J,
    //SMOOTH_C2L,
    //SMOOTH_C2C,
};

enum SmoothMode
{
    MODE_VELOCITY,
    MODE_DISTANCE,
};

enum CurveMode
{
    T_CURVE,
    S_CURVE,
};

// Define a pose/joint point structure used in planned path fifo
struct PathPoint {
    /*
    // Command ID
    int id; 
    // Command type, joint command or cartesian command*/
    MotionType type; 
    // time stamp
    int stamp;
    // point level, start/middle/ending point
    PointLevel  level;
    // the point generated from which command
    MoveCommand1 *source;
    // value in cartesian space or joint space
    union {
        PoseEuler    pose;
        Joint   joint;
    };
};

// Define a angular velocity structure
struct JointOmega {
    double j1;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;
    double j7;
    double j8;
    double j9;
};

// Define a point structure used in joint trajectory fifo
struct JointPoint {
    // time stamp
    int stamp;
    // point level, start/middle/ending point
    PointLevel  level;
    // the point generated from which command
    MoveCommand1 *source;
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

    // percent velocity in move Joint
    // linear velocity in move cartesian
    double velocity;

    // percent accleration in move Joint
    // linear accleration in move cartesian
    // accleration < 0.000001 means using default accleration
    double accleration;

    union {
        PoseEuler       pose_target;
        Joint           joint_target;
        CircleTarget    circle_target;
    };
};

enum ManualDirection {
    STANDBY,
    UPWARD,
    DOWNWARD,
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

enum ManualFrameMode
{
    JOINT,
    WORLD,
    USER,
    TOOL,
};

enum ManualMotionMode
{
    STEP,
    CONTINUOUS,
};



}   // namespace fst_controller



#endif  // #ifndef FST_DATATYPE_H
