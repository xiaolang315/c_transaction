#ifndef __CONTEXT_H_
#define __CONTEXT_H_

#include "mcl/stdc.h"
#include "BaseType.h"
#include "mcl/fwd_decl.h"
#include "ActionContext.h"

MCL_STDC_BEGIN

MCL_FWD_DECL(AsynContext);
MCL_FWD_DECL(ActionDesc);
MCL_FWD_DECL(RollbackContext);

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

MCL_STDC_END

#endif // __CONTEXT_H_
