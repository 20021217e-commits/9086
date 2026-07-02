// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_dma_in.c
//  文件标识：  
//  内容摘要：  实现sata dma in 传输状态机
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月17日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "sata_dma_in.h"
#include "dsata.h"
#include "sata_misc.h"
#include "debug.h"
#include "task_manage.h"
#include "dma1c.h"
#include "scsi.h"
// *****************************************************************************
// 函数名称: ddmai2_send_status
// 函数功能: 子过程
// 入口参数: D2H相关字段
// 出口参数: 
// 说    明: DDMAI->Device DMA Out
// *****************************************************************************
static inline void ddmai2_send_status(U8 bStatus,U8 bError,U8 I)
{
    SATA_SendStatus(bStatus,bError,I); 
}

// *****************************************************************************
// 函数名称: ddmai1_send_data
// 函数功能: 子过程
// 入口参数: ptParam：REQ参数
// 出口参数: 
// 说    明: DDMAI->Device DMA Out
// *****************************************************************************
static inline void ddmai1_send_data(T_HI_TASK *ptTask)
{
    SG_DSATA->rERR_CLR.bits.DFISPATH_SEL = 1;//临时关闭DMA1通道
    //准备好PRD
    DSATA_BuildPrd(ptTask,0);
    //ATA层发起的数据传输，直接启动PRD传输
    DSATA_TransferStart(CMD_SLOT0);
    //DMAC_DMA1Start(DIR_IN,ptTask->tParam.dBuffAddr,ptTask->tParam.dSctSize,ptTask->tParam.dSctSize,0);
    //DMAC_DMA1WaitDone();
    //等待数据传输完成
    tSataDC.unDevTransSts.eDDMAI=DDMAI1_SEND_DATA;
}

// *****************************************************************************
// 函数名称: SATA_DDMAI_DataTransferReq
// 函数功能: DMA IN模式下 数据准备好或者命令出错的事件请求接口
// 入口参数: eType:请求类型

// 出口参数: NONE
// 说    明:  DDMAI->Device DMA in
// *****************************************************************************
void SATA_DDMAI_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t *pfTransferCompleteCb)
{
    tSataDC.pfTransferCompleteCb=pfTransferCompleteCb;//注册数据收发结束事件处理
    if(REQ_STATUS == eType)
    {
        //DRQ data block ready to transfer or error
        if(ptTask->eStatus < CMD_IDLE)
        {
            //小于IDLE的是出错状态
            /*2. Error encountered during command processing.*/
            //tSataDC.unDevTransSts.eDPIOI=DPIOI3_ERROR_STATUS;
#if (SATA_LOG_ENABLE == 1)
            SATA_SaveErrorLog(0x51,0x04|(U8)(1<<ptTask->eStatus),1);
#else
            ddmai2_send_status(0x51,0x04|(U8)(1<<ptTask->eStatus),1);
#endif
        }
        else
        {
            //正常结束状态已经在最后一次PRD传输完成中断中就发送了 --只处理FTL层传来的出错事件
        }
    }
    else 
    {  
        DSATA_EnableAutoSend();
        SG_DSATA->rPIER.bits.FTXDE=1;//ENABLE FTXDS
        DSATA_BuildD2HFis(0x50,0,1);//事先准备好D2H FIS
        if(REQ_CONFIG == eType)
        { 
#ifndef _MAX_PRD_DCB_EN_            
            DSATA_BuildPrd(ptTask,0);//准备好PRD
#else
            DSATA_BuildPrd(ptTask,1);//准备好PRD
#endif

#ifndef __BOOT_CODE__
            /*read dma*/                        /*write dma*/            /*read dma ext*/        /*write dma ext*/
            if( ((0xC8 == tSataDC.bATACmd)||(0xCA == tSataDC.bATACmd))||((0x25 == tSataDC.bATACmd)||(0x35 == tSataDC.bATACmd)))
            {
            	//非私有命令下数据写读由后端自己启动PDTR
            	if((SCSI_MODE  == tScsiCtl.bMode)&& (0x55AA == ptTask->tParam.dLba))
            	{
            		DSATA_TransferStart(CMD_SLOT0); //scsi需要额外手动启动
            	}
            }
            else if( OP_HI_TRIM == ptTask->bOpcode)
            {

            }
            else
#endif
            {
//#if(CMD_REORDER_EN == FALSE)
            //ATA层发起的数据传输，直接启动PRD传输
            	DSATA_TransferStart(CMD_SLOT0);//默认使用reorder功能，如果不是，scsi需要额外手动启动
//#endif
            }


            tSataDC.unDevTransSts.eDDMAI=DDMAI1_SEND_DATA; //等待数据传输完成
        }
        else if(REQ_TRANSFER == eType)
        {
            ddmai1_send_data(ptTask);
        }
    }
}

