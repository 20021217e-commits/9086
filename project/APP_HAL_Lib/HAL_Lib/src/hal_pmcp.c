#include "hal.h"
#include "debug.h"
#include "uitility.h"

//#define __SW_PMCP__
#ifndef __SW_PMCP__
#define PMCP_PARAM_JUDGE_EN	1		// 是否使能参数判断，1表示使能，0表示不使能。使能时参数不合理，将直接退出！
//CSR REG
#define BIT_STEP_LEN			0
#define BIT_RESULT_STEP_LEN		4
#define BIT_DIFF_BIT_NUM		8
#define BIT_RESULT_CNT			16

//#define LOG_PRINT(x,...)


#define PMCP_9086_NEW 1  // 0表示使用9081的硬件接口，1表示使用9086的硬件接口

U32 dResultBuff;
void  PMCP_IRQHandler(void)
{

}

void  HAL_PMCPInit(void)
{

}

inline void HAL_PMCPSetResultAddr(U8 bPmcpNum, U32 dResultAddr)
{
    SG_PMCP(bPmcpNum)->rRESULT_ADDR = dResultAddr;
}

inline U8 HAL_PMCPCheckState(U8 bPmcpNum)
{
    return SG_PMCP(bPmcpNum)->rOP_START.bits.dVAL;
}

U8 HAL_ReturnIdlePmcpNum(void)
{
	do
	{
		if (1==SG_PMCP(0)->rOP_START.bits.dVAL)
		{
			return 0;
		}
		if (1==SG_PMCP(1)->rOP_START.bits.dVAL)
		{
			return 1;
		}
	}while(1);

	return 0xff;
}

U16 HAL_PMCPWaitDone(U8 bPmcpNum)
{
	U32 dTimeCnt = 0;
	while(!SG_PMCP(bPmcpNum)->rOP_START.bits.dVAL)
	{
		dTimeCnt ++;
		if (dTimeCnt > 0xFFFFFF)
		{
			PRINT_TXT("PMCP_E");
			break;
		}
	}
	return 0;
}
//use
inline U16 HAL_PMCPGetResultCount(U8 bPmcpNum)
{
    return SG_PMCP(bPmcpNum)->rGOT_RESULT_CNT.bits.dVAL;
}

// *****************************************************************************
//函数功能: 搜索最小值
// 入口参数:
//			dSrchRamAddr -> 搜索目标内存区起始地址
//			bUnitSize ->搜索目标的步长
//			wSrchUnitCnt ->搜索目标的个数(以bUnitSize为单位)
//			dRetValueBuffAddr ->存放搜索到的值的起始地址
//			dRetItemBuffAddr ->存放搜索到值的偏移的起始地址，偏移值的步长固定为2B
//			wSrchRetCnt ->需要搜索的最小值个数
//			wRingOff ->环形搜索的起始偏移，为0表示为普通搜索模式
// 出口参数:
// 说    明:
// *****************************************************************************
void  HAL_PMCPSrchMin_Cfg(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1) || (wSrchUnitCnt == 0))
	{
		LOG_PRINT("HAL_PMCPSrchMin,Error_Param ");
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchRamAddr;				//源数据起始地址
	SgPMCP->rBASE_ADDR1 = dRetValueBuffAddr;		//原始值存放地址
	SgPMCP->rRESULT_ADDR = dRetItemBuffAddr;		//统计结果ram位置
	SgPMCP->rOPE_CNT.bits.dVAL = wSrchUnitCnt;		// 源数据个数
	SgPMCP->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (2<<BIT_RESULT_STEP_LEN); // 配置源数据步长为bUnitSize，配置搜索结果步长为2byte
	SgPMCP->rRESULT_CNT = wSrchRetCnt; // 需要搜索的最小值个数
	SgPMCP->rOPE_MODE |= 0x04 | (1 << 13) | (1 << 14); // 最值搜索模式/关闭bitmask/最小值搜索/Index保存使能/保存原值使能/正向搜索
	if(wStartOff != 0)
	{
		SgPMCP->rOPE_MODE |= (1 << 8); // 环形搜索
		SgPMCP->rRMD0_ADDR = wStartOff;	// 环形搜索起始index
	}
	SgPMCP->rMOVE_RLT0_OP_ARRL = 0;	// 最小阈值为0
	SgPMCP->rMOVE_RLT0_OP_ARRH = 0;

	// 最小值搜索时，最大阈值不能为源数据步长对应的最大值（如源数据步长为2byte，则最大值不能为0xFFFE）
	if(bUnitSize == 2)
	{
		SgPMCP->rMIN_VAL = 0xFFFE;
		SgPMCP->rMAX_VAL = 0;
	}
	else if(bUnitSize == 4)
	{
		SgPMCP->rMIN_VAL = 0xFFFFFFFE;
		SgPMCP->rMAX_VAL = 0;
	}

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
	}
}

void  HAL_PMCPSrchMin_WithoutFFFF(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1) || (wSrchUnitCnt == 0))
	{
		LOG_PRINT("HAL_PMCPSrchMin,Error_Param ");
		return;
	}
#endif
	HAL_PMCPSrchMin_Cfg(bPmcpIdx, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wStartOff);
	HAL_PMCPWaitDone(bPmcpIdx);
}
}

void HAL_PMCPSrchMin(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
	HAL_PMCPSrchMin_WithFFFF(bPmcpIdx, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wRingOff);
	}
}

// *****************************************************************************
//函数功能: 搜索最大值
// 入口参数:
//			dSrchRamAddr -> 搜索目标内存区起始地址
//			bUnitSize ->搜索目标的步长
//			wSrchUnitCnt ->搜索目标的个数(以bUnitSize为单位)
//			dRetValueBuffAddr ->存放搜索到的值的起始地址
//			dRetItemBuffAddr ->存放搜索到值的偏移的起始地址，偏移值的步长固定为2B
//			wSrchRetCnt ->需要搜索的最大值个数
//			wRingOff ->环形搜索的起始偏移，为0表示为普通搜索模式
// 出口参数:
// 说    明:
// *****************************************************************************
void  HAL_PMCPSrchMax_Cfg(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMax,Error_Param ");
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchRamAddr;				//原数据存放地址
	SgPMCP->rBASE_ADDR1 = dRetValueBuffAddr;		//原数据存放地址
	SgPMCP->rRESULT_ADDR = dRetItemBuffAddr;		//统计结果ram位置
	SgPMCP->rOPE_CNT.bits.dVAL = wSrchUnitCnt;		// 源数据个数
	SgPMCP->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (2<<BIT_RESULT_STEP_LEN); // 配置源数据步长为bUnitSize，配置搜索结果步长为2byte
	SgPMCP->rRESULT_CNT = wSrchRetCnt; // 需要搜索的最大值个数
	SgPMCP->rOPE_MODE |= 0x04 | (1 << 10) | (1 << 13) | (1 << 14); // 最值搜索模式/关闭bitmask/最大值搜索/Index保存使能/保存原值使能/正向搜索
	if(wStartOff != 0)
	{
		SgPMCP->rOPE_MODE |= (1 << 8); // 环形搜索
		SgPMCP->rRMD0_ADDR = wStartOff;	// 环形搜索起始index
	}

	SgPMCP->rMOVE_RLT0_OP_ARRL = 1;	// 最小阈值为1（当为最大值搜索时，最小阈值不能为0）
	SgPMCP->rMOVE_RLT0_OP_ARRH = 0;

	// 最大阈值为全F
	if(bUnitSize == 2)
	{
		SgPMCP->rMIN_VAL = 0xFFFF;
		SgPMCP->rMAX_VAL = 0;
	}
	else if(bUnitSize == 4)
	{
		SgPMCP->rMIN_VAL = 0xFFFFFFFF;
		SgPMCP->rMAX_VAL = 0;
	}

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}


//默认包含值0的搜索，软件特殊处理。
void  HAL_PMCPSrchMax(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
	HAL_PMCPSrchMax_WithZero(bPmcpIdx, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wStartOff);
}
}

//不包含值0的搜索，硬件不支持。
void  HAL_PMCPSrchMax_WithoutZero(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMax,Error_Param ");
		return;
	}
#endif
	HAL_PMCPSrchMax_Cfg(bPmcpIdx, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wStartOff);
	HAL_PMCPWaitDone(bPmcpIdx);
	}
}

// *****************************************************************************
//函数功能: 搜索内存区中值在 [dMin,dMax]区间内的值
// 入口参数:
//			dMin -> 区间最小值
//			dMax -> 区间最大值
//			dMaskBit	->mask值，bit值为1，表示屏蔽该位,0表示不屏蔽
//			dSrchRamAddr ->搜索目标内存区起始地址
//			bUnitSize ->搜索目标的步长
//			wSrchUnitCnt ->搜索目标的个数(以bUnitSize为单位)
//			dRetValueBuffAddr ->存放搜索结果的偏移
//			wSrchRetCnt ->期望搜索到值的个数
// 出口参数: 实际搜索到的在区间内值的个数
// 说    明:
// *****************************************************************************
void  HAL_PMCPSrchGELE_Cfg(U8 bPmcpIdx, U32 dMin, U32 dMax, U32 dMaskBit, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U16 wSrchRetCnt, U08 bResultStepLen)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchGELE,Error_Param ");
		return ;
	}
#endif
	SG_PMCP_TypeDef *sgPmcp = SG_PMCP(bPmcpIdx);
	sgPmcp->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	sgPmcp->rBASE_ADDR0 = dSrchRamAddr;				//设置源数据位置
	sgPmcp->rRESULT_ADDR = dRetValueBuffAddr;		//统计结果ram位置
	sgPmcp->rOPE_CNT.bits.dVAL = wSrchUnitCnt;		//源数据个数
	sgPmcp->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (bResultStepLen<<BIT_RESULT_STEP_LEN);
	sgPmcp->rBIT_MASK = dMaskBit; // bitmask值低32位
	sgPmcp->rINVALID_VAL = 0; // bitmask值高32位为0
	sgPmcp->rRESULT_CNT = wSrchRetCnt;
	// 区间搜索模式/关闭环形搜索/A<=X<=B/clear使能/Index功能保存使能/关闭保存原值/正向搜索
	sgPmcp->rOPE_MODE |= 0x05 | (2 << 10) | (0 << 12) | (1 << 13);

	if(dMaskBit != 0)
	{
		sgPmcp->rOPE_MODE |= (1 << 9); // bitmask功能使能
	}

	sgPmcp->rMOVE_RLT1_OP_ARRL = 0;		// Clear之后的无效值
	sgPmcp->rMOVE_RLT1_OP_ARRH = 0;
	sgPmcp->rMOVE_RLT0_OP_ARRL = dMin;	// 条件A
	sgPmcp->rMOVE_RLT0_OP_ARRH = 0;
	sgPmcp->rMIN_VAL = dMax; // 条件B
	sgPmcp->rMAX_VAL = 0;

	sgPmcp->rPMCP_CTRL.bits.INT_EN = 0;
	sgPmcp->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
U16  HAL_PMCPSrchGELE(U8 bPmcpIdx, U32 dMin, U32 dMax, U32 dMaskBit, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U16 wSrchRetCnt)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
	SG_PMCP_TypeDef *sgPmcp = SG_PMCP(bPmcpIdx);
    U32 dRet;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchGELE,Error_Param ");
		return 0x55aa;
	}
#endif
	HAL_PMCPSrchGELE_Cfg(bPmcpIdx, dMin, dMax, dMaskBit, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, wSrchRetCnt, 2);
    HAL_PMCPWaitDone(bPmcpIdx);
	dRet = sgPmcp->rGOT_RESULT_CNT.bits.dVAL;
	return (U16)dRet;
}
	else
	{
		return 0;
	}
}

// **********************************区间环形搜索*******************************************
//函数功能: 搜索内存区中值在 [dMin,dMax]区间内的值
// 入口参数:
//			dMin -> 区间最小值
//			dMax -> 区间最大值
//			dMaskBit	->mask值，bit值为1，表示屏蔽该位,0表示不屏蔽
//			dSrchRamAddr ->搜索目标内存区起始地址
//			bUnitSize ->搜索目标的步长
//			wSrchUnitCnt ->搜索目标的个数(以bUnitSize为单位)
//			dRetValueBuffAddr ->存放搜索结果的偏移
//			wSrchRetCnt ->期望搜索到值的个数
// 出口参数: 实际搜索到的在区间内值的个数
// 说    明:
// *****************************************************************************
void  HAL_PMCPSrchGELE_Cir_Cfg(U8 bPmcpIdx, U32 dMin, U32 dMax, U32 dMaskBit, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U16 wSrchRetCnt, U32 dCirIndex)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchGELE,Error_Param ");
		return ;
	}
#endif
	SG_PMCP_TypeDef *sgPmcp = SG_PMCP(bPmcpIdx);
	sgPmcp->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	sgPmcp->rBASE_ADDR0 = dSrchRamAddr;					//设置源数据位置
	sgPmcp->rRMD0_ADDR = dCirIndex;					// 环形搜索的搜索起始index
	sgPmcp->rRESULT_ADDR = dRetValueBuffAddr;			//统计结果ram位置
	sgPmcp->rOPE_CNT.bits.dVAL = wSrchUnitCnt;			//源数据个数
	sgPmcp->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (2<<BIT_RESULT_STEP_LEN);
	sgPmcp->rBIT_MASK = dMaskBit; // bitmask值低32位
	sgPmcp->rINVALID_VAL = 0; // bitmask值高32位为0
	sgPmcp->rRESULT_CNT = wSrchRetCnt;
	// 区间搜索模式/环形搜索使能/A<=X<=B/clear使能/Index功能保存使能/关闭保存原值/正向搜索
	sgPmcp->rOPE_MODE |= 0x05 | (1 << 8) | (2 << 10) | (0 << 12) | (1 << 13);

	if(dMaskBit != 0)
	{
		sgPmcp->rOPE_MODE |= (1 << 9); // bitmask功能使能
	}

	sgPmcp->rMOVE_RLT1_OP_ARRL = 0;		// Clear之后的无效值
	sgPmcp->rMOVE_RLT1_OP_ARRH = 0;
	sgPmcp->rMOVE_RLT0_OP_ARRL = dMin;	// 条件A
	sgPmcp->rMOVE_RLT0_OP_ARRH = 0;
	sgPmcp->rMIN_VAL = dMax; // 条件B
	sgPmcp->rMAX_VAL = 0;

	sgPmcp->rPMCP_CTRL.bits.INT_EN = 0;
	sgPmcp->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
U16  HAL_PMCPSrchGELE_Cir(U8 bPmcpIdx, U32 dMin, U32 dMax, U32 dMaskBit, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U16 wSrchRetCnt, U32 dCirIndex)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
	SG_PMCP_TypeDef *sgPmcp = SG_PMCP(bPmcpIdx);
    U32 dRet;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchGELE,Error_Param ");
		return 0x55aa;
	}
#endif
	HAL_PMCPSrchGELE_Cir_Cfg(bPmcpIdx, dMin, dMax, dMaskBit, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, wSrchRetCnt, dCirIndex);
    HAL_PMCPWaitDone(bPmcpIdx);
	dRet = sgPmcp->rGOT_RESULT_CNT.bits.dVAL;
	return (U16)dRet;
}
	else
	{
		return 0;
	}
}
// *****************************************************************************
//函数功能: 在内存中所搜指定值(搜索单元为2或者4Byte)
// 入口参数:
//			dSrchRamAddr -> 内存区起始地址
//			wSrchUnitCnt ->搜索单元的个数
//			bKey ->指定需要搜索的值
// 出口参数:搜索到的值的偏移（2Byte为单位）
// 说    明:
// *****************************************************************************
U16  HAL_PMCPSrchKeyU08(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt, U32 dKey)
{
	if(wSrchUnitCnt != 0)
	{
	U16 wRet;
	__attribute__((aligned(2))) U32 dDesIdxBuf;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchKeyU08,Error_Param ");
		return 0x55aa;
	}
#endif
	wRet = HAL_PMCPSrchGELE(bPmcpIdx, dKey,dKey,0,dSrchRamAddr,bUnitSize,wSrchUnitCnt,
							(U32)&dDesIdxBuf,1);
	if(wRet == 0)
		return 0xffff;
	else
		return ((U16)dDesIdxBuf);
	}
	else
	{
		return 0xffff;
	}
}



// *****************************************************************************
// 函数功能: 查找相同段内的数据
// 入口参数:
//			dSrchMemAddr	->查找目标数据起始地址
//			bSizeOfUnit		->查找目标数据步长
//			wUnitCnt		->查找目标数据的个数(以bSizeOfUnit为单位)
//			dIndxValueMemAddr 	->存放相同段数据Index的起始buff
//			dSegCntValueMemAddr	->存放相同段数据cnt的起始buff
//			dBitMask		->屏蔽值,bit位为1表示屏蔽
//			dInvalidValue	->已找到的数据所设定的值

// 出口参数: 返回总的数据段的个数
// 备注说明: 暂固定索引项的大小为2B, 个数大小也为2B，可通过多次调用 PMCP_SrchGELE
// *****************************************************************************
U16  HAL_PMCPSrchSameSegData(U32 dSrchMemAddr, U08 bSizeOfUnit, U16 wUnitCnt, U32 dIndxValueMemAddr, U32 dSegCntValueMemAddr, U32 dBitMask, U32 dInvalidValue)
{
	return 1;
}

// *****************************************************************************
//函数功能: 搜索某个值是否在某个区间段内
// 入口参数:
//			dSrchValue -> 需要搜索的值
//			dSrchStartValBuff ->搜索目标区间起始地址所在的buff
//			dSrchLenBuff ->搜索目标区间长度所在的buff，步长为2
//			wRetCnt ->期望搜索到的区间的个数
//			dSrchUnitCnt ->目标区间的个数
//			dResultBuff ->结果数据存储地址，步长为4
// 出口参数: 返回搜索到的第一个区间段偏移，0xffff表示没有搜索到区间段
// 说    明:
// *****************************************************************************

