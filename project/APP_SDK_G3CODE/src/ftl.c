
#include "fdl_test.h"
#include "flash_driver.h"
#include "flash_driver_struct.h"
#include "main.h"
#include "debug.h"
#include "fdl_debug.h"
#include "fdl_task_op.h"
#include "hal_define.h"
#include "pri_cmd.h"
#include "hal_ldpc.h"
#include "uitility.h"
#include "ftl.h"
#include "cmdHandle.h"
#include "hal_test.h"
#include "stdio.h"
#include "fdl_debug.h"
#include "stdlib.h"

#ifdef __TEST_JUPITER_SPEED_SLCMODE__
#define BLK_ADDR_WRITE    0x110 //测试的块地址
#define BLK_ADDR_READ     BLK_ADDR_WRITE//0x110 //测试的块地址
#else
#define BLK_ADDR_WRITE    0x80 //测试的块地址
#define BLK_ADDR_READ     0x20 //测试的块地址

#endif




#define SEC_OF_CLST  8  //每个簇扇区数
#define CLST_OF_PAGE 4  //每个簇扇区数
#define TEST_CAPACITY ((2048)*1024/16)//2048M/1024M/512M/128M/64M/
#define PLANE_OF_DEVICE (TEST_CHAN_MAX*CHIP_OF_CHAN*PLANE_OF_LUN)
#define TESTBLKCNT  Min(((TEST_CAPACITY%PAGE_OF_BLOCK_SLC == 0)?(TEST_CAPACITY/PAGE_OF_BLOCK_SLC):(TEST_CAPACITY/PAGE_OF_BLOCK_SLC + PLANE_OF_DEVICE)),1024)

#define TXBENCH_BEST_CAPACITY (TxBenCHTestParam.Capacity)
#define TXBENCH_BEST_ONCEPAGE (TxBenCHTestParam.WritePagesOnce)
#define TXBENCH_BEST_ERASECNT (TxBenCHTestParam.EraseBlockCnt)
#define TXBENCH_BEST_PLANECNT (TxBenCHTestParam.PlaneOfDevice)


T_TXBENCH_CRTL_PARAM TxBenCHTestParam;

void TxBenCHTestParamInit(void)
{
	TxBenCHTestParam.Capacity 		= TEST_CAPACITY;
	TxBenCHTestParam.WritePagesOnce = 1024;
	TxBenCHTestParam.PlaneOfDevice 	= PLANE_OF_DEVICE;
	TxBenCHTestParam.EraseBlockCnt 	= TESTBLKCNT;
	
    //查找最佳的一次写入页个数(需满足被PLANE_OF_DEVICE整除,并只能少不能多)
	while(TxBenCHTestParam.WritePagesOnce%PLANE_OF_DEVICE != 0)
	{
	    TxBenCHTestParam.WritePagesOnce--;
	}
	//查找最佳的容量(需满足同时被PLANE_OF_DEVICE和WritePagesOnce整除,并只能多不能少)
	while(	(TxBenCHTestParam.Capacity%PLANE_OF_DEVICE != 0) || 
			((TxBenCHTestParam.Capacity%TxBenCHTestParam.WritePagesOnce != 0)))
	{
	    TxBenCHTestParam.Capacity++;
	}

	//擦除快个数调整
	//只能比容量多,不能少
	if(TxBenCHTestParam.Capacity%PAGE_OF_BLOCK_SLC != 0)
	{
		//((TxBenCHTestParam.Capacity - TEST_CAPACITY)/PAGE_OF_BLOCK_SLC)是为了防止容量调整比较大
	    TxBenCHTestParam.EraseBlockCnt += ((TxBenCHTestParam.Capacity - TEST_CAPACITY)/PAGE_OF_BLOCK_SLC + 1)*TxBenCHTestParam.PlaneOfDevice;
	}		
}


void FTL_Initial(void)
{
	
#if defined(__TEST_TXBENCH_SPEED_SLCMODE__) || \
	defined(__TEST_JUPITER_SPEED_SLCMODE__)


	U32 i = 0;

	TxBenCHTestParamInit();
	
#ifndef __TEST_JUPITER_SPEED_SLCMODE__/*Jupiter测试少写点儿数据做Host的Dummy读取*/
    FTL_EraseBlockInit(BLK_ADDR_WRITE);//SLC擦
#endif
    
#if 1//写完第一次可以关掉,后面继续用

	FTL_EraseBlockInit(BLK_ADDR_READ);//SLC擦
#ifdef __TEST_TXBENCH_SPEED_SLCMODE__
	for(i = 0;i < (TXBENCH_BEST_CAPACITY/TXBENCH_BEST_ONCEPAGE);i++)
	{
	     FTL_WriteBlockInit(SLC_MODE,
		 					BLK_ADDR_READ,
		 					i*TXBENCH_BEST_ONCEPAGE/TXBENCH_BEST_PLANECNT,
		 					TXBENCH_BEST_ONCEPAGE);//SLC写
	}
#endif

#ifdef __TEST_JUPITER_SPEED_SLCMODE__/*Jupiter测试少写点儿数据做Host的Dummy读取*/
	for(i = 0;i < 4/*(TXBENCH_BEST_CAPACITY/TXBENCH_BEST_ONCEPAGE)*/;i++)
	{
		 FTL_WriteBlockInit(SLC_MODE,
							BLK_ADDR_READ,
							i*TXBENCH_BEST_ONCEPAGE/TXBENCH_BEST_PLANECNT,
							TXBENCH_BEST_ONCEPAGE);//SLC写
	}

#endif

#endif	
    FTL_ReadBlockInit(SLC_MODE,BLK_ADDR_READ);//SLC读
	FTL_ReadRandClustInit(SLC_MODE,BLK_ADDR_READ);

    FDL_Log("Test TXBenCH By SLCMode!!");
#endif


#ifdef __TEST_TXBENCH_SPEED_TLCMODE__
	TxBenCHTestParamInit();
    FDL_Log("TLC Writing,Please wait a moment....");
    HAL_Test_ErsBlks(TLC_MODE, 0, 3,0,1, 32,35);//TLC擦
    HAL_Test_PrgTlcBlks(0,3,0,1,32,35,1);//TLC写
    FTL_ReadBlockInit(TLC_MODE);//TLC读
    FDL_Log("Test TXBenCH By TLCMode!!");
#endif

}


void FTL_NCQWrite(U08 bSecOffs, U32 dTotalSctSize)
{
    U32 dBufferAddr = MEM_DMA_BUFF_ADDR;

#ifdef __NCQ_WR_TLC_ORDER__
    U16 wCnt = ((U16 *)MEM_JCTABLE_BUFF_ADDR)[0];
    U32 dPlnParamAddr = MEM_JCTABLE_BUFF_ADDR+((sizeof(U16) + 7) & (~(U32)0x07));
    U32 dProgOrderUnitAddr =dPlnParamAddr+((sizeof(T_PLANE_PARAM) + 7) & (~(U32)0x07));

    T_PLANE_PARAM *ptPlaneParam = (T_PLANE_PARAM *)(dPlnParamAddr);
    T_FLASH_PROG_ORDER_UNIT  *ptOrderPrgUnit = (T_FLASH_PROG_ORDER_UNIT *)(dProgOrderUnitAddr);

#if 0//打印物理地址
    U16 wK;
    FDL_Log("Cnt=%d,TotalSct=%d",wCnt,dTotalSctSize);
    FDL_Log("HeadUnitCnt=%d,HeadPln=%d",ptPlaneParam->bHeadUnitCnt,ptPlaneParam->bHeadUnitOfMultiPlane);
    FDL_Log("MidUnitCnt=%d,MidPln=%d",ptPlaneParam->wMidUnitCnt,ptPlaneParam->bMidUnitOfMultiPlane);
    FDL_Log("LastUnitCnt=%d,LastPln=%d",ptPlaneParam->bLastUnitCnt,ptPlaneParam->bLastUnitOfMultiPlane);

    FDL_Log("wCnt000:| Chan | Lun | OpPageCnt | PrgCycle | BlkNum | OrderNum |    Row0    |    Row1    |    Row2    |");
    for(wK=0; wK<wCnt; wK++)
    {
        FDL_Log("wCnt%03d:|..%02d..|.%02d..|....%02d.....|....%02d....|...%02d...|....%02d....|.0x%08x.|.0x%08x.|.0x%08x.|",\
                wK,\
                ptOrderPrgUnit[wK].bChan,\
                ptOrderPrgUnit[wK].bLunOfChan,\
                ptOrderPrgUnit[wK].bOpPageCnt,\
                ptOrderPrgUnit[wK].bPrgCycle,\
                ptOrderPrgUnit[wK].wBlkNum,\
                ptOrderPrgUnit[wK].wOrderNum,\
                ptOrderPrgUnit[wK].dRowAddr[0],\
                ptOrderPrgUnit[wK].dRowAddr[1],\
                ptOrderPrgUnit[wK].dRowAddr[2]);
    }
#endif
    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
    tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
    tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

    FDL_JointOrderPrgUnit(&tTaskCtrl,dBufferAddr,dTotalSctSize,ptPlaneParam,ptOrderPrgUnit,MEM_UD_BUFF_ADDR_EXT,wCnt,NULL);
    FDL_WaitTaskDone();
#else

    T_FLASH_PHY_ADDR  *ptPageAddr = (T_FLASH_PHY_ADDR  *)MEM_JCTABLE_BUFF_ADDR;
    U16 *pwSeedIdx = NULL;
    U16 bPageCnt = (bSecOffs+dTotalSctSize+(SECT_OF_PAGE-1))/SECT_OF_PAGE;
	//打印物理地址
//	Fdb_PrintfPhyAddress(ptPageAddr, bPageCnt);

//	FDL_Log("bPageCnt=%x ",bPageCnt);
    U16 tmp_data = *((U16 *)MEM_SEED_BUFF_ADDR);
	if(tmp_data != 0xFFFF)
	{
	   pwSeedIdx = (U16 *)MEM_SEED_BUFF_ADDR;
	}

    FDL_TASK_CTRL tTaskCtrl= {0};

	//tTaskCtrl.dEvenRaidAddr = MEM_COPYBACK_SRC_ADDR;//偶数
	//tTaskCtrl.dOddRaidAddr 	= MEM_COPYBACK_SRC_ADDR + SECTOR_SIZE*DSECT_OF_PAGE + 16*DATA_CLST_OF_PAGE;
	U08MemSet((U08 *)MEM_COPYBACK_SRC_ADDR,0, (SECTOR_SIZE*DSECT_OF_PAGE + 16*DATA_CLST_OF_PAGE)*2);

	
    tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
    tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
    tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

#ifdef __JPT_DMA1_DATA_CP__
	U32 dDmaSIdx = bSecOffs;
	SG_DMA1->rCP_PAT0 = 0x5A5A5A5A;
	SG_DMA1->rSTATUS.bits.DMA1_CP0_FLAG = 1;
	SG_DMA1->rENABE.bits.DMA1_CP_EN = 1;
	HAL_MCStartDMA1(dBufferAddr,bSecOffs,0, DMA1_WR_TO_BUFF, JOINT_NONE);
	if(SG_DMA1->rCP0_EINDEX != SG_DMA1->rINDEX)
    {
		while(1);
    }

	if(SG_DMA1->rSTATUS.bits.DMA1_CP0_FLAG != 0)

    {
		while(1);
    }

	FDL_JointWritePages(&tTaskCtrl,dBufferAddr, bSecOffs, dTotalSctSize, ptPageAddr,bPageCnt,MEM_UD_BUFF_ADDR_EXT,pwSeedIdx);
    FDL_WaitTaskDone();
    FDL_CheckMultiLunReady();
    
	if(SG_DMA1->rSTATUS.bits.DMA1_CP0_FLAG != 1)
    {
		while(1);
    }

	if(dTotalSctSize >8)
	{
		if(SG_DMA1->rCP0_EINDEX != dTotalSctSize-8+dDmaSIdx)
		{
			while(1);
		}
	}
#else
    FDL_JointWritePages(&tTaskCtrl,dBufferAddr, bSecOffs, dTotalSctSize, ptPageAddr,bPageCnt,MEM_UD_BUFF_ADDR_EXT,pwSeedIdx);
    FDL_WaitTaskDone();
    FDL_CheckMultiLunReady();
#endif
    //FDL_Test_Analyse_Exception(tTaskCtrl.dWriteExBuffAddr,WRITE_TYPE);
#endif
}

