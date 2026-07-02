
#ifndef     __FDL_COPY_BACK_H__    
#define     __FDL_COPY_BACK_H__

#include "common.h"
#include "flash_driver_struct.h"
#include "fdl_task_op.h"
#include "fdl_read_retry.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __4CH_X_4CE_SUPPORT__
#define CB_LUN_CNT		(16)  //4Chan4Ce
#else
#define CB_LUN_CNT		(8)  //4Chan2Ce
#endif

	//应用场景设置
	#if (NAND_FLASH == MIC_B0KB)
	#define ORDER_PAGE_MAX 	(2)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == MIC_B16A) 
	#define ORDER_PAGE_MAX	(2)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif(NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)
#ifdef __4CH_X_4CE_SUPPORT__
	#define ORDER_PAGE_MAX	(3)
#else
	#define ORDER_PAGE_MAX	(3)
#endif

	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == GCGF)
	#define ORDER_PAGE_MAX 	(2)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == MIC_B17A)||(NAND_FLASH == MIC_B27A)||(NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)
	#define ORDER_PAGE_MAX	(2)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
    #elif (NAND_FLASH == YMTC_B08T)
#ifdef __4CH_X_4CE_SUPPORT__
	#define ORDER_PAGE_MAX	(2)
#else
	#define ORDER_PAGE_MAX	(3)
#endif
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == MIC_M16)
	#define ORDER_PAGE_MAX	(3)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == MIC_N18)
	#define ORDER_PAGE_MAX	(3)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == MIC_N28)
	#define ORDER_PAGE_MAX	(2)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == K9ADGD8U0A)
	#define ORDER_PAGE_MAX	(1)
	#elif (NAND_FLASH == K9ADGD8U0C)
	#define ORDER_PAGE_MAX	(1)
	#elif (NAND_FLASH == MIC_L06B)
	#define ORDER_PAGE_MAX	(1)
	#elif (NAND_FLASH == TOS_BICS3)
	#define ORDER_PAGE_MAX	(3)
	#define CB_READ_CQ_FIFO (0x70)
	#define CB_WRT_CQ_FIFO (0x60)
	#elif (NAND_FLASH == HYNIX_3DV4)
	#define ORDER_PAGE_MAX	(1)
	#define CB_READ_CQ_FIFO (0x60)
	#define CB_WRT_CQ_FIFO (0xA0)
	#elif (NAND_FLASH == HYNIX_8A1M)
	#define ORDER_PAGE_MAX	(3)
	#define CB_READ_CQ_FIFO (0x70)
	#define CB_WRT_CQ_FIFO (0xA0)  //4 Plane 不够，2Plane可能0x60就够
	#else //flash analyse group
	#define ORDER_PAGE_MAX	(0)
	#define CB_READ_CQ_FIFO (0xFF)
	#define CB_WRT_CQ_FIFO (0xFF)
	#endif

	#define RETRY_INFOR_CLEAR_BYTE   (52)

	typedef struct
	{
		U16 wTaskTailInLun[4][CB_LUN_CNT];	
		U16 wTaskHeadInLun[4][CB_LUN_CNT];	
		
		U16 wSrcIndexInLun[4][CB_LUN_CNT];
		U16 wDesIndexInLun[4][CB_LUN_CNT];
		U08 bCurStepInTask[4][CB_LUN_CNT];
	}T_CB_LUN_QUEUE,*pT_CB_LUN_QUEUE;


	//最多支持8个Lun对，即为16个Lun
	typedef struct
	{
		U08 bLunQueue[CB_LUN_CNT];
		U16 wLunBuffIndex[CB_LUN_CNT];
		U08 bLunPlnMod[CB_LUN_CNT];    //低4Bit表示头Plane模式，高4Bit表示尾Plane模式
		U08 bLunTaskCnt[CB_LUN_CNT];
		U08 bValidLun;
		U08 bLunOpStep;
		U08 bLunIdx;
		U08 bTotelLun;
		U32 dLunQueueAddr;
		U08 bDataType;
#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
		U08 bWtCmdMode;
		U08 bRev[2];
#else
		U08 bRev[3];
#endif
#ifdef __TLC_AUTO_FILL__
		U08 bBlkAutoFill[CB_LUN_CNT];
#endif
	}T_DXXDIE_CB_INFOR,*pT_DXXDIE_CB_INFOR;
	
	typedef struct
	{
		U08 m_bLunRowIndex[MAX_CHAN_COUNT][4];
		U08 m_bLunRowCnt[MAX_CHAN_COUNT][4];
		U32 m_dRow[128];
		U08 m_bClstOff[128];
		U08 m_bClstBuffIndex[128];
	}T_TCB_LUNINFOR,*pT_TCB_LUNINFOR;
	
	typedef struct
	{
		U08 bHeadPln;
		U08 bLastPln;
		U08 bLostClst;
		U08 bBuffPgWindow;
		
		U16 wRdIndex;
		U16 wWtIndex;
	}T_TCB_INFOR,*pT_TCBINFOR;



#ifdef __BADBLK_BINDMAP__
	extern T_FDL_BADBLK_BINDMAP_DATA *ptBind;
    extern U08 bPhy2LogMapChan[4];
#endif


	extern T_DXXDIE_CB_INFOR tCB_ProcessCtrl;
	extern T_TCB_INFOR  tCB_TableProcessCtrl;
	extern T_RETRY_INFOR		tRetryInfor;
	extern T_RETRY_VAR_RECORD	tRetryVar;
	extern T_RETRY_CTRL		tRetryCtrl[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];

	void CB_GetTableCopyBackInfor(T_FLASH_PHY_ADDR const *ptDstPage,U16 wPageCnt,U16 wClstLen,U32 dCBBuffSize);
	void CB_GetExDieSlcCopyBackInfor(U32 dBuffSize);
	void CB_GetInDieTlcCopyBackInfor(U32 dBuffSize);
	void CB_GetExDieSlcCopyBackInfor(U32 dBuffSize);
	void CB_GetExDieTlcCopyBackInfor(U32 dBuffSize);
	void CB_GetInDieSlcCopyBackInfor(U32 dBuffSize);
	void CB_ExceptionGetRdFailType(U08 bChan,U08 bCe,U16 wClstIndex,U08 bBuffDmaIndex,U32 dRowAddr,U32 dUdAddr,U08 bDmaOffInPg,U16 wDmaInfor,U08 bClstOff);

	U8 TaskAllocateMultiLunXXdieSlc(U16 bLunPairNum);
	U8 TaskAllocateMultiLunXXdieTlc(U16 bLunPairNum);

	U8 TaskConvertTableDataCopyBackProg(U8 bVoid);
	U8 TaskConvertMultiLunExdieSlcCopyBack(U8 bChan);
	U8 TaskConvertMultiLunExdieTlcCopyBack(U8 bChan);
	U8 TaskConvertMultiLunIndieSlcCopyBack(U8 bVoid);
	U8 TaskConvertMultiLunIndieTlcCopyBack(U8 bLunPairNum);
	U8 TaskConvertMultiLunInnerTlcCopyBack(U8 bLunPairNum);

#ifdef __cplusplus
}
#endif

#endif






