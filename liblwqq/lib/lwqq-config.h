#ifndef _LWQQ_CONFIG_H__
#define _LWQQ_CONFIG_H__

#define WITH_LIBEV
/* #undef WITH_LIBUV */
#define WITH_SQLITE
/* #undef WITHOUT_ASYNC */
#define ENABLE_SSL
#define WITH_MOZJS
#define MOZJS_185
/* #undef MOZJS_17 */

//use a single thread to poll message
#define USE_MSG_THREAD 1

//enable additional debug function
#define USE_DEBUG 0

#endif /* __CONFIG_H__ */

