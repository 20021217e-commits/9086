// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  main.c
//  文件标识：  
//  内容摘要：  
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

#include "sg9081.h"
#include "main.h"
#include "debug.h"
#include "uart.h"
#include "sata_typedef.h"
#include "scsi.h"
#include "pri_cmd.h"
#include "fdl_test.h"
#include "hal_test.h"
#include "flash_driver.h"
#include "hal.h"
#include "interface.h"
#include "uitility.h"
#include "CmdHandle.h"
#include "fdl_read_retry.h"
#include "ftl.h"
//#define __JTAG_DEBUG__ //使用jtag下载调试要打开，使用PC下载要关闭！！！


#define __JTAG_DEBUG__ //使用jtag下载调试要打开，使用PC下载要关闭！！！
//T_FDL_PARAM			 tFdlParam;


U8_C code_name[] = "sdk";
U8_C bScanVersion[40] = "SG9081CC_G3_CORE_20170609_SDK_V0.98";
U8 flag_PE_offline = 0;
U8 flag_ReadDisturb_offline = 0;


__align(8) T_SCAN_GLOBAL_VAR gScan __section(".comram2");
extern BOOL bIsDummyRWEn;
void LinkChangeHandle (ENUM_LPS state);
void InterfaceFatalErrorHandle ();
U8 ScsiIntCmdHandle(U8 bOpMode);
U8 ScsiIdleCmdHandle(U8 bOpMode);

    //模拟量产

//	//模拟量产工具初始化表格行为








void register_sata_itf(void)
{
    {
        //sata 接口注册
        T_SATA_MOD_ITF *ptSataMI=(T_SATA_MOD_ITF *)(&gScan.bDMABuff[0]);
        Mem32SetVal((U32*)ptSataMI,0,sizeof(T_SATA_MOD_ITF)>>2);
        ptSataMI->dDmaBuffAddr = (U32)(&gScan.bDMABuff[0]);
        ptSataMI->dDmaBuffSize =	MEM_DMA_BUFF_SIZE;
        ptSataMI->pInterfaceFatalErrorEvent_cb = InterfaceFatalErrorHandle;//SATA 数据传输出错，需要移植者进行出错操作
        ptSataMI->pPHYLinkChangeEvent_cb = LinkChangeHandle;//sata 接口 link层事件，省电模式操作等。
        ptSataMI->pSysTick_Event_cb = FA_HeaterControl;
#ifdef RETRY_IN_G0
        ptSataMI->pReadRetry_Process = FDL_ReadRetry;
#endif
        ptSataMI->pRemove_AllTask_cb = FTL_SATAErrorHandle;

#ifndef __DUAL_CORE__
        ptSataMI->pTaskQueuedEvent_cb = INTF_TaskQueuedProcess;//没有实现该接口，将会默认实现RW RAM的流程
#endif
        DSATA_RegisteInterfaceInfo(ptSataMI);
    }

    {
        //SCSI 接口注册
        T_SCSI_MODULE_ITF*  ptScsiMI=(T_SCSI_MODULE_ITF *)(&gScan.bDMABuff[0]);
        Mem32SetVal((U32*)ptScsiMI,0,sizeof(T_SCSI_MODULE_ITF)>>2);

        ptScsiMI->bFwVersion = (U8*)&bScanVersion[0];
        ptScsiMI->ScsiIntCmdHandleExt = ScsiIntCmdHandle;
        ptScsiMI->ScsiIdleCmdHandleExt = ScsiIdleCmdHandle;
		
        Scsi_RegisteInterfaceInfo(ptScsiMI);
    }
}