void FTL_PreNcqRead(U08 bSecOffsOfClust, U32 dTotalSctSize)
{
	U16 i;
	U16 wDtBuffIdx[16];
	U16 wTabBuffIdx[16];
	
    U32 dClstTotal = (bSecOffsOfClust+dTotalSctSize+(SECT_OF_DATA_CLST-1))/SECT_OF_DATA_CLST;
    T_FLASH_PHY_ADDR *ptSrcClst = (T_FLASH_PHY_ADDR *)MEM_PRE4K_DATA_ROW_ADDR;
	//Fdb_PrintfPhyAddress(ptSrcClst, dClstTotal);

    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
    tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
		
	FDL_Log("4K-Pre Test");
	
	U08 bCtlsLen = 0;  
	U16 wClstIdx = 0;
	U08 bLastChanNum  = 0;
	
	ptSrcClst = (T_FLASH_PHY_ADDR *)MEM_PRE4K_DATA_ROW_ADDR;	
	T_FLASH_PHY_ADDR *ptTabSrcClst = (T_FLASH_PHY_ADDR *)MEM_PRE4K_TAB_ROW_ADDR;
	
	T_NCQREAD_CRTL_PARAM *pPre4KCrtl = (T_NCQREAD_CRTL_PARAM *)MEM_DMA_BUFF_ADDR;
	T_PRE4KDATA_GROUP_PARAM *ptPre4KGroupInfor = (T_PRE4KDATA_GROUP_PARAM *)MEM_PRE4K_DATA_GROUP_ADDR;
	U08 bTestMode = ptPre4KGroupInfor->bTestMode;
	
	
	U08 bTotelLun     = pPre4KCrtl->bTotelLun;
	U08 bStartHold    = pPre4KCrtl->bStartHold;
	U08 bMidHold      = pPre4KCrtl->bMidHold;
	U08 bSectionOfClst= pPre4KCrtl->bSectionSize;
	U08 bTableEn      = pPre4KCrtl->bTableEn;

	T_FLASH_PHY_ADDR  *pClstAddr;		
	T_FLASH_PHY_ADDR  *pTabClstAddr;
	U32 dDataUdBase;
	U32 dTabUdBase;
	U32 dDataUdBuff;
	U32 dTabUdBuff;

	dDataUdBase = MEM_UD_BUFF_ADDR_EXT;
	dTabUdBase  = MEM_UD_BUFF_ADDR_EXT+dClstTotal*UD_LEN_OF_DATA_CLST;

	for(i = 0;i<bTotelLun;i++)
	{
		wDtBuffIdx[i]  = i*SECT_OF_DATA_CLST;
		wTabBuffIdx[i] = (16+i)*SECT_OF_DATA_CLST;
	}
	
	for(i = 0; i <  ptPre4KGroupInfor->dGroupCnt; i++)
	{		
		ptPre4KGroupInfor->dGroupIdx = i;
		
		pClstAddr = ptSrcClst + wClstIdx;
		pTabClstAddr = ptTabSrcClst + wClstIdx;
		
		dDataUdBuff = dDataUdBase+wClstIdx*UD_LEN_OF_DATA_CLST;
		dTabUdBuff = dTabUdBase+wClstIdx*UD_LEN_OF_TBLE_CLST;

		if(i == 0)
		{
			if(bSectionOfClst != 0)
			{
				bCtlsLen = bSectionOfClst;
			}
			else
			{
				bCtlsLen = ptPre4KGroupInfor->bSize[i];
			}
			
			//FDL_Log("4K-Pre Start");
			//Fdb_PrintfPhyAddress(pClstAddr, bCtlsLen);					
			bLastChanNum = FDL_JointPreReadClsts4K(	&tTaskCtrl,
													MEM_DMA_BUFF_ADDR, 
													wDtBuffIdx,
													pClstAddr,
													bCtlsLen ,
													dDataUdBuff, 
													NULL,
													0,
													bStartHold);					
			if(bTableEn)
			{
				//Fdb_PrintfPhyAddress(pTabClstAddr,bCtlsLen);					
				bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,MEM_DMA_BUFF_ADDR,wTabBuffIdx,pTabClstAddr,bCtlsLen,dTabUdBuff,NULL,bLastChanNum,bStartHold);
				if(bTestMode == 1)
				{
					bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,MEM_DMA_BUFF_ADDR,wTabBuffIdx,pTabClstAddr,0,dTabUdBuff,NULL,bLastChanNum,bStartHold);
				}
			}
		}
		else
		{
			if(bSectionOfClst != 0)
			{
				bCtlsLen = bSectionOfClst;
			}
			else
			{
				bCtlsLen = ptPre4KGroupInfor->bSize[i];
			}
								
			//FDL_Log("4K-Pre Mid");
			//Fdb_PrintfPhyAddress(pClstAddr, bCtlsLen);
			bLastChanNum = FDL_JointPreReadClsts4K(	&tTaskCtrl,
													MEM_DMA_BUFF_ADDR, 
													wDtBuffIdx,
													pClstAddr,
													bCtlsLen ,
													dDataUdBuff, 
													NULL, 
													bLastChanNum,
													bMidHold);

			if(bTableEn)
			{
				//Fdb_PrintfPhyAddress(pTabClstAddr,bCtlsLen);					
				bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,MEM_DMA_BUFF_ADDR,wTabBuffIdx,pTabClstAddr,bCtlsLen,dTabUdBuff,NULL,bLastChanNum,bMidHold);
				if(bTestMode == 1)
				{
					bLastChanNum = FDL_PreReadTableToBuff4K(&tTaskCtrl,MEM_DMA_BUFF_ADDR,wTabBuffIdx,pTabClstAddr,0,dTabUdBuff,NULL,bLastChanNum,bMidHold);
				}
			}
		}	
				
		wClstIdx += bCtlsLen;
		
		if(i == ptPre4KGroupInfor->dGroupCnt-1)
		{					
			ptPre4KGroupInfor->dGroupIdx++; 
			bCtlsLen = dClstTotal - wClstIdx;
			pClstAddr = ptSrcClst + wClstIdx;	
			dDataUdBuff = dDataUdBase+wClstIdx*UD_LEN_OF_DATA_CLST;
			
			//FDL_Log("4K-Pre Last");
			//Fdb_PrintfPhyAddress(pClstAddr, bCtlsLen);
			bLastChanNum = FDL_JointPreReadClsts4K(&tTaskCtrl,
													MEM_DMA_BUFF_ADDR, 
													wDtBuffIdx,
													pClstAddr, 
													bCtlsLen ,
													dDataUdBuff, 
													NULL, 
													bLastChanNum,
													0);							
				
			//FDL_Log("4K-Pre Check Last %x",bLastChanNum);
			FDL_Pre4K_WaitTaskDone(&tTaskCtrl,bLastChanNum);
			break;
		}
	}
}


void FTL_ContinueReadTest(U08 bChoiceFunc,T_FLASH_PHY_ADDR *ptClstAddr,U32 dTotalSctSize,U08 bSecOffsOfClust)
{
	U08 bStartFlg =0;
	U16 wClstAddCnt;//每次给FDL的簇个数
	U16 wTaskCnt = 0;
	U16 *pwSeedIdx = NULL;
	U32 dClstTotal = dTotalSctSize/SECT_OF_DATA_CLST;

	while(dClstTotal > 0)
	{
//		Delay(2*1000*100);
//		wClstAddCnt = 8;//(4*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*CLST_OF_PAGE);//一次计算4个通道的并行簇数
		wClstAddCnt = rand()%32 + 1;//(4*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*CLST_OF_PAGE);//一次计算4个通道的并行簇数
		if(wClstAddCnt > dClstTotal)
		{
			wClstAddCnt = dClstTotal;
		}
//		FDL_Log("wClstAddCnt = %d",wClstAddCnt);
		
		if(bStartFlg == 0)//start read
		{
			FDL_WaitTaskDone();
			FDL_TASK_CTRL tTaskCtrl= {0};
			tTaskCtrl.dReadExBuffAddr 	= MEM_READ_EXCEP_BUFF_ADDR;
			tTaskCtrl.wReadExBuffSize 	= MEM_READ_EXCEP_BUFF_SIZE;
			tTaskCtrl.bReadFailCtrl		= READ_FAIL_ALL_RETRY;

			if(bChoiceFunc == 3)
			{
			    FDL_Log("Big-sr:Tot=%d,Cnt=%d",dClstTotal,wClstAddCnt);
				FDL_JointReadClsts( &tTaskCtrl,
									MEM_DMA_BUFF_ADDR,
									ptClstAddr,
									bSecOffsOfClust,
									dTotalSctSize,
									wClstAddCnt,
									MEM_UD_BUFF_ADDR_EXT,
									pwSeedIdx);
			}
			else if(bChoiceFunc == 4)
			{
				FDL_Log("4K-sr:Tot=%d,Cnt=%d",dClstTotal,wClstAddCnt);
			    HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffsOfClust, dTotalSctSize, DMA1_RD_TO_HOST, JOINT_12);
				FDL_JointReadClstsRand4K(	&tTaskCtrl,
											MEM_DMA_BUFF_ADDR,
											ptClstAddr,
											bSecOffsOfClust,
											dTotalSctSize,
											wClstAddCnt,
											MEM_UD_BUFF_ADDR_EXT/*,NULL*/,
											pwSeedIdx);
			}

			dClstTotal -= wClstAddCnt;
			wTaskCnt += wClstAddCnt;
			bStartFlg = 1;
			
		}
		else//continue read
		{
#if 0
			if((dClstTotal > 1) && (dClstTotal <= dClstTotalSRC))
			{
				(ptClstAddr + wTaskCnt)->m_bChan = 0xFF;
			}
#endif

			FDL_ContinueTask(wClstAddCnt);
			
			dClstTotal -= wClstAddCnt;
			wTaskCnt += wClstAddCnt;
		}
	}

	FDL_ContinueTask(0);//后面不用continuetask继续传了
	FDL_WaitTaskDone();//结束掉这次FDL的操作
}

