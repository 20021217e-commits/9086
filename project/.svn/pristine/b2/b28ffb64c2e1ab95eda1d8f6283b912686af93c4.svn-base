
#ifndef     __FDL_WRITE_H__    
#define     __FDL_WRITE_H__

#ifdef __cplusplus
extern "C" {
#endif

	#include "fdl_task_op.h"

    // *****************************************************************************
    // 函数说明：分配一个任务中的一个参数（页地址、簇地址）
    // 参数说明：
    //      bTaskIndex：FTL任务所以
    //      wOfstInTask:参数所在的任务中的偏移（逻辑编号）
    // 出口参数: 
    // 返回值：
    // 说   明:
    // *****************************************************************************
    U8 TaskAllocatePageAndClst(U16 wOfstInTask);
    U8 TaskAllocateOrderPage(U16 wOfstInTask);
    void WirteStartDMA2ByCQ(U8 bChan,U32 dOfst,U8 bClstOfPage,U8 bSectOfClst,Joint_Mode joint);

    // *****************************************************************************
    // 函数说明：将buffer数据写入到Flash
    // 参数说明：
    //      bChan：通道编号
    // 出口参数: 
    // 返回值：
    // 说   明:
    // *****************************************************************************
    U8 TaskConvertWritePageToFlash(U8 bChanPhy);
    U8 TaskJointWriteDiscretePageToFlash(U8 bChan);
    U8 TaskConvertWriteLittleDataToFlash(U8 bChanPhy);
	U08 TaskConvertFillBlock(U08 bChan);
    // *****************************************************************************
    // 函数说明：将buffer数据按给定order写入到Flash
    // 参数说明：
    //      bChan：通道编号
    // 出口参数: 
    // 返回值：
    // 说   明:
    // *****************************************************************************
    U8 TaskConvertOrderPrgToFlash(U8 bChanPhy);

    U8 TaskConvertOrderExtPrgToFlash(U8 bChan);
	void FDL_OrderMultiWritePage(U08 bChan, U08 bLun, U32 dRow,  U32 dBuffAddr, Joint_Mode joint, U08 bMultiNum, U08 bBuffIndex,U08 bPrgCycle, U16 wOrderNum, U08 bPageCnt, U32 dUdBuff);
#ifdef _TLC_ORDER_PROGRAM_
	U08 TaskConvertOrderPrgUnit (U08 bChanPhy);
#endif
#ifdef __cplusplus
}
#endif

#endif






