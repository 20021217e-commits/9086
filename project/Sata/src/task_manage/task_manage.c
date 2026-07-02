// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    task_manage.c 
//  文件标识：    
//  内容摘要：    实现 ftl 任务管理，模拟硬件FIFO
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2015年07月17日 
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************
#include "common.h"
#include "debug.h"
#include "task_manage.h"
#include "task_process.h"
#include "dsata.h"
#include "dma1c.h"
#include "ata_general.h"
#include "scsi.h"
#include "ata_gpl.h"
#include "dma1c.h"
#include "uitility.h"
#include "sata_misc.h"

// *****************************************************************************
// 函数名称: TQM_Init
// 函数功能: 任务队列管理模块初始化函数
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void  TQM_Init()
{
    U8 i;

    for(i=0;i<MAX_CMD_NUM;i++)
    {
        tTQC.tHiTasks[i].tParam.bIsUsed=FALSE;
#ifndef __NCQ_QUEUE_NEW__
        tTQC.tHiTasks[i].tParam.qwTaskNum=0;
#endif
        tTQC.tHiTasks[i].b4KFlag=FALSE;
        tTQC.tHiTasks[i].bIsContinue=FALSE;
        tCmdHead[i].dDMABO=0;
        tCmdHead[i].dPRDTBA=(U32)&tPrdTbls[i];
    }
    tTQC.tHiTasks[32].tParam.bIsUsed=FALSE;


    tTQC.bIsNcqCmd=FALSE;
    SG_MCU->rSF0_INT0_STS.dAll=0;//避免产生非预期的事件给G3
    //SATA FTL交互模块EXT_INT初始化
    SG_MCU->rSF0_INT1_STS0_CLR=0xFFFFFFFF;//STS0作为添加任务寄存器
    SG_MCU->rSF0_INT1_EN0=0x0;//禁用相应中断

    SG_MCU->rSF0_INT_EN.dAll=0x03;//使能INT0 和INT1

#ifdef __NCQ_QUEUE_NEW__

    tTQC.bReOdWrIdx=0;
    tTQC.bReOdRdIdx=0;
    tTQC.dReOdCnt=0;
    tTQC.dStartedCnt=0;
    tTQC.dPendingWrSec = 0;
    NCQ_QueueInit();
#endif

    NVIC_EnableIRQ(IRQ_SF1_INT0);
    NVIC_EnableIRQ(IRQ_SF1_INT1);
}
// *****************************************************************************
// 函数名称: TQM_CreateTask
// 函数功能: 创建任务
// 入口参数: eOpcode:任务类型
//             ptParameter:任务参数
//             pfTaskCompleteCb:事务完成回调函数
// 出口参数: 任务是否创建成功
// 说    明: 
// *****************************************************************************
BOOL TQM_CreateTask(CMD_OP eOpcode,T_HI_TASK *ptTask,TaskCompleteCb_t *pfTaskCompleteCb)
{
    U8 bCmdLbaSts;
    U8 bCmdHeadSts;

    //PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
//    DEBUG_PRINT("eOpCode=%d\r\n",eOpcode);
    ptTask->bOpcode=eOpcode;
    tSataDC.pfTaskCompleteCb=pfTaskCompleteCb;
    ptTask->eStatus=CMD_IDLE;

#ifdef    SC_HIDDEN_CMD
    U8 bTempWIdx = 0;
	if((FALSE== tTQC.bHidden_Cmd_Flg) && ( FALSE== SG_DSATA->rEXBUF_STS.bits.CRCPASS_EMPTY))
	{
		if( 0 == tTQC.bWrIdx)
		{
			bTempWIdx = 32;
		}
		else
		{
			bTempWIdx = tTQC.bWrIdx - 1;
		}
		if( CMD_SLOT32 == tTQC.bCmds[bTempWIdx])
		{
			PRINT_TXT("IN_M");
			tTQC.bHidden_Cmd_Flg = TRUE;
			tTQC.bHidden_Cmd_WIdx = bTempWIdx;
		}
	}
#endif
    if(OP_HI_DATA == eOpcode || OP_HI_TRIM == eOpcode)
    {
        if(SG_DMA1->rENABE.bits.EN == 0)
        	DMAC_DMA1Init();
#ifndef __SATA_MAXPERFORMER_TEST
        SG_DSATA->rERR_CLR.bits.DFISPATH_SEL = 0;//所有数据操作默认都需要使用DMA1进行传输
#endif
        //主机发起的事物
        //提前配置PRD信息 --本来该信息是在ATA层发起OP_HI_DATA任务的命令中配置的，但是该种命令较多，优化处理
        tSataDC.DOS_TransferReq(REQ_CONFIG,ptTask,pfTaskCompleteCb);//具体对应的配置接口已经在切换状态的时候注册完毕

        if(((0xC8 == tSataDC.bATACmd/*read dma*/)||(0xCA == tSataDC.bATACmd/*write dma*/))\
         ||((0x25 == tSataDC.bATACmd/*read dma ext*/)||(0x35 == tSataDC.bATACmd/*write dma ext*/)))
          {
//            DEBUG_PRINT("Scsi_ScsiModeVerify LBA=%x\r\n",ptTask->tParam.dLba);
            //DMA & DMA EXT
            bCmdLbaSts = Scsi_ScsiModeVerify(ptTask->tParam.dLba,ptTask->tParam.dSctSize,ptTask->tParam.bDir);
            switch(bCmdLbaSts)
            {
            case CMD_RESPONSE:
                //GPIO_DEBUG(0X43);
                break;

            case CMD_VAILD:
                //GPIO_DEBUG(0X44);
                if (OP_WR == ptTask->tParam.bDir)
                {
                    bCmdHeadSts = Scsi_CmdHeadVerify();
                }
                else
                {
                    bCmdHeadSts = TRUE;
                }

                //tTaskParam.dSctSize = tTaskParam.dSctSize - 1;
                if(TRUE == bCmdHeadSts)
                {
                    //GPIO_DEBUG(0X45);
                    //Scsi_IdleCmdHandle(ptTaskParam->bDir); 
                    if(Scsi_IntCmdHandle(ptTask->tParam.bDir))
                    {

                    }
                    else
                    {
                        //TODO：先收了部分数据发现不是SCSI命令，要整理下，把任务交给FTL
                    }
                }
                else
                {

                    //判断长度
                    //GPIO_DEBUG(0X46);
                    TQM_AddTask(ptTask);//添加任务队列进入任务队列队列
                }
                break;

            case CMD_INVAILD:

            default:
                //GPIO_DEBUG(0X47);
                TQM_AddTask(ptTask);//添加任务队列进入任务队列队列
                break;
            }
#ifdef _SCAN_SCSI_EXCEPTION_RECODE
            Scsi_StorePriCmdInfo(tSataDC.bATACmd, ptTask->tParam.dLba, tScsiCmdHead.bOpCode, tScsiCmdHead.bSubOpCode, tScsiCtl.bState, bCmdLbaSts);
#endif
        }
        else
        {
#ifndef __BOOT_CODE__
			if( SCSI_MODE == tScsiCtl.bMode_old)
			{
				tScsiCtl.bMode_old = NORMAL_MODE;
				DSATA_ExbuffAndCheckPoit_Ctrl(NORMAL_MODE);
			}
#endif
            tScsiCtl.dNextAddr = LBA_00005500;
            tScsiCtl.bMode     = NORMAL_MODE;
            tScsiCtl.bState    = CMD_STATE;
            tScsiCtl.bVaild    = FALSE;
            UART_PRINT("Exit SCSI mode....................\r\n");
            TQM_AddTask(ptTask);//添加任务队列进入任务队列队列
        }

    }
    else
    {
        //HI_NONE_DATA TASK
        ptTask->tParam.bDir=0x3;//非数据相关数据，方向配置为FF，以免误判
        ptTask->tParam.bTag=CMD_SLOT0;//没有初始化这个字段会导致POP接口取错任务属性
        TQM_AddTask(ptTask);//添加任务队列进入任务队列队列
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return TRUE;
}

// *****************************************************************************
// 函数名称: TQM_IntEvent2Ftl
// 函数功能: 通知FTL有新增任务
// 入口参数: NONE
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
static inline void  TQM_IntEvent2Ftl(U8 bSlot)
{
    //产生有新增任务中断信号给FTL模块
    SG_MCU->rSF0_INT1_STS0_SET=(1U<<bSlot);//sts0 用于添加任务
    SG_MCU->rSF0_INT1_EN0=(1U<<bSlot);
}
// *****************************************************************************
// 函数名称: TQM_DiTaskDoneEventHandler
// 函数功能: FTL DI事务处理完成中断
// 入口参数: NONE
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void  TQM_DiTaskDoneEventHandler(U32 dMask)
{
    if((dMask&0x01) != 0)
    {
#ifdef __DUAL_CORE__
    __unlock();//FTL 任务完成
#endif
      DEBUG_PRINT("E_SYNC\r\n");
      if(tSataDC.tSyncEvent.bOpcode ==OP_DI_PIO_DATA &&
         tSataDC.tSyncEvent.eStatus == CMD_ONGOING)
      {
          /*
           * 051498710:STANBY_REQ
             051498860:ATACmd=0x039
             drq=08
             lba:0x0BF6D08C
             dir+sec:0x030FC
             051498900:PO D2H   --原本这里的PIO任务刚刚创建，就错误地回复了D2H
           * */
          //STANDBY REQ之后出现PIO任务，需要继续执行PIO操作
          DEBUG_PRINT("PIO IN new CMD inssert....\r\n");
          //PRINT_TXT("NEW PIO");
          return;
      }

      if(tSataDC.tSyncEvent.pfEventCompleteCb != NULL)
      {
          //COMRESET 发起的IDFY 读写流程，可能中断其他的同步流程
          if((tSataDC.tSyncEvent.bOpcode == OP_DI_DATA) && (tTQC.tDiTask.dLba !=tSataDC.tSyncEvent.dParam[1]))
          {
              //后续有新的同步DI DATA 任务，本次回调过程提前结束。
              //重新发起新任务请求
              //PRINT_HEX("REDO_DI>",tTQC.tDiTask.dLba);
          }
          else
          {
              tSataDC.tSyncEvent.eStatus=tTQC.tDiTask.eStatus;
              tSataDC.tSyncEvent.pfEventCompleteCb(tSataDC.tSyncEvent.eStatus,&tSataDC.tSyncEvent);
          }
      }
      else
      {
          tSataDC.tSyncEvent.bOpcode =OP_NONE_EVENT;
          tSataDC.tSyncEvent.eStatus =CMD_DONE;
      }
    }

    if((dMask&0x02) != 0)
    {
        DEBUG_PRINT("E_ASYNC\r\n");
#ifdef __DUAL_CORE__
    __unlock();//FTL 任务完成
#endif

          if(tSataDC.tAsyncEvent.pfEventCompleteCb != NULL)
          {
              tSataDC.tAsyncEvent.eStatus=tTQC.tDiTask.eStatus;
              tSataDC.tAsyncEvent.pfEventCompleteCb(tSataDC.tAsyncEvent.eStatus,&tSataDC.tAsyncEvent);
          }
          else
          {
              tSataDC.tAsyncEvent.bOpcode =OP_NONE_EVENT;
              tSataDC.tAsyncEvent.eStatus =CMD_DONE;
          }
    }
#if(SATA_LOG_ENABLE ==0)
#ifdef __DUAL_CORE__
    __unlock();//FTL 任务完成
#endif
#endif
}
// *****************************************************************************
// 函数名称: TQM_HiTaskDoneEventHandler
// 函数功能: FTL事务处理完成中断
// 入口参数: NONE
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void  TQM_HiTaskDoneEventHandler(U8 bSlot)
{
    //处理FTL层事务完成中断,获取相关任务完成状态
    T_HI_TASK *ptTask=TQM_GetTask(bSlot);
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    if((ptTask->bOpcode == OP_HI_DATA))
    {
        if((tSataDC.pfTaskCompleteCb != NULL))
        {
            if(ptTask->eStatus< CMD_IDLE)
            {
                 //HI_DATA 相关只响应出错的命令
                tSataDC.pfTaskCompleteCb(ptTask->eStatus,&ptTask->tParam);
                TQM_RemoveTask(bSlot);
            }
            tSataDC.pfTaskCompleteCb=NULL;
        }
    }
    else
    {
        //NONE-DATA CMD 回调返回状态
        if(tSataDC.pfTaskCompleteCb != NULL)
        {
            DEBUG_PRINT("bSlot=%x,ptTask->eStatus= <%x>\r\n" ,bSlot,ptTask->eStatus);
            tSataDC.pfTaskCompleteCb(ptTask->eStatus,&ptTask->tParam);
            TQM_RemoveTask(bSlot);
            tSataDC.pfTaskCompleteCb=NULL;
        }

    }

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
}
// *****************************************************************************
// 函数名称: TQM_RemoveMulTask
// 函数功能: 任务队列管理接口，FTL完成任务以后更新多任务完成状态
// 入口参数: bSlot:完成任务的SLOT
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void  TQM_RemoveMulTask(U32 mask)
{
    U8 bCnt,bSlot;
    //聚合发送SDB
//    DSATA_BulidSDBFis(4,0,0,1,mask);
//    DSATA_EnableRxFis();//重新使能接收-使能接收以后PDMA才会进行发送动作
//    DSATA_TrigeSendFis(SDB_FIS);

    //清理现场
    bCnt=0;
    for(bSlot=0;bSlot<MAX_CMD_NUM;bSlot++)
    {
        if((mask & (1<< bSlot)) != 0)
        {
            bCnt++;
            TQM_RemoveTask(bSlot);
        }
    }

    SATA_UpdateRdIdx(bCnt);
    SG_MCU->rSF1_INT1_STS0_CLR=mask;
}

// *****************************************************************************
// 函数名称: TQM_RemoveTask
// 函数功能: 任务队列管理接口，FTL完成任务以后更新任务完成状态
// 入口参数: bSlot:完成任务的SLOT
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void  TQM_RemoveTask(U8 bSlot)
{

    T_HI_TASK *ptTask=&tTQC.tHiTasks[bSlot];
    ptTask->tParam.bIsUsed=FALSE;
    ptTask->b4KFlag=0;
    ptTask->bIsContinue = 0;
#ifndef __MAN_START_PDTR__

#ifdef SC_WP_4K_PRE
    if(CMD_SLOT32 == bSlot)
    {
    }
    else
#endif
    {
    	SG_MCU->rSF1_INT1_STS0_CLR=1U<<bSlot;//清除任务
    }
#endif
#if (SATA_LOG_ENABLE == 1)
    if(ptTask->bOpcode != OP_HI_DATA && tSataDC.tSyncEvent.bOpcode != OP_DI_PIO_DATA)
    {
        return;
    }
#endif
#ifdef __NCQ_QUEUE_NEW__
    if(DIR_OUT == ptTask->tParam.bDir)
    {
        tTQC.dPendingWrSec -= ptTask->tParam.dSctSize;
        //U32 dVal = ((U32)ptTask->tParam.bTag<<24)|ptTask->tParam.dSctSize;
        //PRINT_HEX("del=",dVal);
    }
#endif
    
#if (SATA_LOG_ENABLE == 1)
    //统计读写命令数,没有错误才需要累加
    if(ptTask->tParam.bDir == DIR_IN)
    {
        tSataLogCtx.qwNumOfReadCmd++;
//        if((U32)(tSataLogCtx.tSmartCtx.wReadSecCntFromNand+ptTask->tParam.dSctSize) >= 65536)
//        {
//            tSataLogCtx.tSmartCtx.wReadSecCntFromNand=(U32)(tSataLogCtx.tSmartCtx.wReadSecCntFromNand+ptTask->tParam.dSctSize)-65536;
//            tSataLogCtx.tSmartCtx.dReadSecCntFromNand++;
//        }
//        else
//        {
//            tSataLogCtx.tSmartCtx.wReadSecCntFromNand+=ptTask->tParam.dSctSize;
//        }

        if((U32)(tSataLogCtx.tSmartCtx.wReadSecCnt+ptTask->tParam.dSctSize) >= 65536)
        {
            tSataLogCtx.tSmartCtx.wReadSecCnt=(U32)(tSataLogCtx.tSmartCtx.wReadSecCnt+ptTask->tParam.dSctSize)-65536;
            tSataLogCtx.tSmartCtx.dReadSecCnt++;
//            if((tSataLogCtx.tSmartCtx.dReadSecCnt & 0x3F) == 0)
//            {
//                tSataLogCtx.enGplUpdateEvent=GPL_E_RESTART;
//                tSataLogCtx.bUpdateEn=TRUE;
//            }
        }
        else
        {
            tSataLogCtx.tSmartCtx.wReadSecCnt+=ptTask->tParam.dSctSize;
        }
    }
    else
    {
        tSataLogCtx.qwNumOfWriteCmd++;
//        if((U32)(tSataLogCtx.tSmartCtx.wWriteSecCnt2Nand+ptTask->tParam.dSctSize) >= 65536)
//        {
//            tSataLogCtx.tSmartCtx.wWriteSecCnt2Nand=(U32)(tSataLogCtx.tSmartCtx.wWriteSecCnt2Nand+ptTask->tParam.dSctSize)-65536;
//            tSataLogCtx.tSmartCtx.dWriteSecCnt2Nand++;
//        }
//        else
//        {
//            tSataLogCtx.tSmartCtx.wWriteSecCnt2Nand+=ptTask->tParam.dSctSize;
//        }

        if((U32)(tSataLogCtx.tSmartCtx.wWriteSecCnt+ptTask->tParam.dSctSize) >= 65536)
        {
            tSataLogCtx.tSmartCtx.wWriteSecCnt=(U32)(tSataLogCtx.tSmartCtx.wWriteSecCnt+ptTask->tParam.dSctSize)-65536;
            tSataLogCtx.tSmartCtx.dWriteSecCnt++;
//            if((tSataLogCtx.tSmartCtx.dWriteSecCnt & 0x3F) == 0)
//            {
//                tSataLogCtx.enGplUpdateEvent=GPL_E_RESTART;
//                tSataLogCtx.bUpdateEn=TRUE;
//            }
        }
        else
        {
            tSataLogCtx.tSmartCtx.wWriteSecCnt+=ptTask->tParam.dSctSize;
        }
    }

    NVIC_DisableIRQ(IRQ_T0);
    if(tSataLogCtx.enGplUpdateEvent != GPL_E_TIMEOUT && tSataLogCtx.enGplUpdateEvent != GPL_E_RESTART)
    {
        tSataLogCtx.dE_RWTimer=tSataDC.dPowerOnTime;
        tSataLogCtx.enGplUpdateEvent=GPL_E_RW;
        tSataLogCtx.bUpdateEn=TRUE;
    }
    NVIC_EnableIRQ(IRQ_T0);
#endif

    DEBUG_PRINT("task done!!\r\n");
}

// *****************************************************************************
// 函数名称: TQM_AbortAllTask
// 函数功能: SATA接口出错，终止任务队列
// 入口参数: NONE
// 出口参数: NONE
// 说    明: 
// *****************************************************************************
void  TQM_AbortAllTask()
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    SG_MCU->rSF1_INT1_STS0_CLR=0xFFFFFFFF;
    SG_MCU->rSF0_INT1_STS0_CLR=0xFFFFFFFF;
    //清除任务，模块重新初始化
    TQM_Init();
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: TQM_AddTask
// 函数功能: 任务队列管理模块添加新任务到任务列表并更新列表相关内部管理信息的模块
//           内部接口
// 入口参数: ptTask 任务指针
// 出口参数: RET_STATUS ：返回函数执行状态，如果等于RET_OK 则返回有效的指针
// 说    明: 
// *****************************************************************************
void  TQM_AddTask(T_HI_TASK *ptTask)
{

    ptTask->tParam.bIsUsed=TRUE;

#ifdef __NCQ_QUEUE_NEW__

    T_HI_TASK* ptail=NCQ_QueueInquireTail();
    if (ptail != NULL && ((ptail->tParam.dLba + ptail->tParam.dSctSize) == ptTask->tParam.dLba))
    {
        //LBA连续
        ptTask->bIsContinue = TRUE;
    }
    else
    {
        ptTask->bIsContinue = FALSE;
    }

#if 0//ndef  SC_H2DFIFO_EN
    ptTask->b4KFlag = ptTask->tParam.b4KCmdFlag;
#else
    if ((ptTask->tParam.dLba&0x07) || (ptTask->tParam.dSctSize!=8))
    {
        ptTask->b4KFlag = FALSE;
    }
    else
    {
//        //4K对齐属性
        ptTask->b4KFlag = TRUE;
    }
#endif

    if(DIR_OUT == ptTask->tParam.bDir)
    {
        tTQC.dPendingWrSec += ptTask->tParam.dSctSize;
        //U32 dVal = ((U32)ptTask->tParam.bTag<<24)|ptTask->tParam.dSctSize;
	    //PRINT_HEX("add=",dVal);
    }
    ptTask->tParam.dTime=tSataDC.dPowerOnTime;//记录命令入队列的时间点
#if 0
    NCQ_QueuePush(ptTask->tParam.bTag);
#else
    tTQC.bCmds[tTQC.bWrIdx]=ptTask->tParam.bTag;

    tTQC.bWrIdx++;
#ifdef  SC_H2DFIFO_EN
   if(33 == tTQC.bWrIdx)
   {
	   tTQC.bWrIdx = 0;
   }
#else
    tTQC.bWrIdx=(tTQC.bWrIdx&0x1F);
#endif
    //NVIC_DisableIRQ(IRQ_SF1_INT0);
    GIE_DISABLE();//该指令更简单
    tTQC.dCnt++;
    GIE_ENABLE();
    //NVIC_EnableIRQ(IRQ_SF1_INT0);
    TEST_PRINT("Ptag= 0x%x,dir = %d\r\n",ptTask->tParam.bTag,ptTask->tParam.bDir);
#ifdef  SC_WP_4K_PRE
    if((U8) CMD_SLOT32 == (U8)ptTask->tParam.bTag )
    {
        SG_MCU->rSF0_INT1_STS0_SET=(1U<<CMD_SLOT31);//sts0 用于添加任务
        SG_MCU->rSF0_INT1_EN0=(1U<<CMD_SLOT31);
//        TEST_PRINT("PSG_MCU->rSF0_INT1_STS0= 0x%x,\r\n",SG_MCU->rSF0_INT1_STS0);
    }
    else
#endif
    {
        SG_MCU->rSF0_INT1_STS0_SET=(1U<<ptTask->tParam.bTag);//sts0 用于添加任务
        SG_MCU->rSF0_INT1_EN0=(1U<<ptTask->tParam.bTag);
//        TEST_PRINT("SG_MCU->rSF0_INT1_STS0= 0x%x,\r\n",SG_MCU->rSF0_INT1_STS0);
    }
#endif

#else
    TQM_IntEvent2Ftl(ptTask->tParam.bTag);
#endif
    ptTask->eStatus=CMD_IDLE;
#ifndef __NCQ_QUEUE_NEW__
    ptTask->tParam.qwTaskNum =tSataDC.qwTaskCnt;
    tSataDC.qwTaskCnt++;//更新命令数
#endif
}

void TQM_CheckDiTaskDone(void)
{
    if(SG_MCU->rSF1_INT1_EN1 != 0)
    {
        //高32位数据
        //DI任务完成通知--传入任务号NCQ需要进入等待READLOG的状态
        U32 mask=SG_MCU->rSF1_INT1_STS1;
        SG_MCU->rSF1_INT1_STS1_CLR=SG_MCU->rSF1_INT1_STS1;
        TQM_DiTaskDoneEventHandler(mask);
        SG_MCU->rSF1_INT1_EN1 =0;
    }
}

#ifndef __G3_FLYCODE
// *****************************************************************************
// 函数名称: EXTA_INT1_IRQHandler
// 函数功能: 外部中断1服务函数,处理FTL触发的数据相关事务完成事件
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SF1_INT1_IRQHandler(void)
{
    //模拟SATA TO FTL 和 SATA TO FTL 添加任务删除任务事件中断
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    if(SG_MCU->rSF1_INT1_EN0 != 0)
    {
       SG_MCU->rSF1_INT1_EN0=0;
       if(tSataDC.bIsNcqCmd)
       TQM_HiTaskDoneEventHandler(DSATA_GetCurrentCmdSlot());//处理G3任务的异常
       else
       TQM_HiTaskDoneEventHandler(CMD_SLOT0);
    }

    TQM_CheckDiTaskDone();

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: EXTB_INT0_IRQHandler
// 函数功能: 外部中断0服务函数，处理FTL触发的异常事务完成事件
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void SF1_INT0_IRQHandler(void)//双核模式下优先级和T0一样高
{
#ifndef __BOOT_CODE__
    switch(tTQC.enEventType)
    {
        case E_TRANS_WO_DMA:
#if (SATA_PIO_NEW == TRUE)
            //解析参数，启动SATA直接数据读写
            DEBUG_PRINT("ETYPE=%d,param0=%d,param1=%d,param2=%x\r\n",tTQC.enEventType,tTQC.dEventParam[0],tTQC.dEventParam[1],tTQC.dEventParam[2]);
            if(OP_NONE_EVENT == tSataDC.tSyncEvent.bOpcode)
            {
                tTQC.dEventParam[3]=0xFFFFFFFF;//非0就是fail
                goto TIME_OUT;//直接终止任务
            }

            T_HI_TASK tTask;
            tTask.eStatus=CMD_VALID;
            tTask.tParam.bTag=CMD_SLOT0;
            tTask.tParam.bDir=tTQC.dEventParam[0];
            tTQC.dEventParam[3]=0;//描述本次数据传输状态，OK是0

            U16 I=0,FIR_PRD=0;
            U32 remain_sec=tTQC.dEventParam[1];
            SATA_XXX_DataTransferReq *dos_transferfunc=tSataDC.DOS_TransferReq;
            U16 loop_cnt=(tTQC.dEventParam[1]/tSataDC.bDrq) + 1;
            if(SG_DSATA->rPCMDR.bits.DPP == 0)
            {
                //第一个PRD 进来
                FIR_PRD=1;
            }
            GIE_DISABLE();
            //ERROR_PRINT("START\r\n");
            for(I=0;I<loop_cnt;I++)
            {
                if(remain_sec == 0)
                {
                    break;
                }

                //根据当前的bDrq 配置重新 配置任务
                if(remain_sec >= tSataDC.bDrq)
                {
                    tTask.tParam.dSctSize=tSataDC.bDrq;
                }
                else
                {
                    tTask.tParam.dSctSize=remain_sec;
                }
                remain_sec-=tTask.tParam.dSctSize;

                tTask.tParam.dBuffAddr=tTQC.dEventParam[2]+((I*tSataDC.bDrq)<<9);
                //ERROR_PRINT("START1111\r\n");
                if(dos_transferfunc != NULL)
                dos_transferfunc(REQ_TRANSFER,&tTask,NULL);
                if(FIR_PRD != 1)
                {
                    //第一次PRD传输，不需要清除DPP，等到下一轮进来清除否则代码执行慢的话会导致重复启动
                    SG_DSATA->rPISR.dAll =DPS_BIT;
                    DSATA_Update_rPCMDR(13,1);//清除pending 启动传输
                }
                else
                {
                    FIR_PRD=0;
                }
                //ERROR_PRINT("START2222\r\n");
                __enable_performance();//re enable

                while(!DSATA_IsPrdTransferDone(CMD_SLOT0))//等数据传输完成 DPS中断产生
                {
                    if(SG_DSATA->rPISR.bits.IFS || __get_pfm_count() >= (tSataDC.dMilliSecondCnt * 20000))//最大21S
                    {
                        //20超时，表示可能出现comreset，信号问题也会导致卡住，要及时退出
                         if(!SG_DSATA->rPISR.bits.IFS)
                         {
                             //非信号问题导致的超时
                             __disbale_performance();
                             tSataDC.dPowerOnTime+=20000;//更新时间计数
                             ERROR_PRINT("PIO wait prd done timeout!!!!!=\r\n");//DMA 模式一定会走这里
                             PRINT_TXT("E:DPSTO");
                         }
                         tSataDC.tSyncEvent.bOpcode =OP_NONE_EVENT;//以免重复发起任务
                         tSataDC.tSyncEvent.pfEventCompleteCb=NULL;
                         tTQC.dEventParam[3]=0xFFFFFFFF;//非0就是fail
                         GIE_ENABLE();//重新使能中断
                         goto TIME_OUT;//直接终止任务
                    }
                }

                DEBUG_PRINT("I=%d\r\n",I);
            }

            //数据传输完成
            if((DSATA_GetPrdRemainItems() == 0))
            {
                SG_DSATA->rPISR.dAll =DPS_BIT;
                DSATA_Update_rPCMDR(13,1);//清除pending 启动传输
            }

            GIE_ENABLE();
#endif
            break;
        case E_TRANS_TIMEOUT:
                ERROR_PRINT("WAITTING DMA1 DONE TIMEOUT!!\r\n");
                SG_MCU->rSF1_INT0_STS.dAll=0;//任务完成清除中断
                DSATA_ExceptionHandler(1);
                //DMA1超时的时候SC模块会卡死，即使是COMRESET都没有办法恢复
                //所以设定COMRESET 来了以后强行使SC恢复
                SG_CPM->rCOMRESET_CTRL.bits.COMRESET_EN=1;
                SG_CPM->rCOMRESET_CTRL.bits.COMRESET_RST_HCLK_EN=1;
                SG_CPM->rCOMRESET_CTRL.bits.COMRESET_RST_ACLK_EN=1;
                SG_CPM->rCOMRESET_CTRL.bits.COMRESET_RST_PCLK_EN=1;
                SG_CPM->rCPM_INT_CTRL.bits.COMRESET_INT_EN=1;//使能异常中断
                ERROR_PRINT("WAITTING COMRESET...\r\n");
            break;
        case E_PMG_ON:
                //FTL通知SATA端可以休眠
#if (L_PWN_MNG_EN == 1)
                if(SG_CPM->rDEVSLP_CTRL.bits.DEVSLP_EN == 0)
                {
                    //DEVSLP没有使能才允许停止 clk
                    SG_CPM->rPHY_POWER_CTRL.bits.PARTIAL_SLUMBER_CLK_GATING_EN=1;//允许clk gating
                    SG_CPM->rPHY_POWER_CTRL.bits.SLUMBER_MODE=1;//允许进入SLUMBER_S
                }
                else
                {
                    SG_CPM->rPHY_POWER_CTRL.bits.PARTIAL_SLUMBER_CLK_GATING_EN=0;//允许clk gating
                    SG_CPM->rPHY_POWER_CTRL.bits.SLUMBER_MODE=0;//允许进入SLUMBER_S
                    SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=1;
                }
                tSataDC.bIsBGActivityOn=FALSE;
#endif
            break;

        case E_PMG_OFF:
                //FTL有必须完成的任务，通知SATA端禁止休眠
#if (L_PWN_MNG_EN == 1)
                SystemClkRecovery();
                SG_CPM->rPHY_POWER_CTRL.bits.PARTIAL_SLUMBER_CLK_GATING_EN=0;//允许clk gating
                SG_CPM->rPHY_POWER_CTRL.bits.SLUMBER_MODE=0;//允许进入SLUMBER_S
                SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=0;//此时的状态不允许进入DEVSLP
                tSataDC.bIsBGActivityOn=TRUE;
#endif
            break;
        case E_UPDATE_REORDER:
            {
#ifdef __NCQ_QUEUE_NEW__
                tTQC.dCnt-=tTQC.dPopedNum;
                tTQC.dPopedNum=0;
#endif
//#ifdef    SC_WP_4K_PRE
//                tTQC.dPRECMD_ReoderCnt -= tTQC.dPRECMD_MoveCnt ;
//                tTQC.dPRECMD_MoveCnt = 0;
//#endif
                tTQC.dReOdCnt-=tTQC.dStartedCnt;
                tTQC.dStartedCnt=0;
#ifndef __MAN_START_PDTR__
                DSATA_CmdReorderRestart((T_PISR*)&SG_DSATA->rPISR,2);
#endif
                //PRINT_DEC("C2=",tTQC.dStartedCnt);
            }
            break;
        default:
            break;
    }
TIME_OUT:
tTQC.enEventType=E_UNDEFINE;
#endif
SG_MCU->rSF1_INT0_STS.dAll=0;//清除中断
}
#endif



T_HI_TASK*  TQM_GetTask(U8 bSlot)
{
    return &tTQC.tHiTasks[bSlot];
}

void Wait4_Ftl2SataTaskDone()
{
    U32 Tstart=tSataDC.dPowerOnTime;
    while(SG_MCU->rSF1_INT0_STS.dAll == 1)//清除操作由另一个核完成。
    {
         if((tSataDC.dPowerOnTime - Tstart) >= 3000)
         {
             //G0 响应超时
             PRINT_TXT("E:F2STO");//FTL TO SATA event time out
#ifdef  __DEBUG_WHILE
             while(1);
#endif
             return;
         }
    }
}

/*
 *函数功能：FTL 请求SATA 收发数据到指定的BUFF中
 *参数bDir：读写方向DIR_IN,DIR_OUT
    参数wSctCnt：数据扇区数
    参数dBuffAddr：数据buffer地址
 *
 * */
void Ftl2SataTask_DataTransReq(U8 bDir,U16 wSctCnt,U32 dBuffAddr)
{
    //不是最后一次传入的buffer size必须要是DRQ size的倍数，DRQ默认为8，即4K，如果改成16则要改成8K的倍数
    tTQC.enEventType=E_TRANS_WO_DMA;
    tTQC.dEventParam[0]=bDir;
    tTQC.dEventParam[1]=wSctCnt;
    tTQC.dEventParam[2]=dBuffAddr;
    SG_MCU->rSF1_INT0_STS.dAll=1;
    Wait4_Ftl2SataTaskDone();
}

/*
 *函数功能：FTL通知SATA DMA1超时事件
 *
 * */
void Ftl2SataTask_Dma1TimeOut(void)
{
    tTQC.enEventType=E_TRANS_TIMEOUT;
    SG_MCU->rSF1_INT0_STS.dAll=1;
    Wait4_Ftl2SataTaskDone();
}

/*
 *函数功能：FTL通知SATA没有后台任务，可以使能休眠唤醒功能
 *
 * */
void Ftl2SataTask_PmgOn(void)
{
    tTQC.enEventType=E_PMG_ON;
    SG_MCU->rSF1_INT0_STS.dAll=1;
    Wait4_Ftl2SataTaskDone();
}

/*
 *函数功能：FTL通知SATA准备进行后台任务，禁止休眠唤醒功能
 *
 * */
void Ftl2SataTask_PmgOff(void)
{
    tTQC.enEventType=E_PMG_OFF;
    SG_MCU->rSF1_INT0_STS.dAll=1;
    Wait4_Ftl2SataTaskDone();
}

#ifdef __NCQ_QUEUE_NEW__

/*
 *
 * 	参数1：NCQ命令队列地址
    参数2：重排后的TAG队列首地址
    参数3：本次续接的命令个数
 *
 * */
void NCQ_CmdReorderQueue(U8* pTags,U8 bCmdCnt)
{
    U8 bTagIdx = 0;
    U8 bReorderCnt = bCmdCnt;
    T_TASK_QUEUE_CTX* queue=&tTQC;

    if((SG_DSATA->rFILL_INVLDA_ST.dAll == 1) || (bCmdCnt == 0))
    {
        //CRC 异常处理流程直接退出
        return;
    }
#ifdef  SC_H2DFIFO_EN
    tTQC.bRecordCmdPop =  tTQC.bRecordCmdPop - bCmdCnt;
#endif

#ifdef    CHECK_CMD_SEQ_EXE
    U8 bTempCmdCnt = bCmdCnt;
    while(bTempCmdCnt--)
    {
		if(g_tDebug.CheckCmdNum)  //只要写命令按顺序POP 出去, 就检测 Reorder 命令顺序
		{
			if(pTags[bTagIdx] !=  g_tDebug.bPopCmds[g_tDebug.bCheckCmdIdx] )  //请教gavin ,FTL  不会POP 读写方向混着的命令
			{
				PRINT_TXT("E: CMD_RuleF");
				while(1);  //Debug
			}
			g_tDebug.CheckCmdNum--;
			g_tDebug.bCheckCmdIdx++;
			if( 33 == g_tDebug.bCheckCmdIdx)
			{
				g_tDebug.bCheckCmdIdx = 0;
			}
		}
		bTagIdx++;
    }
    bTagIdx = 0;
#endif

#ifndef	SC_H2DFIFO_EN

    if(!queue->bIsNcqCmd)
    {
        if(SG_DSATA->rPDTR == 0 && SG_DSATA->rPSERR.dAll == 0)
        {
            //flycode 模式下延迟启动PDTR
            DSATA_TransferStart(CMD_SLOT0);
        }
        SG_MCU->rSF1_INT1_STS0_SET=1;
        SG_MCU->rSF0_INT1_STS0_CLR=1;
        return;
    }
    //Wait4_Ftl2SataTaskDone();
    while(bCmdCnt--)
    {
        queue->bReOdCmds[(queue->bReOdWrIdx&0x1F)]=pTags[bTagIdx];
        bTagIdx++;
        queue->bReOdWrIdx++;
        queue->dReOdCnt++;
    }
    queue->bReOdWrIdx=(queue->bReOdWrIdx&0x1F);
#ifdef __MAN_START_PDTR__
    if(queue->dStartedCnt >= 0xFFFFFFFF-64)
#else
    if(queue->dStartedCnt >= 0xFFFFFFFF-64 || ((tTQC.dReOdCnt - tTQC.dStartedCnt) == bTagIdx && SG_DSATA->rNCQ_DONE_TOTAL.dAll >= SG_DSATA->rNCQ_REORDER_TOTAL.dAll))
#endif
    {
        //不能太频繁产生中断，否则会影响效率
        queue->enEventType=E_UPDATE_REORDER;
        SG_MCU->rSF1_INT0_STS.dAll=1;
        Wait4_Ftl2SataTaskDone();//new
        //不能放到前面去等done，如果G0正在启动命令，卡住更新流程，接下来又去取新命令就可能发生同时改写wPopedNum的问题
    }

#else

#ifdef SC_WP_4K_PRE
    if(SG_DSATA->rPRE_TRIG_CNT && ( CMD_SLOT32 == pTags[0]))   //预收命令优先POP 出去的
    {
    	if(0 == queue->tHiTasks[31].tParam.bIsUsed)
    	{
    		SG_MCU->rSF0_INT1_STS0_CLR= 1 << CMD_SLOT31;
    	}

    	SG_DSATA->rPOP_4K.bits.dVAL = 1;   //PRE_TRIG_CNT 减1
    	queue->dPRECMD_ReoderCnt++;
		if(queue->dPRECMD_ReoderCnt >=256)
		{
			queue->dPRECMD_ReoderCnt =0;
		}
		SG_DSATA->rPOP_ST = 1;             // 下一个系统时钟 HW 清0
    	while(SG_DSATA->rPRE_TRIG_CNT);
    	bTagIdx++;
    	bCmdCnt--;
    	bReorderCnt--;
    	queue->bPRE_Cmd_Flg = FALSE;   //清预收命令Flg
    }
#endif
    if(bCmdCnt>0)
    {
		if(!queue->bIsNcqCmd)
		{
			bCmdCnt = 0;     // 非NCQ 还没完成，新命令不能进来
			SG_MCU->rSF1_INT1_STS0_SET=1;
			SG_MCU->rSF0_INT1_STS0_CLR=1;
			if( !tSataDC.bIsNcqCmd &&(SG_DSATA->rPDTR == 0) && (SG_DSATA->rPSERR.dAll == 0))
			{
				//flycode 模式下延迟启动PDTR
				DSATA_TransferStart(CMD_SLOT0);
			}
			return;
		}

		while(bCmdCnt--)
		{
			queue->bReOdCmds[(queue->bReOdWrIdx&0x1F)]=pTags[bTagIdx];
			bTagIdx++;
			queue->bReOdWrIdx++;
			SG_DSATA->rPOP_ST = 1;       // 下一个系统时钟 HW 清0
			queue->dReOdCnt++;
		}
		queue->bReOdWrIdx=(queue->bReOdWrIdx&0x1F);

#ifdef __MAN_START_PDTR__
		if(queue->dStartedCnt >= 0xFFFFFFFF-64)
#else
		if((queue->dStartedCnt >= 0xFFFFFFFF-64) || (((tTQC.dReOdCnt - tTQC.dStartedCnt) == bReorderCnt) && (SG_DSATA->rNCQ_DONE_TOTAL.dAll >= SG_DSATA->rNCQ_REORDER_TOTAL.dAll)))
#endif
		{
			//不能太频繁产生中断，否则会影响效率
			queue->enEventType=E_UPDATE_REORDER;
			SG_MCU->rSF1_INT0_STS.dAll=1;
			Wait4_Ftl2SataTaskDone();//new
			//不能放到前面去等done，如果G0正在启动命令，卡住更新流程，接下来又去取新命令就可能发生同时改写wPopedNum的问题
		}
    }
//    PRINT(" reoder_cmd  \r\n");
#endif
}


/*
 * NCQ 队列深度查询
 * 返回值：当前队列任务数
 * */
U8 NCQ_GetQueueLen(void)
{

    U8 bCmdPendingNum = (U8)(tTQC.dCnt-tTQC.dPopedNum);
#ifdef  SC_H2DFIFO_EN
    U8 bPopCmdMax = MAX_CMD_NUM - tTQC.bRecordCmdPop;
#ifdef  SC_HIDDEN_CMD
	if( TRUE == tTQC.bHidden_Cmd_Flg)
	{
		if(TRUE == SG_DSATA->rEXBUF_STS.bits.CRCPASS_EMPTY)
		{
	        tTQC.bHidden_Cmd_Flg = FALSE;
			PRINT_TXT("Out_M");
		}
		else
		{
			if(tTQC.bRdIdx == tTQC.bHidden_Cmd_WIdx)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
#endif

    if(bCmdPendingNum > bPopCmdMax)
    {
//    	PRINT("Num = %d\r\n",bCmdPendingNum);
    	return bPopCmdMax;
    }
#endif
    return bCmdPendingNum;
}

U32 NCQ_GetQueueWrSec(void)
{
    return tTQC.dPendingWrSec;
}
/*
 * NCQ 队列初始化
 * */
void NCQ_QueueInit(void)
{
    tTQC.dCnt=0;
    tTQC.bRdIdx=0;
    tTQC.bWrIdx=0;
    tTQC.bF2S_Task_abortFlg = 0;
#ifdef    CHECK_CMD_SEQ_EXE
    g_tDebug.bCheckCmdIdx = 0;
    g_tDebug.CheckCmdNum = 0;
    g_tDebug.bPopCmdIdx = 0;
#endif
    tTQC.dPopedNum=0;
#ifdef  SC_H2DFIFO_EN
    tTQC.bRecordCmdPop = 0;
#endif
#ifdef    SC_WP_4K_PRE
    tTQC.dPRECMD_MoveCnt = 0;
    tTQC.dPRECMD_ReoderCnt = 0;
    tTQC.dPRECMD_PushCnt = 0;
    tTQC.bPRE_Cmd_Flg=0;

#endif
#ifdef  SC_HIDDEN_CMD
    tTQC.bHidden_Cmd_Flg = 0;
    tTQC.bHidden_Cmd_WIdx = 0;
#endif
}

/*
 * NCQ 查询下一条命令响应延迟时间
 * 返回值：延迟时间，单位ms
 * */
U16 NCQ_InquireTimeOfNextCmd(void)
{
    return (tSataDC.dPowerOnTime-tTQC.tHiTasks[tTQC.bCmds[(tTQC.bWrIdx - tSataDC.bCmdCnt)&0x1F]].tParam.dTime);
}


/*
 * 查询NCQ Reoder队列 是否完成
 * 返回值：当前HW 和 SW 的REODER队列是否完成，完成返回0，未完成返回1
 * */
U8 NCQ_InquireReOrderDone(void)
{
    if(SG_DSATA->rNCQ_DONE_TOTAL.dAll >= SG_DSATA->rNCQ_REORDER_TOTAL.dAll &&\
               tTQC.dReOdCnt == tTQC.dStartedCnt)
    {
        return 0 ;
    }
    else
    {
        return 1 ;
    }
}

/*
 * NCQ新任务入队列
 * 参数bTag ：命令TAG号
 * */
void NCQ_QueuePush(U8 bTAG)
{
    //中断里面
    tTQC.bCmds[tTQC.bWrIdx]=bTAG;
    tTQC.bWrIdx++;
#ifdef  SC_H2DFIFO_EN
   if(33 == tTQC.bWrIdx)
   {
	   tTQC.bWrIdx = 0;
   }
#else
    tTQC.bWrIdx=(tTQC.bWrIdx&0x1F);
#endif

    NVIC_DisableIRQ(IRQ_SF1_INT0);
    tTQC.dCnt++;
    NVIC_EnableIRQ(IRQ_SF1_INT0);
    __nds32__dsb();

    SG_MCU->rSF0_INT1_STS0_SET=(1U<<bTAG);//sts0 用于添加任务
    SG_MCU->rSF0_INT1_EN0=(1U<<bTAG);
}

/*
 * 查询NCQ任务队列，队首命令的方向
 * 返回：队首指针
 * */
T_HI_TASK* NCQ_QueueInquire(void)
{
    //查询下一个任务的方向
    if( FALSE == NCQ_GetQueueLen())
    {
        return NULL;
    }
    else
    {
        return &tTQC.tHiTasks[tTQC.bCmds[tTQC.bRdIdx]];
    }
}

/*
 * 查询NCQ任务队列，队尾命令参数
 * 返回：队尾指针
 * */
T_HI_TASK* NCQ_QueueInquireTail(void)
{
    //查询下一个任务的方向
    if(FALSE == NCQ_GetQueueLen())
    {
        return NULL;
    }
    else
    {
#ifdef  SC_H2DFIFO_EN
    	if(0 ==  tTQC.bWrIdx )
    	{
    		return &tTQC.tHiTasks[tTQC.bCmds[(MAX_CMD_NUM)]];
    	}
    	else
    	{
    		return &tTQC.tHiTasks[tTQC.bCmds[tTQC.bWrIdx-1]];
    	}
#else
        return &tTQC.tHiTasks[tTQC.bCmds[(tTQC.bWrIdx-1)&0x1F]];
#endif
    }
}
/*
 * 根据TAG号，获取任务参数指针
 * 返回：队首指针
 * */
T_HI_TASK* NCQ_GetParam(U8 bTag)
{
#ifdef  SC_H2DFIFO_EN
    //TODO:确保传入参数在33以内
    if(bTag>=33)
    {
        return NULL;
    }
#else
    //TODO:确保传入参数在32以内
    if(bTag>=32)
    {
        //PRINT_TXT("E:TagNum>31");
        return NULL;
    }
#endif
    return &tTQC.tHiTasks[bTag];
}
/*
 * NCQ命令队列，命令出栈
 * 参数：
 * T_QTASK* 新命令参数出口
 * bDir：指定任务方向出栈
 * 	 0xFF:IN/OUT BOTH
 * 		0：DIR_OUT
 * 		1：DIR_IN
 * */
T_HI_TASK* NCQ_QueuePop(U8 bDir)
{
    //U32 mask;
    T_HI_TASK *ptr=NULL;
    if(FALSE == NCQ_GetQueueLen())
    {
        return NULL;
    }
    if(!tTQC.bIsNcqCmd && (tTQC.dCnt - tTQC.dPopedNum)>2)
    {
        //TODO：理论上不可能出现这种情况，debug用
#ifdef  __DEBUG_WHILE
        while(1);
#endif
    }
    ptr=&tTQC.tHiTasks[tTQC.bCmds[tTQC.bRdIdx]];


    if(0xFF == bDir || (0x03 == bDir ))  // 0xFF 是表示不管方向， 0x03是SATA DRIVE_NEW 修改的，也是代表不管方向
    {

    }
    else
    {
        if(bDir!=ptr->tParam.bDir)
        {
            //可以进行预取，FF表示不管方向，非FF表示只取指定方向的任务0: OUT，1: IN
            return NULL;
        }
#ifdef  SC_H2DFIFO_EN
    	 tTQC.bRecordCmdPop++;   //这个在非数据传输时 或PIO 命令 不想加。
#endif
    }


    TEST_PRINT("pushtag=%d bDir = %d \r\n",tTQC.bCmds[tTQC.bRdIdx] ,ptr->tParam.bDir);
    //mask=(1<<ptr->tParam.bTag);
    //NCQ_CmdReorderQueue(&ptr->tParam.bTag,1);
    //SG_MCU->rSF0_INT1_STS0_CLR=mask;
    //SG_MCU->rSF1_INT1_STS0_SET=mask;
    //PRINT4("lba=%xd,sec=%xd,dir=%db,tag=%db\r\n",ptr->tParam.dLba,ptr->tParam.dSctSize,ptr->tParam.bDir,ptr->tParam.bTag);
    tTQC.bRdIdx++;
#ifdef  SC_H2DFIFO_EN
   if(33 == tTQC.bRdIdx)
   {
	   tTQC.bRdIdx = 0;
   }

#else
   tTQC.bRdIdx=(tTQC.bRdIdx&0x1F);
#endif
    tTQC.dPopedNum++;

#ifdef    CHECK_CMD_SEQ_EXE
    if( DIR_OUT == ptr->tParam.bDir)
    {
        g_tDebug.CheckCmdNum ++;
        g_tDebug.bPopCmds[g_tDebug.bPopCmdIdx] =ptr->tParam.bTag ;
        g_tDebug.bPopCmdIdx++;
        if(33 == g_tDebug.bPopCmdIdx)
        {
        	g_tDebug.bPopCmdIdx =0;
        }
    }
#endif
    return ptr;
}

/*
 * NCQ命令队列，指定方向的多命令出栈
 * 参数：
 * T_QTASK* 新命令参数出口
 * bDir：指定任务方向出栈
 * 	 	0x03:IN/OUT BOTH
 * 		0：DIR_OUT
 * 		1：DIR_IN
 * tTags:输出tag数组
 
 * dTotalSize:输出总长度
 
 * bCmdType:输出命令类型，1->4KMode 2->大数据 0->小于4K
 
 * 返回值：
 * 		返回的命令数
 * */
U8 NCQ_QueueMulPop(U8 bDir,U8* bTags,U32 *dTotalSize,U8 *bCmdType,U8 bNum)
{
    U8 bCmdCnt=0;
    T_HI_TASK* ptask=NULL;
    *dTotalSize = 0;
    *bCmdType = 0;
    if((FALSE == NCQ_GetQueueLen()) || bNum == 0)
    {
        //PRINT("tTQC.bCnt=%d,tTQC.bPopedNum=%d\r\n",tTQC.bCnt ,tTQC.bPopedNum);
        //while(1);
        return 0;
    }

    if(0xFF == bDir || (0x03 == bDir ))  // 0xFF 是表示不管方向， 0x03是SATA DRIVE_NEW 修改的，也是代表不管方向
    {
    	 bDir=NCQ_QueueInquire()->tParam.bDir;
    }
    else
    {
        if(NCQ_QueueInquire()->tParam.bDir != bDir)
        {
            //可以进行预取，FF表示不管方向，非FF表示只取指定方向的任务0: OUT，1: IN
            return NULL;
        }
    }

    //1、先记录首条CMD  2、再判断剩下的可不可以续接（确保续接的时候一定经过条件筛选）
    ptask=NCQ_QueuePop(bDir);//获取第一条命令，一定可以取到
    *dTotalSize +=ptask->tParam.dSctSize;
    bTags[bCmdCnt++] = ptask->tParam.bTag;//记录返回的TAG号

    while(NCQ_GetQueueLen() !=0)
    {
        //续接剩下的CMD

        if ((bDir == NCQ_QueueInquire()->tParam.bDir) && (bCmdCnt != bNum))
        {
            if (ptask->b4KFlag && NCQ_QueueInquire()->b4KFlag) //如果是当前是4K，一下也是4K 合并
            {
                //都是4k可以续接
            }
            else if (NCQ_QueueInquire()->bIsContinue) //如果地址与前一条是连续，如果地址连续，合并
            {
                //命令间连续可以续接
            }
            else
            {
               break;  //地址不是4K，又不连续，退出
            }
        }
        else
        {
            //方向不一致，或者数目足够，退出
            break;
        }

        //满足续接条件
        ptask=NCQ_QueuePop(bDir);//保证一定可以取到
        *dTotalSize +=ptask->tParam.dSctSize;
        bTags[bCmdCnt++] = ptask->tParam.bTag;//记录返回的TAG号
    }

    if (ptask->b4KFlag)   //4k，整数据对齐
    {
        *bCmdType = 1;
    }else if (*dTotalSize > 8) //>4K大数据
    {
        *bCmdType = 2;
    }
    else //<=4K  或不对齐
    {
        *bCmdType = 3;
    }
    return bCmdCnt;
}

///*
// * NCQ命令确认
// * 新命令出栈完成以后重排序操作
// * */
//void NCQ_QueuedConfirm(void)
//{
//	NCQ_CmdReorderQueue(&tTQC,tTQC.bTags,tTQC.bQdTasCnt);
//	tTQC.bQdTasCnt=0;
//}
// *****************************************************************************
// 函数名称:   SATA_DataFirstPassPointEn(U8 bFirstPassEn)
// 函数功能:  开启 数据过CRC 第一级放行点（FIFO后） 或恢复原来第二级放行点（Exbuff）
// 入口参数:  bFirstPassEn  0: 开启第一级放行点并且关第二级，  1：关第一级放行点并且开启第二级放行点
// 出口参数:
// 说    明:
// *****************************************************************************
void SATA_DataFirstPassPointEn(U8 bFirstPassEn)
{
#if (SC_CRC_PASS_ST_DMA1 == EXBUFF_CRC_PASS)   //只有开口Exbuff 才需要开启放行点的切换
	if(bFirstPassEn)
	{
		SG_DSATA->rFIFO_HOLD.dAll = 0;  // 解决私有命令下开启第一级放行点（FIFO 后），8K数据不能全收到FIFO 中的问题
		SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 1;  //关闭第一级放行
		SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 0;  //关闭第二级
	}
	else
	{
		SG_DSATA->rFIFO_HOLD.dAll = 0x2f;  // 恢复最优性能
		SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 0;  //关闭第一级CRC PASS 检验
		SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 1;  //打开第二级CRC PASS 检验    数据过了CRC 从BUFF 传到DMA1
	}
#endif
}

// *****************************************************************************
// 函数名称:   FTL2Sata_TaskAbort
// 函数功能:   FTL 通知SATA ,后续命令任务终止
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void FTL2Sata_TaskAbort(void)
{

    tTQC.bF2S_Task_abortFlg  = TRUE;
    U32 Tstart=tSataDC.dPowerOnTime;
    while(TRUE == tTQC.bF2S_Task_abortFlg)
    {
    	if((tSataDC.dPowerOnTime - Tstart) >= 50)
    	{
    		PRINT_TXT("E:F2S_Task");//等待超时
    		break;
    	}
    }
}
#endif
void Save_Uart_Buff_To_Log(void)
{
#ifndef __BOOT_CODE__
    PRINT_HEX("PCC>",tSataDC.dPowerCycleCnt);//SMART 0
    PRINT_HEX("POH>",tSataDC.dLifePowerOnTime);//SMART 09

    tSataDC.bSaveUartBuff = TRUE;
#endif
#ifdef __SATA_LOG_EN__
    //发起保存log事件
    tSataLogCtx.enGplUpdateEvent=GPL_E_TIMEOUT;
    tSataLogCtx.bUpdateEn=TRUE;
#endif
}
