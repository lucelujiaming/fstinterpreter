#define MAX_SAFE_STACK (1 * 1024 * 1024)    /* The maximum stack size which is
                                                guaranteed safe to access without
                                                faulting */

void stack_prefault(void) 
{
        unsigned char dummy[MAX_SAFE_STACK];
        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}


static void realTimeTask(void)
{
    
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) 
    {
        FST_ERROR("mlockall failed");
        return MC_FAIL_IN_INIT; 
    }

    //Pre-fault our stack
    stack_prefault();

    // do some work here
}

void main(void)
{
    ThreadHelp rt_thread;
    rt_thread.run(&realTimeTask, NULL, 80);
}