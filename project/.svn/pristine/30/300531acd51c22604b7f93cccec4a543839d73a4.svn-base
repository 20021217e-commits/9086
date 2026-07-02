#include "fdl_copyback.h"
#include "Hal.h"
#include "fdl_read_retry.h"
#include "fdl_task_op.h"
#include "Hal_cq.h"
#include "fdl_common.h"
#include "uitility.h"
#include "flash_driver.h"
#include "fdl_read_retry.h"
#include "hal_pmcp.h"

T_TCB_INFOR  tCB_TableProcessCtrl;
T_DXXDIE_CB_INFOR tCB_ProcessCtrl;

U32_V dWrFailAddr = 0;

U32 CB_AnalysisCopyBackBuffSize(U32 dCBBuffSize)
{
#ifndef __SIMPLIFY_CODE__   //0 //fcl保证给的是128/256/384/512
	if((dCBBuffSize >= 128)&&(dCBBuffSize < 256))//64K的情况
	{
		dCBBuffSize = 128;
	}
	else if((dCBBuffSize >= 256)&&(dCBBuffSize < 384))
	{
		dCBBuffSize = 256;
	}
	else if((dCBBuffSize >= 384)&&(dCBBuffSize < 512))
	{
		dCBBuffSize = 384;
	}
	else if(dCBBuffSize == 512)
	{
		
	}
	else
	{
		PRINT_TXT("CB_PamaExcept");
		while(1);
	}
#endif
	return dCBBuffSize;
}


U8 CB_GetMaxTaskChan(U8 *bValidLunInChan)
{
	U08 i;
	U08 bMaxChan = 0xFF;

	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		if(bValidLunInChan[i] != 0)
		{
			if(bMaxChan == 0xFF)
			{
				bMaxChan = i;
			}
			else
			{
				if(bValidLunInChan[i]>bValidLunInChan[bMaxChan])
				{
					bMaxChan = i;
				}
			}
		}
	}

	if(bMaxChan != 0xFF)
	{
		bValidLunInChan[bMaxChan] = 0;
	}
	
	return bMaxChan;
}

//#pragma GCC optimize("O0")
void CB_AnalysisInDieLunQueue(U8 *pbChanInLunQueue)
{
#ifndef __SIMPLIFY_CODE__
	U08 i;
	U08 *pbLunQueue = (U8 *)(&tCB_ProcessCtrl.bLunQueue[0]);
	U08 bValidLunInChan[4] = {0,0,0,0};
	U08 bValidLunInChanTemp[4];
	U08 bLunInChan[4][4];
	
	MemorySet((U8 *)(&(bLunInChan[0][0])), 0xFF,CB_LUN_CNT);
			
	U08 bChan,bLun;
	U08 bIndex = 0;
	for(bChan = 0;bChan<CHAN_MAX_CNT;bChan++)
	{
		bIndex = 0;
		for(bLun = 0;bLun<tFtlTask.wLen;bLun++)
		{
			if(bChan == pbChanInLunQueue[bLun])
			{
				bLunInChan[bChan][bIndex] = bLun;
				bValidLunInChan[bChan]++;
				bIndex++;
				pbChanInLunQueue[bLun] = 0xFF;
			}
		}
	}
	memcpy((U8 *)bValidLunInChanTemp,(U8 *)bValidLunInChan, 4);
	
	U08 bValidChan = 0;
	U08 bValidChanTemp = 0;
	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		if(bValidLunInChan[i] != 0)
		{
			bValidChan++;
		}
	}
	bValidChanTemp = bValidChan;

	U08 bMaxChan;
	U08 bIdx = 0;
	U08 bChanQue[4] = {0xFF,0xFF,0xFF,0xFF};
	while(bValidChanTemp)
	{
		bMaxChan = CB_GetMaxTaskChan(&bValidLunInChanTemp[0]);  
		if(bMaxChan != 0xFF)
		{
			bChanQue[bIdx] = bMaxChan;
			bIdx++;
			bValidChanTemp--;
		}
	}

	bIdx = 0;
	U8 bIndexInChan[4] = {0,0,0,0};

	for(bLun = 0;bLun<tFtlTask.wLen;bLun++)
	{
		if(pbLunQueue[bLun]!=0xFF)
		{
			while(1)
			{
				bChan = bChanQue[bIdx%bValidChan];
				bIdx++;
				if(bValidLunInChan[bChan] != 0)
				{
					break;
				}
			}
			pbLunQueue[bLun] = bLunInChan[bChan][bIndexInChan[bChan]];
			bIndexInChan[bChan]++;
			bValidLunInChan[bChan]--;
		}
	}
#endif
}

U8 CB_AnalysisInDieLunStep(U08 bValidLun,U32 dCBBuffSize)
{
	U08 bLunStep = dCBBuffSize/(PLANE_OF_LUN*DSECT_OF_PAGE);

#if (READ_RETRY_ARC_MAX != 0)

	if(tFtlTask.dTaskType == TYPE_MULTI_LUN_INDIE_COPYBACK_ORDER_PRG)
	{
		if(tCB_ProcessCtrl.bDataType != 0)
		{
			bLunStep = bLunStep/ORDER_PAGE_MAX;
		}
	}
#endif

	if(bValidLun>bLunStep)
	{
		return bLunStep;
	}
	else
	{
		return bValidLun;
	}
}

U16 CB_AnalysisLogicClstQueue(U32 *ptClstAddr,U8 *pReadClstQueue,U16 wBaseIndex,U08 bSearchIndex,U08 bSearchCnt,U08 bSetLable)
{
	U16 i;
	ptClstAddr = ptClstAddr+bSearchIndex+wBaseIndex;
	U08 *pRdClstQue = pReadClstQueue+bSearchIndex;
	U08 bNestClst = 0;
	
	pRdClstQue[0] = bSetLable;

	U32 dBaseAddr = ptClstAddr[0];
	U32 dNestAddr;

	U08 bOpCnt = 1;
	
	for(i = 1; i<bSearchCnt;i++)
	{
		if(pRdClstQue[i] == 0xFF)
		{
			dNestAddr = ptClstAddr[i];
			
			if((dNestAddr&PLANE_BIT_MASK_OF_ROW) == (dBaseAddr&PLANE_BIT_MASK_OF_ROW))//多Plane操作满足 或者Cache读满足
			{				
				pRdClstQue[i] = bSetLable;
				bOpCnt++;				
			}
			else
			{
				break;
			}
		}
	}

	if(bSearchCnt > 1)
	{
		for(i = 1;i<=bSearchCnt-1;i++)
		{
			if(pRdClstQue[i] == 0xFF)//表示未分配
			{
				bNestClst = bSearchIndex+i;
				break;
			}
		}
	}
	else
	{
		bNestClst = bSearchIndex;
	}

	return ((bNestClst<<8)|bOpCnt);
}


void CB_AnalysisWriteMultiPlnInfor(void)
{
	U16 i;
	U08 bPlane;
	U32 dRow;

	U08 bTaskCntTemp[CB_LUN_CNT];
	U08 bHeadPg;
	U08 bMidPg;
	U08 bLastPg;
	
	T_DST_INFO_OF_LUN *ptDstInfo = (T_DST_INFO_OF_LUN *)tFtlTask.dDstArgsAddr;

	U8 *bMPlnMode = (U8 *)(&tCB_ProcessCtrl.bLunPlnMod[0]);
	U08 *bLunTaskCnt = (U8 *)(&tCB_ProcessCtrl.bLunTaskCnt[0]);

	MemorySet((U8 *)(&bTaskCntTemp[0]), 0,CB_LUN_CNT);

	for(i = 0;i<tFtlTask.wLen;i++)
	{
		bMPlnMode[i] = 0;
		bHeadPg = 0;
		bMidPg = 0;
		bLastPg = 0;
		
		if(ptDstInfo[i].wPageCnt)
		{
			dRow = ptDstInfo[i].pdRowAddr[0];
			bPlane = dRow/ROW_GAP_OF_PLANE%PLANE_OF_LUN;
			
			bHeadPg = Min(ptDstInfo[i].wPageCnt,(PLANE_OF_LUN-bPlane)) ;
			bMPlnMode[i] |= bHeadPg;
				
			bPlane = (ptDstInfo[i].wPageCnt-bHeadPg)%PLANE_OF_LUN;

			if(ptDstInfo[i].wPageCnt-bHeadPg != 0)
			{
				if(bPlane == 0)
				{
					bMPlnMode[i] |= (PLANE_OF_LUN<<4);
					bLastPg = PLANE_OF_LUN;
				}
				else
				{
					bMPlnMode[i] |= (bPlane<<4);
					bLastPg = bPlane;
				}
			}
			else
			{
				bLastPg = 0;
			}
			
			bMidPg = (ptDstInfo[i].wPageCnt-bHeadPg-bLastPg)/PLANE_OF_LUN;

			if(bHeadPg != 0 )
			{
				bHeadPg = 1;
			}

			if(bLastPg != 0)
			{
				bLastPg = 1;
			}
			bTaskCntTemp[i] = bHeadPg+bLastPg+bMidPg;
		}
		else
		{
			bMPlnMode[i] = 0xFF;
			bTaskCntTemp[i] = 0;
		}
	}
	
	for(i = 0;i<tFtlTask.wLen;i++)
	{
		bLunTaskCnt[i] = bTaskCntTemp[i];
	}
}

void CB_LogicRetryInforInit(void)
{
	tRetryInfor.m_bUserDataLen = UD_LEN_OF_DATA_CLST;
	tRetryInfor.m_bUserAttachSize = SECT_OF_DATA_CLST;
	tRetryInfor.m_bSecOfDma = DSECT_OF_DMA;
	tRetryInfor.m_bDataType = TYPE_LOGIC;
	tRetryInfor.m_dDmaBuffAddr = tFtlTask.dHandleBuff;
	tRetryInfor.m_dDmaInforAddr = tFtlTask.dEccStsBuff;
	tRetryInfor.m_OpType = 0;
	MemorySet((U08 *)(&tRetryInfor), 0, RETRY_INFOR_CLEAR_BYTE);
}

void CB_TableRetryInforInit(void)
{	
	tRetryInfor.m_bUserDataLen = UD_LEN_OF_TBLE_CLST;
	tRetryInfor.m_bUserAttachSize = SECT_OF_TBLE_CLST;
	tRetryInfor.m_bSecOfDma = TSECT_OF_DMA;
	tRetryInfor.m_bDataType = TYPE_TABLE;
	tRetryInfor.m_dDmaBuffAddr = tFtlTask.dHandleBuff;
	tRetryInfor.m_dDmaInforAddr = tFtlTask.dEccStsBuff;
	tRetryInfor.m_OpType = 0;
	MemorySet((U08 *)(&tRetryInfor), 0,RETRY_INFOR_CLEAR_BYTE);
}

void CB_GetTableCopyBackInfor(T_FLASH_PHY_ADDR const *ptDstPage,U16 wPageCnt,U16 wClstLen,U32 dCBBuffSize)
{
	U08 bFistOpPg = 0;
	U08 bLastOpPg = 0;
	
	U08 bFistOpCnt = 1;
	U08 bLastOpCnt = 0;
	U16 wMidOpCnt = 0;

	U16 wRemainPage;
	
	dCBBuffSize =  CB_AnalysisCopyBackBuffSize(dCBBuffSize);	
	MemorySet((U8 *)(&(tCB_TableProcessCtrl)),0,sizeof(T_TCB_INFOR));
	
	tCB_TableProcessCtrl.bBuffPgWindow = dCBBuffSize/TSECT_OF_PAGE;
	
	tCB_TableProcessCtrl.wRdIndex = 0;
	tCB_TableProcessCtrl.wWtIndex = 0;
	tCB_TableProcessCtrl.bLastPln = 0;
	tCB_TableProcessCtrl.bLostClst = wPageCnt*TBLE_CLST_OF_PAGE-wClstLen;
	bFistOpPg = PLANE_OF_LUN - (ptDstPage[0].m_dRowAddr/ROW_GAP_OF_PLANE)%PLANE_OF_LUN;

	tCB_TableProcessCtrl.bHeadPln  = Min(bFistOpPg, wPageCnt);
	wRemainPage = wPageCnt-tCB_TableProcessCtrl.bHeadPln;

	if(wRemainPage)
	{
		bLastOpPg = wRemainPage%PLANE_OF_LUN;

		if(bLastOpPg == 0)
		{
			bLastOpPg = PLANE_OF_LUN;
			tCB_TableProcessCtrl.bLastPln = PLANE_OF_LUN;
		}
		else
		{
			tCB_TableProcessCtrl.bLastPln = bLastOpPg;
		}
		bLastOpCnt = 1;
		
		wRemainPage -= tCB_TableProcessCtrl.bLastPln;
		wMidOpCnt = wRemainPage/PLANE_OF_LUN;
	}
	
    tFtlTask.wCur = 0;
    tFtlTask.wLen = bFistOpCnt+bLastOpCnt+wMidOpCnt;
	
	CB_TableRetryInforInit();
}

void CB_GetInDieSlcCopyBackInfor(U32 dBuffSize)
{
	U08 i;
	T_DST_INFO_OF_LUN *ptDstInfo = (T_DST_INFO_OF_LUN *)tFtlTask.dDstArgsAddr;

	U08 bChanInLunQueue[CB_LUN_CNT];
	MemorySet((U8 *)(&(tCB_ProcessCtrl)),0,sizeof(T_DXXDIE_CB_INFOR));
	MemorySet(bChanInLunQueue, 0xFF,CB_LUN_CNT);

	U08 bValidLun = 0;
	for(i = 0;i<tFtlTask.wLen;i++)
	{
		if(ptDstInfo[i].wPageCnt != 0)
		{		
			bChanInLunQueue[i] = ptDstInfo[i].bChan;
			tCB_ProcessCtrl.bLunQueue[i] = i;
			bValidLun++;
		}
		else
		{
			bChanInLunQueue[i] = 0xFF;
			tCB_ProcessCtrl.bLunQueue[i] = 0xFF;
		}
	}
	
	if(LUN_OF_CHAN != 1)
	{
		CB_AnalysisInDieLunQueue(bChanInLunQueue);
	}
	CB_AnalysisWriteMultiPlnInfor();
	
	dBuffSize =  CB_AnalysisCopyBackBuffSize(dBuffSize);
	tCB_ProcessCtrl.bLunOpStep = CB_AnalysisInDieLunStep(bValidLun,dBuffSize);
	tCB_ProcessCtrl.dLunQueueAddr = (U32)(&(tChTaskQueue[0].wLastOfstInTask));
	tCB_ProcessCtrl.bValidLun = bValidLun;
	tCB_ProcessCtrl.bTotelLun = tFtlTask.wLen;
	tCB_ProcessCtrl.bLunIdx = 0;
	MemorySet((U8 *)tCB_ProcessCtrl.dLunQueueAddr, 0,sizeof(T_CB_LUN_QUEUE));

	CB_LogicRetryInforInit();
}

void CB_GetInDieTlcCopyBackInfor(U32 dBuffSize)
{
	U08 i;
	T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr;
#if (READ_RETRY_ARC_MAX != 0)
	T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo = (T_IN_COPY_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr;
#endif	
	MemorySet((U8 *)(&(tCB_ProcessCtrl)),0,sizeof(T_DXXDIE_CB_INFOR));
	
	U08 bChanInLunQueue[CB_LUN_CNT];
	MemorySet(bChanInLunQueue, 0xFF,CB_LUN_CNT);

	U08 bValidLun = 0;
	for(i = 0;i<tFtlTask.wLen;i++)
	{
		if(ptDstInfo[i].wOrderUnitCnt != 0)
		{		
			bChanInLunQueue[i] = ptDstInfo[i].bChan;
			tCB_ProcessCtrl.bLunQueue[i] = i;
			tCB_ProcessCtrl.bLunTaskCnt[i] = ptDstInfo[i].wOrderUnitCnt;
			tCB_ProcessCtrl.bLunPlnMod[i] = PLANE_OF_LUN;
#if (READ_RETRY_ARC_MAX != 0)

			if(tCB_ProcessCtrl.bDataType == 0)
			{
				if(ptSrcInfo[i].pdRowAddr[0] != 0xFFFFFFFF)
				{
					tCB_ProcessCtrl.bDataType = ptSrcInfo[i].pdRowAddr[0]>>24;
				}
			}
#endif
			bValidLun++;
		}
		else
		{
			bChanInLunQueue[i] = 0xFF;
			tCB_ProcessCtrl.bLunQueue[i] = 0xFF;
			tCB_ProcessCtrl.bLunTaskCnt[i] = 0;
			tCB_ProcessCtrl.bLunPlnMod[i] = 0xFF;
		}
	}

	if(LUN_OF_CHAN != 1)
	{
		CB_AnalysisInDieLunQueue(bChanInLunQueue);
	}
	
	dBuffSize =  CB_AnalysisCopyBackBuffSize(dBuffSize);
	tCB_ProcessCtrl.bValidLun = bValidLun;

#if 0
  	U08 bMaxLunOp = dBuffSize/(PLANE_OF_LUN*DSECT_OF_PAGE*ORDER_PAGE_MAX);

  if(CHIP_OF_CHAN >= 2)
  {	
	tCB_ProcessCtrl.bDataType = 1;

    if(bValidLun < bMaxLunOp)
    {
      tCB_ProcessCtrl.bLunOpStep = bValidLun;
    }
    else
    {
      if(bMaxLunOp >= CHAN_MAX_CNT)
      {
        tCB_ProcessCtrl.bLunOpStep = CHAN_MAX_CNT;
      }
      else
      {
        tCB_ProcessCtrl.bLunOpStep = bMaxLunOp;
      }
    }
  }
  else
  {
    tCB_ProcessCtrl.bLunOpStep = CB_AnalysisInDieLunStep(bValidLun,dBuffSize);
  }
#else

  	U08 bMaxLunOp = dBuffSize/(PLANE_OF_LUN*DSECT_OF_PAGE*ORDER_PAGE_MAX);
  	tCB_ProcessCtrl.bLunOpStep = Min(bMaxLunOp,CHAN_MAX_CNT);
	tCB_ProcessCtrl.bLunOpStep = Min(tCB_ProcessCtrl.bLunOpStep,bValidLun);
	tCB_ProcessCtrl.bDataType  = 1;
#endif
	tCB_ProcessCtrl.dLunQueueAddr = (U32)(&(tChTaskQueue[0].wLastOfstInTask));
	tCB_ProcessCtrl.bTotelLun = tFtlTask.wLen;
	tCB_ProcessCtrl.bLunIdx = 0;
	MemorySet((U8 *)tCB_ProcessCtrl.dLunQueueAddr, 0,sizeof(T_CB_LUN_QUEUE));

	CB_LogicRetryInforInit();
}

void CB_GetExDieSlcCopyBackInfor(U32 dBuffSize)
{
	U08 i;
	U16 wBuffIndex = 0;
	U08 bIndexGap  = PLANE_OF_LUN*DSECT_OF_PAGE;
	T_DST_INFO_OF_LUN *ptDstInfo = (T_DST_INFO_OF_LUN *)tFtlTask.dDstArgsAddr;
	MemorySet((U8 *)(&(tCB_ProcessCtrl)),0,sizeof(T_DXXDIE_CB_INFOR));
	dBuffSize =  CB_AnalysisCopyBackBuffSize(dBuffSize);

	for(i = 0;i<tFtlTask.wLen;i++)
	{
		if(ptDstInfo[i].wPageCnt != 0)
		{
			tCB_ProcessCtrl.bLunQueue[i] = i;
			tCB_ProcessCtrl.wLunBuffIndex[i] = wBuffIndex;
			wBuffIndex+=bIndexGap;
		}
		else
		{
			tCB_ProcessCtrl.bLunQueue[i] = 0xFF;
			tCB_ProcessCtrl.wLunBuffIndex[i] = 0xFFFF;
		}
	}
	
	CB_AnalysisWriteMultiPlnInfor();
	tCB_ProcessCtrl.dLunQueueAddr = (U32)(&(tChTaskQueue[0].wLastOfstInTask));
	MemorySet((U8 *)tCB_ProcessCtrl.dLunQueueAddr, 0,sizeof(T_CB_LUN_QUEUE));
	CB_LogicRetryInforInit();
}

void CB_GetExDieTlcCopyBackInfor(U32 dBuffSize)
{
	U08 i;
	U16 wBuffIndex = 0;
	U08 bIndexGap  = PLANE_OF_LUN*DSECT_OF_PAGE;
	T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo = (T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr;
	MemorySet((U8 *)(&(tCB_ProcessCtrl)),0,sizeof(T_DXXDIE_CB_INFOR));
	
	dBuffSize =  CB_AnalysisCopyBackBuffSize(dBuffSize);

	for(i = 0;i<tFtlTask.wLen;i++)
	{
		if(ptDstInfo[i].wOrderUnitCnt != 0)
		{
			tCB_ProcessCtrl.bLunQueue[i] = i;
			tCB_ProcessCtrl.wLunBuffIndex[i] = wBuffIndex;
			wBuffIndex+=bIndexGap;
		}
		else
		{
			tCB_ProcessCtrl.bLunQueue[i] = 0xFF;
			tCB_ProcessCtrl.wLunBuffIndex[i] = 0xFFFF;
		}
	}		
	
	tCB_ProcessCtrl.dLunQueueAddr = (U32)(&(tChTaskQueue[0].wLastOfstInTask));
	MemorySet((U8 *)tCB_ProcessCtrl.dLunQueueAddr, 0,sizeof(T_CB_LUN_QUEUE));
	CB_LogicRetryInforInit();
}

void CB_CheckWriteRealDone(U08 bChan,U08 bCheckBit)
{
	U08 i,bCqFifoFlag;
	for(i = 0;i<LUN_OF_CHAN;i++)
	{
		bCqFifoFlag = HAL_CQWaitIdleSpace(bChan,0x60);
	
		if(bCqFifoFlag)
		{
			U08 bChip = i / LUN_OF_CHIP;
			bChip = CHIPES(bChan, bChip);
			HAL_CQChipSelect(bChan,bChip);
			HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
			HAL_CQWaitDone(bChan);
			if(SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA & (bCheckBit))
			{
				U32 blogchan =bPhyToLogChan[bChan];
				U32 dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,TLC_MODE,blogchan,i,0x00FFFFFF);//((dDesRow&0xFFFFFF)|(((U32)bDesChan)<<24)|(pDstInfo->bLunOfChan<<26));
				//wLastWtFailCnt++;                
                pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
			}
		}
	}	
}

void CB_InnerCopyBackRead(U8 bChan, U8 bLun, U32 *dRowAdd,U08 bMulPln)
{
	U08 bPln;
	U08 bChip = bLun / LUN_OF_CHIP;
	bChip = CHIPES(bChan, bChip);
	
	HAL_CQChipSelect(bChan,bChip);

	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);

	FL_CMD_CQ_SLC_MODE(bChan);

	for(bPln = 0;bPln<bMulPln;bPln++)
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_COPY_BACK_READ1);
		HAL_CQSend5Addr(bChan,0,dRowAdd[bPln]);

		if(bPln+1 == bMulPln)
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_COPY_BACK_READ2);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_READ_MP);
			HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);
		}
	}
}

