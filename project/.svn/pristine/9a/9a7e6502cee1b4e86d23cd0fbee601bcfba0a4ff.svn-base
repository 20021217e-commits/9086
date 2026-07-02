#include "hal.h"
#include "hal_test.h"
#include "hal_cq.h"
#include "hal_define.h"
#include "SumsungSeed.h"
#include "hal_adm.h"
#include "fdl_debug.h"

inline U16 HAL_CQGetIdleSpace(U8 bChan)
{
    return SG_FC(bChan)->rCQ_FIFO_ST_CTRL.bits.CQ_FIFO_IDLE_SPACE;
}

U8 HAL_CQWaitIdleSpace(U8 bChan, U16 wSpaceCount)
{
    U32 dCnt = 0xFFFFFFFF;
    while(HAL_CQGetIdleSpace(bChan) < wSpaceCount)
    {
        dCnt--;
        if (0 == dCnt)
        {
            //return FALSE;
        	while(1);
        }
    }
    return TRUE;
}

U8 HAL_CQWaitDone(U8 bChan)
{
    U08 bFlag = HAL_CQWaitIdleSpace(bChan,CQ_MAX_IDLE_SPACE);
	return bFlag;
}

inline void HAL_CQReset(U8 bChan)
{

    SG_FC(bChan)->rCQ_CTRL.bits.CQ_CLEAR = FUNC_ENABLE;
    SG_FC(bChan)->rCHK_INT_ST.dAll = FUNC_DISABLE;
}

inline void HAL_CQChipSelect(U8 bChan, U8 bChip)
{	
#if((NAND_FLASH == SAMSUNG_3DV5_QLC)||(NAND_FLASH==SAMSUNG_3DV6_TLC))
	if (bChip != INVALID_U8)
	{
		HAL_MCUSendCER_CE(bChan,bChip&0x0F,(bChip>>4)&0x0F);//CE Reduction
	}
#else

    if (INVALID_U8 == bChip)
    {
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0x00; 
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;
    }
    else if (4 > bChan)
    {
		//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0x00;
    	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x4;
		//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
    	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x4;
		
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | (1 << bChip);   
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;
	 }
    else if (4 == bChan) //CE Encode case: CE0~CE15 to bit[3:0]=CH2CE3, CH2CE1, CH0CE2, CH0CE0
    {
    	U8 CH0Setting = 0xFF&(~((((bChip>>1)&1)<<2)|(((bChip>>0)&1)<<0)));// bit[1:0] CH0CE2, CH0CE0
    	U8 CH2Setting = 0xFF&(~((((bChip>>3)&1)<<3)|(((bChip>>2)&1)<<1)));// bit[3:2] CH2CE3, CH2CE1
		SG_FC(0)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | CH0Setting;
		SG_FC(2)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | CH2Setting;
		SG_FC(0)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;
		CQ_WaitDone(0);
		CQ_WaitDone(2);
	 }
#endif
}

inline void HAL_CQSendWRPulse(U8 bChan, U8 bData)
{
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_TX_WR_PULSE | (bData);
}

inline void HAL_CQSendAddr(U8 bChan, U8 bAddr)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR | bAddr;
}

inline void HAL_CQSendColAddr(U8 bChan, U16 wColAddr)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR2 | wColAddr;
}

inline void HAL_CQSendRowAddr(U8 bChan, U32 dRowAddr)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR3 | (dRowAddr & 0xFFFFFF);
}

inline void HAL_CQSendRowAddr_4Byte(U8 bChan, U32 dRowAddr)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR2 | (dRowAddr & 0xFFFF);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR2 | ((dRowAddr>>16) & 0xFFFF);
}

inline void HAL_CQSend5Addr(U8 bChan, U16 wColAddr, U32 dRowAddr)
{
    HAL_CQSendColAddr(bChan,wColAddr);
    HAL_CQSendRowAddr(bChan,dRowAddr);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;				// delay
}

inline void HAL_CQSend6Addr(U8 bChan, U16 wColAddr, U32 dRowAddr)
{
    HAL_CQSendColAddr(bChan,wColAddr);
    HAL_CQSendRowAddr_4Byte(bChan, dRowAddr);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;				// delay
}

