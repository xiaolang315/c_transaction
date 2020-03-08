//
// Created by zhangchao on 2020/2/10.
//

#include "Transaction.h"
#include "RollbackContext.h"
#include "TcLog.h"
#include "Cello.h"

ActionResult toActionResult(TransResult ret) {
    var p0 = $(Float, 0.0);
    switch(ret){
        case TransSucc: return ActionOk;
        case TransFail: return ActionErr;
        case TransContinue: return ActionContinue;
        default:
            LOG_E("unknown TransResult ");
            return ActionUnknown;
    }
}

void upToParent(Context* parent, Context* child) {
    appendRollBackContext(parent->rollbackData, child->rollbackData);
    child->rollbackData = NULL;
}

void NoPrepareChildCtxtFunc(const Context* parent, Context* child){
    if(child && parent) {
        return;
    }
}

TransResult onActionSucc(Context* context) {
    LOG_I("Action Exec Succ");
    destroyContext(context);
    return TransSucc;
}

TransResult onActionFail(Context* context) {
    LOG_E("Action Exec Fail");
    rollback(context->rollbackData);
    destroyContext(context);
    return TransFail;
}
