// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司
//   
//  文件名称：    ata_gpl.c
//  文件标识：
//  内容摘要：    实现ATA8 GPL Feature Set指令集
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

#include "ata_gpl.h"
#include "debug.h"
#include "ata_general.h"
#include "ata_identify.h"
#include "sata_pio_in.h"
#include "sata_pio_out.h"
#include "sata_dma_in.h"
#include "sata_dma_out.h"
#include "sata_misc.h"
#include "dsata.h"
#include "task_manage.h"
#include "uitility.h"
#include "ata_smart.h"
#include "ata_identify.h"
#include "ata_dco.h"
#include "uart.h"

#if (SATA_LOG_ENABLE == 1)
#define DEVICE_STATISTIC_FLAGS_VALID                ((U64)0xC0<<56)
#define DEVICE_STATISTIC_FLAGS_INVALID              ((U64)0x80<<56)
#define DEVICE_STATISTIC_FLAGS_NOT_SUPPORT          ((U64)0x00<<56)
#define RESERVED_VAL                                (0U)

#define SMART_VAR_OFF    (100)


T_LOG_CTX tSataLogCtx __section(".satalogctx");
//所有LOG的表格成员数据分散在各个相应的模块 数据结构中
////对应的LOG的数组偏移 就是虚拟LBA 的偏移地址起始地址
const T_LOG_MAP tLogMaps[]={
    {
         LOG_DIRECTORY_LOG_ADDR,
         LOG_DIRECTORY_SIZE,
         LOG_DIRECTORY_OFFSET,
         RO,
         GPL_SL,
         RESERVED_VAL
    },//Summary DIRECTORY


    /*
     * Table A.64 defines the log page that makes up the SMART summary error log. Summary SMART Error log data
        structures shall include, but are not limited to, Uncorrectable errors, ID Not Found errors for which the LBA
        requested was valid, servo errors, and write fault errors. Summary error log data structures shall not include
        errors attributed to the receipt of faulty commands (e.g., command codes not implemented by the device or
        requests with invalid parameters or invalid LBAs). If the device supports the Comprehensive SMART Error log
        (see A.4), then the Summary SMART Error log duplicates the last five error entries in the Comprehensive
        SMART Error log. The Summary SMART Error log supports 28-bit addressing only.
     */
    {
        SUMMARY_SMART_ERROR_LOG_ADDR,
        COMPH_SMART_ERROR_LOG_SIZE,
        COMPH_SMART_ERROR_LOG_OFFSET,
        RO,
        SL,
        RESERVED_VAL
    },//Summary SMART Error Log,DUPLICATE Comprehensive SMART Error Log
    
    {
        COMPH_SMART_ERROR_LOG_ADDR,
        COMPH_SMART_ERROR_LOG_SIZE,
        COMPH_SMART_ERROR_LOG_OFFSET,
        RO,
        SL,
        RESERVED_VAL
    },//Comprehensive SMART Error Log
    
    {
        EXT_COMPH_SMART_ERROR_LOG_ADDR,
        EXT_COMPH_SMART_ERROR_LOG_SIZE,
        EXT_COMPH_SMART_ERROR_LOG_OFFSET,
        RO,
        GPL,
        RESERVED_VAL
    },//Extended Comprehensive SMART Error Log
    
    {
        DEVICE_STATISTIC_LOG_ADDR,
        DEVICE_STATISTIC_LOG_SIZE,
        DEVICE_STATISTIC_LOG_OFFSET,
        RO,
        GPL_SL,
        RESERVED_VAL
    },//Device Statistics
    
    {
        SMART_SELFTEST_LOG_ADDR,
        SMART_SELFTEST_LOG_SIZE,
        SMART_SELFTEST_LOG_OFFSET,
        RO,
        SL,
        RESERVED_VAL
    },//SMART Self-Test Log
    {
        EXT_SMART_SELFTEST_LOG_ADDR,
        EXT_SMART_SELFTEST_LOG_SIZE,
        EXT_SMART_SELFTEST_LOG_OFFSET,
        RO,
        GPL,
        RESERVED_VAL
    },//Extended SMART Self-Test Log
    
    {
        SELE_SELFTEST_LOG_ADDR,
        SELE_SELFTEST_LOG_SIZE,
        SELE_SELFTEST_LOG_OFFSET,
        RW,
        SL,
        RESERVED_VAL
    },//Selective Self-Test Log
    
    {
        NCQ_ERROR_LOG_ADDR,
        NCQ_ERROR_LOG_SIZE,
        NCQ_ERROR_LOG_OFFSET,
        RO,
        GPL,
        RESERVED_VAL
    },//NCQ Command Error
    
    {
        PHY_E_CONT_LOG_ADDR,
        PHY_E_CONT_LOG_SIZE,
        PHY_E_CONT_LOG_OFFSET,
        RO,
        GPL,
        RESERVED_VAL
    },//SATA Phy Event Counters
    
    {
        IDY_DEV_DATA_LOG_ADDR,
        IDY_DEV_DATA_LOG_SIZE,
        IDY_DEV_DATA_LOG_OFFSET,
        RO,
        GPL_SL,
        RESERVED_VAL
    },//IDENTIFY DEVICE data
    
    {
        HOST_VENDOR_LOG_ADDR(0x80),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x80),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x81),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x81),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific
    
    {
        HOST_VENDOR_LOG_ADDR(0x82),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x82),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x83),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x83),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x84),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x84),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x85),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x85),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x86),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x86),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x87),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x87),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x88),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x88),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x89),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x89),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x8A),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x8A),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x8B),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x8B),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x8C),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x8C),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x8D),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x8D),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x8E),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x8E),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
        HOST_VENDOR_LOG_ADDR(0x8F),
        HOST_VENDOR_LOG_SIZE,
        HOST_VENDOR_LOG_OFFSET(0x8F),
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Host Vendor Specific

    {
       HOST_VENDOR_LOG_ADDR(0x90),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x90),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x91),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x91),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x92),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x92),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x93),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x93),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x94),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x94),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x95),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x95),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x96),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x96),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x97),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x97),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x98),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x98),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x99),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x99),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x9A),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x9A),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x9B),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x9B),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x9C),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x9C),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x9D),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x9D),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x9E),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x9E),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

   {
       HOST_VENDOR_LOG_ADDR(0x9F),
       HOST_VENDOR_LOG_SIZE,
       HOST_VENDOR_LOG_OFFSET(0x9F),
       RW,
       GPL_SL,
       RESERVED_VAL
   },//Host Vendor Specific

    {
        DEVICE_VENDOR_LOG_ADDR,
        DEVICE_VENDOR_LOG_SIZE,
        DEVICE_VENDOR_LOG_OFFSET,
        RW,
        GPL_SL,
        RESERVED_VAL
    },//Device Vendor Specific
    
//    {
//        SCT_CMD_LOG_ADDR,
//        SCT_CMD_LOG_SIZE,
//        SCT_CMD_LOG_OFFSET,
//        RW,
//        GPL_SL,
//        RESERVED_VAL
//    },//SCT Command/Status
//
//    {
//        SCT_DATA_LOG_ADDR,
//        SCT_DATA_LOG_SIZE,
//        SCT_DATA_LOG_OFFSET,
//        RW,
//        GPL_SL,
//        RESERVED_VAL
//    },//SCT Data Transfer
};
// *****************************************************************************
// 函数名称: Build_LODSTable (Build_ListOfDeviceStatisticTable)
// 函数功能: 初始化GeneralStatistics LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
//
// void Build_LODSTable(U8 *pTable)
//{
//    //固定的内容
//    *(U64*)pTable=0x0000000000010001;//Device Statistics Information Header
//    pTable[8]=8;//Number of entries (8) in the following list
//    pTable[8+1]=1;//Log page number of first supported Device Statistics log page (00h)
//    pTable[8+2]=1;//Log page number of first supported General Statistics log page (01h)
//    pTable[8+3]=1;//Log page number of first supported Free Fall Statistics log page (02h)
//    pTable[8+4]=1;//Log page number of first supported Rotating Media Statistics log page (03h)
//    pTable[8+5]=1;//Log page number of first supported General Errors Statistics log page (04h)
//    pTable[8+6]=1;//Log page number of first supported Temperature Statistics log page (05h)
//    pTable[8+7]=1;//Log page number of first supported Transport Statistics  log page (06h)
//    pTable[8+8]=1;//Log page number of first supported Solid State Device Statistics  log page (07h)
//    //TODO:清零？
//}


