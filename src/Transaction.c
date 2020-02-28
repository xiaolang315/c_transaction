//
// Created by zhangchao on 2020/2/10.
//

#include "Transaction.h"
#include "Foreach.h"
#include "stdlib.h"
#include "MemHelp.h"

ActionResult toActionResult(TransResult ret) {
    switch(ret){
        case TransSucc: return ActionOk;
        case TransFail: return ActionErr;
        case TransContinue: return ActionContinue;
        default: return ActionUnknown;
    }
}

void upToParent(Context* parent, Context* child) {
    RollbackContext* next = (RollbackContext*)mallocTc(sizeof(RollbackContext));
    *next = *child->rollbackData;
    parent->rollbackData->next = next;
    child->rollbackData->contexts = NULL;
    child->rollbackData->num = 0;
    child->rollbackData->next = NULL;
}


void NoPrepareChildCtxtFunc(const Context* parent, Context* child){
    if(child && parent) {
        return;
    }
}

TransResult onActionSucc(Context* context) {
    destroyContext(context);
    return TransSucc;
}

TransResult onActionFail(Context* context) {
    rollback(context->rollbackData);
    destroyContext(context);
    return TransFail;
}
