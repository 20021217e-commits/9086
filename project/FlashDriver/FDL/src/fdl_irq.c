#include "fdl_irq.h"
#include "sg9081.h"
#include "fdl_task_op.h"
#include "fdl_exception.h"
#include "fdl_read_retry.h"
#include "fdl_write_fail_handler.h"

void FDL_FCCQIRQHandler(U8 bChan, U8 bIrqNum)
{
	NVIC_ClearPendingIRQ(bIrqNum);
	
    if (HAL_NFGetOrClrChkStsHwFlag(bChan,GET_CHKSTS_FLAG))
    {
		if(tFtlTask.dTaskType == TYPE_ERASE_BLOCKS)
		{
			PRINT_HEX("ERR_ERS_STS_FAIL=",SG_FC(bChan)->rCHK_CQ_CTRL.dAll);
			FDL_Log("bChan=%d",bChan);
			Fdl_EraseFailHandler(bChan);
		}
		else if(SG_DMA2_CH(bChan)->rCTRL.bits.DIR == 0)//should use tFtlTask.dTaskType
		{
		    if((tFtlTask.dTaskType == TYPE_WRITE_PAGES) || (tFtlTask.dTaskType == TYPE_NCQ_WRITE))
            {
                PRINT_TXT("Wt-Fail");
				pFdlParam.FTL_ExceptionWriteFailHandle(0,Fdl_GetWriteFailInfor(bChan));
#ifdef __SLC_PRG_FAIL_HANDL__
                if((tFtlTask.dTaskType == TYPE_WRITE_PAGES) || 
                    (((T_WRITE_EXCEPTION *)tFtlTask.tTaskCtrl->dWriteExBuffAddr)->bDescript == 0))
                {
                    Fdl_WriteFailHandler(bChan);//如果是联动写流程并且异常buff已经置起来了，则不再走流程
                }
                else
                {
                    //PRINT_TXT("HitJointWr 2 fail!!");
                }
#endif
                //Fdl_WriteFailHandler(bChan);
            }      			
		}
		if (SG_FC(bChan)->rCHK_CQ_CTRL.bits.CHK_ST_MAXTIME_FLAG)	//处理Check CE有效性的问题
		{
			bCqTimeoutFlag[bChan] = 1;
		}
		
        HAL_NFGetOrClrChkStsHwFlag(bChan,CLR_CHKSTS_FLAG);
    }
    
	if((SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_FLAG)/* || (SG_DMA2_CH(bChan)->rSTATUS.bits.SVA_OVF_FLAG)*/)
	{	
		FDL_Log("CQ_HOLD_FLAG = 1,bChan=%d",bChan);
//		while(1);
		if(tFtlTask.dTaskType == TYPE_VERIFY_TLC_PAGE)	
		{
#ifdef __VER_TLC_PAGE__
			FDL_RetryRecordFailInfoVerTLC(bChan);
#endif  
		}
		else
		{
			FDL_RetryRecordFailInfor(bChan);	//wOfstInTask在Retry中没用
		}
          			            
        SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;
        __nds32__dsb();
        SG_FC(bChan)->rCQ_HOLD_CTRL.bits.DMA2_ABNL_HOLD_CQ_FLAG =  0;//clear hold en
        __nds32__dsb();
        SG_FC(bChan)->rCQ_HOLD_CTRL.bits.CQ_HOLD_FLAG = 0;//clear int infor
        SG_DMA2_CH(bChan)->rSTATUS.bits.SVA_OVF_FLAG =0;
        __nds32__dsb();
#ifndef	RETRY_IN_G0			//使能该宏定义时，retry在G0实现；关闭后，retry在G3实现
        FDL_ReadRetry();
#endif
	}

#ifdef 	__FDL_CQFIFO_TRIG_IRQ__
	U8 i;
	if (SG_FC(bChan)->rCHK_INT_ST.bits.CQ_FIFO_RDY_INT == 1)
	{
	    for (i = 0; i < 4; i++)
	    {
	    	SG_FC(i)->rCQ_CTRL.bits.CQ_FIFO_RDY_INT_EN = 0;
	    	SG_FC(i)->rCQ_FIFO_ST_CTRL.bits.CQ_FIFO_RDY_FLAG = 0;
	    }
	    RunFtlTaskErgodic(TASK_IRQ);
	    return ;
	}
#endif
    if (SG_FC(bChan)->rCHK_INT_ST.bits.CQ_INT_FLAG)
    {
        SG_FC(bChan)->rCHK_INT_ST.bits.CQ_INT_FLAG = 0;//CQ generate int

		RunFtlTaskErgodic(TASK_IRQ);
    }
#ifdef __TASK_ERROR_FORCE_STOP__
	Fdl_UpdateTaskTime(TASK_IRQ);
#endif
}

