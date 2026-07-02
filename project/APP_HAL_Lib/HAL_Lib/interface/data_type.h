// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称： data_type.h
//  文件标识：  
//  内容摘要： 数据类型定义
//  其它说明：  
//  当前版本： v1.0
//  项   目：  SG9081
//  作   者：  gary
// *****************************************************************************
#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

    #ifndef U8
    #define U8      unsigned    char
    #endif

    #ifndef U8_C
    #define U8_C    unsigned    char    const
    #endif

    #ifndef U08
    #define U08     unsigned    char
    #endif


    #ifndef U16
    #define U16     unsigned    short
    #endif

    #ifndef U32
    #define U32     unsigned    long
    #endif

    #ifndef U64
    #define U64     unsigned    long  long
    #endif

    #ifndef		TRUE
    #define		TRUE    (1)
    #endif

    #ifndef		FALSE
    #define		FALSE   (0)
    #endif


    #ifndef FUNC_DISABLE
    #define FUNC_DISABLE (0)
    #endif

    #ifndef FUNC_ENABLE
    #define FUNC_ENABLE  (1)
    #endif


    #ifndef BITS_OF_BYTE
    #define BITS_OF_BYTE    (8)
    #endif

    #ifndef INVALID_U8
    #define INVALID_U8  (0xFF)
    #endif

    #ifndef INVALID_U16
    #define INVALID_U16  (0xFFFF)
    #endif

    #ifndef INVALID_U32
    #define INVALID_U32  (0xFFFFFFFF)
    #endif

	#ifndef NULL
	#define NULL    (0)
	#endif

    #ifndef PNULL
    #define PNULL    ((void *)0)
    #endif

#ifdef __cplusplus
}
#endif

#endif
