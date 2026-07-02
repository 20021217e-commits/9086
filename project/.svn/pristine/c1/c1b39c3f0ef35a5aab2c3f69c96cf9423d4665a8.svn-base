#define  _CMD_HANDLE_CFG_
#include "common.h"
#include "sg9081.h"
#include "flash_driver.h"
#include "uitility.h"
#include "main.h"
#include "CmdHandle.h"
#include "Global.h"
#include "dma1c.h"
#include "debug.h"
#include "ftl.h"

extern void TMR_T1_Init(U32 dMicrosec,U32 dMicrosecondUnit);

void MultiSctTranfer(U32 dTotalSec,U8 bDir,U8 bCmdCnt);

#define bCmdTag g_tCmdQueInfo.m_bTag

void TaskQueuedSqueHandle(T_CMD_QUE_INFO *queue)
{
    //TODO:解析LBA之间的关系,甚至参考物理地址的关系,重排序执行顺序
    U32 dTotalSec=0;
    U32 dLba;
    U8  bDir,bCmdCnt,bCmdType;

    //PRINT("queue->m_bQueLen:%d\n",queue->m_bQueLen);

    bDir=NCQ_QueueInquire()->tParam.bDir;
    //tSataDC.wNcqTOCnt=4;
    //PRINT(".........................bDir =%x\n",bDir);
//    do
    {
        bCmdCnt=0;
        dTotalSec=0;

        bCmdCnt=NCQ_QueueMulPop(bDir,bCmdTag,&dTotalSec,&bCmdType,32);

//        if(bCmdCnt == 0)
//        {
//            continue;
//        }
        dLba=NCQ_GetParam(bCmdTag[0])->tParam.dLba;

        NCQ_CmdReorderQueue(bCmdTag,bCmdCnt);
#if (_FDL_EFFIC_TEST_ == 1)
        LgRdWrEfficTest(dLba, dTotalSec, bDir);
#endif

        LgRdWrCtrl(dLba, dTotalSec, bDir);
        //NCQ_CmdReorderQueue(bCmdTag,bCmdCnt);

        PRINT_DEC("C=",bCmdCnt);
        //PRINT_DEC("S=",dTotalSec);
        delay_us(100);
        //注意：demo只验证NCQ32性能，NCQ1 性能要额外去掉打印和delay

        if(SG_DSATA->rFILL_INVLDA_ST.dAll == 1)
        {
             //异常处理
             CODE_PRINT("ERROR:dLba:0x%x,i=%d\n",dLba,i);
             SG_DSATA->rERRMSK_CLR.bits.dVAL=1;//不进行这个操作的话，后续非联动启动的DMA1数据没有办法写到MEMORY
             SG_DSATA->rFILL_INVLDA_ST.dAll = 0;
             while(SG_MCU->rSF0_INT1_STS0 != 0);
             return;
        }

    }
//	while(FTL_NCQNewCmdInQuire(&tTQC,bDir,10000));
}

void Test_FTL_NCQ_Op(U8 bOpType)				//目前逻辑读未实现，不适用该函数，后续修改
{
#if defined(__TEST_TXBENCH_SPEED_TLCMODE__) || \
	defined(__TEST_TXBENCH_SPEED_SLCMODE__) || \
	defined(__TEST_JUPITER_SPEED_SLCMODE__)

    U8 bDir = NCQ_QueueInquire()->tParam.bDir;

    if(SG_DSATA->rFILL_INVLDA_ST.dAll == 1)
    {
        //异常处理
        CODE_PRINT("ERROR:dLba:0x%x,i=%d\n",dLba,i);
        g_tPendingCmdList.dCmdBitMask = 0;
        g_tPendingCmdList.bCmdCnt	  = 0;
        g_tPendingCmdList.bCmdIndex   = 0;
        SG_DSATA->rERRMSK_CLR.bits.dVAL=1;
        SG_DSATA->rFILL_INVLDA_ST.dAll = 0;
        while(SG_MCU->rSF0_INT1_STS0 != 0);
    }

    if(bDir)
    {
        FTL_LGRead(bDir);
    }
    else
    {
        FTL_LGWrite(bDir);
    }

#else

    TaskQueuedSqueHandle(&g_tCmdQueInfo);		//方向字段内部处理

#endif
}

BOOL FTL_NCQNewCmdInQuire(T_TASK_QUEUE_CTX *queue,U8 init_dir,U16 wperiod_us)
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
                PRINT("none data cmd..\r\n");
                return FALSE;
            }
            else
            {
                if(!queue->bIsNcqCmd)
                {
                    //等待过程中收到非NCQ命令,NCQ命令处理完成,直接退出
                    PRINT("none ncq  cmd..\r\n");
                    return FALSE;
                }

                if(NCQ_QueueInquire()->tParam.bDir==init_dir)
                {
                    //PRINT_DEC("CP0=",__get_pfm_count());
                    return TRUE;
                }
                else
                {
                    PRINT("dir change...\r\n");
                    return FALSE;
                }
            }
        }

        //TODO:add anyother exit condition
    }
    //等待超时
    return FALSE;
}

void DummyLgRdWrCtrl(U32 dLBA, U32 dSectCnt, U8 bDir)

{
    U16 wTmpSec;
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

        HAL_MCStartDMA1(tSataDC.tSataMI.dDmaBuffAddr,0, wTmpSec,!bDir, 0);

        dSectCnt-=wTmpSec;
    }
    while(dSectCnt>0);
}

        void DummyRW_Process()
{
    //TODO:解析LBA之间的关系,甚至参考物理地址的关系,重排序执行顺序
    U32 dTotalSec=0;
    U32 dLba;
    U8  bDir,bCmdCnt,bCmdType;
    U8 bCmdTags[32];

    //PRINT("queue->m_bQueLen:%d\n",queue->m_bQueLen);

    bDir=NCQ_QueueInquire()->tParam.bDir;
    //tSataDC.wNcqTOCnt=4;
    PRINT(".........................bDir =%x\n",bDir);
    do
    {
        bCmdCnt=0;
        dTotalSec=0;

        bCmdCnt=NCQ_QueueMulPop(bDir,bCmdTags,&dTotalSec,&bCmdType,32);

        if(bCmdCnt == 0)
        {
            continue;
        }
        dLba=NCQ_GetParam(bCmdTags[0])->tParam.dLba;

#if (_FDL_EFFIC_TEST_ == 1)
        LgRdWrEfficTest(dLba, dTotalSec, bDir);
#endif
        NCQ_CmdReorderQueue(bCmdTags,bCmdCnt);
        DummyLgRdWrCtrl(dLba, dTotalSec, bDir);

        PRINT_DEC("C=",bCmdCnt);
        //PRINT_DEC("S=",dTotalSec);
        delay_us(100);
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

    }while(FTL_NCQNewCmdInQuire(&tTQC,bDir,10000));
}

// *****************************************************************************
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void  FTL_NCQCmdHandle(void)
{
    Test_FTL_NCQ_Op(NCQ_QueueInquire()->tParam.bDir);
}

