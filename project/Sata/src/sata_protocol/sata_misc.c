// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_misc.c
//  文件标识：  
//  内容摘要：  miscellaneous :实现POWERON_COMRESET,IDLE,SRESET,NONE_DATA,传输状态机 
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
#include "common.h"
#include "sata_misc.h"
#include "ata_cmd_list.h"
#include "dsata.h"
#include "ata_identify.h"
#include "debug.h"
#include "task_manage.h"
#include "ata_smart.h"
#include "uitility.h"
#if (SATA_LOG_ENABLE == 1)
#include "ata_gpl.h"
#include "ata_security.h"
#endif
#include "dma1c.h"
#include "scsi.h"
#include "mcu.h"
#include "ata_fpdma.h"
#include "sata_fpdma.h"
#ifdef  __FW_TRIGGER_SATA_CRC__
#include "SATA_AbormalHandle.h"
#endif

// *****************************************************************************
// 函数名称: SATA_DevCtx_Init()
// 函数功能: 协议相关变量初始化
// 入口参数: NONE
// 出口参数: NONE
// 说    明: 
// *****************************************************************************    
void SATA_DevCtx_Init(U8 mode)
{
    int i;

    for(i=0;i<MAX_CMD_NUM;i++)
    {
        tPrdTbls[i].dDBAU=0;
        tPrdTbls[i].dReserved=0;
    }

    //SSP enable
    //FOR SSP
    if(FS_SSP == 1)
    {
        /*
         * See table 16 for a list of the preserved feature sets and settings. A Count field value of 06h is used to enable or
            disable software settings preservation. By default, if the device supports software settings preservation the
            feature is enabled when it processes a power-on reset. The enable/disable state for software settings
            preservation shall persist across software reset. The enable/disable state for software settings preservation shall
            be reset to its default state upon COMRESET. The host may disable software settings preservation in order to
            cause software settings to revert to their power-on default state when the device receives a COMRESET
         * */
        //保持IDENTIFY
    }
    else
    {
        //SSP 没有使能的话，发生comreset 以后identify 恢复成上电状态,此时上电状态的IDFY已经被保存在了code区
        Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
        //SetBits(unAtaIdentify.wArray+79,(U8)6,(U8)1,0);
        unAtaIdentify.wArray[79]&=~(W_BIT6);

        if(tSataDC.eSecState >= ATA_SEC3 && tSataDC.eSecState <= ATA_SEC6)
        {
            //SEC3/4/5/6回到 SEC4
            tSataDC.eSecState=ATA_SEC4;
        }
        else
        {
            tSataDC.eSecState=ATA_SEC1;
        }

        /*
         * If the device processes a power-on reset or a hardware reset, then the device shall:
            a) clear the SECURITY COUNT EXPIRED bit to zero; and
            b) set the password attempt counter to five.*/
        tSataDC.bSecPwdAttemptsCnt=5;
        tSataDC.bIsSecPwdAttemptsExceeded=FALSE;
        //SetBits(unAtaIdentify.wArray+128,(U8)4,(U8)1,0);
        unAtaIdentify.wArray[128]&=~(W_BIT4);
    }

    if(tSataDC.eSecState==ATA_SEC1)
    {
        tSataDC.bIsSecFreezeLocked=FALSE;
#if(SATA_LOG_ENABLE == 1)
        //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)0);
        //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
        //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
        //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)0);
        //SetBits(unAtaIdentify.wArray+128,(U8)8,(U8)1,(U8)0);
        unAtaIdentify.wArray[128]&=~(W_BIT1|W_BIT2|W_BIT3|W_BIT8);
        unAtaIdentify.wArray[85]&=~(W_BIT1);
        SG_DSATA->rHWD2HEN.dAll=1;
#endif
    }
    else if(tSataDC.eSecState==ATA_SEC4)
    {
#if(SATA_LOG_ENABLE == 1)
        //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U16)1);
        //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U16)1);
        //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U16)0);
        unAtaIdentify.wArray[128]|=(W_BIT1|W_BIT2);
        unAtaIdentify.wArray[128]&=~(W_BIT3);
        SG_DSATA->rHWD2HEN.dAll=0;//SEC4状态，要关闭NCQ 自动功能
#endif
    }

    if(tSataDC.tSDState == S_SD4 || tSataDC.tSDState == S_SD1)
    {
        tSataDC.bSDFMPVal=0;
        tSataDC.tSDState=S_SD0;
    }
    else if(tSataDC.tSDState == S_SD2)
    {
        SG_DSATA->rHWD2HEN.dAll=0;
    }

    /*A device shall not exit the DCO_Locked state as the result of processing a hardware reset or a software reset.*/

    tSataDC.bMultiDrq=1;
    tSataDC.bCmdCnt=0;
    tSataDC.IsOvLap=0;
    tSataDC.bWrIdx=0;
    tSataDC.bRdIdx=0;
    tSataDC.bPre4kCmdFlg = FALSE;
    tSataDC.bIsNcqCmd=FALSE;
    tSataDC.bIsDiagnosticOk=FALSE;
    tSataDC.bIsBGActivityOn=FALSE;
    tSataDC.pfTransferCompleteCb =NULL;
    tSataDC.pfTaskCompleteCb = NULL;
    tSataDC.bDrq=1;
    //tSataDC.dDevCap = DEVICE_CAPCITY;
    //tSataDC.dMaxLbaAddr4User=tSataDC.dDevCap;
    tSataDC.wNCQCmdIdx=1;
#ifndef __NCQ_QUEUE_NEW__
    tSataDC.qwTaskCnt=0;
#endif
    //soft reset/comreset
    if(DPS_SLEEP == tSataDC.eDPMSts)
    {
        /*
            Transition PM3:PM2:, A device shall transition to the PM2: Standby state after processing a hardware reset,
            software reset, or DEVICE RESET command.
        */
         tSataDC.eDPMSts=DPS_STANBY;
    }
    else
    {
        //其他模式都是要保持自身状态
    }
    /*
     * The enable/disable state for device initiated power management shall persist across software reset.
        The enable/disable state shall be reset to its default disabled state upon COMRESET
     * */
    SG_CPM->rPAWCTRL.bits.PAWEN =0;//禁用重新激活
    tSataDC.bIsDIPMEn=FALSE;
    SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;
    //SetBits(unAtaIdentify.wArray+79,(U8)3,(U8)1,0);//COMRESET clear DIPMEN
    unAtaIdentify.wArray[79]&=~(W_BIT3);
    /*
     * The enable/disable state for Device Automatic Partial to Slumber transitions shall persist
        across software reset. The enable/disable state for Automatic Partial to Slumber transitions shall be reset to its
        default state upon hardware reset
     * */
    SG_DSATA->rPCMDR.bits.APSTE=0;//disable auto p to s
    //SetBits(unAtaIdentify.wArray+79,(U8)7,(U8)1,(U16)0);
    unAtaIdentify.wArray[79]&=~(W_BIT7);
    /*
     * The enable/disable state for the auto-activate optimization shall be preserved across
        software reset. The enable/disable state for the auto-activate optimization shall be reset to its default state upon
        COMRESET.
     * */
    //DMA Setup Auto-Activate optimization
    //SetBits(unAtaIdentify.wArray+79,(U8)2,(U8)1,(U16)0);
    unAtaIdentify.wArray[79]&=~(W_BIT2);
#ifdef  SC_WP_4K_PRE
    SG_DSATA->rACTIVE_SEND.bits.ACTIVE_SEND = FALSE;			  // 预收4K 是否发生第一个DMA ACTIVE
#endif
#if (D2H_AUTO_SEND == TRUE)
    //comreset 会将寄存器复位需要重新赋值
    SG_DSATA->rTOTALCAPCITY = tSataDC.dDevCap;
    SG_DSATA->rMAXTAG.dAll=GetWord(75);
#endif

    tSataDC.bIsAP2SEn=TRUE;
    tSataDC.eLPMSts=LPS_OFF_LINE;
    tSataDC.DPC_Process=NULL;
    tSataDC.INT_Process=NULL;
    tSataDC.DOS_TransferReq=NULL;
    tSataDC.pfTaskCompleteCb = NULL;
    tSataDC.unDevTransSts.eDDR=0;
#if(SATA_LOG_ENABLE == 1)
    //SetBits(unAtaIdentify.wArray+128,(U8)8,1,tSataLogCtx.bMPCFlg);
    unAtaIdentify.wArray[128]&=(~(1<<8));
    unAtaIdentify.wArray[128]|=((tSataLogCtx.bMPCFlg)<<8);
    UpdateIDFYLogPage06(CMD_DONE,&tSataDC.tSyncEvent);
#endif
}

// *****************************************************************************
// 函数名称: SATA_ChangeState
// 函数功能: 切换状态通用接口,用于状态间切换状态
// 入口参数: type切换的传输协议类型，state切换到某一状态
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_ChangeState(ENUM_SATA_TRSP_TYPE eType,U8 bState)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    DEBUG_PRINT("EType=%u(%s),bState=%u\r\n",eType,TRSP_TYPE[eType],bState);
    
    tSataDC.eTransType=eType;
    tSataDC.unDevTransSts.bState=bState;
    tSataDC.DPC_Process=tDevCmdSMTbl_C[eType];
    tSataDC.INT_Process=tDevCmdEHTbl_C[eType];
    tSataDC.DOS_TransferReq=tDevCmdDOSTbl_C[eType];
    TEST_PRINT(" type = 0x%x ,State = %d\r\n",eType,bState);

    DEBUG_PRINT("tSataDC.DPC_Process=%#010lx\r\n",tSataDC.DPC_Process);
    DEBUG_PRINT("tSataDC.INT_Process=%#010lx\r\n",tSataDC.INT_Process);
    DEBUG_PRINT("tSataDC.DOS_TransferReq=%#010lx\r\n",tSataDC.DOS_TransferReq);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: SATA_ComReset_Handler()
