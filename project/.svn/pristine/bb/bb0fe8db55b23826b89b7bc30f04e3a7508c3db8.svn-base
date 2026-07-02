// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司
//   
//  文件名称：  ata_gpl.h
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
#ifndef _ATA_GPL_H_
#define _ATA_GPL_H_
/******************************包含头文件******************************/
#include "sata_typedef.h"

#include "ata_smart.h"
/******************************类型和宏定义******************************/
#define LOG_UPDATE_INTERVAL                     (60000U*10)    //IDLE 状态下10分钟保存一次

#define MAX_LOG_PAGES                           (2048)//1M空间
#define LOG_DIRECTORY_LOG_ADDR                  (0x00)
#define LOG_DIRECTORY_SIZE                      (1U)
#define LOG_DIRECTORY_OFFSET                    (0U)

#define SUMMARY_SMART_ERROR_LOG_ADDR            (0x01)
#define COMPH_SMART_ERROR_LOG_ADDR              (0x02)
#define COMPH_SMART_ERROR_LOG_SIZE              (1U)
#define MAX_SMART_ERROR_LOG_IDX                 (COMPH_SMART_ERROR_LOG_SIZE * 5U) //1个PAGE 可以存5条信息
#define COMPH_SMART_ERROR_LOG_OFFSET            (LOG_DIRECTORY_OFFSET+LOG_DIRECTORY_SIZE)
    
#define EXT_COMPH_SMART_ERROR_LOG_ADDR          (0x03)
#define EXT_COMPH_SMART_ERROR_LOG_SIZE          (1U)
#define MAX_EXT_SMART_ERROR_LOG_IDX             (EXT_COMPH_SMART_ERROR_LOG_SIZE * 4U) //1个PAGE 可以存4条信息
#define EXT_COMPH_SMART_ERROR_LOG_OFFSET        (COMPH_SMART_ERROR_LOG_OFFSET+COMPH_SMART_ERROR_LOG_SIZE)

#define DEVICE_STATISTIC_LOG_ADDR               (0x04)
#define DEVICE_STATISTIC_LOG_SIZE               (8U)
#define DEVICE_STATISTIC_LOG_OFFSET             (EXT_COMPH_SMART_ERROR_LOG_OFFSET+EXT_COMPH_SMART_ERROR_LOG_SIZE)//首地址

#define SMART_SELFTEST_LOG_ADDR                 (0x06)
#define SMART_SELFTEST_LOG_SIZE                 (1U)
#define MAX_SMART_SELFTEST_IDX					(21U * SMART_SELFTEST_LOG_SIZE)
#define SMART_SELFTEST_LOG_OFFSET               (DEVICE_STATISTIC_LOG_OFFSET+DEVICE_STATISTIC_LOG_SIZE)

#define EXT_SMART_SELFTEST_LOG_ADDR             (0x07)
#define EXT_SMART_SELFTEST_LOG_SIZE             (1U)	//MAX 3449
#define MAX_EXT_SMART_SELFTEST_IDX				(19U * EXT_SMART_SELFTEST_LOG_SIZE)
#define EXT_SMART_SELFTEST_LOG_OFFSET           (SMART_SELFTEST_LOG_OFFSET+SMART_SELFTEST_LOG_SIZE)

#define SELE_SELFTEST_LOG_ADDR                  (0x09)
#define SELE_SELFTEST_LOG_SIZE                  (1U)
#define SELE_SELFTEST_LOG_OFFSET                (EXT_SMART_SELFTEST_LOG_OFFSET+EXT_SMART_SELFTEST_LOG_SIZE)

#define NCQ_ERROR_LOG_ADDR                      (0x10)
#define NCQ_ERROR_LOG_SIZE                      (1U)
#define NCQ_ERROR_LOG_OFFSET                    (SELE_SELFTEST_LOG_OFFSET+SELE_SELFTEST_LOG_SIZE)

#define PHY_E_CONT_LOG_ADDR                     (0x11)
#define PHY_E_CONT_LOG_SIZE                     (1U)
#define PHY_E_CONT_LOG_OFFSET                   (NCQ_ERROR_LOG_OFFSET+NCQ_ERROR_LOG_SIZE)

