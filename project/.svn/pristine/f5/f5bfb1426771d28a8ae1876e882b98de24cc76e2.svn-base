
#ifndef     __FDL_TASK_OP_H__    
#define     __FDL_TASK_OP_H__

#include "flash_driver.h"
#include "flash_driver_struct.h"
#include "flash_driver_config.h"
#include "fdl_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

	#define FLASH_DRIVER				(pFdlParam.ptUdpFdl->m_tFlashParam.m_bFlashDriver)//增加Flash驱动能力设置
	#define	BIT_OF_BYTE					8
    #define BYTE_OF_SEC                 512
    #define CHAN_MAX_CNT                (pFdlParam.ptUdpFdl->m_tFlashParam.m_bChanNum)
    #define LUN_OF_CHIP                 (pFdlParam.ptUdpFdl->m_tFlashParam.m_bLunOfChip)
    #define LUN_OF_CHAN                 (pFdlParam.ptUdpFdl->m_tFlashParam.m_bLunOfChan)
    #define LUN_OF_DEV                  (pFdlParam.ptUdpFdl->m_tFlashParam.m_bLunOfDev)
    #define CHIP_OF_CHAN                (pFdlParam.ptUdpFdl->m_tFlashParam.m_bChipOfChan)

	#define SECT_OF_DMA					(pFdlParam.ptUdpFdl->m_tFlashParam.m_bDataSectOfDMA)
	#define SECT_OF_PAGE				(pFdlParam.ptUdpFdl->m_tFlashParam.m_bDataSectOfPage)

    #define DSECT_OF_DMA                (pFdlParam.ptUdpFdl->m_tFlashParam.m_bDataSectOfDMA)
    #define DSECT_OF_PAGE               (pFdlParam.ptUdpFdl->m_tFlashParam.m_bDataSectOfPage)
	#define TSECT_OF_DMA 				(pFdlParam.ptUdpFdl->m_tFlashParam.m_bTabSectOfDMA)
	#define TSECT_OF_PAGE				(pFdlParam.ptUdpFdl->m_tFlashParam.m_bTabSectOfPage)
	
    #define PAGE_SIZE                   (pFdlParam.ptUdpFdl->m_tFlashParam.m_wPageSize)
    #define PAGE_OF_BLOCK               (pFdlParam.ptUdpFdl->m_tFlashParam.m_wPageOfBlk)
    #define PAGE_OF_BLOCK_SLC           (pFdlParam.ptUdpFdl->m_tFlashParam.m_wPageOfBlkSLC)
    #define BLOCK_OF_LUN                (pFdlParam.ptUdpFdl->m_tFlashParam.m_wBlkOfLUN)
    #define PLANE_OF_LUN                (pFdlParam.ptUdpFdl->m_tFlashParam.m_bPlaneOfLun)

    #define ROW_OF_BLOCK                (pFdlParam.ptUdpFdl->m_tFlashParam.m_wRowOfBlk)
    #define ROW_GAP_OF_BLOCK            (pFdlParam.ptUdpFdl->m_tFlashParam.m_wRowGapOfBlkInPlane)
    #define ROW_GAP_OF_PLANE            (pFdlParam.ptUdpFdl->m_tFlashParam.m_dRowGapOfPlane)
    #define ROW_GAP_OF_LUN              (pFdlParam.ptUdpFdl->m_tFlashParam.m_dRowGapOf2LUN)

    #define SECT_OF_DATA_CLST           (pFdlParam.ptUdpFdl->m_tFlashParam.m_bSectOfDataClst)
    #define SECT_OF_TBLE_CLST           (pFdlParam.ptUdpFdl->m_tFlashParam.m_bSectOfTableClst)
    #define DATA_CLST_OF_PAGE           (pFdlParam.ptUdpFdl->m_tFlashParam.m_bDataClstOfPage)
    #define TBLE_CLST_OF_PAGE           (pFdlParam.ptUdpFdl->m_tFlashParam.m_bTableClstOfPage)

    #define DMA_OF_DATA_CLST			(pFdlParam.ptUdpFdl->m_tFlashParam.m_bDMAOfDataClst)
    #define DMA_OF_TABLE_CLST			(pFdlParam.ptUdpFdl->m_tFlashParam.m_bDMAOfTableClst)

    #define UD_LEN_OF_DATA_CLST         (pFdlParam.ptUdpFdl->m_tFlashParam.m_bDataUserDataLen)
    #define UD_LEN_OF_TBLE_CLST         (pFdlParam.ptUdpFdl->m_tFlashParam.m_bTabUserDataLen)

    #define MASK_BYTE_OF_CLST           (pFdlParam.ptUdpFdl->m_tFlashParam.m_bMskByteOfClst)
	#define CHAN_RTY_MODE(ch)			(pFdlParam.ptUdpFdl->m_tFlashParam.m_bRtyMode[ch])

    #define DATA_COL_ADDR(x)            ((U16)pFdlParam.ptUdpFdl->m_tFlashParam.m_wDataColAddr[x])
    #define TBLE_COL_ADDR(x)            ((U16)pFdlParam.ptUdpFdl->m_tFlashParam.m_wTabColAddr[x])

    #define CHANS(x)                    ((U8)pFdlParam.ptUdpFdl->m_tFlashParam.m_bChMapOfDev[x])//注意:一定要对参数x进行严格控制
    #define CHIPES(ch,x)                ((U8)pFdlParam.ptUdpFdl->m_tFlashParam.m_bChipMapOfCh[ch][x])//注意:一定要对参数ch、x进行严格控制
	#define PHYCHIP_OF_LOGLUN(ch,x)		((U8)pFdlParam.ptUdpFdl->m_tFlashParam.m_bPhyChipOfLogLun[ch][x])
	#define PHYLUN_OF_LOGLUN(ch,x)		((U8)pFdlParam.ptUdpFdl->m_tFlashParam.m_bPhyLunOfLogLun[ch][x])

	//#define PAGE_MODEL(x)               (pFdlParam.ptUdpFdl->m_tFlashParam.m_bPageModelMaskBit[x])

    #define CELL_TYPE                   (pFdlParam.ptUdpFdl->m_tFlashParam.m_bCellType)
    #define PAD_DIFF_SWITCH             (pFdlParam.ptUdpFdl->m_tHwReg.PAD_SEL_DIFF)
    #define PAD_FLASH_ODT_SWITCH        (pFdlParam.ptUdpFdl->m_tHwReg.PAD_FLASH_ODT)

    #define MULTI_PLANE_EN              (pFdlParam.ptUdpFdl->m_tFlashOperParam.tMultiPln.bEnable)
    #define MULTI_LUN_EN                (pFdlParam.ptUdpFdl->m_tFlashOperParam.tInterlevEn.bMultiLunEn)//(0)//后面改为由UDP参数控制 ，决定FW是否使用Flash的多LUN操作
    #define MULTI_CE_EN                 (pFdlParam.ptUdpFdl->m_tFlashOperParam.tInterlevEn.bMultiCeEn)//(1)//后面改为由UDP参数控制 ，决定FW是否使用Flash的多CE操作，类似interleave操作
    #define MULTI_CLST_EN               (pFdlParam.ptUdpFdl->m_tFlashOperParam.tInterlevEn.bMultiClstEn)//(1)//决定FW在读操作时判断同个LUN内地址间是否在一个页内
    #define CACHE_READ_EN               (pFdlParam.ptUdpFdl->m_tFlashOperParam.tCache.bReadEn)//是否可以Cache读操作
    #define CACHE_WR_EN                 (pFdlParam.ptUdpFdl->m_tFlashOperParam.tCache.bWriteEn)//是否支持Caceh写操作
    #define SNAP_READ_EN                (pFdlParam.ptUdpFdl->m_tFlashOperParam.tCache.bSnapReadEn)//是否支持Snap RD操作


	
	#define CQ_DEEPTH_ERASE_BLOCKS      (pFdlParam.ptUdpFdl->m_tPhyParam.m_bCQDeepthErase)//擦除CQ FIFO深度
	#define CQ_DEEPTH_WRITE_PAGES       (pFdlParam.ptUdpFdl->m_tPhyParam.m_bCQDeepthWrite) //非联动写CQ FIFO深度
	#define CQ_DEEPTH_READ_TO_BUFFER    (pFdlParam.ptUdpFdl->m_tPhyParam.m_bCQDeepthRead)//非联动读CQ FIFO深度
	#define CQ_DEEPTH_READ_LITTLE_DATA  (pFdlParam.ptUdpFdl->m_tPhyParam.m_bCQDeepthLittleData)//小数据CQ FIFO深度
	#define CQ_DEEPTH_NCQ_WRITE         (pFdlParam.ptUdpFdl->m_tPhyParam.m_bCQDeepthNCQWrite)//联动写CQ FIFO深度
	#define CQ_DEEPTH_NCQ_READ          (pFdlParam.ptUdpFdl->m_tPhyParam.m_bCQDeepthNCQRead)//联动读CQ FIFO深度
	#define CQ_DEEPTH_ORDER_PRG			(0xB0)/*不再使用*/
	#define CQ_DEEPTH_NCQ_ORDER_PRG		(0xA0)/*直接编程使用*/

    #define FIRST_HANDLE_COUNT          (8)
    #define MAX_FTL_TASK                (4)
    #define MAX_CHAN_COUNT              (4)
    #define MAX_LUN_COUNT               (4)//
    #define LUN_OP_CNT_IN_LOGNUM        (16)//在LOGNUM中记录的当前页连续操作的个数
    #define PRE_OP_CNT_IN_LOGNUM		(20)//在LOGNUM中记录的前一页连续操作的个数,用于Cache写异常
    #define NORMAL_OP_MODE              (0) //与上个地址不构成操作
    #define MULTI_PLN_OP_MODE           (1) //与上个地址构成多Plane操作
    #define MULTI_LUN_OP_MODE           (2) //与上个地址构成多LUN操作
    #define MULTI_CE_OP_MODE            (3) //与上个地址构成多CE操作
    #define MULTI_CLST_OP_MODE          (4) //构成多Clst合并操作
    #define CACEH_OP_MODE               (5) //构成Cache操作
