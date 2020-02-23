#include <stddef.h>
#include "SyncTransaction.h"
#include "Context.h"
#include "Foreach.h"

static ActionResult execActions(ActionDesc* actions, uint32_t actionNum, Context* context) {
    FOREACH(ActionDesc, action, actions, actionNum)
        ActionResult ret = action->action(context);
        if(ret != ActionOk){
            return ret;
        }
    FOREACH_END()
    return ActionOk;
}

TransResult syncExec(const Transaction* trans){
    Context* context = initContext(trans->actions, trans->actionNum);
    if(context == NULL) return TransFail;

    ActionResult result = execActions(trans->actions, trans->actionNum, context);
    if(result == ActionErr) {
        return onActionFail(context);
    }

    return onActionSucc(context);
}

ActionResult syncSubTransActionExec(Context* parent, PrepareChildCtxtFunc prepare, const Transaction* trans){
    Context* context = initContext(trans->actions, trans->actionNum);
    if(context == NULL) return ActionErr;

    prepare(parent, context);

    ActionResult result = execActions(trans->actions, trans->actionNum, context);
    if(result == ActionErr) {
        return toActionResult(onActionFail(context));
    }

    upToParent(parent, context);
    return toActionResult(onActionSucc(context));
}



