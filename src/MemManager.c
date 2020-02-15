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
uint32_t leftSize = 0;

typedef struct MemNode {
    void* ptr;
    uint32_t size;
    struct MemNode* next;
} MemNode;

typedef struct MemNodeList {
    MemNode* first;
    MemNode* last;
} MemNodeList;

MemNode firstMem;
MemNodeList memPos;
MemNodeList reuse;

void init(MemNodeList* list) {
   list->first = NULL;
   list->last = NULL;
}

void push_back(MemNodeList* list, MemNode* node) {
    if(list->first == NULL) {
        list->first = node;
    } else {
        list->last->next = node;
    }
    list->last = node;
}

typedef BOOL (*CompareFunc)(const MemNode*, const MemNode*);

struct MemNode* fetch(MemNodeList* list, const MemNode* matchNode, CompareFunc compare) {
    if(list->first == NULL) return NULL;
    if(compare(list->first, matchNode) == TRUE) {
        MemNode* out = list->first;
        list->first = list->first->next;
        return out;
    }
    MemNode* node = list->first->next;
    MemNode* last = list->first;
    while(node != NULL) {
        if (compare(node, matchNode) == TRUE) {
            MemNode* out = node;
            last->next = node->next;
            if(node == list->last) {
                list->last = last;
            }
            return out;
        } else {
            last = node;
            node = node->next;
        }
    }
    return NULL;
}

void defaultMemoryControl() {
    mallocFunc = malloc;
    freeFunc = free;
    leftSize = 0;
    init(&reuse);
    init(&memPos);
}

BOOL memSizeMatcher (const MemNode* var, const MemNode* match) {
    return var->size >= match->size ? TRUE: FALSE;
}

struct MemNode* reUseFetch(uint32_t size) {
    MemNode matcher = {NULL, size, NULL};
    return fetch(&reuse, &matcher, memSizeMatcher);

}

static size_t withMemNode(size_t size) {
   return size + sizeof(MemNode) ;
}

static MemNode* doAllocNewMem(size_t needSize, size_t allocSize) {
    char* newPtr =  memPos.last->ptr + memPos.last->size;
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
    currentNode->next = NULL;
    push_back(&memPos, currentNode);
    return currentNode->ptr;
}

BOOL ptrMatcher (const MemNode* var, const MemNode* match) {
    return var->ptr == match->ptr? TRUE: FALSE;
}


void  staticFree(void * ptr) {
    MemNode matchNode = {ptr, 0, NULL};
    MemNode* toBeFree = fetch(&memPos, &matchNode, ptrMatcher);
    toBeFree->next = NULL;
    push_back(&reuse, toBeFree);
}

void* checkMemLeaksPos() {
    if(memPos.first != NULL) {
        return  memPos.first->ptr;
    }
    return NULL;
}


void memoryControl(void* buff, uint32_t size) {
    mallocFunc = staticMalloc;
    freeFunc = staticFree;
    leftSize = size;
    init(&reuse);
    init(&memPos);
    memPos.last = &firstMem;
    memPos.last->ptr = buff;
    memPos.last->size = 0;
}

void* mallocTc(uint32_t size) {
    return mallocFunc(size);
}

void freeTc(void* ptr) {
    return freeFunc(ptr);
}

