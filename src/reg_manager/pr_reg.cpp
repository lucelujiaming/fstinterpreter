#include "pr_reg.h"
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <sstream>


using namespace std;
using namespace fst_reg;
using namespace fst_controller;


PrReg::PrReg(int size, std::string file_dir):
    BaseReg(REG_TYPE_PR, size), file_path_(file_dir)
{
    data_list_.resize(size + 1);    // id=0 is not used, id start from 1
    // check if sr_reg.yaml exist? if not, create a new one
    file_path_.append("/pr_reg.yaml");
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

PrReg::~PrReg()
{

}

bool PrReg::addReg(void* data_ptr)
{
    if(data_ptr == NULL)
    {
        return false;
    }

    PrRegData* reg_ptr = reinterpret_cast<PrRegData*>(data_ptr);
    if(!isAddInputValid(reg_ptr->id)
        || isOutOfPosLimit(reg_ptr->value))
    {
		printf("addReg::isAddInputValid: id = %d, comment = %s\n", reg_ptr->id, reg_ptr->comment);
        return false;
    }
    BaseRegData reg_data;
    packAddRegData(reg_data, reg_ptr->id, reg_ptr->comment);
    if(!setRegList(reg_data))
    {
		printf("setRegList: id = %d, comment = %s\n", reg_ptr->id, reg_ptr->comment);
        return false;
    }
    memcpy(&data_list_[reg_data.id], &reg_ptr->value, sizeof(PrValue));

#if 0
	PrRegData objPrRegData = * reg_ptr;
		printf("setPr: id = %d, comment = %s\n", objPrRegData.id, objPrRegData.comment);
		printf("setPr: id = (%f, %f, %f, %f, %f, %f) \n", 
			objPrRegData.value.joint_pos[0], objPrRegData.value.joint_pos[1], 
			objPrRegData.value.joint_pos[2], objPrRegData.value.joint_pos[3], 
			objPrRegData.value.joint_pos[4], objPrRegData.value.joint_pos[5]);
		printf("setPr: id = (%f, %f, %f, %f, %f, %f) \n", 
			objPrRegData.value.cartesian_pos.position.x, objPrRegData.value.cartesian_pos.position.y, 
			objPrRegData.value.cartesian_pos.position.z, objPrRegData.value.cartesian_pos.orientation.a, 
			objPrRegData.value.cartesian_pos.orientation.b, objPrRegData.value.cartesian_pos.orientation.c);
#endif	
    return writeRegDataToYaml(reg_data, data_list_[reg_data.id]);
}

bool PrReg::deleteReg(int id)
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
    PrValue data;
    memset(&data, 0, sizeof(PrValue));
    memcpy(&data_list_[reg_data.id], &data, sizeof(PrValue));
    return writeRegDataToYaml(reg_data, data_list_[id]);
}

bool PrReg::getReg(int id, void* data_ptr)
{
    if(!isGetInputValid(id))
    {
	    printf("PrReg::getReg isGetInputValid failed at %d\n", id);
        return false;
    }

    PrRegData* reg_ptr = reinterpret_cast<PrRegData*>(data_ptr);
    BaseRegData reg_data;
    if(!getRegList(id, reg_data))
    {
	    printf("PrReg::getReg getRegList failed at %d\n", id);
        return false;
    }
	printf("PrReg::getReg getRegList at %d, %d with %s\n", 
		id, reg_data.id, reg_data.is_valid ? "TRUE" : "FALSE");
	
    reg_ptr->id = reg_data.id;
	    printf("PrReg::getReg getRegList reg_ptr at %d\n", reg_ptr->id);
    memcpy(reg_ptr->comment, reg_data.comment, MAX_REG_COMMENT_LENGTH * sizeof(char));
    memcpy(&reg_ptr->value, &data_list_[reg_data.id], sizeof(PrValue));
    return true;
}

bool PrReg::setReg(void* data_ptr)
{
    if(data_ptr == NULL)
    {
        return false;
    }

    PrRegData* reg_ptr = reinterpret_cast<PrRegData*>(data_ptr);
    if(!isSetInputValid(reg_ptr->id)
        || isOutOfPosLimit(reg_ptr->value))
    {
		printf("setReg::isSetInputValid: id = %d, comment = %s\n", reg_ptr->id, reg_ptr->comment);
        return false;
    }
	if(strlen(reg_ptr->comment) == 0)
    {
        strcpy(reg_ptr->comment, "EMPTY");
	    printf("MrReg::setReg fill reg_ptr->comment = %s\n", reg_ptr->comment);
	}
        
    BaseRegData reg_data;
    packSetRegData(reg_data, reg_ptr->id, reg_ptr->comment);
	printf("PrReg::setReg setRegList at %d with %s\n", reg_data.id, reg_data.is_valid ? "TRUE" : "FALSE");
    if(!setRegList(reg_data))
    {
	    printf("PrReg::setReg setRegList failed at %d\n", reg_data.id);
        return false;
    }
    memcpy(&data_list_[reg_data.id], &reg_ptr->value, sizeof(PrValue));
    return writeRegDataToYaml(reg_data, data_list_[reg_data.id]);
}

