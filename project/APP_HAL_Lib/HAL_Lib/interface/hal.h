// *****************************************************************************
//  版权所有  (C) 2017,  硅格半导体有限公司
//
//  文件名称：  hal.h
//  文件标识：
//  内容摘要：简要说明flash_driver对外接口函数功能
//  其它说明：详细说明参考《SG9081 HAL接口说明文档.pdf》
//  当前版本：  SDKv2.01.01
//  项    目：  SG9081
//  作    者：
//  完成日期：  2018年02月09日
//
//  修改记录:    
//  修改日期：
//  版 本 号：
//  修 改 人：
//  修改内容：
// *****************************************************************************
#ifndef __HAL_H__
#define __HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_config.h"
#include "data_type.h"
#include "flash_op_cmd.h"
#include "sg9081.h"
#include "Hal_ldpc.h"
#include "Debug.h"
#ifndef  __SW_PMCP__
#include "hal_pmcp.h"
#else
#endif

#ifndef offsetof
#define offsetof(s,m)   ((U32)&(((s *)0)->m))
#endif
#ifndef SECTOR_SIZE
#define SECTOR_SIZE     (512)
#endif

#ifdef __HAL_INTERFACE__

#define HAL_MAX_CHAN_NUM    (4)

typedef struct  _LLR_LOOKUP
{
	union
	{
		__IO unsigned long dAll;
		struct
		{
			__IO unsigned long MAP_0:5 ;/**/
			__IO unsigned long MAP_1:5 ;/**/
			__IO unsigned long MAP_2:5 ;/**/
			__IO unsigned long MAP_3:5 ;/**/
			__IO unsigned long MAP_4:5 ;/**/
			__IO unsigned long MAP_5:5 ;/**/
			__IO unsigned long Reserved:2 ;/*����*/
		}bits;
	}rLDEC_SB_MAP0;/*<Offset:0x14> LDEC_SB_MAP0*/

	union
	{
		__IO unsigned long dAll;
		struct
		{
			__IO unsigned long MAP_6:5 ;/**/
			__IO unsigned long MAP_7:5 ;/**/
			__IO unsigned long MAP_8:5 ;/**/
			__IO unsigned long MAP_9:5 ;/**/
			__IO unsigned long MAP_10:5 ;/**/
			__IO unsigned long MAP_11:5 ;/**/
			__IO unsigned long Reserved:2 ;/*����*/
		}bits;
	}rLDEC_SB_MAP1;/*<Offset:0x18> LDEC_SB_MAP1*/
	union
	{
		__IO unsigned long dAll;
		struct
		{
			__IO unsigned long MAP_12:5 ;/**/
			__IO unsigned long MAP_13:5 ;/**/
			__IO unsigned long MAP_14:5 ;/**/
			__IO unsigned long MAP_15:5 ;/**/
			__IO unsigned long MAP_16:5 ;/**/
			__IO unsigned long MAP_17:5 ;/**/
			__IO unsigned long Reserved:2 ;/*����*/
		}bits;
	}rLDEC_SB_MAP2;/*<Offset:0x1C> LDEC_SB_MAP2*/
	union
	{
		__IO unsigned long dAll;
		struct
		{
			__IO unsigned long MAP_18:5 ;/**/
			__IO unsigned long MAP_19:5 ;/**/
			__IO unsigned long MAP_20:5 ;/**/
			__IO unsigned long MAP_21:5 ;/**/
			__IO unsigned long MAP_22:5 ;/**/
			__IO unsigned long MAP_23:5 ;/**/
			__IO unsigned long Reserved:2 ;/*����*/
		}bits;
	}rLDEC_SB_MAP3;/*<Offset:0x20> LDEC_SB_MAP3*/
	union
	{
		__IO unsigned long dAll;
		struct
		{
			__IO unsigned long MAP_24:5 ;/**/
			__IO unsigned long MAP_25:5 ;/**/
			__IO unsigned long MAP_26:5 ;/**/
			__IO unsigned long MAP_27:5 ;/**/
			__IO unsigned long MAP_28:5 ;/**/
			__IO unsigned long MAP_29:5 ;/**/
			__IO unsigned long Reserved:2 ;/*����*/
		}bits;
	}rLDEC_SB_MAP4;/*<Offset:0x24> LDEC_SB_MAP4*/
	union
	{
		__IO unsigned long dAll;
		struct
		{
			__IO unsigned long MAP_30:5 ;/**/
			__IO unsigned long MAP_31:5 ;/**/
			__IO unsigned long Reserved:22 ;/*����*/
		}bits;
	}rLDEC_SB_MAP5;/*<Offset:0x28> LDEC_SB_MAP5*/
}T_LLR_LOOKUP;

typedef struct
{
    /***********FC寄存器设置**********************/
    U32 FC_dNfCfg;           //HAL赋值给寄存器: rNF_CFG
    U32 FC_dSpopCtrl;        //HAL赋值给寄存器: rFIO_SPOP_CTRL
    U32 FC_dPrngCtrl;	       //----hal未使用!
    U32 FC_dDDRTimingCfg;    //HAL赋值给寄存器: rDDR_TIMING_CFG
    U32 FC_dFCCqSetRdCfgDqs; //HAL赋值给寄存器: rCQ_SET_RD_CFG_DQS
    U32 FC_dCQCtrl;          //HAL赋值给寄存器: rCQ_CTRL
    U32 FC_dCQIntInfo;         //----hal未使用!
    U32 FC_dBadColCtrl;      //HAL赋值给寄存器: rBAD_COLUMN_CTRL
    U32 FC_dBadColInfoBAddr; //HAL赋值给寄存器: rBADCOL_INFO_BADDR
    U32 FC_dAleCleTimingCfg; //HAL赋值给寄存器: rALE_CLE_TIM_CFG
    U32 FC_dCheckStsThrod;   //HAL赋值给寄存器: rCHK_ST_RB_CNT
    U32 FC_dCheckCqCtrl;     //HAL赋值给寄存器: rCHK_CQ_CTRL
    U32 FC_dCQHoldCtrl;      //HAL赋值给寄存器: rCQ_HOLD_CTRL
    U32 FC_dRandSeed;        //HAL赋值给寄存器: rRND_SEED
    U32 FC_dRes[2];          
    /**********ADM寄存器设置*************************/
    U32  ADM_dEccFuncSel;       //----hal未使用，ldpc模式用rECC_CTRL配置，bch模式用rECC_CFG配置
    U32  ADM_dDataErrStopDma2En;//----hal未使用，此功能受fdl接口控制，不用hal实现
    U32  ADM_dBufSize;          //HAL赋值给寄存器: rBUF_SIZE
    U32  ADM_dDma1LenCfg;    //SG_DMA1->rLEN
    U32  ADM_dDma2EnCtrl;       //----hal未使用，dma2en hal自己实现初始化，不需要mptool配置
    U32  ADM_dDma2LenCfg;    //HAL赋值给寄存器: rLEN_CFG
    U32  ADM_dDma2EccCfg;    //HAL赋值给寄存器: rECC_CFG
    U32  ADM_dDma2EccCtrl;   //HAL赋值给寄存器: rECC_CTRL
    U32  ADM_dDma2RndCtrl;   //HAL赋值给寄存器: rRND_CTRL
    U32  ADM_dDma2RaidCtrl;  //HAL赋值给寄存器: rRAID_CTRL
    U32  ADM_gParityLen;     	//小数据的Parity长度
    U08  ADM_gDataPuncLen;	 //数据的parity change长度
    U08  ADM_gTabPuncLen;	 //表格的parity change长度
    U08  ADM_bRes[2];
    U32  ADM_dRes[2];
    /**********LDPC寄存器设置*************************/
    U32  LDPC_dLdecCtrl;		//----hal未使用
    U32  LDPC_dLencCRSet;		//----hal未使用	
    U32  LDPC_dLencMainCtrl;	//----hal未使用
    T_LLR_LOOKUP  LDPC_tLLRLookup[3];//----hal未使用
    U32  LDPC_dMemMtxA; //矩阵A地址
    U32  LDPC_dMemMtxB; //矩阵B地址
    U32  LDPC_dMemMtxP1; //矩阵P1地址
    U32  LDPC_dMemMtxP2; //矩阵P2地址
    U32  LDPC_dRes[2];

    /**********CPM寄存器设置*************************/
    U8  CPM_bTrx2ClkPeriod[4]; //----hal未使用，FTL需自己初始化频率相关参数
    U32 CPM_dSysClkPeriod;     //----hal未使用，FTL需初始化频率相关参数
    U32 CPM_dLencClkPeriod;    //----hal未使用，FTL需初始化频率相关参数
    U32 CPM_dLdecClkPeriod;    //----hal未使用，FTL需初始化频率相关参数
    U8  CPM_bTrx2ClkSourSel[4];//----hal未使用，FTL需初始化频率相关参数
    U32 CPM_dSysClkSourSel;    //----hal未使用，FTL需初始化频率相关参数
    U32 CPM_dLencClkSourSel;   //----hal未使用，FTL需初始化频率相关参数
    U32 CPM_dLdecClkSourSel;   //----hal未使用，FTL需初始化频率相关参数
    U32 CPM_dDQSDelayCtrl;     //HAL赋值给寄存器: rDQS0_DELAY_CTRL  0~3
    U32 CPM_dIntCtrl;          //----hal未使用
    U32 CPM_dRes[4];
    /**********MCU寄存器设置*************************/
    U32 MCU_dG0VectorOffsAddr; //----hal未使用
    U32 MCU_dG3VectorOffsAddr; //----hal未使用
    U32 MCU_dLEDCtrl;          //----hal未使用，需调用接口HAL_LED_Config()使用
    U32 MCU_dLED0PeriodCtrl;   //----hal未使用，需调用接口HAL_LED_Config()使用
    U32 MCU_dLED1PeriodCtrl;   //----hal未使用，需调用接口HAL_LED_Config()使用
    U32 MCU_dRes[4];
    /**********PAD寄存器设置*************************/
    U32 PAD_dSSelCtrl0to3;  //HAL赋值给寄存器: rPAD_SSEL_CTRL0~3
    U32 PAD_dSSelCtrl4;     //HAL赋值给寄存器: rPAD_SSEL_CTRL4
    U32 PAD_dPECtrl0to3;    //HAL赋值给寄存器: rPAD_PE_CTRL0~3
    U32 PAD_dPECtrl4;       //HAL赋值给寄存器: rPAD_PE_CTRL4
    U32 PAD_dPECtrl5;       //HAL赋值给寄存器: rPAD_PE_CTRL5
    U32 PAD_dPSCtrl0to3;    //HAL赋值给寄存器: rPAD_PS_CTRL0~3
    U32 PAD_dPSCtrl4;       //HAL赋值给寄存器: rPAD_PS_CTRL4
    U32 PAD_dPSCtrl5;       //HAL赋值给寄存器: rPAD_PS_CTRL5
    U32 PAD_dFuncCtrl0to3;  //----hal未使用
    U32 PAD_dFuncCtrl4;     //HAL赋值给寄存器: rPAD_FUNC_CTRL4

    //U32 PAD_dRes[1];
    U08  PAD_res[2];
    U08  PAD_FLASH_ODT;     //FLASH ODT 配置
    U08  PAD_SEL_DIFF;      //HAL赋值给寄存器: rPAD_MISC_CTRL.F0_SEL_DIFF

    U32 PAD_dPhyXcfgD0;		//HAL赋值给寄存器: rPHY_XCFGD0
    U32 PAD_dPhyXcfgD1;		//HAL赋值给寄存器: rPHY_XCFGD1
    U32 PAD_dPhyXcfgD2;		//HAL赋值给寄存器: rPHY_XCFGD2
    U32 PAD_dPhyXcfgD3;		//HAL赋值给寄存器: rPHY_XCFGD3
    U32 PAD_dPhyXcfgA0;		//HAL赋值给寄存器: rPHY_XCFGA0
    U32 PAD_dPhyXcfgA1;		//HAL赋值给寄存器: rPHY_XCFGA1
    U32 PAD_dPhyXcfgA2;		//HAL赋值给寄存器: rPHY_XCFGA2
    U32 PAD_dPhyXcfgA3;		//HAL赋值给寄存器: rPHY_XCFGA3
    U32 PAD_dTSEL_CTRL;		//HAL赋值给寄存器: rPAD_TSEL_CTRL
} T_REG_CFG;

