#ifndef FORSIGHT_REGISTERS_MANAGER_H
#define FORSIGHT_REGISTERS_MANAGER_H
#include "forsight_inter_control.h"
#include "forsight_basint.h"

#ifdef USE_FORSIGHT_REGISTERS_MANAGER
#ifndef WIN32
#include "reg_manager_interface_wrapper.h"
using namespace fst_ctrl ;
#endif
#include "reg_manager/forsight_registers_manager.h"

#endif

#define REGSITER_NAMES   "pr;sr;r;mr;mi;mh;uf;tf;pl"

int forgesight_registers_manager_get_register(
							struct thread_control_block* objThreadCntrolBlock, 
							char *name, eval_value * value);
int forgesight_registers_manager_set_register(
							struct thread_control_block* objThreadCntrolBlock, 
							char *name, eval_value * value);
// int forgesight_registers_manager_read_reg(RegMap & reg);
// int forgesight_registers_manager_mod_reg(RegMap & reg);

std::vector<BaseRegData> forgesight_read_valid_pr_lst(int start_id, int size);
std::vector<BaseRegData> forgesight_read_valid_sr_lst(int start_id, int size);
std::vector<BaseRegData> forgesight_read_valid_r_lst(int start_id, int size);
std::vector<BaseRegData> forgesight_read_valid_mr_lst(int start_id, int size);
std::vector<BaseRegData> forgesight_read_valid_hr_lst(int start_id, int size);

int forgesight_registers_manager_get_joint(Joint &joint);
int forgesight_registers_manager_get_cart(PoseEuler &pos);
int forgesight_registers_manager_cartToJoint(PoseEuler pos, Joint &joint);
int forgesight_registers_manager_jointToCart(Joint joint, PoseEuler &pos);

#endif
