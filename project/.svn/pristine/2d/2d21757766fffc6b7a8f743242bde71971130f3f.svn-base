/*
 * ErrEmbed.h
 *
 *  Created on: Dec 27, 2017
 *      Author: webb
 */

#ifndef INC_ERREMBED_H_
#define INC_ERREMBED_H_

#include "data_type.h"


enum ERR_MODE/*错误模式*/
{
	ALL_FAIL=0,	/*簇的两个DMA都FAIL*/
	FIRST_FAIL,	/*簇的第一个DMA FAIL*/
	SECOND_FAIL,/*簇的第二个DMA FAIL*/
	ALL_ONE,	/*簇都是全1数据*/
};

typedef struct
{
	U32 dBadOffs;
	U08 bBadMode;
	U08 bRetryCnt;		//retry次数
}INSERT_ERROR_INFO;


typedef struct
{
	U32 dBadCnt;
	U32 g_Dma2LenCfg[4];
	U32 g_dDma2En[4];
	INSERT_ERROR_INFO *InserErrorInfo;
	U32 dSecondBadOfst;
	U16 wDebug;	
}RETRY_CASE_INFO;


RETRY_CASE_INFO tRetryCaseInfo;

enum DMA1_DIR	{SATA_HOST_OUT=0, SATA_HOST_IN=1};


#if 0/*跳列重新构建*/

/*跳列配置信息*/
typedef struct                                            
{   
	union
	{
		U32 dAll;
		struct
		{
			U32 dJCBaseAddr0_14:15; /*跳列信息基地址*/
			U32 dJCByte15_24:10;	/*当前DMA一共跳列的Byte数(不能超过0x3FF)*/
			U32 dReserved25_31:7;	/*保留*/
		}bits;
	}ALL_JC_CFG_INFO;

#if 0
	U32 dJCBaseAddr0_14:15;	/*跳列信息基地址*/
	U32 dJCByte15_24:10;	/*当前DMA一共跳列的Byte数(不能超过0x3FF)*/
   	U32 dReserved25_31:7;	/*保留*/
#endif
}T_DMA2_JC_CFG_INFO;

/*跳列信息*/
typedef struct                                            
{                                                         
	union
	{
		U32 dAll;
		struct
		{
			U32 dDMA2Col0_11:12;	/*跳列起始位置DMA2的列地址*/
			U32 dJCByteCur12_15:4;	/*当前次跳列Byte数(1-E:1-14Byte;0:32Byte;1:64Byte)*/
			U32 dJCInfoEndFlag16_31:16;/*结束标志*/
		}bits;
	}DMA2_JC_INFO;

#if 0
	U32 dDMA2Col0_11:12;	/*跳列起始位置DMA2的列地址*/
	U32 dJCByteCur12_15:4;	/*当前次跳列Byte数(1-E:1-14Byte;0:32Byte;1:64Byte)*/
	U32 dJCInfoEndFlag16_31:16;/*结束标志*/
#endif

}T_DMA2_JC_INFO;


typedef struct                                            
{                                                         
    U32 dDMA2JCBaseAddr;
	U32 dDMA2JCTableAddr;

	T_DMA2_JC_CFG_INFO *tDMA2CfgInfo;
	U32 dCfgInfoSize;/*跳列配置信息BuffSize(4Byte)*/

	T_DMA2_JC_INFO *tDMA2JCInfo;
	U32 dJCInfoSize;/*跳列区间信息BuffSize(2Byte)*/
	
}T_DMA2_JC_TABLE;

T_DMA2_JC_TABLE tDMA2JCTable;
#endif


void FDL_InsertErrorInit(void);
void FDL_InsertErrorInfoInit(void);
void FDL_InsertErrorSecondDma(U08 bChan, U32 dOffs);
void FDL_InsertErrorFirstDma(U08 bChan, U32 dOffs, U08 bFlashIdx);
void FDL_InsertError(U08 bChan, U32 dOffs, U16 *pwCol, U08 *pbNextOpMode, U32 dRow);
void FDL_InsertErrorCreatJCTable(U32 dDma2CfgInfoBAddr,U32 dDma2CfgInfoBSize,U16 wPageCnt);
void FDL_InsertErrorDealRetryCnt(U08 bChan, U32 dOfst, U08 bCurRetryCnt, U16 *wpCol, U32 dRow);


#endif /* INC_ERREMBED_H_ */
