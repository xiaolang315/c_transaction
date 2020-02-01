#include "Transaction.h"
#include "Context.h"
#include "Foreach.h"

typedef struct ContextSet {
    int id;
    int offset;
}ContextSet;

ContextSet hash[10];
int hashNum = 0;

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

TransResult exec(const Transaction* trans){
    int length = 0;
    hashNum  = 0;
    FOREACH(ActionDesc, action, trans->actions, trans->num)
        FOREACH(ContextDesc, context, action->contexts, action->num)
            getLength(&length, context);
        FOREACH_END()
    FOREACH_END()
    char data[length];

    Context context;
    context.data = data;
    context.castTo = castTo;

    for( unsigned int i =0; i< trans->num; i++ ) {
        ActionResult ret = trans->actions[i].action(&context);
        if(ret != ActionOk){
            return Fail;
        }
    }
    return Succ;
}
