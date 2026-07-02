#include "hal.h"
#include "hal_define.h"
#include "hal_adm.h"

inline void HAL_ADMSetBaseAddr(U8 bChan, U32 dBuffAddr)
{
    if (INVALID_U8 == bChan)
    {
        SG_DMA1->rBSADDR.dAll = dBuffAddr;
    }
    else
    {
        SG_DMA2_CH(bChan)->rBSADDR = dBuffAddr;
    }
}

inline U32 HAL_ADMGetBaseAddr(U8 bChan)
{
    if (INVALID_U8 == bChan)
    {
       return SG_DMA1->rBSADDR.dAll;
    }
    else
    {
       return SG_DMA2_CH(bChan)->rBSADDR;
    }
}

inline void HAL_ADMSetBufferSize(U16 wSect)
{
    SG_ADM->rBUF_SIZE = wSect;
}
inline void HAL_ADMSetDMALen(U8 bChan, U16 wLen)
{
    SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = wLen;
}
inline U16 HAL_ADMGetDMALen(U8 bChan)
{
    return SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN;
}
inline void HAL_ADMSetUdBaseAddr(U8 bChan, U32 dBuffAddr)
{
    SG_DMA2_CH(bChan)->rUDATA_BSADDR.bits.dVAL = dBuffAddr;
}

inline U32 HAL_ADMGetUdBaseAddr(U8 bChan)
{
    return SG_DMA2_CH(bChan)->rUDATA_BSADDR.bits.dVAL;
}

inline void HAL_ADMSetUdLen(U8 bChan, U8 bLen)
{
    SG_DMA2_CH(bChan)->rLEN_CFG.bits.UDATA_LEN = bLen;
}

inline void HAL_ADMSetUdEnable(U8 bChan, U8 bRndNewState)
{
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = bRndNewState;
}

inline void HAL_ADMSetRaidEnable(U8 bChan, U8 bRndNewState)
{
	SG_DMA2_CH(bChan)->rRAID_CTRL.bits.RAID_EN = bRndNewState;
}

inline U8 HAL_ADMGetUdLen(U8 bChan)
{
    return SG_DMA2_CH(bChan)->rLEN_CFG.bits.UDATA_LEN;
}

inline U8 HAL_ADMGetJointMode(U8 bChan)
{
    return SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE;
}

inline void HAL_ADMSetJointMode(U8 bChan,U8 bJointMode)
{
     SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE = bJointMode;
}

inline U32 HAL_ADMGetDMA1Index(void)
{
	return (SG_DMA1->rINDEX);
}

U32 HAL_ADMGetDMA2Index(U08 bChanPhy)
{
	
	U32 BuffIndexLDPCSum = 	SG_DMA2_CH(bChanPhy)->rINDEX_CFG +\
							SG_DMA2_CH(bChanPhy)->rTOTAL_SECT_CFG +\
							SG_DMA2_CH(bChanPhy)->rTOTAL_SECT_BUF;
	
	if(BuffIndexLDPCSum >= SG_DMA2_CH(bChanPhy)->rBUF_INDEX_LDPC)
	{
		return (SG_DMA2_CH(bChanPhy)->rBUF_INDEX_LDPC);
	}
	else
	{
		return (0);
	}
}

inline void HAL_ADMClearDMA1Index(void)
{
	SG_DMA1->rINDEX = 0;
	__nds32__dsb();
}

inline void HAL_ADMClearDMA2Index(U08 bChanPhy)
{
	SG_DMA2_CH(bChanPhy)->rINDEX_CFG = 0;
	SG_DMA2_CH(bChanPhy)->rTOTAL_SECT_CFG = 0;
	__nds32__dsb();
}


void HAL_ADMStartDMA1(U32 dBuffAddr, U32 dBuffIdx, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint)
{
    SG_DMA1->rBSADDR.bits.dVAL = dBuffAddr;
	SG_DMA1->rENABE.bits.EN = 1;
    SG_DMA1->rTOTAL_SECT = wTotalSect;
    SG_DMA1->rINDEX = dBuffIdx;
    
	SG_DMA1->rCTRL.bits.JOINT_EN = (BOOL)joint;

	__nds32__dsb();
    SG_DMA1->rCTRL.bits.DIR = dir;
	__nds32__dsb();
    SG_DMA1->rCTRL.bits.START = 1;
	__nds32__dsb();
}

void HAL_ADMStartDMA1WithoutIndex(U32 dBuffAddr, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint)
{
	SG_DMA1->rBSADDR.bits.dVAL = dBuffAddr;
	SG_DMA1->rTOTAL_SECT = wTotalSect;


 	SG_DMA1->rCTRL.bits.JOINT_EN = (BOOL)joint;

	 __nds32__dsb();
	SG_DMA1->rCTRL.bits.DIR = dir;
	 __nds32__dsb();
	SG_DMA1->rCTRL.bits.START = 1;
	 __nds32__dsb();
}