// *****************************************************************************
// 函数名称: Restore_VarData
// 函数功能: 初始化DEVICE VAR
// 入口参数:
//          pBuff 从NAND FLASH读出来的VAR数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 BOOL Restore_NoneVolatileVar(U8 *pTable)
 {
    T_DEVICE_VAR *pDV=(T_DEVICE_VAR *)pTable;

    if(Check32Sum((U32*)pTable,128) != 0)
    {
        //计算dword checksum失败
        PRINT_TXT("E:BAD,CKSUM");
        return FALSE;
    }

    if(pDV->dHeadVersion != 0x12348765)
    {
        //刚量产的全FF数据
        Mem32SetVal((U32*)pTable,0U,128U);//清除数据,一个调用20byte
    }

    tSataDC.dMilliSecAcc=0;
    tSataDC.dMilliSecAcc +=pDV->dMilliSecAcc;//获取上次上电不满一个小时的毫秒数的余量
    tSataDC.eSecState =pDV->eSecState;//获取上次掉电前的security 状态
    /*
     * The MASTER PASSWORD CAPABILITY bit (see A.11.8.3.2) shall remain unchanged
     * */
    tSataLogCtx.bMPCFlg=pDV->bMPCFlg;//获取MPC 标记
    PRINT_HEX("SEC=",tSataDC.eSecState);
#if(SATA_SEC_ENABLE == TRUE)
    if(tSataDC.eSecState >= ATA_SEC3 && tSataDC.eSecState <= ATA_SEC6)
    {
        //SEC3/4/5/6回到 SEC4
        tSataDC.eSecState=ATA_SEC4;
    }
    else
    {
         tSataDC.eSecState=ATA_SEC1;
    }
#else
    tSataDC.eSecState=ATA_SEC1;
    tSataDC.bIsSecFreezeLocked=FALSE;
#endif

#if(SATA_DCO_ENABLE == TRUE)
    tSataDC.eDCOState = pDV->eDCOState;//获取上次掉电前的DCO 状态
#endif

#if(SATA_SD_ENABLE == TRUE)
    tSataDC.tSDState=pDV->tSDState;//获取上次掉电前的SANITIZE DEVICE 状态
    tSataDC.bSDFMPVal = pDV->bSDFMPVal;//获取上次掉电前的SANITIZE DEVICE  状态
    tSataDC.bSDOCWOEVal = pDV->bSDOCWOEVal;//获取上次掉电前的SANITIZE DEVICE  状态
    tSataDC.wSanitizeProCnt = pDV->wSanitizeProCnt;//获取上次掉电前的SANITIZE DEVICE  状态
    tSataLogCtx.wErrLogIdx =pDV->wErrLogIdx;
    //tSataLogCtx.dSataReconectMode = pDV->dSataReconectMode;
    if(tSataDC.tSDState == S_SD4 || tSataDC.tSDState == S_SD1)
    {
        tSataDC.tSDState=S_SD0;
        tSataDC.bSDFMPVal=0;
    }

    if(tSataDC.wSanitizeProCnt  == 0)
    {
        tSataDC.wSanitizeProCnt =0xFFFF;
    }
#else
    tSataDC.tSDState=S_SD0;
    tSataDC.bSDFMPVal=0;
#endif
    //初始化SMART 变量信息
    T_SMART_CTX *pSV=(T_SMART_CTX *)(pTable+SMART_VAR_OFF);//smart variable
    pSV->wOrigBadBlkCnt=tSataLogCtx.tSmartCtx.wOrigBadBlkCnt;//上电初始化已经完成

    if(tSataLogCtx.tSmartCtx.dReadSecCnt != 0xFFFFFFFF)
    {
        pSV->dReadSecCnt=tSataLogCtx.tSmartCtx.dReadSecCnt;//上电初始化已经完成
        pSV->dWriteSecCnt=tSataLogCtx.tSmartCtx.dWriteSecCnt;//上电初始化已经完成
    }
    MemoryCopy((U8*)&tSataLogCtx.tSmartCtx,(U8*)pSV,sizeof(T_SMART_CTX));

    //PRINT_HEX("RD=",tSataLogCtx.tSmartCtx.wReadSecCnt);
    //PRINT_HEX("WR=",tSataLogCtx.tSmartCtx.wWriteSecCnt);

    return TRUE;
 }

 // *****************************************************************************
 // 函数名称: Build_VarData
 // 函数功能: 初始化DEVICE VAR
 // 入口参数:
 //          pBuff 从NAND FLASH读出来的VAR数据缓冲区
 // 出口参数: NONE
 // 说    明:
 // *****************************************************************************
void Build_NoneVolatileVarTable(U8 *pTable)
{
    T_DEVICE_VAR *pDV=(T_DEVICE_VAR *)pTable;
    pDV->dHeadVersion=0x12348765;
    pDV->dMilliSecAcc=tSataDC.dMilliSecAcc;//保存本次上电不满一个小时的毫秒数的余量，用于更加准确地累加小时计数
    pDV->eSecState=tSataDC.eSecState;//保存上次掉电前的security 状态
    pDV->bMPCFlg=SEC_MPC;
#if(SATA_DCO_ENABLE == TRUE)
    pDV->eDCOState=tSataDC.eDCOState;
#endif
    pDV->tSDState=tSataDC.tSDState;//保存上次掉电前的SANITIZE DEVICE 状态
    pDV->bSDFMPVal=tSataDC.bSDFMPVal;//保存上次掉电前的SANITIZE DEVICE 状态
    pDV->bSDOCWOEVal=tSataDC.bSDOCWOEVal;//保存上次掉电前的SANITIZE DEVICE 状态
    pDV->wSanitizeProCnt=tSataDC.wSanitizeProCnt;//保存上次掉电前的SANITIZE DEVICE 状态
    //pDV->wReadSecCnt=tSataLogCtx.tSmartCtx.wReadSecCnt;
    //pDV->wWriteSecCnt=tSataLogCtx.tSmartCtx.wWriteSecCnt;
    pDV->wErrLogIdx=tSataLogCtx.wErrLogIdx;
    //pDV->dSataReconectMode=tSataLogCtx.dSataReconectMode;
    T_SMART_CTX *pSV=(T_SMART_CTX *)(pTable+SMART_VAR_OFF);//smart variable
    MemoryCopy((U8*)pSV,(U8*)&tSataLogCtx.tSmartCtx,sizeof(T_SMART_CTX));

    ((U32*)pTable)[127]=Check32Sum((U32*)pTable,127);//计算checksum
}

// *****************************************************************************
// 函数名称: Restore_GSInfo (Restore_GeneralStatisticsInfo
// 函数功能: 初始化GeneralStatistics LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Restore_GSInfo(U8 *pTable)
{
    UART_PRINT("Enter func=<%s>[file=%s,line=%d] \r\n",__func__,__MODULE__,__LINE__);

    UART_PRINT("Restore_GSInfo,pTable=%#010lx\r\n",pTable);
    UART_PRINT("pTable[0]=%#010lx\r\n",((U32*)pTable)[0]);
    UART_PRINT("pTable[1]=%#010lx\r\n",((U32*)pTable)[1]);
    UART_PRINT("pTable[2]=%#010lx\r\n",((U32*)pTable)[2]);
    UART_PRINT("pTable[3]=%#010lx\r\n",((U32*)pTable)[3]);

    UART_PRINT("pTable[4]=%#010lx\r\n",((U32*)pTable)[4]);
    UART_PRINT("pTable[5]=%#010lx\r\n",((U32*)pTable)[5]);
    UART_PRINT("pTable[6]=%#010lx\r\n",((U32*)pTable)[6]);
    UART_PRINT("pTable[7]=%#010lx\r\n",((U32*)pTable)[7]);
    
    UART_PRINT("pTable[8]=%#010lx\r\n",((U32*)pTable)[8]);
    UART_PRINT("pTable[9]=%#010lx\r\n",((U32*)pTable)[9]);
    UART_PRINT("pTable[10]=%#010lx\r\n",((U32*)pTable)[10]);
    UART_PRINT("pTable[11]=%#010lx\r\n",((U32*)pTable)[11]);

    UART_PRINT("pTable[12]=%#010lx\r\n",((U32*)pTable)[12]);
    UART_PRINT("pTable[13]=%#010lx\r\n",((U32*)pTable)[13]);
    UART_PRINT("pTable[14]=%#010lx\r\n",((U32*)pTable)[14]);
    UART_PRINT("pTable[15]=%#010lx\r\n",((U32*)pTable)[15]);

    tSataDC.dPowerCycleCnt = *((U64*)pTable+1) & 0xFFFFFFFF;
    UART_PRINT("tSataDC.dPowerCycleCnt=%#010lx\r\n",tSataDC.dPowerCycleCnt);
    tSataDC.dPowerCycleCnt++;//立即更新上电次数
    UART_PRINT("tSataDC.dPowerCycleCnt=%#010lx\r\n",tSataDC.dPowerCycleCnt);
    
    tSataDC.dLifePowerOnTime = *((U64*)pTable+2) & 0xFFFFFFFF;
    UART_PRINT("tSataDC.dLifePowerOnTime=%#010lx\r\n",tSataDC.dLifePowerOnTime);

    //tSataLogCtx.tSmartCtx.wReadSecCntFromNand = *((U64*)pTable+3) & 0xFFFF;
    tSataLogCtx.qwNumOfWriteCmd = *((U64*)pTable+4) & 0xFFFFFFFFFFFF;
    //tSataLogCtx.tSmartCtx.wWriteSecCnt2Nand = *((U64*)pTable+5) & 0xFFFF;
    tSataLogCtx.qwNumOfReadCmd = *((U64*)pTable+6) & 0xFFFFFFFFFFFF;

#if 0
    //USER DEFINED
    tSataDC.dMilliSecAcc=0;
    tSataDC.dMilliSecAcc += *((U64*)pTable+7) & 0xFFFFFFFF;//获取上次上电不满一个小时的毫秒数的余量
    tSataDC.eSecState = *((U64*)pTable+8) & 0xFF;//获取上次掉电前的security 状态
    /*
     * The MASTER PASSWORD CAPABILITY bit (see A.11.8.3.2) shall remain unchanged
     * */
    tSataLogCtx.bMPCFlg=(*((U64*)pTable+8) & 0xFFFF0000)>>16;//获取MPC 标记

#if(SATA_SEC_ENABLE == TRUE)
    if(tSataDC.eSecState == ATA_SEC0 || tSataDC.eSecState == ATA_SEC1 || tSataDC.eSecState == ATA_SEC2)
    {
        tSataDC.eSecState=ATA_SEC1;
        tSataDC.bIsSecFreezeLocked=FALSE;
    }
    else
    {
        tSataDC.eSecState =ATA_SEC4;
    }
#else
    tSataDC.eSecState=ATA_SEC1;
    tSataDC.bIsSecFreezeLocked=FALSE;
#endif

#if(SATA_DCO_ENABLE == TRUE)
    tSataDC.eDCOState = (*((U64*)pTable+9) & 0xFF00)>>8;//获取上次掉电前的DCO 状态
#endif

#if(SATA_SD_ENABLE == TRUE)
    tSataDC.tSDState = *((U64*)pTable+9) & 0xFF;//获取上次掉电前的SANITIZE DEVICE 状态
    tSataDC.bSDFMPVal = *((U64*)pTable+10) & 0xFF;//获取上次掉电前的SANITIZE DEVICE  状态
    tSataDC.bSDOCWOEVal = *((U64*)pTable+11) & 0xFF;//获取上次掉电前的SANITIZE DEVICE  状态
    tSataDC.wSanitizeProCnt = *((U64*)pTable+12) & 0xFFFF;//获取上次掉电前的SANITIZE DEVICE  状态
    tSataLogCtx.tSmartCtx.wReadSecCnt = (*((U64*)pTable+12) & 0xFFFF0000)>>16;
    tSataLogCtx.tSmartCtx.wWriteSecCnt = (*((U64*)pTable+12) & 0xFFFF00000000)>>32;

    if(tSataDC.tSDState == S_SD4 || tSataDC.tSDState == S_SD1)
    {
        tSataDC.tSDState=S_SD0;
        tSataDC.bSDFMPVal=0;
    }

    if(tSataDC.wSanitizeProCnt  == 0)
    {
        tSataDC.wSanitizeProCnt =0xFFFF;
    }
#else
    tSataDC.tSDState=S_SD0;
    tSataDC.bSDFMPVal=0;
#endif

    PRINT_HEX("SEC=",tSataDC.eSecState);
#endif
}

