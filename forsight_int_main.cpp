#ifndef WIN32
#include <unistd.h>
#endif
#include "forsight_inter_control.h"

int main(int  argc, char *argv[])
{
  initShm();

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
	  sleep(1);
#endif
  }
  return 1;
}


