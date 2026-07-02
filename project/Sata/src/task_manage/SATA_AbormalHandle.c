/*
 * SATA_AbormalHandle.c
 *
 *  Created on: Nov 15, 2019
 *      Author: Sun-PC
 */
#include "common.h"
#include "debug.h"
#include "SATA_AbormalHandle.h"
#include "sg9081.h"
#include "dsata.h"
#include "dma1c.h"
#include "uitility.h"


#ifdef  __FW_TRIGGER_SATA_CRC__

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_H2D_FisCrcFail(void)
// 函数功能: H2D_FIS 植CRC错,启动后  Device 接收到第一个H2D 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_H2D_FisCrcFail(void)
{
	SG_DSATA->rTRXCRC.dAll = 0;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
	SG_DSATA->rTRXCRC.bits.H2DCRCE = 1;
	SG_DSATA->rSTCRCE.bits.dVAL    = 1;
}

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_D2H_FisCrcFail(void)
// 函数功能: D2H_FIS 植CRC错,启动后  Device 发送第一个D2H 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_D2H_FisCrcFail(void)
{
	SG_DSATA->rTRXCRC.dAll = 0;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
	SG_DSATA->rTRXCRC.bits.D2HCRCE = 1;
	SG_DSATA->rSTCRCE.bits.dVAL    = 1;
}

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_DMAActive_FisCrcFail
// 函数功能: DMA_Active_FIS 植CRC错,启动后  Device 发送第一个DMA_Active 发生CRC 错
//
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_DMAActive_FisCrcFail(void)
{
	SG_DSATA->rTRXCRC.dAll = 0;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
	SG_DSATA->rTRXCRC.bits.DMAACRCE = 1;
	SG_DSATA->rSTCRCE.bits.dVAL    = 1;
}
// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_DMASetup_FisCrcFail
// 函数功能: 启动后  Device 发送第一个 DMASetup 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_DMASetup_FisCrcFail(void)
{
	SG_DSATA->rTRXCRC.dAll = 0;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
	SG_DSATA->rTRXCRC.bits.DMASTXCRCE = 1;
	SG_DSATA->rSTCRCE.bits.dVAL    = 1;
}
// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_PIOSetup_FisCrcFail
// 函数功能: 启动后  Device 发送第一个 PIOSetup 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_PIOSetup_FisCrcFail(void)
{
	SG_DSATA->rTRXCRC.dAll = 0;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
	SG_DSATA->rTRXCRC.bits.PIOSCRCE = 1;
	SG_DSATA->rSTCRCE.bits.dVAL    = 1;
}

// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_SDB_FisCrcFail
// 函数功能: 启动后  Device 发送第一个SDB 发生CRC 错
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_SDB_FisCrcFail(void)
{
	SG_DSATA->rTRXCRC.dAll = 0;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
	SG_DSATA->rTRXCRC.bits.SDBCRCE = 1;
	SG_DSATA->rSTCRCE.bits.dVAL    = 1;
}



