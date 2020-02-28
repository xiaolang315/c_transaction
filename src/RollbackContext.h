//
// Created by zhangchao on 2020/2/28.
//

#ifndef C_TRANSACTION_ROLLBACKCONTEXT_H
#define C_TRANSACTION_ROLLBACKCONTEXT_H

#include "BaseType.h"
#include "ExternC.h"

EXTERN_STDC_BEGIN

typedef struct RollbackData {
    void* mem;
    uint32_t len;
} RollbackData;

typedef void (*RollBackAction)(RollbackData*);

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

BOOL addRollBack(RollbackContext* , RollBackAction, const RollbackData*);
void rollback(RollbackContext*);

RollbackContext* initRollBackCtxt(uint32_t actionNum);
void destroyRollbackData(RollbackContext*) ;

#define ROLL_BACK_DATA(type, name, ...)\
type name##_s = {__VA_ARGS__};\
    RollbackData name = {&name##_s, sizeof(name##_s)};\

void contextMemFree(RollbackData* mem);

EXTERN_STDC_END

#endif //C_TRANSACTION_ROLLBACKCONTEXT_H
