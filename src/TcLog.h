//
// Created by zhangchao on 2020/2/29.
//

#ifndef C_TRANSACTION_TCLOG_H
#define C_TRANSACTION_TCLOG_H

#include "ExternC.h"

EXTERN_STDC_BEGIN
#include "log.h"

#define LOG_I(...) log_info( __VA_ARGS__)
#define LOG_E(...) log_error( __VA_ARGS__)

EXTERN_STDC_END


#endif //C_TRANSACTION_LOG_H
