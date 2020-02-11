//
// Created by zhangchao on 2020/2/11.
//

#ifndef C_TRANSACTION_MEMHELP_H
#define C_TRANSACTION_MEMHELP_H

#include <stdlib.h>

#define STRUCT_ALLOC(type, var)\
type* var = (type*)malloc(sizeof(type));


#define  MEM_GUARD(n);\
typedef struct MemGuard {\
    void* buff[n];\
    int num;\
}MemGuard; MemGuard mem_guard ;\
mem_guard.num = 0;

void freeAll(void* buff, uint32_t num);

#define CHECK_BOOL_R(cond,  ret)\
if(cond) {\
    freeAll(mem_guard.buff, mem_guard.num);\
    return ret;\
}

#define CHECK_PTR_R(ptr, ret)\
if(ptr == NULL) {\
    freeAll(mem_guard.buff, mem_guard.num);\
    return ret;\
} else {\
   mem_guard.buff[mem_guard.num++] = ptr;\
}

#define CHECK_FREE(ptr)\
if(ptr != NULL) free(ptr)


#endif //C_TRANSACTION_MEMHELP_H
