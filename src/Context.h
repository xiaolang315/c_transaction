#ifndef __CONTEXT_H_
#define __CONTEXT_H_

#include "ExternC.h"

EXTERN_STDC_BEGIN

typedef void* (*CastTo)(void* data, int id);

typedef struct RollbackData {
    void* mem;
    int len;
} RollbackData;

typedef void (*RollBackAction)(RollbackData*);

typedef struct OneRollBackContext {
    RollbackData data;
    RollBackAction action;
} OneRollBackContext;

typedef struct RollbackContext {
    int num;
    OneRollBackContext* contexts;
} RollbackContext;

void AddRollBack(RollbackContext* , RollBackAction, const RollbackData*);
typedef struct Context {
  void* data;
  CastTo castTo;
  RollbackContext rollbackData;
} Context;

#define CTXT_ID(name) name##Id

#define CAST_TO(type, var)\
  type* var = (type*)(context->castTo(context->data, CTXT_ID(type)));

#define DEF_CTXT(name)\
const int CTXT_ID(name) = __COUNTER__;\
struct name

EXTERN_STDC_END

#endif // __CONTEXT_H_