#if 1
void HAL_PMCPSearchSegment_Cfg(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff)
{
	if((dSrchUnitCnt != 0) && (wRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchStartValBuff&(4-1))!=0) || ((dSrchLenBuff&(2-1))!=0) || ((dDesBuff&(4-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSearchSegment,Error_Param ");
		return ;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;
	SgPMCP->rBASE_ADDR0 = dSrchLenBuff;				//设置区间长度组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrchStartValBuff;		//设置数据区间组的起始地址
	SgPMCP->rRMD0_ADDR = 0x490000;
	SgPMCP->rRESULT_ADDR = dDesBuff;				//统计结果存放的地址
	SgPMCP->rOPE_CNT.bits.dVAL = dSrchUnitCnt;		//数据区间的个数
	SgPMCP->rOPE_CSR.dAll = (2<<BIT_STEP_LEN) | (4<<BIT_RESULT_STEP_LEN);
	SgPMCP->rRESULT_CNT = wRetCnt;
	SgPMCP->rOPE_MODE |= 0x0C | (0 << 11);
	SgPMCP->rMIN_VAL = dSrchValue;		//搜索的目标值
	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

U16 HAL_PMCPSearchSegment(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff)
{
	if((dSrchUnitCnt != 0) && (wRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchStartValBuff&(4-1))!=0) || ((dSrchLenBuff&(2-1))!=0) || ((dDesBuff&(4-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSearchSegment,Error_Param ");
		return 0x55aa;
	}
#endif
#if 1
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	HAL_PMCPSearchSegment_Cfg(bPmcpIdx, dSrchValue, dSrchStartValBuff, dSrchLenBuff, wRetCnt, dSrchUnitCnt, dDesBuff);
    HAL_PMCPWaitDone(bPmcpIdx);

	if(SgPMCP->rGOT_RESULT_CNT.bits.dVAL == 0)
		return 0xFFFF;
	else
		return *(U16 *)dDesBuff;
#else
	U32 idx;
	U16 wSrchRetCnt = 0;
	for (idx = 0;idx<dSrchUnitCnt;idx++)
	{
		if (( dSrchValue >=((U32*)dSrchStartValBuff)[idx]) &&
			(dSrchValue<((U32*)dSrchStartValBuff)[idx]+((U16*)dSrchLenBuff)[idx]))
		{
			((U32*)dDesBuff)[wSrchRetCnt++] = idx;
			if (wSrchRetCnt>=wRetCnt)
			{
				break;
			}
		}
	}
	if(wSrchRetCnt == 0)
		return 0xFFFF;
	else
		return *(U16 *)dDesBuff;
#endif
}
	else
	{
		return 0xFFFF;
	}
}
#else

void HAL_PMCPSearchSegment_Cfg(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff)
{
	if((dSrchUnitCnt != 0) && (wRetCnt != 0))
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;
	SgPMCP->rBASE_ADDR0 = dSrchLenBuff;				//设置区间长度组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrchStartValBuff;		//设置数据区间组的起始地址
	SgPMCP->rRESULT_ADDR = dDesBuff;				//统计结果存放的地址
	SgPMCP->rOPE_CNT.bits.dVAL = dSrchUnitCnt;		//数据区间的个数
	SgPMCP->rOPE_CSR.dAll = (2<<BIT_STEP_LEN) | (4<<BIT_RESULT_STEP_LEN);
	SgPMCP->rRESULT_CNT = wRetCnt;
	SgPMCP->rOPE_MODE |= 0x0C | (0 << 11);
	SgPMCP->rMIN_VAL = dSrchValue;		//搜索的目标值
	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

U16 HAL_PMCPSearchSegment1(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff)
{
	if((dSrchUnitCnt != 0) && (wRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchStartValBuff&(4-1))!=0) || ((dSrchLenBuff&(2-1))!=0) || ((dDesBuff&(4-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSearchSegment,Error_Param ");
		return 0x55aa;
	}
#endif
#if 1
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	HAL_PMCPSearchSegment_Cfg(bPmcpIdx, dSrchValue, dSrchStartValBuff, dSrchLenBuff, wRetCnt, dSrchUnitCnt, dDesBuff);
    HAL_PMCPWaitDone(bPmcpIdx);

	if(SgPMCP->rGOT_RESULT_CNT.bits.dVAL == 0)
		return 0xFFFF;
	else
		return *(U16 *)dDesBuff;
#else
	U32 idx;
	U16 wSrchRetCnt = 0;
	for (idx = 0;idx<dSrchUnitCnt;idx++)
	{
		if (( dSrchValue >=((U32*)dSrchStartValBuff)[idx]) &&
			(dSrchValue<((U32*)dSrchStartValBuff)[idx]+((U16*)dSrchLenBuff)[idx]))
		{
			((U32*)dDesBuff)[wSrchRetCnt++] = idx;
			if (wSrchRetCnt>=wRetCnt)
			{
				break;
			}
		}
	}
	if(wSrchRetCnt == 0)
		return 0xFFFF;
	else
		return *(U16 *)dDesBuff;
#endif
}
	else
	{
		return 0xFFFF;
	}
}
U16 PMCP_SearchSegment_V(void)
{
	U32 idx;
	U16 wRetCnt = 0;



	if(PMCP_bSchSteplen == 2)
	{
		for (idx = 0;idx<PMCP_dSchOpecnt;idx++)
		{
			if (( PMCP_dSchMinval >=((U32*)PMCP_dSchBaseaddr1)[idx]) &&
				(PMCP_dSchMinval<((U32*)PMCP_dSchBaseaddr1)[idx]+((U16*)PMCP_dSchBaseaddr0)[idx]))
			{
				((U32*)PMCP_dSchResultaddr)[wRetCnt++] = idx;
				if (wRetCnt>=PMCP_wSchResultcnt)
				{
					break;
				}
			}
		}
	}
	else
	{
		//PRINT_TXT("SearchSegment param error");
	}
	//SG_PMCP(0)->rRESULT_CNT.dAll = wRetCnt;
	//rPMCP_RESULT_CNT = wRetCnt;
	return wRetCnt;
}
U16 HAL_PMCPSearchSegment(U8 bPMCPIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt,U32 dDesBuff)
{

	U32 drelut;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchStartValBuff%8)!=0) || ((dSrchLenBuff%8)!=0) || ((dDesBuff%8)!=0) || (bPMCPIdx>1))
	{
		PRINT_TXT("HAL_PMCPSearchSegment,Error_Param");
		return 0x55aa;
	}
#endif
	PMCP_dSchBaseaddr0 = (U32)dSrchLenBuff;
	PMCP_dSchBaseaddr1 = (U32)dSrchStartValBuff;
	PMCP_dSchResultaddr = dDesBuff;//((U32)(&PxSearchResult[0]));
	PMCP_wSchResultcnt = wRetCnt; //返回结果
	PMCP_bSchSteplen = 2;//步长
	PMCP_dSchMinval =dSrchValue;
	PMCP_dSchOpecnt = dSrchUnitCnt;

	if(0 == PMCP_dSchOpecnt)     //PX搜索，若搜索个数为0，则不会进行搜索，但搜索结果值不会为0，保持原来的值不变。
	{
		drelut = 0;
	}
	else
	{
		drelut = PMCP_SearchSegment_V();//只搜索一个相同的
		//drelut = SG_PMCP(0)->rRESULT_CNT.dAll;
		//drelut =rPMCP_RESULT_CNT; //px搜索结果个数
	}

	if(drelut > 0)//在临时映射表中//
	{
		return   ((U16*)dDesBuff)[0]; //(U16)(PxSearchResult[0]);
	}
	else
		return 0XFFFF;
}
#endif
U16 HAL_SW_SearchSegment(U8 bPmcpIdx, U32 dSrchValue,U32 dSrchStartValBuff,U32 dSrchLenBuff,U16 wRetCnt,U32 dSrchUnitCnt, U32 dDesBuff)
{
	if((wRetCnt != 0) && (dSrchUnitCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchStartValBuff&(4-1))!=0) || ((dSrchLenBuff&(2-1))!=0) || ((dDesBuff&(4-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSearchSegment,Error_Param ");
		return 0x55aa;
	}
#endif
	U32 idx;
	U16 wSrchRetCnt = 0;
	for (idx = 0;idx<dSrchUnitCnt;idx++)
	{
		if (( dSrchValue >=((U32*)dSrchStartValBuff)[idx]) &&
			(dSrchValue<((U32*)dSrchStartValBuff)[idx]+((U16*)dSrchLenBuff)[idx]))
		{
			((U32*)dDesBuff)[wSrchRetCnt++] = idx;
			if (wSrchRetCnt>=wRetCnt)
			{
				break;
			}
		}
	}
	if(wSrchRetCnt == 0)
		return 0xFFFF;
	else
		return *(U16 *)dDesBuff;
}
}

// *****************************************************************************
//函数功能: 带跳列的BIT图统计 统计指定区域内 bit 为1的个数
// 入口参数:
//			dSrcAddr -> 统计的目标内存起始地址
//			dRetAddr ->存储统计结果的buff，步长固定为2Byte
//			dSrchUnitSize ->每个统计单元的大小（单位为1Byte,必须4Byte对齐）
//			dSrchUnitCnt ->统计单元的个数
//			wSeqUnitCnt		->连续数据段的个数
//			wfilterLen		->数据段之间的filter长度（单位为1Byte,必须4Byte对齐），wfilterLen为0表示无跳列

// 出口参数:
// 说    明:
// *****************************************************************************
void  HAL_PMCPBitMapCountExtFilter_Cfg(U8 bPmcpIdx, U32 dSrcAddr,U32 dRetAddr,U32 dSrchUnitSize,U32 dSrchUnitCnt, U16 wSeqUnitCnt, U16 wfilterLen,U8 bResult_Step)
{
	if(dSrchUnitCnt != 0)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrcAddr&(4-1))!=0) || ((dRetAddr&(bResult_Step-1))!=0) || ((wfilterLen&(4-1))!=0) || ((dSrchUnitSize&(4-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPBitMapCountExtFilter,Error_Param ");
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrcAddr;					//BIT统计源数据的起始地址
	SgPMCP->rRESULT_ADDR = dRetAddr;				//统计结果存放的地址
	SgPMCP->rOPE_CNT.bits.dVAL = dSrchUnitSize/4;	//统计单元的长度，以double word为单位
	SgPMCP->rOPE_CSR.dAll = (bResult_Step<<BIT_RESULT_STEP_LEN) | (wSeqUnitCnt<<BIT_DIFF_BIT_NUM);
	SgPMCP->rRESULT_CNT = dSrchUnitCnt; // 统计单元个数
	SgPMCP->rMIN_VAL = wfilterLen/4;	//每隔wSeqUnitCnt个数据段需要跳列的长度，4byte单位
	SgPMCP->rOPE_MODE = 0;	//带跳列的bit统计

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;					//enable int
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
void  HAL_PMCPBitMapCountExtFilter(U8 bPmcpIdx, U32 dSrcAddr,U32 dRetAddr,U32 dSrchUnitSize,U32 dSrchUnitCnt, U16 wSeqUnitCnt, U16 wfilterLen,U8 bResult_Step)
{
	if(dSrchUnitCnt != 0)
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrcAddr&(4-1))!=0) || ((dRetAddr&(bResult_Step-1))!=0) || ((wfilterLen&(4-1))!=0) || ((dSrchUnitSize&(4-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPBitMapCountExtFilter,Error_Param ");
		return;
	}
#endif
	HAL_PMCPBitMapCountExtFilter_Cfg(bPmcpIdx, dSrcAddr, dRetAddr, dSrchUnitSize, dSrchUnitCnt, wSeqUnitCnt, wfilterLen,bResult_Step);
	HAL_PMCPWaitDone(bPmcpIdx);
	}
}

// *****************************************************************************
//函数功能: 区间段与区间段的覆盖情况搜索
// 入口参数:
//	dSrchEaraStart	->需要搜索区间的开始地址
//	dSrchEaraLen	->需要搜索区间长度
//	dSrchDestStartBuff	->搜索目标每个区间开始地址所在的buff
//	bSrchDestStartStep	->搜索目标每个区间开始地址步长,固定为 4 B
//	dSrchDestLenBuff	->搜索目标每个区间长度所在的buff
//	bSrchDestLenStep	->->搜索目标每个区间长度步长
//	bSrchDestCnt	->搜索目标区间的个数
//	dRetOffBuff	->返回搜索到的目标区间引索的buff，步长固定为2Byte
//	dRetCoverInf	->返回搜索到的目标区间的覆盖情况的buff: 0->左边覆盖，1->右边覆盖,2->目标区间被全覆盖,3->需要搜索区间被全覆盖，步长可固定为1Byte

// 出口参数: 返回有覆盖的区间段个数
// 说    明:
// *****************************************************************************
void HAL_PMCP_SearchCover_Cfg(U8 bPmcpIdx, U32 dSrchEaraStart, U32 dSrchEaraLen, U32 dSrchDestStartBuff, U08 bSrchDestStartStep, U32 dSrchDestLenBuff, U08 bSrchDestLenStep,
	U16 bSrchDestCnt, U32 dRetOffBuff, U32 dRetCoverInf)
{
	if((bSrchDestCnt != 0) && (dSrchEaraLen != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchDestStartBuff&(bSrchDestStartStep-1))!=0) || ((dSrchDestLenBuff&(bSrchDestLenStep-1))!=0) ||
		((dRetOffBuff&(2-1))!=0) || !((bSrchDestLenStep==2) || (bSrchDestLenStep==4) || (bSrchDestLenStep==1)) || (bPmcpIdx>1) || (bSrchDestStartStep != 4))
	{
		PRINT_TXT("HAL_PMCP_SearchCover,Error_Param ");
		return ;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchDestLenBuff;		//设置区间长度组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrchDestStartBuff;	//设置数据区间组的起始地址
	SgPMCP->rRESULT_ADDR = dRetOffBuff;			//统计结果存放的地址
	SgPMCP->rBIT_MASK = dRetCoverInf;			//覆盖方式的存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = bSrchDestCnt;	//数据区间的个数
	SgPMCP->rOPE_CSR.dAll = (bSrchDestLenStep<<BIT_STEP_LEN) | (2<<BIT_RESULT_STEP_LEN);
	SgPMCP->rRESULT_CNT = bSrchDestCnt; // 期待结果个数设置为区间个数
	SgPMCP->rOPE_MODE |= 0x0D ;			//数据段间覆盖搜索模式/正向搜索/待搜索区间长度恒为1模式关闭
	SgPMCP->rMIN_VAL = dSrchEaraStart;	//搜索的区间起始值
	SgPMCP->rMAX_VAL = dSrchEaraLen;	//搜索的区间长度值

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
	}
}


U32 PMCP_GetValue(U32 dAddr, U32 dIndex, U32 dElemStep)
{
	U32 dRet = 0;
	if (1 == dElemStep)
	{
		dRet = (*((U8 *)dAddr+dIndex));
	}
	else if (2 == dElemStep)
	{
		dRet = (*((U16 *)dAddr+dIndex));
	}
	else if (4 == dElemStep)
	{
		dRet = (*((U32 *)dAddr+dIndex));
	}
	return dRet;
}

U16 HAL_PMCP_SearchCover(U8 bPmcpIdx, U32 dSrchEaraStart, 		//需要搜索区间的开始地址
    U32 dSrchEaraLen,			//需要搜索区间长度
    U32 dSrchDestStartBuff,	//搜索目标每个区间开始地址所在的buff，步长可配置为4 Byte
    U08 bSrchDestStartStep,
    U32 dSrchDestLenBuff,		//搜索目标每个区间长度所在的buff，步长可配置为1,2，4 Byte
    U08 bSrchDestLenStep,
    U16 bSrchDestCnt,           //搜索目标区间的个数
    U32 dRetOffBuff,			//返回搜索到的目标区间引索的buff，步长可配置为2，4 Byte
    U32 dRetCoverInf			//返回搜索到的目标区间的覆盖情况的buff: 0->左边覆盖，1->右边覆盖,2->目标区间被全覆盖,3->需要搜索区间被全覆盖，步长可固定为1Byte
    )
{
if((dSrchEaraLen != 0) && (bSrchDestCnt != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchDestStartBuff&(bSrchDestStartStep-1))!=0) || ((dSrchDestLenBuff&(bSrchDestLenStep-1))!=0) ||
		((dRetOffBuff&(2-1))!=0) || !((bSrchDestLenStep==2) || (bSrchDestLenStep==4) || (bSrchDestLenStep==1)) || (bPmcpIdx>1) || (bSrchDestStartStep != 4))
	{
		PRINT_TXT("HAL_PMCP_SearchCover,Error_Param ");
		return 0x55aa;
	}
#endif
#if 1
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	HAL_PMCP_SearchCover_Cfg(bPmcpIdx, dSrchEaraStart, dSrchEaraLen, dSrchDestStartBuff, bSrchDestStartStep, dSrchDestLenBuff, bSrchDestLenStep,
		 bSrchDestCnt, dRetOffBuff, dRetCoverInf);
    HAL_PMCPWaitDone(bPmcpIdx);
	return (U16)SgPMCP->rGOT_RESULT_CNT.bits.dVAL ;
#else
	U32 j=0;
	U32 i;
	for (i=0; i<bSrchDestCnt; i++)
	{
		U32 dStart = ((U32 *)dSrchDestStartBuff)[i];		//区间的起始数据值
		U32 dLen = PMCP_GetValue(dSrchDestLenBuff, i, bSrchDestLenStep);
		if (dStart < dSrchEaraStart && (dStart+dLen-1) >= dSrchEaraStart)
		{
			*(((U16 *)dRetOffBuff)+j) = i;							//结果步长固定为2
			if ((dStart+dLen-1) > (dSrchEaraStart+dSrchEaraLen-1))	//被包含
			{
				*(((U08 *)dRetCoverInf)+j) = BE_INVOLVE;
			}
			else
			{
				*(((U08 *)dRetCoverInf)+j) = LEFT_INTERSECTION;
			}
			j++;
		}
		else if (dStart >= dSrchEaraStart && dStart <= (dSrchEaraStart+dSrchEaraLen-1) && (dStart+dLen-1)>(dSrchEaraStart+dSrchEaraLen-1))
		{
			*(((U16 *)dRetOffBuff)+j) = i;							//结果步长固定为2
			*(((U08 *)dRetCoverInf)+j) = RIGHT_INTERSECTION;
			j++;
		}
		else if (dStart >= dSrchEaraStart && (dStart+dLen-1)<= (dSrchEaraStart+dSrchEaraLen-1))
		{
			*(((U16 *)dRetOffBuff)+j) = i;							//结果步长固定为2
			*(((U08 *)dRetCoverInf)+j) = INVOLVE;
			j++;
		}
	}
	return j;		//返回结果个数
#endif
}
else
{
	return 0;
}
}

U16 HAL_SW_SearchCover(U8 bPmcpIdx, U32 dSrchEaraStart, 		//需要搜索区间的开始地址
    U32 dSrchEaraLen,			//需要搜索区间长度
    U32 dSrchDestStartBuff,	//搜索目标每个区间开始地址所在的buff，步长可配置为4 Byte
    U08 bSrchDestStartStep,
    U32 dSrchDestLenBuff,		//搜索目标每个区间长度所在的buff，步长可配置为1,2，4 Byte
    U08 bSrchDestLenStep,
    U16 bSrchDestCnt,           //搜索目标区间的个数
    U32 dRetOffBuff,			//返回搜索到的目标区间引索的buff，步长可配置为2，4 Byte
    U32 dRetCoverInf			//返回搜索到的目标区间的覆盖情况的buff: 0->左边覆盖，1->右边覆盖,2->目标区间被全覆盖,3->需要搜索区间被全覆盖，步长可固定为1Byte
    )
{
	if((dSrchEaraLen != 0) && (bSrchDestCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchDestStartBuff&(bSrchDestStartStep-1))!=0) || ((dSrchDestLenBuff&(bSrchDestLenStep-1))!=0) ||
		((dRetOffBuff&(2-1))!=0) || !((bSrchDestLenStep==2) || (bSrchDestLenStep==4) || (bSrchDestLenStep==1)) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCP_SearchCover,Error_Param ");
		return 0x55aa;
	}
#endif

	U32 j=0;
	U32 i;
	for (i=0; i<bSrchDestCnt; i++)
	{
		U32 dStart = ((U32 *)dSrchDestStartBuff)[i];		//区间的起始数据值
		U32 dLen = PMCP_GetValue(dSrchDestLenBuff, i, bSrchDestLenStep);
		if (dStart < dSrchEaraStart && (dStart+dLen-1) >= dSrchEaraStart)
		{
			*(((U16 *)dRetOffBuff)+j) = i;							//结果步长固定为2
			if ((dStart+dLen-1) > (dSrchEaraStart+dSrchEaraLen-1))	//被包含
			{
				*(((U08 *)dRetCoverInf)+j) = BE_INVOLVE;
			}
			else
			{
				*(((U08 *)dRetCoverInf)+j) = LEFT_INTERSECTION;
			}
			j++;
		}
		else if (dStart >= dSrchEaraStart && dStart <= (dSrchEaraStart+dSrchEaraLen-1) && (dStart+dLen-1)>(dSrchEaraStart+dSrchEaraLen-1))
		{
			*(((U16 *)dRetOffBuff)+j) = i;							//结果步长固定为2
			*(((U08 *)dRetCoverInf)+j) = RIGHT_INTERSECTION;
			j++;
		}
		else if (dStart >= dSrchEaraStart && (dStart+dLen-1)<= (dSrchEaraStart+dSrchEaraLen-1))
		{
			*(((U16 *)dRetOffBuff)+j) = i;							//结果步长固定为2
			*(((U08 *)dRetCoverInf)+j) = INVOLVE;
			j++;
		}

	}
	return j;		//返回结果个数
}
	else
	{
		return 0;
	}
}
// *****************************************************************************
//函数功能: 带清零功能的bit搜索（搜索bit值为1）
// 入口参数:
//	dSrchMemAddr	->需要搜索内存区的开始地址
//	wSrchMemLen	->需要搜索内存区长度（（单位为1Byte,必须4Byte对齐））
//	dIndexBuff	->存放搜索到的bit的偏移的buff，步长固定位2Byte
//	wSeachCnt	->期望搜索到的bit个数
//  bEnableClearBit ->是否使能Clear, 1使能, 0不使能
// 出口参数: 返回有实际搜索到的bit数
// 说    明:
// *****************************************************************************
void  HAL_PMCPSrchValidBits_Cfg(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dIndexBuff,U16 wSeachCnt, U08 bResultStep, U08 bEnableClearBit)
{
	if((wSrchMemLen != 0) && (wSeachCnt != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchMemAddr&(4-1))!=0) || ((wSrchMemLen&(4-1))!=0) || ((dIndexBuff&(bResultStep-1))!=0) || (bPmcpIdx>1) || (bEnableClearBit>1))
	{
		PRINT_TXT("HAL_PMCPSrchValidBits,Error_Param1 ");
		return ;
	}
	if(!((1==bResultStep) || (2==bResultStep) || (4==bResultStep)))
	{
		PRINT_TXT("HAL_PMCPSrchValidBits,Error_Param2 ");
		return ;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchMemAddr;		//BIT搜索源数据的起始地址
	SgPMCP->rRESULT_ADDR = dIndexBuff;		//统计结果存放的地址
	SgPMCP->rOPE_CNT.bits.dVAL = wSrchMemLen/4;				//以double word为单位
	SgPMCP->rOPE_CSR.dAll = (bResultStep << BIT_STEP_LEN);
	SgPMCP->rRESULT_CNT = wSeachCnt; // 期望搜索到的bit个数
	SgPMCP->rOPE_MODE |= 0x20 | (bEnableClearBit << 8);		//带bit清零的bit搜索模式/是否清零/输出结果从0开始计数
	SgPMCP->rBASE_ADDR1 = 0; // 从第0个bit开始搜索
	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;					//Enable Int
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
U16  HAL_PMCPSrchValidBits(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dIndexBuff,U16 wSeachCnt, U08 bResultStep, U08 bEnableClearBit)
{
	if((wSrchMemLen != 0) && (wSeachCnt != 0))
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);

#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchMemAddr&(4-1))!=0) || ((wSrchMemLen&(4-1))!=0) || ((dIndexBuff&(bResultStep-1))!=0) || (bPmcpIdx>1) || (bEnableClearBit>1))
	{
		PRINT_TXT("HAL_PMCPSrchValidBits,Error_Param1 ");
		return 0x55aa;
	}
	if(!((1==bResultStep) || (2==bResultStep) || (4==bResultStep)))
	{
		PRINT_TXT("HAL_PMCPSrchValidBits,Error_Param2 ");
		return 0x55aa;
	}
#endif
	HAL_PMCPSrchValidBits_Cfg(bPmcpIdx, dSrchMemAddr, wSrchMemLen, dIndexBuff, wSeachCnt, bResultStep, bEnableClearBit);
	HAL_PMCPWaitDone(bPmcpIdx);
	return (U16)SgPMCP->rGOT_RESULT_CNT.bits.dVAL ;
}
	else
	{
		return 0;
	}
}
// *****************************************************************************
//函数功能: 矩阵移位
// 入口参数:
//	dSrchMemAddr	->矩阵区的开始地址
//	bUnitSize	->矩阵步长
//	wUnitCnt	->矩阵内单元个数
//	dDestMemAddr	->存放移位后的buff
//	bMode	->1表示左移，0表示右移
//	bShiftBitCnt ->所移的位数
//	dMaskBit ->mask位，1有效

