//
// Created by zhangchao on 2020/2/3.
//
#include "Context.h"
#include "BaseType.h"
#include "Foreach.h"
#include "Action.h"
#include "MemHelp.h"
#include <memory.h>

static int getOffSet(const ContextMap* map,  uint32_t id){
    FOREACH(ContextSet, set, map->hash, map->hashNum)
        if(id == set->id) return set->offset;
    FOREACH_END()
    return -1;
}

static void* castTo(const ContextMap* map, void* data, uint32_t id){
    return data + getOffSet(map, id);
}

static void insertHash(ContextMap* map, uint32_t offset, uint32_t id){
    map->hash[map->hashNum].id = id;
    map->hash[map->hashNum].offset = offset;
    map->hashNum ++;
}

static void getLength(ContextMap* map, uint32_t* length, CtxtActionUse* context){
    int offSet = getOffSet(map, context->id);
    if(offSet == -1) {
        insertHash(map, *length, context->id);
        *length += context->size;
    }
}

BOOL AddRollBack(RollbackContext* context, RollBackAction action, const RollbackData* data){
    if(context->num == context->maxNum) return FALSE;
    context->contexts[context->num].action = action;

    RollbackData* toData = &context->contexts[context->num].data;

    toData->mem = malloc(data->len);
    if(toData->mem == NULL) return FALSE;

    memcpy(toData->mem, data->mem, data->len);
    toData->len = data->len;
    context->num++;
    return TRUE;
}

BOOL includeAsynAction(ActionDesc* actions, uint32_t actionNum) {
    FOREACH(ActionDesc, action, actions, actionNum)
        if(action->type == AsynActionType) {
            return TRUE;
        }
    FOREACH_END()

    return FALSE;
}

static uint32_t initLength(ContextMap* map, ActionDesc* actions, uint32_t actionNum) {
    uint32_t length = 0;
    map->hashNum = 0;

    FOREACH(ActionDesc, action, actions, actionNum)
        FOREACH(CtxtActionUse, context, action->contexts, action->ctxtNum)
            getLength(map, &length, context);
        FOREACH_END()
    FOREACH_END()

    return length;
}

static void initRollBackCtxt(RollbackContext* context, uint32_t actionNum){
    context->next = NULL;
    context->num = 0;
    context->maxNum = actionNum;
}

static void constructContext(Context* context) {
    context->asynContext = NULL;
    context->castTo = castTo;
    context->data = NULL;
}

Context* initContext(ActionDesc* actions, uint32_t actionNum) {
    MEM_GUARD(7);

    STRUCT_ALLOC(Context, context);
    CHECK_PTR_R(context, NULL);
    constructContext(context);

    BOOL isAsyn = includeAsynAction(actions, actionNum);
    if(isAsyn == TRUE) {
        STRUCT_ALLOC(AsynContext, asynContext);
        CHECK_PTR_R(asynContext , NULL);
        asynContext->runtimeActions = NULL;
        context->asynContext = asynContext;
    }

    ARRAY_ALLOC(ContextSet, context->map.hash , actionNum);
    CHECK_PTR_R(context->map.hash , NULL);

    uint32_t length = initLength(&context->map, actions, actionNum);
    context->data = malloc(length);
    CHECK_PTR_R(context->data , NULL);

    ARRAY_ALLOC(OneRollBackContext, context->rollbackData.contexts , actionNum);
    CHECK_PTR_R(context->rollbackData.contexts , NULL);

    initRollBackCtxt(&context->rollbackData, actionNum);

    return context;
}

void destroyContext(Context* context) {
    CHECK_FREE(context->data);
    CHECK_FREE(context->rollbackData.contexts);
    if(context->asynContext) {
        CHECK_FREE(context->asynContext->runtimeActions);
        free(context->asynContext);
    }
    CHECK_FREE(context->map.hash);
    CHECK_FREE(context);
}