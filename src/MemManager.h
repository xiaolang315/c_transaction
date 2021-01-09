//
// Created by zhangchao on 2020/2/13.
//

#ifndef C_TRANSACTION_GLOBALMEMMANAGER_H
#define C_TRANSACTION_GLOBALMEMMANAGER_H

#include "BaseType.h"
#include "mcl/stdc.h"

MCL_STDC_BEGIN

typedef void* (*MallocFunc)(size_t __size);
typedef void  (*FreeFunc)(void *);
typedef void  (*RegisterMemFunc)(void* buff, size_t size);
typedef void* (*CheckLeak)();

typedef struct MemAllocAlgo{
    MallocFunc malloc;
    FreeFunc free;
    RegisterMemFunc registerMem;
    CheckLeak checkLeak;
}MemAllocAlgo;

//you can use this func set your favorite alg
void registerMemAlgo(const MemAllocAlgo*);
//if you want use your local static global mem, tell me use this function
void useStaticMemory(void* buff, uint32_t size);
void useHeapMemory();

//when Mem leak happen the return will be the first leak mem;
void* checkMemLeaksPos();

void* mallocTc(uint32_t);
void freeTc(void*);

MCL_STDC_END

#endif //C_TRANSACTION_GLOBALMEMMANAGER_H
