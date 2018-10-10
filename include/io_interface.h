/**
 * @file io_interface.h
 * @brief 
 * @author WangWei
 * @version 1.0.0
 * @date 2017-06-12
 */
#ifndef TP_INTERFACE_IO_INTERFACE_H_
#define TP_INTERFACE_IO_INTERFACE_H_

// #define USE_IO_MANAGER

#ifndef WIN32
#include <atomic>

#ifdef USE_IO_MANAGER
#include "io_manager/io_manager.h"
#endif

#endif
// #include "motionSL.pb.h"

#define IO_BASE_ADDRESS (100000)
#define IO_MAX_NUM      (1000)

#ifndef USE_IO_MANAGER
#define FST_SUCCESS (unsigned long long int)0
typedef unsigned long long int U64;
#define PARSE_IO_PATH_FAILED                    (unsigned long long int)0x0001000400670009   /*cant use current path to set IO*/
#define INVALID_PATH_FROM_TP                    (unsigned long long int)0x0001000200830004   /*tp sent invalid path*/

// IO Macro and structure begin
#ifndef IO_BASE_ADDRESS
#define IO_BASE_ADDRESS (100000)
#endif

#ifndef IO_MAX_NUM
#define IO_MAX_NUM      (1000)
#endif

#ifndef IO_INPUT
#define IO_INPUT 0
#endif

#ifndef IO_OUTPUT
#define IO_OUTPUT 1
#endif

#ifndef IO_DATAFRAME_MAX
#define IO_DATAFRAME_MAX 5
#endif


#endif


typedef struct _IOPortInfo
{
    uint32_t    msg_id;
    uint32_t    dev_id;
    int         port_type;
    int         port_index;
    int         bytes_len;
}IOPortInfo;


class IOInterface
{
  public:	 
    IOInterface();
    ~IOInterface();
    static IOInterface* instance();
    U64 initial();
    /**
     * @brief: add io device infomation to dev_info_ 
     *
     * @return: 0 if success 
     */
    U64 addIODevices();
//    void getIODevices(motion_spec_DeviceList &dev_list);

    //bool encDevList(BaseTypes_ParameterMsg *param_msg, pb_ostream_t *stream, const pb_field_t *field);

    /**
     * @brief: get io devices number 
     *
     * @return 
     */
    int getIODevNum();
    
    /**
     * @brief: set DO 
     *
     * @param path: path of set DO
     * @param value: value to set
     *
     * @return: 0 if success 
     */
    U64 setDO(const char *path, char value);

    /**
     * @brief: set DO 
     *
     * @param msg_id: id of this DO
     * @param value: value to set
     *
     * @return: 0 if success 
     */
    U64 setDO(int msg_id, unsigned char value);

    /**
     * @brief: set DO 
     *
     * @param io_info: port infomation of DO
     * @param value: value to set
     *
     * @return: 0 if success 
     */
    U64 setDO(IOPortInfo *io_info, char value);

    /**
     * @brief:get DI and DO 
     *
     * @param path: path to get
     * @param buffer: buffer to store value 
     * @param buf_len: buffer len
     * @param io_bytes_len: actual length
     *
     * @return: 0 if success 
     */
    U64 getDIO(const char *path, unsigned char *buffer, int buf_len, int& io_bytes_len);

   /**
     * @brief:get DI and DO 
     *
     * @param msg_id: id to get
     * @param buffer: buffer to store value 
     * @param buf_len: buffer len
     * @param io_bytes_len: actual length
     *
     * @return: 0 if success 
     */
    U64 getDIO(int msg_id, uint8_t *buffer, int buf_len, int& io_bytes_len);
     /**
     * @brief:get DI and DO 
     *
     * @param io_info: infomation of DI or DO
     * @param buffer: buffer to store value 
     * @param buf_len: buffer len
     *
     * @return: 0 if success 
     */
    U64 getDIO(IOPortInfo *io_info, uint8_t *buffer, int buf_len);
    
    /**
     * @brief: check valid of this DI or DO 
     *
     * @param path: path of this DI or DO
     * @param io_info: io infomation 
     *
     * @return: true if it is valid 
     */
    U64 checkIO(const char *path, IOPortInfo* io_info);

    /**
     * @brief: get index of this IO device
     *
     * @param dev_address: address of device
     *
     * @return: index of IO device 
     */
    int getIODevIndex(int dev_address);

    /**
     * @brief: update IO error 
     */
    U64 updateIOError();

//	fst_io_manager::IODeviceInfo* getDevInfoPtr(){ return dev_info_ ; }

  private:
#ifdef USE_IO_MANAGER
    fst_io_manager::IOManager *io_manager_;
    std::atomic<fst_io_manager::IODeviceInfo*>  dev_info_;  //
#endif
    std::atomic_int     io_num_;    //number of IO board
};

#endif