/*
联动Continue不对齐命令测试Case
FTL场景:
		第一把地址尾巴本来要Mask的几个Sector，
		但正好是下一把要Continue的数据的开头，
		所以就不用Mask了，DMA1继续传。
*/
void FTL_ContinueReadDisalignTest(U08 bChoiceFunc,T_FLASH_PHY_ADDR *ptClstAddr,U32 dTotalSctSize,U08 bSecOffsOfClust)
{
	U08 bStartFlg =0;
	U16 wClstAddCnt;//每次给FDL的簇个数
	U16 wTaskCnt = 0;
	U16 *pwSeedIdx = NULL;
	U32 dClstTotal = (bSecOffsOfClust + dTotalSctSize + SECT_OF_DATA_CLST - 1)/SECT_OF_DATA_CLST;

	while(dClstTotal > 0)
	{
		wClstAddCnt = 1;//(4*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*CLST_OF_PAGE);//一次计算4个通道的并行簇数
		if(bStartFlg == 0)//start read
		{
			FDL_WaitTaskDone();
			FDL_TASK_CTRL tTaskCtrl= {0};
			tTaskCtrl.dReadExBuffAddr 	= MEM_READ_EXCEP_BUFF_ADDR;
			tTaskCtrl.wReadExBuffSize 	= MEM_READ_EXCEP_BUFF_SIZE;
			tTaskCtrl.bReadFailCtrl		= READ_FAIL_ALL_RETRY;
			
		    FDL_Log("Big-sr:Tot=%d,Cnt=%d",dClstTotal,wClstAddCnt);

#ifdef __LAST_CLUST_MASK_OFF__
		    bSecOffsOfClust |= 0xF0;/*尾巴不用Mask标记*/
#endif
			FDL_JointReadClsts( &tTaskCtrl,
								MEM_DMA_BUFF_ADDR,
								ptClstAddr,
								bSecOffsOfClust,
								dTotalSctSize,
								wClstAddCnt,
								MEM_UD_BUFF_ADDR_EXT,
								pwSeedIdx);
						
			dClstTotal -= wClstAddCnt;
			wTaskCnt += wClstAddCnt;
			bStartFlg = 1;
			
		}
		else//continue read
		{
#if 0
			if((dClstTotal > 1) && (dClstTotal <= dClstTotalSRC))
			{
				(ptClstAddr + wTaskCnt)->m_bChan = 0xFF;
			}
#endif

			FDL_ContinueTask(wClstAddCnt);
			
			dClstTotal -= wClstAddCnt;
			wTaskCnt += wClstAddCnt;
		}
	}

	FDL_ContinueTask(0);//后面不用continuetask继续传了
	FDL_WaitTaskDone();//结束掉这次FDL的操作
	HAL_ADMWaitDMADone(0xFF);
	HAL_MCWaitDMA1Done();
	Fdb_PrintfBuffStatus();
	HAL_BFCInit();/*完了FTL要自己清除水位*/
}




#if 0
void FTL_NCQRead(U08 bSecOffsOfClust, U32 dTotalSctSize)
{
    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;

    U32 dClstTotal = (bSecOffsOfClust+dTotalSctSize+(SECT_OF_DATA_CLST-1))/SECT_OF_DATA_CLST;
    U16 *pwSeedIdx = NULL;

    U16 tmp_data = *((U16 *)MEM_SEED_BUFF_ADDR);
	if(tmp_data != 0xFFFF)
	{
	   //pwSeedIdx = (U16 *)MEM_SEED_BUFF_ADDR;
	}

    //FDL_Log("SecOffs=%x ,bClstCnt=%x ",bSecOffsOfClust,bClstCnt);
	//打印物理地址
	
	U08MemSet((U08 *)pFdlParam.dEccInfoBuffAddr,0xFF,512*2);
//	Fdb_PrintfPhyAddress(ptClstAddr, dClstTotal);
#if 0
	HAL_ADMSetRndEnable(CHANS(ptClstAddr->m_bChan),1,FUNC_DISABLE/*FUNC_ENABLE*/);/*随机开关*/
	HAL_ADMComSelEccMode(CHANS(ptClstAddr->m_bChan),/*ECC_MODE_LDPC*/ECC_MODE_NONE);/*Ecc开关*/
	HAL_ADMSetUdEnable(CHANS(ptClstAddr->m_bChan),/*FUNC_DISABLE*/FUNC_ENABLE);/*UserData 开关*/	
#endif



	U08 bChoiceFunc = *(U08*)(MEM_DMA_BUFF_ADDR);
//	U08 bChoiceFunc4K = *(U08*)(MEM_DMA_BUFF_ADDR + 1);
    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
    tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
	
	FDL_Log("bChoiceFunc = %d",bChoiceFunc);

	switch(bChoiceFunc)
	{
	    case 0:/*大数据联动读接口*/

			FDL_JointReadClsts(	&tTaskCtrl,
								MEM_DMA_BUFF_ADDR,
								ptClstAddr,
								bSecOffsOfClust,
								dTotalSctSize,
								dClstTotal,
								MEM_UD_BUFF_ADDR_EXT/*,NULL*/,
								pwSeedIdx);
			FDL_Log("Big, MEM_JCTABLE_BUFF_ADDR=0x%x", MEM_JCTABLE_BUFF_ADDR);
			break;

		case 1:/*4K 联动读接口*/
			HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffsOfClust, dTotalSctSize, DMA1_RD_TO_HOST, JOINT_12);
			FDL_JointReadClstsRand4K(	&tTaskCtrl,
										MEM_DMA_BUFF_ADDR,
										ptClstAddr,
										bSecOffsOfClust,
										dTotalSctSize,
										dClstTotal,
										MEM_UD_BUFF_ADDR_EXT/*,NULL*/,
										pwSeedIdx);
			FDL_Log("4K Test");
			break;

		case 2:/*Pre 4K接口*/
			FTL_PreNcqRead(bSecOffsOfClust,dTotalSctSize);
			break;

		case 3:/*大数据Continue读接口*/
			FTL_ContinueReadTest(3,ptClstAddr,dTotalSctSize,bSecOffsOfClust);
			FDL_Log("Continue Read Test");
			break;

		case 4:/*4K Continue读接口*/
			FTL_ContinueReadTest(4,ptClstAddr,dTotalSctSize,bSecOffsOfClust);
			FDL_Log("4K Continue Read Test");
			break;

		case 5:/*大数据Continue读接口*/
			FTL_ContinueReadDisalignTest(5,ptClstAddr,dTotalSctSize,bSecOffsOfClust);
			FDL_Log("Continue Read Disalign Test");
			break;

		default:
			break;


    }

    FDL_WaitTaskDone();
    FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr,READ_TYPE);

}
#else

void FA_ADMInit(void)
{
    U08 i = 0;

	for( i = 0;i < 4;i++)
	{		
		SG_DMA2_CH(i)->rECC_CTRL.bits.ECC_EN = 0;
		SG_DMA2_CH(i)->rENABLE.wAll = 1;
		SG_DMA2_CH(i)->rRND_CTRL.bits.CRC_EN=0;
		SG_DMA2_CH(i)->rRND_CTRL.bits.RND_BYPASS=1;
		SG_DMA2_CH(i)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	}
	
	SG_ADM->rDATA_ERR_STOP_DMA2_EN.dAll = 0x00;

	SG_ADM->rBUF_SIZE = 512;/*仅适用于bSectorOfPage = 38的情况*/
}

void FTL_NCQRead(U32 bSecOffsOfClust, U32 dTotalSctSize)
{	
	U16 wPageOfSet 		= 0;
	T_FLASH_PHY_ADDR *ptPageAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
	U08 bPlaneMode 		= (U08)((ptPageAddr->m_bClstOffs)>>4);//Plane OpertateMode	1-SIngle 2-Two  3-Three...
	U08 bSectorOfPage	= FDL_SECT_OF_PAGE;

	U08 moreSector = bSectorOfPage*bPlaneMode-(bSecOffsOfClust % (bSectorOfPage*bPlaneMode));
//	FDL_Log("moreSector=%d", moreSector);

//	U32 dStatrPage = ((bSecOffsOfClust-(bSectorOfPage*bPlaneMode-1))/(bSectorOfPage*bPlaneMode)) * bPlaneMode;
	U32 dStartPage = (U32)(bSecOffsOfClust/(bSectorOfPage*bPlaneMode)) * bPlaneMode;
	U32 dTotalPage 		= ((bSecOffsOfClust+dTotalSctSize+(bSectorOfPage*bPlaneMode-1))/(bSectorOfPage*bPlaneMode)) *bPlaneMode;

	
	HAL_BFCInit();
	FA_ADMInit();

	if (bSecOffsOfClust > 0) {
		FDL_Log("bSecOffsOfClust=%d,dTotalSctSize=%d,dStartPage =%d,dTotalPage=%d,bSectorOfPage=%d",bSecOffsOfClust,dTotalSctSize,dStartPage,dTotalPage,bSectorOfPage);
	}
	
	if (bSecOffsOfClust > 0 && moreSector > 0) {
		HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffsOfClust,moreSector,DMA1_RD_TO_HOST,JOINT_12);

		for (wPageOfSet = dStartPage; wPageOfSet < dStartPage+bPlaneMode+1; wPageOfSet++) {
			FA_NCQReadData(	(ptPageAddr + wPageOfSet),
								MEM_DMA_BUFF_ADDR,
								wPageOfSet,
								bSectorOfPage);
		}
		dStartPage = dStartPage+bPlaneMode;

		HAL_ADMWaitDMADone(0xFF);
		HAL_MCWaitDMA1Done();
		HAL_BFCInit();

		bSecOffsOfClust = bSecOffsOfClust+moreSector;
		dTotalSctSize = dTotalSctSize-moreSector;
	}

	if (bSecOffsOfClust > 0) {
		FDL_Log("bSecOffsOfClust=%d,dTotalSctSize=%d,dStartPage =%d,dTotalPage=%d,bSectorOfPage=%d",bSecOffsOfClust,dTotalSctSize,dStartPage,dTotalPage,bSectorOfPage);
	}
	
	HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffsOfClust,dTotalSctSize,DMA1_RD_TO_HOST,JOINT_12);
	for(wPageOfSet = dStartPage;wPageOfSet < dTotalPage;wPageOfSet++)
	{
//			FDL_Log("wPageOfSet = %d", wPageOfSet);
			FA_NCQReadData(	(ptPageAddr + wPageOfSet),
											MEM_DMA_BUFF_ADDR,
											wPageOfSet,
											bSectorOfPage);
	}

	
	while(TRUE)
	   {
		   if( (0x100 == HAL_CQGetIdleSpace(0)) &&
			   (0x100 == HAL_CQGetIdleSpace(1)) &&
			   (0x100 == HAL_CQGetIdleSpace(2)) &&
			   (0x100 == HAL_CQGetIdleSpace(3)))
		   {
			   gFdlTskStatus = ST_TASK_DONE;
		   }

		   if (gFdlTskStatus >= ST_TASK_DONE)
		   {

			   break;
		   }
	   }

	HAL_ADMWaitDMADone(0xFF);
	HAL_MCWaitDMA1Done();
//	SG_ADM->rBUF_SIZE.bits.wVAL = 512;

}
#endif

// *****************************************************************************
// 函数功能: 接收host一连续数据联动写入flash,支持起始扇区不构成簇对齐情况
// 入口参数:
//
// 出口参数:
// 说 明:		起始扇区不对齐，FTL需load旧数据
//			此demo代码只演示起始扇区不对齐情况，结束地址是整页对齐，实际不对齐情况
//			FW是把尾缓存
// 测试方法：1. PC先通过写RAM私有命令(bOpCode = 0x11)将
//				a. 需要写的nandflash地一把址放在MEM_PHYADDR1_BUFF_ADDR位置
//				b. 一把userdata放在MEM_UD1_BUFF_ADDR位置
//				c. 一把种子放在MEM_SEED_BUFF_ADDR位置
//				d. 写入数据到DMABUFF上(如不传入，使用BUFF上原先数据)
//			2. PC启动NCQ写，传入两个参数LBA和TotalSect，此LCA映射表最大支持2048
//			3. LBA非簇对齐情况，要读旧数据补齐，带mask只读一个簇，读会占用memory需注意
//				a. 使用的memory不能覆盖步骤1里面已经放好参数的memory
//				b. 读userdata存放位置为MEM_DMA_BUFF_ADDR
//其它说明：由于LCNMAP开辟最大2048，所以写入数据(包括补齐数据)总扇区数不能超过16384
//
// *****************************************************************************
void FTL_LogWrite(U32 dLBA, U32 dTotalSec)
{
#ifdef __FDL_TEST_MODE_EN__
    U08 bSecOffsOfClust = dLBA%32;//SECT_OF_DATA_CLST;//起始扇区地址在簇内偏移

    FTL_NCQWrite(bSecOffsOfClust, dTotalSec);

#endif

}

