#ifndef AE210P_REGS
#define AE210P_REGS
/* Check to see if this file has been included already.  */

#ifndef __ASSEMBLER__
#include <nds32_intrinsic.h>
#endif

/* System Clock */
#define MHz                     1000000
#define CPUFREQ                 (40 * MHz)
#define HCLKFREQ                CPUFREQ
#define PCLKFREQ                CPUFREQ
#define UCLKFREQ                (20 * MHz)

/* External memory mapping */
#define EILM_BASE               0x00000000
//#define EDLM_BASE               0x00200000
#define EDLM_BASE               0x00020000

#define ORIG_RAM_BASE           EDLM_BASE

/************************************
 *  TARGET UART CONSTANTS           *
 ************************************/
#define UART1_BASE              0x00F02000
#define UART2_BASE              0x00F03000
#define UART_BASE               UART2_BASE
#define UART_IDREV_OFFSET       0x00
#define UART_HWCFGR_OFFSET      0x10
#define UART_OSCR_OFFSET        0x14
#define UART_RBR_OFFSET         0x20    /* Receiver Buffer Register for Read */
#define UART_THR_OFFSET         0x20    /* Transmitter Holding Register for Write */
#define UART_IER_OFFSET         0x24    /* Interrupt Enable Register */
/* Baud-Rate Divisor Latch (DL, Offset: 0x20, 0x24 when DLAB = 1) */
#define UART_DLL_OFFSET         0x20
#define UART_DLM_OFFSET         0x24
#define UART_IIR_OFFSET         0x28    /* Interrupt Identification Register */
#define UART_FCR_OFFSET         0x28    /* FIFO Control Register */
#define UART_LCR_OFFSET         0x2C    /* Line Control Register */
#define UART_MCR_OFFSET         0x30    /* Modem Control Register */
#define UART_LSR_OFFSET         0x34    /* Line Status Register for Read */
#define UART_MSR_OFFSET         0x38    /* Modem Status Register */
#define UART_SPR_OFFSET         0x3C    /* Scratch Pad Register */

/************************************
 *  TARGET PIT CONSTANTS            *
 ************************************/
#define PIT_BASE                0x00F04000
#define PIT_IDREV_OFFSET        0x00    /* Offset for PIT ID and revision register */
#define PIT_CONFIG_OFFSET       0x10    /* Offset for PIT configure register */
#define PIT_INTE_OFFSET         0x14    /* Offset for PIT interrupt enable counter */
#define PIT_INTS_OFFSET         0x18    /* Offset for PIT interrupt status counter */
#define PIT_CHNEN_OFFSET        0x1C    /* Offset for PIT channel enable register */
#define PIT_CTRL_CHN(N)         (0x20 + 0x10 * (N))
#define PIT_LOAD_CHN(N)         (0x24 + 0x10 * (N))
#define PIT_CNTR_CHN(N)         (0x28 + 0x10 * (N))

/* PIT Channel 0 ~ 3 as Timer 0 ~ 3, ATCPIT100 */
#define TM_IE                   (PIT_BASE + PIT_INTE_OFFSET)
#define TM_STA                  (PIT_BASE + PIT_INTS_OFFSET)
#define TM_CHNEN                (PIT_BASE + PIT_CHNEN_OFFSET)
#define TM_CTRL(N)              (PIT_BASE + PIT_CTRL_CHN(N))
#define TM_LOAD(N)              (PIT_BASE + PIT_LOAD_CHN(N))
#define TM_CNTR(N)              (PIT_BASE + PIT_CNTR_CHN(N))

#define TM_CTRL_32BIT           (0x1 << 0)
#define TM_CTRL_PCLK            (0x1 << 3)

/************************************
 *  TARGET GPIO CONSTANTS           *
 ************************************/
#define GPIO_BASE               0x00F07000
#define GPIO_IDREV_OFFSET       0x00
#define GPIO_CONFIG_OFFSET      0x10
#define GPIO_DIN_OFFSET         0x20
#define GPIO_DOUT_OFFSET        0x24
#define GPIO_DIR_OFFSET         0x28
#define GPIO_DCLEAR_OFFSET      0x2C
#define GPIO_DSET_OFFSET        0x30
#define GPIO_PE_OFFSET          0x40
#define GPIO_PT_OFFSET          0x44
#define GPIO_IE_OFFSET          0x50
#define GPIO_IEMODE_OFFSET(N)   (0x54 + (N) * 4)
#define GPIO_IS_OFFSET          0x64
#define GPIO_BE_OFFSET          0x70    /* BounceEnable */
#define GPIO_BC_OFFSET          0x74    /* BounceControl */

