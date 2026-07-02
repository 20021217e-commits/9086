#include "fdl_common.h"
#include "hal.h"
#include "fdl_debug.h"
#include "flash_driver_struct.h"
#include "fdl_task_op.h"
#include "uitility.h"
#include "flash_op_cmd.h"
#include "fdl_read_retry.h"
#include "hal_adm.h"
#include "hal_define.h"
#include "hal_pmcp.h"
#ifdef __BADBLK_BINDMAP__
T_FDL_BADBLK_BINDMAP_DATA *ptBind;
U08 bPhy2LogMapChan[4];
#endif

U8 bCqTimeoutFlag[4];
U8 AlgScan_CheckCEValid(U08 bChNum,U08 bChipNum)
{
	U16 wColAddr = 0;
	U32 dRowAddr = 0;

	U32 dReg1 = SG_FC(bChNum)->rCHK_ST_RB_CNT;
	U32 dReg2 = SG_FC(bChNum)->rCHK_CQ_CTRL.dAll;

	SG_FC(bChNum)->rCHK_ST_RB_CNT = 200000;		//200Mhz下，1ms超时
	SG_FC(bChNum)->rCHK_CQ_CTRL.bits.CHK_ST_FAIL_INT_EN = 1;
	__nds32__dsb();
	SG_FC(bChNum)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_INT_EN = 1;
	NVIC_EnableIRQ(IRQ_FC0_CQ+bChNum);
	SG_FC(bChNum)->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL = 1;

	// 通过发送读（0x00---0x30）命令，检测每个通道Ready&Busy状态的变化情况，来判断每个通道是否挂有NandFlash
	memset((void *)&bCqTimeoutFlag[0], 0x00, sizeof(bCqTimeoutFlag));

	HAL_CQWaitDone(bChNum);
	HAL_CQChipSelect(bChNum, bChipNum);
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0xa0;	// delay
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ1;
	HAL_CQSend5Addr(bChNum, wColAddr, dRowAddr);
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ2;
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ_STATUS;

	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x04;
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_0 | 0x60;
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_0 | 0x60;
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | 0x60;
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | 0x60;
	SG_FC(bChNum)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD | 0;

	while(HAL_CQGetIdleSpace(bChNum) < CQ_MAX_IDLE_SPACE);

	U8 bStatus = SG_FC(bChNum)->rRW_PULSE_DATA.bits.RW_DATA;
	LOG_PRINT("ch=%d, chip=%d,status=0x%x\n", bChNum, bChipNum, bStatus);

	SG_FC(bChNum)->rCHK_ST_RB_CNT = dReg1;
	SG_FC(bChNum)->rCHK_CQ_CTRL.dAll = dReg2;

	// 指定通道的CE上挂载有效FLASH时, bCEMap设置为1
	if(!bCqTimeoutFlag[bChNum])
	{
		LOG_PRINT("channel: [%d], chip: [%d] online\n", bChNum, bChipNum);
		return 1;
	}
	else
	{
		bCqTimeoutFlag[bChNum] = 0;//超时后一定要清掉
		return 0;
	}
}

void Fdl_ResetDMA2AndFC(U08 bChan)
{
	U08 bChanPhy = CHANS(bChan);
	T_REG_CFG *ptHwReg = &(pFdlParam.ptUdpFdl->m_tHwReg);

	U32 dBAD_COLUMN_CTRL = SG_FC(bChanPhy)->rBAD_COLUMN_CTRL.dAll;
	U32 dBADCOL_INFO_BADDR = SG_FC(bChanPhy)->rBADCOL_INFO_BADDR.bits.dVAL;
	U32 dDMA2_FILTER_ECC_CFG_BADDR = SG_FC(bChanPhy)->rDMA2_FILTER_ECC_CFG_BADDR;

	switch(bChanPhy)
	{
		case 0:
			SG_CPM->rSOFT_RST0.bits.FC0 = 0;
			SG_CPM->rSOFT_RST1.bits.ADM_DMA20 = 0;
			break;
		case 1:
			SG_CPM->rSOFT_RST0.bits.FC1 = 0;
			SG_CPM->rSOFT_RST1.bits.ADM_DMA21 = 0;
			break;
		case 2:
			SG_CPM->rSOFT_RST0.bits.FC2 = 0;
			SG_CPM->rSOFT_RST1.bits.ADM_DMA22 = 0;
			break;
		case 3:
			SG_CPM->rSOFT_RST0.bits.FC3 = 0;
			SG_CPM->rSOFT_RST1.bits.ADM_DMA23 = 0;
			break;
	}

	/*FC模块初始化*/
	SG_FC(bChanPhy)->rNF_CFG.dAll            = ptHwReg->FC_dNfCfg;
	SG_FC(bChanPhy)->rNF_CFG.bits.DDR_WR_FIFO_CTRL_EN = 0;
	SG_FC(bChanPhy)->rFIO_SPOP_CTRL.dAll     = ptHwReg->FC_dSpopCtrl;
	SG_FC(bChanPhy)->rDDR_TIMING_CFG.dAll    = ptHwReg->FC_dDDRTimingCfg;
	SG_FC(bChanPhy)->rCQ_SET_RD_CFG_DQS.dAll = ptHwReg->FC_dFCCqSetRdCfgDqs;
	SG_FC(bChanPhy)->rCQ_CTRL.dAll           = ptHwReg->FC_dCQCtrl;
	SG_FC(bChanPhy)->rBAD_COLUMN_CTRL.dAll   = ptHwReg->FC_dBadColCtrl;
	SG_FC(bChanPhy)->rBADCOL_INFO_BADDR.dAll = ptHwReg->FC_dBadColInfoBAddr;
	SG_FC(bChanPhy)->rALE_CLE_TIM_CFG.dAll   = ptHwReg->FC_dAleCleTimingCfg;
	SG_FC(bChanPhy)->rCHK_ST_RB_CNT          = (ptHwReg->FC_dCheckStsThrod*2);//0x47FFFF 23ms -> 46 ms
	SG_FC(bChanPhy)->rCHK_CQ_CTRL.dAll       = ptHwReg->FC_dCheckCqCtrl;
	SG_FC(bChanPhy)->rCQ_HOLD_CTRL.dAll      = ptHwReg->FC_dCQHoldCtrl;
	SG_FC(bChanPhy)->rRND_SEED.dAll          = ptHwReg->FC_dRandSeed;
	//hal初始值，不需要量产配置的参数
	SG_FC(bChanPhy)->rCHK_INT_ST.dAll  = 0;

	SG_FC(bChanPhy)->rBAD_COLUMN_CTRL.dAll 			= dBAD_COLUMN_CTRL;
	SG_FC(bChanPhy)->rBADCOL_INFO_BADDR.bits.dVAL 	= dBADCOL_INFO_BADDR;
	SG_FC(bChanPhy)->rDMA2_FILTER_ECC_CFG_BADDR 	= dDMA2_FILTER_ECC_CFG_BADDR;

	/*DMA2模块初始化*/
	SG_DMA2_CH(bChanPhy)->rLEN_CFG.dAll  = ptHwReg->ADM_dDma2LenCfg;
    SG_DMA2_CH(bChanPhy)->rECC_CFG.dAll  = ptHwReg->ADM_dDma2EccCfg;
    SG_DMA2_CH(bChanPhy)->rECC_CTRL.dAll = ptHwReg->ADM_dDma2EccCtrl;
	SG_DMA2_CH(bChanPhy)->rRND_CTRL.bAll = ptHwReg->ADM_dDma2RndCtrl;
	SG_DMA2_CH(bChanPhy)->rRAID_CTRL.dAll = ptHwReg->ADM_dDma2RaidCtrl;
//	//hal初始值，不需要量产配置的参数
	SG_DMA2_CH(bChanPhy)->rCTRL.bits.FIFO_DISABLE = 0;
	SG_DMA2_CH(bChanPhy)->rENABLE.bits.FC_FIFORDY_EN = 0;
	SG_DMA2_CH(bChanPhy)->rENABLE.bits.EN = 1;
	SG_DMA2_CH(bChanPhy)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChanPhy)->rENABLE.bits.SAVE_DMAINFO_EN = 0;
	SG_DMA2_CH(bChanPhy)->rMASK = 0;
	SG_DMA2_CH(bChanPhy)->rSTATUS.dAll = 0;

	HAL_SetOverFlowThresh(bChanPhy,0xFF,0x200);//后面由量产工具给定,rECC_CFG


#if 0
	SG_CPM->rSOFT_RST1.bits.ADM2LDPC = 0;
	SG_CPM->rSOFT_RST0.bits.LDEC	 = 0;
	if(SG_DMA2_CH(bChanPhy)->rECC_CTRL.bits.ECC_MOD_SEL == 1)//ldpc mode
	{
		//初始化完成后MemMtxA/MemMtxB/MemMtxP1/MemMtxP2里的内容可以丢掉，上层设计时可考虑初始化之前临时从nand读出初始化表
		HAL_LDPC_Init(SG_DMA2_CH(bChanPhy)->rECC_CTRL.bits.LDPC_MOD, ptHwReg->LDPC_dMemMtxA, ptHwReg->LDPC_dMemMtxB, ptHwReg->LDPC_dMemMtxP1, ptHwReg->LDPC_dMemMtxP2);
	}
#endif
}


void Fdl_ResetDMA2(U08 bChanPhy)
{
	U08 bChan = 0;
	Joint_Mode JointMode = HAL_ADMGetJointMode(bChanPhy);

	switch(bChanPhy)
	{
		case 0:
			SG_CPM->rSOFT_RST1.bits.ADM_DMA20 = 0;
			break;
		case 1:
			SG_CPM->rSOFT_RST1.bits.ADM_DMA21 = 0;
			break;
		case 2:
			SG_CPM->rSOFT_RST1.bits.ADM_DMA22 = 0;
			break;
		case 3:
			SG_CPM->rSOFT_RST1.bits.ADM_DMA23 = 0;
			break;
	}
	
	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		if(CHANS(bChan) != bChanPhy)
		{
//			PRINT_HEX("bChan = ",bChan);
			break;
		}
	}
	
	MemoryCopy((U08 *)SG_DMA2_CH(bChanPhy),(U08 *)SG_DMA2_CH(CHANS(bChan)),sizeof(SG_DMA2_TypeDef));
	SG_DMA2_CH(bChanPhy)->rCTRL.bits.JOINT_MODE = JointMode;
}

// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司
//
//  文件名称：  fdl_common.c
//  文件标识：
//  内容摘要：  实现FDL内部公共的函数
//  其它说明：  !!!函数名需以"Fdl_"前缀命名函数，以区分以"FDL_"开头的对外接口!!!
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  Johnson
//  完成日期：  2017年01月17日
//
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容
//  修改日期：
//  版 本 号：
//  修 改 人：
//  修改内容：
// *****************************************************************************