#ifdef _TLC_ORDER_PROGRAM_
	#define SLC_OPMODE  (0)
	#define MIX_OPMODE  (1)
	#define NONE_OPMODE (2)
	#define CACHE_ORDER_WR_EN (0)
#endif
	#define PLANE_BIT_MASK_OF_ROW		((U32)(~(ROW_OF_BLOCK*(PLANE_OF_LUN - 1)))) /*Plane Mask Bit*/

	#define GET_SLC_ROW(dRow)				((dRow)&0x00FFFFFF)/*不可单独使用*/
	#define GET_PAGE_NUM(dRow) 				(GET_SLC_ROW(dRow)&(ROW_OF_BLOCK - 1))
	#define GET_BLOCK_NUM(dRow) 			(GET_SLC_ROW(dRow)/ROW_OF_BLOCK)
	#define GET_PLANE_NUM(dRow) 			((GET_SLC_ROW(dRow)&(~PLANE_BIT_MASK_OF_ROW))/ROW_OF_BLOCK)
	#define GET_TLC_FLAG(dRow) 				(dRow >> 24)
	#define GET_ROW_ADDR_SLC(wPage,wBlock)	((U32)((((U32)((wBlock)*ROW_OF_BLOCK)) + (wPage))))
#if TLC_ED3_MODE_SUPPORT
	#define GET_ROW_ADDR_TLC(wPage,wBlock,bWLNum)	(GET_ROW_ADDR_SLC(wPage,wBlock) + (((U32)bWLNum)<<24))/*兼容ED3类型Flash*/
