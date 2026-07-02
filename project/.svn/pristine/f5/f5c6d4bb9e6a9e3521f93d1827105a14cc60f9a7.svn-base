#include "fdl_write.h"
#include "fdl_common.h"
#include "uitility.h"
#include "hal_cq.h"

void WirteStartDMA2ByCQ(U8 bChan,U32 dOfst,U8 bClstOfPage,U8 bSectOfClst,Joint_Mode joint)
{
    U8 i = 0;

	U32 dBuffIdx;
	
    if (tFtlTask.bArgsArr[0] == DISCRETE_NONE)
    {
		HAL_CQStartDMA2Write(bChan, tFtlTask.dHandleBuff, DMA2_WR_TO_FLASH, SECT_OF_PAGE * dOfst, 
						0, SECT_OF_PAGE, joint, (U32)NULL);
    }
    else if (tFtlTask.bArgsArr[0] == DISCRETE_CLST)//离散Buff写Flash暂时不考虑掉头情况
    {
        for (i = 0; i < bClstOfPage; i++)
        {
        	dBuffIdx = tFtlTask.pwBuffIdx[dOfst*bClstOfPage + i];
			if ((dOfst*bClstOfPage + i) >= tFtlTask.bBuffClstCnt)
			{
				dBuffIdx = tFtlTask.pwBuffIdx[0];
			}
            HAL_CQCheckRealDone(bChan);
            HAL_CQStartDMA2(bChan, tFtlTask.dHandleBuff, DMA2_WR_TO_FLASH, dBuffIdx, 
            		bSectOfClst*i, SECT_OF_DATA_CLST, joint, (U32)NULL);        
		}
    }
    else
    {
        HAL_CQStartDMA2(bChan, ((U32*)tFtlTask.dHandleBuff)[dOfst], DMA2_WR_TO_FLASH,//dOfst掉头待处理
                        0, 0, SECT_OF_PAGE, joint, (U32)NULL);
    }

}


#ifdef __FILL_SLC_BLOCKS__
U08 TaskConvertFillBlock(U08 bChan)
{
	U32 dMulitInfo			= 0;/*任务分配时传下来的多重信息*/	
	U08 bOPMode2nd 			= NORMAL_OP_MODE;	
	U08 bLogLunOfChan 		= 0;
	Joint_Mode bJointMode 	= JOINT_NONE;
	T_ADDR_INFO tAddr1st;
	T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChan];

	dMulitInfo		= tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
	bLogLunOfChan 	= VER_TLC_GET_LOG_CE(dMulitInfo);/*逻辑片选用2Bit表示*/

	Fdl_ConvertAddrInfoVerTlc(&tAddr1st,bChan,tChTaskQ);
	
    tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan];
	/*****************************analyse next address for op*********************************/
	if(MULTI_PLANE_EN)
    {			
		if( tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > 1)/*The LUN address bit must be identical*/
		{
			U32 dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead + 1)&(MAX_CHAN_TASK-1)])&0x00ffffff;
			U08 bPlaneNumCur  = (tAddr1st.dRow/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*当前Row地址的Plane Num*/
			U08 bPlaneNumNext  = (dRowOpNext/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*下一个Row地址的Plane Num*/
			
			if( ((tAddr1st.dRow&(ROW_GAP_OF_PLANE - 1)) == (dRowOpNext&(ROW_GAP_OF_PLANE - 1)))&&/*page address bits must be identical*/
				((tAddr1st.dRow&(~PLANE_BIT_MASK_OF_ROW)) != (dRowOpNext&(~PLANE_BIT_MASK_OF_ROW))))/*plane select bits must be different*/
			{
				if(1 != ((tChTaskQ->bPlaneWriteFlag[bLogLunOfChan]>>bPlaneNumNext) & 0x01))/*避免Plane顺序为0 2 0或者0 1 2 0的情况*/
				{
					bOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
					/*标记数据已经写入当前Plane的数据缓存区*/
					tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] |= (1<<bPlaneNumCur);/*目前支持2Plane、4Plane、8Plane*/
				}
			}
		}
    }
	
    if(CACHE_WR_EN)
    {
		if(	(bOPMode2nd != MULTI_PLN_OP_MODE) && //不是多Plane
			((tAddr1st.dRow & (PAGE_OF_BLOCK_SLC - 1)) != (PAGE_OF_BLOCK_SLC - 1)))//当前不是最后一页,就发Cache命令
		{
			bOPMode2nd = CACEH_OP_MODE;//发80-15
			tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] = 0;/*每个Plane缓存区里面的数据已经被推到Flash Array,清零标记*/
		}
    }

    tChTaskQ->bNextAddrOPMode[bLogLunOfChan] = bOPMode2nd;	
	/*****************************start dma2 of current ofst*********************************/ 
	
	HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);	
	HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);
	
    if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
    {
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1_MP);
    }
    else
    {
		FL_CMD_CQ_SLC_MODE(tAddr1st.bChan);
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1);
    }

	HAL_CQSend5Addr(tAddr1st.bChan, DATA_COL_ADDR(0), tAddr1st.dRow);
	HAL_CQSetUd(tAddr1st.bChan, tFtlTask.dUdBuff, tAddr1st.bSecOfClst , tAddr1st.bUdSize,0);
	HAL_CQSetRndSeed(tAddr1st.bChan,tAddr1st.dRow,INVALID_U16);
	WirteStartDMA2ByCQ(tAddr1st.bChan,0,tAddr1st.bClstOfPage,tAddr1st.bSecOfClst,bJointMode);

	/*****************************send program cmd*********************************/
	if(bOPMode2nd == MULTI_PLN_OP_MODE)
	{
	  	HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2_MP);
	}
	else if(bOPMode2nd == CACEH_OP_MODE)
	{
	  	HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_CACHE_PROG2);
	}
	else
	{
	  	HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2);
	}

    /******************chan task end, check all lun/ce status*********************/    
    HAL_CQGenerateInt(tAddr1st.bChan, 0, FALSE);

	tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
	tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;

	return TRUE;
}
#endif




