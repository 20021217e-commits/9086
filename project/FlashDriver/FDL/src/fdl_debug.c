
#include "fdl_debug.h"
#include "fdl_task_op.h"
#include "flash_driver.h"
#include "hal_cq.h"
#include "fdl_copyback.h"
#include "fdl_common.h"


void Fdb_PrintfPhyAddress(T_FLASH_PHY_ADDR *ptAddr, U16 wTotal)
{		
	U16 wOffset  = 0;
	
	PRINT("pAddr000:| DataType | Chan | LunOfChan | ClstOffs | RowAddr  |\n");

#if 0
	for(wOffset = 0; wOffset < wTotal; wOffset++)
	{
		PRINT("pAddr%04d:|....%02X....|..%02X..|.....%02X....|....%02X....|.0x%06X.|\n",wOffset,\
				(ptAddr + wOffset)->m_bDataType,\
				(ptAddr + wOffset)->m_bChan,\
				(ptAddr + wOffset)->m_bLunOfChan,\
				(ptAddr + wOffset)->m_bClstOffs,\
				(ptAddr + wOffset)->m_dRowAddr);
	}
#else
	for(wOffset = 0; wOffset < wTotal; wOffset++)
	{
		PRINT("pAddr%04d:|....%02X....|..%02X..|.....%02X....|....%02X....|.0x%06X.|.%04d-[%04d].|\n",wOffset,\
				(ptAddr + wOffset)->m_bDataType,\
				(ptAddr + wOffset)->m_bChan,\
				(ptAddr + wOffset)->m_bLunOfChan,\
				(ptAddr + wOffset)->m_bClstOffs,\
				(ptAddr + wOffset)->m_dRowAddr,\
				GET_BLOCK_NUM((ptAddr + wOffset)->m_dRowAddr),\
				GET_PAGE_NUM((ptAddr + wOffset)->m_dRowAddr));
	}

#endif
}

void Fdb_PrintfBuffStatus(void)
{		
	#define SWAP_D(A) ( ((A&0x000F) << 12)  |\
					((A&0x00F0) << 4)	|\
					((A&0x0F00) >> 4)	|\
					((A&0xF000) >> 12)) //以4Bit为单位倒叙一个字,例如:0x1234->0x4321

	FDL_Log("%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X",
			 SWAP_D(HAL_BFCGetBuffStatus(0)),SWAP_D(HAL_BFCGetBuffStatus(16)),\
			 SWAP_D(HAL_BFCGetBuffStatus(32)),SWAP_D(HAL_BFCGetBuffStatus(48)),\
			 SWAP_D(HAL_BFCGetBuffStatus(64)),SWAP_D(HAL_BFCGetBuffStatus(80)),\
			 SWAP_D(HAL_BFCGetBuffStatus(96)),SWAP_D(HAL_BFCGetBuffStatus(112)),\
			 SWAP_D(HAL_BFCGetBuffStatus(128)),SWAP_D(HAL_BFCGetBuffStatus(144)),\
			 SWAP_D(HAL_BFCGetBuffStatus(160)),SWAP_D(HAL_BFCGetBuffStatus(176)),\
			 SWAP_D(HAL_BFCGetBuffStatus(192)),SWAP_D(HAL_BFCGetBuffStatus(208)),\
			 SWAP_D(HAL_BFCGetBuffStatus(224)),SWAP_D(HAL_BFCGetBuffStatus(240)));

}

