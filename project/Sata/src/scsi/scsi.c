#include "sg9081.h"
#include "debug.h"
#include "scsi.h"
#include "dma1c.h"
#include "uitility.h"
#include "sata_typedef.h"
#include "task_process.h"
#include "dsata.h"
#ifdef    __PRIVATE_CRC_HANDLE__
#include "AbnormalHandle.h"
#endif

#ifdef __BOOT_CODE__
#include "fc.h"
#endif

T_SCSI_CTRL    tScsiCtl  __section(".scsictrl");
__align(8) T_SCSI_CMD_HEAD tScsiCmdHead __section(".scsicmdhead");
__align(8) T_SCSI_DATA_HEAD tScsiDataHead __section(".scsidatahead");

#ifdef __DUAL_CORE__
U8_C bFwVersionG0[40] = "YS9086BB G0 CODE_20200325 V1.00";
#endif
U8_C bFwVersionG3[40] = "YS9086BB G3 CODE_20200325 V1.00";

U32_C dWRCmdHead[HEAD_KEY_CNT] = 
{
    0x00000000,0x7385F208,0x33DA2BB1,0xC2DF4EB3,
    0xB6BBA9B1,0x80B26CB0,0xB17B5A90,0xA4D3BA0F
};

U32_C dRDCmdHead[HEAD_KEY_CNT] = 
{
    0x00000000,0x857308F2,0x33DA2BB1,0xC2DF4EB3,
    0xB6BBA9B1,0x80B26CB0,0xB17B5A90,0xD3A40FBA
};

void  Scsi_RegisteInterfaceInfo(T_SCSI_MODULE_ITF *ptScsiMI)
{
#ifndef __DUAL_CORE__
    tScsiCtl.dNextAddr = LBA_00005500;
    tScsiCtl.bMode     = NORMAL_MODE;
    tScsiCtl.bMode_old = NORMAL_MODE;
    tScsiCtl.bState    = CMD_STATE;
    tScsiCtl.bVaild    = FALSE;

    //没有分配地址
    tScsiCtl.tScsiMI.bFwVersion=ptScsiMI->bFwVersion;
    tScsiCtl.tScsiMI.ScsiGetNandFlashId = ptScsiMI->ScsiGetNandFlashId;
    tScsiCtl.tScsiMI.ScsiGetValidCENandFlashId = ptScsiMI->ScsiGetValidCENandFlashId;
    tScsiCtl.tScsiMI.ScsiIdleCmdHandleExt = ptScsiMI->ScsiIdleCmdHandleExt;
    tScsiCtl.tScsiMI.ScsiIntCmdHandleExt = ptScsiMI->ScsiIntCmdHandleExt;

    if(tScsiCtl.tScsiMI.bFwVersion == NULL)
    {
        tScsiCtl.tScsiMI.bFwVersion=(U8*)bFwVersionG3;
    }
#else
#ifndef __G3_FLYCODE
    tScsiCtl.dNextAddr = LBA_00005500;
    tScsiCtl.bMode     = NORMAL_MODE;
    tScsiCtl.bState    = CMD_STATE;
    tScsiCtl.bVaild    = FALSE;
#else
    tScsiCtl.tScsiMI.bFwVersion=ptScsiMI->bFwVersion;
    tScsiCtl.tScsiMI.ScsiGetNandFlashId = ptScsiMI->ScsiGetNandFlashId;
    tScsiCtl.tScsiMI.ScsiIdleCmdHandleExt = ptScsiMI->ScsiIdleCmdHandleExt;
    tScsiCtl.tScsiMI.ScsiIntCmdHandleExt = ptScsiMI->ScsiIntCmdHandleExt;
    if(tScsiCtl.tScsiMI.bFwVersion == NULL)
    {
        tScsiCtl.tScsiMI.bFwVersion=(U8*)bFwVersionG3;
    }
#endif

#endif
}

