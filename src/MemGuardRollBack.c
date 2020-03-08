//
// Created by zhangchao on 2020/2/29.
//
#include "MemGuardRollBack.h"
#include "MemManager.h"

void contextMemFree(RollbackData* mem){
    MemPtr* ptr = (MemPtr*)mem->mem;
    freeTc(ptr->ptr);
}