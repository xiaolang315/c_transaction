
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Transaction.h"
#include "Context.h"

namespace {
    DEF_CTXT(SimpleStruct){
        int x;
    };

    DEF_CTXT(SimpleStruct1){
        int t[10];
    };

    ACTION_DEF(SimpleAction2, STRUCTS(
            DEF_ACTION_CTXT(SimpleStruct1)
            ,DEF_ACTION_CTXT(SimpleStruct)
            ))(Context* context) {
        CAST_TO(SimpleStruct1, s);
        s->t[0] = 1;

        CAST_TO(SimpleStruct, s2);
        s2->x++;
        return ActionOk;
    }

    ACTION_DEF(SimpleAction, STRUCTS(DEF_ACTION_CTXT(SimpleStruct)))(Context* context) {
        CAST_TO(SimpleStruct, s);
        s->x = 1;
        return ActionOk;
    }

    ACTION_DEF(SimpleActionCheck, STRUCTS(DEF_ACTION_CTXT(SimpleStruct)))(Context* context) {
        CAST_TO(SimpleStruct, s);
        mock().actualCall("SimpleActionCheck").withParameter("x", s->x);
        return ActionOk;
    }

    DEF_CTXT(SimpleFailStruct){
        char y;
    };

    ACTION_DEF(SimpleFailAction, STRUCTS(DEF_ACTION_CTXT(SimpleFailStruct)))(Context* context) {
        CAST_TO(SimpleFailStruct, s);
        s->y = 2;
        return ActionErr;
    }

    TEST_GROUP(TransactionTest){
        void teardown()
        {
            mock().clear();
        }
    };


    TEST(TransactionTest, exec_one_action) {
        mock().expectOneCall("SimpleActionCheck").ignoreOtherParameters();
        TRANS(trans,
              ACTIONS( SimpleAction
                      ,SimpleAction2
                      ,SimpleActionCheck
              )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransSucc);
        mock().checkExpectations();
    }

    TEST(TransactionTest, exec_one_failed_action_stop_trans) {
        mock().expectNoCall("SimpleActionCheck");
        TRANS(trans,
                ACTIONS( SimpleAction
                        ,SimpleFailAction
                        ,SimpleActionCheck
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }

    struct RollbackStruct {
        int x;
    };

    void RollBackActionDemo(RollbackData* data){
        if(data->len != sizeof(RollbackStruct)) return ;
        RollbackStruct* s = (RollbackStruct*)data->mem;
        mock().actualCall("RollBackDemo").withParameter("x", s->x);
    }

    const int Expect_X = 0xfe;

    NULL_CTXT_ACTION_DEF(SimpleActionRollbackCheck)(Context* context) {
        RollbackStruct s = {Expect_X};
        RollbackData data = {&s, sizeof(s)};
        AddRollBack(&context->rollbackData, RollBackActionDemo, &data);
        return ActionOk;
    }

    TEST(TransactionTest, exec_one_failed_action_roll_back) {
        mock().expectOneCall("RollBackDemo").withParameter("x", Expect_X);
        TRANS(trans,
                ACTIONS( SimpleAction
                        , SimpleActionRollbackCheck
                        , SimpleFailAction
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }


    SUB_TRANS(SubSimpleAction,
              ACTIONS(SimpleAction
                            , SimpleActionRollbackCheck
                            , SimpleFailAction
              )
    )


    TEST(TransactionTest, sub_proc_failed_action_roll_back_do_not_effect_main_proc) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
                ACTIONS( SimpleAction
                               , SubSimpleAction
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransSucc);
        mock().checkExpectations();
    }

    SUB_TRANS(SubSimpleActionSucc,
              ACTIONS(SimpleAction
                      , SimpleActionRollbackCheck
              )
    )

    TEST(TransactionTest, sub_trans_succ_but_main_trans_fail_also_need_roll_back) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
                ACTIONS( SimpleAction
                ,SubSimpleActionSucc
                ,SimpleFailAction
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }

    SUB_TRANS_UP(SubSimpleActionFail,
              ACTIONS(SimpleAction
                      , SimpleFailAction
              )
    )

    TEST(TransactionTest, sub_trans_fail_main_trans_fail) {
        TRANS(trans,
              ACTIONS( SimpleAction
                      ,SubSimpleActionFail
              )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransFail);
    }

} // namespace