//IDENTIFY DEVICE data log (Log Address 30h)
#define IDY_DEV_DATA_LOG_ADDR                   (0x30)
#define IDY_DEV_DATA_LOG_SIZE                   (9U)
#define IDY_DEV_DATA_LOG_OFFSET                 (PHY_E_CONT_LOG_OFFSET+PHY_E_CONT_LOG_SIZE)//首地址

#define HOST_VENDOR_LOG_ADDR(i)                 (i)
#define HOST_VENDOR_LOG_SIZE                    (16U)
#define HOST_VENDOR_LOG_OFFSET(i)               (IDY_DEV_DATA_LOG_OFFSET+IDY_DEV_DATA_LOG_SIZE+HOST_VENDOR_LOG_SIZE*((i)-HOST_VENDOR_LOG_ADDR(0x80)))


#define DEVICE_VENDOR_LOG_ADDR                  (0xA0)
#define DEVICE_VENDOR_LOG_SIZE                  (16U)
#define DEVICE_VENDOR_LOG_OFFSET                (HOST_VENDOR_LOG_OFFSET(0x9F)+HOST_VENDOR_LOG_SIZE)

#define SCT_CMD_LOG_ADDR                        (0xE0)
#define SCT_CMD_LOG_SIZE                        (1U)
#define SCT_CMD_LOG_OFFSET                      (DEVICE_VENDOR_LOG_OFFSET+DEVICE_VENDOR_LOG_SIZE)

#define SCT_DATA_LOG_ADDR                       (0xE1)
#define SCT_DATA_LOG_SIZE                       (1U)
#define SCT_DATA_LOG_OFFSET                     (SCT_CMD_LOG_OFFSET+SCT_CMD_LOG_SIZE)    //22A

#define SMART_DATA_OFFSET						(SCT_DATA_LOG_OFFSET + SCT_DATA_LOG_SIZE) //22B--SMART 在该地址，PAGE0处
#define SMART_DATA_LOG_SIZE          		    (1U)

#define SMART_OVERLAY_OFFSET				    (SMART_DATA_OFFSET + SMART_DATA_LOG_SIZE)   //22C--原始备份数据 SMART 在该地址，PAGE0处
#define SMART_OVERLAY_LOG_SIZE                  (1U)

#define DEVICE_PASSWORD_OFFSET					(SMART_OVERLAY_OFFSET + SMART_OVERLAY_LOG_SIZE)//--DEVICE USER PASSWORD 在该地址，PAGE0处
#define DEVICE_PASSWORD_LOG_SIZE                (2U)

#define DEVICE_DCO_SET_OFFSET					(DEVICE_PASSWORD_OFFSET + DEVICE_PASSWORD_LOG_SIZE)//--DCO 配置的数据
#define DEVICE_DCO_SET_LOG_SIZE                 (1U)

#define DEVICE_VAR_OFFSET						(DEVICE_DCO_SET_OFFSET + DEVICE_DCO_SET_LOG_SIZE)//--DEVICE VAR DATA DEFINE
#define DEVICE_VAR_LOG_SIZE                		(1U)

#define HOST_RWBUFF_OFFSET						(DEVICE_VAR_OFFSET + DEVICE_VAR_LOG_SIZE)//--DEVICE VAR DATA DEFINE
#define HOST_RWBUFF_SIZE                		(1U)

#define VENDOR_ERR_LOG_ADDR                     (HOST_RWBUFF_OFFSET + HOST_RWBUFF_SIZE)
#define VENDOR_ERR_LOG_SIZE                     (MAX_LOG_PAGES-VENDOR_ERR_LOG_ADDR)

typedef enum _GPL_E_TYPE{
    GPL_E_NONE=0,
    GPL_E_TIMEOUT,//超时触发写动作，此事件不能被阻塞
    GPL_E_RW,//读写任务触发写动作 --此事件可以被HOST 任务阻塞
    GPL_E_RESTART,//正在写的过程中又有新的任务过来需要restart
}GPL_E_TYPE;

typedef enum __attribute__ ((__packed__))
{
    RO=0,
    WO,
    RW
}EN_RW_MODE;

typedef enum __attribute__ ((__packed__))
{
    M_OFF_LINE=0,//后台操作
    M_CAPTIVE,//前台
    M_ABORT,
}EN_TEST_MODE;