// 出口参数:
// 说    明:仅支持PMCP0
// *****************************************************************************
void HAL_PMCPArraShiftBitsMask_Cfg(U8 bPmcpIdx, U32 dSrchMemAddr, U08 bUnitSize,U16 wUnitCnt,U32 dDestMemAddr,U08 bRetUnitSize,U08 bMode,U08 bShiftBitCnt,U32 dMaskBit)
{
	if(wUnitCnt != 0)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchMemAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dDestMemAddr&(bRetUnitSize-1))!=0) ||
		!((bRetUnitSize==2) || (bRetUnitSize==4)) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPArraShiftBitsMask,Error_Param ");
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchMemAddr;		//矩阵基地址
	SgPMCP->rRESULT_ADDR = dDestMemAddr;	//结果存放地址
	SgPMCP->rOPE_MODE |= 0x33 | (bMode << 8); // 矩阵移位功能/移位方向
	SgPMCP->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (bRetUnitSize<<BIT_RESULT_STEP_LEN);
	SgPMCP->rOPE_CNT.bits.dVAL = wUnitCnt;	//矩阵元素个数
	SgPMCP->rMIN_VAL = bShiftBitCnt;		//移位数

	if(dMaskBit != 0)
	{
		SgPMCP->rOPE_MODE |= (1 << 9);
		SgPMCP->rBIT_MASK = dMaskBit; // bitmask值
		SgPMCP->rBIT_MASK2 = 0;
	}

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;									//Start Pmcp
	__nds32__dsb();
}
}

void HAL_PMCPArraShiftBitsMask(U8 bPmcpIdx, U32 dSrchMemAddr, U08 bUnitSize,U16 wUnitCnt,U32 dDestMemAddr,U08 bRetUnitSize,U08 bMode,U08 bShiftBitCnt,U32 dMaskBit)
{
	if(wUnitCnt != 0)
	{
#if 1
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchMemAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dDestMemAddr&(bRetUnitSize-1))!=0) ||
		!((bRetUnitSize==2) || (bRetUnitSize==4)) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPArraShiftBitsMask,Error_Param ");
		return;
	}
#endif
	HAL_PMCPArraShiftBitsMask_Cfg(bPmcpIdx, dSrchMemAddr, bUnitSize, wUnitCnt, dDestMemAddr, bRetUnitSize, bMode, bShiftBitCnt, dMaskBit);
	HAL_PMCPWaitDone(bPmcpIdx);
#else
    U16 bNum;
    //U8  bTmp;
    U8 *pbSrchData ;
    U16 *pwSrchData;
    U32 *pdSrchData,dSrcVal;
    U32 dRet;
	dMaskBit = (~dMaskBit);
    for (bNum=0;bNum<wUnitCnt;bNum++)
    {
        if(bUnitSize == 1){
            pbSrchData = (U8 *)dSrchMemAddr;
            dSrcVal = pbSrchData[bNum] & dMaskBit;

        }
        else if(bUnitSize == 2){
            pwSrchData = (U16 *)dSrchMemAddr;
            dSrcVal = pwSrchData[bNum] & dMaskBit;
        }
        else if(bUnitSize == 4){
            pdSrchData = (U32 *)dSrchMemAddr;
            dSrcVal = pdSrchData[bNum] & dMaskBit;
        }
        else
        {
        	dSrcVal = 0;
        }

        if(0 == bMode)
        {
            dRet = dSrcVal>>bShiftBitCnt;
        }
        else
        {
            dRet = dSrcVal<<bShiftBitCnt;
        }

        if(bRetUnitSize == 1){
            pbSrchData = (U8 *)dDestMemAddr;
            pbSrchData[bNum] = (U08)dRet;

        }
        else if(bRetUnitSize == 2){
            pwSrchData = (U16 *)dDestMemAddr;
            pwSrchData[bNum] = (U16)dRet;
        }
        else if(bRetUnitSize == 4){
            pdSrchData = (U32 *)dDestMemAddr;
            pdSrchData[bNum] = (U32)dRet;
        }
    }
#endif
}
}

// *****************************************************************************
//函数功能: 矩阵与固定数相加
// 入口参数:
//			dSrchRamAddr -> 内存区起始地址
//			bUnitSize ->矩阵步长
//			wUnitLen ->矩阵的长度（以bUnitSize为单位）
//			dValue ->相加的固定数

// 出口参数:
// 说    明:该接口使用的是PMCP0，但该功能同时支持PMCP0和PMCP1。使用HAL_PMCPMatrixValueOp接口可同时支持
// *****************************************************************************
void HAL_PMCP0ArraPlus(U32 dSrchRamAddr,U08 bUnitSize,U16 wUnitLen,U32 dValue)
{
	if(wUnitLen != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
    U32 dTimeCnt = 0;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)))
	{
		PRINT_TXT("HAL_PMCPArraPlus,Error_Param ");
		return;
	}
#endif
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchRamAddr;			//矩阵基地址
	SgPMCP->rRESULT_ADDR = dSrchRamAddr;		//结果存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = wUnitLen;		//矩阵元素个数
	SgPMCP->rOPE_MODE |= 0x13 | (4 << 12);		//矩阵与固定数的模式/运算模式为加/关闭bitmask
	SgPMCP->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (bUnitSize<<BIT_RESULT_STEP_LEN); // 矩阵/结果元素步长
	SgPMCP->rMAX_VAL = (U32)dValue;					//固定数值

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;									//Start Pmcp

	__nds32__dsb();
	while(!SgPMCP->rOP_START.bits.dVAL)
    {
        dTimeCnt ++;
        if (0 == (dTimeCnt % 0xFFFFFF))
        {
            //LOG_ERROR("PMCP Sch Time out Ch:%x");
            break;
        }
        }
    }

}

#if 0
#define R_PMCP_DOING            (1)   //读数据，返回1:PMCP操作完成；0:PMCP正在操作
#define R_PMCP_DIV_START        (1)   //write 1 :开启PMCPDIV操作 //read:读取操作状态 0：完成 1：进行中

/****************************************************************************************************
// 函数功能: 求商
// 入口参数: U32 dDividend: 被除数 
//           U32 dDivisor : 除数
// 出口参数: U32 : 商
// 说    明: 
*****************************************************************************************************/
U32 PMCP_Divider_Quo(U32 dDividend, U32 dDivisor, U8 bResLen)
{  
    SG_PMCP(0)->rDIVIDEND = dDividend;
    SG_PMCP(0)->rDIVISOR  = dDivisor;
    
    SG_PMCP(0)->rPMCP_DIV_CTRL.dAll =  bResLen | 0x1;
   
    Delay(1); while (R_PMCP_DIV_START !=(SG_PMCP(0)->rPMCP_DIV_CTRL.dAll & R_PMCP_DIV_START)) {}
    
    return (SG_PMCP(0)->rQUOTIENT);
}

/****************************************************************************************************
// 函数功能: 求余数
// 入口参数: U32 dDividend: 被除数 
//           U32 dDivisor : 除数
// 出口参数: U32 : 余数
// 说    明: 
*****************************************************************************************************/
U32 PMCP_Divider_Rem(U32 dDividend, U32 dDivisor)
{ 
    SG_PMCP(0)->rDIVIDEND = dDividend;
    SG_PMCP(0)->rDIVISOR  = dDivisor;
    
    SG_PMCP(0)->rPMCP_DIV_CTRL.dAll = 0x1;
   
    Delay(1); while (R_PMCP_DIV_START !=(SG_PMCP(0)->rPMCP_DIV_CTRL.dAll & R_PMCP_DIV_START)) {}
    
    return (SG_PMCP(0)->rREMAINDER);

}
#endif

#if 0
// 9086删除了BIT搜索功能

// *****************************************************************************
//函数功能: BIT搜索1
// 入口参数:
//			dSrchRamAddr -> 内存区起始地址
//			wSrchMemLen -> 4byte对齐
//			dBitOneNum -> 搜索第几个1
// 出口参数:
// 说    明:dBitOneNum在PMCP内部默认是从1开始，所以需要加1
// *****************************************************************************
void HAL_PMCPBitSearch1_Cfg(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dBitOneNum)
{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE.dAll = 0;							// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchMemAddr;					//BIT搜索源数据的起始地址
	SgPMCP->rBASE_ADDR1 = dBitOneNum+1;					// 搜索哪一个1
	SgPMCP->rOPE_CNT.bits.dVAL = wSrchMemLen/4;			//以double word为单位
	SgPMCP->rOPE_CSR.dAll = 0;							//
	SgPMCP->rOPE_MODE.bits.OPE_MODE = PX_BIT_SCH1;		//bit 1搜索
	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;					//Enable Int
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
U16 HAL_PMCPBitSearch1(U8 bPmcpIdx, U32 dSrchMemAddr, U16 wSrchMemLen, U32 dBitOneNum)
{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchMemAddr&(8-1))!=0) || ((wSrchMemLen&(4-1))!=0) || (bPmcpIdx>1))
	{
		LOG_PRINT("HAL_PMCPSrchMin,Error_Param ");
		return 0;
	}
#endif
	HAL_PMCPBitSearch1_Cfg(bPmcpIdx, dSrchMemAddr, wSrchMemLen, dBitOneNum);
	HAL_PMCPWaitDone(bPmcpIdx);
	if(1 == SgPMCP->rRESULT_CNT.bits.dVAL)
	{
		return (U16)SgPMCP->rBIT_RESULT.dAll;
	}
	else
	{
		return 0xFFFF;
	}
}
#endif



// *****************************************************************************
//函数功能: 搜索最小值
// 入口参数:
//			dSrchRamAddr -> 搜索目标内存区起始地址
//			bUnitSize ->搜索目标的步长
//			wSrchUnitCnt ->搜索目标的个数(以bUnitSize为单位)
//			dRetValueBuffAddr ->存放搜索到的值的起始地址
//			dRetItemBuffAddr ->存放搜索到值的偏移的起始地址，偏移值的步长固定为2B
//			wSrchRetCnt ->需要搜索的最小值个数
//			wRingOff ->环形搜索的起始偏移，为0表示为普通搜索模式
// 出口参数:
// 说    明:不支持步长为2的情况下，全部bit都被mask；步长为4也同理。
// *****************************************************************************
void HAL_PMCPSrchMinWithMask_Cfg(U8 bPmcpIdx, U32 dSrchRamAddr, U32 dBitMask, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || (0xffffffff==dBitMask) || (0xffff==dBitMask) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMin,Error_Param ");
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrchRamAddr;				//源数据起始地址
	SgPMCP->rBASE_ADDR1 = dRetValueBuffAddr;		//原始值存放地址
	SgPMCP->rRESULT_ADDR = dRetItemBuffAddr;		//统计结果ram位置
	SgPMCP->rOPE_CNT.bits.dVAL = wSrchUnitCnt;		// 源数据个数
	SgPMCP->rOPE_CSR.dAll = (bUnitSize<<BIT_STEP_LEN) | (2<<BIT_RESULT_STEP_LEN); // 配置源数据步长为bUnitSize，配置搜索结果步长为2byte
	SgPMCP->rRESULT_CNT = wSrchRetCnt; // 需要搜索的最小值个数
	SgPMCP->rOPE_MODE |= 0x04 | (1 << 9) | (1 << 13) | (1 << 14); // 最值搜索模式/开启bitmask/最小值搜索/Index保存使能/保存原值使能/正向搜索
	SgPMCP->rBIT_MASK = dBitMask;	// bitmask值低32bit
	SgPMCP->rINVALID_VAL = 0;		// bitmask值高32bit

	if(wRingOff != 0)
	{
		SgPMCP->rOPE_MODE |= (1 << 8); // 环形搜索
		SgPMCP->rRMD0_ADDR = wRingOff;	// 环形搜索起始index
	}
	SgPMCP->rMOVE_RLT0_OP_ARRL = 0;	// 最小阈值为0
	SgPMCP->rMOVE_RLT0_OP_ARRH = 0;

	// 最小值搜索时，最大阈值不能为源数据步长对应的最大值（如源数据步长为2byte，则最大值不能为0xFFFE）
	if(bUnitSize == 2)
	{
		SgPMCP->rMIN_VAL = 0xFFFE;
		SgPMCP->rMAX_VAL = 0;
	}
	else if(bUnitSize == 4)
	{
		SgPMCP->rMIN_VAL = 0xFFFFFFFE;
		SgPMCP->rMAX_VAL = 0;
	}

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

void HAL_PMCPSrchMinWithMask_WithOutFF(U8 bPmcpIdx, U32 dSrchRamAddr, U32 dBitMask, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wRingOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || (0xffffffff==dBitMask) || (0xffff==dBitMask) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMin,Error_Param ");
		return;
	}
#endif
	HAL_PMCPSrchMinWithMask_Cfg(bPmcpIdx, dSrchRamAddr, dBitMask, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wRingOff);
	HAL_PMCPWaitDone(bPmcpIdx);
}
}

void HAL_PMCPSrchMinWithMask(U8 bPmcpIdx, U32 dSrchRamAddr, U32 dBitMask, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0) && (wSrchRetCnt != 0))
		{
#define IDX_RESULT_STEP	2		// 固定结果idx步长为2
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || (0xffffffff==dBitMask) || (0xffff==dBitMask) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMin,Error_Param ");
		return;
	}
#endif
	HAL_PMCPSrchMinWithMask_Cfg(bPmcpIdx, dSrchRamAddr, dBitMask, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wStartOff);
	HAL_PMCPWaitDone(bPmcpIdx);

	if (SgPMCP->rGOT_RESULT_CNT.bits.dVAL < wSrchRetCnt)		// 搜到个数小于期望个数，存在全FF的项
	{
		U16 i, j;
		U16 wIdxUnitCnt = SgPMCP->rGOT_RESULT_CNT.bits.dVAL;
		U08 bReplaceMode = bUnitSize;					// 替换模式等于源数据的步长
		//先用矩阵替换，把最大值搜索到的数据还原
		HAL_PMCPWaitDone(0);
		HAL_PMCP0MemorySetWithIdx_Cfg(dSrchRamAddr, dRetItemBuffAddr, wIdxUnitCnt, IDX_RESULT_STEP, bReplaceMode, 0x55aa55aa);
		HAL_PMCPWaitDone(0);
		PRINT_HEX("End_HAL_PMCP0MemorySetWithIdx,(wSrchRetCnt-wIdxUnitCnt)=%x ", (wSrchRetCnt-wIdxUnitCnt));
		//移位index数据，将前面的若干个元素移到最后面
		//for (i=0; i<wIdxUnitCnt; i++)
		for(i=wIdxUnitCnt; i>0; i--)
		{
			*(U16 *)(dRetItemBuffAddr+((wSrchRetCnt-wIdxUnitCnt)+(i-1))*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16));
			//*(U16 *)(dRetItemBuffAddr+((wSrchRetCnt-wIdxUnitCnt)+i)*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+i*sizeof(U16));
			//PRINT_HEX("i=%d, HAL_PMCPSrchGELE,*(U16 *)(dRetItemBuffAddr+i*sizeof(U16)=%x ",i, *(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16)));
		}
		//区间搜索，搜索0值
		if (bUnitSize == 2)
		{
			i = HAL_PMCPSrchGELE_Cir(0, 0xFFFF, 0xFFFF, 0, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetItemBuffAddr, (wSrchRetCnt-wIdxUnitCnt), wStartOff);
		}
		else if (bUnitSize == 4)
		{
			i = HAL_PMCPSrchGELE_Cir(0, 0xFFFFFFFF, 0xFFFFFFFF, 0, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetItemBuffAddr, (wSrchRetCnt-wIdxUnitCnt), wStartOff);
		}
		//PRINT_HEX("End_HAL_PMCPSrchGELE_Cir,(wSrchRetCnt-wIdxUnitCnt)=%x, i=%x ", (wSrchRetCnt-wIdxUnitCnt), i);
		for (j=0; j<wIdxUnitCnt; j++)
		{
			// 将数据在区间内循环wIdxUnitCnt次，重新排序
			U16 wTemp;
			wTemp = *(U16 *)(dRetItemBuffAddr+(wSrchRetCnt-1)*sizeof(U16));		// 数组最后一个元素
			for (i=wSrchRetCnt; i>1; i--)
			{
				*(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+(i-2)*sizeof(U16));
			}
			*(U16 *)(dRetItemBuffAddr) = wTemp;								// 数据第一个元素
			PRINT_HEX("HAL_PMCPSrchGELE,(wSrchRetCnt-wIdxUnitCnt)=%x ", (wSrchRetCnt-wIdxUnitCnt));
		}
		// 将搜到的数据buf，补上FF数据
		//memset((U08 *)(dRetValueBuffAddr+wIdxUnitCnt*sizeof(U16)), 0x00, (wSrchRetCnt-wIdxUnitCnt)*sizeof(U16));
		MemorySet((U08 *)(dRetValueBuffAddr+wIdxUnitCnt*bUnitSize), 0xFF, (U32)(wSrchRetCnt-wIdxUnitCnt)*bUnitSize);
	}
}
}

