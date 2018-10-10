#ifndef INTERPRETER_REG_MANAGER_BASE_REG_H
#define INTERPRETER_REG_MANAGER_BASE_REG_H

#include <vector>
#include <string>


namespace fst_reg
{
enum {MAX_REG_COMMENT_LENGTH = 32,};

enum
{
    MAX_PR_REG_ID = 10,//200,
    MAX_SR_REG_ID = 10,//300,
    MAX_MR_REG_ID = 10,//1500,
    MAX_R_REG_ID = 10,//1500,
    MAX_HR_REG_ID = 10,//1500,
};

typedef enum
{
    REG_TYPE_PR = 0,
    REG_TYPE_SR = 1,
    REG_TYPE_MR = 2,
    REG_TYPE_R = 3,
    REG_TYPE_HR = 4,
    REG_TYPE_MAX = 5,
    REG_TYPE_INVALID = 6,
}RegType;

typedef struct
{
    int id;
    bool is_valid;
    bool is_changed;
    char comment[MAX_REG_COMMENT_LENGTH];
}BaseRegData;

class BaseReg
{
public:
    BaseReg(RegType type, int size);
    ~BaseReg();

    virtual bool addReg(void* data_ptr) = 0;
    virtual bool deleteReg(int id) = 0;
    virtual bool getReg(int id, void* data_ptr) = 0;
    virtual bool setReg(void* data_ptr) = 0;
    std::vector<BaseRegData> getChangedIdList(int start_id, int size);
    std::vector<BaseRegData> getValidIdList(int start_id, int size);

    RegType getRegType();
    void setReady(bool is_ready);
    bool isReady();
    bool isValid(int id);
    bool setValid(int id, bool is_valid);
    bool setComment(int id, const char comment[MAX_REG_COMMENT_LENGTH]);
    char* getComment(int id);
    bool setRegList(BaseRegData& data);
    bool getRegList(int id, BaseRegData& data);
    bool isAddInputValid(int id);
    bool isDeleteInputValid(int id);
    bool isSetInputValid(int id);
    bool isGetInputValid(int id);
    void packAddRegData(BaseRegData& data, int id, const     char* comment);
    void packDeleteRegData(BaseRegData& data, int id);
    void packSetRegData(BaseRegData& data, int id, const     char* comment);
    
private:
    BaseReg();
    RegType type_;
    bool is_ready_;
    std::vector<BaseRegData> reg_list_;
};

}


#endif
