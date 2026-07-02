

#ifndef     _CMD_HANDLE_H_    
#define     _CMD_HANDLE_H_

#include "Common.h"
#include "sata_typedef.h"

#ifdef  _CMD_HANDLE_CFG_    //模块中定义的资源，所用配置宏
#define  CMD_HANDLE_EXT
#else
#define  CMD_HANDLE_EXT  extern
#endif

#ifndef	U08
#define U08		U8
#endif

#define HOST_CMD_QUE_MAX  (32)

typedef struct 
{
	U32 m_dLba[HOST_CMD_QUE_MAX];           //起始逻辑地址
	U32 m_dLen[HOST_CMD_QUE_MAX];           //逻辑扇区长度
	U08 m_bTag[HOST_CMD_QUE_MAX];           //具体对应的Tag号
	U08 m_bCmd[HOST_CMD_QUE_MAX];           //命令为写、读、擦除
	U32 dStClstNum[HOST_CMD_QUE_MAX];       //这条命令所在的起始簇号
	U32 dEndClstNum[HOST_CMD_QUE_MAX];      //这条命令所在的结束簇号
	U16 wClstCnt[HOST_CMD_QUE_MAX];         //这条命令所包含的簇个数
	U08 bStSctOffsInClst[HOST_CMD_QUE_MAX]; //这条命令所在簇的起始扇区偏移
	U08 bEndSctOffsInClst[HOST_CMD_QUE_MAX];//这条命令所在簇的结束扇区偏移,离最后的扇区的距离
	U32 m_dCmdMaskBit;						//要执行的Tag序列
	U08 m_bQueLen;                          //有效命令队列长度
	U08 m_b4KAlignCmdCnt;                   //4k对齐的命令个数
	U08 m_bOpNcqFlag;                       //为有时，需执行命令
	U08 m_bi;
	U32 m_dRetLba;
	U32 m_dRetLen;
}T_CMD_QUE_INFO;

typedef struct 
{
	U32 dCmdBitMask;//记录现有命令Tag所对应的bit位，以便和新来的命令做判断 
	U08 bCmdType;
	U08 bCmdCnt;
	U08 bCmdIndex;						//32组为循环FIFO，不用COPY
	U08 bReserve;
}T_PENDING_LIST;//以命令来的先后顺序排列


CMD_HANDLE_EXT  __align(8)  T_CMD_QUE_INFO  g_tCmdQueInfo;

CMD_HANDLE_EXT T_PENDING_LIST     g_tPendingCmdList;


extern  void FTL_NCQCmdHandle(void);
extern  BOOL FTL_NCQNewCmdInQuire(T_TASK_QUEUE_CTX *queue,U8 init_dir,U16 wperiod_us);


#endif 