// 函数功能: COMRESET事件中断处理
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_ComReset_Handler(void)
{
    //SATA接口出现COMRESET传输状态回到最初
    //TODO:还需要初始化其他什么东西？
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    SATA_ChangeState(POWERON_COMRESET,DHR0_HW_RESET_ASSERTED);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

}
void error_print(T_PSERR *pserr)
{
    PRINT_HEX("rPSERR=",pserr->dAll);//打印错误类型
#if (SATA_LOG_ENABLE == 1)

    if((pserr->dAll & 0x81FA0F03) != 0)
    {
        ERROR_PRINT("Bit[21] DIAG_C: CRC Error\r\n");
        if(tSataLogCtx.dCRCErrorCnt < 0xFFFFFFFF)
        tSataLogCtx.dCRCErrorCnt++;//CRC 错误统计

        if(!pserr->bits.DIAG_C)
        {
            if(tSataLogCtx.dNoneCRCErrorCnt < 0xFFFFFFFF)
                tSataLogCtx.dNoneCRCErrorCnt++;//非CRC 错统计
        }

    }

    if( pserr->bits.ERR_C)
    {
        ERROR_PRINT("Bit[9] ERR_C: PHY Not Ready Error\r\n");
        if(tSataLogCtx.dPHYRDY2N < 0xFFFFFFFF)
        tSataLogCtx.dPHYRDY2N++;
    }

#else//#if (SATA_LOG_ENABLE == 1)

    if( pserr->bits.RXFOV)
        ERROR_PRINT("Bit[31] RXFOV: RXFIFO Overflow Error\r\n");
    if( pserr->bits.DIAG_T)
        ERROR_PRINT("Bit[24] DIAG_T: Transport State Transition Error\r\n");
    if( pserr->bits.DIAG_S)
        ERROR_PRINT("Bit[23] DIAG_S: Link Sequence Error\r\n");
    if( pserr->bits.DIAG_H)
        ERROR_PRINT("Bit[22] DIAG_H: Handshake Error\r\n");
    if( pserr->bits.DIAG_C)
        ERROR_PRINT("Bit[21] DIAG_C: CRC Error\r\n");
    if( pserr->bits.DIAG_D)
        ERROR_PRINT("Bit[20] DIAG_D: Disparity Error\r\n");
    if( pserr->bits.DIAG_B)
        ERROR_PRINT("Bit[19] DIAG_B: 10B to 8B Decode Error\r\n");
    if( pserr->bits.DIAG_W)
        ERROR_PRINT("Bit[18] DIAG_W: Comm Wake\r\n");
    if( pserr->bits.DIAG_I)
        ERROR_PRINT("Bit[17] DIAG_I: PHY Internal Erroe\r\n");
    if( pserr->bits.ERR_E)
        ERROR_PRINT("Bit[11] ERR_E: Internal Bus Error\r\n");
    if( pserr->bits.ERR_P)
        ERROR_PRINT("Bit[10] ERR_P: Protocol Error\r\n");
    if( pserr->bits.ERR_C)
        ERROR_PRINT("Bit[9] ERR_C: PHY Not Ready Error\r\n");
    if( pserr->bits.ERR_T)
        ERROR_PRINT("Bit[8] ERR_T: Non-recovered Transient Data Integrity Error\r\n");
    if( pserr->bits.ERR_M)
        ERROR_PRINT("Bit[1] ERR_M: Recovered Communication Error\r\n");
    if( pserr->bits.ERR_I)
        ERROR_PRINT("Bit[0] ERR_I: Recovered Data Integrity\r\n");

#endif

}
// *****************************************************************************
// 函数名称: SATA_Non_Fatal_Error_Handler()
// 函数功能: Non Fatal Error 错误处理
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_Non_Fatal_Error_Handler(void)
{

    UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    PRINT_HEX("rPSERR=",SG_DSATA->rPSERR.dAll);//打印错误类型
#ifdef  _SATA_TRIGGER_CRC__
//    Abnormal_SataFail_SaveLog();
    Abnormal_Sata_INFS_Check();
    SG_DSATA->rSTCRCE.dAll=0; // CRC 植错  Disable
#endif

  //  SG_DSATA->rSTCRCE.dAll=0;
    SG_DSATA->rTRXCRC.dAll = 0;
    //error_print((T_PSERR *)&SG_DSATA->rPSERR.dAll);
    //清除错误
    SG_DSATA->rPSERR.dAll=0xFFFFFFFF;//Clear Error reg


}

// *****************************************************************************
// 函数名称: SATA_SendStatus()
// 函数功能: Fatal Error 错误处理
// 入口参数: H2D相关的字段
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_SendStatus(U8 bStatus,U8 bError,U8 I)
{
    UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    SATA_UpdateRdIdx(1);
    SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);//退出当前状态，不处理FTXDS中断了
    DSATA_SendD2HFis(bStatus,bError,I);
    DSATA_DisableAutoSend();
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__); 
}

#if (SATA_LOG_ENABLE == 1)
// *****************************************************************************
// 函数名称: SATA_SaveErrorLog()
// 函数功能: 命令出错以后保存LOG操作
// 入口参数: NONE
// 出口参数: 最终发送的D2H的参数
// 说    明: 
// *****************************************************************************
void SATA_SaveErrorLog(U8 bStatus,U8 bError,U8 bI)
{
    ERROR_PRINT("SATA_SaveErrorLog!!!\r\n");
    PRINT_TXT("SEL");//SATA_SaveErrorLog
    //TODO:保存LOG，错误统计等操作 
    tSataDC.tSyncEvent.bStatus=bStatus;
    tSataDC.tSyncEvent.bError=bError;
    tSataDC.tSyncEvent.bI=bI;
    WriterSmartErrorLogsRouter(CMD_IDLE,NULL);
    WriterGPLLogsRouter(CMD_IDLE,NULL);
}
#endif

