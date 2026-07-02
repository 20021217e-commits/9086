
#include "fdl_task_op.h"
#include "hal.h"
#include "fdl_debug.h"
#include "flash_driver_struct.h"
#include "fdl_copyback.h"
#include "Fdl_common.h"
#include "fdl_read.h"
__align(8) U08 HAL_JCBufff[JC_BUFF_SIZE];

__align(8) T_CHAN_TASK_QUEUE	tChTaskQueue[MAX_CHAN_COUNT];//通道任务队列
__align(8) T_FDL_PARAM			pFdlParam;
__align(8) T_FTL_TASK			tFtlTask;

volatile U08 gFdlTskStatus = ST_TASK_DONE;

#ifdef 	__FDL_CQFIFO_TRIG_IRQ__
void ChanTaskErgodic(U08 bChanPhy)
{    
	//while(1)
	{
		if (tChTaskQueue[bChanPhy].wTaskCnt != 0)
		{
			if (HAL_CQGetIdleSpace(bChanPhy) < tFtlTask.bNeedCQCount)
			{
				SG_FC(bChanPhy)->rCQ_FIFO_THROD.bits.dVAL = tFtlTask.bNeedCQCount;
				SG_FC(bChanPhy)->rCQ_CTRL.bits.CQ_FIFO_RDY_INT_EN = 1;
				return;
			}
			tFtlTask.FTLTaskConvert(bChanPhy);
			if( (tFtlTask.dTaskType == TYPE_NCQ_READ) || 		/*联动读支持Page启动*/
				((tFtlTask.dTaskType == TYPE_READ_TO_BUFFER) &	/*非联动读支持Page启动*/
				(DISCRETE_CLST != tFtlTask.bArgsArr[0])))		/*离散读不能Page启动*/
			{
				tChTaskQueue[bChanPhy].wTaskHead += tChTaskQueue[bChanPhy].bClstCntInPage;
				tChTaskQueue[bChanPhy].wTaskCnt -= tChTaskQueue[bChanPhy].bClstCntInPage;
			}
			else
			{
				tChTaskQueue[bChanPhy].wTaskHead++;
				tChTaskQueue[bChanPhy].wTaskCnt -= 1;
			}

			if (HAL_CQGetIdleSpace(bChanPhy) < 512)
			{
				SG_FC(bChanPhy)->rCQ_FIFO_THROD.bits.dVAL = tFtlTask.bNeedCQCount;
			}
			else
			{
				SG_FC(bChanPhy)->rCQ_FIFO_THROD.bits.dVAL = 1000;
			}
			SG_FC(bChanPhy)->rCQ_CTRL.bits.CQ_FIFO_RDY_INT_EN = 1;
		}
	}
}
#else
void ChanTaskErgodic(U08 bChanPhy)
{    
	//FDL_Log("Enter <%s>",__func__);
#if 0
	while(1)
	{
	if (tChTaskQueue[bChanPhy].wTaskCnt == 0)
    {
		if(tFtlTask.wCur < tFtlTask.wLen)
		{
			HAL_CQWaitIdleSpace(bChanPhy, 1);
			HAL_CQGenerateInt(bChanPhy,0,FUNC_DISABLE);
		}
        return;
    }

    if (HAL_CQGetIdleSpace(bChanPhy) < tFtlTask.bNeedCQCount)
    {
		HAL_CQWaitIdleSpace(bChanPhy, 1);
		HAL_CQGenerateInt(bChanPhy,0,FUNC_DISABLE);
        return;
    }

    tFtlTask.FTLTaskConvert(bChanPhy);

	if( (tFtlTask.dTaskType == TYPE_NCQ_READ) || 		/*联动读支持Page启动*/
		((tFtlTask.dTaskType == TYPE_READ_TO_BUFFER) &	/*非联动读支持Page启动*/
		(DISCRETE_CLST != tFtlTask.bArgsArr[0])))		/*离散读不能Page启动*/
	{
		tChTaskQueue[bChanPhy].wTaskHead += tChTaskQueue[bChanPhy].bClstCntInPage;
		tChTaskQueue[bChanPhy].wTaskCnt -= tChTaskQueue[bChanPhy].bClstCntInPage;
	}
	else
	{
		tChTaskQueue[bChanPhy].wTaskHead++;
		tChTaskQueue[bChanPhy].wTaskCnt -= 1;
	}
	}
#else
	if (tChTaskQueue[bChanPhy].wTaskCnt == 0)
	{
		return;
	}

	if (HAL_CQGetIdleSpace(bChanPhy) < tFtlTask.bNeedCQCount)
	{
		return;
	}

	tFtlTask.FTLTaskConvert(bChanPhy);

	if( (tFtlTask.dTaskType == TYPE_NCQ_READ) || 		/*联动读支持Page启动*/
		((tFtlTask.dTaskType == TYPE_READ_TO_BUFFER) &	/*非联动读支持Page启动*/
		(DISCRETE_CLST != tFtlTask.bArgsArr[0])))		/*离散读不能Page启动*/
	{
		tChTaskQueue[bChanPhy].wTaskHead += tChTaskQueue[bChanPhy].bClstCntInPage;
		tChTaskQueue[bChanPhy].wTaskCnt -= tChTaskQueue[bChanPhy].bClstCntInPage;
	}
	else
	{
		tChTaskQueue[bChanPhy].wTaskHead++;
		tChTaskQueue[bChanPhy].wTaskCnt -= 1;
	}

	//if (tChTaskQueue[bChanPhy].wTaskCnt != 0)
	{
		HAL_CQGenerateInt(bChanPhy,0,FUNC_DISABLE);
	}
#endif
	//FDL_Log("Exsit <%s> bChan = %x tskcnt = %x",__func__,bChan,tChTaskQueue[bChan].wTaskCnt);
}
#endif

