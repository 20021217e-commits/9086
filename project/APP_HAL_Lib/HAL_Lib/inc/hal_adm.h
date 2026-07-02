// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  hal.h
//  文件标识：  
//  内容摘要：  hal接口定义
//  其它说明：  
//  当前版本：  v0.10
//  项   目：  SG9081
//  作   者：  gary
// *****************************************************************************
#ifndef __HAL_ADM_H__
#define __HAL_ADM_H__

#ifdef __cplusplus
extern "C" {
#endif

    #define  R_ADM_ATTR_BIT						(24)
    #define  R_ADM_REG_BIT						(16)
    #define  R_ADM_ATTR_CHECK_DMA1_DONE			(((U32)0x00)<<R_ADM_ATTR_BIT)
    #define  R_ADM_ATTR_INT						(((U32)0x01)<<R_ADM_ATTR_BIT)
    #define  R_ADM_ATTR_HOLD						(((U32)0x02)<<R_ADM_ATTR_BIT)
    #define  R_ADM_ATTR_DELAY						(((U32)0x04)<<R_ADM_ATTR_BIT)
    #define  R_ADM_ATTR_CFG_REG_2B					(((U32)0x05)<<R_ADM_ATTR_BIT)
    #define  R_ADM_ATTR_CFG_REG_1B					(((U32)0x06)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA20_BUF_DONE		(((U32)0x07)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA20_REAL_DONE		(((U32)0x08)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA21_BUF_DONE		(((U32)0x09)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA21_REAL_DONE		(((U32)0x0A)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA22_BUF_DONE		(((U32)0x0B)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA22_REAL_DONE		(((U32)0x0C)<<R_ADM_ATTR_BIT)
    #define  R_ADM_ATTR_CHECK_DMA23_BUF_DONE		(((U32)0x0D)<<R_ADM_ATTR_BIT)
	#define  R_ADM_ATTR_CHECK_DMA23_REAL_DONE		(((U32)0x0E)<<R_ADM_ATTR_BIT)

//寄存器位置定义成宏，效率和代码量
#define DMA2_OFFS_CTRL			((U32)0x00)  //dOfst = offsetof(SG_DMA2_TypeDef, rCTRL);
#define DMA2_OFFS_LEN_CFG		((U32)0x04) //dOfst = (U8)offsetof(SG_DMA2_TypeDef,rLEN_CFG);
#define DMA2_OFFS_UDATA_BSADDR	((U32)0x08) 
#define DMA2_OFFS_INDEX			((U32)0x18) //dOfst = offsetof(SG_DMA2_TypeDef, rINDEX_CFG);
#define DMA2_OFFS_TOTAL_SECT	((U32)0x1C) //dOfst = offsetof(SG_DMA2_TypeDef, rTOTAL_SECT_CFG);
#define DMA2_OFFS_FLASH_INDEX	((U32)0x20) //dOfst = offsetof(SG_DMA2_TypeDef, rFLASH_INDEX);
#define DMA2_OFFS_MASK			((U32)0x24) //dOfst = (U8)offsetof(SG_DMA2_TypeDef, rMASK);
#define DMA2_OFFS_SSRND_SEED	((U32)0x30) //dOfst = (U8)offsetof(SG_DMA2_TypeDef,rSSRND_SEED);
#define DMA2_OFFS_RAID_CTRL		((U32)0x34) //dOfst= offsetof(SG_DMA2_TypeDef,rRAID_CTRL);
#define DMA2_OFFS_RAID_BUF_ADDR	((U32)0x38) //dOfst= offsetof(SG_DMA2_TypeDef,rRAID_BUF_ADDR);
#define DMA2_OFFS_SEED_INDEX	((U32)0x3C) //rSEED_INDEX
#define DMA2_OFFS_BSADDR_HANDLE  ((U32)0x5C)
#define DMA2_OFFS_BSADDR		((U32)0x4C) //dOfst = offsetof(SG_DMA2_TypeDef, rBSADDR);
#define DMA2_OFFS_DMAINF_BSADDR	((U32)0x54) //dOfst = offsetof(SG_DMA2_TypeDef, rDMAINF_BSADDR);

#define FC_OFFS_CHKMIN_TIME     ((U32)0xB8)
#define FC_OFFS_LOGIC_NUM0		((U32)0xE4) //dOfset = offsetof(SG_FC_TypeDef,rLOGIC_NUM)
#define FC_OFFS_LOGIC_NUM1		((U32)0xF0) //dOfset = offsetof(SG_FC_TypeDef,Reserved3)
#define FC_OFFS_FIO_SPOP_CTRL	((U32)0x1C) /*dOfset = offsetof(SG_FC_TypeDef,rFIO_SPOP_CTRL)*/
#define FC_OFFS_NF_CFG			((U32)0x00) /*dOfset = offsetof(SG_FC_TypeDef,rNF_CFG)*/
#define FC_OFFS_NF_BP_CTRL      ((U32)0x08)
#define FC_OFFS_SPC_PATTEN      ((U32)0x0C)
#define FC_OFFS_SPC_UDPATTEN    ((U32)0x10)


/*最高位用来选择PMCP1或者PMCP0*/
#define PMCP0_OFFS_BSADDR0		((U32)0x00) /*SG_PMCP(0)->rBASE_ADDR0*/
#define PMCP1_OFFS_BSADDR0		((U32)0x80) /*SG_PMCP(0)->rBASE_ADDR0*/

#define PMCP0_OFFS_MAX_VAL		((U32)0x24) /*SG_PMCP(0)->rMAX_VAL*/
#define PMCP1_OFFS_MAX_VAL		((U32)0xA4) /*SG_PMCP(0)->rMAX_VAL*/

#define PMCP0_OFFS_MIN_VAL		((U32)0x20) /*SG_PMCP(0)->rMIN_VAL*/
#define PMCP1_OFFS_MIN_VAL		((U32)0xA0) /*SG_PMCP(0)->rMIN_VAL*/

#define PMCP0_OFFS_BIT_MASK		((U32)0x1C) /*SG_PMCP(0)->rBIT_MASK*/
#define PMCP1_OFFS_BIT_MASK		((U32)0x9C) /*SG_PMCP(0)->rBIT_MASK*/

#define PMCP0_OFFS_OPE_MODE		((U32)0x18) /*SG_PMCP(0)->rOPE_MODE*/
#define PMCP1_OFFS_OPE_MODE		((U32)0x98) /*SG_PMCP(0)->rOPE_MODE*/

#define PMCP0_OFFS_RESULT_ADDR	((U32)0x08) /*SG_PMCP(0)->rRESULT_ADDR*/
#define PMCP1_OFFS_RESULT_ADDR	((U32)0x88) /*SG_PMCP(0)->rRESULT_ADDR*/

#define PMCP0_OFFS_OPE_CNT		((U32)0x10) /*SG_PMCP(0)->rOPE_CNT*/
#define PMCP1_OFFS_OPE_CNT		((U32)0x90) /*SG_PMCP(0)->rOPE_CNT*/

#define PMCP0_OFFS_OPE_START	((U32)0x34) /*SG_PMCP(0)->rOP_START*/
#define PMCP1_OFFS_OPE_START	((U32)0xB4) /*SG_PMCP(0)->rOP_START*/

#define PMCP0_OFFS_OPE_CSR		((U32)0x14) /*SG_PMCP(0)->rOPE_CSR*/
#define PMCP1_OFFS_OPE_CSR		((U32)0x94) /*SG_PMCP(0)->rOPE_CSR*/




inline void HAL_ADMSetBaseAddr(U8 bChan, U32 dBuffAddr);
inline U32 HAL_ADMGetBaseAddr(U8 bChan);
inline void HAL_ADMSetBufferSize(U16 wSect);
inline void HAL_ADMSetDMALen(U8 bChan, U16 wLen);
inline U16 HAL_ADMGetDMALen(U8 bChan);
inline void HAL_ADMSetUdBaseAddr(U8 bChan, U32 dBuffAddr);
inline U32 HAL_ADMGetUdBaseAddr(U8 bChan);
inline void HAL_ADMSetUdLen(U8 bChan, U8 bLen);
inline void HAL_ADMSetUdEnable(U8 bChan, U8 bRndNewState);
inline void HAL_ADMSetRaidEnable(U8 bChan, U8 bRndNewState);
inline U8 HAL_ADMGetUdLen(U8 bChan);
inline U8 HAL_ADMGetJointMode(U8 bChan);
inline void HAL_ADMSetJointMode(U8 bChan,U8 bJointMode);
inline U32 HAL_ADMGetDMA1Index(void);
U32 HAL_ADMGetDMA2Index(U08 bChanPhy);
inline void HAL_ADMClearDMA1Index(void);
inline void HAL_ADMClearDMA2Index(U08 bChanPhy);
inline void HAL_MCGenerateInt();
inline void HAL_MCWaitDMA1Done();
inline U08 HAL_MCGetStatus();
void HAL_ADMWaitDMADone(U8 bChan);
volatile U8 bCheckDMA2RealDone(U8 bChan);
volatile U8 bCheckDMA2FCDone(U8 bChan);
volatile U8 bCheckDMA2BufDone(U8 bChan);
inline void HAL_ADMSetRndEnable(U8 bChan, U8 bFunctionNewState, U8 bRndNewState);
inline void HAL_ADMSetMask(U8 bChan,U32 dMask0, U32 dMask1);
inline void HAL_ADMSetDma2EnabledState(U8 bChan,U16 wNewState);
inline U16 HAL_ADMGetDma2EnabledState(U8 bChan);
inline void HAL_ADMSetDma2StopEnableState(U8 bChan, U8 bNewState);
inline U8 HAL_ADMGetDma2StopEnableState(U8 bChan);
void HAL_HostDataTransfer(U32 dBuffAddr,U16 wSec, U08 bDir);
void HAL_ADMComSelEccMode(U8 bChan,U8 bMode);
void HAL_RTY_BFCSetBuffStatus(U16 wIndex,U16 wNewBitPosition);
void HAL_RTY_BFCSetBuffStatusBit(U16 wIndex);
#ifdef __cplusplus
}
#endif
#endif
