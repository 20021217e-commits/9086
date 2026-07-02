
#include "hal_config.h"
#include "hal.h"
#include "hal_define.h"
#include "fdl_read_retry.h"
#include "uitility.h"
#include "fdl_common.h"
#if (FUNC_ENABLE == __HAL_FC_DEFINE__)

inline void HAL_FCSetUdEnable(U8 bChan, U8 bNewState)
{
    SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = bNewState;
}

#endif

#if (FUNC_ENABLE == __HAL_NF_DEFINE__)

U8 HAL_NFSendPluse(U8 bChan,U8 bMode)
{
    U8 bReturnVal = 0xFF;
    
    if(WR_PULSE == bMode)
    {
        SG_FC(bChan)->rSEND_WR_PULSE.bits.dVAL = 1;
    }
    else if(RD_PULSE == bMode)
    {
        SG_FC(bChan)->rSEND_RD_PULSE.bits.dVAL = 1;
    }
    __nds32__dsb();

    while(!(SG_FC(bChan)->rRW_PULSE_DONE.bits.dVAL))
	{
		;
	}

    if(RD_PULSE == bMode)
    {
        bReturnVal = SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA;
    }

    return bReturnVal;
   
}

U8 HAL_NFResetFlash(U8 bChan, U8 bChip,U08 bCmd)
{
#if 1
    U08 bStatus;

	HAL_MCU_SetCe(bChan,bChip,1);
	Delay(0x40);
	HAL_MCU_SendCmd(bChan,bCmd);	
  	Delay(8000*200);
    return bStatus; 
#else
    HAL_CQChipSelect(bChan, bChip);
    HAL_CQCfgLogicNum(bChan,((U32)0x50<<16));
    HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
    HAL_CQCheckIOStatus(bChan, LUN_NULL, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
    HAL_CQWaitDone(bChan);
#endif
}


void HAL_NFResetLun(U8 bChan, U8 bChip, U8 bLun, U32 dRow)
{
    HAL_CQChipSelect(bChan, bChip);
    HAL_CQCfgLogicNum(bChan,((U32)0x51<<16));
    HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_LUN);
    HAL_CQSendRowAddr(bChan,dRow);
    HAL_CQCheckIOStatus(bChan, LUN_NULL, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
    HAL_CQWaitDone(bChan);
}

//PreCmdFunction>0时，发5Ch-C5h指令
void HAL_FA_DQSSetParameter(U08 bChan, U08 bChip, U08 bAddr, U08 Param, U08 PreCmdFunction, U08 SetCmd)
{
		
	HAL_MCU_SetCe(bChan,bChip,1);
	Delay(3);
	if(PreCmdFunction>0)
	{
		HAL_MCU_SendCmd(bChan,0x5C);
		HAL_MCU_SendCmd(bChan,0xC5);		
	}
	HAL_MCU_SendCmd(bChan,SetCmd);
	HAL_MCU_SendOneAddr(bChan,bAddr);
	Delay(4);
//	HAL_MCUSendDataByDqs(bChan,Param);
	HAL_McuSendWRPulse(bChan,Param);
	//Fdl_MCU_CheckStatus(bChan, bChip);		//并不需要等RB Done
}


#ifdef __VMC_EN__
void __section(".hal_init") HAL_NFReadFlashID(U32 dBuffAddr, U8 bChan, U8 bChip)
#else
void  HAL_NFReadFlashID(U32 dBuffAddr, U8 bChan, U8 bChip)
#endif
{
#if 1
#if ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_N38A) || (NAND_FLASH == INTEL_N38B))
	SG_FC(bChan)->rCHK_ST_TIM_CFG.bits.CKST_TSMP = 0x3;	// B27B需修改采样点为3，才能读到ID
#else
	SG_FC(bChan)->rCHK_ST_TIM_CFG.bits.CKST_TSMP = 0x4;//改了采样点，跑低速时调小是高速，比如200MHz，就要把采样点调大
#endif
	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x00;
	HAL_NFResetFlash(bChan, bChip,FL_CMD_RESET_FLASH);

    U08 bSysClkSource 	= SG_CPM->rSYS_CLK_SOURCE_SEL;
	U08 bTrx2ClkSource 	= HAL_CPMGetClkSource(0);
	U08 bSysClkPeriod 	= SG_CPM->rSYS_CLK_PERIOD;;
	U08 bTrx2ClkPeriod 	= HAL_CPMGetTrx2Period(bChan);

	if(bTrx2ClkPeriod < 8)//在较低的分频值读取ID
	{
#ifndef __FPGA_PLATEFORM
		HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod);//HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,8,8,8,8);
#else
		HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,32,32,32,32);
#endif
	}

  	HAL_MCU_SendCmd(bChan,FL_CMD_READID);
  	Delay(0x10);
  	HAL_MCU_SendOneAddr(bChan,0x00);
  	Delay(0x100);

	U08 i;
  	U08 *data = (U8 *)dBuffAddr;

	for (i = 0; i < 8; i++)
	{
		data[i] = HAL_McuSendReadPluse(bChan);
	}

	if(bTrx2ClkPeriod < 8)
	{
		HAL_SystemInit(bSysClkSource,bSysClkPeriod,bSysClkSource,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod);
	}

	Fdl_ClearExceptionState(bChan);
