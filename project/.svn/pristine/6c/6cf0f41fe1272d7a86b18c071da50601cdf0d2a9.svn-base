#include "hal_ldpc.h"
#include "flash_driver_config.h"
#include "fdl_soft_decode.h"

U16_C gMatrixSize[24] =
{
	152, 200, 280, 632,
	0,   0,   0,   0,
	136, 392, 288, 584,
	136, 512, 292, 592,
	136, 648, 296, 604,
	136, 800, 300, 612
};

U08_C gCodeRate[6] = {0x45,0x00,0x87,0xA8,0xC9,0xEA};

U32 HAL_LDPC_LUTCal(U8 bValIdx, U8 bEndBit)
{
	U32 dLUTVal = 0;
	U08 bMapNum,bShift;
	U08 *pLUT_Table = (U8 *)HAL_b3BitLUT;

	for(bShift = 0,bMapNum = bValIdx;bMapNum<bValIdx+4;bMapNum++,bShift++)
	{
		dLUTVal |= (U32)(pLUT_Table[bMapNum]<<(bShift*8));
	}
	return dLUTVal;
}


void Hal_LDPC_LENCSetMatrix(U32 *pMatrixDataA, U16 bMatrixASize, U32 *pMatrixDataB, U16 bMatrixBSize)
{
	U32 wMatriAaddr = 0x1000;
	U32 wMatriBaddr = 0x2000;

	U16 i;

	for(i = 0;i<bMatrixASize;i++)
	{
		SG_LDPC->rMCU_WLENC_MEM_D0 = pMatrixDataA[i];
		SG_LDPC->rMCU_WLENC_MEM_ADDR.dAll = wMatriAaddr;
		wMatriAaddr += 4;
		while(SG_LDPC->rMCU_WLENC_MEM_OP_STS.bits.LENC_MEM_WACK!=1);
		//while(SG_LDPC->rMCU_WLENC_MEM_OP_STS.dAll == 0);
	}

	for(i = 0;i<bMatrixBSize;i++)
	{
		SG_LDPC->rMCU_WLENC_MEM_D0 = pMatrixDataB[i];
		SG_LDPC->rMCU_WLENC_MEM_ADDR.dAll = wMatriBaddr;
		wMatriBaddr += 4;
		//while(SG_LDPC->rMCU_WLENC_MEM_OP_STS.dAll == 0);
		while(SG_LDPC->rMCU_WLENC_MEM_OP_STS.bits.LENC_MEM_WACK!=1);
	}
}

//#ifdef __FW__
//void  __section(".CODE_INIT") HAL_LDPC_LDECSetMatrix(U32 *P1, U16 wMatrixP1Size,  U32 *P2, U16 wMatrixP2Size)
//#else
void Hal_LDPC_LDECSetMatrix(U32 *P1, U16 wMatrixP1Size,  U32 *P2, U16 wMatrixP2Size)
//#endif
{
	U16 i;
	U32 wMatriAaddr = 0x30000;
	U32 wMatriBaddr = 0x35000;

	for(i = 0;i<wMatrixP1Size;i++)
	{
		SG_LDPC->rMCU_WLDEC_MEM_DIN = P1[i];
		SG_LDPC->rMCU_WLDEC_MEM_ADDR.dAll = wMatriAaddr;
		wMatriAaddr += 4;
		while(SG_LDPC->rMCU_WLDEC_MEM_OP_STS.bits.LDEC_MEM_WACK!=1);
		//while(SG_LDPC->rMCU_WLDEC_MEM_OP_STS.dAll==0);
	}

	for(i = 0;i<wMatrixP2Size;i++)
	{
		SG_LDPC->rMCU_WLDEC_MEM_DIN = P2[i];
		SG_LDPC->rMCU_WLDEC_MEM_ADDR.dAll = wMatriBaddr;
		wMatriBaddr += 4;
		while(SG_LDPC->rMCU_WLDEC_MEM_OP_STS.bits.LDEC_MEM_WACK!=1);
		//while(SG_LDPC->rMCU_WLDEC_MEM_OP_STS.dAll==0);
	}
}