inline void HAL_CQSendRndAddr(U8 bChan, U16 wColAddr,U32 dRowAddr)
{
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,wColAddr,dRowAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);

#ifdef __CMD_SAME_AS_SMI__
    HAL_CQSend5Addr(bChan,wColAddr,dRowAddr);
#else
    HAL_CQSendColAddr(bChan,wColAddr);
#endif

    HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;				// delay
}

inline void HAL_CQSend5RndAddr(U8 bChan, U16 wColAddr,U32 dRowAddr)
{
	HAL_CQSendFlashCmd(bChan,FL_CMD_READ1);
	HAL_CQSend5Addr(bChan,0,dRowAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT1);
    HAL_CQSendColAddr(bChan,wColAddr);
    HAL_CQSendFlashCmd(bChan,FL_CMD_RANDOM_DATA_OUTPUT2);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x20;				// delay
}

inline void HAL_CQSendFlashCmd(U8 bChan, U8 bCmd)
{	
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | bCmd;
}

inline void HAL_CQSendInstruct(U8 bChan, U32 dInstruct)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = dInstruct;
}

inline void HAL_CQCfgLogicNum(U8 bChan, U32 dLogNum)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG |((FC_OFFS_LOGIC_NUM0 + 0)<<R_CQ_REG_BIT) | ((dLogNum >> 0));
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG |((FC_OFFS_LOGIC_NUM0 + 2)<<R_CQ_REG_BIT) | ((dLogNum >> 16));
}


void HAL_CQCfgLogicNumExt(U8 bChan, U32 dLogNum)
{

	/*LogNum 扩展,用FC模块隐藏的保留寄存器*/
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG |((FC_OFFS_LOGIC_NUM1 + 0)<<R_CQ_REG_BIT) | ((dLogNum >> 0));
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_FC_REG |((FC_OFFS_LOGIC_NUM1 + 2)<<R_CQ_REG_BIT) | ((dLogNum >> 16));
}

U32 HAL_CQGetLogicNumExt(U8 bChan)
{
	/*LogNum 扩展,用FC模块隐藏的保留寄存器*/
	U32 dLogNum = 0;
	dLogNum = (*(U32 *)SG_FC(bChan)->Reserved3);
	return (dLogNum);
}


inline void HAL_CQGenerateInt(U8 bChan, U8 bIntNum, U8 bStopCQ)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_INT | ((bStopCQ & 0x01) << 7) | (bIntNum & 0x7F);
}

inline void HAL_CQCheckHold(U8 bDma2Dir, U8 bJoint,U8 bChan, U32 dBuffIdx, U16 wTotalSecCfg,U8 bHoldEn)
{
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_FC_DONE;
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0;
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY |0x10;
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_CTRL << R_CQ_REG_BIT) | (bJoint << 4) | (bDma2Dir << 2); //change dir
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_INDEX << R_CQ_REG_BIT) | (dBuffIdx & INVALID_U16);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_INDEX+2) << R_CQ_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_TOTAL_SECT << R_CQ_REG_BIT) | wTotalSecCfg;

	if(bHoldEn == 1)//Hold En
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CQ_CHECK_HOLD;
	}
}


inline void HAL_CQCheckHoldOnly(U8 bChan,U8 bHoldEn)
{
    if(bHoldEn == 1)
    {
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CQ_CHECK_HOLD;
    }
}

inline void HAL_CQCheckReadEnd(U8 bChan,U8 bHoldEn)
{
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
    //SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0;
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY |0x08;
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
    
    if(bHoldEn == 1)
    {
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CQ_CHECK_HOLD;
        //SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
#ifndef __FDL_CQFIFO_TRIG_IRQ__
		HAL_CQGenerateInt(bChan,0,FUNC_DISABLE);
#endif
    }
}

void HAL_CQSetChkStCtrl(U8 bChan,U8 bRegLastVal,U8 bMode)//设置Chk_cq_ctrl中Chk st ctrl是否为CQ STOP 
{
    U32 dOfset = offsetof(SG_FC_TypeDef,rCHK_CQ_CTRL) & INVALID_U8;
    if(bMode == 0)//st timeout 不 stop CQ
    {
        bRegLastVal &= ~(1<<0);
    }
    else//st timeout stop CQ,进入中断
    {
        bRegLastVal |= (1<<0);
    }
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_FC_REG |((dOfset + 0)<<R_CQ_REG_BIT) | (bRegLastVal & INVALID_U8);
}

