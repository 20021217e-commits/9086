// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  hal.h
//  文件标识：  
//  内容摘要：  数据类型定义
//  其它说明：  
//  当前版本：  v0.10
//  项   目：  SG9081
//  作   者：  gary
// *****************************************************************************
#ifndef __FLASH_OP_CMD_H__
#define __FLASH_OP_CMD_H__

#include "flash_driver_config.h"//不包含有问题
#include "hal_cq.h"//不包含有问题


#ifdef __cplusplus
extern "C" {
#endif

#define FL_CMD_READ1                    (0x00)     // ;Read flash command 
#define FL_CMD_READ_SNAP				(0x20)
#define FL_CMD_READ2                    (0x30)    //
#define FL_CMD_READ3                    (0x39)    //
#define FL_CMD_READ_MP                  (0x32)    // read multi plane
#define FL_CMD_READ_SPARE               (0x50)     // ;Read Spare
#define FL_CMD_READID                   (0x90)     // ;Read ID
#define FL_CMD_READID_EXT               (0x91)     // ;Read Extended ID
#define FL_CMD_CACHE_REDA               (0x31)     // ;Cache Read
#define FL_CMD_CACHE_REDA_FINAL         (0x3F)     // ;Cache Read Final
#define FL_CMD_SOFT_READ0               (0x33)
#define FL_CMD_SOFT_READ1				(0x36)
#define FL_CMD_RETRY_ENABLE				(0x5D)

#define FL_CMD_RECOVERY                 (0x38)     // AGN Device Recovery
#define FL_CMD_COPY_BACK_READ1          (0x00)     // ;Read for Copy Back
#define FL_CMD_COPY_BACK_READ2          (0x35)     // ;Read for Copy Back
#define FL_CMD_PAGE_PROG1               (0x80)     // ;Page Program1
#define FL_CMD_PAGE_PROG1_MP            (0x81)    // multi plane prgm
#define FL_CMD_PAGE_PROG2               (0x10)     // ;Page Program2
#define FL_CMD_PAGE_PROG2_MP            (0x11)
#define FL_CMD_COPY_BACK_PROG1          (0x85)     // ;Copy-Back Program1
#define FL_CMD_COPY_BACK_PROG2          (0x10)     // ;Copy-Back Program2
#define FL_CMD_ED3_PRECMD1              (0x09)
#define FL_CMD_ED3_PRECMD2              (0x0D)
#define FL_CMD_ED3_PAGE_PROG			(0xC0)

#define FL_CMD_RAND_DATA_IN             (0x85)     // ;Random Data Input*
#define FL_CMD_RANDOM_DATA_OUTPUT1      (0x05)     // ;Random Data Output1*
#define FL_CMD_RANDOM_DATA_OUTPUT2      (0xE0)     // ;Random Data Output2*
#define FL_CMD_CACHE_PROG1              (0x80)     // ;Cache program1

#ifndef  __SLC_0X10_PRAGRAM__
#define FL_CMD_CACHE_PROG2              (0x15)     // ;Cache program2
#else
#define FL_CMD_CACHE_PROG2				(0x10)	   // ;Cache program2
#endif

#define FL_CMD_PROG_CONFIRM1            (0x8B)    // ;Page Program confirm
#define FL_CMD_PROG_CONFIRM2            (0x10)    // ;Page Program confirm

#define FL_CMD_BLOCK_ERASE1             (0x60)     // ;Block Erase1
#define FL_CMD_BLOCK_ERASE2             (0xD0)     // ;Block Erase2
#define FL_CMD_READ_STATUS              (0x70)     // ;Read Status 
#define FL_CMD_READ_MULTI_PLN_STATUS    (0x71)
#define FL_CMD_READ_ENHANCE_STATUS      (0x78)
#define FL_CMD_READ_LUN_STATUS(x)       (0xf0+x)        
#define FL_CMD_ED3_TLC_SPLCMD           (0xA2)     
#define FL_CMD_SAMSUNG_SPLCMD           (0xDA)    

#define FL_CMD_SET_FEATURE				(0xEF)
#define FL_CMD_GET_FEATURE				(0xEE)
#define FL_CMD_MUL_PLANE_CMD            (0x11)
#define FL_CMD_RESET_FLASH              (0xFF)  
#define FL_CMD_HARD_RESET_FLASH			(0xFD)
#define FL_CMD_SYNCRESET_FLASH			(0xFC)
#define FL_CMD_8T23_RESET_FLASH			(0x89)
#define FL_CMD_RESET_LUN                (0xFA)
#define FL_CMD_SLC_MODE              	(0xDA)
#define FL_CMD_TLC_MODE                 (0xDF)
#define FL_CMD_EXIT_CHK_IOSTA           (0x00)

#if ((NAND_FLASH == SanDisk_BiCS5_QLC) || (NAND_FLASH == TOS_BICS3)||(NAND_FLASH == SanDisk_BiCS6_TLC)||(NAND_FLASH==SanDisk_BiCS8_TLC)||(NAND_FLASH == SanDisk_BiCS6_QLC)||(NAND_FLASH==SanDisk_BiCS8_QLC))/////TOSHIBA flash
#define FL_CMD_ED3_PRO_CMD              (0x1A)
#define FL_CMD_ED3_PRO2_CMD				(0x10)
#elif (NAND_FLASH == YMTC_X2_9060||NAND_FLASH == YMTC_X2_6070||NAND_FLASH == YMTC_X3_9060||NAND_FLASH == YMTC_X3_9070||NAND_FLASH == YMTC_X3_6070||NAND_FLASH == YMTC_X4_9060||NAND_FLASH == YMTC_X4_6080)/////YMTC flash
#define FL_CMD_ED3_PRO_CMD              (0x1A)
#define FL_CMD_ED3_PRO2_CMD				(0x10)
#elif ((NAND_FLASH == Hynix_3DV6_TLC)||\
	   (NAND_FLASH == Hynix_3DV7_TLC)||\
	   (NAND_FLASH == Hynix_3DV5_TLC)||\
	   (NAND_FLASH == Hynix_3DV8_TLC)||\
	   (NAND_FLASH == Hynix_3DV9_TLC)||\
	   (NAND_FLASH == Hynix_3DV7_QLC))/////Hynix flash
