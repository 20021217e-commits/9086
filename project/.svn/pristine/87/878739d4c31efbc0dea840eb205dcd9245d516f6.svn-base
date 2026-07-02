// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    task_process.c 
//  文件标识：    
//  内容摘要：    模拟实现任务处理
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2015年08月19日 
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************
#include "sata_typedef.h"
#include "dsata.h"
#include "task_manage.h"
#include "task_process.h"
#include "debug.h"
#include "sata_misc.h"
#include "dma1c.h"
#include "mcu.h"
#include "ata_general.h"
#if (SATA_LOG_ENABLE == 1)
#include "ata_gpl.h"
#include "ata_general.h"
#endif
#include "ata_identify.h"
#include "uitility.h"
#include "scsi.h"
#ifdef  __FW_TRIGGER_SATA_CRC__
#include "SATA_AbormalHandle.h"
#endif

// *****************************************************************************
// 函数名称: __getlock
// 函数功能: 获取操作锁
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void __getlock(void)
{
#ifndef __DUAL_CORE__
    //单核
    NVIC_DisableIRQ(IRQ_SC);
#else
    //双核
    tSataDC.bEventLock=TRUE;
#endif
#if (L_PWN_MNG_EN == TRUE)
    //有DI任务，预期在高速时钟进行
    if(SG_CPM->rSYS_CLK_SOURCE_SEL == 1)
    {
        SystemClkRecovery();
    }
#endif
}
BOOL __wait4unlocked(void)
{
#ifdef __DUAL_CORE__
    U32 Tstart=tSataDC.dPowerOnTime;
    while(tSataDC.bEventLock == TRUE)
    {
        if((tSataDC.dPowerOnTime - Tstart) >= 60000)
        {
            //DI任务超时清除状态
            SG_MCU->rSF1_INT1_EN1=0;
            SG_MCU->rSF1_INT1_STS1_CLR =SG_MCU->rSF1_INT1_STS1;
            SG_MCU->rSF0_INT1_STS1_CLR= SG_MCU->rSF0_INT1_STS1;
            //PRINT_TXT("E:TO,W4DI");//wait for DI TASK done 60S
            //卡住以后，一直打印会冲掉串口buff。注释掉以免发生上述情况
#ifdef  __DEBUG_WHILE
            while(1);
#endif
            return FALSE;//60S超时
        }
        DSATA_G3_TaskRouter();

        if(tSataDC.dIsNDCTOGuardEn)
		{
			if((tSataDC.dPowerOnTime - tSataDC.dNDCTOGuardTimer) > 2500)
			{
				//NONE DATA 命令超时时间只有3S，所以在2.5S就要回复D2H状态,目前只有setfeature cmd才有
				ATACmd_ReleaseCommand(CMD_IDLE);//提前回复D2H，但是要注意后续不要再回复了！！
				PRINT_TXT("NODATA,TO");
				tSataDC.dIsNDCTOGuardEn = FALSE;
				return FALSE;//提前做超时处理
			}
		}
    }
    return TRUE;
#else
    return TRUE;
#endif
}
/******************************接口定义******************************/
// *****************************************************************************
// 函数名称: __unlock
// 函数功能: 解操作锁
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void __unlock(void)
{
#ifndef __DUAL_CORE__
    //单核
    #if(SATA_LOG_ENABLE == 0)
        NVIC_EnableIRQ(IRQ_SC);
    #else
        if(tSataDC.tAsyncEvent.pfEventCompleteCb != ReadGPLLogsRouter)
        {
             NVIC_EnableIRQ(IRQ_SC);
        }
    #endif
#else
    //双核
    tSataDC.bEventLock=FALSE;
#endif
}