#else
	U08 bRnd_ByPass = SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS;
	U08 bUserDataEn =  SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN;
	U08 bEccCfg = SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN;
    U16 wDmaLen = HAL_ADMGetDMALen(bChan);

	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;

	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x00;
	HAL_NFResetFlash(bChan, bChip,FL_CMD_RESET_FLASH);

	SG_FC(bChan)->rNF_CFG.bits.RD_SMP_CFG = 2;

    HAL_ADMSetDMALen(bChan,8);

    U08 bSysClkSource 	= SG_CPM->rSYS_CLK_SOURCE_SEL;
	U08 bTrx2ClkSource 	= HAL_CPMGetClkSource(0);
	U08 bSysClkPeriod 	= SG_CPM->rSYS_CLK_PERIOD;
	U08 bTrx2ClkPeriod 	= HAL_CPMGetTrx2Period(bChan);
	
	if(bTrx2ClkPeriod < 8)//在较低的分频值读取ID
	{
#ifndef __FPGA_PLATEFORM
		HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,8,8,8,8);
#else
		HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,32,32,32,32);
#endif
	}

    HAL_CQChipSelect(bChan,bChip);
    HAL_CQSendFlashCmd(bChan,FL_CMD_READID);
    HAL_CQSendAddr(bChan,0x00);

    U08 i;
	for(i = 0; i < 6;i++)
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;	//读无效CE的ID，不加delay，返回的就有可能不是FF
	}

    HAL_CQStartDMA2(bChan,dBuffAddr,DMA2_RD_TO_BUFF,0x00,0x00,0x01,JOINT_NONE, (U32)NULL);
    HAL_CQWaitDone(bChan);

    HAL_ADMSetDMALen(bChan,wDmaLen);
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = bRnd_ByPass;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = bUserDataEn;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = bEccCfg;
	
	if(bTrx2ClkPeriod < 8)
	{
		HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod);
	}

	Fdl_ClearExceptionState(bChan);
#endif
}

void HAL_NFEraseBlock(U8 bChan, U8 bChip, U32 dRowAddr)
{
    HAL_CQChipSelect(bChan,bChip);
    HAL_CQSendFlashCmd(bChan,FL_CMD_BLOCK_ERASE1);
    HAL_CQSendRowAddr(bChan,dRowAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_BLOCK_ERASE2);
}


void HAL_NFDebugCE(U08 bCnt)
{
    SG_PADCTRL->rPORT_MODE[1].dAll &= ~(1<<1);//ce1输出模式
    SG_PADCTRL->rPORT_DATA[1].dAll &= ~(1<<1);//H
    SG_PADCTRL->rPAD_FUNC_CTRL[0].bits.FCE1 = GPIO_MODE; //设置为GPIO模式
    
    while(bCnt--)
    {
        SG_PADCTRL->rPORT_DATA[1].dAll |= (1<<1);//L
        Delay(0x1);
        SG_PADCTRL->rPORT_DATA[1].dAll &= ~(1<<1);//H
    }
}

U08 HAL_McuSetFeature(U08 bChan, U08 bChip, U08 bAddr,U08 bSetMode,U08 bByteCnt, U08 *data)
{
	U08 i = 0;
	
	HAL_MCUCheckStatus(bChan, bChip);		//需要等RB Done
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,bAddr);
	Delay(4);
	
	if(bSetMode == 1)/*DQS模式*/
	{
		HAL_MCUSendNByteDataByDqs(bChan,data,bByteCnt);
	}
	else/*写脉冲模式*/
	{
		for (i = 0; i < bByteCnt; i++)
		{
			HAL_McuSendWRPulse(bChan,data[i]);
		}
	}
	HAL_MCUCheckStatus(bChan, bChip);		//需要等RB Done

	return 0;
}

