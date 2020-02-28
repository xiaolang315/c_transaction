# c_transaction

采用C语言实现的`事务管理`提供顺序执行和回滚的的能力，支持同步事务和异步事务的定义。

## 待支持的特性：
* 更完善的测试
* 内存分配算法指定
* 文档支持
* 运行log
* demo
* 使用堆内存时，也提供内存泄漏检测。

## 支持的特性
* 根据定义action所依赖的struct，自动生成context
* 在action内可以注册回滚函数，后续执行失败后，可以执行之前注册的回滚函数
* 支持嵌套事务的定义，子事务执行成功，父事务失败，可以执行子事务的回滚动作
* 支持异步事务的定义，以及异步事务的回滚。
* 默认使用堆内存，可以选择使用静态内存，
* 当使用静态内存时，可以进行内存泄漏的检查

## 快速开始

### 概念解读
* Action 事务原子操作, 支持同步（SYNC_ACTION_DEF）异步（SYNC_ACTION_DEF）定义
* ActionContext Action依赖的上下文结构。
* Context 事务上下文，存储前后Action之间关联数据和回滚数据。
* TransAction 完整事务描述。

### 定义使用Action
````c
    DEF_ACTION_CTXT(SimpleFailStruct){
        char y;
    };

    SYNC_ACTION_DEF(SimpleFailAction, ACTION_CTXTs(ACTION_CTXT(SimpleFailStruct)))(Context* context) {
        CAST_TO(SimpleFailStruct, s);
        s->y = 2;
        return ActionErr;
    }
````
使用`CAST_TO`把 `Context`转换 为`Action`所依赖的`ActionContext`
### 定义执行Trans
```c
 TRANS(trans,
              ACTIONS(SimpleSyncAction
                      , SimpleAction2
                      , SimpleActionCheck
              )
        )
 TransResult ret = syncExec(&trans);
```
`ACTIONS`中顺序放之前定义的动作，框架会根据所有`Action`所依赖的`ActionContext`来确定
`Context`的大小。

### 回滚动作的定义和插入

```c
    void RollBackActionDemo(RollbackData* data){
        if(data->len != sizeof(RollbackStruct)) return ;
        RollbackStruct* s = (RollbackStruct*)data->mem;
    }

    NULL_CTXT_SYNC_ACTION_DEF(SimpleActionRollbackCheck)(Context* context) {
        RollbackStruct s = {Expect_X};
        RollbackData data = {&s, sizeof(s)};
        addRollBack(&context->rollbackData, RollBackActionDemo, &data);
        return ActionOk;
    }
```
### 异步Trans定义

异步 `Action` 定义
```c
ASYN_ACTION_DEF(InitAction, ACTION_CTXTs(ACTION_CTXT(Simple1)))(Context *context) {
    CAST_TO(Simple1, s);
    s->x = 0;
    return ActionOk;
}
```
异步 `Transaction` 使用异步 `Action`
```c
    TRANS(trans,
          ACTIONS(InitAction, SimpleAsynAction, sub1
          )
    )

    Context *context;
    TransResult ret = asynStart(&trans, &context);
    ret = asynExec(context);
```
需要保留上下文指针，在需要时继续 `Transaction`

## Test
使用CppUtest测试框架， https://github.com/cpputest/cpputest .框架自带一个Mock工具，虽然做不到Mockcpp那样无侵入的打桩static函数，
但对作为底层框架，不需要mock第三方函数的，使用起来也比较方便，尤其是换了诸多Mock工具对Mac支持都不太好。
```cpp
    SYNC_ACTION_DEF(SimpleActionCheck, ACTION_CTXTs(ACTION_CTXT(SimpleStruct)))(Context* context) {
            CAST_TO(SimpleStruct, s);
            mock().actualCall("SimpleActionCheck").withParameter("x", s->x);
            return ActionOk;
    }

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

```