// *****************************************************************************
//函数功能：Memory Set（利用分段归类排序的第一步来实现）
//dCount表示要设置多少byte的memory
//说明：仅支持PMCP0
// *****************************************************************************
void HAL_PMCPMemorySet_Cfg(U8 bPmcpIdx, U32 dRamAddr, U32 dValue, U32 dCount)
{
	if(dCount != 0)
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
		if (((dRamAddr&(4-1))!=0) || ((dCount&(4-1))!=0) || (bPmcpIdx>1))
		{
			PRINT_TXT("HAL_PMCPMemorySet,Error_Param ");
			return;
		}
#endif
		SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
		SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
		SgPMCP->rBASE_ADDR0 = dRamAddr;				// memory地址
		SgPMCP->rOPE_CNT.dAll = (dCount/4);			// 源数据个数,以double word为单位
		SgPMCP->rOPE_CSR.dAll = (4<<BIT_STEP_LEN);	// MemorySet步长
		SgPMCP->rOPE_MODE |= 0x32;		// memory set功能
		SgPMCP->rMIN_VAL = dValue;		// 写入Memory的数值
		SgPMCP->rMAX_VAL = 0;				// 写入Memory的数值

		SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
		SgPMCP->rOP_START.bits.dVAL = 1;
		__nds32__dsb();
	}
}
//说明：仅支持PMCP0
void HAL_PMCPMemorySet(U8 bPmcpIdx, U32 dRamAddr, U32 dValue, U32 dCount)
{
	if(dCount != 0)
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
		if (((dRamAddr&(4-1))!=0) || ((dCount&(4-1))!=0) || (bPmcpIdx>1))
		{
			PRINT_TXT("HAL_PMCPMemorySet,Error_Param ");
			return;
		}
#endif
		HAL_PMCPMemorySet_Cfg(bPmcpIdx, dRamAddr, dValue, dCount);
		HAL_PMCPWaitDone(bPmcpIdx);
	}
}


// *****************************************************************************
//函数功能: 区间数据拼接
// 入口参数:
//			ptTrimInfoBuff -> 搜索目标内存区起始地址
//			dTrimAddrBuff ->存放合并项起始地址
//			dTrimSctBuff ->存放合并后的合并项长度
//			dDataLen	-> 数据长度，8byte对齐，byte单位
// 出口参数:
// 说    明:数据源固定8byte，结果长度步长固定为4byte。仅支持PMCP0
// *****************************************************************************
void HAL_PMCP0TrimAddrCombine_Cfg(T_TRIM_INFO *ptTrimInfoBuff, U32 dTrimAddrBuff, U32 dTrimSctBuff, U32 dDataLen)
{
	if(dDataLen != 0)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((((U32)ptTrimInfoBuff)&(8-1))!=0) || ((dTrimAddrBuff&(8-1))!=0) || ((dTrimSctBuff&(4-1))!=0))
	{
		PRINT_TXT("HAL_PMCPTrimAddrCombine,Error_Param ");
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;							// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = (U32)ptTrimInfoBuff;		// 源数据的起始地址
	SgPMCP->rBASE_ADDR1 = dTrimSctBuff;				// 合并项长度的地址
	SgPMCP->rRESULT_ADDR = dTrimAddrBuff;			// 统计结果ram位置
	SgPMCP->rOPE_CNT.bits.dVAL = (dDataLen/8);			// 数据元素个数,以8Byte为单位
	SgPMCP->rOPE_CSR.dAll = (4<<BIT_RESULT_STEP_LEN);	// 结果步长
	SgPMCP->rOPE_MODE = 0x18;	// 区间数据拼接模式
	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
#if 1
U08 HAL_PMCP0TrimAddrCombine(T_TRIM_INFO *ptTrimInfoBuff, U32 dTrimAddrBuff, U32 dTrimSctBuff, U32 dDataLen)
{
	if(dDataLen != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((((U32)ptTrimInfoBuff)&(8-1))!=0) || ((dTrimAddrBuff&(8-1))!=0) || ((dTrimSctBuff&(4-1))!=0))
	{
		PRINT_TXT("HAL_PMCPTrimAddrCombine,Error_Param ");
		return 0;
	}
#endif
	HAL_PMCP0TrimAddrCombine_Cfg(ptTrimInfoBuff, dTrimAddrBuff, dTrimSctBuff, dDataLen);
	HAL_PMCPWaitDone(0);
	return SgPMCP->rGOT_RESULT_CNT.bits.dVAL;
}
	else
	{
		return 0;
	}
}
#else
void SetValue(U8 *pBuffer, U32 nIndex, U32 nStep, U32 nValue)
{
	if (1 == nStep)									// byte
	{
		*((U8 *)pBuffer+nIndex) = (U8)nValue;
	}
	else if (2 == nStep)							// word
	{
		*((U16 *)pBuffer+nIndex) = (U16)nValue;
	}
	else if (4 == nStep)							// dword
	{
		*((U32 *)pBuffer+nIndex) = nValue;
	}
	else if (7 == nStep)							// bit
	{
#if  1
        if (0 == nValue)
        {
            *(pBuffer+nIndex/8) &= (U8)(~(1<<(nIndex%8)));
        }
        else
        {
            *(pBuffer+nIndex/8) |= (U8)(1)<<(nIndex%8);
        }
#else
		*(pBuffer+nIndex/8) &= (U8)(~(1<<(nIndex%8)));
		*(pBuffer+nIndex/8) |= (nValue & 0x01)<<(nIndex%8); //(1<<nIndex%8);
#endif
	}
}
U08 HAL_PMCP0TrimAddrCombine(T_TRIM_INFO *ptTrimInfoBuff, U32 dTrimAddrBuff,U32 dTrimSctBuff, U32 dDataLen)
{
//#define dDataLen		512					// 512BYTE
	//开始合并项
	U08 bIdx = 0;
	U08 bMerge_Flag = 0;
	U64 ddStartAddr = 0;
	U64 ddMerge_SectCnt = 0;
	U64 ddLastElemLBA, ddLastElemSect;
	T_TRIM_INFO *pSrcTrimInfo = ptTrimInfoBuff;
	U32 i;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((((U32)ptTrimInfoBuff)&(8-1))!=0) || ((dTrimAddrBuff&(8-1))!=0) || ((dTrimSctBuff&(8-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0TrimAddrCombine,Error_Param\n");
		return 0;
	}
	if ((dDataLen&(8-1)) != 0)
	{
		PRINT_TXT("HAL_PMCP0TrimAddrCombine,dDataLen Error_Param\n");
		return 0;
	}
#endif
	for (i=0; i<dDataLen/8-1; i++)
	{
		U64 ddElemLBA1 = (U64)((pSrcTrimInfo+i)->dLba & 0xFFFFFFFFFFFF);
		U64 ddElemSect1 = (U64)((pSrcTrimInfo+i)->wSectLen & 0xffff);
		U64 ddElemLBA2 = (U64)((pSrcTrimInfo+i+1)->dLba & 0xFFFFFFFFFFFF);
		U64 ddElemSect2 = (U64)((pSrcTrimInfo+i+1)->wSectLen & 0xffff);

		if (((ddElemLBA1 + ddElemSect1) != ddElemLBA2) || (0 == ddElemSect1))			//lba可以为0
		{
			if (0 == ddElemSect1)
			{
				if (0 == bMerge_Flag)
				{
					continue;
				}
			}
			*((U64 *)(dTrimAddrBuff) + bIdx) = ddStartAddr;
			SetValue((U8 *)dTrimSctBuff, bIdx, 4, (U32)ddMerge_SectCnt);
			if (0 == bMerge_Flag)											// 不可合并，存储数据
			{
				*((U64 *)(dTrimAddrBuff) + bIdx) = ddElemLBA1;
				SetValue((U8 *)dTrimSctBuff, bIdx, 4, (U32)ddElemSect1);
			}
			bIdx++;
			bMerge_Flag = 0;
		}
		else
		{
			if (0 == bMerge_Flag)
			{
				ddStartAddr = ddElemLBA1;									// 可合并，记录首地址
				ddMerge_SectCnt = ddElemSect1;
			}
			bMerge_Flag = 1;
			ddMerge_SectCnt += ddElemSect2;
		}
	}

	if (0 == bMerge_Flag)			// 和上一个不是可以合并的，单独处理
	{
		ddLastElemLBA = (U64)((pSrcTrimInfo+dDataLen/8-1)->dLba & 0xFFFFFFFFFFFF);		//获取最后一个元素
		ddLastElemSect = (U64)((pSrcTrimInfo+dDataLen/8-1)->wSectLen & 0xffff);
		if (0 == ddLastElemSect)	// 元素的扇区个数为0，无效项
		{
			return bIdx;
		}
		*((U64 *)(dTrimAddrBuff) + bIdx) = ddLastElemLBA;
		SetValue((U8 *)dTrimSctBuff, bIdx, 4, (U32)ddLastElemSect);
	}
	else							// 最后一个元素和上一个是可合并项，存储合并后的数据
	{
		*((U64 *)(dTrimAddrBuff) + bIdx) = ddStartAddr;
		SetValue((U8 *)dTrimSctBuff, bIdx, 4, (U32)ddMerge_SectCnt);
	}
	return (bIdx+1);			// 返回合并后总的项个数
}
#endif
// *****************************************************************************
//函数功能: 矩阵操作
// 入口参数:
//			dSrcRamAddr0 -> 矩阵A
//			dSrcRamAddr1 -> 矩阵B
//			dResultAddr  -> 结果存放地址
//			wUnitLen	 -> 矩阵元素个数
//			bUnitStep    -> 矩阵元素步长
//			bResultStep  -> 结果元素步长
//			bMode        -> 0与、1或、2异或、3同或、4加、5减
// 出口参数:
// *****************************************************************************
void HAL_PMCPMatrixMatrixOp_Cfg(U8 bPMCPIdx, U32 dSrcRamAddr0, U32 dSrcRamAddr1, U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bMode)
{
	if(wUnitLen != 0)
{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPMCPIdx);
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrcRamAddr0;			// 矩阵A的起始地址
	SgPMCP->rBASE_ADDR1 = dSrcRamAddr1;			// 矩阵B的起始地址
	SgPMCP->rRESULT_ADDR = dResultAddr;			// 结果ram位置
	SgPMCP->rOPE_CNT.bits.dVAL = wUnitLen;		// 数据元素个数
	SgPMCP->rOPE_CSR.dAll = (bResultStep<<BIT_RESULT_STEP_LEN) | (bUnitStep<<BIT_STEP_LEN);	// 结果步长/矩阵元素步长
	SgPMCP->rOPE_MODE |= 0x12 | (bMode << 12);	// 矩阵操作模式/配置运算模式/关闭bitmask

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
	}
}


// dSrcRamAddr0:矩阵0的起始地址
// dSrcRamAddr0:矩阵1的起始地址
// dResultAddr:结果存放的起始地址
// wUnitLen:矩阵元素个数(两个矩阵相同)
// bUnitSize:矩阵元素步长(两个矩阵相同)
// bResultSize:结果元素步长
// bMode: 不同的值代表不同的操作类型
inline static void MatrixMatrixAnd(U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen)
{
	U16 i;
	for (i=0; i<wUnitLen; i++)
	{
		U32 dValue1 = *(((U32 *)dSrcRamAddr0)+i);
		U32 dValue2 = *(((U32 *)dSrcRamAddr1)+i);
		*(((U32 *)dResultAddr)+i) = (dValue1 & dValue2);
	}
}
inline static void MatrixMatrixOr(U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen)
{
	U16 i;
	for (i=0; i<wUnitLen; i++)
	{
		U32 dValue1 = *(((U32 *)dSrcRamAddr0)+i);
		U32 dValue2 = *(((U32 *)dSrcRamAddr1)+i);
		*(((U32 *)dResultAddr)+i) = (dValue1 | dValue2);
	}
}
inline static void MatrixMatrixXor(U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen)
{
	U16 i;
	for (i=0; i<wUnitLen; i++)
	{
		U32 dValue1 = *(((U32 *)dSrcRamAddr0)+i);
		U32 dValue2 = *(((U32 *)dSrcRamAddr1)+i);
		*(((U32 *)dResultAddr)+i) = (dValue1 ^ dValue2);
	}
}
inline static void MatrixMatrixAor(U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen)
{
	U16 i;
	for (i=0; i<wUnitLen; i++)
	{
		U32 dValue1 = *(((U32 *)dSrcRamAddr0)+i);
		U32 dValue2 = *(((U32 *)dSrcRamAddr1)+i);
		*(((U32 *)dResultAddr)+i) = ~(dValue1 ^ dValue2);
	}
}
inline static U8 MatrixMatrixAdd(U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen, U08 bUnitStep)
{
	U16 i;
	U8 bOvFlag = 0;
	if (2 == bUnitStep)
	{
		for (i=0; i<wUnitLen; i++)
		{
			U16 wValue1 = *(((U16 *)dSrcRamAddr0)+i);
			U16 wValue2 = *(((U16 *)dSrcRamAddr1)+i);
			*(((U16 *)dResultAddr)+i) = (wValue1 + wValue2);
			if (*(((U16 *)dResultAddr)+i)<wValue1 && *(((U16 *)dResultAddr)+i)<wValue2)
			{
				bOvFlag = 1;
			}
		}
	} 
	else if (4 == bUnitStep)
	{
		for (i=0; i<wUnitLen; i++)
		{
			U32 dValue1 = *(((U32 *)dSrcRamAddr0)+i);
			U32 dValue2 = *(((U32 *)dSrcRamAddr1)+i);
			*(((U32 *)dResultAddr)+i) = (dValue1 + dValue2);
			if (*(((U32 *)dResultAddr)+i)<dValue1 && *(((U32 *)dResultAddr)+i)<dValue2)
			{
				bOvFlag = 1;
			}
		}
	}
	return bOvFlag;
}
inline static U8 MatrixMatrixMinus(U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen, U08 bUnitStep)
{
	U16 i;
	U8 bOvFlag = 0;
	if (2 == bUnitStep)
	{
		for (i=0; i<wUnitLen; i++)
		{
			U16 wValue1 = *(((U16 *)dSrcRamAddr0)+i);
			U16 wValue2 = *(((U16 *)dSrcRamAddr1)+i);
			*(((U16 *)dResultAddr)+i) = (U16)(wValue2 - wValue1);
			if (*(((U16 *)dResultAddr)+i)>wValue2)
			{
				bOvFlag = 1;
			}
		}
	} 
	else if (4 == bUnitStep)
	{
		for (i=0; i<wUnitLen; i++)
		{
			U32 dValue1 = *(((U32 *)dSrcRamAddr0)+i);
			U32 dValue2 = *(((U32 *)dSrcRamAddr1)+i);
			*(((U32 *)dResultAddr)+i) = (U32)(dValue2 - dValue1);
			if (*(((U32 *)dResultAddr)+i)>dValue2)
			{
				bOvFlag = 1;
			}
		}
	}
	return bOvFlag;
}

U8 HAL_PMCPMatrixMatrixOp(U8 bPMCPIdx, U32 dSrcRamAddr0, U32 dSrcRamAddr1,U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bMode)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((((U32)dSrcRamAddr0)&(bUnitStep-1))!=0) || ((dSrcRamAddr1&(bUnitStep-1))!=0) || ((dResultAddr&(bResultStep-1))!=0) || ((bPMCPIdx!=0) && (bPMCPIdx!=1)))
	{
		PRINT_TXT("HAL_PMCPMatrixMatrixOp,Error_Param1 ");
		return 0;
	}
	if ((bUnitStep!=bResultStep) || ((bUnitStep!=4) && (bMode<MATRIX_ADD)) || ((bPMCPIdx!=0)&&((bMode==MATRIX_ADD)||(bMode==MATRIX_MINUS))))
	{
		PRINT_TXT("HAL_PMCPMatrixMatrixOp,Error_Param2 ");
		return 0;
	}
	if (((bMode==MATRIX_ADD)||(bMode==MATRIX_MINUS))&&((bUnitStep!=2)&&(bUnitStep!=4)))		//加减法操作，步长可为2或4；其他条件为非法值
	{
		PRINT_TXT("HAL_PMCPMatrixMatrixOp,Error_Param3 ");
		return 0;
	}
	if(bMode>MATRIX_MINUS)
	{
		PRINT_TXT("HAL_PMCPMatrixMatrixOp,Error_Param4 ");
		return 0;
	}
#endif
#if 1
	HAL_PMCPMatrixMatrixOp_Cfg(bPMCPIdx, dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen, bUnitStep, bResultStep, bMode);
	HAL_PMCPWaitDone(bPMCPIdx);
	return SG_PMCP(bPMCPIdx)->rPMCP_STS.bits.OV_FLAG;
#else
	U8 bOvFlag = 0;
	switch(bMode)
	{
	case MATRIX_AND:
		MatrixMatrixAnd(dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen);
		break;
	case MATRIX_OR:
		MatrixMatrixOr(dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen);
		break;
	case MATRIX_XOR:
		MatrixMatrixXor(dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen);
		break;
	case MATRIX_AOR:
		MatrixMatrixAor(dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen);
		break;
	case MATRIX_ADD:
		bOvFlag = MatrixMatrixAdd(dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen, bUnitStep);
		break;
	case MATRIX_MINUS:
		bOvFlag = MatrixMatrixMinus(dSrcRamAddr0, dSrcRamAddr1, dResultAddr, wUnitLen, bUnitStep);
		break;
	default:
		break;
	}
	return bOvFlag;
#endif
}