// *****************************************************************************
// 函数名称: Build_GSTable(Build_GeneralStatisticsTable)
// 函数功能: 根据GeneralStatistics LOG表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 BOOL Build_GSTable(U8 *pTable)
{
     static U32 check_sum=0;
     U32 check_sum_tmp;
     UART_PRINT("Enter func=<%s>[file=%s,line=%d] \r\n",__func__,__MODULE__,__LINE__);
     UART_PRINT("pTable=%#010lx\r\n",pTable);
    *(U64*)pTable=0x0000000000010001;//Device Statistics Information Header
     UART_PRINT("tSataDC.dPowerCycleCnt=%#010lx\r\n",tSataDC.dPowerCycleCnt);
    *((U64*)pTable+1)=tSataDC.dPowerCycleCnt | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+2)=tSataDC.dLifePowerOnTime | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+3)=((U64)tSataLogCtx.tSmartCtx.dWriteSecCnt*64*1024) | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+4)=tSataLogCtx.qwNumOfWriteCmd | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+5)=((U64)tSataLogCtx.tSmartCtx.dReadSecCnt*64*1024) | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+6)=tSataLogCtx.qwNumOfReadCmd | DEVICE_STATISTIC_FLAGS_VALID;

    check_sum_tmp=Check32Sum((U32*)pTable,128);
    if(check_sum != check_sum_tmp)
    {
        check_sum=check_sum_tmp;
        return TRUE;
    }
    else
    {
        return FALSE;
    }

#if 0
    *((U64*)pTable+7)=(tSataDC.dMilliSecAcc)| DEVICE_STATISTIC_FLAGS_INVALID;//保存本次上电不满一个小时的毫秒数的余量，用于更加准确地累加小时计数
    *((U64*)pTable+8)=(tSataDC.eSecState|((SEC_MPC)<<16))| DEVICE_STATISTIC_FLAGS_INVALID;//保存上次掉电前的security 状态
#if(SATA_DCO_ENABLE == TRUE)
    *((U64*)pTable+9)=(tSataDC.tSDState)|(tSataDC.eDCOState<<8)| DEVICE_STATISTIC_FLAGS_INVALID;//保存上次掉电前的SANITIZE DEVICE 状态
#endif
    *((U64*)pTable+10)=(tSataDC.bSDFMPVal)| DEVICE_STATISTIC_FLAGS_INVALID;//保存上次掉电前的SANITIZE DEVICE 状态
    *((U64*)pTable+11)=(tSataDC.bSDOCWOEVal)| DEVICE_STATISTIC_FLAGS_INVALID;//保存上次掉电前的SANITIZE DEVICE 状态
    *((U64*)pTable+12)=(tSataDC.wSanitizeProCnt)|((U64)tSataLogCtx.tSmartCtx.wReadSecCnt<<16)|((U64)tSataLogCtx.tSmartCtx.wWriteSecCnt<<32)|DEVICE_STATISTIC_FLAGS_INVALID;//保存上次掉电前的SANITIZE DEVICE 状态

    UART_PRINT("tSataDC.dMilliSecAcc=%#010lx\r\n",tSataDC.dMilliSecAcc);

    UART_PRINT("pTable[0]=%#010lx\r\n",((U32*)pTable)[0]);
    UART_PRINT("pTable[1]=%#010lx\r\n",((U32*)pTable)[1]);
    UART_PRINT("pTable[2]=%#010lx\r\n",((U32*)pTable)[2]);
    UART_PRINT("pTable[3]=%#010lx\r\n",((U32*)pTable)[3]);

    UART_PRINT("pTable[4]=%#010lx\r\n",((U32*)pTable)[4]);
    UART_PRINT("pTable[5]=%#010lx\r\n",((U32*)pTable)[5]);
    UART_PRINT("pTable[6]=%#010lx\r\n",((U32*)pTable)[6]);
    UART_PRINT("pTable[7]=%#010lx\r\n",((U32*)pTable)[7]);

    UART_PRINT("pTable[8]=%#010lx\r\n",((U32*)pTable)[8]);
    UART_PRINT("pTable[9]=%#010lx\r\n",((U32*)pTable)[9]);
    UART_PRINT("pTable[10]=%#010lx\r\n",((U32*)pTable)[10]);
    UART_PRINT("pTable[11]=%#010lx\r\n",((U32*)pTable)[11]);

    UART_PRINT("pTable[12]=%#010lx\r\n",((U32*)pTable)[12]);
    UART_PRINT("pTable[13]=%#010lx\r\n",((U32*)pTable)[13]);
    UART_PRINT("pTable[14]=%#010lx\r\n",((U32*)pTable)[14]);
    UART_PRINT("pTable[15]=%#010lx\r\n",((U32*)pTable)[15]);
#endif
}
 // *****************************************************************************
 // 函数名称:Restore_UserInfo
 // 函数功能: 初始化device 相关的需要保存的相关变量
 // 入口参数:
 //          pBuff 从NAND FLASH读出来的LOG数据缓冲区
 // 出口参数: NONE
 // 说    明:
 // *****************************************************************************
  void Restore_UserInfo(U8 *pTable)
 {

 }

// *****************************************************************************
// 函数名称:Restore_GESInfo( Restore_GeneralErrorStatisticsInfo)
// 函数功能: 初始化GeneralStatistics error LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Restore_GESInfo(U8 *pTable)
{
    UART_PRINT("Restore_GESInfo,pTable=%#010lx\r\n",pTable);
    tSataLogCtx.dUnUncorrectableErrorCnt = *((U64*)pTable+1) & 0xFFFFFFFF;
    tSataLogCtx.dResetsCntOfCmd = *((U64*)pTable+2) & 0xFFFFFFFF;
}