void Abnormal_SataCtrl_DATA_FisCrcFail_WR(U8 bErrTag,U16 wErrFisNum,U8 bDir)
{
	if(bErrTag == 0x20)
	{
		bErrTag = 0;  // 解决预收的tag 为0x20, 修改为0 的tag 号
	}
	if(0 == bDir)
	{
	    SG_DSATA->rTRXCRC.bits.TXCRCE = 0;
	    SG_DSATA->rTRXCRC.bits.RXCRCE = 1;
	    PRINT_TXT("WR_crc");
	}
	else
	{
	    SG_DSATA->rTRXCRC.bits.TXCRCE = 1;
	    SG_DSATA->rTRXCRC.bits.RXCRCE = 0;
	    PRINT_TXT("Rd_crc");
	}
	SG_DSATA->rTRXCRC.bits.ERR_TAG       = bErrTag;
	SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM  = wErrFisNum;
	SG_DSATA->rSTCRCE.bits.dVAL          = 1;

//	PRINT_HEX("TAGC>",bErrTag);
//	PRINT_HEX("TFIS>",wErrFisNum);

}
void Abnormal_Sata_DisCrcFail(void)
{
	SG_DSATA->rSTCRCE.bits.dVAL = 0;
	SG_DSATA->rTRXCRC.dAll = 0;
}
// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_NONDATA_FisCrcFail
// 函数功能: 触发 NON DATA FIS 发生CRC
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_NONDATA_FisCrcFail(void)
{
    static  U8 bFisType = H2D_FIS;
    switch(bFisType)
    {
	case H2D_FIS:
		 Abnormal_SataCtrl_H2D_FisCrcFail();
		 bFisType= D2H_FIS;
		break;

	case D2H_FIS:
		Abnormal_SataCtrl_D2H_FisCrcFail();
		bFisType= DMA_ACT_FIS;
		break;

	case DMA_ACT_FIS:
		Abnormal_SataCtrl_DMAActive_FisCrcFail();
		bFisType= DMA_SETUP_FIS;
		break;

	case DMA_SETUP_FIS:
		Abnormal_SataCtrl_DMASetup_FisCrcFail();
		bFisType= SDB_FIS;
		break;

	case PIO_SETUP_FIS:
		 Abnormal_SataCtrl_PIOSetup_FisCrcFail();
		 bFisType= SDB_FIS;
		break;

	case SDB_FIS:
		Abnormal_SataCtrl_SDB_FisCrcFail();
		bFisType= H2D_FIS;
		break;

	default:
		break;
    }
}



#endif

#ifdef _SATA_TRIGGER_CRC__
T_SATA_DEBUG  tSataTest  __section(".satadebug");//任务交互队列
// *****************************************************************************
// 函数名称: Abnormal_SataCtrl_NONDATA_FisCrcFail
// 函数功能: 触发 NON DATA FIS 发生CRC
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Abnormal_SataCtrl_NONDATA_FisCrcFail(void)
{
    static  U8 bFisType = 0;
    if( FALSE== tTQC.bIsNcqCmd)
    {
    	if(bFisType > 3)
    	{
    		bFisType= H2D_FIS;
    	}
    }
    tSataTest.bErrCmdType = bFisType;
    switch(bFisType)
    {
#ifdef 	H2D_TRIG_CRC
	case H2D_FIS:
		 Abnormal_SataCtrl_H2D_FisCrcFail();
		 bFisType= D2H_FIS;
		break;
#endif
#ifdef	D2H_TRIG_CRC
	case D2H_FIS:
		Abnormal_SataCtrl_D2H_FisCrcFail();
		bFisType= DMA_ACT_FIS;
		break;
#endif
#ifdef	DMAACTIVE_TRIG_CRC
	case DMA_ACT_FIS:
		Abnormal_SataCtrl_DMAActive_FisCrcFail();
		bFisType= DMA_SETUP_FIS;
		break;
#endif
#ifdef	DMASETUP_TRIG_CRC
	case DMA_SETUP_FIS:
		Abnormal_SataCtrl_DMASetup_FisCrcFail();
		bFisType= SDB_FIS;
		break;
#endif
#ifdef	PIOSETUP_TRIG_CRC
	case PIO_SETUP_FIS:
		 Abnormal_SataCtrl_PIOSetup_FisCrcFail();
		 bFisType= SDB_FIS;
		break;
#endif
#ifdef	SDB_TRIG_CRC
	case SDB_FIS:
		Abnormal_SataCtrl_SDB_FisCrcFail();
		bFisType= H2D_FIS;
		break;
#endif

	default:
		break;
    }
}

