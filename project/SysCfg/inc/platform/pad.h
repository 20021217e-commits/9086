// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  pad.h
//  文件标识：  
//  内容摘要：  pad 相关寄存器配置接口定义
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
#ifndef __PAD__
#define __PAD__
/******************************包含头文件******************************/
#include "common.h"

/******************************类型和宏定义******************************/
#ifndef LED0_NUM
#define LED0_NUM    (0)
#endif
#ifndef LED1_NUM
#define LED1_NUM    (1)
#endif
#ifndef LED2_NUM
#define LED2_NUM    (2)
#endif
#ifndef LED3_NUM
#define LED3_NUM    (3)
#endif

#ifndef LED_MODE_OFF
#define LED_MODE_OFF    (0)
#define LED_MODE_ON     (!LED_MODE_OFF)
#endif

// rPAD_FUNC_CTRL
#define  MODE_OUTPUT              (0x00)    //GPIO模式为输出模式
#define  MODE_INPUT               (0xFF)    //GPIO模式为输入模式


#define  PULLUP_DIS             (0)
#define  PULLUP_EN              (1)

#define PULL_DOWN               (0)
#define PULL_UP                 (1)

#define FXWP_EN                 (1)
#define FXWP_DIS                (0)


#define PAD_PS0to3_REG_DEFAULTS     (0x0000059F)
#define PAD_PE4_REG_DEFAULTS        (0x00FFFFA7)
#define PAD_PS4_REG_DEFAULTS        (0x00FFFFA3)
#define PAD_PS5_REG_DEFAULTS        (0x00000330)

#define  DQS_FC_MODE            (0)
#define  DQS_GPIO_MODE          (1)

#define  MODE_FUNC (0)
#define  MODE_GPIO (1)
#define  MODE_PULLEN            (1)
#define	 MODE_PULLDISABLE       (0)


//rPAD_SSEL_CTRL0
#define  CELL_SSEL_DRIV_21MA    (1)
#define  CELL_SSEL_DRIV_28MA    (2)
#define  CELL_SSEL_DRIV_48MA    (3)
#define  CELL_SSEL_DRIV_55MA    (4)
#define  CELL_SSEL_DRIV_76MA    (5)
#define  CELL_SSEL_DRIV_83MA    (6)
#define  CELL_SSEL_DRIV_103MA   (7)
#define  CELL_SSEL_DRIV_HIGHZ   (0)

#define	 BIT_DQ_SSEL             (0x03)
#define	 BIT_CE_SSEL             (0x03<<4)
#define	 BIT_CLE_SSEL            (0x03<<8)
#define	 BIT_ALE_SSEL            (0x03<<12)
#define	 BIT_WE_SSEL             (0x03<<16)
#define	 BIT_DQS_SSEL            (0x03<<20)
#define	 BIT_RE_SSEL             (0x03<<24)

#define  ALL_SSEL_MODE(bMode)    ((U32)bMode | (U32)bMode <<4| (U32)bMode <<8| (U32)bMode <<12| (U32)bMode <<16| (U32)bMode <<20| (U32)bMode <<24)
//rPORT_DATA0
//rPORT_DATA12
#define  BIT_ROMC_DATA          (1<<4)
#define  BIT_URTTX_DATA         (1<<0)
//rPORT_DATA16
#define	 BIT_6ADDR_DATA         (1<<2)  
#define  BIT_FLASHRND_DATA      (1<<3)

/******************************变量声明******************************/
#define PORT_0        (0)
#define PORT_1        (1)
#define PORT_2        (2)
#define PORT_3        (3)
#define PORT_4        (4)
#define PORT_5        (5)
#define PORT_6        (6)
#define PORT_7        (7)
#define PORT_8        (8)
#define PORT_9        (9)
#define PORT_10        (10)
#define PORT_11        (11)
#define PORT_12        (12)
#define PORT_13        (13)
#define PORT_14        (14)
#define PORT_15        (15)
#define PORT_16        (16)
#define PORT_17        (17)

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
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
	void PAD_Dqs_PullEnCtrl(U8 bChNum,U8 bMode);
	// *****************************************************************************
	// 函数名称: PAD_Dqs_Func
	// 函数功能: 设置dqs的模式
	// 入口参数: bChNum：通道号
	//          bMode:是否使能，1：GPIO模式
	//						   0：FC模式
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_Dqs_Func(U8 bChNum,U8 bMode);
	// *****************************************************************************
	// 函数名称: PAD_RB_Func
	// 函数功能: 设置RB的模式
	// 入口参数:  bMode:是否使能，1：GPIO模式
	//						     0：FC模式
	//         
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_RB_Func(U8 bMode);
	// *****************************************************************************
	// 函数名称: Pad_Ssel_Ch
	// 函数功能: 设置输出加强模式
	// 入口参数: bCh：通道 0,1,...
	//          dVal:寄存器的值
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_SSelCtrl0to3(U8 bPadNum,U32 dVal);

	void PAD_SSelCtrl4(U32 dVal);

	void PAD_TSelCtrl(U32 dVal);

	void PAD_STCtrl(U32 dVal);
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
	void PAD_PullEn0to3(U8 bPadNum,U32 dVal1,U32 dVal2);

	void PAD_PullEn4(U32 dVal1,U32 dVal2);

	void PAD_PullEn5(U32 dVal1,U32 dVal2);

	void PAD_FXWPCtrl(U8 bMode);