// *****************************************************************************
// 函数名称: Build_GESTable(Restore_GeneralErrorStatisticsTable)
// 函数功能: 根据GeneralStatistics error LOG表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
BOOL Build_GESTable(U8 *pTable)
{
     static U32 check_sum=0;
     U32 check_sum_tmp;
    //固定的内容
    *(U64*)pTable=0x0000000000040001;//Device Statistics Information Header
    *((U64*)pTable+1)=tSataLogCtx.dUnUncorrectableErrorCnt | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+2)=tSataLogCtx.dResetsCntOfCmd | DEVICE_STATISTIC_FLAGS_VALID;

    check_sum_tmp=Check32Sum((U32*)pTable,128);
    if(check_sum != check_sum_tmp)
    {
        check_sum=check_sum_tmp;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// *****************************************************************************
// 函数名称:Restore_SSDSInfo( Restore_SolidStateDeviceStatisticsInfo)
// 函数功能: 初始化GeneralStatistics ssd LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Restore_SSDSInfo(U8 *pTable)
{
    //已写入的数据量占可写入的数据总量的百分比
    tSataLogCtx.bPercentUsedCap = *((U64*)pTable+1) & 0xFF;
}

// *****************************************************************************
// 函数名称: Build_SSDSTable(Build_SolidStateDeviceStatisticsTable)
// 函数功能: 根据GeneralStatistics ssd LOG表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 BOOL Build_SSDSTable(U8 *pTable)
{
     static U32 check_sum=0;
     U32 check_sum_tmp;

    //固定的内容
    *(U64*)pTable=0x0000000000070001;//Device Statistics Information Header
    //已写入的数据量占可写入的数据总量的百分比
    *((U64*)pTable+1)=tSataLogCtx.bPercentUsedCap | DEVICE_STATISTIC_FLAGS_VALID;

    check_sum_tmp=Check32Sum((U32*)pTable,128);
    if(check_sum != check_sum_tmp)
    {
        check_sum=check_sum_tmp;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// *****************************************************************************
// 函数名称:Restore_TSInfo( Restore_Temperature StatisticsInfo)
// 函数功能: 初始化Temperature Statistics LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Restore_TSInfo(U8 *pTable)
{
    
    tSataLogCtx.bCurrentTemp = *((U64*)pTable+1) & 0xFF;//当前值需要

    //24小时产生144个数据，计算平均值
    tSataLogCtx.bAverangeShortTemp = *((U64*)pTable+2) & 0xFF;
    tSataLogCtx.bHighestTemp = *((U64*)pTable+4) & 0xff;
    tSataLogCtx.bLowestTemp = *((U64*)pTable+5) & 0xff;
    tSataLogCtx.bHighestAverangeShortTemp = *((U64*)pTable+6) & 0xff;
    tSataLogCtx.bLowestAverangeShortTemp = *((U64*)pTable+7) & 0xff;


    tSataLogCtx.bAverangeLongTemp = *((U64*)pTable+3) & 0xFF;
    tSataLogCtx.bHighestAverangeLongTemp = *((U64*)pTable+8) & 0xff;
    tSataLogCtx.bLowestAverangeLongTemp = *((U64*)pTable+9) & 0xff;
    //每收集42天bAverangeShortTemp的值计算一次平均值
    
    tSataLogCtx.dOvTempNums = *((U64*)pTable+10) & 0xFFFFFFFF;
    tSataLogCtx.dUnDerTempNums = *((U64*)pTable+11) & 0xFFFFFFFF;
    tSataLogCtx.bMaxOpTemp = *((U64*)pTable+12) & 0xff;
    tSataLogCtx.bMinOpTemp = *((U64*)pTable+13) & 0xff;
    
    //扩展变量保存区
    tSataLogCtx.dLongTempAcc = *((U64*)pTable+14) & 0xFFFFFFFF;
    tSataLogCtx.dShortTempAcc = *((U64*)pTable+15) & 0xFFFFFFFF;
    
    if(*(U64*)pTable != 0)
    tSataLogCtx.bSmartEn = *((U64*)pTable+16) & 0xFF;
    else
    tSataLogCtx.bSmartEn=TRUE;

   // tSataLogCtx.bCSELIdx = *((U64*)pTable+16) & 0xFF;
   // tSataLogCtx.bExtCompSmartErrorLogIdx = *((U64*)pTable+17) & 0xFF;

}

// *****************************************************************************
// 函数名称: Build_TSTable(Build_Temperature StatisticsTable)
// 函数功能: 根据Temperature Statistics LOG表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 BOOL Build_TSTable(U8 *pTable)
{
     static U32 check_sum=0;
     U32 check_sum_tmp;
    //固定的内容
    *(U64*)pTable=0x0000000000050001;//Device Statistics Information Header
    *((U64*)pTable+1)=tSataLogCtx.bCurrentTemp | DEVICE_STATISTIC_FLAGS_VALID;//TODO:调用接口获取当前温度
    if(tSataDC.dLifePowerOnTime < 24)
    {
        //量产24小时以内该数据时无效的
        *((U64*)pTable+2)=tSataLogCtx.bAverangeShortTemp | DEVICE_STATISTIC_FLAGS_INVALID;
        *((U64*)pTable+4)=tSataLogCtx.bHighestTemp | DEVICE_STATISTIC_FLAGS_INVALID;
        *((U64*)pTable+5)=tSataLogCtx.bLowestTemp | DEVICE_STATISTIC_FLAGS_INVALID;
        *((U64*)pTable+6)=tSataLogCtx.bHighestAverangeShortTemp | DEVICE_STATISTIC_FLAGS_INVALID;
        *((U64*)pTable+7)=tSataLogCtx.bLowestAverangeShortTemp | DEVICE_STATISTIC_FLAGS_INVALID;
    }
    else
    {
        //24小时产生144个数据，计算平均值
        *((U64*)pTable+2)=tSataLogCtx.bAverangeShortTemp | DEVICE_STATISTIC_FLAGS_VALID;
        *((U64*)pTable+4)=tSataLogCtx.bHighestTemp | DEVICE_STATISTIC_FLAGS_VALID;
        *((U64*)pTable+5)=tSataLogCtx.bLowestTemp | DEVICE_STATISTIC_FLAGS_VALID;
        *((U64*)pTable+6)=tSataLogCtx.bHighestAverangeShortTemp | DEVICE_STATISTIC_FLAGS_VALID;
        *((U64*)pTable+7)=tSataLogCtx.bLowestAverangeShortTemp | DEVICE_STATISTIC_FLAGS_VALID;
    }
    if(tSataDC.dLifePowerOnTime < (24*42))
    {
        //42天以后，该值才有意义
        *((U64*)pTable+3)=tSataLogCtx.bAverangeLongTemp | DEVICE_STATISTIC_FLAGS_INVALID;
        *((U64*)pTable+8)=tSataLogCtx.bHighestAverangeLongTemp | DEVICE_STATISTIC_FLAGS_INVALID;
        *((U64*)pTable+9)=tSataLogCtx.bLowestAverangeLongTemp | DEVICE_STATISTIC_FLAGS_INVALID;
    }
    else
    {
        //每收集42天bAverangeShortTemp的值计算一次平均值
        *((U64*)pTable+3)=tSataLogCtx.bAverangeLongTemp | DEVICE_STATISTIC_FLAGS_VALID;
        *((U64*)pTable+8)=tSataLogCtx.bHighestAverangeLongTemp | DEVICE_STATISTIC_FLAGS_VALID;
        *((U64*)pTable+9)=tSataLogCtx.bLowestAverangeLongTemp | DEVICE_STATISTIC_FLAGS_VALID;
        
    }
    
    *((U64*)pTable+10)=tSataLogCtx.dOvTempNums | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+11)=tSataLogCtx.dUnDerTempNums | DEVICE_STATISTIC_FLAGS_VALID;
    
    *((U64*)pTable+12)=tSataLogCtx.bMaxOpTemp | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+13)=tSataLogCtx.bMinOpTemp | DEVICE_STATISTIC_FLAGS_VALID;
    
    //扩展变量保存区
    *((U64*)pTable+14)=tSataLogCtx.dLongTempAcc | DEVICE_STATISTIC_FLAGS_INVALID;
    *((U64*)pTable+15)=tSataLogCtx.dShortTempAcc | DEVICE_STATISTIC_FLAGS_INVALID;
    /*
     The state of SMART (i.e., enabled or disabled) shall be preserved by the device during all power-on reset events
    */
    *((U64*)pTable+16)=tSataLogCtx.bSmartEn | DEVICE_STATISTIC_FLAGS_INVALID;
    
    
    //*((U64*)pTable+16)=tSataLogCtx.bCSELIdx | DEVICE_STATISTIC_FLAGS_INVALID;
    //*((U64*)pTable+17)=tSataLogCtx.bExtCompSmartErrorLogIdx | DEVICE_STATISTIC_FLAGS_INVALID;

    check_sum_tmp=Check32Sum((U32*)pTable,128);
    if(check_sum != check_sum_tmp)
    {
        check_sum=check_sum_tmp;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// *****************************************************************************
// 函数名称:Restore_TrpSInfo( Restore_ Transport StatisticsInfo)
// 函数功能: 初始化 Transport Statistics LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Restore_TrpSInfo(U8 *pTable)
{
    //已写入的数据量占可写入的数据总量的百分比
    tSataLogCtx.dHWResetNum = *((U64*)pTable+1) & 0xFFFFFFFF;
    tSataLogCtx.dASREventCnt = *((U64*)pTable+2) & 0xFFFFFFFF;
    tSataLogCtx.dCRCErrorCnt = *((U64*)pTable+3) & 0xFFFFFFFF;
}

// *****************************************************************************
// 函数名称: Build_TrpSTable(Build_Transport StatisticsTable)
// 函数功能: 根据 Transport Statistics LOG表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
BOOL Build_TrpSTable(U8 *pTable)
{
    static U32 check_sum=0;
    U32 check_sum_tmp;

    //固定的内容
    *(U64*)pTable=0x0000000000060001;//Device Statistics Information Header
    //已写入的数据量占可写入的数据总量的百分比
    *((U64*)pTable+1)=tSataLogCtx.dHWResetNum | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+2)=tSataLogCtx.dASREventCnt | DEVICE_STATISTIC_FLAGS_VALID;
    *((U64*)pTable+3)=tSataLogCtx.dCRCErrorCnt | DEVICE_STATISTIC_FLAGS_VALID;

    check_sum_tmp=Check32Sum((U32*)pTable,128);
    if(check_sum != check_sum_tmp)
    {
        check_sum=check_sum_tmp;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// *****************************************************************************
// 函数名称:Restore_PhyEventCounterInfo( Restore_ PhyEventCounter Info)
// 函数功能: 初始化  PhyEventCounter LOG表格数据的相关变量
// 入口参数:
//          pBuff 从NAND FLASH读出来的LOG数据缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void Restore_PhyEventCounterInfo(T_LOG_PHYECNT *pTable)
{
    //已写入的数据量占可写入的数据总量的百分比
    tSataLogCtx.dCmdFailDue2ICRC = pTable->tCTX[0].dCounter;
    tSataLogCtx.dPHYRDY2N = pTable->tCTX[1].dCounter;
    tSataLogCtx.dD2hFisDue2Comreset = pTable->tCTX[2].dCounter;
    tSataLogCtx.dCRCErrorCnt = pTable->tCTX[3].dCounter;
    tSataLogCtx.dNoneCRCErrorCnt = pTable->tCTX[4].dCounter;
}

// *****************************************************************************
// 函数名称: Build_PhyEventCounterTable(Build_Transport StatisticsTable)
// 函数功能: 根据 PhyEvent counter LOG表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
BOOL Build_PhyEventTable(T_LOG_PHYECNT *pTable)
{
    static U32 check_sum=0;
    U32 check_sum_tmp;
    //固定的内容
    pTable->tCTX[0].tID.bits.IDFY=0x001;
    pTable->tCTX[0].tID.bits.SIZE=2;
    pTable->tCTX[0].dCounter=tSataLogCtx.dCmdFailDue2ICRC;

    pTable->tCTX[1].tID.bits.IDFY=0x009;
    pTable->tCTX[1].tID.bits.SIZE=2;
    pTable->tCTX[1].dCounter=tSataLogCtx.dPHYRDY2N;

    pTable->tCTX[2].tID.bits.IDFY=0x00A;
    pTable->tCTX[2].tID.bits.SIZE=2;
    pTable->tCTX[2].dCounter=tSataLogCtx.dD2hFisDue2Comreset;

    pTable->tCTX[3].tID.bits.IDFY=0x00B;
    pTable->tCTX[3].tID.bits.SIZE=2;
    pTable->tCTX[3].dCounter=tSataLogCtx.dCRCErrorCnt;

    pTable->tCTX[4].tID.bits.IDFY=0x00D;
    pTable->tCTX[4].tID.bits.SIZE=2;
    pTable->tCTX[4].dCounter=tSataLogCtx.dNoneCRCErrorCnt;

    pTable->bCheckSum = (~GetSum((U8*)pTable,511)) + 1;

    check_sum_tmp=Check32Sum((U32*)pTable,128);
    if(check_sum != check_sum_tmp)
    {
        check_sum=check_sum_tmp;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// *****************************************************************************
// 函数名称:Array2Number
// 函数功能: 数组变量转换
// 入口参数:
//          Array： 输入数组
//          bCnt：有效字节数 取值：1-6
// 出口参数: NONE
// 说    明:
// *****************************************************************************
U64 Array2Number(U8* Array,U8 bCnt)
{
    U64 result=0;
    while(bCnt--)
    {
        result+=Array[bCnt-1]<<((bCnt-1)*8);
    }
    return result;
}
#ifdef __SMART_1__
// *****************************************************************************
// 函数名称: Build_SmartTable
// 函数功能: 根据 samrt 表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Build_SmartTable(UN_SMART_DATA_STRUCTURE* pUnSmart)
{

    //根据相关变量，重建SAMRT表格数据，准备写入FLASH中
    //满额以后不需要再记录
    *(U8*)pUnSmart->atSmtAttrData[RD_ERROR_RATE].abRawData=tSataLogCtx.tSmartCtx.bReadErrorRate;
    *(U8*)pUnSmart->atSmtAttrData[TEMPERATURE].abRawData=tSataLogCtx.bCurrentTemp;
    *(U8*)pUnSmart->atSmtAttrData[THROU_PERFORMANCE].abRawData=tSataLogCtx.tSmartCtx.bThroughPutPerformance;
    
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[RD_RETRY_NUM].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReadRetryNum,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[POWER_ON_HOURS].abRawData,(U8*)&tSataDC.dLifePowerOnTime,4);//设备运行累积时长
    //UART_PRINT("tSataDC.dPowerCycleCnt=%#010lx\r\n",tSataDC.dPowerCycleCnt);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[POWER_CYCLE_CNT].abRawData,(U8*)&tSataDC.dPowerCycleCnt,4);//设备上电次数
    
    //TODO：如果超过48bit，保留全FF？
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[RD_SCT_CNT2NAND].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReadSecCntFromNand,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[WR_SCT_CNT2NAND].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dWriteSecCnt2Nand,4);

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[RD_SCT_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReadSecCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[WR_SCT_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dWriteSecCnt,4);
    
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[BAD_BLK_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wBadBlkCnt,2);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[BAD_CLUSTER_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wBadClusterTblCnt,2);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[MAX_BAD_BLK_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wMaxBadBlkCnt,2);

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[NUM_OF_VALID_BLOCKS].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wValidBlockNums,2);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[UNCONRRECTABLE_SEC_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dUnCorrectSecCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[TOTAL_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dTotalEraseCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[MAXIMUM_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dMaxEraseCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[MINIMUM_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dMinEraseCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[AVERG_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dAvrgEraseCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALL_E_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReAllcatedEventCnt,4);

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALLOC_SCT_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wReAllocatedSecCnt,2);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[PHY_ERROR_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wSataPHYErrCnt,2);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[SATA_DOWNSHIFT_ERR_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wSataDownShiftErrCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT].abRawData,(U8*)&tSataLogCtx.dCRCErrorCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[UNEXP_POWER_LOSS_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dUnExpPowerLossCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[IN_DIE_CPYBK_SLC].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dSLCIndieCopybackCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[IN_DIE_CPYBK_TLC].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dTLCIndieCopybackCnt,4);

    pUnSmart->bErrLogCap=1;//Device error logging supported
    //TODO:self test status updated
    pUnSmart->CheckSum=(~GetSum((U8*)pUnSmart,511)) + 1;
}
#endif

#ifdef __SMART_2__
// *****************************************************************************
// 函数名称: Build_SmartTable
// 函数功能: 根据 samrt 表格相关变量重建表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Build_SmartTable(UN_SMART_DATA_STRUCTURE* pUnSmart)
{
    //根据相关变量，重建SAMRT表格数据，准备写入FLASH中
    //满额以后不需要再记录
    *(U8*)pUnSmart->atSmtAttrData[RD_ERROR_RATE].abRawData=tSataLogCtx.tSmartCtx.bReadErrorRate;

    if(tSataDC.tSataMI.tMpCfg.bits.T_FIX)
    {
        *(U8*)pUnSmart->atSmtAttrData[TEMPERATURE].abRawData=tSataDC.tSataMI.tMpCfg.bits.T_VAL;
    }
    else
    {
        if(tSataLogCtx.bCurrentTemp >= tSataDC.tSataMI.tMpCfg.bits.T_MAX)
        {
            *(U8*)pUnSmart->atSmtAttrData[TEMPERATURE].abRawData=tSataDC.tSataMI.tMpCfg.bits.T_MAX;
        }
        else
        {
            *(U8*)pUnSmart->atSmtAttrData[TEMPERATURE].abRawData=tSataLogCtx.bCurrentTemp;
        }
    }

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[POWER_ON_HOURS].abRawData,(U8*)&tSataDC.dLifePowerOnTime,4);//设备运行累积时长
    //UART_PRINT("tSataDC.dPowerCycleCnt=%#010lx\r\n",tSataDC.dPowerCycleCnt);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[POWER_CYCLE_CNT].abRawData,(U8*)&tSataDC.dPowerCycleCnt,4);//设备上电次数
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[UNEXP_POWER_LOSS_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dUnExpPowerLossCnt,4);

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[RD_SCT_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReadSecCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[WR_SCT_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dWriteSecCnt,4);

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[ORIGNAL_BAD_BLK_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wOrigBadBlkCnt,2);

    //FLASH 相关的参数C3是总开关
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[NUM_OF_VALID_BLOCKS].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wValidBlockNums,2);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[UNCONRRECTABLE_SEC_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dUnCorrectSecCnt,4);

    if((tSataLogCtx.tSmartCtx.dWriteSecCnt*0x10000/*32M*/) > SG_DSATA->rTOTALCAPCITY)//累计一个满盘以后上报
    {
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[TOTAL_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dTotalEraseCnt,4);
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[MAXIMUM_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dMaxEraseCnt,4);
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[MINIMUM_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dMinEraseCnt,4);
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[AVERG_ERASE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dAvrgEraseCnt,4);
    }

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[NAND_WRITE_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dWriteSecCnt2Nand,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[IN_DIE_CPYBK_SLC].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dSLCIndieCopybackCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[IN_DIE_CPYBK_TLC].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dTLCIndieCopybackCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[IDLE_LOOP_COUNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dIdleLoopCnt,4);
    MemoryCopy((U8*)pUnSmart->atSmtAttrData[UGENCY_COUNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dUrgencyCnt,4);

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALLOC_SCT_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wReAllocatedSecCnt,2);
    if(abSmartMask[ID_05] == T_ENABLE)
    {
    }
//	else if(abSmartMask[ID_05] == T_DISABLE)
//	{
//		//do not report ID_05
//		pUnSmart->atSmtAttrData[REALLOC_SCT_CNT].bID=0x03;//05必须上报，否则软件无法解析F1F2
//	}
    else
    {
        U32 val=0;//FIX to 0
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALLOC_SCT_CNT].abRawData,(U8*)&val,4);
    }

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[RD_RETRY_NUM].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReadRetryNum,4);
    if(abSmartMask[ID_C3] == T_ENABLE)
    {
    }
    else if(abSmartMask[ID_C3] == T_DISABLE)
    {
        //do not report ID_C3
        pUnSmart->atSmtAttrData[RD_RETRY_NUM].bID=0xD3;//改写成D7
    }
    else
    {
        U32 val=0;//FIX to 0
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[RD_RETRY_NUM].abRawData,(U8*)&val,4);
    }

    if(abSmartMask[ID_VENDORPARAM0] == 0xAA)
    {
        //需要改一下上报规则
        if(tSataLogCtx.tSmartCtx.wCurPendSecCnt > 5)
        {
            U32 val=0;

            val=(tSataLogCtx.tSmartCtx.dReAllcatedEventCnt/4);
            MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALL_E_CNT].abRawData,(U8*)&val,4);
            if(abSmartMask[ID_C4] == T_ENABLE)
            {
            }
            else if(abSmartMask[ID_C4] == T_DISABLE)
            {
                //do not report ID_C4
                pUnSmart->atSmtAttrData[REALL_E_CNT].bID=0xD4;//改写成D7
            }
            else
            {
                val=0;//FIX to 0
                MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALL_E_CNT].abRawData,(U8*)&val,4);
            }

            val=tSataLogCtx.tSmartCtx.wCurPendSecCnt - 5;//个数减5个
            MemoryCopy((U8*)pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].abRawData,(U8*)&val,4);
            if(abSmartMask[ID_C5] == T_ENABLE)
            {
            }
            else if(abSmartMask[ID_C5] == T_DISABLE)
            {
                //do not report ID_C5
                pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].bID=0xD5;//改写成D7
            }
            else
            {
                U32 val=0;//FIX to 0
                MemoryCopy((U8*)pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].abRawData,(U8*)&val,4);
            }
        }
        else
        {
            U32 val=0;//没有超过5个 FIX to 0
            MemoryCopy((U8*)pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].abRawData,(U8*)&val,4);
            MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALL_E_CNT].abRawData,(U8*)&val,4);
        }
    }
    else
    {
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALL_E_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.dReAllcatedEventCnt,4);
        if(abSmartMask[ID_C4] == T_ENABLE)
        {
        }
        else if(abSmartMask[ID_C4] == T_DISABLE)
        {
            //do not report ID_C4
            pUnSmart->atSmtAttrData[REALL_E_CNT].bID=0xD4;//改写成D7
        }
        else
        {
            U32 val=0;//FIX to 0
            MemoryCopy((U8*)pUnSmart->atSmtAttrData[REALL_E_CNT].abRawData,(U8*)&val,4);
        }

        MemoryCopy((U8*)pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wCurPendSecCnt,2);
        if(abSmartMask[ID_C5] == T_ENABLE)
        {
        }
        else if(abSmartMask[ID_C5] == T_DISABLE)
        {
            //do not report ID_C5
            pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].bID=0xD5;//改写成D7
        }
        else
        {
            U32 val=0;//FIX to 0
            MemoryCopy((U8*)pUnSmart->atSmtAttrData[CUR_PEND_SEC_CNT].abRawData,(U8*)&val,4);
        }
    }

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[SMART_OFFLINE_UNCORECT_ERR_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wSmartOfflineUnconrectErrCnt,2);
    if(abSmartMask[ID_C6] == T_ENABLE)
    {
    }
    else if(abSmartMask[ID_C6] == T_DISABLE)
    {
        //do not report ID_C6
        pUnSmart->atSmtAttrData[SMART_OFFLINE_UNCORECT_ERR_CNT].bID=0xD6;//改写成D7
    }
    else
    {
        U32 val=0;//FIX to 0
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[SMART_OFFLINE_UNCORECT_ERR_CNT].abRawData,(U8*)&val,4);
    }

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT].abRawData,(U8*)&tSataLogCtx.dCRCErrorCnt,4);
    if(abSmartMask[ID_C7] == T_ENABLE)
    {
    }
    else if(abSmartMask[ID_C7] == T_DISABLE)
    {
        //do not report C7
        pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT].bID=0xD7;//C7必须上报，否则软件无法解析F1F2
    }
    else
    {
        U32 val=0;//FIX to 0
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT].abRawData,(U8*)&val,4);
    }

    MemoryCopy((U8*)pUnSmart->atSmtAttrData[PHY_ERROR_CNT].abRawData,(U8*)&tSataLogCtx.tSmartCtx.wSataPHYErrCnt,2);

    MemoryCopy((U8*)pUnSmart->abVS375,(U8*)__DATE__,sizeof(__DATE__));//记录 FW编译时间

    pUnSmart->bShortSelfTestPollTime =2;//2 + 2*((SG_DSATA->rTOTALCAPCITY/((U32)(128)*(U32)(1024*1024*2))));//每100G 花费2分钟
    pUnSmart->bExtSelfTestPollTime = 4;//10 + 10*((SG_DSATA->rTOTALCAPCITY/((U32)(128)*(U32)(1024*1024*2))));//每100G 花费2分钟

    pUnSmart->bErrLogCap=1;//Device error logging supported
    //TODO:self test status updated
    pUnSmart->bCheckSum=(~GetSum((U8*)pUnSmart,511)) + 1;
}
#endif

