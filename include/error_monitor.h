/**
 * @file error_monitor.h
 * @brief 
 * @author WangWei
 * @version 1.0.0
 * @date 2017-07-21
 */
#ifndef RCS_ERROR_H_ 
#define RCS_ERROR_H_

#include "common.h"
#include "lockfree_queue.h"

#define MAX_ERRORS  (10)

namespace  rcs
{
	class Error
    {
	  public:
		Error();
		~Error();
        static Error* instance();
        void clear();
        bool add(U64 code);
        std::string getErrorBytes();
        bool updated();
        bool isInitError();
        int getWarningLevel();
	  private:
        std::atomic_bool    updated_flag_;
        std::atomic_bool    init_err_flag_;
        std::atomic_int     warning_level_; //warning level 
        std::atomic_int     err_cnt_;   //number of error codes
        std::atomic_ullong  pre_code_;  //previous error code
        LFQueue<U64>        err_queue_; //queue to store error codes
        LFQueue<U64>        init_err_qu_; //store init error codes
	};
}

#endif // RCS_ERROR_H_