inline void HAL_MCSetDMA1BuffIndex(U32 dBuffIdx)
{
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 1);

    U32 dOfset = offsetof(SG_DMA1_TypeDef,rINDEX) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (dBuffIdx & INVALID_U16);
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 2) << R_ADM_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);
}

void HAL_MCStartDMA1(U32 dBuffAddr, U32 dBuffIdx, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint)
{
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 6);

    U32 dOfset = offsetof(SG_DMA1_TypeDef,rBSADDR) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset <<R_ADM_REG_BIT) | (dBuffAddr & INVALID_U16);
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 2)<<R_ADM_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);

    dOfset = offsetof(SG_DMA1_TypeDef,rINDEX) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (dBuffIdx & INVALID_U16);
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 2) << R_ADM_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);

    dOfset = offsetof(SG_DMA1_TypeDef,rTOTAL_SECT) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (wTotalSect & INVALID_U16);

    dOfset = offsetof(SG_DMA1_TypeDef,rCTRL) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_1B |(dOfset << R_ADM_REG_BIT) | (joint << 3) | (dir << 1) | 0x01;

    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CHECK_DMA1_DONE;
}

void HAL_MCStartDMA1Continue(U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint)
{
	//MC最多255个，需预防填爆....
	if(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 3)
	{
		PRINT("ERR MC LEN<3\r\n");
	}
	
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 3);
	U32 dOfset;
    dOfset = offsetof(SG_DMA1_TypeDef,rTOTAL_SECT) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (wTotalSect & INVALID_U16);

    dOfset = offsetof(SG_DMA1_TypeDef,rCTRL) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_1B |(dOfset << R_ADM_REG_BIT) | (joint << 3) | (dir << 1) | 0x01;

    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CHECK_DMA1_DONE;
}


void HAL_MCADMSetBaseAddr(U32 dBuffAddr)
{
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 2);
	U32 dOfset = offsetof(SG_DMA1_TypeDef,rBSADDR) & INVALID_U8;
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset <<R_ADM_REG_BIT) | (dBuffAddr & INVALID_U16);
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 2)<<R_ADM_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);   
}

void HAL_MCStartDMA1WithoutBuffAddr(U32 dBuffIdx, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint)
{
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 5);
	
	U32 dOfset = offsetof(SG_DMA1_TypeDef,rINDEX) & INVALID_U8;
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (dBuffIdx & INVALID_U16);
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 2) << R_ADM_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);//配一个0默认BuffIndex不会超过16位

	dOfset = offsetof(SG_DMA1_TypeDef,rTOTAL_SECT) & INVALID_U8;
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (wTotalSect & INVALID_U16);
	
	dOfset = offsetof(SG_DMA1_TypeDef,rCTRL) & INVALID_U8;
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_1B |(dOfset << R_ADM_REG_BIT) | (joint << 3) | (dir << 1) | 0x01;
	
	SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CHECK_DMA1_DONE;

}

void HAL_MCStartDMA1WithoutBuffIndex(U32 dBuffAddr, U16 wTotalSect, DMA1_Dir dir, Joint_Mode joint)
{
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 5);

    U32 dOfset = offsetof(SG_DMA1_TypeDef,rBSADDR) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 0)<<R_ADM_REG_BIT) | ((dBuffAddr >> 0) & INVALID_U16);
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |((dOfset + 2)<<R_ADM_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);

    dOfset = offsetof(SG_DMA1_TypeDef,rTOTAL_SECT) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_2B |(dOfset << R_ADM_REG_BIT) | (wTotalSect & INVALID_U16);

    dOfset = offsetof(SG_DMA1_TypeDef,rCTRL) & INVALID_U8;
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CFG_REG_1B |(dOfset << R_ADM_REG_BIT) | (joint << 3) | (dir << 1) | 0x01;

    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_CHECK_DMA1_DONE;
}

inline void HAL_MCGenerateInt()
{
	while(SG_DMA1->rMC_STS.bits.MC_IDLE_CNT < 1);
    SG_DMA1->rMC_CMD.dAll = R_ADM_ATTR_INT;
}

inline void HAL_MCWaitDMA1Done()
{
	while(SG_DMA1->rMC_STS.bits.MC_DONE == 0);
}

