/*
 * hal_pmcp.h
 *
 *  Created on: Jul 19, 2017
 *      Author: webb
 */

#ifndef INTERFACE_HAL_PMCP_H_
#define INTERFACE_HAL_PMCP_H_

#include "hal_config.h"
#include "data_type.h"
#include "sg9081.h"
#include "Debug.h"

#define PMCP_NUM_0      (0)
#define PMCP_NUM_1      (1)

#ifndef __SW_PMCP__
#if (FUNC_ENABLE == __HAL_PMCP_DEFINE__)


#define PMCPIDX			(PMCP_NUM_0)		//默认使用PMCP0
#define PMCPIDX_1	    (PMCP_NUM_1)

#define PX_FixNum_ADD_Matrix            0b000000        //  固定数的矩阵加
#define PX_FixNum_MINUS_Matrix          0b000001        //	固定数的矩阵减
#define PX_FixNum_AND_Matrix            0b000010        //  固定数的矩阵与
#define PX_FixNum_OR_Matrix             0b000011        //  固定数的矩阵或
#define PX_FixNum_XOR_Matrix            0b000110        //  固定数的矩阵异或
#define PX_FixNum_AOR_Matrix            0b000111        //  固定数的矩阵同或
#define PX_Matrix_ADD_Matrix            0b100000        //	矩阵与矩阵相加
#define PX_Matrix_MINUS_Matrix          0b100001        //	矩阵与矩阵相减
#define PX_Matrix_AND_Matrix            0b100010        //	矩阵与矩阵与
#define PX_Matrix_OR_Matrix             0b100011        //	矩阵与矩阵或
#define PX_Matrix_XOR_Matrix            0b100110        //	矩阵与矩阵异或
#define PX_Matrix_AOR_Matrix            0b100111        //	矩阵与矩阵同或

#define PX_Region_SCH1                  0b001000        //  区间搜索1 A<x<B
#define PX_Region_SCH2                  0b001001        //  区间搜索2 A<x<=B
#define PX_Region_SCH3                  0b001010        //  区间搜索3 A<=x<=B
#define PX_Region_SCH4                  0b001011        //  区间搜索4 A<=x<B

#define PX_Region_Circle_SCH1           0b101000        //  区间环形搜索1
#define PX_Region_Circle_SCH2           0b101001        //  区间环形搜索2
#define PX_Region_Circle_SCH3           0b101010        //  区间环形搜索3
#define PX_Region_Circle_SCH4           0b101011        //  区间环形搜索4

#define PX_MIN_SCH                      0b001100        //  搜索最小值
#define PX_MAX_SCH                      0b001101        //  搜索最大值

#define PX_ThrMinCir_SCH                0b101100        //  带阈值的环形搜索最小值
#define PX_ThrMaxCir_SCH                0b101101        //  带阈值的环形搜索最大值

#define PX_SET_MEM_Index                0b011001        //  带索引的内存值设定
#define PX_BIT_SCH1                     0b011010        //  Bit搜索(搜索Bit值为1)
#define PX_BIT_SCH0                     0b011011        //  Bit搜索(搜索Bit值为0)
#define PX_Memory_Set                   0b011100        //  memory set
#define PX_DataRegion_SCH               0b011101        //  区间段数值搜索
#define PX_BIT_Inquire                  0b011110        //  Bit查询功能
#define PX_IDX_MEMORY_COPY              0b110000        //	带索引的内存值提取
#define PX_REGION_INFO_SCH              0b110001        //	区间段覆盖搜索
#define PX_JC_BIT_COUNT                 0b110010        //	带跳列bit统计
#define PX_CLEAR_BIT_SCH                0b110011        //	带清零bit搜索
#define PX_TRIM_ANALYSE                 0b110110        //	区间数据拼接
#define PX_SORT_CIRCLE_Region           0b110111        //	循环区间排列
#define PX_Matrix_Shift_Mask            0b111000        //	带mask矩阵移位功能
#define PX_MATIRX_ADD					0b110100		// 矩阵元素累加

#define LEFT_COVER_STATE            (0)
#define RIGHT_COVER_STATE           (1)
#define ALL_COVER_STATE             (2)
#define ALL_COVERED_STATE           (3)

#define PMCP_SRCH_MASK_NONE         (0)