#define FL_CMD_ED3_PRO_CMD              (0x22)
#define FL_CMD_ED3_PRO2_CMD				(0x23)
#define FL_CMD_SET_RETRYR1				(0x36)
#define FL_CMD_SET_RETRYR2				(0x16)
#define FL_CMD_GET_RETRYR				(0x37)
#elif ((NAND_FLASH==MIC_N58R)||(NAND_FLASH==MIC_N69R)||(NAND_FLASH==KHMB))
#define FL_CMD_ED3_PRO_CMD              (0x10)	/// don't support 0x1A
#define FL_CMD_ED3_PRO2_CMD				(0x10)
#else ////not a valid command in these flash
#define FL_CMD_ED3_PRO_CMD              (0x1A)	///
#define FL_CMD_ED3_PRO2_CMD				(0x10)
#endif

/////////////////Added Flash Analyse Group
#if	(NAND_FLASH==YMTC_X2_6070)		
#define FL_CMD_COARSE_PROG              (0xDC)     // ;YMTC coarse program
#elif(NAND_FLASH==YMTC_X3_6070)
#define FL_CMD_COARSE_PROG              (0xDC)     // ;YMTC coarse program
#elif(NAND_FLASH==YMTC_X4_6080)
#define FL_CMD_COARSE_PROG              (0xDC)     // ;YMTC coarse program
#elif(NAND_FLASH==MIC_N48R || NAND_FLASH==MIC_N58R || NAND_FLASH==MIC_N69R)
#define FL_CMD_COARSE_PROG              (0x0D)     // ;MIC N48R coarse program
#elif(NAND_FLASH==Hynix_3DV7_QLC)
#define FL_CMD_COARSE_PROG              (0x0D)     // ;Hynix 3DV7 QLC coarse program
#elif(NAND_FLASH==SanDisk_BiCS5_QLC)
#define FL_CMD_COARSE_PROG				(0x0D)	   // ;SanDisk BiCS5 QLC coarse program
#elif(NAND_FLASH==SanDisk_BiCS6_QLC)
#define FL_CMD_COARSE_PROG				(0x0D)	   // ;SanDisk BiCS6 QLC coarse program
#elif(NAND_FLASH==SanDisk_BiCS8_QLC)
#define FL_CMD_COARSE_PROG				(0x0D)	   // ;SanDisk BiCS8 QLC coarse program


