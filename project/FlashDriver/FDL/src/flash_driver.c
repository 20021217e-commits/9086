
#include "flash_driver.h"
#include "fdl_task_op.h"
#include "hal.h"
#include "fdl_debug.h"
#include "fdl_copyback.h"
#include "fdl_erase_block.h"
#include "fdl_read.h"
#include "fdl_write.h"
#include "fdl_exception.h"
#include "hal_define.h"
#include "fdl_common.h"
#include "scsi.h"
#include "uitility.h"
#include "hal_pmcp.h"
#ifdef __GRAY_BOX_TEST__
#include "fdl_PValueSet.h"
#endif

U32 g_dPAD_dTSEL_CTRL;

#ifdef __FW__
void  __section(".CODE_INIT") FDL_Init(T_FDL_PARAM *ptFdlParam)
#else
void   FDL_Init(T_FDL_PARAM *ptFdlParam)
#endif
{
	U8 i;
	U8 bEccCorrMode;
	U16 wParityLength = 0;

    U08MemCopy((U8 *)(&pFdlParam),(U8 *)ptFdlParam,sizeof(T_FDL_PARAM));

#ifdef __RETRY_TABLE_RATE__
	ptFdlParam->pwRRTUseCnt = wRRTUseCnt;
	ptFdlParam->wRRTUseCntBuffSize = sizeof(wRRTUseCnt);
#endif

    HAL_Init(&ptFdlParam->ptUdpFdl->m_tHwReg);

    HAL_BFCInit();
    HAL_PMCPInit();

//	T_REG_CFG *ptHwReg = &ptFdlParam->ptUdpFdl->m_tHwReg;
//	bEccCorrMode = ((ptHwReg->ADM_dDma2EccCtrl>>8) & 0x0f);//ldpc mode
//	wParityLength = HAL_LDPCParityLen[bEccCorrMode-5];
//
//	if(((ptHwReg->ADM_dDma2RndCtrl>>6)&1) == 1)
//	{
//		if(((ptHwReg->ADM_dDma2RndCtrl>>5)&1) == 1)
//		{
//			wParityLength = wParityLength + 4;
//		}
//		else//bch mode
//		{
//			wParityLength = wParityLength + 2;
//		}
//	}

	pFdlParam.ptUdpFdl->m_tHwReg.ADM_gParityLen = wParityLength;

#ifdef __FILL_PAGE_EN__
	T_FLASH_PARAM *ptFlashParam = &ptFdlParam->ptUdpFdl->m_tFlashParam;

#if 1//(NAND_FLASH != YMTC_B08T)

//	HAL_NFFillPage(ptFlashParam->m_bDataSectOfDMA * 512, wParityLength, ptFlashParam->m_bSectOfDataClst, 
//				   ptFlashParam->m_bDataUserDataLen, ptFlashParam->m_bDataSectOfPage, 
//				   ptFlashParam->m_wPageSize, (U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]));
//	HAL_NFFillPage(ptFlashParam->m_bTabSectOfDMA * 512, wParityLength, ptFlashParam->m_bSectOfTableClst, 
//				   ptFlashParam->m_bTabUserDataLen, ptFlashParam->m_bTabSectOfPage,
//				   ptFlashParam->m_wPageSize, (U32)(&HAL_JCBufff[JC_BUFF_TABLE_ST]));

#else
	FDL_JCOrginalInfoInitForB08T((U32)(&tChTaskQueue[0]),1024,TYPE_LOGIC);
	FDL_JCTableCreat((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),(U32)(&tChTaskQueue[0]));
	
	FDL_JCOrginalInfoInitForB08T((U32)(&tChTaskQueue[0]),1024,TYPE_TABLE);
	FDL_JCTableCreat((U32)(&HAL_JCBufff[JC_BUFF_TABLE_ST]),(U32)(&tChTaskQueue[0]));
#endif

				   
#endif

    tFtlTask.wCur = 0;
    tFtlTask.wLen = 0;
	gFdlTskStatus = ST_TASK_DONE;
	
#ifdef __BADBLK_BINDMAP__
	ptBind = (T_FDL_BADBLK_BINDMAP_DATA *)(pFdlParam.ptUdpFdl->m_tHwReg.LDPC_dMemMtxA);
	for (i=0;i<CHAN_MAX_CNT;i++)
	{
		U8 bphyChan = CHANS(i);
		bPhy2LogMapChan[bphyChan] = i;
	}
#endif
	
	//根据通道配置自动匹配阻抗
	U08 bTSELOld = (pFdlParam.ptUdpFdl->m_tHwReg.PAD_dTSEL_CTRL & 0x03);
	U32 dTSELNew = 0;

	for(i = 0;i < CHAN_MAX_CNT;i++)
	{
		dTSELNew += (bTSELOld << (CHANS(i)*2 + 0));
		dTSELNew += (bTSELOld << (CHANS(i)*2 + 8));
		dTSELNew += (bTSELOld << (CHANS(i)*2 + 16));
	}
	g_dPAD_dTSEL_CTRL=dTSELNew;
	SG_PADCTRL->rPAD_TSEL_CTRL0.dAll = g_dPAD_dTSEL_CTRL;//初始化阻抗配置


#ifdef __RETRY_CNT_RECORD__

	U08 bChan,bChip;
	U08 bPhyChan,bPhyChip;

	for(bChan = 0;bChan<CHAN_MAX_CNT;bChan++)
	{
		for(bChip = 0;bChip<LUN_OF_CHAN;bChip++)
		{
			bPhyChan = CHANS(bChan);
			bPhyChip = CHIPES(bPhyChan, bChip);

			bPhyToLogChan[bPhyChan] = bChan;
			bPhyCeToLogLun[bPhyChip] = bChip;
		}
	}
#endif	

}

void FDL_FlashDiffSignalEn(U8 bChan, U8 bChip, U8 ONOFF)
{
	U08 bData[4] = {0,0,0,0};
	U08 FeatureAddress;
#if((NAND_FLASH == MIC_B57T)||(NAND_FLASH == MIC_B47R)||(NAND_FLASH == MIC_N48R)||(NAND_FLASH == MIC_N58R)||(NAND_FLASH == MIC_N69R)||(NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	FeatureAddress = 0x02;
	if(1==ONOFF)
	{
		bData[0] = 0x06;//DQS_t & RE_t Enable
	}
	else
	{
		bData[0] = 0x00;//DQS_t & RE_t Disable
	}
	HAL_NFSetFeature(bChan, bChip, FeatureAddress, &bData[0], 1);
#endif
}


#ifdef __VMC_EN__
U8 __section(".fdl_init") FDL_FlashIFModeInit(U8 bChan, U8 bChip)
#else
U8  FDL_FlashIFModeInit(U8 bChan, U8 bChip)
#endif
{
	FDL_FlashWPCtrl(1);//关闭写保护
	
	U08 bShallowEraseEn = 1;
	U08 bData[4] = {0,0,0,0};
	U08 bSetFailCnt = 0;
    U08 bResetSts,bHardResetSts;
	U08 bSetMose = 0;/*Set Feature 模式:0-写脉冲模式,1-DQS模式，用于上电就是Toggle模式的Flash*/

	if(bChan&0xF0)/*读ID不需要配置浅擦除*/
	{
		//bShallowEraseEn = 0;
		bChan &= 0x0F;
	}
#if((NAND_FLASH == MIC_B27B))
		bSetMose = 1;/*B27B上电就是Toggle模式且不支持其他模式,因此要用DQS模式去Set Feature*/
		SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x2;
		
		//B16A flag check functiong
		bData[0] = 00;
		HAL_NFSetFeature(bChan, bChip, 0xDF, &bData[0], bSetMose);

		bData[0] = 2;
		HAL_NFSetFeature(bChan, bChip, 0x10, &bData[0], bSetMose);
#elif((NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	bData[0] = 0x20;//DDR
	bData[2] = 0x01;//Vccq 1.2
	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x00;
	HAL_NFSetFeature(bChan, bChip, 0x01, &bData[0], 0);
	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
#endif
	delay_us(10);


	if (SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL ==1) //系统时钟源400MHz,TRX2时钟1分频DQS 200M,开flash差分
	{
		FDL_FlashDiffSignalEn(bChan, bChip, 1);
	}
	else// if (SG_CPM->rSYS_CLK_SOURCE_SEL==8U && SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL==2) //系统时钟源400Mhz，TRX2时钟2分频DQS 100M,关flash差分
	{
		FDL_FlashDiffSignalEn(bChan, bChip, 0);
	}


//#if (NAND_FLASH == TOS_BICS3)		
//        HAL_NFResetFlash(bChan,bChip,FL_CMD_8T23_RESET_FLASH);
//#endif
//
//        bResetSts = HAL_NFResetFlash(bChan,bChip,FL_CMD_RESET_FLASH);
//        bHardResetSts = HAL_NFResetFlash(bChan,bChip,FL_CMD_HARD_RESET_FLASH);
//
//        if((bResetSts == FALSE) || (bHardResetSts == FALSE))//复位或硬复位只要有1个fail，就返回2
//        {
//            return 2;
//        }
//
//
//#if (NAND_FLASH == MIC_N28)
//		U08 bDataIn[4] = {0x03,0x00,0x00,0x00};
//		U32 dRow = ((0x00<<16)|(0x80<<8)|0xC9);
//		HAL_CQChipSelect(bChan, bChip);
//		HAL_CQSendFlashCmd(bChan, 0xEB);
//	    HAL_CQSendRowAddr(bChan,dRow);
//		HAL_CQWaitDone(bChan);
//		
//		HAL_MCUSendNByteDataByDqs(bChan,bDataIn,4);
//		HAL_MCUCheckStatus(bChan, bChip);
//
//
//		HAL_CQChipSelect(bChan, bChip);
//		HAL_CQSendFlashCmd(bChan, 0xEA);
//	    HAL_CQSendRowAddr(bChan,dRow);
//		HAL_CQWaitDone(bChan);
//		
//		HAL_MCUCheckStatus(bChan, bChip);
//		bDataIn[0] = 0x00;
//		HAL_McuSendReadPluseGetData(bChan,bDataIn,4);
//#endif
//
//
//
//#if (NAND_FLASH == MIC_M16)
//		//N16 flag check functiong
//		bData[0] = 0x10;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0xDF, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//		bData[0] = 0;
//#endif
//
//#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)||(NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B))
//		//B16A flag check functiong
//		bData[0] = 00;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0xDF, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//
//		bData[0] = 2;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x10, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//
//		bData[1] = 0x00;
//#endif
//
//#if  (NAND_FLASH == MIC_B16A)
//		U08 bK;
//        if(	(bShallowEraseEn == 1) &&					/*读ID不需要配置浅擦除*/
//			(FDL_ONFIParameterGet(bChan,bChip)) &&		/*非镁光Flash也不需要配置浅擦除*/
//			(FALSE == FDL_ShallowEraseEn(bChan, bChip)))/*是否需要浅擦除配置*/
//        {
//            //PRINT_TXT("SSE");
//            bData[0] = 0x2F;
//            HAL_NFSetExtFeature(bChan, bChip, 0xDF, 0x80, 0, &bData[0],1);//设置浅擦除相关            
//            bData[0] = 0;
//            
//            for(bK = 0; bK < 4;bK++)
//            {
//                HAL_NFGetExtFeature(bChan, bChip, (0xE0+bK), 0x02, 0, &bData[0],1);            
//                //PRINT_HEX("Get=",bData[0]);
//                if(bData[0] > 0x06)
//				{
//					bData[0] -= 0x6;
//
//				}
//				else
//				{
//					bData[0] = 0x00;
//
//				} 
//                HAL_NFSetExtFeature(bChan, bChip, (0xE0+bK), 0x02, 0, &bData[0],1);//设置浅擦除相关            
//            }
//
//            bData[0] = 0x2D;
//            HAL_NFSetExtFeature(bChan, bChip, 0xDF, 0x80, 0, &bData[0],1);//设置浅擦除相关            
//        }
//#endif
//
//
//#if(NAND_FLASH == MIC_B0KB)
//
//#ifdef __L06B_TO_B0KB__/*L06B转B0KB*/
//		HAL_L06B2B0KB(bChan, bChip);
//#endif
//
//		bData[0] = 0x00;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x92, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//
//		bData[0] = 2;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x10, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//#endif
//		if ((SG_CPM->rSYS_CLK_SOURCE_SEL==8U) && (SG_CPM->rTRX2_CLK_PERIOD.bits.dVAL==1)) //系统时钟源400Mhz，TRX2时钟1分频DQS 200M
//		{
//			//FDL_Log("SetFeature differenal mode");
//			bData[0] = 0x07;
//		}
//		else	//关差分配置
//		{
//			bData[0] = 0x00;
//		}
//		
//#if (NAND_FLASH == SUMSUNG_3DV4)	
//		HAL_NFSetFeatureWithCheck(bChan, bChip, 0x02, &bData[0],1,bSetMose);/*DQS设置开差分[1]*/
//#elif (NAND_FLASH == TOS_BICS3)
//		/*8T23由于不确定上电的模式,开差分时,分别用写脉冲和DQS方式Set Feature,总有一个要成功*/
//		/*8T23由于不确定上电的模式,开差分时,分别用写脉冲和DQS方式Set Feature,总有一个要成功*/
//		HAL_NFSetFeatureWithCheck(bChan, bChip, 0x02, &bData[0],1,bSetMose);/*写脉冲设置开差分[0]*/
//		HAL_NFSetFeatureWithCheck(bChan, bChip, 0x02, &bData[0],1,!bSetMose);/*DQS设置开差分[1]*/
//#else
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x02, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//#endif
//
//#if (NAND_FLASH == H27QEG8NDB) 	//8NDB Flash本身的驱动能力设置  
//		//FDL_Log("SetFeature flash driver = %x",FLASH_DRIVER);
//		bData[0] = FLASH_DRIVER;//FLASH_DRIVER;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x10, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//#endif	
//
//	//set toggle 2.0
//#if ((NAND_FLASH == K9ADGD8U0C) || (NAND_FLASH == K9ADGD8U0A)) 	
//		bData[0] = 0x07;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x02, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//#elif (NAND_FLASH == TOS_BICS3)	
//
//		bData[0] = 0x00;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x80, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//		SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
//		
//#elif (NAND_FLASH == HYNIX_3DV4)		
//		bData[0] = 0x20;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x01, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//		
//#elif (NAND_FLASH == SUMSUNG_3DV4)/*SUMSUNG_3DV4上电就是Toggle模式*/
//
//#elif ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B))/*B27B上电就是Toggle模式*/
//
//#elif (NAND_FLASH == MIC_N28)
//		bData[0] = 0x37;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x01, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//#else
//		bData[0] = 0x27;
//		if(!HAL_NFSetFeatureWithCheck(bChan, bChip, 0x01, &bData[0],1,bSetMose))
//		{
//			bSetFailCnt++;
//		}
//#endif
		SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
		HAL_CQWaitDone(bChan);
//		HAL_MCUCheckStatus(bChan, bChip);
	delay_us(10);

	Fdl_ClearExceptionState(bChan);

	if(bSetFailCnt == 0)//没有fail，返回0
	{
		return 0;
	}
	else
	{
	
#ifdef __RETRY_CNT_RECORD__
		U08 bLogChan = bPhyToLogChan[bChan];
		U08 bLogLun  = bPhyCeToLogLun[bChip];
		bFlashInitFail[bLogChan][bLogLun]++;
#endif

		return 1; //set/get feature fail，则返回1
	}
}

