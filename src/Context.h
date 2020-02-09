#ifndef __CONTEXT_H_
#define __CONTEXT_H_

#include <stdint.h>
#include "ExternC.h"
#include "BaseType.h"
#include "FwdDecl.h"
#include "Action.h"

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
    struct RollbackContext* next;
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

FWD_DECL(AsynContext);
typedef struct RuntimeAction {
    Action action;
    struct RuntimeAction* next;
    Context* context;
} RuntimeAction;

typedef struct AsynContext {
    RuntimeAction current;
    void* runtimeActions;
} AsynContext;

typedef struct Context {
  void* data;
  CastTo castTo;
  ContextMap map;
  RollbackContext rollbackData;
  AsynContext* asynContext;
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
