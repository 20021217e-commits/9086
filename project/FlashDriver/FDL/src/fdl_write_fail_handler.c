#include "Fdl_write_fail_handler.h"
#include "Fdl_common.h"
#include "uitility.h"
#include "fdl_mcu_op.h"
U8_C PAGE_STAT_PRO_FAIL_IN_PLN[CACHE_PAGE_FAIL_CNT] = 
{
	LUN_STAT_PAGE_PRO_FAIL_CUR,
	LUN_STAT_PAGE_PRO_FAIL_PRE
};


U32 Fdl_GetWriteFailInfor(U8 bChanFail)
{
    U16 wFailOfts = (SG_FC(bChanFail)->rLOGIC_NUM & INVALID_U16);
    U32 dRowRcd;
    
    T_FLASH_PHY_ADDR *ptPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wFailOfts]); 
    dRowRcd = FAIL_ADDR_SET(ptPageAddr->m_bDataType,SLC_MODE,ptPageAddr->m_bChan,ptPageAddr->m_bLunOfChan,ptPageAddr->m_dRowAddr);//((U32)(ptPageAddr[0].m_bChan)<<26)|((U32)(ptPageAddr[0].m_bLunOfChan )<<24)| ptPageAddr[0].m_dRowAddr;
    return dRowRcd;
#if 0
    PRINT_HEX("Wr Fail Row=%x",dRowRcd);
    
    if(PrgFailCnt < 8)
    {
        PrgFailRow[PrgFailCnt] =  dRowRcd;
        PrgFailCnt++;
    }
#endif
}


U16 FDL_SearchOfstByBlkAddr(U8 bChan,U8 bLogLun,U16 wBlkAddr,U16 wOfst,U08 bMode)
{
    U8  bPhyChan;
    U16 wOfstTmp = wOfst;
    
    T_FLASH_PHY_BLK_ADDR *pBlockAddr = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[wOfstTmp]);
    bPhyChan = CHANS(pBlockAddr->m_bChan);
    
    if(FORWARD_SEARCH_ADDR_OFST == bMode)
    {      
        while((bPhyChan != bChan) || 
			  (pBlockAddr->m_bLunOfChan != bLogLun) ||
			  (pBlockAddr->m_wBlkAddr != wBlkAddr))
        {
            if(wOfstTmp == 0)
            {
                wOfstTmp = FAIL_SEARCH_ADDR_OFST;
                break;
            }
            
            wOfstTmp--;
            pBlockAddr = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[wOfstTmp]);
            bPhyChan = CHANS(pBlockAddr->m_bChan);
        }
    }
    else if(FORWARD_SEARCH_LUN_ADDR_OFST == bMode)
    {      
        while((bPhyChan != bChan) || 
			  (pBlockAddr->m_bLunOfChan != bLogLun))
        {
            if(wOfstTmp == 0)
            {
                wOfstTmp = FAIL_SEARCH_ADDR_OFST;
                break;
            }
            
            wOfstTmp--;
            pBlockAddr = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[wOfstTmp]);
            bPhyChan = CHANS(pBlockAddr->m_bChan);
        }
    }

    return wOfstTmp;

}

	

/**************************************************************************
 *函数名称: Fdl_MCUReadtatusData
 *函数功能: MCU方式读取Flash Status数据(默认读取4遍,取最后一遍的值)
 *入口参数:
 * @param bChanPhy 		物理通道号
 * @param bChipPhy 		物理片选号
 * @param bCmd 			Check Status命令(78h或者70h)
 * @param dRow 			Row地址,主要用来选择Plane号
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2019年09月03日(周二))
************************************************************************/
U08 Fdl_MCUReadtatusData(U08 bChanPhy,U08 bChipPhy,U08 bCmd,U32 dRow)
{
	U08 bData[4] = {0,0,0,0};
	HAL_MCU_SetCe(bChanPhy,bChipPhy,1);
	Delay(8);
	HAL_MCU_SendCmd(bChanPhy,bCmd);
	Delay(40);
	if(dRow != 0)
	{
		HAL_MCU_SendThreeAddr(bChanPhy,dRow);
	}

	//发送读脉冲
	HAL_McuSendReadPluseGetData(bChanPhy,bData,4);
	
	HAL_MCU_SendCmd(bChanPhy, FL_CMD_EXIT_CHK_IOSTA);

	return bData[3];
}

#ifdef __SLC_PRG_FAIL_HANDL__