inline U08 HAL_MCGetStatus()
{
	return  SG_DMA1->rMC_STS.bits.MC_DONE; // 1:done 0:not done
}
void HAL_ADMStartDMA2(U8 bChan,U32 dBuffAddr,U32 dDmaInfoAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint)
{
	if(joint == JOINT_NONE)
	{
		SG_DMA2_CH(bChan)->rBSADDR_HANDLE = dBuffAddr;
	}
	else
	{
		SG_DMA2_CH(bChan)->rBSADDR = dBuffAddr;
	}
    SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG = bTotalSect;
    SG_DMA2_CH(bChan)->rINDEX_CFG = dBuffIdx;
    SG_DMA2_CH(bChan)->rFLASH_INDEX.bits.FLASH_TRANS_INDEX = bFlashIdx;
	 __nds32__dsb();
    SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE = joint;
	 __nds32__dsb();
    if (DMA2_WR_TO_FLASH == dir)		//Write
    {
        SG_DMA2_CH(bChan)->rCTRL.bits.DIR = 0;
		 __nds32__dsb();
		SG_DMA2_CH(bChan)->rSTART.bits.START_DMA2_BUF = 1;
		 __nds32__dsb();
		SG_DMA2_CH(bChan)->rSTART.bits.START_DMA2_FC = 1;
    }
    else								//read
    {
        SG_DMA2_CH(bChan)->rDMAINFO_BSADDR.bits.dVAL  = dDmaInfoAddr;
        SG_DMA2_CH(bChan)->rCTRL.bits.DIR = 1;
		 __nds32__dsb();
		//wait Fc_Done
		while(!SG_DMA2_CH(bChan)->rSTATUS.bits.FC_DONE);
		SG_DMA2_CH(bChan)->rSTART.bits.START_DMA2_FC = 1;
		 __nds32__dsb();
		//wait Dma_Done
		while(!SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE);
		SG_DMA2_CH(bChan)->rSTART.bits.START_DMA2_BUF = 1;
		 __nds32__dsb();
    }
}

void HAL_ADMWaitDMADone(U8 bChan)
{
    U32 i = 0;
    
    if (0xFF == bChan)
    {
        while(!SG_DMA1->rSTATUS.bits.TRANS_DONE)
        {
            i++;
            if (i>0xFFFFFF)
            {
                i = 0;
                //LOG_PRINT("DMA1 index = %x, total = %x\r\n",SG_DMA1->rINDEX,SG_DMA1->rTOTAL_SECT);
            }
        }
    }
    else
    {

        if(SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_EN == 1)
        {
            while(!((1 == SG_DMA2_CH(bChan)->rSTATUS.bits.FC_DONE)&&(1 == SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE)))
            {
                i++;
                if (i>0xFFFFFF)
                {
                    i = 0;
                    PRINT("ERR_SoftLdpc WaitDma2 Time-Out Sts=%x\r\n",SG_DMA2_CH(bChan)->rSTATUS.dAll);
                }
            }
        }
        else
        {
            while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
            {
                i++;
                if (i>0xFFFFFF)
                {
                    i = 0;
                    PRINT("ERR_WaitDma2 Time-Out\r\n");
                }
            }
        }

    }
}

volatile U8 bCheckDMA2RealDone(U8 bChan)
{
	if(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
	{
		return 0;
	}
	return 1;
}

volatile U8 bCheckDMA2FCDone(U8 bChan)
{
	if(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.FC_DONE)
	{
		return 0;
	}
	return 1;
}

volatile U8 bCheckDMA2BufDone(U8 bChan)
{
	if(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE)
	{
		return 0;
	}
	return 1;
}

inline void HAL_ADMSetRndEnable(U8 bChan, U8 bFunctionNewState, U8 bRndNewState)
{
    if (bRndNewState == FUNC_ENABLE)
    {
        SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 0;
    }
    else
    {
        SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
    }
	 __nds32__dsb();
    SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_EN = bFunctionNewState;
}

inline void HAL_ADMSetMask(U8 bChan,U32 dMask0, U32 dMask1)
{
    SG_DMA2_CH(bChan)->rMASK = dMask0;
}

inline void HAL_ADMSetDma2EnabledState(U8 bChan,U16 wNewState)
{
    SG_DMA2_CH(bChan)->rENABLE.wAll = wNewState;
}

inline U16 HAL_ADMGetDma2EnabledState(U8 bChan)
{
    return SG_DMA2_CH(bChan)->rENABLE.wAll;
}


inline void HAL_ADMSetDma2StopEnableState(U8 bChan, U8 bNewState)
{
    SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = bNewState;
}

inline U8 HAL_ADMGetDma2StopEnableState(U8 bChan)
{
    return SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll;
}

void HAL_HostDataTransfer(U32 dBuffAddr,U16 wSec, U08 bDir)
{
	if(bDir == 0)//host out 
	{
		HAL_ADMStartDMA1(dBuffAddr,0,wSec,DMA1_WR_TO_BUFF,JOINT_NONE);
	}
	else
	{
		HAL_ADMStartDMA1(dBuffAddr,0,wSec,DMA1_RD_TO_HOST,JOINT_NONE);
	}
	
    HAL_ADMWaitDMADone(0xFF);
}


void HAL_ADMComSelEccMode(U8 bChan,U8 bMode)
{
	if(ECC_MODE_NONE == bMode)
	{
		SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	}
	else
	{
		SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 1;
	}
}

