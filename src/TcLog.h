//
// Created by zhangchao on 2020/2/29.
//

#ifndef C_TRANSACTION_TCLOG_H
#define C_TRANSACTION_TCLOG_H

#include "mcl/stdc.h"
#include "mcl/log.h"

MCL_STDC_BEGIN

#define LOG_I(...) MCL_INFO( __VA_ARGS__)
#define LOG_E(...) MCL_ERR( __VA_ARGS__)

MCL_STDC_END


#endif //C_TRANSACTION_LOG_H
