#ifndef     __FDL_COMMON_H__    
#define     __FDL_COMMON_H__

#include "flash_driver_struct.h"
#include "fdl_task_op.h"

#ifdef __cplusplus
extern "C" {
#endif

extern U8 bCqTimeoutFlag[4];

#define RD_STS_70_CMD (0)
#define RD_STS_71_CMD (1)
#define RD_STS_78_CMD (2)
#define RD_STS_FN_CMD (3)
#define CHECK_RB_PIN (4)


#define FORWARD_SEARCH_ADDR_OFST        0  //向前搜索
#define BACKWARD_SEARCH_ADDR_OFST       1  //向后搜索
#define FORWARD_SEARCH_LUN_ADDR_OFST    2  //向前搜索,只判断同一通道LogLun/Chan是否一致
#define BACKWARD_SEARCH_LUN_ADDR_OFST	3  //向后搜索
#define FAIL_SEARCH_ADDR_OFST           0xFFFF //搜索不到时的返回值

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
U32 Fdl_GetBadMapAddr(U32 dRowAddr,U08 bChan,U08 bLunOfChan);
#endif

void Fdl_ConvertBlockInfo(T_ADDR_INFO *pAddr, T_FLASH_PHY_BLK_ADDR *ptBlkAddr);
void Fdl_ConvertAddrInfo(T_ADDR_INFO *pAddr,U32 dRowAddrBuff,TASK_TYPE bTaskType);
#ifdef __VER_TLC_PAGE__
void Fdl_ConvertAddrInfoVerTlc(T_ADDR_INFO *pAddr, U08 bChan,T_CHAN_TASK_QUEUE *tChTaskQ);
#endif

void Fdl_VirtualChanSet(U32 dStart);

void Fdl_PreVirtualChanSet(void);//Aaron
U8 AlgScan_CheckCEValid(U08 bChNum,U08 bChipNum);
void  Fdl_ReadPreSendCmd(U08 bChan,U32 dRow);

void Fdl_SendReadCmd(U08 bChan, U08 bLun, U32 dRow, U32 wCol,U08 bMultiFlag);
void Fdl_ClearExceptionState(U8 bChan);

U16  Fdl_ConvertColAddr(U08 bDataType,U08 bFlashIdx);
U16  Fdl_SearchOfstByAddr(U8 bFailChan,U8 bFailLogLun,U32 dAddr,U16 wOfst,U08 bMode);
U08  Fdl_AddrModeAnalysis(T_ADDR_INFO *pCurrAddr, T_ADDR_INFO *pNextAddr);
U08  Fdl_McuCheckStatus(U8 bChan,U8 bRdStsMode, U8 bLunOfChip,U32 dRowAddr, U8 bFirstCheckIOBit, U8 bSecondCheckIOBit);
void Fdl_PreRead4K(T_FLASH_PHY_ADDR *ptClstAddr,U08 bClstLen);

void Fdl_CQPreReadMultiCe(U08 bChanPhy,T_CHAN_TASK_QUEUE *tChTaskQ);
void Fdl_CQPreReadMultiPln(U08 bChanPhy,U08 bLunOfChan,U32 dPln0RowOp,U08 bLastReadCmd);
void Fdl_CQPreReadFixPln(U08 bChanPhy,U08 bLunOfChan,U32 dPln0RowOp,U16 wColAddr,U08 bPlaneCnt,U08 bLastReadCmd);

void Fdl_CQPreReadMultiPlaneCacheCmd(U08 bChanPhy,U08 bLunOfChan,T_CHAN_TASK_QUEUE *tChTaskQ);
U32 Fdl_ED3RowAnalyze(U32 dRow,U32 dRowNext,U08 bMode);
void Fdl_CheckCacheRDEnd(U08 bChanPhy);
void Fdl_RaidEnCtrl(U08 bEn);
void Fdl_ConvertAddrInfo(T_ADDR_INFO *pAddr,U32 dRowAddrBuff,TASK_TYPE bTaskType);
void Fdl_CQPreReadSinglePln(U08 bChan, U08 bChip, U32 wCol, U32 dRow,U08 bLastReadCmd);
#ifdef __TASK_ERROR_FORCE_STOP__
void Fdl_UpdateTaskTime(U08 bTaskErgo);
U08 Fdl_GetTaskHoldFlag(void);
void FDL_StopTaskForce(void);
#endif
void Fdl_PreReadMultiCe(T_FLASH_PHY_ADDR *ptClstAddr);
#ifdef __TASK_REORDER_TO_READ_TABLE__	/*读表格Row地址重排序功能*/
void Fdl_TaskReOrderReadTable(void);
#endif


#if (NAND_FLASH == TOS_BICS3)
void BCS3_PowerONReadFlash(U32 dRowOp,U16 wBlkLen);  
#endif


#ifdef __FILL_PAGE_EN__
//void FDL_JCOrginalInfoInit(U32 dJCOrginalInfoAddr,U32 dJCOrginalInfoBuffSize);
U16 FDL_JCTableCreat(U32 dDMAJCTableAddr,U32 dJCOrginalInfoAddr);
void FDL_JCOrginalInfoInitForB08T(U32 dJCOrginalInfoAddr,U32 dJCOrginalInfoBuffSize,U08 bDataType);

#endif

void Fdl_ResetDMA2AndFC(U08 bChan);
void Fdl_ResetDMA2(U08 bChanPhy);

#ifdef __TASK_ERROR_FORCE_STOP__	
void FDL_StopTaskForceTiger(U08 bChan);
#endif
#ifdef __cplusplus
}
#endif

#endif

