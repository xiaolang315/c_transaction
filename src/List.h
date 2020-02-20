//
// Created by zhangchao on 2020/2/15.
//

#ifndef C_TRANSACTION_LIST_H
#define C_TRANSACTION_LIST_H

#include "BaseType.h"
#include "ExternC.h"

EXTERN_STDC_BEGIN

typedef struct ListNode {
    struct ListNode* next;
} ListNode;

typedef struct BaseList {
    ListNode* first;
    ListNode* last;
} BaseList;

void init(BaseList* list);
void push_back(BaseList* list, ListNode* node);

typedef BOOL (*CompareFunc)(const ListNode*, const ListNode*);
BOOL defaultCompare(const ListNode*, const ListNode*);

ListNode* fetch(BaseList* list, const ListNode* matchNode, CompareFunc compare);

BOOL empty(const BaseList* list);

#define DEF_NODE_CONVERT_FUNC(type)\
static inline struct type* to##type(ListNode* node) {\
    return (type*)((char*)node);\
}\
static inline const type* to##type##C(const ListNode* node) {\
    return (const type*)((const char*)node);\
}

#define DEF_NODE_BEGIN(name)\
typedef struct name{\
    ListNode node;\


#define DEF_NODE_END(name)\
} name;
EXTERN_STDC_END

#endif //C_TRANSACTION_LIST_H
