// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_define.c
//  文件标识：  
//  内容摘要：  定义sata 传输协议相关的变量
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月20日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "sata_typedef.h"
#include "sata_misc.h"
#include "sata_pio_in.h"
#include "sata_pio_out.h"
#include "sata_dma_in.h"
#include "sata_dma_out.h"
#include "sata_fpdma.h"
#include "sata_dev_idle.h"

#pragma GCC optimize("O0")
__align(8) T_BUFFER tSyncBuffer  __section(".undocds");
__align(8) T_BUFFER tAsyncBuffer  __section(".combuffer");
T_SATA_DEV_CTX tSataDC __section(".satadc");
T_TASK_QUEUE_CTX  tTQC __section(".tqc");//任务交互队列

SATA_XXX_StateMachine* const tDevCmdSMTbl_C[]={
    SATA_DHR_StateMachine,//Device Hardware Reset
    SATA_DI_StateMachine,//Device Idle
    SATA_DSR_StateMachine,//Device Software Reset
    SATA_DEDD_StateMachine,//Device Execute Device Diagnostic
    0,//Device Device Reset
    SATA_DND_StateMachine,//Device Non-Data
    SATA_DPIOI_StateMachine,//Device PIO In
    SATA_DPIOO_StateMachine,//Device PIO Out
    SATA_DDMAI_StateMachine,//Device DMA In
    SATA_DDMAO_StateMachine,//Device DMA Out
    0,
    0,
    0,
#ifndef __FPDMA_DISABLE__
    SATA_DFPDMAQ_StateMachine,//Device FPDMA Queued
#else
    0,
#endif
    SATA_DND_StateMachine,//UNKNOWN YET
};

//Device command protocol interrupt event handle
SATA_XXX_IntEventHandler* const tDevCmdEHTbl_C[]={
    SATA_DHR_IntEventHandler,//Device Hardware Reset
    SATA_DI_IntEventHandler,//Device Idle
    SATA_DSR_IntEventHandler,//Device Software Reset
    SATA_DEDD_IntEventHandler,//Device Execute Device Diagnostic
    0,//Device Device Reset
    SATA_DND_IntEventHandler,//Device Non-Data
    SATA_DPIOI_IntEventHandler,//Device PIO In
    SATA_DPIOO_IntEventHandler,//Device PIO Out
    SATA_DDMAI_IntEventHandler,//Device DMA In
    SATA_DDMAO_IntEventHandler,//Device DMA Out
    0,
    0,
    0,
#ifndef __FPDMA_DISABLE__
    SATA_DFPDMAQ_IntEventHandler,//Device FPDMA Queued
#else
    0,
#endif
    SATA_DND_IntEventHandler,//UNKNOWN YET
};

//Device command protocol interrupt event handle
SATA_XXX_DataTransferReq* const tDevCmdDOSTbl_C[]={
    0,//Device Hardware Reset
    0,//Device Idle
    0,//Device Software Reset
    0,//Device Execute Device Diagnostic
    0,//Device Device Reset
    SATA_DND_DataTransferReq,//Device Non-Data
    SATA_DPIOI_DataTransferReq,//Device PIO In
    SATA_DPIOO_DataTransferReq,//Device PIO Out
    SATA_DDMAI_DataTransferReq,//Device DMA In
    SATA_DDMAO_DataTransferReq,//Device DMA Out
    0,
    0,
    0,
#ifndef __FPDMA_DISABLE__
    SATA_DFPDMAQ_DataTransferReq,//Device FPDMA Queued
#else
    0,
#endif
    SATA_DND_DataTransferReq,//UNKNOWN YET
};
