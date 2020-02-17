//
// Created by zhangchao on 2020/2/15.
//

#ifndef C_TRANSACTION_LIST_H
#define C_TRANSACTION_LIST_H

#include "BaseType.h"

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
ListNode* fetch(BaseList* list, const ListNode* matchNode, CompareFunc compare);

BOOL empty(BaseList* list);


#endif //C_TRANSACTION_LIST_H