void FTL_LogRead(U32 dLBA, U32 dTotalSec)
{
#ifdef __FDL_TEST_MODE_EN__
    U32 bSecOffsetOfClust = dLBA;//%SECT_OF_DATA_CLST;//起始扇区地址在簇内偏移

//	FDL_Log("secoffs=%x,Total=%x ",bSecOffsetOfClust,dTotalSec);

    FTL_NCQRead(bSecOffsetOfClust, dTotalSec);
#endif
}

void LgRdWrCtrl(U32 dLBA, U32 dSectCnt, U8 bDir)
{
#if  1//read flash
    //ASSERT_TRUE(dSectCnt);
//	FDL_Log("dSectCnt:%d,dLBA:%x",dSectCnt,dLBA);
    //if((dSectCnt%32) !=0)
    //if(dSectCnt == 1)//上盘系统读的绕过
    if ((dSectCnt == 1)||(dLBA < 0x1000)||((dLBA >= 0x3feff0)))//避免上盘时HOST读文件系统，测试NCQ下发的地址都加0x1000,这里再减回去
    {
        if(bDir == 1)//read
        {
            HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR, 0, dSectCnt, DMA1_RD_TO_HOST, 0);
        }
        else//write
        {
            HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR, 0, dSectCnt, DMA1_WR_TO_BUFF, 0);
        }
//			HAL_ADMWaitDMADone(0xFF);
        return;
    }

    if (dLBA >= 0x1000)
    {
        dLBA -= 0x1000;//避免上盘时HOST读文件系统，测试NCQ下发的地址都加0x1000,这里再减回去
    }

//    FTL_LBAAssert(dLBA, dSectCnt);

    if(bDir)
    {
        FTL_LogRead(dLBA, dSectCnt);
    }
    else
    {
        FTL_LogWrite(dLBA, dSectCnt);
    }

#else //虚拟读写

	FTL_DMA1SendRcvData(dSectCnt, bDir);

#endif
}

U8 NCQ_Check4KCmd(U8 bDir)
{
	if(tTQC.dCnt == tTQC.dPopedNum)
    {
		FDL_Log("nd0=%d",tTQC.dCnt);
        return 0xff;
    }
	
	if(bDir != NCQ_QueueInquire()->tParam.bDir)
	{
		FDL_Log("dir err 1");
		return 0xff;//下一条命令方向不对退出
	}
	
	if(NCQ_QueueInquire()->b4KFlag)
	{
		//FDL_Log("IS 4k %d %d,",tTQC.wCnt,tTQC.wPopedNum);
		return 1;
	}

    return 0;
}


U8 NCQ_Get4KCmd(U8 bDir, U8 *bTags, U8 bNum)
{
    T_HI_TASK* pTask;

	U08 bCmdCnt = 0;

    while(tTQC.dCnt > tTQC.dPopedNum)//有命令
    {
		if(bDir != NCQ_QueueInquire()->tParam.bDir)
		{
			FDL_Log("dir e T");
			break;//下一条命令方向不对退出
		}
		
		if(!NCQ_QueueInquire()->b4KFlag)
		{	
			FDL_Log("not 4k T");
			break;//下一条命令不是4K退出
		}
		
		pTask = NCQ_QueuePop(bDir);
		
		bTags[bCmdCnt++] = pTask->tParam.bTag;//记录返回的TAG号

		if(bCmdCnt == bNum)
		{
			break;//达到需要取的数量退出
		}
    }    

    return bCmdCnt;
}

U8 NCQ_GetNormalCmd(U8 bDir,U8* bTags,U32 *dTotalSize, U8 bNum)
{
    U8 bCmdCnt=0;
    T_HI_TASK* ptask;
    *dTotalSize = 0;
	
    if((tTQC.dCnt == tTQC.dPopedNum) || bNum == 0)
    {
        return 0;
    }

    if(bDir != 0xFF && NCQ_QueueInquire()->tParam.bDir != bDir)
    {
        return 0;
    }

    if(bDir == 0xFF)
    {
        bDir=NCQ_QueueInquire()->tParam.bDir;
    }
	
	if((DIR_IN == bDir) && NCQ_QueueInquire()->b4KFlag)//读4K和非4K独立流程
	{	
		FDL_Log("B 4k");
		return 0;//下一条命令是4K退出
	}

	//1、先记录首条CMD
	ptask=NCQ_QueuePop(bDir);//获取第一条命令
	*dTotalSize +=ptask->tParam.dSctSize;
	bTags[bCmdCnt++] = ptask->tParam.bTag;//记录返回的TAG号

	//2、再判断剩下的可不可以续接（确保续接的时候一定经过条件筛选，旧方案可能存在续接错误问题）
    while(NCQ_GetQueueLen() != 0)
    {
    	//续接剩下的CMD
        if((bDir != NCQ_QueueInquire()->tParam.bDir)||\
           (!NCQ_QueueInquire()->bIsContinue)||\
		   (bCmdCnt == bNum))
		{
            break;//下一条方向不相同的任务,或者不连续，或者找到的数目满了，直接退出
        }

		//满足续接条件
		ptask=NCQ_QueuePop(bDir);
		*dTotalSize +=ptask->tParam.dSctSize;
		bTags[bCmdCnt++] = ptask->tParam.bTag;//记录返回的TAG号

    }
#if 0 //GPIO 9 打印2个脉冲
		SG_PADCTRL->rPORT_DATA16.bits.dVAL |=2;
		SG_PADCTRL->rPORT_DATA16.bits.dVAL &=(~2);
		SG_PADCTRL->rPORT_DATA16.bits.dVAL |=2;
		SG_PADCTRL->rPORT_DATA16.bits.dVAL &=(~2);
#endif

    return bCmdCnt;
}


U08 NCQ_WriteGetCmd(U08 bDir,U08* bTags,U32 *dTotalSize,U08 *bType,U08 bNum)
{
    U08 bCmdCnt = 0;
    T_HI_TASK* ptask;
    *dTotalSize = 0;

#if 1
	U08 b4KStatus = TRUE;//默认认为是连续4K
	U08 bContinueStatus = TRUE;//默认认为是连续地址
	U08 retStatus = 1;//返回是4K还是连续，0代码4K, 1代码连续
#endif
	
    if((tTQC.dCnt == tTQC.dPopedNum) || bNum == 0)
    {
        return 0;
    }

    if(bDir != 0xFF && NCQ_QueueInquire()->tParam.bDir != bDir)
    {
        return 0;
    }

    if(bDir == 0xFF)
    {
        bDir=NCQ_QueueInquire()->tParam.bDir;
    }
    //1、先记录首条CMD
	ptask=NCQ_QueuePop(bDir);//获取第一条命令
	*dTotalSize +=ptask->tParam.dSctSize;
	bTags[bCmdCnt++] = ptask->tParam.bTag;//记录返回的TAG号

	//2、再判断剩下的可不可以续接（确保续接的时候一定经过条件筛选）
    while(NCQ_GetQueueLen() != 0)
    {
        if((bDir != NCQ_QueueInquire()->tParam.bDir))
        {
			break;//方向不相同的任务
        }

		if(bCmdCnt == bNum)
		{
			break;//达到获取的命令条数
		}

		if(!(ptask->b4KFlag && NCQ_QueueInquire()->b4KFlag))//非连续4K
		{
			b4KStatus = FALSE;
		}
		
		if(!NCQ_QueueInquire()->bIsContinue)//后面不连续
		{
			bContinueStatus = FALSE;
		}

		if(bContinueStatus || b4KStatus)
		{
			retStatus = bContinueStatus ? 1 : 0;//一定有一个成立，可能都成立，FTL有优先关注连续
		}
		else
		{
			break;//不能拼接
			//退出后，有2种可能
			//1,只有一条命令，无法判断是4K还是连续
			//2,多条命令，根据retStatus，判断是连续命令还是多个4K, 如果是连续，只需取第一个地址和总长度，如果是多个4K,需要取多个地址，长度不用关系，都是4K
		}
		//满足续接条件
		ptask=NCQ_QueuePop(bDir);//获取第一条命令
		*dTotalSize +=ptask->tParam.dSctSize;
		bTags[bCmdCnt++] = ptask->tParam.bTag;//记录返回的TAG号
    }    

	*bType = retStatus;

    return bCmdCnt;
}



void FTL_DMA1SendRcvData(U32 dLbaLen, U08 bDir)
{
    U16 wTmpSec;
    do
    {
        DEBUG_PRINT("dma1 start!!\r\n");
        if(dLbaLen>=MEM_DMA_BUF_SEC)
        {
            wTmpSec=MEM_DMA_BUF_SEC;
        }
        else
        {
            wTmpSec=dLbaLen;
        }

        HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,0, wTmpSec,!bDir, 0);

        dLbaLen-=wTmpSec;
    }
    while(dLbaLen>0);

}


void FTL_4KRead(void)
{
	U08 j;
    U08 TagNub[32] = {0};
	U08 bCmdCnt;

    //sata端获取参数
    U32 dLbaAddr;
    U32 dTotalSec;

	//flash端控制参数
    U08 bStartFlg =0;
    U16 wClstCnt;//每次给FDL的簇个数
    U32 bContiClsCnt = 0;
//    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;
	T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_TXBENCH_W_BUFF;
    U16 *pwSeedIdx = NULL;

	bCmdCnt = NCQ_Get4KCmd(DIR_IN, TagNub, 32);

	if(bCmdCnt == 0)
	{
		return;
	}
	
	//FDL_Log("4c=%d",bCmdCnt);

	NCQ_CmdReorderQueue(TagNub, bCmdCnt);

    dLbaAddr =NCQ_GetParam(TagNub[0])->tParam.dLba;

	dTotalSec = bCmdCnt * SECT_OF_DATA_CLST;
    wClstCnt = bCmdCnt;//都是4K,命令数就是簇数

	//FDL_Log("clscnt=%d",wClstCnt);
	//FDL_Log("seccnt=%d",dTotalSec);

    while(wClstCnt > 0)
    {
        if(bStartFlg == 0)//start read
        {
            FDL_WaitTaskDone();
			
			FDL_Log("sr-4K:Ad=0x%X,Len=%d,Tot=%d",dLbaAddr,dTotalSec,wClstCnt);
			
            FDL_TASK_CTRL tTaskCtrl= {0};
            tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
            tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
            tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

            FDL_JointReadClstsRand4K(&tTaskCtrl,MEM_DMA_BUFF_ADDR,ptClstAddr,0,dTotalSec,wClstCnt,(U32)NULL,pwSeedIdx);
			HAL_MCStartDMA1(tFtlTask.dHandleBuff, 0, dTotalSec, DMA1_RD_TO_HOST, JOINT_12); 


			bContiClsCnt += wClstCnt;
            bStartFlg = 1;
        }
        else//continue read
        {
            FDL_ContinueTask(wClstCnt);			
            HAL_MCStartDMA1Continue(dTotalSec, DMA1_RD_TO_HOST, JOINT_12);
			if((tFtlTask.wLen % 0x8000) == 0)
			{
//				FDL_Log("continue read%08X:dLbaAddr=0x%X,dLbaLen=%d,dClstTotal=%d,wClstAddCnt=%d",tFtlTask.wLen,dLbaAddr,*dCmdLen,dClstTotal,wClstAddCnt);
//				FDL_Log("continue:Len=%08X,dLbaA=0x%08X,DMAIndex=%08X--%08X",tFtlTask.wLen,dLbaAddr,SG_DMA1->rINDEX,SG_DMA2_CH(0)->rINDEX_CFG);
				FDL_Log("4kctn:Len=%08X,Cur=0x%08X,DMAIndex=%08X--%08X,Log=0x%X",tFtlTask.wLen,tFtlTask.wCur,SG_DMA1->rINDEX,SG_DMA2_CH(0)->rINDEX_CFG,SG_FC(0)->rLOGIC_NUM);
			}
        }

		bCmdCnt = 0;

		//HAL_GPIODebug(9, 2);
		while(1)
		{
			FTL_NCQNewCmdInQuire(&tTQC,DIR_IN,10000);
			j = NCQ_Get4KCmd(DIR_IN, &TagNub[bCmdCnt], 32);
		
			if(j > 0)
			{
				bCmdCnt += j;
		
				// FTL逻辑到物理转换...

				if(bCmdCnt > 0)
				{
					break;
				}
			}
			
			if(HAL_MCGetStatus())
			{
				break;
			}
		}	
		//HAL_GPIODebug(9, 2);
		
		if(bCmdCnt == 0)
		{
			//FDL_Log("n4k");
			break;
		}
		
		//FDL_Log("4k n=%d",bCmdCnt);
		
		NCQ_CmdReorderQueue(TagNub, bCmdCnt);

        dLbaAddr = NCQ_GetParam(TagNub[0])->tParam.dLba;

		dTotalSec = bCmdCnt * SECT_OF_DATA_CLST;
        wClstCnt = bCmdCnt;
		bContiClsCnt += wClstCnt;
		
		//FDL_Log("clsct=%d",wClstCnt);
		//FDL_Log("secct=%d",dTotalSec);

		if(bContiClsCnt > 0xFFFF00/*1024*/)//超过地址buff最大范围
        {
			//FDL_Log(">1024");
            bStartFlg = 0;
            bContiClsCnt = 0;
			HAL_MCWaitDMA1Done();
        }
    }

    FDL_ContinueTask(0);//后面不用continuetask继续传了
    FDL_WaitTaskDone();//结束掉这次FDL的操作
}


