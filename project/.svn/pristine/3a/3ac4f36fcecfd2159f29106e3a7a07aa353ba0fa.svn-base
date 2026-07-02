#ifndef _HAL_TEST_H_
#define _HAL_TEST_H_
#include "flash_driver_struct.h"
#include "fdl_test.h"
#include "fdl_read_retry.h"
#include "fdl_soft_decode.h"



typedef struct
{
	T_FLASH_PHY_BLK_ADDR tBlockAddr;
	T_FLASH_PHY_ADDR tPageAddr;
	U32 dBlockCnt;
	U32 dPageCnt;
	U32 dGocacheAddr;
	U32 dGocacheLen;
	U8 bJointEn;
	U8 bPortOrBurst;
	U8 bDir;
	U8 bChanCnt;
}T_HAL_GOCACHE_FLASH, P_HAL_GOCACHE_FLASH;   //为了验证Gocache与Flash同时操作，创建的私有命令中的数据结构体


typedef struct
{
	U16 TargetTemperature;
	U16 LastTemperature;
	U16 CurrentTemperature;
	U16 PreviousTemperature;
	U16 StartTemperature; 
	int dt;	//10.0 ms, interupt period
}PidParameters;

typedef struct
{
    U8 CE_IN_CHANNEL;	//每个Channel中CE的数量
    U8 LUN_IN_CE;	//每个CE中LUN的数量
    U8 PLANE_IN_LUN; //每个LUN中的plane数量
    U16 FLASH_CODE;	//FlashCode
    U32 PAGE_IN_BLOCK; //每个Block中的物理页数量
    U32 SLC_WL_IN_BLOCK; //每个Block中的SLC模式物理页数量
    U32 BYTE_IN_PAGE; //页字节数
    U8 SECTOR_IN_PAGE; //1个page有多少个扇区
    U8 ROW_ADDR_CYCLE; //默认地址轮数（含列地址），比如5或6

    U16 FA_ERASE_TYPE; //擦除方式类别，
    U16 FA_PROGRAM_TYPE; //编程方式类别，比如是否有PreCMD指定WL中的页，是否是one-pass编程，编程顺序（地址计算）类别，比如QLC的锯齿或Coarse-Fine编程
    U16 FA_READ_TYPE; //读方式类别，比如是否有PreCMD指定页，是否发2Eh配置retry，是否使用ROIC（row地址后多加几轮地址）配置retry
    U16 FA_RETRY_TYPE; //配置offset的方式。
    U16 FA_SEED_TYPE;	//选择随机化种子类型（如三星256、512）

    U32 PROGRAM_ORDER_COUNT; //TLC模式下WL编程次数或者页编程次数

    U32 BLOCK_ADDR_GAP;	//块地址的间隔，即Page Address的地址空间大小，2^(PA所用bit数)
    U32 LUN_ADDR_GAP;	//LUN地址的间隔，2^(BA所用bit数[包括plane地址]+PA所用bit数+保留位数-1）

	U8 ERASE_1st;	//e.g: 60h
    U8 ERASE_2nd;	//e.g:D0h
    U8 ERASE_2nd_MULTIPLANE;	//e.g:D0h
    U8 ERASE_SUSPEND;	//e.g:67h
    U8 ERASE_RESUME;	//e.g:D7h

    U8 PROG_1st;
    U8 PROG_1st_COPYBACK;
    U8 PROG_1st_MULTIPLANE;
    U8 PROG_2nd_SLC;
	U8 PROG_2nd_TLC;
    U8 PROG_2nd_TLC_CONFIRM;
	U8 PROG_2nd_MULTIPLANE;
    U8 PROG_2nd_CACHE;
    U8 PROG_SUSPEND;
    U8 PROG_RESUME;
    U8 PROG_COARSE; //YMTC Micron (QLC)

	U8 READ_1st;
	U8 READ_2nd;
	U8 READ_2nd_CACHE;
	U8 READ_CACHE_END;
	U8 READ_2nd_MULTIPLANE;
	U8 READ_2nd_COPYBACK;
	U8 READ_2nd_PARTIAL;
    U8 READ_PRE_RETRY; //Micron
    
	U8 CHANGE_COLUMN_1st;
	U8 CHANGE_COLUMN_1st_PARTIAL;
	U8 CHANGE_COLUMN_2nd;

	U8 RESET;
	U8 RESET_LUN;
	U8 RESET_SYNC;
	U8 HARD_RESET;

	U8 READ_ID;
	U8 READ_UID;
	U8 READ_PARAMETER_PAGE;

	U8 READ_STATUS;
	U8 READ_STATUS_ENHANCED;
	U8 READ_STATUS_ENHANCED_II;

	U8 SLC_ENABLE;
	U8 SLC_EXIT;

	U8 GET_FEATURE;
	U8 SET_FEATURE;
	U8 GET_FEATURE_LUN;
	U8 SET_FEATURE_LUN;

	U8 VOLUME_SET;
	U8 ODT_CONFIG;
	U8 ZQ_CALI_SHORT;
	U8 ZQ_CALI_LONG;
	U8 READ_DQ_TR;
	U8 WRITE_DQ_TR_TX_1st;
	U8 WRITE_DQ_TR_TX_2nd;
	U8 WRITE_DQ_TR_RX;
	U8 READ_2nd_ERASE_PAGE_CHECK;
}FLASH_INFO;