#ifdef __BADBLK_BINDMAP__   //BadBlkBindMap
U32 Fdl_GetBadMapAddr(U32 dRowAddr,U08 bChan,U08 bLunOfChan)
{
    if ((ptBind->wBindBlkLen)&&(dRowAddr!=0xFFFFFFFFU))  //需要把块号的物理地址换掉
	{
		U32  dHRowAddr = dRowAddr & 0xff000000;
		U32  dLRowAddr = dRowAddr & 0x00ffffff;

		if (dLRowAddr >= ptBind->wBindBlkBasePhyAddr)
		{
			if (dLRowAddr>=(((U32)(BLOCK_OF_LUN+ptBind->wBindBlkLen) / PLANE_OF_LUN) * ROW_GAP_OF_BLOCK))
			{
				PRINT_TXT("E:merr0");
				while(1);
			}

			U8 bLogChan = bPhy2LogMapChan[bChan];
			U16 wBlk = (dLRowAddr - ptBind->wBindBlkBasePhyAddr)/ROW_GAP_OF_BLOCK;

			U08 bPln = GET_PLANE_NUM(dLRowAddr); //获取Plane号  ---这个地方还有Bug需要获取这个参数
			U16 *pBindMapBLK = ptBind->wBuff + ptBind->wBindBlkAddr[bLogChan][bLunOfChan]; //块号的基地址
			pBindMapBLK +=(wBlk*PLANE_OF_LUN);

			if (pBindMapBLK[bPln]>=ptBind->wBindBlk)
			{
				pBindMapBLK -=PLANE_OF_LUN;
				if (pBindMapBLK[bPln]>=ptBind->wBindBlk)
				{
					PRINT_TXT("E:merr1");
					while(1);
				}
			}

		    dRowAddr = (pBindMapBLK[bPln]/*/UDP_bPLANE_OF_LUN*/)*(U32)ROW_GAP_OF_BLOCK  | (dLRowAddr & (ROW_GAP_OF_BLOCK - 1))| dHRowAddr;
		}
	}
	return 	dRowAddr;
}
#endif


void Fdl_ConvertAddrInfo(T_ADDR_INFO *pAddr,U32 dRowAddrBuff,TASK_TYPE bTaskType)
{
	if(bTaskType == TYPE_ERASE_BLOCKS)
	{
		T_FLASH_PHY_BLK_ADDR *ptBlkAddr = (T_FLASH_PHY_BLK_ADDR *)dRowAddrBuff;
		pAddr->bChan  	= CHANS(ptBlkAddr->m_bChan);
		pAddr->bChip 	= PHYCHIP_OF_LOGLUN(pAddr->bChan, ptBlkAddr->m_bLunOfChan);
		pAddr->bLun  	= PHYLUN_OF_LOGLUN(pAddr->bChan, ptBlkAddr->m_bLunOfChan);
		pAddr->dRow   	= ptBlkAddr->m_wBlkAddr * ROW_OF_BLOCK+ pAddr->bLun * ROW_GAP_OF_LUN;
	}
	else
	{
		T_FLASH_PHY_ADDR *ptSrcAddr = (T_FLASH_PHY_ADDR *)dRowAddrBuff;
		pAddr->bChan  	= CHANS(ptSrcAddr->m_bChan);
		pAddr->bChip	= PHYCHIP_OF_LOGLUN(pAddr->bChan, ptSrcAddr->m_bLunOfChan);
		pAddr->bLun  	= PHYLUN_OF_LOGLUN(pAddr->bChan, ptSrcAddr->m_bLunOfChan);
		pAddr->dRow   	= ptSrcAddr->m_dRowAddr + pAddr->bLun * ROW_GAP_OF_LUN;

	    if(TYPE_TABLE == ptSrcAddr->m_bDataType)
	    {
			pAddr->wCol	   		= TBLE_COL_ADDR(ptSrcAddr->m_bClstOffs * DMA_OF_TABLE_CLST);
			pAddr->bUdSize     	= UD_LEN_OF_TBLE_CLST;
	        pAddr->bSecOfClst  	= SECT_OF_TBLE_CLST;
			pAddr->bClstOfPage 	= TBLE_CLST_OF_PAGE;
	    }
	    else
	    {
			pAddr->wCol	   		= DATA_COL_ADDR(ptSrcAddr->m_bClstOffs * DMA_OF_DATA_CLST);
			pAddr->bUdSize     	= UD_LEN_OF_DATA_CLST;
	        pAddr->bSecOfClst  	= SECT_OF_DATA_CLST;
			pAddr->bClstOfPage 	= DATA_CLST_OF_PAGE;
	    }
	}    
}


U16 Fdl_ConvertColAddr(U08 bDataType,U08 bFlashIdx)
{
	if(TYPE_TABLE == bDataType)
	{
		return (TBLE_COL_ADDR((bFlashIdx/SECT_OF_DMA)));
	}
	else
	{
		return (DATA_COL_ADDR((bFlashIdx/SECT_OF_DMA)));
	}
}
#ifdef __VER_TLC_PAGE__
void Fdl_ConvertAddrInfoVerTlc(T_ADDR_INFO *pAddr, U08 bChan,T_CHAN_TASK_QUEUE *tChTaskQ)
{
	U08 bLogLun  = (tChTaskQ->wOfstInTask[tChTaskQ->wTaskHead & (MAX_CHAN_TASK-1)]>>VER_TLC_CE_BIT)&0x0F;

	pAddr->bChan = bChan;
	pAddr->bChip = CHIPES(pAddr->bChan,bLogLun);
	pAddr->bLun  = 0;
	pAddr->dRow  = tChTaskQ->tLunTask[bLogLun].dRow[tChTaskQ->tLunTask[bLogLun].wLunHead & (MAX_CHAN_TASK-1)];


	if(TYPE_TABLE == tFtlTask.bDataType)
	{
		pAddr->wCol 		= TBLE_COL_ADDR(0);
		pAddr->bUdSize		= UD_LEN_OF_TBLE_CLST;
		pAddr->bSecOfClst	= SECT_OF_TBLE_CLST;
		pAddr->bClstOfPage	= TBLE_CLST_OF_PAGE;
	}
	else
	{
		pAddr->wCol 		= DATA_COL_ADDR(0);
		pAddr->bUdSize		= UD_LEN_OF_DATA_CLST;
		pAddr->bSecOfClst	= SECT_OF_DATA_CLST;
		pAddr->bClstOfPage	= DATA_CLST_OF_PAGE;
	}
}
#endif


U8 Fdl_AddrModeAnalysis(T_ADDR_INFO *pCurrAddr, T_ADDR_INFO *pNextAddr)
{
    U8 bMode = NORMAL_OP_MODE;

    if((pCurrAddr->bChip == pNextAddr->bChip) && (pCurrAddr->bLun == pNextAddr->bLun) && (pNextAddr->dRow == (pCurrAddr->dRow+ROW_GAP_OF_PLANE)))
    {
        if(MULTI_PLANE_EN && (PLANE_OF_LUN != 1))
        {
            if(((pNextAddr->dRow&(~((U32)(ROW_GAP_OF_PLANE-1))))%(ROW_GAP_OF_PLANE*PLANE_OF_LUN)))//下个块地址不在Plane0时
            {
                bMode =  MULTI_PLN_OP_MODE;//支持多Plane操作
            }
        }
    }
    else if(pCurrAddr->bChip != pNextAddr->bChip)
    {
        if(MULTI_CE_EN && (CHIP_OF_CHAN != 1))
        {
            bMode =  MULTI_CE_OP_MODE;//支持多CE操作
        }
    }
    else if(pCurrAddr->bLun != pNextAddr->bLun)
    {
        if(MULTI_LUN_EN && (LUN_OF_CHIP != 1))
        {
            bMode =  MULTI_LUN_OP_MODE;//支持多LUN操作
        }
    }
    else if((pCurrAddr->bChip == pNextAddr->bChip) && (pCurrAddr->bLun == pNextAddr->bLun) && (pNextAddr->dRow == pCurrAddr->dRow) )//列地址+1不对!!!!!
    {
         if(MULTI_CLST_EN)
         {
             bMode = MULTI_CLST_OP_MODE;//合并簇操作,只有读的时候才会有,擦写不可能进该判断
         }
    }
    else if((pCurrAddr->bChip == pNextAddr->bChip) && (pCurrAddr->bLun == pNextAddr->bLun) && (pNextAddr->dRow == pCurrAddr->dRow+1))
    {
        if((CACHE_READ_EN)||(CACHE_WR_EN))
        {
            bMode =  CACEH_OP_MODE;//支持Cache操作，注意:有可能误判
        }
    }

    return bMode;
}

void Fdl_VirtualChanTaskSetVal(U16 wOfst,U8 bDataType)
{
	U32 i;
    U32 bSetVal           = 0x00;
    U32 dBuffIdx          = 0x00;
    U08 bSecOfClst        = 0x00;
    U32 dDmaBuffAddr      = 0x00;
    Joint_Mode bJointMode = JOINT_NONE;
	T_FLASH_PHY_ADDR *ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOfst]);
	bSetVal = (U32)(ptClstAddr->m_bLunOfChan & 0x000000FF);
	bSetVal = (bSetVal<<24)|(bSetVal<<16)|(bSetVal<<8)|(bSetVal);
	
    if ( TYPE_TABLE == bDataType)
    {
        bSecOfClst  = SECT_OF_TBLE_CLST;
    }
    else
    {
        bSecOfClst  = SECT_OF_DATA_CLST;
    }

    dBuffIdx = wOfst*bSecOfClst;

    if((tFtlTask.dTaskType == TYPE_NCQ_READ) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K))
    {
		/*1、修改虚拟通道里面等DMA1传到指定位置由原来的超时改为死等*/
//        while(SG_DMA1->rINDEX < dBuffIdx);//联动读，等DMA1传输到指定Index
#if 0
        while(SG_DMA1->rINDEX < dBuffIdx);//联动读，等DMA1传输到指定Index
#else
        while((SG_DMA1->rINDEX + SG_ADM->rBUF_SIZE - bSecOfClst) <= dBuffIdx);
#endif
        bJointMode = JOINT_21;
        dBuffIdx = dBuffIdx%SG_ADM->rBUF_SIZE;
    }

    /************************计算Buff Index***************************/
    if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
    {
        dBuffIdx = tFtlTask.pwBuffIdx[wOfst];
        dDmaBuffAddr = tFtlTask.dHandleBuff + tFtlTask.pwBuffIdx[wOfst]*BYTE_OF_SEC;
    }
    else
    {		
		dBuffIdx = dBuffIdx;
        dDmaBuffAddr = tFtlTask.dHandleBuff + dBuffIdx*BYTE_OF_SEC;
    }
    /************************Set Buff 为指定的Value************************/
    if (NULL == tFtlTask.pMskBuff)
    {
        HAL_PMCPMemorySet(PMCP0,dDmaBuffAddr,bSetVal,bSecOfClst*BYTE_OF_SEC);

		if(bJointMode != JOINT_NONE)
    	{

	        if(bSecOfClst <= 16)
	        {
	            HAL_BFCSetBuffStatus(dBuffIdx,(U16)(((U32)0x1<<bSecOfClst)-1));
	        }
	        else
	        {
	            HAL_BFCSetBuffStatus(dBuffIdx,0xFFFF);
	            HAL_BFCSetBuffStatus((dBuffIdx+16),(U16)(((U32)0x1<<(bSecOfClst-16))-1));
	        }
    	}
    }
    else // 带Mask联动读的情况
    {
		U08 *pMaskVal;
		
		if ((tFtlTask.dTaskType == TYPE_NCQ_READ) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K)) // NCQ的联动读，只有头或尾可能有mask信息，
		{				
			if (wOfst == 0)//first
			{
				pMaskVal = &tFtlTask.pMskBuff[NCQ_READ_MASK_FISRT * MASK_BYTE_OF_CLST];
			}
			else if (wOfst == (tFtlTask.wLen-1))//last
			{
				pMaskVal = &tFtlTask.pMskBuff[NCQ_READ_MASK_END * MASK_BYTE_OF_CLST];
			}
			else//中间的若干
			{
				pMaskVal = &tFtlTask.pMskBuff[NCQ_READ_MASK_MID * MASK_BYTE_OF_CLST];
			}
		}
		else
		{
			pMaskVal = &tFtlTask.pMskBuff[wOfst * MASK_BYTE_OF_CLST];
		}
