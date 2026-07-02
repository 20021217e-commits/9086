
#ifndef     __FLASH_DRIVER_STRUCT_H__    
#define     __FLASH_DRIVER_STRUCT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "data_type.h"
#include "hal.h"
#include "flash_driver_config.h"


#ifndef MAX_CHAN_OF_DEV
#define MAX_CHAN_OF_DEV     (4)
#endif

#ifndef MAX_CHIP_OF_CHAN
#define MAX_CHIP_OF_CHAN    (4)
#endif

#ifndef MAX_LUN_OF_CHIP
#define MAX_LUN_OF_CHIP     (4)
#endif

#ifndef MAX_LUN_OF_CHAN
#define MAX_LUN_OF_CHAN     (16)
#endif

#define OUT_OF_MEMORY_ADDR	(0x5F3000)

#define OP_SUCCESS  (0)
#define OP_FAIL     (1)

//联动读使用tFtlTask.bMaskInfo的情况,只有Fisrt和End有可能不为0,Mid为0
#define NCQ_READ_MASK_FISRT   (0)
#define NCQ_READ_MASK_MID	  (2)//有CQ分配时(U16 *)取值,地址也必须是两Byte对齐
#define NCQ_READ_MASK_END	  (7)//4K算法,因为有可能取到4Byte的Mask信息
#define NCQ_READ_MASK_CLST	  (8)


typedef enum __packed
{
    DISCRETE_NONE = 0,
    DISCRETE_CLST,
    DISCRETE_PAGE
}DISCRETE_BUFF_TYPE;


/************************************exception************************************/
#if 1
typedef enum __packed
{
	RD_ERR_VDT 			= 0, 	//0x01-low voltage happen
	RD_ERR_REREAD_PASS 	= 1, 	//0x02-重读
	RD_ERR_OPTION_PASS 	= 2, 	//0x04-option	
	RD_ERR_ARC_PASS 	= 3,	//0x08-arc
	RD_ERR_SOFT_PASS 	= 4,	//0x10-软解			
	RD_ERR_ALL_ZERO 	= 5,	//0x20-data is all 0x00
	RD_ERR_ALL_ONE 		= 6, 	//0x40-data is all 0xff		
	RD_ERR_DATA_ERROR 	= 7,	//0x80-include retry fail,soft decode fail 
	
}READ_ERROR_TYPE;//每个簇的错误类型
#endif

typedef enum __packed
{
    WR_ERR_VDT = 0,    
    WR_PROG_FAIL,
	WR_DATA_OVERLAP,
}WRITE_ERROR_TYPE;


typedef enum __packed
{
    ERS_ERR_VDT = 0,   
    ERS_FAIL,
}ERASE_ERROR_TYPE;

typedef struct _T_READ_EXCEPTION
{
    U8 bDescript;   //0 no data error no overflow,1 no data error but overflow,2 data error
    U8 ArrRev[1];
    U16 wLen;       //length of exception information
}T_READ_EXCEPTION;

typedef struct _T_WRITE_EXCEPTION
{
    U08 bDescript;   	//0 program success，1 no program fail, VDT Fail  2 program fail no VDT Fail,3 program fail,VDT Fail
    U08 bOverlapCnt;    // 0 prgoram data have not overlapped; 1 overlapped
    U16 wExLen;     	//length of exception information
    U32 dHostDateLen;	//length of data which is in dma buffer
//    U16 wRev;
}T_WRITE_EXCEPTION;


typedef struct _T_ERASE_EXCEPTION
{
    U08 bDescript;   //0 erase success,1 VDT fail no erase fail, 2 erase fail no VDT fail, 3 erase fail VDT fail
    U08 ArrRev[1];
    U16 wLen;       //length of exception information
}T_ERASE_EXCEPTION;

typedef struct _T_DMA1_EXCEPTION
{
    U08 bTransErr;   //0 transfer success，1 transfer fail
    U08 ArrRev[1];
    U16 wLen;       //length of exception information
}T_DMA1_EXCEPTION;

typedef struct _T_EXCEPTION_HEADER
{
    U08 bDescript;
    U08 ArrRev[1];
    U16 wLen;
}T_EXCEPTION_HEADER;

