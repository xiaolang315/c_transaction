//
// Created by zhangchao on 2020/2/29.
//

#ifndef C_TRANSACTION_ASSERT_H
#define C_TRANSACTION_ASSERT_H

#include "TcLog.h"

#define RETURN(ret) {return ret;};

#define ASSERT_TRUE_DO(cond, statement) \
if(!cond) { \
LOG_E("check [%s] not true",#cond);\
statement }

#define ASSERT_TRUE_R(cond, ret)\
ASSERT_TRUE_DO(cond, RETURN(ret))

#endif //C_TRANSACTION_ASSERT_H
