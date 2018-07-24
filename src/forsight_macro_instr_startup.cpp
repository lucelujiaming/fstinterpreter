// forsight_launch_code.cpp : 
//
#include "macro_instr_mgr.h"
#include "forsight_inter_control.h"
#include "forsight_macro_instr_startup.h"
#include "forsight_io_controller.h"

#define DATA_PATH        "\/root\/files_manager_python27\/data"

#ifdef WIN32
HANDLE    g_macro_instr_interpreter_handle;
#else
pthread_t g_macro_instr_interpreter_handle;
#endif
extern int  g_iCurrentThreadSeq ;  
extern struct thread_control_block g_thread_control_block[NUM_THREAD];

MacroInstrMgr  *  g_macro_instr_mgr_ptr; 

#ifdef WIN32
unsigned __stdcall macro_instr_thread(void* arg)
#else
void* macro_instr_thread(void* arg)
#endif
{
	int iRet = 0 ; 
	eval_value value ;
	std::map<std::string, io_macro_instr>::iterator it;

	thread_control_block * objThdCtrlBlockPtr = NULL;
	g_macro_instr_mgr_ptr = new MacroInstrMgr();
	while(1)
	{
		it = g_macro_instr_mgr_ptr->ioMgrList.begin();
		while(it != g_macro_instr_mgr_ptr->ioMgrList.end())
		{
			value.setFloatValue(0.0);
			value = forgesight_get_io_status((char *)it->first.c_str());
			if((value.getFloatValue() != 0.0) 
				&&(it->second.iValue == 0)
				&&(strlen(it->second.program_name) > 0)
				&&(it->second.bIsRunning == false))
			{
				printf("start run...\n");
				objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
				if(objThdCtrlBlockPtr->is_in_macro == true)
				{
					printf("Can not run macro again\n");
					break;
				}
				g_iCurrentThreadSeq++ ;
				if(g_iCurrentThreadSeq < 0) break ;
				objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
				
				objThdCtrlBlockPtr->prog_mode = FULL_MODE;
				objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
				setPrgmState(EXECUTE_R);
				if(strlen(it->second.program_name) == 0)
				{
					startFile(objThdCtrlBlockPtr, "sr_test", g_iCurrentThreadSeq);
				}
				else 
				{
					startFile(objThdCtrlBlockPtr, it->second.program_name, g_iCurrentThreadSeq);
				}
#ifdef WIN32
				Sleep(100);
#else
				usleep(1000);
#endif
			}
			else if (it->second.bIsRunning == true)
			{
				printf("it is running...\n");
			}

			it++;         
		}
		// g_macro_instr_mgr_ptr.updateAll();
#ifdef WIN32
		Sleep(100);
#else
		usleep(1000);
#endif
	}
#ifdef WIN32
	CloseHandle( g_macro_instr_interpreter_handle );  
	g_macro_instr_interpreter_handle = NULL; 
	return NULL;
#else
	printf("Enter pthread_join.\n");
	pthread_join(g_macro_instr_interpreter_handle, NULL);
	printf("Left  pthread_join.\n");
	fflush(stdout);
	g_macro_instr_interpreter_handle = 0;
#endif // WIN32
}


bool macro_instr_thread_create(void * args)
{
	bool ret = false;
#ifdef WIN32
	g_macro_instr_interpreter_handle =
		(HANDLE)_beginthreadex(NULL, 0, macro_instr_thread, args, 0, NULL);
	if (NULL != g_macro_instr_interpreter_handle)
	{
		ret = true;
	}
#else
	if (0 == pthread_create(
		&(g_macro_instr_interpreter_handle), NULL, macro_instr_thread, args))
	{
		ret = true;
	}
	else
	{
        printf("start launch_code_thread_create Failed..\n");
		g_macro_instr_interpreter_handle = 0;
	}
#endif
	return ret;
}

void macro_instr_thread_destroy()
{
#ifdef WIN32
	WaitForSingleObject(g_macro_instr_interpreter_handle, INFINITE);
	if (NULL != g_macro_instr_interpreter_handle)
	{
		CloseHandle(g_macro_instr_interpreter_handle);
	}
	g_macro_instr_interpreter_handle = NULL;
#else
	pthread_join(g_macro_instr_interpreter_handle, NULL);
	g_macro_instr_interpreter_handle = 0;
#endif // WIN32
}


