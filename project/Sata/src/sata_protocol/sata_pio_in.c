// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_pio_in.c
//  文件标识：  
//  内容摘要：  实现sata pio in传输状态机 
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
#include "debug.h"
#include "sata_pio_in.h"
#include "dsata.h"
#include "sata_misc.h"
#include "ata_identify.h"
#include "dma1c.h"

// *****************************************************************************
// 函数名称: dpioi1_error_status
// 函数功能: 子过程
// 入口参数: D2H相关字段
// 出口参数: 
// 说    明: DPIOI->Device pio IN
// *****************************************************************************
static inline void dpioi3_error_status(U8 bStatus,U8 bError,U8 I)
{
    SATA_SendStatus(bStatus,bError,I);
}

// *****************************************************************************
// 函数名称: dpioi2_transmit_data
// 函数功能: 发送数据到HOST
// 入口参数:
// 出口参数: 
// 说    明: DPIOI->Device pio IN
// *****************************************************************************
void dpioi2_transmit_data(T_HI_TASK *ptTask)
{
    SG_DSATA->rERR_CLR.bits.DFISPATH_SEL= 1;//临时关闭DMA1通道

    //准备好PRD   
    DSATA_BuildPrd(ptTask,1);
    //准备好PIO SETUP
    DSATA_BulidPioSetUpFis(DIR_IN,(U8)ptTask->tParam.dSctSize);
    //ATA层发起的数据传输，直接启动PRD传输，pio setup也会一并发送
    DSATA_TransferStart(CMD_SLOT0);
    //DMAC_DMA1Start(DIR_IN,ptTask->tParam.dBuffAddr,ptTask->tParam.dSctSize,ptTask->tParam.dSctSize,0);
    //DMAC_DMA1WaitDone();
    //切换状态等待数据传输结束
    tSataDC.unDevTransSts.eDPIOI=DPIOI2_TRANSMIT_DATA;     
}

// *****************************************************************************
// 函数名称: dpioi1_send_pio_setup
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DPIOI->Device pio IN
// *****************************************************************************
void dpioi1_send_pio_setup(void)
{
    //根据已经发送的数据的大小调整偏移量
}

// *****************************************************************************
// 函数名称: dpioi0_pio_in
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DPIOI->Device pio IN
// *****************************************************************************
inline void dpioi0_pio_in(void)
{
}

// *****************************************************************************
// 函数名称: SATA_DPIOI_DataTransferReq
// 函数功能: PIO IN模式下 数据准备好或者命令出错的事件请求接口
// 入口参数: eType:请求类型

// 出口参数: NONE
// 说    明: DPIOI->Device pio IN
// *****************************************************************************
void SATA_DPIOI_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t pfTransferCompleteCb)
{
    tSataDC.pfTransferCompleteCb=pfTransferCompleteCb;//注册数据收发结束事件处理
	TEST_PRINT(" DPI_Dos = %d, st=%x \r\n",eType,ptTask->eStatus);
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
            dpioi3_error_status(0x51,0x04|(U8)(1<<ptTask->eStatus),1);
#endif
        }
        else
        {
           //正常结束状态已经在最后一次PRD传输完成中断中就发送了 --只处理FTL层传来的出错事件
        }

    }
    else
    {
#if(SATA_PIO_NEW  == FALSE)
        if(REQ_CONFIG == eType)
        {
        	//FTL请求的配置
            DSATA_BuildPrd(ptTask,1);
            //准备好PIO SETUP
            DSATA_BulidPioSetUpFis(DIR_IN,(U8)ptTask->tParam.dSctSize);

            //ATA层发起的数据传输，直接启动PRD传输
#ifdef __BOOT_CODE__
            DSATA_TransferStart(CMD_SLOT0);//默认使用reorder功能，如果不是，需要额外手动启动
#endif
            //等待数据传输结束 --不启动，启动权在FTL
            tSataDC.unDevTransSts.eDPIOI=DPIOI2_TRANSMIT_DATA;  
        }
        else
#endif
        if(REQ_TRANSFER == eType)
        {
            //PRINT_HEX("PI drq=",tSataDC.bDrq);
            //配置PRD并启动传输，传入的数据量要小于等于1个DRQ。由ATA层分解任务 --目前还没有发现有大于1个DRQ（512字节）的ATA命令
            dpioi2_transmit_data(ptTask);
        }
    }
}

// *****************************************************************************
// 函数名称: SATA_DPIOI_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的pio data IN command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DPIOI->Device pio IN
// *****************************************************************************
void SATA_DPIOI_StateMachine(void)
{
    switch(tSataDC.unDevTransSts.eDPIOI)
    {
    case DPIOI0_PIO_IN:
        //dpioi0_pio_in();//等待ATA命令处理事件，或者FTL数据准备好事件
        break;
    case DPIOI1_SEND_PIO_SETUP:
        //dpioi1_send_pio_setup()//硬件会在传输数据的时候自动发送PIO SETUP，所以本阶段是一个瞬态
        break;
    case DPIOI2_TRANSMIT_DATA:
        //dpioi2_transmit_data();//合并到SETUP中,这里等待数据传输结束中断，中断中切换状态
        break;
    case DPIOI3_ERROR_STATUS:
        //dpioi3_error_status();//为了加速该操作也是瞬态的
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DPIOI_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的pio data IN command protocol 中断处理
// 入口参数: 
// 出口参数: 
// 说    明: DPIOI->Device pio IN，此函数工作在中断模式
// *****************************************************************************
void SATA_DPIOI_IntEventHandler(T_PISR *ptPISR)
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
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt

    }
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt
        ptPISR->bits.FTXDS=0;
    }
#endif

#if(SATA_PIO_NEW  == TRUE)
    if(ptPISR->bits.DPS)
    {
        GPIO_DEBUG(0x43);
        //DPS: Descriptor Processed
        ptPISR->bits.DPS=0;
        SATA_PrdDoneEventHandler(CMD_SLOT0);

        if((DSATA_GetPrdRemainItems() == 0))
        {
        	//数据传输结束切换状态
            SATA_UpdateRdIdx(1);
            SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
            tSataDC.pfTransferCompleteCb=NULL;
            DSATA_Update_rPCMDR(13,1);
            tSataDC.tSyncEvent.eStatus=CMD_DONE;
            DEBUG_PRINT("DSATA_GetPrdRemainItems == 0\r\n");
            PRINT_TXT("PI DPP");
        }
        //暂时没发现不经过FTL的PIO IN命令会超过1个DRQ（默认512字节）大小。
        //否则需要回调ATA层命令分解任务，这样的话需要每个PRD都置位I标志产生中断
    }
#else
    TEST_PRINT(" pI_EH \r\n");
    if(ptPISR->bits.DPS)
        {
            GPIO_DEBUG(0x43);
            //DPS: Descriptor Processed
            ptPISR->bits.DPS=0;
            SATA_PrdDoneEventHandler(CMD_SLOT0);

            DSATA_Update_rPCMDR(13,1);//SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态

            if((DSATA_GetPrdRemainItems() == 0))
            {
                //数据传输结束切换状态
                SATA_UpdateRdIdx(1);
                SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
                tSataDC.pfTransferCompleteCb=NULL;
                DEBUG_PRINT("DSATA_GetPrdRemainItems == 0\r\n");
                PRINT_TXT("PI DPP");
            }
            //暂时没发现不经过FTL的PIO IN命令会超过1个DRQ（默认512字节）大小。
            //否则需要回调ATA层命令分解任务，这样的话需要每个PRD都置位I标志产生中断
        }
#endif

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
