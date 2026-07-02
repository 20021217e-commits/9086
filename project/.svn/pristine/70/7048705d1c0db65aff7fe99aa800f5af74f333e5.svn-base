// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  wdt.c
//  文件标识：  
//  内容摘要：  看门狗模块驱动 
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月16日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "common.h"
#include "sg9081.h"
#include "hal_cpm.h"
#include "hal.h"
#include "uitility.h"

void HAL_CPMSysResRegInit(void)
{
    SG_CPM->rSYS_RESERVED0 = 0x00;
    SG_CPM->rSYS_RESERVED1 = 0x00;
    SG_CPM->rSYS_RESERVED2 = 0x00;
    SG_CPM->rSYS_RESERVED3 = 0x00;
}

U8 HAL_CPMGetClkSource(U8 bChan)
{
	return SG_CPM->rTRX2_CLK_SOURCE_SEL.dAll;
}

U8 HAL_CPMGetTrx2Period(U8 bChan)
{
	return(SG_CPM->rTRX2_CLK_PERIOD.dAll);
}

//设置TRX2的时钟源，分频系数等
void HAL_CPMTrx2ClkCfg(U8 bChan ,U8 bSouceSel,U8 bPeriod)
{
	U32 dChRel = (U32)1<<bChan;

	SG_CPM->rTRX2_CLK_SOURCE_SEL.dAll = bSouceSel;
	SG_CPM->rSOFT_GATE0.dAll &=  ~(dChRel<<25);
	SG_CPM->rTRX2_CLK_SWITCH_EN.bAll &= ~dChRel;		//FUNC_DISABLE
	SG_CPM->rTRX2_CLK_PERIOD.dAll = bPeriod;
	Delay(50);
	SG_CPM->rTRX2_CLK_SWITCH_EN.bAll |= dChRel;			//FUNC_ENABLE
	Delay(50);
	SG_CPM->rSOFT_GATE0.dAll = 0xFFFFFFFF;
}

void HAL_CPMSysClkCfg(U8 bSouceSel,U32 bPeriod)
{
	SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
	SG_CPM->rSYS_CLK_PERIOD     = bPeriod;
	SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
	SG_CPM->rSYS_CLK_SOURCE_SEL = bSouceSel;
}

void HAL_CPMPhySoftReset(void)
{
    HAL_CPMTrx2ClkCfg(0,CLK_PER_SEL,CLK_1_DIV);
    HAL_CPMTrx2ClkCfg(1,CLK_PER_SEL,CLK_1_DIV);

    HAL_CPMSysClkCfg(CLK_PER_SEL,CLK_1_DIV);
	SG_CPM->rPRE_CLK_SOURCE_SEL = 2;      //选择osc_clk_25M

}

void HAL_CPMSpiClkCfg(U32 bPeriod)
{
	SG_CPM->rSPI_CLK_SWITCH_EN = 0;
	SG_CPM->rSPI_CLK_PERIOD    = bPeriod;
	SG_CPM->rSPI_CLK_SWITCH_EN = 1;
}

void HAL_CPMLdecClkCfg(U8 bSouceSel,U32 bPeriod)
{
	SG_CPM->rLDEC_CLK_SWITCH_EN = 0;
	SG_CPM->rLDEC_CLK_PERIOD = bPeriod;
	SG_CPM->rLDEC_CLK_SWITCH_EN = 1;
	SG_CPM->rLDEC_CLK_SOURCE_SEL = bSouceSel;
}

void HAL_CPMOpenSoftGate0(U8 bGateType)
{
   SG_CPM->rSOFT_GATE0.dAll |= 1<<bGateType;
}

void HAL_CPMCloseSoftGate0(U8 bGateType)
{
	SG_CPM->rSOFT_GATE0.dAll &= ~(1<<bGateType);
}

void HAL_CPMOpenSoftGate1(U8 bGateType)
{
	SG_CPM->rSOFT_GATE1.dAll |= 1<<bGateType;
}

void HAL_CPMCloseSoftGate1(U8 bGateType)
{
	SG_CPM->rSOFT_GATE1.dAll &= ~(1<<bGateType);
}

void HAL_CPMSoftRest(U8 bMode)
{
    SG_CPM->rSOFT_RST0.dAll = (U32)0x03;
    SG_CPM->rSOFT_RST1.wAll = 0x00;
}

void HAL_CPMSetCHxDQSDelay(U8 bChannel, U8 bVaule)
{
	switch(bChannel)
	{
	case 0x00:
		SG_CPM->rDQS0_DELAY_CTRL = bVaule;
		break;

	case 0x01:
		SG_CPM->rDQS1_DELAY_CTRL = bVaule;
		break;

	case 0x02:
		SG_CPM->rDQS2_DELAY_CTRL = bVaule;
		break;

	case 0x03:
		SG_CPM->rDQS3_DELAY_CTRL = bVaule;
		break;

	default:
		break;
	}
}