typedef struct _T_HOST_DATA_INFO
{
    U16	wPageIndex;     //page index of data which is in dma buffer
    U08 wBuffSts[4];    //buffer status of page index
    U08 ArrRev[2];
}T_HOST_DATA_INFO;

typedef struct _T_ERR_INFO
{
    U16 wIndex;         //index of parameter which has occur error
    U16 ErrType;        //error type
}T_ERR_INFO;


/************************************特殊操作参数表格************************************/       
// 在启动DMA读数据前，是否要特殊的Flash命令操作
typedef struct                                            
{                                                         
    U08  bPlnSelCmdFlg;// 是否有“多Plane读数据前的Plane选中命令”标记
    U08  bRandRdCmdFlg;// 是否有“读数据”前的命令序列操作标记
    U08  bRandRdMode;  // 0：为2个字节地址的随机读。1：为5个字节地址的随机读。
    U08  bReserve;
}T_PREV_RD_DATA;   

typedef struct                                            
{                                                         
    U08  bRandWrCmdFlg;// 是否有“写数据”前的命令序列操作标记
    U08  bRandWrMode;  // 0：为2个字节地址的随机写。1：为5个字节地址的随机写。
    U08  bReserve[2];                                     
}T_PREV_WR_DATA;  

typedef struct                                            
{                                                         
    U08  bCmdFlg;        // 是否有“Reset LUN”操作标记
    U08  bReserve[3];                                       
    U32  dCmdCQ;         // ResetLUN的命令CQ。由量产工具配置
}T_RST_LUN;

typedef struct                                            
{                                                         
    U08  bEnable;    // 是否支持MultiPlane操作标记。
    U08  bType;      // 该字段标志多Plane读操作的命令类型。
    				 // 0表示三星、东芝Flash类的。
    				 // 1表示美光Flash类的。
    U08  bRev[2];
    U08  bPln0WRCmd; // 
    U08  bPln0RDCmd; // 多Plane读命令的Plane0命令码
    U08  bPln1WRCmd; // 多Plane写命令的Plane1命令码，默认值为81h
    U08  bPln1RDCmd; // 多Plane读命令的Plane1命令码
}T_MUL_PLN_PARAM; 

typedef struct                                            
{                                                         
    U08  bMultiLunEn; // 是否支持MultiLun操作标记。
    U08  bMultiCeEn;  // 是否支持MultiCe操作标记。
    U08  bMultiClstEn;// 是否支持多簇合并操作标记。
    U08  bReserve[1];
}T_INTERLEAVE_OP; 

typedef struct
{
    U08  bWriteEn; //是否支持Cache写操作标记
    U08  bReadEn;  //是否支持Cache读操作标记
    U08  bSnapReadEn;//是否开启snap read标记，有些颗粒snap read 会加重read disturb问题
    U08  bReserve[1];
}T_CACHE_PARAM;

#define  PREV_RD_CMD_CQ_MAX      (1)
typedef struct                                            
{                                                         
    U08  bCmdFlg;      //SLC模式下，是否有“特定读操作前缀命令序列”操作标记
    U08  bCmdTlcFlg;   //TLC模式下，是否有“特定读操作前缀命令序列”操作标记
    U08  bCmdCQTlcMode;//决定TLC模式前缀是否由dCmdCQ_Tlc发，0:不是，1:是。如ED3 Flash，由Row控制
    U08  bReserve[1];                                        
    U32  dCmdCQ[PREV_RD_CMD_CQ_MAX];   //SLC读前缀命令CQ。由量产工具配置。比如OBP的Flash，配置为0xDA命令;ED3 Flash,配置为0xA2命令
    U32  dCmdTlcCQ[PREV_RD_CMD_CQ_MAX];//TLC读前缀命令CQ。由量产工具配置。比如OBP的Flash，配置为0xDF命令;ED3的Flash,配置为0x01/0x02/0x03命令,由Row(U32)地址最高Byte决定。
}T_PREV_RD_CMD;  

