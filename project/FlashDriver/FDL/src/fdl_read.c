#include "fdl_read.h"
#include "fdl_task_op.h"
#include "flash_driver_struct.h"
#include "hal_cq.h"
#include "fdl_common.h"
#include "hal_adm.h"
#include "flash_op_cmd.h"
#include "fdl_read_retry.h"


void ReadClstStartDMA2ByCQ(	U16 wOfst,
							U32 dBuffIdx,
							U08 m_bClstOffs,
							U32 dUDBuffAddr, 
							U32 dDmaInfoAddr,
							Joint_Mode joint,
							T_ADDR_INFO *tAddrPhy)
{
    U16 wSeedIdx 		= INVALID_U16;
    U08 *pMaskBuffAddr 	= PNULL;
	U08 MaskBuffVaule 	= 0;

    if(NULL != tFtlTask.pMskBuff)
    {
        if ((tFtlTask.dTaskType == TYPE_NCQ_READ) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K))//NCQ的联动读，只有头或尾可能有mask信息，
        {
            if (wOfst == 0)//first
            {
				if((tChTaskQueue[tAddrPhy->bChan].bClstCntInPage == tFtlTask.wLen)&&(tFtlTask.wLen != 1))//修改Mask
				{
					MaskBuffVaule = tFtlTask.pMskBuff[(NCQ_READ_MASK_FISRT+tFtlTask.wLen-1) * MASK_BYTE_OF_CLST];
					tFtlTask.pMskBuff[(NCQ_READ_MASK_FISRT+tFtlTask.wLen-1) * MASK_BYTE_OF_CLST] = tFtlTask.pMskBuff[NCQ_READ_MASK_END * MASK_BYTE_OF_CLST];;
				}
				pMaskBuffAddr = &tFtlTask.pMskBuff[NCQ_READ_MASK_FISRT * MASK_BYTE_OF_CLST];
            }
            else if((wOfst + tChTaskQueue[tAddrPhy->bChan].bClstCntInPage) == (tFtlTask.wLen))
            {
                pMaskBuffAddr = &tFtlTask.pMskBuff[(NCQ_READ_MASK_CLST-tChTaskQueue[tAddrPhy->bChan].bClstCntInPage) * MASK_BYTE_OF_CLST];
            }
            else//中间的若干
            {
                pMaskBuffAddr = &tFtlTask.pMskBuff[NCQ_READ_MASK_MID * MASK_BYTE_OF_CLST];
            }
        }
        else
        {
            pMaskBuffAddr = &tFtlTask.pMskBuff[wOfst * MASK_BYTE_OF_CLST];
        }
    }

    if(NULL != tFtlTask.pwSeedIndex)
    {
        wSeedIdx = ((U16 *)tFtlTask.pwSeedIndex)[wOfst];
    }
	
    HAL_CQStartDMA2Read(tAddrPhy->bChan, 
						tFtlTask.dHandleBuff, 
						dBuffIdx, 
						m_bClstOffs*tAddrPhy->bSecOfClst,
			            joint, 
			            dDmaInfoAddr, 
			            dUDBuffAddr, 
			            tAddrPhy->dRow, 
			            pMaskBuffAddr,
			            MASK_BYTE_OF_CLST*tChTaskQueue[tAddrPhy->bChan].bClstCntInPage,
			            wSeedIdx);
	
    if (DISCRETE_CLST != tFtlTask.bArgsArr[0])
	{
		if(	(tChTaskQueue[tAddrPhy->bChan].bClstCntInPage == tFtlTask.wLen)&&
			(tFtlTask.wLen != 1)&&
			(NULL != tFtlTask.pMskBuff))//恢复上面改掉的Mask,Retry会用到原始的Mask信息,
		{
			tFtlTask.pMskBuff[(NCQ_READ_MASK_FISRT+tFtlTask.wLen-1) * MASK_BYTE_OF_CLST] = MaskBuffVaule;
		}
	}
}


U08 GetClstCntInPage(T_FLASH_PHY_ADDR *ptAddr,U32 dOfst)
{
    U32 dRow			= ptAddr->m_dRowAddr;
    U08 bLun			= ptAddr->m_bLunOfChan;
    U08 bChan			= ptAddr->m_bChan;
    U08 bClstOff		= ptAddr->m_bClstOffs;
    U32 dOfstFirst		= dOfst;
    U08 bClstCntInPage 	= 1;
	
	if( (tFtlTask.dTaskType == TYPE_NCQ_READ) || 		/*联动读支持Page启动*/
		((tFtlTask.dTaskType == TYPE_READ_TO_BUFFER) &	/*非联动读支持Page启动*/
		(DISCRETE_CLST != tFtlTask.bArgsArr[0])))		/*离散读不能Page启动*/
    {
        U16 wRevClstCntOfEccSts;
        wRevClstCntOfEccSts = tFtlTask.wMaxClstOffOfEccSts - dOfstFirst%tFtlTask.wMaxClstOffOfEccSts;

        while(++dOfst < tFtlTask.wCur)//一定是已经分配到各个通道各个LUN的
        {
            ptAddr++;

			if(	((U32)ptAddr >= (tFtlTask.AddrBuffSize + tFtlTask.dSrcArgsAddr)) || 
				(bClstCntInPage >= wRevClstCntOfEccSts ))
            {
				//FDL_Log("large value break:0x%X--0x%X,bClstCntInPage=%d",ptAddr,(tFtlTask.AddrBuffSize + tFtlTask.dSrcArgsAddr),bClstCntInPage);
                break;
            }

            if(	ptAddr->m_bChan 		== bChan &&       
                ptAddr->m_dRowAddr 		== dRow  &&
                ptAddr->m_bLunOfChan 	== bLun  &&
                ptAddr->m_bClstOffs 	== (bClstOff+1))
            {
                bClstOff++;
                bClstCntInPage++;
            }
            else
            {
                break;
            }	
        }
    }

    return bClstCntInPage;
}


void Task_MultiCEForContinue(U08 bChanPhy,U08 bTaskErgo)
{
#ifndef __CACHE_READ_ON__
	if(((MULTI_LUN_EN) || (MULTI_CE_EN))&&(TASK_ADD_SAG == bTaskErgo))
	{
		if((MULTI_CE_EN) && (tFtlTask.dTaskType == TYPE_NCQ_READ))
		{
			tChTaskQueue[bChanPhy].bNextAddrOPMode[0] =  MULTI_CE_OP_MODE;//change to another ce op
		}
	}
#endif
}


#ifdef __VER_TLC_PAGE__
U08 TaskConvertVerifyTLCPage(U08 bChan)
{
	U08 bLun;
    U08 bChip;
    U08 bChipPhy;
    U32 dBuffIdx;
    U32 dUDBuffAddr;
    U32 dDmaInfoAddr;
    U16 wTolSecCfg;

    U32 dRowOp;
	U32 dMulitInfo;/*任务分配时传下来的多重信息*/	
    U08 bLogLunOfCH;	
	U32 dLogicNumExt;/*包含逻辑通道和片选号的Ofset(LunTask和CHTask),作为LogNum写进寄存器*/
	U16 wOfstInCHTask;
    Joint_Mode bJointMode = JOINT_NONE;

    T_ADDR_INFO tAddr1st;
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChan];
	Fdl_ConvertAddrInfoVerTlc(&tAddr1st,bChan,tChTaskQ);

	dMulitInfo		= tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];/*不知道有没有与Row对应不上的风险！！*/	
    dBuffIdx  		= VER_TLC_GET_CH_TASK_OFST(dMulitInfo)*DSECT_OF_PAGE;//bChan*DSECT_OF_PAGE;/*修改,主要用来Retry用*/
    wTolSecCfg  	= DSECT_OF_PAGE;/*一次启动一个Page*/
    bJointMode 		= tFtlTask.bJointMode;
	bLogLunOfCH 	= VER_TLC_GET_LOG_CE(dMulitInfo);/*逻辑片选用2Bit表示*/
	dLogicNumExt	= dMulitInfo;
    tChTaskQ->bClstCntInPage = 1;//PLANE_OF_LUN;
	
#if 0
	PRINT("pAddr%04d:|....%02d....|..%02d..|.....%02d....|....%02d....|.0x%06X.|\n",dOfst&(0x0FFFFF),\
			0,\
			bChan,\
			bLogLunOfChan,\
			0,\
			tAddr1st.dRow);