//HAL相关参数结构体
typedef struct _T_HAL_PARAM
{
    U8 bRev[20];//保留扩展字节
}T_HAL_PARAM;



extern U8_C HAL_LDPCModeList[6];
extern U16_C HAL_LDPCParityLen[6];


// *****************************************************************************
// 函数功能: Hal初始化函数
// 入口参数: 
//      ptHwReg:硬件寄存器初始化，指针类型，该值应该在UDP中存在，HAL中只保存指针
// 出口参数: 无
// 说   明: 
// *****************************************************************************
void HAL_Init(T_REG_CFG *ptHwReg);

#endif


#if (FUNC_ENABLE == __HAL_MCU_DEFINE__)

typedef enum
{
    MCU_CE_MODE             = 0,
    MCU_CMD_MODE            = 1,
    MCU_ADDR_MODE           = 2,
    MCU_PULLUP_CE_RD_MODE   = 3
}MCU_CFG_MODE;

typedef enum
{
    MCU_CHK_STS0            = 0,
	MCU_CHK_STS1			= 1,
}MCU_CHK_ST_MODE;

// *****************************************************************************
// 函数功能: 设置全局中断使能状态
// 入口参数: 
//      bNewState:新状态
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_MCUSetGieState(U8 bNewState);

// *****************************************************************************
// 函数功能: 用MCU方式check io status
// 入口参数: 
//      bChan:通道
//		bChip:片选
//      bMode:类似CQ check status
//      bVal:设置的值，根据不同的bMode有不同含义
// 返回值：无
// 说   明: 无
// *****************************************************************************
volatile U8 HAL_MCUCheckStatus(U8 bChan, U8 bChip);

// *****************************************************************************
// 函数功能: 用MCU方式配置CE/CMD/ADDR
// 入口参数: 
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_MCU_SetCe(U8 bChan, U8 bChip, U8 bVal);
inline void HAL_MCU_SetRd(U8 bChan);
inline void HAL_MCU_SendCmd(U8 bChan, U8 bCmd);
inline void HAL_MCU_SendOneAddr(U8 bChan, U08 bAddr);
inline void HAL_MCU_SendTwoAddr(U8 bChan, U16 wAddr);
inline void HAL_MCU_SendThreeAddr(U8 bChan, U32 dAddr);



#endif

#if (FUNC_ENABLE == __HAL_ADM_DEFINE__)

#define MC_MAX_IDLE_SPACE                        ((U16)256)
typedef enum
{
    DMA1_WR_TO_BUFF = 1,
    DMA1_RD_TO_HOST = 0

}DMA1_Dir;

typedef enum
{
    DMA2_WR_TO_FLASH= 0,
    DMA2_RD_TO_BUFF = 1
}DMA2_Dir;

typedef enum
{
    JOINT_NONE = 0,
    JOINT_12 = 1,
    JOINT_22 = 2,
    JOINT_21 = 3,
}Joint_Mode;

typedef enum
{
    RND_ALG_SUMSUNG_A = 0,
    RND_ALG_SUMSUNG_B = 1,
    RND_ALG_TOSHIBA_A = 2, //东芝2D + 3D_1
    RND_ALG_TOSHIBA_B = 3, //东芝3D_2
}Rnd_Alg_Type;

typedef enum
{
    CELL_SLC = 0,
    CELL_MLC = 1,
    CELL_TLC = 2,
}Seed_Type;

#define HAL_DMA2_FAIL_INT_EN        (1<<6)
#define HAL_DMA2_ALL_ONE_INT_EN     (1<<4)
#define HAL_DMA2_ALL_ZERO_INT_EN    (1<<3)
#define HAL_DMA2_UD_EN              (1<<2)

#define LUN_NULL        (0xFF)
#define PMCP0 0
#define PMCP1 1

//ECC_FUNC_SEL
#define  ECC_MODE_LDPC (0)
#define  ECC_MODE_BCH  (1)
#define  ECC_MODE_NONE (2)

// *****************************************************************************
// 函数功能: MCU 设置DMA 基地址
// 入口参数: 
//      bChan:DMA通道编号，FF表示DMA1
//      dBuffAddr:Buffer 基地址（Byte为单位）
// 返回值：无
// 说   明: 联动设置联动基地址，非联动设置非联动基地址
// *****************************************************************************
inline void HAL_ADMSetBaseAddr(U8 bChan, U32 dBuffAddr);

// *****************************************************************************
// 函数功能: MCU 获取DMA 基地址
// 入口参数: 
//      bChan:DMA通道编号，FF表示DMA1
//      dBuffAddr:Buffer 基地址（Byte为单位）
// 返回值：无
// 说   明: 获取联动或非联动基地址
// *****************************************************************************

inline U32 HAL_ADMGetBaseAddr(U8 bChan);


// *****************************************************************************
// 函数功能: MCU 设置DMA2联动Buffer Size
// 入口参数: 
//      wSect:Buffer 大小（扇区为单位）
// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_ADMSetBufferSize(U16 wSect);

// *****************************************************************************
// 函数功能: MCU 设置DMA2的DMA长度
// 入口参数: 
//      bChan:通道编号
//      wLen:DMA长度，Byte为单位
// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_ADMSetDMALen(U8 bChan, U16 wLen);

// *****************************************************************************
// 函数功能: 获取DMA2的DMA长度
// 入口参数:  
//      bChan:通道编号
// 返回值：DMA长度，Byte为单位
// 说   明: 
// *****************************************************************************
inline U16 HAL_ADMGetDMALen(U8 bChan);

// *****************************************************************************
// 函数功能: 设置通道User Data的基地址
// 入口参数:  
//      bChan:通道编号
//      dBuffAddr:基地址
// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_ADMSetUdBaseAddr(U8 bChan, U32 dBuffAddr);

// *****************************************************************************
// 函数功能: 获取通道User Data的基地址
// 入口参数:  
//      bChan:通道编号
// 返回值：基地址
// 说   明: 
// *****************************************************************************
inline U32 HAL_ADMGetUdBaseAddr(U8 bChan);

// *****************************************************************************
// 函数功能: 设置通道User Data的长度
// 入口参数:  
//      bChan:通道编号
// 返回值：基地址
// 说   明: 
// *****************************************************************************
inline void HAL_ADMSetUdLen(U8 bChan, U8 bLen);

// *****************************************************************************
// 函数功能: 获取通道User Data的长度
// 入口参数:  
//      bChan:通道编号
// 返回值：基地址
// 说   明: 
// *****************************************************************************
inline U8 HAL_ADMGetUdLen(U8 bChan);
inline void HAL_ADMSetUdEnable(U8 bChan, U8 bRndNewState);

// *****************************************************************************
// 函数功能: 获取通道HW联动模式
// 入口参数:  
//      bChan:通道编号
// 返回值：联动模式
// 说   明: 
// *****************************************************************************
inline U8 HAL_ADMGetJointMode(U8 bChan);

// *****************************************************************************
// 函数功能: 设置通道HW联动模式
// 入口参数:  
//      bChan:通道编号
// 返回值：联动模式
// 说   明: 
// *****************************************************************************

inline void HAL_ADMSetJointMode(U8 bChan,U8 bJointMode);


// *****************************************************************************
// 函数功能: 获取DMA1当前的Index
// 入口参数:  
//      	无
// 返回值：DMA1当前的Index
// 说   明: 
// *****************************************************************************
inline U32 HAL_ADMGetDMA1Index(void);

// *****************************************************************************
// 函数功能: 获取指定通道DMA2当前的Index
// 入口参数:  
//      	bChanPhy:物理通道号
// 返回值：定通道DMA2当前的Index
// 说   明: 
// *****************************************************************************
U32 HAL_ADMGetDMA2Index(U08 bChanPhy);


