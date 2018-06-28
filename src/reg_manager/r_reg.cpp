#include "r_reg.h"
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <sstream>


using namespace std;
using namespace fst_reg;
using namespace fst_parameter;


RReg::RReg(int size, std::string file_dir):
    BaseReg(REG_TYPE_R, size), file_path_(file_dir)
{
    data_list_.resize(size + 1);    // id=0 is not used, id start from 1
    // check if r_reg.yaml exist? if not, create a new one
    file_path_.append("/r_reg.yaml");
	printf("RReg::RReg Enter %s\n", file_path_.c_str());
    if(access(file_path_.c_str(), 0) != 0)
    {
	    printf("RReg::RReg access %s\n", file_path_.c_str());
        if(!createYaml())
        {
	        printf("RReg::RReg createYaml %s\n", file_path_.c_str());
            setReady(false);
            return;
        }
    }
	printf("RReg::RReg readAllRegDataFromYaml \n");
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

RReg::~RReg()
{

}

bool RReg::addReg(void* data_ptr)
{
    if(data_ptr == NULL)
    {
        return false;
    }

    RRegData* reg_ptr = reinterpret_cast<RRegData*>(data_ptr);
    if(!isAddInputValid(reg_ptr->id)
        || reg_ptr->value > MAX_R_REG_VALUE
        || reg_ptr->value < -MAX_R_REG_VALUE)
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
    data_list_[reg_data.id] = reg_ptr->value;
    return writeRegDataToYaml(reg_data, data_list_[reg_data.id]);
}

bool RReg::deleteReg(int id)
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
    data_list_[id] = 0;
    return writeRegDataToYaml(reg_data, data_list_[id]);
}

bool RReg::getReg(int id, void* data_ptr)
{
    if(!isGetInputValid(id))
    {
	    printf("RReg::getReg isGetInputValid failed at %d\n", id);
        return false;
    }

    RRegData* reg_ptr = reinterpret_cast<RRegData*>(data_ptr);
    BaseRegData reg_data;
    if(!getRegList(id, reg_data))
    {
	    printf("RReg::getReg getRegList failed at %d\n", id);
        return false;
    }
    reg_ptr->id = reg_data.id;
	    printf("RReg::getReg id reg_ptr at %d\n", reg_ptr->id);
    memcpy(reg_ptr->comment, reg_data.comment, MAX_REG_COMMENT_LENGTH * sizeof(char));
	    printf("RReg::getReg comment reg_ptr at %s\n", reg_ptr->comment);
	    printf("RReg::getReg comment reg_ptr at %s\n", reg_data.comment);
    reg_ptr->value = data_list_[id];
	    printf("RReg::getReg value reg_ptr at %f\n", data_list_[id]);
    return true;
}

bool RReg::setReg(void* data_ptr)
{
    if(data_ptr == NULL)
    {
        return false;
    }

    RRegData* reg_ptr = reinterpret_cast<RRegData*>(data_ptr);
    if(!isSetInputValid(reg_ptr->id)
        || reg_ptr->value > MAX_R_REG_VALUE
        || reg_ptr->value < -MAX_R_REG_VALUE)
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
    data_list_[reg_data.id] = reg_ptr->value;
	    printf("RReg::setReg value reg_ptr at %f\n", data_list_[reg_data.id]);
    return writeRegDataToYaml(reg_data, data_list_[reg_data.id]);
}

RReg::RReg():
    BaseReg(REG_TYPE_INVALID, 0)
{

}

bool RReg::createYaml()
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
        param_.setParam(reg_path + "/value", (double)0);
        param_.setParam(reg_path + "/comment", "empty");
    }
    return param_.dumpParamFile(file_path_.c_str());
}

bool RReg::readAllRegDataFromYaml()
{
    param_.loadParamFile(file_path_.c_str());
    for(int i = 1; i < data_list_.size(); ++i)
    {
        std::string reg_path = getRegPath(i);
        BaseRegData base_data;
        std::string comment;
        param_.getParam(reg_path + "/id", base_data.id);
        param_.getParam(reg_path + "/is_valid", base_data.is_valid);
//		if(base_data.is_valid)
//			printf("RReg::readAllRegDataFromYaml at %d with %s\n", base_data.id, base_data.is_valid ? "TRUE" : "FALSE");
//		else
//			printf("RReg::readAllRegDataFromYaml at %d with %s\n", base_data.id, base_data.is_valid ? "TRUE" : "FALSE");
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

bool RReg::writeRegDataToYaml(const BaseRegData& base_data, const double& data)
{
    std::string reg_path = getRegPath(base_data.id);
	    printf("RReg::writeRegDataToYaml id at %d\n", base_data.id);
    param_.setParam(reg_path + "/id", base_data.id);
	    printf("RReg::writeRegDataToYaml is_valid at %d\n", base_data.is_valid);
    param_.setParam(reg_path + "/is_valid", base_data.is_valid);
	    printf("RReg::writeRegDataToYaml comment at %s\n", base_data.comment);
    param_.setParam(reg_path + "/comment", base_data.comment);
	    printf("RReg::writeRegDataToYaml data at %f\n", data);
    param_.setParam(reg_path + "/value", data);
    return param_.dumpParamFile(file_path_.c_str());
}

std::string RReg::getRegPath(int reg_id)
{
    std::string id_str;
    std::stringstream stream;
    stream << reg_id;
    stream >> id_str;
    return (std::string("RReg") + id_str);
}



