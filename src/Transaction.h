#ifndef __TRANSACTION_H_
#define __TRANSACTION_H_


#include "Action.h"
#include "ExternC.h"

EXTERN_STDC_BEGIN

typedef struct Transaction {
  ActionDesc* actions;
  unsigned int actionNum;
} Transaction;

#define TRANSACTION_DEF(actions)\
  {actions, ARRAY_SIZE(actions)}

typedef enum TransResult {
  TransSucc,
  TransFail
}TransResult;

TransResult exec(const Transaction* trans);

EXTERN_STDC_END

#endif // __TRANSACTION_H_