PrReg::PrReg():
    BaseReg(REG_TYPE_INVALID, 0)
{

}

bool PrReg::isOutOfPosLimit(const PrValue& data)
{
    switch(data.pos_type)
    {
        case POS_TYPE_CARTESIAN:
            if(data.cartesian_pos.position.x > MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.position.x < -MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.position.y > MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.position.y < -MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.position.z > MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.position.z < -MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.orientation.a > MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.orientation.a < -MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.orientation.b > MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.orientation.b < -MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.orientation.c > MAX_PR_REG_POS_VALUE
                || data.cartesian_pos.orientation.c < -MAX_PR_REG_POS_VALUE)
            {
				printf("isOutOfPosLimit::cartesian_pos return false \n");
                return true;
            }
            break;
        case POS_TYPE_JOINT:
            for(int i = 0; i < MAX_AXIS_NUM; ++i)
            {
                if(data.joint_pos[i] > MAX_PR_REG_POS_VALUE
                    || data.joint_pos[i] < -MAX_PR_REG_POS_VALUE)
                {
					printf("isOutOfPosLimit::joint_pos[%d]=%f return false \n", i, data.joint_pos[i]);
                    return true;
                }
            }
            break;
        default:
            return true;
    }

    for(int i = 0; i < MAX_EXT_AXIS_NUM; ++i)
    {
        if(data.ext_pos[i] > MAX_PR_REG_POS_VALUE
            || data.ext_pos[i] < -MAX_PR_REG_POS_VALUE)
        {
			printf("isOutOfPosLimit::ext_pos[%d]=%f return false \n", i, data.ext_pos[i]);
            return true;
        }
    }
    return false;
}

bool PrReg::createYaml()
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
        param_.setParam(reg_path + "/comment", "empty");
        
        std::string cartesian_path = reg_path + "/cartesian";
        std::string position_path = cartesian_path + "/position";
        param_.setParam(position_path + "/x", (double)0);
        param_.setParam(position_path + "/y", (double)0);
        param_.setParam(position_path + "/z", (double)0);
        std::string orientation_path = cartesian_path + "/orientation";
        param_.setParam(orientation_path + "/a", (double)0);
        param_.setParam(orientation_path + "/b", (double)0);
        param_.setParam(orientation_path + "/c", (double)0);
        std::string joint_path = reg_path + "/joint";
        param_.setParam(joint_path + "/j1", (double)0);
        param_.setParam(joint_path + "/j2", (double)0);
        param_.setParam(joint_path + "/j3", (double)0);
        param_.setParam(joint_path + "/j4", (double)0);
        param_.setParam(joint_path + "/j5", (double)0);
        param_.setParam(joint_path + "/j6", (double)0);

        param_.setParam(reg_path + "/pos_type", (int)0);

        std::string ext_pos_path = reg_path + "/ext_pos";
        param_.setParam(ext_pos_path + "/ext1", (double)0);
        param_.setParam(ext_pos_path + "/ext2", (double)0);
        param_.setParam(ext_pos_path + "/ext3", (double)0);

        param_.setParam(reg_path + "/group_id", (int)0);

        std::string posture_path = reg_path + "/posture";
        param_.setParam(posture_path + "/is_flip", (int)0);
        param_.setParam(posture_path + "/is_left", (int)0);
        param_.setParam(posture_path + "/is_up", (int)0);
        param_.setParam(posture_path + "/is_front", (int)0);

        std::string multiturn_path = reg_path + "/multiturn";
        param_.setParam(multiturn_path + "/axis_a", (int)0);
        param_.setParam(multiturn_path + "/axis_b", (int)0);
        param_.setParam(multiturn_path + "/axis_c", (int)0);
    }
    return param_.dumpParamFile(file_path_.c_str());
}

