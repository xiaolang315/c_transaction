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

ListNode* fetch(BaseList* list, const ListNode* matchNode, CompareFunc compare) {
    if(list->first == NULL) return NULL;
    if(compare(list->first, matchNode) == TRUE) {
        ListNode* out = list->first;
        list->first = list->first->next;
        return out;
    }
    ListNode* node = list->first->next;
    ListNode* last = list->first;
    while(node != NULL) {
        if (compare(node, matchNode) == TRUE) {
            ListNode* out = node;
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

BOOL empty(const BaseList* list) {
    return list->first == NULL? TRUE:FALSE;
}