U08 HAL_McuGetFeature(U08 bChan, U08 bChip, U08 bAddr, U08 *data)
{
  //U08 i = 0;
  HAL_MCUCheckStatus(bChan, bChip);    //需要等RB Done
  HAL_MCU_SendCmd(bChan,FL_CMD_GET_FEATURE);
  HAL_MCU_SendOneAddr(bChan,bAddr);
  HAL_MCUCheckStatus(bChan, bChip);    //需要等RB Done


#if 0
#if (NAND_FLASH == TOS_BICS3)
  data[0] = HAL_McuSendReadPluse(bChan);/*8T23和9T23需要一个Dummy Read,其他Flash尚不明确*/
#endif

  for (i = 0; i < 4; i++)
  {
    data[i] = HAL_McuSendReadPluse(bChan);
  }
#else
	HAL_McuSendReadPluseGetData(bChan,data,4);
#endif
	return 0;
}
U08 HAL_NFSetFeatureWithCheck(U08 bChan, U08 bChip, U08 bAddr, U08 *data,U08 bDataNum,U08 bSetMode)
{
	U08 i = 0;
	U08 bRet = TRUE;
	U08 bGetData[4] = {0,0,0,0};
	HAL_NFSetFeature(bChan,bChip,bAddr,data,bSetMode);
	HAL_NFGetFeature(bChan,bChip,bAddr,bGetData,4);
	
	for(i = 0;i < bDataNum;i++)
	{
		if(bGetData[i] != data[i])
		{		
			if(bSetMode == 0)
			{				
				LOG_PRINT("[%d-%d]WR Pulse SetFeature Fail[0x%02X-0x%02X(Error>0x%02X)]\r\n",bChan,bChip,bAddr,data[i],bGetData[i]);
			}
			else
			{
				LOG_PRINT("[%d-%d]DQS Pulse SetFeature Fail[0x%02X-0x%02X(Error>0x%02X)]\r\n",bChan,bChip,bAddr,data[i],bGetData[i]);
			}
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}



void HAL_NFSetFeature(U08 bChan, U08 bChip, U08 bAddr, U08 *data,U08 bSetMode)
{
	U08 i = 0;
#if 1/*MCU方式*/
	HAL_McuSetFeature(bChan,bChip,bAddr,bSetMode,4,data);/*支持DQS模式和写脉冲模式Set Feature*/
#else/*CQ方式*/
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
	
//	HAL_CQSendFlashCmd(bChan,FL_CMD_SET_FEATURE);
//	HAL_CQSendAddr(bChan,bAddr);
	HAL_CQWaitDone(bChan);

	HAL_McuSetFeature(bChan,bChip,bAddr,bSetMode,4,data);/*支持DQS模式和写脉冲模式Set Feature*/
	
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
	HAL_CQWaitDone(bChan);
#endif
}

void HAL_NFGetFeature(U08 bChan, U08 bChip, U08 bAddr, U08 *data, U08 bDataNum)
{
	U08 i = 0;
#if 1/*Mcu方式*/
	HAL_McuGetFeature(bChan,bChip,bAddr,data);
#else/*CQ方式*/
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
	
	HAL_CQSendFlashCmd(bChan,0xEE);
	HAL_CQSendAddr(bChan,bAddr);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
	HAL_CQSendFlashCmd(bChan,FL_CMD_EXIT_CHK_IOSTA);
	HAL_CQWaitDone(bChan);
	
#if 0
	for (i = 0; i < bDataNum; i++)
	{
		data[i] = HAL_McuSendReadPluse(bChan);
	}
#else
	HAL_McuSendReadPluseGetData(bChan,data,bDataNum);
#endif
#endif
}

void HAL_NFGetExtFeature(U08 bChan, U08 bChip, U08 bAddr1, U08 bAddr2, U08 bAddr3, U08 *data, U08 bDataNum)
{
  U08 i = 0;
#if 1
	 HAL_MCU_SetCe(bChan,bChip,1);
#if ((NAND_FLASH>4095)&&(NAND_FLASH<5120))	//YMTC flash
		 HAL_MCU_SendCmd(bChan,0xDD);
		 HAL_MCU_SendOneAddr(bChan,bAddr1);
		 HAL_MCU_SendOneAddr(bChan,bAddr2);
		 Delay(10);//跳过RB
#else
		 HAL_MCU_SendCmd(bChan,0xEA);
		 HAL_MCU_SendOneAddr(bChan, bAddr1);
		 HAL_MCU_SendOneAddr(bChan, bAddr2);
		 HAL_MCU_SendOneAddr(bChan, bAddr3);
		 Delay(0x40);
		 HAL_MCUCheckStatus(bChan, bChip);
		 HAL_MCU_SendCmd(bChan,0);
#endif
	 
	 HAL_McuSendReadPluseGetData(bChan,data,4);

//	 for (i = 0; i < bDataNum; i++)
//	 {
//		 data[i] = HAL_McuSendReadPluse(bChan);
//	 }

#else
	  HAL_CQChipSelect(bChan, bChip);
	  HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
  
	  HAL_CQSendFlashCmd(bChan,0xEA);
	  HAL_CQSendAddr(bChan,bAddr1);
	  HAL_CQSendAddr(bChan,bAddr2);
	  HAL_CQSendAddr(bChan,bAddr3);
	  HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
	  HAL_CQSendFlashCmd(bChan,FL_CMD_EXIT_CHK_IOSTA);
	  HAL_CQWaitDone(bChan);
  
	  for (i = 0; i < bDataNum; i++)
	  {
	    data[i] = HAL_McuSendReadPluse(bChan);
	  }
#endif
}

void HAL_NFSetExtFeature(U08 bChan, U08 bChip, U08 bAddr1, U08 bAddr2, U08 bAddr3, U08 *data, U08 bCnt)
{
#if 1
  U08 i = 0;
  HAL_MCU_SetCe(bChan,bChip,1);
  #if ((NAND_FLASH>4095)&&(NAND_FLASH<5120))	//YMTC flash
	HAL_MCU_SendCmd(bChan,0xDD);	//Set Extended Feature Command
	HAL_MCU_SendOneAddr(bChan,bAddr1);
	HAL_MCU_SendOneAddr(bChan,bAddr2);
	HAL_McuSendWRPulse(bChan, data[0]);
	HAL_McuSendWRPulse(bChan, 0x00);
	HAL_McuSendWRPulse(bChan, 0x00);
	Delay(30);
  #else	
    HAL_MCU_SendCmd(bChan,0xEB);
    HAL_MCU_SendOneAddr(bChan, bAddr1);
    HAL_MCU_SendOneAddr(bChan, bAddr2);
    HAL_MCU_SendOneAddr(bChan, bAddr3);
    Delay(0x40);
    HAL_MCUSendDataByDqs_GAI(bChan,data[0],0x10);
    HAL_MCUSendDataByDqs_GAI(bChan,data[1],0x01);
    HAL_MCUCheckStatus(bChan, bChip);
  #endif
 #else
	 U08 i = 0;
  HAL_CQChipSelect(bChan, bChip);
  HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
  
  HAL_CQSendFlashCmd(bChan,0xEB);
  HAL_CQSendAddr(bChan,bAddr1);
  HAL_CQSendAddr(bChan,bAddr2);
  HAL_CQSendAddr(bChan,bAddr3);
  SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;
  HAL_CQWaitDone(bChan);
    
  for (i = 0; i < bCnt; i++)
  {
    HAL_McuSendWRPulse(bChan,data[i]);
  }

  HAL_CQCheckIOStatus(bChan,0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
  HAL_CQWaitDone(bChan);
#endif
}



void HAL_NFReadRetryTab(U32 dBuffAddr)
{
	U8 bChan = 0;
	U8 bChip = 0;
	U16 wDmaLen = 0;
	for(bChan = 0; bChan < 1; bChan ++)
	{
		SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE	=	0x00;
		wDmaLen = HAL_ADMGetDMALen(bChan);
		HAL_NFResetFlash(bChan,bChip,FL_CMD_RESET_FLASH);
		HAL_CQSendFlashCmd(bChan,0x17);
		HAL_CQSendFlashCmd(bChan,0x04);
		HAL_CQSendFlashCmd(bChan,0x19);
		HAL_CQSendFlashCmd(bChan,0xDA);

		HAL_CQSendFlashCmd(bChan,0x00);
		HAL_CQSendAddr(bChan, 0x00);
		HAL_CQSendAddr(bChan, 0x00);
		HAL_CQSendAddr(bChan, 0x24);
		HAL_CQSendAddr(bChan, 0x02);
		HAL_CQSendAddr(bChan, 0x00);
		HAL_CQSendFlashCmd(bChan,0x30);
		HAL_CQCheckIOStatus(bChan, LUN_NULL, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
		HAL_CQSendFlashCmd(bChan,FL_CMD_EXIT_CHK_IOSTA);

		HAL_ADMSetDMALen(bChan,0x200);
		HAL_CQStartDMA2(bChan,dBuffAddr,DMA2_RD_TO_BUFF,0x00,0x00,0x01,JOINT_NONE, (U32)NULL);
		HAL_CQWaitDone(bChan);
		HAL_ADMSetDMALen(bChan,wDmaLen);
	}
}

void HAL_NFSetRetyReg(U8 bChan,U8 bValue)
{
	SG_FC(bChan)->rMCU_CFG_CLE.dAll = MCU_SEND_CMD_HEAD | 0x36;
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}

	SG_FC(bChan)->rMCU_CFG_ALE.dAll = MCU_SEND_CMD_HEAD | 0x96;
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
	HAL_MCUSendDataByDqs(bChan,0x04);

	SG_FC(bChan)->rMCU_CFG_ALE.dAll = MCU_SEND_CMD_HEAD | 0x7D;
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
	HAL_MCUSendDataByDqs(bChan,0x00);

	SG_FC(bChan)->rMCU_CFG_ALE.dAll = MCU_SEND_CMD_HEAD | 0x96;
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
	HAL_MCUSendDataByDqs(bChan,0x00);

	SG_FC(bChan)->rMCU_CFG_ALE.dAll = MCU_SEND_CMD_HEAD | 0x4E;
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
	HAL_MCUSendDataByDqs(bChan,bValue);

	SG_FC(bChan)->rMCU_CFG_CLE.dAll = MCU_SEND_CMD_HEAD | 0x16;
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
}



void HAL_MCUSendNByteDataByDqs(U8 bChan,U8 *data,U08 bDataLen)
{
	U08 i = 0;
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);/*DQS是Port2,Bit6--Data设置为低*/
	SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);/*DQS是Port2,Bit6--输出模式*/
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = GPIO_MODE;/*DQS先设置为GPIO模式*/
	SG_PADCTRL->rPORT_MODE[bChan*3].bits.dVAL = GPIO_OUT;/*IO线设置为输出模式*/
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQ = GPIO_MODE;/*IO线设置为GPIO模式*/
	__nds32__dsb();

	for (i = 0; i < bDataLen; i++)
	{
		SG_PADCTRL->rPORT_DATA[bChan*3].bits.dVAL = data[i];
		__nds32__dsb();
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(6);
		__nds32__dsb();
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);
		__nds32__dsb();
	}

	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(6);/*DQS是Port2,Bit6--Data设置为高*/
	SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);/*DQS是Port2,Bit6--输出模式*/
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = GPIO_MODE;/*DQS先设置为GPIO模式*/
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = FUCN_MODE;/*DQS先设置为Function模式*/	
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQ = FUCN_MODE;/*IO线设置为Function模式*/
	__nds32__dsb();
}

