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
#include "pad.h"
#include "uitility.h"
#include "sata_typedef.h"
//#ifndef __BOOT_CODE__
//#define __FPGA_PLATEFORM
//#else
//#endif

#if 0
	#ifndef __FPGA_PLATEFORM
	#define LED_PERIOD    (tSataDC.dMicroSecondCnt)	//200M 1.31ms
	#else
	#define LED_PERIOD    (tSataDC.dMicroSecondCnt>>2) 	//FPGA和实际芯片相差4倍 1.31ms*4
	#endif
#endif
// *****************************************************************************
// 函数名称: PAD_Dqs_PullEnCtrl
// 函数功能: 设置dqs上拉是否使能
// 入口参数: bChNum：通道号
//          bMode:是否使能，1：上下拉使能
//						   0：使能关闭
// 出口参数: 
// 说    明: 
// *****************************************************************************
void PAD_Dqs_PullEnCtrl(U8 bChNum,U8 bMode)
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
void PAD_Dqs_Func(U8 bChNum,U8 bMode)
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
void PAD_RB_Func(U8 bMode)
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
void PAD_SSelCtrl0to3(U8 bPadNum,U32 dVal)
{
    if(bPadNum >3)
    {
        return;
    }
    SG_PADCTRL->rPAD_SSEL_CTRL[bPadNum].dAll = dVal;
}

void PAD_SSelCtrl4(U32 dVal)
{
    SG_PADCTRL->rPAD_SSEL_CTRL4.dAll = dVal;
}

void PAD_TSelCtrl(U32 dVal)
{
    SG_PADCTRL->rPAD_TSEL_CTRL0.dAll = dVal;
}

