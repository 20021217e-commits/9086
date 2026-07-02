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
#include "mcu.h"
#include "sg9081.h"
#include "common.h"
#include <nds32_intrinsic.h>

inline void GIE_ENABLE()
{
    __nds32__setgie_en();
}

inline void GIE_DISABLE()
{
    __nds32__setgie_dis();
    __nds32__dsb();
}
/*
 * enable system performance timer
 * */
inline void __enable_performance()
{
    __nds32__mtsr(0,NDS32_SR_PFM_CTL);//disable func
    __nds32__mtsr(2,NDS32_SR_PRUSR_ACC_CTL);//ENABLE FOR USER MODE
    __nds32__mtsr(0,NDS32_SR_PFMC0);//change count to 0
    __nds32__mtsr(1,NDS32_SR_PFM_CTL);//enable count

}
/*
 * enable system performance timer count  --system clk
 * */
inline U32 __get_pfm_count()
{
    return __nds32__mfsr(NDS32_SR_PFMC0);
}

inline void __disbale_performance()
{
    __nds32__mtsr(0,NDS32_SR_PFM_CTL);
}

inline U32 __get_sda_base()
{
#ifdef __DUAL_CORE__
     U32 gp_val;
     __asm("addi.gp\t%0,#0\n\t"
           "la\t$gp,_SDA_BASE_\n\t"
           :"=r" (gp_val));
     return gp_val;
#else
     return 0;
#endif
}

inline U32 __restore_sda_base(U32 intput_gp)
{
#ifdef __DUAL_CORE__
    U32 ret;
    __asm("move\t$gp,%1\n\t"
            : "=r" (ret)
            : "r" (intput_gp));
    return ret;
#else
    return 0;
#endif
}
inline U32 __get_PRIMASK(void)
{
    U32 dTmp;
    dTmp = __nds32__mfsr(NDS32_SR_PSW);
    return !(dTmp&0x01);
}

inline void __set_PRIMASK(U32 dInt)
{
    if(0 == dInt)
    {
        GIE_ENABLE();
    }
    else if(1 == dInt)
    {
        GIE_DISABLE();
    }
}

inline void __delay_us(U32 time,U32 clkcnt_per_us)
{
    __enable_performance();
    while(__get_pfm_count() <= time*clkcnt_per_us);
    __disbale_performance();
}

inline void __delay_ms(U32 time,U32 clkcnt_per_ms)
{
    __enable_performance();
    while(__get_pfm_count() <= time*clkcnt_per_ms);
    __disbale_performance();
}

inline void g0_disable()
{
    SG_CPM->rMCU_CTRL.bAll=0;//关闭G0
}

inline void g0_enable()
{
    SG_CPM->rMCU_CTRL.bAll=1;//打开G0
}

inline void __memory_powerdown()
{
    //Shut PD1 Domain Power
    SG_CPM->rPD_POWER_CTRL.bAll |= (0x3<<4);	//Enable Power Switch and Isolation cell hardware control mode
    SG_CPM->rPD_POWER_CTRL.bits.SHUT_OFF_POWER_REQ = 0x1;
    __enable_performance();//re enable
    while((!SG_CPM->rPD_POWER_STS.bits.ISO_EN_STS) || (!SG_CPM->rPD_POWER_STS.bits.PG_STS))
    {
            if(__get_pfm_count() >= 100)
            {
                break;
            }
    }
    __disbale_performance();
}

//节省代码
void __nop4__(void)
{
	 __nop(); //4byte
	 __nop();
	 __nop(); //4byte
	 __nop();
}

