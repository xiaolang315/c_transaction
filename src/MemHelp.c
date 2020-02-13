//
// Created by zhangchao on 2020/2/11.
//

#include "MemHelp.h"
#include "Foreach.h"

void freeAll(void** buff, uint32_t num){
    FOREACH(void*, var, buff, num)
        freeTc(*var);
    FOREACH_END()
}

