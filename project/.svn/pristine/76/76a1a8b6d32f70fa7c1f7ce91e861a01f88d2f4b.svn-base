#include "common.h"
#include "system_sg9081.h"
#include "uitility.h"
#include "dsata.h"
#include "sata_typedef.h"
#include "sg9081.h"
#include "scsi.h"
#include "ata_identify.h"
#include "ata_dco.h"


#if (SATA_LOG_ENABLE == 1)
#include "ata_gpl.h"
#endif

#define SOURCLK_250M    0UL        /*250、25M时钟源*/
#define SOURCLK_333M    1UL        /*333M时钟源*/
#define SOURCLK_375M    2UL        /*375M时钟源*/
#define SOURCLK_400M    3UL        /*300M时钟源*/
#define SOURCLK_25M     4UL        /*25M时钟源*/

#define CLK_1_DIV       (1U)//分频系数
#define CLK_2_DIV       (2U)
#define CLK_4_DIV       (4U)
#define CLK_6_DIV       (6U)
#define CLK_8_DIV       (8U)
#define CLK_10_DIV      (10U)
#define CLK_12_DIV      (12U)
#define CLK_14_DIV      (14U)
#define CLK_16_DIV      (16U)
#define CLK_32_DIV      (32U)

#define CLK_PER_SEL     (1U)//时钟源选择
#define CLK_333MHZ_SEL  (2U)
#define CLK_375MHZ_SEL  (4U)
#define CLK_400MHZ_SEL  (8U)

//#define __FPGA_PLATEFORM

/*配置时钟*/
#define SYSCLK_SEL                      (CLK_400MHZ_SEL)

#define SYSCLK_PERIOD_SEL               (CLK_2_DIV)
#define TRX2CLK_PERIOD_SEL              (CLK_1_DIV)
#define LDECCLK_PERIOD_SEL				(CLK_2_DIV)
#ifdef __FPGA_PLATEFORM_9086
#define CPM_TRX2ClkCfg_Ch(ch,bSouceSel,bPeriod) {\
                                                 SG_CPM->rSOFT_GATE0.bits.FC0_TRX2_CLK=0;\
                                                 SG_CPM->rSOFT_GATE0.bits.FC1_TRX2_CLK=0;\
                                                 SG_CPM->rSOFT_GATE0.bits.FC2_TRX2_CLK=0;\
                                                 SG_CPM->rSOFT_GATE0.bits.FC3_TRX2_CLK=0;\
                                                 SG_CPM->rTRX2_CLK_SWITCH_EN.bits.bVAL = 0;\
                                                 SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL = TRX2CLK_PERIOD_SEL;\
                                                 Delay(50);\
                                                 SG_CPM->rTRX2_CLK_SWITCH_EN.bits.bVAL = 1;\
                                                 Delay(50);\
                                                 SG_CPM->rRX_CLK_SWITCH_EN.bits.bVAL = 0;\
                                                 SG_CPM->rRX_CLK_PERIOD.bits.dVAL = TRX2CLK_PERIOD_SEL * 2;\
                                                 Delay(50);\
                                                 SG_CPM->rRX_CLK_SWITCH_EN.bits.bVAL = 1;\
                                                 Delay(50);\
                                                 SG_CPM->rSOFT_GATE0.dAll|=0xFFFFFFFF;\
                                                 SG_CPM->rTRX2_CLK_SOURCE_SEL.bits.dVAL = SYSCLK_SEL;\
                                                 SG_CPM->rRX_CLK_SOURCE_SEL.bits.dVAL = SYSCLK_SEL;\
                                                }
#else
#define CPM_TRX2ClkCfg_Ch(ch,bSouceSel,bPeriod) {\
                                                 SG_CPM->rSOFT_GATE0.bits.FC0_TRX2_CLK=0;\
                                                 SG_CPM->rSOFT_GATE0.bits.FC1_TRX2_CLK=0;\
                                                 SG_CPM->rSOFT_GATE0.bits.FC2_TRX2_CLK=0;\
                                                 SG_CPM->rSOFT_GATE0.bits.FC3_TRX2_CLK=0;\
                                                 SG_CPM->rTRX2_CLK_SWITCH_EN.bits.bVAL = 0;\
                                                 SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL = bPeriod;\
                                                 Delay(50);\
                                                 SG_CPM->rTRX2_CLK_SWITCH_EN.bits.bVAL = 1;\
                                                 Delay(50);\
                                                 SG_CPM->rRX_CLK_SWITCH_EN.bits.bVAL = 0;\
                                                 SG_CPM->rRX_CLK_PERIOD.bits.dVAL = bPeriod * 2;\
                                                 Delay(50);\
                                                 SG_CPM->rRX_CLK_SWITCH_EN.bits.bVAL = 1;\
                                                 Delay(50);\
                                                 SG_CPM->rSOFT_GATE0.dAll|=0xFFFFFFFF;\
                                                 SG_CPM->rTRX2_CLK_SOURCE_SEL.bits.dVAL = bSouceSel;\
                                                 SG_CPM->rRX_CLK_SOURCE_SEL.bits.dVAL = bSouceSel;\
                                                }