#define  PMCP_ARR_MODE_VAL_AND    (0)
#define  PMCP_ARR_MODE_VAL_OR     (1)
#define  PMCP_ARR_MODE_VAL_XOR    (2)
#define  PMCP_ARR_MODE_VAL_AOR    (3)
#define  PMCP_ARR_MODE_VAL_ADD    (4)
#define  PMCP_ARR_MODE_VAL_MINUS  (5)

#define  PMCP_ARR_MODE_AND    (0)
#define  PMCP_ARR_MODE_OR     (1)
#define  PMCP_ARR_MODE_XOR    (2)
#define  PMCP_ARR_MODE_AOR    (3)
#define  PMCP_ARR_MODE_ADD    (4)
#define  PMCP_ARR_MODE_MINUS  (5)

#define  PMCP_ARR_SHIFT_MODE_RIGHT  (0)
#define  PMCP_ARR_SHIFT_MODE_LEFT   (1)

#define  PMCP_MEM_SET_INDX_MODE_1BYTE  (1)
#define  PMCP_MEM_SET_INDX_MODE_2BYTE  (2)
#define  PMCP_MEM_SET_INDX_MODE_4BYTE  (4)
#define  PMCP_MEM_SET_INDX_MODE_BIT    (7)


#define HAL_PMCPMatrixValueAdd(bPMCPIdx, dSrcRamAddr, dFixedNum, dResultAddr, wUnitLen, bUnitStep,bResultStep)  \
    HAL_PMCPMatrixValueOp(bPMCPIdx, dSrcRamAddr, dFixedNum, dResultAddr, wUnitLen, bUnitStep, bResultStep, PMCP_ARR_MODE_VAL_ADD)

#define HAL_PMCPMatrixValueBitAnd(bPMCPIdx, dSrcRamAddr, dFixedNum, dResultAddr, wUnitLen, bUnitStep,bResultStep)  \
    HAL_PMCPMatrixValueOp(bPMCPIdx, dSrcRamAddr, dFixedNum, dResultAddr, wUnitLen, bUnitStep, bResultStep, PMCP_ARR_MODE_VAL_AND)

#define HAL_PMCPMatrixMatrixOR(bPMCPIdx, dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen)  \
    HAL_PMCPMatrixMatrixOp(bPMCPIdx, dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen, sizeof(U32), sizeof(U32), PMCP_ARR_MODE_OR)  //固定为U32的操作


enum {LEFT_INTERSECTION=0x00, RIGHT_INTERSECTION=0x01, INVOLVE=0x02, BE_INVOLVE=0x03};

// *****************************************************************************
// 函数功能: 向总线输出调试信息
// 入口参数:
//      bChan：通道号
//		bData：输出的数据
// 出口参数: 无
// 返回值: 无
// 说   明:
// *****************************************************************************
void HAL_DebugSendByte(U8 bChan, U8 bData);

void HAL_PMCPInit(void);//初始化

inline void HAL_PMCPSetResultAddr(U8 bPmcpNum, U32 dResultAddr);

// *****************************************************************************
// 函数功能: 查询PMCP状态
// 入口参数:
//      bPmcpNum:PMCP编号
// 出口参数: 无
// 返回值:
//      0-正在执行，1-计算完成
// 说   明:
// *****************************************************************************
inline U8 HAL_PMCPCheckState(U8 bPmcpNum);

// *****************************************************************************
// 函数功能: 查询PMCP状态，返回当前IDLE的PMCP编号；若无，返回0xff
// 入口参数: 无
// 出口参数: 无
// 返回值:
//      0-PMCP0，1-PMCP1，0xFF-两个都busy
// 说   明:
// *****************************************************************************
U8 HAL_ReturnIdlePmcpNum(void);

// *****************************************************************************
// 函数功能: 等待PMCP执行完成
// 入口参数:
//      bPmcpNum:PMCP编号
// 出口参数: 无
// 返回值:
//      计算结果，关系计算结果的可以利用该值
// 说   明:程序在这里死等
// *****************************************************************************
U16 HAL_PMCPWaitDone(U8 bPmcpNum);