void CB_InnerCopyBackWrite(U8 bChan, U8 bLun, U32 dRowAdd,U08 bMulPln)
{
	U08 bPln;
	U08 bChip = bLun / LUN_OF_CHIP;
	bChip = CHIPES(bChan, bChip);
	
	HAL_CQChipSelect(bChan,bChip);

	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_CPYB);
	
	FL_CMD_CQ_TLC_MODE(bChan);

	for(bPln = 0;bPln<bMulPln;bPln++)
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_COPY_BACK_PROG1);
		HAL_CQSend5Addr(bChan,0,dRowAdd);
		
		if(bPln+1 == bMulPln)
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_COPY_BACK_PROG2);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2_MP);
			HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_CPYB);
		}
		dRowAdd+=ROW_GAP_OF_PLANE;
	}
}


void CB_InnerCopyBackStartDma2(U8 bChan, U8 bLun, U32 *dRowAdd,U32 dDmaAddr,U32 *pdUserAddr,U08 bMultiPln,U08 *bDmaOutInfor,U32 dDmaInforAddr)
{
	U08 i;
	U08 bPln;
	U08 bCqFifoFlag;
	U08 bChip = bLun / LUN_OF_CHIP;
	
	bChip = CHIPES(bChan, bChip);
	
	HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_CPYB);

	U08 bInfor;
	U16 wColAddr;
	U32 dUserAddr;

	for(bPln = 0;bPln<bMultiPln;bPln++)
	{
		bInfor = bDmaOutInfor[bPln];
		dUserAddr = pdUserAddr[bPln];
		
		bCqFifoFlag = HAL_CQWaitIdleSpace(bChan,0x90);

		if(bCqFifoFlag)
		{
			for(i = 0;i<DATA_CLST_OF_PAGE;i++)
			{
				if((bInfor>>i)&0x1)
				{
					wColAddr = i * SECT_OF_DATA_CLST / DSECT_OF_DMA+1;
					wColAddr = DATA_COL_ADDR(wColAddr);
								
					HAL_CQSetRndSeed(bChan,dRowAdd[bPln],INVALID_U16);		
					HAL_CQSend5RndAddr(bChan, wColAddr,dRowAdd[bPln]);

					SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
					HAL_CQSetUd(bChan,dUserAddr,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,1);
					
					U08 bFlashIdx = i* SECT_OF_DATA_CLST+DSECT_OF_DMA;
					HAL_CQStartDMA2(bChan,dDmaAddr, DMA2_RD_TO_BUFF, 0, bFlashIdx, DSECT_OF_DMA, JOINT_NONE, dDmaInforAddr);
					dDmaInforAddr+=2;	
					
					HAL_CQChipSelect(bChan,bChip);
				}
			}
		}
	}
}

void CB_MultiPlnWriteTablePg(U8 bChan, U8 bLun, U32 dRow, U16 wBuffIndex, U32 dBuffAddr, U32 dUserDataBuff, U8 bJointMode,U08 bPlnCnt)
{
	U8 bChip;
    T_FLASH_OPER_PARAM *m_tFlashOP = &(pFdlParam.ptUdpFdl->m_tFlashOperParam);

    bChip = bLun / LUN_OF_CHIP;
    bChip = CHIPES(bChan,bChip);

    HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);

	U08 bPln;
	U08 bPrgCmd1 = m_tFlashOP->tMultiPln.bPln1WRCmd;
	U08 bPrgCmd0;
	
	if(bPlnCnt == 1)
	{
		bPrgCmd0 = FL_CMD_PAGE_PROG1;
	}
	else
	{
		bPrgCmd0 = m_tFlashOP->tMultiPln.bPln0WRCmd;
	}
		
	FL_CMD_CQ_SLC_MODE(bChan);

	for(bPln = 0;bPln<bPlnCnt;bPln++)
	{
		if(bPln != 0)
		{
			HAL_CQSendFlashCmd(bChan,bPrgCmd1);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan,bPrgCmd0);
		}	
		
		HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRow);
		
		HAL_CQSetRndSeed(bChan,dRow,INVALID_U16);
		HAL_CQSetUd(bChan,dUserDataBuff,SECT_OF_TBLE_CLST, UD_LEN_OF_TBLE_CLST,DMA2_WR_TO_FLASH);
		
		HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, (wBuffIndex+bPln*TSECT_OF_PAGE), 0, TSECT_OF_PAGE, bJointMode, (U32)NULL);

		if(bPln+1 == bPlnCnt)
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2);  
		}
		else
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2_MP);
            HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
		}
		
		dUserDataBuff+=(UD_LEN_OF_TBLE_CLST*TBLE_CLST_OF_PAGE);
		dRow += ROW_GAP_OF_PLANE;
	}
}

#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
void CB_SinglePlnWriteLogicPgSLC(U8 bChan, U8 bLun, U32 dRow,U16 wBuffIndex, U32 dBuffAddr, U32 dUserAddr,U08 bPlnCnt)
{
	U8 bChip;
	U32 dRowAddr;
	U08 bChkStsMode;

	bChip = bLun / LUN_OF_CHIP;
	bChip = CHIPES(bChan,bChip);

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);

	U08 bPage;
	for(bPage = 0;bPage<bPlnCnt;bPage++)
	{
		HAL_CQWaitIdleSpace(bChan,CB_WRT_CQ_FIFO);
		dRowAddr = dRow + bPage*ROW_OF_BLOCK;

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
		dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif
		bChkStsMode = FIO_PF_PASS_SLC_WRITE;
		FL_CMD_CQ_SLC_MODE(bChan);

		HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
		HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
		HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
		HAL_CQSetUd(bChan,dUserAddr,SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
		HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex, 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);
		HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2);

		HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
		HAL_CQWaitDone(bChan);//Check上一次编程是否Fail

		wBuffIndex += DSECT_OF_PAGE;
		dUserAddr += (UD_LEN_OF_DATA_CLST*DATA_CLST_OF_PAGE);

		if(SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA & (bChkStsMode))
		{
			dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,SLC_MODE,bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],dRowAddr);
			pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
		}
	}
}
#if (NAND_FLASH == SUMSUNG_3DV4 || NAND_FLASH == AFGM)
void CB_SinglePlnWriteLogicPgTLC(U8 bChan, U8 bLun, U8 bPln, U32* dRow, U16* wBuffIndex, U32 dBuffAddr, U32* dUserAddr,U08 bPageCnt, U08 bSlcOrTlc)
{
	U08 bChip;
	U08 bLastCmd;
	U32 dRowAddr;
	U08 bRowMode;
	U08 bChkStsMode;
	U16 wPrgpage;
	U16 wOrder;

	PRINT_HEX("into CB_SinglePlnWriteLogicPgTLC,bChan:0x%x",bChan);
	PRINT_HEX("bLun:0x%x",bLun);
	PRINT_HEX("bPln:0x%x",bPln);
	PRINT_HEX("pdRow:0x%x",dRow);
	PRINT_HEX("pwBuffIndex:0x%x",wBuffIndex);
	PRINT_HEX("dBuffAddr:0x%x",dBuffAddr);
	PRINT_HEX("pdUserAddr:0x%x",dUserAddr);
	PRINT_HEX("bPageCnt:0x%x",bPageCnt);
	PRINT_HEX("bSlcOrTlc:0x%x",bSlcOrTlc);
	//LOG_PRINT("into CB_SinglePlnWriteLogicPgTLC,bChan:0x%x,bLun:0x%x,bPln:0x%x,dBuffAddr:0x%x,bPageCnt:0x%x,bSlcOrTlc:0x%x",bChan,bLun,bPln,dBuffAddr,bPageCnt,bSlcOrTlc);

    bChip = bLun / LUN_OF_CHIP;
    bChip = CHIPES(bChan,bChip);

    HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);

	U08 bPage;
	for(bPage = 0;bPage<bPageCnt;bPage++)
	{
		HAL_CQWaitIdleSpace(bChan,CB_WRT_CQ_FIFO);
		dRowAddr = dRow[bPage];
		bRowMode = (U08)GET_TLC_FLAG(dRowAddr);//select highest byte ,SLC:0x0  TLC:0x01/0x02/0x03

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
		dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif

#ifdef __BADBLK_BINDMAP_FDLTEST__/*Plane Mapping Jupiter Test*/
		dRowAddr = Fdb_CopyBackRowCov(dRowAddr);
#endif

		if(bSlcOrTlc == SLC_MODE)//SLC Mode
		{
			bLastCmd 	= FL_CMD_PAGE_PROG2;	//0x10
			bChkStsMode = FIO_PF_PASS_SLC_WRITE;
			FL_CMD_CQ_SLC_MODE(bChan);
		}
#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
		else if(bRowMode == 1 || bRowMode == 2)
		{
			bLastCmd = FL_CMD_ED3_PRO_CMD;	//0x1A
			bChkStsMode = FIO_PF_PASS_TLC_WRITE;
			FL_CMD_CQ_ED3_READ_WL(bChan,GET_TLC_FLAG(dRowAddr));
		}
#endif
		else//TLC Mode
		{
			//bLastCmd = FL_CMD_PAGE_PROG2;	//0x10

			bChkStsMode = FIO_PF_PASS_TLC_WRITE;
			FL_CMD_CQ_TLC_MODE(bChan);
			FL_CMD_CQ_ED3_READ_WL(bChan,GET_TLC_FLAG(dRowAddr));

			// get Order of page
			wPrgpage = dRowAddr & 0x3FF;
			if ((wPrgpage < 0x08) || ((wPrgpage >= 0x2F0)&&(wPrgpage < 0x2F8)))
			{
				wOrder = wPrgpage/2;
				if (wPrgpage>= 0x2F0)
				{
					wOrder -=124;
				}
				//tTlcOp->wWL = wOrder;
				//tTlcOp->bPrgCycle =TLC_PRG_CYCLE_2ND+(wPrgpage & 0x01);
			}
			else if (wPrgpage >= 0x2F8) //最后部份
			{
				wOrder = wPrgpage-504;
				//tTlcOp->wWL = wOrder;
				//tTlcOp->bPrgCycle = TLC_PRG_CYCLE_3RD; //最后一项
			}
			else
			{
				wOrder = (wPrgpage-8)/3+4;
				//tTlcOp->wWL = wOrder;
				//tTlcOp->bPrgCycle = (wPrgpage-8)%3; //最后一项 0，1，2
			}
		}

		if(bSlcOrTlc == SLC_MODE)//SLC Mode
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
			HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
			HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
			HAL_CQSetUd(bChan,dUserAddr[bPage],SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
			HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex[bPage], 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);

			HAL_CQSendFlashCmd(bChan,bLastCmd);
		}
		else//TLC Mode
		{
			if(bPageCnt == 3)	//TLC page of TLC Mode
			{
				HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
				HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
				HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
				HAL_CQSetUd(bChan,dUserAddr[bPage],SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
				HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex[bPage], 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);

				HAL_CQSendFlashCmd(bChan,0xC0);
				HAL_CQSendAddr(bChan, (bPln * 0x10 + 0x11) + bPage);	//pln0:11h,12h,13h; pln1:21h,22h,23h; pln0&pln1:31h,32h,33h
			}
			else
			{
				if(bPageCnt == 2)	//MLC page of TLC Mode
				{
					if(bPage == 0)
					{	// Dummy Data Setup
						HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
						HAL_CQSend5Addr(bChan,0x0,0x0);	//addr fix 0x0

						HAL_CQSendFlashCmd(bChan,0xC0);
						HAL_CQSendAddr(bChan, (bPln * 0x10 + 0x11) + bPage);	//pln0:11h,12h,13h; pln1:21h,22h,23h; pln0&pln1:31h,32h,33h
						HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
						HAL_CQWaitDone(bChan);//Check上一次编程是否Fail

//						if(SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA & (bChkStsMode))
//						{
//							dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,bSlcOrTlc,bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],dRowAddr);
//							pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
//						}
						// 2nd page data setup
						HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
						HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
						HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
						HAL_CQSetUd(bChan,dUserAddr[bPage],SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
						HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex[bPage], 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);

						HAL_CQSendFlashCmd(bChan,0xC0);
						HAL_CQSendAddr(bChan, (bPln * 0x10 + 0x11) + bPage + 1);	//pln0:11h,12h,13h; pln1:21h,22h,23h; pln0&pln1:31h,32h,33h
					}
					else
					{
						// 3rd page data setup
						HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
						HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
						HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
						HAL_CQSetUd(bChan,dUserAddr[bPage],SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
						HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex[bPage], 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);

						HAL_CQSendFlashCmd(bChan,0xC0);
						HAL_CQSendAddr(bChan, (bPln * 0x10 + 0x11) + bPage + 1);	//pln0:11h,12h,13h; pln1:21h,22h,23h; pln0&pln1:31h,32h,33h
					}
				}
				else
				{
					// 3rd page data setup
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
					HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
					HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
					HAL_CQSetUd(bChan,dUserAddr[bPage],SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
					HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex[bPage], 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);

					HAL_CQSendFlashCmd(bChan,0xC0);
					HAL_CQSendAddr(bChan, (bPln * 0x10 + 0x11) + bPage + 2);	//pln0:11h,12h,13h; pln1:21h,22h,23h; pln0&pln1:31h,32h,33h
				}
			}
		}

		HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
		HAL_CQWaitDone(bChan);//Check上一次编程是否Fail

		if(SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA & (bChkStsMode))
		{
			dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,bSlcOrTlc,bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],dRowAddr);
			pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
		}
	}
	// program confirm part
	if(bSlcOrTlc == TLC_MODE)//TLC Mode
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_PROG_CONFIRM1);
		HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),wOrder);	//wOrder_addr of page

		HAL_CQSendFlashCmd(bChan,FL_CMD_PROG_CONFIRM2);
	}

}
#else
void CB_SinglePlnWriteLogicPgTLC(U8 bChan, U8 bLun, U32* dRow, U16* wBuffIndex, U32 dBuffAddr, U32* dUserAddr,U08 bPageCnt, U08 bSlcOrTlc)
{
	U08 bChip;
	U08 bLastCmd;
	U32 dRowAddr;
	U08 bRowMode;
	U08 bChkStsMode;

    bChip = bLun / LUN_OF_CHIP;
    bChip = CHIPES(bChan,bChip);

    HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);

	U08 bPage;
	for(bPage = 0;bPage<bPageCnt;bPage++)
	{
		HAL_CQWaitIdleSpace(bChan,CB_WRT_CQ_FIFO);
		dRowAddr = dRow[bPage];
		bRowMode = (U08)GET_TLC_FLAG(dRowAddr);//select highest byte ,SLC:0x0  TLC:0x01/0x02/0x03

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
		dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif

#ifdef __BADBLK_BINDMAP_FDLTEST__/*Plane Mapping Jupiter Test*/
		dRowAddr = Fdb_CopyBackRowCov(dRowAddr);
#endif

		if(bSlcOrTlc == SLC_MODE)//SLC Mode
		{
			bLastCmd 	= FL_CMD_PAGE_PROG2;	//0x10
			bChkStsMode = FIO_PF_PASS_SLC_WRITE;
			FL_CMD_CQ_SLC_MODE(bChan);
		}
#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
		else if(bRowMode == 1 || bRowMode == 2)
		{
			bLastCmd = FL_CMD_ED3_PRO_CMD;	//0x1A
			bChkStsMode = FIO_PF_PASS_TLC_WRITE;
			FL_CMD_CQ_ED3_READ_WL(bChan,GET_TLC_FLAG(dRowAddr));
		}
#endif
		else//TLC Mode
		{
#if (NAND_FLASH == HYNIX_8A1M)
			bLastCmd = FL_CMD_ED3_PRO2_CMD;
#else
			bLastCmd = FL_CMD_PAGE_PROG2;	//0x10
#endif
			bChkStsMode = FIO_PF_PASS_TLC_WRITE;
			FL_CMD_CQ_TLC_MODE(bChan);
			FL_CMD_CQ_ED3_READ_WL(bChan,GET_TLC_FLAG(dRowAddr));
		}

		HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
		HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
		HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
		HAL_CQSetUd(bChan,dUserAddr[bPage],SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
		HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, wBuffIndex[bPage], 0, DSECT_OF_PAGE, JOINT_NONE, (U32)NULL);
		HAL_CQSendFlashCmd(bChan,bLastCmd);

		HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
		HAL_CQWaitDone(bChan);//Check上一次编程是否Fail

		if(SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA & (bChkStsMode))
		{
			dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,bSlcOrTlc,bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],dRowAddr);
			pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
		}
	}
}
#endif
#endif

#if (NAND_FLASH == SUMSUNG_3DV4 || NAND_FLASH == AFGM)
void CB_MultiPlnWriteLogicPg(U8 bChan, U8 bLun, U32 dRow, U16 bWLNum, U08 bWLOff, U08 bOderPage, U16 wBuffIndex, U32 dBuffAddr, U32 dUserAddr,U8 bJointMode,U08 bPlnCnt,U08 bSlcOrTlc)
{
	U8 bChip;
	U32 dRowAddr;

    //T_FLASH_OPER_PARAM *m_tFlashOP = &(pFdlParam.ptUdpFdl->m_tFlashOperParam);

    bChip = bLun / LUN_OF_CHIP;
    bChip = CHIPES(bChan,bChip);

	U08 bPln;
	U08 bPlaneAddr[2] = {0x31,0x32};
	U08 bPageCnt = 1;
	U08 i;
		
    HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);

	if(bSlcOrTlc == 0)
	{
		FL_CMD_CQ_SLC_MODE(bChan);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x50;

		for(bPln = 0;bPln<bPlnCnt;bPln++)
		{
			dRowAddr = dRow+bPln*ROW_GAP_OF_PLANE;
			
#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
			dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif
		
			if(bPln != 0)
			{
				HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1_MP);
			}
			else
			{
				HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
			}	
		
			HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
			HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
			HAL_CQSetUd(bChan,dUserAddr,SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
			HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, (wBuffIndex+bPln*DSECT_OF_PAGE), 0, DSECT_OF_PAGE, bJointMode, (U32)NULL);
			dUserAddr+=DATA_CLST_OF_PAGE*UD_LEN_OF_DATA_CLST;
		
			if(bPlnCnt == 1)
			{
				HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2);
			}
			else
			{
				if(bPln == 0)
				{
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2_MP);
					HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
				}
				else
				{
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2);
				}
			}
		}
	}
	else
	{
		FL_CMD_CQ_TLC_MODE(bChan);

		if(bOderPage == 2)	// MLC
		{
			if(bWLOff == 1)
			{
				bPageCnt = 2;	// MLC fix_page+2nd_page
			}
			else if(bWLOff == 2)
			{
				bPlaneAddr[0] = 0x33;	// MLC 3nd_page
			}
		}
		else if(bOderPage == 1)	// SLC
		{
			bPlaneAddr[0] = 0x33;	// SLC 3nd_page
		}
		else	// TLC
		{
			bPlaneAddr[0] = 0x30+bWLOff;	// TLC 1nd_page/2nd_page/3nd_page
		}

		for(i = 0;i<bPageCnt;i++)
		{
			for(bPln = 0;bPln<bPlnCnt;bPln++)
			{
				if((bPageCnt == 2)&&(i == 0))
				{
					dRowAddr = 0+bPln*ROW_GAP_OF_PLANE;
				}
				else
				{
					dRowAddr = dRow+bPln*ROW_GAP_OF_PLANE;
				}

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
				dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif
		
				if(bPln != 0)
				{
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1_MP);
				}
				else
				{
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG1);
				}	
				HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);

				if((bPageCnt != 2)||(i != 0))
				{
					HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
					HAL_CQSetUd(bChan,dUserAddr,SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
					HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, (wBuffIndex+bPln*DSECT_OF_PAGE), 0, DSECT_OF_PAGE, bJointMode, (U32)NULL);
					dUserAddr+=DATA_CLST_OF_PAGE*UD_LEN_OF_DATA_CLST;
				}
		
				if(bPln == 0)
				{
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2_MP);  
				}
				else
				{
					HAL_CQSendFlashCmd(bChan,0xC0);  
					HAL_CQSendAddr(bChan, bPlaneAddr[i]);
				}
				HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
			}
		}
		

		if(bPlaneAddr[0] == 0x33)
		{
			for(bPln = 0;bPln<bPlnCnt;bPln++)
			{
				dRowAddr = dRow+bPln*ROW_GAP_OF_PLANE;
				
				HAL_CQSendFlashCmd(bChan,0x8B);  
				dRowAddr &= (0xFFFFFF);
				dRowAddr = dRowAddr/ROW_GAP_OF_PLANE*ROW_GAP_OF_PLANE+bWLNum;

#ifdef __BADBLK_BINDMAP__	//BadBlkBindMap
				dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif
				
				HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);

				if(bPln == 0)
				{
					HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2_MP);
					HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
				}
				else
				{
					HAL_CQSendFlashCmd(bChan,0x10);
				}	
			}
		}
	}
}
#else
void CB_MultiPlnWriteLogicPg(U8 bChan, U8 bLun, U32 dRow,U16 wBuffIndex, U32 dBuffAddr, U32 dUserAddr,U8 bJointMode,U08 bPlnCnt,U08 bSlcOrTlc)
{
	U8 bChip;
	U32 dRowAddr;

    T_FLASH_OPER_PARAM *m_tFlashOP = &(pFdlParam.ptUdpFdl->m_tFlashOperParam);

    bChip = bLun / LUN_OF_CHIP;
    bChip = CHIPES(bChan,bChip);

    HAL_CQChipSelect(bChan, bChip);

	U08 bPln;
	U08 bPrgCmd1 = m_tFlashOP->tMultiPln.bPln1WRCmd;
	U08 bPrgCmd0;
	U08 bLastCmd;

#if (NAND_FLASH == TOS_BICS3) || (NAND_FLASH == HYNIX_8A1M)
	U08 dRowMode = (U08)(dRow>>24);//select highest byte ,SLC:0x0  TLC:0x01/0x02/0x03

	if (dRowMode == 0)//SLC Mode
	{
		bLastCmd = FL_CMD_PAGE_PROG2;
	}
	else if (dRowMode==1 || dRowMode==2)//
	{
		bLastCmd = FL_CMD_ED3_PRO_CMD;	//0x1A
	}
	else
	{
		bLastCmd = FL_CMD_ED3_PRO2_CMD;//0x10
	}
#elif ((NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M))
	if(GET_PAGE_NUM(dRow)%3 == 2)
	{		
		bLastCmd = FL_CMD_PAGE_PROG2;
	}
	else
	{		
		bLastCmd = FL_CMD_ED3_PRO_CMD;
	}
#else
	bLastCmd = FL_CMD_PAGE_PROG2;
#endif
	HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);

	if(bPlnCnt == 1)
	{
		bPrgCmd0 = FL_CMD_PAGE_PROG1;
	}
	else
	{
		bPrgCmd0 = m_tFlashOP->tMultiPln.bPln0WRCmd;
	}

	if(bSlcOrTlc == 0)
	{
		bLastCmd = FL_CMD_PAGE_PROG2;/*B08T SLC改为10h编程*/
		FL_CMD_CQ_SLC_MODE(bChan);
	}
	else
	{
		FL_CMD_CQ_TLC_MODE(bChan);
	}

	for(bPln = 0;bPln<bPlnCnt;bPln++)
	{
		dRowAddr = dRow+bPln*ROW_GAP_OF_PLANE;

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
		dRowAddr = pFdlParam.FTL_GetBadMapAddr(dRowAddr,bPhy2LogMapChan[bChan],bLun);
#endif

#if (NAND_FLASH == TOS_BICS3  || NAND_FLASH == HYNIX_3DV4) || (NAND_FLASH == HYNIX_8A1M)
		U08 dRowMode = (U08)(dRowAddr>>24);//select highest byte ,SLC:0x0  TLC:0x01/0x02/0x03

		if(dRowMode == 0)//SLC Mode
		{
			FL_CMD_CQ_SLC_MODE(bChan);
		}
		else//TLC Mode
		{
			FL_CMD_CQ_TLC_MODE(bChan);
			FL_CMD_CQ_ED3_READ_WL(bChan,dRowMode);
		}
#endif

		if(bPln != 0)
		{
			HAL_CQSendFlashCmd(bChan,bPrgCmd1);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan,bPrgCmd0);
		}	
		
		HAL_CQSend5Addr(bChan,DATA_COL_ADDR(0),dRowAddr);
		HAL_CQSetRndSeed(bChan,dRowAddr,INVALID_U16);
		HAL_CQSetUd(bChan,dUserAddr,SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
		HAL_CQStartDMA2(bChan,dBuffAddr, DMA2_WR_TO_FLASH, (wBuffIndex+bPln*DSECT_OF_PAGE), 0, DSECT_OF_PAGE, bJointMode, (U32)NULL);
		if(bPln+1 == bPlnCnt)
		{
			HAL_CQSendFlashCmd(bChan,bLastCmd);  
		}
		else
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_PAGE_PROG2_MP);
            HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS,FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
		}
		dUserAddr+=DATA_CLST_OF_PAGE*UD_LEN_OF_DATA_CLST;
	}
}
#endif

