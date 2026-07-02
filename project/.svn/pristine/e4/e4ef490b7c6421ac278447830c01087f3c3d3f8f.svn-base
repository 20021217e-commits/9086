

#include "fdl_test.h"
#include "flash_driver.h"
#include "flash_driver_struct.h"
#include "main.h"
#include "debug.h"
#include "fdl_debug.h"
#include "fdl_task_op.h"
#include "hal_define.h"
#include "pri_cmd.h"
#include "hal_ldpc.h"
#include "uitility.h"
#include "hal_cq.h"
#include "hal_test.h"
#include "ftl.h"
#include "fdl_soft_decode.h"
#include "fdl_common.h"
#include "fdl_mcu_op.h"
#include "system_sg9081.h"
#include "stdlib.h"
#include "ldpc_matrx.h"

extern const unsigned short SumsungSeedTLC[];

//FDL 基本接口的CQ_DEEPTH数值;任务分配值，添加新Flash时需要调整这组参数
U08 bpCQFIFODeep[4][6] = { 
#if  (NAND_FLASH == TOS_6DDK)
/**Erase Write  Read Little NWrite NRead*/
	0x40, 0x50, 0x80, 0x40, 0x50, 0x60,/*1CE*/
	0x40, 0x50, 0x80, 0x40, 0x50, 0x60,/*2CE*/
	0x40, 0x50, 0x80, 0x40, 0x50, 0x60,/*3CE*/
	0x40, 0x50, 0x80, 0x40, 0x50, 0x60,/*4CE*/
#elif (NAND_FLASH == H27QEG8NDB)
/**Erase Write  Read Little NWrite NRead*/
	0x40, 0x50, 0x60, 0x40, 0x50, 0x90,/*1CE*/
	0x40, 0x50, 0x60, 0x40, 0x50, 0x90,/*2CE*/
	0x40, 0x50, 0x60, 0x40, 0x50, 0x90,/*3CE*/
	0x40, 0x50, 0x60, 0x40, 0x50, 0x90,/*4CE*/

#elif (NAND_FLASH == MIC_B0KB)
/**Erase Write  Read Little NWrite NRead*/	
	0x70, 0x60, 0x80, 0x40, 0x50, 0xC0,/*1CE*/
	0x70, 0x60, 0xC0, 0x40, 0x50, 0xC0,/*2CE*/
	0x70, 0x60, 0xE0, 0x40, 0x50, 0xE0,/*3CE*/
	0x70, 0x60, 0xE0, 0x40, 0x50, 0xE0,/*4CE*/

#elif (NAND_FLASH == MIC_B16A)||(NAND_FLASH == SUMSUNG_3DV4)
/**Erase Write  Read Little NWrite NRead*/
	{0x40, 0x50, 0x68, 0x40, 0x50, 0xE0,},/*1CE*/
	{0x40, 0x50, 0x90, 0x40, 0x50, 0xE0,},/*2CE*/
	{0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,},/*3CE*/
	{0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,}/*4CE*/

#elif (NAND_FLASH == MIC_B17A)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0xC0, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0xD0, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*4CE*/ 

#elif (NAND_FLASH == MIC_M16)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x7F, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0xC0, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*4CE*/ 

#elif (NAND_FLASH == MIC_N18) || (NAND_FLASH == MIC_N28)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x7F, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0xC0, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*4CE*/ 

#elif (NAND_FLASH == MIC_B27A)||(NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)||(NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0xC0, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0xE0, 0x40, 0x50, 0xE0,/*4CE*/ 
#elif (NAND_FLASH == K9ADGD8U0C)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*4CE*/ 
#elif (NAND_FLASH == K9ADGD8U0A)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*4CE*/ 

#elif (NAND_FLASH == MIC_L06B)
/**Erase Write  Read Little NWrite NRead*/	
	0x70, 0x60, 0x80, 0x40, 0x50, 0xC0,/*1CE*/ 
	0x70, 0x60, 0x80, 0x40, 0x50, 0xC0,/*2CE*/ 
	0x70, 0x60, 0x80, 0x40, 0x50, 0xC0,/*3CE*/ 
	0x70, 0x60, 0x80, 0x40, 0x50, 0xC0,/*4CE*/ 
#elif (NAND_FLASH == TOS_BICS3)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x68, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0x70, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0x80, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0x90, 0x40, 0x50, 0xE0,/*4CE*/ 

#elif (NAND_FLASH == HYNIX_3DV4)
/**Erase Write  Read Little NWrite NRead*/	
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*1CE*/ 
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*2CE*/ 
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*3CE*/ 
	0x40, 0x50, 0x60, 0x40, 0x50, 0xE0,/*4CE*/ 
#endif
};


T_UDP_FDL tUdpFdl;
T_FDL_PARAM tFdlParam;

SG_FC_TypeDef tHalFCReg;
SG_DMA2_TypeDef tHalADMReg;

void FDL_Test_SetHwTempVar(SG_FC_TypeDef *pTestFC, SG_DMA2_TypeDef *pTestADM)
{
	//先配置默认值
	//======================================================================
	//设置fc的配置参数
	//pTestFC->rCHK_CQ_CTRL.bits.CHK_RB_CTRL = 0;//CQ不执行检测硬件RB脚超时检测
	pTestFC->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL = 1;              //CQ检测状态超时后停止执行后面的CQ，中断里清flag，处理后继续执行
	pTestFC->rCHK_CQ_CTRL.bits.CHK_ST_MINTIME_INT_EN = 0;    //中断要打开，默认是打开
	pTestFC->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_INT_EN = 1;    //中断要打开，默认是打开
	pTestFC->rCHK_CQ_CTRL.bits.CHK_ST_FAIL_INT_EN = 0;       //中断要打开，默认是打开
#ifdef	__FPGA_PLATEFORM
    pTestFC->rCHK_ST_RB_CNT = 0x47FFFF/16;//TRX2 = 16,0x8FFFF,TRX2 = 2,0x47FFFF;约10~20ms
#else
    pTestFC->rCHK_ST_RB_CNT = 0x47FFFF;//TRX2 = 16,0x8FFFF,TRX2 = 2,0x47FFFF;约10~20ms
#endif
	pTestFC->rNF_CFG.bits.ALL_0X00_THRESHOLD = 0;
	pTestFC->rNF_CFG.bits.ALL_0XFF_THRESHOLD = 32;
	pTestFC->rNF_CFG.bits.RD_SMP_CFG = 0x2;		//采样点

//#define __DQS_200M__  //4通道只能用8bit模式
#if 1//def __DQS_200M__
	pTestFC->rNF_CFG.bits.FC_RD_DATAPH_MODE = 1;//16bit mode
	pTestFC->rNF_CFG.bits.FC_WR_DATAPH_MODE = 1;//16bit mode
#else
	pTestFC->rNF_CFG.bits.FC_RD_DATAPH_MODE = 0;
	pTestFC->rNF_CFG.bits.FC_WR_DATAPH_MODE = 0;
#endif
	pTestFC->rNF_CFG.bits.FLASH_MODE =	0x00;//flash 类型//00:Asyn???ONFI SDR 01:ONFI NV-DDR 10:Toggle???ONFI NV-DDR211:??
	pTestFC->rNF_CFG.bits.DDR_WR_FIFO_CTRL_EN = 1;
	pTestFC->rFIO_SPOP_CTRL.dAll = 0;
	pTestFC->rRND_SEED.dAll = 0;      // !no ftl var
	pTestFC->rBAD_COLUMN_CTRL.dAll = 0;
	pTestFC->rALE_CLE_TIM_CFG.bits.LOW_CYC = 0x03;
	pTestFC->rALE_CLE_TIM_CFG.bits.TOTAL_CYC = 0xa;
    //pTestFC->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN = 0;
    pTestFC->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_EN = 1;
	pTestFC->rCQ_HOLD_CTRL.bits.DMA2_ABNL_HOLD_CQ_EN = 1;
	pTestFC->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_REAL = 0;
	pTestFC->rBADCOL_INFO_BADDR.dAll = MEM_JCTABLE_BUFF_ADDR;
	pTestFC->rDDR_TIMING_CFG.bits.RPRE_CYC = 0xf;
    pTestFC->rCQ_SET_RD_CFG_DQS.dAll   = 0x01;

	//设置adm的配置参数
	pTestADM->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	pTestADM->rLEN_CFG.bits.UDATA_ATTCH_SIZE = 32;
	pTestADM->rLEN_CFG.bits.UDATA_LEN = 0;
	pTestADM->rRND_CTRL.bits.RND_EN = 1;
    pTestADM->rRND_CTRL.bits.RND_BYPASS = 0; //0:开rand 1:关rand
    pTestADM->rRND_CTRL.bits.RND_ALG_SEL = 0;
	pTestADM->rRND_CTRL.bits.CRC_EN = 0; //0:开rand 1:关rand
    pTestADM->rECC_CTRL.bits.ECC_EN = 0;
    pTestADM->rECC_CFG.bits.OVFLOW_THRESHOLD = 0xA;
    if(HAL_DATA_PUNC_LEN == 0)
    {
        pTestADM->rECC_CFG.bits.PT_EN = 0;
    	pTestADM->rECC_CFG.bits.PT_NUM = 0;
    }
    else
    {
        pTestADM->rECC_CFG.bits.PT_EN = 1;
    	pTestADM->rECC_CFG.bits.PT_NUM = HAL_DATA_PUNC_LEN;
    }

	pTestADM->rRAID_CTRL.dAll = 0;	/*Raid功能*/

	//根据flash配置表HAL相关字段修改相应参数
	//======================================================================
	pTestFC->rNF_CFG.bits.FLASH_MODE     = HAL_INTERFACE_MODE;//Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
	pTestADM->rECC_CTRL.bits.LDPC_MOD    = HAL_LDPC_MODE;//LDPC模式选择: 5,7,8
	pTestADM->rLEN_CFG.bits.BASIC_DATA_LEN = HAL_SECT_OF_DMA2*512; //DMA2的有效数据长度1K

	pTestADM->rECC_CFG.bits.OVFLOW_THRESHOLD = 0x3F;
	pTestADM->rECC_CFG.bits.SVA_OVF_TH = 0x200;
}

void FDL_Test_UDPInit(T_REG_CFG *ptHwReg, SG_FC_TypeDef *pTestFC, SG_DMA2_TypeDef *pTestADM)
{
	//赋值给全局变量(模拟Mptool量产赋值UDP，此些全局变量最终在hal_init里赋值给寄存器)
    ptHwReg->FC_dNfCfg              = pTestFC->rNF_CFG.dAll;
    ptHwReg->FC_dSpopCtrl           = pTestFC->rFIO_SPOP_CTRL.dAll;
    ptHwReg->FC_dDDRTimingCfg       = pTestFC->rDDR_TIMING_CFG.dAll;
    ptHwReg->FC_dFCCqSetRdCfgDqs    = pTestFC->rCQ_SET_RD_CFG_DQS.dAll;
    ptHwReg->FC_dCQCtrl             = pTestFC->rCQ_CTRL.dAll;
    ptHwReg->FC_dBadColCtrl         = pTestFC->rBAD_COLUMN_CTRL.dAll;
    ptHwReg->FC_dBadColInfoBAddr    = pTestFC->rBADCOL_INFO_BADDR.dAll;
    ptHwReg->FC_dAleCleTimingCfg    = pTestFC->rALE_CLE_TIM_CFG.dAll;
    ptHwReg->FC_dCheckStsThrod      = pTestFC->rCHK_ST_RB_CNT;
    ptHwReg->FC_dCheckCqCtrl        = pTestFC->rCHK_CQ_CTRL.dAll;
    ptHwReg->FC_dCQHoldCtrl         = pTestFC->rCQ_HOLD_CTRL.dAll;
	ptHwReg->FC_dRandSeed           = pTestFC->rRND_SEED.dAll;

	//adm dma2 dma1参数写入UDP参数
    ptHwReg->ADM_dDma1LenCfg    = 512;
	ptHwReg->ADM_dDma2LenCfg    = pTestADM->rLEN_CFG.dAll;
    ptHwReg->ADM_dDma2EccCfg    = pTestADM->rECC_CFG.dAll;
    ptHwReg->ADM_dDma2EccCtrl   = pTestADM->rECC_CTRL.dAll;
    ptHwReg->ADM_dDma2RndCtrl   = pTestADM->rRND_CTRL.bAll;
    ptHwReg->ADM_dDma2RaidCtrl  = pTestADM->rRAID_CTRL.dAll;
	ptHwReg->ADM_dBufSize 		= MEM_DMA_BUF_SEC;//512;//此参数FTL设计者根据memory规划可进行调整

#if (defined __FPGA_FTL_3DV4_TEMP__) || (defined __FPGA_FTL_N28A_TEMP__) || (defined __FPGA_FTL_B27B_TEMP__) || (defined __FPGA_FTL_INTEL_B27B_TEMP__)
#ifdef __FDL_CRC_FUNC__
#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)
	ptHwReg->ADM_gDataPuncLen = 18;
	ptHwReg->ADM_gTabPuncLen  = 20;
#else (NAND_FLASH == SUMSUNG_3DV4)
	ptHwReg->ADM_gDataPuncLen = 6;
	ptHwReg->ADM_gTabPuncLen  = 8;
#endif	/*#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)*/
#else
#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)
	ptHwReg->ADM_gDataPuncLen = 14;
	ptHwReg->ADM_gTabPuncLen  = 16;
#else (NAND_FLASH == SUMSUNG_3DV4)
	ptHwReg->ADM_gDataPuncLen = 2;
	ptHwReg->ADM_gTabPuncLen  = 4;
#endif	/*#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)*/
#endif
#else
	ptHwReg->ADM_gDataPuncLen = HAL_DATA_PUNC_LEN;
	ptHwReg->ADM_gTabPuncLen  = HAL_TAB_PUNC_LEN;
#endif

	/******LDPC寄存器设置*************************/
	ptHwReg->LDPC_dLdecCtrl = 0;
	ptHwReg->LDPC_dLencCRSet = 0;
	ptHwReg->LDPC_dLencMainCtrl = 0;

		//调用FDL_Init后LDPC初始化完成后MemMtxA/MemMtxB/MemMtxP1/MemMtxP2里的内容可以丢掉，
		//上层设计时可考虑初始化之前临时从nand读出初始化表放在临时的buff上
        if(pTestADM->rECC_CTRL.bits.LDPC_MOD == LDPC_MODE5)
        {
            ptHwReg->LDPC_dMemMtxA = (U32)(&dHAL_Mtx_5_A[0]);
            ptHwReg->LDPC_dMemMtxB = (U32)(&dHAL_Mtx_5_B[0]);
            ptHwReg->LDPC_dMemMtxP1 = (U32)(&dHAL_Mtx_5_P1[0]);
            ptHwReg->LDPC_dMemMtxP2 = (U32)(&dHAL_Mtx_5_P2[0]);
        }
        else if(pTestADM->rECC_CTRL.bits.LDPC_MOD == LDPC_MODE7)
        {
            ptHwReg->LDPC_dMemMtxA = (U32)(&dHAL_Mtx_7_A[0]);
            ptHwReg->LDPC_dMemMtxB = (U32)(&dHAL_Mtx_7_B[0]);
            ptHwReg->LDPC_dMemMtxP1 = (U32)(&dHAL_Mtx_7_P1[0]);
            ptHwReg->LDPC_dMemMtxP2 = (U32)(&dHAL_Mtx_7_P2[0]);

        }
        else if(pTestADM->rECC_CTRL.bits.LDPC_MOD == LDPC_MODE8)
        {
            ptHwReg->LDPC_dMemMtxA = (U32)(&dHAL_Mtx_8_A[0]);
            ptHwReg->LDPC_dMemMtxB = (U32)(&dHAL_Mtx_8_B[0]);
            ptHwReg->LDPC_dMemMtxP1 = (U32)(&dHAL_Mtx_8_P1[0]);
            ptHwReg->LDPC_dMemMtxP2 = (U32)(&dHAL_Mtx_8_P2[0]);
        }
        else if(pTestADM->rECC_CTRL.bits.LDPC_MOD == LDPC_MODE9)
        {
            ptHwReg->LDPC_dMemMtxA = (U32)(&dHAL_Mtx_9_A[0]);
            ptHwReg->LDPC_dMemMtxB = (U32)(&dHAL_Mtx_9_B[0]);
            ptHwReg->LDPC_dMemMtxP1 = (U32)(&dHAL_Mtx_9_P1[0]);
            ptHwReg->LDPC_dMemMtxP2 = (U32)(&dHAL_Mtx_9_P2[0]);
        }
        else if(pTestADM->rECC_CTRL.bits.LDPC_MOD == LDPC_MODE10)
        {
            ptHwReg->LDPC_dMemMtxA = (U32)(&dHAL_Mtx_10_A[0]);
            ptHwReg->LDPC_dMemMtxB = (U32)(&dHAL_Mtx_10_B[0]);
            ptHwReg->LDPC_dMemMtxP1 = (U32)(&dHAL_Mtx_10_P1[0]);
            ptHwReg->LDPC_dMemMtxP2 = (U32)(&dHAL_Mtx_10_P2[0]);
        }
        else
        {
			while(1);
        }
//        ptHwReg->LDPC_dMemMtxA = dHAL_Mtx_9_A;//0x75E0;
//		ptHwReg->LDPC_dMemMtxB = dHAL_Mtx_9_B;//0x7800;
//		ptHwReg->LDPC_dMemMtxP1 = dHAL_Mtx_9_P1;//0x6810;
//		ptHwReg->LDPC_dMemMtxP2 = dHAL_Mtx_9_P2;//0x6CA0;
	//Pad配置参数
	/*******PAD寄存器设置*************************/
	ptHwReg->PAD_dSSelCtrl0to3 = 0x5555555;//0x6666666;0x7777777
	ptHwReg->PAD_dSSelCtrl4 = 0x11111111;
	ptHwReg->PAD_dPECtrl0to3 = 0x00000FFF;
	ptHwReg->PAD_dPECtrl4 = 0x00FFFFA7;
	ptHwReg->PAD_dPECtrl5 = 0x00000FFF;
	ptHwReg->PAD_dPSCtrl0to3 = 0x0000059F;
	ptHwReg->PAD_dPSCtrl4 = 0x00FFFFA3;
	ptHwReg->PAD_dPSCtrl5 = 0x00000330;
	
	ptHwReg->PAD_dFuncCtrl4 = 0x006CFFF3; //led0/1 func mode,uart func mode,GPIO 8/9为GPIO模式
	//其他IO为Function模式，Boot时已正确初始化，不需要重新配置

	ptHwReg->PAD_dPhyXcfgD0 = 0x00000002;
	ptHwReg->PAD_dPhyXcfgD1 = 0x00000030;
	ptHwReg->PAD_dPhyXcfgD2 = 0x00000000;
	ptHwReg->PAD_dPhyXcfgD3 = 0x00000000;
	ptHwReg->PAD_dPhyXcfgA0 = 0xe0020706;
	ptHwReg->PAD_dPhyXcfgA1 = 0x4010C000;
	ptHwReg->PAD_dPhyXcfgA2 = 0x40878680;
	ptHwReg->PAD_dPhyXcfgA3 = 0x00000084;

	/*******MCU寄存器设置*************************/
	ptHwReg->MCU_dLEDCtrl = 0;
	ptHwReg->MCU_dLED0PeriodCtrl = 0;
	ptHwReg->MCU_dLED1PeriodCtrl = 0;
	

	ptHwReg->CPM_dSysClkSourSel	   = SG_CPM->rSYS_CLK_SOURCE_SEL;
	ptHwReg->CPM_dSysClkPeriod	   = SG_CPM->rSYS_CLK_PERIOD;//SYSCLK_PERIOD_SEL;
	ptHwReg->CPM_bTrx2ClkPeriod[0] = SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL;//TRX2CLK_PERIOD_SEL;
	ptHwReg->CPM_bTrx2ClkPeriod[1] = SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL;//TRX2CLK_PERIOD_SEL;
	ptHwReg->CPM_bTrx2ClkPeriod[2] = SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL;//TRX2CLK_PERIOD_SEL;
	ptHwReg->CPM_bTrx2ClkPeriod[3] = SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL;//TRX2CLK_PERIOD_SEL;

	if (ptHwReg->CPM_dSysClkSourSel==8U && ptHwReg->CPM_bTrx2ClkPeriod[0]==1)	//系统时钟源400Mhz，TRX2时钟1分频    DQS 200M
	{
		#if (NAND_FLASH == MIC_B0KB)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_B16A)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == SUMSUNG_3DV4)
			ptHwReg->CPM_dDQSDelayCtrl = 0xf7;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_B17A)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_B27A)||(NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)||(NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
			ptHwReg->CPM_dDQSDelayCtrl = 0x107;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_M16)||(NAND_FLASH == MIC_N18) || (NAND_FLASH == MIC_N28)
			ptHwReg->CPM_dDQSDelayCtrl = 0x100;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == H27QEG8NDB)
			ptHwReg->CPM_dDQSDelayCtrl = 0x111;
			ptHwReg->PAD_dTSEL_CTRL = 0x00555555;
		#elif (NAND_FLASH == K9ADGD8U0C)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == K9ADGD8U0A)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_L06B)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == TOS_BICS3)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;	
		#elif (NAND_FLASH == HYNIX_3DV4)
			ptHwReg->CPM_dDQSDelayCtrl = 0x70;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;		
		#endif
	}
	else if (ptHwReg->CPM_dSysClkSourSel==8U && ptHwReg->CPM_bTrx2ClkPeriod[0]==2)	//系统时钟源400Mhz，TRX2时钟2分频    DQS 100M
	{
		#if (NAND_FLASH == MIC_B0KB)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_B16A)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == SUMSUNG_3DV4)
			ptHwReg->CPM_dDQSDelayCtrl = 0x370;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_B17A)||(NAND_FLASH == MIC_M16)||(NAND_FLASH == MIC_N18) || (NAND_FLASH == MIC_N28)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_B27A)||(NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)||(NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
			ptHwReg->CPM_dDQSDelayCtrl = 0x377;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == H27QEG8NDB)
			ptHwReg->CPM_dDQSDelayCtrl = 0x337;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;//0x00000000
		#elif (NAND_FLASH == K9ADGD8U0C)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == K9ADGD8U0A)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == MIC_L06B)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;
		#elif (NAND_FLASH == TOS_BICS3)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;	
		#elif (NAND_FLASH == HYNIX_3DV4)
			ptHwReg->CPM_dDQSDelayCtrl = 0x13F;
			ptHwReg->PAD_dTSEL_CTRL = 0x00000000;	
		#endif
	}

	/*******CPM寄存器设置*************************/
	//ptHwReg->CPM_dDQSDelayCtrl = 0x10000000;//ADDLL功能验证后，直接调用相关接口设置delay值;
}



void FDL_Test_MptoolUDPHwReg(void)
{
	T_REG_CFG *ptHwReg = &tUdpFdl.m_tHwReg;

	U08MemSet((U08*)&tHalFCReg, 0, sizeof(SG_FC_TypeDef));
	U08MemSet((U08*)&tHalADMReg, 0, sizeof(SG_FC_TypeDef));
		
	FDL_Test_SetHwTempVar(&tHalFCReg, &tHalADMReg);

	FDL_Test_UDPInit(ptHwReg, &tHalFCReg, &tHalADMReg);
}

void FDL_Test_MptoolUDPFlashOperParam(void)
{
	T_FLASH_OPER_PARAM *tFlashOperParam = &tUdpFdl.m_tFlashOperParam;

	tFlashOperParam->tInitMode.bWorkMode = FDL_FLASH_WORK_MODE;
    tFlashOperParam->tInitMode.bFlashIOMode = FDL_FLASH_IO_MODE;
	tFlashOperParam->tInitMode.bCmdFlg = FDL_FLASH_SOT_TYPE;
	tFlashOperParam->tInitMode.dCmdCQ[0] = FDL_MODE_CHANGE_CMD;

	tFlashOperParam->tPrevRdCmd.bCmdFlg = FDL_READ_HAVE_PRE_CMD;
	tFlashOperParam->tPrevRdCmd.dCmdCQ[0] = FDL_READ_SLC_PRE_CMD;

    tFlashOperParam->tPrevRdCmd.bCmdTlcFlg = FDL_READ_HAVE_PRE_CMD_TLC;
	tFlashOperParam->tPrevRdCmd.bCmdCQTlcMode = FDL_READ_PRE_CMD_TLC_MODE;

	tFlashOperParam->tPrevWrCmd.bCmdFlg = FDL_WRITE_HAVE_PRE_CMD;
	tFlashOperParam->tPrevWrCmd.dCmdCQ[0] = FDL_WRITE_SLC_PRE_CMD;

	tFlashOperParam->tPrevErsCmd.bCmdFlg = FDL_ERASE_HAVE_PRE_CMD;
	tFlashOperParam->tPrevErsCmd.dCmdCQ[0] = FDL_ERASE_SLC_PRE_CMD;

    tFlashOperParam->tMultiPln.bEnable = FDL_MULTI_PLN_EN;
	tFlashOperParam->tMultiPln.bPln0WRCmd = FDL_MUTI_PALNE0_WT_CMD;
	tFlashOperParam->tMultiPln.bPln1WRCmd = FDL_MUTI_PALNE1_WT_CMD;

    tFlashOperParam->tInterlevEn.bMultiLunEn = FDL_MULTI_LUN_EN;
    tFlashOperParam->tInterlevEn.bMultiCeEn = FDL_MULTI_CE_EN;
    tFlashOperParam->tInterlevEn.bMultiClstEn = FDL_MULTI_CLST_EN;

    tFlashOperParam->tCache.bReadEn = FDL_CAHCE_READ_EN;
    tFlashOperParam->tCache.bWriteEn = FDL_CAHCE_WRITE_EN;

    tFlashOperParam->tCache.bSnapReadEn = FDL_SNAP_READ_EN;
}


