//
// Created by zhangchao on 2020/2/7.
//
#include "AsynTrans.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>

BOOL initRuntimeActions(AsynContext* context, ActionDesc* actions, int actionNum){

    RuntimeAction* runtimeAction = (RuntimeAction*)malloc(actionNum * sizeof(RuntimeAction));
    if(runtimeAction == NULL) return FALSE;

    RuntimeAction* current = &context->current;
    FOREACH(ActionDesc, action, actions, actionNum)
        current->action = action->action;
        current->next = &runtimeAction[actioni];
        current = current->next;
    FOREACH_END()
    current->next = NULL;
    current->action = NULL;
    context->runtimeActions = runtimeAction;
    return TRUE;
}

void destoryAsynContext(const AsynContext* context) {
    destroyContext(context->context);
    free(context->context);
    free(context->runtimeActions);
}

TransResult start(Transaction* trans, AsynContext* outContext) {
    Context* context = (Context*)malloc(sizeof(Context));
    if(context == NULL) return TransFail;

    BOOL ret = initContext(context, trans->actions, trans->actionNum);
    if(ret == FALSE) {
        free(context);
        return TransFail;
    }
    outContext->context = context;

    ret = initRuntimeActions(outContext, trans->actions, trans->actionNum);
    if(ret == FALSE) {
        destoryAsynContext(outContext);
        return TransFail;
    }

    return asyn_exec(outContext);
}

TransResult asyn_exec(AsynContext* asynContext) {
    do{
        ActionResult ret = asynContext->current.action(asynContext->context);
        if(ret == ActionErr){
            rollback(&asynContext->context->rollbackData);
            destoryAsynContext(asynContext);
            return TransFail;
        } else if(ret == ActionContinue) {
            return TransContinue;
        }
        asynContext->current = *asynContext->current.next;
    } while(asynContext->current.action != NULL);
    destoryAsynContext(asynContext);
    return TransSucc;
}

