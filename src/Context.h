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
    uint32_t len;
} RollbackData;

typedef void (*RollBackAction)(RollbackData*);

typedef struct OneRollBackContext {
    RollbackData data;
    RollBackAction action;
} OneRollBackContext;

typedef struct RollbackContext {
    uint32_t num;
    uint32_t maxNum;
    OneRollBackContext* contexts;
    struct RollbackContext* next;
} RollbackContext;

typedef struct ContextSet {
    uint32_t id;
    uint32_t offset;
}ContextSet;

typedef struct ContextMap {
    ContextSet* hash;
    uint32_t hashNum;
} ContextMap;

typedef void* (*CastTo)(const ContextMap*, void* data, uint32_t id);

FWD_DECL(AsynContext);
typedef struct RuntimeAction {
    Action action;
    struct RuntimeAction* next;
    Context* context;
} RuntimeAction;

typedef struct AsynContext {
    RuntimeAction current;
    void* runtimeActionsBuff;
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
Context* initContext(ActionDesc* actions, uint32_t actionNum);
void destroyContext(Context* context) ;

EXTERN_STDC_END

#endif // __CONTEXT_H_
