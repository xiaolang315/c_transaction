//
// Created by zhangchao on 2020/2/10.
//

#ifndef C_TRANSACTION_TRANSACTION_H
#define C_TRANSACTION_TRANSACTION_H

#include "Action.h"
#include "Context.h"
#include "ExternC.h"

EXTERN_STDC_BEGIN

typedef enum TransResult {
    TransSucc,
    TransContinue,
    TransFail
}TransResult;

typedef struct Transaction {
    const char* name;
    ActionDesc* actions;
    uint32_t actionNum;
} Transaction;

#define TRANSACTION_DEF(name, actions)\
  {name, actions, ARRAY_SIZE(actions)}

#define ACTIONS(...) {__VA_ARGS__}

#define TRANS(name , actions)\
    ActionDesc name##_actions[] = actions;\
    Transaction name = TRANSACTION_DEF(#name, name##_actions);

#define SUB_TRANS(name)\
    ActionResult name##func(Context* context);\
    ActionDesc name = DEF_NULL_CTXT_ACTION_DESC(name##func, );\
    ActionResult name##func(Context* context)

ActionResult toActionResult(TransResult ret) ;
TransResult onActionFail(Context* context);
TransResult onActionSucc(Context* context);
void upToParent(Context* parent, Context* child);


typedef void (*PrepareChildCtxtFunc)(const Context* parent, Context* child);
void NoPrepareChildCtxtFunc(const Context* parent, Context* child);

EXTERN_STDC_END

#endif //C_TRANSACTION_TRANSACTION_H
