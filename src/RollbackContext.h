//
// Created by zhangchao on 2020/2/28.
//

#ifndef C_TRANSACTION_ROLLBACKCONTEXT_H
#define C_TRANSACTION_ROLLBACKCONTEXT_H

#include "BaseType.h"
#include "mcl/stdc.h"
#include "mcl/typedef.h"

MCL_STDC_BEGIN

typedef struct RollbackData {
    void* mem;
    uint32_t len;
} RollbackData;

typedef void (*RollBackAction)(RollbackData*);

MCL_TYPE_DECL(RollbackContext);

BOOL addRollBackAction(RollbackContext* , RollBackAction, const RollbackData*);
void rollback(RollbackContext*);
void appendRollBackContext(RollbackContext* current, RollbackContext* next);

RollbackContext* initRollBackCtxt(uint32_t actionNum);
void destroyRollbackData(RollbackContext*) ;

#define ROLL_BACK_DATA(type, name, ...)\
type name##_s = {__VA_ARGS__};\
    RollbackData name = {&name##_s, sizeof(name##_s)};\


MCL_STDC_END

#endif //C_TRANSACTION_ROLLBACKCONTEXT_H
