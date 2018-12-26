/**
 * @file common.h
 * @brief 
 * @author WangWei
 * @version 1.0.0
 * @date 2016-08-17
 */
#ifndef INTERPRETER_COMMON_H_
#define INTERPRETER_COMMON_H_

#ifdef WIN32
#define FST_INFO    printf("\n"),printf
#define FST_ERROR   printf("\n"),printf
#define FST_WARN    printf("\n"),printf
#else 
#include "common_log.h"
#endif


#endif
