
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "SyncTransaction.h"
#include "Context.h"
#include "MemManager.h"
#include "RollbackContext.h"

namespace {
    DEF_ACTION_CTXT(SimpleStruct){
        int x;
    };

    DEF_ACTION_CTXT(SimpleStruct1){
        int t[10];
    };

    SYNC_ACTION_DEF(SimpleAction2, ACTION_CTXTs(
            ACTION_CTXT(SimpleStruct1)
            , ACTION_CTXT(SimpleStruct)
            ))(Context* context) {
        CAST_TO(SimpleStruct1, s);
        s->t[0] = 1;

        CAST_TO(SimpleStruct, s2);
        s2->x++;
        return ActionOk;
    }

    SYNC_ACTION_DEF(SimpleSyncAction, ACTION_CTXTs(ACTION_CTXT(SimpleStruct)))(Context* context) {
        CAST_TO(SimpleStruct, s);
        s->x = 1;
        return ActionOk;
    }

    SYNC_ACTION_DEF(SimpleActionCheck, ACTION_CTXTs(ACTION_CTXT(SimpleStruct)))(Context* context) {
        CAST_TO(SimpleStruct, s);
        mock().actualCall("SimpleActionCheck").withParameter("x", s->x);
        return ActionOk;
    }

    DEF_ACTION_CTXT(SimpleFailStruct){
        char y;
    };

    SYNC_ACTION_DEF(SimpleFailAction, ACTION_CTXTs(ACTION_CTXT(SimpleFailStruct)))(Context* context) {
        CAST_TO(SimpleFailStruct, s);
        s->y = 2;
        return ActionErr;
    }

    TEST_GROUP(TransactionTest){
        TEST_SETUP() {
            static char buff[10000] ;
            useStaticMemory(buff, ARRAY_SIZE(buff));
        }

        TEST_TEARDOWN()
        {
            mock().clear();
            CHECK_EQUAL(NULL, checkMemLeaksPos());
            useHeapMemory();
        }
    };

    TEST(TransactionTest, exec_one_action) {
        mock().expectOneCall("SimpleActionCheck").ignoreOtherParameters();
        TRANS(trans,
              ACTIONS(SimpleSyncAction
                      , SimpleAction2
                      , SimpleActionCheck
              )
        )
        TransResult ret = syncExec(&trans);
        CHECK_EQUAL(ret, TransSucc);
        mock().checkExpectations();
    }

    TEST(TransactionTest, exec_one_failed_action_stop_trans) {
        mock().expectNoCall("SimpleActionCheck");
        TRANS(trans,
                ACTIONS(SimpleSyncAction
                        , SimpleFailAction
                        , SimpleActionCheck
                )
        )
        TransResult ret = syncExec(&trans);
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

    NULL_CTXT_SYNC_ACTION_DEF(SimpleActionRollbackCheck)(Context* context) {
        ROLL_BACK_DATA(RollbackStruct, data, Expect_X)
        addRollBackAction(context->rollbackData, RollBackActionDemo, &data);
        return ActionOk;
    }

    TEST(TransactionTest, exec_one_failed_action_roll_back) {
        mock().expectOneCall("RollBackDemo").withParameter("x", Expect_X);
        TRANS(trans,
                ACTIONS(SimpleSyncAction
                        , SimpleActionRollbackCheck
                        , SimpleFailAction
                )
        )
        TransResult ret = syncExec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }


    SYNC_SUB_TRANS(SubSimpleAction,
                   ACTIONS(SimpleSyncAction
                            , SimpleActionRollbackCheck
                            , SimpleFailAction
              )
    )


    TEST(TransactionTest, sub_proc_failed_action_roll_back_do_not_effect_main_proc) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
                ACTIONS(SimpleSyncAction
                               , SubSimpleAction
                )
        )
        TransResult ret = syncExec(&trans);
        CHECK_EQUAL(ret, TransSucc);
        mock().checkExpectations();
    }

    SYNC_SUB_TRANS(SubTransSucc,
                   ACTIONS(SimpleSyncAction
                      , SimpleActionRollbackCheck
              )
    )

    TEST(TransactionTest, sub_trans_succ_but_main_trans_fail_also_need_roll_back) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
                ACTIONS(SimpleSyncAction
                , SubTransSucc
                , SimpleFailAction
                )
        )
        TransResult ret = syncExec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }

    SYNC_SUB_TRANS(SubTransSuccLevel2,
                   ACTIONS(SimpleSyncAction
                           , SubTransSucc
                   )
    )

    TEST(TransactionTest, sub_trans_succ_but_main_trans_fail_also_need_roll_back_three_deepth) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
              ACTIONS(SimpleSyncAction
                      , SubTransSuccLevel2
                      , SimpleFailAction
              )
        )
        TransResult ret = syncExec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }

    SYNC_SUB_TRANS_UP(SubSimpleActionFail,
                      ACTIONS(SimpleSyncAction
                      , SimpleFailAction
              )
    )

    TEST(TransactionTest, upgrade_sub_trans_fail_cause_main_trans_fail) {
        TRANS(trans,
              ACTIONS(SimpleSyncAction
                      , SubSimpleActionFail
              )
        )
        TransResult ret = syncExec(&trans);
        CHECK_EQUAL(ret, TransFail);
    }

} // namespace