void  Abnormal_SataCtrl_DATA_FisCrcFail(U8* pbTags,U8 bCnt)
{
	static U8 bErrTagIdx = 1;
	U8 bDir;
	U8 bTag;
	U8 bTagIdx;
	if( (1== bCnt)&&(SG_DSATA->rPRE_TRIG_CNT))
	{
		tSataTest.bSetStatus =TRUE;
		 tSataTest.bErrFisNum = 0 ; //选中间的Data 触发CRC
		 tSataTest.dFailTotalSec = 0;
		 Abnormal_SataCtrl_DATA_FisCrcFail_WR(tSataTest.bErrTag,tSataTest.bErrFisNum,bDir);
	}
	else
	{
		bErrTagIdx ++;
		if(32 == bErrTagIdx )
		{
			bErrTagIdx =1;
		}
		while(bErrTagIdx > bCnt)
		{
			bErrTagIdx = bErrTagIdx >>1;
		}
		 bTag = pbTags[bErrTagIdx];
		 bDir = tTQC.tHiTasks[bTag].tParam.bDir;
		 tSataTest.bErrTag = tTQC.tHiTasks[bTag].tParam.bTag;
		 tSataTest.dFailTotalSec = 0;
		 bTagIdx = 0;
		 while(bErrTagIdx > bTagIdx )
		 {
			 bTag = pbTags[bTagIdx];
			 tSataTest.dFailTotalSec += tTQC.tHiTasks[bTag].tParam.dSctSize;
			 bTagIdx++;
		 }
		 tSataTest.dFailTotalSec = 0;
		 tSataTest.bSetStatus =TRUE;
		if(SG_DSATA->rPRE_TRIG_CNT)
		{
			tSataTest.bErrFisNum = 0 ; //选中间的Data 触发CRC
			if(bDir)
			{
				Abnormal_SataCtrl_DATA_FisCrcFail_WR(tSataTest.bErrTag,tSataTest.bErrFisNum,bDir);
			}
			else
			{
				Abnormal_SataCtrl_DATA_FisCrcFail_WR(tSataTest.bErrTag,tSataTest.bErrFisNum,bDir);
			}
		}
		else
		{
			 tSataTest.bErrFisNum = (tTQC.tHiTasks[bErrTagIdx].tParam.dSctSize >> 5) ; //选中间的Data 触发CRC
			 tSataTest.dFailTotalSec +=  tSataTest.bErrFisNum<<4;
			if(bDir)
			{
				Abnormal_SataCtrl_DATA_FisCrcFail_WR(tSataTest.bErrTag,tSataTest.bErrFisNum,bDir);
			}
			else
			{
				Abnormal_SataCtrl_DATA_FisCrcFail_WR(tSataTest.bErrTag,tSataTest.bErrFisNum,bDir);
			}
		}
	}
}

void  Abnormal_Sata_Ctrl(U8* pbTag,U8 bCnt)
{
	tSataTest.dTotalcnt ++;
	if( tSataTest.dTotalcnt > 0xFFFF0000)
	{
		tSataTest.dTotalcnt = 0;
	}
	if( 0 == (tSataTest.dTotalcnt%5)) //五次配置一次出错
	{
		tSataTest.dTotalcnt_old = tSataTest.dTotalcnt;
		tSataTest.bSetStatus = TRUE;  //可以配置
		 //保存异常异常值
		{
			tSataTest.dReorderDoneTotal = SG_DSATA->rNCQ_DONE_TOTAL.dAll;
			tSataTest.dReorderTotal = SG_DSATA->rNCQ_REORDER_TOTAL.dAll;
			tSataTest.bSATA_FALSE_FLG = SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL;
			tSataTest.dCRC_PASS_IDX = SG_DSATA->rCRC_PASS_INDEX ;
			tSataTest.dPush_idx_Current = SG_DSATA->rPUSH_INDEX.dAll ;
			tSataTest.dDMA_Idx = SG_DMA1->rINDEX;
		}
		if( 0x57== tSataTest.bTrigCrcFlow)  //NON  DATA FIS Trig CRC
		{
			Abnormal_SataCtrl_NONDATA_FisCrcFail();
		}
		if(0xBA== tSataTest.bTrigCrcFlow)  // DATA FIS Trig CRC
		{
			Abnormal_SataCtrl_DATA_FisCrcFail(pbTag,bCnt);
		}
	}
////	if()  // Reorder Dly
////	{
////
////	}
////	if()  //SDB Dly
////	{
////
////	}
}


