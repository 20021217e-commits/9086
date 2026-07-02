// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  dsata.h
//  文件标识：  
//  内容摘要：  sata ip 寄存器配置接口定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月16日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************

#ifndef _DSATA_H_
#define _DSATA_H_
/******************************包含头文件******************************/

#include "sata_typedef.h"

//
//extern T_SATA_VAR tSataVar;
//extern T_GLOBAL_VAR tGlobal;


#define  FAKE_ENGINE			(0U)	   //   模式Host 自发数据源
/*********************************YS9086 Test**********************************/

/********************************* YS9085 Test 的宏定义   END **********************/
#define TEST_DMA1_MAX_EN        (0U)         //DMA1 多次启动。
#define TEST_DMA1_MAXSctSize    (8U)      // DMA1 最大4K 传输， 大于4K需要 等Done 之后重新启动，
#define CRC_TEST			    (0U)      //
#define PRE_4K_WRITE_TEST		(0U)      //  4K 预先写测试
//#define H2D_DMASETUP_TEST			      //  H2D 中断卡DMA_SETUP 功能测试
#define DMA1_8K_SIZE		    (16U)      // DMA1 最大8K传输， 大于8K需要 等Done 之后重新启动，
#define PASS_IDX_VAL		    (0xFFFFU)      // PASS_IDX   大于 0xFFFF 配置 PASS_MIN = 0xFFFF - 7;
#define TAG_REPEAT_TEST
/*********************************Test 的宏定义   END **********************/
#ifndef MAX_CMD_NUM
#define MAX_CMD_NUM          (32U)
#endif

//一个PRD最大传输32M =8192 sectors
#define MAX_PRD_DCB_MAX      (65536U)

#define MILLSEC_OF_ONE_HOUR             ((U64)(1U*60U*60U*1000U))   //一小时的毫秒数
#define MILLSEC_OF_ONE_MIN              ((U64)(1U*60U*1000U))      //一分钟的毫秒数

#define LPM_PARTIAL          (0)
#define LPM_SLUMBER          (!LPM_PARTIAL)

#define LPM_ENABLE_BOTH      (0U)
#define LPM_ENABLE_SLUMBER   (1U)
#define LPM_ENABLE_PARTIAL   (2U)
#define LPM_DISABLE_BOTH     (3U)
#define LPM_PITIMER_VAL      (0xFFFF)

#define PHY_PARTIAL_EN              (1U)
#define PHY_SLUMBER_EN              (2U)
#define PHY_SLUMBER_S_EN            (4U)
#define PHY_OFFLINE_EN              (8U)
#if 0

//PSERR 
#define RXFOV_BIT   ((U32)1<<31) //RXFIFO Overflow Error
#define DIAG_T_BIT  ((U32)1<<24) // Transport State Transition Error
#define DIAG_S_BIT  ((U32)1<<23) //Link Sequence Error
#define DIAG_H_BIT  ((U32)1<<22) //Handshake Error
#define DIAG_C_BIT  ((U32)1<<21) //CRC Error
#define DIAG_D_BIT  ((U32)1<<20) //Disparity Error
#define DIAG_B_BIT  ((U32)1<<19) //10B to 8B Decode Error
#define DIAG_W_BIT  ((U32)1<<18) //Comm Wake
#define DIAG_I_BIT  ((U32)1<<17) // PHY Internal Error
#define ERR_E_BIT   ((U32)1<<11) // Internal Bus Error
#define ERR_P_BIT   ((U32)1<<10) //Protocol Error
#define ERR_C_BIT   ((U32)1<<9)  //PHY Not Ready Error
#define ERR_T_BIT   ((U32)1<<8)  //Non-recovered Transient Data Integrity Error
#define ERR_M_BIT   ((U32)1<<1)  //Recovered Communication Error
#define ERR_I_BIT   ((U32)1<<0)  //Recovered Data Integrity
#endif