#endif

    HAL_CQCheckHold(1,bJointMode,tAddr1st.bChan, dBuffIdx, wTolSecCfg,FALSE);
     /*****************************send flash cmd previously*********************************/
    if(0 == tChTaskQ->bFirstFlag)//if the chan send cmd firstly,send all cmd previouly
    {
        tChTaskQ->bFirstFlag = 1;
        HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1); //set ud,length and AttachSize set once is ok
		Fdl_CQPreReadMultiCe(tAddr1st.bChan,tChTaskQ);
    }
    else if((tChTaskQ->bNextAddrOPMode[0] == MULTI_CE_OP_MODE) ||
            (tChTaskQ->bNextAddrOPMode[0] == MULTI_LUN_OP_MODE))//if normal or cache op,send cmd previously
    {
        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)//片选选中
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
            HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);

            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)//所有lun命令提前先发
            {
                U08 bLunOfChan = bLun + bChip*LUN_OF_CHIP;
                if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0) &&
                   (tChTaskQ->tLunTask[bLunOfChan].bLunRowPreOpCnt == 0))
                {     							
                    dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[tChTaskQ->tLunTask[bLunOfChan].wLunHead & (MAX_CHAN_TASK-1)] & PLANE_BIT_MASK_OF_ROW;					
                    Fdl_CQPreReadMultiPln(tAddr1st.bChan,bLunOfChan,dRowOp,FL_CMD_READ2);
                    tChTaskQ->tLunTask[bLunOfChan].bLunRowPreOpCnt += PLANE_OF_LUN;                      
                }
            }
        }
    }
			
 /*****************************start dma2 of current ofst address*********************************/
	dDmaInfoAddr = tFtlTask.dEccStsBuff + VER_TLC_ECC_INFO_OFSEX(bChan,bLogLunOfCH) + GET_PLANE_NUM(tAddr1st.dRow)*VER_TLC_DMA_CNT_PAGE*2;/*2Byte Per DMA Info pFdlParam.dEccInfoBuffSize*/
    dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];/*UserData Buff(暂时设计成一直覆盖模式)*/
    
    HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
	HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);

	SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay
	SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay

	HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);

	HAL_CQStartDMA2Read(tAddr1st.bChan, 
						tFtlTask.dHandleBuff, 
						dBuffIdx, 
						0, 						/*FlashIndex为0*/
//						tAddr1st.bSecOfClst, 	/*没有用到*/
						bJointMode, 
						dDmaInfoAddr, 
						dUDBuffAddr, 
						tAddr1st.dRow, 
						PNULL, 					/*没有Mask*/
						0,						/*所以Masklen是0*/
						INVALID_U16);			/*默认种子*/

	if(GET_PLANE_NUM(tAddr1st.dRow) == (PLANE_OF_LUN - 1))
	{
		HAL_CQCfgLogicNumExt(tAddr1st.bChan,dMulitInfo);/*LogNum的扩展应用*/
		HAL_CQCheckRealDone(tAddr1st.bChan);
		HAL_CQCheckHoldOnly(tAddr1st.bChan,TRUE);
	}


	
	HAL_CQGenerateInt(tAddr1st.bChan, 0, FUNC_DISABLE);
	
	/*****************************analyse ftl task next ofst address*********************************/
    if((MULTI_LUN_EN) || (MULTI_CE_EN))
    {
        tChTaskQ->bNextAddrOPMode[0] =  MULTI_CE_OP_MODE;//change to another ce op
    }

    tChTaskQ->tLunTask[bLogLunOfCH].wLunHead += tChTaskQ->bClstCntInPage;
    tChTaskQ->tLunTask[bLogLunOfCH].bRowCnt -= tChTaskQ->bClstCntInPage;	
	if(tChTaskQ->tLunTask[bLogLunOfCH].bLunRowPreOpCnt > 0)
	{
		tChTaskQ->tLunTask[bLogLunOfCH].bLunRowPreOpCnt--;
	}

    return TRUE;
}


#endif

U8 TaskVirtualClstConvert(U8 bChanPhy,U32 dOfst,U08 bJointMode,T_FLASH_PHY_ADDR *ptClstAddr)
{
	U32 dBuffIdx;
	U32 dDmaInfoAddr;
	U32 dUDBuffAddr;
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
	//U08 bPhyChip = CHIPES(bChanPhy, ptClstAddr->m_bClstOffs);
	U08 *pMaskBuffAddr = NULL;
	U08 bClstSec = SECT_OF_DATA_CLST;

	if(tFtlTask.bDataType == TYPE_LOGIC)
	{
		dBuffIdx  = dOfst * SECT_OF_DATA_CLST;//计算BuffIndex需要真实的wOfst
	}
	else
		{
		dBuffIdx  = dOfst * SECT_OF_TBLE_CLST;//计算BuffIndex需要真实的wOfst
		}

    if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
		{
        dBuffIdx = tFtlTask.pwBuffIdx[dOfst];
		}
		if(NULL != tFtlTask.pMskBuff)
		{
		if((tFtlTask.dTaskType == TYPE_NCQ_READ) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K))
			{
				if (dOfst == 0)//first
				{
					pMaskBuffAddr = &tFtlTask.pMskBuff[NCQ_READ_MASK_FISRT * MASK_BYTE_OF_CLST];
				}
				else if(dOfst + 1  == tFtlTask.wLen)
				{
					pMaskBuffAddr = &tFtlTask.pMskBuff[NCQ_READ_MASK_END* MASK_BYTE_OF_CLST];
				}
				else//中间的若干
				{
					pMaskBuffAddr = &tFtlTask.pMskBuff[NCQ_READ_MASK_MID * MASK_BYTE_OF_CLST];
				}
			}
			else
			{
				pMaskBuffAddr = &tFtlTask.pMskBuff[dOfst * MASK_BYTE_OF_CLST];
			}
		}
	if(tFtlTask.bDataType == TYPE_LOGIC)
	{
		dDmaInfoAddr = tFtlTask.dEccStsBuff + (((dOfst & (tFtlTask.wContiAddrMax - 1))*SECT_OF_DATA_CLST/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;
		if((U32)NULL == tFtlTask.dUdBuff)
		{
			dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];
	}
	else
	{
			dUDBuffAddr  = tFtlTask.dUdBuff + (dOfst & (tFtlTask.wContiAddrMax - 1))*UD_LEN_OF_DATA_CLST;
		}
		bClstSec = SECT_OF_DATA_CLST;
	}
	else
	{
		dDmaInfoAddr = tFtlTask.dEccStsBuff + (((dOfst & (tFtlTask.wContiAddrMax - 1))*SECT_OF_TBLE_CLST/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;
		if((U32)NULL == tFtlTask.dUdBuff)
		{
			dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];
		}
		else
		{
			dUDBuffAddr  = tFtlTask.dUdBuff + (dOfst & (tFtlTask.wContiAddrMax - 1))*UD_LEN_OF_TBLE_CLST;
		}
		bClstSec = SECT_OF_TBLE_CLST;
	}
    HAL_CQCfgLogicNum(bChanPhy,dOfst);//逻辑Num用真实的wOfst
	HAL_CQCheckHold(1,bJointMode,bChanPhy,dBuffIdx, bClstSec,1);
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_FC_REG | (FC_OFFS_NF_BP_CTRL << R_CQ_REG_BIT) | 0xFF;//
	U16 wFillData = ((ptClstAddr->m_bLunOfChan<<8)|ptClstAddr->m_bLunOfChan);	
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG | (FC_OFFS_SPC_PATTEN << R_CQ_REG_BIT) | wFillData;
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG | ((FC_OFFS_SPC_PATTEN+2) << R_CQ_REG_BIT) | wFillData;
	wFillData = (pFdlParam.bUnValidDataUser[1]<<8|pFdlParam.bUnValidDataUser[0]);
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG | (FC_OFFS_SPC_UDPATTEN << R_CQ_REG_BIT) |wFillData ;
	wFillData = (pFdlParam.bUnValidDataUser[3]<<8|pFdlParam.bUnValidDataUser[2]);
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG | ((FC_OFFS_SPC_UDPATTEN+2) << R_CQ_REG_BIT) | wFillData;
	HAL_CQStartDMA2Read(bChanPhy,
						tFtlTask.dHandleBuff,
						dBuffIdx, 
						0,
						bJointMode, 
						dDmaInfoAddr, 
						dUDBuffAddr, 
						0, 
						pMaskBuffAddr,
						MASK_BYTE_OF_CLST,
						0xFFFF);
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_FC_REG | (FC_OFFS_NF_BP_CTRL << R_CQ_REG_BIT) | 0;
	tChTaskQueue[bChanPhy].bClstCntInPage = 1;
	if(tChTaskQ->wTaskCnt == tChTaskQ->bClstCntInPage)//解决虚拟簇为最后一个簇时不处理异常的问题
    {
        HAL_CQCheckReadEnd(bChanPhy,TRUE);
    }
	tChTaskQ->tLunTask[ptClstAddr->m_bClstOffs].wLunHead += 1;
	tChTaskQ->tLunTask[ptClstAddr->m_bClstOffs].bRowCnt -= 1;

	return TRUE;
}
#ifndef __CACHE_READ_ON__
#ifdef __FDL_HW_VIRTUL_CLST__
U32 GetRowForPreJudgeRead(U08 bJointMode,U08 bChanPhy,U08 bLogLun)
{
	U32 dTempRow = tChTaskQueue[bChanPhy].tLunTask[bLogLun].dCmdRow;
#if 1
	T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];

	if(bJointMode == JOINT_21)
	{
		if((tChTaskQ->tLunTask[bLogLun].bRowPre != 0xFF)&&(tChTaskQ->wTaskCnt > tChTaskQ->bClstCntInPage))
		{
			if( ((GET_PAGE_NUM(dTempRow) != (PAGE_OF_BLOCK_SLC - 1)) && ((dTempRow&0xFF000000) == 0)) ||
				((GET_PAGE_NUM(dTempRow) != (PAGE_OF_BLOCK - 1)) && ((dTempRow&0xFF000000) != 0)))
			{
				dTempRow += 1;
				tChTaskQ->tLunTask[bLogLun].bRowPre = 0xFF;
			}
		}
	}
#endif

	return dTempRow;
}