//************************************
// Method:    HAL_LDPC_Init
// FullName:  HAL_LDPC_Init
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: U8 bMode
//目前固定配置为矩阵8
//************************************
//#ifdef __FW__
//void  __section(".CODE_INIT")  HAL_LDPC_Init(U8 bMode, U32 dMemMtxA, U32 dMemMtxB, U32 dMemMtxP1, U32 dMemMtxP2)
//#else
void HAL_LDPC_Init(U8 bMode, U32 dMemMtxA, U32 dMemMtxB, U32 dMemMtxP1, U32 dMemMtxP2)
//#endif
{
	//0-ADM模块ECC配置
	U08 i;
	for(i = 0;i<4;i++)
	{
		SG_DMA2_CH(i)->rECC_CTRL.bits.LDPC_MOD = bMode;
	}

	//1-先对LDPC模块进行软复位
	SG_CPM->rSOFT_RST0.bits.LENC = 0;
	while(SG_CPM->rSOFT_RST0.bits.LENC == 0);
	SG_CPM->rSOFT_RST0.bits.LDEC = 0;
	while(SG_CPM->rSOFT_RST0.bits.LDEC == 0);

	//2-LENC初始化
	SG_LDPC->rMCU_WLENC_REG_OP_STS; //clear flag
	SG_LDPC->rMCU_WLENC_MEM_OP_STS; //clear flag
	SG_LDPC->rLENC_MEM_OP_MODE.dAll = 0x1; //enable mcu write mem
	SG_LDPC->rLENC_CTRL = 0x12;
	while(SG_LDPC->rMCU_WLENC_REG_OP_STS.bits.LENC_REG_WACK!=1);
	SG_LDPC->rLENC_FORMAT0.dAll = (gCodeRate[bMode-5]|(0x80000000));
	while(SG_LDPC->rMCU_WLENC_REG_OP_STS.bits.LENC_REG_WACK!=1);

	U08 bMatrixIdx = (bMode - 5)*4;
	Hal_LDPC_LENCSetMatrix((U32*)(dMemMtxA), gMatrixSize[bMatrixIdx],  (U32*)(dMemMtxB), gMatrixSize[bMatrixIdx+1]);
	//Hal_LDPC_LENCSetMatrix((U32*)(dMemMtxA), 136,  (U32*)(dMemMtxB), 512);
	SG_LDPC->rLENC_CTRL = 0x100003;
	while(SG_LDPC->rMCU_WLENC_REG_OP_STS.bits.LENC_REG_WACK!=1);
	SG_LDPC->rLENC_MEM_OP_MODE.dAll = 0x0; //disable mcu write mem

	//3-对LDEC初始化
	SG_LDPC->rMCU_WLDEC_REG_OP_STS; //clear flag
	SG_LDPC->rMCU_WLDEC_MEM_OP_STS; //clear flag
	SG_LDPC->rLDEC_MEM_OP_MODE.dAll = 0x1; //enable mcu write mem
	SG_LDPC->rLDEC_CTRL = 0x183C001;
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLP_LDEC_CTRL = 0x103C001;
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);

	SG_LDPC->rLDEC_FORMAT0 = (gCodeRate[bMode-5]|0x5000000);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLP_LDEC_FORMAT0 = (gCodeRate[bMode-5]|0x5000000);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);

	SG_LDPC->rLDEC_H_MEM_CTRL = 0;
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLP_LDEC_OP_MEM_CTRL = 0;
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);

	Hal_LDPC_LDECSetMatrix((U32 *)(dMemMtxP1),gMatrixSize[bMatrixIdx+2],(U32 *)(dMemMtxP2),gMatrixSize[bMatrixIdx+3]);
	//Hal_LDPC_LDECSetMatrix((U32 *)(dMemMtxP1),292,(U32 *)(dMemMtxP2),592);
	SG_LDPC->rLDEC_MEM_OP_MODE.dAll = 0x0; //disable mcu write mem

	//4-软解配置
