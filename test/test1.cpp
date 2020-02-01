
#include "gtest/gtest.h"
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

    ActionResult SimpleAction1(Context* context) {
        CAST_TO(SimpleStruct, s);
        s->x++;
        if(s->x != 2) return ActionErr;
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

#define DEF_CTXT(name) {CTXT_ID(name), sizeof(name)}

    ContextDesc SimpleAction2CtxtDesc[] = {
            DEF_CTXT(SimpleStruct1)
            ,DEF_CTXT(SimpleStruct1)
    };

    ContextDesc SimpleActionCtxtDesc[] = {
            DEF_CTXT(SimpleStruct)
    };



    ActionDesc actions[] = {
             { SimpleAction2 , DEF_CTXT_DESC(SimpleAction2CtxtDesc)}
            ,{ SimpleAction , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
            ,{ SimpleAction1 , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
    };

    ContextDesc SimpleFailActionCtxtDesc[] = {
            DEF_CTXT(SimpleFailStruct)
    };

    ActionDesc actionsFail[] = {
             { SimpleAction , DEF_CTXT_DESC(SimpleActionCtxtDesc)}
            ,{ SimpleFailAction , DEF_CTXT_DESC(SimpleFailActionCtxtDesc)}
    };


    struct TransactionTest : testing::Test {
        virtual void SetUp() override {

        }

        virtual void TearDown() override {
        }

    };

    TEST_F(TransactionTest, exec_one_action) {
        Transaction trans = TRANSACTION_DEF(actions);
        TransResult ret = exec(&trans);
        ASSERT_EQ(ret, Succ);
    }

    TEST_F(TransactionTest, exec_one_failed_action) {
        Transaction trans = TRANSACTION_DEF(actionsFail);
        TransResult ret = exec(&trans);
        ASSERT_EQ(ret, Fail);
    }
} // namespace
