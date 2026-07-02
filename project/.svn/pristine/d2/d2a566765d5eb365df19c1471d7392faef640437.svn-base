// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  dsata.c
//  文件标识：  
//  内容摘要：  SATA 模块驱动
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月16日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "sg9081.h"
#include "sata_misc.h"
#include "debug.h"
#include "dma1c.h"
#include "mcu.h"
#include "task_manage.h"
#if (SATA_LOG_ENABLE == 1)
#include "ata_gpl.h"
#include "ata_sanitize.h"
#endif
#include "task_process.h"
#include "scsi.h"
#include "sata_typedef.h"
#include "dsata.h"
#include "ata_identify.h"
#include "pad.h"
#if(UART_DEBUG_ON == 1)
#include "uart.h"
#endif

#ifdef __BOOT_CODE__
#include "AbnormalHandle.h"
#endif

#include "uitility.h"
#ifdef  __FW_TRIGGER_SATA_CRC__
#include "SATA_AbormalHandle.h"
#endif

#ifndef __G3_FLYCODE
#ifdef  __ALG_FORMAL
#include "main.h"
#endif
#endif

//TODO:BOOT 发布的时候要注释这个信息
//#ifndef __BOOT_CODE__
//#define __FPGA_PLATEFORM
//#else
//#endif
#define SPEED_GEN1    (1U)
#define SPEED_GEN2	  (2U)
#define SPEED_GEN3	  (3U)

#define SATA_SPEED    SPEED_GEN1


//SATA IP Command Head Structure 1K-byte-aligned
__align(1024) T_CMD_HEAD tCmdHead[MAX_CMD_NUM] __section(".cmdhead");
#ifdef  SC_H2DFIFO_EN
__align(1024) UN_HWH2DFIS arrunHWD2HFS[MAX_CMD_NUM + 1] __section(".fsm_hwd2h");//1K对齐
#else
__align(1024) UN_HWD2HFS  arrunHWD2HFS[MAX_CMD_NUM] __section(".fsm_hwd2h");//1K对齐
#endif
//This address is 128-byte-aligned
__align(8) T_PRD_ENTRY tPrdTbls[MAX_CMD_NUM] __section(".prdtable");//一个CMD 对应一个prd
//FIS 公用缓冲区 32byte-aligned
__align(32) UN_FSM unFsmBuff __section(".fsm");
#ifdef SC_EXBUFF_EN
__align(4096) T_ExBuff_8K tpbsata_exbuff __section(".Exbuff");
#endif
//FIS type 列表
U8_C FIS_CODE[]={0x27,0x34,0x39,0x41,0x46,0x58,0x5F,0xa1,0xa6,0xb8,0xbf,0xc7,0xd4,0xd9};

void DSATA_DMA1ModeInit(void)
{
    SG_DMA1->rENABE.bits.EN           = 1;
}

// *****************************************************************************
// 函数名称: DSATA_Init
// 函数功能: SATA初始化函数
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void DSATA_Init(U8 powermode)
{
    if(powermode == 0)
        {
            //modify after review
            //SC复位
            SG_CPM->rPRE_CLK_SOURCE_SEL=0x02;//选择25M
            SG_CPM->rSYS_CLK_SOURCE_SEL=1;//选择pre clk
            //接下来时钟会在25M
            //Disable top-level interrupt enable (GCR.IE) and reset the core (GCR.R).
            SG_DSATA->rGCR.bits.IE=0;//禁用中断
            SG_DSATA->rGCR.bits.R=1;//R: Global Reset

            SG_CPM->rSOFT_RST0.bits.SC=0; //注意：SC不reset，只reset PHY不能完成OOB过程
            SG_CPM->rSOFT_RST0.bits.PHY=0;//MCU时钟来自PHY板所以复位操作要小心

            __nds32__dsb();
            __enable_performance();//re enable
            while( SG_CPM->rSOFT_RST0.bits.PHY ==0 || SG_CPM->rSOFT_RST0.bits.SC == 0)//等待RESET完成
            {
                if(__get_pfm_count() >= 100)
                {
                    break;
                }
            }


            __disbale_performance();
            SG_DSATA->rPDMACR.dAll=0x5a;//FPGA 阶段这个数值能达到最优速度

#ifdef __FPGA_PLATEFORM__FPGA_PLATEFORM
            SG_DSATA->rPSCTLR.bits.SCTL_SPD = SATA_SPEED;//MAX SPEED=GEN1 1.5G，FPGA阶段高速跑得不稳定
#else
    #ifdef __SATA_TEST
                SG_DSATA->rPSCTLR.bits.SCTL_SPD = tSataDC.bGenxSpeed;
    #else
        #if(SATA_LOG_ENABLE == 1)
                SG_DSATA->rPSCTLR.bits.SCTL_SPD = tSataDC.tSataMI.tMpCfg.bits.SATA_SPD;
        #endif
    #endif
#endif

#ifndef __BOOT_CODE__
            SG_DSATA->rRST_RD_EN.dAll=0x01;//避开ALIGN红色的问题
        //    SG_DSATA->rDEL_NONALIGN_EN.dAll=1;//解决GEN1 不识别ALIGN的问题
            //BOOT CODE有些修改不应用，以免隐藏BUG
            SG_DSATA->rDLY_TXSYNC_CNT.dAll =1;//等待128个align时间
            SG_DSATA->rSHORTPATH_EN.dAll=0xF;
#endif
            __enable_performance();//re enable
            while(SG_CPM->rPHY_PLL_STS == 0)
            {
                if(__get_pfm_count() >= 100)
                {
                    break;
                }
            }


            __disbale_performance();
            SystemClkRecovery();//PHY时钟稳定后，恢复时钟配置
        }
        else
        {
            SG_DSATA->rPDMACR.dAll=0x5a;
#ifdef __FPGA_PLATEFORM
            SG_DSATA->rPSCTLR.bits.SCTL_SPD = SATA_SPEED;//MAX SPEED=GEN1 1.5G，FPGA阶段高速跑得不稳定
#endif
#ifndef __BOOT_CODE__
     //       SG_DSATA->rDEL_NONALIGN_EN.dAll=1;//解决GEN1 不识别ALIGN的问题
            SG_DSATA->rRST_RD_EN.dAll=0x01;//避开ALIGN红色的问题
            //BOOT CODE有些修改不应用，以免隐藏BUG
            SG_DSATA->rDLY_TXSYNC_CNT.dAll =1;//等待128个align时间
            SG_DSATA->rSHORTPATH_EN.dAll=0xF;
#endif
        }
    SG_DSATA->rBISTCR.dAll = 0x001700;//J1900 平台认盘问题

    SG_DSATA->rGT_DMAP1_CNT.bits.dVAL = 1;
    SG_DSATA->rCRC_PASS_DLYTM.dAll =  0x0;
    SG_DSATA->rCRC_PASS_DLYEN.dAll = 1;     //解决Exbuff 丢数据的问题
    /*Pointer registers to the FIS Structure and 
    Command List are initialized (PCLBR and PFBR)*/
#ifndef __BOOT_CODE__
#ifdef  SC_H2DINT_BLOCK_EN
     SG_DSATA->rFW_EN.bits.H2DINT_BLOCK_EN = 1;
#endif

#ifdef SC_REORDER_DLY_EN
        SG_DSATA->rREDOR_DLY_TIME.bits.dVAL = tSataDC.dMicroSecondCnt * 10;   //Reorder Dly 10us
#endif  //SC_REORDER_DLY_EN

#ifdef  SC_SDB_DLY_EN
     SG_DSATA->rSDB_DLY_TIME.bits.dVAL =  tSataDC.dMicroSecondCnt / 10;        //SDB Dly 10us
#endif  //SC_SDB_DLY_EN

#ifdef   SC_TRIG_DMASTP_EN
     SG_DSATA->rFW_EN.bits.TRIG_DMASTP_EN = 1; // 等DMA2 数据RDY 再启动 DMASETUP
#endif
#endif    //BOOT Code  不开新功能
#ifdef   SC_CRC_PASS_EN
     SG_DSATA->rFW_EN.bits.CRCPASS_CNT_EN = 1;    // 启动该功能。
#endif //
#ifdef SC_PORT_MULTIPLIER
     SG_DSATA->rMULTI_PL.bits.dVAL = 1; // 1: 支持Multiplier
#endif
    SG_DSATA->rCFGR.bits.dVAL=0;
    SG_DSATA->rPCLBR.dAll=(U32)&tCmdHead[0];//1K对齐
    SG_DSATA->rPFBR.dAll=(U32)(&unFsmBuff.adBuff[0]);//32字节对齐DSATA_FSM_BUFF_ADDR;




    SG_DSATA->rHWCLR_EN.dAll=1;
    
    SG_DSATA->rPCMDR.bits.TXSAS=0;//禁止AUTO Send
    SG_DSATA->rPSCTLR.bits.IPM = LPM_DISABLE_BOTH;//禁用进入HOST发起的省电模式
    SG_DSATA->rNCQ_REORDER_STP.dAll = 0;
    //Clear error register (SERR).
    SG_DSATA->rPSERR.dAll=0xFFFFFFFF;//Clear Error reg

    DSATA_DMA1ModeInit();
    SG_CPM->rCOMRESET_CTRL.bits.COMRESET_EN=0;

#if (L_PWN_MNG_EN == 1)
    //TODO:省电模式分级别
    /*
     * CLK GATEING OFF > CLK GATEING ON
     * PATIAL > SLUMBER > SLUMBER_S > DEVSLP
     * */
   SG_CPM->rPHY_POWER_CTRL.bits.SLUMBER_S_EN=1;//允许进入SLUMBER
   SG_CPM->rPHY_POWER_CTRL.bits.SLUMBER_MODE=0;//1:SLUMBER_S,PHY CLK OFF,0 SLUMBER,PHY CLK ON

   SG_CPM->rSLEEP_CTRL.bits.SLUMBER_S_ENTER_EN=1;
   SG_CPM->rDEVSLP_CTRL.bits.DEVSLP_EN=0;//初始化默认为DISABLE
   SG_CPM->rSLEEP_CNT.bits.PAR_SLU_ENTER_CNT=1;
//   SG_CPM->rDSIN_FILTER_CTRL.bAll=0xF;

   SG_CPM->rDEVSLP_ENTER_MDT.bits.wVAL=25*150;//5ms
   SG_CPM->rDEVSLP_EXIT_MDT.bits.wVAL=25*150;//1ms
   SG_CPM->rCPM_INT_STS.bits.DEVSLP_EXIT_INT_STS=0;
   SG_CPM->rCPM_INT_STS.bits.DEVSLP_ENTER_INT_STS=0;
#if(L_DIPM_EN == 1)
    /*
    1. Firmware writes the PITIMER register (while PCMDR.ST=0) with a timeout value corresponding to
        the required delay for PDMA to request power management state:
        Delay = PITIMER value x Thclk, where Thclk - hclk period
    */
    SG_DSATA->rPITIMER=tSataDC.dMilliSecondCnt*20;//20ms总线没有动作就进入休眠
    SG_DSATA->rPHYNRDY_DLYTM.bits.dVAL=3;
    SG_DSATA->rPHYNRDY_DLYEN.bits.dVAL=1;
    SG_CPM->rPAWCTRL.bits.PAWEN =0;
    SG_CPM->rPAWCTRL.bits.PAWCNT =1;
#endif //#if(L_DIPM_EN == 1)           
       
#if(L_HIPM_EN == 1)
    SG_CPM->rPHY_POWER_CTRL.bits.PARTIAL_SLUMBER_CLK_GATING_EN=0;
    SG_DSATA->rPSCTLR.bits.IPM = LPM_ENABLE_BOTH;  //TODO:MCU 不进入省电模式
#endif //#if(L_HIPM_EN == 1)


#endif //#if (L_PWN_MNG_EN == 1)
    /*
    Firmware sets the PCMDR.APSTE bit only when the host enables this feature via SET
    FEATURES command.
    */


#ifndef   SC_H2DFIFO_EN
     SG_DSATA->rH2DBUF_RST.bits.BUFH2DEN  = 0;  //选择buff 形式存NCQ 命令
     SG_DSATA->rH2DADDR.dAll=(U32)&arrunHWD2HFS[0].adBuff[0];
#else
     SG_DSATA->rH2D_ADDR1 = (U32)&arrunHWD2HFS[0].adBuff[0];
     tSataDC.bCmdPushIdx = 0;
     SG_DSATA->rH2DBUF_RST.dAll = 0x03;
#ifdef  SC_WP_4K_PRE

     SG_DSATA->rPRE4K_1CMD.bits.dVAL = 1;  //开启只预收一条命令
	 DSATA_Clean_TAG();
	 SG_DSATA->rPRE_TRIG_DMABO = 0;
	 SG_DSATA->rACTIVE_SEND.bits.ACTIVE_SEND = FS_AUTO;			  // 预收4K 是否发生第一个DMA ACTIVE
	 SG_DSATA->rACTIVE_SEND.bits.PRD_COMP_INT = 0  ;			  //关中断
	 SG_DSATA->rFW_EN.bits.TRANS_RRDY_EN = 1;  // 在FIFO 满的状态Controller 是否响应host 的 Xrdy,
	 SG_DSATA->rFW_EN.bits.HW_PRIG_4K_EN = 1;  //打开 单条 4K 写 预先接收
#endif  //SC_WP_4K_PRE
#endif

#if (D2H_AUTO_SEND == TRUE)

	    SG_DSATA->rHWD2HDONE0=0;//异常处理接口中存在漏掉清除动作的情况
	    SG_DSATA->rHWD2HEN.dAll=1;
#else
    SG_DSATA->rHWD2HEN.bits.dVAL = 0;
#endif


#ifdef  SC_EXBUFF_EN
    SG_DSATA->rBUFF_EX_BADDR = (U32)&tpbsata_exbuff.bBuff[0];   //SATA Exbuff 的初始化 地址。
    SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 0;
    SG_DSATA->rCLR_EXBUF.bits.dVAL = 1;
    SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 1;

#if (SC_CRC_PASS_ST_DMA1 == EXBUFF_CRC_PASS)
    SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 0;  //关闭第一级CRC PASS 检验
    SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 1 ;  //打开第二级CRC PASS 检验    数据过了CRC 从BUFF 传到DMA1
#else
#if (SC_CRC_PASS_ST_DMA1 == FIRST_CRC_PASS )
    SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 0 ;   //关闭第二级CRC PASS 检验
    SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 1;   //打开第一级CRC PASS 检验    数据过了CRC 从FIFO 传到DMA1
#else
//    SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 0 ;   //关闭第二级CRC PASS 检验    buff
//    SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 0;   //关闭第一级CRC PASS 检验    fifo
#endif
#endif
#else
#if ( SC_CRC_PASS_ST_DMA1 == FIRST_CRC_PASS	)
    SG_DSATA->rFIFO_HOLD.dAll = 0;
    SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 0 ;   //关闭第二级CRC PASS 检验
    SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 1;   //打开第一级CRC PASS 检验    数据过了CRC 从FIFO 传到DMA1
#else
    SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 0 ;   //关闭第二级CRC PASS 检验    buff
    SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 0;   //关闭第一级CRC PASS 检验    fifo
#endif
#endif   // SC_EXBUFF_EN

    //Enable top-level interrupt enable (GCR.IE).
    SG_DSATA->rGCR.bits.IE=1;//使能中断
    SG_DSATA->rPIER.dAll=0xFFFFFF7F;

    //使能中断
//	#ifndef __DUAL_CORE__
//		NVIC_EnableIRQ(IRQ_SC);//SATA中断等到初始化完成以后打开
//	#else
//	#endif
#ifdef __SATA_MAXPERFORMER_TEST
    SG_DSATA->rERR_CLR.bits.DFISPATH_SEL= 1;
#endif
    NVIC_EnableIRQ(IRQ_CPM);
    NVIC_EnableIRQ(IRQ_T0);

}

