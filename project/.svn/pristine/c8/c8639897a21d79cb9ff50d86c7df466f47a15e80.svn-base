// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  uart.h
//  文件标识：  
//  内容摘要：  uart 相关寄存器配置接口定义
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
#ifndef __UART__
#define __UART__                                                        
/******************************包含头文件******************************/
#include "common.h"

/******************************类型和宏定义******************************/
#ifdef __DUAL_CORE__
#ifdef __G3_FLYCODE
#define UART_NUM   (0)
#else
#define UART_NUM   (0)
#endif
#else
#define UART_NUM   (0)
#endif

#ifndef UART_CFG
#define UART_CFG (3)
#endif

#define UART_DMA (0)
#define UART_REG (1)
#define UART_FIFO (2)

#ifndef __G0_CODE
#define UART_FIFO_SIZE (1024)
#else
#define UART_FIFO_SIZE (512)
#endif

#define UART_TXMODE (UART_FIFO)

#ifndef UART0
#define UART0 (0)
#endif

#define UART_REG (1)
#ifndef UART2
#define UART2 (2)
#endif

#define UART_BAUD_9600        (0U)
#define UART_BAUD_19200       (1U)
#define UART_BAUD_38400       (3U)
#define UART_BAUD_57600       (5U)
#define UART_BAUD_115200      (11U)

#define UART_TX         0
#define UART_RX         1
#define UART_BOTH       2

#define TX_DONE_BIT     ((U32)1<<0)
#define RX_DONE_BIT     ((U32)1<<1)    

/******************************变量声明******************************/

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

    // *****************************************************************************
    // 函数名称: UART_Init
    // 函数功能: 初始化函数
    // 入口参数: 
    //           bNum:串口号，可取值：UART0,UART1
    //           dBaud:波特率 UART_BAUD_9600,UART_BAUD_19200,UART_BAUD_38400,UART_BAUD_57600,UART_BAUD_115200
    //           dBufferAddr:收发缓冲区
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void UART_Init(U8 bNum,U32 dBaud,U32 dBufferAddr,U32 dSystemFrequency);
    
    // *****************************************************************************
    // 函数名称: UART_SendMsg
    // 函数功能: 初始化函数
    // 入口参数: 
    //          bNum:串口号，可取值：UART0,UART1
    //          pInfo:待发送的数据缓冲区
    //          wLen:待发送的数据长度
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void UART_SendMsg(U8 bNum,U8 *pInfo,U16 wLen);
    
    // *****************************************************************************
    // 函数名称: UART_Read
    // 函数功能: 初始化函数
    // 入口参数: 
    //          bNum:串口号，可取值：UART0,UART1
    //          pInfo:待发送的数据缓冲区
    //          wLen:待接收的数据长度
    // 出口参数:实际接收的数据长度
    // 说    明: 
    // *****************************************************************************
    U8 UART_Read(U8 bNum,U8 *pInfo,U16 wLen);
    
    // *****************************************************************************
    // 函数名称: UART_Read
    // 函数功能: 缓冲区数据大小
    // 入口参数: 
    //          bNum:串口号，可取值：UART0,UART1
    // 出口参数: 数据大小
    // 说    明: 
    // *****************************************************************************
U16 UART_BytesToRead(U8 bNum);
//    {
//        return SG_UART(bNum)->rTRANS_CNT.bits.wVAL;
//    }
#ifdef __cplusplus
}
#endif
#endif/*__UART__*/
