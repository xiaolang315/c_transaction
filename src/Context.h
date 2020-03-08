#ifndef __CONTEXT_H_
#define __CONTEXT_H_

#include "ExternC.h"
#include "BaseType.h"
#include "FwdDecl.h"
#include "ActionContext.h"

EXTERN_STDC_BEGIN

FWD_DECL(AsynContext);
FWD_DECL(ActionDesc);
FWD_DECL(RollbackContext);

typedef struct Context {
  void* data;
  RollbackContext* rollbackData;
  AsynContext* asynContext;
} Context;

#define CAST_TO(type, var)\
  type* var = (type*)(castTo(context, CTXT_ID(type)));

void* castTo(Context* context, uint32_t id);

Context* initContext(ActionDesc* actions, uint32_t actionNum);
void destroyContext(Context* context) ;

EXTERN_STDC_END

#endif // __CONTEXT_H_
