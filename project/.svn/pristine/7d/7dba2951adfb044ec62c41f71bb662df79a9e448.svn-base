#include "fdl_read_retry.h"
#include "fdl_mcu_op.h"
#include "uitility.h"
#include "fdl_exception.h"
#include "fdl_common.h"
#include "fdl_task_op.h"
#include "fdl_soft_decode.h"
#include "Fdl_CopyBack.h"
#include "Hal.h"
#include "Micron_WL_Addr.h"
#include "hal_adm.h"
#ifdef __GRAY_BOX_TEST__
#include "fdl_PValueSet.h"
#endif
T_RETRY_VAR_RECORD	tRetryVar;
T_RETRY_CTRL		tRetryCtrl[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];
__align(8) T_RETRY_INFOR	tRetryInfor;

//G0\G3的锁机制

#ifndef __FDL_MCU_MUTUAL__
volatile  U08 bG0VarLock[MAX_CHAN_OF_DEV];
volatile  U08 bG3VarLock[MAX_CHAN_OF_DEV];
volatile  U08 bG0SetBuffSts;
volatile  U08 bG3SetBuffSts;
#endif

volatile  U08 bRetryPersistence[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN] = {{FALSE},{FALSE}};

U08 bStateMachine[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];	//Retry流程的状态阶段记录
U08 bBestOption[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];		//对应的最优option值，一直保留

#if (NAND_FLASH == MIC_N18) || (NAND_FLASH == MIC_N28)
#define ARC_CYCLE_CNT    (9)
U08 bArcCorect[ARC_CYCLE_CNT] = {0,0,0xF1,0xEC,0x0D,3,6,9,0};
#else
#define ARC_CYCLE_CNT    (6)
U08 bArcCorect[ARC_CYCLE_CNT] = {0,0,0xF1,0xE2,0xf,0};
#endif

//通道簇的异常信息
U08 bClstExcpInfo[MAX_CHAN_OF_DEV][MAX_CHIP_OF_CHAN];
#ifdef __RETRY_CNT_RECORD__
U8_V bFlashInitFail[2][2] = {{0,0},{0,0}};
U08 bPhyToLogChan[4];
U08 bPhyCeToLogLun[4];
#endif

#if (READ_RETRY_ARC_MAX != 0)
#if((NAND_FLASH == MIC_B0KB) ||\
	(NAND_FLASH == MIC_B16A) ||\
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_B27A) ||\
	(NAND_FLASH == MIC_B27B) ||\
	(NAND_FLASH == INTEL_B27B) ||\
	(NAND_FLASH == MIC_M84C))
#define SET_OPTION_ADDR			(0x89)
#define READ_OFFSET_REG_CNT     (11)
U8_C bFlashReadOffsetAddr[READ_OFFSET_REG_CNT] = {0xA0,0xA1,0xA2,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB};	//对应的设置值

#elif (NAND_FLASH == MIC_M16)
#define SET_OPTION_ADDR			(0x89)
#define READ_OFFSET_REG_CNT  (15)
U8_C bFlashReadOffsetAddr[READ_OFFSET_REG_CNT] = {0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xD0,0xD1,0xD2,
													0xD3,0xD4,0xD5,0xD6};	//对应的设置值
#elif (NAND_FLASH == MIC_N18) || (NAND_FLASH == MIC_N28)
#define SET_OPTION_ADDR			(0x89)
#define READ_OFFSET_REG_CNT  (22)
U8_C bFlashReadOffsetAddr[READ_OFFSET_REG_CNT] = {0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD, //13
												0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6};	//对应的设置值
#endif
extern U08 bArcStartOffset;
#endif

#if((NAND_FLASH == MIC_B0KB) ||\
	(NAND_FLASH == MIC_B16A) ||\
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_B27A) ||\
	(NAND_FLASH == MIC_M16)  ||\
	(NAND_FLASH == MIC_N18)  ||\
	(NAND_FLASH == MIC_N28)  ||\
	(NAND_FLASH == MIC_B27B) ||\
	(NAND_FLASH == INTEL_B27B) ||\
	(NAND_FLASH == MIC_M84C))
U8 bSLCReadOffset[READ_RETRY_OFFSET_MAX] = {0xF2, 0xE4, 0xD6, 0xc8, 0x0e, 0x1c, 0x2a, 0x38, 0xba,		//SLC, MLC LP
						0x46, 0xac, 0x54, 0x9e, 0x62, 0x90, 0x70, 0x82, 0x7e
};
U8 bTLCLPReadOffset[READ_RETRY_OFFSET_MAX] = {0xf9, 0xf2, 0xeb, 0xe4, 0x07, 0x0e, 0x15, 0x1c, 0xdd,	//TLC LP, QLC LP
							0x23, 0xd6, 0x2a, 0xcf, 0x31, 0xc8, 0x38, 0xc1, 0x3f,
};

#if(NAND_FLASH == MIC_N28) || (NAND_FLASH == MIC_N18)
U8 bFeatureAddrArray[4] = {0xA4,0x00,0xD3,0xC8};		/*SLC,MLC LP(Null),TLC LP,QLC LP(QLC Flash)*/
#elif(NAND_FLASH == MIC_M16)
U8 bFeatureAddrArray[4] = {0xA4,0x00,0xD3,0xA8};		/*SLC,MLC LP(Null),TLC LP,QLC LP(QLC Flash-Open  QLC)*/
#else
U8 bFeatureAddrArray[4] = {0xA4,0xA1,0xA8,0x00};		/*SLC,MLC LP,TLC LP(TLC Flash)*/
#endif

#endif

#if (NAND_FLASH == K9ADGD8U0A)
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT] = 
{ 
	0xF6,0x00,0xE2,0x00,0xF6,0x00,0x00,0xB0, //00 
	0xE2,0x00,0xEC,0x00,0xE4,0x00,0x00,0xB5, //01 
	0xE2,0x00,0xF6,0x00,0xE6,0x00,0x00,0xBA, //02 
	0xE2,0x00,0x00,0x00,0xE8,0x00,0x00,0xBF, //03 
	0xE2,0x00,0x0A,0x00,0xEA,0x00,0x00,0xC4, //04 
	0xE2,0x00,0x14,0x00,0xEC,0x00,0x00,0xC9, //05 
	0xEC,0x00,0xE2,0x00,0xEE,0x00,0x00,0xCE, //06 
	0xEC,0x00,0xEC,0x00,0xF0,0x00,0x00,0xD3, //07 
	0xEC,0x00,0xF6,0x00,0xF2,0x00,0x00,0xD8, //08 
	0xEC,0x00,0x00,0x00,0xF4,0x00,0x00,0xDD, //09 
	0xEC,0x00,0x0A,0x00,0xF6,0x00,0x00,0xE2, //10 
	0xEC,0x00,0x14,0x00,0xF8,0x00,0x00,0xE7, //11 
	0xF6,0x00,0xE2,0x00,0xFA,0x00,0x00,0xEC, //12 
	0x00,0x00,0xEC,0x00,0x08,0x00,0x00,0x0F, //13 
	0x00,0x00,0xF6,0x00,0x0A,0x00,0x00,0x14, //14 
	0xF6,0x00,0x00,0x00,0x0C,0x00,0x00,0x19, //15 
	0xF6,0x00,0x0A,0x00,0x02,0x00,0x00,0x00, //16 
	0xF6,0x00,0x14,0x00,0x04,0x00,0x00,0x05, //17 
	0x00,0x00,0xE2,0x00,0x06,0x00,0x00,0x0A, //18 
	0x0A,0x00,0xEC,0x00,0xFC,0x00,0x00,0xF1, //19 
	0x00,0x00,0xF6,0x00,0xFE,0x00,0x00,0xF6, //20 
	0xF6,0x00,0x00,0x00,0x00,0x00,0x00,0xFB, //21 
	0x00,0x00,0x0A,0x00,0x0E,0x00,0x00,0x1E, //22 
	0x00,0x00,0x14,0x00,0x10,0x00,0x00,0x23, //23 
	0xF6,0x00,0xE2,0x00,0x12,0x00,0x00,0x28, //24 
	0x0A,0x00,0xEC,0x00,0x14,0x00,0x00,0x2D, //25 
	0x0A,0x00,0xF6,0x00,0x16,0x00,0x00,0x32, //26 
	0x0A,0x00,0x00,0x00,0x18,0x00,0x00,0x37, //27 
	0x0A,0x00,0x0A,0x00,0x1A,0x00,0x00,0x3C, //28 
	0x0A,0x00,0x14,0x00,0x1C,0x00,0x00,0x41, //29 
	0x14,0x00,0xE2,0x00,0x1E,0x00,0x00,0x46, //30	
	0x14,0x00,0xEC,0x00,0x20,0x00,0x00,0x4B, //31	
};  
#endif

#if (NAND_FLASH == TOS_BICS3)
#if 0
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1]=
{
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 0th
	{0x00,0x04,0x07,0x04,0x08,0x02,0x04,0xF8,0x00},	 // 1th
	{0xFC,0x08,0x02,0x08,0x08,0x04,0x08,0xF0,0x00},	 // 2th
	{0xFC,0x04,0x00,0xFC,0x06,0x02,0xFC,0xE8,0x00},	 // 3th
	{0xFE,0x08,0xFB,0xFC,0x06,0xFF,0xFA,0xE0,0x00},	 // 4th
	{0xFE,0x0A,0xF8,0xF8,0x06,0xFE,0xFA,0xD8,0x00},	 // 5th
	{0xFE,0x02,0xF5,0x00,0x04,0x03,0xFC,0xD0,0x00},	 // 6th
	{0xFE,0xFE,0xF2,0x00,0x04,0x02,0xF8,0xC8,0x00},	 // 7th
	{0xFE,0x02,0xEF,0xFC,0x04,0x00,0xFE,0xC0,0x00},	 // 8th
	{0xFE,0xFE,0xEC,0xFC,0x04,0xFC,0xF8,0xB8,0x00},	 // 9th
	{0x00,0x06,0x04,0xFA,0x04,0xFA,0xF4,0xB0,0x00},	 // 10th
	{0x00,0x03,0x02,0xFA,0x02,0x00,0xFD,0x08,0x00},	 // 11th
	{0x00,0x00,0xFD,0xFA,0x02,0xFE,0x00,0x10,0x00},	 // 12th
	{0x00,0x06,0xFA,0xF7,0x02,0xFE,0xFD,0x18,0x00},	 // 13th
	{0x00,0x03,0xF7,0xF8,0x02,0xFC,0xF8,0x00,0x00},	 // 14th
	{0x00,0x00,0xF4,0xF7,0x02,0xFC,0xF4,0xF8,0x00},	 // 15th
	{0x03,0x04,0x02,0xF6,0x02,0xFC,0xF0,0xF0,0x00},	 // 16th
	{0x03,0x02,0x00,0xF6,0x02,0xF8,0xEC,0xE8,0x00},	 // 17th
	{0x03,0x04,0xFD,0xF4,0x02,0xF8,0xE8,0xE0,0x00},	 // 18th
	{0x03,0x02,0xFA,0xF3,0x00,0x00,0xFA,0xD8,0x00},	 // 19th
	{0x03,0x00,0xF7,0xF4,0x00,0x00,0xF8,0xD0,0x00},	 // 20th
	{0x03,0xFE,0xF4,0xF4,0x00,0xFE,0xFC,0xC8,0x00},	 // 21th
	{0x06,0xFC,0x00,0xF4,0x00,0xFC,0xF8,0xC0,0x00},	 // 22th
	{0x06,0x04,0xFD,0xF2,0x00,0xFC,0xF4,0xB8,0x00},	 // 23th
	{0x06,0x00,0xFA,0xF1,0x00,0xFC,0xF0,0xB0,0x00},	 // 24th
	{0x06,0xFE,0xF7,0xF1,0x00,0xF8,0xEC,0x08,0x00},	 // 25th
	{0x06,0xFC,0xF4,0xF1,0x00,0xF8,0xE8,0x10,0x00},	 // 26th
	{0x04,0x02,0xF1,0xF0,0xFE,0xFA,0xF4,0x18,0x00},	 // 27th
	{0x06,0x02,0x02,0xEE,0xFE,0xFA,0xF0,0x00,0x00},	 // 28th
	{0x09,0x00,0x00,0xEE,0xFE,0xFA,0xEC,0xF8,0x00},	 // 29th
	{0x09,0xFE,0xFD,0xEE,0xFE,0xF8,0xF4,0xF0,0x00},	 // 30th
	{0x09,0xFC,0xFA,0xEE,0xFE,0xF8,0xF0,0xE8,0x00},	 // 31th
	{0x09,0x00,0xF7,0xEC,0xFE,0xF5,0xEB,0xE0,0x00},	 // 32th
	{0x09,0xFE,0xF4,0xEB,0xFC,0xF9,0xF0,0xD8,0x00},	 // 33th
	{0x07,0xFC,0xF0,0xEB,0xFC,0xFA,0xF4,0xD0,0x00},	 // 34th
	{0x08,0xFE,0xFC,0xE8,0xFC,0xF7,0xED,0xC8,0x00},	 // 35th
	{0x08,0xFC,0xF8,0xE8,0xFC,0xF4,0xEA,0xC0,0x00},	 // 36th
	{0x08,0xFA,0xEC,0xE6,0xFA,0xFA,0xF8,0xB8,0x00},	 // 37th
	{0x0A,0xFC,0x02,0xE4,0xFA,0xF9,0xF4,0xB0,0x00},	 // 38th
	{0x0A,0xFA,0xF1,0xE3,0xFA,0xF8,0xF0,0x08,0x00},	 // 39th
	{0x0A,0xFA,0xEE,0xE0,0xFA,0xF5,0xF0,0x10,0x00},	 // 40th
	{0x0C,0xF8,0xEC,0xDE,0xFA,0xF5,0xEC,0x18,0x00},	 // 41th
	{0x0C,0xF6,0xEA,0xDC,0xFA,0xF2,0xE8,0x00,0x00},	 // 42th
	{0xF4,0xF8,0xEE,0xF0,0xF6,0xEF,0xF1,0xF8,0x00},	 // 43th
	{0xF2,0xF4,0xEC,0xEC,0xF2,0xEB,0xED,0xF0,0x00},	 // 44th
	{0xEE,0xF2,0xE8,0xE8,0xF0,0xE7,0xE9,0xE8,0x00},	 // 45th
	{0xEC,0xF0,0xE4,0xE4,0xEE,0xE3,0xE5,0xE0,0x00},	 // 46th
	{0xEA,0xEC,0xE0,0xE0,0xEC,0xDF,0xE1,0xD8,0x00},	 // 47th
};
#else
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1]=
{
  /*| 		 89h		  | 	 8Ah		  |8Bh | Rev*/
  /*|A-LSB|C-CSB|E-LSB|G-CSB|B-MSB|D-MSB|F-MSB|SLC | Rev*/
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  /*01th*/
	{0x00, 0x08, 0x04, 0x02, 0x07, 0x04, 0x04, 0xF8, 0x00},  /*02th*/
	{0xFC, 0x08, 0x08, 0x04, 0x02, 0x08, 0x08, 0xF0, 0x00},  /*03th*/
	{0xFC, 0x06, 0x04, 0x02, 0x00, 0xFC, 0xFC, 0xE8, 0x00},  /*04th*/
	{0xFE, 0x06, 0x08, 0xFF, 0xFB, 0xFA, 0xFC, 0xE0, 0x00},  /*05th*/
	{0xFE, 0x04, 0x02, 0x03, 0xF5, 0xFC, 0x00, 0xD0, 0x00},  /*06th*/
	{0xFE, 0x04, 0xFE, 0x02, 0xF2, 0xF8, 0x00, 0xC8, 0x00},  /*07th*/
	{0xFE, 0x04, 0x02, 0x00, 0xEF, 0xFE, 0xFC, 0xC0, 0x00},  /*08th*/
	{0xFE, 0x04, 0xFE, 0xFC, 0xEC, 0xF8, 0xFC, 0xB8, 0x00},  /*09th*/
	{0x00, 0x04, 0x06, 0xFA, 0x04, 0xF4, 0xFA, 0xB0, 0x00},  /*10th*/
	{0x00, 0x02, 0x03, 0x00, 0x02, 0xFD, 0xFA, 0x08, 0x00},  /*11th*/
	{0x00, 0x02, 0x00, 0xFE, 0xFD, 0x00, 0xFA, 0x10, 0x00},  /*12th*/
	{0x00, 0x02, 0x06, 0xFE, 0xFA, 0xFD, 0xF7, 0x18, 0x00},  /*13th*/
	{0x00, 0x02, 0x03, 0xFC, 0xF7, 0xF8, 0xF8, 0x20, 0x00},  /*14th*/
	{0x00, 0x02, 0x00, 0xFC, 0xF4, 0xF4, 0xF7, 0x28, 0x00},  /*15th*/
	{0x03, 0x02, 0x04, 0xFC, 0x02, 0xF0, 0xF6, 0x00, 0x00},  /*16th*/
	{0x03, 0x02, 0x02, 0xF8, 0x00, 0xEC, 0xF6, 0xE8, 0x00},  /*17th*/
	{0x03, 0x02, 0x04, 0xF8, 0xFD, 0xE8, 0xF4, 0xE0, 0x00},  /*18th*/
	{0x03, 0x00, 0x02, 0x00, 0xFA, 0xFA, 0xF3, 0xD8, 0x00},  /*19th*/
	{0x03, 0x00, 0x00, 0x00, 0xF7, 0xF8, 0xF4, 0xD0, 0x00},  /*20th*/
	{0x03, 0x00, 0xFE, 0xFE, 0xF4, 0xFC, 0xF4, 0xC8, 0x00},  /*21th*/
	{0x06, 0x00, 0xFC, 0xFC, 0x00, 0xF8, 0xF4, 0xC0, 0x00},  /*22th*/
	{0x06, 0x00, 0x04, 0xFC, 0xFD, 0xF4, 0xF2, 0xB8, 0x00},  /*23th*/
	{0x06, 0x00, 0x00, 0xFC, 0xFA, 0xF0, 0xF1, 0xB0, 0x00},  /*24th*/
	{0x06, 0x00, 0xFE, 0xF8, 0xF7, 0xEC, 0xF1, 0x08, 0x00},  /*25th*/
	{0x06, 0x00, 0xFC, 0xF8, 0xF4, 0xE8, 0xF1, 0x10, 0x00},  /*26th*/
	{0x04, 0xFE, 0x02, 0xFA, 0xF1, 0xF4, 0xF0, 0x18, 0x00},  /*27th*/
	{0x06, 0xFE, 0x02, 0xFA, 0x02, 0xF0, 0xEE, 0x00, 0x00},  /*28th*/
	{0x09, 0xFE, 0x00, 0xFA, 0x00, 0xEC, 0xEE, 0xF8, 0x00},  /*29th*/
	{0x09, 0xFE, 0xFE, 0xF8, 0xFD, 0xF4, 0xEE, 0xF0, 0x00},  /*30th*/
	{0x09, 0xFE, 0xFC, 0xF8, 0xFA, 0xF0, 0xEE, 0xE8, 0x00},  /*31th*/
	{0x09, 0xFE, 0x00, 0xF5, 0xF7, 0xEB, 0xEC, 0xE0, 0x00},  /*32th*/
	{0x09, 0xFC, 0xFE, 0xF9, 0xF4, 0xF0, 0xEB, 0xD8, 0x00},  /*33th*/
	{0x07, 0xFC, 0xFC, 0xFA, 0xF0, 0xF4, 0xEB, 0xD0, 0x00},  /*34th*/
	{0x08, 0xFC, 0xFE, 0xF7, 0xFC, 0xED, 0xE8, 0xC8, 0x00},  /*35th*/
	{0x08, 0xFC, 0xFC, 0xF4, 0xF8, 0xEA, 0xE8, 0xC0, 0x00},  /*36th*/
	{0x08, 0xFA, 0xFA, 0xFA, 0xEC, 0xF8, 0xE6, 0xB8, 0x00},  /*37th*/
	{0x0A, 0xFA, 0xFC, 0xF9, 0x02, 0xF4, 0xE4, 0xB0, 0x00},  /*38th*/
	{0x0A, 0xFA, 0xFA, 0xF8, 0xF1, 0xF0, 0xE3, 0x08, 0x00},  /*39th*/
	{0x0A, 0xFA, 0xFA, 0xF5, 0xEE, 0xF0, 0xE0, 0x10, 0x00},  /*40th*/
	{0x0C, 0xFA, 0xF8, 0xF5, 0xEC, 0xEC, 0xDE, 0x18, 0x00},  /*41th*/
	{0x0C, 0xFA, 0xF6, 0xF2, 0xEA, 0xE8, 0xDC, 0x00, 0x00},  /*42th*/
	{0xF4, 0xF6, 0xF8, 0xEF, 0xEE, 0xF1, 0xF0, 0xF8, 0x00},  /*43th*/
	{0xF2, 0xF2, 0xF4, 0xEB, 0xEC, 0xED, 0xEC, 0xF0, 0x00},  /*44th*/
	{0xEE, 0xF0, 0xF2, 0xE7, 0xE8, 0xE9, 0xE8, 0xE8, 0x00},  /*45th*/
	{0xEC, 0xEE, 0xF0, 0xE3, 0xE4, 0xE5, 0xE4, 0xE0, 0x00},  /*46th*/
	{0xEA, 0xEC, 0xEC, 0xDF, 0xE0, 0xE1, 0xE0, 0xD8, 0x00},  /*47th*/
	{0xEB, 0xEF, 0xF0, 0xF1, 0xEF, 0xF1, 0xF1, 0x00, 0x00},  /*48th*/
	{0xD7, 0xE9, 0xE6, 0xE6, 0xE1, 0xE2, 0xDE, 0xC8, 0x00},  /*49th*/
	{0xED, 0xEC, 0xEE, 0xED, 0xEC, 0xED, 0xEA, 0xC0, 0x00},  /*50th*/
	{0xD6, 0xEB, 0xE9, 0xE9, 0xE8, 0xEB, 0xE7, 0xB8, 0x00},  /*51th*/
	{0xC2, 0xE9, 0xE9, 0xE6, 0xE7, 0xE8, 0xE2, 0xB0, 0x00},  /*52th*/
	{0xC2, 0xE9, 0xEA, 0xEB, 0xE5, 0xEB, 0xE4, 0x08, 0x00},  /*53th*/
	{0xE8, 0x11, 0x13, 0x10, 0x11, 0x11, 0x16, 0x10, 0x00},  /*54th*/
	{0xE8, 0x16, 0x14, 0x14, 0x0F, 0x11, 0x0B, 0x18, 0x00},  /*55th*/
	{0xEB, 0x0F, 0x0E, 0x0C, 0x0C, 0x0B, 0x06, 0x20, 0x00},  /*56th*/
	{0xED, 0x11, 0x10, 0x12, 0x0A, 0x0D, 0x06, 0x28, 0x00},  /*57th*/
	{0xEB, 0xFB, 0xEA, 0xEC, 0xEE, 0xEC, 0xE9, 0x30, 0x00},  /*58th*/
	{0xF6, 0xFE, 0xF0, 0xEF, 0xF2, 0xEB, 0xED, 0x38, 0x00},  /*59th*/
	{0xFC, 0x0C, 0xFB, 0x02, 0xF7, 0xF2, 0xF2, 0x00, 0x00},  /*60th*/
	{0x00, 0x0E, 0x06, 0x05, 0x04, 0x01, 0x08, 0xF8, 0x00},  /*61th*/
	{0xE9, 0x09, 0x0A, 0x07, 0x04, 0x00, 0xF7, 0xF0, 0x00},  /*62th*/
	{0x08, 0x09, 0x0A, 0x0C, 0x09, 0x0A, 0x07, 0xE8, 0x00},  /*63th*/
	{0x06, 0x07, 0x08, 0x0A, 0x0D, 0x09, 0x06, 0xE0, 0x00},  /*64th*/
	{0x0A, 0x08, 0x0C, 0x08, 0x12, 0x0D, 0x08, 0xD8, 0x00},  /*65th*/
	{0xDE, 0xEF, 0xED, 0xEA, 0xEF, 0xEF, 0xEE, 0x00, 0x00},  /*66th*/
	{0xF1, 0xF6, 0x0E, 0xFD, 0xF7, 0xF5, 0xF6, 0xF8, 0x00},  /*67th*/
	{0xF1, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0E, 0xF0, 0x00},  /*68th*/
	{0xF1, 0x16, 0xE7, 0x12, 0x15, 0x12, 0x10, 0xE8, 0x00},  /*69th*/
	{0x0A, 0x12, 0x10, 0x13, 0x16, 0x11, 0x0C, 0xE0, 0x00},  /*70th*/
};