// *****************************************************************************
// 函数名称: TPM_Init
// 函数功能: FTL初始化函数
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void  TPM_Init(void)
{
    SG_MCU->rSF1_INT1_STS0_CLR=0xFFFFFFFF;//STS0作为删除任务寄存器
    SG_MCU->rSF1_INT1_EN0=0x0;//FTL开始执行任务之前使能
    SG_MCU->rSF1_INT1_STS1_CLR=0xFFFFFFFF;//STS1暂时未使用，高出32个任务的时候才会用得到
    SG_MCU->rSF1_INT1_EN1=0x0;
    SG_MCU->rSF1_INT0_STS.dAll=0;
    SG_MCU->rSF1_INT_EN.dAll=0x03;//使能INT0 和INT1

    //DI任务标记只需要上电初始化一下
    SG_MCU->rSF0_INT1_STS1_CLR=0xFFFFFFFF;//STS1用作DI任务标记
    SG_MCU->rSF0_INT1_EN1=0x0;//禁用相应中断

//#ifndef __G3_FLYCODE
//    NVIC_EnableIRQ(IRQ_SF0_INT0);
//    NVIC_EnableIRQ(IRQ_SF0_INT1);
//#else
//    NVIC_DisableIRQ(IRQ_SF0_INT0);
//    NVIC_DisableIRQ(IRQ_SF0_INT1);
//#endif
}
// *****************************************************************************
// 函数名称: TPM_IntEvent2Sata
// 函数功能: 通知SATA层FTL事务完结，发送中断给SATA
// 入口参数: 完成的任务槽号
// 出口参数: 
// 说    明: 
// *****************************************************************************
void TPM_IntEvent2Sata(U8 bSlot)
{
    //产生中断信号给SATA模块
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: TPM_GetTask
// 函数功能: 获取任务
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
T_HI_TASK * TPM_GetTask(void)
{ 
    U32 dVal;

    dVal=(SG_MCU->rSF0_INT1_STS0);

    if(dVal != 0)
    {
        return &tTQC.tHiTasks[0];
    }
    else
    {  
        //这种做法可能导致HOST在频繁写数据的时候LOG没有机会更新
        TPM_SyncEventFromSata();//HOST 没有任务的时候才需要执行设备发起的任务
        return NULL;
    }

}
#if(CMD_REORDER_EN == TRUE)
void TPM_MultiSctTranfer(U32 dTotalSec,U8 bDir,U8 bCmdCnt)
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
        DMAC_DMA1Start(bDir,tSataDC.tSataMI.dDmaBuffAddr,wTmpSec,DMA1_TOTAL_BUFF_SIZE,0);
        DMAC_DMA1WaitDone();
        dTotalSec-=wTmpSec;
        UART_PRINT("dma1done\r\n");
        //DEBUG_PRINT("dma1 done!!\r\n");
    }while(dTotalSec>0);
}
#endif


#ifndef __DUAL_CORE__
#ifdef __NCQ_QUEUE_NEW__
BOOL TaskQueuedNewCmdInQuire(T_TASK_QUEUE_CTX *queue,U8 init_dir,U16 wperiod_us)
{
    __enable_performance();
    while(__get_pfm_count() <= wperiod_us * tSataDC.dMicroSecondCnt)
    {
        if(SG_MCU->rSF0_INT1_STS1 != 0 )
        {
            //有DI任务插入
            PRINT("33333333333__get_pfm_count=%d\r\n",__get_pfm_count() );
            return FALSE;
        }

        if(NCQ_GetQueueLen() != 0)
        {
            if(queue->tHiTasks[0].tParam.bIsUsed && queue->tHiTasks[0].bOpcode != OP_HI_DATA)
            {
                //有新的非数据命令插入
//                PRINT("none data cmd..\r\n");
                return FALSE;
            }
            else
            {
                if(!queue->bIsNcqCmd)
                {
                    //等待过程中收到非NCQ命令,NCQ命令处理完成,直接退出
//                    PRINT("none ncq  cmd..\r\n");
                    return FALSE;
                }

                if(NCQ_QueueInquire()->tParam.bDir==init_dir)
                {
                    //PRINT_DEC("CP0=",__get_pfm_count());
                    return TRUE;
                }
                else
                {
//                    PRINT("dir change...\r\n");
                    return FALSE;
                }
            }
        }

        //TODO:add anyother exit condition
    }
    //等待超时
    return FALSE;
}

