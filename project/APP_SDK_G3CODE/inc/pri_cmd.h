#ifndef __JPT_CMD__
#define __JPT_CMD__

#include "common.h"
#include "scsi.h"
#include "hal.h"
#include "flash_driver.h"
#include "hal.h"
#define CMD_READ_FLASH_ID           		(0xA1)
#define CMD_RESET_FLASH           		    (0xA2)
#define CMD_READ_XRAM              		    (0xA3)
#define CMD_WRITE_XRAM            		    (0xA4)
#define CMD_SET_REGISTER_VALUE    		    (0xA5)
#define CMD_GET_REGISTER_VALUE     		    (0xA6)
#define CMD_EXCUTE_CMD            		    (0xA7)
#define OP_CODE_WRITE_PHYPAGE				(0xA8)
#define OP_CODE_READ_PHYPAGE				(0xA9)
#define OP_CODE_GET_VALIDCHAN				(0xAA)
#define OP_CODE_SET_READOFFSET				(0xAB)
#define OP_CODE_ERASE_BLK					(0xAC)
#define OP_CODE_SET_LDPC_ITR				(0xAD)
#define OP_CODE_SET_DMAINFO_SIZE			(0xAE)

#define OP_CODE_READ_CLST_TO_DISCRETE_BUFF	(0xB0)

///////flash analyse group added start
#define SUBCODE_RSBUFF_READ					(0xB1)	//20180307 Status Buffer Read
#define SUBCODE_RSBUFF_INIT					(0xB2)	//20180307 Status Buffer Init to 0xCC
#define SUBCODE_SET_OFS						(0xBA)	//20230411 Set Offset With SetEXFeature

#define SUBCODE_FDL_READ_PARAMETER_PAGE     (0xBB)
#define SUBCODE_COPYBACK_WRITE				(0xBC)
#define SUBCODE_FIND_OV						(0xBD)	//20240401 find ov
#define SUBCODE_FIND_OV_INIT				(0xBE)
#define SUBCODE_FA_GPIO_CTRL				(0xBF)	//20250102 GPIO CTRL

#define SUBCODE_FDL_TEST_INIT				(0xC0)
#define SUBCODE_ERASE 	                    (0xC1)
#define SUBCODE_WRITE                       (0xC2)
#define SUBCODE_READ                        (0xC3)
#define SUBCODE_HAL_WriteBlk				(0xC5)
#define SUBCODE_HAL_Reset					(0xC6)
#define SUBCODE_HAL_SetFeature				(0xC7)
#define SUBCODE_HAL_GetFeature				(0xC8)
#define SUBCODE_UNIVERSAL					(0xCA)
#define SUBCODE_FDL_CHECK_BLK_DATA			(0xCD)
#define SUBCODE_FDL_READ_ID					(0xCE)

#define SUBCODE_FDL_GET_EX_FEATURE			(0xD0)
#define SUBCODE_FDL_SET_EX_FEATURE			(0xD1)
#define SUBCODE_FDL_GET_PARAMETER			(0xD2)
#define SUBCODE_FDL_SET_PARAMETER			(0xD3)
#define SUBCODE_FDL_Delay					(0xD8)
#define SUBCODE_FDL_READPAGEWITHNODATA		(0xD9)
#define SUBCODE_FA_HT_SOCKET_INIT			(0xDA)
#define SUBCODE_FA_HT_SOCKET_FAN_SWITCH		(0xDB)
#define SUBCODE_FA_HT_SOCKET_HEATER_SWITCH	(0xDC)
#define SUBCODE_FA_HT_SOCKET_READ_TEMP		(0xDD)
#define SUBCODE_FA_HT_SOCKET_SET_TEMP		(0xDE)
#define SUBCODE_FA_Flash_Info_Init			(0xDF)
#define SUBCODE_FA_OfflineTaskInit			(0xE0)
#define SUBCODE_FA_OfflineTaskInterrupt		(0xE1)
#define SUBCODE_FDL_READ_UID	            (0xE3)
#define SUBCODE_READ_DISTURB	            (0xEA)
///////flash analyse group added end




///////flash analyse group added end


//#define OP_CODE_LDPC_SET_PVALUE					(0xB1)//
//#define OP_CODE_LDPC_SET_LLR					(0xB2)
//#define OP_CODE_INSERT_BAD_DMA					(0xB3)