void HAL_McuSendReadPluseGetData(U8 bChan,U8 *data,U08 bDataLen)
{
	U08  i = 0;

	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(2);/*RE是Port2,Bit2--Data 设置为0*/
	SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(2);/*RE是Port2,Bit2--输出模式*/
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FRE = GPIO_MODE;/*RE先设置为GPIO模式*/
	__nds32__dsb();

#if 1//((NAND_FLASH == TOS_BICS3) || (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == YMTC_B08T))/*8T23需要多一个脉冲*/
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(2);/*RE是Port2,Bit2--Data 设置为0*/
	data[0] = SG_PADCTRL->rPORT_DATA[bChan*3].bits.dVAL;
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(2);/*RE是Port2,Bit2--Data 设置为1*/
#endif

	for(i = 0;i < bDataLen;i++)
	{
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(2);/*RE是Port2,Bit2--Data 设置为0*/
		data[i] = SG_PADCTRL->rPORT_DATA[bChan*3].bits.dVAL;
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(2);/*RE是Port2,Bit2--Data 设置为1*/
	}

	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FRE= FUCN_MODE;/*恢复为Function模式*/

}




void HAL_MCUSendDataByDqs(U8 bChan,U8 bValue)
{
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);
	SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = GPIO_MODE;
	SG_PADCTRL->rPORT_MODE[bChan*3].bits.dVAL = GPIO_OUT;
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQ = GPIO_MODE;
	SG_PADCTRL->rPORT_DATA[bChan*3].bits.dVAL = bValue;
	__nds32__dsb();
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(6);
	__nds32__dsb();
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = FUCN_MODE;
	 __nds32__dsb();
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQ = FUCN_MODE;
	__nds32__dsb();
}



