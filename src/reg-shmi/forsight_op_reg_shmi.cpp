#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif 

#include "reg-shmi/forsight_op_shmi.h"
#include "reg-shmi/forsight_regs_shmi.h"
#include "reg-shmi/forsight_op_regs_shmi.h"

struct shmi_info pr_info[PR_NUM_MAX];
struct shmi_info sr_info[SR_NUM_MAX];
struct shmi_info r_info[R_NUM_MAX];
struct shmi_info mr_info[MR_NUM_MAX];
struct shmi_info uf_info[UF_NUM_MAX];
struct shmi_info tf_info[TF_NUM_MAX];
struct shmi_info pl_info[TF_NUM_MAX];

#define OFFSET_MEM(mem,struction) (size_t)&(((struction *)0)->mem)

static int fdRegShmi;
static void *ptrRegShmi;

int initShmi(int size) {
	int err = OK_SHMI;
#ifdef WIN32
	FILE * errFile = fopen("reg_shmi", "w");
#else
	fdRegShmi = shm_open("reg_shmi", O_CREAT|O_RDWR, 00777);
	if(fdRegShmi < 0)
	{
		printf("REG_SHMI: init failed with = %d\n", fdRegShmi);
	}
	// printf("REG_SHMI: init shm_open(reg_shmi) with = %d\n", fdRegShmi);
#endif
	
#ifdef WIN32
	ptrRegShmi = malloc(REG_SHMI_SIZE);
	fdRegShmi  = REG_SHMI_SIZE ; 
#else
    ftruncate(fdRegShmi, size);
	ptrRegShmi = mmap(NULL, REG_SHMI_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, fdRegShmi, 0);
#endif
	
	return err;
	
}

void generateFakeData()
{
    int i = 0 ;
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		pr_info[i].ptr = malloc(sizeof(pr_shmi_t));
		pr_shmi_t * pr = (pr_shmi_t *)(pr_info[i].ptr) ;
		pr->pose.position.x = pr->pose.position.y = pr->pose.position.z = 11.1 + i;
		pr->pose.orientation.a = pr->pose.orientation.b = pr->pose.orientation.c = 22.2 + i;
		pr->type = 1 ; pr->id = 230 ; strcpy(pr->comment, "hello PR");
	}
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		sr_info[i].ptr = malloc(sizeof(sr_shmi_t));
		sr_shmi_t * sr = (sr_shmi_t *)(sr_info[i].ptr) ;
		strcpy(sr->value, "SR save string");
		sr->id = 1 ; strcpy(sr->comment, "hello PR");
	}
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		r_info[i].ptr = malloc(sizeof(r_shmi_t));
		r_shmi_t * r = (r_shmi_t *)(r_info[i].ptr) ;
		r->value = 43.1 + i;
		r->id = 1 ; strcpy(r->comment, "hello PR");
	}
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		mr_info[i].ptr = malloc(sizeof(mr_shmi_t));
		mr_shmi_t * mr = (mr_shmi_t *)(mr_info[i].ptr) ;
		mr->value = 123;
		mr->id = 1 ; strcpy(mr->comment, "hello MR");
	}
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		uf_info[i].ptr = malloc(sizeof(uf_shmi_t));
		uf_shmi_t * uf = (uf_shmi_t *)(uf_info[i].ptr) ;
		uf->c.position.x = uf->c.position.y = uf->c.position.z = 11.1 + i;
		uf->c.orientation.a = uf->c.orientation.b = uf->c.orientation.c = 22.2 + i;
		uf->id = 1 ; strcpy(uf->comment, "hello MR");
	}
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		tf_info[i].ptr = malloc(sizeof(tf_shmi_t));
		tf_shmi_t * tf = (tf_shmi_t *)(tf_info[i].ptr) ;
		tf->c.position.x = tf->c.position.y = tf->c.position.z = 11.1 + i;
		tf->c.orientation.a = tf->c.orientation.b = tf->c.orientation.c = 22.2 + i;
		tf->id = 1 ; strcpy(tf->comment, "hello MR");
	}
	for(i = 0 ; i < PR_NUM_MAX ; i++)
	{
		pl_info[i].ptr = malloc(sizeof(pl_shmi_t));
		pl_shmi_t * pl = (pl_shmi_t *)(pl_info[i].ptr) ;
		pl->pose.position.x = pl->pose.position.y = pl->pose.position.z = 11.1 + i;
		pl->pose.orientation.a = pl->pose.orientation.b = pl->pose.orientation.c = 22.2 + i;
		pl->pallet.row = pl->pallet.column = pl->pallet.layer = 5 + i ;
		pl->id = 1 ; strcpy(pl->comment, "hello MR");
	}
}


