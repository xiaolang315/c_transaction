//
// Created by zhangchao on 2020/2/24.
//

#ifndef C_TRANSACTION_ACTIONCONTEXT_H
#define C_TRANSACTION_ACTIONCONTEXT_H

#define CTXT_ID(name) name##Id

#define DEF_ACTION_CTXT(name)\
const int CTXT_ID(name) = __COUNTER__;\
struct name

#endif //C_TRANSACTION_ACTIONCONTEXT_H
