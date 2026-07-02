// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  sata_dev_idle.c
//  文件标识：  
//  内容摘要：  实现sata_dev_idle传输状态机 
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月22日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "sata_dev_idle.h"
#include "dsata.h"
#include "sata_misc.h"
#include "ata_cmd_list.h"
#include "sata_fpdma.h"
#include "debug.h"
#include "scsi.h"
#include "ata_identify.h"

// *****************************************************************************
// 函数名称: di0_device_idle
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************    
static inline void di0_device_idle()
{
   //IDLE mode do nothing
#if (L_PWN_MNG_EN == TRUE)
#if(L_DIPM_EN == TRUE)
    static U16 wCount_dipm=0;
    GIE_DISABLE();
    if((tSataDC.bIsDIPMEn==TRUE)					    &&
       //(tSataDC.eLPMSts == LPS_ACTIVE) 				    &&
       (tSataDC.bCmdCnt == 0)							&&
       (tSataDC.tSyncEvent.bOpcode == OP_NONE_EVENT)	&&
       (tSataDC.tAsyncEvent.bOpcode == OP_NONE_EVENT)	&&
       (tSataDC.bIsBGActivityOn == FALSE) 				&&
       (SG_DSATA->rHWD2HDONE0 == 0)                     &&
       (SG_DSATA->rRECVTAG == 0))
    {
          if(tSataDC.eLPMSts != LPS_ACTIVE)
          {
              //active 状态下才需要使能
              //LowSpeedModeEn();
          }
          else
          {
              if(SG_DSATA->rPSCTLR.bits.ITIMER_EN==0)
              {
                  if(wCount_dipm >= 3)
                  {
                      wCount_dipm=0;

                      //进入IDLE模以后，如果之前已经打开DPIM 则继续使能
                      SG_DSATA->rPSCTLR.bits.ITIMER_PM=LPM_PARTIAL;//TODO:控制省电级别
                      if(SG_DSATA->rPCMDR.bits.APSTE==1)
                      {
                        SG_CPM->rPAWCTRL.bits.PAWEN =0;
                      }
                      else
                      {
                        SG_CPM->rPAWCTRL.bits.PAWEN =1;//使能重新激活
                      }

                      SG_DSATA->rPSCTLR.bits.ITIMER_EN=1;//保证系统处于空闲状态进入休眠模式
                      ERROR_PRINT("Enable DIPM ITIMER_EN....\r\n");
                  }
                  else
                  {
                      wCount_dipm++;
                  }
              }
          }
    }
    else
    {
        wCount_dipm=0;
        //SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;//应对FTL随机发起的后台任务,不停时钟不需要禁用这个func
    }
    GIE_ENABLE();
#endif//#if(L_DIPM_EN == TRUE)

//#if(L_HIPM_EN == TRUE && PWM_CLK_GATING_EN == TRUE)
//        static U16 wCount_hipm=0;
//        if((tSataDC.bIsHIPMEn==TRUE)					&&
//         //(tSataDC.eLPMSts == LPS_ACTIVE) 				&&
//         (tSataDC.bCmdCnt == 0)							&&
//         (tSataDC.tSyncEvent.bOpcode == OP_NONE_EVENT)	&&
//         (tSataDC.tAsyncEvent.bOpcode == OP_NONE_EVENT)	&&
//         (tSataDC.bIsBGActivityOn == FALSE) 			&&
//         (SG_DSATA->rRECVTAG == 0))
//          {
//             if(SG_DSATA->rPSCTLR.bits.IPM==LPM_DISABLE_BOTH)
//             {
//                 if(wCount_hipm >= 3)
//                 {
//                     wCount_hipm=0;
//                     SG_DSATA->rPSCTLR.bits.IPM = LPM_ENABLE_BOTH;//启用进入HOST发起的省电模式
//                 }
//                 else
//                 {
//                     wCount_hipm++;
//                 }
//             }
//
//          }
//        else
//        {
//            SG_DSATA->rPSCTLR.bits.IPM=LPM_DISABLE_BOTH;
//            wCount_hipm=0;
//        }
//#endif//#if(L_HIPM_EN == TRUE)

#endif

}

// *****************************************************************************
// 函数名称: di3_no_cmd
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************
static inline void di3_no_cmd(void)
{
     DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //Request transmission of Register Device to Host FIS with ABRT bit set to one.
    /*
    If in this state, the device shall request that the Transport layer transmit a Register Device to Host
    FIS with register content as described in the command description in the ACS-3 standard and
    the Interrupt bit set to one. --SATA3.2 P562
    */
    DSATA_EnableRxFis();//重新使能接收
#ifndef __BOOT_CODE__
	DSATA_SendD2HFis(0x50,0x00,1);//接受命令
//  DSATA_SendD2HFis(0x51,0x04,1);//拒绝命令
#else
    DSATA_SendD2HFis(0x50,0x00,1); //不属于SSD的特性的 NONE-DATA命令直接默认支持????-SMI 2258xt
#endif
    tSataDC.unDevTransSts.eDI=DI0_DEVICE_IDLE; 
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}