#endif
const U08 RRT_DECOMPRESS_IDX[15] = {0x00,0x01,0x02,0x03,0x08,
									0x04,0x05,0x06,0x08,0x08,
									0x07,0x08,0x08,0x08,0x08}; 
#endif
#if (NAND_FLASH == GCGF)
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1] =
{  //SLC  ,|         MLC		  |
	{0x00 , 0x00, 0x00, 0x00, 0x00}, // S00
	{0x1E , 0xFC, 0x03, 0xFC, 0xFA}, // S01
	{0xE2 , 0x06, 0x00, 0x06, 0xF6}, // S02
	{0xBA , 0xF6, 0x04, 0xF6, 0x10}, // S03
	{0xA6 , 0x0C, 0xFC, 0x0C, 0x08}, // S04
	{0x9C , 0xF0, 0x0A, 0xF0, 0x18}, // S05
	{0x92 , 0x12, 0xF2, 0x12, 0xFC}, // S06
	{0x88 , 0xEA, 0xF0, 0xEA, 0xF8}, // S07
	{0x46 , 0x18, 0xF7, 0x18, 0x08}, // S08
	{0x5A , 0xE4, 0xFC, 0xE4, 0x10}, // S09
	{0x83 , 0x1E, 0x0A, 0x1E, 0x0A}, // S10
	{0x8D , 0xDE, 0x06, 0xDE, 0x00}, // S11
	{0x64 , 0x24, 0xF8, 0x24, 0xF8}, // S12
	{0xF8 , 0xD8, 0xFE, 0xD8, 0xEC}, // S13
	{0xF0 , 0xD2, 0xFC, 0xD2, 0xE2}, // S14
	{0xE8 , 0x03, 0x00, 0x03, 0xFD}, // S15
	{0xE0 , 0xFE, 0xFC, 0xFE, 0xFA}, // S16
	{0xD8 , 0xFC, 0xFC, 0xFC, 0xF6}, // S17
	{0xD0 , 0xFA, 0xF8, 0xFA, 0xF2}, // S18
	{0xC8 , 0xF7, 0xF8, 0xF7, 0xEE}, // S19
	{0xC0 , 0xCC, 0x16, 0xCC, 0x0A}, // S20
	{0xB8 , 0x2A, 0x04, 0x2A, 0x04}, // S21
	{0xB0 , 0x30, 0xF7, 0x30, 0x04}, // S22
	{0x08 , 0xC6, 0x0C, 0xC6, 0x00}, // S23
	{0x10 , 0xC0, 0xE8, 0xC0, 0xFA}, // S24
	{0x18 , 0x09, 0x12, 0x09, 0x08}, // S25
	{0x20 , 0x00, 0xF4, 0x00, 0xF4}, //1 // GCGD 调试过的RRT
	{0x28 , 0xFA, 0xEE, 0xFD, 0xEE}, //2
	{0x30 , 0x06, 0xE8, 0x03, 0xE8}, //3
	{0x38 , 0xF4, 0x0C, 0xFA, 0x0C}, //4
	{0x40 , 0x0C, 0xFA, 0x06, 0xF8}, //5
	{0x48 , 0xEE, 0xFA, 0xF7, 0xFC}, //6
	{0x50 , 0x12, 0x06, 0x09, 0x04}, //7
	{0xFC , 0xE8, 0x06, 0xF4, 0x08}, //8
	{0xF4 , 0x18, 0xE0, 0x0C, 0xE2}, //9
	{0xEC , 0xE2, 0xF4, 0xF1, 0xF8}, //10
	{0xE4 , 0x1E, 0x0C, 0x0F, 0x08}, //11
	{0xDC , 0xDC, 0xF4, 0xEE, 0xEE}, //12
	{0xD4 , 0x24, 0xFA, 0x12, 0xF4}, //13
	{0xCC , 0xD6, 0xEE, 0xEB, 0xE8}, //14
	{0xC4 , 0x2A, 0xEE, 0x15, 0xF4}, //15
	{0xBC , 0xD0, 0xE8, 0xE8, 0xE2}, //16
	{0xB4 , 0x30, 0xE8, 0x18, 0xEE}, //17
	{0x04 , 0xCA, 0x06, 0xE5, 0x0C}, //18
	{0x0C , 0x36, 0x12, 0x1B, 0x0C}, //19
	{0x14 , 0xC4, 0xF4, 0xE2, 0xFC}, //20
	{0x1C , 0x3C, 0xE0, 0x1E, 0xE8}, //21
	{0x24 , 0xBE, 0x0C, 0xDF, 0x04}, //22
	{0x2C , 0x42, 0xFA, 0x21, 0x04}, //23
	{0x34 , 0xB8, 0xEE, 0xDC, 0xF8}, //24
	{0x3C , 0x48, 0x06, 0x24, 0xFC}, //25
	{0x44 , 0xB2, 0x12, 0xD9, 0x08}, //26
	{0x4C , 0x4E, 0xD8, 0x27, 0xE2}, //27
	{0xFE , 0xAC, 0xF4, 0xD6, 0xE8}, //28
	{0xF6 , 0x54, 0xFA, 0x2A, 0xEE}, //29
	{0xEE , 0xA6, 0xEE, 0xD3, 0xE2}, //30
	{0xE6 , 0x5A, 0xE8, 0x2D, 0xF4}, //31
	{0xDE , 0x60, 0xFA, 0x30, 0x08}, //32
	{0xD6 , 0x00, 0xEE, 0x00, 0xFC}, //33
	{0xCE , 0xFA, 0x06, 0xFD, 0xF8}, //34
	{0xC6 , 0x06, 0xE0, 0x03, 0xEE}, //35
	{0xBE , 0xF4, 0x12, 0xFA, 0x04}, //36
	{0xB6 , 0x0C, 0xF4, 0x06, 0x04}, //37
	{0x02 , 0xEE, 0xE8, 0xF7, 0xF8}, //38
	{0x0A , 0x12, 0x0C, 0x09, 0xFC}, //39
	{0x12 , 0xE8, 0xD8, 0xF4, 0xE8}, //40
	{0x1A , 0x18, 0xF4, 0x0C, 0xE2}, //41
	{0x22 , 0xE2, 0xFA, 0xF1, 0xE8}, //42
	{0x2A , 0x1E, 0xFA, 0x0F, 0x0C}, //43
	{0x32 , 0xDC, 0x06, 0xEE, 0xF4}, //44
	{0x3A , 0x24, 0xF4, 0x12, 0x08}, //45
	{0x42 , 0xD6, 0xE8, 0xEB, 0xFC}, //46
	{0x4A , 0x2A, 0xE0, 0x15, 0xF4}, //47
	{0xFF , 0xD0, 0x0C, 0xE8, 0xF8}, //48
	{0xF7 , 0x30, 0xEE, 0x18, 0x04}, //49
	{0xEF , 0xCA, 0x12, 0xE5, 0xFC}, //50
	{0xE7 , 0x36, 0xD8, 0x1B, 0xEE}, //51
	{0xDF , 0xC4, 0xF4, 0xE2, 0x0C}, //52
	{0xD7 , 0x3C, 0xFA, 0x1E, 0xE2}, //53
	{0xCF , 0xBE, 0x06, 0xDF, 0xEE}, //54
	{0xC7 , 0x42, 0xE0, 0x21, 0xF8}, //55
	{0xBF , 0xB8, 0x0C, 0xDC, 0xF4}, //56
	{0xB7 , 0x48, 0xEE, 0x24, 0x08}, //57
	{0x01 , 0xB2, 0x12, 0xD9, 0xF8}, //58
	{0x09 , 0x4E, 0xE8, 0x27, 0x04}, //59
	{0x11 , 0xAC, 0xE0, 0xD6, 0xFC}, //60
	{0x19 , 0x54, 0xD8, 0x2A, 0xF4}, //61
	{0x21 , 0xA6, 0xEE, 0xD3, 0x0C}, //62
	{0x29 , 0x5A, 0x06, 0x2D, 0xE8}, //63
	{0x31 , 0x60, 0x0C, 0x30, 0xEE}, //64
	{0x39 , 0x00, 0x12, 0x00, 0xF4}, //65
	{0x41 , 0xFA, 0xE8, 0xFD, 0x08}, //66
	{0x49 , 0x06, 0xD8, 0x03, 0xF8}, //67
	{0xFD , 0xF4, 0xE8, 0xFA, 0x0C}, //68
	{0xF5 , 0x0C, 0x06, 0x06, 0xE2}, //69
	{0xED , 0xEE, 0xE0, 0xF7, 0x04}, //70
	{0xE5 , 0x12, 0x0C, 0x09, 0xE8}, //71
	{0xDD , 0xE8, 0x12, 0xF4, 0xEE}, //72
	{0xD5 , 0x18, 0xD8, 0x0C, 0xFC}, //73
	{0xCD , 0xE2, 0xE0, 0xF1, 0x08}, //74
	{0xC5 , 0x1E, 0x18, 0x0F, 0x18}, //75
	{0xBD , 0xDC, 0x18, 0xEE, 0x12}, //76
	{0xB5 , 0x24, 0x20, 0x12, 0x18}, //77
	{0x03 , 0xD6, 0x18, 0xEB, 0x0C}, //78
	{0x0B , 0x2A, 0x20, 0x15, 0x12}, //79
	{0x13 , 0xD0, 0x18, 0xE8, 0x08}, //80
	{0x1B , 0x30, 0x28, 0x18, 0x18}, //81
	{0x23 , 0xCA, 0x18, 0xE5, 0x04}, //82
	{0x2B , 0x36, 0x20, 0x1B, 0x0C}, //83
	{0x33 , 0xC4, 0x28, 0xE2, 0x12}, //84
	{0x3B , 0x3C, 0x20, 0x1E, 0x08}, //85
	{0x43 , 0xBE, 0x18, 0xDF, 0xFC}, //86
	{0x00 , 0x42, 0x20, 0x21, 0x04}, //87
	{0x00 , 0xB8, 0x28, 0xDC, 0x0C}, //88
	{0x00 , 0x48, 0x18, 0x24, 0xF8}, //89
	{0x00 , 0xB2, 0x28, 0xD9, 0x08}, //90
	{0x00 , 0x4E, 0x18, 0x27, 0xF4}, //91
	{0x00 , 0xAC, 0x20, 0xD6, 0xFC}, //92
	{0x00 , 0x54, 0x28, 0x2A, 0x04}, //93
	{0x00 , 0xA6, 0x20, 0xD3, 0xF8}, //94
	{0x00 , 0x5A, 0x12, 0x2D, 0x12}, //95
	{0x00 , 0x60, 0x0C, 0x30, 0x12}, //96
	{0x00 , 0x00, 0x12, 0x00, 0x18}, //97
	{0x00 , 0xFA, 0x06, 0xFD, 0x12}, //98
	{0x00 , 0x06, 0x0C, 0x03, 0x18}, //99
	{0x00 , 0xF4, 0x06, 0xFA, 0x18}, //100
	{0x00 , 0x0C, 0xFA, 0x06, 0x12}, //101
	{0x00 , 0xEE, 0xF4, 0xF7, 0x12}, //102
	{0x00 , 0x12, 0xFA, 0x09, 0x18}, //103
	{0x00 , 0xE8, 0xF4, 0xF4, 0x18}, //104
	{0x00 , 0x18, 0xEE, 0x0C, 0x12}  //105
	//SLC最后为0是补的数据
};
#endif
#if (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM)
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1]=
{
#ifdef _TLC_ORDER_PROGRAM_
	//SLC,MLC-R1,MLC-R2,MLC-R3,TLC-R1,TLC-R2,TLC-R3,TLC-R4,TLC-R5,TLC-R6,TLC-R7
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//s00
	{0xB0,0xF6,0xF6,0xE2,0xE6,0xFB,0xF2,0xEE,0xE9,0xE9,0xDD},//s01
	{0xB5,0xE2,0xE4,0xEC,0xE6,0xFD,0xF2,0xEE,0xEE,0xEF,0xE4},//s02
	{0xBA,0xE2,0xE6,0xF6,0xE6,0xFF,0xF2,0xEE,0xF3,0xF5,0xEB},//s03
	{0xBF,0xE2,0xE8,0x00,0xEA,0xFB,0xF5,0xF2,0xE9,0xE9,0xDD},//s04
	{0xC4,0xE2,0xEA,0x0A,0xEA,0xFD,0xF5,0xF2,0xEE,0xEF,0xE4},//s05
	{0xC9,0xE2,0xEC,0x14,0xEA,0xFF,0xF5,0xF2,0xF3,0xF5,0xEB},//s06
	{0xCE,0xEC,0xEE,0xE2,0xEE,0xFB,0xF8,0xF6,0xE9,0xE9,0xDD},//s07
	{0xD3,0xEC,0xF0,0xEC,0xEE,0xFD,0xF8,0xF6,0xEE,0xEF,0xE4},//s08
	{0xD8,0xEC,0xF2,0xF6,0xEE,0xFF,0xF8,0xF6,0xF3,0xF5,0xEB},//s09
	{0xDD,0xEC,0xF4,0x00,0xEC,0xF2,0xEB,0xE4,0xE3,0xE0,0xD7},//s10
	{0xE2,0xEC,0xF6,0x0A,0xEC,0xF4,0xEB,0xE4,0xE8,0xE6,0xDE},//s11
	{0xE7,0xEC,0xF8,0x14,0xEC,0xF6,0xEB,0xE4,0xED,0xEC,0xE5},//s12
	{0xEC,0xF6,0xFA,0xE2,0xF0,0xF2,0xEE,0xE8,0xE3,0xE0,0xD7},//s13
	{0x0F,0x00,0x08,0xEC,0xF0,0xF4,0xEE,0xE8,0xE8,0xE6,0xDE},//s14
	{0x14,0x00,0x0A,0xF6,0xF0,0xF6,0xEE,0xE8,0xED,0xEC,0xE5},//s15
	{0x19,0xF6,0x0C,0x00,0xF4,0xF2,0xF1,0xEC,0xE3,0xE0,0xD7},//s16
	{0x00,0xF6,0x02,0x0A,0xF4,0xF4,0xFB,0xEC,0xE8,0xE6,0xF1},//s17
	{0x05,0xF6,0x04,0x14,0xF4,0xF6,0x0A,0xEC,0xED,0xEC,0x14},//s18
	{0x0A,0x00,0x06,0xE2,0xEE,0x02,0xFC,0xFD,0xFF,0x03,0xFB},//s19
	{0xF1,0x0A,0xFC,0xEC,0xE8,0xEE,0xEE,0xF2,0xF2,0xF2,0xF6},//s20
	{0xF6,0x00,0xFE,0xF6,0xF2,0xFE,0xFD,0xFC,0xF8,0xF8,0xF8},//s21
	{0xFB,0xF6,0x00,0x00,0xF4,0x00,0xFD,0xFC,0xF8,0xFA,0xF8},//s22
	{0x1E,0x00,0x0E,0x0A,0xF2,0xFF,0xFC,0x02,0x02,0x06,0x02},//s23
	{0x23,0x00,0x10,0x14,0xF3,0x01,0x00,0x01,0x03,0x06,0x04},//s24
	{0x28,0xF6,0x12,0xE2,0xFB,0xFF,0xFB,0xFD,0x00,0x02,0x00},//s25
	{0x2D,0x0A,0x14,0xEC,0xF6,0x03,0xFF,0x01,0x04,0x08,0x02},//s26
	{0x32,0x0A,0x16,0xF6,0xE6,0xFE,0xF6,0xF4,0xEF,0xF0,0xE7},//s27
	{0x37,0x0A,0x18,0x00,0x00,0x00,0x04,0x08,0x0E,0x10,0x14},//s28
	{0x3C,0x0A,0x1A,0x0A,0x00,0x0D,0x0D,0x13,0x13,0x1A,0x1A},//s29
	{0x41,0x0A,0x1C,0x14,0x0A,0x06,0x06,0x08,0x06,0x08,0x08},//s30
	{0x46,0x14,0x1E,0xE2,0x0C,0x0D,0x0E,0x0F,0x0F,0x0F,0x10},//s31
	{0x4B,0x14,0x20,0xEC,0x14,0x0A,0x00,0xFB,0xF6,0xF1,0xEC},//s32
	{0xF1,0xF1,0x0F,0x14,0xE1,0x0D,0x08,0x0B,0x08,0x0C,0x06},//s33
	{0xF1,0xF1,0x0A,0x1A,0xF1,0x04,0x00,0x00,0x01,0x07,0x07},//s34
	{0xF1,0xF1,0x0F,0x14,0xEE,0x0B,0x08,0x10,0x07,0x0C,0xF7},//s35
	{0xF1,0xF1,0x0F,0x14,0xF7,0x0A,0x08,0x0E,0x08,0x01,0x04},//s36
	{0xF1,0xF1,0x0F,0x14,0xED,0x0A,0x08,0x07,0x01,0x0B,0x05},//s37
	{0xF1,0xF1,0x0A,0x1A,0xD8,0x04,0xFF,0xF6,0xFD,0x01,0xFE},//s38
	{0xF1,0xF1,0x0A,0x1A,0xDE,0x01,0x00,0xFC,0xF7,0x03,0x06},//s39
	{0xF1,0xF1,0x0A,0x1A,0xE1,0x01,0x00,0xFA,0xFB,0x01,0xF5},//s40
	{0xB0,0xF6,0xF6,0xE2,0xFC,0xFE,0xFC,0xF8,0xFC,0xEE,0xE8},//s41
	{0xB5,0xE2,0xE4,0xEC,0xFA,0xFE,0xFC,0xF8,0xFC,0xEC,0xE6},//s42
	{0xBA,0xE2,0xE6,0xF6,0xF8,0xFE,0xFC,0xF8,0xFC,0xEA,0xE4},//s43
	{0xBF,0xE2,0xE8,0x00,0xF6,0xFE,0xFC,0xF8,0xFC,0xE8,0xE2},//s44
	{0xC4,0xE2,0xEA,0x0A,0xF4,0xFE,0xFC,0xF8,0xFC,0xE6,0xE0},//s45
	{0xC9,0xE2,0xEC,0x14,0xF2,0xFE,0xFC,0xF6,0xFC,0xEE,0xDE},//s46
	{0xCE,0xEC,0xEE,0xE2,0xF0,0xFE,0xF8,0xF6,0xFC,0xEC,0xE8},//s47
	{0xD3,0xEC,0xF0,0xEC,0xFC,0xFE,0xF8,0xF6,0xFA,0xEA,0xE6},//s48
	{0xD8,0xEC,0xF2,0xF6,0xFA,0xFE,0xF8,0xF6,0xFA,0xE8,0xE4},//s49
	{0xDD,0xEC,0xF4,0x00,0xF8,0xFE,0xF8,0xF6,0xFA,0xE6,0xE2},//s50
	{0xE2,0xEC,0xF6,0x0A,0xF6,0xFE,0xF8,0xF4,0xFA,0xEE,0xE0},//s51
	{0xE7,0xEC,0xF8,0x14,0xF4,0xFE,0xF8,0xF4,0xFA,0xEC,0xDE},//s52
	{0xEC,0xF6,0xFA,0xE2,0xF2,0xFE,0xF6,0xF4,0xFA,0xEA,0xE8},//s53
	{0x0F,0x00,0x08,0xEC,0xF0,0xFE,0xF6,0xF4,0xFA,0xE8,0xE6},//s54
	{0x14,0x00,0x0A,0xF6,0xFC,0xFE,0xF6,0xF4,0xF8,0xE6,0xE4},//s55
	{0x19,0xF6,0x0C,0x00,0xFA,0xFE,0xF6,0xF2,0xF8,0xEE,0xE2},//s56
	{0x00,0xF6,0x02,0x0A,0xF8,0xFE,0xF6,0xF2,0xF8,0xEC,0xE0},//s57
	{0x05,0xF6,0x04,0x14,0xF6,0xFE,0xF6,0xF2,0xF8,0xEA,0xDE},//s58
	{0x0A,0x00,0x06,0xE2,0xF4,0xFE,0xF4,0xF2,0xF8,0xE8,0xE8},//s59
	{0xF1,0x0A,0xFC,0xEC,0xF2,0xFE,0xF4,0xF2,0xF8,0xE6,0xE6},//s60
	{0xF6,0x00,0xFE,0xF6,0xF0,0x00,0xF4,0xF8,0xF8,0xEE,0xE4},//s61
	{0xFB,0xF6,0x00,0x00,0xFC,0x00,0xF4,0xF8,0xF6,0xEC,0xE2},//s62
	{0x1E,0x00,0x0E,0x0A,0xFA,0x00,0xF4,0xF8,0xF6,0xEA,0xE0},//s63
	{0x23,0x00,0x10,0x14,0xF8,0x00,0xF4,0xF8,0xF6,0xE8,0xDE},//s64
	{0x28,0xF6,0x12,0xE2,0xF6,0x00,0xFC,0xF8,0xF6,0xE6,0xDC},//s65
	{0x2D,0x0A,0x14,0xEC,0xF4,0x00,0xFA,0xF6,0xF6,0xEE,0xDC},//s66
	{0x32,0x0A,0x16,0xF6,0xF2,0x00,0xF8,0xF6,0xF6,0xEC,0xDC},//s67
	{0x37,0x0A,0x18,0x00,0xF0,0x00,0xF6,0xF6,0xF6,0xEA,0xDC},//s68
	{0x3C,0x0A,0x1A,0x0A,0xFC,0x00,0xF4,0xF6,0xF4,0xE8,0xDC},//s69
	{0x41,0x0A,0x1C,0x14,0xFA,0x00,0xFC,0xF6,0xF4,0xE6,0xEA},//s70
	{0xB0,0xF6,0xF6,0xE2,0xF8,0x00,0xFA,0xF4,0xF4,0xEE,0xEA},//s71
	{0xB5,0xE2,0xE4,0xEC,0xF6,0x00,0xF8,0xF4,0xF4,0xEC,0xEA},//s72
	{0xBA,0xE2,0xE6,0xF6,0xF4,0x00,0xF6,0xF4,0xF4,0xEA,0xEA},//s73
	{0xBF,0xE2,0xE8,0x00,0xF2,0x00,0xF4,0xF4,0xF4,0xE8,0xEA},//s74
	{0xC4,0xE2,0xEA,0x0A,0xF0,0x00,0xFC,0xF4,0xF4,0xE6,0xEC},//s75
	{0xC9,0xE2,0xEC,0x14,0xFC,0x00,0xFA,0xF2,0xF2,0xEE,0xEC},//s76
	{0xCE,0xEC,0xEE,0xE2,0xFA,0x00,0xF8,0xF2,0xF2,0xEC,0xEC},//s77
	{0xD3,0xEC,0xF0,0xEC,0xF8,0x00,0xF6,0xF2,0xF2,0xEA,0xEC},//s78
	{0xD8,0xEC,0xF2,0xF6,0xF6,0x00,0xF4,0xF2,0xF2,0xE8,0xEC},//s79
	{0xDD,0xEC,0xF4,0x00,0xF4,0x00,0xFC,0xF2,0xF2,0xE6,0xDA},//s80
	{0xE2,0xEC,0xF6,0x0A,0xF2,0x02,0xFA,0xF8,0xF2,0xEE,0xDA},//s81
	{0xE7,0xEC,0xF8,0x14,0xF0,0x02,0xF8,0xF8,0xF2,0xEC,0xDA},//s82
	{0xEC,0xF6,0xFA,0xE2,0xFC,0x02,0xF6,0xF8,0xF0,0xEA,0xDA},//s83
	{0x0F,0x00,0x08,0xEC,0xFA,0x02,0xF4,0xF8,0xF0,0xE8,0xDA},//s84
	{0x14,0x00,0x0A,0xF6,0xF8,0x02,0xFC,0xF8,0xF0,0xE6,0xD8},//s85
	{0x19,0xF6,0x0C,0x00,0xF6,0x02,0xFA,0xF6,0xF0,0xEE,0xD8},//s86
	{0x00,0xF6,0x02,0x0A,0xF4,0x02,0xF8,0xF6,0xF0,0xEC,0xD8},//s87
	{0x05,0xF6,0x04,0x14,0xF2,0x02,0xF6,0xF6,0xF0,0xEA,0xD8},//s88
	{0x0A,0x00,0x06,0xE2,0xF0,0x02,0xF4,0xF6,0xF0,0xE8,0xD8},//s89
	{0xF1,0x0A,0xFC,0xEC,0x00,0x02,0x00,0xF6,0x00,0xE6,0x00},//s90
	{0xF6,0x00,0xFE,0xF6,0x00,0x02,0x00,0xF4,0x00,0xEE,0x00}//s91
#else
	//SLC,MLC-R1,MLC-R2,MLC-R3,TLC-R1,TLC-R2,TLC-R3,TLC-R4,TLC-R5,TLC-R6,TLC-R7无效
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//S00
	{0xB0, 0xF6, 0xF6, 0xE2, 0xE6, 0xFB, 0xF2, 0xEE, 0xE9, 0xE9, 0xDD, 0x00},//S01
	{0xB5, 0xE2, 0xE4, 0xEC, 0xE6, 0xFD, 0xF2, 0xEE, 0xEE, 0xEF, 0xE4, 0x00},//S02
	{0xBA, 0xE2, 0xE6, 0xF6, 0xE6, 0xFF, 0xF2, 0xEE, 0xF3, 0xF5, 0xEB, 0x00},//S03
	{0xBF, 0xE2, 0xE8, 0x00, 0xEA, 0xFB, 0xF5, 0xF2, 0xE9, 0xE9, 0xDD, 0x00},//S04
	{0xC4, 0xE2, 0xEA, 0x0A, 0xEA, 0xFD, 0xF5, 0xF2, 0xEE, 0xEF, 0xE4, 0x00},//s05
	{0xC9, 0xE2, 0xEC, 0x14, 0xEA, 0xFF, 0xF5, 0xF2, 0xF3, 0xF5, 0xEB, 0x00},//S06
	{0xCE, 0xEC, 0xEE, 0xE2, 0xEE, 0xFB, 0xF8, 0xF6, 0xE9, 0xE9, 0xDD, 0x00},//S07
	{0xD3, 0xEC, 0xF0, 0xEC, 0xEE, 0xFD, 0xF8, 0xF6, 0xEE, 0xEF, 0xE4, 0x00},//S08
	{0xD8, 0xEC, 0xF2, 0xF6, 0xEE, 0xFF, 0xF8, 0xF6, 0xF3, 0xF5, 0xEB, 0x00},//S09
	{0xDD, 0xEC, 0xF4, 0x00, 0xEC, 0xF2, 0xEB, 0xE4, 0xE3, 0xE0, 0xD7, 0x00},//S10
	{0xE2, 0xEC, 0xF6, 0x0A, 0xEC, 0xF4, 0xEB, 0xE4, 0xE8, 0xE6, 0xDE, 0x00},//S11
	{0xE7, 0xEC, 0xF8, 0x14, 0xEC, 0xF6, 0xEB, 0xE4, 0xED, 0xEC, 0xE5, 0x00},//S12
	{0xEC, 0xF6, 0xFA, 0xE2, 0xF0, 0xF2, 0xEE, 0xE8, 0xE3, 0xE0, 0xD7, 0x00},//S13
	{0x0F, 0x00, 0x08, 0xEC, 0xF0, 0xF4, 0xEE, 0xE8, 0xE8, 0xE6, 0xDE, 0x00},//S14
	{0x14, 0x00, 0x0A, 0xF6, 0xF0, 0xF6, 0xEE, 0xE8, 0xED, 0xEC, 0xE5, 0x00},//S15
	{0x19, 0xF6, 0x0C, 0x00, 0xF4, 0xF2, 0xF1, 0xEC, 0xE3, 0xE0, 0xD7, 0x00},//S16
	{0x00, 0xF6, 0x02, 0x0A, 0xF4, 0xF4, 0xFB, 0xEC, 0xE8, 0xE6, 0xF1, 0x00},//S17
	{0x05, 0xF6, 0x04, 0x14, 0xF4, 0xF6, 0x0A, 0xEC, 0xED, 0xEC, 0x14, 0x00},//S18
	{0x0A, 0x00, 0x06, 0xE2, 0xEE, 0x02, 0xFC, 0xFD, 0xFF, 0x03, 0xFB, 0x00},//S19
	{0xF1, 0x0A, 0xFC, 0xEC, 0xE8, 0xEE, 0xEE, 0xF2, 0xF2, 0xF2, 0xF6, 0x00},//S20
	{0xF6, 0x00, 0xFE, 0xF6, 0xF2, 0xFE, 0xFD, 0xFC, 0xF8, 0xF8, 0xF8, 0x00},//S21
	{0xFB, 0xF6, 0x00, 0x00, 0xF4, 0x00, 0xFD, 0xFC, 0xF8, 0xFA, 0xF8, 0x00},//S22
	{0x1E, 0x00, 0x0E, 0x0A, 0xF2, 0xFF, 0xFC, 0x02, 0x02, 0x06, 0x04, 0x00},//S23
	{0x23, 0x00, 0x10, 0x14, 0xF3, 0x01, 0x00, 0x01, 0x03, 0x06, 0x04, 0x00},//S24
	{0x28, 0xF6, 0x12, 0xE2, 0xFB, 0xFF, 0xFB, 0xFD, 0x00, 0x02, 0x00, 0x00},//S25
	{0x2D, 0x0A, 0x14, 0xEC, 0xF6, 0x03, 0xFF, 0x01, 0x04, 0x08, 0x02, 0x00},//S26
	{0x32, 0x0A, 0x16, 0xF6, 0xE6, 0xFE, 0xF6, 0xF4, 0xEF, 0xF0, 0xE7, 0x00},//S27
	{0x37, 0x0A, 0x18, 0x00, 0x00, 0x00, 0x04, 0x08, 0x0E, 0x10, 0x14, 0x00},//S28
	{0x3C, 0x0A, 0x1A, 0x0A, 0x00, 0x0D, 0x0D, 0x13, 0x13, 0x1A, 0x1A, 0x00},//S29
	{0x41, 0x0A, 0x1C, 0x14, 0x0A, 0x06, 0x06, 0x08, 0x06, 0x08, 0x08, 0x00},//S30
	{0x46, 0x14, 0x1E, 0xE2, 0x0C, 0x0D, 0x0E, 0x0F, 0x0F, 0x0F, 0x10, 0x00},//S31
	{0x4B, 0x14, 0x20, 0xEC, 0x14, 0x0A, 0x00, 0xFB, 0xF6, 0xF1, 0xEC, 0x00},//S32
	{0xF1, 0xF1, 0x0F, 0x14, 0xE1, 0x0D, 0x08, 0x0B, 0x08, 0x0C, 0x06, 0x00},//S33
	{0xF1, 0xF1, 0x0A, 0x1A, 0xF1, 0x04, 0x00, 0x00, 0x01, 0x07, 0x07, 0x00},//S34
	{0xF1, 0xF1, 0x0F, 0x14, 0xEE, 0x0B, 0x08, 0x10, 0x07, 0x0C, 0xF7, 0x00},//S35
	{0xF1, 0xF1, 0x0F, 0x14, 0xF7, 0x0A, 0x08, 0x0E, 0x08, 0x01, 0x04, 0x00},//S36
	{0xF1, 0xF1, 0x0F, 0x14, 0xED, 0x0A, 0x08, 0x07, 0x01, 0x0B, 0x05, 0x00},//S37
	{0xF1, 0xF1, 0x0A, 0x1A, 0xD8, 0x04, 0xFF, 0xF6, 0xFD, 0x01, 0xFE, 0x00},//S38
	{0xF1, 0xF1, 0x0A, 0x1A, 0xDE, 0x01, 0x00, 0xFC, 0xF7, 0x03, 0x06, 0x00},//S39
	{0xF1, 0xF1, 0x0A, 0x1A, 0xE1, 0x01, 0x00, 0xFA, 0xFB, 0x01, 0xF5, 0x00}//S40
#endif

};
//0X0B代表无效
const U08 RRT_DECOMPRESS_IDX[20] = {0x00,0x0B,0x0B,0x0B,//SLC
									0x01,0x0B,0x03,0x0B,//MLC RP0,RP2
									0x02,0x0B,0x0B,0x0B,//MLC RP1,
									0x04,0x08,0x06,0x0A,//TLC
									0x05,0x07,0x09,0x0B};//TLC

