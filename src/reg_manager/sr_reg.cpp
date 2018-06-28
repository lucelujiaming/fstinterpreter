#include "sr_reg.h"
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <sstream>


using namespace std;
using namespace fst_reg;


SrReg::SrReg(int size, std::string file_dir):
    BaseReg(REG_TYPE_SR, size), file_path_(file_dir)
{
    data_list_.resize(size + 1);    // id=0 is not used, id start from 1
    // check if sr_reg.yaml exist? if not, create a new one
    file_path_.append("/sr_reg.yaml");
    if(access(file_path_.c_str(), 0) != 0)
    {
        if(!createYaml())
        {
            setReady(false);
            return;
        }
    }
    // read yaml, load data to data_list_ and reg_list_
    if(!readAllRegDataFromYaml())
    {
        setReady(false);
    }
    else
    {
        setReady(true);
    }
}

SrReg::~SrReg()
{

}

bool SrReg::addReg(void* data_ptr)
{
    if(data_ptr == NULL)
    {
        return false;
    }

    SrRegData* reg_ptr = reinterpret_cast<SrRegData*>(data_ptr);
    if(!isAddInputValid(reg_ptr->id))
    {
		printf("isAddInputValid: id = %d, comment = %s\n", reg_ptr->id, reg_ptr->comment);
        return false;
    }
    BaseRegData reg_data;
    packAddRegData(reg_data, reg_ptr->id, reg_ptr->comment);
    if(!setRegList(reg_data))
    {
		printf("setRegList: id = %d, comment = %s\n", reg_ptr->id, reg_ptr->comment);
        return false;
    }
    data_list_[reg_data.id] = reg_ptr->value.substr(0, MAX_SR_REG_STRING_LENGTH);
    return writeRegDataToYaml(reg_data, data_list_[reg_data.id]);
}

bool SrReg::deleteReg(int id)
{
    if(!isDeleteInputValid(id))
    {
        return false;
    }

    BaseRegData reg_data;
    packDeleteRegData(reg_data, id);
    if(!setRegList(reg_data))
    {
        return false;
    }
    data_list_[id].empty();
    return writeRegDataToYaml(reg_data, data_list_[id]);
}

bool SrReg::getReg(int id, void* data_ptr)
{
    if(!isGetInputValid(id))
    {
	    printf("SrReg::getReg isGetInputValid failed at %d\n", id);
        return false;
    }

    SrRegData* reg_ptr = reinterpret_cast<SrRegData*>(data_ptr);
    BaseRegData reg_data;
    if(!getRegList(id, reg_data))
    {
	    printf("SrReg::getReg getRegList failed at %d\n", id);
        return false;
    }
    reg_ptr->id = reg_data.id;
    memcpy(reg_ptr->comment, reg_data.comment, MAX_REG_COMMENT_LENGTH * sizeof(char));
    reg_ptr->value = data_list_[id];
    return true;
}

bool SrReg::setReg(void* data_ptr)
{
    if(data_ptr == NULL)
    {
        return false;
    }

    SrRegData* reg_ptr = reinterpret_cast<SrRegData*>(data_ptr);
    if(!isSetInputValid(reg_ptr->id))
    {
        return false;
    }
	if(strlen(reg_ptr->comment) == 0)
    {
        strcpy(reg_ptr->comment, "EMPTY");
	    printf("MrReg::setReg fill reg_ptr->comment = %s\n", reg_ptr->comment);
	}
        
    BaseRegData reg_data;
    packSetRegData(reg_data, reg_ptr->id, reg_ptr->comment);
    if(!setRegList(reg_data))
    {
        return false;
    }
    data_list_[reg_data.id] = reg_ptr->value.substr(0, MAX_SR_REG_STRING_LENGTH);
    return writeRegDataToYaml(reg_data, data_list_[reg_data.id]);
}

SrReg::SrReg():
    BaseReg(REG_TYPE_INVALID, 0)
{

}

bool SrReg::createYaml()
{
    std::ofstream fd;
    fd.open(file_path_.c_str(), std::ios::out);
    if(!fd.is_open())
    {
        return false;
    }
    fd.close();
    
    param_.loadParamFile(file_path_.c_str());
    for(int i = 1; i < data_list_.size(); ++i)
    {
        std::string reg_path = getRegPath(i);
        param_.setParam(reg_path + "/id", i);
        param_.setParam(reg_path + "/is_valid", false);
        param_.setParam(reg_path + "/value", "empty");
        param_.setParam(reg_path + "/comment", "empty");
    }
    return param_.dumpParamFile(file_path_.c_str());
}

bool SrReg::readAllRegDataFromYaml()
{
    param_.loadParamFile(file_path_.c_str());
    for(int i = 1; i < data_list_.size(); ++i)
    {
        std::string reg_path = getRegPath(i);
        BaseRegData base_data;
        std::string comment;
        param_.getParam(reg_path + "/id", base_data.id);
        param_.getParam(reg_path + "/is_valid", base_data.is_valid);
        param_.getParam(reg_path + "/comment", comment);
        comment.resize(MAX_REG_COMMENT_LENGTH - 1, 0);
        memcpy(base_data.comment, comment.c_str(), MAX_REG_COMMENT_LENGTH);
        base_data.is_changed = true;
        if(!setRegList(base_data))
        {
            return false;
        }
        param_.getParam(reg_path + "/value", data_list_[i]);        
    }
    return true;
}

bool SrReg::writeRegDataToYaml(const BaseRegData& base_data, const std::string& data)
{
    std::string reg_path = getRegPath(base_data.id);
    param_.setParam(reg_path + "/id", base_data.id);
    param_.setParam(reg_path + "/is_valid", base_data.is_valid);
    param_.setParam(reg_path + "/comment", base_data.comment);
    param_.setParam(reg_path + "/value", data);
    return param_.dumpParamFile(file_path_.c_str());
}

std::string SrReg::getRegPath(int reg_id)
{
    std::string id_str;
    std::stringstream stream;
    stream << reg_id;
    stream >> id_str;
    return (std::string("SrReg") + id_str);
}


