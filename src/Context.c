//
// Created by zhangchao on 2020/2/3.
//
#include "Context.h"
#include "memory.h"
#include "stdlib.h"

void AddRollBack(RollbackContext* context, RollBackAction action, const RollbackData* data){
    context->contexts[context->num].action = action;

    RollbackData* toData = &context->contexts[context->num].data;

    toData->mem = malloc(data->len);
    if(toData->mem == NULL) return;

    memcpy(toData->mem, data->mem, data->len);
    toData->len = data->len;
    context->num++;
}
