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
#ifndef __HAL_CQ_H__
#define __HAL_CQ_H__

#ifdef __cplusplus
extern "C" {
#endif

	#define  CMD_DELAY_CYCLE                (2)
	#define  MCU_SEND_CMD_HEAD              ((1<<31)|(CMD_DELAY_CYCLE<<24))

    #define  R_CQ_ATTR_BIT                  (24)
    #define  R_CQ_REG_BIT                   (16)
    #define  R_CQ_ATTR_SET_CE               (((U32)0x00)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CHK_RB               (((U32)0x01)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CHK_IOSTA_0          (((U32)0x02)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CHK_IOSTA_1          (((U32)0x03)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_STOP                 (((U32)0x05)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_INT                  (((U32)0x06)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_DELAY                (((U32)0x07)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CMD                  ((((U32)0x08)<<R_CQ_ATTR_BIT)|(CMD_DELAY_CYCLE<<16)|(1<<23))
    #define  R_CQ_ATTR_ADDR                 (((U32)0x0A)<<R_CQ_ATTR_BIT)

    #define  R_CQ_ATTR_ST_RD                (((U32)0x0F)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CHECK_DMA2_DONE      (((U32)0x12)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CQ_CHECK_HOLD	    (((U32)0x13)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_NO_STOP              (((U32)0x14)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_ADDR2                (((U32)0x15)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_ADDR3                (((U32)0x16)<<R_CQ_ATTR_BIT)

    #define  R_CQ_ATTR_TX_WR_PULSE          (((U32)0x18)<<R_CQ_ATTR_BIT)
//    #define  R_CQ_ATTR_REG_MOVE_OP          (((U32)0x1B)<<R_CQ_ATTR_BIT)
//    #define  R_CQ_ATTR_REG_AND_OP           (((U32)0x1C)<<R_CQ_ATTR_BIT)
//    #define  R_CQ_ATTR_REG_OR_OP            (((U32)0x1D)<<R_CQ_ATTR_BIT)
//    #define  R_CQ_ATTR_CFG_FC_REG           (((U32)0x1E)<<R_CQ_ATTR_BIT)

    #define  R_CQ_ATTR_CFG_DW_FC_REG        (((U32)0x20)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CFG_SW_FC_REG        (((U32)0x21)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CFG_DW_DMA2_REG      (((U32)0x24)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CFG_SW_DMA2_REG      (((U32)0x25)<<R_CQ_ATTR_BIT)

    #define  R_CQ_ATTR_CMD2                 (((U32)0x26)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CMD3                 (((U32)0x27)<<R_CQ_ATTR_BIT)
	#define  R_CQ_CHECK_FC_DONE			 	(((U32)0x28)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CFG_LOGIC_NUM_REG    (((U32)0x29)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CFG_DW_PMCP_REG      (((U32)0x2A)<<R_CQ_ATTR_BIT)
    #define  R_CQ_ATTR_CHK_PMCP_DONE        (((U32)0x2B)<<R_CQ_ATTR_BIT)
	#define  R_CQ_CHECK_REAL_DONE			(((U32)0x2C)<<R_CQ_ATTR_BIT)

	U16 HAL_GetSamsungSeed(U08 bOffs);
	inline void HAL_CQSend5Addr(U8 bChan, U16 wColAddr, U32 dRowAddr);
#ifdef __cplusplus
}
#endif
#endif