//#define NEW_FLASH_DEBUG
#define FLASH_DATA_BUFF_ADDR     (0x410000)     //Gocache与flash交叉读写验证中Flash数据读写buffer
#define GOCACHE_DATA_BUFF_ADDR   (0x430000)     //Gocache与flash交叉读写验证中Gocache数据读写buffer
#define USRDATA_BUFF_ADDR        (0x450000)     //Gocache与flash交叉读写验证中Usrdata数据读写buffer

#define GFWR_EN       (11)    //GOCACHE与Flash共同工作测试使能
#define GFWR_SUCCESS  (22)    //Gocache与Flash共同工作测试读写数据对比成功
#define GFWR_FAIL     (33)    //Gocache与Flash共同工作测试读写数据对比失败

#ifndef PORT_TEST
#define PORT_TEST  (0)
#endif
#ifndef BURST_TEST
#define BURST_TEST (1)
#endif

#ifndef BURST_LEN
#define BURST_LEN  (512)
#endif

//Gocache Type
U8 HAL_Test_GoCacheWR(T_GOCACHE_PARM *ptGocacheParm);
void HAL_Test_GoCache_ISSI_RegWrireRead(U8 bChan,U8 bCe);

void HAL_Test_GoCachePortWriteRead(U8 bChan,U8 bCe);
void HAL_Test_GoCacheCqPortWriteRead(U8 bChan,U8 bCe);
void HAL_Test_GoCacheBurstWriteRead(U8 bChan,U8 bCe,U16 bBurstSize);
void HAL_Test_GoCacheCqBurstWriteRead(U8 bChan,U8 bCe,U8 bBurstSize);

void HAL_CMD_E1_GoCachePortWriteRead(void);
void HAL_CMD_E3_GoCacheBurstWriteRead(void);
void HAL_CMD_E4_HostDataTransfer(void);
void HAL_CMD_E5_GocacheWithFlash(void);

U08  HAL_Test_GetOrderPrgInfor(U16 wOrderNum,U32 *pPrgInfor);
void HAL_Test_WriteOnePage(U8 bPrgMod,U8 bChan,U8 bChip,U8 bLunOfChip,U8 bCmdTag,U32 dBaseRow,U32 dRowInBlk,U32 dBuffAddr,U32 dUdAddr,U8 bAttchSize,U8 bUdLen);
void HAL_Test_ReadOnePage(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff);

void HAL_Test_ErsBlks(U8 bErsMod,U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd);
void HAL_Test_PrgSlcBlks(U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd,U16 wPagSt,U16 wPagCnt,U8 bRndEn);
void HAL_Test_PrgTlcBlks(U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd,U8 bRndEn);
void HAL_Test_RedBlks(U8 bRdMode,U8 bChanSt,U8 bChanEd, U8 bLunSt, U8 bLunEd,U16 wBlockSt,U16 wBlockEd, U16 wPagSt,U16 wPagCnt,U32 dDtBuff,U32 dUdBuff,U8 bRndEn,U32 dRdInforAddr);
void HAL_Test_Copyback_Blk(U08 bCopybackMode, U16 wPageCount, U16 wSouBlk, U16 wDesBlk);
void HAL_Test_SnapReadOnePage(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff,U08 bCtls);
void HAL_Test_Cache_Random_Read(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff,U32 dNextRow);
void HAL_Test_MultiPlane_Read(U8 bMode,U8 bChan,U8 bChip,U8 bLun,U32 dBaseRow,U32 dRowInBlk,U32 dDataBuff,U32 dUserBuff);
void HAL_Test_System(void);