// *****************************************************************************
// 函数功能: 获取PMCP执行完成后的结果个数
// 入口参数:
//      bPmcpNum:PMCP编号
// 出口参数: 无
// 返回值:
//      计算结果，关系计算结果的可以利用该值
// 说   明:
// *****************************************************************************
//use
inline U16 HAL_PMCPGetResultCount(U8 bPmcpNum);

// *****************************************************************************
// 函数功能: 返回Idle的PMCP编号
// 入口参数: 无
// 出口参数: Idle的PMCP0/1；若无，返回全ff
// 返回值:
// 说   明:
// *****************************************************************************
U8 HAL_ReturnIdlePmcpNum(void);

//use
void HAL_PMCPSearchCover(U8 bPmcpNum, U32 dBuffAddr,U16 wSearchCount, U32 dStartValueBuff,U8 bStartValueStep,U32 dLenBuff,U8 bLenStep,U16 wSegCount, U32 dIndexBuff,U32 dResultBuff);
void HAL_PMCPSrchMin(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff);
void HAL_PMCPSrchMin_WithZero(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff);
void HAL_PMCPSrchMin_WithoutZero(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff);
void HAL_PMCPSrchMax(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff);
void HAL_PMCPSrchMax_WithoutZero(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff);
void HAL_PMCPSrchMax_WithZero(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff);
U16  HAL_PMCPSrchGELE(U8 bPmcpIdx, U32 dMin, U32 dMax, U32 dMaskBit, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U16 wSrchRetCnt);
U16  HAL_PMCPSrchKeyU08(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt, U32 bKey);
U16  HAL_PMCPSrchSameSegData(U32 dSrchMemAddr, U08 bSizeOfUnit, U16 wUnitCnt, U32 dIndxValueMemAddr, U32 dSegCntValueMemAddr, U32 dBitMask, U32 dInvalidValue);
U16  HAL_PMCPSearchSegment(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff);
U16  HAL_SW_SearchSegment(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff);
void HAL_PMCPBitMapCountExtFilter(U8 bPmcpIdx, U32 dSrcAddr,U32 dRetAddr,U32 dSrchUnitSize,U32 dSrchUnitCnt, U16 wSeqUnitCnt, U16 wfilterLen,U8 bResult_Step);
U16 HAL_PMCP_SearchCover(U8 bPmcpIdx, U32 dSrchEaraStart, 		//需要搜索区间的开始地址
    U32 dSrchEaraLen,			//需要搜索区间长度
    U32 dSrchDestStartBuff,	//搜索目标每个区间开始地址所在的buff，步长可配置为1,2，4 Byte
    U08 bSrchDestStartStep,
    U32 dSrchDestLenBuff,	//搜索目标每个区间长度所在的buff，步长可配置为1,2，4 Byte
    U08 bSrchDestLenStep,
    U16 bSrchDestCnt,           //搜索目标区间的个数
    U32 dRetOffBuff,		//返回搜索到的目标区间引索的buff，步长可配置为2，4 Byte
    U32 dRetCoverInf		//返回搜索到的目标区间的覆盖情况的buff: 0->左边覆盖，1->右边覆盖,2->目标区间被全覆盖,3->需要搜索区间被全覆盖，步长可固定为1Byte
    );

U16 HAL_SW_SearchCover(U8 bPmcpIdx, U32 dSrchEaraStart, 		//需要搜索区间的开始地址
    U32 dSrchEaraLen,			//需要搜索区间长度
    U32 dSrchDestStartBuff,	//搜索目标每个区间开始地址所在的buff，步长可配置为1,2，4 Byte
    U08 bSrchDestStartStep,
    U32 dSrchDestLenBuff,	//搜索目标每个区间长度所在的buff，步长可配置为1,2，4 Byte
    U08 bSrchDestLenStep,
    U16 bSrchDestCnt,           //搜索目标区间的个数
    U32 dRetOffBuff,		//返回搜索到的目标区间引索的buff，步长可配置为2，4 Byte
    U32 dRetCoverInf		//返回搜索到的目标区间的覆盖情况的buff: 0->左边覆盖，1->右边覆盖,2->目标区间被全覆盖,3->需要搜索区间被全覆盖，步长可固定为1Byte
    );