#ifdef __VMC_EN__
void __section(".fdl_init") FDL_FlashWPCtrl(U08 bMode)
#else
void FDL_FlashWPCtrl(U08 bMode)
#endif
{
	SG_PADCTRL->rPORT_MODE[15].dAll &= ~(1 << 6);
	if(bMode == 0)//写保护
	{
		SG_PADCTRL->rPORT_DATA[15].dAll &= ~(1 << 6);
	}
	else//非写保护
	{
		SG_PADCTRL->rPORT_DATA[15].dAll |= (1 << 6);
	}
}
#ifdef __FW__
#include  "ApCommon.h"
extern EN_DEVICE_MODE enDeviceMode;
#endif
U08 FDL_CreateTask(FDL_TASK_CTRL *ptFDL_TaskCtrl)
{
	//U08 bTaskType =tFtlTask.dTaskType;
	tFtlTask.dEccStsBuff   = pFdlParam.dEccInfoBuffAddr;
	
#ifdef __GRAY_BOX_TEST__
	FDL_GrayBoxRetryOption();
#endif
	FDL_CheckMultiLunReady();
    if (gFdlTskStatus >= ST_TASK_DONE)
    {
#ifdef __FW__
		if (0 == SG_MCU->rREG_LED_CTRL.bits.LED0_CTRL_EN && M_Normal == enDeviceMode)
		{
			HAL_LED_Config(LED0_NUM,LED_MODE_ON,50);//正常设备busy指示灯：闪烁，内部回收也闪烁，方便观察设备动态
		}
#endif
#ifdef __DYNAMIC_PAD_TSEL_CTRL__
    	if((pFdlParam.ptUdpFdl->m_tHwReg.PAD_dTSEL_CTRL != 0) && (SG_PADCTRL->rPAD_TSEL_CTRL.dAll == 0))
    	{
    		SG_PADCTRL->rPAD_TSEL_CTRL.dAll = g_dPAD_dTSEL_CTRL;
    	}
#endif
#ifdef __SIMPLIFY_CODE__ //没有task并行机制，异常buff都会让在做这次task时分析完。反正就清个头
        U08MemSet((U8 *)(ptFDL_TaskCtrl->dReadExBuffAddr),0,sizeof(T_READ_EXCEPTION));
        U08MemSet((U8 *)(ptFDL_TaskCtrl->dWriteExBuffAddr),0,sizeof(T_WRITE_EXCEPTION));        
        U08MemSet((U8 *)(ptFDL_TaskCtrl->dEraseExBuffAddr),0,sizeof(T_ERASE_EXCEPTION));
#else
		if (ptFDL_TaskCtrl->dReadExBuffAddr != 0) 
		{
			if(	bTaskType == TYPE_READ_TO_BUFFER||
				bTaskType == TYPE_READ_TO_BUFFER_4K||
				bTaskType == TYPE_READ_LITTLE_DATA||
				bTaskType == TYPE_NCQ_READ||
				bTaskType == TYPE_NCQ_READ_4K||
				bTaskType == TYPE_VERIFY_TLC_PAGE||
				bTaskType == TYPE_FILL_BLOCKS||
				bTaskType == TYPE_COPY_BACK||
				bTaskType == TYPE_MULTI_LUN_EXDIE_COPYBACK_PRG||
				bTaskType == TYPE_MULTI_LUN_INDIE_COPYBACK_PRG||
				bTaskType == TYPE_MULTI_LUN_EXDIE_COPYBACK_ORDER_PRG||
				bTaskType == TYPE_MULTI_LUN_INDIE_COPYBACK_ORDER_PRG)
			{
				//clear read Exception head info
	        	U08MemSet((U8 *)(ptFDL_TaskCtrl->dReadExBuffAddr),0,sizeof(T_READ_EXCEPTION));
	  		}
		}
	
		if (ptFDL_TaskCtrl->dWriteExBuffAddr != 0)
		{
			if(	bTaskType == TYPE_WRITE_PAGES||
				bTaskType == TYPE_NCQ_WRITE||
				bTaskType == TYPE_FILL_BLOCKS||
				bTaskType == TYPE_COPY_BACK||
				bTaskType == TYPE_MULTI_LUN_EXDIE_COPYBACK_PRG||
				bTaskType == TYPE_MULTI_LUN_INDIE_COPYBACK_PRG||
				bTaskType == TYPE_MULTI_LUN_EXDIE_COPYBACK_ORDER_PRG||
				bTaskType == TYPE_MULTI_LUN_INDIE_COPYBACK_ORDER_PRG)
				{
		        	//clear write Exception head info
		        	U08MemSet((U8 *)(ptFDL_TaskCtrl->dWriteExBuffAddr),0,sizeof(T_WRITE_EXCEPTION));
				}
		}

		if (ptFDL_TaskCtrl->dEraseExBuffAddr != 0)
		{
			if(bTaskType == TYPE_ERASE_BLOCKS)
			{
	        	//clear erase Exception head info
				U08MemSet((U8 *)(ptFDL_TaskCtrl->dEraseExBuffAddr),0,sizeof(T_WRITE_EXCEPTION));
			}
		}
#endif
        tFtlTask.tTaskCtrl = ptFDL_TaskCtrl; //////HAL_MemCopy(&tFtlTask.tTaskCtrl,ptFDL_TaskCtrl,sizeof(FDL_TASK_CTRL));
        
		HAL_PMCPMemorySet(PMCP0,(U32)(&tChTaskQueue[0]),0,MAX_CHAN_COUNT*sizeof(T_CHAN_TASK_QUEUE));
		HAL_PMCPMemorySet(PMCP0,tFtlTask.dEccStsBuff,0,pFdlParam.dEccInfoBuffSize);
		
		gFdlTskStatus = ST_DOING;
        return TRUE;
    }
    return INVALID_U8;
}

void FDL_ClrRetryReg(U08 bLogChan,U08 bLogLun)
{
#ifdef __RETRY_PERSISTENCE__/*开启Retry保持*/				
	U08 i = 0;
	U32 dRow = 0;
	U08 bData[4] = {0,0,0,0};
    U08 bChan,bChip;

    if(bRetryPersistence[bLogChan][bLogLun])
    {
        //PRINT_TXT("ClrRetryReg");
        
        bChan    = CHANS(bLogChan);
        bChip    = PHYCHIP_OF_LOGLUN(bChan, bLogLun);
    
#if (NAND_FLASH == TOS_BICS3)/*8T23 & 9T23*/
        Retry_SetFlashReg(bChan, bChip, 0,0x01000000);  
        Retry_SetFlashReg(bChan, bChip, 0,0x02000000);  
        Retry_SetFlashReg(bChan, bChip, 0,0x03000000);  
#elif(NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)/*AFGA*/
        Retry_SetFlashReg(bChan, bChip, 0,0xFF000007);
        Retry_SetFlashReg(bChan, bChip, 0,0xFF000008);
#endif		
        Retry_SetFlashReg(bChan, bChip, 0,dRow);//清除slc & Option Flash

        Retry_SetFlashReg_Arc(bChan, bChip, 0x00);  //关闭并清零ARC功能
        Retry_SetArcStartOffset(bChan,bChip,bData);
        
#if (READ_RETRY_ARC_MAX != 0)
        for(i = 0;i < sizeof(bFeatureAddrArray);i++)
        {
            U08 bFeatureAddr = bFeatureAddrArray[i];
            Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bFeatureAddr, bData);
        }
#endif
        
        bRetryPersistence[bLogChan][bLogLun] = FALSE; //清除后标志
    }
#endif
}

U32 FDL_GetCurLocal(U08 bDir)
{
	if(bDir)//Read
	{
		return (SG_DMA1->rINDEX / SECT_OF_DATA_CLST);
	}
	else//Write
	{
		U08 bChan 		= 0;
		U08 bChanPhy 	= 0;
		U32 MaxVaul 	= 0;
		
		for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
		{
			bChanPhy = CHANS(bChan);
			
		    if(SG_DMA2_CH(bChanPhy)->rBUF_INDEX_LDPC > MaxVaul)
		    {
		        MaxVaul = SG_DMA2_CH(bChanPhy)->rBUF_INDEX_LDPC;
		    }
		}
		
		return (MaxVaul / SECT_OF_PAGE);
	}

}


U32 FDL_OpDmaBuffIndex(U08 bChan,U08 bClear)
{
	U32 dBuffIndex = 0;
	if(bChan == U8_ALL_ONE)
	{
		if(bClear)
		{
			HAL_ADMClearDMA1Index();
			return (0);
		}
		dBuffIndex = HAL_ADMGetDMA1Index();/*Get DMA1 Index*/
	}
	else if(bChan < MAX_CHAN_COUNT)
	{
		U08 bChanPhy 	= 0;
		bChanPhy = CHANS(bChan);
		if(bClear)
		{
			HAL_ADMClearDMA2Index(bChanPhy);
			return (0);
		}
		dBuffIndex = HAL_ADMGetDMA2Index(bChanPhy);/*Get DMA2 Index*/ 
	}
	else
	{
		dBuffIndex = 0xFFFFFFFF;/*异常值*/
	}

	return dBuffIndex;
}


