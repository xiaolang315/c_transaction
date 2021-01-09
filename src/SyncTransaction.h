#ifndef __TRANSACTION_H_
#define __TRANSACTION_H_


#include "Transaction.h"

MCL_STDC_BEGIN

TransResult syncExec(const Transaction* trans);
ActionResult syncSubTransActionExec(Context* parent, PrepareChildCtxtFunc, const Transaction* trans);

#define SYNC_SUB_TRANS(name, actions)\
    ActionDesc name##_actions[] = actions;\
    SUB_TRANS(name){\
        Transaction trans = TRANSACTION_DEF(#name, name##_actions);\
        syncSubTransActionExec(context, NoPrepareChildCtxtFunc, &trans);\
        return ActionOk;\
    }

#define SYNC_SUB_TRANS_UP(name, actions)\
    ActionDesc name##_actions[] = actions;\
    SUB_TRANS(name){\
        Transaction trans = TRANSACTION_DEF(#name, name##_actions);\
        return syncSubTransActionExec(context, NoPrepareChildCtxtFunc, &trans);\
    }


MCL_STDC_END

#endif // __TRANSACTION_H_