typedef enum __attribute__ ((__packed__))
{
    R_OFF_LINE=0,//SMART off-line routine
    R_SHORT_SELFTEST,//Short self-test routine
    R_EXT_SELFTEST,//Extended self-test routine
    R_CONVE_SELFTEST,//Conveyance self-test routine
    R_SEL_SELFTEST,//Selective self-test routine
    R_UNDIFINE,
}EN_ROUTE_MODE;

typedef enum __attribute__ ((__packed__))
{
    GPL=0,//ACCESS BY GENERAL LOG
    SL,//ACCESS BY SMART LOG
    GPL_SL,//ACCESS BY BOTH
}EN_ACC_MODE;

typedef struct LOG_MAP{
    U8 bLogAddr;
    U16 wPageNums;
    U16 bLbaOffSet;
    EN_RW_MODE enRwMode;
    EN_ACC_MODE  enAccMode;
    U16 wReserved;
}T_LOG_MAP,*pT_LOG_MAP;

typedef union __IDFY_LOG08
{
    U8 bPayload[512];
    struct
    {

        U64 qwHead_Info;
        U64 qwCapabilities;
        union
        {
            U64 qwCSS; //Current SATA Settings
            struct{
                U64 bCUR_SPD:3;//CURRENT NEGOTIATED SERIAL ATA SIGNAL SPEED field
                /*
                 *  Code Description
                    000b Reporting of current signalling speed is not supported
                    001b Current signalling speed is Gen1
                    010b Current signalling speed is Gen2
                    011b Current signalling speed is Gen3
                    100b..111b Reserved
                 * */
                U64 bNZBO:1;//NON-ZERO BUFFER OFFSETS ENABLED bit
                /**
                 * The IDENTIFY DEVICE data contains a copy of the NON-ZERO BUFFER OFFSETS ENABLED bit (see IDENTIFY
                    DEVICE data word 79 in table 45)
                 */

                U64 bA_DMASETUP:1;//DMA SETUP AUTO-ACTIVATION ENABLED bit
                U64 bDIPM:1;//DEVICE INITIATED POWER MANAGEMENT ENABLED bit
                U64 bIDD:1;//IN-ORDER DATA DELIVERY ENABLED bit
                U64 bHFC:1;//HARDWARE FEATURE CONTROL IS ENABLED bit
                U64 bSSP:1;//SOFTWARE SETTINGS PRESERVATION ENABLED bit
                U64 bAPT2S:1;//AUTOMATIC PARTIAL TO SLUMBER TRANSITIONS ENABLED bit
                U64 bDEVSLP:1;//DEVICE SLEEP ENABLED
                U64 twRFU:52;//triple word
                U64 bSet1:1;//should be 1
            }BIT;
        }unCSS;//Current SATA Settings
        U64 qwRO[61];
    }Type;
}T_IDFY_LOG08;



