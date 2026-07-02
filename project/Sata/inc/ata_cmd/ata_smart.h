// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  ata_smart.h
//  文件标识：  
//  内容摘要：  ata 命令相关定义
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
#ifndef _ATA_SMART_H_
#define _ATA_SMART_H_
/******************************包含头文件******************************/
#include "sata_typedef.h"

/******************************类型和宏定义******************************/
//#define __SMART_1__

#define SHIFT_32M_NUM    (16)    // 按32M 分段，    0x10000 为  N  	 32M = N * 512 Byte   0x10000相当于左移


#ifndef __SMART_1__
#define __SMART_2__
#endif

typedef enum
{
	ID_05,
	ID_C3,
	ID_C4,
	ID_C5,
	ID_C6,
	ID_C7,
	ID_VENDORPARAM0,
	ID_RFU
}EN_SMART_MASK;

typedef enum
{
	T_ENABLE,
	T_DISABLE,
	T_FIX0,
	T_RFU
}EN_MASK_TYPE;


#ifdef __SMART_1__
typedef enum
{
    RD_ERROR_RATE=0,//Read Error Rate 01h
    THROU_PERFORMANCE,//Throughput Performance 02h
    SPIN_UP_TIME,//Spin Up Time 03h
    REALLOC_SCT_CNT,//Reallocated Sector Count 05h
    SEEK_ERROR_RATE,//Seek Error Rate 07h
    SEEK_TIME_PERFORMANCE,//Seek Time performance 08h
    POWER_ON_HOURS,//Power-On hours Count 09h
    SPIN_RETRY_CNT,//Spin Retry Count 0ah
    POWER_CYCLE_CNT,//Drive Power Cycle Count 0ch
    UNCONRRECTABLE_SEC_CNT,//Uncorrectable sector count when r/w A0H
    NUM_OF_VALID_BLOCKS,//Number of valid spare block A1H
    ORIGNAL_BAD_BLK_CNT,//Orignal Bad Block Count A3h
    TOTAL_ERASE_CNT,//TOTAL Erase Count A4H
    MAXIMUM_ERASE_CNT,//MAXIMUM Erase Count A5H
    MINIMUM_ERASE_CNT,//MINIMUM Erase Count A6H
    AVERG_ERASE_CNT,//Average erase count a7h
    PHY_ERROR_CNT,//SATA PHY Error Count A8H
    BAD_BLK_CNT,//Bad Block Count A9H
    MAX_BAD_BLK_CNT,//Maximum Bad Block Count AAH
    BAD_CLUSTER_CNT,//Bad Cluster Table Count AFH
    SATA_DOWNSHIFT_ERR_CNT,//SATA downshift err count B7H
    UNEXP_POWER_LOSS_CNT,//Unexpected Power Loss Count --异常掉电以后，恢复过程中作记录 C0H
    TEMPERATURE,//Temperature C2H
    RD_RETRY_NUM,//Read retry number C3H
    REALL_E_CNT,//Reallocated event Count C4H
    SATA_CRC_ERR_CNT,//Sata PHY retry count C7H
    WR_SCT_CNT2NAND,//Write Sector Count to Nand E9H
    RD_SCT_CNT2NAND,//Read Sector Count from Nand EAH
    WR_SCT_CNT,//Write Sector Count F1H
    RD_SCT_CNT,//Read Sector Count F2H
}EN_SMT_ID;
#endif

#ifdef __SMART_2__
//该定义需要和EXCEL文件对应起来
typedef enum
{
    RD_ERROR_RATE=0,//Read Error Rate 01h
    REALLOC_SCT_CNT,//Reallocated Sector Count 05h
    POWER_ON_HOURS,//Power-On hours Count 09h
    POWER_CYCLE_CNT,//Drive Power Cycle Count 0ch
    UNCONRRECTABLE_SEC_CNT,//Uncorrectable sector count when r/w A0H
    NUM_OF_VALID_BLOCKS,//Number of valid spare block A1H
    ORIGNAL_BAD_BLK_CNT,//Orignal Bad Block Count A3h
    TOTAL_ERASE_CNT,//TOTAL Erase Count A4H
    MAXIMUM_ERASE_CNT,//MAXIMUM Erase Count A5H
    MINIMUM_ERASE_CNT,//MINIMUM Erase Count A6H
    AVERG_ERASE_CNT,//Average erase count a7h
    PHY_ERROR_CNT,//SATA PHY Error Count A8H
    BAD_BLK_CNT,//Bad Block Count A9H
    UGENCY_COUNT,// AFH
    IN_DIE_CPYBK_SLC,// B0H
    IN_DIE_CPYBK_TLC,// B1H
    IDLE_LOOP_COUNT,// B2H
    NAND_PROGRAM_FAIL_CNT,// program fail count B5H
    NAND_ERASE_FAIL_CNT,// erase fail count B6H
    UNEXP_POWER_LOSS_CNT,//Unexpected Power Loss Count --异常掉电以后，恢复过程中作记录 C0H
    TEMPERATURE,//Temperature C2H
    RD_RETRY_NUM,//Read retry number C3H
    REALL_E_CNT,//Reallocated event Count C4H
    CUR_PEND_SEC_CNT,//Current Pending Sector Count C5H
    SMART_OFFLINE_UNCORECT_ERR_CNT,//Smart Off-line Scan Uncorrectable Error Count C6H
    SATA_CRC_ERR_CNT,//Sata PHY retry count C7H
    NAND_RESERVED_SPACE,//available reserved space E8H  --百分比
    WR_SCT_CNT,//Write Sector Count F1H 32m
    RD_SCT_CNT,//Read Sector Count F2H 32m
    //NAND_CUMU_PROGRAM_PAGES,//cumulative program NAND pages F5H
    NAND_WRITE_CNT,//Total NAND Writes F9h. Raw value reports the number of writes to NAND in 1 GB increments.
}EN_SMT_ID;
#endif

