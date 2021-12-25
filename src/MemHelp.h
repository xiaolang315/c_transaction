//
// Created by zhangchao on 2020/2/11.
//

#ifndef A0B2B553_0ECE_4051_86E7_17BE0A343B53
#define A0B2B553_0ECE_4051_86E7_17BE0A343B53

#include "mcl/stdc.h"
#include "MemManager.h"

MCL_STDC_BEGIN

#define STRUCT_ALLOC(type, var)\
type* var = (type*)mallocTc(sizeof(type));

#define ARRAY_ALLOC(type, var, num)\
var = (type*)mallocTc(sizeof(type) * num)

#define CHECK_FREE(ptr)\
if(ptr != NULL) {freeTc(ptr); ptr = NULL;}

MCL_STDC_END

#endif /* A0B2B553_0ECE_4051_86E7_17BE0A343B53 */
