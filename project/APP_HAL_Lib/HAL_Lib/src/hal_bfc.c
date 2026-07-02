
#include "hal_config.h"
#include "fdl_read_retry.h"
#if (FUNC_ENABLE == __HAL_BFC_DEFINE__)
#include "hal.h"
#include "hal_define.h"

void HAL_BFCInit(void)
{
    SG_ADM->rRESET_BUF_STS.bits.dVAL = 1;
    __nds32__dsb();
    while(SG_ADM->rRESET_BUF_STS.bits.dVAL)
    {
        ;
    }
}

U16 HAL_BFCGetBuffStatus(U16 wIndex)
{
	SG_ADM->rBIT_INDEX.dAll = wIndex;
	SG_ADM->rREAD_BUF_STS.dAll = 1;
    __nds32__dsb();
    while(SG_ADM->rREAD_BUF_STS.dAll)
    {
        ;
    }
    return (U16)SG_ADM->rMCU_RDATA_STS.dAll;
}

void HAL_BFCSetBuffStatusBit(U16 wIndex)
{
	SG_ADM->rBIT_INDEX.dAll = wIndex;
	SG_ADM->rSET_BIT_STS.dAll = 0x1;
    __nds32__dsb();
    while(SG_ADM->rSET_BIT_STS.dAll)
    {
        ;
    }
}

void HAL_BFCSetBuffStatus(U16 wIndex,U16 wNewBitPosition)
{
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_GetOwnership(RTY_BUFFSTS_MUTIREG,G3CORE);
#else
	bG3SetBuffSts = 1;
	__nds32__dsb();
	while(bG0SetBuffSts == 1);
#endif
#endif
	while(HAL_BFCGetBuffStatus(wIndex%SG_ADM->rBUF_SIZE)&wNewBitPosition != 0)
	{
	}

	SG_ADM->rBIT_INDEX.bits.dVAL = wIndex;
	SG_ADM->rSET_BIT_STS.bits.dVAL = wNewBitPosition;
    __nds32__dsb();
    while(SG_ADM->rSET_BIT_STS.bits.dVAL)
    {
        ;
    }
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_ReleaseOwnership(RTY_BUFFSTS_MUTIREG,G3CORE);
#else
	bG3SetBuffSts = 0;
#endif
#endif
}

void HAL_RTY_BFCSetBuffStatus(U16 wIndex,U16 wNewBitPosition)
{
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_GetOwnership(RTY_BUFFSTS_MUTIREG,G0CORE);
#else
	bG0SetBuffSts = 1;
	__nds32__dsb();
	while(bG3SetBuffSts == 1)
	{
		bG0SetBuffSts = 0;
	}
	bG0SetBuffSts = 1;
#endif
#endif

	while(HAL_BFCGetBuffStatus(wIndex%SG_ADM->rBUF_SIZE)&wNewBitPosition != 0)
	{
	}

	SG_ADM->rBIT_INDEX.bits.dVAL = wIndex;
	SG_ADM->rSET_BIT_STS.bits.dVAL = wNewBitPosition;
    __nds32__dsb();
    while(SG_ADM->rSET_BIT_STS.bits.dVAL)
    {
        ;
    }
	
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_ReleaseOwnership(RTY_BUFFSTS_MUTIREG,G0CORE);
#else
	bG0SetBuffSts = 0;
#endif
#endif

}

void HAL_RTY_BFCSetBuffStatusBit(U16 wIndex)
{
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_GetOwnership(RTY_BUFFSTS_MUTIREG,G0CORE);
#else
	bG0SetBuffSts = 1;
	__nds32__dsb();
	while(bG3SetBuffSts == 1)
	{
		bG0SetBuffSts = 0;
	}
	bG0SetBuffSts = 1;
#endif
#endif

	while(HAL_BFCGetBuffStatus(wIndex%SG_ADM->rBUF_SIZE)&((U16)(1<<wIndex)) != 0)
	{
	}

	SG_ADM->rBIT_INDEX.dAll = wIndex;
	SG_ADM->rSET_BIT_STS.dAll = 0x1;
    __nds32__dsb();
    while(SG_ADM->rSET_BIT_STS.dAll)
    {
        ;
    }
	
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_ReleaseOwnership(RTY_BUFFSTS_MUTIREG,G0CORE);
#else
		bG0SetBuffSts = 0;
#endif
#endif
}

void HAL_BFCClearBuffStatus(U16 wIndex,U16 wNewBitPosition)
{
	SG_ADM->rBIT_INDEX.dAll = wIndex;
	SG_ADM->rCLR_BIT_STS.bits.dVAL = wNewBitPosition;
    __nds32__dsb();
    while(SG_ADM->rCLR_BIT_STS.bits.dVAL)
    {
        ;
    }
}
void HAL_WaitBuffStatusClr(void)
{
	U16 wIndex;

	for(wIndex=0; wIndex < SG_ADM->rBUF_SIZE; wIndex += 16)
	{
		do
		{
			SG_ADM->rBIT_INDEX.bits.dVAL = wIndex;
			SG_ADM->rREAD_BUF_STS.bits.dVAL = 1;
		    __nds32__dsb();
		    while(SG_ADM->rREAD_BUF_STS.bits.dVAL);		//0:read reg done
		}while(SG_ADM->rMCU_RDATA_STS.bits.dVAL);			//0:buf-clr
	}
}


#endif