#else
	#define GET_ROW_ADDR_TLC(wPage,wBlock,bWLNum)	(0x01000000 + GET_ROW_ADDR_SLC(wPage,wBlock))/*暂不支持ED3类型Flash*/
#endif

#ifdef __VER_TLC_PAGE__/*Verify TLC Page*/
#define VER_TLC_CH_BIT 			(28)/*LogNumExt中的Log CH位*/
#define VER_TLC_CE_BIT 			(30)/*LogNumExt中的Log CE位*/
#define VER_TLC_CE_CH_BIT 		((0x03<<VER_TLC_CE_BIT) + (0x03<<VER_TLC_CH_BIT))
#define VER_TLC_LUN_TASK_BIT	(16)/*LogNumExt中的Lun Task位*/
#define VER_TLC_CH_TASK_BIT		(0)/*LogNumExt中的CH Task位*/
#define VER_TLC_TASK_GAP		(8)/*Task头尾之间预留有一段距离,用于Retry去取Row地址(防止分配任务时被覆盖掉)*/

#define VER_TLC_GET_LOG_CE(dMulitInfo)			(((dMulitInfo)>>VER_TLC_CE_BIT)&0x03)/*获取LogNumExt中隐藏的逻辑片选号*/
#define VER_TLC_GET_LUN_TASK_OFST(dMulitInfo)	(((dMulitInfo)>>VER_TLC_LUN_TASK_BIT)&0x0FFF)/*获取LogNumExt中隐藏的Lun Task Ofset*/
#define VER_TLC_GET_CH_TASK_OFST(dMulitInfo)	((dMulitInfo)&0xFFFF)/*获取LogNumExt中隐藏的CH Task Ofset*/