void HAL_MCUSendDataByDqs_GAI(U8 bChan,U8 bValue,U8 Stat)
{
	if(0x10==(Stat&0xF0))////Set GPIO Output Mode & Pull DQS Down, Preamble
	{
		//DQ GPIO
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);
		SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = GPIO_MODE;
		SG_PADCTRL->rPORT_MODE[bChan*3].bits.dVAL = GPIO_OUT;
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQ = GPIO_MODE;
		__nds32__dsb();
		__nds32__dsb();
	}

	SG_PADCTRL->rPORT_DATA[bChan*3].bits.dVAL = bValue;
	__nds32__dsb();

	//inverse DQS
	if((SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL&0x40) == 0x40)//High
	{
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(6);
	}
	else//Low
	{
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(6);
	}
	__nds32__dsb();


	if(0x01==(Stat&0x0F))////Postamble
	{
		/////Pull ALE & DQS High
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(1);
		SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(1);
		__nds32__dsb();
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = GPIO_MODE;
		__nds32__dsb();
		SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(6);
		__nds32__dsb();
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQS = FUCN_MODE;
		__nds32__dsb();
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FDQ = FUCN_MODE;
		__nds32__dsb();
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = FUCN_MODE;
	}

}



void HAL_NFSwitchJCTable(U32 dJCTableAddr,U08 bJCEn)
{
	U08 bChan;
	
	for(bChan = 0; bChan < 4; bChan++)	//切换4个通道的跳列表配置地址
	{
		SG_FC(bChan)->rDMA2_FILTER_ECC_CFG_BADDR = dJCTableAddr; //DMA配置信息地址	
#if (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)/*B08T 跳列功能要一直开着,其他Flash只有填充,读的时候可以关掉跳列*/
#else
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.FILT_EN = bJCEn; //打开跳列功能
#endif
	}
}