U08 FDL_ContinueTask(U16 wCount)
{
	U32 dBaseLen = tFtlTask.wLen;
    
#ifdef __SLC_PRG_FAIL_HANDL__
    if(tFtlTask.dTaskType == TYPE_NCQ_WRITE)
    {
        if(((T_WRITE_EXCEPTION *)tFtlTask.tTaskCtrl->dWriteExBuffAddr)->bDescript != 0)//编程fail则退出
        {
           return FALSE;
        }
    }
#endif

	tFtlTask.wLen += wCount;
	
#ifdef __RECORD_CONTINUE_COUNT__/*记录Continue每段个数,便于分析*/
	*(U16 *)(&tFtlTask.bArgsArr[(tFtlTask.bArgsArr[42]%20)*2 + 2]) = wCount;/*2-41用来记录*/
	tFtlTask.bArgsArr[42]++;
#endif

#ifdef __TASK_ERROR_FORCE_STOP__
	if(Fdl_GetTaskHoldFlag())
	{
		return FALSE;/*实时检测强制停掉任务标志,在FDL_WaitTaskDone处理*/
	}
#endif

	RunFtlTaskErgodic(TASK_ADD_SAG);

	if((tFtlTask.dTaskType == TYPE_NCQ_READ) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K))
	{
		U08 bChan,bChanPhy;
		for(bChan = 0; bChan < CHAN_MAX_CNT; bChan++)
		{
			bChanPhy = CHANS(bChan);
			if(	(tChTaskQueue[bChanPhy].wTaskCnt == 0) &&	/*队列里面CQ扔完了*/
				(tChTaskQueue[bChanPhy].wTaskHead > 0)) 	/*有配过CQ*/
			{
				while(HAL_CQGetIdleSpace(bChanPhy) < 0x07);	/*保证CQ不填爆*/
				HAL_CQCheckReadEnd(bChanPhy,1);
//				Fdl_CheckCacheRDEnd(bChanPhy);
			}
		}
		
#ifdef __FDL_HW_VIRTUL_CLST__
		if(wCount == 0)
		{
			while(1)
			{
				if(	(tFtlTask.wLen == tFtlTask.wCur) &&
					(0 == tChTaskQueue[0].wTaskCnt)  &&
					(0 == tChTaskQueue[1].wTaskCnt)  &&
					(0 == tChTaskQueue[2].wTaskCnt)  &&
					(0 == tChTaskQueue[3].wTaskCnt))
				{
					for(bChan = 0; bChan < CHAN_MAX_CNT; bChan++)
					{
						bChanPhy = CHANS(bChan);
						if(tChTaskQueue[bChanPhy].wTaskHead > 0)//有配过CQ
						{
							while(HAL_CQGetIdleSpace(bChanPhy) < 0x07);	/*保证CQ不填爆*/
							HAL_CQCheckReadEnd(bChanPhy,1);
//							Fdl_CheckCacheRDEnd(bChanPhy);
						}
					}
					break;
				}
#ifdef __TASK_ERROR_FORCE_STOP__
				else if(Fdl_GetTaskHoldFlag())
				{
					break;/*强制停掉任务,在FDL_WaitTaskDone处理*/
				}
#endif
			}
		}
	}
#else
		if(wCount != 0)
		{
			Fdl_VirtualChanSet(dBaseLen);
		}
		else//continue end, add check hold
		{

			while(1)
			{
				if(	(tFtlTask.wLen == tFtlTask.wCur) &&
					(0 == tChTaskQueue[0].wTaskCnt)  &&
					(0 == tChTaskQueue[1].wTaskCnt)  &&
					(0 == tChTaskQueue[2].wTaskCnt)  &&
					(0 == tChTaskQueue[3].wTaskCnt))
				{
					for(bChan = 0; bChan < CHAN_MAX_CNT; bChan++)
					{
						bChanPhy = CHANS(bChan);
						if(tChTaskQueue[bChanPhy].wTaskHead > 0)//有配过CQ
						{
							while(HAL_CQGetIdleSpace(bChanPhy) < 0x07);	/*保证CQ不填爆*/
							HAL_CQCheckReadEnd(bChanPhy,1);
//							Fdl_CheckCacheRDEnd(bChanPhy);
						}
					}
					break;
				}
#ifdef __TASK_ERROR_FORCE_STOP__
				else if(Fdl_GetTaskHoldFlag())
				{
					break;/*强制停掉任务,在FDL_WaitTaskDone处理*/
				}
#endif
			}
		}
	}

#endif
	return TRUE;
}


void FDL_WaitTaskDone(void)
{
    while(TRUE)
    {		
		if( (tFtlTask.wLen == tFtlTask.wCur) &&
			(0 == tChTaskQueue[0].wTaskCnt)	 &&
			(0 == tChTaskQueue[1].wTaskCnt)	 &&
			(0 == tChTaskQueue[2].wTaskCnt)	 &&
			(0 == tChTaskQueue[3].wTaskCnt)	 &&
			(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(0)) &&
			(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(1)) &&
			(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(2)) &&
			(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(3)))
		{
			gFdlTskStatus = ST_TASK_DONE;
		}
#ifdef __TASK_ERROR_FORCE_STOP__
		else if(Fdl_GetTaskHoldFlag())
		{
			FDL_StopTaskForce();
			gFdlTskStatus = ST_TASK_ERROR_DONE;/*标志Task异常Done*/
			
#if 1/*Debug @kinder*/
			SG_CPM->rSOFT_GATE0.bits.FC0_TRX2_CLK = 1;
			SG_CPM->rSOFT_GATE0.bits.FC1_TRX2_CLK = 1;
			SG_CPM->rSOFT_GATE0.bits.FC2_TRX2_CLK = 1;
			SG_CPM->rSOFT_GATE0.bits.FC3_TRX2_CLK = 1;
#endif
			
			if(pFdlParam.FTL_HandleTaskErrorDone != NULL)
			{
				pFdlParam.FTL_HandleTaskErrorDone();
			}
			
		}
#endif
        if (gFdlTskStatus >= ST_TASK_DONE)
        {
            SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0x18C0;
#ifdef __TASK_ERROR_FORCE_STOP__
			Fdl_UpdateTaskTime(0xFF);
#endif
            break;
        }
    }
    U8 i;
	for(i = 0;i<4;i++)
	{
		SG_DMA2_CH(i)->rECC_CFG.bits.PT_EN = 0;
		SG_DMA2_CH(i)->rECC_CFG.bits.PT_NUM = 0;//pFdlParam.ptUdpFdl->m_tHwReg.ADM_gTabPuncLen;
	}

	//PRINT_HEX("DDDD!>",9);
}

void FDL_MCWaitDMA1Done(void)
{
#ifdef __SLC_PRG_FAIL_HANDL__
    while(SG_DMA1->rMC_STS.bits.MC_DONE == 0)
    {
        if(((T_WRITE_EXCEPTION *)tFtlTask.tTaskCtrl->dWriteExBuffAddr)->bDescript != 0)//编程fail则退出
        {
            //PRINT_HEX("fail dma1=",SG_DMA1->rINDEX);
            break;
        }
    }
#endif
}

void FDL_ResetFlash(U8 bChan, U8 bChip)
{
    bChan = CHANS(bChan);
    bChip = CHIPES(bChan, bChip);

    HAL_NFResetFlash(bChan, bChip,FL_CMD_RESET_FLASH);
}

void FDL_ResetLun(U8 bChan, U8 bLun)
{
    U8 bChip = bLun / LUN_OF_CHIP;

    bChan = CHANS(bChan);
    bChip = CHIPES(bChan, bChip);
	
    HAL_NFResetLun(bChan, bChip, bLun, (bLun * ROW_GAP_OF_LUN));
}

	
		

					


	//等所有CQ执行完毕


void FDL_CheckMultiLunReady(void)
{
	U08 bChip;
	U08 bChipPhy;
	U08 bChan;
	U08 bChanPhy;
	U08 bLun;
	U08 bSecondCheckBit = FIO_PF_PASS_NULL;

	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);
		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
		{
			bChipPhy = CHIPES(bChanPhy,bChip);

			for(bLun = 0;bLun < LUN_OF_CHIP;bLun++)
			{	
				
				if(	(TYPE_NCQ_WRITE == tFtlTask.dTaskType) ||
					(TYPE_WRITE_PAGES == tFtlTask.dTaskType))
				{
					U32 dLogNum = (SG_FC(bChanPhy)->rLOGIC_NUM & INVALID_U16) | (bChip<<PRE_OP_CNT_IN_LOGNUM);	 
    				HAL_CQCfgLogicNum(bChanPhy,dLogNum);
    				bSecondCheckBit = FIO_PF_PASS_SLC_WRITE;
				}

				HAL_CQChipSelect(bChanPhy,bChipPhy);
				HAL_CQCheckIOStatus(bChanPhy,bChipPhy, IO_1_PASS, FIO_RB_READY_ARRAY, bSecondCheckBit);
				HAL_CQSendFlashCmd(bChanPhy,FL_CMD_EXIT_CHK_IOSTA);
			}
		}
	}	
	
	//等所有CQ执行完毕
	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++) 
	{
		bChanPhy = CHANS(bChan);
		if (!HAL_CQWaitDone(bChanPhy))
		{
		    FDL_Log("CheckMultiLunStatus TimeOut!");
		}
	}
}

void FDL_GetDevAllLunDwlsvBlkListStat(U08 *pbDevLunStat)
{
#if((NAND_FLASH == MIC_B16A) ||\
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_M16)	 ||\
	(NAND_FLASH == MIC_N18)  ||\
	(NAND_FLASH == MIC_B27A) ||\
	(NAND_FLASH == MIC_B27B) ||\
	(NAND_FLASH == INTEL_B27B))
	U08 i;
	U08 bChip,bChipPhy;
	U08 bChan,bChanPhy;
	U08 bData[4];
	U08 bStatIdx = 0;

	for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
	{
		for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
		{
			bChanPhy = CHANS(bChan);
			bChipPhy = CHIPES(bChanPhy,bChip);

			HAL_CQChipSelect(bChanPhy,bChipPhy);
			HAL_CQCheckIOStatus(bChanPhy,0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
			HAL_CQSendFlashCmd(bChanPhy,0xD4);
			HAL_CQSendColAddr(bChanPhy, 0xDC00);
		}
	}

	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);
		HAL_CQWaitDone(bChanPhy);
	}

	for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
	{
		for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
		{
			bChanPhy = CHANS(bChan);
			bChipPhy = CHIPES(bChanPhy,bChip);
			HAL_CQChipSelect(bChanPhy,bChipPhy);
			HAL_CQWaitDone(bChanPhy);

			for (i = 0; i < 4; i++)
			{
				bData[i] = HAL_McuSendReadPluse(bChanPhy);
			}

			pbDevLunStat[bStatIdx] = bData[2];
			bStatIdx++;
		}
	}
#endif
}

void FDL_DmaRdClrTimeout(U08 bChan,U32 dBuffAddr,U08 bSecCnt)
{
    U08 bPhyChan;
    U32 dRegValTmp = SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll;

    SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
    bPhyChan = CHANS(bChan);
    FL_CMD_CQ_SLC_MODE(bPhyChan);
    HAL_CQSendFlashCmd(bPhyChan,FL_CMD_READ1);
    HAL_CQSend5Addr(bPhyChan,0,0);
    HAL_CQStartDMA2(bPhyChan, dBuffAddr, DMA2_RD_TO_BUFF,0,0, bSecCnt, JOINT_NONE, tFtlTask.dEccStsBuff);
    SG_FC(bPhyChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
    HAL_CQWaitDone(bPhyChan);

    SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = dRegValTmp;
}

#ifdef __VMC_EN__
void  __section(".fdl_init") FDL_RemoveDevAllLunDwlsvBlkList(void)
#else
void  FDL_RemoveDevAllLunDwlsvBlkList(void)
#endif
{
	U08 i;
	U08 bChip,bChipPhy;
	U08 bChan,bChanPhy;

	U08 bData[4] = {2,0,0,0};

	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
		{
				bChipPhy = CHIPES(bChanPhy,bChip);

				HAL_CQChipSelect(bChanPhy,bChipPhy);
				HAL_CQCheckIOStatus(bChanPhy,0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
				HAL_CQSendFlashCmd(bChanPhy,0xD5);
				HAL_CQSendColAddr(bChanPhy, 0xDC00);
		}
	}

	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);
		HAL_CQWaitDone(bChanPhy);
	}

	bData[0]  = 2;
	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
		{
			bChipPhy = CHIPES(bChanPhy,bChip);
			HAL_CQChipSelect(bChanPhy,bChipPhy);
			HAL_CQWaitDone(bChanPhy);

#if 0
			for(i = 0;i<4;i++)
			{
				HAL_MCUSendDataByDqs(bChanPhy,bData[i]);
			}
#else
			HAL_MCUSendNByteDataByDqs(bChanPhy,bData,4);
#endif
		}
	}


	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
		{
				bChipPhy = CHIPES(bChanPhy,bChip);
				HAL_CQChipSelect(bChanPhy,bChipPhy);
				HAL_CQCheckIOStatus(bChanPhy,0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);

				HAL_CQSendFlashCmd(bChanPhy,0xDA);
				HAL_CQSendFlashCmd(bChanPhy,0x80);
				HAL_CQSend5Addr(bChanPhy,0,0x100*ROW_GAP_OF_PLANE);
				HAL_CQSendFlashCmd(bChanPhy,0x10);
		}
	}

	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);
		HAL_CQWaitDone(bChanPhy);
	}
	
	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
		{
				bChipPhy = CHIPES(bChanPhy,bChip);
				HAL_CQChipSelect(bChanPhy,bChipPhy);
				HAL_CQCheckIOStatus(bChanPhy,0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
				HAL_CQSendFlashCmd(bChanPhy,0xD5);
				HAL_CQSendColAddr(bChanPhy, 0xDC00);
		}
	}

	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);
		HAL_CQWaitDone(bChanPhy);
	}

	bData[0]  = 0;
	for(bChan = 0;bChan < CHAN_MAX_CNT;bChan++)
	{
		bChanPhy = CHANS(bChan);

		for(bChip = 0; bChip < CHIP_OF_CHAN; bChip++)
		{
			bChipPhy = CHIPES(bChanPhy,bChip);
			HAL_CQChipSelect(bChanPhy,bChipPhy);
			HAL_CQWaitDone(bChanPhy);

#if 0
			for(i = 0;i<4;i++)
			{
				HAL_MCUSendDataByDqs(bChanPhy,bData[i]);
			}
#else
			HAL_MCUSendNByteDataByDqs(bChanPhy,bData,4);
#endif
		}
	}

}