#endif
#ifndef __G3_FLYCODE
// *****************************************************************************
// 函数名称: TMR0_IRQHandler
// 函数功能: TMR0中断服务函数 --做system tick  1ms一次
// 入口参数: NONE
// 出口参数:
// 说    明:
// *****************************************************************************
void SysTick_IRQHandler()
{
    SG_MCU->rTIMER0_CTRL.bits.OV_FLAG = 0;//CLEAR FLAG
    //清除time0中断标记
    NVIC_ClearPendingIRQ(IRQ_T0);
    //system tick process
    DSATA_SysTickProcess();
}
#endif

U32_C OSC[]={250000000,333000000,375000000,400000000,25000000};
U32 GetSysTickCnt(void)
{
    U32 dSysTickCnt = 0;
    //根据当前运行时钟更新相关变量
    if(SG_CPM->rSYS_CLK_SOURCE_SEL == CLK_400MHZ_SEL)
    {
        dSysTickCnt = OSC[SOURCLK_400M];
    }
    else if(SG_CPM->rSYS_CLK_SOURCE_SEL == CLK_375MHZ_SEL)
    {
        dSysTickCnt = OSC[SOURCLK_375M];
    }
    else if(SG_CPM->rSYS_CLK_SOURCE_SEL == CLK_333MHZ_SEL)
    {
        dSysTickCnt = OSC[SOURCLK_333M];
    }
    else if(SG_CPM->rSYS_CLK_SOURCE_SEL == CLK_PER_SEL)
    {
        if(SG_CPM->rPRE_CLK_SOURCE_SEL == 1)//250M
        {
            dSysTickCnt = OSC[SOURCLK_250M];
        }
        else
        {
            dSysTickCnt = OSC[SOURCLK_25M];
        }
    }
    else
    {
        //error
    }
    return dSysTickCnt;
}

// *****************************************************************************
// 函数名称: SysTick_Config
// 函数功能: 定时器0毫秒数
// 入口参数: 毫秒数
// 出口参数:
// 说    明:TIME0 作为 SYSTEM STICK 功能 1ms 产生一次中断
// *****************************************************************************
void SysTick_Config(U32 dMlisec,U32 dMlisecondUnit)
{
    SG_MCU->rTIMER0_CTRL.bits.ENABLE=0;
    SG_MCU->rTIMER0_CTRL.bits.MODE = 0;//之后停止计数，自动把重装旧值

    //不能设定为0
    SG_MCU->rTIMER0=dMlisec*dMlisecondUnit;

    //清除可能存在的PENDING中断
    SG_MCU->rTIMER0_CTRL.bits.OV_FLAG=0;
    SG_MCU->rTIMER0_CTRL.bits.ENABLE=1;
}

/**
* update the system clock
*
* @param  none
* @return none
*
* @brief  Setup the microcontroller system.
*         Initialize the System and update the SystemFrequency variable.
*/
//优化必须打开
//#pragma GCC optimize("O0")
void UpdateSystemClock (void)
{
    //U32 var;
    U32 sys_clk=GetSysTickCnt()/SG_CPM->rSYS_CLK_PERIOD;
    DSATA_UpdateSystemClockInfo(sys_clk);
#ifndef __G3_FLYCODE
    SysTick_Config(/*SYSTICK_PERIOD*/10,tSataDC.dMilliSecondCnt);//10毫秒一次中断,初始化并启动定时器
#else
    NVIC_DisableIRQ(IRQ_T0);
    //避免编译器优化SATA 相关变量
//  var=(U32)&tCmdHead;
//	var=var;
//	var=(U32)&arrunHWD2HFS;
//	var=var;
//	var=(U32)&tPrdTbls;
//	var=var;
//	var=(U32)&unFsmBuff;
//	var=var;
//	var=(U32)&tSataDC;
//	var=var;
//	var=(U32)&tTQC;
//	var=var;
//	var=(U32)&unAtaIdentify;
//	var=var;
//	var=(U32)&tScsiCtl;
//	var=var;
//	var=(U32)&tScsiCmdHead;
//	var=var;
//	var=(U32)&tScsiDataHead;
//	var=var;
#endif
//	var=(U32)&tSyncBuffer;
//#if (SATA_LOG_ENABLE == 1)
//	var=(U32)&tSataLogCtx;
//#endif
//	var=(U32)&tAsyncBuffer;
//	var=var;
}