//注意:打印以Sector为单位,当同一个Sector的数据完全相同时,只打印当前Sector的前6个Byte,
//否则打印整个Sector
void Fdb_PrintfMemBySector(U32 pBuffAddr,U32 dSecCnt)
{
#if 0
	//Memory打印-NEW
	U08 TempBuff[SECTOR_SIZE];
	U32 dAddrOffSet = 0;
	for(dAddrOffSet  = pBuffAddr;dAddrOffSet  < (pBuffAddr + dSecCnt*SECTOR_SIZE);dAddrOffSet += SECTOR_SIZE)
	{
		if(0 == ((dAddrOffSet  - pBuffAddr) / SECTOR_SIZE) % 8)
		{
			FDL_Log("\r\n");
		}
	   U08MemSet((U08 *)TempBuff,*((U8 *)((dAddrOffSet + 0))),SECTOR_SIZE);
	   if(HAL_MemComp((U08 *)TempBuff,((U8 *)((dAddrOffSet + 0))),SECTOR_SIZE/4))
	   {
			FDL_Log("%03dSec(%02X)---0x%05X:%02X %02X %02X %02X %02X %02X",(dAddrOffSet - pBuffAddr) / SECTOR_SIZE,(dAddrOffSet - pBuffAddr) / SECTOR_SIZE,dAddrOffSet ,\
				*((U8 *)((dAddrOffSet +0))), *((U8 *)((dAddrOffSet +1))), *((U8 *)((dAddrOffSet +2))),\
				*((U8 *)((dAddrOffSet +3))),*((U8 *)((dAddrOffSet +4))), *((U8 *)((dAddrOffSet +5))));
	   }
	   else
	   {
		   FDL_Log("0x%02X:%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\
				%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",dAddrOffSet ,\
			*((U8 *)((dAddrOffSet +0))), *((U8 *)((dAddrOffSet +1))), *((U8 *)((dAddrOffSet +2))), *((U8 *)((dAddrOffSet +3))),\
			*((U8 *)((dAddrOffSet +4))), *((U8 *)((dAddrOffSet +5))), *((U8 *)((dAddrOffSet +6))), *((U8 *)((dAddrOffSet +7))),\
			*((U8 *)((dAddrOffSet +8))), *((U8 *)((dAddrOffSet +9))), *((U8 *)((dAddrOffSet +10))),*((U8 *)((dAddrOffSet +11))),\
			*((U8 *)((dAddrOffSet +12))),*((U8 *)((dAddrOffSet +13))),*((U8 *)((dAddrOffSet +14))),*((U8 *)((dAddrOffSet +15))),\
			*((U8 *)((dAddrOffSet +16))),*((U8 *)((dAddrOffSet +17))),*((U8 *)((dAddrOffSet +18))),*((U8 *)((dAddrOffSet +19))),\
			*((U8 *)((dAddrOffSet +20))),*((U8 *)((dAddrOffSet +21))),*((U8 *)((dAddrOffSet +22))),*((U8 *)((dAddrOffSet +23))),\
			*((U8 *)((dAddrOffSet +24))),*((U8 *)((dAddrOffSet +25))),*((U8 *)((dAddrOffSet +26))),*((U8 *)((dAddrOffSet +27))),\
			*((U8 *)((dAddrOffSet +28))),*((U8 *)((dAddrOffSet +29))),*((U8 *)((dAddrOffSet +30))),*((U8 *)((dAddrOffSet +31))));
	   }
	}
#endif
}


