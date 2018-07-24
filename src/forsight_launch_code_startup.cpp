// forsight_launch_code.cpp : 
//
#include "launch_code_mgr.h"
#include "forsight_inter_control.h"
#include "forsight_launch_code_startup.h"
#include "forsight_io_controller.h"

#define UI_SERVO_ENABLE             "ui[1]"
#define UI_PAUSE_REQUEST            "ui[2]"
#define UI_RESET                    "ui[3]"
#define UI_START_RESTART            "ui[4]"
#define UI_ABORT_PROGRAM            "ui[5]"
#define UI_SELECTION_STROBE         "ui[6]"
#define UI_MPLCS_START              "ui[7]"
#define UI_PROGRAM_SELECTION_1      "ui[8]"
#define UI_PROGRAM_SELECTION_2      "ui[9]"
#define UI_PROGRAM_SELECTION_3      "ui[10]"
#define UI_PROGRAM_SELECTION_4      "ui[11]"
#define UI_PROGRAM_SELECTION_5      "ui[12]"
#define UI_PROGRAM_SELECTION_6      "ui[13]"

#define UO_CMDENBL                  "uo[1]"	
#define UO_PAUSED                   "uo[2]"	
#define UO_FAULT                    "uo[3]"	
#define UO_PROGRAM_RUNNING          "uo[4]"	
#define UO_SERVO_STATUS_SIGNAL      "uo[5]"	
#define UO_SELECTION_CHECK_REQUEST  "uo[6]"	
#define UO_MPLCS_START_DONE         "uo[7]"	
#define UO_SELECTION_CONFIRM_1      "uo[8]"	
#define UO_SELECTION_CONFIRM_2      "uo[9]"	
#define UO_SELECTION_CONFIRM_3      "uo[10]"
#define UO_SELECTION_CONFIRM_4      "uo[11]"
#define UO_SELECTION_CONFIRM_5      "uo[12]"
#define UO_SELECTION_CONFIRM_6      "uo[13]"

// #define DATA_PATH        "\/root\/files_manager_python27\/data"
#define USE_FAKE_PLC

#ifdef WIN32
HANDLE    g_launch_code_interpreter_handle;
#else
pthread_t g_launch_code_interpreter_handle;
#endif
extern int  g_iCurrentThreadSeq ;  
extern struct thread_control_block g_thread_control_block[NUM_THREAD];

LaunchCodeMgr  *  g_launch_code_mgr_ptr; 

void forgesight_simulate_launch_config_values()
{
	eval_value value ;
	value.setFloatValue(1.0);
	// Pull down all of UIO
	forgesight_reset_uio_config_values();
	// Pull up UI_SELECTION_STROBE
	forgesight_set_io_status(UI_SELECTION_STROBE, value);
	// Set UI_PROGRAM_SELECTION = 0b1101 = 13
	forgesight_set_io_status(UI_PROGRAM_SELECTION_1, value);
	forgesight_set_io_status(UI_PROGRAM_SELECTION_3, value);
	forgesight_set_io_status(UI_PROGRAM_SELECTION_4, value);
}

static int get_program_selection_value()
{
	int iRet = 0 ;
	eval_value value ;
	// Bit 0
	value.setFloatValue(0.0);
	value = forgesight_get_io_status(UI_PROGRAM_SELECTION_1);
	if(value.getFloatValue() != 0.0)   iRet = iRet + 1 ;
	// Bit 1
	value.setFloatValue(0.0);
	value = forgesight_get_io_status(UI_PROGRAM_SELECTION_2);
	if(value.getFloatValue() != 0.0)   iRet = iRet + 2 ;
	// Bit 2
	value.setFloatValue(0.0);
	value = forgesight_get_io_status(UI_PROGRAM_SELECTION_3);
	if(value.getFloatValue() != 0.0)   iRet = iRet + 4 ;
	// Bit 3
	value.setFloatValue(0.0);
	value = forgesight_get_io_status(UI_PROGRAM_SELECTION_4);
	if(value.getFloatValue() != 0.0)   iRet = iRet + 8 ;
	// Bit 4
	value.setFloatValue(0.0);
	value = forgesight_get_io_status(UI_PROGRAM_SELECTION_5);
	if(value.getFloatValue() != 0.0)   iRet = iRet + 16 ;
	// Bit 5
	value.setFloatValue(0.0);
	value = forgesight_get_io_status(UI_PROGRAM_SELECTION_6);
	if(value.getFloatValue() != 0.0)   iRet = iRet + 32 ;
#if 0
	// Bit 6
	value.setFloatValue(0.0);
	value = forgesight_get_io_status("ui[6]");
	if(value.getFloatValue() != 0.0)   iRet = iRet + 64 ;
	// Bit 7
	value.setFloatValue(0.0);
	value = forgesight_get_io_status("ui[7]");
	if(value.getFloatValue() != 0.0)   iRet = iRet + 128 ;
#endif
	return iRet ;
}