#endif

#if (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1]=
{
   /*LP-R1 LP-R5 MP-R2 MP-R4 MP-R6 UP-R3 UP-R7 SLC-P NULL*/
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},//0th
	{0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF7, 0xF7, 0xF8, 0x00,},//1th
	{0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF4, 0xF4, 0xF0, 0x00,},//2th
	{0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x09, 0x09, 0xE8, 0x00,},//3th
	{0x08, 0x08, 0x10, 0x10, 0x10, 0x0C, 0x0C, 0xE0, 0x00,},//4th
	{0xFE, 0x04, 0x00, 0xFA, 0xF7, 0xF0, 0xF0, 0xD8, 0x00,},//5th
	{0x00, 0x06, 0x00, 0x06, 0x09, 0x10, 0x10, 0xD0, 0x00,},//6th
	{0xFE, 0x02, 0xFD, 0x03, 0x06, 0xFE, 0x00, 0xC8, 0x00,},//7th
	{0x00, 0x08, 0x03, 0xFD, 0xFA, 0x02, 0xFE, 0xC0, 0x00,},//8th
	{0xFE, 0x00, 0xFD, 0x00, 0x06, 0xFE, 0xFE, 0xB8, 0x00,},//9th
	{0x03, 0xFA, 0x03, 0x00, 0xFA, 0x02, 0x00, 0xB0, 0x00,},//10th
	{0xFE, 0xFE, 0xFD, 0x00, 0x03, 0xFE, 0xFC, 0x08, 0x00,},//11th
	{0x03, 0xFD, 0x03, 0x00, 0xFD, 0x02, 0x02, 0x10, 0x00,},//12th
	{0xFE, 0xFC, 0xFD, 0xFD, 0xFD, 0xFE, 0xFA, 0x18, 0x00,},//13th
	{0x03, 0x00, 0x03, 0x03, 0x03, 0x02, 0x04, 0x20, 0x00,},//14th
	{0xFE, 0xFA, 0xFD, 0xFA, 0xF7, 0xFD, 0x06, 0x28, 0x00,},//15th
	{0x03, 0x03, 0x03, 0x06, 0x09, 0xFD, 0x03, 0x30, 0x00,},//16th
	{0xFE, 0xF8, 0xFD, 0xFA, 0xF4, 0x03, 0xFA, 0x38, 0x00,},//17th
	{0x03, 0x06, 0x03, 0x06, 0x0C, 0xFD, 0x00, 0x40, 0x00,},//18th
	{0xFC, 0x04, 0xFD, 0xF7, 0xF4, 0x03, 0xFD, 0x48, 0x00,},//19th
	{0x03, 0x09, 0x03, 0x09, 0x0C, 0xFD, 0xFD, 0x50, 0x00,},//20th
	{0xFC, 0x02, 0xFA, 0x00, 0x03, 0x03, 0x00, 0xFC, 0x00,},//21th
	{0x03, 0x0C, 0x06, 0x00, 0xFD, 0xFD, 0xFA, 0xF4, 0x00,},//22th
	{0xFC, 0x00, 0xFA, 0xFD, 0x03, 0x03, 0x03, 0xEC, 0x00,},//23th
	{0x06, 0xFE, 0x06, 0x03, 0xFD, 0xFD, 0xF7, 0xE4, 0x00,},//24th
	{0xFC, 0xFE, 0xFA, 0xFD, 0x00, 0x03, 0x06, 0xDC, 0x00,},//25th
	{0x06, 0x00, 0x06, 0x03, 0x00, 0xFD, 0xF4, 0xD4, 0x00,},//26th
	{0xFC, 0xFC, 0xFA, 0xFA, 0xFA, 0x03, 0x09, 0xCC, 0x00,},//27th
	{0x06, 0x02, 0x06, 0x06, 0x06, 0x03, 0x0C, 0xC4, 0x00,},//28th
	{0xFC, 0xFA, 0xFA, 0xF7, 0xF4, 0x05, 0xFF, 0xBC, 0x00,},//29th
	{0x06, 0x04, 0x06, 0x09, 0x0C, 0x05, 0x01, 0xB4, 0x00,},//30th
	{0xFC, 0xF8, 0xF7, 0xFD, 0x00, 0xFA, 0x03, 0x04, 0x00,},//31th
	{0x06, 0x06, 0x09, 0x03, 0x00, 0x05, 0x03, 0x0C, 0x00,},//32th
	{0xFA, 0x02, 0xF7, 0xFA, 0x00, 0xFA, 0x00, 0x14, 0x00,},//33th
	{0x06, 0x08, 0x09, 0x06, 0x00, 0x05, 0x05, 0x1C, 0x00,},//34th
	{0xFA, 0x00, 0xF7, 0xFA, 0xFD, 0xFA, 0xFD, 0x24, 0x00,},//35th
	{0x06, 0x0A, 0x09, 0x06, 0x03, 0x05, 0x07, 0x2C, 0x00,},//36th
	{0xFA, 0xFE, 0xF7, 0xF7, 0xF7, 0xFA, 0xFA, 0x34, 0x00,},//37th
	{0x06, 0x0C, 0x09, 0x09, 0x09, 0x06, 0xFD, 0x3C, 0x00,},//38th
	{0xFA, 0xFC, 0xF7, 0xF4, 0xF0, 0xFA, 0xF7, 0x44, 0x00,},//39th
	{0x08, 0x02, 0x09, 0x0C, 0x10, 0x06, 0x00, 0x4C, 0x00,},//40th
	{0xFA, 0xFA, 0xF4, 0xFA, 0xFD, 0xFA, 0xF4, 0xFE, 0x00,},//41th
	{0x08, 0x04, 0x0C, 0x06, 0x03, 0x06, 0x03, 0xF6, 0x00,},//42th
	{0xFA, 0xF8, 0xF4, 0xF7, 0xFD, 0x06, 0x06, 0xEE, 0x00,},//43th
	{0x08, 0x06, 0x0C, 0x09, 0x03, 0x06, 0x09, 0xE6, 0x00,},//44th
	{0xFA, 0xF6, 0xF4, 0xF7, 0xFA, 0xF9, 0xFB, 0xDE, 0x00,},//45th
	{0x00, 0x04, 0x0C, 0x09, 0x06, 0x06, 0x0C, 0xD6, 0x00,},//46th
	{0xFA, 0xF4, 0x00, 0xFD, 0xFA, 0xF9, 0xF9, 0xCE, 0x00,},//47th
	{0x08, 0x0A, 0x00, 0xFD, 0xF7, 0x09, 0x00, 0xC6, 0x00,},//48th
	{0xF8, 0x00, 0xF4, 0xF0, 0xEC, 0xF9, 0xF7, 0xBE, 0x00,},//49th
	{0x08, 0x0C, 0x0C, 0x10, 0x14, 0x09, 0x03, 0xB6, 0x00,},//50th
	{0xF8, 0xFE, 0xF0, 0xF4, 0xF7, 0xF7, 0x00, 0x02, 0x00,},//51th
	{0x08, 0x10, 0x10, 0x0C, 0x09, 0x09, 0x06, 0x0A, 0x00,},//52th
	{0xF8, 0xFC, 0x00, 0x03, 0x06, 0xF7, 0xFD, 0x12, 0x00,},//53th
	{0x0A, 0x02, 0x00, 0x03, 0x09, 0xFE, 0x02, 0x1A, 0x00,},//54th
	{0xF8, 0xFA, 0xF0, 0xEC, 0xE8, 0xF7, 0xFA, 0x22, 0x00,},//55th
	{0x0A, 0x04, 0x14, 0x10, 0x0C, 0x09, 0x0C, 0x2A, 0x00,},//56th
	{0xF8, 0xF8, 0xEC, 0xF0, 0xF4, 0x0C, 0x03, 0x32, 0x00,},//57th
	{0x0A, 0x06, 0x14, 0x14, 0x14, 0xF7, 0xF4, 0x3A, 0x00,},//58th
	{0xF8, 0xF6, 0xEC, 0xEC, 0xEC, 0x0C, 0x06, 0x42, 0x00,},//59th
	{0x0A, 0x08, 0xF4, 0xF4, 0xF7, 0xF7, 0xF0, 0x4A, 0x00,},//60th
	{0xF8, 0xF4, 0xEC, 0xE8, 0xE4, 0x0C, 0x09, 0xFF, 0x00,},//61th
	{0x0A, 0x0A, 0xF4, 0xF7, 0xF7, 0xF4, 0xFD, 0xF7, 0x00,},//62th
	{0xF8, 0xF0, 0xE8, 0xEC, 0xF0, 0xF4, 0xFA, 0xEF, 0x00,},//63th
	{0x0A, 0x0C, 0xF7, 0xF4, 0xF4, 0x10, 0x09, 0xE7, 0x00,},//64th
	{0xF6, 0xFE, 0xE8, 0xE8, 0xE8, 0xF4, 0xF7, 0xDF, 0x00,},//65th
	{0x0A, 0x10, 0xF7, 0xF7, 0xF4, 0x14, 0x0C, 0xD7, 0x00,},//66th
	{0xF6, 0xFC, 0xE8, 0xE4, 0xE0, 0xF4, 0xF0, 0xCF, 0x00,},//67th
	{0x0C, 0x04, 0xF7, 0xF7, 0xFA, 0xF4, 0xEC, 0xC7, 0x00,},//68th
	{0xF6, 0xFA, 0xE4, 0xE8, 0xEC, 0xF0, 0xF7, 0xBF, 0x00,},//69th
	{0x0C, 0x06, 0xF7, 0xFA, 0xFA, 0xF0, 0xF4, 0xB7, 0x00,},//70th
	{0xF6, 0xF8, 0xE4, 0xE4, 0xE4, 0xF0, 0xEC, 0x01, 0x00,},//71th
	{0x0C, 0x08, 0xFA, 0xF7, 0xF7, 0xF0, 0xE8, 0x09, 0x00,},//72th
	{0xF6, 0xF6, 0xE0, 0xE4, 0xE8, 0xEC, 0xF4, 0x11, 0x00,},//73th
	{0x0C, 0x0A, 0xFA, 0xFA, 0xF7, 0xEC, 0xF0, 0x19, 0x00,},//74th
	{0xF6, 0xF4, 0xE0, 0xE0, 0xE0, 0xEC, 0xEC, 0x21, 0x00,},//75th
	{0x00, 0xFC, 0xFA, 0xFA, 0xFD, 0xEC, 0xE8, 0x29, 0x00,},//76th
	{0xF6, 0xF0, 0xFA, 0xFD, 0xFD, 0xEC, 0xE4, 0x31, 0x00,},//77th
	{0x0C, 0x10, 0xFD, 0xFA, 0xFA, 0xE8, 0xF0, 0x39, 0x00,},//78th
	{0xF4, 0xFC, 0xFD, 0xFD, 0xFA, 0xE8, 0xEC, 0x41, 0x00,},//79th
	{0x0C, 0x14, 0xFD, 0xFD, 0x00, 0xE8, 0xE8, 0x49, 0x00,},//80th
	{0xF4, 0xFA, 0xFD, 0x00, 0x00, 0xE8, 0xE4, 0xFD, 0x00,},//81th
	{0xF4, 0xF8, 0x00, 0xFD, 0xFD, 0xE8, 0xE0, 0xF5, 0x00,},//82th
	{0xF4, 0xF6, 0x00, 0x00, 0xFD, 0xE4, 0xEC, 0xED, 0x00,},//83th
	{0x00, 0xFE, 0x00, 0x00, 0x03, 0xE4, 0xE8, 0xE5, 0x00,},//84th
	{0xF4, 0xF0, 0x00, 0x03, 0x03, 0xE4, 0xE4, 0xDD, 0x00,},//85th
	{0xF4, 0xEC, 0x03, 0x00, 0x00, 0xE4, 0xE0, 0xD5, 0x00,},//86th
	{0xF0, 0xF8, 0x03, 0x03, 0x00, 0x09, 0x10, 0xCD, 0x00,},//87th
	{0xF0, 0xF6, 0x03, 0x03, 0x06, 0x0C, 0x10, 0xC5, 0x00,},//88th
	{0xF0, 0xF4, 0x03, 0x06, 0x06, 0x10, 0x10, 0xBD, 0x00,},//89th
	{0x00, 0x02, 0x06, 0x03, 0x03, 0x14, 0x10, 0xB5, 0x00,},//90th
	{0xF0, 0xEC, 0x06, 0x06, 0x03, 0x0C, 0x14, 0x03, 0x00,},//91th
	{0xF0, 0xE8, 0x06, 0x06, 0x09, 0x10, 0x14, 0x0B, 0x00,},//92th
	{0xEC, 0xF4, 0x06, 0x09, 0x09, 0x14, 0x14, 0x13, 0x00,},//93th
	{0xEC, 0xF0, 0x09, 0x06, 0x06, 0x00, 0xFD, 0x1B, 0x00,},//94th
	{0xEC, 0xEC, 0x09, 0x09, 0x06, 0x00, 0x03, 0x23, 0x00,},//95th
	{0xEC, 0xE8, 0xEC, 0xEC, 0xE8, 0x00, 0xFA, 0x2B, 0x00,},//96th
	{0xE8, 0xF0, 0xE8, 0xE8, 0xE4, 0x00, 0x06, 0x33, 0x00,},//97th
	{0xE8, 0xEC, 0x14, 0x10, 0x10, 0x00, 0x09, 0x3B, 0x00,},//98th
	{0xE8, 0xE8, 0x10, 0x14, 0x14, 0x10, 0x0C, 0x43, 0x00,},//99th
};
	