/**
* set mcu to low speed mode
*	LowSpeedModeEn
* @param  none
* @return none
*
* @brief  Setup the microcontroller system.
*         Initialize the System and update the SystemFrequency variable.
*/
void LowSpeedModeEn (void)
{
     if(SG_CPM->rSYS_CLK_SOURCE_SEL == 1)
     {
         return;
     }
     SG_CPM->rPRE_CLK_SOURCE_SEL=0x02;//选择25M
     SG_CPM->rSYS_CLK_SOURCE_SEL=1;//选择pre clk

     //接下来时钟会在25M的8分频时钟源运行
     UpdateSystemClock();
}


////实测颗粒度大约30ns
void delay_Clk(U32 __Clk__)
{
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    __enable_performance();
    while(__get_pfm_count() <= __Clk__)
    {
        //PRINT("__get_pfm_count() =%d\r\n",__get_pfm_count());
    }
    __disbale_performance();
    __set_PRIMASK(primask);
}

void delay_us(U32 __us__)
{
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    __enable_performance();
    while(__get_pfm_count() <= __us__*tSataDC.dMicroSecondCnt)
    {
        //PRINT("__get_pfm_count() =%d\r\n",__get_pfm_count());
    }
    __disbale_performance();
    __set_PRIMASK(primask);
}

void delay_ns(U32 __ns__)
{
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    __enable_performance();
    while(__get_pfm_count() <= __ns__*tSataDC.dMicroSecondCnt/1000)
    {
        //PRINT("__get_pfm_count() =%d\r\n",__get_pfm_count());
    }
    __disbale_performance();
    __set_PRIMASK(primask);
}

void delay_ms(U32 __ms__)
{
#if (0 == FOR_EMULATE)
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    __enable_performance();
    while(__get_pfm_count() <= __ms__*tSataDC.dMilliSecondCnt);
    __disbale_performance();
    __set_PRIMASK(primask);
#endif
}