// dSrcRamAddr0:矩阵0的起始地址
// dFixedNum:代表固定数的值
// dResultAddr:结果存放的起始地址
// wUnitLen:矩阵元素个数(两个矩阵相同)
// bUnitSize:矩阵元素步长(两个矩阵相同)
// bResultSize:结果元素步长
// bMode: 不同的值代表不同的操作类型
void HAL_PMCPMatrixValueOp_Cfg(U8 bPMCPIdx, U32 dSrcRamAddr, U32 dFixedNum, U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bMode)
{
	if(wUnitLen != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPMCPIdx);
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrcRamAddr;			// 源数据的起始地址
	SgPMCP->rRESULT_ADDR = dResultAddr;			// 结果ram位置
	SgPMCP->rOPE_CNT.bits.dVAL = wUnitLen;		// 数据元素个数
	SgPMCP->rOPE_CSR.dAll = (bUnitStep<<BIT_STEP_LEN) | (bResultStep<<BIT_RESULT_STEP_LEN);	// 矩阵/结果步长
	SgPMCP->rOPE_MODE |= 0x13 | (bMode << 12);	// 矩阵与固定数操作模式/配置运算模式/关闭bitmask
	SgPMCP->rMAX_VAL = dFixedNum;				// 固定数

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

// 在2byte步长下，固定数dFixedNum[31:16]=dFixedNum[15:0]，即固定数为0x55AA，那dFixedNum[31:0]=0x55AA55AA
U8 HAL_PMCPMatrixValueOp(U8 bPMCPIdx, U32 dSrcRamAddr, U32 dFixedNum, U32 dResultAddr,U16 wUnitLen, U08 bUnitStep, U08 bResultStep, U08 bMode)
{
	if(wUnitLen != 0)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((((U32)dSrcRamAddr)&(bUnitStep-1))!=0) || ((dResultAddr&(bResultStep-1))!=0) || ((bPMCPIdx!=0) && (bPMCPIdx!=1)))
	{
		PRINT_TXT("HAL_PMCPMatrixValueOp,Error_Param1 ");
		return 0;
	}
	if ((bUnitStep!=bResultStep) || ((bUnitStep!=4) && (bMode<MATRIX_ADD)) || ((bPMCPIdx!=0)&&((bMode==MATRIX_ADD)||(bMode==MATRIX_MINUS))))
	{
		PRINT_TXT("HAL_PMCPMatrixValueOp,Error_Param2 ");
		return 0;
	}
	if (((bMode==MATRIX_ADD)||(bMode==MATRIX_MINUS))&&((bUnitStep!=2)&&(bUnitStep!=4)))
	{
		PRINT_TXT("HAL_PMCPMatrixValueOp,Error_Param3 ");
		return 0;
	}
	if (bMode>MATRIX_MINUS)
	{
		PRINT_TXT("HAL_PMCPMatrixValueOp,Error_Param4 ");
		return 0;
	}
#endif

	if (((bMode==MATRIX_ADD)||(bMode==MATRIX_MINUS))&&(bUnitStep==2))
	{
       (((U16 *)&(dFixedNum))[1]) = (((U16 *)&(dFixedNum))[0]);
	}

	HAL_PMCPMatrixValueOp_Cfg(bPMCPIdx, dSrcRamAddr, dFixedNum, dResultAddr, wUnitLen, bUnitStep, bResultStep, bMode);
	HAL_PMCPWaitDone(bPMCPIdx);
	return 0;
}
	else
	{
		return 0;
	}
}
void HAL_PMCP0SpecialMemSetWithIdx_Cfg(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U16 wIdxThreshold, U08 bIdxStep, U08 bReplaceMode, U32 dReplaceValueBase)
{
	if(wIdxUnitCnt != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bReplaceMode-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(bIdxStep-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param ");
		return;
	}
#endif
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dIdxRamAddr;			// index数组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrchRamAddr;			// 元素的存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = wIdxUnitCnt;	// index数组的元素个数
	SgPMCP->rOPE_CSR.dAll = (bReplaceMode<<BIT_RESULT_STEP_LEN) | (bIdxStep<<BIT_STEP_LEN);	// 源数据步长/index数组的步长
	SgPMCP->rOPE_MODE |= 0x09 | (1 << 8);		// 带索引的内存值设定模式/开启特殊替换值使能
	SgPMCP->rMAX_VAL = wIdxThreshold;           // 设定操作源数组索引上限值
	SgPMCP->rMIN_VAL = dReplaceValueBase;		// 设定值低32位
	SgPMCP->rMOVE_RLT1_OP_ARRL = 0; 			// 设定值高32位

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

// 特殊的带索引+替换值+offs的内存值替换功能
// 该功能同时支持两个PMCP，但该接口仅使用PMCP0
void HAL_PMCP0SpecialMemSetWithIdx(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U16 wIdxThreshold, U08 bIdxStep, U08 bReplaceMode, U32 dReplaceValueBase)
{
	if(wIdxUnitCnt != 0)
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bReplaceMode-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(bIdxStep-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param ");
		return;
	}
#endif
	HAL_PMCP0SpecialMemSetWithIdx_Cfg(dSrchRamAddr, dIdxRamAddr, wIdxUnitCnt, wIdxThreshold, bIdxStep, bReplaceMode, dReplaceValueBase);
	HAL_PMCPWaitDone(0);
}
}

/*
特殊的带索引+替换值+offs的内存值替换+原值右移位+计数功能
dSrchRamAddr：内存值，源数据
dIdxRamAddr：索引数组
dCountRamAddr: 用于统计计数的数组Buff地址
wIdxUnitCnt：索引元素个数
wIdxThreshold: 索引项值阈值。小于该阈值的项，才进行替换
bIdxStep：索引步长
bReplaceMode：替换模式（1，2，4byte）
dReplaceValueBase：替换值基值
bRightSftBitCnt  : 要右移位的bit个数
*/
void HAL_PMCP0SpcMemSetWithIdxAndSftCnt_Cfg(U32 dSrchRamAddr, U32 dIdxRamAddr, U32 dCountRamAddr, U16 wIdxUnitCnt, U16 wIdxThreshold, U08 bIdxStep, U08 bReplaceMode, U32 dReplaceValueBase, U08 bRightSftBitCnt)
{
	if(wIdxUnitCnt != 0)
{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bReplaceMode-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(bIdxStep-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param1 ");
		return;
	}
	if(((dCountRamAddr&(4-1))!=0) || !((bReplaceMode==1) || (bReplaceMode==2) || (bReplaceMode==4)))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param2 ");
		return;
	}
	if(bRightSftBitCnt>32)
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param3 ");
		return;
	}
#endif
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dIdxRamAddr;			// index数组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrchRamAddr;			// 元素的存放地址
	SgPMCP->rRMD0_ADDR = dCountRamAddr;			// C数组
	SgPMCP->rOPE_CNT.bits.dVAL = wIdxUnitCnt;	// index数组的元素个数
	SgPMCP->rOPE_CSR.dAll = (bReplaceMode<<BIT_RESULT_STEP_LEN) | (bIdxStep<<BIT_STEP_LEN) | (bRightSftBitCnt << 8);	// 源数据步长/index数组的步长/右移位数
	SgPMCP->rOPE_MODE |= 0x09 | (1 << 8) | (1 << 9) | (4 << 12);	// 带索引的内存值设定模式/开启特殊替换值使能/右移位操作使能/计数数组步长设为4byte
	SgPMCP->rMAX_VAL = wIdxThreshold;			// 最大的下标值，小于这个值才会被替换
	SgPMCP->rMIN_VAL = dReplaceValueBase;		// 设定值低32位
	SgPMCP->rMOVE_RLT1_OP_ARRL = 0; 			// 设定值高32位
	SgPMCP->rBIT_MASK = 0;                   	// bitmask值
	SgPMCP->rINVALID_VAL = 0;
	SgPMCP->rMOVE_RLT0_OP_ARRL = 0;          // 原始最低阈值为0
	SgPMCP->rMOVE_RLT0_OP_ARRH = 0;
	SgPMCP->rMOVE_RLT2_OP_ARRL = 0xFFFFFFFF; // 原始最高阈值为最大值
	SgPMCP->rMOVE_RLT2_OP_ARRH = 0xFFFFFFFF;
	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

// 该功能同时支持两个PMCP，但该接口仅使用PMCP0
U16 HAL_PMCP0SpcMemSetWithIdxAndSftCnt(U32 dSrchRamAddr, U32 dIdxRamAddr, U32 dCountRamAddr, U16 wIdxUnitCnt, U16 wIdxThreshold, U08 bIdxStep, U08 bReplaceMode, U32 dReplaceValueBase, U08 bRightSftBitCnt)
{
	if(wIdxUnitCnt != 0)
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bReplaceMode-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(bIdxStep-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param1 ");
		return 0x55aa;
	}
	if(((dCountRamAddr&(4-1))!=0) || !((bReplaceMode==1) || (bReplaceMode==2) || (bReplaceMode==4)))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param2 ");
		return 0x55aa;
	}
	if(bRightSftBitCnt>32)
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param3 ");
		return 0x55aa;
	}
#endif
#if  1
	HAL_PMCP0SpcMemSetWithIdxAndSftCnt_Cfg(dSrchRamAddr, dIdxRamAddr, dCountRamAddr, wIdxUnitCnt, wIdxThreshold, bIdxStep, bReplaceMode, dReplaceValueBase, bRightSftBitCnt);
	HAL_PMCPWaitDone(0);
	return SG_PMCP(0)->rBIT_RESULT.bits.dVAL;
#else
	U32 i, *pdDat, *pdCnt, dSftValue;
U32 nPos;
U16  *pwItem;
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(8-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(8-1))!=0))
	{
		LOG_PRINT("HAL_PMCP0MemorySetWithIdx,Error_Param ");
		return;
	}
#endif
	if ((bReplaceMode != 1) && (bReplaceMode != 2) && (bReplaceMode != 4))
	{
		bReplaceMode = 7;		//其他模式都是bit替换
	}
	pdDat = (U32 *)dSrchRamAddr;
	pdCnt = (U32 *)dCountRamAddr;
	pwItem = (U16 *)dIdxRamAddr;
	for (i=0; i<wIdxUnitCnt; i++)
	{
		nPos = pwItem[i];  //GetValue((U08 *)dIdxRamAddr, i, bIdxStep, 0xF);
		dSftValue = ((*(pdDat + nPos)) >> bRightSftBitCnt);
		*(pdCnt + dSftValue) = *(pdCnt + dSftValue) + 1;   //原始计数值 + 1
		if (nPos < wIdxThreshold)
		{
			pdDat[nPos] = dReplaceValueBase + i;
		}
	}
#endif
}
	else
	{
		return 0;
	}
}
// 该功能同时支持两个PMCP，但该接口仅使用PMCP0
void HAL_PMCP0MemorySetWithIdx_Cfg(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U08 bIdxStep, U08 bReplaceMode, U32 dSetValue)
{
	if(wIdxUnitCnt != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bReplaceMode-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(bIdxStep-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param ");
		return;
	}
#endif
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dIdxRamAddr;			// index数组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrchRamAddr;			// 元素的存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = wIdxUnitCnt;	// index数组的元素个数
	SgPMCP->rOPE_CSR.dAll = (bReplaceMode<<BIT_RESULT_STEP_LEN) | (bIdxStep<<BIT_STEP_LEN); // index数组的步长
	SgPMCP->rOPE_MODE = 0x09;		// 带索引的内存值设定模式/关闭特殊值、右移
	SgPMCP->rMIN_VAL = dSetValue;				// 设定值低32位
	SgPMCP->rMOVE_RLT1_OP_ARRL = 0; 			// 设定值高32位
	SgPMCP->rMAX_VAL = 0xFFFFFFFF;              // 设定MAX限值为最大值

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
// 该功能同时支持两个PMCP，但该接口仅使用PMCP0
void HAL_PMCP0MemorySetWithIdx(U32 dSrchRamAddr, U32 dIdxRamAddr, U16 wIdxUnitCnt, U08 bIdxStep, U08 bReplaceMode, U32 dSetValue)
{
	if(wIdxUnitCnt != 0)
	{
	#if(1 == PMCP_PARAM_JUDGE_EN)
		if (((dSrchRamAddr&(bReplaceMode-1))!=0) || !((bIdxStep==1) || (bIdxStep==2) || (bIdxStep==4)) || ((dIdxRamAddr&(bIdxStep-1))!=0))
		{
			PRINT_TXT("HAL_PMCP0MemorySetWithIdx,Error_Param ");
			return;
		}
	#endif
	HAL_PMCP0MemorySetWithIdx_Cfg(dSrchRamAddr, dIdxRamAddr, wIdxUnitCnt, bIdxStep, bReplaceMode, dSetValue);
	HAL_PMCPWaitDone(0);
}
}

void  HAL_PMCPSrchMax_WithZero(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0)&& (wSrchRetCnt != 0))
	{
#define IDX_RESULT_STEP	2		// 固定结果idx步长为2
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMax,Error_Param ");
		return;
	}
#endif
	HAL_PMCPSrchMax_Cfg(bPmcpIdx, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wStartOff);
	HAL_PMCPWaitDone(bPmcpIdx);
	if (SgPMCP->rGOT_RESULT_CNT.bits.dVAL < wSrchRetCnt)		// 搜到个数小于期望个数，存在0的项
	{
		U16 i, j;
		U16 wIdxUnitCnt = SgPMCP->rGOT_RESULT_CNT.bits.dVAL;
		U08 bReplaceMode = bUnitSize;					// 替换模式等于源数据的步长
		//先用矩阵替换，把最大值搜索到的数据还原
		HAL_PMCPWaitDone(0);
		HAL_PMCP0MemorySetWithIdx_Cfg(dSrchRamAddr, dRetItemBuffAddr, wIdxUnitCnt, IDX_RESULT_STEP, bReplaceMode, 0x55aa55aa);
		HAL_PMCPWaitDone(0);
		//LOG_PRINT("End_HAL_PMCP0MemorySetWithIdx,(wSrchRetCnt-wIdxUnitCnt)=%x ", (wSrchRetCnt-wIdxUnitCnt));
		//移位index数据，将前面的若干个元素移到最后面
		for(i=wIdxUnitCnt; i>0; i--)
		{
			*(U16 *)(dRetItemBuffAddr+((wSrchRetCnt-wIdxUnitCnt)+(i-1))*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16));
			//*(U16 *)(dRetItemBuffAddr+((wSrchRetCnt-wIdxUnitCnt)+i)*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+i*sizeof(U16));
			//LOG_PRINT("i=%d, HAL_PMCPSrchGELE,*(U16 *)(dRetItemBuffAddr+i*sizeof(U16)=%x ",i, *(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16)));
		}
		//区间搜索，搜索0值
		i = HAL_PMCPSrchGELE_Cir(0, 0x00, 0x00, 0, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetItemBuffAddr, (wSrchRetCnt-wIdxUnitCnt), wStartOff);
		//HAL_PMCPWaitDone(0);
		//LOG_PRINT("End_HAL_PMCPSrchGELE,(wSrchRetCnt-wIdxUnitCnt)=%x, i=%x ", (wSrchRetCnt-wIdxUnitCnt), i);
		for (j=0; j<wIdxUnitCnt; j++)
		{
			// 将数据在区间内循环wIdxUnitCnt次，重新排序
			U16 wTemp;
			wTemp = *(U16 *)(dRetItemBuffAddr+(wSrchRetCnt-1)*sizeof(U16));		// 数组最后一个元素
			for (i=wSrchRetCnt; i>1; i--)
			{
				*(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+(i-2)*sizeof(U16));
			}
			*(U16 *)(dRetItemBuffAddr) = wTemp;								// 数据第一个元素
			//LOG_PRINT("HAL_PMCPSrchGELE,(wSrchRetCnt-wIdxUnitCnt)=%x ", (wSrchRetCnt-wIdxUnitCnt));
		}
		// 将搜到的数据buf，补上0数据
		//memset((U08 *)(dRetValueBuffAddr+wIdxUnitCnt*sizeof(U16)), 0x00, (wSrchRetCnt-wIdxUnitCnt)*sizeof(U16));
		MemorySet((U08 *)(dRetValueBuffAddr+wIdxUnitCnt*bUnitSize), 0x00, (U32)(wSrchRetCnt-wIdxUnitCnt)*bUnitSize);
	}
}
}



void  HAL_PMCPSrchMin_WithFFFF(U8 bPmcpIdx, U32 dSrchRamAddr, U08 bUnitSize, U16 wSrchUnitCnt,U32 dRetValueBuffAddr, U32 dRetItemBuffAddr,U16 wSrchRetCnt,U16 wStartOff)
{
	if((wSrchUnitCnt != 0)&& (wSrchRetCnt != 0))
	{
#define IDX_RESULT_STEP	2		// 固定结果idx步长为2
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrchRamAddr&(bUnitSize-1))!=0) || !((bUnitSize==2) || (bUnitSize==4)) || ((dRetValueBuffAddr&(bUnitSize-1))!=0) || ((dRetItemBuffAddr&(2-1))!=0) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPSrchMax,Error_Param ");
		return;
	}
#endif
	HAL_PMCPSrchMin_Cfg(bPmcpIdx, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetValueBuffAddr, dRetItemBuffAddr, wSrchRetCnt, wStartOff);
	HAL_PMCPWaitDone(bPmcpIdx);
	if (SgPMCP->rGOT_RESULT_CNT.bits.dVAL < wSrchRetCnt)		// 搜到个数小于期望个数，存在全FF的项
	{
		U16 i, j;
		U16 wIdxUnitCnt = SgPMCP->rGOT_RESULT_CNT.bits.dVAL;
		U08 bReplaceMode = bUnitSize;					// 替换模式等于源数据的步长
		//先用矩阵替换，把最大值搜索到的数据还原
		HAL_PMCPWaitDone(0);
		HAL_PMCP0MemorySetWithIdx_Cfg(dSrchRamAddr, dRetItemBuffAddr, wIdxUnitCnt, IDX_RESULT_STEP, bReplaceMode, 0x55aa55aa);
		HAL_PMCPWaitDone(0);
		//LOG_PRINT("End_HAL_PMCP0MemorySetWithIdx,(wSrchRetCnt-wIdxUnitCnt)=%x ", (wSrchRetCnt-wIdxUnitCnt));
		//移位index数据，将前面的若干个元素移到最后面
		//for (i=0; i<wIdxUnitCnt; i++)
		for(i=wIdxUnitCnt; i>0; i--)
		{
			*(U16 *)(dRetItemBuffAddr+((wSrchRetCnt-wIdxUnitCnt)+(i-1))*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16));
			//*(U16 *)(dRetItemBuffAddr+((wSrchRetCnt-wIdxUnitCnt)+i)*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+i*sizeof(U16));
			//LOG_PRINT("i=%d, HAL_PMCPSrchGELE,*(U16 *)(dRetItemBuffAddr+i*sizeof(U16)=%x ",i, *(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16)));
		}
		//区间搜索，搜索0值
		if (bUnitSize == 2)
		{
			i = HAL_PMCPSrchGELE_Cir(0, 0xFFFF, 0xFFFF, 0, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetItemBuffAddr, (wSrchRetCnt-wIdxUnitCnt), wStartOff);
		}
		else if (bUnitSize == 4)
		{
			i = HAL_PMCPSrchGELE_Cir(0, 0xFFFFFFFF, 0xFFFFFFFF, 0, dSrchRamAddr, bUnitSize, wSrchUnitCnt, dRetItemBuffAddr, (wSrchRetCnt-wIdxUnitCnt), wStartOff);
		}
		//LOG_PRINT("End_HAL_PMCPSrchGELE_Cir,(wSrchRetCnt-wIdxUnitCnt)=%x, i=%x ", (wSrchRetCnt-wIdxUnitCnt), i);
		for (j=0; j<wIdxUnitCnt; j++)
		{
			// 将数据在区间内循环wIdxUnitCnt次，重新排序
			U16 wTemp;
			wTemp = *(U16 *)(dRetItemBuffAddr+(wSrchRetCnt-1)*sizeof(U16));		// 数组最后一个元素
			for (i=wSrchRetCnt; i>1; i--)
			{
				*(U16 *)(dRetItemBuffAddr+(i-1)*sizeof(U16)) = *(U16 *)(dRetItemBuffAddr+(i-2)*sizeof(U16));
			}
			*(U16 *)(dRetItemBuffAddr) = wTemp;								// 数据第一个元素
			//LOG_PRINT("HAL_PMCPSrchGELE,(wSrchRetCnt-wIdxUnitCnt)=%x ", (wSrchRetCnt-wIdxUnitCnt));
		}
		// 将搜到的数据buf，补上0数据
		//memset((U08 *)(dRetValueBuffAddr+wIdxUnitCnt*sizeof(U16)), 0x00, (wSrchRetCnt-wIdxUnitCnt)*sizeof(U16));
		MemorySet((U08 *)(dRetValueBuffAddr+wIdxUnitCnt*bUnitSize), 0xFF, (U32)(wSrchRetCnt-wIdxUnitCnt)*bUnitSize);
	}
}
}


