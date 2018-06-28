#ifndef SR_REG_H
#define SR_REG_H

#include "base_reg.h"
#include <string>
#include "parameter_manager/parameter_manager_param_group.h"


namespace fst_reg
{

enum{MAX_SR_REG_STRING_LENGTH = 254,};


typedef struct
{
    int id;
    char comment[MAX_REG_COMMENT_LENGTH];
    std::string value;
}SrRegData;

class SrReg:public BaseReg
{
public:
    SrReg(int size, std::string file_dir);
    ~SrReg();

    virtual bool addReg(void* data_ptr);
    virtual bool deleteReg(int id);
    virtual bool getReg(int id, void* data_ptr);
    virtual bool setReg(void* data_ptr);
    
private:
    SrReg();
    bool createYaml();
    bool readAllRegDataFromYaml();
    bool writeRegDataToYaml(const BaseRegData& base_data, const std::string& data);
    std::string getRegPath(int reg_id);
    
    std::string file_path_;
    fst_parameter::ParamGroup param_;    
    std::vector<std::string> data_list_;   
};

}


#endif