/**************************************************************************
 *函数名称: Fdl_LastAddrModeDataHandle
 *函数功能: 发生编程Fail时,如果其他通道CQ没有处理完一个Plane组,那么此时数据还在各自Plane的Cache Reg里面,由于这些数据已经过了Raid,
 			因此这里需要想办法将这些数据搞进Flash,后面做数据恢复的时候,需要读取的(处理办法就是发一个81-10).
 *入口参数:
 * @param bChanPhy 		物理通道号
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2019年09月03日(周二))
************************************************************************/
void Fdl_LastAddrModeDataHandle(U08 bChanPhy,U32 LogicNum)
{
	U08 bLastAddrOPMode = 0;
	U16 wOffsetLogNum   = (SG_FC(bChanPhy)->rLOGIC_NUM & INVALID_U16);
	T_FLASH_PHY_ADDR *ptPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOffsetLogNum]);

	bLastAddrOPMode = LogicNum;
	if(bLastAddrOPMode == MULTI_PLN_OP_MODE)/*上次发11h,数据还在Cache里面,这次要处理一下,*/
	{
		HAL_MCU_SendCmd(bChanPhy, FL_CMD_PAGE_PROG1_MP);
		HAL_MCU_SendTwoAddr(bChanPhy,0x00);
		HAL_MCU_SendThreeAddr(bChanPhy,ptPageAddr->m_dRowAddr);
		HAL_MCU_SendCmd(bChanPhy, FL_CMD_PAGE_PROG2);
	}
}
/**************************************************************************
 *函数名称: Fdl_WriteFailHandler
 *函数功能: 编程Fail中断处理函数
 *入口参数:
 * @param bFailChanPhy 		Fail的物理通道号
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2019年09月03日(周二))
************************************************************************/
void Fdl_WriteFailHandler(U8 bFailChanPhy)
{
	U08 bChan 			= 0;
	U08 bPlane 			= 0;
	U08 bStatus 		= 0;
	U08 bChanPhy 		= 0;
	U08 bFailChan 		= 0;
	U08 bFailChip 		= 0;
	U32 dFailRowA 		= 0;
	U16 wFailOffset 	= 0;
	U16 wOffsetLogNum 	= 0;/*Fail通道记录的LogicNum*/
	T_FLASH_PHY_ADDR *ptPageAddr;	
	T_PROG_FAIL_INFO tProgFailInfo[PLANE_OF_LUN];


	/*停掉其他通道CQ*/
    U32 dDma2Tmp;
    U08 bNoChange;

    //PRINT_TXT("Wr fail Handle");
    
    if(TYPE_NCQ_WRITE == tFtlTask.dTaskType)
    {
        for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
        {
            bChanPhy = CHANS(bChan);
            dDma2Tmp = SG_DMA2_CH(bChanPhy)->rINDEX_BUF;
            bNoChange = 0;
            if(bChanPhy != bFailChanPhy)
            {
                if(CQ_MAX_IDLE_SPACE != HAL_CQGetIdleSpace(bChanPhy))
                {
                    SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN = 1;
                    while(SG_FC(bChanPhy)->rCHK_INT_ST.bits.CQ_HOLD_FLAG == 0)
                    {
                        if(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(bChanPhy))
                        {
                            break;
                        }
                        delay_ms(1);
                        if(dDma2Tmp == SG_DMA2_CH(bChanPhy)->rINDEX_BUF)
                        {
                            SG_DMA2_CH(bChanPhy)->rCTRL.bits.JOINT_MODE = 0;
                            bNoChange++;
                        }
                        else
                        {
                            bNoChange = 0;
                        }
    
                        if(bNoChange > 10)
                        {
                            //PRINT_HEX("C=",bChanPhy);
                            //PRINT_HEX("No Change Break=",dDma2Tmp);
                            break;
                        }
    
                        dDma2Tmp = SG_DMA2_CH(bChanPhy)->rINDEX_BUF;
                    }
                }           
                Fdl_LastAddrModeDataHandle(bChanPhy,HAL_CQGetLogicNumExt(bChanPhy));
            }
        }

    }
	

	U08MemSet((U08 *)tProgFailInfo,0xFF,sizeof(T_PROG_FAIL_INFO)*PLANE_OF_LUN);
	wOffsetLogNum = (SG_FC(bFailChanPhy)->rLOGIC_NUM & INVALID_U16);

	if(0)
	{
	}
	else
	{
		ptPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOffsetLogNum]);
		bFailChan = ptPageAddr->m_bChan;
		bFailChip = ptPageAddr->m_bLunOfChan;
		dFailRowA = (ptPageAddr->m_dRowAddr&PLANE_BIT_MASK_OF_ROW);
		wFailOffset = wOffsetLogNum;
		
		if(gFdlTskStatus == ST_TASK_DONE)/*Check Multi Lun Ready会重新更新LogNum*/
		{
			bFailChip = SG_FC(bFailChanPhy)->rLOGIC_NUM>>PRE_OP_CNT_IN_LOGNUM;
		}
		
		/*重新对当前Fail的Lun读状态,确认Fail的Plane*/
		HAL_MCUCheckStatus(bFailChanPhy,CHIPES(bFailChanPhy,bFailChip));/*由于是Cache编程,可能还没有TRUE Ready,需要等一下*/
		bStatus = Fdl_MCUReadtatusData(bFailChanPhy,CHIPES(bFailChanPhy,bFailChip),FL_CMD_READ_STATUS,0);
		FDL_Log("bStatus[%02d]=0x%X",wOffsetLogNum,bStatus);

		if(bStatus&(FIO_PF_SLC_CURRENT_PAGE|FIO_PF_SLC_PREVIOU_PAGE))/*检测到一个Fail,在查看是哪个Plane Fail*/
		{
			for(bPlane = 0;bPlane < PLANE_OF_LUN;bPlane++)
			{
				bStatus = Fdl_MCUReadtatusData(bFailChanPhy,CHIPES(bFailChanPhy,bFailChip),FL_CMD_READ_ENHANCE_STATUS,(dFailRowA+bPlane*ROW_GAP_OF_PLANE));
				if(bStatus&FIO_PF_SLC_CURRENT_PAGE)
				{
					tProgFailInfo[bPlane].bCurrentPageFail = 1;
				}
				
				if(bStatus&FIO_PF_SLC_PREVIOU_PAGE)
				{
					tProgFailInfo[bPlane].bPreviouPageFail = 1;
				}
			}
		}
		