const U08 RRT_DECOMPRESS_IDX[16] = {										
										0x00,0x01,0x08,0x08,/*A0h Lower Page */
										0x02,0x03,0x04,0x08,/*A1h Middle Page*/
										0x05,0x06,0x08,0x08,/*A2h Upper Page*/
										0x07,0x08,0x08,0x08,/*A3h SLC Page*/
									}; 


#endif

#if (NAND_FLASH == HYNIX_8A1M)
const U08 RRT[RRT_TABLE_CNT][RRT_PARAM_CNT+1]=
{
   /*LP-R1 MP-R2 UP-R3 MP-R4 LP-R5 MP-R6 UP-R7 SLC-P NULL*/
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}, //0th
	{0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x02, 0x1E, 0x00,}, //1th
	{0xFB, 0xFC, 0x03, 0xFA, 0xFB, 0xF7, 0xFD, 0x1C, 0x00,}, //2th
	{0x00, 0x05, 0x02, 0xFF, 0xFD, 0xFD, 0xFB, 0x1A, 0x00,}, //3th
	{0xFE, 0xFF, 0xFC, 0xFD, 0xF6, 0xFA, 0xF7, 0x18, 0x00,}, //4th
	{0x05, 0x05, 0x06, 0x02, 0x04, 0x02, 0xFD, 0x16, 0x00,}, //5th
	{0x03, 0x01, 0x00, 0xFF, 0x00, 0xFD, 0xF9, 0x14, 0x00,}, //6th
	{0x03, 0x08, 0xFE, 0x05, 0xFD, 0x05, 0xF5, 0x12, 0x00,}, //7th
	{0xFE, 0x03, 0xFA, 0xFF, 0xFA, 0xFF, 0xF7, 0x10, 0x00,}, //8th
	{0xF9, 0xFA, 0x06, 0xFB, 0xF8, 0xFA, 0x02, 0x0E, 0x00,}, //9th
	{0x09, 0x0E, 0x0F, 0x10, 0x0C, 0x0C, 0x0C, 0x0C, 0x00,}, //10th
	{0x09, 0x06, 0x06, 0xFD, 0x03, 0xFA, 0x05, 0x0A, 0x00,}, //11th
	{0x09, 0x06, 0x09, 0xFB, 0x00, 0xF6, 0x08, 0x08, 0x00,}, //12th
	{0x09, 0x05, 0x03, 0x05, 0xF8, 0x04, 0x04, 0x06, 0x00,}, //13th
	{0x09, 0x05, 0x03, 0x00, 0xF6, 0x00, 0x02, 0x04, 0x00,}, //14th
	{0x07, 0x04, 0x03, 0xFE, 0xFB, 0xFD, 0x00, 0x02, 0x00,}, //15th
	{0x07, 0x04, 0xFC, 0xF9, 0xF9, 0xF3, 0xF0, 0x00, 0x00,}, //16th
	{0x06, 0xFF, 0x00, 0xFB, 0x00, 0xF9, 0xFD, 0xFE, 0x00,}, //17th
	{0x06, 0x03, 0x00, 0xFD, 0xFD, 0xFA, 0xFB, 0xFC, 0x00,}, //18th
	{0x00, 0x03, 0xFA, 0xFB, 0xF6, 0xF8, 0xF1, 0xFA, 0x00,}, //19th
	{0x05, 0x03, 0xFE, 0xFB, 0xFF, 0xF5, 0xFD, 0xF8, 0x00,}, //20th
	{0xFC, 0x02, 0xFE, 0x02, 0xF8, 0x02, 0xFB, 0xF6, 0x00,}, //21th
	{0xF6, 0x02, 0xFE, 0x00, 0xF6, 0x00, 0xF9, 0xF4, 0x00,}, //22th
	{0x03, 0x02, 0xFE, 0x03, 0xFB, 0x04, 0xF7, 0xF2, 0x00,}, //23th
	{0x03, 0xF6, 0xF8, 0xF3, 0xF9, 0xF1, 0xF4, 0xF0, 0x00,}, //24th
	{0x03, 0x01, 0xFE, 0xF8, 0xF6, 0xF8, 0xF3, 0xEE, 0x00,}, //25th
	{0x03, 0x00, 0xFC, 0x02, 0xF4, 0x02, 0xF4, 0xEC, 0x00,}, //26th
	{0x03, 0xFE, 0xFC, 0xF7, 0xF2, 0xF3, 0xF2, 0xEA, 0x00,}, //27th
	{0x03, 0xF5, 0xF4, 0xF1, 0xEF, 0xEE, 0xEE, 0xE8, 0x00,}, //28th
	{0x05, 0xFD, 0xFA, 0xFB, 0x09, 0xF8, 0xF9, 0xE6, 0x00,}, //29th
	{0x00, 0xFD, 0xF8, 0xFA, 0xFA, 0xF5, 0xED, 0xE4, 0x00,}, //30th
	{0x00, 0xFD, 0xFA, 0xF9, 0xF8, 0xF6, 0xF3, 0xE2, 0x00,}, //31th
	{0xEF, 0x08, 0xF1, 0x0A, 0xED, 0x0A, 0xE6, 0xE0, 0x00,}, //32th
	{0xF4, 0xFC, 0xF8, 0xF8, 0xF2, 0xF4, 0xF2, 0xDE, 0x00,}, //33th
	{0xFC, 0xFC, 0xF8, 0xEC, 0xF4, 0xE9, 0xE9, 0xDC, 0x00,}, //34th
	{0xFC, 0xFB, 0xF6, 0xF6, 0xF1, 0xF0, 0xEF, 0xDA, 0x00,}, //35th
	{0xFB, 0xFA, 0xF6, 0xF6, 0xF6, 0xF2, 0xEC, 0xD8, 0x00,}, //36th
	{0xFA, 0xFA, 0xF6, 0xF4, 0xF3, 0xEE, 0xE7, 0xD6, 0x00,}, //37th
	{0xED, 0xF9, 0xF4, 0xF2, 0xEA, 0xEF, 0xEB, 0xD4, 0x00,}, //38th
	{0xF9, 0xF8, 0xF3, 0xF5, 0xF1, 0xF2, 0xE6, 0xD2, 0x00,}, //39th
	{0xF8, 0xF8, 0xF5, 0xF8, 0xF6, 0xFA, 0xF6, 0xD0, 0x00,}, //40th
	{0xF8, 0xF5, 0xF2, 0xF6, 0xF2, 0xF5, 0xEA, 0xCE, 0x00,}, //41th
	{0xF8, 0xF5, 0xED, 0xF0, 0xEF, 0xEC, 0xE8, 0xCC, 0x00,}, //42th
	{0xF6, 0x08, 0x03, 0x03, 0xED, 0x00, 0xF9, 0xCA, 0x00,}, //43th
	{0xF4, 0xF0, 0xEA, 0xED, 0xEF, 0xEA, 0xE7, 0xC8, 0x00,}, //44th
	{0xEF, 0x0A, 0x09, 0x05, 0xE8, 0x03, 0x00, 0xC6, 0x00,}, //45th
	{0xE8, 0xF1, 0xF1, 0xEF, 0xE2, 0xF2, 0xF1, 0xC4, 0x00,}, //46th
	{0xE8, 0xF8, 0xE4, 0xF6, 0xDD, 0xF8, 0xE4, 0xBE, 0x00,}, //47th
	{0xE7, 0x0C, 0x0C, 0x07, 0xE5, 0x05, 0xFF, 0xB0, 0x00,}, //48th
	{0xE2, 0x01, 0xE1, 0xFC, 0xE2, 0xFB, 0xD5, 0x9C, 0x00,}, //49th


};


#endif
#ifdef __RETRY_TABLE_RATE__
U16 wRRTUseCnt[RRT_TABLE_CNT + 1];
#endif


void Retry_SetArcStartOffset(U8 bChan,U8 bChip,U08 *pDataVal)//Retry_SetFlashReadOffset
{
#if (READ_RETRY_ARC_MAX != 0)
	U08 m;
	for(m = 0;m<READ_OFFSET_REG_CNT;m++)
	{
		Fdl_MCU_SetFeature_ReadOffs(bChan,bChip, bFlashReadOffsetAddr[m], pDataVal);
	}

#endif
}


void Retry_SetFlashReg_Arc(U8 bChan,U8 bChip,U08 bOptionIdx)
{

#if((NAND_FLASH == MIC_B0KB) ||\
	(NAND_FLASH == MIC_B16A) ||\
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_M16)  ||\
	(NAND_FLASH == MIC_N18)  ||\
	(NAND_FLASH == MIC_N28)  ||\
	(NAND_FLASH == MIC_B27A) ||\
	(NAND_FLASH == MIC_B27B) ||\
	(NAND_FLASH == INTEL_B27B) ||\
	(NAND_FLASH == MIC_M84C))
    U08 bData[4] = {0,0,0,0};
	bData[0] = bOptionIdx;
	HAL_McuSetFeature(bChan, bChip, 0x96, 1,4, &bData[0]);
#endif

	//FDL_Log("Retry_SetFlashReg_Arc, bOptionIdx=0x%x", bOptionIdx);
}

void Retry_SetFlashReg(U8 bChan,U8 bChip,U08 bOptionIdx,U32 dRow)  //Retry_SetFlashOption
{
#if((NAND_FLASH == MIC_B0KB) ||\
	(NAND_FLASH == MIC_B16A) ||\
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_M16)  ||\
	(NAND_FLASH == MIC_B27A) ||\
	(NAND_FLASH == MIC_B27B) ||\
	(NAND_FLASH == INTEL_B27B) ||\
	(NAND_FLASH == MIC_M84C) ||\
	(NAND_FLASH == MIC_N28))
	U08 bData[4] = {0,0,0,0};
	bData[0] = bOptionIdx;
	HAL_McuSetFeature(bChan, bChip, SET_OPTION_ADDR,1,4, &bData[0]);
#endif

#if (NAND_FLASH == TOS_BICS3)
#ifndef __RETRY_PERSISTENCE__
	U08 Idx;

	U08 bRegData[15] = {0};
	U08 *pRRt = (U08 *)RRT[bOptionIdx];

	if(bOptionIdx == READ_RETRY_SET_OPTION_MAX)
	{
		pRRt = bSoftDecodeBaseRRT;
	}
	
	for(Idx = 0;Idx<15;Idx++)
	{
		bRegData[Idx] = pRRt[RRT_DECOMPRESS_IDX[Idx]];
	}

	for(Idx = 0;Idx<3;Idx++)
	{
		HAL_McuSetFeature(bChan, bChip,0x89+Idx,1,5,bRegData+Idx*5);
	}	
#else/*8T23和9T23开启Retry保持功能*/
	U08 bIdx = 0;
	U08 bPageTape = GET_TLC_FLAG(dRow);
	U08 *pRRt = (U08 *)&RRT[bOptionIdx][0];
	U08 bRegData[4] = {0};
	
#ifdef __FLASH_SAND_BICS3__
	U08 bFeatureBase = 0x12;
#endif

#ifdef __FLASH_TOS_BICS3__
	U08 bFeatureBase = 0x89;
#endif

	if(bOptionIdx == READ_RETRY_SET_OPTION_MAX)
	{
		pRRt = bSoftDecodeBaseRRT;
	}

	switch(bPageTape)
	{
		case 0x00:/*SLC*/
			for(bIdx = 0;bIdx < 4;bIdx++)
			{
				bRegData[bIdx] = pRRt[RRT_DECOMPRESS_IDX[bIdx + 10]];
			}
			HAL_McuSetFeature(bChan, bChip,(bFeatureBase + 2),1,4,bRegData);
			break;
		case 0x01:/*LSB*/
			HAL_McuGetFeature(bChan, bChip,bFeatureBase,bRegData);		
			bRegData[0] = pRRt[0];
			bRegData[2] = pRRt[2];
//			FDL_Log("Data1:W%02X-%02X-W%02X-%02X",bRegData[0],bRegData[1],bRegData[2],bRegData[3]);
			HAL_McuSetFeature(bChan, bChip,bFeatureBase,1,4,bRegData);
#if 0
			bRegData[0] = 0;
			bRegData[1] = 0;
			bRegData[2] = 0;
			bRegData[3] = 0;
			HAL_McuGetFeature(bChan, bChip,bFeatureBase,bRegData);	
			FDL_Log("Data1:%02X-%02X-%02X-%02X",bRegData[0],bRegData[1],bRegData[2],bRegData[3]);
#endif
			break;

		case 0x02:/*MSB*/
			for(bIdx = 0;bIdx < 4;bIdx++)
			{
				bRegData[bIdx] = pRRt[RRT_DECOMPRESS_IDX[bIdx + 5]];
			}
			HAL_McuSetFeature(bChan, bChip,(bFeatureBase + 1),1,4,bRegData);
			break;

		case 0x03:/*CSB*/
			HAL_McuGetFeature(bChan, bChip,bFeatureBase,bRegData);					
			bRegData[1] = pRRt[1];
			bRegData[3] = pRRt[3];
//			FDL_Log("Data2:%02X-W%02X-%02X-W%02X",bRegData[0],bRegData[1],bRegData[2],bRegData[3]);
			HAL_McuSetFeature(bChan, bChip,bFeatureBase,1,4,bRegData);
#if 0
			bRegData[0] = 0;
			bRegData[1] = 0;
			bRegData[2] = 0;
			bRegData[3] = 0;
			HAL_McuGetFeature(bChan, bChip,0x89,bRegData);	
			FDL_Log("Data2:%02X-%02X-%02X-%02X",bRegData[0],bRegData[1],bRegData[2],bRegData[3]);
#endif
			break;

		default:
			//PRINT_TXT("8/9T23 Retry default!");
			break;
	}
#endif
#endif

#if (NAND_FLASH == SUMSUNG_3DV4)  || (NAND_FLASH == AFGM)
	U08 Idx;
	U08 bRegData[8] = {0};
	U08 *pRRt = (U08 *)RRT[bOptionIdx];
	
	if(bOptionIdx == READ_RETRY_SET_OPTION_MAX)
	{
		pRRt = bSoftDecodeBaseRRT;
	}
	
//	U16 wPageOff = dRow%ROW_GAP_OF_PLANE;
	U16 wPageOff = GET_PAGE_NUM(dRow);
	
	if((dRow>>24 != 0)&&(wPageOff<0x2F8))//TLC
	{
		U08 bSetOff = 0;
		if((wPageOff > 7)&&(wPageOff < 0x2F0))
		{
			bSetOff = 12;
		}
		else
		{
			bSetOff = 4;
		}

		for(Idx = 0;Idx<8;Idx++)
		{
			bRegData[Idx] = pRRt[RRT_DECOMPRESS_IDX[bSetOff+Idx]];
		}
		
		for(Idx = 0;Idx<2;Idx++)
		{
			HAL_McuSetFeature(bChan, bChip,0x89+Idx,1,4,bRegData+Idx*4);
		}
	}
	else
	{
		for(Idx = 0;Idx<4;Idx++)
		{
			bRegData[Idx] = pRRt[RRT_DECOMPRESS_IDX[Idx]];
		}

		HAL_McuSetFeature(bChan, bChip, 0x8D,1,4,bRegData);
	}
#endif

#if (NAND_FLASH == GCGF)
	U08 Idx;
	U08 bRegData[4] = {0};
	U08 *pRRt =  (U08 *)RRT[bOptionIdx];

	if(bOptionIdx == READ_RETRY_SET_OPTION_MAX)
	{
		pRRt = bSoftDecodeBaseRRT;
	}