#ifdef __LAST_CLUST_MASK_OFF__
	    U32 j;
		for(i=0; i<MASK_BYTE_OF_CLST;i++)
		{
			if(pMaskVal[i] == 0)//无MASK的情况
			{
				HAL_PMCPMemorySet(PMCP0, (dDmaBuffAddr + i*bSecOfClst*SECTOR_SIZE), bSetVal, bSecOfClst*SECTOR_SIZE);

				 if(bJointMode != JOINT_NONE)
				 {
					  HAL_BFCSetBuffStatus(dBuffIdx,(U16)(((U32)0x1<<bSecOfClst)-1));
				 }
			}
			else				//有MASK的情形
			{
				for(j=0; j<bSecOfClst; j++)
				{
					U08 bBuffOfSet = i*8 + j;
					if((pMaskVal[i] & (0x1<<j)) == 0)
					{
						HAL_PMCPMemorySet(PMCP0, (dDmaBuffAddr + bBuffOfSet*SECTOR_SIZE), bSetVal, SECTOR_SIZE);

						if(bJointMode != JOINT_NONE)
						 {
							  HAL_BFCSetBuffStatusBit(dBuffIdx+ bBuffOfSet);
						 }
					}
				}
			}
		}
#else
		// memory填充，水位置起来
		for(i = 0; i < MASK_BYTE_OF_CLST*8; i++)
		{
			if((pMaskVal[i/8] & (0x01<<(i&7))) == 0)// 没有被mask, '1'表示被mask
			{
				HAL_PMCPMemorySet(PMCP0,(dDmaBuffAddr + i*BYTE_OF_SEC), bSetVal, SECTOR_SIZE);

				if(bJointMode != JOINT_NONE)
				{
					HAL_BFCSetBuffStatusBit(dBuffIdx+i);
				}
			}
		}
#endif
    }
}
void Fdl_PreVirtualChanTaskSetVal(U16 wOfst,U8 bDataType)
{			
    U32 bSetVal           = 0x00;
    U32 dBuffIdx          = 0x00;
    U08 bSecOfClst        = 0x00;
    U32 dDmaBuffAddr      = 0x00;
    Joint_Mode bJointMode = JOINT_NONE;
	T_FLASH_PHY_ADDR *ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOfst]);
	bSetVal = (U32)(ptClstAddr->m_bLunOfChan & 0x000000FF);
	bSetVal = (bSetVal<<24)|(bSetVal<<16)|(bSetVal<<8)|(bSetVal);
		
    if ( TYPE_TABLE == bDataType)
    {
        bSecOfClst  = SECT_OF_TBLE_CLST;
		bJointMode = JOINT_NONE;
    }
    else
    {
        bSecOfClst  = SECT_OF_DATA_CLST;
		bJointMode = JOINT_21;
    }

    /************************计算Buff Index***************************/

    dBuffIdx = tFtlTask.pwBuffIdx[wOfst];
    dDmaBuffAddr = tFtlTask.dHandleBuff + tFtlTask.pwBuffIdx[wOfst]*BYTE_OF_SEC;

    /************************Set Buff 为指定的Value************************/
	U08 wBuffStatus = 0;
	while(1)
	{
		wBuffStatus = (U08)HAL_BFCGetBuffStatus(dBuffIdx);
		if(!wBuffStatus)
		{
			break;
		}
	}
	
    HAL_PMCPMemorySet(PMCP0,dDmaBuffAddr,bSetVal,bSecOfClst*BYTE_OF_SEC);
	if(bJointMode != JOINT_NONE)
	{
        if(bSecOfClst <= 16)
        {
            HAL_BFCSetBuffStatus(dBuffIdx,(U16)(((U32)0x1<<bSecOfClst)-1));
        }
        else
        {
            HAL_BFCSetBuffStatus(dBuffIdx,0xFFFF);
            HAL_BFCSetBuffStatus((dBuffIdx+16),(U16)(((U32)0x1<<(bSecOfClst-16))-1));
        }
	}
}

void Fdl_VirtualChanSet(U32 dStart)
{
#ifndef __FDL_HW_VIRTUL_CLST__
    U32 dCur = dStart;

    while (dCur < tFtlTask.wLen)
    {
        T_FLASH_PHY_ADDR *tPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dCur % tFtlTask.wContiAddrMax]);
        U8 bChan = tPageAddr->m_bChan;

        if(bChan == 0xFF)//0xff为无效通道
        {
            Fdl_VirtualChanTaskSetVal(dCur,tPageAddr->m_bDataType);
        }
        dCur++;
    }
#endif
}

void Fdl_PreVirtualChanSet(void)
{
    U32 dCur = 0;
    while (dCur < tFtlTask.wLen)
    {
        T_FLASH_PHY_ADDR *tPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dCur % tFtlTask.wContiAddrMax]);
        U8 bChan = tPageAddr->m_bChan;

        if(bChan == 0xFF)//0xff为无效通道
        {
            Fdl_PreVirtualChanTaskSetVal(dCur,tPageAddr->m_bDataType);
        }
        dCur++;
    }
}

void Fdl_ReadPreSendCmd(U08 bChan,U32 dRow)
{
    U08 dRowMode = (U08)(dRow>>24);//select highest byte ,SLC:0x0  TLC:0x01/0x02/0x03

	if(dRowMode == 0)//SLC Mode
	{
		FL_CMD_CQ_SLC_MODE(bChan);
	}
	else//TLC Mode
	{
		FL_CMD_CQ_TLC_MODE(bChan);
		FL_CMD_CQ_ED3_READ_WL(bChan,dRowMode);
	}
	
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x50;
}


U8 Fdl_McuCheckStatus(U8 bChan,U8 bRdStsMode, U8 bLunOfChip,U32 dRowAddr, U8 bFirstCheckIOBit, U8 bSecondCheckIOBit)
{
    U32 dTimeCoutMax = 0x4FFF;
    U08 bRdData;

    switch(bRdStsMode)
	{
    	case RD_STS_70_CMD:
			HAL_MCU_SendCmd(bChan,FL_CMD_READ_STATUS);
    		break;

    	case RD_STS_71_CMD:
			HAL_MCU_SendCmd(bChan,FL_CMD_READ_MULTI_PLN_STATUS);
    		break;

    	case RD_STS_78_CMD:
			HAL_MCU_SendCmd(bChan,FL_CMD_READ_ENHANCE_STATUS);
			HAL_MCU_SendThreeAddr(bChan,dRowAddr);
    		break;

    	case RD_STS_FN_CMD:
			HAL_MCU_SendCmd(bChan,FL_CMD_READ_LUN_STATUS(bLunOfChip));
    		break;

    	default:
		break;
	}

    Delay(50);

    bRdData = HAL_NFSendPluse(bChan,RD_PULSE);

    while(!(bRdData & bFirstCheckIOBit))//BIT6 = 1 or time-out,exit
    {
        Delay(10);

        bRdData = HAL_NFSendPluse(bChan,RD_PULSE);

        dTimeCoutMax--;

        if(dTimeCoutMax == 0)
        {
        	bRdData = 0xFF;
            FDL_Err("ERR_MCU Chk Sts Timeout!");
            break;
        }
    }

    if(bRdData & bSecondCheckIOBit)//BIT0 = 0,exit
    {
        FDL_Err("ERR_MCU Chk Sts fail!");
    }

    return bRdData;

}

void Fdl_ClearExceptionState(U8 bChan)
{
	HAL_NFGetOrClrChkStsHwFlag(bChan,CLR_CHKSTS_FLAG);

	SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;
	__nds32__dsb();
	SG_FC(bChan)->rCQ_HOLD_CTRL.bits.DMA2_ABNL_HOLD_CQ_FLAG =  0;
	__nds32__dsb();
	SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_FLAG =  0;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rSTATUS.bits.SVA_OVF_FLAG =0;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rSTATUS.bits.DDR_TIMEOUT_FLAG = 0;
	__nds32__dsb();
}

void Fdl_SendReadCmd(U08 bChan, U08 bLun, U32 wCol, U32 dRow, U08 bPreCmdFlag)
{
	HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);

#if(NAND_FLASH == TOS_BICS3)
    HAL_CQSendFlashCmd(bChan,0x5D);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;// delay
#endif

	if(bPreCmdFlag == 0)
	{
		Fdl_ReadPreSendCmd(bChan,dRow);
	}
	else
	{
		FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow);
	}
	
    HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
    HAL_CQSend5Addr(bChan,wCol,dRow);
}

void Fdl_CQPreReadSinglePln(U08 bChan, U08 bLun, U32 wCol, U32 dRow,U08 bLastReadCmd)
{
	Fdl_SendReadCmd(bChan, bLun, wCol, dRow, 0);
	if (bLastReadCmd)
	{
		HAL_CQSendFlashCmd(bChan,bLastReadCmd);
	}	

}

void Fdl_CQPreReadMultiPln(U08 bChanPhy,U08 bLunOfChan,U32 dPln0RowOp,U08 bLastReadCmd)
{
	U08 bPln;
	
	for(bPln = 0;bPln < PLANE_OF_LUN;bPln++) //multi pln cmd
	{
		if(bPln != 0)
		{
			HAL_CQSendFlashCmd(bChanPhy,FL_CMD_READ_MP);			
		}
		Fdl_SendReadCmd(bChanPhy, bLunOfChan, 0, dPln0RowOp+bPln*ROW_GAP_OF_PLANE, bPln);							
	}
	HAL_CQSendFlashCmd(bChanPhy,bLastReadCmd);	
}

void Fdl_CQPreReadFixPln(U08 bChanPhy,U08 bLunOfChan,U32 dPln0RowOp,U16 wColAddr,U08 bPlaneCnt,U08 bLastReadCmd)
{
	U08 bPln;
	
	for(bPln = 0;bPln < bPlaneCnt;bPln++) //multi pln cmd
	{
		if(bPln != 0)
		{
			HAL_CQSendFlashCmd(bChanPhy,FL_CMD_READ_MP);			
		}
		Fdl_SendReadCmd(bChanPhy, bLunOfChan, wColAddr, dPln0RowOp+bPln*ROW_GAP_OF_PLANE, bPln);							
	}
	HAL_CQSendFlashCmd(bChanPhy,bLastReadCmd);	
}

void Fdl_CQPreReadMultiCe(U08 bChanPhy,T_CHAN_TASK_QUEUE *tChTaskQ)
{
    U08 bChip,bChipPhy,bLun,bLunOfChan;
    U32 dRowOp;

    for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)//multi ce cmd
    {
        bChipPhy  = CHIPES(bChanPhy,bChip);
        HAL_CQChipSelect(bChanPhy,bChipPhy);

        for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)//multi lun cmd
        {
            bLunOfChan = bLun+bChip*LUN_OF_CHIP;

            if((tChTaskQ->tLunTask[bLunOfChan].bRowCnt != 0))//lun task should not empty for sending cmd previously
            {
                dRowOp = (tChTaskQ->tLunTask[bLunOfChan].dRow[tChTaskQ->tLunTask[bLunOfChan].wLunHead & (MAX_CHAN_TASK-1)]) & PLANE_BIT_MASK_OF_ROW;
                Fdl_CQPreReadMultiPln(bChanPhy,bLunOfChan,dRowOp,FL_CMD_READ2);

                tChTaskQ->tLunTask[bLunOfChan].bLunRowPreOpCnt += PLANE_OF_LUN;
            }
        }
    }
}