typedef struct __LOG_CTX{

    U32 dVirtualLbaStartAddr;//LOG 保存的虚拟LBA起始地址

    //For SATA Transport Statistics log use
    U32 dHWResetNum;
    U32 dASREventCnt;
    U32 dCRCErrorCnt;//00Bh Optional CRC errors within a host-to-device FIS

    //For Phy Event Counter log use
    U32 dCmdFailDue2ICRC;//001Command failed and ICRC error bit set to one in Error register
    U32 dPHYRDY2N;//009 Transitions from drive PHYRDY to drive PHYRDYn  --link off event??
    U32 dD2hFisDue2Comreset;//00A Register Device to Host FISes sent due to a COMRESET
    U32 dNoneCRCErrorCnt;//00Dh Optional Non-CRC errors within a host-to-device FIS

    U32 dUnUncorrectableErrorCnt;//Number of Reported Uncorrectable Errors
    /*
        The Number of Reported Uncorrectable Errors statistic is a counter that records the number of errors that are
        reported as an Uncorrectable Error (see 6.3.9). This statistic shall be incremented by one for each event.
        Uncorrectable errors that occur during background activity shall not be counted. Uncorrectable errors reported by
        reads to flagged uncorrectable (see 7.71.2) logical blocks should not be counted.
    */
    U32 dResetsCntOfCmd;//Number of Resets Between Command Acceptance and Command Completion

    U64 qwNumOfWriteCmd;//Number of write Commands
    U64 qwNumOfReadCmd;//Number of Read Commands

    U8  bPercentUsedCap;//Percentage Used Endurance Indicator:已写入的数据量占可写入的数据总量的百分比

    //温度控制
    U8  bCurrentTemp;/*当前温度，The Current Temperature statistic is the temperature measured
                        by the device at the time this log page is read.*/

    U8  bAverangeShortTemp; /*
                                The Average Short Term Temperature statistic is a value based on the most recent 144 temperature samples in a
                                24 hour period. 224小时内的144次采样的平均值，每小时采样6次，每10分钟一次
                            */
    U8  bAverangeLongTemp; /*
                                The Average Long Term Temperature statistic is a value based on the most recent 42 Average Short Term
                                Temperature values.
                            */
    U8  bHighestTemp; /*每10分钟记录一次存储最大值*/
    U8  bLowestTemp; /*每10分钟记录一次存储最小值*/

    U8  bHighestAverangeShortTemp;/*bAverangeShortTemp里面出现的最大值*/
    U8  bLowestAverangeShortTemp;/*bAverangeShortTemp里面出现的最小值*/

    U8  bHighestAverangeLongTemp;/*bAverangeLongTemp里面出现的最大值*/
    U8  bLowestAverangeLongTemp;/*bAverangeLongTemp里面出现的最小值*/
    U8  bMaxOpTemp;//设定最高工作温度
    U8  bMinOpTemp;//设定最低工作温度

    U32 dOvTempNums;//超过设定最高温度的次数
    U32 dUnDerTempNums;//低于设定最低温度的次数

    U32 dShortTempAcc;//每10分钟一次的温度累加值
    U32 dLongTempAcc;//每24小时一次的温度累加值

    U32 dE_RWTimer;
    GPL_E_TYPE enGplUpdateEvent;//LOG更新操作事件请求。
    BOOL bUpdateEn;
    U16  wErrLogIdx;//异常信息记录索引
    //smart
    BOOL bSmartEn;
    T_SMART_CTX tSmartCtx;//SMART 相关的数据定义

    //IDENTIFY DEVICE data log
    //大部分参数只读？
    U32 dAccableCap;//32bit 能达到2T，ACCESSIBLE CAPACITY field

    U32 dLgSecSize;//Logical Sector Size
    U32 dBuffSize;//Nominal Buffer Size


    BOOL bIsGplRunning; //写GPL LOG的状态机是否启动
    //BOOL bIsSmartRunning;//写Smart error log的状态机是否启动  --同步写不需要该标记
    BOOL bIsResetEveCnt;
    BOOL bIsSelfTestRunning;//写Smart self test 的状态机是否启动
    EN_ROUTE_MODE enSelfTestRouteMode;
    U16 wMaxCmdNums;
    U8 bMPCFlg;
    U64 qwLbaUDT;//LBD under test

    EN_TEST_MODE enSelfTestMode;

    //SECURITY PASSWORD
    U8 bPassword[32];
    U32 dSataReconectMode;
    //T_DEVICE_VAR tNoV_DevVar;//no Volatile device Var

}T_LOG_CTX,*pT_LOG_CTX;

typedef struct __DEVICE_VAR
{
    U32 dHeadVersion;//0x12348765
    U32 dMilliSecAcc;
    //U16 wReadSecCnt;
    //U16 wWriteSecCnt;
    ENUM_ATA_SECURITY_STATE eSecState;
    U8 bMPCFlg;
    ENUM_DCO_STATE eDCOState;
    T_SD tSDState;
    U8 bSDFMPVal;
    U8 bSDOCWOEVal;
    U16 wSanitizeProCnt;
    U16 wErrLogIdx;
    U32 dSataReconectMode;
}T_DEVICE_VAR;

typedef struct __SMART_OVERLAY
{
    U32  m_dTitle;//0x12348765
    U8 	 m_bSmartC7;//0表示ebable，1表示disable，2表示set 0
    U8 	 m_bFixTemperature;//0表示不固定温度，1表示固定温度，2表示设置最大温度为54度
    U8   m_bAjustTemperature;//固定温度值
    U8   m_bSmartC3;//0表示enable，1表示disable，2表示 NAND FLASH相关的参数都固定成0
    U8   m_bRFU[503];
    U8   bCheckSum;//前面511byte 累加值+1，忽略溢出
}T_SMART_OVERLAY;

