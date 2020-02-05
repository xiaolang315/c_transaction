#include "Transaction.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>
#include "BaseType.h"

static void rollback(RollbackContext* context) {
    FOREACH(OneRollBackContext, ctxt, context->contexts, context->num)
        ctxt->action(&ctxt->data);
        free(ctxt->data.mem);
    FOREACH_END()
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