U8 Scsi_CmdHeadVerify(void)
{
    U32 Tstart=tSataDC.dPowerOnTime;
    GPIO_DEBUG(0x28);
    DMAC_DMA1Start(DMA1_WR,(U32)(&tScsiCmdHead.dTitle[0]),1,DMA1_TOTAL_BUFF_SIZE,0);
    tTQC.tHiTasks[0].tParam.dSctSize -= 1; // 写优先取了1个扇区的数据。
    //DMAC_DMA1WaitDone();
   while (!SG_DMA1->rSTATUS.bits.TRANS_DONE)
   {
            //超时等待500ms退出
            if((tSataDC.dPowerOnTime - Tstart) >= 500)
            {       
                    //SG_DMA1->rCTRL.bits.STOP_DMA1_SC = 1;//Stop_DMA1_SC

                    UART_PRINT("WAITTING DMA1 DONE!!\r\n");
                    UART_PRINT("SG_DMA1->rSTATUS.dAll=%#010lx\r\n",SG_DMA1->rSTATUS.dAll);
                    tScsiCtl.dNextAddr = LBA_00005500;
                    tScsiCtl.bMode     = NORMAL_MODE;
                    tScsiCtl.bState    = CMD_STATE;
                    tScsiCtl.bVaild    = FALSE;
                    GPIO_DEBUG(0X4c);
                    return FALSE;
            }
   }
    GPIO_DEBUG(0x29);

    if (FALSE == Mem32Cmp(&tScsiCmdHead.dTitle[0],(U32*)(&dWRCmdHead[0]),HEAD_KEY_CNT))
    {
        GPIO_DEBUG(0X4a);
        if(tScsiCmdHead.dCheckSum == Check32Sum(&tScsiCmdHead.dTitle[0], sizeof(T_SCSI_CMD_HEAD)/4-1))
        {
            GPIO_DEBUG(0X4b);
#ifdef __PRIVATE_CRC_HANDLE__
            ////for WRITE SCSI CRC MONITOR
            tScsiCtl.dWrValidFlg=0xED1F852F;//for WRITE SCSI CRC MONITOR
#endif

            return TRUE;
        }
         
    }
    tScsiCtl.dNextAddr = LBA_00005500;
    tScsiCtl.bMode     = NORMAL_MODE;
    tScsiCtl.bState    = CMD_STATE;
    tScsiCtl.bVaild    = FALSE;
    GPIO_DEBUG(0X4c);
    return FALSE;
}

void Scsi_DataHeadCheckSum(void)
{
#ifdef __G0_CODE
	MemorySet((U08*)&tScsiDataHead.tPrivData.dReadFlag,0,sizeof(T_PrivDataVerify));	//清除PrivData，量产工具不走校验
#endif
    Mem32Copy(&tScsiDataHead.dTitle[0],(U32*)(&dRDCmdHead[0]), HEAD_KEY_CNT);
    tScsiDataHead.dCheckSum = Check32Sum((U32*)(&tScsiDataHead.dTitle[0]),sizeof(T_SCSI_DATA_HEAD)/4-1);
}


