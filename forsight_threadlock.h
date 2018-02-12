//一个简单的线程锁
#ifndef _THREADLOCK_H 
#define _THREADLOCK_H 

#ifdef WIN32 
#include <Windows.h> 
#else 
#include <pthread.h> 
#endif 

#ifdef WIN32 
	#define LOCK_MUTEXT CRITICAL_SECTION
	#define INIT_LOCK(X) InitializeCriticalSection(X)
	#define DO_LOCK(X) EnterCriticalSection(X)
	#define DO_UNLOCK(X) LeaveCriticalSection(X)
	#define DESTROY_LOCK(X) DeleteCriticalSection(X)
#else 
	#define LOCK_MUTEXT pthread_mutex_t
	#define INIT_LOCK(X) pthread_mutex_init(X,NULL)
	#define DO_LOCK(X) pthread_mutex_lock(X)
	#define DO_UNLOCK(X) pthread_mutex_unlock(X)
	#define DESTROY_LOCK(X) pthread_mutex_destroy(X)
#endif 

class CThreadLock 
{ 
public: 
    CThreadLock(void) 
    { 
        Init(); 
    }; 

    ~CThreadLock() 
    { 
        Close(); 
    }; 

    void Init() 
    { 
        INIT_LOCK(&m_lock);
    }; 

    void Close() 
    { 
        DESTROY_LOCK(&m_lock);
    } 

    void Lock() 
    { 
        DO_LOCK(&m_lock);
    }; 

    void UnLock() 
    { 
        DO_UNLOCK(&m_lock);
    }; 

private: 
    LOCK_MUTEXT m_lock; 
}; 

//自动加锁的类 
class CAutoLock 
{ 
public: 
    CAutoLock(CThreadLock* pThreadLock)  
    {  
        m_pThreadLock = pThreadLock; 
        if(NULL != m_pThreadLock) 
        { 
            m_pThreadLock->Lock(); 
        } 
    }; 
    ~CAutoLock()  
    { 
        if(NULL != m_pThreadLock) 
        { 
            m_pThreadLock->UnLock(); 
        } 
    }; 
    
private: 
    CThreadLock* m_pThreadLock; 
}; 

#endif