///// pass when((bFirstCheckIOBit==1)&&(bSecondCheckIOBit==0)
void HAL_CQCheckIOStatus(U8 bChan, U8 bLunOfChip,IO_Status bStatusType, U8 bFirstCheckIOBit, U8 bSecondCheckIOBit)
{	                                                                                                           
                                                                                                                                                                                                                                                                                                               
    //SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x50;            // 600ns
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_READ_STATUS;                                     
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x80;            // 0.6us                               
                                                                                                             
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (bSecondCheckIOBit << 8) | bFirstCheckIOBit;
        //SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_0 | bFirstCheckIOBit;                           
        //SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_0 | (bSecondCheckIOBit << 8) | bFirstCheckIOBit;
                                                                                                                                                                                                                                                                                                                               	                                                                                                           
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x4;			// delay                                       
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | 0x00;                                                  
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x40;		// delay 400ns                                 
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;                                                          
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x8;			// delay                                       
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = 1<<bChip;                                                                 
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x10;		// delay   
}                                                                                                            


#define RD_STS_70_CMD (0)
#define RD_STS_71_CMD (1)
#define RD_STS_78_CMD (2)
#define RD_STS_FN_CMD (3)
#define CHECK_RB_PIN (4)

void HAL_CQCheckStsCfg(U8 bChan, U8 bRdStsMode, U32 dRowAddr,U8 bChip,U8 bLun,U8 bCheckBit, U8 RowMode)
{    
	U32 overtime = 500000;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_SET_CE | (1 << bChip);//设置片选信号

	switch(bRdStsMode)
	{
	case RD_STS_70_CMD:
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD| FL_CMD_READ_STATUS;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x80;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (bCheckBit);
		break;

	case RD_STS_71_CMD:
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD| FL_CMD_READ_MULTI_PLN_STATUS;
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ADDR | bLun;//发送地址		
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (bCheckBit);
		break;

	case RD_STS_78_CMD:
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD  | FL_CMD_READ_ENHANCE_STATUS;
		
		if(6 == RowMode)	
		{HAL_CQSendRowAddr_4Byte(bChan, dRowAddr);		}
		else if(5 == RowMode)
		{HAL_CQSendRowAddr(bChan, dRowAddr);		}

		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_DELAY | 0x80;
		
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (bCheckBit);
		break;
		
	case CHECK_RB_PIN:
		
		while(overtime--)
		{
			if(1==(SG_PADCTRL->rPORT_DATA[15].bits.dVAL >> 7))
			{
				break;
			}
		}
		break;
	case RD_STS_FN_CMD:
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD| FL_CMD_READ_LUN_STATUS(bLun);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (bCheckBit);
		break;

	default:
		break;
	}

	if(bRdStsMode!=CHECK_RB_PIN)
	{
    	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHK_IOSTA_1 | (bCheckBit);
	}
	//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD ;
//	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CMD | FL_CMD_EXIT_CHK_IOSTA ;
}


inline void HAL_CQCheckFcDone(U8 bChan)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_FC_DONE;
}

inline void HAL_CQCheckDmaDone(U8 bChan)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
}

inline void HAL_CQCheckRealDone(U8 bChan)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_REAL_DONE;
}

