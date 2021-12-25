//
// Created by zhangchao on 2020/2/7.
//

#ifndef BF900DB7_5C77_41EC_BDF8_1A350218CE90
#define BF900DB7_5C77_41EC_BDF8_1A350218CE90

#include "Transaction.h"
#include "Context.h"

MCL_STDC_BEGIN

TransResult asynStart(const Transaction*, Context**);
TransResult asynExec(Context*);

TransResult asynActionStart(Context* parentContext, PrepareChildCtxtFunc , const Transaction* trans);

#define ASYN_SUB_TRANS(name, actions)\
ActionDesc name##_subActions[] = actions;\
SUB_TRANS(name){\
    Transaction trans = TRANSACTION_DEF(#name, name##_subActions);\
    return toActionResult(asynActionStart(context, NoPrepareChildCtxtFunc, &trans));\
}

MCL_STDC_END

#endif /* BF900DB7_5C77_41EC_BDF8_1A350218CE90 */