void CB_SendMultiPlnReadCmd(U8 bChan, U8 bLun, U32 dRow,U08 bTableDataEn,U08 bMultiPln)
{		
    U08 bChip = bLun / LUN_OF_CHIP;
	U08 bLunOfChip = bLun % LUN_OF_CHIP;
    bChip = CHIPES(bChan, bChip);

	U08 bSlcOrTlc;
	U08 bPln;
	U08 bPlnCnt = 1;
	U32 dRowAddr;

	dRow = dRow&PLANE_BIT_MASK_OF_ROW;/*强制搞成Plane0*/

    HAL_CQChipSelect(bChan,bChip);

	if(bMultiPln)
	{
		bPlnCnt = PLANE_OF_LUN;
	}
			
	for(bPln = 0;bPln < bPlnCnt;bPln++)
	{
		dRowAddr = dRow+bPln*ROW_GAP_OF_PLANE;

		if(bPln != 0)
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_READ_MP);
		}	
		
		HAL_CQCheckIOStatus(bChan, bLunOfChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);

#if (NAND_FLASH != TOS_BICS3) && (NAND_FLASH != HYNIX_8A1M)
		if(bPln == 0)
#endif		
		{
			if(bTableDataEn)
			{			
				FL_CMD_CQ_SLC_MODE(bChan);
			}
			else
			{			
				bSlcOrTlc = (U08)(dRowAddr>>24);	
				if(bSlcOrTlc == 0)
				{
					FL_CMD_CQ_SLC_MODE(bChan);
				}
				else
				{
					FL_CMD_CQ_TLC_MODE(bChan);
					FL_CMD_CQ_ED3_READ_WL(bChan,bSlcOrTlc);
				}
			}
		}
		
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
		HAL_CQSend5Addr(bChan,0,dRowAddr);
	}
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);  
}

void CB_SendRandomCacheReadCmd(U8 bChan, U8 bLun, U32 dRow0,U32 dRow1,U08 bRowCnt,U08 bCacheContinue)
{		
    U08 bChip = bLun / LUN_OF_CHIP;
	U08 bLunOfChip = bLun % LUN_OF_CHIP;
    bChip = CHIPES(bChan, bChip);
		
	U08 bLastCmd;
	
	if(bCacheContinue)
	{
		bLastCmd = FL_CMD_CACHE_REDA;
	}
	else
	{
		bLastCmd = FL_CMD_CACHE_REDA_FINAL;
	}
	
    HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan, bLunOfChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);
	FL_CMD_CQ_SLC_MODE(bChan);

	if(bRowCnt == 1)
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
		HAL_CQSend5Addr(bChan,0,dRow0);
		HAL_CQSendFlashCmd(bChan,bLastCmd);
	}
	else
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
		HAL_CQSend5Addr(bChan,0,dRow0);
		HAL_CQSendFlashCmd(bChan,FL_CMD_CACHE_REDA);
		HAL_CQCheckIOStatus(bChan, bLunOfChip, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);

		FL_CMD_CQ_SLC_MODE(bChan);
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
		HAL_CQSend5Addr(bChan,0,dRow1);
		HAL_CQSendFlashCmd(bChan,bLastCmd);
	}
}


void CB_ReadLogicClstStartDma2(U08 bChan,U08 bLun,U32 dSrcRow,U08 bClstOffs,U32 dUserAddr,U32 dDmaInforAddr,U16 wBuffIndex,U08 bCqFifo,U08 bJointMode)
{
	U08 bChip;
	U16 wColAddr;
	
	dUserAddr -= (bClstOffs*UD_LEN_OF_DATA_CLST);
	U08 bCqFifoFlag = HAL_CQWaitIdleSpace(bChan,bCqFifo);
	
	if(bCqFifoFlag)
	{		
		bChip = bLun / LUN_OF_CHIP;
		wColAddr = bClstOffs * SECT_OF_DATA_CLST / SECT_OF_DMA;
		wColAddr = DATA_COL_ADDR(wColAddr);
		bChip = CHIPES(bChan, bChip);

		HAL_CQChipSelect(bChan,bChip);
		HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);
		HAL_CQSend5RndAddr(bChan, wColAddr,dSrcRow);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
		
		HAL_CQSetRndSeed(bChan,dSrcRow,INVALID_U16);
		HAL_CQSetUd(bChan,dUserAddr,SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_RD_TO_BUFF);
		
		U08 bFlashIdx = bClstOffs* SECT_OF_DATA_CLST;
		HAL_CQStartDMA2(bChan,tFtlTask.dHandleBuff, DMA2_RD_TO_BUFF, wBuffIndex, bFlashIdx, SECT_OF_DATA_CLST, bJointMode, dDmaInforAddr);
	}
}

void CB_ReadTableClstStartDma2(U08 bChan,U08 bLun,U32 dSrcRow,U08 bClstOffs,U32 dUserAddr,U32 dDmaInforAddr,U16 wBuffIndex,U08 bCqFifo,U08 bJointMode)
{
	U08 bChip;
	U16 wColAddr;
	dUserAddr -= (bClstOffs*UD_LEN_OF_TBLE_CLST);
	U08 bCqFifoFlag = HAL_CQWaitIdleSpace(bChan,bCqFifo);

	if(bCqFifoFlag)
	{
		bChip = bLun / LUN_OF_CHIP;
		wColAddr = bClstOffs * SECT_OF_TBLE_CLST / TSECT_OF_DMA;
		wColAddr = TBLE_COL_ADDR(wColAddr);
		
		bChip = CHIPES(bChan, bChip);
		HAL_CQChipSelect(bChan,bChip);
		HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_ARRAY, FIO_PF_PASS_NULL);

		HAL_CQSend5RndAddr(bChan, wColAddr,dSrcRow);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
		HAL_CQSetRndSeed(bChan,dSrcRow,INVALID_U16);
		HAL_CQSetUd(bChan,dUserAddr,SECT_OF_TBLE_CLST, UD_LEN_OF_TBLE_CLST,DMA2_RD_TO_BUFF);
		
		U08 bFlashIdx = bClstOffs* SECT_OF_TBLE_CLST;
		HAL_CQStartDMA2(bChan,tFtlTask.dHandleBuff, DMA2_RD_TO_BUFF, wBuffIndex, bFlashIdx, SECT_OF_TBLE_CLST, bJointMode, dDmaInforAddr);
	}
}

void CB_AllocateTableClstToLun(T_FLASH_PHY_ADDR *pSrcClst,U08 bRdClstCnt)
{
	U08 i,j,k;
	U08 bChan;
	U08 bLunOfChan;

	MemorySet((U8 *)(&tChTaskQueue[0].wLastOfstInTask), 0xFF, sizeof(T_TCB_LUNINFOR));
	pT_TCB_LUNINFOR pTCB_Infor = (pT_TCB_LUNINFOR)(&tChTaskQueue[0].wLastOfstInTask);

	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		for(j = 0;j<LUN_OF_CHAN;j++)
		{
			pTCB_Infor->m_bLunRowCnt[i][j] = 0;
		}
	}

	U08 bRowIndex = 0;
	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		for(j = 0;j<LUN_OF_CHAN;j++)
		{
			for(k = 0;k<bRdClstCnt;k++)
			{
				bChan = pSrcClst[k].m_bChan;
				bLunOfChan = pSrcClst[k].m_bLunOfChan;
				
				if((i == bChan)&&(j == bLunOfChan))
				{
					pTCB_Infor->m_bLunRowCnt[i][j]++;

					if(pTCB_Infor->m_bLunRowIndex[i][j] == 0xFF)
					{
						pTCB_Infor->m_bLunRowIndex[i][j] = bRowIndex;
					}
					pTCB_Infor->m_dRow[bRowIndex] = pSrcClst[k].m_dRowAddr;
					pTCB_Infor->m_bClstOff[bRowIndex] = pSrcClst[k].m_bClstOffs;
					pTCB_Infor->m_bClstBuffIndex[bRowIndex] = k;
					bRowIndex++;
				}
			}
		}
	}
}

U08 CB_ExceptionCheckAllChan(void)
{
	U08 i;
	U08 bChan;
	U08 bFailChan = 0;
	
	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		bChan = CHANS(i);
		
		if((SG_DMA2_CH(bChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1)||(SG_DMA2_CH(bChan)->rSTATUS.bits.CRC_FAIL_FLAG == 1))
		{
			Fdl_ClearExceptionState(bChan);
			bFailChan++;
		}
	}

	return bFailChan;
}

void CB_ExceptionGetRdFailType(U08 bChan,U08 bCe,U16 wClstIndex,U08 bBuffDmaIndex,U32 dRowAddr,U32 dUdAddr,U08 bDmaOffInPg,U16 wDmaInfor,U08 bClstOff)
{
	U08 bFailCnt = 0;

	if(DMAINFO_ABNORMAL(wDmaInfor))
	{
		if(DMAINFO_ECCFAIL(wDmaInfor))
		{
			//LOG_PRINT("Rty:Chan %x,Ce %x,RowAddr %x,UdAddr %x,DmaOff %x,DmaIdx %x,ClstIdx %x \r\n",bChan,bCe,dRowAddr,dUdAddr,bDmaOffInPg,bBuffDmaIndex,wClstIndex);
			bFailCnt = tRetryInfor.m_dFailDmaTotal;
			tRetryInfor.m_tDmaInfor[bFailCnt].m_dDmaRow = dRowAddr;
			tRetryInfor.m_tDmaInfor[bFailCnt].m_dUserBaseAddr = dUdAddr;
			tRetryInfor.m_tDmaInfor[bFailCnt].m_bDmaOffsInPage = bDmaOffInPg;
			tRetryInfor.m_tDmaInfor[bFailCnt].m_bBuffDmaIndex = bBuffDmaIndex;
			tRetryInfor.m_bCBClstIdx[bFailCnt] = bClstOff;
			
			tRetryInfor.m_bFailDmaCnt[bChan][bCe]++;
			tRetryInfor.m_dFailDmaTotal++;

			tRetryCtrl[bChan][bCe].bRetryLoopCnt = 1;/*Retry Fail重走次数*/

		}
		else if(DMAINFO_ALLONE(wDmaInfor))
		{
			if(pFdlParam.tReadFailCtrl.m_Fail_ReRdCnt == 0)
			{
				tFtlTask.FTLTaskExceptionHandler(wClstIndex,DMA2_RD_TO_BUFF,(0x01<<RD_ERR_ALL_ONE));
			}
			else
			{
				bFailCnt = tRetryInfor.m_dFailDmaTotal;
				tRetryInfor.m_tDmaInfor[bFailCnt].m_dDmaRow = dRowAddr;
				tRetryInfor.m_tDmaInfor[bFailCnt].m_dUserBaseAddr = dUdAddr;
				tRetryInfor.m_tDmaInfor[bFailCnt].m_bDmaOffsInPage = bDmaOffInPg;
				tRetryInfor.m_tDmaInfor[bFailCnt].m_bBuffDmaIndex = bBuffDmaIndex;
				tRetryInfor.m_bCBClstIdx[bFailCnt] = bClstOff;
				
				tRetryInfor.m_bFailDmaCnt[bChan][bCe]++;
				tRetryInfor.m_dFailDmaTotal++;
			}
		}
		else if(DMAINFO_ALLZERO(wDmaInfor))
		{
			tFtlTask.FTLTaskExceptionHandler(wClstIndex,DMA2_RD_TO_BUFF,(0x01<<RD_ERR_ALL_ZERO));
		}

	}
}

U08 CB_GetSingleRetryDmaInfor(U8 bInforIndex)
{
	U08 bErrType = 0;
	
	if(tRetryInfor.m_tDmaInfor[bInforIndex].m_bBuffDmaIndex >= 0xF0)  
	{
		if(tRetryInfor.m_tDmaInfor[bInforIndex].m_bBuffDmaIndex - 0xF0 < 0x8)						
		{
			bErrType = (0x01<<(tRetryInfor.m_tDmaInfor[bInforIndex].m_bBuffDmaIndex - 0xF0));
		}
		else if(tRetryInfor.m_tDmaInfor[bInforIndex].m_bBuffDmaIndex == 0xFF)
		{
			bErrType = (0x01<<RD_ERR_ALL_ONE);
		}
		else
		{
			while(1);
		}
	}
	else 
	{
		bErrType = (0x01<<RD_ERR_DATA_ERROR);
	}
	return bErrType;
	
}


void CB_ExceptionSaveMultiLunInfor(U08 *bLunPair,U08 *bSrcLunChan,U08 *bLunOfChan, U08 bLunStep,U16 *pwSrcClstIndex)
{
	U08 i,k;
	U08 bChan;
	U08 bLun;
	U08 bChip;
	U08 bRetryCnt;
	U08 bInforIndex;

	U08 bErrType = 0;
	U16 wExptInfor = 0;
	U16 wExptInforTmp = 0;


	for(i = 0;i<bLunStep;i++)
	{
		bChan = bSrcLunChan[i];
		bLun = bLunOfChan[i];
		
		bChip = bLun / LUN_OF_CHIP;
		bChip = CHIPES(bChan, bChip);
		
		bRetryCnt= tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip];
		tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip] = 0;
		bInforIndex = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip];
		
		wExptInfor = 0;
		wExptInforTmp = 0;
		bErrType = 0;
		
		for(k = 0;k< bRetryCnt;k++)
		{
			wExptInfor = tRetryInfor.m_bCBClstIdx[bInforIndex] + pwSrcClstIndex[i];
			wExptInfor |= (bLunPair[i]<<12);

			if(k == 0)
			{
				wExptInforTmp = wExptInfor;
			}
				
			if((wExptInforTmp != wExptInfor))
			{
				tFtlTask.FTLTaskExceptionHandler(wExptInforTmp,DMA2_RD_TO_BUFF,bErrType);
				bErrType = 0;
				wExptInforTmp = wExptInfor;
			}

			bErrType |= CB_GetSingleRetryDmaInfor(bInforIndex);

			if(k == bRetryCnt-1)
			{
				tFtlTask.FTLTaskExceptionHandler(wExptInfor,DMA2_RD_TO_BUFF,bErrType);
			}
						
			bInforIndex++;
		}	
	}
}

void CB_ExceptionSaveSingleLunInfor(U08 bChan,U08 bLunPair,U08 bLunOfChan,U16 wSrcClstIndex)
{
	U08 k;
	U08 bChip;
	U08 bRetryCnt;
	U08 bInforIndex;

	U08 bErrType = 0;
	U16 wExptInfor = 0;
	U16 wExptInforTmp = 0;
		
	bChip = bLunOfChan / LUN_OF_CHIP;
	bChip = CHIPES(bChan, bChip);
		
	bRetryCnt= tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip];
	bInforIndex = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip];
	
	wExptInfor = 0;
	wExptInforTmp = 0;
	bErrType = 0;
		
	for(k = 0;k< bRetryCnt;k++)
	{
		wExptInfor = tRetryInfor.m_bCBClstIdx[bInforIndex]+wSrcClstIndex;
		wExptInfor |= (bLunPair<<12);

		if(k == 0)
		{
			wExptInforTmp = wExptInfor;
		}
			
		if((wExptInforTmp != wExptInfor))
		{
			tFtlTask.FTLTaskExceptionHandler(wExptInforTmp,DMA2_RD_TO_BUFF,bErrType);
			bErrType = 0;
			wExptInforTmp = wExptInfor;
		}
		
		bErrType |= CB_GetSingleRetryDmaInfor(bInforIndex);

		if(k == bRetryCnt-1)
		{
			tFtlTask.FTLTaskExceptionHandler(wExptInfor,DMA2_RD_TO_BUFF,bErrType);
		}
					
		bInforIndex++;
	}
}

void CB_ExceptionSaveTableInfor(U16 wClstBsIdx)
{
	U08 i,j,k;
	U08 bChan;
	U08 bChip;
	U08 bRetryCnt;
	U08 bInforIndex;

	U08 bErrType = 0;
	U16 wExptClstIdx = 0;
	U16 wExptClstIdxTmp = 0;

	for(i =  0;i<CHAN_MAX_CNT;i++)
	{
		for(j = 0;j<LUN_OF_CHAN;j++)
		{
			bChan = CHANS(i);
			bChip = j / LUN_OF_CHIP;
			bChip = CHIPES(bChan, bChip);
			bRetryCnt = tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip];
			bInforIndex = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip];
			
			wExptClstIdx = 0;
			wExptClstIdxTmp = 0;
			bErrType = 0;
			
			for(k = 0;k< bRetryCnt;k++)
			{
				wExptClstIdx = tRetryInfor.m_bCBClstIdx[bInforIndex] + wClstBsIdx;
				
				if(k == 0)
				{
					wExptClstIdxTmp = wExptClstIdx;
				}
					
				if((wExptClstIdxTmp != wExptClstIdx))
				{
					tFtlTask.FTLTaskExceptionHandler(wExptClstIdxTmp,DMA2_RD_TO_BUFF,bErrType);
					bErrType = 0;
					wExptClstIdxTmp = wExptClstIdx;
				}

				bErrType |= CB_GetSingleRetryDmaInfor(bInforIndex);
				
				if(k == bRetryCnt-1)
				{
					tFtlTask.FTLTaskExceptionHandler(wExptClstIdx,DMA2_RD_TO_BUFF,bErrType);
				}
							
				bInforIndex++;
			}
		}
	}
}

