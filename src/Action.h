#ifndef __ACTION_H_
#define __ACTION_H_

#include <stdint.h>
#include "Util.h"
#include "BaseType.h"
#include "FwdDecl.h"

typedef struct Context Context;

typedef enum ActionResult {
   ActionOk,
   ActionContinue,
   ActionErr,
   ActionUnknow
} ActionResult;

typedef ActionResult (*Action)(Context* context);

typedef struct ContextDesc {
    uint32_t id;
    uint32_t size;
} CtxtActionUse;

typedef enum ActionType {
    SyncActionType,
    AsynActionType,
} ActionType;

typedef struct ActionDesc {
    Action action;
    uint32_t ctxtNum;
    CtxtActionUse* contexts;
    ActionType type;
} ActionDesc;

extern CtxtActionUse* NullActionUse;

#define DEF_ACTION_DESC(action, context) {action, ARRAY_SIZE(context), context, SyncActionType}
#define DEF_NULL_CTXT_ACTION_DESC(action) {action, 0, NullActionUse, SyncActionType}

#define DEF_ASYN_ACTION_DESC(action, context) {action, ARRAY_SIZE(context), context, AsynActionType}
#define DEF_NULL_CTXT_ASYN_ACTION_DESC(action) {action, 0, NullActionUse, AsynActionType}

#define DEF_ACTION_CTXT(name) {CTXT_ID(name), sizeof(name)}

#define STRUCTS(...) {__VA_ARGS__}

#define ACTION_DEF(name, structs)\
ActionResult name##func(Context* context);\
CtxtActionUse name##_stucts[] = structs;\
ActionDesc name = DEF_ACTION_DESC(name##func, name##_stucts);\
ActionResult name##func

#define ASYN_ACTION_DEF(name, structs)\
ActionResult name##func(Context* context);\
CtxtActionUse name##_stucts[] = structs;\
ActionDesc name = DEF_ASYN_ACTION_DESC(name##func, name##_stucts);\
ActionResult name##func


#define NULL_CTXT_SYNC_ACTION_DEF(name)\
ActionResult name##func(Context* context);\
ActionDesc name = DEF_NULL_CTXT_ACTION_DESC(name##func);\
ActionResult name##func

#endif // __ACTION_H_