//PCMDR
#define ICC(x)       ((U32)(x)<<28)//Interface Communication Control
#define APSTE_BIT    ((U32)1<<23)  //Automatic Partial to Slumber Transitions Enabled
#define TXBISTA_BIT  ((U32)1<<18)  //Transmit BIST Activate FIS
#define TXSDB_BIT    ((U32)1<<17)  //Transmit Set Device Bits FIS
#define TXREG_BIT    ((U32)1<<16)  //Transmit Register FIS
#define TXSAS_BIT    ((U32)1<<15)  //Transmit Status Auto-Send
#define DTABORT_BIT  ((U32)1<<14)  //Data Transfer Abort
#define DPP_BIT      ((U32)1<<13)  //Descriptor Processing Paused
#define CCS(x)       ((U32)(x)<<8) //Current Command Slot
#define FRXP_BIT     ((U32)1<<4)   //FIS Receive Paused
#define STD_BIT      ((U32)1<<1)   //Start Disable
#define ST_BIT       ((U32)1<<0)   //Start

//PISR
#define MBES_BIT  ((U32)1<<28)//Master Bus Error Status
#define IFS_BIT   ((U32)1<<26)//Interface Fatal Error Status
#define INFS_BIT  ((U32)1<<25)//Interface Non-fatal Error Status    
#define OFS_BIT   ((U32)1<<24)//Overflow Status    
#define LSCS_BIT  ((U32)1<<9) //Link State Change Status    
#define PCS_BIT   ((U32)1<<8) //Port COMRESET Status
#define TXOK_BIT  ((U32)1<<5) //Transmit Okay
#define DMATS_BIT ((U32)1<<4) //DMAT Status
#define DPS_BIT   ((U32)1<<3) //Descriptor Processed
#define UFS_BIT   ((U32)1<<2) //Unknown FIS Interrupt
#define FTXDS_BIT ((U32)1<<1) //FIS Transmit Done Interrupt
#define HDRS_BIT  ((U32)1<<0) //Host to Device Register FIS Interrupt

//Device DMA QUEUED data In command protocol
typedef enum _FIS_TYPE
{
    H2D_FIS=0,
    D2H_FIS,
    DMA_ACT_FIS,
    DMA_SETUP_FIS,
    DATA_FIS,
    BIST_FIS,
    PIO_SETUP_FIS,
    SDB_FIS,
    RFU1,
    RFU2,
    RFU3,
    VENDOR1,
    VENDOR2,
    RFU4,
}ENUM_FIS;//READ DMA QUEUED command protocol

/*FIS Structure Mapping buffer define,该结构体管理FIS公用缓冲区*/ 
typedef union _UN_FSM
{
    U32 adBuff[8];
    union _UN_FSM_FIS
    {
        T_H2D h2d;
        T_D2H d2h;
        T_PIO_SETUP pio_setup;
        T_BIST bist;
        T_SDB sdb;
    }FIS;
}UN_FSM,*pUN_FSM; //FIS Structure Mapping -SATA FIS公用缓冲区定义
#if 0
typedef union _UN_HWH2DFIS
{
    volatile U32 adBuff[8];
    struct
    {
        volatile T_H2D h2d;
        volatile U32   dLBA;
        volatile U16   wLBA_exp;
        volatile U8    bDir:1;
        volatile U8    b4KCmdFlag:1;  // Cmd
        volatile U8    bTag:6;
        volatile U8    bReserved2;   		//FW 做的标志
        volatile U32   dSecSize;
    }HWH2DFS;
}UN_HWH2DFIS,*pUN_HWH2DFIS; //HW回复D2H
#else
typedef union _UN_HWH2DFIS
{
    volatile U32 adBuff[8];
    struct
    {
        volatile T_H2D h2d;
        volatile U32   dLBA;
        volatile U16   wLBA_exp;
        volatile U8    bTag:5;
        volatile U8    bReserved0:1;
        volatile U8    bDir:1;
        volatile U8    bReserved1:1;
        volatile U8    b4KCmdFlag:1;  // Cmd
        volatile U8    bReserved2:7;   		//FW 做的标志
        volatile U32   dSecSize;
    }HWH2DFS;
}UN_HWH2DFIS,*pUN_HWH2DFIS; //HW回复D2H
#endif