#define  PREV_WR_CMD_CQ_MAX      (1)
typedef struct                                            
{                                                         
    U08  bCmdFlg; // 是否有“特定写操作前缀命令序列”操作标记
    U08  bReserve[3];                                        
    U32  dCmdCQ[PREV_WR_CMD_CQ_MAX]; // 写前缀命令CQ。由量产工具配置
}T_PREV_WR_CMD;

#define  PREV_ERS_CMD_CQ_MAX     (1)
typedef struct                                            
{                                                         
    U08  bCmdFlg; // 是否有“特定擦除操作前缀命令序列”操作标记
    U08  bReserve[3];                                        
    U32  dCmdCQ[PREV_ERS_CMD_CQ_MAX]; // 擦除前缀命令CQ。由量产工具配置
}T_PREV_ERS_CMD;

#define  INIT_MODE_CMD_CQ_MAX    (1)
typedef struct                                            
{                                                         
    U08  bCmdFlg;        // 是否有“Flash工作命令切换”操作序列标记,所有前缀命令的前提条件  mlc flash 无，tlc flash 有 
    U08  bWorkMode;       // 用于决定是否把MLC/TLC Flash只当SLC用。0x0:SLC   0x1:MLC/TLC
    U08  bFlashIOMode;    // 0x00: Asycn/Onfi SDR  0x01 Toggle/Onfi-ddr2
    U08  bRev;//bFlgCheckFunc;   // 初始化时是否需要flag check,B16A需要
    U32  dCmdCQ[INIT_MODE_CMD_CQ_MAX]; // 模式切换命令CQ。由量产工具配置
}T_INIT_MODE;

typedef struct  
{
    T_PREV_RD_DATA  tPrevRdData;  // 基本读数据前缀：Plane选中、随机读
    T_PREV_WR_DATA  tPrevWrData;  // 基本写数据前缀：随机写0x85
    T_RST_LUN       tResetLun;    // 多LUN复位命令
    T_MUL_PLN_PARAM tMultiPln;    // 多Plane操作
    T_INTERLEAVE_OP tInterlevEn;  // 包括多LUN/多CE/多簇合并操作使能
    T_CACHE_PARAM   tCache;       // Cache操作
    T_PREV_RD_CMD   tPrevRdCmd;   // 基本读命令前缀操作  SLC/TLC
    T_PREV_WR_CMD   tPrevWrCmd;   // 基本写命令前缀操作  SLC/TLC
    T_PREV_ERS_CMD  tPrevErsCmd;  // 基本擦除命令前缀操作  SLC/TLC
    T_INIT_MODE     tInitMode;    // 初始化运行模式命令
}T_FLASH_OPER_PARAM; 