//	U16 wPageOff = dRow%ROW_GAP_OF_PLANE;
	U16 wPageOff = GET_PAGE_NUM(dRow);

	if((dRow>>24 != 0))//MLC reg偏移为1
	{
		for(Idx = 0;Idx<4;Idx++)
		{
			bRegData[Idx] = pRRt[1+Idx];
		}
		HAL_McuSetFeature(bChan, bChip,0x89,1,4,bRegData);
	}
	else
	{
		for(Idx = 0;Idx<1;Idx++)
		{
			bRegData[Idx] = pRRt[Idx];
		}
		HAL_McuSetFeature(bChan, bChip, 0x89,1,4,bRegData);
	}
#endif
#if (NAND_FLASH == MIC_N18)

#if (READ_RETRY_SET_OPTION_MAX == 1)		//如有修改需要，请先和Webb确认

#else
	if(bOptionIdx == 0)
	{
		Retry_SetFlashReg_Arc(bChan, bChip, 0x1);	//Enable calibration,next time enable persistence
	}
	else if(bOptionIdx == 1)
	{
		Retry_SetFlashReg_Arc(bChan, bChip, 0x3);	//Enable calibration,enable persistence
	}
#endif
#endif

#if (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)

	U08 bIdx = 0;
	U08 bPageTape = 3;/*默认SLC*/
	U08 *pRRt = (U08 *)&RRT[bOptionIdx][0];
	U08 bRegData[4] = {0};
	U08 bFeatureAddr = 0xA0;

	if(bOptionIdx == READ_RETRY_SET_OPTION_MAX)/*软解码偏移*/
	{
		pRRt = bSoftDecodeBaseRRT;
	}
	
	if((dRow>>24))/*TLC Page*/
	{		
		U16 wPageOff = GET_PAGE_NUM(dRow);
		bPageTape = wPageOff%3;/*计算Page Type*/		
	}
	
	for(bIdx = 0;bIdx < 4;bIdx++)
	{
		bRegData[bIdx] = pRRt[RRT_DECOMPRESS_IDX[bIdx + bPageTape*4]];
	}
	bFeatureAddr += bPageTape;

//	FDL_Log("[0x%08x]Data1:%02X-%02X-%02X-%02X",dRow,bRegData[0],bRegData[1],bRegData[2],bRegData[3]);
	HAL_McuSetFeature(bChan, bChip,bFeatureAddr,1,4,bRegData);

#if 0
	bRegData[0] = 0;
	bRegData[1] = 0;
	bRegData[2] = 0;
	bRegData[3] = 0;
	HAL_McuGetFeature(bChan, bChip,bFeatureAddr,bRegData);	
	FDL_Log("[%08d]Data2:%02X-%02X-%02X-%02X",GET_PAGE_NUM(dRow),bRegData[0],bRegData[1],bRegData[2],bRegData[3]);
#endif


#endif

#if (NAND_FLASH == HYNIX_8A1M)

	U08 bIdx = 0;
	U08 bPageTape = GET_TLC_FLAG(dRow);
	U08 *pRRt = (U08 *)&RRT[bOptionIdx][0];
	U08 bRegData[4] = {0,0,0,0};
	U08 bFeatureAddr = 0x25;
	U08 bFeatureCnt = 0x01;
	U08 bFeatureGap = 0x02;
	U08 bRRTStart 	= 0x00;

	if(bOptionIdx == READ_RETRY_SET_OPTION_MAX)/*软解码偏移*/
	{
		pRRt = bSoftDecodeBaseRRT;
	}

	switch(bPageTape)
	{
		case 0x00:/*SLC R0[Feature Addr:0x25]*/						
			bFeatureAddr 	= 0x25;
			bFeatureCnt 	= 0x01;
			bFeatureGap 	= 0x00;
			bRRTStart 		= 0x07;
			break;
			
		case 0x01:/*LSB R3 R7[Feature Addr:0x2B,0x2F]*/
			bFeatureAddr 	= 0x2B;/*Feature起始地址*/
			bFeatureCnt 	= 0x02;/*需要设置的Feature地址*/
			bFeatureGap 	= 0x04;/*Feature地址之间的差值*/
			bRRTStart 		= 0x02;/*RRT Table的起始偏移(差值与bFeatureGap一致)*/
			break;
			
		case 0x02:/*CSB R2 R4 R6[Feature Addr:0x2A,0x2C,0x2E]*/	
			bFeatureAddr 	= 0x2A;
			bFeatureCnt 	= 0x03;
			bFeatureGap 	= 0x02;
			bRRTStart 		= 0x01;	
			break;
			
		case 0x03:/*MSB R1 R5[Feature Addr:0x29,0x2D]*/
			bFeatureAddr 	= 0x29;
			bFeatureCnt 	= 0x02;
			bFeatureGap 	= 0x04;
			bRRTStart 		= 0x00;
			break;
			
		default:
			break;
	}

	HAL_MCUCheckStatus(bChan, bChip);
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_RETRYR1);
	for(bIdx = 0;bIdx < bFeatureCnt;bIdx++)
	{
		bRegData[0] = pRRt[bRRTStart + bIdx*bFeatureGap];				
		HAL_MCU_SendOneAddr(bChan,bFeatureAddr);
		Delay(4);
		HAL_MCUSendNByteDataByDqs(bChan,bRegData,1);
		bFeatureAddr += bFeatureGap;
	}
	HAL_MCU_SendCmd(bChan,FL_CMD_SET_RETRYR2);
	HAL_MCUCheckStatus(bChan, bChip);
	
	/*Get Feature*/
#if 0
	bFeatureAddr -= bFeatureGap*bFeatureCnt;
	HAL_MCU_SendCmd(bChan,FL_CMD_GET_RETRYR);
	for(bIdx = 0;bIdx < bFeatureCnt;bIdx++)
	{
		bRegData[0] = 0;
		HAL_MCU_SendOneAddr(bChan,bFeatureAddr);
		HAL_McuSendReadPluseGetData(bChan,bRegData,1);				
		FDL_Log("Feature[0x%X] = 0x%X",bFeatureAddr,bRegData[0]);
		bFeatureAddr += bFeatureGap;
	}
#endif

#endif
}

void Retry_SaveTaskException(U8 bChan,U32 dOfstInTask,U8 bErrType)
{    
	//if(bErrType == 0){FDL_Err("Retry_SaveTaskException = %d",bErrType);return;}

    if (NULL != tFtlTask.FTLTaskExceptionHandler)
    {
        tFtlTask.FTLTaskExceptionHandler(dOfstInTask, ENTRANCE_DMA, bErrType);
    }
}

U08 Retry_JudgeDmaMask(U32 dIdx,U8 bSecOfClst,U32 dOfstInTask)
{    
    if(tFtlTask.pMskBuff != NULL)
    {
		U08 bMaskInfo; 
    	U08 bOfstInClst; 
		U08 bMaskOfsInClst;
		U08 bMaskIdxOfsInClst; 

		bOfstInClst = dIdx % bSecOfClst;
        bMaskOfsInClst    = bOfstInClst / BIT_OF_BYTE;
        bMaskIdxOfsInClst = bOfstInClst % BIT_OF_BYTE;

		if ((tFtlTask.dTaskType == TYPE_NCQ_READ) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K))
		{
			if (dOfstInTask == 0)//first
			{
				bMaskInfo = (tFtlTask.pMskBuff[NCQ_READ_MASK_FISRT * MASK_BYTE_OF_CLST + bMaskOfsInClst]>> bMaskIdxOfsInClst);
			}
			else if (dOfstInTask == (tFtlTask.wLen-1))//last
			{	
				bMaskInfo = (tFtlTask.pMskBuff[NCQ_READ_MASK_END * MASK_BYTE_OF_CLST + bMaskOfsInClst]>> bMaskIdxOfsInClst);
			}
			else
			{	
				bMaskInfo = 0;//(tFtlTask.pMskBuff[NCQ_READ_MASK_MID * MASK_BYTE_OF_CLST + bMaskOfsInClst]>> bMaskIdxOfsInClst);
			}
		}
		else
		{
        	bMaskInfo  = (tFtlTask.pMskBuff[dOfstInTask * MASK_BYTE_OF_CLST+bMaskOfsInClst] >> bMaskIdxOfsInClst);
		}
				
		bMaskInfo &= ((0x01<<SECT_OF_DMA)-1);		//表格或逻辑数据都是2K
        if(((0x01<<SECT_OF_DMA)-1) == bMaskInfo)
        {
            //FDL_Log("Dma all mask!");
            return DMA_ALL_MASK;
        }
        else
        {
            //FDL_Log("mask info =%x",bMaskInfo);
            return bMaskInfo;
        }
    }

    return 0;
}

U08 Retry_AnalyzeExceptionInfo(U08 bChan, U16 wDmaStatus, U08 bRetryType)
{
	U08 bExcpInfo;
	
	if(DMAINFO_ABNORMAL(wDmaStatus))//retry fail,all one和all zero都会报ecc fail,要单独分开
	{
		if((wDmaStatus & DMA_INFO_LDPC_FAIL) == DMA_INFO_LDPC_FAIL)
		{
			bExcpInfo = 1<<RD_ERR_DATA_ERROR;
		}
		else if ((wDmaStatus & DMA_INFO_ALL_ZERO) == DMA_INFO_ALL_ZERO)
		{
			bExcpInfo = 1<<RD_ERR_ALL_ZERO;
		}
		else
		{
			bExcpInfo = 1<<RD_ERR_ALL_ONE;
		}
	}

#ifdef RETRY_SVALUE
else if((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) && 
		(DMAINFO_SVALUE(wDmaStatus) > RETRY_SVALUE))
	{
		bExcpInfo = 1<<RD_ERR_DATA_ERROR;
	}
#endif
	else//retry pass
	{
		bExcpInfo = (1<<bRetryType);
	}
	return bExcpInfo;
}

#ifdef __VER_TLC_PAGE__
void Retry_VarTLCGetDmaInfor(U08 bChanPhy, U08 bChipPhy, U08 bRetryIdx,T_DMA_INFOR *ptDmaInfor)
{

	U08 bRetryIdxt		= bRetryIdx;
	U32 dLogNumExt 		= VER_TLC_LOG_NUM_EXT(bChanPhy);
	U08 bLogChipOfChan	= VER_TLC_GET_LOG_CE(dLogNumExt);/*取逻辑CE号(已经过详细确认,该CE号确实逻辑的,而bChipPhy是物理CE号)*/	
	U08 (*ptRetryDMAInfo)[4][4] = (U08 *)tRetryInfor.m_tDmaInfor;
	U08 *FailDMAIndex			= (U08 *)(&ptRetryDMAInfo[bChanPhy][bChipPhy]);
	U32 dRowAddr 		= tChTaskQueue[bChanPhy].tLunTask[bLogChipOfChan].dRow[VER_TLC_GET_LUN_TASK_OFST(dLogNumExt) & (MAX_CHAN_TASK-1)] & PLANE_BIT_MASK_OF_ROW;


	ptDmaInfor->bSecOfClst		= SECT_OF_DATA_CLST;
	ptDmaInfor->bSectOfDma		= DSECT_OF_DMA;
	ptDmaInfor->dRowAddr		= dRowAddr + GET_ROW_ADDR_TLC(0,FailDMAIndex[bRetryIdx]/VER_TLC_DMA_CNT_PAGE,0);
	ptDmaInfor->bFlashIdx		= (FailDMAIndex[bRetryIdx]%VER_TLC_DMA_CNT_PAGE)*DSECT_OF_DMA;
	ptDmaInfor->dDmaInfoAddr	= (U32)(tFtlTask.dEccStsBuff + VER_TLC_ECC_INFO_OFSEX(bChanPhy,bLogChipOfChan) + FailDMAIndex[bRetryIdx]*2);
	ptDmaInfor->dDmaBuffAddr	= tFtlTask.dHandleBuff;
	ptDmaInfor->bUdSize 		= UD_LEN_OF_DATA_CLST;	
	ptDmaInfor->dBuffIdx		= 0;//tRetryInfor.m_dFailBuffIndex[bChan][bChip][bRetryIdx];				
	ptDmaInfor->wColAddr		= Fdl_ConvertColAddr(tRetryInfor.m_bDataType, ptDmaInfor->bFlashIdx);  
	ptDmaInfor->bMaskInfo		= 0;
	ptDmaInfor->wSeedIdx		= INVALID_U16;
	ptDmaInfor->dUserDataAddr	= (U32)&tFtlTask.bMetaDataBuff[0];

#ifdef __BADBLK_BINDMAP__	/*BadBlkBondMap*/
//	ptDmaInfor->dRowAddr = Fdl_GetBadMapAddr(ptDmaInfor->dRowAddr,bChanPhy,bLunOfChan);
#endif	
}


#endif

void Retry_ReadGetDmaInfor(U32 dErrDmaIdx, T_DMA_INFOR *ptDmaInfor,U08 *bLunOfChan)
{
	T_ADDR_INFO tAddr;
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;
	U16 *wpDmaInfo = (U16 *)(tFtlTask.dEccStsBuff);
	U32 dOfstInTask = dErrDmaIdx / ptDmaInfor->bSecOfClst;

	ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfstInTask % tFtlTask.wContiAddrMax]); //簇对应的地址信息
	*bLunOfChan = ptClstAddr->m_bLunOfChan;
	Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
	ptDmaInfor->dRowAddr = tAddr.dRow;

	if (DISCRETE_CLST == tFtlTask.bArgsArr[0])
	{
		ptDmaInfor->dBuffIdx = dErrDmaIdx % ptDmaInfor->bSecOfClst + ((U32)tFtlTask.pwBuffIdx[dOfstInTask]);
	}
	else
	{
		ptDmaInfor->dBuffIdx = dErrDmaIdx;						
	}

	ptDmaInfor->dDmaBuffAddr = tFtlTask.dHandleBuff;
	ptDmaInfor->bFlashIdx	 = ptClstAddr->m_bClstOffs * ptDmaInfor->bSecOfClst + dErrDmaIdx % ptDmaInfor->bSecOfClst; 
	ptDmaInfor->wColAddr 	= Fdl_ConvertColAddr(tFtlTask.bDataType, ptDmaInfor->bFlashIdx);	
	ptDmaInfor->bMaskInfo = Retry_JudgeDmaMask(dErrDmaIdx, ptDmaInfor->bSecOfClst, dOfstInTask);
	ptDmaInfor->wSeedIdx = INVALID_U16;

	if(NULL != tFtlTask.pwSeedIndex)
	{
		ptDmaInfor->wSeedIdx = ((U16 *)tFtlTask.pwSeedIndex)[dOfstInTask];
	}

	if (0 == tFtlTask.dUdBuff)
	{
		ptDmaInfor->dUserDataAddr = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr.bUdSize*tAddr.bChan - ptClstAddr->m_bClstOffs * tAddr.bUdSize;
	}
	else
	{
		ptDmaInfor->dUserDataAddr = tFtlTask.dUdBuff + dOfstInTask * tAddr.bUdSize- ptClstAddr->m_bClstOffs * tAddr.bUdSize;
	}	
	ptDmaInfor->dDmaInfoAddr =	(U32)wpDmaInfo + ((dErrDmaIdx/(SECT_OF_DMA))*2)%pFdlParam.dEccInfoBuffSize;
	ptDmaInfor->bUdSize = tAddr.bUdSize;
}

void Retry_CopybackGetDmaInfor(U08 bChan, U08 bChip, U08 bRetryIndex,T_DMA_INFOR *ptDmaInfor)
{
	U08 bStartIdx = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip];
	T_CB_DMAINFOR tDmaInfo = tRetryInfor.m_tDmaInfor[bStartIdx+bRetryIndex];                

	ptDmaInfor->bFlashIdx = tDmaInfo.m_bDmaOffsInPage * tRetryInfor.m_bSecOfDma;
	ptDmaInfor->bUdSize = tRetryInfor.m_bUserDataLen;
	ptDmaInfor->bSecOfClst = tRetryInfor.m_bUserAttachSize;
	ptDmaInfor->bSectOfDma = tRetryInfor.m_bSecOfDma;
	
	ptDmaInfor->dBuffIdx = tDmaInfo.m_bBuffDmaIndex * tRetryInfor.m_bSecOfDma;		
	ptDmaInfor->dDmaInfoAddr = tRetryInfor.m_dDmaInforAddr + ptDmaInfor->dBuffIdx/SECT_OF_DMA*2;
	ptDmaInfor->dDmaBuffAddr =tRetryInfor.m_dDmaBuffAddr;
	ptDmaInfor->dRowAddr = tDmaInfo.m_dDmaRow;
	ptDmaInfor->dUserDataAddr = tDmaInfo.m_dUserBaseAddr;
	
	ptDmaInfor->wSeedIdx = INVALID_U16; 
	ptDmaInfor->wColAddr = Fdl_ConvertColAddr(tRetryInfor.m_bDataType, ptDmaInfor->bFlashIdx);	
	ptDmaInfor->bMaskInfo = 0;
}

#ifdef __PRE_4K_READ_ENABLE__
void Retry_PreReadGetDmaInfo(U32 dErrDmaIdx, T_DMA_INFOR *ptDmaInfor, U08 bChan,U08 *bLunOfChan)
{
	if(TYPE_TABLE == tFtlTask.bDataType)
	{
		ptDmaInfor->bSecOfClst = SECT_OF_TBLE_CLST;
		ptDmaInfor->bSectOfDma = TSECT_OF_DMA;
	}
	else
	{
		ptDmaInfor->bSecOfClst = SECT_OF_DATA_CLST;
		ptDmaInfor->bSectOfDma = DSECT_OF_DMA;
	}
	
	SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_EN = 0;		//关闭hold中断，如果CQ停在hold指令，有int flag就会触发中断，不管CQ是否继续执行
	__nds32__dsb();
	T_ADDR_INFO tAddr;
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;
	U16 *wpDmaInfo = (U16 *)(tFtlTask.dEccStsBuff);
	U32 dOfstInTask = dErrDmaIdx / ptDmaInfor->bSecOfClst;

	ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfstInTask % tFtlTask.wContiAddrMax]); //簇对应的地址信息
	*bLunOfChan 	= ptClstAddr->m_bLunOfChan;
	
	Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
	ptDmaInfor->dRowAddr = tAddr.dRow;
	ptDmaInfor->dBuffIdx = dErrDmaIdx % ptDmaInfor->bSecOfClst + ((U32)tFtlTask.pwBuffIdx[dOfstInTask]);//Aaron	
	ptDmaInfor->dDmaBuffAddr =tFtlTask.dHandleBuff;
	ptDmaInfor->bFlashIdx	 = ptClstAddr->m_bClstOffs * ptDmaInfor->bSecOfClst + dErrDmaIdx % ptDmaInfor->bSecOfClst; 
	ptDmaInfor->wColAddr 	= Fdl_ConvertColAddr(tFtlTask.bDataType, ptDmaInfor->bFlashIdx);	
	ptDmaInfor->bMaskInfo = Retry_JudgeDmaMask(dErrDmaIdx, ptDmaInfor->bSecOfClst, dOfstInTask);
	ptDmaInfor->wSeedIdx = INVALID_U16;

	if(NULL != tFtlTask.pwSeedIndex)
	{
		ptDmaInfor->wSeedIdx = ((U16 *)tFtlTask.pwSeedIndex)[dOfstInTask];
	}

	if (0 == tFtlTask.dUdBuff)
	{
		ptDmaInfor->dUserDataAddr = (U32)&tFtlTask.bMetaDataBuff[0];// + tAddr.bUdSize*tAddr.bChan - ptClstAddr->m_bClstOffs * tAddr.bUdSize;
	}
	else
	{
		ptDmaInfor->dUserDataAddr = tFtlTask.dUdBuff + dOfstInTask * tAddr.bUdSize- ptClstAddr->m_bClstOffs * tAddr.bUdSize;
	}	
	ptDmaInfor->dDmaInfoAddr =	(U32)wpDmaInfo + ((dErrDmaIdx/(SECT_OF_DMA))*2)%pFdlParam.dEccInfoBuffSize;
	ptDmaInfor->bUdSize = tAddr.bUdSize;	
}
#endif

void Retry_GetDmaInfor(U08 bChan, U08 bChip, U08 bRetryIdx, T_DMA_INFOR *ptDmaInfor)
{ 		

	if (tRetryInfor.m_OpType == COPYBACK_RETRY_MODE) //CopyBack
	{
		Retry_CopybackGetDmaInfor(bChan,bChip,bRetryIdx,ptDmaInfor);
	}
#ifdef __VER_TLC_PAGE__
	else if(tRetryInfor.m_OpType == VAR_TLC_PAGE_MODE)
	{
		Retry_VarTLCGetDmaInfor(bChan,bChip,bRetryIdx,ptDmaInfor);
	}
#endif
	else
	{
		U08 bLunOfChan;
#ifdef __PRE_4K_READ_ENABLE__
	 if(TYPE_PRE_READ_4K == tFtlTask.dTaskType)
	{
		Retry_PreReadGetDmaInfo(tRetryInfor.m_dFailBuffIndex[bChan][bChip][bRetryIdx], ptDmaInfor,bChan,&bLunOfChan);
	}	
	else
#endif
	{
		if(TYPE_TABLE == tFtlTask.bDataType)
		{
			ptDmaInfor->bSecOfClst = SECT_OF_TBLE_CLST;
			ptDmaInfor->bSectOfDma = TSECT_OF_DMA;
		}
		else
		{
			ptDmaInfor->bSecOfClst = SECT_OF_DATA_CLST;
			ptDmaInfor->bSectOfDma = DSECT_OF_DMA;
		}
		
		SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_EN = 0;		//关闭hold中断，如果CQ停在hold指令，有int flag就会触发中断，不管CQ是否继续执行
		__nds32__dsb();
		Retry_ReadGetDmaInfor(tRetryInfor.m_dFailBuffIndex[bChan][bChip][bRetryIdx], ptDmaInfor,&bLunOfChan);
	}
	}
}