/**
* Recovery mcu to normal
*	SystemClkRecovery
* @param  none
* @return none
*
* @brief  Setup the microcontroller system.
*         Initialize the System and update the SystemFrequency variable.
*/
void SystemClkRecovery (void)
{
     if(SG_CPM->rSYS_CLK_SOURCE_SEL == SYSCLK_SEL)
     {
         return;
     }
     SG_CPM->rSYS_CLK_SOURCE_SEL = SYSCLK_SEL;
     //接下来时钟会在400M的时钟源运行
     UpdateSystemClock();
}
/**
* Initialize the system
*
* @param  none
* @return none
*
* @brief  Setup the microcontroller system.
*         Initialize the System and update the SystemFrequency variable.
*/
#ifdef __FW__
void __section(".CODE_INIT") SystemInit(void)
#else
void SystemInit(void)
#endif
{
    SG_CPM->rWDT0_EN = 0xAA; //关闭G0WDT
    SG_CPM->rWDT1_EN = 0xAA; //关闭G3WDT
#if FOR_EMULATE
    SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
    SG_CPM->rSYS_CLK_PERIOD     = CLK_2_DIV; //将系统时钟调整到200兆
    SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
    SG_CPM->rSYS_CLK_SOURCE_SEL = SYSCLK_SEL;
    SG_CPM->rLDEC_CLK_SOURCE_SEL = SYSCLK_SEL;
    SG_CPM->rLENC_CLK_SOURCE_SEL = SYSCLK_SEL;
    SG_CPM->rLDEC_CLK_PERIOD = CLK_2_DIV;    //LDPC 软解码过程调整为200兆
    //SG_CPM->rLENC_CLK_PERIOD = 60;

    CPM_TRX2ClkCfg_Ch(0,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
    CPM_TRX2ClkCfg_Ch(1,SOURCLK_333M,CLK_1_DIV);       //通道1设置为166兆 = 333 / 1 /2
    CPM_TRX2ClkCfg_Ch(2,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
    CPM_TRX2ClkCfg_Ch(3,SYSCLK_SEL,CLK_1_DIV);      //通道3设置为200兆 = 400 / 1 / 2
#else
    tSataDC.dPowerOnTime=0;//power on reset 归零

    #ifdef __DUAL_CORE__
        SG_CPM->rSYS_CLK_SWITCH_EN = 0;
        SG_CPM->rSYS_CLK_PERIOD = SYSCLK_PERIOD_SEL;
        SG_CPM->rSYS_CLK_SWITCH_EN = 1;
        SG_CPM->rSYS_CLK_SOURCE_SEL = SYSCLK_SEL;
    #ifdef __G3_FLYCODE
        SG_CPM->rLDEC_CLK_SOURCE_SEL = SYSCLK_SEL;
        SG_CPM->rLDEC_CLK_PERIOD = LDECCLK_PERIOD_SEL;


        CPM_TRX2ClkCfg_Ch(0,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
//        CPM_TRX2ClkCfg_Ch(1,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
//        CPM_TRX2ClkCfg_Ch(2,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
//        CPM_TRX2ClkCfg_Ch(3,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
    #endif//#ifdef __G3_FLYCODE
    #else
        SG_CPM->rSYS_CLK_SWITCH_EN = 0;
        SG_CPM->rSYS_CLK_PERIOD = SYSCLK_PERIOD_SEL;
        SG_CPM->rSYS_CLK_SWITCH_EN = 1;
        SG_CPM->rSYS_CLK_SOURCE_SEL = SYSCLK_SEL;

        SG_CPM->rLDEC_CLK_SOURCE_SEL = SYSCLK_SEL;
        SG_CPM->rLDEC_CLK_PERIOD = LDECCLK_PERIOD_SEL;

        CPM_TRX2ClkCfg_Ch(0,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
        CPM_TRX2ClkCfg_Ch(1,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
        CPM_TRX2ClkCfg_Ch(2,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
        CPM_TRX2ClkCfg_Ch(3,SYSCLK_SEL,TRX2CLK_PERIOD_SEL);
    #endif//#ifdef __DUAL_CORE

#endif//#if FOR_EMULATE
    UpdateSystemClock();
}

void HAL_InitChanClock(U8 bChan,U32 dSysClkSourSel,U32 dSysClkPeriod, U8 bTrx2ClkPeriod)
{
	SG_CPM->rWDT0_EN = 0xAA; //关闭G0WDT
	SG_CPM->rWDT1_EN = 0xAA; //关闭G3WDT

#ifdef __FPGA_PLATEFORM_9086
	dSysClkSourSel = SYSCLK_SEL;
	dSysClkPeriod = SYSCLK_PERIOD_SEL;
	bTrx2ClkPeriod = TRX2CLK_PERIOD_SEL;
#endif

#ifdef __DUAL_CORE__
	SG_CPM->rSYS_CLK_SWITCH_EN = 0;
	SG_CPM->rSYS_CLK_PERIOD = dSysClkPeriod;
	SG_CPM->rSYS_CLK_SWITCH_EN = 1;
	SG_CPM->rSYS_CLK_SOURCE_SEL = dSysClkSourSel;
#ifdef __G3_FLYCODE
	SG_CPM->rLDEC_CLK_SOURCE_SEL = dSysClkSourSel;
	SG_CPM->rLDEC_CLK_PERIOD = dSysClkPeriod;
#endif

#else
	SG_CPM->rSYS_CLK_SWITCH_EN = 0;
	SG_CPM->rSYS_CLK_PERIOD = dSysClkPeriod;
	SG_CPM->rSYS_CLK_SWITCH_EN = 1;
	SG_CPM->rSYS_CLK_SOURCE_SEL = dSysClkSourSel;

	SG_CPM->rLDEC_CLK_SOURCE_SEL = dSysClkSourSel;
#ifdef __FPGA_PLATEFORM_9086
	SG_CPM->rLDEC_CLK_PERIOD = 8;
#else
#ifdef	__FPGA_PLATEFORM
	SG_CPM->rLDEC_CLK_PERIOD = 8;
#else
	SG_CPM->rLDEC_CLK_PERIOD = 2;
#endif
#endif
#endif//#ifdef __DUAL_CORE

	switch(bChan)
	{
		case 0:
			CPM_TRX2ClkCfg_Ch(0,dSysClkSourSel,bTrx2ClkPeriod);
			break;
		case 1:
			CPM_TRX2ClkCfg_Ch(1,dSysClkSourSel,bTrx2ClkPeriod);
			break;
		case 2:
			CPM_TRX2ClkCfg_Ch(2,dSysClkSourSel,bTrx2ClkPeriod);
			break;
		case 3:
			CPM_TRX2ClkCfg_Ch(3,dSysClkSourSel,bTrx2ClkPeriod);
			break;
		default:
			break;	
	}

	UpdateSystemClock();
}