////GPIO8 (GPI[0]) for Vpp Power Control, High-Disable by default, Low-Enable.
void FA_GPIOInit(void)
{
	U8 Val;
	Val = SG_PADCTRL->rPAD_PS_CTRL5.bits.GPIO8;
	FDL_Log("GPIO8 PS = %d", Val);
	Val = SG_PADCTRL->rPAD_PE_CTRL5.bits.GPIO8;
	FDL_Log("GPIO8 PE = %d", Val);
	Val = (U8)(SG_PADCTRL->rPORT_MODE16.bits.dVAL>>0)&1;
	FDL_Log("GPIO8 MODE = %d", Val);
	Val = (U8)(SG_PADCTRL->rPORT_DATA16.bits.dVAL>>0)&1;
	FDL_Log("GPIO8 DATA = %d", Val);
	Val = SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO8;
	FDL_Log("GPIO8 FUNC = %d", Val);


	
	SG_PADCTRL->rPORT_DATA16.bits.dVAL |= BIT_SET_1(0); //Data1
	SG_PADCTRL->rPORT_MODE16.bits.dVAL &= BIT_CLR_0(0); //Output
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO8 = 1;			//GPIO
	SG_PADCTRL->rPAD_PS_CTRL5.bits.GPIO8 = 1;			//Pull UP
	SG_PADCTRL->rPAD_PE_CTRL5.bits.GPIO8 = 0;			//Pull DISABLE
	
	
	Val = SG_PADCTRL->rPAD_PS_CTRL5.bits.GPIO8;
	FDL_Log("GPIO8 PS = %d", Val);
	Val = SG_PADCTRL->rPAD_PE_CTRL5.bits.GPIO8;
	FDL_Log("GPIO8 PE = %d", Val);
	Val = (U8)(SG_PADCTRL->rPORT_MODE16.bits.dVAL>>0)&1;
	FDL_Log("GPIO8 MODE = %d", Val);
	Val = (U8)(SG_PADCTRL->rPORT_DATA16.bits.dVAL>>0)&1;
	FDL_Log("GPIO8 DATA = %d", Val);
	Val = SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO8;
	FDL_Log("GPIO8 FUNC = %d", Val);
}

int main()
{



#ifdef __JTAG_DEBUG__
    //SG_CPM->rSYS_RESERVED0 = 0;//上电后此值可能是乱的，清成0 sata才会初始化
#endif

    SystemInit();	//初始化系统

    extern char __G3_VECTOR_ADDR;
    SG_MCU->rG3_VECTOT_TABLE_OFFSET_ADDRESS = (U32)(&__G3_VECTOR_ADDR);

#ifdef __DUAL_CORE__
    extern char __G0_VECTOR_ADDR;
    SG_MCU->rG0_VECTOT_TABLE_OFFSET_ADDRESS = (U32)(&__G0_VECTOR_ADDR);
    SG_CPM->rMCU_CTRL.bits.bVAL=1; //enable G0
    __delay_ms(4,tSataDC.dMilliSecondCnt);//手动开启G0，等待G0初始化完毕
#endif

    HAL_LED_Config(LED0_NUM,LED_MODE_ON,50);//代码运行指示灯

#if(UART_DEBUG_ON == 1)
    UART_Init(UART_NUM,115200,(U32)&aDebugInfoBuffer[0],tSataDC.dSystemFrequency);	//调试信息输出给HOST,1.8Mbps
#endif

    SystemInit();	//初始化系统

    LOG_PRINT("Code Build @ %s / %s\r\n",__DATE__,__TIME__);
    LOG_PRINT("FPGA Ver= %#010lx\r\n", SG_CPM->rRTL_TAG);

    //虚拟VMC demo

    //实际的VMC流程
    //从量产工具配置中初始化VMC流程控制信息指针
    //while(SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN == 1);//FOR VMC DEBUG,卡死以后单步跟踪

    //根据配置初始化VMC，量产工具会有一个默认MAP的配置（上电相关初始化code）
    //TODO:默认情况下boot会加载好BANK0/1,初始化代码段到RAM,所有的初始化代码段、上电运行流程放置在BANK0/1

    register_sata_itf();

#ifndef __DUAL_CORE__
    DSATA_ModuleInit(1);
#endif
    bIsDummyRWEn=FALSE;
    g_tPendingCmdList.dCmdBitMask = 0;
    g_tPendingCmdList.bCmdCnt     = 0;
    g_tPendingCmdList.bCmdIndex	  = 0;
    GIE_ENABLE();//初始化完毕打开MCU总中断

    FDL_Test_CheckDevice();

    FDL_Test_Init();//私有命令测试前通过C0执行初始化，真实FTL需要初始化
    FTL_Initial();
	FA_GPIOInit();
    //FDL_Main();

    while(1)
        {
    #ifdef __DUAL_CORE__
            INTF_TaskQueuedProcess(&tTQC);
    #else
            DSATA_ModuleProcess();
    #endif
			if(flag_PE_offline)
				FA_PE_offline();	//离线PE
			else if(flag_ReadDisturb_offline)
				//FA_ReadDisturb_offline();
				;
		}

    return 0;
}