U08 TaskConvertWritePageToFlash(U08 bChanPhy)
{
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	U32 dOfstReal = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
	U16 dOfstBuff = dOfstReal & (tFtlTask.wContiAddrMax - 1);//注意:tFtlTask.wContiAddrMax要是2的幂次方
	
    T_ADDR_INFO 	 tAddr1st;
	T_FLASH_PHY_ADDR *ptPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfstBuff]);	
	
	U32 dLogNum;
    U16 wSeedIdx 			= INVALID_U16;
	U08 bOPMode2nd 			= NORMAL_OP_MODE;
    U08 bLogLunOfChan 		= ptPageAddr->m_bLunOfChan;
    Joint_Mode bJointMode 	= JOINT_NONE;

    if(TYPE_NCQ_WRITE == tFtlTask.dTaskType)
    {
        bJointMode = JOINT_21;
    }
	else if (TYPE_WRITE_PAGES == tFtlTask.dTaskType)
	{
		bJointMode = tFtlTask.bJointMode;
	}
	
	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptPageAddr,!TYPE_ERASE_BLOCKS);
    tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan];
    
    /*****************************check last lun/ce status*********************************/  
#if 0
    dLogNum = tChTaskQ->wOfstLunRecord[bLogLunOfChan] | 
    		  (tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]<<LUN_OP_CNT_IN_LOGNUM)|
    		  (tChTaskQ->bPreLunContiOpCnt[bLogLunOfChan]<<PRE_OP_CNT_IN_LOGNUM);
#else
	dLogNum = dOfstReal;
#endif
    
    HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
    HAL_CQCfgLogicNum(tAddr1st.bChan,dLogNum);
#ifdef _TLC_ORDER_PROGRAM_
	HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstReal*tAddr1st.bClstOfPage*tAddr1st.bUdSize,tAddr1st.bSecOfClst,tAddr1st.bUdSize,DMA2_WR_TO_FLASH);
	U08 bCurWriteMode;
	if ((tAddr1st.dRow >> 24) == 0)
	{
		bCurWriteMode = 0;
	}
	else
	{
		bCurWriteMode = 1;
	}
	if (bCurWriteMode == tFtlTask.bLastWriteMode[bChanPhy])
	{
		if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);/*11h后面也不检测Pass Or Fail*/
		}
		else
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_SLC_CACHE_WRITE);/*Cache Programma SLC Previous Pass Or FailBit*/
		}
	}
	else
	{
		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	}
#else
	if(0 == tChTaskQ->bFirstFlag)
	{
		HAL_CQCheckHold(DMA2_WR_TO_FLASH,bJointMode,tAddr1st.bChan,0,0,0);/*调用的目的是为了切方向为写*/
		HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
		HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstBuff*tAddr1st.bClstOfPage*tAddr1st.bUdSize,tAddr1st.bSecOfClst,tAddr1st.bUdSize,DMA2_WR_TO_FLASH);
		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);/*第一次进来不检测Pass Or Fail*/  		
		tChTaskQ->bFirstFlag = 1;
	}
	else
	{
		if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);/*11h后面也不检测Pass Or Fail*/  		
		}
		else
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_SLC_CACHE_WRITE);/*Cache Programma SLC Previous Pass Or FailBit*/		 
		}
	}
#endif

    if(tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan] != 0)
    {
        tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]--;
    }

    /*****************************analyse next address for op*********************************/
    if(MULTI_PLANE_EN)
    {			
		if( tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > 1)/*The LUN address bit must be identical*/
		{
			U32 dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead + 1)&(MAX_CHAN_TASK-1)])&0x00ffffff;
			U08 bPlaneNumCur  = (ptPageAddr->m_dRowAddr/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*当前Row地址的Plane Num*/
			U08 bPlaneNumNext  = (dRowOpNext/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*下一个Row地址的Plane Num*/
			
			if( ((ptPageAddr->m_dRowAddr&(ROW_GAP_OF_PLANE - 1)) == (dRowOpNext&(ROW_GAP_OF_PLANE - 1)))&&/*page address bits must be identical*/
				((ptPageAddr->m_dRowAddr&(~PLANE_BIT_MASK_OF_ROW)) != (dRowOpNext&(~PLANE_BIT_MASK_OF_ROW))))/*plane select bits must be different*/
			{
				if(1 != ((tChTaskQ->bPlaneWriteFlag[bLogLunOfChan]>>bPlaneNumNext) & 0x01))/*避免Plane顺序为0 2 0或者0 1 2 0的情况*/
				{
					bOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
					/*标记数据已经写入当前Plane的数据缓存区*/
					tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] |= (1<<bPlaneNumCur);/*目前支持2Plane、4Plane、8Plane*/
				}
			}
		}
    }
	
    if(CACHE_WR_EN && (TYPE_NCQ_WRITE == tFtlTask.dTaskType))
    {
		if(	(bOPMode2nd != MULTI_PLN_OP_MODE) && //不是多Plane
			((ptPageAddr->m_dRowAddr & (PAGE_OF_BLOCK_SLC - 1)) != (PAGE_OF_BLOCK_SLC - 1)))//当前不是最后一页,就发Cache命令
		{
			bOPMode2nd = CACEH_OP_MODE;//发80-15
			tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] = 0;/*每个Plane缓存区里面的数据已经被推到Flash Array,清零标记*/
		}
    }

    tChTaskQ->bNextAddrOPMode[bLogLunOfChan] = bOPMode2nd;
    tChTaskQ->wOfstLunRecord[bLogLunOfChan]  = dOfstReal;
    
    /*****************************record pre continus op page cnt*********************************/
    if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == NORMAL_OP_MODE)
    {
        tChTaskQ->bPreLunContiOpCnt[bLogLunOfChan] = 0;
		tChTaskQ->bLunContinueOpCnt[bLogLunOfChan] = 0;
    }
    else if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == CACEH_OP_MODE)
    {    	
        tChTaskQ->bPreLunContiOpCnt[bLogLunOfChan] = tChTaskQ->bLunContinueOpCnt[bLogLunOfChan];		
        tChTaskQ->bLunContinueOpCnt[bLogLunOfChan] = 0;
    }

    /*****************************start dma2 of current ofst*********************************/ 
    if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
    {
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1_MP);
    }
    else
    {
#ifdef _TLC_ORDER_PROGRAM_
		if((tAddr1st.dRow >> 24) == 0)
		{
			FL_CMD_CQ_SLC_MODE(bChanPhy);
			tFtlTask.bLastWriteMode[bChanPhy] = 0;
		}
		else
		{
			FL_CMD_CQ_TLC_MODE(bChanPhy);
			tFtlTask.bLastWriteMode[bChanPhy] = 1;
		}
#else
		FL_CMD_CQ_SLC_MODE(bChanPhy);

#endif
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1);
    }

    HAL_CQSend5Addr(tAddr1st.bChan, DATA_COL_ADDR(0), tAddr1st.dRow);
	HAL_CQSetUdBaseAddr(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstBuff*tAddr1st.bClstOfPage*tAddr1st.bUdSize,DMA2_WR_TO_FLASH);//bDir 1:read    0:write    


