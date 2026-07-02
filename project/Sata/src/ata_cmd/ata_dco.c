// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_dco.c 
//  文件标识：    
//  内容摘要：    实现ATA8 Device Configuration Overlay (DCO) Feature Set管理 
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

#include "ata_dco.h"
#include "ata_identify.h"
#include "sata_pio_in.h"
#include "sata_pio_out.h"
#include "sata_dma_in.h"
#include "sata_dma_out.h"
#include "sata_misc.h"
#include "dsata.h"
#include "uitility.h"
#include "sata_typedef.h"
#include "debug.h"
#include "ata_gpl.h"
#include "ata_general.h"

#if(SATA_DCO_ENABLE == TRUE)
void UpdateIdfyFromDcoIdfy(void *pTable)
{
    U16 *wArray=pTable;

    ERROR_PRINT("unAtaIdentify.wArray[63]=%x\r\n",unAtaIdentify.wArray[63],unAtaIdentify.wArray[88]);

    //SetBits(unAtaIdentify.wArray+63,2,3,wArray[1] & 0x7);
    unAtaIdentify.wArray[63]&=(~(0x7));
    unAtaIdentify.wArray[63]|=((wArray[1] & 0x7));
    //SetBits(unAtaIdentify.wArray+88,6,7,wArray[2] & 0x7F);
    unAtaIdentify.wArray[88]&=(~(0x7F));
    unAtaIdentify.wArray[88]|=((wArray[2] & 0x7F));
    ERROR_PRINT("wArray[1]=%x,wArray[2]=%x\r\n",wArray[1],wArray[2]);
    ERROR_PRINT("unAtaIdentify.wArray[63]=%x,unAtaIdentify.wArray[88]=%x\r\n",unAtaIdentify.wArray[63],unAtaIdentify.wArray[88]);

    UN_BITS* data=(UN_BITS*)&wArray[7];
    //SetBits(unAtaIdentify.wArray+119,1,1,data->b.bit14);
    unAtaIdentify.wArray[119]&=(~(1<<1));
    unAtaIdentify.wArray[119]|=(data->b.bit14<<1);
    if(data->b.bit14 == 0)
    {
        //SetBits(unAtaIdentify.wArray+120,1,1,0);
        unAtaIdentify.wArray[120]&=~(W_BIT1);
    }
    //13,12,11 smart
    //SetBits(unAtaIdentify.wArray+84,4,1,data->b.bit9);
    unAtaIdentify.wArray[84]&=(~(1<<4));
    unAtaIdentify.wArray[84]|=((data->b.bit9)<<4);
    if(data->b.bit9 == 0)
    {
        //SetBits(unAtaIdentify.wArray+84,9,1,0);
        //SetBits(unAtaIdentify.wArray+84,10,1,0);
        unAtaIdentify.wArray[84]&=~(W_BIT9|W_BIT10);

        unAtaIdentify.wArray[95] =0;
        unAtaIdentify.wArray[96] =0;
        unAtaIdentify.wArray[97] =0;
        unAtaIdentify.wArray[98] =0;
        unAtaIdentify.wArray[99] =0;
        unAtaIdentify.wArray[104]=0;

    }

    //SetBits(unAtaIdentify.wArray+83,10,1,data->b.bit8);
    unAtaIdentify.wArray[83]&=(~(1<<10));
    unAtaIdentify.wArray[83]|=((data->b.bit8)<<10);
    if(data->b.bit8 == 0)
    {
        unAtaIdentify.wArray[100]=0;
        unAtaIdentify.wArray[101]=0;
        unAtaIdentify.wArray[102]=0;
        unAtaIdentify.wArray[103]=0;
    }

    //SetBits(unAtaIdentify.wArray+82,10,1,data->b.bit7);
    unAtaIdentify.wArray[82]&=(~(1<<10));
    unAtaIdentify.wArray[82]|=((data->b.bit7)<<10);
    if(data->b.bit7 == 0)
    {
        //SetBits(unAtaIdentify.wArray+83,8,1,0);
        unAtaIdentify.wArray[83]&=~(W_BIT8);
        //SetBits(unAtaIdentify.wArray+85,10,1,0);
        unAtaIdentify.wArray[85]&=~(W_BIT10);
        //SetBits(unAtaIdentify.wArray+86,8,1,0);
        unAtaIdentify.wArray[86]&=~(W_BIT8);
    }

    //SetBits(unAtaIdentify.wArray+83,9,1,data->b.bit6);
    unAtaIdentify.wArray[83]&=(~(1<<9));
    unAtaIdentify.wArray[83]|=((data->b.bit6)<<9);
    if(data->b.bit6 == 0)
    {
        unAtaIdentify.wArray[94]=0;
    }

    //SetBits(unAtaIdentify.wArray+83,6,2,data->b.bit4);
    unAtaIdentify.wArray[83]&=(~(0x3<<5));
    unAtaIdentify.wArray[83]|=((data->b.bit4)<<5);
    unAtaIdentify.wArray[83]|=((data->b.bit4)<<6);
    if(data->b.bit4 == 0)
    {
        //SetBits(unAtaIdentify.wArray+86,6,2,0);
        unAtaIdentify.wArray[86]&=~(W_BIT6|W_BIT5);
    }

    //SetBits(unAtaIdentify.wArray+82,1,1,data->b.bit3);
    unAtaIdentify.wArray[82]&=(~(1<<1));
    unAtaIdentify.wArray[82]|=((data->b.bit3)<<1);
    if(data->b.bit3 == 0)
    {
        /*
         * If bit 3 of word 7 is cleared to zero, and security is enabled, then the device shall return command aborted for the
            DEVICE CONFIGURATION SET command
         * */
        //SetBits(unAtaIdentify.wArray+85,1,1,0);
        unAtaIdentify.wArray[85]&=~(W_BIT1);
        unAtaIdentify.wArray[89]=0;
        unAtaIdentify.wArray[90]=0;
        unAtaIdentify.wArray[92]=0;
        unAtaIdentify.wArray[108]=0;
    }

    //SetBits(unAtaIdentify.wArray+84,0,1,data->b.bit2);
    unAtaIdentify.wArray[84]&=(~(1));
    unAtaIdentify.wArray[84]|=((data->b.bit2));
    if(data->b.bit2 == 0)
    {
        //SetBits(unAtaIdentify.wArray+87,0,1,0);
        unAtaIdentify.wArray[87]&=~(W_BIT0);
    }

    //SetBits(unAtaIdentify.wArray+84,1,1,data->b.bit1);
    unAtaIdentify.wArray[84]&=(~(1<<1));
    unAtaIdentify.wArray[84]|=((data->b.bit1)<<1);
    if(data->b.bit1 == 0)
    {
        //SetBits(unAtaIdentify.wArray+87,1,1,0);
        unAtaIdentify.wArray[87]&=~(W_BIT1);
    }

    //SetBits(unAtaIdentify.wArray+82,0,1,data->b.bit0);
    unAtaIdentify.wArray[82]&=(~(1));
    unAtaIdentify.wArray[82]|=((data->b.bit0));
    if(data->b.bit0 == 0)
    {
        //SetBits(unAtaIdentify.wArray+85,0,1,0);
        unAtaIdentify.wArray[85]&=~(W_BIT0);
    }

    data=(UN_BITS*)&wArray[8];
    //SetBits(unAtaIdentify.wArray+78,6,1,data->b.bit4);
    unAtaIdentify.wArray[78]&=(~(1<<6));
    unAtaIdentify.wArray[78]|=((data->b.bit4)<<6);
    if(data->b.bit4 == 0)
    {
        //SetBits(unAtaIdentify.wArray+79,6,1,0);
        unAtaIdentify.wArray[79]&=~(W_BIT6);
        //clear any software settings that would be cleared when the SSP feature set is disabled
    }

    //SetBits(unAtaIdentify.wArray+78,5,1,data->b.bit3);
    unAtaIdentify.wArray[78]&=(~(1<<5));
    unAtaIdentify.wArray[78]|=((data->b.bit3)<<5);
    if(data->b.bit3 == 0)
    {
        //SetBits(unAtaIdentify.wArray+79,5,1,0);
        unAtaIdentify.wArray[79]&=~(W_BIT5);
    }

    //SetBits(unAtaIdentify.wArray+76,9,1,data->b.bit2);//HIPM
    unAtaIdentify.wArray[76]&=(~(1<<9));
    unAtaIdentify.wArray[76]|=((data->b.bit2)<<9);
    if(data->b.bit2 == 0)
    {
        tSataDC.bIsDIPMEn=FALSE;
        SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;
        //SetBits(unAtaIdentify.wArray+78,3,1,0);//DIPM
        unAtaIdentify.wArray[78]&=~(W_BIT3);
        //SetBits(unAtaIdentify.wArray+79,3,1,0);
        //SetBits(unAtaIdentify.wArray+79,7,1,0);
        unAtaIdentify.wArray[79]&=~(W_BIT3|W_BIT7);
        //SetBits(unAtaIdentify.wArray+76,14,1,0);//AP2S
        unAtaIdentify.wArray[76]&=~(W_BIT14);
        tSataDC.bIsHIPMEn=FALSE;
        SG_DSATA->rPSCTLR.bits.IPM=LPM_DISABLE_BOTH;//禁用
        SG_DSATA->rPCMDR.bits.APSTE=0;
    }

    //SetBits(unAtaIdentify.wArray+78,1,1,data->b.bit1);
    unAtaIdentify.wArray[78]&=(~(1<<1));
    unAtaIdentify.wArray[78]|=((data->b.bit1)<<1);
    //SetBits(unAtaIdentify.wArray+78,4,1,data->b.bit1);
    unAtaIdentify.wArray[78]&=(~(1<<4));
    unAtaIdentify.wArray[78]|=((data->b.bit1)<<4);

    if(data->b.bit1 == 0)
    {
        //SetBits(unAtaIdentify.wArray+78,4,1,0);
        unAtaIdentify.wArray[78]&=~(W_BIT4);
        //SetBits(unAtaIdentify.wArray+79,4,1,0);
        unAtaIdentify.wArray[79]&=~(W_BIT4);
    }

    //SetBits(unAtaIdentify.wArray+76,8,1,data->b.bit0);
    unAtaIdentify.wArray[76]&=(~(1<<8));
    unAtaIdentify.wArray[76]|=((data->b.bit0)<<8);
    SG_DSATA->rHWD2HEN.dAll=data->b.bit0;
    if(data->b.bit0 == 0)
    {
        //DISABLE ncq cmd
        //SetBits(unAtaIdentify.wArray+78,1,1,0);
        //SetBits(unAtaIdentify.wArray+78,2,1,0);
        //SetBits(unAtaIdentify.wArray+78,4,1,0);
        unAtaIdentify.wArray[78]&=~(W_BIT1|W_BIT2|W_BIT4);
        //SetBits(unAtaIdentify.wArray+79,1,1,0);
        //SetBits(unAtaIdentify.wArray+79,2,1,0);
        //SetBits(unAtaIdentify.wArray+79,4,1,0);
        unAtaIdentify.wArray[79]&=~(W_BIT1|W_BIT2|W_BIT4);
    }

    data=(UN_BITS*)&wArray[21];
    /*
     * If bit 12 of word 21 is cleared to zero, and security is enabled, then the device shall return command aborted for
    the DEVICE CONFIGURATION SET command
     * */
    if(data->b.bit9 == 0)
    {
        //SetBits(unAtaIdentify.wArray+119,7,1,0);
        unAtaIdentify.wArray[119]&=~(W_BIT7);
        //SetBits(unAtaIdentify.wArray+120,7,1,0);
        unAtaIdentify.wArray[120]&=~(W_BIT7);
    }
    tSataDC.eDCOState=DCO_Reduced_config;
}

