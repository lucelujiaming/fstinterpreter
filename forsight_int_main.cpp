#ifndef WIN32
#include <unistd.h>
#endif
#include "forsight_inter_control.h"

extern jmp_buf e_buf; /* hold environment for longjmp() */
extern struct thread_control_block g_thread_control_block[NUM_THREAD];

#ifdef WIN32
extern HANDLE    g_basic_interpreter_handle[NUM_THREAD];
#else
extern pthread_t g_basic_interpreter_handle[NUM_THREAD];
#endif

int main(int  argc, char *argv[])
{
  initShm();

  while(1)
  {
     bool ret = getIntprtCtrl();
     if (ret)
     {
        parseCtrlComand(&g_thread_control_block[0]);
     }
#ifdef WIN32
	  Sleep(100);
#else
	  sleep(1);
#endif
  }
  return 1;
}


