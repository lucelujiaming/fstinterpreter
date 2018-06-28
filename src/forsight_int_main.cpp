#ifndef WIN32
#include <unistd.h>
#endif
#include "forsight_inter_control.h"
#include "forsight_io_mapping.h"
#include "forsight_io_controller.h"

#define IO_ERROR_INTERVAL_COUNT           (5)  //2ms*5=10ms

int main(int  argc, char *argv[])
{
	initShm();
	append_io_mapping();
	forgesight_load_io_config();
#ifndef WIN32
	load_register_data();
#endif
	while(1)
	{
		bool ret = getIntprtCtrl();
		if (ret)
		{
			parseCtrlComand(); //  &g_thread_control_block[0]);
		}
#ifdef WIN32
		Sleep(100);
#else
		usleep(1000);
#endif
	    static int count = 0;
	    if (++count >= IO_ERROR_INTERVAL_COUNT)
	    {
			updateIOError();
	        count = 0;
	    }
	}
	return 1;
}