void FDL_Test_MptoolUDPFlashParam(void)
{
	U8 bChan,bChip,i,bLun;
    U16 wDataColAddr = 0;
	U16 wTableColAddr = 0;
    U16 wParityLength = 0;
    U16 wTabParityLength = 0;
	U8 bEccCorrMode;

	T_FLASH_PARAM *tFlashParam = &tUdpFdl.m_tFlashParam;
	T_REG_CFG *ptHwReg = &tUdpFdl.m_tHwReg;

    //每个DMA的parity长度，用于计算数据和表格的每个簇的起始列地址

	bEccCorrMode = ((ptHwReg->ADM_dDma2EccCtrl>>8) & 0x0f);//ldpc mode
	wParityLength = HAL_LDPCParityLen[bEccCorrMode-5];
	FDL_Log("Ldpc:%d Partiy:%d",HAL_LDPCModeList[bEccCorrMode-5],wParityLength);

	if(((ptHwReg->ADM_dDma2RndCtrl>>6)&1) == 1)
	{
		if(((ptHwReg->ADM_dDma2RndCtrl>>5)&1) == 1)
	    {
			wParityLength = wParityLength + 4;
		}
		else
	    {
			wParityLength = wParityLength + 2;
	    }
	}
#if (defined __FPGA_FTL_3DV4_TEMP__) || (defined __FPGA_FTL_N28A_TEMP__) || (defined __FPGA_FTL_B27B_TEMP__) || (defined __FPGA_FTL_INTEL_B27B_TEMP__)
#ifdef __FDL_CRC_FUNC__
#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)
	wTabParityLength = wParityLength;
	wTabParityLength -= 20;//4;//
	wParityLength    -= 18;//4;//
#else (NAND_FLASH == SUMSUNG_3DV4)
	wTabParityLength = wParityLength;
	wTabParityLength -= 8;//4;//
	wParityLength    -= 6;//4;//
#endif	/*#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)*/
#else
#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)
	wTabParityLength = wParityLength;
	wTabParityLength -= 16;
	wParityLength    -= 14;
#else (NAND_FLASH == SUMSUNG_3DV4)
	wTabParityLength = wParityLength;
	wTabParityLength -= 4;
	wParityLength    -= 2;
#endif	/*#if (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B)*/
#endif
#else
	wParityLength -= HAL_DATA_PUNC_LEN;
	wTabParityLength = wParityLength;
#endif

#ifdef JUMP_CH_CE//跳CH和CE自动Mapping支持
	tFlashParam->m_bChipOfChan = FDL_CHIP_OF_CHAN;
	tFlashParam->m_bLunOfChip = FDL_LUN_OF_CHIP;
	T_INVAL_CHAN_CHIP_MAP tInvalChanChipMap[MAX_CHAN_OF_DEV];
	U08 bPhyChan = 0;
	U08 bPhyChip = 0;
	U08 j = 0;
	memset(tInvalChanChipMap,0,sizeof(tInvalChanChipMap));
	//空白CH Mapping Set
//	tInvalChanChipMap[0].bInvalChan = 1;//Chan0 Inval
//	tInvalChanChipMap[1].bInvalChan = 1;//Chan1 Inval
//	tInvalChanChipMap[2].bInvalChan = 1;//Chan2 Inval
//	tInvalChanChipMap[3].bInvalChan = 1;//Chan3 Inval

	//空白CE Mapping Set
//	tInvalChanChipMap[0].bInvalChip[0] = 1;//Chan0 CE0 Inval
//	tInvalChanChipMap[1].bInvalChip[0] = 1;//Chan1 CE0 Inval
//	tInvalChanChipMap[2].bInvalChip[0] = 1;//Chan2 CE0 Inval
//	tInvalChanChipMap[3].bInvalChip[0] = 1;//Chan3 CE0 Inval
//
//	tInvalChanChipMap[0].bInvalChip[1] = 1;//Chan0 CE1 Inval
//	tInvalChanChipMap[1].bInvalChip[1] = 1;//Chan1 CE1 Inval
//	tInvalChanChipMap[2].bInvalChip[1] = 1;//Chan2 CE1 Inval
//	tInvalChanChipMap[3].bInvalChip[1] = 1;//Chan3 CE1 Inval
//
//	tInvalChanChipMap[0].bInvalChip[2] = 1;//Chan0 CE2 Inval
//	tInvalChanChipMap[1].bInvalChip[2] = 1;//Chan1 CE2 Inval
//	tInvalChanChipMap[2].bInvalChip[2] = 1;//Chan2 CE2 Inval
//	tInvalChanChipMap[3].bInvalChip[2] = 1;//Chan3 CE2 Inval
//
//	tInvalChanChipMap[0].bInvalChip[3] = 1;//Chan0 CE2 Inval
//	tInvalChanChipMap[1].bInvalChip[3] = 1;//Chan1 CE2 Inval
//	tInvalChanChipMap[2].bInvalChip[3] = 1;//Chan2 CE2 Inval
//	tInvalChanChipMap[3].bInvalChip[3] = 1;//Chan3 CE2 Inval

	for(i = 0; i <	MAX_CHAN_OF_DEV; i++)
	{
		for(j = 0; j <	MAX_CHIP_OF_CHAN; j++)
		{
			if(tInvalChanChipMap[i].bInvalChip[j] != 1)
			{	
				tFlashParam->m_bChipMapOfCh[i][bPhyChip] = j;

				bPhyChip++;

				if(bPhyChip == (tFlashParam->m_bChipOfChan)) 
				{
					break;
				}
			}

		}
		
		if(tInvalChanChipMap[i].bInvalChan != 1)
		{
			tFlashParam->m_bChMapOfDev[bPhyChan] = i;
			bPhyChan++;
		}
		bPhyChip = 0;
	}

	for (bChan = 0; bChan < MAX_CHAN_OF_DEV; bChan++)
	{			
		for (bChip = 0; bChip < tFlashParam->m_bChipOfChan; bChip++)
		{			
			for (bLun = 0; bLun < tFlashParam->m_bLunOfChip; bLun++)
			{
				tFlashParam->m_bPhyChipOfLogLun[bChan][bChip*tFlashParam->m_bLunOfChip + bLun] = tFlashParam->m_bChipMapOfCh[bChan][bChip];
				tFlashParam->m_bPhyLunOfLogLun[bChan][bChip*tFlashParam->m_bLunOfChip + bLun] = bLun;		
			}
		}
	}

#else

	FDL_Test_CheckDevice();
#endif

	tFlashParam->m_bChanNum = 0x00;
	tFlashParam->m_bChipOfChan = 0x00;
	
	for (bChan = 0; bChan < MAX_CHAN_OF_DEV; bChan++)//扫描有多少个通道
	{
		if(tUdpFdl.m_tFlashParam.m_bChMapOfDev[bChan] != 0xFF)
		{
			tFlashParam->m_bChanNum++;
		}
	}

	for (bChip = 0; bChip < MAX_CHIP_OF_CHAN; bChip++)//扫描每个通道有多少个片选
	{
		if(tUdpFdl.m_tFlashParam.m_bChipMapOfCh[tUdpFdl.m_tFlashParam.m_bChMapOfDev[0]][bChip] != 0xFF)
		{
			tFlashParam->m_bChipOfChan++;
		}
	}

	FDL_Log("CHAN_MAX_CNT : %d,CHIP_OF_CHAN : %d",tFlashParam->m_bChanNum,tFlashParam->m_bChipOfChan);

	tFlashParam->m_bLunOfChan = tFlashParam->m_bLunOfChip * tFlashParam->m_bChipOfChan;
	tFlashParam->m_bLunOfDev = tFlashParam->m_bLunOfChan * tFlashParam->m_bChanNum;
    tFlashParam->m_bPlaneOfLun = FDL_PLANE_OF_LUN;
	tFlashParam->m_wPageOfBlk = FDL_PAGE_OF_BLK;
	tFlashParam->m_wPageOfBlkSLC = FDL_SLC_PAGE_OF_BLK;
	tFlashParam->m_wRowOfBlk = FDL_ROW_OF_BLK;
	tFlashParam->m_wRowGapOfBlkInPlane = FDL_ROW_GAP_OF_BLK_IN_PLANE;
	tFlashParam->m_dRowGapOfPlane = FDL_ROW_GAP_OF_PLANE;
	tFlashParam->m_dRowGapOf2LUN = FDL_ROW_GAP_OF_2LUN;
	tFlashParam->m_wBlkOfLUN = FDL_BLK_OF_LUN;
	tFlashParam->m_bDataSectOfPage = FDL_SECT_OF_PAGE;
	tFlashParam->m_bTabSectOfPage = FDL_SECT_OF_PAGE;
	tFlashParam->m_bDataSectOfDMA = HAL_SECT_OF_DMA2;//DMA_LEN;//4;//1; dma length = 2k
	tFlashParam->m_bTabSectOfDMA = HAL_SECT_OF_DMA2;
	tFlashParam->m_bCellType = 3;
	tFlashParam->m_wPageSize = FDL_PAGE_SIZE;

	//HAL_MemSet(&tFlashParam->m_bPageModelMaskBit[0],INVALID_U8,128);

    tFlashParam->m_bDataUserDataLen = 4;
    tFlashParam->m_bTabUserDataLen = 4;
    tFlashParam->m_bMskByteOfClst = 1;
    tFlashParam->m_bSectOfDataClst = 8;

    tFlashParam->m_bSectOfTableClst = 4;
    tFlashParam->m_bTableClstOfPage = tFlashParam->m_bDataSectOfPage / tFlashParam->m_bSectOfTableClst;
    tFlashParam->m_bDataClstOfPage = tFlashParam->m_bDataSectOfPage / tFlashParam->m_bSectOfDataClst;
    tFlashParam->m_bBlockGapInPlane = 1;

    tFlashParam->m_bDMAOfDataClst = tFlashParam->m_bSectOfDataClst / tFlashParam->m_bDataSectOfDMA;
	tFlashParam->m_bDMAOfTableClst = tFlashParam->m_bSectOfTableClst / tFlashParam->m_bTabSectOfDMA;

	
	tUdpFdl.m_tPhyParam.m_bCQDeepthErase 		= bpCQFIFODeep[tFlashParam->m_bLunOfChan - 1][0];
	tUdpFdl.m_tPhyParam.m_bCQDeepthWrite 		= bpCQFIFODeep[tFlashParam->m_bLunOfChan - 1][1];
	tUdpFdl.m_tPhyParam.m_bCQDeepthRead 		= bpCQFIFODeep[tFlashParam->m_bLunOfChan - 1][2];
	tUdpFdl.m_tPhyParam.m_bCQDeepthLittleData 	= bpCQFIFODeep[tFlashParam->m_bLunOfChan - 1][3];
	tUdpFdl.m_tPhyParam.m_bCQDeepthNCQWrite 	= bpCQFIFODeep[tFlashParam->m_bLunOfChan - 1][4];
	tUdpFdl.m_tPhyParam.m_bCQDeepthNCQRead 		= bpCQFIFODeep[tFlashParam->m_bLunOfChan - 1][5];

	
    tFlashParam->m_bDataSectOfPage = tFlashParam->m_wPageSize/(tFlashParam->m_bDataSectOfDMA * 512+wParityLength)*tFlashParam->m_bDataSectOfDMA;

    if(tFlashParam->m_bDataSectOfPage >= 32)
    {
        tFlashParam->m_bDataSectOfPage = 32;
    }
    else  if(tFlashParam->m_bDataSectOfPage >= 16)
    {
        tFlashParam->m_bDataSectOfPage = 16;
    }
    else
    {
        tFlashParam->m_bDataSectOfPage = 8;
    }
//#ifdef __FDL_TEST__ //验证ldpc测试用，量产工具需根据需要及flash page size巨鼎是否用ldpc
	//LDPC模式ECC parity比较长，有的flash放不下，测试模式，固定页存储有效数据容量
	    //tFlashParam->m_bDataSectOfPage = 16;//ldpc的parity比较长，会超出pagesize,测试模式先固定
//#endif

    tFlashParam->m_bTableClstOfPage = tFlashParam->m_bDataSectOfPage / tFlashParam->m_bSectOfTableClst;
    tFlashParam->m_bDataClstOfPage = tFlashParam->m_bDataSectOfPage / tFlashParam->m_bSectOfDataClst;

    U08 bDmaOfDataPage = tFlashParam->m_bDataSectOfPage / tFlashParam->m_bDataSectOfDMA;
	for (i = 0; i < 32; i++)
	{
        if ((i % bDmaOfDataPage) == 0)
        {
            wDataColAddr = 0;
			wTableColAddr = 0;
        }
		else
		{
			wDataColAddr += (tFlashParam->m_bDataSectOfDMA * 512);
			wTableColAddr += (tFlashParam->m_bDataSectOfDMA * 512);

			if (((i) % (tFlashParam->m_bSectOfDataClst / tFlashParam->m_bDataSectOfDMA)) == 0)
			{
				wDataColAddr += tFlashParam->m_bDataUserDataLen;
			}

			if (((i) % (tFlashParam->m_bSectOfTableClst / tFlashParam->m_bDataSectOfDMA)) == 0)
			{
				wTableColAddr += tFlashParam->m_bTabUserDataLen;
			}

            wDataColAddr += wParityLength;
            wTableColAddr += wTabParityLength;
		}
		tFlashParam->m_wDataColAddr[i] = wDataColAddr;
		tFlashParam->m_wTabColAddr[i] = wTableColAddr;
        //FDL_Log("i=%x d=%x",i,tFlashParam->m_wDataColAddr[i]);
	}

#if (NAND_FLASH == MIC_B0KB)
	tFlashParam->m_bFlashDriver = 0x02; 	//设置Flash本身的驱动能力,B0KB不需要设置，default
#elif (NAND_FLASH == H27QEG8NDB)
	tFlashParam->m_bFlashDriver = 0x06;		//设置Flash本身的驱动能力,8NDB需要设置为06
#endif	

}

void FDL_Test_Init()
{
    U8 bChan = 0;
	U8 bChip = 0;

	U08MemSet((U08*)&tUdpFdl,INVALID_U8,sizeof(T_UDP_FDL));
    U08MemSet((U08*)&tFdlParam,INVALID_U8,sizeof(T_FDL_PARAM));

	FDL_Test_MptoolUDPHwReg();

	FDL_Test_MptoolUDPFlashOperParam();

	FDL_Test_MptoolUDPFlashParam();

    tFdlParam.ptUdpFdl = &tUdpFdl;
    tFdlParam.dEccInfoBuffAddr = MEM_DMA2_INFO_ADDR;
	tFdlParam.dEccInfoBuffSize = MEM_DMA2_INFO_SIZE;

	tFdlParam.tWriteFailCtrl.m_Fail_StpEn = 1;
	tFdlParam.tReadFailCtrl.m_Fail_StpEn = 0;
	tFdlParam.tReadFailCtrl.m_Fail_RtyEn = 1;
	tFdlParam.tReadFailCtrl.m_Fail_ReRdCnt = 1;
	tFdlParam.tVDTExceptCtrl.m_VDTStatus = VDT_NORMAL;

	tFdlParam.wContiAddrMaxRead = (MEM_NAND_ADDR_BUFF_SIZE/sizeof(T_FLASH_PHY_ADDR));
	tFdlParam.wContiAddrMaxWrite = (MEM_NAND_ADDR_BUFF_SIZE/sizeof(T_FLASH_PHY_ADDR));
	
	U8 i;
	for (i = 0;i < sizeof(tFdlParam.bUnValidDataUser);i++)
	{
		tFdlParam.bUnValidDataUser[i] = 0xBB+i;
	}

	for (i = 0;i < sizeof(tFdlParam.bUnValidTableUser);i++)
	{
		tFdlParam.bUnValidTableUser[i] = 0xAA+i;
	}

	tFdlParam.FTL_CollectRetryInfor = FTL_ReadRetryFail;//FTL注册给FDL，发生retryfail后调用此函数
	tFdlParam.FTL_ExceptionWriteFailHandle = FTL_WriteFail;//FTL注册给FDL，发生retryfail后调用此函数

	FDL_Init(&tFdlParam);

	for(bChan = 0; bChan < 4; bChan ++)
    {
        for(bChip = 0;bChip<FDL_CHIP_OF_CHAN;bChip++)
        {
        	U8 bChanExt = CHANS(bChan);
			U8 bChipExt = CHIPES(bChanExt, bChip);
			if((bChanExt == 0xFF) || (bChipExt == 0xFF))
			{/*Error bChan Or bChip*/}
			else
			{
				FDL_FlashIFModeInit(bChanExt, bChipExt);
			} 
        }
    }
}

void FDL_CMD_C0_FDLInit(void)
{
    HostCmdResponse();

	
    FDL_Test_Init();

    FDL_Log("C0 Init End");
    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}

void FDL_FlashInit(void)
{
    U08 bChan;
    U08 bChip;
    
    for(bChan = 0; bChan < 4; bChan ++)
    {
        U08 PhybChan = CHANS(bChan);
        if(PhybChan == 0xFF)
        {/*Error bChan*/}
        else
        {
            for(bChip = 0;bChip < 4;bChip++)//FDL_CHIP_OF_CHAN
            {
                U08 PhybChip = CHIPES(PhybChan, bChip);
                if(PhybChip == 0xFF)
                {/*Error bChip*/}
                else
                {
                    FDL_FlashIFModeInit(PhybChan, PhybChip);
                    //FDL_Log("FlashIFModeInit--PhybChan:%d,PhybChip:%d",PhybChan, PhybChip);
                } 
            }
        }
    }
}


#ifdef __FDL_TEST_MODE_EN__

