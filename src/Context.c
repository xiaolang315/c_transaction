//
// Created by zhangchao on 2020/2/3.
//
#include "Context.h"
#include "memory.h"
#include "stdlib.h"
#include "BaseType.h"
#include "Foreach.h"
#include "Action.h"

static int getOffSet(const ContextMap* map,  int id){
    for( unsigned int i =0; i< map->hashNum; i++ ) {
        if(id == map->hash[i].id) return map->hash[i].offset;
    }
    return -1;
}

static void* castTo(const ContextMap* map, void* data, int id){
    return data + getOffSet(map, id);
}

static void insertHash(ContextMap* map, int offset, int id){
    map->hash[map->hashNum].id = id;
    map->hash[map->hashNum].offset = offset;
    map->hashNum ++;
}

static void getLength(ContextMap* map, int* length, CtxtActionUse* context){
    int offSet = getOffSet(map, context->id);
    if(offSet == -1) {
        insertHash(map, *length, context->id);
        *length += context->size;
    }
}

BOOL AddRollBack(RollbackContext* context, RollBackAction action, const RollbackData* data){
    context->contexts[context->num].action = action;

    RollbackData* toData = &context->contexts[context->num].data;

    toData->mem = malloc(data->len);
    if(toData->mem == NULL) return FALSE;

    memcpy(toData->mem, data->mem, data->len);
    toData->len = data->len;
    context->num++;
    return TRUE;
}

static int initLength(ContextMap* map, ActionDesc* actions, int actionNum) {
    int length = 0;
    map->hashNum = 0;

    map->hash = malloc(actionNum * sizeof(ContextSet));
    if(map->hash == NULL) return 0;

    FOREACH(ActionDesc, action, actions, actionNum)
        FOREACH(CtxtActionUse, context, action->contexts, action->ctxtNum)
            getLength(map, &length, context);
        FOREACH_END()
    FOREACH_END()
    return length;
}

BOOL initRollBackCtxt(RollbackContext* context, int actionNum){
    context->contexts = (OneRollBackContext*)malloc(actionNum * sizeof(OneRollBackContext));
    if(context->contexts == NULL) {
        return FALSE;
    }
    context->next = NULL;
    context->num = 0;
    return TRUE;
}

BOOL initContext(Context* context, ActionDesc* actions, int actionNum) {
    int length = initLength(&context->map, actions, actionNum);

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
    free(context->map.hash);
}