//
// Created by zhangchao on 2020/2/11.
//

#ifndef C_TRANSACTION_MEMHELP_H
#define C_TRANSACTION_MEMHELP_H

#include "ExternC.h"
#include "MemManager.h"

EXTERN_STDC_BEGIN

#define STRUCT_ALLOC(type, var)\
type* var = (type*)mallocTc(sizeof(type));

#define ARRAY_ALLOC(type, var, num)\
var = (type*)mallocTc(sizeof(type) * num);

#define  MEM_GUARD(n);\
typedef struct MemGuard {\
    void* buff[n];\
    uint32_t num;\
}MemGuard; MemGuard mem_guard ; const uint32_t max = n;\
mem_guard.num = 0;

void freeAll(void** buff, uint32_t num);

//define this macro for test replace
#define FREE_ALL(buffs, num) freeAll(buffs, num)

#define CHECK_PTR(ptr)\
if(ptr == NULL || mem_guard.num >= max) {\
   FREE_ALL(mem_guard.buff, mem_guard.num);\
} else {\
   mem_guard.buff[mem_guard.num++] = ptr;\
}

#define CHECK_PTR_R(ptr, ret)\
if(ptr == NULL || mem_guard.num >= max) {\
    FREE_ALL(mem_guard.buff, mem_guard.num);\
    return ret;\
} else {\
   mem_guard.buff[mem_guard.num++] = ptr;\
}

#define CHECK_FREE(ptr)\
if(ptr != NULL) freeTc(ptr)


EXTERN_STDC_END

#endif //C_TRANSACTION_MEMHELP_H
