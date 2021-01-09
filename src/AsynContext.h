//
// Created by zhangchao on 2020/2/28.
//

#ifndef C_TRANSACTION_ASYNCONTEXT_H
#define C_TRANSACTION_ASYNCONTEXT_H

#include "Action.h"
#include "mcl/fwd_decl.h"

MCL_FWD_DECL(Context);

typedef struct RuntimeAction {
    Action action;
    struct RuntimeAction* next;
    Context* context;
} RuntimeAction;

typedef struct AsynContext {
    RuntimeAction current;
} AsynContext;

AsynContext*  initAsynContext(ActionDesc* actions, uint32_t actionNum);
void destroyAsynContext(AsynContext* );

#endif //C_TRANSACTION_ASYNCONTEXT_H
