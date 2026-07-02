// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_fpdma.c
//  文件标识：  
//  内容摘要：  实现sata fpdma传输状态机 
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
#include "sata_fpdma.h"
#include "dsata.h"
#include "sata_misc.h"
#include "ata_cmd_list.h"
#include "sata_dev_idle.h"
#include "debug.h"
#include "dma1c.h"
#include "ata_identify.h"

#ifndef __FPDMA_DISABLE__
// *****************************************************************************
// 函数名称: BrokenHost_ClearBusy
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void BrokenHost_ClearBusy(CMD_STATUS status)
{
    /*
    Halt command processing and transmit Register Device to Host
    FIS with ERR bit in Status field set to one, Interrupt bit set to one,
    BSY bit cleared to zero, DRQ bit cleared to zero, and Error field =
    04h.
    */
    U8 error=0x04;
    ERROR_PRINT("BrokenHost_ClearBusy!!!\r\n");
    //FIS NUM 一定是0
    DSATA_ExceptionHandler(0);
/*
 * If the host requested an address outside of the range of user addressable addresses, then:
    a) the ID NOT FOUND bit (see 6.3.5) shall be set to one and the ABORT bit shall be cleared to zero; or
    b) the ID NOT FOUND bit shall be cleared to zero and the ABORT bit shall be set to one.
    If a user addressable address was not found (see 6.3.5), the ABORT bit shall be cleared to zero.
    If the INTERFACE CRC bit (see 6.3.7) is set to one, the ABORT bit shall be set to one
 * */
    if(status == CMD_ID_NOT_FOUND)
    {
        error=(1U<<CMD_ID_NOT_FOUND);
    }
    DSATA_BuildD2HFis(0x51,error,1);
#if (SATA_LOG_ENABLE == 1)    
    SATA_SaveErrorLog(0x51,error,1);
    
#else
    DSATA_Update_rPCMDR(16,1); 
#endif
    DSATA_EnableRxFis();//重新使能接收
    //SG_DSATA->rPISR.dAll=HDRS_BIT;  //虽然是异步任务但是退出中断前要清除中断?????
    tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ13_WaitforClear;
    //等待 read log ext 命令，由中断响应
}

// *****************************************************************************
// 函数名称: ClearInterfaceBsy
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline static void ClearInterfaceBsy(void)
{
    /*
    Transmit Register Device to Host FIS with BSY bit cleared to zero
    and DRQ bit cleared to zero and Interrupt bit cleared to zero to
    mark interface ready for the next command.
    */
    
    /* Firmware must not set
        trigger-send bits (TXREG, TXSDB or TXBISTA) together with TXSAS bit for data 
        commands.
    */
    DSATA_DisableAutoSend();
    DSATA_BuildD2HFis(0x40,0,0);
    DSATA_Update_rPCMDR(16,1); //SG_DSATA->rPCMDR.bits.TXREG = 1;
    DSATA_EnableRxFis();//重新使能接收
    //while(SG_DSATA->rPCMDR.bits.TXREG == 1);//等到D2H发送完成
    GPIO_DEBUG(0x45);
#if (SDB_AUTO_SEND==TRUE)
    DSATA_EnableAutoSend();
#endif
    SG_DSATA->rPIER.bits.FTXDE=0;//该模式下不需要响应该中断
}
// *****************************************************************************
// 函数名称: DataXmitRead
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataXmitRead(T_HI_TASK *ptTask)
{
}
// *****************************************************************************
// 函数名称: DataXmitWrite
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataXmitWrite(T_HI_TASK *ptTask)
{
}
// *****************************************************************************
// 函数名称: AddCommandToQueue
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void AddCommandToQueue(CMD_STATUS eStatus)
{
    //注意索引，新增加的命令可以通过写索引获取，读索引永远指向当前执行中的命令
    tSataDC.bIsNcqCmd=TRUE;//有效NCQ命令
    if(eStatus < CMD_IDLE)
    {
        /*2. Command malformed*/
        //tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ12_BrokenHost_ClearBusy;
        UART_PRINT("Error!!!\r\n");

#if (SATA_LOG_ENABLE == 1)
        tSataDC.tSyncEvent.bNQFlg=FALSE;
        tSataDC.tSyncEvent.Tag= (unFsmBuff.FIS.h2d.dw3.bits.bCount>>3);
#endif
        BrokenHost_ClearBusy(eStatus);//命令层

    }
    else
    {
        /*1. Device successfully en-queued the command*/
        ClearInterfaceBsy();
        tTQC.bIsNcqCmd=TRUE;
        DEBUG_PRINT("******************************************tSataDC.bIsNcqCmd=%d\r\n",tSataDC.bIsNcqCmd);
        //tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ2_ClearInterfaceBsy;
        
    }
}