#ifdef _TLC_ORDER_PROGRAM_
void ChanTaskOrderErgodic(U08 bChanPhy)
{
	if (tChTaskQueue[bChanPhy].wTaskCnt == 0)
	{
		return;
	}
	U08 bNeedCQ;
	if (tFtlTask.bWriteMode == SLC_OPMODE)
	{
		bNeedCQ = tFtlTask.bNeedCQCount;
	}
	else
	{
		bNeedCQ = tFtlTask.bTlcNeedCQCount;
	}
	if (HAL_CQGetIdleSpace(bChanPhy) < bNeedCQ)
	{
		return;
	}
	U08 bPageNum;
	if (tFtlTask.bWriteMode == SLC_OPMODE)
	{
		bPageNum = TaskConvertOrderPrgUnitSLC(bChanPhy);
	}
	else
	{
		bPageNum = TaskConvertOrderPrgUnitTLC(bChanPhy);
	}
	if( bPageNum == 2)
	{
		tChTaskQueue[bChanPhy].wTaskHead++;
		tChTaskQueue[bChanPhy].wTaskCnt -= 1;
	}
}
#endif

void OrderChanTaskErgodic(U08 bChanPhy)
{
	if (tChTaskQueue[bChanPhy].wTaskCnt == 0)
    {
        return;
    }

    if (HAL_CQGetIdleSpace(bChanPhy) < tFtlTask.bNeedCQCount)
    {
        return;
    }

    tFtlTask.FTLTaskConvert(bChanPhy);//TaskConvertReadClstToBuff
	tChTaskQueue[bChanPhy].wTaskHead++;
	tChTaskQueue[bChanPhy].wTaskCnt--;
}


#ifdef __VER_TLC_PAGE__
#if (NAND_FLASH == MIC_M16)
U32 M16_ConvertTlcRow2QlcRow(U32 dRow)
{
	U32 dPageNum 	= GET_PAGE_NUM(dRow);
	U32 dBlockNum 	= GET_BLOCK_NUM(dRow);

    if(dPageNum < 120)
    {
        dPageNum = dPageNum;
    }
    else if(dPageNum <= 2315)
    {
        dPageNum += (dPageNum - (120 + dPageNum%3))/3 + 1;
    }
    else
    {
        dPageNum = 3047 + (dPageNum - 2316)*2;
    }

    return (VER_TLC_GET_ROW(dPageNum,dBlockNum,TLC_PRE_CMD));
}
#endif