typedef   struct
{
    U08 m_bChanNum;                    //设备包含通道的总个数
    U08 m_bChipOfChan;                 //一个通道包含的物理片选数
    U08 m_bLunOfChip;                  //每Chip包含的Lun数
    U08 m_bLunOfChan;                  //一个通道包含的lun数
    U08 m_bLunOfDev;                   //设备包含总的lun数
    U08 m_bPlaneOfLun;                 //每个lun包含plane个数
    U16 m_wPageOfBlk;                  //每个block有多少个可用物理page(TLC)

    U16 m_wRowOfBlk;                   //Block对应的行地址大小，主要用于擦除
    U16 m_wRowGapOfBlkInPlane;         //一个plane内，相邻两个blk的行地址间隔
    U32 m_dRowGapOfPlane;              //每个palne间，的行地址间隔
    
    U32 m_dRowGapOf2LUN;               //两个lun之间行地址的间隔
    U16 m_wBlkOfLUN;                   //每个lun包含的block个数
    U08 m_bDataSectOfPage;             //1个page有多少个扇区
    U08 m_bTabSectOfPage;			   //tab1个page有多少个扇区
 
    U08 m_bSectOfDataClst;             //每个数据簇包含的扇区个数
    U08 m_bSectOfTableClst;            //每个表格簇包含的扇区个数
    U08 m_bDataClstOfPage;             //每个页包含的数据簇个数
    U08 m_bTableClstOfPage;            //每个页包含的表格簇个数
    U08 m_bDataSectOfDMA;              //1个DMA有多少个扇区
	U08 m_bTabSectOfDMA;			   //tab1个DMA有多少个扇区
    U08 m_bDMAOfDataClst;			   //每个数据簇包含的DMA个数
    U08 m_bDMAOfTableClst;			   //每个表格簇包含的DMA个数
    U08 m_bRtyMode[4];
    U08 bRev0[4];

    U08 m_bDataUserDataLen;             //数据区userdata的长度
    U08 m_bTabUserDataLen;              //表格区userdata的长度
    U08 m_bMskByteOfClst;               //每个簇所需的Maskbyte数
    U08 m_bBlockGapInPlane;
    U08 m_bCellType;                    //1： SLC, 2:MLC, 3:TLC
	U08 m_bFlashDriver;					//增加控制Flash本身驱动能力参数
    U16 m_wPageSize;

    U16 m_wDataColAddr[32];             //32个DMA
    U16 m_wTabColAddr[32];              //32个DMA
    U16 m_wPageOfBlkSLC;                //每个block有多少个可用物理page(SLC)
    U16 m_dRetrySvalue;
    U08 m_bChMapOfDev[MAX_CHAN_OF_DEV];              				//通道的mapping,4个通道×4ce
    U08 m_bChipMapOfCh[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];          //片选Mapping

	U08 m_bPhyChipOfLogLun[MAX_CHAN_OF_DEV][MAX_LUN_OF_CHAN];		//FTL的log lun到通道内物理片选
	U08 m_bPhyLunOfLogLun[MAX_CHAN_OF_DEV][MAX_LUN_OF_CHAN]; 		//FTL的log lun到通道内物理片选内物理Lun  
}T_FLASH_PARAM;


typedef struct
{
	U32  GoCache_L2pBaseAddr[2];		//Zone0/X L2p Map ,FDL don't care
	U32	 GoCache_PhyBaseAddr[2];		//Zone0/X DRAM Base, FDL don't care

	U32  GoCache_DevCapacity;           //容量大小
	U16  GoCache_MaxBurstLen[9];        //不同频率下Burst模式最长DMA长度
	U08  GoCache_DevType;               //设备种类
	U08  GoCache_DevVoltage;            //设备电压类型

	U08  GoCache_FuncEn[2];             //功能使能
	U08	 GoCache_Chan[2];
	U08  GoCache_ChipNum[2];
	U08	 bReverse[2];
}T_GOCACHE_PARAM;

typedef struct
{
	U8 bPhyParamEn;
	U8 m_bCRCMode;             //CRC 模式(0:关闭,2:CRC16,4:CRC32)
	U8 m_bCQDeepthErase;         //擦除CQ FIFO深度
	U8 m_bCQDeepthWrite;         //非联动写CQ FIFO深度
	U8 m_bCQDeepthRead;         //非联动读CQ FIFO深度
	U8 m_bCQDeepthLittleData;       //小数据CQ FIFO深度
	U8 m_bCQDeepthNCQWrite;       //联动写CQ FIFO深度
	U8 m_bCQDeepthNCQRead;         //联动读CQ FIFO深度
}T_PHY_PARAM;

typedef struct {
	U32 dGocacheAddr;
	U32 dGocacheLen;
	U8  bPortOrBurst;
	U8  bGocacheEn;
	U8  bFlag;
	U8  bJiaocha;
	U8  bTongshi;
	U8  bReserve[3];
}T_GOCACHE_PARM;    //for gocache test

T_GOCACHE_PARM *ptGocacheParm;

typedef struct
{
	U32  GoCache_L2pBaseAddr[2];		//Zone0/X L2p Map ,FDL don't care

}T_BOOT_PARAM;



typedef struct _T_UDP_FDL
{
    T_REG_CFG           m_tHwReg;
    T_FLASH_PARAM       m_tFlashParam;
    T_FLASH_OPER_PARAM  m_tFlashOperParam;
    T_GOCACHE_PARAM		m_tGoCacheParam;
	T_PHY_PARAM			m_tPhyParam;
}T_UDP_FDL;