// *****************************************************************************
// 函数名称: UpdateIdfyLogPage08
// 函数功能: 根据Idfy 更新 IDFY LOG PAGE 08
// 入口参数: 待修改的缓冲区首地址
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void UpdateIdfyLogPage08(void *pTable)
{

 /*
  * Current SATA Settings
    Bit Meaning
    63 Shall be set to one
    62:10 Reserved
    9 AUTOMATIC PARTIAL TO SLUMBER TRANSITIONS ENABLED bit
    (see A.11.10.3.8)
    8 SOFTWARE SETTINGS PRESERVATION ENABLED bit (see A.11.10.3.7)
    7 HARDWARE FEATURE CONTROL IS ENABLED bit (see A.11.10.3.6)
    6 IN-ORDER DATA DELIVERY ENABLED bit (see A.11.10.3.5)
    5 DEVICE INITIATED POWER MANAGEMENT ENABLED bit (see A.11.10.3.4)
    4 DMA SETUP AUTO-ACTIVATION ENABLED bit (see A.11.10.3.3)
    3 NON-ZERO BUFFER OFFSETS ENABLED bit (see A.11.10.3.2)
    2:0 CURRENT NEGOTIATED SERIAL ATA SIGNAL SPEED field (see A.11.10.3.1)
  * */
 T_IDFY_LOG08 *ptLog=(T_IDFY_LOG08*)pTable;

 //DCO 可能会改动这些
 ptLog->Type.qwCapabilities=0x8000000000000000;
 ptLog->Type.qwCapabilities |=(unAtaIdentify.wArray[76]>>1);
 ptLog->Type.qwCapabilities |=((unAtaIdentify.wArray[77]>>4)<<15);
 ptLog->Type.qwCapabilities |=((unAtaIdentify.wArray[78]>>4)<<18);

 ptLog->Type.unCSS.qwCSS=0x8000000000000000;
 ptLog->Type.unCSS.BIT.bCUR_SPD = SG_DSATA->rPSSTSR.bits.SPD;//GetBits(unAtaIdentify.wArray+77,3,3);

//	 ptLog->Type.unCSS.BIT.bNZBO=GetBits(unAtaIdentify.wArray+79,1,1);
//	 ptLog->Type.unCSS.BIT.bA_DMASETUP =GetBits(unAtaIdentify.wArray+79,2,1);
//	 ptLog->Type.unCSS.BIT.bDIPM =GetBits(unAtaIdentify.wArray+79,3,1);
//	 ptLog->Type.unCSS.BIT.bIDD =GetBits(unAtaIdentify.wArray+79,4,1);
//	 ptLog->Type.unCSS.BIT.bHFC =GetBits(unAtaIdentify.wArray+79,5,1);
//	 ptLog->Type.unCSS.BIT.bSSP =GetBits(unAtaIdentify.wArray+79,6,1);;
//	 ptLog->Type.unCSS.BIT.bAPT2S =GetBits(unAtaIdentify.wArray+79,7,1);
 ptLog->Type.unCSS.qwCSS |=((unAtaIdentify.wArray[79]>>1)<<3); //等效上面注释的部分

}

