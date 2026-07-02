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
		SUBCODE_READ_DISTURB,
		FA_ReadDisturb,
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