typedef union _UN_HWD2HFS
{
    volatile U32 adBuff[8];
    struct
    {
        volatile T_D2H d2h;
        volatile U32   dLBA;
        volatile U16   wLBA_exp;
        volatile U16   wCount;
        volatile U32   dSecSize;
    }HWD2HFS;
}UN_HWD2HFS,*pUN_HWD2HFS; //HW回复D2H


typedef struct
{
	U8  bBuff[1024 * 8];
} T_ExBuff_8K;


typedef struct
{
    volatile U32 dDBA;//DBA: Data Base Address,地址为偶数
    volatile U32 dDBAU;//DBAU: Data Base Address Upper 32 bits
    volatile U32 dReserved;
    union _prd_entry_dw3
    {
        volatile U32 dAll;
        struct _prd_entry_dw3_bits
        {
            volatile U32 DBC: 25;//DBC: Data U8 Count
            volatile U32 Reserved: 5;
            volatile U32 W: 1;//W: Wrap
            volatile U32 I:1;//I: Interrupt on Compeletion
        } bits;
    } dw3;
} T_PRD_ENTRY;

typedef union _CH_DI
    {
        volatile U32 dAll;
        struct
        {
            volatile U32  TAG: 5;//TAG: FPDMA Command TAG
            volatile U32  Reserved1: 1;
            volatile U32  D: 1;//D: Transfer Direction
            volatile U32  P: 1;//P: PIO
            volatile U32  F: 1;//F: First-party DMA (FPDMA)
            volatile U32  A: 1;//A: Auto-Activate
            volatile U32 Reserved2: 5;
            volatile U32 PRDTL:17;//PRDTL: Physical Region Descriptor Table Length
        }bits;
}UN_CH_DI;//DW0 Description Information
        
typedef struct
{
    union 
    {
        volatile U32 dAll;
        struct _DSATA_cmd
        {
            volatile U32  TAG: 5;//TAG: FPDMA Command TAG
            volatile U32  Reserved1: 1;
            volatile U32  D: 1;//D: Transfer Direction
            volatile U32  P: 1;//P: PIO
            volatile U32  F: 1;//F: First-party DMA (FPDMA)
            volatile U32  A: 1;//A: Auto-Activate
            volatile U32  PM: 4;
            volatile U32 Reserved2: 1;
            volatile U32 PRDTL:17;//PRDTL: Physical Region Descriptor Table Length
        } cmd;
    } dw0;
    volatile U32 dDMATC;//DMATC: DMA Transfer Count
    volatile U32 dDMABO;//DMABO: DMA Buffer Offset
    volatile U32 dPRDTBA;// PRD Table Base Address
    volatile U32 dPRDTBAU;//PRD Table Base Address Upper 32 bits
    volatile U32 dReserved1;
    volatile U32 dReserved2;
    volatile U32 dReserved3;
}T_CMD_HEAD;