#define VER_TLC_PAGE_REMAIN(bChan,bChip) 	(tChTaskQueue[bChan].tLunTask[bChip].bClstOff[0])/*该Lun还剩下的Page Cnt,内存复用*/
#define VER_TLC_ECC_INFO_OFSEX(bChan,bChip) (tChTaskQueue[bChan].tLunTask[bChip].bClstOff[1])/*该Lun需要占用DMAInfo Buff的大小,内存复用*/
#define VER_TLC_LUN_TASK_OFSET(bChan,bChip) (tChTaskQueue[bChan].tLunTask[bChip].bClstOff[2])/*记录每个Lun在T_BLK_INFO_PER_LUN Buff中的Ofset,内存复用*/

#define VER_TLC_GET_MULITINFO(bChan,wOfset) 		(tChTaskQueue[bChan].wOfstInTask[(wOfset)&(MAX_CHAN_TASK-1)])/*取dMulit Info*/
#define VER_TLC_GET_ROW_INLUN(bChan,bChip,wOfset) 	(tChTaskQueue[bChan].tLunTask[bChip].dRow[(wOfset)&(MAX_CHAN_TASK-1)])/*取LunTask中的Row*/

#define VER_TLC_DMA_CNT_PAGE 				(tFtlTask.wPageOff)/*一个Page的DMA个数,内存复用*/
#define VER_TLC_FAIL_STOP_THRES 			(tFtlTask.wCurUnit)/*记录Fail Stop的阈值,空间复用*/

#define VER_TLC_LOG_NUM_EXT(bChanPhy)			HAL_CQGetLogicNumExt(bChanPhy)/*Retry时用到的一些信息(通过扩展LogNum传输到该寄存器)*/
#define VER_TLC_GET_ROW(wPage,wBlock,bWLNum)	GET_ROW_ADDR_TLC(wPage,wBlock,bWLNum)/*已兼容ED3类型Flash*/

