#ifndef FORSIGHT_OP_REGS_SHMI_
#define FORSIGHT_OP_REGS_SHMI_

#include <stdint.h>

void generateFakeData();

#include "forsight_op_shmi.h"

// extern static int fdRegShmi;
// extern static void *ptrRegShmi;

#include "forsight_regs_shmi.h"

#define PR_OFFSET 0
#define SR_OFFSET (PR_OFFSET + PR_NUM_MAX * sizeof(pr_shmi_t))
#define R_OFFSET (SR_OFFSET + SR_NUM_MAX * sizeof(sr_shmi_t))
#define MR_OFFSET (R_OFFSET + R_NUM_MAX * sizeof(r_shmi_t))
#define UF_OFFSET (MR_OFFSET + MR_NUM_MAX * sizeof(mr_shmi_t))
#define TF_OFFSET (UF_OFFSET + UF_NUM_MAX * sizeof(uf_shmi_t))
#define PL_OFFSET (TF_OFFSET + TF_NUM_MAX * sizeof(tf_shmi_t))

/* init regs */
int initRegShmi();

/**********************
 ********* PR *********
 **********************/

/*
 * The operated object is all members of PR.
 */
int getPr(void *ptr, uint16_t num);
int setPr(void *ptr, uint16_t num);

/*
 * The operated object is an individual member of PR.
 */
int getPosePr(void *ptr, uint16_t num);
int setPosePr(void *ptr, uint16_t num);

int getJointPr(void *ptr, uint16_t num);
int setJointPr(void *ptr, uint16_t num);

int getTypePr(void *ptr, uint16_t num);
int setTypePr(void *ptr, uint16_t num);

int getIdPr(void *ptr, uint16_t num);
int setIdPr(void *ptr, uint16_t num);

int getCommentPr(void *ptr, uint16_t num);
int setCommentPr(void *ptr, uint16_t num);

/**********************
 ********* SR *********
 **********************/

int getSr(void *ptr, uint16_t num);
int setSr(void *ptr, uint16_t num);

int getValueSr(void *ptr, uint16_t num);
int setValueSr(void *ptr, uint16_t num);

int getIdSr(void *ptr, uint16_t num);
int setIdSr(void *ptr, uint16_t num);

int getCommentSr(void *ptr, uint16_t num);
int setCommentSr(void *ptr, uint16_t num);

/**********************
 ********* R **********
 **********************/
int getR(void *ptr, uint16_t num);
int setR(void *ptr, uint16_t num);

int getValueR(void *ptr, uint16_t num);
int setValueR(void *ptr, uint16_t num);

int getIdR(void *ptr, uint16_t num);
int setIdR(void *ptr, uint16_t num);

int getCommentR(void *ptr, uint16_t num);
int setCommentR(void *ptr, uint16_t num);

/**********************
 ********* MR *********
 **********************/
int getMr(void *ptr, uint16_t num);
int setMr(void *ptr, uint16_t num);
 
int getValueMr(void *ptr, uint16_t num);
int setValueMr(void *ptr, uint16_t num);

int getIdMr(void *ptr, uint16_t num);
int setIdMr(void *ptr, uint16_t num);

int getCommentMr(void *ptr, uint16_t num);
int setCommentMr(void *ptr, uint16_t num);


/**********************
 ********* UF *********
 **********************/
int getUf(void *ptr, uint16_t num);
int setUf(void *ptr, uint16_t num);

int getCoordinateUf(void *ptr, uint16_t num);
int setCoordinateUf(void *ptr, uint16_t num);

int getIdUf(void *ptr, uint16_t num);
int setIdUf(void *ptr, uint16_t num);

int getCommentUf(void *ptr, uint16_t num);
int setCommentUf(void *ptr, uint16_t num);

/**********************
 ********* TF *********
 **********************/
int getTf(void *ptr, uint16_t num);
int setTf(void *ptr, uint16_t num);

int getCoordinateTf(void *ptr, uint16_t num);
int setCoordinateTf(void *ptr, uint16_t num);

int getIdTf(void *ptr, uint16_t num);
int setIdTf(void *ptr, uint16_t num);

int getCommentTf(void *ptr, uint16_t num);
int setCommentTf(void *ptr, uint16_t num);

/**********************
 ********* PL *********
 **********************/
int getPl(void *ptr, uint16_t num);
int setPl(void *ptr, uint16_t num);

int getPosePl(void *ptr, uint16_t num);
int setPosePl(void *ptr, uint16_t num);

int getPalletPl(void *ptr, uint16_t num);
int setPalletPl(void *ptr, uint16_t num);

int getFlagPl(void *ptr, uint16_t num);
int setFlagPl(void *ptr, uint16_t num);

int getIdPl(void *ptr, uint16_t num);
int setIdPl(void *ptr, uint16_t num);

int getCommentPl(void *ptr, uint16_t num);
int setCommentPl(void *ptr, uint16_t num);

#endif