/*
		搜索具体Fail的Page,搜索方法：
			基于当前传进来的CE和CH向前搜索,
			最近的一个同CE和CH的Page,认为是Current Page Fail,
			最近的第二个同CE和CH的page,认为是Previous Page Fail.
*/
		if(gFdlTskStatus == ST_TASK_DONE)/*处理最后一个Page*/
		{
			wFailOffset++;
			ptPageAddr++;
			FDL_Log("Last one = %d",wFailOffset);
		}
		
		while(1)
		{
			
			wFailOffset--;
			ptPageAddr--;
			
			if(	(bFailChan == ptPageAddr->m_bChan) &&
				(bFailChip == ptPageAddr->m_bLunOfChan))
			{
				bPlane = GET_PLANE_NUM(ptPageAddr->m_dRowAddr);
				if(tProgFailInfo[bPlane].bCurrentPageFail == 1)/*当前Page编程Fail*/
				{
					tProgFailInfo[bPlane].wCurFailOffset = wFailOffset;
					tProgFailInfo[bPlane].bCurrentPageFail = 0;

					if(HAL_ADMGetDMA1Index() > (wFailOffset*SECT_OF_PAGE + SG_ADM->rBUF_SIZE.wAll))/*Buff 数据是否被覆盖*/
					{
						tProgFailInfo[bPlane].bCurPageOverlap = WR_DATA_OVERLAP;
					}

					ExceptionWritePageToFlash(wFailOffset,ENTRANCE_CQ,tProgFailInfo[bPlane].bCurPageOverlap);
					
#if 0
					FDL_Log("Fail Row[%02d] = 0x%X[%d],BuffOverlap[DMA1=%04d][Data=%04d]=%d",\
						wFailOffset,
						ptPageAddr->m_dRowAddr,
						bPlane,
						SG_DMA1->rINDEX,
						(wFailOffset*SECT_OF_PAGE + SG_ADM->rBUF_SIZE.wAll),
						tProgFailInfo[bPlane].bCurPageOverlap);
#endif

					
				}
				else if(tProgFailInfo[bPlane].bPreviouPageFail == 1)/*前一个Page编程Fail*/
				{
					tProgFailInfo[bPlane].wPreFailOffset = wFailOffset;
					tProgFailInfo[bPlane].bPreviouPageFail = 0;

					if(HAL_ADMGetDMA1Index() > (wFailOffset*SECT_OF_PAGE + SG_ADM->rBUF_SIZE.wAll))/*Buff 数据是否被覆盖*/
					{
						tProgFailInfo[bPlane].bPrePageOverlap = WR_DATA_OVERLAP;
					}

					ExceptionWritePageToFlash(wFailOffset,ENTRANCE_CQ,tProgFailInfo[bPlane].bPrePageOverlap);

#if 0
					FDL_Log("Fail Row[%02d] = 0x%X[%d],BuffOverlap[DMA1=%04d][Data=%04d]=%d",\
						wFailOffset,
						ptPageAddr->m_dRowAddr,
						bPlane,
						SG_DMA1->rINDEX,
						(wFailOffset*SECT_OF_PAGE + SG_ADM->rBUF_SIZE.wAll),
						tProgFailInfo[bPlane].bPrePageOverlap);
#endif

				}
				else
				{
					U08 bDealCnt = 0;
					for(bPlane = 0;bPlane < PLANE_OF_LUN;bPlane++)
					{
						if(tProgFailInfo[bPlane].bCurrentPageFail == 0)
						{
							bDealCnt++;
						}
						
						if(tProgFailInfo[bPlane].bPreviouPageFail == 0)
						{
							bDealCnt++;
						}
					}
					
					if(bDealCnt == PLANE_OF_LUN*2)
					{
						break;
					}
				}
			}

			if(wFailOffset == 0)/*减到了起始,Break*/
			{
				break;
			}
		}
	}


	/*强制结束任务和所有通道CQ*/
    if(TYPE_NCQ_WRITE == tFtlTask.dTaskType)
    {
	tFtlTask.wCur = tFtlTask.wLen;
    	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
        {
    		bChanPhy = CHANS(bChan);
        	if(	(bChanPhy != bFailChanPhy) && 
			(CQ_MAX_IDLE_SPACE != HAL_CQGetIdleSpace(bChanPhy)))
        	{					
    			SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_FLAG = 1;/*暂停CQ*/ 				           
    			SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN =  0;//clear hold en
    			SG_FC(bChanPhy)->rCHK_INT_ST.bits.CQ_HOLD_FLAG = 0;//clear int infor
    		}
    		else if(bChanPhy == bFailChanPhy)
    		{
       		 	SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_FLAG = 1; 
    		}


    		/*结束软硬件上传输任务*/
    		SG_DMA2_CH(bChanPhy)->rCTRL.bits.STOP 	= 1;/*Stop DMA2*/
            __nds32__dsb();
    		SG_FC(bChanPhy)->rCQ_CTRL.bits.CQ_STOP 	= 1;/*Stop CQ*/
            __nds32__dsb();
    		SG_FC(bChanPhy)->rCQ_CTRL.bits.CQ_CLEAR = 1;/*Clear CQ*/
            __nds32__dsb();
    		SG_FC(bChanPhy)->rCHK_INT_ST.bits.CQ_STOP_ST_RB_CQ_FLAG = 0;/*clear CQ Stop Interrupt infor*/
    		SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_FLAG = 0;/*Recovery CQ*/ 
    		tChTaskQueue[bChanPhy].wTaskCnt = 0;
    	}
        SG_DMA1->rCTRL.bits.STOP = 1;/*Stop DMA1*/
    }

