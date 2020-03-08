#ifndef __FOREACH_H_
#define __FOREACH_H_

#include <stdint.h>

#define IN(list) , list
#define LIMITS(num) , num

#define WITH_TYPE(type),

#define ITEM_INDEX(item) item##i
#define FOREACH_FROM(type, item, list, start, num) \
 for( uint32_t ITEM_INDEX(item) = start; ITEM_INDEX(item) < (num); ITEM_INDEX(item) ++ ) {\
    type* item = &list[ITEM_INDEX(item)];

#define FOREACH(type, item, list, num) \
FOREACH_FROM(type, item, list, 0, num)


#define FOREACH_X(type, ...) \
FOREACH(type,  __VA_ARGS__)

#define FOREACH_END() }

#endif // __FOREACH_H_
