#include "AsynTransaction.h"
#include "MemManager.h"
#include "RollbackContext.h"
#include "Context.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupport_c.h"

DEF_ACTION_CTXT(Simple1) {
    int x;
};

struct AutoCheck {
    AutoCheck(){
        static char buff[10000] ;
        useStaticMemory(buff, ARRAY_SIZE(buff));
    }
    ~AutoCheck(){
        CHECK_EQUAL(NULL, checkMemLeaksPos());
        useHeapMemory();
    }
};

TEST_GROUP(AsynTransTest) {

    TEST_TEARDOWN() {
        mock().clear();
    }

    AutoCheck check;
};

ASYN_ACTION_DEF(InitAction, ACTION_CTXTs(ACTION_CTXT(Simple1)))(Context *context) {
    CAST_TO(Simple1, s);
    s->x = 0;
    return ActionOk;
}


ASYN_ACTION_DEF(SimpleAsynAction, ACTION_CTXTs(ACTION_CTXT(Simple1)))(Context *context) {
    CAST_TO(Simple1, s);
    if (s->x == 1) return ActionOk;
    s->x = 1;
    return ActionContinue;
}

NULL_CTXT_SYNC_ACTION_DEF(SimpleActionCheck)(Context *) {
    mock().actualCall("SimpleActionCheck");
    return ActionOk;
}

TEST(AsynTransTest, start_a_asyn_trans) {
    TRANS(trans,
          ACTIONS(InitAction, SimpleAsynAction, SimpleActionCheck
          )
    )

    mock().expectNoCall("SimpleActionCheck");
    Context *context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);
    mock().checkExpectations();
    mock().clear();

    mock().expectOneCall("SimpleActionCheck");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransSucc);
    mock().checkExpectations();
}

ASYN_SUB_TRANS(sub1, ACTIONS(InitAction, SimpleAsynAction, SimpleActionCheck))

TEST(AsynTransTest, start_a_asyn_trans_with_subtruns) {
    TRANS(trans,
          ACTIONS(InitAction, SimpleAsynAction, sub1
          )
    )

    Context *context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);

    ret = asynExec(context);
    CHECK_EQUAL(ret, TransContinue);

    mock().expectOneCall("SimpleActionCheck");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransSucc);

    mock().checkExpectations();
}

void RollBackActionDemo(RollbackData *) {
    mock().actualCall("RollBackDemo");
}

struct RollbackStruct {
    int x;
};

NULL_CTXT_SYNC_ACTION_DEF(SimpleActionRollbackCheck)(Context *context) {
    RollbackStruct s = {0};
    RollbackData data = {&s, sizeof(s)};
    addRollBackAction(context->rollbackData, RollBackActionDemo, &data);
    return ActionOk;
}

NULL_CTXT_SYNC_ACTION_DEF(FailAction)(Context *) {
    return ActionErr;
}

TEST(AsynTransTest, asyn_trans_can_rollback) {
    AutoCheck autoCheck;
    TRANS(trans,
          ACTIONS(InitAction, SimpleAsynAction, SimpleActionRollbackCheck, FailAction
          )
    )

    Context *context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);

    mock().expectOneCall("RollBackDemo");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransFail);

    mock().checkExpectations();
}

ASYN_SUB_TRANS(SubSuccTrans, ACTIONS(InitAction, SimpleAsynAction, SimpleActionRollbackCheck))

TEST(AsynTransTest, asyn_trans_can_rollback_action_in_sub_trans) {
    TRANS(trans,
          ACTIONS(InitAction, SimpleAsynAction, SubSuccTrans, FailAction
          )
    )

    Context *context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);

    ret = asynExec(context);
    CHECK_EQUAL(ret, TransContinue);

    mock().expectOneCall("RollBackDemo");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransFail);

    mock().checkExpectations();
}
