# c_transaction

采用C语言实现的`事务管理`提供顺序执行和回滚的的能力，支持同步事务和异步事务的定义。

## 待支持的特性：
* 更完善的测试
* 使用堆内存时，也提供内存泄漏检测。
* 内存分配算法指定
* 文档支持
* 运行log
* demo

## 支持的特性
* 根据定义action所依赖的struct，自动生成context
* 在action内可以注册回滚函数，后续执行失败后，可以执行之前注册的回滚函数
* 支持嵌套事务的定义，子事务执行成功，父事务失败，可以执行子事务的回滚动作
* 支持异步事务的定义，以及异步事务的回滚。
* 默认使用堆内存，可以选择使用静态内存，
* 当使用静态内存时，可以进行内存泄漏的检查

## 快速开始

### 定义Action
````c
 ACTION_DEF(ActionName, STRUCTS(DEF_ACTION_CTXT(SimpleStruct)))(Context* context) {
        CAST_TO(SimpleStruct, s);
        return ActionOk;
    }
````



## Test
使用CppUtest测试框架， https://github.com/cpputest/cpputest .框架自带一个Mock工具，虽然做不到Mockcpp那样无侵入的打桩static函数，
但对作为底层框架，不需要mock第三方函数的，使用起来也比较方便，尤其是换了诸多Mock工具对Mac支持都不太好。
