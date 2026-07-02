
#include "hal_config.h"
#include "fdl_read_retry.h"
#include "fdl_mcu_op.h"
#if (FUNC_ENABLE == __HAL_MCU_DEFINE__)
#include "hal.h"
#include "hal_define.h"
#include "uitility.h"
inline void HAL_MCUSetGieState(U8 bNewState)
{
    if (FUNC_ENABLE == bNewState)
    {
        __nds32__setgie_en();
    }
    else
    {
        __nds32__setgie_dis();
    }
    __nds32__dsb();
}
inline void HAL_MCU_SetCe(U8 bChan, U8 bChip, U8 bVal)
{
	SG_FC(bChan)->rMCU_CFG_CE.bits.dVAL = bVal<<bChip;
	__nds32__dsb();
}
inline void HAL_MCU_SetRd(U8 bChan)
{
	SG_FC(bChan)->rMCU_SET_RD.bits.dVAL = 1;
	__nds32__dsb();
	while(0 != SG_FC(bChan)->rMCU_SET_RD.bits.dVAL);
}

inline void HAL_MCU_SendCmd(U8 bChan, U8 bCmd)
{
	SG_FC(bChan)->rMCU_CFG_CLE.dAll = MCU_SEND_CMD_HEAD | bCmd;
	__nds32__dsb();
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}   
}
inline void HAL_MCU_SendOneAddr(U8 bChan, U08 bAddr)
{
	SG_FC(bChan)->rMCU_CFG_ALE.dAll = ((U32)1<<24) | bAddr;        
	__nds32__dsb();
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}  
}
inline void HAL_MCU_SendTwoAddr(U8 bChan, U16 wAddr)
{
#if 0
	SG_FC(bChan)->rMCU_CFG_ALE.dAll = ((U32)2<<24) | wAddr;        
	__nds32__dsb();
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}  
#else
	HAL_MCU_SendOneAddr(bChan, wAddr);
	HAL_MCU_SendOneAddr(bChan, wAddr>>8);
#endif
}
inline void HAL_MCU_SendThreeAddr(U8 bChan, U32 dAddr)
{
#if 0
	SG_FC(bChan)->rMCU_CFG_ALE.dAll = ((U32)3<<24) | dAddr;        
	__nds32__dsb();
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
#else
	HAL_MCU_SendOneAddr(bChan, dAddr);
	HAL_MCU_SendOneAddr(bChan, dAddr>>8);
	HAL_MCU_SendOneAddr(bChan, dAddr>>16);
#endif
#if(NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
	HAL_MCU_SendOneAddr(bChan,0x00);
#endif
}

inline void HAL_MCU_SendFourAddr(U8 bChan, U32 dAddr)
{
#if 0
	SG_FC(bChan)->rMCU_CFG_ALE.dAll = ((U32)3<<24) | dAddr;        
	__nds32__dsb();
	while(1 != SG_FC(bChan)->rMCU_CFG_DONE.bits.dVAL){;}
#else
	HAL_MCU_SendOneAddr(bChan, dAddr);
	HAL_MCU_SendOneAddr(bChan, dAddr>>8);
	HAL_MCU_SendOneAddr(bChan, dAddr>>16);
    HAL_MCU_SendOneAddr(bChan, dAddr>>32);
#endif
}


volatile U8 HAL_MCUCheckStatus(U8 bChan, U8 bChip)
{
	U16 bCheckCnt=0;
	HAL_MCU_SetCe(bChan,bChip,1);
	Delay(8);
	HAL_MCU_SendCmd(bChan,FL_CMD_READ_STATUS);
	Delay(40);
	U16 wCheckTimeOut = 10000;//实际时间>50ms，ARC Read也应该可以满足 同CQ的Check时间基本保持一致
	
	//发送读脉冲
	while(bCheckCnt<4)
	{
		SG_FC(bChan)->rSEND_RD_PULSE.bits.dVAL = 1;
		__nds32__dsb();
		while(SG_FC(bChan)->rRW_PULSE_DONE.bits.dVAL!=1);
		if ((SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA&0x60)==0x60)
		{
			bCheckCnt++;
		}
		delay_us(1);
		wCheckTimeOut--;
		if(wCheckTimeOut == 0)
		{
			break;
		}
	}
	
	//HAL_MCU_SetCe(bChan,bChip,1);
	Delay(8);
	HAL_MCU_SendCmd(bChan, FL_CMD_EXIT_CHK_IOSTA);
	if(bCheckCnt<4)
	{
        //pFdlParam.FTL_CollectRetryInfor(0,0,0xFF,0);
		
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void HAL_MCU_GetOwnership(U08 bMutexReg, U08 bCoreIdx)
{
  volatile U08 *pReg =  (U8 *)(0x5e60b0+4*bMutexReg);
  U08 bOwnerValue[2] = {1,2};
  while(1)
  {
	if(*pReg == 0)
	{
		__nds32__dsb();
	  *pReg = bOwnerValue[bCoreIdx];
	  __nds32__dsb();
	  if(*pReg == bOwnerValue[bCoreIdx])
	  {
		return ;
	  }
	}
	else if(*pReg == bOwnerValue[bCoreIdx])
	{
	  	return ;
	}
  }
}
void HAL_MCU_ReleaseOwnership(U08 bMutexReg, U08 bCoreIdx)
{
	volatile U08 *pReg =  (U8 *)(0x5e60b0+4*bMutexReg);
	  __nds32__dsb();
	*pReg = 0;
}
#endif