void  DSATA_ExbuffAndCheckPoit_Ctrl(U8 bEN)
{
#ifndef __BOOT_CODE__
	if(bEN)
	{
#ifdef	SCSI_CHECK_PASS_IDX_ZERO
		if(SG_DSATA->rCRC_PASS_INDEX)
		{
			 PRINT_TXT("E:SCSI_IDX");
		}
#endif
#ifdef  SC_WP_4K_PRE
		SG_DSATA->rFW_EN.bits.HW_PRIG_4K_EN = 0;  //打开 单条 4K 写 预先接收
#endif
#ifdef  SC_EXBUFF_EN
		SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 0;
#if (SC_CRC_PASS_ST_DMA1 == EXBUFF_CRC_PASS)
		SG_DSATA->rFIFO_HOLD.dAll = 0;  // 解决私有命令下开启第一级放行点（FIFO 后），8K数据不能全收到FIFO 中的问题
		SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 1;  //关闭第一级CRC PASS 检验
		SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 0;  //打开第二级CRC PASS 检验    数据过了CRC 从BUFF 传到DMA1
#endif
#endif   // SC_EXBUFF_EN
	}
	else
	{
		SG_DSATA->rCRC_PASS_MIN = SG_DSATA->rCRC_PASS_INDEX;
#ifdef  SC_EXBUFF_EN
	    SG_DSATA->rBUFF_EX_BADDR = (U32)&tpbsata_exbuff.bBuff[0];   //SATA Exbuff 的初始化 地址。
	    SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 0;
	    SG_DSATA->rCLR_EXBUF.bits.dVAL = 1;
	    SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 1;
#if (SC_CRC_PASS_ST_DMA1 == EXBUFF_CRC_PASS)
		   SG_DSATA->rFIFO_HOLD.dAll = 0x2f;  // 恢复最优性能
		SG_DSATA->rFW_EN.bits.WAIT_CRC_WRDMA1_EN = 0;  //关闭第一级CRC PASS 检验
		SG_DSATA->rFW_EN.bits.RDEXBUF_PASCRC_EN = 1;  //打开第二级CRC PASS 检验    数据过了CRC 从BUFF 传到DMA1
#endif
#endif

#ifdef  SC_WP_4K_PRE

     SG_DSATA->rPRE4K_1CMD.bits.dVAL = 1;  //开启只预收一条命令
	 DSATA_Clean_TAG();
	 SG_DSATA->rPRE_TRIG_DMABO = 0;
	 SG_DSATA->rACTIVE_SEND.bits.ACTIVE_SEND = FS_AUTO;			  // 预收4K 是否发生第一个DMA ACTIVE
	 SG_DSATA->rACTIVE_SEND.bits.PRD_COMP_INT = 0  ;			  //关中断
	 SG_DSATA->rFW_EN.bits.TRANS_RRDY_EN = 1;  // 在FIFO 满的状态Controller 是否响应host 的 Xrdy,
	 SG_DSATA->rFW_EN.bits.HW_PRIG_4K_EN = 1;  //打开 单条 4K 写 预先接收
#endif  //SC_WP_4K_PRE

	}
#endif
}

// *****************************************************************************
// 函数名称: DSATA_ModuleInit
// 函数功能: SATA模块初始化函数
// 入口参数: NONE
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_ModuleInit()
{
#ifdef __BOOT_CODE__
    U8 *bDmaBuff=(U8*)tSataDC.tSataMI.dDmaBuffAddr;
    U32 i;
    for(i=0;i<512*2;i++)
    {	//review:只初始化2个SEC
        bDmaBuff[i]=SG_CPM->rWDT1_CUR_CNT+i;
    }
#endif
#ifdef _SATA_TRIGGER_CRC__
    Abnormal_SataTest_Init();
#endif
    if (0 == (SG_CPM->rSYS_RESERVED0&0x00000002))
    {
       SG_CPM->rSYS_RESERVED0|=0x00000002;
       tSataDC.dPowerOnTime=0;//power on reset 归零
       //SATA_DevCtx_Init(1);//这个初始化交给comreset 中断
#ifdef __FPGA_PLATEFORM
       DSATA_Init(0);//由于要更改MAX speed，所以进行PHYRESET
#else
    #ifdef __SATA_TEST
           DSATA_Init(0);
    #else
        #if(SATA_LOG_ENABLE == 0)
           SG_CPM->rPRE_CLK_SOURCE_SEL=0x02;//选择25M
           SG_CPM->rSYS_CLK_SOURCE_SEL=1;//选择pre clk

           SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0xF;//offline
           Delay(7500000);
           SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0;//normal
           Delay(1000);
           SystemClkRecovery();//PHY时钟稳定后，恢复时钟配置

           DSATA_Init(0);
//               DSATA_Init(1);
        #else
               if(tSataDC.tSataMI.tMpCfg.bits.SATA_SPD == 0 || tSataDC.tSataMI.tMpCfg.bits.SATA_SPD == 3)
               {
                   //default GEN3
                   DSATA_Init(1);
               }
               else
               {
                   DSATA_Init(0);//reset PHY
               }
        #endif
    #endif
#endif
       TQM_Init();//任务队列初始化
       TPM_Init();//Task process module
       DMAC_DMA1Init();
   }
   else
   {

       //将SATA关键的处理接口移交给当前执行中的COED的接口
       tSataDC.pfTransferCompleteCb=NULL;
       tTQC.bIsNcqCmd=FALSE;
//       tSataDC.bIsWrCacheEnabled=FALSE;
       //tSataDC.bIsHWD2HEnabled = FALSE;
       tSataDC.pfTaskCompleteCb=NULL;
#ifndef __NCQ_QUEUE_NEW__
       tSataDC.qwTaskCnt=0;
#endif

       //tSataDC.bIsIdyOverlay =TRUE;
//       if (SG_DSATA->rHWD2HEN.bits.dVAL)
//       {
//            tSataDC.bIsHWD2HEnabled = TRUE;
//       }
       TQM_Init();
       TPM_Init();
       SATA_ChangeState(tSataDC.eTransType,tSataDC.unDevTransSts.bState);
#if(SATA_LOG_ENABLE == 0)
       NVIC_EnableIRQ(IRQ_SC);
#endif
       NVIC_EnableIRQ(IRQ_CPM);
       NVIC_EnableIRQ(IRQ_T0);
   }
}
void StanbyEventDone(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_NONE_EVENT;
    __pevent->pfEventCompleteCb=NULL;
    if(__pevent->eStatus != CMD_ABORT)
    {
        tSataDC.eDPMSts=DPS_STANBY;
    }
    //TODO:g0做一些自己相关的操作
    PRINT_TXT("STANBY_DONE");
}
#ifndef __G3_FLYCODE
// *****************************************************************************
// 函数名称: DSATA_SysTickProcess
// 函数功能: SATA 模块systick 任务
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_SysTickProcess ()
{
    tSataDC.dPowerOnTime+=SYSTICK_PERIOD;

#ifndef __BOOT_CODE__
#ifdef  __ALG_FORMAL
    if(tSataDC.tSataMI.pSysTick_Event_cb !=NULL && tSataUDP.m_dIsValid==SATA_CB_VALID_FLAG)
#else
    if(tSataDC.tSataMI.pSysTick_Event_cb !=NULL && tSataDC.tSataMI.dIsValid==SATA_CB_VALID_FLAG)
#endif
    {
#ifdef __DUAL_CORE__
        GIE_DISABLE();
        tSataDC.tSataMI.pSysTick_Event_cb();
        GIE_ENABLE();
#else
        tSataDC.tSataMI.pSysTick_Event_cb();
#endif
    }

    /*
     * The Standby timer provides a method for the device to enter Standby mode from either Active mode or Idle mode
        following a host programmed period of inactivity. If:
        a) the Standby timer is enabled;
        b) the device is in the Active mode or the Idle mode; and
        c) the Standby timer expires,
        then the device enters the Standby mode if no media access command is received.
        If a media access command is received and the Standby timer is enabled, the Standby timer is:
        a) reinitialized to the value specified by the most recent IDLE command (see 7.14) or STANDBY command
        (see 7.49); and
        b) started.
        If the Standby timer is disabled, the device may automatically enter Standby mode after a vendor specific time
        has expired for a vendor specific reason
     * */
    if(tSataDC.bIsStandyTimerOn && (tSataDC.eDPMSts == DPS_IDLE || tSataDC.eDPMSts == DPS_ACTIVE))
    {
        if(tSataDC.dStanbyTimerCnt >= tSataDC.dStanbyTimerNum/*&& TODO:具体实现可能还需要增加其他判断*/)
        {
            if(tSataDC.tSyncEvent.bOpcode == OP_NONE_EVENT && tSataDC.tSyncEvent.pfEventCompleteCb==NULL)
            {
                tSataDC.tSyncEvent.bOpcode=OP_DI_STANBY_REQ;
                tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
                tSataDC.tSyncEvent.pfEventCompleteCb=StanbyEventDone;
                PRINT_TXT("STANBY_REQ");
            }
            else
            {
                PRINT_TXT("STANBY_REQ_N");
            }
            tSataDC.dStanbyTimerCnt=0;
        }
        else
        {
            tSataDC.dStanbyTimerCnt+=SYSTICK_PERIOD;
        }
    }
#endif

#if (SATA_LOG_ENABLE == 1)
    tSataDC.dMilliSecAcc+=SYSTICK_PERIOD;//1小时毫秒计时器
    if((tSataDC.dPowerOnTime%tSataDC.dSataLogUpdateCount) == 0)
    {
        //重上电1S以后开始保存LOG，主要是为了记录上电次数信息
        tSataDC.dSataLogUpdateCount=LOG_UPDATE_INTERVAL;
        tSataLogCtx.enGplUpdateEvent=GPL_E_TIMEOUT;
        tSataLogCtx.bUpdateEn=TRUE;
    }

#ifdef SATA_RECNT_ENABLE
    if((tSataDC.bFirstH2dCmd == 0xFF) && ((tSataDC.dPowerOnTime - tSataDC.dNDCTOGuardTimer) > 20000) && (tSataLogCtx.dSataReconectMode != 0xDEADDEAD))
    {
    	//上电后20S，都没有收到第一个H2D 命令，FF表示打过第一个D2H以后的状态
    	PRINT_TXT("ENTER REC MODE");
    	tSataLogCtx.dSataReconectMode = 0xDEADDEAD;
    	WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存下tSataLogCtx.dSataReconectMode的状态
    }
#endif

#if 0
    if((tSataDC.dMilliSecAcc %(10U * MILLSEC_OF_ONE_MIN))==0)
    {
        //每隔10分钟需要读取一次温度值
        U8 Temperature=30;
        tSataLogCtx.dShortTempAcc+=Temperature;//TODO：调用接口获取当前温度

        //读到温度以后要比较，记录最高，最低温度
        if(0xFF == tSataLogCtx.bHighestTemp)
        {
            //出厂值,直接赋值
            tSataLogCtx.bHighestTemp=Temperature;
        }
        else
        {
            //记录最大值
            if(tSataLogCtx.bHighestTemp < Temperature)
            {
                tSataLogCtx.bHighestTemp = Temperature;
            }
        }

        if(0xFF == tSataLogCtx.bHighestTemp)
        {
            //出厂值,直接赋值
            tSataLogCtx.bLowestTemp=Temperature;
        }
        else
        {
            //记录最小值
            if(tSataLogCtx.bLowestTemp > Temperature)
            {
                tSataLogCtx.bLowestTemp = Temperature;
            }
        }

        //更新超出温度限定的次数
         if(tSataLogCtx.bMaxOpTemp < Temperature)
         {
             //更新超出最高操作温度的次数
             tSataLogCtx.dOvTempNums++;
         }

         if(tSataLogCtx.bMinOpTemp > Temperature)
         {
             //更新低于最低操作温度的次数
             tSataLogCtx.dUnDerTempNums++;
         }
      }

    //读写任务需要错开
    if(tSataDC.dMilliSecAcc >= MILLSEC_OF_ONE_HOUR)
    {
        //累积满1个小时
        tSataDC.dLifePowerOnTime++;
        if((tSataDC.dLifePowerOnTime%24) == 0)
        {
            //每24小时更新bAverangeShortTemp
            tSataLogCtx.bAverangeShortTemp=(tSataLogCtx.dShortTempAcc/144);
            //更新平均值里的最大值
            if(0xFF == tSataLogCtx.bHighestAverangeShortTemp)
            {
                //出厂值,直接赋值
                tSataLogCtx.bHighestAverangeShortTemp=tSataLogCtx.bAverangeShortTemp;
            }
            else
            {
                //记录最大值
                if(tSataLogCtx.bHighestAverangeShortTemp < tSataLogCtx.bAverangeShortTemp)
                {
                    tSataLogCtx.bHighestAverangeShortTemp = tSataLogCtx.bAverangeShortTemp;
                }
            }

            //更新平均值里的最小值
            if(0xFF == tSataLogCtx.bLowestAverangeShortTemp)
            {
                //出厂值,直接赋值
                tSataLogCtx.bLowestAverangeShortTemp=tSataLogCtx.bAverangeShortTemp;
            }
            else
            {
                //记录最大值
                if(tSataLogCtx.bLowestAverangeShortTemp > tSataLogCtx.bAverangeShortTemp)
                {
                    tSataLogCtx.bLowestAverangeShortTemp = tSataLogCtx.bAverangeShortTemp;
                }
            }

            tSataLogCtx.dLongTempAcc+=tSataLogCtx.bAverangeShortTemp;
            tSataLogCtx.dShortTempAcc=0;
        }
        if((tSataDC.dLifePowerOnTime%1008) == 0)
        {
            //每1008小时更新bAverangeLongTemp
            tSataLogCtx.bAverangeLongTemp= (tSataLogCtx.dLongTempAcc/42);
             //更新平均值里的最大值
            if(0xFF == tSataLogCtx.bHighestAverangeLongTemp)
            {
                //出厂值,直接赋值
                tSataLogCtx.bHighestAverangeLongTemp=tSataLogCtx.bAverangeLongTemp;
            }
            else
            {
                //记录最大值
                if(tSataLogCtx.bHighestAverangeLongTemp < tSataLogCtx.bAverangeLongTemp)
                {
                    tSataLogCtx.bHighestAverangeLongTemp = tSataLogCtx.bAverangeLongTemp;
                }
            }

            //更新平均值里的最小值
            if(0xFF == tSataLogCtx.bLowestAverangeLongTemp)
            {
                //出厂值,直接赋值
                tSataLogCtx.bLowestAverangeLongTemp=tSataLogCtx.bAverangeLongTemp;
            }
            else
            {
                //记录最大值
                if(tSataLogCtx.bLowestAverangeLongTemp > tSataLogCtx.bAverangeLongTemp)
                {
                    tSataLogCtx.bLowestAverangeLongTemp = tSataLogCtx.bAverangeLongTemp;
                }
            }
            tSataLogCtx.dLongTempAcc=0;
        }
        //要不要立即写入FLASH？否则突然掉电就丢掉一个小时统计
        tSataDC.dMilliSecAcc=0;
        tSataLogCtx.enGplUpdateEvent=GPL_E_TIMEOUT;
        tSataLogCtx.bUpdateEn=TRUE;
    }
#else
    if(tSataDC.dMilliSecAcc >= MILLSEC_OF_ONE_HOUR)
    {
        tSataDC.dLifePowerOnTime++;
        tSataDC.dMilliSecAcc=0;
    }
#endif

#endif
}
#endif

