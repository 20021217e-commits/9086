// ********************************************************************* 
//  版权所有  (C) 2017,  硅格半导体有限公司
//   
//  文件名称：    ata_cfa.c
//  文件标识：    
//  内容摘要：    实现ATA8 CFA管理
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2017年06月29日
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************

#include "ata_cfa.h"
#include "sata_misc.h"
#include "ata_general.h"
#include "debug.h"
#include "task_manage.h"
#include "dsata.h"
#include "uitility.h"

#if(SATA_DCO_ENABLE == TRUE)
void EraseSectorsDone(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_NONE_EVENT;
    __pevent->pfEventCompleteCb=NULL;
    ATACmd_ReleaseCommand(eStatus);
}

// *****************************************************************************
// 函数名称: ATACmd_CfaEraseSectors
// 函数功能: 实现ATA命令  CFA ERASE SECTORS
// 入口参数: NONE
// 出口参数: NONE
// 说    明: CFA ERASE SECTORS - C0h, Non-Data
// *****************************************************************************
void ATACmd_CFAEraseSectors(void)
{ 	CMD_STATUS eStatus;
    eStatus=ATACmd_GetCountLba28(&unFsmBuff.FIS.h2d);
    if(eStatus < CMD_IDLE)
    {
        ATACmd_ReleaseCommand(CMD_ABORT);
    }
    else
    {
        tSataDC.tSyncEvent.bOpcode=OP_DI_SANITIZE_DEVICE;
        tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tSyncEvent.dParam[1]=tTQC.tHiTasks[0].tParam.dLba;
        tSataDC.tSyncEvent.dParam[2]=tTQC.tHiTasks[0].tParam.dSctSize;
        tSataDC.tSyncEvent.pfEventCompleteCb=EraseSectorsDone;
    }
}

// *****************************************************************************
// 函数名称: ATACmd_CFARequstExtErrCode
// 函数功能: 实现ATA命令 CFA REQUEST EXTENDED ERROR CODE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: CFA REQUEST EXTENDED ERROR CODE - 03h, Non-Data
// *****************************************************************************
void ATACmd_CFARequstExtErrCode(void)
{
    /*
     * Extended error code Description
    00h No error detected / no additional information
    01h Self-test passed
    02h Reserved
    03h Write / Erase failed
    04h Reserved
    05h Self-test or diagnostic failed
    06h-08h Reserved
    09h Miscellaneous error
    0Ah Reserved
    0Bh Vendor specific
    0Ch Corrupted media format
    0Dh-0Fh Vendor specific
    10h ID Not Found / ID Error
    11h Uncorrectable ECC error
     * */
    ATACmd_ReleaseCommand(CMD_DONE);
}

// *****************************************************************************
// 函数名称: ATACmd_CFATranslateSector
// 函数功能: 实现ATA命令 CFA TRANSLATE SECTOR
// 入口参数: NONE
// 出口参数: NONE
// 说    明:  CFA TRANSLATE SECTOR - 87h, PIO Data-In
// *****************************************************************************
void ATACmd_CFATranslateSector(void)
{

}

// *****************************************************************************
// 函数名称: ATACmd_CFAWriteMultiWOErase
// 函数功能: 实现ATA命令 CFA WRITE MULTIPLE WITHOUT ERASE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: CFA WRITE MULTIPLE WITHOUT ERASE - CDh, PIO Data-Out
// *****************************************************************************
void ATACmd_CFAWriteMultiWOErase(void)
{

}

// *****************************************************************************
// 函数名称: ATACmd_CFAWriteSectorsWOErase
// 函数功能: 实现ATA命令 CFA WRITE SECTORS WITHOUT ERASE
// 入口参数: NONE
// 出口参数: NONE
// 说    明:  CFA WRITE SECTORS WITHOUT ERASE - 38h, PIO Data-Out
// *****************************************************************************
void ATACmd_CFAWriteSectorsWOErase(void)
{

}
#endif