void __nop16__(void)
{
	 __nop4__();
	 __nop4__();
	 __nop4__();
	 __nop4__();
}
#ifdef __FW__
void __section(".CODE_INIT") __reset_memory_poweron()
#else
inline void __reset_memory_poweron()
#endif
{
    __nds32__mtsr(0,NDS32_SR_PFM_CTL);//disable func
    __nds32__mtsr(2,NDS32_SR_PRUSR_ACC_CTL);//ENABLE FOR USER MODE
    __nds32__mtsr(1,NDS32_SR_PFMC0);//change count to 0
    __nds32__mtsr(1,NDS32_SR_PFM_CTL);//enable count

    //review2:reset 流程直接2ms时间,reset powerdown 模块
#if FOR_EMULATE
    while(__nds32__mfsr(NDS32_SR_PFMC0)<= 1000);//仿真时减少为减短时间
#else
    while(__nds32__mfsr(NDS32_SR_PFMC0)<= 3125*20);//250M/8分频，2ms delay
#endif
    __nds32__mtsr(0,NDS32_SR_PFM_CTL);

    //上电初始化 - 配置clock。
    SG_CPM->rSOFT_GATE0.dAll |= ((U32)0xF<<3);		//bit3~6:FC0~3 system clock
    __nds32__dsb();
    SG_CPM->rSOFT_GATE0.dAll |= ((U32)0x7<<8);		//bit8~9:PMCP0~1; bit10:UART0
    __nds32__dsb();
    SG_CPM->rSOFT_GATE0.dAll |= ((U32)0x3<<14);		//bit14~15
    __nds32__dsb();
    SG_CPM->rSOFT_GATE0.dAll |= ((U32)0x1F<<16);	//bit16~20
    __nds32__dsb();
    SG_CPM->rSOFT_GATE0.dAll |= ((U32)0xF<<25);		//bit25~28:FC0~3 TRX2/RX clock
    __nds32__dsb();

    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0x1<<0);		//bit0
    __nds32__dsb();
    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0x1<<1);		//bit1
    __nds32__dsb();
    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0xF<<4);		//bit4~7
    __nds32__dsb();
    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0x3<<8);		//bit8~9
    __nds32__dsb();
    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0x3<<12);		//bit12~13
    __nds32__dsb();
    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0x1<<20);		//bit20
    __nds32__dsb();
    SG_CPM->rSOFT_GATE1.dAll |= ((U32)0x1<<21);		//bit21
    __nds32__dsb();

    SG_CPM->rSOFT_GATE2.dAll |= ((U32)0xF<<0);		//bit0~3
    __nds32__dsb();
    SG_CPM->rSOFT_GATE2.dAll |= ((U32)0xF<<4);		//bit4~7
    __nds32__dsb();
    SG_CPM->rSOFT_GATE2.dAll |= ((U32)0x3<<8);		//bit8~9
    __nds32__dsb();

}
inline void __memory_poweron(U32 cycle)
{
    //Recovery PD1 Domain Power
    SG_CPM->rPG_EXIT_CNT.dAll = cycle;//review2:去掉这两行，直接变成等待2ms
    SG_CPM->rPD_POWER_CTRL.bits.OPEN_POWER_REQ = 0x1;

    __enable_performance();//re enable   --堆栈初始化之前就要调用所以不能调用函数

    while((SG_CPM->rPD_POWER_STS.bits.ISO_EN_STS) || (SG_CPM->rPD_POWER_STS.bits.PG_STS))
    {
        if(__get_pfm_count()>= 100)
        {
            break;
        }
    }
    __disbale_performance();
}

inline void NVIC_EnableIRQ(U8 bIRQ_NUM)
{
    __nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) | ((U32)1<<bIRQ_NUM), NDS32_SR_INT_MASK2);
}

inline void NVIC_DisableIRQ(U8 bIRQ_NUM)
{
    __nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & (~((U32)1<<bIRQ_NUM)), NDS32_SR_INT_MASK2);
}

inline void NVIC_ClearPendingIRQ(U8 bIRQ_NUM)
{
    __nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_PEND2) | ((U32)1<<bIRQ_NUM), NDS32_SR_INT_PEND2);
}

//0 表示最高优先级，3表示最低优先级
inline void NVIC_SetPriority(U8 bIRQ_NUM,U8 bPriority)
{
    U32 mask=0;
    if(bIRQ_NUM > 15)
    {
        mask=__nds32__mfsr(NDS32_SR_INT_PRI2) & (~((U32)3<<(bIRQ_NUM*2-32)));
        mask|=(U32)bPriority<<(bIRQ_NUM*2-32);
        __nds32__mtsr(mask,NDS32_SR_INT_PRI2);
    }
    else
    {
        mask=__nds32__mfsr(NDS32_SR_INT_PRI2) & (~((U32)3<<(bIRQ_NUM*2)));
        mask|=((U32)bPriority<<(bIRQ_NUM*2));
        __nds32__mtsr(mask,NDS32_SR_INT_PRI);
    }

}

