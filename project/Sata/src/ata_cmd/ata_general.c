// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_general.c 
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
#include "dsata.h"
#include "ata_typedef.h"
#include "ata_general.h"
#include "ata_identify.h"
#include "sata_pio_in.h"
#include "sata_pio_out.h"
#include "sata_dma_in.h"
#include "sata_dma_out.h"
#include "sata_misc.h"
#include "task_manage.h"
#include "uitility.h"
#include "ata_gpl.h"
#include "ata_security.h"

// *****************************************************************************
// 函数名称: ATACmd_Chs2Lba
// 函数功能: 28bit命令下的CHS to LBA模式转换
// 入口参数: T_H2D *tpH2d H2D FIS
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Chs2Lba(T_H2D *tpH2d)
{
    /*
     *
     * -  All devices shall support LBA translation.
        -  If the device’s capacity is greater than or equal to one sector and less than or equal to
        16,514,064 sectors the device shall support CHS translation.
        -  If the device’s capacity is greater than 16,514,064 sectors, then the device may support CHS
        translation.
        -  If a device supports CHS translation, then, following a power-on or hardware reset, the CHS
        translation enabled by the device shall be known as the default translation.
        -  If a device supports CHS translation, then a device may allow a host to use the INITIALIZE
        DEVICE PARAMETERS command to select other CHS translations.
        -  If a device supports CHS translation, then IDENTIFY DEVICE words 1,3, and 6 shall describe
        the default translation, and words 53-58 shall describe the current translation
        --ATA/ATAPI-4 P35
     * */
    //CHS 2 LBA
    U16 wCyl_num=(tpH2d->dw1.bits.bLBA_high<<8)+tpH2d->dw1.bits.bLBA_mid;
    U8 bSec_num=tpH2d->dw1.bits.bLBA_low;
    U8 bHead_num=tpH2d->dw1.bits.bDevice&0x0F;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    U16 heads_per_cylinder= unAtaIdentify.wArray[55];
    U16 sectors_per_track= unAtaIdentify.wArray[56];
    //todo: 增加异常处理，head 超了，sec超了

    if((wCyl_num <= (unAtaIdentify.wArray[54] - 1)) &&
       (bHead_num <= (unAtaIdentify.wArray[55] - 1)) &&
       (bSec_num <= (unAtaIdentify.wArray[56])) &&
       (bSec_num != 0))
    {
        tTQC.tHiTasks[0].tParam.dLba = ((((U32)wCyl_num * heads_per_cylinder) + (bHead_num) ) * sectors_per_track) + bSec_num - 1;
        DEBUG_PRINT("dLba=%#010lx,dSctSize=%d\r\n",tTQC.tHiTasks[0].tParam.dLba,tTQC.tHiTasks[0].tParam.dSctSize);
        DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
        return CMD_VALID;
    }
    else
    {
        PRINT_TXT("E:IDNF,CHS");
        return CMD_ID_NOT_FOUND;
    }
}
// *****************************************************************************
// 函数名称: ATACmd_GetCountLba28
// 函数功能: 通用计算获取28bit 的LBA 和SECTOR size
// 入口参数: tpH2d FIS 地址
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_GetCountLba28(T_H2D *tpH2d)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    /*A value of 00h indicates that 256 logical sectors
    are to be transferred*/      
    tTQC.tHiTasks[0].tParam.dSctSize= tpH2d->dw3.bits.bCount;
    if(0 == tTQC.tHiTasks[0].tParam.dSctSize)
    {
        tTQC.tHiTasks[0].tParam.dSctSize=256;
    }
    /*
        ATA4 - P213
        Device/Head -
        bit 6 set to one if LBA address, cleared to zero if CHS address.
        DEV shall indicate the selected device.
        bits (3:0) starting head number or LBA address bits (27:24)
    
    */
    if(tpH2d->dw1.bits.bDevice&0x40)
    {
//        tTQC.tHiTasks[0].tParam.dLba=unFsmBuff.FIS.h2d.dw1.bits.bLBA_low;//0-7bit
//        tTQC.tHiTasks[0].tParam.dLba+=(unFsmBuff.FIS.h2d.dw1.bits.bLBA_mid<<8);//8-15bit
//        tTQC.tHiTasks[0].tParam.dLba+=(unFsmBuff.FIS.h2d.dw1.bits.bLBA_high<<16);//16-23bit
          tTQC.tHiTasks[0].tParam.dLba=(tpH2d->dw1.dAll&0x00FFFFFF);//加速计算LBA
          tTQC.tHiTasks[0].tParam.dLba+=((tpH2d->dw1.bits.bDevice&0x0F)<<24);//24-27bit 在DEVICE的低4bit
          //TBD:ATA8 的DEVICE 是RESERVED的字段，跟旧版本的ATA定义冲突？？？？？
    }
    else
    {
        //CHS mode
        return ATACmd_Chs2Lba(tpH2d);
    }
    //TODO：更多参数检查返回其他状态
    if((tTQC.tHiTasks[0].tParam.dLba + tTQC.tHiTasks[0].tParam.dSctSize) > tSataDC.dMaxLbaAddr4User)
    {
        return CMD_ID_NOT_FOUND;
    }
    else
    {
        DEBUG_PRINT("dLba=%#010lx,dSctSize=%d\r\n",tTQC.tHiTasks[0].tParam.dLba,tTQC.tHiTasks[0].tParam.dSctSize);
        DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
        return CMD_VALID;
    }
}

// *****************************************************************************
// 函数名称: ATACmd_GetCountLba48
// 函数功能: 通用计算获取48bit 的LBA 和SECTOR size
// 入口参数: tpH2d H2D FIS地址
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_GetCountLba48(T_H2D *tpH2d)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
#if(SATA_DCO_ENABLE == TRUE)
    //如果被改成不支持48bit 命令返回error
    if(!DCO_48BIT)
    {
        return CMD_ABORT;
    }
#endif

    if((tpH2d->dw2.bits.bExpLBA_mid !=0) ||
       (tpH2d->dw2.bits.bExpLBA_high !=0) )
    {
        //！！Error,LBA传入的地址大于2T
        PRINT_TXT("E:>2T");
        return CMD_ID_NOT_FOUND;
    }
    
    tTQC.tHiTasks[0].tParam.dSctSize=tpH2d->dw3.dAll&0xFFFF;

    if(0 == tTQC.tHiTasks[0].tParam.dSctSize)
    {
        tTQC.tHiTasks[0].tParam.dSctSize=65536;
    }

//    tTQC.tHiTasks[0].tParam.dLba=unFsmBuff.FIS.h2d.dw1.bits.bLBA_low;
//    tTQC.tHiTasks[0].tParam.dLba+=((U32)unFsmBuff.FIS.h2d.dw1.bits.bLBA_mid<<8);
//    tTQC.tHiTasks[0].tParam.dLba+=((U32)unFsmBuff.FIS.h2d.dw1.bits.bLBA_high<<16);
    tTQC.tHiTasks[0].tParam.dLba=(tpH2d->dw1.dAll&0x00FFFFFF);//加速计算LBA
    tTQC.tHiTasks[0].tParam.dLba+=((U32)tpH2d->dw2.bits.bExpLBA_low<<24);
    //32位LBA地址能包含2T的数据量，所以这里只使用低32位
    //tTQC.tHiTasks[0].tParam.dLba+=((U64)unFsmBuff.FIS.h2d.dw2.bits.bExpLBA_mid<<32);
    //tTQC.tHiTasks[0].tParam.dLba+=((U64)unFsmBuff.FIS.h2d.dw2.bits.bExpLBA_high<<40);

    //TODO：更多参数检查返回其他状态
    if((tTQC.tHiTasks[0].tParam.dLba + tTQC.tHiTasks[0].tParam.dSctSize) > tSataDC.dMaxLbaAddr4User)
    {
        return CMD_ID_NOT_FOUND;
    }
    else
    {
        DEBUG_PRINT("dLba=%#010lx,dSctSize=%d\r\n",tTQC.tHiTasks[0].tParam.dLba,tTQC.tHiTasks[0].tParam.dSctSize);
        DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
        return CMD_VALID;
    }
}

