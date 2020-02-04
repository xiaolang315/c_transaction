
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


    ContextDesc SimpleAction2CtxtDesc[] = {
            DEF_ACTION_CTXT(SimpleStruct1)
            ,DEF_ACTION_CTXT(SimpleStruct1)
    };

    ContextDesc SimpleActionCtxtDesc[] = {
            DEF_ACTION_CTXT(SimpleStruct)
    };



    ActionDesc actions[] = {
            {SimpleAction ,      DEF_CTXT_DESC(SimpleActionCtxtDesc)}
            ,{SimpleAction2 ,     DEF_CTXT_DESC(SimpleAction2CtxtDesc)}
            ,{SimpleActionCheck , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
    };

    ContextDesc SimpleFailActionCtxtDesc[] = {
            DEF_ACTION_CTXT(SimpleFailStruct)
    };

    ActionDesc actionsFail[] = {
             { SimpleAction , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
            ,{ SimpleFailAction , DEF_CTXT_DESC(SimpleFailActionCtxtDesc)}
            ,{SimpleActionCheck , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
    };


    TEST_GROUP(TransactionTest){
        void teardown()
        {
            mock().clear();
        }
    };


    TEST(TransactionTest, exec_one_action) {
        Transaction trans = TRANSACTION_DEF(actions);
        mock().expectOneCall("SimpleActionCheck").withParameter("x",  2);
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, Succ);
        mock().checkExpectations();
    }

    TEST(TransactionTest, exec_one_failed_action_stop_trans) {
        mock().expectNoCall("SimpleActionCheck");
        Transaction trans = TRANSACTION_DEF(actionsFail);
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, Fail);
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

    ActionDesc actionsRollBack[] = {
             { SimpleAction , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
            ,{ SimpleActionRollbackCheck , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
            ,{ SimpleFailAction , DEF_CTXT_DESC(SimpleFailActionCtxtDesc)}
    };

    TEST(TransactionTest, exec_one_failed_action_roll_back) {
        mock().expectOneCall("RollBackDemo").withParameter("x", Expect_X);
        Transaction trans = TRANSACTION_DEF(actionsRollBack);
        TransResult ret = exec(&trans);
        CHECK_EQUAL(ret, Fail);
        mock().checkExpectations();
    }

} // namespace
