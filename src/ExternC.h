#ifndef __EXTERNC_H_
#define __EXTERNC_H_

#ifdef __cplusplus
    #define EXTERN_STDC_BEGIN extern "C" {
    #define EXTERN_STDC_END  }
#else
    #define EXTERN_STDC_BEGIN
    #define EXTERN_STDC_END
#endif

#endif // __EXTERNC_H_
