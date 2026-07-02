/******************************************************************************
* @file:    system_sg9081.c
* @purpose: CMSIS Cortex-M0/3 Device Peripheral Access Layer Source File
*           for the SiliconGo  SG9081
* @version: V1.01
* @date:    19. Aug. 2009
*----------------------------------------------------------------------------
*
* Copyright (C) 2009 ARM Limited. All rights reserved.
*
* ARM Limited (ARM) is supplying this software for use with Cortex-M0 
* processor based microcontrollers.  This file can be freely distributed 
* within development tools that are supporting such ARM based processors. 
*
* THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
******************************************************************************/
#ifndef _SYSTEM_SG_H_
#define _SYSTEM_SG_H_
#include "ae210p.h"
#ifdef __RDT_ONE_SECOND_TICK		//RDT特殊应用需要，顾增加了一个宏控制
	#define SYSTICK_PERIOD  (1000U)
#else
    #define SYSTICK_PERIOD  (10U)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	extern void delay_Clk(U32 __Clk__);
    extern void delay_us(U32 __us__);
    extern void delay_ms(U32 __ms__);
    /**
    * set mcu to low speed mode
    *	LowSpeedModeEn
    * @param  none
    * @return none
    *
    * @brief  Setup the microcontroller system.
    *         Initialize the System and update the SystemFrequency variable.
    */
    extern void LowSpeedModeEn (void);

    /**
    * Recovery mcu to normal
    *	SystemClkRecovery
    * @param  none
    * @return none
    *
    * @brief  Setup the microcontroller system.
    *         Initialize the System and update the SystemFrequency variable.
    */
    extern void SystemClkRecovery (void);
    /**
    * Initialize the system
    *
    * @param  none
    * @return none
    *
    * @brief  Setup the microcontroller system.
    *         Initialize the System and update the SystemFrequency variable.
    */
    extern void SystemInit (void);
    
    /**
    * update the system clock
    *
    * @param  none
    * @return none
    *
    * @brief  Setup the microcontroller system.
    *         Initialize the System and update the SystemFrequency variable.
    */
    extern  void UpdateSystemClock (void);
    extern U32 GetSysTickCnt(void);


	#define SYS_TEM_FREQUENCY	((U32)(GetSysTickCnt()/SG_CPM->rSYS_CLK_PERIOD))/*获取系统时钟*/
	#define MILLI_SECOND_CNT	(SYS_TEM_FREQUENCY/1000)/*确定毫秒计时数*/
	#define MICRO_SECOND_CNT	(MILLI_SECOND_CNT/1000)/*确定微妙计时数*/



#ifdef __cplusplus
}
#endif

#endif /*end of _system_SG_h_*/
