/*
 * SATA_AbormalHandle.h
 *
 *  Created on: Nov 15, 2019
 *      Author: Sun-PC
 */

#ifndef SATA_ABORMALHANDLE_H_
#define SATA_ABORMALHANDLE_H_
#include "sg9081.h"
#include "sata_typedef.h"
#include "dsata.h"

#ifdef _SATA_TRIGGER_CRC__
//#define  _SATA_TEST_PC_CEHCK_     //PC 端Check 状态
//#define _SATA_512K_RAM_SCAN_


typedef struct _SATA_debug
{
	//256 byte
	//1
	U8  bTrigCrcFlow;  //  触发 CRC 的流程，0x5A: non data fis 触发crc test   0xf3: data fis 触发crc test
	U8  bErrFisNum;
	U8  bErrTag;
	U8  bWrOrRdOp;  // 读写操作
	U32 dSetTrigCnt;
	U32 dTrigNum;
	U32 dTotalcnt;  //循环了多少次
	//2
	U32 dTotalcnt_old; // 循环的值,
	U8  bNonDATAFisCase;  //   预收4K \ DMA 命令  \NCQ_32
	U8  bCmdTpye;    //DMA 命令还是NCQ 命令
	U8  bErrCmdType; //出错的命令类型
	U8  bTrigStatus;  //已触发   未触发
	U8  bSetStatus;  //已配置   未配置
	U8  bSetStatus1;  //已配置   未配置
	U8  bSetStatus2;  //已配置   未配置
	U8  bSetStatus3;  //已配置   未配置
	U32 dFailTotalSec;
	//3
	U32 dCRC_PASS_IDX_Err;     //保存old
	U32 dCRC_PASS_IDX; //保存当前的
	U32 dCRC_PASS_MIN_SET;
	U8  bSATA_FALSE_FLG;
	U8  bCRC_PASS_IDX_FALSE;
	U8  bSata_FALSE_FLG_Err;   //SATA_FALSE_FLG 错误
	U8  bMIN_SET_FALSE;
	//4
	U32 dPush_idx_Err;
	U32 dPush_idx_Current;
	U32 dPop_idx;
	U8  bPush_idx_flg;
	U8  bPop_idx_flg;
	U8  bCmdCnt;
	U8  bCmdCnt_flg;
	//5
	U32 dDMA_Idx_old;
	U32 dDMA_Idx;
	U32 dCrcFailDMA_IDX;
	U8  bDma_idx_flg;
	U8  bCrcFailDma_idx_flg;
	U8  bDmaResever1;
	U8  bDmaResever2;
	//6
	U32 dreorderDly;
	U32 dSDBDly;
	U32 ddatafisCnt;
	U8  bOpDly; //选择reorder 还是SDB  Dly
	U8  bDly_Flg;
	U8  bDlyStatus;
	U8  bDly_Flg2;
	//7
	U32 dPre_4K_DATA;
	U32 dPRE_4K_IDX;
	U32 dPRE_4K_cnt;
	U32 dPRE_4K_DoneFlg;
	//8
	U32 dTag_dulErr;
	U32 dLBACrossErr;
	U32 dComResetFlg;
	U32 dSoftResetFlg;
	//9
	U32 dReorderTotal_Err;
	U32 dReorderTotal;
	U32 dReorderDoneTotal_Err;
	U32 dReorderDoneTotal;
	//10
	U32 dRegFlg;
	U32 dRegFailFlg;
	U32 dRegFlg2;
	U32 dRegFailFlg2;
	//10
//	U32 bBuff[24];
}T_SATA_DEBUG;


extern T_SATA_DEBUG  tSataTest;

void Abnormal_SataCtrl_NONDATA_FisCrcFail(void);

void Abnormal_Sata_DisCrcFail(void);
void  Abnormal_Sata_Ctrl(U8* pbTag,U8 bCnt);
void  Abnormal_SataFail_SaveLog(void);
void  Abnormal_SataCheck(void);
void  Abnormal_Sata_INFS_Check(void);
void  Abnormal_Sata_IFS_Check(void);
void  Abnormal_SataTest_Init(void);
#endif

#ifdef  __FW_TRIGGER_SATA_CRC__




// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_H2D_FisCrcFail(void)
// 函数功能: H2D_FIS 植CRC错,启动后  Device 接收到第一个H2D 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
//void Abnormal_SataCtrl_H2D_FisCrcFail(void);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_D2H_FisCrcFail(void)
// 函数功能: D2H_FIS 植CRC错,启动后  Device 发送第一个D2H 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
//void Abnormal_SataCtrl_D2H_FisCrcFail(void);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_DMAActive_FisCrcFail
// 函数功能: DMA_Active_FIS 植CRC错,启动后  Device 发送第一个DMA_Active 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
//void Abnormal_SataCtrl_DMAActive_FisCrcFail(void);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_DMASetup_FisCrcFail
// 函数功能: 启动后  Device 发送第一个 DMASetup 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
//void Abnormal_SataCtrl_DMASetup_FisCrcFail(void);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_PIOSetup_FisCrcFail
// 函数功能: 启动后  Device 发送第一个 PIOSetup 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
//void Abnormal_SataCtrl_PIOSetup_FisCrcFail(void);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_SDB_FisCrcFail
// 函数功能: 启动后  Device 发送第一个SDB 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
//void Abnormal_SataCtrl_SDB_FisCrcFail(void);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_NONDATA_FisCrcFail
// 函数功能: 触发 NON DATA FIS 发生CRC
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************


void Abnormal_SataCtrl_DATA_FisCrcFail_WR(U8 bErrTag,U16 wErrFisNum,U8 bDir);

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_NONDATA_FisCrcFail
// 函数功能: 触发 NON DATA FIS 发生CRC
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_NONDATA_FisCrcFail(void);

#endif

#endif /* SATA_ABORMALHANDLE_H_ */