// *****************************************************************************
// 函数名称: di2_check_cmd
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************    
static inline void di2_check_cmd()
{
    T_ATA_CMD* pCmd;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    pCmd = ATACmd_ParseCmd(unFsmBuff.FIS.h2d.dw0.bits.Cmd);
    
    if(tSataDC.eDPMSts == DPS_SLEEP)
    {
        /*
         * A device transitions from Sleep mode only after processing a hardware reset, a software reset, or a DEVICE
            RESET command. Processing a hardware reset, a software reset, or a DEVICE RESET command may take a
            long time (e.g., 30 seconds). Sleep mode provides the lowest power consumption of any mode.
         * */
        return;
    }

    if(pCmd != NULL)
    {
        tSataDC.bATACmd = pCmd->bCmd;
//        ERROR_PRINT("ATACmd=%x\r\n",tSataDC.bATACmd);
        //if(tSataDC.bATACmd != 0x25 && tSataDC.bATACmd != 0x35 && tSataDC.bATACmd != 0xc8 && tSataDC.bATACmd != 0xca)
        //PRINT_HEX("ATACmd=",tSataDC.bATACmd);
#if(SATA_LOG_ENABLE == TRUE)

#if(SATA_SEC_ENABLE == TRUE)
        if(tSataDC.eSecState == ATA_SEC4)
        {
            //存在用户密码，则先需要校验密码成功以后才可以开放读写命令
            if(pCmd->enIsLocked)
            {
                DSATA_EnableRxFis();//重新使能接收
                DSATA_SendD2HFis(0x51,0x04,1);; //拒绝命令执行
                return;
            }
        }
#endif
#if(SATA_SD_ENABLE == TRUE)
        if(tSataDC.tSDState == S_SD2)
        {
            //S_SD2状态下，只允许执行部分命令
            /*
             *
             * After a device has started processing a Sanitize operation and until the device transitions to the SD0: Sanitize
                Idle state (see 4.17.9.2), the device shall abort all commands other than the:
                a) IDENTIFY DEVICE command;
                b) IDLE IMMEDIATE command with UNLOAD;
                c) READ LOG EXT command or READ LOG DMA EXT command if one of the following log addresses is
                requested:
                A) 10h (i.e., NCQ Command Error log);
                B) 30h (i.e., IDENTIFY DEVICE data log); or
                C) E0h (i.e., SCT Command/Status log);
                d) REQUEST SENSE DATA EXT command;
                e) SANITIZE ANTIFREEZE LOCK EXT command;
                f) SANITIZE STATUS EXT command;
                g) SECURITY UNLOCK command;
                h) SET FEATURES PUIS feature set device spin-up subcommand;
                i) SMART READ LOG command if one of the following log addresses is requested:
                A) 30h (i.e., IDENTIFY DEVICE data log); or
                B) E0h (i.e., SCT Command/Status log);
                j) SMART RETURN STATUS command; and
                k) supported sanitize operations commands (see 4.17.4), if the device is in the SD3: Sanitize Operation
                Failed state (see 4.17.9.5) or the SD4: Sanitize Operation Succeeded state (see 4.17.9.6
             * */
            if((tSataDC.bATACmd == 0xEC) ||
               (tSataDC.bATACmd == 0xB4) ||
               (tSataDC.bATACmd == 0xE1) ||
               ((tSataDC.bATACmd == 0x2F || tSataDC.bATACmd == 0x47) &&
                (unFsmBuff.FIS.h2d.dw1.bits.bLBA_low ==0x10 ||
                 unFsmBuff.FIS.h2d.dw1.bits.bLBA_low ==0x30 ||
                 unFsmBuff.FIS.h2d.dw1.bits.bLBA_low ==0xe0)) ||
               (tSataDC.bATACmd == 0xF2) ||
               ((tSataDC.bATACmd == 0xB0 &&
                 unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L == 0xD5 &&
                (unFsmBuff.FIS.h2d.dw1.bits.bLBA_low == 0x30 ||
                 unFsmBuff.FIS.h2d.dw1.bits.bLBA_low == 0xE0))) ||
               (tSataDC.bATACmd == 0xB0 && unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L == 0xDA))
            {

            }
            else
            {
                DSATA_EnableRxFis();//重新使能接收
                DSATA_SendD2HFis(0x51,0x04,1);; //拒绝命令执行
                return;
            }
        }
#endif

#endif//SATA_LOG_ENABLE

#ifndef __FPDMA_DISABLE__
        //命令存在
        if(FPDMA_QUEUED == pCmd->eCmdTrspType)
        {
            //UART_PRINT("pCmd->eCmdTrspType = FPDMA_QUEUED\r\n");
            /*13. FPDMA QUEUED command protocol.*/
            //更改传输状态机

#if(SATA_DCO_ENABLE == TRUE)
    //如果被改成不支持ncq 命令返回error
    if(!FS_NCQ)
    {
        ERROR_PRINT("Error NCQ NOT SUPPORT!!!!!\r\n");
        DSATA_EnableRxFis();//重新使能接收
        DSATA_SendD2HFis(0x51,0x04,1);; //当做普通的异常处理
        return;
    }
#endif
            SATA_SaveH2DFis(pCmd->eCmdTrspType,&unFsmBuff);
            SATA_ChangeState(FPDMA_QUEUED,DFPDMAQ1_AddCommandToQueue);    
            pCmd->pfCmdHandler();
        }
        else
        {
            DEBUG_PRINT("pCmd->eCmdTrspType =NONE - FPDMA_QUEUED\r\n");
            if(TRUE == tSataDC.bIsNcqCmd)
            {
                DEBUG_PRINT("tSataDC.bIsNcqCmd =TRUE\r\n");
                /*
                14. Not READ FPDMA QUEUED and not WRITE FPDMA QUEUED and not NCQ NON-DATA and not RECEIVE
                FPDMA QUEUED and not SEND FPDMA QUEUED and not DEVICE RESET and native queued command(s)Outstanding
                ->DFPDMAQ12:BrokenHost_ClearBusy
                */
                //更改传输状态机
#if (SATA_LOG_ENABLE == 1)
                tSataDC.tSyncEvent.bNQFlg=TRUE;
                tSataDC.tSyncEvent.Tag= DSATA_GetCurrentCmdSlot();
#endif
                SATA_ChangeState(FPDMA_QUEUED,DFPDMAQ12_BrokenHost_ClearBusy);            
            }
            else
#endif
            {
                /*
                1. Non-data command protocol and no native queued command outstanding.-> DND0: Non-data
                2. PIO data-in command protocol and no native queuedcommand outstanding.-> DPIOI0: PIO_in
                3. PIO data-out command protocol and no native queued command outstanding.->DPIOO0: PIO_out
                4. READ DMA command protocol and no native queued command outstanding.->DDMAI0: DMA_in
                5. WRITE DMA command protocol and no native queued command outstanding.->DDMAO0: DMA_out
                6. PACKET command protocol and no native queuedcommand outstanding.->DPKT0: PACKET
                7. READ DMA QUEUED command protocol and no native queued command outstanding.->DDMAQI0:DMA_queued_in
                8. WRITE DMA QUEUED command protocol and no native queued command outstanding. ->DDMAQO0:DMA_queued_out
                9. EXECUTE DEVICE DIAGNOSTIC command protocol and no native queued command outstanding.->DEDD0:Execute_device_diag
                10. DEVICE RESET command protocol. ->DDR0: Device_reset
                */
                DEBUG_PRINT("tSataDC.bIsNcqCmd =FALSE\r\n");
                SATA_ChangeState(pCmd->eCmdTrspType,0);//更改传输状态机?

                DEBUG_PRINT("tSataDC.bCmdCnt =%u\r\n",tSataDC.bCmdCnt);
                if(tSataDC.bCmdCnt != 0)
                {
                    GPIO_DEBUG(0x93);
                    //非NCQ模式下出现这种情况，说明FW没有及时清理上一条命令信息的上下文。 这样RdIdx还是指向前一个任务，是有问题的
                    SATA_SendStatus(0x51,0x04,1);
                    SATA_ClearCmd();
                    PRINT_TXT("E:CMDABORT");//重新初始化命令管理队列
                    tSataDC.bSaveUartBuff=TRUE;//记录异常信息到LOG页面
                }
                else
                {
                    SATA_SaveH2DFis(pCmd->eCmdTrspType,&unFsmBuff);
                    pCmd->pfCmdHandler();   
                }                    
            }
#ifndef __FPDMA_DISABLE__
        }
#endif
    }
    else
    {
        GPIO_DEBUG(0x72);
        /*11. Command not implemented and no native queued command outstanding.-> DI0: Device_idle*/
        if(FALSE == tSataDC.bIsNcqCmd)
        {
            //tSataDC.unDevTransSts.eDI=DI3_NO_CMD;
            DEBUG_PRINT("Command is not implemented!!\r\n");
            di3_no_cmd();//加速
        }
    }
     DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: di1_check_fis
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************    
static inline void di1_check_fis()
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if (SATA_SReset_Handler() == FALSE)
    {
#ifdef   	H2D_DMASETUP_TEST
    	SG_DSATA->rPISR.dAll=HDRS_BIT;//H2D中断会卡主DMA SETUP流程
#endif
        GPIO_DEBUG(0x71);
        //不是reset信号
        /*3. Register type and C bit set to one.*/
        //TODO:初始版本按照规范定义来，后面可能需要合并DI2_CHECK_CMD加速处理
        //tSataDC.unDevTransSts.eDI=DI2_CHECK_CMD;
        di2_check_cmd();
    }
     DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}



