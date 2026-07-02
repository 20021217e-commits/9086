#include "fdl_erase_block.h"
#include "fdl_debug.h"
#include "flash_driver_struct.h"
#include "fdl_common.h"


U08 TaskAllocateEraseBlock(U16 wOfstInTask)
{	
	T_FLASH_PHY_BLK_ADDR *tBlkAddr = &(((T_FLASH_PHY_BLK_ADDR*)tFtlTask.dSrcArgsAddr)[wOfstInTask]);
	U08 bChanPhy = CHANS(tBlkAddr->m_bChan);//物理通道号
	T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	
	if(tChTaskQ->wTaskTail < (tChTaskQ->wTaskHead + MAX_CHAN_TASK))
	{
		tChTaskQ->wOfstInTask[tChTaskQ->wTaskTail & (MAX_CHAN_TASK-1)] = wOfstInTask;
		tChTaskQ->wTaskCnt++;
		tChTaskQ->wTaskTail++;
	}
	else
	{
		return FALSE;//full
	}
	
	return TRUE;
}


U8 TaskConvertEraseBlock(U8 bChanPhy)
{
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	U16 wOfst = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    T_FLASH_PHY_BLK_ADDR *pBlockAddr = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[wOfst]);
    T_FLASH_PHY_BLK_ADDR *pBlockAddrNext = NULL;

	T_ADDR_INFO tAddr1st;
	T_ADDR_INFO tAddr2nd;

    U08 bNextAddrOPMode = NORMAL_OP_MODE; 
    U08 bLogLunOfChan = pBlockAddr->m_bLunOfChan;
    U32 dLogNum = 0; 
    
	
	Fdl_ConvertAddrInfo(&tAddr1st,(U32)pBlockAddr,TYPE_ERASE_BLOCKS);
    tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan];

    /*****************************check last lun/ce status*********************************/  
    if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] != MULTI_PLN_OP_MODE)
    {        
        HAL_CQChipSelect(tAddr1st.bChan, tAddr1st.bChip);

        if(0 != tChTaskQ->bFirstFlag)
        {
            dLogNum = tChTaskQ->wOfstLunRecord[bLogLunOfChan]|(tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]<<LUN_OP_CNT_IN_LOGNUM);
            HAL_CQCfgLogicNum(tAddr1st.bChan,dLogNum);
            tChTaskQ->bLunContinueOpCnt[bLogLunOfChan] = 0;
			
			if(tFtlTask.bArgsArr[0] == SLC_MODE)
			{
				HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_ERASE, FIO_PF_PASS_SLC_ERASE);/*SLC Pass Or Fail-Bit*/
			}
			else
			{
				HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_ERASE, FIO_PF_PASS_TLC_ERASE);/*TLC Pass Or Fail-Bit*/
			}
        }
		else
		{
			tChTaskQ->bFirstFlag = 1;
		}
    }

    if(tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan] != 0)
    {
        tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]--;
    }
    
    /*****************************analyse next address for op*********************************/
	if((MULTI_PLANE_EN) || (MULTI_LUN_EN) || (MULTI_CE_EN))//多Plane或者多LUN、多CE操作
    {
        if(tChTaskQ->wTaskCnt > 1)
        {       
            pBlockAddrNext = &(((T_FLASH_PHY_BLK_ADDR *)tFtlTask.dSrcArgsAddr)[tChTaskQ->wOfstInTask[(tChTaskQ->wTaskHead+1) & (MAX_CHAN_TASK-1)]]);
			Fdl_ConvertAddrInfo(&tAddr2nd,(U32)pBlockAddrNext,TYPE_ERASE_BLOCKS);
            bNextAddrOPMode = Fdl_AddrModeAnalysis(&tAddr1st, &tAddr2nd);
		}
    }
	
    tChTaskQ->bNextAddrOPMode[bLogLunOfChan] = bNextAddrOPMode;

    /*****************************start erase of current ofst*********************************/
    //FDL_Log("Ofst=%x,c=%x,blk=%x",wOfst,bChan,pBlockAddr->m_wBlkAddr);
    HAL_CQChipSelect(tAddr1st.bChan, tAddr1st.bChip);

	if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] != MULTI_PLN_OP_MODE)//多plane擦中间不发切命令
	{
		if(tFtlTask.bArgsArr[0] == SLC_MODE)
		{
			FL_CMD_CQ_SLC_MODE(tAddr1st.bChan);
		}
		else
		{
			FL_CMD_CQ_TLC_MODE(tAddr1st.bChan);
		}
	}
    
    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_BLOCK_ERASE1);               
    HAL_CQSendRowAddr(tAddr1st.bChan, tAddr1st.dRow);

    if(bNextAddrOPMode != MULTI_PLN_OP_MODE)
    {
        HAL_CQSendFlashCmd(tAddr1st.bChan, FL_CMD_BLOCK_ERASE2);
        tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan]++;
        tChTaskQ->wOfstLunRecord[bLogLunOfChan] = wOfst;
    }
    else
    {
#if ((NAND_FLASH == HYNIX_3DV4) || (NAND_FLASH == HYNIX_8A1M))
		HAL_CQSendFlashCmd(tAddr1st.bChan,0xD1);
#endif
    }

    tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]++;
            
    /******************chan task end, check all lun/ce status*********************/    
    if(wOfst == tChTaskQ->wLastOfstInTask)
    {
        U08 bChip,bChipPhy,bLun;
                
        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
            HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);         

            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)
            {
                bLogLunOfChan = bLun + bChip*LUN_OF_CHIP;
                
                if(tChTaskQ->bLunNoChkStsCnt[bLogLunOfChan] != 0)
                {                             
                    dLogNum = tChTaskQ->wOfstLunRecord[bLogLunOfChan]|(tChTaskQ->bLunContinueOpCnt[bLogLunOfChan]<<LUN_OP_CNT_IN_LOGNUM);
                    HAL_CQCfgLogicNum(tAddr1st.bChan,dLogNum);

					if(tFtlTask.bArgsArr[0] == SLC_MODE)
					{
						HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_ERASE, FIO_PF_PASS_SLC_ERASE);/*SLC Pass Or Fail-Bit*/
					}
					else
					{
						HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_ERASE, FIO_PF_PASS_TLC_ERASE);/*TLC Pass Or Fail-Bit*/
					}
                }
            }
        }
    }

    //HAL_CQGenerateInt(tAddr1st.bChan,0,FUNC_DISABLE);

    return TRUE;
}

