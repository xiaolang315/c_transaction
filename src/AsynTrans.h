//
// Created by zhangchao on 2020/2/7.
//

#ifndef C_TRANSACTION_ASYNTRANS_H
#define C_TRANSACTION_ASYNTRANS_H

#include "Transaction.h"
#include "Context.h"

EXTERN_STDC_BEGIN

TransResult start(Transaction*, Context**);
TransResult asyn_exec(Context*);

ActionResult toActionResult(TransResult ret) ;
TransResult doAsync(Context* context, Transaction* trans);

#define ASYN_SUB_TRANS(name, actions)\
ActionDesc name##_actions[] = actions;\
    ActionResult name##func(Context* context);\
    ActionDesc name = DEF_NULL_CTXT_ACTION_DESC(name##func);\
 ActionResult name##func(Context* context) {\
    Transaction trans = TRANSACTION_DEF(name##_actions);\
    return toActionResult(doAsync(context, &trans));\
}

EXTERN_STDC_END

#endif //C_TRANSACTION_ASYNTRANS_H
