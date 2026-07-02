// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_sanitize.c
//  文件标识：    
//  内容摘要：    实现Sanitize Device feature set
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2017年03月13日
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************

#include "ata_sanitize.h"
#include "ata_identify.h"
#include "sata_misc.h"
#include "dsata.h"
#include "uitility.h"
#include "sata_typedef.h"
#include "debug.h"
#include "ata_gpl.h"
#include "ata_general.h"

#if(SATA_LOG_ENABLE)

void SanitizeEventDone(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;

    tSataDC.tSyncEvent.bOpcode=OP_NONE_EVENT;
    tSataDC.tSyncEvent.pfEventCompleteCb=NULL;
    if(__pevent->eStatus < CMD_IDLE)
    {
        //出错
        tSataDC.tSDState = S_SD3;
        SG_DSATA->rHWD2HEN.dAll=1;//S_SD2 -> S_SD3状态启动NCQ 自动回复D2H功能
        tSataDC.wSanitizeProCnt = 0xFFFF;
        WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存下tSataDC.tSDState的状态
    }
    else
    {

        if(tSataDC.tSDState == S_SD2)
        {
            U32 end=tSataDC.tSyncEvent.dParam[1] + tSataDC.tSyncEvent.dParam[2];
            if(end < tSataDC.dMaxLbaAddr4User)
            {
                tSataDC.wSanitizeProCnt++;
            }
            else
            {
                tSataDC.wSanitizeProCnt=0xFFFF;
            }
        }
        //PRINT_HEX("SD:",tSataDC.wSanitizeProCnt);
        if(tSataDC.wSanitizeProCnt == 0xFFFF)
        {
            tSataDC.tSDState = S_SD4;
            SG_DSATA->rHWD2HEN.dAll=1;//S_SD2 ->S_SD4状态启动NCQ 自动回复D2H功能
            tSataDC.bSDOCWOEVal =1;
        }
        WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存tSataDC.wSanitizeProCnt的状态
    }

}

void Sanitize_Device_state_machine()
{
    static U16 loop_cnt=0;//给异步保存 sata log 留出时间
    if(tSataDC.tSDState == S_SD2 && tSataDC.tSyncEvent.pfEventCompleteCb == NULL && loop_cnt>=10)
    {
        loop_cnt=0;
        U32 size_per_event=0x10000;//类似单次TRIM的大小,默认32M。/*(tSataDC.dMaxLbaAddr4User>>16)*/;
        //DO SANITIZE创建 擦除任务
        tSataDC.tSyncEvent.bOpcode=OP_DI_SANITIZE_DEVICE;
        tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tSyncEvent.dParam[1]=tSataDC.wSanitizeProCnt*size_per_event;
        tSataDC.tSyncEvent.dParam[2]=size_per_event;
        tSataDC.tSyncEvent.pfEventCompleteCb=SanitizeEventDone;
        if((tSataDC.tSyncEvent.dParam[1] + tSataDC.tSyncEvent.dParam[2]) > tSataDC.dMaxLbaAddr4User)
        {
            tSataDC.tSyncEvent.dParam[2]=tSataDC.dMaxLbaAddr4User-tSataDC.tSyncEvent.dParam[1];//修正SIZE
        }
        //PRINT_TXT("SD:ST");
        //PRINT_HEX("LBA:",tSataDC.tSyncEvent.dParam[1]);
        //PRINT_HEX("SIZE:",tSataDC.tSyncEvent.dParam[2]);
    }
    loop_cnt++;
}
/*
 *
 *  00h Reason not reported or sanitize device command failed
    01h Sanitize Command Unsuccessful – The sanitize operation completed with
    physical sectors that are available to be allocated for user data that were
    not successfully sanitized.
    02h Invalid or unsupported value in the Sanitize Device FEATURE field
    03h Device is in the SD1: Sanitize Frozen state (see 4.17.9.3)
    04h SANITIZE FREEZE LOCK command failed as a result of the Sanitize
    Antifreeze Lock value (see 4.17.8) being set to one
 * */
CMD_STATUS ATACmd_Sd_0x0000_SanitizeStatusExt()
{
    CMD_STATUS status=CMD_VALID;
    if(((unFsmBuff.FIS.h2d.dw3.bits.bCount & 0x01) != 0) && tSataDC.bSDFMPVal == 0)
    {
        status = CMD_ABORT;
    }
    else if(tSataDC.tSDState == S_SD3 && ((unFsmBuff.FIS.h2d.dw3.bits.bCount & 0x01) == 0))
    {
        unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x01;
        status = CMD_ABORT;
    }
    else
    {
        unFsmBuff.FIS.d2h.dw3.bits.bExpCount=0;
        unFsmBuff.FIS.d2h.dw3.bits.bExpCount |= (tSataDC.bSDOCWOEVal<<7);
        unFsmBuff.FIS.d2h.dw1.bits.bLBA_low=0xFF;
        unFsmBuff.FIS.d2h.dw1.bits.bLBA_mid=0xFF;
        if(tSataDC.tSDState == S_SD2)
        {
            //报告 进度
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low=(tSataDC.wSanitizeProCnt&0xFF);
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_mid=((tSataDC.wSanitizeProCnt>>8)&0xFF);
            unFsmBuff.FIS.d2h.dw3.bits.bExpCount |= (1<<6);
        }
        else if(tSataDC.tSDState == S_SD1)
        {
            unFsmBuff.FIS.d2h.dw3.bits.bExpCount |= (1<<5);
        }
        else if(tSataDC.tSDState == S_SD3)
        {
            tSataDC.tSDState = S_SD0;
            WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存下tSataDC.tSDState的状态
        }
    }
    return status;
}