U08 FDL_ONFIParameterGet(U08 bChan,U08 bChip)
{
	U08 bData[8] = {0,0,0,0,0,0,0,0};
    U08 bJ;

	#if 1
	HAL_MCU_SetCe(bChan,bChip,1);
	Delay(0x10);
	HAL_MCU_SendCmd(bChan,0xEC);
	Delay(0x10);
	HAL_MCU_SendOneAddr(bChan,0);
	Delay(0x10);
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,0);
	#else
    HAL_CQSendFlashCmd(bChan,0xEC);
    HAL_CQSendAddr(bChan,0);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x10;
    HAL_CQCheckIOStatus(bChan,0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
    
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | 0x0;
	HAL_CQWaitDone(bChan);
	#endif

	for(bJ = 0; bJ < 4;bJ++)
	{
		bData[bJ] = HAL_McuSendReadPluse(bChan);
	}   
	
	if(	(bData[0] == 0x4F) && 
		(bData[1] == 0x4E) &&
 
		(bData[2] == 0x46) && 
		(bData[3] == 0x49))
    
	{
		return TRUE;/*镁光Flash,可能存在浅擦除问题*/
	}
	else
	{
		return FALSE;/*非镁光Flash,不可能存在浅擦除问题*/
	}
 
}

U08 FDL_ShallowEraseEn(U08 bChan,U08 bChip)
{
    U08 bModifyEn = TRUE;

#if ((NAND_FLASH == MIC_B16A))

    U08 bData[8] = {0,0,0,0,0,0,0,0};
  
    U08 bK,bJ,bDataNum,bGetMode;
 
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_MCU_SendCmd(bChan,0xEC);
	HAL_MCU_SendOneAddr(bChan, 0);
	Delay(0x20);
    
	HAL_MCUCheckStatus(bChan, bChip);
    
	HAL_MCU_SendCmd(bChan,0);

    bDataNum = 15;//获取byte58、59、60、61
    HAL_McuSendReadPluse(bChan);
    HAL_McuSendReadPluse(bChan);
	
    for (bK = 0; bK < bDataNum; bK++)
	{
	    for(bJ = 0; bJ < 4;bJ++)
        {   
    		bData[bJ] = HAL_McuSendReadPluse(bChan);
        }   
	}

    if(((bData[0] == 0x33) && (bData[1] == 0x57)) ||
        ((bData[2] == 0x33) && (bData[3] == 0x57)))
    {
        bGetMode = 0;
    }
    else
    {
        bGetMode = 1;
    }
    //PRINT_HEX("bGetMode=",bGetMode);
    
    if(bGetMode == 0)
    {
        bData[0] = 0x2F;
        HAL_NFSetExtFeature(bChan, bChip, 0xDF, 0x80, 0, &bData[0],1);//设置浅擦除相关            
        bData[0] = 0;

        HAL_NFGetExtFeature(bChan, bChip, 0x73, 0x04, 0, &bData[0],1);            
        //PRINT_HEX("Get0=",bData[0]);

        if(bData[0] > 0x28)
        {
            bModifyEn = TRUE;
        }
        else
        {
        	 bModifyEn = FALSE;
        }
        
        
		bData[0] = 0x2D;
        
		HAL_NFSetExtFeature(bChan, bChip, 0xDF, 0x80, 0, &bData[0],1);//设置浅擦除相关               
	}
    
	else
    
	{
        
		HAL_MCU_SendCmd(bChan,0xED);
    	
		HAL_MCU_SendOneAddr(bChan, 0);
		Delay(0x20);	
		HAL_MCUCheckStatus(bChan, bChip);
        
        
		HAL_MCU_SendCmd(bChan,0);	

        
		bDataNum = 132;//获取byte1048~1055
       
		for (bK = 0; bK < bDataNum; bK++)
    	
		{
    		
			for(bJ = 0; bJ < 8;bJ++)
            
			{   
        		
				bData[bJ] = HAL_McuSendReadPluse(bChan);
            
			}   
    	
		}

        //PRINT4("1=%xb,2=%xb,3=%xb,4=%xb\r\n",bData[0],bData[1],bData[2],bData[3]);

        if(bData[0] > 0xD)
        {
            bModifyEn = TRUE;
        }
        else
        {
        	 bModifyEn = FALSE;
        }
    }
#endif
    return bModifyEn;
}


//U8 bChanInfor: Bit0~3 读ID起始通道号；   Bit 4~7  读ID通道个数
//U8 bCeInfor:   Bit0~3 读ID起始Lun或者CE；Bit 4~7  读ID Lun或者CE个数
//U8 bOpInfor：   Bit0~3 读ID之后FC模块设置的模式；Bit4~7 读ID传参是否为物理参数，非0为物理参数，0为逻辑参数
#ifdef __VMC_EN__
void  __section(".fdl_init") FDL_ReadFlashID(U32 IDBuffAdd,U8 bChan,U8 bChip)
#else
void  FDL_ReadFlashID(U32 IDBuffAdd,U8 bChan,U8 bChip)
#endif
{

	HAL_NFReadFlashID(IDBuffAdd, bChan, bChip);
	FDL_FlashIFModeInit(bChan, bChip);

//	U8 bChipNum;
//	U8 bChNum;
//	U8 bPhyChan;
//	U8 bPhyCe;
//	U32 dDmaBuff;
//
//	for(bChipNum = 0; bChipNum < bCeCnt; bChipNum++)
//	{
//		for(bChNum = 0; bChNum < bChanCnt; bChNum++)
//		{
//			if(bOpInfor&0x80)
//			{
//				bPhyChan = CHANS(bChNum);
//				bPhyCe = CHIPES(bPhyChan, bChipNum);
//			}
//			else
//			{
//				bPhyChan = bChNum;
//				bPhyCe = bChipNum;
//			}
//
//			dDmaBuff = (U32)(pbIDBuff+8*bPhyCe+32*bPhyChan);       //ID长度按8Byte计算
//			*(U32*)dDmaBuff=0xFFFFFFFF;//每次读ID之前清成FF
//			*(((U32*)dDmaBuff)+1)=0xFFFFFFFF;
//
//#ifndef __FW__
//			U8 bCEValid = AlgScan_CheckCEValid(bPhyChan, bPhyCe);
//#else
//			U8 bCEValid = 1;//AlgScan_CheckCEValid(bPhyChan, bPhyCe);
//#endif
//			if(1 == bCEValid)
//			{
//				HAL_NFReadFlashID(dDmaBuff, bPhyChan, bPhyCe);
//				FDL_FlashIFModeInit(bPhyChan|0xF0, bPhyCe); 				  /*读ID不需要配置浅擦除*/
//
//				if ((bOpInfor&0x3F)== NF_TOGGLE_FEATURE)//Toggle模式
//				{
//					SG_FC(bPhyChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
//				}
//				Fdl_ClearExceptionState(bPhyChan);
//			}
//			else
//			{
//				*(U32*)dDmaBuff=0xFFFFFFFF;//每次读ID之前清成FF
//				*(((U32*)dDmaBuff)+1)=0xFFFFFFFF;
//			}
//		}
//	}
}

		//PRINT_HEX("GC*", GO_CACHE_CE);
		//PRINT("bChNum=%d,bChipNum=%d\n", bChNum, GO_CACHE_CE);

void FDL_EraseBlocks(FDL_TASK_CTRL *ptTaskCtrl,T_FLASH_PHY_BLK_ADDR *ptBlockAddr, U16 bBlockCnt, U8 bFlashMode)
{
	FDL_CreateTask(ptTaskCtrl);

    T_ERASE_EXCEPTION *tEraseEx = (T_ERASE_EXCEPTION *)tFtlTask.tTaskCtrl->dEraseExBuffAddr;
    tEraseEx->wLen              = 0;
    tEraseEx->bDescript         = OP_SUCCESS;
    tFtlTask.wCur              	= 0;
    tFtlTask.wLen              	= bBlockCnt;
    tFtlTask.dSrcArgsAddr      	= (U32)ptBlockAddr;
    tFtlTask.dTaskType         	= TYPE_ERASE_BLOCKS;
	tFtlTask.bArgsArr[0]   		= bFlashMode;//SLC_MODE/TLC_MODE
	tFtlTask.bNeedCQCount      	= CQ_DEEPTH_ERASE_BLOCKS;
  
    ChanTaskLastAnalysisErase();
	
    tFtlTask.FTLTaskErgodic            = TaskErgodic;
    tFtlTask.FTLTaskAllocate           = TaskAllocateEraseBlock;
    tFtlTask.FTLTaskConvert            = TaskConvertEraseBlock;
    tFtlTask.FTLTaskExceptionHandler   = ExceptionEraseBlock;

    RunFtlTaskErgodic(TASK_INIT);
}

void FDL_WriteLittleDataToFlash(FDL_TASK_CTRL *ptTaskCtrl,
								U32 dBuffAddr, 
								T_FLASH_PHY_ADDR const *ptPageAddr, 
								U16 wDataSize, 
								U8 bNumOfCopies,
								U16 *pwSeedIndex)
{
	U08 bChanPhy = CHANS(ptPageAddr->m_bChan);//获取物理通道号
	U16 wDma2LenTemp = SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN;
	//U08 bPtEn = SG_DMA2_CH(bChanPhy)->rECC_CFG.bits.PT_EN;
	SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN = wDataSize;
	SG_DMA2_CH(bChanPhy)->rECC_CFG.bits.PT_EN = 0;

#ifdef __FILL_PAGE_EN__
	SG_FC(0)->rBAD_COLUMN_CTRL.bits.FILT_EN = 0;  //关闭通道0的跳列功能
	SG_FC(1)->rBAD_COLUMN_CTRL.bits.FILT_EN = 0;  //关闭通道1的跳列功能
	SG_FC(2)->rBAD_COLUMN_CTRL.bits.FILT_EN = 0;  //关闭通道2的跳列功能
	SG_FC(3)->rBAD_COLUMN_CTRL.bits.FILT_EN = 0;  //关闭通道3的跳列功能
#endif

    tFtlTask.dSrcArgsAddr  = (U32)ptPageAddr;
    tFtlTask.dHandleBuff   = dBuffAddr;
    tFtlTask.dTaskType     = TYPE_WRITE_PAGES;
    tFtlTask.wLen          = 1;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = bNumOfCopies;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_WRITE_PAGES;
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN;

    FDL_CreateTask(ptTaskCtrl);

    tFtlTask.FTLTaskErgodic            = TaskErgodicReadWrite;//TaskErgodic;
    tFtlTask.FTLTaskConvert            = TaskConvertWriteLittleDataToFlash;
    tFtlTask.FTLTaskExceptionHandler   = ExceptionWritePageToFlash;

    RunFtlTaskErgodic(TASK_INIT);
	SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN = wDma2LenTemp;
	//SG_DMA2_CH(bChanPhy)->rECC_CFG.bits.PT_EN = bPtEn;

#ifdef __FILL_PAGE_EN__
	FDL_WaitTaskDone();

	SG_FC(0)->rBAD_COLUMN_CTRL.bits.FILT_EN = 1;  //开启通道0的跳列功能
	SG_FC(1)->rBAD_COLUMN_CTRL.bits.FILT_EN = 1;  //开启通道1的跳列功能
	SG_FC(2)->rBAD_COLUMN_CTRL.bits.FILT_EN = 1;  //开启通道2的跳列功能
	SG_FC(3)->rBAD_COLUMN_CTRL.bits.FILT_EN = 1;  //开启通道3的跳列功能
#endif

}


