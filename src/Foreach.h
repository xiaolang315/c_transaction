#ifndef __FOREACH_H_
#define __FOREACH_H_

#include <stdint.h>

#define ITEM_INDEX(item) item##i
#define FOREACH_FROM(type, item, list, start, num) \
 for( uint32_t ITEM_INDEX(item) = start; item##i < num; item##i++ ) {\
    type* item = &list[ITEM_INDEX(item)];

#define FOREACH(type, item, list, num) \
FOREACH_FROM(type, item, list, 0, num)

#define FOREACH_END() }

#endif // __FOREACH_H_
