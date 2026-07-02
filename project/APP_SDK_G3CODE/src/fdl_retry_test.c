
#include "fdl_retry_test.h"
#include "hal_adm.h"
#include "flash_driver.h"
#include "flash_driver_struct.h"
#include "debug.h"
#include "fdl_debug.h"
#include "fdl_task_op.h"
#include "hal_define.h"
#include "hal_ldpc.h"
#include "uitility.h"
#include "hal_cq.h"

#ifdef __INSERT_ERROR_RETRY_TEST_EN__
/**************************************************************************
 *函数名称: FDL_InsertErrorCreatJCTable
 *函数功能: 置错接口所需的相关变量初始化,代码启动初始化
 *入口参数:
 * @param dDma2CfgInfoBAddr 跳列配置信息Buff地址
 * @param dDma2CfgInfoBSize 跳列配置信息Buff大小
 * @param wPageCnt 			Page个数,用于计算DMA个数
 *
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertErrorCreatJCTable(U32 dDma2CfgInfoBAddr,U32 dDma2CfgInfoBSize,U16 wPageCnt)
{
//	wPageCnt = 1;/*只需要创建一个Page的跳列信息表*/
	U16 i;
	U08 bChan;
	U32 dDMACnt 		= wPageCnt*8;
	U32 dCfgInfoSize 	= 4*dDMACnt;/*跳列配置信息BuffSize(4Byte)*/
	U32 dJCInfoSize 	= 4*dDMACnt;/*跳列区间信息BuffSize(2Byte+2Byte结束标志)*/

	if(dCfgInfoSize + dJCInfoSize < dDma2CfgInfoBSize)
	{
		tDMA2JCTable.tDMA2CfgInfo = (T_DMA2_JC_CFG_INFO *)dDma2CfgInfoBAddr;
		tDMA2JCTable.dCfgInfoSize = dCfgInfoSize;

		tDMA2JCTable.tDMA2JCInfo = (T_DMA2_JC_INFO *)(dDma2CfgInfoBAddr + dCfgInfoSize);
		tDMA2JCTable.dJCInfoSize = dJCInfoSize;
	}
	else
	{
		FDL_Log("dDma2CfgInfoBSize Not Enough");
		FDL_Log("******************");
	}

	/*创建4个通道的跳列表*/
	for(i = 0; i < dDMACnt; i++)		//每个DMA2 需要存放的4Byte（2Byte） 个数
	{
		if(i%2 == 0)
		{
			(tDMA2JCTable.tDMA2CfgInfo + i)->ALL_JC_CFG_INFO.bits.dJCBaseAddr0_14	= ((((U32)tDMA2JCTable.tDMA2JCInfo + i*4) & 0x00007FFF) >>1);/*跳列信息基地址*/
			(tDMA2JCTable.tDMA2CfgInfo + i)->ALL_JC_CFG_INFO.bits.dJCByte15_24		= 64;/*当前DMA一共跳列的Byte数(不能超过0x3FF)*/
			(tDMA2JCTable.tDMA2CfgInfo + i)->ALL_JC_CFG_INFO.bits.dReserved25_31 	= 0;/*保留*/
			
			(tDMA2JCTable.tDMA2JCInfo + i)->DMA2_JC_INFO.bits.dDMA2Col0_11			= 0x800;/*从这个DMA的那个列地址开始跳--*/
			(tDMA2JCTable.tDMA2JCInfo + i)->DMA2_JC_INFO.bits.dJCByteCur12_15 		= 0x0F;/*当前次跳列Byte数--0x64Byte*/
			(tDMA2JCTable.tDMA2JCInfo + i)->DMA2_JC_INFO.bits.dJCInfoEndFlag16_31	= 0xFFFF;/*结束标志*/
		}
		else
		{
			(tDMA2JCTable.tDMA2CfgInfo + i)->ALL_JC_CFG_INFO.bits.dJCBaseAddr0_14	= ((((U32)tDMA2JCTable.tDMA2JCInfo + i*4) & 0x00007FFF) >>1);/*跳列信息基地址*/
			(tDMA2JCTable.tDMA2CfgInfo + i)->ALL_JC_CFG_INFO.bits.dJCByte15_24		= 0x00;/*当前DMA一共跳列的Byte数(不能超过0x3FF)*/
			(tDMA2JCTable.tDMA2CfgInfo + i)->ALL_JC_CFG_INFO.bits.dReserved25_31	= 0;/*保留*/
			
			(tDMA2JCTable.tDMA2JCInfo + i)->DMA2_JC_INFO.bits.dDMA2Col0_11			= 0x0FFF;/*从这个DMA的那个列地址开始跳--*/
			(tDMA2JCTable.tDMA2JCInfo + i)->DMA2_JC_INFO.bits.dJCByteCur12_15		= 0x0F;/*当前次跳列Byte数--0x64Byte*/
			(tDMA2JCTable.tDMA2JCInfo + i)->DMA2_JC_INFO.bits.dJCInfoEndFlag16_31	= 0xFFFF;/*结束标志*/
		}
	}

	/*配置跳列地址*/
    for(bChan = 0; bChan < 4; bChan++)	//创建4个通道的跳列表
	{
		SG_FC(bChan)->rDMA2_FILTER_ECC_CFG_BADDR 						= dDma2CfgInfoBAddr; //DMA配置信息地址
		SG_FC(bChan)->rBADCOL_INFO_BADDR.bits.dVAL 						= dDma2CfgInfoBAddr >> 16; //DMA跳列区间信息地址的高16位
    }

}