#ifdef __RAID_FUNCTION_ON__
	if(TYPE_NCQ_WRITE == tFtlTask.dTaskType)
	{
		if(GET_PAGE_NUM(tAddr1st.dRow)%2 == 0)
		{
			HAL_CQSetRaidBaseAddr(tAddr1st.bChan,tFtlTask.tTaskCtrl->dEvenRaidAddr - 0x460000,1);
		}
		else
		{
			HAL_CQSetRaidBaseAddr(tAddr1st.bChan,tFtlTask.tTaskCtrl->dOddRaidAddr - 0x460000,1);
		}
	}
	else
	{
		HAL_CQSetRaidBaseAddr(tAddr1st.bChan,0,0);/*关闭Raid*/

        
	}
#endif

    if(tFtlTask.pwSeedIndex != NULL)
    {
        wSeedIdx = ((U16 *)tFtlTask.pwSeedIndex)[dOfstBuff];
        //FDL_Log("SeedIdx=%x",wSeedIdx);
    }

    HAL_CQSetRndSeed(tAddr1st.bChan,tAddr1st.dRow,wSeedIdx);

  	if(DISCRETE_NONE == tFtlTask.bArgsArr[0])
  	{
    	WirteStartDMA2ByCQ(tAddr1st.bChan,dOfstReal,tAddr1st.bClstOfPage,tAddr1st.bSecOfClst,bJointMode);
  	}
	else
	{
    	WirteStartDMA2ByCQ(tAddr1st.bChan,dOfstBuff,tAddr1st.bClstOfPage,tAddr1st.bSecOfClst,bJointMode);
	}

    /*****************************send program cmd*********************************/
    if(bOPMode2nd == MULTI_PLN_OP_MODE)
    {
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2_MP);        
    }
    else if(bOPMode2nd==CACEH_OP_MODE)
    {
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_CACHE_PROG2);
    }
    else
    {
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2);
    }

	HAL_CQCfgLogicNumExt(tAddr1st.bChan,bOPMode2nd);
	
    if(	(tFtlTask.dTaskType == TYPE_NCQ_WRITE) &&
        (tFtlTask.tTaskCtrl->bWriteFailCtrl == WRITE_FAIL_STOP))
    {
        HAL_CQCheckHoldOnly(tAddr1st.bChan,1);//for other chan check status fail stop cq handler
    }
	

    tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]++;
    tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]++;

    /******************chan task end, check all lun/ce status*********************/    
    //HAL_CQGenerateInt(tAddr1st.bChan, 0, FALSE);

	tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
	tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;

    return TRUE;
}




U08 TaskConvertWriteLittleDataToFlash(U08 bChanPhy)
{
	U08 i = 0;

    U08 bChipPhy,bLunOfChip;
    U08 bUserDataEn = SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN;
    SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN = 0;

	U16 wEcc_Parity         = 160;
    U32 dRowAddr,dColAddr   = 0;

    T_CHAN_TASK_QUEUE *tChTaskQ     = &tChTaskQueue[bChanPhy];
	U16 wOfst = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead& (MAX_CHAN_TASK-1)];
	T_FLASH_PHY_ADDR *ptPageAddr    = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOfst]);

    bChipPhy = CHIPES(bChanPhy,ptPageAddr->m_bLunOfChan / LUN_OF_CHIP);

	U16 wDma2Len =SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN;
	U08 bCopyNum = tFtlTask.bArgsArr[0];

#if 0
	switch (SG_DMA2_CH(bChanPhy)->rECC_CTRL.bits.LDPC_MOD)
	{
	    case 5:
	        wEcc_Parity = HAL_LDPCParityLen[0];
	        break;
	    case 7:
	    case 8:
	        wEcc_Parity = HAL_LDPCParityLen[SG_DMA2_CH(bChanPhy)->rECC_CTRL.bits.LDPC_MOD - 6];
	        break;
	    default://硬件不支持模式,死掉
	        while(1);
	}
#else
	wEcc_Parity = pFdlParam.ptUdpFdl->m_tHwReg.ADM_gParityLen;
#endif
    bLunOfChip = ptPageAddr->m_bLunOfChan % LUN_OF_CHIP;
    dRowAddr = ptPageAddr->m_dRowAddr + bLunOfChip * ROW_GAP_OF_LUN;
    
    HAL_CQSetRndSeed(bChanPhy,dRowAddr,INVALID_U16);
	HAL_CQChipSelect(bChanPhy, bChipPhy);

	HAL_CQCheckIOStatus(bChanPhy, bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_NULL);/*第一次进来不检测Pass Or Fail*/			

	FL_CMD_CQ_SLC_MODE(bChanPhy);
	HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1);
	HAL_CQSend5Addr(bChanPhy, TBLE_COL_ADDR(0), dRowAddr);

	dColAddr = 0;
	for(i = 0;i < bCopyNum;i++)
	{
		HAL_CQSendFlashCmd(bChanPhy, FL_CMD_RAND_DATA_IN);
		HAL_CQSend5Addr(bChanPhy, dColAddr, dRowAddr);
		//HAL_CQSendColAddr(bChanPhy,dColAddr);
		
		HAL_CQStartDMA2(bChanPhy,tFtlTask.dHandleBuff, DMA2_WR_TO_FLASH, 0, 0, 1, JOINT_NONE, (U32)NULL);
		dColAddr += (wDma2Len + wEcc_Parity);
		
		HAL_CQWaitDone(bChanPhy);
	}

	HAL_CQSendFlashCmd(bChanPhy,FL_CMD_PAGE_PROG2);

	HAL_CQCheckIOStatus(bChanPhy, bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_SLC_WRITE);/*SLC Pass Or Fail-Bit2*/ 	 


	HAL_CQWaitDone(bChanPhy);

    SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN = bUserDataEn;
