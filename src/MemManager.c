//
// Created by zhangchao on 2020/2/13.
//
#include <stdlib.h>
#include "MemHelp.h"

typedef void* (*MallocFunc)(size_t __size);
typedef void  (*FreeFunc)(void *);

MallocFunc mallocFunc = malloc;
FreeFunc freeFunc = free;
uint32_t staticSize = 0;
uint32_t leftSize = 0;

typedef struct MemNode {
    void* ptr;
    uint32_t size;
    struct MemNode* next;
} MemNode;

MemNode* lastPos;
MemNode firstPos;

MemNode* reuseFirstMem;
MemNode* reuseLastMem;

void defaultMemoryControl() {
    mallocFunc = malloc;
    freeFunc = free;
    leftSize = 0;
    lastPos = NULL;
    firstPos.next = NULL;
    firstPos.ptr = NULL;
}

struct MemNode* reUseFetch(uint32_t size) {
    if(reuseFirstMem == NULL) return NULL;
    if(reuseFirstMem->size >= size) {
        MemNode* out = reuseFirstMem;
        reuseFirstMem = reuseFirstMem->next;
        return out;
    }
    MemNode* node = reuseFirstMem->next;
    MemNode* last = reuseFirstMem;
    while(node != NULL) {
        if (node->size >= size) {
            MemNode* out = node;
            last->next = node->next;
            return out;
        } else {
            last = node;
            node = node->next;
        }
    }
    return NULL;
}

void reUsePush(struct MemNode* memNode) {
    memNode->next = NULL;
    if(reuseFirstMem == NULL) {
        reuseFirstMem = memNode;
        reuseLastMem = reuseFirstMem;
        return;
    }
    reuseLastMem->next = memNode;
}

void* staticMalloc(size_t __size) {
    MemNode* currentNode = reUseFetch(__size);
    if(currentNode == NULL) {
        if(leftSize < __size) return NULL;
        currentNode = malloc(sizeof(MemNode));
        if(currentNode == NULL) return NULL;
        currentNode->ptr = lastPos->ptr + __size;
        currentNode->size = __size;
        leftSize -= __size;
    }
    currentNode->next = NULL;
    lastPos->next = currentNode;
    lastPos = currentNode;
    return currentNode->ptr;
}

void  staticFree(void * ptr) {
    MemNode* node = firstPos.next;
    MemNode* last = &firstPos;
    do {
        if(ptr == node->ptr) {
            last->next = node->next;
            reUsePush(node);
            break;
        }
        last = node;
        node = node->next;
    }while (node);
}

void* checkMemLeaksPos() {
    if(firstPos.next != NULL)
        return firstPos.next->ptr;
    return NULL;
}

void memoryControl(void* buff, uint32_t size) {
    mallocFunc = staticMalloc;
    freeFunc = staticFree;
    staticSize = size;
    firstPos.ptr = buff;
    firstPos.next = NULL;
    leftSize = size;
    lastPos = &firstPos;
    reuseFirstMem = NULL;
    reuseLastMem = NULL;
}

void* mallocTc(uint32_t size) {
    return mallocFunc(size);
}

void freeTc(void* ptr) {
    return freeFunc(ptr);
}

