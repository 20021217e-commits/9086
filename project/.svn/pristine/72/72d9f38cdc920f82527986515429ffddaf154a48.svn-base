
#include "hal.h"
#include "hal_define.h"
#include "hal_timer.h"
U8_C MTR_CTRL_OFST[4] = {0,0x04,0x8C,0x90};
U8_C MTR_PERIOD_OFST[4] = {0x10,0x14,0x94,0x98};

// *****************************************************************************
// 函数名称: TMR_T0_Init
// 函数功能: 定时器0毫秒数
// 入口参数: 毫秒数
// 出口参数:
// 说    明:TIME0 作为 SYSTEM STICK 功能 1ms 产生一次中断
// *****************************************************************************

void HAL_TMRInit(U8 bTmrNum, U32 dMlisec,U32 dMlisecondUnit)
{
    T_MTR_CTRL *tTmrCtrl = (T_MTR_CTRL *)(SG_MCU_BASE + MTR_CTRL_OFST[bTmrNum]);
    U32 *pMtr = (U32 *)(SG_MCU_BASE + MTR_PERIOD_OFST[bTmrNum]);
    tTmrCtrl->ENABLE = FUNC_DISABLE;
    tTmrCtrl->MODE = 0;
    *pMtr = dMlisec * dMlisecondUnit;
    tTmrCtrl->OV_FLAG = 0;
    tTmrCtrl->ENABLE = FUNC_ENABLE;
}

// *****************************************************************************
// 函数名称: TMR_T1_Delayms
// 函数功能: 定时器1延时毫秒
// 入口参数: 延迟的毫秒数 50M 最大 =85899.345ms, tSataDC.dMilliSecondCnt
// 出口参数:
// 说    明:
// *****************************************************************************
void HAL_TMRDelayMs(U8 bTmrNum, U32 dMlisec,U32 dMicroSecondUnit)
{
    U32 primask=__get_PRIMASK();
    T_MTR_CTRL *tTmrCtrl = (T_MTR_CTRL *)(SG_MCU_BASE + MTR_CTRL_OFST[bTmrNum]);
    U32 *pMtr = (U32 *)(SG_MCU_BASE + MTR_PERIOD_OFST[bTmrNum]);

    __set_PRIMASK(1);
    tTmrCtrl->ENABLE = FUNC_DISABLE;
    *pMtr = dMicroSecondUnit * dMlisec + 1;/*避免0的出现*/;

    //清除可能存在的PENDING中断
    tTmrCtrl->OV_FLAG = 0;

    //用于记录收到FIS命令的时间（上电之后的毫秒计时）
    tTmrCtrl->ENABLE = FUNC_ENABLE;

    while(tTmrCtrl->OV_FLAG == 0);
    tTmrCtrl->OV_FLAG = 0;
    tTmrCtrl->ENABLE = FUNC_DISABLE;
    __set_PRIMASK(primask);
}
// *****************************************************************************
// 函数名称: TMR_T1_Delayus
// 函数功能: 定时器1延时微秒 主频越高误差越小
// 入口参数: 延迟的微秒数 50M 最大 =85899345us,tSataDC.dMicroSecondCnt
// 出口参数: 
// 说    明: 
// *****************************************************************************

void HAL_TMRDelayUs(U8 bTmrNum, U32 dMicrosec,U32 dMlisecondUnit)
{
    U32 primask = __get_PRIMASK();
    T_MTR_CTRL *tTmrCtrl = (T_MTR_CTRL *)(SG_MCU_BASE + MTR_CTRL_OFST[bTmrNum]);
    U32 *pMtr = (U32 *)(SG_MCU_BASE + MTR_PERIOD_OFST[bTmrNum]);

    __set_PRIMASK(1);
    tTmrCtrl->ENABLE = FUNC_DISABLE;
    //不能设定为0
    *pMtr = (dMlisecondUnit * ((dMicrosec >= 1) ? dMicrosec : 1))/*避免0的出现*/;//执行指令时间会导致误差,乘法操作误差大
    //50M 误差=1us
    //清除可能存在的PENDING中断
    tTmrCtrl->OV_FLAG = 0;
    tTmrCtrl->ENABLE = FUNC_ENABLE;//用于记录收到FIS命令的时间（上电之后的毫秒计时）
    while(tTmrCtrl->OV_FLAG == 0);
    tTmrCtrl->OV_FLAG = 0;
    tTmrCtrl->ENABLE = FUNC_DISABLE;

    __set_PRIMASK(primask);
}


void HAL_TMRClearFlag(U8 bTmrNum)
{
    T_MTR_CTRL *tMtrCtrl = (T_MTR_CTRL *)(SG_MCU_BASE + MTR_CTRL_OFST[bTmrNum]);
    tMtrCtrl->OV_FLAG = 0;
}
