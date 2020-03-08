//
// Created by zhangchao on 2020/2/7.
//
#include "AsynTransaction.h"
#include "AsynContext.h"
#include "TcLog.h"

TransResult asynStart(const Transaction* trans, Context** outContext) {
    LOG_I("%s is start", trans->name);
    Context* context = initContext(trans->actions, trans->actionNum);
    if(context == NULL) {
        LOG_E("AsynAction initContext fail!");
        return TransFail;
    }
    *outContext = context;
    return asynExec(context);
}

static TransResult doActions(AsynContext* asynContext, Context* context) {
    do{
        ActionResult ret = asynContext->current.action(context);
        if(ret == ActionErr){
            LOG_E("AsynAction exec fail!");
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

TransResult asynActionStart(Context* parentContext, PrepareChildCtxtFunc prepare, const Transaction* trans){
    RuntimeAction* current = &parentContext->asynContext->current;
    Context* subContext = current->context;
    if (subContext == NULL) {
        Context* childContext = initContext(trans->actions, trans->actionNum);
        if(childContext == NULL) {
            LOG_E("asynActionStart fail because initContext Fail");
            return TransFail;
        }
        prepare(childContext, parentContext);
        current->context = childContext;
    }

    return asynExecInAction(current->context , parentContext);
}


