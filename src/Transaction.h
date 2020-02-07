#ifndef __TRANSACTION_H_
#define __TRANSACTION_H_


#include "Action.h"
#include "ExternC.h"
#include "FwdDecl.h"

EXTERN_STDC_BEGIN

typedef struct Transaction {
  ActionDesc* actions;
  unsigned int actionNum;
} Transaction;

#define TRANSACTION_DEF(actions)\
  {actions, ARRAY_SIZE(actions)}

#define ACTIONS(...) {__VA_ARGS__}

#define TRANS(name , actions)\
    ActionDesc name##_actions[] = actions;\
    Transaction name = TRANSACTION_DEF(name##_actions);


typedef enum TransResult {
  TransSucc,
  TransContinue,
  TransFail
}TransResult;

FWD_DECL(RollbackContext);
void rollback(RollbackContext* context);
TransResult exec(const Transaction* trans);
FWD_DECL(Context);
typedef void (*PrepareChildCtxtFunc)(const Context* parent, Context* child);

void NoPrepareChildCtxtFunc(const Context* parent, Context* child);
TransResult subExec(Context* parent, PrepareChildCtxtFunc, const Transaction* trans);

#define SUB_TRANS(name, actions)\
    ActionDesc name##_actions[] = actions;\
    ActionResult name##func(Context* context);\
    ActionDesc name = DEF_NULL_CTXT_ACTION_DESC(name##func);\
    ActionResult name##func(Context* context) {\
        Transaction trans = TRANSACTION_DEF(name##_actions);\
        subExec(context, NoPrepareChildCtxtFunc, &trans);\
        return ActionOk;\
    }

EXTERN_STDC_END

#endif // __TRANSACTION_H_
