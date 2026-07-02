#include "hal.h"
#include "hal_define.h"
#include "data_type.h"
#include "ToshibaSeed.h"

T_GLOBAL_HAL_VAR gHalVar;

U08_C HAL_LDPCModeList[6]   = {5,6,7,8,9,10};
U16_C HAL_LDPCParityLen[6]  = {160,0,224,256,288,320};


/**************************************************************************
 *函数名称: HAL_GetParityLen
 *函数功能: 获取Parity 长度
 *入口参数:
 * @param Null 
 *
 *出口参数:Null
 * @return wParityLen		Parity 长度
 * @SG_Kinder(2018年07月05日(周四))
************************************************************************/
U16 HAL_GetParityLen(void)
{
	/*注意:需要每个通道的ECC Mode一样,否则可能出现风险*/
	U16 wParityLen = 0;
	switch (SG_DMA2_CH(0)->rECC_CTRL.bits.LDPC_MOD)/*认为每个通道ParityLen都一样,默认取通道0*/
	{
		case 5:
			wParityLen = HAL_LDPCParityLen[0];
			break;
		case 7:
		case 8:
			wParityLen = HAL_LDPCParityLen[SG_DMA2_CH(0)->rECC_CTRL.bits.LDPC_MOD - 6];
			break;
		default://硬件不支持模式,死掉
			while(1);
	}
		return (wParityLen);
}