void Build_Dco_Idfy(void *pTable)
{
    U16 *wArray=pTable;
    Mem32SetVal((U32*)pTable,0,128);//清零

    wArray[0]=0x0002;//head
    /*
     * Multiword DMA modes supported
        15:3 Reserved
        2 1 = Reporting support for Multiword DMA mode 2 and below is changeable
        1 1 = Reporting support for Multiword DMA mode 1 and below is changeable
        0 1 = Reporting support for Multiword DMA mode 0 is changeable
     * */
    wArray[1]=0x0007;

    /*
     * Ultra DMA modes supported
        15:7 Reserved
        6 1 = Reporting support for Ultra DMA mode 6 and below is changeable
        5 1 = Reporting support for Ultra DMA mode 5 and below is changeable
        4 1 = Reporting support for Ultra DMA mode 4 and below is changeable
        3 1 = Reporting support for Ultra DMA mode 3 and below is changeable
        2 1 = Reporting support for Ultra DMA mode 2 and below is changeable
        1 1 = Reporting support for Ultra DMA mode 1 and below is changeable
        0 1 = Reporting support for Ultra DMA mode 0 is changeable
     * */
    wArray[2]=0x007F;

    /*
     *  Maximum LBA (QWord)
        63:48 Reserved
        47:0 Maximum LBA
     * */
    wArray[3]=unAtaIdentify.wArray[100];
    wArray[4]=unAtaIdentify.wArray[101];
    wArray[5]=unAtaIdentify.wArray[102];
    wArray[6]=unAtaIdentify.wArray[103];

    /*
     *  Command set/feature set supported part 1
        15 Reserved
        14 1 = Reporting support for the Write-Read-Verify feature set is changeable   1
        13 1 = Reporting support for the SMART Conveyance self-test is changeable      0
        12 1 = Reporting support for the SMART Selective self-test is changeable       1
        11 1 = Reporting support for the Forced Unit Access is changeable              1
        10 Reserved for TLC  														   0
        9 1 = Reporting support for the Streaming feature set is changeable            0
        8 1 = Reporting support for the 48-bit Addressing feature set is changeable    1
        7 1 = Reporting support for the HPA feature set is changeable                  1
        6 1 = Reporting support for the AAM feature set is changeable                  0
        5 This field is obsolete.                                                      0
        4 1 = Reporting support for the PUIS feature set is changeable                 0
        3 1 = Reporting support for the Security feature set is changeable             1
        2 1 = Reporting support for the SMART error log is changeable                  1
        1 1 = Reporting support for the SMART self-test is changeable                  1
        0 1 = Reporting support for the SMART feature set is changeable                1
     * */
    wArray[7]=0x598F;

    /*
        Serial ATA Command set/feature set supported
        15:5 Reserved for Serial ATA
        4 1 = Reporting support for the SSP feature set is changeable					1
        3 1 = Reporting support for asynchronous notification is changeable				0
        2 1 = Reporting support for interface power management is changeable 			1
        1 1 = Reporting support for non-zero buffer offsets is changeable				0
        0 1 = Reporting support for the NCQ feature set is changeable					1
     * */

    wArray[8]=0x0015;
    /*
     *
     * Command set/feature set supported part 2
        15 1 = Reporting support for the NV Cache feature set is changeable
        14 1 = Reporting support for the NV Cache Power Management feature set is changeable
        13 1 = Reporting support for WRITE UNCORRECTABLE EXT is changeable
        12 1 = Reporting of support for the Trusted Computing feature set is changeable
        11 1 = Reporting support for the Free-fall Control feature set is changeable
        10 1 = Reporting support for the DATA SET MANAGEMENT command is changeable
        9 1 = Reporting support for Extended Power Conditions is changeable
        8:0 Reserved
     * */

    wArray[16]=0x0001;//SMI 这样配置
    /*
    Command set/feature set supported part 2
    15 1 = Reporting support for the NV Cache feature set is changeable
    14 1 = Reporting support for the NV Cache Power Management feature set is changeable
    13 1 = Reporting support for WRITE UNCORRECTABLE EXT is changeable
    12 1 = Reporting of support for the Trusted Computing feature set is changeable
    11 1 = Reporting support for the Free-fall Control feature set is changeable
    10 1 = Reporting support for the DATA SET MANAGEMENT command is changeable
    9 1 = Reporting support for Extended Power Conditions is changeable
    8:0 Reserved
    */
    wArray[21]=0x3400;
    wArray[255]=0x00A5;

    UpdateCheckSum((U8*)wArray,512);
}
#if(SATA_LOG_ENABLE == 1)
void UpdateDCODataDone(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_DI_DATA;
    __pevent->eStatus=CMD_ONGOING;
    __pevent->dParam[0]=DIR_IN;
    __pevent->dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + IDY_DEV_DATA_LOG_OFFSET + 8;
    __pevent->dParam[2]=1;
    __pevent->dParam[3]=(U32)tSyncBuffer.bArray;
    __pevent->pfEventCompleteCb=UpdateIDFYLogPage08Router;
}