#if (NAND_FLASH == TOS_BICS3)
    HAL_NFResetFlash(bFailChanPhy,CHIPES(bFailChanPhy,bFailChip),FL_CMD_RESET_FLASH);/*清掉Flash的异常状态*/
    HAL_MCUCheckStatus(bFailChanPhy,CHIPES(bFailChanPhy,bFailChip));
#endif
}

#endif


void Fdl_EraseFailHandler(U8 bChanFail)
{
    U08 bFailLogLun;
    U08 bTmp;
    U08 bFailBlkCnt;
    
    U16 wFailOftsTmp;
    U16 wFailBlkAddr; 
    U16 bFailBlkAddrTmp;
	//U08 bStatus = 0;
	//U08 bErasePF = FIO_PF_PASS_SLC_ERASE;

    
	U16 wOfst = (SG_FC(bChanFail)->rLOGIC_NUM & INVALID_U16);
    T_FLASH_PHY_BLK_ADDR *pBlockAddr = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[wOfst]);
    
    bFailLogLun  = pBlockAddr->m_bLunOfChan;    
    bFailBlkCnt  = SG_FC(bChanFail)->rLOGIC_NUM>>16;
    wFailBlkAddr = pBlockAddr->m_wBlkAddr;
#if 0  //不再次chk，直接认为是有问题的，不然像0x80这种超时状态没有检测出来，反正ftl有重擦机制
	bStatus = Fdl_MCUReadtatusData(bChanFail,PHYCHIP_OF_LOGLUN(bChanFail, bFailLogLun),FL_CMD_READ_STATUS,0);

	if(tFtlTask.bArgsArr[0] == TLC_MODE)/*TLC 模式Erase,改变PF Bit*/
	{
		bErasePF = FIO_PF_PASS_TLC_ERASE;
	}


    if(!(bStatus&bErasePF))//用MCU Chk pass则不记录
    {
//        PRINT_HEX("Mcu_Chk0:",bStatus);
        return;
    }