U8 TaskConvertReadClstToBuff(U8 bChanPhy)
{
    U08 bChipPhy;
    U08 bLunOfChan;
    U32 dBuffIdx;
    U32 dUDBuffAddr;
    U32 dDmaInfoAddr;
    U16 wTotalSecCfg;

    U32 dOfst;
    U32 dRowOp;
    U08 bLogLunOfChan;
    Joint_Mode bJointMode 	= JOINT_NONE;

    T_ADDR_INFO tAddr1st;
    T_FLASH_PHY_ADDR *ptClstAddr;
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];

    dOfst 			= tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    ptClstAddr 		= &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfst & (tFtlTask.wContiAddrMax - 1)]);//注意:tFtlTask.wContiAddrMax要是2的幂次方
    bLogLunOfChan 	= ptClstAddr->m_bLunOfChan;

    if(tFtlTask.dTaskType == TYPE_NCQ_READ)
    {
        bJointMode = JOINT_21;
    }
    else if (TYPE_READ_TO_BUFFER == tFtlTask.dTaskType)
    {
        bJointMode = tFtlTask.bJointMode;
    }

	if(ptClstAddr->m_bChan == 0xFF)
    {
		U8 bRet = TaskVirtualClstConvert(bChanPhy,dOfst,bJointMode,ptClstAddr);
		return bRet;
    }
	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
    if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
    {
        dBuffIdx = tFtlTask.pwBuffIdx[dOfst];//离散读簇到Buff暂时不考虑ContinueTask的情况
    }
    else
    {
        dBuffIdx  = dOfst * tAddr1st.bSecOfClst;//计算BuffIndex需要真实的wOfst
    }

    tChTaskQ->bClstCntInPage = GetClstCntInPage(ptClstAddr,dOfst);
    wTotalSecCfg = tChTaskQ->bClstCntInPage*tAddr1st.bSecOfClst;
	HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,DMA2_RD_TO_BUFF); //set ud,length and AttachSize set once is ok
    for(bLunOfChan = 0; bLunOfChan < LUN_OF_CHAN; bLunOfChan++)//片选选中
    {
		if(bLogLunOfChan == bLunOfChan)
        {
			dRowOp = (tAddr1st.dRow&PLANE_BIT_MASK_OF_ROW);
        }
		else
        {
			if(tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0)
			{
				dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[tChTaskQ->tLunTask[bLunOfChan].wLunHead & (MAX_CHAN_TASK-1)];
				if(dRowOp != 0xFFFFFFFF)
				{
					tChTaskQ->tLunTask[bLunOfChan].bRowPre = 0;
				}
			}
			else
			{
				dRowOp = GetRowForPreJudgeRead(bJointMode,bChanPhy,bLunOfChan);
			}
        }

		if((dRowOp != 0xFFFFFFFF)&&(tChTaskQueue[bChanPhy].tLunTask[bLunOfChan].dCmdRow != dRowOp))
		{
			dRowOp &= (PLANE_BIT_MASK_OF_ROW);
            bChipPhy  = CHIPES(bChanPhy,bLunOfChan);   
			HAL_CQChipSelect(bChanPhy,bChipPhy);
			Fdl_CQPreReadMultiPln(bChanPhy,bChipPhy,dRowOp,FL_CMD_READ2);
			tChTaskQueue[bChanPhy].tLunTask[bLunOfChan].dCmdRow = dRowOp;
        }        
    }

    /*****************************start dma2 of current ofst address*********************************/
    if((U32)NULL == tFtlTask.dUdBuff)
    {
        dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs*tAddr1st.bUdSize;
    }
    else
    {
        dUDBuffAddr  = tFtlTask.dUdBuff + dOfst*tAddr1st.bUdSize - ptClstAddr->m_bClstOffs*tAddr1st.bUdSize;
    }

    dDmaInfoAddr = tFtlTask.dEccStsBuff + ((dOfst*tAddr1st.bSecOfClst/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;
    HAL_CQCfgLogicNum(tAddr1st.bChan,dOfst);//逻辑Num用真实的wOfst
    HAL_CQCheckHold(1,bJointMode,tAddr1st.bChan,dBuffIdx, wTotalSecCfg,TRUE);
	HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
	HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
#if (NAND_FLASH == TOS_BICS3)
    FL_CMD_CQ_TLC_MULTI_PLN(tAddr1st.dRow);
#endif
	HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);	
	ReadClstStartDMA2ByCQ(	dOfst,
							dBuffIdx,
							ptClstAddr->m_bClstOffs,
							dUDBuffAddr,								
							dDmaInfoAddr,
							bJointMode,																
							&tAddr1st);
	if(tChTaskQ->wTaskCnt == tChTaskQ->bClstCntInPage)
    {
        HAL_CQCheckReadEnd(tAddr1st.bChan,TRUE);
    }
    tChTaskQ->tLunTask[bLogLunOfChan].wLunHead += tChTaskQ->bClstCntInPage;
    tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt -= tChTaskQ->bClstCntInPage;

    dRowOp = 0xFFFFFFFF;
	if(tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt != 0)
    {
		dRowOp = tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)];
    }
	else
	{
		dRowOp = GetRowForPreJudgeRead(bJointMode,bChanPhy,bLogLunOfChan);
	}

	if(dRowOp != 0xFFFFFFFF)
	{
		dRowOp &= (PLANE_BIT_MASK_OF_ROW);
		if(tChTaskQueue[bChanPhy].tLunTask[bLogLunOfChan].dCmdRow != dRowOp)
		{
			HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
			Fdl_CQPreReadMultiPln(tAddr1st.bChan,tAddr1st.bChip,dRowOp,FL_CMD_READ2);
			tChTaskQueue[bChanPhy].tLunTask[bLogLunOfChan].dCmdRow = dRowOp;
		}
	}
    return TRUE;
}
#else
U8 TaskConvertReadClstToBuff(U8 bChanPhy)
{
    U08 bLun;
    U08 bChip;
    U08 bChipPhy;
    U08 bLunOfChan;
    U32 dBuffIdx;
    U32 dUDBuffAddr;
    U32 dDmaInfoAddr;
    U16 wTotalSecCfg;

    U32 dOfst;
    U32 dRowOp;
    U32 dRowOpNext;
    U08 bLogLunOfChan;
    U08 bOPMode2nd      	= NORMAL_OP_MODE;//the relationship of current addr and next addr in chan task ofst
    U08 bLunOPMode2nd   	= NORMAL_OP_MODE;//the relationship of current addr and next addr in lun task
    U08 bCheckHoldEn    	= TRUE;//decide to check hold or not
    U08 PreSendCmdFlag		= 0;//命令预发标志,目的是为了后面切回当前CE
    Joint_Mode bJointMode 	= JOINT_NONE;

    T_ADDR_INFO tAddr1st;
    T_FLASH_PHY_ADDR *ptClstAddr;
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];

    dOfst 			= tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    ptClstAddr 		= &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfst & (tFtlTask.wContiAddrMax - 1)]);//注意:tFtlTask.wContiAddrMax要是2的幂次方
    bLogLunOfChan 	= ptClstAddr->m_bLunOfChan;


    if(tFtlTask.dTaskType == TYPE_NCQ_READ)
    {
        bJointMode = JOINT_21;
    }
    else if (TYPE_READ_TO_BUFFER == tFtlTask.dTaskType)
    {
        bJointMode = tFtlTask.bJointMode;
    }

	//FDL_Log("Real-Off-%x,Chan%x,CE%x,",dOfst,bChanPhy,bLogLunOfChan);
	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
    HAL_CQCfgLogicNum(tAddr1st.bChan,dOfst);//逻辑Num用真实的wOfst

    if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
    {
        bCheckHoldEn = FALSE;
    }

    if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
    {
        dBuffIdx = tFtlTask.pwBuffIdx[dOfst];//离散读簇到Buff暂时不考虑ContinueTask的情况
    }
    else
    {
        dBuffIdx  = dOfst * tAddr1st.bSecOfClst;//计算BuffIndex需要真实的wOfst
    }

    tChTaskQ->bClstCntInPage = GetClstCntInPage(ptClstAddr,dOfst);
    wTotalSecCfg = tChTaskQ->bClstCntInPage*tAddr1st.bSecOfClst;

    HAL_CQCheckHold(1,bJointMode,tAddr1st.bChan, dBuffIdx, wTotalSecCfg,bCheckHoldEn);
	
    /*****************************analyse next address in lun for op*********************************/
    tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1];

    if( tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > tChTaskQ->bClstCntInPage)
    {
#if (NAND_FLASH == TOS_BICS3) || (NAND_FLASH == HYNIX_8A1M)
	    U8 dWlNub = 0;
        U8 dWlNextNub = 0;
		
        dRowOp = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)]);
        dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead+tChTaskQ->bClstCntInPage) & (MAX_CHAN_TASK-1)]);
		
		 if (dRowOp &0xff000000)
        {
            dWlNub =  (dRowOp>>24);
      		dRowOp &= 0x00FFFFFF;
        }
        if (dRowOpNext &0xff000000)
        {        
      		dWlNextNub =  (dRowOpNext>>24);
      		dRowOpNext &= 0x00FFFFFF;
        }        

        if(MULTI_CLST_EN && (dRowOp == dRowOpNext)&&(dWlNub == dWlNextNub))
        {
            bLunOPMode2nd = MULTI_CLST_OP_MODE;//support multi clst op
        }
        else if(MULTI_PLANE_EN && ((dRowOp&PLANE_BIT_MASK_OF_ROW) == (dRowOpNext&PLANE_BIT_MASK_OF_ROW))&&(dWlNub == dWlNextNub))
        {
            bLunOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
        }
		