U8 Scsi_ScsiModeVerify(U32 dLba,U16 wSct,U8 bOpMode)
{
    if ((SCSI_MODE == tScsiCtl.bMode) &&(LBA_000055AA == dLba))
    {
		if ((OP_RD == bOpMode)&&(1 == wSct)&&(CMD_STATE == tScsiCtl.bState))
		{
#ifndef __BOOT_CODE__
			if( NORMAL_MODE == tScsiCtl.bMode_old)
			{
				tScsiCtl.bMode_old = SCSI_MODE ;
				DSATA_ExbuffAndCheckPoit_Ctrl(SCSI_MODE);
			}
#endif
			//GPIO_DEBUG(0X54);
			Scsi_DataHeadCheckSum();
			GPIO_DEBUG(0x38);
			DMAC_DMA1Start(DMA1_RD,(U32)(&tScsiDataHead.dTitle[0]),1,DMA1_TOTAL_BUFF_SIZE,0);
			DMAC_DMA1WaitDone();
			GPIO_DEBUG(0x39);

			return CMD_RESPONSE;
		}
		else if((OP_WR == bOpMode)&&(CMD_STATE == tScsiCtl.bState))
		{
			//GPIO_DEBUG(0X55);
			tScsiCtl.bState = DATA_STATE;
			return CMD_VAILD;
		}
		else if((OP_RD == bOpMode)&&(DATA_STATE == tScsiCtl.bState))
		{
			//GPIO_DEBUG(0X56);
			tScsiCtl.bState = DATA_STATE;
			return CMD_VAILD;
		}
    }
    else
    {
#ifndef __BOOT_CODE__
		if(( SCSI_MODE == tScsiCtl.bMode_old )&&(NORMAL_MODE == tScsiCtl.bMode))
		{
			tScsiCtl.bMode_old = NORMAL_MODE ;
			DSATA_ExbuffAndCheckPoit_Ctrl(NORMAL_MODE);
		}
#endif
        if ((SCSI_MODE != tScsiCtl.bMode)&& (dLba == tScsiCtl.dNextAddr)&&(1 == wSct)&&(OP_RD == bOpMode))
        {
            switch(tScsiCtl.dNextAddr)
            {
            case LBA_00005500:
                //GPIO_DEBUG(0X50);
                tScsiCtl.dNextAddr = LBA_00000055;
                break;

            case LBA_00000055:
                //GPIO_DEBUG(0X51);
                tScsiCtl.dNextAddr = LBA_0000AA00;
                break;

            case LBA_0000AA00:
                //GPIO_DEBUG(0X52);
                tScsiCtl.dNextAddr = LBA_000000AA;
                break;

            case LBA_000000AA:
                //GPIO_DEBUG(0X53);
                tScsiCtl.dNextAddr = LBA_000055AA;
                tScsiCtl.bMode     = SCSI_MODE;
                tScsiCtl.bState    = CMD_STATE;
                tScsiCtl.bVaild    = FALSE;
                UART_PRINT("Enter SCSI mode....................\r\n");
                break;

            default:
                break;
            }
        }
        else
        {
            tScsiCtl.dNextAddr = LBA_00005500;
            tScsiCtl.bMode     = NORMAL_MODE;
            tScsiCtl.bState    = CMD_STATE;
            tScsiCtl.bVaild    = FALSE;
            UART_PRINT("Exit SCSI mode....................\r\n");
            //GPIO_DEBUG(0X57);
        }
        return CMD_INVAILD;
    }
}



