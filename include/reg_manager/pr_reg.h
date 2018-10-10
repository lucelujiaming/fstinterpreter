#ifndef INTERPRETER_REG_MANAGER_PR_REG_H
#define INTERPRETER_REG_MANAGER_PR_REG_H

#include "base_reg.h"
#include "fst_datatype.h"
#include "parameter_manager/parameter_manager_param_group.h"


namespace fst_reg
{

#define MAX_PR_REG_POS_VALUE 99999999.999

enum
{
    MAX_PR_AXIS_NUM = 6,
    MAX_PR_EXT_AXIS_NUM = 3,
};

typedef enum
{
    POS_TYPE_CARTESIAN = 0,
    POS_TYPE_JOINT = 1,
}PosType;

typedef struct
{
    fst_controller::PoseEuler cartesian_pos;
    double joint_pos[MAX_PR_AXIS_NUM];
    int pos_type;
    double ext_pos[MAX_PR_EXT_AXIS_NUM];   // not used
    int group_id;           // not used
    bool posture[4];        // not used
    int multiturn[3];       // not used
}PrValue;

typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    PrValue value;
}PrRegData;

class PrReg:public BaseReg
{
public:
    PrReg(int size, std::string file_dir);
    ~PrReg();

    virtual bool addReg(void* data_ptr);
    virtual bool deleteReg(int id);
    virtual bool getReg(int id, void* data_ptr);
    virtual bool setReg(void* data_ptr);
    
private:
    PrReg();
    bool isOutOfPosLimit(const PrValue& data);
    bool createYaml();
    bool readAllRegDataFromYaml();
    bool writeRegDataToYaml(const BaseRegData& base_data, const PrValue& data);
    std::string getRegPath(int reg_id);

    std::string file_path_;
    fst_parameter::ParamGroup param_;
    std::vector<PrValue> data_list_;   
};

}


#endif

