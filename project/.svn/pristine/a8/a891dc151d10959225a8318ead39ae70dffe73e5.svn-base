#include <nds32_intrinsic.h>
#include "config.h"
#include "assert_demos.h"
#include "debug.h"
#include "sg9081.h"
#include "uitility.h"
#include "common.h"
#include "sata_typedef.h"
#ifdef CFG_GCOV
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef __G3_FLYCODE
#ifdef  __ALG_FORMAL
#include "main.h"
#endif
#endif

/* It will use Default_Handler if you don't have one */
#pragma weak tlb_exception_handler   = Tlb_exception_handler
#pragma weak error_exception_handler = SysError_IRQHandler
#pragma weak syscall_handler         = SysCall_Handler

#pragma weak HW0_ISR   = Default_Handler
#pragma weak HW1_ISR   = Default_Handler
#pragma weak HW2_ISR   = Default_Handler
#pragma weak HW3_ISR   = Default_Handler
#pragma weak HW4_ISR   = Default_Handler
#pragma weak HW5_ISR   = Default_Handler
#pragma weak HW6_ISR   = Default_Handler
#pragma weak HW7_ISR   = Default_Handler
#pragma weak HW8_ISR   = Default_Handler
#pragma weak HW9_ISR   = Default_Handler
#pragma weak HW10_ISR  = Default_Handler
#pragma weak HW11_ISR  = Default_Handler
#pragma weak HW12_ISR  = Default_Handler
#pragma weak HW13_ISR  = Default_Handler
#pragma weak HW14_ISR  = Default_Handler
#pragma weak HW15_ISR  = Default_Handler
#pragma weak HW16_ISR  = Default_Handler
#pragma weak HW17_ISR  = Default_Handler
#pragma weak HW18_ISR  = Default_Handler
#pragma weak HW19_ISR  = Default_Handler
#pragma weak HW20_ISR  = Default_Handler
#pragma weak HW21_ISR  = Default_Handler
#pragma weak HW22_ISR  = Default_Handler
#pragma weak HW23_ISR  = Default_Handler
#pragma weak HW24_ISR  = Default_Handler
#pragma weak HW25_ISR  = Default_Handler
#pragma weak HW26_ISR  = Default_Handler
#pragma weak HW27_ISR  = Default_Handler
#pragma weak HW28_ISR  = Default_Handler
#pragma weak HW29_ISR  = Default_Handler
#pragma weak HW30_ISR  = Default_Handler
#pragma weak HW31_ISR  = Default_Handler
#pragma weak SW0_ISR   = Default_Handler
#pragma weak VEP0_ISR  = Default_Handler
#pragma weak VEP1_ISR  = Default_Handler
#pragma weak VEP2_ISR  = Default_Handler
#pragma weak VEP3_ISR  = Default_Handler
#pragma weak VEP4_ISR  = Default_Handler
#pragma weak VEP5_ISR  = Default_Handler
#pragma weak VEP6_ISR  = Default_Handler
#pragma weak VEP7_ISR  = Default_Handler
#pragma weak VEP8_ISR  = Default_Handler
#pragma weak VEP9_ISR  = Default_Handler
#pragma weak VEP10_ISR = Default_Handler
#pragma weak VEP11_ISR = Default_Handler
#pragma weak VEP12_ISR = Default_Handler
#pragma weak VEP13_ISR = Default_Handler
#pragma weak VEP14_ISR = Default_Handler
#pragma weak VEP15_ISR = Default_Handler
#pragma weak VEP16_ISR = Default_Handler
#pragma weak VEP17_ISR = Default_Handler
#pragma weak VEP18_ISR = Default_Handler
#pragma weak VEP19_ISR = Default_Handler
#pragma weak VEP20_ISR = Default_Handler
#pragma weak VEP21_ISR = Default_Handler
#pragma weak VEP22_ISR = Default_Handler
#pragma weak VEP23_ISR = Default_Handler
#pragma weak VEP24_ISR = Default_Handler
#pragma weak VEP25_ISR = Default_Handler
#pragma weak VEP26_ISR = Default_Handler
#pragma weak VEP27_ISR = Default_Handler
#pragma weak VEP28_ISR = Default_Handler
#pragma weak VEP29_ISR = Default_Handler
#pragma weak VEP30_ISR = Default_Handler
#pragma weak VEP31_ISR = Default_Handler
#pragma weak VEP32_ISR = Default_Handler
#pragma weak VEP33_ISR = Default_Handler
#pragma weak VEP34_ISR = Default_Handler
#pragma weak VEP35_ISR = Default_Handler
#pragma weak VEP36_ISR = Default_Handler
#pragma weak VEP37_ISR = Default_Handler
#pragma weak VEP38_ISR = Default_Handler
#pragma weak VEP39_ISR = Default_Handler
#pragma weak VEP40_ISR = Default_Handler
#pragma weak VEP41_ISR = Default_Handler
#pragma weak VEP42_ISR = Default_Handler
#pragma weak VEP43_ISR = Default_Handler
#pragma weak VEP44_ISR = Default_Handler
#pragma weak VEP45_ISR = Default_Handler
#pragma weak VEP46_ISR = Default_Handler
#pragma weak VEP47_ISR = Default_Handler
#pragma weak VEP48_ISR = Default_Handler
#pragma weak VEP49_ISR = Default_Handler
#pragma weak VEP50_ISR = Default_Handler
#pragma weak VEP51_ISR = Default_Handler
#pragma weak VEP52_ISR = Default_Handler
#pragma weak VEP53_ISR = Default_Handler
#pragma weak VEP54_ISR = Default_Handler
#pragma weak VEP55_ISR = Default_Handler
#pragma weak VEP56_ISR = Default_Handler
#pragma weak VEP57_ISR = Default_Handler
#pragma weak VEP58_ISR = Default_Handler
#pragma weak VEP59_ISR = Default_Handler
#pragma weak VEP60_ISR = Default_Handler
#pragma weak VEP61_ISR = Default_Handler
#pragma weak VEP62_ISR = Default_Handler
#pragma weak VEP63_ISR = Default_Handler

