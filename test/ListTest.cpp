//
// Created by zhangchao on 2020/2/19.
//
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "List.h"

TEST_GROUP(ListTest) {
    TEST_SETUP() {
    }
    TEST_TEARDOWN(){
        mock().clear();
    }
};

TEST(ListTest, can_push_back_to_list_and_fetch_succ) {
    BaseList  list;
    init(&list);
    CHECK_EQUAL(empty(&list), TRUE);
    ListNode node1;
    push_back(&list, &node1);

    CHECK_EQUAL(empty(&list), FALSE);
    ListNode* out = fetch(&list, &node1, defaultCompare);

    CHECK_EQUAL(out, &node1);
    CHECK_EQUAL(empty(&list), TRUE);
}
