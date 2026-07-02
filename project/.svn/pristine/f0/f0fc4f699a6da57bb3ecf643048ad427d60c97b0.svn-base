#include "sg9081.h"
#include "sata_typedef.h"
#include "dma1c.h"
//#include "DMAC.h"
#include "debug.h"
#include "uitility.h"
#include "sata_misc.h"
#ifdef __G3_FLYCODE
#include "interface.h"
#endif
#include "scsi.h"

void DMAC_DMA1Init(void)
{
   SG_DMA1->rENABE.bits.EN                  = 1U;
   SG_DMA1->rENABE.bits.MC_HOLD_INTEN       = 0U;
   SG_DMA1->rENABE.bits.TOTAL_DONE_INT_EN   = 0U;
   SG_DMA1->rLEN.bits.wVAL                  = 0x200U;
   SG_DMA1->rINDEX                          = 0x0U;
   SG_DMA1->rTOTAL_SECT                     = 0x0U;

   SG_DMA1->rSTATUS.bits.DMA1_DONE_INT_STS = 0U;
   SG_DMA1->rSTATUS.bits.MC_INT_STS         = 0U;
   SG_DMA1->rSTATUS.bits.HOLD_INT_STS       = 0U;
}

void DMAC_DMA1WaitDone(void)
{
    U32 Tstart=tSataDC.dPowerOnTime;
    while (!SG_DMA1->rSTATUS.bits.TRANS_DONE)
   {
        //超时等待500ms退出
        if((tSataDC.dPowerOnTime - Tstart) >= 3000)
        {      
            Tstart=tSataDC.dPowerOnTime;
            ERROR_PRINT("WAITTING DMA1 DONE TIMEOUT!!\r\n");
            TEST_PRINT("DMA1_idx = %d,\r\n",SG_DMA1->rINDEX);

#ifndef __BOOT_CODE__
#ifdef __DUAL_CORE__
#ifdef __G3_FLYCODE
            /*方案1，disable DMA1*/
            //SG_DMA1->rCTRL.bits.STOP=1;//必须要STOP
            //SG_DMA1->rENABE.bits.EN=0;
            /*方案2，启动填充*/
//            SG_DSATA->rFILL_INVLDA_ST.dAll = 1;//开启填充
//            while (!SG_DMA1->rSTATUS.bits.TRANS_DONE);
            //通知G0 进行数据传输出错
            tTQC.enEventType=E_TRANS_TIMEOUT;
            SG_MCU->rSF1_INT0_STS.dAll=1;
            __nds32__dsb();
            Tstart = tSataDC.dPowerOnTime;
            while(SG_MCU->rSF1_INT0_STS.dAll == 1)//清除操作由另一个核完成。
            {
                 if((tSataDC.dPowerOnTime - Tstart) >= 100)
                 {
                     //G0 响应超时
                     ERROR_PRINT("WAITTING G0 TASK DONE TIMEOUT!!\r\n");
                     SG_DMA1->rCTRL.bits.STOP=1;//必须要STOP
                     return;
                 }
            }
#endif
#else
            DSATA_ExceptionHandler(1);
            SG_CPM->rCOMRESET_CTRL.bits.COMRESET_EN=1;
            SG_CPM->rCOMRESET_CTRL.bits.COMRESET_RST_HCLK_EN=1;
            SG_CPM->rCOMRESET_CTRL.bits.COMRESET_RST_ACLK_EN=1;
            SG_CPM->rCOMRESET_CTRL.bits.COMRESET_RST_PCLK_EN=1;
            SG_CPM->rCPM_INT_CTRL.bits.COMRESET_INT_EN=1;//使能异常中断
#endif

#else
            SG_DMA1->rCTRL.bits.STOP=1;//必须要STOP
#endif
        }
   }
    /*方案1，disable DMA1*/
   //SG_DMA1->rENABE.bits.EN=1;
}
void  WaitBuffStatusClr(void)
{
    U16 wIndex;

    for(wIndex=0; wIndex < SG_ADM->rBUF_SIZE; wIndex += 16)
    {
        do
        {
        	SG_ADM->rBIT_INDEX.bits.dVAL = wIndex;
        	SG_ADM->rREAD_BUF_STS.bits.dVAL = 1;
            __nds32__dsb();
            while(SG_ADM->rREAD_BUF_STS.bits.dVAL);		//0:read reg done
        }while(SG_ADM->rMCU_RDATA_STS.bits.dVAL);			//0:buf-clr
    }
}
//#pragma O0//sg9081cc调用此接口默认是切换到非联动模式
void DMAC_DMA1Start(U8 bDir,U32 dDmaBuffAddr,U16 wSctCnt,U32 dDmaBuffSize,U8 wIndex)
{
#ifdef __PRIVATE_CRC_HANDLE__
	extern U8 Abnormal_Sata_GetFlag(void);
	if(TRUE == Abnormal_Sata_GetFlag() && bDir == 1)
	{
		PRINT("Abnormal_Sata_GetFlag\n");
		return;
	}
#endif



#ifndef __ALG_FORMAL
#ifndef __RAW_G3_CODE
#ifdef __G3_FLYCODE
    if(E_TASK_DI_LOG == bTaskType || E_TASK_DI_DATA == bTaskType)
    {
        if(E_TASK_DI_LOG == bTaskType)
        {
            //sata log 数据有自己的缓冲区，需要搬移
            DEBUG_PRINT("DMABUFFER=%x,Index=%x\r\n",dDmaBuffAddr,wIndex);
            INTF_Di_DataProcess(&tTQC,dDmaBuffAddr);
        }
        else
        {
            //E_TASK_DI 共用buffer上层已经计算好数据偏移，不用再搬移
            //通知G0 进行数据传输
            tTQC.enEventType=E_TRANS_WO_DMA;
            tTQC.dEventParam[0]=bDir;
            tTQC.dEventParam[1]=wSctCnt;
            tTQC.dEventParam[2]=dDmaBuffAddr + wIndex * 512;
            SG_MCU->rSF1_INT0_STS.dAll=1;
            DEBUG_PRINT("DMABUFFER=%x,Index=%x\r\n",tTQC.dEventParam[2],wIndex);
            __nds32__dsb();
            U32 Tstart=tSataDC.dPowerOnTime;
            while(SG_MCU->rSF1_INT0_STS.dAll == 1)//清除操作由另一个核完成。
            {
                 if((tSataDC.dPowerOnTime - Tstart) >= 3000)
                 {
                     //G0 响应超时
                     Tstart=tSataDC.dPowerOnTime;
                     while(1);
                 }
            }

        }
        return;
    }

#endif
#endif
#endif
//	if( !tSataDC.bIsNcqCmd && (NORMAL_MODE == tScsiCtl.bMode)&&(SG_DSATA->rPDTR == 0) && (SG_DSATA->rPSERR.dAll == 0))
//	{
//		//flycode 模式下延迟启动PDTR
//		DSATA_TransferStart(CMD_SLOT0);
//	}
    //wait buff-status clr
#ifndef __BOOT_CODE__
    if(SG_DMA1->rCTRL.bits.JOINT_EN)
    {
        WaitBuffStatusClr();
        while(SG_DMA2_CH(0)->rSTATUS.bits.REAL_DONE == 0);
        while(SG_DMA2_CH(1)->rSTATUS.bits.REAL_DONE == 0);
        while(SG_DMA2_CH(2)->rSTATUS.bits.REAL_DONE == 0);
        while(SG_DMA2_CH(3)->rSTATUS.bits.REAL_DONE == 0);
        SG_DMA1->rCTRL.bits.JOINT_EN = 0;
    }
#endif
    SG_DMA1->rBSADDR.dAll = dDmaBuffAddr;
    //SG_ADM->rBUF_SIZE.wAll = dDmaBuffSize;
    //ADM_DMA1SetBaseAddr(dDmaBuffAddr);
    //ADM_SetBuffSize(dDmaBuffSize);

    SG_DMA1->rTOTAL_SECT                     = wSctCnt;
    SG_DMA1->rINDEX                          = wIndex;
/*
    ERROR_PRINT("SG_DSATA->rPDTR=%#010lx\r\n",SG_DSATA->rPDTR);
    ERROR_PRINT("SG_DSATA->rPCMDR.dAll=%#010lx\r\n",SG_DSATA->rPCMDR.dAll);
    ERROR_PRINT("SG_DSATA->rPSSTSR.dAll=%#010lx\r\n",SG_DSATA->rPSSTSR.dAll);
    ERROR_PRINT("SG_DSATA->rPPICR=%#010lx\r\n",SG_DSATA->rPPICR);

    ERROR_PRINT("dDmaBuffAddr=%#010lx\r\n",dDmaBuffAddr);
    ERROR_PRINT("SG_DMA1->rINDEX=%#010lx\r\n",SG_DMA1->rINDEX);
    ERROR_PRINT("SG_DMA1->rTOTAL_SECT=%#010lx\r\n",SG_DMA1->rTOTAL_SECT);
*/
    if (DMA1_RD == bDir)
    {
        SG_DMA1->rCTRL.bits.DIR = 0;
    }
    else
    {
        SG_DMA1->rCTRL.bits.DIR = 1;
    }
    __nds32__dsb();
    SG_DMA1->rCTRL.bits.START = 1;
}

void DMAC_DMA1JointModeEn(void)
{
    SG_DMA1->rCTRL.bits.JOINT_EN = 1;
}

void DMAC_DMA1JointModeDis(void)
{
    SG_DMA1->rCTRL.bits.JOINT_EN = 0;
}