void PAD_STCtrl(U32 dVal)
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
void PAD_PullEn0to3(U8 bPadNum,U32 dVal1,U32 dVal2)
{
    if(bPadNum >3)
    {
        return;
    }
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
void PAD_PullEn4(U32 dVal1,U32 dVal2)
{
    SG_PADCTRL->rPAD_PE_CTRL4.dAll = dVal1;
    SG_PADCTRL->rPAD_PS_CTRL4.dAll = dVal2;
}

void PAD_PullEn5(U32 dVal1,U32 dVal2)
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
void PAD_FuncCtrl0to3(U8 bCh, U32 dVal)
{
    if(bCh >3)
    {
        return;
    }
    SG_PADCTRL->rPAD_FUNC_CTRL[bCh].dAll = dVal;
}

void PAD_FuncCtrl4(U32 dVal)
{
    SG_PADCTRL->rPAD_FUNC_CTRL4.dAll = dVal;
}

void PAD_PortMode(U8 bPortNum,U32 dIOMode)
{
    if (bPortNum>16)
    {
        return;
    }
    if(16 == bPortNum)
    {
    	SG_PADCTRL->rPORT_MODE16.dAll |= dIOMode;
    }
    SG_PADCTRL->rPORT_MODE[bPortNum].dAll |= dIOMode;
}

void PAD_FXWPCtrl(U8 bMode)
{
    SG_PADCTRL->rPORT_MODE[15].bits.dVAL &= 0xBF;
    if(bMode == 1)
    {
        SG_PADCTRL->rPORT_DATA[15].bits.dVAL &= 0xBF;
    }
    else
    {
        SG_PADCTRL->rPORT_DATA[15].bits.dVAL |= 0x40;
    }
}

void PAD_PortData(U8 bPortNum, U32 dVal)
{
    if (bPortNum>16)
    {
        return;
    }
    if (16 == bPortNum)
    {
    	SG_PADCTRL->rPORT_DATA16.dAll = dVal;
    }
    SG_PADCTRL->rPORT_DATA[bPortNum].dAll = dVal;
}

void PAD_PortData_InOut(U8 bPortNum,U32 dIOMode ,U32 dVal)
{
//    if (bPortNum>16)
//    {
//        return;
//    }
    PAD_PortMode(bPortNum,dIOMode);
    PAD_PortData(bPortNum,dVal);
}
// *****************************************************************************
// 函数名称: Pad_SpiModeSel
// 函数功能: 设置SPI pad模式，GPIO和FUNC模式
// 入口参数: bMode： 0: Function Mode,1: GPIO Mode
// 出口参数: 
// 说    明: 
// *****************************************************************************
void PAD_SpiModeSel(U8 bMode)
{
	if(0 == bMode)
	{
		SG_PADCTRL->rPAD_FUNC_CTRL4.dAll &= ~(0x3F << 10);	//SPI Function mode
	}
//    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPICK = bMode;
//    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPICS = bMode;
//    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID0 = bMode;
//    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID1 = bMode;
//    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID2 = bMode;
//    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.SPID3 = bMode;
}

void PAD_UartModeSel(U8 bMode)
{
    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTRX = bMode;
    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTTX = bMode;
}

void PAD_PowerDownCfg(void)
{
    SG_PADCTRL->rPAD_PE_CTRL[0].dAll=0x00000fff;
    SG_PADCTRL->rPAD_PE_CTRL[1].dAll=0x00000fff;
    SG_PADCTRL->rPAD_PE_CTRL[2].dAll=0x00000fff;
    SG_PADCTRL->rPAD_PE_CTRL[3].dAll=0x00000fff;
    SG_PADCTRL->rPAD_PE_CTRL4.dAll=0x00ffffa7;
    SG_PADCTRL->rPAD_PE_CTRL5.dAll=0x00000fff;
    SG_PADCTRL->rPAD_PS_CTRL[0].dAll=0x0000059f;
    SG_PADCTRL->rPAD_PS_CTRL[1].dAll=0x0000059f;
    SG_PADCTRL->rPAD_PS_CTRL[2].dAll=0x0000059f;
    SG_PADCTRL->rPAD_PS_CTRL[3].dAll=0x0000059f;
    SG_PADCTRL->rPAD_PS_CTRL4.dAll=0x00ffffa5;
    SG_PADCTRL->rPAD_PS_CTRL5.dAll=0x00000330;

    Mem32SetVal((U32*)SG_PADCTRL->rPORT_MODE,0xff,15);

    SG_PADCTRL->rPORT_MODE[15].dAll=0xbf;
    SG_PADCTRL->rPORT_MODE16.dAll=0xff;
    SG_PADCTRL->rPORT_MODE17.dAll=0xff;

    SG_PADCTRL->rPAD_FUNC_CTRL[0].dAll=0x000003ff;
    SG_PADCTRL->rPAD_FUNC_CTRL[1].dAll=0x000003ff;
    SG_PADCTRL->rPAD_FUNC_CTRL[2].dAll=0x000003ff;
    SG_PADCTRL->rPAD_FUNC_CTRL[3].dAll=0x000003ff;
    SG_PADCTRL->rPAD_FUNC_CTRL4.dAll=0x07fffff3;
    SG_PADCTRL->rPAD_MISC_CTRL.dAll=0x00000020;
    SG_PADCTRL->rAIP_CTRL.dAll=0x00000f1f;
}

void PAD_LEDxModeSel(U8 bNum,U8 bMode)
{
    if(0 == bNum)
    {
        SG_PADCTRL->rPAD_FUNC_CTRL4.bits.LED0 = bMode;
        SG_PADCTRL->rPAD_SSEL_CTRL4.bits.LED0=4;//增加LED驱动能力
        SG_PADCTRL->rPORT_MODE[12].dAll &=~(1<<5);//led0 p12.5 gpio MODE OUTPUT 0

#ifdef  __FPGA_PLATEFORM
        SG_PADCTRL->rPORT_DATA[12].dAll |=(1<<5);//FPGA输出0是亮
#else
        SG_PADCTRL->rPORT_DATA[12].dAll &=~(1<<5);//实际芯片输出1是亮
#endif

    }
    else if(1 == bNum)
    {
        SG_PADCTRL->rPAD_FUNC_CTRL4.bits.LED1 = bMode;
        SG_PADCTRL->rPAD_SSEL_CTRL4.bits.LED1=4;//增加LED驱动能力
        SG_PADCTRL->rPORT_MODE[12].dAll &=~(1<<6);//led1 p12.6  gpio MODE OUTPUT 0

#ifdef  __FPGA_PLATEFORM
        SG_PADCTRL->rPORT_DATA[12].dAll |=(1<<6);//FPGA输出0是亮
#else
        SG_PADCTRL->rPORT_DATA[12].dAll &=~(1<<6);//实际芯片输出1是亮
#endif

    }
    else if(2 == bNum)
    {
//	    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO1 = bMode;
//		SG_PADCTRL->rPAD_SSEL_CTRL4.bits.GPIO1=4;//增加LED驱动能力
    }
    else if(3 == bNum)
    {
//	    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO2 = bMode;
//		SG_PADCTRL->rPAD_SSEL_CTRL4.bits.GPIO2=4;//增加LED驱动能力
    }
}
// *****************************************************************************
// 函数名称: PAD_RomcModeSel
// 函数功能: 设置romc上下拉模式，调用会上下拉使能
// 入口参数: bMode： 0: pull down,1: pull up
// 出口参数: 
// 说    明: 
// *****************************************************************************
void PAD_RomcModeSel(U8 bMode)
{
    SG_PADCTRL->rPAD_PE_CTRL4.bits.URTTX = PULLUP_EN;
    SG_PADCTRL->rPAD_PS_CTRL4.bits.URTTX = PULL_UP;
    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTTX   = bMode; //使能ROMC 模式

}

// *****************************************************************************
// 函数名称: PAD_Func_Init
// 函数功能: 将四个通道都设置成func模式
// 入口参数:
// 出口参数: 
// 说    明: 
// *****************************************************************************
void PAD_Func_Init(void)
{

    //CH0 pad initial
    //rb0 for fio
    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB	= 0;	//fxrb - fio mode

    SG_PADCTRL->rPAD_FUNC_CTRL[0].dAll			= 0;
    SG_PADCTRL->rPAD_FUNC_CTRL[1].dAll			= 0;
    SG_PADCTRL->rPAD_FUNC_CTRL[2].dAll			= 0;
    SG_PADCTRL->rPAD_FUNC_CTRL[3].dAll			= 0;
// 
// 	//for test
// 	SG_PADCTRL->rPORT_MODE9.bits.dVAL		= 0 ;	//gpio9 out mode
// 	SG_PADCTRL->rPORT_DATA9.bits.dVAL		= 0x0;	//gpio out low
// 	SG_PADCTRL->rPAD_FUNC_CTRL3.bits.FDQ		= 1;	//0:fio; 1-gpio	
}
// *****************************************************************************
// 函数名称: PAD_GetData
// 函数功能: 获取port的值
// 入口参数: port编号
// 出口参数: port中的值
// 说    明: 
// *****************************************************************************
U32 PAD_GetData(U8 bPortNum)
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
#if 0	//将RDT和boot的LED配置合并
	#ifdef __RDT_SATA_CTRL  //RDT需要灵活配置各灯闪烁频率
	void LED_Config(U8 bNum,U8 bMode,U16 bPeriod)
	{
		switch(bNum)
		{
			case LED0_NUM:
				SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=0;
				SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED0_CTRL_PERIOD=(bPeriod);//50ms
				PAD_LEDxModeSel(LED0_NUM,!bMode);
				SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=bMode;
				break;
			case LED1_NUM:
				{
	//				U8 speed=SG_DSATA->rPSSTSR.bits.SPD;
	//				if(speed == 1)
	//				{
	//					bPeriod=(LED_PERIOD<<2);//1600ms
	//				}
	//				else if(speed == 2)
	//				{
	//					bPeriod=(LED_PERIOD);//400ms
	//				}
	//				else if(speed == 3)
	//				{
	//					bPeriod=(LED_PERIOD>>3);//50ms
	//				}
	//				else
	//				{
	//					bPeriod=(LED_PERIOD>>3);//50ms
	//				}
	//
	//				SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=0;
	//				SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED1_CTRL_PERIOD=bPeriod;
	//				PAD_LEDxModeSel(LED1_NUM,!bMode);
	//				SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=bMode;
					SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=0;
					SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED1_CTRL_PERIOD=(bPeriod);//50ms
					PAD_LEDxModeSel(LED1_NUM,!bMode);
					SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=bMode;
					break;
				}
				break;
	//        case LED2_NUM:
	//            SG_MCU->rREG_LED_CTRL.bits.LED2_CTRL_EN=0;
	//            SG_MCU->rREG_LED_CTRL1_PERIOD.bits.LED2_CTRL_PERIOD=bPeriod;
	//            PAD_LEDxModeSel(LED2_NUM,!bMode);
	//            SG_MCU->rREG_LED_CTRL.bits.LED2_CTRL_EN=bMode;
	//            break;
	//        case LED3_NUM:
	//            SG_MCU->rREG_LED_CTRL.bits.LED3_CTRL_EN=0;
	//            SG_MCU->rREG_LED_CTRL1_PERIOD.bits.LED3_CTRL_PERIOD=bPeriod;
	//            PAD_LEDxModeSel(LED3_NUM,!bMode);
	//            SG_MCU->rREG_LED_CTRL.bits.LED3_CTRL_EN=bMode;
	//            break;
			default:
				break;
		}
	}
	#endif
#endif

//BOOT code模式和scan code模型下的LED闪烁配置一致，目前没有用到LED2和LED3，故暂时屏蔽。
#if ((defined  __BOOT_CODE__) || (defined  __RDT_SATA_CTRL))
void LED_Config(U8 bNum,U8 bMode,U16 bPeriod)
{
	switch(bNum)
	{
		case LED0_NUM:
			SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=0;
			SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED0_CTRL_PERIOD=bPeriod;
			PAD_LEDxModeSel(LED0_NUM,!bMode);
			SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=bMode;
			break;
		case LED1_NUM:
			SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=0;
			SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED1_CTRL_PERIOD=bPeriod;
			PAD_LEDxModeSel(LED1_NUM,!bMode);
			SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=bMode;
			break;
//		case LED2_NUM:
//			SG_MCU->rREG_LED_CTRL.bits.LED2_CTRL_EN=0;
//			SG_MCU->rREG_LED_CTRL1_PERIOD.bits.LED2_CTRL_PERIOD=bPeriod;
//			PAD_LEDxModeSel(LED2_NUM,!bMode);
//			SG_MCU->rREG_LED_CTRL.bits.LED2_CTRL_EN=bMode;
//			break;
//		case LED3_NUM:
//			SG_MCU->rREG_LED_CTRL.bits.LED3_CTRL_EN=0;
//			SG_MCU->rREG_LED_CTRL1_PERIOD.bits.LED3_CTRL_PERIOD=bPeriod;
//			PAD_LEDxModeSel(LED3_NUM,!bMode);
//			SG_MCU->rREG_LED_CTRL.bits.LED3_CTRL_EN=bMode;
//			break;
		default:
			break;
	}
}
#else
void LED_Config(U8 bNum,U8 bMode,U16 bPeriod)
{
	//LED0/1 驱动
	SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=0;
	SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=0;
	SG_PADCTRL->rPAD_FUNC_CTRL4.dAll|=(3<<16);//改成GPIO 模式，不闪烁
	SG_PADCTRL->rPORT_MODE[12].bits.dVAL&=(~(1<<(5+bNum)));//clear bit 5/6 out put mode
	SG_PADCTRL->rPORT_DATA[12].bits.dVAL&=(~(1<<(5+bNum)));//clear bit 5/6 off mode
	delay_us(1);//不delay的话，可能灭不掉灯
	SG_PADCTRL->rPORT_DATA[12].bits.dVAL|= (bMode<<(5+bNum));
	#if 0
	switch(bNum)
	{
		case LED0_NUM:
			SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=0;
			SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED0_CTRL_PERIOD=(LED_PERIOD>>3);//50ms
			PAD_LEDxModeSel(LED0_NUM,!bMode);
			SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN=bMode;
			break;
		case LED1_NUM:
			{
				U8 speed=SG_DSATA->rPSSTSR.bits.SPD;
				if(speed == 1)
				{
					bPeriod=(LED_PERIOD<<2);//1600ms
				}
				else if(speed == 2)
				{
					bPeriod=(LED_PERIOD);//400ms
				}
				else if(speed == 3)
				{
					bPeriod=(LED_PERIOD>>3);//50ms
				}
				else
				{
					bPeriod=(LED_PERIOD>>3);//50ms
				}

				SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=0;
				SG_MCU->rREG_LED_CTRL0_PERIOD.bits.LED1_CTRL_PERIOD=bPeriod;
				PAD_LEDxModeSel(LED1_NUM,!bMode);
				SG_MCU->rREG_LED_CTRL.bits.LED1_CTRL_EN=bMode;
			}
			break;
//        case LED2_NUM:
//            SG_MCU->rREG_LED_CTRL.bits.LED2_CTRL_EN=0;
//            SG_MCU->rREG_LED_CTRL1_PERIOD.bits.LED2_CTRL_PERIOD=bPeriod;
//            PAD_LEDxModeSel(LED2_NUM,!bMode);
//            SG_MCU->rREG_LED_CTRL.bits.LED2_CTRL_EN=bMode;
//            break;
//        case LED3_NUM:
//            SG_MCU->rREG_LED_CTRL.bits.LED3_CTRL_EN=0;
//            SG_MCU->rREG_LED_CTRL1_PERIOD.bits.LED3_CTRL_PERIOD=bPeriod;
//            PAD_LEDxModeSel(LED3_NUM,!bMode);
//            SG_MCU->rREG_LED_CTRL.bits.LED3_CTRL_EN=bMode;
//            break;
		default:
			break;
	}
	#endif
}
#endif	//#ifdef __RDT_SATA_CTRL || __BOOT_CODE__
void PAD_VDT_Init(U8 bMode, U8 bLevel,U8 bVDTEn)
{
    NVIC_EnableIRQ(IRQ_CPM);

    SG_PADCTRL->rAIP_CTRL.bits.VDT_WLEVEL = bLevel;
    SG_PADCTRL->rAIP_CTRL.bits.VDT_VMODE  = bMode;
    SG_PADCTRL->rAIP_CTRL.bits.VDT_PWDB   = 1;

    SG_CPM->rCPM_INT_STS.bits.VDT_WARN_FALL_INT_STS = 0;
    SG_CPM->rCPM_INT_STS.bits.VDT_WARN_RISE_INT_STS = 0;
    SG_CPM->rCPM_INT_STS.bits.VDT_LOW_INT_STS = 0;
    SG_CPM->rVDT_CNT = 0;
    if(bVDTEn == 0)
    {
        SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_FALL_INT_EN = 0;
        SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_RISE_INT_EN = 0;
    }
//	SG_CPM->rVDT_LOW_THRESHOLD = 0x1000;	//4.096毫秒
    else
    {
        SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_FALL_INT_EN = 1;
        SG_CPM->rCPM_INT_CTRL.bits.VDT_WARN_RISE_INT_EN = 1;
    }
}

// *****************************************************************************
// 函数名称: PAD_SetCE
// 函数功能: 20200329 Tender要求在各CE从GPIO模式切换成Function模式前，
//			将PORT_DATA1/4/7/10设置成0xFF,以解决CE短暂为低电平的现象
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void PAD_SetCE()
{
	PAD_PortData(PORT_1, 0xFF);
	PAD_PortData(PORT_4, 0xFF);
	PAD_PortData(PORT_7, 0xFF);
	PAD_PortData(PORT_10, 0xFF);
}