// *****************************************************************************
// 函数功能: 清除DMA1当前的Index
// 入口参数:  
//      	无
// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_ADMClearDMA1Index(void);

// *****************************************************************************
// 函数功能: 清除指定通道DMA2当前的Index
// 入口参数:  
//      	bChanPhy:物理通道号
// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_ADMClearDMA2Index(U08 bChanPhy);



// *****************************************************************************
// 函数功能: MCU 启动DMA1
// 入口参数: 
//      dir:方向
//      wBuffIdx:数据所在的RAM地址（扇区为单位）
//      wTotalSect:传输数据的总大小（扇区为单位）
//      joint:联动模式
// 返回值：无
// 说   明: 只是启动DMA1，不会等待DMA1完成，后续需要等待DMA1 完成
// *****************************************************************************
void HAL_ADMStartDMA1(U32 dBuffAddr, U32 dBuffIdx, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint);


void HAL_ADMStartDMA1WithoutIndex(U32 dBuffAddr, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint);

// *****************************************************************************
// 函数功能: MC 设置DMA1 Buffer Index
// 入口参数: 
//      wBuffIdx:数据所在的RAM地址（扇区为单位）
// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_MCSetDMA1BuffIndex(U32 dBuffIdx);

// *****************************************************************************
// 函数功能: MC 启动DMA1
// 入口参数: 
//      bChan:通道编号

//      wBuffIdx:数据所在的RAM地址（扇区为单位）
//      wTotalSect:传输数据的总大小（扇区为单位）
//      dir:方向
//      joint:联动模式
// 返回值：无
// 说   明: 最后会增加一条等待DMA1 done的命令
// *****************************************************************************
void HAL_MCStartDMA1(U32 dBuffAddr, U32 dBuffIdx, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint);


// *****************************************************************************
// 函数功能: MC 启动DMA1,但是不设置Buff Addr
// 入口参数: 
//      wBuffIdx:数据所在的RAM地址（扇区为单位）
//      wTotalSect:传输数据的总大小（扇区为单位）
//      dir:方向
//      joint:联动模式
// 返回值：无
// 说   明: 最后会增加一条等待DMA1 done的命令
// *****************************************************************************
void HAL_MCStartDMA1WithoutBuffAddr(U32 dBuffIdx, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint);

// *****************************************************************************
// 函数功能: 设置DMA1BuffAddr
// 入口参数: 
//      dBuffAddr:数据所在的RAM起始地址（扇区为单位）
// 返回值：无
// 说   明: 最后会增加一条等待DMA1 done的命令
// *****************************************************************************
void HAL_MCADMSetBaseAddr(U32 dBuffAddr);

void HAL_MCStartDMA1Continue(U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint);
// *****************************************************************************
// 函数功能: MC 启动DMA1,但是不设置Buffer Index
// 入口参数: 
//      bChan:通道编号
//      wTotalSect:传输数据的总大小（扇区为单位）
//      dir:方向
//      joint:联动模式
// 返回值：无
// 说   明: 最后会增加一条等待DMA1 done的命令
// *****************************************************************************
void HAL_MCStartDMA1WithoutBuffIndex(U32 dBuffAddr, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint);

// *****************************************************************************
// 函数功能: MC 产生一个中断
// 入口参数:
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_MCGenerateInt();

inline void HAL_MCWaitDMA1Done();

inline U08 HAL_MCGetStatus();

// *****************************************************************************
// 函数功能: MCU 启动DMA2
// 入口参数: 
//      bChan:通道编号
//      dir:方向
//      wBuffIdx:数据所在的RAM地址（扇区为单位）
//      bFlashIdx:数据所在的Flash位置（扇区为单位）
//      bTotalSect:传输数据的总大小（扇区为单位）
//      joint:联动模式
// 返回值：无
// 说   明: 只是启动DMA2，不会等待DMA2完成，后续需要等待DMA2 完成
// *****************************************************************************
void HAL_ADMStartDMA2(U8 bChan,U32 dBuffAddr,U32 dDmaInfoAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint);

// *****************************************************************************
// 函数功能: 等待DMA 完成
// 入口参数: 
//      bChan:通道编号，如果是0xFF则表示DMA1，其余表示DMA2
// 返回值：无
// 说   明: 无
// *****************************************************************************
void HAL_ADMWaitDMADone(U8 bChan);

// *****************************************************************************
// 函数功能: 设置Rnd使能
// 入口参数: 
//      bChan:通道编号
//      bFunctionNewState:Rnd模块功能
//      bRndNewState:rnd效果，FUNC_DISABLE不随机，FUNC_ENABLE随机
// 返回值：无
// 说   明: 
//      1、使用RAID时，bFunctionNewState必须为FUNC_DISABLE,其他情况必须为FUNC_ENABLE
//      2、bRndNewState 是否数据随机，在bFunctionNewState==FUNC_ENABLE时有效
// *****************************************************************************
inline void HAL_ADMSetRndEnable(U8 bChan, U8 bFunctionNewState, U8 bRndNewState);

// *****************************************************************************
// 函数功能: 设置Mask功能
// 入口参数: 
//      bChan:通道编号
//      dMask0:mask0
//      dMask1:dMask1
// 返回值：无
// 说   明: 
//      1、设置MASK bit的值
//      2、dMask0对应[31:0],dMask对应[63:32]
// *****************************************************************************
inline void HAL_ADMSetMask(U8 bChan,U32 dMask0, U32 dMask1);

// *****************************************************************************
// 函数功能: 设置DMA2 Enable 寄存器
// 入口参数: 
//      bChan:通道编号
//      wNewState:新的状态
// 返回值：无
// 说   明:无
// *****************************************************************************
inline void HAL_ADMSetDma2EnabledState(U8 bChan,U16 wNewState);

// *****************************************************************************
// 函数功能: 获取DMA2 Enable 寄存器
// 入口参数: 
//      bChan:通道编号
// 返回值：DMA2 enable状态
// 说   明:无
// *****************************************************************************
inline U16 HAL_ADMGetDma2EnabledState(U8 bChan);

// *****************************************************************************
// 函数功能: 设置DMA2 Stop enable 寄存器
// 入口参数: 
//      bChan:通道编号
//      wNewState:新的状态
// 返回值：无
// 说   明:无
// *****************************************************************************
inline void HAL_ADMSetDma2StopEnableState(U8 bChan, U8 bNewState);

// *****************************************************************************
// 函数功能: 获取DMA2 Stop enable寄存器
// 入口参数: 
//      bChan:通道编号
// 返回值：DMA2 enable状态
// 说   明:无
// *****************************************************************************
inline U8 HAL_ADMGetDma2StopEnableState(U8 bChan);

// *****************************************************************************
// 函数功能: 传输数据
// 入口参数: 
//      dBuffAddr:ram地址
//		wSec:扇区数
//		bDir:方向 0:host写 1:host读
// 返回值:无
// 说   明:无
// *****************************************************************************

// *****************************************************************************
// 函数功能: MCU 读Flash的一个DMA数据
// 入口参数: 

// 返回值：无
// 说   明: 启动DMA2，等待DMA2完成
// *****************************************************************************
inline void HAL_MCUReadOneDma(U8 bChan,U16 wBufIndex, U8 bFlashIdx, U16 wDmaSect, U32 dDmaBuffAddr, U32 dDmaInfoAddr);

// *****************************************************************************
// 函数功能:读Flash MCU发送命令
// 入口参数: 

// 返回值：无
// 说   明: 
// *****************************************************************************
inline void HAL_MCUReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U8 bFlashMode);

void HAL_HostDataTransfer(U32 dBuffAddr,U16 wSec, U08 bDir);
void HAL_ADMComSelEccMode(U8 bChan,U8 bMode);

#endif

#if (FUNC_ENABLE == __HAL_FC_DEFINE__)

// *****************************************************************************
// 函数功能: 设置Userdata使能
// 入口参数: 
//      bNewState:新状态
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_FCSetUdEnable(U8 bChan, U8 bNewState);
#endif

#if (FUNC_ENABLE == __HAL_CQ_DEFINE__)

#define CQ_MAX_IDLE_SPACE                        ((U16)1024)

typedef enum
{
    IO_0_PASS = 0,
    IO_1_PASS
}IO_Status;

// *****************************************************************************
// 函数功能: 获取CQ当前的idle空闲空间
// 入口参数: 
//      bChan:通道编号
// 返回值：Idle空闲数量
// 说   明: 最后会增加一条等待DMA2 done的命令
// *****************************************************************************
inline U16 HAL_CQGetIdleSpace(U8 bChan);

// *****************************************************************************
// 函数功能: 等待CQ空出特定数量的空间
// 入口参数: 
//      bChan:通道编号
// 返回值：是否超时
// 说   明:调用此函数，如果CQ idle数量小于给定值，则等待，直到空闲数量大于等于给定值为止
// *****************************************************************************
U8 HAL_CQWaitIdleSpace(U8 bChan, U16 wSpaceCount);

// *****************************************************************************
// 函数功能: 等待CQ做完
// 入口参数: 
//      bChan:通道编号
// 返回值：
// 说   明:调用此函数，如果CQ正在执行，则等待，直到CQ做完为止
// *****************************************************************************
U8 HAL_CQWaitDone(U8 bChan);

// *****************************************************************************
// 函数功能: 停止CQ并清除CQ FIFO
// 入口参数: 
//      bChan:通道编号
// 返回值：
// 说   明:调用此函数，如果CQ正在执行，则等待，直到CQ做完为止
// *****************************************************************************
void HAL_CQReset(U8 bChan);

// *****************************************************************************
// 函数功能: CQ选择片选
// 入口参数: 
//      bChan:通道编号
//      bChip:通道中的片选
// 返回值：
// 说   明:
// *****************************************************************************
void HAL_CQChipSelect(U8 bChan, U8 bChip);

// *****************************************************************************
// 函数功能: CQ发送写脉冲，FIO输出Info Part确定的数据
// 入口参数: 
//      bChan:通道编号
//      bData:FIO输出的数据
// 返回值：
// 说   明:只支持Asyn模式
// *****************************************************************************

