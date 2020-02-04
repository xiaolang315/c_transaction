#include "Transaction.h"
#include "Context.h"
#include "Foreach.h"
#include <stdlib.h>

typedef struct ContextSet {
    int id;
    int offset;
}ContextSet;

ContextSet hash[10];
int hashNum = 0;

int initLength(const Transaction *trans);

static int getOffSet(int id){
    for( unsigned int i =0; i< hashNum; i++ ) {
        if(id == hash[i].id) return hash[i].offset;
    }
    return -1;
}

static void* castTo(void* data, int id){
    return data + getOffSet(id);
}

static void insertHash(int offset, int id){
    hash[hashNum].id = id;
    hash[hashNum].offset = offset;
    hashNum ++;
}

static void getLength(int* length, ContextDesc* context){
    int offSet = getOffSet(context->id);
    if(offSet == -1) {
        insertHash(*length, context->id);
        *length += context->size;
    }
}

void rollback(Context* context) {
    FOREACH(OneRollBackContext, ctxt, context->rollbackData.contexts, context->rollbackData.num)
        ctxt->action(&ctxt->data);
        free(ctxt->data.mem);
    FOREACH_END()
}

TransResult exec(const Transaction* trans){
    int length = initLength(trans);
    char data[length];

    Context context;
    context.data = data;
    context.castTo = castTo;
    OneRollBackContext rollBackActions[trans->actionNum];
    context.rollbackData.contexts = rollBackActions;
    context.rollbackData.num = 0;

    FOREACH(ActionDesc, action, trans->actions, trans->actionNum)
        ActionResult ret = action->action(&context);
        if(ret != ActionOk){
            rollback(&context);
            return Fail;
        }
    FOREACH_END()
    return Succ;
}

int initLength(const Transaction *trans) {
    int length;
    hashNum  = 0;
    FOREACH(ActionDesc, action, trans->actions, trans->actionNum)
        FOREACH(ContextDesc, context, action->contexts, action->ctxtNum)
            getLength(&length, context);
        FOREACH_END()
    FOREACH_END()
    return length;
}
