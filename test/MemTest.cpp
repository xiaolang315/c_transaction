//
// Created by zhangchao on 2020/2/12.
//
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "MemHelp.h"

TEST_GROUP(MemGuardTest) {
    TEST_SETUP() {
        static char static_buff[1000];
        useStaticMemory(static_buff, 1000);
    }
    TEST_TEARDOWN(){
        mock().clear();
        useHeapMemory();
    }
};

bool fooFunc() {
    MEM_GUARD(1);

    void* buff = mallocTc(100);
    CHECK_PTR_R(buff, false);

    mock().actualCall("malloc1");
    void* buff1 = mallocTc(100);
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

    MEM_GUARD(2);

    void* buff = mallocTc(100);
    CHECK_PTR(buff);

    CHECK_EQUAL(mem_guard.num, 1);
    CHECK_EQUAL(mem_guard.buff[0], buff);

    void* buff1 = mallocTc(100);
    CHECK_PTR(buff1);
    CHECK_EQUAL(mem_guard.num, 2);
    CHECK_EQUAL(mem_guard.buff[1], buff1);


    mock().expectOneCall("freeDemo").withParameter("num", 2);
    CHECK_PTR(NULL);//check fail trigger free all

    CHECK_EQUAL(NULL, checkMemLeaksPos());
}

TEST_GROUP(MemControl) {

    TEST_TEARDOWN(){
        useHeapMemory();
    }
};

TEST(MemControl, should_alloc_succ_with_right_size_from_static_mem) {

    void* buff[1000];
    useStaticMemory(buff, 1000);
    void* buff1 = mallocTc(100);
    CHECK_TRUE(buff1 != NULL);

    void* buff2 = mallocTc(1000);
    CHECK_EQUAL(buff2, NULL);

    void* buff3 = mallocTc(100);
    CHECK_TRUE(buff3 != NULL);

    freeTc(buff1);
    CHECK_EQUAL(checkMemLeaksPos(), buff3);

    freeTc(buff3);
    CHECK_EQUAL(checkMemLeaksPos(), NULL);
}

TEST(MemControl, should_alloc_succ_when_free_enough_size_after_mem_empty) {
    void* buff[1000];
    useStaticMemory(buff, 1000);
    void* buff1 = mallocTc(400);
    CHECK_TRUE(buff1 != NULL);

    void* buff2 = mallocTc(400);
    CHECK_TRUE(buff2 != NULL);

    void* buff3 = mallocTc(400);
    CHECK_EQUAL(buff3, NULL);

    freeTc(buff1);

    buff3 =mallocTc(400);
    CHECK_TRUE(buff3 != NULL);

    freeTc(buff2);
    freeTc(buff3);
    CHECK_EQUAL(checkMemLeaksPos(), NULL);
}
