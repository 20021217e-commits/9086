#include "interface.h"
#include "sata_typedef.h"
#include "dma1c.h"
#include "sg9081.h"
#include "main.h"
#include "debug.h"
#include "ata_identify.h"
#include "uitility.h"
#include "CmdHandle.h"
#include "hal.h"
#include "ftl.h"


#if defined(__TEST_TXBENCH_SPEED_TLCMODE__) || \
	defined(__TEST_TXBENCH_SPEED_SLCMODE__) || \
	defined(__TEST_JUPITER_SPEED_SLCMODE__)
#else
#define __FDL_TEST__  //延时等NCQ聚拢
#endif

U64 uCapacity;
E_TASK_TYPE	bTaskType;
BOOL bIsDummyRWEn;

void DummyRW_Process ();

void Remove_AllTask(U8 bTag,U32 dFisNum)
{
    //remove all tasks
    U32 __sda_addr=__get_sda_base();
    //根据TAG,FISNUM计算出错的DMA idx位置
//	if((dFisNum*16) > tTQC.tHiTasks[bTag].tParam.dSctSize)
//	{
//		//不可能出现这种情况，出现了肯定有什么地方没处理好
//		while(1);
//	}
    //err_idx= tTQC.tHiTasks[bTag].tParam.dDmaIdx + (dFisNum)*16;//计算实际出错的FIS包对应的DMA IDX位置

    g_tPendingCmdList.dCmdBitMask = 0;
    g_tPendingCmdList.bCmdCnt     = 0;
    g_tPendingCmdList.bCmdIndex	  = 0;

    __restore_sda_base(__sda_addr);
}
void  LgRdWrCtrl(U32  dLgSectAddr,  U32  dLen,  U8 bType);

void Exception_DummyReadWrite(U32 dwTag)
{
    //get sp
    //U32 dSP=__nds32__mfsr(NDS32_SR_SP_USR);
    //printf sp info
    U32 dSP=__nds32__get_current_sp();
    PRINT_HEX("SP>",dSP);
    Delay(10);//等待打印完成

    //放弃当前进行中的所有任务
    //reset phy
    SG_CPM->rPRE_CLK_SOURCE_SEL=0x02;//选择25M
    SG_CPM->rSYS_CLK_SOURCE_SEL=1;//选择pre clk

    SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0xF;//offline
    Delay(10);
    SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0;//normal
    Delay(10);
    SystemClkRecovery();//PHY时钟稳定后，恢复时钟配置

    bIsDummyRWEn=TRUE;
    //do something
    while(SG_MCU->rSF0_INT1_STS0 != 0 || SG_MCU->rSF1_INT1_STS0 != 0)
    {
        if(SG_DSATA->rFILL_INVLDA_ST.dAll != 0)
        {
            SG_DSATA->rFILL_INVLDA_ST.dAll=0;
        }
    }

    bIsDummyRWEn=TRUE;
    //do something
    while(1)
    {
#ifdef __DUAL_CORE__
        INTF_TaskQueuedProcess(&tTQC);
#else
        DSATA_ModuleProcess();
#endif
    }
}

void INTF_TaskQueuedProcess (T_TASK_QUEUE_CTX *queue)
{
    U32 dVal;
    /********************************HI 任务处理********************************/
    dVal=SG_MCU->rSF0_INT1_STS0;
    if(dVal!=0 && SG_MCU->rSF1_INT1_STS0 == 0)
    {
        bTaskType = E_TASK_HI;

        if(queue->tHiTasks[0].tParam.bIsUsed && queue->tHiTasks[0].bOpcode != OP_HI_DATA)
        {
            SG_MCU->rSF0_INT1_STS0_CLR =dVal;//清除准备处理的任务
            INTF_Hi_NoneDataTaskProcess(queue);
        }
        else
        {
            //TODO:有HI 任务，如果支持省电模式，就切换到ACTIVE状态
            if(!bIsDummyRWEn)
            {
#ifdef __FDL_TEST__
                static U32 cnt=0,num_bk=0;
                U8 i,num;
                for(i=32,num=0;i>0;i--)
                {
                    if((dVal &(1<<(i-1))) != 0)
                    {
                        num++;//统计dVal中的BIT=1的位数
                    }
                }
                if(num_bk!=num)
                {
                    //等待过程中有新任务出现，可能是NCQ命令流程，则刷新超时计数
                    cnt=0;
                }
                num_bk=num;//记录新的命令数
                if(num >= 32 ||cnt >= 1000)
                {
                    cnt=0;
                    num_bk=0;
                    INTF_Hi_DataTaskProcess(queue);
                }
                else
                {
                    cnt++;
                }
#else
                INTF_Hi_DataTaskProcess(queue);
#endif
            }
            else
            {
                DummyRW_Process();
            }
        }

    }

    /***********************************DI 任务处理***********************************/
    dVal=SG_MCU->rSF0_INT1_STS1;
    if(dVal!=0 && SG_MCU->rSF1_INT1_STS1 == 0)
    {
        bTaskType = E_TASK_DI_DATA;

        SG_MCU->rSF1_INT1_STS1_SET =dVal;//记录准备处理的任务
        SG_MCU->rSF0_INT1_STS1_CLR =dVal;//清除准备处理的任务
        INTF_Di_TaskProcess(queue);

        bTaskType = E_TASK_NONE;
    }
}

