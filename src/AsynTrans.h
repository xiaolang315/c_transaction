//
// Created by zhangchao on 2020/2/7.
//

#ifndef C_TRANSACTION_ASYNTRANS_H
#define C_TRANSACTION_ASYNTRANS_H

#include "Transaction.h"
#include "Context.h"
#include "ExternC.h"

EXTERN_STDC_BEGIN

typedef struct AsynContext {
    Context*  context;
    int actionIndex;
} AsynContext;

TransResult start(Transaction*, AsynContext*);
TransResult asyn_exec(Transaction*, AsynContext*);

EXTERN_STDC_END

#endif //C_TRANSACTION_ASYNTRANS_H