//异常分析函数
/*
void FDL_Test_Analyse_Exception(U32 dExceptionBuffAddr,U8 bTaskType)
{
    U32 dBuffAddr = dExceptionBuffAddr;
    U16 i = 0;

    if(bTaskType == ERASE_TYPE)//erase
    {
        T_ERASE_EXCEPTION *tErsEx = (T_ERASE_EXCEPTION *)dBuffAddr;
        T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_ERASE_EXCEPTION));

//      FDL_Log("erase_Dt=%x wLen=%x ",tErsEx->bDescript,tErsEx->wLen);
		if (0 != tErsEx->bDescript)
		{
			FDL_Log("ErsEx descript:%x",tErsEx->bDescript);
		}

        for(i=0;i<tErsEx->wLen;i++)
        {
            FDL_Log("Idx:%x",tInfo->wIndex);//error off task
            FDL_Log("ErrTy:%x",tInfo->ErrType);
            tInfo++;
        }
    }
    else if(bTaskType == READ_TYPE)//read
    {
        T_READ_EXCEPTION *tReadEx = (T_READ_EXCEPTION *)dBuffAddr;
        T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_READ_EXCEPTION));

        //FDL_Log("read_Dt:=%x wLen=%x",tReadEx->bDescript,tReadEx->wLen);

		if (0 != tReadEx->bDescript)
		{
			FDL_Log("ReadEx descript:%x",tReadEx->bDescript);
		}
		
        for(i=0;i<tReadEx->wLen;i++)
        {
            //FDL_Log("ErrInfo:0x%04d-0x%02X",tInfo->wIndex,tInfo->ErrType);//error off task
//            FDL_Log("ErrTy:0x%02X",tInfo->ErrType);
            tInfo++;
        }
    }
    else if(bTaskType == WRITE_TYPE)//write
    {
        T_WRITE_EXCEPTION *tWriteEx = (T_WRITE_EXCEPTION *)dBuffAddr;
        T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_WRITE_EXCEPTION));

//        FDL_Log("write_Dt=%x wLen=%x",tWriteEx->bDescript,tWriteEx->wExLen);

		if (0 != tWriteEx->bDescript)
		{
			FDL_Log("WriteEx descript[%02d]:0x%02x",tWriteEx->wExLen,tWriteEx->bDescript);
		}

        for(i=0;i<tWriteEx->wExLen;i++)
        {
            FDL_Log("Idx:%02d",tInfo->wIndex);//error off task
            FDL_Log("ErrTy:0x%02x",tInfo->ErrType);
            tInfo++;
        }
    }
    else
    {
    }
}
*/
//#ifdef __FDL_TEST_FW_CTRL__
//
//void FDL_CMD_A9_ReadFlash_PhyPage()
//{
//	
//}
//
//void FDL_CMD_A8_WriteFlash_PhyPage()
//{
//}
//
//void SetRegisterValue()
//{
//}
//
//void GetRegisterValue()
//{
//}
//
//void Get_ValidChan()
//{
//}
//
////******************************************************************************
////                        测试接口地址生成函数
//void FDL_Test_CreateReadClstInfo(U32 dBlkAddr, U16 dPageOffs, U16 wClstCnt, U8 bPageCnt, U8 bPlaneCnt, U8 bLunCnt, U8 bChanCnt, T_FLASH_PHY_ADDR *ptClstAddr)
//{
//	U32 i,j,k;
//    U08 bChan = 0;
//	U08 bLunOfChan = 0;
//    U32 dRowAddr;
//
//    if(bPlaneCnt == 1)
//    {
//        for (j = 0; j < bPageCnt; j++)
//        {
//            for (i = 0; i < DATA_CLST_OF_PAGE; i++)
//            {
//                dRowAddr = dBlkAddr*ROW_OF_BLOCK + dPageOffs + j/bChanCnt;
//                ptClstAddr->m_bChan = bChan;
//                ptClstAddr->m_bClstOffs = i;
//                ptClstAddr->m_bDataType = TYPE_LOGIC;
//                ptClstAddr->m_bLunOfChan = bLunOfChan;
//                ptClstAddr->m_dRowAddr = dRowAddr;
//    			ptClstAddr++;
//
//                //FDL_Log("Chan:%x B_ROW:%x",bChan,dRowAddr);
//            }
//
//    		bChan++;
//
//        	if (bChan >= bChanCnt)
//        	{
//        		bChan = 0;
//        	}
//        }
//    }
//    else if(bPlaneCnt == 2)
//    {
//        for (j = 0; j < (bPageCnt/2); j++)
//        {
//            for(k=0; k<2; k++)//plane
//            {
//                for (i = 0; i < DATA_CLST_OF_PAGE; i++)
//                {
//                    dRowAddr = ((dBlkAddr+k)*ROW_OF_BLOCK) + dPageOffs + j/bChanCnt;
//                    ptClstAddr->m_bChan = bChan;
//                    ptClstAddr->m_bClstOffs = i;
//                    ptClstAddr->m_bDataType = TYPE_LOGIC;
//                    ptClstAddr->m_bLunOfChan = bLunOfChan;
//                    ptClstAddr->m_dRowAddr = dRowAddr;
//        			ptClstAddr++;
//                }
//            }
//
//    		bChan++;
//
//        	if (bChan >= bChanCnt)
//        	{
//        		bChan = 0;
//        	}
//        }
//    }
//}
//
//void FDL_Test_CreateWritePageInfo(U32 dBlkAddr, U16 dPageOffs, U16 wClstCnt, U8 bPageCnt, U8 bPlaneCnt, U8 bLunCnt, U8 bChanCnt, T_FLASH_PHY_ADDR *ptPageAddr)
//{
//	U32 i,j,k;
//    U08 bChan = 0;
//	U08 bLunOfChan = 0;
//    U32 dRowAddr;
//
//    if(bPlaneCnt == 1)
//    {
//    	for (i = 0,j=0; i < bPageCnt; i++)
//    	{
//    		dRowAddr = (dBlkAddr+(j/ROW_OF_BLOCK))*ROW_OF_BLOCK +(j%ROW_OF_BLOCK)+dPageOffs;
//    		ptPageAddr->m_bDataType = TYPE_LOGIC;
//    		ptPageAddr->m_bChan    = bChan;
//    		ptPageAddr->m_bLunOfChan = bLunOfChan;
//    		ptPageAddr->m_dRowAddr = dRowAddr;
//			ptPageAddr->m_bClstOffs = 0;
//    		ptPageAddr++;
//    		bChan++;
//
//    		if (bChan >= bChanCnt)
//    		{
//    			bChan = 0;
//    			j++;
//    		}
//    	}
//    }
//    else if(bPlaneCnt == 2)//2Plane联动写测试
//    {
//        for (j = 0; j < (bPageCnt/2); j++)
//        {
//            for(k=0; k<2; k++)
//            {
//                dRowAddr = ((dBlkAddr+k)+(j/ROW_OF_BLOCK))*ROW_OF_BLOCK +((j%ROW_OF_BLOCK)/bChanCnt)+dPageOffs;
//        		ptPageAddr->m_bDataType = TYPE_LOGIC;
//        		ptPageAddr->m_bChan    = bChan;
//        		ptPageAddr->m_bLunOfChan = bLunOfChan;
//        		ptPageAddr->m_dRowAddr = dRowAddr;
//
//        		ptPageAddr++;
//            }
//
//    		bChan++;
//
//    		if (bChan >= bChanCnt)
//    		{
//    			bChan = 0;
//    		}
//        }
//    }
//}
//
//
//void FDL_Test_CreatOrderBuffInfo(U08 bTestCase,U16 wTestBlk,U32 dSrcInforBuff)
//{
//	U08 i = 0;
//	U16 j = 0;
//	U08 bCnt = 0;
//	U08 bPageCnt = 0;
//	if (bTestCase < 2)
//		bCnt = 4;
//	else
//		bCnt = 2;
//	if (bTestCase % 2)
//		bPageCnt = 2;
//	else
//		bPageCnt = 1;
//	if (bTestCase == 4)
//		bPageCnt = 2;
//	if (bTestCase == 5)
//		bCnt = 5;
//	T_FLASH_PROG_ORDER_UNIT * pOrderUnit[5] = {0};
//	for(i = 0;i<bCnt;i++)
//	{
//		pOrderUnit[i] = (T_FLASH_PROG_ORDER_UNIT *)dSrcInforBuff;
//		dSrcInforBuff = (U32)dSrcInforBuff + sizeof(T_FLASH_PROG_ORDER_UNIT);
//	}
//	if(bTestCase < 5)
//	{
//		for(i = 0; i < bCnt; i++)
//		{
//			pOrderUnit[i]->bChan = i;
//			pOrderUnit[i]->bLunOfChan = 0;
//			pOrderUnit[i]->bOpPageCnt = bPageCnt;
//			for(j = 0; j < bPageCnt; j++)
//			{
//				if(bPageCnt == 1)
//				    pOrderUnit[i]->dRowAddr[j] = wTestBlk + j;
//				if(bPageCnt == 2)
//					pOrderUnit[i]->dRowAddr[j] = wTestBlk + j + 16;
//			}
//		}
//	}
//	else if(bTestCase == 5)
//	{
//		pOrderUnit[0]->bChan = 0;
//		pOrderUnit[0]->bLunOfChan = 0;
//		pOrderUnit[0]->bOpPageCnt = 2;
//		pOrderUnit[0]->dRowAddr[0] = wTestBlk + 16;
//		pOrderUnit[0]->dRowAddr[1] = wTestBlk + 17;
//
//		pOrderUnit[1]->bChan = 2;
//		pOrderUnit[1]->bLunOfChan = 0;
//		pOrderUnit[1]->bOpPageCnt = 2;
//		pOrderUnit[1]->dRowAddr[0] = wTestBlk + 16;
//		pOrderUnit[1]->dRowAddr[1] = wTestBlk + 17;
//
//		pOrderUnit[2]->bChan = 0;
//		pOrderUnit[2]->bLunOfChan = 0;
//		pOrderUnit[2]->bOpPageCnt = 1;
//		pOrderUnit[2]->dRowAddr[0] = wTestBlk;
//
//		pOrderUnit[3]->bChan = 1;
//		pOrderUnit[3]->bLunOfChan = 0;
//		pOrderUnit[3]->bOpPageCnt = 1;
//		pOrderUnit[3]->dRowAddr[0] = wTestBlk;
//
//		pOrderUnit[4]->bChan = 3;
//		pOrderUnit[4]->bLunOfChan = 0;
//		pOrderUnit[4]->bOpPageCnt = 1;
//		pOrderUnit[4]->dRowAddr[0] = wTestBlk;
//
//	}
//}
//
//
//void FDL_Test_CreatCopyBackProgInfo(U8 bLogicData,U32 dSrcInforBuff,U32 dDesInforBuff,U8 bChanCnt,U8 bLunCnt,U8 bPlaneCnt,U32 dPageCnt,U16 wSrcStartBlk,U16 wDesStartBlk)
//{
//	U16 i,j,k,m;
//	T_FLASH_PHY_ADDR *pSrcInfor = (T_FLASH_PHY_ADDR *)dSrcInforBuff;
//	T_FLASH_PHY_ADDR *pDesInfor = (T_FLASH_PHY_ADDR *)dDesInforBuff;
//
//	U08 bClstCnt = TBLE_CLST_OF_PAGE;
//	U08 bDataType = TYPE_TABLE;
//	if(bLogicData == TYPE_LOGIC)
//	{
//		bClstCnt = DATA_CLST_OF_PAGE;
//		bDataType = TYPE_LOGIC;
//	}
//	else
//	{
//		bClstCnt = TBLE_CLST_OF_PAGE;
//		bDataType = TYPE_TABLE;
//	}
//
//	U08 bPage = 0;
//	for(m = 0;m < dPageCnt;)
//	{
//		for(i = 0;i<bPlaneCnt;i++)
//		{
//			for(j = 0;j<bLunCnt;j++)
//			{
//				for(k = 0;k<bChanCnt;k++)
//				{
//					pDesInfor->m_bChan = k;
//					pDesInfor->m_bClstOffs = 0;
//					pDesInfor->m_bDataType = bDataType;
//					pDesInfor->m_bLunOfChan = j;
//					pDesInfor->m_dRowAddr = (wDesStartBlk+i)*ROW_OF_BLOCK+bPage;
//
//					pDesInfor++;
//					m++;
//				}
//			}
//
//		}
//
//		bPage++;
//	}
//
//	bPage = 0;
//	U32 dSrcClst = dPageCnt*bClstCnt;
//	while(dSrcClst)
//	{
//		for(i = 0;i < bClstCnt;i++)
//		{
//			for(k = 0;k < bPlaneCnt;k++)
//			{
//				for(j = 0;j<bLunCnt;j++)
//				{
//					for(m = 0;m<bChanCnt;m++)
//					{
//						pSrcInfor->m_bChan = m;
//						pSrcInfor->m_bClstOffs = i;
//						pSrcInfor->m_bDataType = bDataType;
//						pSrcInfor->m_bLunOfChan = j;
//						pSrcInfor->m_dRowAddr = (wSrcStartBlk+k)*ROW_OF_BLOCK+bPage;
//						pSrcInfor++;
//						dSrcClst--;
//					}
//				}
//			}
//		}
//		bPage++;
//	}
//}
//
//void FDL_Test_CreatMultiLunExdieCopyBackInfo(U8 bCaseType,U8 bLunCnt,U8 bTestLun,U16 wPlane0Blk,U32 dSrcInforBuff,U32 dDesInforBuff,U32 dSrcUserBuffer,U32 dDesUserBuffer,U16 wCopyClstCnt,U8 bLostClst,U8 bSlcOrTlc,U8 bMultiPlaneEn)
//{
//	U08 i = 0;
//	U16 j = 0;
//
//	T_SRC_INFO_OF_LUN *pSrcInfo[4] = {0};
//	U32* pSrcRowAddr[4] = {0};
//	U08* pSrcClstOffs[4] = {0};
//
//	T_DST_INFO_OF_LUN *pDstInfo[4] = {0};
//	U32* pDesRowAddr[4] = {0};
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcInfo[i] = (T_SRC_INFO_OF_LUN *)dSrcInforBuff;
//		dSrcInforBuff = (U32)dSrcInforBuff + sizeof(T_SRC_INFO_OF_LUN);
//
//		pDstInfo[i] = (T_DST_INFO_OF_LUN *)dDesInforBuff;
//		dDesInforBuff = (U32)dDesInforBuff + sizeof(T_DST_INFO_OF_LUN);
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcRowAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += (wCopyClstCnt*sizeof(U32));
//        if(bMultiPlaneEn == 0)
//        {
//		   pDesRowAddr[i] = (U32*)dDesInforBuff;
//		   dDesInforBuff += (wCopyClstCnt/DATA_CLST_OF_PAGE*sizeof(U32));
//        }
//        else
//        {
//           pDesRowAddr[i] = (U32*)dDesInforBuff;
//		   dDesInforBuff += ((wCopyClstCnt/(DATA_CLST_OF_PAGE*PLANE_OF_LUN))*sizeof(U32));
//        }
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcClstOffs[i] = (U08*)dSrcInforBuff;
//		dSrcInforBuff += (wCopyClstCnt*sizeof(U8));
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		if((0 == bCaseType)||(3 == bCaseType))
//		{
//			pSrcInfo[i]->bChan = i;
//			pSrcInfo[i]->bLunOfChan = bTestLun;
//
//			pDstInfo[i]->bChan = i;
//			pDstInfo[i]->bLunOfChan = bTestLun+1;
//		}
//		else if((1 == bCaseType)||((4 == bCaseType)))
//		{
//			pSrcInfo[i]->bChan = i/2;
//			pSrcInfo[i]->bLunOfChan = i%2;
//
//			pDstInfo[i]->bChan = i/2+2;
//			pDstInfo[i]->bLunOfChan = i%2;
//		}
//		else if((2 == bCaseType)||(5 == bCaseType))
//		{
//            pSrcInfo[0]->bChan = 0;
//            pSrcInfo[0]->bLunOfChan = 0;
//            pDstInfo[0]->bChan = 2;
//			pDstInfo[0]->bLunOfChan = 0;
//            pSrcInfo[1]->bChan = 0;
//            pSrcInfo[1]->bLunOfChan = 1;
//            pDstInfo[1]->bChan = 2;
//			pDstInfo[1]->bLunOfChan = 1;
//            pSrcInfo[2]->bChan = 1;
//            pSrcInfo[2]->bLunOfChan = 0;
//            pDstInfo[2]->bChan = 1;
//			pDstInfo[2]->bLunOfChan = 1;
//            pSrcInfo[3]->bChan = 3;
//            pSrcInfo[3]->bLunOfChan = 0;
//            pDstInfo[3]->bChan = 3;
//			pDstInfo[3]->bLunOfChan = 1;
//		}
//		else if((6 == bCaseType)||(8 == bCaseType))
//		{
//			pSrcInfo[i]->bChan = i/2;
//			pSrcInfo[i]->bLunOfChan = i%2;
//
//			pDstInfo[i]->bChan = i/2+2;
//			pDstInfo[i]->bLunOfChan = i%2;
//		}
//		
//		pSrcInfo[i]->wClstLen = wCopyClstCnt-bLostClst;
//		pSrcInfo[i]->pdRowAddr= pSrcRowAddr[i];
//		pSrcInfo[i]->pbClstOffs= pSrcClstOffs[i];
//		pSrcInfo[i]->dUdBuffAddr= dSrcUserBuffer+i*0x100;
//		
//        if(bMultiPlaneEn == 0)
//        {
//		    pDstInfo[i]->wPageCnt = wCopyClstCnt/DATA_CLST_OF_PAGE;
//        }
//        else
//        {
//            pDstInfo[i]->wPageCnt = wCopyClstCnt/(DATA_CLST_OF_PAGE*PLANE_OF_LUN);
//        }
//		
//		pDstInfo[i]->pdRowAddr= pDesRowAddr[i];
//
//		for(j = 0;j<pDstInfo[i]->wPageCnt;j++)
//		{
//			*(pDesRowAddr[i]+j) = wPlane0Blk*ROW_OF_BLOCK + j;
//		}
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		U08 *pRowAddrTemp = (U08 *)pSrcRowAddr[i];
//		U08 *pClstOff = pSrcClstOffs[i];
//
//		for(j = 0;j<wCopyClstCnt-bLostClst;j++)
//		{
//            if(bMultiPlaneEn == 0)
//            {
//			    *(pSrcRowAddr[i]+j) = wPlane0Blk*ROW_OF_BLOCK + j/DATA_CLST_OF_PAGE;
//            }
//            else
//            {
//				U08 bplane = j / DATA_CLST_OF_PAGE% PLANE_OF_LUN;
//				*(pSrcRowAddr[i]+j) = (wPlane0Blk+bplane)*ROW_OF_BLOCK + j/(DATA_CLST_OF_PAGE*PLANE_OF_LUN);
//            }
//
//			pRowAddrTemp = (U08 *)(pSrcRowAddr[i]+j);
//			
//			if(bSlcOrTlc == TLC_MODE)
//			{
//              if(bMultiPlaneEn == 0)
//              {
//				*(pRowAddrTemp+3) = (j%(3*DATA_CLST_OF_PAGE)/DATA_CLST_OF_PAGE+1);
//              }
//              else
//              {
//                *(pRowAddrTemp+3) = (j%(3*DATA_CLST_OF_PAGE*PLANE_OF_LUN)/(DATA_CLST_OF_PAGE*PLANE_OF_LUN)+1);
//              }
//			}
//			*(pClstOff+j) = j%DATA_CLST_OF_PAGE;
//		}
//	}
//}
//
//void FDL_Test_CreatMultiLunIndieCopyBackInfo(U8 bCaseType,U8 bLunCnt,U8 bTestLun,U16 wPlaneBlk,U32 dSrcInforBuff,U32 dDesInforBuff,U32 dSrcUserBuffer,U32 dDesUserBuffer,U16 wCopyClstCnt,U08 bLostClst,U8 bSlcOrTlc)
//{	
//	U08 i = 0;
//	U16 j = 0;
//
//	T_SRC_INFO_OF_LUN *pSrcInfo[4] = {0};
//	U32* pSrcRowAddr[4] = {0};
//	U08* pSrcClstOffs[4] = {0};
//
//	T_DST_INFO_OF_LUN *pDstInfo[4] = {0};
//	U32* pDesRowAddr[4] = {0};
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcInfo[i] = (T_SRC_INFO_OF_LUN *)dSrcInforBuff;
//		dSrcInforBuff = (U32)dSrcInforBuff + sizeof(T_SRC_INFO_OF_LUN);
//
//		pDstInfo[i] = (T_DST_INFO_OF_LUN *)dDesInforBuff;
//		dDesInforBuff = (U32)dDesInforBuff + sizeof(T_DST_INFO_OF_LUN);
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcRowAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += (wCopyClstCnt*sizeof(U32));
//		pDesRowAddr[i] = (U32*)dDesInforBuff;
//		dDesInforBuff += (wCopyClstCnt/DATA_CLST_OF_PAGE*sizeof(U32));
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcClstOffs[i] = (U08*)dSrcInforBuff;
//		dSrcInforBuff += (wCopyClstCnt*sizeof(U8));
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		pSrcInfo[i]->bChan = i;
//		pSrcInfo[i]->bLunOfChan = bTestLun;
//
//		pDstInfo[i]->bChan = i;
//		pDstInfo[i]->bLunOfChan = bTestLun;
//		
//		pSrcInfo[i]->wClstLen = wCopyClstCnt;
//		pSrcInfo[i]->pdRowAddr= pSrcRowAddr[i];
//		pSrcInfo[i]->pbClstOffs= pSrcClstOffs[i];
//		pSrcInfo[i]->dUdBuffAddr= dSrcUserBuffer+i*0x100;
//		pDstInfo[i]->wPageCnt = (wCopyClstCnt+bLostClst)/DATA_CLST_OF_PAGE;
//		pDstInfo[i]->pdRowAddr= pDesRowAddr[i];
//
//		for(j = 0;j<pDstInfo[i]->wPageCnt;j++)
//		{
//			*(pDesRowAddr[i]+j) = (wPlaneBlk+1)*ROW_OF_BLOCK + j;
//		}
//	}
//
//	for(i = 0;i<bLunCnt;i++)
//	{
//		U08 *pRowAddrTemp = (U08 *)pSrcRowAddr[i];
//		U08 *pClstOff = pSrcClstOffs[i];
//
//		for(j = 0;j<wCopyClstCnt;j++)
//		{
//			*(pSrcRowAddr[i]+j) = wPlaneBlk*ROW_OF_BLOCK + j/DATA_CLST_OF_PAGE;
//
//			pRowAddrTemp = (U08 *)(pSrcRowAddr[i]+j);
//			if(bSlcOrTlc == TLC_MODE)
//			{
//			  *(pRowAddrTemp+3) = (j%12)/4+1;
//			}
//			*(pClstOff+j) = j%DATA_CLST_OF_PAGE;
//		}
//	}
//}
//
//
//void FDL_Test_CreatMultiLunExdieCopyBackOrderInfo(	U08 bCaseType,
//													U08 bLunPairCnt,
//													U32 dSrcInforBuff,
//							  						U32 dDesInforBuff,
//							  						U32 dSrcUserBuffer,
//							  						U32 dDesUserBuffer,
//							  						U16 wOrderStart,
//							  						U16 wOrderUnitCnt,
//							  						U16 wCopyClstCnt,
//							  						U08 bPhySrcLun,
//							  						U16 wPhySrcBlk,
//							  						U08 bSrcReadMode,
//							  						U08 bMutiPlnEn)
//{
//	U08 i = 0;
//	U16 j = 0;
//
//#if 1
//	U32 dSrcUserAddr[8];//最多8个Lun对，每个Lun对分配一个页的数据Buff 相应的UserDataBuff也相应分配好
//	U32 dDesUserAddr[8];
//
//	for(i = 0;i<8;i++)
//	{
//		dSrcUserAddr[i] = dSrcUserBuffer+i*UD_LEN_OF_DATA_CLST*DATA_CLST_OF_PAGE;
//		dDesUserAddr[i] = dSrcUserAddr[i];
//	}
//#endif
//
//	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo[4] = {0};
//	U32* pSrcRowAddr[4] = {0};
//	U08* pSrcClstOffs[4] = {0};
//	U32* pSrcUserDataAddr[4] = {0};
//
//	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo[4] = {0};
//	T_PROG_ORDER_UNIT_IN_BLK *pDesOrderUnitAddr[4] = {0};
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcInfo[i] = (T_IN_COPY_SRC_INFO_OF_LUN *)dSrcInforBuff;
//		dSrcInforBuff += sizeof(T_IN_COPY_SRC_INFO_OF_LUN);
//
//		pDstInfo[i] = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)dDesInforBuff;
//		dDesInforBuff += sizeof(T_FLASH_ORDER_PROG_INFO_OF_LUN);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcRowAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += wCopyClstCnt*sizeof(U32);
//
//		pDesOrderUnitAddr[i] = (T_PROG_ORDER_UNIT_IN_BLK *)dDesInforBuff;
//		dDesInforBuff += wOrderUnitCnt*sizeof(T_PROG_ORDER_UNIT_IN_BLK);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcClstOffs[i] = (U08*)dSrcInforBuff;
//		dSrcInforBuff += wCopyClstCnt*sizeof(U08);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcUserDataAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += wCopyClstCnt*sizeof(U32);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		if((bCaseType == 0)||(bCaseType == 3))
//		{
//			pSrcInfo[i]->bChan = i;
//			pSrcInfo[i]->bLunOfChan = bPhySrcLun;
//
//			pDstInfo[i]->bChan = i;
//			pDstInfo[i]->bLunOfChan = bPhySrcLun+1;
//		}
//		else if(bCaseType <= 3)
//		{
//			pSrcInfo[i]->bChan = i/2;
//			pSrcInfo[i]->bLunOfChan = i%2;
//
//			pDstInfo[i]->bChan = i/2+2;
//			pDstInfo[i]->bLunOfChan = i%2;
//		}
//		else if((bCaseType == 2)||(bCaseType == 5))
//		{
//			pSrcInfo[0]->bChan = 0;
//			pSrcInfo[0]->bLunOfChan = 0;
//			pSrcInfo[1]->bChan = 1;
//			pSrcInfo[1]->bLunOfChan = 0;
//			pSrcInfo[2]->bChan = 2;
//			pSrcInfo[2]->bLunOfChan = 0;
//			pSrcInfo[3]->bChan = 2;
//			pSrcInfo[3]->bLunOfChan = 1;
//
//			pDstInfo[0]->bChan = 0;
//			pDstInfo[0]->bLunOfChan = 1;
//			pDstInfo[1]->bChan = 1;
//			pDstInfo[1]->bLunOfChan = 1;
//			pDstInfo[2]->bChan = 3;
//			pDstInfo[2]->bLunOfChan = 0;
//			pDstInfo[3]->bChan = 3;
//			pDstInfo[3]->bLunOfChan = 1;
//		}
//
//		pSrcInfo[i]->wClstLen = wCopyClstCnt;
//		pSrcInfo[i]->pdRowAddr= pSrcRowAddr[i];
//		pSrcInfo[i]->pbClstOffs= pSrcClstOffs[i];
//		pSrcInfo[i]->pdUdBuffAddr = pSrcUserDataAddr[i];
//
//		pDstInfo[i]->wOrderUnitCnt = wOrderUnitCnt;
//		pDstInfo[i]->ptOrderUnit = pDesOrderUnitAddr[i];
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		U08 *pRowAddrTemp = (U08 *)pSrcRowAddr[i];
//
//		for(j = 0;j<wCopyClstCnt;j++)
//		{
//			if(j%DATA_CLST_OF_PAGE == 0)
//			{
//				dSrcUserBuffer = dSrcUserAddr[i];
//			}
//
//			*(pSrcClstOffs[i]+j) = j%DATA_CLST_OF_PAGE;
//			*(pSrcUserDataAddr[i]+j) = dSrcUserBuffer;
//			dSrcUserBuffer+=UD_LEN_OF_DATA_CLST;
//
//			if (bSrcReadMode == SLC_MODE)
//			{
//		    	*(pSrcRowAddr[i]+j) = (wPhySrcBlk+((j % (DATA_CLST_OF_PAGE*PLANE_OF_LUN))/ DATA_CLST_OF_PAGE))*ROW_OF_BLOCK + j/(DATA_CLST_OF_PAGE * PLANE_OF_LUN);
//			}
//			else if(bSrcReadMode == TLC_MODE)
//			{
//				*(pSrcRowAddr[i]+j) = wPhySrcBlk*ROW_OF_BLOCK + j/DATA_CLST_OF_PAGE;
//			}
//
//			pRowAddrTemp = (U08 *)(pSrcRowAddr[i]+j);
//			if(bSrcReadMode == TLC_MODE)
//			{
//              if(wCopyClstCnt == 512)
//              {
//				*(pRowAddrTemp+3) = (j%12)/4+1;
//              }
//              else if(wCopyClstCnt == 1024)
//              {
//                *(pRowAddrTemp+3) = (j%24)/8+1;
//              }
//			}
//		}
//	}
//
//	U32 dOrderInfor[3];
//	U08 k;
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		T_PROG_ORDER_UNIT_IN_BLK *pDesUnit = pDesOrderUnitAddr[i];
//
//		for(j = wOrderStart;j<wOrderUnitCnt;j++)
//		{
//			HAL_Test_GetOrderPrgInfor(j,dOrderInfor);
//			pDesUnit->bOpPageCnt = dOrderInfor[0];
//			pDesUnit->bPrgCycle = 1;
//			pDesUnit->wOrderNum = j;
//
//			for(k = 0;k<dOrderInfor[0];k++)
//			{
//				pDesUnit->dRowAddr[k] = dOrderInfor[k+1];
//				pDesUnit->dRowAddr[k] |= ((U32)((k+1)<<24));
//				pDesUnit->dUserDataAddr[k] = dDesUserAddr[i];
//			}
//			pDesUnit++;
//		}
//	}
//}
//
//
//void FDL_Test_CreatMultiLunIndieCopyBackOrderInfo(U08 bCaseType,
//												U08 bLunPairCnt,
//												U32 dSrcInforBuff,
//												U32 dDesInforBuff,
//												U32 dSrcUserBuffer,
//												U32 dDesUserBuffer,
//												U16 wOrderStart,
//												U16 wOrderUnitCnt,
//												U16 wCopyClstCnt,
//												U08 bPhySrcLun,
//												U16 wPhySrcBlk,
//												U08 bSrcReadMode,
//												U08 bMutiPlnEn)
//{
//	U08 i = 0;
//	U16 j = 0;
//
//	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo[4] = {0};
//	U32* pSrcRowAddr[4] = {0};
//	U08* pSrcClstOffs[4] = {0};
//	U32* pSrcUserDataAddr[4] = {0};
//
//	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo[4] = {0};
//	T_PROG_ORDER_UNIT_IN_BLK *pDesOrderUnitAddr[4] = {0};
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcInfo[i] = (T_IN_COPY_SRC_INFO_OF_LUN *)dSrcInforBuff;
//		dSrcInforBuff += sizeof(T_IN_COPY_SRC_INFO_OF_LUN);
//
//		pDstInfo[i] = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)dDesInforBuff;
//		dDesInforBuff += sizeof(T_FLASH_ORDER_PROG_INFO_OF_LUN);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcRowAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += wCopyClstCnt*sizeof(U32);
//
//		pDesOrderUnitAddr[i] = (T_PROG_ORDER_UNIT_IN_BLK *)dDesInforBuff;
//		dDesInforBuff += wOrderUnitCnt*sizeof(T_PROG_ORDER_UNIT_IN_BLK);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcClstOffs[i] = (U08*)dSrcInforBuff;
//		dSrcInforBuff += wCopyClstCnt*sizeof(U08);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcUserDataAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += wCopyClstCnt*sizeof(U32);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcInfo[i]->bChan = i;
//		pSrcInfo[i]->bLunOfChan = bPhySrcLun;
//
//		pDstInfo[i]->bChan = i;
//		pDstInfo[i]->bLunOfChan = bPhySrcLun;
//
//		pSrcInfo[i]->wClstLen = wCopyClstCnt;
//		pSrcInfo[i]->pdRowAddr= pSrcRowAddr[i];
//		pSrcInfo[i]->pbClstOffs= pSrcClstOffs[i];
//		pSrcInfo[i]->pdUdBuffAddr = pSrcUserDataAddr[i];
//
//		pDstInfo[i]->wOrderUnitCnt = wOrderUnitCnt;
//		pDstInfo[i]->ptOrderUnit = pDesOrderUnitAddr[i];
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		U08 *pRowAddrTemp = (U08 *)pSrcRowAddr[i];
//
//		for(j = 0;j<wCopyClstCnt;j++)
//		{
//			*(pSrcClstOffs[i]+j) = j%DATA_CLST_OF_PAGE;
//			*(pSrcUserDataAddr[i]+j) = dSrcUserBuffer;
//			dSrcUserBuffer+=UD_LEN_OF_DATA_CLST;
//
//			if (bSrcReadMode == SLC_MODE)
//			{
//				*(pSrcRowAddr[i]+j) = (wPhySrcBlk+((j % (DATA_CLST_OF_PAGE*PLANE_OF_LUN))/ DATA_CLST_OF_PAGE))*ROW_OF_BLOCK + j/(DATA_CLST_OF_PAGE * PLANE_OF_LUN);
//			}
//			else if(bSrcReadMode == TLC_MODE)
//			{
//				*(pSrcRowAddr[i]+j) = wPhySrcBlk*ROW_OF_BLOCK + j/DATA_CLST_OF_PAGE;
//			}
//
//			pRowAddrTemp = (U08 *)(pSrcRowAddr[i]+j);
//			if(bSrcReadMode == TLC_MODE)
//			{
//			  if(wCopyClstCnt == 512)
//			  {
//				*(pRowAddrTemp+3) = (j%12)/4+1;
//			  }
//			  else if(wCopyClstCnt == 1024)
//			  {
//				*(pRowAddrTemp+3) = (j%24)/8+1;
//			  }
//			}
//		}
//	}
//
//	U32 dOrderInfor[3];
//	U08 k;
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		T_PROG_ORDER_UNIT_IN_BLK *pDesUnit = pDesOrderUnitAddr[i];
//		for(j = wOrderStart;j<wOrderUnitCnt;j++)
//		{
//			HAL_Test_GetOrderPrgInfor(j,dOrderInfor);
//			pDesUnit->bOpPageCnt = dOrderInfor[0];
//			pDesUnit->bPrgCycle = 1;
//			pDesUnit->wOrderNum = j;
//			for(k = 0;k<dOrderInfor[0];k++)
//			{
//				pDesUnit->dRowAddr[k] = 4 * ROW_OF_BLOCK + dOrderInfor[k+1];
//				pDesUnit->dRowAddr[k] |= ((U32)((k+1)<<24));
//				pDesUnit->dUserDataAddr[k] = dDesUserBuffer;
//			}
//			pDesUnit++;
//		}
//	}
//}
//
//void FDL_Test_CreatMultiLunInnerCopyBackOrderInfo(U08 bCaseType,
//												U08 bLunPairCnt,
//												U32 dSrcInforBuff,
//												U32 dDesInforBuff,
//												U32 dSrcUserBuffer,
//												U32 dDesUserBuffer,
//												U16 wOrderStart,
//												U16 wOrderUnitCnt,
//												U16 wCopyClstCnt,
//												U16 wPhySrcBlk,
//												U16 wPhyDesBlk,
//												U08 bMutiPlnEn)
//{
//	U08 i = 0;
//	U16 j = 0;
//
//	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo[4] = {0};
//	U32* pSrcRowAddr[4] = {0};
//	U08* pSrcClstOffs[4] = {0};
//
//	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo[4] = {0};
//	T_PROG_ORDER_UNIT_IN_BLK *pDesOrderUnitAddr[4] = {0};
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcInfo[i] = (T_IN_COPY_SRC_INFO_OF_LUN *)dSrcInforBuff;
//		dSrcInforBuff = (U32)dSrcInforBuff + sizeof(T_IN_COPY_SRC_INFO_OF_LUN);
//
//		pDstInfo[i] = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)dDesInforBuff;
//		dDesInforBuff = (U32)dDesInforBuff + sizeof(T_FLASH_ORDER_PROG_INFO_OF_LUN);
//	}
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcRowAddr[i] = (U32*)dSrcInforBuff;
//		dSrcInforBuff += (wCopyClstCnt*sizeof(U32));
//
//		pDesOrderUnitAddr[i] = (T_PROG_ORDER_UNIT_IN_BLK *)dDesInforBuff;
//		dDesInforBuff += wOrderUnitCnt*sizeof(T_PROG_ORDER_UNIT_IN_BLK);
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcClstOffs[i] = (U08*)dSrcInforBuff;
//		dSrcInforBuff += (wCopyClstCnt*sizeof(U8));
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		pSrcInfo[i]->bChan = i;
//		pSrcInfo[i]->bLunOfChan = 0;
//		pSrcInfo[i]->wClstLen = wCopyClstCnt;
//		pSrcInfo[i]->pdRowAddr= pSrcRowAddr[i];
//		pSrcInfo[i]->pbClstOffs= pSrcClstOffs[i];
//		pSrcInfo[i]->pdUdBuffAddr = (U32 *)dSrcUserBuffer;
//
//		pDstInfo[i]->bChan = i;
//		pDstInfo[i]->bLunOfChan = 0;
//		pDstInfo[i]->wOrderUnitCnt = wOrderUnitCnt;
//		pDstInfo[i]->ptOrderUnit = pDesOrderUnitAddr[i];
//	}
//
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		U08 *pClstOff = pSrcClstOffs[i];
//	
//		for ( j = 0; j < wCopyClstCnt; j++)
//		{
//			if (bMutiPlnEn == 0)
//			{
//				*(pSrcRowAddr[i]+j) = wPhySrcBlk * ROW_OF_BLOCK + j / DATA_CLST_OF_PAGE;
//			}
//			else
//			{
//				*(pSrcRowAddr[i]+j) = (wPhySrcBlk + j % 4) * ROW_OF_BLOCK + j / (DATA_CLST_OF_PAGE * 4);
//			}
//			*(pClstOff+j) = 0x0f;
//		}
//	}
//
//	U32 dOrderInfor[3];
//	U08 k;
//	for(i = 0;i<bLunPairCnt;i++)
//	{
//		T_PROG_ORDER_UNIT_IN_BLK *pDesUnit = pDesOrderUnitAddr[i];
//		for(j = wOrderStart;j<wOrderUnitCnt;j++)
//		{
//			HAL_Test_GetOrderPrgInfor(j,dOrderInfor);
//			pDesUnit->bOpPageCnt = dOrderInfor[0];
//			pDesUnit->bPrgCycle = 1;
//			pDesUnit->wOrderNum = j;
//			for(k = 0;k<dOrderInfor[0];k++)
//			{
//				pDesUnit->dRowAddr[k] = wPhyDesBlk * ROW_OF_BLOCK + dOrderInfor[k+1];
//				pDesUnit->dUserDataAddr[k] = dDesUserBuffer;
//			}
//			pDesUnit++;
//		}
//	}
//}
//												
////						  测试接口地址生成函数 End
////******************************************************************************
//
////******************************************************************************
////                        常规接口测试
//
//void FDL_Test_ReadFlashID(void)
//{
//	U8 i;
//	for(i = 0;i<MAX_CHAN_COUNT;i++)
//	{
//		MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0, 8);
//		FDL_ReadFlashID(MEM_DMA_BUFF_ADDR,i, 0);
//	}
//}
//
//void FDL_Test_EraseBlocks(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 wBlkStartAddr, U16 wBlkCnt)
//{
//    U16 i,j,k;
//    U8 bLunOfChan = 0;
//
//    T_FLASH_PHY_BLK_ADDR *tBlockAddr = (T_FLASH_PHY_BLK_ADDR*)MEM_NAND_ADDR_BUFF_ADDR;
//
//    if(bPlaneCnt == 1)//1plane
//    {
//    	//wBlkCnt: all channal erase block total
//        for (i = wBlkStartAddr; i < (wBlkStartAddr+(wBlkCnt/bChanCnt)); i++)
//        {
//    		for (j = 0; j < bChanCnt; j++)
//    		{
//    	        tBlockAddr->m_bChan = j;
//    	        tBlockAddr->m_bLunOfChan = bLunOfChan;
//    	        tBlockAddr->m_wBlkAddr = i;
//    			//FDL_Log("if: %x %x %x", tBlockAddr->m_bChan, tBlockAddr->m_bLunOfChan, tBlockAddr->m_wBlkAddr);
//    			tBlockAddr++;
//    		}
//        }
//    }
//    else if(bPlaneCnt == 2)//2plane
//    {
//        for (i = wBlkStartAddr; i < (wBlkStartAddr+wBlkCnt); i+= bChanCnt*2)
//        {
//    		for (j = 0; j < bChanCnt; j++)
//    		{
//                for (k = 0; k < 2; k++)
//                {
//                    tBlockAddr->m_bChan = j;
//        	        tBlockAddr->m_bLunOfChan = bLunOfChan;
//        	        tBlockAddr->m_wBlkAddr = i+k;
//                    //FDL_Log("bk: %x", tBlockAddr->m_wBlkAddr);
//        			tBlockAddr++;
//                }
//    		}
//        }
//    }
//
//    tBlockAddr = (T_FLASH_PHY_BLK_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//#if 0//打印物理地址
//	U32 wOffset  = 0;
//	FDL_Log("pAddr000:| Chan | LunOfChan | BlkAddr	|");
//	for(wOffset = 0;wOffset < wBlkCnt;wOffset++)
//	{
//		FDL_Log("pAddr%03d:|..%02d..|.....%02d....|.0x%06X.|",wOffset,\
//					(tBlockAddr + wOffset)->m_bChan,\
//					(tBlockAddr + wOffset)->m_bLunOfChan,\
//					(tBlockAddr + wOffset)->m_wBlkAddr);
//	}
//#endif
//
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//
//    FDL_EraseBlocks(&tTaskCtrl,tBlockAddr,wBlkCnt,SLC_MODE);
//    FDL_WaitTaskDone();
//
//    FDL_Test_Analyse_Exception(tTaskCtrl.dEraseExBuffAddr,ERASE_TYPE);//erase excepiton buff is the same as read's
//
//}
////                        常规接口测试 End
////******************************************************************************
//
//
////******************************************************************************
////                        非联动数据传输接口测试
//
//void FDL_Test_WriteLittleDataMultiChan(U8 bChan,U16 wDataSize,U8 bNumOfCopies)
//{
//    U32 dRowAddr;
//    U16 wBlock = 0;
//	U08 bLunOfChan = 0;
//
//    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//    dRowAddr = wBlock * ROW_OF_BLOCK;
//    tPageAddr->m_bChan = bChan;
//    tPageAddr->m_bClstOffs = 0;
//    tPageAddr->m_bDataType = TYPE_LOGIC;
//    tPageAddr->m_bLunOfChan = bLunOfChan;
//    tPageAddr->m_dRowAddr = dRowAddr;
//	
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
//
//    FDL_WriteLittleDataToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR, tPageAddr, wDataSize, bNumOfCopies,NULL);
//    FDL_WaitTaskDone();
//}
//
//void FDL_Test_ReadLittleDataMultiChan(U8 bChan, U16 wDataSize,U8 bIdxOfCopies)
//{
//	U8 bLunOfChan = 0;
//    U16 wBlock = 0;
//    U32 dRowAddr;
//
//    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//    dRowAddr = wBlock * ROW_OF_BLOCK;
//    tPageAddr->m_bChan = bChan;
//    tPageAddr->m_bClstOffs = 0;
//    tPageAddr->m_bDataType = TYPE_LOGIC;
//    tPageAddr->m_bLunOfChan = bLunOfChan;
//    tPageAddr->m_dRowAddr = dRowAddr;
//	
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
//
//    FDL_ReadLittleDataToBuffer(&tTaskCtrl,MEM_DMA_BUFF_ADDR,tPageAddr,wDataSize,bIdxOfCopies,NULL);
//    FDL_WaitTaskDone();
//}
//
//void FDL_Test_WriteBuffToFlash(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub, U32 dPageCnt)
//{
//    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//    //配置写的地址参数
//    FDL_Test_CreateWritePageInfo(dStartBlkNub, 0, (dPageCnt*DATA_CLST_OF_PAGE), dPageCnt, bPlaneCnt, bLunCnt, bChanCnt, tPageAddr);
//#if 0//打印物理地址
//			U32 wOffset  = 0;
//			FDL_Log("pAddr000:| DataType | Chan | LunOfChan | ClstOffs | RowAddr  |");
//			for(wOffset = 0;wOffset < dPageCnt;wOffset++)
//			{
//				FDL_Log("pAddr%03d:|....%02d....|..%02d..|.....%02d....|....%02d....|.0x%06X.|",wOffset,\
//							(tPageAddr + wOffset)->m_bDataType,\
//							(tPageAddr + wOffset)->m_bChan,\
//							(tPageAddr + wOffset)->m_bLunOfChan,\
//							(tPageAddr + wOffset)->m_bClstOffs,\
//							(tPageAddr + wOffset)->m_dRowAddr);
//			}
//#endif
//
//    if(dPageCnt <= 8 )//DMA_BUFF最大为256Sec,因此dPageCnt不能超过8(8*32Sec=256Sec)
//    {
//        U08MemSet((U8 *)MEM_DMA_BUFF_ADDR,0x88,dPageCnt * 32 * 512);
//    }
//    else
//    {
//        U08MemSet((U8 *)MEM_DMA_BUFF_ADDR,0x88,8 * 32 * 512);
//    }
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//    tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
//    tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
//
//    FDL_WriteBuffToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR,(T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR,dPageCnt,MEM_UD_BUFF_ADDR,NULL);
//    FDL_WaitTaskDone();
//
//    FDL_Test_Analyse_Exception(tTaskCtrl.dWriteExBuffAddr,WRITE_TYPE);
//}
//
//
//void FDL_Test_ReadClstToBuff(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub, U16 wStartPageNub, U32 dPageCnt)
//{
//    U32 bClstCnt = dPageCnt*DATA_CLST_OF_PAGE;
//
//    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//    FDL_Test_CreateReadClstInfo(dStartBlkNub, wStartPageNub, bClstCnt, dPageCnt, bPlaneCnt, bLunCnt, bChanCnt, ptClstAddr);
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
//
//    FDL_ReadClstToBuffWithMsk((FDL_TASK_CTRL *)(&tTaskCtrl),MEM_DMA_BUFF_ADDR,(T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR,bClstCnt ,(MEM_UD_BUFF_ADDR + 512),NULL,NULL);
//    FDL_WaitTaskDone();
//
//    //分析异常情况
//    FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr,READ_TYPE);
//}
//
//void FDL_Test_ReadClstToBuffWithMask(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub, U32 dPageCnt)
//{
//	U08 i;//k;
//    U32 bClstCnt = dPageCnt*DATA_CLST_OF_PAGE;
//    U08 pMskBuff[16];
//
//    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//    FDL_Test_CreateReadClstInfo(dStartBlkNub, 0, bClstCnt, dPageCnt, bPlaneCnt, bLunCnt, bChanCnt, ptClstAddr);
//
//    //set mask info
//    for(i=0;i<dPageCnt*DATA_CLST_OF_PAGE*MASK_BYTE_OF_CLST;i++)
//    {
//        ((U8 *)pMskBuff)[i] = 0;
//    }
//
//    pMskBuff[0] = 0xFF;
//    pMskBuff[1] = 0xFF;
//    pMskBuff[2] = 0xFF;
//    pMskBuff[3] = 0x3F;
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
//
//    FDL_ReadClstToBuffWithMsk(&tTaskCtrl,MEM_DMA_BUFF_ADDR,(T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR,bClstCnt ,(MEM_UD_BUFF_ADDR + 512),pMskBuff,NULL);
//    FDL_WaitTaskDone();
//}
//
////                        非联动数据传输接口测试 End
////******************************************************************************
//
//
////******************************************************************************
////                        联动数据传输接口测试
//
//void FDL_Test_JointWriteHostToDiscreteBuff(void)
//{
//    //分段传输，每段传输扇区个数
//    U16 wSec[8] =     {8, 8,  4,  8,  4,  8,   8,   8};
//    //每段传输位置相对DMA_Buff地址的Index偏移
//	U16 wBuffIdx[8] = {0, 16, 32, 48, 64, 80, 96, 112};
//
//
////	U16 wSec[WSEC_NUM] = {8, 8, 8, 8, 8,8,8,8};//分段传输，记录每段传输扇区个数
////	U16 wBuffIdx[WSEC_NUM] = {0, 20, 40, 60, 80,112,120,140};//每段传输位置相对base地址的Index偏移
//	FDL_JointWriteHostToDiscreteBuff(MEM_DMA_BUFF_ADDR, wBuffIdx, wSec, 8, JOINT_12);
//#if 0
//    FDL_Log(    "********Mem Printf********\r\n%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X",
//                 SWAP_D(HAL_BFCGetBuffStatus(0*16)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(16)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(32)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(48)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(64)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(80)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(96)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(112)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(128)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(144)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(160)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(176)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(192)),\
//                 SWAP_D(HAL_BFCGetBuffStatus(208))
//     );
//#endif
//#if 0
//    for(i = 0; i < 5; i++)
//	{
//		dBuffAddr = MEM_DMA_BUFF_ADDR + wBuffIdx[i]*BYTE_OF_SEC;
//        FDL_Log("wSec[%d]-wBuffIdx[%d]:%d-%d,dBuffAddr:0x%X-0x%X",i,i,wSec[i],wBuffIdx[i],dBuffAddr,dBuffAddr + (wSec[i] * BYTE_OF_SEC));
//        for(j = dBuffAddr;j < dBuffAddr + (wSec[i] * BYTE_OF_SEC);j+=16)
//        {
//           FDL_Log("0x%02X:%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",j,\
//            *((U8 *)((j+0))), *((U8 *)((j+1))), *((U8 *)((j+2))), *((U8 *)((j+3))),\
//            *((U8 *)((j+4))), *((U8 *)((j+5))), *((U8 *)((j+6))), *((U8 *)((j+7))),\
//            *((U8 *)((j+8))), *((U8 *)((j+9))), *((U8 *)((j+10))),*((U8 *)((j+11))),\
//            *((U8 *)((j+12))),*((U8 *)((j+13))),*((U8 *)((j+14))),*((U8 *)((j+15))));
//        }
//    }
//    #endif
//}
//
//void FDL_Test_WriteDiscreteBuffToFlash( U8 bChanCnt,U32 wBlkStartAddr,U32 dTotalSctSize,U16 wStartPageNub)
//{
//    U32 i;
//    U08 bLunOfChan = 0;
//    U08 bChan = 0;
//    U32 dRowAddr;
//	U16 *pwSeedIndex = NULL;
//	U16 wBuffIdx[8] = {0, 16, 32, 48, 64, 80, 96, 112};;//每段传输位置相对base地址的Index偏移
//	U08 bBuffClstCnt = dTotalSctSize/SECT_OF_DATA_CLST;
//
//    /*
//           计算Page个数,一个Page有32个Sec
//          例如TotalSct = 33,需要2Page才能存下,PageCnt = (33 + (32 - 1)) / 32 =64 / 32 =2;
//           例如TotalSct = 31,需要1Page才能存下,PageCnt = (31 + (32 - 1)) / 32 =63 / 32 =1;
//    */
//	U8 bPageCnt = (dTotalSctSize + SECT_OF_PAGE - 1)/SECT_OF_PAGE;
//    //FDL_Log("TotalSct:%d,bPageCnt:%d,bClstCnt:%d,SECT_OF_DATA_CLST:%d",dTotalSctSize,bPageCnt,bBuffClstCnt,SECT_OF_DATA_CLST);
//
//    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//
//    for (i = 0; i < bPageCnt; i++)
//    {
//    	dRowAddr = wBlkStartAddr*ROW_OF_BLOCK + i/bChanCnt + wStartPageNub;
//    	tPageAddr->m_bChan = bChan;
//    	tPageAddr->m_bClstOffs = 0;
//    	tPageAddr->m_bDataType = TYPE_LOGIC;
//    	tPageAddr->m_bLunOfChan = bLunOfChan;
//    	tPageAddr->m_dRowAddr = dRowAddr;
//    	tPageAddr++;
//
//    	bChan++;
//
//    	if (bChan >= bChanCnt)
//    	{
//    		bChan = 0;
//    	}
//    }
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
//
//	FDL_WriteDiscreteBuffToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR, wBuffIdx, bBuffClstCnt,
//			(T_FLASH_PHY_ADDR const *)MEM_NAND_ADDR_BUFF_ADDR, bPageCnt, MEM_UD_BUFF_ADDR,
//			pwSeedIndex, /*JOINT_22*/JOINT_NONE);
//
//	FDL_WaitTaskDone();
//}
//
//void FDL_Test_ReadClstToDiscreteBuff(U8 bChanCnt, U32 wBlkStartAddr, U32 dTotalClstSize)
//{
//
//
//	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//	U16 wClstCnt 		= 8;
//    U16 *pwSeedIndex 	= NULL;
//    U16 wBuffIndex[8] 	= {0, 16, 32, 48, 64, 80, 96, 112};//每段传输位置相对base地址的Index偏移
//    U08 bMaskBuff[8]	= {0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55};
//
//	FDL_Test_CreateReadClstInfo(wBlkStartAddr, 0, wClstCnt, wClstCnt/DATA_CLST_OF_PAGE, 1, 1, bChanCnt, tClstAddr);
//
//
//#if 0//打印物理地址
//		U32 wOffset  = 0;
//		FDL_Log("pAddr000:| DataType | Chan | LunOfChan | ClstOffs | RowAddr  |");
//		for(wOffset = 0;wOffset < wClstCnt;wOffset++)
//		{
//			FDL_Log("pAddr%03d:|....%02d....|..%02d..|.....%02d....|....%02d....|.0x%06X.|",wOffset,\
//						(tClstAddr + wOffset)->m_bDataType,\
//						(tClstAddr + wOffset)->m_bChan,\
//						(tClstAddr + wOffset)->m_bLunOfChan,\
//						(tClstAddr + wOffset)->m_bClstOffs,\
//						(tClstAddr + wOffset)->m_dRowAddr);
//		}
//#endif
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
//	FDL_ReadClstToDiscreteBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR, wBuffIndex, (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR,
//					wClstCnt, MEM_UD1_BUFF_ADDR, bMaskBuff, pwSeedIndex, JOINT_NONE);
//	FDL_WaitTaskDone();
//
//}
//
//void FDL_Test_JointWritePages(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub,U32 dTotalSctSize)
//{
//    U16 bPageCnt;
//    U32 dBufferAddr = MEM_DMA_BUFF_ADDR;
//
//    T_FLASH_PHY_ADDR  *ptPageAddr = (T_FLASH_PHY_ADDR  *)MEM_JCTABLE_BUFF_ADDR;
//    U16 *pwSeedIdx = NULL;
//
//	bPageCnt = dTotalSctSize/SECT_OF_PAGE;
//
//#ifdef __FDL_TEST_FW_CTRL__
//    FDL_Test_CreateWritePageInfo(dStartBlkNub, 0, (bPageCnt*DATA_CLST_OF_PAGE), bPageCnt, bPlaneCnt, bLunCnt, bChanCnt, ptPageAddr);
//#else
//
//    if(((U16 *)MEM_SEED_BUFF_ADDR)[0] != 0xFFFF)
//    {
//        pwSeedIdx = (U16 *)MEM_SEED_BUFF_ADDR;
//    }
//#endif
//#if 0//打印物理地址
//    FDL_Log("ptPageAddr--0000:|DataType | Chan | LunOfChan | ClstOffs | RowAddr  |");
//    for(i = 0;i < bPageCnt;i++)
//    {
//        FDL_Log("ptClstAddr--%04d:|   %02d    |  %02d  |    %02d     |   %02d     | 0x%06X |",i,\
//                    (ptPageAddr + i)->m_bDataType,\
//                    (ptPageAddr + i)->m_bChan,\
//                    (ptPageAddr + i)->m_bLunOfChan,\
//                    (ptPageAddr + i)->m_bClstOffs,\
//                    (ptPageAddr + i)->m_dRowAddr);
//    }
//
//#endif
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
//
//    FDL_JointWritePages(&tTaskCtrl,dBufferAddr, 0, dTotalSctSize, ptPageAddr,bPageCnt,MEM_UD1_BUFF_ADDR,pwSeedIdx);
//    FDL_WaitTaskDone();
//
//    FDL_Test_Analyse_Exception(tTaskCtrl.dWriteExBuffAddr,WRITE_TYPE);
//}
//
//void FDL_Test_JointReadClsts(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub,U32 dTotalSctSize)
//{
//
//    U16 bPageCnt;
//    U16 bClstCnt = 0;
//    U32 dBufferAddr = MEM_DMA_BUFF_ADDR;
//
//    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
//    U16 *pwSeedIdx = NULL;
//
//	bPageCnt = dTotalSctSize/SECT_OF_PAGE;
//    bClstCnt = bPageCnt*DATA_CLST_OF_PAGE;
//
//#ifdef __FDL_TEST_FW_CTRL__
//    //创建读的簇信息
//    FDL_Test_CreateReadClstInfo(dStartBlkNub, 0, bClstCnt, bPageCnt, bPlaneCnt, bLunCnt, bChanCnt, ptClstAddr);
//#else
//    if(((U16 *)MEM_SEED_BUFF_ADDR)[0] != 0xFFFF)
//    {
//        pwSeedIdx = (U16 *)MEM_SEED_BUFF_ADDR;
//    }
//#endif
//
//#if 0//打印物理地址
//    FDL_Log("ptClstAddr--0000:|DataType | Chan | LunOfChan | ClstOffs | RowAddr  |");
//    for(i = 0;i < bClstCnt;i++)
//    {
//        FDL_Log("ptClstAddr--%04d:|   %02d    |  %02d  |    %02d     |   %02d     | 0x%06X |",i,\
//                    (ptClstAddr + i)->m_bDataType,\
//                    (ptClstAddr + i)->m_bChan,\
//                    (ptClstAddr + i)->m_bLunOfChan,\
//                    (ptClstAddr + i)->m_bClstOffs,\
//                    (ptClstAddr + i)->m_dRowAddr);
//    }
//
//#endif
//
//    //调用FDL接口执行读簇
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
//
//    FDL_JointReadClsts(&tTaskCtrl,dBufferAddr,ptClstAddr,0,dTotalSctSize,bClstCnt,MEM_UD1_BUFF_ADDR/*,NULL*/,pwSeedIdx);
//    FDL_WaitTaskDone();
//
//    FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr,READ_TYPE);
//
//}
//
//void FDL_Test_JointReadClstsWithMask(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub, U32 dBufferAddr,U32 dBufferSize,U32 dTotalSctSize)
//{
//	U32 i;
//    U16 bPageCnt;
//    U16 bClstCnt = 0;
//	U08 pMskBuff[16];
//
//    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
//
//	bPageCnt = dTotalSctSize/SECT_OF_PAGE;
//    bClstCnt = bPageCnt*DATA_CLST_OF_PAGE;
//
//    //创建读的簇信息
//    FDL_Test_CreateReadClstInfo(dStartBlkNub, 0, bClstCnt, bPageCnt, bPlaneCnt, bLunCnt, bChanCnt, ptClstAddr);
//
//    FDL_Log("Mask");
//
//    for(i=0;i<bPageCnt*DATA_CLST_OF_PAGE*MASK_BYTE_OF_CLST;i++)
//    {
//        ((U8 *)pMskBuff)[i] = 0;
//    }
//
//    U08MemSet(pMskBuff, 0, 16);
//
//#if 0
//    pMskBuff[0] = 0x7C;
//    pMskBuff[1] = 0xC3;
//    pMskBuff[2] = 0x33;
//    pMskBuff[3] = 0xCC;
//
//    SG_BFC->rBIT_INDEX.dAll = 0;
//    SG_BFC->rSET_BIT_STS.dAll = 0xC37C;
//    while(SG_BFC->rSET_BIT_STS.dAll){;}
//    SG_BFC->rBIT_INDEX.dAll = 16;
//    SG_BFC->rSET_BIT_STS.dAll = 0xCC33;
//    while(SG_BFC->rSET_BIT_STS.dAll){;}
//#endif
//
//#if 0
//    pMskBuff[0] = 0xFF;
//    pMskBuff[1] = 0xFF;
//    pMskBuff[2] = 0xFF;
//    pMskBuff[3] = 0x3F;
//
//    SG_BFC->rBIT_INDEX.dAll = 0;
//    SG_BFC->rSET_BIT_STS.dAll = 0xFFFF;
//    while(SG_BFC->rSET_BIT_STS.dAll){;}
//    SG_BFC->rBIT_INDEX.dAll = 16;
//    SG_BFC->rSET_BIT_STS.dAll = 0x3FFF;
//    while(SG_BFC->rSET_BIT_STS.dAll){;}
//#endif
//
//#if 1
//    pMskBuff[0] = 0x03;
//    pMskBuff[1] = 0x00;
//    pMskBuff[2] = 0x00;
//    pMskBuff[3] = 0x00;
//
//    //set buff status
//    SG_BFC->rBIT_INDEX.dAll = 0;
//    SG_BFC->rSET_BIT_STS.dAll = 0x0003;
//    while(SG_BFC->rSET_BIT_STS.dAll){;}
//#endif
//
//    //FDL_Log("MaskAddr:%x",pMskBuff);
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
//		
//    FDL_JointReadClsts(&tTaskCtrl,dBufferAddr,ptClstAddr,0,dTotalSctSize,bClstCnt,(MEM_UD_BUFF_ADDR + 512)/*,pMskBuff*/,NULL);
//    FDL_WaitTaskDone();
//}
//
////                        联动数据传输接口测试 End
////******************************************************************************
//
////******************************************************************************
////                        数据搬移CopyBack接口测试
//
//void FDL_Test_CopyBackProg(U8 bChanCnt,U08 bUserSame,U8 bLostClst)
//{
//	
//}
//
//void FDL_Test_MultiLunExdieCopyBackPrg(U8 bCase,U08 bLostClst)
//{
//	
//}
//
//void FDL_Test_MultiLunInDieCopyBackPrg(U8 bTestCase)
//{
//	
//}
//
//void FDL_Test_MultiLunExdieCopyBackOrderPrg(U8 bCase)
//{
//
//}
//
//void FDL_Test_MultiLunIndieCopyBackOrderPrg(U8 bCase)
//{
//	
//}
//
//void FDL_Test_MultiLunInnerCopyBackOrderPrg(U8 bCase)
//{
//	
//}
//
////                        数据搬移CopyBack Order接口测试 End
////******************************************************************************
//
////******************************************************************************
////                         Order编程接口
//void FDL_TEST_OrderPrgBuffToFlash(U8 bTestCase)
//{
//	enum
//	{
//		MODE_FOURCHAN_SINGLE_ONEPAGE = 0,
//		MODE_FOURCHAN_SINGLE_TWOPAGE,
//		MODE_TWOCHAN_FOUR_ONEPAGE,
//		MODE_TWOCHAN_TWO_TWOPAGE,
//		MODE_TWOCHAN_FOUR_TWOPAGE,
//		MODE_2C3C_1P4P_2P1P,
//	};
//	T_PLANE_PARAM * tPlaneParam;
//	tPlaneParam = (T_PLANE_PARAM *)(0x480000);
//	U16 i = 0;
//	U08 j = 0;
//	U08 k = 0;
//	U32 dBufferSou = (0x460000);
//	U32 dBuffUs = (0x470000);
//	U16 wTestBlk = 0;
//	U08 bCnt = 0;
//
//	if(bTestCase < 2)
//	{
//		bCnt = 4;
//		tPlaneParam->bHeadUnitCnt 			= 4;
//		tPlaneParam->wMidUnitCnt 			= 4;
//		tPlaneParam->bLastUnitCnt 			= 4;
//		tPlaneParam->bHeadUnitOfMultiPlane 	= 1;
//		tPlaneParam->bMidUnitOfMultiPlane 	= 1;
//		tPlaneParam->bLastUnitOfMultiPlane 	= 1;
//	}
//	else if((bTestCase >= 2) && (bTestCase < 4))
//	{
//		bCnt = 2;
//		tPlaneParam->bHeadUnitCnt 			= 2;
//		tPlaneParam->wMidUnitCnt 			= 2;
//		tPlaneParam->bLastUnitCnt 			= 2;
//		if(bTestCase == 2)
//		tPlaneParam->bHeadUnitOfMultiPlane 	= 4;
//		else if(bTestCase == 3)
//		tPlaneParam->bHeadUnitOfMultiPlane 	= 2;
//	}
//	else if(bTestCase == 4)
//	{
//		bCnt = 2;
//		tPlaneParam->bHeadUnitCnt 			= 2;
//		tPlaneParam->bHeadUnitOfMultiPlane 	= 4;
//	}
//	else if(bTestCase == 5)
//	{
//		bCnt = 5;
//		tPlaneParam->bHeadUnitCnt 			= 2;
//		tPlaneParam->wMidUnitCnt 			= 5;
//		tPlaneParam->bLastUnitCnt 			= 5;
//		tPlaneParam->bHeadUnitOfMultiPlane 	= 1;
//		tPlaneParam->bMidUnitOfMultiPlane 	= 4;
//		tPlaneParam->bLastUnitOfMultiPlane 	= 1;
//	}
//	for(i = 0; i < 512; i++)
//	{
//		MemorySet((U8 *)(0x410000 + 512*i),i,512);
//	}
//	for(i = 0;i<0x400;i++)
//	{
//		MemorySet((U8 *)(dBuffUs+i*4),0x55+i,4);
//	}
//	
//	HAL_Test_ErsBlks(TLC_MODE, 0, 3, 0,0, 0, 3);
//	
//	FDL_Test_CreatOrderBuffInfo(bTestCase, wTestBlk,dBufferSou);
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
//
//	FDL_OrderPrgBuffToFlash(&tTaskCtrl,0x410000, tPlaneParam, (T_FLASH_PROG_ORDER_UNIT*)dBufferSou, dBuffUs, bCnt,NULL);
//	FDL_WaitTaskDone();
//	if(MODE_FOURCHAN_SINGLE_ONEPAGE == bTestCase)
//	{
//		for(i = 0; i < 4; i++)
//		{
//			LOG_PRINT("CASE 0 Chan=%d\r\n",i);
//			MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//			//FDL_ReadOnePage(i,0,0,wTestBlk,0,MEM_DMA_BUFF_ADDR,0,1);
//			if(SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//			{
//				LOG_PRINT("FailPage\r\n");
//				SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//			}
//		}
//	}
//	else if(MODE_FOURCHAN_SINGLE_TWOPAGE == bTestCase)
//	{
//		for(i = 0; i < 4; i++)
//		{
//			LOG_PRINT("CASE 1 Chan=%d\r\n",i);
//			for(j = 0; j < 2; j++)
//			{
//				MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//				//FDL_ReadOnePage(i,0,0,wTestBlk+j+16,0,MEM_DMA_BUFF_ADDR,0,1);
//				if(SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//				{
//					LOG_PRINT("FailPage\r\n");
//					SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//				}
//			}
//		}
//	}
//	else if(MODE_TWOCHAN_FOUR_ONEPAGE == bTestCase)
//	{
//		for(i = 0; i < 2; i++)
//		{
//			LOG_PRINT("CASE 2 Chan=%d\r\n",i);
//			for(j = 0; j < 4; j++)
//			{
//
//				MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//				//FDL_ReadOnePage(i,0,0,(wTestBlk+j*2048),0,MEM_DMA_BUFF_ADDR,0,1);
//				if(SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//				{
//					LOG_PRINT("FailPage\r\n");
//					SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//				}
//			}
//		}
//	}
//	else if(MODE_TWOCHAN_TWO_TWOPAGE == bTestCase)
//	{
//		for(i = 0; i < 2; i++)
//		{
//			LOG_PRINT("CASE 2 Chan=%d\r\n",i);
//			for(j = 0; j < 2; j++)
//			{
//				for(k = 0; k < 2; k++)
//				{
//					MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//					//FDL_ReadOnePage(i,0,0,(wTestBlk+j*2048+k+16),0,MEM_DMA_BUFF_ADDR,0,1);
//					if(SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//					{
//						LOG_PRINT("FailPage\r\n");
//						SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//					}
//				}
//			}
//		}
//	}
//	else if(MODE_TWOCHAN_FOUR_TWOPAGE == bTestCase)
//	{
//		for(i = 0; i < 2; i++)
//		{
//			LOG_PRINT("CASE 2 Chan=%d\r\n",i);
//			for(j = 0; j < 4; j++)
//			{
//				for(k = 0; k < 2; k++)
//				{
//					MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//					//FDL_ReadOnePage(i,0,0,(wTestBlk+j*2048+k+16),0,MEM_DMA_BUFF_ADDR,0,1);
//					if(SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//					{
//						LOG_PRINT("FailPage\r\n");
//						SG_DMA2_CH(i)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//					}
//				}
//			}
//		}
//	}
//	else if(MODE_2C3C_1P4P_2P1P == bTestCase)
//	{
//
//		LOG_PRINT("CASE 2 Chan=0\r\n");
//		for(k = 0; k < 2; k++)
//		{
//			MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//			//FDL_ReadOnePage(0,0,0,(wTestBlk+k+16),0,MEM_DMA_BUFF_ADDR,0,1);
//			if(SG_DMA2_CH(0)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//			{
//				LOG_PRINT("FailPage\r\n");
//				SG_DMA2_CH(0)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//			}
//		}
//		LOG_PRINT("CASE 2 Chan=2\r\n");
//		for(k = 0; k < 2; k++)
//		{
//			MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//			//FDL_ReadOnePage(2,0,0,(wTestBlk+k+16),0,MEM_DMA_BUFF_ADDR,0,1);
//			if(SG_DMA2_CH(2)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//			{
//				LOG_PRINT("FailPage\r\n");
//				SG_DMA2_CH(2)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//			}
//		}
//		LOG_PRINT("CASE 2 Chan=0\r\n");
//		for(k = 0; k < 4; k++)
//		{
//			MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//			//FDL_ReadOnePage(0,0,0,(wTestBlk+k*2048),0,MEM_DMA_BUFF_ADDR,0,1);
//			if(SG_DMA2_CH(0)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//			{
//				LOG_PRINT("FailPage\r\n");
//				SG_DMA2_CH(0)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//			}
//		}
//		LOG_PRINT("CASE 2 Chan=1\r\n");
//		for(k = 0; k < 4; k++)
//		{
//			MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//			//FDL_ReadOnePage(1,0,0,(wTestBlk+k*2048),0,MEM_DMA_BUFF_ADDR,0,1);
//			if(SG_DMA2_CH(1)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//			{
//				LOG_PRINT("FailPage\r\n");
//				SG_DMA2_CH(1)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//			}
//		}
//		LOG_PRINT("CASE 2 Chan=3\r\n");
//		for(k = 0; k < 4; k++)
//		{
//			MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xFF,16*0x200);
//			//FDL_ReadOnePage(3,0,0,(wTestBlk+k*2048),0,MEM_DMA_BUFF_ADDR,0,1);
//			if(SG_DMA2_CH(3)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
//			{
//				LOG_PRINT("FailPage\r\n");
//				SG_DMA2_CH(3)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
//			}
//		}
//
//	}
//
//
//}
//
////                         Order编程接口 End
////******************************************************************************
//
//
////******************************************************************************
////                         Continue Task
//void FDL_Test_ContinueCopyBack(U8 bChanCnt, U32 dStartBlkNub, U16 wStartPageNub, U32 dPageCnt)
//{
//	int dClusterOfPage = DATA_CLST_OF_PAGE;
//
//	//FDL_TASK_CTRL tTaskCtrl = {{0,0},0,1,0,1};
//	T_FLASH_PHY_ADDR *ptSrcClst = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//	T_FLASH_PHY_ADDR *ptDstPage = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
//
//	//源地址赋值
//	FDL_Test_CreateReadClstInfo(dStartBlkNub, 0, (dPageCnt*dClusterOfPage), dPageCnt, 1, 1, bChanCnt, ptSrcClst);
//
//	//目标地址
//	FDL_Test_CreateWritePageInfo(dStartBlkNub, wStartPageNub, (dPageCnt*dClusterOfPage), dPageCnt, 1, 1, bChanCnt, ptDstPage);
//
//	FDL_TASK_CTRL tTaskCtrl={0};
//	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bReadFailCtrl = READ_FAIL_STOP;
//
//	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
//	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
//	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
//
//    ptSrcClst = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
//	ptDstPage = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
//
//#if 0 //一次执行完
//	FDL_CopyBackProg(ptSrcClst, dPageCnt*dClusterOfPage, ptDstPage, dPageCnt, MEM_UD_BUFF_ADDR, MEM_UD_BUFF_ADDR,NULL,NULL);
//
//#else//使用continue task
//
//	FDL_CopyBackProg(&tTaskCtrl,ptSrcClst, dPageCnt*dClusterOfPage, ptDstPage, dPageCnt-2, MEM_DMA_BUFF_ADDR,MEM_DMA_BUF_SEC, MEM_UD_BUFF_ADDR, MEM_UD_BUFF_ADDR,NULL,NULL);
//
//	HAL_Delay(10);
//
//	FDL_ContinueTask(2);//添加2个元素
//
//#endif
//
//	FDL_WaitTaskDone();
//
//}
//
//void FDL_Test_ContinueTask(U08 bFdlMode)
//{
//	enum
//    {
//        FDL_COPY_BACK = 0,
//    };
//
//	if(bFdlMode == FDL_COPY_BACK)
//	{
//
//		FDL_Test_EraseBlocks(4, 1, 1, 30, 4);
//
//		FDL_Test_WriteBuffToFlash(2, 1, 1, 30, 8);
//
//		FDL_Test_ContinueCopyBack(2, 30, 4, 4);
//
//		FDL_Test_ReadClstToBuff(2, 1, 1, 30, 4, 4);
//	}
//	else
//	{
//
//	}
//
//}
////                         Continue Task End
////******************************************************************************
//
//
////#ifdef __FDL_TEST_FW_CTRL__
//void FDL_CMD_C1_EraseBlocks(void)
//{
//	HostCmdResponse();
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0xee, 512);
//	FDL_Log("C1 EraseBlocks");
//
//#ifdef __MULTI_PLANE_TEST__
//    FDL_Test_EraseBlocks(1, 1, 2, 80, 4);
//#else
//    FDL_Test_EraseBlocks(1, 1, 1, 80, 4);
//#endif
//
//    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//
//	FDL_Log("C1 end");
//}
//
//void FDL_CMD_C2_WriteBuffToFlash(void)
//{
//	HostCmdResponse();
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 48*512);
//	FDL_Log("C2 WriteBuffToFlash");
//
//#if 0//4 channel
//	FDL_Test_WriteBuffToFlash(4, 1, 1, 30, 4);
//#else//1 channel
//
//#ifdef __MULTI_PLANE_TEST__
//	FDL_Test_WriteBuffToFlash(1, 1, 2, 80, 4);
//#else
//    FDL_Test_WriteBuffToFlash(1, 1, 1, 80, 4);
//#endif
//
//#endif
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//	
//	FDL_Log("C2 End");
//}
//
//void FDL_CMD_C3_ReadClstToBuff(void)
//{
//	HostCmdResponse();
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 48*512);
//	FDL_Log("C3 ReadClstToBuff");
//
//#if 0//4 channel
//	FDL_Test_ReadClstToBuff(4, 1, 1, 30, 0, 4);
//
//#else//1 channel
//
//#ifdef __MULTI_PLANE_TEST__
//	FDL_Test_ReadClstToBuff(1, 1, 2, 80, 0, 4);
//#else
//	FDL_Test_ReadClstToBuff(1, 1, 1, 80, 0, 4);
//#endif
//
//#endif
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//
//	FDL_Log("C3 End");
//}
//
//void FDL_CMD_C4_ReadClstToBuffWithMask(void)
//{
//	HostCmdResponse();
//
//	FDL_Log("C4 ReadClstToBuffWithMask");
//
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 128*512);
//
//#ifdef __MULTI_PLANE_TEST__
//    FDL_Test_ReadClstToBuffWithMask(1, 1, 2, 80, 4);
//#else
//    FDL_Test_ReadClstToBuffWithMask(1, 1, 1, 80, 4);
//#endif
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
//
//	FDL_Log("C4 End");
//}
//
//void FDL_CMD_C5_WriteLittleData(void)
//{
//    FDL_Log("C5 little data Write");
//	HostCmdResponse();
//
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 128*512);
//
//	//先擦除若干个块
//	FDL_Test_EraseBlocks(1,1,1,0,1);
//
//    FDL_Test_WriteLittleDataMultiChan(0, 32, 10);
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//    FDL_Log("C5 End");
//}
//
//void FDL_CMD_C6_ReadLittleData(void)
//{
//    FDL_Log("C6 little data Read");
//	HostCmdResponse();
//
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 128*512);
//
//    FDL_Test_ReadLittleDataMultiChan(0, 32, 0);
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//    FDL_Log("C6 End");
//}
//
//void FDL_CMD_C7_JointWritePages(void)
//{
//    HostCmdResponse();
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 128*512);
//
//    FDL_Log("C7 JointWrite SecCnt=%x",tPrivCMD.wOpSctCnt);
//
//#ifdef __MULTI_PLANE_TEST__
//    FDL_Test_EraseBlocks(4, 1, 2, 80, 8);
//	FDL_Test_JointWritePages(4, 1, 2, 80, tPrivCMD.wOpSctCnt);
//
//#else
//    FDL_Test_EraseBlocks(4, 1, 1, 80, 8);
//	FDL_Test_JointWritePages(4, 1, 1, 80, tPrivCMD.wOpSctCnt);
//#endif
//
//	FDL_Log("C7 End");
//}
//
//void FDL_CMD_C8_JointReadClsts(void)
//{
//	HostCmdResponse();
//	FDL_Log("C8 JointRead wOpSctCnt=%x",tPrivCMD.wOpSctCnt);
//
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 128*512);
//
//#ifdef __MULTI_PLANE_TEST__
//    FDL_Test_JointReadClsts(4, 1, 2, 80, tPrivCMD.wOpSctCnt);
//#else
//    FDL_Test_JointReadClsts(4, 1, 1, 80, tPrivCMD.wOpSctCnt);
//#endif
//
//	FDL_Log("C8 End");
//}
//
//void FDL_CMD_C9_JointReadClstsWithMask(void)
//{
//	HostCmdResponse();
//	FDL_Log("C9 JointReadMask wOpSctCnt=%x\r\n",tPrivCMD.wOpSctCnt);
//
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 128*512);
//
//#ifdef __MULTI_PLANE_TEST__
//    FDL_Test_JointReadClstsWithMask(4, 1, 2, 30, MEM_DMA_BUFF_ADDR, MEM_DMA_BUFF_SIZE, tPrivCMD.wOpSctCnt);
//#else
//    FDL_Test_JointReadClstsWithMask(4, 1, 1, 30, MEM_DMA_BUFF_ADDR, MEM_DMA_BUFF_SIZE, tPrivCMD.wOpSctCnt);
//#endif
//
//	FDL_Log("C9 End");
//}
//
//void FDL_CMD_CA_WriteDiscreteBuffToFlash(void)
//{
//   	HostCmdResponse();
//	
//    FDL_Log("CA discrete Buffer write");
//
//    FDL_Test_EraseBlocks(4,1,1,30,8);
//
//	FDL_Test_WriteDiscreteBuffToFlash(4, 80, 64, 2);
//	
//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
//	FDL_Log("CA End");
//}
//
//void FDL_CMD_CB_ReadClstToDiscreteBuff(void)
//{
//   	HostCmdResponse();
//	
//    FDL_Log("CB discrete Buffer read:");
//	
//	FDL_Test_EraseBlocks(1, 1, 1, 80, 4);
//	
//	FDL_Test_WriteBuffToFlash(1, 1, 1, 80, 4);
//
//    FDL_Test_ReadClstToDiscreteBuff(1, 80, 8);
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to hosta
//	
//	FDL_Log("CB End");
//}
//
//void FDL_CMD_CC_MergeBuffClstToFlash(void)
//{
//	HostCmdResponse();
//
//	FDL_Log("CC MergeBuffClstToFlash");
//	
//	FDL_Test_EraseBlocks(1, 1, 1, 80, 4);
//	
//	FDL_Test_WriteBuffToFlash(1, 1, 1, 80, 2);
//	
//	FDL_Test_MergeBuffClstToFlash(1,80,2);
//	
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//	FDL_Log("CC End");
//}
//
//void FDL_CMD_CD_JointDiscreteWirteToBuff(void)
//{
//}
//
//void FDL_CMD_CE_FlashReadID(void)
//{
//	FDL_Log("CE Read ID");
//	HostCmdResponse();
//
//	FDL_Test_ReadFlashID();
//    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//	FDL_Log("CE End");
//}
//
//void FDL_CMD_D8_CopyBackProg(void)
//{
//	HostCmdResponse();
//	FDL_Log("D8 CopyBackProg");
//
//	FDL_Test_CopyBackProg(0,0,0);
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//
//	FDL_Log("D8 End");
//}
//
//void FDL_CMD_D9_MultiLunExdieCopyBackPrg(void)
//{
//    HostCmdResponse();
//
//	FDL_Log("D9 MultiLunExdieCopyBackPrg");
//
//    FDL_Test_MultiLunExdieCopyBackPrg(0,0);
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//	FDL_Log("D9 End");
//}
//
//void FDL_CMD_DA_MultiLunIndieCopyBackPrg(void)
//{
//    HostCmdResponse();
//
//    FDL_Log("DA MultiLunInDieCopyBackPrg");
//
//	FDL_Test_MultiLunInDieCopyBackPrg(0); 
//
//    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//    FDL_Log("DA End");
//}
//
//void FDL_CMD_DB_OrderPrgBuffToFlash(void)
//{
//	HostCmdResponse();
//
//    FDL_Log("DB OrderPrg");
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 128*512);
//
//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
//}
//
//void FDL_CMD_DC_JointOrderPrgUnit(void)
//{
//	HostCmdResponse();
//
//    FDL_Log("DC JointOrder");
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 128*512);
//
//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
//}
//
//void FDL_CMD_DD_MultiLunExdieCopyBackOrderPrg(void)
//{
//    HostCmdResponse();
//
//	U08 bTestCase = tPrivCMD.bOpCh;
//	FDL_Log("Cmd :DD MultiLunExdieCopyBackOrderPrg Case %x",bTestCase);
//
//    //...
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//}
//
//void FDL_CMD_DE_MultiLunIndieCopyBackOrderPrg(void)
//{
//    HostCmdResponse();
//
//	U08 bTestCase = tPrivCMD.bOpCh;
//	FDL_Log("Cmd :DE MultiLunInDieCopyBackPrg Case %x",bTestCase);
//
//    //...
//
//	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
//}
//
//void FDL_CMD_DF_MultiLunInnerCopyBackOrderPrg(void)
//{
//	HostCmdResponse();
//
//    FDL_Log("DF MultiLunInner");
//	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 128*512);
//
//    //...
//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
//}
//
//void FDL_CMD_F2_WriteTlcBlk(void)
//{
//
//}
//
//#else//PC发下来操作的地址参数