// *****************************************************************************
// 函数名称: ddmai0_dma_in
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DDMAI->Device DMA Out
// *****************************************************************************
void ddmai0_dma_in(void)
{
}

// *****************************************************************************
// 函数名称: SATA_DDMAI_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的DMA data IN command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DDMAI->Device DMA Out
// *****************************************************************************
void SATA_DDMAI_StateMachine(void)
{
    switch(tSataDC.unDevTransSts.eDDMAI)
    {
    case DDMAI0_DMA_IN:
        //ddmai0_dma_in();
        break;
    case DDMAI1_SEND_DATA:
        //ddmai1_send_data();//等PRD数据传输的中断-DPS
        break;
    case DDMAI2_SEND_STATUS:
        //ddmai2_send_status();
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DDMAI_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的DMA data IN command protocol 中断处理
// 入口参数: 
// 出口参数: 
// 说    明: DFPDMAQ->Device FPDMA Queued，此函数工作在中断模式
// *****************************************************************************
void SATA_DDMAI_IntEventHandler(T_PISR *ptPISR)
{
#if 0
    if(ptPISR->bits.PCS)
    {
        //PCS: Port COMRESET Status
    }

    if(ptPISR->bits.IFS)
    {
        //IFS: Interface Fatal Error Status
        //截获处理的话要自己清除中断标记,退出去以后原始的就不再执行了
    }
    if(ptPISR->bits.LSCS)
    {
        //LSCS: Link State Change Status

    }
    if(ptPISR->bits.INFS)
    {
        //INFS: Interface Non-fatal Error Status
    }
    if(ptPISR->bits.HDRS)
    {
        //HDRS: Host to Device Register FIS Interrupt
    }
    if(ptPISR->bits.UFS)
    {
        //UFS: Unknown FIS Interrupt

    }
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt
        //该模式下出现这个中断表示自动发送的D2H已经被HOST接收（R_OK收到）
        ptPISR->bits.FTXDS=0;
        
    }
    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed
        U8 bSlot=DSATA_GetCurrentCmdSlot();
         //SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态
        DSATA_Update_rPCMDR(13,1);
        ptPISR->bits.DPS=0;
        TQM_RemoveTask(bSlot);
        SATA_FTXDS_Handler();
    }
#endif
    
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt
        //该模式下出现这个中断表示自动发送的D2H已经被HOST接收（R_OK收到）
       // PRINT("DMAIN MODE FTXDS: FIS Transmit Done Interrupt\r\n");
        ptPISR->bits.FTXDS=0;
        TQM_RemoveTask(CMD_SLOT0);
        SATA_FTXDS_Handler();

    }
#if 0


    if(ptPISR->bits.DMATS)
    {
        //DMATS: DMAT Status

    }
    if(ptPISR->bits.TXOK)
    {
        //TXOK: Transmit Okay

    }
    if(ptPISR->bits.OFS)
    {
        //OFS: Overflow Status

    } 
    if(ptPISR->bits.MBES)
    {
        //MBES: Master Bus Error Status

    }
#endif
}