#endif
////////////////

    

//直接用成CQ组合的命令,可考虑用UDP配置
/*
	宏解释:
	1、FL_CMD_CQ_SLC_MODE--切SLC模式命令
	2、FL_CMD_CQ_TLC_MODE--切TLC模式命令
	3、FL_CMD_CQ_ED3_READ_WL--对于ED3类型Flash,需要发每个Page的Word Line号
	4、FL_CMD_CQ_TLC_MULTI_PLN--在Multi Plane操作之间是否需要发切TLC命令(或者Word Line号)
	5、FL_CMD_MCU_SLC_MODE--MCU方式切SLC模式命令
	6、FL_CMD_MCU_TLC_MODE--MCU方式切TLC模式命令
	7、FL_CMD_MCU_ED3_READ_WL--MCU方式发每个Page的Word Line号
*/
#if (NAND_FLASH == H27QEG8NDB)
	#define FL_CMD_CQ_SLC_MODE(bChan)		     HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xA2))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)  HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | bWlNum))
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)	 (dRow>>24)?FL_CMD_CQ_ED3_READ_WL(bChan,(dRow>>24)):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)		 	 HAL_MCU_SendCmd(bChan,0xA2)
	#define FL_CMD_MCU_TLC_MODE(bChan)		 	 
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) HAL_MCU_SendCmd(bChan,bWlNum)
#elif (NAND_FLASH == MIC_B0KB)
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDA))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDF))
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)  //(dRow>>24)?FL_CMD_CQ_TLC_MODE(bChan):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDA)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) 
#elif(	(NAND_FLASH == MIC_B16A) ||\
		(NAND_FLASH == MIC_B17A) ||\
		(NAND_FLASH == MIC_M16)  ||\
		(NAND_FLASH == MIC_N18)  ||\
		(NAND_FLASH == MIC_N28)  ||\
		(NAND_FLASH == MIC_B27A) ||\
		(NAND_FLASH == MIC_B27B) ||\
		(NAND_FLASH == INTEL_B27B) ||\
		(NAND_FLASH == MIC_M84C) ||\
		(NAND_FLASH == GCGF)||\
		(NAND_FLASH == AFGM)||\
		(NAND_FLASH == SUMSUNG_3DV4)||\
		(NAND_FLASH == YMTC_B08T)||\
		(NAND_FLASH == YMTC_A06M))
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDA))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDF))
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)  //(dRow>>24)?FL_CMD_CQ_TLC_MODE(bChan):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)		 	 HAL_MCU_SendCmd(bChan,0xDA)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum)
#elif (NAND_FLASH == K9ADGD8U0C)
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDA))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDF))
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)  //(dRow>>24)?FL_CMD_CQ_TLC_MODE(bChan):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)		 	 HAL_MCU_SendCmd(bChan,0xDA)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) 
#elif (NAND_FLASH == K9ADGD8U0A)
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDA))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDF))
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)  //(dRow>>24)?FL_CMD_CQ_TLC_MODE(bChan):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)		 	 HAL_MCU_SendCmd(bChan,0xDA)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) 
#elif (NAND_FLASH == MIC_L06B)
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDA))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xDF))
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)  //(dRow>>24)?FL_CMD_CQ_TLC_MODE(bChan):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDA)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) 
#elif (NAND_FLASH == TOS_BICS3)
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xA2))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)  HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | bWlNum))
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)	 (dRow>>24)?FL_CMD_CQ_ED3_READ_WL(bChan,(dRow>>24)):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xA2)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 //HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) HAL_MCU_SendCmd(bChan,bWlNum)
#elif ( (NAND_FLASH == HYNIX_8A1M) ||\
		(NAND_FLASH == HYNIX_3DV4))
	#define FL_CMD_CQ_SLC_MODE(bChan)			 HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | 0xA2))
	#define FL_CMD_CQ_TLC_MODE(bChan)			 
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)  HAL_CQSendInstruct(bChan,(R_CQ_ATTR_CMD | bWlNum))
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)	 (dRow>>24)?FL_CMD_CQ_ED3_READ_WL(bChan,(dRow>>24)):0/*多Plane之间是否需要发TLC前缀命令*/
	#define FL_CMD_MCU_SLC_MODE(bChan)			 HAL_MCU_SendCmd(bChan,0xA2)
	#define FL_CMD_MCU_TLC_MODE(bChan)			 //HAL_MCU_SendCmd(bChan,0xDF)
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum) HAL_MCU_SendCmd(bChan,bWlNum)	
#else //added by flash analys
	#define FL_CMD_CQ_SLC_MODE(bChan)
	#define FL_CMD_CQ_TLC_MODE(bChan)			 
	#define FL_CMD_CQ_ED3_READ_WL(bChan,bWlNum)
	#define FL_CMD_CQ_TLC_MULTI_PLN(bChan,dRow)	
	#define FL_CMD_MCU_SLC_MODE(bChan)	
	#define FL_CMD_MCU_TLC_MODE(bChan)	
	#define FL_CMD_MCU_ED3_READ_WL(bChan,bWlNum)