U08 CB_HandleReadExCeptionTableSlc(T_FLASH_PHY_ADDR *pSrcClst,U16 wClstBsIdx,U08 bRdClstCnt)
{
	U08 i,j,m,n;
	U08 bBuffClstIdx = 0;

	U08 bDmaOffInPg = 0;

	U32 dRow = 0;
	U32 dMetaAddr;
	U16 wDmaInfor;

	U08 bChan = 0;
	U08 bChip = 0;
	U08 bFailDmaCnt = 0;
	U08 bFailDmaOfset = 0;
	U16 *pwDmaInfor = (U16 *)tRetryInfor.m_dDmaInforAddr;
	pT_TCB_LUNINFOR pTCB_Infor = (pT_TCB_LUNINFOR)(&tChTaskQueue[0].wLastOfstInTask);
		
	U08 bExFlag = CB_ExceptionCheckAllChan();

	if(bExFlag)
	{
		U08 bDmaOfClst = SECT_OF_TBLE_CLST/TSECT_OF_DMA;
		
		for(i = 0;i<CHAN_MAX_CNT;i++)
		{
			for(j = 0;j<LUN_OF_CHAN;j++)
			{
				bChan = CHANS(i);
				bChip = j / LUN_OF_CHIP;
				bChip = CHIPES(bChan, bChip);
				
				for(m = 0;m<pTCB_Infor->m_bLunRowCnt[i][j];m++)
				{
					bBuffClstIdx = pTCB_Infor->m_bClstBuffIndex[pTCB_Infor->m_bLunRowIndex[i][j]+m];
					
								
					for(n = 0;n<bDmaOfClst;n++)
					{
						wDmaInfor = pwDmaInfor[bBuffClstIdx*bDmaOfClst+n];

						if(DMAINFO_ABNORMAL(wDmaInfor))
						{
							if(n == 0)
							{
								dRow = pSrcClst[bBuffClstIdx].m_dRowAddr;
								bDmaOffInPg = pSrcClst[bBuffClstIdx].m_bClstOffs*bDmaOfClst;
							}
							
							if(tFtlTask.dUdBuff == 0)//源为0，不关心源的UserData,可以覆盖写
							{
								dMetaAddr = (U32)(&(tFtlTask.bMetaDataBuff[0]));
							}
							else
							{
								dMetaAddr = tFtlTask.dUdBuff + (wClstBsIdx + bBuffClstIdx)*UD_LEN_OF_TBLE_CLST;
							}
							
							dMetaAddr -= pSrcClst[bBuffClstIdx].m_bClstOffs*UD_LEN_OF_TBLE_CLST;
							CB_ExceptionGetRdFailType(bChan,bChip,wClstBsIdx+bBuffClstIdx,bBuffClstIdx*bDmaOfClst+n,dRow,dMetaAddr,bDmaOffInPg+n,wDmaInfor,bBuffClstIdx);
						}
					}
				}
				
				if(i+j == 0)
				{
					tRetryInfor.m_bCBFailDmaOffst[bChan][bChip] = 0;
				}
				else
				{
					tRetryInfor.m_bCBFailDmaOffst[bChan][bChip] = bFailDmaOfset+bFailDmaCnt;
				}
				
				bFailDmaOfset = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip];
				bFailDmaCnt = tRetryInfor.m_bFailDmaCnt[bChan][bChip];
			}
		}

		U08 bTotelFail = 0;
		for(i = 0;i<CHAN_MAX_CNT;i++)
		{
			U08 bChan = CHANS(i);
			for(j = 0;j<LUN_OF_CHAN;j++)
			{
				bChip = j / LUN_OF_CHIP;
				bChip = CHIPES(bChan, bChip);
				tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip] = tRetryInfor.m_bFailDmaCnt[bChan][bChip];

				bTotelFail += tRetryInfor.m_bFailDmaCnt[bChan][bChip];
			}
		}

		tRetryInfor.m_dFailDmaTotal = bTotelFail;
		FDL_CopyBackRetry();
	}
	return bExFlag;
}

U8 CB_HandleReadExCeptionInDieSlc(U08 bLunCnt,U08 *pPhyChan,U08 *pbLunOfChan,U32 *pdpdRowAddr,U16 *pwClstIndex,U08 *pbRdClstCnt,
											U32 *pbpdClstOff,U32 *pdLunDmaInfor,U32 *pdUserBaseBuff,U16 *wRdBuffIndex)
{	
	U08 i,j,k;
	U08 bChip;
	U08 bChipLast;
	U08 bLunOfChan;
	U08 bDmaOff;
	U16 wClstIndex;
	U32 dMetaAddr;
	
	U32 *pdRowAddr[CB_LUN_CNT];
	U08 *pbClstOff[CB_LUN_CNT];
	U16 *pDmaInfor[CB_LUN_CNT];

	U08 bExFlag = CB_ExceptionCheckAllChan();

	if(bExFlag) 							
	{
		U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;
				
		for(i = 0;i<bLunCnt;i++)
		{
			pdRowAddr[i] = (U32 *)pdpdRowAddr[i];
			pbClstOff[i] = (U08 *)pbpdClstOff[i];
			pDmaInfor[i] = (U16 *)pdLunDmaInfor[i];
		}
		
		MemorySet((U8 *)(&tRetryInfor), 0,RETRY_INFOR_CLEAR_BYTE);
							
		for(i = 0;i<bLunCnt;i++)
		{		
			bLunOfChan = pbLunOfChan[i];
			bChip = bLunOfChan / LUN_OF_CHIP;
			bChip = CHIPES(pPhyChan[i], bChip);
			
			for(j = 0;j<pbRdClstCnt[i];j++)
			{	
				wClstIndex = pwClstIndex[i]+j;
				bDmaOff = pbClstOff[i][wClstIndex]*bDmaOfClst;
				dMetaAddr = pdUserBaseBuff[i]+wClstIndex*UD_LEN_OF_DATA_CLST;
				dMetaAddr -= (pbClstOff[i][wClstIndex])*UD_LEN_OF_DATA_CLST;
				
				for(k = 0;k<bDmaOfClst;k++)
				{

					U32 dRowAddr = pdRowAddr[i][wClstIndex];
					CB_ExceptionGetRdFailType(pPhyChan[i],bChip,wClstIndex,wRdBuffIndex[i]/DSECT_OF_DMA+j*bDmaOfClst+k,dRowAddr,dMetaAddr, bDmaOff+k,pDmaInfor[i][j*bDmaOfClst+k],j);
				}
			}

			if(i == 0)
			{
				tRetryInfor.m_bCBFailDmaOffst[pPhyChan[i]][bChip] = 0;
			}
			else
			{
				bChipLast = pbLunOfChan[i-1]/ LUN_OF_CHIP;
				bChipLast = CHIPES(pPhyChan[i-1], bChipLast);
				tRetryInfor.m_bCBFailDmaOffst[pPhyChan[i]][bChip] = tRetryInfor.m_bCBFailDmaOffst[pPhyChan[i-1]][bChipLast]+tRetryInfor.m_bFailDmaCnt[pPhyChan[i-1]][bChipLast];
			}
		}
		
		U08 bTotelFail = 0;			
		for(i = 0;i<CHAN_MAX_CNT;i++)
		{
			U08 bChan = CHANS(i);
			for(j = 0;j<LUN_OF_CHAN;j++)
			{
				bChip = j / LUN_OF_CHIP;
				bChip = CHIPES(bChan, bChip);
				
				bTotelFail += tRetryInfor.m_bFailDmaCnt[bChan][bChip];
				tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip]=tRetryInfor.m_bFailDmaCnt[bChan][bChip];
			}
		}
		tRetryInfor.m_dFailDmaTotal = bTotelFail;		
		FDL_CopyBackRetry();
	}
	
	return bExFlag;
}


U8 CB_HandleReadExCeptionInDieTlc(U08 bLunCnt,U08 *pPhyChan,U08 *pbLunOfChan,U32 *pdpdRowAddr,U16 *pwClstIndex,U08 *pbRdClstCnt,
												U32 *pbpdClstOff,U32 *pdLunDmaInfor,U32 *pdpdMetaBaseBuff,U16 *wRdBuffIndex)
{
	U08 i,j,k;
	U08 bChip;
	U08 bChipLast;
	U08 bLunOfChan;
	U08 bDmaOff;
	U16 wClstIndex;
	U32 dMetaAddr;
	
	U32 *pdRowAddr[CB_LUN_CNT];
	U08 *pbClstOff[CB_LUN_CNT];
	U16 *pDmaInfor[CB_LUN_CNT];
	U32 *pdMetaBaseBuff[CB_LUN_CNT];

	U08 bExFlag = CB_ExceptionCheckAllChan();

	if(bExFlag) 							
	{
		U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;
				
		for(i = 0;i<bLunCnt;i++)
		{
			pdRowAddr[i] = (U32 *)pdpdRowAddr[i];
			pbClstOff[i] = (U08 *)pbpdClstOff[i];
			pDmaInfor[i] = (U16 *)pdLunDmaInfor[i];
			pdMetaBaseBuff[i] = (U32 *)pdpdMetaBaseBuff[i];
		}
		
		MemorySet((U8 *)(&tRetryInfor), 0,RETRY_INFOR_CLEAR_BYTE);
		
		for(i = 0;i<bLunCnt;i++)
		{		
			bLunOfChan = pbLunOfChan[i];
			bChip = bLunOfChan / LUN_OF_CHIP;
			bChip = CHIPES(pPhyChan[i], bChip);
			
			for(j = 0;j<pbRdClstCnt[i];j++)
			{	
				wClstIndex = pwClstIndex[i]+j;
				bDmaOff = pbClstOff[i][wClstIndex]*bDmaOfClst;
				dMetaAddr = pdMetaBaseBuff[i][wClstIndex];
				dMetaAddr -= (pbClstOff[i][wClstIndex])*UD_LEN_OF_DATA_CLST;
				
				for(k = 0;k<bDmaOfClst;k++)
				{
					U32 dRowAddr = pdRowAddr[i][wClstIndex];
					CB_ExceptionGetRdFailType(pPhyChan[i],bChip,wClstIndex,wRdBuffIndex[i]/DSECT_OF_DMA+j*bDmaOfClst+k,dRowAddr,dMetaAddr, bDmaOff+k,pDmaInfor[i][j*bDmaOfClst+k],j);
				}
			}

			if(i == 0)
			{
				tRetryInfor.m_bCBFailDmaOffst[pPhyChan[i]][bChip] = 0;
			}
			else
			{
				bChipLast = pbLunOfChan[i-1]/ LUN_OF_CHIP;
				bChipLast = CHIPES(pPhyChan[i-1], bChipLast);
				tRetryInfor.m_bCBFailDmaOffst[pPhyChan[i]][bChip] = tRetryInfor.m_bCBFailDmaOffst[pPhyChan[i-1]][bChipLast]+tRetryInfor.m_bFailDmaCnt[pPhyChan[i-1]][bChipLast];
			}
		}
		
		U08 bTotelFail = 0;
		
		for(i = 0;i<CHAN_MAX_CNT;i++)
		{
			U08 bChan = CHANS(i);
			for(j = 0;j<LUN_OF_CHAN;j++)
			{
				bChip = j / LUN_OF_CHIP;
				bChip = CHIPES(bChan, bChip);
				bTotelFail += tRetryInfor.m_bFailDmaCnt[bChan][bChip];
				tRetryInfor.m_bFailDmaCntBackUp[bChan][bChip]=tRetryInfor.m_bFailDmaCnt[bChan][bChip];
			}
		}
		tRetryInfor.m_dFailDmaTotal = bTotelFail;
		FDL_CopyBackRetry();
	}
	
	return bExFlag;
}
													
U08 CB_HandleReadExCeptionExDieSlc(U08 bSrcChan,U08 bLunOfChan,U08 bClstCnt,U16 wSrcClstIndex,U16 wBuffIndex,U32 *dRow,U8 *bClstOf,U32 UserDataAddr,U32 dDmaInforBase)
{
	U08 j,k;
	U08 bDmaOff;
	U16 wClstIdx;
	U32 dMetaAddr;
	U16 *pDmaInfor = (U16 *)dDmaInforBase;

    U08 bChip = bLunOfChan / LUN_OF_CHIP;
    bChip = CHIPES(bSrcChan, bChip);
		
	if((SG_DMA2_CH(bSrcChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1)||(SG_DMA2_CH(bSrcChan)->rSTATUS.bits.CRC_FAIL_FLAG == 1))
	{
		U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;
		
		Fdl_ClearExceptionState(bSrcChan);
		MemorySet((U8 *)(&tRetryInfor), 0,RETRY_INFOR_CLEAR_BYTE);
						
		for(j = 0;j<bClstCnt;j++)
		{		
			wClstIdx = wSrcClstIndex+j;
			bDmaOff = bClstOf[wClstIdx]*bDmaOfClst;
			dMetaAddr = UserDataAddr+wClstIdx*UD_LEN_OF_DATA_CLST;
			dMetaAddr -= (bClstOf[wClstIdx])*UD_LEN_OF_DATA_CLST;
				
			for(k = 0;k<bDmaOfClst;k++)
			{
				U32 dRowAddr = dRow[wClstIdx];
				CB_ExceptionGetRdFailType(bSrcChan,bChip,wClstIdx,wBuffIndex/DSECT_OF_DMA+j*bDmaOfClst+k,dRowAddr,dMetaAddr, bDmaOff+k,pDmaInfor[j*bDmaOfClst+k],j);
			}
		}

		tRetryInfor.m_dFailDmaTotal = tRetryInfor.m_bFailDmaCnt[bSrcChan][bChip];
		tRetryInfor.m_bFailDmaCntBackUp[bSrcChan][bChip]=tRetryInfor.m_bFailDmaCnt[bSrcChan][bChip];
		
		FDL_CopyBackRetry();
		
		return 1;
	}
	return 0;
}
											
U08 CB_HandleReadExCeptionExDieTlc(U08 bSrcChan,U08 bLunOfChan,U08 bClstCnt,U16 wSrcClstIndex,U16 wBuffIndex,U32 *dRow,U8 *bClstOf,U32 *UserDataAddr,U32 dDmaInforBase)
{
	U08 j,k;
	U08 bDmaOff;
	U32 dMetaAddr;
	U16 wClstIdx;
	U16 *pDmaInfor = (U16 *)dDmaInforBase;

    U08 bChip = bLunOfChan / LUN_OF_CHIP;
    bChip = CHIPES(bSrcChan, bChip);
		
	if((SG_DMA2_CH(bSrcChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1)||(SG_DMA2_CH(bSrcChan)->rSTATUS.bits.CRC_FAIL_FLAG == 1))
	{
		U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;
		
		Fdl_ClearExceptionState(bSrcChan);
		
		MemorySet((U8 *)(&tRetryInfor), 0,RETRY_INFOR_CLEAR_BYTE);
						
		for(j = 0;j<bClstCnt;j++)
		{	
			wClstIdx = wSrcClstIndex+j;
			bDmaOff = bClstOf[wClstIdx]*bDmaOfClst;
			dMetaAddr = UserDataAddr[wClstIdx];
			dMetaAddr -= (bClstOf[wClstIdx])*UD_LEN_OF_DATA_CLST;
				
			for(k = 0;k<bDmaOfClst;k++)
			{
               U32 dRowAddr = dRow[wClstIdx];
				CB_ExceptionGetRdFailType(bSrcChan,bChip,wClstIdx,wBuffIndex/DSECT_OF_DMA+j*bDmaOfClst+k,dRowAddr,dMetaAddr, bDmaOff+k,pDmaInfor[j*bDmaOfClst+k],j);
			}
		}

		tRetryInfor.m_dFailDmaTotal = tRetryInfor.m_bFailDmaCnt[bSrcChan][bChip];
		tRetryInfor.m_bFailDmaCntBackUp[bSrcChan][bChip]=tRetryInfor.m_bFailDmaCnt[bSrcChan][bChip];
		
		FDL_CopyBackRetry();
		
		return 1;
	}
	return 0;
}

U08 CB_HandleReadExCeptionInnerTlc(U08 bSrcChan,U08 bLunOfChan,U16 wSrcPgIndex,U32 *dRow,U32 *pUserDataAddr,U08 *bDmaOutInfor,U32 dDmaInforBase)
{
	U08 i,j;
	U08 bDmaValidInfor;
		
	U08 bEccIndex = 0;
	U16 *pDmaInfor = (U16 *)dDmaInforBase;

    U08 bChip = bLunOfChan / LUN_OF_CHIP;
    bChip = CHIPES(bSrcChan, bChip);
		
	if(SG_DMA2_CH(bSrcChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1) 							
	{
		U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;
								
		for(i = 0;i<PLANE_OF_LUN;i++)
		{	
			bDmaValidInfor = bDmaOutInfor[i];
			for(j = 0;j<DATA_CLST_OF_PAGE;j++)
			{
				if((bDmaValidInfor>>j)&1)
				{
                   U32 dRowAddr = dRow[i];
					CB_ExceptionGetRdFailType(bSrcChan,bChip,wSrcPgIndex+i,0,dRowAddr,pUserDataAddr[i],j*bDmaOfClst+1,pDmaInfor[bEccIndex],i);
					bEccIndex++;
				}
			}
		}		
		return 1;
	}
	return 0;
}

U8 CB_SrcClstLostHandle(U08 bDataType,U08 JointMode,U16 wSecIndex,U32 dUserBuff, U8 bLostClst)//处理不完整页
{
	U08 i,j;
	U08 bUserLen;
	U08 bSecOfDma;
	U08 bSecOfClst;
	U08 bDmaOfClst;
	U08 *pbUnValidUser;
	
	if(bDataType == TYPE_LOGIC)
	{
		bUserLen = UD_LEN_OF_DATA_CLST;
		pbUnValidUser = (U8 *)(&(pFdlParam.bUnValidDataUser[0]));
		bSecOfDma = DSECT_OF_DMA;
		bSecOfClst = SECT_OF_DATA_CLST;
	}
	else
	{
		bUserLen = UD_LEN_OF_TBLE_CLST;
		pbUnValidUser = (U8 *)(&(pFdlParam.bUnValidTableUser[0]));
		bSecOfDma = TSECT_OF_DMA;
		bSecOfClst = SECT_OF_TBLE_CLST;
	}
	bDmaOfClst = bSecOfClst/bSecOfDma;

	for(i = 0;i<bLostClst;i++)
	{
		for(j = 0;j < bUserLen;j++)
		{
			*((U8 *)(dUserBuff+i*bUserLen+j)) =  pbUnValidUser[j];
		}
	}

	if(JointMode != 0)
	{
		for(i = 0;i < bLostClst*bDmaOfClst;i++)
		{
			HAL_BFCSetBuffStatus(wSecIndex+i*bSecOfDma,(((U32)0x1<<bSecOfDma)-1));
		}
	}
	return TRUE;
}

void CB_SrcClstReadTableData(U8 bClstCnt,U16 wClstBaseIndex)
{
	U08 bWaitCqIdleRead  = 0x60;
	pT_TCB_LUNINFOR pTCB_Infor = (pT_TCB_LUNINFOR)(&tChTaskQueue[0].wLastOfstInTask);

	U08 i,j;
	U08 bSrcLunCnt = 0;
	U08 bRdClstCnt[CB_LUN_CNT];
	U08 bRdClstIndex[CB_LUN_CNT];
	U08 bLunToChan[CB_LUN_CNT];
	U08 bLunOfChan[CB_LUN_CNT];

	MemorySet((U8 *)(&bRdClstCnt[0]), 	0, CB_LUN_CNT);
	MemorySet((U8 *)(&bRdClstIndex[0]), 0, CB_LUN_CNT);
	MemorySet((U8 *)(&bLunToChan[0]), 	0, CB_LUN_CNT);
	MemorySet((U8 *)(&bLunOfChan[0]), 	0, CB_LUN_CNT);

	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		for(j = 0;j<LUN_OF_CHAN;j++)
		{
			if(pTCB_Infor->m_bLunRowIndex[i][j] != 0xFF)
			{
				if(bRdClstCnt[bSrcLunCnt] == 0)
				{
					bRdClstCnt[bSrcLunCnt] = pTCB_Infor->m_bLunRowCnt[i][j];
					bRdClstIndex[bSrcLunCnt] = pTCB_Infor->m_bLunRowIndex[i][j];
					bLunToChan[bSrcLunCnt] = i;
					bLunOfChan[bSrcLunCnt] = j;
				}
				bSrcLunCnt++;
			}
		}
	}

	U08 bChanNum,bCqFifoFlag,bClstOffset;
	U32 dDmaInfoBase,dDmaInfoAddr,dUserBuff;
	
	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		HAL_CQWaitDone(CHANS(j));//等上一次各通道CQ Done
	}

	U32 dClstIndex;
	U32 dClstRealIndex;
	U32 dBuffIndex;

	U32 dSrcRow0 = 0;
	U32 dLastRow0 = 0;
	U16 bQueueDma2Index = 0;
	dDmaInfoBase = tFtlTask.dEccStsBuff;

	U32 dLastRowInLun[4][4];
	MemorySet((U8 *)(&(dLastRowInLun[0][0])), 0xFF,64);

	while(bClstCnt)
	{
		for(j = 0;j< bSrcLunCnt; j++)
		{
			if(bRdClstCnt[j] != 0)
			{
				bChanNum = CHANS(bLunToChan[j]);
				dClstIndex = bRdClstIndex[j]+bQueueDma2Index;
				dSrcRow0 = pTCB_Infor->m_dRow[dClstIndex];
				dLastRow0 = dLastRowInLun[bChanNum][bLunOfChan[j]];
				
				if((dSrcRow0>=dLastRow0)&&((dSrcRow0&PLANE_BIT_MASK_OF_ROW) == (dLastRow0&PLANE_BIT_MASK_OF_ROW)))
				{
					
				}
				else
				{
					dLastRowInLun[bChanNum][bLunOfChan[j]] = 0;				
					dLastRowInLun[bChanNum][bLunOfChan[j]] = dSrcRow0;
					bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,0x50);
					
					if(bCqFifoFlag)
					{					
						CB_SendMultiPlnReadCmd(bChanNum, bLunOfChan[j], dSrcRow0,1,1);
					}
				}
			}
		}

		for(j = 0;j< bSrcLunCnt; j++)
		{
			if(bRdClstCnt[j] != 0)
			{
				bChanNum = CHANS(bLunToChan[j]);
				dClstIndex = bRdClstIndex[j]+bQueueDma2Index;
				dClstRealIndex = pTCB_Infor->m_bClstBuffIndex[dClstIndex];
				bClstOffset =  pTCB_Infor->m_bClstOff[dClstIndex];
				dBuffIndex = dClstRealIndex*SECT_OF_TBLE_CLST;
				
				dDmaInfoAddr = dDmaInfoBase+dClstRealIndex*2;
				dSrcRow0 = pTCB_Infor->m_dRow[dClstIndex];
				
				if(tFtlTask.dUdBuff == 0)//源为0，不关心源的UserData,可以覆盖写
				{
					dUserBuff = (U32)(&(tFtlTask.bMetaDataBuff[0]));
				}
				else
				{
					dUserBuff = tFtlTask.dUdBuff + (wClstBaseIndex + dClstRealIndex)*UD_LEN_OF_TBLE_CLST;
				}

				CB_ReadTableClstStartDma2(bChanNum,bLunOfChan[j],dSrcRow0,bClstOffset,
							dUserBuff,dDmaInfoAddr,dBuffIndex,bWaitCqIdleRead,JOINT_NONE);

				bRdClstCnt[j]--;
				bClstCnt--;
			}
		}
		bQueueDma2Index++;
	}

	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		SG_FC(CHANS(j))->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
		HAL_CQWaitDone(CHANS(j));
	}
}