#else
        dRowOp = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)]) & 0x00ffffff;
        dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead+tChTaskQ->bClstCntInPage) & (MAX_CHAN_TASK-1)]) & 0x00ffffff;

        if(MULTI_CLST_EN && (dRowOp == dRowOpNext))
        {
            bLunOPMode2nd = MULTI_CLST_OP_MODE;//support multi clst op
        }
        else if(MULTI_PLANE_EN && ((dRowOp&PLANE_BIT_MASK_OF_ROW) == (dRowOpNext&PLANE_BIT_MASK_OF_ROW)))
        {
            bLunOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
        }
#endif	
    }
    tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1] = bLunOPMode2nd;//next lun address op mode record in bNextAddrOPMode[Lun+1]

    /*****************************send flash cmd previously*********************************/
    if((tChTaskQ->bFirstFlag & 0x01) == 0x00)//if the chan send cmd firstly,send all cmd previouly
	{
		HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1); //set ud,length and AttachSize set once is ok

		if((tChTaskQ->bFirstFlag & 0x10) != 0x10)//该通道前面没有预发,这里要预发
		{
			Fdl_CQPreReadMultiCe(tAddr1st.bChan,tChTaskQ);
		}
		tChTaskQ->bFirstFlag |= 0x01;
	}
    else if((tChTaskQ->bNextAddrOPMode[0] == MULTI_CE_OP_MODE) ||
            (tChTaskQ->bNextAddrOPMode[0] == MULTI_LUN_OP_MODE))//if normal or cache op,send cmd previously
    {
        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)//片选选中
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);         
            HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);

            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)//所有lun命令提前先发
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
                //FDL_Log("chan=%x lun=%x bLunOfChan=%x",bChan,bLogLunOfChan,bLunOfChan);
                //FDL_Log("LunCnt=%x LunPreOpCnt=%x",tChTaskQ->tLunTask[bLunOfChan].bRowCnt,tChTaskQ->tLunTask[bLunOfChan].bLunRowPreOpCnt);
                if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0) &&
                        (tChTaskQ->tLunTask[bLunOfChan].bLunRowPreOpCnt == 0))
                {                   
                    dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[tChTaskQ->tLunTask[bLunOfChan].wLunHead & (MAX_CHAN_TASK-1)] & PLANE_BIT_MASK_OF_ROW;
                    Fdl_CQPreReadMultiPln(tAddr1st.bChan,bLunOfChan,dRowOp,FL_CMD_READ2);
                    tChTaskQ->tLunTask[bLunOfChan].bLunRowPreOpCnt += PLANE_OF_LUN;

                }
            }
        }
        PreSendCmdFlag = 1;//置命令预发标志,下面切回当前CE
    }
    else if(tChTaskQ->bNextAddrOPMode[0] == NORMAL_OP_MODE)//in the same lun
    {
        HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
        tChTaskQ->tLunTask[bLogLunOfChan].bLunRowPreOpCnt = 0;
       
        if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == NORMAL_OP_MODE)
        {
            dRowOp = tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)] & PLANE_BIT_MASK_OF_ROW;

            Fdl_CQPreReadMultiPln(tAddr1st.bChan,bLogLunOfChan,dRowOp,FL_CMD_READ2);

            tChTaskQ->tLunTask[bLogLunOfChan].bLunRowPreOpCnt += PLANE_OF_LUN;
        }        
    }

    /*****************************start dma2 of current ofst address*********************************/
    if((U32)NULL == tFtlTask.dUdBuff)
    {
        dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs*tAddr1st.bUdSize;
    }
    else
    {
        dUDBuffAddr  = tFtlTask.dUdBuff + (dOfst & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bUdSize - ptClstAddr->m_bClstOffs*tAddr1st.bUdSize;
    }

    dDmaInfoAddr = tFtlTask.dEccStsBuff + (((dOfst & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bSecOfClst/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;


	if(	(PreSendCmdFlag) ||	//有预发多Plane命令,要切回当前CE
		(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] != MULTI_CLST_OP_MODE))//in one page,no send check status
	{
		PreSendCmdFlag = 0;
		HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
		HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
	}
		
	SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay
	SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay

#if (NAND_FLASH == TOS_BICS3) || (NAND_FLASH == HYNIX_8A1M)
    FL_CMD_CQ_TLC_MULTI_PLN(tAddr1st.bChan,tAddr1st.dRow);
#endif


	HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);	


	ReadClstStartDMA2ByCQ(	dOfst,
							dBuffIdx,
							ptClstAddr->m_bClstOffs,
							dUDBuffAddr,								
							dDmaInfoAddr,
							bJointMode,																
							&tAddr1st);

                            
	if(tChTaskQ->wTaskCnt == tChTaskQ->bClstCntInPage)
    {
        HAL_CQCheckReadEnd(tAddr1st.bChan,bCheckHoldEn);
    }

    //HAL_CQGenerateInt(tAddr1st.bChan, 0, FUNC_DISABLE);

    /*****************************analyse ftl task next ofst address*********************************/
    if((MULTI_LUN_EN) || (MULTI_CE_EN))
    {
        //T_FLASH_PHY_ADDR *ptClstAddr2nd = NULL;
        T_ADDR_INFO tAddr2nd;

        if(tChTaskQ->wTaskCnt > tChTaskQ->bClstCntInPage)
        {
            U32 wOfstNext = tChTaskQ->wOfstInTask[(tChTaskQ->wTaskHead+tChTaskQ->bClstCntInPage) & (MAX_CHAN_TASK-1)];
            ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOfstNext % tFtlTask.wContiAddrMax]);

            tAddr2nd.bChip = PHYCHIP_OF_LOGLUN(tAddr1st.bChan,ptClstAddr->m_bLunOfChan);
            tAddr2nd.bLun  = PHYLUN_OF_LOGLUN(tAddr1st.bChan,ptClstAddr->m_bLunOfChan);
            if(tAddr1st.bChip != tAddr2nd.bChip)
            {
                if(MULTI_CE_EN)
                {
                    bOPMode2nd =  MULTI_CE_OP_MODE;//change to another ce op
                }
            }
            else if(tAddr1st.bLun != tAddr2nd.bLun)
            {
                if(MULTI_LUN_EN)
                {
                    bOPMode2nd =  MULTI_LUN_OP_MODE;//change to another lun op
                }
            }
        }
    }

    tChTaskQ->bNextAddrOPMode[0] = bOPMode2nd;