// *****************************************************************************
// 函数名称: DataPhaseReadSetup
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataPhaseReadSetup(T_HI_TASK *ptTask)
{
}
// *****************************************************************************
// 函数名称: DataPhasePreWriteSetup
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataPhasePreWriteSetup(T_HI_TASK *ptTask)
{
    /* DMA Setup FIS Auto-Activate option supported and enabled*/
}
// *****************************************************************************
// 函数名称: DataPhase_WriteSetup
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataPhase_WriteSetup(void)
{
}
// *****************************************************************************
// 函数名称: DataPhase_OldWriteSetup
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataPhase_OldWriteSetup(void)
{
}
// *****************************************************************************
// 函数名称: DataPhase_XmitActivate
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void DataPhase_XmitActivate(void)
{
}
// *****************************************************************************
// 函数名称: fpdmaq_error
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void fpdmaq_error(void)
{
    U32 dSActive=0;
    /*
                Halt command processing and transmit Set Device Bits FIS to host
                with ERR bit in Status field set to one, Interrupt bit set to one, ATA
                error code set to one in Error field, and bits in ACT field cleared to
                zero for any outstanding queued commands and bits set to one for
                any successfully completed queued commands that completion
                notification not yet delivered
     * */
#if (SDB_AUTO_SEND == TRUE)
    //开启AUTO功能没有已经完成却没有回复状态的命令,如果关闭AUTO功能，需要记录哪些命令已经完成
    //dSActive=0;
#else
    //利用reorder 机制就可以知道哪些任务已经顺利完成
    U8 I;
    for(I=0;I<SG_DSATA->rNCQ_DONE_TOTAL.dAll;I++)
    {
        dSActive|=(1<<SG_DSATA->rNCQ_DONE_REG[I]);//把DONE寄存器对应的BIT置位
    }
#endif
    DSATA_BulidSDBFis(0,1,tSataDC.tSyncEvent.bError,1,dSActive);
    //TODO:SAVE TO LOG
#if (SATA_LOG_ENABLE == 1) 
    GIE_DISABLE();
    if(tSataDC.bIsNcqCmd)
    {
        SATA_SaveErrorLog(0,tSataDC.tSyncEvent.bError,1);//保存之前需要配置好ERROR TYPE,NF 标记等
    }
    else
    {
        //这个时候已经来了COMRESET的话就不需要再创建任务了， 否则IDFY06 任务就会被清掉
        PRINT_TXT("NO-ERRLOG");
    }
    GIE_ENABLE();
#else
    //准备Set Device Bit FIS--ERROR state  --sactive 已经完成的置1，未完成的置0
    DSATA_TrigeSendFis(SDB_FIS);
    DSATA_EnableRxFis();//重新使能接收 
    ERROR_PRINT("fpdmaq_error,SEND SDB\r\n");//打印错误类型
#endif

    tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ13_WaitforClear;
    //等待 read log ext 命令，由中断响应
}

// *****************************************************************************
// 函数名称: WaitforClear
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void WaitforClear(void)
{
    //中断中处理该过程
}

