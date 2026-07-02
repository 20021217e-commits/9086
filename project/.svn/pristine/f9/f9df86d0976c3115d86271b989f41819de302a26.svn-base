// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_power_mg.c 
//  文件标识：    
//  内容摘要：    实现ATA8 Power Manege Feature Set管理 
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2015年08月04日 
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************

#include "ata_power_mg.h"
#include "sata_misc.h"
#include "debug.h"
#include "task_manage.h"
#include "dsata.h"
#include "uitility.h"
#include "ata_general.h"

// *****************************************************************************
// 函数名称: PowerMngEventDone
// 函数功能: 省电模式切换 任务结束
// 入口参数: NONE
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void PowerMngEventDone(CMD_STATUS eStatus,void *ptParam)
{
     //TODO:g0做一些自己相关的操作
     T_HI_TASK_PARAM *__ptParam=(T_HI_TASK_PARAM *)ptParam;
     tSataDC.eDPMSts=__ptParam->dBuffAddr;
     ATACmd_ReleaseCommand(eStatus);
     ERROR_PRINT("PowerMngEventDone..........\r\n");
}
// *****************************************************************************
// 函数名称: ATACmd_CheckPowerMode
// 函数功能: 实现ATA命令 CHECK POWER MODE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: CHECK POWER MODE – E5h, Non-Data
// *****************************************************************************
void ATACmd_CheckPowerMode(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    /*
      *
      * Value Description
        00h Device is in the:
        a) PM2:Standby state (see 4.15.4) and the EPC feature set (see 4.9) is not
        enabled; or
        b) PM2:Standby state, the EPC feature set is enabled, and the device is in the
        Standby_z power condition (see 4.9.2).
        01h Device is in the PM2:Standby state, the EPC feature set is enabled, and the device is
        in the Standby_y power condition (see 4.9.2).
        02h..3Fh Reserved
        40h..41h Obsolete
        42h..7Fh Reserved
        80h Device is in the
        a) PM1:Idle state (see 4.15.4) and EPC feature set is not supported; or
        b) PM1:Idle state and EPC feature set is supported and the EPC feature set is
        disabled.
        81h Device is in the PM1:Idle state, the EPC feature set is enable, and the device is in the
        Idle_a power condition (see 4.9.2).
        82h Device is in the PM1:Idle state, the EPC feature set is enabled, and the device is in
        the Idle_b power condition (see 4.9.2).
        83h Device is in the PM1:Idle state, the EPC feature set is enabled, and the device is in
        the Idle_c power condition (see 4.9.2).
        84h..FEh Reserved
        FFh Device is in the PM0:Active state or PM1:Idle state.
      * */

     if(tSataDC.eDPMSts == DPS_STANBY)
     {
         unFsmBuff.FIS.d2h.dw3.bits.bCount=0;
     }
     else if(tSataDC.eDPMSts == DPS_ACTIVE || tSataDC.eDPMSts == DPS_IDLE)
     {
         unFsmBuff.FIS.d2h.dw3.bits.bCount=0xFF;
     }

     /*
      *
      * If the LOW POWER STANDBY SUPPORTED bit is set to one (see A.11.5.2.36), then this field is as
        described in this table. Otherwise this field is N/A.
        Bit Description
        27:20 Device is waiting to enter a lower power condition:
        Value Description
        00h Standby_z
        01h Standby_y
        02h..80h Reserved
        81h Idle_a
        81h Idle_b
        82h Idle_c
        83h..FEh Reserved
        FFh Device is not waiting to enter a lower power condition
        19 Device is held in the current power condition
        18:0 Reserved
      * */
    ATACmd_ReleaseCommand(eStatus);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

CMD_STATUS UpdateStanbyTimerCount()
{
    CMD_STATUS eStatus=CMD_VALID;
    /*
     * COUNT field Description
        00h Standby timer disabled
        01h-F0h (value *5) seconds (i.e., 5 seconds to 1 200 seconds (i.e., 20
        minutes))
        F1h-FBh ((value – 240) * 30) minutes (i.e., 30 minutes to 330 minutes
        (i.e., 5.5 hours))
        FCh 21 minutes
        FDh Between 8 hours and 12 hours
        FEh Reserved
        FFh 21 minutes 15 seconds
        Note - Times are approximate.
     * */
#ifdef PHY_PARAM_TEST
    U8 type=(unFsmBuff.FIS.h2d.dw3.bits.bCount>>4);
    U8 val=(unFsmBuff.FIS.h2d.dw3.bits.bCount&0xF);
    PRINT_HEX("PARAM=",unFsmBuff.FIS.h2d.dw3.bits.bCount);
    if(type == 0)
    {
        //PLL 相关参数配置
        SG_PADCTRL->rPHY_XCFGA2=(SG_PADCTRL->rPHY_XCFGA2&0xFFFFFF0F);//清除原参数
        SG_PADCTRL->rPHY_XCFGA2|=(val<<4);
        PRINT_HEX("rPHY_XCFGA2=",SG_PADCTRL->rPHY_XCFGA2);
    }
    else if(type == 1)
    {
        //其他 相关参数
    }
    else
    {
        //其他 相关参数
    }
#else
#ifndef __BOOT_CODE__
    tSataDC.bIsStandyTimerOn = TRUE;
    if(unFsmBuff.FIS.h2d.dw3.bits.bCount == 0)
    {
        tSataDC.dStanbyTimerNum=0;
        tSataDC.bIsStandyTimerOn = FALSE;
        tSataDC.dStanbyTimerCnt=0;
    }
    else if(unFsmBuff.FIS.h2d.dw3.bits.bCount>=0x01 && unFsmBuff.FIS.h2d.dw3.bits.bCount<=0xF0)
    {
        tSataDC.dStanbyTimerNum=(unFsmBuff.FIS.h2d.dw3.bits.bCount*5/*秒*/);
    }
    else if(unFsmBuff.FIS.h2d.dw3.bits.bCount>=0xF1 && unFsmBuff.FIS.h2d.dw3.bits.bCount<=0xFB)
    {
        tSataDC.dStanbyTimerNum=((unFsmBuff.FIS.h2d.dw3.bits.bCount-240)*30/*分钟*/*60/*秒*/);
    }
    else if(unFsmBuff.FIS.h2d.dw3.bits.bCount == 0xFC)
    {
        tSataDC.dStanbyTimerNum=21/*分钟*/*60;
    }
    else if(unFsmBuff.FIS.h2d.dw3.bits.bCount == 0xFD)
    {
        tSataDC.dStanbyTimerNum=8/*小时*/*60/*分钟*/*60/*秒*/;
    }
    else if(unFsmBuff.FIS.h2d.dw3.bits.bCount == 0xFF)
    {
        tSataDC.dStanbyTimerNum=21/*分钟*/*60 + 15;
    }
    else //FE
    {
        //保留位
        tSataDC.dStanbyTimerNum=0;
        tSataDC.bIsStandyTimerOn = FALSE;
        tSataDC.dStanbyTimerCnt=0;
        eStatus=CMD_ABORT;
    }
#endif

    tSataDC.dStanbyTimerNum=tSataDC.dStanbyTimerNum*1000/*毫秒*/;
#endif
    return eStatus;
}
// *****************************************************************************
// 函数名称: ATACmd_Idle
// 函数功能: 实现ATA命令 IDLE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: IDLE – E3h, Non-Data
// *****************************************************************************
void ATACmd_Idle(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus =UpdateStanbyTimerCount();
    if(eStatus == CMD_VALID)
    {
        //    tSataDC.tSyncEvent.bOpcode=OP_HI_DPM_IDLE;
        //    tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
        //    tSataDC.tSyncEvent.dParam[0]=(U32)DPS_IDLE;
        //    tSataDC.tSyncEvent.pfEventCompleteCb=PowerMngEventDone;
        tTQC.tHiTasks[0].tParam.dBuffAddr =(U32)DPS_IDLE;
        //TQM_CreateTask(OP_HI_DPM_IDLE,&tTQC.tHiTasks[0],PowerMngEventDone);
		ATACmd_ReleaseCommand(CMD_IDLE);
    }
    else
    {
        ATACmd_ReleaseCommand(eStatus);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_IdleImmediate
// 函数功能: 实现ATA命令 IDLE IMMEDIATE
// 入口参数: NONE
// 出口参数: NONE
// 说    明:  IDLE IMMEDIATE – E1h, Non-Data
// *****************************************************************************
void ATACmd_IdleImmediate(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
//    tSataDC.tSyncEvent.bOpcode=OP_HI_DPM_IDLE;
//    tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
//    tSataDC.tSyncEvent.dParam[0]=(U32)DPS_IDLE;
//    tSataDC.tSyncEvent.pfEventCompleteCb=PowerMngEventDone;

    tTQC.tHiTasks[0].tParam.dBuffAddr =(U32)DPS_IDLE;
    //TQM_CreateTask(OP_HI_DPM_IDLE,&tTQC.tHiTasks[0],PowerMngEventDone);
	ATACmd_ReleaseCommand(CMD_IDLE);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_Sleep
// 函数功能: 实现ATA命令 SLEEP
// 入口参数: NONE
// 出口参数: NONE
// 说    明: E6h, Non-Data
// *****************************************************************************
void ATACmd_Sleep(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
//    tSataDC.tSyncEvent.bOpcode=OP_HI_DPM_SLEEP;
//    tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
//    tSataDC.tSyncEvent.dParam[0]=(U32)DPS_SLEEP;
//    tSataDC.tSyncEvent.pfEventCompleteCb=PowerMngEventDone;
    SG_DSATA->rHWD2HEN.dAll=0;//sleep 状态下所有的命令都不接受
    tTQC.tHiTasks[0].tParam.dBuffAddr =(U32)DPS_SLEEP;
    //TQM_CreateTask(OP_HI_DPM_SLEEP,&tTQC.tHiTasks[0],PowerMngEventDone);
	ATACmd_ReleaseCommand(CMD_IDLE);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_Standby
// 函数功能: 实现ATA命令 STANBY
// 入口参数: NONE
// 出口参数: NONE
// 说    明: STANDBY – E2h, Non-Data
// *****************************************************************************
void ATACmd_Standby(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    eStatus =UpdateStanbyTimerCount();
    if(eStatus == CMD_VALID)
    {
//		tSataDC.tSyncEvent.bOpcode=OP_HI_DPM_STANBY;
//		tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
//		tSataDC.tSyncEvent.dParam[0]=(U32)DPS_STANBY;
//		tSataDC.tSyncEvent.pfEventCompleteCb=PowerMngEventDone;
        tTQC.tHiTasks[0].tParam.dBuffAddr =(U32)DPS_STANBY;
        //TQM_CreateTask(OP_HI_DPM_STANBY,&tTQC.tHiTasks[0],PowerMngEventDone);
		ATACmd_ReleaseCommand(eStatus);
    }
    else
    {
        ATACmd_ReleaseCommand(eStatus);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_StandbyImmediate
// 函数功能: 实现ATA命令 STANBY IMMEDIATE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: STANDBY IMMEDIATE – E0h, Non-Data
// *****************************************************************************
void ATACmd_StandbyImmediate(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
//    tSataDC.tSyncEvent.bOpcode=OP_HI_DPM_STANBY;
//    tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
//    tSataDC.tSyncEvent.dParam[0]=(U32)DPS_STANBY;
//    tSataDC.tSyncEvent.pfEventCompleteCb=PowerMngEventDone;
    tTQC.tHiTasks[0].tParam.dBuffAddr =(U32)DPS_STANBY;
    //TQM_CreateTask(OP_HI_DPM_STANBY,&tTQC.tHiTasks[0],PowerMngEventDone);
	ATACmd_ReleaseCommand(CMD_IDLE);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