#ifndef __FDL_CQFIFO_TRIG_IRQ__
		HAL_CQGenerateInt(bChanPhy,0,FUNC_DISABLE);
#endif

	return TRUE;
}
U08 TaskConvertOrderPrgToFlash(U08 bChanPhy)
{
	T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	U16 wOfst = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
	
	T_FLASH_PROG_ORDER_UNIT *ptOrderPrgUnit = &(((T_FLASH_PROG_ORDER_UNIT *)tFtlTask.dSrcArgsAddr)[wOfst]);
	T_PLANE_PARAM *ptPlnParam = (T_PLANE_PARAM *)tFtlTask.dDstArgsAddr;
	
	U08 bLun  		= PHYLUN_OF_LOGLUN(bChanPhy, ptOrderPrgUnit->bLunOfChan);
	U08 bChip 		= PHYCHIP_OF_LOGLUN(bChanPhy, ptOrderPrgUnit->bLunOfChan);
	U08 bMultiPlnNum;
	
	U08 bK,bJ;
	U32 dRow = 0;
	U16 wLastUnitStartOff = ptPlnParam[0].bHeadUnitCnt+ptPlnParam[0].wMidUnitCnt;//可进接口时计算为全局变量
	
	U32 dUdBuffAddr = tFtlTask.dUdBuff;
	U16 wPageOffBase = tChTaskQ->wPageOffInFtlTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
	U16 wPageOff,wBuffIdx;
	
	Joint_Mode bJointMode = JOINT_NONE;

    if(TYPE_NCQ_WRITE == tFtlTask.dTaskType)
    {
        bJointMode = JOINT_21;
    }

	if(wOfst<ptPlnParam[0].bHeadUnitCnt)
	{
		bMultiPlnNum = ptPlnParam[0].bHeadUnitOfMultiPlane;
	}
	else if(wOfst<wLastUnitStartOff)
	{
		bMultiPlnNum = ptPlnParam[0].bMidUnitOfMultiPlane;
	}
	else
	{
		bMultiPlnNum = ptPlnParam[0].bLastUnitOfMultiPlane;
	}

	HAL_CQChipSelect(bChanPhy, bChip);

	for(bJ = 0;bJ < ptOrderPrgUnit[0].bOpPageCnt;bJ++)//页循环
	{
		dRow = ptOrderPrgUnit[0].dRowAddr[bJ];
		for(bK = 0; bK < bMultiPlnNum; bK++)//Plane循环
		{
			HAL_CQCheckIOStatus(bChanPhy, bLun, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_SLC_WRITE);
			if(bK == 0)
			{
				FL_CMD_CQ_ED3_READ_WL(bChanPhy,ARRAY(dRow,3));
				FL_CMD_CQ_TLC_MODE(bChanPhy);
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1);
			}
			else
			{
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG1_MP);
			}
			
			HAL_CQSend5Addr(bChanPhy, 0, (dRow + bK * ROW_OF_BLOCK));
			HAL_CQSetRndSeed(bChanPhy, (dRow + bK * ROW_OF_BLOCK), INVALID_U16);
			
			wPageOff = wPageOffBase+bJ+bK*ptOrderPrgUnit->bOpPageCnt;
			dUdBuffAddr = tFtlTask.dUdBuff+ wPageOff * DATA_CLST_OF_PAGE * UD_LEN_OF_DATA_CLST;
			HAL_CQSetUd(bChanPhy,dUdBuffAddr ,SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);

			wBuffIdx = wPageOff*SECT_OF_PAGE;
			HAL_CQStartDMA2(bChanPhy, tFtlTask.dHandleBuff, DMA2_WR_TO_FLASH, wBuffIdx, 0, SECT_OF_PAGE, bJointMode, (U32)NULL);

			if((bK+1) < (bMultiPlnNum))
			{
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG2_MP);
			}
			else if((bK + 1) == bMultiPlnNum)
			{
				HAL_CQSendFlashCmd(bChanPhy, FL_CMD_PAGE_PROG2);
			}
		}
	}

#ifndef __FDL_CQFIFO_TRIG_IRQ__
		HAL_CQGenerateInt(bChanPhy,0,FUNC_DISABLE);
#endif
    return TRUE;
}