U16  HAL_PMCPSrchValidBits(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dIndexBuff,U16 wSeachCnt, U08 bResultStep, U08 bEnableClearBit);
void HAL_PMCPArraShiftBitsMask(U8 bPmcpIdx, U32 dSrchMemAddr, U08 bUnitSize,U16 wUnitCnt,U32 dDestMemAddr,U08 bRetUnitSize,U08 bMode,U08 bShiftBitCnt,U32 dMaskBit);
void HAL_PMCP0ArraPlus(U32 dSrchRamAddr,U08 bUnitSize,U16 wUnitLen,U32 dValue);
void HAL_PMCPSrchMinWithMask(U8 bPmcpIdx, U32 dSrchRamAddr, U32 dBitMask,  U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff);
void HAL_PMCPMemorySet(U8 bPmcpIdx, U32 dRamAddr, U32 dValue, U32 dCount);
U8   HAL_PMCPMemCopy(U8 bPMCPIdx, U32 dResultAddr, U32 dSrcRamAddr, U16 wUnitLen, U8 bUnitStep);
typedef struct
{
	U32 dLba;
	U16 wRev;
	U16 wSectLen;
}T_TRIM_INFO;

U08 HAL_PMCP0TrimAddrCombine(T_TRIM_INFO *ptTrimInfoBuff, U32 dTrimAddrBuff,U32 dTrimSctBuff, U32 dDataLen);

//矩阵操作模式，适用于矩阵矩阵操作，和矩阵固定数操作
enum
{
	MATRIX_AND = 0,
	MATRIX_OR = 1,
	MATRIX_XOR = 2,
	MATRIX_AOR = 3,
	MATRIX_ADD = 4,
	MATRIX_MINUS = 5,
};
U8 HAL_PMCPMatrixMatrixOp(U8 bPMCPIdx, U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bMode);
U8 HAL_PMCPMatrixValueOp(U8 bPMCPIdx, U32 dSrcRamAddr, U32 dFixedNum, U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bMode);
void HAL_PMCP0MemorySetWithIdx(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U08 bIdxStep, U08 bReplaceMode, U32 dSetValue);
void HAL_PMCP0SpecialMemSetWithIdx(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U16 wIdxThreshold, U08 bIdxStep, U08 bReplaceMode, U32 dReplaceValueBase);

