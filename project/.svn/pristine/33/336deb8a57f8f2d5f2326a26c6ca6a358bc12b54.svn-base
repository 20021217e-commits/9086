// *****************************************************************************
//  版权所有  (C) 2014,  硅格半导体有限公司 
//   
//  文件名称：  uitility.h
//  文件标识：  
//  内容摘要：  特殊功能函数的定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月21日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef  __UITILITY_H__
#define  __UITILITY_H__

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

#ifndef MemoryCopy
#define MemoryCopy 		U08MemCopy
#endif

#ifndef MemorySet
#define MemorySet 		U08MemSet
#endif

#ifndef Mem32Copy
#define Mem32Copy 		U32MemCopy
#endif

#ifndef Mem32SetVal
#define Mem32SetVal 	U32MemSet
#endif

#ifndef MemoryCmp
#define MemoryCmp 		U08MemCmp
#endif

#ifndef Mem32Cmp
#define Mem32Cmp 		U32MemCmp
#endif
/******************************************************************************/
/*                            功能宏定义申明                                  */
/******************************************************************************/
#define MIN_U8(a,b)        (((unsigned char)(a) < (unsigned char)(b)) ? (unsigned char)(a) : (unsigned char)(b))
#define MIN_U16(a,b)       (((unsigned short)(a) < (unsigned short)(b)) ? (unsigned short)(a) : (unsigned short)(b))
#define MIN_U32(a,b)       (((unsigned long)(a) < (unsigned long)(b)) ? (unsigned long)(a) : (unsigned long)(b))
#define DISABLE_SYSTICK    (SysTick->CTRL = 0;)
/******************************************************************************/
/*                            函数申明                                        */
/******************************************************************************/
void  Delay(U32  dTimerCnt);    //嵌入式汇编延时函数
void  NOP2(void);
void  CPU_INT_Disable(void);
void  CPU_INT_Enable(void);
void  U32MemCopy(U32 *pDest, U32 *pSour, U32 dLen);
void  U16MemCopy(U16 *pwDest, U16 *pwSour, U32 dLen);
void  U32MemSet(U32 *pDest,  U32 dValue, U32 dLen);
void  U16MemSet(U16 *pDest,  U16 bValue, U32 dLen);
void  U08MemSet(U08 *pDest,  U08 bValue, U32 dLen);
void U08MemCopy(U08 *pDest, U08 *pSour, U32 dLen);
U08  U08MemCmp(U08 *pDest, U08 *pSour, U32 dLen);
U08  U32MemCmp(U32 *pDest, U32 *pSour, U32 dLen);
void  U16MemPlus(U16 *pwDest, U16 wCnt, U16 wValue);
U8    CheckSum(U8 *pAddr, U32 wLen);
U8    GetSum(U8 *pAddr, U32 wLen);
U32   Check32Sum(U32 * pdSource, U16 wCnt);

#endif/*end of __UITILITY_H__*/