//	tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)] = 0;//Delete
    tChTaskQ->tLunTask[bLogLunOfChan].wLunHead += tChTaskQ->bClstCntInPage;
    tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt -= tChTaskQ->bClstCntInPage;
    //FDL_Log("chan=%x lun=%x rcnt=%x",bChan,bLogLunOfChan,tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt);

    if(tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1] != MULTI_CLST_OP_MODE)//next lun addr is not in the same page,reduce lun_preopcnt
    {
        if(tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1] == NORMAL_OP_MODE)
        {
            tChTaskQ->tLunTask[bLogLunOfChan].bLunRowPreOpCnt = 0;
        }
        else
        {
            tChTaskQ->tLunTask[bLogLunOfChan].bLunRowPreOpCnt--;
        }
    }
    if(tChTaskQ->bNextAddrOPMode[0] != NORMAL_OP_MODE)//Change Ce/lun,so this lun page have read,set RowPreOpCnt=0
    {
        tChTaskQ->tLunTask[bLogLunOfChan].bLunRowPreOpCnt = 0;
    }

    return TRUE;
}
#endif
#else
U8 TaskConvertReadClstToBuff(U8 bChanPhy)
{
    U08 bLun;
    U08 bChip;
    U08 bChipPhy;
    U08 bLunOfChan;
    U32 dBuffIdx;
    U32 dUDBuffAddr;
    U32 dDmaInfoAddr;
    U16 wTotalSecCfg;

    U32 dOfst;
    U32 dRowOp;
    U32 dRowOpNext;
    U08 bLogLunOfChan;
    U08 bLunOPMode2nd   	= NORMAL_OP_MODE;//the relationship of current addr and next addr in lun task
    U08 bCheckHoldEn    	= TRUE;//decide to check hold or not
    Joint_Mode bJointMode 	= JOINT_NONE;

    T_ADDR_INFO tAddr1st;
    T_FLASH_PHY_ADDR *ptClstAddr;
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];

    dOfst 			= tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    ptClstAddr 		= &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfst & (tFtlTask.wContiAddrMax - 1)]);//注意:tFtlTask.wContiAddrMax要是2的幂次方
    bLogLunOfChan 	= ptClstAddr->m_bLunOfChan;

	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
    HAL_CQCfgLogicNum(tAddr1st.bChan,dOfst);//逻辑Num用真实的wOfst

	
	U32 dLogicNum = (	(tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1]) +
						(tChTaskQ->bCacheOpFlag[bLogLunOfChan]<<4))&0xFF;
	HAL_CQCfgLogicNumExt(tAddr1st.bChan,dLogicNum);

	
    if(tFtlTask.dTaskType == TYPE_NCQ_READ)
    {
        bJointMode = JOINT_21;
    }
    else if (TYPE_READ_TO_BUFFER == tFtlTask.dTaskType)
    {
        bJointMode = tFtlTask.bJointMode;
    }

    if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
    {
        dBuffIdx = tFtlTask.pwBuffIdx[dOfst];//离散读簇到Buff暂时不考虑ContinueTask的情况
    }
    else
    {
        dBuffIdx  = dOfst * tAddr1st.bSecOfClst;//计算BuffIndex需要真实的wOfst
    }

    tChTaskQ->bClstCntInPage = GetClstCntInPage(ptClstAddr,dOfst);
    wTotalSecCfg = tChTaskQ->bClstCntInPage*tAddr1st.bSecOfClst;

    HAL_CQCheckHold(1,bJointMode,tAddr1st.bChan, dBuffIdx, wTotalSecCfg,bCheckHoldEn);
	
    /*****************************analyse next address in lun for op*********************************/
    tChTaskQ->bLastAddrOPMode[bLogLunOfChan] = tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1];

    if( tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > tChTaskQ->bClstCntInPage)
    {		
        dRowOp = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)]);
        dRowOpNext = (tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead+tChTaskQ->bClstCntInPage) & (MAX_CHAN_TASK-1)]);

        if(MULTI_CLST_EN && (dRowOp == dRowOpNext))
        {
            bLunOPMode2nd = MULTI_CLST_OP_MODE;//support multi clst op
        }
        else if(MULTI_PLANE_EN && ((dRowOp&PLANE_BIT_MASK_OF_ROW) == (dRowOpNext&PLANE_BIT_MASK_OF_ROW)))
        {
            bLunOPMode2nd = MULTI_PLN_OP_MODE;//support multi plane op
        }
		
#ifdef __TAB_CACHE_RD_OFF__
		else if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))/*#@CacheRead#*/
#else
		else if(CACHE_READ_EN)/*#@CacheRead#*/
#endif
        {

#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
			if(TRUE == Fdl_ED3RowAnalyze(dRowOp,dRowOpNext,0))//1、next addr = current addr+1,form cache op;
			{
				bLunOPMode2nd = CACEH_OP_MODE;//support cache read op
			}
#else
			if(((dRowOp+1) == dRowOpNext))//1、next addr = current addr+1,form cache op;
			{
				bLunOPMode2nd = CACEH_OP_MODE;//support cache read op
			}
			else if((dRowOp&PLANE_BIT_MASK_OF_ROW) == ((dRowOpNext - 1)&PLANE_BIT_MASK_OF_ROW))/*必须保证在同一个Plane组内部*/
			{
				U08 bPln = 0;
				
				for(bPln = (PLANE_OF_LUN-1); bPln>=0; bPln--) //2、Mutiplane Op before, (next addr-1) and current addr is Multipln op, form cache op
				{
					if((dRowOpNext+(bPln*ROW_GAP_OF_PLANE)) == (dRowOp+1))
					{
						bLunOPMode2nd = CACEH_OP_MODE;
						break;
					}
					if(bPln == 0)
					{
						break;
					}
				}
			}
#endif
        }
    }
	
    tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1] = bLunOPMode2nd;//next lun address op mode record in bNextAddrOPMode[Lun+1]

	HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);
    /*****************************send flash cmd previously*********************************/
    if((tChTaskQ->bFirstFlag & 0x01) == 0x00)//if the chan send cmd firstly,send all cmd previouly
	{
		HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1); //set ud,length and AttachSize set once is ok

		if((tChTaskQ->bFirstFlag & 0x10) != 0x10)//该通道前面没有预发,这里要预发
		{
			Fdl_CQPreReadMultiCe(tAddr1st.bChan,tChTaskQ);
		}
		
#ifdef __TAB_CACHE_RD_OFF__
		if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))/*#@CacheRead#*/
#else
		if(CACHE_READ_EN)/*#@CacheRead#*/
#endif

        {
			HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_CACHE_READ, FIO_PF_PASS_NULL);
			Fdl_CQPreReadMultiPlaneCacheCmd(tAddr1st.bChan,bLogLunOfChan,tChTaskQ);
        }
		tChTaskQ->bFirstFlag |= 0x01;
	}
    else if(tChTaskQ->bNextAddrOPMode[0] == NORMAL_OP_MODE)//in the same lun
	{

		if(	(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == CACEH_OP_MODE) &&/*Cache 操作*/
			(tChTaskQ->bCacheOpFlag[bLogLunOfChan] == 1))
		{
			if(tChTaskQ->bNextAddrOPMode[bLogLunOfChan+1] == NORMAL_OP_MODE)
			{
				HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA_FINAL);
				tChTaskQ->bCacheOpFlag[bLogLunOfChan] = 0;
			}
			else/*Cache Read*/
			{
				Fdl_CQPreReadMultiPlaneCacheCmd(tAddr1st.bChan,bLogLunOfChan,tChTaskQ);
			}
		}
		else/*Cache 模式被打断，要切到正常模式重发命令(00-30,00-31)*/
		{
			if(	(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == NORMAL_OP_MODE) ||
				((tChTaskQ->bLastAddrOPMode[bLogLunOfChan] == CACEH_OP_MODE) && 
				(tChTaskQ->bCacheOpFlag[bLogLunOfChan] == 0)))//bCacheOpFlag may be cleared by retry, read from flash array again
			{				
				dRowOp = tChTaskQ->tLunTask[bLogLunOfChan].dRow[tChTaskQ->tLunTask[bLogLunOfChan].wLunHead & (MAX_CHAN_TASK-1)] & PLANE_BIT_MASK_OF_ROW;
				HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA_FINAL);
				Fdl_CQPreReadMultiPln(tAddr1st.bChan,bLogLunOfChan,dRowOp,FL_CMD_READ2);
				tChTaskQ->bCacheOpFlag[bLogLunOfChan] = 0;

#ifdef __TAB_CACHE_RD_OFF__
				if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))/*#@CacheRead#*/
#else
				if(CACHE_READ_EN)/*#@CacheRead#*/
#endif
				{
					Fdl_CQPreReadMultiPlaneCacheCmd(tAddr1st.bChan,bLogLunOfChan,tChTaskQ);
				}
			}
		}
	}


    /*****************************start dma2 of current ofst address*********************************/
    if((U32)NULL == tFtlTask.dUdBuff)
    {
        dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs*tAddr1st.bUdSize;
    }
    else
    {
        dUDBuffAddr  = tFtlTask.dUdBuff + (dOfst & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bUdSize - ptClstAddr->m_bClstOffs*tAddr1st.bUdSize;
    }

    dDmaInfoAddr = tFtlTask.dEccStsBuff + (((dOfst & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bSecOfClst/DSECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;


	if(tChTaskQ->bLastAddrOPMode[bLogLunOfChan] != MULTI_CLST_OP_MODE)//in one page,no send check status
	{	


#ifdef __TAB_CACHE_RD_OFF__
		if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))/*#@CacheRead#-增加Cache读后,出数据之前只等C0*/
#else
		if(CACHE_READ_EN)/*#@CacheRead#-增加Cache读后,出数据之前只等C0*/
#endif
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_CACHE_READ, FIO_PF_PASS_NULL);
		}
		else
		{
			HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		}
	}

	SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay
	SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay

	HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);
	ReadClstStartDMA2ByCQ(	dOfst,
							dBuffIdx,
							ptClstAddr->m_bClstOffs,
							dUDBuffAddr,								
							dDmaInfoAddr,
							bJointMode,
							&tAddr1st);
                            
	if(tChTaskQ->wTaskCnt == tChTaskQ->bClstCntInPage)
    {
        HAL_CQCheckReadEnd(tAddr1st.bChan,bCheckHoldEn);
#ifdef __TAB_CACHE_RD_OFF__
		if(tFtlTask.dTaskType == TYPE_NCQ_READ)
#endif
		{
			HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
			HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
			HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA_FINAL);
			HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		}
	}

	HAL_CQGenerateInt(tAddr1st.bChan, 0, FUNC_DISABLE);

    /*****************************analyse ftl task next ofst address*********************************/
    tChTaskQ->tLunTask[bLogLunOfChan].wLunHead += tChTaskQ->bClstCntInPage;
    tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt -= tChTaskQ->bClstCntInPage;

    return TRUE;
}


#endif