#ifdef __FW__
U8  __section(".CODE_INIT") HAL_NFFillPage(U16 wDMABasicLen,U16 wECCParity,U08 bUDataAttachSize,U08 bUDataLen,U08 bTotalSect,U16 wPageSize,U32 wDma2FiltBAddr)
#else
U8 HAL_NFFillPage(U16 wDMABasicLen,U16 wECCParity,U08 bUDataAttachSize,U08 bUDataLen,U08 bTotalSect,U16 wPageSize,U32 wDma2FiltBAddr)
#endif
//创建跳列表
{
	if(wDMABasicLen != 2048)
	{
		return FALSE;
	}
	U08 bDMACnt = bTotalSect/(wDMABasicLen/512);	//表示一页多少个DMA
	U08 bTotalUserDataLen = bUDataAttachSize==0 ? 0 :(bTotalSect/bUDataAttachSize) * bUDataLen;	//userData的总长度
	U16 wJCMaxByte = (wPageSize - bTotalUserDataLen - bDMACnt*(wDMABasicLen + wECCParity));		//最大跳列个数
	U32 dJCTableBaseAddr = wDma2FiltBAddr;	
	U08 bJCTableBuffLen	= (8*6); // 每个通道的跳列占据的空间 8:DMA个数 6：每个DMA配置信息占6Byte
	U32 bJCTableOffset	= 0x20; // (8)*4 = 0x20	
	U32 dDMA2JCBaseAddr = 0;
	U32 dDMA2JCTableAddr = 0;
	U16 wDMA2JCCnt = 0x0000;
	U32 dVaule0_14 = 0;	//4byte 的第一部分（表示2Byte RAM地址）
	U32 dValue15_24 = 0;	//4byte 的第二部分（表示跳列的个数）
	U08 bChan = 0;
	U08 i = 0;
	
	for(bChan = 0; bChan < 4; bChan++)	//创建4个通道的跳列表
	{
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.REDUANT_FILL_DATA_RND_EN = 1; //在FLASH未写满的区域填入随机数
		__nds32__dsb();
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.FILT_EN = 1; //打开跳列功能
		__nds32__dsb();
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.FILL_DATA_SEL = 1; //在FLASH坏列地址上填入随机数
		__nds32__dsb();
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.BADCOL_FILL_DATA = 0x22; //在坏列地址上填入0x22（当前模式没有用到）
		__nds32__dsb();
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.REDUANT_FILL_DATA = 0x55; //在FLASH未写满区域填入0x55（当前模式没有用到）
		SG_FC(bChan)->rRND_SEED.bits.dVAL = 0x55; //设置随机种子
		SG_FC(bChan)->rBADCOL_INFO_BADDR.bits.dVAL = wDma2FiltBAddr >> 16; //DMA跳列区间信息地址的高16位

		SG_FC(bChan)->rDMA2_FILTER_ECC_CFG_BADDR = wDma2FiltBAddr + 0 * bJCTableBuffLen; //DMA配置信息地址
		dDMA2JCBaseAddr = dJCTableBaseAddr + 0 * bJCTableBuffLen;	//每个通道存放4Byte 跳列的地址
		dDMA2JCTableAddr = dDMA2JCBaseAddr + bJCTableOffset;				//每个通道存放2Byte 信息的地址

		for(i = 0; i < bDMACnt; i++)		//每个DMA2 需要存放的4Byte（2Byte） 个数
		{
			if(i == bDMACnt - 1)
			{
				wDMA2JCCnt = wJCMaxByte; 

				if(wDMA2JCCnt > 0x3FF)
				{
					wDMA2JCCnt = 0x3FF;
				}
			}
			else
			{
				wDMA2JCCnt = 0x0000;
			}

			dVaule0_14 = ((dDMA2JCTableAddr & 0x0000FFFF) >>1); //0-14有值，其他都为0
			dValue15_24 = ((wDMA2JCCnt & 0x3FF) << 15);				//15-24有值，其他都为0
			*((U32 *)dDMA2JCBaseAddr) = dVaule0_14 | dValue15_24;	//4Byte 数据
			*((U16 *)dDMA2JCTableAddr) = 0xFFFF;	//2Byte 数据(若有跳列，应该用For循环赋值 DMA2_JC_Cnt次)
			dDMA2JCBaseAddr += 4;								//下一个DMA的4Byte数据的地址
			dDMA2JCTableAddr += 2;			//下一个DMA的2Byte数据的地址
		}
	}
	
	return TRUE;
}


