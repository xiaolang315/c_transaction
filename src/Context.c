//
// Created by zhangchao on 2020/2/3.
//
#include "Context.h"
#include "Foreach.h"
#include "Action.h"
#include "MemHelp.h"
#include "AsynContext.h"
#include "RollbackContext.h"
#include "MemGuardRollBack.h"

typedef struct ContextSet {
    uint32_t id;
    uint32_t offset;
}ContextSet;

typedef struct ContextMap {
    ContextSet* hash;
    uint32_t hashNum;
} ContextMap;

static int getOffSet(const ContextMap* map,  uint32_t id){
    FOREACH(ContextSet, set, map->hash, map->hashNum)
        if(id == set->id) return set->offset;
    FOREACH_END()
    return -1;
}

typedef struct ContextMem {
    Context context;
    ContextMap map;
} ContextMem;

void* castTo(Context* context, uint32_t id){
    ContextMem* mem = (ContextMem*) context;
    return context->data+ getOffSet(&mem->map, id);
}

static void insertHash(ContextMap* map, uint32_t offset, uint32_t id){
    map->hash[map->hashNum].id = id;
    map->hash[map->hashNum].offset = offset;
    map->hashNum ++;
}

static void getLength(ContextMap* map, uint32_t* length, ActionContext* context){
    int offSet = getOffSet(map, context->id);
    if(offSet == -1) {
        insertHash(map, *length, context->id);
        *length += context->size;
    }
}


BOOL isIncludeAsynAction(ActionDesc* actions, uint32_t actionNum) {
    FOREACH(ActionDesc, action, actions, actionNum)
        if(action->type == AsynActionType) {
            return TRUE;
        }
    FOREACH_END()

    return FALSE;
}

static uint32_t initContextMap(ContextMap* map, ActionDesc* actions, uint32_t actionNum) {
    uint32_t length = 0;
    map->hashNum = 0;

    FOREACH(ActionDesc, action, actions, actionNum)
        FOREACH(ActionContext, context, action->contexts, action->ctxtNum)
            getLength(map, &length, context);
        FOREACH_END()
    FOREACH_END()

    return length;
}

static void constructContext(Context* context) {
    context->asynContext = NULL;
    context->data = NULL;
}

void freeAsynContext(RollbackData* data){
    MemPtr* ptr = (MemPtr*)data->mem;
    destroyAsynContext((AsynContext*)ptr->ptr);
}

void freeRollBack(RollbackData* data){
    MemPtr* ptr = (MemPtr*)data->mem;
    destroyRollbackData((RollbackContext*)ptr->ptr);
}

Context* initContext(ActionDesc* actions, uint32_t actionNum) {
    MEM_GUARD_E_R(7, NULL);

    STRUCT_ALLOC(ContextMem, mem);
    CHECK_PTR_E_R(mem, NULL);

    Context* context = (Context*) mem;
    constructContext(context);

    BOOL isAsyn = isIncludeAsynAction(actions, actionNum);
    if(isAsyn == TRUE) {
        context->asynContext = initAsynContext(actions, actionNum);
        CHECK_PTR_E_FUNC_R(context->asynContext,  freeAsynContext, NULL);
    }

    ARRAY_ALLOC(ContextSet, mem->map.hash , actionNum);
    CHECK_PTR_E_R(mem->map.hash , NULL);

    uint32_t length = initContextMap(&mem->map, actions, actionNum);
    context->data = mallocTc(length);
    CHECK_PTR_E_R(context->data , NULL);

    context->rollbackData = initRollBackCtxt(actionNum);
    CHECK_PTR_E_FUNC_R(&context->rollbackData,  freeRollBack, NULL);

    MEM_GUARD_END()
    return context;
}

void destroyContext(Context* context) {
    CHECK_FREE(context->data);
    destroyRollbackData(context->rollbackData);
    context->rollbackData = NULL;

    if(context->asynContext != NULL) {
        destroyAsynContext(context->asynContext);
        context->asynContext = NULL;
    }
    ContextMem * mem = (ContextMem*) context;
    CHECK_FREE(mem->map.hash);
    CHECK_FREE(mem);
}