// 带索引的内存值设定
// 输入参数	dSrcIdxAddr:Index数组的起始地址
// dSrcRamAddr0:被替换数组起始地址
// dValue: 替换值
// wIndexLen: index数组长度
// bIndexStep:index步长(支持 1/2/4byte)
// bReplaceMode:替换数组步长(支持1bit,1/2/4byte)
void HAL_PMCP0SetValueToMem(U32 dSrcIdxAddr, U32 dSrcRamAddr, U32 dValue, U16 wUnitLen, U08 bIndexStep, U08 bReplaceMode)
{
	if(wUnitLen != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrcIdxAddr&(bIndexStep-1))!=0) || !((bIndexStep==1) || (bIndexStep==2) || (bIndexStep==4)) || ((dSrcRamAddr&(bReplaceMode-1))!=0))
	{
		PRINT_TXT("HAL_PMCP0SetValueToMem,Error_Param ");
		return;
	}
#endif
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrcIdxAddr;		// index数组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrcRamAddr;		// 元素的存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = wUnitLen;	// index数组的元素个数
	SgPMCP->rOPE_CSR.dAll = (bReplaceMode<<BIT_RESULT_STEP_LEN) | (bIndexStep<<BIT_STEP_LEN);			// index数组的步长
	SgPMCP->rOPE_MODE = 0x09;	// 带索引的内存值设定模式/关闭特殊值、右移
	SgPMCP->rMIN_VAL = dValue;				// 设定值低32位
	SgPMCP->rMOVE_RLT1_OP_ARRL = 0; 		// 设定值高32位
	SgPMCP->rMAX_VAL = 0xFFFFFFFF;          // 设定MAX限值为最大值

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();

	HAL_PMCPWaitDone(0);
}
}

// 带索引的内存值提取
// 输入参数	dSrcRamAddr:源数据起始地址
// dIndexAddr : index的地址
// dRetBuffAddr: 返回目的数据的地址
// wIndexLen: copy元素个数
// 返回信息	返回实际搜到元素个数
void HAL_PMCPGetValueFromMem_Cfg(U08 bPmcpIdx, U32 dSrcRamAddr, U32 dIndexAddr, U32 dRetBuffAddr, U16 wUnitLen, U08 bIndexStep, U08 bResultStep)
{
	if(wUnitLen != 0)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrcRamAddr&(bResultStep-1))!=0) || ((dIndexAddr&(bIndexStep-1))!=0) || ((dRetBuffAddr&(bResultStep-1))!=0) || !((bResultStep==2) || (bResultStep==4))
		 || !((bIndexStep==1) || (bIndexStep==2)) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPGetValueFromMem,Error_Param ");
		while(1);
		return;
	}
#endif
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(bPmcpIdx);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dIndexAddr;		// index数组的起始地址
	SgPMCP->rBASE_ADDR1 = dSrcRamAddr;		// 元素的存放地址
	SgPMCP->rRESULT_ADDR = dRetBuffAddr;	// Copy数据的存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = wUnitLen;	// index数组的元素个数，提取复制的元素个数
	SgPMCP->rOPE_CSR.dAll = (bResultStep<<BIT_RESULT_STEP_LEN) | (bIndexStep<<BIT_STEP_LEN);			// index数组的步长
	SgPMCP->rOPE_MODE |= 0x0A | (1 << 8);	// 带索引的内存提取模式/记录原值使能/关闭移位统计

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}

U16 HAL_PMCPGetValueFromMem(U08 bPmcpIdx, U32 dSrcRamAddr, U32 dIndexAddr, U32 dRetBuffAddr, U16 wUnitLen, U08 bIndexStep, U08 bResultStep)
{
	if(wUnitLen != 0)
	{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if (((dSrcRamAddr&(bResultStep-1))!=0) || ((dIndexAddr&(bIndexStep-1))!=0) || ((dRetBuffAddr&(bResultStep-1))!=0) || !((bResultStep==2) || (bResultStep==4))
		 || !((bIndexStep==1) || (bIndexStep==2)) || (bPmcpIdx>1))
	{
		PRINT_TXT("HAL_PMCPGetValueFromMem,Error_Param ");
		while(1);
		return 0;
	}
#endif
	HAL_PMCPGetValueFromMem_Cfg(bPmcpIdx, dSrcRamAddr, dIndexAddr, dRetBuffAddr, wUnitLen, bIndexStep, bResultStep);
	HAL_PMCPWaitDone(bPmcpIdx);
	return wUnitLen;		// 返回提取复制的个数，等于设定值
}
}

#if 0
//只使用PMCP0
void HAL_PMCPDivision(U16 wDividend, U08 bDivisor,U16 *pwQuotient,U08 *pbRemainder)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if ((0==bDivisor) || (NULL==pwQuotient) || (NULL==pbRemainder))
	{
		PRINT_TXT("HAL_PMCPDivision,Error_Param ");
		return;
	}
#endif
	SG_PMCP(0)->rDIVIDEND = wDividend;			// 被除数
	SG_PMCP(0)->rDIVISOR  = bDivisor;			// 除数

	SG_PMCP(0)->rPMCP_DIV_CTRL.dAll =  (0x01<<1) | 0x1;		// 商是16bit的，所以设置0x01；具体看MAS

	__nds32__dsb(); 
	// Wait Done
	while (R_PMCP_DIV_START !=(SG_PMCP(0)->rPMCP_DIV_CTRL.dAll & R_PMCP_DIV_START)) {}

	pwQuotient[0] = SG_PMCP(0)->rQUOTIENT;
	pbRemainder[0] = SG_PMCP(0)->rREMAINDER;
}
#endif

void Hal_PMCP0MatrixAddAll_Cfg(U32 dMatrixSrcAddr, U32 dMatrixResultAddr, U32 dMatrixElemCnt, U32 dMatrixSrcStep)
{
	if(dMatrixElemCnt != 0)
	{
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dMatrixSrcAddr;		// 矩阵的起始地址
	SgPMCP->rRESULT_ADDR = dMatrixResultAddr;	// 矩阵的结果存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = dMatrixElemCnt;// 元素个数
	SgPMCP->rOPE_CSR.dAll = (dMatrixSrcStep<<BIT_STEP_LEN);	// 矩阵元素步长
	SgPMCP->rOPE_MODE |= 0x14; // 矩阵累加功能/关闭bitmask

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;
	__nds32__dsb();
}
}
// 该功能同时支持两个PMCP，但该接口仅使用PMCP0
U32 Hal_PMCP0MatrixAddAll(U32 dMatrixSrcAddr, U32 dMatrixResultAddr, U32 dMatrixElemCnt, U32 dMatrixSrcStep)
{
	if(dMatrixElemCnt != 0)
	{
	Hal_PMCP0MatrixAddAll_Cfg(dMatrixSrcAddr, dMatrixResultAddr, dMatrixElemCnt, dMatrixSrcStep);
	HAL_PMCPWaitDone(0);
	return SG_PMCP(0)->rPMCP_STS.bits.OV_FLAG;
	}
	else
	{
return 0;
	}
}


U16  HAL_PMCPSrchSeqSeg(U8 bPMCPIdx, U32 dStartVal, U32 dSrcAreaDatMemAddr, U08 bSrcDatSize, U32 dSrcAreaLenMemAddr, U08 bSrcLenSize, U16 wSrchUnitCnt,U32 dRetItemBuffAddr, U32 *pdTotalLen)
{
    U16 *pwAreaLen, wSrchedCnt, wTotal, wItem, *pwRetItem;
    U32 *pdAreaLen, dNextVal, dTmpLen;
    U32  dArr[4], dTmpRetItemMemAddr;
    dTmpRetItemMemAddr = ((U32)&dArr[0] + 7) & (~(U32)0x07);
    wTotal = 0;
    pwRetItem = (U16 *)dRetItemBuffAddr;
#if(1 == PMCP_PARAM_JUDGE_EN)
    if (((dSrcAreaDatMemAddr%bSrcDatSize)!=0) || !((bSrcDatSize==2) || (bSrcDatSize==4)) || ((dRetItemBuffAddr%2)!=0) || (bPMCPIdx>1))
    {
    	PRINT_TXT("HAL_PMCPSrchSeqSeg,Error_Param ");
    	return 0x55aa;
    }
#endif
    dTmpLen  = 0;
    dNextVal = dStartVal;
    while(1)
    {

    	wSrchedCnt = HAL_PMCPSrchGELE(bPMCPIdx,dNextVal,dNextVal,0,dSrcAreaDatMemAddr,bSrcDatSize,wSrchUnitCnt,dTmpRetItemMemAddr,1);
    	if (0 < wSrchedCnt)  //找到相同值，继续搜索能否接上。
    	{
    	    wItem = *((U16 *)dTmpRetItemMemAddr);
    	    if (2 == bSrcLenSize)
    	    {
    	        pwAreaLen = (U16 *)dSrcAreaLenMemAddr;
    	        dTmpLen = pwAreaLen[wItem];
    	    }
    	    else if (4 == bSrcLenSize)
    	    {
    	        pdAreaLen = (U32 *)dSrcAreaLenMemAddr;
    	        dTmpLen = pdAreaLen[wItem];
    	    }
    	    pwRetItem[wTotal] = wItem;
    	    dNextVal +=  dTmpLen;    //下一个地址
    	    wTotal++;
    	}
    	else
    	{
    	    break;
    	}
    }

    *pdTotalLen = dNextVal - dStartVal;
    return wTotal;

}


U8  HAL_PMCPMemCopy(U8 bPMCPIdx, U32 dResultAddr, U32 dSrcRamAddr, U16 wUnitLen, U8 bUnitStep)
{
    //直接使用PMCP的矩阵与固定数0进行按位或操作，来实现数据复制功能。//case 1:  nOpeMode = PX_FixNum_OR_Matrix; 矩阵与操作，固定为U32的，不能设定步长!!
    //return  HAL_PMCPMatrixValueOp(bPMCPIdx, dSrcRamAddr, 0, dResultAddr, wUnitLen, sizeof(U32), sizeof(U32), 1);

    //换成矩阵与固定数0相加的方式。
    return  HAL_PMCPMatrixValueOp(bPMCPIdx, dSrcRamAddr, 0, dResultAddr, wUnitLen, bUnitStep, bUnitStep, PMCP_ARR_MODE_VAL_ADD);
}