/*#@CacheRead#*/
void Fdl_CQPreReadMultiPlaneCacheCmd(U08 bChanPhy,U08 bLunOfChan,T_CHAN_TASK_QUEUE *tChTaskQ)
{
    U32 dRowOp 		= 0;
	U32 dRowOpNext 	= 0;
	dRowOp = tChTaskQ->tLunTask[bLunOfChan].dRow[tChTaskQ->tLunTask[bLunOfChan].wLunHead & (MAX_CHAN_TASK-1)];//first addr must in plane0, or change the for cycle

#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
	if(Fdl_ED3RowAnalyze(dRowOp,0xFFFFFFFF,3))/*ED3 Flash最后一页分析*/
#else
	if( ((GET_PAGE_NUM(dRowOp) == (PAGE_OF_BLOCK_SLC - 1)) && ((dRowOp&0xFF000000) == 0)) ||
		((GET_PAGE_NUM(dRowOp) == (PAGE_OF_BLOCK - 1)) && ((dRowOp&0xFF000000) != 0)))//SLC/TLC Mode,NextRow is outsize of block, send 0x3F
#endif
	{
		if(	((tChTaskQ->bFirstFlag & 0x01) != 0x00) &&/*第一次就为最后一页,不处理*/
			(tChTaskQ->bCacheOpFlag[bLunOfChan] != 0))/*Data Reg(二级缓存)有数据*/
		{
			/*是最后一页,就不需要发Cache命令,发3Fh*/
			HAL_CQSendFlashCmd(bChanPhy,FL_CMD_CACHE_REDA_FINAL);
			tChTaskQ->bCacheOpFlag[bLunOfChan] = 0;
		}
	}
	else
	{
#ifdef __CACHE_READ_ONLY_31H__
		HAL_CQCheckIOStatus(bChanPhy, bLunOfChan, IO_1_PASS,  FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		HAL_CQSendFlashCmd(bChanPhy,FL_CMD_CACHE_REDA);/*Cache Cmd*/
#else

#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
		dRowOpNext = Fdl_ED3RowAnalyze(dRowOp,0xFFFFFFFF,0);/*出来的地址已经转成Plane0*/
#else
		dRowOpNext = ((dRowOp + 1)&PLANE_BIT_MASK_OF_ROW);
#endif
		Fdl_CQPreReadMultiPln(bChanPhy,bLunOfChan,dRowOpNext,FL_CMD_CACHE_REDA);
#endif
		tChTaskQ->bCacheOpFlag[bLunOfChan] = 1;
	}
}


void Fdl_CheckCacheRDEnd(U08 bChanPhy)
{
#ifdef __CACHE_READ_ON__
#ifdef __TAB_CACHE_RD_OFF__
	if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))
#else
	if(CACHE_READ_EN)
#endif
	{
		U08 bChip = 0;
		U08 bChipPhy = 0;
		for(bChip = 0;bChip < CHIP_OF_CHAN;bChip++)
		{
			while(HAL_CQGetIdleSpace(bChanPhy) < 30);	/*保证CQ不填爆*/
			bChipPhy = CHIPES(bChanPhy,bChip);
			HAL_CQChipSelect(bChanPhy,bChipPhy);//片选到当前CE			
			HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);		
			HAL_CQSendFlashCmd(bChanPhy,FL_CMD_CACHE_REDA_FINAL);
			HAL_CQCheckIOStatus(bChanPhy, 0, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);	
			HAL_CQWaitDone(bChanPhy);
		}
	}
#endif
}
/**************************************************************************
 *函数名称: Fdl_GetRaidPage
 *函数功能: 启动DMA2,将数据过Raid模块,得到Raid数据(一个Raid一个Page+UserData)
 *入口参数:
 * @param bChan 		逻辑通道号
 * @param dBuffAddr 	Data Buff
 * @param dUdBffAddr 	User Data Buff
 * @param dRaidBuff 	Raid Result Buff
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2019年09月03日(周二))
************************************************************************/
void Fdl_GetRaidPage(U08 bChan,U32 dBuffAddr,U32 dUdBffAddr,U32 dRaidBuff)
{
	U08 bChanPhy = CHANS(bChan);
	HAL_CQCheckHold(DMA2_WR_TO_FLASH,JOINT_NONE,bChanPhy,0,0,0);/*调用的目的是为了切方向为写*/
	HAL_CQSetUd(bChanPhy,dUdBffAddr,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,DMA2_WR_TO_FLASH);
	HAL_CQSetRaidBaseAddr(bChanPhy,dRaidBuff - 0x460000,1);
	HAL_CQStartDMA2Write(bChanPhy, dBuffAddr, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE,JOINT_NONE, NULL);
	HAL_CQWaitDone(bChanPhy);
}

/**************************************************************************
 *函数名称: Fdl_DmaRdClrTimeoutMCU
 *函数功能: MCU方式启动DMA2,解决Check Io Status超时时，硬件卡死的Bug.
 *入口参数:
 * @param bChan 		逻辑通道号
 * @param dBuffAddr 	Data Buff
 * @param bSecCnt 		读取的Sector Cnt
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2019年12月17日(周二))
************************************************************************/
void Fdl_DmaRdClrTimeoutMCU(U08 bChan,U32 dBuffAddr,U32 dDMAInfoAddr,U08 bSecCnt)
{
    U08 bPhyChan;
    U32 dRegValTmp = SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll;
    
    SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
    bPhyChan = CHANS(bChan);

	HAL_MCU_SetCe(bChan,0,1);
	HAL_MCU_SendCmd(bPhyChan,FL_CMD_RANDOM_DATA_OUTPUT1);
	HAL_MCU_SendTwoAddr(bPhyChan,0);
//	HAL_MCU_SendThreeAddr(bChan,0);
    HAL_MCU_SendCmd(bPhyChan,FL_CMD_RANDOM_DATA_OUTPUT2);
	HAL_ADMStartDMA2(bPhyChan, dBuffAddr, dDMAInfoAddr, DMA2_RD_TO_BUFF, 0, 0, bSecCnt, JOINT_NONE);
	U32 dCnt = 0xFFFFFF;
	while(0==SG_DMA2_CH(bPhyChan)->rSTATUS.bits.BUF_DONE)//解决多片选下，没等done信号，发生种子出错的问题
	{
		dCnt--;
        if (0 == dCnt)
        {
			PRINT_HEX("Retry Time out>",bPhyChan);
        }
	}

	Fdl_PullUpCeAndRd(bPhyChan);	

    SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = dRegValTmp;
}

#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
/**************************************************************************
 *函数名称: Fdl_ED3RowAnalyze
 *函数功能: 分析ED3 Row地址是否连续(返回0:不连续,1:连续),基于dRow产生一个连续的地址,分析dRow是否为最后一页
 *			
 *入口参数:
 * @param dRow 		基础Row地址
 * @param dRowNext 	下一个需要分析的地址(为0xFFFFFFFF时,用于产生新的连续Row地址)
 * @param bMode 	产生新的连续Row地址的模式(0:基于dRow加一,2:基于dRow减一,3:用于分析dRow是否为最后一页)
 * 
 *出口参数:Null
 * @return dRowAddrRet	返回是否连续或者新的Row地址(Plane0)
 * @SG_Kinder(2019年04月11日(周四))
************************************************************************/
U32 Fdl_ED3RowAnalyze(U32 dRow,U32 dRowNext,U08 bMode)
{
	U32 dRowAddrRet	= FALSE;	
	U16 wBlock		= GET_BLOCK_NUM(dRow);	
	U16 wPageNum	= GET_PAGE_NUM(dRow);
	U08 bCellType 	= dRow>>24;/*SLC Or TLC*/	
	U16 wBlockMaskBit = (~((U16)PLANE_OF_LUN - 1));/*暂时支持1、2、4plane*/

	if(dRowNext != 0xFFFFFFFF)/*Row Next有值*/
	{
		U16 wBlockNext		= GET_BLOCK_NUM(dRowNext);
		U16 wPageNumNext 	= GET_PAGE_NUM(dRowNext);
		U08 bCellTypeNext 	= dRowNext>>24;/*SLC Or TLC*/

		if((wBlock&wBlockMaskBit) == (wBlockNext&wBlockMaskBit))/*是同一个绑定块*/
		{						
			if((bCellType > 0) && (bCellTypeNext > 0))/*同时为TLC*/
			{
				if((wPageNum*TLC_PRE_CMD + bCellType + 1) == (wPageNumNext*TLC_PRE_CMD + bCellTypeNext))/*Page是加一的关系*/
				{
					dRowAddrRet = TRUE;
				}
			}
			else if((bCellType == 0) && (bCellTypeNext == 0))/*同时为SLC*/
			{
				if((wPageNum + 1) == (wPageNumNext))/*Page是加一的关系*/
				{
					dRowAddrRet = TRUE;
				}
			}									
		}
	}
	else/*为ED3 Flash生成下一个连续的Row地址或者上一个连续的Row地址(用于Retry)*/
	{		
		if(bMode <= 2)
		{
			if(bCellType)/*TLC*/
			{				
				wPageNum = wPageNum*TLC_PRE_CMD + bCellType;				
				dRowAddrRet = GET_ROW_ADDR_TLC(((wPageNum - bMode)/TLC_PRE_CMD),(wBlock&wBlockMaskBit),(((wPageNum - bMode)%TLC_PRE_CMD) + 1));
				
				if(wPageNum < bMode)/*第一页的上一页*/
				{
					dRowAddrRet = GET_ROW_ADDR_TLC(0xFF,((wBlock&wBlockMaskBit) - PLANE_OF_LUN),0x03);
				}
				else if((bMode == 0) && (wPageNum == PAGE_OF_BLOCK))/*最后一页的下一页(bMode=0)*/
				{
					dRowAddrRet = GET_ROW_ADDR_TLC(0x00,((wBlock&wBlockMaskBit) + PLANE_OF_LUN),0x01);
				}				
			}
			else/*SLC*/
			{
				dRowAddrRet = GET_ROW_ADDR_SLC((wPageNum + 1 - bMode),(wBlock&wBlockMaskBit));
				
				if((bMode == 2) && (wPageNum == 0))/*第一页的上一页*/
				{
					dRowAddrRet = GET_ROW_ADDR_SLC(0xFF,((wBlock&wBlockMaskBit) - PLANE_OF_LUN));
				}
				else if((bMode == 0) && ((wPageNum + 1) == PAGE_OF_BLOCK_SLC))/*最后一页的下一页(bMode=0)*/
				{
					dRowAddrRet = GET_ROW_ADDR_SLC(0x00,((wBlock&wBlockMaskBit) + PLANE_OF_LUN));
				}
			}
		}		
		else/*分析传进来的ROW是不是最后一页*/
		{	
			if(bCellType)/*TLC*/
			{
				wPageNum = wPageNum*TLC_PRE_CMD + bCellType;
				if(wPageNum == PAGE_OF_BLOCK)/*是最后一页*/
				{
					dRowAddrRet = TRUE;
				}
			}
			else/*SLC*/
			{
				if(wPageNum == (PAGE_OF_BLOCK_SLC - 1))/*是最后一页*/
				{
					dRowAddrRet = TRUE;
				}
			}			
		}
	}

	return dRowAddrRet;
}

#endif

//扫描每个Lun第一个地址在ptClstAddr中的偏移(稳定了,由FTL传参数过来)
void Fdl_ScanFirstOffSetofLun(U08 *FirstOfSet,T_FLASH_PHY_ADDR *ptAddr)
{
    U16 wOfSet 		= 0;
    U08 bLunNum 	= 0;
    U08 bLunCnt 	= CHAN_MAX_CNT*LUN_OF_CHIP*CHIP_OF_CHAN;
	U16 wTaskLen 	= (tFtlTask.wLen <= 0x100)?tFtlTask.wLen:0x100;//将len限制在255以内
	
    while (wOfSet < wTaskLen)
    {
        if(0xFF == ptAddr->m_bChan)  
        {
			wOfSet++;
        	ptAddr++;
            continue;
        }

        if(FirstOfSet[ptAddr->m_bChan*LUN_OF_CHAN + ptAddr->m_bLunOfChan] == 0xFF)//value the fisrt ofst in ftl task
        {
            FirstOfSet[ptAddr->m_bChan*LUN_OF_CHAN + ptAddr->m_bLunOfChan] = wOfSet;
            bLunNum++;

            if(bLunNum == bLunCnt)
            {
                break;
            }
        }

        wOfSet++;
        ptAddr++;
    }
}

