/**
 * @file io_interface.cpp
 * @brief 
 * @author WangWei
 * @version 1.0.0
 * @date 2017-06-12
 */
	 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "ip_address.h"

#include "io_interface.h"
#include "common.h"
#include "error_code.h"
// #ifndef WIN32
// #include "common/error_code.h"
// #endif
#include "error_monitor.h"
#include <boost/algorithm/string.hpp>
#include "forsight_inter_control.h"

IOInterface::IOInterface()
{
    U64 result = initial();
    if (result != TPI_SUCCESS)
    {
        setWarning(result);
    }
}

IOInterface::~IOInterface()
{
#ifdef USE_IO_MANAGER
   if (io_manager_ != NULL)
       delete io_manager_;
   if (dev_info_ != NULL)
       delete [] dev_info_;
#endif
}

IOInterface* IOInterface::instance()
{
    static IOInterface io_interface;

    return &io_interface;
}

/**
 * @brief : get local ip address
 *
 * @return : the ip address in the form of string
 */
std::string getLocalIP()
{
	int fd;
    struct ifreq ifr;

    char iface[] = "eth0";
     
    fd = socket(AF_INET, SOCK_DGRAM, 0);
 
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
 
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
 
    ioctl(fd, SIOCGIFADDR, &ifr);
 
    close(fd);
 
    //display result
    //printf("%s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
	std::string ret = inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
	return ret;
}

U64 IOInterface::initial()
{
    U64 result = 0;
#ifdef USE_IO_MANAGER
    io_manager_ = new fst_io_manager::IOManager;
    std::string str_addr = getLocalIP();
	if(str_addr.substr(0,3) == "192")
	{
    	FST_INFO("Use Fake io_manager_");
        result = io_manager_->init(1);
	}
    else
	{
    	FST_INFO("Use True io_manager_");
	    result = io_manager_->init(0);
	}
    if (result != TPI_SUCCESS)
    {
        FST_ERROR("io_manager_ init failed:%llx", result);
        return result;
    }
    //-----------------get num of devices.----------------------//
    io_num_.store(io_manager_->getDevicesNum());
    FST_INFO("IOInterface::initial io_num_:%d",io_num_.load());

    dev_info_ = new fst_io_manager::IODeviceInfo[io_num_];

    for (int i = 0; i < io_num_; i++)
    {
        result = io_manager_->getDeviceInfo(i, dev_info_[i]);
        //FST_INFO("input:%d,output:%d", dev_info_[i].input, dev_info_[i].output);
        if (result != TPI_SUCCESS)
            return result;
    }
#endif

    return TPI_SUCCESS;
}


int IOInterface::getIODevNum()
{
    return io_num_;
}

/*
void IOInterface::getIODevices(motion_spec_DeviceList &dev_list)
{
    dev_list.dev_info_count = io_num_;
    for (int i = 0; i < io_num_; i++)
    {
        strcpy(dev_list.dev_info[i].communication_type, dev_info_[i].communication_type.c_str());
        dev_list.dev_info[i].device_number = dev_info_[i].device_number;
        dev_list.dev_info[i].device_type = (motion_spec_DeviceType)dev_info_[i].device_type;
        dev_list.dev_info[i].input = dev_info_[i].input;
        dev_list.dev_info[i].output = dev_info_[i].output;
    }
}
*/

/*
bool IOInterface::encDevList(BaseTypes_ParameterMsg *param_msg, pb_ostream_t *stream, const pb_field_t *field)
{
    param_msg->has_info = true;
    param_msg->info.overwrite_active = 0; //not active
    param_msg->info.data_type = 2;  // uint8
    param_msg->info.data_size = 1;  // 1 byte
    param_msg->info.number_of_elements = 1;  
    param_msg->info.param_type = BaseTypes_ParamType_INPUT_SIGNAL;  //input
    param_msg->info.permission = BaseTypes_Permission_permission_undefined;
    param_msg->info.user_level = BaseTypes_UserLevel_user_level_undefined;
    param_msg->info.unit = BaseTypes_Unit_unit_undefined;
    for (int i = 0; i < io_num_; i++)
    {        
        strcpy(param_msg->info.path, dev_info_[i].path.c_str());
        param_msg->info.id = dev_info_[i].id;

        if (!pb_encode_tag_for_field(stream, field))
            return false;        
        if (!pb_encode_submessage(stream, BaseTypes_ParameterMsg_fields, param_msg))
            return false;
    }

    return true;
}
*/