U8 TaskConvertReadLittleDataToFlash(U8 bChanPhy)
{
    U8 bChipPhy,bLunOfChip;
    U32 dRowAddr,dColAddr = 0;
    U16 wEcc_Parity = 0;

    U8 bUserDataEn = SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN;
    SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN = 0;

    T_CHAN_TASK_QUEUE *tChTaskQ     = &tChTaskQueue[bChanPhy];
    U16 wOfst = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    T_FLASH_PHY_ADDR *ptPageAddr    = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOfst]);
    bChipPhy = CHIPES(bChanPhy,ptPageAddr->m_bLunOfChan / LUN_OF_CHIP);

    U16 wDma2Len    =SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN;
    U08 bIdxOfCopies= tFtlTask.bArgsArr[0];
    U32 dDmaInfoAddr= tFtlTask.dEccStsBuff;

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
    U16 wDma2LenWithParity =  wDma2Len+wEcc_Parity;

    bLunOfChip  = ptPageAddr->m_bLunOfChan % LUN_OF_CHIP;
    dRowAddr    = ptPageAddr->m_dRowAddr + bLunOfChip * ROW_GAP_OF_LUN;

    HAL_CQSetRndSeed(bChanPhy,dRowAddr,INVALID_U16);

    HAL_CQChipSelect(bChanPhy,bChipPhy);
	HAL_CQCheckIOStatus(bChanPhy, bLunOfChip, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);

    FL_CMD_CQ_SLC_MODE(bChanPhy);
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x50;
    HAL_CQSendFlashCmd(bChanPhy,FL_CMD_READ1);
    HAL_CQSend5Addr(bChanPhy,dColAddr,dRowAddr);
    HAL_CQSendFlashCmd(bChanPhy,FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChanPhy, bLunOfChip, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);

	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay
	SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xFF;				// delay

    dColAddr =  0;
    dColAddr += (bIdxOfCopies*wDma2LenWithParity);
    HAL_CQSendRndAddr(bChanPhy,dColAddr,dRowAddr);
    HAL_CQStartDMA2(bChanPhy, tFtlTask.dHandleBuff, DMA2_RD_TO_BUFF,0,0, 1, JOINT_NONE, dDmaInfoAddr);

    SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
    HAL_CQWaitDone(bChanPhy);
	
	if((SG_DMA2_CH(bChanPhy)->rSTATUS.bits.ECC_FAIL_FLAG == 1)||(SG_DMA2_CH(bChanPhy)->rSTATUS.bits.CRC_FAIL_FLAG == 1))
	{	
		U16 wExptInfor = FDL_LittleDataRetry(bChanPhy,bChipPhy,tFtlTask.dHandleBuff,dRowAddr,dColAddr,dDmaInfoAddr);
		tFtlTask.FTLTaskExceptionHandler(0,0,wExptInfor);
	}
	
    SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN = bUserDataEn;
    return TRUE;
}

#ifdef __FDL_HW_VIRTUL_CLST__
U8 TaskConvert4KReadClstToBuff(U8 bChanPhy)
{
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
    U32 dOfst = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    T_FLASH_PHY_ADDR *ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfst & (tFtlTask.wContiAddrMax - 1)]);
    T_ADDR_INFO tAddr1st;
    U08 bChip; 
	U08 bLunOfChan;
    U08 bChipPhy;
    U32 dDmaInfoAddr;
    U32 dUDBuffAddr;
    U32 dBuffIdx;
    U32 dRowOpNext;
    U16 wColOpNext;
    U16 wColOp=0;
    U08 bLogLunOfChan = ptClstAddr->m_bLunOfChan;
    U08 bCheckHoldEn = TRUE;//decide to check hold or not
    Joint_Mode bJointMode = JOINT_NONE;

    if(tFtlTask.dTaskType == TYPE_NCQ_READ_4K)
    {
        bJointMode = JOINT_21;
    }
    else if (TYPE_READ_TO_BUFFER_4K == tFtlTask.dTaskType)
    {
        bJointMode = tFtlTask.bJointMode;
    }

	if(ptClstAddr->m_bChan == 0xFF)
	{
		U8 bRet = TaskVirtualClstConvert(bChanPhy,dOfst,bJointMode,ptClstAddr);
		return bRet;
	}

	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);

	if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
    {
        dBuffIdx = tFtlTask.pwBuffIdx[dOfst];
    }
    else
    {
        dBuffIdx  = dOfst * tAddr1st.bSecOfClst;
    }

    /*********************************************第一次发送****************************************/
#ifdef FDL_4K_CACHE_RAND_READ_ON
    if (0 == tChTaskQ->bFirstFlag)
    {
        tChTaskQ->bFirstFlag = 1;
        HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
                if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0))
                {
                    HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);
                    dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[0];

                	HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                    Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA_FINAL);

					HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                    Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                    HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ2);

                }
            }
        }

        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
                if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt > 1))
                {
                    HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);
                    dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[1];
					HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                    HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA);
                }
                else
                {
                    tChTaskQ->bStopFlag[bLunOfChan] = 1;			//如果这个LUN内没有第二个任务了，那么默认他已经任务为暂时还没分配，置起stop标志位，以便于后面重新预发
                }
            }
        }
    }

    //*******************************************************************************************************************
    else 
    {
        bLastLun = tChTaskQ->bLastLun;
        dRowOpNext = tChTaskQ->tLunTask[bLastLun].dRow[(tChTaskQ->tLunTask[bLastLun].wLunHead+1) & (MAX_CHAN_TASK-1)];
        bLastChip = CHIPES((tAddr1st.bChan), bLastLun);

        if(tChTaskQ->bStopFlag[bLastLun] == 0)
        {
            if(tChTaskQ->tLunTask[bLastLun].bRowCnt != 0)
            {
                HAL_CQChipSelect(tAddr1st.bChan,bLastChip);//片选到当前CE
                Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOpNext);
                HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOpNext);
                HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA);

            }
            if(tChTaskQ->tLunTask[bLastLun].bRowCnt == 1)   
            {
                tChTaskQ->bStopFlag[bLastLun] = 1;		//如果这个LUN内没有第二个任务了，那么默认他已经任务为暂时还没分配，置起stop标志位，以便于后面重新预发	
                tChTaskQ->bPreSendFlag[bLastLun] = 1;	//由于是在中间断了的，所以，一级缓存里还有数据，下面的处理应该直出数据，而不需要发命令
            }
        }
        if(tChTaskQ->bStopFlag[bLogLunOfChan] == 1)
        {
            if (tChTaskQ->bPreSendFlag[bLogLunOfChan] != 1)//判断断了之后是否将断之后的数据读出
            {
                tChTaskQ->bPreSendFlag[bLogLunOfChan] = 0;
                dRowOp = tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead) & (MAX_CHAN_TASK-1)];
                dRowOpNext = tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead+1) & (MAX_CHAN_TASK-1)];
                if(tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt != 0)	//这个LUN断了之后又重新有任务的话，那么再一次进行预发
                {
                
                    HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
                    HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
					Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                    HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ2);
					
                    if(tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > 1)
                    {                 
                        HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
                        HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                        HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                        HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOpNext);
                        HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA);
						
                        tChTaskQ->bStopFlag[bLogLunOfChan] = 0; //如果任务有两个及以上的话，那么我们则进入正常的流程，将停住的标志位置起
                    }
                }
            }
            else
            {
                tChTaskQ->bPreSendFlag[bLogLunOfChan] = 0;//将要读断了之后的数据，所以，将这个标志位置起
            }
        }
    }
#else 
    for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
    {
        bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
        bLunOfChan = bChip;
			HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1);
        if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0) && (tChTaskQ->bPreSendFlag[bLunOfChan] == 0))
        {
            HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);
#ifdef FDL_4K_SNAP_READ_EN
            wColOpNext = (tChTaskQ->tLunTask[bLunOfChan].bClstOff[(tChTaskQ->tLunTask[bLunOfChan].wLunHead) & (MAX_CHAN_TASK - 1)]) * ((tAddr1st.bSecOfClst) / SECT_OF_DMA);
        	if(ptClstAddr->m_bDataType == TYPE_LOGIC)
			{
            	wColOp = DATA_COL_ADDR(wColOpNext);
			}
			else
			{
				wColOp = TBLE_COL_ADDR(wColOpNext);
			}
#endif
            dRowOpNext = tChTaskQ->tLunTask[bLunOfChan].dRow[(tChTaskQ->tLunTask[bLunOfChan].wLunHead) & (MAX_CHAN_TASK-1)];

            if(SNAP_READ_EN == 0)/*不支持Snap Read*/
			{
				Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,(SNAP_READ_EN + 1),FL_CMD_READ2);
			}
			else
			{
#if (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
				Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,(SNAP_READ_EN + 1),FL_CMD_READ2);
#elif (NAND_FLASH != MIC_M16)   ////注N18模式，QLC支持半页读，TLC M16不支持半页读
				Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,SNAP_READ_EN,FL_CMD_SNAP_READ);
#else
				if((dRowOpNext >>24) == SLC_MODE)
				{
					Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,SNAP_READ_EN,FL_CMD_SNAP_READ);
				}
				else
				{
					Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,(SNAP_READ_EN + 1),FL_CMD_READ2);
				}