void UpdateIDFYLog(CMD_STATUS eStatus,void *ptEvent,U8 pageoff,UpdateIDFYLogRouter func)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_DI_DATA;
    __pevent->eStatus=CMD_ONGOING;
    __pevent->dParam[0]=DIR_IN;
    __pevent->dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + IDY_DEV_DATA_LOG_OFFSET + pageoff;
    __pevent->dParam[2]=1;
    __pevent->dParam[3]=(U32)tSyncBuffer.bArray;
    __pevent->pfEventCompleteCb=func;
    __pevent->bStatus =eStatus;
}

void UpdateIDFYLogPage08Router(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;

    if(__pevent->dParam[0] == DIR_IN)
    {
        DEBUG_PRINT("UpdateIDFYLogPage08Router : get data\r\n");
        UpdateIdfyLogPage08((void*)__pevent->dParam[3]);
        __pevent->bOpcode=OP_DI_DATA;
        __pevent->eStatus=CMD_ONGOING;
        __pevent->dParam[0]=DIR_OUT;
        __pevent->dParam[2]=1;
        __pevent->pfEventCompleteCb=UpdateIDFYLogPage08Router;
    }
    else
    {
        DEBUG_PRINT("UpdateIDFYLogPage08Router : write over\r\n");
        __pevent->bOpcode=OP_NONE_EVENT;
        __pevent->pfEventCompleteCb=NULL;
        //TODO:可能会有异常处理
        if(tSataDC.eTransType != POWERON_COMRESET)//如果是COMRESET 引发的更新，则不需要回复D2H,否则会出现重复D2H的BUG
        {
            PRINT_TXT("SETF-D");
            ATACmd_ReleaseCommand(eStatus);
            tSataDC.dIsNDCTOGuardEn = FALSE;
        }
        else
        {
            PRINT_TXT("IDFY P08");
            //切换到COMRESET流程回复D2H
            SATA_ChangeState(POWERON_COMRESET,DHR0_HW_RESET_ASSERTED);
        }
        //--由于涉及到共用combuff的问题，所以要重新启动正在进行中的任务
        WriterGPLLogsRouter(CMD_IDLE,NULL);

    }
}

void UpdateIDFYLogPage08(CMD_STATUS eStatus,void *ptEvent)
{
    UpdateIDFYLog(eStatus,ptEvent,8,UpdateIDFYLogPage08Router);
}

void DI_FlushDone(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_NONE_EVENT;
    __pevent->pfEventCompleteCb=NULL;
    if(tSataLogCtx.bIsSelfTestRunning)
    {
        //开启self test
        SmartSelfTestRouter(CMD_IDLE,NULL);//唤醒之前被抢占的任务
    }
    else
    {
        tSataDC.tAsyncEvent.bOpcode=OP_NONE_EVENT;
        tSataDC.tAsyncEvent.pfEventCompleteCb=NULL;
    }
    //PRINT_TXT("FLSHED");//di flush done
}

void DI_Flush(void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_DI_FLUSH;
    __pevent->pfEventCompleteCb=DI_FlushDone;
    //PRINT_TXT("DIFLSH");//di flush
}

//#pragma GCC optimize("O0")
// *****************************************************************************
// 函数名称: WriterGPLLogsRouter
// 函数功能: 写所有的LOG 到FLASH 中的状态机
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void WriterGPLLogsRouter(CMD_STATUS eStatus,void *ptTaskParam)
{
    //函数内部状态机
    //准备数据缓冲区，DMA缓冲区不一定有效，要分多次写，用SATA自己的缓冲区，分多次写入LOG
    //除非有BUFF使用的互斥机制，（加锁）
    //利用DMA buff的话会加快写的过程
    static U8 bState=0;//next sate
    U32 addr=(U32)tAsyncBuffer.dArray;
    U32 lba_offset =DEVICE_STATISTIC_LOG_OFFSET;
    U8 size=1;
    //PRINT_TXT("WriterGPLLogsRouter IN");//WRITE LOG FAIL
    if(eStatus < CMD_IDLE)
    {
        //写LOG 失败,出错处理？
        PRINT_TXT("E:WRLOG");//WRITE LOG FAIL
        goto error_handler;
    }
    if((tSataDC.tAsyncEvent.pfEventCompleteCb==ReadGPLLogsRouter) || (tSataDC.tAsyncEvent.pfEventCompleteCb ==  SmartSelfTestRouter))
    {
#ifdef  __DEBUG_WHILE
        while(1);
#endif
        //被阻止运行
        tSataLogCtx.bUpdateEn=TRUE;
        tSataLogCtx.enGplUpdateEvent=GPL_E_RESTART;
        return;
    }
//    if(eStatus == CMD_IDLE)
//    {
//        //IDLE 状态下必须bState=0，有新的写任务请求，状态机重新开始写log
//        //1.定时器1小时触发一次，2.开机事件触发一次
//        bState=0;
//        tSataLogCtx.bIsGplRunning=TRUE;
//    }

    if(tSataLogCtx.bIsGplRunning == TRUE)
    {
        if(eStatus == CMD_IDLE && tSataLogCtx.bUpdateEn == FALSE)
        {
            tSataLogCtx.bUpdateEn=TRUE;
            tSataLogCtx.enGplUpdateEvent=GPL_E_RESTART;
            bState=0;
            return;
        }
    }
    else
    {
        //IDLE 状态下必须bState=0，有新的写任务请求，状态机重新开始写log
        //1.定时器1小时触发一次，2.开机事件触发一次
        bState=0;
        tSataLogCtx.bIsGplRunning=TRUE;
    }

//    if(tSataDC.tSyncEvent.bOpcode != OP_NONE_EVENT)
//    {
//        //有同步任务
//        DEBUG_PRINT("tSyncEvent insert222222222~~~~.....!\r\n");
//        return;
//    }
init_buffer:
    Mem32SetVal((U32*)addr,0U,128U);//清除数据,一个调用20byte
    DEBUG_PRINT("Enter func=<%s>[line=%d] [*********bState=%d]\r\n",__func__,__LINE__,bState);
    switch(bState)
    {
        case 0:
            if(tSataDC.bSaveUartBuff == TRUE)
            {
                //记录串口信息
                addr=SG_UART(UART_NUM)->rDMA_ADDR;
                size=(SG_UART(UART_NUM)->rDMA_LEN_TX.wAll+1)>>9;//最大2K
                lba_offset =VENDOR_ERR_LOG_ADDR + tSataLogCtx.wErrLogIdx;
                if(lba_offset >= MAX_LOG_PAGES)
                {
                    //掉头
                    //sata log剩下的区域都留给异常事件的串口log
                    tSataLogCtx.wErrLogIdx=(MAX_LOG_PAGES - VENDOR_ERR_LOG_ADDR)/2;//只从中间覆盖，保证能看到最早期的异常记录
                }
                lba_offset =VENDOR_ERR_LOG_ADDR + tSataLogCtx.wErrLogIdx;
                tSataLogCtx.wErrLogIdx+=size;
                bState=1;//下一个状态
                break;
            }
        case 1:
            //write smart data
            tSataDC.bSaveUartBuff=FALSE;//确保写成功以后再清状态
            lba_offset =SMART_DATA_OFFSET;
            Build_NoneVolatileVarTable((U8*)addr);//NONE VOLATILE VARIABLE
            bState=2;
            break;
        case 2:
            //写SMART 表备份数据
            if(tSataLogCtx.enGplUpdateEvent == GPL_E_TIMEOUT)
            {
                //timeout 要把关键变量数据做备份
                lba_offset =DEVICE_VAR_OFFSET;
                Build_NoneVolatileVarTable((U8*)addr);//NONE VOLATILE VARIABLE
                bState=3;
                break;
            }
        case 3:
            //写SATA Phy Event Counters log Format
            lba_offset =PHY_E_CONT_LOG_OFFSET;
            bState=4;//下一个状态
            if(Build_PhyEventTable((T_LOG_PHYECNT*)addr))//Phy Event Counters log
            {
                //需要更新
                break;
            }
            goto init_buffer;
        case 4:
            //写General statistic log
            lba_offset =DEVICE_STATISTIC_LOG_OFFSET + 1;
            bState=5;//下一个状态
            if(Build_GSTable((U8*)addr))//General statistic
            {
                break;
            }
            goto init_buffer;
        case 5:
            //写General Errors Statistics
            lba_offset =DEVICE_STATISTIC_LOG_OFFSET + 4;
            bState=6;//下一个状态
            if(Build_GESTable((U8*)addr))//General Errors  statistic
            {
                break;
            }
            goto init_buffer;
        case 6:
            //写Temperature Statistics LOG
            lba_offset =DEVICE_STATISTIC_LOG_OFFSET + 5;
            bState=7;//下一个状态
            if(Build_TSTable((U8*)addr))//Temperature Statistics
            {
                break;
            }
            goto init_buffer;
        case 7:
            //写Transport Statistics LOG
            lba_offset =DEVICE_STATISTIC_LOG_OFFSET + 6;
            bState=8;//下一个状态
            if(Build_TrpSTable((U8*)addr))
            {
                break;
            }
            goto init_buffer;
        case 8:
             //写TSolid State Device Statistics  LOG
            if(Build_SSDSTable((U8*)addr))//Solid State Device Statistics
            {
                lba_offset =DEVICE_STATISTIC_LOG_OFFSET + 7;
                bState=9;//下一个状态
                break;
            }
        default:
            bState=0xFF;
            break;
    }
    if(bState != 0xFF)
    {
        tSataDC.tAsyncEvent.bOpcode=OP_DI_DATA;
        tSataDC.tAsyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tAsyncEvent.dParam[0]=DIR_OUT;  //方向
        tSataDC.tAsyncEvent.dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + (lba_offset);//起始LBA
        tSataDC.tAsyncEvent.dParam[2]=size;//size
        tSataDC.tAsyncEvent.dParam[3]=addr;
        tSataDC.tAsyncEvent.pfEventCompleteCb=WriterGPLLogsRouter;//写LOG任务完成事件回调
        //PRINT_HEX("W:",tSataDC.tAsyncEvent.dParam[1]);
        //PRINT_HEX("BUFFADDR=",tSataDC.tAsyncEvent.dParam[3]);
    }
    else
    {
        bState=0;
        //所有写任务完成，退出任务
        tSataLogCtx.bIsGplRunning=FALSE;
        tSataLogCtx.bIsResetEveCnt=FALSE;
        tSataLogCtx.enGplUpdateEvent=GPL_E_NONE;//关闭本次事件属性
        DI_Flush(&tSataDC.tAsyncEvent);//触发更新表格
        //ERROR_PRINT("Exit func=<%s>[file=%s,line=%d] [*********bState=%d]\r\n",__func__,__MODULE__,__LINE__,bState);
    }
//    PRINT_TXT("WriterGPLLogsRouter OUT");
    return;

error_handler:
    tSataDC.tAsyncEvent.bOpcode=OP_NONE_EVENT;
    tSataDC.tAsyncEvent.pfEventCompleteCb =NULL;
}

