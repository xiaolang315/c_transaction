#include "SyncTransaction.h"
#include "Context.h"
#include "Foreach.h"
#include "BaseType.h"

static TransResult execActions(ActionDesc* actions, uint32_t actionNum, Context* context) {
    FOREACH(ActionDesc, action, actions, actionNum)
        ActionResult ret = action->action(context);
        if(ret != ActionOk){
            return TransFail;
        }
    FOREACH_END()
    return TransSucc;
}

TransResult syncExec(const Transaction* trans){
    Context context;
    BOOL ret = initContext(&context, trans->actions, trans->actionNum);
    if(ret == FALSE) return TransFail;

    TransResult result = execActions(trans->actions, trans->actionNum, &context);
    if(result == TransFail) {
        rollback(&context.rollbackData);
        destroyContext(&context);
        return TransFail;
    }

    destroyContext(&context);
    return TransSucc;
}

ActionResult syncSubTransActionExec(Context* parent, PrepareChildCtxtFunc prepare, const Transaction* trans){
    Context context;
    BOOL ret = initContext(&context, trans->actions, trans->actionNum);
    if(ret == FALSE) return ActionErr;

    prepare(parent, &context);

    TransResult result = execActions(trans->actions, trans->actionNum, &context);
    if(result == TransFail) {
        rollback(&context.rollbackData);
        destroyContext(&context);
        return ActionErr;
    }

    upToParent(parent, &context);
    destroyContext(&context);
    return ActionOk;
}