#pragma  pack(1)  
//用于管理ext ERROR 结构
typedef struct _EXTCOMP_CDS  //command Data structure
{
    //If the command data structure represents a hardware reset
    //content of byte n shall be FFh, the content of bytes n+1 through n+7 are vendor specific,
    // and the content of bytesn+8 through n+11 shall contain the timestamp.
    U8 bControl;
    U8 bFeature;
    U8 bFeature_H;
    U8 bCount;
    
    U8 bCount_H;
    U8 bLba_low;
    U8 bLba_mid;
    U8 bLba_high;
    
    U8 bExpLba_low;
    U8 bExpLba_mid;
    U8 bExpLba_high;
    U8 bDevice;
    
    U8 bCmd;
    U8 bRFU;
    
    U32 dTimestamp; 
}T_EXTCOMP_CDS;

typedef struct{
    union{
        U16 wVal;
        struct
        {
            U16 IDFY:12;
            U16 SIZE:3;
            U16 VENDOR_F:1;
        }bits;
    }tID;
    U32 dCounter;
}T_COUNTX;

typedef struct {
    U32 dRFU;//记录条目数
    T_COUNTX tCTX[80];//6*80
    U8 bRFU[27];
    U8 bCheckSum;
}T_LOG_PHYECNT,*pT_LOG_PHYECNT;

typedef struct _EXTCOMP_EDS  //ERROR Data structure
{
    U8 bReserved;//set to 01
    U8 bError;
    U8 bCount;
    U8 bCount_H;
    
    U8 bLba_low;
    U8 bLba_mid;
    U8 bLba_high;
    U8 bExpLba_low;
    
    U8 bExpLba_mid;
    U8 bExpLba_high;
    U8 bStatus;
    U8 bDevice;
    
    U8 bExtInfo[19];
    U8 bState;
    U16 wLifeTimestamp; 
}T_EXTCOMP_EDS;


typedef struct _EXTCOMP_DS  //Data structure
{
    T_EXTCOMP_CDS tCmdDS[5];
    T_EXTCOMP_EDS tErrorDS;
    
}T_EXTCOMP_DS;

typedef union _EXTCOMP_ERROR
{
   U8 bBuff[512];
    
   struct{
       
       U8 bVersion;
       U8 bReserved;
       U16 wIndex; 
       
       T_EXTCOMP_DS tDS[MAX_EXT_SMART_ERROR_LOG_IDX];//error log data structure
       
       U16 wDevErrCnt;
       U8 dRFU[9];
       U8 bCheckSum;
   }m;//member 
}T_EXTCOMP_ERROR;

//用于管理ERROR 结构
typedef struct _COMP_CDS  //command Data structure
{
    //If the command data structure represents a hardware reset
    //content of byte n shall be FFh, the content of bytes n+1 through n+7 are vendor specific,
    // and the content of bytesn+8 through n+11 shall contain the timestamp.
    U8 bControl;
    U8 bFeature;
    U8 bCount;
    U8 bLba_low;
    
    U8 bLba_mid;
    U8 bLba_high;
    U8 bDevice;
    U8 bCmd;
    
    U32 dTimestamp; 
}T_COMP_CDS;

typedef struct _COMP_EDS  //ERROR Data structure
{
    U8 bReserved;//set to 01
    U8 bError;
    U8 bCount;
    U8 bLba_low;
    
    U8 bLba_mid;
    U8 bLba_high;
    U8 bStatus;
    U8 bDevice;
    
    U8 bExtInfo[19];
    U8 bState;
    
    U16 wLifeTimestamp; 
}T_COMP_EDS;

typedef struct _COMP_DS  //Data structure
{
    T_COMP_CDS tCmdDS[5];//60
    T_COMP_EDS tErrorDS;//30
    
}T_COMP_DS;