void FDL_ReadLittleDataToBuffer(FDL_TASK_CTRL *ptTaskCtrl,
								U32 dBuffAddr, 
								T_FLASH_PHY_ADDR const *ptPageAddr, 
								U16 wDataSize, 
								U8 bIdxOfCopies,
								U16 *pwSeedIndex)
{
	U08 bChanPhy = CHANS(ptPageAddr->m_bChan);//获取物理通道号
	U16 wDma2LenTemp = SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN;
	//U08 bPtEn = SG_DMA2_CH(bChanPhy)->rECC_CFG.bits.PT_EN;
	SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN = wDataSize;
	SG_DMA2_CH(bChanPhy)->rECC_CFG.bits.PT_EN = 0;
	
#if (FUNC_ENABLE == FDL_EXCEPTION_DETECTIVE_ON)
	if ((dBuffAddr % 8) || (wDataSize % 8))
	{
		FDL_Log("Address does not have 8 byte aligned <%s>[%x][%x]",__func__,dBuffAddr,wDataSize);
	}
#endif


	tFtlTask.dSrcArgsAddr  = (U32)ptPageAddr;
	tFtlTask.dHandleBuff   = dBuffAddr;
	tFtlTask.dTaskType     = TYPE_READ_LITTLE_DATA;
	tFtlTask.wLen          = 1;
	tFtlTask.wCur          = 0;
	tFtlTask.bArgsArr[0]   = bIdxOfCopies;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_READ_LITTLE_DATA;
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;

    FDL_CreateTask(ptTaskCtrl);

    if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
    {
        SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
    }

	tFtlTask.FTLTaskErgodic            = TaskErgodicReadWrite;//TaskErgodic;
	tFtlTask.FTLTaskConvert            = TaskConvertReadLittleDataToFlash;
	tFtlTask.FTLTaskExceptionHandler   = ExceptionReadClstToBuff;

	RunFtlTaskErgodic(TASK_INIT);
	SG_DMA2_CH(bChanPhy)->rLEN_CFG.bits.BASIC_DATA_LEN = wDma2LenTemp;
	//SG_DMA2_CH(bChanPhy)->rECC_CFG.bits.PT_EN = bPtEn;
}


void FDL_WriteBuffToFlash(FDL_TASK_CTRL *ptTaskCtrl,
						U32 dBuffAddr, 
						T_FLASH_PHY_ADDR const *ptPageAddr, 
						U16 bPageCnt,
						U32 dUdBuffAddr,
						U16 *pwSeedIndex)
{	
    tFtlTask.dSrcArgsAddr  = (U32)ptPageAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = dBuffAddr;
    tFtlTask.dTaskType     = TYPE_WRITE_PAGES;
    tFtlTask.pwSeedIndex   = pwSeedIndex;
    tFtlTask.wLen          = bPageCnt;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_WRITE_PAGES;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxWrite;	
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN;

#ifdef __FILL_PAGE_EN__
		if(ptPageAddr->m_bDataType == TYPE_LOGIC)
		{
			HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
		}
		else
		{
			HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_TABLE_ST]),1);//切到表格的补页跳列表位置
		}
#endif

    FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,ptPageAddr[0].m_bDataType);


    tFtlTask.FTLTaskErgodic  = TaskErgodicReadWrite;//TaskErgodic;
    tFtlTask.FTLTaskConvert  = TaskConvertWritePageToFlash;
    tFtlTask.FTLTaskExceptionHandler = ExceptionWritePageToFlash;

    RunFtlTaskErgodic(TASK_INIT);
}

						
//#ifdef __VER_TLC_PAGE__
void FDL_VerifyTLCPage( FDL_TASK_CTRL *ptTaskCtrl,
						T_BLK_INFO_PER_LUN *ptBlockInfo,
						U16 wThres,
						U08 bLunCnt, 
						U32 dBuffAddr,					
						U32 dUdBuffAddr,
						U16 *pwSeedIndex)
{
#ifdef __VER_TLC_PAGE__
	tFtlTask.dEccStsBuff = pFdlParam.dEccInfoBuffAddr;
	tFtlTask.dTaskType	   = TYPE_VERIFY_TLC_PAGE;//TYPE_VERIFY_TLC_PAGE;
	tFtlTask.dSrcArgsAddr  = (U32)ptBlockInfo;	/*Block Info, 内存复用*/
	tFtlTask.dUdBuff	   = dUdBuffAddr;
	tFtlTask.dHandleBuff   = OUT_OF_MEMORY_ADDR;
	tFtlTask.pwSeedIndex   = pwSeedIndex;	
	tFtlTask.wCurUnit	   = wThres;/*记录Fail Stop的阈值,空间复用*/
	tFtlTask.wLen		   = bLunCnt;
	tFtlTask.wCur		   = 0;
	tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_READ_TO_BUFFER;
	tFtlTask.bJointMode    = JOINT_NONE;
	tFtlTask.bDataType	   = TYPE_LOGIC;	
	tFtlTask.wPageOff	   = DMA_OF_DATA_CLST*DATA_CLST_OF_PAGE;/*一个Page的DMA个数,空间复用*/
	tFtlTask.bCurChan	   = 0;/*记录当前Task队列里面涉及的CH个数,空间复用*/
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;	
	tFtlTask.AddrBuffSize  = pFdlParam.wContiAddrMaxRead*sizeof(T_FLASH_PHY_ADDR);
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;

	FDL_CreateTask(ptTaskCtrl);
#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),0);/*RD 关闭跳列填充*/
#endif
	tFtlTask.FTLTaskErgodic  = TaskErgodicVerifyTLCPage;
	tFtlTask.FTLTaskConvert  = TaskConvertVerifyTLCPage;
	tFtlTask.FTLTaskExceptionHandler = ExceptionReadClstToBuff;
	RunFtlTaskErgodic(TASK_INIT);
#endif
}
//#endif


void FDL_FillSLCBlocks( FDL_TASK_CTRL *ptTaskCtrl,
						T_BLK_INFO_PER_LUN *ptBlockInfo,
						U08 bLunCnt, 
						U08 bDataType,
						U32 dBuffAddr,					
						U16 *pwSeedIndex)
{
#ifdef __FILL_SLC_BLOCKS__
	tFtlTask.dEccStsBuff = pFdlParam.dEccInfoBuffAddr;
	tFtlTask.dTaskType	   = TYPE_FILL_BLOCKS;
	tFtlTask.dSrcArgsAddr  = (U32)ptBlockInfo;
	tFtlTask.dHandleBuff   = dBuffAddr;
	tFtlTask.pwSeedIndex   = pwSeedIndex;
	tFtlTask.wLen		   = bLunCnt;
	tFtlTask.wCur		   = 0;
	tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_WRITE_PAGES;
	tFtlTask.bJointMode    = JOINT_NONE;
	tFtlTask.bDataType	   = bDataType;	
	tFtlTask.wPageOff	   = DMA_OF_DATA_CLST*DATA_CLST_OF_PAGE;/*一个Page的DMA个数,空间复用*/
	tFtlTask.bCurChan	   = 0;/*记录当前Task队列里面涉及的CH个数,空间复用*/
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;	
	tFtlTask.AddrBuffSize  = pFdlParam.wContiAddrMaxRead*sizeof(T_FLASH_PHY_ADDR);
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;
	if(bDataType == TYPE_LOGIC)
	{
		tFtlTask.dUdBuff	   = pFdlParam.bUnValidDataUser;
	}
	else
	{		
		tFtlTask.dUdBuff	   = pFdlParam.bUnValidTableUser;
	}


	FDL_CreateTask(ptTaskCtrl);
	
	tFtlTask.FTLTaskErgodic  = TaskErgodicVerifyTLCPage;
	tFtlTask.FTLTaskConvert  = TaskConvertFillBlock;
	tFtlTask.FTLTaskExceptionHandler = ExceptionWritePageToFlash;
	RunFtlTaskErgodic(TASK_INIT);
#endif
}



void ReadClstToBuff(FDL_TASK_CTRL *ptTaskCtrl,
					U32 dBuffAddr, 
					U16 *pwBuffIdx, 
					T_FLASH_PHY_ADDR const *ptClstAddr,
					U16 wClstLen, 
					U32 dUdBuffAddr, 
					U08 *pMskBuff, 
					U08 bDiscreteBuff, 
					U16 *pwSeedIndex,
					U08 bJointMode)
{

    tFtlTask.dTaskType     = TYPE_READ_TO_BUFFER;
    tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = dBuffAddr;
	tFtlTask.pwBuffIdx	   = pwBuffIdx;
    tFtlTask.pMskBuff      = pMskBuff;
    tFtlTask.pwSeedIndex   = pwSeedIndex;
    tFtlTask.wLen          = wClstLen;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = bDiscreteBuff;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_READ_TO_BUFFER;
	tFtlTask.bJointMode	   = bJointMode;
	tFtlTask.bDataType 	   = ptClstAddr->m_bDataType;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;	
	tFtlTask.AddrBuffSize  = pFdlParam.wContiAddrMaxRead*sizeof(T_FLASH_PHY_ADDR);
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;
	
	if(tFtlTask.bDataType == 0)
	{
		tFtlTask.wMaxClstOffOfEccSts = pFdlParam.dEccInfoBuffSize/(2*DMA_OF_DATA_CLST);//DataClst
	}
	else/*非联动Page启动要支持表格簇*/
	{
		tFtlTask.wMaxClstOffOfEccSts = pFdlParam.dEccInfoBuffSize/(2*DMA_OF_TABLE_CLST);//TableClst
	}

#ifdef __TASK_REORDER_TO_READ_TABLE__	/*读表格Row地址重排序功能*/
	if( (tFtlTask.pwBuffIdx != NULL) &&	
		(*(U16 *)tFtlTask.pwBuffIdx == 0xFFFF) &&
		(tFtlTask.bArgsArr[0] == DISCRETE_CLST))/*Row地址重排序*/
	{
		Fdl_TaskReOrderReadTable();
	}
#endif

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,ptClstAddr[0].m_bDataType);
	Fdl_PreReadMultiCe(ptClstAddr);

#ifdef __FILL_PAGE_EN__
	if(tFtlTask.bDataType == TYPE_LOGIC)
	{
		HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),0);//RD 关闭跳列填充,切到数据的补页跳列表位置
	}
	else
	{
		HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_TABLE_ST]),0);//RD 关闭跳列填充,切到表格的补页跳列表位置
	}
#endif


    if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
    {
        SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
    }

#ifdef __FDL_HW_VIRTUL_CLST__
	tFtlTask.FTLTaskErgodic  = TaskErgodicRead;//TaskErgodic;
#else
    tFtlTask.FTLTaskErgodic  = TaskErgodicReadWrite;//TaskErgodic;
#endif
    tFtlTask.FTLTaskConvert  = TaskConvertReadClstToBuff;
    tFtlTask.FTLTaskExceptionHandler = ExceptionReadClstToBuff;
    RunFtlTaskErgodic(TASK_INIT);
    Fdl_VirtualChanSet(0);
}

void FDL_ReadClstToBuff(	FDL_TASK_CTRL *ptTaskCtrl,
						U32 dBuffAddr, 
						T_FLASH_PHY_ADDR const *ptClstAddr, 
						U16 wClstLen, 
						U32 dUdBuffAddr,
						U16 *pwSeedIndex)
{
	if(dBuffAddr == (U32)NULL)
	{
	    dBuffAddr = OUT_OF_MEMORY_ADDR;
	}
	
	ReadClstToBuff(ptTaskCtrl,dBuffAddr,NULL,ptClstAddr,wClstLen,dUdBuffAddr,NULL,DISCRETE_NONE,pwSeedIndex,JOINT_NONE);
}

void FDL_ReadClstToBuff4K(	FDL_TASK_CTRL *ptTaskCtrl,
						U32 dBuffAddr, 
						T_FLASH_PHY_ADDR const *ptClstAddr, 
						U16 wClstLen, 
						U32 dUdBuffAddr,
						U16 *pwSeedIndex)
{

	tFtlTask.dEccStsBuff = pFdlParam.dEccInfoBuffAddr;

	tFtlTask.dTaskType	   = TYPE_READ_TO_BUFFER_4K;
	tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
	tFtlTask.dUdBuff	   = dUdBuffAddr;
	tFtlTask.dHandleBuff   = dBuffAddr;
	tFtlTask.pwBuffIdx	   = NULL;
	tFtlTask.pMskBuff	   = NULL;
	tFtlTask.pwSeedIndex   = pwSeedIndex;
	tFtlTask.wLen		   = wClstLen;
	tFtlTask.wCur		   = 0;
	tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_READ_TO_BUFFER;
	tFtlTask.bJointMode = JOINT_NONE;
	tFtlTask.bDataType	= ptClstAddr->m_bDataType;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;	
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,ptClstAddr[0].m_bDataType);

	if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
	{
		SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;           //No stop Dma2;Change buf status;hold en disable
	}

	tFtlTask.FTLTaskErgodic  = TaskErgodicReadWrite;
	tFtlTask.FTLTaskConvert  = TaskConvert4KReadClstToBuff;
	tFtlTask.FTLTaskExceptionHandler = ExceptionReadClstToBuff;
	RunFtlTaskErgodic(TASK_INIT);
	Fdl_VirtualChanSet(0);
}