// *****************************************************************************
// 函数名称: ReadGPLLogsRouter
// 函数功能: FTL 初始化工作完成 事件触发读LOG 的
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void ReadGPLLogsRouter(CMD_STATUS eStatus,void *ptTaskParam)
{
    //函数内部状态机
    static U8 bState=0;//next sate
    U8 size=1;
    U32 addr=(U32)tAsyncBuffer.dArray;
    U32 lba_offset =DEVICE_STATISTIC_LOG_OFFSET;  
    T_EVENT* pEvent=(T_EVENT*)ptTaskParam;
    //pEvent->dParam[3]+=(tSataDC.tAsyncEvent.dParam[1]-tSataDC.tSataMI.dVirtualLbaStartAddr)<<9;
    UART_PRINT("Enter func=<%s> [file=%s,line=%d] [*********bState=%d]\r\n",__func__,__MODULE__,__LINE__,bState);
    
    if(eStatus < CMD_IDLE || (tSataDC.tSataMI.dVirtualLbaStartAddr == 0))
    {
        //写LOG 失败,出错处理？
        PRINT_TXT("E:RDLOG");//READ LOG FAIL
        goto error_handler;
    }
    
    if(eStatus == CMD_IDLE)
    {
        //IDLE 状态下必须bState=0，有新的写任务请求，状态机重新开始写log
        //1.定时器1小时触发一次，2.开机事件触发一次
        bState=0;
    }
    
    switch(bState)
    {
        case 0:
            //read General statistic log 
            addr=tSataDC.tSataMI.dDmaBuffAddr;
            bState=1;//下一个状态
            size=8;
            break;
        case 1:
            //读SMART DATA LOG
            //分析read General statistic log 的返回结果
            if(eStatus >= CMD_IDLE)
            {
                UART_PRINT("read General statistic log  pEvent->dParam[3]=%#010lx\r\n",pEvent->dParam[3]);
                Restore_GSInfo((U8*)(pEvent->dParam[3]+(1U*512U)));//General statistic
                Restore_GESInfo((U8*)(pEvent->dParam[3]+(4U*512U)));//General Errors Statistics
                Restore_TSInfo((U8*)(pEvent->dParam[3]+(5U*512U)));//Temperature Statistics
                Restore_TrpSInfo((U8*)(pEvent->dParam[3]+(6U*512U)));//Transport Statistics
                Restore_SSDSInfo((U8*)(pEvent->dParam[3]+(7U*512U)));//Solid State Device Statistics 
            }
            lba_offset =SMART_DATA_OFFSET;
            bState=2;//下一个状态
            break;
        case 2:
            //分析SMART DATA LOG 数据
            if(eStatus >= CMD_IDLE && Restore_NoneVolatileVar((U8*)(pEvent->dParam[3])))
            {
                //重建相关可变的参数
                //读 self-test记录
                lba_offset =SELE_SELFTEST_LOG_OFFSET;
                bState=3;//下一个状态
            }
            else
            {
                //重读备份数据
                lba_offset =DEVICE_VAR_OFFSET;
                if((tSataDC.tAsyncEvent.dParam[1] - tSataDC.tSataMI.dVirtualLbaStartAddr) == DEVICE_VAR_OFFSET)
                {
                    //数据错误且没有备份数据
                    Mem32SetVal((U32*)addr,0U,128U);//清除数据
                    Restore_NoneVolatileVar((U8*)addr);
                    PRINT_TXT("E:NO-V,ERR");//no-volatile ear data lost！！
                    lba_offset =SELE_SELFTEST_LOG_OFFSET;
                    bState=3;//下一个状态
                }
                //保持state=2
            }
            break;
        case 3:
            //分析 self-test记录 数据
            if(eStatus >= CMD_IDLE)
            {
                Mem32Copy((U32*)&tSmartSelSelfLog,(U32*)pEvent->dParam[3],128);
            }
             //读SATA Phy Event Counters log Format
            lba_offset =PHY_E_CONT_LOG_OFFSET;
            bState=4;//下一个状态
            break;
        case 4:
            if(eStatus >= CMD_IDLE)
            {
                Restore_PhyEventCounterInfo((T_LOG_PHYECNT*)pEvent->dParam[3]);
            }
#if(SATA_DCO_ENABLE == TRUE)
            //读DCO
            lba_offset =DEVICE_DCO_SET_OFFSET;
            addr=(U32)&tSyncBuffer.wArray[0];
            bState=5;//下一个状态
            break;
        case 5:
            if(eStatus < CMD_IDLE)
            {
                tSataDC.eDCOState = DCO_Factory_config;
            }

            lba_offset =SMART_OVERLAY_OFFSET;
            bState=6;
            break;
        case 6:
            //检查SMART overlay数据合法性
             if(eStatus >= CMD_IDLE)
             {
                 if(((U32*)pEvent->dParam[3])[0] == 0x12348765 &&\
                   GetSum((U8*)pEvent->dParam[3],512) == 0)
                 {
                     //校验pass
                     //重新配置overlay 参数
                     T_SMART_OVERLAY* pSMOVLY=(T_SMART_OVERLAY*)pEvent->dParam[3];
                     //tSataDC.tSataMI.tMpCfg.bits.C7_DIS=pSMOVLY->m_bSmartC7;//0是ENABLE,1是DISABLE
                     abSmartMask[ID_C7]=pSMOVLY->m_bSmartC7;
                     //tSataDC.tSataMI.tMpCfg.bits.C7_FIX=(pSMOVLY->m_bSmartC7 == 2);//2固定为0
                     tSataDC.tSataMI.tMpCfg.bits.T_FIX=(pSMOVLY->m_bFixTemperature!=0);
                     tSataDC.tSataMI.tMpCfg.bits.T_MAX=pSMOVLY->m_bAjustTemperature;
                     tSataDC.tSataMI.tMpCfg.bits.T_VAL=pSMOVLY->m_bAjustTemperature;
                     //tSataDC.tSataMI.tMpCfg.bits.C3_DIS=pSMOVLY->m_bSmartC3;
                     abSmartMask[ID_C3]=pSMOVLY->m_bSmartC3;
                     //tSataDC.tSataMI.tMpCfg.bits.C3_FIX=(pSMOVLY->m_bSmartC3 == 2);
                 }
                 else
                 {
                     //PRINT_TXT("E:SMOVLY");
                 }
             }
#endif
            lba_offset =IDY_DEV_DATA_LOG_OFFSET + 1;/*Copy of IDENTIFY DEVICE data page*/
            bState=7;
            break;
        case 7:
             if(eStatus >= CMD_IDLE)
             {
                //读 identify是最后一步，校验数据
                if(((UN_ATA_INDETIFY_T*)pEvent->dParam[3])->bArray[510] == 0xA5 &&\
                    GetSum((U8*)pEvent->dParam[3],512) == 0)
                {
                    //校验pass拷贝到code区
                    Mem32Copy((U32*)unAtaIdentify_C.dwArray,(U32*)pEvent->dParam[3],sizeof(UN_ATA_INDETIFY_T)>>2);
                }
                else
                {
                    //PRINT_TXT("E:IDFY");
                }
             }
        default:
            bState=0xFF;
            break;
    }
    if(bState != 0xFF)
    {
        //FTL初始化完成,发起读取LOG任务事件
        tSataDC.tAsyncEvent.bOpcode=OP_DI_DATA;
        tSataDC.tAsyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tAsyncEvent.dParam[0]=DIR_IN;  //方向
        tSataDC.tAsyncEvent.dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + (lba_offset);//起始LBA
        tSataDC.tAsyncEvent.dParam[2]=size;
        tSataDC.tAsyncEvent.dParam[3]=addr;
        tSataDC.tAsyncEvent.pfEventCompleteCb=ReadGPLLogsRouter;//读LOG任务完成事件回调
        //PRINT_HEX("R:",tSataDC.tAsyncEvent.dParam[1]);
    }
    else
    {
        //上电初始化完成，分析IDENTIFY 数据
        Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
        //量产算法更新容量
//        unAtaIdentify.wArray[60] =tSataDC.tSataMI.dVirtualLbaStartAddr&0xFFFF;
//        unAtaIdentify.wArray[61] =(tSataDC.tSataMI.dVirtualLbaStartAddr>>16)&0xFFFF;;
//        unAtaIdentify.wArray[100] =tSataDC.tSataMI.dVirtualLbaStartAddr&0xFFFF;;
//        unAtaIdentify.wArray[101] =(tSataDC.tSataMI.dVirtualLbaStartAddr>>16)&0xFFFF;
//        unAtaIdentify.wArray[102] =0;
//        unAtaIdentify.wArray[103] =0;
        Update_IdfyParam();
        bState=0;
        //读LOG结束，写入上电次数到FLASH
        NVIC_EnableIRQ(IRQ_SC);//开启了SATA LOG 以后等到，读完相关log之后再开启SATA通信
        tSataDC.tAsyncEvent.pfEventCompleteCb=NULL;
        WriterGPLLogsRouter(CMD_IDLE,NULL);
        UART_PRINT("Exit func=<%s>[file=%s,line=%d] [*********bState=%d]\r\n",__func__,__MODULE__,__LINE__,bState);
    } 
    return;
error_handler:
    tSataDC.tAsyncEvent.bOpcode=OP_NONE_EVENT;
    //上电初始化完成，分析IDENTIFY 数据
    Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
    //量产算法更新容量
//        unAtaIdentify.wArray[60] =tSataDC.tSataMI.dVirtualLbaStartAddr&0xFFFF;
//        unAtaIdentify.wArray[61] =(tSataDC.tSataMI.dVirtualLbaStartAddr>>16)&0xFFFF;;
//        unAtaIdentify.wArray[100] =tSataDC.tSataMI.dVirtualLbaStartAddr&0xFFFF;;
//        unAtaIdentify.wArray[101] =(tSataDC.tSataMI.dVirtualLbaStartAddr>>16)&0xFFFF;
//        unAtaIdentify.wArray[102] =0;
//        unAtaIdentify.wArray[103] =0;
    Update_IdfyParam();
    bState=0;
    //读LOG结束，写入上电次数到FLASH
    NVIC_EnableIRQ(IRQ_SC);//开启了SATA LOG 以后等到，读完相关log之后再开启SATA通信
    tSataDC.tAsyncEvent.pfEventCompleteCb=NULL;
    WriterGPLLogsRouter(CMD_IDLE,NULL);
}