U8 CB_SrcClstReadInDieSlc(U08 bLunStep,U08 *bLunChan,U08 *pbReadClst,U16 *wSrcClstIndex,
                          U32 *dRow,U32 *bClstOf,U32 *UserDataAddr,U08 *bLunOfChan)
{
	U08 j;

	U16 wRdBuffIndex[CB_LUN_CNT];
	U08 bSetLable[CB_LUN_CNT];
	U08 bTotelClst = 0;
	U08 bMultiLunRdQueue[CB_LUN_CNT][16];
	U08 bMultiLunQueueIndex[CB_LUN_CNT];
	U08 bRClstTemp[CB_LUN_CNT];
	U08 bNestClst[CB_LUN_CNT];
	U08 bLunRdHit[CB_LUN_CNT];
	U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;

	U08 bChanNum,bCqFifoFlag,bClstOffset;

	U32 dSrcRowAddr,dDmaInfoBase,dDmaInfoAddr,dUserBuff;

	U32 *dSrcRow[CB_LUN_CNT];
	U08 *bClstOffs[CB_LUN_CNT];
	U32 dLunDmaInfor[CB_LUN_CNT];

	dDmaInfoBase = tFtlTask.dEccStsBuff;
	
	MemorySet((U8 *)(&wRdBuffIndex[0]),0,CB_LUN_CNT*sizeof(U16));
	MemorySet((U8 *)(&bSetLable[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bMultiLunQueueIndex[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bRClstTemp[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bNestClst[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bLunRdHit[0]),0,CB_LUN_CNT*sizeof(U08));

	for(j = 0;j<bLunStep;j++)
	{
		if(pbReadClst[j])//证明此Lun还有任务
		{
			dSrcRow[j] = (U32 *)dRow[j];
			bClstOffs[j] = (U8 *)bClstOf[j];
			wRdBuffIndex[j] = j*PLANE_OF_LUN*DSECT_OF_PAGE;
			bRClstTemp[j] = pbReadClst[j];
			
			dLunDmaInfor[j] = dDmaInfoBase+bTotelClst*bDmaOfClst*2;
			bTotelClst+=pbReadClst[j];
		}
		else
		{
			pbReadClst[j] = 0;
		}
	}

	MemorySet((U08 *)&(bMultiLunRdQueue[0][0]), 0xFF,bLunStep*16);
	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		HAL_CQWaitDone(CHANS(j));//等上一次各通道CQ Done
	}

	U08 bValidClstLable;
	for(j = 0;j<bLunStep;j++)
	{				
		if(bRClstTemp[j] != 0)
		{	
			bChanNum = bLunChan[j];
			bValidClstLable = bMultiLunRdQueue[j][bMultiLunQueueIndex[j]];

			if(bValidClstLable != 0xAA)
			{
				dSrcRowAddr = dSrcRow[j][wSrcClstIndex[j]+bMultiLunQueueIndex[j]];
				
				bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_READ_CQ_FIFO);
				if(bCqFifoFlag)
				{
					CB_SendMultiPlnReadCmd(bChanNum,bLunOfChan[j],dSrcRowAddr,0,1);
				}
			}	
		}
	}

	U16 wPreInfor;
	for(j = 0;j<bLunStep;j++)
	{			
		if(bRClstTemp[j] != 0)
		{
			bChanNum = bLunChan[j];
			wPreInfor = CB_AnalysisLogicClstQueue(dSrcRow[j],(U8 *)(&bMultiLunRdQueue[j][0]),wSrcClstIndex[j],bMultiLunQueueIndex[j],pbReadClst[j]-bMultiLunQueueIndex[j],bSetLable[j]);
			bNestClst[j]  = (wPreInfor>>8);
			bLunRdHit[j] = wPreInfor;
		}
	}

	U08 bQueueIndex;
	U32 dClstIndex;

	while(bTotelClst)
	{
		for(j = 0;j<bLunStep;j++)
		{		
			if(bRClstTemp[j] != 0)
			{
				bChanNum = bLunChan[j];
				
				if(bLunRdHit[j])
				{
					for(bQueueIndex = bMultiLunQueueIndex[j];bQueueIndex<pbReadClst[j];bQueueIndex++)
					{						
						if(bMultiLunRdQueue[j][bQueueIndex] == bSetLable[j])
						{	
							dDmaInfoAddr = dLunDmaInfor[j]+bQueueIndex*bDmaOfClst*2;
							dClstIndex = wSrcClstIndex[j]+bQueueIndex;
							dSrcRowAddr = dSrcRow[j][dClstIndex];
							bClstOffset = bClstOffs[j][dClstIndex];
														
							dUserBuff = UserDataAddr[j]+ dClstIndex*UD_LEN_OF_DATA_CLST;
							
							CB_ReadLogicClstStartDma2(bChanNum,bLunOfChan[j],dSrcRowAddr,bClstOffset,dUserBuff,dDmaInfoAddr,wRdBuffIndex[j]+bQueueIndex*SECT_OF_DATA_CLST,CB_READ_CQ_FIFO,JOINT_NONE);
							bLunRdHit[j]--;

							bTotelClst--;
							bRClstTemp[j]--;
							bMultiLunQueueIndex[j] = bQueueIndex;
							bMultiLunQueueIndex[j]++;
							break;
						}
						bMultiLunQueueIndex[j]++;
					}
				}
				else
				{										
					bMultiLunQueueIndex[j] = bNestClst[j];
					if(bRClstTemp[j] != 0)
					{	
						dSrcRowAddr = dSrcRow[j][wSrcClstIndex[j]+bMultiLunQueueIndex[j]];
						
						bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_READ_CQ_FIFO);
						if(bCqFifoFlag)
						{
							CB_SendMultiPlnReadCmd(bChanNum,bLunOfChan[j],dSrcRowAddr,0,1);
						}
						bSetLable[j]++;
						wPreInfor = CB_AnalysisLogicClstQueue(dSrcRow[j],(U8 *)(&bMultiLunRdQueue[j][0]),wSrcClstIndex[j],bMultiLunQueueIndex[j],pbReadClst[j]-bMultiLunQueueIndex[j],bSetLable[j]);
						bNestClst[j]  = (wPreInfor>>8);
						bLunRdHit[j] = wPreInfor;
					}
				}				
			}
		}	
	}
	
	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		SG_FC(CHANS(j))->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
		HAL_CQWaitDone(CHANS(j));//等上一次各通道CQ Done
	}
	
	U08 bExptFlag = CB_HandleReadExCeptionInDieSlc(bLunStep,&bLunChan[0],&bLunOfChan[0],dRow,wSrcClstIndex,pbReadClst,
									bClstOf,&dLunDmaInfor[0],&UserDataAddr[0],wRdBuffIndex);
	return bExptFlag;
	
}

U8 CB_SrcClstReadInDieTlc(U08 bLunStep,U08 *bLunChan,U08 *pbReadClst,U16 *wSrcClstIndex,U32 *dRow,
							U32 *bClstOf,U32 *UserDataAddr,U08 *bLunOfChan)
{
	U16 wRdBuffIndex[CB_LUN_CNT];
	U08 bSetLable[CB_LUN_CNT];
	U08 bTotelClst = 0;
	U08 bMultiLunRdQueue[CB_LUN_CNT][ORDER_PAGE_MAX*16];
	U08 bMultiLunQueueIndex[CB_LUN_CNT];
	U08 bRClstTemp[CB_LUN_CNT];
	U08 bNestClst[CB_LUN_CNT];
	U08 bLunRdHit[CB_LUN_CNT];
	U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;

	U08 i,j;
	U08 bChanNum,bCqFifoFlag,bClstOffset;

	U32 dSrcRowAddr,dDmaInfoBase,dDmaInfoAddr,dUserBuff;

	U32 *dSrcRow[CB_LUN_CNT];
	U08 *bClstOffs[CB_LUN_CNT];
	U32 *dMetaAddr[CB_LUN_CNT];
	U32 dLunDmaInfor[CB_LUN_CNT];
	
	MemorySet((U8 *)(&wRdBuffIndex[0]),0,CB_LUN_CNT*sizeof(U16));
	MemorySet((U8 *)(&bSetLable[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bMultiLunQueueIndex[0]),0xFF,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bRClstTemp[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bNestClst[0]),0,CB_LUN_CNT*sizeof(U08));
	MemorySet((U8 *)(&bLunRdHit[0]),0,CB_LUN_CNT*sizeof(U08));
	
	dDmaInfoBase = tFtlTask.dEccStsBuff;

	U16 wLunBuffSize = PLANE_OF_LUN*SECT_OF_PAGE;
	if(tCB_ProcessCtrl.bDataType != 0)
	{
		 wLunBuffSize *= ORDER_PAGE_MAX;
	}

	for(j = 0;j<bLunStep;j++)
	{
		if(pbReadClst[j])//证明此Lun还有任务
		{
			dSrcRow[j] = (U32 *)dRow[j];
			bClstOffs[j] = (U8 *)bClstOf[j];
			dMetaAddr[j] = (U32 *)UserDataAddr[j];
			wRdBuffIndex[j] = j*wLunBuffSize;
			bRClstTemp[j] = pbReadClst[j];
			
			dLunDmaInfor[j] = dDmaInfoBase+bTotelClst*bDmaOfClst*2;
			bTotelClst+=pbReadClst[j];
		}
		else
		{
			pbReadClst[j] = 0;
		}
	}
	
	MemorySet((U08 *)dDmaInfoBase,0,bTotelClst*bDmaOfClst*2);// B0KB支持ARC CopyBack的时候 存在虚拟簇和有效簇随机交叉的情况
	MemorySet((U08 *)&(bMultiLunRdQueue[0][0]), 0xFF,bLunStep*ORDER_PAGE_MAX*16);

	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		HAL_CQWaitDone(CHANS(j));//等上一次各通道CQ Done
	}
		
	U16 wBuffIndex;
	for(j = 0;j<bLunStep;j++)
	{
		for(i = 0;i<pbReadClst[j];i++)
		{
			if(dSrcRow[j][wSrcClstIndex[j]+i] == 0xFFFFFFFF)
			{
				bMultiLunRdQueue[j][i] = 0xAA;//无效簇
				bRClstTemp[j]--;
				bTotelClst--;
				bClstOffset = bClstOffs[j][wSrcClstIndex[j]+i];
				U32 dInitData = (bClstOffset|(bClstOffset<<8)|(bClstOffset<<16)|(bClstOffset<<24));
				dUserBuff = dMetaAddr[j][wSrcClstIndex[j]+i];
				wBuffIndex = wRdBuffIndex[j]+i*SECT_OF_DATA_CLST;

				HAL_PMCPMemorySet(0, tFtlTask.dHandleBuff+wBuffIndex*SECTOR_SIZE, dInitData,0x1000);
				CB_SrcClstLostHandle(TYPE_LOGIC,JOINT_NONE,wBuffIndex,dUserBuff, 1);
			}
			else if(bMultiLunQueueIndex[j] == 0xFF)
			{
				bMultiLunQueueIndex[j] = i;
			}
		}
	}

	U08 bValidClstLable;
	for(j = 0;j<bLunStep;j++)
	{				
		if(bRClstTemp[j] != 0)
		{	
			bChanNum = bLunChan[j];
			bValidClstLable = bMultiLunRdQueue[j][bMultiLunQueueIndex[j]];

			if(bValidClstLable != 0xAA)
			{
				dSrcRowAddr = dSrcRow[j][wSrcClstIndex[j]+bMultiLunQueueIndex[j]];
				
				bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_READ_CQ_FIFO);
				if(bCqFifoFlag)
				{
					CB_SendMultiPlnReadCmd(bChanNum,bLunOfChan[j],dSrcRowAddr,0,1);
				}
			}	
		}
	}
	
	//FDL_Log("Clst1 %x",pbReadClst[0]);

	U16 wPreInfor;
	for(j = 0;j<bLunStep;j++)
	{			
		if(bRClstTemp[j] != 0)
		{
			bChanNum = bLunChan[j];
			wPreInfor = CB_AnalysisLogicClstQueue(dSrcRow[j],(U8 *)(&bMultiLunRdQueue[j][0]),wSrcClstIndex[j],bMultiLunQueueIndex[j],pbReadClst[j]-bMultiLunQueueIndex[j],bSetLable[j]);
			bNestClst[j]  = (wPreInfor>>8);
			bLunRdHit[j] = wPreInfor;
		}
	}

	U08 bQueueIndex;
	U32 dClstIndex;

	while(bTotelClst)
	{
		for(j = 0;j<bLunStep;j++)
		{		
			if(bRClstTemp[j] != 0)
			{
				bChanNum = bLunChan[j];
				if(bLunRdHit[j])
				{
					for(bQueueIndex = bMultiLunQueueIndex[j];bQueueIndex<pbReadClst[j];bQueueIndex++)
					{						
						if(bMultiLunRdQueue[j][bQueueIndex] == bSetLable[j])
						{	
							dDmaInfoAddr = dLunDmaInfor[j]+bQueueIndex*bDmaOfClst*2;
							dClstIndex = wSrcClstIndex[j]+bQueueIndex;
							dSrcRowAddr = dSrcRow[j][dClstIndex];
							bClstOffset = bClstOffs[j][dClstIndex];
														
							dUserBuff = dMetaAddr[j][dClstIndex];
							CB_ReadLogicClstStartDma2(bChanNum,bLunOfChan[j],dSrcRowAddr,bClstOffset,dUserBuff,dDmaInfoAddr,wRdBuffIndex[j]+bQueueIndex*SECT_OF_DATA_CLST,CB_READ_CQ_FIFO,JOINT_NONE);
							bLunRdHit[j]--;

							bTotelClst--;
							bRClstTemp[j]--;
							bMultiLunQueueIndex[j] = bQueueIndex;
							bMultiLunQueueIndex[j]++;
							break;
						}
						bMultiLunQueueIndex[j]++;
					}
				}
				else
				{										
					bMultiLunQueueIndex[j] = bNestClst[j];
					if(bRClstTemp[j] != 0)
					{	
						dSrcRowAddr = dSrcRow[j][wSrcClstIndex[j]+bMultiLunQueueIndex[j]];
						
						bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_READ_CQ_FIFO);
						if(bCqFifoFlag)
						{
							CB_SendMultiPlnReadCmd(bChanNum,bLunOfChan[j],dSrcRowAddr,0,1);
						}
						bSetLable[j]++;
						wPreInfor = CB_AnalysisLogicClstQueue(dSrcRow[j],(U8 *)(&bMultiLunRdQueue[j][0]),wSrcClstIndex[j],bMultiLunQueueIndex[j],pbReadClst[j]-bMultiLunQueueIndex[j],bSetLable[j]);
						bNestClst[j]  = (wPreInfor>>8);
						bLunRdHit[j] = wPreInfor;
					}
					
				}				
			}
		}	
	}

	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		SG_FC(CHANS(j))->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
		HAL_CQWaitDone(CHANS(j));//等上一次各通道CQ Done
	}
	
	U08 bExptFlag = CB_HandleReadExCeptionInDieTlc(bLunStep,&bLunChan[0],&bLunOfChan[0],dRow,wSrcClstIndex,pbReadClst,
									bClstOf,&dLunDmaInfor[0],&UserDataAddr[0],wRdBuffIndex);
	return bExptFlag;
}
												