U08 TaskConvertOrderExtPrgToFlash(U08 bChan)
{
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChan];
	U16 wOfst = tChTaskQ->wOfstInTask[0];
	T_FLASH_PROG_ORDER_EXTEND_UNIT *ptOrderPrgUnit = &(((T_FLASH_PROG_ORDER_EXTEND_UNIT *)tFtlTask.dSrcArgsAddr)[wOfst]);


    U08 i,j;
	U08 bOpPageCnt = 0;
    U08 bChip,bLunOfChip,bUdSize,bSectOfClst,bClstOfPage;
	
    U16 wBuffIdxExt = 0;

    U32 dRowAddr[3],dLogNum;
    U32 dDmaInfoAddr;
	U32 dUDBuffAddrExt = 0;

    bChan       = CHANS(bChan);
    bChip       = ptOrderPrgUnit->bLunOfChan / LUN_OF_CHIP;
    bLunOfChip  = ptOrderPrgUnit->bLunOfChan % LUN_OF_CHIP;    
    bChip       = CHIPES(bChan,bChip);

    //calculate OpPageCnt number before ofs,for UD & BufIdx
    T_FLASH_PROG_ORDER_EXTEND_UNIT *ptOrderPrgUnitTmp = &(((T_FLASH_PROG_ORDER_EXTEND_UNIT *)tFtlTask.dSrcArgsAddr)[0]);
    for(i = 0; i < wOfst; i++)
    {
        ptOrderPrgUnitTmp = &(((T_FLASH_PROG_ORDER_EXTEND_UNIT *)tFtlTask.dSrcArgsAddr)[i]);
        bOpPageCnt += ptOrderPrgUnitTmp->bOpPageCnt;
        
        for(j = 0; j < ptOrderPrgUnit->bOpPageCnt; j++)
        {
            if(ptOrderPrgUnitTmp->bSrcType[i] == FLASH_DATA){bOpPageCnt--;}
        }
    }
    
    for(i = 0; i < ptOrderPrgUnit->bOpPageCnt; i++)
    {
        dRowAddr[i] = ptOrderPrgUnit->dRowAddr[i] + bLunOfChip * ROW_GAP_OF_LUN;
    }

    bUdSize     = UD_LEN_OF_DATA_CLST;
    bSectOfClst = SECT_OF_DATA_CLST;
    bClstOfPage = SECT_OF_PAGE / SECT_OF_DATA_CLST;
    dLogNum     = wOfst;

    HAL_CQChipSelect(bChan,bChip);

    for(i = 0; i < ptOrderPrgUnit->bOpPageCnt; i++)
    {
        HAL_CQCheckIOStatus(bChan,bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_SLC_WRITE);

        if(ptOrderPrgUnitTmp->bSrcType[i] == FLASH_DATA)//Read Data From Flash
        {
            wBuffIdxExt = bChan * SECT_OF_PAGE;
            HAL_CQCheckHold(1,JOINT_NONE, bChan, wBuffIdxExt, SECT_OF_PAGE,1);
            HAL_CQCfgLogicNum(bChan,dLogNum);            
            HAL_CQSendFlashCmd(bChan, (i+1));
            HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
            HAL_CQSend5Addr(bChan,0,dRowAddr[i]);
            HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
            HAL_CQCheckIOStatus(bChan,bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_SLC_WRITE);
            HAL_CQSendRndAddr(bChan,DATA_COL_ADDR(0),dRowAddr[i]);
            dUDBuffAddrExt = tFtlTask.dUdBuffExt+ bChan*bClstOfPage * bUdSize;
            HAL_CQSetUd(bChan,dUDBuffAddrExt,bSectOfClst,bUdSize,0); 
            HAL_CQSetRndSeed(bChan,dRowAddr[i],INVALID_U16);
            
            dDmaInfoAddr = tFtlTask.dEccStsBuff+(wOfst*SECT_OF_PAGE/SECT_OF_DMA)*2;//for retry
            HAL_CQStartDMA2(bChan, tFtlTask.dHandleBuffExt, DMA2_RD_TO_BUFF, wBuffIdxExt,0, SECT_OF_PAGE, JOINT_NONE, dDmaInfoAddr);
            HAL_CQCheckReadEnd(bChan,1);
        }
        
        if(ptOrderPrgUnit->bPrgCycle == 0)
        {
            HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PRECMD1);
        }
        else if(ptOrderPrgUnit->bPrgCycle == 1)
        {
            HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PRECMD2);
        }

        HAL_CQSendFlashCmd(bChan, (i+1));
        HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
        HAL_CQSend5Addr(bChan, DATA_COL_ADDR(0), dRowAddr[i]);
        HAL_CQCfgLogicNum(bChan,dLogNum);
        HAL_CQSetRndSeed(bChan,dRowAddr[i],INVALID_U16);

        if(ptOrderPrgUnitTmp->bSrcType[i] == FLASH_DATA)//Data From Flash,No-Joint WR
        {            
            HAL_CQSetUd(bChan, dUDBuffAddrExt, bSectOfClst, bUdSize,0);
            HAL_CQStartDMA2(bChan, tFtlTask.dHandleBuffExt, DMA2_WR_TO_FLASH,wBuffIdxExt, 0, SECT_OF_PAGE, JOINT_NONE, (U32)NULL);
        }
        else
        {            
            HAL_CQSetUd(bChan, tFtlTask.dUdBuff + (bOpPageCnt+i)*bClstOfPage*bUdSize, bSectOfClst, bUdSize,0);
            HAL_CQStartDMA2(bChan, tFtlTask.dHandleBuff, DMA2_WR_TO_FLASH,(bOpPageCnt+i)*SECT_OF_PAGE, 0, SECT_OF_PAGE, JOINT_21, (U32)NULL);
        }

        if(i == (ptOrderPrgUnit->bOpPageCnt-1))
        {
            HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);//last page ,send 0x10
        }
        else
        {
            HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PRO_CMD);//send 0x1A
        }
    }

    HAL_CQCheckIOStatus(bChan, bLunOfChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_SLC_WRITE);
#ifndef __FDL_CQFIFO_TRIG_IRQ__
		HAL_CQGenerateInt(bChan,0,FUNC_DISABLE);
#endif

    return TRUE;
}

void FDL_OrderMultiWritePage(U08 bChan, U08 bLun, U32 dRow,  U32 dBuffAddr, Joint_Mode joint, U08 bMultiNum, U08 bBuffIndex,U08 bPrgCycle, U16 wOrderNum, U08 bPageCnt, U32 dUdBuff)
{
	U08 i = 0;
	U08 bChip = bLun;
	U16 wCol = 0;
	HAL_CQChipSelect(bChan, bChip);
	for(i = 0; i < bMultiNum; i++)
	{
		HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_SLC_WRITE);
		if(i == 0)// (bMultiNum ==  1)
		{
			FL_CMD_CQ_ED3_READ_WL(bChan,ARRAY(dRow,3));
			FL_CMD_CQ_TLC_MODE(bChan);
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
		}
		else //if(bMultiNum > 1)
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1_MP);
		}
		HAL_CQSend5Addr(bChan, wCol, (dRow + i * ROW_OF_BLOCK));
		HAL_CQSetRndSeed(bChan, (dRow + i * ROW_OF_BLOCK), INVALID_U16);
		HAL_CQSetUd(bChan,(dUdBuff + i * bPageCnt * (SECT_OF_PAGE / SECT_OF_DATA_CLST) * UD_LEN_OF_DATA_CLST),SECT_OF_DATA_CLST, UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
		HAL_CQStartDMA2(bChan, dBuffAddr, DMA2_WR_TO_FLASH, (bBuffIndex + i * bPageCnt * SECT_OF_PAGE), 0, SECT_OF_PAGE, joint, (U32)NULL);
		if((i+1) < (bMultiNum))
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
		}
		else if((i + 1) == bMultiNum)
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
		}
	}
}
#ifdef _TLC_ORDER_PROGRAM_
U08	TaskConvertOrderPrgUnitSLC (U08 bChanPhy)
{

	T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	U32 dOfstReal = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
	U16 dOfstBuff = dOfstReal & (tFtlTask.wContiAddrMax - 1);//注意:tFtlTask.wContiAddrMax要是2的幂次方

	T_ADDR_INFO 	 tAddr1st;
	U32 dLogNum;
	U16 wSeedIdx			= INVALID_U16;
	U08 bOPMode2nd			= NORMAL_OP_MODE;
	U08 bLogLunOfChan;
	Joint_Mode bJointMode	= JOINT_21;
	T_FLASH_PHY_ADDR *ptPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfstBuff]);
	bLogLunOfChan = ptPageAddr->m_bLunOfChan;
	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptPageAddr,!TYPE_ERASE_BLOCKS);
	tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan];