/*
void FDL_CMD_C1_EraseBlocks(void)
{
    //FDL_TASK_CTRL tTaskCtrl = {{0,0},0,1};
    T_FLASH_PHY_BLK_ADDR *tBlockAddr = (T_FLASH_PHY_BLK_ADDR*)MEM_NAND_ADDR_BUFF_ADDR;
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bMode = pCtrl->bBuff[0];
    FDL_Log("C1 Erase BlkNum=%x",pCtrl->dBlkCnt);
#if 1//打印物理地址
		U32 wOffset  = 0;
		FDL_Log("pAddr000:| Chan | LunOfChan | BlkAddr  |");
		for(wOffset = 0;wOffset < pCtrl->dBlkCnt;wOffset++)
		{
			FDL_Log("pAddr%03d:|..%02d..|.....%02d....|.0x%06X.|",wOffset,\
						(tBlockAddr + wOffset)->m_bChan,\
						(tBlockAddr + wOffset)->m_bLunOfChan,\
						(tBlockAddr + wOffset)->m_wBlkAddr);
		}
#endif

    HostCmdResponse();

    FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
    FDL_EraseBlocks(&tTaskCtrl,tBlockAddr,pCtrl->dBlkCnt,bMode);
    FDL_WaitTaskDone();

    FDL_Log("C1 Erase End");

    FDL_Test_Analyse_Exception(tTaskCtrl.dEraseExBuffAddr,ERASE_TYPE);//erase excepiton buff is the same as read's

    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host

}

void FDL_CMD_C2_WriteBuffToFlash(void)
{
#if 1
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
    T_FLASH_PHY_ADDR *ptPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
    
    U08 bSeedIdxEn = pCtrl->dSeedBuffEn;//0:无SeedIdx信息，1:有SeedIdx信息
    U16 *pwSeedIdxBuff = NULL;

    if(bSeedIdxEn)
    {
        FDL_Log("SeedIdxEn");
        pwSeedIdxBuff = (U16 *)MEM_SEED_BUFF_ADDR;
    }

	HostCmdResponse();
	//打印物理地址
	Fdb_PrintfPhyAddress(ptPageAddr, pCtrl->dPageCnt);

#if 0
	HAL_ADMSetRndEnable(CHANS(ptPageAddr->m_bChan),1,FUNC_DISABLE);//随机开关
	HAL_ADMComSelEccMode(CHANS(ptPageAddr->m_bChan),ECC_MODE_NONE);//Ecc开关
	HAL_ADMSetUdEnable(CHANS(ptPageAddr->m_bChan),FUNC_ENABLE);//UserData 开关
#endif



	FDL_Log("C2 Write PageNum =%x",pCtrl->dPageCnt);

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
    tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
    tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

	//tTaskCtrl.dEvenRaidAddr = MEM_COPYBACK_SRC_ADDR;//偶数
	//tTaskCtrl.dOddRaidAddr 	= MEM_COPYBACK_SRC_ADDR + SECTOR_SIZE*DSECT_OF_PAGE + 16*DATA_CLST_OF_PAGE;
	
	U08MemSet((U08 *)MEM_COPYBACK_SRC_ADDR,0, (SECTOR_SIZE*DSECT_OF_PAGE + 16*DATA_CLST_OF_PAGE)*2);


    FDL_WriteBuffToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR,ptPageAddr,pCtrl->dPageCnt,MEM_UD_BUFF_ADDR,pwSeedIdxBuff);
    

    FDL_WaitTaskDone();
    FDL_CheckMultiLunReady();

#if 0
	HAL_ADMSetRndEnable(CHANS(ptPageAddr->m_bChan),1,FUNC_ENABLE);//随机开关
	HAL_ADMComSelEccMode(CHANS(ptPageAddr->m_bChan),ECC_MODE_LDPC);//Ecc开关
	HAL_ADMSetUdEnable(CHANS(ptPageAddr->m_bChan),FUNC_ENABLE);//UserData 开关
#endif
    


    FDL_Log("C2 Write End");

    FDL_Test_Analyse_Exception(tTaskCtrl.dWriteExBuffAddr,WRITE_TYPE);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
#else
    FDL_Test_8NDB_WriteTlcBlk(0,0,6);
    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host

#endif
}

void FDL_CMD_C3_ReadClstToBuff(void)
{
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;

    U08 bMaskEn = pCtrl->dMaskEn;//0:无Mask信息，1:有Mask信息
    U08 bSeedIdxEn = pCtrl->dSeedBuffEn;//0:无SeedIdx信息，1:有SeedIdx信息
    U16 *pwSeedIdxBuff = NULL;

    if(bSeedIdxEn)
    {
        FDL_Log("SeedIdxEn");
        pwSeedIdxBuff = (U16 *)MEM_SEED_BUFF_ADDR;
    }

	HostCmdResponse();
	
	FDL_Log("C3 Read ClstNum = %d",pCtrl->dClstCnt);
	U08 bLoopCnt = pCtrl->bBuff[0];
	//FDL_Log("LOOPCNT = %x",bLoopCnt);
	//打印物理地址
	Fdb_PrintfPhyAddress(tClstAddr, pCtrl->dClstCnt);
	

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

#if 0
	HAL_ADMSetRndEnable(CHANS(tClstAddr->m_bChan),1,FUNC_DISABLE);//随机开关
	HAL_ADMComSelEccMode(CHANS(tClstAddr->m_bChan),ECC_MODE_NONE);//Ecc开关
	HAL_ADMSetUdEnable(CHANS(tClstAddr->m_bChan),FUNC_ENABLE);//UserData 开关
#endif

    if(bMaskEn)//有MASK信息
    {
        FDL_Log("MASK Read");
        FDL_ReadClstToBuffWithMsk(&tTaskCtrl,MEM_DMA_BUFF_ADDR,tClstAddr,pCtrl->dClstCnt ,\
                                 (MEM_UD_BUFF_ADDR),(U8 *)(MEM_MSK_BUFF_ADDR),pwSeedIdxBuff);
    }
    else
    {
		
    	FDL_ReadClstToBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR,tClstAddr,pCtrl->dClstCnt ,\
                                 (MEM_UD_BUFF_ADDR),pwSeedIdxBuff);
    }


    FDL_WaitTaskDone();


    FDL_Log("C3 Read End");


	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}
*/