/***************************9086新功能接口*********************************************/
/*
多base数据解压
dBaseSrcAddr：base数组
dCntSrcAddr：每个base解压个数数组（固定为4B，解压个数不能为0）
dResultAddr：解压结果存放地址
dDataInc：数值增量
dContinueNum：base连续解压个数
dJumpColNum：跳列个数
wBaseOpeCnt：解压的base的总数
bDataStep：base数组和结果数组的步长（2，4，8）

注：仅支持PMCP0
*/
#ifdef __NEW_PMCP_FUNC__
#if 1
void HAL_PMCPMultBaseDataUnzip(U8 bPmcpIdx, U32 dBaseSrcAddr, U32 dCntSrcAddr, U32 dResultAddr, U32 dDataInc, U32 dContinueNum, U32 dJumpColNum, U16 wBaseOpeCnt, U8 bDataStep)
{
	U32 i = 0;
	U32 unzipCnt = 0; // 解压个数

#if(1 == PMCP_PARAM_JUDGE_EN)
	// check step len
	if ((bDataStep != 2) && (bDataStep != 4) && (bDataStep != 8))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: bDataStep ");
		return;
	}
	if ((dContinueNum >= (1 << 24)) || (dJumpColNum >= (1 << 24)) || (wBaseOpeCnt >= (1 << 24)) || (bPmcpIdx > 0) || (dContinueNum == 0)  || (wBaseOpeCnt == 0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: Param_Error ");
		return;
	}

	if((dJumpColNum == 0) && (wBaseOpeCnt > 1))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: JumpColNum Error");
		return;
	}
	// check addr align
	if (((dBaseSrcAddr & (bDataStep-1))!=0) || ((dResultAddr & (bDataStep-1))!=0) || ((dCntSrcAddr & (4-1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: addr align ");
		return;
	}
	// 判断解压个数是否有为0的情况
	for(i = 0; i < wBaseOpeCnt; i++)
	{
		unzipCnt = *((U32 *)dCntSrcAddr + i);	// 解压个数（数据步长4byte）
		if(unzipCnt == 0)
		{
			PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: unzipCnt can't be zero! ");
			return;
		}
	}
#endif
	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dBaseSrcAddr;			// base数组
	SgPMCP->rBASE_ADDR1 = dCntSrcAddr;			// 每个base解压个数数组
	SgPMCP->rRESULT_ADDR = dResultAddr;			// 解压结果存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = wBaseOpeCnt;	// 解压的base的总数
	SgPMCP->rOPE_MODE = 0x1D;					// 多base数据解压模式
	SgPMCP->rOPE_CSR.dAll = bDataStep;			// base数组和结果数组的步长
	SgPMCP->rMAX_VAL = dContinueNum;			// base连续解压个数
	SgPMCP->rMIN_VAL = dJumpColNum;				// 跳列个数
	SgPMCP->rBIT_MASK = dDataInc;				// 数值增量

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);

}
#else
U64 SW_PMCP_GetValueEnhance(U8 *pBuffer, U32 nIndex, U32 nStep, U8 nByteMask /*= 0xFF*/)
{
	U64 nBitsMask = 0;
	U64 nByteStatus = 0xFF;
	U64 nRet = 0;
	U64 i;
	for (i=0; i<8; i++)
	{
		if ((nByteMask>>i) & 0x01)
		{
			nBitsMask += (nByteStatus << i*8);
		}
	}

	if (3 == nStep)
	{
		U8 nByteData = *((U8 *)(pBuffer+ nIndex/8));
		nRet = (nByteData>>(nIndex%8))&0x01;
	}
	else if (1 == nStep)
	{
		nRet = (*((U8 *)pBuffer+nIndex)) & nBitsMask;
	}
	else if (2 == nStep)
	{
		nRet = (*((U16 *)pBuffer+nIndex)) & nBitsMask;
	}
	else if (4 == nStep)
	{
		nRet = (*((U32 *)pBuffer+nIndex)) & nBitsMask;
	}
	else if (8 == nStep)
	{
		nRet = (*((U64 *)pBuffer+nIndex)) & nBitsMask;
	}
	return nRet;
}

void SW_PMCP_SetValueEnhance(U8 *pBuffer, U32 nIndex, U32 nStep, U64 nValue)
{
	if (1 == nStep)									// byte
	{
		*((U8 *)pBuffer+nIndex) = (U8)nValue;
	}
	else if (2 == nStep)							// word
	{
		*((U16 *)pBuffer+nIndex) = (U16)nValue;
	}
	else if (4 == nStep)							// dword
	{
		*((U32 *)pBuffer+nIndex) = (U32)nValue;
	}
	else if (3 == nStep)							// bit
	{
		*(pBuffer+nIndex/8) &= (U8)(~(1<<(nIndex%8)));
		*(pBuffer+nIndex/8) |= (nValue & 0x01)<<(nIndex%8); //(1<<nIndex%8);
	}
	else if (8 == nStep)							// ddword
	{
		*((U64 *)pBuffer+nIndex) = nValue;
	}
}
void HAL_PMCPMultBaseDataUnzip(U8 bPmcpIdx, U32 dBaseSrcAddr, U32 dCntSrcAddr, U32 dResultAddr, U32 dDataInc, U32 dContinueNum, U32 dJumpColNum, U16 wBaseOpeCnt, U8 bDataStep)
{
	U64 i = 0;
	U64 j = 0;
	U64 pos = 0;
	U64 baseVal = 0;  // base值
	U64 unzipCnt = 0; // 解压个数

#if(1 == PMCP_PARAM_JUDGE_EN)
	// check step len
	if ((bDataStep != 2) && (bDataStep != 4) && (bDataStep != 8))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: bDataStep\n");
		return;
	}
	// check addr align
	if (((dBaseSrcAddr & (bDataStep-1))!=0) || (( dBaseSrcAddr & (bDataStep-1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: addr align\n");
		return;
	}
	if ((dContinueNum >= (1 << 24)) || (dJumpColNum >= (1 << 24)) || (wBaseOpeCnt >= (1 << 24)))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: Param_Error\n");
		return;
	}
	// check addr align
	if (((dBaseSrcAddr & (bDataStep-1))!=0) || ((dResultAddr & (bDataStep-1))!=0) || ((dCntSrcAddr & (4-1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: addr align\n");
		return;
	}
	// 判断解压个数是否有为0的情况
	for(i = 0; i < wBaseOpeCnt; i++)
	{
		unzipCnt = SW_PMCP_GetValueEnhance((U8 *)dCntSrcAddr, i, 4, 0xFF);	// 解压个数（数据步长4byte）
		if(unzipCnt == 0)
		{
			PRINT_TXT("[Error_Param] HAL_PMCPMultBaseDataUnzip: unzipCnt can't be zero!\n");
			return;
		}
	}
#endif

	for (i = 0; i < wBaseOpeCnt; i++)
	{
		baseVal = SW_PMCP_GetValueEnhance((U8 *)dBaseSrcAddr, i, bDataStep, 0xFF); // base值
		unzipCnt = SW_PMCP_GetValueEnhance((U8 *)dCntSrcAddr, i, 4, 0xFF);	// 解压个数（数据步长4byte）
		// 每个base的起始偏移值
		pos = (i * dContinueNum);

		for (j = 0; j < unzipCnt; j++)
		{
			SW_PMCP_SetValueEnhance((U8 *)dResultAddr, pos, bDataStep, baseVal + dDataInc * j);
			if ((j % dContinueNum) == (dContinueNum - 1))
			{
				// 到达连续解压单元末尾，加上跳列后的buffer索引
				pos += (dJumpColNum + 1);
			}
			else
			{
				pos++;
			}
		}
	}
}
#endif

/*
带bitmask的数组移位及段统计功能
dSrcBuff：源数据起始地址
dRetBuff：统计结果起始地址
bSrcStep：源数据步长（1，2，4，8）
bRetStep：结果数据步长（1，2，4）
bRightShiftNum：右移位数（0~63位）
ddMinVal：最小阈值
ddMaxVal：最大阈值
dOpeCnt：数组大小
ddBitMaskVal：bitmask值
bAddDir：段数组值加1减1开关（0：减1，1：加1）
返回值：段统计溢出标志，0：无溢出，1：溢出

注：仅支持PMCP0
*/
#if 1
U8 HAL_PMCPShiftStatisticsWithMask(U8 bPmcpIdx, U32 dSrcBuff, U32 dRetBuff, U8 bSrcStep, U8 bRetStep, U8 bRightShiftNum, U64 ddMinVal, U64 ddMaxVal, U32 dOpeCnt, U64 ddBitMaskVal, U8 bAddDir)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	// check step len
	if ((bSrcStep != 1) && (bSrcStep != 2) && (bSrcStep != 4) && (bSrcStep != 8))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bSrcStep ");
		return 0;
	}
	// check step len
	if ((bRetStep != 1) && (bRetStep != 2) && (bRetStep != 4))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bRetStep ");
		return 0;
	}
	// check data
	if (bRightShiftNum >= (1<<6) || (bPmcpIdx > 0) || (dOpeCnt == 0) || (bRightShiftNum >= 64)) // 值限制判断
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: Param_Error ");
		return 0;
	}
	// check data
	if ((bAddDir != 0) && (bAddDir != 1))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bAddDir ");
		return 0;
	}
	// check addr align
	if (((dSrcBuff & (bSrcStep-1))!=0) || ((dRetBuff & (bRetStep-1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: addr align ");
		return 0;
	}
#endif
	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = dSrcBuff;			// 源数据起始地址
	SgPMCP->rBASE_ADDR1 = dRetBuff;			// 移位统计结果存放地址
	SgPMCP->rOPE_CNT.bits.dVAL = dOpeCnt;	// 数组大小
	SgPMCP->rOPE_MODE |= 0x25 | (bAddDir << 8); // 带bitmask的数组移位及段统计模式/段数组值加1减1开关
	SgPMCP->rMIN_VAL = (U32)ddMinVal;	// 最小阈值
	SgPMCP->rMOVE_RLT0_OP_ARRL = (U32)(ddMinVal >> 32);
	SgPMCP->rMAX_VAL = (U32)ddMaxVal;	// 最大阈值
	SgPMCP->rMOVE_RLT0_OP_ARRH = (U32)(ddMaxVal >> 32);
	// 源数据步长[4bit](1,2,4,8byte)/目的数据步长[4bit](1,2,4byte)/右移位个数[6bit]
	SgPMCP->rOPE_CSR.dAll = (bSrcStep<<BIT_STEP_LEN) | (bRetStep<<BIT_RESULT_STEP_LEN) | (bRightShiftNum << BIT_DIFF_BIT_NUM);
	SgPMCP->rINVALID_VAL = (U32)(ddBitMaskVal >> 32);			// bitmask值高位
	SgPMCP->rBIT_MASK = (U32)ddBitMaskVal;					// bitmask值低位

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);
	return SG_PMCP(0)->rPMCP_STS.bits.OV_FLAG;
}
#else
U8 HAL_PMCPShiftStatisticsWithMask(U8 bPmcpIdx, U32 dSrcBuff, U32 dRetBuff, U8 bSrcStep, U8 bRetStep, U8 bRightShiftNum, U64 ddMinVal, U64 ddMaxVal, U32 dOpeCnt, U64 ddBitMaskVal, U8 bAddDir)
{
	U64 idx = 0;
	U64 srcVal = 0;
	U64 maskSrcVal = 0;
	U64 shiftMaskSrcVal = 0;
	U64 desCnt = 0;
	U8  overFlag = 0;				// 溢出标志[1bit]（0：无溢出，1：溢出）
	U64 maxLimitVal = 0;

#if(1 == PMCP_PARAM_JUDGE_EN)
	// check step len
	if ((bSrcStep != 1) && (bSrcStep != 2) && (bSrcStep != 4) && (bSrcStep != 8))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bSrcStep\n");
		return 0;
	}
	// check step len
	if ((bRetStep != 1) && (bRetStep != 2) && (bRetStep != 4))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bRetStep\n");
		return 0;
	}
	// check data
	if (bRightShiftNum >= (1<<6)) // 值限制判断
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bRightShiftNum\n");
		return 0;
	}
	// check data
	if ((bAddDir != 0) && (bAddDir != 1))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: bAddDir\n");
		return 0;
	}
	// check addr align
	if (((dSrcBuff & (bSrcStep-1))!=0) || ((dRetBuff & (bRetStep-1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPShiftStatisticsWithMask: addr align\n");
		return 0;
	}
#endif

	maxLimitVal = (((U64)1 << (bRetStep * 8)) - 1); // 计算出上溢出时的边界值

	for (idx = 0; idx < dOpeCnt; idx++)
	{
		srcVal = SW_PMCP_GetValueEnhance((U8 *)dSrcBuff, idx, bSrcStep, 0xFF);	// 提取源值

		maskSrcVal = srcVal & ~ddBitMaskVal; // bit mask操作

		// 判断是否在阈值区间范围内
		if ((maskSrcVal >= ddMinVal) && (maskSrcVal <= ddMaxVal))
		{
			shiftMaskSrcVal = maskSrcVal >> bRightShiftNum; // 右移位

			desCnt = SW_PMCP_GetValueEnhance((U8 *)dRetBuff, shiftMaskSrcVal, bRetStep, 0xFF); // 段统计

			if (1 == bAddDir)
			{
				// 计数加1
				if (maxLimitVal == desCnt) // 判断上溢出
				{
					overFlag = 1;
				}
				desCnt++;

			}
			else
			{
				// 计数减1
				if (0 == desCnt) // 判断下溢出
				{
					overFlag = 1;
				}
				desCnt--;
			}

			// 统计计数写入目的数据Buffer
			SW_PMCP_SetValueEnhance((U8 *)dRetBuff, shiftMaskSrcVal, bRetStep, desCnt);
		}
	}
	return overFlag;
}

#endif

/*
索引数据步长固定为2Byte
源数据步长固定为4Byte
与操作结果步长固定为2Byte
右移结果步长固定为4Byte
dMaskValue与数据直接进行与操作

注：移位范围为0~31，仅支持PMCP0
*/
#if 1
void HAL_PMCPArraShirtAndWithIndex(U8 bPmcpIdx, U32 *pdSrcBuff, U16 *pwIndexBuff,U16 wIndexCnt,U32 *pdShiftRetBuff1, U16 *pAndRetBuff2,U08 bRightShirtBitCnt,U32 dMaskValue)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	// check addr align
	if ((((U32)pdSrcBuff & (4-1))!=0) || (((U32)pwIndexBuff & (2-1))!=0) || (((U32)pdShiftRetBuff1 & (4-1))!=0) || (((U32)pAndRetBuff2 & (2-1))!=0) || (wIndexCnt == 0) || (bPmcpIdx > 0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPCellDataCompare: addr align ");
		return;
	}
	if (bRightShirtBitCnt >= 32)
	{
		PRINT_TXT("[Error_Param] HAL_PMCPCellDataCompare: bRightShirtBitCnt ");
		return;
	}
#endif
	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = (U32)pdSrcBuff;	// 源数据起始地址
	SgPMCP->rBASE_ADDR1 = (U32)pwIndexBuff;	// 索引数据起始地址
	SgPMCP->rRESULT_ADDR = (U32)pAndRetBuff2;	// 结果0起始地址(与操作)
	SgPMCP->rRMD0_ADDR = (U32)pdShiftRetBuff1;	// 结果1起始地址(右移操作)
	SgPMCP->rOPE_CNT.bits.dVAL = wIndexCnt;		// 数据总数
	// 带索引的矩阵多结果及综合运算模式/多结果输出使能/结果个数为2/关闭结果0移位/使能结果1移位/索引步长为2
	SgPMCP->rOPE_MODE |= 0x11 | (1 << 8) | (2 << 12) |(0 << 16) | (1 << 17) | (2 << 24);
	// 源数据步长为4/结果0步长为2
	SgPMCP->rOPE_CSR.dAll = (4<<BIT_STEP_LEN) | (2<<BIT_RESULT_STEP_LEN);
	SgPMCP->rBIT_MASK = 0;		// bit mask 值为0
	SgPMCP->rINVALID_VAL = 0;
	SgPMCP->rMAX_VAL = 4;		// 结果1步长为4
	SgPMCP->rMIN_VAL = (bRightShirtBitCnt << 6); 	//结果1右移位数
	// 结果0逻辑运算模式为与/结果1逻辑运算模式为或/结果1移位模式为右移
	SgPMCP->rMATRIX_MOVE_OP_SEL = 0 | (1 << 4 ) | (0 << 27) ;
	SgPMCP->rMOVE_RLT0_OP_ARRH = 0;								// 结果0综合运算的固定数
	SgPMCP->rMOVE_RLT0_OP_ARRL = dMaskValue;
	SgPMCP->rMOVE_RLT1_OP_ARRH = 0;								// 结果1综合运算的固定数为0
	SgPMCP->rMOVE_RLT1_OP_ARRL = 0;

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);

}
#else
void HAL_PMCPArraShirtAndWithIndex(U8 bPmcpIdx, U32 *pdSrcBuff, U16 *pwIndexBuff,U16 wIndexCnt,U32 *pdShiftRetBuff1, U16 *pAndRetBuff2,U08 bRightShirtBitCnt,U32 dMaskValue)
{
	U32 i = 0;
	U32 idx = 0;
	U32 srcVal = 0;
	U32 ret0Val = 0;
	U32 ret1Val = 0;

#if(1 == PMCP_PARAM_JUDGE_EN)
	// check addr align
	if ((((U32)pdSrcBuff & (4-1))!=0) || (((U32)pwIndexBuff & (2-1))!=0) || (((U32)pdShiftRetBuff1 & (4-1))!=0) || (((U32)pAndRetBuff2 & (2-1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPCellDataCompare: addr align\n");
		return;
	}
	if (bRightShirtBitCnt >= 32)
	{
		PRINT_TXT("[Error_Param] HAL_PMCPCellDataCompare: bRightShirtBitCnt\n");
		return;
	}
#endif

	for(i = 0; i < wIndexCnt; i++)
	{
		// 取索引数据
		idx = SW_PMCP_GetValueEnhance((U8 *)pwIndexBuff, i, 2, 0xFF); // 索引数据步长固定为2Byte

		// 取源数据
		srcVal = SW_PMCP_GetValueEnhance((U8 *)pdSrcBuff, idx, 4, 0xFF); // 源数据步长固定为4Byte

		ret0Val = srcVal;
		ret1Val = srcVal;

		ret0Val >>= bRightShirtBitCnt; // 右移
		ret1Val &= dMaskValue; // 与操作

		SW_PMCP_SetValueEnhance((U8 *)pdShiftRetBuff1, i, 4, ret0Val); // 右移结果步长固定为4Byte
		SW_PMCP_SetValueEnhance((U8 *)pAndRetBuff2, i, 2, ret1Val); // 与操作结果步长固定为2Byte
	}
}
#endif

/*
分段归类排序第一步：初始化段链表 （该功能可以用于设置一片连续的memory）
dSegListAddr：段链表地址
bStep：段链表步长（当作为memory set功能时，步长可设置为1/2/4/8，当作为分段归类排序功能时，步长只能设置为2/4）
bVaule：设置的值（当作为分段归类排序功能时，段链表需初始化为全F）
bLen：段链表长度（以步长为单位）

注：1、在作为分段归类排序功能中时，在第一步，段链表步长等于中间链表/排序结果步长，但在第二/三步，段链表步长会变为中间链表/排序结果步长的两倍，
	       相当于段链表总长度缩小了一半，因此在初始化链表时，需保证段链表总长度为中间链表总长度的两倍
	2、仅支持PMCP0
*/
void HAL_PMCPDataSortSeg_Step1(U8 bPmcpIdx, U32 dSegListAddr, U8 bStep, U64 bVaule, U32 bLen)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if ((bPmcpIdx != 0) || (bLen == 0))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step1: Param_Error ");
		return;
	}
	if (!((bStep == 1) || (bStep == 2) || (bStep == 4) || (bStep == 8)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step1: Param_Error ");
		return;
	}
	if (bLen >= (1 << 24))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step1: Param_Error ");
		return;
	}
	if((dSegListAddr & (bStep - 1))!=0)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step1: addr align ");
		return;
	}
#endif
	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR1 = dSegListAddr;		// 段链表的地址
	SgPMCP->rRESULT_CNT = bLen;				// 段链表初始化长度
	SgPMCP->rMOVE_RLT3_OP_ARRL = bVaule;	// 段链表初始化值
	SgPMCP->rMOVE_RLT3_OP_ARRH = bVaule >> 32;
	SgPMCP->rOPE_CSR.dAll = (bStep<<BIT_RESULT_STEP_LEN); // 段链表步长
	SgPMCP->rOPE_MODE |= 0x24 | (0x1 << 9); // 单独使能第一步

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);
}

// 分段归类排序第二步：生成段链表和中间链表
//
// 注：仅支持PMCP0
#if 1
void HAL_PMCPDataSortSeg_Step2(U8 bPmcpIdx,T_STEP2_PARAM1 * ptParam1, T_STEP2_PARAM2 * ptParam2, T_DATA_SORT_SEG_RESULT *ptSortResult)
{
	U8 dSegListStep = 0;

	dSegListStep = ptParam1->bMiddleStep * 2; // 第二步时，段链表步长为中间链表步长的两倍

#if(1 == PMCP_PARAM_JUDGE_EN)
	if ((bPmcpIdx != 0) || (ptParam1->bDataCnt == 0))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error ");
		return;
	}
	if (!((ptParam1->bDataStep == 2) || (ptParam1->bDataStep == 4) || (ptParam1->bDataStep == 8)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error ");
		return;
	}
	if (!((ptParam1->bMiddleStep == 2) || (ptParam1->bMiddleStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error ");
		return;
	}
	if (ptParam1->bDataCnt >= (1 << 24))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error ");
		return;
	}
	if(ptParam1->bSegBitWide >= (ptParam1->bDataStep * 8))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error ");
		return;
	}
	if((ptParam1->dSegListAddr & (dSegListStep - 1))!=0 || (ptParam1->dDataAddr & (ptParam1->bDataStep - 1))!=0 || (ptParam1->dMiddleListAddr & (ptParam1->bMiddleStep - 1))!=0)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: addr align ");
		return ;
	}
#endif
	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = ptParam1->dDataAddr;		// 源数据的起始地址
	SgPMCP->rBASE_ADDR1 = ptParam1->dSegListAddr;	// 段链表的地址
	SgPMCP->rRESULT_ADDR = ptParam1->dMiddleListAddr; // 中间链表的地址
	SgPMCP->rMOVE_RLT1_OP_ARRL = (U32)ptParam2->dInVaildMaskMin; // 源数据阈值最小值
	SgPMCP->rMOVE_RLT1_OP_ARRH = (U32)(ptParam2->dInVaildMaskMin >> 32);
	SgPMCP->rMOVE_RLT2_OP_ARRL = (U32)ptParam2->dInVaildMaskMax; // 源数据阈值最大值
	SgPMCP->rMOVE_RLT2_OP_ARRH = (U32)(ptParam2->dInVaildMaskMax >> 32);
	SgPMCP->rMOVE_RLT0_OP_ARRL = (U32)ptParam2->dVaildMaskMin; // bitmask后数据最小阈值
	SgPMCP->rMOVE_RLT0_OP_ARRH = (U32)(ptParam2->dVaildMaskMin >> 32);
	SgPMCP->rMIN_VAL = (U32)ptParam2->dVaildMaskMax;		  // bitmask后数据最大阈值
	SgPMCP->rMAX_VAL = (U32)(ptParam2->dVaildMaskMax >> 32);
	SgPMCP->rBIT_MASK = (U32)ptParam2->ddFrontEndMask;			// bitmask值
	SgPMCP->rINVALID_VAL = (U32)(ptParam2->ddFrontEndMask >> 32);
	SgPMCP->rBIT_MASK2 = (U32)ptParam2->ddBackEndMask; // dVaildMaskMax和dVaildMaskMin阈值过滤后再次进行bitmask的值
	SgPMCP->rMOVE_RLT4_OP_ARRH = (U32)(ptParam2->ddBackEndMask >> 32);
	// 数据项步长/中间链表和排序结果数据步长/右移位数
	SgPMCP->rOPE_CSR.dAll = (ptParam1->bDataStep<<BIT_STEP_LEN) | (ptParam1->bMiddleStep<<BIT_RESULT_STEP_LEN) | (ptParam1->bSegBitWide<<BIT_DIFF_BIT_NUM);
	SgPMCP->rOPE_CNT.dAll = ptParam1->bDataCnt;	// 数据项总数
	SgPMCP->rOPE_MODE |= 0x24 | (0x1 << 10); // 分段归类排序模式/单独使能第二步

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);

	ptSortResult->dVaildItemNum = SgPMCP->rGOT_RESULT_CNT.bits.dVAL; // 有效项总个数
	ptSortResult->dVaildSegCnt = SgPMCP->rBIT_RESULT.bits.dVAL; // 有效段总个数
	ptSortResult->dInValidItemCnt = SgPMCP->rRESULT_CNT2; // 落在dInVaildMaskMax和dInVaildMaskMin之间的个数
	ptSortResult->dValidMinSeg = SgPMCP->rRESULT_CNT3; // 有效段最小段号
}
#else
void HAL_PMCPDataSortSeg_Step2(U8 bPmcpIdx,T_STEP2_PARAM1 * ptParam1, T_STEP2_PARAM2 * ptParam2, T_DATA_SORT_SEG_RESULT *ptSortResult)
{
	U64 idx = 0;

	U64 srcVal = 0;     // 源数据
	U64 srcMaskVal = 0;	// mask后的数据
	U64 segListVal = 0;	// 段链表值
	U64 itemNum = 0;    // 段号
	U64 segListStartNode = 0;  // 段链表开始节点
	U64 segListEndNode = 0;	   // 段链表结束节点
	U32 segListNodeBitLen = 0; // 段链表中间位置的bit长度，等于段链表步长 * 8bit / 2
	U32 middleListVal  = 0;	   // 存放中间链表的数据
	U32 tempSegCnt	   = 0;    // 用于记录每段包含的项个数
	U64 vaildSegTotal = 0;		 // 有效段总个数
	U64 validIdxTotal = 0;		 // 有效项总个数
	U8 dSegListStep = 0;		 // 段链表步长

	U32 dSrcVaildNum = 0; // 源数据满足阈值范围个数
	U32 dValidMinSeg = 0xFFFFFFFF; // 有效段最小段号

	dSegListStep = ptParam1->bMiddleStep * 2; // 第二步时，段链表步长为中间链表步长的两倍

#if(1 == PMCP_PARAM_JUDGE_EN)
	if (bPmcpIdx != 0 && bPmcpIdx != 1)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error\n");
		return;
	}
	if (!((ptParam1->bDataStep == 2) || (ptParam1->bDataStep == 4) || (ptParam1->bDataStep == 8)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error\n");
		return;
	}
	if (!((ptParam1->bMiddleStep == 2) || (ptParam1->bMiddleStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error\n");
		return;
	}
	if (ptParam1->bDataCnt >= (1 << 24))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error\n");
		return;
	}
	if(ptParam1->bSegBitWide >= (ptParam1->bDataStep * 8))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: Param_Error\n");
		return;
	}
	if((ptParam1->dSegListAddr & (dSegListStep - 1))!=0 || (ptParam1->dDataAddr & (ptParam1->bDataStep - 1))!=0 || (ptParam1->dMiddleListAddr & (ptParam1->bMiddleStep - 1))!=0)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step2: addr align\n");
		return ;
	}
