#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Context.h"
#include "AsynTransaction.h"

DEF_CTXT(Simple1) {
    int x;
};

TEST_GROUP(AsynTransTest) {
    void teardown()
    {
        mock().clear();
    }
};

ASYN_ACTION_DEF(SimpleAsynAction, STRUCTS(DEF_ACTION_CTXT(Simple1)))(Context* context) {
    CAST_TO(Simple1, s);
    if(s->x == 1) return ActionOk;
    s->x = 1;
    return ActionContinue;
}

NULL_CTXT_SYNC_ACTION_DEF(SimpleActionCheck)(Context* ) {
    mock().actualCall("SimpleActionCheck");
    return ActionOk;
}

TEST(AsynTransTest, start_a_asyn_trans) {
    TRANS(trans,
          ACTIONS(SimpleAsynAction
                  , SimpleActionCheck
          )
    )

    mock().expectNoCall("SimpleActionCheck");
    Context* context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);
    mock().checkExpectations();
    mock().clear();

    mock().expectOneCall("SimpleActionCheck");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransSucc);
    mock().checkExpectations();
}

ASYN_SUB_TRANS(sub, ACTIONS(SimpleAsynAction, SimpleActionCheck))

TEST(AsynTransTest, start_a_asyn_trans_with_subtruns) {

    TRANS(trans,
          ACTIONS(SimpleAsynAction
                  , sub
          )
    )

    Context* context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);

    ret = asynExec(context);
    CHECK_EQUAL(ret, TransContinue);

    mock().expectOneCall("SimpleActionCheck");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransSucc);

    mock().checkExpectations();
}

void RollBackActionDemo(RollbackData*){
    mock().actualCall("RollBackDemo");
}

struct RollbackStruct {
    int x;
};

NULL_CTXT_SYNC_ACTION_DEF(SimpleActionRollbackCheck)(Context* context) {
    RollbackStruct s ;
    RollbackData data = {&s, sizeof(s)};
    AddRollBack(&context->rollbackData, RollBackActionDemo, &data);
    return ActionOk;
}

NULL_CTXT_SYNC_ACTION_DEF(FailAction)(Context* ) {
    return ActionErr;
}

TEST(AsynTransTest, asyn_trans_can_rollback) {
    TRANS(trans,
          ACTIONS(SimpleAsynAction
                  , SimpleActionRollbackCheck
                  , FailAction
          )
    )

    Context* context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);

    mock().expectOneCall("RollBackDemo");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransFail);

    mock().checkExpectations();
}

ASYN_SUB_TRANS(SubSuccTrans, ACTIONS(SimpleAsynAction, SimpleActionRollbackCheck))

TEST(AsynTransTest, asyn_trans_can_rollback_action_in_sub_trans) {
    TRANS(trans,
          ACTIONS(SimpleAsynAction
                  , SubSuccTrans
                  , FailAction
          )
    )

    Context* context;
    TransResult ret = asynStart(&trans, &context);
    CHECK_EQUAL(ret, TransContinue);

    ret = asynExec(context);
    CHECK_EQUAL(ret, TransContinue);

    mock().expectOneCall("RollBackDemo");
    ret = asynExec(context);
    CHECK_EQUAL(ret, TransFail);

    mock().checkExpectations();
}