void TaskQueuedDummyLgRdWrCtrl(U32 dLBA, U32 dSectCnt, U8 bDir)
{

    U16 wTmpSec;
#ifndef  _SATA_512K_RAM_SCAN_
    do
    {
        DEBUG_PRINT("dma1 start!!\r\n");
        if(dSectCnt>=(tSataDC.tSataMI.dDmaBuffSize>>9))
        {
            wTmpSec=(tSataDC.tSataMI.dDmaBuffSize>>9);
        }
        else
        {
            wTmpSec=dSectCnt;
        }

#if    TEST_DMA1_MAX_EN

        if (wTmpSec > TEST_DMA1_MAXSctSize )
        {
        	wTmpSec = TEST_DMA1_MAXSctSize;
        }

#endif   // TEST_DMA1_MAX_EN
#ifdef __PRIVATE_CRC_HANDLE__
        //for WRITE SCSI CRC MONITOR
        extern T_SCSI_CTRL    tScsiCtl;
        *(((U32*)tSataDC.tSataMI.dDmaBuffAddr) + 64) = tScsiCtl.dWrValidFlg;
#endif
        DMAC_DMA1Start(bDir,tSataDC.tSataMI.dDmaBuffAddr,wTmpSec,DMA1_TOTAL_BUFF_SIZE,0);
        DMAC_DMA1WaitDone();
        dSectCnt-=wTmpSec;
    }
    while(dSectCnt>0);

#else   //512K 的RAMDisk 盘扫描
    U32 dAddrs = tSataDC.tSataMI.dDmaBuffAddr;
    U8 bSect = 0;
    if((dLBA>0x1000) &&(dLBA>0x1400))
    {
    	    wTmpSec=dSectCnt;
            dAddrs = dAddrs + (dLBA - 0x1000);
            DMAC_DMA1Start(bDir,dAddrs,wTmpSec,DMA1_TOTAL_BUFF_SIZE,0);
            DMAC_DMA1WaitDone();
    }
    else
    {
        do
        {
            if(dSectCnt>=(tSataDC.tSataMI.dDmaBuffSize >> 11))
            {
                wTmpSec=(tSataDC.tSataMI.dDmaBuffSize >> 11);  //512K
            }
            else
            {
                wTmpSec=dSectCnt;
            }

    #if    TEST_DMA1_MAX_EN

            if (wTmpSec > TEST_DMA1_MAXSctSize )
            {
            	wTmpSec = TEST_DMA1_MAXSctSize;
            }

    #endif   // TEST_DMA1_MAX_EN
    #ifdef __PRIVATE_CRC_HANDLE__
            //for WRITE SCSI CRC MONITOR
            extern T_SCSI_CTRL    tScsiCtl;
            *(((U32*)tSataDC.tSataMI.dDmaBuffAddr) + 64) = tScsiCtl.dWrValidFlg;
    #endif
            DMAC_DMA1Start(bDir,dAddrs,wTmpSec,DMA1_TOTAL_BUFF_SIZE,0);
            DMAC_DMA1WaitDone();

            dSectCnt-=wTmpSec;
        }
        while(dSectCnt>0);
    }

#endif
}

void TaskQueuedDummyRW_Process()
{
    //TODO:解析LBA之间的关系,甚至参考物理地址的关系,重排序执行顺序
    U32 dTotalSec=0;
    U32 dLba;
    U8  bDir,bCmdCnt,bCmdType;
    U8 bCmdTags[32];

    //PRINT("queue->m_bQueLen:%d\n",queue->m_bQueLen);

    bDir=NCQ_QueueInquire()->tParam.bDir;
    //tSataDC.wNcqTOCnt=4;
    TEST_PRINT(".........................bDir =%x\n",bDir);
    
//#ifdef __PRIVATE_CRC_HANDLE__
//    Abnormal_Sata_ClearStFlag();
//#endif

#if  CRC_TEST
    Abnormal_OneTimeOccur();
#endif  //

    do
    {
        bCmdCnt=0;
        dTotalSec=0;

        bCmdCnt=NCQ_QueueMulPop(bDir,bCmdTags,&dTotalSec,&bCmdType,32);

        if(bCmdCnt )
        {
            dLba=NCQ_GetParam(bCmdTags[0])->tParam.dLba;

    #if (_FDL_EFFIC_TEST_ == 1)
            LgRdWrEfficTest(dLba, dTotalSec, bDir);
    #endif
//            PRINT("dTotalSec = 0x%x\r\n",dTotalSec);
#ifdef _SATA_TRIGGER_CRC__
//            Abnormal_Sata_Ctrl(bCmdTags,bCmdCnt);
#endif
            NCQ_CmdReorderQueue(bCmdTags,bCmdCnt);
            TaskQueuedDummyLgRdWrCtrl(dLba, dTotalSec, bDir);
#ifdef _SATA_TRIGGER_CRC__
//            Abnormal_SataCheck();
#endif
            TEST_PRINT("Pro2Pass_idx = %d,min = %d,Flg = %d\r\n",SG_DSATA->rCRC_PASS_INDEX,SG_DSATA->rCRC_PASS_MIN,SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL);
//            PRINT("Pro2Pass_idx = 0x%x,bCmdCnt = %d\r\n",SG_DSATA->rCRC_PASS_INDEX,bCmdCnt);

            //PRINT_DEC("C=",bCmdCnt);
            //PRINT_DEC("S=",dTotalSec);
//            delay_us(100);
            //注意：demo只验证NCQ32性能，NCQ1 性能要额外去掉打印和delay

            if(SG_DSATA->rFILL_INVLDA_ST.dAll == 1)
            {
             //异常处理
             CODE_PRINT("ERROR:dLba:0x%x,i=%d\n",dLba,i);
             SG_DSATA->rERRMSK_CLR.bits.dVAL=1;
             SG_DSATA->rFILL_INVLDA_ST.dAll = 0;
             while(SG_MCU->rSF0_INT1_STS0 != 0);
             return;
            }
        }
    }while(TaskQueuedNewCmdInQuire(&tTQC,bDir,10000));
}
#endif

