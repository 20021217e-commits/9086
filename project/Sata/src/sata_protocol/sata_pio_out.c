// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_pio_out.c
//  文件标识：  
//  内容摘要：  实现sata pio out传输状态机 
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
#include "sata_pio_out.h"
#include "sata_misc.h"
#include "dsata.h"
#include "ata_identify.h"
#include "dma1c.h"
#include "debug.h"

// *****************************************************************************
// 函数名称: dpioo1_send_status
// 函数功能: 子过程
// 入口参数: D2H相关字段
// 出口参数: 
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
static inline void dpioo3_send_status(U8 bStatus,U8 bError,U8 I)
{
    //TODO:保存LOG？调用SMART模块接口。-未定义
    SATA_SendStatus(bStatus,bError,I);
}

// *****************************************************************************
// 函数名称: dpioo2_receive_data
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
void dpioo2_receive_data(T_HI_TASK *ptTask)
{
    SG_DSATA->rERR_CLR.bits.DFISPATH_SEL = 1;//临时关闭DMA1通道
    //ATA层发起的数据传输，分解单次任务
    //准备好PRD
    DSATA_BuildPrd(ptTask,1);
    //准备好PIO SETUP
    DSATA_BulidPioSetUpFis(DIR_OUT,(U8)ptTask->tParam.dSctSize);

    //ATA层发起的数据传输，直接启动PRD传输，pio setup也会一并发送
    DSATA_TransferStart(CMD_SLOT0);

    //DMAC_DMA1Start(DIR_OUT,ptTask->tParam.dBuffAddr,ptTask->tParam.dSctSize,ptTask->tParam.dSctSize,0);
    //DMAC_DMA1WaitDone();
    //等待单次PRD接收完成
    tSataDC.unDevTransSts.eDPIOO=DPIOO2_RECEIVE_DATA;
}

// *****************************************************************************
// 函数名称: dpioo1_send_pio_setup
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
void dpioo1_send_pio_setup(void)
{
}

// *****************************************************************************
// 函数名称: dpioo0_send_pio_setup
// 函数功能: 子过程
// 入口参数: none
// 出口参数: 
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
inline void dpioo0_pio_out(void)
{

}
// *****************************************************************************
// 函数名称: SATA_DPIOO_DataTransferReq
// 函数功能: PIO OUT模式下 FTL数据缓冲区准备好或者命令出错的事件处理
// 入口参数: eType:请求类型

// 出口参数: NONE
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
void SATA_DPIOO_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t *pfTransferCompleteCb)
{ 
    tSataDC.pfTransferCompleteCb=pfTransferCompleteCb;//注册数据收发结束事件处理
    TEST_PRINT(" Dpo_Dos = %d, st=%x \r\n",eType,ptTask->eStatus);
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
            dpioo3_send_status(0x51,0x04|(U8)(1<<ptTask->eStatus),1);
#endif
        }
        else
        {
            //不经过FTL的HOST写操作正确状态回复，因为需要ATA层检查接收的数据有效性，所以状态发送需要滞后
            dpioo3_send_status(0x50,0,1);
        }
    }
    else     
    {
#if(SATA_PIO_NEW  == FALSE)
        if(REQ_CONFIG == eType)
        {

            DSATA_EnableAutoSend();
            SG_DSATA->rPIER.bits.FTXDE=1;//ENABLE FTXDS
            //FTL请求的配置准备
            DSATA_BuildPrd(ptTask,1);
            //准备好PIO SETUP
            DSATA_BulidPioSetUpFis(DIR_OUT,(U8)ptTask->tParam.dSctSize);

//#if(CMD_REORDER_EN == FALSE)
            //ATA层发起的数据传输，直接启动PRD传输
#ifdef __BOOT_CODE__
            DSATA_TransferStart(CMD_SLOT0);//默认使用reorder功能，如果不是，需要额外手动启动
#endif
//#endif
            //等待数据传输结束 --不启动，启动权在FTL
            tSataDC.unDevTransSts.eDPIOO=DPIOO2_RECEIVE_DATA;
        }
        else
#endif
        if(REQ_TRANSFER == eType)
        {
            DSATA_DisableAutoSend();
            //PRINT_HEX("PO drq=",tSataDC.bDrq);
            //配置PRD并启动传输，传入的数据量要小于等于1个DRQ。由ATA层分解任务  --目前还没有发现有大于1个DRQ（512字节）的ATA命令
            dpioo2_receive_data(ptTask);
        }

    }
}
// *****************************************************************************
// 函数名称: SATA_DPIOO_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的pio data OUT command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DPIOO->Device pio Out
// *****************************************************************************
void SATA_DPIOO_StateMachine(void)
{

    switch(tSataDC.unDevTransSts.eDPIOO)
    {
    case DPIOO0_PIO_OUT:
        //dpioo0_pio_out();
        break;
    case DPIOO1_SEND_PIO_SETUP:
        //dpioo1_send_pio_setup();//硬件会在传输数据的时候自动发送PIO SETUP，所以本阶段是瞬态的
        break;    
    case DPIOO2_RECEIVE_DATA:
        //dpioo2_receive_data();//该过程和PIO SETUP 合并了
        break;
    case DPIOO3_SEND_STATUS:
        //dpioo3_send_status();//为了加速该操作也是瞬态的
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DPIOO_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的pio data OUT command protocol 中断处理
// 入口参数: 
// 出口参数: 
// 说    明: DPIOO->Device pio Out，此函数工作在中断模式
// *****************************************************************************
void SATA_DPIOO_IntEventHandler(T_PISR *ptPISR)
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
#endif
    TEST_PRINT(" PioO_EH \r\n");
#if(SATA_PIO_NEW  == FALSE)
    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed
        ptPISR->bits.DPS=0;
        //The PDMA suspends any activity until firmware clears this bit by writing 1 to it. 
        //通知ATA层单次PRD事务结束，1.ATA层需要进行数据校验,2.PIO任务分解
        if((DSATA_GetPrdRemainItems() == 0))
        {
#ifdef	SC_EXBUFF_EN
            SG_DSATA->rCLR_EXBUF.bits.dVAL = 1;
            SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 1;
#endif

            DSATA_BuildD2HFis(0x50,0,1);//事先准备好D2H FIS。PIO out最后一阶段数据是D2H，虽然是自动发送但是要事先准备好
            //可以被上层改写
        }
        SATA_PrdDoneEventHandler(CMD_SLOT0);
        DSATA_Update_rPCMDR(13,1);//SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态
        PRINT_TXT("PO DPP");
    } 
    

    if(ptPISR->bits.FTXDS)
    {
        ptPISR->bits.FTXDS=0;
        //数据传输结束切换状态
        SATA_UpdateRdIdx(1);
        SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
        DSATA_DisableAutoSend();
        tSataDC.pfTransferCompleteCb=NULL;
        PRINT_TXT("PO END");
        //因为FTXDS 肯定是发生在最后一个PRD数据传输之后，这么实现是因为暂时没发现不经过FTL的PIO OUT命令会超过1个DRQ（默认512字节）大小。
    }
#else
    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed
        ptPISR->bits.DPS=0;
        //The PDMA suspends any activity until firmware clears this bit by writing 1 to it.
        //通知ATA层单次PRD事务结束，1.ATA层需要进行数据校验,
        SATA_PrdDoneEventHandler(CMD_SLOT0);
        DSATA_Update_rPCMDR(13,1);//SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态
        PRINT_TXT("PO DPP");
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