/////////////////////////////////////////////WDT//////////////////////////////////////
// *****************************************************************************
// 函数名称: HAL_CPMWDT0_Init
// 函数功能: WDT0功能初始化函数
// 入口参数: 看门狗复位时间，单位毫秒
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_CPMWDT0_Init(U32 dTime)
{
   /*定时器时钟 OSC CLK25M 25M？*/
   SG_CPM->rWDT0_RST_FLAG=0;
   CPM_WDT0_Disable();
   CPM_WDT0_FeedDog();
    
   SG_CPM->rWDT0_THRESHOLD     =  WDT_MILLISECOND_CNT * dTime;
   CPM_WDT0_Enable();
   //要不要使能中断？
   //NVIC_EnableIRQ(WDT0_IRQn);
}
// *****************************************************************************
// 函数名称: HAL_CPMWDT1_Init
// 函数功能: WDT1功能初始化函数
// 入口参数: 看门狗复位时间，单位毫秒
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_CPMWDT1_Init(U32 dTime)
{
   /*定时器时钟 OSC CLK25M 25M？*/
   SG_CPM->rWDT1_RST_FLAG=0;//add after review
   CPM_WDT1_Disable();
   CPM_WDT1_FeedDog();
   SG_CPM->rWDT1_RST_CTRL.dAll = 0xFF; 
   SG_CPM->rWDT1_THRESHOLD     =  WDT_MILLISECOND_CNT * dTime;
   CPM_WDT1_Enable();
   //要不要使能中断？
   //NVIC_EnableIRQ(WDT1_IRQn);
}


U32 HAL_CPMGetVDTCnt(void)
{
	return SG_CPM->rVDT_CNT;
}

void HAL_CPMSetVDTCnt(U32 dCnt)
{
	SG_CPM->rVDT_CNT = dCnt;
}

void HAL_CPMSetPhyPWMode(U8 bMode)
{
	//SG_CPM->rPHY_MODE_CTRL.bAll = bMode;
	SG_CPM->rPHY_POWER_CTRL.dAll = bMode;
}

U8 HAL_CPMGetSysPeriod(void)
{
	return SG_CPM->rSYS_CLK_PERIOD;
}


void HAL_CPMADDLL_Init(U8 bChan,U8 bMode,U32 dInitVal)
{
	U32 *pdRegCtrl = (U32 *)REG_ADDLL_CTRL_ADDR(bChan);
	U32 *pdRegInitVal = (U32 *)REG_ADDLL_INITVAL_ADDR(bChan);

	if(bMode == 0)
	{
		*pdRegCtrl = *pdRegCtrl&(BIT_CLR_0(BIT_ADJUST_MODE));
	}
	else
	{
		*pdRegCtrl = *pdRegCtrl|(BIT_SET_1(BIT_ADJUST_MODE));
	}

	*pdRegInitVal = dInitVal;
}

void HAL_CPMADDLL_Start(U8 bChan)
{
	U32 *pdRegCtrl = (U32 *)REG_ADDLL_CTRL_ADDR(bChan);
	*pdRegCtrl = *pdRegCtrl|(BIT_SET_1(BIT_LOCK_STAT));
}

void HAL_CPMADDLL_Stop(U8 bChan)
{
	U32 *pdRegCtrl = (U32 *)REG_ADDLL_CTRL_ADDR(bChan);
	*pdRegCtrl = *pdRegCtrl|(BIT_SET_1(BIT_LOCK_STOP));
}

void HAL_CPMADDLL_Updata(U8 bChan)
{
	U32 *pdRegCtrl = (U32 *)REG_ADDLL_CTRL_ADDR(bChan);
	*pdRegCtrl = *pdRegCtrl|(BIT_SET_1(BIT_UPDATA_VAL));
}

U8 HAL_CPMADDLL_WaitDone(U8 bChan)
{
    U32 dCnt = 0x3FFF;
	volatile U32 *pdRegSts = (U32 *)REG_ADDLL_STS_ADDR(bChan);
	while(*pdRegSts != 0x3)
	{
        dCnt--;
        if (0 == dCnt)
        {
            return FALSE;
        }
	}
    return TRUE;
}

U32 HAL_CPMADDLL_GetVal(U8 bChan)
{
	U32 *pdReg = (U32 *)REG_ADDLL_DELAYVAL_ADDR(bChan);
	return (*pdReg);
}

void HAL_CPMDQS_HandleSetDelay(U8 bChan,U32 dDelayVal)
{
	U32 *pdReg = (U32 *)REG_DQS_DELAY_ADDR(bChan);
	*pdReg = dDelayVal;
}

U8 HAL_CPMDQS_AddllSetDelay(U8 bChan,U8 bMode,U32 dInitVal)
{
	#if 0
	U8 bPeriod =  HAL_CPMGetTrx2Period(bChan);
	if(bPeriod <= 6)//时钟源是400M的时候，必须是6分频以下才能使用ADDLL 功能
	{
		HAL_CPMADDLL_Stop(bChan);
		HAL_CPMADDLL_Init(bChan,bMode,dInitVal);
		HAL_CPMADDLL_Start(bChan);
		U8 bFlagDn	= HAL_CPMADDLL_WaitDone(bChan);
		
		if(bFlagDn)
		{
			HAL_CPMADDLL_Updata(bChan);
		}
		else 
		{
			return FALSE;
		}
		
		U32 dDelayVal =  HAL_CPMADDLL_GetVal(bChan);
		HAL_CPMDQS_HandleSetDelay(bChan,dDelayVal);
	}
	#endif
	
	return TRUE;
}