#ifdef __FDL_HW_VIRTUL_CLST__
void Fdl_PreReadMultiCe(T_FLASH_PHY_ADDR *ptClstAddr)
{
	//U08	bPln;
    U32 dRowOp;
    U08 bChan,bChanPhy;
	U08	bChip,bChipPhy;
	U08	bLun,bLunOfChan;

    U08 FirstOfSet[CHAN_MAX_CNT*LUN_OF_CHAN];//目前只支持一次地址个数小于255的情况
    U08MemSet((U08 *)FirstOfSet,0xFF,sizeof(FirstOfSet));
    Fdl_ScanFirstOffSetofLun(FirstOfSet,ptClstAddr);

    for (bChan = 0; bChan < CHAN_MAX_CNT; bChan++)
    {
        bChanPhy  = CHANS(bChan);
        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)//multi ce cmd
        {
            bChipPhy  = CHIPES(bChanPhy,bChip);
            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)//multi lun cmd
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
				if(FirstOfSet[bChan*CHIP_OF_CHAN + bLunOfChan] != 0xFF)
                {
					HAL_CQChipSelect(bChanPhy,bChipPhy);              					
					dRowOp = ((ptClstAddr + FirstOfSet[bChan*CHIP_OF_CHAN + bLunOfChan])->m_dRowAddr) & PLANE_BIT_MASK_OF_ROW;	 
					Fdl_CQPreReadMultiPln(bChanPhy,bChipPhy,dRowOp,FL_CMD_READ2);
					tChTaskQueue[bChanPhy].tLunTask[bLunOfChan].dCmdRow = dRowOp;
                }
				else
				{
					tChTaskQueue[bChanPhy].tLunTask[bLunOfChan].dCmdRow = 0xFEFFFFFF;//处理使用Block0的BUG，命令预发后，continue前地址被清0，检测两个地址相同，不发命令导致数据fail
				}
            }
        }
    }
}
#else
void Fdl_PreReadMultiCe(T_FLASH_PHY_ADDR *ptClstAddr)
{
    U32 dRowOp;
    U08 bChan,bChanPhy;
	U08	bChip,bChipPhy;
	U08	bLun,bLunOfChan;
    U08 FirstOfSet[CHAN_MAX_CNT*LUN_OF_CHAN];//目前只支持一次地址个数小于255的情况
    U08MemSet((U08 *)FirstOfSet,0xFF,sizeof(FirstOfSet));
    Fdl_ScanFirstOffSetofLun(FirstOfSet,ptClstAddr);

    for (bChan = 0; bChan < CHAN_MAX_CNT; bChan++)
    {
        bChanPhy  = CHANS(bChan);
		
        if(bChanPhy == 0xFF)
        {
            continue;
        }
		
        for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)//multi ce cmd
        {
            bChipPhy  = CHIPES(bChanPhy,bChip);

            for(bLun = 0; bLun < LUN_OF_CHIP; bLun++)//multi lun cmd
            {
                bLunOfChan = bLun+bChip*LUN_OF_CHIP;
				
				if(FirstOfSet[bChan*CHIP_OF_CHAN + bLunOfChan] != 0xFF)
                {
					HAL_CQChipSelect(bChanPhy,bChipPhy);              					
					dRowOp = ((ptClstAddr + FirstOfSet[bChan*CHIP_OF_CHAN + bLunOfChan])->m_dRowAddr) & PLANE_BIT_MASK_OF_ROW;	 

					Fdl_CQPreReadMultiPln(bChanPhy,bLunOfChan,dRowOp,FL_CMD_READ2);
					
	                tChTaskQueue[bChanPhy].tLunTask[bLunOfChan].bLunRowPreOpCnt += PLANE_OF_LUN;
					tChTaskQueue[bChanPhy].bFirstFlag = 0x10;
                }
            }
        }
    }
}
#endif

U16 Fdl_SearchOfstByAddr(U8 bFailChan,U8 bFailLogLun,U32 dAddr,U16 wOfst,U08 bMode)
{
    U8  bPhyChan;
    U16 wOfstTmp = wOfst;

    T_FLASH_PHY_ADDR *ptPageAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wOfstTmp]);
    bPhyChan = CHANS(ptPageAddr->m_bChan);

    if(FORWARD_SEARCH_ADDR_OFST == bMode)
    {
        while((bPhyChan != bFailChan) &&
			  (ptPageAddr->m_bLunOfChan != bFailLogLun) &&
			  (ptPageAddr->m_dRowAddr != dAddr))
        {
            if(wOfstTmp == 0)
            {
                wOfstTmp = FAIL_SEARCH_ADDR_OFST;
                break;
            }

            wOfstTmp--;
            ptPageAddr--;
            bPhyChan = CHANS(ptPageAddr->m_bChan);
        }
    }
	else if(FORWARD_SEARCH_LUN_ADDR_OFST == bMode)
	{
        while((bPhyChan != bFailChan) ||
			  (ptPageAddr->m_bLunOfChan != bFailLogLun))
        {
            if(wOfstTmp == 0)
            {
                wOfstTmp = FAIL_SEARCH_ADDR_OFST;
                break;
            }

            wOfstTmp--;
            ptPageAddr--;
            bPhyChan = CHANS(ptPageAddr->m_bChan);
        }
	}
    else if(BACKWARD_SEARCH_ADDR_OFST == bMode)
    {
        while((bPhyChan != bFailChan) &&
			  (ptPageAddr->m_bLunOfChan != bFailLogLun) &&
			  (ptPageAddr->m_dRowAddr != dAddr))
        {
            if(wOfstTmp == tFtlTask.wLen)
            {
                wOfstTmp = FAIL_SEARCH_ADDR_OFST;
                break;
            }

            wOfstTmp++;
            ptPageAddr++;
            bPhyChan = CHANS(ptPageAddr->m_bChan);
        }
    }
	else if(BACKWARD_SEARCH_LUN_ADDR_OFST == bMode)
	{
		while((bFailChan != bPhyChan) ||
			  (bFailLogLun != ptPageAddr->m_bLunOfChan))
        {
            if(wOfstTmp == tFtlTask.wLen-1)
            {
                wOfstTmp = FAIL_SEARCH_ADDR_OFST;
                break;
            }

            wOfstTmp++;
            ptPageAddr++;
            bPhyChan = CHANS(ptPageAddr->m_bChan);
        }
    }

    return wOfstTmp;
}

void Fdl_PreRead4K(T_FLASH_PHY_ADDR *ptClstAddr,U08 bClstLen)
{
//	U08	i;
//    U32 dRowOp;
//    U08 bChan;
//	U08 bChanPhy;
//	U08	bChip;
//	U08 bChipPhy;
//	U16 wCol;
//
//	for(i = 0; i <  bClstLen; i++)
//	{
//		bChan = ptClstAddr->m_bChan;
//		if(bChan < CHAN_MAX_CNT)
//		{
//			bChip = ptClstAddr->m_bLunOfChan;
//			bChanPhy = CHANS(bChan);
//			bChipPhy = CHIPES(bChanPhy, bChip);
//			HAL_CQChipSelect(bChanPhy,bChipPhy);
//			dRowOp = ptClstAddr->m_dRowAddr;
//			
//			if(TYPE_TABLE == ptClstAddr->m_bDataType)
//			{
//				wCol = TBLE_COL_ADDR(ptClstAddr->m_bClstOffs * DMA_OF_TABLE_CLST);
//			}
//			else
//			{
//				wCol = DATA_COL_ADDR(ptClstAddr->m_bClstOffs * DMA_OF_DATA_CLST);
//			}
//
//			if(SNAP_READ_EN == 0)/*不支持Snap Read*/
//			{
//				Fdl_CQPreReadFixPln(bChanPhy,0,dRowOp,wCol,(SNAP_READ_EN + 1),FL_CMD_READ2);
//			}
//			else
//			{
//				Fdl_CQPreReadFixPln(bChanPhy,0,dRowOp,wCol,SNAP_READ_EN,FL_CMD_SNAP_READ);
//			}
//		}
//		ptClstAddr++;
//	}

}

#ifdef __TASK_ERROR_FORCE_STOP__
/**************************************************************************
 *函数名称: WDT0_IRQHandler
 *函数功能: WDT0中断(暂时不用)
 *入口参数:
 * NC
 *出口参数:
 * NC
 * @SG_Kinder-2018年03月08日(周四)
************************************************************************/
void WDT0_IRQHandler()
{
	SG_CPM->rWDT0_RST_FLAG = 0;
	FDL_Log("WDT0_IRQHandler");
	/*CLEAR IRQ FLAG*/
    NVIC_ClearPendingIRQ(IRQ_WDT0_RST);
}

/**************************************************************************
 *函数名称: Fdl_UpdateTaskTime
 *函数功能: 任务计时,根据传进来的任务状态,该函数具有多重功能:
 *			1.初始化看门狗,任务创建的时候调用;
 *			2.关闭看门狗,任务结束时候调用;
 *			3.喂狗,任务执行过程中在FC中断里面调用;
 *入口参数:
 * @bTaskErgo:任务状态,
 *				TASK_INIT 	:初始化任务计时;
 *				INVALID_U8	:结束任务计时;
 *				其他			:重置任务计时--喂狗;
 *出口参数:
 * NC
 * @SG_Kinder-2018年03月08日(周四)
************************************************************************/
void Fdl_UpdateTaskTime(U08 bTaskErgo)
{
	if(bTaskErgo == TASK_INIT)/*初始化任务看门狗*/
	{
		SG_CPM->rWDT0_RST_CTRL.dAll = 0xAAAAAAAA;/*不复位任何硬件*/
		SG_CPM->rWDT0_THRESHOLD = 0x2FFFFFF;/* 2x0.6s = 1.2s*/
		SG_CPM->rWDT0_EN = 0xFF;/*写入0xAA会关闭WDT0,其他任何值都会打开*/
	}
	else if(bTaskErgo == INVALID_U8)/*关闭任务看门狗*/
	{
	    SG_CPM->rWDT0_EN = 0xAA;/*写入0xAA会关闭WDT0,其他任何值都会打开*/
		SG_CPM->rWDT0_RST_FLAG = 0;
	}
	else/*要在中断里面不断喂狗*/
	{
	   SG_CPM->rWDT0_CLEAR = 1;/*喂狗*/ 
	}	
}

/**************************************************************************
 *函数名称: Fdl_GetTaskHoldFlag
 *函数功能: 获取任务被卡住标志
 *入口参数:
 * NC
 *出口参数:
 * NC
 * @SG_Kinder-2018年03月08日(周四)
************************************************************************/
U08 Fdl_GetTaskHoldFlag(void)
{
	return ((U08)SG_CPM->rWDT0_RST_FLAG);
}