inline void HAL_CQSendWRPulse(U8 bChan, U8 bData);

// *****************************************************************************
// 函数功能: CQ发送读脉冲，FIO输出Info Part确定的数据
// 入口参数:
//      bChan:通道编号
//      bData:FIO输出的数据
// 返回值：
// 说   明:只支持Asyn模式
// *****************************************************************************
inline U8 HAL_McuSendReadPluse(U8 bChan);
void HAL_McuSendReadPluseGetData(U8 bChan,U8 *data,U08 bDataLen);
void HAL_MCU_GetOwnership(U08 bMutexReg, U08 bCoreIdx);
void HAL_MCU_ReleaseOwnership(U08 bMutexReg, U08 bCoreIdx);

// *****************************************************************************
// 函数功能: CQ发送一个地址
// 入口参数: 
//      bChan:通道编号
//      bAddr:地址
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSendAddr(U8 bChan, U8 bAddr);

// *****************************************************************************
// 函数功能: CQ发送列地址
// 入口参数: 
//      bChan:通道编号
//      wColAddr:列地址
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSendColAddr(U8 bChan, U16 wColAddr);

// *****************************************************************************
// 函数功能: CQ发送行地址
// 入口参数: 
//      bChan:通道编号
//      dRowAddr:行地址
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSendRowAddr(U8 bChan, U32 dRowAddr);

// *****************************************************************************
// 函数功能: CQ发送4Byte行地址
// 入口参数: 
//      bChan:通道编号
//      dRowAddr:行地址
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSendRowAddr_4Byte(U8 bChan, U32 dRowAddr);

// *****************************************************************************
// 函数功能: CQ发送行、列地址
// 入口参数: 
//      bChan:通道编号
//      wColAddr:列地址
//      dRowAddr:行地址
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSend5Addr(U8 bChan, U16 wColAddr, U32 dRowAddr);

// *****************************************************************************
// 函数功能: CQ发送4个行、2个列地址
// 入口参数: 
//      bChan:通道编号
//      wColAddr:列地址
//      dRowAddr:行地址
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSend6Addr(U8 bChan, U16 wColAddr, U32 dRowAddr);

// *****************************************************************************
// 函数功能: CQ发送随机地址
// 入口参数: 
//      bChan:通道编号
//      wColAddr:列地址
//      dRowAddr:行地址
// 返回值：
// 说   明:
// *****************************************************************************
void HAL_CQSendRndAddr(U8 bChan, U16 wColAddr,U32 dRowAddr);
void HAL_CQSend5RndAddr(U8 bChan, U16 wColAddr,U32 dRowAddr);

// *****************************************************************************
// 函数功能: CQ发送Flash命令
// 入口参数: 
//      bChan:通道编号
//      bCmd:命令
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSendFlashCmd(U8 bChan, U8 bCmd);

// *****************************************************************************
// 函数功能: CQ发送一条指令
// 入口参数: 
//      bChan:通道编号
//      dInstruct:需要发送的指令
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQSendInstruct(U8 bChan, U32 dInstruct);

// *****************************************************************************
// 函数功能: CQ配置Log Number
// 入口参数: 
//      bChan:通道编号
//      dLogNum:需要配置的数
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQCfgLogicNum(U8 bChan, U32 dLogNum);
inline void HAL_CQCfgLogicNumExt(U8 bChan, U32 dLogNum);
inline U32 HAL_CQGetLogicNumExt(U8 bChan);


// *****************************************************************************
// 函数功能: CQ产生一个中断
// 入口参数: 
//      bChan:通道编号
//      bIntNum:中断编号
//      bStopCQ:产生中断，是否停止后面的CQ
// 返回值：
// 说   明:
// *****************************************************************************
inline void HAL_CQGenerateInt(U8 bChan, U8 bIntNum, U8 bStopCQ);


void HAL_CQCheckHold(U8 bDma2Dir,U8 bJoint, U8 bChan, U32 dBuffIdx, U16 wTotalSecCfg,U8 bHoldEn);

inline void HAL_CQCheckHoldOnly(U8 bChan,U8 bHoldEn);

void HAL_CQCheckReadEnd(U8 bChan,U8 bHoldEn);

void HAL_CQSetChkStCtrl(U8 bChan,U8 bRegLastVal,U8 bMode);

// *****************************************************************************
// 函数功能: CQ Check IO 状态
// 入口参数: 
//      bChan:通道编号
//      bLunOfChip:Chip中的Lun编号
//      bStatusType:checke状态类型
//      bFirstCheckIOBit:第一次需要check的IO bit位置
//      bSecondCheckIOBit:满足bFirstCheckIOBit的条件下check bSecondCheckIOBit是否为0
// 返回值：
// 说   明:
// *****************************************************************************
void HAL_CQCheckIOStatus(U8 bChan, U8 bLunOfChip,IO_Status bStatusType, U8 bFirstCheckIOBit, U8 bSecondCheckIOBit);

// *****************************************************************************
// 函数功能: CQ Check 三星FLASH IO 状态
// 入口参数: 
//      bChan:通道编号
//      bLunOfChip:Chip中的Lun编号
//      bStatusType:checke状态类型
//      bFirstCheckIOBit:第一次需要check的IO bit位置
//      bSecondCheckIOBit:满足bFirstCheckIOBit的条件下check bSecondCheckIOBit是否为0
// 返回值：
// 说   明:
// *****************************************************************************
//#if ((NAND_FLASH == K9ADGD8U0C) || (NAND_FLASH == K9ADGD8U0A)) 	
//void HAL_U0xCQCheckIOStatus(U8 bChan, U8 bLunOfChip,IO_Status bStatusType, U8 bFirstCheckIOBit, U8 bSecondCheckIOBit);
//#endif

// *****************************************************************************
// 函数功能: CQ Check IO 状态
// 入口参数: 
//      bChan:通道编号
// 返回值：
// 说   明:
// *****************************************************************************
void HAL_CQCheckStsCfg(U8 bChan, U8 bRdStsMode, U32 dRowAddr,U8 bChip,U8 bLun,U8 bCheckBit, U8 RowMode);

// *****************************************************************************
// 函数功能: CQ 启动DMA2
// 入口参数: 
//      bChan:通道编号
//      dir:方向
//      wBuffIdx:数据所在的RAM地址（扇区为单位）
//      bFlashIdx:数据所在的Flash位置（扇区为单位）
//      bTotalSect:传输数据的总大小（扇区为单位）
//      joint:联动模式
// 返回值：无
// 说   明: 最后会增加一条等待DMA2 done的命令
// *****************************************************************************

void HAL_CQ_SetBeforeCheckHold(U8 bChan,U16 bBufIdx,U8 bTotalSect);

void HAL_CQStartDMA2(U8 bChan, U32 dBuffAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint, U32 dDmaInfoAddr);

inline void HAL_CQCheckRealDone(U8 bChan);

// *****************************************************************************
// 函数功能: CQ 启动DMA2读
// 入口参数: 
//      bChan:通道编号
//      dir:方向
//      wBuffIdx:数据所在的RAM地址（扇区为单位）
//      bFlashIdx:数据所在的Flash位置（扇区为单位）
//      bTotalSect:传输数据的总大小（扇区为单位）
//      joint:联动模式
// 返回值：无
// *****************************************************************************
void HAL_CQStartDMA2Read(U8 bChan, U32 dBuffAddr, U32 dBuffIdx, U8 bFlashIdx, Joint_Mode joint, U32 dDmaInfoAddr, U32 dUDBuffAddr, U32 dRowAddr, U8 *pMaskBuff, U8 bMaskLen,U16 wSeedIdx);

// *****************************************************************************
// 函数功能: CQ 启动DMA2写
// 入口参数: 
//      bChan:通道编号
// 返回值：无
// *****************************************************************************
void HAL_CQStartDMA2Write(U8 bChan, U32 dBuffAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint, U32 dDmaInfoAddr);

// *****************************************************************************
// 函数功能: CQ 设置User data相关信息
// 入口参数: 
//      bChan:通道编号
//      dUdBuff:user data基地址
//      bAttachSize:多少长度带一次user data
//      bUdLen:user data长度
// 返回值：无
// 说   明: 无
// *****************************************************************************
void HAL_CQSetUd(U8 bChan, U32 dUdBuff, U8 bAttachSize, U8 bUdLen,U8 bDir);

// *****************************************************************************
// 函数功能: CQ 设置User data相关信息
// 入口参数: 
//      bChan:通道编号
//      dUdBuff:user data基地址
// 返回值：无
// 说   明: 无
// *****************************************************************************
void HAL_CQSetUdBaseAddr(U8 bChan, U32 dUdBuff,U8 bDir);

// *****************************************************************************
// 函数功能: CQ 设置Raid Buff基地址
// 入口参数: 
//      bChan:通道编号
//      dRaidBuff:Raid Buff基地址
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_CQSetRaidBaseAddr(U08 bChan,U32 dRaidBuff,U08 bMode);


// *****************************************************************************
// 函数功能: CQ 设置User data相关信息
// 入口参数: 
//      bChan:通道编号
//      bAttachSize:多少长度带一次user data
// 返回值：无
// 说   明:无
// *****************************************************************************
inline void HAL_CQSetUdAttachSize(U8 bChan, U8 bAttachSize);

// *****************************************************************************
// 函数功能: CQ 设置User data的长度
// 入口参数: 
//      bChan:通道编号
//      bUdLen:长度
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_CQSetUdLen(U8 bChan, U8 bUdLen);

// *****************************************************************************
// 函数功能: CQ 设置Mask信息
// 入口参数: 
//      bChan:通道编号
//      pMskBuff:mask所在的起始地址
//      bMskLen:设置mask的长度
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_CQSetMask(U8 bChan, U8 *pMskBuff, U8 bMskLen);

// *****************************************************************************
// 函数功能: CQ 设置随机种子
// 入口参数: 
//      bChan:通道编号
//      dRowAddr:行地址
// 返回值：无
// 说   明: 无
// *****************************************************************************
inline void HAL_CQSetRndSeed(U8 bChan, U32 dRowAddr,U16 wSeedIdx);

