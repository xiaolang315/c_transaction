//
// Created by zhangchao on 2020/2/13.
//
#include <stdlib.h>
#include "MemHelp.h"
#include "BaseType.h"
#include "List.h"

typedef void* (*MallocFunc)(size_t __size);
typedef void  (*FreeFunc)(void *);

MallocFunc mallocFunc = malloc;
FreeFunc freeFunc = free;
uint32_t staticMemSize = 0;
uint32_t leftSize = 0;

DEF_NODE_BEGIN(MemNode)
    void* ptr;
    uint32_t size;
DEF_NODE_END(MemNode)

MemNode firstMem;
BaseList busyMem;
BaseList reuseMem;

DEF_NODE_CONVERT(MemNode)

void useHeapMemory() {
    mallocFunc = malloc;
    freeFunc = free;
    leftSize = 0;
    init(&reuseMem);
    init(&busyMem);
}

static BOOL memSizeMatcher (const ListNode * var, const ListNode* match) {
    const MemNode* varMemNode = toMemNodeC(var);
    const MemNode* matchMemNode = toMemNodeC(match);
    return varMemNode->size >= matchMemNode->size ? TRUE: FALSE;
}

struct MemNode* reUseFetch(uint32_t size) {
    MemNode matcher = {NULL};
    matcher.size = size;
    return toMemNode(fetch(&reuseMem, (ListNode*)&matcher, memSizeMatcher));
}

static size_t withMemNode(size_t size) {
   return size + sizeof(MemNode) ;
}

static MemNode* doAllocNewMem(size_t needSize, size_t allocSize) {
    MemNode* lastPos = toMemNode(busyMem.last);
    char* newPtr =  lastPos->ptr + lastPos->size;
    MemNode* currentNode = (MemNode*)(newPtr + allocSize );
    currentNode->ptr = newPtr;
    currentNode->size = needSize;
    leftSize -= needSize;
    return currentNode;
}

void* staticMalloc(size_t __size) {
    size_t needSize = withMemNode(__size);
    MemNode* currentNode = reUseFetch(needSize);
    if(currentNode == NULL) {
        if(leftSize < needSize ) return NULL;
        currentNode = doAllocNewMem(needSize, __size);
    }
    push_back(&busyMem, (ListNode*)currentNode);
    return currentNode->ptr;
}

static BOOL ptrMatcher (const ListNode * var, const ListNode * match) {
    const MemNode* varMemNode = toMemNodeC(var);
    const MemNode* matchMemNode = toMemNodeC(match);
    return varMemNode->ptr == matchMemNode->ptr? TRUE: FALSE;
}


void  staticFree(void * ptr) {
    MemNode matchNode = { NULL};
    matchNode.ptr = ptr;
    ListNode* toBeFree = fetch(&busyMem, (ListNode*)&matchNode, ptrMatcher);
    toBeFree->next = NULL;
    push_back(&reuseMem, toBeFree);
}

void* checkMemLeaksPos() {
    if(empty(&busyMem) != TRUE) {
        return toMemNode(busyMem.first)->ptr;
    }
    return NULL;
}

void useStaticMemory(void* buff, uint32_t size) {
    mallocFunc = staticMalloc;
    freeFunc = staticFree;
    leftSize = size;
    staticMemSize = size;
    init(&reuseMem);
    init(&busyMem);
    busyMem.last = (ListNode*)&firstMem;
    MemNode* lastPos = toMemNode(busyMem.last);
    lastPos->ptr = buff;
    lastPos->size = 0;
}

void* mallocTc(uint32_t size) {
    return mallocFunc(size);
}

void freeTc(void* ptr) {
    return freeFunc(ptr);
}