static void clear_program_selection_value()
{
	int iRet = 0 ;
	eval_value value ;
	// Bit 0
	value.setFloatValue(0.0);
	forgesight_set_io_status(UI_PROGRAM_SELECTION_1, value);
	// Bit 1
	forgesight_set_io_status(UI_PROGRAM_SELECTION_2, value);
	// Bit 2
	forgesight_set_io_status(UI_PROGRAM_SELECTION_3, value);
	// Bit 3
	forgesight_set_io_status(UI_PROGRAM_SELECTION_4, value);
	// Bit 4
	forgesight_set_io_status(UI_PROGRAM_SELECTION_5, value);
	// Bit 5
	forgesight_set_io_status(UI_PROGRAM_SELECTION_6, value);
#if 0
	// Bit 6
	value = forgesight_get_io_status("ui[6]");
	// Bit 7
	value = forgesight_get_io_status("ui[7]");
#endif
}

static void set_selection_confirm_value(int iUI)
{
	int iRet = 0 ;
	eval_value valueOn , valueOff;
	valueOn.setFloatValue(1.0);
	valueOff.setFloatValue(0.0);
	// Bit 0
	if (iUI & 1)
		forgesight_set_io_status(UO_SELECTION_CONFIRM_1, valueOn);
	else
		forgesight_set_io_status(UO_SELECTION_CONFIRM_1, valueOff);
	iUI >>= 1;
	// Bit 1
	if (iUI & 1)
		forgesight_set_io_status(UO_SELECTION_CONFIRM_2, valueOn);
	else
		forgesight_set_io_status(UO_SELECTION_CONFIRM_2, valueOff);
	iUI >>= 1;
	// Bit 2
	if (iUI & 1)
		forgesight_set_io_status(UO_SELECTION_CONFIRM_3, valueOn);
	else
		forgesight_set_io_status(UO_SELECTION_CONFIRM_3, valueOff);
	iUI >>= 1;
	// Bit 3
	if (iUI & 1)
		forgesight_set_io_status(UO_SELECTION_CONFIRM_4, valueOn);
	else
		forgesight_set_io_status(UO_SELECTION_CONFIRM_4, valueOff);
	iUI >>= 1;
	// Bit 4
	if (iUI & 1)
		forgesight_set_io_status(UO_SELECTION_CONFIRM_5, valueOn);
	else
		forgesight_set_io_status(UO_SELECTION_CONFIRM_5, valueOff);
	iUI >>= 1;
	// Bit 5
	if (iUI & 1)
		forgesight_set_io_status(UO_SELECTION_CONFIRM_6, valueOn);
	else
		forgesight_set_io_status(UO_SELECTION_CONFIRM_6, valueOff);
	iUI >>= 1;
#if 0
	// Bit 6
	value.setFloatValue(0.0);
	value = forgesight_get_io_status("ui[6]");
	if(value.getFloatValue() != 0.0)   iRet = iRet + 64 ;
	// Bit 7
	value.setFloatValue(0.0);
	value = forgesight_get_io_status("ui[7]");
	if(value.getFloatValue() != 0.0)   iRet = iRet + 128 ;
#endif
}

static void clear_selection_confirm_value()
{
	int iRet = 0 ;
	eval_value value ;
	// Bit 0
	value.setFloatValue(0.0);
	forgesight_set_io_status(UO_SELECTION_CONFIRM_1, value);
	// Bit 1
	forgesight_set_io_status(UO_SELECTION_CONFIRM_2, value);
	// Bit 2
	forgesight_set_io_status(UO_SELECTION_CONFIRM_3, value);
	// Bit 3
	forgesight_set_io_status(UO_SELECTION_CONFIRM_4, value);
	// Bit 4
	forgesight_set_io_status(UO_SELECTION_CONFIRM_5, value);
	// Bit 5
	forgesight_set_io_status(UO_SELECTION_CONFIRM_6, value);
#if 0
	// Bit 6
	value = forgesight_get_io_status("ui[6]");
	// Bit 7
	value = forgesight_get_io_status("ui[7]");
#endif
}

