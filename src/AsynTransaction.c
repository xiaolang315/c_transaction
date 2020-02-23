//
// Created by zhangchao on 2020/2/7.
//
#include "AsynTransaction.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>
#include "MemHelp.h"

static void initRuntimeAction(RuntimeAction* self, Action action, RuntimeAction* next) {
    self->action = action;
    self->context = NULL;
    self->next = next;
}

static BOOL initRuntimeActions(AsynContext* context, ActionDesc* actions, uint32_t actionNum){
    RuntimeAction* mem = NULL;
    ARRAY_ALLOC(RuntimeAction, mem, actionNum);
    if(mem == NULL) return FALSE;

    RuntimeAction* current = &context->current;
    FOREACH(ActionDesc, action, actions, actionNum)
        initRuntimeAction(current, action->action, &mem[ITEM_INDEX(action)]);
        current = current->next;
    FOREACH_END()
    initRuntimeAction(current, NULL, NULL);

    context->runtimeActionsBuff = mem;
    return TRUE;
}

static  Context* initAsynContext(const Transaction* trans) {
    Context* context = initContext(trans->actions, trans->actionNum);
    if(context == NULL) {
        return NULL;
    }

    BOOL ret = initRuntimeActions(context->asynContext, trans->actions, trans->actionNum);
    if(ret == FALSE) {
        destroyContext(context);
        return NULL;
    }
    return context;
}

TransResult asynStart(const Transaction* trans, Context** outContext) {
    Context* context = initAsynContext(trans);
    if(context == NULL) {
        return TransFail;
    }
    *outContext = context;
    return asynExec(context);
}

static TransResult doActions(AsynContext* asynContext, Context* context) {
    do{
        ActionResult ret = asynContext->current.action(context);
        if(ret == ActionErr){
            onActionFail(context);
            return TransFail;
        } else if(ret == ActionContinue) {
            return TransContinue;
        }
        asynContext->current = *asynContext->current.next;
    } while(asynContext->current.action != NULL);
    return TransSucc;
}

TransResult asynExec(Context* context) {
    if(context == NULL || context->asynContext == NULL) return TransFail;
    TransResult ret = doActions(context->asynContext, context);
    if(ret != TransSucc) {
        return ret;
    }
    return onActionSucc(context);
}

static TransResult asynExecInAction(Context* context, Context* parent) {
    TransResult ret = doActions(context->asynContext, context);
    if(ret != TransSucc) {
        return ret;
    }

    upToParent(parent, context);
    return onActionSucc(context);
}

TransResult asynActionStart(Context* parentContext, PrepareChildCtxtFunc prepare, Transaction* trans){
    RuntimeAction* current = &parentContext->asynContext->current;
    Context* subContext = current->context;
    if (subContext == NULL) {
        Context* childContext = initAsynContext(trans);
        if(childContext == NULL) {
            return TransFail;
        }
        prepare(childContext, parentContext);
        current->context = childContext;
    }

    return asynExecInAction(current->context , parentContext);
}


