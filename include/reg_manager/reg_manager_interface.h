#ifndef REG_MANAGER_INTERFACE_H
#define REG_MANAGER_INTERFACE_H

#include "fst_datatype.h"
#include "base_reg.h"
#include "pr_reg.h"
#include "mr_reg.h"
#include "sr_reg.h"
#include "r_reg.h"
#include <string>

namespace fst_reg
{
class RegManagerInterface
{
public:
    RegManagerInterface(std::string file_dir);
    ~RegManagerInterface();

    //------------------------------------------------------------
    // Function:    isReady
    // Summary: Return the status of RegManagerInterface.
    // In:      None
    // Out:     None
    // Return:  true: RegManagerInterface API can be used correctly
    //          false:RegManagerInterface API will not work
    //------------------------------------------------------------
    bool isReady();
    //------------------------------------------------------------
    // Function:    addPrReg
    // Summary: add a Pr Reg to the reg list
    // In:      data_ptr: corresponding data of the reg
    // Out:     none
    // Return:  true: operation success
    //          false:operation failed
    //------------------------------------------------------------
    bool addPrReg(PrRegData* data_ptr);
    //------------------------------------------------------------
    // Function:    deletePrReg
    // Summary: delete a Pr Reg from the reg list
    // In:      id: index of the pr reg to be deleted
    // Out:     none
    // Return:  true: operation success
    //          false:operation failed
    //------------------------------------------------------------    
    bool deletePrReg(int id);
    //------------------------------------------------------------
    // Function:    getPrReg
    // Summary: get a Pr Reg data from the reg list
    // In:      id: index of the pr reg to be deleted
    // Out:     data_ptr: data pointer of storing the data of the expected pr reg 
    // Return:  true: operation success
    //          false:operation failed
    //------------------------------------------------------------   
    bool getPrReg(int id, PrRegData* data_ptr);
    //------------------------------------------------------------
    // Function:    setPrReg
    // Summary: set a Pr Reg data which has already been in the reg list
    // In:      data_ptr: data pointer of storing the data of the expected pr reg 
    // Out:     none
    // Return:  true: operation success
    //          false:operation failed
    //------------------------------------------------------------ 
    bool setPrReg(PrRegData* data_ptr);
    //------------------------------------------------------------
    // Function:    getPrRegChangedIdList
    // Summary: list all changed indexes of pr regs in the given index range
    // In:      start_id: start index of pr reg
    //          size: the scope of scanning
    // Out:     none
    // Return:  list of changed pr regs
    //------------------------------------------------------------
    std::vector<BaseRegData> getPrRegChangedIdList(int start_id, int size);

    //------------------------------------------------------------
    // Function:    getPrRegValidIdList
    // Summary: list all valid indexes of pr regs in the given index range,
    //          calling this API will not change the corresponding is_changed flag of PrRegs
    // In:      start_id: start index of pr reg
    //          size: the scope of scanning
    // Out:     none
    // Return:  list of valid pr regs
    //------------------------------------------------------------
    std::vector<BaseRegData> getPrRegValidIdList(int start_id, int size);
    
    // MrReg APIgroup: the same as PrReg API group
    bool addMrReg(MrRegData* data_ptr);
    bool deleteMrReg(int id);
    bool getMrReg(int id, MrRegData* data_ptr);
    bool setMrReg(MrRegData* data_ptr);    
    std::vector<BaseRegData> getMrRegChangedIdList(int start_id, int size);
    std::vector<BaseRegData> getMrRegValidIdList(int start_id, int size);

    // SrReg APIgroup: the same as PrReg API group
    bool addSrReg(SrRegData* data_ptr);
    bool deleteSrReg(int id);
    bool getSrReg(int id, SrRegData* data_ptr);
    bool setSrReg(SrRegData* data_ptr);    
    std::vector<BaseRegData> getSrRegChangedIdList(int start_id, int size);
    std::vector<BaseRegData> getSrRegValidIdList(int start_id, int size);

    // RReg APIgroup: the same as PrReg API group
    bool addRReg(RRegData* data_ptr);
    bool deleteRReg(int id);
    bool getRReg(int id, RRegData* data_ptr);
    bool setRReg(RRegData* data_ptr);    
    std::vector<BaseRegData> getRRegChangedIdList(int start_id, int size);
    std::vector<BaseRegData> getRRegValidIdList(int start_id, int size);
    
private:
    RegManagerInterface();      // default allocator is not used
    
    BaseReg* reg_ptr_[REG_TYPE_MAX];

    bool is_ready_;
    std::string file_dir_;
};

}


#endif