#endif


/*支持长江Flash的4 Row地址*/
 #if(NAND_FLASH == YMTC_B08T)
	#define HAL_CQSendAddr4th(bChan) 	HAL_CQSendAddr(bChan,0x00)/*YMTC Flash要发4个地址*/
 #else
 	#define HAL_CQSendAddr4th(bChan)
 #endif


										 
 /*Flash 7个Bit状态分别表示的意义,可能对于不同的Flash意义和名字不符合*/
 #define FIO_BITX_STATUS			 (0)
 #define FIO_BIT0_STATUS			 (((U08)1) << 0)/*Bit0:当前编程、擦除操作Pass Or Fail*/
 #define FIO_BIT1_STATUS			 (((U08)1) << 1)/*Bit1:前面一次编程、擦除操作Pass Or Fail*/
 #define FIO_BIT2_STATUS			 (((U08)1) << 2)/*Bit2:编程、擦除操作暂停--或者其他含义*/
 #define FIO_BIT3_STATUS			 (((U08)1) << 3)/*Bit3:保留--或者其他含义*/
 #define FIO_BIT4_STATUS			 (((U08)1) << 4)/*Bit4:保留--或者其他含义*/
 #define FIO_BIT5_STATUS			 (((U08)1) << 5)/*Bit5:Array Ready Or Busy*/
 #define FIO_BIT6_STATUS			 (((U08)1) << 6)/*Bit6:I/O Ready Or Busy*/
 #define FIO_BIT7_STATUS			 (((U08)1) << 7)/*Bit7:写保护状态*/

 
 #define FIO_RB_READY_IO			 FIO_BIT6_STATUS	 
 #define FIO_RB_READY_ARRAY 		 FIO_BIT5_STATUS 
 #define FIO_RB_READY_INIT			 FIO_BIT5_STATUS
 #define FIO_PF_PASS_OTHER			 FIO_BIT0_STATUS 
 #define FIO_PF_PASS_NULL			 FIO_BITX_STATUS 