extern int uart_puts(const char *);

__attribute__((used))
static void Default_Handler(unsigned char bIRQ)
{
    //do nothing
    NVIC_DisableIRQ(bIRQ);
    //while(1);
}

#pragma weak __soc_init = __null_function

void __null_function()
{;
}
#ifdef __FW__
void __section(".CODE_INIT") __c_init()
#else
void __c_init()
#endif
{
/* Use compiler builtin memcpy and memset */
#define MEMCPY(des, src, n) __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

    extern char _end;
    extern char __bss_start;
    int size;

    /* If we load code to RAM we don't need to copy
     * data section and lma will be equal to vma.
     * */
#if defined(CFG_LLINIT) && !defined(CFG_REMAP)
    /* data section will be copied before we remap.
     * We don't need to copy data section here. */
    extern char __data_lmastart;
    extern char __data_start;
    extern char _edata;
    /* Copy data section to RAM */
    size = &_edata - &__data_start;
    U08MemCopy((U8*)&__data_start, (U8*)&__data_lmastart, size);
#endif
    /* Clear bss section */
    size = &_end - &__bss_start;
    U08MemSet((U8*)&__bss_start, 0, size);

//#define DEBUG_TEST
#ifdef DEBUG_TEST
    extern char _stack;
    extern char _stack_size;
    U32 _stack_bound=(U32)&_stack;
    _stack_bound-=(U32)&_stack_size;

     if ((0 == (SG_CPM->rSYS_RESERVED0&0x00000002)))
     {
         Mem32SetVal((U32*)0x0045D800,0xFFFFFFFF,0x002800>>2);
     }
     if ((0 == (SG_CPM->rSYS_RESERVED0&0x00000001)))
     {
         Mem32SetVal((U32*)0x00460000,0xFFFFFFFF,0x002800>>2);
     }
     Mem32SetVal(((U32*)_stack_bound),0xFFFFFFFF,(((U32)&_stack_size)>>2)-16);
 #endif
    return;
}

