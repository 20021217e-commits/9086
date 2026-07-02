// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  mian.c
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
#include "pad.h"
#include "sata_typedef.h"
#include "scsi.h"
#include "stdio.h"
#include "uart.h"
#include "uitility.h"



U8_C code_name[]="G0";

//没有被调用的函数也参加编译的方法示例
__attribute__((used))
void Function_RFU()
{
    //RESERVED FOR FUTURE USE
    while(1);
}
//#pragma GCC optimize("O0")

int main()
{

    extern char __VECTOR_ADDR;
    SG_MCU->rG0_VECTOT_TABLE_OFFSET_ADDRESS = (U32)(&__VECTOR_ADDR);

    if (0 == (SG_CPM->rSYS_RESERVED0&0x00000002))
    {

        SG_PADCTRL->rPHY_XCFGD0 = 0x00000002;
        SG_PADCTRL->rPHY_XCFGD1 = 0x00000030;
        SG_PADCTRL->rPHY_XCFGD2 = 0x00000000;
        SG_PADCTRL->rPHY_XCFGD3.dAll = 0x00000000;

        SG_PADCTRL->rPHY_XCFGA0 = 0xe0020706;
        SG_PADCTRL->rPHY_XCFGA1 = 0x4010C000;
        SG_PADCTRL->rPHY_XCFGA2 = 0x40878f70;
        SG_PADCTRL->rPHY_XCFGA3.dAll = 0x0000084;

    }

    //SystemInit();	//初始化系统时钟 移到crt0.s 文件 -提前将时钟切换到200M(以200M时钟初始化BSS等段)

    UART_Init(UART_NUM,1843200,(U32)aDebugInfoBuffer,tSataDC.dSystemFrequency);//调试信息输出给HOST,1.8Mbps
    delay_ms(10);//确保G3某些注册接口注册完成
    {
        //sata 接口注册
        T_SATA_MOD_ITF *ptSataMI=(T_SATA_MOD_ITF *)&tAsyncBuffer;
        Mem32SetVal((U32*)ptSataMI,0,sizeof(T_SATA_MOD_ITF)>>2);
        ptSataMI->dVirtualLbaStartAddr = DEVICE_CAPCITY; //TODO:需要量产工具根据实际情况进行初始化
        ptSataMI->tMpCfg.dAll=0;//TODO:需要量产工具根据实际情况进行初始化
        DSATA_RegisteInterfaceInfo(ptSataMI);
    }

    {
        //SCSI 接口注册
        T_SCSI_MODULE_ITF*  ptScsiMI=(T_SCSI_MODULE_ITF *)&tAsyncBuffer;
        Mem32SetVal((U32*)ptScsiMI,0,sizeof(T_SCSI_MODULE_ITF)>>2);
        Scsi_RegisteInterfaceInfo(ptScsiMI);
    }

    SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN=0;
    SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=1;
    DSATA_ModuleInit();

    SG_CPM->rMONITOR_CTRL =0x0000;

    //初始化完毕打开MCU总中断
    ERROR_PRINT("APP_G0Code Build @ %s / %s\r\n",__DATE__,__TIME__);
    PRINT_DEC("system clock=",tSataDC.dSystemFrequency);
    GIE_ENABLE();
    while(1)
    {
        DSATA_ModuleProcess();
    }
    return 0;
}