typedef struct _T_READ_FAIL_CTRL
{
	U08 m_Fail_RtyEn;       //正常数据读发生Fail 是否Retry
	U08 m_Fail_StpEn;       //两层含义：（1）读Fail，Retry失败之后是否Stop Task；（2）正常数据读Fail，不进行Retry，是否Stop Task；
	U08 m_Fail_ReRdCnt;      //全0xFF数据不进行Retry时，进行重读的次数（不需要重读时设置为0）
	U08 m_bRev;
}T_READ_FAIL_CTRL;

typedef struct _T_WRITE_FAIL_CTRL
{
	U08 m_Fail_StpEn;       //写Fail时，是否Stop      Task
	U08 bRrev[3];
}T_WRITE_FAIL_CTRL;

typedef struct _T_VDT_EXCEPT_CTRL
{
	U08 m_VDTStatus;       //VDT的状态
	U08 bRrev[3];
}T_VDT_EXCEPT_CTRL;

//此结构体需要放在UDP中，初始化时调用FDL_Init将此结构体指针传递进来
typedef struct _T_FDL_PARAM
{
    T_UDP_FDL *ptUdpFdl;    //UDP中的固定参数指针
    /***********Buffer地址和大小设置**********************/
    U32 dEccInfoBuffAddr;       //ECC存放地址
    U32 dEccInfoBuffSize;       
    /***********特殊数据配置**********************/
	U08 bUnValidDataUser[16];   //无效UserData设置
	U08 bUnValidTableUser[16];   //无效UserData设置
	U16 wContiAddrMaxRead;  //必须是2的整数次幂，FDL会用 &(wContiAddrMaxRead-1)来减少%运算
	U16 wContiAddrMaxWrite; //必须是2的整数次幂，FDL会用 &(wContiAddrMaxRead-1)来减少%运算
    /***********异常控制配置**********************/
	T_WRITE_FAIL_CTRL tWriteFailCtrl;   //写异常时处理流程的控制参数
	T_READ_FAIL_CTRL tReadFailCtrl;	   //读异常时处理流程的控制参数
	T_VDT_EXCEPT_CTRL tVDTExceptCtrl;	//VDT异常处理流程的控制参数	
#ifdef __RETRY_TABLE_RATE__
	U16 *pwRRTUseCnt;
	U16 wRRTUseCntBuffSize;
	U16 wRev;
#endif
	void (*FTL_ExceptionWriteFailHandle)(U08 bMode,U32 dAddr);//回调函数，FTL注册，在写异常发生后，FDL调用此函数，传给FTL异常偏移	
	void (*FTL_ExceptionVdtHandle)(U08 bMode);//回调函数，FTL注册
	void (*FTL_CollectRetryInfor)(U08 bChan,U08 bLun,U08 bRtyType,U32 dAddr);//回调函数，FTL注册
	U32 (*FTL_GetBadMapAddr)(U32 dRowAddr,U08 bLogChan,U08 bLunOfChan);
}T_FDL_PARAM;

#ifdef __BADBLK_BINDMAP__
	typedef struct
	{
		//U32 dTitle;       //读flash的标记
		U16 wBindBlkLen;     //保留先
		U16 wBindBlk;     //邦定的块号，
		U32 wBindBlkBasePhyAddr; //邦定块的基础地址，wBondBlk*邦定的间隔
		U16 wBindBlkAddr[4][4];  //4Chan //8CE //
		U16 wBuff[4*4*4*40+16];      //4Chan //4CE //4个Plane的地址
		U08 bRev[8];
		U32 dCheckSum;
	}T_FDL_BADBLK_BINDMAP_DATA;
#endif