/******************************变量声明******************************/
//SATA IP Command Head Structure 1K-byte-aligned
extern T_CMD_HEAD tCmdHead[] ;
#ifdef  SC_H2DFIFO_EN
extern UN_HWH2DFIS arrunHWD2HFS[] ;//1K对齐
#else
extern UN_HWD2HFS arrunHWD2HFS[] ;//1K对齐
#endif
//This address is 128-byte-aligned
extern T_PRD_ENTRY tPrdTbls[] ;//一个CMD 对应一个prd
//FIS 公用缓冲区 32byte-aligned
extern UN_FSM unFsmBuff;
extern U8_C FIS_CODE[];

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

    /**
    * update the system clock
    *
    * @param  none
    * @return none
    *
    * @brief  Setup the microcontroller system.
    *         Initialize the System and update the SystemFrequency variable.
    */
    void DSATA_UpdateSystemClock (void);

    //TODO:SATA相关寄存器配置宏以及接口定义
    // *****************************************************************************
    // 函数名称: DSATA_Init
    // 函数功能: SATA初始化函数
    // 入口参数: NONE
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void DSATA_Init(U8);
    // *****************************************************************************
    // 函数名称: DSATA_StartupEnable
    // 函数功能: SATA启动
    // 入口参数: NONE
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void  DSATA_ExbuffAndCheckPoit_Ctrl(U8 bEN);

    void DSATA_StartupEnable(void);
    // *****************************************************************************
    // 函数名称: DSATA_TrigeSendFis
    // 函数功能: 上层调用该接口发送某一类型FIS，屏蔽掉硬件操作
    // 入口参数: ENUM_FIS FIS种类
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************
    void DSATA_TrigeSendFis(ENUM_FIS eFisType);   
    
    // *****************************************************************************
    // 函数名称: DSATA_Update_rPCMDR(U8 bBit,U8 bVal)
    // 函数功能: 由于PCMDR寄存器的特殊性需要特别方法置位
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  void DSATA_Update_rPCMDR(U8 bBit,U8 bVal);
//    {
//        //除去写1清零位的干扰
//        U32 mask = SG_DSATA->rPCMDR.dAll&(~DPP_BIT)&(~FRXP_BIT)&(~STD_BIT)&(~(1U<<bBit));
//        SG_DSATA->rPCMDR.dAll=mask |(bVal<<bBit);
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_TransferStart
    // 函数功能: 启动一次PRD entry的数据传输
    // 入口参数: 
    //           slot:槽编号0-31
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
extern void DSATA_TransferStart(U8 bSlot);// suke 非常重点

    // *****************************************************************************
    // 函数名称: DSATA_TransferStart
    // 函数功能: 启动一次PRD entry的数据传输
    // 入口参数: 
    //           slot:槽编号0-31
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
extern void DSATA_TransferMultiStart(U32 dFlag);// suke 非常重点


    // *****************************************************************************
    // 函数名称: DSATA_IsPrdTransferDone
    // 函数功能: 检查本次PRD传输是否结束
    // 入口参数: 
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    extern BOOL DSATA_IsPrdTransferDone(CMD_SLOT slot);
    //    {
    //        return (SG_DSATA->rPCMDR.bits.DPP == 1);
    //    }
    
     // *****************************************************************************
    // 函数名称: DSATA_GetCurrentCmdSlot
    // 函数功能: 获取当前进行数据传输的SLOT
    // 入口参数: 
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    extern U8 DSATA_GetCurrentCmdSlot();
    //    {
    //        return (U8)(SG_DSATA->rPCMDR.bits.CCS);
    //    }
    
    // *****************************************************************************
    // 函数名称: DSATA_BulidSDBFis
    // 函数功能: 配置D2H 公共部分
    // 入口参数: 
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void DSATA_BulidSDBFis(U8 status_Hi,U8 status_Lo,U8 bError,U8 I,U32 dProtocol_spec);

    // *****************************************************************************
    // 函数名称: DSATA_BuildD2HFis
    // 函数功能: 配置D2H
    // 入口参数:
    //           status:d2h的状态字段
    //           error：error字段
    //           I：中断
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void DSATA_BuildD2HFis(U8 bStatus,U8 bError,U8 I);

    // *****************************************************************************
    // 函数名称: DSATA_SendD2HFis
    // 函数功能: 配置D2H 公共部分
    // 入口参数: 
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void DSATA_SendD2HFis(U8 bStatus,U8 bError,U8 I);
    
    // *****************************************************************************
    // 函数名称: DSATA_BulidPioSetUpFis
    // 函数功能: 配置PIO_SETUP
    // 入口参数: 数据传输方向,本次传输数据大小
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void DSATA_BulidPioSetUpFis(U8 bDir,U8 bSize);
    
    // *****************************************************************************
    // 函数名称: DSATA_BuildPrd
    // 函数功能: 配置PRD表格
    // 入口参数: 
    //          I/W:中断位
    //            ptParam：param
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************
    void DSATA_BuildPrd(T_HI_TASK *ptTask,U8 bI);
    
    // *****************************************************************************
    // 函数名称: DSATA_SetCmdHead
    // 函数功能: cmd head配置
    // 入口参数: slot:命令槽
    //             di:Description Information
    //             dmact:dmaCnt:DMA Transfer Count
    //             dmabo:DMA Buffer Offset
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************
    void DSATA_SetCmdHead(CMD_SLOT bSlot,UN_CH_DI unDI,U32 dSector,U32 wDrq,U32 dDmatc);
        
    // *****************************************************************************
    // 函数名称: DSATA_ClearFsmBuff
    // 函数功能: 清楚FIS缓冲区
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************
    void DSATA_ClearFsmBuff(void);

    // *****************************************************************************
    // 函数名称: DSATA_LinkCheck
    // 函数功能: 检查链路层状态
    // 入口参数: NONE
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    BOOL DSATA_LinkCheck(void);    
    
    
    // *****************************************************************************
    // 函数名称: DSATA_GetPrdRemainItems
    // 函数功能: 使能自动发送FIS功能，对上层屏蔽硬件寄存器
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  U32 DSATA_GetPrdRemainItems(void);
//    {
//        return SG_DSATA->rPPICR;
//    }
    // *****************************************************************************
    // 函数名称: DSATA_IsAnySlotStarted
    // 函数功能: 查看是否已经启动数据传输
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern inline BOOL DSATA_IsAnySlotStarted(void);
//    {
//        return (SG_DSATA->rPDTR!= 0);
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_IsAnySlotStarted
    // 函数功能: 查看当前是否使能自动发送功能
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  BOOL DSATA_IsAutoSendEnabled(void);
//    {
//        return (SG_DSATA->rPCMDR.bits.TXSAS == 1);
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_Enable_Auto_Send
    // 函数功能: 查询bSlot任务的读写状态1：读，0：写
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern U8 DSATA_GetCmdDir(U8 bSlot);
//    {
//        //return (tCmdHead[bSlot].dw0.cmd.D);
//        return tSataVar.arrtCmdHead.dw0.cmd.D;
//    }

    // *****************************************************************************
    // 函数名称: DSATA_IsPioInMode
    // 函数功能: 查询当前的SLOT是否是PIOIN传输模式
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern U8 DSATA_IsPioInMode(U8 bSlot);
//    {
//        return ((tCmdHead[bSlot].dw0.cmd.P==1) && (DSATA_GetCmdDir(bSlot) == DIR_IN));
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_Enable_Auto_Send
    // 函数功能: 使能自动发送FIS功能，对上层屏蔽硬件寄存器
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  void DSATA_EnableAutoSend(void);
//    {
//        DSATA_Update_rPCMDR(15,1);
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_Disable_Auto_Send
    // 函数功能: 禁用自动发送FIS功能，对上层屏蔽硬件寄存器
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  void DSATA_DisableAutoSend(void);
//    {
//        DSATA_Update_rPCMDR(15,0);
//        SG_DSATA->rPIER.bits.FTXDE=0;//ENABLE FTXDS
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_Enable_RxFis
    // 函数功能: 重新使能接收
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern void DSATA_EnableRxFis(void);
//    {
//        //SG_DSATA->rPCMDR.bits.FRXP=1;
//        DSATA_Update_rPCMDR(4,1);
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_Enable_RxFis
    // 函数功能: 重新使能接收
    // 入口参数: bSlot命令号
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  void DSATA_Wait4PrdComplete(U8 bSlot);
//    {
//        if(((SG_DSATA->rPDTR&(1U<<bSlot)) == 1))
//        {
//            //已经启动数据传输
//            while(SG_DSATA->rPCMDR.bits.DPP != 1);//等待上一个PRD传输结束
//        }
//
//    }
    // *****************************************************************************
    // 函数名称: DSATA_Disable_ST
    // 函数功能: 清除ST位，对上层屏蔽硬件寄存器
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern void DSATA_DisableST(void);
//    {
//        SG_DSATA->rPCMDR.bits.ST=0;
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_IsSlotStarted
    // 函数功能: 查询该SLOT是否已经启动
    // 入口参数: bSlot:slot 编号
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
    extern BOOL DSATA_IsSlotStarted(U8 bSlot);
    //    {
    //        return ((SG_DSATA->rPDTR&(1U<<bSlot)) != 0);
    //    }

    // *****************************************************************************
    // 函数名称: DSATA_EnableDPP
    // 函数功能: 设置PRD table 中的Ibit位，用以产生DPP中断
    // 入口参数: bSlot:
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    void DSATA_EnableDPP(U8 bSlot);

    // *****************************************************************************
    // 函数名称: DSATA_PRE_4K_ExpHandler
    // 函数功能: 已经预收了4K 命令产生了异常
    // 入口参数: bType:异常类型
    //
    //
    //
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    void DSATA_PRE_4K_ExpHandler(void);

    // *****************************************************************************
    // 函数名称: DSATA_ExceptionHandler
    // 函数功能: SATA端产生的异常处理
    // 入口参数: bType:异常类型
    //
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    BOOL  DSATA_ExceptionHandler(U8 bType);

    // *****************************************************************************
    // 函数名称: DSATA_Enable_INT
    // 函数功能: 使能中断
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
    extern void DSATA_Enable_INT(void);