U8 CB_SrcClstReadExDieSlc(U08 bSrcChan,U08 bDesChan,U08 bLunOfChan,U08 bClstCnt,U16 wSrcClstIndex,U16 wBuffIndex,U32 *dRow,U8 *bClstOf,U32 UserDataAddr,U32 dDmaInforBase)
{
	U08 bSetLable = 0;
	U08 bMultiLunRdQueue[16];
	U08 bMultiLunQueueIndex = 0;
	U08 bNestClst = 0;
	U08 bLunRdHit = 0;
	U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;

	U08 bCqFifoFlag;
	U32 dSrcRowAddr,dDmaInfoAddr,dUserBuff;
	
	U08 bRClstTemp = bClstCnt;
	U08 bTotelClst = bClstCnt;
	
	MemorySet((U08 *)&(bMultiLunRdQueue[0]), 0xFF,16);	
	HAL_CQWaitDone(bDesChan);//等上一次各通道CQ Done
	dSrcRowAddr = dRow[wSrcClstIndex+bMultiLunQueueIndex];
	CB_SendMultiPlnReadCmd(bSrcChan,bLunOfChan,dSrcRowAddr,0,1);

	U16 wPreInfor;
	wPreInfor = CB_AnalysisLogicClstQueue(dRow,(U8 *)(&bMultiLunRdQueue[0]),wSrcClstIndex,bMultiLunQueueIndex,bClstCnt-bMultiLunQueueIndex,bSetLable);
	bNestClst  = (wPreInfor>>8);
	bLunRdHit = wPreInfor;

	U08 bQueueIndex;
	U32 dClstIndex;
	
	while(bTotelClst)
	{
		if(bLunRdHit)
		{
			for(bQueueIndex = bMultiLunQueueIndex;bQueueIndex<bClstCnt;bQueueIndex++)
			{						
				if(bMultiLunRdQueue[bQueueIndex] == bSetLable)
				{	
					dDmaInfoAddr = dDmaInforBase+bQueueIndex*bDmaOfClst*2;
					dClstIndex = wSrcClstIndex+bQueueIndex;
					dSrcRowAddr = dRow[dClstIndex];
					U08 bClstOffset = bClstOf[dClstIndex];
												
					dUserBuff = UserDataAddr+ dClstIndex*UD_LEN_OF_DATA_CLST;
					
					CB_ReadLogicClstStartDma2(bSrcChan,bLunOfChan,dSrcRowAddr,bClstOffset,dUserBuff,dDmaInfoAddr,wBuffIndex+bQueueIndex*SECT_OF_DATA_CLST,CB_READ_CQ_FIFO,JOINT_NONE);
					bLunRdHit--;
					bRClstTemp--;

					bTotelClst--;
					bMultiLunQueueIndex = bQueueIndex;
					bMultiLunQueueIndex++;
					break;
				}
				bMultiLunQueueIndex++;
			}
		}
		else
		{										
			bMultiLunQueueIndex = bNestClst;
			if(bRClstTemp != 0)
			{	
				dSrcRowAddr = dRow[wSrcClstIndex+bMultiLunQueueIndex];
				
				bCqFifoFlag = HAL_CQWaitIdleSpace(bSrcChan,CB_READ_CQ_FIFO);
				if(bCqFifoFlag)
				{
					CB_SendMultiPlnReadCmd(bSrcChan,bLunOfChan,dSrcRowAddr,0,1);
				}
				bSetLable++;
				wPreInfor = CB_AnalysisLogicClstQueue(dRow,(U8 *)(&bMultiLunRdQueue[0]),wSrcClstIndex,bMultiLunQueueIndex,bClstCnt-bMultiLunQueueIndex,bSetLable);
				bNestClst  = (wPreInfor>>8);
				bLunRdHit = wPreInfor;
			}
		}				
	}
	
	SG_FC(bSrcChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQWaitDone(bSrcChan);//等上一次各通道CQ Done

	U08 bExptFlag = CB_HandleReadExCeptionExDieSlc(bSrcChan,bLunOfChan,bClstCnt,wSrcClstIndex,wBuffIndex,dRow,bClstOf,UserDataAddr,dDmaInforBase);
	return bExptFlag;
}
												
U8 CB_SrcClstReadExDieTlc(U08 bSrcChan,U08 bDesChan,U08 bLunOfChan,U08 bClstCnt,U16 wSrcClstIndex,U16 wBuffBaseIndex,U32 *dRow,U8 *bClstOf,U32 *dMetaAddr,U32 dDmaInfoBase)
{
	U08 bSetLable = 0;
	U08 bTotelClst = 0;
	U08 bMultiLunRdQueue[16];
	U08 bMultiLunQueueIndex = 0xFF;
	U08 bNestClst = 0;
	U08 bLunRdHit = 0;
	U08 bDmaOfClst = SECT_OF_DATA_CLST/DSECT_OF_DMA;

	U08 i;
	U08 bClstOffset;
	U08 bCqFifoFlag;
	U08 bRClstTemp = bClstCnt;
	bTotelClst = bClstCnt;
	
	U32 dSrcRowAddr,dDmaInfoAddr,dUserBuff;
	
	MemorySet((U08 *)&(bMultiLunRdQueue[0]),0xFF,16);	
	HAL_CQWaitDone(bDesChan);//等上一次各通道CQ Done
	
	U16 wBuffIndex;
	for(i = 0;i<bClstCnt;i++)
	{
		if(dRow[wSrcClstIndex+i] == 0xFFFFFFFF)
		{
			bMultiLunRdQueue[i] = 0xAA;//无效簇
			bRClstTemp--;
			bTotelClst--;
			bClstOffset = bClstOf[wSrcClstIndex+i];
			U32 dInitData = (bClstOffset|(bClstOffset<<8)|(bClstOffset<<16)|(bClstOffset<<24));
			dUserBuff = dMetaAddr[wSrcClstIndex+i];
			wBuffIndex = wBuffBaseIndex+i*SECT_OF_DATA_CLST;

			HAL_PMCPMemorySet(0, tFtlTask.dHandleBuff+wBuffIndex*SECTOR_SIZE, dInitData,0x1000);
			CB_SrcClstLostHandle(TYPE_LOGIC,0,wBuffIndex,dUserBuff, 1);
		}
		else if(bMultiLunQueueIndex == 0xFF)
		{
			bMultiLunQueueIndex = i;
		}
	}
	
	if(bRClstTemp == 0)
	{
		return 0;
	}
	else
	{
		bTotelClst = bRClstTemp;
		bClstCnt = bRClstTemp;
	}
	
	dSrcRowAddr = dRow[wSrcClstIndex+bMultiLunQueueIndex];
	CB_SendMultiPlnReadCmd(bSrcChan,bLunOfChan,dSrcRowAddr,0,1);

	U16 wPreInfor;
	wPreInfor = CB_AnalysisLogicClstQueue(dRow,(U8 *)(&bMultiLunRdQueue[0]),wSrcClstIndex,bMultiLunQueueIndex,bClstCnt-bMultiLunQueueIndex,bSetLable);
	bNestClst  = (wPreInfor>>8);
	bLunRdHit = wPreInfor;

	U08 bQueueIndex;
	U32 dClstIndex;

	while(bTotelClst)
	{
		if(bLunRdHit)
		{
			for(bQueueIndex = bMultiLunQueueIndex;bQueueIndex<bClstCnt;bQueueIndex++)
			{						
				if(bMultiLunRdQueue[bQueueIndex] == bSetLable)
				{	
					dDmaInfoAddr = dDmaInfoBase+bQueueIndex*bDmaOfClst*2;
					dClstIndex = wSrcClstIndex+bQueueIndex;
					dSrcRowAddr = dRow[dClstIndex];
					U08 bClstOffset = bClstOf[dClstIndex];
												
					dUserBuff = dMetaAddr[wSrcClstIndex+bQueueIndex];
					CB_ReadLogicClstStartDma2(bSrcChan,bLunOfChan,dSrcRowAddr,bClstOffset,dUserBuff,dDmaInfoAddr,wBuffBaseIndex+bQueueIndex*SECT_OF_DATA_CLST,CB_READ_CQ_FIFO,JOINT_NONE);
					
					bLunRdHit--;
					bRClstTemp--;

					bTotelClst--;
					bMultiLunQueueIndex = bQueueIndex;
					bMultiLunQueueIndex++;
					break;
				}
				bMultiLunQueueIndex++;
			}
		}
		else
		{										
			bMultiLunQueueIndex = bNestClst;
			if(bRClstTemp != 0)
			{	
				dSrcRowAddr = dRow[wSrcClstIndex+bMultiLunQueueIndex];
				
				bCqFifoFlag = HAL_CQWaitIdleSpace(bSrcChan,CB_READ_CQ_FIFO);
				if(bCqFifoFlag)
				{
					CB_SendMultiPlnReadCmd(bSrcChan,bLunOfChan,dSrcRowAddr,0,1);
				}
				bSetLable++;
				wPreInfor = CB_AnalysisLogicClstQueue(dRow,(U8 *)(&bMultiLunRdQueue[0]),wSrcClstIndex,bMultiLunQueueIndex,bClstCnt-bMultiLunQueueIndex,bSetLable);
				bNestClst  = (wPreInfor>>8);
				bLunRdHit = wPreInfor;
			}
		}				
	}
	
	SG_FC(bSrcChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQWaitDone(bSrcChan);//等上一次各通道CQ Done
	
	U08 bExptFlag = CB_HandleReadExCeptionExDieTlc(bSrcChan,bLunOfChan,bClstCnt,wSrcClstIndex,wBuffBaseIndex,dRow,bClstOf,dMetaAddr,dDmaInfoBase);
	return bExptFlag;	
}											

U8 TaskAllocateMultiLunXXdieTlc(U16 bLunPairNum)
{
	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo = &(((T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr)[bLunPairNum]);
	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;

	U8 bChanPhy = CHANS(pDstInfo->bChan);
	
	if(tChTaskQueue[bChanPhy].wTaskCnt >= MAX_CHAN_TASK)
	{
		return FALSE;
	}
		
	U16 wTailInLun = pCB_LunQueue->wTaskTailInLun[bChanPhy][bLunPairNum];
	if(wTailInLun < pDstInfo->wOrderUnitCnt)
	{
		tChTaskQueue[bChanPhy].wTaskCnt++;
		pCB_LunQueue->wTaskTailInLun[bChanPhy][bLunPairNum]++;
	}

	return TRUE;
}

U8 TaskAllocateMultiLunXXdieSlc( U16 wLunPair)
{
	T_DST_INFO_OF_LUN *pDstInfo = &(((T_DST_INFO_OF_LUN *)tFtlTask.dDstArgsAddr)[wLunPair]);
	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;
	
	U08 bLunTaskCnt = tCB_ProcessCtrl.bLunTaskCnt[wLunPair];
	
	U08 bPhyChan = CHANS(pDstInfo->bChan);
		
	if(tChTaskQueue[bPhyChan].wTaskCnt<MAX_CHAN_TASK)
	{
		U16 wTailInLun= pCB_LunQueue->wTaskTailInLun[bPhyChan][wLunPair];
				
		if(wTailInLun < bLunTaskCnt)
		{
			tChTaskQueue[bPhyChan].wTaskCnt++;
			pCB_LunQueue->wTaskTailInLun[bPhyChan][wLunPair]++;
		}
	}
	
	return TRUE;
}

U8 TaskConvertTableDataCopyBackProg(U8 bvoid)
{
	U08 i;
	U08 bChan;
	U08 bLunOfChan;	
	U08 bCqFifoFlag;
	
	U08 bPgWindow = 0;
	U08 bClstWindow = 0;
	U08 bUnValidClst = 0;
	U08 bWtPlnIndex = 0;
	U08 bWtPlnMode[CB_LUN_CNT];
	
	U16 wRdClstIndex = tCB_TableProcessCtrl.wRdIndex;
	U16 wWtPgIndex = tCB_TableProcessCtrl.wWtIndex;
	U08 bBuffPgWindow = tCB_TableProcessCtrl.bBuffPgWindow;

	T_FLASH_PHY_ADDR *pSrcClst = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wRdClstIndex]);
	T_FLASH_PHY_ADDR *pDesPg = &(((T_FLASH_PHY_ADDR *)tFtlTask.dDstArgsAddr)[wWtPgIndex]);
	MemorySet((U8 *)(&bWtPlnMode[0]),0,CB_LUN_CNT);
	
	while(bBuffPgWindow)
	{
		if(tFtlTask.wCur == 0)
		{
			bPgWindow += tCB_TableProcessCtrl.bHeadPln;
			bWtPlnMode[bWtPlnIndex] = tCB_TableProcessCtrl.bHeadPln;
			tFtlTask.wCur++;

			if(tFtlTask.wCur == tFtlTask.wLen)
			{
				bUnValidClst = tCB_TableProcessCtrl.bLostClst;
			}
			break;
		}
		else if(tFtlTask.wCur == tFtlTask.wLen-1)
		{
			bPgWindow += tCB_TableProcessCtrl.bLastPln;
			bUnValidClst = tCB_TableProcessCtrl.bLostClst;
			bWtPlnMode[bWtPlnIndex] = tCB_TableProcessCtrl.bLastPln;
			tFtlTask.wCur++;
			break;
		}
		else
		{
			bPgWindow += PLANE_OF_LUN;
			bWtPlnMode[bWtPlnIndex] = PLANE_OF_LUN;
			tFtlTask.wCur++;
		}

		if(bPgWindow >= (PLANE_OF_LUN*CHAN_MAX_CNT*CHIP_OF_CHAN))
		{
			break;
		}
		
		bBuffPgWindow -= PLANE_OF_LUN;
		bWtPlnIndex++;
	}
	
	bClstWindow = bPgWindow*TBLE_CLST_OF_PAGE - bUnValidClst;

	bWtPlnIndex = 0;
	U16 wBuffIndex = 0;
	U08 bOpPgIndex = 0;
	U32 dDesRow;
	U08 bPgWindowTemp = bPgWindow;

#if 1//def __TZ_CB_WR_FAIL__
	while(bPgWindowTemp)
	{
		if(bWtPlnMode[bWtPlnIndex] != 0)
		{
			bChan = CHANS(pDesPg[bOpPgIndex].m_bChan);
			U08 bChip = pDesPg[bOpPgIndex].m_bLunOfChan / LUN_OF_CHIP;
			bChip = CHIPES(bChan, bChip);
			HAL_CQWaitDone(bChan);//等上一次各通道写 CQ Done
			HAL_CQChipSelect(bChan,bChip);
			HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
			HAL_CQWaitDone(bChan);//Check上一次编程是否Fail

			if(SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA& (FIO_PF_PASS_SLC_WRITE))
			{
				U08 blogchan =bPhyToLogChan[bChan];
				dWrFailAddr = FAIL_ADDR_SET(TYPE_TABLE,SLC_MODE,blogchan,bLunOfChan,0);//((dDesRow&0xFFFFFF)|(((U32)bDesChan)<<24)|(pDstInfo->bLunOfChan<<26));
				pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
			}
			bPgWindowTemp -= bWtPlnMode[bWtPlnIndex];
			bOpPgIndex += bWtPlnMode[bWtPlnIndex];
			bWtPlnIndex ++;
		}
	}
#endif

	if(bClstWindow)
	{
		CB_AllocateTableClstToLun(pSrcClst,bClstWindow);
		CB_SrcClstReadTableData(bClstWindow,wRdClstIndex);

		U08 bExFlag = CB_HandleReadExCeptionTableSlc(pSrcClst,wRdClstIndex,bClstWindow);
		if(bExFlag)
		{
			CB_ExceptionSaveTableInfor(wRdClstIndex);
		}
	}
	
	U32 dUserBuff = tFtlTask.dDestUdBuff+wWtPgIndex*TBLE_CLST_OF_PAGE*UD_LEN_OF_TBLE_CLST;
	
	if(bUnValidClst)
	{
		CB_SrcClstLostHandle(TYPE_TABLE,JOINT_NONE,bClstWindow*SECT_OF_TBLE_CLST,dUserBuff+bClstWindow*UD_LEN_OF_TBLE_CLST,bUnValidClst);
	}

	bWtPlnIndex = 0;
	wBuffIndex = 0;
	bOpPgIndex = 0;
	bPgWindowTemp = bPgWindow;
	while(bPgWindowTemp)
	{
		if(bWtPlnMode[bWtPlnIndex] != 0)
		{
			bChan = CHANS(pDesPg[bOpPgIndex].m_bChan);
			bLunOfChan = pDesPg[bOpPgIndex].m_bLunOfChan;
			dDesRow = pDesPg[bOpPgIndex].m_dRowAddr;
			
			bCqFifoFlag = HAL_CQWaitIdleSpace(bChan,CB_WRT_CQ_FIFO);
			
			if(bCqFifoFlag)
			{
				CB_MultiPlnWriteTablePg(bChan,bLunOfChan, dDesRow,wBuffIndex,tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,bWtPlnMode[bWtPlnIndex]);
			}
			
			bPgWindowTemp -= bWtPlnMode[bWtPlnIndex];
			bOpPgIndex += bWtPlnMode[bWtPlnIndex];
			wBuffIndex += TSECT_OF_PAGE*bWtPlnMode[bWtPlnIndex];
			dUserBuff += UD_LEN_OF_TBLE_CLST*TBLE_CLST_OF_PAGE*bWtPlnMode[bWtPlnIndex];
			
			bWtPlnIndex ++;
		}
	}

	//信息更新
	tCB_TableProcessCtrl.wRdIndex += bClstWindow;
	tCB_TableProcessCtrl.wWtIndex += bPgWindow;
	
	if(tFtlTask.wCur == tFtlTask.wLen)
	{
		for(i = 0;i<CHAN_MAX_CNT;i++)
		{
			bChan = CHANS(i);
			CB_CheckWriteRealDone(bChan,FIO_PF_PASS_SLC_WRITE);
		}
	}
	else
	{
		HAL_CQGenerateInt(CHANS(0), 0, FUNC_DISABLE);//任选泽一个通道进入中断
	}		
	return TRUE;
}

U8 TaskConvertMultiLunExdieSlcCopyBack(U8 bLunPair)
{	
	U08 bCqFifoFlag;
	U08 bReadClst;
	U08 bUnValidClst = 0;
	U08 bPlaneInLun = PLANE_OF_LUN;
	
	U16 wBuffIndex;
	U32 dUserBuff;
	U32 dUserBuffBase;
	
	T_SRC_INFO_OF_LUN *pSrcInfo = ((T_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr)+bLunPair;
	T_DST_INFO_OF_LUN *pDstInfo = ((T_DST_INFO_OF_LUN *)tFtlTask.dDstArgsAddr)+bLunPair;
	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;

	U08 bSrcChan = CHANS(pSrcInfo->bChan);
	U08 bDesChan = CHANS(pDstInfo->bChan);
	U16 wTaskHd = pCB_LunQueue->wTaskHeadInLun[bDesChan][bLunPair];
	U16 wTaskTail = pCB_LunQueue->wTaskTailInLun[bDesChan][bLunPair];

	if(wTaskHd >= wTaskTail)
	{
		return FALSE;
	}
	
	U08 bHeadPage = tCB_ProcessCtrl.bLunPlnMod[bLunPair]&(0xf);
	U32 dDmaInfoBase = tFtlTask.dEccStsBuff+(bLunPair*PLANE_OF_LUN*DATA_CLST_OF_PAGE*SECT_OF_DATA_CLST/TSECT_OF_DMA)*2;
	
	U16 wLunBuffIndex = tCB_ProcessCtrl.wLunBuffIndex[bLunPair];
	U16 wSrcClstIndex = pCB_LunQueue->wSrcIndexInLun[bDesChan][bLunPair];
	U16 wDesPgIndex = pCB_LunQueue->wDesIndexInLun[bDesChan][bLunPair];


	if(wTaskHd == 0)
	{
		bPlaneInLun = bHeadPage;
	}
	
	bReadClst = DATA_CLST_OF_PAGE*bPlaneInLun;
		
	if(wTaskTail - wTaskHd == 1)//Lun内最后一个Task
	{
		bUnValidClst = pDstInfo->wPageCnt*DATA_CLST_OF_PAGE -pSrcInfo->wClstLen;
		if(wTaskHd != 0)
		{
			bPlaneInLun = (tCB_ProcessCtrl.bLunPlnMod[bLunPair]>>4)&(0xf);
			bReadClst = DATA_CLST_OF_PAGE*bPlaneInLun;
		}
		bReadClst -= bUnValidClst;		
	}

	dUserBuffBase = pSrcInfo->dUdBuffAddr+wSrcClstIndex*UD_LEN_OF_DATA_CLST;

#if 1//def __DZ_CB_WR_FAIL__

		HAL_CQWaitDone(bDesChan);//等上一次各通道写 CQ Done

		U08 bChip = pDstInfo->bLunOfChan / LUN_OF_CHIP;
		bChip = CHIPES(bDesChan, bChip);

	    HAL_CQChipSelect(bDesChan,bChip);
		HAL_CQCheckIOStatus(bDesChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
		HAL_CQWaitDone(bDesChan);//Check上一次编程是否Fail

		if(SG_FC(bDesChan)->rRW_PULSE_DATA.bits.RW_DATA & (FIO_PF_PASS_SLC_WRITE))
		{
			//if(wWtFailCnt<8)
			{
				dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,SLC_MODE,pDstInfo->bChan,pDstInfo->bLunOfChan,0);//((dDesRow&0xFFFFFF)|(((U32)bDesChan)<<24)|(pDstInfo->bLunOfChan<<26));
			}
			//dWrFailAddr = bDesChan *
	        pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
			//wWtFailCnt++;
		}
#endif

	U08 bExptFlag = CB_SrcClstReadExDieSlc(bSrcChan,bDesChan,pSrcInfo->bLunOfChan,bReadClst,wSrcClstIndex,wLunBuffIndex,pSrcInfo->pdRowAddr,pSrcInfo->pbClstOffs,pSrcInfo->dUdBuffAddr,dDmaInfoBase);
	if(bExptFlag)
	{
		CB_ExceptionSaveSingleLunInfor(bSrcChan,bLunPair,pSrcInfo->bLunOfChan,wSrcClstIndex);
	}

	if(bUnValidClst)
	{
		HAL_CQWaitDone(bSrcChan);
		wBuffIndex = wLunBuffIndex+bReadClst*SECT_OF_DATA_CLST;
		dUserBuff = dUserBuffBase + bReadClst*UD_LEN_OF_DATA_CLST;
		CB_SrcClstLostHandle(TYPE_LOGIC,JOINT_NONE,wBuffIndex,dUserBuff,bUnValidClst);//处理不完整页
	}
		
	bCqFifoFlag = HAL_CQWaitIdleSpace(bDesChan,CB_WRT_CQ_FIFO);

	if(bCqFifoFlag)
	{
#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
			if(tCB_ProcessCtrl.bWtCmdMode == 1)
			{
#if (NAND_FLASH == SUMSUNG_3DV4 || NAND_FLASH == AFGM)
				CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan, (pDstInfo->pdRowAddr)[wDesPgIndex],0,1,0, wLunBuffIndex, tFtlTask.dHandleBuff, dUserBuffBase, JOINT_NONE,bPlaneInLun,SLC_MODE);
#else
				CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan, (pDstInfo->pdRowAddr)[wDesPgIndex], wLunBuffIndex, tFtlTask.dHandleBuff, dUserBuffBase, JOINT_NONE,bPlaneInLun,SLC_MODE);
#endif
			}
			else
			{
				CB_SinglePlnWriteLogicPgSLC(bDesChan,pDstInfo->bLunOfChan,(pDstInfo->pdRowAddr)[wDesPgIndex], wLunBuffIndex, tFtlTask.dHandleBuff,dUserBuffBase,bPlaneInLun);
			}
#else
#if (NAND_FLASH == SUMSUNG_3DV4 || NAND_FLASH == AFGM)
			CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan, (pDstInfo->pdRowAddr)[wDesPgIndex],0,1,0, wLunBuffIndex, tFtlTask.dHandleBuff, dUserBuffBase, JOINT_NONE,bPlaneInLun,SLC_MODE);
#else
			CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan, (pDstInfo->pdRowAddr)[wDesPgIndex], wLunBuffIndex, tFtlTask.dHandleBuff, dUserBuffBase, JOINT_NONE,bPlaneInLun,SLC_MODE);
#endif
#endif
	}

	//更新Index信息
	pCB_LunQueue->wSrcIndexInLun[bDesChan][bLunPair] += bReadClst;
	pCB_LunQueue->wDesIndexInLun[bDesChan][bLunPair] += bPlaneInLun;
	
	tChTaskQueue[bDesChan].wTaskCnt--;	
	pCB_LunQueue->wTaskHeadInLun[bDesChan][bLunPair]++;

	if(tChTaskQueue[bDesChan].wTaskCnt == 0)
	{
		CB_CheckWriteRealDone(bDesChan,FIO_PF_PASS_SLC_WRITE);
	}

	HAL_CQGenerateInt(bSrcChan, 0, FUNC_DISABLE);
	
	if(bSrcChan != bDesChan)
	{
		HAL_CQGenerateInt(bDesChan, 0, FUNC_DISABLE);
	}

	return TRUE;
}


