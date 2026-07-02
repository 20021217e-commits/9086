#ifndef __JPT_FDL__
#define __JPT_FDL__

#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"
#include "flash_driver_config.h"

#define __FDL_TEST_MODE_EN__
#define __MULTI_PLANE_TEST__    //读写擦按照2plane操作
//#define __FDL_TEST_FW_CTRL__  //地址参数由FW配置，关闭:由PC通过另一条私有命令下发地址参数
//#define __NCQ_WR_TLC_ORDER__//B16A、8M3M等TLC直接编程,联动写测试


#define JUMP_CH_CE //跳通道和片选支持

#define  FDL_TEST_FLASH  NAND_FLASH

#if  (NAND_FLASH == YMTC_X2_6070)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3048  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*8)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          33554432    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2640 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1512  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif  (NAND_FLASH == YMTC_X3_6070)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              5544  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1386  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (8192*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         8192  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          33554432    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               403 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18816  //页字节数
#define  FDL_PGM_ORDER_COUNT          2760  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == YMTC_X2_9060)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              2304  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          2048*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1980 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          768  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == YMTC_X3_9060)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              2304  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          2048*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1996 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          768  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == YMTC_X3_9070)


//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数

//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             6    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              4176  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1392  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*8)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         FDL_ROW_OF_BLK  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          FDL_ROW_OF_BLK*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2178 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18368  //页字节数
#define  FDL_PGM_ORDER_COUNT          1392  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xA2)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == YMTC_X4_9060)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              4800  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1600  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (8192*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         8192  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          8192*1024    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               988 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1600  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif  (NAND_FLASH == YMTC_X4_6080)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             8    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              8480  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          2136  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               16384  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (16384*8)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         16384  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          67108864    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               270 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                19136  //页字节数
#define  FDL_PGM_ORDER_COUNT          4208  //QLC模式下WL编程次数或者页编程次数
#define  FDL_TLC_PGM_ORDER_COUNT      6408  //TLC模式下WL编程次数或者页编程次数
#define  FDL_TLC_PAGE_OF_BLK          6408  //1个Block有多少个TLC物理页(TLC模式块内可用物理页数)



//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和QLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SanDisk_BiCS5_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1792  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          3580  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (512*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*8192    //两个lun之间行地址的间隔 2^38
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               5052 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18976  //页字节数
#define  FDL_PGM_ORDER_COUNT          896  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (2)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SanDisk_BiCS5_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1344  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          448  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               512//512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  512*2//(512*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         512//512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*2048    //两个lun之间行地址的间隔 2^38
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               3323 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18336  //页字节数
#define  FDL_PGM_ORDER_COUNT          448  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有,2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SanDisk_BiCS6_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              2430  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          810  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048//512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  2048*4//(512*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048//512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*8192    //两个lun之间行地址的间隔 2^38
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               3660 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18336  //页字节数
#define  FDL_PGM_ORDER_COUNT          810  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有,2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SanDisk_BiCS8_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数

//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3270  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1090  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048//512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  2048*4//(512*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048//512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*8192    //两个lun之间行地址的间隔 2^38
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2748 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18336  //页字节数
#define  FDL_PGM_ORDER_COUNT          3270  //TLC模式下WL编程次数或者页编程次数

//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有,2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SanDisk_BiCS8_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数

//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              4360  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1090  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048//512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  2048*4//(512*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048//512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*8192    //两个lun之间行地址的间隔 2^38
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2108 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18984  //页字节数
#define  FDL_PGM_ORDER_COUNT          2180  //TLC模式下WL编程次数或者页编程次数

//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (2)//读命令前是否有前缀命令: 0为没有，1为有,2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SanDisk_BiCS6_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3240  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          810  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048//512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  2048*4//(512*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048//512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*8192    //两个lun之间行地址的间隔 2^38
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2796 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18976  //页字节数
#define  FDL_PGM_ORDER_COUNT          1620  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (2)//读命令前是否有前缀命令: 0为没有，1为有,2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == Hynix_3DV5_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1152  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          384  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (512*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               3816 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          384  //TLC模式下WL编程次数或者页编程次数

