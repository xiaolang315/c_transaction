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
        current->context = NULL;
        current = current->next;
    FOREACH_END()
    current->next = NULL;
    current->action = NULL;
    current->context = NULL;
    context->runtimeActions = runtimeAction;
    return TRUE;
}

TransResult start(Transaction* trans, Context** outContext) {
    Context* context = (Context*)malloc(sizeof(Context));
    if(context == NULL) return TransFail;

    BOOL ret = initContext(context, trans->actions, trans->actionNum);
    if(ret == FALSE) {
        free(context);
        return TransFail;
    }
    *outContext = context;

    ret = initRuntimeActions(context->asynContext, trans->actions, trans->actionNum);
    if(ret == FALSE) {
        destroyContext(context);
        return TransFail;
    }

    return asyn_exec(context);
}

TransResult asyn_exec(Context* context) {
    do{
        ActionResult ret = context->asynContext->current.action(context);
        if(ret == ActionErr){
            rollback(&context->rollbackData);
            destroyContext(context);
            return TransFail;
        } else if(ret == ActionContinue) {
            return TransContinue;
        }
        context->asynContext->current = *context->asynContext->current.next;
    } while(context->asynContext->current.action != NULL);
    destroyContext(context);
    return TransSucc;
}

ActionResult toActionResult(TransResult ret) {
    switch(ret){
        case TransSucc: return ActionOk;
        case TransFail: return ActionErr;
        case TransContinue: return ActionContinue;
        default: return ActionUnknow;
    }
}

TransResult doAsync(Context* context, Transaction* trans){
    Context** subContext = &context->asynContext->current.context;
    if (*subContext == NULL) {
        return start(trans, subContext);
    } else {
        return asyn_exec(*subContext);
    }
}