void Retry_ReadDMA(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor)
{
	Fdl_MCUReadCacheCmd(bChan, bChip, ptDmaInfor->wColAddr);
	HAL_McuSetMask(bChan, ptDmaInfor->bMaskInfo, MASK_BYTE_OF_CLST,0);						 
	HAL_McuSetRndSeed(bChan, ptDmaInfor->dRowAddr, ptDmaInfor->wSeedIdx);
	HAL_McuSetUd(bChan, ptDmaInfor->dUserDataAddr, ptDmaInfor->bSecOfClst, ptDmaInfor->bUdSize, 1);

	Joint_Mode JointMode = HAL_ADMGetJointMode(bChan);
	HAL_ADMStartDMA2(bChan, ptDmaInfor->dDmaBuffAddr, ptDmaInfor->dDmaInfoAddr, DMA2_RD_TO_BUFF, 
		ptDmaInfor->dBuffIdx, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma, JointMode);

	while(0==SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE);		//解决多片选下，没等done信号，发生种子出错的问题
}

U32 RetryInfoRcd(U08 bLogChan,U08 bLogLun,U08 bSecOfClst,U32 dRow)
{
    U08 bSlcMode = ((dRow>>24)&0x03); //bit24,bit25 1,2,3 ED3Flash
    U08 bTabZone = ((bSecOfClst == SECT_OF_TBLE_CLST)?(TYPE_TABLE):(TYPE_LOGIC));
    U32 dRcdRow = FAIL_ADDR_SET(bTabZone,bSlcMode,bLogChan,bLogLun,(dRow&0xFFFFFF));

    return dRcdRow;
}

void Retry_MultiDMA(U08 bOnlyReadMaxCnt, U08 bSetOptionMaxCnt, U08 bArcMaxCnt)
{
	//static U08 bArcEnableFlag[4][4] = {{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0}};
	U08 bChan; 
	U08 bChip;
    U32 dFailAddr;
	T_DMA_INFOR tDmaInfor;
	U8 bRegNum; //retry配置寄存器哪组
	T_RETRY_CTRL *ptCtrl;

	if(bOnlyReadMaxCnt == 0)
	{
		bOnlyReadMaxCnt = 1;
	}

	//第1阶段，发送读命令
	for (bChan=0; bChan<4; bChan++)
	{
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
		HAL_MCU_GetOwnership(RTY_CNT_MUTIREG,G0CORE);
#else
		bG0VarLock[bChan] = 1;			//增加锁，防止出现G0 retry和G3中断同时发生的情况
		__nds32__dsb();
		while(bG3VarLock[bChan] == 1)
		{
			bG0VarLock[bChan] = 0;
		}
		bG0VarLock[bChan] = 1;
#endif
#endif

		for (bChip=0; bChip<4; bChip++)
		{			
			if ((bStateMachine[bChan][bChip] == RETRY_STATE1) && (tRetryInfor.m_bFailDmaCnt[bChan][bChip] > 0))
			{	
				bStateMachine[bChan][bChip] = RETRY_STATE2;
				ptCtrl = &tRetryCtrl[bChan][bChip]; 
				Retry_GetDmaInfor(bChan, bChip, ptCtrl->bDmaNum, &tDmaInfor);
				//LOG_PRINT("S1,bChan=%d, bChip=%d, bDmaNum=0x%x,bRetryTimes=%d,dBuffIdx=0x%x,Row=0x%x\n", bChan, bChip, ptCtrl->bDmaNum, ptCtrl->bRetryTimes,tDmaInfor.dBuffIdx,tDmaInfor.dRowAddr);

				if(ptCtrl->bEnableARC != 0)
				{
					//bArcEnableFlag[bChan][bChip] = 1;
					if(ptCtrl->bRetryTimes == 0)
					{
						Retry_SetFlashReg_Arc(bChan, bChip, 0x1);	//Enable calibration,next time enable persistence
					}
					else if(ptCtrl->bRetryTimes == 1)
					{
						Retry_SetFlashReg_Arc(bChan, bChip, 0x3);	//Enable calibration,enable persistence
					}
				}
#if (READ_RETRY_ARC_MAX != 0)
				else if(ptCtrl->bEnableReadOffset != 0)
				{
					U8 bData[4] = {0, 0, 0, 0};
					U8 bFeatureAddr = bFeatureAddrArray[ptCtrl->bEnableReadOffset-1];
					bData[0] = bTLCLPReadOffset[ptCtrl->bRetryTimes];	//TLC LP页
					if((ptCtrl->bEnableReadOffset == SLC_LP_PAGE) || (ptCtrl->bEnableReadOffset == MLC_LP_PAGE))	//SLC页
					{
						bData[0] = bSLCReadOffset[ptCtrl->bRetryTimes];
					}
					Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bFeatureAddr, bData);
				}
#endif
				else if (ptCtrl->bSetOptionEn != 0) //设置RetryOption的情况
				{
					if (0 == ptCtrl->bRetryTimes)//配置Retry的起始option
					{
						ptCtrl->bStartOption = bBestOption[bChan][bChip];
						ptCtrl->wMinSvalue = 0xFFFF;	//初始化Svalue
					}
					
					bRegNum = (ptCtrl->bStartOption + ptCtrl->bRetryTimes) % bSetOptionMaxCnt;
										
					if(ptCtrl->bRetryTimes == 0)
					{
						Retry_SetFlashReg_Arc(bChan, bChip, 0x0);	//close calibration,close persistence
					}

					Retry_SetFlashReg(bChan, bChip, bRegNum,tDmaInfor.dRowAddr);
				}

				Fdl_MCUSendPreReadCmd(bChan,bChip,(tDmaInfor.dRowAddr&0xFFFFFF),tDmaInfor.wColAddr,ARRAY(tDmaInfor.dRowAddr,3));
			}
		}
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
		HAL_MCU_ReleaseOwnership(RTY_CNT_MUTIREG, G0CORE);
#else
		bG0VarLock[bChan] = 0;
#endif
#endif
	}

	//第二阶段，启动DMA2进行数据传输
	for (bChan=0; bChan<4; bChan++)
	{
		for (bChip=0; bChip<4; bChip++)
		{	
			if ((bStateMachine[bChan][bChip] == RETRY_STATE2) && (tRetryInfor.m_bFailDmaCnt[bChan][bChip] > 0))
			{		
				bStateMachine[bChan][bChip] = RETRY_STATE3;
				ptCtrl = &tRetryCtrl[bChan][bChip]; 

				U08 bStatus = HAL_MCUCheckStatus(bChan, bChip);
				Retry_GetDmaInfor(bChan, bChip, ptCtrl->bDmaNum, &tDmaInfor);

				if(FALSE == bStatus)	//如果check status失败，重新发00-30命令
				{
					Fdl_MCUSendPreReadCmd(bChan,bChip,(tDmaInfor.dRowAddr&0xFFFFFF),tDmaInfor.wColAddr,ARRAY(tDmaInfor.dRowAddr,3));
					HAL_MCUCheckStatus(bChan, bChip);			
				}
				delay_us(1);
				Retry_ReadDMA(bChan, bChip, &tDmaInfor);
				//LOG_PRINT("S2,bChan=%d, bChip=%d, bDmaNum=0x%x,wColAddr=0x%x\n", bChan, bChip, ptCtrl->bDmaNum,tDmaInfor.wColAddr);				
			}
		}
	}

	//第三阶段，等数据传输及解码完成
	for (bChan=0; bChan<4; bChan++)
	{
		for (bChip=0; bChip<4; bChip++)
		{				
			if ((bStateMachine[bChan][bChip] == RETRY_STATE3) && (tRetryInfor.m_bFailDmaCnt[bChan][bChip] > 0))
			{
				bStateMachine[bChan][bChip] = RETRY_STATE1;
				
				ptCtrl = &tRetryCtrl[bChan][bChip]; 
				Retry_GetDmaInfor(bChan, bChip, ptCtrl->bDmaNum, &tDmaInfor);
				while (0 == SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE);
				
				U16 wDmaSts;
				U08 bMaxRetryCnt;
				U08 bRetryType 		= 0;
				U08 bRecordExcpFlag = 0;								
                
				wDmaSts = (*((U16 *)tDmaInfor.dDmaInfoAddr));
#ifdef __TEST_LLR_MAX_ECC__
				wDmaSts |= (3<<14);
#endif
				//LOG_PRINT("S3, chan=%d,chip=%d,bRetryTimes=%d,wDmaSts=0x%x,dBuffIdx=0x%x\n",bChan,bChip,ptCtrl->bRetryTimes, wDmaSts,tDmaInfor.dBuffIdx);
#ifndef RETRY_SVALUE
				if(DMAINFO_ABNORMAL(wDmaSts))
#else
				if(	(DMAINFO_ABNORMAL(wDmaSts)) ||
					((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) & (DMAINFO_SVALUE(wDmaSts)>RETRY_SVALUE)))
#endif
				{
					if(ptCtrl->bEnableARC == 1)
					{
						bMaxRetryCnt = bArcMaxCnt;
					}
					else if (ptCtrl->bEnableReadOffset > 0)		//setoffset模式
					{
						bMaxRetryCnt = READ_RETRY_OFFSET_MAX;
						if ((wDmaSts & 0x1fff) < ptCtrl->wMinSvalue)
						{
							ptCtrl->wMinSvalue = (wDmaSts&0x1fff);		//保留最小Svalue
							bBestOption[bChan][bChip] = (ptCtrl->bStartOption + ptCtrl->bRetryTimes) % bMaxRetryCnt;;			//保留最小Svalue对应的RetryOption
						}
					}
					else if (ptCtrl->bSetOptionEn == 1)//需要setfeature模式
					{
						bMaxRetryCnt = bSetOptionMaxCnt;
						
						if ((wDmaSts & 0x1fff) < ptCtrl->wMinSvalue)
						{
							ptCtrl->wMinSvalue = (wDmaSts&0x1fff);		//保留最小Svalue
							bBestOption[bChan][bChip] = (ptCtrl->bStartOption + ptCtrl->bRetryTimes) % bMaxRetryCnt;;			//保留最小Svalue对应的RetryOption
						}
					}
					else //不需要setfeature模式
					{
						bMaxRetryCnt = bOnlyReadMaxCnt;
					}

					ptCtrl->bRetryTimes++;

					if (ptCtrl->bRetryTimes >= bMaxRetryCnt)
					{
						//非全1或者条件不满足READ_ALL_ONE_ONLY_RE_RD
						if (!(DMAINFO_ALLONE(wDmaSts) && (tFtlTask.tTaskCtrl->bReadFailCtrl==READ_ALL_ONE_ONLY_RE_RD)))
						{
#if (READ_RETRY_ARC_MAX != 0)
							U8 bSLCOrTLCLPFlag = IFSLC_OR_TLCLP(tDmaInfor.dRowAddr);
#endif
							if ((bSetOptionMaxCnt != 0) && (ptCtrl->bSetOptionEn == 0)) //进入setfeature retry模式
							{
								ptCtrl->bSetOptionEn = 1; //作为从简单read Retry几次到setfeature的切换
								ptCtrl->bRetryTimes = 0;
								continue;
							}
#if (READ_RETRY_ARC_MAX != 0)
							else if((bArcMaxCnt != 0) && (bSLCOrTLCLPFlag > 0))	//SLC页 OR TLC LP页，不走ARC流程；其他页正常走ARC模式
							{
								//Set Offset
								if(ptCtrl->bArcTime == 0)		//只进入一次
								{
									ptCtrl->wMinSvalue = 0xFFFF;
									ptCtrl->bStartOption = 0;
									ptCtrl->bRetryTimes = 0;
									ptCtrl->bArcTime = 1;
									ptCtrl->bEnableReadOffset = bSLCOrTLCLPFlag;
									Retry_SetFlashReg_Arc(bChan, bChip, 0);/*关闭ARC*/
									Retry_SetFlashReg(bChan, bChip, 0,tDmaInfor.dRowAddr);/*恢复Option*/	
									continue;
								}
								else
								{
									U8 bStartOffset = bTLCLPReadOffset[bBestOption[bChan][bChip]];
									if((ptCtrl->bEnableReadOffset == SLC_LP_PAGE) || (ptCtrl->bEnableReadOffset == MLC_LP_PAGE))	//SLC页
									{
										bStartOffset = bSLCReadOffset[bBestOption[bChan][bChip]];
									}									
									U08 pReadOffset[4] = {bStartOffset,0,0,0};
									U8 bFeatureAddr = bFeatureAddrArray[ptCtrl->bEnableReadOffset-1];
#ifdef __FLASH_M26A_2048P__/*M26A半容量支持*/
									if(GET_PAGE_NUM(tDmaInfor.dRowAddr) >= 1904)
									{
										bFeatureAddr -= 0x20;/**/
									}
#endif						
									Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bFeatureAddr, pReadOffset);
									//FDL_LDPC_SoftDecode(bChan, bChip, &tDmaInfor, ARRAY(tDmaInfor.dRowAddr,3));
									wDmaSts = (*((volatile U16 *)tDmaInfor.dDmaInfoAddr));		//先做一次软解码
#ifndef RETRY_SVALUE
									if(DMAINFO_ABNORMAL(wDmaSts))
#else
									if(	(DMAINFO_ABNORMAL(wDmaSts)) ||
										((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) & (DMAINFO_SVALUE(wDmaSts)>RETRY_SVALUE)))
#endif
									{
										bRetryType = RD_ERR_DATA_ERROR;/*软解Fail*/
									}
									else
									{
										bRetryType = RD_ERR_SOFT_PASS;/*软解Pass*/
									}
								}
							}
							else if((bArcMaxCnt != 0) && (((tDmaInfor.dRowAddr >>24) != 0)||(tFtlTask.dTaskType < TYPE_MULTI_LUN_INDIE_COPYBACK_ORDER_PRG)))	//copyback流程不能开ARC
							{
								ptCtrl->bEnableARC = 1; //作为从SETFEATURE到ARC的切换
								ptCtrl->bRetryTimes = 0;
								
								//进入软解码流程
								if(ptCtrl->bArcTime != 0)
								{
									bArcStartOffset = bArcCorect[ptCtrl->bArcTime-1];		
									//FDL_LDPC_SoftDecode(bChan, bChip, &tDmaInfor, ARRAY(tDmaInfor.dRowAddr,3));
									wDmaSts = (*((volatile U16 *)tDmaInfor.dDmaInfoAddr));
								}
								
#ifndef RETRY_SVALUE
								if(DMAINFO_ABNORMAL(wDmaSts))
#else
								if(	(DMAINFO_ABNORMAL(wDmaSts)) ||
									((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) & (DMAINFO_SVALUE(wDmaSts)>RETRY_SVALUE)))
#endif
								{
									Retry_SetFlashReg_Arc(bChan, bChip, 0); 
									Retry_SetFlashReg(bChan, bChip, 0,tDmaInfor.dRowAddr);
									
									U08 pReadOffset[4] = {bArcCorect[ptCtrl->bArcTime],0,0,0};
									Retry_SetArcStartOffset(bChan,bChip,pReadOffset);
									if(ptCtrl->bArcTime < ARC_CYCLE_CNT - 1)
									{
										ptCtrl->bArcTime++;
										//FDL_Log("ARC>%d",ptCtrl->bArcTime);
										continue;
									}	
								}
								else
								{
									bRetryType = RD_ERR_SOFT_PASS;/*软解Pass*/
									//PRINT_HEX("SR>",tDmaInfor.dRowAddr);
									//PRINT_HEX("SC>",tDmaInfor.wColAddr);
									//PRINT_HEX("ST>",ptCtrl->bArcTime-1);
								}
							}
#endif
						}

#if (READ_RETRY_ARC_MAX == 0)//进入软解码流程
						if (!(DMAINFO_ALLONE(wDmaSts) && (tFtlTask.tTaskCtrl->bReadFailCtrl==READ_ALL_ONE_ONLY_RE_RD)))
						{
							//LOG_PRINT("bChan = %d, bChip = %d, bBestOption = %d\n", bChan, bChip, bBestOption[bChan][bChip]);
							Retry_SetFlashReg(bChan, bChip, bBestOption[bChan][bChip],tDmaInfor.dRowAddr);
#ifdef __TEST_LLR_MAX_ECC__
							Fdl_MCUSendPreReadCmd(bChan,bChip,tDmaInfor.dRowAddr,tDmaInfor.wColAddr,ARRAY(tDmaInfor.dRowAddr,3));
							Fdb_RetryLdpcEccGet(bChan,bChip,&tDmaInfor);
#endif
							//FDL_LDPC_SoftDecode(bChan, bChip, &tDmaInfor, ARRAY(tDmaInfor.dRowAddr,3));
							wDmaSts = (*((volatile U16 *)tDmaInfor.dDmaInfoAddr));
#ifndef RETRY_SVALUE
							if((DMAINFO_ABNORMAL(wDmaSts)))/*Retry  Fail*/
#else
							if(	(DMAINFO_ABNORMAL(wDmaSts)) ||
								((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) & (DMAINFO_SVALUE(wDmaSts)>RETRY_SVALUE)))
#endif
							{
								bRetryType = RD_ERR_DATA_ERROR;/*软解Fail*/
							}
							else
							{
								bRetryType = RD_ERR_SOFT_PASS;/*软解Pass*/
							}
						}
#endif
						//判断Fail置上水位

						if(DMAINFO_ABNORMAL(wDmaSts))
						{
							/*Retry Fail考虑重走Retry流程*/
							if(ptCtrl->bRetryLoopCnt > 0)
							{								
								ptCtrl->bArcTime 		= 0;
								ptCtrl->bEnableARC 		= 0;
								ptCtrl->bRetryTimes 	= 0;
								ptCtrl->bSetOptionEn 	= 0;								
								ptCtrl->bEnableReadOffset = 0;
								ptCtrl->bRetryLoopCnt--;
								FDL_FlashIFModeInit(bChan,bChip);/*重新复位Flash*/
								//PRINT_HEX("Retry Loop>",ptCtrl->bRetryLoopCnt);
								continue;
							}
													
							if(JOINT_NONE != SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE)/*联动读Retry Fail,置水位*/
							{
								if(tDmaInfor.bMaskInfo == 0)
								{
									HAL_RTY_BFCSetBuffStatus((tDmaInfor.dBuffIdx%SG_ADM->rBUF_SIZE),(((U32)0x1<<SECT_OF_DMA)-1));
								}
								else
								{
									U8 bK;

									for (bK = 0; bK < SECT_OF_DMA; bK++)
									{
										if(0 == ((tDmaInfor.bMaskInfo >> bK)& 0x01))
										{
											HAL_RTY_BFCSetBuffStatusBit(((tDmaInfor.dBuffIdx+bK)%SG_ADM->rBUF_SIZE));
										}
									}
								}
							}
							
							if( (TYPE_TABLE == tFtlTask.bDataType) && 
								(TYPE_READ_TO_BUFFER == tFtlTask.dTaskType))/*表格Retry Fail,将Buff全部置成全FF*/
							{		
								
								MemorySet((U08 *)tDmaInfor.dDmaBuffAddr + tDmaInfor.dBuffIdx*SECTOR_SIZE,0xFF,tDmaInfor.bSectOfDma*SECTOR_SIZE);
								MemorySet((U08 *)(tDmaInfor.dUserDataAddr + tDmaInfor.bFlashIdx),0xFF,tDmaInfor.bUdSize);
							}							
						}
						
						SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;

						if(tRetryInfor.m_OpType == COPYBACK_RETRY_MODE)  //CopyBack Retry 信息记录
						{
							U08 bRtyIdx = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip]+ptCtrl->bDmaNum;

							if(DMAINFO_ALLONE(wDmaSts))
							{
								tRetryInfor.m_tDmaInfor[bRtyIdx].m_bBuffDmaIndex = 0xFF;
							}
							else if (DMAINFO_ABNORMAL(wDmaSts)==0)	//Retry Pass
							{
								tRetryInfor.m_tDmaInfor[bRtyIdx].m_bBuffDmaIndex = 0xF0+RD_ERR_SOFT_PASS;
							}
							tRetryInfor.m_dFailDmaTotal--;			
						}

						//进行下一个DMA的retry
						tRetryInfor.m_bFailDmaCnt[bChan][bChip]--;
						ptCtrl->bDmaNum++;
						bRecordExcpFlag = 1;
					}

					
				}
				else
				{
					/*Retry Type收集上报*/
					if(1 == ptCtrl->bEnableARC)/*ARC Pass*/
					{
						bRetryType = RD_ERR_ARC_PASS;
					}
					else if(ptCtrl->bEnableReadOffset > 0)
					{
						bRetryType = RD_ERR_ARC_PASS;
					}
					else if(1 == ptCtrl->bSetOptionEn)/*Set Option Pass*/
					{
						bRetryType = RD_ERR_OPTION_PASS;
#ifdef __RETRY_TABLE_RATE__
						if(wRRTUseCnt[bBestOption[bChan][bChip]] < 0xFFFF)
						{
							wRRTUseCnt[bBestOption[bChan][bChip]]++;
							wRRTUseCnt[RRT_TABLE_CNT] = 0xDEAD;//标识内容变化
						}
#endif
					}
					else /*重读Pass*/
					{
						//PRINT_TXT("re read");
						bRetryType = RD_ERR_REREAD_PASS;
					}

					if(tRetryInfor.m_OpType == COPYBACK_RETRY_MODE)  //CopyBack Retry Pass信息记录
					{
						U08 bRtyIdx = tRetryInfor.m_bCBFailDmaOffst[bChan][bChip]+ptCtrl->bDmaNum;
						tRetryInfor.m_tDmaInfor[bRtyIdx].m_bBuffDmaIndex = 0xF0 + bRetryType;
						tRetryInfor.m_dFailDmaTotal--;
					}
										
					if ((ptCtrl->bSetOptionEn == 1) && (ptCtrl->bEnableARC != 1))//需要setfeature模式
					{
						//Retry成功，认为这是最好的retry option
						bBestOption[bChan][bChip] = (ptCtrl->bStartOption + ptCtrl->bRetryTimes) % bSetOptionMaxCnt;
					}
					tRetryInfor.m_bFailDmaCnt[bChan][bChip]--;
					ptCtrl->bDmaNum++;
					bRecordExcpFlag = 1;
				}
				//记录异常信息前，此DMA已经Retry结束，把flash的retryoption恢复默认值
				
				if (1 == bRecordExcpFlag)
				{
#ifdef __RETRY_PERSISTENCE__/*开启Retry保持*/				
					if(!DMAINFO_ABNORMAL(wDmaSts))
					{												
						if(ptCtrl->bEnableARC==1)
						{
							U08 bArcCfg = 0x2;	//N18 Flash,解码成功，开启persistence
							Retry_SetFlashReg_Arc(bChan, bChip, bArcCfg);	
						}
						
						if(bRetryType != RD_ERR_REREAD_PASS)
						{
							bRetryPersistence[bPhyToLogChan[bChan]][bPhyCeToLogLun[bChip]] = TRUE;
						}
					}
					else
					{
						Retry_SetFlashReg(bChan, bChip, 0,tDmaInfor.dRowAddr);	//SetFeature恢复为diasble(default)						
						if(1 == ptCtrl->bEnableARC)/*ARC Pass*/
						{
							Retry_SetFlashReg_Arc(bChan, bChip, 0x00);	//关闭并清零ARC功能
							U08 pReadOffset[4] = {0,0,0,0};
							Retry_SetArcStartOffset(bChan,bChip,pReadOffset);
						}
					
#if (READ_RETRY_ARC_MAX != 0)
						if(ptCtrl->bEnableReadOffset > 0)
						{
							U8 bData[4] = {0, 0, 0, 0};
							U8 bFeatureAddr = bFeatureAddrArray[ptCtrl->bEnableReadOffset-1];
							Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bFeatureAddr, bData);
						}
#endif
					}
					
#else/*不开启Retry保持,SetFeature&ARC恢复为diasble(default)*/
					if(1 == ptCtrl->bEnableARC)/*ARC Pass*/
					{
						Retry_SetFlashReg_Arc(bChan, bChip, 0x00);	//关闭并清零ARC功能
						U08 pReadOffset[4] = {0,0,0,0};
						Retry_SetArcStartOffset(bChan,bChip,pReadOffset);
					}
					
					if(1 == ptCtrl->bSetOptionEn)
					{
						Retry_SetFlashReg(bChan, bChip, 0,tDmaInfor.dRowAddr);	//SetFeature恢复为diasble(default)
					}
					
#if (READ_RETRY_ARC_MAX != 0)
					if(ptCtrl->bEnableReadOffset > 0)
					{
						U8 bData[4] = {0, 0, 0, 0};
						U8 bFeatureAddr = bFeatureAddrArray[ptCtrl->bEnableReadOffset-1];
						Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bFeatureAddr, bData);
					}