void FDL_ReadClstToBuffWithMsk(	FDL_TASK_CTRL *ptTaskCtrl,
								U32 dBuffAddr, 
								T_FLASH_PHY_ADDR const *ptClstAddr, 
								U16 wClstLen, 
								U32 dUdBuffAddr,
								U8 *pMskBuff,
								U16 *pwSeedIndex)
{
	ReadClstToBuff(ptTaskCtrl,dBuffAddr,NULL,ptClstAddr,wClstLen,dUdBuffAddr,pMskBuff,DISCRETE_NONE,pwSeedIndex,JOINT_NONE);
}

void FDL_ReadClstToDiscreteBuff(FDL_TASK_CTRL *ptTaskCtrl,
								U32 dBaseBuffAddr,
                                U16 *pwBuffIdx,
                    			T_FLASH_PHY_ADDR const *ptClstAddr,
                    			U16 wClstLen,
                    			U32 dUdBuffAddr,
                    			U08 *pMaskBuff,
                    			U16 *pwSeedIndex,
                    			U08 bJointMode)
{
	
	ReadClstToBuff(ptTaskCtrl,dBaseBuffAddr, pwBuffIdx, ptClstAddr, wClstLen, dUdBuffAddr,
				pMaskBuff, DISCRETE_CLST, pwSeedIndex, bJointMode);
}


void FDL_ReadClstWithRowReorder(FDL_TASK_CTRL *ptTaskCtrl,
								U32 dBaseBuffAddr,
								T_FLASH_PHY_ADDR const *ptClstAddr,
								U16 wClstLen,
								U32 dUdBuffAddr,								
								U16 *pwSeedIndex,
								U16 *pwBuffIdx)
{			
#ifdef __TASK_REORDER_TO_READ_TABLE__
	pwBuffIdx[0] = 0xFFFF;/*标志需要重排序*/
#else
	pwBuffIdx = NULL;
#endif
	ReadClstToBuff(ptTaskCtrl,dBaseBuffAddr, pwBuffIdx, ptClstAddr, wClstLen, dUdBuffAddr,
				NULL, DISCRETE_CLST, pwSeedIndex, JOINT_NONE);
}
							
U08 FDL_PreReadTableToBuff4K( FDL_TASK_CTRL *ptTaskCtrl,
							  U32 dBuffAddr, 
							  U16 *pwBuffIdx, 
							  T_FLASH_PHY_ADDR const *ptClstAddr,								
							  U08 bClstLen, 
							  U32 dUdBuffAddr, 
							  U16 *pwSeedIndex,
							  U08 bLastChanNum,
							  U08 bCheckHold)
{
	Fdl_PreRead4K((T_FLASH_PHY_ADDR *)ptClstAddr,bClstLen);
	U08MemSet((U8 *)(ptTaskCtrl->dReadExBuffAddr),0,sizeof(T_READ_EXCEPTION));	

	
	U08 i = 0;
	U08 bPhyChan = 0;
	U08 bLastClstLen = tFtlTask.wLen;
	tFtlTask.bArgsArr[50] = 0;
	HandlePreReadExCeption((T_FLASH_PHY_ADDR *)ptClstAddr, bLastChanNum, bClstLen);
	HAL_LDPC_ParityChangeCfg(1,1);
	
	gFdlTskStatus 		   = ST_DOING;
	tFtlTask.bArgsArr[50]  = bLastClstLen;
	tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
	tFtlTask.dEccStsBuff   = pFdlParam.dEccInfoBuffAddr;
	tFtlTask.pwBuffIdx	   = pwBuffIdx;
	tFtlTask.dTaskType	   = TYPE_PRE_READ_4K;
	tFtlTask.dUdBuff	   = dUdBuffAddr;
	tFtlTask.dHandleBuff   = dBuffAddr;
	tFtlTask.pwSeedIndex   = pwSeedIndex;
	tFtlTask.wLen		   = bClstLen;
	tFtlTask.wCur		   = tFtlTask.wLen;
	tFtlTask.bArgsArr[0]   = DISCRETE_CLST;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_READ_TO_BUFFER;
	tFtlTask.bDataType	   = ptClstAddr->m_bDataType;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;	
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;
	tFtlTask.pMskBuff = NULL;
	tFtlTask.tTaskCtrl = ptTaskCtrl;	
	tFtlTask.FTLTaskExceptionHandler   = ExceptionPreRead4K;
		
	if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
	{
		SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
	}	
	
	U08 bCurChanNum;
	bCurChanNum = ChanAnalysis4K();

	for(i = 0; i <	MAX_CHAN_COUNT; i++)
	{
		if((1 << i) & bCurChanNum)
		{
			bPhyChan = CHANS(i);
			HAL_CQSetUd(bPhyChan,tFtlTask.dUdBuff,SECT_OF_TBLE_CLST,UD_LEN_OF_TBLE_CLST,1);
		}	
	}
	
	for(i = 0; i <  bClstLen; i++)
	{
		Convert4KPreReadTable(i);
	}	
	
	if(bCheckHold)
	{
		HandlePreReadExCeption(NULL,bCurChanNum,0);
	}	
	Fdl_PreVirtualChanSet();
	return bCurChanNum;
}

void FDL_WriteDiscreteBuffToFlash(	FDL_TASK_CTRL *ptTaskCtrl,
									U32 dBaseBuffAddr,
									U16 *pwBuffIdx,
									U08 bBuffClstCnt,
									T_FLASH_PHY_ADDR const *ptPageAddr,
									U08 bPageCnt,
									U32 dUdBuffAddr,
									U16 *pwSeedIndex,
									U08 bJointMode)

{	
    tFtlTask.dSrcArgsAddr  = (U32)ptPageAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = dBaseBuffAddr;
	tFtlTask.pwBuffIdx 	= pwBuffIdx;
    tFtlTask.dTaskType     = TYPE_WRITE_PAGES;
    tFtlTask.wLen          = bPageCnt;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = DISCRETE_CLST;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_WRITE_PAGES;
	tFtlTask.pwSeedIndex 	= pwSeedIndex;
	tFtlTask.bJointMode	= bJointMode;
	tFtlTask.bBuffClstCnt 	= bBuffClstCnt;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxWrite;
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN;

#ifdef __FILL_PAGE_EN__
		if(ptPageAddr->m_bDataType == TYPE_LOGIC)
		{
			HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
		}
		else
		{
			HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_TABLE_ST]),1);//切到表格的补页跳列表位置
		}
#endif

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,ptPageAddr[0].m_bDataType);

    tFtlTask.FTLTaskErgodic  = TaskErgodicReadWrite;//TaskErgodic;
    tFtlTask.FTLTaskConvert  = TaskConvertWritePageToFlash;
    tFtlTask.FTLTaskExceptionHandler = ExceptionWritePageToFlash;

    RunFtlTaskErgodic(TASK_INIT);
}

void FDL_JointReadClsts(	FDL_TASK_CTRL *ptTaskCtrl,
						U32 dBuffAddr,
						T_FLASH_PHY_ADDR const *ptClstAddr,
						U08 bSecOfstInFstClst,
						U32 dTotalSec,
						U16 wClstLen, 
						U32 dUdBuffAddr,
						U16 *pwSeedIndex)
{
	U16 i = 0;
	U08 bSecOfstInLastClst;
	
    tFtlTask.dTaskType     = TYPE_NCQ_READ;
    tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = dBuffAddr;
    tFtlTask.pwSeedIndex   = pwSeedIndex;
    tFtlTask.wLen          = wClstLen;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_NCQ_READ;
	tFtlTask.bDataType     = ptClstAddr->m_bDataType;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;
	tFtlTask.AddrBuffSize  = pFdlParam.wContiAddrMaxRead*sizeof(T_FLASH_PHY_ADDR);
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;
	
	tFtlTask.wMaxClstOffOfEccSts = pFdlParam.dEccInfoBuffSize/(2*DMA_OF_DATA_CLST);//联动读保证只用DataClst


    FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);

	Fdl_PreReadMultiCe((T_FLASH_PHY_ADDR *)ptClstAddr);
	HAL_ADMWaitDMADone(0xFF);
	HAL_MCWaitDMA1Done();

	HAL_BFCInit();

#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),0);/*RD 关闭跳列填充*/
#endif


    if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
    {
        SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
    }

    HAL_MCStartDMA1(tFtlTask.dHandleBuff,bSecOfstInFstClst&0x0F, dTotalSec, DMA1_RD_TO_HOST, JOINT_12);

	// NCQ读Mask信息清除,fdl自己计算
#ifdef __LAST_CLUST_MASK_OFF__
	if(bSecOfstInFstClst&0xF0)
	{
		bSecOfstInLastClst = 0;
		bSecOfstInFstClst  &= 0x0F;
	}
	else
#endif
	{
		bSecOfstInLastClst = (bSecOfstInFstClst + dTotalSec)%SECT_OF_DATA_CLST; 
	}

	
	if((bSecOfstInFstClst > 0)||(bSecOfstInLastClst > 0))//需要设置mask信息
	{
		tFtlTask.pMskBuff = tFtlTask.bMaskInfo;
        MemorySet((U8 * )&tFtlTask.bMaskInfo[0], 0, sizeof(tFtlTask.bMaskInfo));

		// 第一个簇mask信息设置（前mask）
		if (bSecOfstInFstClst > 0)
		{
			for(i = 0; i < bSecOfstInFstClst; i++)
			{
				tFtlTask.bMaskInfo[i/8] |= (U32)0x01<<(i&(8-1));
			}
		}
		
		// 最后一个簇mask信息设置（尾mask）
		if (bSecOfstInLastClst > 0)
		{
			for(i = bSecOfstInLastClst; i < 8*MASK_BYTE_OF_CLST; i++)
			{
				if((bSecOfstInFstClst+dTotalSec) < SECT_OF_DATA_CLST)//小于一个簇数据
				{
					tFtlTask.bMaskInfo[i/8] |= (U32)0x01<<(i&(8-1));
				}
				else
				{
					tFtlTask.bMaskInfo[NCQ_READ_MASK_END*MASK_BYTE_OF_CLST+i/8] |= (U32)0x01<<(i&(8-1));
				}
			}
			
		}
	}
	else//不用设置mask信息
	{
		tFtlTask.pMskBuff = NULL;
	}
	
#ifdef __FDL_HW_VIRTUL_CLST__
	tFtlTask.FTLTaskErgodic            = TaskErgodicRead;//TaskErgodic;
#else
    tFtlTask.FTLTaskErgodic            = TaskErgodicReadWrite;//TaskErgodic;
#endif
    tFtlTask.FTLTaskConvert            = TaskConvertReadClstToBuff;
    tFtlTask.FTLTaskExceptionHandler   = ExceptionReadClstToBuff;
    RunFtlTaskErgodic(TASK_INIT);
	
    Fdl_VirtualChanSet(0);
}
void FDL_JointReadClstsRand4K(	FDL_TASK_CTRL *ptTaskCtrl,
						U32 dBuffAddr,
						T_FLASH_PHY_ADDR const *ptClstAddr,
						U08 bSecOfstInFstClst,
						U32 dTotalSec,
						U16 wClstLen,
						U32 dUdBuffAddr,
						U16 *pwSeedIndex)
{
	U08 i;
	U08 bSecOfstInLastClst;

    tFtlTask.dTaskType     = TYPE_NCQ_READ_4K;
    tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = dBuffAddr;
    tFtlTask.pwSeedIndex   = pwSeedIndex;
    tFtlTask.wLen          = wClstLen;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_NCQ_READ;
	tFtlTask.bDataType     = ptClstAddr->m_bDataType;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);
	
#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),0);/*RD 关闭跳列填充*/
#endif


    if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
    {
        SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
    }

	// NCQ读Mask信息清除,fdl自己计算
	bSecOfstInLastClst = (bSecOfstInFstClst+dTotalSec)%SECT_OF_DATA_CLST;
	if((bSecOfstInFstClst > 0)||(bSecOfstInLastClst > 0))//需要设置mask信息
	{
		tFtlTask.pMskBuff = tFtlTask.bMaskInfo;
        MemorySet((U8 * )&tFtlTask.bMaskInfo[0], 0, sizeof(tFtlTask.bMaskInfo));

		// 第一个簇mask信息设置（前mask）
		if (bSecOfstInFstClst > 0)
		{
			for(i = 0; i < bSecOfstInFstClst; i++)
			{
				tFtlTask.bMaskInfo[i/8] |= (U32)0x01<<(i&(8-1));
			}
		}

		// 最后一个簇mask信息设置（尾mask）
		if (bSecOfstInLastClst > 0)
		{
			for(i = bSecOfstInLastClst; i < 8*MASK_BYTE_OF_CLST; i++)
			{
				if((bSecOfstInFstClst+dTotalSec) < SECT_OF_DATA_CLST)//小于一个簇数据
				{
					tFtlTask.bMaskInfo[i/8] |= (U32)0x01<<(i&(8-1));
				}
				else
				{
					tFtlTask.bMaskInfo[NCQ_READ_MASK_END*MASK_BYTE_OF_CLST+i/8] |= (U32)0x01<<(i&(8-1));
				}
			}

		}
	}
	else//不用设置mask信息
	{
		tFtlTask.pMskBuff = NULL;
	}

	//ChanTaskLastAnalysis();
	
	tChTaskQueue[0].bClstCntInPage = 1;
	tChTaskQueue[1].bClstCntInPage = 1;
	tChTaskQueue[2].bClstCntInPage = 1;
	tChTaskQueue[3].bClstCntInPage = 1;