#endif



    enum
    {
        TASK_INIT = 0,  //任务初始化
        TASK_ADD_SAG,   //任务添加段
        TASK_IRQ        //任务中断
    };
	
    enum
    {
        MOVE_SRC = 0,  //源任务
        MOVE_DST,      //任务目标
    };

    //Task的类型
    typedef enum         //这个结构体，不能随便乱移位置
    {
		TYPE_NONE = 0,
		TYPE_ERASE_BLOCKS,			//擦除块
		TYPE_WRITE_PAGES,			 //写入数据到flash
		TYPE_READ_TO_BUFFER_4K,
		TYPE_READ_TO_BUFFER,
		TYPE_READ_LITTLE_DATA,
		TYPE_NCQ_WRITE,
#ifdef _TLC_ORDER_PROGRAM_
		TYPE_NCQ_ORDER_WRITE,
#endif
		TYPE_NCQ_READ_4K,
		TYPE_PRE_READ_4K,
		TYPE_NCQ_READ,
		TYPE_FILL_BLOCKS,
		TYPE_VERIFY_TLC_PAGE,
		//Copy Back Task
		TYPE_COPY_BACK,
		TYPE_MULTI_LUN_EXDIE_COPYBACK_PRG,
		TYPE_MULTI_LUN_INDIE_COPYBACK_PRG,
		TYPE_MULTI_LUN_EXDIE_COPYBACK_ORDER_PRG,
		TYPE_MULTI_LUN_INDIE_COPYBACK_ORDER_PRG,
		TYPE_MULTI_LUN_INNER_COPYBACK_ORDER_PRG,
	}TASK_TYPE;

 //FDL 基本接口任务分配数设置,添加新Flash时需要调整参数
