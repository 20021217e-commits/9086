// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_fpdma.c 
//  文件标识：    
//  内容摘要：    ATA规范的通用命令实现
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2015年07月24日 
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************
#include "debug.h"
#include "ata_fpdma.h"
#include "sata_fpdma.h"
#include "sata_misc.h"
#include "dsata.h"
#include "ata_typedef.h"
#include "ata_identify.h"

#ifndef __BOOT_CODE__
#ifndef __FPDMA_DISABLE__
// *****************************************************************************
// 函数名称: ATACmd_FpdmaRwTaskComplete
// 函数功能: Fpdma读写数据任务完成事件处理
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_FpdmaRwTaskComplete(CMD_STATUS eStatus,void *ptTaskParam)
{
	//TODO:error handler
}

// *****************************************************************************
// 函数名称: ATACmd_GetFpdmaLba
// 函数功能: 通用处理过程
// 入口参数: tpH2d H2D fis 地址
//          ptTaskParam
// 出口参数: 命令有效性检查结果
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_GetFpdmaLba(T_H2D *tpH2d,T_HI_TASK_PARAM *ptTaskParam)
{
    //检查命令有效性   
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    if(!ptTaskParam->bIsUsed/*(SG_MCU->rSF0_INT1_STS0 & (1U<<ptTaskParam->bTag)) == 0*/)
    {

        if((tpH2d->dw2.bits.bExpLBA_mid !=0) ||
            (tpH2d->dw2.bits.bExpLBA_high !=0) )
        {
            //！！Error,LBA传入的地址大于2T,大于32bit位地址
            GPIO_DEBUG(0x44);
            return CMD_ID_NOT_FOUND;
        }
        ptTaskParam->dSctSize=tpH2d->dw0.bits.bFeatures_L + (tpH2d->dw2.bits.bFeatures_H<<8);
        
        if(0 == ptTaskParam->dSctSize)
        {
            ptTaskParam->dSctSize=65536;//ncq dma 模式不会涉及到PRDL 不够的情况
        }

//        lba=tSataDC.tCmdList[bIdx].h2d.dw1.bits.bLBA_low;
//        lba+=((U32)tSataDC.tCmdList[bIdx].h2d.dw1.bits.bLBA_mid<<8U);
//        lba+=((U32)tSataDC.tCmdList[bIdx].h2d.dw1.bits.bLBA_high<<16U);
        ptTaskParam->dLba=(tpH2d->dw1.dAll&0x00FFFFFF);//加速计算LBA
        ptTaskParam->dLba+=((U32)tpH2d->dw2.bits.bExpLBA_low<<24U);
        //32位LBA地址能包含2T的数据量，所以这里只使用低32位
        //lba+=((U64)tSataDC.tCmdList[bIdx].h2d.dw2.bits.bExpLBA_mid<<32U);
        //lba+=((U64)tSataDC.tCmdList[bIdx].h2d.dw2.bits.bExpLBA_high<<40U);
        if((ptTaskParam->dLba + ptTaskParam->dSctSize) > tSataDC.dMaxLbaAddr4User)
        {
            UART_PRINT("CMD_ID_NOT_FOUND!!!\r\n");
             //DMA暂时只支持一帧数据不超过4M
            return CMD_ID_NOT_FOUND;
        }
        else
        {
           // DEBUG_PRINT("dLba=%#010lx,dSctSize=%d\r\n",ptTaskParam->dLba,tTQC.ptTaskParam->dSctSize);
            return CMD_VALID;
        }

    }
    else
    {
        DEBUG_PRINT("Exit abort ,Tag=%d already exsist func=<%s>[file=%s,line=%d]\r\n",ptTaskParam->bTag,__func__,__MODULE__,__LINE__);
        //TAG已经被占用
        return CMD_ABORT;
    }

}
// *****************************************************************************
// 函数名称: ATACmd_RWFpdmaQueued
// 函数功能: 创建FPDMA任务公用代码
// 入口参数: bDir： 读写
// 出口参数: NONE
// 说    明: WRITE FPDMA QUEUED -61h, DMA Queued
// *****************************************************************************
static inline void ATACmd_RWFpdmaQueued(U8 bDir)
{
    UN_CH_DI unDI;
    U8 bTag=(unFsmBuff.FIS.h2d.dw3.bits.bCount>>3);
    T_HI_TASK *ptTask=&tTQC.tHiTasks[bTag];
    CMD_STATUS eStatus;

    if(bTag > (unAtaIdentify.wArray[75]&0x1F))
    {
        //对于NCQ max queue deepth 小于31的需要检查TAG 号
        eStatus=CMD_ABORT;
    }
    else
    {
        eStatus=ATACmd_GetFpdmaLba(&unFsmBuff.FIS.h2d,&ptTask->tParam);
    }

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
    tSataDC.bCmdIdx[bTag]=(tSataDC.bWrIdx-1)&0x1F;
#endif 
    AddCommandToQueue(eStatus);

    if(CMD_VALID == eStatus)
    {
        //READ 不能使能AUTO ACTIVE 功能否则接口不能传输数据
        unDI.dAll=0x100|bTag|(bDir<<6);
        
        if(bDir == DIR_OUT)
        unDI.dAll|=((FS_AUTO<<9));

        
        DEBUG_PRINT("Tag=%d\r\n",bTag);
        //0x100=
        //di.bits.F=1;
        //di.bits.A=0;
        //di.bits.D=DIR;
        //联动模式下PRDNUM=1
        DSATA_SetCmdHead((CMD_SLOT)bTag,
                          unDI,
                          ptTask->tParam.dSctSize,
                          ptTask->tParam.dSctSize,
                         (ptTask->tParam.dSctSize<<9));

        ptTask->tParam.bTag=bTag;
        ptTask->tParam.bDir=bDir;
        ptTask->tParam.bFua = (unFsmBuff.FIS.h2d.dw1.bits.bDevice>>7);
        ptTask->tParam.dBuffAddr=tSataDC.tSataMI.dDmaBuffAddr;
        
        TQM_CreateTask(OP_HI_DATA,ptTask,ATACmd_FpdmaRwTaskComplete);
    }
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
}
// *****************************************************************************
// 函数名称: ATACmd_WriteFpdmaQueued
// 函数功能: 实现ATA命令  WRITE FPDMA QUEUED
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE FPDMA QUEUED -61h, DMA Queued
// *****************************************************************************
void ATACmd_WriteFpdmaQueued(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    ATACmd_RWFpdmaQueued(DIR_OUT);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}


// *****************************************************************************
// 函数名称: ATACmd_WriteFpdmaQueued
// 函数功能: 实现ATA命令  READ FPDMA QUEUED
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ FPDMA QUEUED -60h, DMA Queued
// *****************************************************************************
void ATACmd_ReadFpdmaQueued(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    ATACmd_RWFpdmaQueued(DIR_IN);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

#endif //__FPDMA_DISABLE__
#endif //__BOOT_CODE__