#define GPIO_USED_GCOV          0x02    /* Which GPIO to do GCOV */
#define GPIO_USED_MASK          0x7F    /* Which GPIOs to use */

/************************************
 *  IRQ number                      *
 ************************************/
//#define IRQ_RTC_PERIOD          0
//#define IRQ_RTC_ALARM           1
//#define IRQ_PIT                 0x10
//#define IRQ_UART                2
//#define IRQ_SPI1                3
//#define IRQ_SPI2                4
//#define IRQ_I2C                 5
//#define IRQ_GPIO                6
//#define IRQ_UART1               7
//#define IRQ_UART2               8
//#define IRQ_DMA                 9
//#define IRQ_BMC                 10
//#define IRQ_SWI                 11
//#define IRQ_EXTERNAL(N)         (12 + (N))
//
//#define IRQ_TIMER1              IRQ_PIT
/***********sg9081cc st********************************/
#define IRQ_VMC0                0
#define IRQ_VMC1                1
#define IRQ_WDT0_RST            2
#define IRQ_WDT1_RST            3
#define IRQ_SC                  4
#define IRQ_DMAC_DMA1           5
#define IRQ_DMAC_DMA20          6
#define IRQ_DMAC_DMA21          7
#define IRQ_DMAC_DMA22          8
#define IRQ_DMAC_DMA23          9
#define IRQ_T0                  10
#define IRQ_T1                  11
#define IRQ_PMCP0               12
#define IRQ_PMCP1               13
#define IRQ_FC0_CQ              14
#define IRQ_FC1_CQ              15
#define IRQ_FC2_CQ           	16
#define IRQ_FC3_CQ         		17
#define IRQ_T2	                18
#define IRQ_T3                  19
#define IRQ_CPM                 20
#define IRQ_SF0_INT0            21
#define IRQ_SF0_INT1            22
#define IRQ_SF1_INT0            23
#define IRQ_SF1_INT1            24
#define IRQ_UART0               25
#define IRQ_UART1               26
#define IRQ_UART2               27
#define IRQ_SPI                 28
#define IRQ_EXT_GPIO            29
//#define Reserved                30
//#define Reserved                31

#define IC_VMC0                (1 <<IRQ_VMC0)
#define IC_VMC1                (1 <<IRQ_VMC1)
#define IC_WDT0_RST            (1 <<IRQ_WDT0_RST)
#define IC_WDT1_RST            (1 <<IRQ_WDT1_RST)
#define IC_SC                  (1 <<IRQ_SC)
#define IC_DMAC_DMA1           (1 <<IRQ_DMAC_DMA1)
#define IC_DMAC_DMA20          (1 <<IRQ_DMAC_DMA20)
#define IC_DMAC_DMA21          (1 <<IRQ_DMAC_DMA21)
#define IC_DMAC_DMA22          (1 <<IRQ_DMAC_DMA22)
#define IC_DMAC_DMA23          (1 <<IRQ_DMAC_DMA23)
#define IC_T0                  (1 <<IRQ_T0)
#define IC_T1                  (1 <<IRQ_T1)
#define IC_PMCP0               (1 <<IRQ_PMCP0)
#define IC_PMCP1               (1 <<IRQ_PMCP1)

#define IC_FC0_CQ               (1 <<IRQ_FC0_CQ)
#define IC_FC1_CQ               (1 <<IRQ_FC1_CQ)
#define IC_FC2_CQ               (1 <<IRQ_FC2_CQ)
#define IC_FC3_CQ               (1 <<IRQ_FC3_CQ)

#define IC_T2              	 	(1 <<IRQ_T2)
#define IC_T3              	 	(1 <<IRQ_T3)

