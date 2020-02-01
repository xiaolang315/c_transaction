#ifndef __ACTION_H_
#define __ACTION_H_

#include "Util.h"

typedef struct Context Context;

typedef enum ActionResult {
   ActionOk,
   ActionErr
} ActionResult;

typedef ActionResult (*Action)(Context* context);

typedef struct ContextDesc {
    int id;
    int size;
} ContextDesc;

typedef struct ActionDesc {
    Action action;
    int num;
    ContextDesc* contexts;
} ActionDesc;

#define DEF_CTXT_DESC(name) ARRAY_SIZE(name), name

#endif // __ACTION_H_