void FDL_CMD_C5_WriteLittleData(void)
{

	FDL_Log("C5 little data Write");
	HostCmdResponse();
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;

	U16 wDataSize = pCmd->dOpByteCnt;
	U8  bNumOfCopies = pCmd->wOpSctCnt;
	FDL_Log("C5 little data Write 0x%x,%d",wDataSize,bNumOfCopies);

    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

    FDL_WriteLittleDataToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR, tPageAddr, wDataSize, bNumOfCopies,NULL);
    FDL_WaitTaskDone();

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
    FDL_Log("C5 little data Write End");


}

void FDL_CMD_C6_ReadLittleData(void)
{
	FDL_Log("C6 little data Read");
	HostCmdResponse();
	
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	U16 wDataSize = pCmd->dOpByteCnt;
	U8  bNumOfCopies = pCmd->wOpSctCnt;
	U8  bRetryMode = pCmd->wOpSctCnt>>8;
    FDL_Log("C6 little data Read End 0x%x,%d",wDataSize,bNumOfCopies);

	T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
	
	FDL_TASK_CTRL tTaskCtrl = {0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

	U32 dBuffAddr = MEM_DMA_BUFF_ADDR;
	tFtlTask.bArgsArr[1]   = bRetryMode;
	
	FDL_ReadLittleDataToBuffer(&tTaskCtrl,dBuffAddr,tPageAddr,wDataSize,0,NULL);
	FDL_WaitTaskDone();

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
    FDL_Log("C6 little data Read End");

}

void FDL_CMD_C7_JointWritePages(void)
{
    HostCmdResponse();
	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 128*512);

    T_FLASH_PHY_ADDR  *ptPageAddr = (T_FLASH_PHY_ADDR  *)MEM_NAND_ADDR_BUFF_ADDR;

    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

	FDL_Log("C7 New Joint Write totalsec=%x\r\n",pCtrl->dSecCnt);

	U16 bPageCnt = pCtrl->dSecCnt/SECT_OF_PAGE;

    SG_ADM->rBUF_SIZE = pCtrl->dSize;
    //FDL_Log("rBUF_SIZE=%x",SG_ADM->rBUF_SIZE.bits.wVAL);

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

    FDL_JointWritePages(&tTaskCtrl,MEM_DMA_BUFF_ADDR,0,pCtrl->dSecCnt,ptPageAddr,bPageCnt,MEM_UD_BUFF_ADDR,NULL);
    FDL_WaitTaskDone();

    FDL_Test_Analyse_Exception(tTaskCtrl.dWriteExBuffAddr,2);

}



void FDL_CMD_C8_JointReadClsts(void)
{
	HostCmdResponse();

	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x46, 128*512);

	T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

	FDL_Log("C8 Joint Read totalsec=%x\r\n",pCtrl->dSecCnt);

	U16 bClstCnt = pCtrl->dSecCnt/SECT_OF_DATA_CLST;

	SG_ADM->rBUF_SIZE = pCtrl->dSize;
	//FDL_Log("rBUF_SIZE=%x",SG_ADM->rBUF_SIZE.bits.wVAL);

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
	
	FDL_JointReadClsts(&tTaskCtrl,MEM_DMA_BUFF_ADDR,ptClstAddr,0,(bClstCnt*SECT_OF_DATA_CLST),bClstCnt,(MEM_UD_BUFF_ADDR + 512)/*,NULL*/,NULL);
	FDL_WaitTaskDone();
	FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr,1);

}

/*
//void FDL_CMD_C9_JointReadClstsWithMask(void)
void FDL_CMD_C9_PreReadTableToBuff4K(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;

	U08 bSeedIdxEn = pCtrl->dSeedBuffEn;//0:无SeedIdx信息，1:有SeedIdx信息
	U16 *pwSeedIdxBuff = NULL;

	if(bSeedIdxEn)
	{
		FDL_Log("SeedIdxEn");
		pwSeedIdxBuff = (U16 *)MEM_SEED_BUFF_ADDR;
	}

	HostCmdResponse();
	
	FDL_Log("C9 4K Read ClstNum = %d",pCtrl->dClstCnt);
	U08 bLoopCnt = pCtrl->bBuff[0];
	FDL_Log("LOOPCNT = %x",bLoopCnt);
	//打印物理地址
	//Fdb_PrintfPhyAddress(tClstAddr, pCtrl->dClstCnt);

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

	
	U16 i = 0;
	U16 INDEX[128];
	U16 wIndex = 0;
	
	for(i = 0; i <	pCtrl->dClstCnt; i++)
	{
		wIndex = i * SECT_OF_TBLE_CLST;
		INDEX[i] = wIndex;
	}
	U08 bLastChanNum = 0;
	
	U16 Seed = rand();
	//Seed = 58421;
	FDL_Log("SEED = %d",Seed);
	srand(Seed);
	U08 bCtlsLen = 0;
	U16 * bBuffIndex;
	T_FLASH_PHY_ADDR  *pClstAddr;		
	//U16 * bLastBuffIndex;
	//U16 wLastCtlsLen = 0;
	U16 wCtlsOffts = 0;
	//U08 bCheckHoldLast = 0;

	for(i = 0; i <	pCtrl->dClstCnt; i++)
	{
		bCtlsLen = ((rand() % bLoopCnt) + 1);
		//bCheckHoldLast = (((rand() % 13) == 0)?1:0);
		//FDL_Log("%d",bCtlsLen);
		bBuffIndex = INDEX;
		pClstAddr = tClstAddr + wCtlsOffts;
		
		if(i == 0)
		{		
			bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,
													MEM_DMA_BUFF_ADDR,
													bBuffIndex,
													pClstAddr,
													bCtlsLen,
													MEM_UD_BUFF_ADDR,
													pwSeedIdxBuff,
													bLastChanNum,
													0);
		}
		else
		{
			bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,
													MEM_DMA_BUFF_ADDR,
													(bBuffIndex + wCtlsOffts),
													pClstAddr,
													bCtlsLen,
													(MEM_UD_BUFF_ADDR + UD_LEN_OF_TBLE_CLST * wCtlsOffts),
													pwSeedIdxBuff,
													bLastChanNum,
													0);
		}	
				
		wCtlsOffts += bCtlsLen; 

		if(((pCtrl->dClstCnt - wCtlsOffts) < (bLoopCnt+1)))
		{
			if (((pCtrl->dClstCnt - wCtlsOffts) != 0))
			{
				bCtlsLen = pCtrl->dClstCnt - wCtlsOffts;
				FDL_Log("%d",bCtlsLen);
				bBuffIndex = INDEX;
				pClstAddr = tClstAddr + wCtlsOffts;
				bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,
														MEM_DMA_BUFF_ADDR, 
														(bBuffIndex + wCtlsOffts),
														pClstAddr, 
														bCtlsLen,
														(MEM_UD_BUFF_ADDR + UD_LEN_OF_TBLE_CLST * wCtlsOffts), 
														pwSeedIdxBuff, 
														bLastChanNum,
														1);
			}
			break;
		}
	}
	

	FDL_WaitTaskDone();

	FDL_Log("C9 4K Read End");
	FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr,READ_TYPE);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}
*/

void FDL_CMD_CA_WriteDiscreteBuffToFlash(void)
{
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;

    U32 i = 0,j = 0;
    U32 dTotalSctSize = pCtrl->dSecCnt;//扇区个数
    U16 wPageCnt = dTotalSctSize / SECT_OF_PAGE;//页个数
    U16 wClstCnt = dTotalSctSize / SECT_OF_DATA_CLST;
    U16 *pwSeedIndex = NULL;
    U16 wBuffIdx[32];//每段传输位置相对base地址的Index偏移
    HostCmdResponse();
    tPageAddr = (T_FLASH_PHY_ADDR *)(MEM_NAND_ADDR_BUFF_ADDR + sizeof(T_FLASH_PHY_ADDR)*wClstCnt);//偏移到Page地址

    for(i = 0;i < dTotalSctSize;i += 8 ,j++)//生成BuffIndex,和私有命令B0的对应
    {
      wBuffIdx[j] = i;
    }
    FDL_Log("CA discrete Buffer write");
	//打印物理地址
//	Fdb_PrintfPhyAddress(tPageAddr, wPageCnt);
//	Fdb_PrintfMemBySector(MEM_DMA_BUFF_ADDR,wPageCnt*32);

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
    FDL_WriteDiscreteBuffToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR,wBuffIdx,wClstCnt,
                        tPageAddr,wPageCnt,MEM_UD_BUFF_ADDR,
                        pwSeedIndex,JOINT_22);
    FDL_WaitTaskDone();
	
	//打印水位
	Fdb_PrintfBuffStatus();
    FDL_Log("Discrete Buff Wirte To Flash TaskDone");

	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
}


void FDL_CMD_CB_ReadClstToDiscreteBuff(void)
{
#if 0
    T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

    U16 *pwSeedIndex = NULL;
    U16 wBuffIndex = pCtrl->bBuff[0];//当前段BuffIdx
    U16 wSecCnt = pCtrl->dSecCnt;//返回到Host扇区个数
    U08 bMaskBuff[2];
    U16 wBuffIdx[2];
    //U32 i = 0;
    HostCmdResponse();
    FDL_Log("CB discrete Buffer read");
    tClstAddr += pCtrl->bBuff[2];//偏移到当前簇地址
    bMaskBuff[0] = HAL_BFCGetBuffStatus(wBuffIndex)&0xFF;
    bMaskBuff[1] = (HAL_BFCGetBuffStatus(wBuffIndex)>>8)&0xFF;
    wBuffIdx[0] = wBuffIndex;
    wBuffIdx[1] = wBuffIndex + 8;

	FDL_Log("Mask:0x%02X--0x%02X",bMaskBuff[0],bMaskBuff[1]);
	//打印物理地址
//   	Fdb_PrintfPhyAddress(tClstAddr,2);
   	
	FDL_TASK_CTRL tTaskCtrl={0};
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
	FDL_ReadClstToDiscreteBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR, wBuffIdx, tClstAddr,
					2, MEM_UD1_BUFF_ADDR, bMaskBuff, pwSeedIndex, JOINT_22);
	FDL_WaitTaskDone();	
	
	//打印水位
	Fdb_PrintfBuffStatus();

//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR + SECTOR_SIZE * wBuffIdx[0], wSecCnt, JOINT_NONE);//send n sector status information to host
	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
#else
	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

    U16 *pwSeedIndex = NULL;
    U16 *wBuffIndex = (U16 *)&pCtrl->bBuff[0];//当前段BuffIdx
    U16 wClstCnt = pCtrl->dClstCnt;//返回到Host扇区个数
    U08 bMaskBuff[16];

    U32 i = 0;
    HostCmdResponse();
    FDL_Log("CB discrete Buffer read");
    for(i = 0;i < 16;i++)
    {
		bMaskBuff[i] = HAL_BFCGetBuffStatus(*(wBuffIndex + i))&0xFF;
    }

	FDL_Log("Mask:0x%02X--0x%02X",bMaskBuff[0],bMaskBuff[1]);
	//打印物理地址
//   	Fdb_PrintfPhyAddress(tClstAddr,2);
   	
	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
	FDL_ReadClstToDiscreteBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR, wBuffIndex, tClstAddr,
					wClstCnt, MEM_UD1_BUFF_ADDR, bMaskBuff, pwSeedIndex, JOINT_22);
	FDL_WaitTaskDone();	
	
	//打印水位
	Fdb_PrintfBuffStatus();

//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR + SECTOR_SIZE * wBuffIdx[0], wSecCnt, JOINT_NONE);//send n sector status information to host
	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host

#endif
}

void FDL_CMD_CD_JointDiscreteWirteToBuff(void)
{
#if 0
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
    U16 wBuffIndex = pCtrl->bBuff[0];//当前段BuffIdx
    U16 wSecCnt = pCtrl->bBuff[1];//当前段SecCnt
    FDL_Log("CD Discrete Write Buff  wBuffIndex=%d,wSecCnt=%d",wBuffIndex,wSecCnt);
    FDL_JointWriteHostToDiscreteBuff(MEM_DMA_BUFF_ADDR, (U16 *)&wBuffIndex, (U16 *)&wSecCnt, 1, JOINT_12);

	//打印水位
	Fdb_PrintfBuffStatus();
#else
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U16 *wBuffIndex = (U16 *)&pCtrl->bBuff[0];//当前段BuffIdx
	U16 *wSecCntOnce = (U16 *)&pCtrl->bBuff[16*2];//当前段SecCnt
	U16 wSecCnt = pCtrl->dSize;
	FDL_Log("CD Discrete Write Buff  wBuffIndex=%d,wSecCnt=%d",wBuffIndex,wSecCnt);
	FDL_JointWriteHostToDiscreteBuff(MEM_DMA_BUFF_ADDR, wBuffIndex, wSecCntOnce, wSecCnt, JOINT_12);

	//打印水位
	Fdb_PrintfBuffStatus();

#endif
}

void FDL_Test_ReadFlashID(void)
{
	U8 i;
	for(i = 0;i<MAX_CHAN_COUNT;i++)
	{
		MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0, 8);
		FDL_ReadFlashID(MEM_DMA_BUFF_ADDR,i, 0);
	}
}

void FDL_CMD_CE_FlashReadID(void)
{
	FDL_Log("CE Read ID");
	HostCmdResponse();
	//FDL_Test_ReadFlashID();
    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
	FDL_Log("CE End");
}