#define IC_CPM                 (1 <<IRQ_CPM)
#define IC_SF0_INT0            (1 <<IRQ_SF0_INT0)
#define IC_SF0_INT1            (1 <<IRQ_SF0_INT1)
#define IC_SF1_INT0            (1 <<IRQ_SF1_INT0)
#define IC_SF1_INT1            (1 <<IRQ_SF1_INT1)
#define IC_UART0               (1 <<IRQ_UART0)
#define IC_UART1               (1 <<IRQ_UART1)
#define IC_UART2               (1 <<IRQ_UART2)
#define IC_SPI                 (1 <<IRQ_SPI)
#define IC_EXT_GPIO            (1 <<IRQ_EXT_GPIO)


/***********sg9081cc end********************************/

/* interrupt sources */
//#define IC_SWI                  (1 << IRQ_SWI)
//#define IC_GPIO                 (1 << IRQ_GPIO)
//#define IC_TIMER1               (1 << IRQ_PIT)
//#define IC_UART                 (1 << IRQ_UART)

/* Low-level port I/O */
#define inw(reg)        (*((volatile unsigned int *) (reg)))
#define outw(reg, data) ((*((volatile unsigned int *)(reg)))=(unsigned int)(data))

#ifndef NDS32_INTR_DEF
/*
 * NDS32_REG_SET_BITS(addr, mask)
 * Do set bits to 1 at specified location
 * Operation: *addr = (*addr | mask)
 */
#define NDS32_SET_BITS(addr, mask) outw(addr, inw(addr) | mask)

/*
 * NDS32_REG_WAIT4_BIT_ON(addr, mask)
 * Read the contents at addr and use mask to
 * mask off unused bits then wait until all of
 * the remaining bits are on
 *
 * Operation: while(!(*addr & mask));
 */
#define NDS32_REG_WAIT4_BIT_ON(addr, mask) while(0 == (inw(addr) & mask))

/*
 * NDS32_REG_WAIT4_BIT_OFF(addr, mask)
 * Read the contents at addr and use mask to
 * mask off unused bits then wait until all of
 * the remaining bits are off
 *
 * Operation: while((*addr & mask));
 */
#define NDS32_REG_WAIT4_BIT_OFF(addr, mask) while(inw(addr) & mask)

/*
 * NDS32_REG_WRITE(addr, data)
 * Write data to specified location at addr
 *
 * Operation: *addr = data
 * */
#define NDS32_REG_WRITE(addr, data) outw(addr, data)

/*
 * NDS32_FMEMCPY_BYTE(dst, src, size)
 * Do forward (low address to high address) memory copy in byte
 * */
#define NDS32_FMEMCPY_BYTE(dst, src, size) \
	__asm__ __volatile__( \
		"sethi   $r3, hi20(%2)       \n\t" \
		"ori     $r3, $r3, lo12(%2)  \n\t" \
		"sethi   $r1, hi20(%0)       \n\t" \
		"ori     $r1, $r1, lo12(%0)  \n\t" \
		"sethi   $r2, hi20(%1)       \n\t" \
		"ori     $r2, $r2, lo12(%1)  \n\t" \
		"1:\n\t" \
		"lbi.bi  $r0, [$r2], 1       \n\t" \
		"sbi.bi  $r0, [$r1], 1       \n\t" \
		"addi    $r3, $r3, -1        \n\t" \
		"bnez    $r3, 1b             \n\t" \
		::"i"(dst),"i"(src),"i"(size):"$r0", "$r1","$r2","$r3")

#define NDS32_FMEMCPY(dst, src, size) \
	__asm__ __volatile__( \
		"sethi   $r3, hi20(%2)       \n\t" \
		"ori     $r3, $r3, lo12(%2)  \n\t" \
		"sethi   $r1, hi20(%0)       \n\t" \
		"ori     $r1, $r1, lo12(%0)  \n\t" \
		"sethi   $r2, hi20(%1)       \n\t" \
		"ori     $r2, $r2, lo12(%1)  \n\t" \
		"1:\n\t" \
		"lwi.bi  $r0, [$r2], 4       \n\t" \
		"swi.bi  $r0, [$r1], 4       \n\t" \
		"addi    $r3, $r3, -4        \n\t" \
		"bgtz    $r3, 1b             \n\t" \
		::"i"(dst),"i"(src),"i"(size):"$r0", "$r1","$r2","$r3")

#endif

/************************************
 *  HAL Level : Memory interface    *
 ************************************/