typedef enum __packed
{
    READ_FAIL_ALL_RETRY = 0,    //= 0x00,默认配置，发生ECC FAIL/ALL ONE/ALL ZERO/DDR TIMEOUT都进行Retry
    READ_FAIL_ALL_NO_RETRY,     //= 0x01,发生异常情况不进行Retry,不会保存异常信息，此时Exception Info不可信
    READ_FAIL_STOP,             //= 0x02,发生异常情况停止DMA2，未实现
    READ_ALL_ONE_NO_RETRY,      //= 0x03 如果数据为全0xFF，不进行Retry，上报FTL的Exception Info可信
    READ_ALL_ONE_ONLY_RE_RD,     //= 0x04 如果数据为全0xFF，直接重读，不配置Flash寄存器
#ifdef RETRY_SVALUE
    READ_VERIFY_TLC_PAGE,     //读校验要对Retry结果进行特殊处理
#endif
}READ_FAIL_CTRL;

typedef enum __packed
{
    WRITE_FAIL_STOP = 0,        //= 0x00,默认配置，发生编程Fail即数据传输,强制TaskDone,后续由FTL处理(联动写有效)
    WRITE_FAIL_NO_STOP          //= 0x01,发生编程Fail不停止数据传输，走完写流程
}WRITE_FAIL_CTRL;

typedef struct
{
	U32 dReadExBuffAddr;   //读异常Buffer地址
	U16 wReadExBuffSize;   //读异常Buffer大小，以sector为单位
	U08 bReadFailCtrl;	   //读异常时处理流程的控制参数
	U08 bRrev[1];			   //保留字节
	
	U32 dWriteExBuffAddr;  //写异常Buffer地址
	U16 wWriteExBuffSize;  //写异常Buffer大小，以sector为单位
	U08 bWriteFailCtrl;    //写异常时处理流程的控制参数
	U08 bWrev[1];          //写异常时处理流程的扩展控制参数
	
	U32 dEraseExBuffAddr;  //擦异常Buffer地址
	U16 wEraseExBuffSize;  //擦异常Buffer大小，以sector为单位
	U08 bErev[2];		   //擦异常时处理流程的控制参数

	U08 bContinueTask;     //1:后面会用continueTask添加元素， 0:后面
	U08 bReserve[3]; 

#ifdef __RAID_FUNCTION_ON__
	U32 dEvenRaidAddr; //设置联动写偶数页Raid地址
	U32 dOddRaidAddr; //设置联动写奇数页Raid地址
#endif
}FDL_TASK_CTRL;


typedef enum __packed
{
    TYPE_LOGIC = 0,     //操作的是逻辑数据（或者说为普通MLC/TLC页模式）
    TYPE_TABLE,         //操作的是表格数据（或者说为SLC页模式）
    TYPE_VMC            //操作的是vmc数据（关键数据写在SLC页上）
}DATA_TYPE;

typedef enum __packed
{
    SLC_MODE = 0,
    MLC_MODE,
    TLC_MODE
}FLASH_MODE;

typedef enum __packed
{
    ST_DOING = 0,
    ST_CQ_CFG_DONE,
    ST_DMA1_READ_DONE,
    ST_TASK_DONE,
    ST_TASK_ERROR_DONE
}FDL_TASK_STATUS;

typedef enum
{
    HOST_WR = 0,
    HOST_RD = 1,
}DATA_DIR;

typedef enum __packed
{
    HOST_DATA = 0,
    FLASH_DATA
}ORDER_PRG_DATA_SRC;

typedef  struct
{   
    U08  m_bDataType;   // 数据类型
    U08  m_bChan;       // 通道编号
    U08  m_bLunOfChan;  // 通道中的Lun编号
    U08  m_bClstOffs;   // 页中簇偏移
    U32  m_dRowAddr;    // Lun中的行地址
} T_FLASH_PHY_ADDR; 

typedef  struct/*用于TLC Page数据校验时,地址数据的结构*/
{   
    U08  bChan;      // Channel index
    U08  bLunOfChan; // LUN index of Channel
    U16  wBlkStart;   // block index of LUN
    U16  wPageOffSet;
    U16  wPageCnt;/*注意,page不要溢出(0x3FFF)：4Plane:最多4096page,2Plane:最多8192Page*/
} T_BLK_INFO_PER_LUN;

typedef  struct
{   
    U08  m_bChan;       // 通道编号
    U08  m_bLunOfChan;  // 通道中Lun编号
    U16  m_wBlkAddr;    // Lun中的块地址
} T_FLASH_PHY_BLK_ADDR; 