void HAL_CQStartDMA2Read(	U08 bChan, 
							U32 dBuffAddr, 
							U32 dBuffIdx, 
							U08 bFlashIdx,  
							Joint_Mode joint, 
							U32 dDmaInfoAddr, 
							U32 dUDBuffAddr, 
							U32 dRowAddr, 
							U08 *pMaskBuff, 
							U08 bMaskLen ,
							U16 wSeedIdx)
{
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_CTRL << R_CQ_REG_BIT) | (joint << 4) | (DMA2_RD_TO_BUFF << 2); //change dir

	//check FC done

	//flash index
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (DMA2_OFFS_FLASH_INDEX << R_CQ_REG_BIT) | bFlashIdx;// | (1<<9);//cfg flash idx and start Fc

	//control
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_CTRL << R_CQ_REG_BIT) | (joint << 4) | (DMA2_RD_TO_BUFF << 2); //start Fc
	
	//check Buf_done
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
	HAL_CQSetRndSeed(bChan,dRowAddr,wSeedIdx);//等Buff Done之后才能配种子

	//set mask
	if (NULL != pMaskBuff)
	{
		HAL_CQSetMask(bChan, pMaskBuff, bMaskLen);
	}

	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_UDATA_BSADDR<<R_CQ_REG_BIT) | (dUDBuffAddr & INVALID_U16);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_UDATA_BSADDR+2)<<R_CQ_REG_BIT) | ((dUDBuffAddr >> 16) & INVALID_U16);

	//dBuffAddr
	if(joint == JOINT_NONE)
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_BSADDR_HANDLE << R_CQ_REG_BIT) | (dBuffAddr & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_BSADDR_HANDLE+2) << R_CQ_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);
	}
	else
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_BSADDR << R_CQ_REG_BIT) | (dBuffAddr & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_BSADDR+2) << R_CQ_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);
	}

	//ldpc mode
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_DMAINF_BSADDR<<16) | (dDmaInfoAddr & 0xffff);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_DMAINF_BSADDR+2)<<16) | ((dDmaInfoAddr>>16) & 0xffff);

	//BuffIndex
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_INDEX << R_CQ_REG_BIT) | (dBuffIdx & INVALID_U16);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_INDEX+2) << R_CQ_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);

	//control
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_CTRL << R_CQ_REG_BIT) | (joint << 4) | (DMA2_RD_TO_BUFF << 2); //start Buf	
	
	U32 dOfst = offsetof(SG_DMA2_TypeDef, rSTART);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG |(dOfst << R_CQ_REG_BIT) | 0x3;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_FC_DONE;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_ST_RD;
}

void HAL_CQStartDMA2Write(U8 bChan, U32 dBuffAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint, U32 dDmaInfoAddr)
{
	U32 dOfst = offsetof(SG_DMA2_TypeDef, rCTRL);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (joint << 4) | (dir << 2); //start Buf

	//dBuffAddr
	if(joint == JOINT_NONE)
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_BSADDR_HANDLE << R_CQ_REG_BIT) | (dBuffAddr & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_BSADDR_HANDLE+2) << R_CQ_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);
	}
	else
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_BSADDR << R_CQ_REG_BIT) | (dBuffAddr & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_BSADDR+2) << R_CQ_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);
	}
            
    //flash index
    dOfst = offsetof(SG_DMA2_TypeDef, rFLASH_INDEX);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bFlashIdx;

    //BuffIndex
    dOfst = offsetof(SG_DMA2_TypeDef, rINDEX_CFG);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (dBuffIdx & INVALID_U16);
    dOfst += 2;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);
    
    //total sector
    dOfst = offsetof(SG_DMA2_TypeDef, rTOTAL_SECT_CFG);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bTotalSect;
    
    //control
    
	dOfst = offsetof(SG_DMA2_TypeDef, rSTART);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG |(dOfst << R_CQ_REG_BIT) | 0x3;
    //check done
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_FC_DONE;
}

void HAL_CQStartDMA2(U8 bChan, U32 dBuffAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint, U32 dDmaInfoAddr)
{
	U32 dOfst = offsetof(SG_DMA2_TypeDef, rCTRL);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (joint << 4) | (dir << 2); //start Buf
	if(dir == 1)	//read
	{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
		HAL_CQSetDMAInfoAddr(bChan, dDmaInfoAddr);
	}

	//dBuffAddr
	if(joint == JOINT_NONE)
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_BSADDR_HANDLE << R_CQ_REG_BIT) | (dBuffAddr & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_BSADDR_HANDLE+2) << R_CQ_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);
	}
	else
	{
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_BSADDR << R_CQ_REG_BIT) | (dBuffAddr & INVALID_U16);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_BSADDR+2) << R_CQ_REG_BIT) | ((dBuffAddr >> 16) & INVALID_U16);
	}

		//check FC done

		//flash index
		dOfst = offsetof(SG_DMA2_TypeDef, rFLASH_INDEX);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bFlashIdx;
		//total sector

		
		//BuffIndex
		dOfst = offsetof(SG_DMA2_TypeDef, rINDEX_CFG);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (dBuffIdx & INVALID_U16);
		dOfst += 2;
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | ((dBuffIdx >> 16) & INVALID_U16);
		//flash index

		//total sector
		dOfst = offsetof(SG_DMA2_TypeDef, rTOTAL_SECT_CFG);
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bTotalSect;
		
		//BuffIndex
	
	dOfst = offsetof(SG_DMA2_TypeDef, rSTART);
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG |(dOfst << R_CQ_REG_BIT) | 0x3;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_CHECK_FC_DONE;	
}

