//
// Created by zhangchao on 2020/2/29.
//

#ifndef C_TRANSACTION_MEMGUARDROLLBACK_H
#define C_TRANSACTION_MEMGUARDROLLBACK_H

#include "RollbackContext.h"
#include "Assert.h"

#define  MEM_GUARD_VAR __mguard

#define MEM_GUARD_E(num) \
    RollbackContext* __mguard = initRollBackCtxt(num);\
    if(MEM_GUARD_VAR == NULL) return;

#define MEM_GUARD_E_R(num, ret) \
    RollbackContext* __mguard = initRollBackCtxt(num);\
    if(MEM_GUARD_VAR == NULL) return ret;

#define MEM_GUARD_END() destroyRollbackData(MEM_GUARD_VAR);\

typedef struct MemPtr {
    void* ptr;
} MemPtr;

#define CHECK_PTR_E_FUNC_DO(ptr, func, statement)\
if(ptr!= NULL) {\
    ROLL_BACK_DATA(MemPtr, memPtr, ptr);\
    addRollBackAction(MEM_GUARD_VAR, func, &memPtr);\
} else {\
    rollback(MEM_GUARD_VAR);\
    destroyRollbackData(MEM_GUARD_VAR);\
    statement;\
}


#define SET_NULL(ret) {ret = NULL;};

#define CHECK_PTR_E_FUNC_R(ptr, func, ret)\
CHECK_PTR_E_FUNC_DO(ptr, func, RETURN(ret))

#define CHECK_PTR_E_FUNC(ptr, func)\
CHECK_PTR_E_FUNC_DO(ptr, func, SET_NULL(__mguard))

void contextMemFree(RollbackData* mem);

#define CHECK_PTR_E(ptr) CHECK_PTR_E_FUNC(ptr, contextMemFree)
#define CHECK_PTR_E_R(ptr, ret) CHECK_PTR_E_FUNC_R(ptr, contextMemFree, ret)

#endif //C_TRANSACTION_MEMGUARDROLLBACK_H
