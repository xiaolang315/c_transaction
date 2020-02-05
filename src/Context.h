#ifndef __CONTEXT_H_
#define __CONTEXT_H_

#include <stdint.h>
#include "ExternC.h"
#include "BaseType.h"
#include "FwdDecl.h"

EXTERN_STDC_BEGIN


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

typedef struct ContextSet {
    int id;
    int offset;
}ContextSet;

typedef struct ContextMap {
    ContextSet* hash;
    int hashNum;
} ContextMap;

typedef void* (*CastTo)(const ContextMap*, void* data, int id);

typedef struct Context {
  void* data;
  CastTo castTo;
  ContextMap map;
  RollbackContext rollbackData;
} Context;

#define CTXT_ID(name) name##Id

#define CAST_TO(type, var)\
  type* var = (type*)(context->castTo(&context->map, context->data, CTXT_ID(type)));

#define DEF_CTXT(name)\
const int CTXT_ID(name) = __COUNTER__;\
struct name

BOOL AddRollBack(RollbackContext* , RollBackAction, const RollbackData*);

FWD_DECL(ActionDesc);
BOOL initContext(Context* context, ActionDesc*, int actionNum) ;
void destroyContext(Context* context) ;

EXTERN_STDC_END

#endif // __CONTEXT_H_