#ifdef __VMC_CODE__
void VMC_Init(U8 bVmcNum,U8 bBankSize,U32 dCodeVmaStartAddr,U32 dCodeVmaEndAddr,U32 dBank0VA,U32 dBank1VA,U32 dBank0PA,U32 dBank1PA)
{
    SG_VMC(bVmcNum)->rVMC_CTRL.bAll = 0;//禁用VMC
    SG_VMC(bVmcNum)->rINT_STS.bAll  = 0;//清除中断状态
    SG_VMC(bVmcNum)->rBANK_SIZE.bAll=bBankSize;
    SG_VMC(bVmcNum)->rPA_MIN=(dCodeVmaStartAddr>>3);//寄存器保存的是8byte地址
    SG_VMC(bVmcNum)->rPA_MAX=(dCodeVmaEndAddr>>3);//寄存器保存的是8byte地址
    SG_VMC(bVmcNum)->rPA0=(dBank0PA>>3);//寄存器保存的是8byte地址
    SG_VMC(bVmcNum)->rPA1=(dBank1PA>>3);//寄存器保存的是8byte地址
    SG_VMC(bVmcNum)->rVA0=(dBank0VA>>3);//寄存器保存的是8byte地址
    SG_VMC(bVmcNum)->rVA1=(dBank1VA>>3);//寄存器保存的是8byte地址
    SG_VMC(bVmcNum)->rINT_EN.bAll  =1;
    SG_VMC(bVmcNum)->rVMC_CTRL.bAll=1;//使能VMC
    NVIC_EnableIRQ(bVmcNum);//使能VMC中断
}

void VMC_UpdateMap(U8 bVmcNum,U8 bBankIdx,U32 dBankVA)
{
    //U32 dVA=(SG_VMC(bNum)->rHIT_ADDR.dAll<<3);
    U8 bBankSize=SG_VMC(bVmcNum)->rBANK_SIZE.bAll;
    //更新这个值得时候默认是关闭VMC1的
    if(bBankIdx == 0)
    {
        //SG_VMC(bVmcNum)->rPA0=((dBankPA)>>(bBankSize+11));//按照BANK SIZE 换算地址
        SG_VMC(bVmcNum)->rVA0=((dBankVA)>>(bBankSize+11));//按照BANK SIZE 换算地址
    }
    else
    {
        //SG_VMC(bVmcNum)->rPA1=((dBankPA)>>(bBankSize+11));//按照BANK SIZE 换算地址
        SG_VMC(bVmcNum)->rVA1=((dBankVA)>>(bBankSize+11));//按照BANK SIZE 换算地址
    }
}

void vmc_irqhandler(U8 bVmcNum)
{
    SG_VMC(bVmcNum)->rVMC_CTRL.bAll=0;//禁用VMC1
    SG_VMC(bVmcNum)->rINT_STS.bAll=0;//清除VMC1中断状态
    //VMC异常事件回调上层应用重新更新映射关系
    if(bVmcNum == 0)
    {
        //if(VMC0_SwapCodeHandler !=NULL)
            //VMC0_SwapCodeHandler((SG_VMC(bNum)->rHIT_ADDR.dAll<<3));
    }
    else
    {
        //if(VMC1_SwapCodeHandler !=NULL)
            //VMC1_SwapCodeHandler((SG_VMC(bNum)->rHIT_ADDR.dAll<<3));
    }
    SG_VMC(bVmcNum)->rVMC_CTRL.bAll=1;//重新使能VMC1
}

/*
 * G3 VMC1异常中断
 * */
void VMC1_IRQHandler(void)
{
    vmc_irqhandler(1);
}

/*
 * G0 VMC0异常中断
 * */
void VMC0_IRQHandler(void)
{
    vmc_irqhandler(0);
}

#endif

#ifdef __VMC_FUNC__
/********************************VMC 相关定义***************************************/
static VMC_SwapCodeHandler_t  *VMC0_SwapCodeHandler=0;
static VMC_SwapCodeHandler_t  *VMC1_SwapCodeHandler=0;