#if (SATA_LOG_ENABLE == 1)
BOOL IsNeedUpdateGpl(void)
{
    //更新GPL 后台数据
    if(tSataLogCtx.enGplUpdateEvent == GPL_E_TIMEOUT || tSataLogCtx.enGplUpdateEvent == GPL_E_RESTART)
    {
        //发生更新超时事件，且此时没有DI任务进行,立即更新log 数据
        return TRUE;
    }
    else if(SG_MCU->rSF0_INT1_STS0 == 0 && SG_MCU->rSF1_INT1_STS0==0 && SG_MCU->rSF0_INT1_STS1 == 0)
    {
        //没有 pending以及进行中的的HOST任务
        if(tSataLogCtx.enGplUpdateEvent == GPL_E_RW)
        {
            //host 数据读写触发的gpl log更新操作
            if((tSataDC.dPowerOnTime-tSataLogCtx.dE_RWTimer) >=100)//总线IDLE时间100ms
            {
                //在没有host 任务之1S以后再更新
                return TRUE;
            }
        }
        else
        {
            tSataLogCtx.dE_RWTimer=tSataDC.dPowerOnTime;
        }
    }
    else
    {
        tSataLogCtx.dE_RWTimer=tSataDC.dPowerOnTime;
    }
    return FALSE;
}
#endif
#ifndef __G3_FLYCODE
void DSATA_G3_TaskRouter(void)
{
#ifdef  __ALG_FORMAL
    if(tSataUDP.m_dIsValid==SATA_CB_VALID_FLAG)
#else
    if(tSataDC.tSataMI.dIsValid==SATA_CB_VALID_FLAG)
#endif
    {
        U32 primask=__get_PRIMASK();
        __set_PRIMASK(1);

         if(tSataDC.tSataMI.pReadRetry_Process != NULL)
         {
             tSataDC.tSataMI.pReadRetry_Process();
         }

         if(tSataDC.tSataMI.pFtlTask_Process != NULL)
         {
             tSataDC.tSataMI.pFtlTask_Process();
         }

         __set_PRIMASK(primask);
    }
}


// *****************************************************************************
// 函数名称: DSATA_Module_Process
// 函数功能: SATA 模块主循环
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_ModuleProcess ()
{
    //SATA 协议主循环处理
    if(tSataDC.DPC_Process != NULL)
    {
        tSataDC.DPC_Process();
    }
    //scsi PROCESS
    if (tScsiCtl.bVaild)
    {
        Scsi_IdleCmdHandle(tScsiCtl.bDir);
    }

    //SATA任务解析
    TPM_StateMachine();

#if (SATA_LOG_ENABLE == 1)
     //中断中会改写tSataLogCtx.bUpdateEn，如果没有屏蔽中断则有可能丢失TRUE状态
     if(tSataLogCtx.bUpdateEn)
     {
         if(IsNeedUpdateGpl())
         {
             GIE_DISABLE();
             WriterGPLLogsRouter(CMD_IDLE,NULL);
             tSataLogCtx.bUpdateEn=FALSE;
             GIE_ENABLE();
         }
     }
     Sanitize_Device_state_machine();
#endif
#ifndef __BOOT_CODE__
     DSATA_G3_TaskRouter();
#endif
}
#endif
// *****************************************************************************
// 函数名称: DSATA_UpdateSystemClockInfo
// 函数功能: 获取平台系统时钟信息初始化SATA时间相关变量
// 入口参数: sys_clk 系统时钟
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_UpdateSystemClockInfo (U32 sys_clk)
{
    tSataDC.dSystemFrequency =sys_clk;
    tSataDC.dMilliSecondCnt =tSataDC.dSystemFrequency/1000;//更新毫秒计数
    tSataDC.dMicroSecondCnt=tSataDC.dMilliSecondCnt/1000;//确定微妙计时数
}
// *****************************************************************************
// 函数名称: DSATA_RegisteInterfaceInfo(T_SATA_MOD_ITF *ptSataMI)
// 函数功能: 获取操作锁
// 入口参数: ptSataMI :SATA 模块上下文
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_RegisteInterfaceInfo(T_SATA_MOD_ITF *ptSataMI)
{
#ifdef __BOOT_CODE__
	Abnormal_PowerOnInit();
#endif
#ifndef __DUAL_CORE__
    //单核
    tSataDC.tSataMI.dDmaBuffAddr =ptSataMI->dDmaBuffAddr;
    tSataDC.tSataMI.dDmaBuffSize =ptSataMI->dDmaBuffSize;
#ifndef __BOOT_CODE__
    tSataDC.tSataMI.dVirtualLbaStartAddr=ptSataMI->dVirtualLbaStartAddr;
    tSataDC.tSataMI.tMpCfg.dAll=ptSataMI->tMpCfg.dAll;
    if(tSataDC.tSataMI.dVirtualLbaStartAddr == 0)
#endif
    {
        tSataDC.tSataMI.dVirtualLbaStartAddr=DEVICE_CAPCITY;
    }
    //tSataDC.bIsIdyOverlay =TRUE;
    tSataDC.tSataMI.pSysTick_Event_cb=ptSataMI->pSysTick_Event_cb;
#if(L_PWN_MNG_EN == TRUE)
    tSataDC.tSataMI.pDeepSleepEntry_Event_cb=ptSataMI->pDeepSleepEntry_Event_cb;
    tSataDC.tSataMI.pDeepSleepExit_Event_cb=ptSataMI->pDeepSleepExit_Event_cb;
#endif
    tSataDC.tSataMI.pPHYLinkChangeEvent_cb = ptSataMI->pPHYLinkChangeEvent_cb;
    tSataDC.tSataMI.pInterfaceFatalErrorEvent_cb = ptSataMI->pInterfaceFatalErrorEvent_cb;
    tSataDC.tSataMI.pRemove_AllTask_cb=ptSataMI->pRemove_AllTask_cb;
#ifndef __BOOT_CODE__
    tSataDC.tSataMI.pReadRetry_Process=ptSataMI->pReadRetry_Process;
    tSataDC.tSataMI.pSystemExceptionEvent_cb=ptSataMI->pSystemExceptionEvent_cb;
    tSataDC.tSataMI.pFtlTask_Process=ptSataMI->pFtlTask_Process;
    if(ptSataMI->pTaskQueuedEvent_cb != NULL)
    {
        tSataDC.tSataMI.pTaskQueuedEvent_cb = ptSataMI->pTaskQueuedEvent_cb;
    }
    else
#endif
    {
        tSataDC.tSataMI.pTaskQueuedEvent_cb = TaskQueuedEventDefaultHandle;
    }
    //tSataDC.dPowerOnTime=0;
    tSataDC.tSataMI.dIsValid=SATA_CB_VALID_FLAG;
#if(SATA_LOG_ENABLE == 1)
    tSataDC.dSataLogUpdateCount=LOG_UPDATE_INTERVAL;
#endif
    //tSataDC.bIsIdyOverlay =TRUE;
#ifndef __BOOT_CODE__
    tSataDC.wNcqTOCnt=1;
    tSataDC.bNcqBrustTH=20;
    tSataDC.bNcqMaxBrustCnt=5;
    tSataDC.bGenxSpeed=0xFF;
    tSataDC.bIsDcoFreezeLocked=FALSE;//poweron 才释放
    tSataDC.eDPMSts=DPS_ACTIVE;
    tSataDC.dStanbyTimerCnt=0;
    tSataDC.bIsStandyTimerOn = FALSE;
    tSataDC.bSecPwdAttemptsCnt=5;
    tSataDC.bIsSecPwdAttemptsExceeded=FALSE;
    tSataDC.bIsSecErasePrepared=FALSE;
    tSataDC.bCmdCnt=0;

    tSataDC.bPHYRESET=0;
    tSataDC.__sda_bas=__get_sda_base();
#if(SATA_LOG_ENABLE == 0)
    tSataDC.tSDState = S_SD0;
    tSataDC.bSDFMPVal=0;
    tSataDC.bSDOCWOEVal=0;
    tSataDC.wSanitizeProCnt=0xFFFF;
    tSataDC.dIsNDCTOGuardEn=FALSE;
#endif

#endif


    tSataDC.bIsWrCacheEnabled=FALSE;
    tSataDC.pfTaskCompleteCb=NULL;

    tSataDC.tSyncEvent.bOpcode=OP_NONE_EVENT;
    tSataDC.tSyncEvent.pfEventCompleteCb=NULL;
    tSataDC.bIsHIPMEn=TRUE;
    tSataDC.tAsyncEvent.bOpcode=OP_DI_WAIT4_FTL_INIT_DONE;
    tSataDC.tAsyncEvent.eStatus=CMD_IDLE;
#if (SATA_LOG_ENABLE == 1)
    tSataDC.tAsyncEvent.pfEventCompleteCb=ReadGPLLogsRouter;
#else
    tSataDC.tAsyncEvent.pfEventCompleteCb=NULL;
#endif

#if (SATA_LOG_ENABLE == 1)
    tSataLogCtx.enGplUpdateEvent=GPL_E_NONE;
    tSataLogCtx.bIsGplRunning=FALSE;
    tSataLogCtx.bIsSelfTestRunning=FALSE;
    tSataLogCtx.bUpdateEn=FALSE;
    tSataLogCtx.bIsResetEveCnt=FALSE;
    tSataDC.bFirstH2dCmd=0;
#endif

    if ((0 == (SG_CPM->rSYS_RESERVED0&0x00000002)))
    {
        tSataDC.DPC_Process=NULL;
        tSataDC.INT_Process=NULL;
        tSataDC.DOS_TransferReq=NULL;
        tSataDC.unDevTransSts.eDDR=0;
        tSataDC.eLPMSts=LPS_OFF_LINE;
        tSataDC.eTransType =UNDEFINE;
    }
#else
#ifndef __G3_FLYCODE
    tSataDC.tSataMI.pTaskQueuedEvent_cb=NULL;
    tSataDC.tSataMI.dVirtualLbaStartAddr=ptSataMI->dVirtualLbaStartAddr;
    tSataDC.tSataMI.tMpCfg.dAll=ptSataMI->tMpCfg.dAll;
#if(SATA_LOG_ENABLE == 1)
    tSataDC.dSataLogUpdateCount=LOG_UPDATE_INTERVAL;
#endif
    //tSataDC.bIsIdyOverlay =TRUE;
    tSataDC.bGenxSpeed=0xFF;
    tSataDC.bIsDcoFreezeLocked=FALSE;//poweron 才释放
    tSataDC.eDPMSts=DPS_ACTIVE;
    tSataDC.dStanbyTimerCnt=0;
    tSataDC.bIsStandyTimerOn = FALSE;
    tSataDC.bSecPwdAttemptsCnt=5;
    tSataDC.bIsSecPwdAttemptsExceeded=FALSE;
    tSataDC.bIsSecErasePrepared=FALSE;
    tSataDC.bCmdCnt=0;
    tSataDC.wNcqTOCnt=1;
    tSataDC.bNcqBrustTH=20;
    tSataDC.bNcqMaxBrustCnt=5;
    tSataDC.bSaveUartBuff=FALSE;
    tSataDC.__sda_bas=__get_sda_base();
    tSataDC.dIsNDCTOGuardEn=FALSE;
    tSataDC.bPHYRESET=0;
#if(SATA_LOG_ENABLE == 0)
    tSataDC.tSDState = S_SD0;
    tSataDC.bSDFMPVal=0;
    tSataDC.bSDOCWOEVal=0;
    tSataDC.wSanitizeProCnt=0xFFFF;
#endif

    tSataDC.bIsWrCacheEnabled=FALSE;
    tSataDC.pfTaskCompleteCb=NULL;

    tSataDC.tSyncEvent.bOpcode=OP_NONE_EVENT;
    tSataDC.tSyncEvent.pfEventCompleteCb=NULL;
    tSataDC.bIsHIPMEn=FALSE;
    tSataDC.tAsyncEvent.bOpcode=OP_DI_WAIT4_FTL_INIT_DONE;
    tSataDC.tAsyncEvent.eStatus=CMD_IDLE;
#if (SATA_LOG_ENABLE == 1)
    tSataDC.tAsyncEvent.pfEventCompleteCb=ReadGPLLogsRouter;
#else
    tSataDC.tAsyncEvent.pfEventCompleteCb=NULL;
#endif

#if (SATA_LOG_ENABLE == 1)
    tSataLogCtx.enGplUpdateEvent=GPL_E_NONE;
    tSataLogCtx.bIsGplRunning=FALSE;
    tSataLogCtx.bIsSelfTestRunning=FALSE;
    tSataLogCtx.bUpdateEn=FALSE;
    tSataLogCtx.bIsResetEveCnt=FALSE;
    tSataDC.bFirstH2dCmd=0;
#endif

    if (0 == (SG_CPM->rSYS_RESERVED0&0x00000002))
    {
        tSataDC.DPC_Process=NULL;
        tSataDC.INT_Process=NULL;
        tSataDC.DOS_TransferReq=NULL;
        tSataDC.unDevTransSts.eDDR=0;
        tSataDC.eLPMSts=LPS_OFF_LINE;
        tSataDC.eTransType =UNDEFINE;//初始化状态
    }
#else
    tSataDC.tSataMI.dDmaBuffAddr =ptSataMI->dDmaBuffAddr;
    tSataDC.tSataMI.dDmaBuffSize =ptSataMI->dDmaBuffSize;
#if(L_PWN_MNG_EN == TRUE)
    tSataDC.tSataMI.pDeepSleepEntry_Event_cb=ptSataMI->pDeepSleepEntry_Event_cb;
    tSataDC.tSataMI.pDeepSleepExit_Event_cb=ptSataMI->pDeepSleepExit_Event_cb;
#endif
    tSataDC.tSataMI.pSysTick_Event_cb=ptSataMI->pSysTick_Event_cb;
    tSataDC.tSataMI.pPHYLinkChangeEvent_cb = ptSataMI->pPHYLinkChangeEvent_cb;
    tSataDC.tSataMI.pInterfaceFatalErrorEvent_cb = ptSataMI->pInterfaceFatalErrorEvent_cb;
    tSataDC.tSataMI.pRemove_AllTask_cb=ptSataMI->pRemove_AllTask_cb;
    tSataDC.tSataMI.pReadRetry_Process=ptSataMI->pReadRetry_Process;
    tSataDC.tSataMI.pFtlTask_Process=ptSataMI->pFtlTask_Process;
    tSataDC.tSataMI.pSystemExceptionEvent_cb=ptSataMI->pSystemExceptionEvent_cb;
    tSataDC.tSataMI.dIsValid=SATA_CB_VALID_FLAG;
#ifndef __ALG_FORMAL
    g0_enable();
#endif
#endif

#endif

}

// *****************************************************************************
// 函数名称: DSATA_StartupEnable
// 函数功能: SATA启动
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void DSATA_StartupEnable()
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    SG_DSATA->rPSERR.dAll=0xFFFFFFFF;//Clear Error reg
    //TODO:可以合并? --有些比特位不能一起置位
    SG_DSATA->rPCMDR.dAll=0;
    SG_DSATA->rPCMDR.bits.STD=1;//CLEAR Startup Disabled - STD
    SG_DSATA->rPCMDR.bits.ST=1;//ENABLE dsata
    SG_DSATA->rPCMDR.bits.FRXP=1;//Exit FIS Rx Pause state

    SG_DSATA->rPISR.dAll=0xFFFFFFFF;//Clear interrupt status (PISR <- 0xffffffff).
    SG_DSATA->rPIER.dAll=0x7EFFFF5D;  // SG_DSATA->rPIER.dAll=0xFFFFFF7F;