#ifdef __FDL_HW_VIRTUL_CLST__
	tFtlTask.FTLTaskErgodic            = TaskErgodicRead;//TaskErgodic;
#else
    tFtlTask.FTLTaskErgodic            = TaskErgodicReadWrite;//TaskErgodic;
#endif
    tFtlTask.FTLTaskConvert            = TaskConvert4KReadClstToBuff;
    tFtlTask.FTLTaskExceptionHandler   = ExceptionReadClstToBuff;
    RunFtlTaskErgodic(TASK_INIT);
    Fdl_VirtualChanSet(0);
}
				
U08 FDL_JointPreReadClsts4K(FDL_TASK_CTRL *ptTaskCtrl,
							U32 dBuffAddr,
							U16 *pwBuffIdx,
							T_FLASH_PHY_ADDR const *ptClstAddr,											
							U08 bClstLen, 
							U32 dUdBuffAddr,
							U16 *pwSeedIndex,						
							U08 bLastChanNum,
							U08 bCheckHold)
{
	Fdl_PreRead4K((T_FLASH_PHY_ADDR *)ptClstAddr,bClstLen);
	U08MemSet((U8 *)(ptTaskCtrl->dReadExBuffAddr),0,sizeof(T_READ_EXCEPTION));

	U16 i = 0;
	U08 bPhyChan = 0;
	U08 bTotalSec = bClstLen * SECT_OF_DATA_CLST;
	U08 bLastClstLen = tFtlTask.wLen;
	tFtlTask.bArgsArr[50] = 0;

	HandlePreReadExCeption((T_FLASH_PHY_ADDR *)ptClstAddr,bLastChanNum,bClstLen);
	HAL_LDPC_ParityChangeCfg(1,0);
	
	gFdlTskStatus = ST_DOING;
	tFtlTask.bArgsArr[50]  = bLastClstLen;//记录上次操作了多少个len，以便于发生retry，区分上下
	tFtlTask.dSrcArgsAddr  = (U32)ptClstAddr;
	tFtlTask.pwBuffIdx	   = pwBuffIdx;
	tFtlTask.dEccStsBuff   = pFdlParam.dEccInfoBuffAddr;
	tFtlTask.dTaskType	   = TYPE_PRE_READ_4K;
	tFtlTask.dUdBuff	   = dUdBuffAddr;
	tFtlTask.dHandleBuff   = dBuffAddr;
	tFtlTask.pwSeedIndex   = pwSeedIndex;
	tFtlTask.wLen		   = bClstLen;
	tFtlTask.wCur		   = tFtlTask.wLen;
	tFtlTask.bArgsArr[0]   = DISCRETE_CLST;
	tFtlTask.bDataType	   = ptClstAddr->m_bDataType;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxRead;
	tFtlTask.AddrBuffSize  = pFdlParam.wContiAddrMaxRead*sizeof(T_FLASH_PHY_ADDR);
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN*DATA_CLST_OF_PAGE;	
	tFtlTask.wMaxClstOffOfEccSts = pFdlParam.dEccInfoBuffSize/(2*DMA_OF_DATA_CLST);//联动读保证只用DataClst	
	tFtlTask.pMskBuff = NULL;	
	tFtlTask.tTaskCtrl = ptTaskCtrl;
	tFtlTask.FTLTaskExceptionHandler   = ExceptionPreRead4K;

		
	if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_NO_RETRY)
	{
		SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0;//No stop Dma2;Change buf status;hold en disable
	}
	HAL_MCWaitDMA1Done();
	HAL_MCStartDMA1(tFtlTask.dHandleBuff,tFtlTask.pwBuffIdx[0], bTotalSec, DMA1_RD_TO_HOST, JOINT_12);
	
	U08 bCurChanNum;
	bCurChanNum = ChanAnalysis4K();
	
	for(i = 0; i <	CHAN_MAX_CNT; i++)
	{
		if((1 << i) & bCurChanNum)
		{
			bPhyChan = CHANS(i);
			HAL_CQSetUd(bPhyChan,tFtlTask.dUdBuff,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,1);
		}	
	}
	
	for(i = 0; i <  bClstLen; i++)
	{
		Convert4KPreRead(i);
	}
	
	//Fdl_PreVirtualChanSet();
	return bCurChanNum;
}

void FDL_Pre4K_WaitTaskDone(FDL_TASK_CTRL *ptTaskCtrl,U08 bLastChanNum)
{
	tFtlTask.bArgsArr[50] = 0;
	U08MemSet((U8 *)(ptTaskCtrl->dReadExBuffAddr),0,sizeof(T_READ_EXCEPTION));
    HandlePreReadExCeption(NULL,bLastChanNum,0);
    FDL_WaitTaskDone();
}

void FDL_JointWritePages(	FDL_TASK_CTRL *ptTaskCtrl,
						U32 dBuffAddr, 
						U8 bSecOffs, 
						U32 dTotalSec, 
						T_FLASH_PHY_ADDR *ptPageAddr, 
						U16 bPageCnt,
						U32 dUdBuffAddr, 
						U16 *pwSeedIndex)
{
    tFtlTask.dSrcArgsAddr  = (U32)ptPageAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = (U32)dBuffAddr;
    tFtlTask.pwSeedIndex   = pwSeedIndex;
    tFtlTask.dTaskType     = TYPE_NCQ_WRITE;
    tFtlTask.wLen          = bPageCnt;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_NCQ_WRITE;
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxWrite;	
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN;
	
	HAL_MCStartDMA1(dBuffAddr,bSecOffs, dTotalSec, DMA1_WR_TO_BUFF, JOINT_12);	

#ifdef __FILL_PAGE_EN__
		HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
#endif

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);

    if(tFtlTask.tTaskCtrl->bWriteFailCtrl== WRITE_FAIL_STOP)
    {
        HAL_NFChkStCtrl(MAX_CHAN_COUNT,FAIL_STOP_CQ);
    }
#if 1
	if (bSecOffs > 0)//前面sector的水位置起来
	{	
		if(bSecOffs <= 16)
        {
            HAL_BFCSetBuffStatus(0,(U16)(((U32)0x1<<bSecOffs)-1));
        }
        else
        {
            HAL_BFCSetBuffStatus(0,0xFFFF);
            HAL_BFCSetBuffStatus(16,(U16)(((U32)0x1<<(bSecOffs-16))-1));
        }
	}
#else/*支持Buffindex(bSecOffs)从Buffsize内任意位置开始*/
	U16 bBuffIndex = 0; 
	while(bSecOffs > 0)
	{
		if(bSecOffs <= 16)
		{
			HAL_BFCSetBuffStatus(bBuffIndex,(U16)(((U32)0x1<<bSecOffs)-1));
			bSecOffs = 0;
		}
		else
		{
			HAL_BFCSetBuffStatus(bBuffIndex,0xFFFF);
			bBuffIndex += 16;
			bSecOffs -= 16;
		}
	}
#endif

    tFtlTask.FTLTaskErgodic  = TaskErgodicReadWrite;//TaskErgodic;
    tFtlTask.FTLTaskConvert  = TaskConvertWritePageToFlash;
    tFtlTask.FTLTaskExceptionHandler = ExceptionWritePageToFlash;
    RunFtlTaskErgodic(TASK_INIT);
}

void FDL_JointWriteHostToDiscreteBuff(  U32 dBaseBuffAddr,
                                        U16 *pwBuffIdx,
                    					U16 *pwSec,
                    					U08 bCnt,
                    					U08 bJointMode)
{
	U08 i = 0;

	HAL_MCADMSetBaseAddr(dBaseBuffAddr);
	
	for(i = 0; i < bCnt; i++)
	{
		HAL_MCStartDMA1WithoutBuffAddr(pwBuffIdx[i],pwSec[i],DMA1_WR_TO_BUFF,bJointMode);
		HAL_MCWaitDMA1Done();
	}
}
		
void FDL_CopyBackProg(FDL_TASK_CTRL *ptTaskCtrl,T_FLASH_PHY_ADDR *ptSrcClst, U16 wClstLen,T_FLASH_PHY_ADDR const *ptDstPage, U16 wPageCnt, U32 dCBBuff,U32 dCBBuffSize,U32 dSrcUdBuffAddr,U32 dDstUdBuffAddr,U16 *pwSrcSeedIndex, U16 *pwDstSeedIndex)
{		

    tFtlTask.dTaskType     = TYPE_COPY_BACK;
    tFtlTask.dSrcArgsAddr  = (U32)ptSrcClst;
    tFtlTask.dDstArgsAddr  = (U32)ptDstPage;
    tFtlTask.dUdBuff       = (U32)dSrcUdBuffAddr;
	tFtlTask.dDestUdBuff   = (U32)dDstUdBuffAddr;
	tFtlTask.wCur = 0;
	tFtlTask.wLen = wPageCnt;
	
#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_TABLE_ST]),1);//切到表格的补页跳列表位置
#endif

    tFtlTask.dHandleBuff   = dCBBuff;
    tFtlTask.pMskBuff      = NULL;
	tFtlTask.bDataType = ptSrcClst->m_bDataType;
	
	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,1);

	tFtlTask.dSrcSeedIndex = NULL;
	tFtlTask.dDesSeedIndex = NULL;
	if((pwSrcSeedIndex != NULL)&&(pwDstSeedIndex != NULL))
	{
		tFtlTask.dSrcSeedIndex = pwSrcSeedIndex;
		tFtlTask.dDesSeedIndex = pwDstSeedIndex;
	}

	CB_GetTableCopyBackInfor(ptDstPage,wPageCnt,wClstLen,dCBBuffSize);
	
	tFtlTask.FTLTaskErgodic  = TaskErgodicTableDataCopyBack;
	tFtlTask.FTLTaskConvert  = TaskConvertTableDataCopyBackProg;
	tFtlTask.FTLTaskExceptionHandler = ExceptionCopyBack;
	
 	 RunFtlTaskErgodic(TASK_INIT);	
}

void FDL_MultiLunExdieCopyBackPrg (FDL_TASK_CTRL *ptTaskCtrl,
									T_SRC_INFO_OF_LUN *ptSrcInfo, 
									T_DST_INFO_OF_LUN *ptDstInfo,
									U32 dCBBuff,
									U32 dCBBuffSize,
									U08 bMultiPlnMode,
									U08 bCnt,
									U16 *pwSrcSeedIndex, 
									U16 *pwDstSeedIndex)
{	
	tFtlTask.dTaskType = TYPE_MULTI_LUN_EXDIE_COPYBACK_PRG;
	tFtlTask.dSrcArgsAddr = (U32)ptSrcInfo;
	tFtlTask.dDstArgsAddr = (U32)ptDstInfo;
	tFtlTask.dHandleBuff = dCBBuff;
	
	tFtlTask.pMskBuff = PNULL;
	tFtlTask.wCur = bCnt;
	tFtlTask.wLen =  bCnt;
	
	tFtlTask.bNeedCQCount = 0xE0;
	tFtlTask.bDataType = TYPE_LOGIC;

#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
#endif

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);

	tFtlTask.dSrcSeedIndex = NULL;
	tFtlTask.dDesSeedIndex = NULL;
	if((pwSrcSeedIndex != NULL)&&(pwDstSeedIndex != NULL))
	{
		tFtlTask.dSrcSeedIndex = pwSrcSeedIndex;
		tFtlTask.dDesSeedIndex = pwDstSeedIndex;
	}

	tFtlTask.FTLTaskErgodic  = TaskErgodicSingleLunCopyBack;
	tFtlTask.FTLTaskAllocate = TaskAllocateMultiLunXXdieSlc;
	tFtlTask.FTLTaskConvert  = TaskConvertMultiLunExdieSlcCopyBack;
	tFtlTask.FTLTaskExceptionHandler = ExceptionCopyBack;
	
	CB_GetExDieSlcCopyBackInfor(dCBBuffSize);
#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
	tCB_ProcessCtrl.bWtCmdMode = bMultiPlnMode;
#endif
	RunFtlTaskErgodic(TASK_INIT);
}
									
