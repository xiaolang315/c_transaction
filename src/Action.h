#ifndef __ACTION_H_
#define __ACTION_H_

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
    int id;
    int size;
} CtxtActionUse;

FWD_DECL(AsynContext);
typedef AsynContext (*CreateSubTrans)(Context* context);

typedef enum ActionType {
    SyncAction,
    AsynAction,
    AsynSubTrans,
} ActionType;
typedef struct ActionDesc {
    Action action;
    int ctxtNum;
    CtxtActionUse* contexts;
    ActionType type;
    CreateSubTrans subTrans;
} ActionDesc;

extern CtxtActionUse* NullActionUse;

#define DEF_ACTION_DESC(action, context) {action, ARRAY_SIZE(context), context, SyncAction, NULL}
#define DEF_ASYN_ACTION_DESC(action, context) {action, ARRAY_SIZE(context), context, AsynAction, NULL}
#define DEF_NULL_CTXT_ACTION_DESC(action) {action, 0, NullActionUse, SyncAction, NULL}

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


#define NULL_CTXT_ACTION_DEF(name)\
ActionResult name##func(Context* context);\
ActionDesc name = DEF_NULL_CTXT_ACTION_DESC(name##func);\
ActionResult name##func

#endif // __ACTION_H_
