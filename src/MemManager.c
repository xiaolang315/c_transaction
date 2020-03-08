//
// Created by zhangchao on 2020/2/13.
//
#include "MemHelp.h"
#include "BaseType.h"
#include "mem/StaticMemAlgo.h"
#include "mem/HeapMemAlgo.h"

const MemAllocAlgo* currentAlgo;

void registerMemAlgo(const MemAllocAlgo* algo) {
    currentAlgo = algo;
}

void useHeapMemory() {
    registerMemAlgo(memAllocAlgo());
}

void* checkMemLeaksPos() {
    return currentAlgo->checkLeak();
}

void useStaticMemory(void* buff, uint32_t size) {
    registerMemAlgo(staticMemAlgo());
    currentAlgo->registerMem(buff, size);
}

void* mallocTc(uint32_t size) {
    return currentAlgo->malloc(size);
}

void freeTc(void* ptr) {
    return currentAlgo->free(ptr);
}

