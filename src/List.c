//
// Created by zhangchao on 2020/2/15.
//
#include <stddef.h>
#include "List.h"

void init(BaseList* list) {
    list->first = NULL;
    list->last = NULL;
}

void push_back(BaseList* list, ListNode* node) {
    node->next = NULL;
    if(list->first == NULL) {
        list->first = node;
    } else {
        list->last->next = node;
    }
    list->last = node;
}

static ListNode* popFirst(BaseList* list) {
    ListNode* out = list->first;
    list->first = list->first->next;
    return out;
}

static ListNode* popNodeNotFirst(BaseList* list, ListNode* node, ListNode* last) {
    ListNode* out = node;
    last->next = node->next;
    if(node == list->last) {
        list->last = last;
    }
    return out;
}

ListNode* fetch(BaseList* list, const ListNode* matchNode, CompareFunc compare) {
    if(list->first == NULL) return NULL;
    if(compare(list->first, matchNode) == TRUE) {
        return popFirst(list);
    }
    ListNode* node = list->first->next;
    ListNode* last = list->first;
    while(node != NULL) {
        if (compare(node, matchNode) == TRUE) {
            return popNodeNotFirst(list, node, last);
        } else {
            last = node;
            node = node->next;
        }
    }
    return NULL;
}

BOOL defaultCompare(const ListNode* left, const ListNode* right) {
    return left == right ? TRUE : FALSE;
}

BOOL isEmpty(const BaseList* list) {
    return list->first == NULL? TRUE:FALSE;
}