typedef union _COMP_ERROR
{
   U8 bBuff[512];
    
   struct{
       
       U8 bVersion;
       U8 bIndex;  
       T_COMP_DS tDS[MAX_SMART_ERROR_LOG_IDX];//error log data structure -450
       U16 wDevErrCnt;
       U8 dRFU[57];
       U8 bCheckSum;
   }m;//member 
}T_COMP_ERROR; 
typedef void UpdateIDFYLogRouter(CMD_STATUS eStatus,void *ptEvent);
#pragma pack(4)
/******************************变量声明******************************/
#if (SATA_LOG_ENABLE == 1)
extern T_LOG_CTX tSataLogCtx;
#endif

extern const T_LOG_MAP tLogMaps[];
/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif
    
    // *****************************************************************************
    // 函数名称: CMD_STATUS ATACmd_LogAddr2Lba(T_H2D *tpH2d,EN_RW_MODE enRwMode,EN_ACC_MODE enAccMode)
    // 函数功能: 子函数
    // 入口参数: tpH2d FSM buffer 地址
    //          enRwMode 读写操作模式
    //          enAccMode 访问方式 GPL/SL
    // 出口参数: NONE
    // 说    明:
    // *****************************************************************************
    CMD_STATUS ATACmd_LogAddr2Lba(T_H2D *tpH2d,EN_RW_MODE enRwMode,EN_ACC_MODE enAccMode);
    
#if (SATA_LOG_ENABLE == 1)    
    // *****************************************************************************
    // 函数名称: WriterGPLLogsRouter
    // 函数功能: 写所有的LOG 到FLASH 中
    // 入口参数: eStatus:任务完成状态
    //           ptTaskParam:前一个完成的任务相关参数
    // 出口参数: NONE
    // 说    明:
    // *****************************************************************************
    void WriterGPLLogsRouter(CMD_STATUS eStatus,void *ptTaskParam);

    // *****************************************************************************
    // 函数名称: UpdateIdfyLogPage08
    // 函数功能: 根据Idfy 更新 IDFY LOG PAGE 08
    // 入口参数: 待修改的缓冲区首地址
    // 出口参数: NONE
    // 说    明:
    // *****************************************************************************
    void UpdateIDFYLogPage08(CMD_STATUS eStatus,void *ptEvent);
    void UpdateIDFYLog(CMD_STATUS eStatus,void *ptEvent,U8 pageoff,UpdateIDFYLogRouter func);
    // *****************************************************************************
    // 函数名称: ReadGPLLogsRouter
    // 函数功能: FTL 初始化工作完成 事件触发读LOG 的
    // 入口参数: eStatus:任务完成状态
    //           ptTaskParam:前一个完成的任务相关参数
    // 出口参数: NONE
    // 说    明:
    // *****************************************************************************
    void ReadGPLLogsRouter(CMD_STATUS eStatus,void *ptTaskParam);

    // *****************************************************************************
    // 函数名称: Build_SmartTable
    // 函数功能: 根据 samrt 表格相关变量重建表格数据
    // 入口参数: pTable 待读写的表格缓冲区
    // 出口参数: NONE
    // 说    明:
    // *****************************************************************************
     void Build_SmartTable(UN_SMART_DATA_STRUCTURE* pUnSmart);
#endif

    // *****************************************************************************
    // 函数名称: ATACmd_ReadLogExt
    // 函数功能: 实现ATA命令 READ LOG EXT
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: READ LOG EXT - 2Fh, PIO Data-In
    // *****************************************************************************
    void ATACmd_ReadLogExt(void);    

    // *****************************************************************************
    // 函数名称: ATACmd_WriteLogExt
    // 函数功能: 实现ATA命令 WRITE LOG EXT
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: WRITE LOG EXT - 3Fh, PIO Data-Out
    // *****************************************************************************
    void ATACmd_WriteLogExt(void);

    // *****************************************************************************
    // 函数名称: ATACmd_WriteLogDmaExt
    // 函数功能: 实现ATA命令 WRITE LOG DMA EXT
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: WRITE LOG DMA EXT - 57h, DMA
    // *****************************************************************************
    void ATACmd_WriteLogDmaExt(void);

    // *****************************************************************************
    // 函数名称: ATACmd_ReadLogDmaExt
    // 函数功能: 实现ATA命令 READ LOG DMA EXT
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: READ LOG DMA EXT - 47h, DMA
    // *****************************************************************************
    void ATACmd_ReadLogDmaExt(void);
    

#ifdef __cplusplus
}
#endif
#endif/*_ATA_GPL_H_*/