U32 PMCP0_Divider_Quo(U32 dDividend, U32 dDivisor, U8 bResLen);
U32 PMCP0_Divider_Rem(U32 dDividend, U32 dDivisor);
U16 HAL_PMCPBitSearch1(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dBitOneNum);
void HAL_PMCP0SetValueToMem(U32 dSrcIdxAddr, U32 dSrcRamAddr, U32 dValue, U16 wUnitLen, U08 bIndexStep, U08 bReplaceMode);
U16 HAL_PMCPGetValueFromMem(U08 bPmcpIdx, U32 dSrcRamAddr, U32 dIndexAddr, U32 dRetBuffAddr, U16 wUnitLen, U08 bIndexStep, U08 bResultStep);
void HAL_PMCPDivision(U16 wDividend, U08 bDivisor,U16 *pwQuotient,U08 *pbRemainder);
U32 Hal_PMCP0MatrixAddAll(U32 dMatrixSrcAddr, U32 dMatrixResultAddr, U32 dMatrixElemCnt, U32 dMatrixSrcStep);
U16  HAL_PMCPSrchSeqSeg(U8 bPMCPIdx, U32 dStartVal, U32 dSrcAreaDatMemAddr, U08 bSrcDatSize, U32 dSrcAreaLenMemAddr, U08 bSrcLenSize, U16 wSrchUnitCnt,U32 dRetItemBuffAddr, U32 *pdTotalLen);
//PMCP的寄存器配置接口
void HAL_PMCPSearchCover_Cfg(U8 bPmcpNum, U32 dNeedSchValue,U16 wNeedSchLen, U32 dStartValueBuff,U8 bRetIndexStepLen,U32 dLenBuff,U8 bSchLenArrStep,U16 wSegCount, U32 dIndexBuff,U32 dResultBuff);
void HAL_PMCPSrchMin_Cfg(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff);
void HAL_PMCPSrchMax_Cfg(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff);
void HAL_PMCPSrchGELE_Cfg(U8 bPmcpIdx, U32 dMin, U32 dMax, U32 dMaskBit, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U16 wSrchRetCnt, U08 bResultStepLen);
void HAL_PMCPSearchSegment_Cfg(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff);
void HAL_PMCPBitMapCountExtFilter_Cfg(U8 bPmcpIdx, U32 dSrcAddr,U32 dRetAddr,U32 dSrchUnitSize,U32 dSrchUnitCnt, U16 wSeqUnitCnt, U16 wfilterLen,U8 bResult_Step);
void HAL_PMCP_SearchCover_Cfg(U8 bPmcpIdx, U32 dSrchEaraStart, U32 dSrchEaraLen, U32 dSrchDestStartBuff,	U08 bSrchDestStartStep, U32 dSrchDestLenBuff, U08 bSrchDestLenStep,
U16 bSrchDestCnt, U32 dRetOffBuff, U32 dRetCoverInf);
void  HAL_PMCPSrchValidBits_Cfg(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dIndexBuff,U16 wSeachCnt, U8 bResultStep, U08 bEnableClearBit);
void HAL_PMCPArraShiftBitsMask_Cfg(U8 bPmcpIdx, U32 dSrchMemAddr, U08 bUnitSize,U16 wUnitCnt,U32 dDestMemAddr,U08 bRetUnitSize,U08 bMode,U08 bShiftBitCnt,U32 dMaskBit);
void HAL_PMCPBitSearch1_Cfg(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dBitOneNum);
void HAL_PMCPSrchMinWithMask_Cfg(U8 bPmcpIdx, U32 dSrchRamAddr, U32 dBitMask, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff);
void HAL_PMCPMemorySet_Cfg(U8 bPmcpIdx, U32 dRamAddr, U32 dValue, U32 dCount);
void HAL_PMCP0TrimAddrCombine_Cfg(T_TRIM_INFO *ptTrimInfoBuff, U32 dTrimAddrBuff, U32 dTrimSctBuff, U32 dDataLen);
void HAL_PMCPMatrixMatrixOp_Cfg(U8 bPMCPIdx, U32 dSrcRamAddr0, U32 dSrcRamAddr1, U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bPmcpOpeMode);
void HAL_PMCPMatrixValueOp_Cfg(U8 bPMCPIdx, U32 dSrcRamAddr, U32 dFixedNum, U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bPmcpOpeMode);
void HAL_PMCP0MemorySetWithIdx_Cfg(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U08 bIdxStep, U08 bReplaceMode, U32 dSetValue);
void HAL_PMCP0SpecialMemSetWithIdx_Cfg(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U16 wIdxThreshold, U08 bIdxStep, U08 bReplaceMode, U32 dReplaceValueBase);
void Hal_PMCP0MatrixAddAll_Cfg(U32 dMatrixSrcAddr, U32 dMatrixResultAddr, U32 dMatrixElemCnt, U32 dMatrixSrcStep);

#define R_U32           ((U32)(0)<<1)//商为32bit，z配置为R_U32，运算周期为32cycle
#define R_U16           ((U32)(1)<<1)//商为16bit，z配置为R_U16，运算周期为16cycle
#define R_U8            ((U32)(2)<<1)//商为8bit，z配置为R_U8，运算周期为8cycle

#define Div(x, y, z)    PMCP0_Divider_Quo(x, y, z) //求商运算
#define Mod(x, y)       PMCP0_Divider_Rem(x, y) //求余运算
#define GetMod()        (SG_PMCP(0)->rREMAINDER) //PX除法，会同时求商和余数，这个宏只能用于函数Div(x, y)后。