#endif
//    PRINT_HEX("Mcu_Chk1:",bStatus);
    for(bTmp = 0;bTmp<bFailBlkCnt;bTmp++)
    {
		bFailBlkAddrTmp = wFailBlkAddr - bTmp;	
        wFailOftsTmp = FDL_SearchOfstByBlkAddr(bChanFail,bFailLogLun,bFailBlkAddrTmp,wOfst,FORWARD_SEARCH_ADDR_OFST);          
        //PRINT4("fdl fail c%xb,ofst=%xw,Cnt=%xb,Blk=%xw\r\n",bChanFail,wOfst,wFailOftsTmp,wFailBlkAddr);
        if(wFailOftsTmp != FAIL_SEARCH_ADDR_OFST)
        {
            tFtlTask.FTLTaskExceptionHandler(wFailOftsTmp, ENTRANCE_CQ, ERS_FAIL);
        }
    }
}






#if 0
void Debug_RdSlcPage(U32 dRdBuffAddr,T_FLASH_PHY_ADDR const *ptPageAddr,U32 dUdBuffAddr)
{
    T_FLASH_PHY_ADDR *ptOpBlkPageArr;
	T_FLASH_PHY_ADDR tOpClustPageArr[DATA_CLST_OF_PAGE];
	U32 dSpcStackQueSize[1];
	U08 bClstIdx;
	
	ptOpBlkPageArr = tOpClustPageArr;

	for(bClstIdx = 0;bClstIdx < DATA_CLST_OF_PAGE;bClstIdx++)
	{
		U08MemCopy((U08 *)(&(ptOpBlkPageArr[bClstIdx])),(U08 *)(ptPageAddr),sizeof(T_FLASH_PHY_ADDR));
		ptOpBlkPageArr[bClstIdx].m_bClstOffs = bClstIdx;
	}
    
	FDL_TASK_CTRL tTaskCtrl={0};
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
	
	FDL_ReadClstToBuff(&tTaskCtrl, dRdBuffAddr, ptOpBlkPageArr, DATA_CLST_OF_PAGE, dUdBuffAddr, NULL);
    FDL_WaitTaskDone();

}