void  Abnormal_SataComResetFail_SaveLog(void)
{
	//8
	tSataTest.dComResetFlg = TRUE;
	//1
	tSataTest.dTrigNum ++;
	//2
	//3
	tSataTest.dCRC_PASS_IDX_Err = SG_DSATA->rCRC_PASS_INDEX;
	tSataTest.bSATA_FALSE_FLG = SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL;

	//4
	tSataTest.dPush_idx_Err = SG_DSATA->rPUSH_INDEX.dAll;

	//5
	tSataTest.dCrcFailDMA_IDX = SG_DMA1->rINDEX;
	 //


	//	9
	tSataTest.dReorderTotal_Err = SG_DSATA->rNCQ_REORDER_TOTAL.dAll;
	tSataTest.dReorderDoneTotal_Err = SG_DSATA->rNCQ_DONE_TOTAL.dAll;

}


void  Abnormal_SataFail_SaveLog(void)
{
	//1
	tSataTest.dTrigNum ++;
	//2
	//3
	tSataTest.dCRC_PASS_IDX_Err = SG_DSATA->rCRC_PASS_INDEX;
	tSataTest.bSATA_FALSE_FLG = SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL;

	//4
	tSataTest.dPush_idx_Err = SG_DSATA->rPUSH_INDEX.dAll;

	//5
	tSataTest.dCrcFailDMA_IDX = SG_DMA1->rINDEX;

	//	9
	tSataTest.dReorderTotal_Err = SG_DSATA->rNCQ_REORDER_TOTAL.dAll;
	tSataTest.dReorderDoneTotal_Err = SG_DSATA->rNCQ_DONE_TOTAL.dAll;

}

void  Abnormal_SataCheck(void)
{
	if(tSataTest.dSetTrigCnt == tSataTest.dTrigNum)
	{
		if( 0x57 ==tSataTest.bTrigCrcFlow)  // NON DATA FIS 发生CRC
		{
			//适合单条命令或非NCQ 命令下校验， 不适合多条NCQ 命令校验
//			if(tSataTest.dPush_idx_Current != tSataTest.dPush_idx_Err)  // 校验pushidx
//			{
//				tSataTest.bPush_idx_flg = TRUE;
//	#ifdef	_SATA_TEST_PC_CEHCK_
//				while(1);
//	#endif
//			}
			if(TRUE == tSataTest.bSATA_FALSE_FLG )//
			{
				if(tSataTest.dCRC_PASS_IDX != (SG_DSATA->rCRC_PASS_INDEX - tSataTest.dDMA_Idx_old))
				{
					tSataTest.bSata_FALSE_FLG_Err = TRUE;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
				}
			}
			else
			{
				if(tSataTest.dCRC_PASS_IDX  != (SG_DSATA->rCRC_PASS_INDEX - SG_DMA1->rINDEX))
				{
					tSataTest.bSata_FALSE_FLG_Err = TRUE;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
				}
			}
			//在非NCQ 命令下校验，
			if(tSataTest.dReorderTotal != tSataTest.dReorderDoneTotal)
			{
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
			}
		}
		else if( 0xBA ==tSataTest.bTrigCrcFlow)
		{
			//适合单条命令或非NCQ 命令下校验， 不适合多条NCQ 命令校验
//			if(tSataTest.dPush_idx_Current != tSataTest.dPush_idx_Err)  // 校验pushidx
//			{
//				tSataTest.bPush_idx_flg = TRUE;
//	#ifdef	_SATA_TEST_PC_CEHCK_
//				while(1);
//	#endif
//			}
			if(TRUE == tSataTest.bSATA_FALSE_FLG )//
			{
				if(tSataTest.dCRC_PASS_IDX != (SG_DSATA->rCRC_PASS_INDEX - tSataTest.dDMA_Idx_old))
				{
					tSataTest.bSata_FALSE_FLG_Err = TRUE;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
				}
			}
			else
			{
				if(tSataTest.dCRC_PASS_IDX  != (SG_DSATA->rCRC_PASS_INDEX - SG_DMA1->rINDEX))
				{
					tSataTest.bSata_FALSE_FLG_Err = TRUE;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
				}
			}
			//在非NCQ 命令下校验，
			if(tSataTest.dReorderTotal != tSataTest.dReorderDoneTotal)
			{
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
			}
		}
	}
	else
	{

	}
}

