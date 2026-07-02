
#ifndef     __FDL_SOFT_DECODE_H__    
#define     __FDL_SOFT_DECODE_H__
#include "fdl_task_op.h"
#include "fdl_read_retry.h"
#include "flash_driver_config.h"


#ifdef __cplusplus
extern "C" {
#endif

#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)|| (NAND_FLASH == MIC_B27B) || (NAND_FLASH == MIC_M84C) || (NAND_FLASH == INTEL_B27B))
#define __SOFT_OR_EX_LLR_3BIT__//3CW亦或解码
//#define __SOFT_INNER_LLR_3BIT__
#elif (NAND_FLASH == MIC_B0KB)
#define __SOFT_EX_LLR_3BIT__
#define __SOFT_EX_LLR_5BIT__

#elif (NAND_FLASH == TOS_BICS3)
//#define __SOFT_EX_LLR_5BIT__
#define __SOFT_OR_EX_LLR_3BIT__//3CW亦或解码
#ifdef __SOFT_OR_EX_LLR_3BIT__
#define MEM_SOFT_DATA 	0x470000	//3CW异或软解码存放数据基地址
#endif
#elif (NAND_FLASH == HYNIX_3DV4)
#define __SOFT_EX_LLR_3BIT__
#define __SOFT_EX_LLR_5BIT__

#elif (NAND_FLASH == HYNIX_8A1M)
#define __SOFT_EX_LLR_5BIT__
#define __SOFT_EX_LLR_3BIT__

#elif (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == AFGM) ||  (NAND_FLASH == GCGF)
#define __SOFT_OR_EX_LLR_3BIT__//3CW亦或解码
#elif (NAND_FLASH == MIC_M16)
#define __SOFT_INNER_LLR_3BIT__
//#define __SOFT_EX_LLR_3BIT__
//#define __SOFT_EX_LLR_5BIT__

#elif (NAND_FLASH == MIC_N18)
#define __SOFT_INNER_LLR_3BIT__
//#define __SOFT_EX_LLR_3BIT__
//#define __SOFT_EX_LLR_5BIT__

#elif(NAND_FLASH == MIC_N28)
//#define __SOFT_INNER_LLR_3BIT__
#define __SOFT_EX_LLR_3BIT__
#define __SOFT_EX_LLR_5BIT__

#elif ((NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M))
#define __SOFT_OR_EX_LLR_3BIT__//3CW亦或解码
#define __SOFT_EX_LLR_3BIT__
#define __SOFT_EX_LLR_5BIT__
#else //added by flash analyse group
#define __SOFT_INNER_LLR_3BIT__
#endif

#ifdef __SOFT_INNER_LLR_3BIT__
extern U8_C HAL_b3BitLUT[32];/*内部软解*/
#endif

#ifdef __SOFT_EX_LLR_5BIT__
extern U8_C HAL_b5BitLUT[32];/*外部5Bit软解*/
#endif

#if defined(__SOFT_EX_LLR_3BIT__)||defined(__SOFT_OR_EX_LLR_3BIT__)
extern U8_C HAL_b3BitLUT[32];/*外部3Bit软解*/
#define MEM_SOFT_DATA 	0x490000	//3CW异或软解码存放数据基地址
#endif

enum WLPageMode
{
	SLC_PAGE,	//对应的WL类型
	MLC_PAGE,
	TLC_PAGE,
};

//U8 FDL_LDPC_SoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bFlashMode);
void Fdl_MCU_SetFeature_ReadOffs(U8 bChan, U8 bChip, U8 bAddr, U8 *pData);

#ifdef __cplusplus
}
#endif

#endif