typedef  struct
{   
    U08  bChan;         // 通道编号
    U08  bLunOfChan;    // 通道中的Lun编号
    U16  wClstLen;      // 操作的簇个数
    U32  *pdRowAddr;    // 行地址指针
    U08  *pbClstOffs;   // 簇偏移指针
    U32  dUdBuffAddr;  // User Data地址指针
}T_SRC_INFO_OF_LUN; 

typedef  struct
{   
    U08  bChan;         // 通道编号
    U08  bLunOfChan;    // 通道中的Lun编号
    U16  wPageCnt;      // 操作的页个数
    U32  *pdRowAddr;    // 行地址指针（LUN）
}T_DST_INFO_OF_LUN; 

typedef  struct
{   
    U08  bChan;        // Channel index
    U08  bLunOfChan;   // LUN index of Channel  源页的LUN信息与目的页的通道信息一致
    U16  wClstLen;   //总共的簇地址个数....注意，当在进行Flash TLC Copyback编程时，源数据页的个数，必须比实际要编程的WL末页要多6个页的簇个数。
    U32  *pdRowAddr;     //row addr of LUN == blknum| wordline num
    U08  *pbClstOffs;    // cluster offset in page
    U32  *pdUdBuffAddr;
}T_IN_COPY_SRC_INFO_OF_LUN; 

typedef struct
{
	U08 bChan;
	U08 bLunOfChan;
	U08 bPrgCycle; //program cycle
	U08 bOpPageCnt;
	U32 dRowAddr[3]; //row addr of LUN //注意，在__ONE_BLK_PROG_MODE__中，最高Byte为Wordline内的页偏移.. 00-> SLC module, 01/02/03--TLC module 下WL内的3个页分区 4K FTL 的第 4 页module, 01/02/03--TLC module 下WL内的3个页
} T_FLASH_PROG_ORDER_UNIT;

typedef  struct
{   
    U08  bChan;         
    U08  bLunOfChan;
    U16  wBlkNum;
    U16  wOrderNum;        //m_bReserved[2];
    U08  bPrgCycle;        //program cycle
    U08  bOpPageCnt; 
    U32  dRowAddr[3];    //row addr of LUN          //注意，在__ONE_BLK_PROG_MODE__中，最高Byte为Wordline内的页偏移.. 00-> SLC module, 01/02/03--TLC module 下WL内的3个页
    U08  bSrcType[3];    //每个页的数据源。  0--Host发送数据。  1--Flash数据。该Flash的数据页与当前操作页一致。也就是将本页数据先读取出来，再回写回去。
    U08  bMode;          //操作模式。。0--默认模式为正常写入模式。即所有页的数据源都为一种方式。Host或者在Buffer上。
                         //而其它非零值，表明每个操作页的数据源不同。
} T_FLASH_PROG_ORDER_EXTEND_UNIT; 
	
typedef struct
{
	U08  bReserved;		//	保留字段
	U08  bHeadUnitCnt;	//	开头部分操作的unit个数
	U16  wMidUnitCnt;	//	中间部分操作的unit个数
	U08  bLastUnitCnt;	//	最后部分操作的unit个数
	U08  bHeadUnitOfMultiPlane;//	表示开始部分unit操作所采用的multiplane模式
	U08  bMidUnitOfMultiPlane;//	表示中间部分unit操作所采用的multiplane模式
	U08  bLastUnitOfMultiPlane;//	表示最后部分unit操作所采用的multiplane模式
} T_PLANE_PARAM;

typedef struct
{
	U08  bPrgCycle;
	U08  bOpPageCnt;
	U16  wOrderNum;
	U32  dRowAddr[3]; 
	U32  dUserDataAddr[3];
} T_PROG_ORDER_UNIT_IN_BLK;

typedef struct
{
	U08 bChan;
	U08 bLunOfChan;
	U16 wOrderUnitCnt; 						//Order Unit的总个数;
	T_PROG_ORDER_UNIT_IN_BLK *ptOrderUnit;  //program order unit数组
}T_FLASH_ORDER_PROG_INFO_OF_LUN; //

