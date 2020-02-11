//
// Created by zhangchao on 2020/2/7.
//
#include "AsynTransaction.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>

static BOOL initRuntimeActions(AsynContext* context, ActionDesc* actions, uint32_t actionNum){

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

TransResult asynStart(const Transaction* trans, Context** outContext) {
    Context* context = initContext(trans->actions, trans->actionNum);
    if(context == NULL) {
        destroyContext(context);
        return TransFail;
    }
    *outContext = context;

    BOOL ret = initRuntimeActions(context->asynContext, trans->actions, trans->actionNum);
    if(ret == FALSE) {
        destroyContext(context);
        return TransFail;
    }

    return asynExec(context);
}

static TransResult doActions(AsynContext* asynContext, Context* context) {
    do{
        ActionResult ret = asynContext->current.action(context);
        if(ret == ActionErr){
            rollback(&context->rollbackData);
            destroyContext(context);
            return TransFail;
        } else if(ret == ActionContinue) {
            return TransContinue;
        }
        asynContext->current = *asynContext->current.next;
    } while(asynContext->current.action != NULL);
    return TransSucc;
}

TransResult asynExec(Context* context)
{
    if(context == NULL || context->asynContext == NULL) return TransFail;
    TransResult ret = doActions(context->asynContext, context);
    if(ret != TransSucc) {
        return ret;
    }
    destroyContext(context);
    return TransSucc;
}

static TransResult asynExecInAction(Context* context, Context* parent)
{
    TransResult ret = doActions(context->asynContext, context);
    if(ret != TransSucc) {
        return ret;
    }

    upToParent(parent, context);
    destroyContext(context);
    return TransSucc;
}

TransResult asynActionStart(Context* parentContext, PrepareChildCtxtFunc prepare, Transaction* trans){
    Context** subContext = &parentContext->asynContext->current.context;
    if (*subContext == NULL) {
        Context* childContext = initContext(trans->actions, trans->actionNum);
        if(childContext == NULL) {
            destroyContext(childContext);
            return TransFail;
        }
        prepare(childContext, *subContext);
        *subContext = childContext;

        BOOL ret = initRuntimeActions(childContext->asynContext, trans->actions, trans->actionNum);
        if(ret == FALSE) {
            destroyContext(childContext);
            return TransFail;
        }
    }

    return asynExecInAction(*subContext, parentContext);
}