// *****************************************************************************
// 函数名称: di4_set_serice
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************
inline void di4_set_serice(void)
{
}
// *****************************************************************************
// 函数名称: di5_serice_test
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************
inline void di5_serice_test(void)
{
    /*
    PACKET PIO data-in or PACKET PIO data-out.
    */
    //TODO:实现上面的传输协议
}
// *****************************************************************************
// 函数名称: di6_service_send_tag
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************
inline void di6_service_send_tag(void)
{
}
// *****************************************************************************
// 函数名称: di7_service_decode
// 函数功能: 子过程
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************
inline void di7_service_decode(void)
{
    /*
    1. PACKET PIO data-in.
    2. PACKET PIO data-out.
    3. PACKET DMA data-in.
    4. PACKET DMA data-out.
    5. READ DMA QUEUED. 
    6. WRITE DMA QUEUED.
    */
}
// *****************************************************************************
// 函数名称: SATA_DI_StateMachine
// 函数功能: 实现SATA设备命令传输协议中的Device idle protocol 状态机
// 入口参数: 
// 出口参数: 
// 说    明: DI->Device Idle
// *****************************************************************************    
void SATA_DI_StateMachine(void)
{

    switch(tSataDC.unDevTransSts.eDI)
    {
    case DI0_DEVICE_IDLE:
        //等待接收HOST TO DEVICE FIS
        //TODO:设备电源管理？
        di0_device_idle();
        break;
        //在中断中执行下面三个状态
    case DI1_CHECK_FIS:
        //di1_check_fis();
        break;
    case DI2_CHECK_CMD:
        //di2_check_cmd();
        break;
    case DI3_NO_CMD:
        //di3_no_cmd();
        break;    
    case DI4_SET_SERVICE:
        //di4_set_serice();
        break;
    case DI5_SERVICE_TEST:
        //di5_serice_test();
        break;
    case DI6_SERVICE_SEND_TAG:
        //di6_service_send_tag();
        break;
    case DI7_SERVICE_DECODE:
        //di7_service_decode();
        break;
    default:
        //!!Error,不可能出现这种情况
        break;
    }
}

