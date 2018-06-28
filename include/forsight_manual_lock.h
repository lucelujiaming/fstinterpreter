////////////////////////////////////////////////////////////////
//
//Descript: mannual lock.
//  Author: guowenyan
//    Date: 2013.12.16
//
////////////////////////////////////////////////////////////////
#ifndef _MANUAL_LOCK_H_
#define _MANUAL_LOCK_H_

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif


class base_lock
{
public:
	virtual ~base_lock() { }

public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
};

#ifdef WIN32
class win_cs_lock : public base_lock
{
	CRITICAL_SECTION m_cs;
public:
	win_cs_lock() { InitializeCriticalSection(&m_cs); }
	virtual ~win_cs_lock() { DeleteCriticalSection(&m_cs); }

	virtual void lock() { EnterCriticalSection(&m_cs); }
	virtual void unlock() { LeaveCriticalSection(&m_cs); }
};

#else
class linux_mutex_lock : public base_lock
{
	pthread_mutex_t m_mutex;
public:
	linux_mutex_lock() { pthread_mutex_init(&m_mutex, NULL); }
	virtual ~linux_mutex_lock() { pthread_mutex_destroy(&m_mutex); }

	virtual void lock() { pthread_mutex_lock(&m_mutex); }
	virtual void unlock() { pthread_mutex_unlock(&m_mutex); }
};
#endif


#endif  //_MANUAL_LOCK_H_
