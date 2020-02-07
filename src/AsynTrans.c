//
// Created by zhangchao on 2020/2/7.
//
#include "AsynTrans.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>

TransResult start(Transaction* trans, AsynContext* outContext) {
    Context* context = (Context*)malloc(sizeof(Context));
    BOOL ret = initContext(context, trans->actions, trans->actionNum);
    if(ret == FALSE) return TransFail;

    outContext->context = context;

    FOREACH(ActionDesc, action, trans->actions, trans->actionNum)
        ActionResult ret = action->action(context);
        if(ret == ActionErr){
            rollback(&context->rollbackData);
            destroyContext(context);
            return TransFail;
        } else if(ret == ActionContinue) {
            outContext->actionIndex = (action - trans->actions)/ sizeof(ActionDesc);
            return TransContinue;
        }
    FOREACH_END()

    destroyContext(context);
    return TransSucc;
}

TransResult asyn_exec(Transaction* trans, AsynContext* asynContext) {
    FOREACH_FROM(ActionDesc, action, trans->actions, asynContext->actionIndex, trans->actionNum)
        ActionResult ret = action->action(asynContext->context);
        if(ret == ActionErr){
            rollback(&asynContext->context->rollbackData);
            destroyContext(asynContext->context);
            return TransFail;
        } else if(ret == ActionContinue) {
            return TransContinue;
        }
    FOREACH_END()
    destroyContext(asynContext->context);
    return TransSucc;
}

