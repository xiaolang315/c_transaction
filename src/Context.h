#ifndef __CONTEXT_H_
#define __CONTEXT_H_

typedef void* (*CastTo)(void* data, int id);
typedef struct Context {
  void* data;
  CastTo castTo;
} Context;

#define CTXT_ID(name) name##Id

#define CAST_TO(type, var)\
  type* var = (type*)(context->castTo(context->data, CTXT_ID(type)));

#define DEF_CTXT(name)\
const int CTXT_ID(name) = __COUNTER__;\
struct name

#endif // __CONTEXT_H_
