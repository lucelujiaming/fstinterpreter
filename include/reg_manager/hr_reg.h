#ifndef INTERPRETER_REG_MANAGER_HR_REG_H
#define INTERPRETER_REG_MANAGER_HR_REG_H

#include "base_reg.h"
#include "fst_datatype.h"
#include "parameter_manager/parameter_manager_param_group.h"


namespace fst_reg
{

#define MAX_HR_REG_POS_VALUE 99999999.999

enum
{
    MAX_HR_AXIS_NUM = 6,
    MAX_HR_EXT_AXIS_NUM = 3,
};

typedef struct
{
    double joint_pos[MAX_HR_AXIS_NUM];
    double ext_pos[MAX_HR_EXT_AXIS_NUM];   // not used
    int group_id;           // not used
    bool posture[4];        // not used
    int multiturn[3];       // not used
}HrValue;

typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    HrValue value;
}HrRegData;

class HrReg:public BaseReg
{
public:
    HrReg(int size, std::string file_dir);
    ~HrReg();

    virtual bool addReg(void* data_ptr);
    virtual bool deleteReg(int id);
    virtual bool getReg(int id, void* data_ptr);
    virtual bool setReg(void* data_ptr);
    
private:
    HrReg();
    bool isOutOfPosLimit(const HrValue& data);
    bool createYaml();
    bool readAllRegDataFromYaml();
    bool writeRegDataToYaml(const BaseRegData& base_data, const HrValue& data);
    std::string getRegPath(int reg_id);

    std::string file_path_;
    fst_parameter::ParamGroup param_;
    std::vector<HrValue> data_list_;   
};

}


#endif

