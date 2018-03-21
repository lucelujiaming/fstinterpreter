#include <stdio.h>
#include "forsight_op_shmi.h"
#include "forsight_regs_shmi.h"

#define OFFSET_MEM(mem,struction) (size_t)&(((struction *)0)->mem)

struct shmi_info pr_info[PR_NUM_MAX];
struct shmi_info sr_info[SR_NUM_MAX];
struct shmi_info r_info[R_NUM_MAX];
struct shmi_info mr_info[MR_NUM_MAX];
struct shmi_info uf_info[UF_NUM_MAX];
struct shmi_info tf_info[TF_NUM_MAX];
struct shmi_info pl_info[TF_NUM_MAX];

/***********************************
 ********         PR        ********
 ***********************************/

/*
 * The operated object is all members of PR.
 */
int getPr(void *ptr, uint16_t num) {
	pr_info[num].offset = 0;
	pr_info[num].size = sizeof(pr_shmi_t);
	return transData(ptr, &pr_info[num], (uint8_t)READ_REG_SHMI);
}
int setPr(void *ptr, uint16_t num) {
	pr_info[num].offset = 0;
	pr_info[num].size = sizeof(pr_shmi_t);
	return transData(ptr, &pr_info[num], (uint8_t)WRITE_REG_SHMI);
}

/*
 * The operated object is an individual member of PR.
 */
int getPosePr(void *ptr, uint16_t num) {
	pr_info[num].offset = OFFSET_MEM(pose, pr_shmi_t);
	pr_info[num].size = sizeof(PoseEuler);
	return transData(ptr, &pr_info[num], (uint8_t)READ_REG_SHMI);
}