#define HAL_MEMORY_SETUP(_base_)                                        \
do {                                                                    \
        /* Enable DLM */                                                \
        __nds32__mtsr_dsb(_base_ | 1, NDS32_SR_DLMB);                   \
} while(0)

#define HAL_MEMORY_REMAP()
#define HAL_MEMORY_REMAP_ADJUST()

/************************************
 *  HAL Level : Interrupt           *
 ************************************/
#define HW_ISR(n)               HW##n##_ISR
#define SET_HWISR(hw)           HW_ISR(hw)

//ISR function name for PIT/GPIO/SWI */
/***********sg9081cc st********************************/

#define VMC0_IRQHandler           SET_HWISR(IRQ_VMC0)
#define VMC1_IRQHandler		  	  SET_HWISR(IRQ_VMC1)
#define WDT0_IRQHandler           SET_HWISR(IRQ_WDT0_RST)
#define WDT1_IRQHandler		  	  SET_HWISR(IRQ_WDT1_RST)
#define DSATA_IRQHandler          SET_HWISR(IRQ_SC)
#define DMA1_IRQHandler           SET_HWISR(IRQ_DMAC_DMA1)
#define DMA20_IRQHandler          SET_HWISR(IRQ_DMAC_DMA20)
#define DMA21_IRQHandler          SET_HWISR(IRQ_DMAC_DMA21)
#define DMA22_IRQHandler          SET_HWISR(IRQ_DMAC_DMA22)
#define DMA23_IRQHandler          SET_HWISR(IRQ_DMAC_DMA23)
#define SysTick_IRQHandler        SET_HWISR(IRQ_T0)
#define MCU_T1_IRQHandler		  SET_HWISR(IRQ_T1)
#define PMCP0_IRQHandler          SET_HWISR(IRQ_PMCP0)
#define PMCP1_IRQHandler		  SET_HWISR(IRQ_PMCP1)
#define FC0_CQ_IRQHandler         SET_HWISR(IRQ_FC0_CQ)
#define FC1_CQ_IRQHandler         SET_HWISR(IRQ_FC1_CQ)
#define FC2_CQ_IRQHandler         SET_HWISR(IRQ_FC2_CQ)
#define FC3_CQ_IRQHandler         SET_HWISR(IRQ_FC3_CQ)
#define MCU_T2_IRQHandler         SET_HWISR(IRQ_T2)
#define MCU_T3_IRQHandler		  SET_HWISR(IRQ_T3)
#define CPM_IRQHandler		  	  SET_HWISR(IRQ_CPM)
#define SF0_INT0_IRQHandler       SET_HWISR(IRQ_SF0_INT0)
#define SF0_INT1_IRQHandler       SET_HWISR(IRQ_SF0_INT1)
#define SF1_INT0_IRQHandler       SET_HWISR(IRQ_SF1_INT0)
#define SF1_INT1_IRQHandler       SET_HWISR(IRQ_SF1_INT1)
#define UART0_IRQHandler          SET_HWISR(IRQ_UART0)
#define UART1_IRQHandler          SET_HWISR(IRQ_UART1)
#define UART2_IRQHandler          SET_HWISR(IRQ_UART2)
#define SPI_IRQHandler            SET_HWISR(IRQ_SPI)
#define EXT_GPIO_IRQHandler       SET_HWISR(IRQ_EXT_GPIO)

/***********sg9081cc end********************************/

// AE210P without interrupt controller
#define HAL_INTC_HWIRQ_ROUTE(_vector_, _hw_)
#define HAL_INTC_IRQ_CONFIGURE(_irqs_, _edge_, _falling_)
#define HAL_INTC_IRQ_MASK(_irqs_)
#define HAL_INTC_IRQ_UNMASK(_irqs_)
#define HAL_INTC_IRQ_CLEAR(_irqs_)
#define HAL_INTC_FIQ_CONFIGURE(_irqs_, _edge_, _falling_)
#define HAL_INTC_FIQ_MASK(_irqs_)
#define HAL_INTC_FIQ_UNMASK(_irqs_)
#define HAL_INTC_FIQ_CLEAR(_irqs_)


/************************************
 *  HAL Level : Timer               *
 ************************************/