/*
 * VMC1_Init
 * VMC1 模块初始化
 * bNum:表示选择的VMC号可取值
 * 					VMC_NUM0,
 * 					VMC_NUM1
 * bBankSize:配置VMC的bank 大小，该值会影响PA,VA的配置
 * 					VMC_BANKSIZE_4K
 * 					VMC_BANKSIZE_8K
 * VMC_SwapCodeHandler:VMC代码置换接口
 * */
void VMC_Init(U8 bNum,U8 bBankSize,VMC_SwapCodeHandler_t *pHandler)
{
    SG_VMC(bNum)->rVMC_CTRL.bAll=0;//禁用VMC
    SG_VMC(bNum)->rINT_STS.bAll=0;//清除中断状态
    SG_VMC(bNum)->rBANK_SIZE.bAll=bBankSize;
    SG_VMC(bNum)->rPA_MIN=(0x3e0000>>3);//寄存器保存的是8byte地址
    SG_VMC(bNum)->rPA_MAX=(0x4a0000>>3);//寄存器保存的是8byte地址
    SG_VMC(bNum)->rPA0=0;//寄存器保存的是8byte地址
    SG_VMC(bNum)->rPA1=0;//寄存器保存的是8byte地址
    SG_VMC(bNum)->rVA0=0;//寄存器保存的是8byte地址
    SG_VMC(bNum)->rVA1=0;//寄存器保存的是8byte地址
    SG_VMC(bNum)->rINT_EN.bAll=1;
    SG_VMC(bNum)->rVMC_CTRL.bAll=1;//使能VMC
    NVIC_EnableIRQ(bNum);//使能VMC中断

    if(bNum == 0)
    VMC0_SwapCodeHandler=pHandler;//注册VMC0回调事件接口
    else
    VMC1_SwapCodeHandler=pHandler;//注册VMC1回调事件接口
}

/*
 * VMC1_UpdateMap
 * 更新VMC1的映射关系
 * bNum:表示选择的VMC号可取值
 * 					VMC_NUM0,
 * 					VMC_NUM1
 * bIdx:关系表索引，单个VMC提供最多两个映射关系表
 * dPA:代码实际运行时地址
 * */

void VMC_UpdateMap(U8 bNum,U8 bIdx,U32 dPA)
{
    U32 dVA=(SG_VMC(bNum)->rHIT_ADDR.dAll<<3);
    U8 bBankSize=SG_VMC(bNum)->rBANK_SIZE.bAll;
    //更新这个值得时候默认是关闭VMC1的
    if(bIdx == 0)
    {
        SG_VMC(bNum)->rPA0=((dPA)>>(bBankSize+11));//按照BANK SIZE 换算地址
        SG_VMC(bNum)->rVA0=((dVA)>>(bBankSize+11));//按照BANK SIZE 换算地址
    }
    else
    {
        SG_VMC(bNum)->rPA1=((dPA)>>(bBankSize+11));//按照BANK SIZE 换算地址
        SG_VMC(bNum)->rVA1=((dVA)>>(bBankSize+11));//按照BANK SIZE 换算地址
    }
}
//#pragma GCC optimize("O0")
void vmc_irqhandler(U8 bNum)
{
    SG_VMC(bNum)->rVMC_CTRL.bAll=0;//禁用VMC1
    SG_VMC(bNum)->rINT_STS.bAll=0;//清除VMC1中断状态
    //VMC异常事件回调上层应用重新更新映射关系
    if(bNum == 0)
    {
        if(VMC0_SwapCodeHandler !=NULL)
            VMC0_SwapCodeHandler((SG_VMC(bNum)->rHIT_ADDR.dAll<<3));
    }
    else
    {
        if(VMC1_SwapCodeHandler !=NULL)
            VMC1_SwapCodeHandler((SG_VMC(bNum)->rHIT_ADDR.dAll<<3));
    }
    SG_VMC(bNum)->rVMC_CTRL.bAll=1;//重新使能VMC1
}

/*
 * G3 VMC1异常中断
 * */
void VMC1_IRQHandler(void)
{
    vmc_irqhandler(1);
}

/*
 * G0 VMC0异常中断
 * */
void VMC0_IRQHandler(void)
{
    vmc_irqhandler(0);
}

#endif