#endif
			}
            tChTaskQ->bPreSendFlag[bLunOfChan] = 1;
        }
    }
#endif
    /*****************************start dma2 of current ofst address*********************************/

    if((U32)NULL == tFtlTask.dUdBuff)
    {
        dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
    }
    else
    {
        dUDBuffAddr  = tFtlTask.dUdBuff + ((dOfst & (tFtlTask.wContiAddrMax - 1)) & (tFtlTask.wContiAddrMax - 1)) * tAddr1st.bUdSize - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
    }
    dDmaInfoAddr = tFtlTask.dEccStsBuff+(((dOfst & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bSecOfClst/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;
	HAL_CQCfgLogicNum(tAddr1st.bChan,dOfst);
    HAL_CQCheckHold(1,bJointMode,tAddr1st.bChan, dBuffIdx, tAddr1st.bSecOfClst,bCheckHoldEn);
    HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
    tChTaskQ->bLastLun = bLogLunOfChan;
	HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS,FIO_RB_READY_IO, FIO_PF_PASS_NULL);/*mark*/
	HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_EXIT_CHK_IOSTA);

	if(SNAP_READ_EN != 1)/*Two Snap和非Snap RD需要发05-E0*/
	{
		HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);
	}
    else
	{
#if ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B))
		HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);//B27B 需要发送列地址2X2 CE2才不会发生重读问题
#endif
	}
	ReadClstStartDMA2ByCQ(	dOfst,
							dBuffIdx,
							ptClstAddr->m_bClstOffs,
							dUDBuffAddr,								
							dDmaInfoAddr,
							bJointMode,																
							&tAddr1st);
    tChTaskQ->bPreSendFlag[bLogLunOfChan] = 0;//******************************************
	if(tChTaskQ->wTaskCnt == tChTaskQ->bClstCntInPage)
    {

        HAL_CQCheckReadEnd(tAddr1st.bChan,bCheckHoldEn);
    }

    /*****************************analyse ftl task next ofst address*********************************/
    tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
    tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;
    return TRUE;
}
#else
U8 TaskConvert4KReadClstToBuff(U8 bChanPhy)
{
    T_CHAN_TASK_QUEUE *tChTaskQ = &tChTaskQueue[bChanPhy];
    U32 dOfst = tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)];
    T_FLASH_PHY_ADDR *ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfst & (tFtlTask.wContiAddrMax - 1)]);
    T_ADDR_INFO tAddr1st;
    U08 bChip; 
	U08 bLunOfChan;
    U08 bChipPhy;
    U32 dDmaInfoAddr;
    U32 dUDBuffAddr;
    U32 dLogNum;
    U32 dBuffIdx;
    U32 dRowOpNext;
    U16 wColOp;
    U16 wColOpNext;
    U08 bLogLunOfChan = ptClstAddr->m_bLunOfChan;
    U08 bCheckHoldEn = TRUE;//decide to check hold or not
    Joint_Mode bJointMode = JOINT_NONE;
    if(tFtlTask.dTaskType == TYPE_NCQ_READ_4K)
    {
        bJointMode = JOINT_21;
    }
    else if (TYPE_READ_TO_BUFFER_4K == tFtlTask.dTaskType)
    {
        bJointMode = tFtlTask.bJointMode;
    }
	Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
	if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
    {
        dBuffIdx = tFtlTask.pwBuffIdx[dOfst];
    }
    else
    {
        dBuffIdx  = dOfst * tAddr1st.bSecOfClst;
    }
    dLogNum = dOfst;
    HAL_CQCfgLogicNum(tAddr1st.bChan,dLogNum);
    HAL_CQCheckHold(1,bJointMode,tAddr1st.bChan, dBuffIdx, tAddr1st.bSecOfClst,bCheckHoldEn);

    /*********************************************第一次发送****************************************/
#ifdef FDL_4K_CACHE_RAND_READ_ON
    if (0 == tChTaskQ->bFirstFlag)
    {
        tChTaskQ->bFirstFlag = 1;
        HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
                if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0))
                {
                    HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);
                    dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[0];
                	HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                    Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOp);                       
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA_FINAL);
					HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                    Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOp);                    
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                    HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ2);
                }
            }
        }
        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
        {
            bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
                if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt > 1))
                {
                    HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);
                    dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[1];
					HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                    HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA);
                }
                else
                {
                    tChTaskQ->bStopFlag[bLunOfChan] = 1;			//如果这个LUN内没有第二个任务了，那么默认他已经任务为暂时还没分配，置起stop标志位，以便于后面重新预发
                }
            }
        }
    }

    //*******************************************************************************************************************
    else 
    {
        bLastLun = tChTaskQ->bLastLun;
        dRowOpNext = tChTaskQ->tLunTask[bLastLun].dRow[(tChTaskQ->tLunTask[bLastLun].wLunHead+1) & (MAX_CHAN_TASK-1)];
        bLastChip = CHIPES((tAddr1st.bChan), bLastLun);

        if(tChTaskQ->bStopFlag[bLastLun] == 0)
        {
            if(tChTaskQ->tLunTask[bLastLun].bRowCnt != 0)
            {
                HAL_CQChipSelect(tAddr1st.bChan,bLastChip);//片选到当前CE
                Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOpNext);
                HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOpNext);
                HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA);
            }
            if(tChTaskQ->tLunTask[bLastLun].bRowCnt == 1)   
            {
                tChTaskQ->bStopFlag[bLastLun] = 1;		//如果这个LUN内没有第二个任务了，那么默认他已经任务为暂时还没分配，置起stop标志位，以便于后面重新预发	
                tChTaskQ->bPreSendFlag[bLastLun] = 1;	//由于是在中间断了的，所以，一级缓存里还有数据，下面的处理应该直出数据，而不需要发命令
            }
        }
        if(tChTaskQ->bStopFlag[bLogLunOfChan] == 1)
        {
            if (tChTaskQ->bPreSendFlag[bLogLunOfChan] != 1)//判断断了之后是否将断之后的数据读出
            {
                tChTaskQ->bPreSendFlag[bLogLunOfChan] = 0;
                dRowOp = tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead) & (MAX_CHAN_TASK-1)];
                dRowOpNext = tChTaskQ->tLunTask[bLogLunOfChan].dRow[(tChTaskQ->tLunTask[bLogLunOfChan].wLunHead+1) & (MAX_CHAN_TASK-1)];
                if(tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt != 0)	//这个LUN断了之后又重新有任务的话，那么再一次进行预发
                {
                    HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
                    HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
					Fdl_ReadPreSendCmd(tAddr1st.bChan,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                    HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOp);
                    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ2);
                    if(tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt > 1)
                    {                 
                        HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE
                        HAL_CQCheckIOStatus(tAddr1st.bChan, bLunOfChan, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
                        HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_READ1);
                        HAL_CQSend5Addr(tAddr1st.bChan,0,dRowOpNext);
                        HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_CACHE_REDA);
                        tChTaskQ->bStopFlag[bLogLunOfChan] = 0; //如果任务有两个及以上的话，那么我们则进入正常的流程，将停住的标志位置起
                    }
                }
            }
            else
            {
                tChTaskQ->bPreSendFlag[bLogLunOfChan] = 0;//将要读断了之后的数据，所以，将这个标志位置起
            }
        }
    }
#else 
    for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
    {
        bChipPhy  = CHIPES(tAddr1st.bChan,bChip);
        bLunOfChan = bChip;
		if (0 == tChTaskQ->bFirstFlag)	
		{
			tChTaskQ->bFirstFlag = 1;
			HAL_CQSetUd(tAddr1st.bChan,tFtlTask.dUdBuff,tAddr1st.bSecOfClst,tAddr1st.bUdSize,1);
		}
        if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0) && (tChTaskQ->bPreSendFlag[bLunOfChan] == 0))
        {
            HAL_CQChipSelect(tAddr1st.bChan,bChipPhy);
#ifdef FDL_4K_SNAP_READ_EN
            wColOpNext = (tChTaskQ->tLunTask[bLunOfChan].bClstOff[(tChTaskQ->tLunTask[bLunOfChan].wLunHead) & (MAX_CHAN_TASK - 1)]) * ((tAddr1st.bSecOfClst) / SECT_OF_DMA);
        	if(ptClstAddr->m_bDataType == TYPE_LOGIC)
			{
            	wColOp = DATA_COL_ADDR(wColOpNext);
			}
			else
			{
				wColOp = TBLE_COL_ADDR(wColOpNext);
			}
#endif
            dRowOpNext = tChTaskQ->tLunTask[bLunOfChan].dRow[(tChTaskQ->tLunTask[bLunOfChan].wLunHead) & (MAX_CHAN_TASK-1)];

            if(SNAP_READ_EN == 0)/*不支持Snap Read*/
			{
				Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,(SNAP_READ_EN + 1),FL_CMD_READ2);
			}
			else
			{
#if (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
				Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,(SNAP_READ_EN + 1),FL_CMD_READ2);
#elif (NAND_FLASH != MIC_M16)   ////注N18模式，QLC支持半页读，TLC M16不支持半页读
				Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,SNAP_READ_EN,FL_CMD_SNAP_READ);
