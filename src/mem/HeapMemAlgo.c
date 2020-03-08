//
// Created by zhangchao on 2020/3/8.
//
#include "MemManager.h"
#include <stdlib.h>

void  NullRegisterMemFunc(void* buff, size_t size) {

}

void* checkLeak(){
    return NULL;
}

void* HeapMallocFunc(size_t __size){

    return malloc(__size);
}

void  HeapFreeFunc(void * ptr) {
    free(ptr);
}

MemAllocAlgo heapAlgo = {
        HeapMallocFunc, HeapFreeFunc, NullRegisterMemFunc,checkLeak
};

const MemAllocAlgo* memAllocAlgo(){
    return &heapAlgo;
}