// *****************************************************************************
// 函数名称: LinkChangeHandle
// 函数功能: SATA LINK 层连接变化事件处理接口
// 入口参数: state 当前接口状态
// 出口参数:
// 说    明:
// *****************************************************************************
void LinkChangeHandle (ENUM_LPS state )
{
#if 0
    U32 __sda_addr=__get_sda_base();
    if(state == LPS_ACTIVE)
    {
        tSataDC.bGenxSpeed=SG_DSATA->rPSSTSR.bits.SPD;

        CODE_PRINT("Current speed=GEN%d\r\n", tSataDC.bGenxSpeed);
        U8 speed=50;
        //SATA 速度指示LED
        if(tSataDC.bGenxSpeed == 1)
        {
            speed=100;
        }
        else if(tSataDC.bGenxSpeed == 2)
        {
            speed=25;
        }
        else if(tSataDC.bGenxSpeed == 3)
        {
             speed=5;
        }
        else
        {
            speed=100;
        }
        HAL_LED_Config(LED1_NUM,LED_MODE_ON,speed);
    }
    else
    {
        HAL_LED_Config(LED1_NUM,LED_MODE_OFF,50);
    }
    __restore_sda_base(__sda_addr);
#else
    U32 __sda_addr=__get_sda_base();
    __restore_sda_base(__sda_addr);
#endif
}

// *****************************************************************************
// 函数名称: InterfaceFatalErrorHandle
// 函数功能: SATA 数据传输出现FATAL error
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void InterfaceFatalErrorHandle ()
{
    U32 __sda_addr=__get_sda_base();
    //SATA接口数据传输出现不可恢复的错误
    //异常清理
    __restore_sda_base(__sda_addr);
}
// *****************************************************************************
// 函数名称: DeepSleepExit
// 函数功能: SATA 端退出DEEPSLEEP休眠事件
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DeepSleepExit ()
{
    U32 __sda_addr=__get_sda_base();
    //重新初始化FC相关寄存器
    //FDL_Init(&tFdlParam);
//    test_print(" DeepSleepExit...\r\n");
    __restore_sda_base(__sda_addr);
}


U8 ScsiIntCmdHandle(U8 bOpMode)
{
#ifdef __DUAL_CORE__
    GIE_DISABLE();
    U32 __sda_addr=__get_sda_base();
#endif
    U8 bDir;
    PRI_CMD *CmdTemp;

    //U8 bDir = ((PRI_CMD *)((U32)&tScsiCmdHead.bDataDefine[0]))->bDir;
    CmdTemp = (PRI_CMD *)((U32)&tScsiCmdHead.bDataDefine[0]);
    bDir = CmdTemp->bDir;

    if (0xC0 == tScsiCmdHead.bOpCode)
    {
        //LOG_PRINT("ScsiIntCmdHandle\r\n");
        if (((OP_RD == bOpMode)&&(OP_RD == bDir)) ||
            ((OP_WR == bOpMode)&&(OP_WR == bDir)))
        {
            tScsiCtl.bVaild = TRUE;
        }

#ifdef __DUAL_CORE__
    __restore_sda_base(__sda_addr);
#endif
        return TRUE;
    }
#ifdef __DUAL_CORE__
    __restore_sda_base(__sda_addr);
    GIE_ENABLE();
#endif
    return FALSE;
}

U8 ScsiIdleCmdHandle(U8 bOpMode)
{
#ifdef __DUAL_CORE__
    GIE_DISABLE();
    U32 __sda_addr=__get_sda_base();
#endif
    if (0xC0 == tScsiCmdHead.bOpCode)
    {
        //LOG_PRINT("ScsiIdleCmdHandle\r\n");
        tScsiCtl.bVaild = FALSE;
        tScsiCtl.bState = CMD_STATE;
        
        MemCopy((U8 *)&tPrivCMD,(U8 *)&tScsiCmdHead.bDataDefine[0],sizeof(tPrivCMD));
        Scsi_JptSwitchHandle(tScsiCmdHead.bSubOpCode);

#ifdef __DUAL_CORE__
    __restore_sda_base(__sda_addr);
    GIE_ENABLE();
#endif
        return TRUE;
    }
#ifdef __DUAL_CORE__
    __restore_sda_base(__sda_addr);
    GIE_ENABLE();
#endif
    return FALSE;
}
