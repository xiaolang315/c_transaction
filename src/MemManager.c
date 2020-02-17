//
// Created by zhangchao on 2020/2/13.
//
#include <stdlib.h>
#include "MemHelp.h"
#include "BaseType.h"

typedef void* (*MallocFunc)(size_t __size);
typedef void  (*FreeFunc)(void *);

MallocFunc mallocFunc = malloc;
FreeFunc freeFunc = free;
uint32_t staticMemSize = 0;
uint32_t leftSize = 0;

#include "List.h"

typedef struct MemNode {
    ListNode node;
    void* ptr;
    uint32_t size;
} MemNode;

MemNode firstMem;
BaseList memPos;
BaseList reuse;

struct MemNode* toMemNode(ListNode* node) {
    return (MemNode*)((char*)node);
}

const MemNode* toMemNode_c(const ListNode* node) {
    return (const MemNode*)((const char*)node);
}


void defaultMemoryControl() {
    mallocFunc = malloc;
    freeFunc = free;
    leftSize = 0;
    init(&reuse);
    init(&memPos);
}

BOOL memSizeMatcher (const ListNode * var, const ListNode* match) {
    const MemNode* varMemNode = toMemNode_c(var);
    const MemNode* matchMemNode = toMemNode_c(match);
    return varMemNode->size >= matchMemNode->size ? TRUE: FALSE;
}

struct MemNode* reUseFetch(uint32_t size) {
    MemNode matcher = {NULL};
    matcher.size = size;
    return toMemNode(fetch(&reuse, (ListNode*)&matcher, memSizeMatcher));
}

static size_t withMemNode(size_t size) {
   return size + sizeof(MemNode) ;
}

static MemNode* doAllocNewMem(size_t needSize, size_t allocSize) {
    MemNode* lastPos = toMemNode(memPos.last);
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
    currentNode->node.next = NULL;
    push_back(&memPos, (ListNode*)currentNode);
    return currentNode->ptr;
}

BOOL ptrMatcher (const ListNode * var, const ListNode * match) {
    const MemNode* varMemNode = toMemNode_c(var);
    const MemNode* matchMemNode = toMemNode_c(match);
    return varMemNode->ptr == matchMemNode->ptr? TRUE: FALSE;
}


void  staticFree(void * ptr) {
    MemNode matchNode = { NULL};
    matchNode.ptr = ptr;
    ListNode* toBeFree = fetch(&memPos, (ListNode*)&matchNode, ptrMatcher);
    toBeFree->next = NULL;
    push_back(&reuse, toBeFree);
}

void* checkMemLeaksPos() {
    if(memPos.first != NULL) {
        return  toMemNode(memPos.first)->ptr;
    }
    return NULL;
}


void memoryControl(void* buff, uint32_t size) {
    mallocFunc = staticMalloc;
    freeFunc = staticFree;
    leftSize = size;
    staticMemSize = size;
    init(&reuse);
    init(&memPos);
    memPos.last = (ListNode*)&firstMem;
    MemNode* lastPos = toMemNode(memPos.last);
    lastPos->ptr = buff;
    lastPos->size = 0;
}

void* mallocTc(uint32_t size) {
    return mallocFunc(size);
}

void freeTc(void* ptr) {
    return freeFunc(ptr);
}