#if (1 == DEBUG_LEVEL)
	FDL_Log("dOfstReal=%d, Row=0x%x",dOfstReal,tAddr1st.dRow);
#endif
	dLogNum = dOfstReal;
	HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
	HAL_CQCfgLogicNum(tAddr1st.bChan,dLogNum);
	HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstBuff*tAddr1st.bClstOfPage*tAddr1st.bUdSize,tAddr1st.bSecOfClst,tAddr1st.bUdSize,DMA2_WR_TO_FLASH);
	if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
	{
		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);/*11h后面也不检测Pass Or Fail*/
	}
	else
	{
		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_SLC_CACHE_WRITE);/*Cache Programma SLC Previous Pass Or FailBit*/
	}
	if(tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan] != 0)
	{
		tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]--;
	}
	if( tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > 1)/*The LUN address bit must be identical*/
	{
		U32 dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead + 1)&(MAX_CHAN_TASK-1)])&0x00ffffff;
		U08 bPlaneNumCur  = (ptPageAddr->m_dRowAddr/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*当前Row地址的Plane Num*/
		U08 bPlaneNumNext  = (dRowOpNext/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*下一个Row地址的Plane Num*/
		if( ((ptPageAddr->m_dRowAddr&(ROW_GAP_OF_PLANE - 1)) == (dRowOpNext&(ROW_GAP_OF_PLANE - 1)))&&/*page address bits must be identical*/
			((ptPageAddr->m_dRowAddr&(~PLANE_BIT_MASK_OF_ROW)) != (dRowOpNext&(~PLANE_BIT_MASK_OF_ROW))))/*plane select bits must be different*/
		{
			if(1 != ((tChTaskQ->bPlaneWriteFlag[bLogLunOfChan]>>bPlaneNumNext) & 0x01))/*避免Plane顺序为0 2 0或者0 1 2 0的情况*/
			{
				bOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
				tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] |= (1<<bPlaneNumCur);/*目前支持2Plane、4Plane、8Plane*/
			}
		}
	}
	if( (bOPMode2nd != MULTI_PLN_OP_MODE) && //不是多Plane
		((ptPageAddr->m_dRowAddr & (PAGE_OF_BLOCK_SLC - 1)) != (PAGE_OF_BLOCK_SLC - 1)))//当前不是最后一页,就发Cache命令
	{
		bOPMode2nd = CACEH_OP_MODE;//发80-15
		tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] = 0;/*每个Plane缓存区里面的数据已经被推到Flash Array,清零标记*/
	}
	tChTaskQ->bNextAddrOPMode[bLogLunOfChan] = bOPMode2nd;
	tChTaskQ->wOfstLunRecord[bLogLunOfChan]  = dOfstReal;
	if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == NORMAL_OP_MODE)
	{
		tChTaskQ->bPreLunContiOpCnt[bLogLunOfChan] = 0;
		tChTaskQ->bLunContinueOpCnt[bLogLunOfChan] = 0;
	}
	else if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == CACEH_OP_MODE)
	{
		tChTaskQ->bPreLunContiOpCnt[bLogLunOfChan] = tChTaskQ->bLunContinueOpCnt[bLogLunOfChan];
		tChTaskQ->bLunContinueOpCnt[bLogLunOfChan] = 0;
	}
	if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
	{
		HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1_MP);
	}
	else
	{
		FL_CMD_CQ_SLC_MODE(bChanPhy);
		HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1);
	}
	HAL_CQSend5Addr(tAddr1st.bChan, DATA_COL_ADDR(0), tAddr1st.dRow);
	HAL_CQSetUdBaseAddr(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstBuff*tAddr1st.bClstOfPage*tAddr1st.bUdSize,DMA2_WR_TO_FLASH);//bDir 1:read	 0:write
#ifdef __RAID_FUNCTION_ON__
	if(TYPE_NCQ_WRITE == tFtlTask.dTaskType)
	{
		if(GET_PAGE_NUM(tAddr1st.dRow)%2 == 0)
		{
			HAL_CQSetRaidBaseAddr(tAddr1st.bChan,tFtlTask.tTaskCtrl->dEvenRaidAddr - 0x460000,1);
		}
		else
		{
			HAL_CQSetRaidBaseAddr(tAddr1st.bChan,tFtlTask.tTaskCtrl->dOddRaidAddr - 0x460000,1);
		}
	}
	else
	{
		HAL_CQSetRaidBaseAddr(tAddr1st.bChan,0,0);/*关闭Raid*/
	}