void FDL_DMAIRQHandler(U8 bChan, U8 bIrqNum)
{
    HAL_ADMWaitDMADone(bChan);
    SG_DMA2_CH(bChan)->rCTRL.bits.FIFO_CLR = 1;
    NVIC_ClearPendingIRQ(bIrqNum);
}

void FC0_CQ_IRQHandler(void)
{
    FDL_FCCQIRQHandler(0,IRQ_FC0_CQ);   
}

void FC1_CQ_IRQHandler(void)
{
    FDL_FCCQIRQHandler(1,IRQ_FC1_CQ);   
}

void FC2_CQ_IRQHandler(void)
{
    FDL_FCCQIRQHandler(2,IRQ_FC2_CQ);
}

void FC3_CQ_IRQHandler(void)
{
    FDL_FCCQIRQHandler(3,IRQ_FC3_CQ);
}

void DMA20_IRQHandler(void)
{
    FDL_DMAIRQHandler(0,IRQ_DMAC_DMA20);
}

void DMA21_IRQHandler(void)
{
    FDL_DMAIRQHandler(1,IRQ_DMAC_DMA21);
}

void DMA22_IRQHandler(void)
{
    FDL_DMAIRQHandler(2,IRQ_DMAC_DMA22);
}

void DMA23_IRQHandler(void)
{
    FDL_DMAIRQHandler(3,IRQ_DMAC_DMA23);
}

void DMA1_IRQHandler(void)
{
	GIE_DISABLE();
	NVIC_ClearPendingIRQ(IRQ_DMAC_DMA1);

	if (SG_DMA1->rSTATUS.bits.MC_INT_STS)
	{
		if (SG_DMA1->rCTRL.bits.DIR == 0)//1:w 0:r
		{
			gFdlTskStatus |= ST_DMA1_READ_DONE;
		}
	}
	
	SG_DMA1->rSTATUS.dAll = 0;
	GIE_ENABLE();
}