#endif
#endif
					//PRINT_HEX("RTYR:",tDmaInfor.dRowAddr);
					//PRINT_HEX("RTYP:",bRetryType);
				}
				
				if(1 == bRecordExcpFlag)
				{
					ptCtrl->bSetOptionEn = 0;		
					ptCtrl->bRetryTimes = 0;	
					ptCtrl->bEnableARC = 0;
					ptCtrl->bArcTime = 0;
					ptCtrl->bEnableReadOffset = 0;
					(*((volatile U16 *)tDmaInfor.dDmaInfoAddr)) &= (0x3FFF); 
				}
				
				//FTL保存异常信息
				if ((1 == bRecordExcpFlag) && (bRetryType != RD_ERR_REREAD_PASS))//只考虑onlyread = 1;onlyread=0有问题
				{
					PRINT_HEX("Retry:",tDmaInfor.dRowAddr);
					FDL_Log("bChan=%d,bChip=%d,bPhyToLogChan=%d,bPhyCeToLogLun=%d",bChan,bChip,bPhyToLogChan[bChan],bPhyCeToLogLun[bChip]);
#ifdef __RETRY_CNT_RECORD__
    				U08 bRcdMode;
                    /*记录Retry信息*/
                    if(tFtlTask.tTaskCtrl->bReadFailCtrl == READ_FAIL_ALL_RETRY)
                    {
                        pFdlParam.FTL_CollectRetryInfor(bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],0,0);//ftl保证实现该回调,各chan各lun的retry次数计数

#ifndef RETRY_SVALUE
						if(DMAINFO_ABNORMAL(wDmaSts))
#else
						if(	(DMAINFO_ABNORMAL(wDmaSts)) ||
							((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) & (DMAINFO_SVALUE(wDmaSts)>RETRY_SVALUE)))
#endif
                        {
                            dFailAddr = RetryInfoRcd(bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],tDmaInfor.bSecOfClst,tDmaInfor.dRowAddr);
                            
                            PRINT_HEX("fail_r:",dFailAddr);
                            PRINT_HEX("c:",tDmaInfor.wColAddr);
                            PRINT_HEX("t:",tFtlTask.dTaskType);
                            PRINT_HEX("S:",wDmaSts);
                            (tRetryInfor.m_OpType != VAR_TLC_PAGE_MODE)?(bRcdMode = 2):(bRcdMode = 3);//tlc的retry fail只记次数，不记地址

                            if(tFtlTask.dTaskType >= TYPE_VERIFY_TLC_PAGE)
                            {
                            	bRcdMode|=0x10;
                            }
                            pFdlParam.FTL_CollectRetryInfor(bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],bRcdMode,dFailAddr);
                        }
                    }   
#endif
				}
				
				//FDL保存异常信息
				if ((1 == bRecordExcpFlag) && 
					(tRetryInfor.m_OpType != COPYBACK_RETRY_MODE))
				{
					U32 dCurClstOff;
					U32 dNextClstOff;

					//FDL_Log("bRetryType:%d-0x%X",bRetryType,wDmaSts);
					bClstExcpInfo[bChan][bChip] |= Retry_AnalyzeExceptionInfo(bChan, wDmaSts, bRetryType);
					dCurClstOff = tRetryInfor.m_dFailBuffIndex[bChan][bChip][ptCtrl->bDmaNum-1]/tDmaInfor.bSecOfClst;
					
					if (0 != tRetryInfor.m_bFailDmaCnt[bChan][bChip])
					{
						dNextClstOff = tRetryInfor.m_dFailBuffIndex[bChan][bChip][ptCtrl->bDmaNum]/tDmaInfor.bSecOfClst;	//下一个DMA所在的簇号
					}
					else
					{
						dNextClstOff = 0xFFFFFFFF;				//无效簇地址
					}
					
					if (dCurClstOff != dNextClstOff)
					{
						if (bClstExcpInfo[bChan][bChip]!=0)
						{
							Retry_SaveTaskException(bChan, dCurClstOff, bClstExcpInfo[bChan][bChip]);
							//LOG_PRINT("ClstOff=0x%x, ExcpInfo=0x%x\n", dCurClstOff, bClstExcpInfo[bChan][bChip]);
							bClstExcpInfo[bChan][bChip] = 0;
						}
					}
				}
			}
		}
	}
}

U08 Retry_FailDmaCnt(U08 bChan)
{
    if ((tRetryInfor.m_bFailDmaCnt[bChan][0] == 0) 
        && (tRetryInfor.m_bFailDmaCnt[bChan][1] == 0) 
        && (tRetryInfor.m_bFailDmaCnt[bChan][2] == 0) 
        && (tRetryInfor.m_bFailDmaCnt[bChan][3] == 0))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void Retry_CQPauseHandle(U08 bChan)
{
    SG_DMA2_CH(bChan)->rSTATUS.dAll = 0; 
    __nds32__dsb(); //清除flag
    SG_FC(bChan)->rCQ_HOLD_CTRL.bits.DMA2_ABNL_HOLD_CQ_FLAG = 0;
    __nds32__dsb();
    SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_FLAG = 0;
    __nds32__dsb(); //clear int infor
    SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_EN = 1; 
    __nds32__dsb(); //恢复hold中断
    SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0;        //retry结束退出HP模式
    
    U08MemSet((U08*)tRetryCtrl[bChan], 0x00, sizeof(tRetryCtrl[bChan]));
    U08MemSet((U08*)bStateMachine[bChan], 0x00, sizeof(bStateMachine[bChan]));
}

void Retry_FailRcdType(U08 bChan,U08 bType)
{
    SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 1; //后面retry流程直接进入HP模式
    //tRetryVar.wLastClstOffs[bChan] = wFinalOfst; //最后一个簇偏移
    tRetryVar.dRetryDma2BufIndex[bChan] = SG_DMA2_CH(bChan)->rINDEX_CFG;        //记录check hold前配置的，retry退出前恢复
    tRetryVar.bRetryDma2TotalSect[bChan] = SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG;  //记录check hold前配置的，retry退出前恢复
    SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_EN = 1; //暂停CQ
    __nds32__dsb();
    tRetryInfor.m_OpType = bType;
    tRetryVar.bLDPCFailFlag[bChan] = 1;     //记录Fail的标志
}
void Retry_CQRecover(U08 bChan)
{
    SG_DMA2_CH(bChan)->rINDEX_CFG        = tRetryVar.dRetryDma2BufIndex[bChan];     //记录check hold前配置的，retry退出前恢复
    SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG   = tRetryVar.bRetryDma2TotalSect[bChan];        //记录check hold前配置的，retry退出前恢复
    
    tRetryVar.bLDPCFailFlag[bChan] = 0;
    SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_EN = 0;
    __nds32__dsb();
}


#ifdef __VER_TLC_PAGE__
void Retry_RestartCQVerTLC()
{
	U08 bChan;
	T_ADDR_INFO tAddr;
	//恢复CQ执行
	for (bChan=0; bChan<4; bChan++)
	{
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
		HAL_MCU_GetOwnership(RTY_CNT_MUTIREG,G0CORE);
#else

		bG0VarLock[bChan] = 1;
		__nds32__dsb();
		while(bG3VarLock[bChan] == 1)
		{
			bG0VarLock[bChan] = 0;
		}
		bG0VarLock[bChan] = 1;
#endif
#endif

        if(Retry_FailDmaCnt(bChan))
		{
			if (1==SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_FLAG)
			{	
                Retry_CQPauseHandle(bChan);

                Retry_CQRecover(bChan);
			}
		}
		bG0VarLock[bChan] = 0;
		__nds32__dsb();
	}
}

#endif


void Retry_RestartCQ()
{
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;
	T_ADDR_INFO tAddr;
	U08 bChan,bFlashMode;
	//恢复CQ执行
#ifdef __GRAY_BOX_TEST__
	FDL_GrayBoxRetryOption();
#endif

	for (bChan=0; bChan<4; bChan++)
	{
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
		HAL_MCU_GetOwnership(RTY_CNT_MUTIREG, G0CORE);
#else
		bG0VarLock[bChan] = 1;
		__nds32__dsb();
		while(bG3VarLock[bChan] == 1)
		{
			bG0VarLock[bChan] = 0;
		}
		bG0VarLock[bChan] = 1;
#endif
#endif

#if 0
		if ((tRetryInfor.m_bFailDmaCnt[bChan][0] == 0) 
			&& (tRetryInfor.m_bFailDmaCnt[bChan][1] == 0) 
			&& (tRetryInfor.m_bFailDmaCnt[bChan][2] == 0) 
			&& (tRetryInfor.m_bFailDmaCnt[bChan][3] == 0))
#else
        if(Retry_FailDmaCnt(bChan))
#endif 
		{
			if (1==SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_EN)
			{		
#if 0
				SG_DMA2_CH(bChan)->rSTATUS.dAll = 0; __nds32__dsb(); //清除flag
				SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN = 0; __nds32__dsb();
				SG_FC(bChan)->rCHK_INT_ST.bits.CQ_HOLD_FLAG = 0; __nds32__dsb(); //clear int infor
				SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_EN = 1; __nds32__dsb(); //恢复hold中断
				
				SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0;		//retry结束退出HP模式

				U08MemSet(tRetryCtrl[bChan], 0x00, sizeof(tRetryCtrl[bChan]));
				U08MemSet(bStateMachine[bChan], 0x00, sizeof(bStateMachine[bChan]));
#else
                Retry_CQPauseHandle(bChan);
#endif
				//恢复Cache里的数据
				if(tRetryVar.wRetryLunBit[bChan] > 0)
				{
					U08 bRetryLun;
					U16 wLunFailOfst;
					for(bRetryLun=0; bRetryLun < MAX_LUN_OF_CHAN; bRetryLun++)
					{
						if(tRetryVar.wRetryLunBit[bChan]&(1<<bRetryLun) && (tRetryVar.wLastClstOffs[bChan] < tFtlTask.wLen))
						{
							wLunFailOfst = Fdl_SearchOfstByAddr(bChan,bRetryLun,0,tRetryVar.wLastClstOffs[bChan],BACKWARD_SEARCH_LUN_ADDR_OFST);

							if(wLunFailOfst != FAIL_SEARCH_ADDR_OFST)
							{							
								ptClstAddr 	= &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wLunFailOfst % tFtlTask.wContiAddrMax]);
								Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
								if((tFtlTask.dTaskType == TYPE_READ_TO_BUFFER_4K) || (tFtlTask.dTaskType == TYPE_NCQ_READ_4K))
								{
									bFlashMode = tAddr.dRow >> 24;
									//LOG_PRINT("bFlashMode2 = %d",bFlashMode);
									Fdl_MCUReadCmd(bChan, tAddr.bChip, tAddr.dRow, tAddr.wCol, bFlashMode);	
								}
								else
								{
#ifdef __CACHE_READ_ON__
#ifdef __TAB_CACHE_RD_OFF__
									if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))
#else
									if(CACHE_READ_EN)/*#@CacheRead#*/
#endif
									{
										U32 dLogicNum = HAL_CQGetLogicNumExt(bChan);
										U08 bLastAddrOPMode = dLogicNum&0x0F;
										U08 bCacheOpFlag = (dLogicNum>>4)&0x0F;

										if(	(bLastAddrOPMode == NORMAL_OP_MODE) ||/**/
											((bLastAddrOPMode == CACEH_OP_MODE) && 
											(bCacheOpFlag == 0)))
										{
											/*	Retry 后，要读的数据在Cache Reg(一级缓存)里面或者
												Retry 后，Data Reg和Cache Reg里面都不存在数据----无所谓怎么做
												主流程会处理*/
											Fdl_MCUMultiPlnReadCmd(bChan,tAddr.bChip,tAddr.bLun,tAddr.dRow);
											
#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
											U32 dRowCache = Fdl_ED3RowAnalyze(tAddr.dRow,0xFFFFFFFF,0);
#else
											U32 dRowCache = tAddr.dRow + 1;
#endif										
											Fdl_MCU_MultiPlnCacheReadCmd(bChan,tAddr.bChip,tAddr.bLun,dRowCache);
										}
										else if((bLastAddrOPMode == CACEH_OP_MODE) &&/*Data Reg*/
												(bCacheOpFlag == 1))
										{
											/*Retry 后要读的数据在Data Reg(二级缓存)里面------对搜到的地址减一，然后发00-30h,31h*/
#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
											U32 dRowCache = Fdl_ED3RowAnalyze(tAddr.dRow,0xFFFFFFFF,2);
#else
											U32 dRowCache = tAddr.dRow - 1;
#endif	
											Fdl_MCUMultiPlnReadCmd(bChan,tAddr.bChip,tAddr.bLun,dRowCache);
											Fdl_MCU_MultiPlnCacheReadCmd(bChan,tAddr.bChip,tAddr.bLun,tAddr.dRow);
										}
										else
										{
											/*Retry 后要读的数据同时存在于Data Reg和Cache Reg里面-----直接对搜到的地址发00-30h,31h*/
											Fdl_MCUMultiPlnReadCmd(bChan,tAddr.bChip,tAddr.bLun,tAddr.dRow);
#if TLC_ED3_MODE_SUPPORT/*兼容ED3类型Flash*/
											U32 dRowCache = Fdl_ED3RowAnalyze(tAddr.dRow,0xFFFFFFFF,0);
#else
											U32 dRowCache = tAddr.dRow + 1;
#endif
											Fdl_MCU_MultiPlnCacheReadCmd(bChan,tAddr.bChip,tAddr.bLun,dRowCache);
										}
									}
									else
									{										
										Fdl_MCUMultiPlnReadCmd(bChan,tAddr.bChip,tAddr.bLun,tAddr.dRow);
									}
#else
									Fdl_MCUMultiPlnReadCmd(bChan,tAddr.bChip,tAddr.bLun,tAddr.dRow);
#endif
								}
							}
							else
							{
								tChTaskQueue[bChan].tLunTask[bRetryLun].dCmdRow = 0xFFFFFF;//后面没有任务，不需要恢复
							}
						}
					}
					tRetryVar.wRetryLunBit[bChan] = 0;		//清零
				}
#if 0
				SG_DMA2_CH(bChan)->rINDEX_CFG		 = tRetryVar.dRetryDma2BufIndex[bChan];		//记录check hold前配置的，retry退出前恢复
				SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG 	 = tRetryVar.bRetryDma2TotalSect[bChan];		//记录check hold前配置的，retry退出前恢复

				tRetryVar.bLDPCFailFlag[bChan] = 0;
				SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_PAUSE_FLAG = 0; 
				__nds32__dsb();
#else
                Retry_CQRecover(bChan);
#endif 
			}
		}
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
		HAL_MCU_ReleaseOwnership(RTY_CNT_MUTIREG, G0CORE);
#else
		bG0VarLock[bChan] = 0;
#endif
#endif
	}
}

/******************************************************
描述：Retry接口，由G0调用
参数：
	无
*******************************************************/
void FDL_ReadRetry(void)
{
#ifdef RETRY_IN_G0
	U32 __sda_addr=__get_sda_base();

	if ((tRetryVar.bLDPCFailFlag[0] != 0) || (tRetryVar.bLDPCFailFlag[1] != 0) || 
		(tRetryVar.bLDPCFailFlag[2] != 0) || (tRetryVar.bLDPCFailFlag[3] != 0))
	{
		Retry_MultiDMA(pFdlParam.tReadFailCtrl.m_Fail_ReRdCnt,READ_RETRY_SET_OPTION_MAX, READ_RETRY_ARC_MAX);

		if (tRetryInfor.m_OpType == READ_RETRY_MODE) 
		{
			Retry_RestartCQ();
		}
		else if(tRetryInfor.m_OpType == VAR_TLC_PAGE_MODE)
		{
#ifdef __VER_TLC_PAGE__
			Retry_RestartCQVerTLC();
#endif
		}

	}
	__restore_sda_base(__sda_addr);
#else
	while ((tRetryVar.bLDPCFailFlag[0] != 0) || (tRetryVar.bLDPCFailFlag[1] != 0) ||
			(tRetryVar.bLDPCFailFlag[2] != 0) || (tRetryVar.bLDPCFailFlag[3] != 0))
	{
		Retry_MultiDMA(pFdlParam.tReadFailCtrl.m_Fail_ReRdCnt,READ_RETRY_SET_OPTION_MAX,READ_RETRY_ARC_MAX);

		if (tRetryInfor.m_OpType == READ_RETRY_MODE) 
		{
			Retry_RestartCQ();
		}
		else if(tRetryInfor.m_OpType == VAR_TLC_PAGE_MODE)
		{
#ifdef __VER_TLC_PAGE__
			Retry_RestartCQVerTLC();
#endif
		}
	}
#endif
}

void FDL_CopyBackRetry(void)
{
	U08 i;
	U08 bChan;
	
	for(i = 0; i < 4; i++)
	{
		SG_DMA2_CH(i)->rECC_CTRL.bits.BYPASS_LP = 1; //Retry LDPC模块直接进入HP模式
	}
	
	while(tRetryInfor.m_dFailDmaTotal)
	{
		Retry_MultiDMA(pFdlParam.tReadFailCtrl.m_Fail_ReRdCnt,READ_RETRY_SET_OPTION_MAX, READ_RETRY_ARC_MAX);
	}
	
	for (i = 0; i < CHAN_MAX_CNT; i++)
	{
		bChan = CHANS(i);
		Fdl_ClearExceptionState(bChan);
		SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0; //retry结束退出HP模式	
	}	
	
	U08MemSet((U08*)tRetryCtrl, 0x00, sizeof(tRetryCtrl));
	U08MemSet((U08*)bStateMachine, 0x00, sizeof(bStateMachine));
}