// *****************************************************************************
// 函数名称: SendQueue_CleanACK
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void SendQueue_CleanACK(void)
{
    DSATA_DisableAutoSend();
    //准备Set Device Bit FIS
    DSATA_BulidSDBFis(4,0,0,0,0xFFFFFFFF);
    DSATA_TrigeSendFis(SDB_FIS);
    DSATA_EnableRxFis();//重新使能接收 
}

// *****************************************************************************
// 函数名称: SendQueue_CleanACKDMA
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
inline void SendQueue_CleanACKDMA(void)
{
    SendQueue_CleanACK();
}

// *****************************************************************************
// 函数名称: SATA_DFPDMAQ_DataTransferReq
// 函数功能: FPDMA模式下读数据事务处理
// 入口参数: eType:请求类型

// 出口参数: NONE
// 说    明:  DFPDMAQ->Device FPDMA Queued
// *****************************************************************************
void SATA_DFPDMAQ_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t *pfTransferCompleteCb)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    TEST_PRINT(" FPD_Dos = %d, st=%x \r\n",eType,ptTask->eStatus);
    if(REQ_STATUS == eType)
    {
        //DRQ data block ready to transfer or error
        if(ptTask->eStatus < CMD_IDLE)
        {
            //flash 读写数据失败
            //tSataDC.unDevTransSts.eDPIOI=BrokenHost_ClearBusy;
            BrokenHost_ClearBusy(ptTask->eStatus);
        }
        else
        {
            //最后一个PRD发送完成中断中切换掉了状态
        }
    }
    else if(REQ_CONFIG == eType)
    {
#if (SDB_AUTO_SEND==TRUE)
#if (D2H_AUTO_SEND == TRUE)
    //tSataDC.bLaTag=ptTask->tParam.bTag;
#endif
#ifdef __SDB_AUTO_ACC__
     DSATA_BuildPrd(ptTask,0);  //准备好PRD，保证最少有一次中断进行状态切换操作
#else
     DSATA_BuildPrd(ptTask,1);  //准备好PRD，保证最少有一次中断进行状态切换操作
#endif

#else
        //聚合发送SDB不需要产生DPS中断，如果同时启动的话只需最后一个PRD产生中断就可以
        DSATA_BuildPrd(ptTask,0);  //准备好PRD
#endif    
    }
    else if(REQ_TRANSFER == eType)
    {
        //不会有该分支
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: SATA_DFPDMAQ_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的FPDMA QUEUED command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DFPDMAQ->Device FPDMA Queued
// *****************************************************************************
void SATA_DFPDMAQ_StateMachine(void)
{
    switch(tSataDC.unDevTransSts.eDFPDMAQ)
    {
        case DFPDMAQ0_UnDefine:
            break;
        case DFPDMAQ1_AddCommandToQueue:
            //AddCommandToQueue();
            break;
        case DFPDMAQ2_ClearInterfaceBsy:
            //ClearInterfaceBsy();
            break;
        case DFPDMAQ3_DataPhaseReadSetup:
            //DataPhaseReadSetup();
            break;
        case DFPDMAQ4_DataPhasePreWriteSetup:
            //DataPhasePreWriteSetup();
            break;
        case DFPDMAQ5_DataPhase_WriteSetup:
            //DataPhase_WriteSetup();
            break;
        case DFPDMAQ6_DataPhase_OldWriteSetup:
            //DataPhase_OldWriteSetup();
            break;
        case DFPDMAQ7_DataPhase_XmitActivate:
            //DataPhase_XmitActivate();
            break;
        case DFPDMAQ8_DataXmitRead:
            //DataXmitRead();//等PRD数据传输的中断-DPS
            break;
        case DFPDMAQ9_DataXmitWrite:
            //DataXmitWrite();//等PRD数据传输的中断-DPS
            break;
        case DFPDMAQ10_SendStatus:
            //SendStatus();
            break;
        case DFPDMAQ11_ERROR:
            fpdmaq_error();
            break;
        case DFPDMAQ12_BrokenHost_ClearBusy:
            BrokenHost_ClearBusy(CMD_ABORT);
            break;
        case DFPDMAQ13_WaitforClear:
            //WaitforClear();
            break;
        case DFPDMAQ14_SendQueue_CleanACK:
            //SendQueue_CleanACK();
            break;
        case DFPDMAQ15_SendQueue_CleanACKDMA:
            //SendQueue_CleanACKDMA();
            break;
        default:
            //!!Error,不可能出现这种情况
            break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DFPDMAQ_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的FPDMA QUEUED command protocol 中断处理
// 入口参数: 
// 出口参数: 
// 说    明: DFPDMAQ->Device FPDMA Queued，此函数工作在中断模式
// *****************************************************************************
void SATA_DFPDMAQ_IntEventHandler(T_PISR *ptPISR)
{
    
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    TEST_PRINT(" fpd_EH \r\n");
    if(ptPISR->bits.IFS)
    {
        //IFS: Interface Fatal Error Status
        //截获处理的话要自己清除中断标记
        ptPISR->bits.IFS=0;
        ptPISR->bits.HDRS=0;
        PRINT_TXT("IFS:NCQ");
        SATA_Fatal_Error_Handler();
        TEST_PRINT(" ifs_fpd \r\n");
        //清除残留的中断标记--理应直接退出中断
        return;
        //等待 read log ext 命令，由中断响应
    }

    if(ptPISR->bits.REORDST)
    {
//#if (SDB_AUTO_SEND==FALSE)
        //将本次连续启动的最后一个TAG号的DPP置位
        //tPrdTbls[SG_DSATA->rNCQ_REORDER_REG[SG_DSATA->rNCQ_REORDER_TOTAL.dAll-1]].dw3.bits.I=1;
        //ERROR_PRINT("SG_DSATA->rNCQ_REORDER_REG[SG_DSATA->rNCQ_REORDER_TOTAL.dAll=%d]=%d\r\n",SG_DSATA->rNCQ_REORDER_TOTAL.dAll,SG_DSATA->rNCQ_REORDER_REG[SG_DSATA->rNCQ_REORDER_TOTAL.dAll-1]);
//#endif
    }

#ifndef __NCQ_QUEUE_NEW__
    //新方式不需要DPP中断
    NVIC_DisableIRQ(IRQ_SF1_INT0);
    if(ptPISR->bits.DPS)
    {
         //DPS 中断处理第一部分，提前把DPP清除掉，让SDB 的AUTO发送效率更高
         U8 bSlot;
         DEBUG_PRINT("SG_DMA1->rINDEX=%#010lx\r\n",SG_DMA1->rINDEX);
         DEBUG_PRINT("SG_DMA1->rTOTAL_SECT=%#010lx\r\n",SG_DMA1->rTOTAL_SECT);
         DEBUG_PRINT("SG_DMA1->rSTATUS.bits.TRANS_DONE=%#010lx\r\n",SG_DMA1->rSTATUS.bits.TRANS_DONE);
         //DPS: Descriptor Processed
         if(DFPDMAQ13_WaitforClear == tSataDC.unDevTransSts.eDFPDMAQ)
         {
             //发生异常直接退出
             DSATA_Update_rPCMDR(13,1);//TODO：DPP需要手动清除？DMA ABORT 以后？
             return;
         }
         //多个SLOT同时启动的话可能存在CCS位提前被置换成下一个SLOT,所以在下一个SLOT启动之前就清除掉相关标记
         bSlot=DSATA_GetCurrentCmdSlot();
         GPIO_DEBUG(0x46);
         SG_DSATA->rPISR.dAll=FTXDS_BIT;//清除旧的标记
         DSATA_Update_rPCMDR(13,1);
#if (SDB_AUTO_SEND == TRUE)
         {
                 //自动发送SDB
                 DEBUG_PRINT("bSlot=%d\r\n",bSlot);
                 TQM_RemoveTask(bSlot);
                 SATA_UpdateRdIdx(1);
         }
#else
         //SEND SDB
         unFsmBuff.FIS.sdb.dw0.dAll=0x004040A1;
         unFsmBuff.FIS.sdb.dw1.dAll=SG_MCU->rSF1_INT1_STS0;
         DSATA_Update_rPCMDR(4,1);//重新使能接收-使能接收以后PDMA才会进行发送动作
         DSATA_Update_rPCMDR(17,1);//DSATA_TrigeSendFis(SDB_FIS);
         TQM_RemoveMulTask(SG_MCU->rSF1_INT1_STS0);
 #endif
    }
    NVIC_EnableIRQ(IRQ_SF1_INT0);
#endif

    if(ptPISR->bits.HDRS || SG_DSATA->rPISR.bits.HDRS)
    {
        //pfm_restart();
        ptPISR->bits.HDRS=0;
//        TEST_PRINT(" fpd_EH_HD \r\n");
        //HDRS: Host to Device Register FIS Interrupt
        //TODO:如果不在tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ13_WaitforClear状态呢？要不要考虑
        if(SATA_SReset_Handler() == FALSE)
        {
            T_ATA_CMD* pCmd = ATACmd_ParseCmd(unFsmBuff.FIS.h2d.dw0.bits.Cmd);

            if(pCmd != NULL)
            {
                /*
                 *
                 * DFPDMAQ13: WaitforClear Wait for host to either issue a command to read the Queued Error
                    Log or issue SRST
                    1. READ LOG EXT command with Queued Error Log

                    2. READ LOG DMA EXT command with Queued Error Log

                    3. SRST received → DSR0:

                    4. Any other command receivedW
                 * */

                if(DFPDMAQ13_WaitforClear == tSataDC.unDevTransSts.eDFPDMAQ)
                {
                    if(0x2F == unFsmBuff.FIS.h2d.dw0.bits.Cmd && unFsmBuff.FIS.h2d.dw1.bits.bLBA_low == 0x10)
                    {
                        //7.24 READ LOG EXT 2Fh, PIO Data-In
                        SATA_SaveH2DFis(pCmd->eCmdTrspType,&unFsmBuff);
                        SendQueue_CleanACK();
                        SATA_ChangeState(PIO_DATA_IN,DPIOI0_PIO_IN);
                        pCmd->pfCmdHandler();
                        SG_DSATA->rHWD2HEN.dAll=1;
                        tSataDC.bIsNcqCmd=FALSE;
                        ERROR_PRINT("READ LOG EXT\r\n");
                        PRINT_TXT("READ LOG EXT");
                        //tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ14_SendQueue_CleanACK;
                    }
                    else if(0x47 == unFsmBuff.FIS.h2d.dw0.bits.Cmd && unFsmBuff.FIS.h2d.dw1.bits.bLBA_low == 0x10)
                    {
                        //7.25 READ LOG DMA EXT 47h, DMA
                        SATA_SaveH2DFis(pCmd->eCmdTrspType,&unFsmBuff);
                        SendQueue_CleanACKDMA();
                        SATA_ChangeState(DMA_DATA_IN,DDMAI0_DMA_IN);
                        pCmd->pfCmdHandler();
                        SG_DSATA->rHWD2HEN.dAll=1;
                        tSataDC.bIsNcqCmd=FALSE;
                        ERROR_PRINT("READ LOG DMA EXT\r\n");
                        PRINT_TXT("READ LOG DMA EXT");
                        //tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ15_SendQueue_CleanACKDMA;
                    }
                    else
                    {
                        //tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ12_BrokenHost_ClearBusy;
                        PRINT_HEX("E:ClrBsy0>",unFsmBuff.FIS.h2d.dw0.bits.Cmd);
                        DSATA_EnableRxFis();//重新使能接收
                        DSATA_SendD2HFis(0x51,0x04,1);; //拒绝命令执行
                        ERROR_PRINT("DFPDMAQ12_BrokenHost_ClearBusy1.....\r\n");
                        return;
                    }
                }
                else
                {
                    if(0x60 == unFsmBuff.FIS.h2d.dw0.bits.Cmd || 0x61 == unFsmBuff.FIS.h2d.dw0.bits.Cmd)
                    {
                        //新任务
                        NVIC_DisableIRQ(IRQ_SF1_INT0);//涉及到 tSataDC.bCmdCnt 更新冲突
                        SATA_SaveH2DFis(pCmd->eCmdTrspType,&unFsmBuff);
                        NVIC_EnableIRQ(IRQ_SF1_INT0);
                        pCmd->pfCmdHandler();

                    }
                    else
                    {
#ifndef __BOOT_CODE__
                        if(SG_DSATA->rHWD2HDONE0 != 0)
                        {
                            //tSataDC.unDevTransSts.eDFPDMAQ=DFPDMAQ12_BrokenHost_ClearBusy;
                            #if (SATA_LOG_ENABLE == 1)
                                tSataDC.tSyncEvent.bNQFlg=TRUE;
                                tSataDC.tSyncEvent.Tag= DSATA_GetCurrentCmdSlot();
                            #endif
                            PRINT_HEX("E:ClrBsy1>",unFsmBuff.FIS.h2d.dw0.bits.Cmd);
                            BrokenHost_ClearBusy(CMD_ABORT);
                            ERROR_PRINT("DFPDMAQ12_BrokenHoSst_ClearBusy. save log data1....\r\n");
                        }
                        else
                        {
                            //NCQ命令结束后收到非NCQ命令继续处理
                        	SG_DSATA->rPISR.dAll=(HDRS_BIT) | (FTXDS_BIT);//清除H2D中断(NCQ 模式直接跳转非NCQ模式)，否则数据可能传不动
                            ptPISR->bits.HDRS=1;
                            ptPISR->bits.FTXDS=0;
                            tSataDC.bCmdCnt=0;
                            PRINT_TXT("NO-NCQ CMD");
                        }
#else
						BrokenHost_ClearBusy(CMD_ABORT);
#endif
                    }
                }
            }
            else
            {
                //未知指令--有可能是NCQ命令，查表失败的
#ifndef __BOOT_CODE__
                if(DFPDMAQ13_WaitforClear != tSataDC.unDevTransSts.eDFPDMAQ)
                {
                    if(SG_DSATA->rHWD2HDONE0 != 0)
                    {
						if(0x60 == unFsmBuff.FIS.h2d.dw0.bits.Cmd || 0x61 == unFsmBuff.FIS.h2d.dw0.bits.Cmd)
						{
							tSataDC.tSyncEvent.bNQFlg=!tSataDC.bIsNcqCmd;
							tSataDC.tSyncEvent.Tag=DSATA_GetCurrentCmdSlot();
							tSataDC.tSyncEvent.bError=0x04U;
						}
						else
						{
						#if (SATA_LOG_ENABLE == 1)
							tSataDC.tSyncEvent.bNQFlg=TRUE;
							tSataDC.tSyncEvent.Tag= DSATA_GetCurrentCmdSlot();
						#endif
						}
						PRINT_HEX("E:DC_cnt1>",tSataDC.bCmdCnt);
						PRINT_HEX("E:ClrBsy2>",unFsmBuff.FIS.h2d.dw0.bits.Cmd);
						BrokenHost_ClearBusy(CMD_ABORT);
						ERROR_PRINT("DFPDMAQ12_BrokenHost_ClearBusy. save log data2....\r\n");
                    }
                    else
                    {
                        //NCQ命令结束后收到非NCQ命令继续处理
                    	SG_DSATA->rPISR.dAll=(HDRS_BIT) | (FTXDS_BIT);//清除H2D中断(NCQ 模式直接跳转非NCQ模式)，否则数据可能传不动
                        ptPISR->bits.HDRS=1;
                        ptPISR->bits.FTXDS=0;
                        tSataDC.bCmdCnt=0;
                        PRINT_TXT("NO-NCQ CMD");
                    }
                }
                else
                {
                    //已经进入NCQ error 流程直接回复D2H
                    DSATA_EnableRxFis();//重新使能接收
                    PRINT_HEX("E:ClrBsy3>",unFsmBuff.FIS.h2d.dw0.bits.Cmd);
                    DSATA_SendD2HFis(0x51,0x04,1); //拒绝命令执行
                    ERROR_PRINT("DFPDMAQ12_BrokenHost_ClearBusy2.\r\n");
                    return;
                }
#else
				 BrokenHost_ClearBusy(CMD_ABORT);
#endif
            }
        }
        UART_PRINT("tSataDC.eTransType=%d\r\n",tSataDC.eTransType);
    }/*end of HDRS INT*/

    //if(ptPISR->bits.DPS)
    {
        //DPS 中断处理第二部分，以免有H2D中断一起发生，导致错误进入IDLE状态
        //ptPISR->bits.DPS=0;
        if(tSataDC.bCmdCnt == 0 &&\
           SG_DSATA->rHWD2HDONE0 == 0 &&\
           tSataDC.eTransType == FPDMA_QUEUED &&\
           tSataDC.unDevTransSts.eDFPDMAQ!=DFPDMAQ13_WaitforClear &&\
           tSataDC.unDevTransSts.eDFPDMAQ!=DFPDMAQ11_ERROR)//ERROR 是中间态，最终会成为WaitforClear
        {
            //最后一次reorder中断会触发这个流程
            GPIO_DEBUG(0x84);
            UART_PRINT("Req sending SDB...!!\r\n");
//            TEST_PRINT(" fpd_EH_ENT\r\n");
            __enable_performance();
            U32 Tstart=tSataDC.dPowerOnTime;
            while((tSataDC.dPowerOnTime - Tstart) <= 50)//等HOST可能下发的NCQ命令，就不切换到IDLE模式
            {
               if(SG_DSATA->rRECVTAG != 0)
               {
                   //有新的NCQ命令
                    return;
               }
               else if((SG_DSATA->rPCMDR.bits.FRXP == 1 || SG_DSATA->rPISR.bits.LSCS == 1)|| (SG_DSATA->rPISR.bits.IFS == 1)|| (SG_DSATA->rPISR.bits.INFS == 1)||(1 == ptPISR->bits.IFS)||(1 == ptPISR->bits.INFS))
               {
                   //NCQ 完成以后，有新命令，或者产生link change 中断需要及时退出去处理
                   break;
               }
            }

            tSataDC.bIsNcqCmd=FALSE;
            tTQC.bIsNcqCmd=FALSE;

            //PRINT_TXT("NCQ END");
#ifdef   H2D_DMASETUP_TEST
             SG_DSATA->rPISR.dAll=HDRS_BIT;//H2D中断会卡主DMA SETUP流程
	         SG_DSATA->rPIER.bits.HDRE |= HDRS_BIT;//H2D中断会卡主DMA SETUP流程
#endif //#ifndef   H2D_DMASETUP_TEST

            //DSATA_DisableAutoSend();
            SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
            GPIO_DEBUG(0x85);
        }
        //SATA端任务管理上下文更新了，更新FTL任务，以免两边不同步
        DEBUG_PRINT("DSATA_GetCurrentCmdSlot=%d\r\n",DSATA_GetCurrentCmdSlot());

    }
//    TEST_PRINT(" fpd_EH_END \r\n");
#if 0
    
    if(ptPISR->bits.TXOK)
    {
        //TXOK: Transmit Okay
        bSlot=DSATA_GetCurrentCmdSlot();
        ptPISR->bits.TXOK=0;
        GPIO_DEBUG(0x73);
        GPIO_DEBUGDW(SG_DSATA->rPCMDR.dAll); 
        GPIO_DEBUGDW(tPrdTbls[bSlot].dw3.dAll); 
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
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#endif //__FPDMA_DISABLE__
