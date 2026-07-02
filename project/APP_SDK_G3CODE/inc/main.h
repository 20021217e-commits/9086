#ifndef    _MAIN_H_
#define    _MAIN_H_

#include "debug.h"

#ifndef SECTOR_SIZE
#define SECTOR_SIZE     (512)
#endif

#define MEM_DMA_BUFF_ADDR			((U32)(&gScan.bDMABuff[0]))
#define MEM_DMA_BUF_SEC				(512)
#define MEM_DMA_BUFF_SIZE           (MEM_DMA_BUF_SEC * SECTOR_SIZE)
#define MEM_UD_BUFF_ADDR			((U32)(&gScan.bUserDataBuff[0]))
#define MEM_UD_BUFF_SEC				(4)
#define MEM_UD_BUFF_SIZE			(MEM_UD_BUFF_SEC * SECTOR_SIZE)

#define MEM_DMA2_INFO_ADDR		    ((U32)(&gScan.bDMA2InfoBuff[0]))
#define MEM_DMA2_INFO_SIZE			(MEM_DMA_BUF_SEC*2*2)
#define MEM_JCTABLE_BUFF_ADDR		((U32)(&gScan.bJCTableBuff[0]))
#define MEM_JCTABLE_BUFF_SEC		(16)
#define MEM_JCTABLE_BUFF_SIZE		(MEM_JCTABLE_BUFF_SEC * SECTOR_SIZE)


#define MEM_JOINT_BUFF_ADDR			((U32)(&gScan.bDMABuff[MEM_DMA_BUFF_SIZE >> 1]))
#define MEM_JOINT_BUFF_SEC			(128)
#define MEM_JOINT_BUFF_SIZE			(MEM_JOINT_BUFF_SEC * SECTOR_SIZE)



#define MEM_NAND_ADDR_BUFF_ADDR     ((U32)(&gScan.bNandAddrBuff[0]))
#define MEM_NAND_ADDR_BUFF_SEC      16//(2)
#define MEM_NAND_ADDR_BUFF_SIZE     (MEM_NAND_ADDR_BUFF_SEC * SECTOR_SIZE)

#define MEM_READ_EXCEP_BUFF_ADDR    ((U32)(&gScan.bReadExcepBuff[0]))
#define MEM_READ_EXCEP_BUFF_SEC     (10)
#define MEM_READ_EXCEP_BUFF_SIZE    (MEM_READ_EXCEP_BUFF_SEC * SECTOR_SIZE)

#define MEM_WRITE_EXCEP_BUFF_ADDR   ((U32)(&gScan.bWriteExcepBuff[0]))
#define MEM_WRITE_EXCEP_BUFF_SEC    (2)
#define MEM_WRITE_EXCEP_BUFF_SIZE   (MEM_WRITE_EXCEP_BUFF_SEC * SECTOR_SIZE)

#define MEM_UD1_BUFF_ADDR           ((U32)(&gScan.bUd1Buff[0])) 
#define MEM_UD1_BUFF_SEC		    (16)
#define MEM_UD1_BUFF_SIZE			(MEM_UD1_BUFF_SEC * SECTOR_SIZE)

#define MEM_MSK_BUFF_ADDR		    ((U32)(&gScan.bMskBuff[0])) 
#define MEM_MSK_BUFF_SEC			(2)
#define MEM_MSK_BUFF_SIZE		    (MEM_MSK_BUFF_SEC*SECTOR_SIZE)

#define MEM_SEED_BUFF_ADDR          ((U32)(&gScan.bSeedBuff[0]))
#define MEM_SEED_BUFF_SEC			(4)
#define MEM_SEED_BUFF_SIZE		    (MEM_SEED_BUFF_SEC*SECTOR_SIZE)

#define MEM_TXBENCH_W_BUFF			(MEM_SEED_BUFF_ADDR + MEM_SEED_BUFF_SIZE)


#define MEM_TXBENCH_W_SEC			(16)
#define MEM_TXBENCH_W_SIZE			(MEM_TXBENCH_W_SEC*SECTOR_SIZE)

//以下为CopyBack验证Case所需Buff,实际大小FTL需根据一次操作的数据量和Buff使用规则自行分配
#define MEM_COPYBACK_SRC_ADDR       0x470000//(MEM_JCTABLE_BUFF_ADDR)
#define MEM_COPYBACK_SRC_SEC        0x20//(12)
#define MEM_COPYBACK_SRC_SIZE       (MEM_COPYBACK_SRC_SEC*SECTOR_SIZE)