#define HAL_TIMER_INITIALIZE()                                          \
do {                                                                    \
        outw(TM_CHNEN, 0x00000000);                                     \
        outw(TM_CTRL(0), TM_CTRL_32BIT | TM_CTRL_PCLK);                 \
        outw(TM_CTRL(1), TM_CTRL_32BIT | TM_CTRL_PCLK);                 \
        outw(TM_CTRL(2), TM_CTRL_32BIT | TM_CTRL_PCLK);                 \
        outw(TM_CTRL(3), TM_CTRL_32BIT | TM_CTRL_PCLK);                 \
        outw(TM_IE, 0x00000000);                                        \
        outw(TM_STA, 0xFFFFFFFF);                                       \
} while(0)

#define HAL_TIMER_START(_tmr_)                                          \
        outw(TM_CHNEN, inw(TM_CHNEN) | (0x1 << (4 * (_tmr_))))

#define HAL_TIMER_STOP(_tmr_)                                           \
         outw(TM_CHNEN, inw(TM_CHNEN) & ~(0x1 << (4 * (_tmr_))))

#define HAL_TIMER_READ(_tmr_, _pvalue_)                                 \
        *(volatile unsigned int *)_pvalue_ =                            \
                        inw(TM_LOAD(_tmr_)) - inw(TM_CNTR(_tmr_))

#define HAL_TIMER_SET_PERIOD(_tmr_, _period_)                           \
        outw(TM_LOAD(_tmr_), _period_)

#define HAL_TIMER_IRQ_ENABLE(_tmr_, _ie_)                               \
do {                                                                    \
        if(_ie_)                                                        \
                outw(TM_IE, inw(TM_IE) | (0x1 << (4 * (_tmr_))));       \
        else                                                            \
                outw(TM_IE, inw(TM_IE) & ~(0x1 << (4 * (_tmr_))));      \
} while(0)

#define HAL_TIMER_IRQ_STATUS(_tmr_, _pvalue_)                           \
        *(volatile unsigned int *)_pvalue_ =                            \
                        (inw(TM_STA) & (0xF << (4 * (_tmr_))))

#define HAL_TIMER_IRQ_CLR(_tmr_)                                        \
        outw(TM_STA, 0xF << (4 * (_tmr_)))

/************************************
 *  HAL Level : GPIO                *
 ************************************/
#define HAL_GPIO_INITIALIZE(_pin_)                                      \
do {                                                                    \
        outw(GPIO_BASE + GPIO_DIR_OFFSET,                               \
		inw(GPIO_BASE + GPIO_DIR_OFFSET) & ~_pin_);             \
        outw(GPIO_BASE + GPIO_DCLEAR_OFFSET, -1);                       \
        outw(GPIO_BASE + GPIO_IEMODE_OFFSET(0), 0x55555555);            \
        outw(GPIO_BASE + GPIO_IEMODE_OFFSET(1), 0x55555555);            \
        outw(GPIO_BASE + GPIO_IEMODE_OFFSET(2), 0x55555555);            \
        outw(GPIO_BASE + GPIO_IEMODE_OFFSET(3), 0x55555555);            \
        outw(GPIO_BASE + GPIO_BC_OFFSET, 0x000000FF);                   \
        outw(GPIO_BASE + GPIO_BE_OFFSET, _pin_);                        \
        outw(GPIO_BASE + GPIO_IS_OFFSET, -1);                           \
        outw(GPIO_BASE + GPIO_IE_OFFSET, _pin_);                        \
} while(0)

#define HAL_GPIO_READ()                 inw(GPIO_BASE + GPIO_DIN_OFFSET)

#define HAL_GPIO_IRQ_CLR(_pin_)         outw(GPIO_BASE + GPIO_IS_OFFSET, _pin_)

#ifdef __ASSEMBLER__
/************************************
 *  HAL : AE210P defined vectors    *
 ************************************/