#endif
	HAL_CQSetRndSeed(tAddr1st.bChan,tAddr1st.dRow,wSeedIdx);
	WirteStartDMA2ByCQ(tAddr1st.bChan,dOfstReal,tAddr1st.bClstOfPage,tAddr1st.bSecOfClst,bJointMode);
	if(bOPMode2nd == MULTI_PLN_OP_MODE)
	{
		HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2_MP);
	}
	else if(bOPMode2nd==CACEH_OP_MODE)
	{
		HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_CACHE_PROG2);
	}
	else
	{
		HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2);
	}
	HAL_CQCfgLogicNumExt(tAddr1st.bChan,bOPMode2nd);
	tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]++;
	tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]++;
	HAL_CQGenerateInt(tAddr1st.bChan, 0, FALSE);
	tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
	tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;
	return 2;
}
U08	TaskConvertOrderPrgUnitTLC (U08 bChanPhy)
{
	T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	U32 dOfstBuff = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)] & (tFtlTask.wContiAddrMax - 1);
	U16 dOfstReal = tChTaskQ->wPageOffInFtlTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
	T_ADDR_INFO 	 tAddr1st;
	U08 bLogLunOfChan;
	U08 bPageNum;
	U08 bPlane;
	U32 dRowInPlane;
	U32 dRowInWL;
	U16 wOrderAddr;
	U32 dUdBuffAddr = tFtlTask.dUdBuff;
	U16 wPage;
	U08 bBuffAddrInWL[3] = {0x31, 0x32, 0x33};
	T_FLASH_PROG_ORDER_UNIT *ptPageAddr = &(((T_FLASH_PROG_ORDER_UNIT *)tFtlTask.dSrcArgsAddr)[dOfstBuff]);
	if ((ptPageAddr->dRowAddr[0] >> 24) == 0)
	{
		U32 dLogNum;
		U08 bOPMode2nd			= NORMAL_OP_MODE;
		bLogLunOfChan = ptPageAddr->bLunOfChan;
		tAddr1st.bChan	= CHANS(ptPageAddr->bChan);
		tAddr1st.bChip	= PHYCHIP_OF_LOGLUN(tAddr1st.bChan, ptPageAddr->bLunOfChan);
		tAddr1st.bLun	= PHYLUN_OF_LOGLUN(tAddr1st.bChan, ptPageAddr->bLunOfChan);
		tAddr1st.dRow	= ptPageAddr->dRowAddr[0] + tAddr1st.bLun * ROW_GAP_OF_LUN;
		tAddr1st.wCol			= DATA_COL_ADDR(0 * DMA_OF_DATA_CLST);
		tAddr1st.bUdSize		= UD_LEN_OF_DATA_CLST;
		tAddr1st.bSecOfClst 	= SECT_OF_DATA_CLST;
		tAddr1st.bClstOfPage	= DATA_CLST_OF_PAGE;
		tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan];
		dLogNum = dOfstReal;
		HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
		HAL_CQCfgLogicNum(tAddr1st.bChan,dLogNum);
		HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstReal*tAddr1st.bClstOfPage*tAddr1st.bUdSize,tAddr1st.bSecOfClst,tAddr1st.bUdSize,DMA2_WR_TO_FLASH);
		if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);/*11h后面也不检测Pass Or Fail*/
		}
		else
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_SLC_CACHE_WRITE);/*Cache Programma SLC Previous Pass Or FailBit*/
		}
		if(tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan] != 0)
		{
			tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]--;
		}
		if( tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > 1)/*The LUN address bit must be identical*/
		{
			U32 dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead + 1)&(MAX_CHAN_TASK-1)])&0x00ffffff;
			U08 bPlaneNumCur  = (ptPageAddr->dRowAddr[0]/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*当前Row地址的Plane Num*/
			U08 bPlaneNumNext  = (dRowOpNext/ROW_GAP_OF_PLANE)&(PLANE_OF_LUN-1);/*下一个Row地址的Plane Num*/
			if( ((ptPageAddr->dRowAddr[0]&(ROW_GAP_OF_PLANE - 1)) == (dRowOpNext&(ROW_GAP_OF_PLANE - 1)))&&/*page address bits must be identical*/
				((ptPageAddr->dRowAddr[0]&(~PLANE_BIT_MASK_OF_ROW)) != (dRowOpNext&(~PLANE_BIT_MASK_OF_ROW))))/*plane select bits must be different*/
			{
				if(1 != ((tChTaskQ->bPlaneWriteFlag[bLogLunOfChan]>>bPlaneNumNext) & 0x01))/*避免Plane顺序为0 2 0或者0 1 2 0的情况*/
				{
					bOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
					tChTaskQ->bPlaneWriteFlag[bLogLunOfChan] |= (1<<bPlaneNumCur);/*目前支持2Plane、4Plane、8Plane*/
				}
			}
		}
		tChTaskQ->bNextAddrOPMode[bLogLunOfChan] = bOPMode2nd;
		tChTaskQ->wOfstLunRecord[bLogLunOfChan]  = dOfstReal;
		if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == NORMAL_OP_MODE)
		{
			tChTaskQ->bPreLunContiOpCnt[bLogLunOfChan] = 0;
			tChTaskQ->bLunContinueOpCnt[bLogLunOfChan] = 0;
		}
		if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == MULTI_PLN_OP_MODE)
		{
			HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1_MP);
		}
		else
		{
			FL_CMD_CQ_SLC_MODE(bChanPhy);
			HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1);
		}
		HAL_CQSend5Addr(tAddr1st.bChan, DATA_COL_ADDR(0), tAddr1st.dRow);
		HAL_CQSetUdBaseAddr(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstReal*tAddr1st.bClstOfPage*tAddr1st.bUdSize,DMA2_WR_TO_FLASH);//bDir 1:read	 0:write
		HAL_CQSetRndSeed(tAddr1st.bChan,tAddr1st.dRow,INVALID_U16);
		WirteStartDMA2ByCQ(tAddr1st.bChan,dOfstReal,tAddr1st.bClstOfPage,tAddr1st.bSecOfClst,JOINT_21);
		if(bOPMode2nd == MULTI_PLN_OP_MODE)
		{
			HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2_MP);
		}
		else
		{
			HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_CACHE_PROG2);
		}
		HAL_CQCfgLogicNumExt(tAddr1st.bChan,bOPMode2nd);
		tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]++;
		tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]++;
		HAL_CQGenerateInt(tAddr1st.bChan, 0, FALSE);
		tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
		tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;
		return 2;
	}
	else
	{
		bLogLunOfChan = ptPageAddr->bLunOfChan;
		bPageNum = tChTaskQ->bPageOffInOrderUnit[bLogLunOfChan];
		tAddr1st.bChan	= CHANS(ptPageAddr->bChan);
		tAddr1st.bChip	= PHYCHIP_OF_LOGLUN(tAddr1st.bChan, ptPageAddr->bLunOfChan);
		tAddr1st.bLun	= PHYLUN_OF_LOGLUN(tAddr1st.bChan, ptPageAddr->bLunOfChan);
		tAddr1st.dRow	= ptPageAddr->dRowAddr[0] + tAddr1st.bLun * ROW_GAP_OF_LUN;
		tAddr1st.wCol			= DATA_COL_ADDR(0 * DMA_OF_DATA_CLST);
		tAddr1st.bUdSize		= UD_LEN_OF_DATA_CLST;
		tAddr1st.bSecOfClst 	= SECT_OF_DATA_CLST;
		tAddr1st.bClstOfPage	= DATA_CLST_OF_PAGE;
		HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
		HAL_CQCfgLogicNum(tAddr1st.bChan,dOfstBuff);
		HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff + dOfstReal*tAddr1st.bClstOfPage*tAddr1st.bUdSize,tAddr1st.bSecOfClst,tAddr1st.bUdSize,DMA2_WR_TO_FLASH);