// *****************************************************************************
// 函数名称: CMD_STATUS ATACmd_LogAddr2Lba(T_H2D *tpH2d,EN_RW_MODE enRwMode,EN_ACC_MODE enAccMode)
// 函数功能: 子函数
// 入口参数: tpH2d FSM buffer 地址
//          enRwMode 读写操作模式
//          enAccMode 访问方式 GPL/SL
// 出口参数: NONE
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_LogAddr2Lba(T_H2D *tpH2d,EN_RW_MODE enRwMode,EN_ACC_MODE enAccMode)
{
    U8 i;
    U8 bLogAddr=tpH2d->dw1.bits.bLBA_low;
    U16 wPageNums;
    U16 wCounts;
    //UART_PRINT("Enter func=<%s> [file=%s,line=%d] \r\n",__func__,__MODULE__,__LINE__); 
    //A device may return command aborted if an Interface CRC error has occurred.
    
    if(enAccMode == SL)
    {
        wPageNums=0;
        wCounts=(tpH2d->dw3.dAll&0xFF);
    }
    else
    {
        wPageNums=tpH2d->dw1.bits.bLBA_mid+(tpH2d->dw2.bits.bExpLBA_mid<<8);
        wCounts=(tpH2d->dw3.dAll&0xFFFF);
    }
    //参数检查
    if(0 == wCounts)
    {
       return CMD_ABORT; 
    }
    
    for(i=0;i<sizeof(tLogMaps)/sizeof(T_LOG_MAP);i++)
    {
        if(tLogMaps[i].bLogAddr == bLogAddr)
        {
            //找到LOG
            break;
        }
    }
    if(i<sizeof(tLogMaps)/sizeof(T_LOG_MAP))
    {
        //找到LOG 记录，检查相关参数
        if(((tLogMaps[i].enAccMode != GPL_SL) &&(enAccMode != tLogMaps[i].enAccMode))|| //SL/GPL访问权限冲突
            ((enRwMode == WO) && (tLogMaps[i].enRwMode == RO))||//读写权限冲突
            (wPageNums + wCounts) > tLogMaps[i].wPageNums) //访问大小越界
        {
            return CMD_ABORT; 
        }
        else
        {
            //！！使用之前一定要初始化  dVirtualLbaStartAddr
            tTQC.tHiTasks[0].tParam.dLba=tLogMaps[i].bLbaOffSet+tSataDC.tSataMI.dVirtualLbaStartAddr + wPageNums;//对于 Device Statistics pages -wPageNums 就是其业内偏移地址
            PRINT_HEX("L:",tTQC.tHiTasks[0].tParam.dLba);
            tTQC.tHiTasks[0].tParam.dSctSize=wCounts;

            if(bLogAddr == 0x11)
            {
                if((tpH2d->dw0.bits.bFeatures_L & 0x01) != 0)
                {
                    tSataLogCtx.dCmdFailDue2ICRC =0;
                    tSataLogCtx.dPHYRDY2N = 0;
                    tSataLogCtx.dD2hFisDue2Comreset = 0;
                    tSataLogCtx.dCRCErrorCnt =0;
                    tSataLogCtx.dNoneCRCErrorCnt =0;
                    WriterGPLLogsRouter(CMD_IDLE,NULL);//更新状态
                    //标记count计数正在更新中
                    tSataLogCtx.bIsResetEveCnt=TRUE;
                }
                else
                {
                    if(tSataLogCtx.bIsResetEveCnt)
                    {
                        T_HI_TASK tTask;
                        //如果正在更新中的话，直接回复全0数据
                        Build_PhyEventTable((T_LOG_PHYECNT*)tAsyncBuffer.dArray);
                        tTask.eStatus=CMD_VALID;
                        tTask.tParam.bTag=CMD_SLOT0;
                        tTask.tParam.bDir=DIR_IN;
                        tTask.tParam.dBuffAddr=(U32)(&tAsyncBuffer.dArray);
                        tTask.tParam.dSctSize=1;

                        UN_CH_DI unDI;
                        if(tSataDC.eTransType == DMA_DATA_IN)
                        {
                            unDI.dAll=0x40;
                        }
                        else
                        {
                            unDI.dAll=0xC0;
                        }

                        DSATA_SetCmdHead(CMD_SLOT0,unDI,1,tSataDC.bDrq,1<<9);

                        tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,NULL);
                        //PRINT_TXT("RESET EVENT COUNT");
                        return CMD_COMPLETED;
                    }
                }
            }
            return CMD_VALID;
        }
        
    }
    else
    {
        //遍历完数组，没有找到对应LOG信息
        return CMD_ABORT;
    }
}
#else
// *****************************************************************************
// 函数名称: CMD_STATUS ATACmd_LogAddr2Lba(T_H2D *tpH2d,EN_RW_MODE enRwMode,EN_ACC_MODE enAccMode)
// 函数功能: 子函数
// 入口参数: tpH2d FSM buffer 地址
//          enRwMode 读写操作模式
//          enAccMode 访问方式 GPL/SL
// 出口参数: NONE
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_LogAddr2Lba(T_H2D *tpH2d,EN_RW_MODE enRwMode,EN_ACC_MODE enAccMode)
{
    U16 wPageNums;
    U16 wCounts;
    //UART_PRINT("Enter func=<%s> [file=%s,line=%d] \r\n",__func__,__MODULE__,__LINE__);
    //A device may return command aborted if an Interface CRC error has occurred.

    if(enAccMode == SL)
    {
        wPageNums=0;
        wCounts=(tpH2d->dw3.dAll&0xFF);
    }
    else
    {
        wPageNums=tpH2d->dw1.bits.bLBA_mid+(tpH2d->dw2.bits.bExpLBA_mid<<8);
        wCounts=(tpH2d->dw3.dAll&0xFFFF);
    }
    wPageNums = wPageNums;
    //参数检查
    if(0 == wCounts)
    {
       return CMD_ABORT;
    }
    tTQC.tHiTasks[0].tParam.dLba=0;
    tTQC.tHiTasks[0].tParam.dSctSize=wCounts;
    return CMD_VALID;
}
#endif /*SATA_LOG_ENABLE==1*/
#ifndef __BOOT_CODE__
// *****************************************************************************
// 函数名称: ATACmd_ReadLogExt
// 函数功能: 实现ATA命令 READ LOG EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ LOG EXT - 2Fh, PIO Data-In
// *****************************************************************************
void ATACmd_ReadLogExt(void)
{
    CMD_STATUS eStatus;
    //UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    eStatus=ATACmd_LogAddr2Lba(&unFsmBuff.FIS.h2d,RO,GPL);
    ATACmd_CreatePioTask(eStatus,DIR_IN);
    //UART_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_WriteLogExt
// 函数功能: 实现ATA命令 WRITE LOG EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE LOG EXT - 3Fh, PIO Data-Out
// *****************************************************************************
void ATACmd_WriteLogExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus=ATACmd_LogAddr2Lba(&unFsmBuff.FIS.h2d,WO,GPL);
    ATACmd_CreatePioTask(eStatus,DIR_OUT);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_WriteLogDmaExt
// 函数功能: 实现ATA命令 WRITE LOG DMA EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: WRITE LOG DMA EXT - 57h, DMA
// *****************************************************************************
void ATACmd_ReadLogDmaExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_LogAddr2Lba(&unFsmBuff.FIS.h2d,RO,GPL);
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_IN);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_ReadLogDmaExt
// 函数功能: 实现ATA命令 READ LOG DMA EXT
// 入口参数: NONE
// 出口参数: NONE
// 说    明: READ LOG DMA EXT - 47h, DMA
// *****************************************************************************
void ATACmd_WriteLogDmaExt(void)
{
    CMD_STATUS eStatus;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
    eStatus=ATACmd_LogAddr2Lba(&unFsmBuff.FIS.h2d,WO,GPL);
    ATACmd_CreateDmaTask(OP_HI_DATA,eStatus,DIR_OUT);
    
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#endif