U8 TaskConvertMultiLunExdieTlcCopyBack(U8 bLunPairNum)
{	
	U32 dUserBuff;
	U08 bCqFifoFlag;
	
	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo = ((T_IN_COPY_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr)+bLunPairNum;
	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo = ((T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr)+bLunPairNum;
	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;
	
	U08 bSrcChan = CHANS(pSrcInfo->bChan);
	U08 bDesChan = CHANS(pDstInfo->bChan);

	U16 wTaskHd = pCB_LunQueue->wTaskHeadInLun[bDesChan][bLunPairNum];
	U16 wTaskTail =  pCB_LunQueue->wTaskTailInLun[bDesChan][bLunPairNum];

	if(wTaskHd >= wTaskTail)
	{
		return FALSE;
	}
	
	T_PROG_ORDER_UNIT_IN_BLK *pOrderAddr = pDstInfo->ptOrderUnit+wTaskHd;
	
	U16 wBuffIndexBs = tCB_ProcessCtrl.wLunBuffIndex[bLunPairNum];

	U08 bDesStep =  pCB_LunQueue->bCurStepInTask[bDesChan][bLunPairNum];
	U16 wSrcClstIndex = pCB_LunQueue->wSrcIndexInLun[bDesChan][bLunPairNum];
	U08 bReadClst = DATA_CLST_OF_PAGE * PLANE_OF_LUN;

	
	HAL_CQWaitDone(bDesChan);//等上一次各通道写 CQ Done
	
	U08 bChip = pDstInfo->bLunOfChan / LUN_OF_CHIP;
	bChip = CHIPES(bDesChan, bChip);
	
	HAL_CQChipSelect(bDesChan,bChip);
	HAL_CQCheckIOStatus(bDesChan, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
	HAL_CQWaitDone(bDesChan);//Check上一次编程是否Fail

	if(SG_FC(bDesChan)->rRW_PULSE_DATA.bits.RW_DATA& (FIO_PF_PASS_TLC_WRITE))
	{
		U32 dDesRow = pOrderAddr->dRowAddr[bDesStep];
		//if(wWtFailCnt<8)
		{
			dWrFailAddr = FAIL_ADDR_SET(TYPE_LOGIC,TLC_MODE,pDstInfo->bChan,pDstInfo->bLunOfChan,dDesRow);//((dDesRow&0xFFFFFF)|(((U32)bDesChan)<<24)|(pDstInfo->bLunOfChan<<26));
		}
        
        pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
		//wWtFailCnt++;
	}

	U32 dDmaInfoBase = tFtlTask.dEccStsBuff+(bLunPairNum*PLANE_OF_LUN*DATA_CLST_OF_PAGE*SECT_OF_DATA_CLST/TSECT_OF_DMA)*2;
	U08 bExptFlag = CB_SrcClstReadExDieTlc(bSrcChan,bDesChan,pSrcInfo->bLunOfChan,bReadClst,wSrcClstIndex,wBuffIndexBs,pSrcInfo->pdRowAddr,pSrcInfo->pbClstOffs,pSrcInfo->pdUdBuffAddr,dDmaInfoBase);
	
	if(bExptFlag)
	{
		CB_ExceptionSaveSingleLunInfor(bSrcChan,bLunPairNum,pSrcInfo->bLunOfChan,wSrcClstIndex);
	}
	
	dUserBuff = pOrderAddr->dUserDataAddr[bDesStep];
	bCqFifoFlag = HAL_CQWaitIdleSpace(bDesChan,CB_WRT_CQ_FIFO);

#ifdef __TLC_AUTO_FILL__
	U32 dRowLast = pOrderAddr->dRowAddr[bDesStep];
#endif

	if(bCqFifoFlag)
	{
		
#if (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)
		CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan,pOrderAddr->dRowAddr[bDesStep],pOrderAddr->wOrderNum,bDesStep+1,pOrderAddr->bOpPageCnt,wBuffIndexBs, tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
#else
		CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan,pOrderAddr->dRowAddr[bDesStep],wBuffIndexBs, tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
#endif
	}

	pCB_LunQueue->bCurStepInTask[bDesChan][bLunPairNum]++;
	pCB_LunQueue->wSrcIndexInLun[bDesChan][bLunPairNum]+=PLANE_OF_LUN*DATA_CLST_OF_PAGE;

	if(pCB_LunQueue->bCurStepInTask[bDesChan][bLunPairNum] == pOrderAddr->bOpPageCnt)
	{
		pCB_LunQueue->wTaskHeadInLun[bDesChan][bLunPairNum]++;
		pCB_LunQueue->bCurStepInTask[bDesChan][bLunPairNum] = 0;
		tChTaskQueue[bDesChan].wTaskCnt--;
	}

	if(tChTaskQueue[bDesChan].wTaskCnt == 0)
	{
		CB_CheckWriteRealDone(bDesChan,FIO_PF_PASS_TLC_WRITE);
	}
	
#ifdef __TLC_AUTO_FILL__
	if((dRowLast&(0xBD7)) == (0xBD7))
	{
		if(tCB_ProcessCtrl.bBlkAutoFill[bLunPairNum] == 0)
		{
			U32 dRowFill[4] = {3033,3037,3041,3045};
			U16 m,n;
			U32 dPhyRow;
			U32 dBaseRow = dRowLast/ROW_GAP_OF_PLANE*ROW_GAP_OF_PLANE;
		
			for(m = 0;m<4;m++)
			{
				for(n = 0;n<3;n++)
				{
					dPhyRow = dBaseRow+dRowFill[m]+n;
					HAL_CQWaitIdleSpace(bDesChan,CQ_MAX_IDLE_SPACE);
					CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan,dPhyRow|0xFF000000,0,tFtlTask.dHandleBuff,tFtlTask.dHandleBuff+0x4000*PLANE_OF_LUN,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
					CB_CheckWriteRealDone(bDesChan,FIO_PF_PASS_TLC_WRITE);
				}
			}
		
			for(m = 0;m<12;m++)
			{
				dPhyRow =  dBaseRow+3049+m*2;
				HAL_CQWaitIdleSpace(bDesChan,CQ_MAX_IDLE_SPACE);
				CB_MultiPlnWriteLogicPg(bDesChan,pDstInfo->bLunOfChan,dPhyRow|0xFF000000,0,tFtlTask.dHandleBuff,tFtlTask.dHandleBuff+0x4000*PLANE_OF_LUN,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
				CB_CheckWriteRealDone(bDesChan,FIO_PF_PASS_TLC_WRITE);
			}
			tCB_ProcessCtrl.bBlkAutoFill[bLunPairNum] = 0xAB;
		}
	}
#endif

	HAL_CQGenerateInt(bSrcChan, 0, FUNC_DISABLE);
	
	if(bSrcChan != bDesChan)
	{
		HAL_CQGenerateInt(bDesChan, 0, FUNC_DISABLE);
	}

	return TRUE;
}


U8 TaskConvertMultiLunIndieSlcCopyBack(U8 bVoid)
{	
	U08 bLunNum;
	U08 bChanNum;
	
	U08 bLunPair[CB_LUN_CNT];
	U08 bLunChan[CB_LUN_CNT];
	U16 wTaskHdInLun[CB_LUN_CNT];
	U16 wTaskTailInLun[CB_LUN_CNT];

	U08 bReadClst[CB_LUN_CNT];
	U08 bPlnMode[CB_LUN_CNT];
	
	U16 wSrcClstIndex[CB_LUN_CNT];
	U16 wDesPgIndex[CB_LUN_CNT];
	U16 wLunLastTask[CB_LUN_CNT];
	U32 *dSrcRowAddr[CB_LUN_CNT];
	U08 *bSrcClstOf[CB_LUN_CNT];
	U32 SrcUserDataAddr[CB_LUN_CNT];
	U08 bSrcLunOfChan[CB_LUN_CNT];
	
	U32 dUserBuff;
	
	U32 dDesRow;
	U08 bCqFifoFlag;
		
	T_SRC_INFO_OF_LUN *pSrcInfo = ((T_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr);
	T_DST_INFO_OF_LUN *pDstInfo = ((T_DST_INFO_OF_LUN *)tFtlTask.dDstArgsAddr);
	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;
	
	U08 bLunStep = tCB_ProcessCtrl.bLunOpStep;
	U08 *pLunQueue = (U8 *)(&tCB_ProcessCtrl.bLunQueue[0]);

	U08 j = 0;
	U08 bLunIndex = tCB_ProcessCtrl.bLunIdx;
	
	MemorySet((U8 *)(&bReadClst[0]), 0,CB_LUN_CNT);
	MemorySet((U8 *)(&bPlnMode[0]), 0,CB_LUN_CNT);

	U08 bLunStepTemp = bLunStep;
	
	while(bLunStepTemp)
	{
		bLunPair[j] = pLunQueue[bLunIndex%tCB_ProcessCtrl.bTotelLun];

		if(bLunPair[j] != 0xFF)
		{
			bLunNum = bLunPair[j];
			

			bLunChan[j] = CHANS(pSrcInfo[bLunNum].bChan);
			bChanNum = bLunChan[j];
			
			wTaskHdInLun[j] = pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum];
			wTaskTailInLun[j] = pCB_LunQueue->wTaskTailInLun[bChanNum][bLunNum];

			if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
			{	
				wSrcClstIndex[j] = pCB_LunQueue->wSrcIndexInLun[bChanNum][bLunNum];
				wDesPgIndex[j] = pCB_LunQueue->wDesIndexInLun[bChanNum][bLunNum];
				
				wLunLastTask[j] = tCB_ProcessCtrl.bLunTaskCnt[bLunNum];
			}
			else
			{
				wLunLastTask[j] = tCB_ProcessCtrl.bLunTaskCnt[bLunNum];


				if(tCB_ProcessCtrl.bValidLun>0)
				{
					tCB_ProcessCtrl.bValidLun--;
				}
			}
			j++;
			bLunStepTemp--;
		}
		bLunIndex++;
	}
	tCB_ProcessCtrl.bLunIdx = bLunIndex%tCB_ProcessCtrl.bTotelLun;
	
	U08 bUnValidClst[CB_LUN_CNT];
	U16 wRdBuffIndex[CB_LUN_CNT];
	
	MemorySet((U8 *)(&bUnValidClst[0]), 0,CB_LUN_CNT);
	MemorySet((U8 *)(&wRdBuffIndex[0]), 0,CB_LUN_CNT*sizeof(U16));
	
	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bLunChan[j];
		bLunNum = bLunPair[j];
		bSrcLunOfChan[j] = pSrcInfo[bLunNum].bLunOfChan;
		
		if((wTaskHdInLun[j] < wTaskTailInLun[j]))//证明此Lun还有任务
		{			
			wRdBuffIndex[j] = j*PLANE_OF_LUN*DSECT_OF_PAGE;
			dSrcRowAddr[j] = pSrcInfo[bLunNum].pdRowAddr;
			bSrcClstOf[j] = pSrcInfo[bLunNum].pbClstOffs;
			SrcUserDataAddr[j] = pSrcInfo[bLunNum].dUdBuffAddr;

			if(wTaskHdInLun[j] == 0) //起始 Task 并且不是最后一个Task 
			{
				bPlnMode[j] = tCB_ProcessCtrl.bLunPlnMod[bLunNum]&(0xf);
				
				if(wLunLastTask[j] == wTaskHdInLun[j]+1)
				{
					bUnValidClst[j] = pDstInfo[bLunNum].wPageCnt*DATA_CLST_OF_PAGE -pSrcInfo[bLunNum].wClstLen;
				}
			}
			else if(wLunLastTask[j] == wTaskHdInLun[j]+1)
			{
				bUnValidClst[j] = pDstInfo[bLunNum].wPageCnt*DATA_CLST_OF_PAGE -pSrcInfo[bLunNum].wClstLen;
				bPlnMode[j] = (tCB_ProcessCtrl.bLunPlnMod[bLunNum]>>4)&(0xf);
			}
			else
			{
				bPlnMode[j] = PLANE_OF_LUN;
			}
			
			bReadClst[j] = DATA_CLST_OF_PAGE*bPlnMode[j];
			bReadClst[j] -= bUnValidClst[j];
		}
	}

#if 1//def __DZ_CB_WR_FAIL__
	for(j = 0;j<bLunStep;j++)
	{
		if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
		{
			bChanNum = bLunChan[j];
			bLunNum = bLunPair[j];

			HAL_CQWaitDone(bChanNum);//等上一次各通道写CQ Done
			U08 bChip = pDstInfo[bLunNum].bLunOfChan / LUN_OF_CHIP;
			bChip = CHIPES(bChanNum, bChip);

			HAL_CQChipSelect(bChanNum,bChip);
			HAL_CQCheckIOStatus(bChanNum, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
			HAL_CQWaitDone(bChanNum);//Check上一次编程是否Fail

			if(SG_FC(bChanNum)->rRW_PULSE_DATA.bits.RW_DATA & (FIO_PF_PASS_SLC_WRITE))
			{
				//if(wWtFailCnt<8)
				{
					dWrFailAddr =  FAIL_ADDR_SET(TYPE_LOGIC,TLC_MODE,pDstInfo[bLunNum].bChan,pDstInfo[bLunNum].bLunOfChan,0);//((dDesRow&0xFFFFFF)|(bChanNum<<24)|(pDstInfo[bLunNum].bLunOfChan<<26));
				}

                pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
				//wWtFailCnt++;
			}
		}
	}
#endif

	U08 bExptFlag = CB_SrcClstReadInDieSlc(bLunStep,&bLunChan[0],&bReadClst[0],(U16 *)(&wSrcClstIndex[0]),(U32 *)(&dSrcRowAddr[0]),
							(U32 *)(&bSrcClstOf[0]),(U32 *)(&SrcUserDataAddr[0]),(U08 *)(&bSrcLunOfChan[0]));
	if(bExptFlag)
	{
		CB_ExceptionSaveMultiLunInfor(bLunPair,&bLunChan[0],(U08 *)(&bSrcLunOfChan[0]),bLunStep,(U16 *)(&wSrcClstIndex[0]));
	}
	
	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bLunChan[j];
		bLunNum = bLunPair[j];
		
		if((bReadClst[j] != 0)&&(bUnValidClst[j]))
		{
			dUserBuff = pSrcInfo[bLunNum].dUdBuffAddr+ (wSrcClstIndex[j]+bReadClst[j])*UD_LEN_OF_DATA_CLST;
			CB_SrcClstLostHandle(TYPE_LOGIC,JOINT_NONE,wRdBuffIndex[j]+bReadClst[j]*SECT_OF_DATA_CLST,dUserBuff, bUnValidClst[j]);
		}
	}

	U16 wBuffIndex;
	for(j = 0;j<bLunStep;j++)
	{
		if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
		{
			bChanNum = bLunChan[j];
			bLunNum = bLunPair[j];
			wBuffIndex = j*PLANE_OF_LUN*DSECT_OF_PAGE;
		
			dDesRow = pDstInfo[bLunNum].pdRowAddr[wDesPgIndex[j]];
			dUserBuff = pSrcInfo[bLunNum].dUdBuffAddr+wDesPgIndex[j]*DATA_CLST_OF_PAGE*UD_LEN_OF_DATA_CLST;
	
			bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_WRT_CQ_FIFO);
	
			if(bCqFifoFlag)
			{
#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
				if(tCB_ProcessCtrl.bWtCmdMode == 1)
				{
#if (NAND_FLASH == SUMSUNG_3DV4 || NAND_FLASH == AFGM)
				CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan, dDesRow,0,1,0, wBuffIndex, tFtlTask.dHandleBuff,dUserBuff, JOINT_NONE,bPlnMode[j],SLC_MODE);
#else
					CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan, dDesRow, wBuffIndex, tFtlTask.dHandleBuff,dUserBuff, JOINT_NONE,bPlnMode[j],SLC_MODE);
#endif
				}
				else
				{
					CB_SinglePlnWriteLogicPgSLC(bChanNum,pDstInfo[bLunNum].bLunOfChan, dDesRow, wBuffIndex, tFtlTask.dHandleBuff,dUserBuff,bPlnMode[j]);
				}
#else
#if (NAND_FLASH == SUMSUNG_3DV4 || NAND_FLASH == AFGM)
				CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan, dDesRow,0,1,0, wBuffIndex, tFtlTask.dHandleBuff,dUserBuff, JOINT_NONE,bPlnMode[j],SLC_MODE);
#else
				CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan, dDesRow, wBuffIndex, tFtlTask.dHandleBuff,dUserBuff, JOINT_NONE,bPlnMode[j],SLC_MODE);
#endif
#endif
			}
		}
	}

	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bLunChan[j];
		bLunNum = bLunPair[j];
		
		if((wTaskHdInLun[j] < wTaskTailInLun[j])&&(tChTaskQueue[bChanNum].wTaskCnt > 0))
		{
			tChTaskQueue[bChanNum].wTaskCnt--;
		}

		if(tChTaskQueue[bChanNum].wTaskCnt == 0)
		{
			CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_SLC_WRITE);
		}
		
		if(pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum] < pCB_LunQueue->wTaskTailInLun[bChanNum][bLunNum])
		{
			pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum]++;
			pCB_LunQueue->wDesIndexInLun[bChanNum][bLunNum]+=bPlnMode[j];
			pCB_LunQueue->wSrcIndexInLun[bChanNum][bLunNum]+=(bReadClst[j]+bUnValidClst[j]);
		}					
	}
	
	for(j = 0;j<CHAN_MAX_CNT;j++)
	{
		HAL_CQWaitDone(CHANS(j));//等上一次各通道CQ Done
	}
	
	HAL_CQGenerateInt(CHANS(0), 0, FUNC_DISABLE);
	
	return TRUE;
}

#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
U8 TaskConvertMultiLunIndieTlcCopyBack(U8 bVoid)
{		
	U08 i,j;
	
	U08 bLunNum;
	U08 bChanNum;
	
	U08 bLunPair[CB_LUN_CNT];
	U08 bSrcChans[CB_LUN_CNT];
	U08 bDesChans[CB_LUN_CNT];
	U16 wTaskHdInLun[CB_LUN_CNT];
	U16 wTaskTailInLun[CB_LUN_CNT];
	
	U08 bDesStep[CB_LUN_CNT];
	U16 wSrcClstIndex[CB_LUN_CNT];
	U16 wDesOrderIndex[CB_LUN_CNT];

	U32 dUserBuff;
	U32 dDesRow;
	U08 bCqFifoFlag;

	T_PROG_ORDER_UNIT_IN_BLK *pOrderAddr = NULL;
	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo = ((T_IN_COPY_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr);
	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo = ((T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr);

	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;

	U08 bPrgPage = 1;
	U08 bCheckClst = DATA_CLST_OF_PAGE * PLANE_OF_LUN;
	U08 bLunStep = tCB_ProcessCtrl.bLunOpStep;
	U08 bLunIndex = tCB_ProcessCtrl.bLunIdx;
	U08 *pLunQueue = (U8 *)(&tCB_ProcessCtrl.bLunQueue[0]);

	if(bLunStep>(tCB_ProcessCtrl.bTotelLun-(bLunIndex%tCB_ProcessCtrl.bTotelLun)))
	{
		bLunStep = tCB_ProcessCtrl.bTotelLun-(bLunIndex%tCB_ProcessCtrl.bTotelLun);
	}

	j = 0;
	U08 bLunStepTemp = bLunStep;
	while(bLunStepTemp)
	{
		bLunPair[j] = pLunQueue[bLunIndex%tCB_ProcessCtrl.bTotelLun];
		
		if(bLunPair[j] != 0xFF)
		{
			bLunNum = bLunPair[j];
			bSrcChans[j] = CHANS(pSrcInfo[bLunNum].bChan);
			bDesChans[j] = CHANS(pDstInfo[bLunNum].bChan);
			bChanNum = bDesChans[j];

			wTaskHdInLun[j] = pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum];
			wTaskTailInLun[j] = pCB_LunQueue->wTaskTailInLun[bChanNum][bLunNum];
			
			if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
			{	
				bDesStep[j] = pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum];
				wSrcClstIndex[j] = pCB_LunQueue->wSrcIndexInLun[bChanNum][bLunNum];
			
				wDesOrderIndex[j] = wTaskHdInLun[j];
			}
			else
			{
				if(tCB_ProcessCtrl.bValidLun>0)
				{
					tCB_ProcessCtrl.bValidLun--;
				}
			}

			j++;
			bLunStepTemp--;
		}
		bLunIndex++;
	}
	tCB_ProcessCtrl.bLunIdx = bLunIndex%tCB_ProcessCtrl.bTotelLun;

	U16 wBuffIndex;

	U32 *dSrcRowAddr[CB_LUN_CNT];
	U08 *bSrcClstOf[CB_LUN_CNT];
	U32 *SrcUserDataAddr[CB_LUN_CNT];
	U08 bSrcLunOfChan[CB_LUN_CNT]; 
	
	U08 bReadClst[CB_LUN_CNT];
	MemorySet((U8 *)(&bReadClst[0]), 0,CB_LUN_CNT);
	
	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bSrcChans[j];
		bLunNum = bLunPair[j];
		bSrcLunOfChan[j] = pSrcInfo[bLunNum].bLunOfChan;
		if((wTaskHdInLun[j] < wTaskTailInLun[j]))//证明此Lun还有任务
		{
			pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];
			
			if(tCB_ProcessCtrl.bDataType != 0)
			{
				if((pOrderAddr->bOpPageCnt>ORDER_PAGE_MAX))
				{					
					if(bDesStep[j] == 0)
					{
						bPrgPage = ORDER_PAGE_MAX;
					}
					else
					{
						bPrgPage = pOrderAddr->bOpPageCnt - ORDER_PAGE_MAX;
					}
				}
				else
				{
					bPrgPage = pOrderAddr->bOpPageCnt;
				}
			}
			bReadClst[j] = bCheckClst*bPrgPage;
						
			dSrcRowAddr[j] = pSrcInfo[bLunNum].pdRowAddr;
			bSrcClstOf[j] = pSrcInfo[bLunNum].pbClstOffs;
			SrcUserDataAddr[j] = pSrcInfo[bLunNum].pdUdBuffAddr;
		}
	}
	
	for(j = 0;j<bLunStep;j++)
	{
		if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
		{
			bChanNum = bDesChans[j];
			bLunNum = bLunPair[j];
		
			pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];
			dDesRow = pOrderAddr->dRowAddr[bDesStep[j]];
			
			HAL_CQWaitDone(bChanNum);//等上一次各通道写CQ Done
			U08 bChip = pDstInfo[bLunNum].bLunOfChan / LUN_OF_CHIP;
			bChip = CHIPES(bChanNum, bChip);
			
			HAL_CQChipSelect(bChanNum,bChip);
			HAL_CQCheckIOStatus(bChanNum, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
			HAL_CQWaitDone(bChanNum);//Check上一次编程是否Fail

			if(SG_FC(bChanNum)->rRW_PULSE_DATA.bits.RW_DATA & (FIO_PF_PASS_TLC_WRITE))
			{
				//if(wWtFailCnt<8)
				{
					dWrFailAddr =  FAIL_ADDR_SET(TYPE_LOGIC,TLC_MODE,pDstInfo[bLunNum].bChan,pDstInfo[bLunNum].bLunOfChan,dDesRow);//((dDesRow&0xFFFFFF)|(bChanNum<<24)|(pDstInfo[bLunNum].bLunOfChan<<26));
				}

                pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
				//wWtFailCnt++;
			}
		}
	}

	U08 bExptFlag = CB_SrcClstReadInDieTlc(bLunStep,&bSrcChans[0],&bReadClst[0],(U16 *)(&wSrcClstIndex[0]),
			(U32 *)(&dSrcRowAddr[0]),(U32 *)(&bSrcClstOf[0]),(U32 *)(&SrcUserDataAddr[0]),(U08 *)(&bSrcLunOfChan[0]));

	if(bExptFlag)
	{
		CB_ExceptionSaveMultiLunInfor(bLunPair,&bSrcChans[0],(U08 *)(&bSrcLunOfChan[0]),bLunStep,(U16 *)(&wSrcClstIndex[0]));
	}

	U08 bPageIdx;

#ifdef __TLC_AUTO_FILL__
	U32 dRowLast[CB_LUN_CNT];