void Scsi_BasisCmdHandle(void)
{
    UART_PRINT("Scsi_BasisCmdHandle\r\n");
    T_BASIS_INFO *ptBasisInfo = (T_BASIS_INFO *)(&tScsiDataHead.bDataDefine[0]);

#ifdef __DUAL_CORE__
      MemoryCopy((U8 *)(&ptBasisInfo->bG0FwVersion[0]),(U8 *)(&bFwVersionG0[0]),FW_VERSION_SIZE);
#else
      //fill zero @ G0 ID
      MemorySet((U8 *)(&ptBasisInfo->bG0FwVersion[0]),0,FW_VERSION_SIZE);
#endif
#ifndef __BOOT_CODE__
      MemoryCopy((U8 *)(&ptBasisInfo->bG3FwVersion[0]),(U8 *)(&tScsiCtl.tScsiMI.bFwVersion[0]),FW_VERSION_SIZE);
      if(tScsiCtl.tScsiMI.ScsiGetNandFlashId != NULL)
      {
#ifdef __DUAL_CORE__
          //GIE_DISABLE();
          //tScsiCtl.tScsiMI.ScsiGetNandFlashId((U8 *)(&ptBasisInfo->bFlashID[0][0][0]));
          //GIE_ENABLE();
          //双核模式，不能直接调用
        __getlock();
        tTQC.tDiTask.eStatus=CMD_VALID;
        tTQC.tDiTask.bOpcode=OP_DI_DUAL_SCSI_IDLE;
        U08MemSet((U8 *)(&ptBasisInfo->bFlashID[0][0][0]),0xFF,sizeof(ptBasisInfo->bFlashID));
        tTQC.tDiTask.dSctSize=(U32)(&ptBasisInfo->bFlashID[0][0][0]);
        tTQC.tDiTask.dLba=(U32)tScsiCtl.tScsiMI.ScsiGetNandFlashId;
        SG_MCU->rSF0_INT1_STS1_SET =1;//ADD DI Task
        __wait4unlocked();//等待任务完成中断
        __unlock();
#else
          tScsiCtl.tScsiMI.ScsiGetNandFlashId((U8 *)(&ptBasisInfo->bFlashID[0][0][0]));
#endif
      }
#else
      //if((tScsiCmdHead.bSubOpCode & 0x01) == 0)     //控制是否返回固件版本号,等于1则不返回
      {
          MemoryCopy((U8 *)(&ptBasisInfo->bG3FwVersion[0]),(U8 *)(&tScsiCtl.tScsiMI.bFwVersion[0]),FW_VERSION_SIZE);
      }

      if((tScsiCmdHead.bSubOpCode & 0x02) == 0)     //控制是否复位flash,等于1则不进行reset
      {
    	  FC_NFReset_Ach();
      }

      if((tScsiCmdHead.bSubOpCode & 0x04) == 0)     //控制是否返回flash ID,等于1则不返回ID
      {
          if(tScsiCtl.tScsiMI.ScsiGetNandFlashId != NULL)
          {
              tScsiCtl.tScsiMI.ScsiGetNandFlashId((U8 *)(&ptBasisInfo->bFlashID[0][0][0]));
          }
      }
      else if((tScsiCmdHead.bSubOpCode & 0x08) == 0)     //先判断有效性在读Flash ID
      {

    	  tScsiCtl.tScsiMI.ScsiGetValidCENandFlashId((U8 *)(&ptBasisInfo->bFlashID[0][0][0]));

      }
#endif
    ptBasisInfo->dResReg0Val=SG_CPM->rSYS_RESERVED0;//FLASH ID之后，上报reserved 寄存器的值
    tScsiDataHead.dCheckSum = Check32Sum((U32*)(&tScsiDataHead.dTitle[0]),sizeof(T_SCSI_DATA_HEAD)/4-1);
    DMAC_DMA1Start(DMA1_RD,(U32)(&tScsiDataHead.dTitle[0]),1,DMA1_TOTAL_BUFF_SIZE,0);
    DMAC_DMA1WaitDone();
    UART_PRINT("Scsi_BasisCmdHandle\r\n");
}