// *****************************************************************************
// 函数名称: SATA_Fatal_Error_Handler()
// 函数功能: Fatal Error 错误处理
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_Fatal_Error_Handler(void)
{
    //ERROR_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    /*
    If IFS equals 1 during an Rx data command, firmware should do the following prior to sending a Command
    Abort D2H RegFIS:
    1. Interrupt is generated and PISR.IFS=1 and current command is Rx data command.
    2. Poll Link state (DIAGNR register at addr 0xB4 bits [28:24]) until 0x01 (Link Idle).
    3. Clear PISR/PSERR register bits and PCMDR.ST bit to exit Fatal state.
    4. Issue D2H RegFIS with Status=51h (Command Abort).
    */
	TEST_PRINT("IfsDmaIdx = %d\r\n",SG_DMA1->rINDEX);
	TEST_PRINT("IfsPass_idx = %d,min = %d,Flg = %d\r\n",SG_DSATA->rCRC_PASS_INDEX,SG_DSATA->rCRC_PASS_MIN,SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL);
    U8 Tag=DSATA_GetCurrentCmdSlot();//sg9081cc
    T_PSERR error_reg;
    error_reg.dAll=SG_DSATA->rPSERR.dAll;
    PRINT_HEX("rPSERR=",SG_DSATA->rPSERR.dAll);//打印错误类型
    SG_DSATA->rPSERR.dAll=error_reg.dAll;//Clear Error reg

    SG_DSATA->rPCMDR.bits.DTABORT=1;


#ifdef   SC_EXBUFF_EN
     DSATA_PRE_4K_ExpHandler();  // PRE_4K 命令的开了二级buff 的异常处理
#endif
#ifdef _SATA_TRIGGER_CRC__
//     Abnormal_SataFail_SaveLog();
//     Abnormal_Sata_IFS_Check();
#endif
#ifndef __BOOT_CODE__
    {
        U32 Tstart=tSataDC.dPowerOnTime;
        while(SG_DSATA->rDIAGNR.bits.LINK_CS!=0x01)
        {
           if((tSataDC.dPowerOnTime - Tstart) >= 10)
           {
               //出现在CMD 最后一个DATA FIS末尾的R_ERR，controller内部状态机可能会卡住
               ERROR_PRINT("rDIAGNR=%x\r\n",SG_DSATA->rDIAGNR.dAll);
               break;
           }
        }
    }

    if(tSataDC.bIsNcqCmd)
    {
        SG_DSATA->rHWD2HDONE0=0;//注意：软件在清除HWD2HEN使能之前要先清除该寄存器。
        SG_DSATA->rHWD2HEN.dAll=0;//NCQ 进入异常模式，取消自动回复D2H
    }
#endif
    //NOTE:！！！！！重要！！！！！！不在这里做 ST=0的话 pio 异常会卡死 导致 E:D2HTO
    SG_DSATA->rPCMDR.bits.ST=0;
    __nds32__dsb();
    SG_DSATA->rPCMDR.bits.ST=1;//ENABLE dsata


    SG_DSATA->rPCMDR.bits.TXSAS=0;//disable Transmit Status Auto-Send

    DSATA_ExceptionHandler(0xFF);

    LOG_PRINT("Tag=%d,SG_DSATA->rDATAFIS_NUM=%d\r\n",Tag,SG_DSATA->rDATAFIS_NUM);
    //数据传输出错
   // SG_DSATA->rSTCRCE.dAll=0;

    error_print(&error_reg);
    U8 err_crc=(error_reg.bits.DIAG_C<<7)|(error_reg.bits.ERR_C<<7);

    tSataDC.tSyncEvent.bNQFlg=!tSataDC.bIsNcqCmd;
    tSataDC.tSyncEvent.Tag=Tag;
    tSataDC.tSyncEvent.bError=0x04U|err_crc;

    if(SG_DSATA->rPISR.bits.PCS)
    {
        //COMRESET 出现再发送D2H、SDB会导致上盘流程失败
#if (SATA_LOG_ENABLE == 1)
        SATA_SaveErrorLog(0x7F,tSataDC.tSyncEvent.bError,1);
#endif
        SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
        return;
    }
    //TODO:中间是不是需要等待一点时间？
#if (TXRX_CRC_ERROR == TRUE)
    SG_DSATA->rTRXCRC.bits.TXCRC =0;
    SG_DSATA->rTRXCRC.bits.RXCRC =0;
    SG_DSATA->rSTCRCE.dAll=0;
#endif
#ifndef __BOOT_CODE__
    if(tSataDC.bIsNcqCmd)
    {
        SATA_ChangeState(FPDMA_QUEUED,DFPDMAQ11_ERROR);
    }
    else
#endif
    {
        U8 status=0x51;
#if (SATA_LOG_ENABLE == 1)
        //SATA_SaveErrorLog(0x7F,0x04U|err_crc,1);//TODO:IFS提前回复D2H
        /*
         * There is no Register Device to Host FIS transmitted after a Serial ATA CRC error on the last Data
            FIS of a PIO-in command nor following a Serial ATA CRC error on the Advanced Technology
            Attachment Packet Interface (ATAPI) command packet transfer.
         * */
        if(tSataDC.eTransType == PIO_DATA_IN)
        {
            status=0x7F;
        }
        SATA_SaveErrorLog(status,tSataDC.tSyncEvent.bError,1);
#else
        SATA_SendStatus(status,tSataDC.tSyncEvent.bError,1);
#endif
        ERROR_PRINT("NONE fpdmaq error\r\n");//打印错误类型
    }

    if(tSataDC.tSataMI.pInterfaceFatalErrorEvent_cb != NULL)
    {
#ifdef __DUAL_CORE__
        GIE_DISABLE();
        tSataDC.tSataMI.pInterfaceFatalErrorEvent_cb();
        GIE_ENABLE();
#else
        tSataDC.tSataMI.pInterfaceFatalErrorEvent_cb();
#endif
    }
}
// *****************************************************************************
// 函数名称: SATA_Link_State_Change_Handler()
// 函数功能: Link_State_Change 错误处理
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_Link_State_Change_Handler(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
//    if(LPS_ACTIVE == (ENUM_LPS)SG_DSATA->rPSSTSR.bits.IPM)
//    {
//        //由其他状态转变为ACTIVE状态
//        if(LPS_PARTIAL == tSataDC.eLPMSts)
//        {
//            //PARTIAL MODE ENABLE MODULE
//        }
//        else if(LPS_SLUMBER == tSataDC.eLPMSts)
//        {
//             //SLUMBER MODE ENABLE MODULE
//        }
//        tSataDC.eLPMSts=(ENUM_LPS)SG_DSATA->rPSSTSR.bits.IPM;
//        return;
//    }
    
//    tSataDC.eLPMSts=(ENUM_LPS)SG_DSATA->rPSSTSR.bits.IPM;
//    
//    if(LPS_OFF_LINE == tSataDC.eLPMSts)
//    { 
//        DEBUG_PRINT("!!ERROR, link had gone\r\n");
//        DSATA_Init();  
//    }
//    else if(LPS_PARTIAL == tSataDC.eLPMSts)
//    {
//        //PARTIAL MODE DISABLE MODULE
//    }
//    else if(LPS_SLUMBER == tSataDC.eLPMSts)
//    {
//         //SLUMBER MODE DISABLE MODULE
//    }
//    else if(LPS_DEV_SLEEP == tSataDC.eLPMSts)
//    {
//         //DEV_SLEEP MODE DISABLE MODULE
//    }
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: SATA_SReset_Handler()
// 函数功能: Software reset 判断,在某些传输模式中断中抢占任务
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
BOOL SATA_SReset_Handler(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    DEBUG_PRINT("*****************\r\n");


#if (D2H_AUTO_SEND == TRUE)
    //提前检查是否出现FPDMA CMD
#ifndef  SC_H2DFIFO_EN
    U32 dVal=SG_DSATA->rRECVTAG;
    __nds32__dsb();

    SG_DSATA->rRECVTAG =dVal;
    __nds32__dsb();
#else
    U32 dVal = FALSE ;

      if((tSataDC.bCmdPushIdx != SG_DSATA->rPUSH_INDEX.bits.dVAL) || (  0xFFFFFFFF==SG_DSATA->rRECVTAG))
      {
    	  dVal = TRUE;
      }

#endif

srst_error_handle:
    //ERROR_PRINT("dVal=%#010lx\r\n",dVal);
    if((SG_DSATA->rH2DERR.dAll != 0 && (!SG_DSATA->rH2DERR.bits.SRSTCMD)))
    {
        //AUTO SEND D2H 出现异常
        CMD_STATUS eStatus = CMD_ABORT;
        PRINT_HEX("rH2DERR=",SG_DSATA->rH2DERR.dAll);
        if(SG_DSATA->rH2DERR.bits.CROSBORD)
        {
            eStatus = CMD_ID_NOT_FOUND;
            PRINT_TXT("ID_NOT_FOUND");
        }
        else
        {
            PRINT_TXT("DUPLICATE TAG");
        }
        UART_PRINT("SG_DSATA->rTOTALCAPCITY = %x\r\n",SG_DSATA->rTOTALCAPCITY);
        UART_PRINT("SG_DSATA->rMAXTAG.dAll = %x\r\n",SG_DSATA->rMAXTAG.dAll);
        SATA_ChangeState(FPDMA_QUEUED,DFPDMAQ1_AddCommandToQueue);//自动出错处理，只发一个NCQ cmd 也算进入NCQ模式
        SATA_SaveH2DFis(FPDMA_QUEUED,&unFsmBuff);
#if (SATA_LOG_ENABLE == 1)
        tSataDC.bCmdIdx[unFsmBuff.FIS.h2d.dw3.bits.bCount>>3]=(tSataDC.bWrIdx-1)&0x1F;
#endif
        //自动回复D2H出错
        AddCommandToQueue(eStatus);
        SG_DSATA->rERR_CLR.bits.CLR_NCQCNT=1;
        tSataDC.wNCQCmdIdx=1;
        SG_DSATA->rH2DERR.dAll=0;
        return TRUE;
    }

    //ERROR_PRINT("SG_DSATA->rPCMDR.bits.FRXP=%d\r\n",SG_DSATA->rPCMDR.bits.FRXP);
    if(SG_DSATA->rPCMDR.bits.FRXP==1 || SG_DSATA->rH2DERR.bits.SRSTCMD)   //自动发送异常或， 收到软复位命令
    {
        //SG_DSATA->rPCMDR.bits.FRXP==1 表示有新的非NCQ 命令起来,会被ST=0 清掉
        SG_DSATA->rH2DERR.bits.SRSTCMD =0;
        if(0 == unFsmBuff.FIS.h2d.dw0.bits.C)
        {
            #if (SATA_LOG_ENABLE == 1)
            tSataLogCtx.dResetsCntOfCmd++;//软件复位命令，在CMD之间
            #endif

            U8 bCtrl=unFsmBuff.FIS.h2d.dw3.bits.bCtrl;
            SATA_SaveH2DFis(SRESET,&unFsmBuff);

            if ((bCtrl&0x04) == 0x04)
            {
                /*1. Register type, C bit cleared to zero,SRST set to one.*/
                if(tSataDC.eTransType != SRESET)
                {
                    //复位操作切换到reset 流程
                    //HOST 发SOFT RESET的时候会像IFS中断那样提前终止DATA FIS的传输，所以可能需要开启填充
                    ////DONOT CHANGE!!!!! srst 不能ST=0否则容易误清SRST第二个CMD的状态
                    DSATA_ExceptionHandler(0);
                    //重启SATA 接口接收下一个reset cmd，ST=0的时候SATA端还是会接收FIS
                    DSATA_DisableAutoSend();
                    tSataDC.bIsNcqCmd=FALSE;
                    tTQC.bIsNcqCmd=FALSE;
                    SATA_ChangeState(SRESET,DSR0_SOFT_RESET_ASSERT);
                }
                else
                {
                    if(DSR0_SOFT_RESET_ASSERT != tSataDC.unDevTransSts.eDSR)
                    {
                        //正在诊断又收到RESET
                        tSataDC.unDevTransSts.eDSR=DSR0_SOFT_RESET_ASSERT;
                    }
                    else
                    {
                        //目前正是复位流程，如果没有复位确认，继续在当前状态
                    }
                }

                PRINT_TXT("SR=1");
            }
            else
            {
                PRINT_TXT("SR=0");
                if(tSataDC.eTransType != SRESET)
                {
                    //非SOFT RESET 状态下收到reset 确认信号-没有收到过起始的reset信号R=1
                    PRINT_TXT("N SR=1\r\n");
                }
                else
                {
                    //software reset状态下收到确认信号
                    /*
                    Firmware should not clear the PCMDR.FRXP bit when the second H2D Control Register
                    FIS (C=0 and SRST=0) has been received. It should clear PCMDR.ST instead.
                    */

                    tSataDC.unDevTransSts.eDSR=DSR1_EXECUTE_DIAGNOSTICS;
                    if(tSataDC.bIsRevertingEnabled)
                    {
                        #ifndef	_FLYCODE_EN_
                        //Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
                        #endif
                    }

                }
            }

            DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
            return TRUE;
        }
        else
        {

        }
        //当前收到的cmd 不是FPDMA cmd
        if(dVal != 0)
        {
            //一堆NCQ命令跟着一个非NCQ命令
            tSataDC.bIsNcqCmd=TRUE;
            tTQC.bIsNcqCmd=TRUE;
            SATA_ChangeState(FPDMA_QUEUED,DFPDMAQ1_AddCommandToQueue);
            PRINT_TXT("NO-NCQ");
        }

        return FALSE;
    }
    else
    {

        if(dVal == 0)
            return TRUE;
        //正确的FPDMA cmd 被回应
        if(tSataDC.eTransType != FPDMA_QUEUED)
        {
#if (SDB_AUTO_SEND==TRUE)
            DSATA_EnableAutoSend();
#endif
            //idle 状态过来的
            SG_DSATA->rERR_CLR.bits.DFISPATH_SEL = 0;//DMA 模式
            SATA_ChangeState(FPDMA_QUEUED,DFPDMAQ1_AddCommandToQueue);
            tSataDC.bIsNcqCmd=TRUE;//有效NCQ命令
            tTQC.bIsNcqCmd=TRUE;
			if( SCSI_MODE == tScsiCtl.bMode_old)
			{
				tScsiCtl.bMode_old = NORMAL_MODE;
				DSATA_ExbuffAndCheckPoit_Ctrl(NORMAL_MODE);
			}
            tScsiCtl.dNextAddr = LBA_00005500;
            tScsiCtl.bMode     = NORMAL_MODE;
            tScsiCtl.bState    = CMD_STATE;
            tScsiCtl.bVaild    = FALSE;
            UART_PRINT("Exit SCSI mode....................\r\n");

            //PRINT_TXT("NCQ ST");
        }
#ifndef  SC_H2DFIFO_EN

        U8 N=0,bTag=0,dma_auto_active=FS_AUTO;

        do
        {
            //__enable_performance();
#ifdef __DUAL_CORE__
            SG_DSATA->rPISR.dAll=HDRS_BIT;//H2D中断会卡主DMA SETUP流程

            if(dVal == 0)
            {
                dVal=SG_DSATA->rRECVTAG;
                __nds32__dsb();
                SG_DSATA->rRECVTAG =dVal;
            }
#endif

            //优化扫描流程
            if(((1<<bTag) & dVal) && arrunHWD2HFS[bTag].HWD2HFS.wCount == tSataDC.wNCQCmdIdx)
            {
                //如果没有重排的话，很大概率会提前找到下一个命令
                //HOST 一般都是按照tag号，递增
                //PRINT_TXT("ACC");
            }
            else
            {
                //优化查找流程，按照命令先后顺序入cmd队列
                N=0;

                if((dVal&0xffffff) == 0)
                {
                    N=24;
                }
                else  if((dVal&0xffff) == 0)
                {
                    N=16;
                }
                else  if((dVal&0xff) == 0)
                {
                    N=8;
                }

                //根据AUTO D2H记录的索引号，按照索引从小到达的顺序添加任务，保证队列中的任务的索引是正确的
                for(/*N=0*/;N<32;N++)
                {
                    if(((1<<N) & dVal) && arrunHWD2HFS[N].HWD2HFS.wCount == tSataDC.wNCQCmdIdx)
                    {
                        bTag=N;//找到CMD
                        break;
                    }
                }
            }
           //PRINT_DEC("IN1=",__get_pfm_count());//25-524个时钟
           //__enable_performance();
           if(N<32)
           {
                UN_FSM *unpFSM=(UN_FSM*)&arrunHWD2HFS[bTag].adBuff[0];
                //UN_CH_DI unDI;
                T_HI_TASK *ptTask=&tTQC.tHiTasks[bTag];
                U8 bDir = !(unpFSM->FIS.h2d.dw0.bits.Cmd & 0x01);

                //tSataDC.bATACmd = unpFSM->FIS.h2d.dw0.bits.Cmd;
                //ERROR_PRINT("ATACmd=%x\r\n",tSataDC.bATACmd);

                //找到对应的BIT 号、tag号
                ptTask->tParam.dLba=arrunHWD2HFS[bTag].HWD2HFS.dLBA;//LBA
                ptTask->tParam.dSctSize=arrunHWD2HFS[bTag].HWD2HFS.dSecSize;//SEC SIZE

                //READ 不能使能AUTO ACTIVE 功能否则接口不能传输数据
                //unDI.dAll=0x100|bTag|(bDir<<6)|(dma_auto_active<<9)|(1<<15);
                //if(bDir == DIR_OUT)
                    //unDI.dAll|=((dma_auto_active<<9));

#if 0
                DSATA_SetCmdHead((CMD_SLOT)bTag,
                                  unDI,
                                  ptTask->tParam.dSctSize,
                                  ptTask->tParam.dSctSize,
                                 (ptTask->tParam.dSctSize<<9),
                                 0);

                ptTask->tParam.bTag=bTag;
                ptTask->tParam.bDir=bDir;
                ptTask->tParam.bFua = (unpFSM->FIS.h2d.dw1.bits.bDevice>>7);
                ptTask->tParam.dBuffAddr=tSataDC.tSataMI.dDmaBuffAddr;
                TQM_CreateTask(OP_HI_DATA,ptTask,ATACmd_FpdmaRwTaskComplete);
#else
#ifdef SC_PORT_MULTIPLIER
                U8 bPM = unpFSM->FIS.h2d.dw0.bits.PM_port;
                tCmdHead[bTag].dw0.dAll=0x100|bTag|(bDir<<6)| (bPM<<10)|(1<<15);
#else
                tCmdHead[bTag].dw0.dAll=0x100|bTag|(bDir<<6)| (1<<15);
#endif
                if(bDir == DIR_OUT)//READ 不能使能AUTO ACTIVE 功能否则接口不能传输数据
                    tCmdHead[bTag].dw0.dAll|=(dma_auto_active<<9);

                //tCmdHead[bTag].dw0.cmd.PRDTL=1;//放到上面有加速
                tCmdHead[bTag].dDMATC=(ptTask->tParam.dSctSize<<9);
                //tCmdHead[bTag].dDMABO=0;
                //tCmdHead[bTag].dPRDTBA=(U32)&tPrdTbls[bTag];

                ptTask->tParam.bTag=bTag;
                ptTask->tParam.bDir=bDir;
//                PRINT("DIR = %d\r\n",bDir);
                ptTask->tParam.bFua = (unpFSM->FIS.h2d.dw1.bits.bDevice>>7);
                ptTask->tParam.dBuffAddr=tSataDC.tSataMI.dDmaBuffAddr;
                ptTask->bOpcode=OP_HI_DATA;
                //tSataDC.pfTaskCompleteCb=ATACmd_FpdmaRwTaskComplete;

                tPrdTbls[ptTask->tParam.bTag].dw3.dAll=0x40000000 | ((ptTask->tParam.dSctSize<<9) -1);//tPrdTbls[bSlot].dw3.bits.W=1;
#if (SDB_AUTO_SEND==TRUE)
                //通过中断发送状态
#ifdef __SDB_AUTO_ACC__
                //tPrdTbls[ptTask->tParam.bTag].dw3.bits.I=0;
#else
                tPrdTbls[ptTask->tParam.bTag].dw3.bits.I=1;
#endif
#else
                //tPrdTbls[ptTask->tParam.bTag].dw3.bits.I=0;
#endif
                tPrdTbls[ptTask->tParam.bTag].dDBA=ptTask->tParam.dBuffAddr;//这个buffer不赋值的话会导致NCQ 跑挂
                //tPrdTbls[ptTask->tParam.bTag].dw3.bits.DBC=(ptTask->tParam.dSctSize<<9) -1;

                TQM_AddTask(ptTask);
                //PRINT_DEC("IN22=",__get_pfm_count());//205 个时钟
                //PRINT_DEC("bTag=",bTag);

                GIE_DISABLE();
                SATA_SaveH2DFis(FPDMA_QUEUED,unpFSM);
                GIE_ENABLE();
#if (SATA_LOG_ENABLE == 1)
                tSataDC.bCmdIdx[bTag]=(tSataDC.bWrIdx-1)&0x1F;
#endif
                //NOTE:这个位置G3可能产生新命令的reorder请求并立即启动，下面调用DSATA_CmdReorderRestart会提前清除任务。
#ifdef __NCQ_QUEUE_NEW__
                //if(SG_DSATA->rPISR.bits.CMDONS)
                DSATA_CmdReorderRestart((T_PISR*)&SG_DSATA->rPISR,1);//更新任务的过程中可能产生DPP，或者reorder done
#endif
                //NOTE:SATA_SaveH2DFis 在这个位置的话可能会由于SF寄存器已经被清除，导致tSataDC.bCmdCnt管理异常！！！

                //清除已经添加过的任务
                dVal^=(1<<bTag);
                tSataDC.wNCQCmdIdx++;//指向下一个CMD
                bTag++;
                bTag=(bTag&0x1F);//提前指向下一个CMD
#endif
           }
           else
           {
               PRINT_TXT("E:NCQ");//count 没有匹配上，没有找到CMD
               tSataDC.wNCQCmdIdx=1;
               SG_DSATA->rERR_CLR.bits.CLR_NCQCNT=1;
               break;
           }
#ifdef __DUAL_CORE__
        }while(dVal != 0 || SG_DSATA->rRECVTAG != 0);
#else
        }while(dVal != 0 /*|| SG_DSATA->rRECVTAG != 0*/);
#endif
#else
//        PRINT(",POP = %d,Push = %d,wr=%d\r\n",SG_DSATA->rPOP_INDEX.bits.dVAL,SG_DSATA->rPUSH_INDEX.bits.dVAL,tTQC.bCmdPushIdx);
		U8 bTag=0,dma_auto_active=FS_AUTO;
		U32 dRecvTagMask = 0;;

			do
			{
//	            if(dVal == 0)
//	            {
//	            	SG_DSATA->rPISR.dAll=HDRS_BIT;//H2D中断会卡主DMA SETUP流程
//	                dVal=SG_DSATA->rRECVTAG;
//	                __nds32__dsb();
//	                SG_DSATA->rRECVTAG =dVal;
//	            }
//	            SG_DSATA->rPISR.dAll=HDRS_BIT;//H2D中断会卡主DMA SETUP流程

				UN_HWH2DFIS *unpH2DFSM=(UN_HWH2DFIS*)&arrunHWD2HFS[tSataDC.bCmdPushIdx].adBuff[0] ;
				bTag = unpH2DFSM->HWH2DFS.bTag;
				dRecvTagMask |= 1<<bTag ;

#ifdef   SC_WP_4K_PRE
				//if((SG_DSATA->rPRE_TRIG_CNT)&&( TRUE == SG_DSATA->rPRE_TRIG_ON.bits.dVAL)&&(FALSE==tTQC.bPRE_Cmd_Flg)) //存在预收命令，并且还没把H2D 搬到tTQC.tHiTasks[32]
				if((FALSE==tTQC.bPRE_Cmd_Flg)&&(SG_DSATA->rPRE_TRIG_CNT)) //存在预收命令，并且还没把H2D 搬到tTQC.tHiTasks[32]
				{
//					PRINT_HEX("PreTag=",bTag);
//					PRINT_HEX("Flg1>",tTQC.bPRE_Cmd_Flg);
//					PRINT_HEX("Pg>",bTag);
//					PRINT_HEX("BT>", tTQC.dDebugTag);
//					PRINT_HEX("s1>",SG_DSATA->rEXBUF_STS.dAll);
					bTag = CMD_SLOT32;    // 修改预收命令的tag = 32
					tTQC.bPRE_Cmd_Flg = TRUE;
					tSataDC.bPre4kCmdFlg = TRUE;
		            SG_DSATA->rPIER.bits.FTXDE=1;//ENABLE FTXDS
		            tTQC.dPRECMD_PushCnt++;
		            if(tTQC.dPRECMD_PushCnt>=256)
		            {
		            	tTQC.dPRECMD_PushCnt=0;
		            }
#ifdef  _SATA_DEBUG_MESSAGE_
		            if(0 == SG_DSATA->rPRE_TRIG_CNT)
		            {
		            	PRINT_TXT("E:PRE>");
		            	 while(1);
		            }
#endif
				}
				else
#endif  //SC_WP_4K_PRE
				{
					 SG_DSATA->rPIER.bits.FTXDE=0;//ENABLE FTXDS
//					 PRINT_HEX("st2>",SG_DSATA->rEXBUF_STS.dAll);
//					PRINT_HEX("P>",SG_DSATA->rPRE_TRIG_CNT);
//					PRINT_HEX("On>",SG_DSATA->rPRE_TRIG_ON.bits.dVAL);
//					PRINT_HEX("Flg>",tTQC.bPRE_Cmd_Flg);
//					PRINT_HEX("PreTag=",bTag);

					U8 bDir = unpH2DFSM->HWH2DFS.bDir;
#ifdef SC_PORT_MULTIPLIER
					U8 bPM = unpH2DFSM->HWH2DFS.h2d.dw0.bits.PM_port;
					tCmdHead[bTag].dw0.dAll=0x100|bTag|(bDir<<6)|(bPM<<10)|(1<<15);
#else
					tCmdHead[bTag].dw0.dAll=0x100|bTag|(bDir<<6)|(1<<15);
#endif
					if(bDir == DIR_OUT)//READ 不能使能AUTO ACTIVE 功能否则接口不能传输数据
						tCmdHead[bTag].dw0.dAll|=(dma_auto_active<<9);
	                tCmdHead[bTag].dDMATC=unpH2DFSM->HWH2DFS.dSecSize<<9;
	                tPrdTbls[bTag].dw3.dAll=0x40000000 | ((unpH2DFSM->HWH2DFS.dSecSize<<9) -1);//tPrdTbls[bSlot].dw3.bits.W=1;
	                tPrdTbls[bTag].dDBA=tSataDC.tSataMI.dDmaBuffAddr;//这个buffer不赋值的话会导致NCQ 跑挂
#if (SDB_AUTO_SEND==TRUE)
                   //通过中断发送状态
#ifdef __SDB_AUTO_ACC__
                   //tPrdTbls[ptTask->tParam.bTag].dw3.bits.I=0;
#else
                   tPrdTbls[bTag].dw3.bits.I=1;
#endif
#else
				  //tPrdTbls[ptTask->tParam.bTag].dw3.bits.I=0;
#endif
				}
	//			Mem32Copy((U32*)&tTQC.tHiTasks[bTag].tParam,(U32*) &arrunHWD2HFS[tTQC.bCmdPushIdx].HWH2DFS.dLBA ,3);
				tTQC.tHiTasks[bTag].tParam.dLba = arrunHWD2HFS[tSataDC.bCmdPushIdx].HWH2DFS.dLBA;
				tTQC.tHiTasks[bTag].tParam.dSctSize = arrunHWD2HFS[tSataDC.bCmdPushIdx].HWH2DFS.dSecSize;
				*(U32*)&tTQC.tHiTasks[bTag].tParam.wLBA_exp = *(U32*)&arrunHWD2HFS[tSataDC.bCmdPushIdx].HWH2DFS.wLBA_exp ;   // Copy 一个DW0
				tTQC.tHiTasks[bTag].tParam.bFua = (unpH2DFSM->HWH2DFS.h2d.dw1.bits.bDevice>>7);
				tTQC.tHiTasks[bTag].tParam.dBuffAddr=tSataDC.tSataMI.dDmaBuffAddr;
				tTQC.tHiTasks[bTag].bOpcode=OP_HI_DATA;
				tTQC.tHiTasks[bTag].tParam.bTag = bTag;
//				tTQC.tHiTasks[bTag].tParam.bDir = tTQC.tHiTasks[bTag].tParam.bNcqCmdDir;
                TQM_AddTask(&tTQC.tHiTasks[bTag]);

				GIE_DISABLE();
				SATA_SaveH2DFis(FPDMA_QUEUED,(UN_FSM *)unpH2DFSM);
				GIE_ENABLE();
#if (SATA_LOG_ENABLE == 1)
			bTag = unpH2DFSM->HWH2DFS.bTag;
			tSataDC.bCmdIdx[bTag]=(tSataDC.bWrIdx-1)&0x1F;
#endif
			tSataDC.bCmdPushIdx++;   //指向下一个push 的命令
#ifdef  SC_H2DFIFO_EN
            	if(33 == tSataDC.bCmdPushIdx)
            	{
            		tSataDC.bCmdPushIdx = 0;
            	}

#else
            	tSataDC.bCmdPushIdx = tSataDC.bCmdPushIdx & 0x1F;
#endif
			}while(tSataDC.bCmdPushIdx != SG_DSATA->rPUSH_INDEX.bits.dVAL);

//			}while(dVal != 0 || SG_DSATA->rRECVTAG != 0);
			SG_DSATA->rRECVTAG = dRecvTagMask; //取tag 并把rRECVTAG 相应的bit 位清0
//			 PRINT_HEX("Tg>",dRecvTagMask);
			// rRECVTAG 比 SG_DSATA->rPUSH_INDEX 先置起
//			if( MAX_CMD_NUM == SG_DSATA->rH2D_COUNTER.bits.dVAL)
//			{
//				SG_DSATA->rPISR.bits.HDRS = 0;
//			}
#endif
			//解决命令结束
            if((TRUE == tSataDC.bPre4kCmdFlg)&&(FALSE == SG_DSATA->rPRE_TRIG_ON.bits.dVAL))
            {
        		SG_DSATA->rPIER.bits.FTXDE=0;//DISABLE FTXDS
        		tSataDC.bPre4kCmdFlg = FALSE;
        		SATA_UpdateRdIdx(1);
//        		PRINT_HEX("Dcnt11>",tSataDC.bCmdCnt);
            }
#ifdef   H2D_DMASETUP_TEST

//        SG_DSATA->rPISR.dAll=HDRS_BIT;//H2D中断会卡主DMA SETUP流程
         SG_DSATA->rPIER.bits.HDRE = 0;//H2D中断会卡主DMA SETUP流程
#endif //#ifndef   H2D_DMASETUP_TEST
        tSataDC.eDPMSts=DPS_ACTIVE;
        tSataDC.dStanbyTimerCnt=0;
#if(SATA_LOG_ENABLE == 1)
        tSataLogCtx.dE_RWTimer=tSataDC.dPowerOnTime;//有新任务就把更新LOG的定时器计数清零
#endif
        if(SG_DSATA->rH2DERR.dAll != 0 || SG_DSATA->rPCMDR.bits.FRXP==1)
        {
            //NCQ cmd malformed,如果此时出现ERR，可能会丢失中断卡死
            PRINT_TXT("NCQ malformed1");
            goto srst_error_handle;
        }
        return TRUE;
    }
#else
    if(0 == unFsmBuff.FIS.h2d.dw0.bits.C)
    {
        #if (SATA_LOG_ENABLE == 1)
        tSataLogCtx.dResetsCntOfCmd++;//软件复位命令，在CMD之间
        #endif

        if ((unFsmBuff.FIS.h2d.dw3.bits.bCtrl&0x04) == 0x04)
        {
            /*1. Register type, C bit cleared to zero,SRST set to one.*/
            SG_DSATA->rPCMDR.bits.DTABORT=1;//终止可能存在的数据传输过程
            //SATA_ClearCmd();
            SATA_SaveH2DFis(SRESET,&unFsmBuff);
            if(tSataDC.eTransType != SRESET)
            {
                //复位操作切换到reset 流程
                SATA_ChangeState(SRESET,DSR0_SOFT_RESET_ASSERT);
            }
            else
            {

                if(DSR0_SOFT_RESET_ASSERT != tSataDC.unDevTransSts.eDSR)
                {
                    //正在诊断又收到RESET
                    tSataDC.unDevTransSts.eDSR=DSR0_SOFT_RESET_ASSERT;
                }
                else
                {
                    //目前正是复位流程，如果没有复位确认，继续在当前状态
                }
            }
            DEBUG_PRINT("DSATA software reset R=1\r\n");
        }
        else
        {
            if(tSataDC.eTransType != SRESET)
            {
                //非SOFT RESET 状态下收到reset 确认信号-没有收到过起始的reset信号R=1
            }
            else
            {
                //software reset状态下收到确认信号
                /*
                Firmware should not clear the PCMDR.FRXP bit when the second H2D Control Register
                FIS (C=0 and SRST=0) has been received. It should clear PCMDR.ST instead.
                */
                SG_DSATA->rPCMDR.bits.ST=0;
                tSataDC.unDevTransSts.eDSR=DSR1_EXECUTE_DIAGNOSTICS;
                if(tSataDC.bIsRevertingEnabled)
                {
                    #ifndef	_FLYCODE_EN_
                    Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
                    #endif
                }
                DEBUG_PRINT("DSATA software reset R=0\r\n");
            }
        }

            DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
            return TRUE;
    }
    else
    {

    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return FALSE;
#endif
}

// *****************************************************************************
// 函数名称: SATA_PrdDoneEventHandler()
// 函数功能: 单次PRD完成中断处理
// 入口参数: slot
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_PrdDoneEventHandler(U8 bSlot)
{
    T_HI_TASK *ptTask;
    DEBUG_PRINT("bSlot =%u\r\n" ,bSlot);
    GPIO_DEBUG(0x45);
    if(tSataDC.pfTransferCompleteCb != NULL)
    {
        //回调ATA层任务处理 --1.PIO传输任务分解 --2.不经过FTL的数据写任务完成后的数据校验
        //接口参数对于不经过FTL模块的任务（不写NAND FLASH 直接写读缓冲区的ATA命令）暂时没有意义
        ptTask=TQM_GetTask(bSlot);
        //先删除任务再回调重新创建任务
        if(TRUE == ptTask->tParam.bIsUsed)
        {
            //该任务需要经过FTL模块
            TQM_RemoveTask(bSlot);
        }
        GPIO_DEBUG(0x46);
        tSataDC.pfTransferCompleteCb(CMD_DONE,&ptTask->tParam);
       // tSataDC.pfTransferCompleteCb=NULL; //不可以清掉，因为之前的调用会把它重新赋值
    }
    DEBUG_PRINT("EXIT <%s>\r\n" ,__func__);    
}
    
// *****************************************************************************
// 函数名称: SATA_UpdateRdIdx()
// 函数功能: 更新取任务索引，多个地方会进行该操作所以封装成函数
// 入口参数: bNum 完成的任务数
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_UpdateRdIdx(U8 bNum)
{
    DEBUG_PRINT("ENTER <%s>\r\n" ,__func__);
    DEBUG_PRINT("tSataDC.bWrIdx=%u,tSataDC.bCmdCnt=%u,tSataDC.bRdIdx=%u\r\n",tSataDC.bWrIdx,tSataDC.bCmdCnt,tSataDC.bRdIdx);
    //读索引指向下一个命令
    if(tSataDC.bCmdCnt>0)
    {
        tSataDC.bRdIdx+=bNum;
        tSataDC.bCmdCnt-=bNum;
    }

    if(tSataDC.bRdIdx >= MAX_CMD_NUM)
    {
        //正确完成至少32个命令
        tSataDC.IsOvLap=TRUE;
        tSataDC.bRdIdx=tSataDC.bRdIdx&0x1F;
    }
    
    DEBUG_PRINT("IsOvLap=%u\r\n",tSataDC.IsOvLap);
    DEBUG_PRINT("tSataDC.bWrIdx=%u,tSataDC.bCmdCnt=%u,tSataDC.bRdIdx=%u\r\n",tSataDC.bWrIdx,tSataDC.bCmdCnt,tSataDC.bRdIdx);
    DEBUG_PRINT("EXIT <%s>\r\n" ,__func__);
}
// *****************************************************************************
// 函数名称: SATA_FTXDS_Handler()
// 函数功能: SATA传输层数据收发完成后正确的状态D2H被HOST接收中断统一处理接口
// 入口参数: NONE
// 出口参数: 
// 说    明: 所有数据模块都是一样的操作，可以合并处理
// *****************************************************************************
void SATA_FTXDS_Handler(void)
{
    DEBUG_PRINT("ENTER <%s>\r\n" ,__func__);
    //本次ATA命令相关的数据传输结束
    if(DSATA_IsAutoSendEnabled())
    {
        //使能自动发送的肯定是需要经过FTL的任务，完成以后切换状态
        SATA_UpdateRdIdx(1);
        SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
        //SATA端任务管理上下文更新了，更新FTL任务，以免两边不同步
        DSATA_DisableAutoSend();
    }
    
    DEBUG_PRINT("EXIT <%s>\r\n" ,__func__);
}
// *****************************************************************************
// 函数名称: SATA_SaveH2DFis()
// 函数功能: 保存有效的FIS
// 入口参数:  eType 当前传输类型
// 出口参数: 是否正常保存
// 说    明: 
// *****************************************************************************
BOOL SATA_SaveH2DFis(ENUM_SATA_TRSP_TYPE eType,UN_FSM *unpFSM)
{
    T_H2D *ptH2d=&tSataDC.tCmdList[tSataDC.bWrIdx].h2d;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //DEBUG_PRINT("tSataDC.bWrIdx=%u,tSataDC.bCmdCnt=%u,tSataDC.bRdIdx=%u\r\n",tSataDC.bWrIdx,tSataDC.bCmdCnt,tSataDC.bRdIdx);
#if (SATA_LOG_ENABLE == 1)     
    tSataDC.arrenAddrMode[tSataDC.bWrIdx]=tSataDC.enAddrMode;
#endif

//    if(MAX_CMD_NUM == tSataDC.bCmdCnt)
//    {
//        //ERROR!!当前缓冲区已满
//        //发送ABORT FIS
//        //TODO:出错处理,记录LOG
//        GPIO_DEBUG(0x94);
//        SATA_SendStatus(0x51,0x04,1);
//        SATA_ClearCmd();
//        return FALSE;
//    }
    ptH2d->dw0.dAll=unpFSM->FIS.h2d.dw0.dAll;
    ptH2d->dw1.dAll=unpFSM->FIS.h2d.dw1.dAll;
    ptH2d->dw2.dAll=unpFSM->FIS.h2d.dw2.dAll;
    ptH2d->dw3.dAll=unpFSM->FIS.h2d.dw3.dAll;
    ptH2d->dw4.dAll=unpFSM->FIS.h2d.dw4.dAll;

    tSataDC.bPM = ptH2d->dw0.bits.PM_port;
    //Mem32Copy((U32*)(&tSataDC.tCmdList[tSataDC.bWrIdx].h2d.dw0.dAll),unFsmBuff.adBuff,sizeof(T_H2D)>>2);//bug fix 四字节对齐访问，长度溢出
    //UART_PRINT("tSataDC.bWrIdx =%u\r\n",tSataDC.bWrIdx);
    tSataDC.tCmdList[tSataDC.bWrIdx].dTimeStamp=tSataDC.dPowerOnTime;
    //更新命令索引
    tSataDC.bWrIdx++;
    //更新队列中任务数
    tSataDC.bCmdCnt++;

    if(MAX_CMD_NUM == tSataDC.bWrIdx)
    {
        tSataDC.bWrIdx=0;
    }

    if(eType != FPDMA_QUEUED)
    {
         DSATA_EnableRxFis();//重新使能接收
    }
    else
    {
        /*
        If the H2D Register FIS with a new command is received before a DMA Setup FIS is
        transmitted, the TxFIFO is cleared to prevent the DMA Setup FIS from transmitting. In this
        case, firmware must prepare the D2H Register FIS and set the PCMDR.TXREG bit prior to
        clearing PISR.HDRS and PCMDR.FRXP bits to send the D2H Register FIS to un-queue this
        new command. DMA Setup FIS is sent after D2H Register FIS.
        */
        //等到回复了H2D fis以后再使能
    }
    //DEBUG_PRINT("tSataDC.bWrIdx=%u,tSataDC.bCmdCnt=%u,tSataDC.bRdIdx=%u\r\n",tSataDC.bWrIdx,tSataDC.bCmdCnt,tSataDC.bRdIdx);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return TRUE;
}

// *****************************************************************************
// 函数名称: SATA_Execute_Diag
// 函数功能: 设备诊断
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
BOOL SATA_Execute_Diag(void)
{
    //设备诊断过程
    return TRUE;
}


// *****************************************************************************
// 函数名称: SATA_SendGoodBadStatus()
// 函数功能: 公用子函数
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SATA_SendGoodBadStatus(BOOL status,U8 I)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //准备缓冲区数据
    DSATA_ClearFsmBuff();
    GPIO_DEBUG(0x58);

    unFsmBuff.FIS.d2h.dw0.bits.bType=FIS_CODE[D2H_FIS];
    unFsmBuff.FIS.d2h.dw0.bits.bStatus=0x50;
    unFsmBuff.FIS.d2h.dw0.bits.bError=0x01;//good status 是01
    unFsmBuff.FIS.d2h.dw0.bits.I=I;

    unFsmBuff.FIS.d2h.dw1.bits.bLBA_low=0x01;
    unFsmBuff.FIS.d2h.dw3.bits.bCount=0x01;
    //SATA接口READY进行诊断过程
    if(status)
    {
        //诊断成功
        //TODO:在诊断的过程中可以被Software reset 打断
        //将来变成状态机？
    }
    else
    {
        //诊断失败
        unFsmBuff.FIS.d2h.dw0.bits.bError=0x00;
    }
//     GPIO_DEBUGDW((U32)unFsmBuff.adBuff);
//     GPIO_DEBUGDW(unFsmBuff.FIS.d2h.dw0.dAll);
//     GPIO_DEBUGDW(unFsmBuff.FIS.d2h.dw1.dAll);
//     GPIO_DEBUGDW(unFsmBuff.FIS.d2h.dw2.dAll);
//     GPIO_DEBUGDW(unFsmBuff.FIS.d2h.dw3.dAll);
//     GPIO_DEBUGDW(unFsmBuff.FIS.d2h.dw4.dAll);
    //诊断完成发送状态,发送 Device to Host fis
    DSATA_TrigeSendFis(D2H_FIS);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: SATA_ClearCmd
// 函数功能: 出错清除任务管理相关
// 入口参数: 
// 出口参数: 
// 说    明:
// *****************************************************************************
void SATA_ClearCmd(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //清除命令

    DEBUG_PRINT("tSataDC.bCmdCnt=%u,tSataDC.bWrIdx=%u,tSataDC.bRdIdx=%u\r\n",tSataDC.bCmdCnt,tSataDC.bWrIdx,tSataDC.bRdIdx);

#if (D2H_AUTO_SEND == TRUE)
    SG_DSATA->rERR_CLR.bits.CLR_NCQCNT=1;
    tSataDC.wNCQCmdIdx=1;
#endif
    
    //if(tSataDC.bCmdCnt != 0)
    {
        TQM_AbortAllTask();
    }
    tSataDC.bCmdCnt=0;
    tSataDC.bRdIdx=tSataDC.bWrIdx;

#ifndef __NCQ_QUEUE_NEW__
    tSataDC.qwTaskCnt=0;
#endif

//#if (SATA_LOG_ENABLE == 0)
//    tSataDC.bIsNcqCmd=FALSE;
//    tTQC.bIsNcqCmd=FALSE;
//#endif
    
    DEBUG_PRINT("tSataDC.bWrIdx=%u,tSataDC.bRdIdx=%u\r\n",tSataDC.bWrIdx,tSataDC.bRdIdx);
   
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: SATA_DHR_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的Power-on and COMRESET protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DHR->Device Hardware Reset
// *****************************************************************************
void SATA_DHR_StateMachine(void)
{

    //上电之后最开始跑的状态机
    switch(tSataDC.unDevTransSts.eHDR)
    {
    case DHR0_HW_RESET_ASSERTED:

        /*1. Hardware reset negated.->DHR1:Execute_diagnostics*/
        //break;
    case DHR1_EXECUTE_DIGAGNOSTICS:
        /*
        1. Initialization and diagnostics completed successfully.->DHR2:Send_good_status
        2. Initialization completed and diagnostics failed. ->DHR3:Send_bad_status
        */
        //break;
    case DHR2_SEND_GOOD_STATUS:
        //break;
    case DHR3_SEND_BAD_STATUS:

        //为了加快速度四个状态一并处理，PON,COMRESET产生，等待SATA接口READY

        if(DSATA_LinkCheck())
        {
            /*
            The following steps are done to enable the DSATA engine.
            These steps should be done after SATA link has been determined to have 
            been successfully established.
            */
			SG_DSATA->rDEL_NONALIGN_EN.dAll  = 0;//NOTE:LINK 成功以后，发送D2H前关闭此标记，解决J1900 不识别X_READY的问题

            NVIC_DisableIRQ(IRQ_SC);
            DSATA_StartupEnable();
            //delay_ms(600);//FOR tigo 工控主板，延迟600ms以上才可以认盘，问题已经修复，不存在此需要
            //但是GEN3 还是有几率进BIOS！！？？？？
            delay_ms(10);

            if(SG_DSATA->rPCMDR.bits.FRXP)
            {
            	SG_DSATA->rPISR.dAll=HDRS_BIT;//重要：有些主板在没有打D2H之前就发H2D过来（dell 灵越笔记本）,实际上不需要处理这个命令，清除标记
            	DSATA_EnableRxFis();
            }

            delay_ms(1);

            SATA_SendGoodBadStatus(SATA_Execute_Diag(),0);//诊断并发送状态

            if(SG_DSATA->rPCMDR.bits.FRXP)
            {
				SG_DSATA->rPISR.dAll=HDRS_BIT;//有些主板（thinkpad 笔记本）没有发D2H之前会发SOFTRESET 命令，而SOFTRESET 必需要清除2次 H2D中断和FRXP
				DSATA_EnableRxFis();
            }
            PRINT_TXT("D2H 600");

#if (SATA_LOG_ENABLE == 1)
            if(tSataDC.bGenxSpeed != 0xFF)
            {
                //power cycle期间不需要计算次数
                if(tSataLogCtx.dD2hFisDue2Comreset < 0xFFFFFFFF)
                    tSataLogCtx.dD2hFisDue2Comreset++;
                if(tSataDC.bGenxSpeed != SG_DSATA->rPSSTSR.bits.SPD)
                {
                    //新旧值不相同
                    if(tSataLogCtx.tSmartCtx.wSataDownShiftErrCnt < 0xFFFFFFFF)
                        tSataLogCtx.tSmartCtx.wSataDownShiftErrCnt++;
                }
                }
            else
            {
                if(tSataLogCtx.dD2hFisDue2Comreset > 0 && tSataLogCtx.dD2hFisDue2Comreset != 0xFFFFFFFF)
                {
                    tSataLogCtx.dD2hFisDue2Comreset--;//ULINK 测试需要减掉 掉电前的一次数值
                }
            }
            tSataDC.bGenxSpeed=SG_DSATA->rPSSTSR.bits.SPD;

#ifdef SATA_RECNT_ENABLE
            //WILLIAM:工控平台J1900 上盘兼容性处理
            if(tSataDC.bGenxSpeed == 3)
			{
				tSataLogCtx.dSataReconectMode = 0;//暂时默认SATA3 平台不启用此机制
			}
            else
            {
				if(tSataDC.bFirstH2dCmd != 0xFF)
				{
					//打出D2H 以后开始监控第一个H2D来的时间
					tSataDC.bFirstH2dCmd = 0xFF;
					tSataDC.dNDCTOGuardTimer=tSataDC.dPowerOnTime;
				}
            }
            //WILLIAM:工控平台J1900 上盘兼容性处理end
#endif

#endif
            //SetBits(unAtaIdentify.wArray+77,(U8)3,(U8)3,((U16)(SG_DSATA->rPSSTSR.bits.SPD)));
            unAtaIdentify.wArray[77]&=(~(7<<1));
            unAtaIdentify.wArray[77]|=((SG_DSATA->rPSSTSR.bits.SPD)<<1);
            GPIO_DEBUG(0x65);
            //切换状态
            SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE); 

#ifdef SATA_RECNT_ENABLE
            if((tSataDC.bGenxSpeed != 3) && (tSataDC.bPHYRESET != 0xAA) && (tSataLogCtx.dSataReconectMode == 0xDEADDEAD))
            {
            	//WILLIAM:工控平台J1900 上盘兼容性处理
                PRINT_TXT("wait4H2D");//100MS超时
                delay_ms(100);
                if((SG_DSATA->rPISR.dAll & HDRS_BIT) == 0)
                {
                	//没有收到H2D，直接复位
                	PRINT_TXT("Reconnect..");//100MS超时
                	delay_ms(1);
					//reset PHY
					SG_CPM->rPRE_CLK_SOURCE_SEL=0x02;//选择25M
					SG_CPM->rSYS_CLK_SOURCE_SEL=1;//选择pre clk
					SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0xF;//offline
					SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0;//normal
					Delay(10);
					SystemClkRecovery();//PHY时钟稳定后，恢复时钟配置
                }
            }
            tSataDC.bPHYRESET = 0;
#endif
            NVIC_EnableIRQ(IRQ_SC);
        }
       
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DHR_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的Power-on and COMRESET protocol状态下中
//           断响应函数
// 入口参数: void* 指针，实际使用中会传入SATA设备中断寄存器内容
// 出口参数: 
// 说    明: DHR->Device Hardware Reset，此函数工作在中断模式
// *****************************************************************************
void SATA_DHR_IntEventHandler(T_PISR *ptPISR)
{

    //TODO:这个阶段需不需要响应中断？PON以后会不会产生COMRESET中断？
    //后期如果确定不需要可以删除相关分支
    //暂时该入口是SATA状态机的最开始入口
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

    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed

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
}

// *****************************************************************************
// 函数名称: SATA_DSR_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的Software reset protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DSR->Device Software Reset
// *****************************************************************************    
void SATA_DSR_StateMachine(void)
{
    switch(tSataDC.unDevTransSts.eDSR)
    {
    case DSR0_SOFT_RESET_ASSERT:
        /*
        1.Make sure link is up
        2.Re-start and re-enable (as in Startup Enable)
        */
        /*if(DSATA_LinkCheck())
        {
        DSATA_StartupEnable(); 
        }*/
        break; 
    case DSR1_EXECUTE_DIAGNOSTICS: 
        //break;
    case DSR2_SEND_GOOD_STATUS:
        //break;
    case DSR3_SEND_BAD_STATUS:
        if(DSATA_LinkCheck())
        {
            NVIC_DisableIRQ(IRQ_SC);
#ifndef __BOOT_CODE__
            if(tSataDC.eSecState != ATA_SEC4)
            {
                SG_DSATA->rHWD2HEN.dAll=1;
            }
#endif
            SATA_ClearCmd();
            DSATA_StartupEnable();
#ifndef __BOOT_CODE__
            if(DPS_SLEEP == tSataDC.eDPMSts)
            {
                /*
                    Transition PM3:PM2:, A device shall transition to the PM2: Standby state after processing a hardware reset,
                    software reset, or DEVICE RESET command.
                */
                 tSataDC.eDPMSts=DPS_STANBY;
            }
#endif
            PRINT_TXT("D2H");
            SATA_SendGoodBadStatus(SATA_Execute_Diag(),0);//诊断并发送状态
            //切换状态
            SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
            NVIC_EnableIRQ(IRQ_SC);
        }
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DSR_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的Software reset protocol中断处理函数
// 入口参数: T_PISR* 指针，实际使用中会传入SATA中断寄存器
// 出口参数: 
// 说    明: DSR->Device Software Reset，此函数工作在中断模式
// *****************************************************************************    
void SATA_DSR_IntEventHandler(T_PISR *ptPISR)
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
#endif
    if(ptPISR->bits.HDRS)
    {
        //HDRS: Host to Device Register FIS Interrupt
        //SRESET状态下的RESET要自己处理，因为需要转到特定的状态去
        ptPISR->bits.HDRS=0;//清除中断，截获中断处理
        /*
        if a Register Host to Device FIS is received with the C bit in the FIS cleared
        to zero and the SRST bit cleared to zero in the Device Control register, the
        device shall transition to the DSR1: Execute_diagnostics state.
        */
        SATA_SReset_Handler();
    }
#if 0
    if(ptPISR->bits.UFS)
    {
        //UFS: Unknown FIS Interrupt

    }
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt

    }

    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed

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
}

// *****************************************************************************
// 函数名称: SATA_DEDD_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的EXECUTE DEVICE DIAGNOSTIC command
//           protocol状态机
// 入口参数: 
// 出口参数: 
// 说    明: DEDD->Device Execute Device Diagnostic
// *****************************************************************************    
void SATA_DEDD_StateMachine(void)
{

    switch(tSataDC.unDevTransSts.eDEDD)
    {
    case DEDD0_EXECUTE_DEVICE_DIAG:
        //break;
    case DEDD1_SEND_GOOD_STATUS:
        //break;
    case DEDD2_SEND_BAD_STATUS:
        UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
        NVIC_DisableIRQ(IRQ_SC);
        DSATA_ExceptionHandler(0);
        SATA_ClearCmd();
        DSATA_StartupEnable();
        SATA_SendGoodBadStatus(SATA_Execute_Diag(),1);//诊断并发送状态
        //切换状态
        SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE); 
        NVIC_EnableIRQ(IRQ_SC);
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DEDD_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的EXECUTE DEVICE DIAGNOSTIC command
//           protocol状态机
// 入口参数: T_PISR* 指针，实际使用中会传入SATA中断寄存器
// 出口参数: 
// 说    明: DEDD->Device Execute Device Diagnostic,此函数工作在中断模式
// *****************************************************************************    
void SATA_DEDD_IntEventHandler(T_PISR *ptPISR)
{
    //TODO :该状态下收到非RESET H2D FIS怎么办？
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

    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed

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
}
void SATA_StopBgActivity(void)
{
    tSataDC.bIsBGActivityOn=FALSE;
}
#if 0
// *****************************************************************************
// 函数名称: SATA_DDR_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的DEVICE RESET command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DDR->Device Device Reset
// *****************************************************************************    
void SATA_DDR_StateMachine(void)
{
    switch(tSataDC.unDevTransSts.eDDR)
    {
    case DDR0_DEVICE_RESET:
        //Stop processing and background activity.
        //TODO:终止后台任务
        stop_bg_activity();
        tSataDC.unDevTransSts.eDDR=DDR1_SEND_GOOD_STATUS;
        break;
    case DDR1_SEND_GOOD_STATUS:
        //后台任务已经停止了
       
        if(FALSE == tSataDC.bIsBGActivityOn)
        {
             NVIC_DisableIRQ(DSATA_IRQn);
            //DSATA_ClearFsmBuff();
            Mem32SetVal(&unFsmBuff.adBuff[0],0x00,7);
            unFsmBuff.FIS.d2h.dw0.bits.bType=FIS_CODE[D2H_FIS];
            unFsmBuff.FIS.d2h.dw0.bits.bError=0x01;
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low=0x01;
            unFsmBuff.FIS.d2h.dw3.bits.bCount=0x01;

            unFsmBuff.FIS.d2h.dw0.bits.bStatus=0x00;
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_mid=0x14;
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_high=0xEB;
            
            DSATA_TrigeSendFis(D2H_FIS);
            
            SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);
            NVIC_EnableIRQ(DSATA_IRQn);
        }  
                
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DDR_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的DEVICE RESET command protocol中断处理函数
// 入口参数: T_PISR* 指针，实际使用中会传入SATA中断寄存器
// 出口参数: 
// 说    明: DDR->Device Device Reset，此函数工作在中断模式
// *****************************************************************************    
void SATA_DDR_IntEventHandler(T_PISR *ptPISR)
{


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
        /*
        If the host asserts SRST in the Device Control register before the
        device has completed processing the DEVICE RESET protocol, then the device shall immediately
        start processing its software reset protocol from the beginning.
        */
        //HDRS: Host to Device Register FIS Interrupt
        SATA_SReset_Handler();
        //能够产生中断，如果不是SRST呢？
    }
    if(ptPISR->bits.UFS)
    {
        //UFS: Unknown FIS Interrupt

    }
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt

    }

    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed

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
}
#endif

// *****************************************************************************
// 函数名称: SATA_DND_DataTransferReq
// 函数功能: non data模式下ATA命令事件或者，FTL事件处理
// 入口参数: eType:请求类型

// 出口参数: NONE
// 说    明: DND->Device Non-Data
// *****************************************************************************
void SATA_DND_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t *pfTransferCompleteCb)
{     
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    if(REQ_STATUS == eType)
    {
        //根据传下来的状态回复相应的信息，ATA命令解析完成事件
        if(ptTask->eStatus < CMD_IDLE)
        {
            /*
                6.2.9 Error bit
                The Error bit shall be set to one if any bit in the Error field (see 6.3) is set to one. In ATA/ATAPI-7 this bit was
                documented as the ERR bit.
            */
             //！！部分HOST 某些历史命令不支持就不能顺利上盘
             SATA_SendStatus(0x51,(U8)(1<<ptTask->eStatus),1);

        }
        else
        {
            SATA_SendStatus(0x50,0x00,1); 
        }
    }
    else if(REQ_CONFIG == eType)
    {
        //有些命令需要等待FTL完成才能回复状态
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}  
// *****************************************************************************
// 函数名称: SATA_DND_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的Non-data command protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DND->Device Non-Data
// *****************************************************************************    
void SATA_DND_StateMachine(void)
{
    //上电之后最开始跑的状态
    switch(tSataDC.unDevTransSts.eDND)
    {
    case DND0_NON_DATA:
        break;
    case DND1_SEND_STATUS:
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DND_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的Non-data command protocol 状态机
// 入口参数: T_PISR* 指针，实际使用中会传入SATA中断寄存器
// 出口参数: 
// 说    明: DND->Device Non-Data，此函数工作在中断模式
// *****************************************************************************    
void SATA_DND_IntEventHandler(T_PISR *ptPISR)
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

    if(ptPISR->bits.DPS)
    {
        //DPS: Descriptor Processed

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
}