void FTL_SATAErrorHandle(U8 bTag,U32 dFisNum)
{
	U32 __sda_addr=__get_sda_base();
	//此处实现清除UserData相关代码,如果userdata不清除会以为是有效数据
    FDL_Log("Enter FTL_SATAErrorHandle!!");
    __restore_sda_base(__sda_addr);
}


#if defined(__TEST_UPDATE_NAND_ADDR_WRITE__) || \
	defined(__TEST_UPDATE_NAND_ADDR__)

void FTL_UpdateNANDAddr(T_FLASH_PHY_ADDR *ptNandAddr,U16 *wAddCnt,U32 *dUpdateLocal,U08 bDir)
{
	//放地址
	U16 wGap 				= 0;
	U32 dLogNum 			= FDL_GetCurLocal(bDir);//当前执行的相对位置
	U16 wNandAddrCnt 		= pFdlParam.wContiAddrMaxRead;
	U32 dLogNumReal 		= dLogNum%wNandAddrCnt;//当前执行的实际位置
	U32 dUpdateLocalReal 	= (*dUpdateLocal)%wNandAddrCnt;//当前更新的实际位置
	U16 wBlockStartAddr 	= bDir?(BLK_ADDR_READ):(BLK_ADDR_WRITE);
	T_FLASH_PHY_ADDR *ptNandAddr2 = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;

	static U16 wPageOfPlane;
	
	//地址Buff初始化阶段
	if((*dUpdateLocal) == 0)
	{
		U16 wAddrCnt		= wNandAddrCnt;
		U08 bChanCnt		= TEST_CHAN_MAX;
		U16 wBlkStartAddr	= wBlockStartAddr;
		U16 wPageOffset 	= 0;
		U08 bInValidChan	= 0xFF;
		U08 bDataType		= 0;				
		FTL_CreateNandAddrWirtRead(ptNandAddr2,wAddrCnt,bDir,bChanCnt,wBlkStartAddr,wPageOffset,bInValidChan,bDataType,SLC_MODE);
//		Fdb_PrintfPhyAddress(ptNandAddr2,wAddrCnt);
		Mem32Copy((U32*)ptNandAddr,(U32*)ptNandAddr2,(*wAddCnt)*2);
		
		(*dUpdateLocal) += (*wAddCnt);//设置相对位置

		//读写2GB数据,在当前Flash配置下每个Plane最多需要提供多少个页
		wPageOfPlane = TXBENCH_BEST_CAPACITY/TXBENCH_BEST_PLANECNT;
		
		FDL_Log("wPlaneOfDevice=%d,wPageOfPlane=%d",TXBENCH_BEST_PLANECNT,wPageOfPlane);
		return;
	}

	//Continue阶段
	if(dUpdateLocalReal >= dLogNumReal)
	{
		wGap = wNandAddrCnt - dUpdateLocalReal + dLogNumReal;
	}
	else
	{
		wGap = dLogNumReal - dUpdateLocalReal;
	}
		
	if(wGap >= (*wAddCnt))
	{
		if((wNandAddrCnt - dUpdateLocalReal) >= (*wAddCnt))//空间充足,随便放
		{
			U08MemCopy((U08*)(ptNandAddr + dUpdateLocalReal),(U08*)(ptNandAddr2 + dUpdateLocalReal),(*wAddCnt)*8);
		}
		else//剩余位置不够,就少放点儿,留着下次再放
		{				
			U08MemCopy((U08*)(ptNandAddr + dUpdateLocalReal),(U08*)(ptNandAddr2 + dUpdateLocalReal),(wNandAddrCnt - dUpdateLocalReal)*8);
			(*wAddCnt) = (wNandAddrCnt - dUpdateLocalReal);
		}
		
		(*dUpdateLocal)  += (*wAddCnt);//更新当前覆盖的相对位置
		dUpdateLocalReal = (*dUpdateLocal)%wNandAddrCnt;//更新当前覆盖的实际位置
		
		if(dUpdateLocalReal == 0)//源Buff里面的地址用完了,重新更新
		{
			U16 wAddrCnt		= wNandAddrCnt;//
		    U08 bChanCnt		= TEST_CHAN_MAX;
		    U16 wBlkStartAddr	= wBlockStartAddr;
			U16 wPageOffset 	= bDir?(((*dUpdateLocal)/TXBENCH_BEST_PLANECNT/DATA_CLST_OF_PAGE)%wPageOfPlane):(((*dUpdateLocal)/TXBENCH_BEST_PLANECNT)%wPageOfPlane);//12288
		    U08 bInValidChan	= 0xFF;
		    U08 bDataType		= 0;
			FTL_CreateNandAddrWirtRead(ptNandAddr2,wAddrCnt,bDir,bChanCnt,wBlkStartAddr,wPageOffset,bInValidChan,bDataType,SLC_MODE);
//			Fdb_PrintfPhyAddress(ptNandAddr2,wAddrCnt);
			
			if((wPageOffset >= wPageOfPlane) && (bDir == 0))//写的数据超范围,要重新擦掉
			{
			    FTL_EraseBlockInit(BLK_ADDR_WRITE);//SLC擦
			}
			//给SATA置错
#if 0
			if(bDir == 0)
			{
				SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM	= (SG_DMA1->rINDEX/17*13)%16;//配置出错时的数据量,以8K为单位(0:第一个8K出错,1:第二个8K出错,…)
				SG_DSATA->rTRXCRC.bits.ERR_TAG		= (SG_DMA1->rINDEX/13*11)%32;//植入的出错命令的Tag号(非NCQ命令=0)
				SG_DSATA->rTRXCRC.bits.RXCRC 		= 1;//Write
				SG_DSATA->rSTCRCE.dAll				= 1;   				
				FDL_Log("Set SATA Error!!*******TAG:%d,Num:%d",SG_DSATA->rTRXCRC.bits.ERR_TAG,SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM);
			}
#endif


		}
	}
	else
	{	
		FDL_Log("warning:Convert is too slow,waiting!!!wGap(%d) < (*wAddCnt)(%d)",wGap,(*wAddCnt));
		(*wAddCnt) = 0;//Convert还没有把地址取走(LogNum与覆盖位置离得太近),这次不放,等Convert一段时间
	}

}
#endif

void FTL_NormalRead(void)
{
    U08 TagNub[32] = {0};
	U08 bCmdCnt;

    //sata端获取参数
    U32 dLbaAddr;
    U32 dTotalSec;

	//flash端控制参数
    U08 bStartFlg =0;
    U08 bSecOffsOfClust;
    U32 dClstTotal;//获取一次LBA,LEN后要读多少个簇
    U16 wClstAddCnt;//每次给FDL的簇个数
    U32 bContiClsCnt = 0;
	U08 bNextEndOffs;
	
#if 1//def __TEST_UPDATE_NAND_ADDR__	
	U32 dUpdateLocal = 0;//当前更新到哪里(内存中的相对位置)
#endif
	T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
	U16 *pwSeedIdx = NULL;

	bCmdCnt = NCQ_GetNormalCmd(DIR_IN, TagNub, &dTotalSec,32);

	//FDL_Log("BC=%d",bCmdCnt);

	if(bCmdCnt == 0)
	{
		return;
	}
#if 1
	else if (dTotalSec < 8)/*系统读文件系统时，不让操作Flash*/
	{
		FDL_Log("Dummy Read-%d",dTotalSec);
		NCQ_CmdReorderQueue(TagNub,bCmdCnt);
	    HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR, 0, dTotalSec, DMA1_RD_TO_HOST, 0);
		return;
	}