//
//    SG_DSATA->rPIER.bits.FTXDE=0;
//    SG_DSATA->rPIER.bits.OFE=0;//禁用OFE中断,FATAL error 中已经处理
//    SG_DSATA->rPIER.bits.TXOKE=0;//禁用TXOKE中断，未使用
//    SG_DSATA->rPIER.bits.REORDSTE=0;
    SG_DSATA->rNCQ_REORDER_STP.dAll = 0;

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: DSATA_LinkCheck
// 函数功能: 检查链路层状态
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
BOOL DSATA_LinkCheck()
{
    //Is the interface in active state (PSSTRS.IPM == 0x1)
    if(SG_DSATA->rPSSTSR.bits.IPM != 0x01)
    {
        return FALSE;
    }
    //Has the link speed been negotiated (PSSTRS.SPD != 0x0)
    if(0x00 == SG_DSATA->rPSSTSR.bits.SPD)
    {
        return FALSE;
    }
    //Has Host presence been detected and PHY communication established (PSSTRS.DET == 0x3)
    if(SG_DSATA->rPSSTSR.bits.DET != 0x03)
    {
        return FALSE;
    }
    return TRUE;
}
// *****************************************************************************
// 函数名称: DSATA_ClearFsmBuff
// 函数功能: 清除FIS缓冲区
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_ClearFsmBuff()
{
	U8 bi ;
	for(bi = 0; bi< 6; bi++)
	{
		 unFsmBuff.adBuff[bi]=0;
	}
//    unFsmBuff.adBuff[0]=0;
//    unFsmBuff.adBuff[1]=0;
//    unFsmBuff.adBuff[2]=0;
//    unFsmBuff.adBuff[3]=0;
//    unFsmBuff.adBuff[4]=0;
//    unFsmBuff.adBuff[5]=0;
//    unFsmBuff.adBuff[6]=0;

}

// *****************************************************************************
// 函数名称: DSATA_BuildD2HFis
// 函数功能: 配置D2H
// 入口参数:
//           status:d2h的状态字段
//           error：error字段
//           I：中断
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_BuildD2HFis(U8 bStatus,U8 bError,U8 I)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //if(tTQC.bIsNcqCmd == FALSE || tSataDC.bIsHWD2HEnabled == FALSE)
    {
        //DSATA_ClearFsmBuff();
        unFsmBuff.FIS.d2h.dw0.bits.bType=FIS_CODE[D2H_FIS];

        unFsmBuff.FIS.d2h.dw0.bits.I=I;
        unFsmBuff.FIS.d2h.dw0.bits.Reserved1=0;
        unFsmBuff.FIS.d2h.dw0.bits.Reserved2=0;
#ifdef SC_PORT_MULTIPLIER
        unFsmBuff.FIS.d2h.dw0.bits.PM_port= tSataDC.bPM;
#endif
        unFsmBuff.FIS.d2h.dw0.bits.bStatus=bStatus;
        unFsmBuff.FIS.d2h.dw0.bits.bError=bError;
    }
}

// *****************************************************************************
// 函数名称: DSATA_SendD2HFis
// 函数功能: 配置并发送D2H
// 入口参数:
//           status:d2h的状态字段
//           error：error字段
//           I：中断
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_SendD2HFis(U8 bStatus,U8 bError,U8 I)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

#if(SATA_LOG_ENABLE)
    if(tSataLogCtx.dCmdFailDue2ICRC < 0xFFFFFFFF)
    {
        if(bError==CMD_CRC_ERR)
        {
            tSataLogCtx.dCmdFailDue2ICRC++;//CRC 错统计
            WriterGPLLogsRouter(CMD_IDLE,NULL);//更新状态
        }
    }
#endif

    DSATA_BuildD2HFis(bStatus,bError,I);
    DSATA_TrigeSendFis(D2H_FIS);

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: DSATA_BulidSDBFis
// 函数功能: 配置SDB
// 入口参数:
//          SDB相关字段
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_BulidSDBFis(U8 status_Hi,U8 status_Lo,U8 bError,U8 I,U32 dProtocol_spec)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    unFsmBuff.FIS.sdb.dw0.bits.bType=FIS_CODE[SDB_FIS];
    unFsmBuff.FIS.sdb.dw0.bits.I=I;
#ifdef SC_PORT_MULTIPLIER
    unFsmBuff.FIS.sdb.dw0.bits.PM_port= tSataDC.bPM;
#endif
    unFsmBuff.FIS.sdb.dw0.bits.Status_Hi=status_Hi;
    unFsmBuff.FIS.sdb.dw0.bits.Status_Lo=status_Lo;
    unFsmBuff.FIS.sdb.dw0.bits.bError=bError;
    unFsmBuff.FIS.sdb.dw1.dAll=dProtocol_spec;

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: DSATA_BulidPioSetUpFis
// 函数功能: 配置PIO_SETUP
// 入口参数:
//          dir:数据传输方向,
//          size:大小
// 出口参数: 本次传输数据大小
// 说    明:
// *****************************************************************************
void DSATA_BulidPioSetUpFis(U8 bDir,U8 bSize)
{
    //准备缓冲区数据
    //DSATA_ClearFsmBuff();
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

    if(!DSATA_IsSlotStarted(CMD_SLOT0))
    {
        //该SLOT还未启动，属于第一次配置
        DEBUG_PRINT("First time pio setup\r\n");
        unFsmBuff.FIS.pio_setup.dw0.dAll=0;
        unFsmBuff.FIS.pio_setup.dw0.bits.bType=FIS_CODE[PIO_SETUP_FIS];
        unFsmBuff.FIS.pio_setup.dw0.bits.D=bDir;
#ifdef SC_PORT_MULTIPLIER
        unFsmBuff.FIS.pio_setup.dw0.bits.PM_port= tSataDC.bPM;
#endif
        unFsmBuff.FIS.pio_setup.dw0.bits.bStatus=0x58;

        unFsmBuff.FIS.pio_setup.dw4.bits.wTrascnt=(U16)(bSize<<9);//NOTE：bSize=256的时候会溢出
        unFsmBuff.FIS.pio_setup.dw3.bits.bE_status=0xD0;
        if(bDir == DIR_IN)
        {
            //读每一个I要置位
            unFsmBuff.FIS.pio_setup.dw0.bits.I=1;
            if(1U == tCmdHead[CMD_SLOT0].dw0.cmd.PRDTL)
            {
                //PIO OUT mode 是使用最后一个D2H来表示传输完成的，所以要保持D0--第一次也是最后一次
                unFsmBuff.FIS.pio_setup.dw3.bits.bE_status=0x50;
            }
        }
        else
        {
        }
    }
    else
    {
        DEBUG_PRINT("DSATA_GetPrdRemainItems=%d\r\n",DSATA_GetPrdRemainItems());
        unFsmBuff.FIS.pio_setup.dw0.bits.I=1;
        if(DSATA_GetPrdRemainItems() == 1U)
        {
            DEBUG_PRINT("Last time pio setup\r\n");
            //肯定是最后一次传输清除BSY,DRQ位，重新改写需要改写的字段
            unFsmBuff.FIS.pio_setup.dw4.bits.wTrascnt=(U16)(bSize<<9);//NOTE：bSize>128的时候会溢出

            if(bDir == DIR_IN)
            {
                //PIO IN mode 是使用最后一个完成状态在pio setup中
                unFsmBuff.FIS.pio_setup.dw3.bits.bE_status=0x50;
            }
        }
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: DSATA_BuildPrd
// 函数功能: 配置PRD表格
// 入口参数:
//          I/W:中断位
//            ptParam：param
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_BuildPrd(T_HI_TASK *ptTask,U8 bI)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //tPrdTbls[ptTask->tParam.bTag].dDBAU=0;
    //tPrdTbls[ptTask->tParam.bTag].dReserved=0;

    tPrdTbls[ptTask->tParam.bTag].dw3.dAll=0x40000000;//tPrdTbls[bSlot].dw3.bits.W=1;
    //通过中断发送状态
    tPrdTbls[ptTask->tParam.bTag].dw3.bits.I=bI;

    tPrdTbls[ptTask->tParam.bTag].dDBA=ptTask->tParam.dBuffAddr;

    tPrdTbls[ptTask->tParam.bTag].dw3.bits.DBC=(ptTask->tParam.dSctSize<<9) -1;

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: DSATA_SetCmdHead
// 函数功能: cmd head 配置接口
// 入口参数: slot:命令槽
//             di:Description Information
//             sector:数据扇区数
//             dmact:dmaCnt:DMA Transfer Count
//             dmabo:DMA Buffer Offset
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_SetCmdHead(CMD_SLOT bSlot,UN_CH_DI unDI,U32 dSector,U32 dDrq,U32 dDmatc)
{

    tCmdHead[bSlot].dw0.dAll=unDI.dAll;
#ifdef SC_PORT_MULTIPLIER
   	U8 bPM = tSataDC.bPM  ;
    tCmdHead[bSlot].dw0.cmd.PM =  bPM;  //不能调换位置
#endif
    if(dDrq == dSector)
    {
        //避开除法操作
        tCmdHead[bSlot].dw0.cmd.PRDTL=1;
    }
    else if(dDrq == 1)
    {
        //避开除法操作
        tCmdHead[bSlot].dw0.cmd.PRDTL=dSector;
    }
    else
    {
        //正常操作
        tCmdHead[bSlot].dw0.cmd.PRDTL=(U32)((dSector+dDrq-1)/dDrq) /*==>(sector/drq)+ ((sector%drq)!=0)*/;
    }

    tCmdHead[bSlot].dDMATC=dDmatc;
    //tCmdHead[bSlot].dDMABO=dDmabo;//默认初始化0
    //tCmdHead[bSlot].dPRDTBA=(U32)&tPrdTbls[bSlot]; //默认初始化好了
}
// *****************************************************************************
// 函数名称: DSATA_TrigeSendFis
// 函数功能: 上层调用该接口发送某一类型FIS，屏蔽掉硬件操作
// 入口参数: ENUM_FIS FIS种类
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_TrigeSendFis(ENUM_FIS eFisType)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //TODO:如果发送FIS出错呢？
    DSATA_Update_rPCMDR(13,1);

    switch(eFisType)
    {
        case D2H_FIS:
            {
                GPIO_DEBUG(0x60);
                DSATA_Update_rPCMDR(16,1);
                //之前的方法会把DPP给清除，导致流程出错
                __enable_performance();//re enable
                while(SG_DSATA->rPCMDR.bits.TXREG != 0)
                {
                    //DPP没有清除的话就没有办法发送！！！！
                    if(__get_pfm_count() >= (U32)100*200000)
                    {
//                    	tSataDC.bPHYRESET = 0xAA;
                        PRINT_TXT("E:D2HTO");//100mS超时
                        delay_ms(1);
                        //reset PHY
//                    	SG_CPM->rPRE_CLK_SOURCE_SEL=0x02;//选择25M
//                    	SG_CPM->rSYS_CLK_SOURCE_SEL=1;//选择pre clk
//                    	SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0xF;//offline
//                    	SG_PADCTRL->rPHY_CTRL.bits.PHY_VCTRL=0;//normal
//                    	Delay(10);
//                    	SystemClkRecovery();//PHY时钟稳定后，恢复时钟配置
                        break;
                    }
                }

            }
            break;
        case SDB_FIS:

            __enable_performance();//re enable
            while(SG_DSATA->rPCMDR.bits.TXSDB != 0)
            {
                //DPP没有清除的话就没有办法发送！！！！
                if(__get_pfm_count() >= (U32)1*200000000)
                {
                    PRINT_TXT("E:SDBTO");//1S超时
                    break;
                }
            }
            //SG_DSATA->rPCMDR.bits.TXSDB=1;
            DSATA_Update_rPCMDR(17,1);
            break;
        case BIST_FIS:
            while(SG_DSATA->rPCMDR.bits.TXBISTA != 0);
            //SG_DSATA->rPCMDR.bits.TXBISTA=1;
            DSATA_Update_rPCMDR(18,1);
            break;
        default:
            //其他的FIS 控制器自己发送
            //!!ERROR
            break;
    }
    DEBUG_PRINT("SG_DSATA->rPCMDR=%#010lx\r\n",SG_DSATA->rPCMDR.dAll);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);

}
#ifndef __G3_FLYCODE
#ifndef __BOOT_CODE__
// *****************************************************************************
// 函数名称: CPM_IRQHandler
// 函数功能: CPM中断服务函数
// 入口参数: NONE
// 出口参数:
// 说    明:
// *****************************************************************************
#ifndef __RDT_SATA_CTRL
void CPM_IRQHandler(void)
{
    //static U8 power_flg=0;
    U16 int_sts=SG_CPM->rCPM_INT_STS.wAll;
    U16 tmp_sts=int_sts;
    int_sts &= 0x10C;//只处理bit2，3，8
    SG_CPM->rCPM_INT_STS.wAll &= (~int_sts);//只清楚本分支需要处理的状态

    UART_PRINT("Enter:  File=<%s> Func=<%s> \r\n",__FILE__,__func__);
    UART_PRINT("CPM_IRQHandler int_sts=%x\r\n",int_sts);

#ifdef _SCAN_SCSI_EXCEPTION_RECODE
    VDT_Exception_Record(int_sts);
#endif

    if(((int_sts>>8)&0x01))
    {
        SG_CPM->rCPM_INT_STS.bits.COMRESET_INT_STS=0;
        SG_CPM->rCPM_INT_CTRL.bits.COMRESET_INT_EN=0;//禁用中断
        ERROR_PRINT("SG_CPM->rCPM_INT_STS.bits.COMRESET_INT_EN\r\n");
        DSATA_Init(1);//由DMA1超时触发的COMRESET流程
        PRINT_TXT("COMRESET_INT_EN");
    }
    delay_ms(50);//VDT 异常退出
    PRINT_HEX("G0_VDT>",tmp_sts);
#if 0//(L_PWN_MNG_EN == TRUE)
    if(((int_sts>>2)&0x01))
    {
        SG_CPM->rCPM_INT_STS.bits.DEVSLP_ENTER_INT_STS=0;
        //enter deepsleep
        if(SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN == 0)
        {
            if((SG_CPM->rPOWER_STS.bits.DEVICE_SLEEP_STS) == 0x1)		//Device have been in Sleep mode, Need Firmware wakeup device
            {
                //Send Wakeup Request
                SG_CPM->rSLEEP_CTRL.bits.EXIT_SLEEP_MODE_REQ = 0x1;
                while(!SG_CPM->rPOWER_STS.bits.DEVICE_WAKEUP_STS);	//Wait Device be wakeup
                SG_CPM->rPOWER_STS.bits.DEVICE_SLEEP_STS = 0x1;
                SG_CPM->rSLEEP_CTRL.bits.SLUMBER_S_ENTER_EN = 0x0;
            }

#ifdef __DUAL_CORE__
            //双核模式，不能直接调用
            __nds32__mtsr(0x55545550, NDS32_SR_INT_PRI2);//临时调整SF1_INT1中断优先级0，保证wait到中断执行
            __getlock();
            tTQC.tDiTask.eStatus=CMD_VALID;
            tTQC.tDiTask.bOpcode=OP_DI_DEVSLP_REQ;
            tTQC.tDiTask.dLba=(U32)tSataDC.tSataMI.pDeepSleepEntry_Event_cb;
            SG_MCU->rSF0_INT1_STS1_SET =1;//ADD DI Task
            if(!__wait4unlocked())//等待任务完成中断
            {
                //超时,不进入休眠
                tTQC.tDiTask.eStatus=FALSE;
            }
            __unlock();
            __nds32__mtsr(0x55555550, NDS32_SR_INT_PRI2);//恢复SF1_INT1中断优先级

            if(!tTQC.tDiTask.eStatus)
            {
                //TODO:FTL不同意进入休眠,FTL不同意以后需要在合适的时机通知G0 可以进入sleep？？
                ERROR_PRINT("DeepSleep mode enter.Failed!!..\r\n");
                return;
            }
            //FTL 已经准备好进入DEVSLP
#else
            if(tSataDC.tSataMI.pDeepSleepEntry_Event_cb != NULL)
            {
                if(!tSataDC.tSataMI.pDeepSleepEntry_Event_cb())
                {
                    //不允许进入DEVSLP
                    ERROR_PRINT("DeepSleep mode enter.Failed!!..\r\n");
                    return;
                }
            }
#endif
            ERROR_PRINT("DeepSleep mode enter........\r\n");
            SG_CPM->rCPM_INT_STS.bits.DEVSLP_EXIT_INT_STS=0;
            SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN=1;
            SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=0;//已经进入sleep 不需要再响应这个中断

            delay_ms(10);

            SG_CPM->rPHY_POWER_CTRL.dAll |= (0xF<<18); //Gating HCLK/PCLK/ACLK/MCU_CLK;
            power_flg=1;
            __nds32__dsb();
            PAD_PowerDownCfg();
            LED_Config(LED0_NUM,LED_MODE_OFF,50);
            LED_Config(LED1_NUM,LED_MODE_OFF,50);

            //Shut PD1 Domain Power
            __memory_powerdown();
            //WAKEUP 过程中可能会产生误判

            SG_CPM->rSLEEP_CTRL.bits.ENTER_OFFLINE_MODE_REQ=1;

            while(!SG_CPM->rPOWER_STS.bits.DEVICE_WAKEUP_STS);//保证唤醒之前定位在这里
            if(SG_CPM->rPOWER_STS.bits.DEVICE_SLEEP_STS == 1)
            SG_CPM->rPOWER_STS.bits.DEVICE_SLEEP_STS = 0x1;//退出休眠
        }
    }

    if(((int_sts>>3)&0x01))
    {
        SG_CPM->rCPM_INT_STS.bits.DEVSLP_EXIT_INT_STS=0;
        //exit deepsleep
        if(power_flg == 1)
        {
            power_flg=0;

            //Wait PHY PLL Ready
            //while(!SG_CPM->rPHY_PLL_STS);
            SystemClkRecovery();

            SG_CPM->rPHY_POWER_CTRL.bits.PCLK_GATING_EN = 0x0;
            SG_CPM->rPHY_POWER_CTRL.bits.MCU_CLK_GATING_EN = 0x0;
            SG_CPM->rPHY_POWER_CTRL.bits.HCLK_GATING_EN = 0x0;

            //Recovery PD1 Domain Power
            __memory_poweron(0xFF);
            //重新初始化被关闭的模块

            DSATA_Init(1);

            SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN=0;
            if(tSataDC.tSataMI.pDeepSleepExit_Event_cb != NULL)
            {
#ifdef __DUAL_CORE__
                GIE_DISABLE();
                tSataDC.tSataMI.pDeepSleepExit_Event_cb();
                GIE_ENABLE();
#else
                tSataDC.tSataMI.pDeepSleepExit_Event_cb();
#endif
            }

#if (UART_DEBUG_ON == 1)
            UART_Init(UART_NUM,1843200,(U32)aDebugInfoBuffer,tSataDC.dSystemFrequency);//调试信息输出给HOST,1.8Mbps
#endif
            SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_ENTER_INT_EN=1;
            SG_CPM->rDEVSLP_CTRL.bits.DEVSLP_EN=1;
            ERROR_PRINT("DeepSleep mode exit........\r\n");
        }
    }
#endif
}