void __cpu_init()
{
    unsigned int tmp;

    /* turn on BTB */
    tmp = 0x0;
    __nds32__mtsr(tmp, NDS32_SR_MISC_CTL);

    /* disable all hardware interrupts */
    __nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
    if (__nds32__mfsr(NDS32_SR_IVB) & 0x01)   // program priority
        __nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#endif

#if defined(CFG_EVIC)
    /* set EVIC, vector size: 4 bytes, base: 0x0 */
    __nds32__mtsr(0x1<<13, NDS32_SR_IVB);
#else
# if defined(USE_C_EXT)
    /* If we use v3/v3m toolchain and want to use
     * C extension please use USE_C_EXT in CFLAGS
     */
#ifdef __NDS32_ISA_V3__
    /* set IVIC, vector size: 4 bytes, base: 0x0 */
    __nds32__mtsr(0x0, NDS32_SR_IVB);
#else
    /* set IVIC, vector size: 16 bytes, base: 0x0 */
    __nds32__mtsr(0x1<<14, NDS32_SR_IVB);
#endif
# else
    /* set IVIC, vector size: 4 bytes, base: 0x0
     * If we use v3/v3m toolchain and want to use
     * assembly version please don't use USE_C_EXT
     * in CFLAGS */
    __nds32__mtsr(0x0, NDS32_SR_IVB);
# endif
#endif
    /* Set PSW INTL to 0 */
    tmp = __nds32__mfsr(NDS32_SR_PSW);
//	tmp = tmp & 0xfffffff9;
    tmp = tmp & 0xfffffff9;
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
    /* Set PSW CPL to 7 to allow any priority */
    tmp = tmp | 0x70008;
#endif
    __nds32__mtsr_dsb(tmp, NDS32_SR_PSW);
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
    /* Check interrupt priority programmable*
    * IVB.PROG_PRI_LVL
    *      0: Fixed priority       -- no exist ir18 1r19
    *      1: Programmable priority
    */
    if (__nds32__mfsr(NDS32_SR_IVB) & 0x01) {
        /* Set PPL2FIX_EN to 0 to enable Programmable
        * Priority Level */
        __nds32__mtsr(0x0, NDS32_SR_INT_CTRL);
        /* Check IVIC numbers (IVB.NIVIC) */
        if ((__nds32__mfsr(NDS32_SR_IVB) & 0x0E)>>1 == 5) {	// 32IVIC
            /* set priority HW10,14,15,16,17: 0,
            * HW#-: 0 */
#ifndef __DUAL_CORE__
            __nds32__mtsr(0x05455555, NDS32_SR_INT_PRI);
			__nds32__mtsr(0x55555550, NDS32_SR_INT_PRI2);//20200207review：boot的中断优先级配置保持与9081一致！！！
#else

#ifndef __G3_FLYCODE
            //G0
            __nds32__mtsr(0x55455555, NDS32_SR_INT_PRI);
            __nds32__mtsr(0x55551555, NDS32_SR_INT_PRI2);//0x55551555
#else
            //G3
            __nds32__mtsr(0x55555555, NDS32_SR_INT_PRI);
            __nds32__mtsr(0x55555555, NDS32_SR_INT_PRI2);
#endif

#endif
        } else {
            /* set priority HW0: 0, HW1: 1, HW2: 2, HW3: 3
            * HW4-: 0 */
            __nds32__mtsr(0x000000e4, NDS32_SR_INT_PRI);
        }
    }
#endif
    //init for sp protection fucntion
    extern char _stack;
    extern char _stack_size;
    U32 _stack_bound=(U32)&_stack;
    _stack_bound-=(U32)&_stack_size;
    __nds32__mtsr(_stack_bound, NDS32_SR_SP_BOUND);
    __nds32__mtsr(0x0000000d, NDS32_SR_HSP_CTL);
    return;
}

inline void stack_bound_init()
{
	U32 _stack_bound=(U32)0x430000;
	_stack_bound-=(U32)0x800;
	__nds32__mtsr(_stack_bound, NDS32_SR_SP_BOUND);
	__nds32__mtsr(0x0000000d, NDS32_SR_HSP_CTL);
}

void __soc_init();

void __init()
{
/*----------------------------------------------------------
   !!  Users should NOT add any code before this comment  !!
------------------------------------------------------------*/
    __cpu_init();
#if (FOR_EMULATE == 0)
    __c_init();     //copy data section, clean bss
#endif

    __soc_init();
    /* Double check ZOL supporting */
    /*
     * Check whether the CPU configured with ZOL supported.
     * The MSC_CFG.MSC_EXT($cr4) indicates MSC_CFG2 register exist
     * and MSC_CFG2($cr7) bit 5 indicates ZOL supporting.
     */
#ifdef CFG_HWZOL
    if (!((__nds32__mfsr(NDS32_SR_MSC_CFG) & (3 << 30))
        && (__nds32__mfsr(NDS32_SR_MSC_CFG2) & (1 << 5)))) {
        /* CPU doesn't support ZOL, but build with ZOL supporting. */
        uart_puts("CPU doesn't support ZOL, but build with ZOL supporting !!\n");
#ifdef CFG_GCOV
        exit(1);
#else
        while(1);
#endif
    }
#else
    if ((__nds32__mfsr(NDS32_SR_MSC_CFG) & (3 << 30))
        && (__nds32__mfsr(NDS32_SR_MSC_CFG2) & (1 << 5))) {
        /* CPU supports ZOL, but build without ZOL supporting. */
//		uart_puts("CPU supports ZOL, but build without ZOL supporting !!\n");
#ifdef CFG_GCOV
        exit(1);
#else
        while(1);
#endif
    }
#endif
}