bool PrReg::readAllRegDataFromYaml()
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
        
        std::string cartesian_path = reg_path + "/cartesian";
        std::string position_path = cartesian_path + "/position";
        param_.getParam(position_path + "/x", data_list_[i].cartesian_pos.position.x);
        param_.getParam(position_path + "/y", data_list_[i].cartesian_pos.position.y);
        param_.getParam(position_path + "/z", data_list_[i].cartesian_pos.position.z);
        std::string orientation_path = cartesian_path + "/orientation";
        param_.getParam(orientation_path + "/a", data_list_[i].cartesian_pos.orientation.a);
        param_.getParam(orientation_path + "/b", data_list_[i].cartesian_pos.orientation.b);
        param_.getParam(orientation_path + "/c", data_list_[i].cartesian_pos.orientation.c);
        std::string joint_path = reg_path + "/joint";
        param_.getParam(joint_path + "/j1", data_list_[i].joint_pos[0]);
        param_.getParam(joint_path + "/j2", data_list_[i].joint_pos[1]);
        param_.getParam(joint_path + "/j3", data_list_[i].joint_pos[2]);
        param_.getParam(joint_path + "/j4", data_list_[i].joint_pos[3]);
        param_.getParam(joint_path + "/j5", data_list_[i].joint_pos[4]);
        param_.getParam(joint_path + "/j6", data_list_[i].joint_pos[5]);

        param_.getParam(reg_path + "/pos_type", data_list_[i].pos_type);

        std::string ext_pos_path = reg_path + "/ext_pos";
        param_.getParam(ext_pos_path + "/ext1", data_list_[i].ext_pos[0]);
        param_.getParam(ext_pos_path + "/ext2", data_list_[i].ext_pos[1]);
        param_.getParam(ext_pos_path + "/ext3", data_list_[i].ext_pos[2]);

        param_.getParam(reg_path + "/group_id", data_list_[i].group_id);

        std::string posture_path = reg_path + "/posture";
        param_.getParam(posture_path + "/is_flip", data_list_[i].posture[0]);
        param_.getParam(posture_path + "/is_left", data_list_[i].posture[1]);
        param_.getParam(posture_path + "/is_up", data_list_[i].posture[2]);
        param_.getParam(posture_path + "/is_front", data_list_[i].posture[3]);

        std::string multiturn_path = reg_path + "/multiturn";
        param_.getParam(multiturn_path + "/axis_a", data_list_[i].multiturn[0]);
        param_.getParam(multiturn_path + "/axis_b", data_list_[i].multiturn[1]);
        param_.getParam(multiturn_path + "/axis_c", data_list_[i].multiturn[2]);      
    }
    return true;
}

bool PrReg::writeRegDataToYaml(const BaseRegData& base_data, const PrValue& data)
{
    std::string reg_path = getRegPath(base_data.id);
    param_.setParam(reg_path + "/id", base_data.id);
    param_.setParam(reg_path + "/is_valid", base_data.is_valid);
    param_.setParam(reg_path + "/comment", base_data.comment);
    
    std::string cartesian_path = reg_path + "/cartesian";
    std::string position_path = cartesian_path + "/position";
    param_.setParam(position_path + "/x", data_list_[base_data.id].cartesian_pos.position.x);
    param_.setParam(position_path + "/y", data_list_[base_data.id].cartesian_pos.position.y);
    param_.setParam(position_path + "/z", data_list_[base_data.id].cartesian_pos.position.z);
    std::string orientation_path = cartesian_path + "/orientation";
    param_.setParam(orientation_path + "/a", data_list_[base_data.id].cartesian_pos.orientation.a);
    param_.setParam(orientation_path + "/b", data_list_[base_data.id].cartesian_pos.orientation.b);
    param_.setParam(orientation_path + "/c", data_list_[base_data.id].cartesian_pos.orientation.c);
    std::string joint_path = reg_path + "/joint";
    param_.setParam(joint_path + "/j1", data_list_[base_data.id].joint_pos[0]);
    param_.setParam(joint_path + "/j2", data_list_[base_data.id].joint_pos[1]);
    param_.setParam(joint_path + "/j3", data_list_[base_data.id].joint_pos[2]);
    param_.setParam(joint_path + "/j4", data_list_[base_data.id].joint_pos[3]);
    param_.setParam(joint_path + "/j5", data_list_[base_data.id].joint_pos[4]);
    param_.setParam(joint_path + "/j6", data_list_[base_data.id].joint_pos[5]);
    
    param_.setParam(reg_path + "/pos_type", data_list_[base_data.id].pos_type);
    
    std::string ext_pos_path = reg_path + "/ext_pos";
    param_.setParam(ext_pos_path + "/ext1", data_list_[base_data.id].ext_pos[0]);
    param_.setParam(ext_pos_path + "/ext2", data_list_[base_data.id].ext_pos[1]);
    param_.setParam(ext_pos_path + "/ext3", data_list_[base_data.id].ext_pos[2]);
    
    param_.setParam(reg_path + "/group_id", data_list_[base_data.id].group_id);
    
    std::string posture_path = reg_path + "/posture";
    param_.setParam(posture_path + "/is_flip", data_list_[base_data.id].posture[0]);
    param_.setParam(posture_path + "/is_left", data_list_[base_data.id].posture[1]);
    param_.setParam(posture_path + "/is_up", data_list_[base_data.id].posture[2]);
    param_.setParam(posture_path + "/is_front", data_list_[base_data.id].posture[3]);
    
    std::string multiturn_path = reg_path + "/multiturn";
    param_.setParam(multiturn_path + "/axis_a", data_list_[base_data.id].multiturn[0]);
    param_.setParam(multiturn_path + "/axis_b", data_list_[base_data.id].multiturn[1]);
    param_.setParam(multiturn_path + "/axis_c", data_list_[base_data.id].multiturn[2]);  

    return param_.dumpParamFile(file_path_.c_str());
}

std::string PrReg::getRegPath(int reg_id)
{
    std::string id_str;
    std::stringstream stream;
    stream << reg_id;
    stream >> id_str;
    return (std::string("PrReg") + id_str);
}