#endif

	NCQ_CmdReorderQueue(TagNub,bCmdCnt);

    dLbaAddr = NCQ_GetParam(TagNub[0])->tParam.dLba;

	//FDL_Log("B LBA=%d,LEN=%d",dLbaAddr,dTotalSec);

    bSecOffsOfClust = dLbaAddr % SECT_OF_DATA_CLST;//起始扇区在簇内的偏移
    dClstTotal = (bSecOffsOfClust + dTotalSec + (SECT_OF_DATA_CLST-1))/SECT_OF_DATA_CLST;//此次逻辑读要读flash簇个数

	bNextEndOffs = ((dLbaAddr + dTotalSec) % SECT_OF_DATA_CLST);//起始扇区在簇内的偏移

	//FDL_Log("BT=%d,ED=%d",dClstTotal, bNextEndOffs);

    bStartFlg = 0;

    while(dClstTotal > 0)
    {
        //FTL不可能一次把所有物理地址都计算出来，FTL先计算出一部分
        wClstAddCnt = (4*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*CLST_OF_PAGE);//一次计算4个通道的并行簇数

        if(wClstAddCnt > dClstTotal)
        {
            wClstAddCnt = dClstTotal;
        }
		
        //FTL取映射表，向地址buff填簇地址信息(此模拟代码已经放好)
        if(bStartFlg == 0)//start read
        {
            FDL_WaitTaskDone();
#if 1//def __TEST_UPDATE_NAND_ADDR__
			dUpdateLocal = 0;
			FTL_UpdateNANDAddr(ptClstAddr,&wClstAddCnt,&dUpdateLocal,1);
#endif
			FDL_TASK_CTRL tTaskCtrl= {0};
            tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
            tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
            tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

			FDL_Log("Big-sr:Ad=0x%X,Len=%d,Tot=%d,Cnt=%d",dLbaAddr,dTotalSec,dClstTotal,wClstAddCnt);
//			FDL_Log("Restart111--p:%d,%d,0x%X",bSecOffsOfClust,bNextEndOffs,bContiClsCnt);
//			Fdb_PrintfPhyAddress(ptClstAddr,wClstAddCnt);

            FDL_JointReadClsts(	&tTaskCtrl,
								MEM_DMA_BUFF_ADDR,
								ptClstAddr,
								bSecOffsOfClust,
								dTotalSec,
								wClstAddCnt,
								(U32)NULL,
								pwSeedIdx);

			bContiClsCnt += wClstAddCnt;
			
			if((bSecOffsOfClust == 0) && (bNextEndOffs == 0))
			{
            	bStartFlg = 1;
			}
			else//当前任务有Mask,下次不能Continue,要重新启动
			{
			   break; 
			}
        }
        else//continue read
        {
#ifdef __TEST_UPDATE_NAND_ADDR__
			FTL_UpdateNANDAddr(ptClstAddr,&wClstAddCnt,&dUpdateLocal,1);//更新地址
#endif	
            FDL_ContinueTask(wClstAddCnt);
			if((tFtlTask.wLen % 0x8000) == 0)
			{
//				FDL_Log("continue read%08X:dLbaAddr=0x%X,dLbaLen=%d,dClstTotal=%d,wClstAddCnt=%d",tFtlTask.wLen,dLbaAddr,*dCmdLen,dClstTotal,wClstAddCnt);
//				FDL_Log("continue:Len=%08X,dLbaA=0x%08X,DMAIndex=%08X--%08X",tFtlTask.wLen,dLbaAddr,SG_DMA1->rINDEX,SG_DMA2_CH(0)->rINDEX_CFG);
				FDL_Log("continue:Len=%08X,Cur=0x%08X,DMAIndex=%08X--%08X,Log=0x%X",tFtlTask.wLen,tFtlTask.wCur,SG_DMA1->rINDEX,SG_DMA2_CH(0)->rINDEX_CFG,SG_FC(0)->rLOGIC_NUM);
			}
        }
		
        dClstTotal -= wClstAddCnt;

		if(dClstTotal == 0)
        {
			FTL_NCQNewCmdInQuire(&tTQC,DIR_IN,10000);
			bCmdCnt = NCQ_GetNormalCmd(DIR_IN, TagNub, &dTotalSec,32);
			//FDL_Log("b2T=%d",bCmdCnt);

			if (bCmdCnt == 0)
            {
				FDL_Log("1.Continueend=%d-0x%x",bCmdCnt,tFtlTask.wLen);
                break;
            }
			
            NCQ_CmdReorderQueue(TagNub, bCmdCnt);
            dLbaAddr =NCQ_GetParam(TagNub[0])->tParam.dLba;
            bSecOffsOfClust = dLbaAddr % SECT_OF_DATA_CLST;//起始扇区在簇内的偏移
            bNextEndOffs = ((dLbaAddr + dTotalSec) % SECT_OF_DATA_CLST);//起始扇区在簇内的偏移
            dClstTotal = (bSecOffsOfClust + dTotalSec + (SECT_OF_DATA_CLST-1))/SECT_OF_DATA_CLST;//此次逻辑读要读flash簇个数
            bContiClsCnt += dClstTotal;

            //FDL_Log("gT:Ad=0x%X,Len=%d,ctn=%d,Offs=%d",dLbaAddr,dTotalSec,bContiClsCnt,bSecOffsOfClust);

			if((bSecOffsOfClust != 0) || (bNextEndOffs != 0) || (bContiClsCnt > 0xFFFF00)/*(bContiClsCnt > 1024)*/)//后面地址不对齐或长度不够成整簇
            {			
            	FDL_ContinueTask(0);	
//				FDL_Log("gT:Ad=0x%X,Len=%d,ctn=%d,Offs=%d,bContiClsCnt=%x",dLbaAddr,dTotalSec,bContiClsCnt,bSecOffsOfClust,bContiClsCnt);
		        bStartFlg = 0;
                bContiClsCnt = 0;
//				HAL_MCWaitDMA1Done();
				FDL_Log("2.Continueend=%d--Len=%08X",bCmdCnt,tFtlTask.wLen);
            }
            else
            {
                HAL_MCStartDMA1Continue(dTotalSec, DMA1_RD_TO_HOST, JOINT_12);
            }
        }
    }

	if((tFtlTask.wLen != 0x40000) && (tFtlTask.wLen > 32))
	{
	    FDL_Log("RD NCQ CMD Not Continued-Len=0x%x",tFtlTask.wLen);
	}

    FDL_ContinueTask(0);//后面不用continuetask继续传了
    FDL_WaitTaskDone();//结束掉这次FDL的操作
}

void FTL_LGRead(void)
{
	U08 bRet;
	
    tSataDC.wNcqTOCnt=1;//低于brust 条件时单次启动命令条数
    tSataDC.bNcqBrustTH=32;//sata 命令缓存超过该值开启brust 传输模式
    tSataDC.bNcqMaxBrustCnt=5;//单次brust 传输的最大命令数

    do
    {
		bRet = NCQ_Check4KCmd(DIR_IN);

		//FDL_Log("bRet=%x",bRet);

		if (bRet == 0xff)
        {
            continue;//没有命令了，或下条命令方向不对
        }
		
		if (bRet == 1)
        {
			tSataDC.wNcqTOCnt = 1;

			FTL_4KRead();
        }
        else
        {
			tSataDC.wNcqTOCnt = 4;
			
            FTL_NormalRead();
			
        }
		//出现SATA异常,需要清除填充标志
		if(SG_DSATA->rFILL_INVLDA_ST.dAll != 0)
		{
			SG_DSATA->rFILL_INVLDA_ST.dAll = 0;
			//FDL_Log("*******************SATA CRC Erro!!");
		}
    }while(FTL_NCQNewCmdInQuire(&tTQC,DIR_IN,1000));
}

/*
void FTL_WriteData()
{
	U08 TagNub[32] = {0};
	U08 bCmdCnt;

	U32 dLbaAddr;
	U32 dTotalSec;//每次获取命令扇区数

	U08 bSecOffs;//当前命令的起始扇区簇内偏移
	U08 bEndOffs;//当前命令的结束扇区簇内偏移
	U08 bNextEndOffs;//下一条命令的结束扇区簇内偏移
	
	U32 dPageTotal;//获取一次LBA,LEN后要读多少个页
	U16 wPageAddCnt;//每次给FDL的页个数
	U32 dRemainSec;//还未转换成flash地址的DMA1剩余扇区数
	
	U08 bStartFlg =0;
	U16 wPageAddCntMax = 4*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN+4;//一次计算4个通道的并行簇数

	T_FLASH_PHY_ADDR *ptPageAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;

	U16 *pwSeedIdx = NULL;
#ifdef __TEST_UPDATE_NAND_ADDR_WRITE__
	U32 dUpdateLocal = 0;//当前更新到哪里(内存中的相对位置)
#endif
	U08 retTpye;
	do
	{
		bCmdCnt = NCQ_WriteGetCmd(DIR_OUT,TagNub,&dTotalSec,&retTpye,16);

		if (!bCmdCnt)
		{
			continue;
		}
		
		NCQ_CmdReorderQueue(TagNub,bCmdCnt);

		dLbaAddr = NCQ_GetParam(TagNub[0])->tParam.dLba;
		bSecOffs = dLbaAddr % SECT_OF_DATA_CLST;//起始扇区在页内的偏移
		bEndOffs = ((dLbaAddr + dTotalSec) % SECT_OF_DATA_CLST);

		if(((bSecOffs != 0) || (bEndOffs != 0)) && (dTotalSec < SECT_OF_PAGE))//头或尾不对齐，并且够不成整页
		{
			//FDL_Log("small=%x %x %x",bSecOffs,bEndOffs,dTotalSec);

			//先缓存，地址FTL需分配好
			HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffs, dTotalSec, DMA1_WR_TO_BUFF, JOINT_NONE);	
			HAL_MCWaitDMA1Done();
			continue;
		}
		else //大数据或对齐数据
		{
			dPageTotal = (bSecOffs + dTotalSec) / SECT_OF_PAGE;//flash页个数，为0等于启动联动接口先接收数据
			bStartFlg = 0;
			dRemainSec = dTotalSec;
	
			while(dRemainSec > 0)
			{
				do
				{
					wPageAddCnt = dPageTotal;//存在第一次启动dPageTotal为0的情况，相当于先启动DMA1

					if(wPageAddCnt > wPageAddCntMax)
					{
						wPageAddCnt = wPageAddCntMax;
					}

					//FTL取映射表，向地址buff填簇地址信息
					
					if(bStartFlg == 0)//start read
					{						
						FDL_WaitTaskDone();

						//启动数据接收
						//HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffs, dTotalSec, DMA1_WR_TO_BUFF, JOINT_12);	

						if(bSecOffs > 0)
						{
							//read old clust data...
						}
#ifdef __TEST_UPDATE_NAND_ADDR_WRITE__	
						dUpdateLocal = 0;
						FTL_UpdateNANDAddr(ptPageAddr,&wPageAddCnt,&dUpdateLocal,0);
#endif
						FDL_TASK_CTRL tTaskCtrl= {0};
						tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
						tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
						tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_NO_STOP;//WRITE_FAIL_STOP;

						FDL_Log("start w:Lba=0x%X,Len=%d,Total=%d,AddCnt=%d,SecOff=%d",dLbaAddr,dTotalSec,dPageTotal,wPageAddCnt,bSecOffs);
						FDL_JointWritePages(&tTaskCtrl,
										   MEM_DMA_BUFF_ADDR,
										   bSecOffs,
										   dTotalSec,
										   ptPageAddr,
										   wPageAddCnt, 					   
										   MEM_UD1_BUFF_ADDR,
										   pwSeedIdx);
						
						bStartFlg = 1;			   
						dRemainSec -= (wPageAddCnt*SECT_OF_PAGE-bSecOffs);
					}
					else//continue read
					{
						if(wPageAddCnt > 0)//凑够1个页
						{			
							//HAL_GPIODebug(9, 3);
							//FDL_Log("ctn w:Lba=0x%X,Len=%d,Total=%d,AddCnt=%d,SecOff=%d",dLbaAddr,dTotalSec,dPageTotal,wPageAddCnt,bSecOffs);
#ifdef __TEST_UPDATE_NAND_ADDR_WRITE__
							FTL_UpdateNANDAddr(ptPageAddr,&wPageAddCnt,&dUpdateLocal,0);
#endif
							FDL_ContinueTask(wPageAddCnt);
							dRemainSec -= (wPageAddCnt*SECT_OF_PAGE);
						}
					}

					dPageTotal -= wPageAddCnt;					
				}while(dPageTotal > 0);


				//查询下一条命令
				while(1)
				{
					bCmdCnt = NCQ_WriteGetCmd(DIR_OUT, TagNub, &dTotalSec,&retTpye,16);

					if(bCmdCnt > 0)
					{
						//HAL_GPIODebug(9, 1);
						break;
					}

					if( (tFtlTask.wLen == tFtlTask.wCur)
						&&(0 == tChTaskQueue[0].wTaskCnt)
						&&(0 == tChTaskQueue[1].wTaskCnt)
						&&(0 == tChTaskQueue[2].wTaskCnt)
						&&(0 == tChTaskQueue[3].wTaskCnt)
						&&(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(0))
						&&(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(1))
						&&(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(2))
						&&(CQ_MAX_IDLE_SPACE == HAL_CQGetIdleSpace(3)))
					{
						if(FTL_NCQNewCmdInQuire(&tTQC,DIR_OUT,10000))
						{
							continue;
						}
						else
						{
							FDL_Log("2.0.ContinueTask end,tFtlTask.wLen=%d",tFtlTask.wLen);
							//FDL_Log("ncm0");
							break;//dma2都完成了还没新命令来
						}
					}
				}
				
				if (bCmdCnt == 0)
				{		
					if(dRemainSec > 0)//最后剩余的扇区数据水位清除，FTL单独处理
					{
						//FDL_Log("RemSec:%d",dRemainSec);
						//已经在RAM里，没写到flash上的数据，要清除水位
						HAL_BFCInit();

						//并把这部分遗留数据处理掉(搬走或写到哪里)
						//......
						
						dRemainSec = 0;
					}
					FDL_Log("2.1.ContinueTask end,tFtlTask.wLen=%d",tFtlTask.wLen);
					break;
				}
				
				//HAL_GPIODebug(9, 1);
				NCQ_CmdReorderQueue(TagNub, bCmdCnt);

				dLbaAddr =NCQ_GetParam(TagNub[0])->tParam.dLba;

				bSecOffs = dLbaAddr % SECT_OF_DATA_CLST;
				bNextEndOffs = ((dLbaAddr + dTotalSec) % SECT_OF_DATA_CLST);
				
				if((bEndOffs != 0) //上一条命令尾不对齐
					|| (bSecOffs != 0) //下一条命令头不对齐
					|| (bNextEndOffs != 0))//下一条命令尾不对齐
				{
					//FDL_Log("bSecOff:%d,bNextEndOffs:%d",bSecOffs,bNextEndOffs);
					bStartFlg = 0;

					FDL_Log("1.ContinueTask end");
					FDL_ContinueTask(0);
					FDL_WaitTaskDone();//等DMA2传完

					if(dRemainSec > 0)//最后剩余的扇区数据水位清除，FTL单独处理
					{
						//FDL_Log("dRemainSec:%d",dRemainSec);
						//已经在RAM里，没写到flash上的数据，要清除水位
						HAL_BFCInit();

						//并把这部分遗留数据处理掉(搬走或写到哪里)
						//......
						
						dRemainSec = 0;
					}
					
					if(((bSecOffs != 0) || (bEndOffs != 0)) && (dTotalSec < SECT_OF_PAGE))//头或尾不对齐，并且够不成整页
					{
						//FDL_Log("small=%x %x %x",bSecOffs,bEndOffs,dTotalSec);
					
						//先缓存，地址FTL需分配好
						HAL_MCStartDMA1(MEM_DMA_BUFF_ADDR,bSecOffs, dTotalSec, DMA1_WR_TO_BUFF, JOINT_NONE);	
						HAL_MCWaitDMA1Done();	
					}

					break;//
				}
				else
				{
					HAL_MCStartDMA1Continue(dTotalSec, DMA1_WR_TO_BUFF, JOINT_12);
					
					dRemainSec += dTotalSec;//得到新的DMA1还未转化DMA2页地址的剩余扇区数
					dPageTotal = (bSecOffs + dRemainSec)/SECT_OF_PAGE;//得到新的还需写的页数，不对齐会少一页
					//FDL_Log("D%d %d",dRemainSec,dPageTotal);
					//HAL_GPIODebug(9, 2);

				}
			} //end  while(dRemainSec > 0)
		}
		
	}while(FTL_NCQNewCmdInQuire(&tTQC,DIR_OUT,1000));

	FDL_Log("2.2.ContinueTask end,tFtlTask.wLen=%d",tFtlTask.wLen);
	if(tFtlTask.wLen != 65536)
	{
	    FDL_Log("WR NCQ CMD Not Continued-Len=0x%x",tFtlTask.wLen);
	}
	FDL_ContinueTask(0);
	FDL_WaitTaskDone();
	FDL_CheckMultiLunReady();
	//FDL_Log("taskdone");
}
*/

