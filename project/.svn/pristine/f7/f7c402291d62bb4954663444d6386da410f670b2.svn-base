// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_dma_out.c
//  文件标识：  
//  内容摘要：  实现sata dma out 传输状态机 
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
#include "sg9081.h"
#include "sata_dma_out.h"
#include "dsata.h"
#include "sata_misc.h"
#include "debug.h"
#include "dma1c.h"
#include "scsi.h"

// *****************************************************************************
// 函数名称: ddmao3_send_status
// 函数功能: 子过程
// 入口参数: D2H相关字段
// 出口参数: 
// 说    明: DDMAO->Device DMA Out
// *****************************************************************************
static inline void ddmao3_send_status(U8 bStatus,U8 bError,U8 I)
{
    SATA_SendStatus(bStatus,bError,I);
}

// *****************************************************************************
// 函数名称: ddmao2_receive_data
// 函数功能: 子过程
// 入口参数: ptParam：REQ参数
// 出口参数: 
// 说    明: DDMAO->Device DMA Out
// *****************************************************************************
static inline void ddmao2_receive_data(T_HI_TASK *ptTask)
{ 
    SG_DSATA->rERR_CLR.bits.DFISPATH_SEL = 1;//临时关闭DMA1通道
    DSATA_BuildPrd(ptTask,1);
    //ATA层发起的数据传输，直接启动PRD传输
    DSATA_TransferStart(CMD_SLOT0);

    //DMAC_DMA1Start(DIR_OUT,ptTask->tParam.dBuffAddr,ptTask->tParam.dSctSize,ptTask->tParam.dSctSize,0);
    //DMAC_DMA1WaitDone();
    //等待数据接收完成中断
    tSataDC.unDevTransSts.eDDMAO=DDMAO2_RECEIVE_DATA;
}
// *****************************************************************************
// 函数名称: ddmao1_dma_active
// 函数功能: 子过程
// 入口参数: ptParam：REQ参数
// 出口参数: 
// 说    明: DDMAO->Device DMA Out
// *****************************************************************************
static inline void ddmao1_dma_active(T_HI_TASK *ptTask)
{
    //硬件自动发送DMA ACTIVE
    ddmao2_receive_data(ptTask);
}

// *****************************************************************************
// 函数名称: SATA_DDMAO_DataTransferReq
// 函数功能: DMA OUT模式下 FTL数据缓冲区准备好或者命令出错的事件处理
// 入口参数: eType:请求类型

// 出口参数: NONE
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
void SATA_DDMAO_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t *pfTransferCompleteCb)
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
            ddmao3_send_status(0x51,0x04|(U8)(1<<ptTask->eStatus),1);
#endif
        }
        else
        {
            //不经过FTL的HOST写操作正确状态回复，因为需要ATA层检查接收的数据有效性，所以状态发送需要滞后
            ddmao3_send_status(0x50,0,1);
        }
    }
    else
    {
        if(REQ_CONFIG == eType)
        {
            DSATA_EnableAutoSend();
            SG_DSATA->rPIER.bits.FTXDE=1;//ENABLE FTXDS
            DSATA_BuildPrd(ptTask,0);//准备好PRD
#ifndef __BOOT_CODE__
            if(ptTask->bOpcode == OP_HI_TRIM)
            {
                DSATA_DisableAutoSend();
                SG_DSATA->rPIER.bits.FTXDE=0;//DISABLE FTXDS
            }
#endif
            DSATA_BuildD2HFis(0x50,0,1);//事先准备好D2H FIS
#ifndef __BOOT_CODE__
                 /*read dma*/                        /*write dma*/            /*read dma ext*/        /*write dma ext*/
            if( ((0xC8 == tSataDC.bATACmd)||(0xCA == tSataDC.bATACmd))||((0x25 == tSataDC.bATACmd)||(0x35 == tSataDC.bATACmd)))
			 {
            	//非私有命令下数据写读由后端自己启动PDTR
            	if((SCSI_MODE == tScsiCtl.bMode)&& (0x55AA == ptTask->tParam.dLba))
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

            tSataDC.unDevTransSts.eDDMAO=DDMAO2_RECEIVE_DATA;//等待数据传输完成
        }
        else if(REQ_TRANSFER == eType)
        {
            //记录事务发起者的数据接收完成处理接口
            //tSataDC.pfTransferCompleteCb=pReq->pfDataTransferCompleteCb;//命令发起处赋值
            ddmao1_dma_active(ptTask);
        }
    }

}

// *****************************************************************************
// 函数名称: ddmao0_pio_out
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DDMAO->Device DMA Out
// *****************************************************************************
void ddmao0_dma_out(void)
{

}

// *****************************************************************************
// 函数名称: SATA_DDMAO_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的DMA data OUT command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DDMAO->Device DMA Out
// *****************************************************************************
void SATA_DDMAO_StateMachine(void)
{
    switch(tSataDC.unDevTransSts.eDDMAO)
    {
    case DDMAO0_DMA_OUT:
        //ddmao0_dma_out();
        break;
    case DDMAO1_DMA_ACTIVE:
        //ddmao1_dma_active();
        break;
    case DDMAO2_RECEIVE_DATA:
        //ddmao2_receive_data();//等PRD数据传输的中断-DPS
        break;
    case DDMAO3_SEND_STATUS:
        //ddmao3_send_status();
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DDMAO_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的DMA data OUT command protocol 中断处理
// 入口参数: 
// 出口参数: 
// 说    明: DFPDMAQ->Device FPDMA Queued，此函数工作在中断模式
// *****************************************************************************
void SATA_DDMAO_IntEventHandler(T_PISR *ptPISR)
{
#if 0
    if(ptPISR->bits.PCS)
    {
        //PCS: Port COMRESET Status
    }

    if(ptPISR->bits.IFS)
    {
        //IFS: Interface Fatal Error Status

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

    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed
         //SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态
        DSATA_Update_rPCMDR(13,1);

        //通知ATA层单次PRD事务结束，ATA层需要进行数据校验
        SATA_PrdDoneEventHandler(CMD_SLOT0);
        SATA_FTXDS_Handler();
        ptPISR->bits.DPS=0;       
    }
#endif
    
    TEST_PRINT(" dmaO_EH \r\n");
    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed
        ptPISR->bits.DPS=0;
        if(!DSATA_IsAutoSendEnabled())
        {
            SATA_PrdDoneEventHandler(CMD_SLOT0);//回调可能存在的后续操作
            ERROR_PRINT("SATA_PrdDoneEventHandler TRIM\r\n");
            DSATA_Update_rPCMDR(13,1);
        }
    }

    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt
        //该模式下出现这个中断表示自动发送的D2H已经被HOST接收（R_OK收到）
        ptPISR->bits.FTXDS=0;
		SATA_PrdDoneEventHandler(CMD_SLOT0);
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

