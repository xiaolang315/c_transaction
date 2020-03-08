//
// Created by zhangchao on 2020/2/28.
//
#include "AsynContext.h"
#include "MemHelp.h"
#include "BaseType.h"
#include "Foreach.h"
#include "MemGuard.h"

typedef struct AsynContextMem {
    struct AsynContext context;
    void* runtimeActionsBuff;
} AsynContextMem;


static void initRuntimeAction(RuntimeAction* self, Action action, RuntimeAction* next) {
    self->action = action;
    self->context = NULL;
    self->next = next;
}

AsynContext* initAsynContext(ActionDesc* actions, uint32_t actionNum){

    MEM_GUARD(2);
    STRUCT_ALLOC(AsynContextMem, mem);
    CHECK_PTR_R(mem, NULL);
    struct AsynContext* context = &mem->context;

    RuntimeAction* runtimeActions= NULL;
    ARRAY_ALLOC(RuntimeAction, runtimeActions, actionNum);
    CHECK_PTR_R(runtimeActions, NULL);

    RuntimeAction* current = &context->current;
    FOREACH_X(ActionDesc, action IN(actions) LIMITS(actionNum))
        initRuntimeAction(current, action->action, &runtimeActions[ITEM_INDEX(action)]);
        current = current->next;
    FOREACH_END()
    initRuntimeAction(current, NULL, NULL);

    mem->runtimeActionsBuff = runtimeActions;
    return &mem->context;
}

void destroyAsynContext(AsynContext* asynContext) {
    AsynContextMem* mem = (AsynContextMem*)asynContext;
    CHECK_FREE(mem->runtimeActionsBuff);
    freeTc(mem);
}