//#define SUBCODE_FDL_ERASE									(0xC1)
//#define SUBCODE_FDL_WRITE									(0xC2)
//#define SUBCODE_FDL_READ									(0xC3)
//#define SUBCODE_FDL_READ_WITH_MASK							(0xC4)
//#define SUBCODE_FDL_WRITE_LITTLE_DATA						(0xC5)
//#define SUBCODE_FDL_READ_LITTLE_DATA						(0xC6)
//#define SUBCODE_FDL_JOINT_WRITE								(0xC7)
//#define SUBCODE_FDL_JOINT_READ								(0xC8)
////#define SUBCODE_FDL_JOINT_READ_WITH_MASK                    (0xC9)
//#define SUBCODE_FDL_PREREAD_TABLE_TO_BUFF4K                 (0xC9)
////#define SUBCODE_FDL_WRITE_DISCRETE_BUFF_TO_FLASH			(0xCA)
////#define SUBCODE_FDL_READ_CLST_TO_DISCRETE_BUFF				(0xCB)
//#define SUBCODE_FDL_MERGE_BUFF_CLST_TO_FLASH				(0xCC)
////#define SUBCODE_FDL_CONTINUE_TASK                           (0xCD)
//#define SUBCODE_FDL_DISCRETE_BUFF_READ_NOJOINT				(0xCF)
//
//
//#define SUBCODE_FDL_DEBUG									(0xD0)
//#define SUBCODE_FDL_COPYBACK_PROG							(0xD8)
//#define SUBCODE_FDL_MULTI_LUN_EX_DIE_COPYBACK_PRG			(0xD9)
//#define SUBCODE_FDL_MULTI_LUN_IN_DIE_COPYBACK_PRG			(0xDA)
//#define SUBCODE_FDL_ORDER_PRG_BUFF_TO_FLASH					(0xDB)
//#define SUBCODE_FDL_JOINT_ORDER_PRG_UNIT					(0xDC)
//#define SUBCODE_FDL_MULTI_LUN_EX_DIE_COPYBACK_ORDER_PRG		(0xDD)
//#define SUBCODE_FDL_MULTI_LUN_IN_DIE_COPYBACK_ORDER_PRG		(0xDE)
//#define SUBCODE_FDL_MULTI_LUN_INNER_COPYBACK_ORDER_PRG		(0xDF)

#define SUBCODE_FDL_LLRTEST                      (0xE5)

//GoCache
#define SUBCODE_HAL_GOCACHE_MCU_PORT_WR          (0xEA)
#define SUBCODE_HAL_GOCACHE_CQ_PORT_WR           (0xEB)
#define SUBCODE_HAL_GOCACHE_MCU_BURST_WR         (0xEC)
#define SUBCODE_HAL_GOCACHE_CQ_BURST_WR          (0xED)
#define SUBCODE_HAL_GOCACHE_WITH_FLASH           (0xEE)
#define SUBCODE_GOCACHE_FUNCTION_SWITCH          (0xEF)



#define SUBCODE_FDL_NCQ_RD_CFG					(0xF7)
#define SUBCODE_DUMMY_READWRITE_JUMP_OUT                   (0xF0)
#define SUBCODE_DUMMY_READWRITE_JUMP_IN                   (0xF1)
#define SUBCODE_FDL_WRITE_TLC_PAGE                     (0xF3)

#define SUBCODE_FDL_WRITE_TLC_BLK				(0XF2)//Aaron 

//#define OP_CODE_VERIFY_TLC_PAGE						(0xE0)/*快速检验CopyBack搬移到TLC Page的数据*/
#define SUBCODE_FDL_SCAN_FLASH_DRIVER				(0xE2)/*Flash Drive Scan*/
#define SUBCODE_FDL_SCAN_GC_DRIVER					(0xE3)/*Flash Drive Scan*/
#define SUBCODE_FDL_FLASH_INIT						(0xE4)/*Flash Drive Scan*/


#define SUBCODE_FDL_DUMMY_WRITE_DATA				(0xFE)
#define SUBCODE_FDL_INJECTION_CRC_ERR               (0xFF)
//#define HOST_WR         (DMA1_WR)
//#define HOST_RD         (DMA1_RD)

typedef struct STR_PRI_CMD
{
    U32 bOpPageAddr;
    U16 bOpBlk;
    U16 bOpPageOffs;

    U8  bOpCh;
    U8  bOpCe;
    U8  bOpLun;
    U8  bOpPln;

    U32 dOpByteCnt;
    U16 wOpSctCnt;
    U8  bErrDma;
    U8  bErrCnt;

    U8  bDir;
    U8  Reserve[7];
}PRI_CMD,*PPRI_CMD;

typedef struct
{
    U8 PrivCmd;
    void (*Function)(void);
}PRIV_FUNC_T;

extern PRI_CMD tPrivCMD;

void MemCopy(U8 *pDst,U8 *pSrc,U16 wLen);
void Scsi_JptSwitchHandle(U8 bSubOpCode);
void HostDataTransfer(DMA1_Dir bDir,U32 dBuffAddr,U16 wSec,Joint_Mode bJoint);
void HostCmdResponse();
#endif





