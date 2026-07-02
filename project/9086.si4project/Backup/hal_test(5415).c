#include "common.h"
#include "data_type.h"
#include "Hal_test.h"
#include "main.h"
#include "fdl_debug.h"
#include "pri_cmd.h"
#include "fdl_task_op.h"
#include "fdl_test.h"
#include "hal_cq.h"
#include "uitility.h"
#include "fdl_common.h"

#include "flash_driver_struct.h"

#define HAL_TEST_ERR   (0)
#define HAL_TEST_OK    (1)


////flash analyse group added
#define FA_RS_BUFFADD 				MEM_COPYBACK_SRC_ADDR //Buffer for Read Status:  bit0-Pass bit1-Fail
#define FA_RS_BUFFARY 				((U8*)FA_RS_BUFFADD)
#define FA_RS_BUFFSIZE 				262144	//262144bit = 32768Byte = 64Sector = 32KB
#define FA_WARE_TLCWAREDATA_ADD		(MEM_DMA_BUFF_ADDR + 131072) //TLC Standard Ware Data Buff 54K
#define FA_WARE_TLCWAREDATA_ARY		((U8*)(FA_WARE_TLCWAREDATA_ADD)) //TLC Standard Ware Data Buff 54K
#define FA_WARE_MLCWAREDATA_ADD		(MEM_DMA_BUFF_ADDR + 186368)  //MLC Standard Ware Data Buff 32K
#define FA_WARE_SLCWAREDATA_ADD		(MEM_DMA_BUFF_ADDR + 219136)  //SLC Standard Ware Data Buff 2K
#define FA_WARE_LnWAREDATA_ADD		(MEM_DMA_BUFF_ADDR + 221184)  //Ln Ware Data Buff 2K
#define FA_WARE_PAGEINFO_ADD 		(MEM_DMA_BUFF_ADDR + 229376) //Page Info Buff 30K
#define FA_WARE_PAGEINFO_ARRAY		((U16*)FA_WARE_PAGEINFO_ADD)	
#define FA_TEMP_BUFFADD				(MEM_DMA_BUFF_ADDR + 260096)	//Temp Buff 2K
#define FA_CUR_TEMP_ADD				((U16*)FA_TEMP_BUFFADD)
#define FA_RB_RECORD_BUFF_ADD       MEM_CB_RETRY_INFOR_ADDR
#define FA_RB_RECORD_ARRAY          ((U16 *)FA_RB_RECORD_BUFF_ADD)
#define FA_FLASH_INFO_BUFF			((U8*)MEM_DMA_BUFF_ADDR)
#define FA_Offline_Block			((U8*)MEM_NAND_ADDR_BUFF_ADDR)
#define FA_Offline_Status			((U32*)MEM_DMA_BUFF_ADDR)
U32		FaRsIndx;							//Current Idx for RS Buffer


volatile U8	SysCallCounter = 0;				//interupt counter
volatile U8 IsHTControlling = 0;			//flag is 1 to start temperature controlling process
volatile U8	IsHeating = 0;					//flag is 1 when raising temperature, 0 when lowering,2 when balancing
volatile U16 HTCtrlCnt = 0;					//counter used in linear period
volatile U8	IsHeaterOn = 0;					//flag is 1 to open heater till next interupt, 0 to close
volatile U8 IsFanOn = 0;					//flag is 1 to open fan till next interupt
U16 LinearPeriod = 5000;				//Raise TargetTemperature linearly to prevent overheating
U16 SlowStartThreshold = 7680;			//For TargetTemperature lower than SlowStartThreshold, open fan while heating to prevent overheating
volatile U16 CurTempSensor1 = 0;
volatile U16 CurTempSensor2 = 0;
volatile U8 IsUpdating = 0;
volatile U8 IsManualHeating = 0;
PidParameters pid = {
	0,	//TargetTemperature
	0,	//LastTemperature
	0,	//CurrentTemperature
	0,	//PreviousTemperature
	0,	//StartTemperature		
	10,			//dt 10ms temperature readout period/heater control policy update period
};
FLASH_INFO NANDFlash;

U16 ProgramMap[80];

U16 Offline_BlockList[100];
U16 Offline_OperationCntList[16];
//U16 Offline_ReadtimeList[30];
U32 Offline_PE_counter = 0;
U8 Offline_PE_Stage_counter = 0;
U8 Offline_Program_Type = 0;	//0:Normal WriteBlock 1:CopyBack WriteBlock 2:Erase Only
U8 Offline_Chan = 0;
U8 Offline_Chip = 0;
U8 Offline_OperationMode = 0;
U8 Offline_PlaneCnt = 0;
U8 Offline_OperationStage = 0;
U8 Offline_Program_StartSeed = 0;
U8 Offline_RBMode = 0;
U8 Offline_RSRecord = 0;
U16 Offline_RSRecBlkList[5];
U8 Offline_RSBlkCnt = 0;
U16 Offline_RSRecIndex = 0;
U8 Offline_RSBlkIndex = 0;
extern U8 flag_PE_offline;
extern U8 flag_ReadDisturb_offline;


U8 Bit_Selecter[8]=
{
	0x7F,
	0xBF,
	0xDF,
	0xEF,
	0xF7,
	0xFB,
	0xFD,
	0xFE
};

///////////////////////////////////////////////////////////////////////////////

U8 HAL_Test_GoCacheWR(T_GOCACHE_PARM *ptGocacheParm)
{

    return GFWR_SUCCESS;
}

void HAL_Test_GoCachePortWriteRead(U8 bChan,U8 bCe)
{



}

void HAL_Test_GoCacheBurstWriteRead(U8 bChan,U8 bCe,U16 bBurstSize)
{




}

void HAL_CMD_E1_GoCachePortWriteRead(void)
{
	HostCmdResponse();
	FDL_Log("E1 GoCache_PortWriteRead_Cq:\r\n");

	U08 bChan = 0;
	U08 bCe = 3;
	HAL_Test_GoCachePortWriteRead(bChan,bCe);

	FDL_Log("E1 Over");
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}

void HAL_CMD_E3_GoCacheBurstWriteRead(void)
{
	HostCmdResponse();
	FDL_Log("E3 GoCache_BurstWriteRead_Cq:\r\n");

	U08 bChan = 0;
	U08 bCe = 3;
	U08 bBurstSize = 0x10;

	HAL_Test_GoCacheBurstWriteRead(bChan,bCe,bBurstSize);

	FDL_Log("E3 Over");
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);//send 1 sector status information to host
}

void HAL_CMD_E4_HostDataTransfer(void)
{
	//FDL_Log("E4 wOpSctCnt=%x,dir:%d\r\n",tPrivCMD.wOpSctCnt, tPrivCMD.bDir);
    if (1 == tPrivCMD.bDir)//HOST_RD
    {
    	HostCmdResponse();
    }

    HAL_HostDataTransfer(MEM_DMA_BUFF_ADDR, tPrivCMD.wOpSctCnt, tPrivCMD.bDir);
}

//为E5命令流程封装的接口
void E5_GocacheWriteRead(T_HAL_GOCACHE_FLASH tHalGocacheWithFlash, U8 bDir)
{
	if(PORT_TEST == tHalGocacheWithFlash.bPortOrBurst)
	{
		U32 dLen = tHalGocacheWithFlash.dGocacheLen;
		U32 dDataAddr = GOCACHE_DATA_BUFF_ADDR;
		U32 dGocacheAddr = tHalGocacheWithFlash.dGocacheAddr;
		while(dLen)
		{
		    //HAL_GoCachePortWR(3, 3, bDir, *((U32 *)dDataAddr), dGocacheAddr);
		    dLen -= sizeof(U32);
		    dDataAddr += sizeof(U32);
		    dGocacheAddr += sizeof(U32);
		}
	}
	if(BURST_TEST == tHalGocacheWithFlash.bPortOrBurst)
	{
		//HAL_GoCacheBurstWR(3, 3, bDir, tHalGocacheWithFlash.dGocacheLen, tHalGocacheWithFlash.dGocacheAddr, GOCACHE_DATA_BUFF_ADDR);
	}
}

void E5_FlashEraseWriteRead(T_HAL_GOCACHE_FLASH tHalGocacheWithFlash)
{
	U16 * pwSeedIdxBuff = NULL;
    U8 i;

	FDL_TASK_CTRL tTaskCtrl = { 0 };
	tTaskCtrl.dEraseExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
	tTaskCtrl.wEraseExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	for(i = 0; i < tHalGocacheWithFlash.bChanCnt; i++)  //判断单通道还是多通道
	{
		if(tHalGocacheWithFlash.bChanCnt != 1)
		{
            tHalGocacheWithFlash.tBlockAddr.m_bChan = i;
            tHalGocacheWithFlash.tPageAddr.m_bChan = i;
		}
	    FDL_EraseBlocks(&tTaskCtrl, &tHalGocacheWithFlash.tBlockAddr, tHalGocacheWithFlash.dBlockCnt, SLC_MODE);
	}
	FDL_WaitTaskDone();
	FDL_Test_Analyse_Exception(tTaskCtrl.dEraseExBuffAddr, ERASE_TYPE);

	//写
    tTaskCtrl.dWriteExBuffAddr = MEM_WRITE_EXCEP_BUFF_ADDR;
    tTaskCtrl.wWriteExBuffSize = MEM_WRITE_EXCEP_BUFF_SIZE;
	tTaskCtrl.bWriteFailCtrl = WRITE_FAIL_STOP;
	for(i = 0; i < tHalGocacheWithFlash.bChanCnt; i++)
	{
		if(tHalGocacheWithFlash.bChanCnt != 1)
		{
            tHalGocacheWithFlash.tBlockAddr.m_bChan = i;
            tHalGocacheWithFlash.tPageAddr.m_bChan = i;
		}
		if(tHalGocacheWithFlash.bJointEn)
		{
		   	FDL_JointWritePages(&tTaskCtrl, FLASH_DATA_BUFF_ADDR, 0, tHalGocacheWithFlash.dPageCnt * 32, &tHalGocacheWithFlash.tPageAddr, tHalGocacheWithFlash.dPageCnt, USRDATA_BUFF_ADDR, pwSeedIdxBuff);
		}
	    else
	    {
	    	FDL_WriteBuffToFlash(&tTaskCtrl,FLASH_DATA_BUFF_ADDR,&tHalGocacheWithFlash.tPageAddr, tHalGocacheWithFlash.dPageCnt,USRDATA_BUFF_ADDR, pwSeedIdxBuff);
	    }
	}
	FDL_WaitTaskDone();
	FDL_Test_Analyse_Exception(tTaskCtrl.dWriteExBuffAddr, WRITE_TYPE);

	//读
	tTaskCtrl.bReadFailCtrl = READ_FAIL_ALL_RETRY;
    tTaskCtrl.dReadExBuffAddr = MEM_READ_EXCEP_BUFF_ADDR;
    tTaskCtrl.wReadExBuffSize = MEM_READ_EXCEP_BUFF_SIZE;
	for(i = 0; i < tHalGocacheWithFlash.bChanCnt; i++)
	{
		if(tHalGocacheWithFlash.bChanCnt != 1)
		{
            tHalGocacheWithFlash.tBlockAddr.m_bChan = i;
            tHalGocacheWithFlash.tPageAddr.m_bChan = i;
		}
	    if(tHalGocacheWithFlash.bJointEn)
	    {
	        FDL_JointReadClsts(&tTaskCtrl,FLASH_DATA_BUFF_ADDR,&tHalGocacheWithFlash.tPageAddr,0,tHalGocacheWithFlash.dPageCnt * 32,tHalGocacheWithFlash.dPageCnt * DATA_CLST_OF_PAGE,(USRDATA_BUFF_ADDR + 512),pwSeedIdxBuff);
	    }
	    else
	    {
	        FDL_ReadClstToBuff(&tTaskCtrl, FLASH_DATA_BUFF_ADDR, &tHalGocacheWithFlash.tPageAddr, tHalGocacheWithFlash.dPageCnt, USRDATA_BUFF_ADDR, pwSeedIdxBuff);
	    }
	}
	FDL_WaitTaskDone();
	FDL_Test_Analyse_Exception(tTaskCtrl.dReadExBuffAddr, READ_TYPE);
}

void HAL_CMD_E5_GocacheWithFlash(void)
{
	HostCmdResponse();
//	SG_ADM->rBUF_SIZE.bits.wVAL = 512;


    //擦交叉、联动/非联动、多通道同时/单通道

	//写交叉


    //读交叉

}

void HAL_Test_WriteOnePage(U8 bPrgMod,U8 bChan,U8 bChip,U8 bLunOfChip,U8 bCmdTag,U32 dBaseRow,U32 dRowInBlk,U32 dBuffAddr,U32 dUdAddr,U8 bAttchSize,U8 bUdLen)
{
	U32 dRndSeed = dBaseRow + dRowInBlk + bCmdTag;
	HAL_CQChipSelect(bChan,bChip);
	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
	HAL_CQSetRndSeed(bChan,dRndSeed,INVALID_U16);
	HAL_CQSetUd(bChan, dUdAddr, bAttchSize ,bUdLen,DMA2_WR_TO_FLASH);

	#if (FDL_TEST_FLASH == H27QEG8NDB)
	if(bPrgMod == TLC_MODE)
	{
		HAL_CQSetRndSeed(bChan,dBaseRow+(dRowInBlk&0xFFFFFF)*3+(dRowInBlk >> 24)-1,INVALID_U16);
		if(bCmdTag == 0)
		{
			HAL_CQSendFlashCmd(bChan, 0x09);
			HAL_CQSendFlashCmd(bChan, (dRowInBlk >> 24));
		}
		else if(bCmdTag == 1)
		{
			HAL_CQSendFlashCmd(bChan, 0x0D);
			HAL_CQSendFlashCmd(bChan, (dRowInBlk >> 24));
		}
		else
		{
			HAL_CQSendFlashCmd(bChan, (dRowInBlk >> 24));
		}

	}
	else
	{
		if(bCmdTag)
		{
			HAL_CQSendFlashCmd(bChan, bCmdTag);
		}
	}
	#endif

#if (FDL_TEST_FLASH == MIC_B0KB)
	if(bPrgMod == SLC_MODE)
	{
		if(bCmdTag)
		{
			HAL_CQSendFlashCmd(bChan, bCmdTag);
		}
	}
#endif

#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)|| (NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)||(NAND_FLASH == YMTC_B08T))
		if(bPrgMod == SLC_MODE)
		{
			HAL_CQSendFlashCmd(bChan, 0xDA);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan, 0xDF);
		}
#endif

#if (FDL_TEST_FLASH == K9ADGD8U0C)
	if(bPrgMod == SLC_MODE)
	{
		if(bCmdTag)
		{
			HAL_CQSendFlashCmd(bChan, bCmdTag);
		}
	}
#endif

#if (FDL_TEST_FLASH == TOS_BICS3)
	if (bPrgMod == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan, bCmdTag);
	}
	else if (bPrgMod == SLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan, 0xA2);
	}
#endif

#if (FDL_TEST_FLASH == HYNIX_3DV4)
	if (bPrgMod == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan, bCmdTag);
	}
	else if (bPrgMod == SLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan, 0xA2);
	}
#endif

	U32 dRow = dBaseRow + dRowInBlk;
	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	HAL_CQSend5Addr(bChan, 0, dRow);
	HAL_CQStartDMA2(bChan, dBuffAddr, DMA2_WR_TO_FLASH, 0, 0,SECT_OF_PAGE,JOINT_NONE, MEM_DMA2_INFO_ADDR);

	if(bPrgMod == TLC_MODE)
	{
		#if (FDL_TEST_FLASH == H27QEG8NDB)
		if(((dRowInBlk >> 24) < 3)&&((dRowInBlk >> 24) != 0))
		{
			HAL_CQSendFlashCmd(bChan, 0x1a);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
		}
		#endif

		#if  (FDL_TEST_FLASH == MIC_B0KB)
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
		#endif
		#if  ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)|| (NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)||(NAND_FLASH == YMTC_B08T))
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
		#endif

		#if  (FDL_TEST_FLASH == MIC_L06B)
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
		#endif

		#if (FDL_TEST_FLASH == K9ADGD8U0C)
		if(bPrgMod == TLC_MODE)
		{
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, (0x11 + (dRowInBlk%3)));
			U32 dCMD;
			if((dRowInBlk%3) == 2)
			{
				HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
				HAL_CQSendFlashCmd(bChan, 0xdf);
				HAL_CQSendFlashCmd(bChan, 0x8B);
				HAL_CQSend5Addr(bChan, 0, (dBaseRow+(dRowInBlk / 3)));
				//HAL_CQSendAddr(bChan,(dRowInBlk / 3));
				HAL_CQSendFlashCmd(bChan, 0x10);
			}
		}
		#endif
		#if (FDL_TEST_FLASH == K9ADGD8U0A)
		if(bPrgMod == TLC_MODE)
		{
			HAL_CQSendFlashCmd(bChan, 0xC0);
			HAL_CQSendAddr(bChan, (0x31 + (dRowInBlk%2)));
			U32 dCMD;
			if((dRowInBlk%2) == 1)
			{
				HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);
				HAL_CQSendFlashCmd(bChan, 0xdf);
				HAL_CQSendFlashCmd(bChan, 0x8B);
				HAL_CQSend5Addr(bChan, 0, (dBaseRow+(dRowInBlk / 2)));
				//HAL_CQSendAddr(bChan,(dRowInBlk / 3));
				HAL_CQSendFlashCmd(bChan, 0x10);
			}
		}
		#endif

		#if (FDL_TEST_FLASH == TOS_BICS3)
		if (bCmdTag != 3)
		{
			HAL_CQSendFlashCmd(bChan, 0x1A);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
		}
		#endif

		#if (FDL_TEST_FLASH == HYNIX_3DV4)
		if (bCmdTag != 3)
		{
			HAL_CQSendFlashCmd(bChan, 0x22);
		}
		else
		{
			HAL_CQSendFlashCmd(bChan, 0x23);//FL_CMD_PAGE_PROG2
		}
		#endif
	}
	else
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2);
	}
}

void HAL_Test_ReadOnePage(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff)
{
    U32 dRowAddr;
    U16 wColAddr = DATA_COL_ADDR(0);

	U08 bCmdTag = dRowInBlk%3+1;
	U32 dRndSeed = dRowInBlk + bCmdTag;
	//HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);
    HAL_CQSetRndSeed(bChan,dRndSeed,INVALID_U16);

	if(bMode == TLC_MODE)
	{
		#if (FDL_TEST_FLASH == H27QEG8NDB)
        //HAL_CQSetRndSeed(bChan,dRowInBlk/3,INVALID_U16);
		dRowAddr = dRowInBlk/3 + dBaseRow;
		#elif (FDL_TEST_FLASH == TOS_BICS3)
		dRowAddr = (dRowInBlk/3)*3 + dBaseRow;
		#elif (FDL_TEST_FLASH == HYNIX_3DV4)
		dRowAddr = (dRowInBlk/3)*3 + dBaseRow;
		#else
		dRowAddr = dRowInBlk + dBaseRow;
		#endif
	}
	else
	{
        //HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);
		dRowAddr = dRowInBlk + dBaseRow;
	}
    HAL_CQSetUd(bChan,dUserBuff,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,1);

    HAL_CQChipSelect(bChan,bChip);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_WRITE, FIO_PF_PASS_TLC_WRITE);

#if (FDL_TEST_FLASH == H27QEG8NDB)
	U8 bPreCmd = dRowInBlk%3+1;
	if(bMode == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan,bPreCmd);
	}
	else
	{
		if(pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.bCmdFlg)
		{
			HAL_CQSendFlashCmd(bChan,pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.dCmdCQ[0]);
		}
	}
#endif

#if (FDL_TEST_FLASH == MIC_B0KB)
	if(bMode == SLC_MODE)
	{
		if(pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.bCmdFlg)
		{
			HAL_CQSendFlashCmd(bChan,pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.dCmdCQ[0]);
		}
	}
#endif

#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)|| (NAND_FLASH == MIC_B27B)||(NAND_FLASH == INTEL_B27B)||(NAND_FLASH == YMTC_B08T))
		if(bMode == SLC_MODE)
		{
			if(pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.bCmdFlg)
			{
				HAL_CQSendFlashCmd(bChan,pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.dCmdCQ[0]);
			}
		}
#endif

#if (FDL_TEST_FLASH == K9ADGD8U0C)
		if(bMode == SLC_MODE)
		{
			if(pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.bCmdFlg)
			{
				HAL_CQSendFlashCmd(bChan,pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevRdCmd.dCmdCQ[0]);
			}
		}
#endif

#if (FDL_TEST_FLASH == TOS_BICS3)
	U8 bPreCmd = dRowInBlk%3+1;
	if (bMode == SLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan,0xA2);
	}
	else if (bMode == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan,bPreCmd);//LSB Page 0x01/CSB Page 0x02/MSB Page 0x03
	}
#endif

#if (FDL_TEST_FLASH == HYNIX_3DV4)
		U8 bPreCmd = dRowInBlk%3+1;
		if (bMode == SLC_MODE)
		{
			HAL_CQSendFlashCmd(bChan,0xA2);
		}
		else if (bMode == TLC_MODE)
		{
			HAL_CQSendFlashCmd(bChan,bPreCmd);//LSB Page 0x01/CSB Page 0x02/MSB Page 0x03
		}
#endif

    HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
    HAL_CQSend5Addr(bChan,wColAddr,dRowAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
	HAL_CQSendRndAddr(bChan,wColAddr,dRowAddr);

	HAL_CQStartDMA2(bChan,dDataBuff, DMA2_RD_TO_BUFF, 0,0, SECT_OF_PAGE, 0,MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	while(HAL_CQGetIdleSpace(bChan) < 0x90);
}

void HAL_Test_ErsBlks(U8 bErsMod,U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd)
{
	U08 bChan;
	U08 bChip;
    U08 bLunOfChip;
	U08 bPreCmd = 0;

    U32 dRowAddr;

    if (bErsMod == SLC_MODE)
    {
		if (pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevErsCmd.bCmdFlg)
		{
			bPreCmd = pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevErsCmd.dCmdCQ[0];
		}
	}
	else
	{
		#if (NAND_FLASH == TOS_BICS3 || NAND_FLASH == HYNIX_3DV4)
			bPreCmd = 0x00;
		#else
			bPreCmd = 0xDF;
		#endif
	}

	U16 i,j,k;
	for (k = wBlockSt; k <= wBlockEd; k++)
	{
		for (j = bLunSt; j <= bLunEd; j++)
		{
			for (i = bChanSt; i <= bChanEd; i++ )
			{
				bChip		= j / LUN_OF_CHIP;
				bLunOfChip	= j % LUN_OF_CHIP;
				dRowAddr	= k * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;

				bChan = CHANS(i);
				bChip = CHIPES(bChan,bChip);

				HAL_CQChipSelect(bChan,bChip);

				if(bPreCmd)
				{
					HAL_CQSendFlashCmd(bChan,bPreCmd);
				}

				HAL_NFEraseBlock(bChan, bChip, dRowAddr);
				HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_ERASE, FIO_PF_PASS_TLC_ERASE);
			}
			for (i = bChanSt; i <=bChanEd; i++ )
			{
				bChan = CHANS(i);
				HAL_CQWaitDone(bChan);
				if (HAL_NFGetOrClrChkStsHwFlag(bChan,GET_CHKSTS_FLAG))
				{
					FDL_Log("Erase Fail %x", k);
					HAL_NFGetOrClrChkStsHwFlag(bChan,CLR_CHKSTS_FLAG);

					SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;
					SG_FC(bChan)->rCQ_HOLD_CTRL.bits.DMA2_ABNL_HOLD_CQ_FLAG =  0;//clear hold en
					SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_FLAG = 0;//clear int infor
					SG_DMA2_CH(bChan)->rSTATUS.bits.SVA_OVF_FLAG =0;
				}

			}
		}
	}
}

void HAL_Test_PrgSlcBlks(U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd,U16 wPagSt,U16 wPagCnt,U8 bRndEn)
{
	U08 bChan;
	U08 bChip;
    U08 bLunOfChip;
	U08 bPreCmd = 0;
	//U08 bInitBt = 4;

	if(wPagSt+wPagCnt > ROW_OF_BLOCK)
	{
		return;
	}

	U16 i,j,k,m,t;
	if(bRndEn == 0)
	{
		for(i = bChanSt; i <= bChanEd; i++)
		{
			SG_DMA2_CH(i)->rRND_CTRL.bits.RND_BYPASS = 1;
		}
	}

	if (pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevWrCmd.bCmdFlg)
	{
		bPreCmd = pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevWrCmd.dCmdCQ[0];
	}

	for(i = 0; i < MEM_UD_BUFF_SIZE; i++)
	{
		MemorySet((U8 *)(MEM_UD_BUFF_ADDR+UD_LEN_OF_DATA_CLST*i),0x11+i,UD_LEN_OF_DATA_CLST);
	}

	U08 bData = 0;
	for (bData = 0; bData < FDL_SECT_OF_PAGE; bData++)
	{
		MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+bData*SECTOR_SIZE),bData, SECTOR_SIZE);
	}

	for(m = wPagSt; m < wPagSt+wPagCnt; m++)
	{
		for (k = wBlockSt; k <= wBlockEd; k++)
		{
			for (j = bLunSt; j <= bLunEd; j++)
			{
				for (i = bChanSt; i <= bChanEd; i++ )
				{
					bChip		= j / LUN_OF_CHIP;
					bLunOfChip	= j % LUN_OF_CHIP;

					bChan = CHANS(i);
					bChip = CHIPES(bChan,bChip);

					U32 dBaseRow = k*ROW_OF_BLOCK+ROW_GAP_OF_LUN*bLunOfChip;
				#if 0
					for(n = 0;n < DATA_CLST_OF_PAGE;n++)
					{
						for(t = 0;t < SECT_OF_DATA_CLST/SECT_OF_DMA;t++)
						{
							U32 dColAddr = n*4096+t*2048;
							MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChan, bInitBt);
							dColAddr+=bInitBt;
							MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChip, bInitBt);
							dColAddr+=bInitBt;
							Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dBaseRow+m,bInitBt/4);
							dColAddr += bInitBt;
							MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
						}
					}
				#endif
					HAL_Test_WriteOnePage(SLC_MODE,bChan,bChip,bLunOfChip,bPreCmd,dBaseRow,m,MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
				}
				for (t = bChanSt; t <= bChanEd; t++ )
				{
					bChan = CHANS(t);
					HAL_CQWaitDone(bChan);
				}
			}
		}
	}
}


void HAL_Test_PrgSlcTableBlks(U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd,U16 wPagSt,U16 wPagCnt,U8 bRndEn)
{
	U08 bChan;
	U08 bChip;
    U08 bLunOfChip;
	U08 bPreCmd = 0;
	U08 bInitBt = 4;

	if(wPagSt+wPagCnt > ROW_OF_BLOCK)
	{
		return;
	}

	U16 i,j,k,m,n,t;
	if(bRndEn == 0)
	{
		for(i = bChanSt; i <= bChanEd; i++)
		{
			SG_DMA2_CH(i)->rRND_CTRL.bits.RND_BYPASS = 1;
		}
	}

	if (pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevWrCmd.bCmdFlg)
	{
		bPreCmd = pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevWrCmd.dCmdCQ[0];
	}

	for(i = 0; i < MEM_UD_BUFF_SIZE; i++)
	{
		MemorySet((U8 *)(MEM_UD_BUFF_ADDR+UD_LEN_OF_TBLE_CLST*i),0x11+i,UD_LEN_OF_TBLE_CLST);
	}

	MemorySet((U8 *)MEM_DMA_BUFF_ADDR,0xAA, 16*1024);

	for(m = wPagSt; m < wPagSt+wPagCnt; m++)
	{
		for (k = wBlockSt; k <= wBlockEd; k++)
		{
			for (j = bLunSt; j <= bLunEd; j++)
			{
				for (i = bChanSt; i <= bChanEd; i++ )
				{
					bChip		= j / LUN_OF_CHIP;
					bLunOfChip	= j % LUN_OF_CHIP;

					bChan = CHANS(i);
					bChip = CHIPES(bChan,bChip);

					U32 dBaseRow = k*ROW_OF_BLOCK+ROW_GAP_OF_LUN*bLunOfChip;
					for(n = 0;n < TBLE_CLST_OF_PAGE;n++)
					{
						for(t = 0;t < SECT_OF_TBLE_CLST/SECT_OF_TBLE_CLST;t++)
						{
							U32 dColAddr = n*4096+t*2048;
							MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChan, bInitBt);
							dColAddr+=bInitBt;
							MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChip, bInitBt);
							dColAddr+=bInitBt;
							Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dBaseRow+m,bInitBt/4);
							dColAddr += bInitBt;
							MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
						}
					}
					HAL_Test_WriteOnePage(SLC_MODE,bChan,bChip,bLunOfChip,bPreCmd,dBaseRow,m,MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR,SECT_OF_TBLE_CLST,UD_LEN_OF_TBLE_CLST);
				}
				for (t = bChanSt; t <= bChanEd; t++ )
				{
					bChan = CHANS(t);
					HAL_CQWaitDone(bChan);
				}
			}
		}
	}
}


U8 HAL_Test_8NDB_GetOrderPrgInfor(U16 wOrderNum,U32 *pPrgInfor)
{
	if (wOrderNum < 384)
	{
			if (wOrderNum < 3)
			{
				if(wOrderNum == 0)
				{
					pPrgInfor[0] = 0 + (1 << 24);
					pPrgInfor[1] = 0 + (2 << 24);
					pPrgInfor[2] = 0 + (3 << 24);
					pPrgInfor[3] = 0;
				}

				else if(wOrderNum == 2)
				{
					pPrgInfor[0] = 0 + (1 << 24);
					pPrgInfor[1] = 0 + (2 << 24);
					pPrgInfor[2] = 0 + (3 << 24);
					pPrgInfor[3] = 1;
				}

				else if(wOrderNum == 1)
				{
					pPrgInfor[0] = 1 + (1 << 24);
					pPrgInfor[1] = 1 + (2 << 24);
					pPrgInfor[2] = 1 + (3 << 24);
					pPrgInfor[3] = 0;
				}
			}


			else if((wOrderNum >= 3) && (wOrderNum < 381))
			{
				if((wOrderNum % 3) == 0)
				{
					pPrgInfor[0] = ((wOrderNum + 3) / 3) + (1 << 24);
					pPrgInfor[1] = ((wOrderNum + 3) / 3) + (2 << 24);
					pPrgInfor[2] = ((wOrderNum + 3) / 3) + (3 << 24);
					pPrgInfor[3] = 0;
				}
				else if((wOrderNum % 3) == 1)
				{
					pPrgInfor[0] = ((wOrderNum - 1) / 3) + (1 << 24);
					pPrgInfor[1] = ((wOrderNum - 1) / 3) + (2 << 24);
					pPrgInfor[2] = ((wOrderNum - 1) / 3) + (3 << 24);
					pPrgInfor[3] = 1;
				}
				else if((wOrderNum % 3) == 2)
				{
					pPrgInfor[0] = ((wOrderNum - 5) / 3) + (1 << 24);
					pPrgInfor[1] = ((wOrderNum - 5) / 3) + (2 << 24);
					pPrgInfor[2] = ((wOrderNum - 5) / 3) + (3 << 24);
					pPrgInfor[3] = 2;
				}
			}


			else if(wOrderNum >= 381)
			{
				if(wOrderNum == 381)
				{
					pPrgInfor[0] = 127 + (1 << 24);
					pPrgInfor[1] = 127 + (2 << 24);
					pPrgInfor[2] = 127 + (3 << 24);
					pPrgInfor[3] = 1;
				}
				else if(wOrderNum == 382)
				{
					pPrgInfor[0] = 126 + (1 << 24);
					pPrgInfor[1] = 126 + (2 << 24);
					pPrgInfor[2] = 126 + (3 << 24);
					pPrgInfor[3] = 2;
				}
				else if(wOrderNum == 383)
				{
					pPrgInfor[0] = 127 + (1 << 24);
					pPrgInfor[1] = 127 + (2 << 24);
					pPrgInfor[2] = 127 + (3 << 24);
					pPrgInfor[3] = 2;
				}
			}
			return TRUE;
	}
	else
	{
		return FALSE;
	}
}

U8 HAL_Test_U0C_GetOrderPrgInfor(U16 wOrderNum,U32 *pPrgInfor)
{
	if (wOrderNum < 128)
	{
		pPrgInfor[0] = wOrderNum*3;
		pPrgInfor[1] = (wOrderNum*3 + 1);
		pPrgInfor[2] = (wOrderNum*3 + 2);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

U8 HAL_Test_U0A_GetOrderPrgInfor(U16 wOrderNum,U32 *pPrgInfor)
{
	if (wOrderNum < 128)
	{
		pPrgInfor[0] = wOrderNum*2;
		pPrgInfor[1] = (wOrderNum*2 + 1);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

U8 HAL_Test_B0KB_GetOrderPrgInfor(U16 wOrderNum,U32 *pPrgInfor)
{
	U32 wPageNum[5] = {0,16,112,208,1504};
	U08 wOffset = 63;

	if(wOrderNum < 1024)
	{
		if(wOrderNum < 16)//page1~15
		{
			pPrgInfor[0] = 1;
			pPrgInfor[1] = wPageNum[0] + wOrderNum+(1 << 24);
		}
		else if(wOrderNum < 64)//page 16~111 1Order = 2Page  0x40  = 0x10+(112-16)/2
		{
			pPrgInfor[0] = 2;
			pPrgInfor[1] = wPageNum[1] + (wOrderNum - 16) * 2+(1 << 24);
			pPrgInfor[2] = wPageNum[1] + (wOrderNum - 16) * 2 + 1+(2 << 24);
		}
		else if(wOrderNum < 128)//page 112(49)113 114~205(111) 206 207 0x7A = 0x3A+32*2
		{
			pPrgInfor[0] = 2;
			if(0 == (wOrderNum % 2))
			{
				pPrgInfor[1] = wPageNum[2] + (wOrderNum - 64) / 2 * 3+(1 << 24);
				pPrgInfor[2] = pPrgInfor[1] - wOffset - (wOrderNum - 64) / 2+(2 << 24);
			}
			else
			{
				pPrgInfor[1] = wPageNum[2] + 1 + (wOrderNum - 65) / 2 * 3+(1 << 24);
				pPrgInfor[2] = pPrgInfor[1] + 1+(2 << 24);
			}
		}
		else if(wOrderNum < 992)//page 208(114)209 210~1501(1407) 1502 1503 0x3DA = (1504-208)/3*2+0x7A
		{
			pPrgInfor[0] = 2;
			if(0 == (wOrderNum % 2))
			{
				pPrgInfor[1] = wPageNum[3] + (wOrderNum - 128) / 2 * 3+(1 << 24);
				pPrgInfor[2] = pPrgInfor[1] - 94+(2 << 24);
			}
			else
			{
				pPrgInfor[1] = wPageNum[3] + 1 + (wOrderNum - 129) / 2 * 3+(1 << 24);
				pPrgInfor[2] = pPrgInfor[1] + 1+(2 << 24);
			}
		}
		else if(wOrderNum < 1024)//page 1504(1410)1505 ~ 1534(1455) 1535  0x7A = (1535-1505)/2*2+0x3DA
		{
			if(0 == (wOrderNum % 2))
			{
				pPrgInfor[0] = 2;
				pPrgInfor[1] = wPageNum[4] + (wOrderNum - 992)+(1 << 24);
				pPrgInfor[2] = pPrgInfor[1] - 94 + (wOrderNum - 992) / 2+(2 << 24);
			}
			else
			{
				pPrgInfor[0] = 1;
				pPrgInfor[1] = wPageNum[4] + (wOrderNum - 992)+(1 << 24);
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

U8 HAL_Test_GetOrderPrgInfor(U16 wOrderNum,U32 *pPrgInfor)
{
	U08 bContinue = 0;
#if (FDL_TEST_FLASH == H27QEG8NDB)
	bContinue =  HAL_Test_8NDB_GetOrderPrgInfor(wOrderNum,pPrgInfor);
#endif

#if (FDL_TEST_FLASH == MIC_B0KB)
	bContinue = HAL_Test_B0KB_GetOrderPrgInfor(wOrderNum,pPrgInfor);
#endif

#if (FDL_TEST_FLASH == K9ADGD8U0C)
	bContinue = HAL_Test_U0C_GetOrderPrgInfor(wOrderNum,pPrgInfor);
#endif

#if (FDL_TEST_FLASH == K9ADGD8U0A)
	bContinue = HAL_Test_U0A_GetOrderPrgInfor(wOrderNum,pPrgInfor);
#endif


return bContinue;
}

void HAL_Test_8NDB_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bChip		= bLun / LUN_OF_CHIP;
    U32 dBasRowAddr    = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bChip);

	U08 bOrderCntinue;
	U32 dOrderPrg[4];
	//U32 dColAddr;
	U32 dPage;

	U08 bInitBt = 4;
	U16 wOrderCnt = 384;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	U16 i,j,k,n;
	for (i = 0;i < wOrderCnt;i++)
	{
		bOrderCntinue = HAL_Test_GetOrderPrgInfor(i,(U32 *)(&(dOrderPrg[0])));
		if(bOrderCntinue)
		{
			for(j = 0; j < 3; j++)
			{
				dPage = dBasRowAddr+(dOrderPrg[j]&0xFFFFFF)*3+j;
				for(n = 0; n < 4; n++)
				{
					for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
					{
						U32 dColAddr = n*4096+k*2048;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChan, bInitBt);
						dColAddr+=bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLunOfChip, bInitBt);
						dColAddr+=bInitBt;
						Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
						dColAddr += bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
					}
				}
				HAL_Test_WriteOnePage(TLC_MODE, bChan,bChip,bLun,dOrderPrg[3],dBasRowAddr,dOrderPrg[j],MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
				HAL_CQWaitDone(bChan);
			}
		}
		else
		{
			break;
		}
	}
}

void HAL_Test_U0C_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bChip		= bLun / LUN_OF_CHIP;
    U32 dBasRowAddr    = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bChip);

	U08 bOrderCntinue;
	U32 dOrderPrg[4];
	//U32 dColAddr;
	U32 dPage;

	U08 bInitBt = 4;
	U16 wOrderCnt = 128;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	U16 i,j,k,n;
	for (i = 0;i < wOrderCnt;i++)
	{
		bOrderCntinue = HAL_Test_GetOrderPrgInfor(i,(U32 *)(&(dOrderPrg[0])));
		if(bOrderCntinue)
		{
			for(j = 0; j < 3; j++)
			{
				dPage = dBasRowAddr+dOrderPrg[j];
				for(n = 0; n < 4; n++)
				{
					for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
					{
						U32 dColAddr = n*4096+k*2048;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChan, bInitBt);
						dColAddr+=bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLunOfChip, bInitBt);
						dColAddr+=bInitBt;
						Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
						dColAddr += bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr), bInitBt,n);
					}
				}
				HAL_Test_WriteOnePage(TLC_MODE, bChan,bChip,bLun,dOrderPrg[3],dBasRowAddr,dOrderPrg[j],MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
				HAL_CQWaitDone(bChan);

			}
		}
		else
		{
			break;
		}
	}
}

void HAL_Test_U0A_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bChip		= bLun / LUN_OF_CHIP;
	U08 bLogChan    = bChan;
    U32 dBasRowAddr    = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bChip);

	U08 bOrderCntinue;
	U32 dOrderPrg[4];
	//U32 dColAddr;
	U32 dPage;

	U08 bInitBt = 4;
	U16 wOrderCnt = 128;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	U16 i,j,k,n;
	for (i = 0;i < wOrderCnt;i++)
	{
		bOrderCntinue = HAL_Test_GetOrderPrgInfor(i,(U32 *)(&(dOrderPrg[0])));
		if(bOrderCntinue)
		{
			for(j = 0; j < 2; j++)
			{
				dPage = dBasRowAddr+dOrderPrg[j];
				for(n = 0; n < 4; n++)
				{
					for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
					{
						U32 dColAddr = n*4096+k*2048;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLogChan, bInitBt);
						dColAddr+=bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLun, bInitBt);
						dColAddr+=bInitBt;
						Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
						dColAddr += bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
					}
				}
				HAL_Test_WriteOnePage(TLC_MODE, bChan,bChip,bLun,dOrderPrg[3],dBasRowAddr,dOrderPrg[j],MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
				HAL_CQWaitDone(bChan);

			}
		}
		else
		{
			break;
		}
	}
}

void HAL_Test_B0KB_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bLogChip		= bLun / LUN_OF_CHIP;
	U08 bChip		= 0;
	U08 bLogChan = bChan;
    U32 dBasRowAddr    = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bLogChip);

	U32 dPage;
	U08 bOrderCntinue;
	U32 dOrderPrg[4];

	U08 bCmdTag = 0;
	U08 bInitBt = 4;
	U16 wOrderCnt = 0x400;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	U16 i,j,k,n;
	for (i = 0;i < wOrderCnt;i++)
	{
		bOrderCntinue = HAL_Test_GetOrderPrgInfor(i,(U32 *)(&(dOrderPrg[0])));
		if(bOrderCntinue)
		{
			for (j = 0;j < dOrderPrg[0];j++)
			{
				dPage = dBasRowAddr+(dOrderPrg[j+1]&0xFFFFFF);//+j;
				for(n = 0; n < 4; n++)
				{
					for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
					{
						U32 dColAddr = n*4096+k*2048;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLogChan, bInitBt);
						dColAddr+=bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLun, bInitBt);
						dColAddr+=bInitBt;
						Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
						dColAddr += bInitBt;
						MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
					}
				}
				HAL_Test_WriteOnePage(TLC_MODE,bChan,bChip,bLunOfChip,bCmdTag,dBasRowAddr,dOrderPrg[j+1],MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
				HAL_CQWaitDone(bChan);
			}
		}
		else
		{
			break;
		}
	}
}

void HAL_Test_B16A_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bLogChip	= bLun / LUN_OF_CHIP;
	U08 bLogChan = bChan;
	U08 bChip		= 0;
    U32 dBasRowAddr = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bLogChip);

	U32 dPage;
	U08 bCmdTag = 0;
	U08 bInitBt = 4;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);
	MemorySet((U8 *)(MEM_UD_BUFF_ADDR_EXT),0x5A, 16*1024);

	U16 i,k,n;
	for (i = 0;i < FDL_PAGE_OF_BLK;i++)
	{
		dPage = dBasRowAddr+i;//+j;
		for(n = 0; n < 4; n++)
		{
			for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
			{
				U32 dColAddr = n*4096+k*2048;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLogChan, bInitBt);
				dColAddr+=bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLun, bInitBt);
				dColAddr+=bInitBt;
				Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
				dColAddr += bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
			}
		}
		HAL_Test_WriteOnePage(TLC_MODE,bChan,bChip,bLunOfChip,bCmdTag,dBasRowAddr,i,MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
		HAL_CQWaitDone(bChan);
	}
}

void HAL_Test_L06B_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bLogChip	= bLun / LUN_OF_CHIP;
	U08 bChip		= 0;
    U32 dBasRowAddr = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bLogChip);

	U32 dPage;
	U08 bCmdTag = 0;
	U08 bInitBt = 4;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	U16 i,k,n;
	for (i = 0;i < FDL_PAGE_OF_BLK;i++)
	{
		dPage = dBasRowAddr+i;//+j;
		for(n = 0; n < 4; n++)
		{
			for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
			{
				U32 dColAddr = n*4096+k*2048;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bChan, bInitBt);
				dColAddr+=bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLogChip, bInitBt);
				dColAddr+=bInitBt;
				Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
				dColAddr += bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
			}
		}
		HAL_Test_WriteOnePage(TLC_MODE,bChan,bChip,bLunOfChip,bCmdTag,dBasRowAddr,i,MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
		HAL_CQWaitDone(bChan);
	}
}

void HAL_Test_BICS3_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bLogChip	= bLun / LUN_OF_CHIP;
	U08 bLogChan = bChan;
	U08 bChip		= 0;
    U32 dBasRowAddr = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bLogChip);

	U32 dPage;
	//U08 bOrderCntinue;
	//U32 dOrderPrg[4];

	//U08 bData = 0;
	U08 bCmdTag = 0;
	U16 wWlCnt = 256;
	U16 i,k,n;

	U08 bInitBt = 4;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	for (i = 0;i < wWlCnt;i++)//FDL_PAGE_OF_BLK
	{
		dPage = dBasRowAddr+i;//+j;
		for(n = 0; n < 4; n++)
		{
			for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
			{
				U32 dColAddr = n*4096+k*2048;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLogChan, bInitBt);
				dColAddr+=bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLun, bInitBt);
				dColAddr+=bInitBt;
				Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
				dColAddr += bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
			}
		}
		for (bCmdTag = 1; bCmdTag <= 3; bCmdTag++)
		{
			HAL_Test_WriteOnePage(TLC_MODE,bChan,bChip,bLunOfChip,bCmdTag,dBasRowAddr,i,MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
			HAL_CQWaitDone(bChan);
		}
	}
}

void HAL_Test_3DV4_WriteTlcBlk(U8 bChan,U8 bLun,U32 dStartBlkNub)
{
    U08 bLunOfChip  = bLun % LUN_OF_CHIP;
	U08 bLogChip	= bLun / LUN_OF_CHIP;
	U08 bLogChan = bChan;
	U08 bChip		= 0;
    U32 dBasRowAddr = dStartBlkNub * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;
    bChan = CHANS(bChan);
	bChip = CHIPES(bChan,bLogChip);

	U32 dPage;
	//U08 bOrderCntinue;
	//U32 dOrderPrg[4];

	//U08 bData = 0;
	U08 bCmdTag = 0;
	U16 wWlCnt = 256;
	U16 i,k,n;

	U08 bInitBt = 4;

	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR),0xAA, 16*1024);

	for (i = 0;i < wWlCnt;i++)//FDL_PAGE_OF_BLK
	{
		dPage = dBasRowAddr+i;//+j;
		for(n = 0; n < 4; n++)
		{
			for(k = 0;k < SECT_OF_DATA_CLST/SECT_OF_DMA;k++)
			{
				U32 dColAddr = n*4096+k*2048;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLogChan, bInitBt);
				dColAddr+=bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),bLun, bInitBt);
				dColAddr+=bInitBt;
				Mem32SetVal((U32 *)(MEM_DMA_BUFF_ADDR+dColAddr),dPage,bInitBt/4);
				dColAddr += bInitBt;
				MemorySet((U8 *)(MEM_DMA_BUFF_ADDR+dColAddr),n, bInitBt);
			}
		}
		for (bCmdTag = 1; bCmdTag <= 3; bCmdTag++)
		{
			HAL_Test_WriteOnePage(TLC_MODE,bChan,bChip,bLunOfChip,bCmdTag,dBasRowAddr,i,MEM_DMA_BUFF_ADDR,MEM_UD_BUFF_ADDR_EXT,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST);
			HAL_CQWaitDone(bChan);
		}
	}
}

void HAL_Test_PrgTlcBlks(U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd,U8 bRndEn)
{
	U16 i,j,k;
	U16 wBlkNum = 0;
	if(bRndEn == 0)
	{
		for(i = bChanSt; i <= bChanEd; i++)
		{
			SG_DMA2_CH(i)->rRND_CTRL.bits.RND_BYPASS = 1;
		}
	}
	#ifdef LOG_PHY_ADDR
	FDL_Log("pAddr000:|  Chan  |  Luns  |   BlkAddr  |");
	#endif

	for (i = bChanSt; i <= bChanEd; i++)
	{
		for(j = bLunSt;j <= bLunEd;j++)
		{
			for(k = wBlockSt;k <= wBlockEd; k++)
			{
				#if (FDL_TEST_FLASH == H27QEG8NDB)
				HAL_Test_8NDB_WriteTlcBlk(i,j,k);
				#endif

				#if (FDL_TEST_FLASH == MIC_B0KB)
				HAL_Test_B0KB_WriteTlcBlk(i,j,k);
				#ifdef LOG_PHY_ADDR
				FDL_Log("pAddr%03d:|   %02d   |   %02d   |  0x%06x  |",wBlkNum,i,j,(k * ROW_OF_BLOCK + (j / LUN_OF_CHIP) * ROW_GAP_OF_LUN));
				#endif
				#endif
				#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)|| (NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B) ||(NAND_FLASH == YMTC_B08T))
				HAL_Test_B16A_WriteTlcBlk(i,j,k);
				#endif

				#if (FDL_TEST_FLASH == K9ADGD8U0C)
				HAL_Test_U0C_WriteTlcBlk(i,j,k);
				#endif

				#if (FDL_TEST_FLASH == K9ADGD8U0A)
				HAL_Test_U0A_WriteTlcBlk(i,j,k);
				#endif

				#if (FDL_TEST_FLASH == MIC_L06B)
				HAL_Test_L06B_WriteTlcBlk(i,j,k);
				#endif

				#if (FDL_TEST_FLASH == TOS_BICS3)
				HAL_Test_BICS3_WriteTlcBlk(i,j,k);
				#endif

				#if (FDL_TEST_FLASH == HYNIX_3DV4)
				HAL_Test_3DV4_WriteTlcBlk(i,j,k);
				#endif
				wBlkNum++;
			}
		}
	}
}

U32 HAL_GetRdInfor(U08 bChanSt,U08 bChanEd,U08 bLunSt,U08 bLunEd,U32 dDtAddr,U32 dInforAddr,U08 bDtInitVal)
{
	U08 i,j,n,t;
	U32 dInforSize = 0;
	U08 *pInforBuff = (U08 *)(dInforAddr);
	U08 *pDtBuff = (U08 *)dDtAddr;

	for (j = bLunSt; j <= bLunEd; j++)
	{
		for (i = bChanSt; i <= bChanEd; i++ )
		{
			for(n = 0;n < DATA_CLST_OF_PAGE;n++)
			{
				for(t = 0;t < SECT_OF_DATA_CLST/SECT_OF_DMA;t++)
				{
					MemCopy(pInforBuff,pDtBuff, 16);
					if(bDtInitVal)
					{
						MemorySet((U8 *)pDtBuff,bDtInitVal, 0x10);
					}
					pDtBuff += 2048;
					pInforBuff+=0x10;
					dInforSize+=0x10;
				}
			}
		}
	}
	return(dInforSize);
}
void HAL_Test_RedBlks(U8 bRdMode,U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd, U16 wPagSt,U16 wPagCnt,U32 dDtBuff,U32 dUdBuff,U8 bRndEn,U32 dRdInforAddr)
{
	U08 bChan;
	U08 bChip;
    U08 bLunOfChip;
	U08 bPreCmd = 0;
	U08 bRdFail = 0;

	U32 dDtBuffTp = dDtBuff;
	U32 dInforBuffTp = dRdInforAddr;

    U32 dBaseRow;
	U32 i,j,k,m,t;

	if(bRndEn == 0)
	{
		for(i = bChanSt; i <= bChanEd; i++)
		{
			SG_DMA2_CH(i)->rRND_CTRL.bits.RND_BYPASS = 1;
		}
	}

	if(bRdMode == SLC_MODE)
	{
		if (pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevWrCmd.bCmdFlg)
		{
			bPreCmd = pFdlParam.ptUdpFdl->m_tFlashOperParam.tPrevWrCmd.dCmdCQ[0];
		}
	}

	U32 dRdInforSize;
	U32 dRdInforTp = 0;
	for(m = wPagSt; m < wPagSt+wPagCnt; m++)
	{
		for (k = wBlockSt; k <= wBlockEd; k++)
		{
			dDtBuff = dDtBuffTp;
			for (j = bLunSt; j <= bLunEd; j++)
			{
				for (i = bChanSt; i <= bChanEd; i++ )
				{
					bChip		= j / LUN_OF_CHIP;
					bLunOfChip	= j % LUN_OF_CHIP;
					dBaseRow	= k * ROW_OF_BLOCK + bLunOfChip * ROW_GAP_OF_LUN;

					bChan = CHANS(i);
					bChip = CHIPES(bChan,bChip);

				#if (NAND_FLASH != TOS_BICS3)
					if(bPreCmd)
					{
						HAL_CQSendFlashCmd(bChan,bPreCmd);
					}
				#endif
					HAL_Test_ReadOnePage(bRdMode,bChan,bChip,bLunOfChip,dBaseRow,m,dDtBuff,dUdBuff);

					dDtBuff+=SECT_OF_PAGE*SECTOR_SIZE;
					dUdBuff+=UD_LEN_OF_DATA_CLST*DATA_CLST_OF_PAGE;
				}
				for (t = bChanSt; t <= bChanEd; t++)
				{
					bChan = CHANS(t);
					HAL_CQWaitDone(bChan);

					if(SG_DMA2_CH(bChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1)
					{
						LOG_PRINT("Rd_Fail Chan = %x ,Lun  = %x,Page = %x\r\n",bChan,j,m);
						SG_DMA2_CH(bChan)->rSTATUS.bits.ECC_FAIL_FLAG = 0;
					}
				}
			}

			if(dRdInforAddr != 0)
			{
				dRdInforSize = HAL_GetRdInfor(bChanSt,bChanEd,bLunSt,bLunEd,dDtBuffTp,dRdInforAddr+4,0xAA);
				dRdInforAddr+=dRdInforSize;
				dRdInforTp+=dRdInforSize;
				for(j = 0;j < (bLunEd+1-bLunSt)*(bChanEd+1-bChanSt)*SECTOR_SIZE*SECT_OF_PAGE;j++)
				{
					if(*((U08 *)dDtBuffTp+j) != 0xAA)
					{
						bRdFail = 1;
						return;
					}
				}
			}
		}
	}

	if(bRdFail)
	{
		*((U32 *)dRdInforAddr) = 0xFFFFFFFF;
	}
	else
	{
		*((U32 *)dInforBuffTp) = dRdInforTp;
	}
}


#ifdef NEW_FLASH_DEBUG
void HAL_Test_MultiPlane_Read(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff)
{
	U32 dRowAddr;
	U08 i = 0;
	bChan = CHANS(bChan);
	HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);

	if(bMode == TLC_MODE)
	{
		#if (FDL_TEST_FLASH == H27QEG8NDB)
        //HAL_CQSetRndSeed(bChan,dRowInBlk/3,INVALID_U16);
		dRowAddr = dRowInBlk/3 + dBaseRow;
		#else
		dRowAddr = dRowInBlk + dBaseRow;
		#endif
	}
	else
	{
        //HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);
		dRowAddr = dRowInBlk + dBaseRow;
	}
    HAL_CQSetUd(bChan,dUserBuff,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,1);

    HAL_CQChipSelect(bChan,bChip);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);

	if(bMode == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan,0xdf);
	}
	else
	{
		HAL_CQSendFlashCmd(bChan,0xda);
	}
	for(i = 0; i <  FDL_PLANE_OF_LUN; i++)
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	    HAL_CQSend5Addr(bChan,0,(dRowAddr + ROW_OF_BLOCK * i));
		if((i + 1) == FDL_PLANE_OF_LUN)
		{
			HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
		}
		else
		{
	   		HAL_CQSendFlashCmd(bChan,FL_CMD_READ_MP);
		}
	    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);
		//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA;//退出检测状态
	}
#if 0
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
    HAL_CQSend5Addr(bChan,0,dRowAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_READ_MP);
    HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);

	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,(dRowAddr+ROW_OF_BLOCK));
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_IO,FIO_PF_PASS_NULL);

	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,(dRowAddr+ROW_OF_BLOCK));
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_IO,FIO_PF_PASS_NULL);

	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,(dRowAddr+ROW_OF_BLOCK));
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_IO,FIO_PF_PASS_NULL);
#endif
	for(i = 0; i <  FDL_PLANE_OF_LUN; i++)
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
		HAL_CQSend5Addr(bChan,0,(dRowAddr + i*ROW_OF_BLOCK));
		HAL_CQSendRndAddr(bChan,0,dRowAddr);

		HAL_CQStartDMA2(bChan,dDataBuff, DMA2_RD_TO_BUFF, 0,0, 32, 0,MEM_DMA2_INFO_ADDR);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
		while(HAL_CQGetIdleSpace(bChan) < 0xE0);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_READ, FIO_PF_PASS_NULL);

	}
	#if 0
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,(dRowAddr));
	HAL_CQSendRndAddr(bChan,0,dRowAddr);

	HAL_CQStartDMA2(bChan,dDataBuff, DMA2_RD_TO_BUFF, 0,0, 32, 0,MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	while(HAL_CQGetIdleSpace(bChan) < 0xE0);

	HAL_CQCheckIOStatus(bChan, bLun, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
	//HAL_CQSendFlashCmd(bChan,0xda);
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,(dRowAddr+ROW_OF_BLOCK));
	HAL_CQSendRndAddr(bChan,0,dRowAddr);
	HAL_CQStartDMA2(bChan,dDataBuff, DMA2_RD_TO_BUFF, 0,0, 32, 0,MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA;//退出检测状态
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	while(HAL_CQGetIdleSpace(bChan) < 0xE0);
	#endif
}

void HAL_Test_Copyback(U08 bMode,U16 wPageCnt,U16 bSourceBlk,U16 bDesBlk)
{
	//bMode
	//0.SLC TO SLC
	//1.SLC TO TLC
	//2.TLC TO SLC
	//3.TLC TO TLC
	U08 bCmd[2]={0};
	U08 bChan = 0;
	U08 bChip = 0;
	U16 i = 0;

	(bMode < 2)?(bCmd[0] = 0xda):(bCmd[0] = 0xdf);
	((bMode % 2) == 1)?(bCmd[1] = 0xdf):(bCmd[1] = 0xda);
	for(i = 0; i < wPageCnt; i++)
	{
		HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
		HAL_CQChipSelect(bChan,bChip);
		HAL_CQSendFlashCmd(bChan,bCmd[0]);
	    HAL_CQSendFlashCmd(bChan,0x00);
	  	HAL_CQSend5Addr(bChan,0,bSourceBlk*ROW_OF_BLOCK+i);
		HAL_CQSendFlashCmd(bChan,0x35);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);

		HAL_CQChipSelect(bChan,bChip);
		HAL_CQSendFlashCmd(bChan,bCmd[1]);
		if(bCmd[1] == 0xda)
		{
			HAL_CQSendFlashCmd(bChan,0x85);
			HAL_CQSend5Addr(bChan,0,ROW_OF_BLOCK*bDesBlk+i);
			HAL_CQSendFlashCmd(bChan,0x10);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
		}
		else
		{
			#if ((NAND_FLASH == K9ADGD8U0A))
			{
				HAL_CQSendFlashCmd(bChan,0x85);
				HAL_CQSend5Addr(bChan,0,ROW_OF_BLOCK*bDesBlk+i);
				HAL_CQSendFlashCmd(bChan, 0xC0);
				HAL_CQSendAddr(bChan, (0x31 + ((i)%2)));
				HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
				U32 dCMD;
				if(((i)%2) == 1)
				{
					HAL_CQSendFlashCmd(bChan, 0xdf);
					HAL_CQSendFlashCmd(bChan, 0x8B);
					HAL_CQSend5Addr(bChan, 0, (ROW_OF_BLOCK*bDesBlk+(i / 2)));
					HAL_CQSendFlashCmd(bChan, 0x10);
				}
			}
			#endif
		}
		while(HAL_CQGetIdleSpace(bChan) < 0xF0);
	}
}
void HAL_Test_Copyback_Multi_Plane(U08 bMode,U16 wPageCnt,U16 bSourceBlk,U16 bDesBlk)
{
		//bMode
	//0.SLC TO SLC
	//1.SLC TO TLC
	//2.TLC TO SLC
	//3.TLC TO TLC
	U08 bCmd[2]={0};
	U08 bChan = 0;
	U08 bChip = 0;
	U16 i = 0;

	(bMode < 2)?(bCmd[0] = 0xda):(bCmd[0] = 0xdf);
	((bMode % 2) == 1)?(bCmd[1] = 0xdf):(bCmd[1] = 0xda);
	for(i = 0; i < wPageCnt; i++)
	{
		HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
		HAL_CQChipSelect(bChan,bChip);
		HAL_CQSendFlashCmd(bChan,bCmd[0]);
	    HAL_CQSendFlashCmd(bChan,0x00);
	  	HAL_CQSend5Addr(bChan,0,bSourceBlk*ROW_OF_BLOCK+i);
		HAL_CQSendFlashCmd(bChan,0x32);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
		HAL_CQSendFlashCmd(bChan,0x00);
	  	HAL_CQSend5Addr(bChan,0,(bSourceBlk+1)*ROW_OF_BLOCK+i);
		HAL_CQSendFlashCmd(bChan,0x35);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);

		HAL_CQChipSelect(bChan,bChip);
		HAL_CQSendFlashCmd(bChan,bCmd[1]);
		if(bCmd[1] == 0xda)
		{
			HAL_CQSendFlashCmd(bChan,0x85);
			HAL_CQSend5Addr(bChan,0,ROW_OF_BLOCK*bDesBlk+i);
			HAL_CQSendFlashCmd(bChan,0x11);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS,FIO_RB_READY_IO, FIO_PF_PASS_NULL);
			HAL_CQSendFlashCmd(bChan,0x81);
			HAL_CQSend5Addr(bChan,0,ROW_OF_BLOCK*(bDesBlk+1)+i);
			HAL_CQSendFlashCmd(bChan,0x10);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
		}
		else
		{
			#if ((NAND_FLASH == K9ADGD8U0A))
			{
				HAL_CQSendFlashCmd(bChan,0x85);
				HAL_CQSend5Addr(bChan,0,ROW_OF_BLOCK*bDesBlk+i);
				HAL_CQSendFlashCmd(bChan,0x11);
				HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_NULL);
				HAL_CQSendFlashCmd(bChan,0x81);
				HAL_CQSend5Addr(bChan,0,ROW_OF_BLOCK*(bDesBlk+1)+i);
				HAL_CQSendFlashCmd(bChan, 0xC0);
				HAL_CQSendAddr(bChan, (0x31 + ((i)%2)));
				HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
				U32 dCMD;
				if(((i)%2) == 1)
				{
					HAL_CQSendFlashCmd(bChan, 0xdf);
					HAL_CQSendFlashCmd(bChan, 0x8B);
					HAL_CQSend5Addr(bChan, 0, (ROW_OF_BLOCK*bDesBlk+(i / 2)));
					HAL_CQSendFlashCmd(bChan, 0x11);
					HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS,  FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
					HAL_CQSendFlashCmd(bChan, 0x8B);
					HAL_CQSend5Addr(bChan, 0, (ROW_OF_BLOCK*(bDesBlk+1)+(i / 2)));
					HAL_CQSendFlashCmd(bChan, 0x10);
					HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS,  FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
				}
			}
			#endif
		}
		while(HAL_CQGetIdleSpace(bChan) < 0xF0);
	}
}

void HAL_Test_Copyback_Blk(U08 bCopybackMode, U16 wPageCount, U16 wSouBlk, U16 wDesBlk)
{
	//bMode
	//0.SLC TO SLC
	//1.SLC TO TLC
	//2.TLC TO SLC
	//3.TLC TO TLC
	U08 bChan = 0;
	U16 i;
	U08 bChanSt = 0;
	U08 bChanEd = 0;
	U08 bLunSt = 0;
	U08 bLunEd = 0;
	U16 wBlockSt = wSouBlk;
	U16 wBlockEd = wBlockSt;
	U16 wPagSt = 0;
	U16 wPagCnt = PAGE_OF_BLOCK_SLC;
	U08 bRndEn = 1;
	U32 dDtBuff = MEM_DMA_BUFF_ADDR;
	U32 dUdBuff = MEM_UD_BUFF_ADDR;
	U08 bMode[2] = {0};
	(bCopybackMode < 2)?(bMode[0] = SLC_MODE):(bMode[0] = TLC_MODE);
	((bCopybackMode % 2) == 1)?(bMode[1] = TLC_MODE):(bMode[1] = SLC_MODE);

	HAL_Test_ErsBlks(bMode[0],bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_ErsBlks(bMode[1],bChanSt,bChanEd,bLunSt,bLunEd,wDesBlk,wDesBlk);
	if(bMode[0] == SLC_MODE)
	{
		HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	}
	else
	{
		HAL_Test_PrgTlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,bRndEn);
	}
	HAL_Test_Copyback(bCopybackMode,wPageCount,wSouBlk,wDesBlk);
	for(i = 0; i < wPageCount; i++)
	{
		HAL_Test_ReadOnePage(bMode[1],bChanSt,bChanEd,0,ROW_OF_BLOCK*wDesBlk,i,MEM_DMA_BUFF_ADDR,dUdBuff);
	}
}
void HAL_Test_Copyback_Multi_Plane_Blk(U08 bCopybackMode, U16 wPageCount, U16 wSouBlk, U16 wDesBlk)
{
	//bMode
	//0.SLC TO SLC
	//1.SLC TO TLC
	//2.TLC TO SLC
	//3.TLC TO TLC
	U08 bChan = 0;
	U16 i;
	U08 bChanSt = 0;
	U08 bChanEd = 0;
	U08 bLunSt = 0;
	U08 bLunEd = 0;
	U16 wBlockSt = wSouBlk;
	U16 wBlockEd = wBlockSt+1;
	U16 wPagSt = 0;
	U16 wPagCnt = PAGE_OF_BLOCK_SLC;
	U08 bRndEn = 1;
	U32 dDtBuff = MEM_DMA_BUFF_ADDR;
	U32 dUdBuff = MEM_UD_BUFF_ADDR;
	U08 bMode[2] = {0};
	(bCopybackMode < 2)?(bMode[0] = SLC_MODE):(bMode[0] = TLC_MODE);
	((bCopybackMode % 2) == 1)?(bMode[1] = TLC_MODE):(bMode[1] = SLC_MODE);

	HAL_Test_ErsBlks(bMode[0],bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_ErsBlks(bMode[1],bChanSt,bChanEd,bLunSt,bLunEd,wDesBlk,(wDesBlk+1));
	if(bMode[0] == SLC_MODE)
	{
		HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	}
	else
	{
		HAL_Test_PrgTlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,bRndEn);
	}
	HAL_Test_Copyback_Multi_Plane(bCopybackMode,wPageCount,wSouBlk,wDesBlk);
	for(i = 0; i < wPageCount; i++)
	{
		HAL_Test_MultiPlane_Read(bMode[1],bChanSt,bChanEd,0,ROW_OF_BLOCK*wDesBlk,i,MEM_DMA_BUFF_ADDR,dUdBuff);
	}
}

void HAL_Test_SnapReadOnePage(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff,U08 bCtls)
{
    U32 dRowAddr;
    U16 wColAddr = DATA_COL_ADDR(bCtls);

	HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);

	if(bMode == TLC_MODE)
	{
		#if (FDL_TEST_FLASH == H27QEG8NDB)
        //HAL_CQSetRndSeed(bChan,dRowInBlk/3,INVALID_U16);
		dRowAddr = dRowInBlk/3 + dBaseRow;
		#else
		dRowAddr = dRowInBlk + dBaseRow;
		#endif
	}
	else
	{
        //HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);
		dRowAddr = dRowInBlk + dBaseRow;
	}
    HAL_CQSetUd(bChan,dUserBuff,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,1);

    HAL_CQChipSelect(bChan,bChip);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
	if(bMode == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan,0xdf);
	}
	else
	{
		HAL_CQSendFlashCmd(bChan,0xda);
	}

	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
    HAL_CQSend5Addr(bChan,wColAddr,dRowAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_READ_SNAP);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
	HAL_CQSendRndAddr(bChan,wColAddr,dRowAddr);

	HAL_CQStartDMA2(bChan,dDataBuff, DMA2_RD_TO_BUFF, 0,0, 8, 0,MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	while(HAL_CQGetIdleSpace(bChan) < 0xF0);
}
void HAL_Test_Cache_Random_Read(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff,U32 dNextRow)
{
	U32 dRowAddr;
	static U08 bFirstSend = 0;
	HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);

	if(bMode == TLC_MODE)
	{
		#if (FDL_TEST_FLASH == H27QEG8NDB)
        //HAL_CQSetRndSeed(bChan,dRowInBlk/3,INVALID_U16);
		dRowAddr = dRowInBlk/3 + dBaseRow;
		#else
		dRowAddr = dRowInBlk + dBaseRow;
		#endif
	}
	else
	{
        //HAL_CQSetRndSeed(bChan,dRowInBlk,INVALID_U16);
		dRowAddr = dRowInBlk + dBaseRow;
	}
    HAL_CQSetUd(bChan,dUserBuff,SECT_OF_DATA_CLST,UD_LEN_OF_DATA_CLST,1);

    HAL_CQChipSelect(bChan,bChip);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);

	if(bMode == TLC_MODE)
	{
		HAL_CQSendFlashCmd(bChan,0xdf);
	}
	else
	{
		HAL_CQSendFlashCmd(bChan,0xda);
	}
	if(bFirstSend == 0)
	{
		HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	    HAL_CQSend5Addr(bChan,0,dRowAddr);
	    HAL_CQSendFlashCmd(bChan,FL_CMD_READ2);
	    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
		bFirstSend = 1;
	}
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,dNextRow);
	HAL_CQSendFlashCmd(bChan,FL_CMD_CACHE_REDA);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_IO, FIO_PF_PASS_OTHER);
	HAL_CQSendRndAddr(bChan,0,dRowAddr);

	HAL_CQStartDMA2(bChan,dDataBuff, DMA2_RD_TO_BUFF, 0,0, 32, 0,MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	while(HAL_CQGetIdleSpace(bChan) < 0x90);
}

void HAL_Test_System(void)
{
	#define HAL_TEST_SLC_NORMAL_WAR
	//#define HAL_TEST_TLC_NORMAL_WAR
	//#define HAL_TEST_SLC_MULTI_PLANE_READ
	//#define HAL_TEST_TLC_MULTI_PLANE_READ
	//#define HAL_TEST_SLC_FDL_4K_SNAP_READ_EN
	//#define HAL_TEST_TLC_FDL_4K_SNAP_READ_EN
	//#define HAL_TEST_COPYBACK
	//#define HAL_TEST_COPYBACK_MULTI
	//#define HAL_TEST_COPYBACK_Multi_Plane_SLC_TO_SLC
	//#define HAL_TEST_COPYBACK_Multi_Plane_SLC_TO_TLC
	//#define HAL_TEST_COPYBACK_Multi_Plane_TLC_TO_SLC
	//#define HAL_TEST_COPYBACK_Multi_Plane_TLC_TO_TLC
	//#define HAL_TEST_CACHE_READ_SEQUESE_IN_ONE_BLK
	//#define HAL_TEST_CACHE_READ_GAP_IN_SAME_BLK
	//#define HAL_TEST_CACHE_READ_GAP_IN_DIFF_BLK
	enum{SLC_TO_SLC = 0,SLC_TO_TLC,TLC_TO_SLC,TLC_TO_TLC};
	U08 bChan = 0;
	U16 i;
	U08 bChanSt = 1;
	U08 bChanEd = 1;
	U08 bLunSt = 0;
	U08 bLunEd = 0;
	U16 wBlockSt = 20;
	U16 wBlockEd = 20;
	U16 wPagSt = 0;
	U16 wPagCnt = PAGE_OF_BLOCK_SLC;
	U08 bRndEn = 1;
	U32 dDtBuff = MEM_DMA_BUFF_ADDR;
	U32 dUdBuff = MEM_UD_BUFF_ADDR;

	#ifdef HAL_TEST_SLC_NORMAL_WAR
	HAL_Test_ErsBlks(SLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	for(i = 0; i < PAGE_OF_BLOCK_SLC; i++)
	{
		HAL_Test_ReadOnePage(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff);
	}
	#endif

	#ifdef HAL_TEST_TLC_NORMAL_WAR
	HAL_Test_ErsBlks(TLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgTlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,bRndEn);
	for(i = 0; i < PAGE_OF_BLOCK; i++)
	{
		HAL_Test_ReadOnePage(TLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff);
	}
	#endif

	#ifdef HAL_TEST_SLC_MULTI_PLANE_READ
	wBlockEd = wBlockSt+1;
	HAL_Test_ErsBlks(SLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	for(i = 0; i < PAGE_OF_BLOCK_SLC; i++)
	{
		HAL_Test_MultiPlane_Read(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff);
	}
	#endif

	#ifdef HAL_TEST_TLC_MULTI_PLANE_READ
	wBlockEd = wBlockSt+1;
	HAL_Test_ErsBlks(TLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgTlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,bRndEn);
	for(i = 0; i < PAGE_OF_BLOCK; i++)
	{
		HAL_Test_MultiPlane_Read(TLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff);
	}
	#endif

    #ifdef HAL_TEST_SLC_FDL_4K_SNAP_READ_EN
	HAL_Test_ErsBlks(SLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	for(i = 0; i < PAGE_OF_BLOCK_SLC; i++)
	{
		HAL_Test_SnapReadOnePage(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,0);
		HAL_Test_SnapReadOnePage(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,2);
		HAL_Test_SnapReadOnePage(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,4);
		HAL_Test_SnapReadOnePage(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,6);
	}
	#endif

	#ifdef HAL_TEST_TLC_FDL_4K_SNAP_READ_EN
	HAL_Test_ErsBlks(TLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgTlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,bRndEn);
	for(i = 0; i < PAGE_OF_BLOCK; i++)
	{
		HAL_Test_SnapReadOnePage(TLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,0);
		HAL_Test_SnapReadOnePage(TLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,2);
		HAL_Test_SnapReadOnePage(TLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,4);
		HAL_Test_SnapReadOnePage(TLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,i,MEM_DMA_BUFF_ADDR,dUdBuff,6);
	}
	#endif

	#ifdef HAL_TEST_COPYBACK
	U08 bCopyBackPageCount = 128;
	U16 wSouBlk = 0;
	U16 wDesBlk = 2;
	U08 bCopyBackMOde = SLC_TO_SLC;
	//U08 bCopyBackMOde = SLC_TO_TLC;
	//U08 bCopyBackMOde = TLC_TO_SLC;
	//U08 bCopyBackMOde = TLC_TO_TLC;
	HAL_Test_Copyback_Blk(bCopyBackMOde,bCopyBackPageCount,wSouBlk,wDesBlk);
	#endif

	#ifdef HAL_TEST_COPYBACK_MULTI
	U08 bCopyBackPageCount = 128;
	U16 wSouBlk = 0;
	U16 wDesBlk = 2;
	//U08 bCopyBackMOde = SLC_TO_SLC;
	U08 bCopyBackMOde = SLC_TO_TLC;
	//U08 bCopyBackMOde = TLC_TO_SLC;
	//U08 bCopyBackMOde = TLC_TO_TLC;
	HAL_Test_Copyback_Multi_Plane_Blk(bCopyBackMOde,bCopyBackPageCount,wSouBlk,wDesBlk);
	#endif

	#ifdef HAL_TEST_CACHE_READ_SEQUESE_IN_ONE_BLK
	HAL_Test_ErsBlks(SLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	for(i = 1; i <  128; i++)
	{
		HAL_Test_Cache_Random_Read(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,0,MEM_DMA_BUFF_ADDR,dUdBuff,(ROW_OF_BLOCK*wBlockSt+i));
	}
	#endif

	#ifdef HAL_TEST_CACHE_READ_GAP_IN_SAME_BLK
	HAL_Test_ErsBlks(SLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	for(i = 1; i <  128; i++)
	{
		HAL_Test_Cache_Random_Read(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,0,MEM_DMA_BUFF_ADDR,dUdBuff,(ROW_OF_BLOCK*wBlockSt+i*2));
	}
	#endif

	#ifdef HAL_TEST_CACHE_READ_GAP_IN_DIFF_BLK
	wBlockEd = wBlockSt+1;
	HAL_Test_ErsBlks(SLC_MODE,bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd);
	HAL_Test_PrgSlcBlks(bChanSt,bChanEd,bLunSt,bLunEd,wBlockSt,wBlockEd,wPagSt,wPagCnt,bRndEn);
	for(i = 1; i <  128; i++)
	{
		HAL_Test_Cache_Random_Read(SLC_MODE,bChanSt,0,0,ROW_OF_BLOCK*wBlockSt,0,MEM_DMA_BUFF_ADDR,dUdBuff,(ROW_OF_BLOCK*(wBlockSt+(i / 2))+i*2));
	}
	#endif
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////code down below is added by flash analyse group
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FA_Flash_Info_Init()
{
	HostCmdResponse();
	NANDFlash.CE_IN_CHANNEL = FA_FLASH_INFO_BUFF[0];	//每个Channel中CE的数量
    NANDFlash.LUN_IN_CE = FA_FLASH_INFO_BUFF[1];	//每个CE中LUN的数量
    NANDFlash.PLANE_IN_LUN = FA_FLASH_INFO_BUFF[2]; //每个LUN中的plane数量
    NANDFlash.FLASH_CODE = ((U16)FA_FLASH_INFO_BUFF[3])<<8 + FA_FLASH_INFO_BUFF[4];	//FlashCode
    NANDFlash.PAGE_IN_BLOCK = ((U32)FA_FLASH_INFO_BUFF[5])<<24 + ((U32)FA_FLASH_INFO_BUFF[6])<<16 + ((U32)FA_FLASH_INFO_BUFF[7])<<8 + FA_FLASH_INFO_BUFF[8]; //每个Block中的物理页数量
    NANDFlash.SLC_WL_IN_BLOCK = ((U32)FA_FLASH_INFO_BUFF[9])<<24 + ((U32)FA_FLASH_INFO_BUFF[10])<<16 + ((U32)FA_FLASH_INFO_BUFF[11])<<8 + FA_FLASH_INFO_BUFF[12]; //每个Block中的SLC模式物理页数量
    NANDFlash.BYTE_IN_PAGE = ((U32)FA_FLASH_INFO_BUFF[13])<<24 + ((U32)FA_FLASH_INFO_BUFF[14])<<16 + ((U32)FA_FLASH_INFO_BUFF[15])<<8 + FA_FLASH_INFO_BUFF[16]; //页字节数
    NANDFlash.SECTOR_IN_PAGE = FA_FLASH_INFO_BUFF[17]; //1个page有多少个扇区
    NANDFlash.ROW_ADDR_CYCLE = FA_FLASH_INFO_BUFF[18]; //默认地址轮数（含列地址），比如5或6

    NANDFlash.FA_ERASE_TYPE = ((U16)FA_FLASH_INFO_BUFF[19])<<8 + FA_FLASH_INFO_BUFF[20]; //擦除方式类别，
    NANDFlash.FA_PROGRAM_TYPE = ((U16)FA_FLASH_INFO_BUFF[21])<<8 + FA_FLASH_INFO_BUFF[22]; //编程方式类别，比如是否有PreCMD指定WL中的页，是否是one-pass编程，编程顺序（地址计算）类别，比如QLC的锯齿或Coarse-Fine编程
    NANDFlash.FA_READ_TYPE = ((U16)FA_FLASH_INFO_BUFF[23])<<8 + FA_FLASH_INFO_BUFF[24]; //读方式类别，比如是否有PreCMD指定页，是否发2Eh配置retry，是否使用ROIC（row地址后多加几轮地址）配置retry
    NANDFlash.FA_RETRY_TYPE = ((U16)FA_FLASH_INFO_BUFF[25])<<8 + FA_FLASH_INFO_BUFF[26]; //配置offset的方式。
    NANDFlash.FA_SEED_TYPE = ((U16)FA_FLASH_INFO_BUFF[27])<<8 + FA_FLASH_INFO_BUFF[28];	//选择随机化种子类型（如三星256、512）

    NANDFlash.PROGRAM_ORDER_COUNT = ((U32)FA_FLASH_INFO_BUFF[29])<<24 + ((U32)FA_FLASH_INFO_BUFF[30])<<16 + ((U32)FA_FLASH_INFO_BUFF[31])<<8 + FA_FLASH_INFO_BUFF[32]; //TLC模式下WL编程次数或者页编程次数
    NANDFlash.BLOCK_ADDR_GAP = ((U32)FA_FLASH_INFO_BUFF[33])<<24 + ((U32)FA_FLASH_INFO_BUFF[34])<<16 + ((U32)FA_FLASH_INFO_BUFF[35])<<8 + FA_FLASH_INFO_BUFF[36];	//块地址的间隔，即Page Address的地址空间大小，2^(PA所用bit数)
    NANDFlash.LUN_ADDR_GAP = ((U32)FA_FLASH_INFO_BUFF[37])<<24 + ((U32)FA_FLASH_INFO_BUFF[38])<<16 + ((U32)FA_FLASH_INFO_BUFF[39])<<8 + FA_FLASH_INFO_BUFF[40];	//LUN地址的间隔，2^(BA所用bit数[包括plane地址]+PA所用bit数+保留位数-1）

	NANDFlash.ERASE_1st = FA_FLASH_INFO_BUFF[41];	//e.g: 60h
    NANDFlash.ERASE_2nd = FA_FLASH_INFO_BUFF[42];	//e.g:D0h
    NANDFlash.ERASE_2nd_MULTIPLANE = FA_FLASH_INFO_BUFF[43];	//e.g:D0h
    NANDFlash.ERASE_SUSPEND = FA_FLASH_INFO_BUFF[44];	//e.g:67h
    NANDFlash.ERASE_RESUME = FA_FLASH_INFO_BUFF[45];	//e.g:D7h

    NANDFlash.PROG_1st = FA_FLASH_INFO_BUFF[46];
    NANDFlash.PROG_1st_COPYBACK = FA_FLASH_INFO_BUFF[47];
    NANDFlash.PROG_1st_MULTIPLANE = FA_FLASH_INFO_BUFF[48];
    NANDFlash.PROG_2nd_SLC = FA_FLASH_INFO_BUFF[49];
	NANDFlash.PROG_2nd_TLC = FA_FLASH_INFO_BUFF[50];
    NANDFlash.PROG_2nd_TLC_CONFIRM = FA_FLASH_INFO_BUFF[51];
	NANDFlash.PROG_2nd_MULTIPLANE = FA_FLASH_INFO_BUFF[52];
   	NANDFlash.PROG_2nd_CACHE = FA_FLASH_INFO_BUFF[53];
    NANDFlash.PROG_SUSPEND = FA_FLASH_INFO_BUFF[54];
    NANDFlash.PROG_RESUME = FA_FLASH_INFO_BUFF[55];
    NANDFlash.PROG_COARSE = FA_FLASH_INFO_BUFF[56]; //YMTC Micron (QLC)

	NANDFlash.READ_1st = FA_FLASH_INFO_BUFF[57];
	NANDFlash.READ_2nd = FA_FLASH_INFO_BUFF[58];
	NANDFlash.READ_2nd_CACHE = FA_FLASH_INFO_BUFF[59];
	NANDFlash.READ_CACHE_END = FA_FLASH_INFO_BUFF[60];
	NANDFlash.READ_2nd_MULTIPLANE = FA_FLASH_INFO_BUFF[61];
	NANDFlash.READ_2nd_COPYBACK = FA_FLASH_INFO_BUFF[62];
	NANDFlash.READ_2nd_PARTIAL = FA_FLASH_INFO_BUFF[63];
    NANDFlash.READ_PRE_RETRY = FA_FLASH_INFO_BUFF[64]; //Micron
    
	NANDFlash.CHANGE_COLUMN_1st = FA_FLASH_INFO_BUFF[65];
	NANDFlash.CHANGE_COLUMN_1st_PARTIAL = FA_FLASH_INFO_BUFF[66];
	NANDFlash.CHANGE_COLUMN_2nd = FA_FLASH_INFO_BUFF[67];

	NANDFlash.RESET = FA_FLASH_INFO_BUFF[68];
	NANDFlash.RESET_LUN = FA_FLASH_INFO_BUFF[69];
	NANDFlash.RESET_SYNC = FA_FLASH_INFO_BUFF[70];
	NANDFlash.HARD_RESET = FA_FLASH_INFO_BUFF[71];

	NANDFlash.READ_ID = FA_FLASH_INFO_BUFF[72];
	NANDFlash.READ_UID = FA_FLASH_INFO_BUFF[73];
	NANDFlash.READ_PARAMETER_PAGE = FA_FLASH_INFO_BUFF[74];

	NANDFlash.READ_STATUS = FA_FLASH_INFO_BUFF[75];
	NANDFlash.READ_STATUS_ENHANCED = FA_FLASH_INFO_BUFF[76];
	NANDFlash.READ_STATUS_ENHANCED_II = FA_FLASH_INFO_BUFF[77];

	NANDFlash.SLC_ENABLE = FA_FLASH_INFO_BUFF[78];
	NANDFlash.SLC_EXIT = FA_FLASH_INFO_BUFF[79];

	NANDFlash.GET_FEATURE = FA_FLASH_INFO_BUFF[80];
	NANDFlash.SET_FEATURE = FA_FLASH_INFO_BUFF[81];
	NANDFlash.GET_FEATURE_LUN = FA_FLASH_INFO_BUFF[82];
	NANDFlash.SET_FEATURE_LUN = FA_FLASH_INFO_BUFF[83];

	NANDFlash.VOLUME_SET = FA_FLASH_INFO_BUFF[84];
	NANDFlash.ODT_CONFIG = FA_FLASH_INFO_BUFF[85];
	NANDFlash.ZQ_CALI_SHORT = FA_FLASH_INFO_BUFF[86];
	NANDFlash.ZQ_CALI_LONG = FA_FLASH_INFO_BUFF[87];
	NANDFlash.READ_DQ_TR = FA_FLASH_INFO_BUFF[88];
	NANDFlash.WRITE_DQ_TR_TX_1st = FA_FLASH_INFO_BUFF[89];
	NANDFlash.WRITE_DQ_TR_TX_2nd = FA_FLASH_INFO_BUFF[90];
	NANDFlash.WRITE_DQ_TR_RX = FA_FLASH_INFO_BUFF[91];
	NANDFlash.READ_2nd_ERASE_PAGE_CHECK = FA_FLASH_INFO_BUFF[92];


	FDL_Log("FlashInfoInit");
	int i = 0;
	for(i =0 ;i<42;i++)
		FDL_Log("%d",FA_FLASH_INFO_BUFF[i]);
	for(i =42;i<94;i++)
		FDL_Log("%h",FA_FLASH_INFO_BUFF[i]);
}

void HAL_Delay_ms(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	if(0==pCtrl->dBlkCnt)
	{
		delay_us(pCtrl->offset);
	}
	else
	{
		delay_ms(pCtrl->offset);
	}

	HostDataTransfer(DMA1_RD_TO_HOST, MEM_DMA_BUFF_ADDR, 1, JOINT_NONE);
}

void RS_Buff_Init(void)
{
	HostCmdResponse();
	MemorySet((U8 *) FA_RS_BUFFADD, 0xCC, FA_RS_BUFFSIZE>>3);
	FaRsIndx = 0;
	HostDataTransfer(DMA1_RD_TO_HOST,FA_RS_BUFFADD,1,JOINT_NONE);
}

void RS_Indx_Increase()
{
	FaRsIndx = (FaRsIndx+1)&(FA_RS_BUFFSIZE-1);
}

//bCheckBit-表示状态的bit位置 4-toshiba/sandisk SLC mode   1-other flash/mode
void RS_SaveStatusInfo(U8 Status, U8 bCheckBit)
{
	FA_RS_BUFFARY[FaRsIndx>>3]= (FA_RS_BUFFARY[FaRsIndx>>3] & Bit_Selecter[FaRsIndx&7]) | (((Status>>bCheckBit)&1)<<(7-(FaRsIndx&7)));
	RS_Indx_Increase();
}

U8 RS_CheckStatusWithSave(T_FDL_FTCTRL* pCtrl)
{
	U8 BlkIdx, bCheckBit, SRData;
	U32 dRowAdd;
	bCheckBit = 0;
	for(BlkIdx = 0; BlkIdx<(pCtrl->dPlaneCnt); ++BlkIdx)
	{
		dRowAdd = (((U16*)&(pCtrl->bBlklist[0]))[BlkIdx]) * FDL_ROW_OF_BLK;
	#if	((NAND_FLASH>=3584)&&(NAND_FLASH<4096))//Samsung 3D Flash
		HAL_CQCheckStsCfg(pCtrl->bChan, CHECK_RB_PIN, 0, pCtrl->bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
	#endif
    #if (NAND_FLASH == KHMB)
		HAL_CQCheckStsCfg(pCtrl->bChan, RD_STS_70_CMD, dRowAdd, pCtrl->bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
	#else
		HAL_CQCheckStsCfg(pCtrl->bChan, RD_STS_78_CMD, dRowAdd, pCtrl->bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
	#endif
		HAL_CQWaitDone(pCtrl->bChan);
		SRData = HAL_McuSendReadPluse(pCtrl->bChan);
		SG_FC(pCtrl->bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
		HAL_CQChipSelect(pCtrl->bChan, INVALID_U8);
		SG_FC(pCtrl->bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
		RS_SaveStatusInfo(SRData, bCheckBit);
		SG_FC(pCtrl->bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA ;
	}
	return SRData;
}

void RS_CheckStatusWithSave_2(U8 bChan, U8 bChip, U32 dRowAdd)
{
	U8 SRData;
	HAL_CQCheckStsCfg(bChan, RD_STS_78_CMD, dRowAdd, bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
	HAL_CQWaitDone(bChan);
	SRData = HAL_McuSendReadPluse(bChan);
	RS_SaveStatusInfo(SRData, 0);
}

void RS_CheckStatusWithSaveOffline(T_FDL_FTCTRL* pCtrl, U8 OperationType,U32 OperationRowAdd)
{
	U8 BlkIdx, bCheckBit, SRData;
	U32 dRowAdd;
	bCheckBit = 0;
	for(BlkIdx = 0; BlkIdx<(pCtrl->dPlaneCnt); ++BlkIdx)
	{
		dRowAdd = (((U16*)&(pCtrl->bBlklist[0]))[BlkIdx]) * FDL_ROW_OF_BLK;
#if ((NAND_FLASH>=3584)&&(NAND_FLASH<4096))//Samsung 3D Flash
		HAL_CQCheckStsCfg(pCtrl->bChan, CHECK_RB_PIN, 0, pCtrl->bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
#endif
		HAL_CQCheckStsCfg(pCtrl->bChan, RD_STS_78_CMD, dRowAdd, pCtrl->bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
		//HAL_CQCheckStsCfg(pCtrl->bChan, RD_STS_70_CMD, dRowAdd, pCtrl->bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
		HAL_CQWaitDone(pCtrl->bChan);
		SRData = HAL_McuSendReadPluse(pCtrl->bChan);
		SG_FC(pCtrl->bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
		HAL_CQChipSelect(pCtrl->bChan, INVALID_U8);
		SG_FC(pCtrl->bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
		//RS_SaveStatusInfo(SRData, bCheckBit);
		
		SG_FC(pCtrl->bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA ;
	}
}


void RS_Report(void)
{
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,FA_RS_BUFFADD,FA_RS_BUFFSIZE>>12,JOINT_NONE);
	MemorySet((U8 *) FA_RS_BUFFADD, 0xCC, FA_RS_BUFFSIZE>>3);
	FaRsIndx = 0;
}

//////bMode: 0-SLC 1-normal Mode
void FA_SendOprModeCmd(U8 bChan, U8 bMode)
{	
#if	((NAND_FLASH==YMTC_X2_6070)||(NAND_FLASH==YMTC_X3_6070)||(NAND_FLASH==MIC_B27B)||(NAND_FLASH==YMTC_X2_9060)||(NAND_FLASH==YMTC_X3_9060)||(NAND_FLASH==YMTC_X4_9060)||(NAND_FLASH==YMTC_X4_6080)||(NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV6P)||(NAND_FLASH==YMTC_X3_9070)||(NAND_FLASH==SAMSUNG_3DV7_TLC)||(NAND_FLASH == SAMSUNG_3DV8_TLC)||(NAND_FLASH == SAMSUNG_3DV4_QLC)||(NAND_FLASH == SAMSUNG_3DV5_QLC)||(NAND_FLASH == SAMSUNG_3DV7_QLC))
	if(0==bMode) //OprMod 0-SLC 1-TLC
	{
		HAL_CQSendFlashCmd(bChan, 0xDA);
	}
	else if(1==bMode)
	{
		HAL_CQSendFlashCmd(bChan, 0xDF);
	}
	else if(2==bMode)
	{
		HAL_CQSendFlashCmd(bChan, 0xDE);
	}
#elif((NAND_FLASH == INTEL_N38A)||(NAND_FLASH == INTEL_N38B)||(NAND_FLASH == INTEL_N4xA))
	if(0==bMode) //OprMod 0-SLC 1-TLC
	{
		HAL_CQSendFlashCmd(bChan, 0x3B);
	}
	else if(1==bMode)
	{
		HAL_CQSendFlashCmd(bChan, 0x3C);
	}
#elif((NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH==SanDisk_BiCS5_QLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC)||(NAND_FLASH==SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH==SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
	if(0==bMode) //OprMod 0-SLC 1-TLC
	{
		HAL_CQSendFlashCmd(bChan, 0xA2);
	}
#elif((NAND_FLASH==MIC_N48R)||(NAND_FLASH==MIC_B47R)||(NAND_FLASH==MIC_B58R)||(NAND_FLASH==MIC_N58R)||(NAND_FLASH==MIC_N69R))
	if(0==bMode) //OprMod 0-SLC 1-TLC
	{
		HAL_CQSendFlashCmd(bChan, 0x3B);//Auto Exit
	}
#elif((NAND_FLASH==MIC_B57T))
	if(0==bMode) //OprMod 0-SLC 1-TLC
	{
		HAL_CQSendFlashCmd(bChan, 0x3B);//Auto Exit
	}
	else
	{
		HAL_CQSendFlashCmd(bChan, 0xBA);//ERASE PREFIX,非SLC模式必须发
	}
#elif(NAND_FLASH==KHMB)
	if(0==bMode) //OprMod 0-SLC 1-TLC
	{
		HAL_CQSendFlashCmd(bChan, 0xA2);//Auto Exit
	}
#endif
}

/////////需要用不同编程确认指令编程UP(或XP)的flash
U8 GetPageMark(U32 PageAddr)
{
#if (NAND_FLASH==YMTC_X2_6070)
	if(PageAddr<18)
	{
		return (U8)((PageAddr%3)/2);
	}
	else if(PageAddr<1506)
	{
		return (U8)(((PageAddr-18)%4)/3);
	}
	else if(PageAddr<1542)
	{
		return (U8)(((PageAddr-1506)%3)/2);
	}
	else if(PageAddr<3030)
	{
		return (U8)(((PageAddr-1542)%4)/3);
	}
	else
	{
		return (U8)(((PageAddr-3030)%3)/2);
	}
#elif (NAND_FLASH==YMTC_X3_6070)
	if(PageAddr<18)
	{
		return (U8)((PageAddr%3)/2);
	}
	else if((PageAddr>=18)&&(PageAddr<2706))
	{
		return (U8)(((PageAddr-18)%4)/3);
	}
	else if((PageAddr>=2706)&&(PageAddr<2778))
	{
		return (U8)(((PageAddr-2706)%3)/2);
	}
	else if((PageAddr>=2778)&&(PageAddr<5466))
	{
		return (U8)(((PageAddr-2778)%4)/3);
	}
	else if((PageAddr>=5466)&&(PageAddr<5520))
	{
		return (U8)(((PageAddr-5466)%3)/2);
	}
	else//5520~5543 SLC Page
	{
		return 1;
	}
#elif (NAND_FLASH==YMTC_X4_6080)
	if(PageAddr<24)
	{
		return (U8)((PageAddr%3)/2);
	}
	else if((PageAddr>=24)&&(PageAddr<4152))
	{
		return (U8)(((PageAddr-24)%4)/3);
	}
	else if((PageAddr>=4152)&&(PageAddr<4248))
	{
		return (U8)(((PageAddr-4152)%3)/2);
	}
	else if((PageAddr>=4248)&&(PageAddr<8408))
	{
		return (U8)(((PageAddr-4248)%4)/3);
	}
	else
	{
		return (U8)(((PageAddr-8408)%3)/2);
	}

#endif
}

void GetWLAndPage(U32 PageAddr, U32 *WLAddr, U32 *PageIndex)
{
	#if((NAND_FLASH==SAMSUNG_3DV6_TLC))
		if(PageAddr < 16)
		{
			*WLAddr = PageAddr / 2;
			*PageIndex = PageAddr % 2 + 1;
		}
		else if(PageAddr >= 3064)
		{
			*WLAddr = (PageAddr - 1016) / 2;
			*PageIndex = (PageAddr - 1016) % 2 + 1;
		}
		else 
		{
			*WLAddr = (PageAddr + 8) / 3;
			*PageIndex = (PageAddr + 8) % 3;
		}
/*
        if(PageAddr < 12)
        {
            *WLAddr = PageAddr / 2;
            *PageIndex = PageAddr % 2 + 1;
        }
        else if(PageAddr >= 2280 && PageAddr < 2292)//MLC
        {
            *WLAddr = (PageAddr - 2280) / 2+762;
            *PageIndex = (PageAddr - 2280) % 2 + 1;
        }
        else if (PageAddr >= 2292)//SLC
        {
            *WLAddr = (PageAddr - 2292)+768;
            *PageIndex = 2;
        }
        else//TLC
        {
            *WLAddr = (PageAddr + 6) / 3;
            *PageIndex = (PageAddr + 6) % 3;
        }
*/
	#elif((NAND_FLASH==SAMSUNG_3DV8_TLC))
		if(PageAddr < 12)
		{
			*WLAddr = PageAddr / 2;
			*PageIndex = PageAddr % 2 + 1;
		}
		else if((PageAddr>=4224)&&(PageAddr<4236))
		{
			*WLAddr = (PageAddr - 1404) / 2;
			*PageIndex = (PageAddr - 1404) % 2 + 1;
		}
		else if(PageAddr>=4236)
		{
			*WLAddr = PageAddr - 2820;
			*PageIndex = 2;
		}
		else//PageAddr>=12 && PageAddr<4224
		{
			*WLAddr = (PageAddr + 6) / 3;
			*PageIndex = (PageAddr + 6) % 3;
		}
	#elif(((NAND_FLASH==SAMSUNG_3DV7_TLC)))
		if(PageAddr<12)
		{
			*WLAddr = PageAddr / 2;
			*PageIndex = PageAddr % 2 + 1;
		}
		else if((PageAddr>=12)&&(PageAddr<1434))
		{
			*WLAddr = (PageAddr+6)/3;
			*PageIndex = (PageAddr+6)%3;
		}
		else if((PageAddr>=1434)&&(PageAddr<1446))
		{
			*WLAddr = (PageAddr-474) / 2;
			*PageIndex = (PageAddr-474) % 2 + 1;
		}
		else if((PageAddr>=1446)&&(PageAddr<1458))
		{
			*WLAddr = PageAddr-960;
			*PageIndex = 2;
		}
		else if((PageAddr>=1458)&&(PageAddr<1470))
		{
			*WLAddr = (PageAddr-462) / 2;
			*PageIndex = (PageAddr-462) % 2 + 1;
		}
		else if((PageAddr>=1470)&&(PageAddr<3144))
		{
			*WLAddr = (PageAddr+42)/3;
			*PageIndex = (PageAddr+42)%3;
		}
		else if((PageAddr>=3144)&&(PageAddr<3156))
		{
			*WLAddr = (PageAddr-1020) / 2;
			*PageIndex = (PageAddr-1020) % 2 + 1;
		}
		else//PageAddr>=3156 && PageAddr<3167
		{
			*WLAddr = PageAddr-2088;
			*PageIndex = 2;
		}
    #elif(((NAND_FLASH == SAMSUNG_3DV6P)))
        if(PageAddr < 32)
		{
			*WLAddr = PageAddr / 2;
			*PageIndex = PageAddr % 2 + 1;
		}
		else if(PageAddr >= 3176)
		{
			*WLAddr = (PageAddr - 1048) / 2;
			*PageIndex = (PageAddr - 1048) % 2 + 1;
		}
		else
		{
			*WLAddr = (PageAddr + 16) / 3;
			*PageIndex = (PageAddr + 16) % 3;
		}
    #elif ((NAND_FLASH == SAMSUNG_3DV5_2K_MLC))
        if (PageAddr < 64)
        {
            *WLAddr = PageAddr;
			*PageIndex = PageAddr % 2 + 1;
        }
	#endif
}

/////////编程WL地址和编程次数
void GetWLAndStats(U16 PgmOrder, U16 *pPgmInfoAry)
{
#if (NAND_FLASH==YMTC_X2_6070)  //pPgmInfoAry [0]-WLAdd [1]-PgmStats(0-coarse/1-fine)
	#if 0 ///编程方式：WL间平行
		U16 LayerOrder = PgmOrder/6;
		U8 StringAdd = PgmOrder%6;
		if(LayerOrder<2)
		{
			pPgmInfoAry[0] = LayerOrder*6+StringAdd;
			pPgmInfoAry[1] = 0;
		}
		else if(LayerOrder<127)
		{
			pPgmInfoAry[0] = (LayerOrder/2+1-LayerOrder%2)*6+StringAdd;
			pPgmInfoAry[1] = LayerOrder%2;
		}
		else if(LayerOrder<128)
		{
			pPgmInfoAry[0] = 65*6+StringAdd;
			pPgmInfoAry[1] = 0;
		}
		else
		{
			pPgmInfoAry[0] = (LayerOrder/2+2-LayerOrder%2)*6+StringAdd;
			pPgmInfoAry[1] = LayerOrder%2;
		}	
	#else ///编程方式：锯齿状编程
		U16 CycleOrder = PgmOrder/12;
		U8 StringAdd = (PgmOrder%12)/2;
		U8 PgmMark = PgmOrder%2;
		if(CycleOrder<1)
		{
			pPgmInfoAry[0] = PgmOrder;
			pPgmInfoAry[1] = 0;
		}
		else if(CycleOrder<63)
		{
			pPgmInfoAry[0] = (CycleOrder+1-PgmMark)*6+StringAdd;
			pPgmInfoAry[1] = PgmMark;
		}
		else if(CycleOrder<64)
		{
			pPgmInfoAry[0] = PgmOrder - 372;
			pPgmInfoAry[1] = 0;
		}
		else
		{
			pPgmInfoAry[0] = (CycleOrder+2-PgmMark)*6+StringAdd;
			pPgmInfoAry[1] = PgmMark;
		}
	#endif
#elif (NAND_FLASH==YMTC_X3_6070)
	if(PgmOrder<12)
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 0;
	}
	else if((PgmOrder>=12) &&(PgmOrder<1356))
	{
		pPgmInfoAry[0] = PgmOrder - (2*((PgmOrder%12)/6)+PgmOrder/12-1)*6;
		pPgmInfoAry[1] = (PgmOrder%12)/6;
	}
	else if((PgmOrder>=1356) &&(PgmOrder<1380))
	{
		pPgmInfoAry[0] = PgmOrder - 672;
		pPgmInfoAry[1] = 0;
	}
	else if((PgmOrder>=1380) &&(PgmOrder<2724))
	{
		pPgmInfoAry[0] = PgmOrder - (PgmOrder/12+2*((PgmOrder%12)/6)-3)*6;
		pPgmInfoAry[1] = (PgmOrder%12)/6;
	}
	else//2724~2759
	{
		pPgmInfoAry[0] = PgmOrder - 1344;
		pPgmInfoAry[1] = 0;
	}
#elif (NAND_FLASH==YMTC_X4_6080)
	if(PgmOrder<16)
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 0;
	}
	else if((PgmOrder>=16) &&(PgmOrder<2080))
	{
		pPgmInfoAry[0] = (PgmOrder - (PgmOrder % 2)*17)/2 + 8;
		pPgmInfoAry[1] = (PgmOrder%2);
	}
	else if((PgmOrder>=2080) &&(PgmOrder<2112))
	{
		pPgmInfoAry[0] = PgmOrder - 1032;
		pPgmInfoAry[1] = 0;
	}
	else if((PgmOrder>=2112) &&(PgmOrder<4192))
	{
		pPgmInfoAry[0] = (PgmOrder - (PgmOrder%2)*17)/2 + 24;
		pPgmInfoAry[1] = (PgmOrder%2);
	}
	else//4192~4207
	{
		pPgmInfoAry[0] = PgmOrder - 2072;
		pPgmInfoAry[1] = 0;
	}
#elif (NAND_FLASH==MIC_N48R)
	U16 CycleOrder = PgmOrder/8;
	U8 PgmMark = PgmOrder%2;
	U8 StringAdd = (PgmOrder%8)/2;
	if(PgmOrder<8)//前半块前两层
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 0;
	}
	else if(PgmOrder<704)
	{
		pPgmInfoAry[0] = (CycleOrder+1-PgmMark)*4+StringAdd;
		pPgmInfoAry[1] = PgmMark;
	}
	else if(PgmOrder<712)//后半块前两层
	{
		pPgmInfoAry[0] = PgmOrder-348;
		pPgmInfoAry[1] = 0;
	}
	else
	{
		pPgmInfoAry[0] = (CycleOrder+2-PgmMark)*4+StringAdd;
		pPgmInfoAry[1] = PgmMark;
	}
#elif (NAND_FLASH==MIC_N58R)
	U16 CycleOrder = PgmOrder/8;
	U8 PgmMark = PgmOrder%2;
	U8 StringAdd = (PgmOrder%8)/2;
	if(PgmOrder<8)//前半块前两层
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 1;
	}
	else if(PgmOrder<928)
	{
		pPgmInfoAry[0] = (CycleOrder+1-PgmMark)*4+StringAdd;
		pPgmInfoAry[1] = PgmMark;
	}
	else if(PgmOrder<936)//后半块前两层
	{
		pPgmInfoAry[0] = PgmOrder-460;
		pPgmInfoAry[1] = 1;
	}
	else
	{
		pPgmInfoAry[0] = (CycleOrder+2-PgmMark)*4+StringAdd;
		pPgmInfoAry[1] = PgmMark;
	}
#elif (NAND_FLASH==MIC_N69R)
	U8 PgmMark = (PgmOrder+1)%2;
	if(PgmOrder<4)//前半块前两层
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 1;
	}
	else if(PgmOrder<1093)
	{
		pPgmInfoAry[0] = (PgmOrder+9*PgmMark-5)/2;
		pPgmInfoAry[1] = PgmMark;
	}
	else if(PgmOrder<1105)//后半块前两层
	{
		pPgmInfoAry[0] = PgmOrder-544;
		pPgmInfoAry[1] = 1;
	}
	else
	{
		pPgmInfoAry[0] = (PgmOrder+9*PgmMark+7)/2;
		pPgmInfoAry[1] = PgmMark;
	}
#elif (NAND_FLASH==INTEL_N38B)
	///////[0]-WlAddr   [1]1-1st Pass 2-2nd Pass
	U8 EvenOdd = PgmOrder%2;
	if(PgmOrder<32)//前面edgeTLC
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 1;
	}
	else if(PgmOrder<1472)
	{
		pPgmInfoAry[0] = (U16)(PgmOrder/2+32*EvenOdd-16);
		pPgmInfoAry[1] = 2-EvenOdd;
	}
	else if(PgmOrder<1520)
	{
		U16 PgmMark = (PgmOrder - 1472)%3;
		U16 StrAddr = (PgmOrder - 1472)/3;
		pPgmInfoAry[0] = (U16)(720 + StrAddr + (PgmMark==0?0:32));
		pPgmInfoAry[1] = 2 -(PgmMark%2);
	}
	else if(PgmOrder<1536)
	{
		pPgmInfoAry[0] = PgmOrder - 784;
		pPgmInfoAry[1] = 2;
	}
	else
	{
		pPgmInfoAry[0] = PgmOrder/2;
		pPgmInfoAry[1] = 1 + EvenOdd;
	}
#elif (NAND_FLASH==INTEL_N4xA)
	///////[0]-WlAddr   [1]1-1st Pass 2-2nd Pass
	U8 EvenOdd = PgmOrder%2;
	if(PgmOrder<32)//前面edgeTLC
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 1;
	}
	else if(PgmOrder<1984)
	{
		pPgmInfoAry[0] = (U16)(PgmOrder/2+32*EvenOdd-16);
		pPgmInfoAry[1] = 2-EvenOdd;
	}
	else if(PgmOrder<2032)
	{
		U16 PgmMark = (PgmOrder - 1984)%3;
		U16 StrAddr = (PgmOrder - 1984)/3;
		pPgmInfoAry[0] = (U16)(976 + StrAddr + (PgmMark==0?0:32));
		pPgmInfoAry[1] = 2 -(PgmMark%2);
	}
	else if(PgmOrder<2048)
	{
		pPgmInfoAry[0] = PgmOrder - 1040;
		pPgmInfoAry[1] = 2;
	}
	else
	{
		pPgmInfoAry[0] = PgmOrder/2;
		pPgmInfoAry[1] = 1 + EvenOdd;
	}
	

#elif (NAND_FLASH==Hynix_3DV7_QLC)
	///////[0]-WlAddr   [1]1-1st Pass 2-2nd Pass
	U8 EvenOdd = PgmOrder%2;
	if(PgmOrder>=0&&PgmOrder<=8)
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 1;
	}
	else if(PgmOrder>=9&&PgmOrder<=2807)
	{
		pPgmInfoAry[0] = (U16)((PgmOrder-9*EvenOdd)/2+4*(1-EvenOdd));
		pPgmInfoAry[1] = 1+EvenOdd;
	}
	else if(PgmOrder>=2808)
	{
		pPgmInfoAry[0] = PgmOrder - 1408;
		pPgmInfoAry[1] = 2;
	}
#elif (NAND_FLASH==SanDisk_BiCS5_QLC)
	//pPgmInfoAry[0]-WlAddr   [1]1-1st Pass 2-2nd Pass
	U8 EvenOdd = PgmOrder%2;
	if(PgmOrder<4)
	{
		pPgmInfoAry[0] = PgmOrder;
		pPgmInfoAry[1] = 1;
	}
	else if((PgmOrder>=4) && (PgmOrder<13) && (EvenOdd==0))
	{
		pPgmInfoAry[0] = PgmOrder/2+2;
		pPgmInfoAry[1] = 1;
	}
	else if((PgmOrder>=5) && (PgmOrder<13) && (EvenOdd==1))
	{
		pPgmInfoAry[0] = (PgmOrder-5)/2;
		pPgmInfoAry[1] = 2;
	}
	else if((PgmOrder>=13) && (PgmOrder<892))
	{
		pPgmInfoAry[0] = (PgmOrder-(9*EvenOdd))/2+2;
		pPgmInfoAry[1] = 1+EvenOdd;
	}
	else if(PgmOrder>=892)
	{
		pPgmInfoAry[0] = PgmOrder-448;
		pPgmInfoAry[1] = 2;
	}

#elif (NAND_FLASH==SanDisk_BiCS6_QLC)
		//pPgmInfoAry[0]-WlAddr   [1]1-1st Pass 2-2nd Pass
		U8 EvenOdd = PgmOrder%2;
		if(PgmOrder<5)
		{
			pPgmInfoAry[0] = PgmOrder;
			pPgmInfoAry[1] = 1;
		}
		else if((PgmOrder>=5) && (PgmOrder<15) && (EvenOdd==0))
		{
			pPgmInfoAry[0] = PgmOrder/2-3;
			pPgmInfoAry[1] = 2;
		}
		else if((PgmOrder>=5) && (PgmOrder<15) && (EvenOdd==1))
		{
			pPgmInfoAry[0] = (PgmOrder+5)/2;
			pPgmInfoAry[1] = 1;
		}
		else if((PgmOrder>=15) && (PgmOrder<1615))
		{
			pPgmInfoAry[0] = (PgmOrder+(11*EvenOdd))/2-3;
			pPgmInfoAry[1] = 2-EvenOdd;
		}
		else if(PgmOrder>=1615)
		{
			pPgmInfoAry[0] = PgmOrder-810;
			pPgmInfoAry[1] = 2;
		}

#elif (NAND_FLASH==SanDisk_BiCS8_QLC)
		//pPgmInfoAry[0]-WlAddr   [1]1-1st Pass 2-2nd Pass
		U8 EvenOdd = PgmOrder%2;
		if(PgmOrder<5)
		{
			pPgmInfoAry[0] = PgmOrder;
			pPgmInfoAry[1] = 1;
		}
		else if((PgmOrder>=5) && (PgmOrder<15) && (EvenOdd==0))
		{
			pPgmInfoAry[0] = PgmOrder/2-3;
			pPgmInfoAry[1] = 2;
		}
		else if((PgmOrder>=5) && (PgmOrder<15) && (EvenOdd==1))
		{
			pPgmInfoAry[0] = (PgmOrder+5)/2;
			pPgmInfoAry[1] = 1;
		}
		else if((PgmOrder>=15) && (PgmOrder<2175))
		{
			pPgmInfoAry[0] = (PgmOrder+(11*EvenOdd))/2-3;
			pPgmInfoAry[1] = 2-EvenOdd;
		}
		else if(PgmOrder>=2175)
		{
			pPgmInfoAry[0] = PgmOrder-1090;
			pPgmInfoAry[1] = 2;
		}
	
#elif (NAND_FLASH==SAMSUNG_3DV4_QLC)
   // Layer 0~1
    if (PgmOrder < 8)
    {
        pPgmInfoAry[0] = PgmOrder;
        pPgmInfoAry[1] = 1;
    }
    //层间交替
    else if (PgmOrder >= 8 && PgmOrder < 480)
    {
        U8 temp_str = (PgmOrder-8)%8;
        U8 temp_Layer = (PgmOrder-8-temp_str)/8;

        if (temp_str < 4)
        {
            pPgmInfoAry[0] = 8+temp_Layer*4+temp_str;
            pPgmInfoAry[1] = 1;
        }
        else
        {
            pPgmInfoAry[0] = 4+temp_Layer*4+temp_str-4;
            pPgmInfoAry[1] = 2;
        }
    }
    else if (PgmOrder >= 480)
    {
        pPgmInfoAry[0] = 244+ PgmOrder-480;
        pPgmInfoAry[1] = 1;
    }
#elif (NAND_FLASH==SAMSUNG_3DV5_QLC)
      // Layer 0~1
       if (PgmOrder < 8)
       {
           pPgmInfoAry[0] = PgmOrder;
           pPgmInfoAry[1] = 1;
       }
       //层间交替
       else if (PgmOrder >= 8 && PgmOrder < 712)
       {
           U8 temp_str = (PgmOrder-8)%8;
           U8 temp_Layer = (PgmOrder-8-temp_str)/8;
   
           if (temp_str < 4)
           {
               pPgmInfoAry[0] = 8+temp_Layer*4+temp_str;
               pPgmInfoAry[1] = 1;
           }
           else
           {
               pPgmInfoAry[0] = 4+temp_Layer*4+temp_str-4;
               pPgmInfoAry[1] = 2;
           }
       }
       else if (PgmOrder >= 712)
       {
           pPgmInfoAry[0] = 360+ (PgmOrder-712);
           pPgmInfoAry[1] = 1;
       }

#elif (NAND_FLASH==SAMSUNG_3DV7_QLC)
   // Layer 0~1
    if (PgmOrder < 16)
    {
        pPgmInfoAry[0] = PgmOrder;
        pPgmInfoAry[1] = 1;
    }
    else if (PgmOrder >= 16 && PgmOrder < 1248)
    {
        U8 EvenOdd = PgmOrder%2;
        if (EvenOdd==0)
        {
            pPgmInfoAry[0] = 16+(PgmOrder-16)/2;
            pPgmInfoAry[1] = 1;
        }
        else
        {
            pPgmInfoAry[0] = 8+(PgmOrder-EvenOdd-16)/2;
            pPgmInfoAry[1] = 2;
        }
    }
    else if (PgmOrder >= 1248 && PgmOrder < 1296)
    {
        pPgmInfoAry[0] = 632+(PgmOrder-1248);
        pPgmInfoAry[1] = 1;
    }
    else if (PgmOrder >= 1296 && PgmOrder < 2752)
    {
       U8 EvenOdd = PgmOrder%2;
        if (EvenOdd==0)
        {
            pPgmInfoAry[0] = 680+(PgmOrder-1296)/2;
            pPgmInfoAry[1] = 1;
        }
        else
        {
            pPgmInfoAry[0] = 672+(PgmOrder-EvenOdd-1296)/2;
            pPgmInfoAry[1] = 2;
        }
    }
    else if (PgmOrder >= 2752)
    {
        pPgmInfoAry[0] = 1408+(PgmOrder-2752);
        pPgmInfoAry[1] = 1;
    }
    
#endif
}

/////////WL起始页地址和WL类型
void GetStartPage(U16 WlAddr, U16 *pWLInfoAry)
{
#if (NAND_FLASH==YMTC_X2_6070)	 //pWLInfoAry [0]-StartPageAddr [1]-WlType(3-TLC/4-QLC/1-SLC)
	if(WlAddr<6)
	{
		pWLInfoAry[0] = (U16)(WlAddr*3);
		pWLInfoAry[1] = 3;
	}
	else if(WlAddr<378)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-6)*4)+18);
		pWLInfoAry[1] = 4;
	}
	else if(WlAddr<390)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-378)*3)+1506);
		pWLInfoAry[1] = 3;
	}
	else if(WlAddr<762)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-390)*4)+1542);
		pWLInfoAry[1] = 4;
	}
	else
	{
		pWLInfoAry[0] = (U16)(((WlAddr-762)*3)+3030);
		pWLInfoAry[1] = 3;
	}
#elif (NAND_FLASH==YMTC_X3_6070)	 //pWLInfoAry [0]-StartPageAddr [1]-WlType(3-TLC/4-QLC/1-SLC)
	if(WlAddr<6)
	{
		pWLInfoAry[0] = (U16)(WlAddr*3);
		pWLInfoAry[1] = 3;
	}
	else if((WlAddr>=6)&&(WlAddr<678))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-6)*4)+18);
		pWLInfoAry[1] = 4;
	}
	else if((WlAddr>=678)&&(WlAddr
<702))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-678)*3)+2706);
		pWLInfoAry[1] = 3;
	}
	else if((WlAddr>=702)&&(WlAddr<1374))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-702)*4)+2778);
		pWLInfoAry[1] = 4;
	}
	else if((WlAddr>=1374)&&(WlAddr<1392))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-1374)*3)+5466);
		pWLInfoAry[1] = 3;
	}
	else//WL1392~1415
	{
		pWLInfoAry[0] = (U16)((WlAddr-1392)+5520);
		pWLInfoAry[1] = 1;
	}
#elif (NAND_FLASH==YMTC_X4_6080)	 //pWLInfoAry [0]-StartPageAddr [1]-WlType(3-TLC/4-QLC/1-SLC)
	if(WlAddr<8)
	{
		pWLInfoAry[0] = (U16)(WlAddr*3);
		pWLInfoAry[1] = 3;
	}
	else if((WlAddr>=8)&&(WlAddr<1040))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-8)*4)+24);
		pWLInfoAry[1] = 4;
	}
	else if((WlAddr>=1040)&&(WlAddr<1072))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-1040)*3)+4152);
		pWLInfoAry[1] = 3;
	}
	else if((WlAddr>=1072)&&(WlAddr<2112))
	{
		pWLInfoAry[0] = (U16)(((WlAddr-1072)*4)+4248);
		pWLInfoAry[1] = 4;
	}
	else //WL2112~2136
	{
		pWLInfoAry[0] = (U16)(((WlAddr-2112)*3)+8408);
		pWLInfoAry[1] = 3;
	}
#elif (NAND_FLASH==MIC_N48R)	 //pWLInfoAry [0]-StartPageAddr [1]-WlType(0-MLC/1-QLC)
	if(WlAddr<4)
	{
		pWLInfoAry[0] = (U16)(WlAddr*2);//MLC
		pWLInfoAry[1] = 0;
	}
	else if(WlAddr<352)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-4)*4)+8);//QLC
		pWLInfoAry[1] = 1;
	}
	else if(WlAddr<360)
	{
		pWLInfoAry[0] = (U16)(WlAddr*2+696);//MLC
		pWLInfoAry[1] = 0;
	}
	else if(WlAddr<708)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-360)*4)+1416);//QLC
		pWLInfoAry[1] = 1;
	}
	else
	{
		pWLInfoAry[0] = (U16)(WlAddr*2+1392);//MLC
		pWLInfoAry[1] = 0;
	}
#elif (NAND_FLASH==MIC_N58R)	 //pWLInfoAry [0]-StartPageAddr [1]-WlType(2-MLC/4-QLC)
	if(WlAddr<4)
	{
		pWLInfoAry[0] = (U16)(WlAddr*2);//MLC
		pWLInfoAry[1] = 2;
	}
	else if(WlAddr<464)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-4)*4)+8);//QLC
		pWLInfoAry[1] = 4;
	}
	else if(WlAddr<472)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-464)*2)+1848);//MLC
		pWLInfoAry[1] = 2;
	}
	else if(WlAddr<932)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-472)*4)+1864);//QLC
		pWLInfoAry[1] = 4;
	}
	else
	{
		pWLInfoAry[0] = (U16)(((WlAddr-932)*2)+3704);//MLC
		pWLInfoAry[1] = 2;
	}
#elif (NAND_FLASH==MIC_N69R)	 //pWLInfoAry [0]-StartPageAddr [1]-WlType(2-MLC/4-QLC)
	if(WlAddr<544)
	{
		pWLInfoAry[0] = (U16)(WlAddr*4);//QLC
		pWLInfoAry[1] = 4;
	}
	else if(WlAddr<556)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-544)*2)+2176);//MLC
		pWLInfoAry[1] = 2;
	}
	else if(WlAddr<1108)
	{
		pWLInfoAry[0] = (U16)(((WlAddr-556)*4)+2200);//QLC
		pWLInfoAry[1] = 4;
	}
	else
	{
		pWLInfoAry[0] = (U16)(((WlAddr-1108)*2)+4408);//MLC
		pWLInfoAry[1] = 2;
	}

#elif(NAND_FLASH == INTEL_N38B) 	 //pWLInfoAry [0]-StartPageAddr in 1st Pass [1]-StartPageAddr in 2nd Pass [2]-WlType(0-eTLC/1-QLC)
	if(WlAddr<32)
	{
		pWLInfoAry[0] = WlAddr*1 + 0;
	}
	else if(WlAddr<753)
	{
		pWLInfoAry[0] = WlAddr*4 - 94;
	}
	else if(WlAddr<768)
	{
		pWLInfoAry[0] = WlAddr*5 - 846;
	}
	else
	{
		pWLInfoAry[0] = WlAddr*3 + 720;
	}
	if(WlAddr<32)
	{
		pWLInfoAry[2] = 0;
	}
	else if(WlAddr<752)
	{
		pWLInfoAry[2] = 1;
	}
	else
	{
		pWLInfoAry[2] = 0;
	}

	if(WlAddr<32)
	{
		pWLInfoAry[1] = pWLInfoAry[0] + pWLInfoAry[2] + WlAddr*3 + 32;
	}
	else if(WlAddr<721)
	{
		pWLInfoAry[1] = pWLInfoAry[0] + pWLInfoAry[2] + 124;
	}
	else if(WlAddr<737)
	{
		pWLInfoAry[1] = pWLInfoAry[0] + pWLInfoAry[2] + WlAddr - 596;
	}
	else if(WlAddr<752)
	{
		pWLInfoAry[1] = pWLInfoAry[0] + pWLInfoAry[2] - WlAddr*2 + 1612;
	}
	else
	{
		pWLInfoAry[1] = pWLInfoAry[0] + pWLInfoAry[2] + 1;
	}
#elif(NAND_FLASH == INTEL_N4xA) 	 //pWLInfoAry [0]-StartPageAddr in 1st Pass [1]-StartPageAddr in 2nd Pass [2]-WlType(0-eTLC/1-QLC)
	if(WlAddr<32)
	{
		pWLInfoAry[0] = WlAddr*1 + 0;
	}
	else if(WlAddr<1009)
	{
		pWLInfoAry[0] = WlAddr*4 - 94;
	}
	else if(WlAddr<1024)
	{
		pWLInfoAry[0] = WlAddr*5 - 1102;
	}
	else
	{
		pWLInfoAry[0] = WlAddr*3 + 976;
	}
	if(WlAddr<32)
	{
		pWLInfoAry[2] = 0;
	}
	else if(WlAddr<1008)
	{
		pWLInfoAry[2] = 1;
	}
	else
	{
		pWLInfoAry[2] = 0;
	}

	if(WlAddr<977)
	{
		pWLInfoAry[1] = WlAddr*4 + 32;
	}
	else if(WlAddr<993)
	{
		pWLInfoAry[1] = WlAddr*5 - 944;
	}
	else if(WlAddr<1008)
	{
		pWLInfoAry[1] = WlAddr*2 + 2032;
	}
	else if(WlAddr<1024)
	{
		pWLInfoAry[1] = WlAddr*5 - 1101;
	}
	else
	{
		pWLInfoAry[1] = WlAddr*3 + 977;
	}
#elif (NAND_FLASH == SAMSUNG_3DV4_QLC)
    if (WlAddr < 4)
    {
        pWLInfoAry[0] = WlAddr*3;
        pWLInfoAry[1] = 3;
    }
    else if (WlAddr >= 4 && WlAddr < 240)
    {
        pWLInfoAry[0] = 12+(WlAddr-4)*4;
        pWLInfoAry[1] = 4;
    }
    else if (WlAddr >= 240 && WlAddr < 256)
    {
        pWLInfoAry[0] = 956+(WlAddr-240)*3;
        pWLInfoAry[1] = 3;
    }
    else
    {
        pWLInfoAry[0] = 1004+(WlAddr-256);
        pWLInfoAry[1] = 1;
    }
#elif (NAND_FLASH == SAMSUNG_3DV5_QLC)
        if (WlAddr < 4)
        {
            pWLInfoAry[0] = WlAddr*3;
            pWLInfoAry[1] = 3;
        }
        else if (WlAddr >= 4 && WlAddr < 356)
        {
            pWLInfoAry[0] = 12+(WlAddr-4)*4;
            pWLInfoAry[1] = 4;
        }
        else if (WlAddr >= 356 && WlAddr < 368)
        {
            pWLInfoAry[0] = 1420+(WlAddr-356)*3;
            pWLInfoAry[1] = 3;
        }
        else
        {
            pWLInfoAry[0] = 1456+(WlAddr-368)*2;
            pWLInfoAry[1] = 2;
        }

#elif (NAND_FLASH == SAMSUNG_3DV7_QLC)
    if (WlAddr < 8)
    {
        pWLInfoAry[0] = WlAddr*3;
        pWLInfoAry[1] = 3;
    }
    else if (WlAddr >= 8 && WlAddr < 624)
    {
        pWLInfoAry[0] = 24+(WlAddr-8)*4;
        pWLInfoAry[1] = 4;
    }
    else if (WlAddr >= 624 && WlAddr < 648)
    {
        pWLInfoAry[0] = 2488+(WlAddr-624)*3;
        pWLInfoAry[1] = 3;
    }
    else if (WlAddr >= 648 && WlAddr < 664)
    {
        pWLInfoAry[0] = 2560+(WlAddr-648)*2;
        pWLInfoAry[1] = 2;
    }
    else if (WlAddr >= 664 && WlAddr < 672)
    {
        pWLInfoAry[0] = 2592+(WlAddr-664)*3;
        pWLInfoAry[1] = 3;
    }
    else if (WlAddr >= 672 && WlAddr < 1400)
    {
        pWLInfoAry[0] = 2616+(WlAddr-672)*4;
        pWLInfoAry[1] = 4;
    }
    else if (WlAddr >= 1400 && WlAddr < 1424)
    {
        pWLInfoAry[0] = 5528+(WlAddr-1400)*3;
        pWLInfoAry[1] = 3;
    }
    else
    {
        pWLInfoAry[0] = 5600+(WlAddr-1424)*2;
        pWLInfoAry[1] = 2;
    }
#endif
}

void MultiPlane_WritePage(T_FDL_FTCTRL *pCtrl, U32 PageAddress, U32 PageOrder)
{
	U8 bChan,bChip,BlkIdx;
	U16 *BlkList;
	U32	dRowAdd;
	U8 ConfirmCMD,PreCMD,PgmStats,UPMark;
	U8 Fast00hWrite;
	BlkList = (U16 *)(&pCtrl->bBlklist[0]);
	BlkIdx = 0;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;

	Fast00hWrite = pCtrl->dMode == 0x80 ? 1 : 0;//for all-00h data program in slc mode.
	pCtrl->dMode = (U8)(pCtrl->dMode&0x03);
	if(1 == Fast00hWrite)
	{
		SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	}
	
	//HAL_CQChipSelect(bChan, bChip);
	//HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

	// 修改版
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	
	PgmStats = (PageAddress>>16)&1;
#if	(NAND_FLASH==MIC_N48R)
	UPMark = (PageAddress>>17)&1;
#endif
	PageAddress = PageAddress&0xFFFF;

	//FDL_Log("Mode: %d", pCtrl->dMode);

	FA_SendOprModeCmd(bChan, pCtrl->dMode&0x03);
	HAL_CQSetRndSeed(bChan, PageAddress+(pCtrl->dSeedIdx), INVALID_U16);

#if	((NAND_FLASH==YMTC_X2_6070)	||(NAND_FLASH==YMTC_X3_6070)||(NAND_FLASH==YMTC_X4_6080)||(NAND_FLASH==MIC_N48R))
	if((1==pCtrl->dMode)&&(0==PgmStats))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);	//Coarse Program
	}
#endif
	PreCMD = pCtrl->dSeedBuffEn;
	HAL_CQSendFlashCmd(bChan, PreCMD);
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAddress;
	if((NAND_FLASH == KHMB) && (pCtrl->dMode == 0))
	{
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAddress*2;
	}
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
	if(Fast00hWrite == 0 || PageAddress == 0 || PreCMD != 0x85)
	{
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
		BlkIdx++;
		while(BlkIdx<(pCtrl->dPlaneCnt))
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
			HAL_CQWaitDone(bChan);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAddress;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
			BlkIdx++;
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE , JOINT_NONE, MEM_DMA2_INFO_ADDR);
		}
	}///////////SLC Fast All-00h data program: write 00h at page0, and use 85h-cmd to program
#if ((NAND_FLASH ==YMTC_X2_6070)||(NAND_FLASH ==YMTC_X3_6070)||(NAND_FLASH ==YMTC_X4_6080))
	UPMark = GetPageMark(PageAddress);
	if((0==pCtrl->dMode)||((1==pCtrl->dMode)&&(1==UPMark))||((2==pCtrl->dMode)&&(2==(PageAddress % 3))))
	{
		ConfirmCMD = FL_CMD_PAGE_PROG2;
	}
	else
	{
		ConfirmCMD = FL_CMD_ED3_PRO_CMD;
	}
#elif((NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH ==YMTC_X3_9060)||(NAND_FLASH ==YMTC_X3_9070)||(NAND_FLASH ==YMTC_X4_9060))
	if((0==pCtrl->dMode))
	{
		ConfirmCMD = FL_CMD_PAGE_PROG2;
	}
	else if(2==(PageAddress % 3))
	{
		ConfirmCMD = FL_CMD_PAGE_PROG2;
	}
	else
	{
		ConfirmCMD = FL_CMD_ED3_PRO_CMD;
	}
#else
	ConfirmCMD = pCtrl->P4;
#endif
	HAL_CQSendFlashCmd(bChan, ConfirmCMD);		//Optional Confirm CMD

	FDL_Log("%d: P%d_S%d", PageOrder, PageAddress, PgmStats);
	///////////////////////////////记录RB时间
	if(1==pCtrl->bRBMode)
	{
		FA_RecordRB( bChan, bChip, PageOrder, 0);
	}
	else
	{
		HAL_CQWaitDone(bChan);
		delay_us(1);//增加1us延时
	}
#if	((NAND_FLASH==YMTC_X2_6070)||(NAND_FLASH==YMTC_X3_6070)||(NAND_FLASH==YMTC_X4_6080)||(NAND_FLASH ==MIC_N48R)||(NAND_FLASH ==MIC_N58R)||(NAND_FLASH ==MIC_N69R))  /////////只记录实际执行编程时的状态
	if((0==pCtrl->dMode)||((1==pCtrl->dMode)&&(1==UPMark))||((2==pCtrl->dMode)&&(2==(PageAddress % 3))))
		RS_CheckStatusWithSave(pCtrl);
#elif((NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH==YMTC_X3_9060)||(NAND_FLASH==YMTC_X3_9070)||(NAND_FLASH==YMTC_X4_9060))
	if((0==pCtrl->dMode)||(2==(PageAddress % 3)))
		RS_CheckStatusWithSave(pCtrl);
#else
	RS_CheckStatusWithSave(pCtrl);
#endif
	pCtrl->dMode = (U8)(pCtrl->dMode+Fast00hWrite*0x80);
	return;
}


void MultiPlane_WritePage_SAMSUNG(T_FDL_FTCTRL *pCtrl, U32 WLAddress, U32 PageAddress, U32 PageOrder)
{
	U8 bChan,bChip,BlkIdx;
	U16 *BlkList;
	U32	dRowAdd;
	U8 ConfirmCMD,PreCMD;
    U8 PgmStats;
	BlkList = (U16 *)(&pCtrl->bBlklist[0]);
	BlkIdx = 0;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

    PgmStats = (PageAddress>>16)&1;
    PgmStats = (PageAddress>>16)&1;
	PageAddress = PageAddress&0xFFFF;


	FA_SendOprModeCmd(bChan, pCtrl->dMode);
	HAL_CQSetRndSeed(bChan, PageAddress+(pCtrl->dSeedIdx), INVALID_U16);

#if	((NAND_FLASH==SAMSUNG_3DV6_TLC))
	if(1==pCtrl->dMode)
	{
		if((PageAddress < 16 && PageOrder == 1) ||(PageAddress >= 3064 && PageOrder == 1))
		{
			Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
		}
	}
#elif	((NAND_FLASH==SAMSUNG_3DV6P))
	if(1==pCtrl->dMode)
	{
		if((PageAddress < 32 && PageOrder == 1) ||(PageAddress >= 3176 && PageOrder == 1))
		{
			Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
		}
	}
#elif((NAND_FLASH==SAMSUNG_3DV7_TLC))
	if(1==pCtrl->dMode)
	{
		if((PageAddress < 12 && PageOrder == 1) ||
			(PageAddress >= 1434 && PageAddress < 1446 &&PageOrder == 1)||
			(PageAddress >= 1458 && PageAddress < 1470 &&PageOrder == 1)||
			(PageAddress >= 3144 && PageAddress < 3156 &&PageOrder == 1))
			{
				Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
			}
	}
#elif((NAND_FLASH==SAMSUNG_3DV8_TLC))
	if(1==pCtrl->dMode)
	{
		if((PageAddress < 12 && PageOrder == 1) ||
			(PageAddress >= 4224 && PageAddress < 4236 &&PageOrder == 1))
			{
				Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
			}
	}
#elif(NAND_FLASH == SAMSUNG_3DV4_QLC)
    if(1==pCtrl->dMode)
	{
		if (PageAddress > 1003)
		{
			PageOrder = 2;
		}
	}
#endif


	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAddress;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
	BlkIdx++;
	while(BlkIdx<(pCtrl->dPlaneCnt))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1_MP);
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAddress;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
		BlkIdx++;
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE , JOINT_NONE, MEM_DMA2_INFO_ADDR);
	}
	if ((1 == pCtrl->dMode))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PAGE_PROG);
		//2Plane Flash:0x31,0x32,0x33;4Plane Flash:0xF1,0xF2,0xF3
		if(0 == PageOrder)//LSB
		{
            //FDL_Log("Program LSB %d\n", PageAddress);
			HAL_CQSendAddr(bChan, 0x01+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
		}
		else if(1 == PageOrder)//CSB
		{
            //FDL_Log("Program CSB %d\n", PageAddress);
			HAL_CQSendAddr(bChan, 0x02+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
            
		}
		else if(2 == PageOrder)//MSB
		{
            //FDL_Log("Program MSB %d\n", PageAddress);
			HAL_CQSendAddr(bChan, 0x03+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
            #if ((NAND_FLASH == SAMSUNG_3DV6P)||(NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV8_TLC)||(NAND_FLASH == SAMSUNG_3DV7_TLC))
            {
			    MultiPlane_SAMSUNG_Confirm(pCtrl,WLAddress,PageAddress);
            }
            #endif
		}
        else if (3 == PageOrder)
        {
            HAL_CQSendAddr(bChan, 0x04+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
        }
        
        #if ((NAND_FLASH == SAMSUNG_3DV4_QLC)||(NAND_FLASH == SAMSUNG_3DV5_QLC)||(NAND_FLASH == SAMSUNG_3DV7_QLC))
        {
            if (PgmStats == 1)
            {
                //FDL_Log("Confirm Order %d\n", WLAddress);
                MultiPlane_SAMSUNG_Confirm(pCtrl,WLAddress,PageAddress);
            }
        }
        #endif
	}
	else
	{
		ConfirmCMD = FL_CMD_PAGE_PROG2;
		HAL_CQSendFlashCmd(bChan, ConfirmCMD);		//Optional Confirm CMD
	}

	///////////////////////////////记录RB时间
	if(1==pCtrl->bRBMode)
	{
		FA_RecordRB( bChan, bChip, PageAddress, 0);
	}
	else
	{
		HAL_CQWaitDone(bChan);
	}
	RS_CheckStatusWithSave(pCtrl);

	return;
}


void MultiPlane_WritePage_PrefixCMD(T_FDL_FTCTRL *pCtrl, U32 PageAddress, U32 PageOrder)
{
	U8 bChan,bChip,BlkIdx,PreCMD;
	U16 *BlkList;
	U32	dRowAdd, WlAddr;
	U8 ConfirmCMD,PgmStats,UPMark,CoarseStats;
#if ((NAND_FLASH == MIC_N58R))
	U32 RBOrder;
#else
	U32 RBOrder = PageOrder;
#endif
	BlkList = (U16 *)(&pCtrl->bBlklist[0]);
	BlkIdx = 0;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

#if	  ((NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	if(1==pCtrl->dMode)
	{
		PreCMD = PageAddress%3 + 1;
		WlAddr = PageAddress/3;
		PgmStats = PreCMD/3;
	}
	else
	{
		PreCMD = 0;
		WlAddr = PageAddress;
	}
#endif

#if	  ((NAND_FLASH == Hynix_3DV7_QLC))

		if(1==pCtrl->dMode)
		{
			CoarseStats = (PageAddress>>16)&1;
			PageAddress&=0xFFFF;
			PreCMD = PageAddress%4 + 1;
			WlAddr = PageAddress/4;
			PgmStats = PreCMD/4;
			if(1==CoarseStats) HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);//Coarse Program
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAddress;
		}
#endif

#if	  ((NAND_FLASH == SanDisk_BiCS5_QLC))
		if(1==pCtrl->dMode)
		{
			CoarseStats = (PageAddress>>16)&1;
			PageAddress&=0xFFFF;
			PreCMD = PageAddress%4 + 1;
			WlAddr = PageAddress/4;
			PgmStats = PreCMD/4;
			if(1==CoarseStats) HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);//Coarse Program
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAddress;
		}
#endif

#if	  ((NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
		if(1==pCtrl->dMode)
		{
			CoarseStats = (PageAddress>>16)&1;
			PageAddress&=0xFFFF;
			PreCMD = (PageAddress%4) + 1;
			WlAddr = ((PageAddress/4)/5)*8+((PageAddress/4)%5);
			PgmStats = PreCMD/4;
			if(1==CoarseStats) HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);//Coarse Program
		}
		else
		{
			PreCMD = 0;
			WlAddr = (PageAddress/5)*8+(PageAddress%5);
		}
#endif

#if	  ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
            if(1==pCtrl->dMode)
            {
                PreCMD = PageAddress%3 + 1;
                WlAddr = (PageAddress/3)/5*8+(PageAddress/3)%5;
                PgmStats = PreCMD/3;
            }
            else
            {
                PreCMD = 0;
                WlAddr = PageAddress/5*8+PageAddress%5;
            }
#endif


#if	  ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		if(1==pCtrl->dMode)
		{
			//edge SLC
			if (PageAddress < 4 || PageAddress >2779) {
				PreCMD = 1;
				//最后一层是从WL 933, page 2780开始的。
				WlAddr = (PageAddress < 4)?PageAddress : 932+PageAddress-2780;

			}
			//edge MLC
			else if (PageAddress > 1383 && PageAddress < 1400) {
				PreCMD = (PageAddress-1384)%2 + 1;
				WlAddr = (PageAddress-1384)/2+464;

			}
			//前半部分TLC
			else if (PageAddress < 1384) {
				PreCMD = (PageAddress-4)%3+1;
				WlAddr = (PageAddress-4)/3+4;

			}
			//剩下的后半部分TLC,从page 1400开始
			else {
				PreCMD = (PageAddress-1400)%3+1;
				WlAddr = (PageAddress-1400)/3+472;

			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAddress;
		}
		PgmStats = 1;
#endif

#if	  ((NAND_FLASH == MIC_N58R))
		if(1==pCtrl->dMode)
		{
			//FDL_Log("PageAddress: %d",PageAddress);
			CoarseStats = (PageAddress>>16)&1;
			PageAddress&=0xFFFF;
			//FDL_Log("PageAddress: %d",PageAddress);
			PreCMD = PageOrder+1;
			WlAddr = PageAddress;
			if(WlAddr<4)
			{
				RBOrder = WlAddr*2+PageOrder;
				PgmStats = PreCMD/2;
			}
			else if(WlAddr<464)
			{
				RBOrder = (WlAddr-4)*4+PageOrder+8;
				PgmStats = PreCMD/4;
			}
			else if(WlAddr<472)
			{
				RBOrder = (WlAddr-464)*2+PageOrder+1848;
				PgmStats = PreCMD/2;
			}
			else if(WlAddr<932)
			{
				RBOrder = (WlAddr-472)*4+PageOrder+1864;
				PgmStats = PreCMD/4;
			}
			else
			{
				RBOrder = (WlAddr-932)*2+PageOrder+3704;
				PgmStats = PreCMD/2;
			}
			/*
			//edge MLC
			if (PageAddress < 8 || PageAddress >3703) {
				PreCMD = PageAddress%2 + 1;
				//最后一层是从WL 932, page 3704开始的。
				WlAddr = (PageAddress < 8)?(PageAddress/2) : (932+((PageAddress-3704)/2));
				PgmStats = PreCMD/2;
			}
			//edge MLC
			else if (PageAddress > 1847 && PageAddress < 1864) {
				PreCMD = (PageAddress-1848)%2 + 1;
				WlAddr = (PageAddress-1848)/2+464;
				PgmStats = PreCMD/2;
			}
			//前半部分QLC
			else if (PageAddress < 1848) {
				PreCMD = (PageAddress-8)%4+1;
				WlAddr = (PageAddress-8)/4+4;
				PgmStats = PreCMD/4;
			}
			//剩下的后半部分QLC
			else {
				PreCMD = (PageAddress-1864)%4+1;
				WlAddr = (PageAddress-1864)/4+472;
				PgmStats = PreCMD/4;
				
			}
			*/
			//HAL_CQSetRndSeed(bChan, RBOrder+(pCtrl->dSeedIdx), INVALID_U16);
			//HAL_CQWaitDone(bChan);
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAddress;
			RBOrder = PageAddress;
		}
#endif

#if	  ((NAND_FLASH == MIC_N69R))
		if(1==pCtrl->dMode)
		{
			//FDL_Log("PageAddress: %d",PageAddress);
			CoarseStats = (PageAddress>>16)&1;
			PageAddress&=0xFFFF;
			//FDL_Log("PageAddress: %d",PageAddress);
			PreCMD = pCtrl->dSectionCnt+1;
			WlAddr = PageAddress;
			PageAddress = pCtrl->dPageCnt;
			if(WlAddr<544)
			{
				RBOrder = WlAddr*2+1-CoarseStats;
				PgmStats = PreCMD/4;
			}
			else if(WlAddr<556)
			{
				RBOrder = WlAddr*2;
				PgmStats = PreCMD/2;
			}
			else if(WlAddr<1108)
			{
				RBOrder = WlAddr*2+1-CoarseStats;
				PgmStats = PreCMD/4;
			}
			else
			{
				RBOrder = WlAddr*2;
				PgmStats = PreCMD/2;
			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAddress;
			RBOrder = PageAddress;
		}
#endif

#if (NAND_FLASH == MIC_N58R)
	HAL_CQSetRndSeed(bChan, RBOrder+(pCtrl->dSeedIdx), INVALID_U16);
	FDL_Log("HAL_CQSetRndSeed: %d",(RBOrder+(pCtrl->dSeedIdx)));
#else
	HAL_CQSetRndSeed(bChan, PageAddress+(pCtrl->dSeedIdx), INVALID_U16);
#endif
	FA_SendOprModeCmd(bChan, pCtrl->dMode);
#if ((NAND_FLASH == MIC_N58R) || (NAND_FLASH == MIC_N69R))
	if(1==CoarseStats)
		HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);//Coarse Program
#endif
	//FDL_Log("PreCMD %d, WlAddr %d, PgmStats %d",PreCMD,WlAddr,PgmStats);

	if (1==pCtrl->dMode) HAL_CQSendFlashCmd(bChan, PreCMD);
	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
	BlkIdx++;
	while(BlkIdx<(pCtrl->dPlaneCnt))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

		if (1==pCtrl->dMode) HAL_CQSendFlashCmd(bChan, PreCMD);
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
		BlkIdx++;
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE , JOINT_NONE, MEM_DMA2_INFO_ADDR);
	}
	if(0==pCtrl->dMode)
	{
		ConfirmCMD = FL_CMD_PAGE_PROG2;
	}
	else if(1==PgmStats)//Last Page Of WL
	{
		ConfirmCMD = FL_CMD_ED3_PRO2_CMD;
	}
	else
	{
		ConfirmCMD = FL_CMD_ED3_PRO_CMD;
	}
	HAL_CQSendFlashCmd(bChan, ConfirmCMD);		//Optional Confirm CMD
	HAL_CQWaitDone(bChan);

	if((1==pCtrl->dMode)&&(1!=PgmStats))
	{
		return; ////非实际执行编程时，不记录RB和编程状态
	}

	///////////////////////////////记录RB时间
	if(1==pCtrl->bRBMode)
	{
#if ((NAND_FLASH == MIC_N58R))
		RBOrder = WlAddr * 2 + 1 - CoarseStats;	//RB record in WLAddr Order, 0:Coarse, 1:Fine
#endif
		FA_RecordRB( bChan, bChip, RBOrder, 0);
	}
	else
	{
		HAL_CQWaitDone(bChan);
	}
	RS_CheckStatusWithSave(pCtrl);
	return;
}


void Samsung_MultiPlane_WriteDummyPage(U8 BlkIdx,U16 *BlkList,U8 dPlaneCnt,U8 bChan,U8 bChip )
{
		U32 dRowAdd;
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK;
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
		#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
		#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
		#endif
		BlkIdx++;
		while(BlkIdx<dPlaneCnt)
		{
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
			HAL_CQWaitDone(bChan);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
			HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1_MP);
			HAL_CQSend5Addr(bChan, 0, dRowAdd );
			BlkIdx++;
		}
		HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PAGE_PROG);
		//2Plane Flash:0x31,0x32,0x33;4Plane Flash:0xF1,0xF2,0xF3
		HAL_CQSendAddr(bChan, 0x01+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
}

void MultiPlane_SAMSUNG_Confirm(T_FDL_FTCTRL* pCtrl,U16 WLAddr,U16 PageAddr)
{
	U8 bChan,bChip,BlkIdx,CfmCMD;
	U16 *BlkList,RBOrder;
	U32	dRowAdd;
	U32 Delay_us;
	BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	BlkIdx = 0;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;
	CfmCMD = pCtrl->P4;
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WLAddr;
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	HAL_CQSendFlashCmd(bChan, 0x8B);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
	BlkIdx++;
	while(BlkIdx<pCtrl->dPlaneCnt)
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
		HAL_CQSendFlashCmd(bChan, 0x8B);
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WLAddr;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
		BlkIdx++;
	}
	HAL_CQSendFlashCmd(bChan, CfmCMD);
}

void FA_WriteBlk(void)
{
	U32 i,PageIndex,PageAddr,WLAddr;
	U16 PgmCnt,PgmBegin,PgmEnd;
	U8 bChan;
	U8 bChip;
	U8 ProgramSelect;
	U16 PGMOrder;
	U8 BuffTemp[10];
	U8 Pattern;
	U8 FillData;

	//FDL_Log("Program Blk\n");
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	bChan=pCtrl->bChan;
	bChip=pCtrl->bChip;
	ProgramSelect = pCtrl->dPageAddr;
	Pattern = pCtrl->dWareIdx;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_ALG_SEL=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR + FDL_SECT_OF_PAGE*SECTOR_SIZE),  0xFF, FDL_SECT_OF_PAGE*SECTOR_SIZE);

	#if ((NAND_FLASH == YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080))
		U16 PgmInfo[2], WlInfo[2];
		if (0==(pCtrl->dMode&0x03))
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i,i);
			}
		}
		else if (1==(pCtrl->dMode&0x03))
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));//PgmInfo[0]:WL Add,[1]coarse/fine
				GetStartPage(PgmInfo[0], &(WlInfo[0]));//WLInfo[0]:start page,[1]:WL Type
				for(i = WlInfo[0]; i< (WlInfo[0]+WlInfo[1]); ++i)
				{
					//FDL_Log("%d: W%d_P%d_S%d_T%d", PGMOrder, PgmInfo[0], i, PgmInfo[1], WlInfo[1]);
					PageIndex = i;
					if((WlInfo[1]==1)||(WlInfo[1]==3)||((WlInfo[1]==4)&&(PgmInfo[1]==1))) PageIndex += 0x10000; ///Fine In QLC WL
					//if(((WlInfo[1]==1)&&(PgmInfo[1]==0))) PageIndex += 0x10000; ///fine In QLC WL
					MultiPlane_WritePage(pCtrl, PageIndex, PGMOrder);
				}

			}
		}
		else // X4-6080 TLC 模式
		{
			PgmCnt=FDL_TLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				//FDL_Log("PrmOrd: %d", i);
				MultiPlane_WritePage(pCtrl, i, i);
			}
		}	
	#elif (NAND_FLASH == KHMB)
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
		}
		for(i = 0; i < PgmCnt; i++)
		{
			MultiPlane_WritePage(pCtrl, i, i);
		}
	#elif ((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == INTEL_N38A)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X3_9070)||(NAND_FLASH == YMTC_X4_9060))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
		}
		for(i = 0; i < PgmCnt; i++)
		{
			MultiPlane_WritePage(pCtrl, i, i);
		}
	#elif ((NAND_FLASH == MIC_B47R))
		{
			if (0==pCtrl->dMode)
			{
				PgmCnt=FDL_SLC_PAGE_OF_BLK;
			}
			else
			{
				PgmCnt=FDL_PAGE_OF_BLK;
			}
			for(i = 0; i < PgmCnt; i++)
			{
				if(Pattern<8)
				{

					if(i>3 && i<1048)
						FillData = ((Pattern>>((i-4)%3))&(0x1))?(0xFF):(0x00);
					else if(i>1063 && i<2108)
						FillData = ((Pattern>>((i-1064)%3))&(0x1))?(0xFF):(0x00);
					else
						FillData = 0xFF;
					SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
					MemorySet((U8 * )(MEM_DMA_BUFF_ADDR), FillData , FDL_SECT_OF_PAGE*SECTOR_SIZE);
				}
				else if(Pattern == 8)	//LP random data, CP and MP 0xFF
				{
					if((i>3) && (i<1048) && ((i-4)%3 == 0))	//TLC WL LP
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
					else if(i>1063 && i<2108 && ((i-1064)%3 == 0)) //TLC WL LP
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
					else
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0xFF, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
				}
				MultiPlane_WritePage(pCtrl, i, i);
			}
		}		
	#elif ((NAND_FLASH == INTEL_N38B)||(NAND_FLASH == INTEL_N4xA))
		U16 PgmInfo[2], WlInfo[3];
		if (0==(pCtrl->dMode &1))
		{
			U32 PageOfs = ProgramSelect*FDL_SLC_PAGE_OF_BLK/3;
			PgmCnt=FDL_SLC_PAGE_OF_BLK/3;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i + PageOfs,i + PageOfs);
			}
		}
		else
		{
			U32 PageOfs = ProgramSelect*FDL_PAGE_OF_BLK/3;
			U8 ProgAlgSel = ((pCtrl->dMode>>7)&1); //PreRead Switch
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				U16 ProgPageTab[4];
				U8 ProgPageCnt = 0;
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				if(1 == PgmInfo[1]) //1st pass
				{
					if( 0 == WlInfo[2] )	//eTLC
					{
						ProgPageTab[0] = WlInfo[0] + PageOfs;
						ProgPageCnt = 1;
					}
					else					//QLC
					{
						ProgPageTab[0] = WlInfo[0] + PageOfs;
						ProgPageTab[1] = WlInfo[0] + 1 + PageOfs;
						ProgPageCnt = 2;
					}
				}
				else 				//2nd pass
				{
					if( 0 == WlInfo[2] )	//eTLC
					{
						ProgPageTab[0] = WlInfo[1] + PageOfs;
						ProgPageTab[1] = WlInfo[1] + 1 + PageOfs;
						ProgPageTab[2] = WlInfo[0] + PageOfs;
						ProgPageCnt = 2 + ProgAlgSel;
					}
					else					//QLC
					{
						ProgPageTab[0] = WlInfo[1] + PageOfs;
						ProgPageTab[1] = WlInfo[1] + 1 + PageOfs;
						ProgPageTab[2] = WlInfo[0] + PageOfs;
						ProgPageTab[3] = WlInfo[0] + 1 + PageOfs;
						ProgPageCnt = 2 + 2*ProgAlgSel;
					}
				}
				for(i = 0; i< ProgPageCnt; ++i)
				{
					PageIndex = ProgPageTab[i];
					MultiPlane_WritePage(pCtrl, PageIndex, PageIndex);
				}
			}
		}
	#elif ((NAND_FLASH == MIC_N48R))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i,i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				for(i = WlInfo[0]; i< (WlInfo[0]+2+2*WlInfo[1]); ++i)
				{
					PageIndex = i;
					if((WlInfo[1]==0)||((WlInfo[1]==1)&&(PgmInfo[1]==1))) PageIndex += 0x10000; ///Coarse In QLC WL
					if(i==(WlInfo[0]+1+2*WlInfo[1])) PageIndex += 0x20000; ///Last Page In WL
					MultiPlane_WritePage(pCtrl, PageIndex, PGMOrder);
				}
			}
		}
	#elif ((NAND_FLASH == MIC_N58R))
		U16 PgmInfo[2], WlInfo[2];
		U16 WL_index;	//	
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			//PgmCnt=FDL_PGM_ORDER_COUNT;
			//FDL_Log("Program Blk N58R\n");
			// Edge MLC
			for(WL_index = 0;WL_index<4;WL_index++)
			{
				for(i = 0;i<2;i++)
				{
					PageAddr = WL_index;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			//the first QLC layer of first half
			
			for(WL_index = 4;WL_index<8;WL_index++)
			{
				for(i = 0;i<4;i++)
				{
					PageAddr = WL_index;
					PageAddr+=0x10000;//Coarse In QLC WL
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			//QLC first half
			for(WL_index = 8;WL_index<464;WL_index++)
			{
				for(i = 0;i<4;i++)
				{
					PageAddr = WL_index;
					PageAddr+=0x10000;//Coarse In QLC WL
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
				//HAL_CQWaitDone(bChan);
				for(i = 0;i<4;i++)
				{
					PageAddr = (WL_index-4);
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			
			//MLC in the middle
			for(WL_index = 464;WL_index<468;WL_index++)
			{
				for(i = 0;i<2;i++)
				{
					PageAddr = WL_index;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
				//HAL_CQWaitDone(bChan);
				for(i = 0;i<4;i++)
				{
					PageAddr = (WL_index-4);
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			for(WL_index = 468;WL_index<472;WL_index++)
			{
				for(i = 0;i<2;i++)
				{
					PageAddr = WL_index;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}	
			for(WL_index = 472;WL_index<476;WL_index++)
			{
				for(i = 0;i<4;i++)
				{
					PageAddr = WL_index;
					PageAddr+=0x10000;//Coarse In QLC WL
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			for(WL_index = 476;WL_index<932;WL_index++)
			{
				for(i = 0;i<4;i++)
				{
					PageAddr = WL_index;
					PageAddr+=0x10000;//Coarse In QLC WL
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
				//HAL_CQWaitDone(bChan);
				for(i = 0;i<4;i++)
				{
					PageAddr = (WL_index-4);
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			for(WL_index = 932;WL_index<936;WL_index++)
			{
				for(i = 0;i<2;i++)
				{
					PageAddr = WL_index;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
				HAL_CQWaitDone(bChan);
				for(i = 0;i<4;i++)
				{
					PageAddr = (WL_index-4);
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
			
			/*
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				FDL_Log("Order %d, WL %d, First page %d, PgmInfo[1] %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
				for(i = WlInfo[0];i<(WlInfo[0]+WlInfo[1]);i++)
				{
					
					PageAddr = i;
					FDL_Log("Order %d, WL %d, PageAddr %d\n", PGMOrder, PgmInfo[0], PageAddr);
					if((WlInfo[1]==4)&&(PgmInfo[1]==1)) PageAddr+=0x10000;	//Coarse In QLC WL
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);			
				}
			}
			*/
		}
	#elif ((NAND_FLASH == MIC_N69R))
		U16 PgmInfo[2], WlInfo[2];
		U16 WL_index;	//	
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			//FDL_Log("Program Blk N58R\n");
			// Edge MLC

			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				//FDL_Log("Order %d, WL %d, First page %d, PgmInfo[1] %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
				for(PageIndex = 0;PageIndex<WlInfo[1];PageIndex++)
				{
					
					PageAddr = PageIndex+WlInfo[0];
					WLAddr =PgmInfo[0];

					pCtrl->dPageCnt = PageAddr;
					pCtrl->dSectionCnt = PageIndex;
					
					//FDL_Log("Order %d, WL %d, PageAddr %d\n", PGMOrder, PgmInfo[0], PageAddr);
					if((WlInfo[1]==4)&&(PgmInfo[1]==1)) WLAddr+=0x10000;	//Coarse In QLC WL
					//U8 IsCorase;
					//IsCorase = (WLAddr>>16)&1;
					//FDL_Log("Order %d, WL %d, PageAddr %d, IsCorase %d", PGMOrder, PgmInfo[0], PageAddr, IsCorase);
					MultiPlane_WritePage_PrefixCMD(pCtrl, WLAddr, PGMOrder);
				}
			}
		}
	#elif ((NAND_FLASH == Hynix_3DV6_TLC) || (NAND_FLASH == Hynix_3DV7_TLC)|| (NAND_FLASH == Hynix_3DV5_TLC)|| (NAND_FLASH == Hynix_3DV8_TLC)|| (NAND_FLASH == Hynix_3DV9_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(i = 0; i < PgmCnt; i++)
			{
				for(PageIndex = 0;PageIndex<3;++PageIndex)
				{
					PageAddr = i*3+PageIndex;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
		}
	#elif ((NAND_FLASH == Hynix_3DV7_QLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				for(PageIndex = 0;PageIndex<4;PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if(PgmInfo[1]==1) PageAddr+=0x10000;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}

		}
	#elif ((NAND_FLASH == SanDisk_BiCS5_QLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				for(PageIndex = 0;PageIndex<4;PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if(PgmInfo[1]==1) PageAddr+=0x10000;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}
		}
	#elif ((NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
			U16 PgmInfo[2];
			if (0==pCtrl->dMode)
			{
				PgmCnt=FDL_SLC_PAGE_OF_BLK;
				for(i = 0; i < PgmCnt; i++)
				{
					MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
				}
			}
			else
			{
				PgmCnt=FDL_PGM_ORDER_COUNT;
				for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
				{
					GetWLAndStats(PGMOrder, &(PgmInfo[0]));
					for(PageIndex = 0;PageIndex<4;PageIndex++)
					{
						PageAddr = PgmInfo[0]*4+PageIndex;
						if(PgmInfo[1]==1) PageAddr+=0x10000;
						MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
					}
				}
			}

    #elif ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
            PgmCnt=FDL_PGM_ORDER_COUNT;
			for(i = 0; i < PgmCnt; i++)
			{
				for(PageIndex = 0;PageIndex<3;++PageIndex)
				{
					PageAddr = i*3+PageIndex;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
		}
	#elif ((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
    #elif ((NAND_FLASH == SAMSUNG_3DV4_QLC))
       U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    //FDL_Log("page %d\n", i);
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}
    #elif ((NAND_FLASH == SAMSUNG_3DV5_QLC))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    //FDL_Log("page %d\n", i);
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}

    #elif ((NAND_FLASH == SAMSUNG_3DV7_QLC))
	    U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;//last page in WL
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}
	#elif ((NAND_FLASH == SAMSUNG_3DV8_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
	#elif ((NAND_FLASH == SAMSUNG_3DV7_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, 0);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
	#elif ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
	#endif
	//FDL_Log("WriteBlkOver");
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_WriteBlkNew(void)
{
	U32 i,PageIndex,PageAddr,WLAddr;
	U16 PgmCnt,PgmBegin,PgmEnd;
	U8 bChan;
	U8 bChip;
	U8 ProgramSelect;
	U16 PGMOrder;
	U8 BuffTemp[10];
	U8 Pattern;
	U8 FillData;


	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	bChan=pCtrl->bChan;
	bChip=pCtrl->bChip;
	ProgramSelect = pCtrl->dPageAddr;
	Pattern = pCtrl->dWareIdx;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_ALG_SEL=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR + FDL_SECT_OF_PAGE*SECTOR_SIZE),  0xFF, FDL_SECT_OF_PAGE*SECTOR_SIZE);
	/*
	while()//loop each line in Program Map
	{
		
		
	}
	*/
	#if ((NAND_FLASH == YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i,i);
			}
		}
		else if (1==(pCtrl->dMode&0x01))
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));//PgmInfo[0]:WL Add,[1]coarse/fine
				GetStartPage(PgmInfo[0], &(WlInfo[0]));//WLInfo[0]:start page,[1]:WL Type
				for(i = WlInfo[0]; i< (WlInfo[0]+WlInfo[1]); ++i)
				{
					//FDL_Log("%d: W%d_P%d_S%d_T%d", PGMOrder, PgmInfo[0], i, PgmInfo[1], WlInfo[1]);
					PageIndex = i;
					if((WlInfo[1]==1)||(WlInfo[1]==3)||((WlInfo[1]==4)&&(PgmInfo[1]==1))) PageIndex += 0x10000; ///Fine In QLC WL
					//if(((WlInfo[1]==1)&&(PgmInfo[1]==0))) PageIndex += 0x10000; ///fine In QLC WL
					MultiPlane_WritePage(pCtrl, PageIndex, PGMOrder);
				}

			}
		}
		else // X4-6080 TLC 模式
		{
			PgmCnt=FDL_TLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i, i);
			}
		}
	#elif ((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == INTEL_N38A)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X3_9070)||(NAND_FLASH == YMTC_X4_9060))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
		}
		for(i = 0; i < PgmCnt; i++)
		{
			MultiPlane_WritePage(pCtrl, i, i);
		}
	#elif ((NAND_FLASH == MIC_B47R))
		{
			if (0==pCtrl->dMode)
			{
				PgmCnt=FDL_SLC_PAGE_OF_BLK;
			}
			else
			{
				PgmCnt=FDL_PAGE_OF_BLK;
			}
			for(i = 0; i < PgmCnt; i++)
			{
				if(Pattern<8)
				{

					if(i>3 && i<1048)
						FillData = ((Pattern>>((i-4)%3))&(0x1))?(0xFF):(0x00);
					else if(i>1063 && i<2108)
						FillData = ((Pattern>>((i-1064)%3))&(0x1))?(0xFF):(0x00);
					else
						FillData = 0xFF;
					SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
					MemorySet((U8 * )(MEM_DMA_BUFF_ADDR), FillData , FDL_SECT_OF_PAGE*SECTOR_SIZE);
				}
				else if(Pattern == 8)	//LP random data, CP and MP 0xFF
				{
					if((i>3) && (i<1048) && ((i-4)%3 == 0))	//TLC WL LP
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
					else if(i>1063 && i<2108 && ((i-1064)%3 == 0)) //TLC WL LP
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
					else
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0xFF, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
				}
				MultiPlane_WritePage(pCtrl, i, i);
			}
		}
	#elif ((NAND_FLASH == INTEL_N38B)||(NAND_FLASH == INTEL_N4xA))
		U16 PgmInfo[2], WlInfo[3];
		if (0==(pCtrl->dMode &1))
		{
			U32 PageOfs = ProgramSelect*FDL_SLC_PAGE_OF_BLK/3;
			PgmCnt=FDL_SLC_PAGE_OF_BLK/3;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i + PageOfs,i + PageOfs);
			}
		}
		else
		{
			U32 PageOfs = ProgramSelect*FDL_PAGE_OF_BLK/3;
			U8 ProgAlgSel = ((pCtrl->dMode>>7)&1); //PreRead Switch
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				U16 ProgPageTab[4];
				U8 ProgPageCnt = 0;
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				if(1 == PgmInfo[1]) //1st pass
				{
					if( 0 == WlInfo[2] )	//eTLC
					{
						ProgPageTab[0] = WlInfo[0] + PageOfs;
						ProgPageCnt = 1;
					}
					else					//QLC
					{
						ProgPageTab[0] = WlInfo[0] + PageOfs;
						ProgPageTab[1] = WlInfo[0] + 1 + PageOfs;
						ProgPageCnt = 2;
					}
				}
				else 				//2nd pass
				{
					if( 0 == WlInfo[2] )	//eTLC
					{
						ProgPageTab[0] = WlInfo[1] + PageOfs;
						ProgPageTab[1] = WlInfo[1] + 1 + PageOfs;
						ProgPageTab[2] = WlInfo[0] + PageOfs;
						ProgPageCnt = 2 + ProgAlgSel;
					}
					else					//QLC
					{
						ProgPageTab[0] = WlInfo[1] + PageOfs;
						ProgPageTab[1] = WlInfo[1] + 1 + PageOfs;
						ProgPageTab[2] = WlInfo[0] + PageOfs;
						ProgPageTab[3] = WlInfo[0] + 1 + PageOfs;
						ProgPageCnt = 2 + 2*ProgAlgSel;
					}
				}
				for(i = 0; i< ProgPageCnt; ++i)
				{
					PageIndex = ProgPageTab[i];
					MultiPlane_WritePage(pCtrl, PageIndex, PageIndex);
				}
			}
		}
	#elif ((NAND_FLASH == MIC_N48R))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i,i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				for(i = WlInfo[0]; i< (WlInfo[0]+2+2*WlInfo[1]); ++i)
				{
					PageIndex = i;
					if((WlInfo[1]==0)||((WlInfo[1]==1)&&(PgmInfo[1]==1))) PageIndex += 0x10000; ///Coarse In QLC WL
					if(i==(WlInfo[0]+1+2*WlInfo[1])) PageIndex += 0x20000; ///Last Page In WL
					MultiPlane_WritePage(pCtrl, PageIndex, PGMOrder);
				}
			}
		}
	#elif ((NAND_FLASH == Hynix_3DV6_TLC) || (NAND_FLASH == Hynix_3DV7_TLC)|| (NAND_FLASH == Hynix_3DV5_TLC)|| (NAND_FLASH == Hynix_3DV8_TLC)|| (NAND_FLASH == Hynix_3DV9_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(i = 0; i < PgmCnt; i++)
			{
				for(PageIndex = 0;PageIndex<3;++PageIndex)
				{
					PageAddr = i*3+PageIndex;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
		}
	#elif ((NAND_FLASH == Hynix_3DV7_QLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				for(PageIndex = 0;PageIndex<4;PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if(PgmInfo[1]==1) PageAddr+=0x10000;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}

		}
	#elif ((NAND_FLASH == SanDisk_BiCS5_QLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				for(PageIndex = 0;PageIndex<4;PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if(PgmInfo[1]==1) PageAddr+=0x10000;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}
		}
	#elif ((NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
			U16 PgmInfo[2];
			if (0==pCtrl->dMode)
			{
				PgmCnt=FDL_SLC_PAGE_OF_BLK;
				for(i = 0; i < PgmCnt; i++)
				{
					MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
				}
			}
			else
			{
				PgmCnt=FDL_PGM_ORDER_COUNT;
				for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
				{
					GetWLAndStats(PGMOrder, &(PgmInfo[0]));
					for(PageIndex = 0;PageIndex<4;PageIndex++)
					{
						PageAddr = PgmInfo[0]*4+PageIndex;
						if(PgmInfo[1]==1) PageAddr+=0x10000;
						MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
					}
				}
			}

    #elif ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
            PgmCnt=FDL_PGM_ORDER_COUNT;
			for(i = 0; i < PgmCnt; i++)
			{
				for(PageIndex = 0;PageIndex<3;++PageIndex)
				{
					PageAddr = i*3+PageIndex;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
		}
	#elif ((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
    #elif ((NAND_FLASH == SAMSUNG_3DV4_QLC))
       U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    //FDL_Log("page %d\n", i);
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}
    #elif ((NAND_FLASH == SAMSUNG_3DV5_QLC))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    //FDL_Log("page %d\n", i);
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}

    #elif ((NAND_FLASH == SAMSUNG_3DV7_QLC))
	    U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;//last page in WL
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}
	#elif ((NAND_FLASH == SAMSUNG_3DV8_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
	#elif ((NAND_FLASH == SAMSUNG_UNKNOWN))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, 0);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
	#elif ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
	#endif
	//FDL_Log("WriteBlkOver");
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_RecordRB(U8 bChan,U8 bChip, U32 PageOrder, U8 bSPIMode)
{
	U32 time;
	U16 ADCValIdx=1;
	HAL_CQWaitDone(bChan);
	delay_us(1);//增加1us延时
	__enable_performance();
	HAL_CQCheckStsCfg(bChan, CHECK_RB_PIN, 0, bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
	//HAL_CQCheckStsCfg(bChan, RD_STS_70_CMD, 0, bChip, 0, FIO_STAT_READY, FDL_ROW_ADDR_MODE);
	//HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	HAL_CQWaitDone(bChan);
	time=__get_pfm_count();
	FA_RB_RECORD_ARRAY[PageOrder]=(time*5+1550)/1000;
	//FA_RB_RECORD_ARRAY[PageOrder]=(time*5+500)/1000;
	FDL_Log("PageOrder %d,time=%d",PageOrder,FA_RB_RECORD_ARRAY[PageOrder]);
}

void FA_WritePage_WoPreCMD(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 PmgStats,bChan,bChip,BlkIdx,PreCMD,ConfirmCMD,PagePosition;
	U16 *BlkList;
	U32	dRowAdd,PageOrder,WLAddress;
	U32 PageAdd;
	U32 UPMark,BlockAddress;
	U32 Delay_us;
	HostCmdResponse();

	BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	BlkIdx = 0;
	bChan = pCtrl->bChan;
	bChip = (U8)pCtrl->bChip&0x0F;
	#if ((NAND_FLASH == YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080)||(NAND_FLASH == MIC_N48R))
	PageAdd=(pCtrl->dPageAddr)&0xFFFF;
	PmgStats = (U8)(pCtrl->dPageAddr >> 16); //0-Coarse Program or 1-Fine Program
	#else
	PageAdd=pCtrl->dPageAddr;
	#endif
	//FDL_Log("PageAdd = %d", PageAdd);
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;

	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

	////////////////////Mode Select
	FA_SendOprModeCmd(bChan, pCtrl->dMode);

	///////////////////Page Program Command Sequence
	PreCMD = pCtrl->dSeedBuffEn;
#if  ((NAND_FLASH == YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080)||(NAND_FLASH == MIC_N48R))
	if((0==PmgStats)&&(1==pCtrl->dMode)) HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);	//Coarse Program
#endif

#if ((NAND_FLASH == SAMSUNG_3DV6_TLC))
	if((1 == pCtrl->dMode))
	{
		GetWLAndPage( PageAdd, &WLAddress, &PageOrder);
		if((PageAdd < 16 && PageAdd%2 == 0) ||(PageAdd >= 3064 && PageAdd%2 == 0))
//        if((PageAdd < 12 && PageAdd%2 == 0) ||(PageAdd >= 2280 && PageAdd < 2292 && PageAdd%2 == 0))
		Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
	}
#endif

#if ((NAND_FLASH == SAMSUNG_3DV8_TLC))
	if((1 == pCtrl->dMode))
	{
		GetWLAndPage( PageAdd, &WLAddress, &PageOrder);
		if((PageAdd < 12 && PageAdd%2 == 0) ||(PageAdd >= 4224 && PageAdd < 4236 && PageAdd%2 == 0))
		Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
	}
#endif
#if ((NAND_FLASH == SAMSUNG_3DV7_TLC))
		if((1 == pCtrl->dMode))
		{
			GetWLAndPage( PageAdd, &WLAddress, &PageOrder);
			if((PageAdd < 12 && PageAdd%2 == 0)||
				(PageAdd >= 1434 && PageAdd < 1446 && PageAdd%2 == 0)||
				(PageAdd >= 1458 && PageAdd < 1470 && PageAdd%2 == 0)||
				(PageAdd >= 3144 && PageAdd < 3156 && PageAdd%2 == 0))
				Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
		}
#endif
#if ((NAND_FLASH == SAMSUNG_3DV6P))
    if((1 == pCtrl->dMode))
    {
		GetWLAndPage( PageAdd, &WLAddress, &PageOrder);
		if((PageAdd < 32 && PageAdd%2 == 0) ||(PageAdd >= 3176 && PageAdd%2 == 0))
		Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
    }

#endif
	HAL_CQSendFlashCmd(bChan, PreCMD);
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
	if((NAND_FLASH == KHMB) && (pCtrl->dMode == 0))
	{
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd*2;
	}
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);

	BlkIdx++;
	while(BlkIdx<(pCtrl->dPlaneCnt))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

#if((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV7_TLC)||(NAND_FLASH == SAMSUNG_3DV8_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
        HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1_MP);
#else
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
#endif
        
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
		BlkIdx++;
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE , JOINT_NONE, MEM_DMA2_INFO_ADDR);
	}
#if ((NAND_FLASH ==YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080))
	UPMark = GetPageMark(PageAdd);
	if(((1==pCtrl->dMode)&&(1==UPMark))||(0==pCtrl->dMode)||((2==pCtrl->dMode)&&(2==PageAdd % 3)))
	{
		ConfirmCMD = pCtrl->P4;
	}
	else
	{
		ConfirmCMD = FL_CMD_ED3_PRO_CMD;
	}
#elif((NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH ==YMTC_X3_9060)||(NAND_FLASH ==YMTC_X3_9070)||(NAND_FLASH ==YMTC_X4_9060))
	if((2==PageAdd % 3)||(0==pCtrl->dMode))
	{
		ConfirmCMD = pCtrl->P4;
	}
	else
	{
		ConfirmCMD = FL_CMD_ED3_PRO_CMD;
	}
#elif((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV7_TLC)||(NAND_FLASH == SAMSUNG_3DV8_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
	if ((1 == pCtrl->dMode))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PAGE_PROG);
		//2Plane Flash:0x31,0x32,0x33;4Plane Flash:0xF1,0xF2,0xF3
		if(0 == PageOrder)//LSB
		{
			HAL_CQSendAddr(bChan, 0x01+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
		}
		else if(1 == PageOrder)//CSB
		{
			HAL_CQSendAddr(bChan, 0x02+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
		}
		else if(2 == PageOrder)//MSB
		{
			HAL_CQSendAddr(bChan, 0x03+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
			MultiPlane_SAMSUNG_Confirm(pCtrl,WLAddress,PageAdd);
		}
	}
	else
	{
		ConfirmCMD = pCtrl->P4;
		HAL_CQSendFlashCmd(bChan, ConfirmCMD);		//Optional Confirm CMD
	}
#else
	ConfirmCMD = pCtrl->P4;
#endif

	if((NAND_FLASH != SAMSUNG_3DV6_TLC)&&(NAND_FLASH != SAMSUNG_3DV7_TLC)&&(NAND_FLASH != SAMSUNG_3DV8_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
	{
		HAL_CQSendFlashCmd(bChan, ConfirmCMD);		//Confirm CMD
	}

	if (0<pCtrl->bRBMode)
	{
		///////////////////////////////记录RB时间
		FA_RecordRB( bChan, bChip, 0, pCtrl->bRBMode-1);
		///////////////////////////////////////////
	}
	else if(1<(pCtrl->dDelayCtrl&0x80000000))
	{
		Delay_us = ((pCtrl->dDelayCtrl&0x3FFFFFFF)%200000);
		HAL_CQWaitDone(bChan);
		delay_us(Delay_us);
#if  ((NAND_FLASH > 1023)&&(NAND_FLASH < 2048))||(NAND_FLASH>4095)	//Micron flash & YMTC flash
		HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
#else
		HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
#endif
		FDL_Log("PGM Reset Delay_us = %d", Delay_us);
	}
	else if(1<(pCtrl->dDelayCtrl&0x40000000))
	{
		Delay_us = ((pCtrl->dDelayCtrl&0x3FFFFFFF)%200000);
		HAL_CQWaitDone(bChan);
		delay_us(Delay_us);
#if ((NAND_FLASH ==YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH ==YMTC_X3_9060)||(NAND_FLASH ==YMTC_X3_9070))
		HAL_CQSendFlashCmd(bChan, 0x87);
		HAL_CQSendRowAddr_4Byte(bChan, BlkList[0] * FDL_ROW_OF_BLK);
#elif((NAND_FLASH ==YMTC_X4_9060)||(NAND_FLASH == YMTC_X4_6080))
		HAL_CQSendFlashCmd(bChan, 0x87);
		HAL_CQSend6Addr(bChan, 0, BlkList[0] * FDL_ROW_OF_BLK );///////////////6Byte Addr
#elif (NAND_FLASH==MIC_B47R)
		HAL_CQSendFlashCmd(bChan, 0x84);
		HAL_CQSendRowAddr_4Byte(bChan, BlkList[0] * FDL_ROW_OF_BLK);		
#endif
		FDL_Log("PGM Suspend Delay_us = %d", Delay_us);
	}
	else if((0x11 == ConfirmCMD) && (0==pCtrl->dMode))
	{}
	else
	{
		HAL_CQWaitDone(bChan);
	}
	if((0x11 != ConfirmCMD) || (0 != pCtrl->dMode))
	{
		RS_CheckStatusWithSave(pCtrl);
	}
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_WritePage_PreCMD(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 PmgStats,bChan,bChip,BlkIdx,PreCMD,ReadCMD,ConfirmCMD,PagePosition;
	U16 *BlkList;
	U32	dRowAdd;
	U32 PageAdd, WlAddr;
	U32 UPMark,BlockAddress;
	U32 Delay_us;
	BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	BlkIdx = 0;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;
	PageAdd=pCtrl->dPageAddr;
	PmgStats = (PageAdd>>16)&1;
	PageAdd&=0xFFFF;

////////////////////TLC Flash
#if	  ((NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	if(1==pCtrl->dMode)
	{
		PreCMD = PageAdd%3 + 1;
		WlAddr = PageAdd/3;
	}
	else
	{
		PreCMD = 0;
		WlAddr = PageAdd;
	}
#endif

#if ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
    if(1==pCtrl->dMode)
    {
        PreCMD = PageAdd%3 + 1;
        WlAddr = (PageAdd/3)/5*8+(PageAdd/3)%5;
    }
    else
    {
        PreCMD = 0;
        WlAddr = PageAdd/5*8+PageAdd%5;
    }

#endif


#if ((NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
    if(1==pCtrl->dMode)
    {
        PreCMD = PageAdd%4 + 1;
        WlAddr = (PageAdd/4)/5*8+(PageAdd/4)%5;
    }
    else
    {
        PreCMD = 0;
        WlAddr = PageAdd/5*8+PageAdd%5;
    }

#endif



#if	  ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		if(1==pCtrl->dMode)
		{
			//edge SLC
			if (PageAdd < 4 || PageAdd >2779) {
				PreCMD = 1;
				//最后一层是从WL 933, page 2780开始的。
				WlAddr = (PageAdd < 4)?PageAdd : 932+PageAdd-2780;

			}
			//edge MLC
			else if (PageAdd > 1383 && PageAdd < 1400) {
				PreCMD = (PageAdd-1384)%2 + 1;
				WlAddr = (PageAdd-1384)/2+464;

			}
			//前半部分TLC
			else if (PageAdd < 1384) {
				PreCMD = (PageAdd-4)%3+1;
				WlAddr = (PageAdd-4)/3+4;

			}
			//剩下的后半部分TLC,从page 1400开始
			else {
				PreCMD = (PageAdd-1400)%3+1;
				WlAddr = (PageAdd-1400)/3+472;

			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
		}
#endif

#if	  ((NAND_FLASH == MIC_N58R))
		U8 ConfirmStats;
		if(1==pCtrl->dMode)
		{
			//edge MLC
			if (PageAdd < 8 || PageAdd >3703) {
				PreCMD = (PageAdd%2)+1;
				//最后一层是从WL 932, page 3704开始的。
				WlAddr = (PageAdd < 8)?(PageAdd/2) : 932+(PageAdd-3704)/2;
				if(PreCMD == 2)
					ConfirmStats = 1;
			}
			//edge MLC
			else if (PageAdd > 1847 && PageAdd < 1864) {
				PreCMD = ((PageAdd-1848)%2) + 1;
				WlAddr = (PageAdd-1848)/2+464;
				if(PreCMD == 2)
					ConfirmStats = 1;
			}
			//前半部分QLC
			else if (PageAdd < 1848) {
				PreCMD = ((PageAdd-8)%4)+1;
				WlAddr = (PageAdd-8)/4+4;
				if(PreCMD == 4)
					ConfirmStats = 1;
			}
			//剩下的后半部分QLC
			else {
				PreCMD = ((PageAdd-1864)%4)+1;
				WlAddr = (PageAdd-1864)/4+472;
				if(PreCMD == 4)
					ConfirmStats = 1;
			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
			ConfirmStats = 1;
		}
#endif

#if	  ((NAND_FLASH == MIC_N69R))
		U8 ConfirmStats;
		if(1==pCtrl->dMode)
		{
			//edge MLC
			if (PageAdd >4407) {
				PreCMD = (PageAdd%2)+1;
				//最后一层是从WL 932, page 3704开始的。
				WlAddr = (PageAdd-4408)/2+1108;
				if(PreCMD == 2)
					ConfirmStats = 1;
			}
			//edge MLC
			else if (PageAdd > 2175 && PageAdd < 2200) {
				PreCMD = ((PageAdd-2176)%2) + 1;
				WlAddr = (PageAdd-2176)/2+544;
				if(PreCMD == 2)
					ConfirmStats = 1;
			}
			//前半部分QLC
			else if (PageAdd < 2176) {
				PreCMD = (PageAdd%4)+1;
				WlAddr = PageAdd/4;
				if(PreCMD == 4)
					ConfirmStats = 1;
			}
			//剩下的后半部分QLC
			else {
				PreCMD = ((PageAdd-2200)%4)+1;
				WlAddr = (PageAdd-2200)/4+556;
				if(PreCMD == 4)
					ConfirmStats = 1;
			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
			ConfirmStats = 1;
		}
#endif


#if	  ((NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC))
		if(1==pCtrl->dMode)
		{
			PreCMD = PageAdd%4 + 1;
			WlAddr = PageAdd/4;
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
		}
#endif


	HostCmdResponse();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;

	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);


	////////////////////Mode Select
	FA_SendOprModeCmd(bChan, pCtrl->dMode);
#if((NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH == SanDisk_BiCS8_QLC)||(NAND_FLASH == MIC_N58R)||(NAND_FLASH == MIC_N69R))
	if(1==PmgStats) HAL_CQSendFlashCmd(bChan, FL_CMD_COARSE_PROG);
#endif
	if(1==pCtrl->dMode)	HAL_CQSendFlashCmd(bChan, PreCMD);

	///////////////////Page Program Command Sequence
	HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);

	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;

#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);

	BlkIdx++;
	while(BlkIdx<(pCtrl->dPlaneCnt))
	{
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

		if(1==pCtrl->dMode)	HAL_CQSendFlashCmd(bChan, PreCMD);
		HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
		//HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1_MP);
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
		BlkIdx++;
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE , JOINT_NONE, MEM_DMA2_INFO_ADDR);
	}
	
#if	  ((NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
	if(0==pCtrl->dMode)
	{
		ConfirmCMD = pCtrl->P4;
	}
	else if( 3 == PreCMD)
	{
		//ConfirmCMD = pCtrl->P4;
		ConfirmCMD = FL_CMD_ED3_PRO2_CMD;
	}
	else
	{
		ConfirmCMD = FL_CMD_ED3_PRO_CMD;
	}

#endif

#if	  ((NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
		if(0==pCtrl->dMode)
		{
			ConfirmCMD = pCtrl->P4;
		}
		else if( 4 == PreCMD)
		{
			ConfirmCMD = pCtrl->P4;
		}
		else
		{
			ConfirmCMD = FL_CMD_ED3_PRO_CMD;
		}

#endif

#if	  ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		ConfirmCMD = pCtrl->P4;
#endif

#if ((NAND_FLASH == MIC_N58R)||(NAND_FLASH == MIC_N69R))
		if( ConfirmStats == 1)
		{
			ConfirmCMD = pCtrl->P4;
		}
		else
		{
			ConfirmCMD = FL_CMD_ED3_PRO_CMD;
		}

#endif


	HAL_CQSendFlashCmd(bChan, ConfirmCMD);		//Confirm CMD
	if (0<pCtrl->bRBMode)
	{
		///////////////////////////////记录RB时间
		FA_RecordRB( bChan, bChip, 0, pCtrl->bRBMode-1);
		///////////////////////////////////////////
	}
	else if(1<(pCtrl->dDelayCtrl&0x80000000))
	{
		Delay_us = ((pCtrl->dDelayCtrl&0x3FFFFFFF)%200000);
		HAL_CQWaitDone(bChan);
		delay_us(Delay_us);
		#if ((NAND_FLASH ==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH ==Hynix_3DV8_TLC)||(NAND_FLASH ==Hynix_3DV9_TLC))
			HAL_CQSendFlashCmd(bChan,FL_CMD_SYNCRESET_FLASH);
		#else
			HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
		#endif
		//FDL_Log("PGM Reset Delay_us = %d", Delay_us);
	}
	else if(1<(pCtrl->dDelayCtrl&0x40000000))
	{
		Delay_us = ((pCtrl->dDelayCtrl&0x3FFFFFFF)%200000);
		HAL_CQWaitDone(bChan);
		delay_us(Delay_us);
#if ((NAND_FLASH ==Hynix_3DV6_TLC))//TODO

#endif
		//FDL_Log("PGM Suspend Delay_us = %d", Delay_us);
	}
	else
	{
		HAL_CQWaitDone(bChan);
	}
	RS_CheckStatusWithSave(pCtrl);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_WritePage_V5_MLC(void)
{
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
    T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
    U8 PmgStats,bChan,bChip,BlkIdx,PreCMD,ConfirmCMD,PagePosition;
    U16 *BlkList;
    U32 dRowAdd,PageOrder,WLAddress;
    U32 PageAdd;
    U32 UPMark,BlockAddress;
    U32 Delay_us;
    HostCmdResponse();

    BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
    BlkIdx = 0;
    bChan = pCtrl->bChan;
    bChip = (U8)pCtrl->bChip&0x0F;

    PageAdd=pCtrl->dPageAddr;

    //FDL_Log("PageAdd = %d", PageAdd);
    SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
    SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
    SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
    SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
    SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;

    HAL_CQWaitDone(bChan);
    HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
    HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

    ////////////////////Mode Select
    FA_SendOprModeCmd(bChan, pCtrl->dMode);

    ///////////////////Page Program Command Sequence
    PreCMD = pCtrl->dSeedBuffEn;

#if ((NAND_FLASH == SAMSUNG_3DV5_2K_MLC))
    if((1 == pCtrl->dMode))
    {
            GetWLAndPage( PageAdd, &WLAddress, &PageOrder);
//    //      if((PageAdd < 16 && PageAdd%2 == 0) ||(PageAdd >= 3064 && PageAdd%2 == 0))
//            if((PageAdd < 12 && PageAdd%2 == 0) ||(PageAdd >= 2280 && PageAdd < 2292 && PageAdd%2 == 0))
//            Samsung_MultiPlane_WriteDummyPage(BlkIdx, BlkList, pCtrl->dPlaneCnt, bChan,bChip);
    }
#endif

    HAL_CQSendFlashCmd(bChan, PreCMD);

    if (PreCMD == 0x80)
    {
        dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
        HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
        HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
        HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
    
        BlkIdx++;
        while(BlkIdx<(pCtrl->dPlaneCnt))
        {
            HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG2_MP);
            HAL_CQWaitDone(bChan);
            HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
    
#if((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV7_TLC)||(NAND_FLASH == SAMSUNG_3DV8_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
            HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1_MP);
#else
            HAL_CQSendFlashCmd(bChan, FL_CMD_PAGE_PROG1);
#endif
            
            dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
        HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
        HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
            BlkIdx++;
            HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE , JOINT_NONE, MEM_DMA2_INFO_ADDR);
        }
    }
    else if (PreCMD == 0x82)
    {
        for(BlkIdx = 0; BlkIdx<8; ++BlkIdx)
        {
            dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
            HAL_CQSendRowAddr_4Byte(bChan, dRowAdd);///////////////6Byte Addr
        }

        HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
    }

#if((NAND_FLASH == SAMSUNG_3DV5_2K_MLC))
    if ((1 == pCtrl->dMode))
    {
        HAL_CQSendFlashCmd(bChan, FL_CMD_ED3_PAGE_PROG);
        //2Plane Flash:0x31,0x32,0x33;4Plane Flash:0xF1,0xF2,0xF3
        if(0 == PageOrder)//LSB
        {
            HAL_CQSendAddr(bChan, 0x01+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
        }
        else if(1 == PageOrder)//CSB
        {
            HAL_CQSendAddr(bChan, 0x02+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
        }
        else if(2 == PageOrder)//MSB
        {
            HAL_CQSendAddr(bChan, 0x03+(((1<<FDL_PLANE_OF_LUN)-1)<<4));
            MultiPlane_SAMSUNG_Confirm(pCtrl,WLAddress,PageAdd);
        }
    }
    else
    {
        ConfirmCMD = pCtrl->P4;
        HAL_CQSendFlashCmd(bChan, ConfirmCMD);      //Optional Confirm CMD
    }
#else
    ConfirmCMD = pCtrl->P4;
#endif

    if((NAND_FLASH != SAMSUNG_3DV5_2K_MLC))
    {
        HAL_CQSendFlashCmd(bChan, ConfirmCMD);      //Confirm CMD
    }

    if (0<pCtrl->bRBMode)
    {
        ///////////////////////////////记录RB时间
        FA_RecordRB( bChan, bChip, 0, pCtrl->bRBMode-1);
        ///////////////////////////////////////////
    }
    else if(1<(pCtrl->dDelayCtrl&0x80000000))
    {
        Delay_us = ((pCtrl->dDelayCtrl&0x3FFFFFFF)%200000);
        HAL_CQWaitDone(bChan);
        delay_us(Delay_us);
#if  ((NAND_FLASH > 1023)&&(NAND_FLASH < 2048))||(NAND_FLASH>4095)	//Micron flash & YMTC flash
        HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
#else
        HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
#endif
        FDL_Log("PGM Reset Delay_us = %d", Delay_us);
    }
    else if(1<(pCtrl->dDelayCtrl&0x40000000))
    {
        Delay_us = ((pCtrl->dDelayCtrl&0x3FFFFFFF)%200000);
        HAL_CQWaitDone(bChan);
        delay_us(Delay_us);
#if ((NAND_FLASH ==YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH ==YMTC_X3_9060)||(NAND_FLASH ==YMTC_X3_9070)||(NAND_FLASH ==YMTC_X4_9060)||(NAND_FLASH == YMTC_X4_6080))
        HAL_CQSendFlashCmd(bChan, 0x87);
        HAL_CQSendRowAddr_4Byte(bChan, BlkList[0] * FDL_ROW_OF_BLK);
#endif
        FDL_Log("PGM Suspend Delay_us = %d", Delay_us);
    }
    else
    {
        HAL_CQWaitDone(bChan);
    }
    RS_CheckStatusWithSave(pCtrl);
    HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_WritePage(void)
{
#if ((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_N58R)||(NAND_FLASH == MIC_N69R)||(NAND_FLASH == MIC_B57T)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
	FA_WritePage_PreCMD();
#elif ((NAND_FLASH==SAMSUNG_3DV5_2K_MLC))
    FA_WritePage_V5_MLC();
#else
	FA_WritePage_WoPreCMD();
#endif
}

void FA_SubReadData_ParameterPage(T_FDL_FTCTRL *pCtrl)
{
	U8 bChan,bChip;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	if (1==((pCtrl->dSecCnt>>8)&1))
	{
		HAL_CQSendFlashCmd(bChan, (pCtrl->dSecCnt)&0xFF);
	}
	else
	{
		HAL_CQSendFlashCmd(bChan, 0xEC);
	}
	if (1==((pCtrl->dSeedBuffEn>>8)&1))
	{
		HAL_CQSendRowAddr(bChan, (pCtrl->dSeedBuffEn)&0xFF);
	}
	else
	{
		HAL_CQSendRowAddr(bChan, 0x00);
	}

	HAL_CQWaitDone(bChan);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
//	HAL_CQSendFlashCmd(bChan, 0x00);
    HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
	HAL_CQSendColAddr(bChan, 0);
	HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);

    
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	HAL_CQWaitDone(bChan);
	HAL_CQWaitDone(bChan);
}


void FA_SubReadData_WoPreCMD(T_FDL_FTCTRL *pCtrl, U32 PageAdd, U32 PageOrder)
{
	U8 bChan,bChip,BlkIdx,BlkCnt,i,offset;
	U16 *BlkList;
	U16	wColAdd;
	U16 AppendixAddr;//////// For some flash, such as N48R, that need another address cycle to configure read option.
	U32	dRowAdd;

	BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	BlkIdx = 0;
	BlkCnt = pCtrl->dPlaneCnt;
	bChan = pCtrl->bChan;
	bChip = (U8)pCtrl->bChip&0x0F;
	wColAdd = pCtrl->dWareMode[0];
	offset = pCtrl->P4;
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
	if((NAND_FLASH == KHMB) && (pCtrl->dMode == 0))
	{
		dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd*2;
	}
	AppendixAddr = pCtrl->dWareMode[3];  ///
	HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	if (1==((pCtrl->dSecCnt>>8)&1))
	{
		FA_SendOprModeCmd(bChan, pCtrl->dMode);
		#if(NAND_FLASH == KHMB)
			HAL_CQSendFlashCmd(bChan,0x26);
			HAL_CQSendFlashCmd(bChan,0x5D);
		#elif((NAND_FLASH == MIC_B47R))
			HAL_CQSendFlashCmd(bChan,0x2E);
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );
		#elif((NAND_FLASH == MIC_N48R))
			HAL_CQSendFlashCmd(bChan,0x2E);
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );

            //为了读软解数据需要配置最优阈值
            //HAL_CQSendFlashCmd(bChan,0x2E);
			//HAL_CQSendAddr(bChan, pCtrl->P4 );
			//HAL_CQSendAddr(bChan, pCtrl->dWareMode[1] );
			//HAL_CQSendAddr(bChan, pCtrl->dWareMode[2] );
			//HAL_CQSendAddr(bChan, pCtrl->dSeedIdx );
		#endif
		HAL_CQSendFlashCmd(bChan, (pCtrl->dSecCnt)&0xFF);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
//为了读软解数据需要配置最优阈值而注释掉，下面同样
		if(1==(AppendixAddr>>8))
		{
			HAL_CQSendAddr(bChan, AppendixAddr&0xFF);
		}
		++BlkIdx;
		while(BlkIdx<BlkCnt)
		{
			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
			HAL_CQSendFlashCmd(bChan, 0x32);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
			if(1==(AppendixAddr>>8))
			{
				HAL_CQSendAddr(bChan, AppendixAddr&0xFF);
			}
			++BlkIdx;
		}
	}
	if (1==((pCtrl->dSeedBuffEn>>8)&1))
	{
		HAL_CQSendFlashCmd(bChan, (pCtrl->dSeedBuffEn)&0xFF);
	}
	if(0<pCtrl->bRBMode)
	{
		FA_RecordRB( bChan, bChip, PageOrder, pCtrl->bRBMode-1);
	}
	else
	{
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	}
	if(1>=pCtrl->bRBMode)
	{
		for(BlkIdx = 0; BlkIdx<BlkCnt; ++BlkIdx)
		{
			HAL_CQWaitDone(bChan);
//			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
			HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
			HAL_CQSendColAddr(bChan, 0);
			HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);

			HAL_CQWaitDone(bChan);
			Delay(1000);

			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR+BlkIdx*FDL_SECT_OF_PAGE*SECTOR_SIZE, DMA2_RD_TO_BUFF, 0, 0, FDL_SECT_OF_PAGE+2, JOINT_NONE, MEM_DMA2_INFO_ADDR);

//			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
//			HAL_CQChipSelect(bChan, INVALID_U8);
//			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;

			HAL_CQWaitDone(bChan);
//			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
//			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0;
//			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;

		}
	}
	HAL_CQWaitDone(bChan);
}

void FA_SubReadData_PreCMD(T_FDL_FTCTRL *pCtrl, U32 PageAdd, U32 PageOrder)
{
	U8 bChan,bChip,BlkIdx,BlkCnt,i,offset,PreCMD;
	U16 *BlkList;
	U16	wColAdd;
	U16 AppendixAddr;//////// For some flash, such as N48R, that need another address cycle to configure read option.
	U32	dRowAdd, WlAddr;
    U8 bRetry_En;

	BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	BlkIdx = 0;
	BlkCnt = pCtrl->dPlaneCnt;
	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;
	wColAdd = pCtrl->dWareMode[0];
	offset = pCtrl->P4;
	AppendixAddr = pCtrl->dWareMode[3];  //


#if	  ((NAND_FLASH == Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	if(1==pCtrl->dMode)
	{
		PreCMD = PageAdd%3 + 1;
		WlAddr = PageAdd/3;
	}
	else
	{
		PreCMD = 0;
		WlAddr = PageAdd;
	}
#endif

#if	  ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
        if(1==pCtrl->dMode)
        {
            PreCMD = PageAdd%3 + 1;
            WlAddr = (PageAdd/3)/5*8+(PageAdd/3)%5;
        }
        else
        {
            PreCMD = 0;
            WlAddr = PageAdd/5*8+PageAdd%5;
        }
#endif

#if	  ((NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
        if(1==pCtrl->dMode)
        {
            PreCMD = PageAdd%4 + 1;
            WlAddr = (PageAdd/4)/5*8+(PageAdd/4)%5;
        }
        else
        {
            PreCMD = 0;
            WlAddr = PageAdd/5*8+PageAdd%5;
        }
#endif

#if	  ((NAND_FLASH == Hynix_3DV7_QLC))
		if(1==pCtrl->dMode)
		{
			PreCMD = PageAdd%4 + 1;
			WlAddr = PageAdd/4;
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
		}
#endif
#if	  ((NAND_FLASH == SanDisk_BiCS5_QLC))
			if(1==pCtrl->dMode)
			{
				PreCMD = PageAdd%4 + 1;
				WlAddr = PageAdd/4;
			}
			else
			{
				PreCMD = 0;
				WlAddr = PageAdd;
			}
#endif
#if	  ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		if(1==pCtrl->dMode)
		{
			//edge SLC
			if (PageAdd < 4 || PageAdd >2779) {
				PreCMD = 1;
				//最后一层是从WL 933, page 2780开始的。
				WlAddr = (PageAdd < 4)?PageAdd : 932+PageAdd-2780;

			}
			//edge MLC
			else if (PageAdd > 1383 && PageAdd < 1400) {
				PreCMD = (PageAdd-1384)%2 + 1;
				WlAddr = (PageAdd-1384)/2+464;

			}
			//前半部分TLC
			else if (PageAdd < 1384) {
				PreCMD = (PageAdd-4)%3+1;
				WlAddr = (PageAdd-4)/3+4;

			}
			//剩下的后半部分TLC,从page 1400开始
			else {
				PreCMD = (PageAdd-1400)%3+1;
				WlAddr = (PageAdd-1400)/3+472;

			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
		}
#endif
#if	  ((NAND_FLASH == MIC_N58R))
		U8 IsQLCWL = 0;
		if(1==pCtrl->dMode)
		{
			//edge MLC
			if (PageAdd < 8 || PageAdd >3703) {
				PreCMD = (PageAdd%2)+1;
				//最后一层是从WL 932, page 3704开始的。
				WlAddr = (PageAdd < 8)?(PageAdd/2) : 932+(PageAdd-3704)/2;
				IsQLCWL = 0;
			}
			//edge MLC
			else if (PageAdd > 1847 && PageAdd < 1864) {
				PreCMD = ((PageAdd-1848)%2) + 1;
				WlAddr = (PageAdd-1848)/2+464;
				IsQLCWL = 0;
			}
			//前半部分QLC
			else if (PageAdd < 1848) {
				PreCMD = ((PageAdd-8)%4)+1;
				WlAddr = (PageAdd-8)/4+4;
				IsQLCWL = 1;
			}
			//剩下的后半部分QLC
			else {
				PreCMD = ((PageAdd-1864)%4)+1;
				WlAddr = (PageAdd-1864)/4+472;
				IsQLCWL = 1;
			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
		}
#endif

#if	  ((NAND_FLASH == MIC_N69R))
		U8 IsQLCWL = 0;
		if(1==pCtrl->dMode)
		{
			//edge MLC
			if (PageAdd >4407) {
				PreCMD = (PageAdd%2)+1;
				//最后一层是从WL 932, page 3704开始的。
				WlAddr = (PageAdd-4408)/2+1108;
				IsQLCWL = 0;
			}
			//edge MLC
			else if (PageAdd > 2175 && PageAdd < 2200) {
				PreCMD = ((PageAdd-2176)%2) + 1;
				WlAddr = (PageAdd-2176)/2+544;
				IsQLCWL = 0;
			}
			//前半部分QLC
			else if (PageAdd < 2176) {
				PreCMD = (PageAdd%4)+1;
				WlAddr = PageAdd/4;
				IsQLCWL = 1;
			}
			//剩下的后半部分QLC
			else {
				PreCMD = ((PageAdd-2200)%4)+1;
				WlAddr = (PageAdd-2200)/4+556;
				IsQLCWL = 1;
			}
		}
		else
		{
			PreCMD = 0;
			WlAddr = PageAdd;
		}
#endif

#if ((NAND_FLASH == TOSHIBA_FLASH)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC)) ///需要retry使能指令的flash类型
        bRetry_En = TRUE;
#else
        bRetry_En = FALSE;
#endif

//#if (NAND_FLASH == DYNAMIC_MODE_FLASH) ///每个plane都需要发模式切换指令的flash类型
//        bModeSel_En = TRUE;
//#else
//        bModeSel_En = FALSE;
//#endif


	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;
	//FDL_Log("ReadPage dRowAddr:%d", dRowAdd);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	if (1==((pCtrl->dSecCnt>>8)&1))
	{
		FA_SendOprModeCmd(bChan, pCtrl->dMode);

         if(TRUE == bRetry_En)
        {
//            if(1 == ((offset >> 0)&1))
//            {
              HAL_CQSendFlashCmd(bChan, FL_CMD_RETRY_ENABLE);
//            }
            if (1 == ((offset >> 1)&1))
            {
              HAL_CQSendFlashCmd(bChan, 0x26);
            }
            if (1 == ((offset >> 2)&1))
            {
              HAL_CQSendFlashCmd(bChan, 0x07);
        	}
        }
        
#if((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
			HAL_CQSendFlashCmd(bChan,0x2E);
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );
			HAL_CQSendAddr(bChan, offset );
#elif((NAND_FLASH == MIC_N58R))
			U8 readoffset[4];
			for(i = 0;i<4;i++)
			{
				readoffset[i] = offset;
			}
			if(1==pCtrl->dMode)
			{
				if ((IsQLCWL == 1) && (PreCMD == 3)) 
				{
					//QLC XP
					readoffset[3] = 0;
				}
				else if((IsQLCWL == 0) && (PreCMD == 1))
				{
					//MLC LP
					readoffset[1] = 0; 
					readoffset[2] = 0; 
					readoffset[3] = 0; 
				}
				else if((IsQLCWL == 0) && (PreCMD == 2))
				{
					//MLC UP
					readoffset[2] = 0; 
					readoffset[3] = 0; 
				}
			}
			else
			{
				//SLC
				readoffset[1] = 0; 
				readoffset[2] = 0; 
				readoffset[3] = 0; 
			}
			HAL_CQSendFlashCmd(bChan,0x2E);
			for(i = 0;i<4;i++)
			{
				HAL_CQSendAddr(bChan, readoffset[i]);
			}
#endif
		if(1==pCtrl->dMode)	HAL_CQSendFlashCmd(bChan, PreCMD);
		HAL_CQSendFlashCmd(bChan, (pCtrl->dSecCnt)&0xFF);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
#if((NAND_FLASH == Hynix_3DV8_TLC))
        HAL_CQSendAddr(bChan, offset );
        HAL_CQSendAddr(bChan, offset );
        HAL_CQSendAddr(bChan, offset );
        HAL_CQSendAddr(bChan, offset );
#endif

		if(1==(AppendixAddr>>8))
		{
			HAL_CQSendAddr(bChan, AppendixAddr&0xFF);
		}
		++BlkIdx;
		while(BlkIdx<BlkCnt)
		{
			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;
			HAL_CQSendFlashCmd(bChan, 0x32);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			if(1==pCtrl->dMode)	HAL_CQSendFlashCmd(bChan, PreCMD);
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
			if(1==(AppendixAddr>>8))
			{
				HAL_CQSendAddr(bChan, AppendixAddr&0xFF);
			}
			++BlkIdx;
		}
	}
	if (1==((pCtrl->dSeedBuffEn>>8)&1))
	{
		HAL_CQSendFlashCmd(bChan, (pCtrl->dSeedBuffEn)&0xFF);
	}
	if(0<pCtrl->bRBMode)
	{
		FA_RecordRB( bChan, bChip, PageOrder, pCtrl->bRBMode-1);
	}
	else
	{
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	}
	if(1>=pCtrl->bRBMode)
	{
		for(BlkIdx = 0; BlkIdx<BlkCnt; ++BlkIdx)
		{
			HAL_CQWaitDone(bChan);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + WlAddr;
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
			HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
			HAL_CQSendColAddr(bChan, 0);
			HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR+BlkIdx*FDL_SECT_OF_PAGE*SECTOR_SIZE, DMA2_RD_TO_BUFF, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
//			HAL_CQChipSelect(bChan, INVALID_U8);
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
			HAL_CQWaitDone(bChan);
		}
	}
	HAL_CQWaitDone(bChan);

}

void FA_ReadData(void)
{
	U8 i;
	U8 BlkIdx;
	U16 *BlkList;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=pCtrl->bChan;
	U8 bChip=pCtrl->bChip;
	HostCmdResponse();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_EN=1;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 2*512;

#if((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH == MIC_B57T)||(NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_N58R)||(NAND_FLASH == MIC_N69R)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC)||(NAND_FLASH == SanDisk_BiCS5_TLC))
	FA_SubReadData_PreCMD(pCtrl, pCtrl->dPageAddr, 0);
#elif((NAND_FLASH==SAMSUNG_3DV5_2K_MLC))
    FA_SubReadData_V5_MLC(pCtrl, pCtrl->dPageAddr, 0);
#else
	FA_SubReadData_WoPreCMD(pCtrl, pCtrl->dPageAddr, 0);
#endif
	if(0!=pCtrl->P4)
	{
		HAL_PMCPBitMapCountExtFilter(0, MEM_DMA_BUFF_ADDR,MEM_DMA_BUFF_ADDR+FDL_SECT_OF_PAGE*512,FDL_PAGE_SIZE,1, 1, 0, 8);
	}
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR+FDL_SECT_OF_PAGE*512,1,JOINT_NONE);
}
void FA_WriteData(void)
{
	U8 i;
	U8 BlkIdx;
	U16 *BlkList;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=pCtrl->bChan;
	U8 bChip=pCtrl->bChip;
	HostCmdResponse();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_EN=1;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 2*512;

}

void FA_SubReadData_V5_MLC(T_FDL_FTCTRL *pCtrl, U32 PageAdd, U32 PageOrder)
{
	U8 bChan,bChip,BlkIdx,BlkCnt,i,offset;
	U16 *BlkList;
	U16	wColAdd;
	U16 AppendixAddr;//////// For some flash, such as N48R, that need another address cycle to configure read option.
	U32	dRowAdd;

	BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	BlkIdx = 0;
	BlkCnt = pCtrl->dPlaneCnt;
	bChan = pCtrl->bChan;
	bChip = (U8)pCtrl->bChip&0x0F;
	wColAdd = pCtrl->dWareMode[0];
	offset = pCtrl->P4;
	dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
	AppendixAddr = pCtrl->dWareMode[3];  ///

	HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	if (1==((pCtrl->dSecCnt>>8)&1))
	{
		FA_SendOprModeCmd(bChan, pCtrl->dMode);

		HAL_CQSendFlashCmd(bChan, (pCtrl->dSecCnt)&0xFF);

        if ((pCtrl->dSecCnt)&0xFF == 0x00)
        {
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)

		    HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		    HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
        }
        else if ((pCtrl->dSecCnt)&0xFF == 0x01)
        {
            HAL_CQSendRowAddr_4Byte(bChan, dRowAdd);
        }
        else if ((pCtrl->dSecCnt)&0xFF == 0x02)
        {
            for(BlkIdx = 0; BlkIdx<8; ++BlkIdx)
		    {
    			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
                HAL_CQSendRowAddr_4Byte(bChan, dRowAdd);
            }
        }

		if(1==(AppendixAddr>>8))
		{
			HAL_CQSendAddr(bChan, AppendixAddr&0xFF);
		}
		++BlkIdx;

        if((pCtrl->dSecCnt)&0xFF == 0x00)
        {
            while(BlkIdx<BlkCnt)
    		{
    			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
    			HAL_CQSendFlashCmd(bChan, 0x32);
    			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
    			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
    			HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
    			HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
    			if(1==(AppendixAddr>>8))
    			{
    				HAL_CQSendAddr(bChan, AppendixAddr&0xFF);
    			}
    			++BlkIdx;
    		}
        }
	}
	if (1==((pCtrl->dSeedBuffEn>>8)&1))
	{
		HAL_CQSendFlashCmd(bChan, (pCtrl->dSeedBuffEn)&0xFF);
	}
	if(0<pCtrl->bRBMode)
	{
		FA_RecordRB( bChan, bChip, PageOrder, pCtrl->bRBMode-1);
	}
	else
	{
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	}
	if(1>=pCtrl->bRBMode)
	{
		for(BlkIdx = 0; BlkIdx<BlkCnt; ++BlkIdx)
		{
			HAL_CQWaitDone(bChan);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			dRowAdd = (BlkList[BlkIdx]) * FDL_ROW_OF_BLK + PageAdd;
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
			HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
			HAL_CQSendColAddr(bChan, 0);
			HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
			HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR+BlkIdx*FDL_SECT_OF_PAGE*SECTOR_SIZE, DMA2_RD_TO_BUFF, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
			HAL_CQChipSelect(bChan, INVALID_U8);
			SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
			HAL_CQWaitDone(bChan);
		}
	}
	HAL_CQWaitDone(bChan);
}

void MultiPlane_Erase(U8 bChan, U8 bChip, U16 dBlkAddr, U16 dPageaddr, U32 dMode)
{
	U32 dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + dPageaddr;

	FA_SendOprModeCmd(bChan,dMode);

	HAL_CQSendFlashCmd(bChan, FL_CMD_BLOCK_ERASE1);

	#if (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSendRowAddr_4Byte(bChan, dRowAddr);
	#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSendRowAddr(bChan, dRowAddr);
	#endif
}

void FA_ReadParameterPage(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=pCtrl->bChan;
	U8 bChip=pCtrl->bChip;
	HostCmdResponse();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_EN=1;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 2*512;

	FA_SubReadData_ParameterPage(pCtrl);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_EraseBlk(void)
{
	U8 bChan;
	U8 bChip;
	U8 EraseSelect;
	U08 bData[4] = {0,0,0,0};
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U16 *BlkList = (U16 *)(&pCtrl->bBlklist[0]);
	U8 BlkIdx = 0;
	U16 dPageaddr = 0;
	U32 dMode, Delay_us;
	SG_FC(bChan)->rCHK_ST_RB_CNT = 0xFFFFFFFF;
	HostCmdResponse();
	bChan=pCtrl->bChan;
	bChip=(U8)pCtrl->bChip&0x0F;
	dMode=pCtrl->dMode;
	EraseSelect = pCtrl->P4;
	#if((NAND_FLASH == INTEL_N38A)||(NAND_FLASH == INTEL_N38B)||(NAND_FLASH == INTEL_N4xA))

		if(0==dMode) //OprMod 0-SLC 1-TLC
		{
			dPageaddr = EraseSelect * FDL_SLC_PAGE_OF_BLK / 3;
		}
		else if(1==dMode)
		{
			dPageaddr = EraseSelect * FDL_PAGE_OF_BLK / 3;
		}

		BlkIdx = 0;
		HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
		HAL_CQWaitIdleSpace(bChan, 0x20);
		MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], dPageaddr, dMode);
		++BlkIdx;
		while (pCtrl->dPlaneCnt>BlkIdx)
		{
			MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], dPageaddr, 0xFF);
			++BlkIdx;
		}
		HAL_CQSendFlashCmd(bChan, FL_CMD_BLOCK_ERASE2);
		//HAL_CQWaitDone(bChan);
		//RS_CheckStatusWithSave(pCtrl);//此处已经RB拉高，无法暂停
			
	#else
        HAL_CQWaitDone(bChan);
		HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
		HAL_CQWaitIdleSpace(bChan, 0x20);
		MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], 0, dMode);
		++BlkIdx;
		while (pCtrl->dPlaneCnt>BlkIdx)
		{
			MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], 0, 0xFF);
			++BlkIdx;
		}
		HAL_CQSendFlashCmd(bChan, FL_CMD_BLOCK_ERASE2);
	#endif
	delay_us(1);

	if (0<pCtrl->bRBMode)
	{
		FA_RecordRB( bChan, bChip, 0, pCtrl->bRBMode-1);
	}

	if(1<(pCtrl->dDelayCtrl&0x80000000))
	{
		Delay_us = ((pCtrl->dDelayCtrl&0x7FFF)%200000);
		HAL_CQWaitDone(bChan);
		delay_us(Delay_us);
		#if  ((NAND_FLASH > 1023)&&(NAND_FLASH < 2048))||(NAND_FLASH>4095)	//Micron flash & YMTC flash
			HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
		#elif(NAND_FLASH ==Hynix_3DV7_QLC)
			HAL_CQSendFlashCmd(bChan, FL_CMD_SYNCRESET_FLASH);
		#else
			HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
		#endif
			FDL_Log("ERS Reset Delay_us = %d", Delay_us);
	}
	if(1<(pCtrl->dDelayCtrl&0x40000000))
	{
		Delay_us = ((pCtrl->dDelayCtrl&0x7FFF)%200000);
		HAL_CQWaitDone(bChan);
		delay_us(Delay_us);
		#if ((NAND_FLASH ==YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH ==YMTC_X3_9060)||(NAND_FLASH ==YMTC_X3_9070)||(NAND_FLASH ==YMTC_X4_9060)||(NAND_FLASH == YMTC_X4_6080))
			HAL_CQSendFlashCmd(bChan, 0x67);
			HAL_CQSendRowAddr_4Byte(bChan, BlkList[0] * FDL_ROW_OF_BLK);
		#else
			HAL_CQSendFlashCmd(bChan, 0x61);
		#endif
		FDL_Log("ERS Suspend Delay_us = %d", Delay_us);
	}
	RS_CheckStatusWithSave(pCtrl);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}
/*
void FA_EraseBlk_RE(void)
{
	U8 bChan;
	U8 bChip;
	U8 EraseSelect;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U16 *BlkList = (U16 *)(&pCtrl-> bBlklist[0]);
	U8 BlkIdx = 0;
	U16 dPageaddr = 0;
	U32 dMode;
	U32 Delay_us;
	SG_FC(bChan)->rCHK_ST_RB_CNT = 0xFFFFFFFF;
	HostCmdResponse();
	bChan=pCtrl->bChan;
	bChip=pCtrl->bChip;
	dMode=pCtrl->dMode;
	EraseSelect = pCtrl->P4;

	switch(NANDFlash->ERASE_TYPE):
		case 1:	//YMTC
			HAL_CQChipSelect(bChan, bChip);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
			HAL_CQWaitIdleSpace(bChan, 0x20);
			MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], 0, dMode);
			++BlkIdx;
			while (pCtrl->dPlaneCnt>BlkIdx)
			{
				MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], 0, 0xFF);
				++BlkIdx;
			}
			HAL_CQSendFlashCmd(bChan, FL_CMD_BLOCK_ERASE2);

			delay_us(1);

			if (0<pCtrl->bRBMode)
			{
				FA_RecordRB( bChan, bChip, 0, pCtrl->bRBMode-1);
			}
	
			if(1<(pCtrl->dDelayCtrl&0x80000000))
			{
				Delay_us = ((pCtrl->dDelayCtrl&0x7FFF)%200000);
				HAL_CQWaitDone(bChan);
				delay_us(Delay_us);
				HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
			}
			break;
		case 2:	//Intel SuperBlock
			if(0==dMode) //OprMod 0-SLC 1-TLC
			{
				dPageaddr = EraseSelect * FDL_SLC_PAGE_OF_BLK / 3;
			}
			else if(1==dMode)
			{
				dPageaddr = EraseSelect * FDL_PAGE_OF_BLK / 3;
			}
	
			BlkIdx = 0;
			HAL_CQChipSelect(bChan, bChip);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
			HAL_CQWaitIdleSpace(bChan, 0x20);
			MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], dPageaddr, dMode);
			++BlkIdx;
			while (pCtrl->dPlaneCnt>BlkIdx)
			{
				MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], dPageaddr, 0xFF);
				++BlkIdx;
			}
			HAL_CQSendFlashCmd(bChan, FL_CMD_BLOCK_ERASE2);

			delay_us(1);
			break;
				
		delay_us(1);
	
		if (0<pCtrl->bRBMode)
		{
			FA_RecordRB( bChan, bChip, 0, pCtrl->bRBMode-1);
		}
	
		if(1<(pCtrl->dDelayCtrl&0x80000000))
		{
			Delay_us = ((pCtrl->dDelayCtrl&0x7FFF)%200000);
			HAL_CQWaitDone(bChan);
			delay_us(Delay_us);
	#if  ((NAND_FLASH > 1023)&&(NAND_FLASH < 2048))||(NAND_FLASH>4095)	//Micron flash & YMTC flash
				HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
	#elif(NAND_FLASH == Hynix_3DV7_QLC)
				HAL_CQSendFlashCmd(bChan, FL_CMD_SYNCRESET_FLASH);
	#else
				HAL_CQSendFlashCmd(bChan, FL_CMD_RESET_FLASH);
	#endif
				FDL_Log("ERS Reset Delay_us = %d", Delay_us);
		}
		if(1<(pCtrl->dDelayCtrl&0x40000000))
		{
			Delay_us = ((pCtrl->dDelayCtrl&0x7FFF)%200000);
			HAL_CQWaitDone(bChan);
			delay_us(Delay_us);
	#if ((NAND_FLASH ==YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH ==YMTC_X2_9060)||(NAND_FLASH ==YMTC_X3_9060)||(NAND_FLASH ==YMTC_X3_9070)||(NAND_FLASH ==YMTC_X4_9060)||(NAND_FLASH == YMTC_X4_6080))
				HAL_CQSendFlashCmd(bChan, 0x67);
				HAL_CQSendRowAddr_4Byte(bChan, BlkList[0] * FDL_ROW_OF_BLK);
	#else
				HAL_CQSendFlashCmd(bChan, 0x61);
	#endif
			FDL_Log("ERS Suspend Delay_us = %d", Delay_us);
		}
		RS_CheckStatusWithSave(pCtrl);
		HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);

}
*/
void FA_EraseBlk_Type1(U8 bChan, U8 bChip, U16 *BlkList, U16 dPageaddr, U32 dMode, U8 PlaneCnt)
{
	//YMTC Micron
	U8 BlkIdx = 0;
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitIdleSpace(bChan, 0x20);
	MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], 0, dMode);
	++BlkIdx;
	while(PlaneCnt>BlkIdx)
	{
		MultiPlane_Erase(bChan, bChip, BlkList[BlkIdx], 0, 0xFF);
		++BlkIdx;
	}
	HAL_CQSendFlashCmd(bChan, NANDFlash.ERASE_2nd_MULTIPLANE);
	
	return 0;
}

void FA_Universal(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip&0x03;// pCtrl->bChip > 3 for deselect any CE
	U8 opCnt=pCtrl->bRBMode;
	U8 opIndex;
	U16 Operation;
	U8 RDDatacnt=0;
	U8 PadIOTemp,PadModeTemp;
	HAL_CQWaitDone(bChan & 3);
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR), 0, 2*SECTOR_SIZE);

	PadIOTemp = SG_PADCTRL->rPORT_MODE[15].bits.dVAL;
	PadModeTemp = SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB;
	SG_PADCTRL->rPORT_MODE[15].bits.dVAL |=(BIT_SET_1(7)); //设置为输入模式
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB = GPIO_MODE; //设置为GPIO模式

	HAL_CQChipSelect(bChan, (U8)pCtrl->bChip);
	bChan = (U8)(bChan & 3);

	for (opIndex=0;opIndex<opCnt;opIndex++)
	{
		Operation=((U16*)pCtrl->bBlklist)[opIndex];
		//FDL_Log("Operation=%03X",Operation);
		switch ((Operation>>8))
		{
			case (0):
			{
				HAL_CQSendFlashCmd(bChan, Operation);
				//FDL_Log("CMD=%02X",Operation);
				break;
			}
			case (1):
			{
				HAL_CQSendAddr(bChan,Operation);
				//FDL_Log("Add=%02X",Operation);
				break;
			}
			case (2):
			{
				if(0==(Operation&0xFF))
				{
					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_RB_READY_ERASE, FIO_PF_PASS_TLC_ERASE);
					HAL_CQSendFlashCmd(bChan, 0x00);
					//FDL_Log("RB=%02X",Operation);
				}
				else
				{
					U8 RBStatus = 0;
					U16 RBRst;
					HAL_CQWaitDone(bChan);
					__enable_performance();
					while(__get_pfm_count()<1000)
					{
						if(0==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
						{
							RBStatus = 1;
							break;
						}
					}
					if(RBStatus == 1)
					{
						while(__get_pfm_count()<1200000) //~6ms over time
						{
							if(1==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
							{
								break;
							}
						}
						RBRst = (__get_pfm_count()+10)/20 + 2;
					}
					((U16 *)(MEM_DMA_BUFF_ADDR))[201] = RBStatus;
					((U16 *)(MEM_DMA_BUFF_ADDR))[200] = (RBRst > 60000) ? 0xFFFF : RBRst;
				}
				break;
			}
			case (3):
			{
				HAL_CQWaitDone(bChan);
				SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x00;
				HAL_McuSendWRPulse(bChan,Operation);
				//FDL_Log("WRData=%02X",Operation);
				break;
			}
			case (4):
			{
				U8 WrStat = 0;
				if((opIndex==0)||((((U16*)pCtrl->bBlklist)[opIndex-1])>>8)!=4)//1st dqs wr
					WrStat+=0x10;
				if((opIndex==opCnt-1)||((((U16*)pCtrl->bBlklist)[opIndex+1])>>8)!=4)//last dqs wr
					WrStat+=0x01;

//				FDL_Log("opIndex=%d",opIndex);
//				FDL_Log("WrStat=%d",WrStat);
//				FDL_Log("pre:%d",((U16*)pCtrl->bBlklist)[opIndex-1]);
//				FDL_Log("lst:%d",((U16*)pCtrl->bBlklist)[opIndex+1]);

				HAL_CQWaitDone(bChan);
				HAL_MCUSendDataByDqs_GAI(bChan,Operation,WrStat);
				//HAL_MCUSendDataByDqs(bChan,Operation);
				//FDL_Log("DQSData=%02X",Operation);
				break;
			}
			case (5):
			{
				HAL_CQWaitDone(bChan);
				SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x00;
				((U8 *)(MEM_DMA_BUFF_ADDR))[RDDatacnt]=HAL_McuSendReadPluse(bChan);
				RDDatacnt++;
				break;
			}
			case (6):
			{
				HAL_CQWaitDone(bChan);
				SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
				SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = (Operation&0xFF);
				HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, 0, 0, 1 , JOINT_NONE, MEM_DMA2_INFO_ADDR);
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
				HAL_CQChipSelect(bChan, INVALID_U8);
				SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
				break;
			}
			//case 7:9081电流采样
			//case 8:ALE读脉冲
			case (9): ///delay us
			{
				U16 Dly_us = ((U16*)pCtrl->bBlklist)[++opIndex];
				delay_us(Dly_us);
				break;
			}
			case (10): ///Random data
			{
				SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
				SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_ALG_SEL=0;
				SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
				MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);

				HAL_CQSetRndSeed(bChan, Operation%256, INVALID_U16);
				HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
				HAL_CQWaitDone(bChan);
				break;
			}
			case (11): // 拉高ALE
			{
				U8 WrStat = 0;
				if((Operation & 0x0F) == 14)//1st dqs wr
					WrStat+=0x10;
				if((Operation & 0x0F) == 15)//last dqs wr
					WrStat+=0x01;

//				FDL_Log("opIndex=%d",opIndex);
//				FDL_Log("WrStat=%d",WrStat);
//				FDL_Log("pre:%d",((U16*)pCtrl->bBlklist)[opIndex-1]);
//				FDL_Log("lst:%d",((U16*)pCtrl->bBlklist)[opIndex+1]);

				HAL_CQWaitDone(bChan);
				if(0x10==(WrStat&0xF0))////Set GPIO Output Mode & Pull DQS Down, Preamble
				{
					//DQ GPIO
					SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(1);
					SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(1);
					__nds32__dsb();
					__nds32__dsb();
					SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(1);
					__nds32__dsb();
					SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = GPIO_MODE;
					__nds32__dsb();
				}

				if(0x01==(WrStat&0x0F))////Postamble
				{
					/////Pull ALE & DQS High
					//SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = GPIO_MODE;
					//__nds32__dsb();
					//SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(1);
					//__nds32__dsb();
					SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = FUCN_MODE;
				}
				//HAL_MCUSendDataByDqs(bChan,Operation);
				//FDL_Log("DQSData=%02X",Operation);
				break;
			}
			default:
			break;
		}
	}
	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;

	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB = PadModeTemp;
	SG_PADCTRL->rPORT_MODE[15].bits.dVAL = PadIOTemp;

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_SetFeature(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 bChan=(U8)pCtrl->dClstCnt;
	U8 bChip=(U8)pCtrl->dMaskEn;
	U8 data[4];
	U8 Addr;

	//HAL_CQWaitDone(bChan);
//    HAL_MCUSendCER_CE(bChan, bChip, 0);
    HAL_CQChipSelect(bChan, bChip);
	HAL_CQWaitDone(bChan);


    
//	HAL_MCU_SetCe(bChan,bChip,1);
	Addr = pCtrl->FeatureAddress;
	data[0]=pCtrl->P1;
	data[1]=pCtrl->P2;
	data[2]=pCtrl->P3;
	data[3]=pCtrl->P4;
	HAL_NFSetFeature(bChan, bChip, Addr, &data[0], 1);
    HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);

//  HAL_CQSendFlashCmd(bChan,FL_CMD_SET_FEATURE);
//  HAL_CQSendAddr(bChan,bAddr);
//    HAL_CQWaitDone(bChan);
//    HAL_MCUCheckStatus(bChan, bChip);       //需要等RB Done
//    HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
//    HAL_MCU_SendOneAddr(bChan,Addr);
//    Delay(4);
//
//    SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 8;
//    ((U8*)MEM_DMA_BUFF_ADDR)[0] = data[0];
//    ((U8*)MEM_DMA_BUFF_ADDR)[1] = data[0];
//    ((U8*)MEM_DMA_BUFF_ADDR)[2] = data[1];
//    ((U8*)MEM_DMA_BUFF_ADDR)[3] = data[1];
//    ((U8*)MEM_DMA_BUFF_ADDR)[4] = data[2];
//    ((U8*)MEM_DMA_BUFF_ADDR)[5] = data[2];
//    ((U8*)MEM_DMA_BUFF_ADDR)[6] = data[3];
//    ((U8*)MEM_DMA_BUFF_ADDR)[7] = data[3];
//    HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_WR_TO_FLASH, 0, 0, 1, JOINT_NONE, MEM_DMA2_INFO_ADDR);
//    HAL_CQCheckIOStatus(bChan, 0, IO_1_PASS, FIO_RB_READY_INIT, FIO_PF_PASS_NULL);
//    HAL_CQWaitDone(bChan);


	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_SetCE(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 bChan = (U8)pCtrl->dClstCnt;
	U8 bChip = (U8)pCtrl->dMaskEn;
	U8 Val = pCtrl->P1;
	HAL_CQWaitDone(bChan);
	//HAL_MCU_SetCe(bChan,bChip,Val);
	HAL_CQChipSelect(bChan, bChip);
//    HAL_MCUSendCER_CE(bChan,  bChip, 0);
    
	HAL_CQWaitDone(bChan);
	HAL_McuSendWRPulse(bChan,bChan);
	HAL_McuSendWRPulse(bChan,bChip);
	HAL_McuSendWRPulse(bChan,bChan);
	HAL_McuSendWRPulse(bChan,bChip);
	//HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_GetFeature(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip;
	U08 bData[6] = {0,0,0,0,0};
	U8 P1;
	U8 P2;
	U8 P3;
	U8 P4;
	U8 i;
	U8* feature, bAddr;
	feature = (U8 *)MEM_DMA_BUFF_ADDR;
	bAddr = pCtrl->FeatureAddress;

//    HAL_MCUSendCER_CE(bChan, bChip, 0);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQWaitDone(bChan);
	HAL_NFGetFeature(bChan,bChip,bAddr,feature,4);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_ReadID(void)
{
	U8 i;
	U8 bChan;
	U8 bChip;
    U08 bSysClkSource 	= SG_CPM->rSYS_CLK_SOURCE_SEL;
	U08 bTrx2ClkSource 	= HAL_CPMGetClkSource(0);
	U08 bSysClkPeriod 	= SG_CPM->rSYS_CLK_PERIOD;;
	U08 bTrx2ClkPeriod;

	U32 RegValue;
//	FDL_Log("JCTABLE_ADD = %08X", MEM_JCTABLE_BUFF_ADDR);
//	FDL_Log("DMA_ADD = %08X", MEM_DMA_BUFF_ADDR);
//	FDL_Log("DMA_Data %02X", ((U8 *)MEM_DMA_BUFF_ADDR)[0]);
//	FDL_Log("DMA_Data %02X", ((U8 *)MEM_DMA_BUFF_ADDR)[1]);
//	FDL_Log("DMA_Data %02X", ((U8 *)MEM_DMA_BUFF_ADDR)[2]);
//	FDL_Log("DMA_Data %02X", ((U8 *)MEM_DMA_BUFF_ADDR)[3]);
//	FDL_Log("DMA_Data %02X", ((U8 *)MEM_DMA_BUFF_ADDR)[4]);
//	FDL_Log("DMA_Data %02X", ((U8 *)MEM_DMA_BUFF_ADDR)[5]);
//
//	RegValue = SG_DMA2_CH(bChan)->rENABLE.wAll;
//	FDL_Log("Reg %08X", RegValue);



	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);

//    HAL_CQWaitDone(bChan);
//    HAL_MCUSendCER_CE(bChan, bChip, 0);
    
	HostCmdResponse();
	bChan=pCtrl->P1;
	bChip=pCtrl->P2;
	bTrx2ClkPeriod 	= HAL_CPMGetTrx2Period(bChan);
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQSendFlashCmd(bChan,0x90);
	HAL_CQSendAddr(bChan,0x00);
	HAL_CQWaitDone(bChan);
//	HAL_SystemInit(bSysClkSource,bSysClkPeriod,bTrx2ClkSource,32,32,32,32);
	SG_CPM->rSYS_CLK_SWITCH_EN = 0;
	SG_CPM->rSYS_CLK_PERIOD = 4;//SYSCLK_PERIOD_SEL
	SG_CPM->rSYS_CLK_SWITCH_EN = 1;
	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x00;
	for (i = 0; i < 8; i++)
	{
		((U8 *)MEM_DMA_BUFF_ADDR)[i]= HAL_McuSendReadPluse(bChan);
	}
	((U16 *)MEM_DMA_BUFF_ADDR)[5] = NAND_FLASH;
//	HAL_SystemInit(bSysClkSource,bSysClkPeriod,bSysClkSource,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod,bTrx2ClkPeriod);
	SG_CPM->rSYS_CLK_SWITCH_EN = 0;
	SG_CPM->rSYS_CLK_PERIOD = bSysClkPeriod;//SYSCLK_PERIOD_SEL
	SG_CPM->rSYS_CLK_SWITCH_EN = 1;
	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;


	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_ReadUID(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=pCtrl->bChan;
	U8 bChip=pCtrl->bChip;

	HostCmdResponse();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_EN=1;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 2*512;

	FA_SubReadData_ReadUID(pCtrl);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_SubReadData_ReadUID(T_FDL_FTCTRL *pCtrl)
{
	U8 bChan,bChip;
	U16 wColAdd = 0;
	U32 dRowAdd = 0;

	bChan = pCtrl->bChan;
	bChip = pCtrl->bChip;
	dRowAdd = pCtrl->dPageAddr;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

#if ((NAND_FLASH==YMTC_X2_9060)||(NAND_FLASH==YMTC_X3_9060)||(NAND_FLASH==YMTC_X3_9070)||(NAND_FLASH==YMTC_X4_9060)||(NAND_FLASH==YMTC_X2_6070)||(NAND_FLASH==YMTC_X3_6070)||(NAND_FLASH==YMTC_X4_6080)||(NAND_FLASH==INTEL_N38A)||(NAND_FLASH==INTEL_N38B)||(NAND_FLASH==INTEL_N4xA)||(NAND_FLASH==MIC_B27B)||(NAND_FLASH==MIC_B47R)||(NAND_FLASH==MIC_B58R)||(NAND_FLASH==MIC_B57T)||(NAND_FLASH==MIC_N48R)||(NAND_FLASH==MIC_N58R))
	HAL_CQSendFlashCmd(bChan,0xED);
	HAL_CQSendAddr(bChan,0x00);

#elif((NAND_FLASH==SAMSUNG_3DV6_TLC)||(NAND_FLASH==SAMSUNG_3DV8_TLC)||(NAND_FLASH==SAMSUNG_3DV6P)||(NAND_FLASH==SAMSUNG_3DV7_TLC)||(NAND_FLASH==SAMSUNG_3DV5_QLC)||(NAND_FLASH==SAMSUNG_3DV4_QLC)||(NAND_FLASH==SAMSUNG_3DV7_QLC)||(NAND_FLASH==SAMSUNG_3DV5_2K_MLC))
	HAL_CQSendFlashCmd(bChan,0xDA);
	HAL_CQSendFlashCmd(bChan,0x30);
	HAL_CQSendFlashCmd(bChan,0x65);

	HAL_CQSendFlashCmd(bChan,0x00);

#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif
	HAL_CQSendFlashCmd(bChan,0x30);

#elif((NAND_FLASH==Hynix_3DV5_TLC)||(NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH==Hynix_3DV7_TLC)||(NAND_FLASH==Hynix_3DV8_TLC)||(NAND_FLASH==Hynix_3DV9_TLC)||(NAND_FLASH==Hynix_3DV7_QLC))
	HAL_CQSendFlashCmd(bChan,0x02);
	HAL_CQSendFlashCmd(bChan,0x19);
	HAL_CQSendFlashCmd(bChan,0xDA);

	HAL_CQSendFlashCmd(bChan,0x00);

	if (dRowAdd == 0)
	{
		dRowAdd = FDL_UID_dRow_ADDR;
	}
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSend6Addr(bChan, wColAdd, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSend5Addr(bChan, wColAdd, dRowAdd );///////////////5Byte Addr
#endif

	HAL_CQSendFlashCmd(bChan,0x30);

#endif

	HAL_CQWaitDone(bChan);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

	HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
	HAL_CQSendColAddr(bChan, 0);
	HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);

	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	HAL_CQWaitDone(bChan);
	HAL_CQWaitDone(bChan);

#if((NAND_FLASH==SAMSUNG_3DV6_TLC)||(NAND_FLASH==SAMSUNG_3DV8_TLC)||(NAND_FLASH==SAMSUNG_3DV6P)||(NAND_FLASH==SAMSUNG_3DV7_TLC)||(NAND_FLASH==SAMSUNG_3DV5_QLC)||(NAND_FLASH==SAMSUNG_3DV4_QLC)||(NAND_FLASH==SAMSUNG_3DV7_QLC)||(NAND_FLASH==SAMSUNG_3DV5_2K_MLC)||(NAND_FLASH==Hynix_3DV5_TLC)||(NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH==Hynix_3DV7_TLC)||(NAND_FLASH==Hynix_3DV8_TLC)||(NAND_FLASH==Hynix_3DV9_TLC)||(NAND_FLASH==Hynix_3DV7_QLC))
	HAL_CQSendFlashCmd(bChan,0xFF);
	HAL_CQWaitDone(bChan);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);

	HAL_CQSendFlashCmd(bChan,0xDF);
#endif

}

void FA_NCQReadData(T_FLASH_PHY_ADDR const *ptClstAddr,U32 dDMA2BaseAdd,U32 dofset,U08 bSectorOfPage)
{
	U16 wCol  			= 0;
	U08 bChan 			= (U08)(((ptClstAddr->m_bLunOfChan)>>2)&0x03);
	U08 bChip 			= (U08)(((ptClstAddr->m_bLunOfChan)>>4)&0x03);
	U32 dRowAddr 		= ptClstAddr->m_dRowAddr & 0xffffffff;
	U08 bPlaneMode 		= (U08)((ptClstAddr->m_bClstOffs)>>4);//Plane OpertateMode	1-SIngle 2-Two  3-Three...
	U08 bOpPlane 		= (U08)(ptClstAddr->m_bClstOffs & 0X0F);//Current Operate Plane Idx 0-1stPla 1-2ndPla...
	U08 bOpmode         = (U08)(ptClstAddr->m_bDataType & 0X0F);
	U08 offset         = (U08)(ptClstAddr->m_bChan & 0XFF);
	U08 i;
	U32 dBuffIndex 		= dofset*bSectorOfPage;
	U32 RBPosition;
	U16 PreCMD;
	U16 dWLAddr;
	U8 bRetry_En;
	U8 bModeSel_En;    ////多plane操作时，是否每个plane都需要发模式选择指令
	U8 bPagePreCmd_En; ////页前缀指令使能
	T_FLASH_PHY_ADDR  * PagePassed = (T_FLASH_PHY_ADDR  * )(ptClstAddr-(bPlaneMode-1));


#if (FDL_READ_HAVE_PRE_CMD == WL_ADD_TYPE_QLC)///行地址由WL_ADD和BLK_ADD组成的QLC类型，需要与jupiter同步		
	#if ((NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))
		U32 PageAddress=dRowAddr&(4*FDL_ROW_OF_BLK-1);
		U32 BlockAddress=dRowAddr/(4*FDL_ROW_OF_BLK);
		if(1==bOpmode)
		{
			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+(PageAddress/4)/5*8+(PageAddress/4)%5;
			PreCMD=PageAddress%4 + 1;
			bPagePreCmd_En = TRUE;
		}
		else
		{
			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+PageAddress/5*8+PageAddress%5;
			PreCMD=0;
			bPagePreCmd_En = FALSE;
		}
		RBPosition = PageAddress;
	#elif ((NAND_FLASH == MIC_N58R))
		U32 PageAddress = dRowAddr&(4*FDL_ROW_OF_BLK-1);
		U32 BlockAddress = dRowAddr/(4*FDL_ROW_OF_BLK);
		U8 IsQLCWL = 0;
		//FDL_Log("BlockAdd:%d, PageAdd:%d", BlockAddress, PageAddress);
		if(1==bOpmode)
		{
			//edge MLC 首尾
			if (PageAddress < 8 || PageAddress >3703) {
				PreCMD = PageAddress%2+1;
				//最后一层是从WL 932, page 3704开始的。
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+((PageAddress < 8)?(PageAddress/2) : 932+(PageAddress-3704)/2);
				IsQLCWL = 0;
			}
			//edge MLC 中间
			else if (PageAddress > 1847 && PageAddress < 1864) {
				PreCMD = (PageAddress-1848)%2 + 1;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-1848)/2+464;
				IsQLCWL = 0;
			}
			//前半部分QLC
			else if (PageAddress < 1848) {
				PreCMD = (PageAddress-8)%4+1;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-8)/4+4;
				IsQLCWL = 1;
			}
			//剩下的后半部分QLC,从page 1864开始
			else {
				PreCMD = (PageAddress-1864)%4+1;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-1864)/4+472;
				IsQLCWL = 1;
			}
			bPagePreCmd_En = TRUE;
		}
		else
		{
			PreCMD = 0;
			dRowAddr = BlockAddress*FDL_ROW_OF_BLK+PageAddress;
			bPagePreCmd_En = FALSE;		
		}
		RBPosition = PageAddress;
		//FDL_Log("BlockAdd:%d, PreCMD:%d, dRowAddr:%d", BlockAddress, PreCMD, dRowAddr);
	#elif ((NAND_FLASH == MIC_N69R))
		U32 PageAddress = dRowAddr&(4*FDL_ROW_OF_BLK-1);
		U32 BlockAddress = dRowAddr/(4*FDL_ROW_OF_BLK);
		U8 IsQLCWL = 0;
		//FDL_Log("BlockAdd:%d, PageAdd:%d", BlockAddress, PageAddress);
		if(1==bOpmode)
		{
			//edge MLC 首尾
			if (PageAddress >4407) {
				PreCMD = PageAddress%2+1;
				//最后一层是从WL 1108, page 4408开始的。
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-4408)/2+1108;
				IsQLCWL = 0;
			}
			//edge MLC 中间
			else if (PageAddress > 2175 && PageAddress < 2200) {
				PreCMD = (PageAddress-2176)%2 + 1;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-2176)/2+544;
				IsQLCWL = 0;
			}
			//前半部分QLC
			else if (PageAddress < 2176) {
				PreCMD = PageAddress%4+1;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+PageAddress/4;
				IsQLCWL = 1;
			}
			//剩下的后半部分QLC,从page 2200开始
			else {
				PreCMD = (PageAddress-2200)%4+1;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-2200)/4+556;
				IsQLCWL = 1;
			}
			bPagePreCmd_En = TRUE;
		}
		else
		{
			PreCMD = 0;
			dRowAddr = BlockAddress*FDL_ROW_OF_BLK+PageAddress;
			bPagePreCmd_En = FALSE;		
		}
		RBPosition = PageAddress;
		//FDL_Log("BlockAdd:%d, PreCMD:%d, dRowAddr:%d", BlockAddress, PreCMD, dRowAddr);
	#else
		U32 PageAddress=dRowAddr&(4*FDL_ROW_OF_BLK-1);
		U32 BlockAddress=dRowAddr/(4*FDL_ROW_OF_BLK);
		if(1==bOpmode)
		{
			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+PageAddress/4;
			PreCMD=PageAddress%4 + 1;
			bPagePreCmd_En = TRUE;
		}
		else
		{
			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+PageAddress;
			PreCMD=0;
			bPagePreCmd_En = FALSE;
		}
		RBPosition = PageAddress;
	#endif

#elif ((FDL_READ_HAVE_PRE_CMD == WL_ADD_TYPE_TLC))///行地址由WL_ADD和BLK_ADD组成的TLC类型，需要与jupiter同步
	#if ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
			U32 PageAddress = dRowAddr&(4*FDL_ROW_OF_BLK-1);
			U32 BlockAddress = dRowAddr/(4*FDL_ROW_OF_BLK);
//			FDL_Log("BlockAdd:%d, PageAdd:%d", BlockAddress, PageAddress);
			if(1==bOpmode)
			{
				//edge SLC
				if (PageAddress < 4 || PageAddress >2779) {
					PreCMD = 1;
					//最后一层是从WL 933, page 2780开始的。
					dRowAddr = BlockAddress*FDL_ROW_OF_BLK+((PageAddress < 4)?PageAddress : 932+PageAddress-2780);
				}
				//edge MLC
				else if (PageAddress > 1383 && PageAddress < 1400) {
					PreCMD = (PageAddress-1384)%2 + 1;
					dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-1384)/2+464;
				}
				//前半部分TLC
				else if (PageAddress < 1384) {
					PreCMD = (PageAddress-4)%3+1;
					dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-4)/3+4;
				}
				//剩下的后半部分TLC,从page 1400开始
				else {
					PreCMD = (PageAddress-1400)%3+1;
					dRowAddr = BlockAddress*FDL_ROW_OF_BLK+(PageAddress-1400)/3+472;
				}
				bPagePreCmd_En = TRUE;
			}
			else
			{
				PreCMD = 0;
				dRowAddr = BlockAddress*FDL_ROW_OF_BLK+PageAddress;
				bPagePreCmd_En = FALSE;
			}
			RBPosition = PageAddress;
//			FDL_Log("BlockAdd:%d, PreCMD:%d, dRowAddr:%d", BlockAddress, PreCMD, dRowAddr);
    #elif ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
    		U32 PageAddress=dRowAddr&(4*FDL_ROW_OF_BLK-1);
    		U32 BlockAddress=dRowAddr/(4*FDL_ROW_OF_BLK);
    		if(1==bOpmode)
    		{
    			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+(PageAddress/3)/5*8+(PageAddress/3)%5;
    			PreCMD=PageAddress%3 + 1;
    			bPagePreCmd_En = TRUE;
    		}
    		else
    		{
    			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+PageAddress/5*8+PageAddress%5;
    			PreCMD=0;
    			bPagePreCmd_En = FALSE;
    		}
    		RBPosition = PageAddress;
	#else
		U32 PageAddress=dRowAddr&(4*FDL_ROW_OF_BLK-1);
		U32 BlockAddress=dRowAddr/(4*FDL_ROW_OF_BLK);
		if(1==bOpmode)
		{
			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+PageAddress/3;
			PreCMD=PageAddress%3 + 1;
			bPagePreCmd_En = TRUE;
		}
		else
		{
			dRowAddr=BlockAddress*FDL_ROW_OF_BLK+PageAddress;
			PreCMD=0;
			bPagePreCmd_En = FALSE;
		}
		RBPosition = PageAddress;
	#endif
#else							  ///行地址有PAGE_ADD和BLK_ADD组成的类型，默认类型，直接用jupiter给出的行地址
		U32 PageAddress=dRowAddr&(FDL_ROW_OF_BLK-1);
		RBPosition = PageAddress;
		bPagePreCmd_En = FALSE;
//		FDL_Log("RowAddr =%08X",dRowAddr);
#endif

	if((NAND_FLASH == KHMB) && (bOpmode == 0))
	{
		dRowAddr = dRowAddr + PageAddress;
	}
	
#if ((NAND_FLASH == TOSHIBA_FLASH)||(NAND_FLASH == KHMB)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC)) ///需要retry使能指令的flash类型
	bRetry_En = TRUE;
#else
	bRetry_En = FALSE;
#endif

#if (NAND_FLASH == DYNAMIC_MODE_FLASH) ///每个plane都需要发模式切换指令的flash类型
	bModeSel_En = TRUE;
#else
	bModeSel_En = FALSE;
#endif

	//FDL_Log("NCQ dRowAddr:%d", dRowAddr);

	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	delay_us(1);

	if(TRUE == bRetry_En)
	{
		HAL_CQSendFlashCmd(bChan, 0x26);
		HAL_CQSendFlashCmd(bChan, FL_CMD_RETRY_ENABLE);
//		HAL_CQSendFlashCmd(bChan, 0x07);
	}
	if((bOpPlane == 0)||(TRUE == bModeSel_En))
	{
		FA_SendOprModeCmd(bChan, bOpmode);
	}

/***
#if((NAND_FLASH == MIC_N58R))
	U8 readoffset[4];
	for(i = 0;i<4;i++)
	{
		readoffset[i] = offset;
	}
	if(1==bOpmode)
	{
		if ((IsQLCWL == 1) && (PreCMD == 3)) 
		{
			//QLC XP
			readoffset[3] = 0;
		}
		else if((IsQLCWL == 0) && (PreCMD == 1))
		{
			//MLC LP
			readoffset[1] = 0; 
			readoffset[2] = 0; 
			readoffset[3] = 0; 
		}
		else if((IsQLCWL == 0) && (PreCMD == 2))
		{
			//MLC UP
			readoffset[2] = 0; 
			readoffset[3] = 0; 
		}
	}
	else
	{
		//SLC
		readoffset[1] = 0; 
		readoffset[2] = 0; 
		readoffset[3] = 0; 
	}
		HAL_CQSendFlashCmd(bChan,0x2E);
		HAL_CQSendAddr(bChan, readoffset[0]);
		HAL_CQSendAddr(bChan, readoffset[1]);
		HAL_CQSendAddr(bChan, readoffset[2]);
		HAL_CQSendAddr(bChan, readoffset[3]);
#endif
***/
	if(TRUE == bPagePreCmd_En)
	{
		HAL_CQSendFlashCmd(bChan, PreCMD);
	}

#if((NAND_FLASH == MIC_B47R)||(NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		HAL_CQSendFlashCmd(bChan,0x2E);
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
#elif((NAND_FLASH == MIC_N48R))
		HAL_CQSendFlashCmd(bChan,0x2E);
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
#endif


	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
	HAL_CQSend6Addr(bChan, 0, dRowAddr );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
	HAL_CQSend5Addr(bChan, 0, dRowAddr );///////////////5Byte Addr
#endif

#if((NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
		HAL_CQSendAddr(bChan, offset );
#endif

	if(bOpPlane != (bPlaneMode-1))
	{

//        #if ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
//        if (bOpPlane == 0)
//        {
//            HAL_CQSendFlashCmd(bChan, FL_CMD_READ_MP);
//        }
//        else
//        {
//            HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
//        }
//        #else
        HAL_CQSendFlashCmd(bChan, FL_CMD_READ_MP);
//        #endif

		return;
	}
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	///////////////////////////////////////记录RB时间
	if(1==(ptClstAddr->m_bDataType)>>4&0xF)
	{
		FA_RecordRB( bChan, bChip, RBPosition, 0);
	}
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);


	for(i = 0; i < bPlaneMode; ++i)
	{
		HAL_CQWaitDone(bChan);
		dBuffIndex 	= (dofset-bPlaneMode+i+1)*bSectorOfPage;

#if ((FDL_READ_HAVE_PRE_CMD == WL_ADD_TYPE_QLC)||(FDL_READ_HAVE_PRE_CMD == WL_ADD_TYPE_TLC))
		dRowAddr = PagePassed[i].m_dRowAddr & 0xffffffff;
		BlockAddress=dRowAddr/(4*FDL_ROW_OF_BLK);
		//FDL_Log("BlockAdd:%d, dRowAddr:%d", BlockAddress, dRowAddr);
		dRowAddr=BlockAddress*FDL_ROW_OF_BLK;
		//FDL_Log("New dRowAddr:%d", dRowAddr);
#else
		dRowAddr = PagePassed[i].m_dRowAddr & 0xffffffff;
#endif

		HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
		//FDL_Log("NCQ dRowAddr:%d", dRowAddr);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
		HAL_CQSend6Addr(bChan, 0, dRowAddr );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
		HAL_CQSend5Addr(bChan, 0, dRowAddr );///////////////5Byte Addr
#endif
		HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
		HAL_CQSendColAddr(bChan, wCol);
		HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
		delay_us(1);
//		FDL_Log("%06X %06X %06X %04X", SG_DMA1->rINDEX, dBuffIndex, SG_DMA2_CH(bChan)->rINDEX_OF_RAM.wAll, SG_ADM->rBUF_SIZE);
		HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, dBuffIndex, 0, bSectorOfPage, JOINT_21, MEM_DMA2_INFO_ADDR);
		HAL_CQWaitDone(bChan);
//		HAL_ADMStartDMA1(MEM_DMA_BUFF_ADDR,0,bSectorOfPage,DMA1_RD_TO_HOST,JOINT_NONE);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
//		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
//		HAL_CQChipSelect(bChan, bChip);
	}
	
}

void FA_SetOffset(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip;
	U8 bLUN =(U8)pCtrl->bRBMode;
	U8 offset = (U8)pCtrl->bBlklist[0];

#if((NAND_FLASH == INTEL_N38B))
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitDone(bChan);
	U8 addr;
	for (addr = 0x20; addr <= 0x2E; ++addr)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,0xEB);
		HAL_MCU_SendOneAddr(bChan, addr);
		HAL_MCU_SendOneAddr(bChan, 0x07);
		HAL_MCU_SendOneAddr(bChan, bLUN);
		Delay(0x40);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x10);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x01);
	}
	for (addr = 0x32; addr <= 0x38; ++addr)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,0xEB);
		HAL_MCU_SendOneAddr(bChan, addr);
		HAL_MCU_SendOneAddr(bChan, 0x07);
		HAL_MCU_SendOneAddr(bChan, bLUN);
		Delay(0x40);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x10);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x01);
	}
#elif((NAND_FLASH == INTEL_N4xA))
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitDone(bChan);
	U8 addr;
	for (addr = 0xBE; addr <= 0xDA; addr += 2)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,0xEB);
		HAL_MCU_SendOneAddr(bChan, addr);
		HAL_MCU_SendOneAddr(bChan, 0x07);
		HAL_MCU_SendOneAddr(bChan, bLUN);
		Delay(0x40);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x10);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x01);
	}
	for (addr = 0xE2; addr <= 0xEE; addr += 2)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,0xEB);
		HAL_MCU_SendOneAddr(bChan, addr);
		HAL_MCU_SendOneAddr(bChan, 0x07);
		HAL_MCU_SendOneAddr(bChan, bLUN);
		Delay(0x40);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x10);
		HAL_MCUSendDataByDqs_GAI(bChan,offset,0x01);
	}
#elif((NAND_FLASH == INTEL_N38A))
	U8 idata[4] = {offset,offset,offset,offset};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	U8 addr;
	for (addr = 0xC0; addr <= 0xCF ; ++addr)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
		HAL_MCU_SendOneAddr(bChan,addr);
		Delay(4);
		HAL_MCUSendNByteDataByDqs(bChan,idata,4);
	}
	for (addr = 0xD0; addr <= 0xD6 ; ++addr)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
		HAL_MCU_SendOneAddr(bChan,addr);
		Delay(4);
		HAL_MCUSendNByteDataByDqs(bChan,idata,4);
	}
#elif((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV7_TLC))
	U8 idata[4] = {offset,offset,offset,offset};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	U8 addr;

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x89);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,idata,4);


	idata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x8A);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,idata,4);

	idata[1] = 0;
	idata[2] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x8D);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,idata,4);
#elif(NAND_FLASH == Hynix_3DV8_TLC)
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitDone(bChan);

	U8 Param[4] = {0,0,0,0};

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xF5);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,Param,4);
	
	U8 addr;
	for (addr = 0xA9; addr <= 0xAF; addr++)
	{
		HAL_MCU_SetCe(bChan,bChip,1);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
		HAL_CQWaitDone(bChan);
		Param[0]=offset;
		HAL_CQSendFlashCmd(bChan,0x36);
		HAL_CQSendAddr(bChan,addr);
		HAL_CQWaitDone(bChan);
		HAL_MCUSendNByteDataByDqs(bChan,Param,1);
		HAL_CQSendFlashCmd(bChan,0x16);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);	
	}
#elif(NAND_FLASH == Hynix_3DV9_TLC)
		HAL_MCU_SetCe(bChan,bChip,1);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
		HAL_CQWaitDone(bChan);
	
		U8 Param[4] = {0,0,0,0};
	
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
		HAL_MCU_SendOneAddr(bChan,0xF5);
		Delay(4);
		HAL_MCUSendNByteDataByDqs(bChan,Param,4);
		
		U8 addr;
		for (addr = 0xA7; addr <= 0xAE; addr++)
		{
			HAL_MCU_SetCe(bChan,bChip,1);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
			HAL_CQWaitDone(bChan);
			Param[0]=offset;
			HAL_CQSendFlashCmd(bChan,0x36);
			HAL_CQSendAddr(bChan,addr);
			HAL_CQWaitDone(bChan);
			HAL_MCUSendNByteDataByDqs(bChan,Param,1);
			HAL_CQSendFlashCmd(bChan,0x16);
			HAL_CQWaitDone(bChan);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);	
		}

#elif((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X3_9070)||(NAND_FLASH == YMTC_X4_9060))
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);

	U8 ydata[4] = {offset,offset,offset,0};
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA1);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[2] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA2);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[1] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA3);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#elif((NAND_FLASH == YMTC_X2_6070))
	U8 ydata[4] = {offset,offset,offset,offset};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	U8 addr;
	for (addr = 0xA1; addr <= 0xA3; ++addr)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
		HAL_MCU_SendOneAddr(bChan,addr);
		Delay(4);
		HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
	}

	ydata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xAC);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[2] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xAB);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xAD);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[1] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA4);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#elif((NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080))
	U8 ydata[4] = {offset,offset,offset,offset};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	U8 addr;
	for (addr = 0xA1; addr <= 0xA3; ++addr)
	{
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
		HAL_MCU_SendOneAddr(bChan,addr);
		Delay(4);
		HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
	}

	ydata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x91);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

 	ydata[2] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x90);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x92);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[1] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xB0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#elif((NAND_FLASH == YMTC_X4_9060))
	U8 ydata[4] = {offset,offset,offset,0};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA1);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[2] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
	
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA2);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#elif ((NAND_FLASH == SanDisk_BiCS8_TLC))
	U8 ydata[4] = {offset,offset,offset,offset};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x12);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x13);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
	
	ydata[2] = 0;
	ydata[1] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x14);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#endif

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_COPYBACKWrite(void)
{
//	FDL_Log("start");
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();

	U8 bChan 			= (U8)pCtrl->dRev[0];
	U8 bChip 			= (U8)pCtrl->dRev[1];	
	U8 SeedIdx			= (U8)pCtrl->P1;
	U8 Layer_Part_Cnt 	= (U8)pCtrl->P2;
	U8 ProgramMode		= (U8)pCtrl->P3;	//编程模式,0为顺序编程,1为WL锯齿,2为layer锯齿
	U8 BlkCnt			= (U8)pCtrl->P4;
//	U8 Gap				= (U8)pCtrl->FeatureAddress;

	FDL_Log("bChan:%d",bChan);
	FDL_Log("bChip:%d",bChip);
	FDL_Log("SeedIdx:%d",SeedIdx);
	FDL_Log("Layer_Part_Cnt:%d",Layer_Part_Cnt);
	FDL_Log("BlkCnt:%d",BlkCnt);
	FDL_Log("ProgramMode:%d",ProgramMode);

	U16 *BlkList 		= (U16 *)(MEM_DMA_BUFF_ADDR + 4000);
	U16 *SeedList 		= (U16 *)(MEM_DMA_BUFF_ADDR + 4000 + BlkCnt*2);
//	U8 	*SectionList 	= (U8  *)(MEM_DMA_BUFF_ADDR	+ 4000 + BlkCnt*2*5);
	U16 *ProgramMap		= (U16 *)(MEM_DMA_BUFF_ADDR + 5000);

	FDL_Log("BlkList:%d\t%d\t%d\t%d\t",BlkList[0],BlkList[1],BlkList[2],BlkList[3]);

	U8 id = 0;
	for (id = 0; id < 3; id++)
	{
		FDL_Log("SeedList[%d]:%d\t%d\t%d\t%d\t",id,SeedList[id*BlkCnt+0],SeedList[id*BlkCnt+1],SeedList[id*BlkCnt+2],SeedList[id*BlkCnt+3]);
	}

#if ((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH==Hynix_3DV7_TLC)||(NAND_FLASH==Hynix_3DV8_TLC)||(NAND_FLASH==Hynix_3DV9_TLC)||(NAND_FLASH==SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH==YMTC_X2_9060)||(NAND_FLASH==YMTC_X3_9060)||(NAND_FLASH==YMTC_X4_9060))
	U8 	PreCMD;
	U32 PageAddr, WlAddr, dRowAddr;

	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, bChip);

	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;

	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

#if ((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH==Hynix_3DV7_TLC)||(NAND_FLASH==Hynix_3DV8_TLC)||(NAND_FLASH==Hynix_3DV9_TLC))
	#define  FDL_WL_OF_LAYER 8
	#define	 FL_CMD_COPY_BACK_PROG2_M_PLANE  (0x11)
#elif ((NAND_FLASH==SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
	#define  FDL_WL_OF_LAYER 5
	#define	 FL_CMD_COPY_BACK_PROG2_M_PLANE  (0x11)
#elif ((NAND_FLASH==YMTC_X2_9060)||(NAND_FLASH==YMTC_X3_9060))
	#define  FDL_WL_OF_LAYER 6
	#define	 FL_CMD_COPY_BACK_PROG2_M_PLANE  (0x11)
#elif ((NAND_FLASH==YMTC_X4_9060))
	#define  FDL_WL_OF_LAYER 10
	#define	 FL_CMD_COPY_BACK_PROG2_M_PLANE  (0x11)
#endif

	U8 Layer_Part_Idx, Page_Idx, Blk_Idx;
	for (Layer_Part_Idx = 0; Layer_Part_Idx < Layer_Part_Cnt; Layer_Part_Idx++)
	{
//		FDL_Log("%d,%d,%d,%d,%d",ProgramMap[Layer_Part_Idx*5+0],ProgramMap[Layer_Part_Idx*5+1],ProgramMap[Layer_Part_Idx*5+2],ProgramMap[Layer_Part_Idx*5+3],ProgramMap[Layer_Part_Idx*5+4]);

		U16 WL_Start 		= ProgramMap[Layer_Part_Idx*5+0] * FDL_WL_OF_LAYER;
		U16 WL_End 			= (ProgramMap[Layer_Part_Idx*5+1]+1) * FDL_WL_OF_LAYER;
		U16 StartPageAddr	= ProgramMap[Layer_Part_Idx*5+2];
		U8 xLC				= ProgramMap[Layer_Part_Idx*5+3];
		U8 Prog_Time		= ProgramMap[Layer_Part_Idx*5+4];

		PageAddr	= StartPageAddr;
		WlAddr		= WL_Start;
		for (WlAddr = WL_Start; WlAddr < WL_End; WlAddr++)
		{
			for (Page_Idx = 0; Page_Idx < xLC; Page_Idx++)
			{
				//当前页的种子号
				U8 SeedOffset = (SeedIdx+PageAddr)&0xFF;
				//SLC读取数据
				FA_SendOprModeCmd(bChan, 0);
				for (Blk_Idx = 0; Blk_Idx < BlkCnt; Blk_Idx++)
				{
#if ((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T)||(NAND_FLASH == Hynix_3DV8_TLC))
					dRowAddr = (SeedList[BlkCnt*Page_Idx+Blk_Idx]) * FDL_ROW_OF_BLK + SeedOffset;
#elif ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH == SanDisk_BiCS8_TLC))
					dRowAddr = (SeedList[BlkCnt*Page_Idx+Blk_Idx]) * FDL_ROW_OF_BLK + SeedOffset/5*8+SeedOffset%5;
#else
					dRowAddr = (SeedList[BlkCnt*Page_Idx+Blk_Idx]) * FDL_ROW_OF_BLK + SeedOffset;
#endif
					HAL_CQSendFlashCmd(bChan, FL_CMD_COPY_BACK_READ1);	
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
					HAL_CQSend6Addr(bChan, 0, dRowAddr);
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
					HAL_CQSend5Addr(bChan, 0, dRowAddr);
#endif	
					HAL_CQSendFlashCmd(bChan, (Blk_Idx!=BlkCnt-1)? FL_CMD_READ_MP:FL_CMD_COPY_BACK_READ2);	
					HAL_CQWaitDone(bChan);
					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
				}		
				//TLC写入数据
				for (Blk_Idx = 0; Blk_Idx < BlkCnt; Blk_Idx++)
				{
					//FA_SendOprModeCmd(bChan, 1);
#if ((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T)||(NAND_FLASH == Hynix_3DV8_TLC))
					dRowAddr = (BlkList[Blk_Idx]) * FDL_ROW_OF_BLK + WlAddr;
					PreCMD = Page_Idx + 1;
					HAL_CQSendFlashCmd(bChan, PreCMD);
#elif ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH == SanDisk_BiCS8_TLC))
					dRowAddr = (BlkList[Blk_Idx]) * FDL_ROW_OF_BLK + WlAddr/5*8+WlAddr%5;
					PreCMD = Page_Idx + 1;
					HAL_CQSendFlashCmd(bChan, PreCMD);
#else
					dRowAddr = (BlkList[Blk_Idx]) * FDL_ROW_OF_BLK + PageAddr;
#endif
					HAL_CQSendFlashCmd(bChan, FL_CMD_COPY_BACK_PROG1);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
					HAL_CQSend6Addr(bChan, 0, dRowAddr);
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
					HAL_CQSend5Addr(bChan, 0, dRowAddr);
#endif				
					HAL_CQSendFlashCmd(bChan, (Blk_Idx!=BlkCnt-1)? FL_CMD_COPY_BACK_PROG2_M_PLANE:(Page_Idx!=xLC-1)?FL_CMD_ED3_PRO_CMD:FL_CMD_ED3_PRO2_CMD);
					HAL_CQWaitDone(bChan);
					HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
				}
				PageAddr++;	
			}
		}
	}
#endif
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
//	FDL_Log("end");
}

/*
void FA_COPYBACKWrite_PMLC(void)
{
	//FDL_Log("start");
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_CTRL *pCtrl = (T_FDL_CTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();

	U8 SeedOffset=(U8)pCtrl->P1;
	U8 Gap=(U8)pCtrl->FeatureAddress;
	U8 BlkCnt=(U8)pCtrl->P4;
	U16 WLCnt = FDL_PAGE_OF_BLK/3;

	U16 *Blk = (U16 *)(MEM_DMA_BUFF_ADDR+4000);
	U8 *SectionList = (U8 *)(MEM_DMA_BUFF_ADDR+4000+4*2*BlkCnt);

	U16 BlkList[4];
	U16 SeedList[3][4];
	//U8 SectionList[16];
	int idx = 0;
	int jdx = 0;
	for (idx = 0; idx < 4; idx++)
	{
		BlkList[idx] = Blk[idx];
	}
	for (idx = 0; idx < 3; idx++)
	{
		for (jdx = 0; jdx < 4; jdx++)
		{
			SeedList[idx][jdx] = Blk[4*(idx+1)+jdx];
		}
	}

	//FDL_Log("BlkCnt=%d", BlkCnt);
	//FDL_Log("seed0:%d,%d,%d,%d",SeedList[0][0],SeedList[0][1],SeedList[0][2],SeedList[0][3]);
	//FDL_Log("seed1:%d,%d,%d,%d",SeedList[1][0],SeedList[1][1],SeedList[1][2],SeedList[1][3]);
	//FDL_Log("seed2:%d,%d,%d,%d",SeedList[2][0],SeedList[2][1],SeedList[2][2],SeedList[2][3]);
	
	//FDL_Log("blk:%d,%d,%d,%d",BlkList[0],BlkList[1],BlkList[2],BlkList[3]);	

	//FDL_Log("section:%d,%d,%d,%d,%d,%d,%d,%d",SectionList[0],SectionList[1],SectionList[2],SectionList[3],SectionList[4],SectionList[5],SectionList[6],SectionList[7]);

	U8 bChan=0;
	U8 bChip=0;
	U8 opCnt=9;
	U8 opIndex;
	U16 Operation;
	U8 RDDatacnt=0;
	U8 PadIOTemp,PadModeTemp;


	HAL_CQWaitDone(bChan);
	HAL_CQChipSelect(bChan, bChip);

	PadIOTemp = SG_PADCTRL->rPORT_MODE[15].bits.dVAL;
	PadModeTemp = SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB;
	SG_PADCTRL->rPORT_MODE[15].bits.dVAL |=(BIT_SET_1(7)); //设置为输入模式
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB = GPIO_MODE; //设置为GPIO模式

#if (NAND_FLASH == YMTC_X3_9060)
	U16 oList[24][10]={};
	if (BlkCnt == 1)
	{
		U16 sList[6][10]={
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[0][0]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[1][0]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[2][0]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x010,0x200},
		};
		U8 i = 0;
		U8 j = 0;
		for (i=0;i<6;i++)
		{
			for (j=0;j<10;j++)
			{
				oList[i][j] = sList[i][j];
			}
		}
	}
	else if (BlkCnt == 2)
	{
		U16 sList[12][10]={
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[0][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[0][1]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[1][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[1][1]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[2][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[2][1]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x010,0x200},
		};	
		  U8 i = 0;
		  U8 j = 0;
		  for (i=0;i<12;i++)
		  {
			  for (j=0;j<10;j++)
			  {
				  oList[i][j] = sList[i][j];
			  }
		  }
	}
	else if (BlkCnt == 3)
	{
		U16 sList[18][10]={
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[0][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[0][1]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[0][2]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[2]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[1][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[1][1]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[1][2]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[2]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[2][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[2][1]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[2][2]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[2]/16,0x100,0x010,0x200},
		};	
		  U8 i = 0;
		  U8 j = 0;
		  for (i=0;i<18;i++)
		  {
			  for (j=0;j<10;j++)
			  {
				  oList[i][j] = sList[i][j];
			  }
		  }		  
	}
	else if (BlkCnt==4)
	{
		U16 sList[24][10]={
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[0][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[0][1]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[0][2]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[0][3]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[2]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[3]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[1][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[1][1]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[1][2]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[1][3]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[2]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[3]/16,0x100,0x01A,0x200},
		  {0x0DA,0x000,0x100,0x100,0,0,0x100+SeedList[2][0]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[2][1]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[2][2]/16,0x100,0x032,0x200},
				{0x000,0x100,0x100,0,0,0x100+SeedList[2][3]/16,0x100,0x035,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[0]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[1]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[2]/16,0x100,0x011,0x200},
				{0x085,0x100,0x100,0,0,0x100+BlkList[3]/16,0x100,0x010,0x200},
		};
		  U8 i = 0;
		  U8 j = 0;
		  for (i=0;i<24;i++)
		  {
			  for (j=0;j<10;j++)
			  {
				  oList[i][j] = sList[i][j];
			  }
		  }		  
	}


	U16 SeedSrtWL = 0;
	U16 WLPerLayer = 6;
	switch (SeedOffset%3)
	{
		case (0):
		{
			SeedSrtWL = 0 + SeedOffset/3;
			break;
		}
		case (1):
		{
			SeedSrtWL = 171 + SeedOffset/3;
			break;
		}
		case (2):
		{
			SeedSrtWL = 86 + SeedOffset/3;
			break;
		}
	}
	
	int WL;	
	for (WL=0;WL<WLCnt;WL++) //实际上是按照WL行进的
	{
		if (WL%((Gap+1)*WLPerLayer)>=WLPerLayer)
		{
			//FDL_Log("%d",WL);
			continue;
		}
		if (SectionList[WL%WLPerLayer]==0)
		{
			continue;
		}
		int listIdx;
		U16 SeedWL = WL+SeedSrtWL;
		FDL_Log("SeedWL=%d",SeedWL);
		if (SeedWL >= WLCnt)

		{
			SeedWL = SeedWL%WLCnt;
		}
		for (listIdx=0;listIdx<6*BlkCnt;listIdx++)
		{
			if (listIdx%(2*BlkCnt)==0)
			{
				opCnt = 10;
				oList[listIdx][4] = 0x100+(SeedWL)%256;
				oList[listIdx][5] = 0x100+(SeedWL)/256+16*(SeedList[listIdx/(2*BlkCnt)][listIdx%BlkCnt]%16);
			}
			else if (listIdx%(2*BlkCnt)<BlkCnt)
			{
				opCnt = 9;
				oList[listIdx][3] = 0x100+(SeedWL)%256;
				oList[listIdx][4] = 0x100+(SeedWL)/256+16*(SeedList[listIdx/(2*BlkCnt)][listIdx%BlkCnt]%16);
			}
			else
			{
				opCnt = 9;
				oList[listIdx][3] = 0x100+(WL*3+listIdx/(2*BlkCnt))%256;
				oList[listIdx][4] = 0x100+(WL*3+listIdx/(2*BlkCnt))/256+16*(BlkList[listIdx%BlkCnt]%16);
			}
	
			for (opIndex=0;opIndex<opCnt;opIndex++)
			{
				HAL_CQWaitDone(bChan);
				Operation=oList[listIdx][opIndex];
				switch ((Operation>>8))
				{
					case (0):
					{
						HAL_CQSendFlashCmd(bChan, Operation);
						break;
					}
					case (1):
					{
						HAL_CQSendAddr(bChan,Operation);
						break;
					}
					case (2):
					{
						U8 RBStatus = 0;
						U16 RBRst;
						HAL_CQWaitDone(bChan);
						__enable_performance();
						
						while(__get_pfm_count()<1000)
						{
							if(0==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
							{
								RBStatus = 1;
								break;
							}
						}
						if(RBStatus == 1)
						{
							while(__get_pfm_count()<1200000) //~6ms over time
							{
								if(1==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
								{
									break;
								}
							}
							RBRst = (__get_pfm_count()+10)/20 + 2;
						}		
						break;

					}
					default:
					break;
				}
			}
		}
	}	
#elif (NAND_FLASH == Hynix_3DV8_TLC)
	U16 oList[30][9]={};
	if (BlkCnt == 1)
	{
		U16 sList[12][9]={
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x022,0x200},
			
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x022,0x200},

			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x023,0x200},
		};	
		U8 i = 0;
		U8 j = 0;
		for (i=0;i<12;i++)
		{
		  for (j=0;j<9;j++)
		  {
			  oList[i][j] = sList[i][j];
		  }
		}	
	} 
	else if (BlkCnt == 2)
	{
		U16 sList[18][9]={
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x022,0x200},
			
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x022,0x200},

			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x023,0x200},
		};		
		U8 i = 0;
		U8 j = 0;
		for (i=0;i<18;i++)
		{
		  for (j=0;j<9;j++)
		  {
			  oList[i][j] = sList[i][j];
		  }
		}	
	} 
	else if (BlkCnt == 3)
	{
		U16 sList[24][9]={
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x022,0x200},
			
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x022,0x200},

			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x023,0x200},
		};		
		U8 i = 0;
		U8 j = 0;
		for (i=0;i<24;i++)
		{
		  for (j=0;j<9;j++)
		  {
			  oList[i][j] = sList[i][j];
		  }
		}
	} 
	else 
	{
		U16 sList[30][9]={
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x001,0x085,0x100,0x100,0,0,0,0x022,0x200},
			
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x002,0x085,0x100,0x100,0,0,0,0x022,0x200},

			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x032,0x200},
			{0x0A2,0x000,0x100,0x100,0,0,0,0x035,0x200},
			
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x011,0x200},
			{0x003,0x085,0x100,0x100,0,0,0,0x023,0x200},
		};
		U8 i = 0;
		U8 j = 0;
		for (i=0;i<30;i++)
		{
		  for (j=0;j<9;j++)
		  {
			  oList[i][j] = sList[i][j];
		  }
		}			
	}
		
	U16 OutRangeSrt = 112; //当超出page范围时的起始位置:WL计数

	U16 SeedSrtWL = 0;
	U16 WLPerLayer = 8;
	switch (SeedOffset%3)
	{
		case (0):
		{
			SeedSrtWL = 0 + SeedOffset/3;
			break;
		}
		case (1):
		{
			SeedSrtWL = 171 + SeedOffset/3;
			break;
		}
		case (2):
		{
			SeedSrtWL = 86 + SeedOffset/3;
			break;
		}
	}

	int WL;
	//FDL_Log("%d %d %d %d",BlkList[0],BlkList[1],BlkList[2],BlkList[3]);
	for (WL=0;WL<WLCnt;WL++)
	{
		if (WL%((Gap+1)*WLPerLayer)>=WLPerLayer)
		{
			//FDL_Log("%d",WL);
			continue;
		}
		if (SectionList[WL%WLPerLayer]==0)
		{
			continue;
		}					
		int listIdx;
		U16 SeedWL = WL+SeedSrtWL;
		if (SeedWL >= WLCnt)
		{
			SeedWL = OutRangeSrt+SeedWL%WLCnt;
		}
		for (listIdx=0;listIdx<(BlkCnt+1)*6;listIdx++)
		{
			if (listIdx % (BlkCnt+1) == 0)
			{
				//首条为欺骗指令，无需处理
			}
			else if (listIdx%((BlkCnt+1)*2)<(BlkCnt+1))
			{
				oList[listIdx][4] = 0x100+SeedWL%256;
				oList[listIdx][5] = 0x100+SeedWL/256+SeedList[listIdx/((BlkCnt+1)*2)][listIdx%(BlkCnt+1)-1]%32*8;
				oList[listIdx][6] = 0x100+SeedList[listIdx/((BlkCnt+1)*2)][listIdx%(BlkCnt+1)-1]/32;
			}
			else
			{
				oList[listIdx][4] = 0x100+WL%256;
				oList[listIdx][5] = 0x100+WL/256+BlkList[listIdx%(BlkCnt+1)-1]%32*8;
				oList[listIdx][6] = 0x100+BlkList[listIdx%(BlkCnt+1)-1]/32;
			}
	
			for (opIndex=0;opIndex<opCnt;opIndex++)
			{
				HAL_CQWaitDone(bChan);
				Operation=oList[listIdx][opIndex];
				switch ((Operation>>8))
				{
					case (0):
					{
						HAL_CQSendFlashCmd(bChan, Operation);
						break;
					}
					case (1):
					{
						HAL_CQSendAddr(bChan,Operation);
						break;
					}
					case (2):
					{
						U8 RBStatus = 0;
						U16 RBRst;
						HAL_CQWaitDone(bChan);
						__enable_performance();
						
						while(__get_pfm_count()<1000)
						{
							if(0==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
							{
								RBStatus = 1;
								break;
							}
						}
						if(RBStatus == 1)
						{
							while(__get_pfm_count()<1200000) //~6ms over time
							{
								if(1==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
								{
									break;
								}
							}
							RBRst = (__get_pfm_count()+10)/20 + 2;
						}		
						break;
					}
					default:
					break;
				}
			}
		}
	}

#endif

	SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;
	
	SG_PADCTRL->rPAD_FUNC_CTRL4.bits.FXRB = PadModeTemp;
	SG_PADCTRL->rPORT_MODE[15].bits.dVAL = PadIOTemp;
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
	//FDL_Log("end");
}
*/

void FA_ReadDisturb(void)
{
	//FDL_Log("start");
	U32 i;
	U8 j;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=pCtrl->bChan;
	U8 bChip=pCtrl->bChip;
	//U8 dMode=pCtrl->dMode;
	U16 BlkAdd = (U16)(pCtrl->dSeedIdx);
	U32 dReadTime = pCtrl->dSecCnt;
	U16 PageCnt = pCtrl->dWareIdx;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.wAll = 1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_EN=1;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 2*512;
/*
	FDL_Log("bChan=%d",bChan);
	FDL_Log("bChip=%d",bChip);
	FDL_Log("BlkAdd=%d",BlkAdd);
	FDL_Log("dReadTime=%d",dReadTime);
	FDL_Log("PageCnt=%d",PageCnt);
*/

	// condition1: PreCMD
#if((NAND_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == SanDisk_BiCS5_QLC)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH == MIC_B57T)||(NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_N58R)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == SanDisk_BiCS6_QLC))
	//FA_SubReadData_PreCMD(pCtrl, pCtrl->dPageAddr, 0);
	U8 PreCMD;
	U16 WlAddr = (U16)(pCtrl->dSeedBuffEn);
	U32	dRowAdd= BlkAdd * FDL_ROW_OF_BLK + WlAddr;
	//FDL_Log("WlAddr=%d",WlAddr);	

	//FDL_Log("ReadPage dRowAddr:%d", dRowAdd);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	HAL_CQWaitDone(bChan);

	for(i=0;i<dReadTime;++i)
	{
		for(j=0;j<PageCnt;++j)
		{
			//__enable_performance();
			FA_SendOprModeCmd(bChan, 0x01);
			HAL_CQSendFlashCmd(bChan, j+1);
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, 0, dRowAdd);///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, 0, dRowAdd);///////////////5Byte Addr
#endif
			HAL_CQSendFlashCmd(bChan, 0x30);
			HAL_CQWaitDone(bChan);	
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			HAL_CQWaitDone(bChan);
			
			//U16 tmp = __get_pfm_count();
			//FDL_Log("WL:%d\tPage:%d\ttime:%d", i, j, (tmp*5+1550)/1000);
			
			__delay_us(10, 200);  // 推测是固件(CQ?)或者硬件本身的限制，这个延时是必要的...Paul 20250313
		}	
	}
#else  //condition2: without PreCMD
	//FA_SubReadData_WoPreCMD(pCtrl, pCtrl->dPageAddr, 0);
	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
	U16 *PageList = (U16 *)(&pCtrl-> bBlklist[0]);

	for(i=0;i<dReadTime;++i) //i->ReadCnt
	{
		for(j=0;j<PageCnt;++j) //j->PageIdx
		{
			U32	dRowAdd = BlkAdd * FDL_ROW_OF_BLK + PageList[j];
			FA_SendOprModeCmd(bChan, 0x01);
			
			HAL_CQSendFlashCmd(bChan, 0x00);
#if	  (ADDR_MODE_6 == FDL_ROW_ADDR_MODE)
			HAL_CQSend6Addr(bChan, 0, dRowAdd );///////////////6Byte Addr
#elif (ADDR_MODE_5 == FDL_ROW_ADDR_MODE)
			HAL_CQSend5Addr(bChan, 0, dRowAdd );///////////////5Byte Addr
#endif
			HAL_CQSendFlashCmd(bChan, 0x30);
			HAL_CQWaitDone(bChan);
			HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
			HAL_CQWaitDone(bChan);
			__delay_us(10, 200);  // 推测是固件(CQ?)或者硬件本身的限制，这个延时是必要的...Paul 20250313		
		}
	}
#endif
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}
#ifdef FDL_PAGE_SIZE
	#define FDL_CW_SIZE FDL_PAGE_SIZE/8
#endif

void FA_FindOV_Init(void)
{
	FDL_Log("start Init.");
	T_RDWR_DATA *pCmd 	= (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);	
	
	/***********************************MEM_DMA_BUFF_ADDR分配***********************************/
	//40-40+pagecnt:	groupModel差分数组
	//40+pagecnt-14335:	LPPage位图数组
	//14536-14835:		RRT数组
	//14836,14840:		出错数
	//14844:			出错和
	//14848:			被算入的WL数量
	//14900-14925:		offsets(SLC[1]+MLC[3]+TLC[7]+QLC[15]共计26字节)
	//14926-15999:		各种变量
	//16000-18999:		层偏移数组
	//19000,19036:		临时数组和非block的单层偏移数组
	//20480-40959:		ReadPage

	//40960-65535:		页最优出错
	//65600-90175:		页默认出错
	//90200-136999:		最优读电压
	
	U16 *GroupModel		= (U16 *)(MEM_DMA_BUFF_ADDR + 1000);			//页模型差分数组

	U8 	*bChan			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4000);
	U8 	*bChip			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4004);
	U8 	*RRTGroupCnt	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4008);			//RRT分组数
	U8 	*xLC			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4012);			//1:SLC 2:MLC 3:TLC 4:QLC
	U32 *totalPageCnt	= (U32 *)(MEM_DMA_BUFF_ADDR	+ 4016);			//总页数
	U8 	*WLPerLayer		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4024);			//每层WL数
	U8 	*layerCalFreq	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4028);			//每隔n层进行一次抛物线运算
	U8 	*isFirstBlk		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4032);			//是否是第一个分析的块
	U16	*WLPerRnd		= (U16 *)(MEM_DMA_BUFF_ADDR + 4036);			//每轮分析多少WL
	U16 *BlkID			= (U16 *)(MEM_DMA_BUFF_ADDR + 4040);

	U32 *Add			= (U32 *)(MEM_DMA_BUFF_ADDR + 4104);
	U8	*Cfg			= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4200);

	U32 *Add_OE			= (U32 *)(MEM_DMA_BUFF_ADDR + 4300);
	U32 *Add_DE			= (U32 *)(MEM_DMA_BUFF_ADDR + 4308);
	U32 *Add_OV			= (U32 *)(MEM_DMA_BUFF_ADDR + 4316);
	U32 *Add_OVE		= (U32 *)(MEM_DMA_BUFF_ADDR + 4324);
	U32 *Add_OE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4332);
	U32 *Add_DE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4340);
	U32 *Add_OV_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4348);
	U32 *Add_OVE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4356);
	U32 *Add_End		= (U32 *)(MEM_DMA_BUFF_ADDR + 4364);
	
	U8 	*OutDefErrEN	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4364);			//输出默认出错
	U8 	*OutWLErrEN		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4368);			//输出WL/页出错以及OV
	U8 	*OutCwErrEN		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4372);			//输出Cw/页出错以及OV
	U8 	*OutCwPageEN	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4376);			//输出Cw/页出错以及OV

	U8 	*totalVReadCnt 	= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4400);			//总读电压数量
	U8 	*ofsStartIdx 	= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4404);			//根据xLC确定offsets数组起始索引

	U8 	*vRead			= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4408);			//分页读电压索引数组
	U8 	*vReadCnt		= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4428);			//分页读电压数量数组
	U8 	*vReadStartIdx	= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4436);			//分页读电压起始索引数组
	U8 	*vReadJudge		= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4712);			//vRead判决数组

	signed char *RRTList = (signed char *)(MEM_DMA_BUFF_ADDR + 5000);	//RRT数组

	//测试用变量
	U32 *ReadCnt		= (U32 *)(MEM_DMA_BUFF_ADDR	+ 19100);			//读页次数

	/******************************初始化全局固定变量******************************/
	*bChan			= (U8)pCtrl	->bChan;
	*bChip			= (U8)pCtrl	->bChip;
	*RRTGroupCnt	= (U8)pCtrl	->dSeedIdx;
	*xLC			= (U8)pCtrl	->bRBMode;
	*totalPageCnt	= (U32)pCtrl->dPageCnt;	
	*WLPerLayer		= (U8)pCtrl	->P4;
	*layerCalFreq	= (U8)pCtrl	->FeatureAddress;
	*WLPerRnd		= (U16)pCtrl->dPlaneCnt;
	*isFirstBlk		= 1;

	*Add_OE			= Add[0];
	*Add_DE			= Add[1];
	*Add_OV			= Add[2];
	*Add_OVE		= Add[3];
	*Add_OE_Cw		= Add[4];
	*Add_DE_Cw		= Add[5];
	*Add_OV_Cw		= Add[6];
	*Add_OVE_Cw		= Add[7];
	*Add_End		= Add[8];
	
	*OutDefErrEN	= Cfg[0];
	*OutWLErrEN		= Cfg[1];
	*OutCwErrEN		= Cfg[2];
	*OutCwPageEN	= Cfg[3];

	*totalVReadCnt 	= *xLC==1? 1 : *xLC==2? 3 : *xLC==3? 7 : 15;
	*ofsStartIdx 	= *xLC==1? 0 : *xLC==2? 1 : *xLC==3? 4 : 11;

	FDL_Log("bChan:\t\t%d",		*bChan);
	FDL_Log("bChip:\t\t%d",		*bChip);
	FDL_Log("xLC:\t\t%d",		*xLC);
	FDL_Log("RRTGroupCnt:\t%d",	*RRTGroupCnt);
	FDL_Log("totalPageCnt:\t%d",*totalPageCnt);
	FDL_Log("WLPerLayer:\t%d",	*WLPerLayer);
	FDL_Log("layerCalFreq:\t%d",*layerCalFreq);
	FDL_Log("WLPerRnd:\t%d",	*WLPerRnd);
	FDL_Log("PLANE_OF_LUN:\t%d",FDL_PLANE_OF_LUN);
	FDL_Log("BLK_OF_LUN:\t%d",	FDL_BLK_OF_LUN);
	FDL_Log("OutDefErrEN:\t%d", *OutDefErrEN);
	FDL_Log("OutWLErrEN:\t%d", 	*OutWLErrEN);
	FDL_Log("OutCwErrEN:\t%d", 	*OutCwErrEN);
	FDL_Log("OutCwPageEN:\t%d", *OutCwPageEN);

	FDL_Log("Add_OE:\t\t%d",	*Add_OE);
	FDL_Log("Add_DE:\t\t%d",	*Add_DE);
	FDL_Log("Add_OV:\t\t%d",	*Add_OV);
	FDL_Log("Add_OVE:\t\t%d",	*Add_OVE);
	FDL_Log("Add_OE_Cw:\t%d",	*Add_OE_Cw);
	FDL_Log("Add_DE_Cw:\t%d",	*Add_DE_Cw);
	FDL_Log("Add_OV_Cw:\t%d",	*Add_OV_Cw);
	FDL_Log("Add_OVE_Cw:\t%d",	*Add_OVE_Cw);
	FDL_Log("Add_End:\t\t%d",	*Add_End);	
	
	/*******************初始化vRead、vReadCnt和vReadStartIdx数组*******************/
	U16 pageIdx,vRIdx,gVRIdx;
	for (pageIdx = 0, gVRIdx = 0; pageIdx < *xLC; pageIdx++)
	{
		vReadCnt[pageIdx]		= (U8)pCtrl->bBlklist[20+pageIdx];
		vReadStartIdx[pageIdx]	= pageIdx==0
?0:(vReadCnt[pageIdx-1]+vReadStartIdx[pageIdx-1]);
		for (vRIdx = 0;vRIdx < vReadCnt[pageIdx]; vRIdx++, gVRIdx++)
		{
			vRead[vReadStartIdx[pageIdx]+vRIdx]	= (U8)pCtrl->bBlklist[24+gVRIdx];
			FDL_Log("%d\tVRead[%d][%d] = %d",vReadStartIdx[pageIdx]+vRIdx,pageIdx,vRIdx,vRead[vReadStartIdx[pageIdx]+vRIdx]);
		}
	}

	/*********************************初始化RRT数组*********************************/
	U16 RRTGroupIdx;
	for (RRTGroupIdx = 0; RRTGroupIdx < *RRTGroupCnt; RRTGroupIdx++)
	{

		FDL_Log("%d\t%d\t%d\t%d\t%d\t%d\t%d",RRTList[RRTGroupIdx* (*totalVReadCnt+1)+0],RRTList[RRTGroupIdx* (*totalVReadCnt+1)+1],RRTList[RRTGroupIdx* (*totalVReadCnt+1)+2],RRTList[RRTGroupIdx* (*totalVReadCnt+1)+3],RRTList[RRTGroupIdx* (*totalVReadCnt+1)+4],RRTList[RRTGroupIdx* (*totalVReadCnt+1)+5],RRTList[RRTGroupIdx* (*totalVReadCnt+1)+6]);	
	}

	U8 i = 0;
	for (;i<*totalVReadCnt**xLC;i++)
	{
		FDL_Log("vReadJudge[%d]=%d",i,vReadJudge[i]);
	}
	
	SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS 		= 0; 		//关闭绕过模式，数据读出会被解随机化
	SG_DMA2_CH(*bChan)->rLEN_CFG.bits.BASIC_DATA_LEN 	= 2*512;
	
	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR + 4600), 0x00, 100);
	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR + 4800), 0x00, 200);
	MemorySet((U8 *)(MEM_DMA_BUFF_ADDR + 71800), 0x00, 180000);
	FDL_Log("END Init.");
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_FindOV(void)
{
	FDL_Log("Start");
	T_RDWR_DATA *pCmd 	= (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U16 BlkIdx			= (U16)pCtrl->dDelayCtrl; //待分析块号
	//__enable_performance();
	/***********************************MEM_DMA_BUFF_ADDR分配************************************/
	//-999:				容易被各种函数覆盖，避免使用
	//1000-3999:		groupModel压缩差分数组
	//4000-4999:		各种变量
	//5000-5199:		RRT数组
	//6000-9999:		层Offset，用于层偏移运算

	//10240-30719:		defaultPageStore
	//30800-51279:		defaultCwStore
	//51300-55300:		defaultCwHandle
	//55400-75879:		exReadPage
	//75900-96379:		calPage
	//96400-:			结果数组

	U16 *GroupModel 	= (U16 *)(MEM_DMA_BUFF_ADDR + 1000);		//页模型差分数组

	U8 	*bChan			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4000);
	U8 	*bChip			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4004);
	U8 	*RRTGroupCnt	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4008);		//RRT分组数
	U8 	*xLC			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4012);		//1:SLC 2:MLC 3:TLC 4:QLC
	U32 *totalPageCnt	= (U32 *)(MEM_DMA_BUFF_ADDR	+ 4016);		//总页数
	U8 	*WLPerLayer		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4024);		//每层WL数
	U8 	*layerCalFreq	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4028);		//每隔n层进行一次抛物线运算
	U8 	*isFirstBlk		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4032);		//是否是第一个分析的块
	U16	*WLPerRnd		= (U16 *)(MEM_DMA_BUFF_ADDR + 4036);		//每轮分析多少WL
	U16 *BlkID			= (U16 *)(MEM_DMA_BUFF_ADDR + 4040);		//块ID，通过与传入的BlkIdx比较来确定是否为该块的第一轮

	U32 *Add_OE			= (U32 *)(MEM_DMA_BUFF_ADDR + 4300);
	U32 *Add_DE			= (U32 *)(MEM_DMA_BUFF_ADDR + 4308);
	U32 *Add_OV			= (U32 *)(MEM_DMA_BUFF_ADDR + 4316);
	U32 *Add_OVE		= (U32 *)(MEM_DMA_BUFF_ADDR + 4324);
	U32 *Add_OE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4332);
	U32 *Add_DE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4340);
	U32 *Add_OV_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4348);
	U32 *Add_OVE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4356);
	
	U8 	*OutDefErrEN	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4364);			//输出默认出错
	U8 	*OutWLErrEN		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4368);			//输出WL/页出错以及OV
	U8 	*OutCwErrEN		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4372);			//输出Cw/页出错以及OV
	U8 	*OutCwPageEN	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4376);			//输出Cw/页出错以及OV

	U8 	*totalVReadCnt 	= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4400);		//总读电压数量
	U8 	*ofsStartIdx 	= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4404);		//根据xLC确定offsets数组起始索引
	U8 	*vRead			= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4408);		//分页读电压索引数组
	U8 	*vReadCnt		= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4428);		//分页读电压数量数组
	U8 	*vReadStartIdx	= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4436);		//分页读电压起始索引数组
	U8 	*vReadJudge		= (U8 *)(MEM_DMA_BUFF_ADDR	+ 4712);		//vRead判决数组

	U8 	*offsets 		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4500); 		//offsets[0:SLC,1-3:MLC,4-10:TLC,11-25:QLC]

	signed char *RRTList = (signed char *)(MEM_DMA_BUFF_ADDR + 5000);		//RRT数组

	short *tempOfs			= (short *)(MEM_DMA_BUFF_ADDR + 4532);			//临时的offset统计数组
	signed char *calOfs		= (signed char *)(MEM_DMA_BUFF_ADDR + 4568);	//非Blk0用于计算累计层偏移
	signed char *layerOfs	= (signed char *)(MEM_DMA_BUFF_ADDR + 6000);	//Blk0的层offset,第n层R2即layerOfs[n*vReadCnt+1]

	//运算变量
	//每一页都要累加curPageIdx,每一WL都要累加curLPPage
	U8 	*curWLPageCnt 	= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4600);		//WL内的页索引，计算完成后充当该WL的页数量
	U16 *curPageIdx		= (U16 *)(MEM_DMA_BUFF_ADDR + 4604);		//当前页的索引
	U16 *curLPPage		= (U16 *)(MEM_DMA_BUFF_ADDR + 4608);		//当前页的LP页号，通过差分数组累加得来
	U16 *curPage 		= (U16 *)(MEM_DMA_BUFF_ADDR + 4612);		//当前页的非LP页号，由LP页号累加差分数组得来
	U16 *curWL			= (U16 *)(MEM_DMA_BUFF_ADDR + 4616);		//当前WL
	U8 	*curLayer 		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4620);		//当前层
	U16 *gStackCntIdx 	= (U16 *)(MEM_DMA_BUFF_ADDR + 4628);
	U16 *gStackCntIdxN	= (U16 *)(MEM_DMA_BUFF_ADDR + 4632);
	U16 *StackCnt 		= (U16 *)(MEM_DMA_BUFF_ADDR + 4636);
	U8 	*isFirstWL		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 4640);
	//抛物线寻优用到的各个int变量
	int *k1 			= (int *)(MEM_DMA_BUFF_ADDR + 4644);
	int *k2 			= (int *)(MEM_DMA_BUFF_ADDR + 4652);
	int *a 				= (int *)(MEM_DMA_BUFF_ADDR + 4660);
	int *b 				= (int *)(MEM_DMA_BUFF_ADDR + 4668);

	//结果变量
	U32 *pErrTemp 			= (U32 *)(MEM_DMA_BUFF_ADDR + 4800);		//出错数
	U32 *pErrSumOE 			= (U32 *)(MEM_DMA_BUFF_ADDR + 4808); 		//出错和
	U32 *pErrSquareSumOE 	= (U32 *)(MEM_DMA_BUFF_ADDR + 4816);		//出错平方和
	U32 *pErrSumDE 			= (U32 *)(MEM_DMA_BUFF_ADDR + 4824); 		//出错和
	U32 *pErrSquareSumDE 	= (U32 *)(MEM_DMA_BUFF_ADDR + 4832);		//出错平方和

	U32 *pErrSumOE_Cw 		= (U32 *)(MEM_DMA_BUFF_ADDR + 4848); 		//出错和
	U32 *pErrSquareSumOE_Cw	= (U32 *)(MEM_DMA_BUFF_ADDR + 4856);		//出错平方和
	U32 *pErrSumDE_Cw		= (U32 *)(MEM_DMA_BUFF_ADDR + 4864); 		//出错和
	U32 *pErrSquareSumDE_Cw	= (U32 *)(MEM_DMA_BUFF_ADDR + 4872);		//出错平方和


	//输出变量
	U16 *Ret_OE			= (U16 *)(MEM_DMA_BUFF_ADDR + *Add_OE);
	U16 *Ret_DE			= (U16 *)(MEM_DMA_BUFF_ADDR + *Add_DE);
	U8 	*Ret_OV			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ *Add_OV);
	U16 *Ret_OVE		= (U16 *)(MEM_DMA_BUFF_ADDR + *Add_OVE);

	U16 *Ret_OE_Cw		= (U16 *)(MEM_DMA_BUFF_ADDR + *Add_OE_Cw);
	U16 *Ret_DE_Cw		= (U16 *)(MEM_DMA_BUFF_ADDR + *Add_DE_Cw);
	U8 	*Ret_OV_Cw		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ *Add_OV_Cw);
	U16 *Ret_OVE_Cw		= (U16 *)(MEM_DMA_BUFF_ADDR + *Add_OVE_Cw);	

	//额外页
	U8 *dePage			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 10240);	//默认纠错读取的页
	U8 *haPageCw		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 30800);	//处理单个Cw存的页(Handle)
	U8 *dePageCw		= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 34900);	//默认纠错读取的Cw
	U8 *exPage			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 55400);	//不解码读出其他的页
	U8 *calPage			= (U8 *)(MEM_DMA_BUFF_ADDR 	+ 75900);	//计算最优读电压出错的页

	//用于快速拷贝的U64数组
	U64 *U64dePage		= (U64 *)dePage;
	U64 *U64dePageCw0	= (U64 *)dePageCw;
	U64 *U64haPageCw0	= (U64 *)haPageCw;
	U64 *U64exPage		= (U64 *)exPage;
	U64 *U64calPage		= (U64 *)calPage;
	U64 *U64dePageCw[8];
	U64 *U64haPageCw[8];
	U8	*dePageCwDE[8];		//用于计算Cw的DE时的寻址

	//通用索引
	U8	vReadIdx;
	U8	cwIdx;
	U16 byteIdx;

	if (OutCwErrEN)
	{
		for (cwIdx = 0; cwIdx < 8; cwIdx++)
		{
			U64dePageCw[cwIdx] 	= (U64 *)(dePageCw+FDL_PAGE_SIZE/8*cwIdx);
			U64haPageCw[cwIdx] 	= (U64 *)(haPageCw+((FDL_PAGE_SIZE/8*cwIdx)&1023));
			dePageCwDE[cwIdx]	= (U8 *)(dePage+FDL_PAGE_SIZE/8*cwIdx);
			FDL_Log("[%d]\tU64haPageCw=%d\tU64dePageCw=%d\tdePageCwDE=%d",cwIdx,((U32)U64haPageCw[cwIdx]-MEM_DMA_BUFF_ADDR),((U32)U64dePageCw[cwIdx]-MEM_DMA_BUFF_ADDR),((U32)dePageCwDE[cwIdx]-MEM_DMA_BUFF_ADDR));
		}
	}

	//测试变量
	//U32 *ReadCnt		= (U32 *)(MEM_DMA_BUFF_ADDR	+ 4216); 	//读页次数
	//*ReadCnt = 0;
	FDL_Log("isFirst:%d",*isFirstBlk);
	/**************************************初始化块相关变量**************************************/
	//传入的块号与BlkID不一致，此次私有命令为该块的第一轮
	//(*BlkID == BlkIdx && *isFirstBlk == 1)
	if (*BlkID != BlkIdx || (*isFirstBlk == 1 && *curWL == 0))
	{ 
		//非第一个块在进入该判断时curWL值必然不为0
		//第一个块在进入该判断时curWL必然为0
		//两种情况均只会进入该判断1次
		if (*curWL != 0) 
		{
			*isFirstBlk = 0;
		}
		*BlkID 				= BlkIdx;
		*pErrTemp			= 0;
		*pErrSumOE			= 0;
		*pErrSquareSumOE	= 0;
		*pErrSumDE			= 0;
		*pErrSquareSumDE	= 0;
		*pErrSumOE_Cw		= 0;
		*pErrSquareSumOE_Cw	= 0;
		*pErrSumDE_Cw		= 0;
		*pErrSquareSumDE_Cw	= 0;
		
		*curWLPageCnt 		= 0;
		*curPageIdx			= 0;
		*curLPPage			= 0;
		*curPage 			= 0;
		*curWL				= 0;
		*curLayer 			= 0;
		*gStackCntIdx 		= 0;
		*gStackCntIdxN 		= 0;
		*StackCnt 			= 0;
		*isFirstWL			= 0;
		
		MemorySet((U8 *)(MEM_DMA_BUFF_ADDR + 4532), 0x00, 30);
	}

	/******************************************寻优运算******************************************/
	//每一页都要累加curPageIdx,每一WL都要累加curLPPage
	U16 Pages[4] = {0};							//WL内各页号
	U16 localWL = 0;							//每一轮的轮内局部WL索引

	/*
	FDL_Log("curWLPageCnt:%d",*curWLPageCnt);
	FDL_Log("curPageIdx:%d",*curPageIdx);
	FDL_Log("curLPPage:%d",*curLPPage);
	FDL_Log("curPage:%d",*curPage);
	FDL_Log("curWL:%d",*curWL);
	FDL_Log("curPage:%d",*curPage);
	FDL_Log("StackCnt:%d",*StackCnt);
	FDL_Log("gStackCntIdx:%d",*gStackCntIdx);
	FDL_Log("gStackCntIdxN:%d",*gStackCntIdxN);
	*/
	
	while (*curPageIdx<FDL_PAGE_OF_BLK && localWL<*WLPerRnd) //遍历WL
	{
		//FDL_Log("GM:%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t",GroupModel[0],GroupModel[1],GroupModel[2],GroupModel[3],GroupModel[4],GroupModel[5],GroupModel[6],GroupModel[7]);
		//解析GroupModel
		if (*StackCnt == 0)
		{
			*gStackCntIdx = *gStackCntIdxN;
			*StackCnt = GroupModel[*gStackCntIdx] - 1;
			*isFirstWL = 1;
		}
		else
		{
			(*StackCnt)--;
			*isFirstWL = 0;
		}

		/***********************************计算当前WL页号***********************************/
		*curLPPage += GroupModel[*gStackCntIdx+1];
		*curPage = *curLPPage;
		for (*curWLPageCnt = 0;;)
		{
			Pages[*curWLPageCnt] = *curPage;
			(*curPageIdx)++;
			(*curWLPageCnt)++;
			
			if (GroupModel[*gStackCntIdx+1+*curWLPageCnt] == 65535)
			{
				if (*isFirstWL)
				{
					*gStackCntIdxN = *gStackCntIdx+1+*curWLPageCnt+1;
				}
				
				break;
			}
			else
			{
				*curPage = *curLPPage+GroupModel[*gStackCntIdx+1+*curWLPageCnt];
			}
		}
		*curLayer = *curWL/(*WLPerLayer);
		
		//跳过非xLC的WL，并且在不开启页出错输出的情况下每层只选一条WL
		if ((*curWLPageCnt < *xLC) ||((!*OutWLErrEN) && (!*OutCwErrEN) && (*curWL % *WLPerLayer != (*curWL/(*WLPerLayer)) % *WLPerLayer)))
		{
			(*curWL)++;
			localWL++;
			continue;
		}
		
		//FDL_Log("WL:%d,Page:%d\t%d\t%d",*curWL,Pages[0],Pages[1],Pages[2]);
		/**************************************默认出错**************************************/
		if (*OutDefErrEN)
		{
			U32 DE_WL 		= 0;
			U32 DE_Cw[8] 	= {0};
			U8 curWLPageIdx = 0;
			for (; curWLPageIdx < *curWLPageCnt; curWLPageIdx++) 	//遍历页
			{
				U16 DE_Page = 0;
				//offset置零、读取页得到默认出错
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
				{
					U8 vR						= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					offsets[*ofsStartIdx+vR]	= 0;
				}
				
				SetOffsetForFindOV(*bChan, *bChip, offsets);


				Get_Page_Retry_Error(*bChan, *bChip, BlkIdx, Pages[curWLPageIdx], (U32)pErrTemp);

				DE_Page = *pErrTemp;
				DE_WL += DE_Page;
	
				if (*OutWLErrEN)
				{
					Ret_DE[localWL* *xLC+curWLPageIdx] = DE_Page;
				}

				if (*OutCwPageEN || *OutCwErrEN)
				{
					U16 DE_Page_Cw;
					for (cwIdx = 0; cwIdx < 8; cwIdx++)
					{
						//FDL_Log("cw:%d\tdeaddr: %d",cwIdx,((U32)dePageCwDE[cwIdx]-MEM_DMA_BUFF_ADDR));
						HAL_PMCPBitMapCountExtFilter(0,(U32)dePageCwDE[cwIdx],(U32)&DE_Page_Cw,FDL_CW_SIZE,1,1,0,2);
						if (*OutCwErrEN) Ret_DE_Cw[(localWL*8+cwIdx)**xLC+curWLPageIdx] = DE_Page_Cw;					
						if (*OutCwPageEN)
						{
							DE_Cw[cwIdx] += DE_Page_Cw;
						}
					}
				}
			}
			*pErrSumDE			+= DE_WL;
			*pErrSquareSumDE	+= DE_WL * DE_WL;
			if (*OutCwPageEN)
			{
				for (cwIdx = 0; cwIdx < 8; cwIdx++)
				{
					*pErrSumDE_Cw		+= DE_Cw[cwIdx];
					*pErrSquareSumDE_Cw	+= DE_Cw[cwIdx] * DE_Cw[cwIdx];
				}
			}
			//FDL_Log("WL:%d\tDEWL:%d\tDESUM:%d\tDE_CW[%d]=%d",localWL,DE_WL,*pErrSumDE, localWL%8,DE_Cw[localWL%8]);
		}
		
		/*************************************层偏移运算*************************************/
		//每16层才进行一次findOV，其余的WL均用层偏移
		//Blk0不进行层偏移运算，而是全WL计算，得出层偏移
		if ((!*isFirstBlk) && (*curLayer % *layerCalFreq != 0))
		{
			U8 curWLPageIdx = 0;
			U32 OE_WL		= 0;									//WL最优出错
			U32 OE_Cw[8]	= {0};									//Cw最优出错	
			for (; curWLPageIdx < *curWLPageCnt; curWLPageIdx++) 	//遍历页
			{
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) //遍历当前页读电压
				{
					U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					offsets[*ofsStartIdx+vR] = layerOfs[*curLayer* *totalVReadCnt+vR]+calOfs[vR];
				}
				
				SetOffsetForFindOV(*bChan, *bChip, offsets);
				Get_Page_Retry_Error(*bChan, *bChip, BlkIdx, Pages[curWLPageIdx], (U32)pErrTemp);

				//在此基础上，进行小幅度左右探寻
				U8 step 		= 2;
				U32 OE_Page 	= 0;
				U32 OE_Page_Cw	= 0;
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) 
				{
					U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					ShortSeek(*bChan,*bChip,BlkIdx,Pages[curWLPageIdx],step,&OE_Page,*pErrTemp,offsets,*ofsStartIdx+vR);				
				}
				
				if (*OutWLErrEN == 1)
				{
					/**********************************OVE********************************/
					//解码读出本页数据
					DataMove(*bChan, *bChip, BlkIdx, Pages[curWLPageIdx], (U32)exPage);
					//不解码读出其他页数据
					SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
					for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
					{
						for (byteIdx = 0; byteIdx<FDL_PAGE_SIZE/8; byteIdx+=4)
						{
							U64calPage[byteIdx] 	= U64dePage[byteIdx];
							U64calPage[byteIdx+1]	= U64dePage[byteIdx+1];
							U64calPage[byteIdx+2]	= U64dePage[byteIdx+2];
							U64calPage[byteIdx+3]	= U64dePage[byteIdx+3];
						}
						
						U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
						U8 exPageIdx;
						*pErrTemp = 0;
						for(exPageIdx = 0; exPageIdx<*xLC; exPageIdx++)
						{
							if (exPageIdx==curWLPageIdx)
							{
								continue;
							}
							
							DataMove(*bChan, *bChip, BlkIdx, Pages[exPageIdx], (U32)exPage);
							if (vReadJudge[(vReadStartIdx[curWLPageIdx]+vReadIdx)**xLC+exPageIdx] == 1)
							{
								//和1的页进行与运算
								for (byteIdx = 0; byteIdx<FDL_PAGE_SIZE/8; byteIdx+=4)
								{
									U64calPage[byteIdx] 	&= U64exPage[byteIdx];
									U64calPage[byteIdx+1]	&= U64exPage[byteIdx+1];
									U64calPage[byteIdx+2]	&= U64exPage[byteIdx+2];
									U64calPage[byteIdx+3]	&= U64exPage[byteIdx+3];
								}
							}

							else
							{
								//和0的页进行与非运算
								for (byteIdx = 0; byteIdx<FDL_PAGE_SIZE/8; byteIdx+=4)
								{
									U64calPage[byteIdx] 	&= ~U64exPage[byteIdx];
									U64calPage[byteIdx+1]	&= ~U64exPage[byteIdx+1];
									U64calPage[byteIdx+2]	&= ~U64exPage[byteIdx+2];
									U64calPage[byteIdx+3]	&= ~U64exPage[byteIdx+3];
								}						
							}
						}
						HAL_PMCPBitMapCountExtFilter(0,(U32)calPage,(U32)pErrTemp,FDL_PAGE_SIZE,1, 1, 0, 2);
						//if (localWL == 0)FDL_Log("R%d OVE:\t%d",vR,*pErrTemp);
						Ret_OVE[localWL* *totalVReadCnt+vR] = *pErrTemp;
					}
					SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 0;
					/**********************************OV+OE********************************/
					Ret_OE[localWL* *xLC+curWLPageIdx] = OE_Page;
					for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) 
					{
						U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
						Ret_OV[localWL* *totalVReadCnt+vR] = offsets[*ofsStartIdx+vR]; //将读电压写入
					}
				}
				OE_WL += OE_Page;
				/**********************************Cw_pageOE+OV+OE********************************/
				//在WL大体确定了最优出错后，再进行Cw的细分
				if (*OutCwErrEN || *OutCwPageEN)
				{
					for (cwIdx = 0; cwIdx < 8; cwIdx++)
					{
						for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) //遍历当前页读电压
						{
							//抛物线寻优后通过不断缩小的步长进行左右探寻
							U8 vR 		= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
							U8 step 	= 1; //初始探寻步长
							U32 OVETemp;
							U32 oriErr;
							HAL_PMCPBitMapCountExtFilter(0,(U32)dePage+FDL_CW_SIZE*cwIdx,(U32)(&oriErr),FDL_CW_SIZE,1, 1, 0, 2);
							ShortSeek_Cw(*bChan,*bChip,BlkIdx,Pages[curWLPageIdx],step,&OVETemp,oriErr,offsets,*ofsStartIdx+vR,cwIdx);
							
							//更新页最优出错
							OE_Page_Cw 	= OVETemp;
							*pErrTemp	= OVETemp;

							//OV
							if (*OutCwErrEN) Ret_OV_Cw[(localWL*8+cwIdx)**totalVReadCnt+vR] = offsets[*ofsStartIdx+vR];
						}
						//pageOE
						OE_Cw[cwIdx] += OE_Page_Cw;
						
						//OE
						if (*OutCwErrEN)
						{
							Ret_OE_Cw[(localWL*8+cwIdx)**xLC+curWLPageIdx] = OE_Page_Cw;
							//读出数据，为OVE计算做准备
							for (byteIdx = 0; byteIdx < FDL_CW_SIZE; byteIdx +=4)
							{
								U64dePageCw[cwIdx][byteIdx]   = U64haPageCw[cwIdx][byteIdx];
								U64dePageCw[cwIdx][byteIdx+1] = U64haPageCw[cwIdx][byteIdx+1];
								U64dePageCw[cwIdx][byteIdx+2] = U64haPageCw[cwIdx][byteIdx+2];
								U64dePageCw[cwIdx][byteIdx+3] = U64haPageCw[cwIdx][byteIdx+3];
							}
						}
					}				
				}
						
				/**********************************Cw_OVE********************************/
				if (*OutCwErrEN)
				{
					//思路为多Cw拼在一起，统一计算OVE
					SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
					for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
					{
						for (byteIdx = 0; byteIdx<FDL_CW_SIZE; byteIdx+=4)//FDL_CW_SIZE只是充当FDL_PAGE_SIZE/8的作用
						{
							U64calPage[byteIdx] 	= U64dePageCw0[byteIdx];
							U64calPage[byteIdx+1] 	= U64dePageCw0[byteIdx+1];
							U64calPage[byteIdx+2] 	= U64dePageCw0[byteIdx+2];
							U64calPage[byteIdx+3] 	= U64dePageCw0[byteIdx+3];
						}

						U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
						U8 exPageIdx;
						*pErrTemp = 0;
						for(exPageIdx = 0; exPageIdx<*xLC; exPageIdx++)
						{
							if (exPageIdx==curWLPageIdx)
							{
								continue;
							}
							
							DataMove(*bChan, *bChip, BlkIdx, Pages[exPageIdx], (U32)exPage);
							if (vReadJudge[(vReadStartIdx[curWLPageIdx]+vReadIdx)**xLC+exPageIdx] == 1)
							{
								//和1的页进行与运算
								for (byteIdx = 0; byteIdx<FDL_CW_SIZE; byteIdx+=4)
								{
									U64calPage[byteIdx] 	&= U64exPage[byteIdx];
									U64calPage[byteIdx+1] 	&= U64exPage[byteIdx+1];
									U64calPage[byteIdx+2] 	&= U64exPage[byteIdx+2];
									U64calPage[byteIdx+3] 	&= U64exPage[byteIdx+3];
								}
							}

							else
							{
								//和0的页进行与非运算
								for (byteIdx = 0; byteIdx<FDL_CW_SIZE; byteIdx+=4)
								{
									U64calPage[byteIdx] 	&= ~U64exPage[byteIdx];
									U64calPage[byteIdx+1] 	&= ~U64exPage[byteIdx+1];
									U64calPage[byteIdx+2] 	&= ~U64exPage[byteIdx+2];
									U64calPage[byteIdx+3] 	&= ~U64exPage[byteIdx+3];
								}						
							}
						}
						for (cwIdx = 0; cwIdx < 8; cwIdx++)
						{
							HAL_PMCPBitMapCountExtFilter(0,(U32)(calPage+FDL_CW_SIZE*cwIdx),(U32)pErrTemp,FDL_CW_SIZE,1,1,0,2);
							Ret_OVE_Cw[(localWL*8+cwIdx)* *totalVReadCnt+vR] = *pErrTemp;					
						}
					}			
					SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 0;		
				}				
			}

			//WL最优出错及其平方累加，得到全块的最优出错和以及最优出错平方和
			*pErrSumOE 			+= OE_WL;
			*pErrSquareSumOE	+= OE_WL * OE_WL;
			if (*OutCwPageEN)
			{
				for (cwIdx = 0; cwIdx < 8; cwIdx++)
				{
					*pErrSumOE_Cw		+= OE_Cw[cwIdx];
					*pErrSquareSumOE_Cw	+= OE_Cw[cwIdx] * OE_Cw[cwIdx];
				}
			}			
			//FDL_Log("WL:%d\tOE_WL:%d\tErrSumOE=\t%d",*curWL,OE_WL,*pErrSumOE);
			(*curWL)++;
			localWL++;
			continue;
		}

		/**************************************寻优运算**************************************/			
		U8 curWLPageIdx = 0;
		U32 OE_WL		= 0;									//WL最优出错
		U32 OE_Cw[8]	= {0};									//Cw最优出错		
		for (; curWLPageIdx < *curWLPageCnt; curWLPageIdx++) 	//遍历页
		{	
			U8 RRTIdx 		= 0; 								//通用索引:RRT
			U32 OE_Page		= 0; 								//页最优出错
			U32 OE_Page_Cw	= 0; 								//页最优出错Cw
			U32 RRT_Err[3] 	= { 								//储存三点出错
				FDL_PAGE_SIZE*8,
				FDL_PAGE_SIZE*8,
				FDL_PAGE_SIZE*8
			}; 

			/************************************确定拐点************************************/
			for (; RRTIdx < *RRTGroupCnt; RRTIdx++) 			//遍历RRT
			{
				//根据RRT设置offset、读取页并计算出错
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
				{
					U8 vR 						= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					offsets[*ofsStartIdx+vR] 	= RRTList[RRTIdx*(*totalVReadCnt+1)+vR];
				}
				SetOffsetForFindOV(*bChan, *bChip, offsets);
				Get_Page_Retry_Error(*bChan, *bChip, BlkIdx, Pages[curWLPageIdx], (U32)pErrTemp);
				//根据出错情况赋值不同的变量
				if (*pErrTemp <= RRT_Err[1])
				{
					//正常情况下，第一组RRT会经过此条件
					RRT_Err[0] = RRT_Err[1];
					RRT_Err[1] = *pErrTemp;
				}
				else if (*pErrTemp > RRT_Err[1])
				{
					//一旦发现出错数有上升，则退出循环
					RRT_Err[2] = *pErrTemp;
					break;
				}
			}

			/*****************************抛物线寻优+递归二分搜索****************************/
			for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) //遍历当前页读电压
			{
				/*********************************抛物线寻优*********************************/
				U8 vR 					= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx]; //映射到vRead
				signed char OV 			= 0;
				signed char RRT_sel[3] 	= {
					RRTIdx>1? RRTList[(RRTIdx-2)*(*totalVReadCnt+1)+vR] : -128,
					RRTList[(RRTIdx-1)*(*totalVReadCnt+1)+vR],
					RRTIdx<*RRTGroupCnt? RRTList[RRTIdx*(*totalVReadCnt+1)+vR] : 127
				};
				//计算抛物线顶点横坐标OV
				if ((RRT_sel[0] == RRT_sel[1]) || (RRT_sel[1] == RRT_sel[2]))
				{
					OV = RRT_sel[1];
				}
				else
				{
					//由于环境不支持float，使用缩放因子1000，并用int类型确保不会溢出
					*k1 = ((int)RRT_Err[1] - (int)RRT_Err[0]) * 1000 / (RRT_sel[1] - RRT_sel[0]);
					*k2 = ((int)RRT_Err[2] - (int)RRT_Err[1]) * 1000 / (RRT_sel[2] - RRT_sel[1]);						
					if (*k1 == *k2)
					{
						OV = RRT_sel[1];
					}
					else
					{
						*a = (*k1 - *k2) / (RRT_sel[0] - RRT_sel[2]);
						*b = *k1 - *a * (RRT_sel[0] + RRT_sel[1]);
						OV = (signed char)(-*b / (2 * *a)); //由于还需要进行左右探寻，这里不作四舍五入
					}
				}
				//得到抛物线顶点的出错数
				offsets[*ofsStartIdx+vR] = OV; //将OV存入offsets,之后无需单独传OV
			}
			SetOffsetForFindOV(*bChan, *bChip, offsets);
			Get_Page_Retry_Error(*bChan, *bChip, BlkIdx, Pages[curWLPageIdx], (U32)pErrTemp);
				
			/**********************************递归二分搜索******************************/
			for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) 
			{
				U8 vR	= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx]; //映射到vRead
				U8 step = 4; //初始探寻步长
				U32 OVETemp;
				ShortSeek(*bChan,*bChip,BlkIdx,Pages[curWLPageIdx],step,&OVETemp,*pErrTemp,offsets,*ofsStartIdx+vR);
				//FDL_Log("result:\tvRead:%d\tOV:%d\tOVE:%d\t",vRead[curWLPageIdx][vReadIdx],(signed char)(offsets[vReadStartIdx+vRead[curWLPageIdx][vReadIdx]]),OVETemp);

				//更新页最优出错
				OE_Page 	= OVETemp;
				*pErrTemp 	= OVETemp;
			}

			/**********************************再次递归二分搜索******************************/
			//在每个页的非最右读电压找到最优之前，多次读页之间的差异可能会受到最右读电压的影响
			//因此在最右读电压找到最优之后，再次对所有读电压进行一次寻优
			for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) //遍历当前页读电压
			{
				//抛物线寻优后通过不断缩小的步长进行左右探寻
				U8 vR 	= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
				U8 step = 2; //初始探寻步长
				U32 OVETemp;
				ShortSeek(*bChan,*bChip,BlkIdx,Pages[curWLPageIdx],step,&OVETemp,*pErrTemp,offsets,*ofsStartIdx+vR);

				//更新页最优出错
				OE_Page 	= OVETemp;
				*pErrTemp 	= OVETemp;
			}
			//页最优出错累加，得到WL最优出错
			OE_WL += OE_Page;

			/***********************************WL_OV+OE+OVE*********************************/
			if (*OutWLErrEN)
			{
				U8 vReadIdx = 0;
				Ret_OE[localWL* *xLC+curWLPageIdx] = OE_Page;
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) 
				{
					U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					Ret_OV[localWL**totalVReadCnt+vR] = offsets[*ofsStartIdx+vR]; //将读电压写入
				}
				//解码读出数据
				DataMove(*bChan, *bChip, BlkIdx, Pages[curWLPageIdx], (U32)dePage);

				//不解码读出其他页数据
				SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
				{
					for (byteIdx = 0; byteIdx<FDL_PAGE_SIZE/8; byteIdx+=4)
					{
						U64calPage[byteIdx] 	= U64dePage[byteIdx];
						U64calPage[byteIdx+1] 	= U64dePage[byteIdx+1];
						U64calPage[byteIdx+2] 	= U64dePage[byteIdx+2];
						U64calPage[byteIdx+3] 	= U64dePage[byteIdx+3];
					}

					U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					U8 exPageIdx;
					*pErrTemp = 0;
					for(exPageIdx = 0; exPageIdx<*xLC; exPageIdx++)
					{
						if (exPageIdx==curWLPageIdx)
						{
							continue;
						}
						
						DataMove(*bChan, *bChip, BlkIdx, Pages[exPageIdx], (U32)exPage);
						if (vReadJudge[(vReadStartIdx[curWLPageIdx]+vReadIdx)**xLC+exPageIdx] == 1)
						{
							//和1的页进行与运算
							for (byteIdx = 0; byteIdx<FDL_PAGE_SIZE/8; byteIdx+=4)
							{
								U64calPage[byteIdx] 	&= U64exPage[byteIdx];
								U64calPage[byteIdx+1] 	&= U64exPage[byteIdx+1];
								U64calPage[byteIdx+2] 	&= U64exPage[byteIdx+2];
								U64calPage[byteIdx+3] 	&= U64exPage[byteIdx+3];
							}
						}

						else
						{
							//和0的页进行与非运算
							for (byteIdx = 0; byteIdx<FDL_PAGE_SIZE/8; byteIdx+=4)
							{
								U64calPage[byteIdx] 	&= ~U64exPage[byteIdx];
								U64calPage[byteIdx+1] 	&= ~U64exPage[byteIdx+1];
								U64calPage[byteIdx+2] 	&= ~U64exPage[byteIdx+2];
								U64calPage[byteIdx+3] 	&= ~U64exPage[byteIdx+3];
							}						
						}
					}
					HAL_PMCPBitMapCountExtFilter(0,(U32)calPage,(U32)pErrTemp,FDL_PAGE_SIZE,1, 1, 0, 2);
					Ret_OVE[localWL* *totalVReadCnt+vR] = *pErrTemp;
					//if (localWL == 0)FDL_Log("R%d OVE:\t%d",vR,*pErrTemp);
				}			
				SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 0;
			}
			
			/**********************************Cw_pageOE+OV+OE********************************/
			//在WL大体确定了最优出错后，再进行Cw的细分
			if (*OutCwErrEN || *OutCwPageEN)
			{
				for (cwIdx = 0; cwIdx < 8; cwIdx++)
				{
					for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++) //遍历当前页读电压
					{
						//抛物线寻优后通过不断缩小的步长进行左右探寻
						U8 vR 		= vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
						U8 step 	= 1; //初始探寻步长
						U32 OVETemp;
						U32 oriErr;
						HAL_PMCPBitMapCountExtFilter(0,(U32)dePage+FDL_CW_SIZE*cwIdx,(U32)(&oriErr),FDL_CW_SIZE,1, 1, 0, 2);
						ShortSeek_Cw(*bChan,*bChip,BlkIdx,Pages[curWLPageIdx],step,&OVETemp,oriErr,offsets,*ofsStartIdx+vR,cwIdx);
						
						//更新页最优出错
						OE_Page_Cw 	= OVETemp;
						*pErrTemp	= OVETemp;

						//OV
						if (*OutCwErrEN) Ret_OV_Cw[(localWL*8+cwIdx)**totalVReadCnt+vR] = offsets[*ofsStartIdx+vR];
					}
					//pageOE
					OE_Cw[cwIdx] += OE_Page_Cw;

					//OE
					if (*OutCwErrEN)
					{
						Ret_OE_Cw[(localWL*8+cwIdx)**xLC+curWLPageIdx] = OE_Page_Cw;
						//读出数据，为OVE计算做准备
						for (byteIdx = 0; byteIdx < FDL_CW_SIZE; byteIdx +=4)
						{
							U64dePageCw[cwIdx][byteIdx]   = U64haPageCw[cwIdx][byteIdx];
							U64dePageCw[cwIdx][byteIdx+1] = U64haPageCw[cwIdx][byteIdx+1];
							U64dePageCw[cwIdx][byteIdx+2] = U64haPageCw[cwIdx][byteIdx+2];
							U64dePageCw[cwIdx][byteIdx+3] = U64haPageCw[cwIdx][byteIdx+3];
						}
					}
				}				
			}

			/**********************************Cw_OVE********************************/
			if (*OutCwErrEN)
			{
				//思路为多Cw拼在一起，统一计算OVE
				SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 1;
				for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
				{
					for (byteIdx = 0; byteIdx<FDL_CW_SIZE; byteIdx+=4)//FDL_CW_SIZE只是充当FDL_PAGE_SIZE/8的作用
					{
						U64calPage[byteIdx] 	= U64dePageCw0[byteIdx];
						U64calPage[byteIdx+1] 	= U64dePageCw0[byteIdx+1];
						U64calPage[byteIdx+2] 	= U64dePageCw0[byteIdx+2];
						U64calPage[byteIdx+3] 	= U64dePageCw0[byteIdx+3];
					}

					U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
					U8 exPageIdx;
					*pErrTemp = 0;
					for(exPageIdx = 0; exPageIdx<*xLC; exPageIdx++)
					{
						if (exPageIdx==curWLPageIdx)
						{
							continue;
						}
						
						DataMove(*bChan, *bChip, BlkIdx, Pages[exPageIdx], (U32)exPage);
						if (vReadJudge[(vReadStartIdx[curWLPageIdx]+vReadIdx)**xLC+exPageIdx] == 1)
						{
							//和1的页进行与运算
							for (byteIdx = 0; byteIdx<FDL_CW_SIZE; byteIdx+=4)
							{
								U64calPage[byteIdx] 	&= U64exPage[byteIdx];
								U64calPage[byteIdx+1] 	&= U64exPage[byteIdx+1];
								U64calPage[byteIdx+2] 	&= U64exPage[byteIdx+2];
								U64calPage[byteIdx+3] 	&= U64exPage[byteIdx+3];
							}
						}

						else
						{
							//和0的页进行与非运算
							for (byteIdx = 0; byteIdx<FDL_CW_SIZE; byteIdx+=4)
							{
								U64calPage[byteIdx] 	&= ~U64exPage[byteIdx];
								U64calPage[byteIdx+1] 	&= ~U64exPage[byteIdx+1];
								U64calPage[byteIdx+2] 	&= ~U64exPage[byteIdx+2];
								U64calPage[byteIdx+3] 	&= ~U64exPage[byteIdx+3];
							}						
						}
					}
					for (cwIdx = 0; cwIdx < 8; cwIdx++)
					{
						HAL_PMCPBitMapCountExtFilter(0,(U32)(calPage+FDL_CW_SIZE*cwIdx),(U32)pErrTemp,FDL_CW_SIZE,1,1,0,2);
						Ret_OVE_Cw[(localWL*8+cwIdx)* *totalVReadCnt+vR] = *pErrTemp;					
					}
					//if (localWL == 0)FDL_Log("R%d OVE:\t%d",vR,*pErrTemp);
				}			
				SG_DMA2_CH(*bChan)->rRND_CTRL.bits.RND_BYPASS = 0;		
			}

			/************************************计算层平均OV********************************/
			//Blk0计算平均OV,其余Blk计算层偏移
			if (!*OutWLErrEN && !*OutWLErrEN)
			{
				if (*isFirstBlk)
				{
					U8 vReadIdx = 0;
					for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
					{
						U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
						layerOfs[*curLayer* *totalVReadCnt+vR] = (signed char)offsets[*ofsStartIdx+vR];
					}
				}
				else
				{
					U8 vReadIdx = 0;
					for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
					{
						//计算当前层与blk0的当前层的阈值均值之差
						U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
						calOfs[vR] = (signed char)offsets[*ofsStartIdx+vR] - layerOfs[*curLayer* *totalVReadCnt+vR];
					}
				}
			}
			else
			{
				if (*isFirstBlk)
				{

					if ((*curWL+1) % *WLPerLayer == 0)
					{
						U8 vReadIdx = 0;
						for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
						{
							U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
							layerOfs[*curLayer* *totalVReadCnt+vR] = tempOfs[vR]/(*WLPerLayer);
							tempOfs[vR] = 0;
						}
					}
					else
					{
						U8 vReadIdx = 0;
						for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
						{
							U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
							tempOfs[vR] += (signed char)offsets[*ofsStartIdx+vR];
						}
					}
				}
				else
				{
					if ((*curWL+1) % *WLPerLayer == 0)
					{
						U8 vReadIdx = 0;
						for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
						{
							U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
							calOfs[vR] = tempOfs[vR]/(*WLPerLayer) - layerOfs[*curLayer* *totalVReadCnt+vR];
							tempOfs[vR] = 0;
						}
					}
					else
					{
						U8 vReadIdx = 0;
						for (vReadIdx = 0; vReadIdx < vReadCnt[curWLPageIdx]; vReadIdx++)
						{
							U8 vR = vRead[vReadStartIdx[curWLPageIdx]+vReadIdx];
							tempOfs[vR] += (signed char)offsets[*ofsStartIdx+vR];
						}
					}
				}
			}
		}
		
		/************************************累加计算结果************************************/
		//WL最优出错及其平方累加，得到全块的最优出错和以及最优出错平方和
		*pErrSumOE 			+= OE_WL;
		*pErrSquareSumOE	+= OE_WL * OE_WL;
		//FDL_Log("OEWL:%d\tsum:%d\tssum:%d", OE_WL,*pErrSumOE,*pErrSquareSumOE);
		if (*OutCwPageEN)
		{
			
			for (cwIdx = 0; cwIdx < 8; cwIdx++)
			{
				*pErrSumOE_Cw		+= OE_Cw[cwIdx];
				*pErrSquareSumOE_Cw	+= OE_Cw[cwIdx] * OE_Cw[cwIdx];
			}
			
			//FDL_Log("WL=%d\tOE_Cw:%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",localWL,OE_Cw[0],OE_Cw[1],OE_Cw[2],OE_Cw[3],OE_Cw[4],OE_Cw[5],OE_Cw[6],OE_Cw[7]);
		}

		(*curWL)++;
		localWL++;
	}
	
	/**************************************储存计算结果**************************************/
	//考虑到精度，最优出错均值与标准差由host进行计算
	//FDL_Log("Blk:\t%d\tRnd:\t%d\tErrSum:\t%d\tErrSumSquare:\t%d\tReadCnt:%d",BlkIdx,localWL/200-1,*pErrSumOE,*pErrSquareSumOE,*ReadCnt);
	FDL_Log("Blk:%d\tRnd:%d\tErrSum:%d\tErrSumSquare:%d\tErrSumCw:%d\tErrSumSquareCw:%d",BlkIdx,(*curWL)/(*WLPerRnd),*pErrSumOE,*pErrSquareSumOE,*pErrSumOE_Cw,*pErrSquareSumOE_Cw);
	//U32 time=__get_pfm_count();
	//FDL_Log("time:%u us",time/200);
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void ShortSeek(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, signed char step, U32 *ret, U32 oriErr, U8 *offsets, U8 offsetIdx)
{
	//该函数为左右探寻的递归函数
	//使用While代替递归以减少传参和函数调用开销
	//为了减少由于读取误差带来的无限递归可能，递归点本身的出错将不会再次读取
	//FDL_Log("ori:\t%d\toffsetIdx:%d\t",oriErr,offsetIdx);
	
	//出错数存地址
	U32 dErrTempAdd = MEM_DMA_BUFF_ADDR+4840; 
	U32 *pErrTemp = (U32 *)dErrTempAdd;

	//int judge = dPageAddr>=0&&dPageAddr<=2;

	while (step) //这里只对step是否为0进行判断，+-1/2均等于0
	{
		if (step > 0) //先右后左
		{
			if ((U8)(offsets[offsetIdx]+128) <= 255-step)
			{
				offsets[offsetIdx] += step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd);
				if (*pErrTemp < oriErr)
				{	
					//if (judge) FDL_Log("PA:%-4d\tstep=%d\toffset->%-3d\tPositive[r]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] -= step;
				}				
			}

			if ((U8)(offsets[offsetIdx]+128 >= step))
			{
				offsets[offsetIdx] -= step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd);
				if (*pErrTemp < oriErr)
				{
					step *= -1; //当往反方向跳变时，调转step方向
					//if (judge) FDL_Log("PA:%-4d\tstep=%d\toffset->%-3d\tNegative[l]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] += step;
				}
			}
		}
		else //先左后右
		{
			if ((U8)(offsets[offsetIdx]+128 >= -step))
			{
				offsets[offsetIdx] += step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd);
				if (*pErrTemp < oriErr)
				{	
					//if (judge) FDL_Log("PA:%-4d\tstep=%d\toffset->%-3d\tPositive[l]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] -= step;
				}

			}
			if ((U8)(offsets[offsetIdx]+128) <= 255+step)
			{
				offsets[offsetIdx] -= step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd);
				if (*pErrTemp < oriErr)
				{
					step *= -1; //当往反方向跳变时，调转step方向
					//if (judge) FDL_Log("PA:%-4d\tstep=%d\toffset->%-3d\tNegative[r]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] += step;
				}				
			}

		}

		//若该点最小，则步长减半后继续该点
		//if (judge) FDL_Log("PA:%-4d\tstep=%d\toffset->%-3d\tstill\t\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,step,offsets[offsetIdx],oriErr,oriErr,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
		step /= 2;
	}
	*ret = oriErr;
}

void ShortSeek_Cw(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, signed char step, U32 *ret, U32 oriErr, U8 *offsets, U8 offsetIdx, U8 Cw)
{
	//该函数为左右探寻的递归函数
	//使用While代替递归以减少传参和函数调用开销
	//为了减少由于读取误差带来的无限递归可能，递归点本身的出错将不会再次读取
	//FDL_Log("ori:\t%d\toffsetIdx:%d\t",oriErr,offsetIdx);
	
	//出错数存地址
	U32 dErrTempAdd = MEM_DMA_BUFF_ADDR+4840; 
	U32 *pErrTemp = (U32 *)dErrTempAdd;
	
	//int judge = dPageAddr>=0&&dPageAddr<=2;

	while (step) //这里只对step是否为0进行判断，+-1/2均等于0
	{
		if (step > 0) //先右后左
		{
			if ((U8)(offsets[offsetIdx]+128) <= 255-step)
			{
				offsets[offsetIdx] += step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error_Cw(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd, Cw);
				if (*pErrTemp < oriErr)
				{	
					//if (judge) FDL_Log("PA:%-4d\tCw:%-2d\tstep=%d\toffset->%-3d\tPositive[r]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,Cw,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] -= step;
				}				
			}

			if ((U8)(offsets[offsetIdx]+128 >= step))
			{
				offsets[offsetIdx] -= step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error_Cw(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd, Cw);
				if (*pErrTemp < oriErr)
				{
					step *= -1; //当往反方向跳变时，调转step方向
					//if (judge) FDL_Log("PA:%-4d\tCw:%-2d\tstep=%d\toffset->%-3d\tNegative[l]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,Cw,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] += step;
				}
			}
		}
		else //先左后右
		{
			if ((U8)(offsets[offsetIdx]+128 >= -step))
			{
				offsets[offsetIdx] += step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error_Cw(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd, Cw);
				if (*pErrTemp < oriErr)
				{	
					//if (judge) FDL_Log("PA:%-4d\tCw:%-2d\tstep=%d\toffset->%-3d\tPositive[l]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,Cw,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] -= step;
				}

			}
			if ((U8)(offsets[offsetIdx]+128) <= 255+step)
			{
				offsets[offsetIdx] -= step;
				SetOffsetForFindOV(bChan, bChip, offsets);
				Get_Page_Retry_Error_Cw(bChan, bChip, dBlkAddr, dPageAddr, dErrTempAdd, Cw);
				if (*pErrTemp < oriErr)
				{
					step *= -1; //当往反方向跳变时，调转step方向
					//if (judge) FDL_Log("PA:%-4d\tCw:%-2d\tstep=%d\toffset->%-3d\tNegative[r]\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,Cw,step,offsets[offsetIdx],oriErr,*pErrTemp,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
					oriErr = *pErrTemp;
					continue;
				}
				else

				{


					offsets[offsetIdx] += step;
				}				
			}

		}

		//若该点最小，则步长减半后继续该点
		//if (judge) FDL_Log("PA:%-4d\tCw:%-2d\tstep=%d\toffset->%-3d\tstill\t\t%-5d\t%-5d\tofs:%3d,%3d,%3d,%3d,%3d,%3d,%3d",dPageAddr,Cw,step,offsets[offsetIdx],oriErr,oriErr,(signed char)(offsets[4]),(signed char)(offsets[5]),(signed char)(offsets[6]),(signed char)(offsets[7]),(signed char)(offsets[8]),(signed char)(offsets[9]),(signed char)(offsets[10]));
		step /= 2;
	}
	*ret = oriErr;
}

void Get_Page_Retry_Error(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, U32 dErrTempAddr)
{
	U32 dBuffAddr = MEM_DMA_BUFF_ADDR+10240;
	DataMove(bChan, bChip, dBlkAddr, dPageAddr, dBuffAddr);
	HAL_PMCPBitMapCountExtFilter(0,dBuffAddr,dErrTempAddr,FDL_PAGE_SIZE,1,1,0,2);
}

void Get_Page_Retry_Error_Cw(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, U32 dErrTempAddr, U8 Cw)
{
	U32 dBuffAddr = MEM_DMA_BUFF_ADDR+30800;
	DataMove_Cw(bChan, bChip, dBlkAddr, dPageAddr, dBuffAddr, Cw);
	HAL_PMCPBitMapCountExtFilter(0,dBuffAddr+((FDL_CW_SIZE*Cw)&1023),dErrTempAddr,FDL_CW_SIZE,1,1,0,2);
}

void DataMove(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, U32 dBuffAddr) //将页数据移动到指定内存地址
{
	//数据是否解码需要在函数外部定义，避免性能浪费
	//传参基本是U8和U32，均低于/等于U32类型的指针大小，因此无需指针传递
	U32	dRowAddr, WlAddr;
	U8 PreCMD;

	//U32 *ReadCnt		= (U32 *)(MEM_DMA_BUFF_ADDR+19100); 	//读页次数
	//(*ReadCnt)++;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

#if(NAND_FLASH == Hynix_3DV8_TLC)
	PreCMD = dPageAddr%3 + 1;
	WlAddr = dPageAddr/3;
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + WlAddr;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0);//种子设定
	FA_SendOprModeCmd(bChan, 1);
	HAL_CQSendFlashCmd(bChan, PreCMD);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend5Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, 0, dRowAddr);
#elif((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X4_9060))
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + dPageAddr;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0); //种子设定
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, 0, dRowAddr);
#elif(NAND_FLASH == SanDisk_BiCS8_TLC)
	PreCMD = dPageAddr%3 + 1;
	WlAddr = dPageAddr/3;
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + WlAddr/5*8+WlAddr%5;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0);//种子设定
	HAL_CQSendFlashCmd(bChan, FL_CMD_RETRY_ENABLE);
	HAL_CQSendFlashCmd(bChan, PreCMD);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, 0, dRowAddr);
#elif(NAND_FLASH == SAMSUNG_3DV7_TLC)
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + dPageAddr;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0); //种子设定
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, 0, dRowAddr);
#endif


	HAL_CQStartDMA2(bChan, dBuffAddr, DMA2_RD_TO_BUFF, 0, 0, FDL_SECT_OF_PAGE, JOINT_NONE, MEM_DMA2_INFO_ADDR);

	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	HAL_CQWaitDone(bChan);
}

void DataMove_Cw(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, U32 dBuffAddr, U8 Cw) //将页数据移动到指定内存地址
{
	//数据是否解码需要在函数外部定义，避免性能浪费
	//传参基本是U8和U32，均低于/等于U32类型的指针大小，因此无需指针传递
	U32	dRowAddr, WlAddr;
	U8 PreCMD;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

	//partical Read
#if(NAND_FLASH == Hynix_3DV8_TLC)
	PreCMD = dPageAddr%3 + 1;
	WlAddr = dPageAddr/3;
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + WlAddr;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0);//种子设定
	FA_SendOprModeCmd(bChan, 1);
	HAL_CQSendFlashCmd(bChan, PreCMD);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend5Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, (FDL_CW_SIZE*Cw)&~1023, dRowAddr);
#elif((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X4_9060))
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + dPageAddr;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0); //种子设定
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, (FDL_CW_SIZE*Cw)&~1023, dRowAddr);
#elif(NAND_FLASH == SanDisk_BiCS8_TLC)
	PreCMD = dPageAddr%3 + 1;
	WlAddr = dPageAddr/3;
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + WlAddr/5*8+WlAddr%5;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0);//种子设定
	HAL_CQSendFlashCmd(bChan, FL_CMD_RETRY_ENABLE);
	HAL_CQSendFlashCmd(bChan, PreCMD);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, (FDL_CW_SIZE*Cw)&~1023, dRowAddr);	
#elif(NAND_FLASH == SAMSUNG_3DV7_TLC)
	dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + dPageAddr;
	HAL_CQSetRndSeed(bChan, dPageAddr, 0);//种子设定
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, (FDL_CW_SIZE*Cw)&~1023, dRowAddr);
#endif


	
	//DMA read 固定6个sector
	HAL_CQStartDMA2(bChan, dBuffAddr, DMA2_RD_TO_BUFF, 0, 0, 6, JOINT_NONE, MEM_DMA2_INFO_ADDR);

	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	HAL_CQWaitDone(bChan);
}

void SetOffsetForFindOV(U8 bChan, U8 bChip, U8 *offsets)
{
	//这里的offsets排布如下:SLC[1]+MLC[3]+TLC[7]+QLC[15]共计26字节
	//即offsets[0]为SLC,offsets[1-3]为MLC,offsets[4-10]为TLC,offsets[11-25]为QLC
	U8 bLUN = 0;
#if((NAND_FLASH == INTEL_N38B)) //待修改

#elif((NAND_FLASH == INTEL_N48A)) //待修改

#elif((NAND_FLASH == INTEL_N38A)) //待修改

#elif((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV7_TLC))
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);

	U8 idata[4] = {offsets[4],offsets[8],offsets[6],offsets[10]};
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x89);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,idata,4);

	idata[0] = offsets[5];
	idata[1] = offsets[7];
	idata[2] = offsets[9];
	idata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x8A);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,idata,4);

	idata[0] = offsets[0];
	idata[1] = 0;
	idata[2] = 0;
	idata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x8D);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,idata,4);
#elif((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X3_9070))
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	U8 ydata[4] = {offsets[4],offsets[8],0,0};
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[0] = offsets[5];
	ydata[1] = offsets[7];
	ydata[2] = offsets[9];
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA1);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
	
	ydata[0] = offsets[6];
	ydata[1] = offsets[10];
	ydata[2] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA2);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[0] = offsets[0];
	ydata[1] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA3);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#elif((NAND_FLASH == YMTC_X2_6070)) //待修改

#elif((NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080)) //待修改

#elif((NAND_FLASH == YMTC_X4_9060))
	U8 ydata[4] = {offsets[4],offsets[8],0,0};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);
	
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA0);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[0] = offsets[5];
	ydata[1] = offsets[7];
	ydata[2] = offsets[9];
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA1);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
	
	ydata[0] = offsets[6];
	ydata[1] = offsets[10];
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xA2);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#elif (NAND_FLASH == Hynix_3DV8_TLC)
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitDone(bChan);

	U8 Param[4] = {0,0,0,0};

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0xF5);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,Param,4);
	
	U8 addr;
	for (addr = 0xA9; addr <= 0xAF; addr++)
	{
		HAL_MCU_SetCe(bChan,bChip,1);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
		HAL_CQWaitDone(bChan);
		Param[0]=offsets[addr-0xA9+4];
		HAL_CQSendFlashCmd(bChan,0x36);
		HAL_CQSendAddr(bChan,addr);
		HAL_CQWaitDone(bChan);
		HAL_MCUSendNByteDataByDqs(bChan,Param,1);
		HAL_CQSendFlashCmd(bChan,0x16);
		HAL_CQWaitDone(bChan);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);	
	}
#elif ((NAND_FLASH == SanDisk_BiCS8_TLC))
	U8 ydata[4] = {offsets[4],offsets[6],offsets[8],offsets[10]};
	HAL_CQWaitDone(bChan);
	HAL_MCU_SetCe(bChan,bChip,1);

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x12);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);

	ydata[0] = offsets[5];
	ydata[1] = offsets[7];
	ydata[2] = offsets[9];
	ydata[3] = 0;
	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
	HAL_MCU_SendOneAddr(bChan,0x13);
	Delay(4);
	HAL_MCUSendNByteDataByDqs(bChan,ydata,4);
#endif

}

void FA_GetExtendedFA(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bData[4] = {0,0,0,0};
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip;
	U8 bAdd1=(U8)pCtrl->FeatureAddress;
	U8 bAdd2=(U8)pCtrl->P4;
	U8 bLUN =(U8)pCtrl->bRBMode;
#if ((NAND_FLASH>4095)&&(NAND_FLASH<5120))	//YMTC flash
	U8 PmIdx = bLUN;
#else
	U8 PmIdx = 1;
#endif
	U16 P1;
	U16 DmaLenTemp = SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN;
	HostCmdResponse();
	HAL_MCU_SetCe(bChan,bChip,1);

	if(pCtrl->dMode == 0)
	{
		HAL_NFGetExtFeature(bChan, bChip, bAdd1, bAdd2, bLUN, (U8 *)bData, 4);
		MemoryCopy((U8 *)MEM_DMA_BUFF_ADDR, (U8 *)bData, 4);
	}
	else if(pCtrl->dMode==1)
	{
		U8 * pBuff = (U8 *)MEM_DMA_BUFF_ADDR;
		HAL_MCU_SetCe(bChan,bChip,1);
		if(pCtrl->dPageAddr==2)
		{
			for(P1 = 0; P1<256; ++P1)//Add2遍历
			{
				bAdd2 = (U8)P1;
				HAL_NFGetExtFeature(bChan, bChip, bAdd1, bAdd2, bLUN, (U8 *)bData, 4);
				pBuff[P1]= bData[PmIdx];
			}
		}
		else
		{
			for(P1 = 0; P1<256; ++P1)//Add1遍历
			{
				bAdd1 = (U8)P1;
				HAL_NFGetExtFeature(bChan, bChip, bAdd1, bAdd2, bLUN, (U8 *)bData, 4);
				pBuff[P1]= bData[PmIdx];
			}
		}
	}

	HAL_CQWaitDone(bChan);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_SetExtendedFA(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip;
	U8 bLUN =(U8)pCtrl->bRBMode;
	U8 Add1 = (U8)pCtrl->FeatureAddress;
	U8 Add2 = (U8)pCtrl->P4;
	U8 Param = (U8)pCtrl->bBlklist[0];
	U8 bData[4] = {Param,Param,Param,Param};
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitDone(bChan);

	HAL_NFSetExtFeature(bChan, bChip, Add1, Add2, bLUN, bData, 2);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_SetParamter(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	//FDL_Log("Set Param CH%d CE%d Add%02X Param%02X PreCmdEnbale:%d", (U8)pCtrl->bChan,(U8)pCtrl->bChip,(U8)pCtrl->FeatureAddress,(U8)pCtrl->bBlklist[0], (U8)pCtrl->P4);
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip;
	U8 Add = (U8)pCtrl->FeatureAddress;
	U8 Param = (U8)pCtrl->bBlklist[0];
	U8 PreCmdOption = (U8)pCtrl->P4;
	U8 SetCmd = (U8) pCtrl->bRBMode;
	U08 bData[4] = {0,0,0,0};
	HAL_MCU_SetCe(bChan,bChip,1);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQWaitDone(bChan);

#if ((FDL_TEST_FLASH==SanDisk_BiCS5_QLC)||(FDL_TEST_FLASH==SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(FDL_TEST_FLASH==SanDisk_BiCS6_QLC)||(FDL_TEST_FLASH==SanDisk_BiCS8_QLC)||(FDL_TEST_FLASH==KHMB))
    HAL_FA_DQSSetParameter(bChan, bChip, Add, Param, PreCmdOption, SetCmd);
#else if ((FDL_TEST_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	HAL_CQSendFlashCmd(bChan,0x36);
	HAL_CQSendAddr(bChan,Add);
	HAL_CQWaitDone(bChan);
	HAL_MCUSendNByteDataByDqs(bChan,&Param,1);
	HAL_CQSendFlashCmd(bChan,0x16);
	HAL_CQWaitDone(bChan);
	HAL_CQCheckIOStatus(0,0, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
#endif
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_GetParamter(void)
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=(U8)pCtrl->bChan;
	U8 bChip=(U8)pCtrl->bChip;
	U8 Add = (U8)pCtrl->FeatureAddress;
	U16 DmaLenTemp = SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN;
	HostCmdResponse();
	FDL_Log("Get Param CH%d CE%d Add%02X", bChan,bChip,Add );
	HAL_MCU_SetCe(bChan,bChip,1);

#if ((FDL_TEST_FLASH==SanDisk_BiCS5_QLC)||(FDL_TEST_FLASH==SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(FDL_TEST_FLASH==SanDisk_BiCS6_QLC)||(FDL_TEST_FLASH==SanDisk_BiCS8_QLC)||(FDL_TEST_FLASH==KHMB))
	HAL_FA_DQSSetParameter(bChan, bChip, 0x00, 0x01, 1, 0x55);
	HAL_FA_DQSSetParameter(bChan, bChip, 0x01, 0x36, 0, 0x55);
	HAL_MCU_SendCmd(bChan,0x5F);
	//HAL_MCU_SendCmd(bChan,0x00);

	HAL_MCU_SendOneAddr(bChan,Add);
	HAL_MCU_SendCmd(bChan,0x5F);
	HAL_CQWaitDone(bChan);
    HAL_CQWaitDone(bChan);
#else if ((FDL_TEST_FLASH==Hynix_3DV6_TLC)||(NAND_FLASH == Hynix_3DV7_TLC)||(NAND_FLASH == Hynix_3DV5_TLC)||(NAND_FLASH == Hynix_3DV7_QLC)||(NAND_FLASH == Hynix_3DV8_TLC)||(NAND_FLASH == Hynix_3DV9_TLC))
	HAL_MCU_SendCmd(bChan,0x37);
	Delay(6);
	HAL_MCU_SendOneAddr(bChan,Add);
	Delay(6);
	HAL_McuSendReadPluse(bChan);
#endif

#if 1
	((U8 *)MEM_DMA_BUFF_ADDR)[0] = HAL_McuSendReadPluse(bChan);
#else
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 512;
	HAL_CQStartDMA2(bChan, MEM_DMA_BUFF_ADDR, DMA2_RD_TO_BUFF, 0, 0, 1, JOINT_NONE, MEM_DMA2_INFO_ADDR);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = DmaLenTemp;
#endif
	HAL_CQWaitDone(bChan);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}


void FA_Reset(void)
{
	U8 bChan;
	U8 bChip;
	U8 bCMD;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	bChan=pCtrl->bChan;
	bChip=pCtrl->bChip;
	bCMD=pCtrl->dMode;

    HAL_CQWaitDone(bChan);
    HAL_CQChipSelect(bChan, bChip);
    HAL_CQSendFlashCmd(bChan, bCMD);
    HAL_CQCheckIOStatus(bChan, LUN_NULL, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
    HAL_CQWaitDone(bChan);

	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}


void FA_CheckBlkData_ReadData(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr,U32 dDMA2BaseAdd, U32 dMode, U8 bLen, U16 ColAdd)
{
	U32 dRowAddr = dBlkAddr * FDL_ROW_OF_BLK + dPageAddr;

	HAL_CQChipSelect(bChan, bChip);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

	if(0==dMode)
	{
		HAL_CQSendFlashCmd(bChan, 0x3B);
	}

	HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
	HAL_CQSend6Addr(bChan, 0, dRowAddr);
	HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
	HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
	HAL_CQSendRndAddr(bChan, ColAdd, dRowAddr);

	HAL_CQStartDMA2(bChan, dDMA2BaseAdd, DMA2_RD_TO_BUFF, 0, 0, bLen, JOINT_NONE, MEM_DMA2_INFO_ADDR);

	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
	HAL_CQChipSelect(bChan, INVALID_U8);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	HAL_CQWaitDone(bChan);
}

void FA_CheckBlkData(void)
{
	U16 PageIdx, PageSum,BlkIdx,BlkSum;
	U16 * pErrBuff = ((U16 *)(MEM_DMA_BUFF_ADDR+FDL_SECT_OF_PAGE*512));
	U32 dErrTempAdd = MEM_DMA_BUFF_ADDR+FDL_SECT_OF_PAGE*512+20480;
	U16 *pErrTemp = (U16 *)dErrTempAdd;
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan=pCtrl->bChan;
	U8 bChip=pCtrl->bChip;
	U8 ReadLen =FDL_SECT_OF_PAGE;	// Read Data Len Of PageRead, should be a even number
	U16 FailPageSum = 0;
	U16 FailPageIdx = 0;
	U16 ErrThld = pCtrl->dSectionCnt;	// Errbit Judge Value
	U16 ReReadCnt = (U16)(pCtrl->dPageCnt);	// Maximum Of Reread Times
	U16 ReReadIdx;
	U16 DataCheckLen = FDL_PAGE_SIZE/2;
	U32 TimeMark;

	MemorySet((U8 *)pErrBuff, 0xCC,5120);
	HostCmdResponse();
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 0;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 2*512;


	BlkSum = 2224;
	PageIdx = pCtrl->dPlaneCnt;
	HAL_CQSetRndSeed(bChan, pCtrl->dSeedIdx, INVALID_U16);

	for(BlkIdx = 0; BlkIdx < BlkSum; ++BlkIdx)
	{
		FA_CheckBlkData_ReadData(bChan, bChip, BlkIdx, PageIdx,MEM_DMA_BUFF_ADDR,pCtrl->dMode,ReadLen,0);

		HAL_PMCPBitMapCountExtFilter(0, MEM_DMA_BUFF_ADDR,dErrTempAdd,DataCheckLen,1, 1, 0, 2);
		pErrBuff[BlkIdx] = *pErrTemp;
	}
	///
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR+FDL_SECT_OF_PAGE*512,10,JOINT_NONE);
}

void HAL_ReadPage_WithNoDataOut(T_FDL_FTCTRL *pCtrl)
{
    U8 bChan, bChip, BlkIdx, PreCMD, PlaneCnt;
    U32 dRowAdd, ReadTime, ReadTimeCnt;
    U16 PageAdd, PageAddStart, PageAddEnd, Block, BlockStart, BlockEnd;
    bChan = pCtrl->bChan;
    bChip = pCtrl->bChip;
    PageAddStart = pCtrl->dPageAddr;
    PageAddEnd = pCtrl->dPageCnt;
    ReadTimeCnt = pCtrl->dSeedIdx;
    PlaneCnt = pCtrl->dPlaneCnt;
    BlockStart = pCtrl->dSecCnt;
    BlockEnd = pCtrl->dSectionCnt;

    for (ReadTime = 0; ReadTime < ReadTimeCnt; ReadTime++)
    {
        for (Block = BlockStart; Block <= BlockEnd; Block = Block + PlaneCnt)
        {
            for (PageAdd = PageAddStart; PageAdd <= PageAddEnd; PageAdd++)
            {

                //        	  BlkIdx = 0;
                //        	  PreCMD=PageAdd%3;
                HAL_CQChipSelect(bChan, bChip);
                HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_TRUE_READY, FIO_STAT_PASS);

#if (FDL_TEST_FLASH == INTEL_N4xA)
                dRowAdd = (Block + BlkIdx) * FDL_ROW_OF_BLK + PageAdd;
                if (0 == pCtrl->dMode)
                {
                        HAL_CQSendFlashCmd(bChan, 0xDA);
                }
                else
                {
                        HAL_CQSendFlashCmd(bChan, 0xDF);
                }
                HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
                HAL_CQSend6Addr(bChan, 0, dRowAdd);

                BlkIdx++;
                while (BlkIdx < PlaneCnt)
                {
                        HAL_CQSendFlashCmd(bChan, FL_CMD_READ_MP);
                        HAL_CQWaitDone(bChan);
                        HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);

                        dRowAdd = (Block + BlkIdx) * FDL_ROW_OF_BLK + PageAdd;
                        if (0 == pCtrl->dMode)
                        {
                            HAL_CQSendFlashCmd(bChan, 0xDA);
                        }
                        else
                        {
                            HAL_CQSendFlashCmd(bChan, 0xDF);
                        }
                        
                        HAL_CQSendFlashCmd(bChan, FL_CMD_READ1);
                        HAL_CQSend5Addr(bChan, 0, dRowAdd);
                        BlkIdx++;
                }
                HAL_CQSendFlashCmd(bChan, FL_CMD_READ2);
                HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_TRUE_READY, FIO_STAT_PASS);
//                SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
//                HAL_CQChipSelect(bChan, INVALID_U8);
//                SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
                HAL_CQWaitDone(bChan);
#endif
            }
        }
    }
}

void ReadPage_WithNoDataOut(void)
{
  HostCmdResponse();
  HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);

  T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
  T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
  //HostCmdResponse();

  HAL_ReadPage_WithNoDataOut(pCtrl);
  U8 bChan=(U8)pCtrl->bChan;
  U8 bChip=(U8)pCtrl->bChip;
  U08 bData[4] = {0,0,0,0};
  U8 P1;
  U8 P2;
  U8 P3;
  U8 P4;
  U8 i;
  U8 feature[4];
  U8 * pulse = feature;
  U8 gettemp=(U8)pCtrl->P4;

  if(gettemp==1)
  {
    SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	#if ((FDL_TEST_FLASH == MIC_B16A))
	  HALDMFC_DQSSetFeature( bChan,  bChip,  0x01, 0, 0, 0, 0);
	  SG_FC(0)->rNF_CFG.bits.FLASH_MODE = 0x00;  //Set Controller to Asyn
	  HAL_CQChipSelect(bChan, bChip);
	  HAL_CQSendFlashCmd(bChan,0xEE);
	  HAL_CQSendAddr(bChan,pCtrl->FeatureAddress);
	  HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_TRUE_READY, FIO_STAT_PASS);
	  HAL_CQSendFlashCmd(bChan,FIO_EXIT_CHK_IOSTA);
	  HAL_CQWaitDone(bChan);
	  for (i = 0; i < 4; i++)
	  {
	    feature[i]= HAL_McuSendReadPluse(bChan);
	    //FDL_Log("feature =%x",feature[i]);
	  }
	  MemoryCopy((U8 *)MEM_DMA_BUFF_ADDR, pulse, 4);
	  bData[0] = 0x27;//toggle配0x20
	  bData[1] = 0;
	  HAL_NFSetFeature(bChan, bChip, 0x01, &bData[0]);
	  HAL_CQCheckIOStatus(bChan,bChip, IO_1_PASS, FIO_STAT_TRUE_READY, FIO_STAT_PASS);
	  SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;  //Set Controller to Toggle
	  HAL_CQWaitDone(bChan);
	#endif

	#if ((FDL_TEST_FLASH == TOSHIBA_8T23)||(FDL_TEST_FLASH == TOSHIBA_8T24))
		SG_FC(0)->rNF_CFG.bits.FLASH_MODE = 0x00;
		HAL_CQChipSelect(bChan, bChip);
		HAL_CQSendFlashCmd(bChan,0xB9);
		HAL_CQCheckIOStatus(bChan, bChip, IO_1_PASS, FIO_STAT_TRUE_READY, FIO_STAT_PASS);
		HAL_CQSendFlashCmd(bChan,FIO_EXIT_CHK_IOSTA);
	  	HAL_CQWaitDone(bChan);
		HAL_CQSendFlashCmd(bChan,0x7C);
		HAL_Delay(500);
		for (i = 0; i < 4; i++)
		{
		  feature[i]= HAL_McuSendReadPluse(bChan);
		}
		MemoryCopy((U8 *)MEM_DMA_BUFF_ADDR, pulse, 4);
		SG_FC(bChan)->rNF_CFG.bits.FLASH_MODE = 0x02;  //Set Controller to Toggle
	  	HAL_CQWaitDone(bChan);
	#endif
  }
}

void FA_HighTempSocketInit(void)
{
	U8 bChan = 1;
	HAL_CQWaitDone(bChan);
	HostCmdResponse();
	/* 
		P == Port
		P3_0 == F1DQ0 == SCL_1
		P3_1 == F1DQ1 == SDA_1
		P3_2 == F1DQ2 == SCL_2
		P3_3 == F1DQ3 == SDA_2
		P3_4 == F1DQ4 == Heater Control
		P3_5 == F1DQ5 == Fan Control
	*/
	U8 bPin = 0;
	
	for(bPin=0;bPin<6;bPin++)
		SG_PADCTRL->rPORT_MODE[3].bits.dVAL &= (BIT_CLR_0(bPin)); // P3_0~P3_5 Output
		//SG_PADCTRL->rPORT_MODE[3].bits.dVAL |= (BIT_SET_1(bPin));	// P3_0~P3_5 Input
	for(bPin=0;bPin<4;bPin++)	//bPin < 4
		SG_PADCTRL->rPORT_DATA[3].dAll |= (BIT_SET_1(bPin));	// P3_0~P3_3 High
	for(bPin=4;bPin<6;bPin++)	//bPin = 4
		SG_PADCTRL->rPORT_DATA[3].dAll &= (BIT_CLR_0(bPin));	// P3_4~P3_5 Low


	//rPAD_PE_CTRL
	SG_PADCTRL->rPAD_PE_CTRL[1].bits.FDQ = 0;	//disable F1DQ pull up
	//rPAD_PS_CTRL
	
	//rPAD_TSEL_CTRL0
	SG_PADCTRL->rPAD_TSEL_CTRL0.bits.F1DQ = 0;	//F1DQ input impedance 0 Z

	SG_PADCTRL->rPAD_FUNC_CTRL[1].dAll = 0xFFFF;	// F1DQ -> GPIO mode
	__nds32__dsb();
	((U8 *)MEM_DMA_BUFF_ADDR)[0] = 1;
	HAL_CQWaitDone(bChan);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_HighTempSocketReadTemp(void)
{
	U8 bChan = 1;
	HAL_CQWaitDone(bChan);
	HostCmdResponse();
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 ReadOption = (U8)pCtrl->bChip;
	U16 bTemp1 = 0;
	U16 bTemp2 = 0;

	if(ReadOption == 0)
	{
		if(IsHTControlling)
		{
			while(IsUpdating);
			bTemp1 = CurTempSensor1;
			bTemp2 = CurTempSensor2;
			__nds32__dsb();
		}
		else
		{
			FA_TMPSensorStop(1);
			FA_TMPSensorStop(2);
			delay_us(5);	// bus-free time 4.7us

			bTemp1 = FA_TMPSensorGetTemp(1);
			bTemp2 = FA_TMPSensorGetTemp(2);
		}

		FA_CUR_TEMP_ADD[0] = bTemp1;
		FA_CUR_TEMP_ADD[1] = bTemp2;
	}
	else if(ReadOption == 1)
	{
		if(IsHTControlling)
		{
			while(IsUpdating);
			bTemp1 = CurTempSensor1;
			__nds32__dsb();
		}
		else
		{
			FA_TMPSensorStop(1);
			delay_us(5);	// bus-free time 4.7us

			bTemp1 = FA_TMPSensorGetTemp(1);
		}

		FA_CUR_TEMP_ADD[0] = bTemp1;
	}
	else if(ReadOption == 2)
	{
		if(IsHTControlling)
		{
			while(IsUpdating);
			bTemp1 = CurTempSensor2;
			__nds32__dsb();
		}
		else
		{
			FA_TMPSensorStop(2);
			delay_us(5);	// bus-free time 4.7us
			bTemp1 = FA_TMPSensorGetTemp(2);
		}

		FA_CUR_TEMP_ADD[0] = bTemp1;
	}
	else
	{
		FDL_Log("Invalid Option");
	}
	
	HAL_CQWaitDone(bChan);
	HostDataTransfer(DMA1_RD_TO_HOST,FA_CUR_TEMP_ADD,1,JOINT_NONE);
}


void FA_SocketFanSwitch(void)
{
	U8 bChan = 1;
	HAL_CQWaitDone(bChan);
	HostCmdResponse();
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 FanOption =(U8)pCtrl->bChip;
	IsHTControlling = 0;
	if(FanOption == 1)
	{
		FA_OpenSocketFan();
		((U8 *)MEM_DMA_BUFF_ADDR)[0] = 1;
	}
	else if(FanOption == 0)
	{
		FA_CloseSocketFan();
		((U8 *)MEM_DMA_BUFF_ADDR)[0] = 0;
	}
	else
	{
		FDL_Log("Invalid Option");
		((U8 *)MEM_DMA_BUFF_ADDR)[0] = 2;
	}
	
	HAL_CQWaitDone(bChan);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_SocketHeaterSwitch(void)
{
	U8 bChan = 1;
	HAL_CQWaitDone(bChan);
	HostCmdResponse();
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 HeaterOption =(U8)pCtrl->bChip;
	IsHTControlling = 0;
	if(HeaterOption == 1)
	{
		IsManualHeating = 1;
		FA_OpenSocketHeater();
		((U8 *)MEM_DMA_BUFF_ADDR)[0] = 1;
	}
	else if(HeaterOption == 0)
	{
		FA_CloseSocketHeater();
		((U8 *)MEM_DMA_BUFF_ADDR)[0] = 0;
	}
	else
	{
		FDL_Log("Invalid Option");
		((U8 *)MEM_DMA_BUFF_ADDR)[0] = 2;
	}
	
	HAL_CQWaitDone(bChan);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}
void FA_SocketSetTemperature(void)

{
	U8 bChan = 1;
	HAL_CQWaitDone(bChan);
	HostCmdResponse();
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U16 TargetTemp = (U16)pCtrl->dSeedIdx;
	LinearPeriod = (U16)pCtrl->dPageCnt;
	SlowStartThreshold = (U16)pCtrl->dPlaneCnt;
	IsHTControlling = 0;
	SysCallCounter = 0;
	IsHeating = 0;
	pid.TargetTemperature = TargetTemp;
	pid.LastTemperature = 0;
	pid.PreviousTemperature = 0;

	FDL_Log("SetTarget:%u",TargetTemp);
	delay_ms(10);
	__nds32__dsb();
	IsHTControlling = 1;

	((U8 *)MEM_DMA_BUFF_ADDR)[0] = 1;
	HAL_CQWaitDone(bChan);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}


void FA_OpenSocketFan(void)
{
	U8 bPin = 5;
	SG_PADCTRL->rPORT_DATA[3].dAll |= (BIT_SET_1(bPin));	// P3_5 High
}

void FA_CloseSocketFan(void)
{
	U8 bPin = 5;
	SG_PADCTRL->rPORT_DATA[3].dAll &= (BIT_CLR_0(bPin));	// P3_5 Low
}

void FA_OpenSocketHeater(void)
{
	U8 bPin = 4;
	SG_PADCTRL->rPORT_DATA[3].dAll |= (BIT_SET_1(bPin));	// P3_4 High
}

void FA_CloseSocketHeater(void)
{
	U8 bPin = 4;
	SG_PADCTRL->rPORT_DATA[3].dAll &= (BIT_CLR_0(bPin));	// P3_4 Low
}

void FA_TMPSensorSetLevel(U8 SensorID,U8 LineType,U8 Level)
{
	// SensorID: 1 -> SCL_1 SDA_1; 2 -> SCL_2 SDA_2
	// LineType: 0:SCL; 1:SDA
	// Level: 0 = Low; 1 = High;
	U8 bPin;
	if(LineType>1)
		FDL_Log("Invalid LineType");
	else if(SensorID==1)
		bPin = LineType;
	else if(SensorID==2)
		bPin = SensorID + LineType;
	else
		FDL_Log("Invalid SensorID");
	
	if(Level == 0)
		SG_PADCTRL->rPORT_DATA[3].dAll &= (BIT_CLR_0(bPin));	// Low
	else if(Level == 1)
		SG_PADCTRL->rPORT_DATA[3].dAll |= (BIT_SET_1(bPin));	// High
	else
		FDL_Log("Invalid Level");
	
}

void FA_TMPSensorSetSDAMode(U8 SensorID,U8 bMode)
{
	// bMode: 0 -> Out; 1 -> In (From host's perspective)
	U8 bPin = 0;
	if(SensorID==1)
		bPin = 1;
	else if(SensorID==2)
		bPin = 3;
	else
		FDL_Log("Invalid SensorID");
		
	if(bMode == 0)
		SG_PADCTRL->rPORT_MODE[3].bits.dVAL &= (BIT_CLR_0(bPin));	//Output
	else if(bMode == 1)
	{
		//SG_PADCTRL->rPORT_DATA[3].dAll &= (BIT_CLR_0(bPin));		//Low
		SG_PADCTRL->rPORT_MODE[3].bits.dVAL |= (BIT_SET_1(bPin));	//Input
		SG_PADCTRL->rPAD_PE_CTRL[1].bits.FDQ = 0;	//disable F1DQ pull up
		SG_PADCTRL->rPAD_TSEL_CTRL0.bits.F1DQ = 0;	//F1DQ input impedance 0 Z
	}
	else
		FDL_Log("Invalid Mode");
	
}

void FA_TMPSensorStart(U8 SensorID)
{
	FA_TMPSensorSetSDAMode(SensorID, 0); // SDA Out
	FA_TMPSensorSetLevel(SensorID, 1, 1);	// Set SDA High
	delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 0, 1);	// Set SCL High
	delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 1, 0);	// Set SDA Low
	delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
}

void FA_TMPSensorStop(U8 SensorID)
{
	FA_TMPSensorSetSDAMode(SensorID, 0); // SDA Out
	FA_TMPSensorSetLevel(SensorID, 1, 0);	// Set SDA Low
	delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 0, 1);	// Set SCL High
	delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 1, 1);	// Set SDA High
}

U8 FA_TMPSensorReadSDA(U8 SensorID)
{
	U8 bRet = 0;
	if(SensorID==1)
		bRet = (U8)((SG_PADCTRL->rPORT_DATA[3].bits.dVAL & 0x02)>>1);
	else if(SensorID==2)
		bRet = (U8)((SG_PADCTRL->rPORT_DATA[3].bits.dVAL & 0x08)>>3);
	else
		FDL_Log("Invalid SensorID");
	__nds32__dsb();
	return bRet;
}


U8 FA_TMPSensorWriteByte(U8 SensorID, U8 bData)
{
	U8 bBitIdx;
	U8 Nck = 1;
	U8 timer = 0;
	
	FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
	FA_TMPSensorSetSDAMode(SensorID, 0);	// SDA Out
	
	for(bBitIdx = 0;bBitIdx < 8;bBitIdx++)
	{
		delay_us(5);
		if((bData<<bBitIdx)&0x80)
			FA_TMPSensorSetLevel(SensorID, 1, 1);	// Set SDA High
		else
			FA_TMPSensorSetLevel(SensorID, 1, 0);	// Set SDA Low
		delay_us(1);
		FA_TMPSensorSetLevel(SensorID, 0, 1);	// Set SCL High
		delay_us(4);	//TMP114 SCL High period min = 4us
		FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
	}
	
	//wait ACK
	FA_TMPSensorSetLevel(SensorID, 1, 1);	// Set SDA High(SDA open drain output)
	//delay_us(1);
	FA_TMPSensorSetSDAMode(SensorID, 1); // SDA In
	delay_us(5);							//TMP114 SCL Low period min = 4.7us
	FA_TMPSensorSetLevel(SensorID, 0, 1);	// Set SCL High
	delay_us(5);	//TMP114 SCL High period min = 4us
	Nck = FA_TMPSensorReadSDA(SensorID);
	//if(Nck == 0)
		//return Nck;	//0: Success
	//else
		//FA_TMPSensorStop(SensorID);
	return Nck;	//1: Fail
}

U8 FA_TMPSensorReadByte(U8 SensorID, U8 NAck)
{
	U8 bRet = 0x00;
	U8 bBitIdx = 0;
	
	FA_TMPSensorSetSDAMode(SensorID, 1); // SDA In
	for(bBitIdx = 0;bBitIdx < 8;bBitIdx++)
	{
		bRet = bRet << 1;
		delay_us(5);
		FA_TMPSensorSetLevel(SensorID, 0, 1);	// Set SCL High
		delay_us(5);
		bRet += FA_TMPSensorReadSDA(SensorID);
		FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
	}
	// Send Ack(0) or NAck(1)
	FA_TMPSensorSetSDAMode(SensorID, 0);	// SDA Out
	delay_us(5);
	if(NAck)
		FA_TMPSensorSetLevel(SensorID, 1, 1);	// Set SDA High
	else
		FA_TMPSensorSetLevel(SensorID, 1, 0);	// Set SDA Low
	delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 0, 1);	// Set SCL High
	delay_us(5);
	FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low

	return bRet;
}

U16 FA_TMPSensorGetTemp(U8 SensorID)
{
	U8 bAck;
	U16 bTemp=0x0000;
	U8 bData;
	U8 TargetAddressWrite =  0x90;	//TMP114A Target Address 1001000 Write 0
	U8 TargetAddressRead = 0x91;	//TMP114A Target Address 1001000 Read 1
	U8 TempResReg = 0x00;		// Auto-Increment=0,fixed=000 TempResRegAddr=0000
	U8 ConfigReg = 0x03;
	//U8 failcnt = 0;
	//Start communication, set target address, Auto-Increment and register Pointer
	FA_TMPSensorStart(SensorID); 
	((U8 *)MEM_DMA_BUFF_ADDR)[3+SensorID] = FA_TMPSensorWriteByte(SensorID,TargetAddressWrite);
	//delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
	((U8 *)MEM_DMA_BUFF_ADDR)[5+SensorID] = FA_TMPSensorWriteByte(SensorID,TempResReg);
	//FA_TMPSensorWriteByte(SensorID,ConfigReg);
	//delay_us(1);
	FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
	//Restart, read from temperature result register
	FA_TMPSensorStart(SensorID);
	FA_TMPSensorWriteByte(SensorID,TargetAddressRead);
	FA_TMPSensorSetLevel(SensorID, 0, 0);	// Set SCL Low
	U8 MSB = FA_TMPSensorReadByte(SensorID,0);
	U8 LSB = FA_TMPSensorReadByte(SensorID,1);
	FA_TMPSensorStop(SensorID);
	bTemp += MSB;
	bTemp = (bTemp<<8) + LSB;
	
	return bTemp;
}

void FA_HeaterControl(void)
{
    U32 __sda_addr=__get_sda_base();
	if(IsHTControlling)
	{
		if(SysCallCounter>2)
		{
			if(IsHeaterOn)
			{
				FA_OpenSocketHeater();
			}
			else
			{
				FA_CloseSocketHeater();
			}
			PIDUpdateCurrentTemperature();
			if(IsFanOn && IsHeating==0 && IsHigherTemperature(pid.TargetTemperature,pid.CurrentTemperature))	
			{
				FA_CloseSocketFan();
				IsFanOn = 0;
			}
			else if(IsHeating == 1 && IsHigherTemperature(pid.CurrentTemperature,pid.TargetTemperature))
			{
				IsHeating = 0;
				HTCtrlCnt = 0;
			}
			IsHeaterOn = FA_HeaterOperationCalculate(IsHeating);
			HTCtrlCnt++;
		}
		else
		{
			PIDUpdateCurrentTemperature();
			pid.StartTemperature = pid.CurrentTemperature;
			if(SysCallCounter == 0)
			{
				HTCtrlCnt = 1;
				if(IsHigherTemperature(pid.CurrentTemperature,pid.TargetTemperature))
				{
					FA_OpenSocketFan();
					IsFanOn = 1;
					IsHeating = 0;
				}
				else if(IsHigherTemperature(pid.TargetTemperature,pid.CurrentTemperature) && pid.TargetTemperature<=SlowStartThreshold)
				{
					FA_OpenSocketFan();
					IsFanOn = 1;
					IsHeating = 1;
				}
				else
					IsHeating = 1;
				//pid.CumulativeError = 0;
			}
			IsHeaterOn = FA_HeaterOperationCalculate(IsHeating);
			SysCallCounter++;
			HTCtrlCnt++;
		}
		FDL_Log("Cur:%u",pid.CurrentTemperature);
		//FDL_Log("Heater:%u",IsHeaterOn);
		__nds32__dsb();
	}
	else
	{
		if(IsManualHeating!=1)
			FA_CloseSocketHeater();
	}
	__restore_sda_base(__sda_addr);
}

U8 FA_HeaterOperationCalculate(U8 Option)
{
	//FDL_Log("CalStart");
	int res;
	int TargetTemp;
	int TargetTemperature;
	int StartTemperature;
	int CurrentTemperature;
	int LastTemperature;
	int PreviousTemperature;
	
	if(pid.TargetTemperature & 0x8000)
		TargetTemperature = -((~pid.TargetTemperature + 1) & 0x7FFF);
	else
		TargetTemperature = pid.TargetTemperature;
	
	if(pid.StartTemperature & 0x8000)
		StartTemperature = -((~pid.StartTemperature + 1) & 0x7FFF);
	else
		StartTemperature = pid.StartTemperature;
	
	if(pid.CurrentTemperature & 0x8000)
		CurrentTemperature = -((~pid.CurrentTemperature + 1) & 0x7FFF);
	else
		CurrentTemperature = pid.CurrentTemperature;
	
	if(pid.LastTemperature & 0x8000)
		LastTemperature = -((~pid.LastTemperature + 1) & 0x7FFF);
	else
		LastTemperature = pid.LastTemperature;
	
	if(pid.PreviousTemperature & 0x8000)
		PreviousTemperature = -((~pid.PreviousTemperature + 1) & 0x7FFF);
	else
		PreviousTemperature = pid.PreviousTemperature;
	
	//FDL_Log("Option%d",Option);
	if(Option == 1)
	{
		if(HTCtrlCnt<LinearPeriod)
			TargetTemp = (StartTemperature + ((int)HTCtrlCnt * (TargetTemperature - StartTemperature))/LinearPeriod);
		else
		{
			TargetTemp = TargetTemperature;
			HTCtrlCnt = LinearPeriod;
		}
		//FDL_Log("HTCtrlCnt:%u",HTCtrlCnt);
		//FDL_Log("SmallTarget:%d",TargetTemp);
		//FDL_Log("Target:%d",pid.TargetTemperature);
		res = (TargetTemp - 2*CurrentTemperature - (CurrentTemperature>>1) + 2*LastTemperature - (PreviousTemperature>>1));
	}
	else
	{
		res = (TargetTemperature - 2*CurrentTemperature - (CurrentTemperature>>1) + 2*LastTemperature - (PreviousTemperature>>1));
		//HTCtrlCnt = 0;
	}
	/*
	else if(Option == 2)
	{
		pid.CumulativeError += (TargetTemperature - CurrentTemperature);
		res = pid.CumulativeError;
	}
	*/
	//FDL_Log("res:%d",res);
	__nds32__dsb();
	if(res>=0)
	{
		return 1;
	}
	if(res<-256 && IsFanOn == 0)
	{
		FA_OpenSocketFan();
		IsFanOn = 1;
	}
	return 0;
}

void PIDUpdateCurrentTemperature(void)
{
	IsUpdating = 1;
	
	//FDL_Log("PIDUpdateCurrentTemperature");
	U16 temp1 = FA_TMPSensorGetTemp(1);
	U16 temp2 = FA_TMPSensorGetTemp(2);

	CurTempSensor1 = temp1;
	CurTempSensor2 = temp2;

	pid.PreviousTemperature = pid.LastTemperature;	//a = (Vcur - Vpre)/dt
	pid.LastTemperature = pid.CurrentTemperature;


	if((temp1 & 0x8000) && (temp2 & 0x8000))
		pid.CurrentTemperature = (U16)((temp1+temp2)>>1);
	else if(((temp1 & 0x8000) ^ (temp2 & 0x8000)) && (((U16)(temp1+temp2))&0x8000))
		pid.CurrentTemperature = ~(((U16)~(temp1+temp2)+1)>>1)+1;
	else
		pid.CurrentTemperature = ((U16)(temp1+temp2))>>1;

	//FDL_Log("temp1:%u",temp1);
	//FDL_Log("temp2:%u",temp2);
	//FDL_Log("curtemp:%u",pid.CurrentTemperature);
	IsUpdating = 0;
}
U8 IsHigherTemperature(U16 Temp1,U16 Temp2)
{
	if((Temp1 & 0x8000) == (Temp2 & 0x8000))
	{
		if(Temp1-Temp2>0)
			return 1;
		else
			return 0;
	}
	else
	{
		if((Temp1 & 0x8000) > (Temp2 & 0x8000))	//Temp2 negative
			return 1;
		else
			return 0;
	}
}



void FA_CE_Reduction(void)
{
    T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
    U8 bChan, bChip;

    bChan = (U8)pCtrl->bChan;
    bChip = (U8)pCtrl->bChip;
    U08 bCERChip = (U8)pCtrl->FeatureAddress;

//如果传行地址的话用这个
//        U32 dRow;
//    dRow = pCtrl.FeatureAddress;
//    U08 bCERChip = (dRow & VALID_ROW_BIT_MASK_) / FDL_ROW_GAP_OF_2LUN;
    HAL_CQWaitDone(bChan);

	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = GPIO_MODE;/*IO线设置为GPIO模式*/
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(1); // ALE初始状态拉低
	SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(1); // ALE 设置为输出模式

	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(1);

	SG_FC(bChan)->rMCU_CFG_CE.bits.dVAL = (1 << bChip);
	HAL_MCU_SendCmd(bChan,0xE1);
	HAL_MCU_SendCmd(bChan,bCERChip);
    FDL_Log("CERChip %d\n", bCERChip);

	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = FUCN_MODE;//IO线设置为Function模式

    HAL_CQCheckIOStatus(bChan, LUN_NULL, IO_1_PASS, FIO_STAT_READY, FIO_STAT_NULL);
    HAL_CQWaitDone(bChan);
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}


void HAL_MCUSendCER_CE(U8 bChan,U8 bChip, U32 dRow)
{
//	U08 bCERChip = (dRow & VALID_ROW_BIT_MASK_) / ROW_GAP_OF_LUN;
    U08 bCERChip = (U08)dRow;

    //FDL_Log("CERChip:%d",dRow);

    U8 temp;
	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = GPIO_MODE;/*IO线设置为GPIO模式*/
	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(1); // ALE初始状态拉低
	SG_PADCTRL->rPORT_MODE[bChan*3+2].bits.dVAL &= BIT_CLR_0(1); // ALE 设置为输出模式

	SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(1);

	SG_FC(bChan)->rMCU_CFG_CE.bits.dVAL = (1 << bChip);
	HAL_MCU_SendCmd(bChan,0xE1);
    
    SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL &= BIT_CLR_0(1); 
    __nds32__dsb();
//    while (temp<200)
//    {
//        temp++;
//    }

    SG_PADCTRL->rPORT_DATA[bChan*3+2].bits.dVAL |= BIT_SET_1(1); 

	HAL_MCU_SendCmd(bChan,bCERChip);

	SG_PADCTRL->rPAD_FUNC_CTRL[bChan].bits.FALE = FUCN_MODE;
}

void FA_OfflineTaskInit()
{
	
	
	U8 TaskType = 0;
	U8 i,j,k;
	U32 d;

	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	
	Offline_Chan = pCtrl->bChan;
	Offline_Chip = pCtrl->bChip;
	Offline_OperationMode = pCtrl->dMode;
	Offline_PlaneCnt = pCtrl->dPlaneCnt;
	Offline_OperationStage = pCtrl->dSecCnt;
	Offline_RBMode = pCtrl->bRBMode;
	Offline_RSRecord = pCtrl->dDelayCtrl;
	Offline_RSBlkCnt = pCtrl->dWareIdx;
	TaskType = pCtrl->P4;
	
	U16 *OperationCntList = (U16 *)(&pCtrl->bBlklist[0]);
	
	switch(TaskType)
	{
		case 1:	//PE offline Task
			//Init PE Blocks List;
			k=0;
			for(i = 0;i<Offline_OperationStage*Offline_PlaneCnt*2;i+=2){
				Offline_BlockList[k] = (((U16)FA_Offline_Block[i])<<8) + ((U16)FA_Offline_Block[i+1]);
				//FDL_Log("Offline_BlockList%u:%u",k,Offline_BlockList[k]);
				k++;
				//FDL_Log("Offline_BlockList:%u",FA_Offline_Block[i]);
				//FDL_Log("Offline_BlockList:%u",FA_Offline_Block[i+1]);
			}
			//Init PE times List;
			for(i=0;i<Offline_OperationStage;i++){
				Offline_OperationCntList[i] = OperationCntList[i];
				//FDL_Log("Offline_OperationCntList[%u]:%u",i,Offline_OperationCntList[i]);
			}
			//Init Offline Program Type, 0:Normal WriteBlock; 1:Copyback WriteBlock; 2:Erase
			Offline_Program_Type = pCtrl->dSectionCnt;
			//FDL_Log("Offline_Program_Type:%u",Offline_Program_Type);
			Offline_PE_Stage_counter = 1;
			Offline_PE_counter = OperationCntList[Offline_OperationStage-1];
			Offline_Program_StartSeed = Offline_PE_counter%256;
			flag_PE_offline = 1;
			//FDL_Log("Offline PE Enabled");
			//FDL_Log("Offline_PE_counter:%u",Offline_PE_counter);
			
			break;
		case 2:	//Read Disturb Task



			flag_ReadDisturb_offline = 1;
			break;
		case 3:
			/*
			PRINT_TXT("delay");
			for(d = 0;d<(pCtrl->dPlaneCnt);d++)
				delay_ms(1000);
			PRINT_TXT("delay end");
			*/
			break;
	}
	if(Offline_RSRecord)
	{
		Offline_RSRecIndex = 0;
		Offline_RSBlkIndex = 0;
		for(i = 0;i<Offline_RSBlkCnt;i++)
			Offline_RSRecBlkList[i] = pCtrl->dWareMode[i];
	}
	
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

void FA_PE_offline()
{
	//Perform 1 Round operation in Triangular PE
	//Add Operate Block of this round
	U16 *BlockList;
	U8 i,j,k;
	U8 BlkIdx = 0;
	U8 DeckSelect = 0;
	//HAL_CQWaitDone(Offline_Chan);
	//FDL_Log("FA_PE_offline");
	if(Offline_OperationStage-Offline_PE_Stage_counter-1>=0){
		if(Offline_PE_counter==Offline_OperationCntList[Offline_OperationStage-Offline_PE_Stage_counter-1]){
			Offline_PE_Stage_counter++;
		}
	}
	for(j=0;j<Offline_PE_Stage_counter;j++){
		//FDL_Log("j<Offline_PE_Stage_counter");
		for(i=0;i<Offline_PlaneCnt;i++){
			BlockList[i] = Offline_BlockList[(((Offline_OperationStage-j-1)*Offline_PlaneCnt)+i)];
			//FDL_Log("BlockList%u:%u",i,BlockList[i]);
		}
	
		
		#if((NAND_FLASH == INTEL_N38A)||(NAND_FLASH == INTEL_N38B)||(NAND_FLASH == INTEL_N4xA))
			U16 dPageaddr;
			//EraseBlock
			for(DeckSelect = 2;DeckSelect>=0;DeckSelect--){
				if(0==Offline_OperationMode) //OprMod 0-SLC 1-TLC
				{
					dPageaddr = DeckSelect * FDL_SLC_PAGE_OF_BLK / 3;
				}
				else if(1==Offline_OperationMode)
				{
					dPageaddr = DeckSelect * FDL_PAGE_OF_BLK / 3;
				}
				BlkIdx = 0;
				//HAL_CQWaitDone(Offline_Chan);
				HAL_CQChipSelect(Offline_Chan, Offline_Chip);
				HAL_CQCheckIOStatus(Offline_Chan, Offline_Chip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
				HAL_CQWaitIdleSpace(Offline_Chan, 0x20);
				MultiPlane_Erase(Offline_Chan, Offline_Chip, BlockList[BlkIdx], dPageaddr, Offline_OperationMode);
				++BlkIdx;
				while (Offline_PlaneCnt>BlkIdx)
				{
					MultiPlane_Erase(Offline_Chan, Offline_Chip, BlockList[BlkIdx], dPageaddr, 0xFF);
					++BlkIdx;
				}
				HAL_CQSendFlashCmd(Offline_Chan, FL_CMD_BLOCK_ERASE2);
				
				//Wait busy time; to-do: record RB time
				/*
				__enable_performance();
				while(__get_pfm_count()<1000)
				{	
					if(0==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
						break;
				}
				*/
				//if(Offline_RSRecord)
					//RS_CheckStatusWithSaveOffline();
				//else
					//HAL_CQCheckIOStatus(Offline_Chan, Offline_Chip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS); //Check Operation Done
				//FDL_Log("EraseBlkOver");
				//to-do: check status
				//RS_CheckStatusWithSave_2(Offline_Chan,Offline_Chip,)

				//WriteBlock
				if(Offline_Program_Type == 0)
				{
					FA_Offline_WriteBlock_ver1(DeckSelect,BlockList);
				}
				else if(Offline_Program_Type == 1){;}
				else if(Offline_Program_Type == 2){;}
				else{;}

				
			}
		#else
			//EraseBlock
	        //HAL_CQWaitDone(Offline_Chan);
			HAL_CQChipSelect(Offline_Chan, Offline_Chip);
			HAL_CQCheckIOStatus(Offline_Chan, Offline_Chip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS);
			HAL_CQWaitIdleSpace(Offline_Chan, 0x20);
			for(i=0;i<Offline_PE_Stage_counter;i++){
				MultiPlane_Erase(Offline_Chan, Offline_Chip, BlockList[BlkIdx], 0, Offline_OperationMode);
				++BlkIdx;
				while (Offline_PlaneCnt>BlkIdx)
				{
					MultiPlane_Erase(Offline_Chan, Offline_Chip, BlockList[BlkIdx], 0, 0xFF);
					++BlkIdx;
				}
				HAL_CQSendFlashCmd(Offline_Chan, FL_CMD_BLOCK_ERASE2);
			}
			

			//Wait erase busy time; to-do: record RB time
			/*
			__enable_performance();
			while(__get_pfm_count()<1000)
			{
				if(0==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
					break;
			}
			*/
			if(Offline_RSRecord)
				;//RS_CheckStatusWithSaveOffline();
			else
				HAL_CQCheckIOStatus(Offline_Chan, Offline_Chip, IO_1_PASS, FIO_STAT_READY, FIO_STAT_PASS); //Check Operation Done
				//Check Erase Done by RB pin
			//FDL_Log("EraseBlkOver");
			
		
			//WriteBlock
			if(Offline_Program_Type == 0)
			{
				FA_Offline_WriteBlock_ver1(DeckSelect,BlockList);
			}
			else if(Offline_Program_Type == 1){;}
			else if(Offline_Program_Type == 2){;}
			else{;}

		#endif
		
	}
		

	Offline_PE_counter--;
	if(Offline_PE_counter<=0){
		flag_PE_offline = 0;
		FDL_Log("Offline PE End");
	}
	Offline_Program_StartSeed = Offline_PE_counter%256;
	//FDL_Log("Offline_PE_counter:%u",Offline_PE_counter);
}

void FA_Offline_WriteBlock_ver1(U8 DeckSelect, U16* BlockList)
{
	U8 bChan = Offline_Chan;
	U8 bChip = Offline_Chip;
	U32 i,PageIndex,PageAddr,WLAddr;
	U16 PgmCnt,PgmBegin,PgmEnd;
	U8 ProgramSelect = DeckSelect;
	U16 PGMOrder;
	U8 BuffTemp[10];
	U8 Pattern;
	U8 FillData;
	T_FDL_FTCTRL *pCtrl;
	for(i=0;i<Offline_PlaneCnt;i++)
		((U16*) pCtrl->bBlklist)[i] = BlockList[i];
	pCtrl->bChan = Offline_Chan;
	pCtrl->bChip = Offline_Chip;
	pCtrl->dMode = Offline_OperationMode;
	pCtrl->dSeedIdx = Offline_Program_StartSeed;
	pCtrl->dPlaneCnt = Offline_PlaneCnt;
	pCtrl->dSeedBuffEn = FL_CMD_PAGE_PROG1;//PreCMD
	pCtrl->P4 = FL_CMD_PROG_CONFIRM2;	//CfmCMD
	pCtrl->bRBMode = Offline_RBMode;
	//FDL_Log("WriteBlkStart");


	Pattern = 255;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 1024;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_ALG_SEL=0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
	MemorySet((U8 * )(MEM_DMA_BUFF_ADDR + FDL_SECT_OF_PAGE*SECTOR_SIZE),  0xFF, FDL_SECT_OF_PAGE*SECTOR_SIZE);

		#if ((NAND_FLASH == YMTC_X2_6070)||(NAND_FLASH == YMTC_X3_6070)||(NAND_FLASH == YMTC_X4_6080))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i,i);
			}
		}
		else if (1==(pCtrl->dMode&0x01))
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));//PgmInfo[0]:WL Add,[1]coarse/fine
				GetStartPage(PgmInfo[0], &(WlInfo[0]));//WLInfo[0]:start page,[1]:WL Type
				for(i = WlInfo[0]; i< (WlInfo[0]+WlInfo[1]); ++i)
				{
					//FDL_Log("%d: W%d_P%d_S%d_T%d", PGMOrder, PgmInfo[0], i, PgmInfo[1], WlInfo[1]);
					PageIndex = i;
					if((WlInfo[1]==1)||(WlInfo[1]==3)||((WlInfo[1]==4)&&(PgmInfo[1]==1))) PageIndex += 0x10000; ///Fine In QLC WL
					//if(((WlInfo[1]==1)&&(PgmInfo[1]==0))) PageIndex += 0x10000; ///fine In QLC WL
					MultiPlane_WritePage(pCtrl, PageIndex, PGMOrder);
				}

			}
		}
		else // X4-6080 TLC 模式
		{
			PgmCnt=FDL_TLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i, i);
			}
		}
	#elif ((NAND_FLASH == YMTC_X2_9060)||(NAND_FLASH == INTEL_N38A)||(NAND_FLASH == YMTC_X3_9060)||(NAND_FLASH == YMTC_X3_9070)||(NAND_FLASH == YMTC_X4_9060))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
		}
		for(i = 0; i < PgmCnt; i++)
		{
			MultiPlane_WritePage(pCtrl, i, i);
		}
	#elif ((NAND_FLASH == MIC_B47R))
		{
			if (0==pCtrl->dMode)
			{
				PgmCnt=FDL_SLC_PAGE_OF_BLK;
			}
			else
			{
				PgmCnt=FDL_PAGE_OF_BLK;
			}
			for(i = 0; i < PgmCnt; i++)
			{
				if(Pattern<8)
				{

					if(i>3 && i<1048)
						FillData = ((Pattern>>((i-4)%3))&(0x1))?(0xFF):(0x00);
					else if(i>1063 && i<2108)
						FillData = ((Pattern>>((i-1064)%3))&(0x1))?(0xFF):(0x00);
					else
						FillData = 0xFF;
					SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
					MemorySet((U8 * )(MEM_DMA_BUFF_ADDR), FillData , FDL_SECT_OF_PAGE*SECTOR_SIZE);
				}
				else if(Pattern == 8)	//LP random data, CP and MP 0xFF
				{
					if((i>3) && (i<1048) && ((i-4)%3 == 0))	//TLC WL LP
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
					else if(i>1063 && i<2108 && ((i-1064)%3 == 0)) //TLC WL LP
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=0;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0x00, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
					else
					{
						SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
						MemorySet((U8 * )(MEM_DMA_BUFF_ADDR),  0xFF, FDL_SECT_OF_PAGE*SECTOR_SIZE);
					}
				}
				MultiPlane_WritePage(pCtrl, i, i);
			}
		}
	#elif ((NAND_FLASH == INTEL_N38B)||(NAND_FLASH == INTEL_N4xA))
		U16 PgmInfo[2], WlInfo[3];
		if (0==(pCtrl->dMode &1))
		{
			U32 PageOfs = ProgramSelect*FDL_SLC_PAGE_OF_BLK/3;
			PgmCnt=FDL_SLC_PAGE_OF_BLK/3;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i + PageOfs,i + PageOfs);
			}
		}
		else
		{
			U32 PageOfs = ProgramSelect*FDL_PAGE_OF_BLK/3;
			U8 ProgAlgSel = ((pCtrl->dMode>>7)&1); //PreRead Switch
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				U16 ProgPageTab[4];
				U8 ProgPageCnt = 0;
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				if(1 == PgmInfo[1]) //1st pass
				{
					if( 0 == WlInfo[2] )	//eTLC
					{
						ProgPageTab[0] = WlInfo[0] + PageOfs;
						ProgPageCnt = 1;
					}
					else					//QLC
					{
						ProgPageTab[0] = WlInfo[0] + PageOfs;
						ProgPageTab[1] = WlInfo[0] + 1 + PageOfs;
						ProgPageCnt = 2;
					}
				}
				else 				//2nd pass
				{
					if( 0 == WlInfo[2] )	//eTLC
					{
						ProgPageTab[0] = WlInfo[1] + PageOfs;
						ProgPageTab[1] = WlInfo[1] + 1 + PageOfs;
						ProgPageTab[2] = WlInfo[0] + PageOfs;
						ProgPageCnt = 2 + ProgAlgSel;
					}
					else					//QLC
					{
						ProgPageTab[0] = WlInfo[1] + PageOfs;
						ProgPageTab[1] = WlInfo[1] + 1 + PageOfs;
						ProgPageTab[2] = WlInfo[0] + PageOfs;
						ProgPageTab[3] = WlInfo[0] + 1 + PageOfs;
						ProgPageCnt = 2 + 2*ProgAlgSel;
					}
				}
				for(i = 0; i< ProgPageCnt; ++i)
				{
					PageIndex = ProgPageTab[i];
					MultiPlane_WritePage(pCtrl, PageIndex, PageIndex);
				}
			}
		}
	#elif ((NAND_FLASH == MIC_N48R))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage(pCtrl, i,i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				for(i = WlInfo[0]; i< (WlInfo[0]+2+2*WlInfo[1]); ++i)
				{
					PageIndex = i;
					if((WlInfo[1]==0)||((WlInfo[1]==1)&&(PgmInfo[1]==1))) PageIndex += 0x10000; ///Coarse In QLC WL
					if(i==(WlInfo[0]+1+2*WlInfo[1])) PageIndex += 0x20000; ///Last Page In WL
					MultiPlane_WritePage(pCtrl, PageIndex, PGMOrder);
				}
			}
		}
	#elif ((NAND_FLASH == MIC_N58R))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				GetStartPage(PgmInfo[0], &(WlInfo[0]));
				for(PageIndex = 0;PageIndex<WlInfo[1];PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if((WlInfo[1]==2)||((WlInfo[1]==4)&&(PgmInfo[1]==1))) PageAddr+=0x10000;	//Coarse In QLC WL
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}

		}
	#elif ((NAND_FLASH == Hynix_3DV6_TLC) || (NAND_FLASH == Hynix_3DV7_TLC)|| (NAND_FLASH == Hynix_3DV5_TLC)|| (NAND_FLASH == Hynix_3DV8_TLC)|| (NAND_FLASH == Hynix_3DV9_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(i = 0; i < PgmCnt; i++)
			{
				for(PageIndex = 0;PageIndex<3;++PageIndex)
				{
					PageAddr = i*3+PageIndex;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
		}
	#elif ((NAND_FLASH == Hynix_3DV7_QLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				for(PageIndex = 0;PageIndex<4;PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if(PgmInfo[1]==1) PageAddr+=0x10000;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}

		}
	#elif ((NAND_FLASH == SanDisk_BiCS5_QLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PGM_ORDER_COUNT;
			for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
			{
				GetWLAndStats(PGMOrder, &(PgmInfo[0]));
				for(PageIndex = 0;PageIndex<4;PageIndex++)
				{
					PageAddr = PgmInfo[0]*4+PageIndex;
					if(PgmInfo[1]==1) PageAddr+=0x10000;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
				}
			}
		}
#elif ((NAND_FLASH == SanDisk_BiCS6_QLC)||(FDL_TEST_FLASH==SanDisk_BiCS8_QLC))
			U16 PgmInfo[2];
			if (0==pCtrl->dMode)
			{
				PgmCnt=FDL_SLC_PAGE_OF_BLK;
				for(i = 0; i < PgmCnt; i++)
				{
					MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
				}
			}
			else
			{
				PgmCnt=FDL_PGM_ORDER_COUNT;
				for(PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
				{
					GetWLAndStats(PGMOrder, &(PgmInfo[0]));
					for(PageIndex = 0;PageIndex<4;PageIndex++)
					{
						PageAddr = PgmInfo[0]*4+PageIndex;
						if(PgmInfo[1]==1) PageAddr+=0x10000;
						MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, PGMOrder);
					}
				}
			}

    #elif ((NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC))
		U16 PgmInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
            PgmCnt=FDL_PGM_ORDER_COUNT;
			for(i = 0; i < PgmCnt; i++)
			{
				for(PageIndex = 0;PageIndex<3;++PageIndex)
				{
					PageAddr = i*3+PageIndex;
					MultiPlane_WritePage_PrefixCMD(pCtrl, PageAddr, i);
				}
			}
		}
	#elif ((NAND_FLASH == SAMSUNG_3DV6_TLC)||(NAND_FLASH == SAMSUNG_3DV6P))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
    #elif ((NAND_FLASH == SAMSUNG_3DV4_QLC))
       U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    //FDL_Log("page %d\n", i);
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}
    #elif ((NAND_FLASH == SAMSUNG_3DV5_QLC))
		U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    //FDL_Log("page %d\n", i);
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}

    #elif ((NAND_FLASH == SAMSUNG_3DV7_QLC))
	    U16 PgmInfo[2], WlInfo[2];
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{

            PgmCnt = FDL_PGM_ORDER_COUNT;
            for (PGMOrder = 0; PGMOrder < PgmCnt; PGMOrder++)
            {
                
                GetWLAndStats(PGMOrder, &(PgmInfo[0]));//get WL Add
                GetStartPage(PgmInfo[0], &(WlInfo[0]));//get page Add
                //FDL_Log("Order %d, WL %d, First page %d, program index %d\n", PGMOrder, PgmInfo[0], WlInfo[0], PgmInfo[1]);
                for (i = WlInfo[0]; i < WlInfo[0]+WlInfo[1]; i++)
                {
                    PageIndex = i;
                    if (i == WlInfo[0]+WlInfo[1]-1) PageIndex += 0x10000;//last page in WL
                    MultiPlane_WritePage_SAMSUNG(pCtrl, PGMOrder, PageIndex, i-WlInfo[0]);
                }
            }
            //FDL_Log("End Program Blk\n");

		}
	#elif ((NAND_FLASH == SAMSUNG_3DV8_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
	#elif ((NAND_FLASH == SAMSUNG_3DV7_TLC))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_SAMSUNG(pCtrl, i, i, 0);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				GetWLAndPage(i,&WLAddr,&PageIndex);
				MultiPlane_WritePage_SAMSUNG(pCtrl, WLAddr, i, PageIndex);
			}
		}
	#elif ((NAND_FLASH == MIC_B58R)||(NAND_FLASH == MIC_B57T))
		if (0==pCtrl->dMode)
		{
			PgmCnt=FDL_SLC_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
		else
		{
			PgmCnt=FDL_PAGE_OF_BLK;
			for(i = 0; i < PgmCnt; i++)
			{
				MultiPlane_WritePage_PrefixCMD(pCtrl, i, i);
			}
		}
	#endif
	//FDL_Log("WriteBlkOver");
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS=1;
	//HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}
void FA_OfflineTaskInterrupt()
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 TaskType = pCtrl->P4;
	switch(TaskType)
	{
		case 0:	//Pause Offline Task
			if(flag_PE_offline)
				flag_PE_offline = 0;
			if(flag_ReadDisturb_offline)
				flag_ReadDisturb_offline = 0;
			FDL_Log("Offline PE Suspended");
			FDL_Log("Offline_PE_counter:%u",Offline_PE_counter);
			FA_Offline_Status[0] = Offline_PE_counter;
			
			HostDataTransfer(DMA1_RD_TO_HOST,FA_Offline_Status,1,JOINT_NONE);
			break;
		case 1:	//Get Status of Offline PE Task;
			//get PE count
			FA_Offline_Status[0] = Offline_PE_counter;
			//RB time
			
			//RS buffer
			HostDataTransfer(DMA1_RD_TO_HOST,FA_Offline_Status,1,JOINT_NONE);
			break;
		case 2:	//Resume PE offline
			FDL_Log("Offline PE Resumed");
			FDL_Log("Offline_PE_counter:%u",Offline_PE_counter);
			flag_PE_offline = 1;
			break;
		case 3 :
			SG_FC(0)->rCHK_ST_RB_CNT = 200000*30;	//1us超时	//200000 = 200Mhz下，1ms超时
			__nds32__dsb();
			SG_FC(0)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_INT_EN = 1;
			__nds32__dsb();
			SG_FC(0)->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL = 1;
			__nds32__dsb();
			break;
	}
	
}


U8 FA_GPIOCTRL_FUNC(U8 GPIO_NUM, U8 FUNCCTRL)
{
	if(GPIO_NUM == 2)//GPIO10 can only work in GPIO mode.
	{
		return 1;
	}
	if(GPIO_NUM > 2)//Invalid GPIO number
	{
		return 0xFF;
	}
	if(1 == (FUNCCTRL >> 1))
	{
		if(GPIO_NUM == 0)
		{SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO8 = (FUNCCTRL&1);}
		else if(GPIO_NUM == 1)
		{SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO9 = (FUNCCTRL&1);}
	}
	__nds32__dsb();
	__nds32__dsb();
	return GPIO_NUM == 0 ? SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO8 : SG_PADCTRL->rPAD_FUNC_CTRL4.bits.GPIO9;
}
U8 FA_GPIOCTRL_PS(U8 GPIO_NUM, U8 PSCTRL)
{
	if(GPIO_NUM > 2)//Invalid GPIO number
	{
		return 0xFF;
	}
	if(1 == (PSCTRL >> 1))
	{
		if(0 == (PSCTRL&1))
		{SG_PADCTRL->rPAD_PS_CTRL5.dAll &= BIT_CLR_0(8+GPIO_NUM);}
		else
		{SG_PADCTRL->rPAD_PS_CTRL5.dAll |= BIT_SET_1(8+GPIO_NUM);}
	}
	__nds32__dsb();
	__nds32__dsb();
	return ((U8)SG_PADCTRL->rPAD_PS_CTRL5.dAll >> (8+GPIO_NUM))&1;
}
U8 FA_GPIOCTRL_PE(U8 GPIO_NUM, U8 PECTRL)
{
	if(GPIO_NUM > 2)//Invalid GPIO number
	{
		return 0xFF;
	}
	if(1 == (PECTRL >> 1))
	{
		if(0 == (PECTRL&1))
		{SG_PADCTRL->rPAD_PE_CTRL5.dAll &= BIT_CLR_0(8+GPIO_NUM);}
		else
		{SG_PADCTRL->rPAD_PE_CTRL5.dAll |= BIT_SET_1(8+GPIO_NUM);}
	}
	__nds32__dsb();
	__nds32__dsb();
	return (U8)(SG_PADCTRL->rPAD_PE_CTRL5.dAll >> (8+GPIO_NUM))&1;
}
U8 FA_GPIOCTRL_MODE(U8 GPIO_NUM, U8 MODECTRL)
{
	if(GPIO_NUM > 2)//Invalid GPIO number
	{
		return 0xFF;
	}
	if(1 == (MODECTRL >> 1))
	{
		if(0 == (MODECTRL&1))
		{SG_PADCTRL->rPORT_MODE16.bits.dVAL &= BIT_CLR_0(GPIO_NUM);}
		else
		{SG_PADCTRL->rPORT_MODE16.bits.dVAL |= BIT_SET_1(GPIO_NUM);}
	}
	__nds32__dsb();
	__nds32__dsb();
	return (U8)(SG_PADCTRL->rPORT_MODE16.bits.dVAL >> (GPIO_NUM))&1;
}
U8 FA_GPIOCTRL_DATA(U8 GPIO_NUM, U8 DATACTRL)
{
	if(GPIO_NUM > 2)//Invalid GPIO number
	{
		return 0xFF;
	}
	if(1 == (DATACTRL >> 1))
	{
		if(0 == (DATACTRL&1))
		{SG_PADCTRL->rPORT_DATA16.bits.dVAL &= BIT_CLR_0(GPIO_NUM);}
		else
		{SG_PADCTRL->rPORT_DATA16.bits.dVAL |= BIT_SET_1(GPIO_NUM);}
	}
	__nds32__dsb();
	__nds32__dsb();
	return (U8)(SG_PADCTRL->rPORT_DATA16.bits.dVAL >> (GPIO_NUM))&1;
}

//////for GPIO8,9,10 only
////////////GPIO8 for VPP Ctrl, Low-Enable / High-Disable(Dft.)
void FA_GPIOCTRL()
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	HostCmdResponse();
	U8 GPIO_NUM		= (U8)((pCtrl->dMode 			)&0x03);//0-GPIO8, 1-GPIO9, 2-GPIO10, others-invalid
	U8 PSCTRL	 	= (U8)((pCtrl->FeatureAddress	)&0x03);//2'b10-Set to 0, 2'b11-Set to 1, 2'b0X-no setting
	U8 PECTRL 		= (U8)((pCtrl->bChan			)&0x03);//2'b10-Set to 0, 2'b11-Set to 1, 2'b0X-no setting
	U8 MODECTRL		= (U8)((pCtrl->bChip			)&0x03);//2'b10-Set to 0, 2'b11-Set to 1, 2'b0X-no setting
	U8 FUNCCTRL 	= (U8)((pCtrl->bRBMode			)&0x03);//2'b10-Set to 0, 2'b11-Set to 1, 2'b0X-no setting
	U8 DATACTRL 	= (U8)((pCtrl->P4				)&0x03);//2'b10-Set to 0, 2'b11-Set to 1, 2'b0X-no setting
	
	((U8*)MEM_DMA_BUFF_ADDR)[0] = 	FA_GPIOCTRL_PS(		GPIO_NUM, 0);
	((U8*)MEM_DMA_BUFF_ADDR)[1] = 	FA_GPIOCTRL_PE(		GPIO_NUM, 0);
	((U8*)MEM_DMA_BUFF_ADDR)[2] = 	FA_GPIOCTRL_MODE(	GPIO_NUM, 0);
	((U8*)MEM_DMA_BUFF_ADDR)[3] = 	FA_GPIOCTRL_FUNC(	GPIO_NUM, 0);
	((U8*)MEM_DMA_BUFF_ADDR)[4] = 	FA_GPIOCTRL_DATA(	GPIO_NUM, 0);

	
	FDL_Log("PS = %d", ((U8*)MEM_DMA_BUFF_ADDR)[0]);
	FDL_Log("PE = %d", ((U8*)MEM_DMA_BUFF_ADDR)[1]);
	FDL_Log("MODE = %d", ((U8*)MEM_DMA_BUFF_ADDR)[2]);
	FDL_Log("FUNC = %d", ((U8*)MEM_DMA_BUFF_ADDR)[3]);
	FDL_Log("DATA = %d", ((U8*)MEM_DMA_BUFF_ADDR)[4]);
	
	
	FDL_Log("GPIO = %d", GPIO_NUM);
	FDL_Log("PSCTRL = %d", PSCTRL);
	FDL_Log("PECTRL = %d", PECTRL);
	FDL_Log("MODECTRL = %d", MODECTRL);
	FDL_Log("FUNCCTRL = %d", FUNCCTRL);
	FDL_Log("DATACTRL = %d", DATACTRL);
	
	((U8*)MEM_DMA_BUFF_ADDR)[0] = 	FA_GPIOCTRL_PS(		GPIO_NUM, PSCTRL);
	((U8*)MEM_DMA_BUFF_ADDR)[1] = 	FA_GPIOCTRL_PE(		GPIO_NUM, PECTRL);
									FA_GPIOCTRL_DATA(	GPIO_NUM, DATACTRL);
	((U8*)MEM_DMA_BUFF_ADDR)[2] = 	FA_GPIOCTRL_MODE(	GPIO_NUM, MODECTRL);
	((U8*)MEM_DMA_BUFF_ADDR)[3] = 	FA_GPIOCTRL_FUNC(	GPIO_NUM, FUNCCTRL);
	((U8*)MEM_DMA_BUFF_ADDR)[4] = 	FA_GPIOCTRL_DATA(	GPIO_NUM, 0);

	
	FDL_Log("PS = %d", ((U8*)MEM_DMA_BUFF_ADDR)[0]);
	FDL_Log("PE = %d", ((U8*)MEM_DMA_BUFF_ADDR)[1]);
	FDL_Log("MODE = %d", ((U8*)MEM_DMA_BUFF_ADDR)[2]);
	FDL_Log("FUNC = %d", ((U8*)MEM_DMA_BUFF_ADDR)[3]);
	FDL_Log("DATA = %d", ((U8*)MEM_DMA_BUFF_ADDR)[4]);
	
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);////return register value
}
void FA_CQINTCfgChk()
{
	T_RDWR_DATA *pCmd = (T_RDWR_DATA*)tScsiCmdHead.bDataDefine;
	T_FDL_FTCTRL *pCtrl = (T_FDL_FTCTRL *)(&pCmd->dIndex[0]);
	U8 bChan = pCtrl->bChan;
	U8 TaskType = pCtrl->dMode;
	U32 maxtime_threshold = pCtrl->dDelayCtrl;	//us，check status最长等待时间限制
	U32 mintime_threshold = pCtrl->dSectionCnt;
	U8 check_status_maxtime_interrupt_enable = pCtrl->bChip;	//超时是否触发中断，1：开启超时中断，0：关闭超时中断
	U8 cq_continue = pCtrl->bRBMode;	//chk_st0/1指令timeout后，停止CQ还是继续执行CQ? 1：停止	 0：继续执行
	switch(TaskType)
	{
		case 0:
			SG_FC(bChan)->rCHK_ST_RB_CNT = 200*maxtime_threshold;	//1us超时	//200000 = 200Mhz下，1ms超时
			__nds32__dsb();
			SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_INT_EN = check_status_maxtime_interrupt_enable;	//超时是否触发中断，1：开启超时中断，0：关闭超时中断
			__nds32__dsb();
			SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL = cq_continue; //chk_st0/1指令timeout后，停止CQ还是继续执行CQ? 1：停止	 0：继续执行
			__nds32__dsb();
			SG_FC(bChan)->rCHK_ST_RB_THROD_MIN = mintime_threshold;
			__nds32__dsb();
			break;
		case 1:
			FA_FLASH_INFO_BUFF[0] = bCqTimeoutFlag[bChan];
			FA_FLASH_INFO_BUFF[1] = SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_INT_EN;
			FA_FLASH_INFO_BUFF[2] = SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAX_CTRL;
			FA_FLASH_INFO_BUFF[3] = SG_FC(bChan)->rCHK_ST_RB_CNT;
			FA_FLASH_INFO_BUFF[4] = SG_FC(bChan)->rCHK_ST_RB_THROD_MIN;
			
			bCqTimeoutFlag[bChan] = 0;
			break;
	}
	HostCmdResponse();
	HostDataTransfer(DMA1_RD_TO_HOST,MEM_DMA_BUFF_ADDR,1,JOINT_NONE);
}