#define  FDL_UID_dRow_ADDR            (0x169)  //读取UID的行地址


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == Hynix_3DV6_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1536  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          512  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (512*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         512  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          512*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2888 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          512  //TLC模式下WL编程次数或者页编程次数

#define  FDL_UID_dRow_ADDR            (0x1f9)  //读取UID的行地址


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == Hynix_3DV7_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (0)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              4224  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1408  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (2048*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          2048*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1084 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1408  //TLC模式下WL编程次数或者页编程次数

#define  FDL_UID_dRow_ADDR            (0x1379)  //读取UID的行地址


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == Hynix_3DV8_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (0)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              5712  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1904  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (2048*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          2048*1024    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               836 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1904  //TLC模式下WL编程次数或者页编程次数

#define  FDL_UID_dRow_ADDR            (0x1251)  //读取UID的行地址



//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == Hynix_3DV9_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (0)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              7704  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          2568  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          4096*1024    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1212 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          2568  //TLC模式下WL编程次数或者页编程次数

#define  FDL_UID_dRow_ADDR            (0x1251)  //读取UID的行地址



//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == Hynix_3DV7_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              5632  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1408  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (2048*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          2048*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1616 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18944  //页字节数
#define  FDL_PGM_ORDER_COUNT          2816  //TLC模式下WL编程次数或者页编程次数

#define  FDL_UID_dRow_ADDR            (0x1201)  //读取UID的行地址


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (2)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SAMSUNG_3DV6_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3096  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1040  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          4096*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1458 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1040//1040  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SAMSUNG_3DV5_2K_MLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              8    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             8    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              5888  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          2944  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192//4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (8192*8)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         8192//4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          4096*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6//5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               3024 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36//4//36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432//2048//18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          5888//1040  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == KHMB)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             1    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              256  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          128  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               256  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  256  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         256  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          256  //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5    //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1280 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                17664//页字节数
#define  FDL_PGM_ORDER_COUNT          256  //MLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SAMSUNG_3DV4_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1012//1472//V7:5632//3096  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          264//376//V7:1408//1040  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024//2048//V7:8192//4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         FDL_ROW_OF_BLK  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          16384*FDL_ROW_OF_BLK    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5//V7:6//5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               8500 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36//38//36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432//18816//18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          500//1040  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SAMSUNG_3DV5_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1472//V7:5632//3096  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          376//V7:1408//1040  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048//V7:8192//4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         FDL_ROW_OF_BLK  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          FDL_ROW_OF_BLK*8192    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5//V7:6//5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               6000 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38//36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18816//18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          728//1040  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == SAMSUNG_3DV7_QLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              5632//3096  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1440  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192//4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         FDL_ROW_OF_BLK  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          FDL_ROW_OF_BLK*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1628 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             38   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18816  //页字节数
#define  FDL_PGM_ORDER_COUNT          2784//1040  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持




#elif (NAND_FLASH == SAMSUNG_3DV6P)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3208  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1072  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          4096*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  5     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1406 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1080  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SAMSUNG_3DV8_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              4248  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1416  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (8192*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         8192  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          8192*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               2224 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1428  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SAMSUNG_3DV9_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              5148  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1716  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (8192*4)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         8192  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          8192*4096    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1812 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          1728  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif (NAND_FLASH == SAMSUNG_3DV7_TLC)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3168  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1056  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (4096*2)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          4096*2048    //两个lun之间行地址的间隔
#define  FDL_ROW_ADDR_MODE			  6     //5地址模式（含列地址）
#define  FDL_BLK_OF_LUN               1458 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             36   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                18432  //页字节数
#define  FDL_PGM_ORDER_COUNT          3168  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持



#elif   (NAND_FLASH == MIC_B27B)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1352 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              3456  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1152  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_SECT_OF_PAGE             38   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数
#define  FDL_PGM_ORDER_COUNT          3456  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_B47R)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               2224 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              2112  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          704   //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             36   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18352 //页字节数
#define  FDL_PGM_ORDER_COUNT          2112  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0x3B)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_B58R)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             6    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               3790 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              2784  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          936   //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*8192)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             36   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18352 //页字节数
#define  FDL_PGM_ORDER_COUNT          7440  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0x3B)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (1)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_B57T)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1740 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              2784  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          928   //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*8192)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             36   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18352 //页字节数
#define  FDL_PGM_ORDER_COUNT          2784  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0x3B)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (1)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_N48R)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               3268 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              2816  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          704   //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             36   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18352 //页字节数
#define  FDL_PGM_ORDER_COUNT          1408  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0x3B)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0x3B)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_N58R)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               2540 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              3712  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          928   //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个plane间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*4096)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             38   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18992 //页字节数
#define  FDL_PGM_ORDER_COUNT          1856  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0x3B)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0x3B)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_QLC_SB1_CMD		  (0x01)
#define  FDL_READ_QLC_SB2_CMD		  (0x02)
#define  FDL_READ_QLC_SB3_CMD		  (0x03)
#define  FDL_READ_QLC_SB4_CMD		  (0x04)


#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_N69R)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             6    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               4086 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              4416  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1104   //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个plane间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*8) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*8192)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             38   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                19192 //页字节数
#define  FDL_PGM_ORDER_COUNT          2208  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0x3B)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (2)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0x3B)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_QLC_SB1_CMD		  (0x01)
#define  FDL_READ_QLC_SB2_CMD		  (0x02)
#define  FDL_READ_QLC_SB3_CMD		  (0x03)
#define  FDL_READ_QLC_SB4_CMD		  (0x04)


#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif   (NAND_FLASH == INTEL_N38A)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1032 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              9216  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          2304  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               16384  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             38   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0x3B)
#define  FDL_WRITE_TLC_PRE_CMD        (0x3C)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0x3B)
#define  FDL_ERASE_TLC_PRE_CMD        (0x3C)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持


#elif   (NAND_FLASH == INTEL_N38B)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1000 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              9216  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          2304  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               16384  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             38   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数
#define  FDL_PGM_ORDER_COUNT          1568  //TLC模式下WL编程次数或者页编程次数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0x3B)
#define  FDL_WRITE_TLC_PRE_CMD        (0x3C)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0x3B)
#define  FDL_ERASE_TLC_PRE_CMD        (0x3C)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == INTEL_N4xA)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 (0)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              4    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1016 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              12288  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          3072  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               16384  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_ROW_ADDR_MODE			  6     //6地址模式（含列地址）
#define  FDL_SECT_OF_PAGE             38   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数
#define  FDL_PGM_ORDER_COUNT          2080  //TLC/QLC模式下WL编程次数或者页编程次数。per deck


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为TLC, 2为QLC
#define  FDL_READ_SLC_PRE_CMD         (0x3B)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0x3C)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0x3B)
#define  FDL_WRITE_TLC_PRE_CMD        (0x3C)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0x3B)
#define  FDL_ERASE_TLC_PRE_CMD        (0x3C)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持



#endif


/*    these flash parameters down below are defined by SSD FW group
#if  (NAND_FLASH == TOS_6DDK)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           0    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 0    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             24   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             2    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              256  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          256  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               256  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  256  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         256  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          0    //两个lun之间行地址的间隔
#define  FDL_BLK_OF_LUN               2132 //每个lun包含的block个数
#define  FDL_SECT_OF_PAGE             32   //1个page有多少个扇区
#define  FDL_PAGE_SIZE                17664  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (0)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (0)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (0)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (0)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (0)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (0)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (0)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (0)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (0)//是否支持cache写操作，0为不支持，1为支持

#elif  (NAND_FLASH == H27QEG8NDB)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             2//4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              384  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          128  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               256  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  512  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         256  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (2732*256)    //
#define  FDL_BLK_OF_LUN               2732 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18432  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDE)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDE)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDE)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_B16A)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             2    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              2304  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  8192  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (1008*4096)    //
#define  FDL_BLK_OF_LUN               1008 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18592  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_B17A)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#ifdef __FDL_CRC_FUNC__
#define  HAL_CRC_MODE                 (1)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (0)//(8)
#define  HAL_TAB_PUNC_LEN             (0)
#else
#define  HAL_CRC_MODE                 (0xFF)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (0)
#define  HAL_TAB_PUNC_LEN             (0)
#endif
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              2304  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  16384  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (2016*4096)    //
#define  FDL_BLK_OF_LUN               2016 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18592  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == MIC_B27A)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               944 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              5184  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1728  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)
#define  FDL_SECT_OF_PAGE             32   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持


#elif   (NAND_FLASH == MIC_B27B)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#ifdef __FDL_CRC_FUNC__
#define  HAL_CRC_MODE                 (1)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (18)//(8)
#define  HAL_TAB_PUNC_LEN             (20)
#else
#define  HAL_CRC_MODE                 (0xFF)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#endif
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1352 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              3456  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1152  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_SECT_OF_PAGE             32   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == INTEL_B27B)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#ifdef __FDL_CRC_FUNC__
#define  HAL_CRC_MODE                 (1)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (18)//(8)
#define  HAL_TAB_PUNC_LEN             (20)
#else
#define  HAL_CRC_MODE                 (0xFF)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (14)
#define  HAL_TAB_PUNC_LEN             (16)
#endif
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               1352 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              3456  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1152  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_SECT_OF_PAGE             32   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == MIC_N18)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数

//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              3072//2328  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768//792  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  16384  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (2944*4096)    //
#define  FDL_BLK_OF_LUN               2944 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18592  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持


#elif (NAND_FLASH == MIC_N28)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#ifdef __FDL_CRC_FUNC__
#define  HAL_CRC_MODE                 (1)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (8)//(8)
#define  HAL_TAB_PUNC_LEN             (8)
#else
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (4)
#define  HAL_TAB_PUNC_LEN             (4)
#endif
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D
#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数

//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              4608  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          1152  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               8192  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN)  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)//(1<<20);	//
#define  FDL_BLK_OF_LUN               1968 //
#define  FDL_SECT_OF_PAGE             32	//
#define  FDL_PAGE_SIZE                18592  //页字节数

//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif (NAND_FLASH == MIC_M16)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              2304//2328  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          768//792  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               4096  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  16384  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         4096  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (2944*4096)    //
#define  FDL_BLK_OF_LUN               2944 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18592  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif  (NAND_FLASH == MIC_B0KB)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             2    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1536  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          512//544  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  8192  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         2048  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (1024*4096)    //
#define  FDL_BLK_OF_LUN               2192 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18592  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (1)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)//B0KB datasheet have no slc mode describe
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x81)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif  (NAND_FLASH == K9ADGD8U0C)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             2    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             1    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              384  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          128  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               512  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  512  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         0  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (512*4096)    //
#define  FDL_BLK_OF_LUN               2800 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18432  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (1)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)//
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x81)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif  (NAND_FLASH == K9ADGD8U0A)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                5//7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             4    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              256  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          128  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               256  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  512  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         256  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (512*4096)    //
#define  FDL_BLK_OF_LUN               4096 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                17920  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (1)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)//
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x81)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif  (NAND_FLASH == MIC_L06B)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             2    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1024 //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          512  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  4096  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         1024  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (1024*4096)    //
#define  FDL_BLK_OF_LUN               2192 //
#define  FDL_SECT_OF_PAGE             32   //
#define  FDL_PAGE_SIZE                18592  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (1)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)//B0KB datasheet have no slc mode describe
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x81)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == TOS_BICS3)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //由于页大小的关系，bics3不能用模式8	//LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              768  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          256  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               256  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  512  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         256  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (2956*256)//(1<<20);	//
#define  FDL_BLK_OF_LUN               2956 //
#define  FDL_SECT_OF_PAGE             32	//
#define  FDL_PAGE_SIZE                18336  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xA2)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         //(0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        //(0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        //(0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)//0x81
#define  FDL_MUTI_PALNE1_WT_CMD       (0x80)//0x81

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == HYNIX_3DV4)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7//8    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             4    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              1728  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          576  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  4096  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         1024  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (1024*4096)    //
#define  FDL_BLK_OF_LUN               2520 //
#define  FDL_SECT_OF_PAGE             16   //
#define  FDL_PAGE_SIZE                9216  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (1)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xA2)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         //(0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xA2)
#define  FDL_WRITE_TLC_PRE_CMD        //(0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xA2)
#define  FDL_ERASE_TLC_PRE_CMD        //(0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == SUMSUNG_3DV4)

//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                8    //LDPC模式选择: 5,7,8
#ifdef __FDL_CRC_FUNC__
#define  HAL_CRC_MODE                 (1)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (8)//(8)
#define  HAL_TAB_PUNC_LEN             (8)
#else
#define  HAL_CRC_MODE                 (0xFF)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (4)
#define  HAL_TAB_PUNC_LEN             (4)
#endif
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数


//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             2    //每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    //每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    //每个LUN中包含的plane个数
#define  FDL_PAGE_OF_BLK              768  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          256  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               1024  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  2048  //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_PLANE         1024  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (1024*2852)    //
#define  FDL_BLK_OF_LUN               2852 //

#define  FDL_SECT_OF_PAGE             32   //32
#define  FDL_PAGE_SIZE                18432//18432  //页字节数


//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (1)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (0)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         //(0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        //(0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#define  FDL_SNAP_READ_EN             (0)//是否支持snap读操作，0为不支持，1为支持

#elif   (NAND_FLASH == YMTC_B08T)
//HAL REG PARAM
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                7    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D

#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数

//FLASH PHY PARAM
#define  FDL_CHIP_OF_CHAN             1    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             2    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               2012 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              1152  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          384  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               2048  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_SECT_OF_PAGE             32   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18432 //页字节数

//FLASH CMD PARAM
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令

#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)

#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)

#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)

#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)

#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)

#define  FDL_MULTI_PLN_EN             (1)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持

#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持

#elif   (NAND_FLASH == YMTC_A06M)
#define  HAL_INTERFACE_MODE           2    //Nandflash接口选择: 0为NORMAL模式, 2为TOGGLE模式
#define  HAL_ECC_MODE                 1    //ECC纠错模块选择: 0为BCH, 1为LDPC模式
#define  HAL_BCH_CORR_BIT             0x28   //BCH模式纠错bit能力选择: 40，36，30，24，16，8
#define  HAL_LDPC_MODE                9    //LDPC模式选择: 5,7,8
#define  HAL_CRC_MODE                 1    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_SECT_OF_DMA2             4    //DMA2的有效数据长度(单位:扇区数)
#define  HAL_RND_SEED_MODE            0    //随机模式选择: 0:三星算法, 1:三星算法, 2:东芝2D/3D, 3:东芝 3D
#define  HAL_PAGE_FILL_BYTE           0    //页内最后空余需硬件填充字节数
#ifdef __FDL_CRC_FUNC__
#define  HAL_CRC_MODE                 (1)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (8)    //(8)
#define  HAL_TAB_PUNC_LEN             (8)
#else
#define  HAL_CRC_MODE                 (0xFF)    //CRC模式选择---4:CRC32,2:CRC16,0:关闭CRC模式
#define  HAL_DATA_PUNC_LEN            (16)
#define  HAL_TAB_PUNC_LEN             (16)
#endif
#define  FDL_CHIP_OF_CHAN             1    	//每个通道中的chip个数
#define  FDL_LUN_OF_CHIP              1    	//每个Chip中的LUN个数
#define  FDL_PLANE_OF_LUN             1    	//每个LUN中包含的plane个数
#define  FDL_BLK_OF_LUN               2144 	//一个Lun的块个数
#define  FDL_PAGE_OF_BLK              256  //1个Block有多少个物理页(正常模式块内可用物理页数)
#define  FDL_SLC_PAGE_OF_BLK          256  //1个Block有多少个SLC物理页(SLC模式块内可用物理页数)
#define  FDL_ROW_OF_BLK               256  //Block占用的物理行地址大小，用于计算页地址
#define  FDL_ROW_GAP_OF_PLANE         (FDL_ROW_OF_BLK)  //每个palne间的行地址间隔
#define  FDL_ROW_GAP_OF_BLK_IN_PLANE  (FDL_ROW_OF_BLK*FDL_PLANE_OF_LUN) //一个plane内，相邻两个blk的行地址间隔
#define  FDL_ROW_GAP_OF_2LUN          (FDL_ROW_OF_BLK*FDL_BLK_OF_LUN)	//两个Lun之间的行地址间隔
#define  FDL_SECT_OF_PAGE             32   	//一个Page的Sector个数
#define  FDL_PAGE_SIZE                18592 //页字节数
#define  FDL_FLASH_SOT_TYPE           (1)//flash是SLC或TLC类型: 0为SLC, 1为TLC
#define  FDL_FLASH_WORK_MODE          (0)//flash处于SLC或TLC模式: 0为SLC, 1为TLC
#define  FDL_FLASH_IO_MODE            (1)//flash接口模式  0为Asycn  1:Toggle
#define  FDL_MODE_CHANGE_CMD          (0xBF)//flashSLC和TLC模式之间切换命令
#define  FDL_READ_HAVE_PRE_CMD        (1)//读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_SLC_PRE_CMD         (0xDA)
#define  FDL_READ_HAVE_PRE_CMD_TLC    (1)//TLC读命令前是否有前缀命令: 0为没有，1为有
#define  FDL_READ_PRE_CMD_TLC_MODE    (0)//TLC读前缀命令模式:1为取FDL_READ_TLC_PRE_CMD，0为由Row最高Byte决定
#define  FDL_READ_TLC_PRE_CMD         (0xDF)
#define  FDL_WRITE_HAVE_PRE_CMD       (1)//写命令前是否有前缀命令: 0为没有，1为有
#define  FDL_WRITE_SLC_PRE_CMD        (0xDA)
#define  FDL_WRITE_TLC_PRE_CMD        (0xDF)
#define  FDL_ERASE_HAVE_PRE_CMD       (1)//擦命令前是否有前缀命令: 0为没有，1为有
#define  FDL_ERASE_SLC_PRE_CMD        (0xDA)
#define  FDL_ERASE_TLC_PRE_CMD        (0xDF)
#define  FDL_MUTI_PALNE0_WT_CMD       (0x80)
#define  FDL_MUTI_PALNE1_WT_CMD       (0x81)
#define  FDL_READ_TLC_SB1_CMD		  (0x01)
#define  FDL_READ_TLC_SB2_CMD		  (0x02)
#define  FDL_READ_TLC_SB3_CMD		  (0x03)
#define  FDL_MULTI_PLN_EN             (0)//是否支持多plane操作，0为不支持，1为支持
#define  FDL_MULTI_LUN_EN             (0)//是否支持多lun操作，0为不支持，1为支持
#define  FDL_MULTI_CE_EN              (1)//是否支持多ce操作，0为不支持，1为支持
#define  FDL_MULTI_CLST_EN            (1)//是否支持多clst合并操作，0为不支持，1为支持
#define  FDL_CAHCE_READ_EN            (1)//是否支持cache读操作，0为不支持，1为支持
#define  FDL_CAHCE_WRITE_EN           (1)//是否支持cache写操作，0为不支持，1为支持
#endif
*/



//#ifdef JUMP_CH_CE//跳CH和CE自动Mapping支持
typedef struct _T_INVAL_CHAN_CHIP_MAP
{
	U8 bInvalChan;//无效通道,0:表示该通道无效,1:表示该通道有效
	U8 bInvalChip[4];//无效片选
}T_INVAL_CHAN_CHIP_MAP;
//#endif


typedef struct _T_RDWR_DATA
{
	U32 dOpPageAddr;
	U16 wOpBlk;
	U16 wOpPageOffs;

	U8  bOpCh;
	U8  bOpCe;
	U8  bOpLun;
	U8  bOpPln;

	U32 dOpByteCnt;
	U16 wOpSctCnt;
	U8  bErrDma;
	U8  bErrCnt;

	U8  bDir;
	U8  Reserve[5];
	U16 wDevFrequency;
	U32 dIndex[32];

}T_RDWR_DATA;

typedef struct
{
	U32 dBlkAddr;
	U32 dBlkCnt;
	U32 dPageAddr;
	U32 dPageCnt;
	U32 dClstCnt;
    U32 dSecCnt;
	U32 dSeedBuffEn;
	U32 dMaskEn;
    U32 dSize;
    U32 dRev[2];
	U8 *pBuff;
	U8 bBuff[20*4];
	U32 offset;
	U8 FeatureAddress;
	U8 P1;
	U8 P2;
	U8 P3;
	U8 P4;
}T_FDL_CTRL;//total 32*4byte

typedef struct
{
	U32 dPlaneCnt;
	U32 dSeedIdx;
	U32 dPageAddr;
	U32 dPageCnt;
	U32 dDelayCtrl; // [31] ResetOnOff      [14:0] Delay_us
	U32 dSecCnt;
	U32 dSeedBuffEn;
	U32 dSectionCnt; //每层的Section数
	U32 dWareIdx;
	U16 dWareMode[6];  //0-7:Logic0-Logic7 8:StdWareData 9:SamsungRandData
	U8  bBlklist[20*4];
	U32 dMode;//0-SLC 1-TLC
	U8 FeatureAddress;
	U8 bChan;
	U8 bChip;
	U8 bRBMode;
	U8 P4;
}T_FDL_FTCTRL;//total 32*12byte

typedef struct
{
	U08 bRndState;
	U08 bEccState;
	U32 dPadDriver;
	U32 dInputImpedance;
	U32 dFlashInnnerImpedan;
	U32 dSysclkSel;
	U32 dSysclkPeriodSel;
	U32 dTrx2clkPeriodSel;
	U32 dRegDelayVal;
    U08 bChan[4];
    U08 bCe[4];
    U16 wBlk;
    U08 bWtEn;
    U08 bRev[3];
}T_FDL_SCAN_FLASH_DRIVER;

typedef struct
{
    U08 bDir;
    U08 bDataPatten;
    U08 bGcMode;
    U08 bScanEn;
    U08 bKbyteSlice;
    U08 bRev[3];
    U32 dPadDriver;
    U32 dInputImpedance;
    U32 dSysclkSel;
    U32 dSysclkPeriodSel;
    U32 dTrx2clkPeriodSel;
    U32 dRegDelayVal;
	U32 dGcAddr;
}T_FDL_SCAN_GC_DRIVER;


typedef struct _Register
{
    U32 RegAddr;
    U32 RegVal;
    U8 Width;
    U8 Operate;
}Register;




#define OP_MOVE         (0)
#define OP_AND          (1)
#define OP_OR           (2)
#define OP_XOR          (3)


#define ERASE_TYPE	(0)
#define READ_TYPE	(1)
#define WRITE_TYPE	(2)

typedef enum
{
    M_1CH_1LUN_1PLN = 0,
    M_1CH_1LUN_2PLN = 1,
    M_1CH_2LUN_1PLN = 2,
    M_1CH_2LUN_2PLN = 3,
    M_2CH_1LUN_1PLN = 4,
    M_2CH_1LUN_2PLN = 5,
    M_2CH_2LUN_1PLN = 6,
    M_2CH_2LUN_2PLN = 7,
    M_4CH_1LUN_1PLN = 8,
    M_4CH_1LUN_2PLN = 9,
    M_4CH_2LUN_1PLN = 10,
    M_4CH_2LUN_2PLN = 11,
}FLASH_TEST_MODE;

typedef struct CRC_ERR
{
	U8 bTag;
	U8  bDir;
	U16 wDataFisNum;
}T_CRC_ERR;



void FDL_Main();
void FDL_Test_Init();

void FDL_CMD_C0_FDLInit(void);
void FDL_CMD_C1_EraseBlocks(void);
void FDL_CMD_C2_WriteBuffToFlash(void);
void FDL_CMD_C3_ReadClstToBuff(void);
void FDL_CMD_C5_WriteLittleData(void);
void FDL_CMD_C6_ReadLittleData(void);
void FDL_CMD_C7_JointWritePages(void);
void FDL_CMD_C8_JointReadClsts(void);
//void FDL_CMD_C9_JointReadClstsWithMask(void);
void FDL_CMD_C9_PreReadTableToBuff4K(void);
void FDL_CMD_CA_WriteDiscreteBuffToFlash(void);
void FDL_CMD_CB_ReadClstToDiscreteBuff(void);
void FDL_CMD_CC_MergeBuffClstToFlash(void);
void FDL_CMD_CD_JointDiscreteWirteToBuff(void);
void FDL_CMD_CE_FlashReadID(void);
void FDL_CMD_CF_DiscreteBuffReadNoJoint(void);

void FDL_CMD_D0_ReadClstToBuffDebug(void);
void FDL_CMD_D8_CopyBackProg(void);
void FDL_CMD_D9_MultiLunExdieCopyBackPrg(void);
void FDL_CMD_DA_MultiLunIndieCopyBackPrg(void);
void FDL_CMD_DB_OrderPrgBuffToFlash(void);
void FDL_CMD_DC_JointOrderPrgUnit(void);
void FDL_CMD_DE_MultiLunIndieCopyBackOrderPrg(void);
void FDL_CMD_DD_MultiLunExdieCopyBackOrderPrg(void);
void FDL_CMD_DF_MultiLunInnerCopyBackOrderPrg(void);
void FDL_CMD_F2_WriteTlcBlk(void);
void FDL_CMD_F3_WriteTlcPage(void);
void FDL_CMD_E2_ScanFlashDriver(void);
void FDL_CMD_E3_ScanGCDriver(void);
void FDL_CMD_E4_FlashInit(void);

void FDL_CMD_FE_GetFlashIOData(void);
void FDL_CMD_FF_InjectionCRCErr(void);

void FDL_CMD_E0_VerifyTLCPage(void);

void SetRegisterValue(void);
void GetRegisterValue(void);
void FDL_CMD_A8_WriteFlash_PhyPage();
void FDL_CMD_A9_ReadFlash_PhyPage();
void Get_ValidChan();
void SetReadOffsVal();
void FDL_EraseBlk();
void FDL_SetLDPC_ItrVal();
void FDL_SetSmallDMAInfoSize();

void FDL_Test_EraseBlocks(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 wBlkStartAddr,U16 wBlkCnt);
void FDL_Test_JointReadClsts(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub,U32 dTotalSctSize);
void FDL_Test_JointWritePages(U8 bChanCnt, U8 bLunCnt, U8 bPlaneCnt, U32 dStartBlkNub,U32 dTotalSctSize);
void FDL_Test_Analyse_Exception(U32 dExceptionBuffAddr,U8 bTaskType);
void Expt_ReadClstToDiscreteBuff();
void Ldpc_SetPValue();
void Ldpc_SetLLRAndOffset();
void CheckDevice(void);
void CQ_WaitDone(U8 bChan);
void Chan_Check_Status(U8 bChan, U8 bChip, U8 bTestFailEn);

#ifdef __cplusplus
}
#endif
#endif





