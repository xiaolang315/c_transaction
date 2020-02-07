//
// Created by zhangchao on 2020/2/7.
//
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Context.h"
#include "AsynTrans.h"

DEF_CTXT(Simple1) {
    int x;
};

TEST_GROUP(AsynTransTest) {
    void teardown()
    {
        mock().clear();
    }
};



ACTION_DEF(SimpleAction, STRUCTS(DEF_ACTION_CTXT(Simple1)))(Context* context) {
    CAST_TO(Simple1, s);
    if(s->x == 1) return ActionOk;
    s->x = 1;
    return ActionContinue;
}

NULL_CTXT_ACTION_DEF(SimpleActionCheck)(Context* context) {
    mock().actualCall("SimpleActionCheck");
    return ActionOk;
}

TEST(AsynTransTest, start_a_asyn_trans) {
    TRANS(trans,
          ACTIONS(SimpleAction
                  , SimpleActionCheck
          )
    )

    mock().expectNoCall("SimpleActionCheck");
    AsynContext context;
    TransResult ret = start(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);
    mock().checkExpectations();
    mock().clear();

    mock().expectOneCall("SimpleActionCheck");
    ret = asyn_exec(&trans, &context);
    CHECK_EQUAL(ret, TransSucc);
    mock().checkExpectations();
}

