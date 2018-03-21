#ifndef REGS_SHMI_
#define REGS_SHMI_

#include "forsight_fst_datatype.h"  

#define REGS_TYPE_NUM 7
#define PR_REGS 0 
#define SR_REGS 1
#define R_REGS 2
#define MR_REGS 3
#define UF_REGS 4
#define TF_REGS 5
#define PL_REGS 6

#define PR_NUM_MAX 200
#define SR_NUM_MAX 300
#define R_NUM_MAX 1500
#define MR_NUM_MAX 1500
#define UF_NUM_MAX 10
#define TF_NUM_MAX 10
#define PL_NUM_MAX 100

#define COMMENT_SIZE 16
#define SR_VALUE_SIZE 254

typedef struct
{
	PoseEuler pose;
	Joint     joint;
	int type;
	int id;
	char comment[COMMENT_SIZE];
}pr_shmi_t;

typedef struct
{
	char value[SR_VALUE_SIZE];
	int id;
	char comment[COMMENT_SIZE];
}sr_shmi_t;

typedef struct
{
	double value;
	int id;
	char comment[COMMENT_SIZE];
}r_shmi_t;

typedef struct
{
	int value;
	int id;
	char comment[COMMENT_SIZE];
}mr_shmi_t;

typedef PoseEuler Coordinate;

typedef struct
{
	Coordinate c; 
	int id;
	char comment[COMMENT_SIZE];
}uf_shmi_t;

typedef struct
{
	Coordinate c;
	int id;
	char comment[COMMENT_SIZE];
}tf_shmi_t;

typedef struct
{
	int row;
	int column;
	int layer;
}pl_t;

typedef struct
{
	PoseEuler pose;
	pl_t pallet;
	int flag;
	int id;
	char comment[COMMENT_SIZE];
}pl_shmi_t;

#endif