#define  PAD_ALL_PULL_EN      (U32_ALL_ONE)
#define  PAD_ALL_PULL_DISABLE (ALL_ZERO)
#define  PAD_ALL_PULL_UP      (U32_ALL_ONE)
#define  PAD_ALL_PULL_DOWN    (ALL_ZERO)
	// *****************************************************************************
	// 函数名称: PAD_FuncCtrl0to3
	// 函数功能: 设置pad模式，GPIO和FUNC模式
	// 入口参数: bCh：通道 0,1,...
	//          dVal:寄存器的值
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_FuncCtrl0to3(U8 bCh, U32 dVal);
	
	void PAD_FuncCtrl4(U32 dVal);
#define  PAD_ALL_GPIO_MODE  (U32_ALL_ONE)
#define  PAD_ALL_FUNC_MODE  (ALL_ZERO)
	// *****************************************************************************
	// 函数名称: PAD_PortData
	// 函数功能: 设置整个port的值
	// 入口参数: bPortNum： port的编号
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_PortData(U8 bPortNum, U32 dVal);

	void PAD_PortMode(U8 bPortNum,U32 dIOMode);

	void PAD_PortData_InOut(U8 bPortNum,U32 dIOMode ,U32 dVal);

	// *****************************************************************************
	// 函数名称: Pad_SpiModeSel
	// 函数功能: 设置SPI pad模式，GPIO和FUNC模式
	// 入口参数: bMode： 0: Function Mode,1: GPIO Mode
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_SpiModeSel(U8 bMode);

	void PAD_UartModeSel(U8 bMode);

	void PAD_UartCfg(U8 bCfg);
	void PAD_PowerDownCfg(void);

	void PAD_LEDxModeSel(U8 bNum,U8 bMode);
	// *****************************************************************************
	// 函数名称: PAD_RomcModeSel
	// 函数功能: 设置romc上下拉模式，调用会上下拉使能
	// 入口参数: bMode： 0: pull down,1: pull up
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_RomcModeSel(U8 bMode);
	// *****************************************************************************
	// 函数名称: PAD_Func_Init
	// 函数功能: 将四个通道都设置成func模式
	// 入口参数:
	// 出口参数: 
	// 说    明: 
	// *****************************************************************************
	void PAD_Func_Init(void);
	// *****************************************************************************
	// 函数名称: PAD_GetData
	// 函数功能: 获取port的值
	// 入口参数: port编号
	// 出口参数: port中的值
	// 说    明: 
	// *****************************************************************************
	U32 PAD_GetData(U8 bPortNum);

	// *****************************************************************************
	// 函数名称: LED_Config
	// 函数功能: 配置LED功能
	// 入口参数: bNum：LED 编号
	//           bMode:工作模式
	//         bPeriod:闪烁周期
	// 出口参数:
	// 说    明:
	// *****************************************************************************
	void LED_Config(U8 bNum,U8 bMode,U16 bPeriod);
	void PAD_VDT_Init(U8 bMode, U8 bLevel,U8 bVDTEn);

	// *****************************************************************************
	// 函数名称: PAD_SetCE
	// 函数功能: 20200329 Tender要求在各CE从GPIO模式切换成Function模式前，
	//			将PORT_DATA1/4/7/10设置成0xFF,以解决CE短暂为低电平的现象
	// 入口参数:
	// 出口参数:
	// 说    明:
	// *****************************************************************************
	void PAD_SetCE();

#ifdef __cplusplus
}
#endif
#endif/*__PAD__*/