void FDL_CMD_D0_ReadClstToBuffDebug(void)
{
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

	
    T_ADDR_INFO tAddr1st;
	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;

	U08 i 			= 0;
	U08 bOpCMD 		= 0;
	U08 bChanPhy 	= CHANS(tClstAddr->m_bChan);
	U08 bChipPhy 	= CHIPES(bChanPhy,tClstAddr->m_bLunOfChan);
	U16 wColAddr 	= 0;
	U08 bOrderCnt  	= pCtrl->dClstCnt;
	U08 bLunOfChan 	= tClstAddr->m_bLunOfChan;
	U32 dDAMBuffAddr= (U32)MEM_DMA_BUFF_ADDR;
	U32 dUDBuffAddr = (U32)MEM_UD_BUFF_ADDR;
	U32 dDmaInfoAddr= (U32)MEM_DMA2_INFO_ADDR;
	U32 dSectorCnt 	= pCtrl->dSecCnt;
	U32 dRaidBuff = MEM_COPYBACK_SRC_ADDR;
//	U08MemSet((U08 *)dRaidBuff,0xAA,dSectorCnt*SECTOR_SIZE + 64);
	U32 dBuffIndex = 0;
	
	HostCmdResponse();

	Fdb_PrintfPhyAddress(tClstAddr, pCtrl->dClstCnt);


//	FDL_Log("bOpCMD=0x%x,bOpCMD=0x%X",bOpCMD,bOpCMD);
	for(i = 0;i < bOrderCnt;i++)
	{
		bOpCMD = (tClstAddr + i)->m_bClstOffs;
		(tClstAddr + i)->m_bClstOffs = 0;
		
		Fdl_ConvertAddrInfo(&tAddr1st,(U32)(tClstAddr + i),!TYPE_ERASE_BLOCKS);
		HAL_CQSetUd(bChanPhy,dUDBuffAddr,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1); //set ud,length and AttachSize set once is ok
	    HAL_CQCheckHold(1,JOINT_NONE,bChanPhy, 0, SECT_OF_PAGE,FALSE);
		HAL_CQChipSelect(bChanPhy,bChipPhy);	

		switch(bOpCMD)
	    {
		    case 0x30:		
				Fdl_CQPreReadMultiPln(bChanPhy,bChipPhy,tAddr1st.dRow,FL_CMD_READ2);
				HAL_CQCheckIOStatus(bChanPhy, bChipPhy, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		        break;
		    case 0x31:		
				Fdl_CQPreReadMultiPln(bChanPhy,bChipPhy,tAddr1st.dRow,FL_CMD_CACHE_REDA);
				HAL_CQCheckIOStatus(bChanPhy, bChipPhy, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		        break;
			case 0x3F:			
				HAL_CQSendFlashCmd(bChanPhy,FL_CMD_CACHE_REDA_FINAL);			
				HAL_CQCheckIOStatus(bChanPhy, bChipPhy, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		        break;
			case 0x04:	
//				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1);
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG2);				
				HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS,FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);/*mark*/
		        break;
		    case 0x05:
				HAL_CQCheckHold(DMA2_WR_TO_FLASH,tAddr1st.bChan,JOINT_NONE,0,0,0);
				HAL_CQSetRaidBaseAddr(bChanPhy,dRaidBuff - 0x460000,1);
				HAL_CQStartDMA2Write(bChanPhy, dDAMBuffAddr, DMA2_WR_TO_FLASH, dBuffIndex, 0,dSectorCnt,JOINT_NONE, dDmaInfoAddr);
				HAL_CQWaitDone(bChanPhy);
				dBuffIndex += dSectorCnt;
		        break;

			case 0x06:
				

//				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1);
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1_MP);
				
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG2);				
				HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS,FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);/*mark*/
//				Fdl_GetRaidPage(bChanPhy,dDAMBuffAddr,dUDBuffAddr,dRaidBuff);
		        break;

			/*CQ原子操作Demo*/
			case 0x11:/*擦除块操作*/
				HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS,FIO_RB_READY_IO, FIO_PF_PASS_SLC_ERASE);/*mark*/
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_SLC_MODE);
				HAL_CQSendFlashCmd(bChanPhy,FL_CMD_BLOCK_ERASE1);               
    			HAL_CQSendRowAddr(bChanPhy, tAddr1st.dRow);
				HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_BLOCK_ERASE2);
				HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS,FIO_RB_READY_IO, FIO_PF_PASS_SLC_ERASE);/*mark*/
				HAL_CQWaitDone(bChanPhy);
		        break;
				
			case 0x12:/*Write*/	
#if 1
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_SLC_MODE);
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1);
				HAL_CQSendColAddr(bChanPhy, wColAddr);
				HAL_CQSendRowAddr(bChanPhy,tAddr1st.dRow);
				HAL_CQStartDMA2(bChanPhy, dDAMBuffAddr, DMA2_WR_TO_FLASH, 0, 0,dSectorCnt,JOINT_NONE, dDmaInfoAddr);
//				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_CACHE_PROG2);	
//				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG2);	
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG2_MP);	
				SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;	
				HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS,FIO_RB_READY_IO, FIO_PF_PASS_NULL);/*mark*/

				HAL_CQWaitDone(bChanPhy);
#else
#endif
				break;
				
			case 0x13:/*Read*/
				MemorySet((U08 *)dDAMBuffAddr,0xAA,dSectorCnt*512);
				
//				tAddr1st.dRow = GET_ROW_ADDR_SLC(0,GET_BLOCK_NUM(tAddr1st.dRow) + 1);

#if 1
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_SLC_MODE);
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_READ1);
				HAL_CQSendColAddr(bChanPhy, wColAddr);
				HAL_CQSendRowAddr(bChanPhy,tAddr1st.dRow);
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_READ2);
				HAL_CQCheckIOStatus(bChanPhy, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
				
				HAL_CQSendFlashCmd(bChanPhy,FL_CMD_READ1);
				HAL_CQSendColAddr(bChanPhy, wColAddr);
				HAL_CQSendRowAddr(bChanPhy,tAddr1st.dRow);
			    HAL_CQSendFlashCmd(bChanPhy,FL_CMD_RANDOM_DATA_OUTPUT1);
			    HAL_CQSendColAddr(bChanPhy,wColAddr);
			    HAL_CQSendFlashCmd(bChanPhy,FL_CMD_RANDOM_DATA_OUTPUT2);
				HAL_CQStartDMA2(bChanPhy,
								dDAMBuffAddr,
								DMA2_RD_TO_BUFF,
								0,
								0,
								dSectorCnt,
								JOINT_NONE,
								dDmaInfoAddr);
				HAL_CQWaitDone(bChanPhy);
#else
#endif
				break;
				

		    default:
		        break;
    	}

		HAL_CQWaitDone(bChanPhy);
	}
	
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}




void FDL_CMD_CF_DiscreteBuffReadNoJoint(void)
{
	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

    U32 i = 0;
	U16 wSecCnt			= pCtrl->dSecCnt;//返回到Host扇区个数
	U16 wClstCnt 		= pCtrl->dClstCnt;//读取簇个数
	U08 bSecOfClst		= pCtrl->bBuff[1];//
    U16 *pwSeedIndex 	= NULL;
	U08 *pbMaskBuff 	= (U08 *)MEM_MSK_BUFF_ADDR;
	U16 *pwBuffIndex 	= (U16 *)((U08 *)MEM_MSK_BUFF_ADDR + SECTOR_SIZE);//当前段BuffIdx
	
	if(tClstAddr->m_bDataType)//表格簇不会有mask的情况
	{ 
		FDL_Log("###########No Mask");
    	pbMaskBuff = NULL;
	}

    HostCmdResponse();
    FDL_Log("CF discrete Buffer read-%d",wClstCnt);

#if 0
	HAL_ADMSetRndEnable(CHANS(tClstAddr->m_bChan),1,/*FUNC_DISABLE*/FUNC_ENABLE);/*随机开关*/
	HAL_ADMComSelEccMode(CHANS(tClstAddr->m_bChan),/*ECC_MODE_NONE*/ECC_MODE_LDPC);/*Ecc开关*/
	HAL_ADMSetUdEnable(CHANS(tClstAddr->m_bChan),FUNC_DISABLE/*FUNC_ENABLE*/);/*UserData 开关*/	
	SECT_OF_PAGE = 32;
#endif



	//打印物理地址
//	Fdb_PrintfPhyAddress(tClstAddr,wClstCnt);

	FDL_TASK_CTRL tTaskCtrl		= {0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr 	= MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize 	= MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl 	= READ_FAIL_ALL_RETRY;
	
	 FDL_ReadClstToDiscreteBuff(&tTaskCtrl,
								MEM_DMA_BUFF_ADDR,
								pwBuffIndex,
								tClstAddr,
								wClstCnt,
								MEM_UD_BUFF_ADDR_EXT,
								pbMaskBuff,
								pwSeedIndex,
								0);

	FDL_WaitTaskDone();	
	
  for (i = 0; i < wClstCnt; i++)
  {
	  HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR, pwBuffIndex[i], bSecOfClst,DMA1_RD_TO_HOST, JOINT_NONE);
  }
	//HAL_MCStartDMA1(MEM_COPYBACK_SRC_ADDR, pwBuffIndex[i], wSecCnt, DMA1_RD_TO_HOST, JOINT_NONE);
	HAL_MCWaitDMA1Done();

}

U16 wCBBuffSize[3] = {256,384,512};
static U08 bCBBuffIdx = 0;
void FDL_CMD_D8_CopyBackProg(void)
{
	FDL_TASK_CTRL tTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U16 wClst = pCtrl->dClstCnt;
	U16 wPageCnt = pCtrl->dPageCnt;
	U08 bDstUdSameAsSrc = pCtrl->bBuff[0];
	U32 dSrcUserBuff;
	U32 dDesUserBuff;

	HostCmdResponse();

	dSrcUserBuff = MEM_COPYBACK_UD_ADDR;
	
	if(bDstUdSameAsSrc == 0)//源Ud与目的Ud不同
	{
		dDesUserBuff = MEM_COPYBACK_UD_ADDR+MEM_COPYBACK_UD_SIZE/2;
	}
	else//源Ud与目的Ud相同
	{
		dDesUserBuff = dSrcUserBuff;
	}
	
	FDL_Log("Cmd :D8 CopyBackPrg Start...Src ClstLen=%d,Dst PageCnt=%d",wClst,wPageCnt);
#if 0//打印地址信息
	T_FLASH_PHY_ADDR *ptSrcClst = (T_FLASH_PHY_ADDR *)MEM_COPYBACK_SRC_ADDR;
	T_FLASH_PHY_ADDR *ptPageAddr = (T_FLASH_PHY_ADDR *)MEM_COPYBACK_DST_ADDR;
	U32 wOffset  = 0;
	FDL_Log("Src pAddr000:| DataType | Chan | LunOfChan | ClstOffs | RowAddr  |");
	for(wOffset = 0;wOffset < wClst;wOffset++)
	{
		FDL_Log("Src pAddr%03d:|....%02d....|..%02d..|.....%02d....|....%02d....|.0x%06X.|",wOffset,\
					ptSrcClst[wOffset].m_bDataType,\
					ptSrcClst[wOffset].m_bChan,\
					ptSrcClst[wOffset].m_bLunOfChan,\
					ptSrcClst[wOffset].m_bClstOffs,\
					ptSrcClst[wOffset].m_dRowAddr);
	}
	FDL_Log("Dst pAddr000:| DataType | Chan | LunOfChan | ClstOffs | RowAddr  |");
	for(wOffset = 0;wOffset < wPageCnt;wOffset++)
	{
		FDL_Log("Dst pAddr%03d:|....%02d....|..%02d..|.....%02d....|....%02d....|.0x%06X.|",wOffset,\
					ptPageAddr[wOffset].m_bDataType,\
					ptPageAddr[wOffset].m_bChan,\
					ptPageAddr[wOffset].m_bLunOfChan,\
					ptPageAddr[wOffset].m_bClstOffs,\
					ptPageAddr[wOffset].m_dRowAddr);
	}
#endif

    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;

	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;


	FDL_CopyBackProg(&tTaskCtrl,(T_FLASH_PHY_ADDR *)MEM_COPYBACK_SRC_ADDR, wClst,
						(T_FLASH_PHY_ADDR const *)MEM_COPYBACK_DST_ADDR, wPageCnt, 
						MEM_DMA_BUFF_ADDR,wCBBuffSize[bCBBuffIdx%3],
						dSrcUserBuff,dDesUserBuff,NULL,NULL);
	FDL_WaitTaskDone();
	bCBBuffIdx++;
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
	FDL_Log("CopyBackPrg Test End");
}

void FDL_CMD_D9_MultiLunExdieCopyBackPrg(void)
{
	FDL_TASK_CTRL tTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bMultiPlaneEn = pCtrl->bBuff[0];
	U08 bLunPairCnt   = pCtrl->bBuff[1];
	U32 dSrcAddr = MEM_COPYBACK_SRC_ADDR;
	U32 dDstAddr = MEM_COPYBACK_SRC_ADDR+((sizeof(T_IN_COPY_SRC_INFO_OF_LUN)*bLunPairCnt + 7) & (~(U32)0x07));

	HostCmdResponse();

	FDL_Log("Cmd :D9 ExdieCopyBack Start..");

#if 0//打印通道信息
	FDL_Log("dSrcAddr=%x,dDstAddr=%x",dSrcAddr,dDstAddr);

	U08 bK;
	U16 wK;
	U32 *pdRow;
	U08 *pbClst;
	T_SRC_INFO_OF_LUN *ptSrcInf = (T_SRC_INFO_OF_LUN *)dSrcAddr;
	T_DST_INFO_OF_LUN *ptDstInf = (T_DST_INFO_OF_LUN *)dDstAddr;
	for(bK=0;bK<bLunPairCnt;bK++)
	{
		FDL_Log("Chan%d Lun%d To Chan%d Lun%d wClstLen=%d,wPageCnt=%d,SrcUd=0x%x",\
				ptSrcInf[bK].bChan,ptSrcInf[bK].bLunOfChan,\
			    ptDstInf[bK].bChan ,ptDstInf[bK].bLunOfChan,\
			    ptSrcInf[bK].wClstLen,ptDstInf[bK].wPageCnt,\
			    ptSrcInf[bK].dUdBuffAddr);
	#if 0//打印详细地址信息
		pdRow = ptSrcInf[bK].pdRowAddr;
		pbClst = ptSrcInf[bK].pbClstOffs;
		
		for(wK=0;wK<ptSrcInf[bK].wClstLen;wK++)
		{
			FDL_Log("Src dRow=0x%x,bClst=%d",pdRow[wK],pbClst[wK]);
		}
		
		pdRow = ptDstInf[bK].pdRowAddr;
		for(wK=0;wK<ptDstInf[bK].wPageCnt;wK++)
		{
			FDL_Log("Dst dRow=0x%x",pdRow[wK]);
		}
	#endif
	}
#endif

    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;

	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;


	FDL_MultiLunExdieCopyBackPrg(&tTaskCtrl,(T_SRC_INFO_OF_LUN *)dSrcAddr,
		(T_DST_INFO_OF_LUN *)dDstAddr,MEM_DMA_BUFF_ADDR,wCBBuffSize[bCBBuffIdx%3],
		bMultiPlaneEn,bLunPairCnt,NULL,NULL);
	FDL_WaitTaskDone();
	bCBBuffIdx++;
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host

	FDL_Log("Cmd :D9 ExdieCopyBack End");
}

void FDL_CMD_DA_MultiLunIndieCopyBackPrg(void)
{
	FDL_TASK_CTRL tTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bMultiPlaneEn = pCtrl->bBuff[0];
	U08 bLunPairCnt   = pCtrl->bBuff[1];
	U32 dSrcAddr = MEM_COPYBACK_SRC_ADDR;
	U32 dDstAddr = MEM_COPYBACK_SRC_ADDR+((sizeof(T_IN_COPY_SRC_INFO_OF_LUN)*bLunPairCnt + 7) & (~(U32)0x07));

	HostCmdResponse();

	FDL_Log("Cmd :DA IndieCopyBack Start..");
	
#if 0
	FDL_Log("dSrcAddr=%x,dDstAddr=%x",dSrcAddr,dDstAddr);

	U08 bK;
	U16 wK;
	U32 *pdRow;
	U08 *pbClst;
	
	T_SRC_INFO_OF_LUN *ptSrcInf = (T_SRC_INFO_OF_LUN *)dSrcAddr;
	T_DST_INFO_OF_LUN *ptDstInf = (T_DST_INFO_OF_LUN *)dDstAddr;
	for(bK=0;bK<bLunPairCnt;bK++)
	{
		FDL_Log("Chan%d Lun%d -> Chan%d Lun%d wClstLen=%d,wPageCnt=%d SrcUd=0x%x",\
				ptSrcInf[bK].bChan,ptSrcInf[bK].bLunOfChan,\
				ptDstInf[bK].bChan,ptDstInf[bK].bLunOfChan,\
				ptSrcInf[bK].wClstLen,ptDstInf[bK].wPageCnt,\
				ptSrcInf[bK].dUdBuffAddr);
#if 0
		pdRow = ptSrcInf[bK].pdRowAddr;
		pbClst = ptSrcInf[bK].pbClstOffs;
		
		for(wK=0;wK<ptSrcInf[bK].wClstLen;wK++)
		{
			FDL_Log("Src dRow=0x%x,bClst=%d",pdRow[wK],pbClst[wK]);
		}
		
		pdRow = ptDstInf[bK].pdRowAddr;
		for(wK=0;wK<ptDstInf[bK].wPageCnt;wK++)
		{
			FDL_Log("Dst dRow=0x%x",pdRow[wK]);
		}
#endif
	}
#endif
		

		tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
		tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
		tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
		tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
		
		tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
		tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;


		FDL_MultiLunIndieCopyBackPrg(&tTaskCtrl,(T_SRC_INFO_OF_LUN *)dSrcAddr,
			(T_DST_INFO_OF_LUN *)dDstAddr,MEM_DMA_BUFF_ADDR,wCBBuffSize[bCBBuffIdx%3],
			bMultiPlaneEn,bLunPairCnt,NULL,NULL);
		FDL_WaitTaskDone();
		bCBBuffIdx++;
		HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
	
		FDL_Log("Cmd :DA IndieCopyBack End");
}


void FDL_CMD_DB_OrderPrgBuffToFlash(void)
{
	FDL_TASK_CTRL ptTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bCnt = pCtrl->bBuff[0];
	
	U32 dPlaneParamAddr = MEM_NAND_ADDR_BUFF_ADDR;
	U32 dOrderPrgUnitAddr = MEM_NAND_ADDR_BUFF_ADDR+((sizeof(T_PLANE_PARAM) + 7) & (~(U32)0x07));
	
	T_PLANE_PARAM *ptPlaneParam = (T_PLANE_PARAM *)dPlaneParamAddr;
	T_FLASH_PROG_ORDER_UNIT *ptOrderPrgUnit = (T_FLASH_PROG_ORDER_UNIT *)dOrderPrgUnitAddr;

	HostCmdResponse();

    FDL_Log("CMD :DB OrderPrg Start..");
	
#if 0//打印物理地址
	U16 wK;
	FDL_Log("Cnt=%d",bCnt);
	FDL_Log("HeadUnitCnt=%d,HeadPln=%d",ptPlaneParam->bHeadUnitCnt,ptPlaneParam->bHeadUnitOfMultiPlane);
	FDL_Log("MidUnitCnt=%d,MidPln=%d",ptPlaneParam->wMidUnitCnt,ptPlaneParam->bMidUnitOfMultiPlane);
	FDL_Log("LastUnitCnt=%d,LastPln=%d",ptPlaneParam->bLastUnitCnt,ptPlaneParam->bLastUnitOfMultiPlane);

	for(wK=0;wK<bCnt;wK++)
	{
		FDL_Log("wK=%d",wK);
		FDL_Log("Chan=%d,Lun=%d,OpPageCnt=%d",ptOrderPrgUnit[wK].bChan,ptOrderPrgUnit[wK].bLunOfChan,ptOrderPrgUnit[wK].bOpPageCnt);
		FDL_Log("Row0=0x%x,Row1=0x%x,Row2=0x%x",ptOrderPrgUnit[wK].dRowAddr[0],ptOrderPrgUnit[wK].dRowAddr[1],ptOrderPrgUnit[wK].dRowAddr[2]);
		FDL_Log("Order=%d,BlkNum=%d,Cycle=%d",ptOrderPrgUnit[wK].wOrderNum,ptOrderPrgUnit[wK].wBlkNum,ptOrderPrgUnit[wK].bPrgCycle);
	}
#endif	
	
	FDL_OrderPrgBuffToFlash(&ptTaskCtrl,MEM_DMA_BUFF_ADDR,ptPlaneParam,ptOrderPrgUnit,MEM_UD1_BUFF_ADDR,bCnt,NULL);
	FDL_WaitTaskDone();	
	
	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
	FDL_Log("CMD :DB OrderPrg End");
}

void FDL_CMD_DC_JointOrderPrgUnit(void)
{
	HostCmdResponse();

    FDL_Log("CMD :DC JointOrder Start..");
	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0x35, 128*512);

	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host
	
	FDL_Log("CMD :DC JointOrder End..");
}

void FDL_CMD_DD_MultiLunExdieCopyBackOrderPrg(void)
{
	FDL_TASK_CTRL tTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bMultiPlaneEn = pCtrl->bBuff[0];
	U08 bLunPairCnt   = pCtrl->bBuff[1];
	U32 dSrcAddr = MEM_COPYBACK_SRC_ADDR;
	U32 dDstAddr = MEM_COPYBACK_SRC_ADDR+((sizeof(T_IN_COPY_SRC_INFO_OF_LUN)*bLunPairCnt + 7) & (~(U32)0x07));
	
	T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo = (T_IN_COPY_SRC_INFO_OF_LUN *)dSrcAddr;
	T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)dDstAddr;

    HostCmdResponse();
	FDL_Log("Cmd :DD ExdieCopyBack OrderPrg Start..");

#if 0//打印通道间信息
	U08 bK;
	U16 wK;
	U32 *pdRow;
	U08 *pbClst;
	U32 *pdUd;
	T_PROG_ORDER_UNIT_IN_BLK *ptDstUnit;

	for(bK = 0;bK<bLunPairCnt;bK++)
	{
		FDL_Log("Chan%d Lun%d -> Chan%d Lun%d wClstLen=%d,wOrderUnitCnt=%x",\
				ptSrcInfo[bK].bChan,ptSrcInfo[bK].bLunOfChan,\
				ptDstInfo[bK].bChan,ptDstInfo[bK].bLunOfChan,\
				ptSrcInfo[bK].wClstLen,ptDstInfo[bK].wOrderUnitCnt);
	#if 0//打印详细信息
		pdRow = ptSrcInfo[bK].pdRowAddr;
		pbClst = ptSrcInfo[bK].pbClstOffs;
		pdUd = ptSrcInfo[bK].pdUdBuffAddr;
		
		for(wK=0;wK<ptSrcInfo[bK].wClstLen;wK++)
		{
			FDL_Log("Src dRow=0x%x,bClst=%d,Ud=0x%x",pdRow[wK],pbClst[wK],ptSrcInfo[bK].pdUdBuffAddr[wK]);
		}
		
		ptDstUnit = ptDstInfo[bK].ptOrderUnit;
		for(wK=0;wK<ptDstInfo[bK].wOrderUnitCnt;wK++)
		{
			FDL_Log("Unit PageCnt=%x,Row[0]=0x%x,Row[1]=0x%x,Row[2]=0x%x",ptDstUnit[wK].bOpPageCnt,\
					ptDstUnit[wK].dRowAddr[0],ptDstUnit[wK].dRowAddr[1],ptDstUnit[wK].dRowAddr[2]);
			FDL_Log("Unit OrderNum=%x,Ud[0]=0x%x,Ud[1]=0x%x,Ud[2]=0x%x",ptDstUnit[wK].wOrderNum,\
					ptDstUnit[wK].dUserDataAddr[0],ptDstUnit[wK].dUserDataAddr[1],ptDstUnit[wK].dUserDataAddr[2]);
		}
	#endif
	}
#endif

    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;

	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;


	FDL_MultiLunExdieCopyBackOrderPrg(&tTaskCtrl,ptSrcInfo,ptDstInfo,MEM_DMA_BUFF_ADDR,\
									wCBBuffSize[bCBBuffIdx%3],bMultiPlaneEn,bLunPairCnt,\
									NULL,NULL);
	FDL_WaitTaskDone();			
	bCBBuffIdx++;
    FDL_Log("Cmd :DD ExdieCopyBack OrderPrg End");

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}

void FDL_CMD_DE_MultiLunIndieCopyBackOrderPrg(void)
{
	FDL_TASK_CTRL tTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bLunPairCnt   = pCtrl->bBuff[1];
	U32 dSrcAddr = MEM_COPYBACK_SRC_ADDR;
	U32 dDstAddr = MEM_COPYBACK_SRC_ADDR+((sizeof(T_IN_COPY_SRC_INFO_OF_LUN)*bLunPairCnt + 7) & (~(U32)0x07));
	
	T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo = (T_IN_COPY_SRC_INFO_OF_LUN *)dSrcAddr;
	T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)dDstAddr;

	HostCmdResponse();
	FDL_Log("Cmd :DE Indie CopyBackOrderPrg Start.");

	//if(Order<0x3)
	{
	#if 0//打印通道间信息
		U08 bK;
		U16 wK;
		U32 *pdRow;
		U08 *pbClst;
		U32 *pdUd;
		T_PROG_ORDER_UNIT_IN_BLK *ptDstUnit;
		for(bK = 0;bK<bLunPairCnt;bK++)
		{
			FDL_Log("Chan%d Lun%d -> Chan%d Lun%d wClstLen=%d,wOrderUnitCnt=%x",\
					ptSrcInfo[bK].bChan,ptSrcInfo[bK].bLunOfChan,\
					ptDstInfo[bK].bChan,ptDstInfo[bK].bLunOfChan,\
					ptSrcInfo[bK].wClstLen,ptDstInfo[bK].wOrderUnitCnt);
	#if 1//打印详细信息
			pdRow = ptSrcInfo[bK].pdRowAddr;
			pbClst = ptSrcInfo[bK].pbClstOffs;
			pdUd = ptSrcInfo[bK].pdUdBuffAddr;

			for(wK=0;wK<ptSrcInfo[bK].wClstLen;wK++)
			{
				FDL_Log("Src dRow=0x%x,bClst=%d,Ud=0x%x",pdRow[wK],pbClst[wK],ptSrcInfo[bK].pdUdBuffAddr[wK]);
			}

			ptDstUnit = ptDstInfo[bK].ptOrderUnit;
			for(wK=0;wK<ptDstInfo[bK].wOrderUnitCnt;wK++)
			{
				FDL_Log("Unit PageCnt=%x,Row[0]=0x%x,Row[1]=0x%x,Row[2]=0x%x",ptDstUnit[wK].bOpPageCnt,\
						ptDstUnit[wK].dRowAddr[0],ptDstUnit[wK].dRowAddr[1],ptDstUnit[wK].dRowAddr[2]);
				FDL_Log("Unit OrderNum=%x,Ud[0]=0x%x,Ud[1]=0x%x,Ud[2]=0x%x",ptDstUnit[wK].wOrderNum,\
						ptDstUnit[wK].dUserDataAddr[0],ptDstUnit[wK].dUserDataAddr[1],ptDstUnit[wK].dUserDataAddr[2]);
			}
	#endif
		}
	#endif
	}

    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;

	tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;


	//if(Order<0x3)
	{
		FDL_MultiLunIndieCopyBackOrderPrg(&tTaskCtrl,ptSrcInfo,ptDstInfo,MEM_DMA_BUFF_ADDR,\
										wCBBuffSize[bCBBuffIdx%3],1,bLunPairCnt,\
										NULL,NULL);
		FDL_WaitTaskDone();
	}
	bCBBuffIdx++;

	FDL_Log("Cmd :DE Indie CopyBackOrderPrg End(0x%X),Order",GET_BLOCK_NUM(ptDstInfo->ptOrderUnit->dRowAddr[0]));

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}