void  Abnormal_Sata_INFS_Check(void)
{
	if(tSataTest.dSetTrigCnt == tSataTest.dTrigNum)
	{
		if( 0x57 ==tSataTest.bTrigCrcFlow)  // NON DATA FIS 发生CRC
		{
			if(SG_DSATA->rSATA_FAIL_FLAG.dAll)
			{
				tSataTest.bSATA_FALSE_FLG = SG_DSATA->rSATA_FAIL_FLAG.dAll;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
			}
		}

	}
}


void  Abnormal_Sata_IFS_Check(void)
{
	if(tSataTest.dSetTrigCnt == tSataTest.dTrigNum)
	{
		 if( 0xBA ==tSataTest.bTrigCrcFlow)
		{
			if(SG_DSATA->rSATA_FAIL_FLAG.dAll)
			{
				tSataTest.bSATA_FALSE_FLG = SG_DSATA->rSATA_FAIL_FLAG.dAll;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
			}
			else
			{
				tSataTest.bSATA_FALSE_FLG = SG_DSATA->rSATA_FAIL_FLAG.dAll;
#ifdef	_SATA_TEST_PC_CEHCK_
					while(1);
#endif
			}

		}
	}
}

void  Abnormal_SataTest_Init(void)
{
	//1
	tSataTest.bTrigCrcFlow = 0x57;
	tSataTest.dSetTrigCnt = 0;
	tSataTest.dTrigNum = 0;
	//2
	tSataTest.dTotalcnt = 0;
	tSataTest.dTotalcnt_old = 0;  //设置出错的值
	tSataTest.dFailTotalSec = 0;
	//3
	tSataTest.dCRC_PASS_IDX_Err = 0;
	tSataTest.dCRC_PASS_IDX = 0;
	tSataTest.bSATA_FALSE_FLG = 0;
	tSataTest.bCRC_PASS_IDX_FALSE = 0 ;
	tSataTest.bSata_FALSE_FLG_Err = 0;
	tSataTest.bMIN_SET_FALSE = 0 ;
	//4
	tSataTest.dPush_idx_Err = 0;
	tSataTest.dPush_idx_Current = 0;
	tSataTest.bPush_idx_flg = 0;
	tSataTest.bPop_idx_flg = 0;
	//5
	tSataTest.dDMA_Idx = 0;
	tSataTest.dCrcFailDMA_IDX = 0;
	tSataTest.bDma_idx_flg = 0;
	tSataTest.bCrcFailDma_idx_flg = 0;
	//8
	tSataTest.dTag_dulErr = 0;
	tSataTest.dLBACrossErr = 0;
	tSataTest.dComResetFlg = 0;
	tSataTest.dSoftResetFlg = 0;
	//	9
	tSataTest.dReorderTotal_Err = 0;
	tSataTest.dReorderTotal = 0;
	tSataTest.dReorderDoneTotal_Err = 0;
	tSataTest.dReorderDoneTotal = 0;
}
#endif