/**************************************************************************
 *函数名称: FDL_InsertErrorInit
 *函数功能: 置错接口所需的相关变量初始化,代码启动初始化
 *入口参数:
 *Null
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertErrorInit(void)
{
	
	U08 bChan = 0;
	
	tRetryCaseInfo.InserErrorInfo = (INSERT_ERROR_INFO*)MEM_COPYBACK_UD_ADDR;
	tRetryCaseInfo.dBadCnt = 0;/*Error Cnt*/
	tRetryCaseInfo.dSecondBadOfst = 0xFFFFFFFF;
	tRetryCaseInfo.wDebug = 0;
	
	for(bChan = 0; bChan < 4; bChan++)
	{
		tRetryCaseInfo.g_Dma2LenCfg[bChan] = 0x8100800;//SG_DMA2_CH(bChan)->rLEN_CFG.dAll;
		tRetryCaseInfo.g_dDma2En[bChan] = 0x405;//SG_DMA2_CH(bChan)->rENABLE.wAll;
//		LOG_PRINT("g_Dma2LenCfg[%d]=0x%x\n", bChan,tRetryCaseInfo.g_Dma2LenCfg[bChan]);
	}
}

/**************************************************************************
 *函数名称: FDL_InsertErrorInfoInit
 *函数功能: 置错接口所需的相关变量初始化，每次更新置错信息初始化
 *入口参数:
 *Null
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertErrorInfoInit(void)
{
	U32 i = 0;
	U08 bChan = 0;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	U08 *ptData = (U08 *)(&pCmd->dIndex[0]);

	tRetryCaseInfo.InserErrorInfo = (INSERT_ERROR_INFO*)MEM_COPYBACK_UD_ADDR;
	tRetryCaseInfo.dBadCnt = pCmd->dOpByteCnt;/*Error Cnt*/
	tRetryCaseInfo.dSecondBadOfst = 0xFFFFFFFF;
	tRetryCaseInfo.wDebug = 0;
	
#if 0
	for(i = 0; i < tRetryCaseInfo.dBadCnt; i++)
	{
		LOG_PRINT(	"bBadCnt=%d, dBadOffs=%04d,bBadMode=%d,bRetryCnt=%d\n",
					tRetryCaseInfo.dBadCnt,
					tRetryCaseInfo.InserErrorInfo[i].dBadOffs,
					tRetryCaseInfo.InserErrorInfo[i].bBadMode,
					tRetryCaseInfo.InserErrorInfo[i].bRetryCnt);
	}
