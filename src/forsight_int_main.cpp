#ifndef WIN32
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "common.h"
using namespace fst_log;
#else
#include "forsight_io_mapping.h"
#endif
#include "forsight_inter_control.h"
#include "forsight_io_controller.h"

#include "reg_manager/reg_manager_interface_wrapper.h"

#define IO_ERROR_INTERVAL_COUNT           (5)  //2ms*5=10ms

// #ifndef WIN32
// Logger glog;
// #endif

#ifndef WIN32
void signalInterrupt(int signo) 
{
	if(log_ptr_ != NULL)
	{
        FST_INFO("Free log_ptr_ in the signalInterrupt");
		
		delete log_ptr_;
		log_ptr_ = NULL ;
	}
    _exit(0);
}
#endif


/************************************************* 
	Function:		main
	Description:	main function.
	Input:			name  - io name
	Input:			NULL
	Output: 		NULL
	Return: 		Never
*************************************************/ 
int main(int  argc, char *argv[])
{
	InterpreterControl intprt_ctrl; 
#ifndef WIN32
	signal(SIGINT, signalInterrupt);
	if(log_ptr_ == NULL)
	{
		log_ptr_ = new fst_log::Logger();
    	FST_LOG_INIT("Interpreter");
	}
#endif
	initInterpreter();
	memset(&intprt_ctrl, 0x00, sizeof(intprt_ctrl));
#ifndef WIN32
	intprt_ctrl.cmd = fst_base::INTERPRETER_SERVER_CMD_START ;
#else
	intprt_ctrl.cmd = fst_base::INTERPRETER_SERVER_CMD_START ;
	append_io_mapping();
	forgesight_load_io_config();
#endif
	load_register_data();
	while(1)
	{
#ifndef WIN32
		std::vector<fst_base::ProcessCommRequestResponse>::iterator it;
		std::vector<fst_base::ProcessCommRequestResponse> request_list
			= g_objInterpreterServer->popTaskFromRequestList();
		if(request_list.size() != 0)
		{
			for(it = request_list.begin(); it != request_list.end(); ++it)
			{
				memset(&intprt_ctrl, 0x00, sizeof(intprt_ctrl));
				intprt_ctrl.cmd = it->cmd_id ;
	            FST_INFO("parseCtrlComand at %d ", intprt_ctrl.cmd);
				parseCtrlComand(intprt_ctrl, it->request_data_ptr);
				bool * bRsp = it->response_data_ptr;
				*bRsp = true;
				g_objInterpreterServer->pushTaskToResponseList(*it);
			}
			usleep(1000);
		    static int count = 0;
		    if (++count >= IO_ERROR_INTERVAL_COUNT)
		    {
				updateIOError();
		        count = 0;
		    }
		}
		else
		{
			intprt_ctrl.cmd = fst_base::INTERPRETER_SERVER_CMD_LOAD ;
			usleep(1000);
		}
#else
		parseCtrlComand(intprt_ctrl, "timer_test");
		intprt_ctrl.cmd = fst_base::INTERPRETER_SERVER_CMD_LOAD ;
		Sleep(100);
#endif
	}
	
	uninitInterpreter();
#ifndef WIN32
	if(log_ptr_ != NULL)
	{
        FST_INFO("Free log_ptr_");
		
		delete log_ptr_;
		log_ptr_ = NULL ;
	}
#endif
	return 1;
}