U08 TaskErgodicVerifyTLCPage(U08 bTaskErgo)
{
	U08 bLun 		= 0;
	U08 bChip 		= 0;
	U08 bChan 		= 0;
	U08 bPlane 		= 0;
	U08 bChanPhy 	= 0;
	U32 dRowAddr 	= 0;
	
	T_CHAN_TASK_QUEUE *pTaskQ = (T_CHAN_TASK_QUEUE *)(&tChTaskQueue[0]);
	T_BLK_INFO_PER_LUN *ptBlockInfo;

    //遍历任务
    while (tFtlTask.wCur < tFtlTask.wLen)
    {
		for(bLun = 0;bLun < tFtlTask.wLen;bLun++)/*每次都遍历一遍LunCnt*/
		{
			ptBlockInfo = &((T_BLK_INFO_PER_LUN *)tFtlTask.dSrcArgsAddr)[bLun];
			
			if(ptBlockInfo->wPageCnt > 0)/*先看看读写够了没*/
			{
				bChan = ptBlockInfo->bChan;
				bChanPhy = CHANS(bChan);

				pTaskQ = &tChTaskQueue[bChanPhy];
				
				if((pTaskQ->wTaskTail - pTaskQ->wTaskHead) < MAX_CHAN_TASK - VER_TLC_TASK_GAP)/*检查空间够不够*/
				{					
#if 1				/*查找看看，有没有一个空间更够的LunTask先装满*/
					U08 bLunRowCnt = pTaskQ->tLunTask[ptBlockInfo->bLunOfChan].bRowCnt;

					for(bChip = 0;bChip < CHIP_OF_CHAN;bChip++)
					{
						if(	(pTaskQ->tLunTask[bChip].bRowCnt < bLunRowCnt)&&/*找到一个并且*/
							(VER_TLC_PAGE_REMAIN(bChanPhy,bChip) > 0))/*还有任务*/
						{
							bLunRowCnt = pTaskQ->tLunTask[bChip].bRowCnt;
						}
					}
					
					if(bLunRowCnt != pTaskQ->tLunTask[ptBlockInfo->bLunOfChan].bRowCnt)
					{
						continue;/*找到了一个空间更够的LunTask先装满*/
					}
#endif

					bChip = ptBlockInfo->bLunOfChan;

					/*第一次进来,记录每个Lun的一些信息*/
					if(pTaskQ->tLunTask[bChip].wLunTail == 0)
					{
#if 0
//						PRINT("pAddr000:| Chan | LunOfChan | wBlkStart | wPageOffSet | wPageCnt  |\n");
						PRINT("pAddr:|....0x%02x....|..0x%02x..|.....0x%04X....|....0x%04X....|.0x%04X.|\n",\
								ptBlockInfo->bChan,\
								ptBlockInfo->bLunOfChan,\
								ptBlockInfo->wBlkStart,\
								ptBlockInfo->wPageOffSet,\
								ptBlockInfo->wPageCnt);
#endif
						VER_TLC_LUN_TASK_OFSET(bChanPhy,bChip)	= bLun;/*记录该Lun在T_BLK_INFO_PER_LUN Buff中的Ofset*/
						/*需要占用DMAInfo Buff的个数(2Byte),空间复用*/
						VER_TLC_ECC_INFO_OFSEX(bChanPhy,bChip)  = (bChan*CHIP_OF_CHAN + bChip)*PLANE_OF_LUN*VER_TLC_DMA_CNT_PAGE*2;
						ptBlockInfo->wPageCnt = ptBlockInfo->wPageCnt>>(PLANE_OF_LUN>>1);/*FTL 会按照真实的页个数给过来,需要转换到Plane组中*/											
					}
					
					for(bPlane = 0;bPlane < PLANE_OF_LUN;bPlane++)
					{
#ifdef __FILL_SLC_BLOCKS__
						if(TYPE_FILL_BLOCKS == tFtlTask.dTaskType)
						{
							dRowAddr	= GET_ROW_ADDR_SLC(ptBlockInfo->wPageOffSet,ptBlockInfo->wBlkStart + bPlane);
						}
						else
#endif
						{
							dRowAddr	= VER_TLC_GET_ROW(ptBlockInfo->wPageOffSet/TLC_PRE_CMD,ptBlockInfo->wBlkStart + bPlane,ptBlockInfo->wPageOffSet%TLC_PRE_CMD + 1);

#if (NAND_FLASH == MIC_M16)
							dRowAddr = M16_ConvertTlcRow2QlcRow(dRowAddr);
#endif
						}

#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
						dRowAddr = Fdl_GetBadMapAddr(dRowAddr,bChanPhy,bChip);
#endif
						pTaskQ->tLunTask[bChip].dRow[pTaskQ->tLunTask[bChip].wLunTail & (MAX_CHAN_TASK-1)] = dRowAddr;
						
						U32 wOfstInTask = (bChip<<VER_TLC_CE_BIT) + pTaskQ->wTaskTail;
						wOfstInTask += ((U32)((U32)pTaskQ->tLunTask[bChip].wLunTail)<<VER_TLC_LUN_TASK_BIT);/*这里会溢出,不能超过->4Plane:4096page,2Plane:8192Page*/
						pTaskQ->wOfstInTask[pTaskQ->wTaskTail&(MAX_CHAN_TASK-1)] = wOfstInTask;
						
						pTaskQ->wTaskCnt++;
						pTaskQ->wTaskTail++;
						pTaskQ->tLunTask[bChip].bRowCnt++;
						pTaskQ->tLunTask[bChip].wLunTail++;
					}
					
					ptBlockInfo->wPageOffSet++;/*页偏移加加*/
					ptBlockInfo->wPageCnt--;/*页个数减减*/
					VER_TLC_PAGE_REMAIN(bChanPhy,bChip) = ptBlockInfo->wPageCnt;/*记录该Lun还剩余多少个页,空间复用*/					
					
					if(ptBlockInfo->wPageCnt == 0)
					{
						tFtlTask.wCur++;/*标识某个Lun已经分配完了*/
					}
				}
			}
		}
		
		if(pTaskQ->wTaskTail >= MAX_CHAN_TASK + pTaskQ->wTaskHead - VER_TLC_TASK_GAP)
		{
			break;//full
		}
    }


	/*最后都遍历一下通道队列*/
	for(bChan = 0; bChan < MAX_CHAN_COUNT; bChan++)
	{
		bChanPhy = CHANS(bChan);//获取物理通道号
		if(INVALID_U8 != bChanPhy)
		{
			ChanTaskErgodic(bChanPhy);
		}

	}

	return TRUE;

}