typedef struct
{
    U64 qwStartingLba;
    U64 qwEndingLba;
}T_SPANX;

#pragma  pack(1)

typedef struct
{
    U16 wVersion;
    T_SPANX Spans[5];
    U8 bRFU82[256];
    U8 bVS338[154];
    U64 qwCurLbaUDT;
    U16 wCurSpanUDT;
    U16 wFeatureFlags;
    U8 bVS504[4];
    U16 wSelSelfPendingTime;
    U8 bReserved;
    U8 bCheckSum;
}T_SEL_SELFLOG;

typedef struct
{
    U8 bLba;//Content of the LBA field
    U8 bSelfES;// self-test execution status
    U16 wTimeStamp;//Life timestamp shall contain the power-on lifetime of the device in hours when the nth self-test subcommand wascompleted
    U8 bFail;
    U8 bLba28[4];
    U8 bVS9[15];
}T_DES_ENTRY28;

typedef struct
{
    U8 bLba;//Content of the LBA field
    U8 bSelfES;// self-test execution status
    U16 wTimeStamp;//Life timestamp shall contain the power-on lifetime of the device in hours when the nth self-test subcommand wascompleted
    U8 bFail;
    U8 bLba48[6];
    U8 bVS11[15];
}T_DES_ENTRY48;

typedef struct
{
    U16 wVersion;//0001
    T_DES_ENTRY28 tDesEntrys[21];
    U8 bVS506[2];
    U8 bIndex;
    U8 bReserved[2];
    U8 bCheckSum;
}T_SELF_TEST_LOG;

typedef struct
{
    U8 bVersion;//01
    U8 bReserved0;
    U16 wIndex;
    T_DES_ENTRY48 tDesEntrys[19];
    U8 bVS498[2];
    U8 bReserved1[11];
    U8 bCheckSum;
}T_EXTSELF_TEST_LOG;

#pragma  pack(4)
//用于建SMART表,维护SMART
typedef struct _SMART_CTX
{
    /*HDD相关？*/
    U8 bReadErrorRate;
    U8 bThroughPutPerformance;
    U8 bSeekErrorRate;
    U8 bSeekTimePerformance;
   
    U8 bSpinRetryCnt;
    U8 bRFU;
    U8 bWriteHead;
    /*NAND FLASH相关，FTL算法相关，可以反应算法执行环境*/
    U8 bAvaResvdSpace;//available reserved space E8H --百分比

    U32 dReadRetryNum;//Read retry number
    
    U32 dUnCorrectSecCnt;//Uncorrectable sector count when r/w

    U16 wValidBlockNums;//Number of valid spare block A1H
    U16 wBadBlkCnt;//Bad Block Count

    U16 wOrigBadBlkCnt;//Number of original bad block
    U16 wProgramFailCnt;//program fail

    U16 wEraseFailCnt;//erase fail

#ifdef __SMART_1__
    U16 wBadClusterTblCnt;//Bad Cluster Table Count
    U16 wMaxBadBlkCnt;//Maximum Bad Block Count
#endif

#ifdef __SMART_2__
    U16 wCurPendSecCnt;//CUR_PEND_SEC_CNT
    U16 wSmartOfflineUnconrectErrCnt;//SMART_OFFLINE_UNCORECT_ERR_CNT
#endif
    
    U32 dTotalEraseCnt;//TOTAL Erase Count
    U32 dMaxEraseCnt;//Maximum erase count
    U32 dMinEraseCnt;//Minimum Erase Count
    U32 dAvrgEraseCnt;//Average erase count

    U32 dUnExpPowerLossCnt;//Unexpected Power Loss Count
    
    //数据读写统计,可以估算写入放大比
    U32 dWriteSecCnt2Nand;//写入NAND FLASH的数据量  以32M数据为单位，每32M加1
    U32 dReadSecCntFromNand;//从NAND FLASH读的数据量 以32M数据为单位 每32M加1

    U32 dWriteSecCnt;//SATA
    U32 dReadSecCnt;//SATA

    U16 wWriteSecCnt2Nand;//写入NAND FLASH的数据量，以1sec为单位，累加到10000(32M),清0
    U16 wReadSecCntFromNand;//从NAND FLASH读的数据量 以1sec为单位，累加到10000(32M),清0
    U16 wWriteSecCnt;//SATA
    U16 wReadSecCnt;//SATA

    //用户定义必须实现
    U16 wReAllocatedSecCnt;//Reallocated Sector Count later bad block //新增坏块数
    U16 wSataPHYErrCnt;//SATA

    U32 dReAllcatedEventCnt;//Reallocated event Count  //read retry fail 次数
    U32 wSataDownShiftErrCnt;//SATA 掉速次数统计（没有lock在gen3？）

    U32 dSLCIndieCopybackCnt;
    U32 dTLCIndieCopybackCnt;
    U32 dIdleLoopCnt;
    U32 dUrgencyCnt;

}T_SMART_CTX;