#define HAL_Div(x, y, z)	Div(x, y, z)
#define HAL_Mod(x, y)		Mod(x, y)
#define HAL_GetMod()		GetMod()
#ifdef __NEW_PMCP_FUNC__
typedef struct
{
	U32 dVaildSegCnt;    // 有效段总个数
	U32 dVaildItemNum;   // 有效项总个数
	U32 dValidMinSeg;    // 有效段最小段号
	U32 dInValidItemCnt; // 落在dInVaildMaskMax和dInVaildMaskMin之间的个数
}T_DATA_SORT_SEG_RESULT;
typedef struct
{
	U32 dSegListAddr;		// 段链表地址（段链表步长固定为中间链表步长的两倍）
	U32 bDataCnt;			// 数据项总数
	U32 dDataAddr;			// 数据项地址
	U32 dMiddleListAddr;	// 中间链表地址
	U08 bDataStep;			// 数据项步长（2，4，8）
	U08 bMiddleStep;		// 中间链表步长（2，4）
	U08 bSegBitWide;		// 右移位数（不能超过数据步长）
	U08 RSV;
}T_STEP2_PARAM1;
typedef struct
{
	/*********************/
	U64 dVaildMaskMax;		// bitmask后数据最大阈值
	U64 dVaildMaskMin;		// bitmask后数据最小阈值
	U64 ddFrontEndMask;		// bitmask值
	U64 ddBackEndMask;		// dVaildMaskMax和dVaildMaskMin阈值过滤后再次进行bitmask的值
	/*******************/
	U64 dInVaildMaskMax;	// 源数据阈值最大值
	U64 dInVaildMaskMin;	// 源数据阈值最小值
}T_STEP2_PARAM2;
typedef struct
{
	U32 dSegListAddr;		// 段链表地址（段链表步长固定为中间链表步长的两倍）
	U32 dMiddleListAddr;	// 中间链表地址
	U32 dSegOrderResAddr;	// 分段排序结果地址
	U32 dRetOffsAddr;		// 排序后每段的索引位置RetOffs地址
	U32 dSegNumAddr;		// 排序后每段的段号SegNum地址
	U32 dSegCntAddr;		// 排序后每段包含的项个数SegCnt地址
	U8  bMidAndResStep;		// 中间链表/排序结果（2，4，两者步长一致）
	U8  bRetOffsStep;		// RetOffs步长（2，4）
	U8  bSegCntStep;		// SegCnt步长（2，4）
	U8  bSegNumStep;		// SegNum数组步长（2，4）
	U32 dSegListLen;		// 段链表长度（此时的长度应为第一步中的长度的一半）
}T_STEP3_PARAM1;
typedef struct
{
	U32 bValidItemCnt;		// 有效项总个数
	U32 dValidMinSeg;		// 有效段最小段号
	U8  bClrListEn;			// 清除段链表信息使能（1使能，0不使能）
	U8  bReserve[3];
}T_STEP3_PARAM2;
void HAL_PMCPMultBaseDataUnzip(U8 bPmcpIdx, U32 dBaseSrcAddr, U32 dCntSrcAddr, U32 dResultAddr, U32 dDataInc, U32 dContinueNum, U32 dJumpColNum, U16 wBaseOpeCnt, U8 bDataStep);
void HAL_PMCPDataSortSeg_Step1(U8 bPmcpIdx, U32 dSegListAddr, U8 bStep, U64 bVaule, U32 bLen);
U8 HAL_PMCPShiftStatisticsWithMask(U8 bPmcpIdx, U32 dSrcBuff, U32 dRetBuff, U8 bSrcStep, U8 bRetStep, U8 bRightShiftNum, U64 ddMinVal, U64 ddMaxVal, U32 dOpeCnt, U64 ddBitMaskVal, U8 bAddDir);
void HAL_PMCPArraShirtAndWithIndex(U8 bPmcpIdx, U32 *pdSrcBuff, U16 *pwIndexBuff,U16 wIndexCnt,U32 *pdShiftRetBuff1, U16 *pAndRetBuff2,U08 bRightShirtBitCnt,U32 dMaskValue);
void HAL_PMCPDataSortSeg_Step2(U8 bPmcpIdx,T_STEP2_PARAM1 * ptParam1, T_STEP2_PARAM2 * ptParam2, T_DATA_SORT_SEG_RESULT *ptSortResult);
void HAL_PMCPDataSortSeg_Step3(U8 bPmcpIdx,T_STEP3_PARAM1 *ptParam1,T_STEP3_PARAM2 *ptParam2);
void HAL_PMCPValueReplaceAndCntCalc(U32 *pdOldValuebase,U16 *pwIndexBuff,U32 *pdNewValueBuff,U32 dCntBuffAddr,U08 bStepOfCnt,U32 dMaskValue,U08 bRightShirtNum,U16 wTotalIndexCnt);
#endif //end of new PMCP
#endif
#endif
#endif /* INTERFACE_HAL_PMCP_H_ */