#define HAL_TRX2ClkCfg_Ch(ch,bSouceSel,bPeriod) {\
                                                 SG_CPM->rSOFT_GATE0.bits.FC##ch##_TRX2_CLK=0;\
                                                 SG_CPM->rTRX2_CLK_SWITCH_EN.bits.bVAL = 0;\
                                                 SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL = bPeriod;\
                                                 Delay(50);\
                                                 SG_CPM->rTRX2_CLK_SWITCH_EN.bits.bVAL = 1;\
                                                 Delay(50);\
                                                 SG_CPM->rRX_CLK_SWITCH_EN.bits.bVAL = 0;\
                                                 SG_CPM->rRX_CLK_PERIOD.bits.dVAL = bPeriod*2;\
                                                 Delay(50);\
                                                 SG_CPM->rRX_CLK_SWITCH_EN.bits.bVAL = 1;\
                                                 Delay(50);\
                                                 SG_CPM->rSOFT_GATE0.dAll|=0xFFFFFFFF;\
                                                 SG_CPM->rTRX2_CLK_SOURCE_SEL.bits.dVAL = bSouceSel;\
                                                 SG_CPM->rRX_CLK_SOURCE_SEL.bits.dVAL = bSouceSel;\
                                                }
#if 0//def __FW__
void  __section(".CODE_INIT") HAL_SystemInit(U32 dSysClkSourSel, U32 dSysClkPeriod,U08 bTrx2ClkSourSel,
		U08 bTrx2ClkPeriod0, U08 bTrx2ClkPeriod1,
		U08 bTrx2ClkPeriod2, U08 bTrx2ClkPeriod3)
#else
void HAL_SystemInit(U32 dSysClkSourSel,  U32 dSysClkPeriod,U08 bTrx2ClkSourSel,
					U08 bTrx2ClkPeriod0, U08 bTrx2ClkPeriod1,
					U08 bTrx2ClkPeriod2, U08 bTrx2ClkPeriod3)
#endif
{
	//SG_CPM->rWDT0_EN = 0xAA; //关闭G0WDT
	//SG_CPM->rWDT1_EN = 0xAA; //关闭G3WDT

#ifdef __DUAL_CORE__
	SG_CPM->rSYS_CLK_SWITCH_EN = 0;
	SG_CPM->rSYS_CLK_PERIOD = dSysClkPeriod;//SYSCLK_PERIOD_SEL
	SG_CPM->rSYS_CLK_SWITCH_EN = 1;
	SG_CPM->rSYS_CLK_SOURCE_SEL = dSysClkSourSel;
#ifdef __G3_FLYCODE
	SG_CPM->rLDEC_CLK_SOURCE_SEL = dSysClkSourSel;
	#ifdef	__FPGA_PLATEFORM
        SG_CPM->rLDEC_CLK_PERIOD = dSysClkPeriod;
	#else
	SG_CPM->rLDEC_CLK_PERIOD = dSysClkPeriod;
	#endif

	HAL_TRX2ClkCfg_Ch(0,dSysClkSourSel,bTrx2ClkPeriod0);//TRX2CLK_PERIOD_SEL
	HAL_TRX2ClkCfg_Ch(1,dSysClkSourSel,bTrx2ClkPeriod1);//TRX2CLK_PERIOD_SEL
	HAL_TRX2ClkCfg_Ch(2,dSysClkSourSel,bTrx2ClkPeriod2);//TRX2CLK_PERIOD_SEL
	HAL_TRX2ClkCfg_Ch(3,dSysClkSourSel,bTrx2ClkPeriod3);//TRX2CLK_PERIOD_SEL
#endif

#else
	SG_CPM->rSYS_CLK_SWITCH_EN = 0;
	SG_CPM->rSYS_CLK_PERIOD = dSysClkPeriod;//SYSCLK_PERIOD_SEL
	SG_CPM->rSYS_CLK_SWITCH_EN = 1;
	SG_CPM->rSYS_CLK_SOURCE_SEL = dSysClkSourSel;

	SG_CPM->rLDEC_CLK_SOURCE_SEL = dSysClkSourSel;
	SG_CPM->rLDEC_CLK_PERIOD = 2;

	HAL_TRX2ClkCfg_Ch(0,dSysClkSourSel,bTrx2ClkPeriod0);//TRX2CLK_PERIOD_SEL
	HAL_TRX2ClkCfg_Ch(1,dSysClkSourSel,bTrx2ClkPeriod1);//TRX2CLK_PERIOD_SEL
	HAL_TRX2ClkCfg_Ch(2,dSysClkSourSel,bTrx2ClkPeriod2);//TRX2CLK_PERIOD_SEL
	HAL_TRX2ClkCfg_Ch(3,dSysClkSourSel,bTrx2ClkPeriod3);//TRX2CLK_PERIOD_SEL
#endif//#ifdef __DUAL_CORE

	UpdateSystemClock();
}