#if (CACHE_ORDER_WR_EN)
		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_SLC_CACHE_WRITE);/*Cache Programma SLC Previous Pass Or FailBit*/
#else
		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
#endif
		if(tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan] != 0)
		{
			tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]--;
		}
		if (ptPageAddr->bOpPageCnt == 0x03)
		{
			tFtlTask.dRowAddr[2] = ptPageAddr->dRowAddr[2];
			tFtlTask.dRowAddr[1] = ptPageAddr->dRowAddr[1];
			tFtlTask.dRowAddr[0] = ptPageAddr->dRowAddr[0];
			dOfstReal = dOfstReal + 2 * bPageNum;
		}
		else if (ptPageAddr->bOpPageCnt == 0x02)
		{
			tFtlTask.dRowAddr[2] = ptPageAddr->dRowAddr[1];
			tFtlTask.dRowAddr[1] = ptPageAddr->dRowAddr[0];
			tFtlTask.dRowAddr[0] = 0x00;
			dOfstReal = dOfstReal + 2 * (bPageNum - 1);
		}
		else if (ptPageAddr->bOpPageCnt == 0x01)
		{
			tFtlTask.dRowAddr[2] = ptPageAddr->dRowAddr[0];
			tFtlTask.dRowAddr[1] = 0x00;
			tFtlTask.dRowAddr[0] = 0x00;
		}
		tAddr1st.dRow = tFtlTask.dRowAddr[bPageNum] + tAddr1st.bLun * ROW_GAP_OF_LUN;
		dRowInWL = tAddr1st.dRow;
		for (bPlane = 0; bPlane < PLANE_OF_LUN; bPlane++)
		{
			if ((ptPageAddr->bOpPageCnt == 2) && (bPageNum == 0))
			{
				dRowInPlane = 0;
			}
			else if ((ptPageAddr->bOpPageCnt == 1) && (bPageNum != 2))
			{
				break;
			}
			else
			{
				dRowInPlane  = dRowInWL + bPlane * ROW_OF_BLOCK;
			}
			if (bPlane == 0)
			{
				FL_CMD_CQ_TLC_MODE(bChanPhy);
				HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1);
			}
			else
			{
				HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG1_MP);
			}
			HAL_CQSend5Addr(tAddr1st.bChan, DATA_COL_ADDR(0), dRowInPlane);
			if ((ptPageAddr->bOpPageCnt == 3)
				||((ptPageAddr->bOpPageCnt == 2) && (bPageNum != 0))
				|| ((ptPageAddr->bOpPageCnt == 1) && (bPageNum == 2)))
			{
				dUdBuffAddr = tFtlTask.dUdBuff + dOfstReal*tAddr1st.bClstOfPage*tAddr1st.bUdSize;
				HAL_CQSetUdBaseAddr(tAddr1st.bChan,dUdBuffAddr,DMA2_WR_TO_FLASH);//bDir 1:read	 0:write
				HAL_CQSetRndSeed(tAddr1st.bChan,dRowInPlane,INVALID_U16);
				WirteStartDMA2ByCQ(tAddr1st.bChan,dOfstReal,tAddr1st.bClstOfPage,tAddr1st.bSecOfClst,JOINT_21);
				dOfstReal = dOfstReal + 1;
			}
			if (bPlane == (PLANE_OF_LUN - 1))
			{
				HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_ED3_PAGE_PROG);
				HAL_CQSendAddr(tAddr1st.bChan, bBuffAddrInWL[bPageNum]);
			}
			else
			{
				HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2_MP);
			}
			HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);
		}
		tChTaskQ->bPageOffInOrderUnit[bLogLunOfChan] ++;
		if (bPageNum == 2)
		{
			tChTaskQ->bPageOffInOrderUnit[bLogLunOfChan] = 0;
			for (bPlane = 0; bPlane < PLANE_OF_LUN; bPlane++)
			{
				HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PROG_CONFIRM1);  //Program Order Address
				tAddr1st.dRow = tFtlTask.dRowAddr[3 - ptPageAddr->bOpPageCnt] + tAddr1st.bLun * ROW_GAP_OF_LUN;
				dRowInWL = tAddr1st.dRow;
				wPage = tAddr1st.dRow & 0x3FF;
				if (wPage < 0x08 )
				{
					wOrderAddr = wPage / 2;
				}
				else if (wPage >= 0x08 && wPage < 0x2F0)
				{
					wOrderAddr = (wPage - 0x08)/3 + 0x04;
				}
				else if ((wPage >= 0x2F0) & (wPage < 0x2F8))
				{
					wOrderAddr = (wPage - 0x2F0)/2 + 0xFC;
				}
				else if ((wPage >= 0x2F8) & (wPage < 0x300))
				{
					wOrderAddr = (wPage - 0x2F8) + 0x100;
				}
				else
				{
					FDL_Log("Error：Page Over，Page=%x",wPage);
					while(1);
				}
				dRowInPlane = (dRowInWL & 0xFFFC00) | wOrderAddr;
				dRowInPlane  = dRowInPlane + bPlane * ROW_OF_BLOCK;
				HAL_CQSend5Addr(tAddr1st.bChan, DATA_COL_ADDR(0), dRowInPlane);
				if (bPlane == (PLANE_OF_LUN - 1))
				{
#if (CACHE_ORDER_WR_EN)
					HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_CACHE_PROG2);
#else
					HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PROG_CONFIRM2);
#endif
				}
				else
				{
					HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_PAGE_PROG2_MP);
					HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_CACHE_WRITE, FIO_PF_PASS_NULL);
				}
			}
			tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]++;
			tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]++;
			tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
			tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;
		}
		HAL_CQGenerateInt(tAddr1st.bChan, 0, FALSE);
		return bPageNum;
	}
}
U08	TaskConvertOrderPrgUnit (U08 bChanPhy)
{
	U08 bPageNum;
	if (tFtlTask.bWriteMode == SLC_OPMODE)
	{
		bPageNum = TaskConvertOrderPrgUnitSLC(bChanPhy);
		return bPageNum;
	}
	else
	{
		bPageNum = TaskConvertOrderPrgUnitTLC(bChanPhy);
		return bPageNum;
	}
}
#endif
