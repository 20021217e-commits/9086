// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司
//   
//  文件名称：  ata_cmd_list.c
//  文件标识：
//  内容摘要：  ata 命令及其属性实例化
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
#include "debug.h"
#include "ata_cmd_list.h" 
#include "ata_identify.h"
#include "ata_general.h"
#include "ata_fpdma.h"
#include "ata_gpl.h"
#include "ata_dco.h"
#include "ata_smart.h"
#include "ata_power_mg.h"
#include "ata_security.h"
#include "ata_sanitize.h"
#include "ata_hpa.h"

#if (SATA_LOG_ENABLE == 1)
//建立表格的方法二，码值匹配法
const T_ATA_CMD tATACmdList_C[]={
    
    //NCQ，数据传输相关
#if (D2H_AUTO_SEND == FALSE)
    {0x60,FPDMA_QUEUED,MODE_48BIT,MODE_LOCKED,ATACmd_ReadFpdmaQueued}, //READ FPDMA QUEUED -60h, DMA Queued
    {0x61,FPDMA_QUEUED,MODE_48BIT,MODE_LOCKED,ATACmd_WriteFpdmaQueued},//WRITE FPDMA QUEUED -61h, DMA Queued
#endif

    {0xC8,DMA_DATA_IN,MODE_28BIT,MODE_LOCKED,ATACmd_ReadDma},//READ DMA -C8h, DMA
    {0xCA,DMA_DATA_OUT,MODE_28BIT,MODE_LOCKED,ATACmd_WriteDma},//WRITE DMA- CAh, DMA

    {0x25,DMA_DATA_IN,MODE_48BIT,MODE_LOCKED,ATACmd_ReadDmaExt},//READ DMA EXT - 25h, DMA
    {0x35,DMA_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_WriteDmaExt},//WRITE DMA EXT - 35h, DMA
    {0x3D,DMA_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_WriteDmaFuaExt},//WRITE DMA EXT - 3Dh, DMA

    {0xC4,PIO_DATA_IN,MODE_28BIT,MODE_LOCKED,ATACmd_ReadMultiple},//READ MULTIPLE -C4h, PIO Data-In
    {0x29,PIO_DATA_IN,MODE_48BIT,MODE_LOCKED,ATACmd_ReadMultipleExt},//READ MULTIPLE EXT -29h, PIO Data-In
    {0x20,PIO_DATA_IN,MODE_28BIT,MODE_LOCKED,ATACmd_ReadMultiple},//READ SECTOR -20h, PIO Data-In
    {0x24,PIO_DATA_IN,MODE_48BIT,MODE_LOCKED,ATACmd_ReadMultipleExt},//READ SECTOR EXT -24h, PIO Data-In
    
    {0xC5,PIO_DATA_OUT,MODE_28BIT,MODE_LOCKED,ATACmd_WriteMultiple},//WRITE MULTIPLE -C5h, PIO Data-Out
    {0x39,PIO_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_WriteMultipleExt},//WRITE MULTIPLE EXT -39h, PIO Data-Out
    {0x30,PIO_DATA_OUT,MODE_28BIT,MODE_LOCKED,ATACmd_WriteMultiple},//WRITE SECTOR -30h, PIO Data-Out
    {0x34,PIO_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_WriteMultipleExt},//WRITE SECTOR EXT -34h, PIO Data-Out
    
    {0xCE,PIO_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_WriteMultipleFuaExt},//WRITE MULTIPLE FUA EXT -CEh, PIO Data-Out
    {0xC6,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_SetMultipleMode},//SET MULTIPLE MODE - C6h, Non-Data

    //FLUSH
    {0xE7,NONE_DATA,MODE_28BIT,MODE_LOCKED,ATACmd_FlushCache},// FLUSH CACHE - E7h, Non-Data
    {0xEA,NONE_DATA,MODE_48BIT,MODE_LOCKED,ATACmd_FlushCache},// FLUSH CACHE EXT- EAh, Non-Data

    //省电管理
    {0xE0,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_StandbyImmediate},//STANDBY IMMEDIATE – E0h, Non-Data
    {0xE1,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_IdleImmediate},//IDLE IMMEDIATE – E1h, Non-Data
    {0xE2,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_Standby},//STANDBY – E2h, Non-Data
    {0xE3,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_Idle},//IDLE – E3h, Non-Data
    {0xE5,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_CheckPowerMode},//CHECK POWER MODE – E5h, Non-Data
    {0xE6,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_Sleep},// E6h, Non-Data
    
    //GPL相关
    {0x47,DMA_DATA_IN,MODE_48BIT,MODE_LOCKED,ATACmd_ReadLogDmaExt},//READ LOG DMA EXT - 47h, DMA
    {0x57,DMA_DATA_OUT,MODE_48BIT,MODE_UNLOCKED,ATACmd_WriteLogDmaExt},//WRITE LOG DMA EXT - 57h, DMA
#ifndef __JUPITER_DEBUG__
    {0x2F,PIO_DATA_IN,MODE_48BIT,MODE_UNLOCKED,ATACmd_ReadLogExt},//READ LOG EXT - 2Fh, PIO Data-In
#endif
    {0x3F,PIO_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_WriteLogExt},//WRITE LOG EXT - 3Fh, PIO Data-Out
    
    {0xE4,PIO_DATA_IN,MODE_28BIT,MODE_UNLOCKED,ATACmd_ReadBuff},//READ BUFF - E8h, PIO Data-In
    {0xE8,PIO_DATA_OUT,MODE_28BIT,MODE_UNLOCKED,ATACmd_WriteBuff},//WRITE BUFF - E4h, PIO Data-Out
    
        
    {0xE9,DMA_DATA_IN,MODE_28BIT,MODE_UNLOCKED,ATACmd_ReadBuffDma},//READ BUFF dma - E9h, dma Data-In
    {0xEB,DMA_DATA_OUT,MODE_28BIT,MODE_UNLOCKED,ATACmd_WriteBuffDma},//WRITE BUFF dma- Ebh, dma Data-Out

    //低频率使用
    {0xEC,PIO_DATA_IN,MODE_28BIT,MODE_UNLOCKED,ATACmd_Identify},//IDENTIFY DEVICE - ECh, PIO Data-In
#ifndef __JUPITER_DEBUG__
    {0xEF,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_SetFeatures},//SET FEATURES -EFh, Non-Data
#endif
    {0x90,EXECUTE_DEVICE_DIGA,MODE_28BIT,MODE_UNLOCKED,ATACmd_ExeDevDiagnostic},//EXECUTE DEVICE DIAGNOSTIC 90h, Execute Device Diagnostic

    //SECURITY
    {0xF5,NONE_DATA,MODE_28BIT,MODE_LOCKED,ATACmd_SecurityFreezeLock},//SECURITY FREEZE LOCK - F5h, Non-Data
#if(SATA_SEC_ENABLE == TRUE)
    {0xF1,PIO_DATA_OUT,MODE_28BIT,MODE_LOCKED,ATACmd_SecuritySetPassWord},//SECURITY SET PASSWORD – F1h, PIO Data-Out
    {0xF4,PIO_DATA_OUT,MODE_28BIT,MODE_UNLOCKED,ATACmd_SecurityEraseUnit},//SECURITY ERASE UNIT – F4h, PIO Data-Out
    {0xF6,PIO_DATA_OUT,MODE_28BIT,MODE_LOCKED,ATACmd_SecurityDisablePassword},//SECURITY DISABLE PASSWORD – F6h, PIO Data-Out
    {0xF3,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_SecurityErasePrepare},//SECURITY ERASE PREPARE – F3h, Non-Data
    {0xF2,PIO_DATA_OUT,MODE_28BIT,MODE_UNLOCKED,ATACmd_SecurityUnLock},//SECURITY UNLOCK – F2h, PIO Data-Out
#endif

    //DCO
#if (SATA_DCO_ENABLE == 1)
    {0xB1,UNKNOWN_YET,MODE_28BIT,MODE_LOCKED,ATACmd_DcoFeatureSet},//Device Configuration Overlay (0xB1)
#endif

#if (SATA_SD_ENABLE == 1)
    {0xB4,UNKNOWN_YET,MODE_48BIT,MODE_UNLOCKED,ATACmd_Sd_FeatureSet},//Device sanitize (0xB4)
#endif
#ifndef __JUPITER_DEBUG__
    //SMART
    {0xB0,UNKNOWN_YET,MODE_28BIT,MODE_UNLOCKED,ATACmd_SmartFeatureSet},//SMART (0xB0)
#endif
    {0x91,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_InitDevParam},// INITIALIZE DEVICE PARAMETERS - 91h, Non-Data - ATA4
    
    {0x10,NONE_DATA,MODE_28BIT,MODE_LOCKED,ATACmd_LegacyCmdSupport},//RECALIBRATE (0x10)
    {0x70,NONE_DATA,MODE_28BIT,MODE_LOCKED,ATACmd_LegacyCmdSupport},//SEEK (0x70)
    {0x27,NONE_DATA,MODE_48BIT,MODE_LOCKED,ATACmd_ReadNativeMaxAddrExt},//read native ext (0x27)
    {0xF8,NONE_DATA,MODE_28BIT,MODE_LOCKED,ATACmd_ReadNativeMaxAddr},//read native (0xF8)
    //{0x00,NONE_DATA,MODE_28BIT,MODE_UNLOCKED,ATACmd_LegacyCmdSupport},//NOP (0x00) SMI INTEL都回复status 51 error 04，注释掉就是这种效果
    {0x40,NONE_DATA,MODE_28BIT,MODE_LOCKED,ATACmd_ReadVerify},//READVFY (0x40)   --NOTE:以后需要具体实现,扩展SATA LOG接口
    {0x42,NONE_DATA,MODE_48BIT,MODE_LOCKED,ATACmd_ReadVerify},//RDVFYEXT (0x42)
    {0x06,DMA_DATA_OUT,MODE_48BIT,MODE_LOCKED,ATACmd_DataSetManagement},//DATA SET MANAGEMENT - 06h, DMA--NOTE:以后需要具体实现，扩展SATA LOG接口
};
#else