inline void HAL_CQSetDMAInfoAddr(U8 bChan, U32 dDmaInfoAddr);

inline void HAL_McuSetUd(U8 bChan, U32 dUdBuff, U8 bAttachSize, U8 bUdLen,U8 bDir);

inline void HAL_McuSetMask(U8 bChan, U8 bMskInfo, U8 bMskLen,U8 bMakIdx);

void HAL_McuSetRndSeed(U8 bChan, U32 dRowAddr,U16 wSeedIdx);

inline void HAL_McuSendWRPulse(U8 bChan, U8 bData);

inline void HAL_CqSetEccCtrl(U8 bChan,U32 dEccCtrl);

inline void HAL_CQ_PullRd(U8 bChan,U8 bChipNum);

inline void HAL_CqSetEccCtrlReg(U8 bChan,U32 dEccCtrl);

#endif


#if (__HAL_NF_DEFINE__ == FUNC_ENABLE)

#define NF_ASYCN_FEATURE      0
#define NF_TOGGLE_FEATURE     1
#define NF_TOGGLE2_0_FEATURE  2

#define FAIL_NO_STOP_CQ         0
#define FAIL_STOP_CQ            1

#define WR_PULSE                0
#define RD_PULSE                1

#define GET_CHKSTS_FLAG         0
#define CLR_CHKSTS_FLAG         1

#define ALL_ONE_JUDGE           0
#define ALL_ZERO_JUDGE          1
#define ECC_FAIL_JUDGE          2
#define DDR_TIMEOUT_JUDGE       3 //only bch
#define OVERFLOW_JUDGE          4
#define ABNORMAL_JUDGE          5 //include all one,all zero and ecc-fail

#ifndef __FDL_CRC_FUNC__
#define DMA_INFO_LDPC_FAIL           ((U16)0x3<<14)
#define DMA_INFO_ALL_ONE             ((U16)0x2<<14)
#define DMA_INFO_ALL_ZERO            ((U16)0x1<<14)
#define DMA_INFO_ABNORMAL            (0xC000)
#define DMA_INFO_LDPC_SVAL           (0xFFF)

#define DMAINFO_ALLONE(wDmaInfo)     ((wDmaInfo & DMA_INFO_ABNORMAL) == DMA_INFO_ALL_ONE)
#define DMAINFO_ALLZERO(wDmaInfo)    ((wDmaInfo & DMA_INFO_ABNORMAL) == DMA_INFO_ALL_ZERO)
#define DMAINFO_ECCFAIL(wDmaInfo)    ((wDmaInfo & DMA_INFO_ABNORMAL) == DMA_INFO_LDPC_FAIL)
#define DMAINFO_TIMEOUT(wDmaInfo)    (0)
#define DMAINFO_OVERFLOW(wDmaInfo)   (HAL_NFGetOverFlowThresh(0,1) < (wDmaInfo&DMA_INFO_LDPC_SVAL))//默认认为所有通道的Overflow阈值都一样,取通道0
#define DMAINFO_SVALUE(wDmaInfo)     ((wDmaInfo&DMA_INFO_LDPC_SVAL) << 1)//左移1bit获取S-Value完整值，参见ADM MAS
#define DMAINFO_ABNORMAL(wDmaInfo)   (wDmaInfo & DMA_INFO_ABNORMAL)
#else
#define DMA_INFO_LDPC_FAIL          ((U16)0x7<<13)
#define DMA_INFO_ALL_ONE       		((U16)0x5<<13)
#define DMA_INFO_ALL_ZERO      		((U16)0x3<<13)
#define DMA_INFO_ABNORMAL           (0xE000)
#define DMA_INFO_LDPC_SVAL          (0xFFF)


#define DMAINFO_ALLONE(wDmaInfo)     ((wDmaInfo & DMA_INFO_ABNORMAL) == DMA_INFO_ALL_ONE)
#define DMAINFO_ALLZERO(wDmaInfo)    ((wDmaInfo & DMA_INFO_ABNORMAL) == DMA_INFO_ALL_ZERO)
#define DMAINFO_ECCFAIL(wDmaInfo)    ((wDmaInfo & DMA_INFO_ABNORMAL) == DMA_INFO_LDPC_FAIL)
#define DMAINFO_TIMEOUT(wDmaInfo)    (0)
#define DMAINFO_OVERFLOW(wDmaInfo)   (HAL_NFGetOverFlowThresh(0,1) < (wDmaInfo&DMA_INFO_LDPC_SVAL))//默认认为所有通道的Overflow阈值都一样,取通道0
#define DMAINFO_SVALUE(wDmaInfo)     ((wDmaInfo&DMA_INFO_LDPC_SVAL) << 1)//左移1bit获取S-Value完整值，参见ADM MAS
#define DMAINFO_ABNORMAL(wDmaInfo)   (wDmaInfo & DMA_INFO_ABNORMAL)
#endif


// *****************************************************************************
// 函数功能: 往Flash上改送读/写脉冲
// 入口参数: 
//      bChan:通道编号
//      bMode:发读脉冲还是写脉冲:WR_PULSE(写);RD_PULSE(读)
// 出口参数:bMode = RD_PULSE时,返回FIO上数据，其余时候返回0xFF
// 返回值:无
// 说   明:无
// *****************************************************************************
U8 HAL_NFSendPluse(U8 bChan,U8 bMode);

// *****************************************************************************
// 函数功能: Reset Flash
// 入口参数: 
//      bChan:通道编号
//      bChip:片选
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************
U8 HAL_NFResetFlash(U8 bChan, U8 bChip,U08 bCmd);


// *****************************************************************************
// 函数功能: Reset Lun
// 入口参数: 
//      bChan:通道编号
//      bLun:通道中的Lun编号
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************
void HAL_NFResetLun(U8 bChan, U8 bChip, U8 bLun, U32 dRow);

// *****************************************************************************
// 函数功能: 读取Flash ID
// 入口参数: 
//      dBuffAddr:存放ID的buffer地址
//      bChan:通道编号
//      bChip:片选
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************
void HAL_NFReadFlashID(U32 dBuffAddr, U8 bChan, U8 bChip);

// *****************************************************************************
// 函数功能: 擦除一个快
// 入口参数: 
//      bChan:通道编号
//      bChip:片选
//      dRowAddr:行地址
//      bFlashMode:擦除模式
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************
void HAL_NFEraseBlock(U8 bChan, U8 bChip, U32 dRowAddr);

// *****************************************************************************
// 函数功能:使用通道1的 CE3 抖动指定的次数
// 入口参数: 
//      bCnt:CE抖动次数
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************
void HAL_NFDebugCE(U08 bCnt);    
// *****************************************************************************
// 函数功能:使用DQS发送数据
// 入口参数: 
//      bChan:通道编号
//      bValue:发送的值 
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************
void HAL_MCUSendDataByDqs(U8 bChan,U8 bValue);
void HAL_MCUSendNByteDataByDqs(U8 bChan,U8 *data,U08 bDataLen);

// *****************************************************************************
// 函数功能:切换数据和表格的跳列表
// 入口参数: 
//      
//      
// 出口参数:无
// 返回值:无
// 说   明:跳列表用于补页功能
// *****************************************************************************    
void HAL_NFSwitchJCTable(U32 dJCTableAddr,U08 bJCEn);

// *****************************************************************************
// 函数功能:使用DQS发送数据
// 入口参数: 
//      
//      
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************    
U8 HAL_NFFillPage(U16 wDMABasicLen,U16 wECCParity,U08 bUDataAttachSize,U08 bUDataLen,U08 bTotalSect,U16 wPageSize,U32 wDma2FiltBAddr);

// *****************************************************************************
// 函数功能:带校验功能的Set Flash feature
// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// ***************************************************************************** 
U08 HAL_NFSetFeatureWithCheck(U08 bChan, U08 bChip, U08 bAddr, U08 *data,U08 bDataNum,U08 bSetMode);


// *****************************************************************************
// 函数功能:用MCU的方式设置Flash feature
// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// ***************************************************************************** 
U08 HAL_McuSetFeature(U08 bChan, U08 bChip, U08 bAddr,U08 bSetMode,U08 bByteCnt, U08 *data);

// 函数功能:设置Flash feature
// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// *****************************************************************************  
void HAL_NFSetFeature(U08 bChan, U08 bChip, U08 bAddr, U08 *data,U08 bSetMode);
// *****************************************************************************
// 函数功能:读回Flash feature
// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// *****************************************************************************  

void HAL_NFGetFeature(U08 bChan, U08 bChip, U08 bAddr, U08 *data, U08 bDataNum);
void HAL_NFSetExtFeature(U08 bChan, U08 bChip, U08 bAddr1, U08 bAddr2, U08 bAddr3, U08 *data, U08 bCnt);
void HAL_NFGetExtFeature(U08 bChan, U08 bChip, U08 bAddr1, U08 bAddr2, U08 bAddr3, U08 *data, U08 bDataNum);
U08 HAL_McuSetFeature(U08 bChan, U08 bChip, U08 bAddr,U08 bSetMode,U08 bByteCnt, U08 *data);


// *****************************************************************************
// 函数功能:设置Flash SLC模式时Retry寄存器
// 入口参数: 
//      bChan:设置的Flash模式:
// 出口参数:无
// 返回值:无
// 说   明:无
// ***************************************************************************** 

void HAL_NFSetRetyReg(U8 bChan,U8 bValue);

void HAL_NFReadRetryTab(U32 dBuffAddr);


// *****************************************************************************
// 函数功能:设置check status fail/time-out是否停止CQ
// 入口参数: 
//      bChanCnt:通道数,从0到bChanCnt
//      bMode:FAIL_STOP_CQ    1 ; FAIL_NO_STOP_CQ 0
// 出口参数:无
// 返回值:无
// 说   明:无
// *****************************************************************************      
void HAL_NFChkStCtrl(U8 bChanCnt,U8 bMode);

