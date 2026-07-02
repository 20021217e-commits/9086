// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  debug.h
//  文件标识：  
//  内容摘要：  调试功能定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月16日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef _DEBUG_H_

#define _DEBUG_H_
#include "common.h"

#ifndef __MODULE__
#define __MODULE__ __FILE__
#endif

#ifndef ASSERT_DEBUG_ON
#define ASSERT_DEBUG_ON     (1)
#endif

#ifndef GPIO_DEBUG_ON
#define GPIO_DEBUG_ON       (0)
#endif

#ifndef UART_DEBUG_ON

#ifndef __BOOT_CODE__
#if (1==FOR_EMULATE)

#define UART_DEBUG_ON       (0)

#else//#if (1==FOR_EMULATE)

#ifndef __G0_CODE
#ifndef __ALG_FORMAL
    #define UART_DEBUG_ON       (1)
#else
    #define UART_DEBUG_ON       (0)
#endif//#ifndef __ALG_FORMAL
#else
    #define UART_DEBUG_ON       (0)
#endif//#ifndef __G0_CODE

#endif
#define UART_PRINT_EN	    (1)  //用于区别 BOOT 和其它可打印的Code  boot code 关闭打印
#else
#define UART_PRINT_EN     	(0)
#define UART_DEBUG_ON       (0)
#endif
#endif

#ifndef DEBUG_OUT_BUFFER_SIZE
#define DEBUG_OUT_BUFFER_SIZE (128U)
#endif

extern void PRINT_TXT(char* text);
extern void PRINT_DEC(char* text,U32 dVAL);
extern void PRINT_HEX(char* text,U32 dVAL);
extern int PRINTS(const char *pfmt,void *param1,void *param2,void *param3,void *param4,void *param5);

#define PRINT1(FMT,A) 				PRINTS(FMT,&A,NULL,NULL,NULL,NULL)
#define PRINT2(FMT,A,B) 			PRINTS(FMT,&A,&B,NULL,NULL,NULL)
#define PRINT3(FMT,A,B,C) 			PRINTS(FMT,&A,&B,&C,NULL,NULL)
#define PRINT4(FMT,A,B,C,D) 		PRINTS(FMT,&A,&B,&C,&D,NULL)
#define PRINT5(FMT,A,B,C,D,E) 		PRINTS(FMT,&A,&B,&C,&D,&E)