void Fdb_CQDeepTest(U08 bChanPhy,U08 bFlow,U08 *LogString)
{

#if 0
	if(bFlow == 0)
	{
		HAL_CQWaitDone(bChanPhy);
		SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN = 1;
		SG_FC(bChanPhy)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CQ_CHECK_HOLD;
	}
	else if(bFlow == 1)
	{
		FDL_Log("[%s Chan%d]CQ Deep = %d",LogString,bChanPhy,0x100 - HAL_CQGetIdleSpace(bChanPhy));
	    SG_FC(bChanPhy)->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN =  0;//clear hold en
	    SG_FC(bChanPhy)->rCHK_INT_ST.bits.CQ_HOLD_FLAG = 0;//clear int infor
		HAL_CQWaitDone(bChanPhy);
	}
	else
	{
		 HAL_CQCheckHold(1,0,bChanPhy, 0, 0,0);
		 HAL_CQWaitDone(bChanPhy);
	}
#else
#endif
}
#ifdef __BADBLK_BINDMAP_FDLTEST__//BadBlkBindMap
/**************************************************************************
 *函数名称: Fdb_CopyBackRowCov
 *函数功能: CopyBack PlaneMapping Debug,将Row地址转化为同一个Plane
 *			
 *入口参数:
 * @param dRowAddr：Row地址
 * 
 *出口参数:转化好的Row地址
 * @YS_Kinder(2019年12月13日(周五))
************************************************************************/
U32 Fdb_CopyBackRowCov(U32 dRowAddr)
{
	U32 dRowAddrNew = 0;
	U08 bPlane = GET_PLANE_NUM(dRowAddr);
	U16 wBlock = GET_BLOCK_NUM(dRowAddr);
	U16 wPage  = GET_PAGE_NUM(dRowAddr);
	
	if(bPlane == 0)
	{
		dRowAddrNew = GET_ROW_ADDR_TLC(wPage,wBlock + PLANE_OF_LUN + 1,GET_TLC_FLAG(dRowAddr));/*将Plane0转化为离他最近的Plane1*/
	}
	else
	{
		dRowAddrNew = dRowAddr;
	}
	
	if(wPage < 20)
	{
		FDL_Log("dRowAddr = 0x%X->0x%X",dRowAddr,dRowAddrNew);
	}
	return dRowAddrNew;
}
#endif
/**************************************************************************
 *函数名称: Fdb_RetryLdpcEccGet
 *函数功能: 根据传进来的Retry Type判断并记录最高级别的Retry Type
 *入口参数:
 * @param wRetryTypePar RetryType(0xFF55->1->0xA5->2->0xA5->
 						3->0xA5->4->0xA5->5->0xA5->5->0xA5->0xFFAA)
 * 
 *出口参数:Null
 * @return Null
 *@SG_Kinder(2018年10月16日(周二))
************************************************************************/
#if 0

typedef struct
{
	U32 dRetryCnt;
	U32 dRetrySoftLLR;		/*软解码*/
	U32 dRetryARC1Cnt;		/*ARC*/
	U32 dRetryARC2Cnt;		/*ARC*/
	U32 dRetryARC3Cnt;		/*ARC*/
	U32 dRetryARC4Cnt;		/*ARC*/
	U32 dRetryARC5Cnt;		/*ARC*/
	U32 dRetryOptionCnt;	/*Set Option*/
	U32 dRetryRdCnt;		/*重读*/
	U32 dRetryFail;			/*Fail*/
}T_FDL_RETRY_CNT_RECORD;

//T_FDL_RETRY_CNT_RECORD tRetryCntRecord;

