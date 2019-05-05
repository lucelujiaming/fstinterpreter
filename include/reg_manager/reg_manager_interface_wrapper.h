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

#define REGSITER_NAMES   "pr;uf;tf;pl"


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

/**********************
 ********* SR *********
 **********************/

bool reg_manager_interface_getSr(string &ptr, uint16_t num);
bool reg_manager_interface_setSr(string &ptr, uint16_t num);

bool reg_manager_interface_getValueSr(string &strVal, uint16_t num);
bool reg_manager_interface_setValueSr(string &strVal, uint16_t num);

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

std::vector<BaseRegData> reg_manager_interface_read_valid_pr_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_sr_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_r_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_mr_lst(int start_id, int size);
std::vector<BaseRegData> reg_manager_interface_read_valid_hr_lst(int start_id, int size);

bool reg_manager_interface_getJoint(Joint &joint);
bool reg_manager_interface_getCart(PoseEuler &pos);
bool reg_manager_interface_cartToJoint(PoseEuler pos, Joint &joint);
bool reg_manager_interface_jointToCart(Joint joint, PoseEuler &pos);
bool reg_manager_interface_getUserOpMode(int& mode);

#endif