#pragma  pack(1)    //按字节对齐--unSmartDS.atSmtAttrData[0].unFlag.wFlag=(U16)(0xFFFF); //--地址不对齐，编译器会自动进行处理
typedef struct _ATTRI_VALUE
{
    U8  bID;//属性ID
    
    union 
    {
        U16 wFlag;//属性标识
        U8 bThreshold;//保留
        struct
        {
            U16 PFA:1; //Pre-fail/Advisory bit
                        /*
                         This bit is applicable only when the value of this attribute is less than or
                                equal to its threshhold.
                                0 : Advisory: The usage of age of the device has
                                    exceeded its intended design life period
                                1: Pre-failure notification:
                                   Failure is predicted within 24 hours
                        */
            U16 ODC:1; // Online data collection bit 
                        /* 
                           0: This value of this attribute is only updated during offline activities
                           1: The value of this attribute is updated during
                              both normal operation and offline activities
                        */
            U16 VS:4;//自定义vendor specific
            U16 Reserved:10;//保留位
        }bits;
    }unFlag; 
    
    U8 bCurrent;//current
    U8 bWorst;//worst
    U8 abRawData[6];
    U8 bReseverd;//保留
    
}T_ATTRI_VALUE;

typedef struct _UN_SMART_DATA_STRUCTURE
{
    U16 wVS0;//Vendor specific
    
    T_ATTRI_VALUE  atSmtAttrData[30];//Smart attribute data
    
    U8 bOffLineDCSts;//Off-line data collection status
    U8 bSelfTestExeSts;//Self-test execution status byte
    U16 wTotalTime;//Total time in seconds to complete off-line data collection activity
    U8 bVS366;
    U8 bOffLineDCCap;//Off-line data collection capability
    U16 wSmartCap;//SMART capability
    U8 bErrLogCap;//Error logging capability
                    /*
                    7:1 Reserved
                        0:1=Device error logging supported
                    */
    U8 bVS371;
    U8 bShortSelfTestPollTime;//Short self-test routine recommended polling time (default:1)
    U8 bExtSelfTestPollTime;//Extended self-test routine recommended polling time (default:1)
    U8 bConvSelfTestPollTime;//Conveyance self-test routine recommended polling time (default:0)
    U8 abVS375[133];//Vendor specific
    U16 wRevisionNum;//Revision Number
    U8 bVS510;
    U8 bCheckSum; //Checksum
}UN_SMART_DATA_STRUCTURE;

#pragma  pack(4)  
/******************************变量声明******************************/
extern UN_SMART_DATA_STRUCTURE unSmartDS_C;
extern U8 abSmartMask[];
#if (SATA_LOG_ENABLE == 1)
extern T_SEL_SELFLOG tSmartSelSelfLog;
#endif

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

    // *****************************************************************************
    // 函数名称: ATACmd_SmartFeatureSet
    // 函数功能: 实现ATA命令 SMART
    // 入口参数: NONE
    // 出口参数: 
    // 说    明: SMART 0xB0
    // *****************************************************************************
    void ATACmd_SmartFeatureSet(void);
    
#if (SATA_LOG_ENABLE == 1)    
    // *****************************************************************************
    // 函数名称: WriterSmartErrorLogsRouter
    // 函数功能: 写所有的smart error log 到FLASH 中的状态机
    // 入口参数: eStatus:任务完成状态
    //           ptTaskParam:前一个完成的任务相关参数
    // 出口参数: NONE
    // 说    明: 
    // *****************************************************************************
    void WriterSmartErrorLogsRouter(CMD_STATUS eStatus,void *ptTaskParam);
    
    // *****************************************************************************
    // 函数名称: SmartSelfTestRouter
    // 函数功能: 写所有的smart self test router到FLASH 中的状态机
    // 入口参数: eStatus:任务完成状态
    //           ptTaskParam:前一个完成的任务相关参数
    // 出口参数: NONE
    // 说    明: 
    // *****************************************************************************
    void SmartSelfTestRouter(CMD_STATUS eStatus,void *ptTaskParam);
#endif
    
#ifdef __cplusplus
}
#endif
#endif/*_ATA_SMART_H_*/



