//
// Created by zhangchao on 2020/2/28.
//

#include "RollbackContext.h"
#include "MemManager.h"
#include "Foreach.h"
#include <memory.h>
#include "MemGuard.h"
#include "MemHelp.h"
#include "TcLog.h"

typedef struct OneRollBackContext {
    RollbackData data;
    RollBackAction action;
} OneRollBackContext;

typedef struct RollbackContext {
    uint32_t num;
    uint32_t maxNum;
    OneRollBackContext* contexts;
    struct RollbackContext* next;
} RollbackContext;

BOOL addRollBackAction(RollbackContext* context, RollBackAction action, const RollbackData* data){
    if(context->num == context->maxNum) return FALSE;
    context->contexts[context->num].action = action;

    RollbackData* toData = &context->contexts[context->num].data;

    toData->mem = mallocTc(data->len);
    if(toData->mem == NULL) return FALSE;

    memcpy(toData->mem, data->mem, data->len);
    toData->len = data->len;
    context->num++;
    return TRUE;
}

void rollback(RollbackContext* context) {
    if(context->num != 0) {
        LOG_I("start rollback with [%x] rollback action", context->num);
        FOREACH(OneRollBackContext, ctxt, context->contexts, context->num)
            ctxt->action(&ctxt->data);
        FOREACH_END()
    }
    if(context->next) {
        rollback(context->next);
    }
}

RollbackContext* initRollBackCtxt(uint32_t actionNum){
    MEM_GUARD(2);
    STRUCT_ALLOC(RollbackContext, rollbackContext);
    CHECK_PTR_R(rollbackContext, NULL);

    rollbackContext->next = NULL;
    rollbackContext->num = 0;
    rollbackContext->maxNum = actionNum;

    ARRAY_ALLOC(OneRollBackContext, rollbackContext->contexts, actionNum);
    CHECK_PTR_R(rollbackContext->contexts, NULL);

    return rollbackContext;
}

void destroyRollbackData(RollbackContext* context) {
    if(context == NULL) return;
    if(context->contexts != NULL) {
        FOREACH(OneRollBackContext, ctxt, context->contexts, context->num)
            CHECK_FREE(ctxt->data.mem);
        FOREACH_END()
        freeTc(context->contexts);
        context->contexts = NULL;
    }
    if(context->next) {
        destroyRollbackData(context->next);
        context->next = NULL;
    }
    freeTc(context);
}



void appendRollBackContext(RollbackContext* current, RollbackContext* next){
    current->next = next;
}

