#include "pri_cmd.h"
#include "main.h"
#include "sg9081.h"
#include "debug.h"
#include "flash_driver.h"
#include "string.h"
#include "sata_typedef.h"
#include "fdl_test.h"
#include "uitility.h"
#include "hal_test.h"

PRI_CMD tPrivCMD;

void MemCopy(U8 *pDst,U8 *pSrc,U16 wLen)
{
    while(wLen--)
    {
        *pDst++ = *pSrc++;
    }
}

void HostDataTransfer(DMA1_Dir bDir,U32 dBuffAddr,U16 wSec,Joint_Mode bJoint)
{
    HAL_ADMStartDMA1(dBuffAddr,0,wSec,bDir,bJoint);
    HAL_ADMWaitDMADone(0xFF);
}

void HostCmdResponse()
{
    HostDataTransfer(DMA1_RD_TO_HOST,(U32)(&tScsiDataHead.dTitle[0]),1,JOINT_NONE);
}


static const PRIV_FUNC_T  PrivCmdTip[] =
{
	
    {
        SUBCODE_FDL_TEST_INIT,
        FDL_CMD_C0_FDLInit,
    },
#ifdef __FDL_TEST_MODE_EN__
/////flash analys added start
	
    {
        SUBCODE_HAL_WriteBlk,
        FA_WriteBlk,
    },
	{
		SUBCODE_ERASE,
		FA_EraseBlk,
	},
	{
		SUBCODE_WRITE,
		FA_WritePage,
	},
	{
		SUBCODE_READ,
		FA_ReadData,
	},
	{
		SUBCODE_FDL_READ_PARAMETER_PAGE,
		FA_ReadParameterPage,
	},
    {
        SUBCODE_UNIVERSAL,
        FA_Universal,   
    },
	{
		SUBCODE_HAL_SetFeature,
		FA_SetFeature,
	},
	{
		SUBCODE_HAL_GetFeature,
		FA_GetFeature,
	},
	{
		SUBCODE_FDL_READ_ID,
		FA_ReadID,
	},	
	{
		SUBCODE_FDL_READ_UID,
		FA_ReadUID,
	},
    {
        SUBCODE_HAL_Reset,
        FA_Reset,
    },
    {
    	SUBCODE_RSBUFF_READ,
		RS_Report,
    },
    {
    	SUBCODE_RSBUFF_INIT,
		RS_Buff_Init,
    },
    
    {
    	SUBCODE_FDL_Delay,
		HAL_Delay_ms,
    },
    
    {
    	SUBCODE_FDL_GET_EX_FEATURE,
		FA_GetExtendedFA,
    },
    {
    	SUBCODE_FDL_SET_EX_FEATURE,
		FA_SetExtendedFA,
    },
    {
    	SUBCODE_FDL_SET_PARAMETER,
		FA_SetParamter,
    },
    {
    	SUBCODE_FDL_GET_PARAMETER,
		FA_GetParamter,
    },
    
    {
    	SUBCODE_FDL_CHECK_BLK_DATA,
		FA_CheckBlkData,		
    },
    {
    	SUBCODE_SET_OFS,
		FA_SetOffset,
    },
    {
    	SUBCODE_COPYBACK_WRITE,
    	FA_COPYBACKWrite,
    },
	{
		SUBCODE_FIND_OV_INIT,
		FA_FindOV_Init,
	},
	{
		SUBCODE_FIND_OV,
		FA_FindOV,
	},
    {
        SUBCODE_FDL_READPAGEWITHNODATA,
        ReadPage_WithNoDataOut,
    },
    
	{
		SUBCODE_FA_HT_SOCKET_INIT,
		FA_HighTempSocketInit,
    },
    {
		SUBCODE_FA_HT_SOCKET_FAN_SWITCH,
		FA_SocketFanSwitch,
    },
	{
		SUBCODE_FA_HT_SOCKET_HEATER_SWITCH,
		FA_SocketHeaterSwitch,
	},
	{
		SUBCODE_FA_HT_SOCKET_READ_TEMP,
		FA_HighTempSocketReadTemp,
	},
	{
		SUBCODE_FA_HT_SOCKET_SET_TEMP,
		FA_SocketSetTemperature,
	},
	{
		SUBCODE_FA_Flash_Info_Init,
		FA_Flash_Info_Init,
	},
	{
		SUBCODE_FA_OfflineTaskInit,
		FA_OfflineTaskInit,
	},
	{
		SUBCODE_FA_OfflineTaskInterrupt,
		FA_OfflineTaskInterrupt,
	},
	{
		SUBCODE_FA_GPIO_CTRL,
		FA_GPIOCTRL,
	},
		
//	{
//      SUBCODE_FDL_LLRTEST,
//      FA_LLRTest,
//	},
//	{
//		SUBCODE_FDL_NCQ_RD_CFG,
//		FA_NcqRead_Config,
//    },
/////flash analys added end

//
//	{
//		SUBCODE_FDL_ERASE,
//		FDL_CMD_C1_EraseBlocks,
//	},
//	{
//		SUBCODE_FDL_WRITE,
//		FDL_CMD_C2_WriteBuffToFlash,
//	},
//	{
//		SUBCODE_FDL_READ,
//		FDL_CMD_C3_ReadClstToBuff,
//	},
//	{
//        SUBCODE_FDL_WRITE_LITTLE_DATA,
//        FDL_CMD_C5_WriteLittleData,
//    },
//    {
//        SUBCODE_FDL_READ_LITTLE_DATA,
//        FDL_CMD_C6_ReadLittleData,
//    },
//	{
//		SUBCODE_FDL_JOINT_WRITE,
//		FDL_CMD_C7_JointWritePages,
//	},
	//{
    //    SUBCODE_FDL_JOINT_READ_WITH_MASK,
    //    FDL_CMD_C9_JointReadClstsWithMask,
    //},
//    {
//        SUBCODE_FDL_PREREAD_TABLE_TO_BUFF4K,
//		FDL_CMD_C9_PreReadTableToBuff4K
//    },
//    {
//        SUBCODE_FDL_WRITE_DISCRETE_BUFF_TO_FLASH,
//        FDL_CMD_CA_WriteDiscreteBuffToFlash,
//    },
//    {
//        SUBCODE_FDL_READ_CLST_TO_DISCRETE_BUFF,
//        FDL_CMD_CB_ReadClstToDiscreteBuff,
//    },
//
//	{
//		SUBCODE_FDL_DISCRETE_BUFF_READ_NOJOINT,
//		FDL_CMD_CF_DiscreteBuffReadNoJoint,
//	},
//
//	{
//		SUBCODE_FDL_JOINT_READ,
//		FDL_CMD_C8_JointReadClsts,
//	},		
//	{
//        SUBCODE_FDL_COPYBACK_PROG,
//        FDL_CMD_D8_CopyBackProg,
//    },
//	{
//        SUBCODE_FDL_MULTI_LUN_EX_DIE_COPYBACK_PRG,
//        FDL_CMD_D9_MultiLunExdieCopyBackPrg,
//    },
//    {
//        SUBCODE_FDL_MULTI_LUN_IN_DIE_COPYBACK_PRG,
//        FDL_CMD_DA_MultiLunIndieCopyBackPrg,
//    },
//    {
//        SUBCODE_FDL_ORDER_PRG_BUFF_TO_FLASH,
//        FDL_CMD_DB_OrderPrgBuffToFlash,
//    },
//    {
//        SUBCODE_FDL_JOINT_ORDER_PRG_UNIT,
//        FDL_CMD_DC_JointOrderPrgUnit,
//    },
//    {
//        SUBCODE_FDL_MULTI_LUN_EX_DIE_COPYBACK_ORDER_PRG,
//        FDL_CMD_DD_MultiLunExdieCopyBackOrderPrg,
//    },
//    {
//        SUBCODE_FDL_MULTI_LUN_IN_DIE_COPYBACK_ORDER_PRG,
//        FDL_CMD_DE_MultiLunIndieCopyBackOrderPrg,
//    },
//    {
//        SUBCODE_FDL_MULTI_LUN_INNER_COPYBACK_ORDER_PRG,
//        FDL_CMD_DF_MultiLunInnerCopyBackOrderPrg,
//    },
//
//	{
//		SUBCODE_FDL_DEBUG,
//		FDL_CMD_D0_ReadClstToBuffDebug,
//	},
//	{
//		//SUBCODE_HAL_GOCACHE_CQ_BURST_WR,
//		//HAL_CMD_E3_GoCacheBurstWriteRead,
//	},
//	{
//		//SUBCODE_HAL_GOCACHE_WITH_FLASH,
//		//HAL_CMD_E5_GocacheWithFlash,
//	},
//    {
//            SUBCODE_FDL_WRITE_DISCRETE_BUFF_TO_FLASH,
//            FDL_CMD_CA_WriteDiscreteBuffToFlash,
//    },
//    {
//            SUBCODE_FDL_READ_CLST_TO_DISCRETE_BUFF,
//            FDL_CMD_CB_ReadClstToDiscreteBuff,
//    },
//    {
//            SUBCODE_FDL_JOINT_DISCRETE_WIRTE_TO_BUFF,
//            FDL_CMD_CD_JointDiscreteWirteToBuff,
//    },
//    {
//    		SUBCODE_FDL_READ_ID,
//            FDL_CMD_CE_FlashReadID,
//    },
	/*
    {
    		SUBCODE_FDL_WRITE_TLC_BLK,
			FDL_CMD_F2_WriteTlcBlk,
    },
	{
            SUBCODE_FDL_WRITE_TLC_PAGE,
			FDL_CMD_F3_WriteTlcPage,
    },     
	{
			OP_CODE_WRITE_PHYPAGE,
			FDL_CMD_A8_WriteFlash_PhyPage,
	},
	{
			OP_CODE_READ_PHYPAGE,
			FDL_CMD_A9_ReadFlash_PhyPage,
	},
	{
			CMD_SET_REGISTER_VALUE,
			SetRegisterValue,
	},
	{
			CMD_GET_REGISTER_VALUE,
			GetRegisterValue,
	},
	{
			OP_CODE_GET_VALIDCHAN,
			Get_ValidChan,
	},
	{
			SUBCODE_FDL_INJECTION_CRC_ERR,
			FDL_CMD_FF_InjectionCRCErr,
	},
	{
			OP_CODE_SET_READOFFSET,
			SetReadOffsVal,
	},
	{
			OP_CODE_ERASE_BLK,
			FDL_EraseBlk,
	},
	{
			OP_CODE_SET_LDPC_ITR,
			FDL_SetLDPC_ItrVal,
	},
	{
			OP_CODE_SET_DMAINFO_SIZE,
			FDL_SetSmallDMAInfoSize,
	},
	{
			OP_CODE_READ_CLST_TO_DISCRETE_BUFF,
			Expt_ReadClstToDiscreteBuff,
	},
//	{
//			OP_CODE_LDPC_SET_PVALUE,
//			Ldpc_SetPValue,
//	},
//	{
//			OP_CODE_LDPC_SET_LLR,
//			Ldpc_SetLLRAndOffset,
//	},
	{
			OP_CODE_VERIFY_TLC_PAGE,
			FDL_CMD_E0_VerifyTLCPage,
	},
	{
		    SUBCODE_FDL_SCAN_FLASH_DRIVER,
			FDL_CMD_E2_ScanFlashDriver,
	},
	{
		    SUBCODE_FDL_SCAN_GC_DRIVER,
			FDL_CMD_E3_ScanGCDriver,
	},
	{
		    SUBCODE_FDL_FLASH_INIT,
			FDL_CMD_E4_FlashInit,
	}
	*/
#endif
};

void Scsi_JptSwitchHandle(U8 bSubOpCode)
{
    U8 idx;
//	LOG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    for (idx = 0; idx < sizeof(PrivCmdTip) / sizeof(PRIV_FUNC_T); idx++)
    {
        if (bSubOpCode == PrivCmdTip[idx].PrivCmd)
        {
//        	LOG_PRINT("OpCode = %x\r\n", bSubOpCode);
#ifndef __DUAL_CORE__
        	NVIC_DisableIRQ(IRQ_SC);
#endif
            PrivCmdTip[idx].Function();

#ifndef __DUAL_CORE__
            NVIC_EnableIRQ(IRQ_SC);
#endif
            break;
        }
    }
//    LOG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

