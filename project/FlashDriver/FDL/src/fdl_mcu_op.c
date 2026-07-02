#include "fdl_mcu_op.h"
#include "hal.h"
#include "uitility.h"

// *****************************************************************************
//  版权所有  (C) 2017,  硅格半导体有限公司
//
//  文件名称：  fdl_mcu_op.c
//  文件标识：
//  内容摘要：  实现FDL对Flash的MCU操作
//  其它说明：  !!!函数名需以"Fdl_"前缀命名函数，以区分以"FDL_"开头的对外接口!!!
//  当前版本：  v0.10
//  项    目：  SG9081
//
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容
//  修改日期：
//  版 本 号：
//  修 改 人：
//  修改内容：
// *****************************************************************************


void Fdl_MCUPreRead(U08 bChan,U08 bFlashMode)
{
#if(NAND_FLASH == TOS_BICS3)
	HAL_MCU_SendCmd(bChan,0x5D);
	Delay(20);
#endif

	if(bFlashMode == 0)
	{
		FL_CMD_MCU_SLC_MODE(bChan);
	}
	else
	{
		FL_CMD_MCU_TLC_MODE(bChan);
		FL_CMD_MCU_ED3_READ_WL(bChan,bFlashMode);
	}

	delay_us(1);
}


void Fdl_MCU_MultiPlnCacheReadCmd(U08 bChan,U08 bChip,U08 bLun,U32 dRowAddr)
{
	U08 bPln;
	U32 dRowOp = (dRowAddr & PLANE_BIT_MASK_OF_ROW)&0xFFFFFF;//Pln0的起始地址

	HAL_MCUCheckStatus(bChan, bChip);

#ifdef __CACHE_READ_ONLY_31H__
	HAL_MCU_SendCmd(bChan,FL_CMD_CACHE_REDA);
	Delay(5);		
	HAL_MCUCheckStatus(bChan, bChip);
#else
	for(bPln=0;bPln<PLANE_OF_LUN;bPln++)
	{
		Fdl_MCUPreRead(bChan,ARRAY(dRowAddr,3));
		HAL_MCU_SendCmd(bChan,FL_CMD_READ1);
		HAL_MCU_SendTwoAddr(bChan,0);
		HAL_MCU_SendThreeAddr(bChan,dRowOp);

		if(bPln==(PLANE_OF_LUN-1))
		{
			HAL_MCU_SendCmd(bChan,FL_CMD_CACHE_REDA);
		}
		else
		{
			HAL_MCU_SendCmd(bChan,FL_CMD_READ_MP);
		}
		
		HAL_MCUCheckStatus(bChan, bChip);
		dRowOp += ROW_GAP_OF_PLANE;
	}
#endif
}

void Fdl_MCUMultiPlnReadCmd(U08 bChan,U08 bChip,U08 bLun,U32 dRowAddr)
{
	U08 bPln;
	U32 dRowOp = (dRowAddr & PLANE_BIT_MASK_OF_ROW)&0xFFFFFF;//Pln0的起始地址

	HAL_MCUCheckStatus(bChan, bChip);
	
	for(bPln=0;bPln<PLANE_OF_LUN;bPln++)
	{					
	#if (NAND_FLASH != TOS_BICS3) && (NAND_FLASH != HYNIX_8A1M)
		if(bPln == 0)
	#endif
		{
			Fdl_MCUPreRead(bChan,ARRAY(dRowAddr,3));
		}

		HAL_MCU_SendCmd(bChan,FL_CMD_READ1);
		HAL_MCU_SendTwoAddr(bChan,0);
		HAL_MCU_SendThreeAddr(bChan,dRowOp);

		if(bPln==(PLANE_OF_LUN-1))
		{
			HAL_MCU_SendCmd(bChan,FL_CMD_READ2);
		}
		else
		{
			HAL_MCU_SendCmd(bChan,FL_CMD_READ_MP);
		}
		
		HAL_MCUCheckStatus(bChan, bChip);
		dRowOp += ROW_GAP_OF_PLANE;
	}
}

void Fdl_MCUReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode)
{    
	HAL_MCUCheckStatus(bChan, bChip);
	Delay(5);
	Fdl_MCUPreRead(bChan,bFlashMode);//前缀(0xDA/0xDF 0x01/0x02/0x03)
	Delay(5);
	HAL_MCU_SendCmd(bChan,FL_CMD_READ1);
	HAL_MCU_SendTwoAddr(bChan,wColAddr);
	HAL_MCU_SendThreeAddr(bChan,dRowAddr);
	HAL_MCU_SendCmd(bChan,FL_CMD_READ2);
	Delay(10);
	
	HAL_MCUCheckStatus(bChan, bChip);
    Delay(5);
	HAL_MCU_SendCmd(bChan,FL_CMD_EXIT_CHK_IOSTA);
    HAL_MCU_SendCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
	HAL_MCU_SendTwoAddr(bChan,wColAddr);
	HAL_MCU_SendThreeAddr(bChan,dRowAddr);
    HAL_MCU_SendCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
	Delay(50);
}

void Fdl_MCUSendPreReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode)
{
	HAL_MCUCheckStatus(bChan, bChip);
	Fdl_MCUPreRead(bChan,bFlashMode);//前缀(0xDA/0xDF 0x01/0x02/0x03)
	
	HAL_MCU_SendCmd(bChan,FL_CMD_READ1);
	HAL_MCU_SendTwoAddr(bChan,wColAddr);
	HAL_MCU_SendThreeAddr(bChan,dRowAddr);
	HAL_MCU_SendCmd(bChan,FL_CMD_READ2);
	Delay(10);
}

void Fdl_MCUReadCacheCmd(U8 bChan, U8 bChip, U16 wColAddr)
{
	HAL_MCUCheckStatus(bChan, bChip);
    HAL_MCU_SendCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
	HAL_MCU_SendTwoAddr(bChan,wColAddr);
    HAL_MCU_SendCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
	Delay(10);
}

void Fdl_MCUReadOneDma(U8 bChan,U16 wBufIndex, U8 bFlashIdx, U16 wDmaSect, U32 dDmaBuffAddr, U32 dDmaInfoAddr)
{
    Joint_Mode JointMode = HAL_ADMGetJointMode(bChan);
    
    HAL_ADMStartDMA2(bChan,dDmaBuffAddr,dDmaInfoAddr,DMA2_RD_TO_BUFF,wBufIndex,bFlashIdx,wDmaSect,JointMode);
	HAL_ADMWaitDMADone(bChan); 
}