#endif
#endif
#endif
// *****************************************************************************
// 函数名称: DSATA_PRE_4K_ExpHandler
// 函数功能: 已经预收了4K 命令产生了异常
// 入口参数: bType:异常类型
//
//
//
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_PRE_4K_ExpHandler(void)
{
#if  (EXBUFF_CRC_PASS == SC_CRC_PASS_ST_DMA1)     //预收的4K 数据发生了CRC 则，ExBuff 是空的
    if( FALSE == SG_DSATA->rEXBUF_STS.bits.CRCPASS_EMPTY)  //二级buff 中还有数据过了CRC 还没传走
    {
#ifdef  SC_WP_4K_PRE
    	U8 dVal=SG_MCU->rSF0_INT1_STS1;
    	if((SG_DSATA->rPRE_TRIG_CNT)&&( FALSE == (dVal&(1 << CMD_SLOT31))))     //PRE 命令还没启动
    	{
    		U8 bPushIdx = SG_DSATA->rPOP_INDEX.bits.dVAL;
			UN_HWH2DFIS *unpH2DFSM=(UN_HWH2DFIS*)&arrunHWD2HFS[bPushIdx].adBuff[0] ;
			U8 bTag = unpH2DFSM->HWH2DFS.bTag;
            T_HI_TASK *ptTask=&tTQC.tHiTasks[bTag];
			bTag = CMD_SLOT32;
			ptTask->tParam.bTag = bTag;
			tTQC.tHiTasks[bTag].tParam.dLba = arrunHWD2HFS[bPushIdx].HWH2DFS.dLBA;
			tTQC.tHiTasks[bTag].tParam.dSctSize = arrunHWD2HFS[bPushIdx].HWH2DFS.dSecSize;
			*(U32*)&tTQC.tHiTasks[bTag].tParam.wLBA_exp = *(U32*)&arrunHWD2HFS[bPushIdx].HWH2DFS.wLBA_exp ;   // Copy 一个DW0
			tTQC.tHiTasks[bTag].tParam.bFua = arrunHWD2HFS[bPushIdx].HWH2DFS.h2d.dw1.bits.bDevice<<7;
			tTQC.tHiTasks[bTag].bOpcode = OP_HI_DATA;

			TQM_AddTask(ptTask);
			GIE_DISABLE();
			SATA_SaveH2DFis(FPDMA_QUEUED,(UN_FSM *)unpH2DFSM);
			GIE_ENABLE();
    	}
#ifndef __DUAL_CORE__
    	if(SG_DMA1->rSTATUS.bits.TRANS_DONE)  // DMA1 在空闲状态，则启动DMA1
    	{
           DMAC_DMA1Start(tTQC.tHiTasks[CMD_SLOT32].tParam.bDir,tSataDC.tSataMI.dDmaBuffAddr,tTQC.tHiTasks[CMD_SLOT32].tParam.dSctSize,DMA1_TOTAL_BUFF_SIZE,0);//把exbuff 中的数据取走
    	}
#endif
#endif
    	__enable_performance();//re enable
    	while(FALSE == SG_DSATA->rEXBUF_STS.bits.CRCPASS_EMPTY)    //等待G3 把exbuff 搬空
    	{
            if(__get_pfm_count() >= (U32)tSataDC.dMicroSecondCnt * 1000)     //1ms
            {
                PRINT_TXT("E:Exbuff");//1S超时
                break;
            }
    	}
    	__disbale_performance();

    }
#endif
}
#ifdef  _SATA_DEBUG_MESSAGE_
volatile static U8 bFlg_Cnt = 0;
volatile static U32 bFlg_Time = 0;
#endif
// *****************************************************************************
// 函数名称: DSATA_ExceptionHandler
// 函数功能: SATA端产生的异常处理
// 入口参数: bType:异常类型
//        0:protocol err,cmd err/srst,
//        1:dma1 TO,
//        FF:hw error,comreset/linkoff/crc err
// 出口参数:
// 说    明:
// *****************************************************************************
BOOL  DSATA_ExceptionHandler(U8 bType)
{

    if( (SG_MCU->rSF0_INT1_STS0 == 0) &&\
        (SG_MCU->rSF1_INT1_STS0 == 0) &&\
        ((SG_DMA1->rSTATUS.bits.TRANS_DONE == 1) && (SG_DMA1->rMC_STS.bits.MC_DONE == 1)) &&\
        (SG_DSATA->rPDTR == 0) &&\
		(SG_DSATA->rPPBCR == 0))
    {
        //TODO:要细分出来PIO/DMA的流程
        //SATA_ClearCmd();//SATA 命令管理变量复位
#ifndef __BOOT_CODE__
        if(tSataDC.bCmdCnt != 0)
        {
            PRINT_HEX("Cmd0=",tSataDC.bATACmd);
            Save_Uart_Buff_To_Log();
        }
#endif
        return FALSE;//没有任务不需要开启填充
    }
    TEST_PRINT("S = %d,sts= %d 0x%x,0x%x\r\n",SG_DMA1->rSTATUS.bits.TRANS_DONE,SG_DMA1->rMC_STS.bits.MC_DONE,SG_DSATA->rPDTR ,SG_DSATA->rPPBCR);


    TEST_PRINT("ExcPass_idx = %d,min = %d,Flg = %d\r\n",SG_DSATA->rCRC_PASS_INDEX,SG_DSATA->rCRC_PASS_MIN,SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL);

    U8 cal_tag=0;
    U8 fill_open=FALSE;
#ifndef __BOOT_CODE__
    SG_DSATA->rNCQ_REORDER_STP.dAll = 1;//阻止硬件继续配置reorder
    if(tSataDC.bIsNcqCmd)
    {
        SG_DSATA->rHWD2HDONE0=0;//注意：软件在清除HWD2HEN使能之前要先清除该寄存器。
        SG_DSATA->rHWD2HEN.dAll=0;//NCQ 进入异常模式，取消自动回复D2H

    }
    else
    {
        if((0xFF == tTQC.tHiTasks[0].tParam.bDir )|| (0x03 == tTQC.tHiTasks[0].tParam.bDir ))
        {
            //非数据相关的命令 不需要走CRC处理流程
            PRINT_HEX("Cmd1=",tSataDC.bATACmd);
            Save_Uart_Buff_To_Log();
            SATA_ClearCmd();
            return FALSE;
        }
    }
#else
    if((0xFF == tTQC.tHiTasks[0].tParam.bDir )|| (0x03 == tTQC.tHiTasks[0].tParam.bDir ))
    {
        //非数据相关的命令 不需要走CRC处理流程
        PRINT_HEX("Cmd1=",tSataDC.bATACmd);
        Save_Uart_Buff_To_Log();
        SATA_ClearCmd();
        return FALSE;
    }
    fill_open=TRUE;
#endif
#ifdef  _SATA_DEBUG_MESSAGE_
    PRINT_HEX("widx>",tTQC.bWrIdx);
    PRINT_HEX("RIdx>",tTQC.bRdIdx);
    PRINT_HEX("ReWidx>",tTQC.bReOdWrIdx);
    PRINT_HEX("ReRidx>",tTQC.bReOdRdIdx);
    PRINT_HEX("cnt>",tTQC.dCnt);
    PRINT_HEX("popNum>",tTQC.dPopedNum);
#endif
    //DONOT CHANGE!!!!!COMRESET 以后不能再ST=0，否则可能导致D2H发不出去
    if(bType != 0xFF)
    {
        //protocol error
        SG_DSATA->rPCMDR.bits.ST=0;
        __nds32__dsb();
        SG_DSATA->rPCMDR.bits.ST=1;//ENABLE dsata
    }
#ifndef __BOOT_CODE__
	if(FALSE == SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL)
	{
		SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL = TRUE;
		PRINT_TXT("E:Flg");//error： wait for ST CLR timeout
#ifdef  _SATA_DEBUG_MESSAGE_
		bFlg_Cnt ++;
		bFlg_Time =tSataDC.dPowerOnTime;
#endif
	}

    if((FALSE == tTQC.bIsNcqCmd) && (tTQC.tHiTasks[0].bOpcode == OP_HI_TRIM))
    {
        //TRIM 流程不需要等待启动，可能已经启动过了
        //--需要后续流程自己区分，如果是TRIM流程不需要填充UD
        //确保进入到trim 流程以后再回调
        fill_open=TRUE;
        U32 Tstart=tSataDC.dPowerOnTime;
        while((SG_MCU->rSF1_INT1_STS0&0x1) == 0)//确保进入到trim 流程以后再回调
        {
          if((tSataDC.dPowerOnTime - Tstart) >= 3000)
          {
              PRINT_TXT("E:WSF10TO");//等待DMA1启动超时
              //#ifdef  __DEBUG_WHILE
#ifdef  _SATA_DEBUG_MESSAGE_
                          while(1);
#endif
              //#endif
              break;
          }
#ifndef __BOOT_CODE__
          DSATA_G3_TaskRouter();
          TQM_CheckDiTaskDone();//修复等待过程中G3 DI任务完成卡死超时的问题
#endif
        }
    }
    else
    {
#ifdef __DUAL_CORE__
        //检查命令是否已经处理，DMA1是否是该命令的DONE状态
        //特别是读的时候，最后一个FIS发生CRC ，DMA1是可以提前DONE的，非NCQ状态下再等DONE就会超时！！！！！！
        if((FALSE == tTQC.bIsNcqCmd) && \
           (SG_DSATA->rPPBCR == 0) && \
           (SG_DMA1->rSTATUS.bits.TRANS_DONE == 1) && \
           (tTQC.tHiTasks[0].tParam.bDir == DIR_IN))
        {
            //非NCQ数据传输完成，PDTR提前启动，所以PPBCR为0 表示不需要再次启动DMA1
            PRINT_TXT("fill close");
            fill_open=TRUE;
        }
        else
        {
            fill_open=TRUE;
            U32 Tstart=tSataDC.dPowerOnTime;
            while(SG_DMA1->rSTATUS.bits.TRANS_DONE == 1)//等待FTL流程启动DMA1，这样IDX才是稳定准确的 --NCQ流程这样做没问题
            {
              if((tSataDC.dPowerOnTime - Tstart) >= 3000)
              {
            	  fill_open = FALSE;//已经做完所有任务？
                  PRINT_TXT("E:WDMATO");//等待DMA1启动超时

                  //#ifdef  __DEBUG_WHILE
#ifdef  _SATA_DEBUG_MESSAGE_
                              while(1);
#endif
                  //#endif
                  break;
              }
#ifndef __BOOT_CODE__
              if(TRUE == tTQC.bF2S_Task_abortFlg)  //FTL 通知SATA task 终止
              {
            	  break;
              }
#endif
              DSATA_G3_TaskRouter();
              TQM_CheckDiTaskDone();//修复等待过程中G3 DI任务完成卡死超时的问题
            }
        }
#else
        fill_open=TRUE;
#endif
        PRINT_HEX("DMA1idx>",SG_DMA1->rINDEX);
        PRINT_HEX("DMA1sec>",SG_DMA1->rTOTAL_SECT);
        //除TRIM外的数据操作,等待FTL端启动任务，再进入到虚拟填充，保证填充的UD是准确的
    }

    Save_Uart_Buff_To_Log();
    //打印关键变量 --后续会被清除，所以要先在串口记录下来
    PRINT_HEX("SF0>",SG_MCU->rSF0_INT1_STS0);
    PRINT_HEX("SF1>",SG_MCU->rSF1_INT1_STS0);

    if(tSataDC.bIsNcqCmd)
    {
        //等DMA1 重新启动，能够保证FW更新过 REORDER寄存器
    	if(SG_DSATA->rNCQ_REORDER_REG[SG_DSATA->rNCQ_DONE_TOTAL.dAll] != 0XFF)
		{
    		cal_tag = (SG_DSATA->rNCQ_REORDER_REG[SG_DSATA->rNCQ_DONE_TOTAL.dAll] & 0x1F);
		}
		else
		{
			//无效的记录
			cal_tag=tTQC.bReOdCmds[((tTQC.bReOdRdIdx-tSataDC.bReorderCnt)&0x1F)];
		}
    	//cal_tag = (SG_DSATA->rNCQ_REORDER_REG[SG_DSATA->rNCQ_DONE_TOTAL.dAll] & 0x1F);
        PRINT_HEX("TAG>",cal_tag);
    }
    SG_DSATA->rNCQ_REORDER_EN.dAll = 0;
    SG_DSATA->rNCQ_DONE_TOTAL.dAll=0;
    SG_DSATA->rNCQ_REORDER_TOTAL.dAll=0;
#endif


    PRINT_HEX("LBA>",tTQC.tHiTasks[cal_tag].tParam.dLba);
    PRINT_DEC("SEC>",tTQC.tHiTasks[cal_tag].tParam.dSctSize);
    PRINT_DEC("DIR>",tTQC.tHiTasks[cal_tag].tParam.bDir);

    //while(1);//TODO:调试阶段卡住，方便调试问题，SATA LOG测试需要打开
    //双核模式需要block住
    if(fill_open)
    {
        if(tSataDC.tSataMI.pRemove_AllTask_cb != NULL)
        {
#ifdef __DUAL_CORE__
            GIE_DISABLE();
            tSataDC.tSataMI.pRemove_AllTask_cb(cal_tag,0);
            GIE_ENABLE();
#else
            tSataDC.tSataMI.pRemove_AllTask_cb(cal_tag,0);
#endif
        }

        if(SG_DMA1->rCTRL.bits.DIR)
        {
            //host write
            //根据DMA1的数据长度进行填充补足
            //填充数据
            SG_DSATA->rFILL_INVLDATAH=0xFEEFFEEF;
            SG_DSATA->rFILL_INVLDATAL=0xFEEFFEEF;
        }
        else
        {

        }
        //开启填充
        SG_DSATA->rFILL_INVLDA_ST.dAll = 1;//开启填充,不管DMA1当前是否是DONE状态，都可以开启？
        __nds32__dsb();
#ifndef __DUAL_CORE__
#ifdef  __PRIVATE_CRC_HANDLE__
			tScsiCtl.dWrValidFlg=0;
#endif  //
        while (!SG_DMA1->rSTATUS.bits.TRANS_DONE);//单核没有办法等ST被清掉
        SG_DSATA->rFILL_INVLDA_ST.dAll = 0;
        //NVIC_DisableIRQ(IRQ_SC);
       // GIE_DISABLE();
#else
        SG_MCU->rSF1_INT1_STS0_CLR=0xFFFFFFFF;
        SG_MCU->rSF0_INT1_STS0_CLR=0xFFFFFFFF;//以免旧命令清除延迟导致取命令异常

        U32 Tstart=tSataDC.dPowerOnTime;
        while(SG_DSATA->rFILL_INVLDA_ST.dAll)//等待FTL异常处理完成
        {
        	if( TRUE ==  tTQC.bF2S_Task_abortFlg )
        	{
        	     tTQC.bF2S_Task_abortFlg  = FALSE;
        	     PRINT_TXT("E:CLR");
        	}

            if((tSataDC.dPowerOnTime - Tstart) >= 8000)
            {
                PRINT_TXT("E:TO,W4ST=0");//error： wait for ST CLR timeout
                SG_DSATA->rFILL_INVLDA_ST.dAll=0;
    //#ifdef  __DEBUG_WHILE
#ifdef  _SATA_DEBUG_MESSAGE_
                while(1);
#endif
    //#endif
                break;
            }
            DSATA_G3_TaskRouter();
            TQM_CheckDiTaskDone();//修复等待过程中G3 DI任务完成卡死超时的问题
        }
#endif
    }
   //根据DMA1的执行状态，填充DMA1
    SATA_ClearCmd();
#ifdef SC_H2DFIFO_EN
    tSataDC.bCmdPushIdx = 0;
    SG_DSATA->rH2DBUF_RST.dAll = 0x03;
#endif

    SG_DSATA->rDMCRST.dAll = 1;   //WILLIAM:复位SC 和DMA1之间的 数据通路，DMASETUP 之后来了COMRESET 会导致数据通路卡死，进而导致TXREG 发布出去D2H
#ifdef  SC_EXBUFF_EN
    SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 0;
    SG_DSATA->rCLR_EXBUF.bits.dVAL = 1;
    SG_DSATA->rFW_EN.bits.BUFFER_EX_EN = 1;
#endif
    U32 dRecttag = SG_DSATA->rRECVTAG;
    SG_DSATA->rRECVTAG = dRecttag;
    SG_DSATA->rH2DERR.dAll =0;
    SG_DSATA->rERR_CLR.dAll=7;
//    if(tSataDC.bIsNcqCmd)
//    {
//        SG_DSATA->rHWD2HDONE0=0;//注意：软件在清除HWD2HEN使能之前要先清除该寄存器。
//        SG_DSATA->rHWD2HEN.dAll=0;//NCQ 进入异常模式，取消自动回复D2H
//    }
#ifndef __BOOT_CODE__
    SG_DSATA->rNCQ_REORDER_STP.dAll = 0;
#endif
    SG_DSATA->rERRMSK_CLR.bits.dVAL=1;//不进行这个操作的话，后续非联动启动的DMA1数据没有办法写到MEMORY
    SG_CPM->rSYS_RESERVED3 = 0;
    return TRUE;
}