void Fdl_RetryTypeMark(U16 wRetryTypePar)
{
	U08 bLastRetryType 	= 0;
	U08 bCurRetryType 	= 0;
	static U08 bLastPar = 0;
	U16 wRetryType1 	= 0;
	T_FDL_RETRY_CNT_RECORD *tRetryCntRecord = (T_FDL_RETRY_CNT_RECORD *)MEM_COPYBACK_SRC_ADDR;
	U16 *wRetryType 	= (U16 *)(MEM_TXBENCH_W_BUFF + 500);
	U16 wTestArray[13] 	= {0x0000,0xFF55,0x0100,0x01A5,0x0200,0x02A5,0x0300,0x03A5,0x0400,0x04A5,0x0500,0x05A5,0xFFAA};

	for(bLastRetryType = 0;bLastRetryType < 13;bLastRetryType++)/*确认上一次达到的Retry阶段*/
	{
		if(*wRetryType == wTestArray[bLastRetryType])
		{
			break;
		}
	}

	if(wRetryTypePar < 6)/*Arc*/
	{
		bLastPar = wRetryTypePar;
		U16 wRetryTypeTemp = wRetryTypePar<<8;

		for(bCurRetryType = 0;bCurRetryType < 13;bCurRetryType++)
		{
			if(wRetryTypeTemp == wTestArray[bCurRetryType])
			{
				break;
			}
		}

		if(bLastRetryType < bCurRetryType)
		{
			*wRetryType = wRetryTypeTemp;
		}
	}
	else if(wRetryTypePar == 0xA5)/*Soft Decode*/
	{
		U16 wRetryTypeTemp = (bLastPar<<8) + 0xA5;

		for(bCurRetryType = 0;bCurRetryType < 13;bCurRetryType++)
		{
			if(wRetryTypeTemp == wTestArray[bCurRetryType])
			{
				break;
			}
		}

		if(bLastRetryType < bCurRetryType)
		{
			*wRetryType = wRetryTypeTemp;
		}
	}
	else if(wRetryTypePar == 0xFF55)/*Option*/
	{
		if(bLastRetryType == 0)
		{
			*wRetryType = wRetryTypePar;
		}
	}
	else if(wRetryTypePar == 0xFFAA)/*Retry Fail*/
	{
		*wRetryType = wRetryTypePar;
	}
	else if(wRetryTypePar == 0xFFFF)
	{
		switch(*wRetryType)
		{
		case 0xFF55:/*option*/
			tRetryCntRecord->dRetryOptionCnt++;
			FDL_Log("***********optionstage0:RetryType=0x%04X",*wRetryType);
			break;
		case 0x0100:/*Arcstage1*/
			tRetryCntRecord->dRetryARC1Cnt++;
			FDL_Log("**************Arcstage1:RetryType=0x%04X",*wRetryType);
			break;
		case 0x01A5:/*SArcstage1*/
			tRetryCntRecord->dRetrySoftLLR++;
			FDL_Log("************Arc-Sstage1:RetryType=0x%04X",*wRetryType);
			break;
		case 0x0200:/*Arcstage2*/
			tRetryCntRecord->dRetryARC2Cnt++;
			FDL_Log("**************Arcstage2:RetryType=0x%04X",*wRetryType);
			break;
		case 0x02A5:/*SArcstage2*/
			tRetryCntRecord->dRetrySoftLLR++;
			FDL_Log("************Arc-Sstage2:RetryType=0x%04X",*wRetryType);
			break;
		case 0x0300:/*Arcstage3*/
			tRetryCntRecord->dRetryARC3Cnt++;
			FDL_Log("**************Arcstage3:RetryType=0x%04X",*wRetryType);
			break;
		case 0x03A5:/*SArcstage3*/
			tRetryCntRecord->dRetrySoftLLR++;
			FDL_Log("************Arc-Sstage3:RetryType=0x%04X",*wRetryType);
			break;
		case 0x0400:/*Arcstage4*/
			tRetryCntRecord->dRetryARC4Cnt++;
			FDL_Log("**************Arcstage4:RetryType=0x%04X",*wRetryType);
			break;
		case 0x04A5:/*SArcstage4*/
			tRetryCntRecord->dRetrySoftLLR++;
			FDL_Log("************Arc-Sstage4:RetryType=0x%04X",*wRetryType);
			break;
		case 0x0500:/*Arcstage5*/
			tRetryCntRecord->dRetryARC5Cnt++;
			FDL_Log("**************Arcstage5:RetryType=0x%04X",*wRetryType);
			break;
		case 0x05A5:/*SArcstage5*/
			tRetryCntRecord->dRetrySoftLLR++;
			FDL_Log("************Arc-Sstage5:RetryType=0x%04X",*wRetryType);
			break;
		case 0xFFAA:/*RetryFail*/
			tRetryCntRecord->dRetryFail++;
			FDL_Log("**************RetryFail:RetryType=0x%04X",*wRetryType);
			break;
		default:	/*unknownType*/
			FDL_Log("************unknownType:RetryType=0x%04X",*wRetryType);
			break;
		}	
		*wRetryType = 0;
	}
	else
	{
		*wRetryType = 0x5FAF;/*不知道的参数类型*/
		FDL_Log("unknown Type:0x%X",wRetryTypePar);
	}
	
}

#endif