void TaskQueuedEventDefaultHandle (T_TASK_QUEUE_CTX *queue)
{
    U32 dVal;

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    /***********************************DI 任务处理***********************************/
    dVal=SG_MCU->rSF0_INT1_STS1;
    if(dVal!=0)
    {
        SG_MCU->rSF1_INT1_EN1=0;//保证完成任务以后再产生中断
        SG_MCU->rSF1_INT1_STS1_SET =dVal;//记录准备处理的任务
        SG_MCU->rSF0_INT1_STS1_CLR =dVal;//清除准备处理的任务
        //有DI 任务,处理DI任务
        if(queue->tDiTask.bOpcode == OP_DI_SWITCHCODE)
        {
#ifdef __DUAL_CORE__
            //代码切换
            ((void (*)())(queue->tDiTask.dLba))();
#endif
        }
#if (SATA_LOG_ENABLE == 1)
        else if(queue->tDiTask.bOpcode == OP_DI_WAIT4_FTL_INIT_DONE)
        {
            SG_MCU->rSF1_INT1_EN1=dVal;//任务完成产生中断通知SATA
        }
        else if(queue->tDiTask.bOpcode == OP_DI_DATA)
        {
            SG_MCU->rSF1_INT1_EN1=dVal;//任务完成产生中断通知SATA
        }
#endif
        else
        {
            //未知任务，直接完成
            SG_MCU->rSF1_INT1_EN1=dVal;//任务完成产生中断通知SATA
        }
    }


    /********************************HI 任务处理********************************/
    dVal=SG_MCU->rSF0_INT1_STS0;
    if(dVal!=0)
    {
        //有HI 任务
        SG_MCU->rSF1_INT1_EN0=0;//保证完成任务以后再产生中断

#ifdef        SC_WP_4K_PRE
        if((queue->bIsNcqCmd == FALSE) && (FALSE == SG_DSATA->rPRE_TRIG_CNT ))//如果优先有 预收命令，优先处理预收命令
#else
        if(queue->bIsNcqCmd == FALSE)
#endif
        {
            //非NCQ cmd 所有的参数都在SLOT0
            //这里处理NONE-DATA cmd
    		SG_MCU->rSF0_INT1_STS0_CLR = dVal;//清除准备处理的任务

            if(queue->tHiTasks[0].bOpcode != OP_HI_DATA)
            {
#ifdef __NCQ_QUEUE_NEW__
                 NCQ_QueuePop(0xFF);
#endif
                 SG_MCU->rSF1_INT1_STS0_SET=1;//记录启动的任务
            }

            if(queue->tHiTasks[0].bOpcode == OP_HI_DATA)
            {

            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_TRIM)
            {
                //RAMDISK 将TRIM 操作当作普通的DMA WRITE 操作
                DMAC_DMA1Start(queue->tHiTasks[0].tParam.bDir,tSataDC.tSataMI.dDmaBuffAddr,queue->tHiTasks[0].tParam.dSctSize,DMA1_TOTAL_BUFF_SIZE,0);
                DMAC_DMA1WaitDone();
                //TRIM 数据收下来的后续处理
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_FLUSH)
            {
                //FLUSH
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_IDFY)
            {
                if(queue->tHiTasks[0].tParam.bDir == DIR_IN)
                {
                    //如果没有使能SATALOG,就从UDP 中获取
                    Mem32Copy((U32*)tTQC.tHiTasks[0].tParam.dBuffAddr,(U32*)unAtaIdentify_C.dwArray,128);
                }
                else
                {
                    //有些IDENTIFY 的内容是需要掉电保存的，
                    //要利用COMBUFFER 先读出来，再修改需要保存的项目，再写到NANDFLASH
                    //写IDENTIFY
                }
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_IDLE)
            {
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_ACTIVE)
            {
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_STANBY)
            {
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else if(queue->tHiTasks[0].bOpcode == OP_HI_DPM_SLEEP)
            {
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
            else
            {
                //未知任务，直接完成
                SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
                return;
            }
        }

        //直接跑到下面执行数据任务
#ifndef __BOOT_CODE__
        TaskQueuedDummyRW_Process();
#else
#if 1
        /************************下面处理的是OP_HI_DATA 处理**********************/
#if(CMD_REORDER_EN == FALSE)
        int i;
#if (SDB_AUTO_SEND==FALSE)
        if(tSataDC.bIsNcqCmd == TRUE)
        {
            for(i=MAX_CMD_NUM;i>0;i--)
            {
                if((SG_MCU->rSF1_INT1_STS0 &(1<<(i-1))) != 0)
                {
                    break;
                }
            }

            //最后执行的SLOT号PRD置1产生中断
            tPrdTbls[(i-1)].dw3.bits.I=1; //TODO 具体的情况需要根据REORDER具体配置
        }
#endif
        for(i=0;i<MAX_CMD_NUM;i++)
        {
            //处理所有OP_HI_DATA 任务
            if((dVal & (1<<i)) != 0)
            {
                //对应slot有任务
                if(queue->bIsNcqCmd == TRUE)
                {
                    //ATA层发起的数据传输，直接启动PRD传输
                    DSATA_TransferStart(i);
                }


                U16 wTmpSec;
                U32 dTotalSec=queue->tHiTasks[i].tParam.dSctSize;
#if (TXRX_CRC_ERROR == TRUE)
                SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM=5;//(((dTotalSec*512)-1)>>13)+1;
                SG_DSATA->rTRXCRC.bits.ERR_TAG=i;

                SG_DSATA->rTRXCRC.bits.TXCRC =queue->tHiTasks[i].tParam.bDir;
                SG_DSATA->rTRXCRC.bits.RXCRC =!queue->tHiTasks[i].tParam.bDir;
                SG_DSATA->rSTCRCE.dAll=1;
#endif

                do{
                      DEBUG_PRINT("dma1 start!!\r\n");
                      if(dTotalSec>=(tSataDC.tSataMI.dDmaBuffSize>>9))
                      {
                          wTmpSec=(tSataDC.tSataMI.dDmaBuffSize>>9);
                      }
                      else
                      {
                           wTmpSec=dTotalSec;
                      }
                      GPIO_DEBUG(0x18);

                      DMAC_DMA1Start(queue->tHiTasks[i].tParam.bDir,tSataDC.tSataMI.dDmaBuffAddr,wTmpSec,DMA1_TOTAL_BUFF_SIZE,0);
                      DMAC_DMA1WaitDone();
                      GPIO_DEBUG(0x19);
                      dTotalSec-=wTmpSec;
                      DEBUG_PRINT("dma1 done!!\r\n");
                   }while(dTotalSec>0);
            }
        }
#else
        U32 dTotalSec;
        U8  bCnt=0;
        U8  i;
        U8  bDir=0xFF;

        SG_DSATA->rNCQ_REORDER_EN.dAll = 0;
        SG_DSATA->rNCQ_REORDER_TOTAL.dAll         = 0;//写操作会触发启动，所以要在使能之前清除

        SG_DSATA->rNCQ_REORDER_EN.dAll = 1;//初始化操作要在使能操作之前
        dTotalSec = 0;
        for(i=0;i<MAX_CMD_NUM;i++)
        {
            if(queue->tHiTasks[i].tParam.bIsUsed)
            {
                if ((bDir != queue->tHiTasks[i].tParam.bDir)&&(0xFF != bDir))
                {
                    TPM_MultiSctTranfer(dTotalSec,bDir,bCnt);
                    dTotalSec = 0;
                    bCnt = 0;
                }
                bDir       = queue->tHiTasks[i].tParam.bDir;
                dTotalSec += queue->tHiTasks[i].tParam.dSctSize;
                SG_DSATA->rNCQ_REORDER_REG[bCnt++] = i;//tTQC.tHiTasks[i].tParam.bTag;
            }
        }
        TPM_MultiSctTranfer(dTotalSec,bDir,bCnt);
        SG_DSATA->rNCQ_REORDER_EN.dAll = 0;
#endif
#ifdef __PRIVATE_CRC_HANDLE__
        extern void Abnormal_Sata_ClearStFlag(void);
        Abnormal_Sata_ClearStFlag();
#endif
        SG_MCU->rSF1_INT1_EN0=dVal;//任务完成产生中断通知SATA
#endif
#endif
    }

}
#endif
// *****************************************************************************
// 函数名称: TPM_IntEventFromSata
// 函数功能: SATA 紧急事件处理接口
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void TPM_IntEventFromSata(void)
{ 
 //中断中处理同步事件，事件发生必须及时处理
}

// *****************************************************************************
// 函数名称: TPM_AsyncEventFromSata
// 函数功能: SATA 异步事件处理接口,可延迟处理的事件
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void TPM_AsyncEventFromSata(void)
{
    //主循环中处理异步事件，事件发生后可以延迟处理
    switch(tSataDC.tAsyncEvent.bOpcode)
    {
        case OP_DI_WAIT4_FTL_INIT_DONE:
            //异步任务完成通知SATA
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_WAIT4_FTL_INIT_DONE;
#if (SATA_LOG_ENABLE == 0)
            tSataDC.tAsyncEvent.bOpcode=OP_NONE_EVENT;
#endif
            SG_MCU->rSF0_INT1_STS1_SET=2;//添加事物标记
#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);//调用FTL 写数据
            }
#else
             __getlock();
             __wait4unlocked();//等待任务完成中断 --如果不等的话会出现量产工具读版本号冲突的问题
             //__unlock();
             NVIC_EnableIRQ(IRQ_CPM);//g3初始化完成再跑VDT事件

#endif

#if (SATA_LOG_ENABLE == 0)
             Mem32Copy((U32*)unAtaIdentify.dwArray,(U32*)unAtaIdentify_C.dwArray,sizeof(unAtaIdentify)>>2);
             if(tSataDC.tSataMI.dVirtualLbaStartAddr == 0)
             {
                 tSataDC.tSataMI.dVirtualLbaStartAddr=DEVICE_CAPCITY;
             }
             unAtaIdentify.wArray[60] =tSataDC.tSataMI.dVirtualLbaStartAddr&0xFFFF;
             unAtaIdentify.wArray[61] =(tSataDC.tSataMI.dVirtualLbaStartAddr>>16)&0xFFFF;;
             unAtaIdentify.wArray[100] =tSataDC.tSataMI.dVirtualLbaStartAddr&0xFFFF;;
             unAtaIdentify.wArray[101] =(tSataDC.tSataMI.dVirtualLbaStartAddr>>16)&0xFFFF;
             unAtaIdentify.wArray[102] =0;
             unAtaIdentify.wArray[103] =0;

#ifdef SCAN_DEVICE_CAPACITY
		     unAtaIdentify.wArray[60] =0xFFFF;
		     unAtaIdentify.wArray[61] =0xFFF;
		     unAtaIdentify.wArray[100] =0;
		     unAtaIdentify.wArray[101] =0x4000;//512G
		     unAtaIdentify.wArray[102] =0;
		     unAtaIdentify.wArray[103] =0;
		     Update_IdfyParam();
#endif

             Update_IdfyParam();
             NVIC_EnableIRQ(IRQ_SC);//开启了SATA LOG 以后等到，读完相关log之后再开启SATA通信
#endif

            break;
        case OP_DI_DATA:
#ifndef __BOOT_CODE__
            //UART_PRINT("Enter func=<%s>[file=%s,line=%d] OP_DI_DATA\r\n",__func__,__MODULE__,__LINE__);
            //准备取任务，以及执行任务的时候需要屏蔽SATA 中断。防止参数被中途修改（如果来合法的HOST 读写请求呢？）
            GIE_DISABLE();
            //获取EVENT参数
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_DATA;
            tTQC.tDiTask.bDir =tSataDC.tAsyncEvent.dParam[0];
            tTQC.tDiTask.dLba =tSataDC.tAsyncEvent.dParam[1];
            tTQC.tDiTask.dSctSize =tSataDC.tAsyncEvent.dParam[2];
            tTQC.tDiTask.dBuffAddr =tSataDC.tAsyncEvent.dParam[3];
            GIE_ENABLE();
            SG_MCU->rSF0_INT1_STS1_SET =2;//ADD DI Task
#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);//调用FTL 写数据
            }
#else
            __getlock();
            __wait4unlocked();//等待任务完成中断--如果不等的话会出现量产工具读版本号冲突的问题
            //__unlock();
#endif
            //异步任务完成通知SATA
            break;
        case OP_DI_FLUSH:
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_FLUSH;
            SG_MCU->rSF0_INT1_STS1_SET=2;//添加事物标记
#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);
            }
#else
            __getlock();
            __wait4unlocked();//等待任务完成中断--如果不等的话会出现量产工具读版本号冲突的问题
            //__unlock();
#endif
#endif
            break;
        case OP_NONE_EVENT:
            //任务被打断，却没有重新开始
            break;
        default:
            break;
    }

}