// *****************************************************************************
// 函数功能:设置OverFlow阈值
// 入口参数: 
//      bChan:需要设置的通道
//      bSetVal_0:设置阈值0:  BCH模式指错误个数,LDPC模式指迭代次数,6bit有效
//      wSetVal_1:仅LDPC模式效，为Svalue阈值,12bit有效
// 出口参数:无
// 返回值:无
// 说   明:如果LDPC仅关心Svalue阈值，则将bSetVal_0设置为0xFF;如果仅关心迭代次数
//			阈值,则将wSetVal_1设置为0xFFFF
// *****************************************************************************     
void HAL_SetOverFlowThresh(U8 bChan,U8 bSetVal_0,U16 wSetVal_1);

// *****************************************************************************
// 函数功能:返回设置的OverFlow阈值
// 入口参数: 
//      bChan:需要设置的通道
//      bValNum:设置为0,对应HAL_NFSetOverFlowThresh里的bSetVal_0
//              设置为1,对应wSetVal_1
// 出口参数: bValNum=0时, 返回值为错误个数或迭代次数阈值
//           bValNum=1时，返回值为sValue阈值,BCH模式下不关心，所以返回0xFFFF
// 返回值:无
// 说   明:无
// *****************************************************************************     
U16 HAL_NFGetOverFlowThresh(U8 bChan,U8 bValNum);

// *****************************************************************************
// 函数功能:获取或者清楚硬件Check status异常时的Flag
// 入口参数: 
//      bChan:需要设置的通道
//      bMode:GET_CHKSTS_FLAG 获取;CLR_CHKSTS_FLAG,清除
// 出口参数:bMode=GET_CHKSTS_FLAG，返回非0值表示有异常发生
// 返回值:无
// 说   明:无
// ***************************************************************************** 
U8 HAL_NFGetOrClrChkStsHwFlag(U8 bChan,U8 bMode);

// 函数功能:L06B转B0KB Set Feature(85h->P1=02h,P2=P3=P4=0)
#if (NAND_FLASH == TOS_BICS3)

// 函数功能:设置BICS3 Flash Parameter
// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// *****************************************************************************  
//void HAL_BICS3SetParameter(U08 bChan, U08 bChip, U08 bAddr, U08 data,U08 Enter);
void HAL_BICS3SoftDecodeSetOffset(U08 bChan,U08 bChip,U08 bStep,U08 bBestOption);


// 函数功能:获取BICS3 Flash Parameter
// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// *****************************************************************************  
//void HAL_BICS3GetParameter(U08 bChan, U08 bChip, U08 bAddr, U08 *data,U08 Enter);
#endif

// 入口参数: 
// 出口参数:
// 返回值:
// 说   明:
// *****************************************************************************  
void HAL_L06B2B0KB(U08 bChan,U08 bChip);
void HAL_SetRegisterOffset(U08 bChan,U08 bChip,U08 bStep);


#endif


#if  (FUNC_ENABLE == __HAL_SPECIAL_FUNCTION__)
// *****************************************************************************
// 函数功能: 求最小值
// 入口参数: 
//      x:数据1
//      y:数据2
// 返回值：数据中较小值
// 说   明: 
// *****************************************************************************
#define Min(x,y)        ((x) >= (y) ? (y) : (x))

// *****************************************************************************
// 函数功能: 求最大值
// 入口参数: 
//      x:数据1
//      y:数据2
// 返回值：数据中较大值
// 说   明: 
// *****************************************************************************
#define Max(x,y)        ((x) >= (y) ? (x) : (y))

// *****************************************************************************
// 函数功能: Memory 拷贝函数
// 入口参数: 
//      dst:目标地址
//      src:源数据所在地址
//      count:需要拷贝数据大小
// 出口参数: 无
// 说   明: 
// *****************************************************************************
void *HAL_MemCopy(void *dst, const void *src,U32 count);

// *****************************************************************************
// 函数功能: Memory 比较函数
// 入口参数:
//      dst:目标地址
//      src:源数据所在地址
//      count:需要比较数据大小
// 出口参数: 无
// 说   明: 提高效率，以U32类型比较，长度必须为4的倍数
// *****************************************************************************
U8 HAL_MemComp(void *dst, const void *src,U32 count);

// *****************************************************************************
// 函数功能: Memory 设置函数
// 入口参数: 
//      dst:目标地址
//      value:需要设置的数值
//      count:需要设置的memory大小
// 出口参数: 无
// 说   明: 
// *****************************************************************************
void *HAL_MemSet(void *dst, U32 value, U32 count);

// *****************************************************************************
// 函数功能: 简单Deley
// 入口参数: 
//      dTimerCnt:delay值
// 出口参数: 无
// 说   明: 
// *****************************************************************************
void  HAL_Delay(U32  dTimerCnt);
#endif

#if (FUNC_ENABLE == __HAL_BFC_DEFINE__)

// *****************************************************************************
// 函数功能: BFC初始化
// 入口参数: 无
// 出口参数: 无
// 说   明: 
// *****************************************************************************
void HAL_BFCInit(void);

// *****************************************************************************
// 函数功能: 获取Buffer Status值
// 入口参数: 
//      wIndex:索引值
// 出口参数: 无
// 返回值：wIndex开始的16个sector的buffer status
// 说   明: 
// *****************************************************************************
U16 HAL_BFCGetBuffStatus(U16 wIndex);

// *****************************************************************************
// 函数功能: 设置相应扇区的Buffer Status为1
// 入口参数: 
//      wIndex:索引值
//      wNewBitPosition:需要设置的bit位置
// 出口参数: 无
// 返回值：无
// 说   明: 
// *****************************************************************************
void HAL_BFCSetBuffStatusBit(U16 wIndex);

// *****************************************************************************
// 函数功能: 设置相应扇区的Buffer Status为1
// 入口参数: 
//      wIndex:索引值
//      wNewBitPosition:需要设置的bit位置
// 出口参数: 无
// 返回值：无
// 说   明: 
// *****************************************************************************
void HAL_BFCSetBuffStatus(U16 wIndex,U16 wNewBitPosition);

// *****************************************************************************
// 函数功能: 设置相应扇区的Buffer status为0
// 入口参数: 
//      wIndex:索引值
//      wNewBitPosition:需要清除的bit位置
// 出口参数: 无
// 返回值：wIndex开始的16个sector的buffer status
// 说   明: 
// *****************************************************************************
void HAL_BFCClearBuffStatus(U16 wIndex,U16 wNewBitPosition);

// *****************************************************************************
  // 函数功能: 等待Buffer status为0
  // 入口参数:
  //
  //
  // 出口参数: 无
  // 返回值：
  // 说   明:
  // *****************************************************************************
void HAL_WaitBuffStatusClr(void);

#endif

#if (FUNC_ENABLE == __HAL_MTR_DEFINE__)

#define TIMER_NUM_0   (0)
#define TIMER_NUM_1   (1)
#define TIMER_NUM_2   (2)
#define TIMER_NUM_3   (3)
#define ONE_HOUR_IN_MILLISECND           0x36EE80        /*1小时的毫秒数*/

// *****************************************************************************
// 函数功能: 初始化定时器
// 入口参数: 
//      bTmrNum:定时器编号（0-3）
//      dMlisec:毫秒
//      dMlisecondUnit:毫秒单位
// 出口参数: 无
// 返回值：wIndex开始的16个sector的buffer status
// 说   明: 
// *****************************************************************************
void HAL_TMRInit(U8 bTmrNum, U32 dMlisec,U32 dMlisecondUnit);
void HAL_TMRDelayMs(U8 bTmrNum, U32 dMicrosec,U32 dMicroSecondUnit);
void HAL_TMRDelayUs(U8 bTmrNum, U32 dMicrosec,U32 dMicroSecondUnit);
void HAL_TMRClearFlag(U8 bTmrNum);
#endif

#if (FUNC_ENABLE == __HAL_UART_DEFINE__)

#define UART0 0
#define UART1 1
#define UART2 2

#define UART_BAUD_9600        (0U)
#define UART_BAUD_19200       (1U)
#define UART_BAUD_38400       (3U)
#define UART_BAUD_57600       (5U)
#define UART_BAUD_115200      (11U)

#define UART_TX         0
#define UART_RX         1
#define UART_BOTH       2

#define TX_DONE_BIT     ((U32)1<<0)
#define RX_DONE_BIT     ((U32)1<<1)


// *****************************************************************************
// 函数名称: UART_Init
// 函数功能: 初始化函数
// 入口参数: 
//           bNum:串口号，可取值：UART0,UART1
//           dBaud:波特率 UART_BAUD_9600,UART_BAUD_19200,UART_BAUD_38400,UART_BAUD_57600,UART_BAUD_115200
//           dBufferAddr:收发缓冲区
// 出口参数: 
// 说    明: 
// *****************************************************************************
//void HAL_UARTInit(U8 bNum,U32 dBaud,U32 dBufferAddr,U32 dMicroSecondUnit);
void HAL_UARTSendMsg(U8 bNum,U8 *pInfo,U16 wLen);
//U8 HAL_UARTRead(U8 bNum,U8 *pInfo,U16 wLen);
//U16 HAL_UARTBytesToRead(U8 bNum);

#endif

#if (FUNC_ENABLE == __HAL_PAD_DEFINE__)
#define GPIO_MODE                  (1)
#define FUCN_MODE                  (0)

#define GPIO_IN                    (1)
#define GPIO_OUT                   (0)

#define LED0_NUM    (0)
#define LED1_NUM    (1)

#define LED_MODE_OFF    (0)
#define LED_MODE_ON     (!LED_MODE_OFF)

// rPAD_FUNC_CTRL
#define  MODE_OUTPUT              (0x00)    //GPIO模式为输出模式
#define  MODE_INPUT               (0xFF)    //GPIO模式为输入模式


#define  PULLUP_DIS             (0)
#define  PULLUP_EN              (1)

#define  PULL_DOWN               (0)
#define  PULL_UP                 (1)
#define  PAD_PS0to3_REG_DEFAULTS     (0x0000059F)
#define  PAD_PS4_REG_DEFAULTS        (0x00FFFFA7)

#define  DQS_FC_MODE            (0)
#define  DQS_GPIO_MODE          (1)

