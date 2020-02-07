//
// Created by zhangchao on 2020/2/7.
//

#ifndef C_TRANSACTION_SYNCTRANS_H
#define C_TRANSACTION_SYNCTRANS_H


TransResult exec(const Transaction* trans);

FWD_DECL(Context);
typedef void (*PrepareChildCtxtFunc)(const Context* parent, Context* child);

void NoPrepareChildCtxtFunc(const Context* parent, Context* child);
TransResult subExec(Context* parent, PrepareChildCtxtFunc, const SyncTrans* trans);


#endif //C_TRANSACTION_SYNCTRANS_H
