##==============================================================================
##
##  crt0.S
##
##  nds32 startup code
##
##==============================================================================
##
## Copyright (c) 1995, 1996, 1997, 1998 Cygnus Solutions
##
## The authors hereby grant permission to use, copy, modify, distribute,
## and license this software and its documentation for any purpose, provided
## that existing copyright notices are retained in all copies and that this
## notice is included verbatim in any distributions. No written agreement,
## license, or royalty fee is required for any of the authorized uses.
## Modifications to this software may be copyrighted by their authors
## and need not follow the licensing terms described here, provided that
## the new terms are clearly indicated on the first page of each file where
## they apply.
##

#include <nds32_init.inc>
#include "config.h"
#include "interrupt.h"
#if defined(CFG_AG102)
#include "ag102.h"
#elif defined(CFG_AE210P)
#include "ae210p.h"
#else
#include "ag101p.h"
#endif

##------------------------------------------------------------------------------
    .macro vector name
    .align 2
    j OS_Trap_\name
    .endm

    .macro OS_Trap_Interrupt_HW num
OS_Trap_Interrupt_HW\num:
    push $r0
    li  $r0, \num
    b   common_ISR_wrapper
    .endm

    .macro LED x
// Disable LED display because the simulator doesn't support LED function.
#ifndef	CFG_SIMU
    hal_set_led \x
#endif
    .endm

    .section .nds32_init, "ax"
!========================================================================
! Vector table
!========================================================================
    .align 2
exception_vector:
    j _start                    !  (0) Trap Reset
    vector TLB_Fill             !  (1) Trap TLB fill
    vector PTE_Not_Present      !  (2) Trap PTE not present
    vector TLB_Misc             !  (3) Trap TLB misc
    vector TLB_VLPT_Miss        !  (4) Trap TLB VLPT miss
    vector Machine_Error        !  (5) Trap Machine error
    vector Debug_Related        !  (6) Trap Debug related
    vector General_Exception    !  (7) Trap General exception
    vector Syscall              !  (8) Syscall

    /* HW interrupt vector layout */
    hal_hw_vectors
    .section .text
    .align  2
exception_vector_end:
exception_handler:
    .long tlb_exception_handler
    .long error_exception_handler
    .long syscall_handler
ISR_TABLE:
    .long HW0_ISR
    .long HW1_ISR
    .long HW2_ISR
    .long HW3_ISR
    .long HW4_ISR
    .long HW5_ISR
    .long HW6_ISR
    .long HW7_ISR
    .long HW8_ISR
    .long HW9_ISR
    .long HW10_ISR
    .long HW11_ISR
    .long HW12_ISR
    .long HW13_ISR
    .long HW14_ISR
    .long HW15_ISR
    .long HW16_ISR
    .long HW17_ISR
    .long HW18_ISR
    .long HW19_ISR
    .long HW20_ISR
    .long HW21_ISR
    .long HW22_ISR
    .long HW23_ISR
    .long HW24_ISR
    .long HW25_ISR
    .long HW26_ISR
    .long HW27_ISR
    .long HW28_ISR
    .long HW29_ISR
    .long HW30_ISR
    .long HW31_ISR
/*
 * exception handlers
 */

/*----------------------------------------------------------------------
   Since N903 doesn't have MMU, make them share the common error handler.
----------------------------------------------------------------------*/
OS_Trap_TLB_Fill:

    //SAVE_ALL
    li  $r0, #0x1
    b   tlb_exception_handler


OS_Trap_PTE_Not_Present:

    //SAVE_ALL
    li  $r0, #0x2
    b   tlb_exception_handler


OS_Trap_TLB_Misc:

    //SAVE_ALL
    li  $r0, #0x3
    b   tlb_exception_handler


OS_Trap_TLB_VLPT_Miss:

    //SAVE_ALL
    li  $r0, #0x4
    b   tlb_exception_handler


OS_Trap_Machine_Error:

    //SAVE_ALL
    li  $r0, #0x5
    b   error_exception_handler

OS_Trap_Debug_Related:

    //SAVE_ALL
    li  $r0, #0x6
    b   error_exception_handler

OS_Trap_General_Exception:

    SAVE_ALL_HW
    li  $r0, #0x7
    b   error_exception_handler
    RESTORE_ALL_HW
    iret

