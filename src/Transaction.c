#include "Transaction.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>
#include "BaseType.h"

void rollback(RollbackContext* context) {
    FOREACH(OneRollBackContext, ctxt, context->contexts, context->num)
        ctxt->action(&ctxt->data);
        free(ctxt->data.mem);
    FOREACH_END()
    if(context->next) {
        rollback(context->next);
        free((context->next));
    }
}

TransResult exec(const Transaction* trans){
    Context context;
    BOOL ret = initContext(&context, trans->actions, trans->actionNum);
    if(ret == FALSE) return TransFail;

    FOREACH(ActionDesc, action, trans->actions, trans->actionNum)
        ActionResult ret = action->action(&context);
        if(ret != ActionOk){
            rollback(&context.rollbackData);
            destroyContext(&context);
            return TransFail;
        }
    FOREACH_END()

    destroyContext(&context);
    return TransSucc;
}

void NoPrepareChildCtxtFunc(const Context* parent, Context* child){

}

static void upToParent(Context* parent, Context* child) {
    RollbackContext* next = (RollbackContext*)malloc(sizeof(RollbackContext));
    *next = child->rollbackData;
    parent->rollbackData.next = next;
    child->rollbackData.contexts = NULL;
}

ActionResult subExec(Context* parent, PrepareChildCtxtFunc prepare, const Transaction* trans){
    Context context;
    BOOL ret = initContext(&context, trans->actions, trans->actionNum);
    if(ret == FALSE) return ActionOk;

    prepare(parent, &context);

    FOREACH(ActionDesc, action, trans->actions, trans->actionNum)
        ActionResult ret = action->action(&context);
        if(ret != ActionOk){
            rollback(&context.rollbackData);
            destroyContext(&context);
            return ret;
        }
    FOREACH_END()
    upToParent(parent, &context);
    destroyContext(&context);
    return ActionOk;
}