void HAL_NFChkStCtrl(U8 bChanCnt,U8 bMode)
{
    U8 bChan;

    for(bChan = 0;bChan<bChanCnt;bChan++)
    {
        SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL = bMode;//1:status time-out/fail stop cq
    }
}

void HAL_SetOverFlowThresh(U8 bChan,U8 bSetVal_0,U16 wSetVal_1)
{
    SG_DMA2_CH(bChan)->rECC_CFG.bits.OVFLOW_THRESHOLD = bSetVal_0&0x3F;


	__nds32__dsb();
    SG_DMA2_CH(bChan)->rECC_CFG.bits.SVA_OVF_TH = wSetVal_1&0x1FFF;

}

U16 HAL_NFGetOverFlowThresh(U8 bChan,U8 bValNum)
{
    if(bValNum == 0)
    {
        return SG_DMA2_CH(bChan)->rECC_CFG.bits.OVFLOW_THRESHOLD;//BCH模式下为ECC阈值,LDPC模式下为迭代次数
    }
    else
    {

		return SG_DMA2_CH(bChan)->rECC_CFG.bits.SVA_OVF_TH;

    }
}
U8 HAL_NFGetOrClrChkStsHwFlag(U8 bChan,U8 bMode)
{
    if(bMode == GET_CHKSTS_FLAG)
    {
        return ((SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_FAIL_FLAG)|
        		(SG_FC(bChan)->rCHK_INT_ST.bits.CHK_ST_FAIL_INT)|
                (SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_FLAG)|
                (SG_FC(bChan)->rCHK_INT_ST.bits.CHK_ST_MAX_INT)|
				(SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MINTIME_FLAG)|
				(SG_FC(bChan)->rCHK_INT_ST.bits.CHK_ST_MIN_INT));
    }
    else if(bMode == CLR_CHKSTS_FLAG)
    {
       SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_FAIL_FLAG = 0;
       SG_FC(bChan)->rCHK_INT_ST.bits.CHK_ST_FAIL_INT = 0;
       SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_FLAG = 0;
       SG_FC(bChan)->rCHK_INT_ST.bits.CHK_ST_MAX_INT = 0;
        SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MINTIME_FLAG = 0;
	   SG_FC(bChan)->rCHK_INT_ST.bits.CHK_ST_MIN_INT = 0;
    }
    
    return 0;
}