U64 IOInterface::setDO(const char *path, char value)
{
    std::vector<std::string> vc_path;
    boost::split(vc_path, path, boost::is_any_of("/"));
    int size = vc_path.size();
    if ((size != 6) || (vc_path[0] != "root") 
    || (vc_path[1] != "IO") || (vc_path[4] != "DO"))
    {
        return INVALID_PATH_FROM_TP;        
    }
    for (int i = 0; i < getIODevNum(); i++)
    {
#ifdef USE_IO_MANAGER
        if ((vc_path[2] == dev_info_[i].communication_type) 
        && (stoi(vc_path[3]) == dev_info_[i].device_number))
        {
            FST_INFO("id:%d, port:%d, value:%d", dev_info_[i].id, stoi(vc_path[5]), value);
            return io_manager_->setModuleValue(dev_info_[i].id, stoi(vc_path[5]), value);
        }
#else
		return FST_SUCCESS;
#endif
    }
    return PARSE_IO_PATH_FAILED;
}


U64 IOInterface::setDO(int msg_id, unsigned char value)
{
    int index = msg_id % IO_MAX_NUM;
    int dev_address = msg_id - index;
    FST_INFO("dev_address:%d,index:%d, value:%d", dev_address, index, value);
#ifdef USE_IO_MANAGER
    return io_manager_->setModuleValue(dev_address, index, value);
#else
    return FST_SUCCESS;
#endif
}

U64 IOInterface::setDO(IOPortInfo *io_info, char value)
{
    if (io_info->port_index == 0)
    {
        return INVALID_PATH_FROM_TP;
    }
#ifdef USE_IO_MANAGER
    return io_manager_->setModuleValue(io_info->dev_id, io_info->port_index, value);
#else
    return FST_SUCCESS;
#endif
}

U64 IOInterface::getDIO(const char *path, unsigned char *buffer, int buf_len, int& io_bytes_len)
{
    std::vector<std::string> vc_path;
    boost::split(vc_path, path, boost::is_any_of("/"));
#ifdef USE_IO_MANAGER
    

    int size = vc_path.size();
    for (int i = 0; i < getIODevNum(); i++)
    {
        if ((vc_path[2] == dev_info_[i].communication_type) 
        && (stoi(vc_path[3]) == dev_info_[i].device_number))
        {
            if (size == 4)
            {
                int io_len;
                U64 result = io_manager_->getModuleValues(dev_info_[i].id, buf_len, buffer, io_bytes_len);
				io_bytes_len = ((dev_info_[i].input + 7) >> 3) + ((dev_info_[i].output + 7) >> 3);
                /*printf("==buffer:");*/
                //for(int j = 0; j < io_bytes_len; j++)
                    //printf("%xd ", buffer[j]);
                /*printf("\n");*/
                return result;
            }
            else if (size == 6)
            {
                io_bytes_len = 1;
                if (vc_path[4] == "DI")
                {
                    FST_INFO("get module value input:id:%d, index:%d",dev_info_[i].id, stoi(vc_path[5]));
                    return io_manager_->getModuleValue(dev_info_[i].id, IO_INPUT, stoi(vc_path[5]), buffer[0]);
                }
                else if (vc_path[4] == "DO")
                {
                    FST_INFO("get module value output:id:%d, index:%d", dev_info_[i].id, stoi(vc_path[5]));
                    return io_manager_->getModuleValue(dev_info_[i].id, IO_OUTPUT, stoi(vc_path[5]), buffer[0]);
                }
                else
                {
                    return PARSE_IO_PATH_FAILED;
                }
            }
            else 
            {
                return PARSE_IO_PATH_FAILED;
            }
        }
    }
    return PARSE_IO_PATH_FAILED;
#else
	return FST_SUCCESS;
#endif
}