/**************************************************************************
 *函数名称: FDL_StopTaskForce
 *函数功能: 强制停止任务
 *入口参数:
 * NC
 *出口参数:
 * NC
 * @SG_Kinder-2018年03月08日(周四)
************************************************************************/
void FDL_StopTaskForce(void)
{
	U16 i = 0;
	extern T_FDL_PARAM tFdlParam;	
	extern T_RETRY_INFOR tRetryInfor;
	extern T_RETRY_CTRL tRetryCtrl[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];
	extern U08 bStateMachine[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];//Retry流程的状态阶段记录
	extern U08 bBestOption[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];//对应的最优option值，一直保留
	
	/*硬件上的复位*/
	SG_DMA2_CH(0)->rCTRL.bits.STOP = 1;
	SG_DMA2_CH(1)->rCTRL.bits.STOP = 1;
	SG_DMA2_CH(2)->rCTRL.bits.STOP = 1;
	SG_DMA2_CH(3)->rCTRL.bits.STOP = 1;

	SG_CPM->rSOFT_RST0.bits.FC0 = 0;
	SG_CPM->rSOFT_RST0.bits.FC1 = 0;
	SG_CPM->rSOFT_RST0.bits.FC2 = 0;
	SG_CPM->rSOFT_RST0.bits.FC3 = 0;

	SG_CPM->rSOFT_RST1.bits.ADM_DMA20 = 0;
	SG_CPM->rSOFT_RST1.bits.ADM_DMA21 = 0;
	SG_CPM->rSOFT_RST1.bits.ADM_DMA22 = 0;
	SG_CPM->rSOFT_RST1.bits.ADM_DMA23 = 0;

	SG_CPM->rSOFT_RST1.bits.ADM2LDPC = 0;
	SG_CPM->rSOFT_RST0.bits.LDEC	 = 0;

	/*Retry 相关变量清零*/
	MemorySet((U08 *)&tRetryInfor,0,sizeof(T_RETRY_INFOR));
	MemorySet((U08 *)bBestOption,0,MAX_CHAN_OF_DEV*MAX_CHIP_OF_CHAN);
	MemorySet((U08 *)bStateMachine,0,MAX_CHAN_OF_DEV*MAX_CHIP_OF_CHAN);
	MemorySet((U08 *)tRetryCtrl,0,sizeof(T_RETRY_CTRL)*MAX_CHAN_OF_DEV*MAX_CHIP_OF_CHAN);

	/*联动的时候,不论读写,让DMA1走完*/
	if(	(tFtlTask.dTaskType == TYPE_NCQ_READ) ||
		(tFtlTask.dTaskType == TYPE_NCQ_READ_4K))
	{
		/*置水位,直到DMA1 Done*/
		do
		{
			for(i = 0;i < SG_ADM->rBUF_SIZE.wAll;i += 0x10)
			{
				HAL_BFCSetBuffStatus(i,0xFFFF);
			}			
		}while((!SG_DMA1->rSTATUS.bits.TRANS_DONE)|(!SG_DMA1->rMC_STS.bits.MC_DONE));
		/*最后都清一遍水位*/
		for(i = 0;i < SG_ADM->rBUF_SIZE.wAll;i += 0x10)
		{
			HAL_BFCClearBuffStatus(i,0xFFFF);
		}
	}
	else if(tFtlTask.dTaskType == TYPE_NCQ_WRITE)/*Joint Write*/
	{
		/*清水位,一直清,直到DMA1 Done*/
		do
		{
			for(i = 0;i < SG_ADM->rBUF_SIZE.wAll;i += 0x10)
			{
				HAL_BFCClearBuffStatus(i,0xFFFF);
			}			
		}while((!SG_DMA1->rSTATUS.bits.TRANS_DONE)|(!SG_DMA1->rMC_STS.bits.MC_DONE));
		/*最后都清一遍水位*/
		for(i = 0;i < SG_ADM->rBUF_SIZE.wAll;i += 0x10)
		{
			HAL_BFCClearBuffStatus(i,0xFFFF);
		}
	}

	/*软件上的复位*/
	tFtlTask.wCur = tFtlTask.wLen;
	for(i = 0;i < CHAN_MAX_CNT;i++)
	{
		tChTaskQueue[i].wTaskCnt = 0;
	}
		
	/*重新初始化硬件和Flash,由FTL来调?*/
//	FDL_Init(&tFdlParam);
	
}
#endif