int initRegShmi()
{
	int cnt ;
	int err = OK_SHMI;
	err = initShmi(1024);
	if(err < 0)
	{
		printf("Error: initShmi failed, err = %d\n", err);
		return err;
	}
	/* init pr */
	for(cnt = 0; cnt < PR_NUM_MAX; cnt++)
	{
#ifdef WIN32
		pr_info[cnt].ptr = ((char *)ptrRegShmi + PR_OFFSET + sizeof(pr_shmi_t) *cnt);
#else
		pr_info[cnt].ptr = (void *)(ptrRegShmi + PR_OFFSET + sizeof(pr_shmi_t) *cnt);
#endif
		pr_info[cnt].offset = 0;
		pr_info[cnt].size = 0;
		pr_info[cnt].pts.read = 1;
		pr_info[cnt].pts.write = 0;
		pr_info[cnt].pts.turn = 0;
	}
	/* init sr */
	for(cnt = 0; cnt < SR_NUM_MAX; cnt++)
	{
#ifdef WIN32
		sr_info[cnt].ptr = (char *)ptrRegShmi + SR_OFFSET + sizeof(sr_shmi_t) *cnt;
#else
		sr_info[cnt].ptr = ptrRegShmi + SR_OFFSET + sizeof(sr_shmi_t) *cnt;
#endif
		sr_info[cnt].offset = 0;
		sr_info[cnt].size = 0;
		sr_info[cnt].pts.read = 1;
		sr_info[cnt].pts.write = 0;
		sr_info[cnt].pts.turn = 0;
	}
	/* init r */
	for(cnt = 0; cnt < R_NUM_MAX; cnt++)
	{
#ifdef WIN32
		r_info[cnt].ptr = (char *)ptrRegShmi + R_OFFSET + sizeof(r_shmi_t) *cnt;
#else
		r_info[cnt].ptr = ptrRegShmi + R_OFFSET + sizeof(r_shmi_t) *cnt;
#endif
		r_info[cnt].offset = 0;
		r_info[cnt].size = 0;
		r_info[cnt].pts.read = 1;
		r_info[cnt].pts.write = 0;
		r_info[cnt].pts.turn = 0;
	}
	/* init mr */
	for(cnt = 0; cnt < MR_NUM_MAX; cnt++)
	{
#ifdef WIN32
		mr_info[cnt].ptr = (char *)ptrRegShmi + MR_OFFSET + sizeof(mr_shmi_t) *cnt;
#else
		mr_info[cnt].ptr = ptrRegShmi + MR_OFFSET + sizeof(mr_shmi_t) *cnt;
#endif
		mr_info[cnt].offset = 0;
		mr_info[cnt].size = 0;
		mr_info[cnt].pts.read = 1;
		mr_info[cnt].pts.write = 0;
		mr_info[cnt].pts.turn = 0;
	}
	/* init uf */
	for(cnt = 0; cnt < UF_NUM_MAX; cnt++)
	{
#ifdef WIN32
		uf_info[cnt].ptr = (char *)ptrRegShmi + UF_OFFSET + sizeof(uf_shmi_t) *cnt;
#else
		uf_info[cnt].ptr = ptrRegShmi + UF_OFFSET + sizeof(uf_shmi_t) *cnt;
#endif
		uf_info[cnt].offset = 0;
		uf_info[cnt].size = 0;
		uf_info[cnt].pts.read = 1;
		uf_info[cnt].pts.write = 0;
		uf_info[cnt].pts.turn = 0;
	}
	/* init tf */
	for(cnt = 0; cnt < TF_NUM_MAX; cnt++)
	{
#ifdef WIN32
		tf_info[cnt].ptr = (char *)ptrRegShmi + TF_OFFSET + sizeof(tf_shmi_t) *cnt;
#else
		tf_info[cnt].ptr = ptrRegShmi + TF_OFFSET + sizeof(tf_shmi_t) *cnt;
#endif
		tf_info[cnt].offset = 0;
		tf_info[cnt].size = 0;
		tf_info[cnt].pts.read = 1;
		tf_info[cnt].pts.write = 0;
		tf_info[cnt].pts.turn = 0;
	}
	/* init pl */
/*
	for(cnt = 0; cnt < PL_NUM_MAX; cnt++)
	{
#ifdef WIN32
		pl_info[cnt].ptr = (char *)ptrRegShmi + PL_OFFSET + sizeof(pl_shmi_t) *cnt;
#else
		pl_info[cnt].ptr = ptrRegShmi + PL_OFFSET + sizeof(pl_shmi_t) *cnt;
#endif
		pl_info[cnt].offset = 0;
		pl_info[cnt].size = 0;
		pl_info[cnt].pts.read = 1;
		pl_info[cnt].pts.write = 0;
		pl_info[cnt].pts.turn = 0;
	}
*/
	printf("init all register over\n");
	
	return err;
}

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

int getPalletPl(void *ptr, uint16_t num) {
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