// *****************************************************************************
// 函数名称: ATACmd_ExeDevDiagnostic
// 函数功能: 实现ATA命令 EXECUTE DEVICE DIAGNOSTIC 
// 入口参数: NONE
// 出口参数: NONE
// 说    明: EXECUTE DEVICE DIAGNOSTIC - 90h
// *****************************************************************************
void ATACmd_ExeDevDiagnostic(void)
{
     UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_Identify
// 函数功能: 实现ATA命令 IDENTIFY DEVICE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: IDENTIFY DEVICE - ECh, PIO Data-In
// *****************************************************************************
void ATACmd_Identify(void)
{
    UN_CH_DI unDI;
    T_HI_TASK tTask;

   // UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    unDI.dAll=0xC0;
    /*
    0xC0=
    unDI.dAll=0;
    di.bits.P=1;
    di.bits.D=DIR_IN;
    */
    UpdateCheckSum(unAtaIdentify.bArray,sizeof(unAtaIdentify));
    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,tSataDC.bDrq,1<<9);
    tTask.eStatus=CMD_VALID;
    tTask.tParam.bTag=CMD_SLOT0;
    tTask.tParam.bDir=DIR_IN;
    tTask.tParam.dBuffAddr=(U32)(&unAtaIdentify.bArray[0]);
    tTask.tParam.dSctSize=1;
    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,NULL);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_DataSetManagement
