//
// Created by zhangchao on 2020/2/13.
//

#ifndef C_TRANSACTION_GLOBALMEMMANAGER_H
#define C_TRANSACTION_GLOBALMEMMANAGER_H

#include <stdint.h>
#include "ExternC.h"

EXTERN_STDC_BEGIN
//if you want use your local static global mem, tell me use this function

void useStaticMemory(void* buff, uint32_t size);
void useHeapMemory();

void* checkMemLeaksPos();

void* mallocTc(uint32_t);
void freeTc(void*);

EXTERN_STDC_END

#endif //C_TRANSACTION_GLOBALMEMMANAGER_H
