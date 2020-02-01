#ifndef __FOREACH_H_
#define __FOREACH_H_


#define FOREACH(type, item, list, num) \
 for( unsigned int item##i = 0; item##i < num; item##i++ ) {\
    type* item = &list[item##i];

#define FOREACH_END() }

#endif // __FOREACH_H_
