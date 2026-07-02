// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司
//   
//  文件名称：    ata_smart.c
//  文件标识：
//  内容摘要：    实现ATA8 SMART Feature Set管理
//  其它说明：
//  项    目：    SG9081
//  作    者：    WILLIAM
//  完成日期：    2015年07月07日
//   
//  修改记录 1：
//        修改日期：
//        版 本 号：
//        修 改 人：
//        修改内容：
//  修改记录 2：
// **********************************************************************
#include "ata_typedef.h"
#include "ata_smart.h"
#include "sata_pio_in.h"
#include "sata_pio_out.h"
#include "sata_dma_in.h"
#include "sata_dma_out.h"
#include "sata_misc.h"
#include "ata_general.h"
#include "dsata.h"
#include "debug.h"
#include "task_manage.h"
#include "ata_gpl.h"
#include "uitility.h"
#include "ata_identify.h"
#if (SATA_LOG_ENABLE == TRUE)
// *****************************************************************************
// 函数名称: ATACmd_ReadNativeMaxAddr
// 函数功能: 实现ATA命令 READ NATIVE MAX ADDRESS
// 入口参数: NONE
// 出口参数:
// 说    明: READ NATIVE MAX ADDRESS - F8h, Non-Data
// *****************************************************************************
void ATACmd_ReadNativeMaxAddr(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

	/*If the 48-bit Address feature set is supported and the 48-bit native max
      address is greater than 268,435,455, the READ NATIVE MAX ADDRESS
      command shall return a maximum value of 268,435,454（FFFFFFE）*/
	tSataDC.eDPMSts=DPS_ACTIVE;
    tSataDC.dStanbyTimerCnt=0;
	
    U32 dCap=tSataDC.dDevCap;
    if(tSataDC.dDevCap > 0xFFFFFFF)
    {
    	//28BIT 可以支持的最大值
    	dCap=0xFFFFFFF;
    }

    unFsmBuff.FIS.d2h.dw1.dAll &= 0xFF000000;
    unFsmBuff.FIS.d2h.dw1.dAll |= (dCap & 0x00FFFFFF) - 1;//note:LBA 的最大值是容量-1
    unFsmBuff.FIS.d2h.dw1.bits.bDevice &= 0xF0;
    unFsmBuff.FIS.d2h.dw1.bits.bDevice |= (dCap>>24);
    ATACmd_ReleaseCommand(eStatus);

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_ReadNativeMaxAddrExt
// 函数功能: 实现ATA命令 READ NATIVE MAX ADDRESS
// 入口参数: NONE
// 出口参数:
// 说    明: READ NATIVE MAX ADDRESS - 27h, Non-Data
// *****************************************************************************
void ATACmd_ReadNativeMaxAddrExt(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    tSataDC.eDPMSts=DPS_ACTIVE;
    tSataDC.dStanbyTimerCnt=0;
	
    unFsmBuff.FIS.d2h.dw1.dAll &= 0xFF000000;
    unFsmBuff.FIS.d2h.dw1.dAll |= (tSataDC.dDevCap & 0x00FFFFFF) - 1;//note:LBA 的最大值是容量-1
    unFsmBuff.FIS.d2h.dw2.bits.bExpLBA_low = ((tSataDC.dDevCap & 0xFF000000)>>24);
    ATACmd_ReleaseCommand(eStatus);

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

CMD_STATUS ATACmd_Hpa_0x04_SetMaxFreezeLock()
{
    return CMD_VALID;
}
//SET MAX LOCK - F9h/02h, Non-Data
CMD_STATUS ATACmd_Hpa_0x02_SetMaxLock()
{
    return CMD_VALID;
}

//SET MAX SET PASSWORD - F9h/01h, PIO Data-Out
CMD_STATUS ATACmd_Hpa_0x01_SetPassword()
{
    return CMD_VALID;
}

//SET MAX SET PASSWORD DMA - F9h/05h, PIO Data-Out
CMD_STATUS ATACmd_Hpa_0x05_SetPasswordDma()
{
    return CMD_VALID;
}

//SET MAX UNLOCK - F9h/03h, PIO Data-Out
CMD_STATUS ATACmd_Hpa_0x03_UnLock()
{
    return CMD_VALID;
}

//SET MAX UNLOCK DMA - F9h/06h, DMA
CMD_STATUS ATACmd_Hpa_0x06_UnLockDma()
{
    return CMD_VALID;
}

// *****************************************************************************
// 函数名称: ATACmd_SetMaxAddr
// 函数功能: 实现ATA命令 SET MAX addr
// 入口参数: NONE
// 出口参数:
// 说    明: SET MAX 0xF9
// *****************************************************************************
CMD_STATUS ATACmd_SetMaxAddr(void)
{
    return CMD_VALID;
}

// *****************************************************************************
// 函数名称: ATACmd_SetMax
// 函数功能: 实现ATA命令 SET MAX
// 入口参数: NONE
// 出口参数:
// 说    明: SET MAX 0xF9
// *****************************************************************************
void ATACmd_SetMax(void)
{
    CMD_STATUS eStatus=CMD_DONE;
    T_HI_TASK tTask;

    REQ_TYPE eType=REQ_STATUS;
    TransferCompleteCb_t *pfFunc=NULL;
    tTask.tParam.bTag=CMD_SLOT0;

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    ERROR_PRINT("SetMax SUB CMD=%d\r\n",unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L);

#if(SATA_DCO_ENABLE == TRUE)
    //如果被改成不支持SMART 命令返回error
    if(!DCO_HPA)
    {
        SATA_ChangeState(NONE_DATA,0);
        ATACmd_ReleaseCommand(CMD_ABORT);
        return;
    }
#endif

    switch(unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L)
    {
        case 0x04:
            //SET MAX FREEZE LOCK – F9h/04h, Non-Data
            eStatus=ATACmd_Hpa_0x04_SetMaxFreezeLock();
            break;
        case 0x02:
            //SET MAX LOCK - F9h/02h, Non-Data
            eStatus=ATACmd_Hpa_0x02_SetMaxLock();
            break;
        case 0x01:
            //SET MAX SET PASSWORD - F9h/01h, PIO Data-Out
            if(CMD_DONE == ATACmd_Hpa_0x01_SetPassword())
            return;//这个模式不需要出去发送状态
        case 0x05:
            //SET MAX SET PASSWORD DMA - F9h/05h, DMA
            if(CMD_DONE == ATACmd_Hpa_0x05_SetPasswordDma())
            return;//这个模式不需要出去发送状态
        case 0x03:
            //SET MAX UNLOCK - F9h/03h, PIO Data-Out
            if(CMD_DONE == ATACmd_Hpa_0x03_UnLock())
            return;//这个模式不需要出去发送状态
        case 0x06:
            //SET MAX UNLOCK DMA - F9h/06h, DMA
            if(CMD_DONE == ATACmd_Hpa_0x06_UnLockDma())
            return;//这个模式不需要出去发送状态
        default:
            // SET MAX ADDRESS - F9h, Non-Data
            eStatus=ATACmd_SetMaxAddr();
            break;
    }
    tTask.eStatus=eStatus;
    tSataDC.DOS_TransferReq(eType,&tTask,pfFunc);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SetMaxAddrExt
// 函数功能: 实现ATA命令 SET MAX addr extern
// 入口参数: NONE
// 出口参数:
// 说    明: SET MAX 0xF9
// *****************************************************************************
void ATACmd_SetMaxAddrExt(void)
{

}
#endif
