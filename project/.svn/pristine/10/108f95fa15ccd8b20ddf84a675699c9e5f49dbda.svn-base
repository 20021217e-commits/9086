// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  pad.c
//  文件标识：  
//  内容摘要：  pad 相关操作实现
//  其它说明：  
//  当前版本：  v0.01
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年11月10日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "sg9081.h"
#include "hal.h"
#include "uitility.h"
// *****************************************************************************
// 函数名称: PAD_Dqs_PullEnCtrl
// 函数功能: 设置dqs上拉是否使能
// 入口参数: bChNum：通道号
//          bMode:是否使能，1：上下拉使能
//						   0：使能关闭
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADDqsPullEnCtrl(U8 bChNum,U8 bMode)
{
	SG_PADCTRL->rPAD_PE_CTRL[bChNum].bits.FDQS = bMode;
}

// *****************************************************************************
// 函数名称: PAD_Dqs_Func
// 函数功能: 设置dqs的模式
// 入口参数: bChNum：通道号
//          bMode:是否使能，1：GPIO模式
//						   0：FC模式
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADDqsFunc(U8 bChNum,U8 bMode)
{
    SG_PADCTRL->rPAD_FUNC_CTRL[bChNum].bits.FDQS = bMode;
}

// *****************************************************************************
// 函数名称: PAD_RB_Func
// 函数功能: 设置RB的模式
// 入口参数:  bMode:是否使能，1：GPIO模式
//						     0：FC模式
//         
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADRBFunc(U8 bMode)
{
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB = bMode;
}

// *****************************************************************************
// 函数名称: Pad_Ssel_Ch
// 函数功能: 设置输出加强模式
// 入口参数: bPadNum：pad号
//          dVal:寄存器的值
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADSSelCtrl0to3(U8 bPadNum,U32 dVal)
{
	if(bPadNum >3)
	{
		return;
	}
	SG_PADCTRL->rPAD_SSEL_CTRL[bPadNum].dAll = dVal;
}

void HAL_PADSSelCtrl4(U32 dVal)
{
	SG_PADCTRL->rPAD_SSEL_CTRL4.dAll = dVal;
}


U32 HAL_PADTSelGet(void)
{
	U32 dReturnVal = 0;
	dReturnVal = SG_PADCTRL->rPAD_TSEL_CTRL0.dAll;
	return (dReturnVal&0x00FFFFFF);/*低24位有效*/
}

void HAL_PADTSelCtrl(U32 dVal)
{
	SG_PADCTRL->rPAD_TSEL_CTRL0.dAll = dVal;
}

void HAL_PADSTCtrl(U32 dVal)
{
	SG_PADCTRL->rPAD_ST_CTRL.dAll = dVal;
}
// *****************************************************************************
// 函数名称: PAD_PullEn0to3
// 函数功能: 设置上下拉和使能
// 入口参数: bCh：通道 0,1,...
//          dVal1:设置是否拉高拉低   1：表示可以拉高拉低、0：表示不可以拉高拉低
//          dVal2:设置拉高还是拉低   1：表示拉高、       0：表示拉低 
//          每一bit均表示一个pad脚
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADPullEn0to3(U8 bPadNum,U32 dVal1,U32 dVal2)
{

	SG_PADCTRL->rPAD_PE_CTRL[bPadNum].dAll = dVal1;
	SG_PADCTRL->rPAD_PS_CTRL[bPadNum].dAll = dVal2;
}
// *****************************************************************************
// 函数名称: Pad_PullEn
// 函数功能: 设置上下拉和使能
// 入口参数: dVal:寄存器的值
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADPullEn4(U32 dVal1,U32 dVal2)
{
	SG_PADCTRL->rPAD_PE_CTRL4.dAll = dVal1;
	SG_PADCTRL->rPAD_PS_CTRL4.dAll = dVal2;
}

void HAL_PADPullEn5(U32 dVal1,U32 dVal2)
{
	SG_PADCTRL->rPAD_PE_CTRL5.dAll = dVal1;
	SG_PADCTRL->rPAD_PS_CTRL5.dAll = dVal2;
}
// *****************************************************************************
// 函数名称: PAD_FuncCtrl0to3
// 函数功能: 设置pad模式，GPIO和FUNC模式
// 入口参数: bCh：通道 0,1,...
//          dVal:寄存器的值
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADFuncCtrl0to3(U8 bCh, U32 dVal)
{
	SG_PADCTRL->rPAD_FUNC_CTRL[bCh].dAll = dVal;
}