#endif

	U16 wLunBuffSize = PLANE_OF_LUN*SECT_OF_PAGE;
	if(tCB_ProcessCtrl.bDataType != 0)
	{
		 wLunBuffSize *= ORDER_PAGE_MAX;
	}

	U08 bLunPrgPage[8] = {0};
	for(j = 0;j<bLunStep;j++)
	{
		bLunPrgPage[j] = bReadClst[j]/bCheckClst;
	}


	if(tCB_ProcessCtrl.bWtCmdMode != 0)
	{
		for(i = 0;i<ORDER_PAGE_MAX;i++)
		{
			for(j = 0;j<bLunStep;j++)
			{
				if(i>=bLunPrgPage[j])
				{
					continue;
				}

				if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
				{
					bChanNum = bDesChans[j];
					bLunNum = bLunPair[j];
					wBuffIndex = j*wLunBuffSize+i*PLANE_OF_LUN*DSECT_OF_PAGE;

					pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];

					bPageIdx = bDesStep[j]+i;
					dDesRow = pOrderAddr->dRowAddr[bPageIdx];

#ifdef __TLC_AUTO_FILL__
					dRowLast[j] = dDesRow;
#endif
					dUserBuff = pOrderAddr->dUserDataAddr[bPageIdx];

					bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_WRT_CQ_FIFO);

					if(bCqFifoFlag)
					{
#if (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)
						CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dDesRow,pOrderAddr->wOrderNum,bPageIdx+1,pOrderAddr->bOpPageCnt,wBuffIndex,tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
#else
						CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dDesRow,wBuffIndex,tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
#endif
					}
				}
			}
		}
	}
	else
	{
		U16 wBuffIdx[ORDER_PAGE_MAX];
		U32 dUDAddr[ORDER_PAGE_MAX];
		U32 dRow[ORDER_PAGE_MAX];
		U08 bPln;

		for(j = 0;j<bLunStep;j++)
		{
			if(wTaskHdInLun[j] < wTaskTailInLun[j])
			{
				bChanNum = bDesChans[j];
				bLunNum = bLunPair[j];

				for(bPln = 0;bPln < PLANE_OF_LUN;bPln++)
				{
					for(i = 0;i<bLunPrgPage[j];i++)
					{
						wBuffIdx[i] = j*wLunBuffSize+i*PLANE_OF_LUN*DSECT_OF_PAGE+bPln*DSECT_OF_PAGE;

						pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];
						bPageIdx = bDesStep[j]+i;

						dRow[i] = pOrderAddr->dRowAddr[bPageIdx]+bPln*ROW_GAP_OF_PLANE;
						dUDAddr[i] = pOrderAddr->dUserDataAddr[bPageIdx]+bPln*DATA_CLST_OF_PAGE*UD_LEN_OF_DATA_CLST;
					}
#if (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)//(NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)
					CB_SinglePlnWriteLogicPgTLC(bChanNum,pDstInfo[bLunNum].bLunOfChan,bPln,dRow,wBuffIdx,tFtlTask.dHandleBuff, dUDAddr,bLunPrgPage[j],TLC_MODE);
#else
//					FDL_Log("Pln = %x! Buff = %x,%x! UD = %x,%x! Row = %x,%x",bPln,wBuffIndex[0],wBuffIndex[1],dUDAddr[0],dUDAddr[1],dRow[0],dRow[1]);
					CB_SinglePlnWriteLogicPgTLC(bChanNum,pDstInfo[bLunNum].bLunOfChan,dRow,wBuffIdx,tFtlTask.dHandleBuff, dUDAddr,bLunPrgPage[j],TLC_MODE);
#endif
				}
			}
		}
	}

	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bDesChans[j];
		bLunNum = bLunPair[j];

		pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum]+= bLunPrgPage[j];
		pCB_LunQueue->wSrcIndexInLun[bChanNum][bLunNum] += bReadClst[j];

		pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];

		if(pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum] == pOrderAddr->bOpPageCnt)
		{
			if((wTaskHdInLun[j] < wTaskTailInLun[j])&&(tChTaskQueue[bChanNum].wTaskCnt > 0))
			{
				tChTaskQueue[bChanNum].wTaskCnt--;
			}

			pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum]++;
			pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum] = 0;
		}

		if(tChTaskQueue[bChanNum].wTaskCnt == 0)
		{
			CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_TLC_WRITE);
		}

#ifdef __TLC_AUTO_FILL__
		if((dRowLast[j]&(0xBD7)) == (0xBD7))
		{
			if(tCB_ProcessCtrl.bBlkAutoFill[bLunNum] == 0)
			{
				U32 dRowFill[4] = {3033,3037,3041,3045};
				U16 m,n;
				U32 dPhyRow;
				U32 dBaseRow = dRowLast[j]/ROW_GAP_OF_PLANE*ROW_GAP_OF_PLANE;

				for(m = 0;m<4;m++)
				{
					for(n = 0;n<3;n++)
					{
						dPhyRow = dBaseRow+dRowFill[m]+n;
						HAL_CQWaitIdleSpace(bChanNum,0x100);
						CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dPhyRow|0xFF000000,0,tFtlTask.dHandleBuff,tFtlTask.dHandleBuff+0x4000*PLANE_OF_LUN,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
						CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_TLC_WRITE);
					}
				}

				for(m = 0;m<12;m++)
				{
					dPhyRow =  dBaseRow+3049+m*2;
					HAL_CQWaitIdleSpace(bChanNum,0x100);
					CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dPhyRow|0xFF000000,0,tFtlTask.dHandleBuff,tFtlTask.dHandleBuff+0x4000*PLANE_OF_LUN,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
					CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_TLC_WRITE);
				}
				tCB_ProcessCtrl.bBlkAutoFill[bLunNum] = 0xAB;
			}
		}
#endif
	}


	HAL_CQGenerateInt(bDesChans[0], 0, FUNC_DISABLE);

	return TRUE;
}

#else

U8 TaskConvertMultiLunIndieTlcCopyBack(U8 bVoid)
{
	U08 i,j;

	U08 bLunNum;
	U08 bChanNum;

	U08 bLunPair[CB_LUN_CNT];
	U08 bLunChan[CB_LUN_CNT];
	U16 wTaskHdInLun[CB_LUN_CNT];
	U16 wTaskTailInLun[CB_LUN_CNT];

	U08 bDesStep[CB_LUN_CNT];
	U16 wSrcClstIndex[CB_LUN_CNT];
	U16 wDesOrderIndex[CB_LUN_CNT];

	U32 dUserBuff;
	U32 dDesRow;
	U08 bCqFifoFlag;

	T_PROG_ORDER_UNIT_IN_BLK *pOrderAddr = NULL;
	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo = ((T_IN_COPY_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr);
	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo = ((T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr);

	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;

	U08 bPrgPage = 1;
	U08 bCheckClst = DATA_CLST_OF_PAGE * PLANE_OF_LUN;
	U08 bLunStep = tCB_ProcessCtrl.bLunOpStep;
	U08 bLunIndex = tCB_ProcessCtrl.bLunIdx;
	U08 *pLunQueue = (U8 *)(&tCB_ProcessCtrl.bLunQueue[0]);

	if(bLunStep>(tCB_ProcessCtrl.bTotelLun-(bLunIndex%tCB_ProcessCtrl.bTotelLun)))
	{
		bLunStep = tCB_ProcessCtrl.bTotelLun-(bLunIndex%tCB_ProcessCtrl.bTotelLun);
	}

	j = 0;
	U08 bLunStepTemp = bLunStep;
	while(bLunStepTemp)
	{
		bLunPair[j] = pLunQueue[bLunIndex%tCB_ProcessCtrl.bTotelLun];

		if(bLunPair[j] != 0xFF)
		{
			bLunNum = bLunPair[j];
			bLunChan[j] = CHANS(pSrcInfo[bLunNum].bChan);
			bChanNum = bLunChan[j];

			wTaskHdInLun[j] = pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum];
			wTaskTailInLun[j] = pCB_LunQueue->wTaskTailInLun[bChanNum][bLunNum];

			if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
			{
				bDesStep[j] = pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum];
				wSrcClstIndex[j] = pCB_LunQueue->wSrcIndexInLun[bChanNum][bLunNum];

				wDesOrderIndex[j] = wTaskHdInLun[j];
			}
			else
			{
				if(tCB_ProcessCtrl.bValidLun>0)
				{
					tCB_ProcessCtrl.bValidLun--;
				}
			}

			j++;
			bLunStepTemp--;
		}
		bLunIndex++;
	}
	tCB_ProcessCtrl.bLunIdx = bLunIndex%tCB_ProcessCtrl.bTotelLun;

	U16 wBuffIndex;

	U32 *dSrcRowAddr[CB_LUN_CNT];
	U08 *bSrcClstOf[CB_LUN_CNT];
	U32 *SrcUserDataAddr[CB_LUN_CNT];
	U08 bSrcLunOfChan[CB_LUN_CNT];

	U08 bReadClst[CB_LUN_CNT];
	MemorySet((U8 *)(&bReadClst[0]), 0,CB_LUN_CNT);

	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bLunChan[j];
		bLunNum = bLunPair[j];
		bSrcLunOfChan[j] = pSrcInfo[bLunNum].bLunOfChan;
		if((wTaskHdInLun[j] < wTaskTailInLun[j]))//证明此Lun还有任务
		{
			pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];

			if(tCB_ProcessCtrl.bDataType != 0)
			{
				if((pOrderAddr->bOpPageCnt>ORDER_PAGE_MAX))
				{
					if(bDesStep[j] == 0)
					{
						bPrgPage = ORDER_PAGE_MAX;
					}
					else
					{
						bPrgPage = pOrderAddr->bOpPageCnt - ORDER_PAGE_MAX;
					}
				}
				else
				{
					bPrgPage = pOrderAddr->bOpPageCnt;
				}
			}
			bReadClst[j] = bCheckClst*bPrgPage;

			dSrcRowAddr[j] = pSrcInfo[bLunNum].pdRowAddr;
			bSrcClstOf[j] = pSrcInfo[bLunNum].pbClstOffs;
			SrcUserDataAddr[j] = pSrcInfo[bLunNum].pdUdBuffAddr;
		}
	}

	for(j = 0;j<bLunStep;j++)
	{
		if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
		{
			bChanNum = bLunChan[j];
			bLunNum = bLunPair[j];

			pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];
			dDesRow = pOrderAddr->dRowAddr[bDesStep[j]];

			HAL_CQWaitDone(bChanNum);//等上一次各通道写CQ Done
			U08 bChip = pDstInfo[bLunNum].bLunOfChan / LUN_OF_CHIP;
			bChip = CHIPES(bChanNum, bChip);

			HAL_CQChipSelect(bChanNum,bChip);
			HAL_CQCheckIOStatus(bChanNum, 0, IO_1_PASS, FIO_RB_READY_CPYB, FIO_PF_PASS_NULL);
			HAL_CQWaitDone(bChanNum);//Check上一次编程是否Fail

			if(SG_FC(bChanNum)->rRW_PULSE_DATA.bits.RW_DATA & (FIO_PF_PASS_TLC_WRITE))
			{
				//if(wWtFailCnt<8)
				{
					dWrFailAddr =  FAIL_ADDR_SET(TYPE_LOGIC,TLC_MODE,pDstInfo[bLunNum].bChan,pDstInfo[bLunNum].bLunOfChan,dDesRow);//((dDesRow&0xFFFFFF)|(bChanNum<<24)|(pDstInfo[bLunNum].bLunOfChan<<26));
				}

                pFdlParam.FTL_ExceptionWriteFailHandle(1,dWrFailAddr);
				//wWtFailCnt++;
			}
		}
	}
	U08 bExptFlag = CB_SrcClstReadInDieTlc(bLunStep,&bLunChan[0],&bReadClst[0],(U16 *)(&wSrcClstIndex[0]),
			(U32 *)(&dSrcRowAddr[0]),(U32 *)(&bSrcClstOf[0]),(U32 *)(&SrcUserDataAddr[0]),(U08 *)(&bSrcLunOfChan[0]));

	if(bExptFlag)
	{
		CB_ExceptionSaveMultiLunInfor(bLunPair,&bLunChan[0],(U08 *)(&bSrcLunOfChan[0]),bLunStep,(U16 *)(&wSrcClstIndex[0]));
	}

	U08 bPageIdx;
	
#ifdef __TLC_AUTO_FILL__
	U32 dRowLast[CB_LUN_CNT];
#endif

	U16 wLunBuffSize = PLANE_OF_LUN*SECT_OF_PAGE;
	if(tCB_ProcessCtrl.bDataType != 0)
	{
		 wLunBuffSize *= ORDER_PAGE_MAX;
	}

	U08 bLunPrgPage[8] = {0};
	for(j = 0;j<bLunStep;j++)
	{
		bLunPrgPage[j] = bReadClst[j]/bCheckClst;
	}


	for(i = 0;i<ORDER_PAGE_MAX;i++)
	{
		for(j = 0;j<bLunStep;j++)
		{
			if(i>=bLunPrgPage[j])
			{
				continue;
			}
			//bPrgPage = bReadClst[j]/bCheckClst;
			//for(i = 0;i<bPrgPage;i++)
			{
				if(wTaskHdInLun[j] < wTaskTailInLun[j])//证明此Lun还有任务
				{
					bChanNum = bLunChan[j];
					bLunNum = bLunPair[j];
					wBuffIndex = j*wLunBuffSize+i*PLANE_OF_LUN*DSECT_OF_PAGE;

					pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];

					bPageIdx = bDesStep[j]+i;
					dDesRow = pOrderAddr->dRowAddr[bPageIdx];

	#ifdef __TLC_AUTO_FILL__
					dRowLast[j] = dDesRow;
	#endif
					dUserBuff = pOrderAddr->dUserDataAddr[bPageIdx];

					bCqFifoFlag = HAL_CQWaitIdleSpace(bChanNum,CB_WRT_CQ_FIFO);
				
					if(bCqFifoFlag)
					{
	#if (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)
						CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dDesRow,pOrderAddr->wOrderNum,bPageIdx+1,pOrderAddr->bOpPageCnt,wBuffIndex,tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
	#else
						CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dDesRow,wBuffIndex,tFtlTask.dHandleBuff,dUserBuff,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
	#endif
					}
				}
			}
		}
	}

	for(j = 0;j<bLunStep;j++)
	{
		bChanNum = bLunChan[j];
		bLunNum = bLunPair[j];
		
		pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum]+= bLunPrgPage[j];
		pCB_LunQueue->wSrcIndexInLun[bChanNum][bLunNum] += bReadClst[j];

		pOrderAddr = pDstInfo[bLunNum].ptOrderUnit+wDesOrderIndex[j];

		if(pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum] == pOrderAddr->bOpPageCnt)
		{
			if((wTaskHdInLun[j] < wTaskTailInLun[j])&&(tChTaskQueue[bChanNum].wTaskCnt > 0))
			{
				tChTaskQueue[bChanNum].wTaskCnt--;
			}
			
			pCB_LunQueue->wTaskHeadInLun[bChanNum][bLunNum]++;
			pCB_LunQueue->bCurStepInTask[bChanNum][bLunNum] = 0;
		}

		if(tChTaskQueue[bChanNum].wTaskCnt == 0)
		{
			CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_TLC_WRITE);
		}

#ifdef __TLC_AUTO_FILL__
		if((dRowLast[j]&(0xBD7)) == (0xBD7))
		{
			if(tCB_ProcessCtrl.bBlkAutoFill[bLunNum] == 0)
			{
				U32 dRowFill[4] = {3033,3037,3041,3045};
				U16 m,n;
				U32 dPhyRow;
				U32 dBaseRow = dRowLast[j]/ROW_GAP_OF_PLANE*ROW_GAP_OF_PLANE;

				for(m = 0;m<4;m++)
				{
					for(n = 0;n<3;n++)
					{
						dPhyRow = dBaseRow+dRowFill[m]+n;
						HAL_CQWaitIdleSpace(bChanNum,CQ_MAX_IDLE_SPACE);
						CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dPhyRow|0xFF000000,0,tFtlTask.dHandleBuff,tFtlTask.dHandleBuff+0x4000*PLANE_OF_LUN,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
						CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_TLC_WRITE);
					}
				}

				for(m = 0;m<12;m++)
				{
					dPhyRow =  dBaseRow+3049+m*2;
					HAL_CQWaitIdleSpace(bChanNum,CQ_MAX_IDLE_SPACE);
					CB_MultiPlnWriteLogicPg(bChanNum,pDstInfo[bLunNum].bLunOfChan,dPhyRow|0xFF000000,0,tFtlTask.dHandleBuff,tFtlTask.dHandleBuff+0x4000*PLANE_OF_LUN,JOINT_NONE,PLANE_OF_LUN,TLC_MODE);
					CB_CheckWriteRealDone(bChanNum,FIO_PF_PASS_TLC_WRITE);
				}
				tCB_ProcessCtrl.bBlkAutoFill[bLunNum] = 0xAB;
			}
		}
#endif
	}
	

	HAL_CQGenerateInt(bLunChan[0], 0, FUNC_DISABLE);
	
	return TRUE;
}
#endif

U8 TaskConvertMultiLunInnerTlcCopyBack(U8 bLunPairCnt)
{	
	U08 bCqFlag;
	U08 bLunPair;
	U08 bDesStep;
	
	U16 wTaskTail;
	U32 dDesRow;
	
	U08 bLun[4];
	U08 bChan[4];
	U16 wSrcPgIndex[4];
	U16 wTaskOffset[4];
	U32 *pdSrcRow[4];
	U32 dDmaInfoBase[4];
	
	U08 bDmaInforSize = PLANE_OF_LUN*DMA_OF_DATA_CLST*DATA_CLST_OF_PAGE*2;
	
	T_IN_COPY_SRC_INFO_OF_LUN *pSrcInfo = ((T_IN_COPY_SRC_INFO_OF_LUN *)tFtlTask.dSrcArgsAddr);
	T_FLASH_ORDER_PROG_INFO_OF_LUN *pDstInfo = ((T_FLASH_ORDER_PROG_INFO_OF_LUN *)tFtlTask.dDstArgsAddr);
	pT_CB_LUN_QUEUE pCB_LunQueue = (pT_CB_LUN_QUEUE)tCB_ProcessCtrl.dLunQueueAddr;
	
	for(bLunPair = 0;bLunPair < bLunPairCnt;bLunPair++)
	{
		bChan[bLunPair] = CHANS(pSrcInfo[bLunPair].bChan);
		bLun[bLunPair] = pSrcInfo[bLunPair].bLunOfChan;
		
		wTaskOffset[bLunPair] = pCB_LunQueue->wTaskHeadInLun[bChan[bLunPair]][bLunPair];
		wTaskTail = pCB_LunQueue->wTaskTailInLun[bChan[bLunPair]][bLunPair];
		
		if(wTaskOffset[bLunPair] >= wTaskTail)
		{
			continue;
		}
		
		wSrcPgIndex[bLunPair] = pCB_LunQueue->wSrcIndexInLun[bChan[bLunPair]][bLunPair];
		pdSrcRow[bLunPair] = (U32 *)(&pSrcInfo[bLunPair].pdRowAddr[wSrcPgIndex[bLunPair]]); 
		
		dDmaInfoBase[bLunPair] = tFtlTask.dEccStsBuff+bLunPair*bDmaInforSize;		
	}	
	
	MemorySet((U8 *)tFtlTask.dEccStsBuff, 0,bLunPairCnt*bDmaInforSize); //按最大启动DMA数计算Infor地址
	
	for(bLunPair = 0;bLunPair < bLunPairCnt;bLunPair++)
	{
		bCqFlag = HAL_CQWaitDone(bChan[bLunPair]);
		
		if(bCqFlag)
		{
			CB_InnerCopyBackRead(bChan[bLunPair],bLun[bLunPair], pdSrcRow[bLunPair],PLANE_OF_LUN);
		}
	}
	
	for(bLunPair = 0;bLunPair < bLunPairCnt;bLunPair++)
	{		
		//需要UserData的Dma数据出来		Dma很多参数不需要配置(UserData相关的参数)
		CB_InnerCopyBackStartDma2(bChan[bLunPair],bLun[bLunPair], pdSrcRow[bLunPair],tFtlTask.dHandleBuff,&pSrcInfo[bLunPair].pdUdBuffAddr[wSrcPgIndex[bLunPair]],PLANE_OF_LUN,&pSrcInfo[bLunPair].pbClstOffs[wSrcPgIndex[bLunPair]],dDmaInfoBase[bLunPair]);
	}
	
	U08 bChip; 
	U08 bChipLast;
	U08 bExptFlag = 0;
	
	MemorySet((U8 *)(&tRetryInfor), 0,36);
	
	for(bLunPair = 0;bLunPair < bLunPairCnt;bLunPair++)
	{
		bCqFlag = HAL_CQWaitDone(bChan[bLunPair]);
		
		if(bCqFlag)
		{
			bExptFlag +=  CB_HandleReadExCeptionInnerTlc(bChan[bLunPair],bLun[bLunPair],wSrcPgIndex[bLunPair],pdSrcRow[bLunPair],&pSrcInfo[bLunPair].pdUdBuffAddr[wSrcPgIndex[bLunPair]],&pSrcInfo[bLunPair].pbClstOffs[wSrcPgIndex[bLunPair]],dDmaInfoBase[bLunPair]);
		}
		
		bChip = bLun[bLunPair] / LUN_OF_CHIP;			
   		bChip = CHIPES(bChan[bLunPair], bChip);
		
		if(bLunPair == 0)
		{
			tRetryInfor.m_bCBFailDmaOffst[bChan[bLunPair]][bChip] = 0;
		}
		else
		{
			bChipLast = bLun[bLunPair-1] / LUN_OF_CHIP;
			bChipLast = CHIPES(bChan[bLunPair-1], bChipLast);
			tRetryInfor.m_bCBFailDmaOffst[bChan[bLunPair]][bChip] = tRetryInfor.m_bCBFailDmaOffst[bChan[bLunPair-1]][bChipLast]+tRetryInfor.m_bFailDmaCnt[bChan[bLunPair-1]][bChipLast];
		}
	}
		
	if(bExptFlag)
	{	
		FDL_CopyBackRetry();
	}	
			
	T_PROG_ORDER_UNIT_IN_BLK *pOrderAddr;
	
	for(bLunPair = 0;bLunPair < bLunPairCnt;bLunPair++)
	{
		pOrderAddr = pDstInfo[bLunPair].ptOrderUnit+wTaskOffset[bLunPair];
		bDesStep = pCB_LunQueue->bCurStepInTask[bChan[bLunPair]][bLunPair];
		dDesRow = pOrderAddr->dRowAddr[bDesStep];		
		CB_InnerCopyBackWrite(bChan[bLunPair],bLun[bLunPair],dDesRow,PLANE_OF_LUN);
		
		pCB_LunQueue->bCurStepInTask[bChan[bLunPair]][bLunPair]++;
		pCB_LunQueue->wSrcIndexInLun[bChan[bLunPair]][bLunPair]+=PLANE_OF_LUN;
			
		if(pCB_LunQueue->bCurStepInTask[bChan[bLunPair]][bLunPair] == pOrderAddr->bOpPageCnt)
		{
			pCB_LunQueue->bCurStepInTask[bChan[bLunPair]][bLunPair] = 0;
			pCB_LunQueue->wTaskHeadInLun[bChan[bLunPair]][bLunPair]++;
			tChTaskQueue[bChan[bLunPair]].wTaskCnt--;
		}
	}
	HAL_CQGenerateInt(0, 0, FUNC_DISABLE);
	
	return TRUE;
}

