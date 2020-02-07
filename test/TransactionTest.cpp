
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

    ActionResult SimpleAction2(Context* context) {
        CAST_TO(SimpleStruct1, s);
        s->t[0] = 1;

        CAST_TO(SimpleStruct, s2);
        s2->x++;
        return ActionOk;
    }

    ActionResult SimpleAction(Context* context) {
        CAST_TO(SimpleStruct, s);
        s->x = 1;
        return ActionOk;
    }

    ActionResult SimpleActionCheck(Context* context) {
        CAST_TO(SimpleStruct, s);
        mock().actualCall("SimpleActionCheck").withParameter("x", s->x);
        return ActionOk;
    }

    DEF_CTXT(SimpleFailStruct){
        char y;
    };

    ActionResult SimpleFailAction(Context* context) {
        CAST_TO(SimpleFailStruct, s);
        s->y = 2;
        return ActionErr;
    }

    CtxtActionUse SimpleAction2CtxtDesc[] = {
            DEF_ACTION_CTXT(SimpleStruct1)
            ,DEF_ACTION_CTXT(SimpleStruct1)
    };

    CtxtActionUse SimpleActionCtxtDesc[] = {
            DEF_ACTION_CTXT(SimpleStruct)
    };

    CtxtActionUse SimpleFailActionCtxtDesc[] = {
            DEF_ACTION_CTXT(SimpleFailStruct)
    };

    TEST_GROUP(TransactionTest){
        void teardown()
        {
            mock().clear();
        }
    };


    TEST(TransactionTest, exec_one_action) {
        TRANS(trans,
              ACTIONS( DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                      ,DEF_ACTION_DESC(SimpleAction2 , SimpleAction2CtxtDesc)
                      ,DEF_ACTION_DESC(SimpleActionCheck , SimpleActionCtxtDesc)
              )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransSucc);
        mock().checkExpectations();
    }

    TEST(TransactionTest, exec_one_failed_action_stop_trans) {
        mock().expectNoCall("SimpleActionCheck");
        TRANS(trans,
                ACTIONS( DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                        ,DEF_ACTION_DESC(SimpleFailAction , SimpleFailActionCtxtDesc)
                        ,DEF_ACTION_DESC(SimpleActionCheck , SimpleActionCtxtDesc)
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

    ActionResult SimpleActionRollbackCheck(Context* context) {
        RollbackStruct s = {Expect_X};
        RollbackData data = {&s, sizeof(s)};
        AddRollBack(&context->rollbackData, RollBackActionDemo, &data);
        return ActionOk;
    }

    TEST(TransactionTest, exec_one_failed_action_roll_back) {
        mock().expectOneCall("RollBackDemo").withParameter("x", Expect_X);
        TRANS(trans,
                ACTIONS( DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                        , DEF_ACTION_DESC(SimpleActionRollbackCheck , SimpleActionCtxtDesc)
                        , DEF_ACTION_DESC(SimpleFailAction , SimpleFailActionCtxtDesc)
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }


    SUB_TRANS(SubSimpleAction,
              ACTIONS(DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                            , DEF_ACTION_DESC(SimpleActionRollbackCheck , SimpleActionCtxtDesc)
                            , DEF_ACTION_DESC(SimpleFailAction , SimpleFailActionCtxtDesc))
    )


    TEST(TransactionTest, sub_proc_failed_action_roll_back_do_not_effect_main_proc) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
                ACTIONS( DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                               , DEF_NULL_CTXT_ACTION_DESC(SubSimpleAction)
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransSucc);
        mock().checkExpectations();
    }

    SUB_TRANS(SubSimpleActionSucc,
              ACTIONS(DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                              , DEF_ACTION_DESC(SimpleActionRollbackCheck , SimpleActionCtxtDesc)
              )
    )

    TEST(TransactionTest, sub_trans_succ_but_main_trans_fail_also_need_roll_back) {
        mock().expectOneCall("RollBackDemo").ignoreOtherParameters();
        TRANS(trans,
                ACTIONS( DEF_ACTION_DESC(SimpleAction , SimpleActionCtxtDesc)
                ,DEF_NULL_CTXT_ACTION_DESC(SubSimpleActionSucc)
                ,DEF_ACTION_DESC(SimpleFailAction , SimpleFailActionCtxtDesc)
                )
        )
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, TransFail);
        mock().checkExpectations();
    }

} // namespace