#endif

	HAL_HostDataTransfer(MEM_DMA_BUFF_ADDR, 1, SATA_HOST_OUT);	//接收DMA数据
	
	for(bChan = 0; bChan < 4; bChan++)
	{
		tRetryCaseInfo.g_Dma2LenCfg[bChan] = 0x8100800;//SG_DMA2_CH(bChan)->rLEN_CFG.dAll;
		tRetryCaseInfo.g_dDma2En[bChan] = 0x405;//SG_DMA2_CH(bChan)->rENABLE.wAll;
//		LOG_PRINT("g_Dma2LenCfg[%d]=0x%x\n", bChan,tRetryCaseInfo.g_Dma2LenCfg[bChan]);
	}
	
	FDL_InsertErrorCreatJCTable(MEM_COPYBACK_SRC_ADDR,MEM_COPYBACK_SRC_SIZE,1);
}

/**************************************************************************
 *函数名称: FDL_InsertError
 *函数功能: 常规置错接口
 *入口参数:
 * @param bChan 		物理通道号
 * @param dOffs 		簇偏移
 * @param pwCol 		列地址指针
 * @param pbNextOpMode 	下一个地址操作模式
 * @param dRow 			Row地址
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertError(U08 bChan, U32 dOffs, U16 *pwCol, U08 *pbNextOpMode, U32 dRow)
{
	U32 i;
		
	for(i = 0; i < tRetryCaseInfo.dBadCnt; i++)
	{
		if(dOffs == tRetryCaseInfo.InserErrorInfo[i].dBadOffs)
		{
			if(ALL_FAIL == tRetryCaseInfo.InserErrorInfo[i].bBadMode)
			{
				*pwCol += 30;
//				LOG_PRINT("*ALL_FAIL\n");
			}
			else if(FIRST_FAIL == tRetryCaseInfo.InserErrorInfo[i].bBadMode)
			{

				if(*pwCol != 0)
				{
					*pwCol -= 64;	//确保第二个DMA不会发生Fail
//					LOG_PRINT("FIRST_FAIL dOffs = %d\n",dOffs);
				}
				else
				{
//					LOG_PRINT("FIRST_NoFAIL dOffs = %d\n",dOffs);
					tRetryCaseInfo.wDebug++;
				}
			}
			else if(ALL_ONE == tRetryCaseInfo.InserErrorInfo[i].bBadMode)
			{
				*pbNextOpMode = 0;
				HAL_CQSendFlashCmd(bChan,0x80);
				HAL_CQSend5Addr(bChan,0,dRow);
//				PRINT("ALL_ONE\n");
			}
			
			break;
		}
	}
}



/**************************************************************************
 *函数名称: FDL_InsertErrorFirstDma
 *函数功能: 特殊置错接口,第一个DMA置错(跳列方式给第一个DMA置错)
 *入口参数:
 * @param bChan 		物理通道号
 * @param dOffs 		簇偏移
 * @param bFlashIdx 	Flash Index
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertErrorFirstDma(U08 bChan, U32 dOffs, U08 bFlashIdx)
{
	U32 i;
	U08 bFailFlag = 0;
	for(i=0; i<tRetryCaseInfo.dBadCnt; i++)
	{
		if(dOffs == tRetryCaseInfo.InserErrorInfo[i].dBadOffs)
		{
			if(FIRST_FAIL == tRetryCaseInfo.InserErrorInfo[i].bBadMode)
			{
				if(bFlashIdx != 0)
				{
					U32 dOfset = offsetof(SG_FC_TypeDef,rBAD_COLUMN_CTRL);
					U32 dVal = 0x690006;
					SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG |(dOfset<<R_CQ_REG_BIT) | ((dVal >> 0) & INVALID_U16);
					SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_FC_REG |((dOfset + 2)<<R_CQ_REG_BIT) | ((dVal >> 16) & INVALID_U16);
					bFailFlag = 1;
//					LOG_PRINT("FIRST_FAIL-bFlashIdx=%d\n",bFlashIdx);
				}
				break;
			}
		}
	}
	
	if(!bFailFlag)
	{
		//不值错，恢复为默认值
		U32 dOfset = offsetof(SG_FC_TypeDef,rBAD_COLUMN_CTRL);
		U32 dVal = 0x0;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG |(dOfset<<R_CQ_REG_BIT) | ((dVal >> 0) & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_FC_REG |((dOfset + 2)<<R_CQ_REG_BIT) | ((dVal >> 16) & INVALID_U16);
	}
}

/**************************************************************************
 *函数名称: FDL_InsertErrorSecondDma
 *函数功能: 特殊置错接口,第二个DMA置错(关UserData方式给第二个DMA置错)
 *入口参数:
 * @param bChan 	物理通道号
 * @param dOffs 	簇偏移
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertErrorSecondDma(U08 bChan, U32 dOffs)
{
	U32 i;
	U08 bFailFlag = 0;
	for(i=0; i<tRetryCaseInfo.dBadCnt; i++)
	{
		if(dOffs == tRetryCaseInfo.InserErrorInfo[i].dBadOffs)
		{
			if(SECOND_FAIL == tRetryCaseInfo.InserErrorInfo[i].bBadMode)
			{
				U16 wDma2En = tRetryCaseInfo.g_dDma2En[bChan];
				U32 dOfset = offsetof(SG_DMA2_TypeDef,rENABLE);
				wDma2En &= ~(1<<2);
				__nds32__dsb();
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfset<<16) | (wDma2En & 0xffff); //关userdata
				bFailFlag = 1;
				tRetryCaseInfo.dSecondBadOfst = dOffs;
//				LOG_PRINT("*SECOND_FAIL-dOffs=%d\n",dOffs);
				break;
			}
		}
	}
	
	if(!bFailFlag)
	{
		//不值错，恢复为默认值
		U32 dOfset = offsetof(SG_DMA2_TypeDef,rENABLE);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfset<<16) | (tRetryCaseInfo.g_dDma2En[bChan] & 0xffff); //还原userdata
	}

}

/**************************************************************************
 *函数名称: FDL_InsertErrorDealRetryCnt
 *函数功能: 处理RetryCnt
 *入口参数:
 * @param bChan 		物理通道号
 * @param dOffs 		簇偏移
 * @param bCurRetryCnt 	当前RetryCnt
 * @param wpCol 		列地址指针
 * @param dRow 			Row地址
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2018年03月30日(周五))
************************************************************************/
void FDL_InsertErrorDealRetryCnt(U08 bChan, U32 dOfst, U08 bCurRetryCnt, U16 *wpCol, U32 dRow)
{

	U16 bOfstIdx;
	for(bOfstIdx=0; bOfstIdx<tRetryCaseInfo.dBadCnt; bOfstIdx++)
	{
		if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].dBadOffs == dOfst)
		{
			break;
		}
	}
	
	if(tRetryCaseInfo.dBadCnt==bOfstIdx)
	{
		SG_FC(bChan)->rBAD_COLUMN_CTRL.dAll = 0x0;
		SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 1;
		return;
	}
