// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  sg9081.h
//  文件标识：  
//  内容摘要：  sg9081外设资源定义，所有寄存器资源定义
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
#ifndef _SG9081_H_
#define _SG9081_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/

#include <nds32_intrinsic.h>
#include "ae210p.h"
#include "regCPM.h"
#include "regADMCOM.h"
#include "regDMA1.h"
#include "regDMA2.h"
#include "regFC.h"
#include "regMCU.h"
#include "regPadctrl.h"
#include "regSPI.h"
#include "regUART.h"
#include "regSC.h"
#include "regPMCP.h"
#include "regLDPC.h"
#include "system_sg9081.h"
#include "mcu.h"
/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
static    const    U32        SG_DMA2_BASE_ARRY[4]={
    SG_DMA2_BASE + 0x200*0 ,
    SG_DMA2_BASE + 0x200*1 ,
    SG_DMA2_BASE + 0x200*2 ,
    SG_DMA2_BASE + 0x200*3 ,
};
static    const    U32        SG_UART_BASE_ARRY[2] =  {
    SG_UART_BASE + 0x1000*0,
    SG_UART_BASE + 0x1000*1,
    //SG_UART_BASE + 0x1000*2
};        
static    const    U32        SG_FC_BASE_ARRY[4] ={
    SG_FC_BASE + 0x1000*0 ,
    SG_FC_BASE + 0x1000*1 ,
    SG_FC_BASE + 0x1000*2 ,
    SG_FC_BASE + 0x1000*3 ,
};
static    const    U32         SG_PMCP_BASE_ARRY[2]=
{
		SG_PMCP_BASE + 0x1000*0,
		SG_PMCP_BASE + 0x1000*1
};

#define SG_CPM               ((SG_CPM_TypeDef *) SG_CPM_BASE)
#define SG_MCU               ((SG_MCU_TypeDef  *) SG_MCU_BASE )
#define SG_DIU               ((SG_DIU_TypeDef  *) SG_DIU_BASE )

#define SG_SPI               ((SG_SPI_TypeDef   *) SG_SPI_BASE )
#define SG_PADCTRL           ((SG_PADCTRL_TypeDef   *) SG_PADCTRL_BASE  )
#define SG_DSATA             ((SG_SC_TypeDef  *) SG_SC_BASE )
#define SG_ADM               ((SG_ADMCOM_TypeDef  *) SG_ADMCOM_BASE )
#define SG_DMA1              ((SG_DMA1_TypeDef  *) SG_DMA1_BASE )
#define SG_LDPC              ((SG_LDPC_TypeDef  *) SG_LDPC_BASE)
//#define SG_BFC				 ((SG_BFC_TypeDef  *) SG_BFC_BASE)

#define SG_UART(x)           ((SG_UART_TypeDef   *) (SG_UART_BASE_ARRY[x]))
#define SG_DMA2_CH(x)        ((SG_DMA2_TypeDef  *) (SG_DMA2_BASE_ARRY[x]))
#define SG_FC(x)             ((SG_FC_TypeDef  *) (SG_FC_BASE_ARRY[x])) 
#define SG_PMCP(x)           ((SG_PMCP_TypeDef  *)(SG_PMCP_BASE_ARRY[x]))
//#define SG_VMC(x)           ((SG_VMC_TypeDef  *)(SG_VMC_ARRY[x]))

#ifdef __cplusplus
}
#endif
#endif/*end of _SG9081_H_*/