void comreset(T_PISR* pPISR)
{
	SG_DSATA->rDEL_NONALIGN_EN.dAll = 1;//NOTE:打开此标记，解决GEN3平台 COMRESET握手失败问题

    if(pPISR->bits.PCS)
    {
        PRINT_TXT("COMRESET");
    }
    else
    if(pPISR->bits.COMWKS)
    {
        PRINT_TXT("COMWAKE");
    }
    //过滤掉下面的中断处理
    pPISR->bits.IFS=0;
    pPISR->bits.INFS=0;
    pPISR->bits.HDRS=0;
#if(ASR_DISABLE == TRUE)
//    SG_PADCTRL->rPHY_XCFGD0&=(~0x00000010);//REenable ASR
#endif
#ifdef  _SATA_TRIGGER_CRC__
    Abnormal_SataComResetFail_SaveLog();
#endif


#ifdef   SC_EXBUFF_EN
     DSATA_PRE_4K_ExpHandler();  // PRE_4K 命令的开了二级buff 的异常处理
#endif

    SG_DSATA->rERR_CLR.dAll=7;//comreset能不能自动复位内部值？
 //   SG_DSATA->rSTCRCE.dAll=0;//植入出错操作以后，出发以后直接关闭
    SG_DSATA->rERRMSK_CLR.dAll=1;//发生CRC以后，不进行这个操作的话，后续非联动启动的DMA1数据没有办法写到MEMORY
    SG_DSATA->rCRCNT.dAll = 0;

    DSATA_ExceptionHandler(0xFF);

#ifdef SC_H2DFIFO_EN
    tSataDC.bCmdPushIdx = 0;
    SG_DSATA->rH2DBUF_RST.dAll = 0x03;
#endif
#if (D2H_AUTO_SEND == TRUE)
    SG_DSATA->rHWD2HDONE0=0;//异常处理接口中存在漏掉清除动作的情况
    SG_DSATA->rHWD2HEN.dAll=1;   //abort 命令后重新启动
#else
    SG_DSATA->rHWD2HEN.bits.dVAL = 0;

#endif

    SATA_DevCtx_Init(0);
    /*	The enable/disable state for software settings
        preservation shall persist across software reset. The enable/disable state for software settings preservation shall
        be reset to its default state upon COMRESET. The host may disable software settings preservation in order to
        cause software settings to revert to their power-on default state when the device receives a COMRESET.
     */
    unAtaIdentify.wArray[79]|=(1<<6);//default enable SSP

#if(SATA_LOG_ENABLE == 0)
    SATA_ChangeState(POWERON_COMRESET,DHR0_HW_RESET_ASSERTED);
#endif
    tSataDC.eTransType=POWERON_COMRESET;//记录COMRESET 流程

#if (SATA_LOG_ENABLE == 1)
    //sata log 中COMRESET 也算一个CMD，需要被记录
    /*
     If the command data structure represents a hardware reset, then the
        content of byte n （CONTROL field）shall be FFh, the content of bytes n+1 through n+13 are vendor specific, and the content of
        bytes n+14 through n+17 shall contain the timestamp.
    */
    unFsmBuff.FIS.h2d.dw3.bits.bCtrl=0xFF;
    tSataDC.enAddrMode=MODE_28BIT;
    SATA_SaveH2DFis(SRESET,&unFsmBuff);
    SATA_UpdateRdIdx(1);

    tSataLogCtx.dHWResetNum++;
    tSataLogCtx.dResetsCntOfCmd++;

    //复位REORDER状态，异常状态下可能导致reorder 初始状态异常！！
    SG_DSATA->rNCQ_REORDER_EN.dAll = 0;
    SG_DSATA->rNCQ_DONE_TOTAL.dAll=0;
    SG_DSATA->rNCQ_REORDER_TOTAL.dAll=0;
    SG_DSATA->rDMCRST.bits.dVAL =1;
    SG_DSATA->rNCQ_REORDER_STP.dAll = 0;


    SG_CPM->rSYS_RESERVED3 = 0;//置位SATA数据传输异常标记，给G3流程参考
    //手动植入读CRC错
//    SG_DSATA->rTRXCRC.bits.ERR_DFIS_NUM=14;
//    SG_DSATA->rTRXCRC.bits.ERR_TAG=14;
//    SG_DSATA->rTRXCRC.bits.TXCRC =1;
//    SG_DSATA->rTRXCRC.bits.RXCRC =0;
//    SG_DSATA->rSTCRCE.dAll=1;
#endif
}