void HAL_CQSetUd(U8 bChan, U32 dUdBuff, U8 bAttachSize, U8 bUdLen,U8 bDir)
{	
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CHECK_DMA2_DONE;
    HAL_CQSetUdBaseAddr(bChan,dUdBuff,bDir);//bDir 1:read    0:write    
    HAL_CQSetUdLen(bChan,bUdLen);
    HAL_CQSetUdAttachSize(bChan,bAttachSize);
}

inline void HAL_CQSetUdBaseAddr(U8 bChan,U32 dUdBuff,U8 bDir)
{
    if(bDir == DMA2_RD_TO_BUFF)//read,set raid buff addr as userdata addr
    {
#ifdef __RAID_FUNCTION_ON__
        //SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (DMA2_OFFS_RAID_CTRL<<R_CQ_REG_BIT) | (0x00& INVALID_U8);/*读关闭Raid*/
#endif
	}

    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_UDATA_BSADDR<<R_CQ_REG_BIT) | (dUdBuff & INVALID_U16);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_UDATA_BSADDR+2)<<R_CQ_REG_BIT) | ((dUdBuff >> 16) & INVALID_U16);
}

inline void HAL_CQSetRaidBaseAddr(U08 bChan,U32 dRaidBuff,U08 bMode)
{
	if(bMode == 1)
	{
		//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (DMA2_OFFS_RAID_CTRL<<R_CQ_REG_BIT) | (0x01& INVALID_U8);/*写打开Raid*/
	    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_RAID_BUF_ADDR<<R_CQ_REG_BIT) | (dRaidBuff & INVALID_U16);
	    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_RAID_BUF_ADDR+2)<<R_CQ_REG_BIT) | ((dRaidBuff >> 16) & INVALID_U16);
	}
	else
	{
		//SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (DMA2_OFFS_RAID_CTRL<<R_CQ_REG_BIT) | (0x00& INVALID_U8);/*写打开Raid*/
	}
}

inline void HAL_CQSetUdAttachSize(U8 bChan,U8 bAttachSize)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rLEN_CFG);
    dOfst += 3;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst<<R_CQ_REG_BIT) | bAttachSize;

	dOfst = (U8)offsetof(SG_DMA2_TypeDef,rUDATA_LEN_HANDLE);
	dOfst += 1;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst<<R_CQ_REG_BIT) | bAttachSize;
}

inline void HAL_CQSetUdLen(U8 bChan, U8 bUdLen)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rLEN_CFG);
    dOfst += 2;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bUdLen;
	
	dOfst = (U8)offsetof(SG_DMA2_TypeDef,rUDATA_LEN_HANDLE);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst<<R_CQ_REG_BIT) | bUdLen;
}

inline void HAL_CQSetDma2Len(U8 bChan, U16 wDma2Len)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rLEN_CFG);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | wDma2Len;
}

void HAL_CQSetLenCfgReg(U8 bChan, U32 dLenCfg)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rLEN_CFG);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (dLenCfg & INVALID_U16);
    dOfst+=2;
	SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | ((dLenCfg >> 16) & INVALID_U16);
}

void HAL_CQSetBchMode(U8 bChan, U8 bMode)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rECC_CFG);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bMode;
}

void HAL_CQSetOverFlow(U8 bChan, U8 bOverFlowVal)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rECC_CFG);
	dOfst+=1;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | bOverFlowVal;
}

void HAL_CQSetSvalueOverFlow(U8 bChan, U16 wOverFlowVal)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rECC_CFG);
	dOfst+=2;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | wOverFlowVal;
}

void HAL_CQSetEccCfgReg(U8 bChan, U32 dEccCfg)
{
    U32 dOfst = (U8)offsetof(SG_DMA2_TypeDef,rECC_CFG);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (dEccCfg & INVALID_U16);
	dOfst+=2;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | ((dEccCfg >> 16) & INVALID_U16);
}

