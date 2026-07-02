
#ifndef     __FDL_READ_RETRY_H__    
#define     __FDL_READ_RETRY_H__
#include "fdl_task_op.h"

#ifndef __ALG_SCAN_PROJECT__	//Scan工程不能打开这个宏，retry在G3进行
#define RETRY_IN_G0
#ifdef RETRY_IN_G0
#define __FDL_MCU_MUTUAL__
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NO_RETRY			0
#define NORMAL_RETRY		1
#define ALLONE_ONLY_REREAD	2
#define DMA_ALL_MASK        INVALID_U8

extern U08 bBestOption[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];		//对应的最优option值
extern volatile  U08  bRetryPersistence[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];
extern U8 bFeatureAddrArray[4];

#if (READ_RETRY_ARC_MAX == 0)
extern U08 bSoftDecodeBaseRRT[RRT_PARAM_CNT+1];
extern const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1];
#endif

	typedef struct
	{
		U32 bDmaNum;		//retry哪一个DMA
		U08 bRetryTimes;	//当前dma retry了多少次
		U08 bStartOption;	//从哪一组option配置寄存器进行retry
		U08 bSetOptionEn;	//从重读几次到setfeature的切换标志
		U08 bEnableARC;		//是否使能ARC功能的标志
		U08 bArcTime;
		U08 bEnableReadOffset;	//Micron LP页不使用ARC,使用Offset方式
		U16 wMinSvalue;		//当前retry的dma的svalue最小值
	    U08 bRetryLoopCnt;/*Retry Fail重走Retry*/
		U08 bRev[3];
	}T_RETRY_CTRL;

	typedef struct
	{
		U32 m_dDmaRow;
		U32 m_dUserBaseAddr;
		U08 m_bDmaOffsInPage;
		U08 m_bBuffDmaIndex;
	}T_CB_DMAINFOR;

	typedef struct
	{
		U08 m_bFailDmaCnt[4][4];
		U08 m_bFailDmaCntBackUp[4][4];
		U08 m_bCBFailDmaOffst[4][4];
		U32 m_dFailDmaTotal;
		U32 m_dFailBuffIndex[4][4][16];		//每个片选最大Fail个数为16

		T_CB_DMAINFOR m_tDmaInfor[128];   
		U08 m_bCBClstIdx[128];                 

		U32 m_dDmaBuffAddr;
		U32 m_dDmaInforAddr;
		U08 m_bUserDataLen;
		U08 m_bUserAttachSize;
		U08 m_bSecOfDma;
		U08 m_bDataType;
		U08 m_OpType;
	}T_RETRY_INFOR;

	typedef struct
	{
		U16 wColAddr;
		U16 wSeedIdx;
				
		U32 dRowAddr;
		U32 dBuffIdx;
		U32 dUserDataAddr;
		U32 dDmaBuffAddr;
		U32 dDmaInfoAddr;

		U08 bFlashIdx;
		U08 bMaskInfo;
		U08 bUdSize;
		U08 bSectOfDma;
		U08 bSecOfClst;
	}T_DMA_INFOR;

#ifdef __RETRY_CNT_RECORD__
	typedef enum _RETRY_TYPE
	{
	    RTY_REREAD=0,
	    RTY_OPTION,
	    RTY_ARC,
	    RTY_SOFT,
	    RTY_TIMEOUT,
	}RETRY_TYPE;

	extern U08 bPhyToLogChan[4];
	extern U08 bPhyCeToLogLun[4];
	extern U8_V bFlashInitFail[2][2];
#endif

extern volatile  U08 bG0SetBuffSts;
extern volatile  U08 bG3SetBuffSts;
extern volatile  U08 bG0PmcpSts;
extern volatile  U08 bG3PmcpSts;

#ifdef __RETRY_TABLE_RATE__
extern U16 wRRTUseCnt[RRT_TABLE_CNT + 1];
#endif

	void FDL_CopyBackRetry(void);

	// ******************************Read Retry***********************************
	enum RETRY_STATE
	{
		RETRY_STATE1, 	//retry发送命令阶段
		RETRY_STATE2, 	//启动DMA2阶段
		RETRY_STATE3, 	//wait real done和判断DMAINFO，设置水位阶段
		RETRY_STATE4, 	//3CW软解码阶段
		RETRY_STATE5	//5CW软解码阶段
	}; 

	enum RETRY_MODE
	{
		COPYBACK_RETRY_MODE,	
		READ_RETRY_MODE,	//联动非联动的读流程
		VAR_TLC_PAGE_MODE/*校验CopyBack数据接口的Retry*/
	};


	typedef struct
	{
		U08 bFailCnt[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];			//每个片选Fail Dma的个数
		U32 dFailIndex[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN][16];	//Fail Dma的Index 最多16个(待确认？)
	}T_FAIL_DMA_INFOR;

	typedef struct
	{
		U16 wLastClstOffs[MAX_CHAN_OF_DEV];			//retry前正常读流程最后一个簇偏移
		U16 wRetryLunBit[MAX_CHAN_OF_DEV];			//逻辑lun是否进行了MCU Retry,每bit表示各lun状态,如lun0lun1 重读了,则记为0x0003
		U32 dRetryDma2BufIndex[MAX_CHAN_OF_DEV];		//retry前配置的buffindex
		U32 bRetryDma2TotalSect[MAX_CHAN_OF_DEV];	//retry前配置的total sector
		U08 bLDPCFailFlag[MAX_CHAN_OF_DEV];			//通道fail标记
	}T_RETRY_VAR_RECORD;

	void Retry_SetFlashReg(U8 bChan,U8 bChip,U08 bOptionIdx,U32 dRow);

	#ifdef __PRE_4K_READ_ENABLE__
	U32 FDL_PreRead4KRetryGetInfo(U08 bChan);
	void Retry_PreRestartCQ(U08 bRestartCQEn);
	#endif

	void Retry_MultiDMA(U08 bOnlyReadMaxCnt, U08 bSetOptionMaxCnt, U08 bArcMaxCnt);

    void Retry_SaveTaskException(U8 bChan,U32 dOfstInTask,U8 bRet);
    void FDL_RetryRecordFailInfor(U8 bChan);
	void FDL_RetryRecordFailInfoVerTLC(U8 bChan);

	void FDL_ReadRetry(void);
	U16 FDL_LittleDataRetry(U08 bChan,U08 bChip,U32 dBuffAddr,U32 dRow,U16 wColAddr,U32 dDmaInforAddr);
	void Retry_SetArcStartOffset(U8 bChan,U8 bChip,U08 *pDataVal);//Retry_SetFlashReadOffset
	void Retry_SetFlashReg_Arc(U8 bChan,U8 bChip,U08 bOptionIdx);
#ifdef __cplusplus
}
#endif

#endif