void linkchange(T_PISR* pPISR)
{
    U8 bCurState;
    //LSCS: Link State Change Status
    UART_PRINT("LSCS: Link State Change Status\r\n");
    //when BIST Activate FIS is successfully received from the host

    bCurState=(ENUM_LPS)SG_DSATA->rPSSTSR.bits.IPM;
    //ERROR_PRINT("SG_DSATA->rPSSTSR=%#010lx [%s]\r\n",SG_DSATA->rPSSTSR.dAll, LPS_STATE[bCurState]);
#ifndef __BOOT_CODE__
    if(tSataDC.eLPMSts == bCurState)
    {
        //重复流程不需要处理
        return;
    }
#endif

    if(LPS_OFF_LINE == bCurState)
    {
        //LED_Config(LED1_NUM,LED_MODE_OFF,100);
    }

    if(LPS_OFF_LINE == bCurState && (!pPISR->bits.PCS)/*comreset link_off 一起发生*/)
    {
        PRINT_TXT("L_OFF");
        ERROR_PRINT("LSCS: Link State Change Status\r\n");
#ifndef __BOOT_CODE__
        DSATA_ExceptionHandler(0xFF);
#else
        //delay_ms(1);//PHY reset会暂时切换时钟，可能导致串口打印出现乱码
        SATA_ClearCmd();
        DSATA_Init(0);  //不再切时钟，理论上不会有问题，
#endif
        tSataDC.eTransType =UNDEFINE;
#if (SATA_LOG_ENABLE == 1)
        if(tSataLogCtx.dPHYRDY2N < 0xFFFFFFFF)
        {
            tSataLogCtx.dPHYRDY2N++;//CRC 错误统计
            WriterGPLLogsRouter(CMD_IDLE,NULL);//更新状态
        }
#endif
    }
#ifndef __BOOT_CODE__
    else if(LPS_ACTIVE == bCurState)
    {
        PRINT_DEC("SPD=GEN", SG_DSATA->rPSSTSR.bits.SPD);
        //LED_Config(LED1_NUM,LED_MODE_ON,100);//flycode 模式下 LED1保持灭的状态
        if(SG_DSATA->rPSSTSR.bits.DET== 4)
        {
            // PHY in offline mode as a result of the interface being disabled or running in a BIST loopback mode.
#if(ASR_DISABLE == TRUE)
//            SG_PADCTRL->rPHY_XCFGD0|=0x00000010;//disable ASR
            ERROR_PRINT("..................disable ASR\r\n");
#endif
#if (SATA_LOG_ENABLE == 1)
            tSataLogCtx.dCmdFailDue2ICRC =0;
            tSataLogCtx.dPHYRDY2N = 0;
            tSataLogCtx.dD2hFisDue2Comreset = 0;
            tSataLogCtx.dCRCErrorCnt =0;
            tSataLogCtx.dNoneCRCErrorCnt =0;
            WriterGPLLogsRouter(CMD_IDLE,NULL);//更新状态
#endif
        }
        else
        {
#if (L_PWN_MNG_EN==1)
            if(pPISR->bits.PMREQ_S || pPISR->bits.PMREQ_P)
            {

            }
            else
            {
                UART_PRINT("tSataDC.eLPMSts= [%s]\r\n", LPS_STATE[tSataDC.eLPMSts]);
                if(/*!SG_DSATA->rPISR.bits.HDRS && */(tSataDC.bIsDIPMEn == TRUE) && (SG_CPM->rPAWCTRL.bits.PAWEN == 1) && (LPS_PARTIAL == tSataDC.eLPMSts))
                {
                    SG_CPM->rPAWCTRL.bits.PAWEN =0;//使能重新激活
                    //固件切换到低频时钟
                    //LowSpeedModeEn();
                    SG_CPM->rSLEEP_CTRL.bits.SLUMBER_S_ENTER_EN=1;
                    //尝试令HOST 进入SLUMBER
                    SG_DSATA->rPSCTLR.bits.ITIMER_PM=LPM_SLUMBER;
//                        if(!SG_DSATA->rPISR.bits.HDRS)//确保安全进入slumber前，HOST 没有新命令
//                        SG_DSATA->rPCMDR.bits.ICC=LPS_SLUMBER;
                    SG_DSATA->rPSCTLR.bits.ITIMER_EN=1;
                    ERROR_PRINT("AUTO TO LPS_SLUMBER....\r\n");

                }
                if((tSataDC.bIsDIPMEn == TRUE) && (LPS_SLUMBER == tSataDC.eLPMSts))
                {
                    //device 发起的slumber comwake
                    SG_CPM->rPAWCTRL.bits.PAWEN =1;//使能重新激活
                    //尝试令HOST 进入PARTIAL
                    SG_DSATA->rPSCTLR.bits.ITIMER_PM=LPM_PARTIAL;//TODO:控制省电级别

                    if(SG_DSATA->rPCMDR.bits.APSTE==1)
                    {
                        SG_CPM->rPAWCTRL.bits.PAWEN =0;
                    }
                }
            }
#endif
        }
    }
    else
    {
        //省电模式，要做什么？
        if(LPS_PARTIAL == bCurState || LPS_SLUMBER == bCurState)
        {
            //PATIAL,SLUMBER 模式进入SLUMBER_S，DEEPSLEEP 不需要进入
            //ERROR_PRINT("System CLK enter low speed mode...\r\n");
//            	if(tSataDC.bIsDIPMEn == TRUE)
//            	{
//            		SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;
//            		UART_PRINT("SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;\r\n");
//            	}
            //固件切换到低频时钟
            //LowSpeedModeEn();

            if(LPS_SLUMBER == bCurState)
            {
                PRINT_TXT("PM_S");
            }
            else
            {
                PRINT_TXT("PM_P");
            }
        }

    }
#endif
    if(tSataDC.tSataMI.pPHYLinkChangeEvent_cb != NULL)
    {
#ifdef __DUAL_CORE__
        GIE_DISABLE();
        tSataDC.tSataMI.pPHYLinkChangeEvent_cb(bCurState);
        GIE_ENABLE();
#else
        tSataDC.tSataMI.pPHYLinkChangeEvent_cb(bCurState);
#endif
    }
    tSataDC.eLPMSts= (ENUM_LPS) bCurState;
    GPIO_DEBUG(0x53);
}
void DSATA_CmdReorderRestart(T_PISR* pPISR,U8 mode)
{
#ifndef __BOOT_CODE__

    U32 mask=0;
    //NVIC_DisableIRQ(IRQ_SF1_INT0);
    GIE_DISABLE();//该指令更简单
    U32 mask_done;
#ifdef __SDB_AUTO_ACC__
    mask_done=SG_MCU->rSF1_INT1_STS0;
#else
    U8 bSlot=0;
    U8 bDPP=0;

    //在命令入队列的过程中，有可能有数据传输完成，需要提前清除DPP
    if(pPISR->bits.DPS && tSataDC.bIsNcqCmd)
    {
       if(mode == 0)
           pPISR->bits.DPS=0;//清除中断，后续就不再响应
       else
           pPISR->dAll=DPS_BIT;//清除中断，下次就不再响应
       //多个SLOT同时启动的话可能存在CCS位提前被置换成下一个SLOT,所以在下一个SLOT启动之前就清除掉相关标记
       bSlot=DSATA_GetCurrentCmdSlot();
       DSATA_Update_rPCMDR(13,1);
#if 1
       bDPP=1;
#if (SDB_AUTO_SEND==TRUE)
#else
        //DSATA_BulidSDBFis(4,0,0,1,mask);
        unFsmBuff.FIS.sdb.dw0.dAll=0x004040A1;
        unFsmBuff.FIS.sdb.dw1.dAll=SG_MCU->rSF1_INT1_STS0;
        DSATA_Update_rPCMDR(4,1);//重新使能接收-使能接收以后PDMA才会进行发送动作
        DSATA_Update_rPCMDR(17,1);//DSATA_TrigeSendFis(SDB_FIS);
        mask_done=SG_MCU->rSF1_INT1_STS0;
#endif
#else
#if (SDB_AUTO_SEND==TRUE)
       //自动发送SDB
       DEBUG_PRINT("SG_DSATA->rPISR.bits.DPS=1\r\n");
       TQM_RemoveTask(bSlot);
       SATA_UpdateRdIdx(1);
#else
       TQM_RemoveMulTask(SG_MCU->rSF1_INT1_STS0);
#endif
#endif
    }
#endif

#ifdef __SDB_AUTO_ACC__
    if(/*SG_DSATA->rNCQ_REORDER_STP.dAll == 0 &&\*/
       SG_DSATA->rNCQ_DONE_TOTAL.dAll >= SG_DSATA->rNCQ_REORDER_TOTAL.dAll &&\
       tTQC.dReOdCnt != tTQC.dStartedCnt)
#else
    if(SG_DSATA->rNCQ_DONE_TOTAL.dAll >= SG_DSATA->rNCQ_REORDER_TOTAL.dAll &&\
       tTQC.dReOdCnt != tTQC.dStartedCnt &&\
       !SG_DSATA->rPISR.bits.DPS)
#endif
    {
        U8 I=0;
        U8 CNT=tSataDC.wNcqTOCnt;//多条命令拆分成多次启动，让HOST有机会插入新命令

        U8 remain=tTQC.dReOdCnt-tTQC.dStartedCnt;
//        PRINT("remain = %d\r\n",remain);
//        TEST_PRINT("remain = %d\r\n",remain);
//        static  U8 bTemp = 8;
//        		U8 bTpempTag = 0;
//        if(remain>=30)
//        {
//            if(bTemp == 30)
//            {
//           	bTemp = 15;
//            }
//            bTemp++;
//            bTpempTag = tTQC.bReOdCmds[bTemp];
//
//            tTQC.bReOdCmds[bTemp] = tTQC.bReOdCmds[bTemp+1];
//            PRINT("Tag1 = 0x%x tag2=0x%x\r\n", bTpempTag,tTQC.bReOdCmds[bTemp] );
//
//            tTQC.bReOdCmds[bTemp + 1] = bTpempTag ;
//        }

#ifdef  SC_H2DFIFO_EN
        CNT=remain;
#else
        if(remain > tSataDC.bNcqBrustTH)
        {
            CNT=tSataDC.bNcqMaxBrustCnt;//读RAM带宽 372M+
        }
        else if(remain < CNT)
        {
            CNT=remain;
        }
#endif

        SG_DSATA->rNCQ_REORDER_EN.dAll            = 0;
        SG_DSATA->rNCQ_REORDER_TOTAL.dAll         = 0;//写操作会触发启动，所以要在使能之前清除
        SG_DSATA->rNCQ_REORDER_EN.dAll            = 1;//初始化操作要在使能操作之前
//        SG_DSATA->rNCQ_REORDER_EN.dAll            = 0x3;//初始化操作要在使能操作之前

        while(CNT--)
        {
//            PRINT("reorder = 0x%x idx=%d\r\n",tTQC.bReOdCmds[((tTQC.bReOdRdIdx)&0x1F)],tTQC.bReOdRdIdx);
            SG_DSATA->rNCQ_REORDER_REG[I]=tTQC.bReOdCmds[((tTQC.bReOdRdIdx++)&0x1F)];
            mask|=(1<<SG_DSATA->rNCQ_REORDER_REG[I]);
            I++;
        }

#if (SDB_AUTO_SEND==FALSE)
        //将本次连续启动的最后一个TAG号的DPP置位，这个配置不能在这里做。需要一个REORDER的START中断，在中断里面执行
        tPrdTbls[SG_DSATA->rNCQ_REORDER_REG[I-1]].dw3.bits.I=1;
#endif
//        SG_DSATA->rNCQ_REORDER_NOTREADY.dAll = 1;//初始化操作要在使能操作之前
        SG_DSATA->rNCQ_REORDER_TOTAL.dAll = I;        //启动前等待H2D插入，以免一堆数据传输都不能插入任务
        tSataDC.bReorderCnt = I;

        if(mode==0&&\
           SG_DSATA->rNCQ_REORDER_REG[0] <= 10 && \
           !SG_DSATA->rPISR.bits.HDRS)
        {
            __enable_performance();
            while((__get_pfm_count() <200));//避开与HOST可能下发的新命令的竞争
        }

        if(SG_DSATA->rH2DERR.dAll != 0 || SG_DSATA->rPCMDR.bits.FRXP==1)
        {
            //NCQ cmd malformed,如果此时出现ERR，可能会丢失中断卡死
            //NCQ命令出现异常，终止命令
            tTQC.bReOdRdIdx=(tTQC.bReOdRdIdx&0x1F);
            tTQC.dStartedCnt=tTQC.dReOdCnt;
            PRINT_TXT("E:NCQ CMD MALFORMED");
            GIE_ENABLE();
            return;
        }

//        PRINT_HEX("rPSERR=",SG_DSATA->rPSERR.dAll);//打印错误类型
//        PRINT_HEX("PISR=",SG_DSATA->rPISR.dAll);//
//        PRINT_HEX("rReorderEN=",SG_DSATA->rNCQ_REORDER_EN.dAll);//
//        PRINT_HEX("rPDTR=",SG_DSATA->rPDTR);//
        SG_DSATA->rNCQ_CMD_ST.dAll = 1;
        TEST_PRINT("ReoderT=%d\r\n",I);
//        PRINT_TXT("ST_Reorder");
#ifndef  H2D_DMASETUP_TEST
        if(SG_DSATA->rPISR.bits.HDRS && mode==0)
        {
            SG_DSATA->rPISR.dAll=HDRS_BIT;
            pPISR->bits.HDRS=1;
        }
#else
        if(SG_DSATA->rPISR.bits.HDRS && mode==0)
        {
//            SG_DSATA->rPISR.dAll=HDRS_BIT;
            pPISR->bits.HDRS=1;
        }
#endif

        tTQC.bReOdRdIdx=(tTQC.bReOdRdIdx&0x1F);
        tTQC.dStartedCnt+=I;
        SG_MCU->rSF0_INT1_STS0_CLR=mask;
        SG_MCU->rSF1_INT1_STS0_SET=mask;
        //PRINT_HEX("MIR=",(mode<<14)| (I<<8) |(tTQC.dReOdCnt-tTQC.dStartedCnt));
    }

#ifdef __SDB_AUTO_ACC__
    if(mask_done!=0)
    {
        TQM_RemoveMulTask(mask_done);
    }
#else
    if(bDPP)
    {
#if (SDB_AUTO_SEND==TRUE)
       //自动发送SDB
       DEBUG_PRINT("SG_DSATA->rPISR.bits.DPS=1\r\n");
       TQM_RemoveTask(bSlot);
       SATA_UpdateRdIdx(1);
#else
       TQM_RemoveMulTask(mask_done);
#endif
    }
#endif

    GIE_ENABLE();//该指令更简单
    //NVIC_EnableIRQ(IRQ_SF1_INT0);
#endif
}