#endif



U08 TaskErgodic(U08 bTaskErgo)
{

    U08 i = 0;
	U08 bRet=0;
    U16 wCount = LUN_OF_DEV*PLANE_OF_LUN*SECT_OF_DATA_CLST+MAX_CHAN_COUNT; //72; //ensure lun task get the lun address

    //FDL_Log("Enter <%s> TaskErgo = %x",__func__,bTaskErgo);
    //FDL_Log("wCount = %x"wCount);
	//FDL_Log("wCur = %x,wLen = %x",tFtlTask.wCur,tFtlTask.wLen);

    //遍历任务
    while (tFtlTask.wCur < tFtlTask.wLen)
    {
        bRet = tFtlTask.FTLTaskAllocate(tFtlTask.wCur);

		if(!bRet)
		{
			break;//channal task fifo full, can't add task
		}

        tFtlTask.wCur++;

        if (!(--wCount))
        {
            break;
        }
    }

	//最后都遍历一下通道队列
    for (i = 0; i < MAX_CHAN_COUNT; i++)
    {
    	U08 bChanPhy = CHANS(i);//获取物理通道号
		if(0xFF != bChanPhy)
		{
        	ChanTaskErgodic(bChanPhy);
		}
    }
    //FDL_Log("Exsit <%s>",__func__);
    return TRUE;
}

