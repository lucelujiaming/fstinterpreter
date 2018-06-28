////////////////////////////////////////////////////////////////
//
//Descript: auto lock.
//  Author: guowenyan
//    Date: 2013.12.16
//
////////////////////////////////////////////////////////////////
#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#include "forsight_manual_lock.h"

class auto_lock
{
	base_lock *m_p_base_lock;
public:
	auto_lock(base_lock *p_base_lock) : m_p_base_lock(p_base_lock) 
	{
		if (m_p_base_lock)  
			m_p_base_lock->lock();
	}

	~auto_lock()
	{
		if (m_p_base_lock)  
			m_p_base_lock->unlock();
	}
};

#endif //_AUTO_LOCK_H_