//flash analyse add
void FA_RecordRB(U8 bChan,U8 bChip, U32 PageOrder, U8 bSPIMode);
void FA_WriteBlk(void);
void FA_WritePage(void);
void FA_ReadData(void);
void FA_ReadParameterPage(void);
void FA_EraseBlk(void);
void FA_Universal(void);
void FA_GetExtendedFA(void);
void FA_SetExtendedFA(void);
void FA_SetFeature(void);
void FA_GetFeature(void);
void FA_SetParamter(void);
void FA_GetParamter(void);
void FA_ReadID(void);
void FA_ReadUID(void);
void FA_SubReadData_ReadUID(T_FDL_FTCTRL *pCtrl);
void FA_NCQReadData(T_FLASH_PHY_ADDR const *ptClstAddr,U32 dDMA2BaseAdd,U32 dofset,U08 bSectorOfPage);
void FA_Reset(void);
void RS_Buff_Init(void);
void RS_Report(void);
void HAL_Delay_ms(void);
void MultiPlane_WritePage(T_FDL_FTCTRL *pCtrl, U32 PageAddress, U32 PageOrder);
void MultiPlane_WritePage_PrefixCMD(T_FDL_FTCTRL *pCtrl, U32 PageAddress, U32 PageOrder);
void FA_WritePage_PreCMD(void);
void FA_WritePage_V5_MLC(void);
void FA_WritePage_WoPreCMD(void);
void FA_SubReadData_PreCMD(T_FDL_FTCTRL *pCtrl, U32 PageAdd, U32 PageOrder);
void FA_SubReadData_V5_MLC(T_FDL_FTCTRL *pCtrl, U32 PageAdd, U32 PageOrder);
void FA_SubReadData_WoPreCMD(T_FDL_FTCTRL *pCtrl, U32 PageAdd, U32 PageOrder);
void FA_SubReadData_ParameterPage(T_FDL_FTCTRL *pCtrl);
void FA_CheckBlkData(void);
void FA_SetOffset(void);
void FA_COPYBACKWrite(void);
void FA_FindOV_Init(void);
void FA_FindOV(void);
void FA_Flash_Info_Init(void);

//void FA_LLRTest(void);
void Fdl_MCUSendReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode, U08 PreCmd,U08* temp_RRT);
//void Read_DMAData(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor);
//U8 FA_SoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor);
//void Read_SoftDecodeData(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bSBNum);
//void Fdl_SoftDecodeToLDPC(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bSBNum);
//void FA_LLRTest(void);
void Fdl_MCUSendReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode, U08 PreCmd,U08* temp_RRT);

void ReadPage_WithNoDataOut(void);
void HAL_ReadPage_WithNoDataOut(T_FDL_FTCTRL *pCtrl);
void FA_CE_Reduction(void);
void HAL_MCUSendCER_CE(U8 bChan,U8 bChip, U32 dRow);
void FA_HighTempSocketInit(void);
void FA_HighTempSocketReadTemp(void);
void FA_SocketFanSwitch(void);
void FA_SocketHeaterSwitch(void);
void FA_SocketSetTemperature(void);
void FA_OpenSocketFan(void);
void FA_CloseSocketFan(void);
void FA_OpenSocketHeater(void);
void FA_CloseSocketHeater(void);
void FA_TMPSensorSetLevel(U8 SensorID, U8 LineType, U8 Level);
void FA_TMPSensorSetSDAMode(U8 SensorID, U8 bMode);
void FA_TMPSensorStart(U8 SensorID);
void FA_TMPSensorStop(U8 SensorID);
U8 FA_TMPSensorReadSDA(U8 SensorID);
U8 FA_TMPSensorWriteByte(U8 SensorID, U8 bData);
U8 FA_TMPSensorReadByte(U8 SensorID, U8 NAck);
U16 FA_TMPSensorGetTemp(U8 SensorID);
void FA_HeaterControl(void);
U8 FA_HeaterOperationCalculate(U8 Option);
void PIDUpdateCurrentTemperature(void);
U8 IsHigherTemperature(U16 Temp1,U16 Temp2);
void Get_Page_Retry_Error(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, U32 dErrTempAdd);
void SetOffsetForFindOV(U8 bChan, U8 bChip, U8 *offsets);
void ShortSeek(U8 bChan, U8 bChip, U32 dBlkAddr, U32 dPageAddr, signed char step, U32 *ret, U32 oriErr, U8 *offsets, U8 offsetIdx);
void FA_PE_offline();
void FA_ReadDisturb_offline();
void FA_OfflineTaskInit();
void FA_Offline_WriteBlock_ver1( 		U8 DeckSelect, U16* BlockList);
void FA_OfflineTaskInterrupt();
void FA_GPIOCTRL();
U8 FA_GPIOCTRL_FUNC(U8 GPIO_NUM, U8 FUNCCTRL);
U8 FA_GPIOCTRL_PS(U8 GPIO_NUM, U8 PSCTRL);
U8 FA_GPIOCTRL_PE(U8 GPIO_NUM, U8 PECTRL);
U8 FA_GPIOCTRL_MODE(U8 GPIO_NUM, U8 MODECTRL);
U8 FA_GPIOCTRL_DATA(U8 GPIO_NUM, U8 DATACTRL);


//void Read_DMAData(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor);
//U8 FA_SoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor);
//void Read_SoftDecodeData(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bSBNum);
//void Fdl_SoftDecodeToLDPC(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bSBNum);

void ReadPage_WithNoDataOut(void);
void HAL_ReadPage_WithNoDataOut(T_FDL_FTCTRL *pCtrl);
//void FA_CE_Reduction(void);
//void HAL_MCUSendCER_CE(U8 bChan,U8 bChip, U32 dRow);




#endif
