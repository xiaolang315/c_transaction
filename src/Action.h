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

#define DEF_SYNC_ACTION_DESC(action, context) {action, ARRAY_SIZE(context), context, SyncActionType}
#define DEF_NULL_CTXT_ACTION_DESC(action, context) {action, 0, NullActionUse, SyncActionType}
#define DEF_ASYN_ACTION_DESC(action, context) {action, ARRAY_SIZE(context), context, AsynActionType}

#define ACTION_CTXT(name) {CTXT_ID(name), sizeof(name)}
#define ACTION_CTXTs(...) {__VA_ARGS__}

#define ACTION_DEF(name, name_struct, type_macro)\
ActionResult name##func(Context* context);\
ActionDesc name = type_macro(name##func, name_struct);\
ActionResult name##func

#define SYNC_ACTION_DEF(name, structs)\
CtxtActionUse name##_stucts[] = structs;\
ACTION_DEF(name, name##_stucts, DEF_SYNC_ACTION_DESC)

#define ASYN_ACTION_DEF(name, structs)\
CtxtActionUse name##_stucts[] = structs;\
ACTION_DEF(name, name##_stucts, DEF_ASYN_ACTION_DESC)

#define NULL_CTXT_SYNC_ACTION_DEF(name)\
ACTION_DEF(name, "", DEF_NULL_CTXT_ACTION_DESC)

#endif // __ACTION_H_