void Fdl_ProgFailRecovery(void)
{


 	T_WRITE_EXCEPTION *tWriteExSrc = (T_WRITE_EXCEPTION *)tFtlTask.tTaskCtrl->dWriteExBuffAddr;
    T_ERR_INFO *tInfo = (T_ERR_INFO *)(tFtlTask.tTaskCtrl->dWriteExBuffAddr + sizeof(T_WRITE_EXCEPTION));
    U16 wLen;
    U16 wFirstFailOffset = INVALID_U16;
    U08 bChan;
    U16 wFailOffSet;

    U08 bJ,bK;
    U16 wIndexTmp;
    T_FLASH_PHY_ADDR ptOpPageAddr;
    T_FLASH_PHY_ADDR *ptOpPageAddrBackUp;
    U16 wRealFailLen;
    U16 wRealOverlap;

    U32 dBuffTmp;
    U16 wOverLapOffset[2] = {INVALID_U16,INVALID_U16};

    U32 dVitPageOff,dBackUpOff;
    U32 dPageIdx;

    U32 dOpWrBuff;
    U32 dWrUdBuff;//缓存的ud的buff
    U32 dLastPage;//当前dma1传输的最后一个页
    U08 bClstIdx;
    U32 dRaidBuff;
    U32 dDma2Idx;
    U08 bPrgFailFlag;
    T_FLASH_PHY_ADDR ptBackUpPageAddr;//备份地址
    T_FLASH_PHY_BLK_ADDR ptBackUpBlkAddr;
    T_WRITE_EXCEPTION tWriteExBackUp; //得先把头信息保存起来，否则调用fdl接口可能清掉了
    T_WRITE_EXCEPTION *tWriteEx = (T_WRITE_EXCEPTION *)(&tWriteExBackUp);
    U32 dCacheBuff;
	U16 wPageEnd[4];
	U16 wPageStart;
	T_FLASH_PHY_ADDR *ptBasePageAddr = (T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr;
	U32 *pUd = (U32 *)tFtlTask.dUdBuff;

	wPageStart = GET_PAGE_NUM(ptBasePageAddr->m_dRowAddr);
	
    
    PRINT_HEX("Dma1=",SG_DMA1->rINDEX);
    
    //获取各个chan最后写的页的偏移
    for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
    {	
        dDma2Idx = SG_DMA2_CH(CHANS(bChan))->rINDEX_BUF;
        if(dDma2Idx >= DSECT_OF_PAGE)
        {
            wPageEnd[bChan] = (dDma2Idx - DSECT_OF_PAGE)/DSECT_OF_PAGE;
        }
        else
        {
            wPageEnd[bChan] = 0;
        }
        PRINT3("prgend c=%xb,page=%xw,dma2=%xd\r\n",bChan,g_tRaidCtrl.wPageEnd[bChan],dDma2Idx);
    }
	
    tWriteEx->bDescript = tWriteExSrc->bDescript;
    tWriteEx->bOverlapCnt = tWriteExSrc->bOverlapCnt;
    tWriteEx->wExLen  = tWriteExSrc->wExLen;
    tWriteEx->dHostDateLen = tWriteExSrc->dHostDateLen;
    
    wRealFailLen = tWriteEx->wExLen;
    wRealOverlap = tWriteEx->bOverlapCnt;


  	if(tWriteEx->bOverlapCnt > 2)//如果出错且被覆盖数据的页超过2个页恢复不了!!
    {
        PRINT_HEX("Err ov=",tWriteEx->bOverlapCnt);
        return;
    }
    else if(tWriteEx->bOverlapCnt == 2)//如果出错且被覆盖数据的页同为奇数页或者偶数页，处理不了
    {
        if((GET_PAGE_NUM(ptBasePageAddr[tInfo[0].wIndex].m_dRowAddr)%2) ==
            (GET_PAGE_NUM(ptBasePageAddr[tInfo[1].wIndex].m_dRowAddr)%2))
        {
            PRINT_HEX("Err ov2=",tWriteEx->bOverlapCnt);
            return;
        }
    }

   //记录出错地址在地址buff上的偏移
    for (bJ = 0;bJ < tWriteEx->bOverlapCnt;bJ++)//最多两个覆盖
    {
        wOverLapOffset[bJ] = tInfo[bJ].wIndex;
        PRINT_HEX("Fail Ud=",pUd[wOverLapOffset[bJ]*DATA_CLST_OF_PAGE]);
    }
	
	//3、对中间出错的页进行raid恢复：将参与raid运算的地址依次读出写入过raid
    dVitPageOff = wPageStart;
    dPageIdx = 0;


	//不管有没有覆盖,发生了编程Fail,都要将编程fail后面数据进行备份，只是有覆盖的需要进行恢复
    //当次参入raid的页地址都要先读出来再启动dma2写,只需要保证过raid,不需要实际写入Flash
    //如果有1000个页，则需要读1000个页？
    PRINT_TXT("----start raid----");
    if(tWriteEx->bOverlapCnt > 0)
    {
        for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
        {   
			for(bJ = 0;bJ < wPageEnd[bChan];bJ++)
			{
				Debug_RdSlcPage(dBuffTmp,&ptOpPageAddr,NULL);//从物理地址读出数据
			}

			 U32 *pdRaidVal = (U32 *)(dBuffTmp);
             if (SLC_PAGE_NUM(ptOpPageAddr.m_dRowAddr)%2 == 0)
             {
                 dRaidBuff = tFDLTaskCtrl.dEvenRaidAddr;
             }
             else
             {
                 dRaidBuff = tFDLTaskCtrl.dOddRaidAddr;
             }

//           Fdl_VirtPrgPage(ptOpPageAddr.m_bChan, dBuffTmp,dUdBuff+dPageIdx*sizeof(USER_DATA_LEN_PER_CLST)*UDP_bDAT_CLST_OF_PAGE,dRaidBuff);//启动dma2进行虚拟编程,开启raid
             FDL_WaitTaskDone();					                        
 		}

            dPageIdx++;
            dVitPageOff++;
        }
    }


}
#else
#endif