void FDL_MultiLunIndieCopyBackPrg (FDL_TASK_CTRL *ptTaskCtrl,
									T_SRC_INFO_OF_LUN *ptSrcInfo,
									T_DST_INFO_OF_LUN *ptDstInfo,
									U32 dCBBuff,
									U32 dCBBuffSize,
									U08 bMultiPlnMode,
									U08 bCnt,
									U16 *pwSrcSeedIndex,
									U16 *pwDstSeedIndex)
{	
	
	tFtlTask.dTaskType = TYPE_MULTI_LUN_INDIE_COPYBACK_PRG;
	tFtlTask.dSrcArgsAddr = (U32)ptSrcInfo;
	tFtlTask.dDstArgsAddr = (U32)ptDstInfo;
	tFtlTask.dHandleBuff = dCBBuff;
	
	tFtlTask.pMskBuff = PNULL;
	tFtlTask.wCur = bCnt;
	tFtlTask.wLen =  bCnt;
	
	tFtlTask.bDataType = TYPE_LOGIC;
	
#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
#endif

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);

	tFtlTask.dSrcSeedIndex = NULL;
	tFtlTask.dDesSeedIndex = NULL;
	if((pwSrcSeedIndex != NULL)&&(pwDstSeedIndex != NULL))
	{
		tFtlTask.dSrcSeedIndex = pwSrcSeedIndex;
		tFtlTask.dDesSeedIndex = pwDstSeedIndex;
	}

	tFtlTask.FTLTaskErgodic  = TaskErgodicMultiLunCopyBack;
	tFtlTask.FTLTaskAllocate = TaskAllocateMultiLunXXdieSlc;
	tFtlTask.FTLTaskConvert  = TaskConvertMultiLunIndieSlcCopyBack;
	tFtlTask.FTLTaskExceptionHandler = ExceptionCopyBack;

	CB_GetInDieSlcCopyBackInfor(dCBBuffSize);
#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
	tCB_ProcessCtrl.bWtCmdMode = bMultiPlnMode;
#endif
	RunFtlTaskErgodic(TASK_INIT);
}

void FDL_OrderPrgBuffToFlash(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_PLANE_PARAM * ptPlaneParam, T_FLASH_PROG_ORDER_UNIT *ptOrderPrgUnit, U32 dUdBuffAddr, U16 wCnt, U16 *pwSeedIndex)
{	
	tFtlTask.dDstArgsAddr  = (U32)ptPlaneParam;
    tFtlTask.dSrcArgsAddr  = (U32)ptOrderPrgUnit;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = dBuffAddr;
    tFtlTask.dTaskType     = TYPE_WRITE_PAGES;
    tFtlTask.wLen          = wCnt;
    tFtlTask.wCur          = 0;
	tFtlTask.bNeedCQCount  = CQ_DEEPTH_ORDER_PRG;
	tFtlTask.wPageOff	   = 0;
	
	FDL_CreateTask(ptTaskCtrl);

    tFtlTask.FTLTaskErgodic            = OrderTaskErgodic;
    tFtlTask.FTLTaskConvert            = TaskConvertOrderPrgToFlash;
    tFtlTask.FTLTaskExceptionHandler   = ExceptionWritePageToFlash;

    RunFtlTaskErgodic(TASK_INIT);
}

#ifdef _TLC_ORDER_PROGRAM_
void FDL_JointOrderPrgUnit(FDL_TASK_CTRL *ptTaskCtrl,U32 dDmaBuffAddr,U32 dDma1SecCnt, U32 dUnitBaseAddr, U32 dUdBuffAddr)
{
	U08 bSecOffs = 0;

	tFtlTask.dSrcArgsAddr  = dUnitBaseAddr;
    tFtlTask.dUdBuff       = dUdBuffAddr;
    tFtlTask.dHandleBuff   = (U32)dDmaBuffAddr;
    tFtlTask.pwSeedIndex   = NULL;
    tFtlTask.dTaskType     	= TYPE_NCQ_ORDER_WRITE;
    tFtlTask.wLen          = 0;
    tFtlTask.wCur          = 0;
    tFtlTask.bArgsArr[0]   = DISCRETE_NONE;
	tFtlTask.bNeedCQCount  	= CQ_DEEPTH_NCQ_WRITE; //0x50;   //UDP传参
	tFtlTask.bTlcNeedCQCount  = CQ_DEEPTH_NCQ_ORDER_PRG; //0xA0;   //no less than 0x90
	tFtlTask.wContiAddrMax = pFdlParam.wContiAddrMaxWrite;
	tFtlTask.wFirstErgodicCnt = CHIP_OF_CHAN*CHAN_MAX_CNT*PLANE_OF_LUN;
	tFtlTask.wPageOff	   = 0;
	HAL_MCStartDMA1(dDmaBuffAddr,bSecOffs, dDma1SecCnt, DMA1_WR_TO_BUFF, JOINT_12);
	FDL_Log("bSecOffs=%d,dDma1SecCnt=%d",bSecOffs,dDma1SecCnt);

	FDL_CreateTask(ptTaskCtrl);

	if(tFtlTask.tTaskCtrl->bWriteFailCtrl== WRITE_FAIL_STOP)
    {
        HAL_NFChkStCtrl(MAX_CHAN_COUNT,FAIL_STOP_CQ);
    }

    tFtlTask.FTLTaskErgodic  = TaskErgodicOrderPrgUnit;//TaskErgodic;
    //tFtlTask.FTLTaskConvert  = TaskConvertOrderPrgUnit;
    tFtlTask.FTLTaskExceptionHandler = ExceptionOrderPrgUnit;

}
#endif
void FDL_MultiLunExdieCopyBackOrderPrg( FDL_TASK_CTRL *ptTaskCtrl,
										T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo, 
										T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo,
										U32 dCBBuff,
										U32 dCBBuffSize,
										U08 bMultiPlnMode,
										U08 bCnt,
										U16 *pwSrcSeedIndex, 
										U16 *pwDstSeedIndex)
{	
	tFtlTask.dTaskType = TYPE_MULTI_LUN_EXDIE_COPYBACK_ORDER_PRG;
	tFtlTask.dSrcArgsAddr = (U32)ptSrcInfo;
	tFtlTask.dDstArgsAddr = (U32)ptDstInfo;
	tFtlTask.dHandleBuff = dCBBuff;

	tFtlTask.pMskBuff = PNULL;
	tFtlTask.wCur = bCnt;
	tFtlTask.wLen =  bCnt;
	tFtlTask.bNeedCQCount = 0xE0;

#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
#endif

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);

	tFtlTask.dSrcSeedIndex = NULL;
	tFtlTask.dDesSeedIndex = NULL;
	if((pwSrcSeedIndex != NULL)&&(pwDstSeedIndex != NULL))
	{
		tFtlTask.dSrcSeedIndex = pwSrcSeedIndex;
		tFtlTask.dDesSeedIndex = pwDstSeedIndex;
	}

#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
	CB_GetInDieTlcCopyBackInfor(dCBBuffSize);
	tCB_ProcessCtrl.bWtCmdMode = bMultiPlnMode;

	tFtlTask.FTLTaskErgodic  = TaskErgodicMultiLunCopyBack;
	tFtlTask.FTLTaskAllocate = TaskAllocateMultiLunXXdieTlc;
	tFtlTask.FTLTaskConvert  = TaskConvertMultiLunIndieTlcCopyBack;
#else
	CB_GetExDieTlcCopyBackInfor(dCBBuffSize);

	tFtlTask.FTLTaskErgodic  = TaskErgodicSingleLunCopyBack;
	tFtlTask.FTLTaskAllocate = TaskAllocateMultiLunXXdieTlc;
	tFtlTask.FTLTaskConvert  = TaskConvertMultiLunExdieTlcCopyBack;
#endif
	tFtlTask.FTLTaskExceptionHandler = ExceptionCopyBack;

	RunFtlTaskErgodic(TASK_INIT);
}

void FDL_MultiLunIndieCopyBackOrderPrg (FDL_TASK_CTRL *ptTaskCtrl,
										T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo, 
										T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo,
										U32 dCBBuff,
										U32 dCBBuffSize,
										U08 bMultiPlnMode, 
										U08 bCnt, 
										U16 *pwSrcSeedIndex, 
										U16 *pwDstSeedIndex)
{	
	
	tFtlTask.dTaskType = TYPE_MULTI_LUN_INDIE_COPYBACK_ORDER_PRG;
	tFtlTask.dSrcArgsAddr = (U32)ptSrcInfo;
	tFtlTask.dDstArgsAddr = (U32)ptDstInfo;
	tFtlTask.dHandleBuff = dCBBuff;
	
	tFtlTask.pMskBuff = PNULL;
	tFtlTask.wCur = bCnt;
	tFtlTask.wLen =  bCnt;
	tFtlTask.bNeedCQCount = 0xE0;

#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
#endif	

	FDL_CreateTask(ptTaskCtrl);
	HAL_LDPC_ParityChangeCfg(1,0);

	tFtlTask.dSrcSeedIndex = NULL;
	tFtlTask.dDesSeedIndex = NULL;
	if((pwSrcSeedIndex != NULL)&&(pwDstSeedIndex != NULL))
	{
		tFtlTask.dSrcSeedIndex = pwSrcSeedIndex;
		tFtlTask.dDesSeedIndex = pwDstSeedIndex;
	}
	
	CB_GetInDieTlcCopyBackInfor(dCBBuffSize);
#if defined (__BADBLK_BINDMAP__) ||\
	defined (__BADBLK_BINDMAP_FDLTEST__)//BadBlkBindMap
	tCB_ProcessCtrl.bWtCmdMode = bMultiPlnMode;
#endif
	tFtlTask.FTLTaskErgodic  = TaskErgodicMultiLunCopyBack;
	tFtlTask.FTLTaskAllocate = TaskAllocateMultiLunXXdieTlc;
	tFtlTask.FTLTaskConvert  = TaskConvertMultiLunIndieTlcCopyBack;
	tFtlTask.FTLTaskExceptionHandler = ExceptionCopyBack;

	RunFtlTaskErgodic(TASK_INIT);

}

void FDL_MultiLunInnerCopyBackOrderPrg(FDL_TASK_CTRL *ptTaskCtrl,
										T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo, 
										T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo, 
										U32 dCBBuff,
										U32 dCBBuffSize,
										U08 bMultiPlnMode,  
										U08 bCnt, 
										U16 *pwSrcSeedIndex,
										U16 *pwDstSeedIndex)
{	
	
	tFtlTask.dTaskType = TYPE_MULTI_LUN_INNER_COPYBACK_ORDER_PRG;
	tFtlTask.dSrcArgsAddr = (U32)ptSrcInfo;
	tFtlTask.dDstArgsAddr = (U32)ptDstInfo;
	tFtlTask.dHandleBuff = dCBBuff;
	
	tFtlTask.pMskBuff = PNULL;
	tFtlTask.wCur = bCnt;
	tFtlTask.wLen =  bCnt;
	tFtlTask.bNeedCQCount = 0xE0;

#ifdef __FILL_PAGE_EN__
	HAL_NFSwitchJCTable((U32)(&HAL_JCBufff[JC_BUFF_DATA_ST]),1);//切到数据的补页跳列表位置
#endif	

	FDL_CreateTask(ptTaskCtrl);

	tFtlTask.dSrcSeedIndex = NULL;
	tFtlTask.dDesSeedIndex = NULL;
	if((pwSrcSeedIndex != NULL)&&(pwDstSeedIndex != NULL))
	{
		tFtlTask.dSrcSeedIndex = pwSrcSeedIndex;
		tFtlTask.dDesSeedIndex = pwDstSeedIndex;
	}
	
	CB_GetInDieTlcCopyBackInfor(dCBBuffSize);
		
	tFtlTask.FTLTaskErgodic  = TaskErgodicInnerCopyBack;
	tFtlTask.FTLTaskAllocate = TaskAllocateMultiLunXXdieTlc;
	tFtlTask.FTLTaskConvert  = TaskConvertMultiLunInnerTlcCopyBack;
	tFtlTask.FTLTaskExceptionHandler = ExceptionCopyBack;

	RunFtlTaskErgodic(TASK_INIT);
}

#ifdef _TLC_ORDER_PROGRAM_
U08 FDL_ContinueUnitTask(U08 bSlcOrMixMode, U16 wCount)
{
	U08 i;
	if (wCount > 0)
	{
		tFtlTask.bWriteMode =  bSlcOrMixMode;	//0:SLC Mode  1:TLC Mode mix SLC Mode
		tFtlTask.wLen += wCount;
		if (tFtlTask.bWriteMode == MIX_OPMODE)
		{
			for (i = 0; i < 3; i++)
			{
				RunFtlTaskErgodic(TASK_ADD_SAG);
			}
		}
		else
		{
			RunFtlTaskErgodic(TASK_ADD_SAG);
		}
	}
	else
	{
	}
	return TRUE;
}
#endif
