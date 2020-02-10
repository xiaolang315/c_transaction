//
// Created by zhangchao on 2020/2/3.
//
#include "Context.h"
#include "memory.h"
#include "stdlib.h"
#include "BaseType.h"
#include "Foreach.h"
#include "Action.h"

static int getOffSet(const ContextMap* map,  uint32_t id){
    for( unsigned int i =0; i< map->hashNum; i++ ) {
        if(id == map->hash[i].id) return map->hash[i].offset;
    }
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

static uint32_t initLength(ContextMap* map, ActionDesc* actions, uint32_t actionNum, BOOL* outAync) {
    uint32_t length = 0;
    BOOL isAsync = FALSE;
    map->hashNum = 0;

    map->hash = malloc(actionNum * sizeof(ContextSet));
    if(map->hash == NULL) return 0;

    FOREACH(ActionDesc, action, actions, actionNum)
        if(action->type == AsynActionType) {
            isAsync = TRUE;
        }
        FOREACH(CtxtActionUse, context, action->contexts, action->ctxtNum)
            getLength(map, &length, context);
        FOREACH_END()
    FOREACH_END()

    *outAync = isAsync;
    return length;
}

BOOL initRollBackCtxt(RollbackContext* context, int actionNum){
    context->contexts = (OneRollBackContext*)malloc(actionNum * sizeof(OneRollBackContext));
    if(context->contexts == NULL) {
        return FALSE;
    }
    context->next = NULL;
    context->num = 0;
    context->maxNum = actionNum;
    return TRUE;
}

BOOL initContext(Context* context, ActionDesc* actions, int actionNum) {
    BOOL isAsyn = FALSE;
    uint32_t length = initLength(&context->map, actions, actionNum, &isAsyn);

    if(isAsyn == TRUE) {
        context->asynContext = (AsynContext*)malloc(sizeof(AsynContext));
        if(context->data == NULL) return FALSE;
        context->asynContext->runtimeActions = NULL;
    } else {
        context->asynContext = NULL;
    }

    char* data = malloc(length);
    if(context->data == NULL) return FALSE;
    memset(data, 0, sizeof(length));

    context->data = data;
    context->castTo = castTo;

    BOOL ret = initRollBackCtxt(&context->rollbackData, actionNum);
    if(ret == FALSE) {
        free(context->data);
        return ret;
    }
    return TRUE;
}

void destroyContext(Context* context) {
    free(context->data);
    if(context->rollbackData.contexts) free(context->rollbackData.contexts);
    if(context->asynContext) {
        if(context->asynContext->runtimeActions) {
            free(context->asynContext->runtimeActions);
        }
        free(context->asynContext);
    }
    free(context->map.hash);
}