void HAL_CQSetMask(U8 bChan, U8 *pMskBuff, U8 bMskLen)
{
	U08 i=0;
	
    while(i < bMskLen)
    {
		SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_SW_DMA2_REG | ((DMA2_OFFS_MASK+i)<<R_CQ_REG_BIT) | *pMskBuff;
        pMskBuff++;
        i++;
    }
}

#define SUMSUNG_SLC_SEED_CNT  86  //sizeof(SumsungSeedSLC)/sizeof(SumsungSeedSLC[0]);//可写成常数
#define SUMSUNG_TLC_SEED_CNT  256 //sizeof(SumsungSeedTLC)/sizeof(SumsungSeedTLC[0]);
#define TOSHIBA_A_SEED_CNT    208
#define TOSHIBA_B_SEED_CNT    134  //注意，最后是增加了1倍:2*67
#define TOSHIBA_2D_3DGEN1_GAP 1    //东芝选择2D/3D 1代算法，需要决定index_h=index_l+TOSHIBA_2D_3DGEN1_GAP ;目前设置为1或7,后面可设置为HAL UDP的一个全局参数

U16 HAL_GetSamsungSeed(U08 bOffs)
{    
    return SumsungSeedTLC[bOffs];
}

U16 HAL_SumsungSeedIdx(U32 dIdx, U08 bCellType)
{    
    if (CELL_SLC == bCellType)//slc mode
    {
        dIdx %= SUMSUNG_SLC_SEED_CNT; 
        return SumsungSeedSLC[dIdx];
    }
    else//tlc mode
    {
        dIdx &= 0xFF;  
        return SumsungSeedTLC[dIdx];
    }
}

U16 HAL_ToshibaSeedIdx(U32 dIdx,U8 bRngAlg)
{
    U16 wToshibaSeedIdx;
    
    if(bRngAlg == RND_ALG_TOSHIBA_A)//3D-2/3不需要seed_idx_h
    {
        dIdx %= TOSHIBA_A_SEED_CNT;//硬件设置了寄存器，可以不做取余运算
        dIdx |= ((dIdx+TOSHIBA_2D_3DGEN1_GAP)<<8);
    }
    else
    {
        dIdx %= TOSHIBA_B_SEED_CNT;//硬件设置了寄存器，可以不做取余运算
    }

    wToshibaSeedIdx = (U16)dIdx;
    
    return wToshibaSeedIdx;//return seed idx
}

void HAL_CQSetRndSeed(U8 bChan, U32 dRowAddr,U16 wSeedIdx)
{	

//    if(wSeedIdx == 0xFFFF)//when wSeedIdx = 0xFFFF, use dRowAddr as wSeedIdx
//    {
//#if (NAND_FLASH == TOS_BICS3) || (NAND_FLASH == HYNIX_8A1M)
//			if (dRowAddr&0xff000000)
//			{
//				wSeedIdx = 3*(dRowAddr&0xFF) + (dRowAddr>>24)-1;//只用16位
//			}
//			else
//#endif
//			{
				wSeedIdx = dRowAddr;
//			}
//    }

    //if (((gHalVar.pHwReg->ADM_dDma2RndCtrl >> 1) & 0x03) < RND_ALG_TOSHIBA_A)//设置三星种子
//    {
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_SSRND_SEED << R_CQ_REG_BIT) | SumsungSeedTLC[wSeedIdx & 0xFF];
//    }
#if 0
    else //设置东芝种子
    {
	    if(((gHalVar.pHwReg->ADM_dDma2RndCtrl >> 1) & 0x03) == RND_ALG_TOSHIBA_A)//3D-2/3不需要seed_idx_h
	    {
	        wSeedIdx %= TOSHIBA_A_SEED_CNT;//硬件设置了寄存器，可以不做取余运算
	        wSeedIdx |= ((wSeedIdx+TOSHIBA_2D_3DGEN1_GAP)<<8);
	    }
	    else
	    {
	        wSeedIdx %= TOSHIBA_B_SEED_CNT;//硬件设置了寄存器，可以不做取余运算
	    }
        
        SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_SEED_INDEX << R_CQ_REG_BIT) | wSeedIdx;
    }
