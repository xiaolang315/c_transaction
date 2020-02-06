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
} CtxtActionUse;

typedef struct ActionDesc {
    Action action;
    int ctxtNum;
    CtxtActionUse* contexts;
} ActionDesc;

extern CtxtActionUse* NullActionUse;
#define NULL_CTXT_DESC() 0, NullActionUse

#define DEF_CTXT_DESC(name) ARRAY_SIZE(name), name

#define DEF_ACTION_CTXT(name) {CTXT_ID(name), sizeof(name)}

#endif // __ACTION_H_