U16 FDL_LittleDataRetry(U08 bChan,U08 bChip,U32 dBuffAddr,U32 dRow,U16 wColAddr,U32 dDmaInforAddr)
{
	U16 wSvalue;
	U08 bRtyOption;
	U16 wExptInfor = 0;
	U16 wMinSvalue = 0xFFFF;
	U08 bSetOptionCnt = READ_RETRY_SET_OPTION_MAX;
	U08 bReRdCnt = pFdlParam.tReadFailCtrl.m_Fail_ReRdCnt;
	
	bRtyOption = bBestOption[bChan][bChip];
	
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 1; //Retry LDPC模块直接进入HP模式

	while(1)
	{
		LOG_PRINT("Little Retry \n");
		if (bReRdCnt == 0)		 //设置RetryOption的情况
		{
			Retry_SetFlashReg(bChan, bChip,bRtyOption%READ_RETRY_SET_OPTION_MAX,dRow);
		}

		HAL_MCUCheckStatus(bChan, bChip);
		Fdl_MCUSendPreReadCmd(bChan,bChip,dRow,wColAddr,SLC_MODE);
		HAL_MCUCheckStatus(bChan, bChip);

		Fdl_MCUReadCacheCmd(bChan, bChip, wColAddr);
		HAL_McuSetRndSeed(bChan,dRow,0xffff);
		HAL_ADMStartDMA2(bChan,dBuffAddr,dDmaInforAddr,DMA2_RD_TO_BUFF,0,0,1,JOINT_NONE);
		while(0==SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE); //多片选时等FC Done

		if(bReRdCnt > 0)
		{
			bReRdCnt--;
		}
		else
		{
			bSetOptionCnt--;
		}
		
		if((SG_DMA2_CH(bChan)->rSTATUS.bits.ECC_FAIL_FLAG == 0) ||(SG_DMA2_CH(bChan)->rSTATUS.bits.CRC_FAIL_FLAG == 1))	//Retry OK
		{		
			wExptInfor = (0x01<<RD_ERR_OPTION_PASS);/*小数据Retry,不管怎么Pass,都报RD_ERR_OPTION_PASS*/
			break;	
		}
		
		if((bReRdCnt == 0)&&(SG_DMA2_CH(bChan)->rSTATUS.bits.DATA_ALL1_FLAG == 0))
		{
			wSvalue = SG_DMA2_CH(bChan)->rECC_STATUS.bits.SVALUE;
			if (wSvalue < wMinSvalue)
			{
				wMinSvalue = wSvalue;
				bBestOption[bChan][bChip] = bRtyOption%READ_RETRY_SET_OPTION_MAX;
			}
			
			bRtyOption++;
		}

		if ((bReRdCnt == 0)&&(SG_DMA2_CH(bChan)->rSTATUS.bits.DATA_ALL1_FLAG == 1))	//需要setfeature模式
		{
			wExptInfor = (0x01<<RD_ERR_ALL_ONE);
			break;	
		}
		else if((bSetOptionCnt == 0)&&((SG_DMA2_CH(bChan)->rSTATUS.bits.ECC_FAIL_FLAG == 1)||(SG_DMA2_CH(bChan)->rSTATUS.bits.CRC_FAIL_FLAG == 1)))
		{
			wExptInfor = (0x01<<RD_ERR_DATA_ERROR);
			break;	
		}

		SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;
	}	
	
	//软解码函数...
	
	Fdl_ClearExceptionState(bChan);
	
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0; //retry结束退出HP模式
	return wExptInfor;
}


#ifdef __VER_TLC_PAGE__
void FDL_RetryRecordFailInfoVerTLC(U8 bChan)
{
	U16 wDma2Status;
	U32 dDMAInfoIndex;/*2Byte为单位*/
	U08 bRetryMode = NORMAL_RETRY; //记录当次DMA的Retry方式	
	U32 dLogNumExt = VER_TLC_LOG_NUM_EXT(bChan);

	U08 bChanPhy = bChan;
	U08 bLogChip = VER_TLC_GET_LOG_CE(dLogNumExt);

	U08 bChipPhy 	= CHIPES(bChanPhy,bLogChip);
	U32 dBuffIdx 	= bChanPhy*DSECT_OF_PAGE;/*Buff Index*/
	U16 *pwDmaInfo 	= (U16 *)(tFtlTask.dEccStsBuff);/*DMA Info Base Addr*/

	HAL_ADMWaitDMADone(bChanPhy); //wait DMA2 real done
	
	U08 bDMACntOfPage   = VER_TLC_DMA_CNT_PAGE;/*每个Page的DMA个数*/
	U16 wDMAInfoStart	= VER_TLC_ECC_INFO_OFSEX(bChanPhy,bLogChip)/2;/*一个Index表示2Byte,要除以2*/
	U16 wDMAInfoEnd 	= wDMAInfoStart + PLANE_OF_LUN*bDMACntOfPage;


	
/*G3 G0锁方式，保证不会同时操作共享变量*/
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_GetOwnership(RTY_CNT_MUTIREG, G3CORE);
#else
	bG3VarLock[bChan] = 1;
	__nds32__dsb();
	while(bG0VarLock[bChan] == 1);
#endif
#endif

	for(dDMAInfoIndex = wDMAInfoStart;dDMAInfoIndex < wDMAInfoEnd;dDMAInfoIndex++)//search next error dma
	{		
		wDma2Status  = pwDmaInfo[dDMAInfoIndex];
				
		if(DMAINFO_ABNORMAL(wDma2Status)) //记录异常，包括：全1，全0，fail
		{	
			if(bRetryMode != NO_RETRY)/*找到一个Fail的DMA*/
			{		

				U08 (*ptRetryDMAInfo)[4][4] = (U08 *)tRetryInfor.m_tDmaInfor;
				U08 *FailDMAIndex			= (U08 *)(&ptRetryDMAInfo[bChanPhy][bChipPhy]);

				/*对m_dFailBuffIndex进行特殊处理,方便记录异常信息*/
				dBuffIdx = (VER_TLC_LUN_TASK_OFSET(bChanPhy,bLogChip)<<12);/*只记录Lun所在的Ofset*/
				dBuffIdx *= SECT_OF_DATA_CLST;/*乘以8是为了兼容Retry里面保存异常信息的代码:除以tDmaInfor.bSecOfClst*/
				tRetryInfor.m_dFailBuffIndex[bChanPhy][bChipPhy][tRetryInfor.m_bFailDmaCnt[bChanPhy][bChipPhy]] = dBuffIdx; //保存出错的信息
				FailDMAIndex[tRetryInfor.m_bFailDmaCnt[bChanPhy][bChipPhy]] = dDMAInfoIndex - wDMAInfoStart;				
				tRetryVar.wRetryLunBit[bChanPhy] |= ((U16)0x01 << bLogChip);//该LogLun需要进行MCU Retry
				tRetryInfor.m_bFailDmaCnt[bChanPhy][bChipPhy]++;	

				if (0==tRetryVar.bLDPCFailFlag[bChanPhy])		//只记录一次就可以
				{
                    Retry_FailRcdType(bChanPhy,VAR_TLC_PAGE_MODE);
				}
			}	
		}
	}
	
#ifdef RETRY_IN_G0
#ifndef __FDL_MCU_MUTUAL__
	bG3VarLock[bChanPhy] = 0;
#endif
#endif
}

#endif

void FDL_RetryRecordFailInfor(U8 bChan)
{
    U08 bMaskDmaInfo;
	U08 bSecOfClst;		
	U16 wDma2Status;
	U32 dBuffIdx;
	U32 dDma2IndexFail;
	U32 dDma2IndexLdpc;
	U32 dOfstInTask; //当前DMA buffindex对应的簇offset
	U32 dOfstOfPre = INVALID_U32; //前一个DMA buffIdx对应的簇offset
	U08 bRetryMode = NORMAL_RETRY; //记录当次DMA的Retry方式
	U16 wFinalOfst = (SG_FC(bChan)->rLOGIC_NUM & 0xFFFFFF);//记录该同通道当前传输到的ofst
	U16 *pwDmaInfo = (U16 *)(tFtlTask.dEccStsBuff);
	
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;
	T_ADDR_INFO tAddr;

	if(TYPE_TABLE == tFtlTask.bDataType)
	{
		bSecOfClst = SECT_OF_TBLE_CLST;
	}
	else
	{
		bSecOfClst = SECT_OF_DATA_CLST;
	}
    
	while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE);
	//HAL_ADMWaitDMADone(bChan); //wait real done

	dDma2IndexFail = tChTaskQueue[bChan].wBufIdxFail;					//SG_DMA2_CH(bChan)->rBUF_INDEX_FAIL失效不能用!!

	if (DISCRETE_CLST == tFtlTask.bArgsArr[0])//离散读不能用SG_DMA2_CH(bChan)->rBUF_INDEX_LDPC做为搜索的结束点!!
	{		
		dDma2IndexLdpc = wFinalOfst*bSecOfClst;	//当前传输到的Idx

		if(SG_DMA2_CH(bChan)->rBUF_INDEX_LDPC == tFtlTask.pwBuffIdx[wFinalOfst]+bSecOfClst)//最后一次CheckHold没配置LogNum，需要额外判断 
		{
			dDma2IndexLdpc += bSecOfClst;
		}
	}
	else
	{
		dDma2IndexLdpc = SG_DMA2_CH(bChan)->rBUF_INDEX_LDPC;	//当前传输到的Idx
	}

	if(SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE >= JOINT_22)	//联动模式，才需要考虑掉头
	{
		if(SG_DMA2_CH(bChan)->rBUF_INDEX_LDPC > SG_ADM->rBUF_SIZE)//Overflow可能遗漏!!
		{
			U32 dDma2IdxFailTmp = SG_DMA2_CH(bChan)->rBUF_INDEX_LDPC-SG_ADM->rBUF_SIZE;

			dDma2IndexFail = Max(dDma2IdxFailTmp,dDma2IndexFail);
		}
	}
	
	//G3 G0锁方式，保证不会同时操作共享变量
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_GetOwnership(RTY_CNT_MUTIREG, G3CORE);
#else
	bG3VarLock[bChan] = 1;
	__nds32__dsb();
	while(bG0VarLock[bChan] == 1);
#endif	
#endif
//	SG_DMA2_CH(bChan)->rSTATUS.bits.CRC_FAIL_FLAG = 1;

		//LOG_PRINT("CRC_FAIL_FLAG=%d,failIdx=0x%x\n", SG_DMA2_CH(bChan)->rSTATUS.bits.CRC_FAIL_FLAG,dDma2IndexFail);
		//联动模式需要等DMA1停下来，才能知道该从哪里开始Retry;原因是dDma2IndexFail不可靠
			//CRC判断前的初始化，停止其他通道的LDPC解码，清除了buf_index后再打开
			//清水位，设置DMAINFO为Fail状态

//				U32 dOff = (SG_DMA1->rINDEX - dDma2IndexFail)%SG_ADM->rBUF_SIZE.bits.wVAL;	//求余之后的值
//				dDma2IndexFail += dOff;		//DMA1比DMA2的位置远，直接把DMA2 INDEX移到那边
			//恢复其他通道的执行

	//LOG_PRINT("Dma1Idx=0x%x,FAIL=0x%x,ldpc=0x%x\n", SG_DMA1->rINDEX, dDma2IndexFail, dDma2IndexLdpc);
	for(dBuffIdx = dDma2IndexFail; dBuffIdx < dDma2IndexLdpc; dBuffIdx += SECT_OF_DMA)//search next error dma
	{			
		dOfstInTask = dBuffIdx / bSecOfClst;//下一个簇
		wDma2Status  = pwDmaInfo[(dBuffIdx/SECT_OF_DMA) % (pFdlParam.dEccInfoBuffSize/2)];

        bMaskDmaInfo = Retry_JudgeDmaMask(dBuffIdx, bSecOfClst, dOfstInTask);
		ptClstAddr 	 = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfstInTask % tFtlTask.wContiAddrMax]); //地址可能掉头了

		if(ptClstAddr->m_bChan == 0xFF)	//虚拟簇不需要处理
		{
			tAddr.bChan = 0xFF;
		}
		else
		{
			Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
		}
		
		if((tAddr.bChan == bChan) && (bMaskDmaInfo != DMA_ALL_MASK)) //Fail DMA属于当前触发中断的通道
		{		  
            //FDL_Log("BufIdx=0x%x,Dma2Sts=0x%x,OfstInTask=%d",dBuffIdx, wDma2Status,dOfstInTask);
            if(tFtlTask.tTaskCtrl->bReadFailCtrl  == READ_ALL_ONE_NO_RETRY)		//非联动接口
            {
                if(DMAINFO_ALLONE(wDma2Status))
                {
                    //FDL_Log("All One No Retry");
					if(dOfstOfPre != dOfstInTask) //不是同一个簇
					{
						//FDL_Log("ExpInfo ofts #%x Saved",dOfstInTask);
						Retry_SaveTaskException(bChan,dOfstInTask, (1<<RD_ERR_ALL_ONE));
					}
					
					dOfstOfPre = dOfstInTask;
					bRetryMode = NO_RETRY;
                }
				else
				{
					bRetryMode = NORMAL_RETRY;//非全1进行Retry
				}
            }
			
            //发生CRC FAIL

				//联动模式需要等DMA1停下来，才能知道该从哪里开始Retry
					//清水位，设置DMAINFO为Fail状态
			
#ifndef RETRY_SVALUE
			if(DMAINFO_ABNORMAL(wDma2Status))
#else
			if(	(DMAINFO_ABNORMAL(wDma2Status)) ||
				((tFtlTask.tTaskCtrl->bReadFailCtrl==READ_VERIFY_TLC_PAGE) & (DMAINFO_SVALUE(wDma2Status)>RETRY_SVALUE)))
#endif
//			if(DMAINFO_ABNORMAL(wDma2Status)) //记录异常，包括：全1，全0，fail
			{	
		
				if(bRetryMode != NO_RETRY)
				{
					//FDL_Log("NF Row Lun Ce = 0x%x,%x,%x",tAddr.dRow,tAddr.bLun,tAddr.bChip);
					tRetryVar.wRetryLunBit[bChan] |= ((U16)0x01 << ptClstAddr->m_bLunOfChan);//该LogLun需要进行MCU Retry
					tRetryInfor.m_dFailBuffIndex[bChan][tAddr.bChip][tRetryInfor.m_bFailDmaCnt[bChan][tAddr.bChip]] = dBuffIdx; //保存出错的Index
					tRetryInfor.m_bFailDmaCnt[bChan][tAddr.bChip]++;	
					//LOG_PRINT("dBuffIdx=0x%x\n", dBuffIdx);

					if (0==tRetryVar.bLDPCFailFlag[bChan])		//只记录一次就可以
					{

#ifdef __CACHE_READ_ON__/*Retry前发一个3Fh(需要保证已经等了RealDone)*/
						if(CACHE_READ_EN && (tFtlTask.dTaskType == TYPE_NCQ_READ))
						{
							HAL_MCUCheckStatus(bChan,tAddr.bChip);							
							HAL_MCU_SendCmd(bChan,FL_CMD_CACHE_REDA_FINAL);/*3Fh的RB在Retry发命令阶段检测*/
						}
#endif

						//Retry之后需要恢复的信息
						tRetryVar.wLastClstOffs[bChan] = wFinalOfst; //最后一个簇偏移
                        Retry_FailRcdType(bChan, READ_RETRY_MODE);
                    }
				}	
			}
		}
	}
	
	tChTaskQueue[bChan].wBufIdxFail = dDma2IndexLdpc;
#ifdef RETRY_IN_G0
#ifdef __FDL_MCU_MUTUAL__
	HAL_MCU_ReleaseOwnership(RTY_CNT_MUTIREG, G3CORE);
#else
	bG3VarLock[bChan] = 0;
#endif
#endif
}
#ifdef __PRE_4K_READ_ENABLE__

U32 FDL_PreRead4KRetryGetInfo(U08 bChan)
{
	U08 i = 0;
	U32 dOfstInTask;
	U16 wDma2Status;
	U08 bSecOfClst;	
	U08 bRetryMode = NORMAL_RETRY; 
	T_ADDR_INFO tAddr;
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;	
	U16 *pwDmaInfo = (U16 *)(tFtlTask.dEccStsBuff);

	if(TYPE_TABLE == tFtlTask.bDataType)
	{
		bSecOfClst = SECT_OF_TBLE_CLST;
	}
	else
	{
		bSecOfClst = SECT_OF_DATA_CLST;
	}
	
	HAL_ADMWaitDMADone(bChan); //wait real done
	
	for(i = 0; i <  tFtlTask.wLen * bSecOfClst; i += SECT_OF_DMA)
	{
		dOfstInTask = i / bSecOfClst;//下一个簇
		wDma2Status  = pwDmaInfo[(i/SECT_OF_DMA) % (pFdlParam.dEccInfoBuffSize/2)];
		ptClstAddr 	 = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[dOfstInTask % tFtlTask.wContiAddrMax]); //地址可能掉头了
		
		if(ptClstAddr->m_bChan == 0xFF)	//虚拟簇不需要处理
		{
			tAddr.bChan = 0xFF;
		}
		else
		{
			Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
		}
		
		if(tAddr.bChan == bChan)
		{
			if(DMAINFO_ABNORMAL(wDma2Status)) //记录异常，包括：全1，全0，fail
			{	
				if(bRetryMode != NO_RETRY)
				{
					tRetryVar.wRetryLunBit[bChan] |= ((U16)0x01 << ptClstAddr->m_bLunOfChan);//该LogLun需要进行MCU Retry
					tRetryInfor.m_dFailBuffIndex[bChan][tAddr.bChip][tRetryInfor.m_bFailDmaCnt[bChan][tAddr.bChip]] = i; //保存出错的Index
					tRetryInfor.m_bFailDmaCnt[bChan][tAddr.bChip]++;

					SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 1; //后面retry流程直接进入HP模式
					__nds32__dsb();
					tRetryInfor.m_OpType = READ_RETRY_MODE;
				}	
			}
		}
	}	
	
	return (U32)(&tRetryInfor.m_bFailDmaCnt[0]);
}

#ifdef __FDL_HW_VIRTUL_CLST__
void Retry_PreRestartCQ(U08 bRestartCQEn)
{
	U08 bChan;
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;
	T_ADDR_INFO tAddr;
	U08 bFlashMode;
	//恢复CQ执行
	for (bChan=0; bChan<4; bChan++)
	{

        if(Retry_FailDmaCnt(bChan))

		{			

                Retry_CQPauseHandle(bChan);

				//恢复Cache里的数据
				if(tRetryVar.wRetryLunBit[bChan] > 0)
				{
				tRetryVar.wRetryLunBit[bChan] = 0;		//清零
					if(0 != bRestartCQEn)
					{

					ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[SG_FC(bChan)->rLOGIC_NUM % tFtlTask.wContiAddrMax]);
									Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
									bFlashMode = tAddr.dRow >> 24;
									//LOG_PRINT("bFlashMode = %d",bFlashMode);
									Fdl_MCUReadCmd(bChan, tAddr.bChip, tAddr.dRow, tAddr.wCol, bFlashMode); 
								}
							}
						}
					}
				}


#else
void Retry_PreRestartCQ(U08 bRestartCQEn)
{
	U08 bChan;
	T_FLASH_PHY_ADDR *ptClstAddr = NULL;
	T_ADDR_INFO tAddr;
	U08 bFlashMode;
	//恢复CQ执行
	for (bChan=0; bChan<4; bChan++)
	{

#if 0
		if ((tRetryInfor.m_bFailDmaCnt[bChan][0] == 0) 
			&& (tRetryInfor.m_bFailDmaCnt[bChan][1] == 0) 
			&& (tRetryInfor.m_bFailDmaCnt[bChan][2] == 0) 
			&& (tRetryInfor.m_bFailDmaCnt[bChan][3] == 0))
#else
        if(Retry_FailDmaCnt(bChan))
#endif
		{			
#if 0
				SG_DMA2_CH(bChan)->rSTATUS.dAll = 0; __nds32__dsb(); //清除flag
				SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_EN = 0; __nds32__dsb();
				SG_FC(bChan)->rCHK_INT_ST.bits.CQ_HOLD_FLAG = 0; __nds32__dsb(); //clear int infor
				SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_INT_EN = 1; __nds32__dsb(); //恢复hold中断 			
				SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0;		//retry结束退出HP模式

				U08MemSet(tRetryCtrl[bChan], 0x00, sizeof(tRetryCtrl[bChan]));
				U08MemSet(bStateMachine[bChan], 0x00, sizeof(bStateMachine[bChan]));
#else
                Retry_CQPauseHandle(bChan);
#endif
				//恢复Cache里的数据
				if(tRetryVar.wRetryLunBit[bChan] > 0)
				{
					U08 bRetryLun;
					U16 wLunFailOfst;
					if(0 != bRestartCQEn)
					{
						for(bRetryLun=0; bRetryLun < MAX_LUN_OF_CHAN; bRetryLun++)
						{
							if(tRetryVar.wRetryLunBit[bChan]&(1<<bRetryLun))
							{
								wLunFailOfst = Fdl_SearchOfstByAddr(bChan,bRetryLun,0,0,BACKWARD_SEARCH_LUN_ADDR_OFST); 
								if(wLunFailOfst != FAIL_SEARCH_ADDR_OFST)
								{
									ptClstAddr = &(((T_FLASH_PHY_ADDR *)tFtlTask.dSrcArgsAddr)[wLunFailOfst % tFtlTask.wContiAddrMax]);
									Fdl_ConvertAddrInfo(&tAddr,(U32)ptClstAddr,!TYPE_ERASE_BLOCKS);
									bFlashMode = tAddr.dRow >> 24;
									//LOG_PRINT("bFlashMode = %d",bFlashMode);
									Fdl_MCUReadCmd(bChan, tAddr.bChip, tAddr.dRow, tAddr.wCol, bFlashMode); 
								}
							}
						}
					}
					tRetryVar.wRetryLunBit[bChan] = 0;		//清零
				}
		}
	}
}
#endif

#endif