#ifdef __FW__
void  __section(".CODE_INIT") HAL_Init(T_REG_CFG *ptHwReg)
#else
void  HAL_Init(T_REG_CFG *ptHwReg)
#endif
{
	U8 bChan;
	gHalVar.pHwReg = ptHwReg;

	for (bChan = 0; bChan < 4; bChan++)
	{
		SG_FC(bChan)->rNF_CFG.dAll            = ptHwReg->FC_dNfCfg;   
		SG_FC(bChan)->rNF_CFG.bits.DDR_WR_FIFO_CTRL_EN = 1;	// 写FIFO时的余量控制功能(DDR Flash高速读模式下（DQS大于50Mhz）需要打开此功能)，默认打开
		SG_FC(bChan)->rNF_CFG.bits.DDR_WR_FIFO_THROD = 2;	// rx_fifo将要被写满的阈值（FW使用2即可）
		SG_FC(bChan)->rFIO_SPOP_CTRL.dAll     = ptHwReg->FC_dSpopCtrl;           
		SG_FC(bChan)->rDDR_TIMING_CFG.dAll    = ptHwReg->FC_dDDRTimingCfg;       
		SG_FC(bChan)->rCQ_SET_RD_CFG_DQS.dAll = ptHwReg->FC_dFCCqSetRdCfgDqs;    
		SG_FC(bChan)->rCQ_CTRL.dAll           = ptHwReg->FC_dCQCtrl;             
		SG_FC(bChan)->rBAD_COLUMN_CTRL.dAll   = ptHwReg->FC_dBadColCtrl;         
		SG_FC(bChan)->rBADCOL_INFO_BADDR.dAll = ptHwReg->FC_dBadColInfoBAddr;    
		SG_FC(bChan)->rALE_CLE_TIM_CFG.dAll   = ptHwReg->FC_dAleCleTimingCfg;    
		SG_FC(bChan)->rCHK_ST_RB_CNT          = (ptHwReg->FC_dCheckStsThrod*2);//0x47FFFF 23ms -> 46 ms      
		SG_FC(bChan)->rCHK_CQ_CTRL.dAll       = ptHwReg->FC_dCheckCqCtrl;        
		SG_FC(bChan)->rCQ_HOLD_CTRL.dAll      = ptHwReg->FC_dCQHoldCtrl;         
		SG_FC(bChan)->rRND_SEED.dAll          = ptHwReg->FC_dRandSeed;

		SG_FC(bChan)->rCHK_ST_RB_THROD_MIN = 0;

		SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL = 0;//CQ检测状态超时后停止执行后面的CQ，中断里清flag，处理后继续执行
		SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_FAIL_CTRL = 0;//but 继续执行 in flash analyse group test plan---200715
		SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MIN_CTRL = 0;

		SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MINTIME_INT_EN = 1;//中断要打开，默认是打开
		SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_INT_EN = 1;//中断要打开，默认是打开
		SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_FAIL_INT_EN = 1;//中断要打开，默认是打开

		//hal初始值，不需要量产配置的参数
		SG_FC(bChan)->rCHK_INT_ST.dAll  = 0;

		NVIC_EnableIRQ(IRQ_FC0_CQ+bChan);
	}

	SG_DMA1->rLEN.wAll   = ptHwReg->ADM_dDma1LenCfg;

	for (bChan = 0; bChan < 4; bChan++)
    {
		SG_DMA2_CH(bChan)->rLEN_CFG.dAll  = ptHwReg->ADM_dDma2LenCfg;     
        SG_DMA2_CH(bChan)->rECC_CFG.dAll  = ptHwReg->ADM_dDma2EccCfg;
        SG_DMA2_CH(bChan)->rECC_CTRL.dAll = ptHwReg->ADM_dDma2EccCtrl;
		SG_DMA2_CH(bChan)->rRND_CTRL.bAll = ptHwReg->ADM_dDma2RndCtrl;
		SG_DMA2_CH(bChan)->rRAID_CTRL.dAll = ptHwReg->ADM_dDma2RaidCtrl;

		//hal初始值，不需要量产配置的参数
 		SG_DMA2_CH(bChan)->rCTRL.bits.FIFO_DISABLE = 0;
 		SG_DMA2_CH(bChan)->rENABLE.bits.FC_FIFORDY_EN = 0;
		SG_DMA2_CH(bChan)->rENABLE.bits.EN = 1;
		SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
		SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN = 0;
		SG_DMA2_CH(bChan)->rMASK = 0;
		SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;
		
		HAL_SetOverFlowThresh(bChan,0xFF,0x200);//后面由量产工具给定,rECC_CFG
		NVIC_EnableIRQ(IRQ_DMAC_DMA20+bChan);
    }

#ifdef __BADBLK_BINDMAP__
	extern U8 bPowerOnFlg;
		//MAPPING 复用矩阵初始化值区域
	if(bPowerOnFlg)
#else
#endif
	{
		//初始化完成后MemMtxA/MemMtxB/MemMtxP1/MemMtxP2里的内容可以丢掉，上层设计时可考虑初始化之前临时从nand读出初始化表
//	ptHwReg->LDPC_dMemMtxA = 0x75E0;
//	ptHwReg->LDPC_dMemMtxB = 0x7800;
//	ptHwReg->LDPC_dMemMtxP1 = 0x6810;
//	ptHwReg->LDPC_dMemMtxP2 = 0x6CA0;

//		HAL_LDPC_Init(SG_DMA2_CH(0)->rECC_CTRL.bits.LDPC_MOD, ptHwReg->LDPC_dMemMtxA, ptHwReg->LDPC_dMemMtxB, ptHwReg->LDPC_dMemMtxP1, ptHwReg->LDPC_dMemMtxP2);
	//HAL_DMA2_ParityLenCfg(1); //未开启CRC
	}

	//hal初始值，不需要量产配置的参数
	SG_ADM->rBUF_SIZE = ptHwReg->ADM_dBufSize;
	SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0x18C0;//  ptHwReg->ADM_dDataErrStopDma2En;//FAIL NO STOP, ERR HOLD CQ ENABLE,NO_W_BUF_STS
	SG_ADM->rTOS_SEED_BSADDR.dAll = 0x490000;//2D种子BOOT里的地址

	NVIC_EnableIRQ(IRQ_DMAC_DMA1);

	//PAD initial
    SG_PADCTRL->rPAD_SSEL_CTRL[0].dAll = ptHwReg->PAD_dSSelCtrl0to3;
    SG_PADCTRL->rPAD_SSEL_CTRL[1].dAll = ptHwReg->PAD_dSSelCtrl0to3;
    SG_PADCTRL->rPAD_SSEL_CTRL[2].dAll = ptHwReg->PAD_dSSelCtrl0to3;
    SG_PADCTRL->rPAD_SSEL_CTRL[3].dAll = ptHwReg->PAD_dSSelCtrl0to3;
    SG_PADCTRL->rPAD_SSEL_CTRL4.dAll = ptHwReg->PAD_dSSelCtrl4;

	for(bChan = 0;bChan<4;bChan++)
	{
		SG_PADCTRL->rPAD_FUNC_CTRL[bChan].dAll = MODE_FUNC;
		SG_PADCTRL->rPAD_PE_CTRL[bChan].dAll = ptHwReg->PAD_dPECtrl0to3;
		SG_PADCTRL->rPAD_PS_CTRL[bChan].dAll = ptHwReg->PAD_dPSCtrl0to3;
	}
	
    SG_PADCTRL->rPAD_PE_CTRL4.dAll = ptHwReg->PAD_dPECtrl4;
    SG_PADCTRL->rPAD_PE_CTRL5.dAll = ptHwReg->PAD_dPECtrl5;
    SG_PADCTRL->rPAD_PS_CTRL4.dAll = ptHwReg->PAD_dPSCtrl4;
    SG_PADCTRL->rPAD_PS_CTRL5.dAll = ptHwReg->PAD_dPSCtrl5;
    SG_PADCTRL->rPAD_FUNC_CTRL4.dAll = ptHwReg->PAD_dFuncCtrl4;
    
#ifdef DEBUG_STAGE		//debug阶段接逻辑分析仪调试，要打开这个宏
	SG_PADCTRL->rPAD_TSEL_CTRL.dAll = 0xFFFFFF;
#else
	SG_PADCTRL->rPAD_TSEL_CTRL0.dAll = ptHwReg->PAD_dTSEL_CTRL;
#endif
	
#ifndef  __FPGA_PLATEFORM
	HAL_SystemInit(ptHwReg->CPM_dSysClkSourSel, ptHwReg->CPM_dSysClkPeriod, ptHwReg->CPM_bTrx2ClkSourSel[0],
				   ptHwReg->CPM_bTrx2ClkPeriod[0], ptHwReg->CPM_bTrx2ClkPeriod[1],
				   ptHwReg->CPM_bTrx2ClkPeriod[2], ptHwReg->CPM_bTrx2ClkPeriod[3]);
#else
	SG_CPM->rLDEC_CLK_PERIOD = 0x8;
#endif

    SG_CPM->rDQS0_DELAY_CTRL = 0x111;//0x111;//0x30;//ptHwReg->CPM_dDQSDelayCtrl;
    SG_CPM->rDQS1_DELAY_CTRL = 0x113;//0x30;//ptHwReg->CPM_dDQSDelayCtrl;
    SG_CPM->rDQS2_DELAY_CTRL = 0x133;//0x30;//ptHwReg->CPM_dDQSDelayCtrl;
    SG_CPM->rDQS3_DELAY_CTRL = 0x110;//0x30;//ptHwReg->CPM_dDQSDelayCtrl;

	if(1)//默认总是打开 (SG_CPM->rSYS_CLK_SOURCE_SEL==8U &&SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL ==1) //系统时钟源400MHz,TRX2时钟1分频DQS 200M
	{
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F0_SEL_DIFF = 1;	//主控开差分
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F1_SEL_DIFF = 1;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F2_SEL_DIFF = 1;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F3_SEL_DIFF = 1;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.VREF_DIR_IN = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.VREF_STB = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F0RDN = 1;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F1RDN = 1;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F2RDN = 1;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F3RDN = 1;

		for (bChan = 0; bChan < 4; bChan++)
		{
			SG_FC(bChan)->rNF_CFG.bits.FC_RD_DATAPH_MODE = 1;	//200M 用16bit mode
			SG_FC(bChan)->rNF_CFG.bits.FC_WR_DATAPH_MODE = 1;	//200M 用16bit mode
		}
	}
 	else if (SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL==2) //系统时钟源400Mhz，TRX2时钟2分频DQS 100M
	{
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F0_SEL_DIFF = 0;	//主控关差分
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F1_SEL_DIFF = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F2_SEL_DIFF = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F3_SEL_DIFF = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F0RDN = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F1RDN = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F2RDN = 0;
		SG_PADCTRL->rPAD_MISC_CTRL.bits.F3RDN = 0;

		for (bChan = 0; bChan < 4; bChan++)
		{
			SG_FC(bChan)->rNF_CFG.bits.FC_RD_DATAPH_MODE = 0;	//100M 用8bit mode
			SG_FC(bChan)->rNF_CFG.bits.FC_WR_DATAPH_MODE = 0;	//100M 用8bit mode
		}
	}
	else 		//DQS既不是100M，也不是200M
	{
		for (bChan = 0; bChan < 4; bChan++)
		{
			HAL_CPMDQS_AddllSetDelay(bChan,0,0);
		}
	}
	
#if (NAND_FLASH == H27QEG8NDB)
	if (SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.CH0_TRX2_CLK_PERIOD==1) //DQS 200M 
	{
		SG_PADCTRL->rPAD_TSEL_CTRL.dAll = 0xFFFFFF;
		for (bChan = 0; bChan < 4; bChan++)
		{
			HAL_CPMDQS_HandleSetDelay(bChan,0x111);
		}
	}
	else if (SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.CH0_TRX2_CLK_PERIOD==2) //DQS 100M 
	{
		SG_PADCTRL->rPAD_TSEL_CTRL.dAll = 0xFFFFFF;
		for (bChan = 0; bChan < 4; bChan++)
		{
			HAL_CPMDQS_AddllSetDelay(bChan,0,0);
		}
	}
#endif

	//HAL_EFUSE_Read();

#if 0 //当前IC没有烧录efuse,校准功能不准确，VDT功能先关闭
	HAL_VDT_Calibration();
	HAL_VDT_Init(1, 0x09);//VDT报警电压阈值2.5V -> 2.7V
#endif
}