#endif
}

inline void HAL_CQSetDMAInfoAddr(U8 bChan, U32 dDmaInfoAddr)
{
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (DMA2_OFFS_DMAINF_BSADDR<<16) | (dDmaInfoAddr & 0xffff);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | ((DMA2_OFFS_DMAINF_BSADDR+2)<<16) | ((dDmaInfoAddr>>16) & 0xffff);
}

inline void HAL_McuSetUd(U8 bChan, U32 dUdBuff, U8 bAttachSize, U8 bUdLen,U8 bDir)
{
    SG_DMA2_CH(bChan)->rUDATA_BSADDR.dAll  = dUdBuff;

	SG_DMA2_CH(bChan)->rLEN_CFG.bits.UDATA_LEN = bUdLen;
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.UDATA_ATTCH_SIZE = bAttachSize;
	SG_DMA2_CH(bChan)->rUDATA_LEN_HANDLE.bits.USER_DATA_LEN_HANDLE = bUdLen;
	SG_DMA2_CH(bChan)->rUDATA_LEN_HANDLE.bits.UDATA_ATTACH_SIZE_HANDLE = bAttachSize;
}

inline void HAL_McuSetMask(U8 bChan, U8 bMskInfo, U8 bMskLen,U8 bMakIdx)
{        
    SG_DMA2_CH(bChan)->rMASK = (bMskInfo>>bMakIdx);
}


void HAL_McuSetRndSeed(U8 bChan, U32 dRowAddr,U16 wSeedIdx)
{
    //U8 bRngAlg = (gHalVar.pHwReg->ADM_dDma2RndCtrl >> 1) & 0x03;

	if(wSeedIdx == 0xFFFF)//when wSeedIdx = 0xFFFF, use dRowAddr as wSeedIdx
	{
#if (NAND_FLASH == TOS_BICS3) || (NAND_FLASH == HYNIX_8A1M)
			if (dRowAddr&0xff000000)
			{
				wSeedIdx = 3*(dRowAddr&0xFF) + (dRowAddr>>24)-1;//只用16位
			}
			else
#endif
			{
				wSeedIdx = dRowAddr;//只用16位
			}
	}

    //if (bRngAlg < RND_ALG_TOSHIBA_A)//设置三星种子
    {
    	wSeedIdx = HAL_SumsungSeedIdx(wSeedIdx, CELL_TLC);
    }
    //else//设置东芝种子
    {
    	//wSeedIdx = HAL_ToshibaSeedIdx(wSeedIdx,bRngAlg);
    }
    SG_DMA2_CH(bChan)->rSSRND_SEED.bits.wVAL = wSeedIdx;
}

void HAL_Debug(U8 bChan, U8 bData)
{
	SG_FC(bChan)->rSEND_WR_PULSE.bits.dVAL = bData;
	while(1 != SG_FC(bChan)->rRW_PULSE_DONE.bits.dVAL);
}

//Async模式的写接口
inline void HAL_McuSendWRPulse(U8 bChan, U8 bData)
{
	SG_FC(bChan)->rSEND_WR_PULSE.bits.dVAL = bData;
	__nds32__dsb();
	while(1 != SG_FC(bChan)->rRW_PULSE_DONE.bits.dVAL);
}

inline U8 HAL_McuSendReadPluse(U8 bChan)
{
	U8 temp = 0;
	SG_FC(bChan)->rSEND_RD_PULSE.bits.dVAL = 1;
	__nds32__dsb();
    while(!(SG_FC(bChan)->rRW_PULSE_DONE.bits.dVAL)){}
	temp = (U8)SG_FC(bChan)->rRW_PULSE_DATA.bits.RW_DATA;
	return temp;
}

inline void HAL_CqSetEccCtrlReg(U8 bChan,U32 dEccCtrl)
{
    U32 dOfst = offsetof(SG_DMA2_TypeDef, rECC_CTRL);
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | (dEccCtrl & INVALID_U16);
    dOfst += 2;
    SG_FC(bChan)->rCQ_INSTRUCT_REG = R_CQ_ATTR_CFG_DW_DMA2_REG | (dOfst << R_CQ_REG_BIT) | ((dEccCtrl >> 16) & INVALID_U16);

}