void FDL_CMD_DF_MultiLunInnerCopyBackOrderPrg(void)
{
	FDL_TASK_CTRL ptTaskCtrl={0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bMultiPlaneEn = pCtrl->bBuff[0];
	U08 bLunPairCnt   = pCtrl->bBuff[1];
	U32 dSrcAddr = MEM_COPYBACK_SRC_ADDR;
	U32 dDstAddr = MEM_COPYBACK_SRC_ADDR+((sizeof(T_IN_COPY_SRC_INFO_OF_LUN)*bLunPairCnt + 7) & (~(U32)0x07));
	
	T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo = (T_IN_COPY_SRC_INFO_OF_LUN *)dSrcAddr;
	T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)dDstAddr;

	HostCmdResponse();
	FDL_Log("Cmd :DF Inner CopyBackOrderPrg Start..");

#if 0//打印通道间信息
	U08 bK;
	U16 wK;
	U32 *pdRow;
	U08 *pbClst;
	U32 *pdUd;
	T_PROG_ORDER_UNIT_IN_BLK *ptDstUnit;

	for(bK = 0;bK<bLunPairCnt;bK++)
	{
		FDL_Log("Chan%d Lun%d -> Chan%d Lun%d wClstLen=%d,wOrderUnitCnt=%x",\
				ptSrcInfo[bK].bChan,ptSrcInfo[bK].bLunOfChan,\
				ptDstInfo[bK].bChan,ptDstInfo[bK].bLunOfChan,\
				ptSrcInfo[bK].wClstLen,ptDstInfo[bK].wOrderUnitCnt);
#if 0//打印详细信息
		pdRow = ptSrcInfo[bK].pdRowAddr;
		pbClst = ptSrcInfo[bK].pbClstOffs;
		pdUd = ptSrcInfo[bK].pdUdBuffAddr;
		
		for(wK=0;wK<ptSrcInfo[bK].wClstLen;wK++)
		{
			FDL_Log("Src dRow=0x%x,bClst=%d,Ud=0x%x",pdRow[wK],pbClst[wK],ptSrcInfo[bK].pdUdBuffAddr[wK]);
		}
		
		ptDstUnit = ptDstInfo[bK].ptOrderUnit;
		for(wK=0;wK<ptDstInfo[bK].wOrderUnitCnt;wK++)
		{
			FDL_Log("Unit PageCnt=%x,Row[0]=0x%x,Row[1]=0x%x,Row[2]=0x%x",ptDstUnit[wK].bOpPageCnt,\
					ptDstUnit[wK].dRowAddr[0],ptDstUnit[wK].dRowAddr[1],ptDstUnit[wK].dRowAddr[2]);
			FDL_Log("Unit OrderNum=%x,Ud[0]=0x%x,Ud[1]=0x%x,Ud[2]=0x%x",ptDstUnit[wK].wOrderNum,\
					ptDstUnit[wK].dUserDataAddr[0],ptDstUnit[wK].dUserDataAddr[1],ptDstUnit[wK].dUserDataAddr[2]);
		}
#endif
	}
#endif

	ptTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	ptTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	ptTaskCtrl.bReadFailCtrl = READ_FAIL_STOP;

	ptTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
	ptTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	ptTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

	FDL_MultiLunInnerCopyBackOrderPrg(&ptTaskCtrl,ptSrcInfo,ptDstInfo,MEM_DMA_BUFF_ADDR,\
									MEM_DMA_BUF_SEC,bMultiPlaneEn,bLunPairCnt,\
									NULL,NULL);
	FDL_WaitTaskDone(); 
	FDL_Log("Cmd :DF Inner CopyBackOrderPrg End");

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}

void FDL_CMD_E0_VerifyTLCPage(void)
{
	
#ifdef __VER_TLC_PAGE__
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

	U08 bChip;
	U08 bChan;
	U08 Bluncnt = 0;

	U08 bChanCnt = pCtrl->bBuff[0];
	U08 bChipCnt = pCtrl->bBuff[1];
	U32 dBlkAddr = pCtrl->dBlkAddr;
	U32 dPageStr = pCtrl->dPageAddr;
	U32 dPageCnt = pCtrl->dPageCnt;
	T_BLK_INFO_PER_LUN *ptBlockInfo = (T_BLK_INFO_PER_LUN *)MEM_NAND_ADDR_BUFF_ADDR;

	HostCmdResponse();

	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;	
	tTaskCtrl.bReadFailCtrl   = READ_FAIL_ALL_RETRY;
	
	for(bChan = 0;bChan < bChanCnt;bChan++)
	{
		for(bChip = 0;bChip < bChipCnt;bChip++)
		{
			(ptBlockInfo + bChan*CHIP_OF_CHAN+bChip)->bChan 		= bChan;
			(ptBlockInfo + bChan*CHIP_OF_CHAN+bChip)->bLunOfChan	= bChip;
			(ptBlockInfo + bChan*CHIP_OF_CHAN+bChip)->wBlkStart 	= dBlkAddr&(~(PLANE_OF_LUN - 1));/*起始块(一定是Plane0)*/
			(ptBlockInfo + bChan*CHIP_OF_CHAN+bChip)->wPageOffSet	= dPageStr;/*从第n页开始*/
//			(ptBlockInfo + bChan*CHIP_OF_CHAN+bChip)->wPageCnt		= 256;//dPageCnt*PLANE_OF_LUN;/*读m个Page*/
			(ptBlockInfo + bChan*CHIP_OF_CHAN+bChip)->wPageCnt		= dPageCnt*PLANE_OF_LUN;/*读m个Page*/
			Bluncnt++;
		}
	}

	U08MemSet((U08 *)pFdlParam.dEccInfoBuffAddr,0xFF,512*2);


	FDL_VerifyTLCPage(	&tTaskCtrl,
						ptBlockInfo,
						10,
						Bluncnt, 
						OUT_OF_MEMORY_ADDR,					
						0,
						NULL);

	FDL_WaitTaskDone(); 

    FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr,READ_TYPE);

	Fdl_ClearExceptionState(CHANS(0));

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_TXBENCH_W_BUFF,1,JOINT_NONE);//send 1 sector status information to host
	
#if(NAND_FLASH == MIC_M16) || ((NAND_FLASH == MIC_N18))
	FDL_Log("M16 VerifyTLCPage Test:0x%08X(BlkAdd=0x%X)",pCtrl->dSize,dBlkAddr);
#elif(NAND_FLASH == MIC_B0KB)
	FDL_Log("B0KB VerifyTLCPage Test:0x%08X(BlkAdd=0x%X)",pCtrl->dSize,dBlkAddr);
#endif
#endif
}

void FDL_CMD_E2_ScanFlashDriver(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	T_FDL_SCAN_FLASH_DRIVER *tFlashDriverScan = (T_FDL_SCAN_FLASH_DRIVER *)pCtrl->bBuff;

	HostCmdResponse();
	U08 bRndState 			= tFlashDriverScan->bRndState;
	U08 bEccState 			= tFlashDriverScan->bEccState;
	U08 bWtEn 				= tFlashDriverScan->bWtEn;

	U32 dPadDriver 			= tFlashDriverScan->dPadDriver;
	U32 dInputImpedance 	= tFlashDriverScan->dInputImpedance;
	U32 dSysclkSel	 		= tFlashDriverScan->dSysclkSel;
	U32 dSysclkPeriodSel 	= tFlashDriverScan->dSysclkPeriodSel;
	U32 dTrx2clkPeriodSel 	= tFlashDriverScan->dTrx2clkPeriodSel;
	U32 dRegDelayVal 		= tFlashDriverScan->dRegDelayVal;
	
	U08 Chan 		= 0;
	U08 bChip 		= 0;

	U32 dRowAddr 	= GET_ROW_ADDR_SLC(0x00,tFlashDriverScan->wBlk);
	/*传下来的参数先配置到寄存器*/

	for (Chan = 0; Chan < 4; Chan++)
	{
		if(tFlashDriverScan->bChan[Chan] == 0xFE)
		{
			SG_DMA2_CH(Chan)->rECC_CTRL.bits.ECC_EN = bEccState;/*开关ECC*/
			HAL_ADMSetRndEnable(Chan, 1, bRndState);/*开关rand*/
			HAL_PADSSelCtrl0to3(Chan, dPadDriver);/*配置PAD驱动能力*/
			HAL_PADTSelCtrl(dInputImpedance);/*配置input impedance 	TSEL*/
			HAL_InitChanClock(Chan,dSysclkSel,dSysclkPeriodSel,dTrx2clkPeriodSel);/*配置时钟*/
			HAL_CPMDQS_HandleSetDelay(Chan,dRegDelayVal);/*设置DQS delay值*/

 			if (1)//默认总是打开(SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL == 1) //系统时钟源400MHz,TRX2时钟1分频DQS 200M
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

				SG_FC(Chan)->rNF_CFG.bits.FC_RD_DATAPH_MODE = 1;	//200M 用16bit mode
				SG_FC(Chan)->rNF_CFG.bits.FC_WR_DATAPH_MODE = 1;	//200M 用16bit mode
			}
		 	else if(SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL != 1) //系统时钟源400Mhz，TRX2时钟2分频DQS 100M
			{
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F0_SEL_DIFF = 0;	//主控关差分
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F1_SEL_DIFF = 0;
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F2_SEL_DIFF = 0;
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F3_SEL_DIFF = 0;
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F0RDN = 0;
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F1RDN = 0;
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F2RDN = 0;
				SG_PADCTRL->rPAD_MISC_CTRL.bits.F3RDN = 0;


				SG_FC(Chan)->rNF_CFG.bits.FC_RD_DATAPH_MODE = 0;	//100M 用8bit mode
				SG_FC(Chan)->rNF_CFG.bits.FC_WR_DATAPH_MODE = 0;	//100M 用8bit mode
			}
		}
	}

	for(bChip = 0;bChip<4;bChip++)
	{
		for(Chan = 0; Chan < 4; Chan++)
		{
			if((tFlashDriverScan->bChan[Chan] == 0xFE)&&(tFlashDriverScan->bCe[bChip] == 0xFE))
			{
				if(bWtEn == 0)
				{
					U32 dDMABuffAddr = MEM_DMA_BUFF_ADDR + (Chan*4+bChip)*SECT_OF_PAGE*BYTE_OF_SEC;

					HAL_CQChipSelect(Chan,bChip);
					HAL_CQCheckIOStatus(Chan, bChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_OTHER);

					FL_CMD_CQ_SLC_MODE(Chan);
					SG_FC(Chan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY |0x40;
					HAL_CQSendFlashCmd(Chan, FL_CMD_PAGE_PROG1);
					HAL_CQSend5Addr(Chan, 0, dRowAddr);
					HAL_CQSetRndSeed(Chan, dRowAddr, INVALID_U16);
					HAL_CQSetUd(Chan, MEM_UD_BUFF_ADDR, SECT_OF_DATA_CLST , UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);

					HAL_CQStartDMA2(Chan, dDMABuffAddr, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE, JOINT_NONE, (U32)NULL);
					HAL_CQCheckRealDone(Chan);
				}
			}
		}

		for(Chan = 0; Chan < 4; Chan++)
		{
			HAL_CQWaitDone(Chan);
		}


		for(Chan = 0; Chan < 4; Chan++)
		{
			if((tFlashDriverScan->bChan[Chan] == 0xFE)&&(tFlashDriverScan->bCe[bChip] == 0xFE))
			{
				U32 dDMABuffAddr = MEM_DMA_BUFF_ADDR + (Chan*4+bChip)*SECT_OF_PAGE*BYTE_OF_SEC;
				//读数据
				U08MemSet((U8 *)(dDMABuffAddr),0,0x4000);
		
				HAL_CQChipSelect(Chan,bChip);
				HAL_CQCheckIOStatus(Chan, bChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_OTHER);

				if(bWtEn == 1)
				{
					FL_CMD_CQ_SLC_MODE(Chan);
					SG_FC(Chan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY |0x40;
					HAL_CQSendFlashCmd(Chan, 0x00);
					HAL_CQSend5Addr(Chan, 0, dRowAddr);
					HAL_CQSendFlashCmd(Chan, 0x30);
				}
		
				HAL_CQCheckIOStatus(Chan, bChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_OTHER);
		
				HAL_CQSend5RndAddr(Chan, 0, dRowAddr);
				HAL_CQSetRndSeed(Chan, dRowAddr, INVALID_U16);
				HAL_CQSetUd(Chan, MEM_UD_BUFF_ADDR, SECT_OF_DATA_CLST , UD_LEN_OF_DATA_CLST,DMA2_RD_TO_BUFF);
		
				HAL_CQStartDMA2(Chan, dDMABuffAddr, DMA2_RD_TO_BUFF, 0,0, SECT_OF_PAGE, JOINT_NONE,MEM_DMA2_INFO_ADDR);
				HAL_CQCheckRealDone(Chan);
			}
		}

		for(Chan = 0; Chan < 4; Chan++)
		{
			HAL_CQWaitDone(Chan);
		}
	}

	HostDataTransfer(DMA1_RD_TO_HOST,0x480000,2,JOINT_NONE);//send 1 sector status information to host
}


#define DIR_WT      (1)
#define DIR_RD      (0)
#define MODE_BURST  (0)
#define MODE_PORT   (1)

#pragma GCC optimize("O0")
void FDL_CMD_E3_ScanGCDriver(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	T_FDL_SCAN_GC_DRIVER *tFlashDriverScan = (T_FDL_SCAN_GC_DRIVER *)pCtrl->bBuff;

	HostCmdResponse();

	U08 bDir               =  tFlashDriverScan->bDir;
    //U08 bDataPatten        =  tFlashDriverScan->bDataPatten;
    //U08 bGcMode            =  tFlashDriverScan->bGcMode;
    U08 bScanEn            =  tFlashDriverScan->bScanEn;
    U08 bKbyteSlice        =  tFlashDriverScan->bKbyteSlice;

	//U32 dPadDriver 			= tFlashDriverScan->dPadDriver;
	U32 dInputImpedance 	= tFlashDriverScan->dInputImpedance;
	U32 dSysclkSel	 		= tFlashDriverScan->dSysclkSel;
	U32 dSysclkPeriodSel 	= tFlashDriverScan->dSysclkPeriodSel;
	U32 dTrx2clkPeriodSel 	= tFlashDriverScan->dTrx2clkPeriodSel;
	U32 dRegDelayVal 		= tFlashDriverScan->dRegDelayVal;
	U32 dGcAddr             = tFlashDriverScan->dGcAddr;

	/*传下来的参数先配置到寄存器*/
	//HAL_PADSSelCtrl0to3(bChanPhy, dPadDriver);/*配置PAD驱动能力*/
	U32 dRdBuffAddr = MEM_DMA_BUFF_ADDR+bKbyteSlice*1024;
	U32 dOnceDataLen = bKbyteSlice*1024;

	if((bDir == DIR_WT)&&(bScanEn == 1))
	{
		if(dGcAddr == 0)
		{
			HAL_PADTSelCtrl(dInputImpedance);/*配置input impedance	TSEL*/
			HAL_InitChanClock(3,dSysclkSel,dSysclkPeriodSel,4);/*配置时钟*/
			HAL_CPMDQS_HandleSetDelay(3,0x1FF);/*设置DQS delay值*/

		}

	}

#if 0
	if((bDir == DIR_RD)&&(bScanEn == 0))
	{
		MemorySet((U8 *)(dRdBuffAddr),0x66,dOnceDataLen);
		if(dGcAddr == 0)
		{
			HAL_PADTSelCtrl(0);/*配置input impedance	TSEL*/
			HAL_InitChanClock(3,dSysclkSel,dSysclkPeriodSel,2);/*配置时钟*/
			HAL_CPMDQS_HandleSetDelay(3,0x31);/*设置DQS delay值*/
		}
		HAL_GoCacheBurstWR(3,3,DIR_READ,dOnceDataLen,dGcAddr,dRdBuffAddr);
	}
#endif

	if((bDir == DIR_RD)&&(bScanEn == 1))
	{
		MemorySet((U8 *)(dRdBuffAddr),0x66,dOnceDataLen);
		if(dGcAddr == 0)
		{
			HAL_PADTSelCtrl(dInputImpedance);/*配置input impedance	TSEL*/
			HAL_InitChanClock(3,dSysclkSel,dSysclkPeriodSel,dTrx2clkPeriodSel);/*配置时钟*/
			HAL_CPMDQS_HandleSetDelay(3,dRegDelayVal);/*设置DQS delay值*/


		}
	}

	HostDataTransfer(DMA1_RD_TO_HOST,dRdBuffAddr,pCtrl->dSecCnt,JOINT_NONE);//send 1 sector status information to host
}

//0(B0KB),1(B16A),2(B17A),3(8T23_Tos),4(8T23_Sand),5(M16),6(N18),7(B27)
void FDL_CMD_E4_FlashInit(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	T_FDL_SCAN_FLASH_DRIVER *tFlashDriverScan = (T_FDL_SCAN_FLASH_DRIVER *)pCtrl->bBuff;

	HostCmdResponse();
	FDL_Test_Init();

	U08 bChan;
	U08 bChip;
	U08 bRealWrite = tFlashDriverScan->bWtEn;

	for(bChan = 0;bChan<4;bChan++)
	{
		for(bChip = 0;bChip<4;bChip++)
		{
			if((tFlashDriverScan->bChan[bChan] == 0xFE)&&(tFlashDriverScan->bCe[bChip] == 0xFE))
			{
				FDL_Log("Flash Init!Chan = %d, CE = %d",bChan,bChip);

				U32 dSysclkSel			= tFlashDriverScan->dSysclkSel;
				U32 dSysclkPeriodSel	= tFlashDriverScan->dSysclkPeriodSel;
				U32 dTrx2clkPeriodSel	= tFlashDriverScan->dTrx2clkPeriodSel;

				HAL_InitChanClock(bChan,dSysclkSel,dSysclkPeriodSel,dTrx2clkPeriodSel);/*配置时钟*/
				FDL_FlashIFModeInit(bChan, bChip);

				if(tFlashDriverScan->dFlashInnnerImpedan != 0xFF)/*支持Flash内部阻抗扫描*/
				{
					U08 bData[4] = {0};
					bData[0] = tFlashDriverScan->dFlashInnnerImpedan;
					HAL_McuSetFeature(bChan,bChip,0x10,1,4,bData);
					bData[0] = 0xFF;
					HAL_McuGetFeature(bChan,bChip,0x10,bData);
					if(bData[0] != tFlashDriverScan->dFlashInnnerImpedan)
					{
						FDL_Log("************Flash Drive Set Fail!");
					}
					else
					{
//						FDL_Log("Set OK");
					}
				}
			}
		}
	}

	U16 wBlkWt = tFlashDriverScan->wBlk;
	U32 dRow = wBlkWt*ROW_GAP_OF_PLANE;
	U32 dDMABuffAddr;

	if(bRealWrite)
	{
		FDL_Log("Real Write Flash Scan !");

		for(bChan = 0;bChan<4;bChan++)
		{
			SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;/*开关ECC*/
			for(bChip = 0;bChip<4;bChip++)
			{
				if((tFlashDriverScan->bChan[bChan] == 0xFE)&&(tFlashDriverScan->bCe[bChip] == 0xFE))
				{
					dDMABuffAddr = MEM_DMA_BUFF_ADDR+0x400 + (bChan*4+bChip)*SECT_OF_PAGE*BYTE_OF_SEC;

					HAL_CQChipSelect(bChan,bChip);
					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);

					FL_CMD_CQ_SLC_MODE(bChan);
					HAL_CQSendFlashCmd(bChan,FL_CMD_BLOCK_ERASE1);
					HAL_CQSendRowAddr(bChan, dRow);
					HAL_CQSendFlashCmd(bChan,FL_CMD_BLOCK_ERASE2);

					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
					HAL_CQChipSelect(bChan,bChip);
					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);

					FL_CMD_CQ_SLC_MODE(bChan);
					HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
					HAL_CQSend5Addr(bChan, 0, dRow);
					HAL_CQSetRndSeed(bChan, dRow, INVALID_U16);
					HAL_CQSetUd(bChan, MEM_UD_BUFF_ADDR, SECT_OF_DATA_CLST , UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);

					HAL_CQStartDMA2(bChan, dDMABuffAddr, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE, JOINT_NONE, (U32)NULL);
					HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
					HAL_CQCheckRealDone(bChan);
					HAL_CQWaitDone(bChan);
				}
			}
		}
	}

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,pCtrl->dSecCnt,JOINT_NONE);
}



void FDL_CMD_F2_WriteTlcBlk(void)
{
	U08 bChanSt;		
	U08 bChanEd;		
	U08 bLunSt;		
	U08 bLunEd;	
	U16 wBlockSt;	 
	U16 wBlockEd;
	//U16 wFailEn;

    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
    
	HostCmdResponse();//有何用处
	bChanSt = pCtrl->bBuff[0];
	bChanEd = pCtrl->bBuff[1];
	bLunSt = pCtrl->bBuff[2];
	bLunEd = pCtrl->bBuff[3];
	wBlockSt = (U16)pCtrl->dBlkAddr;
	wBlockEd = (U16)pCtrl->dBlkCnt;
	//HAL_Test_PrgTlcBlks(bChanSt,bChanEd, bLunSt,bLunEd,wBlockSt,wBlockEd,1);
	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);
}


void SUMSUNG_3DV4GetPageType(U32 wPageNum,U16 *bWLNum,U08 *pbPageOffsInWL)
{
	U16 wPrgWLNum 		= 0;/*WL的编号*/
	//U16 wTmpPageNum 	= 0;
	U16 wPageOffInWl 	= 0;

	if(wPageNum <= 7)
	{
		wPrgWLNum = wPageNum/2;
		wPageOffInWl = wPageNum%2 + 2;
	}
	else if(wPageNum <= 0x2EF)
	{
		wPrgWLNum = (wPageNum + 1)/3 + 1;		
		wPageOffInWl = (wPageNum - 2)%3 + 1;
	}
	else if(wPageNum <= 0x2F7)
	{
		wPrgWLNum = 252 + (wPageNum - 752)/2;		
		wPageOffInWl = wPageNum%2 + 2;
	}
	else
	{
		wPrgWLNum = 248 + wPageNum - 752;
		wPageOffInWl = 3;
	}

	*bWLNum = wPrgWLNum;
	*pbPageOffsInWL = wPageOffInWl;
}

void FDL_CMD_F3_WriteTlcPage(void)
{
	#if (NAND_FLASH != SUMSUNG_3DV4) 

	U08 bChanSt;		
	U08 bLunSt;		
	U32 dBlockSt;	 
	U32 dFailEn;
	U32 dPgRow;
	U08 bChan;
	U08 bCmdTag;
	
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
    
	HostCmdResponse();//有何用处
	bChanSt = pCtrl->bBuff[0];
	bLunSt 	= pCtrl->bBuff[1];
	bCmdTag = pCtrl->bBuff[2];
	dBlockSt = (U16)pCtrl->dBlkAddr;
	dPgRow = pCtrl->dPageAddr;
	dFailEn = pCtrl->dRev[0];

	
    U08 bLunOfChip  = bLunSt % LUN_OF_CHIP;
	U08 bLogChip	= bLunSt / LUN_OF_CHIP;
	U08 bChip		= 0;
	
    bChan = CHANS(bChanSt);
	bChip = CHIPES(bChan,bLogChip);
	U32 dRow = GET_ROW_ADDR_TLC(dPgRow, dBlockSt, bCmdTag);
	U08 bLastProgCmd = FL_CMD_PAGE_PROG2;
	
	if((bCmdTag == 1) || (bCmdTag == 2))//兼容ED3 Flash
	{
		bLastProgCmd = FL_CMD_ED3_PRO_CMD;
	}
	
	HAL_ADMComSelEccMode(bChan,ECC_MODE_NONE);

	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;

	HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);

	FL_CMD_CQ_TLC_MODE(bChan);
	FL_CMD_CQ_ED3_READ_WL(bChan,bCmdTag);

	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay
	
	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	HAL_CQSend5Addr(bChan, 0, dRow);
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
		
	HAL_CQSendFlashCmd(bChan, bLastProgCmd);
	FDL_Log("WtTlcChan = %x-%d-0x%X",bChan,bChip,dRow);

	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	HAL_CQWaitDone(bChan);
	
	HAL_ADMComSelEccMode(bChan,ECC_MODE_LDPC);
	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);
#else

	U08 bChanSt;		
	U08 bLunSt;		
	U32 dBlockSt;	 
	//U32 dFailEn=0;
	U32 dPgRow;
	U08 bChan;
	U08 bCmdTag;
	
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
    
	HostCmdResponse();//有何用处
	bChanSt = pCtrl->bBuff[0];
	bLunSt 	= pCtrl->bBuff[1];
	bCmdTag = pCtrl->bBuff[2];
	dBlockSt = (U16)pCtrl->dBlkAddr;
	dPgRow = pCtrl->dPageAddr;
	//dFailEn = pCtrl->dRev[0];

	
    //U08 bLunOfChip  = bLunSt % LUN_OF_CHIP;
	U08 bLogChip	= bLunSt / LUN_OF_CHIP;
	U08 bChip		= 0;
	
    bChan = CHANS(bChanSt);
	bChip = CHIPES(bChan,bLogChip);
	U32 dRow = GET_ROW_ADDR_TLC(dPgRow, dBlockSt, bCmdTag);
	U08 bLastProgCmd = FL_CMD_PAGE_PROG2;
	
	if((bCmdTag == 1) || (bCmdTag == 2))//兼容ED3 Flash
	{
		bLastProgCmd = FL_CMD_ED3_PRO_CMD;
	}

	U16 wWordLine = 0;
	U08 bPageType = 0;
	SUMSUNG_3DV4GetPageType(dPgRow,&wWordLine,&bPageType);
	FDL_Log("0x%08X:PageNum = %03d,PageType:%03d--%d",dRow,dPgRow,wWordLine,bPageType);

	HAL_ADMComSelEccMode(bChan,ECC_MODE_NONE);
	HAL_CQChipSelect(bChan,bChip);
	FL_CMD_MCU_TLC_MODE(bChan);
//	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);


	if(wWordLine <= 3)
	{
		if(bPageType == 2)
		{
			bLastProgCmd = FL_CMD_PAGE_PROG2 + 1 + (GET_PLANE_NUM(dRow) << 4);	
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan, 0, 0);
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, bLastProgCmd);
			
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x50;				// delay
			HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);

			bLastProgCmd = FL_CMD_PAGE_PROG2 + bPageType + (GET_PLANE_NUM(dRow) << 4);	
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan, 0, dRow);		
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, bLastProgCmd);			
		}
		else
		{
			bLastProgCmd = FL_CMD_PAGE_PROG2 + bPageType + (GET_PLANE_NUM(dRow) << 4);	
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan, 0, dRow);			
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, bLastProgCmd);

			HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);
			HAL_CQSendFlashCmd(bChan, 0x8B);
			HAL_CQSend5Addr(bChan, 0, GET_ROW_ADDR_TLC(wWordLine, dBlockSt, bCmdTag));
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);			
		}
		
		
	}
	else if(wWordLine <= 251)
	{
		bLastProgCmd = FL_CMD_PAGE_PROG2 + bPageType + (GET_PLANE_NUM(dRow) << 4);		

		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
		HAL_CQSend5Addr(bChan, 0, dRow);
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
		HAL_CQSendFlashCmd(bChan, 0xC0);		
		HAL_CQSendAddr(bChan, bLastProgCmd);		
		if(bPageType == 3)
		{
			
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x50;				// delay
			HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);
			HAL_CQSendFlashCmd(bChan, 0x8B);
			HAL_CQSend5Addr(bChan, 0, GET_ROW_ADDR_TLC(wWordLine, dBlockSt, bCmdTag));
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);			
		}
	}
	else if(wWordLine <= 255)
	{
		if(bPageType == 2)
		{
			bLastProgCmd = FL_CMD_PAGE_PROG2 + 1 + (GET_PLANE_NUM(dRow) << 4);	
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan, 0, 0);
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, bLastProgCmd);			
			HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);

			bLastProgCmd = FL_CMD_PAGE_PROG2 + bPageType + (GET_PLANE_NUM(dRow) << 4);	
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan, 0, dRow);		
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, bLastProgCmd);			
		}
		else
		{
			bLastProgCmd = FL_CMD_PAGE_PROG2 + bPageType + (GET_PLANE_NUM(dRow) << 4);	
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan, 0, dRow);			
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, bLastProgCmd);

			HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);
			HAL_CQSendFlashCmd(bChan, 0x8B);
			HAL_CQSend5Addr(bChan, 0, GET_ROW_ADDR_TLC(wWordLine, dBlockSt, bCmdTag));
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);			
		}
		
	}
	else
	{
		bLastProgCmd = FL_CMD_PAGE_PROG2 + bPageType + (GET_PLANE_NUM(dRow) << 4);	
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
		HAL_CQSend5Addr(bChan, 0, dRow);			
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
		HAL_CQSendFlashCmd(bChan, 0xC0);
		HAL_CQSendAddr(bChan, bLastProgCmd);
		
		HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);
		HAL_CQSendFlashCmd(bChan, 0x8B);
		HAL_CQSend5Addr(bChan, 0, GET_ROW_ADDR_TLC(wWordLine, dBlockSt, bCmdTag));
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);			
	}

	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);

	HAL_CQWaitDone(bChan);
	HAL_ADMComSelEccMode(bChan,ECC_MODE_LDPC);
#if 0
	ADM_ComSelEccMode(bChan,MODE_NONE);

	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;

	HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan,0, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);

	FL_CMD_CQ_TLC_MODE(bChan);
	FL_CMD_CQ_ED3_READ_WL(bChan,bCmdTag);

	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay
	
	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	HAL_CQSend5Addr(bChan, 0, dRow);
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE + 4,JOINT_NONE, MEM_DMA2_INFO_ADDR);
		
	HAL_CQSendFlashCmd(bChan, bLastProgCmd);
	FDL_Log("WtTlcChan = %x-%d-0x%X",bChan,bChip,dRow);

	HAL_CQCheckIOStatus(bChan,0, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	HAL_CQWaitDone(bChan);
	
	ADM_ComSelEccMode(bChan,MODE_LDPC);
#else
#endif
	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);
#endif
}