const T_ATA_CMD tATACmdList_C[]={

#ifndef __BOOT_CODE__
    //NCQ，数据传输相关
    {0xA1,PIO_DATA_IN,ATACmd_Refuse},//IDENTIFY DEVICE - A1h, PIO Data-In//AMD平台发现的命令，在Jupiter平台设置为拒绝访问
    {0x60,FPDMA_QUEUED,ATACmd_ReadFpdmaQueued}, //READ FPDMA QUEUED -60h, DMA Queued
    {0x61,FPDMA_QUEUED,ATACmd_WriteFpdmaQueued},//WRITE FPDMA QUEUED -61h, DMA Queued

    {0x25,DMA_DATA_IN,ATACmd_ReadDmaExt},//READ DMA EXT - 25h, DMA
    {0x35,DMA_DATA_OUT,ATACmd_WriteDmaExt},//WRITE DMA EXT - 35h, DMA
    {0x3D,DMA_DATA_OUT,ATACmd_WriteDmaFuaExt},//WRITE DMA EXT - 3Dh, DMA
    {0x24,PIO_DATA_IN,ATACmd_ReadMultipleExt},//READ SECTOR EXT -24h, PIO Data-In
    {0x39,PIO_DATA_OUT,ATACmd_WriteMultipleExt},//WRITE MULTIPLE EXT -39h, PIO Data-Out
    {0x29,PIO_DATA_IN,ATACmd_ReadMultipleExt},//READ MULTIPLE EXT -29h, PIO Data-In
    {0x34,PIO_DATA_OUT,ATACmd_WriteMultipleExt},//WRITE SECTOR EXT -34h, PIO Data-Out
    {0xCE,PIO_DATA_OUT,ATACmd_WriteMultipleFuaExt},//WRITE MULTIPLE FUA EXT -CEh, PIO Data-Out
#else
    {0x25,DMA_DATA_IN,ATACmd_ReadDma},//READ DMA EXT - 25h, DMA
    {0x35,DMA_DATA_OUT,ATACmd_WriteDma},//WRITE DMA EXT - 35h, DMA
#endif

    {0xC8,DMA_DATA_IN,ATACmd_ReadDma},//READ DMA -C8h, DMA
    {0xCA,DMA_DATA_OUT,ATACmd_WriteDma},//WRITE DMA- CAh, DMA

    {0xC6,NONE_DATA,ATACmd_SetMultipleMode},//SET MULTIPLE MODE - C6h, Non-Data
    {0xC4,PIO_DATA_IN,ATACmd_ReadMultiple},//READ MULTIPLE -C4h, PIO Data-In
    {0x20,PIO_DATA_IN,ATACmd_ReadMultiple},//READ SECTOR -20h, PIO Data-In
    {0x30,PIO_DATA_OUT,ATACmd_WriteMultiple},//WRITE SECTOR -30h, PIO Data-Out
    {0xC5,PIO_DATA_OUT,ATACmd_WriteMultiple},//WRITE MULTIPLE -C5h, PIO Data-Out

    //FLUSH
    {0xE7,NONE_DATA,ATACmd_FlushCache},// FLUSH CACHE - E7h, Non-Data
    {0xEA,NONE_DATA,ATACmd_FlushCache},// FLUSH CACHE EXT- EAh, Non-Data

#ifndef __BOOT_CODE__
    //GPL相关
    {0x47,DMA_DATA_IN,ATACmd_ReadLogDmaExt},//READ LOG DMA EXT - 47h, DMA
    {0x57,DMA_DATA_OUT,ATACmd_WriteLogDmaExt},//WRITE LOG DMA EXT - 57h, DMA
#ifndef __JUPITER_DEBUG__
    {0x2F,PIO_DATA_IN,ATACmd_ReadLogExt},//READ LOG EXT - 2Fh, PIO Data-In
#endif
    {0x3F,PIO_DATA_OUT,ATACmd_WriteLogExt},//WRITE LOG EXT - 3Fh, PIO Data-Out
#endif
    //低频率使用
    {0xEC,PIO_DATA_IN,ATACmd_Identify},//IDENTIFY DEVICE - ECh, PIO Data-In

//    {0xEF,NONE_DATA,ATACmd_SetFeatures},//SET FEATURES -EFh, Non-Data

    {0x90,EXECUTE_DEVICE_DIGA,ATACmd_ExeDevDiagnostic},//EXECUTE DEVICE DIAGNOSTIC 90h, Execute Device Diagnostic

    //SECURITY
    {0xF5,NONE_DATA,ATACmd_SecurityFreezeLock},//SECURITY FREEZE LOCK - F5h, Non-Data
#ifndef __BOOT_CODE__
    //DCO
#if (SATA_DCO_ENABLE == 1)
    {0xB1,UNKNOWN_YET,ATACmd_DcoFeatureSet},//Device Configuration Overlay (0xB1)
#endif
#ifndef __JUPITER_DEBUG__
    //SMART
    {0xB0,UNKNOWN_YET,ATACmd_SmartFeatureSet},//SMART (0xB0)
#endif
#endif
    {0x10,NONE_DATA,ATACmd_LegacyCmdSupport},//RECALIBRATE (0x10)
    {0x70,NONE_DATA,ATACmd_LegacyCmdSupport},//SEEK (0x70)

    {0x40,NONE_DATA,ATACmd_LegacyCmdSupport},//READVFY (0x40)   --NOTE:以后需要具体实现,扩展SATA LOG接口
    {0x42,NONE_DATA,ATACmd_LegacyCmdSupport},//RDVFYEXT (0x42)  --NOTE:以后需要具体实现，扩展SATA LOG接口

    /************************mandatory command***********************/
    //省电管理
    {0xE0,NONE_DATA,ATACmd_StandbyImmediate},//STANDBY IMMEDIATE – E0h, Non-Data
    {0xE1,NONE_DATA,ATACmd_IdleImmediate},//IDLE IMMEDIATE – E1h, Non-Data
    {0xE2,NONE_DATA,ATACmd_Standby},//STANDBY – E2h, Non-Data
    {0xE3,NONE_DATA,ATACmd_Idle},//IDLE – E3h, Non-Data
    {0xE5,NONE_DATA,ATACmd_CheckPowerMode},//CHECK POWER MODE – E5h, Non-Data
    {0xE6,NONE_DATA,ATACmd_Sleep},// E6h, Non-Data

    {0xE4,PIO_DATA_IN,ATACmd_ReadBuff},//READ BUFF - E8h, PIO Data-In
    {0xE8,PIO_DATA_OUT,ATACmd_WriteBuff},//WRITE BUFF - E4h, PIO Data-Out


    {0xE9,DMA_DATA_IN,ATACmd_ReadBuffDma},//READ BUFF dma - E9h, dma Data-In
    {0xEB,DMA_DATA_OUT,ATACmd_WriteBuffDma},//WRITE BUFF dma- Ebh, dma Data-Out

    {0x91,NONE_DATA,ATACmd_InitDevParam},// INITIALIZE DEVICE PARAMETERS - 91h, Non-Data - ATA4

    {0x10,NONE_DATA,ATACmd_LegacyCmdSupport},//RECALIBRATE (0x10)
    {0x70,NONE_DATA,ATACmd_LegacyCmdSupport},//SEEK (0x70)

    {0x06,DMA_DATA_OUT,ATACmd_DataSetManagement},//DATA SET MANAGEMENT - 06h, DMA

};
#endif
// *****************************************************************************
// 函数名称: ATACmd_ParseCmd
// 函数功能: 解析收到的命令所属的设备传输类型以及处理接口
// 入口参数: Cmd 命令代码
// 出口参数:
// 说    明:
// *****************************************************************************
T_ATA_CMD* ATACmd_ParseCmd(U8 bCmd)
{
    T_ATA_CMD *ptCmd=NULL;
    U8 i;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    DEBUG_PRINT("Cmd=%x\r\n",bCmd);
    //从命令列表中寻找命令信息
    for(i=0;i<sizeof(tATACmdList_C)/sizeof(T_ATA_CMD);i++)
    {
        if(bCmd == tATACmdList_C[i].bCmd)
        {
            ptCmd=(T_ATA_CMD*)&tATACmdList_C[i];
            break;
        }
    }

//	if(i == sizeof(tATACmdList_C)/sizeof(T_ATA_CMD))
//	{
//		PRINT_HEX("cmd > ",bCmd);//用来串口打印接收到的ATA命令
//	}
    
#if (SATA_LOG_ENABLE == 1)  
    if(ptCmd != NULL)
    tSataDC.enAddrMode=ptCmd->enAddrMode;
#endif
    
    DEBUG_PRINT("i=%u\r\n",i);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return ptCmd;
}