#ifndef __BOOT_CODE__
	SG_LDPC->rLDEC_SB_MAP0 = HAL_LDPC_LUTCal(0,4);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP1 = HAL_LDPC_LUTCal(4,8);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP2 = HAL_LDPC_LUTCal(8,12);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP3 = HAL_LDPC_LUTCal(12,16);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP4 = HAL_LDPC_LUTCal(16,20);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP5 = HAL_LDPC_LUTCal(20,24);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP6 = HAL_LDPC_LUTCal(24,28);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
	SG_LDPC->rLDEC_SB_MAP7 = HAL_LDPC_LUTCal(28,32);
	while(SG_LDPC->rMCU_WLDEC_REG_OP_STS.bits.LDEC_REG_WACK!=1);
#endif
}


void HAL_LDPC_ParityChangeCfg(U08 En,U08 DataType)
{
	U08 i;

	if(DataType == 0)
	{
		for(i = 0;i<4;i++)
		{
			SG_DMA2_CH(i)->rECC_CFG.bits.PT_EN = 1;
			SG_DMA2_CH(i)->rECC_CFG.bits.PT_NUM = pFdlParam.ptUdpFdl->m_tHwReg.ADM_gDataPuncLen;
		}
	}
	else
	{
		for(i = 0;i<4;i++)
		{
			SG_DMA2_CH(i)->rECC_CFG.bits.PT_EN = 1;
			SG_DMA2_CH(i)->rECC_CFG.bits.PT_NUM = pFdlParam.ptUdpFdl->m_tHwReg.ADM_gTabPuncLen;
		}
	}
}

void HAL_DMA2_ParityLenCfg(U08 bDataType) //未开启CRC
{
	U08 bDmaCnt = 0;
	U32 dMesageLen = 16384;
	U32 dPageSize = PAGE_SIZE;

	if(bDataType == TYPE_LOGIC)
	{
		dMesageLen += UD_LEN_OF_DATA_CLST*DATA_CLST_OF_PAGE;
		bDmaCnt = DMA_OF_DATA_CLST*DATA_CLST_OF_PAGE;
	}
	else
	{
		dMesageLen += UD_LEN_OF_TBLE_CLST*TBLE_CLST_OF_PAGE;
		bDmaCnt = DMA_OF_TABLE_CLST*TBLE_CLST_OF_PAGE;
	}

	if(((SG_DMA2_CH(0)->rRND_CTRL.bAll>>6)&1) == 1)
	{
		if(((SG_DMA2_CH(0)->rRND_CTRL.bAll>>5)&1) == 1)
	    {
			dMesageLen += 32;
		}
		else
	    {
			dMesageLen += 16;
	    }
	}
	U16 LenCfg0 = SG_DMA2_CH(0)->rECC_CTRL.bits.LDPC_MOD*32;
	U16 LenCfg1 = (dPageSize-dMesageLen)/bDmaCnt;
	if(LenCfg0 > LenCfg1)
	{
		HAL_LDPC_ParityChangeCfg(1,LenCfg0-LenCfg1);
	}
	else
	{
		HAL_LDPC_ParityChangeCfg(0,0);
	}
}
/*
void HAL_LDPC_ItrMaxCfg(U08 ItrVal)
{
	U32 dLdecCtrl = SG_LDPC->rLDEC_CTRL&0xFFFC0FFF;
	U32 dLpLdecCtrl = SG_LDPC->rLP_LDEC_CTRL&0xFFFC0FFF;

	dLdecCtrl |= (U32)(ItrVal<<12);
	dLpLdecCtrl |= (U32)(ItrVal<<12);
	SG_LDPC->rLDEC_CTRL = dLdecCtrl;
	SG_LDPC->rLP_LDEC_CTRL = dLpLdecCtrl;
}
*/