#else
				if((dRowOpNext >>24) == SLC_MODE)
				{
					Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,SNAP_READ_EN,FL_CMD_SNAP_READ);
				}
				else
				{
					Fdl_CQPreReadFixPln(tAddr1st.bChan,bLunOfChan,dRowOpNext,wColOp,(SNAP_READ_EN + 1),FL_CMD_READ2);
				}
#endif
			}
            tChTaskQ->bPreSendFlag[bLunOfChan] = 1;
        }
    }
#endif
    /*****************************start dma2 of current ofst address*********************************/

    if((U32)NULL == tFtlTask.dUdBuff)
    {
        dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
    }
    else
    {
        dUDBuffAddr  = tFtlTask.dUdBuff + ((dOfst & (tFtlTask.wContiAddrMax - 1)) & (tFtlTask.wContiAddrMax - 1)) * tAddr1st.bUdSize - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
    }

    dDmaInfoAddr = tFtlTask.dEccStsBuff+(((dOfst & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bSecOfClst/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;

    HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE

    tChTaskQ->bLastLun = bLogLunOfChan;
    HAL_CQCheckIOStatus(tAddr1st.bChan, 0, IO_1_PASS,FIO_RB_READY_IO, FIO_PF_PASS_NULL);/*mark*/
    HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_EXIT_CHK_IOSTA);

    if(SNAP_READ_EN != 1)/*Two Snap和非Snap RD需要发05-E0*/
    {
    	HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);
    }
    else
    {
#if ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B))
	HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);//B27B 需要发送列地址2X2 CE2才不会发生重读问题
#endif
    }
#if 0
    ReadClstStartDMA2ByCQ(tAddr1st.bChan, tAddr1st.bChip, dOfst, ptClstAddr->m_bClstOffs,
                          tAddr1st.bSecOfClst, dBuffIdx, bJointMode, dDmaInfoAddr, dUDBuffAddr,
                          tAddr1st.dRow);  //CQ Num=16
#else
	ReadClstStartDMA2ByCQ(	dOfst,
							dBuffIdx,
							ptClstAddr->m_bClstOffs,
							dUDBuffAddr,								
							dDmaInfoAddr,
							bJointMode,																
							&tAddr1st);

#endif

    tChTaskQ->bPreSendFlag[bLogLunOfChan] = 0;//******************************************

	if(tChTaskQ->wTaskCnt == tChTaskQ->bClstCntInPage)
    {
        HAL_CQCheckReadEnd(tAddr1st.bChan,bCheckHoldEn);
    }

    /*****************************analyse ftl task next ofst address*********************************/
    tChTaskQ->tLunTask[bLogLunOfChan].wLunHead++;
    tChTaskQ->tLunTask[bLogLunOfChan].bRowCnt--;

    //HAL_CQGenerateInt(tAddr1st.bChan, 0, FUNC_DISABLE);
    return TRUE;
}
#endif

void Convert4KPreRead(U08 bOfts)
{
	T_ADDR_INFO tAddr1st;
	Joint_Mode bJointMode = JOINT_21;
	U32 dBuffIdx;
	U32 dUDBuffAddr;
	U32 dDmaInfoAddr;
	T_FLASH_PHY_ADDR *ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[bOfts & (tFtlTask.wContiAddrMax - 1)]);
	if(ptClstAddr->m_bChan != 0xff)
	{
		Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
		dBuffIdx = tFtlTask.pwBuffIdx[bOfts];

		SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_TOTAL_SECT << R_CQ_REG_BIT) | tAddr1st.bSecOfClst;
		if((U32)NULL == tFtlTask.dUdBuff)
		{
			dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
		}
		else
		{
			dUDBuffAddr  = tFtlTask.dUdBuff + ((bOfts & (tFtlTask.wContiAddrMax - 1)) & (tFtlTask.wContiAddrMax - 1)) * tAddr1st.bUdSize - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
		}
		dDmaInfoAddr = tFtlTask.dEccStsBuff+(((bOfts & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bSecOfClst/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;

		HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE

		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);/*mark*/
		HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_EXIT_CHK_IOSTA);
		if(SNAP_READ_EN != 1)/*Two Plane Snap和非Snap RD需要发05-E0*/
		{
			HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);
		}
		else
		{
#if ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B))
			HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);//B27B 需要发送列地址2X2 CE2才不会发生重读问题
#endif
		}

		ReadClstStartDMA2ByCQ(	bOfts,
								dBuffIdx,
								ptClstAddr->m_bClstOffs,
								dUDBuffAddr,								
								dDmaInfoAddr,
								bJointMode, 															
								&tAddr1st);

	}
}

void Convert4KPreReadTable(U08 bOfts)
{
	T_ADDR_INFO tAddr1st;
	Joint_Mode bJointMode = JOINT_NONE;
	U32 dBuffIdx;
	U32 dUDBuffAddr;
	U32 dDmaInfoAddr;
	T_FLASH_PHY_ADDR *ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[bOfts & (tFtlTask.wContiAddrMax - 1)]);
	if(ptClstAddr->m_bChan != 0xff)
	{
		Fdl_ConvertAddrInfo(&tAddr1st,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
	    dBuffIdx = tFtlTask.pwBuffIdx[bOfts];
		SG_FC(tAddr1st.bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_TOTAL_SECT << R_CQ_REG_BIT) | tAddr1st.bSecOfClst;

		if((U32)NULL == tFtlTask.dUdBuff)
		{
			dUDBuffAddr  = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr1st.bUdSize*tAddr1st.bChan - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
		}
		else
		{
			dUDBuffAddr  = tFtlTask.dUdBuff + bOfts * tAddr1st.bUdSize - ptClstAddr->m_bClstOffs * tAddr1st.bUdSize;
		}
		dDmaInfoAddr = tFtlTask.dEccStsBuff+(((bOfts & (tFtlTask.wContiAddrMax - 1))*tAddr1st.bSecOfClst/SECT_OF_DMA)*2) % pFdlParam.dEccInfoBuffSize;

		HAL_CQChipSelect(tAddr1st.bChan,tAddr1st.bChip);//片选到当前CE

		HAL_CQCheckIOStatus(tAddr1st.bChan, tAddr1st.bLun, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);/*mark*/
		HAL_CQSendFlashCmd(tAddr1st.bChan,FL_CMD_EXIT_CHK_IOSTA);
		
		if(SNAP_READ_EN != 1)/*Two Snap和非Snap RD需要发05-E0*/
		{
			HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);
		}
		else
		{
#if ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B))
			HAL_CQSendRndAddr(tAddr1st.bChan,tAddr1st.wCol,tAddr1st.dRow);//B27B 需要发送列地址2X2 CE2才不会发生重读问题
#endif
		}
		ReadClstStartDMA2ByCQ(	bOfts,
								dBuffIdx,
								ptClstAddr->m_bClstOffs,
								dUDBuffAddr,								
								dDmaInfoAddr,
								bJointMode, 															
								&tAddr1st);

	}
}

void HandlePreReadExCeption(T_FLASH_PHY_ADDR *ptClstAddr, U08 bChanNum, U08 bClstCntCur)
{
	U08 i = 0;
	U08 bChan;
	U08 bFailChanCnt = 0;
	U08 bFailChan[4]; 
	U32 *bFailDmaCnt = NULL;
	U08 bRestartCQEn;
	
	U08 bPhyChan = 0;
	for(i = 0; i <	MAX_CHAN_COUNT; i++)
	{	
		if((1 << i) & bChanNum)
		{
			bPhyChan = CHANS(i);		
			SG_FC(bPhyChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;						
			HAL_CQWaitDone(bPhyChan);
		}
	}

	#ifdef __PRE_4K_READ_ENABLE__
	for(i = 0;i < CHAN_MAX_CNT;i++)
	{
		bChan = CHANS(i);	
		if((SG_DMA2_CH(bChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1)||(SG_DMA2_CH(bChan)->rSTATUS.bits.CRC_FAIL_FLAG == 1))
		{
			Fdl_ClearExceptionState(bChan);
			bFailChan[bFailChanCnt] = bChan;
			bFailChanCnt++;
		}
	}
	
	if(bFailChanCnt)
	{
		PRINT_TXT("PreR_Retry");
		for(i = 0; i <  bFailChanCnt; i++)
		{
			bFailDmaCnt =(U32*) FDL_PreRead4KRetryGetInfo(bFailChan[i]);
		}
		while(1)
		{
			Retry_MultiDMA(pFdlParam.tReadFailCtrl.m_Fail_ReRdCnt,READ_RETRY_SET_OPTION_MAX, READ_RETRY_ARC_MAX);
			if(((*bFailDmaCnt)== 0) && ((*(bFailDmaCnt+1)) == 0) && ((*(bFailDmaCnt+2)) == 0) && ((*(bFailDmaCnt+3)) == 0))
			{	
				break;
			}
		}				

		if(ptClstAddr != NULL)
		{
			bRestartCQEn = 1;
			tFtlTask.wLen = bClstCntCur;
		}
		else
		{
			bRestartCQEn = 0;
		}
		tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
		Retry_PreRestartCQ(bRestartCQEn);
	}
	#endif
}