int setPosePr(void *ptr, uint16_t num) {
	pr_info[num].offset = OFFSET_MEM(pose, pr_shmi_t);
	pr_info[num].size = sizeof(PoseEuler);
	return transData(ptr, &pr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getJointPr(void *ptr, uint16_t num){
	pr_info[num].offset = OFFSET_MEM(joint, pr_shmi_t);
	pr_info[num].size = sizeof(PoseEuler);
	return transData(ptr, &pr_info[num], (uint8_t)READ_REG_SHMI);
}

int setJointPr(void *ptr, uint16_t num) {
	pr_info[num].offset = OFFSET_MEM(joint, pr_shmi_t);
	pr_info[num].size = sizeof(PoseEuler);
	return transData(ptr, &pr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getTypePr(void *ptr, uint16_t num) {
	pr_info[num].offset = OFFSET_MEM(type, pr_shmi_t);
	pr_info[num].size = sizeof(int);
	return transData(ptr, &pr_info[num], (uint8_t)READ_REG_SHMI);
}

int setTypePr(void *ptr, uint16_t num) {
	pr_info[num].offset = OFFSET_MEM(type, pr_shmi_t);
	pr_info[num].size = sizeof(int);
	return transData(ptr, &pr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdPr(void *ptr, uint16_t num){
	pr_info[num].offset = OFFSET_MEM(id, pr_shmi_t);
	pr_info[num].size = sizeof(int);
	return transData(ptr, &pr_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdPr(void *ptr, uint16_t num) {
	pr_info[num].offset = OFFSET_MEM(id, pr_shmi_t);
	pr_info[num].size = sizeof(int);
	return transData(ptr, &pr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentPr(void *ptr, uint16_t num) {
    pr_info[num].offset = OFFSET_MEM(comment, pr_shmi_t);
    pr_info[num].size = (size_t)COMMENT_SIZE;
	return transData(ptr, &pr_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentPr(void *ptr, uint16_t num) {
    pr_info[num].offset = OFFSET_MEM(comment, pr_shmi_t);
    pr_info[num].size = (size_t)COMMENT_SIZE;
    return transData(ptr, &pr_info[num], (uint8_t)WRITE_REG_SHMI);
}

/***********************************
 *********        SR        ********
 ***********************************/
int getSr(void *ptr, uint16_t num) {
	sr_info[num].offset = 0;
	sr_info[num].size = sizeof(sr_shmi_t);
	return transData(ptr, &sr_info[num], (uint8_t)READ_REG_SHMI);
}
int setSr(void *ptr, uint16_t num) {
	sr_info[num].offset = 0;
	sr_info[num].size = sizeof(sr_shmi_t);
	return transData(ptr, &sr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getValueSr(void *ptr, uint16_t num) {
	sr_info[num].offset = OFFSET_MEM(value,sr_shmi_t);
	sr_info[num].size = SR_VALUE_SIZE;
	return transData(ptr, &sr_info[num], (uint8_t)READ_REG_SHMI);
}
int setValueSr(void *ptr, uint16_t num) {
	sr_info[num].offset = OFFSET_MEM(value,sr_shmi_t);
	sr_info[num].size = SR_VALUE_SIZE;
	return transData(ptr, &sr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdSr(void *ptr, uint16_t num) {
#ifdef WIN32
	sr_shmi_t temp,  * tempPtr = &temp;
    sr_info[num].offset = (int)&(tempPtr->id) - (int)tempPtr ;
#else
	sr_info[num].offset = OFFSET_MEM(id,sr_shmi_t);
#endif
	sr_info[num].size = sizeof(int);
	return transData(ptr, &sr_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdSr(void *ptr, uint16_t num) {
#ifdef WIN32
	sr_shmi_t temp,  * tempPtr = &temp;
    sr_info[num].offset = (int)&(tempPtr->id) - (int)tempPtr ;
#else
	sr_info[num].offset = OFFSET_MEM(id,sr_shmi_t);
#endif
	sr_info[num].size = sizeof(int);
	return transData(ptr, &sr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentSr(void *ptr, uint16_t num) {
#ifdef WIN32
	sr_shmi_t temp,  * tempPtr = &temp;
    sr_info[num].offset = (int)&(tempPtr->comment) - (int)tempPtr ;
#else
	sr_info[num].offset = OFFSET_MEM(comment,sr_shmi_t);
#endif
	sr_info[num].size = COMMENT_SIZE;
	return transData(ptr, &sr_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentSr(void *ptr, uint16_t num) {
#ifdef WIN32
	sr_shmi_t temp,  * tempPtr = &temp;
    sr_info[num].offset = (int)&(tempPtr->comment) - (int)tempPtr ;
#else
	sr_info[num].offset = OFFSET_MEM(comment,sr_shmi_t);
#endif
	sr_info[num].size = COMMENT_SIZE;
	return transData(ptr, &sr_info[num], (uint8_t)WRITE_REG_SHMI);
}

/***********************************
 *********        R        *********
 ***********************************/
int getR(void *ptr, uint16_t num) {
	r_info[num].offset = 0;
	r_info[num].size = sizeof(r_shmi_t);
	return transData(ptr, &r_info[num], (uint8_t)READ_REG_SHMI);
}
int setR(void *ptr, uint16_t num) {
	r_info[num].offset = 0;
	r_info[num].size = sizeof(r_shmi_t);
	return transData(ptr, &r_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getValueR(void *ptr, uint16_t num) {
	r_info[num].offset = OFFSET_MEM(value,r_shmi_t);
	r_info[num].size = sizeof(((r_shmi_t *)0)->value);
	return transData(ptr, &r_info[num], (uint8_t)READ_REG_SHMI);
}
int setValueR(void *ptr, uint16_t num) {
	r_info[num].offset = OFFSET_MEM(value,r_shmi_t);
	r_info[num].size = sizeof(((r_shmi_t *)0)->value);
	return transData(ptr, &r_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdR(void *ptr, uint16_t num) {
	r_info[num].offset = OFFSET_MEM(id,r_shmi_t);
	r_info[num].size = sizeof(((r_shmi_t *)0)->id);
	return transData(ptr, &r_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdR(void *ptr, uint16_t num) {
	r_info[num].offset = OFFSET_MEM(id,r_shmi_t);
	r_info[num].size = sizeof(((r_shmi_t *)0)->id);
	return transData(ptr, &r_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentR(void *ptr, uint16_t num) {
	r_info[num].offset = OFFSET_MEM(comment,r_shmi_t);
	r_info[num].size = sizeof(((r_shmi_t *)0)->comment);
	return transData(ptr, &r_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentR(void *ptr, uint16_t num) {
	r_info[num].offset = OFFSET_MEM(comment,r_shmi_t);
	r_info[num].size = sizeof(((r_shmi_t *)0)->comment);
	return transData(ptr, &r_info[num], (uint8_t)WRITE_REG_SHMI);
}

/***********************************
 *********        MR        ********
 ***********************************/
int getMr(void *ptr, uint16_t num) {
	mr_info[num].offset = 0;
	mr_info[num].size = sizeof(mr_shmi_t);
	return transData(ptr, &mr_info[num], (uint8_t)READ_REG_SHMI);
}
int setMr(void *ptr, uint16_t num) {
	mr_info[num].offset = 0;
	mr_info[num].size = sizeof(mr_shmi_t);
	return transData(ptr, &mr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getValueMr(void *ptr, uint16_t num) {
	mr_info[num].offset = OFFSET_MEM(value,mr_shmi_t);
	mr_info[num].size = sizeof(((mr_shmi_t *)0)->value);
	return transData(ptr, &mr_info[num], (uint8_t)READ_REG_SHMI);
}
int setValueMr(void *ptr, uint16_t num) {
	mr_info[num].offset = OFFSET_MEM(value,mr_shmi_t);
	mr_info[num].size = sizeof(((mr_shmi_t *)0)->value);
	return transData(ptr, &mr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdMr(void *ptr, uint16_t num) {
	mr_info[num].offset = OFFSET_MEM(id,mr_shmi_t);
	mr_info[num].size = sizeof(((mr_shmi_t *)0)->id);
	return transData(ptr, &mr_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdMr(void *ptr, uint16_t num) {
	mr_info[num].offset = OFFSET_MEM(id,mr_shmi_t);
	mr_info[num].size = sizeof(((mr_shmi_t *)0)->id);
	return transData(ptr, &mr_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentMr(void *ptr, uint16_t num) {
	mr_info[num].offset = OFFSET_MEM(comment,mr_shmi_t);
	mr_info[num].size = sizeof(((mr_shmi_t *)0)->comment);
	return transData(ptr, &mr_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentMr(void *ptr, uint16_t num) {
	mr_info[num].offset = OFFSET_MEM(comment,mr_shmi_t);
	mr_info[num].size = sizeof(((mr_shmi_t *)0)->comment);
	return transData(ptr, &mr_info[num], (uint8_t)WRITE_REG_SHMI);
}

/***********************************
 *********        UF        ********
 ***********************************/
int getUf(void *ptr, uint16_t num) {
	uf_info[num].offset = 0;
	uf_info[num].size = sizeof(uf_shmi_t);
	return transData(ptr, &uf_info[num], (uint8_t)READ_REG_SHMI);
}
int setUf(void *ptr, uint16_t num) {
	uf_info[num].offset = 0;
	uf_info[num].size = sizeof(uf_shmi_t);
	return transData(ptr, &uf_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCoordinateUf(void *ptr, uint16_t num) {
	uf_info[num].offset = OFFSET_MEM(c,uf_shmi_t);
	uf_info[num].size = sizeof(((uf_shmi_t *)0)->c);
	return transData(ptr, &uf_info[num], (uint8_t)READ_REG_SHMI);
}
int setCoordinateUf(void *ptr, uint16_t num) {
	uf_info[num].offset = OFFSET_MEM(c,uf_shmi_t);
	uf_info[num].size = sizeof(((uf_shmi_t *)0)->c);
	return transData(ptr, &uf_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdUf(void *ptr, uint16_t num) {
	uf_info[num].offset = OFFSET_MEM(id,uf_shmi_t);
	uf_info[num].size = sizeof(((uf_shmi_t *)0)->id);
	return transData(ptr, &uf_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdUf(void *ptr, uint16_t num) {
	uf_info[num].offset = OFFSET_MEM(id,uf_shmi_t);
	uf_info[num].size = sizeof(((uf_shmi_t *)0)->id);
	return transData(ptr, &uf_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentUf(void *ptr, uint16_t num) {
	uf_info[num].offset = OFFSET_MEM(comment,uf_shmi_t);
	uf_info[num].size = sizeof(((uf_shmi_t *)0)->comment);
	return transData(ptr, &uf_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentUf(void *ptr, uint16_t num) {
	uf_info[num].offset = OFFSET_MEM(comment,uf_shmi_t);
	uf_info[num].size = sizeof(((uf_shmi_t *)0)->comment);
	return transData(ptr, &uf_info[num], (uint8_t)WRITE_REG_SHMI);
}

/***********************************
 *********        TF        ********
 ***********************************/
int getTf(void *ptr, uint16_t num) {
	tf_info[num].offset = 0;
	tf_info[num].size = sizeof(tf_shmi_t);
	return transData(ptr, &tf_info[num], (uint8_t)READ_REG_SHMI);
}
int setTf(void *ptr, uint16_t num) {
	tf_info[num].offset = 0;
	tf_info[num].size = sizeof(tf_shmi_t);
	return transData(ptr, &tf_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCoordinateTf(void *ptr, uint16_t num) {
	tf_info[num].offset = OFFSET_MEM(c,tf_shmi_t);
	tf_info[num].size = sizeof(((tf_shmi_t *)0)->c);
	return transData(ptr, &tf_info[num], (uint8_t)READ_REG_SHMI);
}
int setCoordinateTf(void *ptr, uint16_t num) {
	tf_info[num].offset = OFFSET_MEM(c,tf_shmi_t);
	tf_info[num].size = sizeof(((tf_shmi_t *)0)->c);
	return transData(ptr, &tf_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdTf(void *ptr, uint16_t num) {
	tf_info[num].offset = OFFSET_MEM(id,tf_shmi_t);
	tf_info[num].size = sizeof(((tf_shmi_t *)0)->id);
	return transData(ptr, &tf_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdTf(void *ptr, uint16_t num) {
	tf_info[num].offset = OFFSET_MEM(id,tf_shmi_t);
	tf_info[num].size = sizeof(((tf_shmi_t *)0)->id);
	return transData(ptr, &tf_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentTf(void *ptr, uint16_t num) {
	tf_info[num].offset = OFFSET_MEM(comment,tf_shmi_t);
	tf_info[num].size = sizeof(((tf_shmi_t *)0)->comment);
	return transData(ptr, &tf_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentTf(void *ptr, uint16_t num) {
	tf_info[num].offset = OFFSET_MEM(comment,tf_shmi_t);
	tf_info[num].size = sizeof(((tf_shmi_t *)0)->comment);
	return transData(ptr, &tf_info[num], (uint8_t)WRITE_REG_SHMI);
}

/***********************************
 *********        PL        ********
 ***********************************/
int getPl(void *ptr, uint16_t num) {
	pl_info[num].offset = 0;
	pl_info[num].size = sizeof(pl_shmi_t);
	return transData(ptr, &pl_info[num], (uint8_t)READ_REG_SHMI);
}
int setPl(void *ptr, uint16_t num) {
	pl_info[num].offset = 0;
	pl_info[num].size = sizeof(pl_shmi_t);
	return transData(ptr, &pl_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getPosePl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(pose,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->pose);
	return transData(ptr, &pl_info[num], (uint8_t)READ_REG_SHMI);
}
int setPosePl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(pose,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->pose);
	return transData(ptr, &pl_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdPalletPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(pallet,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->pallet);
	return transData(ptr, &pl_info[num], (uint8_t)READ_REG_SHMI);
}
int setPalletPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(pallet,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->pallet);
	return transData(ptr, &pl_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getFlagPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(flag,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->flag);
	return transData(ptr, &pl_info[num], (uint8_t)READ_REG_SHMI);
}
int setFlagPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(flag,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->flag);
	return transData(ptr, &pl_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getIdPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(id,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->id);
	return transData(ptr, &pl_info[num], (uint8_t)READ_REG_SHMI);
}
int setIdPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(id,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->id);
	return transData(ptr, &pl_info[num], (uint8_t)WRITE_REG_SHMI);
}

int getCommentPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(comment,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->comment);
	return transData(ptr, &pl_info[num], (uint8_t)READ_REG_SHMI);
}
int setCommentPl(void *ptr, uint16_t num) {
	pl_info[num].offset = OFFSET_MEM(comment,pl_shmi_t);
	pl_info[num].size = sizeof(((pl_shmi_t *)0)->comment);
	return transData(ptr, &pl_info[num], (uint8_t)WRITE_REG_SHMI);
}
