//
// Created by zhangchao on 2020/2/7.
//

#ifndef C_TRANSACTION_ASYNTRANSACTION_H
#define C_TRANSACTION_ASYNTRANSACTION_H

#include "Transaction.h"
#include "Context.h"

EXTERN_STDC_BEGIN

TransResult asynStart(const Transaction*, Context**);
TransResult asynExec(Context*);

TransResult asynActionStart(Context* parentContext, PrepareChildCtxtFunc , Transaction* trans);

#define ASYN_SUB_TRANS(name, actions)\
ActionDesc name##_subActions[] = actions;\
SUB_TRANS(name){\
    Transaction trans = TRANSACTION_DEF(name##_subActions);\
    return toActionResult(asynActionStart(context, NoPrepareChildCtxtFunc, &trans));\
}

EXTERN_STDC_END

#endif //C_TRANSACTION_ASYNTRANSACTION_H