#define  MODE_FUNC (0)
#define  MODE_GPIO (1)
#define  MODE_PULLEN            (1)
#define	 MODE_PULLDISABLE       (0)


//rPAD_SSEL_CTRL0
#define  CELL_SSEL_DRIV_21MA    (1)
#define  CELL_SSEL_DRIV_28MA    (2)
#define  CELL_SSEL_DRIV_48MA    (3)
#define  CELL_SSEL_DRIV_55MA    (4)
#define  CELL_SSEL_DRIV_76MA    (5)
#define  CELL_SSEL_DRIV_83MA    (6)
#define  CELL_SSEL_DRIV_103MA   (7)
#define  CELL_SSEL_DRIV_HIGHZ   (0)

#define	 BIT_DQ_SSEL             (0x03)
#define	 BIT_CE_SSEL             (0x03<<4)
#define	 BIT_CLE_SSEL            (0x03<<8)
#define	 BIT_ALE_SSEL            (0x03<<12)
#define	 BIT_WE_SSEL             (0x03<<16)
#define	 BIT_DQS_SSEL            (0x03<<20)
#define	 BIT_RE_SSEL             (0x03<<24)

#define  ALL_SSEL_MODE(bMode)    ((U32)bMode | (U32)bMode <<4| (U32)bMode <<8| (U32)bMode <<12| (U32)bMode <<16| (U32)bMode <<20| (U32)bMode <<24)
//rPORT_DATA0
//rPORT_DATA12
#define  BIT_ROMC_DATA          (1<<4)
//rPORT_DATA16
#define	 BIT_6ADDR_DATA         (1<<2)  
#define  BIT_FLASHRND_DATA      (1<<3)


/******************************变量声明******************************/
#define PORT_0        (0)
#define PORT_1        (1)
#define PORT_2        (2)
#define PORT_3        (3)
#define PORT_4        (4)
#define PORT_5        (5)
#define PORT_6        (6)
#define PORT_7        (7)
#define PORT_8        (8)
#define PORT_9        (9)
#define PORT_10        (10)
#define PORT_11        (11)
#define PORT_12        (12)
#define PORT_13        (13)
#define PORT_14        (14)
#define PORT_15        (15)
#define PORT_16        (16)
#define PORT_17        (17)



void HAL_PADDqsPullEnCtrl(U8 bChNum,U8 bMode);
void HAL_PADDqsFunc(U8 bChNum,U8 bMode);
void HAL_PADRBFunc(U8 bMode);
void HAL_PADSSelCtrl0to3(U8 bPadNum,U32 dVal);
void HAL_PADSSelCtrl4(U32 dVal);
void HAL_PADTSelCtrl(U32 dVal);
U32  HAL_PADTSelGet(void);
void HAL_PADSTCtrl(U32 dVal);
void HAL_PADPullEn0to3(U8 bPadNum,U32 dVal1,U32 dVal2);
void HAL_PADPullEn4(U32 dVal1,U32 dVal2);
void HAL_PADPullEn5(U32 dVal1,U32 dVal2);
void HAL_PADFuncCtrl0to3(U8 bCh, U32 dVal);
void HAL_PADFuncCtrl4(U32 dVal);
void HAL_PADSetPortMode(U8 bPortNum,U32 dIOMode);
void HAL_PADSetPortData(U8 bPortNum, U32 dVal);
void HAL_PADPortDataInOut(U8 bPortNum,U32 dIOMode ,U32 dVal);
void HAL_PADSpiModeSel(U8 bMode);
void HAL_PADUartModeSel(U8 bMode);
void HAL_PADUartCfg(U8 bCfg);
void HAL_PADLEDxModeSel(U8 bNum,U8 bMode);
void HAL_PADRomcModeSel(U8 bMode);
U32 HAL_PADGetPortData(U8 bPortNum);
void HAL_LED_Config(U8 bNum,U8 bMode,U16 bPeriod);
U08 HAL_GetCurrentSensor(void);


#endif


#if (FUNC_ENABLE == __HAL_CPM_DEFINE__)


#define WDT_DISABLE             (0xAA)
#define WDT_ENABLE              (0x00)

#define WDT_SOURCE_CLK          (25000000U) /*25M*/
#define WDT_MILLISECOND_CNT     (25000U) /*1MS 计数*/

#define CLK_1_DIV       (1U)//分频系数
#define CLK_2_DIV       (2U)
#define CLK_4_DIV       (4U)
#define CLK_6_DIV       (6U)
#define CLK_8_DIV       (8U)
#define CLK_10_DIV      (10U)
#define CLK_12_DIV      (12U)
#define CLK_14_DIV      (14U)
#define CLK_16_DIV      (16U)

#define CLK_PER_SEL     (1U)//时钟源选择
#define CLK_333MHZ_SEL  (2U)
#define CLK_375MHZ_SEL  (4U)
#define CLK_400MHZ_SEL  (8U)

#define GATE0_MCU_G0          (0U)
#define GATE0_MCU_G3          (1U)
#define GATE0_DIU             (2U)
#define GATE0_FC0_SYSTEM_CLK  (3U)
#define GATE0_FC1_SYSTEM_CLK  (4U)
#define GATE0_FC2_SYSTEM_CLK  (5U)
#define GATE0_FC3_SYSTEM_CLK  (6U)
#define GATE0_PADCTRL         (7U)
#define GATE0_PMCP0           (8U)
#define GATE0_PMCP1           (9U)
#define GATE0_UART0           (10U)
#define GATE0_UART1           (11U)
#define GATE0_VMC0            (12U)
#define GATE0_VMC1            (13U)
#define GATE0_ADM             (14U)
#define GATE0_BFC             (15U)
#define GATE0_DARB            (16U)
#define GATE0_DMA             (17U)
#define GATE0_RAID            (18U)
#define GATE0_RM              (19U)
#define GATE0_SPI             (20U)
#define GATE0_BCH0_ENCODE_CLK (21U)
#define GATE0_BCH0_DECODE_CLK (22U)
#define GATE0_BCH1_ENCODE_CLK (23U)
#define GATE0_BCH1_DECODE_CLK (24U)
#define GATE0_FC0_TRX2_CLK    (25U)
#define GATE0_FC1_TRX2_CLK    (26U)
#define GATE0_FC2_TRX2_CLK    (27U)
#define GATE0_FC3_TRX2_CLK    (28U)

#define GATE1_LENC_SYSTEM_CLK (0U)
#define GATE1_LDEC_SYSTEM_CLK (1U)
#define GATE1_SC_SYSTEM_CLK   (2U)
#define GATE1_ROM             (3U)
#define GATE1_COMRAM0         (4U)
#define GATE1_COMRAM1         (5U)
#define GATE1_COMRAM2         (6U)
#define GATE1_COMRAM3         (7U)
#define GATE1_COMRAM4         (8U)
#define GATE1_COMRAM5         (9U)
#define GATE1_COMRAM6         (10U)
#define GATE1_COMRAM7         (11U)
#define GATE1_COMRAM8         (12U)
#define GATE1_COMRAM9         (13U)
#define GATE1_MIXRAM0         (14U)
#define GATE1_MIXRAM1         (15U)
#define GATE1_RAIDRAM0        (16U)
#define GATE1_RAIDRAM1        (17U)
#define GATE1_RAIDRAM2        (18U)
#define GATE1_RAIDRAM3        (19U)
#define GATE1_SC_TRANS_CLK    (20U)
#define GATE1_SC_PMALIVE_CLK  (21U)
#define GATE1_LENC_ENCODE_CLK (22U)
#define GATE1_LDEC_ENCODE_CLK (23U)

#define SREST_MCU_G0     (0U)
#define SREST_MCU_G3     (1U)
#define SREST_DIU        (2U)
#define SREST_FC0_MODULE (3U)//包括sysclk，trx2clk
#define SREST_FC1_MODULE (4U)//包括sysclk，trx2clk
#define SREST_FC2_MODULE (5U)//包括sysclk，trx2clk
#define SREST_FC3_MODULE (6U)//包括sysclk，trx2clk
#define SREST_FC0        (7U)
#define SREST_FC1        (8U)
#define SREST_FC2        (9U)
#define SREST_FC3        (10U)
#define SREST_PADCTRL    (11U)
#define SREST_PMCP0      (12U)
#define SREST_PMCP1      (13U)
#define SREST_UART0      (14U)
#define SREST_UART1      (15U)
#define SREST_VMC0       (16U)
#define SREST_VMC1       (17U)
#define SREST_ADM        (18U)
#define SREST_BFC        (19U)
#define SREST_DARB       (20U)
#define SREST_DMA        (21U)
#define SREST_RAID       (22U)
#define SREST_RM         (23U)
#define SREST_SPI        (24U)
#define SREST_BCH0       (25U)
#define SREST_BCH1       (26U)
#define SREST_SC         (27U)
#define SREST_LENC       (28U)
#define SREST_LDEC       (29U)
#define SREST_PHY        (30U)

#define CPM_MONITOR      (0U)
#define MCU_MONITOR      (1U)
#define VMC0_MONITOR     (2U)
#define VMC1_MONITOR     (3U)
#define DMAC_MONITOR     (4U)
#define FC0_MONITOR      (5U)
#define PMCP0_MONITOR    (6U)
#define PMCP1_MONITOR    (7U)
#define PMCP2_MONITOR    (8U)
#define PMCP3_MONITOR    (9U)
#define SPI_MONITOR      (10U)
#define UART0_MONITOR    (11U)
#define UART1_MONITOR    (12U)
#define SC_MONITOR       (13U)
#define MEMCTROL_MONITOR (14U)
#define FC1_MONITOR      (15U)
#define FC2_MONITOR      (16U)
#define FC3_MONITOR      (17U)

#define NONE_MODE        (0U)
#define PARTIAL_MODE     (1U)
#define SLUMBER_MODE     (2U)  
#define SLUMBER_S_MODE   (4U)  
#define OFFLINE_MODE     (8U)


typedef enum
{
	VDT_NORMAL = 0,
    VDT_FALL   = 1,
    VDT_LOW    = 2,
}VDT_Status;

