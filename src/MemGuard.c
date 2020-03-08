//
// Created by zhangchao on 2020/2/11.
//

#include "MemGuard.h"
#include "Foreach.h"
#include "MemManager.h"

void freeAll(void** buff, uint32_t num){
    FOREACH(void*, var, buff, num)
        freeTc(*var);
    FOREACH_END()
}