#define MAX_CHAN_TASK                                          32//设计成2的整数次幂便与%运算

    //FTL的一个任务
    typedef struct _T_FTL_TASK
    {
        U08 bNeedCQCount;                                                       //配置一个任务所需的CQ个数
        TASK_TYPE dTaskType;                                                    //任务类型
		U16 wPageOff; //continue task, addr buff上最多放多少地址，超过要掉头
		volatile U32 wCur;																//指示下一个需要处理的地址的偏移
		volatile U32 wLen;																//需要处理的地址长度
		U16 wContiAddrMax; //continue task, addr buff上最多放多少地址，超过要掉头

		U32 dEccStsBuff;
		U16 wMaxClstOffOfEccSts;												/*ECC Buff 最多放多少个簇的DMAInfo(包括表格簇和数据簇)*/
		U16 wFirstErgodicCnt;
        U32 dHandleBuff;
        U32 dHandleBuffExt;
        U32 dSrcArgsAddr;                                                       //参数所在的Ram地址
        U32 dDstArgsAddr;
		U16 *dSrcSeedIndex;
		U16 *dDesSeedIndex;
		U32 dUdBuff;                                                            //user data地址
        U32 dUdBuffExt;
        U32 dDestUdBuff;                                                        //搬移操作，目的地址user data地址
        U32 dRealLogNum;                                                        //实时的逻辑编号，在中断中赋值
        U08 *pMskBuff;                                                          //Mask信息所在的地址        
        U16 *pwSeedIndex;                                                       //种子信息所在地址指针
		U16 *pwBuffIdx;
		U08 bArgsArr[52];                                                       //备用参数保存
		U08 bMaskInfo[12]; 
		U08 bMetaDataBuff[128];													//应用于FTL不关心userdata情况，FDL向此位置覆盖放置

//        U32 bRev32;              //配合上面8BYTE 对齐
		U32 AddrBuffSize;//物理地址Buff大小(Byte为单位)
        U08 bTaskIndex;                                                         //当前任务在实际任务队列中的索引
        U08 bDataType;                                                          //read data is log data or table data, 1:table 0:log
		U08 bJointMode;
		U08 bBuffClstCnt;
		U08 bLogChan;
#ifdef _TLC_ORDER_PROGRAM_
		U32 dRowAddr[3];
		U08 bWriteMode;															//记录写入时Continue为纯SLC还是MIX
		U08 bTlcNeedCQCount;                                                    //配置一个任务所需的CQ个数
        U08 bRev[2];
		U08 bLastWriteMode[4]; 													//记录上一次写为SLC还是MLC，TLC
#endif
		U08 bCurFFChan;													
		U08 bCurFFChanLun;
		U08 bRev[2];
        U08 (*FTLTaskErgodic)(U8 bTaskErgo);                                    //任务遍历函数
        U08 (*FTLTaskAllocate)(U16 wOfstInTask);  //处理任务地址的函数指针
        U08 (*FTLTaskConvert)(U8 bChan);                                        //转换成CQ的函数,返回值表示是否为任务的最后一个地址
        void (*FTLTaskExceptionHandler)(U16 wOfstInTask, U8 bEntranceType, U8 bErrType);//异常处理函数指针
        FDL_TASK_CTRL *tTaskCtrl;//指针是4个byte
    }T_FTL_TASK;

    //通道任务队列中的一个任务
    typedef struct
    {
        T_FTL_TASK *ptFtlTask;       //指向FTL任务的指针
        U16 wOfstInTask;            //当前任务在FTL任务参数的偏移
        U16 wReverse;
    }T_CHAN_TASK;


	
	typedef struct
    {
		U16 wNextOfsInTask[MAX_CHAN_TASK];    //2Plane的下个ofs
		U08 bCnt;        //记录了多少个ofs
		U08 bRev[3];
    }T_NEXT_OFSINTASK;

    typedef struct
    {
        U32 dRow[MAX_CHAN_TASK];
		U08 bClstOff[MAX_CHAN_TASK];
		U16 wOfstInTask[MAX_CHAN_TASK];
		U16 wLunHead;
		U16 wLunTail;
		U32 dCmdRow;
        U08 bRowCnt;
        U08 bRowPre;
        U08 bLunRowPreOpCnt;//Lun内的已经通过00-30读到Flash Cahce Register的地址数,读到Ram后减1
		U08 bRev[1];

    }T_LUN_TASK;  

	//通道任务队列
	typedef struct
	{
		volatile U32 wTaskHead;
		volatile U32 wTaskTail;
		volatile U32 wTaskCnt;
		U32 wLastOfstInTask;				//记录当前task里面属于每个通道的最后一个任务在task里面的偏移
		U32 wOfstInTask[MAX_CHAN_TASK];
        U08 bFirstFlag;
		U08 bClstCntInPage;
		U08 bRev0[6];
		U16 wPageOffInFtlTask[MAX_CHAN_TASK];//20170714新增用于OrderPrg记录页偏移,@gavin
#ifdef _TLC_ORDER_PROGRAM_
		U08 bPageOffInOrderUnit[MAX_LUN_COUNT];//20191127新增用于OrderPrg记录页偏移,@Jarvis
#endif

		T_LUN_TASK tLunTask[MAX_LUN_COUNT];
		U08 bLastAddrOPMode[MAX_LUN_COUNT];
		U08 bNextAddrOPMode[MAX_LUN_COUNT+1];//bNextAddrOPMode[0] for ftl task, others for lun task
		U08 bRev2[3];						//8字节对齐
		U08 bCacheOpFlag[MAX_LUN_COUNT];
		U32 wOfstLunRecord[MAX_LUN_COUNT]; 
		U08 bLunNoChkStsCnt[MAX_LUN_COUNT];
		U08 bLunContinueOpCnt[MAX_LUN_COUNT];
		U08 bPreLunContiOpCnt[MAX_LUN_COUNT];//record previous op page cnt 
		U32 wBufIdxFail;					//记录搜索DMAINFO时FailIdx的记录
		U08 bPlaneWriteFlag[MAX_LUN_COUNT];/*标记数据已经写入当前Plane的数据缓存区,目前支持1plane、2Plane、4Plane、8Plane*/
		U08 bRev3[3];						//8字节对齐

		//4K rand read
		U08 bLastLun;//Aaron
		U08 bStopFlag[MAX_LUN_COUNT];		//用于记录4K读时，可能当前LUN的任务通道内，没有来的及分配两个任务，如果只剩一个任务了，则置起这个标志位
		U08 bPreSendFlag[MAX_LUN_COUNT];	//记录是否已经预发出去了，cache时是表示二级缓存的预发，4K时是多CE并行的预发
	}T_CHAN_TASK_QUEUE; //8字节对齐


    typedef struct
    {    	    	
        T_CHAN_TASK_QUEUE   tChTaskQueue[MAX_CHAN_COUNT];//通道任务队列
        U32 bRev[2];		//不要使用!
        T_FDL_PARAM         pFdlParam;
        T_FTL_TASK          tFtlTask;		
    }T_GLOBAL_FDL_VAR;


    typedef struct
    {
        U32 *pdBuffAddr;
        U32 *pUdBuff;
        U32 *pMskBuff;
        T_FLASH_PHY_ADDR *pOldAddrBuff;
        T_FLASH_PHY_ADDR *pPageAddr;
    }T_WRITE_MSK_PARAM;

	typedef struct 
    {
        U08 bChipOfLun[MAX_LUN_COUNT];
        U08 bLunOfDie[MAX_LUN_COUNT];
		U16 wColOfTabClust[16];
		U16 wColOfDataClust[16];
    }T_NAND_MAP;

	#define JC_BUFF_SIZE 		(140)
	#define JC_BUFF_TABLE_ST 	(0)
	#define JC_BUFF_DATA_ST 	(72)
	extern U8 HAL_JCBufff[JC_BUFF_SIZE];

	extern T_CHAN_TASK_QUEUE	tChTaskQueue[MAX_CHAN_COUNT];//通道任务队列
	extern T_FDL_PARAM			pFdlParam;
	extern T_FTL_TASK			tFtlTask;
	extern volatile U08 gFdlTskStatus;
   
    // *****************************************************************************
    // 函数说明：遍历一次通道队列
    // 参数说明：
    //        bChan：通道编号
    // 出口参数: 
    // 返回值：无
    // 说   明:无
    // *****************************************************************************
    void ChanTaskErgodic(U8 bChan);

    // *****************************************************************************
    // 函数说明：FTL任务遍历分配
    // 参数说明：
    //        bTaskErgo：调用参数
    // 出口参数: 
    // 返回值：
    //      0 地址还没有分配完
    //      1 地址已经分配完
    // 说   明:三个地方会调用次函数
    //      a:初始化任务的时候
    //      b:添加分段任务
    //      c:中断
    // *****************************************************************************
    U8 TaskErgodic(U8 bTaskErgo);
    U8 TaskErgodicRead(U8 bTaskErgo);
	U8 TaskErgodicReadWrite(U8 bTaskErgo);