// *****************************************************************************
// 函数名称: TPM_SyncEventFromSata
// 函数功能: SATA 同步事件处理接口,可延迟处理的事件
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void TPM_SyncEventFromSata(void)
{
    //主循环中处理同步事件，事件发生后可以延迟处理
    switch(tSataDC.tSyncEvent.bOpcode)
    {
        case OP_DI_DMA_DATA:
        case OP_DI_PIO_DATA:
        case OP_DI_DATA:
            //处理任务
            //准备取任务，同步事件执行期间 HOST 不会下发新的任务，不存在嵌套可能
            //同步事件完成通知SATA

            //UART_PRINT("Enter func=<%s>[file=%s,line=%d] OP_DI_DATA\r\n",__func__,__MODULE__,__LINE__);
            //准备取任务，以及执行任务的时候需要屏蔽SATA 中断。防止参数被中途修改（如果来合法的HOST 读写请求呢？）
        {
#ifndef __BOOT_CODE__
            U8 opcode=tSataDC.tSyncEvent.bOpcode;
            GIE_DISABLE();
            if(opcode == OP_DI_PIO_DATA)
            {
                //PIO模式可能存在连续的读写过程，不禁用SATA中断的话会导致PIO写提前发出D2H
                PRINT_TXT("PIODI_S");
                NVIC_DisableIRQ(IRQ_SC);
                tSataDC.tSyncEvent.eStatus = CMD_PENDING;//标记准备执行状态-ONGOING是初始化状态
            }
            //获取EVENT参数
            DEBUG_PRINT("create di task!!!!\r\n");
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=tSataDC.tSyncEvent.bOpcode;
            tTQC.tDiTask.bDir =tSataDC.tSyncEvent.dParam[0];
            tTQC.tDiTask.dLba =tSataDC.tSyncEvent.dParam[1];
            tTQC.tDiTask.dSctSize =tSataDC.tSyncEvent.dParam[2];
            tTQC.tDiTask.dBuffAddr =tSataDC.tSyncEvent.dParam[3];
            GIE_ENABLE();
            SG_MCU->rSF0_INT1_STS1_SET=1;//添加事物标记

#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);//调用FTL 写数据
            }
#else
            __getlock();

            if(__wait4unlocked() == FALSE)//等待任务完成中断--如果不等的话会出现量产工具读版本号冲突的问题
            {
                //超时
            	GIE_DISABLE();
                tSataDC.tSyncEvent.bOpcode =OP_NONE_EVENT;//以免重复发起任务
                tSataDC.tSyncEvent.pfEventCompleteCb=NULL;
                GIE_ENABLE();
            }

            //__unlock();//中断里面unlock
            if(opcode == OP_DI_PIO_DATA)
            {
                PRINT_TXT("PIODI_E");
                NVIC_EnableIRQ(IRQ_SC);
            }
#endif
#endif
        }
            break;
#ifndef __BOOT_CODE__
        case OP_DI_SANITIZE_DEVICE:

            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_SANITIZE_DEVICE;
            tTQC.tDiTask.dLba=tSataDC.tSyncEvent.dParam[1];
            tTQC.tDiTask.dSctSize=tSataDC.tSyncEvent.dParam[2];
            SG_MCU->rSF0_INT1_STS1_SET=1;//添加事物标记

#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);
            }