#ifdef __TASK_REORDER_TO_READ_TABLE__	/*读表格Row地址重排序功能*/
/**************************************************************************
 *函数名称: Fdl_TaskReOrderReadTable
 *函数功能: 读表格Row地址重排序功能
 *入口参数:
 * NC
 *出口参数:
 * NC
 * @SG_Kinder-2018年03月01日(周四)
************************************************************************/
void Fdl_TaskReOrderReadTable(void)
{
	U32 wOffset 	= 0;
	U16 wTaskLen	= tFtlTask.wLen;
	U16 wRowCnt 	= wTaskLen;
	U16 wRowIndex	= wTaskLen*2;

	__align(8) U32 dRowAddrSortRet[wRowCnt];/*Row地址重排序结果Buff(源数据值)--其他用途,临时Buff*/
	__align(8) U16 dRowAddrSortOffSetRet[wRowIndex];/*Row地址重排序结果Buff(Index值)*/

	U16 *pBuffIndex = (U16 *)tFtlTask.pwBuffIdx;
	T_FLASH_PHY_ADDR *pRowAddrSrc = (T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr;
	T_FLASH_PHY_ADDR *pRowAddrCpy = (T_FLASH_PHY_ADDR *)tFtlTask.dHandleBuff;

	for(wOffset = 1;wOffset < wRowIndex;wOffset += 2)
	{
		*(pBuffIndex++) = wOffset;
	}
	pBuffIndex = (U16 *)tFtlTask.pwBuffIdx;
	
	/******第一阶段:Buff备份,Row地址提取********/
	/*MemCpy--备份源RowAdd Buff*/
	HAL_PMCPMatrixValueOp(	PMCPIDX,				/*PMCP索引*/
							(U32)pRowAddrSrc,		/*源Buff指针*/
							0,						/*加上的固定数(固定数为0,表示MemCpy)*/
							(U32)pRowAddrCpy,		/*结果Buff指针*/
							wRowCnt*2,				/*处理的项数*/
							sizeof(U32),			/*源Buff步长*/
							sizeof(U32),			/*源Buff步长*/
							MATRIX_ADD);			/*模式为矩阵加固定数*/

	 /*带Index的内存提取--将Row地址(T_FLASH_PHY_ADDR高4字节)提取出来*/
	 HAL_PMCPGetValueFromMem(	PMCPIDX,			/*PMCP索引*/
								(U32)pRowAddrCpy,	/*源Buff指针*/
								(U32)pBuffIndex,	/*Index Buff*/
								(U32)pRowAddrSrc,	/*结果Buff(直接拷贝源Buff的值,大小与源Buff一致)*/
								wTaskLen,			/*处理的项数*/
								sizeof(U16),		/*Index步长*/
								sizeof(U32));		/*Buff步长(源Buff和结果Buff)*/

	/******第二阶段:Row重排序********/
	/*带Mask的最小值搜索--对提取出来的Row进行重排序*/
	HAL_PMCPSrchMinWithMask(PMCPIDX,					/*PMCP索引*/
							(U32)pRowAddrSrc,			/*源Buff指针*/
							(~PLANE_BIT_MASK_OF_ROW),	/*Mask Bit 1有效*/
							sizeof(U32),				/*搜索步长*/
							wRowCnt,					/*搜索项数*/
							(U32)dRowAddrSortRet,		/*结果Buff(直接拷贝源Buff的值,大小与源Buff一致)*/
							(U32)dRowAddrSortOffSetRet, /*结果Buff(存放结果的OffSet,每个OffSet 2Byte表示)*/
							wRowCnt,					/*期望的得到的结果项数()*/
							0); 						/*从哪一个OffSet开始搜索*/

	/******第三阶段:处理重排序后的结果********/
	/*带mask矩阵移位功能(乘法器or除法器)*/
	/*1、处理排序后的Buff Index--(OffSet*4)*/								
	HAL_PMCPArraShiftBitsMask_Cfg(	PMCPIDX,					/*PMCP索引*/
									(U32)dRowAddrSortOffSetRet, /*源Buff指针*/
									sizeof(U16),				/*源Buff步长*/
									wRowCnt,					/*移位项数*/
									(U32)pBuffIndex,			/*结果Buff指针(存放移位后的结果)*/
									sizeof(U16),				/*结果Buff步长*/
									1,							/*1表示左移，0表示右移*/
									2,							/*所移的位数(BuffIndex需要乘以4)*/
									0); 						/*Mask Bit 1有效*/
	HAL_PMCPWaitDone(PMCPIDX);

	/*2、处理排序后的Row Index--(OffSet*2)*/
	HAL_PMCPArraShiftBitsMask_Cfg(	PMCPIDX,					/*PMCP索引*/
									(U32)dRowAddrSortOffSetRet, /*源Buff指针*/
									sizeof(U16),				/*源Buff步长*/
									wRowCnt,					/*移位项数*/
									(U32)pRowAddrSrc,			/*结果Buff指针(存放移位后的结果)*/
									sizeof(U32)/*sizeof(U16)*/, /*结果Buff步长(故意将步长设置为4Byte，便于下面插空)*/
									1,							/*1表示左移，0表示右移*/
									1,							/*所移的位数*/
									0); 						/*Mask Bit 1有效*/	
	HAL_PMCPWaitDone(PMCPIDX);

	/*3、处理排序后的Row Index--(OffSet*2)*/
	HAL_PMCPArraShiftBitsMask_Cfg(	PMCPIDX,						/*PMCP索引*/
									(U32)dRowAddrSortOffSetRet, 	/*源Buff指针*/
									sizeof(U16),					/*源Buff步长*/
									wRowCnt,						/*移位项数*/
									(U32)dRowAddrSortRet,/*结果Buff指针(存放移位后的结果)*/
									sizeof(U32)/*sizeof(U16)*/, 	/*结果Buff步长(故意将步长设置为4Byte，便于下面插空)*/
									1,								/*1表示左移，0表示右移*/
									17, 							/*所移的位数*/
									0); 							/*Mask Bit 1有效*/
	HAL_PMCPWaitDone(PMCPIDX);
	
	/*MemCpy--矩阵加固定数*/
	HAL_PMCPMatrixValueOp(	PMCPIDX,					/*PMCP索引*/
							(U32)dRowAddrSortRet,		/*源Buff指针*/
							0x00010000, 				/*加上的固定数(固定数为0,表示MemCpy)*/
							(U32)dRowAddrSortOffSetRet, /*结果Buff指针*/
							wRowCnt*2,					/*处理的项数*/
							sizeof(U32),				/*源Buff步长*/
							sizeof(U32),				/*源Buff步长*/
							MATRIX_ADD);				/*模式为矩阵加固定数*/
		
	/*MemCpy--矩阵与矩阵相加*/
	HAL_PMCPMatrixMatrixOp_Cfg( PMCPIDX,				/*PMCP索引*/
							(U32)dRowAddrSortOffSetRet,	/*源矩阵Buff1*/ 
							(U32)pRowAddrSrc,			/*源矩阵Buff2*/
							(U32)dRowAddrSortRet, 		/*操作结果Buff*/
							wRowCnt,					/*操作项数*/
							sizeof(U32),				/*操作步长*/
							sizeof(U32),				/*结果步长*/
							MATRIX_ADD);				/*模式*/
	HAL_PMCPWaitDone(PMCPIDX);

	/*带Index的内存提取--按照排序的Index将RowAddress提取到源Buff*/
	HAL_PMCPGetValueFromMem(   PMCPIDX, 		   /*PMCP索引*/
							   (U32)pRowAddrCpy,   /*源Buff指针*/
							   (U32)dRowAddrSortRet,/*Index Buff*/
							   (U32)pRowAddrSrc,   /*结果Buff(直接拷贝源Buff的值,大小与源Buff一致)*/
							   wTaskLen*2,		   /*处理的项数*/
							   sizeof(U16), 	   /*Index步长*/
							   sizeof(U32));	   /*Buff步长(源Buff和结果Buff)*/

#if 0
	for(wOffset = 0;wOffset < wTaskLen;wOffset++)
	{
		PRINT("pAddr%04d-%04X:|....%02d....|..%02d..|.....%02d....|....%02d....|.0x%06X.|.0x%04X-0x%03X-%d.|\n",wOffset,dRowAddrSortRet[wOffset]>>17,\
		(pRowAddrSrc + wOffset)->m_bDataType,\
		(pRowAddrSrc + wOffset)->m_bChan,\
		(pRowAddrSrc + wOffset)->m_bLunOfChan,\
		(pRowAddrSrc + wOffset)->m_bClstOffs,\
		(pRowAddrSrc + wOffset)->m_dRowAddr,\
		(pRowAddrSrc + wOffset)->m_dRowAddr>>11,\
		(pRowAddrSrc + wOffset)->m_dRowAddr&0x7FF,\
		(((pRowAddrSrc + wOffset)->m_dRowAddr/2048)&(4-1)));
	}
#endif

}

#endif



#if (NAND_FLASH == TOS_BICS3)
void BCS3_PowerONReadFlash(U32 dRowOp,U16 wBlkLen)   //wBlkLen = 2956/2
{
	U8 i,j,bPhyChan,bPhyChip,bRet;
	while (wBlkLen)  //总块数
	{
		for(i = 0;i < CHAN_MAX_CNT;)
		{
			bPhyChan = CHANS(i);
			bRet = HAL_CQWaitIdleSpace(bPhyChan,0x50); //如果CQ小于时，Loop一下，
			
			if (bRet) //没有查到通道
			{
				for(j = 0;j < CHIP_OF_CHAN;j++)
				{
					bPhyChip  = CHIPES(bPhyChan,j);
					HAL_CQChipSelect(bPhyChan,bPhyChip);//片选到当前
					HAL_CQCheckIOStatus(bPhyChan, 0, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
					HAL_CQSendFlashCmd(bPhyChan,FL_CMD_EXIT_CHK_IOSTA);

					Fdl_CQPreReadMultiPln(bPhyChan,bPhyChip,dRowOp,FL_CMD_READ2);
				}
				i++;//通道保持不变
			}

		}
		dRowOp +=ROW_GAP_OF_BLOCK; //下一个块
		wBlkLen--; //
	}
}
#endif



#ifdef __TASK_ERROR_FORCE_STOP__	

/**************************************************************************
 *函数名称: FDL_StopTaskForceTiger(Debug用,后面删除)
 *函数功能: 触发一个卡死
 *入口参数:@bChan:卡死通道号
 * NC
 *出口参数:
 * NC
 * @SG_Kinder-2018n年06月01日(周五)
************************************************************************/
void FDL_StopTaskForceTiger(U08 bChan)
{
	/*触发一个卡死*/
	switch(bChan)
	{
		case 0:
			SG_CPM->rSOFT_GATE0.bits.FC0_TRX2_CLK = 0;
			break;
		case 1:
			SG_CPM->rSOFT_GATE0.bits.FC1_TRX2_CLK = 0;
			break;
		case 2:
			SG_CPM->rSOFT_GATE0.bits.FC2_TRX2_CLK = 0;
			break;
		case 3:
			SG_CPM->rSOFT_GATE0.bits.FC3_TRX2_CLK = 0;
			break;
		default:
			break;
	}
}

#endif


#ifdef __FILL_PAGE_EN__
/**************************************************************************
 *函数名称: FDL_JCOrginalInfoInitForB08T
 *函数功能: 初始化跳列源信息:
 			1、每个DMA指定位置跳过列的Byte数;
 			2、同时还要更新列地址;
 *入口参数:
 * @param dJCOrginalInfoAddr 		存放跳列源信息Buff地址
 * @param dJCOrginalInfoBuffSize 	存放跳列源信息的Buff大小
 * @param bDataType 				生成跳列源信息的Data Type(Table Or Data)
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年07月03日(周二))
************************************************************************/
#ifdef __FW__
void  __section(".CODE_INIT") FDL_JCOrginalInfoInitForB08T(U32 dJCOrginalInfoAddr,U32 dJCOrginalInfoBuffSize,U08 bDataType)
#else
void FDL_JCOrginalInfoInitForB08T(U32 dJCOrginalInfoAddr,U32 dJCOrginalInfoBuffSize,U08 bDataType)
#endif
{
	U08 i = 0;
	U08 j = 0;
	U08 m = 0;
	U08 k = 0;

	U32MemSet((U32*)dJCOrginalInfoAddr,0,dJCOrginalInfoBuffSize);

	T_DMA2_JC_ORGINAL_INFO_TOTAL *tpDma2JCOrginalInfoTotal = (T_DMA2_JC_ORGINAL_INFO_TOTAL *)dJCOrginalInfoAddr;
	tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo = (T_DMA2_JC_ORGINAL_INFO *)(dJCOrginalInfoAddr + sizeof(T_DMA2_JC_ORGINAL_INFO_TOTAL));
	tpDma2JCOrginalInfoTotal->tJCUDPParam.bJCOrginalInfoCnt = 1;/*准备生成多少组跳列源信息(以Page为单位--一个Page内的所有DMA跳列信息都确定后算一组有效的信息)*/

	U16 *wpColAddr;
	if(bDataType == TYPE_TABLE)
	{
		wpColAddr = pFdlParam.ptUdpFdl->m_tFlashParam.m_wTabColAddr;
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt 	= DMA_OF_TABLE_CLST;
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bUseDataLen 	= UD_LEN_OF_TBLE_CLST;
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bDMASectors 	= TSECT_OF_DMA;		
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bPageClustCnt = TBLE_CLST_OF_PAGE;		
	}
	else
	{		
		wpColAddr = pFdlParam.ptUdpFdl->m_tFlashParam.m_wDataColAddr;
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt 	= DMA_OF_DATA_CLST;
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bUseDataLen 	= UD_LEN_OF_DATA_CLST;
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bDMASectors 	= DSECT_OF_DMA;		
		tpDma2JCOrginalInfoTotal->tJCUDPParam.bPageClustCnt = DATA_CLST_OF_PAGE;
	}

	//U16 wPageSize			= PAGE_SIZE;
	U08 bDMACnt 			= tpDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt*tpDma2JCOrginalInfoTotal->tJCUDPParam.bPageClustCnt;
	U16 wChunkSize			= 4608;/*B08T一个Chunk的大小:4K+512Byte*/
	U08 bPartiySize 		= HAL_GetParityLen();
	U16 wDMASize			= tpDma2JCOrginalInfoTotal->tJCUDPParam.bDMASectors*SECTOR_SIZE + bPartiySize;
	U16 wClustSize			= tpDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt*wDMASize;
	U08 bChunkClustCnt		= wChunkSize/wClustSize;/*一个Chunk Size可以放多少个簇,用于计算Chunk内UserData个数*/
	U16 wJCTotalByte		= wChunkSize - (wDMASize*tpDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt + tpDma2JCOrginalInfoTotal->tJCUDPParam.bUseDataLen)*bChunkClustCnt;
	U16 wJCStartCol 		= wChunkSize - wDMASize - wJCTotalByte - (bChunkClustCnt - 1)*tpDma2JCOrginalInfoTotal->tJCUDPParam.bUseDataLen;
	U16 wJC_ByteCntOfDMA[bDMACnt];/*记录每个DMA的列地址偏移*/


	U16 wJCOrginalInfoSize 	= sizeof(T_DMA2_JC_ORGINAL_INFO) * tpDma2JCOrginalInfoTotal->tJCUDPParam.bJCOrginalInfoCnt;/*头信息大小*/ 
	U16 wGroupInfoOffSet 	= wJCOrginalInfoSize + sizeof(T_DMA2_JC_ORGINAL_INFO_TOTAL);
	U08 bJCDMAGroupInfoSize = sizeof(T_JC_DMA_GROUP_INFO);
	U16MemSet(wJC_ByteCntOfDMA,0,sizeof(wJC_ByteCntOfDMA)/sizeof(U16));


	for(i = 0; i < tpDma2JCOrginalInfoTotal->tJCUDPParam.bJCOrginalInfoCnt; i++)
	{
		for(j = 0; j < bDMACnt; j++)
		{	
			for(m = 0;m <= j;m++)
			{				
				tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wColumnOffsetOfDMA += wJC_ByteCntOfDMA[m];/*记录每个DMA的列地址偏移*/
			}
			
			wpColAddr[j + 0x00] += tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wColumnOffsetOfDMA;		
			wpColAddr[j + 0x08] += tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wColumnOffsetOfDMA;		
			wpColAddr[j + 0x10] += tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wColumnOffsetOfDMA;		
			wpColAddr[j + 0x18] += tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wColumnOffsetOfDMA;		
#if (NAND_FLASH == YMTC_B08T)
			if(bDataType == TYPE_TABLE)
			{				
				tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wJCGroupCnt = 0;//j%2;			
			}
			else
			{
				tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wJCGroupCnt = j%2;			
			}
#else
			tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wJCGroupCnt = 0;//j%2;			
#endif
			tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].tJCDMAGroupInfo = (T_JC_DMA_GROUP_INFO *)(dJCOrginalInfoAddr + wGroupInfoOffSet);
			
			for(k = 0;k < tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].wJCGroupCnt;k++)
			{				
				tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].tJCDMAGroupInfo[k].wJCTotalByteCnt = wJCTotalByte; /*跳列的Byte数(需保证为偶数)*/
				tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].tJCPageGroupInfo[j].tJCDMAGroupInfo[k].wJCStartColumn  = wJCStartCol;/*跳列起始位置(需保证为偶数)*/
				tpDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].wJCTotalByteOfPage += wJCTotalByte;
				wJC_ByteCntOfDMA[j] += wJCTotalByte;
				wGroupInfoOffSet += bJCDMAGroupInfoSize;
			}		
		}
	}
}