#endif

	segListNodeBitLen = (dSegListStep * 8) / 2; // 段链表中间位置的bit长度

	// 更新段链表和中间链表
	for (idx = 0; idx < ptParam1->bDataCnt; idx++)
	{
		srcVal = SW_PMCP_GetValueEnhance((U8 *)ptParam1->dDataAddr, idx, ptParam1->bDataStep, 0xFF); // 提取源值

		if (srcVal >= ptParam2->dInVaildMaskMin && srcVal <= ptParam2->dInVaildMaskMax) // 与源数据阈值进行比较，统计满足范围的源数据个数
		{
			dSrcVaildNum++;
		}

		srcMaskVal = srcVal & ~ptParam2->ddFrontEndMask; // bit mask操作

		if ((srcMaskVal >= ptParam2->dVaildMaskMin) && (srcMaskVal <= ptParam2->dVaildMaskMax)) // 判断是否在阈值区间范围内
		{
			validIdxTotal++; // 有效项总数加1

			srcMaskVal &= ~ptParam2->ddBackEndMask; // 再次进行bitmask

			itemNum = srcMaskVal >> ptParam1->bSegBitWide; // 右移位取段号

			if (itemNum > (((U64)1 << segListNodeBitLen) - 1))	// 段号>结束节点为最大值（例：0xFFFF）时，异常处理
			{
				PRINT_TXT("[Error_Param] SW_PMCP_DataSortSeg: itemNum over\n");
				return ;
			}

			dValidMinSeg = min(dValidMinSeg, itemNum); // 记录当前最小段号

			// 根据段号取段链表值
			segListVal = SW_PMCP_GetValueEnhance((U8 *)ptParam1->dSegListAddr, itemNum, dSegListStep, 0xFF);
			// 算出段链表开始节点和结束节点
			segListStartNode = (segListVal >> segListNodeBitLen) & (((U64)1 << segListNodeBitLen) - 1);
			segListEndNode = segListVal & (((U64)1 << segListNodeBitLen) - 1);

			/* 若链表起始节点为空(0xFFFF),则将该原始数值的项号i存入该链表起始节点。若起始节点为非空（比如值k），则
			将该项号i存入到结束节点。并将值i填入段中间链表数组TmpL第k项(结束节点项)。*/
			if ((((U64)1 << segListNodeBitLen) - 1) == segListStartNode)	// 开始节点为最大值（例：0xFFFF）
			{
				vaildSegTotal++; // 分段段号总数加1

				// 链表起始节点为空，将项号存入起始节点
				segListStartNode = idx;
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegListAddr, itemNum, dSegListStep, (segListStartNode << segListNodeBitLen) | (((U64)1 << segListNodeBitLen) - 1));
			}
			else if ((((U64)1 << segListNodeBitLen) - 1) == segListEndNode)
			{
				// 链表起始节点不为空，结束节点为空，取起始节点更新中间链表
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dMiddleListAddr, segListStartNode, ptParam1->bMiddleStep, idx);	// 以结束节点值为索引的位置更新中间链表

				// 段链表结束节点更新为新index
				segListEndNode = idx;
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegListAddr, itemNum, dSegListStep, (segListStartNode << segListNodeBitLen) | segListEndNode);
			}
			else
			{	// 链表起始节点和结束节点都不为空，取结束节点更新中间链表
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dMiddleListAddr, segListEndNode, ptParam1->bMiddleStep, idx);	// 以结束节点值为索引的位置更新中间链表

				// 段链表结束节点更新为新index
				segListEndNode = idx;
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegListAddr, itemNum, dSegListStep, (segListStartNode << segListNodeBitLen) | segListEndNode);
			}


		}
	}

	// 赋值给结果结构体
	ptSortResult->dVaildSegCnt = vaildSegTotal;
	ptSortResult->dVaildItemNum = validIdxTotal;
	ptSortResult->dInValidItemCnt = dSrcVaildNum;
	ptSortResult->dValidMinSeg = dValidMinSeg;
}
#endif

// 分段归类排序第三步：生成结果
//
// 注：仅支持PMCP0
#if 1
void HAL_PMCPDataSortSeg_Step3(U8 bPmcpIdx,T_STEP3_PARAM1 *ptParam1,T_STEP3_PARAM2 *ptParam2)
{
	U8 dSegListStep = 0;

	dSegListStep = ptParam1->bMidAndResStep * 2; // 第三步时，段链表步长为中间链表/排序结果步长的两倍

#if(1 == PMCP_PARAM_JUDGE_EN)
	if ((bPmcpIdx != 0) || (ptParam1->dSegListLen == 0))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error ");
		return;
	}
	if (!((ptParam1->bMidAndResStep == 2) || (ptParam1->bMidAndResStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error ");
		return;
	}
	if (!((ptParam1->bRetOffsStep == 2) || (ptParam1->bRetOffsStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error ");
		return;
	}
	if (!((ptParam1->bSegCntStep == 2) || (ptParam1->bSegCntStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error ");
		return;
	}
	if (ptParam1->dSegListLen >= (1 << 24))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error ");
		return;
	}
	if (!((ptParam2->bClrListEn == 0) || (ptParam2->bClrListEn == 1)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error ");
		return;
	}
	if((ptParam1->dSegListAddr & (dSegListStep-1))!=0 || (ptParam1->dMiddleListAddr & (ptParam1->bMidAndResStep-1))!=0 || (ptParam1->dSegOrderResAddr & (ptParam1->bMidAndResStep-1))!=0 || (ptParam1->dRetOffsAddr & (ptParam1->bRetOffsStep-1))!=0 || (ptParam1->dSegNumAddr & (ptParam1->bSegNumStep-1))!=0 || (ptParam1->dSegCntAddr & (ptParam1->bSegCntStep-1))!=0)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: addr align ");
		return;
	}
#endif

	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;					// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR1 = ptParam1->dSegListAddr;		// 段链表的地址
	SgPMCP->rRESULT_ADDR = ptParam1->dMiddleListAddr;	// 中间链表的地址
	SgPMCP->rRMD0_ADDR = ptParam1->dSegOrderResAddr;	// 分段排序结果Ret数组地址
	SgPMCP->rRMD1_ADDR = ptParam1->dRetOffsAddr;		// 排序后每段索引位置RetOffs
	SgPMCP->rRMD2_ADDR = ptParam1->dSegNumAddr;			// 排序后每段段号SegNum
	SgPMCP->rRMD3_ADDR = ptParam1->dSegCntAddr;			// 排序后每段包含的项个数SegCnt
	SgPMCP->rOPE_CSR.dAll = (ptParam1->bMidAndResStep<<BIT_RESULT_STEP_LEN);   // 中间链表和排序结果数据步长(2，4)
	SgPMCP->rMATRIX_MOVE_OP_SEL = ptParam1->bSegCntStep | (ptParam1->bRetOffsStep << 4) | (ptParam1->bSegNumStep << 8);
	SgPMCP->rMOVE_RLT4_OP_ARRL = ptParam1->dSegListLen;    // 段链表初始化长度
	SgPMCP->rMOVE_RLT5_OP_ARRL = ptParam2->dValidMinSeg;   // 有效段最小段号
	SgPMCP->rMOVE_RLT5_OP_ARRH = ptParam2->bValidItemCnt;  // 段链表有效项个数
	SgPMCP->rOPE_MODE |= 0x24 | (ptParam2->bClrListEn << 8) | (0x1 << 11); // 分段归类排序模式/清除链表信息使能/单独使能第三步

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);

}
#else
void HAL_PMCPDataSortSeg_Step3(U8 bPmcpIdx,T_STEP3_PARAM1 *ptParam1,T_STEP3_PARAM2 *ptParam2)
{
	U64 idx = 0;
	U32 tempSegCnt = 0;   	// 用于记录每段包含的项个数
	U64 segListVal = 0;		// 段链表值
	U32 segListNodeBitLen = 0; // 段链表中间位置的bit长度，等于段链表步长 * 8bit / 2
	U32 segListStartNode = 0;  // 段链表开始节点
	U32 segListEndNode = 0;	   // 段链表结束节点
	U32 middleListVal  = 0;	   // 存放中间链表的数据
	U32 retIdx = 0; // Ret数组的index
	U32 segIdx = 0; // RetOffs、SegNum、SegCnt数组的idx
	U8 dSegListStep = 0;	   // 段链表步长
	U32 validIdxNum = 0;		 // 用于记录有效项个数

	dSegListStep = ptParam1->bMidAndResStep * 2; // 第三步时，段链表步长为中间链表/排序结果步长的两倍

#if(1 == PMCP_PARAM_JUDGE_EN)
	if (bPmcpIdx != 0 && bPmcpIdx != 1)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error\n");
		return;
	}
	if (!((ptParam1->bMidAndResStep == 2) || (ptParam1->bMidAndResStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error\n");
		return;
	}
	if (!((ptParam1->bRetOffsStep == 2) || (ptParam1->bRetOffsStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error\n");
		return;
	}
	if (!((ptParam1->bSegCntStep == 2) || (ptParam1->bSegCntStep == 4)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error\n");
		return;
	}
	if (ptParam1->dSegListLen >= (1 << 24))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error\n");
		return;
	}
	if (!((ptParam2->bClrListEn == 0) || (ptParam2->bClrListEn == 1)))
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: Param_Error\n");
		return;
	}
	if((ptParam1->dSegListAddr & (dSegListStep-1))!=0 || (ptParam1->dMiddleListAddr & (ptParam1->bMidAndResStep-1))!=0 || (ptParam1->dSegOrderResAddr & (ptParam1->bMidAndResStep-1))!=0 || (ptParam1->dRetOffsAddr & (ptParam1->bRetOffsStep-1))!=0 || (ptParam1->dSegNumAddr & (ptParam1->bSegNumStep-1))!=0 || (ptParam1->dSegCntAddr & (ptParam1->bSegCntStep-1))!=0)
	{
		PRINT_TXT("[Error] HAL_PMCPDataSortSeg_Step3: addr align\n");
		return;
	}
#endif

	segListNodeBitLen = (dSegListStep * 8) / 2; // 段链表中间位置的bit长度

	// 从最小有效段号开始搜寻段链表，计算出四个结果数组
	for(idx = ptParam2->dValidMinSeg; idx < ptParam1->dSegListLen; idx++)
	{
		tempSegCnt = 0; // 清零

		segListVal = SW_PMCP_GetValueEnhance((U8 *)ptParam1->dSegListAddr, idx, dSegListStep, 0xFF); // 取段链表值

		if(segListVal == (((U64)1 << dSegListStep * 8) - 1)) // 判断段链表值是否为全F
		{
			continue;
		}
		else
		{
			// 算出段链表开始节点和结束节点
			segListStartNode = (segListVal >> segListNodeBitLen) & (((U64)1 << segListNodeBitLen) - 1);
			segListEndNode = segListVal & (((U64)1 << segListNodeBitLen) - 1);

			if((((U64)1 << segListNodeBitLen) - 1) == segListEndNode) // 如果开始节点和结束节点相等，说明该段只有1项
			{
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegOrderResAddr, retIdx, ptParam1->bMidAndResStep, segListStartNode); // 设置Ret数组

				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dRetOffsAddr, segIdx, ptParam1->bRetOffsStep, retIdx); // 设置RetOffs数组

				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegNumAddr, segIdx, ptParam1->bSegNumStep, idx); // 设置SegNum数组

				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegCntAddr, segIdx, ptParam1->bSegCntStep, 1); // 设置SegCnt数组

				retIdx++;
				segIdx++;
				validIdxNum++;
			}
			else
			{
				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegOrderResAddr, retIdx, ptParam1->bMidAndResStep, segListStartNode); // 设置Ret数组

				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dRetOffsAddr, segIdx, ptParam1->bRetOffsStep, retIdx); // 设置RetOffs数组
				retIdx++;
				tempSegCnt++;
				validIdxNum++;

				SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegNumAddr, segIdx, ptParam1->bSegNumStep, idx); // 设置SegNum数组

				middleListVal = SW_PMCP_GetValueEnhance((U8 *)ptParam1->dMiddleListAddr, segListStartNode, ptParam1->bMidAndResStep, 0xFF); // 根据开始节点到中间链表取数据

				while(1)
				{
					if(middleListVal == segListEndNode) // 说明到最后一项
					{
						SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegOrderResAddr, retIdx, ptParam1->bMidAndResStep, middleListVal); // 设置Ret数组
						retIdx++;
						tempSegCnt++;
						validIdxNum++;

						SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegCntAddr, segIdx, ptParam1->bSegCntStep, tempSegCnt); // 设置SegCnt数组

						break;
					}
					else
					{
						SW_PMCP_SetValueEnhance((U8 *)ptParam1->dSegOrderResAddr, retIdx, ptParam1->bMidAndResStep, middleListVal); // 设置Ret数组
						retIdx++;
						tempSegCnt++;
						validIdxNum++;
					}
					middleListVal = SW_PMCP_GetValueEnhance((U8 *)ptParam1->dMiddleListAddr, middleListVal, ptParam1->bMidAndResStep, 0xFF); // 到中间链表取数据
				}
				segIdx++;
			}
		}

		if (validIdxNum == ptParam2->bValidItemCnt) // 搜索到的有效项个数等于有效项总个数，提前退出
		{
			break;
		}
	}

	if (ptParam2->bClrListEn == 1) // 清除段链表信息使能
	{
		memset((U8 *)ptParam1->dSegListAddr, 0xFF, ptParam1->dSegListLen * dSegListStep); // 将段链表初始化为全F
	}
}
#endif

//带索引带数据的内存值设定
//pdOldValuebase：指向旧数据的base地址，默认步长为U32
//pwIndexBuff：指向index的buff地址，默认步长为U16
//pdNewValueBuff：指向new value的buff地址，默认步长为U32
//dCntBuffAddr：cnt的buff地址
//bStepOfCnt：描述cnt的步长（1，2，4，8）
//dMaskValue：对old value或new value的mask操作的值
//bRightShirtNum：mask操作后右移的bit数
//wTotalIndexCnt：需要操作的总数
//
//注：仅支持PMCP0
#if 1
void HAL_PMCPValueReplaceAndCntCalc(U32 *pdOldValuebase,U16 *pwIndexBuff,U32 *pdNewValueBuff,U32 dCntBuffAddr,U08 bStepOfCnt,U32 dMaskValue,U08 bRightShirtNum,U16 wTotalIndexCnt)
{
#if(1 == PMCP_PARAM_JUDGE_EN)
	if ((bStepOfCnt != 1) && (bStepOfCnt != 2) && (bStepOfCnt != 4) && (bStepOfCnt != 8))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPValueReplaceAndCntCalc: bStepOfCnt  ");
		return;
	}
	if ((wTotalIndexCnt == 0) || (bRightShirtNum >= 32))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPValueReplaceAndCntCalc: Param_Error ");
		return;
	}
	if(((((U32)pdOldValuebase) & (4 - 1))!=0) || ((((U32)pwIndexBuff) & (2 - 1))!=0) || ((((U32)pdNewValueBuff) & (4 - 1))!=0) || ((dCntBuffAddr & (bStepOfCnt - 1))!=0))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPValueReplaceAndCntCalc: addr align ");
		return;
	}
#endif
	HAL_PMCPWaitDone(0);
	SG_PMCP_TypeDef *SgPMCP = SG_PMCP(0);
	SgPMCP->rOPE_MODE = 0;						// 初始化为0，防止控制位需要都清零
	SgPMCP->rBASE_ADDR0 = (U32)pwIndexBuff;		// 索引数据起始地址
	SgPMCP->rBASE_ADDR1 = (U32)pdNewValueBuff;	// 数值数组起始地址
	SgPMCP->rRESULT_ADDR = (U32)pdOldValuebase;	// 源数据起始地址
	SgPMCP->rRMD0_ADDR = dCntBuffAddr;		// 源数据右移位统计结果地址
	SgPMCP->rRMD1_ADDR = dCntBuffAddr;		// 替换值右移位统计结果地址(同上，两这共用一个统计buff)
	SgPMCP->rRESULT_CNT = wTotalIndexCnt;		// 需要替换的数据总数
	SgPMCP->rMAX_VAL = 4 | (bStepOfCnt << 4) | (bStepOfCnt << 8);	// 源数据步长/新旧值移位统计结果步长为相同值
	SgPMCP->rOPE_CSR.dAll = (2<<BIT_STEP_LEN) | (4<<BIT_RESULT_STEP_LEN); // 索引数组步长为2/数值数组步长为4
	SgPMCP->rMIN_VAL = bRightShirtNum | (bRightShirtNum << 8); // 新旧值右移位数为相同值
	// 带索引带数据的内存值设定模式/旧值移位统计使能/旧值移位统计为减1模式/新值移位统计使能/新值移位统计为加1模式
	SgPMCP->rOPE_MODE |= 0x1C | (1 << 8) | (0 << 9) | (1 << 12) | (1 << 13) | (1 << 16);
	SgPMCP->rBIT_MASK = dMaskValue; // bitmask
	SgPMCP->rINVALID_VAL = 0;
	SgPMCP->rMOVE_RLT0_OP_ARRL = 0;  		// 源数据阈值最小值
	SgPMCP->rMOVE_RLT0_OP_ARRH = 0;
	SgPMCP->rMOVE_RLT1_OP_ARRL = 0xFFFFFFFF;// 源数据阈值最大值
	SgPMCP->rMOVE_RLT1_OP_ARRH = 0;
	SgPMCP->rMOVE_RLT2_OP_ARRL = 0; 		// 替换值阈值最小值
	SgPMCP->rMOVE_RLT2_OP_ARRH = 0;
	SgPMCP->rMOVE_RLT3_OP_ARRL = 0xFFFFFFFF;// 替换值阈值最大值
	SgPMCP->rMOVE_RLT3_OP_ARRH = 0;

	SgPMCP->rPMCP_CTRL.bits.INT_EN = 0;
	SgPMCP->rOP_START.bits.dVAL = 1;

	HAL_PMCPWaitDone(0);

}
#else
void HAL_PMCPValueReplaceAndCntCalc(U32 *pdOldValuebase,U16 *pwIndexBuff,U32 *pdNewValueBuff,U32 dCntBuffAddr,U08 bStepOfCnt,U32 dMaskValue,U08 bRightShirtNum,U16 wTotalIndexCnt)
{
	U64 i=0;
	U64 idx = 0;
	U64 ddNewVal = 0; // 新数据
	U64 ddOldVal = 0; // 旧数据
	U64 ddNewCnt = 0; // 新数据统计结果计数
	U64 ddOldCnt = 0; // 旧数据统计结果计数

	U8 bOldValueStep = 4; // 旧数据步长固定为4byte
	U8 bIndexStep = 2;	  // index数组步长固定为2byte
	U8 bNewValueStep = 4; // 新数据步长固定为4byte

#if(1 == PMCP_PARAM_JUDGE_EN)

	if ((bStepOfCnt != 1) && (bStepOfCnt != 2) && (bStepOfCnt != 4) && (bStepOfCnt != 8))
	{
		PRINT_TXT("[Error_Param] HAL_PMCPValueReplaceAndCntCalc: bStepOfCnt \n");
		return;
	}

#endif

	for (i = 0; i < wTotalIndexCnt; i++)
	{
		idx = SW_PMCP_GetValueEnhance((U8 *)pwIndexBuff, i, bIndexStep, 0xFF); // 索引

		ddNewVal = SW_PMCP_GetValueEnhance((U8 *)pdNewValueBuff, i, bNewValueStep, 0xFF); // 替换值，即新数据

		ddOldVal = SW_PMCP_GetValueEnhance((U8 *)pdOldValuebase, idx, bOldValueStep, 0xFF); // 被替换值,即旧数据

		SW_PMCP_SetValueEnhance((U8 *)pdOldValuebase, idx, bOldValueStep, ddNewVal); //执行替换

		ddOldVal &= ~dMaskValue; // 对旧数据进行bitmask
		ddOldVal >>= bRightShirtNum; // 对旧数据进行右移
		ddOldCnt = SW_PMCP_GetValueEnhance((U8 *)dCntBuffAddr, ddOldVal, bStepOfCnt, 0xFF); // 取出旧数据对应位置的cnt值
		ddOldCnt--; // 旧数据固定为减操作
		SW_PMCP_SetValueEnhance((U8 *)dCntBuffAddr, ddOldVal, bStepOfCnt, ddOldCnt); // 设置旧数据对应位置的cnt值

		ddNewVal &= ~dMaskValue; // 对新数据进行bitmask
		ddNewVal >>= bRightShirtNum; // 对新数据进行右移
		ddNewCnt = SW_PMCP_GetValueEnhance((U8 *)dCntBuffAddr, ddNewVal, bStepOfCnt, 0xFF); // 取出新数据对应位置的cnt值
		ddNewCnt++; // 新数据固定为加操作
		SW_PMCP_SetValueEnhance((U8 *)dCntBuffAddr, ddNewVal, bStepOfCnt, ddNewCnt); // 设置新数据对应位置的cnt值

	}
}
#endif
#endif
#else
// 打开SW_PMCP时，使用下面的接口
void HAL_PMCPMemorySet(U08 dPmcpIdx, U32 dSrchRamAddr, U32 dVal, U32 dLen)
{
	U32 *pDest = (U32 *) dSrchRamAddr;
	U32 i=0;

	dLen >>= 2 ;		//div 4和FW PMCP 保持一致

	while(i < dLen)
	{
		pDest[i]=dVal;
		i++;
	}
}

void  HAL_PMCPInit(void)
{

}
#endif