void HAL_BICS3SetParameter(U08 bChan, U08 bChip, U08 bAddr, U08 data,U08 Enter)
{
	if(!Enter)
	{
		HAL_MCU_SetCe(bChan,bChip,1);
		HAL_MCU_SendCmd(bChan,0x5C);
		HAL_MCU_SendCmd(bChan,0xC5);
#if 0/*选择CE里面的某个Lun*/
		HAL_MCU_SendCmd(bChan,0xF1);
		delay_us(2);
		HAL_MCU_SendCmd(bChan,0x56);
		delay_us(2);
#else/*选择CE里面的所有Lun*/
		HAL_MCU_SendCmd(bChan,0x55);
		
#endif
		HAL_MCU_SendOneAddr(bChan,0X00);
		HAL_McuSendWRPulse(bChan,0X01);
	}

#if 0/*选择CE里面的某个Lun*/
	HAL_MCU_SendCmd(bChan,0xF1);
	delay_us(2);
	HAL_MCU_SendCmd(bChan,0x56);
	delay_us(2);
#else/*选择CE里面的所有Lun*/
	HAL_MCU_SendCmd(bChan,0x55);
	
#endif


	HAL_MCU_SendOneAddr(bChan,bAddr);	
	HAL_McuSendWRPulse(bChan,data);
	
	HAL_MCU_SendCmd(bChan,0x5D);
}

void HAL_BICS3GetParameter(U08 bChan, U08 bChip, U08 bAddr, U08 *data,U08 Enter)
{
	if(!Enter)
	{
		HAL_MCU_SetCe(bChan,bChip,1);
		HAL_MCU_SendCmd(bChan,0x5C);
		HAL_MCU_SendCmd(bChan,0xC5);
#if 0/*选择CE里面的某个Lun*/
		HAL_MCU_SendCmd(bChan,0xF1);
		delay_us(2);
		HAL_MCU_SendCmd(bChan,0x56);
		delay_us(2);
#else/*选择CE里面的所有Lun*/
		HAL_MCU_SendCmd(bChan,0x55);

#endif
		HAL_MCU_SendOneAddr(bChan,0X00);
		HAL_McuSendWRPulse(bChan,0X01);
	}

#if 0/*选择CE里面的某个Lun*/
	HAL_MCU_SendCmd(bChan,0xF1);
	delay_us(2);
	HAL_MCU_SendCmd(bChan,0x56);
	delay_us(2);
#else/*选择CE里面的所有Lun*/
	HAL_MCU_SendCmd(bChan,0x55);
	
#endif

	HAL_MCU_SendOneAddr(bChan,0X01);
	HAL_McuSendWRPulse(bChan,0X06);
	
	HAL_MCU_SendCmd(bChan,0x5F);
	HAL_MCU_SendCmd(bChan,0x00);


	HAL_MCU_SendOneAddr(bChan,bAddr);
	HAL_MCU_SendCmd(bChan,0x5F);
	*data = HAL_McuSendReadPluse(bChan);
	HAL_MCU_SendCmd(bChan,0x5F);
	HAL_MCU_SendCmd(bChan,0x00);
	HAL_MCU_SendCmd(bChan,0x5F);
	HAL_MCU_SendCmd(bChan,0x5F);
}

void HAL_L06B2B0KB(U08 bChan,U08 bChip)
{
	U08 bData[4];
	HAL_NFResetFlash(bChan,bChip,FL_CMD_HARD_RESET_FLASH);
	Delay(0x40);
	HAL_NFResetFlash(bChan,bChip,FL_CMD_HARD_RESET_FLASH);

	bData[0] = 0x03;
	bData[1] = 0x00;
	bData[2] = 0x00;
	bData[3] = 0x00;
	
	HAL_NFSetFeature(bChan,bChip,0x85,&bData[0],0);
	HAL_NFResetFlash(bChan,bChip,FL_CMD_RESET_FLASH);
}

void HAL_SetRegisterOffset(U08 bChan,U08 bChip,U08 bStep)
{
	U08 i;
	U08 bRegData[4] = {0,0,0,0};

	for(i = 0;i< 8;i++)
	{
		bRegData[0]	= bStep;
		HAL_McuSetFeature(bChan,bChip,0xA4+i,1,4, bRegData);
	}
}
#endif



