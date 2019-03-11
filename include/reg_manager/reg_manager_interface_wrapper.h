#ifndef REG_MANAGER_INTERFACE_WRAPPER_H
#define REG_MANAGER_INTERFACE_WRAPPER_H

// #define USE_LOCAL_REG_MANAGER_INTERFACE

#ifndef WIN32

#include "process_comm.h"
using namespace fst_ctrl ;
extern fst_base::InterpreterClient* g_objRegManagerInterface;
extern fst_base::InterpreterServer* g_objInterpreterServer ;

#endif
#include "reg_manager/forsight_registers_manager.h"

#define REGSITER_NAMES   "pr;sr;uf;tf;pl"


bool load_register_data();

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
bool reg_manager_interface_getR(double *ptr, uint16_t num);
bool reg_manager_interface_setR(double *ptr, uint16_t num);

bool reg_manager_interface_getValueR(double *ptr, uint16_t num);
bool reg_manager_interface_setValueR(double *ptr, uint16_t num);

/**********************
 ********* MR *********
 **********************/
bool reg_manager_interface_getMr(int *ptr, uint16_t num);
bool reg_manager_interface_setMr(int *ptr, uint16_t num);
 
bool reg_manager_interface_getValueMr(int *ptr, uint16_t num);
bool reg_manager_interface_setValueMr(int *ptr, uint16_t num);

/**********************
 ********* MI **********
 **********************/
bool reg_manager_interface_getMI(int *ptr, uint16_t num);
bool reg_manager_interface_setMI(int *ptr, uint16_t num);

bool reg_manager_interface_getValueMI(int *ptr, uint16_t num);
bool reg_manager_interface_setValueMI(int *ptr, uint16_t num);

/**********************
 ********* MH **********
 **********************/
bool reg_manager_interface_getMH(int *ptr, uint16_t num);
bool reg_manager_interface_setMH(int *ptr, uint16_t num);

bool reg_manager_interface_getValueMH(int *ptr, uint16_t num);
bool reg_manager_interface_setValueMH(int *ptr, uint16_t num);

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

bool reg_manager_interface_getJoint(Joint &joint);
bool reg_manager_interface_getCart(PoseEuler &pos);
bool reg_manager_interface_cartToJoint(PoseEuler pos, Joint &joint);
bool reg_manager_interface_jointToCart(Joint joint, PoseEuler &pos);

#endif