void FTL_LGWrite(void)
{
    tSataDC.wNcqTOCnt = 32;//低于brust 条件时单次启动命令条数
    tSataDC.bNcqBrustTH=16;//sata 命令缓存超过该值开启brust 传输模式
    tSataDC.bNcqMaxBrustCnt=5;//单次brust 传输的最大命令数

    //FTL_EraseBlockInit(BLK_ADDR_WRITE);//SLC擦

	//FTL_WriteData();
}


void FTL_SetNandAddr(T_FLASH_PHY_ADDR *ptAddr,U08 bDataType,U08 bChan,U08 bClstOffs,U08 bLunOfChan,U32 dRowAddr)
{
    ptAddr->m_bDataType  = bDataType;
    ptAddr->m_bChan      = bChan;
    ptAddr->m_bClstOffs  = bClstOffs;
    ptAddr->m_bLunOfChan = bLunOfChan;
    ptAddr->m_dRowAddr   = dRowAddr;
}

void FTL_CreateNandAddrRandom(T_FLASH_PHY_ADDR  *ptClustAddrRD,U16 wAddrCnt,U08 bDir,U08 bChanCnt,U16 wBlkStartAddr,U32 dPageOffset,U8 bInValidChan,U08 bDataType,U08 Mode)
{
	U32 dRowAddrOfPage,dRowAddrOfBlock;
	//srand((int)time(0));
	srand(5627);
	U16 i = 0;
	for(i = 0;i < 1024;i++)
	{
		dRowAddrOfPage	= rand() % 16;//行地址中的页号
		dRowAddrOfBlock = (wBlkStartAddr);//行地址中的块号

		(ptClustAddrRD + i)->m_bDataType	= bDataType;   // 数据类型
		#if 0
		(ptClustAddrRD + i)->m_bChan		= rand() % TEST_CHAN_MAX;	   // 通道编号
		(ptClustAddrRD + i)->m_bLunOfChan	= rand() % FDL_CHIP_OF_CHAN;	// 通道中的Lun编号
		#else
		(ptClustAddrRD + i)->m_bChan		= i % TEST_CHAN_MAX;	   // 通道编号
		(ptClustAddrRD + i)->m_bLunOfChan	= (i/FDL_CHIP_OF_CHAN) % FDL_CHIP_OF_CHAN;	// 通道中的Lun编号
		#endif
		(ptClustAddrRD + i)->m_bClstOffs	= rand() % 4;	// 页中簇偏移
		(ptClustAddrRD + i)->m_dRowAddr 	= dRowAddrOfBlock*FDL_ROW_OF_BLK + dRowAddrOfPage;	  // Lun中的行地址
	}
}


//多CE多Plane模式  //loop order Clst -> Chan -> Plan ->Chip
void FTL_CreateNandAddrWirtRead(T_FLASH_PHY_ADDR  *ptPageAddr,
								U16 wAddrCnt,
								U08 bDir,
								U08 bChanCnt,
								U16 wBlkStartAddr,
								U32 dPageOffset,
								U08 bInValidChan,
								U08 bDataType,
								U08 Mode)
{
    U32 dRowAddr	=0;
    U32 i,j,k,n,m;
//loop order Clst -> Chan -> Plan ->Chip
    if(bDir == 1)
    {
        U08 bClstOfPage	=((TYPE_LOGIC == bDataType)?DATA_CLST_OF_PAGE : TBLE_CLST_OF_PAGE);//一个Page的簇个数确认
        for(i=0; i<(wAddrCnt/bClstOfPage)/(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt) + (wAddrCnt/bClstOfPage)%(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt); i++)
        {
            if(i < (wAddrCnt/bClstOfPage)/(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))
            {
                for(j=0 ; j<FDL_CHIP_OF_CHAN; j++)
                {
                    for(k=0; k<FDL_PLANE_OF_LUN; k++)
                    {
                        for(n=0; n<bChanCnt; n++)
                        {
                            for(m=0; m<bClstOfPage; m++)
                            {
                                if(Mode == SLC_MODE)
                                {
                                    dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_SLC_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_SLC_PAGE_OF_BLK);
                                }
                                else
                                {
                                    dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_PAGE_OF_BLK) + 0x01000000;
                                }

                                if(n == bInValidChan)
                                {
                                    ptPageAddr->m_bDataType  = bDataType;
                                    ptPageAddr->m_bChan      = INVALID_U8;
                                    ptPageAddr->m_bClstOffs  = m;
                                    ptPageAddr->m_bLunOfChan = j;
                                    ptPageAddr->m_dRowAddr   = dRowAddr;
                                }
                                else
                                {
                                    ptPageAddr->m_bDataType  = bDataType;
                                    ptPageAddr->m_bChan      = n;
                                    ptPageAddr->m_bClstOffs  = m;
                                    ptPageAddr->m_bLunOfChan = j;
                                    ptPageAddr->m_dRowAddr   = dRowAddr;
                                }
                                ptPageAddr++;
                            }
                        }
                    }
                }
            }
            else//余数部分 //loop order Clst -> Chan -> Plan ->Chip余数部分的Loop Order待修改
            {
                for(n=0; n<bChanCnt; n++)
                {
                    if((n*bClstOfPage) == wAddrCnt%(bClstOfPage*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))//个数达到,跳出循环
                    {
                        break;//地址个数达到，结束所有循环
                    }
                    for(j=0 ; j<FDL_CHIP_OF_CHAN; j++)
                    {
                        if(((n*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN+j)*bClstOfPage) == wAddrCnt%(bClstOfPage*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))//个数达到,跳出循环
                        {
                            n = bChanCnt;//地址个数达到，结束所有循环
                            break;
                        }
                        for(k=0; k<FDL_PLANE_OF_LUN; k++)
                        {
                            if(((n*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN+j*FDL_PLANE_OF_LUN+k)*bClstOfPage) == wAddrCnt%(bClstOfPage*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))//个数达到,跳出循环
                            {
                                n = bChanCnt;//地址个数达到，结束所有循环
                                j = FDL_CHIP_OF_CHAN;
                                break;
                            }
                            for(m=0; m<bClstOfPage; m++)
                            {
                                if(Mode == SLC_MODE)
                                {
                                    dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_SLC_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_SLC_PAGE_OF_BLK);
                                }
                                else
                                {
                                    dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_PAGE_OF_BLK) + 0x01000000;
                                }

                                if(n == bInValidChan)
                                {
                                    ptPageAddr->m_bDataType  = bDataType;
                                    ptPageAddr->m_bChan      = INVALID_U8;
                                    ptPageAddr->m_bClstOffs  = m;
                                    ptPageAddr->m_bLunOfChan = j;
                                    ptPageAddr->m_dRowAddr   = dRowAddr;
                                }
                                else
                                {
                                    ptPageAddr->m_bDataType  = bDataType;
                                    ptPageAddr->m_bChan      = n;
                                    ptPageAddr->m_bClstOffs  = m;
                                    ptPageAddr->m_bLunOfChan = j;
                                    ptPageAddr->m_dRowAddr   = dRowAddr;
                                }
                                ptPageAddr++;
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    else//loop order Chan -> Plan ->Chip
    {
        for(i=0; i<wAddrCnt/(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt) + wAddrCnt%(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt); i++)
        {
            if(i < wAddrCnt/(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))
            {
                for(j=0 ; j<FDL_CHIP_OF_CHAN; j++)
                {
                    for(k=0; k<FDL_PLANE_OF_LUN; k++)
                    {
                        for(n=0; n<bChanCnt; n++)
                        {
                            if(Mode == SLC_MODE)
                            {
                                dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_SLC_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_SLC_PAGE_OF_BLK);
                            }
                            else
                            {
                                dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_PAGE_OF_BLK) + 0x01000000;
                            }

                            ptPageAddr->m_bDataType  = bDataType;
                            ptPageAddr->m_bChan      = n;
                            ptPageAddr->m_bClstOffs  = 0;
                            ptPageAddr->m_bLunOfChan = j;
                            ptPageAddr->m_dRowAddr   = dRowAddr;
                            ptPageAddr++;
                        }
                    }
                }
            }
            else//余数部分//loop order Clst -> Chan -> Plan ->Chip余数部分的Loop Order待修改
            {
                for(n=0; n<bChanCnt; n++)
                {
                    if(n == wAddrCnt%(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))//个数达到,跳出循环
                    {
                        break;
                    }
                    for(j=0 ; j<FDL_CHIP_OF_CHAN; j++)
                    {
                        if((n*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN+j) == wAddrCnt%(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))//个数达到,跳出循环
                        {
                            n = bChanCnt;
                            break;
                        }
                        for(k=0; k<FDL_PLANE_OF_LUN; k++)
                        {
                            if((n*FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN+j*FDL_PLANE_OF_LUN+k) == wAddrCnt%(FDL_CHIP_OF_CHAN*FDL_PLANE_OF_LUN*bChanCnt))//个数达到,跳出循环
                            {
                                n = bChanCnt;
                                j = FDL_CHIP_OF_CHAN;
                                break;
                            }
                            if(Mode == SLC_MODE)
                            {
                                dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_SLC_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_SLC_PAGE_OF_BLK);
                            }
                            else
                            {
                                dRowAddr = (wBlkStartAddr + ((i + dPageOffset)/FDL_PAGE_OF_BLK)*FDL_PLANE_OF_LUN + k)*FDL_ROW_OF_BLK +((i + dPageOffset)%FDL_PAGE_OF_BLK) + 0x01000000;
                            }

                            ptPageAddr->m_bDataType  = bDataType;
                            ptPageAddr->m_bChan      = n;
                            ptPageAddr->m_bClstOffs  = 0;
                            ptPageAddr->m_bLunOfChan = j;
                            ptPageAddr->m_dRowAddr   = dRowAddr;
                            ptPageAddr++;
                        }
                    }
                }
                break;
            }
        }
    }
}


