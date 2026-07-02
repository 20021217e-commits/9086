#ifndef __FDL_EXCEPTION_H__
#define __FDL_EXCEPTION_H__

#include "flash_driver_struct.h"
#include "fdl_task_op.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum 
    {
        ENTRANCE_CQ = 0,
        ENTRANCE_DMA,
        ENTRANCE_VDT
    }Exception_Entrace;

    // *****************************************************************************
    // 函数说明：擦除异常处理
    // 参数说明：
    //      tFtlTask：任务指针
    //      wOfstInTask:产生中断的位置，在任务中的偏移
    //      bErrType:异常类型
    // 出口参数: 
    // 返回值：
    // 说   明:
    // *****************************************************************************
    void ExceptionEraseBlock(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);

    // *****************************************************************************
    // 函数说明：读取异常处理
    // 参数说明：
    //      tFtlTask：任务指针
    //      wOfstInTask:产生中断的位置，在任务中的偏移
    //      bErrType:异常类型
    // 出口参数: 
    // 返回值：
    // 说   明:
    // *****************************************************************************
    void ExceptionReadClstToBuff(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);

    // *****************************************************************************
    // 函数说明：写数据异常处理
    // 参数说明：
    //      tFtlTask：任务指针
    //      wOfstInTask:产生中断的位置，在任务中的偏移
    //      bErrType:异常类型
    // 出口参数: 
    // 返回值：
    // 说   明:
    // *****************************************************************************
    void ExceptionWritePageToFlash(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);
    void ExceptionCopyBack(U16 wOfstInTask, U8 bDir, U8 bErrType);
    void ExceptionMergeBuffClstToFlash(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);
	void ExceptionPreRead4K(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);
#ifdef _TLC_ORDER_PROGRAM_
	void ExceptionOrderPrgUnit(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);
#endif	
#ifdef __cplusplus
}
#endif
#endif