// 函数功能: 实现ATA命令 DATA SET MANAGEMENT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: DATA SET MANAGEMENT - 06h, DMA
// *****************************************************************************
void ATACmd_DataSetManagement(void)
{
    //TRIM
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(((unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L & 0x01) == 0) ||
        (unFsmBuff.FIS.h2d.dw3.bits.bCount == 0) ||
        (unFsmBuff.FIS.h2d.dw3.bits.bCount > MAX_TRIM_ENTRY))
    {
        eStatus= CMD_ABORT;
    }
    else
    {
        eStatus=ATACmd_GetCountLba48(&unFsmBuff.FIS.h2d);
        tTQC.tHiTasks[0].tParam.dLba=0;
        //TODO:该接口是一个后台模式接口，HOST 发送完数据下来FTL 根据OPCODE知道是TRIM cmd
    }
    PRINT_TXT("TRIM");
    //ATACmd_CreateDmaTask(OP_HI_TRIM,eStatus,DIR_OUT);
	ATACmd_ReleaseCommand(eStatus);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_InitDevParam
// 函数功能: 实现ATA命令 INITIALIZE DEVICE PARAMETERS - 91h, Non-Data - ATA4
// 入口参数: NONE
// 出口参数: NONE
// 说    明: INITIALIZE DEVICE PARAMETERS - 91h, Non-Data - ATA4
// *****************************************************************************
void ATACmd_InitDevParam(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    //For SATA logo test  CHS SUPPORT
    /*
        After a successful INITIALIZE DEVICE PARAMETERS command the content of all IDENTIFY DEVICE
        words shall comply with 6.2.1 in addition to the following:
        1) The content of words 1, 3, 6, and (61:60) shall be unchanged.
        2) The content of word 55 shall equal the (Max head value requested by the host + 1).
        3) The content of word 56 shall equal the (Logical sectors per logical track value requested by the host).
        4) If the content of word (61:60) is less than or equal to 16,514,064, then word 54 shall equal the whole
        number result of [[(the content of words (61:60)) / [(the new content of word 55 as determined by the
        successful INITIALIZE DEVICE PARAMETERS command) * (the new content of word 56 as determined
        by the successful INITIALIZE DEVICE PARAMETERS command)]], or 65,535 whichever is less.
        5) If the content of word (61:60) is greater than 16,514,064, then word 54 shall equal the whole number
        result of [[(16,514,064) / [(the new content of word 55 as determined by the successful INITIALIZE
        DEVICE PARAMETERS command) * (the new content of word 56 as determined by the successful
        INITIALIZE DEVICE PARAMETERS command)]] or 65,535 whichever is less.
        Words (58:57) shall equal [(the new content of word 54) * (the new content of word 55) * (the new
        content of word 56)].
        --ATA4,P109
    */
    
    unAtaIdentify.wArray[55]=((unFsmBuff.FIS.h2d.dw1.bits.bDevice&0x0F) + 1);
    unAtaIdentify.wArray[56]=unFsmBuff.FIS.h2d.dw3.bits.bCount;

    U32 dTmp;
    U32 dCap28=(unAtaIdentify.wArray[61] << 16) + unAtaIdentify.wArray[60];
    {
        //54 计算
        if(dCap28 <= 16514064)
        {
            
        }
        else
        {
           dCap28=16514064;
         
        }
        dTmp=dCap28/(unAtaIdentify.wArray[55]*unAtaIdentify.wArray[56]);
        unAtaIdentify.wArray[54]=dTmp>65535?65535:dTmp;//TODO:到底是3FFF还是FFFF?
    }
    
    {
        //58:57计算
        dCap28=unAtaIdentify.wArray[54]*unAtaIdentify.wArray[55]*unAtaIdentify.wArray[56];
        unAtaIdentify.wArray[57]=(dCap28&0xFFFF);
        unAtaIdentify.wArray[58]=(dCap28>>16);
    }
    
    ATACmd_ReleaseCommand(eStatus);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: FlushCacheDone
// 函数功能: flush cache 任务结束
// 入口参数: NONE
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void FlushCacheDone(CMD_STATUS eStatus,void *ptTaskParam)
{
    ATACmd_ReleaseCommand(eStatus);
}
// *****************************************************************************
// 函数名称: ATACmd_FlushCache
// 函数功能: 实现ATA命令 FLUSH CACHE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: FLUSH CACHE - E7h, Non-Data
// *****************************************************************************
void ATACmd_FlushCache(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    PRINT_TXT("FLUSH");
    //TQM_CreateTask(OP_HI_FLUSH,&tTQC.tHiTasks[0],FlushCacheDone);
	ATACmd_ReleaseCommand(CMD_IDLE);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

void read_verify_result(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_NONE_EVENT;
    __pevent->pfEventCompleteCb=NULL;
    ATACmd_ReleaseCommand(eStatus);
    //sync 任务结束
}
// *****************************************************************************
// 函数名称: ATACmd_ReadVerify
// 函数功能: 实现上报ATA历史命令的支持
// 入口参数: NONE
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void ATACmd_ReadVerify(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(unFsmBuff.FIS.h2d.dw0.bits.Cmd == 0x40)
    {
        eStatus=ATACmd_GetCountLba28(&unFsmBuff.FIS.h2d);
    }
    else
    {
        eStatus=ATACmd_GetCountLba48(&unFsmBuff.FIS.h2d);
    }

    if(eStatus == CMD_VALID)
    {
        tSataDC.tSyncEvent.bOpcode=OP_DI_DATA;
        tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tSyncEvent.dParam[0]=DIR_IN;
        tSataDC.tSyncEvent.dParam[1]=tTQC.tHiTasks[0].tParam.dLba;
        tSataDC.tSyncEvent.dParam[2]=tTQC.tHiTasks[0].tParam.dSctSize;
        tSataDC.tSyncEvent.dParam[3]=tSataDC.tSataMI.dDmaBuffAddr;
        tSataDC.tSyncEvent.pfEventCompleteCb=read_verify_result;
    }
    else
    {
        ATACmd_ReleaseCommand(eStatus);
    }

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_LegacyCmdSupport
// 函数功能: 实现上报ATA历史命令的支持
// 入口参数: NONE
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_LegacyCmdSupport(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    //SEEK
    tSataDC.eDPMSts=DPS_ACTIVE;
    tSataDC.dStanbyTimerCnt=0;

    ATACmd_ReleaseCommand(eStatus);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_FlushCacheExt
// 函数功能: 实现ATA命令 FLUSH CACHE EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: FLUSH CACHE EXT  - EAh, Non-Data
// *****************************************************************************
void ATACmd_FlushCacheExt(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    ATACmd_ReleaseCommand(eStatus);
    
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: SetFeatures_0x10_enable_sata_feature
// 函数功能: 实现SetFeatures 10 子命令
// 入口参数: bCount：H2D中的count字段
// 出口参数: 命令有效性检查状态
// 说    明: FEATURES - 10h
// *****************************************************************************
CMD_STATUS SetFeatures_0x10_enable_sata_feature(U8 bCount)
{
    switch(bCount)
    {
//      case 1:
//            //Non-zero Buffer Offsets
//            break;
        case 2:
            //PRINT_TXT("E-DMAA");
             //DMA Setup Auto-Activate optimization -word78
            //SetBits(unAtaIdentify.wArray+79,(U8)2,(U8)1,(U16)1);
            unAtaIdentify.wArray[79]|=(W_BIT2);
#ifdef  SC_WP_4K_PRE
            SG_DSATA->rACTIVE_SEND.bits.ACTIVE_SEND = TRUE;			  // 预收4K 是否发生第一个DMA ACTIVE
#endif
            break;
        case 3:
            //PRINT_TXT("EDIPM");
#if(SATA_DCO_ENABLE == TRUE)
            if(!DCO_DIPM)
            {
                return CMD_ABORT;
            }
#endif
            //Device-initiated interface power state transitions
            //SetBits(unAtaIdentify.wArray+79,(U8)3,(U8)1,(U16)1);
            unAtaIdentify.wArray[79]|=(W_BIT3);
            //配置发生在读IDENTIFY之前
            UART_PRINT("unAtaIdentify.wArray[79]=%x\r\n",unAtaIdentify.wArray[79]);
            //TODO:增加使能进入休眠模式的代码
        #if (L_PWN_MNG_EN == 1)
            #if (L_DIPM_EN == 1)
                /*
                1. Firmware writes the PITIMER register (while PCMDR.ST=0) with a timeout value corresponding to
                    the required delay for PDMA to request power management state:
                    Delay = PITIMER value x Thclk, where Thclk - hclk period
                */
                tSataDC.bIsDIPMEn=TRUE;
                //save register
//                Mem32Copy((U32*)tSataDC.tSataMI.dDmaBuffAddr,(U32*)SG_PADCTRL_BASE,sizeof(SG_PADCTRL_TypeDef));
//                {
//            
//                    SG_PADCTRL->rPAD_PE_CTRL0.dAll=0x00000fff;
//                    SG_PADCTRL->rPAD_PE_CTRL1.dAll=0x000003ff;
//                    SG_PADCTRL->rPAD_PE_CTRL2.dAll=0x00000fff;
//                    SG_PADCTRL->rPAD_PE_CTRL3.dAll=0x000003ff;
//                    SG_PADCTRL->rPAD_PE_CTRL4.dAll=0x00ffffa7;
//                    SG_PADCTRL->rPAD_PE_CTRL5.dAll=0x000000ff;
//                    SG_PADCTRL->rPAD_PS_CTRL0.dAll=0x0000059f;
//                    SG_PADCTRL->rPAD_PS_CTRL1.dAll=0x0000059f;
//                    SG_PADCTRL->rPAD_PS_CTRL2.dAll=0x0000059f;
//                    SG_PADCTRL->rPAD_PS_CTRL3.dAll=0x0000059f;
//                    SG_PADCTRL->rPAD_PS_CTRL4.dAll=0x00ffffa7;
//                    SG_PADCTRL->rPAD_PS_CTRL5.dAll=0x000000ff;
//                   
//                    Mem32SetVal((U32*)&SG_PADCTRL->rPORT_MODE0.dAll,0xff,15);
//                    
//                    SG_PADCTRL->rPORT_MODE15.dAll=0xbf;
//                    
//                    SG_PADCTRL->rPAD_FUNC_CTRL0.dAll=0x000003ff;
//                    SG_PADCTRL->rPAD_FUNC_CTRL1.dAll=0x000003ff;
//                    SG_PADCTRL->rPAD_FUNC_CTRL2.dAll=0x000003ff;
//                    SG_PADCTRL->rPAD_FUNC_CTRL3.dAll=0x000003ff;
//                    SG_PADCTRL->rPAD_FUNC_CTRL4.dAll=0x007ffff3;
//                    SG_PADCTRL->rPAD_MISC_CTRL.dAll=0x00000020; 
//                    SG_PADCTRL->rAIP_CTRL.dAll=0x00000fff;
//                }

                SG_DSATA->rPSCTLR.bits.ITIMER_PM=LPM_PARTIAL;//TODO:控制省电级别

                if(SG_DSATA->rPCMDR.bits.APSTE==1)
                {
                    SG_CPM->rPAWCTRL.bits.PAWEN =0;
                }
                else
                {
                    SG_CPM->rPAWCTRL.bits.PAWEN =1;//使能重新激活
                }

            #endif 
        #endif            
        break;
//        case 4:
//            //Guaranteed In-Order Data Delivery
//            break;
//        case 5:
//            //Asynchronous Notification
//            break;
        case 6:
            //PRINT_TXT("E-SSP");
#if(SATA_DCO_ENABLE == TRUE)
            if(!DCO_SSP)
            {
                return CMD_ABORT;
            }
#endif
            //Software Settings Preservation
            //SetBits(unAtaIdentify.wArray+79,(U8)6,(U8)1,(U16)1);
            unAtaIdentify.wArray[79]|=(W_BIT6);
            break;
        case 7:
            //PRINT_TXT("E-AP2S");
#if(SATA_DCO_ENABLE == TRUE)
            if(!DCO_AP2S)
            {
                return CMD_ABORT;
            }
#endif
            //SetBits(unAtaIdentify.wArray+79,(U8)7,(U8)1,(U16)1);
            unAtaIdentify.wArray[79]|=(W_BIT7);
            SG_DSATA->rPCMDR.bits.APSTE=1;//enable auto p to s
            break;
        case 9:
             //PRINT_TXT("EDSLP");
             //SetBits(unAtaIdentify.wArray+79,(U8)8,(U8)1,(U16)1);
             unAtaIdentify.wArray[79]|=(W_BIT8);
             //ENABLE DEVICE SLEEP
#if (L_PWN_MNG_EN == 1)
             SG_CPM->rPHY_POWER_CTRL.bits.PARTIAL_SLUMBER_CLK_GATING_EN=0;//允许clk gating
             SG_CPM->rPHY_POWER_CTRL.bits.SLUMBER_MODE=0;//允许进入SLUMBER_S
             SG_CPM->rDEVSLP_CTRL.bits.DEVSLP_EN=1;
             SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=1;//使能异常中断
             SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN=0;
#endif
             break;
        default:
            return CMD_ABORT;
    }
    return CMD_VALID;
}

// *****************************************************************************
// 函数名称: SetFeatures_0x90_disable_sata_feature
// 函数功能: 实现SetFeatures 90 子命令
// 入口参数: bCount：H2D中的count字段
// 出口参数: 命令有效性检查状态
// 说    明: FEATURES - 90h
// *****************************************************************************
CMD_STATUS SetFeatures_0x90_disable_sata_feature(U8 bCount)
{
    switch(bCount)
    {
//      case 1:
//            //Non-zero Buffer Offsets
//            break;
        case 2:
            //PRINT_TXT("D-DMAA");
             //DMA Setup Auto-Activate optimization -word78
            //SetBits(unAtaIdentify.wArray+79,(U8)2,(U8)1,(U16)0);
            unAtaIdentify.wArray[79]&=~(W_BIT2);
#ifdef  SC_WP_4K_PRE
            SG_DSATA->rACTIVE_SEND.bits.ACTIVE_SEND = FALSE;			  // 预收4K 是否发生第一个DMA ACTIVE
#endif
            break;
        case 3:
            //PRINT_TXT("DDIPM");
#if(SATA_DCO_ENABLE == TRUE)
            if(!DCO_DIPM)
            {
                return CMD_ABORT;
            }
#endif
            //Device-initiated interface power state transitions
            //SetBits(unAtaIdentify.wArray+79,(U8)3,(U8)1,(U16)0);
            unAtaIdentify.wArray[79]&=~(W_BIT3);
            //TODO:增加禁止进入休眠模式的代码
        #if (L_PWN_MNG_EN == 1)
            #if(L_DIPM_EN == 1)
                tSataDC.bIsDIPMEn=FALSE;
                SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;
            #endif 
        #endif
            break;
//        case 4:
//            //Guaranteed In-Order Data Delivery
//            break;
//        case 5:
//            //Asynchronous Notification
//            break;
        case 6:
            //PRINT_TXT("D-SSP");
#if(SATA_DCO_ENABLE == TRUE)
            if(!DCO_SSP)
            {
                return CMD_ABORT;
            }
#endif
            //Software Settings Preservation
            //SetBits(unAtaIdentify.wArray+79,(U8)6,(U8)1,(U16)0);
            unAtaIdentify.wArray[79]&=~(W_BIT6);
        case 7:
            //PRINT_TXT("D-AP2S");
#if(SATA_DCO_ENABLE == TRUE)
            if(!DCO_AP2S)
            {
                return CMD_ABORT;
            }
#endif
            //SetBits(unAtaIdentify.wArray+79,(U8)7,(U8)1,(U16)0);
            unAtaIdentify.wArray[79]&=~(W_BIT7);
            SG_DSATA->rPCMDR.bits.APSTE=0;//disable auto p to s
            break;
        case 9:

             //PRINT_TXT("DDSLP");
             //SetBits(unAtaIdentify.wArray+79,(U8)8,(U8)1,(U16)0);
             unAtaIdentify.wArray[79]&=~(W_BIT8);
             //disable device sleep
#if (L_PWN_MNG_EN == 1)
             SG_CPM->rDEVSLP_CTRL.bits.DEVSLP_EN=0;
             SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=0;//清除异常中断
             SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN=0;
#endif
             break;
        default: 
            return CMD_ABORT;

    }
    return CMD_VALID;
}
// *****************************************************************************
// 函数名称: SetFeatures_0x03_set_transfer_mode
// 函数功能: 实现SetFeatures 03 子命令
// 入口参数: bCount：H2D中的count字段
// 出口参数: 命令有效性检查状态
// 说    明: FEATURES - 03h
// *****************************************************************************
CMD_STATUS SetFeatures_0x03_set_transfer_mode(U8 bCount)
{
    /*
    PIO default mode 0_0000b 000b
    PIO default mode, disable IORDY 0_0000b 001b
    PIO flow control transfer mode 0_0001b Mode
    Retired 0_0010b N/A
    Multiword DMA mode 0_0100b Mode
    Ultra DMA mode 0_1000b Mode
    Reserved 1_0000b N/A
    */  
    //参数出错怎么办？
    U8 bType=(bCount>>3);
    U8 bMode=(bCount&0x07);
    
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    DEBUG_PRINT("origin WORD63=%#06lx,WORD88=%#06lx\r\n",*(unAtaIdentify.wArray+63),*(unAtaIdentify.wArray+88));
    DEBUG_PRINT("bType=%u,bMode=%u\r\n",bType,bMode);
    //两种DMA只能选择其中一种
#if 1
    if(bType == 4)
    {
        //清除UDMA选择-word88
        //SetBits(unAtaIdentify.wArray+88,(U8)14,(U8)7,(U16)0);
         unAtaIdentify.wArray[88]&=(~(0x7F<<8));
        //设置Multiword DMA mode -word63 可选模式0-2，超出自动截断
        //SetBits(unAtaIdentify.wArray+63,(U8)10,(U8)3,((U16)((U16)1<<bMode)));
        unAtaIdentify.wArray[63]&=(~(7<<8));
        unAtaIdentify.wArray[63]|=(1<<(bMode+8));

    }
    else if(bType == 8)
    {
        //清除Multiword DMA mode -word63  
        //SetBits(unAtaIdentify.wArray+63,(U8)10,(U8)3,(U16)0);
        unAtaIdentify.wArray[63]&=(~(7<<8));
        //UDMA选择-word88  可选模式0-6，超出自动截断
        //SetBits(unAtaIdentify.wArray+88,(U8)14,(U8)7,((U16)((U32)1<<bMode)));
        unAtaIdentify.wArray[88]&=(~(0x7F<<8));
        unAtaIdentify.wArray[88]|=(1<<(bMode+8));
    }
    else if(bType == 1)
    {
        //TODO:    PIO flow control transfer mode
        //SATA设备不需要关心？
    }
    else if(bType == 0)
    {
        //TODO:    PIO default mode
        
    }
    else
    {
        DEBUG_PRINT("Exit func abort=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
        return CMD_ABORT;
    }
#endif
    // DEBUG_PRINT("Exit func abort=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //return CMD_ABORT;
    DEBUG_PRINT("after config WORD63=%#06lx,WORD88=%#06lx\r\n",*(unAtaIdentify.wArray+63),*(unAtaIdentify.wArray+88));
    DEBUG_PRINT("Exit func ok=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_VALID;
}

// *****************************************************************************
// 函数名称: ATACmd_SetFeatures
// 函数功能: 实现ATA命令 SET FEATURES
// 入口参数: NONE
// 出口参数: NONE
// 说    明: SET FEATURES - EFh, Non-Data
// *****************************************************************************
void ATACmd_SetFeatures(void)
{
    CMD_STATUS eStatus=CMD_IDLE;

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    switch(unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L)
    {
        case 0x03:
            //Set transfer mode
            eStatus=SetFeatures_0x03_set_transfer_mode(unFsmBuff.FIS.h2d.dw3.bits.bCount);
            break;
        case 0x02:
            //Enable volatile write cache 
            tSataDC.bIsWrCacheEnabled=TRUE;
            //SetBits(unAtaIdentify.wArray+85,(U8)5,(U8)1,(U16)1);
            unAtaIdentify.wArray[85]|=(W_BIT5);
            break;
        case 0x82:
             tSataDC.bIsWrCacheEnabled=FALSE;
             //SetBits(unAtaIdentify.wArray+85,(U8)5,(U8)1,(U16)0);
             unAtaIdentify.wArray[85]&=~(W_BIT5);
            //Disable volatile write cache 
            break;
        
        case 0x05:
            //Subcommand code 05h allows the host to enable APM
            //SetBits(unAtaIdentify.wArray+86,(U8)3,(U8)1,(U16)1);
            unAtaIdentify.wArray[86]|=(W_BIT3);
            unAtaIdentify.wArray[91]=unFsmBuff.FIS.h2d.dw3.bits.bCount;
        case 0x85:
            //Subcommand code 85h allows the host to disable APM
            //SetBits(unAtaIdentify.wArray+86,(U8)3,(U8)1,(U16)0);
            unAtaIdentify.wArray[86]&=~(W_BIT3);
        
        case 0x55:
            //Disable look ahead cache 
            tSataDC.bIsLookAheadEnabled=FALSE;
            //SetBits(unAtaIdentify.wArray+85,(U8)6,(U8)1,(U16)0);
            unAtaIdentify.wArray[85]&=~(W_BIT6);
            break;
        case 0xaa:
             tSataDC.bIsLookAheadEnabled=TRUE;
            //SetBits(unAtaIdentify.wArray+85,(U8)6,(U8)1,(U16)1);
            unAtaIdentify.wArray[85]|=(W_BIT6);
            //ENABLE look ahead cache 
            break;
        
        case 0x66:
            // disable the reverting to defaults mode.
            tSataDC.bIsRevertingEnabled=FALSE;
            break;
        case 0xCC:
             //enable the reverting to defaults mode.
            tSataDC.bIsRevertingEnabled=TRUE;
            break;
        case 0x10:
            
            //Enable SATA feature
            eStatus=SetFeatures_0x10_enable_sata_feature(unFsmBuff.FIS.h2d.dw3.bits.bCount);
            break;
        case 0x90:
            //Disable SATA feature
            eStatus=SetFeatures_0x90_disable_sata_feature(unFsmBuff.FIS.h2d.dw3.bits.bCount);
            break;
        default:
            PRINT_HEX("E:SFEA",unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L);//error cmd abort
            eStatus=CMD_ABORT;
            break;
    }
#if (SATA_LOG_ENABLE == 1) //NOTE:重要！！sleep 测试过程中可能存在响应超时问题，但是去掉这个会导致协议测试fail
    if(eStatus!=CMD_ABORT)
    {
        //改写30h，00page内容
        UpdateIDFYLogPage08(CMD_IDLE,&tSataDC.tSyncEvent);
        PRINT_TXT("SETF");
        tSataDC.dIsNDCTOGuardEn=TRUE;
        tSataDC.dNDCTOGuardTimer=tSataDC.dPowerOnTime;
    }
    else
    {
        ATACmd_ReleaseCommand(eStatus);
    }
#else
        ATACmd_ReleaseCommand(eStatus);
#endif
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SetMultipleMode
// 函数功能: 实现ATA命令SET MULTIPLE MODE 
// 入口参数: NONE
// 出口参数: NONE
// 说    明: SET MULTIPLE MODE - C6h, Non-Data
// *****************************************************************************
void ATACmd_SetMultipleMode(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(unFsmBuff.FIS.h2d.dw3.bits.bCount <= (U8)DRQ_SIZE)
    {
        /*
         *
         * If the contents of the COUNT field are zero and the SET MULTIPLE command returns command completion
            without an error, then the device shall respond to any subsequent READ MULTIPLE command, READ
            MULTIPLE EXT command, WRITE MULTIPLE command, WRITE MULTIPLE EXT command, and WRITE
            MULTIPLE FUA EXT command with command aborted until a subsequent SET MULTIPLE command with the
            COUNT field is not cleared to zero returns command completion without an error.
            ------------------------------------------------------------------------------------
         * If the contents of the COUNT field are zero, then the device may:
            a) disable multiple mode (i.e., respond with command aborted for all subsequent READ MULTIPLE
            commands, READ MULTIPLE EXT commands, WRITE MULTIPLE commands, WRITE MULTIPLE EXT
            commands, and WRITE MULTIPLE FUA EXT commands);
            b) return command aborted for all SET MULTIPLE MODE commands; or
            c) retain the previous multiple mode settings.
         * */
        tSataDC.bMultiDrq=unFsmBuff.FIS.h2d.dw3.bits.bCount;

//        if(0 == tSataDC.bMultiDrq)
//        {
//            eStatus=CMD_ABORT;
//        }
//        else
//        {
            /*After a successful SET MULTIPLE command the device shall report the valid value set by that command in
            IDENTIFY DEVICE data word 59.*/
            //SetBits(unAtaIdentify.wArray+59,8,9,(1<<8)|tSataDC.bMultiDrq);
            unAtaIdentify.wArray[59]&=(~(0x1FF));
            unAtaIdentify.wArray[59]|=((1<<8)|tSataDC.bMultiDrq);
//        }

    }
    else
    {
        eStatus=CMD_ABORT;
    }
    
    ATACmd_ReleaseCommand(eStatus);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_PioTaskComplete
// 函数功能: 共用代码
// 入口参数: bDir:读/写
//           eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_PioTaskComplete(U8 bDir,CMD_STATUS eStatus,T_HI_TASK_PARAM *ptEvent)
{
     //PIO数据任务传输完成事件处理
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(eStatus < CMD_IDLE)
    {
       ATACmd_CreatePioTask(eStatus,bDir); //发送状态
    }
    else
    {
#if(SATA_PIO_NEW  == FALSE)
        tSataDC.dPioSctCnt-=tTQC.tHiTasks[0].tParam.dSctSize;//更新数据余量
        tTQC.tHiTasks[0].tParam.dLba+=tTQC.tHiTasks[0].tParam.dSctSize;
        if(0 == tSataDC.dPioSctCnt)
        {
            //使能了自动发送功能，正常情况下的D2H FIS已经提前准备好发送了
            //ATACmd_CreatePioTask(eStatus,bDir,NULL); //发送状态
        }
        else
        {
            
            if(tSataDC.dPioSctCnt>tSataDC.bDrq)
            {
                tTQC.tHiTasks[0].tParam.dSctSize=tSataDC.bDrq;
            }
            else
            {
                 tTQC.tHiTasks[0].tParam.dSctSize=tSataDC.dPioSctCnt;
            }
           
            if(DIR_IN == bDir)
            {
                TQM_CreateTask(OP_HI_DATA,&tTQC.tHiTasks[0],ATACmd_PioRdTaskComplete);
            }
            else
            {
#ifdef	SC_EXBUFF_EN
            SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 0;
#endif
                TQM_CreateTask(OP_HI_DATA,&tTQC.tHiTasks[0],ATACmd_PioWrTaskComplete);
            }
            
        }
#else
        T_EVENT *pevent=(T_EVENT *)ptEvent;

        if(DIR_OUT == bDir)
        {
            //数据传输完成,PIO OUT D2H
            SATA_SendStatus(0x50,0x00,1);
            PRINT_TXT("PO D2H");
        }
        else
        {
            SATA_UpdateRdIdx(1);
            SATA_ChangeState(DEV_IDLE,DI0_DEVICE_IDLE);//退出当前状态，不处理FTXDS中断了
            PRINT_TXT("PI DONE");
        }
        TQM_RemoveTask(0);//更新SMART写读数值计数
        pevent->bOpcode =OP_NONE_EVENT;
        pevent->pfEventCompleteCb=NULL;

#endif
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_PioRdTaskComplete
// 函数功能: pio写大数据任务完成事件处理
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_PioRdTaskComplete(CMD_STATUS eStatus,void *ptEvent)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    ATACmd_PioTaskComplete(DIR_IN,eStatus,(T_HI_TASK_PARAM*)ptEvent);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_PioWrTaskComplete
// 函数功能: pio写大数据任务完成事件处理
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_PioWrTaskComplete(CMD_STATUS eStatus,void *ptEvent)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    ATACmd_PioTaskComplete(DIR_OUT,eStatus,(T_HI_TASK_PARAM*)ptEvent);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: DI_FlushCache
// 函数功能: 设备发起的FLlush操作
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void DI_FlushCacheDone(CMD_STATUS eStatus,void *ptTaskParam)
{
    T_EVENT *pevent=(T_EVENT *)ptTaskParam;
    //PIO 命令经由DI命令转换传输（组合成单个大数据传输）

    //创建DI PIO任务
    pevent->bOpcode=OP_DI_PIO_DATA;
    pevent->eStatus=CMD_ONGOING;

    pevent->dParam[1]=tTQC.tHiTasks[0].tParam.dLba;//起始LBA

    pevent->dParam[2]=tSataDC.dPioSctCnt;
    //}

    if(DIR_IN == pevent->dParam[0])
    {
        pevent->pfEventCompleteCb=ATACmd_PioRdTaskComplete;
    }
    else
    {
        pevent->pfEventCompleteCb=ATACmd_PioWrTaskComplete;
    }
    DEBUG_PRINT("dir=%d,lba=%x,size=%x,remain=%x\r\n",pevent->dParam[0],pevent->dParam[1],pevent->dParam[2],tSataDC.dPioSctCnt);
}
// *****************************************************************************
// 函数名称: ATACmd_CreatePioTask
// 函数功能: 首次创建PIO任务
// 入口参数: eStatus：命令状态
//           bDir:方向 
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_CreatePioTask(CMD_STATUS eStatus,U8 bDir)
{
    UN_CH_DI unDI;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(CMD_COMPLETED == eStatus)
    {
        return;
    }
    else if(eStatus >= CMD_IDLE)
    {
        if(
           tSataDC.bATACmd==0xe4 /*read buff*/||\
           tSataDC.bATACmd==0xe8 /*write buff*/||\
           tSataDC.bATACmd==0x3f /*read log*/||\
           tSataDC.bATACmd==0x2f /*write log*/||\
           tSataDC.bATACmd==0x20 /*read sec*/||\
           tSataDC.bATACmd==0x24 /*write sec*/||\
           tSataDC.bATACmd==0x30 /*read sec ext*/||\
           tSataDC.bATACmd==0x34 /*write sec ext*/)
        {
            tSataDC.bDrq=1;
        }
        else
        {
            /*
             *
             * If the contents of the COUNT field are zero and the SET MULTIPLE command returns command completion
                without an error, then the device shall respond to any subsequent READ MULTIPLE command, READ
                MULTIPLE EXT command, WRITE MULTIPLE command, WRITE MULTIPLE EXT command, and WRITE
                MULTIPLE FUA EXT command with command aborted until a subsequent SET MULTIPLE command with the
                COUNT field is not cleared to zero returns command completion without an error.
             * */
            if(0 == tSataDC.bMultiDrq)
            {
                ATACmd_ReleaseCommand(CMD_ABORT);
                return;
            }
            tSataDC.bDrq=tSataDC.bMultiDrq;//临时调整DRQ --DRQ=1 read sector情况不需要这些操作
        }
        PRINT_DEC("drq=",tSataDC.bDrq);
        unDI.dAll=0x80|(bDir<<6);
        
       // UART_PRINT("tTQC.tHiTasks[0].tParam.dSctSize=%#010lx\r\n",tTQC.tHiTasks[0].tParam.dSctSize);
        
        DSATA_SetCmdHead(CMD_SLOT0,
                         unDI,
                         tTQC.tHiTasks[0].tParam.dSctSize,
                         tSataDC.bDrq,
                         0);
         //UART_PRINT("tCmdHead[0].dw0.cmd.PRDTL=%#010lx,tSataDC.bDrq=%d \r\n",tCmdHead[0].dw0.cmd.PRDTL,tSataDC.bDrq);
        tSataDC.dPioSctCnt = tTQC.tHiTasks[0].tParam.dSctSize;//记录PIO模式下总的数据传输量
        tTQC.tHiTasks[0].tParam.bDir = bDir;

#if(SATA_PIO_NEW  == FALSE)
        if(tSataDC.dPioSctCnt>tSataDC.bDrq)
        {
            tTQC.tHiTasks[0].tParam.dSctSize=tSataDC.bDrq;
        }
        else
        {
             tTQC.tHiTasks[0].tParam.dSctSize=tSataDC.dPioSctCnt;
        }
        
        tTQC.tHiTasks[0].tParam.bDir=bDir;
        tTQC.tHiTasks[0].tParam.bTag=(U8)CMD_SLOT0;
        tTQC.tHiTasks[0].tParam.dBuffAddr=tSataDC.tSataMI.dDmaBuffAddr;
        
        if(DIR_IN == bDir)
        {
            TQM_CreateTask(OP_HI_DATA,&tTQC.tHiTasks[0],ATACmd_PioRdTaskComplete);
        }
        else
        {
#ifdef	SC_EXBUFF_EN
            SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 0;
#endif
            TQM_CreateTask(OP_HI_DATA,&tTQC.tHiTasks[0],ATACmd_PioWrTaskComplete);
        }
#else
        if(DIR_OUT == bDir)
		{
        	tTQC.dPendingWrSec += tTQC.tHiTasks[0].tParam.dSctSize;
		}

        tSataDC.tSyncEvent.bOpcode=OP_DI_PIO_DATA;
        tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tSyncEvent.dParam[0]=bDir;
        tSataDC.tSyncEvent.dParam[1]=tTQC.tHiTasks[0].tParam.dLba;//起始LBA
        tSataDC.tSyncEvent.dParam[2]=tTQC.tHiTasks[0].tParam.dSctSize;

        if(DIR_IN ==  tSataDC.tSyncEvent.dParam[0])
        {
            tSataDC.tSyncEvent.pfEventCompleteCb=ATACmd_PioRdTaskComplete;
        }
        else
        {
            tSataDC.tSyncEvent.pfEventCompleteCb=ATACmd_PioWrTaskComplete;
        }
        //ERROR_PRINT("create pio task!!!!\r\n");
        tSataDC.eDPMSts=DPS_ACTIVE;//If a media access is required, the device shall transition to the PM0: Active state.
        tSataDC.dStanbyTimerCnt=0;
#endif
    }
    else
    {
        ATACmd_ReleaseCommand(eStatus);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_RwMultiple
// 函数功能: 子函数 
// 入口参数: tpH2d FSM buffer 地址
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_RwMultiple(T_H2D *tpH2d)
{
    /*
    If IDENTIFY DEVICE data word 59 bit 8 is cleared to zero or IDENTIFY DEVICE data word 59 bits 7:0
    (see 7.12.7.21) are cleared to zero, and a WRITE MULTIPLE command is received by the device, and the device
    has not returned command completion without an error for a SET MULTIPLE MODE command, the device shall
    return command aborted. A SET MULTIPLE MODE command that returns command completion without an error
    should precede a WRITE MULTIPLE command.
    */
    //TODO:如果不符合要求，将命令状态置为ABORT，通知传输层放弃执行
    /*A value of 00h indicates that 256 logical sectors
    are to be transferred*/
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
//    if(0 == tSataDC.bMultiDrq)
//    {
//        return CMD_ABORT;
//    }
    return ATACmd_GetCountLba28(tpH2d);
}

// *****************************************************************************
// 函数名称: ATACmd_RwMultipleExt
// 函数功能: 子函数 
// 入口参数: tpH2d FSM buffer 地址
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_RwMultipleExt(T_H2D *tpH2d)
{
    /*
    If IDENTIFY DEVICE data word 59 bit 8 is cleared to zero or IDENTIFY DEVICE data word 59 bits 7:0
    (see 7.12.7.21) are cleared to zero, and a WRITE MULTIPLE command is received by the device, and the device
    has not returned command completion without an error for a SET MULTIPLE MODE command, the device shall
    return command aborted. A SET MULTIPLE MODE command that returns command completion without an error
    should precede a WRITE MULTIPLE command.
    */
    //TODO:如果不符合要求，将命令状态置为ABORT，通知传输层放弃执行
    /*A value of 0000h indicates that 65536 logical sectors
    are to be transferred*/
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return ATACmd_GetCountLba48(tpH2d);
}
// *****************************************************************************
// 函数名称: ATACmd_ReadMultiple
// 函数功能: 实现ATA命令READ MULTIPLE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ MULTIPLE -C4h, PIO Data-In
// *****************************************************************************
void ATACmd_ReadMultiple(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus=ATACmd_RwMultiple(&unFsmBuff.FIS.h2d);
    ATACmd_CreatePioTask(eStatus,DIR_IN);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
}

// *****************************************************************************
// 函数名称: ATACmd_ReadBuff
// 函数功能: 实现ATA命令READ buff
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ BUFF -E8h, PIO Data-In
// *****************************************************************************
void ATACmd_ReadBuff(void)
{
#if 0
    UN_CH_DI unDI;
    T_HI_TASK tTask;
   // UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    unDI.dAll=0x80|(DIR_IN<<6);
    /*
    0xC0=
    unDI.dAll=0;
    di.bits.P=1;
    di.bits.D=DIR_IN;
    */

    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,tSataDC.bDrq,1<<9);
    tTask.eStatus=CMD_VALID;
    tTask.tParam.bTag=CMD_SLOT0;
    tTask.tParam.bDir=DIR_IN;
    tTask.tParam.dBuffAddr=(U32)(RWBUFF_ADDR);
    tTask.tParam.dSctSize=1;//如果大小大于DRQ，需要创建回调函数分解任务
    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,NULL);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#else
    tTQC.tHiTasks[0].tParam.dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + HOST_RWBUFF_OFFSET;
    tTQC.tHiTasks[0].tParam.dSctSize=1;
    PRINT_HEX("RBUF:",tTQC.tHiTasks[0].tParam.dLba);
    ATACmd_CreatePioTask(CMD_IDLE,DIR_IN);
#endif
}
void BuffDataReceived(CMD_STATUS eStatus,void *ptParam)
{
    //数据接收完成
    ATACmd_ReleaseCommand(eStatus);//发送状态，PIO OUT或者DMA OUT
}
// *****************************************************************************
// 函数名称: ATACmd_WriteBuff
// 函数功能: 实现ATA命令write buff
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ BUFF -E4h, PIO Data-out
// *****************************************************************************
void ATACmd_WriteBuff(void)
{
#if 0
    UN_CH_DI unDI;
    T_HI_TASK tTask;
   // UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    unDI.dAll=0x80|(DIR_OUT<<6);
    /*
    0xC0=
    unDI.dAll=0;
    di.bits.P=1;
    di.bits.D=DIR_out;
    */

    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,tSataDC.bDrq,1<<9);
    tTask.eStatus=CMD_VALID;
    tTask.tParam.bDir=DIR_OUT;
    tTask.tParam.bTag=CMD_SLOT0;
    tTask.tParam.dBuffAddr=(U32)(RWBUFF_ADDR);
    tTask.tParam.dSctSize=1;//如果大小大于DRQ，需要创建回调函数分解任务
    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,BuffDataReceived);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#else
    tTQC.tHiTasks[0].tParam.dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + HOST_RWBUFF_OFFSET;
    tTQC.tHiTasks[0].tParam.dSctSize=1;
    PRINT_HEX("RBUF:",tTQC.tHiTasks[0].tParam.dLba);
    ATACmd_CreatePioTask(CMD_IDLE,DIR_OUT);
#endif
}

// *****************************************************************************
// 函数名称: ATACmd_ReadBuff
// 函数功能: 实现ATA命令READ buff
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ BUFF -E9h, dma Data-In
// *****************************************************************************
void ATACmd_ReadBuffDma(void)
{
#if 0
    UN_CH_DI unDI;
    T_HI_TASK tTask;
    // UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    unDI.dAll=(DIR_IN<<6);
    //di.bits.P=0;
    //di.bits.F=0;
    //di.bits.A=0;
    //di.bits.D=DIR_IN;

    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,1,1<<9);
    tTask.eStatus=CMD_VALID;
    tTask.tParam.bDir=DIR_IN;
    tTask.tParam.bTag=CMD_SLOT0;
    tTask.tParam.dBuffAddr=(U32)(RWBUFF_ADDR);
    tTask.tParam.dSctSize=1;//如果大小大于DRQ，需要创建回调函数分解任务
    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,NULL);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#else
    tTQC.tHiTasks[0].tParam.dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + HOST_RWBUFF_OFFSET;
    tTQC.tHiTasks[0].tParam.dSctSize=1;
    PRINT_HEX("RBUF:",tTQC.tHiTasks[0].tParam.dLba);
    ATACmd_CreateDmaTask(OP_HI_DATA,CMD_VALID,DIR_IN);
#endif
}

// *****************************************************************************
// 函数名称: ATACmd_WriteBuff
// 函数功能: 实现ATA命令write buff
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ BUFF -EBh, dma Data-out
// *****************************************************************************
void ATACmd_WriteBuffDma(void)
{
#if 0
    UN_CH_DI unDI;
    T_HI_TASK tTask;
    // UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    unDI.dAll=(DIR_OUT<<6);
    //di.bits.P=0;
    //di.bits.F=0;
    //di.bits.A=0;
    //di.bits.D=DIR_OUT;

    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,1,1<<9);
    tTask.eStatus=CMD_VALID;
    tTask.tParam.bDir=DIR_OUT;
    tTask.tParam.bTag=CMD_SLOT0;
    tTask.tParam.dBuffAddr=(U32)(RWBUFF_ADDR);
    tTask.tParam.dSctSize=1;//如果大小大于DRQ，需要创建回调函数分解任务
    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,BuffDataReceived);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#else
    tTQC.tHiTasks[0].tParam.dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + HOST_RWBUFF_OFFSET;
    tTQC.tHiTasks[0].tParam.dSctSize=1;
    PRINT_HEX("RBUF:",tTQC.tHiTasks[0].tParam.dLba);
    ATACmd_CreateDmaTask(OP_HI_DATA,CMD_VALID,DIR_OUT);
#endif
}



// *****************************************************************************
// 函数名称: ATACmd_ReadMultipleExt
// 函数功能: 实现ATA命令READ MULTIPLE EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ MULTIPLE -29h, PIO Data-In
// *****************************************************************************
void ATACmd_ReadMultipleExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus=ATACmd_RwMultipleExt(&unFsmBuff.FIS.h2d);
    ATACmd_CreatePioTask(eStatus,DIR_IN);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_WriteMultiple
// 函数功能: 实现ATA命令 WRITE MULTIPLE
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE MULTIPLE -C5h, PIO Data-Out
// *****************************************************************************
void ATACmd_WriteMultiple(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus=ATACmd_RwMultiple(&unFsmBuff.FIS.h2d);
    ATACmd_CreatePioTask(eStatus,DIR_OUT);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_WriteMultipleExt
// 函数功能: 实现ATA命令 WRITE MULTIPLE EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE MULTIPLE -39h, PIO Data-Out
// *****************************************************************************
void ATACmd_WriteMultipleExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus=ATACmd_RwMultipleExt(&unFsmBuff.FIS.h2d);
    ATACmd_CreatePioTask(eStatus,DIR_OUT);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_WriteMultipleFuaExt
// 函数功能: 实现ATA命令 WRITE MULTIPLE FUA EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE MULTIPLE -CEh, PIO Data-Out
// *****************************************************************************
void ATACmd_WriteMultipleFuaExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus=ATACmd_RwMultipleExt(&unFsmBuff.FIS.h2d);
    tTQC.tHiTasks[0].tParam.bFua=1;
    ATACmd_CreatePioTask(eStatus,DIR_OUT);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_DmaRwTaskComplete
// 函数功能: dma写大数据任务完成事件处理
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void ATACmd_DmaRwTaskComplete(CMD_STATUS eStatus,void *ptTaskParam)
{

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(eStatus < CMD_IDLE)
    {
       ATACmd_ReleaseCommand(eStatus);
       PRINT_TXT("E:TRIM");
    }
    else
    {
         if(!DSATA_IsAutoSendEnabled())
         {
             //TRIM 操作才会进入到这个分支
             ATACmd_ReleaseCommand(eStatus);
             //PRINT_TXT("Trim OK");
         }
    }

    //TODO:可能存在更多操作
}

// *****************************************************************************
// 函数名称: ATACmd_CreateDmaTask
// 函数功能: 通用代码
// 入口参数: eStatus：命令状态
//           bDir:方向 
// 出口参数: NONE
// 说    明: WRITE DMA- CAh, DMA
// *****************************************************************************
void ATACmd_CreateDmaTask(CMD_OP eOpcode,CMD_STATUS eStatus,U8 bDir)
{
    UN_CH_DI unDI;

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);  

    if(CMD_COMPLETED == eStatus)
    {
        //PRINT_TXT("CMD_COMPLETED");
        return;
    }
    else if(eStatus >= CMD_IDLE)
    {
        //PRINT_TXT("CMD_IDLE");
        unDI.dAll=(bDir<<6);
        //di.bits.P=0;
        //di.bits.F=0;
        //di.bits.A=0;
        //di.bits.D=DIR_OUT;
        if(bDir == DIR_OUT)
        unDI.dAll|=((FS_AUTO<<9));

        DSATA_SetCmdHead(CMD_SLOT0,
                            unDI,
                            tTQC.tHiTasks[0].tParam.dSctSize,
                            tTQC.tHiTasks[0].tParam.dSctSize,
                            (tTQC.tHiTasks[0].tParam.dSctSize<<9));
        
        tTQC.tHiTasks[0].tParam.bDir=bDir;
        tTQC.tHiTasks[0].tParam.bTag=(U8)CMD_SLOT0;
        tTQC.tHiTasks[0].tParam.dBuffAddr=tSataDC.tSataMI.dDmaBuffAddr;
        TQM_CreateTask(eOpcode,&tTQC.tHiTasks[0],ATACmd_DmaRwTaskComplete);
    }
    else   
    {
        ATACmd_ReleaseCommand(eStatus);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_WriteDma
// 函数功能: 实现ATA命令 WRITE DMA
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE DMA- CAh, DMA
// *****************************************************************************
void ATACmd_WriteDma(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_GetCountLba28(&unFsmBuff.FIS.h2d);
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_OUT);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_ReadDma
// 函数功能: 实现ATA命令 READ DMA
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ DMA -C8h, DMA
// *****************************************************************************
void ATACmd_ReadDma(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_GetCountLba28(&unFsmBuff.FIS.h2d);
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_IN);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_WriteDmaExt
// 函数功能: 实现ATA命令 WRITE DMA EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE DMA EXT - 35h, DMA
// *****************************************************************************
void ATACmd_WriteDmaExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_GetCountLba48(&unFsmBuff.FIS.h2d);
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_OUT);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_WriteDmaFuaExt
// 函数功能: 实现ATA命令 WRITE DMA FUA EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE DMA EXT - 3Dh, DMA
// *****************************************************************************
void ATACmd_WriteDmaFuaExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_GetCountLba48(&unFsmBuff.FIS.h2d);
    tTQC.tHiTasks[0].tParam.bFua=1;
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_OUT);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_WriteDmaExt
// 函数功能: 实现ATA命令 READ DMA EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ DMA EXT - 25h, DMA
// *****************************************************************************
void ATACmd_ReadDmaExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_GetCountLba48(&unFsmBuff.FIS.h2d);
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_IN);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#if(SATA_LOG_ENABLE == 1)
void DataReceiveComplete(CMD_STATUS eStatus,void *ptParam)
{
    //数据接收完成
    T_HI_TASK_PARAM *__ptParam=(T_HI_TASK_PARAM *)ptParam;

    if(__ptParam->bTag == 0xF1)
    {
        security_set_password_F1((U8*)__ptParam->dBuffAddr);
    }
    else if(__ptParam->bTag == 0xF2)
    {
        security_unlock_F2((U8*)__ptParam->dBuffAddr);
    }
    else if(__ptParam->bTag == 0xF4)
    {
        security_erase_unit_F4((U8*)__ptParam->dBuffAddr);
    }
    else if(__ptParam->bTag == 0xF6)
    {
        security_disable_password_F6((U8*)__ptParam->dBuffAddr);
    }
}

void ReceiveDataFromHost(U8 bCmd)
{
    UN_CH_DI unDI;
    T_HI_TASK tTask;

    unDI.dAll=0x80|(DIR_OUT<<6);
    /*
    0xC0=
    unDI.dAll=0;
    di.bits.P=1;
    di.bits.D=DIR_out;
    */

    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,tSataDC.bDrq,1<<9);

    tTQC.tHiTasks[0].tParam.bTag=bCmd;
    tTQC.tHiTasks[0].tParam.dBuffAddr=(U32)tSyncBuffer.dArray;

    tTask.eStatus=CMD_VALID;
    tTask.tParam.bDir=DIR_OUT;
    tTask.tParam.bTag=CMD_SLOT0;
    tTask.tParam.dBuffAddr=(U32)tSyncBuffer.dArray;
    tTask.tParam.dSctSize=1;
    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,DataReceiveComplete);
}
#endif

void ATACmd_ReleaseCommand(CMD_STATUS status)
{
    T_HI_TASK tTask;
    tTask.eStatus = status;
    if((SG_DSATA->rPSSTSR.dAll & 0xF0F) == 0x103 && (!SG_DSATA->rPISR.bits.COMWKS) && (!SG_DSATA->rPISR.bits.PCS) && (tSataDC.DOS_TransferReq != NULL))
    {
        //PRINT_TXT("SEND D2H");
        //保证没有来过comreset 和  COMWAKE,且没有处理过COMRESET流程,才可以发送D2H
        tSataDC.DOS_TransferReq(REQ_STATUS,&tTask,NULL);
    }
}

#if 1
// *****************************************************************************
// 函数名称: ATACmd_Refuse
// 函数功能: 对于不支持的ATA命令，回复拒绝访问
// 入口参数: NONE
// 出口参数: NONE
// 说	  明: 
// *****************************************************************************
void ATACmd_Refuse(void)
{
    CMD_STATUS eStatus=CMD_ABORT;
	ATACmd_ReleaseCommand(eStatus);
	PRINT_TXT("Refuse A1");
}
#endif