static inline void CPM_ClearVDTRISESts(void)
{
    SG_CPM->rCPM_INT_STS.bits.VDT_WARN_RISE_INT_STS =0;
}

static inline void CPM_ClearVDTFALLSts(void)
{
    SG_CPM->rCPM_INT_STS.bits.VDT_WARN_FALL_INT_STS =0;
}

// *****************************************************************************
// 函数名称: CPM_VDT_FALLINT_EN
// 函数功能: FALL INT中断使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_VDT_FALLINT_EN(void)
{
    SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_FALL_INT_EN=1;
}

// *****************************************************************************
// 函数名称: CPM_VDT_FALLINT_DIS
// 函数功能: FALL INT中断关闭
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_VDT_FALLINT_DIS(void)
{
    SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_FALL_INT_EN=0;
}

// *****************************************************************************
// 函数名称: CPM_VDT_RISEINT_EN
// 函数功能: RISE INT中断使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_VDT_RISEINT_EN(void)
{
    SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_RISE_INT_EN=1;
}

// *****************************************************************************
// 函数名称: CPM_VDT_RISEINT_DIS
// 函数功能: RISE INT中断关闭
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_VDT_RISEINT_DIS(void)
{
    SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_RISE_INT_EN=0;
}
// *****************************************************************************
// 函数名称: CPM_VDT_LOWINT_EN
// 函数功能: LOW INT中断使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_VDT_LOWINT_EN(void)
{
	SG_CPM->rCPM_INT_CTRL.bits.VDT_LOW_INT_EN = 1;
}

// *****************************************************************************
// 函数名称: CPM_VDT_LOWINT_DIS
// 函数功能: LOW INT中断关闭
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_VDT_LOWINT_DIS(void)
{
	SG_CPM->rCPM_INT_CTRL.bits.VDT_LOW_INT_EN = 0;
}

// *****************************************************************************
// 函数名称: CPM_WDT0_INT_EN
// 函数功能: WDT0_中断使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT0_INT_EN(void)
{
  //  SG_CPM->rCPM_INT_CTRL.bits.WDT0_INT_EN=1;
	SG_CPM->rWDT0_EN = 1;
}

// *****************************************************************************
// 函数名称: CPM_WDT0_INT_DIS
// 函数功能: WDT0_中断关闭
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT0_INT_DIS(void)
{
  //  SG_CPM->rCPM_INT_CTRL.bits.WDT0_INT_EN=0;
	SG_CPM->rWDT0_EN = 0xaa;
}

// *****************************************************************************
// 函数名称: CPM_WDT1_INT_EN
// 函数功能: WDT1_中断使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT1_INT_EN(void)
{
  //  SG_CPM->rCPM_INT_CTRL.bits.WDT1_INT_EN=1;
	SG_CPM->rWDT1_EN = 1;
}

// *****************************************************************************
// 函数名称: CPM_WDT1_INT_DIS
// 函数功能: WDT1_中断关闭
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT1_INT_DIS(void)
{
   // SG_CPM->rCPM_INT_CTRL.bits.WDT1_INT_EN=0;
	SG_CPM->rWDT1_EN = 0xaa;
}

// *****************************************************************************
// 函数名称: WDT_Clear
// 函数功能: WDT喂狗
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT0_FeedDog(void)
{
    SG_CPM->rWDT0_CLEAR=1;
}

// *****************************************************************************
// 函数名称: WDT_Enable
// 函数功能: WDT使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT0_Enable(void)
{
    SG_CPM->rWDT0_EN=WDT_ENABLE;
}

// *****************************************************************************
// 函数名称: WDT_Disable
// 函数功能: WDT除能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT0_Disable(void)
{
    SG_CPM->rWDT0_EN=WDT_DISABLE;
}  
// *****************************************************************************
// 函数名称: WDT1_Clear
// 函数功能: WDT1喂狗
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT1_FeedDog(void)
{
    SG_CPM->rWDT1_CLEAR=1;
}

// *****************************************************************************
// 函数名称: CPM_WDT1_Enable
// 函数功能: WDT1使能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT1_Enable(void)
{
    SG_CPM->rWDT1_EN=WDT_ENABLE;
}

// *****************************************************************************
// 函数名称: CPM_WDT1_Disable
// 函数功能: WDT1除能
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************?
static inline void CPM_WDT1_Disable(void)
{
    SG_CPM->rWDT1_EN=WDT_DISABLE;
}
/******************END 看门狗相关WDT*******************/ 


static inline void CPM_ENABLE_G0(void)
{
    SG_CPM->rMCU_CTRL.bits.bVAL = 1;
}

static inline void CPM_DIABLE_G0(void)
{
    SG_CPM->rMCU_CTRL.bits.bVAL = 0;
}

void HAL_CPMSysResRegInit(void);
void HAL_CPMTrx2ClkCfg(U8 bChan, U8 bSouceSel,U8 bPeriod);
void HAL_CPMSysClkCfg(U8 bSouceSel,U32 bPeriod);
void HAL_CPMPhySoftReset(void);
void HAL_CPMSpiClkCfg(U32 bPeriod);
void HAL_CPMLdecClkCfg(U8 bSouceSel,U32 bPeriod);
void HAL_CPMOpenSoftGate0(U8 bGateType);
void HAL_CPMCloseSoftGate0(U8 bGateType);
void HAL_CPMOpenSoftGate1(U8 bGateType);
void HAL_CPMCloseSoftGate1(U8 bGateType);
void HAL_CPMSoftRest(U8 bMode);
void HAL_CPMSetCHxDQSDelay(U8 bChannel, U8 bVaule);
void HAL_CPMWDT0_Init(U32 dTime);
void HAL_CPMWDT1_Init(U32 dTime);
U32 HAL_CPMGetVDTCnt(void);
void HAL_CPMSetVDTCnt(U32 dCnt);
void HAL_CPMSetPhyPWMode(U8 bMode);
U32 HAL_CPMGetSysTickCnt(void);
U8 HAL_CPMGetClkSource(U8 bChan);
U8 HAL_CPMGetSysPeriod(void);
U8 HAL_CPMGetTrx2Period(U8 bChan);

void HAL_CPMDQS_HandleSetDelay(U8 bChan,U32 dDelayVal);
U8 HAL_CPMDQS_AddllSetDelay(U8 bChan,U8 bMode,U32 dInitVal);

#endif



#if (FUNC_ENABLE == __HAL_SATA_DEFINE__)

typedef  struct
{  
    U08  m_bNCQTag[32];
    U32  m_dTabCnt; //有效tag个数
}T_NCQ_NUM; 

// *****************************************************************************
// 函数功能: 
// 入口参数: 
// 出口参数: 无
// 返回值: 
// 说   明:
// *****************************************************************************
inline U8 HAL_GetSataErrInfo(void);

// *****************************************************************************
// 函数功能: 
// 入口参数: 
// 出口参数:
// 返回值: 
// 说   明:
// *****************************************************************************
inline U8 HAL_ClrSataErrInfo(void);

// *****************************************************************************
// 函数说明： 设置NCQ tag号
// 参数说明：
//        ptNCQ：NCQ排列，需要设置的寄存器
//        ptReadDesc:Host读的地址
// 返回值：无
// 说   明:
// *****************************************************************************
void HAL_SataSetNCQTag(T_NCQ_NUM *ptNCQ);
#endif

// *****************************************************************************
// 函数功能: 在联动写过程中，发生编程fail，此时dma停止 操作，
//           但dmabuff上可能还有有效数据，需此接口告诉FTL哪些数据有效
// 入口参数:
// 出口参数:
// 返回值:
// 说	明:
// *****************************************************************************
#define BIT_CLR_0(Bit)            (~((U32)(0x1<<(Bit))))
#define BIT_SET_1(Bit)            ((U32)(0x1<<(Bit)))

void HAL_GPIOInit(void);
void HAL_GPIODebug(U8 bGPIONum,U16 bPulseCnt);
void HAL_GPIOValDebug(U8 bData);
// *****************************************************************************
// 函数功能: HAL系统初始化，用于配置系统时钟源,系统时钟分频和TRX2时钟分频
// 入口参数: dSysClkSourSel			系统时钟源
//			 dSysClkPeriod    	系统时钟分频系数
//			 bTrx2ClkPeriod0	通道0的TRX2时钟分频系数
//			 bTrx2ClkPeriod1	通道1的TRX2时钟分频系数
//			 bTrx2ClkPeriod2	通道2的TRX2时钟分频系数
//			 bTrx2ClkPeriod3	通道3的TRX2时钟分频系数
// 出口参数:
// 返回值: 
// 说   明:
// *****************************************************************************
void HAL_SystemInit(U32 dSysClkSourSel, U32 dSysClkPeriod, U08 bTrx2ClkSourSel, U8 bTrx2ClkPeriod0, U8 bTrx2ClkPeriod1,U8 bTrx2ClkPeriod2, U8 bTrx2ClkPeriod3);

// *****************************************************************************
// 函数功能: 读efuse信息
// 入口参数: 
// 出口参数:
// 返回值: 
// 说   明:
// *****************************************************************************
void HAL_EFUSE_Read(void);

// *****************************************************************************
// 函数功能: VDT校准
// 入口参数: 
// 出口参数:
// 返回值: 
// 说   明:
// *****************************************************************************
void HAL_VDT_Calibration(void);

// *****************************************************************************
// 函数功能: 读取temperature sensor的值 
// 入口参数: 
// 出口参数:
// 返回值: 	temperature sensor的值 	
// 说   明:
// *****************************************************************************
//U08 HAL_GetTemperature(U08 bMode);
U08 HAL_GetTemperature(void);

// *****************************************************************************
// 函数功能: VDT功能初始化
// 入口参数: bMode:电压选择 	0: 1.8V   1: 3.3V
//			 bLevel:告警电平值，共16档	
// 出口参数:
// 返回值: 		
// 说   明:
// *****************************************************************************
void HAL_VDT_Init(U08 bMode, U08 bLevel);

U08 HAL_McuGetFeature(U08 bChan, U08 bChip, U08 bAddr, U08 *data);

#ifdef __cplusplus
}
#endif
#endif/*end of _SG9081_H_*/