typedef  struct
{   
	U32  dRow;
	U16  wCol;
    U08  bChan;        
    U08  bChip;          
    U08  bLun; 
	U08  bUdSize;    //table and data diff
	U08  bSecOfClst; //table and data diff 
	U08	 bClstOfPage;//table and data diff 
}T_ADDR_INFO; 

#ifdef __SLC_PRG_FAIL_HANDL__
typedef  struct
{   
    U16 wCurFailOffset;
	U16 wPreFailOffset;
	U08 bCurPageOverlap;
    U08 bPrePageOverlap;
	U08 bCurrentPageFail;
	U08 bPreviouPageFail;
} T_PROG_FAIL_INFO; 
#endif


#ifdef __FILL_PAGE_EN__


/*跳列配置信息*/
typedef struct											  
{	
	union
	{
		U32 dAll;
		struct
		{
			U32 dJCBaseAddr0_14:15; /*跳列信息基地址*/
			U32 dJCByteCnt15_24:10; /*当前DMA一共跳列的Byte数(不能超过0x3FF)*/
			U32 dReserved025_31:7;	/*保留*/
		}bits;
	}ALL_JC_CFG_INFO;
	
}T_DMA2_JC_CFG_INFO;

/*跳列信息*/
typedef struct											  
{														  
	union
	{
		U16 wAll;
		struct
		{
			U16 wJCStartColumn0_11:12;	/*跳列起始位置DMA2的列地址*/
			U16 wJCByteCntCur12_15:4;	/*当前次跳列Byte数(1-E:1-14Byte;0:32Byte;1:64Byte)*/
		}bits;
	}DMA2_JC_INFO;
}T_DMA2_JC_INFO;


typedef struct
{
	U16 wJCTotalByteCnt;	/*跳列的Byte数(需保证为偶数)*/
	U16 wJCStartColumn; 	/*跳列起始位置(需保证为偶数)*/
}T_JC_DMA_GROUP_INFO;		/*bJCDMAGroupInfoSize = 4Byte!!!*/


typedef struct											  
{	
	struct
	{
		U16 wJCGroupCnt;			/*当前DMA跳列组数*/
		U16 wColumnOffsetOfDMA; 	/*记录每个DMA的列地址偏移*/
		T_JC_DMA_GROUP_INFO *tJCDMAGroupInfo;
	}tJCPageGroupInfo[8];			/*每个Page 8个DMA*/

	U16 wJCTotalByteOfPage; 		/*一个页总的跳列Byte数*/
	U16 wRev;			/*一个页总的跳列Byte数*/
	
}T_DMA2_JC_ORGINAL_INFO;





typedef struct											  
{	
	struct
	{
		/*UDP物理参数(Table和Data不一样,在这里统一初始化)*/
		U08 bClstDmaCnt;
		U08 bPageClustCnt;
		U08 bUseDataLen;
		U08 bDMASectors;
	//	U16 *wpColAddr;
		U08 bJCOrginalInfoCnt;/*准备生成多少组跳列源信息(以Page为单位--一个Page内的所有DMA跳列信息都确定后算一组有效的信息)*/
		U08 bRev[3];
		
	}tJCUDPParam;

	T_DMA2_JC_ORGINAL_INFO	*tDma2JCOrginalInfo;/*跳列源信息结构*/
	
}T_DMA2_JC_ORGINAL_INFO_TOTAL;


typedef struct											  
{		 
	T_DMA2_JC_CFG_INFO *tDMA2CfgInfo;
	U32 dJCCfgInfoCnt;					/*当前Buff记录跳列配置信息的个数*/
	U32 dCfgInfoSize;					/*跳列配置信息BuffSize(4Byte)*/

	T_DMA2_JC_INFO *tDMA2JCInfo;
	U32 dJCInfoCnt; 					/*当前Buff记录跳列区间信息的个数*/

	T_DMA2_JC_ORGINAL_INFO_TOTAL	*tDma2JCOrginalInfoTotal;
	
}T_DMA2_JC_TABLE;

#else
#endif



#ifdef __cplusplus
}
#endif

#endif