#ifdef CFG_GCOV
void abort(void)
{
    fflush(stdout);
    exit(1);
}
#endif


void SysCall_Handler(unsigned char bIRQ)
{
    ERROR_PRINT("Enter func=<%s>[line=%d]\r\n",__func__,__LINE__);
    while(1);
}

// *****************************************************************************
// 函数名称: Tlb_exception_handler
// 函数功能: 系统异常中断服务函数
// 入口参数: NONE
// 出口参数:
// 说    明:
// *****************************************************************************
void Tlb_exception_handler(unsigned char bIRQ)
{
    ERROR_PRINT("Enter func=<%s>[line=%d]\r\n",__func__,__LINE__);
    while(1);
}

//#pragma GCC optimize("O0")
// *****************************************************************************
// 函数名称: SysError_IRQHandler
// 函数功能: 系统异常中断服务函数
// 入口参数: NONE
// 出口参数:
// 说    明:
// *****************************************************************************
void SysError_IRQHandler(unsigned char bIRQ)
{
#ifndef __G3_FLYCODE
    __get_sda_base();
#endif
#if (UART_DEBUG_ON==1)
    U32 dITYPE=__nds32__mfsr(NDS32_SR_ITYPE);
    //U32 dSP=__nds32__mfsr(NDS32_SR_SP_PRIV);
    U32 dIPC=__nds32__mfsr(NDS32_SR_IPC);
    U8 uETYPE=(dITYPE&0x0F);
    U8 uINST=(dITYPE&0x10)>>4;
    U8 uSUB_ETYPE=(dITYPE&0xF0000)>>16;

    char * ptr=(char *)General_exception[8];
    ERROR_PRINT("Enter func=<%s>[line=%d]\r\n",__func__,__LINE__);
    ERROR_PRINT("dITYPE= %x,bIRQ=%d\r\n", dITYPE,bIRQ);
    ERROR_PRINT("dIPC= %x\r\n", dIPC);
    ERROR_PRINT("uINST=%d\r\n", uINST);
    ERROR_PRINT("uSUB_ETYPE=%d\r\n", uSUB_ETYPE);
    ERROR_PRINT("I_TYPE:%s\r\n",I_TYPE[bIRQ]);

    if(bIRQ == 7)
    {
        //extern const char General_exception[12][55];
        ptr=(char *)General_exception[uETYPE];
    }
    else if(bIRQ == 5)
    {
        //extern const char Machine_error[6][50];
        ptr=(char *)Machine_error[uETYPE];
    }
    else if(bIRQ == 3)
    {
        //extern const char TLB_misc[6][35];
        ptr=(char *)TLB_misc[uETYPE];
    }
    ERROR_PRINT("%d.%s\r\n",uETYPE,ptr);

    //un define exception
    U8 *__ptr=(U8 *)(dIPC-8);
    U8 i=0;
    for(i=0;i<16;i++)
    {
        ERROR_PRINT("[%x]=%x\r\n", __ptr+i,__ptr[i]);
    }
#endif

#ifdef __BOOT_CODE__
    while(1);//不进行喂狗操作，直接复位
#else
    PRINT_TXT("E:Sys");
    //PRINT_HEX("IPC:",__nds32__mfsr(NDS32_SR_IPC));
    //PRINT_HEX("ITYPE:",__nds32__mfsr(NDS32_SR_ITYPE));
    //打印当前异常寄存器
    SG_CPM->rSYS_RESERVED0=0;
    SG_CPM->rSYS_RESERVED1=0;
    SG_CPM->rSYS_RESERVED2=0;
    SG_CPM->rSYS_RESERVED3=0;

#ifndef __G3_FLYCODE
#ifdef  __ALG_FORMAL
    if(tSataDC.tSataMI.pSystemExceptionEvent_cb != NULL && tSataUDP.m_dIsValid==SATA_CB_VALID_FLAG)
#else
    if(tSataDC.tSataMI.pSystemExceptionEvent_cb != NULL && tSataDC.tSataMI.dIsValid == SATA_CB_VALID_FLAG)
#endif
    {
        GIE_DISABLE();
        tSataDC.tSataMI.pSystemExceptionEvent_cb();
        GIE_ENABLE();
    }
#endif

    while(1);

#endif

}