void FDL_CMD_FE_GetFlashIOData(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

	HostCmdResponse();//有何用处
	U08 bChan = 0;
	U08 bChip = 0;
	U08 bLunOfChip = 0;
	U32 dRow = 0;

	U32 dRndSeed = pCtrl->dBlkAddr;
	U08 bDataMode = pCtrl->dMaskEn;
	U08 bAttchSize = SECT_OF_DATA_CLST;
	U08 bUdLen = UD_LEN_OF_DATA_CLST;

	if(bDataMode == 1)
	{
		bAttchSize = SECT_OF_TBLE_CLST;
		bUdLen = UD_LEN_OF_TBLE_CLST;
	}

    FDL_Log("FE start Data Mode %x",bDataMode);
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 0;
	HAL_LDPC_ParityChangeCfg(1,bDataMode);

	HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan,bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	HAL_CQSendFlashCmd(bChan, 0xDA);

	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	HAL_CQSend5Addr(bChan, 0, dRow);

	HAL_CQSetRndSeed(bChan,dRndSeed,INVALID_U16);
	HAL_CQSetUd(bChan,MEM_UD_BUFF_ADDR, bAttchSize ,bUdLen,DMA2_WR_TO_FLASH);
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE,JOINT_NONE, MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQWaitDone(bChan);

	HAL_CQCheckIOStatus(bChan,bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	HAL_CQSendRndAddr(bChan, 0,dRow);
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, 0, 0,SECT_OF_PAGE,JOINT_NONE, MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQWaitDone(bChan);

	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
	HAL_CQCheckIOStatus(bChan,bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	HAL_CQSendRndAddr(bChan, 0,dRow);
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, 0, 0,SECT_OF_PAGE+8,JOINT_NONE, MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQWaitDone(bChan);

	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 0;

    FDL_Log("FE end %d",dRndSeed);

	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);
}



void FDL_CMD_FF_InjectionCRCErr(void)
{
	//T_CRC_ERR *pCRC= (T_CRC_ERR*)&tPrivCMD;

    FDL_Log("FF start");
#if 0
	SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM=pCRC->wDataFisNum;//(((dTotalSec*512)-1)>>13)+1;
	SG_DSATA->rTRXCRC.bits.ERR_TAG=pCRC->bTag;
	//SG_DSATA->rTRXCRC.bits.TXCRC =pCRC->bDir;
	//SG_DSATA->rTRXCRC.bits.RXCRC =!pCRC->bDir;
	SG_DSATA->rSTCRCE.dAll=1;
	LOG_PRINT("bTag = %x, wDataFisNum = %x, TXCRC = %x RXCRC = %x\n",
			SG_DSATA->rTRXCRC.bits.ERR_TAG,
			SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM,
			//SG_DSATA->rTRXCRC.bits.TXCRC,
			//SG_DSATA->rTRXCRC.bits.RXCRC
			);
#endif
    FDL_Log("FF end");
}

void FDL_CMD_A8_WriteFlash_PhyPage()
{
	//U08 bCmdTag;
	
	//T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	//T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	
	U8 bChanSt = tPrivCMD.bOpCh;
	U8 bLunSt = tPrivCMD.bOpCe;
	U32 dRowAddr = tPrivCMD.bOpPageAddr;
	//U16 wColAddr = tPrivCMD.bOpPageOffs;
	U8 bFlashMode = tPrivCMD.Reserve[0];
		
	U08 bLogChip	= bLunSt / LUN_OF_CHIP;	
	U08 bChan = CHANS(bChanSt);
	U08 bChip = CHIPES(bChan,bLogChip);

	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
    SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 0x200;
	
	HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);

	if(bFlashMode == SLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan, 0xDA);
	}
	else
	{
		HAL_CQSendFlashCmd(bChan, 0xDF);
	}

	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	HAL_CQSend5Addr(bChan, 0, dRowAddr);
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE+8,JOINT_NONE, MEM_DMA2_INFO_ADDR);
	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
	HAL_CQWaitDone(bChan);

	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 0;
    SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 0x800;
	HostDataTransfer(DMA1_WR_TO_BUFF,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
}


void FDL_CMD_A9_ReadFlash_PhyPage()
{
#if 1//def __TEST_LLR_MAX_ECC__

	U08 bChan = tPrivCMD.bOpCh;
	U08 bChip = tPrivCMD.bOpCe;
	U32 dRowAddr = tPrivCMD.bOpPageAddr;
	U16 wColAddr = tPrivCMD.bOpPageOffs;
	//U08 bFlashMode = tPrivCMD.Reserve[0];
	U32 dUserDataAddr = MEM_UD1_BUFF_ADDR;
	U08 bArcEn = tPrivCMD.Reserve[1];
	U08 bSetOptionEn = tPrivCMD.Reserve[2];

	U08 bArcCnt = READ_RETRY_ARC_MAX;
	U08 i;
	
	bChan = CHANS(bChan);
	bChan = 0;
	bSetOptionEn = 0;
	bArcEn = 1;

	if(bSetOptionEn == 1)
	{
		FDL_FlashIFModeInit(bChan, bChip);
		U8 bOptionOK = 0;//CheckBestOption(bChan,bChip,dRowAddr,wColAddr);
		FDL_Log("Best Option**********************0x%x--0x%08X",bOptionOK,dRowAddr);
		HAL_CQWaitDone(bChan);
		Chan_Check_Status(bChan, bChip, 0);
		Retry_SetFlashReg(bChan,bChip,bOptionOK, dRowAddr);
		Chan_Check_Status(bChan, bChip, 0);
	}
	
	HAL_ADMComSelEccMode(bChan,2);/*关闭ECC*/
	//FDL_Log("RdChan = %d-0x%X-%d",bChan,dRowAddr,bArcEn);
	
	if(bArcEn == 1)
#if 0
	{		
		U08 pReadOffset[4] = {0x0,0,0,0};
		Retry_SetArcStartOffset(bChan,bChip,pReadOffset);

		for(i = 0;i<bArcCnt;i++)
		{
			CQ_WaitDone(bChan);
			Toggle_Pullup(bChan);
			Chan_Check_Status(bChan, bChip, 0);
			if(i == 0)
			{
				Retry_SetFlashReg_Arc(bChan, bChip, 0x1);
			}
			else if(i == 1)
			{
				Retry_SetFlashReg_Arc(bChan, bChip, 0x3);
			}

			Toggle_Pullup(bChan);

			__nds32__dsb();
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE|1<<bChip;
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x1a;				// delay
			if(0 == bFlashMode)
			{
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_SLC_MODE;
			}
			else
			{
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_TLC_MODE;
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x1a;				// delay
			}
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ1;
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR2 | wColAddr;
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR3 | dRowAddr;
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ2;

			CQ_WaitDone(bChan);
			Toggle_Pullup(bChan);
			Chan_Check_Status(bChan, bChip, 0);
			CQ_WaitDone(bChan);

			if(i+1 == bArcCnt)
			{
				SG_FC(bChan)->rCQ_INSTRUCT_REG = 1<<bChip;
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x1a;				// delay
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_RANDOM_DATA_OUTPUT1;
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR2 | wColAddr;
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_RANDOM_DATA_OUTPUT2;

				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x3; 			// delay
				CQ_WaitDone(bChan);
				SG_DMA2_CH(bChan)->rINDEX_CFG = 0;
				SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG = tPrivCMD.wOpSctCnt;

				SG_DMA2_CH(bChan)->rFLASH_INDEX.dAll = 0x000;
				SG_DMA2_CH(bChan)->rBSADDR.dAll = MEM_DMA_BUFF_ADDR;

				SG_DMA2_CH(bChan)->rUDATA_BSADDR.dAll = dUserDataAddr;
				SG_DMA2_CH(bChan)->rDMAINF_BSADDR.dAll = MEM_DMA2_INFO_ADDR;
				HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
				SG_DMA2_CH(bChan)->rCTRL.wAll = 0x7;
				__nds32__dsb();
				while(SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE == 0)
				{
					//LOG_PRINT("bChan=%d, bChip=%d, SG_DMA2_CH(bChan)->rSTATUS.dAll 0x%x\r\n", bChan, bChip, SG_DMA2_CH(bChan)->rSTATUS.dAll);
				}
				Toggle_Pullup(bChan);
			}
		}
		FDL_FlashIFModeInit(bChan,bChip);
	}
#else
	{
			if((dRowAddr&0x00FFFFFF) == 0x4CB654)
			{
				FDL_Log("IS This Row");
			}
			
//			{0,0,0xF1,0xEC,0xf,3,6,9,0};
//			U08 pDataVal[4] = {0xF0,0,0,0};
			U08 pDataVal[4] = {0xF1,0,0,0};

#if 0
			for(m = 0;m<8;m++)
			{
				Fdl_MCU_SetFeature_ReadOffs(bChan,bChip, 0xA4+m, pDataVal);
			}
#else
			Retry_SetArcStartOffset(bChan,bChip,pDataVal);
#endif
	
			for(i = 0;i<bArcCnt;i++)
			{
				CQ_WaitDone(bChan);
				Chan_Check_Status(bChan, bChip, 0);
	
				if(i == 0)
				{
					Retry_SetFlashReg_Arc(bChan, bChip, 0x1);
				}
				else if(i == 1)
				{
					Retry_SetFlashReg_Arc(bChan, bChip, 0x3);
				}
				HAL_CQChipSelect(bChan,bChip);
				Fdl_ReadPreSendCmd(bChan,dRowAddr);
				Fdl_CQPreReadSinglePln(bChan,bChip,wColAddr,dRowAddr,FL_CMD_READ2);
				HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);
				HAL_CQSendRndAddr(bChan,wColAddr,dRowAddr); 
				
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x3; 			// delay
				CQ_WaitDone(bChan);
				SG_DMA2_CH(bChan)->rINDEX_CFG = 0;
				SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG = tPrivCMD.wOpSctCnt;

				SG_DMA2_CH(bChan)->rFLASH_INDEX.dAll = 0x000;
				SG_DMA2_CH(bChan)->rBSADDR = MEM_DMA_BUFF_ADDR;

				SG_DMA2_CH(bChan)->rUDATA_BSADDR.dAll = dUserDataAddr;
				SG_DMA2_CH(bChan)->rDMAINFO_BSADDR.dAll = MEM_DMA2_INFO_ADDR;
				HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
				SG_DMA2_CH(bChan)->rCTRL.wAll = 0x7;
				__nds32__dsb();
				while(SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE == 0)
				{
					//LOG_PRINT("bChan=%d, bChip=%d, SG_DMA2_CH(bChan)->rSTATUS.dAll 0x%x\r\n", bChan, bChip, SG_DMA2_CH(bChan)->rSTATUS.dAll);
				}
			}
			FDL_FlashIFModeInit(bChan,bChip);
		}
#endif
	HAL_CQWaitDone(bChan);

	if(bArcEn == 0)
	{
		HAL_CQWaitDone(bChan);
		HAL_CQChipSelect(bChan,bChip);//片选到当前CE
		
//		HAL_CQSendFlashCmd(bChan,0x5D);

		Fdl_ReadPreSendCmd(bChan,dRowAddr);
		Fdl_CQPreReadSinglePln(bChan,bChip,wColAddr,dRowAddr,FL_CMD_READ2);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);
		HAL_CQSendRndAddr(bChan,wColAddr,dRowAddr); 

		SG_FC(bChan)->rCQ_INSTRUCT_REG = 1<<bChip;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x1a;				// delay
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_RANDOM_DATA_OUTPUT1;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR2 | wColAddr;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_RANDOM_DATA_OUTPUT2;
		
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x3; 			// delay
		CQ_WaitDone(bChan);

		SG_DMA2_CH(bChan)->rINDEX_CFG = 0;
		SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG = tPrivCMD.wOpSctCnt;

		SG_DMA2_CH(bChan)->rFLASH_INDEX.dAll = 0x000;
		SG_DMA2_CH(bChan)->rBSADDR = MEM_DMA_BUFF_ADDR;

		SG_DMA2_CH(bChan)->rUDATA_BSADDR.dAll = dUserDataAddr;
		SG_DMA2_CH(bChan)->rDMAINFO_BSADDR.dAll = MEM_DMA2_INFO_ADDR;

		HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);

		SG_DMA2_CH(bChan)->rCTRL.wAll = 0x7;
		__nds32__dsb();
		while(SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE == 0)
		{
			//LOG_PRINT("bChan=%d, bChip=%d, SG_DMA2_CH(bChan)->rSTATUS.dAll 0x%x\r\n", bChan, bChip, SG_DMA2_CH(bChan)->rSTATUS.dAll);
		}
	}
	
	//FDL_Log("RdChan = %d-0x%X-%d",bChan,dRowAddr,bArcEn);
	FDL_Log("Row = 0x%X",dRowAddr);
	
	HAL_ADMComSelEccMode(bChan,1);/*打开ECC*/
		
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	tPrivCMD.wOpSctCnt = 0;
#endif
}




void FDL_Test_CheckDevice(void)
{
	U08 Chan = 0;
	U08 Chip = 0;
	//U08 Lun = 0;
	//U08 bPhyChan = 0;
	//U08 bPhyChip = 0;
	//U08 i,j = 0;
	U08 bDieOK[4][4];
	MemorySet((U8 *)MEM_DMA_BUFF_ADDR, 0, 128);
	MemorySet((U8 *)bDieOK, 0, 16);

	//U08 bValidChan = 0xFF;
	//U08 bValidChip = 0;
	
	for(Chan = 0;Chan < 4;Chan++)
	{
		for(Chip = 0;Chip < 4;Chip++)
		{
			HAL_NFReadFlashID(MEM_DMA_BUFF_ADDR + Chan*4*8 + Chip*8,Chan, Chip);
			if(*(U64 *)(MEM_DMA_BUFF_ADDR + Chan*4*8 + Chip*8) != 0xFFFFFFFFFFFFFFFF)
			{
				bDieOK[Chan][Chip] = 111;
				U08 *pID = (U08 *)(MEM_DMA_BUFF_ADDR + Chan*4*8 + Chip*8);
				FDL_Log("[%d][%d]Flash ID = %02X %02X %02X %02X %02X %02X",Chan,Chip,pID[0],pID[1],pID[2],pID[3],pID[4],pID[5]);
			}
		}
	}

	FDL_Log("*****************************************");
	FDL_Log("*CH/CE:|..CE0..|..CE1..|..CE2..|..CE3..|*");
	for(Chan = 0;Chan < 4;Chan++)
	{	
		FDL_Log("*Chan%d:|..%03d..|..%03d..|..%03d..|..%03d..|*",Chan,bDieOK[Chan][0],
			bDieOK[Chan][1],bDieOK[Chan][2],bDieOK[Chan][3]);
	}
	FDL_Log("*****************************************");

#ifndef JUMP_CH_CE//跳CH和CE自动Mapping支持

	for(Chip = 0;Chip<4;Chip++)
	{
		if(bDieOK[bValidChan][Chip] == 111)
		{
			bValidChip++;
		}
	}
	
	T_FLASH_PARAM *tFlashParam = &tUdpFdl.m_tFlashParam;
	tFlashParam->m_bChipOfChan = bValidChip;
	tFlashParam->m_bLunOfChip = 1;
	T_INVAL_CHAN_CHIP_MAP tInvalChanChipMap[MAX_CHAN_OF_DEV] = {0};
	
	for(Chan = 0;Chan<MAX_CHAN_OF_DEV;Chan++)
	{
		for(Chip = 0;Chip<bValidChip;Chip++)
		{
			if(bDieOK[Chan][Chip] != 111)
			{
				tInvalChanChipMap[Chan].bInvalChan = 1;
				tInvalChanChipMap[Chan].bInvalChip[Chip] = 1;
			}
		}
	}

	for(i = 0; i <	MAX_CHAN_OF_DEV; i++)
	{
		for(j = 0; j <	MAX_CHIP_OF_CHAN; j++)
		{
			if(tInvalChanChipMap[i].bInvalChip[j] != 1)
			{	
				tFlashParam->m_bChipMapOfCh[i][bPhyChip] = j;

				bPhyChip++;

				if(bPhyChip == (tFlashParam->m_bChipOfChan)) 
				{
					break;
				}
			}

		}
		
		if(tInvalChanChipMap[i].bInvalChan != 1)
		{
			tFlashParam->m_bChMapOfDev[bPhyChan] = i;
			bPhyChan++;
		}
		bPhyChip = 0;
	}

	for (Chan = 0; Chan < MAX_CHAN_OF_DEV; Chan++)
	{			
		for (Chip = 0; Chip < tFlashParam->m_bChipOfChan; Chip++)
		{			
			for (Lun = 0; Lun < tFlashParam->m_bLunOfChip; Lun++)
			{
				tFlashParam->m_bPhyChipOfLogLun[Chan][Chip*tFlashParam->m_bLunOfChip + Lun] = tFlashParam->m_bChipMapOfCh[Chan][Chip];
				tFlashParam->m_bPhyLunOfLogLun[Chan][Chip*tFlashParam->m_bLunOfChip + Lun] = Lun;		
			}
		}
	}
#endif
}


void MemCpyWithOp(U8 *pDst,U8 *pSrc,U8 bOp,U16 wLen)
{
    while(wLen--)
    {
        switch(bOp)
        {
        case OP_MOVE:
            *(pDst++) = *(pSrc++);
            break;
        case OP_AND:
            *(pDst++) &= *pSrc++;
            break;
        case OP_OR:
            *(pDst++) |= *(pSrc++);
            break;
        case OP_XOR:
            *(pDst++) ^= *(pSrc++);
            break;
        default:
            *(pDst++) = *(pSrc++);
        }
    }
}

void SetRegisterValue(void)
{
    U16 wIdx;
    Register *pRegister = (Register *)MEM_DMA_BUFF_ADDR;
   // LOG_PRINT("SetRegisterValue = 0x%x\r\n",MEM_DMA_BUFF_ADDR);
    HostDataTransfer(DMA1_WR_TO_BUFF,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);

    for (wIdx = 0; wIdx < tPrivCMD.dOpByteCnt; wIdx++, pRegister++)
    {
        MemCpyWithOp((U8*)pRegister->RegAddr,(U8*)&pRegister->RegVal,pRegister->Operate,pRegister->Width);
        //LOG_PRINT("width = %x\r\n",pRegister->Width);
        if(pRegister->Width == 1)
        {
       //     LOG_PRINT("RegAddr = %x, SetVal = %x, GetVal = %x\n", (U32)pRegister->RegAddr, pRegister->RegVal, *(U8*)pRegister->RegAddr);
        }
        else if(pRegister->Width == 2)
        {
       //     LOG_PRINT("RegAddr = %x, SetVal = %x, GetVal = %x\n", (U32)pRegister->RegAddr, pRegister->RegVal, *(U16*)pRegister->RegAddr);
        }
        else
        {
       //     LOG_PRINT("RegAddr = %x, SetVal = %x, GetVal = %x\n", (U32)pRegister->RegAddr, pRegister->RegVal, *(U32*)pRegister->RegAddr);
        }
    }
}
void GetRegisterValue(void)
{
    U16 wIdx;
    //LOG_PRINT("GetRegisterValue = 0x%x\r\n",MEM_DMA_BUFF_ADDR);
    HostCmdResponse();

    Register *pRegister = (Register *)MEM_DMA_BUFF_ADDR;
    //LOG_PRINT("GetRegisterValue = 0x%x, 0x%x\r\n",*(U8*)&pRegister->RegVal, *(U8*)pRegister->RegAddr);

    for (wIdx = 0; wIdx < tPrivCMD.dOpByteCnt; wIdx++, pRegister++)
    {
        MemCpyWithOp((U8*)&pRegister->RegVal, (U8*)pRegister->RegAddr, OP_MOVE, pRegister->Width);

        if(pRegister->Width == 1)
		{
			//LOG_PRINT("RegAddr = %x, GetVal = %x, AddrVal = %x\n", (U32)pRegister->RegAddr, pRegister->RegVal, *(U8*)pRegister->RegAddr);
		}
		else if(pRegister->Width == 2)
		{
			//LOG_PRINT("RegAddr = %x, GetVal = %x, AddrVal = %x\n", (U32)pRegister->RegAddr, pRegister->RegVal, *(U16*)pRegister->RegAddr);
		}
		else
		{
			//LOG_PRINT("RegAddr = %x, GetVal = %x, AddrVal = %x\n", (U32)pRegister->RegAddr, pRegister->RegVal, *(U32*)pRegister->RegAddr);
		}
    }

    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
}

 void CQ_WaitDone(U8 bChan)
{
    while(SG_FC(bChan)->rCQ_FIFO_ST_CTRL.bits.CQ_FIFO_IDLE_SPACE != 1024);
}

 void Chan_Check_Status(U8 bChan, U8 bChip, U8 bTestFailEn)
{
	SG_FC(bChan)->rCQ_INSTRUCT_REG = 1<<bChip;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x18;				// delay
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ_STATUS;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;				// delay
	if (/*1 == bTestFailEn*/0)		// test fail
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (0x80 | (0x1<<8));
	}
	else
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (0x40);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (0x40);
	}
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x8;				// delay
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA;
	CQ_WaitDone(bChan);
}


void Get_ValidChan()
{
	U8 bChanCnt = tPrivCMD.bOpCh;
	U8 bChipCnt = tPrivCMD.bOpCe;
	U8 bChan, bChip;
	T_FLASH_PARAM *tFlashParam = &tUdpFdl.m_tFlashParam;

	LOG_PRINT("bChan:%d,bChip:%d,tPrivCMD.wOpSctCnt: 0x%x\r\n", bChanCnt,bChipCnt,tPrivCMD.wOpSctCnt);
	HostDataTransfer(DMA1_WR_TO_BUFF,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	for (bChan = 0; bChan < MAX_CHAN_OF_DEV; bChan++)
	{
		tFlashParam->m_bChMapOfDev[bChan] = *((U8 *)MEM_DMA_BUFF_ADDR+bChan);
		for (bChip = 0; bChip < MAX_CHAN_OF_DEV; bChip++)
		{
			tFlashParam->m_bChipMapOfCh[bChan][bChip] = *((U8 *)MEM_DMA_BUFF_ADDR+bChanCnt+bChip);
			LOG_PRINT("tFlashParam->m_bChipMapOfCh[%d]=%d\n", bChan,tFlashParam->m_bChipMapOfCh[bChan][bChip]);
		}
		LOG_PRINT("tFlashParam->m_bChMapOfDev[bChan]=%d\n", tFlashParam->m_bChMapOfDev[bChan]);
	}
}
void SetReadOffsVal()
{
	U8 bChan = tPrivCMD.bOpCh;
	U8 bOffsVal = tPrivCMD.Reserve[1];

	//Fdl_SetReadOffset(bChan, 0, dRowAddr, bFlashMode, bOffsVal);
	FDL_Log(" Fdl_SetReadOffset, bChan=%d, bOffsVal=0x%x", bChan, bOffsVal);

	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	tPrivCMD.wOpSctCnt = 0;
	LOG_PRINT("tPrivCMD.wOpSctCnt=%d\n", tPrivCMD.wOpSctCnt);
}
void FDL_EraseBlk()
{
	U8 bChan = tPrivCMD.bOpCh;
	U8 bChip = tPrivCMD.bOpCe;
	U32 dBlkAddr = tPrivCMD.bOpPageAddr;
	U8 bFlashMode = tPrivCMD.Reserve[0];
//	LOG_PRINT("bChan=%d,bChip=%d,dBlkAddr=0x%x,bFlashMode=%d\n", bChan, bChip,dBlkAddr,bFlashMode);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = 1<<bChip;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xa;				// delay
	if(0 == bFlashMode)
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_SLC_MODE;
	}
	else
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_TLC_MODE;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xa;				// delay
	}
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_BLOCK_ERASE1;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR3 | dBlkAddr;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_BLOCK_ERASE2;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ_STATUS;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | 0x40;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = 1<<bChip;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xa;				// delay
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA;
    while(SG_FC(bChan)->rCQ_FIFO_ST_CTRL.bits.CQ_FIFO_IDLE_SPACE != 1024);

	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	tPrivCMD.wOpSctCnt = 0;
	//LOG_PRINT("tPrivCMD.wOpSctCnt=%d\n", tPrivCMD.wOpSctCnt);
}

void FDL_SetLDPC_ItrVal()
{

}

void FDL_SetSmallDMAInfoSize()
{
	U8 bEccInfoBuffSize = tPrivCMD.Reserve[0];
	U8 bADM_BUF_SIZE = tPrivCMD.Reserve[1];
	tFdlParam.dEccInfoBuffSize = bEccInfoBuffSize;
	pFdlParam.dEccInfoBuffSize = bEccInfoBuffSize;
	SG_ADM->rBUF_SIZE = bADM_BUF_SIZE;
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	tPrivCMD.wOpSctCnt = 0;
	LOG_PRINT("bEccInfoBuffSize=%d,bADM_BUF_SIZE=%d\n", bEccInfoBuffSize, bADM_BUF_SIZE);
	LOG_PRINT("FDL_SetSmallDMAInfoSize=%d\n", tPrivCMD.wOpSctCnt);
}

void clear_sts()
{
	U32 i;

	for(i=0; i</*(256+32)*/512/16; i++)
	{
		SG_ADM->rBIT_INDEX.bits.dVAL = i*16;
		SG_ADM->rCLR_BIT_STS.bits.dVAL = 0xffff;
		while(0 != SG_ADM->rCLR_BIT_STS.bits.dVAL);		// 硬件执行完自动清零
		SG_ADM->rREAD_BUF_STS.bits.dVAL = 1;
		while(0 != SG_ADM->rREAD_BUF_STS.bits.dVAL);		// 硬件执行完自动清零
	}
}

void Expt_ReadClstToDiscreteBuff()
{
	T_FLASH_PHY_ADDR *tClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	U08 bJointMode = JOINT_21;
	if(0==pCtrl->dRev[0])
	{
		bJointMode = JOINT_NONE;
	}
	tPrivCMD.wOpSctCnt = 1;


	U16 *pwSeedIndex = NULL;
	U16 *wBuffIndex = (U16 *)&pCtrl->bBuff[0];//当前段BuffIdx
	U16 wClstCnt = pCtrl->dClstCnt;//返回到Host扇区个数
	U08 bMaskBuff[16];

	U32 i = 0;
	//HostCmdResponse();
	FDL_Log("CB discrete Buffer read, wClstCnt=0x%x", wClstCnt);
	for(i = 0;i < 16;i++)
	{
		bMaskBuff[i] = 0;//HAL_BFCGetBuffStatus(*(wBuffIndex + i))&0xFF;
		FDL_Log("*(wBuffIndex+%d)=0x%x", i, *(wBuffIndex+i));
	}

	FDL_Log("Mask:0x%02X--0x%02X",bMaskBuff[0],bMaskBuff[1]);
	//打印物理地址
  	Fdb_PrintfPhyAddress(tClstAddr,2);

	FDL_TASK_CTRL tTaskCtrl={0};
	tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
	FDL_ReadClstToDiscreteBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR, wBuffIndex, tClstAddr,
					wClstCnt, MEM_UD1_BUFF_ADDR, bMaskBuff, pwSeedIndex, bJointMode);
	FDL_WaitTaskDone();

	//打印水位
	Fdb_PrintfBuffStatus();
	clear_sts();
	Fdb_PrintfBuffStatus();

	HostDataTransfer(DMA1_WR_TO_BUFF,MEM_NAND_ADDR_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	FDL_Log("HostDataTransfer_done");
	tPrivCMD.wOpSctCnt = 0;
//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR + SECTOR_SIZE * wBuffIdx[0], wSecCnt, JOINT_NONE);//send n sector status information to host
	//HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host

}

void Ldpc_SetPValue()
{
	U8 bChan = 0;
	U8 bChip = 0;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	U08 *pValue = (U08 *)(&pCmd->dIndex[0]);
	U08 bOffsetData[4] = {0, 0, 0, 0};
	U08 i;
	U8_C bTLC_Offset_FeatureAddr[7] = {0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB};	//对应的设置值
	static U08 bSetReadRetry = 0;

	if(0==bSetReadRetry)
	{
		Retry_SetFlashReg(bChan, bChip, 0, 0);
		bSetReadRetry = 1;		//只初始化一次
		FDL_Log("bSetReadRetry=0x%x", bSetReadRetry);

		for(i=0; i<7; i++)
		{
			bOffsetData[0] = 0x00;
			Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bTLC_Offset_FeatureAddr[i], bOffsetData);
		}
	}

	for(i=0; i<7; i++)
	{
		bOffsetData[0] = pValue[i];
		FDL_Log("bOffsetData[0]=0x%x, bSetReadRetry=0x%x", bOffsetData[0], bSetReadRetry);
		//Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bTLC_Offset_FeatureAddr[i], bOffsetData);		//TLC Open Page
		Retry_SetFlashReg(bChan, bChip, bSetReadRetry-1, 0);
	}
	bSetReadRetry++;
	if(bSetReadRetry>0x10)
	{
		bSetReadRetry = 1;
	}

	HostDataTransfer(DMA1_WR_TO_BUFF,MEM_NAND_ADDR_BUFF_ADDR,tPrivCMD.wOpSctCnt,JOINT_NONE);
	FDL_Log("HostDataTransfer_done");
	tPrivCMD.wOpSctCnt = 0;
//	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR + SECTOR_SIZE * wBuffIdx[0], wSecCnt, JOINT_NONE);//send n sector status information to host
	//HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);//send 1 sector status information to host

}

U08 bG_OffsetStep = 0;
U08 bG_SbMode = 2;

void Ldpc_SetLLRAndOffset()
{
}
//#endif //ifdef __FDL_TEST_FW_CTRL__

void FDL_Main(void)
{
#if 0// 2chan
	FDL_ReadFlashID(dDtBuff,0,0);
	HAL_Test_System();
#endif
}
#endif //#ifdef __FDL_TEST_MODE_EN__