//不同的flash需要check的bit位是不同的
#if ((NAND_FLASH == K9ADGD8U0C)||(NAND_FLASH == K9ADGD8U0A))
 /*First Check Bit:RB Check*/
 #define FIO_RB_READY_ERASE 		 FIO_BIT5_STATUS	 
 #define FIO_RB_READY_WRITE 		 FIO_BIT5_STATUS	 
 #define FIO_RB_READY_CACHE_WRITE	 FIO_BIT6_STATUS	 
 #define FIO_RB_READY_READ			 FIO_BIT5_STATUS	 
 #define FIO_RB_READY_CACHE_READ	 FIO_BIT6_STATUS	 
 #define FIO_RB_READY_CPYB			 FIO_BIT5_STATUS	 
 /*Second Check Bit:Pass Or Fail(PF)*/
 #define FIO_PF_PASS_SLC_ERASE		 FIO_BITX_STATUS
 #define FIO_PF_PASS_TLC_ERASE		 FIO_BITX_STATUS
 #define FIO_PF_PASS_SLC_WRITE		 FIO_BITX_STATUS
 #define FIO_PF_PASS_SLC_CACHE_WRITE FIO_BITX_STATUS
 #define FIO_PF_PASS_TLC_WRITE		 FIO_BITX_STATUS
 #define FIO_PF_PASS_CPYB			 FIO_BITX_STATUS
 
#define FIO_PF_SLC_CURRENT_PAGE         FIO_BIT2_STATUS/*Cache Program SLC Pass/Fail for current page*/
#define FIO_PF_SLC_PREVIOU_PAGE         FIO_BIT3_STATUS/*Cache Program SLC Pass/Fail for previous page*/ 
#elif (NAND_FLASH == TOS_BICS3)
#ifdef __FLASH_SAND_BICS3__
	/*First Check Bit:RB Check*/
	#define FIO_RB_READY_ERASE 		 		FIO_BIT6_STATUS
	#define FIO_RB_READY_WRITE 		 		FIO_BIT5_STATUS
	#define FIO_RB_READY_CACHE_WRITE	 	FIO_BIT6_STATUS
	#define FIO_RB_READY_READ			 	FIO_BIT5_STATUS
	#define FIO_RB_READY_CACHE_READ	 		FIO_BIT6_STATUS
	#define FIO_RB_READY_CPYB			 	FIO_BIT5_STATUS
	/*Second Check Bit:Pass Or Fail(PF)*/
	#define FIO_PF_PASS_SLC_ERASE		 	FIO_BIT2_STATUS
	#define FIO_PF_PASS_TLC_ERASE		 	FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_WRITE		 	FIO_BIT2_STATUS
	#define FIO_PF_PASS_SLC_CACHE_WRITE 	FIO_BIT3_STATUS/*8T23 Cache Write Bit3*/
	#define FIO_PF_PASS_TLC_WRITE		 	FIO_BIT0_STATUS
	#define FIO_PF_PASS_CPYB			 	FIO_BIT2_STATUS

	#define FIO_PF_SLC_CURRENT_PAGE			FIO_BIT2_STATUS/*Cache Program SLC Pass/Fail for current page*/
	#define FIO_PF_SLC_PREVIOU_PAGE			FIO_BIT3_STATUS/*Cache Program SLC Pass/Fail for previous page*/
#endif

#ifdef __FLASH_TOS_BICS3__
	/*First Check Bit:RB Check*/
	#define FIO_RB_READY_ERASE 		 		FIO_BIT6_STATUS
	#define FIO_RB_READY_WRITE 		 		FIO_BIT6_STATUS
	#define FIO_RB_READY_CACHE_WRITE	 	FIO_BIT6_STATUS
	#define FIO_RB_READY_READ			 	FIO_BIT6_STATUS
	#define FIO_RB_READY_CACHE_READ	 		FIO_BIT6_STATUS
	#define FIO_RB_READY_CPYB			 	FIO_BIT6_STATUS
	/*Second Check Bit:Pass Or Fail(PF)*/
	#define FIO_PF_PASS_SLC_ERASE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_TLC_ERASE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_WRITE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_CACHE_WRITE 	FIO_BIT1_STATUS
	#define FIO_PF_PASS_TLC_WRITE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_CPYB				FIO_BIT0_STATUS

	#define FIO_PF_SLC_CURRENT_PAGE			FIO_BIT0_STATUS/*Cache Program SLC Pass/Fail for current page*/
	#define FIO_PF_SLC_PREVIOU_PAGE			FIO_BIT1_STATUS/*Cache Program SLC Pass/Fail for previous page*/