/*
void FTL_CreateNandAddrErase(U08 bChanCnt,U16 wBlkStartAddr,U16 wBlkCnt)
{
    U32 i;
    U08 j = 0;//bChan = 0;
    U08 m = 0;//bLun = 0;

    T_FLASH_PHY_BLK_ADDR *tBlockAddr = (T_FLASH_PHY_BLK_ADDR*)MEM_NAND_ADDR_BUFF_ADDR;

    for (i=0; i < wBlkCnt/(bChanCnt*FDL_CHIP_OF_CHAN) + wBlkCnt%(bChanCnt*FDL_CHIP_OF_CHAN); i++)
    {
        if(i < wBlkCnt/(bChanCnt*FDL_CHIP_OF_CHAN))//整数部分
        {
            for (j = 0; j < bChanCnt; j++)
            {
                for(m=0; m < FDL_CHIP_OF_CHAN; m++)
                {
                    tBlockAddr[m+(i*bChanCnt+j)*FDL_CHIP_OF_CHAN].m_bChan		= j;
                    tBlockAddr[m+(i*bChanCnt+j)*FDL_CHIP_OF_CHAN].m_bLunOfChan	= m;
                    tBlockAddr[m+(i*bChanCnt+j)*FDL_CHIP_OF_CHAN].m_wBlkAddr	= wBlkStartAddr + i;
                }
            }
        }
        else//余数部分
        {
            for (j = 0; j < bChanCnt; j++)
            {
                if(j == wBlkCnt%(bChanCnt*FDL_CHIP_OF_CHAN))//个数达到,跳出循环
                {
                    break;
                }
                for(m=0; m < FDL_CHIP_OF_CHAN; m++)
                {
                    if((j*FDL_CHIP_OF_CHAN + m) == wBlkCnt%(FDL_CHIP_OF_CHAN*bChanCnt))//个数达到,跳出循环
                    {
                        j = bChanCnt;
                        break;
                    }
                    tBlockAddr[m+(i*bChanCnt+j)*FDL_CHIP_OF_CHAN].m_bChan		= j;
                    tBlockAddr[m+(i*bChanCnt+j)*FDL_CHIP_OF_CHAN].m_bLunOfChan	= m;
                    tBlockAddr[m+(i*bChanCnt+j)*FDL_CHIP_OF_CHAN].m_wBlkAddr	= wBlkStartAddr + i;
                }
            }
            break;
        }
    }
}
*/


// *****************************************************************************
// 函数功能: 擦除要测试的块
// 入口参数:
//
// 出口参数: 无
// 说     明:
// *****************************************************************************
/*
void FTL_EraseBlockInit(U16 wStartBlock)
{
    U16 bBlkCnt = TXBENCH_BEST_ERASECNT;
	if(bBlkCnt >= 1024)
	{
	    FDL_Log("Warning By Kinder!!! The Capacity May Be too large!!");
	} 
	else
	{
	    FDL_Log("****Erase Block:%d",bBlkCnt);
	}

    T_FLASH_PHY_BLK_ADDR *tBlockAddr = (T_FLASH_PHY_BLK_ADDR*)MEM_NAND_ADDR_BUFF_ADDR;
    FTL_CreateNandAddrErase(TEST_CHAN_MAX, wStartBlock, bBlkCnt);

#if 0//打印物理地址
    U32 wOffset  = 0;
    FDL_Log("pAddr000:| Chan | LunOfChan | BlkAddr	|");
    for(wOffset = 0; wOffset < bBlkCnt; wOffset++)
    {
        FDL_Log("pAddr%03d:|..%02d..|.....%02d....|.0x%06X.|",wOffset,\
                (tBlockAddr + wOffset)->m_bChan,\
                (tBlockAddr + wOffset)->m_bLunOfChan,\
                (tBlockAddr + wOffset)->m_wBlkAddr);
    }
#endif


    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;

    FDL_EraseBlocks(&tTaskCtrl,tBlockAddr,bBlkCnt,SLC_MODE);
    FDL_WaitTaskDone();
}

// *****************************************************************************
// 函数功能: 测试块写入数据
// 入口参数:
//
// 出口参数: 无
// 说     明:
// *****************************************************************************
void FTL_WriteBlockInit(U08 bMode,U16 wStartBlock,U16 wPageOffset,U16 WritePagesOnce)
{
    U16 wi;
    U32 dTotalSecCnt	= WritePagesOnce*32;//8192*4;//写1024个页，读测试只1024个簇
    U16 wPageCnt		= TXBENCH_BEST_PLANECNT;//MEM_DMA_BUF_SEC / FDL_SECT_OF_PAGE;//一次最多能写几个页
    U16 wAddrCnt		= dTotalSecCnt/FDL_SECT_OF_PAGE;//一共要写几页
    U08 bDir			= 0;//Write
    U08 bChanCnt		= TEST_CHAN_MAX;
    U16 wBlkStartAddr	= wStartBlock;
    U08 bInValidChan	= 0xFF;
    U08 bDataType		= 0;
    T_FLASH_PHY_ADDR *tPageAddr = (T_FLASH_PHY_ADDR *)MEM_JCTABLE_BUFF_ADDR;

    FTL_CreateNandAddrWirtRead(tPageAddr,wAddrCnt,bDir,bChanCnt,wBlkStartAddr,wPageOffset,bInValidChan,bDataType,bMode);
//    Fdb_PrintfPhyAddress(tPageAddr,wAddrCnt);

	U08MemSet((U8 *)MEM_DMA_BUFF_ADDR, 0x55, MEM_DMA_BUFF_SIZE);

    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
    tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
    tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;

    for(wi =0; wi<(wAddrCnt/wPageCnt); wi++)//分多次写完
    {
#if 0//将每个Sector的数据设置为PhyAddr信息//
		U08 n = 0,k = 0;
		for(k = 0; k < wPageCnt; k++)
		{
			for(n = 0; n < 32; n++)
			{
				U32 dColAddr = (k*32 + n)*SECTOR_SIZE;
				*(U08 *)(MEM_DMA_BUFF_ADDR + dColAddr) = n; 	
				
				dColAddr += sizeof(n);
				*(U08 *)(MEM_DMA_BUFF_ADDR + dColAddr) = (tPageAddr + wi*wPageCnt + k)->m_bDataType;
				
				dColAddr += sizeof((tPageAddr + k)->m_bDataType);
				*(U08 *)(MEM_DMA_BUFF_ADDR + dColAddr) = (tPageAddr + wi*wPageCnt + k)->m_bChan;
				
				dColAddr += sizeof((tPageAddr + k)->m_bChan);
				*(U08 *)(MEM_DMA_BUFF_ADDR + dColAddr) = (tPageAddr + wi*wPageCnt + k)->m_bLunOfChan;
				
				dColAddr += sizeof((tPageAddr + k)->m_bLunOfChan);
				*(U08 *)(MEM_DMA_BUFF_ADDR + dColAddr) = (tPageAddr + wi*wPageCnt + k)->m_bClstOffs;
				
				dColAddr += sizeof((tPageAddr + k)->m_bClstOffs) + 3;
				*(U32 *)(MEM_DMA_BUFF_ADDR + dColAddr) = (tPageAddr + wi*wPageCnt + k)->m_dRowAddr;
			}	
		}
#endif
        FDL_WriteBuffToFlash(&tTaskCtrl,MEM_DMA_BUFF_ADDR,tPageAddr,wPageCnt,MEM_UD_BUFF_ADDR,NULL);
        FDL_WaitTaskDone();
        FDL_CheckMultiLunReady();
        tPageAddr += wPageCnt;
    }
	
	FDL_Log("FTL_WriteBlockInit:%d-%d",wPageOffset*TXBENCH_BEST_PLANECNT/WritePagesOnce,wi);

}
*/

// *****************************************************************************
// 函数功能: 检验写入的块数据
// 入口参数:
//
// 出口参数: 无
// 说     明:
// *****************************************************************************
/*
void FTL_ReadBlockInit(U08 bMode,U16 wStartBlock)
{

    U16 wi;
    U32 dTotalSecCnt	= 8192;//要读1024个簇
    U16 wAddrCnt		= dTotalSecCnt/SECT_OF_DATA_CLST;//
    U08 bDir			= 1;//read
    U08 bChanCnt		= TEST_CHAN_MAX;
    U16 wBlkStartAddr	= wStartBlock;
    U16 wPageOffset 	= 0;
    U08 bInValidChan	= 0xFF;
    U08 bDataType		= 0;
    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
    FTL_CreateNandAddrWirtRead(ptClstAddr,wAddrCnt,bDir,bChanCnt,wBlkStartAddr,wPageOffset,bInValidChan,bDataType,bMode);

    U32 dClstTotal = wAddrCnt;//要读1024个簇
    U16 wClstCnt = MEM_DMA_BUF_SEC/SEC_OF_CLST;//一次最多能读多少个簇

//    Fdb_PrintfPhyAddress(ptClstAddr,wAddrCnt);

    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
    tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

    for(wi =0; wi<(dClstTotal/wClstCnt); wi++)//分多次写完
    {
        FDL_ReadClstToBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR,ptClstAddr,wClstCnt,(MEM_UD_BUFF_ADDR),NULL);
        FDL_WaitTaskDone();
        ptClstAddr += wClstCnt;
    }
	
	FDL_Log("ReadBlockInit");
}
*/
/*
void FTL_ReadRandClustInit(U08 bMode,U16 wStartBlock)
{
    U32 dTotalSecCnt	= 8192;//要读1024个簇
    U16 wAddrCnt		= dTotalSecCnt/SECT_OF_DATA_CLST;//
    U08 bDir			= 1;//read
    U08 bChanCnt		= TEST_CHAN_MAX;
    U16 wBlkStartAddr	= wStartBlock;
    U16 wPageOffset 	= 0;
    U08 bInValidChan	= 0xFF;
    U08 bDataType		= 0;
//    T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_NAND_ADDR_BUFF_ADDR;
	T_FLASH_PHY_ADDR *ptClstAddr = (T_FLASH_PHY_ADDR *)MEM_TXBENCH_W_BUFF;
	FTL_CreateNandAddrRandom(ptClstAddr,wAddrCnt,bDir,bChanCnt,wBlkStartAddr,wPageOffset,bInValidChan,bDataType,bMode);

#if 0
    U32 dClstTotal = wAddrCnt;//要读1024个簇
    U16 wClstCnt = MEM_DMA_BUF_SEC/SEC_OF_CLST;//一次最多能读多少个簇

//    Fdb_PrintfPhyAddress(ptClstAddr,wAddrCnt);

    FDL_TASK_CTRL tTaskCtrl= {0};
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
    tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;

    for(wi =0; wi<(dClstTotal/wClstCnt); wi++)//分多次写完
    {
        FDL_ReadClstToBuff(&tTaskCtrl,MEM_DMA_BUFF_ADDR,ptClstAddr,wClstCnt,(MEM_UD_BUFF_ADDR),NULL);
        FDL_WaitTaskDone();
        ptClstAddr += wClstCnt;
    }
#endif
	
	FDL_Log("FTL_ReadRandClustInit");
}
*/
void FTL_ReadRetryFail(U32 dIdxOffst)
{
	//FDL_Log("FTL_ReadRetryFail---dIdxOffst=0x%X",dIdxOffst);
}

void FTL_WriteFail(U32 dIdxOffst)
{
	FDL_Log("FTL_WriteFail---dIdxOffst=0x%X",dIdxOffst);
}

