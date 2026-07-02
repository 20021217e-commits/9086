
#ifndef __FTL__
#define __FTL__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "flash_driver_struct.h"


typedef  struct
{   
    U32 Capacity;   	// 测试容量
    U16 WritePagesOnce; // 一次写多少页(一般是1024,但有不能平均分配的情况,需要调整)
    U08 PlaneOfDevice;  // 当前硬件测试的Plane数
    U16 EraseBlockCnt;  // 需擦除的页数
} T_TXBENCH_CRTL_PARAM; 

typedef  struct
{   
    U08 bRdFunc;   	
    U08 bRd4KFunc; 
    U08 bTotelLun;  
    U08 bStartHold;
	U08 bMidHold;
	U08 bSectionSize;
	U08 bTableEn;
	U08 bRev[1];
} T_NCQREAD_CRTL_PARAM; 

typedef  struct
{   
	U32 dGroupCnt; 
    U32 dGroupIdx;   	
    U08 bSize[0x400]; //Section Size,ClstLen
   	U32 dDtRtyInf[0x400]; //Section RetryInfor 2Bit Meaning 0,OK;1Pass;2Fail;3FF
	U32 dTbRtyInf[0x400]; //Section RetryInfor 2Bit Meaning 0,OK;1Pass;2Fail;3FF
	
	U32 dDataExBuff;
	U32 dTabExBuff;
	U08 bTestMode;
	U08 bDataType;
	U08 bRev[2];
} T_PRE4KDATA_GROUP_PARAM; 

typedef  struct
{   
	U32 dRow[4][4]; 
	U32 dCol[4][4]; 
} T_PRE4KCQ_ReStart_PARAM; 


void FTL_Initial(void);
void LgRdWrCtrl(U32 dLBA, U32 dSectCnt, U8 bDir);
void FTL_SATAErrorHandle(U8 bTag,U32 dFisNum);
void FTL_CreateNandAddrWrite(U08 bChanCnt,U16 wBlkStartAddr,U16 wAddrCnt);
void FdlPhyAddrGenerateMultiPlaneAndCEMode(T_FLASH_PHY_ADDR  *ptPageAddr,
											U16 wAddrCnt,
											U08 bDir,
											U08 bChanCnt,
											U16 wBlkStartAddr,
											U32 dPageOffset,
											U08 bInValidChan,
											U08 bDataType);

void FTL_CreateNandAddrRead(U08 bChanCnt,U16 wBlkStartAddr,U16 wAddrCnt);
void FTL_CreateNandAddrErase(U08 bChanCnt,U16 wBlkStartAddr,U16 wBlkCnt);
void FTL_EraseBlockInit(U16 wStartBlock);
void FTL_WriteBlockInit(U08 bMode,U16 wStartBlock,U16 wPageOffset,U16 WritePagesOnce);
void FTL_ReadBlockInit(U08 bMode,U16 wStartBlock);
void FTL_ReadRandClustInit(U08 bMode,U16 wStartBlock);
void FTL_LGRead();
void FTL_LGWrite();
void FTL_ReadRetryFail(U32 dIdxOffst);
void FTL_WriteFail(U32 dIdxOffst);
void FTL_CreateNandAddrWirtRead(T_FLASH_PHY_ADDR  *ptPageAddr,U16 wAddrCnt,U08 bDir,U08 bChanCnt,U16 wBlkStartAddr,U32 dPageOffset,U08 bInValidChan,U08 bDataType,U08 Mode);


#define __TEST_UPDATE_NAND_ADDR_WRITE__/*是否启用物理地址实时更新功能(Write)*/
#define __TEST_UPDATE_NAND_ADDR__		/*是否启用物理地址实时更新功能(Read)*/

//#define __TEST_JUPITER_SPEED_SLCMODE__	/*Jupiter NCQ Continue Test*/
//#define __TEST_TXBENCH_SPEED_SLCMODE__	/*TXBench NCQ Continue Test SLCMode*/
//#define __TEST_TXBENCH_SPEED_TLCMODE__	/*TXBench NCQ Continue Test TLCMode*/
#define TEST_CHAN_MAX      4			/*通道个数*/



#ifdef __cplusplus
}
#endif
#endif