#define MEM_COPYBACK_DST_ADDR       (MEM_COPYBACK_SRC_ADDR+MEM_COPYBACK_SRC_SIZE)
#define MEM_COPYBACK_DST_SEC         0x28//(4)
#define MEM_COPYBACK_DST_SIZE       (MEM_COPYBACK_DST_SEC*SECTOR_SIZE)

#define MEM_COPYBACK_UD_ADDR      	(MEM_COPYBACK_DST_ADDR+MEM_COPYBACK_DST_SIZE)
#define MEM_COPYBACK_UD_SEC       	 0x28//(4)
#define MEM_COPYBACK_UD_SIZE      	(MEM_COPYBACK_UD_SEC*SECTOR_SIZE)

#define MEM_CB_RETRY_INFOR_ADDR     (MEM_COPYBACK_UD_ADDR+MEM_COPYBACK_UD_SIZE)
#define MEM_CB_RETRY_INFOR_SEC       0x40
#define MEM_CB_RETRY_INFOR_SIZE     (MEM_CB_RETRY_INFOR_SEC*SECTOR_SIZE)

#define MEM_UD_BUFF_ADDR_EXT       	 (MEM_CB_RETRY_INFOR_ADDR+MEM_CB_RETRY_INFOR_SIZE) 
#define MEM_UD_BUFF_SEC_EXT			 (0x40)
#define MEM_UD_BUFF_SIZE_EXT		 (MEM_UD_BUFF_SEC_EXT*SECTOR_SIZE)

//以下为Pre 4K Case所需Buff
#define MEM_PRE4K_DATA_ROW_ADDR      MEM_COPYBACK_SRC_ADDR
#define MEM_PRE4K_DATA_ROW_SEC       (0x10)

#define MEM_PRE4K_TAB_ROW_ADDR       (MEM_PRE4K_DATA_ROW_ADDR+MEM_PRE4K_DATA_ROW_SEC*SECTOR_SIZE)
#define MEM_PRE4K_TAB_ROW_SEC        (0x10)

#define MEM_PRE4K_DATA_GROUP_ADDR    (MEM_PRE4K_TAB_ROW_ADDR+MEM_PRE4K_TAB_ROW_SEC*SECTOR_SIZE)
#define MEM_PRE4K_DATA_GROUP_SEC     (0x20)

#define MEM_PRE4K_DATA_EXCEP_ADDR    (MEM_PRE4K_DATA_GROUP_ADDR+MEM_PRE4K_DATA_GROUP_SEC*SECTOR_SIZE)
#define MEM_PRE4K_DATA_EXCEP_SEC	 (0x20)

#define MEM_PRE4K_TAB_EXCEP_ADDR     (MEM_PRE4K_DATA_EXCEP_ADDR+MEM_PRE4K_DATA_EXCEP_SEC*SECTOR_SIZE)
#define MEM_PRE4K_TAB_EXCEP_SEC		 (0x20) //注意不要随意扩大,覆盖到       MEM_UD_BUFF_ADDR_EXT

typedef struct _T_SCAN_GLOBAL_VAR
{
    U8 bDMABuff[MEM_DMA_BUFF_SIZE];              //DMABUFF
    U8 bUserDataBuff[MEM_UD_BUFF_SIZE];          //FDL读Nandflash过程中保存Userdata的buff
    U8 bDMA2InfoBuff[MEM_DMA2_INFO_SIZE];        //FDL接口读Nandflash过程中保存DMA的ECC信息buff
    U8 bJCTableBuff[MEM_JCTABLE_BUFF_SIZE];      //跳列表，4个通道的，每个通道需计算好偏移

	U8 bNandAddrBuff[MEM_NAND_ADDR_BUFF_SIZE];   //FTL应用层放置下发给FDL的Nandflash物理页地址buff
	U8 bReadExcepBuff[MEM_READ_EXCEP_BUFF_SIZE]; //FDL接口读或擦发生异常，异常信息放置buff
	U8 bWriteExcepBuff[MEM_WRITE_EXCEP_BUFF_SIZE];//FDL接口写发生异常，异常信息放置buff

    U8 bUd1Buff[MEM_UD1_BUFF_SIZE];
    U8 bMskBuff[MEM_MSK_BUFF_SIZE];

    U8 bSeedBuff[MEM_SEED_BUFF_SIZE];

	
    U8 bTxbenchWBuff[MEM_TXBENCH_W_SIZE];
}T_SCAN_GLOBAL_VAR;

extern T_SCAN_GLOBAL_VAR gScan;



#endif