/***********sg9081cc st********************************/
.macro  hal_hw_vectors
        vector Interrupt_HW0   //  (9) Interrupt HW0
        vector Interrupt_HW1   // (10) Interrupt HW1
        vector Interrupt_HW2     // (11) Interrupt HW2(wdt0)
        vector Interrupt_HW3     // (12) Interrupt HW3(wdt1)
        vector Interrupt_HW4     // (13) Interrupt HW4(sc)
        vector Interrupt_HW5   // (14) Interrupt HW5
        vector Interrupt_HW6   // (15) Interrupt HW6
        vector Interrupt_HW7   // (16) Interrupt HW7
        vector Interrupt_HW8   // (17) Interrupt HW8
        vector Interrupt_HW9   // (18) Interrupt HW9
        vector Interrupt_HW10    // (19) Interrupt HW10(T0)
        vector Interrupt_HW11   // (20) Interrupt HW11
        vector Interrupt_HW12   // (21) Interrupt HW12
        vector Interrupt_HW13   // (22) Interrupt HW13
        vector Interrupt_HW14    // (23) Interrupt HW14(fc0)
        vector Interrupt_HW15   // (24) Interrupt HW15(fc1)
        vector Interrupt_HW16   // (25) Interrupt HW16(fc2)
        vector Interrupt_HW17   // (26) Interrupt HW17(fc3)
        vector Interrupt_HW18   // (27) InterruptHW18
        vector Interrupt_HW19   // (28) Interrupt HW19
        vector Interrupt_HW20   // (29) Interrupt HW20
        vector Interrupt_HW21    // (30) Interrupt HW21(SF0_INT0)
        vector Interrupt_HW22    // (31) Interrupt HW22(SF0_INT1)
        vector Interrupt_HW23    // (32) Interrupt HW23(SF1_INT0)
        vector Interrupt_HW24    // (33) Interrupt HW24(SF1_INT1)
        vector Interrupt_HW25    // (34) Interrupt HW25(uart0)
        vector Interrupt_HW26    // (35) Interrupt HW26(uart1)
        vector Interrupt_HW27    // (36) Interrupt HW27(uart2)
        vector Interrupt_HW28   // (37) Interrupt HW28
        vector Interrupt_HW29   // (38) Interrupt HW29
        vector Interrupt_UNDEF   // (39) Interrupt HW30
        vector Interrupt_UNDEF   // (40) Interrupt HW31
.endm

.macro	hal_hw_ISR

		Os_Trap_Interrupt_HW	IRQ_VMC0
		Os_Trap_Interrupt_HW   	IRQ_VMC1
		Os_Trap_Interrupt_HW   	IRQ_WDT0_RST
		Os_Trap_Interrupt_HW   	IRQ_WDT1_RST
		Os_Trap_Interrupt_HW   	IRQ_SC
		Os_Trap_Interrupt_HW   	IRQ_DMAC_DMA1
		Os_Trap_Interrupt_HW	IRQ_DMAC_DMA20
		Os_Trap_Interrupt_HW   	IRQ_DMAC_DMA21
		Os_Trap_Interrupt_HW   	IRQ_DMAC_DMA22
		Os_Trap_Interrupt_HW   	IRQ_DMAC_DMA23
		Os_Trap_Interrupt_HW   	IRQ_T0
		Os_Trap_Interrupt_HW   	IRQ_T1
		Os_Trap_Interrupt_HW   	IRQ_PMCP0
		Os_Trap_Interrupt_HW   	IRQ_PMCP1
		Os_Trap_Interrupt_HW   	IRQ_FC0_CQ
		Os_Trap_Interrupt_HW   	IRQ_FC1_CQ
		Os_Trap_Interrupt_HW   	IRQ_FC2_CQ
		Os_Trap_Interrupt_HW   	IRQ_FC3_CQ
		Os_Trap_Interrupt_HW   	IRQ_T2
		Os_Trap_Interrupt_HW   	IRQ_T3
		Os_Trap_Interrupt_HW   	IRQ_CPM
		Os_Trap_Interrupt_HW	IRQ_SF0_INT0
		Os_Trap_Interrupt_HW   	IRQ_SF0_INT1
		Os_Trap_Interrupt_HW   	IRQ_SF1_INT0
		Os_Trap_Interrupt_HW   	IRQ_SF1_INT1
		Os_Trap_Interrupt_HW   	IRQ_UART0
		Os_Trap_Interrupt_HW   	IRQ_UART1
		Os_Trap_Interrupt_HW   	IRQ_UART2
		Os_Trap_Interrupt_HW   	IRQ_SPI
		Os_Trap_Interrupt_HW   	IRQ_EXT_GPIO
.endm
/***********sg9081cc end********************************/


.macro	hal_set_led, val
.endm
#endif  /* __ASSEMBLER__ */

#endif  /* AE210P_REGS */