#endif
#elif (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM) || (NAND_FLASH == GCGF)
  	/*First Check Bit:RB Check*/
	#define FIO_RB_READY_ERASE			FIO_BIT5_STATUS
	#define FIO_RB_READY_WRITE			FIO_BIT5_STATUS
	#define FIO_RB_READY_CACHE_WRITE	FIO_BIT6_STATUS
	#define FIO_RB_READY_READ			FIO_BIT5_STATUS
	#define FIO_RB_READY_CACHE_READ 	FIO_BIT6_STATUS
	#define FIO_RB_READY_CPYB			FIO_BIT5_STATUS
	/*Second Check Bit:Pass Or Fail(PF)*/
	#define FIO_PF_PASS_SLC_ERASE		FIO_BIT0_STATUS
	#define FIO_PF_PASS_TLC_ERASE		FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_WRITE		FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_CACHE_WRITE FIO_BIT1_STATUS
	#define FIO_PF_PASS_TLC_WRITE		FIO_BIT0_STATUS
	#define FIO_PF_PASS_CPYB			FIO_BIT0_STATUS
	#define FIO_PF_SLC_CURRENT_PAGE         FIO_BIT0_STATUS/*Cache Program SLC Pass/Fail for current page*/
	#define FIO_PF_SLC_PREVIOU_PAGE         FIO_BIT1_STATUS/*Cache Program SLC Pass/Fail for previous page*/
#elif (NAND_FLASH == HYNIX_8A1M)
	/*First Check Bit:RB Check*/
	#define FIO_RB_READY_ERASE				FIO_BIT6_STATUS
	#define FIO_RB_READY_WRITE				FIO_BIT6_STATUS
	#define FIO_RB_READY_CACHE_WRITE		FIO_BIT6_STATUS
	#define FIO_RB_READY_READ				FIO_BIT6_STATUS
	#define FIO_RB_READY_CACHE_READ 		FIO_BIT6_STATUS
	#define FIO_RB_READY_CPYB				FIO_BIT5_STATUS
	/*Second Check Bit:Pass Or Fail(PF)*/
	#define FIO_PF_PASS_SLC_ERASE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_TLC_ERASE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_WRITE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_CACHE_WRITE 	FIO_BIT1_STATUS
	#define FIO_PF_PASS_TLC_WRITE			FIO_BIT0_STATUS
	#define FIO_PF_PASS_CPYB				FIO_BIT0_STATUS
	#define FIO_PF_SLC_CURRENT_PAGE         FIO_BIT0_STATUS/*Cache Program SLC Pass/Fail for current page*/
	#define FIO_PF_SLC_PREVIOU_PAGE         FIO_BIT1_STATUS/*Cache Program SLC Pass/Fail for previous page*/

#else
 /*First Check Bit:RB Check*/
 #define FIO_RB_READY_ERASE 		 FIO_BIT5_STATUS	 
 #define FIO_RB_READY_WRITE 		 FIO_BIT5_STATUS	 
 #define FIO_RB_READY_CACHE_WRITE	 FIO_BIT6_STATUS	 
 #define FIO_RB_READY_READ			 FIO_BIT5_STATUS	 
 #define FIO_RB_READY_CACHE_READ	 FIO_BIT6_STATUS
 #define FIO_RB_READY_CPYB			 FIO_BIT5_STATUS	 
 /*Second Check Bit:Pass Or Fail(PF)*/
 #define FIO_PF_PASS_SLC_ERASE		 FIO_BIT0_STATUS
 #define FIO_PF_PASS_TLC_ERASE		 FIO_BIT0_STATUS
 #define FIO_PF_PASS_SLC_WRITE		 FIO_BIT0_STATUS
	#define FIO_PF_PASS_SLC_CACHE_WRITE 	FIO_BIT1_STATUS
 #define FIO_PF_PASS_TLC_WRITE		 FIO_BIT0_STATUS
 #define FIO_PF_PASS_CPYB			 FIO_BIT0_STATUS
 