CMD_STATUS ATACmd_Sd_0x0011_CrtptoScrambleExt()
{
    if(!SD_CRYPTO || tSataDC.tSDState == S_SD1)
    {
        //不支持这种模式
        if(!SD_CRYPTO)
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x02;
        else
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x03;
        return CMD_ABORT;
    }
    else
    {

    }
    return CMD_DONE;
}

CMD_STATUS ATACmd_Sd_0x0012_BlockEraseExt()
{
    CMD_STATUS status=CMD_VALID;
    if(!SEC_ERASE || tSataDC.tSDState == S_SD1)
    {
        if(!SEC_ERASE)
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x02;
        else
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x03;
        status = CMD_ABORT;
    }
    else
    {
        U32 lba=(unFsmBuff.FIS.h2d.dw1.dAll&0x00FFFFFF);
        lba+=((U32)unFsmBuff.FIS.h2d.dw2.bits.bExpLBA_low<<24);
        if(lba != 0x426B4572 || tSataDC.tSDState == S_SD2 || tSataDC.tSDState == S_SD1)
        {
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x00;//default error
            status = CMD_ABORT;
        }
        else if((tSataDC.tSDState == S_SD3 || tSataDC.tSDState == S_SD4) &&
                tSataDC.bSDFMPVal == 0 &&
                ((unFsmBuff.FIS.h2d.dw3.bits.bCount & 0x10) != 0))
        {
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x00;//default error
            status = CMD_ABORT;
        }
        else
        {
            tSataDC.bSDFMPVal = ((unFsmBuff.FIS.h2d.dw3.bits.bCount>>4) & 0x01);
            tSataDC.tSDState = S_SD2;
            tSataDC.bSDOCWOEVal =0;
            tSataDC.wSanitizeProCnt=0;
            SG_DSATA->rHWD2HEN.dAll=0;//S_SD2状态退出启动NCQ 自动回复D2H功能
            WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存下tSataDC.tSDState的状态
        }
    }
    return status;
}

CMD_STATUS ATACmd_Sd_0x0014_OverwriteExt()
{
    if(!SEC_OVERWRITE || tSataDC.tSDState == S_SD1)
    {
        //不支持这种模式
        if(!SEC_OVERWRITE)
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x02;
        else
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x03;
        return CMD_ABORT;
    }
    else
    {

    }
    return CMD_DONE;
}

CMD_STATUS ATACmd_Sd_0x0020_FreezeLockExt()
{
    CMD_STATUS status=CMD_VALID;
    U32 lba=(unFsmBuff.FIS.h2d.dw1.dAll&0x00FFFFFF);
    lba+=((U32)unFsmBuff.FIS.h2d.dw2.bits.bExpLBA_low<<24);
    if((lba != 0x46724C6B) || (tSataDC.tSDState == S_SD1))
    {
        unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x00;//default error
        status = CMD_ABORT;
    }
    else
    {
        tSataDC.tSDState = S_SD1;
        WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存下tSataDC.tSDState的状态
    }
    return status;
}

CMD_STATUS ATACmd_Sd_0x0040_AntiFreezeLockExt()
{
    if(!SEC_ANTIFREE || tSataDC.tSDState == S_SD1)
    {
        if(!SEC_ANTIFREE)
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x02;
        else
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x03;
        return CMD_ABORT;
    }
    else
    {

    }
    return CMD_DONE;
}
// *****************************************************************************
// 函数名称: ATACmd_Sd_FeatureSet
// 函数功能: 实现ATA命令Sanitize device
// 入口参数: NONE
// 出口参数:
// 说    明: Sanitize device 0xB4
// *****************************************************************************
void ATACmd_Sd_FeatureSet(void)
{
    CMD_STATUS eStatus=CMD_DONE;
    U16 features=unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L+(unFsmBuff.FIS.h2d.dw2.bits.bFeatures_H<<8);

#if(SATA_SEC_ENABLE == TRUE)
        if(tSataDC.eSecState == ATA_SEC4 && features != 0x0000)
        {
            //存在用户密码，则先需要校验密码成功以后才可以开放读写命令
            features=0xFFFF;
        }
#endif

    switch(features)
    {
        case 0x0000:
            //SANITIZE STATUS EXT – B4h/0000h, Non-Data
            eStatus=ATACmd_Sd_0x0000_SanitizeStatusExt();
            break;
        case 0x0011:
            //CRYPTO SCRAMBLE EXT – B4h/0011h, Non-Data
            eStatus=ATACmd_Sd_0x0011_CrtptoScrambleExt();
            break;
        case 0x0012:
            //BLOCK ERASE EXT – B4h/0012h, Non-Data
            eStatus=ATACmd_Sd_0x0012_BlockEraseExt();
            break;
        case 0x0014:
            //OVERWRITE EXT – B4h/0014h, Non-Data
            eStatus=ATACmd_Sd_0x0014_OverwriteExt();
            break;
        case 0x0020:
            // SANITIZE FREEZE LOCK EXT – B4h/0020h, Non-Data
            eStatus=ATACmd_Sd_0x0020_FreezeLockExt();
            break;
        case 0x0040:
            //SANITIZE ANTIFREEZE LOCK EXT – B4h/0040h, Non-Data
            eStatus=ATACmd_Sd_0x0040_AntiFreezeLockExt();
            break;
        default:
            //没有这种情况
            unFsmBuff.FIS.d2h.dw1.bits.bLBA_low = 0x02;
            eStatus=CMD_ABORT;
            break;
    }
    ATACmd_ReleaseCommand(eStatus);//具体对应的接口已经在切换状态的时候注册完毕
}
#endif