void HAL_PADFuncCtrl4(U32 dVal)
{
	SG_PADCTRL->rPAD_FUNC_CTRL4.dAll = dVal;
}

void HAL_PADSetPortMode(U8 bPortNum,U32 dIOMode)
{

	SG_PADCTRL->rPORT_MODE[bPortNum].dAll = dIOMode;
}

void HAL_PADSetPortData(U8 bPortNum, U32 dVal)
{

	SG_PADCTRL->rPORT_DATA[bPortNum].dAll = dVal;
}

void HAL_PADPortDataInOut(U8 bPortNum,U32 dIOMode ,U32 dVal)
{

    HAL_PADSetPortMode(bPortNum,dIOMode);
    HAL_PADSetPortData(bPortNum,dVal);
}
// *****************************************************************************
// 函数名称: Pad_SpiModeSel
// 函数功能: 设置SPI pad模式，GPIO和FUNC模式
// 入口参数: bMode： 0: Function Mode,1: GPIO Mode
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADSpiModeSel(U8 bMode)
{
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPICK = bMode;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPICS = bMode;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID0 = bMode;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID1 = bMode;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID2 = bMode;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID3 = bMode;
}

void HAL_PADUartModeSel(U8 bMode)
{
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTRX = bMode;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTTX = bMode;
}

void HAL_PADUartCfg(U8 bCfg)
{
	SG_PADCTRL->rUART_CFG.dAll = bCfg;
}

void HAL_PADLEDxModeSel(U8 bNum,U8 bMode)
{
	if(0 == bNum)
	{
		SG_PADCTRL->rPAD_FUNC_CTRL4.bits.LED0 = bMode;
		SG_PADCTRL->rPAD_SSEL_CTRL4.bits.LED0=4;//增加LED驱动能力
	}
	else if(1 == bNum)
	{
	    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.LED1 = bMode;
		SG_PADCTRL->rPAD_SSEL_CTRL4.bits.LED1=4;//增加LED驱动能力
	}
}
// *****************************************************************************
// 函数名称: PAD_RomcModeSel
// 函数功能: 设置romc上下拉模式，调用会上下拉使能
// 入口参数: bMode： 0: pull down,1: pull up
// 出口参数: 
// 说    明: 
// *****************************************************************************
void HAL_PADRomcModeSel(U8 bMode)
{
	SG_PADCTRL->rPAD_PE_CTRL4.bits.ROMC = PULLUP_EN;
	SG_PADCTRL->rPAD_PS_CTRL4.bits.ROMC = PULL_UP;
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.ROMC   = bMode; //使能ROMC 模式

}


// *****************************************************************************
// 函数名称: PAD_GetData
// 函数功能: 获取port的值
// 入口参数: port编号
// 出口参数: port中的值
// 说    明: 
// *****************************************************************************
U32 HAL_PADGetPortData(U8 bPortNum)
{
	U32 dRet;
	if (16 == bPortNum)
	{
		return SG_PADCTRL->rPORT_DATA16.dAll;
	}
	else if(17 == bPortNum)
	{
		return SG_PADCTRL->rPORT_DATA17.dAll;
	}
	dRet = SG_PADCTRL->rPORT_DATA[bPortNum].dAll;
	return dRet;
}

// *****************************************************************************
// 函数名称: LED_Config
// 函数功能: 配置LED功能
// 入口参数: bNum：LED 编号 0,1,...
//          bMode:工作模式 ON/OFF
//        bPeriod:闪烁周期
// 出口参数:
// 说    明:
// *****************************************************************************
void HAL_LED_Config(U8 bNum,U8 bMode,U16 bPeriod)
{
#if 1
	switch(bNum)
	{
	case LED0_NUM:
		SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=0;
		SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED0_CTRL_PERIOD=bPeriod;
		//PAD_LEDxModeSel(LED0_NUM,!bMode);
        HAL_PADLEDxModeSel(LED0_NUM,!bMode);
		SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=bMode;
		break;
	case LED1_NUM:
		SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=0;
		SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED1_CTRL_PERIOD=bPeriod;
		HAL_PADLEDxModeSel(LED1_NUM,!bMode);
		SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=bMode;
		break;
	default:
		break;
	}
#else
  //LED0/1 驱动
  SG_PADCTRL->rPAD_FUNC_CTRL4.dAll|=(3<<16);//改成GPIO 模式，不闪烁
  SG_PADCTRL->rPORT_MODE[12].dAll&=0x9F;//clear bit 5/6 out put mode
  SG_PADCTRL->rPORT_DATA[12].dAll&=0x9F;//clear bit 5/6 off mode
  //可能存在肉眼观察不到的闪烁
  __nds32__dsb();
  __nds32__dsb();
  __nds32__dsb();
  SG_PADCTRL->rPORT_DATA[12].dAll|= (bMode<<(5+bNum));
#endif

}