OS_Trap_Syscall:

    SYSCALL_SAVE_ALL
    bal syscall_handler
    SYSCALL_RESTORE_ALL
    iret

common_ISR_wrapper:
    SAVE_ALL_HW

    pushm $r0, $r0  //保存中断号
    //NVIC_DisableIRQ(IRQ_SC);
    jal NVIC_DisableIRQ
    //GIE_ENABLE();
    gie_enable //jal GIE_ENABLE
    popm $r0, $r0  //保存中断号

    la  $r2, ISR_TABLE
    lw  $r1, [$r2+$r0<<2]

    pushm $r0, $r0//保存中断号
    jral $r1
    popm $r0, $r0//保存中断号

    //NVIC_EnableIRQ(IRQ_SC);
    jal NVIC_EnableIRQ
    RESTORE_ALL_HW
    iret

//undefined interrupt
OS_Trap_Interrupt_UNDEF:
    b   OS_Trap_Interrupt_UNDEF

    /* Other HW interrupt handlers */
    hal_hw_ISR

##------------------------------------------------------------------------------
## Startup code

#ifdef CFG_LLINIT
#ifdef CFG_REMAP
#define LED_VALUE (0x66) //Burn
#else
#define LED_VALUE (0x11) //xIp
#endif
#else
#define LED_VALUE (0x00) //lOad
#endif

    .section .text
    .global _start
    .weak _call_exit
    .weak _SDA_BASE_
    .func _start
    .type _start, @function

    .align 2
_start:
    !************************** Begin of do-not-modify **************************
    ! Please don't modify this code
    ! Initialize the registers used by the compiler
    //上电关闭中断
    jal GIE_DISABLE

#ifdef __DUAL_CORE__
    #ifdef __G3_FLYCODE
        #ifndef __ALG_FORMAL
            jal  g0_disable   !双核模式下，先运行G3初始化，注册变量初始化完成以后再打开G0
        #endif
    #endif
#else
	//willaim建议：在 boot下，关闭G0核
	jal  g0_disable
#endif

    movi55 $r0,#0x0
    //更改SP之前要关闭堆栈保护功能
    mtsr $r0,$hsp_ctl

    movi55 $r0,#0x0
    //关闭所有外设的中断使能,待各个模块初始化完成以后再使能
    mtsr $r0, $INT_MASK2

#ifdef __DUAL_CORE__
    #ifdef __G3_FLYCODE
        //上电开启使能MEMORY --双核只要G3初始化一遍就行
        li $r0,#0xFF
        jal __reset_memory_poweron
        !----SG_CPM->rPOWER_CTRL.bits.ISO_EN=1;
    #endif
#else
    //上电开启使能MEMORY --单核
    li $r0,#0xFF
    jal __reset_memory_poweron
    !----SG_CPM->rPOWER_CTRL.bits.ISO_EN=1;
#endif

//scan code 放置在掉电区，没复位前，不允许使用。因此复位仅在boot code中实现。
#ifdef __BOOT_CODE__
	! Initialize default stack pointer
	li      $sp, #0x430000
	jal stack_bound_init
	//nds32_init
    !先初始化堆栈在进行函数调用
	jal __reset_memory_poweron
#endif
	!先初始化堆栈，再进行函数调用
	nds32_init		! NDS32 startup initial macro in <nds32_init.inc>

#ifdef CFG_DEBUG
    mfsr $r0, $MSC_CFG

#ifndef CONFIG_NO_NDS32_EXT_EX9
    li  $r1, (1 << 24)      ! EIT
    and $r2, $r0, $r1
1:	beqz $r2, 1b
#endif

#ifdef __NDS32_EXT_IFC__
    li  $r1, (1 << 19)      ! IFC
    and $r2, $r0, $r1
1:	beqz $r2, 1b
#endif

#endif //#ifdef CFG_DEBUG

    !*************************** End of do-not-modify ***************************
    //20200207review：此处保持与9081的boot一致，不提前切回高分频！！！
#ifndef __BOOT_CODE__
    jal SystemInit  !早点把时钟切换到高频
#endif

    ;LED LED_VALUE           ! light LED

#ifdef CFG_LLINIT
    ;bal _nds32_init_mem
#endif
    bal __init
#ifdef CFG_GCOV
    bal _crt_init
    bal main
    jal exit
#else
    bal main
1:  b   1b
#endif

    .size _start, .-_start

    .end