#ifndef __G3_FLYCODE
// *****************************************************************************
// 函数名称: DSATA_IRQHandler
// 函数功能: SATA中断服务函数
// 入口参数: NONE
// 出口参数: 
// 说    明: 
// *****************************************************************************
void DSATA_IRQHandler(void)
{
    T_PISR tPISR;
    //ERROR_PRINT("SG_DSATA->rPISR.dAll=%#010lx,tSataDC.eLPMSts=%d \r\n",SG_DSATA->rPISR.dAll,tSataDC.eLPMSts);
    //UART_PRINT("SG_DSATA->rPCMDR.dAll=%#010lx\r\n",SG_DSATA->rPCMDR.dAll);
    //DEBUG_PRINT("DSATA_GetCurrentCmdSlot=%d\r\n",DSATA_GetCurrentCmdSlot());

    //PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    //PRINT("eTransType=%u(%s)\r\n",tSataDC.eTransType,TRSP_TYPE[tSataDC.eTransType]);
    //Delay(5000);
    //PRINT("DSATA_GetCurrentCmdSlot=%d\r\n",DSATA_GetCurrentCmdSlot());
    //__enable_performance();//开启定时器计数
#if (L_PWN_MNG_EN==1)
    //if(tPISR.bits.COMWKS)
    {
        SystemClkRecovery();//有中断就意味着有任务，可以恢复时钟
        //DISABLE POWERMANAGE
        SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;//唤醒以后可能有新命令来了，就关闭DIPM,等回到IDLE再打开
    }
#endif
    //PRINT_HEX("rPISR=",SG_DSATA->rPISR.dAll);
    NVIC_DisableIRQ(IRQ_SF1_INT0);//更高优先级的中断嵌套

    tPISR.dAll=(SG_DSATA->rPISR.dAll&SG_DSATA->rPIER.dAll);//暂存中断
    SG_DSATA->rPISR.dAll=(~SG_DSATA->rPIER.dAll);

    if(tPISR.bits.IFS)
    {
    	//数据传输异常，提前置位异常标记,给G3参考
    	SG_CPM->rSYS_RESERVED3 = 0xCC;
    }

#if (SDB_AUTO_SEND==TRUE)
    NVIC_EnableIRQ(IRQ_SF1_INT0);//不涉及到回复SDB冲突
#else
    if(!tPISR.bits.DPS)
    {
         NVIC_EnableIRQ(IRQ_SF1_INT0);//不涉及到回复SDB冲突
    }
    else
    {
        //等到回复完SDB再打开中断，DSATA_CmdReorderRestart执行完成以后
        //PRINT_TXT("mode3\r\n");
    }
#endif

    __nds32__dsb();
    GPIO_DEBUG(0x90);
    GPIO_DEBUGDW(SG_DSATA->rPISR.dAll);
    GPIO_DEBUGDW(tPISR.dAll);
    GPIO_DEBUGDW(SG_DSATA->rPCMDR.dAll);
#if (D2H_AUTO_SEND == FALSE)
    if(tSataDC.bIsNcqCmd == TRUE)
    {

        /*If the H2D Register FIS with a new command is received before a DMA Setup FIS is
        transmitted, the TxFIFO is cleared to prevent the DMA Setup FIS from transmitting. In this
        case, firmware must prepare the D2H Register FIS and set the PCMDR.TXREG bit prior to
        clearing PISR.HDRS and PCMDR.FRXP bits to send the D2H Register FIS to un-queue this
        new command. DMA Setup FIS is sent after D2H Register FIS.
        */
        //H2D的中断延后在发送完D2H后清除
        SG_DSATA->rPISR.dAll=(tPISR.dAll&(~HDRS_BIT));//清除除去H2D外的所有中断
    }
    else
    {
        SG_DSATA->rPISR.dAll=tPISR.dAll;
    }
#else
    if(tSataDC.bIsNcqCmd == TRUE)
    {

#ifndef   H2D_DMASETUP_TEST
        if(SG_DSATA->rH2DERR.dAll != 0 || SG_DSATA->rPCMDR.bits.FRXP==1)
        {
            SG_DSATA->rPISR.dAll=(tPISR.dAll&(~HDRS_BIT));//清除除去H2D外的所有中断
            //PRINT_TXT("NCQ cmd malformed");
        }
        else
        {
            SG_DSATA->rPISR.dAll=tPISR.dAll;
        }
#else
        {
        	SG_DSATA->rPISR.dAll= tPISR.dAll & 0xFFFFFFFE;
        }
#endif   //  H2D_DMASETUP_TEST
    }
    else
    {
#ifndef   H2D_DMASETUP_TEST
        SG_DSATA->rPISR.dAll=tPISR.dAll;
#else
        SG_DSATA->rPISR.dAll = tPISR.dAll & 0xFFFFFFFE;
#endif   //  H2D_DMASETUP_TEST

    }
#endif

    //NOTE:清除中断一定要注意，进入中断以后对SG_DSATA->rPISR.dAll只读一遍，如果再次读可能值就不一样了。
    //中断处理
    GPIO_DEBUGDW(SG_DSATA->rPISR.dAll);
    GPIO_DEBUGDW(tPISR.dAll);


#ifndef __BOOT_CODE__
    if(tSataDC.eTransType != POWERON_COMRESET && (tPISR.bits.PCS || (tPISR.bits.COMWKS && tSataDC.eLPMSts == LPS_OFF_LINE)))
#else
    if(tPISR.bits.PCS || (tPISR.bits.COMWKS && LPS_OFF_LINE ==  tSataDC.eLPMSts))
#endif
    {
        GPIO_DEBUG(0x52);
        //PCS: Port COMRESET Status
        //理论上判定comreset中断是最好的，但是硬件有时候可能会丢失COMRESET中断:
        /*
         *eg:
         * 0673260:COMWAKE
                    COMWAKE
                    SPD=GEN03
                    IDFY P06
                    D2H
         * */
        //只通过COMWAKE判定的话， --COMWAKE 中断可能会晚于LINK change 中断，导致LPS_OFF_LINE ==  tSataDC.eLPMSts 不满足,
        //COMRESET -> SPD=GEN3 ->SPD=GEN3 所以在link active的时候的COMWAKE  -- 目前没有办法区分这个情况！！！！！
        //patial/slumber的状态是硬件状态，软件可能跟不上！！！！，没有办法区分comwake是否是由COMRESET带起
        comreset(&tPISR);
    }

    if(tPISR.bits.LSCS)
    {
        linkchange(&tPISR);
        tPISR.bits.LSCS=0;
    }

    if(tPISR.bits.CMDONS)
    {
        //REORDER DONE
        DSATA_CmdReorderRestart(&tPISR,0);
        tPISR.bits.CMDONS=0;
    }

#ifdef  SC_WP_4K_PRE


    if(tTQC.dPRECMD_ReoderCnt != tTQC.dPRECMD_MoveCnt)
    {
    	tTQC.dPRECMD_MoveCnt++;
    	if(tTQC.dPRECMD_MoveCnt>=256)
    	{
    		tTQC.dPRECMD_MoveCnt = 0;
    	}
    	TQM_RemoveTask(CMD_SLOT32);
    }
    //解决两种情况
    //1、预收命令正常结束
    //2、预收命令结束后有收到新的预收命令。
	if(( TRUE == tSataDC.bPre4kCmdFlg)&&((FALSE == SG_DSATA->rPRE_TRIG_ON.bits.dVAL)||(SG_DSATA->rPRE_TRIG_CNT &&(tTQC.dPRECMD_ReoderCnt == tTQC.dPRECMD_PushCnt))))
	{
		SG_DSATA->rPIER.bits.FTXDE=0;//DISABLE FTXDS
		tSataDC.bPre4kCmdFlg = FALSE;
		SATA_UpdateRdIdx(1);
//		PRINT_HEX("Flg3>",tTQC.bPRE_Cmd_Flg);
//		PRINT_HEX("Dcnt>",tSataDC.bCmdCnt);
	}
//    if(( TRUE == tTQC.bPRE_Cmd_Flg)&&(ptPISR->bits.DPS || SG_DSATA->rPISR.bits.DPS))  //需开启PRE_4K 的PRD I 中断

#endif

    //传输模式下的中断处理入口
    if(/*(LPS_ACTIVE == tSataDC.eLPMSts) &&*/
        (tSataDC.INT_Process != NULL))
    {
        ENUM_SATA_TRSP_TYPE type= UNKNOWN_YET;
        do
        {
            type=tSataDC.eTransType;
            tSataDC.INT_Process(&tPISR);
            __nds32__dsb();
        }while((type!=tSataDC.eTransType)&&(tPISR.dAll!=0));
        //可能同时来多个中断
    }

#if(L_PWN_MNG_EN == TRUE)
    if(tPISR.bits.PMNACK)
    {
        if(SG_DSATA->rPSCTLR.bits.ITIMER_PM == LPM_PARTIAL)
        {
            //HOST 不支持DIPM
            tSataDC.bIsDIPMEn=FALSE;
            SG_DSATA->rPSCTLR.bits.ITIMER_EN=0;
        }
        else if(SG_DSATA->rPSCTLR.bits.ITIMER_PM == LPM_SLUMBER)
        {
            //HOST不支持SLUMBER重新尝试发起PATIAL请求
            tSataDC.bIsAP2SEn=FALSE;
            SG_CPM->rPAWCTRL.bits.PAWEN =0;
            SG_DSATA->rPSCTLR.bits.ITIMER_PM=LPM_PARTIAL;//TODO:控制省电级别
        }
        //HOST send PMNACK
        UART_PRINT("HOST send PMNACK\r\n");
        //PATIAL/SLUMBER被拒绝，禁用，或者重新进入PATIAL模式
    }

    if(tPISR.bits.PMREQ_P)
    {
        UART_PRINT("tPISR.bits.PMREQ_P...\r\n");//增加打印，可能会丢失SRST中断
        //HOST 发起的进入PATIAL的请求
        SG_CPM->rPAWCTRL.bits.PAWEN =0;
        PRINT_TXT("PMREQ_P");
    }
    if(tPISR.bits.PMREQ_S)
    {
        //HOST 发起的进入SLUMBER的请求，最终会进入SLUMBER_S
        //固件提前切换到低频时钟
        UART_PRINT("tPISR.bits.PMREQ_S...\r\n");
        //LowSpeedModeEn();
        //SG_CPM->rSLEEP_CTRL.bits.SLUMBER_S_ENTER_EN=1;
        PRINT_TXT("PMREQ_S");
    }
#endif


    if(tPISR.bits.IFS)
    {

        PRINT_TXT("IFS");
        //IFS: Interface Fatal Error Status
        SATA_Fatal_Error_Handler();
    }

    if(tPISR.bits.INFS)
    {
        PRINT_TXT("INFS");
        //INFS: Interface Non-fatal Error Status
        SATA_Non_Fatal_Error_Handler();
    }    

    if(tPISR.bits.HDRS)
    {
        //HDRS: Host to Device Register FIS Interrupt
        if(SATA_SReset_Handler()==FALSE)
        {
//            //!!ERROR,PIO DMA FPDMA 状态下收到 其他命令
//            SATA_Fatal_Error_Handler();
            ERROR_PRINT("Unprocessd H2D fis!!\r\n");
            PRINT_TXT("E:UPH2D");
            DSATA_EnableRxFis();//重新使能接收
            DSATA_SendD2HFis(0x51,0x04,1);
        }
    }

    if(tPISR.bits.UFS)
    {
        //UFS: Unknown FIS Interrupt
        tPISR.bits.UFS=0;
        if(tSataDC.bCmdCnt != 0)
        {
            SATA_Fatal_Error_Handler();
        }
    }

    if(tPISR.bits.FTXDS)
    {
        //FTXDS: FIS Transmit Done Interrupt
        //PIO OUT/DMA IN/OUT/FPDMA 中出现该事件表示某一个SLOT的数据收发正确结束（R_OK收到）
    }

    if(tPISR.bits.DPS)
    {
        //DPS: Descriptor Processed
        //PIO IN/out需要该中断知道DSATA数据传输结束,分解传输任务
        //SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态 
        DSATA_Update_rPCMDR(13,1);
        //The PDMA suspends any activity until firmware clears this bit by writing 1 to it. 
    }

    if(tPISR.bits.DMATS)
    {
        //DMATS: DMAT Status
        /*This bit is set when DMATp is detected during Data FIS transmission. Firmware must
        abort the current transfer by setting the PCMDR.DTABORT bit.*/
        SG_DSATA->rPCMDR.bits.DTABORT=1;
        //状态怎么切换？这个是链路层的事情？
    }
#if 0
    if(tPISR.bits.TXOK)
    {
        //TXOK: Transmit Okay
    }
    if(tPISR.bits.OFS)
    {
        //OFS: Overflow Status
    } 
#endif

    if(tPISR.bits.MBES)
    {
        //MBES: Master Bus Error Status
        SG_DSATA->rPCMDR.bits.DTABORT=1;
        SATA_Fatal_Error_Handler();
        //这种错误什么情况下出现？
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#endif


// *****************************************************************************
// 函数名称: DSATA_Update_rPCMDR(U8 bBit,U8 bVal)
// 函数功能: 由于PCMDR寄存器的特殊性需要特别方法置位
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Update_rPCMDR(U8 bBit,U8 bVal)
{
    //除去写1清零位的干扰
    U32 mask = SG_DSATA->rPCMDR.dAll&(~DPP_BIT)&(~FRXP_BIT)&(~STD_BIT)&(~(1U<<bBit));
    SG_DSATA->rPCMDR.dAll=mask |(bVal<<bBit);
}

// *****************************************************************************
// 函数名称: DSATA_TransferStart
// 函数功能: 启动一次PRD entry的数据传输
// 入口参数:
//           slot:槽编号0-31
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_TransferStart(U8 bSlot)// suke 非常重点
{
    if(SG_DSATA->rPDTR == 0)
    {
        SG_DSATA->rPDTR = ((U32)1U<<bSlot);//启动传输
    }
}

// *****************************************************************************
// 函数名称: DSATA_TransferStart
// 函数功能: 启动一次PRD entry的数据传输
// 入口参数:
//           slot:槽编号0-31
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_TransferMultiStart(U32 dFlag)// suke 非常重点
{
    //之前未启动
    SG_DSATA->rPDTR = dFlag;//启动传输
    //非DMA模式的话清除，一定是等到数据准备好才清除
    //SG_DSATA->rPCMDR.bits.DPP=1;//清除暂停状态
    //DSATA_Update_rPCMDR(13,1);//注意，在不使用DMA1功能，即使用原生DMA1功能时可就要执行
}

// *****************************************************************************
// 函数名称: DSATA_IsPrdTransferDone
// 函数功能: 检查本次PRD传输是否结束
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
BOOL DSATA_IsPrdTransferDone(CMD_SLOT slot)
{
    return (tPrdTbls[slot].dw3.bits.I==0 || SG_DSATA->rPISR.bits.DPS == 1);
}

 // *****************************************************************************
// 函数名称: DSATA_GetCurrentCmdSlot
// 函数功能: 获取当前进行数据传输的SLOT
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
inline U8 DSATA_GetCurrentCmdSlot()
{
    return (U8)(SG_DSATA->rPCMDR.bits.CCS);
}


// *****************************************************************************
// 函数名称: DSATA_GetPrdRemainItems
// 函数功能: 使能自动发送FIS功能，对上层屏蔽硬件寄存器
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U32 DSATA_GetPrdRemainItems(void)
{
    return SG_DSATA->rPPICR;
}
// *****************************************************************************
// 函数名称: DSATA_IsAnySlotStarted
// 函数功能: 查看是否已经启动数据传输
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
BOOL DSATA_IsAnySlotStarted(void)
{
    return (SG_DSATA->rPDTR != 0);
}

// *****************************************************************************
// 函数名称: DSATA_IsAnySlotStarted
// 函数功能: 查看当前是否使能自动发送功能
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
BOOL DSATA_IsAutoSendEnabled(void)
{
    return (SG_DSATA->rPCMDR.bits.TXSAS == 1);
}

// *****************************************************************************
// 函数名称: DSATA_Enable_Auto_Send
// 函数功能: 查询bSlot任务的读写状态1：读，0：写
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U8 DSATA_GetCmdDir(U8 bSlot)
{
    return (tCmdHead[bSlot].dw0.cmd.D);
}

// *****************************************************************************
// 函数名称: DSATA_IsPioInMode
// 函数功能: 查询当前的SLOT是否是PIOIN传输模式
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U8 DSATA_IsPioInMode(U8 bSlot)
{
    return ((tCmdHead[bSlot].dw0.cmd.P==1) && (DSATA_GetCmdDir(bSlot) == DIR_IN));
}

// *****************************************************************************
// 函数名称: DSATA_Enable_Auto_Send
// 函数功能: 使能自动发送FIS功能，对上层屏蔽硬件寄存器
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_EnableAutoSend(void)
{
    SG_DSATA->rPISR.dAll=FTXDS_BIT;//清除旧的标记
    DSATA_Update_rPCMDR(15,1);
}

// *****************************************************************************
// 函数名称: DSATA_Disable_Auto_Send
// 函数功能: 禁用自动发送FIS功能，对上层屏蔽硬件寄存器
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_DisableAutoSend(void)
{
    DSATA_Update_rPCMDR(15,0);
    SG_DSATA->rPIER.bits.FTXDE=0;//ENABLE FTXDS
}

// *****************************************************************************
// 函数名称: DSATA_Enable_RxFis
// 函数功能: 重新使能接收
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_EnableRxFis(void)
{
    //SG_DSATA->rPCMDR.bits.FRXP=1;
    DSATA_Update_rPCMDR(4,1);
}

// *****************************************************************************
// 函数名称: DSATA_Enable_RxFis
// 函数功能: 重新使能接收
// 入口参数: bSlot命令号
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Wait4PrdComplete(U8 bSlot)
{
    if(((SG_DSATA->rPDTR&(1U<<bSlot)) == 1))
    {
        //已经启动数据传输
        while(SG_DSATA->rPCMDR.bits.DPP != 1);//等待上一个PRD传输结束
    }

}
// *****************************************************************************
// 函数名称: DSATA_Disable_ST
// 函数功能: 清除ST位，对上层屏蔽硬件寄存器
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_DisableST(void)
{
    SG_DSATA->rPCMDR.bits.ST=0;
}

// *****************************************************************************
// 函数名称: DSATA_EnableDPP
// 函数功能: 设置PRD table 中的Ibit位，用以产生DPP中断
// 入口参数: bSlot:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_EnableDPP(U8 bSlot)
{
    tPrdTbls[bSlot].dw3.bits.I=1;
}

// *****************************************************************************
// 函数名称: DSATA_IsSlotStarted
// 函数功能: 查询该SLOT是否已经启动
// 入口参数: bSlot:slot 编号
// 出口参数:
// 说    明:
// *****************************************************************************
BOOL DSATA_IsSlotStarted(U8 bSlot)
{
    return ((SG_DSATA->rPDTR&(1U<<bSlot)) != 0);
}
// *****************************************************************************
// 函数名称: DSATA_Enable_INT
// 函数功能: 使能中断
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Enable_INT(void)
{
    SG_DSATA->rGCR.bits.IE=1;
}

// *****************************************************************************
// 函数名称: DSATA_Disable_INT
// 函数功能: 禁用中断
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Disable_INT(void)
{
    SG_DSATA->rGCR.bits.IE=0;
}


// *****************************************************************************
// 函数名称: DSATA_Trig_Count_DATAFIS
// 函数功能: 统计在指定时间内 收到DATA_FIS
// 入口参数: 以系统时钟为单位，指定时间长度统计
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Trig_Count_DATAFIS(U32* dTime)
{
	SG_DSATA->rSATA_CAL_TIMEH = *((U32*) dTime + 1);
	SG_DSATA->rSATA_CAL_TIMEL = *((U32*) dTime);   //单位系统时间
	SG_DSATA->rSATA_CAL_ST.bits.dVAL = 1; //启动

}
// *****************************************************************************
// 函数名称: DSATA_Check_DATAFISCount_Done
// 函数功能:  Check 指定时间内 统计DATA_FIS 结束
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U8 DSATA_Check_DATAFISCount_Done(void)
{
	if (1 == SG_DSATA->rSATA_CALDONE_FLAG.bits.dVAL )
	{
		SG_DSATA->rSATA_CALDONE_FLAG.bits.dVAL = 1;
		return  TRUE;
	}
	return FALSE;
}
// *****************************************************************************
// 函数名称: DSATA_Check_DATAFISCount_Done
// 函数功能: 返回 统计的DATA_FIS 的个数
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_DATAFIS_Cnt(U32* dDataFisCnt)
{
	*((U32*)dDataFisCnt    ) = SG_DSATA->rSATA_DFIS_CNTL;
	*((U32*)dDataFisCnt + 1) = SG_DSATA->rSATA_DFIS_CNTH;
}

// *****************************************************************************
// 函数名称: DSATA_DATAFISCount_Abort
// 函数功能:  终止统计
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_DATAFISCount_Abort(void)
{
	SG_DSATA->rSATA_CAL_STOP.bits.dVAL = 1;
}

#ifdef   SC_CRC_PASS_EN
// *****************************************************************************
// 函数名称: DSATA_CRCPASS_Index
// 函数功能: 返回 CRC_PASS_Index
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U32 DSATA_CRCPASS_Index(void)
{
	return  (U32)SG_DSATA->rCRC_PASS_INDEX;
}

// *****************************************************************************
// 函数名称: DSATA_CRCPASS_MIN
// 函数功能:
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_CRCPASS_MIN(U32 dCrcPaccMin)
{
	SG_DSATA->rCRC_PASS_MIN = dCrcPaccMin;
}

// *****************************************************************************
// 函数名称: DSATA_Check_CRCFail_Flg
// 函数功能: Check  data CRC Faill标志
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U8 DSATA_Check_CRCFail_Flg(void)
{
	return SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL;
}

// *****************************************************************************
// 函数名称:  DSATA_Clean_CRCPASS_INDEX
// 函数功能:  Clean CRC_FAIl flg 和 CRC_PASS_Index
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Clean_CRCPASS_INDEX(void)
{
	SG_DSATA->rCRC_PASS_INDEX = 0;
	SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL = 0;
}
#endif  //  SC_CRC_PASS_EN


#ifdef  SC_WP_4K_PRE
// *****************************************************************************
// 函数名称:  DSATA_PRE_Trig_Tag
// 函数功能:  返回 预先触发4K 写的Tag 号
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
U8 DSATA_PRE_Trig_Tag(void)
{
	return (U8)SG_DSATA->rPRE_TRIG_CNT ;
}

// *****************************************************************************
// 函数名称:  DSATA_Clean_TAG
// 函数功能:  Clean 4K 的标记和Tag 号
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_Clean_TAG(void)
{

	SG_DSATA->rPRE_TRIG_CNT = 0;//  Tag 号初始化为33
}
#endif  //SC_WP_4K_PRE