U08 HAL_GetCurrentSensor(void)
{
	U08 wTemp = 0;
	U08 bRegCount = 0;
	bRegCount = SG_PADCTRL->rAIP_STS.bits.TS_OUT_SP;
	wTemp = bRegCount * 5;
	//SG_PADCTRL->rAIP_CTRL.dAll |= (1 << 7);
	return wTemp;
}


U32 dEfuseDoutL	= 0;
U32 dEfuseDoutH	= 0;

void HAL_EFUSE_Read(void)
{
	U08 i = 0;
	
	SG_PADCTRL->rEFUSE_CTRL.bits.CS = 0;
	//SG_PADCTRL->rEFUSE_CTRL.bits.RW = 1;
	SG_PADCTRL->rEFUSE_CTRL.bits.SCLK = 0;

	SG_PADCTRL->rEFUSE_CTRL.bits.RW = 0;
	__nop();
	SG_PADCTRL->rEFUSE_CTRL.bits.CS = 1;
	__nop();
	
	for (i = 0; i < 32; i++)
	{
		SG_PADCTRL->rEFUSE_CTRL.bits.SCLK = 1;
		__nop16();__nop8();
		SG_PADCTRL->rEFUSE_CTRL.bits.SCLK = 0;
		__nop();

		dEfuseDoutL |= SG_PADCTRL->rEFUSE_DOUT.bits.dVAL << i;
		__nop8();
	}

	for (i = 0; i < 32; i++)
	{
		SG_PADCTRL->rEFUSE_CTRL.bits.SCLK = 1;
		__nop16();__nop8();
		SG_PADCTRL->rEFUSE_CTRL.bits.SCLK = 0;
		__nop();

		dEfuseDoutH |= SG_PADCTRL->rEFUSE_DOUT.bits.dVAL << i;
		__nop8();
	}
	SG_PADCTRL->rEFUSE_CTRL.bits.CS = 0;
	__nop();
	SG_PADCTRL->rEFUSE_CTRL.bits.RW = 1;
}
#ifdef __FW__
void  __section(".CODE_INIT") HAL_VDT_Calibration(void)
#else
void HAL_VDT_Calibration(void)
#endif
{
	//dEfuseDoutL	= 0x810;							//不用CP的值去校准VDT和sensor，改用默认值0x810去校准,vdt 温度一起校准，只需要校准一次
	//U32 dVDTvalue = dEfuseDoutL&0xFFF;
	SG_PADCTRL->rVDT_CT.bits.dVAL &= (~0xFFF);
	SG_PADCTRL->rVDT_CT.bits.dVAL |= 0x810;		//efuse_dout[11:5]写入到vdt_ct[11:5]
	while ((SG_PADCTRL->rVDT_CT.bits.dVAL & 0xFFF) != 0x810);
	delay_ms(50);//等待校准完成
}

U08 HAL_GetTemperature(void)
{
	return SG_PADCTRL->rAIP_STS.bits.TS_OUT_SP;
}

