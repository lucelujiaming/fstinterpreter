#include "reg_manager_interface.h"
#include <fstream>

using namespace fst_reg;
using namespace std;

RegManagerInterface::RegManagerInterface(std::string file_dir):
    is_ready_(false), file_dir_(file_dir)
{
	printf("RegManagerInterface::RegManagerInterface Enter \n");
	
    reg_ptr_[REG_TYPE_PR] = new PrReg(MAX_PR_REG_ID, file_dir);
    reg_ptr_[REG_TYPE_MR] = new MrReg(MAX_MR_REG_ID, file_dir);
    reg_ptr_[REG_TYPE_SR] = new SrReg(MAX_SR_REG_ID, file_dir);
    reg_ptr_[REG_TYPE_R] = new RReg(MAX_R_REG_ID, file_dir);

    for(int i = 0; i < REG_TYPE_MAX; ++i)
    {
        if(reg_ptr_[i] == NULL)
        {
            return;
        }
    }
    is_ready_ = isReady();
    if(!is_ready_)
    {
        return;
    }
}

RegManagerInterface::~RegManagerInterface()
{

}

bool RegManagerInterface::isReady()
{
    for(int i = 0; i < REG_TYPE_MAX; ++i)
    {
        if(!reg_ptr_[i]->isReady())
        {
            return false;
        }
    }
    return true;
}

bool RegManagerInterface::addPrReg(PrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_PR]->addReg(reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::deletePrReg(int id)
{
    return reg_ptr_[REG_TYPE_PR]->deleteReg(id);
}

bool RegManagerInterface::getPrReg(int id, PrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_PR]->getReg(id, reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::setPrReg(PrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_PR]->setReg(reinterpret_cast<void*>(data_ptr));
}

std::vector<BaseRegData> RegManagerInterface::getPrRegChangedIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_PR]->getChangedIdList(start_id, size);
}

std::vector<BaseRegData> RegManagerInterface::getPrRegValidIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_PR]->getValidIdList(start_id, size);
}

bool RegManagerInterface::addMrReg(MrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_MR]->addReg(reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::deleteMrReg(int id)
{
    return reg_ptr_[REG_TYPE_MR]->deleteReg(id);
}

bool RegManagerInterface::getMrReg(int id, MrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_MR]->getReg(id, reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::setMrReg(MrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_MR]->setReg(reinterpret_cast<void*>(data_ptr));
}

std::vector<BaseRegData> RegManagerInterface::getMrRegChangedIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_MR]->getChangedIdList(start_id, size);
}

std::vector<BaseRegData> RegManagerInterface::getMrRegValidIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_MR]->getValidIdList(start_id, size);
}

bool RegManagerInterface::addSrReg(SrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_SR]->addReg(reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::deleteSrReg(int id)
{
    return reg_ptr_[REG_TYPE_SR]->deleteReg(id);
}

bool RegManagerInterface::getSrReg(int id, SrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_SR]->getReg(id, reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::setSrReg(SrRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_SR]->setReg(reinterpret_cast<void*>(data_ptr));
}

std::vector<BaseRegData> RegManagerInterface::getSrRegChangedIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_SR]->getChangedIdList(start_id, size);
}

std::vector<BaseRegData> RegManagerInterface::getSrRegValidIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_SR]->getValidIdList(start_id, size);
}

bool RegManagerInterface::addRReg(RRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_R]->addReg(reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::deleteRReg(int id)
{
    return reg_ptr_[REG_TYPE_R]->deleteReg(id);
}

bool RegManagerInterface::getRReg(int id, RRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_R]->getReg(id, reinterpret_cast<void*>(data_ptr));
}

bool RegManagerInterface::setRReg(RRegData* data_ptr)
{
    return reg_ptr_[REG_TYPE_R]->setReg(reinterpret_cast<void*>(data_ptr));
}

std::vector<BaseRegData> RegManagerInterface::getRRegChangedIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_R]->getChangedIdList(start_id, size);
}

std::vector<BaseRegData> RegManagerInterface::getRRegValidIdList(int start_id, int size)
{
    return reg_ptr_[REG_TYPE_R]->getValidIdList(start_id, size);
}

RegManagerInterface::RegManagerInterface()
{

}