void Scsi_RdWrRamHandle(void)
{
    U16 wSctCnt;
    T_OP_RAM_INFO *ptOpRamInfo = (T_OP_RAM_INFO *)(&tScsiCmdHead.bDataDefine[0]);
#ifndef __DUAL_CORE__
    U32 primask=__get_PRIMASK();
    U32_V *pVecReg;
    pVecReg = &SG_MCU->rG3_VECTOT_TABLE_OFFSET_ADDRESS;
    U32 vector_addr=*pVecReg;
    U32 addr=ptOpRamInfo->dRamAddr;
#ifndef __BOOT_CODE__
    U8 period=SG_CPM->rSYS_CLK_PERIOD;
    if(addr < 0x8000)
    {
        //mcu 访问boot code区间需要降频
        SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
        SG_CPM->rSYS_CLK_PERIOD     = 8;//8 分频
        SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
        __set_PRIMASK(1);
        *pVecReg=0;//临时映射回0地址
    }
#else
    if(addr < 0xA3)
    {
        __set_PRIMASK(1);
        *pVecReg=0;//临时映射回0地址
    }
#endif
#ifdef    __PRIVATE_CRC_HANDLE__
	if(TRUE == Abnormal_Sata_GetFlag())
	{
		Abnormal_Sata_ExitScsiState();
	}
	else
#endif
#endif
	{
		if (RD_SUB_CMD == tScsiCmdHead.bSubOpCode)
		{

#ifdef PRIV_CMD_DATA_JUDGE
			if (tScsiCmdHead.bEnhanceEnable == 0) //
			{
				U32 dBuffAddr = ScanGetBuffAddr();
				MemoryCopy((U08 *)dBuffAddr, (U08 *)ptOpRamInfo->dRamAddr, (ptOpRamInfo->wSctCnt*512));
				U32 dStatus = 0;//SG_DMA2_CH(tBlockAddr.m_bChan)->rSTATUS.dAll;
				PrivRead_CreateDataJudge(dBuffAddr, ptOpRamInfo->wSctCnt, dStatus, &tScsiCmdHead, &tScsiDataHead);

				DMAC_DMA1Start(DMA1_RD,(U32)(&tScsiDataHead.dTitle[0]),1,DMA1_TOTAL_BUFF_SIZE,0);//先回包头
				DMAC_DMA1WaitDone();
				DMAC_DMA1Start(DMA1_RD,dBuffAddr,ptOpRamInfo->wSctCnt,DMA1_TOTAL_BUFF_SIZE,0);
				DMAC_DMA1WaitDone();
			}
			else
#endif
			{


				DMAC_DMA1Start(DMA1_RD,(U32)(&tScsiDataHead.dTitle[0]),1,DMA1_TOTAL_BUFF_SIZE,0);//先回包头
				DMAC_DMA1WaitDone();

				for(wSctCnt=0; wSctCnt<ptOpRamInfo->wSctCnt; wSctCnt++)
				{
#ifndef __DUAL_CORE__
					MemoryCopy((U8 *)(tSataDC.tSataMI.dDmaBuffAddr),(U8 *)(ptOpRamInfo->dRamAddr+ wSctCnt*512),512);
#else
            //通过G0调用过去还是不能读到ROM数据
//            if(tScsiCtl.tScsiMI.ScsiGetRomData != NULL)
//            {
//            	tScsiCtl.tScsiMI.ScsiGetRomData(ptOpRamInfo->dRamAddr,(U8 *)(tSataDC.tSataMI.dDmaBuffAddr),512);
//            }

            __getlock();
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_GET_ROM;
            tTQC.tDiTask.dLba =ptOpRamInfo->dRamAddr+ wSctCnt*512;
            tTQC.tDiTask.dSctSize =512;
            tTQC.tDiTask.dBuffAddr =tSataDC.tSataMI.dDmaBuffAddr;
            SG_MCU->rSF0_INT1_STS1_SET =1;//ADD DI Task

            __wait4unlocked();//等待任务完成中断
            __unlock();
#endif
					DMAC_DMA1Start(DMA1_RD,tSataDC.tSataMI.dDmaBuffAddr,1,DMA1_TOTAL_BUFF_SIZE,0);
					DMAC_DMA1WaitDone();
				}
			}
		}
		else if(WR_SUB_CMD == tScsiCmdHead.bSubOpCode)
		{
			DMAC_DMA1Start(DMA1_WR,ptOpRamInfo->dRamAddr,ptOpRamInfo->wSctCnt,DMA1_TOTAL_BUFF_SIZE,0);
			DMAC_DMA1WaitDone();
#ifdef PRIV_CMD_DATA_JUDGE
			 if (tScsiCmdHead.bEnhanceEnable == 0)
			 {
				 PrivWrite_DataJudge(ptOpRamInfo->dRamAddr, ptOpRamInfo->wSctCnt, tScsiCmdHead);
			 }
#endif
#ifdef  __PRIVATE_CRC_HANDLE__
			if(TRUE == Abnormal_Sata_GetFlag())
			{
				Abnormal_Sata_ExitScsiState();
			}
#endif
		}
	}
#ifndef __DUAL_CORE__
#ifndef __BOOT_CODE__
    if(addr <= 0x8000)
    {
        SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
        SG_CPM->rSYS_CLK_PERIOD     = period;
        SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
        *pVecReg=vector_addr;//恢复临时映射
        __nds32__dsb();
        __set_PRIMASK(primask);
    }
#else
    if(addr <= 0xA3)
	{
    	*pVecReg=vector_addr;//恢复临时映射
    	__nds32__dsb();
		__set_PRIMASK(primask);
	}
#endif
#endif
}
//#pragma GCC optimize ("O0")
void Scsi_CoreSwitchHandle(void)
{
    T_OP_RAM_INFO *ptOpRamInfo = (T_OP_RAM_INFO *)(&tScsiCmdHead.bDataDefine[0]);
    T_CORE_SWITCH_INFO *ptCoreSwitchInfo = (T_CORE_SWITCH_INFO *)(tSataDC.tSataMI.dDmaBuffAddr);
    
    __set_PRIMASK(1);//关闭MCU总中断 
    NVIC_DisableIRQ(IRQ_SC);
    NVIC_DisableIRQ(IRQ_SF1_INT0);
    NVIC_DisableIRQ(IRQ_SF1_INT1);
    
    DMAC_DMA1Start(DMA1_WR,tSataDC.tSataMI.dDmaBuffAddr,ptOpRamInfo->wSctCnt,DMA1_TOTAL_BUFF_SIZE,0);
    DMAC_DMA1WaitDone();
#ifdef PRIV_CMD_DATA_JUDGE
    if (tScsiCmdHead.bEnhanceEnable == 0)
    {
    	PrivWrite_DataJudge(tSataDC.tSataMI.dDmaBuffAddr, ptOpRamInfo->wSctCnt, tScsiCmdHead);
    }
#endif
#ifdef   __PRIVATE_CRC_HANDLE__
	if(TRUE == Abnormal_Sata_GetFlag())
	{
		Abnormal_Sata_ExitScsiState();
	}
	else
#endif
	{
	    if (ptCoreSwitchInfo->dCheckSum == Check32Sum((U32 *)tSataDC.tSataMI.dDmaBuffAddr,sizeof(T_CORE_SWITCH_INFO)/4-1))
	    {
	        SG_CPM->rSYS_RESERVED0 = ptCoreSwitchInfo->dResReg0Val;

	        if(ptCoreSwitchInfo->bG0RunningEn)
	        {
	            NVIC_ClearPendingIRQ(IRQ_SC);
	            NVIC_ClearPendingIRQ(IRQ_SF1_INT0);
	            NVIC_ClearPendingIRQ(IRQ_SF1_INT1);
	            SG_MCU->rG0_VECTOT_TABLE_OFFSET_ADDRESS = ptCoreSwitchInfo->dG0VectorAddr;
	            __nds32__dsb();
	            SG_CPM->rMCU_CTRL.bits.bVAL = 1;
	        }

	        if(ptCoreSwitchInfo->bG3RunningEn)
	        {
#ifndef __DUAL_CORE__
#ifndef __BOOT_CODE__
	             if(ptCoreSwitchInfo->dG3PCAddr < 0x8000)
	             {
	                 //mcu 访问boot code区间需要降频
	                 SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
	                 SG_CPM->rSYS_CLK_PERIOD     = 8;
	                 Delay(2);
	                 SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
	             }
#endif
	             ((void (*)())(ptCoreSwitchInfo->dG3PCAddr))();
#else
	            tTQC.tDiTask.bOpcode=OP_DI_SWITCHCODE;
	            tTQC.tDiTask.eStatus=CMD_VALID;
	            tTQC.tDiTask.dLba=ptCoreSwitchInfo->dG3PCAddr;
	            //创建通知G3切换代码的任务
	            SG_MCU->rSF0_INT1_STS1_SET =1;
	            SG_MCU->rSF1_INT1_STS1_CLR =SG_MCU->rSF1_INT1_STS1;
	            SG_CPM->rMCU_CTRL.bits.bVAL = 0;
	            while(1);
#endif
	            UART_PRINT("ptCoreSwitchInfo->dG3PCAddr=%#010lx\r\n",ptCoreSwitchInfo->dG3PCAddr);
	        }

	    }
	}

    NVIC_EnableIRQ(IRQ_SC);
    NVIC_EnableIRQ(IRQ_SF1_INT0);
    NVIC_EnableIRQ(IRQ_SF1_INT1);
    __set_PRIMASK(0);//打开MCU总中断 
}


