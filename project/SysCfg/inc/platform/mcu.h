// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  mcu.h
//  文件标识：  
//  内容摘要：  mcu 相关寄存器配置接口定义
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
#ifndef __MCU__
#define __MCU__
/******************************包含头文件******************************/
#include "common.h"
#include "uitility.h"

#ifndef __nop
#define __nop() __nds32__nop()
#endif

#ifndef __nop8
#define __nop8() {	__nds32__nop(); __nds32__nop(); __nds32__nop(); __nds32__nop();\
								__nds32__nop(); __nds32__nop(); __nds32__nop(); __nds32__nop();\
								}
#endif

#ifndef __nop16
#define __nop16() {	__nds32__nop(); __nds32__nop(); __nds32__nop(); __nds32__nop();\
									__nds32__nop(); __nds32__nop(); __nds32__nop(); __nds32__nop();\
									__nds32__nop(); __nds32__nop(); __nds32__nop(); __nds32__nop();\
	 	 	 	 	 	 	 	 	__nds32__nop(); __nds32__nop(); __nds32__nop();__nds32__nop();\
								}
#endif

typedef void (VMC_SwapCodeHandler_t)(U32 dLoadAddr);

/******************************变量声明******************************/

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

	extern  inline void GIE_ENABLE();

	extern inline U32 __get_sda_base();

	extern inline U32 __restore_sda_base(U32 intput_gp);

	extern  inline void GIE_DISABLE();

	extern inline void __delay_us(U32 time,U32 clkcnt_per_us);
	extern inline void __delay_ms(U32 time,U32 clkcnt_per_ms);

	extern inline void __enable_performance();

	extern inline void __disbale_performance();
	extern  inline U32 __get_pfm_count(void);
	extern  inline U32 __get_PRIMASK(void);

	extern  inline void __set_PRIMASK(U32 dInt);

	extern  inline void NVIC_EnableIRQ(U8 bIRQ_NUM);

	extern  inline void NVIC_DisableIRQ(U8 bIRQ_NUM);

	extern  inline void NVIC_ClearPendingIRQ(U8 bIRQ_NUM);

	extern  inline void NVIC_SetPriority(U8 bIRQ_NUM,U8 bPriority);
	extern inline void g0_disable();
	extern inline void g0_enable();
	extern inline void __memory_powerdown();
	extern inline void __memory_poweron(U32 cycle);
	extern inline void __reset_memory_poweron();

	#define G3CORE   (1)
	#define G0CORE   (0)
	#define RTY_CNT_MUTIREG (0)
	#define RTY_BUFFSTS_MUTIREG  (1)
	#define RTY_PMCP0_MUTIREG  	 (2)
	#define RTY_PMCP1_MUTIREG  	 (3)


#ifdef __cplusplus
}
#endif
#endif/*__MCU__*/