//    {
//        SG_DSATA->rGCR.bits.IE=1;
//    }
    
    // *****************************************************************************
    // 函数名称: DSATA_Disable_INT
    // 函数功能: 禁用中断
    // 入口参数: 
    // 出口参数:  
    // 说    明: 
    // *****************************************************************************    
extern  void DSATA_Disable_INT(void);
//    {
//        SG_DSATA->rGCR.bits.IE=0;
//    }
    // *****************************************************************************
    // 函数名称: DSATA_IRQHandler
    // 函数功能: SATA中断服务函数
    // 入口参数: NONE
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    void DSATA_IRQHandler(void);

    void DSATA_G3_TaskRouter(void);
#ifdef   SC_CRC_PASS_EN
    // *****************************************************************************
    // 函数名称: DSATA_CRCPASS_Index
    // 函数功能: 返回 CRC_PASS_Index
    // 入口参数:
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    U32 DSATA_CRCPASS_Index(void);

    // *****************************************************************************
    // 函数名称: DSATA_CRCPASS_MIN
    // 函数功能:
    // 入口参数:
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    void DSATA_CRCPASS_MIN(U32 dCrcPaccMin);

    // *****************************************************************************
    // 函数名称: DSATA_Check_CRCFail_Flg
    // 函数功能: Check  data CRC Faill标志
    // 入口参数:
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    U8 DSATA_Check_CRCFail_Flg(void);

    // *****************************************************************************
    // 函数名称:  DSATA_Clean_CRCPASS_INDEX
    // 函数功能:  Clean CRC_FAIl flg 和 CRC_PASS_Index
    // 入口参数:
    // 出口参数:
    // 说    明:
    // *****************************************************************************
    void DSATA_Clean_CRCPASS_INDEX(void);
#endif  //   SC_CRC_PASS_EN

#ifdef  SC_WP_4K_PRE
// *****************************************************************************
// 函数名称:  DSATA_PRE_Trig_Tag
// 函数功能:  返回 预先触发4K 写的Tag 号
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U8 DSATA_PRE_Trig_Tag(void);

// *****************************************************************************
// 函数名称:  DSATA_Clean_TAG
// 函数功能:  Clean 4K 的标记和Tag 号
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Clean_TAG(void);
#endif  //SC_WP_4K_PRE


#ifdef __cplusplus
}
#endif
#endif/*_DSATA_H_*/