U8 Scsi_IntCmdHandle(U8 bOpMode)
{
    U8 bFlag = TRUE;
    tScsiCtl.bDir = bOpMode;
    switch(tScsiCmdHead.bOpCode)
    {
    case 0x10: 
        if(OP_RD == bOpMode)
        {
            tScsiCtl.bVaild = TRUE;
        }
        break;

    case 0x11:
        if (((OP_RD == bOpMode)&&(RD_SUB_CMD == tScsiCmdHead.bSubOpCode))
            || ((OP_WR == bOpMode)&&(WR_SUB_CMD == tScsiCmdHead.bSubOpCode)))//sg9081cc
        {
            tScsiCtl.bVaild = TRUE;
        }
        break;

    case 0x12:
        if(OP_WR == bOpMode)
        {
            tScsiCtl.bVaild = TRUE;
        }
        break;

    default:

        if(tScsiCtl.tScsiMI.ScsiIntCmdHandleExt != NULL)
        {
#ifdef __DUAL_CORE__
            GIE_DISABLE();
            bFlag = tScsiCtl.tScsiMI.ScsiIntCmdHandleExt(bOpMode);
            GIE_ENABLE();
#else
            bFlag = tScsiCtl.tScsiMI.ScsiIntCmdHandleExt(bOpMode);
#endif
        }
        else
        {

            bFlag = FALSE;
        }

        if(!bFlag)
        {
            tScsiCtl.dNextAddr = LBA_00005500;
            tScsiCtl.bMode     = NORMAL_MODE;
            tScsiCtl.bState    = CMD_STATE;
            tScsiCtl.bVaild    = FALSE;
        }
        break;
    }
    return bFlag;
}