/**************************************************************************
 *函数名称: FDL_JCInfoSet
 *函数功能: 配置Plane中某个DMA的跳列区间信息和跳列CFG信息
 *入口参数:
 * @param bJCOrginalInfoIndex 	选定当一组跳列源信息(Index)
 * @param bDMAIndex 			选定当前组跳列源信息中的某一个DMA
 * @param wFillSize 			是否需要在页尾填充以及填充的大小
 * @param tDMA2JCTable			存储的跳列表基地址的数据结构
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年07月03日(周二))
************************************************************************/
void FDL_JCInfoSet(U08 bJCOrginalInfoIndex,U08 bDMAIndex,U16 wFillSize,T_DMA2_JC_TABLE *tDMA2JCTable)
{
	U08 i = 0;	

	U08	bClstDmaCnt 	= tDMA2JCTable->tDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt;
	U08	bPageClustCnt 	= tDMA2JCTable->tDma2JCOrginalInfoTotal->tJCUDPParam.bPageClustCnt;
	U32 bPageDMACnt 	= bClstDmaCnt*bPageClustCnt;/*一个Page的DMA个数*/
	U16 wJCByteCntTotal = 0;	

	U08 bJCGroupCnt = tDMA2JCTable->tDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[bJCOrginalInfoIndex].tJCPageGroupInfo[bDMAIndex].wJCGroupCnt;/*当前DMA需要跳列的组数*/
	T_JC_DMA_GROUP_INFO *tJCDMAGroupInfoTemp = tDMA2JCTable->tDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[bJCOrginalInfoIndex].tJCPageGroupInfo[bDMAIndex].tJCDMAGroupInfo;/*取原始跳列信息*/

	if(bJCGroupCnt > 0)
	{
		U16 wJCBaseAddr0_14 = ((((U32)(tDMA2JCTable->tDMA2JCInfo + tDMA2JCTable->dJCInfoCnt)) & 0x7FFF) >>1);
		tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dJCBaseAddr0_14 = wJCBaseAddr0_14; 			
		tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dReserved025_31 = 0;
		
		for(i = 0;i < bJCGroupCnt;i++)
		{
			U16 wJCByteCntCur 		= tJCDMAGroupInfoTemp[i].wJCTotalByteCnt;
			U16 wJCStartColumnCur 	= tJCDMAGroupInfoTemp[i].wJCStartColumn;
			wJCByteCntTotal += wJCByteCntCur;

			U08 bJCByteCntSub 		= 0;
			U16 wJCStartColumnSub 	= 0;
			while(wJCByteCntCur > 0)
			{
				if(wJCByteCntCur <= 14)
				{
					bJCByteCntSub = wJCByteCntCur;
					wJCByteCntCur -= bJCByteCntSub;
				}
				else if(wJCByteCntCur == 32)
				{
					bJCByteCntSub = 0x00;/*4’h0表示要跳过32byte*/
					wJCByteCntCur -= 32;
				}
				else if(wJCByteCntCur == 64)
				{
					bJCByteCntSub = 0x0F;/*4’hF表示要跳过64byte*/
					wJCByteCntCur -= 64;							
				}/*满足以上条件,都只需要配一组跳列信息即可*/
				else/*跳列Byte数大于14,且不等于32和64,就需要配置多组跳列信息*/
				{
					if(wJCByteCntCur > 64)
					{
						bJCByteCntSub = 0x0F;/*4’hF表示要跳过64byte*/
						wJCByteCntCur -= 64;										
					}
					else if(wJCByteCntCur > 32)
					{
						bJCByteCntSub = 0x00;/*4’h0表示要跳过32byte*/
						wJCByteCntCur -= 32;								
					}
					else
					{
						bJCByteCntSub = 0x0E;
						wJCByteCntCur -= 0x0E;
					}								
				}
				
				tDMA2JCTable->tDMA2JCInfo[tDMA2JCTable->dJCInfoCnt].DMA2_JC_INFO.bits.wJCStartColumn0_11 = wJCStartColumnCur + wJCStartColumnSub;
				tDMA2JCTable->tDMA2JCInfo[tDMA2JCTable->dJCInfoCnt].DMA2_JC_INFO.bits.wJCByteCntCur12_15 = bJCByteCntSub;
				
				if(bJCByteCntSub == 0x00)
				{
					wJCStartColumnSub += 32;/*列地址偏移*/
				}
				else if(bJCByteCntSub == 0x0F)
				{
					wJCStartColumnSub += 32;/*列地址偏移*/
				}
				else
				{
					wJCStartColumnSub += bJCByteCntSub;/*列地址偏移*/
				}													
				tDMA2JCTable->dJCInfoCnt++;
			}
		}

		tDMA2JCTable->tDMA2JCInfo[tDMA2JCTable->dJCInfoCnt].DMA2_JC_INFO.wAll = 0xFFFF;/*每组跳列信息后面加一个结束标志*/
		
		if(tDMA2JCTable->dJCInfoCnt%4 == 0x00)/*绕过HW Bug:8byte地址的前两byte为16’hffff*/
		{			
			tDMA2JCTable->dJCInfoCnt += 0x04;
		}
		else
		{
			tDMA2JCTable->dJCInfoCnt++;
		}
		

		if(bDMAIndex == (bPageDMACnt - 1))/*Page最后一个DMA未写满的区域补全*/
		{
			tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dJCByteCnt15_24 = wJCByteCntTotal + wFillSize;
		}
		else
		{						
			tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dJCByteCnt15_24 = wJCByteCntTotal;
		}
		tDMA2JCTable->dJCCfgInfoCnt++;
	}
	else/*当前DMA不需跳列*/
	{
		if(bDMAIndex == (bPageDMACnt - 1))/*Page最后一个DMA未写满的区域补全*/
		{
			wJCByteCntTotal = wFillSize;
		}

		U16 wJCBaseAddr0_14 = ((((U32)((U32)tDMA2JCTable->tDMA2CfgInfo + tDMA2JCTable->dCfgInfoSize)) & 0x7FFF) >>1);
		tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dJCBaseAddr0_14 = wJCBaseAddr0_14;			
		tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dJCByteCnt15_24 = wJCByteCntTotal;
		tDMA2JCTable->tDMA2CfgInfo[tDMA2JCTable->dJCCfgInfoCnt].ALL_JC_CFG_INFO.bits.dReserved025_31 = 0;		
		tDMA2JCTable->dJCCfgInfoCnt++;		
	}				
}

/**************************************************************************
 *函数名称: FDL_JCTableCreat
 *函数功能: 初始化跳列Table
 *入口参数:
 * @param dDMAJCTableAddr 		存放跳列表的基地址
 * @param dJCOrginalInfoAddr 	存放跳列原始信息的基地址(存放了每个DMA从哪个位置跳过多少列的信息)
 *
 *出口参数:Null
 * @return wJCTableBuffSize		跳列表创建完后,占用Buff的大小
 * @SG_Kinder(2018年07月03日(周二))
************************************************************************/
#ifdef __FW__
U16  __section(".CODE_INIT") FDL_JCTableCreat(U32 dDMAJCTableAddr,U32 dJCOrginalInfoAddr)
#else
U16 FDL_JCTableCreat(U32 dDMAJCTableAddr,U32 dJCOrginalInfoAddr)
#endif
{
	/*循环变量*/
	U08 i = 0;
	U08 bChan = 0;
	U08 bDMANum = 0;

	/*局部临时变量*/
	U16 wParityLen;			/*Parity 长度*/
	U16 wFillByteSize;		/*填充Byte数*/
	U16 wUsedByteSize;		/*一个Page中用掉的总Byte数,用于算填充Byte数*/
	U16 wJCTableBuffSize;	/*计算跳列表创建完后,占用Buff的大小*/
	U08 bJCOrginalInfoCnt;	/*跳列原始信息的Buff有效信息的组数(存放了每个DMA从哪个位置跳过多少列的信息)*/
	U16 wJCTotalByteOfPage;	/*一个页总的跳列Byte数*/
	
	T_DMA2_JC_TABLE tDMA2JCTable;
	//T_DMA2_JC_ORGINAL_INFO	*tDma2JCOrginalInfo = 0;


	tDMA2JCTable.tDma2JCOrginalInfoTotal = (T_DMA2_JC_ORGINAL_INFO_TOTAL *)dJCOrginalInfoAddr;/*跳列原始信息指针*/
	//tDma2JCOrginalInfo = tDMA2JCTable.tDma2JCOrginalInfoTotal->tDma2JCOrginalInfo;
	bJCOrginalInfoCnt = tDMA2JCTable.tDma2JCOrginalInfoTotal->tJCUDPParam.bJCOrginalInfoCnt;


	/*UDP物理参数*/
	U16 wPageSize		= PAGE_SIZE;
	U08 bDMAOfClust 	= tDMA2JCTable.tDma2JCOrginalInfoTotal->tJCUDPParam.bClstDmaCnt;
	U08 bUserDataLen	= tDMA2JCTable.tDma2JCOrginalInfoTotal->tJCUDPParam.bUseDataLen;
	U08 bClustOfPage	= tDMA2JCTable.tDma2JCOrginalInfoTotal->tJCUDPParam.bPageClustCnt;		
	U08 bSectorOfPage	= bDMAOfClust*bClustOfPage*tDMA2JCTable.tDma2JCOrginalInfoTotal->tJCUDPParam.bDMASectors;
	U32 bDMACnt 		= bDMAOfClust*bClustOfPage;/*一个Page的DMA个数*/
	U32 dCfgInfoSize	= bJCOrginalInfoCnt*bDMACnt*sizeof(T_DMA2_JC_CFG_INFO);/*跳列配置信息BuffSize(4Byte)*/
	MemorySet((U08 *)dDMAJCTableAddr, 0x00, dCfgInfoSize);/*Memory 清零*/

	
	tDMA2JCTable.dJCInfoCnt			= 0;
	tDMA2JCTable.dJCCfgInfoCnt		= 0;
	tDMA2JCTable.dCfgInfoSize 		= dCfgInfoSize;
	tDMA2JCTable.tDMA2CfgInfo 		= (T_DMA2_JC_CFG_INFO *)dDMAJCTableAddr;/*跳列配置信息基地址*/
	tDMA2JCTable.tDMA2JCInfo 		= (T_DMA2_JC_INFO *)(dDMAJCTableAddr + dCfgInfoSize + sizeof(T_DMA2_JC_INFO));/*跳列区间信息基地址(多偏移2Byte,用来放跳列DMA的区间信息0xFFFF)*/


	((T_DMA2_JC_INFO *)((U32)tDMA2JCTable.tDMA2CfgInfo + tDMA2JCTable.dCfgInfoSize))->DMA2_JC_INFO.wAll = 0xFFFF;/*不跳列DMA的区间信息*/
	wParityLen = HAL_GetParityLen();/*获取Parity Len*/


	for(i = 0;i < bJCOrginalInfoCnt;i++)
	{
		wJCTotalByteOfPage  = tDMA2JCTable.tDma2JCOrginalInfoTotal->tDma2JCOrginalInfo[i].wJCTotalByteOfPage;
		wUsedByteSize		= 	wJCTotalByteOfPage +					/*总的跳列Byte数*/
								bSectorOfPage*BYTE_OF_SEC +				/*有效数据Byte数*/
								bUserDataLen*bClustOfPage +				/*UserData Byte数*/
								wParityLen*bDMAOfClust*bClustOfPage;	/*Parity Byte数*/
		if(wPageSize >= wUsedByteSize)
		{					
			wFillByteSize = wPageSize - wUsedByteSize;					/*Page Size(18592)*/
			//PRINT_HEX("FillSize=",wFillByteSize);
		}
		else/*跳列过多,超出了PageSize的范围*/
		{
			//PRINT_HEX("OV>",wUsedByteSize);
			while(1);
		}
															
		for(bDMANum = 0;bDMANum < bDMACnt;bDMANum++)
		{										
			FDL_JCInfoSet(i,bDMANum,wFillByteSize,&tDMA2JCTable);
		}				
	}			


	/*计算一下JC Buff Size,返回给上层*/
	wJCTableBuffSize = tDMA2JCTable.dCfgInfoSize + (tDMA2JCTable.dJCInfoCnt + 1)*sizeof(T_DMA2_JC_INFO);
//	FDL_Log("JCB>0x%X",wJCTableBuffSize);

	/*配置跳列地址*/
	for(bChan = 0; bChan < 4; bChan++)	//创建4个通道的跳列表
	{		
		/*	
			{FILL_DATA_RND_EN,FILL_DATA_SEL}
			2’b11：在Flash坏列地址上填入随机数
			2’b01：填入由BADCOL_FILL_DATA REG定义的数据
			2’b10（default）、2’b00：填入的数据与该坏列地址区间之后下一个有效列地址上的数据相同
		*/
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.FILL_DATA_RND_EN 			= 1; 	/*在FLASH坏列地址上填入随机数 开关*/
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.FILL_DATA_SEL 				= 1; 	/*在FLASH坏列地址上填入随机数 开关*/	
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.REDUANT_FILL_DATA_RND_EN 	= 1; 	/*在FLASH未写满的区域填入随机数 开关*/	
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.BADCOL_FILL_DATA 			= 0x5A; /*在坏列地址上填入0x5A*/
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.REDUANT_FILL_DATA 			= 0x55; /*在FLASH未写满区域填入0x55*/
		SG_FC(bChan)->rBAD_COLUMN_CTRL.bits.FILT_EN 					= 1; 	/*打开跳列功能*/
		SG_FC(bChan)->rDMA2_FILTER_ECC_CFG_BADDR	= dDMAJCTableAddr; 			/*DMA配置信息地址*/
		SG_FC(bChan)->rBADCOL_INFO_BADDR.bits.dVAL	= dDMAJCTableAddr >> 16; 	/*DMA跳列区间信息地址的高16位		*/
		SG_FC(bChan)->rRND_SEED.bits.dVAL 			= 0x55; /*设置在跳列位置和Page末尾处填充随机数据时所用的Randomize算法的种子*/
	}
	return wJCTableBuffSize;
}
#endif

void Fdl_RaidEnCtrl(U08 bEn)
{
    U08 bChan,bPhyChan;

    for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
    {
        bPhyChan = CHANS(bChan);
        HAL_ADMSetRaidEnable(bPhyChan,bEn);
    }
}