void UpdateDCOData()
{
    tSataDC.tSyncEvent.bOpcode=OP_DI_DATA;
    tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
    tSataDC.tSyncEvent.dParam[0]=DIR_OUT;
    tSataDC.tSyncEvent.dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + DEVICE_DCO_SET_OFFSET;
    tSataDC.tSyncEvent.dParam[2]=1;
    tSataDC.tSyncEvent.dParam[3]=(U32)tSyncBuffer.wArray;
    tSataDC.tSyncEvent.pfEventCompleteCb=UpdateDCODataDone;
}
#endif
// *****************************************************************************
// 函数名称: ATACmd_Dco_0xC0_DevCfgRestore
// 函数功能: DEVICE CONFIGURATION RESTORE - B1h/C0h, Non-Data
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_0xC0_DevCfgRestore(void)
{
    if(tSataDC.bIsDcoFreezeLocked == TRUE)
    {
        return CMD_ABORT;
    }
    else
    {
        tSataDC.eDCOState=DCO_Factory_config;
        Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
        Update_IdfyParam();
#if(SATA_LOG_ENABLE == 1)
        WriterGPLLogsRouter(CMD_IDLE,NULL);
#endif
    }
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Dco_0xC1_DevCfgFreezeLock
// 函数功能: DEVICE CONFIGURATION FREEZE LOCK - B1h/C1h, Non-Data
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_0xC1_DevCfgFreezeLock(void)
{
    if(tSataDC.bIsDcoFreezeLocked == TRUE)
    {
        return CMD_ABORT;
    }
    else
    {
        tSataDC.bIsDcoFreezeLocked=TRUE;
    }
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Dco_0xC2_DevCfgIdentify
// 函数功能: DEVICE CONFIGURATION IDENTIFY - B1h/C2h, PIO Data-In
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_0xC2_DevCfgIdentify(void)
{
    if(tSataDC.bIsDcoFreezeLocked == TRUE)
    {
        return CMD_ABORT;
    }
    else
    {
        SATA_ChangeState(PIO_DATA_IN,0); 
        Build_Dco_Idfy((void*)tSyncBuffer.wArray);
    }
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Dco_0xC3_DevCfgSet
// 函数功能:  DEVICE CONFIGURATION SET - B1h/C3h, PIO Data-Out
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_0xC3_DevCfgSet(void)
{
    if(tSataDC.bIsDcoFreezeLocked == TRUE || tSataDC.eDCOState== DCO_Reduced_config)
    {
        return CMD_ABORT;
    }
    else
    {
        SATA_ChangeState(PIO_DATA_OUT,0); 
    }
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Dco_0xC4_DevCfgIdentifyDma
// 函数功能: DEVICE CONFIGURATION IDENTIFY DMA - B1h/C4h, DMA
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_0xC4_DevCfgIdentifyDma(void)
{
    if(tSataDC.bIsDcoFreezeLocked == TRUE)
    {
        return CMD_ABORT;
    }
    else
    {
        SATA_ChangeState(DMA_DATA_IN,0); 
        Build_Dco_Idfy((void*)tSyncBuffer.wArray);
    }
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Dco_0xC5_DevCfgSetDma
// 函数功能: DEVICE CONFIGURATION SET DMA - B1h/C5h, DMA
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_0xC5_DevCfgSetDma(void)
{
    if(tSataDC.bIsDcoFreezeLocked == TRUE || tSataDC.eDCOState== DCO_Reduced_config)
    {
        return CMD_ABORT;
    }
    else
    {
        SATA_ChangeState(DMA_DATA_OUT,0); 
    }
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Dco_CheckData
// 函数功能: DEVICE CONFIGURATION SET DMA - B1h/C5h, DMA
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明: 
// *****************************************************************************
CMD_STATUS ATACmd_Dco_CheckData(void)
{
    //数据有效性检查CHECK SUM 等
    if(GetSum(tSyncBuffer.bArray,512) == 0)
    {
        //数据有效，进行后续处理,后续可能还有其他检测项
        UpdateIdfyFromDcoIdfy((void*)tSyncBuffer.wArray);
        return CMD_VALID;
    }
    else
    {
        ERROR_PRINT("abort dco wArray[1]=%x,wArray[2]=%x\r\n",tSyncBuffer.wArray[1],tSyncBuffer.wArray[2]);
        return CMD_ABORT;
    }
}
// *****************************************************************************
// 函数名称: ATACmd_DcoDataReceived
// 函数功能: DMA数据接收完成事务处理
// 入口参数: eStatus 数据接收状态
//          ptParam数据接收任务相关参数
// 出口参数: 
// 说    明: 该接口由传输层回调
// *****************************************************************************
void ATACmd_DcoDataReceived(CMD_STATUS eStatus,void *ptParam)
{
    //数据接收完成
    eStatus = ATACmd_Dco_CheckData();
#if(SATA_LOG_ENABLE == 1)
    if(eStatus!=CMD_ABORT)
    {
        UpdateDCOData();
    }
    else
    {
        ATACmd_ReleaseCommand(eStatus);
    }
#else
    ATACmd_ReleaseCommand(eStatus);
#endif

}

// *****************************************************************************
// 函数名称: ATACmd_DcoFeatureSet
// 函数功能: 实现ATA命令 Device Configuration Overlay
// 入口参数: NONE
// 出口参数: 
// 说    明: Device Configuration Overlay (DCO) 0xB1
// *****************************************************************************
void ATACmd_DcoFeatureSet(void)
{
    CMD_STATUS eStatus=CMD_DONE;
    T_HI_TASK tTask;
    UN_CH_DI unDI;
    REQ_TYPE eType=REQ_STATUS;
    TransferCompleteCb_t *pfFunc=NULL;
    unDI.dAll=0x80;
    tTask.tParam.dBuffAddr=(U32)&tSyncBuffer.bArray[0];
    tTask.tParam.dSctSize=1;
    tTask.tParam.bTag=CMD_SLOT0;

    switch(unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L)
    {
    case 0xC0:
        //DEVICE CONFIGURATION RESTORE - B1h/C0h, Non-Data
        eStatus=ATACmd_Dco_0xC0_DevCfgRestore();
        break;
    case 0xC1:
        //DEVICE CONFIGURATION FREEZE LOCK - B1h/C1h, Non-Data
        eStatus=ATACmd_Dco_0xC1_DevCfgFreezeLock();
        break;
    case 0xC2:
        //DEVICE CONFIGURATION IDENTIFY - B1h/C2h, PIO Data-In
        unDI.dAll=0xC0;
        eStatus=ATACmd_Dco_0xC2_DevCfgIdentify();
        if(eStatus != CMD_ABORT)
        {
            eType=REQ_TRANSFER;
        }
        break;
    case 0xC3:
        //DEVICE CONFIGURATION SET - B1h/C3h, PIO Data-Out
        eStatus=ATACmd_Dco_0xC3_DevCfgSet();
        if(eStatus != CMD_ABORT)
        {
            pfFunc=ATACmd_DcoDataReceived;
            eType=REQ_TRANSFER;
        }
        break;
    case 0xC4:
        unDI.dAll=0x40;
        // DEVICE CONFIGURATION IDENTIFY DMA - B1h/C4h, DMA
        eStatus=ATACmd_Dco_0xC4_DevCfgIdentifyDma();
        if(eStatus != CMD_ABORT)
        {
            eType=REQ_TRANSFER;
        }
        break;
    case 0xC5:
        //DEVICE CONFIGURATION SET DMA - B1h/C5h, DMA
        unDI.dAll=0x00;
        eStatus=ATACmd_Dco_0xC5_DevCfgSetDma();
        if(eStatus != CMD_ABORT)
        {
            pfFunc=ATACmd_DcoDataReceived;
            eType=REQ_TRANSFER;
        }
        break;
    default:
        //没有这种情况
        SATA_ChangeState(NONE_DATA,0); 
        eStatus=CMD_ABORT;
        break;
    }
    tTask.eStatus=eStatus;
    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,tSataDC.bDrq,1<<9);
    tSataDC.DOS_TransferReq(eType,&tTask,pfFunc);//具体对应的接口已经在切换状态的时候注册完毕
}
#endif