#ifdef __FDL_HW_VIRTUL_CLST__
U08 TaskErgodicRead(U08 bTaskErgo)
{
	U08 bLogLun;
	U08 bLogChan;
	U08 bPhyChan;
	T_FLASH_PHY_ADDR *pAddr;
	T_CHAN_TASK_QUEUE *pTaskQ;

    //遍历任务
    while (tFtlTask.wCur < tFtlTask.wLen)
    {
		pAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[tFtlTask.wCur & (tFtlTask.wContiAddrMax - 1)]);
		bLogChan = pAddr->m_bChan;
		bLogLun = pAddr->m_bLunOfChan;

		if(INVALID_U8 == bLogChan)
		{
			if(tFtlTask.wCur == 0)
			{
				bLogChan = 0;
				bLogLun = 0;
			}
			else
			{
				bLogChan = tFtlTask.bCurFFChan;
				bLogLun = tFtlTask.bCurFFChanLun;
			}
			pAddr->m_dRowAddr = 0xFFFFFFFF;
			pAddr->m_bClstOffs = bLogLun;
		}
		
		tFtlTask.bCurFFChan = bLogChan;													
		tFtlTask.bCurFFChanLun = bLogLun;													

		bPhyChan = CHANS(bLogChan);
		pTaskQ = &tChTaskQueue[bPhyChan];
		if(pTaskQ->wTaskTail >= (pTaskQ->wTaskHead + MAX_CHAN_TASK))
		{
			break;//full
		}
		pTaskQ->wOfstInTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
		pTaskQ->wTaskCnt++;
		pTaskQ->wTaskTail++;
		pTaskQ->tLunTask[bLogLun].dRow[pTaskQ->tLunTask[bLogLun].wLunTail & (MAX_CHAN_TASK-1)] = pAddr->m_dRowAddr;
		pTaskQ->tLunTask[bLogLun].bClstOff[pTaskQ->tLunTask[bLogLun].wLunTail & (MAX_CHAN_TASK-1)] = pAddr->m_bClstOffs;
		pTaskQ->tLunTask[bLogLun].wOfstInTask[pTaskQ->tLunTask[bLogLun].wLunTail& (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
		pTaskQ->tLunTask[bLogLun].bRowCnt++;
		pTaskQ->tLunTask[bLogLun].wLunTail++;
        tFtlTask.wCur++;
		if((TASK_INIT == bTaskErgo) && (tFtlTask.wFirstErgodicCnt == tFtlTask.wCur))
		{
			break;
		}
    }
    U08 i;
	for (i = 0; i < CHAN_MAX_CNT; i++)
	{
		ChanTaskErgodic(CHANS(i));
	}
    return TRUE;
}
#endif
U08 TaskErgodicReadWrite(U08 bTaskErgo)
{
	U08 bLogLun;
	U08 bLogChan;
	U08 bPhyChan;
	T_FLASH_PHY_ADDR *pAddr;
	T_CHAN_TASK_QUEUE *pTaskQ;
    while (tFtlTask.wCur < tFtlTask.wLen)
    {
		pAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[tFtlTask.wCur & (tFtlTask.wContiAddrMax - 1)]);
		bLogChan = pAddr->m_bChan;
		bLogLun = pAddr->m_bLunOfChan;
		if(INVALID_U8 == bLogChan)
		{
			tFtlTask.wCur++;//!!!!!!!!
			continue;//逻辑通道号为0xFF,跳过
		}
		bPhyChan = CHANS(bLogChan);
		pTaskQ = &tChTaskQueue[bPhyChan];
		
		if(bPhyChan < MAX_CHAN_COUNT)
		{
			if(pTaskQ->wTaskTail >= (pTaskQ->wTaskHead + MAX_CHAN_TASK))
			{
				break;//full
			}
			
			pTaskQ->wOfstInTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
			pTaskQ->wTaskCnt++;
			pTaskQ->wTaskTail++;
			
			pTaskQ->tLunTask[bLogLun].dRow[pTaskQ->tLunTask[bLogLun].wLunTail & (MAX_CHAN_TASK-1)] = pAddr->m_dRowAddr;
			
			pTaskQ->tLunTask[bLogLun].bClstOff[pTaskQ->tLunTask[bLogLun].wLunTail & (MAX_CHAN_TASK-1)] = pAddr->m_bClstOffs;
			pTaskQ->tLunTask[bLogLun].wOfstInTask[pTaskQ->tLunTask[bLogLun].wLunTail& (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
			
			pTaskQ->tLunTask[bLogLun].bRowCnt++;
			pTaskQ->tLunTask[bLogLun].wLunTail++;
			
			//FDL_Log("ch=%x,lun=%x,hd=%x,tl=%x,ct=%x",pAddr->m_bChan,bLun,pTaskQ->tLunTask[bLun].wLunHead,pTaskQ->tLunTask[bLun].wLunTail,pTaskQ->tLunTask[bLun].bRowCnt);
		}
		else
		{
			FDL_Log("Error Chan Num!-bPhyChan:%d,bLogChan:%d,pAddr=0x%X",bPhyChan,bLogChan,pAddr);//虚拟通道
		}

        tFtlTask.wCur++;

		if((TASK_INIT == bTaskErgo) && (tFtlTask.wFirstErgodicCnt == tFtlTask.wCur))
		{
			break;
		}
    }
	
	//最后都遍历一下通道队列
	U08 i;
	U08 bChanPhy;
	for (i = 0; i < CHAN_MAX_CNT; i++)
	{
		bChanPhy = CHANS(i);
		Task_MultiCEForContinue(bChanPhy,bTaskErgo);/*解决XXX问题(Continue强制标记支持多CE)*/
		if(TASK_INIT == bTaskErgo)
		{	/*解决一个簇情况下，预发扔太多CQ导致return问题*/
			while(HAL_CQGetIdleSpace(bChanPhy) < (tFtlTask.bNeedCQCount));/*CQ FIFO检测*/
		}
		ChanTaskErgodic(bChanPhy);
	}

    return TRUE;
}


U08 OrderTaskErgodic(U08 bTaskErgo)
{
	U08 bLogChan,bPhyChan;
    U16 wCount = LUN_OF_DEV+MAX_CHAN_COUNT; //ensure lun task get the lun address    
	T_FLASH_PROG_ORDER_UNIT *ptOrderUnit;
	T_CHAN_TASK_QUEUE *ptChTaskQ;
	T_PLANE_PARAM *ptPlnParam = (T_PLANE_PARAM *)tFtlTask.dDstArgsAddr;
	U16 wLastUnitStartOff = ptPlnParam[0].bHeadUnitCnt+ptPlnParam[0].wMidUnitCnt;//可进接口时计算为全局变量
	U08 bMultiPlnNum;
	
    //遍历任务
    while (tFtlTask.wCur < tFtlTask.wLen)
    {
    	ptOrderUnit = &(((T_FLASH_PROG_ORDER_UNIT *)tFtlTask.dSrcArgsAddr)[tFtlTask.wCur]);
	    bLogChan = ptOrderUnit->bChan;
		
		if(tFtlTask.wCur<ptPlnParam[0].bHeadUnitCnt)
		{
			bMultiPlnNum = ptPlnParam[0].bHeadUnitOfMultiPlane;
		}
		else if(tFtlTask.wCur<wLastUnitStartOff)
		{
			bMultiPlnNum = ptPlnParam[0].bMidUnitOfMultiPlane;
		}
		else
		{
			bMultiPlnNum = ptPlnParam[0].bLastUnitOfMultiPlane;
		}
		
		if(bLogChan < MAX_CHAN_COUNT)
		{
			bPhyChan = CHANS(bLogChan);
		
		    if(bPhyChan < MAX_CHAN_COUNT)
		    {
		        ptChTaskQ = &tChTaskQueue[bPhyChan];

				if(ptChTaskQ->wTaskTail >= (ptChTaskQ->wTaskHead + MAX_CHAN_TASK))//通道队列分配完了
				{
					break;//full
				}
				
				ptChTaskQ->wOfstInTask[ptChTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
				ptChTaskQ->wPageOffInFtlTask[ptChTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wPageOff;
				ptChTaskQ->wTaskCnt++;
				ptChTaskQ->wTaskTail++;
		        
		    }
			else
			{
				FDL_Err("Err Phy Chan=%d",bPhyChan);
			}
		}
		else if(bLogChan == INVALID_U8)
		{
			FDL_Log("LogChan = 0xFF,wOfst=%d",tFtlTask.wCur);
		}
		else
		{
			while(1);
		}

		tFtlTask.wCur++;
		tFtlTask.wPageOff += (ptOrderUnit->bOpPageCnt*bMultiPlnNum);
		
        if (!(--wCount))
        {
            break;
        }
    }
	
	U08 bK;
	U08 bChanPhy;
	for (bK = 0; bK < MAX_CHAN_COUNT; bK++)
	{
		bChanPhy = CHANS(bK);//获取物理通道号
		
		if(bChanPhy<MAX_CHAN_COUNT)
		{
        	OrderChanTaskErgodic(bChanPhy);
		}
	}
	
    return TRUE;
}


U08 TaskErgodicTableDataCopyBack(U08 bTaskErgo)
{   
	if(tFtlTask.wCur < tFtlTask.wLen)
	{
		tFtlTask.FTLTaskConvert(0);
	}

	return TRUE;
}

U08 TaskErgodicMultiLunCopyBack(U08 bTaskErgo)
{   
	U08 i;
	U08 bLun;
	U08 bRealLun;
		
	U08 bTempLun;
	for(i = 0;i<2;i++)
	{
		bTempLun = tCB_ProcessCtrl.bLunOpStep;
		bLun = tCB_ProcessCtrl.bLunIdx;
		
		while(bTempLun)
		{
			bRealLun = tCB_ProcessCtrl.bLunQueue[bLun%tCB_ProcessCtrl.bTotelLun];
			if(bRealLun != 0xFF)
			{
				tFtlTask.FTLTaskAllocate(bRealLun);
				bTempLun--;
			}
			bLun++;
		}
	}

	U16 wTaskCnt = 0;
	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		wTaskCnt += tChTaskQueue[CHANS(i)].wTaskCnt;
	}
	
	if(wTaskCnt||tCB_ProcessCtrl.bValidLun)
	{
		tFtlTask.FTLTaskConvert(tFtlTask.wLen);
	}
	
    return TRUE;
}

U08 TaskErgodicInnerCopyBack(U08 bTaskErgo)
{   
	U08 i;
	U08 bLun;
	U08 bRealLun;
		
	for(i = 0;i<2;i++)
	{
		for(bLun = 0;bLun<tFtlTask.wLen;bLun++)
		{	
			bRealLun = tCB_ProcessCtrl.bLunQueue[bLun];
			if(bRealLun != 0xFF)
			{
				tFtlTask.FTLTaskAllocate(bRealLun);
			}
		}
	}

	U16 wTaskCnt = 0;
	for(i = 0;i<CHAN_MAX_CNT;i++)
	{
		wTaskCnt += tChTaskQueue[CHANS(i)].wTaskCnt;
	}

	if(wTaskCnt)
	{
		tFtlTask.FTLTaskConvert(tFtlTask.wLen);
	}
	
    return TRUE;
}


U08 TaskErgodicSingleLunCopyBack(U08 bTaskErgo)
{   
	U08 i,j;
	U08 bLunPair;
			
	for(i = 0;i<2;i++)
	{
		for(j = 0;j<tFtlTask.wLen;j++)
		{	
			bLunPair = tCB_ProcessCtrl.bLunQueue[j];
			if(bLunPair != 0xFF)
			{				
				tFtlTask.FTLTaskAllocate(bLunPair);
			}
		}
	}
		
    for (j = 0; j < tFtlTask.wLen; j++)
    {		
		bLunPair = tCB_ProcessCtrl.bLunQueue[j];
		if(bLunPair != 0xFF)
		{
			tFtlTask.FTLTaskConvert(bLunPair);
		}
	}
	
    return TRUE;
}

void ChanTaskLastAnalysisErase(void)
{
    U16 i;
    U08 bCh;
	T_FLASH_PHY_BLK_ADDR *ptAddr;
	U08 bChanPhy = 0;
	tChTaskQueue[0].wLastOfstInTask = 0; 
	tChTaskQueue[1].wLastOfstInTask = 0;
	tChTaskQueue[2].wLastOfstInTask = 0;	
	tChTaskQueue[3].wLastOfstInTask = 0;
	ptAddr = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[(tFtlTask.wLen)]);
    bCh = 0;
	i = tFtlTask.wLen;
	while (i)
	{
		i--;
		ptAddr--;
		
		bChanPhy = CHANS(ptAddr->m_bChan);//获取物理通道号
        if(bChanPhy < MAX_CHAN_COUNT)
        {
        
			if(tChTaskQueue[bChanPhy].wLastOfstInTask  == 0x00)//value the fisrt ofst in ftl task
		    {
				tChTaskQueue[bChanPhy].bFirstFlag = 0;
				tChTaskQueue[bChanPhy].wLastOfstInTask = i;
				bCh++;
				if(bCh==CHAN_MAX_CNT)
				{
					break;
				}
        	}
        }
	}
	
}

void RunFtlTaskErgodic(U08 bTaskErgo)
{
	if (gFdlTskStatus == ST_TASK_DONE)
    {
        return;
    }

    if (TASK_IRQ != bTaskErgo)
    {
        HAL_MCUSetGieState(FUNC_DISABLE);
#ifdef __TASK_ERROR_FORCE_STOP__
		Fdl_UpdateTaskTime(bTaskErgo);
#endif
    }

    tFtlTask.FTLTaskErgodic(bTaskErgo);

    if (TASK_IRQ != bTaskErgo)
    {
        HAL_MCUSetGieState(FUNC_ENABLE);
    }
}

U08 ChanAnalysis4K(void)
{
	U08 i;
	T_FLASH_PHY_ADDR *ptAddr;
	U08 bCurChanNum = 0;
	for(i = 0; i <  tFtlTask.wLen; i++)
	{
		ptAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[i]);
		if (ptAddr->m_bChan < MAX_CHAN_COUNT)
		{
			bCurChanNum |= (1 << ptAddr->m_bChan);
		}
	} 
	return bCurChanNum;
}
#ifdef _TLC_ORDER_PROGRAM_
U08 TaskErgodicOrderPrgUnit (U08 bTaskErgo)
{
	U08 i;
	U08 bLogLun;
	U08 bLogChan;
	U08 bPhyChan;
	T_CHAN_TASK_QUEUE *pTaskQ;
	if (tFtlTask.bWriteMode == SLC_OPMODE)  //SLC mode
	{
		T_FLASH_PHY_ADDR *pAddr;
		while (tFtlTask.wCur < tFtlTask.wLen)
		{
			pAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[tFtlTask.wCur & (tFtlTask.wContiAddrMax - 1)]);
			bLogChan = pAddr->m_bChan;
			bLogLun = pAddr->m_bLunOfChan;
			bPhyChan = CHANS(bLogChan);
			pTaskQ = &tChTaskQueue[bPhyChan];
			if(pTaskQ->wTaskTail >= (pTaskQ->wTaskHead + MAX_CHAN_TASK))
			{
				break;//full
}

			pTaskQ->wOfstInTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
			pTaskQ->wTaskCnt++;
			pTaskQ->wTaskTail++;

			pTaskQ->tLunTask[bLogLun].dRow[pTaskQ->tLunTask[bLogLun].wLunTail & (MAX_CHAN_TASK-1)] = pAddr->m_dRowAddr;

			pTaskQ->tLunTask[bLogLun].bRowCnt++;
			pTaskQ->tLunTask[bLogLun].wLunTail++;
			tFtlTask.wCur++;
			if((TASK_INIT == bTaskErgo) && (tFtlTask.wFirstErgodicCnt == tFtlTask.wCur))
			{
				break;
			}
		}
	}
	else
	{
		T_FLASH_PROG_ORDER_UNIT *pAddr;
		while (tFtlTask.wCur < tFtlTask.wLen)
		{
			pAddr = &(((T_FLASH_PROG_ORDER_UNIT *)tFtlTask.dSrcArgsAddr)[tFtlTask.wCur & (tFtlTask.wContiAddrMax - 1)]);
			bLogChan = pAddr->bChan;
			bLogLun = pAddr->bLunOfChan;
			bPhyChan = CHANS(bLogChan);
			pTaskQ = &tChTaskQueue[bPhyChan];
				if(pTaskQ->wTaskTail >= (pTaskQ->wTaskHead + MAX_CHAN_TASK))
				{
					break;//full
				}
			pTaskQ->wOfstInTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wCur;
			if (tFtlTask.wCur == 0)
			{
				pTaskQ->wPageOffInFtlTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = 0;
			}
			else
			{
				pAddr --;
				if ((pAddr->dRowAddr[0] >> 24) == 0)
				{
					pTaskQ->wPageOffInFtlTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wPageOff + pAddr->bOpPageCnt;
				}
				else
				{
					pTaskQ->wPageOffInFtlTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = tFtlTask.wPageOff + pAddr->bOpPageCnt * PLANE_OF_LUN;
				}
				pAddr ++;
			}
			tFtlTask.wPageOff = pTaskQ->wPageOffInFtlTask[pTaskQ->wTaskTail & (MAX_CHAN_TASK-1)];
			pTaskQ->wTaskCnt++;
			pTaskQ->wTaskTail++;
			pTaskQ->tLunTask[bLogLun].dRow[pTaskQ->tLunTask[bLogLun].wLunTail & (MAX_CHAN_TASK-1)] = pAddr->dRowAddr[0];
			pTaskQ->tLunTask[bLogLun].bRowCnt++;
			pTaskQ->tLunTask[bLogLun].wLunTail++;
			tFtlTask.wCur++;
		}
	}
	for (i = 0; i < CHAN_MAX_CNT; i++)
	{
		ChanTaskOrderErgodic(CHANS(i));
	}
	return TRUE;
}
#endif