//	LOG_PRINT("bOfstIdx=%d\n", bOfstIdx);
	
	if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bBadMode == ALL_FAIL)
	{
		SG_FC(bChan)->rBAD_COLUMN_CTRL.dAll = 0x0;
		SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 1;
		if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt > bCurRetryCnt)
		{
			*wpCol += 30;
		}
//		LOG_PRINT("ALL_FAIL=%d, %d\n", tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt, bCurRetryCnt);
	}
	else if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bBadMode == FIRST_FAIL)
	{
		SG_FC(bChan)->rBAD_COLUMN_CTRL.dAll = 0x0;
		if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt > bCurRetryCnt)
		{
			SG_FC(bChan)->rBAD_COLUMN_CTRL.dAll = 0x690006;
		}
//		LOG_PRINT("FIRST_FAIL=%d, %d\n", tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt, bCurRetryCnt);
		SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 1;
	}
	else if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bBadMode == SECOND_FAIL)
	{
		SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 1;
		if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt > bCurRetryCnt)
		{
			SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
		}
//		LOG_PRINT("SECOND_FAIL=%d, %d,rLEN_CFG=0x%x\n", tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt, bCurRetryCnt,SG_DMA2_CH(bChan)->rLEN_CFG.dAll);
		SG_FC(bChan)->rBAD_COLUMN_CTRL.dAll = 0x0;
	}
	else if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bBadMode == ALL_ONE)
	{
		if(tRetryCaseInfo.InserErrorInfo[bOfstIdx].bRetryCnt>bCurRetryCnt)
		{
			HAL_MCU_SendCmd(bChan,FL_CMD_PAGE_PROG1);
			HAL_MCU_SendTwoAddr(bChan,0x00);
			HAL_MCU_SendThreeAddr(bChan,dRow);
		}
	}
	__nds32__dsb();
}

#endif



