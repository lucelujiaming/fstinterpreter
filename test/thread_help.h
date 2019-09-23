#ifndef THREAD_HELP_H
#define THREAD_HELP_H


#include <pthread.h>
#include <sched.h>
#include <functional>

namespace fst_base
{
typedef void (*threadFunc)(void*);

class ThreadHelp
{
public:
    ThreadHelp();
    ~ThreadHelp();

    bool run(threadFunc func_ptr, void* data, int priority);
    void join();
    

private:
    pthread_t pid_;
};

}

#endif

