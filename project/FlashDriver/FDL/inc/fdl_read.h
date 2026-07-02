
#ifndef     __FDL_READ_H__
#define     __FDL_READ_H__

#ifdef __cplusplus
extern "C" {
#endif

	#include "fdl_task_op.h"
	
	// *****************************************************************************
    // 函数说明：读取数据到buffer的地址转换
    // 参数说明：
    //      bChan：通道编号
    // 出口参数:
    // 返回 值：
    // 说   明:
    // *****************************************************************************
    U8 TaskConvertReadClstToBuff(U8 bChanPhy);
	U8 TaskConvertVerifyTLCPage(U8 bChanPhy);

	U8 TaskConvertReadLittleDataToFlash(U8 bChanPhy);

    U8 TaskConvertMergeBuffReadClstToFlash(U8 bChanPhy);

	U8 TaskConvert4KReadClstToBuff(U8 bChanPhy);
	void Convert4KPreRead(U08 bOfts);
	void Convert4KPreReadTable(U08 bOfts);
	void HandlePreReadExCeption(T_FLASH_PHY_ADDR *ptClstAddr, U08 bChanNum, U08 bClstCntCur);
	void Task_MultiCEForContinue(U08 bChanPhy,U08 bTaskErgo);

#ifdef __cplusplus
}
#endif

#endif






