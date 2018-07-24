#ifndef REG_MANAGER_INTERFACE_WRAPPER_H
#define REG_MANAGER_INTERFACE_WRAPPER_H

#ifndef WIN32
#include "reg_manager/reg_manager_interface.h"
using namespace fst_reg ;
#endif
#include "reg_manager/forsight_registers_manager.h"

#define REGSITER_NAMES   "pr;sr;r;mr;uf;tf;pl"

void load_register_data();

/**********************
 ********* PR *********
 **********************/

bool reg_manager_interface_getPr(PrRegData *ptr, uint16_t num);
bool reg_manager_interface_setPr(PrRegData *ptr, uint16_t num);
bool reg_manager_interface_delPr(uint16_t num);

/*
 * The operated object is an individual member of PR.
 */
bool reg_manager_interface_getPosePr(PoseEuler *ptr, uint16_t num);
bool reg_manager_interface_setPosePr(PoseEuler *ptr, uint16_t num);

bool reg_manager_interface_getJointPr(Joint *ptr, uint16_t num);
bool reg_manager_interface_setJointPr(Joint *ptr, uint16_t num);

bool reg_manager_interface_getTypePr(int *ptr, uint16_t num);
bool reg_manager_interface_setTypePr(int *ptr, uint16_t num);

bool reg_manager_interface_getIdPr(int *ptr, uint16_t num);
bool reg_manager_interface_setIdPr(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentPr(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentPr(char *ptr, uint16_t num);

/**********************
 ********* SR *********
 **********************/

bool reg_manager_interface_getSr(SrRegData *ptr, uint16_t num);
bool reg_manager_interface_setSr(SrRegData *ptr, uint16_t num);
bool reg_manager_interface_delSr(uint16_t num);

bool reg_manager_interface_getValueSr(string &strVal, uint16_t num);
bool reg_manager_interface_setValueSr(string &strVal, uint16_t num);

bool reg_manager_interface_getIdSr(int *ptr, uint16_t num);
bool reg_manager_interface_setIdSr(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentSr(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentSr(char *ptr, uint16_t num);

/**********************
 ********* R **********
 **********************/
bool reg_manager_interface_getR(RRegData *ptr, uint16_t num);
bool reg_manager_interface_setR(RRegData *ptr, uint16_t num);
bool reg_manager_interface_delR(uint16_t num);

bool reg_manager_interface_getValueR(double *ptr, uint16_t num);
bool reg_manager_interface_setValueR(double *ptr, uint16_t num);

bool reg_manager_interface_getIdR(int *ptr, uint16_t num);
bool reg_manager_interface_setIdR(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentR(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentR(char *ptr, uint16_t num);

/**********************
 ********* MR *********
 **********************/
bool reg_manager_interface_getMr(MrRegData *ptr, uint16_t num);
bool reg_manager_interface_setMr(MrRegData *ptr, uint16_t num);
bool reg_manager_interface_delMr(uint16_t num);
 
bool reg_manager_interface_getValueMr(int *ptr, uint16_t num);
bool reg_manager_interface_setValueMr(int *ptr, uint16_t num);

bool reg_manager_interface_getIdMr(int *ptr, uint16_t num);
bool reg_manager_interface_setIdMr(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentMr(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentMr(char *ptr, uint16_t num);

/**********************
 ********* HR *********
 **********************/
	
bool reg_manager_interface_getHr(HrRegData *ptr, uint16_t num);
bool reg_manager_interface_setHr(HrRegData *ptr, uint16_t num);
bool reg_manager_interface_delHr(uint16_t num);

/*
 * The operated object is an individual member of HR.
 */
bool reg_manager_interface_getPoseHr(PoseEuler *ptr, uint16_t num);
bool reg_manager_interface_setPoseHr(PoseEuler *ptr, uint16_t num);

bool reg_manager_interface_getJointHr(Joint *ptr, uint16_t num);
bool reg_manager_interface_setJointHr(Joint *ptr, uint16_t num);

bool reg_manager_interface_getIdHr(int *ptr, uint16_t num);
bool reg_manager_interface_setIdHr(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentHr(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentHr(char *ptr, uint16_t num);


/**********************
 ********* UF *********
 **********************/
bool reg_manager_interface_getUf(void *ptr, uint16_t num);
bool reg_manager_interface_setUf(void *ptr, uint16_t num);

bool reg_manager_interface_getCoordinateUf(void *ptr, uint16_t num);
bool reg_manager_interface_setCoordinateUf(void *ptr, uint16_t num);

bool reg_manager_interface_getIdUf(int *ptr, uint16_t num);
bool reg_manager_interface_setIdUf(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentUf(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentUf(char *ptr, uint16_t num);

/**********************
 ********* TF *********
 **********************/
bool reg_manager_interface_getTf(void *ptr, uint16_t num);
bool reg_manager_interface_setTf(void *ptr, uint16_t num);

bool reg_manager_interface_getCoordinateTf(void *ptr, uint16_t num);
bool reg_manager_interface_setCoordinateTf(void *ptr, uint16_t num);

bool reg_manager_interface_getIdTf(int *ptr, uint16_t num);
bool reg_manager_interface_setIdTf(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentTf(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentTf(char *ptr, uint16_t num);

/**********************
 ********* PL *********
 **********************/
bool reg_manager_interface_getPl(void *ptr, uint16_t num);
bool reg_manager_interface_setPl(void *ptr, uint16_t num);

bool reg_manager_interface_getPosePl(PoseEuler* pose, int index);
bool reg_manager_interface_setPosePl(PoseEuler* pose, int index);

bool reg_manager_interface_getPalletPl(pl_t *ptr, uint16_t num);
bool reg_manager_interface_setPalletPl(pl_t *ptr, uint16_t num);

bool reg_manager_interface_getFlagPl(int *ptr, uint16_t num);
bool reg_manager_interface_setFlagPl(int *ptr, uint16_t num);

bool reg_manager_interface_getIdPl(int *ptr, uint16_t num);
bool reg_manager_interface_setIdPl(int *ptr, uint16_t num);

bool reg_manager_interface_getCommentPl(char *ptr, uint16_t num);
bool reg_manager_interface_setCommentPl(char *ptr, uint16_t num);

std::vector<BaseRegData> reg_manager_interface_read_valid_pr_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_sr_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_r_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_mr_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_hr_lst(int start_id, int size);

#endif