// *****************************************************************************
// 函数名称: SATA_DI_IntEventHandler
// 函数功能: 实现SATA设备命令传输协议中的Device idle protocol状态中断处理函数
// 入口参数: void* 指针，实际使用中会传入SATA设备中断寄存器内容
// 出口参数: 
// 说    明: DI->Device Idle，此函数工作在中断模式
// *****************************************************************************    
void SATA_DI_IntEventHandler(T_PISR *ptPISR)
{
     DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
     GPIO_DEBUG(0x69);
     GPIO_DEBUGDW(ptPISR->dAll);
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
        //pfm_restart();
        DEBUG_PRINT("ENTER HDRS: Host to Device Register FIS Interrupt\r\n");
        //HDRS: Host to Device Register FIS Interrupt
        ptPISR->bits.HDRS=0;
        GPIO_DEBUG(0x70);
        /*1. FIS receipt -> DI1: Check_FIS*/
        //tSataDC.unDevTransSts.eDI=DI1_CHECK_FIS;
#ifdef SATA_RECNT_ENABLE
        tSataDC.bFirstH2dCmd = 0xAA;
#endif
        di1_check_fis();
    }

    if(ptPISR->bits.UFS)//suke 和HDRS应该不会同时存在
    {
        //UFS: Unknown FIS Interrupt
        ptPISR->bits.UFS=0;
        /*5.Unexpected FIS type.*/
        tSataDC.unDevTransSts.eDI=DI0_DEVICE_IDLE;
    }

#if 0
    if(ptPISR->bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt
        ptPISR->bits.FTXDS=0;
        DSATA_DisableAutoSend();
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
     DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