#define FIO_PF_SLC_CURRENT_PAGE         FIO_BIT0_STATUS/*Cache Program SLC Pass/Fail for current page*/
#define FIO_PF_SLC_PREVIOU_PAGE         FIO_BIT1_STATUS/*Cache Program SLC Pass/Fail for previous page*/ 
#endif

////////////flash analyse group
#define FIO_STAT_WP 					(((U32)1) << 7)
#define FIO_STAT_READY					(((U32)1) << 6)
#define FIO_STAT_TRUE_READY 			(((U32)1) << 5)
#define FIO_STAT_SUSPEND				(((U32)1) << 2)
#define FIO_STAT_PASS					(((U32)1) << 0)
#define FIO_STAT_PASS_PRE_PAGE			(((U32)1) << 1)
#define FIO_STAT_PASS_PLANE0			(((U32)1) << 1)
#define FIO_STAT_PASS_PLANE1			(((U32)1) << 2)
#define FIO_STAT_NULL					(0)
/////////////////////////////////

#if((NAND_FLASH == H27QEG8NDB)	||\
	(NAND_FLASH == MIC_B0KB)	||\
	(NAND_FLASH == K9ADGD8U0C)	||\
	(NAND_FLASH == K9ADGD8U0A)	||\
	(NAND_FLASH == MIC_L06B)	||\
	(NAND_FLASH == TOS_BICS3)	||\
	(NAND_FLASH == YMTC_B08T)	||\
	(NAND_FLASH == MIC_M84C)	||\
	(NAND_FLASH == AFGM)		||\
	(NAND_FLASH == SUMSUNG_3DV4)||\
	(NAND_FLASH == HYNIX_8A1M))
	#define FL_CMD_SNAP_READ		FL_CMD_READ2
#elif(	(NAND_FLASH == MIC_B16A) 	||\
		(NAND_FLASH == MIC_B17A)	||\
		(NAND_FLASH == MIC_B27A)	||\
		(NAND_FLASH == MIC_B27B) 	||\
		(NAND_FLASH == INTEL_B27B)  ||\
		(NAND_FLASH == MIC_M16) 	||\
		(NAND_FLASH == MIC_N18)		||\
		(NAND_FLASH == MIC_N28)		||\
		(NAND_FLASH == HYNIX_3DV4))	||\
		(NAND_FLASH == GCGF)
	#define FL_CMD_SNAP_READ		FL_CMD_READ_SNAP
#else
	#define FL_CMD_SNAP_READ		0xFF
#endif



#if (NAND_FLASH == H27QEG8NDB)
#define  FDL_MODE_CHANGE_CMD		(0xBF)//flashSLC和TLC模式之间切换命令
#define  FDL_SLC_PRE_CMD			(0xA2)
#define  FDL_TLC_PRE_CMD			(0xDE)
#define  FDL_MUTI_PL0_WT_CMD		(0x80)
#define  FDL_MUTI_PL1_WT_CMD		(0x81)
#define  FDL_READ_TLC_SB1_CMD		(0x01)
#define  FDL_READ_TLC_SB2_CMD		(0x02)
#define  FDL_READ_TLC_SB3_CMD		(0x03)
#endif

#define FAIL_ADDR_CHAN_BIT_OFFS  (24)
#define FAIL_ADDR_LUN_BIT_OFFS   (26)
#define FAIL_ADDR_MODE_BIT_OFFS  (28)  //0:slc,1:tlc
#define FAIL_ADDR_ZONE_BIT_OFFS  (30)  //0:tab,1:data

#define FAIL_ADDR_SET(zone,slcmode,chan,lun,row) (((U32)(zone)<<FAIL_ADDR_ZONE_BIT_OFFS)|\
                                                     ((U32)(slcmode)<<FAIL_ADDR_MODE_BIT_OFFS)|\
                                                     ((U32)(chan)<<FAIL_ADDR_CHAN_BIT_OFFS)|\
													 ((U32)(lun)<<FAIL_ADDR_LUN_BIT_OFFS)|\
                                                     (U32)(row&0xFFFFFF))

#ifdef __cplusplus
}
#endif

#endif
