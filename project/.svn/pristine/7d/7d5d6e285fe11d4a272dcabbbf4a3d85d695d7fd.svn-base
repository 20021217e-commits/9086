//////////////////////////////////////////////////////////////////////////
// Summary:
//	本文件用于定义NCQ的相关数据结构
//////////////////////////////////////////////////////////////////////////
#ifndef	__NCQDEFINE_H_
#define	__NCQDEFINE_H_
// NCQ队列的最大个数
#ifndef MAX_CMD_NUM
#define MAX_CMD_NUM 32
#endif
// 用于存放每条NCQ的基本信息
typedef struct _TASK_PARAM
{
	// 记录NCQ任务编号，启动任务传输需要传入该参数
	U8 tag;
	// 读写方向 -数据操作才有意义，0，写入Flash，1，从Flash读取数据
	U8 dir;
	// 强制写入NAND FLASH -数据操作
	U8 fua;
	// 0:已经执行完毕的命令,1:未执行的有效命令 
	U8 isUsed; 

	// 32位LBA地址能包含2T的数据量，所以这里只使用低32位
	U32 lba;
	// 该NCQ命令操作的数据包长度，以扇区为单位
	U32 sctNum;
	// 计数器，用于表示NCQ命令的新旧
	U32 counter; 

	// 记录NCQ对应的buffer的地址
	U8* pData;

	//Reserved for future used
	U32 RFU[1];

} TASK_PARAM, *PTASK_PARAM;

// 记录整个NCQ队列的信息
typedef struct _TASK_QUEUE
{
	// NCQ 32,，如果不支持NCQ，则该队列只存放一个命令
	TASK_PARAM tasks[MAX_CMD_NUM];

	// 标识当前是否为NCQ命令
	// 非0 : 是，此时需要查询整个tasks队列中的有效NCQ
	// 0 : 不是，此时固定只区tasks的第一个元素
	U8 isNcqCmd; 

	//任务队列个数
	U32 taskCnt; 

} TASK_QUEUE, *PTASK_QUEUE; 

// 记录FW重排命令后的顺序
typedef struct _CMD_REORDER_REG
{
	// 记录重排后的NCQ命令的tag号，此buffer仅为内部可见，外部需要通过接口访问
	U8 reorderTasks[MAX_CMD_NUM];
	// CRR队列中有效的命令个数，从0开始取。FTL需要保证该buffer中的有效命令是连续的
	U32 taskCnt;

} CMD_REORDER_REG, *PCMD_REORDER_REG;
#endif