#ifdef WIN32
unsigned __stdcall launch_code_thread(void* arg)
#else
void* launch_code_thread(void* arg)
#endif
{
	bool bIsSelectionStrobe = false ;
	int iRet = 0 ; 
	eval_value value ;
	thread_control_block * objThdCtrlBlockPtr = NULL;
	g_launch_code_mgr_ptr = new LaunchCodeMgr();
	while(1)
	{
		// According to chapter 2.4 in the Document(0110401110300)
		// Step1: Get Selection Strobe pulse
		value.setFloatValue(0.0);
		value = forgesight_get_io_status(UI_SELECTION_STROBE);
		if(value.getFloatValue() != 0.0)
		{
			bIsSelectionStrobe = true ;
			// Step2: Get Program Selection 1-6
			iRet = get_program_selection_value();
			// Step3: Response to Selection Confirm
			set_selection_confirm_value(iRet);
			// Step3: Pull up the Selection Check Request
			value.setFloatValue(1.0);
			forgesight_set_io_status(UO_SELECTION_CHECK_REQUEST, value);
#ifdef USE_FAKE_PLC
			// Step4: Send MPLCS Start
			value.setFloatValue(1.0);
			forgesight_set_io_status(UI_MPLCS_START, value);
#endif
		}
		else
		{
			bIsSelectionStrobe = false ;
		}
		// Step5: Find main program
		value.setFloatValue(0.0);
		value = forgesight_get_io_status(UI_MPLCS_START);
		if(value.getFloatValue() != 0.0)
		{
			std:: string strRet = g_launch_code_mgr_ptr->getProgramByCode(iRet);
			// Step6: Send MPLCS Start Done
			value.setFloatValue(1.0);
			forgesight_set_io_status(UO_MPLCS_START_DONE, value);
			if(strRet != "")
			{
	            printf("start run...\n");
				g_iCurrentThreadSeq++ ;
				if(g_iCurrentThreadSeq < 0) break ;
			    objThdCtrlBlockPtr = &g_thread_control_block[g_iCurrentThreadSeq];
				
	            objThdCtrlBlockPtr->prog_mode = FULL_MODE;
				objThdCtrlBlockPtr->execute_direction = EXECUTE_FORWARD ;
	            setPrgmState(EXECUTE_R);
				if(strlen(strRet.c_str()) == 0)
				{
					startFile(objThdCtrlBlockPtr, "sr_test", g_iCurrentThreadSeq);
				}
				else 
				{
					startFile(objThdCtrlBlockPtr, (char *)strRet.c_str(), g_iCurrentThreadSeq);
				}
			}
#ifdef USE_FAKE_PLC
			// Step7: Pull off the Selection Strobe
			value.setFloatValue(0.0);
			forgesight_set_io_status(UI_SELECTION_STROBE, value);
			// Step7: Pull off the MPLCS Start
			value.setFloatValue(0.0);
			forgesight_set_io_status(UI_MPLCS_START, value);
			// Step7: Pull off the Program Selection
			clear_program_selection_value();
#endif
		}
		// SelectionStrobe was pulled up
		if(bIsSelectionStrobe)
		{
			value.setFloatValue(0.0);
			value = forgesight_get_io_status(UI_SELECTION_STROBE);
			// SelectionStrobe was pulled up and disappears now.
			if(value.getFloatValue() == 0.0)
			{
				value.setFloatValue(0.0);
				// Step 8: Pull down Selection Check Request
				forgesight_set_io_status(UO_SELECTION_CHECK_REQUEST, value);
				// Step 8: Pull down Selection Confirm 
				clear_selection_confirm_value();
				// Step 8: Pull down MPLCS Start Done
				forgesight_set_io_status(UO_MPLCS_START_DONE, value);
			}
		}
		// g_launch_code_mgr_ptr.updateAll();
#ifdef WIN32
		Sleep(100);
#else
		usleep(1000);
#endif
	}
	delete g_launch_code_mgr_ptr;

#ifdef WIN32
	CloseHandle( g_launch_code_interpreter_handle );  
	g_launch_code_interpreter_handle = NULL; 
	return NULL;
#else
	printf("Enter pthread_join.\n");
	pthread_join(g_launch_code_interpreter_handle, NULL);
	printf("Left  pthread_join.\n");
	fflush(stdout);
	g_launch_code_interpreter_handle = 0;
#endif // WIN32
}


bool launch_code_thread_create(void * args)
{
	bool ret = false;
#ifdef WIN32
	g_launch_code_interpreter_handle =
		(HANDLE)_beginthreadex(NULL, 0, launch_code_thread, args, 0, NULL);
	if (NULL != g_launch_code_interpreter_handle)
	{
		ret = true;
	}
#else
	if (0 == pthread_create(
		&(g_launch_code_interpreter_handle), NULL, launch_code_thread, args))
	{
		ret = true;
	}
	else
	{
        printf("start launch_code_thread_create Failed..\n");
		g_launch_code_interpreter_handle = 0;
	}
#endif
	return ret;
}

void launch_code_thread_destroy()
{
#ifdef WIN32
	WaitForSingleObject(g_launch_code_interpreter_handle, INFINITE);
	if (NULL != g_launch_code_interpreter_handle)
	{
		CloseHandle(g_launch_code_interpreter_handle);
	}
	g_launch_code_interpreter_handle = NULL;
#else
	pthread_join(g_launch_code_interpreter_handle, NULL);
	g_launch_code_interpreter_handle = 0;
#endif // WIN32
}


