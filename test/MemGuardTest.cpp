//
// Created by zhangchao on 2020/2/12.
//
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "MemHelp.h"

TEST_GROUP(MemGuardTest) {
    TEST_TEARDOWN(){
        mock().clear();
    }
};

bool fooFunc() {
    MEM_GUARD(1)

    void* buff = malloc(100);
    CHECK_PTR_R(buff, false);

    mock().actualCall("malloc1");
    void* buff1 = malloc(100);
    CHECK_PTR_R(buff1, false);

    mock().actualCall("malloc2");
    return true;
}

TEST(MemGuardTest , mem_alloc_ex_max_num_should_return_fail) {

    mock().expectNoCall("malloc2");
    mock().expectOneCall("malloc1");
    CHECK_FALSE(fooFunc());
}


void freeAllDemo(void** buff, uint32_t num){
    mock().actualCall("freeDemo").withParameter("num", num);
    freeAll(buff, num);
}

#undef FREE_ALL
#define FREE_ALL(buffs, num) freeAllDemo(buffs, num)

TEST(MemGuardTest , mem_guard_can_record_mem_after_check) {

    MEM_GUARD(2)

    void* buff = malloc(100);
    CHECK_PTR(buff);

    CHECK_EQUAL(mem_guard.num, 1);
    CHECK_EQUAL(mem_guard.buff[0], buff);

    void* buff1 = malloc(100);
    CHECK_PTR(buff1);
    CHECK_EQUAL(mem_guard.num, 2);
    CHECK_EQUAL(mem_guard.buff[1], buff1);


    mock().expectOneCall("freeDemo").withParameter("num", 2);
    CHECK_PTR(NULL);
}