#ifdef __FW__
void  __section(".CODE_INIT") HAL_VDT_Init(U08 bMode, U08 bLevel)
#else
void HAL_VDT_Init(U08 bMode, U08 bLevel)
#endif
{
	SG_PADCTRL->rAIP_CTRL.bits.VDT_WLEVEL = bLevel;
	SG_PADCTRL->rAIP_CTRL.bits.VDT_VMODE  = bMode;
	SG_PADCTRL->rAIP_CTRL.bits.VDT_PWDB   = 1;

	delay_us(100);
	//如果不加delay，之前由于VDT_WLEVEL=3.3V,low状态一直在，VDT_WLEVEL一设置为2.7会有一个rising状态延时出现，后面的清零不起作用
	SG_CPM->rCPM_INT_STS.bits.VDT_WARN_FALL_INT_STS = 0;
	SG_CPM->rCPM_INT_STS.bits.VDT_WARN_RISE_INT_STS = 0;
	SG_CPM->rCPM_INT_STS.bits.VDT_LOW_INT_STS = 0;
	SG_CPM->rVDT_CNT = 0;

	SG_CPM->rVDT_LOW_THRESHOLD = 0x0F;	//16us

	CPM_VDT_FALLINT_EN();//
	CPM_VDT_LOWINT_EN();//
	CPM_VDT_RISEINT_EN();

	NVIC_EnableIRQ(IRQ_CPM);//初始化完以后再打开VDT中断
}
void HAL_GPIOHighOrLow(U8 bGPIONum,U8 bState)
{
	if(bState == 0)
	{
		SG_PADCTRL->rPORT_DATA16.bits.dVAL &=(BIT_CLR_0(bGPIONum-8)); //设置为输出模式
	}
	else
	{
		SG_PADCTRL->rPORT_DATA16.bits.dVAL |=(BIT_SET_1(bGPIONum-8)); //设置为输出模式
	}
}

void HAL_GPIOInit(void)
{
	SG_PADCTRL->rPORT_MODE16.bits.dVAL &=(BIT_CLR_0(0)); //设置为输出模式
	SG_PADCTRL->rPORT_MODE16.bits.dVAL &=(BIT_CLR_0(1)); //设置为输出模式
	
	SG_PADCTRL->rPORT_DATA16.bits.dVAL &=(BIT_CLR_0(0)); //设置为输出模式
	SG_PADCTRL->rPORT_DATA16.bits.dVAL &=(BIT_CLR_0(1)); //设置为输出模式
	
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO8 = GPIO_MODE; //设置为GPIO模式
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO9 = GPIO_MODE; //设置为GPIO模式

	HAL_GPIOHighOrLow(8,1);
	HAL_GPIOHighOrLow(9,1);
}

//通过GPIO 8或者9发送指定数目的脉冲
void HAL_GPIODebug(U8 bGPIONum,U16 bPulseCnt)
{
	while(bPulseCnt)
	{
	 HAL_GPIOHighOrLow(bGPIONum,1);
	 Delay(1);
	 HAL_GPIOHighOrLow(bGPIONum,0);
	 Delay(1);
		bPulseCnt--;
	}
	//HAL_GPIOHighOrLow(bGPIONum,0);
}
void HAL_GPIOValDebug(U8 bData)
{
	U16 i = 8;
	while (i--)
	{
		if ((bData >> i) & 0x01)
		{
			SG_PADCTRL->rPORT_DATA16.bits.dVAL |=1; //设置为输出模式bit1:clk ==1
			Delay(0);
			SG_PADCTRL->rPORT_DATA16.bits.dVAL |=2; //设置为输出模式bit1:clk ==1
			Delay(0);
			SG_PADCTRL->rPORT_DATA16.bits.dVAL &=~2; //设置为输出模式clk == 0
			Delay(0);
		}
		else
		{
			SG_PADCTRL->rPORT_DATA16.bits.dVAL &=~1; //设置为输出模式
			Delay(0);
			SG_PADCTRL->rPORT_DATA16.bits.dVAL |=2; //设置为输出模式 bit1:clk  bit0==0
			Delay(0);
			SG_PADCTRL->rPORT_DATA16.bits.dVAL &=~2; //设置为输出模式
			Delay(0);
		}
	}
	SG_PADCTRL->rPORT_DATA16.bits.dVAL |=3; //设置为输出模式
	Delay(1);
}
void HAL_GPIO8Debug(U16 bPulseCnt)
{
  while(bPulseCnt--)
  {
   SG_PADCTRL->rPORT_DATA16.bits.dVAL = 1;  //Port16.0
   __nds32__nop();
   SG_PADCTRL->rPORT_DATA16.bits.dVAL = 0;
  }
}
void HAL_GPIO9Debug(U16 bPulseCnt)
{
  while(bPulseCnt--)
  {
     SG_PADCTRL->rPORT_DATA16.bits.dVAL = 2;  //Port16.1
     __nds32__nop();
     SG_PADCTRL->rPORT_DATA16.bits.dVAL = 0;
  }
}
