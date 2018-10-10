#ifndef INTERPRETER_REG_MANAGER_MR_REG_H
#define INTERPRETER_REG_MANAGER_MR_REG_H

#include "base_reg.h"
#include "parameter_manager/parameter_manager_param_group.h"


namespace fst_reg
{

#define MAX_MR_REG_VALUE 99999999


typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    int value;
}MrRegData;

class MrReg:public BaseReg
{
public:
    MrReg(int size, std::string file_dir);
    ~MrReg();

    virtual bool addReg(void* data_ptr);
    virtual bool deleteReg(int id);
    virtual bool getReg(int id, void* data_ptr);
    virtual bool setReg(void* data_ptr);
    
private:
    MrReg();
    bool createYaml();
    bool readAllRegDataFromYaml();
    bool writeRegDataToYaml(const BaseRegData& base_data, const int& data);
    std::string getRegPath(int reg_id);

    std::string file_path_;
    fst_parameter::ParamGroup param_;
    std::vector<int> data_list_;   
};

}


#endif