U64 IOInterface::getDIO(int msg_id, uint8_t *buffer, int buf_len, int& io_bytes_len)
{
    unsigned int index = msg_id % IO_MAX_NUM;
    int dev_address = msg_id - index;
    //FST_INFO("index:%d,dev_address:%d", index, dev_address);
    int i = getIODevIndex(dev_address);
    if (i < 0)
    {
        return INVALID_PATH_FROM_TP;
    }
#ifdef USE_IO_MANAGER
    fst_io_manager::IODeviceInfo dev = dev_info_[i];
#endif
    if (index == 0)
    {
        int io_len;
#ifdef USE_IO_MANAGER
        U64 result = io_manager_->getModuleValues(dev.id, buf_len, buffer, io_len);
        io_bytes_len = ((dev.input + 7) >> 3) + ((dev.output + 7) >> 3);
#else
		U64 result = FST_SUCCESS;
#endif
        return result;
    }
    else
    {
        io_bytes_len = 1;
#ifdef USE_IO_MANAGER
        if (index <= dev.input)
        {
            return io_manager_->getModuleValue(dev.id, IO_INPUT, index, buffer[0]);
        }
        else
        {
            unsigned int new_index = index - dev.input;
            return io_manager_->getModuleValue(dev.id, IO_OUTPUT, new_index, buffer[0]);
        }
#else
		return FST_SUCCESS;
#endif
    }
}

U64 IOInterface::getDIO(IOPortInfo *io_info, uint8_t *buffer, int buf_len)
{
#ifdef USE_IO_MANAGER
    if (io_info->port_index == 0)
    {
        int io_len;
        printf("io_info->port_index == 0 with %d.\n", io_info->dev_id);
        return io_manager_->getModuleValues(io_info->dev_id, buf_len, buffer, io_len);
    }
    else
    {
		printf("IOInterface::getDIO (%d) at %d with %d\n", 
			 buffer[0], io_info->port_index, io_info->bytes_len);
        return io_manager_->getModuleValue(io_info->dev_id, io_info->port_type, io_info->port_index, buffer[0]);
     }
#else
			return FST_SUCCESS;
#endif

}

U64 IOInterface::checkIO(const char *path, IOPortInfo* io_info)
{
    std::vector<std::string> vc_path;
    boost::split(vc_path, path, boost::is_any_of("/"));
#ifdef USE_IO_MANAGER
    
    int size = vc_path.size();
    printf("\t Io_interface::getIODevNum: %d\n", getIODevNum());
    for (int i = 0; i < getIODevNum(); i++)
    {
    		printf("\t device_number: %s\n", dev_info_[i].path.c_str());
    		printf("\t id: %d\n", dev_info_[i].id);
    		printf("\t device_number: %d\n", dev_info_[i].device_number);
        if ((vc_path[2] == dev_info_[i].communication_type) 
        && (stoi(vc_path[3]) == dev_info_[i].device_number))
        {
            io_info->dev_id = dev_info_[i].id;
            if (size == 4)
            {
                io_info->msg_id = io_info->dev_id;
                io_info->port_index = 0;
                io_info->port_type = IO_INPUT;
                io_info->bytes_len = ((dev_info_[i].input + 7) >> 3) + ((dev_info_[i].output + 7) >> 3);
                return TPI_SUCCESS;
            }
            else if (size == 6)       
            {
                int index = stoi(vc_path[5]);
                if (vc_path[4] == "DI")
                {   
                    if (index > (int)dev_info_[i].input)
                        return INVALID_PATH_FROM_TP;
                    io_info->port_index = index;
                    io_info->port_type = IO_INPUT;
                    io_info->msg_id = io_info->dev_id + index;
                }
                else if (vc_path[4] == "DO")
                {   
                    if (index > (int)dev_info_[i].output)
                        return INVALID_PATH_FROM_TP;
                    io_info->port_index = index;
                    io_info->port_type = IO_OUTPUT;
                    io_info->msg_id = io_info->dev_id + dev_info_[i].input + index;
                }
                else
                {
                    return INVALID_PATH_FROM_TP;
                }
                io_info->bytes_len =  1;
                return TPI_SUCCESS;
            }
            else 
            {
                return PARSE_IO_PATH_FAILED;
            }
        }
    }
    return PARSE_IO_PATH_FAILED;
#else
	return FST_SUCCESS;
#endif
}



int IOInterface::getIODevIndex(int dev_address)
{
#ifdef USE_IO_MANAGER
    for (int i = 0; i < io_num_; i++)
    {        
        if ((int)dev_info_[i].id == dev_address)
        {
            return i;
        }
   }

   return -1;
#else
	return -1;
#endif
}


U64 IOInterface::updateIOError()
{
#ifdef USE_IO_MANAGER
    static U64 result = io_manager_->getIOError();
#else
	static U64 result = FST_SUCCESS;
#endif
    if (result != TPI_SUCCESS)
    {
        setWarning(result);
    }
	return result ;
}