#else
            __getlock();
            __wait4unlocked();//等待任务完成中断--如果不等的话会出现量产工具读版本号冲突的问题
#endif
//
            break;
        case OP_DI_STANBY_REQ:
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_STANBY_REQ;
            SG_MCU->rSF0_INT1_STS1_SET=1;//添加事物标记

#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);
            }
#else
            __getlock();
            __wait4unlocked();//等待任务完成中断--如果不等的话会出现量产工具读版本号冲突的问题
#endif
//           __unlock();
            break;
        case OP_DI_ERASE_UNIT://擦除全盘数据

            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_ERASE_UNIT;
            tTQC.tDiTask.bDir =tSataDC.tSyncEvent.dParam[0];
            SG_MCU->rSF0_INT1_STS1_SET=1;//添加事物标记
#ifndef __DUAL_CORE__
            if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
            {
                tTQC.enTaskType=E_DI_TASK;
                tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);
            }
#else
            __getlock();
            __wait4unlocked();//等待任务完成中断--如果不等的话会出现量产工具读版本号冲突的问题
#endif
#endif
//           __unlock();
            break;
        case OP_NONE_EVENT:
            TPM_AsyncEventFromSata();//异步事件是优先级最低的任务
            break;
        default:
            break;
    }

}

// *****************************************************************************
// 函数名称: TPM_ParseTask
// 函数功能: FTL 任务解析接口
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************
void TPM_ParseTask()
{
#ifndef __DUAL_CORE__
    if(tSataDC.tSataMI.pTaskQueuedEvent_cb != NULL)
    {
        tTQC.enTaskType=E_HI_TASK;
        tSataDC.tSataMI.pTaskQueuedEvent_cb(&tTQC);
    }
#else
    //do nothing --HI task的交互已经在SATA中断中完成
    //置位rSF0_INT1_STS0用以与G3交互
#endif
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

//#pragma GCC optimize("O0")
// *****************************************************************************
// 函数名称: TPM_StateMachine
// 函数功能: 
// 入口参数: 
// 出口参数: 
// 说    明: 
// *****************************************************************************    
void TPM_StateMachine(void)
{
#ifndef __DUAL_CORE__
    //SATA 任务队列任务解析
    //if(SG_MCU->rSF1_INT1_STS0 == 0)
    {
        //当前没有进行中任务或者任务完成取下一个任务
        if(TPM_GetTask() != NULL)
        {
            //找到任务,处理任务
            TPM_ParseTask();
        }
#ifndef __BOOT_CODE__
        else
        {
            if((SG_MCU->rSF0_INT1_STS0 == 0 && SG_MCU->rSF0_INT1_STS1 == 0) &&
                                                     (SG_MCU->rSF1_INT1_STS1 == 0))
            {
                TPM_SyncEventFromSata();//推送设备发起的任务
            }
        }
#endif
    }
#else
    //单核
#if(SATA_LOG_ENABLE == 1)

    if(SG_MCU->rSF0_INT1_STS1 == 0 && SG_MCU->rSF1_INT1_STS1 == 0)
    {
        //没有pending,以及正在执行的DI任务
        TPM_SyncEventFromSata();//推送设备发起的任务
    }
#else

    if((SG_MCU->rSF0_INT1_STS0 == 0 && SG_MCU->rSF0_INT1_STS1 == 0) &&
                                     (SG_MCU->rSF1_INT1_STS1 == 0))
    {
        TPM_SyncEventFromSata();//推送设备发起的任务
    }

#endif

#endif
}
#ifndef __DUAL_CORE__
// *****************************************************************************
// 函数名称: EXTA_INT1_IRQHandler
// 函数功能: 外部中断1服务函数，处理SATA模块添加数据相关任务事件
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SF0_INT1_IRQHandler(void)
{
    //模拟SATA TO FTL 和 SATA TO FTL 添加任务删除任务事件中断
    //UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__); 
//    ERROR_PRINT("SG_MCU->rSF0_INT1_STS0=%#010lx\r\n",SG_MCU->rSF0_INT1_STS0 );
    if(SG_MCU->rSF0_INT1_EN0 != 0)
    {
        SG_MCU->rSF0_INT1_EN0=0;//清除中断

    }
   // UART_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: EXTA_INT0_IRQHandler
// 函数功能: 外部中断0服务函数，处理SATA模块异常事务相关事件
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SF0_INT0_IRQHandler(void)
{
//    UART_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    SG_MCU->rSF0_INT0_STS.dAll=0;//清除中断
    //TPM_IntEventFromSata();
//   UART_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#endif

#if  FAKE_ENGINE


// *****************************************************************************
// 函数名称:
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Fake_engine_ST(U8 bDir, U8 bFlg, U32 dSctcnt, U32 dLba)
{
	SG_DSATA->rFK_WR_DONE.bits.dVAL = 0;
	SG_DSATA->rPIER_EX.bits.dVAL = 0; //  先关中断再清中断标记
	SG_DSATA->rPISR_EX.bits.dVAL = 0; //
	SG_DSATA->rPIER_EX.bits.dVAL = 1; //
	SG_DSATA->rFK_ENG_DIR.bits.dVAL = bDir ; // 1 ：写        0：读
	SG_DSATA->rFK_ENG_CNT = bFlag;        // Flg 的标志
	SG_DSATA->rFK_ENG_SCTCNT = dSctcnt ;  //
	SG_DSATA->rFK_ENG_LBA = dLba;
	SG_DSATA->rFW_EN.bits.FAKE_ENGINE_EN = 1;  //启动
}

// *****************************************************************************
// 函数名称:
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Fake_engine_Write_Check_Done(void)
{
	if (1 == SG_DSATA->rFK_WR_DONE.bits.dVAL)
	{
		SG_DSATA->rFK_WR_DONE.bits.dVAL = 0;
		SG_DSATA->rPIER_EX.bits.dVAL = 0; //
		SG_DSATA->rPISR_EX.bits.dVAL = 0; //
		SG_DSATA->rFK_WR_DONE.bits.dVAL = 0; // 需 清0
		return TRUE;
	}
	return FALSE;
}

// *****************************************************************************
// 函数名称:
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Fake_engine_Write(U8 bFlg, U32 dSctcnt, U32 dLba)
{
	Fake_engine_ST(DIR_IN, bFlg, dSctcnt, dLba);
	while(FALSE == Fake_engine_Write_Check_Done());
}

// *****************************************************************************
// 函数名称:
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void Fake_engine_Read(U8 bFlg, U32 dSctcnt, U32 dLba)
{
	Fake_engine_ST(DIR_IN, bFlg, dSctcnt, dLba);
	DMAC_DMA1WaitDone();   //等DMA1 Done
}



#endif // FAKE_ENGINE


