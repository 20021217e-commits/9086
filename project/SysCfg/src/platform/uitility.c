//  版权所有  (C) 2014,  硅格半导体有限公司 
//   
//  文件名称：  SpsFunc.c
//  文件标识：  
//  内容摘要：  
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  Suke
//  完成日期：  2014年04月04日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 2015年07月21日 
//  版 本 号： 
//  修 改 人： william
//  修改内容： 项目->SG9081
// *****************************************************************************
#include "sg9081.h"
#include "uitility.h"

void NOP2(void)
{    
    //__nop();
    __nds32__nop();
}

// *****************************************************************************
// 函数名称: Delay()
// 函数功能: "嵌入式汇编"方式的延迟函数
// 入口参数: dTimerCnt :1--> 120ns;  83-->10us;  8333 --> 1ms;  sys clk:25mhz!!
// 出口参数: 
// 说    明: 在20mhz的系统时钟情况下，测试，1-->150ns;  1000-->150us!!
//           其实循环体共3个clk!!
//           SUBS    r0,r0,#1    // 1 clk
//             BCS     Delay       // 2 clk
//           BX      LR           
// *****************************************************************************
/*
  3ef8a4:	d5 04       	j8 3ef8ac <HAL_Delay+0x8>
  3ef8a6:	40 00 00 09 	nop
  3ef8aa:	80 01       	mov55 $r0,$r1
  3ef8ac:	9e 41       	subi333 $r1,$r0,#0x1
  3ef8ae:	c8 fc       	bnez38 $r0,3ef8a6 <HAL_Delay+0x2>
  5n+8
 * */
void  Delay(U32  dTimerCnt)
{
#if FOR_EMULATE
	dTimerCnt  = 50;	//缩短IC 仿真时间
#endif
    while(dTimerCnt--)
    {
        //__NOP();
        __nds32__nop();
    }
}

// *****************************************************************************
// 函数名称:void  CPU_INT_Disable()
// 函数功能: 嵌入式汇编关MCU中断
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void  CPU_INT_Disable(void)
{
     //register uint32_t __regPriMask         __ASM("primask");
      //__regPriMask = (1);
    __nds32__setgie_dis();
    __nds32__dsb();
}    

// *****************************************************************************
// 函数名称: __asm  void  CPU_INT_Enable()
// 函数功能: 嵌入式汇编开MCU中断
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void  CPU_INT_Enable(void)
{ 
     //register uint32_t __regPriMask         __ASM("primask");
     // __regPriMask = (0);
    __nds32__setgie_en();
}
   
// *****************************************************************************
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void  U32MemCopy(U32 *pDest, U32 *pSour, U32 dLen)
{
    U32 i=0;
    while(i < dLen){
        pDest[i]=pSour[i];
        i++;
    }
}

// *****************************************************************************
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void  U16MemCopy(U16 *pwDest, U16 *pwSour, U32 dLen)
{
    while(dLen--)
    {
        *pwDest = *pwSour++;
        pwDest++;
    }

}

void  U16MemPlus(U16 *pwDest, U16 wCnt, U16 wValue)
{
    while(wCnt--)
    {
        *pwDest = *pwDest + wValue;
        pwDest++;
    }

}

// *****************************************************************************
// 函数功能:
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void  U32MemSet(U32 *pDest,  U32 dValue, U32 dLen)
{
    U32 i=0;
    while(i < dLen){
        pDest[i]=dValue;
        i++;
    }
}

// *****************************************************************************
// 函数功能:
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void  U08MemSet(U08 *pDest,  U08 bValue, U32 dLen)
{
    U32 i=0;
    while(i < dLen){
        pDest[i]=bValue;
        i++;
    }
}
void  U16MemSet(U16 *pDest,  U16 bValue, U32 dLen)
{
    U32 i=0;
    while(i < dLen){
        pDest[i]=bValue;
        i++;
    }
}

//------------------------------------------------------------------------------
void U08MemCopy(U08 *pDest, U08 *pSour, U32 dLen)
{
    U32 i=0;
    while(i < dLen){
        pDest[i]=pSour[i];
        i++;
    }
//    if (pDest <= pSour || pDest >= (pSour + dLen))
//    {
//        while (dLen--)
//        {
//            *pDest = *pSour;
//            pDest++;
//            pSour++;
//        }
//    }
//    else
//    {
//        pDest = pDest + dLen - 1;
//        pSour = pSour + dLen - 1;
//
//        while (dLen--)
//        {
//            *pDest = *pSour;
//            pDest++;
//            pSour++;
//        }
//    }
}


U08 U08MemCmp(U08 *pDest, U08 *pSour, U32 dLen)
{
    U32 i;
    for(i=0;i<dLen;i++)
    {
        if(pDest[i] != pSour[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}

U08 U32MemCmp(U32 *pDest, U32 *pSour, U32 dLen)
{
    U32 i;
    for(i=0;i<dLen;i++)
    {
        if(pDest[i] != pSour[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}
// *****************************************************************************
// 函数名称: CheckSum()
// 函数功能: 指定区域的所有数据的check sum
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
U8  CheckSum(U8 *pAddr, U32 wLen)
{
    U8 bRet=0;
    while(wLen--)
    {
        bRet ^= pAddr[wLen];
    }
    return bRet;
}
// *****************************************************************************
// 函数名称: GetSum()
// 函数功能: 指定区域的所有数据的 sum
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
U8  GetSum(U8 *pAddr, U32 wLen)
{
    U8 bRet=0;
    while(wLen--)
    {
        bRet += pAddr[wLen];
    }
    return bRet;
}

// *****************************************************************************
// 函数名称: Check32Sum()
// 函数功能: 以DW为单位计算CHECK SUM
// 入口参数:pdSource 数据源
//          wCnt 数据长度
// 出口参数: 
// 说    明: 
// *****************************************************************************
U32 Check32Sum(U32 * pdSource, U16 wCnt)
{
    U32 dCheckNum;
    U16 wCheckIdx;
    dCheckNum = pdSource[0];
    for (wCheckIdx=1; wCheckIdx<wCnt; wCheckIdx++)
    {
        dCheckNum = pdSource[wCheckIdx]^dCheckNum;
    }
    return dCheckNum;
} 
