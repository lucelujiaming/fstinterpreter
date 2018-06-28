#include "base_reg.h"
#include <cstring>


using namespace std;
using namespace fst_reg;


BaseReg::BaseReg(RegType type, int size):
    type_(type), is_ready_(false)
{
    reg_list_.resize(size + 1); // id=0 is not used, id start from 1
}

BaseReg::~BaseReg()
{

}

std::vector<BaseRegData> BaseReg::getChangedIdList(int start_id, int size)
{
    std::vector<BaseRegData> list;
    int end_id = start_id + size;
    start_id = (start_id <= 0 ? 1 : start_id);
    end_id = (end_id < reg_list_.size() ? end_id : reg_list_.size());
    for(int i = start_id; i < end_id; ++i)
    {
        if(reg_list_[i].is_changed)
        {
            list.push_back(reg_list_[i]);
            reg_list_[i].is_changed = false;
        }
    }
    return list;
}

std::vector<BaseRegData> BaseReg::getValidIdList(int start_id, int size)
{
    std::vector<BaseRegData> list;
    int end_id = start_id + size;
    start_id = (start_id <= 0 ? 1 : start_id);
    end_id = (end_id < reg_list_.size() ? end_id : reg_list_.size());
    for(int i = start_id; i < end_id; ++i)
    {
        if(reg_list_[i].is_valid)
        {
            list.push_back(reg_list_[i]);
        }
    }
    return list;
}

RegType BaseReg::getRegType()
{
    return type_;
}

void BaseReg::setReady(bool is_ready)
{
    is_ready_ = is_ready;
}

bool BaseReg::isReady()
{
    return is_ready_;
}

bool BaseReg::isValid(int id)
{
    if(id <= 0
        || id >= reg_list_.size())
    {
        return false;
    }
    else
    {
        return reg_list_[id].is_valid;
    }
}

bool BaseReg::setValid(int id, bool is_valid)
{
    if(id <= 0
        || id >= reg_list_.size())
    {
        return false;
    }
    else
    {
        reg_list_[id].is_valid = is_valid;
        return true;
    }
}

bool BaseReg::setComment(int id, const char comment[MAX_REG_COMMENT_LENGTH])
{
    if(id <= 0
        || id >= reg_list_.size())
    {
        return false;
    }
    else
    {
        memcpy(reg_list_[id].comment, comment, MAX_REG_COMMENT_LENGTH * sizeof(char));
        return true;
    }
}

char* BaseReg::getComment(int id)
{
    if(id <= 0
        || id >= reg_list_.size())
    {
        return NULL;
    }
    else
    {
        return reg_list_[id].comment;
    }
}

bool BaseReg::setRegList(BaseRegData& data)
{
    if(data.id <= 0
        || data.id >= reg_list_.size())
    {
        return false;
    }
    else
    {
        reg_list_[data.id].id = data.id;
        reg_list_[data.id].is_valid = data.is_valid;
        reg_list_[data.id].is_changed = data.is_changed;
        memcpy(reg_list_[data.id].comment, data.comment, MAX_REG_COMMENT_LENGTH * sizeof(char));
        return true;
    }
}

bool BaseReg::getRegList(int id, BaseRegData& data)
{
    if(id <= 0
        || id >= reg_list_.size())
    {
	    printf("BaseReg::getRegList failed at %d\n", id);
        return false;
    }
    else
    {
        data.id = reg_list_[id].id;
        data.is_valid = reg_list_[id].is_valid;
        data.is_changed = reg_list_[id].is_changed;
        memcpy(data.comment, reg_list_[id].comment, MAX_REG_COMMENT_LENGTH * sizeof(char));
	    printf("BaseReg::getRegList over at %d\n", id);
        return true;
    }
}

bool BaseReg::isAddInputValid(int id)
{
    if(id <= 0
        || id >= reg_list_.size()
        || reg_list_[id].is_valid)   // can't add some exist reg
    {
		printf("isAddInputValid: id = %d is inValid \n", id);
        return false;
    }
    else
    {
        return true;
    }
}

bool BaseReg::isDeleteInputValid(int id)
{
    if(id <= 0)
    {
	    printf("isDeleteInputValid id failed at %d\n", id);
        return false;
    }
    else if(id >= reg_list_.size())
    {
	    printf("isDeleteInputValid sizefailed at %d\n", id);
        return false;
    }
    else if(!reg_list_[id].is_valid)    // can't delete some none-existent reg
    {
	    printf("isDeleteInputValid is_valid failed at %d\n", id);
        return false;
    }
    else
    {
        return true;
    }
}

bool BaseReg::isSetInputValid(int id)
{
    return isDeleteInputValid(id);
}

bool BaseReg::isGetInputValid(int id)
{
    return isDeleteInputValid(id);
}

void BaseReg::packAddRegData(BaseRegData& data, int id, const char* comment_ptr)
{
    data.id = id;
    data.is_valid = true;
    data.is_changed = true;
    memcpy(data.comment, comment_ptr, MAX_REG_COMMENT_LENGTH * sizeof(char));
}

void BaseReg::packDeleteRegData(BaseRegData& data, int id)
{
    data.id = id;
    data.is_valid = false;
    data.is_changed = true;
    std::string comment("Not used");
    comment.resize(MAX_REG_COMMENT_LENGTH - 1, 0);
    memcpy(data.comment, comment.c_str(), MAX_REG_COMMENT_LENGTH);
}

void BaseReg::packSetRegData(BaseRegData& data, int id, const char* comment)
{
    packAddRegData(data, id, comment);
}

BaseReg::BaseReg()
{

}