#ifdef __DUAL_CORE__
void GetRomData(U32 addr,U8 *pDesBuff,U32 size)
{
    U32 primask=__get_PRIMASK();
    U32_V *pVecReg;
    pVecReg = &SG_MCU->rG3_VECTOT_TABLE_OFFSET_ADDRESS;
    U32 vector_addr=*pVecReg;
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

    MemoryCopy(pDesBuff,(U8 *)(addr),size);

    if(addr < 0x8000)
    {
        //mcu 访问boot code区间需要降频
        SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
        SG_CPM->rSYS_CLK_PERIOD     = period;//8 分频
        SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
        *pVecReg=vector_addr;//恢复临时映射
        __nds32__dsb();
        __set_PRIMASK(primask);
    }
}
#endif

void INTF_Di_TaskProcess (T_TASK_QUEUE_CTX *queue)
{

    //有DI 任务,处理DI任务
    if(queue->tDiTask.bOpcode == OP_DI_SWITCHCODE)
    {
#ifdef __DUAL_CORE__
        //代码切换
        SG_MCU->rSF1_INT1_STS1_CLR =SG_MCU->rSF1_INT1_STS1;
        if(queue->tDiTask.dLba < 0x8000)
        {
             //mcu 访问boot code区间需要降频
             SG_CPM->rSYS_CLK_SWITCH_EN  = 0;
             SG_CPM->rSYS_CLK_PERIOD     = 8;//8 分频
             SG_CPM->rSYS_CLK_SWITCH_EN  = 1;
        }
        ((void (*)())(queue->tDiTask.dLba))();//G0 传入的BOOT __start地址
#endif
    }
    else if(queue->tDiTask.bOpcode == OP_DI_WAIT4_FTL_INIT_DONE)
    {
        ERROR_PRINT("OP_DI_WAIT4_FTL_INIT_DONE........,__get_pfm_count()=%d\r\n",__get_pfm_count());
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
    else if(queue->tDiTask.bOpcode == OP_DI_DATA || queue->tDiTask.bOpcode == OP_DI_PIO_DATA || queue->tDiTask.bOpcode == OP_DI_DMA_DATA)
    {
        DEBUG_PRINT("dLba=%x,dSctSize=%x,bDir=%x,dBuffAddr=%x\r\n",queue->tDiTask.dLba, queue->tDiTask.dSctSize, queue->tDiTask.bDir,queue->tDiTask.dBuffAddr);
        if(queue->tDiTask.bOpcode == OP_DI_DATA)
        {
            bTaskType = E_TASK_DI_LOG;
        }
        LgRdWrCtrl(queue->tDiTask.dLba, queue->tDiTask.dSctSize, queue->tDiTask.bDir);
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
    else if(queue->tDiTask.bOpcode == OP_DI_ERASE_UNIT)
    {
        //擦除全盘数据
        //queue->tDiTask.bDir Erase Mode
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
    else if(queue->tDiTask.bOpcode == OP_DI_SANITIZE_DEVICE)
    {
        //擦除对应的目标区域,输入参数：
        /*
            queue->tDiTask.dLba//LBA
            queue->tDiTask.dSctSize//SECTOR SIZE
        */
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
    else if(queue->tDiTask.bOpcode == OP_DI_FLUSH)
    {
            SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
#ifdef __DUAL_CORE__
    else if(queue->tDiTask.bOpcode == OP_DI_GET_ROM)
    {
        GetRomData(queue->tDiTask.dLba,(U8*)queue->tDiTask.dBuffAddr,queue->tDiTask.dSctSize);
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
    else if(queue->tDiTask.bOpcode == OP_DI_DUAL_SCSI_INT || queue->tDiTask.bOpcode == OP_DI_DUAL_SCSI_IDLE)
    {
        if(queue->tDiTask.dLba != 0)
        {
            queue->tDiTask.eStatus=((U8 (*)(U8))(queue->tDiTask.dLba))(queue->tDiTask.bDir);
        }
        else
        {
            queue->tDiTask.eStatus=FALSE;
        }
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }
#endif
    else
    {
        //未知任务，直接完成
        SG_MCU->rSF1_INT1_EN1=3;//任务完成产生中断通知SATA
    }


}
void MultiSctTranfer(U32 dTotalSec,U8 bDir,U8 bCmdCnt)
{
    U16 wTmpSec;
    if (0 == dTotalSec)
    {
        return;
    }
    SG_DSATA->rNCQ_REORDER_TOTAL.dAll = bCmdCnt;
    SG_DSATA->rNCQ_CMD_ST.dAll=1;
    do
    {
        DEBUG_PRINT("dma1 start!!\r\n");
        if(dTotalSec>=(tSataDC.tSataMI.dDmaBuffSize>>9))
        {
              wTmpSec=(tSataDC.tSataMI.dDmaBuffSize>>9);
        }
        else
        {
              wTmpSec=dTotalSec;
        }
        UART_PRINT("dma1st\r\n");
        //DEBUG_PRINT("dma1 st!!\r\n");
        if(SG_DSATA->rFILL_INVLDA_ST.dAll == 0)
        {
            DMAC_DMA1Start(bDir,tSataDC.tSataMI.dDmaBuffAddr,wTmpSec,DMA1_TOTAL_BUFF_SIZE,0);
            DMAC_DMA1WaitDone();
            if(SG_DSATA->rFILL_INVLDA_ST.dAll == 1)
            {
                //异常处理
                SG_DSATA->rFILL_INVLDA_ST.dAll = 0;
            }
        }

        dTotalSec-=wTmpSec;
        UART_PRINT("dma1done\r\n");
        //DEBUG_PRINT("dma1 done!!\r\n");
    }while(dTotalSec>0);
}

void INTF_Hi_NoneDataTaskProcess (T_TASK_QUEUE_CTX *queue)
{
    SG_MCU->rSF1_INT1_STS0_SET=1;//记录启动的任务
    //非NCQ cmd 所有的参数都在SLOT0
    //这里处理NONE-DATA cmd
#ifdef __NCQ_QUEUE_NEW__
    NCQ_QueuePop(0xFF);
#endif
    if(queue->tHiTasks[0].bOpcode == OP_HI_TRIM)
    {
#ifdef __NCQ_QUEUE_NEW__
    	U8 bTag = queue->tHiTasks[0].tParam.bTag;
        NCQ_CmdReorderQueue(&bTag,1);
#endif
        //RAMDISK 将TRIM 操作当作普通的DMA WRITE 操作
        DMAC_DMA1Start(queue->tHiTasks[0].tParam.bDir,tSataDC.tSataMI.dDmaBuffAddr,queue->tHiTasks[0].tParam.dSctSize,DMA1_TOTAL_BUFF_SIZE,0);
        DMAC_DMA1WaitDone();

        U64 *pentrys=(U64*)tSataDC.tSataMI.dDmaBuffAddr;
        //检查 参数有效性
        U16 I;
        U64 lba;
        U16 len;
        for(I=0;I<queue->tHiTasks[0].tParam.dSctSize * 64;I++)
        {
            lba = (pentrys[I] & 0xFFFFFFFFFFFF);
            len = ((pentrys[I] >> 48) & 0xFFFF);
            if((lba + len) > uCapacity)
            {
                queue->tHiTasks[0].eStatus = CMD_ABORT;
                break;
            }
            //do trim operation
        }
        ERROR_PRINT("queue->tHiTasks[0].bOpcode == OP_HI_TRIM\r\n");
        //TRIM 数据收下来的后续处理
        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
    else if(queue->tHiTasks[0].bOpcode == OP_HI_FLUSH)
    {

        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
    else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_IDLE)
    {
        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
    else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_ACTIVE)
    {
        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
    else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_STANBY)
    {
        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
    else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_SLEEP)
    {
        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
    else
    {
        //未知任务，直接完成
        SG_MCU->rSF1_INT1_EN0=1;//任务完成产生中断通知SATA
        return;
    }
}
//内存拷贝的动作，需要在前一次DMA2操作结束之后，后续利用该接口需要注意
void INTF_Di_DataProcess(T_TASK_QUEUE_CTX *queue, U32 dBufAddr)
{
    if(SATALOG_WR == queue->tDiTask.bDir)
    {
        U08MemCopy((U8*)dBufAddr,(U8*)queue->tDiTask.dBuffAddr,queue->tDiTask.dSctSize*SECTOR_SIZE);
    }
    else
    {
        U08MemCopy((U8*)queue->tDiTask.dBuffAddr,(U8*)dBufAddr,queue->tDiTask.dSctSize*SECTOR_SIZE);
    }
}

void INTF_Hi_DataTaskProcess (T_TASK_QUEUE_CTX *queue)
{
        FTL_NCQCmdHandle();
}