//	U08 TaskErgodicReadWriteDisc(U08 bTaskErgo);

    // *****************************************************************************
    // 函数说明：执行FTL任务遍历函数
    // 参数说明：
    //        bTaskErgo：遍历任务参数
    // 出口参数: 
    // 返回值：无
    // 说   明:无
    // *****************************************************************************
    U8 OrderTaskErgodic(U8 bTaskErgo);
    void RunFtlTaskErgodic(U8 bTaskErgo);

#ifdef __VER_TLC_PAGE__
	U08 TaskErgodicVerifyTLCPage(U08 bTaskErgo);
#endif

    // *****************************************************************************
    // 函数说明：统计task里面属于每个通道的最后一个操作所在的位置
    // 参数说明：
    //
    // 出口参数:
    // 返回值：无
    // 说   明:为了解决CQ执行最后一个读操作需做特殊处理
    // *****************************************************************************
	void ChanTaskLastAnalysisErase(void);
	void ChanTaskValidOrderAnalysis(void);
	
	U08 TaskErgodicTableDataCopyBack(U08 bTaskErgo);
	U08 TaskErgodicSingleLunCopyBack(U08 bTaskErgo);
	U08 TaskErgodicMultiLunCopyBack(U08 bTaskErgo);
	U08 TaskErgodicInnerCopyBack(U08 bTaskErgo);

	U08 ChanAnalysis4K(void);
	
//	void TaskErgodicReadTableClsts(U16 *pwBuffIdx);
#ifdef _TLC_ORDER_PROGRAM_
	U08 TaskErgodicOrderPrgUnit (U08 bTaskErgo);
#endif
	

#ifdef __cplusplus
}
#endif

#endif






