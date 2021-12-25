#ifndef F0C97A87_F9E1_4866_85C8_E8468F4E79A5
#define F0C97A87_F9E1_4866_85C8_E8468F4E79A5

#include "mcl/stdc.h"
#include "BaseType.h"
#include "mcl/typedef.h"
#include "ActionContext.h"

MCL_STDC_BEGIN

MCL_TYPE_DECL(AsynContext);
MCL_TYPE_DECL(ActionDesc);
MCL_TYPE_DECL(RollbackContext);

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

#endif /* F0C97A87_F9E1_4866_85C8_E8468F4E79A5 */