#ifdef __cplusplus
extern "C" {
#endif
    
#if (GPIO_DEBUG_ON == 1)   
    
void GPIO_DEBUG_INIT(void);
void GPIO_DEBUGDW(U32 dwDATA);
    
inline static void GPIO_DEBUG(U8 bDATA)
{
    SG_PADCTRL->rPORT_DATA14.bits.dVAL = bDATA;
}
#else
//用无效的语句填充调试版本的代码，使其不参与编译
#define GPIO_DEBUG(x)
#define GPIO_DEBUGDW(x)
#define GPIO_DEBUG_INIT(x)
#endif
#ifndef __G0_CODE
extern __align(8) U8 aDebugInfoBuffer[DEBUG_OUT_BUFFER_SIZE];
#else
extern __align(8) U8 aDebugInfoBuffer[32];
#endif

#if (UART_DEBUG_ON  ==  1)

extern const char TRSP_TYPE[15][20];
extern const char LPS_STATE[8][15];
extern const char TLB_misc[6][35];
extern const char Machine_error[6][50];
extern const char General_exception[12][55];
extern const char I_TYPE[8][20];
extern int PRINT(const char *format,...);

/*DEBUG 模块开关*/
#define  APP_RAMDISK_DEBUG    (FALSE)
#define  APP_JUPITER_DEBUG    (TRUE)

#if (SATA_DEBUG_ON == TRUE)
#define UART_PRINT  	PRINT
#define ERROR_PRINT 	UART_PRINT
#define MONITOR_PRINT 	UART_PRINT
#else
#define UART_PRINT(x,...)
#define DEBUG_PRINT(x,...)
//#define ERROR_PRINT(x,...)
#define ERROR_PRINT  PRINT
//#define TEST_PRINT   PRINT
//#define TEST_FIFO_PRINT   PRINT
#define TEST_PRINT(x,...)
#define TEST_FIFO_PRINT(x,...)
#endif

#if(APP_JUPITER_DEBUG == TRUE)
#define LOG_PRINT    	PRINT
#define JPT_ERROR_PRINT    	PRINT
#else
#define LOG_PRINT(x,...)
#endif

#ifdef G3_DEBUG_ON
#define CODE_PRINT		PRINT
#else
#define CODE_PRINT(x,...)
#endif

int UART_PRINT_BUFF(U8 *pBuff,U32 dBuffSize);

#ifdef __PFM_MONITOR__
extern void pfm_restart();
extern void pfm_print(const char *,U8);
#else
#define pfm_restart(...)
#define pfm_print(...)
#endif

#if	(SIMULATION == 1)  
#include "stdio.h"
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR1          (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          (0x01000000)
#endif/*end of #if	(SIMULATION==1)*/

#if (ASSERT_DEBUG_ON == 1)

inline static void breakExecution(void)
{
    while(1);
}

#define BREAK  breakExecution()
#define ASSERT(x) \
    do {	\
        if(!((U32)(x))) { \
            PRINT("ASSERT failed! [%d] == 0.\n", ((U32)(x))); \
            BREAK;	\
        }	\
    } while(0)


/* Define for ASSERT_TRUE. */
#define ASSERT_TRUE(x) \
    do {	\
        if(!((U32)(x))) { \
            PRINT("ASSERT_TRUE failed! [%d] == 0.\n", (U32)((U32)(x))); \
            BREAK;	\
        }	\
    } while (0)


/* Define for ASSERT_FALSE. */
#define ASSERT_FALSE(x) \
    do {	\
        if(((U32)(x))) { \
            PRINT("ASSERT_FALSE failed! [%d] != 0.\n", ((U32)(x))); \
            BREAK;	\
        }	\
    } while (0)


/* Define for ASSERT_EQ (Equal).*/
#define ASSERT_EQ(x, y) \
    do {	\
        if(((U32)(x)) != ((U32)(y))) { \
            PRINT("ASSERT_EQ failed! [%d] != [%d].\n", ((U32)(x)), ((U32)(y))); \
            BREAK;	\
        }	\
    } while (0)


/* Define for ASSERT_NEQ (Not equal). */
#define ASSERT_NEQ(x, y) \
    do {	\
        if(((U32)(x)) == ((U32)(y))) { \
            PRINT("ASSERT_NEQ failed! [%d] == [%d].\n", ((U32)(x)), ((U32)(y))); \
            BREAK;	\
        }	\
    } while(0)


/* Define for ASSERT_GT (Greater than). */
#define ASSERT_GT(x, y) \
    do {	\
        if(((U32)(x)) <= ((U32)(y))) { \
            PRINT("ASSERT_GT failed! [%d] <= [%d].\n", ((U32)(x)), ((U32)(y))); \
            BREAK;	\
        }	\
    } while(0)


/* Define for ASSERT_GE (Greater or equal). */
#define ASSERT_GE(x, y) \
    do {	\
        if(((U32)(x)) < ((U32)(y))) { \
            PRINT("ASSERT_GE failed! [%d] < [%d].\n", ((U32)(x)), ((U32)(y))); \
            BREAK;	\
        }	\
    } while(0)


/* Define for ASSERT_LT (Less than). */
#define ASSERT_LT(x, y) \
    do {	\
        if(((U32)(x)) >= ((U32)(y))) { \
            PRINT("ASSERT_LT failed! [%d] >= [%d].\n", ((U32)(x)), ((U32)(y))); \
            BREAK;	\
        }	\
    } while(0)


/* Define for ASSERT_LE (Less or equal). */
#define ASSERT_LE(x, y) \
    do {	\
        if(((U32)(x)) > ((U32)(y))) { \
            PRINT("ASSERT_LE failed! [%d] > [%d].\n", ((U32)(x)), ((U32)(y))); \
            BREAK;	\
        }	\
    } while(0)

#else
#define ASSERT(x)
#define ASSERT_TRUE(x)
#define ASSERT_FALSE(x)
#define ASSERT_EQ(x, y)
#define ASSERT_NEQ(x, y)
#define ASSERT_GT(x, y)
#define ASSERT_GE(x, y)
#define ASSERT_LT(x, y)
#define ASSERT_LE(x, y)
#endif
    
#else
/******************UART_DEBUG_ON == 0***********************/

//用无效的语句填充调试版本的代码，使其不参与编译

#define DEBUG_PRINT(x,...)
#define PRINT(x,...)
#define UART_PRINT(x,...)
#define ERROR_PRINT(x,...)
#define LOG_PRINT(x,...)
#define CODE_PRINT(x,...)
#define UART_PRINT_BUFF(x,...)
#define pfm_restart(...)
#define pfm_print(...)

#define TEST_PRINT(x,...)
#define TEST_FIFO_PRINT(x,...)

#if  (FALSE == UART_PRINT_EN)
	#define PRINT_TXT(x)
	#define PRINT_DEC(x,...)
	#define PRINT_HEX(x,...)
#endif

#endif/*end of #if (UART_DEBUG_ON==1)*/

#ifdef __cplusplus
}
#endif
#endif/*_DEBUG_H_*/