#ifdef __DUAL_CORE__
void CPM_IRQHandler(void)
{
	U08 bVDTStatus		= VDT_NORMAL;
	//U32 dBuffAddr		= 0;
    U16 wInt_sts		= SG_CPM->rCPM_INT_STS.wAll;

    wInt_sts &=SG_CPM->rCPM_INT_CTRL.wAll;

    if(wInt_sts&0x01)			//VDT_fall_int_sts. vdt_warn falling int status
	{
    	SG_CPM->rCPM_INT_STS.bits.VDT_WARN_FALL_INT_STS = 0;
    	SG_CPM->rVDT_CNT = 0;	//cout VDT_low_cnt from VDT_fall_int
    	bVDTStatus = VDT_FALL;

		if (pFdlParam.tVDTExceptCtrl.m_VDTStatus != VDT_LOW)	//只有等	FTL清了该标志之后，才重新记录
		{
			pFdlParam.tVDTExceptCtrl.m_VDTStatus = bVDTStatus;
		}
    	
		FDL_Log("VDT warn falling");
	}

	if((wInt_sts>>6)&0x01)		//VDT_low_int_sts. vdt_warn low int status
	{
		PRINT_TXT("V_Low");
	    //PRINT_HEX("V_Low IPC:", __nds32__mfsr(NDS32_SR_IPC));//打印触发VDT中断时运行的PC指针
		pFdlParam.FTL_ExceptionVdtHandle(0);
            
		if (ST_TASK_DONE != gFdlTskStatus)
		{
			bVDTStatus = VDT_LOW;
        
			pFdlParam.tVDTExceptCtrl.m_VDTStatus = bVDTStatus;

#ifndef __SIMPLIFY_CODE__ //并未使用异常buff记录的vdt标志来处理发生vdt异常时的动作
			if (TYPE_ERASE_BLOCKS == tFtlTask.dTaskType)
			{
				dBuffAddr = tFtlTask.tTaskCtrl->dEraseExBuffAddr;
				
				T_ERASE_EXCEPTION *tEraseEx = (T_ERASE_EXCEPTION *)dBuffAddr;
				tEraseEx->bDescript |= (1<<ERS_ERR_VDT);
			}
			else if (TYPE_WRITE_PAGES==tFtlTask.dTaskType || TYPE_NCQ_WRITE==tFtlTask.dTaskType)
			{
				dBuffAddr = tFtlTask.tTaskCtrl->dWriteExBuffAddr;
				
				T_WRITE_EXCEPTION *tWriteEx = (T_WRITE_EXCEPTION *)dBuffAddr;
				tWriteEx->bDescript |= (1<<WR_ERR_VDT);
			}
			else if ((TYPE_READ_TO_BUFFER==tFtlTask.dTaskType) || 
					 (TYPE_READ_LITTLE_DATA==tFtlTask.dTaskType) || 
					 (TYPE_NCQ_READ==tFtlTask.dTaskType) || 
					 (TYPE_NCQ_READ_4K==tFtlTask.dTaskType) || 
					 (TYPE_READ_TO_BUFFER_4K==tFtlTask.dTaskType) || 
					 (TYPE_PRE_READ_4K==tFtlTask.dTaskType))
			{
				dBuffAddr = tFtlTask.tTaskCtrl->dReadExBuffAddr;
				
				T_READ_EXCEPTION *tReadEx = (T_READ_EXCEPTION *)dBuffAddr;
				tReadEx->bDescript |= (1<<RD_ERR_VDT);
			}
#endif
		}

		while (0 == SG_CPM->rCPM_INT_STS.bits.VDT_WARN_RISE_INT_STS);	//等VDT上升沿
        delay_ms(2);//防止电源波动及在操作flash写读过程电压波动

        pFdlParam.FTL_ExceptionVdtHandle(2);

		bVDTStatus = VDT_NORMAL;
		if (pFdlParam.tVDTExceptCtrl.m_VDTStatus != VDT_LOW)	//
		{
			pFdlParam.tVDTExceptCtrl.m_VDTStatus = bVDTStatus;
		}
		
		SG_CPM->rCPM_INT_STS.bits.VDT_WARN_RISE_INT_STS = 0;
		SG_CPM->rCPM_INT_STS.bits.VDT_LOW_INT_STS = 0;
		SG_CPM->rVDT_CNT = 0;	 
		FDL_Log("VDT warn rising");
	}

	if((wInt_sts>>1)&0x01)			//VDT_rise_int_sts. vdt_warn rising int status
	{
		bVDTStatus = VDT_NORMAL;	//上升沿到来，表明VDT已经处于正常状态
		if (pFdlParam.tVDTExceptCtrl.m_VDTStatus != VDT_LOW)	//只有等	FTL清了该标志之后，才重新记录
		{
			pFdlParam.tVDTExceptCtrl.m_VDTStatus = bVDTStatus;
		}
		
		SG_CPM->rCPM_INT_STS.bits.VDT_WARN_RISE_INT_STS = 0;
		FDL_Log("VDT warn rising2");
	}
	//FDL_Log("VDTStatus = %d",pFdlParam.tVDTExceptCtrl.m_VDTStatus);
	NVIC_ClearPendingIRQ(IRQ_CPM);
}
#endif

