#ifndef __FOREACH_H_
#define __FOREACH_H_

#define FOREACH_FROM(type, item, list, start, num) \
 for( uint32_t item##i = start; item##i < num; item##i++ ) {\
    type* item = &list[item##i];

#define FOREACH(type, item, list, num) \
FOREACH_FROM(type, item, list, 0, num)

#define FOREACH_END() }

#endif // __FOREACH_H_