//标准的操作类型0x10~0x1f
U8 Scsi_IdleCmdHandle(U8 bOpMode)
{
    U8 bFlag = TRUE;
    tScsiCtl.bVaild = FALSE;
    tScsiCtl.bState = CMD_STATE;
    switch(tScsiCmdHead.bOpCode)
    {
    case 0x10: 
        //if(OP_RD == bOpMode)
        //{
            Scsi_BasisCmdHandle();
        //}
        break;

    case 0x11:
        //if ((OP_RD == bOpMode)&&(RD_SUB_CMD == tScsiCmdHead.bSubOpCode)
        //|| (OP_WR == bOpMode)&&(WR_SUB_CMD == tScsiCmdHead.bSubOpCode))
        //{
            Scsi_RdWrRamHandle();
        //}
        break;

    case 0x12:
        //if(OP_WR == bOpMode)
        //{
            Scsi_CoreSwitchHandle();
        //}
        break;

    default:
#ifndef __BOOT_CODE__
#ifndef __DUAL_CORE__
        if(tScsiCtl.tScsiMI.ScsiIdleCmdHandleExt != NULL)
        {
            bFlag = tScsiCtl.tScsiMI.ScsiIdleCmdHandleExt(bOpMode);
        }
        else
        {

            bFlag = FALSE;
        }
#else
        //双核模式，不能直接调用
        __getlock();
        tTQC.tDiTask.eStatus=CMD_VALID;
        tTQC.tDiTask.bOpcode=OP_DI_DUAL_SCSI_IDLE;
        tTQC.tDiTask.dSctSize=bOpMode;
        tTQC.tDiTask.dLba=(U32)tScsiCtl.tScsiMI.ScsiIdleCmdHandleExt;
        SG_MCU->rSF0_INT1_STS1_SET =1;//ADD DI Task
        if(!__wait4unlocked())//等待任务完成中断
        {
            //超时
            bFlag = FALSE;
        }
        else
        {
            bFlag=tTQC.tDiTask.eStatus;
        }
        __unlock();
#endif

        if(!bFlag)
#endif
        {
            tScsiCtl.dNextAddr = LBA_00005500;
            tScsiCtl.bMode     = NORMAL_MODE;
            //TODO：没有实现的SCSI命令，不启动DMA1会导致SATA接口没有数据超时

			U32 dSctSize = tTQC.tHiTasks[0].tParam.dSctSize;
			if((DIR_OUT == tTQC.tHiTasks[0].tParam.bDir) && (dSctSize>1))
			{
				 dSctSize = dSctSize - 1;
			}
			DMAC_DMA1Start(tTQC.tHiTasks[0].tParam.bDir,tSataDC.tSataMI.dDmaBuffAddr,tTQC.tHiTasks[0].tParam.dSctSize,DMA1_TOTAL_BUFF_SIZE,0);//先回包头
			DMAC_DMA1WaitDone();
        }
        break;
    }
#ifdef __PRIVATE_CRC_HANDLE__
    Abnormal_Sata_ClearStFlag();
#endif
    return bFlag;
}


void Scsi_DummyReadWrite(U32 dwTag)
{
}
