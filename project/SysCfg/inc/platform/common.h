#ifndef    _COMMON_H_
#define    _COMMON_H_

#ifndef FOR_EMULATE
#define FOR_EMULATE (0)    //仿真情况
#endif

#ifndef U8
#define    U08      unsigned    char            //unsigned 8bit
#endif

#ifndef U08
#define    U08      unsigned    char            //unsigned 8bit
#endif

#ifndef U16
#define    U16     unsigned    short        //unsigned 16bit
#endif

#ifndef U32
#define    U32     unsigned 	long            //unsigned 32bit
#endif

#ifndef U64
#define    U64     unsigned    long  long    //unsigned 64bit
#endif

#ifndef U8_V
#define    U8_V    volatile	   U08
#endif

#ifndef U08_V
#define    U08_V    volatile	 U08
#endif


#ifndef U16_V
#define    U16_V    volatile	   U16
#endif
#ifndef U32_V
#define    U32_V    volatile	   U32
#endif
#ifndef U64_V
#define    U64_V    volatile	   U32
#endif


#ifndef U8_C
#define    U8_C    const	   U08
#endif

#ifndef U08_C
#define    U08_C    const	   U08
#endif


#ifndef U16_C
#define    U16_C    const	   U16
#endif

#ifndef U32_C
#define    U32_C    const	   U32
#endif

#ifndef U64_C
#define    U64_C    const	   U64
#endif

#ifndef	   BOOL
#define    BOOL    U8
#endif

#ifndef	   TRUE
#define    TRUE    (1U)
#endif

#ifndef	   FALSE
#define    FALSE    (0U)
#endif

#ifndef NULL
#define    NULL   (0)
#endif

#define    LOW     (0)
#define    HIGH    (1)

#ifndef __IO
#define   __IO volatile
#define   __I  volatile
#define   __O  volatile
#endif

// maximum values of exact-width unsigned integer types 
#ifndef 	U8_MAX
#define    U8_MAX      (255U)
#endif

#ifndef    U16_MAX
#define    U16_MAX     (65535U)
#endif

#ifndef    ALL_ZERO
#define    ALL_ZERO    (0)
#endif

#ifndef    U8_ALL_ONE
#define    U8_ALL_ONE  (0xFF)
#endif

#ifndef    U16_ALL_ONE
#define    U16_ALL_ONE (0xFFFF)
#endif

#ifndef		U32_ALL_ONE
#define    U32_ALL_ONE (0xFFFFFFFF)
#endif

#ifndef __noinit__
#define __noinit__ __attribute__((zero_init)) 
#endif

#ifndef __align
#define __align(x) __attribute__ ((aligned(x)))
#endif


#ifndef __section
#define __section(_name) __attribute__ ((section(_name)))
#endif

#ifndef __packed
#define __packed __attribute__ ((__packed__))
#endif

#ifndef ARRAY
#define ARRAY(x,y)    (((U8 *)&(x))[y])
#endif
#include "sg9081.h"
#endif /*end of _COMMON_H_*/
