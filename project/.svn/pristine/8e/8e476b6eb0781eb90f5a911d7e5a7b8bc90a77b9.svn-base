// *****************************************************************************
//  版权所有  (C) 2016,  硅格半导体有限公司
//   
//  文件名称：  SATA_typedef.h
//  文件标识：  
//  内容摘要：  sata 协议相关结构定义
//  其它说明：  
//  当前版本：  v0.20
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2016年07月21日
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef _SATA_TYPEDEF_H_
#define _SATA_TYPEDEF_H_
/******************************包头文件******************************/

/***************************相关功能宏开关*****************************/

/*
 * SATA function cofiguration,
 * !!!Do not change when satalib is used.
 *
 * */
/********************/
//    Sun 添加的功能控制宏
//#define   CHECK_CMD_SEQ_EXE

/******************************类型和宏定义******************************/

#define  CRC_PASS_DISABLE	 	(0U)       // 关了CRC_PASS 才启动DMA1 传输功能
#define  FIRST_CRC_PASS		 	(1U)       // 第一级CRC PASS 之后才HW启动DMA1 传输    FIFO 之后
#define  EXBUFF_CRC_PASS		(2U)       // 第二级CRC PASS 之后才HW启动DMA1 传输    Buff 之后    （只选其一）

/*********************************Test 的宏定义  START *********************/
#ifdef  SATA_DRIVE_NEW
#ifndef __BOOT_CODE__
	#define  SC_H2DFIFO_EN

//	#define  SC_REORDER_DLY_EN   	    // REORDER _delay 的控制宏
	#define  SC_H2DINT_BLOCK_EN  	    // H2D 中断标志卡 DMASETUP 的控制宏
//	#define  SC_SDB_DLY_EN       	    // SDB _DELAY 的控制宏
	#define  SC_CRC_PASS_EN				//   过了CRC 加上 CRC_PASS_INDEX

	//#define  SC_PORT_MULTIPLIER

//	#define  SC_TRIG_DMASTP_EN 	      //等DMA2 的数据准备好之后再启动DMASETUP
#ifndef __JUPITER_DEBUG__
	#define  SC_EXBUFF_EN               //二级Buff 的控制宏
	#define  SC_WP_4K_PRE				//   4K 写命令预收功能开关，默认三条命令，下面是只预收一条命令
	#define  SC_CRC_PASS_ST_DMA1 	EXBUFF_CRC_PASS
#else
	#define  SC_CRC_PASS_ST_DMA1 	CRC_PASS_DISABLE
#endif
#if  (SC_CRC_PASS_ST_DMA1 == EXBUFF_CRC_PASS)
	#define  SC_HIDDEN_CMD				//
#endif
	// 异常验证控制宏
//	#define  _SATA_TRIGGER_CRC__		// 触发CRC   DATA FIS 和  NON DATA FIS
//	#define __FW_TRIGGER_SATA_CRC__		// FW 触发SATA  CRC
    #define _SATA_DEBUG_MESSAGE_
#else
	#define	 SC_H2DFIFO_EN
	#define  SC_CRC_PASS_EN				//   过了CRC 加上 CRC_PASS_INDEX
#endif
#endif

#ifdef    SC_CRC_PASS_EN
//  #define   SCSI_CHECK_PASS_IDX_ZERO
#endif

/***********************************/

#ifndef __BOOT_CODE__
	#define __NCQ_QUEUE_NEW__
    #define D2H_AUTO_SEND	    (TRUE)
    #define CRC_ERR_HANDLE      (TRUE)
#else
    #define __FPDMA_DISABLE__
    #define D2H_AUTO_SEND	    (FALSE)
    #define CRC_ERR_HANDLE      (FALSE)
#endif


#define CMD_REORDER_EN	    	(FALSE)
#define SDB_AUTO_SEND       	(TRUE)

#if(SDB_AUTO_SEND == TRUE)
#define __SDB_AUTO_ACC__
#endif

#ifndef __BOOT_CODE__
    #define L_PWN_MNG_EN        (TRUE)  /*使能link电源管理*/
    #define L_DIPM_EN           (TRUE)  /*是否使能DEVICE INIT PM*/
    #define L_HIPM_EN           (TRUE)  /*是否使能HOST INIT PM*/
    #define ASR_DISABLE			(TRUE)
#else
    #define L_PWN_MNG_EN        (FALSE)  /*使能link电源管理*/
    #define L_DIPM_EN           (FALSE)  /*是否使能DEVICE INIT PM*/
    #define L_HIPM_EN           (FALSE)  /*是否使能HOST INIT PM*/
    #define ASR_DISABLE			(FALSE)
#endif

#define PWM_CLK_GATING_EN   	(FALSE)  /*省电模式下是否需要停止时钟,停止时钟的时候要特别注意FLASH 端的操作*/
#define SATA_DEBUG_ON	    	(FALSE)

#ifdef __SATA_LOG_EN__
#define SATA_LOG_ENABLE     	(1)  /*是否开启SATA LOG 功能*/
#else
#define SATA_LOG_ENABLE     	(0)  /*是否开启SATA LOG 功能*/
#endif

#if(SATA_LOG_ENABLE == 1)
    #define SATA_PIO_NEW		(TRUE)/*PIO 命令新的传输机制*/
    #define SATA_DCO_ENABLE 	(FALSE) /*DEVICE CONFIG OVERLAY feature set*/
    #define SATA_HPA_ENABLE 	(TRUE) /*Host Protected Area feature set*/
    #define SATA_SEC_ENABLE 	(TRUE) /*SECURITY feature set*/
    #define SATA_SCT_ENABLE 	(TRUE) /*SCT feature set*/
    #define SATA_SD_ENABLE 		(TRUE) /*SANITIZE DEVICE feature set*/
	//#define SATA_RECNT_ENABLE	/*sata 接口主动重连机制，应对工控主板问题*/
#else
    #define SATA_PIO_NEW		(FALSE)/*PIO 命令新的传输机制*/
    #define SATA_DCO_ENABLE 	(FALSE)
    #define SATA_SEC_ENABLE 	(FALSE)
    #define SATA_SD_ENABLE 		(FALSE)
    #define SATA_SCT_ENABLE 	(FALSE)
#endif
/******************************************************************/
#ifndef DIR_IN
#define DIR_IN          (1U)
#endif

#ifndef DIR_OUT
#define DIR_OUT         (0U)
#endif

#ifndef DEVICE_CAPCITY
#define DEVICE_CAPCITY      (0x00400000U) /*默认 设备上报的容量*/
#endif

#define PRE4K_CMD_NUM         (1U)		  /*预收的命令 最多一条*/
#ifndef MAX_CMD_NUM
#define MAX_CMD_NUM         (32U)		  /*默认支持的NCQ 深度*/
#endif

#ifndef CALL_BACK
#define CALL_BACK
#endif

#define SATA_CB_VALID_FLAG      (0x12345678)

/*************************基本类型定义************************/
#ifndef U8
#define    U8      unsigned    char            //unsigned 8bit
#endif

#ifndef U16
#define    U16     unsigned    short        //unsigned 16bit
#endif

#ifndef U32
#define    U32     unsigned 	long            //unsigned 32bit
#endif

#ifndef U64
#define    U64     unsigned    long  long    //unsigned 64bit
#endif

#ifndef U8_V
#define    U8_V    volatile	   U8
#endif

#ifndef U16_V
#define    U16_V    volatile	   U16
#endif
#ifndef U32_V
#define    U32_V    volatile	   U32
#endif
#ifndef U64_V
#define    U64_V    volatile	   U32
#endif


#ifndef U8_C
#define    U8_C    const	   U8
#endif

#ifndef U16_C
#define    U16_C    const	   U16
#endif

#ifndef U32_C
#define    U32_C    const	   U32
#endif

#ifndef U64_C
#define    U64_C    const	   U64
#endif

#ifndef	   BOOL
#define    BOOL    U8
#endif

#ifndef	   TRUE
#define    TRUE    (1U)
#endif

#ifndef	   FALSE
#define    FALSE    (0U)
#endif

#ifndef NULL
#define    NULL   (0)
#endif
//传输层通用请求接口参数定义
typedef union _BUFFER
{
    U8  bArray[512];
    U16 wArray[512/2];
    U32 dArray[512/4];
}T_BUFFER,*pT_BUFFER;

typedef enum _CMD_SLOT{
    CMD_SLOT0=0,
    CMD_SLOT1,
    CMD_SLOT2,
    CMD_SLOT3,
    CMD_SLOT4,
    CMD_SLOT5,
    CMD_SLOT6,
    CMD_SLOT7,
    CMD_SLOT8,
    CMD_SLOT9,
    CMD_SLOT10,
    CMD_SLOT11,
    CMD_SLOT12,
    CMD_SLOT13,
    CMD_SLOT14,
    CMD_SLOT15,
    CMD_SLOT16,
    CMD_SLOT17,
    CMD_SLOT18,
    CMD_SLOT19,
    CMD_SLOT20,
    CMD_SLOT21,
    CMD_SLOT22,
    CMD_SLOT23,
    CMD_SLOT24,
    CMD_SLOT25,
    CMD_SLOT26,
    CMD_SLOT27,
    CMD_SLOT28,
    CMD_SLOT29,
    CMD_SLOT30,
    CMD_SLOT31,
	CMD_SLOT32,
}CMD_SLOT;

typedef enum 
{
    POWERON_COMRESET=0,
    DEV_IDLE,
    SRESET,//software reset
    EXECUTE_DEVICE_DIGA,//EXECUTE DEVICE DIAGNOSTIC command protocol
    DEV_RESET,
    NONE_DATA,
    PIO_DATA_IN,
    PIO_DATA_OUT,
    DMA_DATA_IN,
    DMA_DATA_OUT,
    PACKET,
    READ_DMA_QUEUED,
    WRITE_DMA_QUEUED,
    FPDMA_QUEUED,
    UNKNOWN_YET,
    UNDEFINE,
}ENUM_SATA_TRSP_TYPE;//transport type

/*
 * Media access required 都要回到ACTIVE状态
 * */
//Device Power state
typedef enum
{
    DPS_IDLE=0,
    DPS_ACTIVE,
    DPS_SLEEP,
    DPS_STANBY,
}ENUM_DPS;//Device Power state

//Device Power state
typedef enum
{
    LPS_OFF_LINE=0,// Host not present or communication not established or lost
    LPS_ACTIVE,
    LPS_PARTIAL,
    LPS_SLUMBER=6,
    LPS_DEV_SLEEP,
}ENUM_LPS;//link Power state

//Device PHY state
typedef enum
{
    PHYS_NOT_DET=0,// Host not present or communication not established or lost
    PHYS_COMREST_DET,
    PHYS_READY=3,
    PHYS_OFF_LINE,
}ENUM_PHYS;//PHY  state

//Device HW Reset
typedef enum
{
    DHR0_HW_RESET_ASSERTED=0,//COMRESET中断，上电初始化为这个状态
    DHR1_EXECUTE_DIGAGNOSTICS,
    DHR2_SEND_GOOD_STATUS,
    DHR3_SEND_BAD_STATUS,
}ENUM_DHR;//PON and comreset status type

//Device Idle
typedef enum
{
    DI0_DEVICE_IDLE=0,
    DI1_CHECK_FIS,
    DI2_CHECK_CMD,
    DI3_NO_CMD,
    DI4_SET_SERVICE,
    DI5_SERVICE_TEST,
    DI6_SERVICE_SEND_TAG,
    DI7_SERVICE_DECODE,
}ENUM_DI;//Device idle protocol

//Device SW Reset
typedef enum
{
    DSR0_SOFT_RESET_ASSERT=0,
    DSR1_EXECUTE_DIAGNOSTICS,
    DSR2_SEND_GOOD_STATUS,
    DSR3_SEND_BAD_STATUS,
}ENUM_DSR;//Software reset protocol

//Device Execute Device Diagnostic
typedef enum
{
    DEDD0_EXECUTE_DEVICE_DIAG=0,
    DEDD1_SEND_GOOD_STATUS,
    DEDD2_SEND_BAD_STATUS,
}ENUM_DEDD;//EXECUTE DEVICE DIAGNOSTIC command protocol

//Device Device Reset
typedef enum
{
    DDR0_DEVICE_RESET=0,
    DDR1_SEND_GOOD_STATUS,
}ENUM_DDR;//DEVICE RESET command protocol

//Device Non-Data
typedef enum
{
    DND0_NON_DATA=0,
    DND1_SEND_STATUS,
}ENUM_DND;//Non-data command protocol

//Device PIO In
typedef enum
{
    DPIOI0_PIO_IN=0,
    DPIOI1_SEND_PIO_SETUP,
    DPIOI2_TRANSMIT_DATA,
    DPIOI3_ERROR_STATUS,
}ENUM_DPIOI;//PIO data-in command protocol

//Device PIO OUT
typedef enum
{
    DPIOO0_PIO_OUT=0,
    DPIOO1_SEND_PIO_SETUP,
    DPIOO2_RECEIVE_DATA,
    DPIOO3_SEND_STATUS,
}ENUM_DPIOO;//PIO data-out command protocol

//Device DMA data in command protocol
typedef enum
{
    DDMAI0_DMA_IN=0,
    DDMAI1_SEND_DATA,
    DDMAI2_SEND_STATUS,
}ENUM_DDMAI;//DMA data in command protocol

//Device DMA data OUT command protocol
typedef enum
{
    DDMAO0_DMA_OUT=0,
    DDMAO1_DMA_ACTIVE,
    DDMAO2_RECEIVE_DATA,
    DDMAO3_SEND_STATUS,
}ENUM_DDMAO;//DMA data out command protocol

//Device Packet
typedef enum
{
    DPKT0_PACKET=0,

}ENUM_DPKTO;//PACKET protocol

//Device DMA QUEUED data In command protocol
typedef enum
{
    DDMAQI0_DMA_QUEUED_IN=0,
    DDMAQI1_SEND_DATA,
    DDMAQI2_PREPARE_DATA,
    DDMAQI3_SEND_STATUS,
    DDMAQI4_RELEASE,
}ENUM_DDMAQI;//READ DMA QUEUED command protocol

//Device DMA data QUEUED OUT command protocol
typedef enum
{
    DDMAQO0_DMA_OUT=0,
    DDMAQO1_DMA_ACTIVE,
    DDMAQO2_RECEIVE_DATA,
    DDMAQO3_PREPARE_DATA_BUFF,
    DDMAQO4_SEND_STATUS,
    DDMAQO5_RELEASE,
}ENUM_DDMAQO;//WRITE DMA QUEUED command protocol

//Device FPDMA Queued
typedef enum
{
    DFPDMAQ0_UnDefine=0,
    DFPDMAQ1_AddCommandToQueue,
    DFPDMAQ2_ClearInterfaceBsy,
    DFPDMAQ3_DataPhaseReadSetup,
    DFPDMAQ4_DataPhasePreWriteSetup,
    DFPDMAQ5_DataPhase_WriteSetup,
    DFPDMAQ6_DataPhase_OldWriteSetup,
    DFPDMAQ7_DataPhase_XmitActivate,
    DFPDMAQ8_DataXmitRead,
    DFPDMAQ9_DataXmitWrite,
    DFPDMAQ10_SendStatus,
    DFPDMAQ11_ERROR,
    DFPDMAQ12_BrokenHost_ClearBusy,
    DFPDMAQ13_WaitforClear,
    DFPDMAQ14_SendQueue_CleanACK,
    DFPDMAQ15_SendQueue_CleanACKDMA,
}ENUM_DFPDMAQ;//FPDMA command protocol

typedef union
{
    ENUM_DHR     eHDR;
    ENUM_DI      eDI;
    ENUM_DSR     eDSR;
    ENUM_DEDD    eDEDD;
    ENUM_DDR     eDDR;
    ENUM_DND     eDND;
    ENUM_DPIOI   eDPIOI;
    ENUM_DPIOO   eDPIOO;
    ENUM_DDMAI   eDDMAI;
    ENUM_DDMAO   eDDMAO;
    ENUM_DPKTO   eDPKTO;
    ENUM_DDMAQI  eDDMAQI;
    ENUM_DDMAQO  eDDMAQO;
    ENUM_DFPDMAQ eDFPDMAQ;
    U8 bState;
}UN_DEV_TRANS_STATES;

//传输层请求类型
typedef enum
{
   REQ_CONFIG=0,//FTL 任务分解
   REQ_TRANSFER,//ATA命令不经过FTL直接数据传输
   REQ_STATUS//ATA命令不经过FTL直接数据传输
}REQ_TYPE;

typedef enum
{
    ATA_SEC0=0,
    ATA_SEC1,//POWERON DISABLE
    ATA_SEC2,
    ATA_SEC3,
    ATA_SEC4,//POWERON ENABLE
    ATA_SEC5,
    ATA_SEC6,
}ENUM_ATA_SECURITY_STATE;//security state define

typedef enum  __attribute__((__packed__))
{
    DCO_Factory_config=0,
    DCO_locked,
    DCO_Reduced_config,
}ENUM_DCO_STATE;//Device Configuration overlay state define

typedef enum __attribute__((__packed__))
{
    //出错状态，比特位置跟ATA8 ACS2 中的ERROR对应，这样右移对应的位就能得到ERROR字段
    CMD_MISC_ERR=0,
                    /*
                     * ILLEGAL LENGTH INDICATOR bit 6.3.6
                        COMMAND COMPLETION TIME OUT bit 6.3.3
                        CFA ERROR bit CFA-CF
                    */
    CMD_EOM,//END OF MEDIA bit
    CMD_ABORT,
    CMD_RFU0,
    CMD_ID_NOT_FOUND,
    CMD_RFU1,
    CMD_UNCORRECT_ERR,
    CMD_CRC_ERR,

    //正常执行状态
    CMD_IDLE=10,
    CMD_VALID,//参数被验证正确
    CMD_ONGOING,
    CMD_PENDING,
    CMD_DONE,//子任务完成
    CMD_COMPLETED,//整个任务完成
}CMD_STATUS;

typedef union __MP_CFG
{
    U32 dAll;//预留扩展接口
    struct{
        U32 SATA_SPD:2;/*MAX SATA SPEED
                                0:GEN3
                                1:GEN1
                                2:GEN2
                                3:GEN3
                         */
        U32 C7_DIS:1;/*1:SMART C7 DISABLE,
                             0:SMART C7 ENABLE
                        */
        U32 C7_FIX:1;/* 1:SMART C7 FIX 0
                        0:NORMAL
                         */
        U32 T_FIX:1;/* 1:TEMPERATURE FIX
                         0:NORMAL
                         */
        U32 C3_DIS:1;
        U32 C3_FIX:1;

        U32 RFU1:1;
        U32 T_VAL:8;/* 0-7,TEMPERATURE FIX VAL*/
        U32 T_MAX:8;/* 0-7,TEMPERATURE MAX VAL*/
        U32 RFU2:8;
    }bits;
}T_MP_CFG;

typedef union _PISR
{
    U32 dAll;
    struct
    {
        U32 HDRS:1;/*bit0 HDRS: Host to Device Register FIS Interrupt*/
        U32 FTXDS:1;/*bit1 FTXDS: FIS Transmit Done Interrupt*/
        U32 UFS:1;/*bit2 UFS: Unknown FIS Interrupt*/
        U32 DPS:1;/*bit3 DPS: Descriptor Processed*/
        U32 DMATS:1; /*bit4 DMATS: DMAT Status*/
        U32 TXOK:1;/*bit5 TXOK: Transmit Okay*/
        U32 PMNACK:1;/*bit6 PMNACK: device收到HOST的休眠请求，回复PMNAK中断*/
        U32 Reserved0:1;/*bit7*/
        U32 PCS:1;/*bit8 PCS: Port COMRESET Status*/
        U32 LSCS:1;/*bit9 LSCS: Link State Change Status*/
        U32 PMREQ_P:1; /*bit10*/
        U32 PMREQ_S:1; /*bit11*/
        U32 Reserved1:12; /*bit12-23*/
        U32 OFS:1;/*bit24 OFS: Overflow Status*/
        U32 INFS:1;/*bit25 INFS: Interface Non-fatal Error Status*/
        U32 IFS:1;/*bit26 IFS: Interface Fatal Error Status*/
        U32 Reserved2:1; /*bit27*/
        U32 MBES:1;/*bit28 MBES: Master Bus Error Status*/
        U32 CMDONS:1;
        U32 COMWKS:1;
        U32 REORDST:1;/*bit29-31*/
    }bits;
}T_PISR;

typedef union _PSERR
{
    unsigned long dAll;
    struct
    {
        unsigned long ERR_I:1;/*bit0 ERR_I: Recovered Data Integrity*/
        unsigned long ERR_M:1;/*bit1 ERR_M: Recovered Communication Error*/
        unsigned long Reserved0:6;/*bit2-7*/
        unsigned long ERR_T:1; /*bit8 ERR_T: Non-recovered Transient Data Integrity Error*/
        unsigned long ERR_C:1; /*bit9 ERR_C: PHY Not Ready Error*/
        unsigned long ERR_P:1; /*bit10 ERR_P: Protocol Error*/
        unsigned long ERR_E:1; /*bit11 ERR_E: Internal Bus Error*/
        unsigned long Reserved1:5;/*bit12-16*/
        unsigned long DIAG_I:1;/*bit17 DIAG_I: PHY Internal Error*/
        unsigned long DIAG_W:1;/*bit18 DIAG_W: Comm Wake*/
        unsigned long DIAG_B:1;/*bit19 DIAG_B: 10B to 8B Decode Error*/
        unsigned long DIAG_D:1;/*bit20 DIAG_D: Disparity Error*/
        unsigned long DIAG_C:1;/*bit21 DIAG_C: CRC Error*/
        unsigned long DIAG_H:1;/*bit22 DIAG_H: Handshake Error*/
        unsigned long DIAG_S:1;/*bit23 DIAG_S: Link Sequence Error*/
        unsigned long DIAG_T:1;/*bit24 DIAG_T: Transport State Transition Error*/
        unsigned long Reserved2:6; /*bit25-30*/
        unsigned long RXFOV:1; /*bit23 RXFOV: RXFIFO Overflow Error*/
    }bits;
}T_PSERR;

//Host to Divece FIS define
typedef struct _T_H2D
{
    union _T_H2D_dw0
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    Type;
            volatile U8    PM_port: 4;
            volatile U8    R: 3;
            volatile U8    C: 1;
            volatile U8    Cmd;
            volatile U8    bFeatures_L; //Features 7:0
        } bits;
    }dw0;

    union _T_H2D_dw1
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bLBA_low; //LBA(7:0):LBA low register
            volatile U8    bLBA_mid; //LBA(15:8):LBA mid register
            volatile U8    bLBA_high; //LBA(23:16):LBA high register
            volatile U8    bDevice; //Device(7:0)
        } bits;
    }dw1;

    union _T_H2D_dw2
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bExpLBA_low; //expanded address field LBA(31:24):LBA low register
            volatile U8    bExpLBA_mid; //expanded address field LBA(39:32):LBA mid register
            volatile U8    bExpLBA_high; //expanded address field LBA(47:40):LBA high register
            volatile U8    bFeatures_H; //Features(15:8)
        } bits;
    }dw2;

    union _T_H2D_dw3
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bCount; //Count(7:0)
            volatile U8    bExpCount; //Count(15:8)
            volatile U8    bICC; //ICC(7:0)
            volatile U8    bCtrl; //Control(7:0)
        } bits;
    }dw3;

    union _T_H2D_dw4
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bAuxiliary0; //Auxiliary(7:0)
            volatile U8    bAuxiliary1; //Auxiliary(15:8)
            volatile U8    bAuxiliary2; //Auxiliary(23:16)
            volatile U8    bAuxiliary3; //Auxiliary(31:24)
        } bits;
    }dw4;
}T_H2D,*pT_H2D;

//Divece to host FIS define
typedef struct _T_D2H
{
    union _T_D2H_dw0
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bType;
            volatile U8    PM_port: 4;
            volatile U8    Reserved1: 2;
            volatile U8    I: 1; //Interrupt
            volatile U8    Reserved2: 1;
            volatile U8    bStatus;    //Status 7:0
            volatile U8    bError;
        } bits;
    }dw0;

    union _T_D2H_dw1
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bLBA_low; //LBA(7:0):LBA low register
            volatile U8    bLBA_mid; //LBA(15:8):LBA mid register
            volatile U8    bLBA_high; //LBA(23:16):LBA high register
            volatile U8    bDevice; //Device(7:0)
        } bits;
    }dw1;

    union _T_D2H_dw2
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bExpLBA_low; //expanded address field LBA(31:24):LBA low register
            volatile U8    bExpLBA_mid; //expanded address field LBA(39:32):LBA mid register
            volatile U8    bExpLBA_high; //expanded address field LBA(47:40):LBA high register
            volatile U8    bFeatures; //Features(15:8)
        } bits;
    }dw2;

    union _T_D2H_dw3
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bCount; //Count(7:0)
            volatile U8    bExpCount; //Count(15:8)
            volatile U8    bReserved1;
            volatile U8    bReserved2;
        } bits;
    }dw3;

    union _T_D2H_dw4
    {
        volatile U32 dAll;
        struct
        {
            volatile U32 dReserved1;
        } bits;
    }dw4;
}T_D2H,*pT_D2H;

//PIO Setup FIS define
typedef struct _T_PIO_SETUP
{
    union _T_PIO_SETUP_dw0
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bType;
            volatile U8    PM_port: 4;
            volatile U8    Reserved1: 1;
            volatile U8    D: 1; //Dirrection 1=device o host
            volatile U8    I: 1; //Interrupt
            volatile U8    Reserved2: 1;
            volatile U8    bStatus;
            volatile U8    bError;
        } bits;
    }dw0;

    union _T_PIO_SETUP_dw1
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bLBA_low; //LBA(7:0):LBA low register
            volatile U8    bLBA_mid; //LBA(15:8):LBA mid register
            volatile U8    bLBA_high; //LBA(23:16):LBA high register
            volatile U8    bDevice; //Device(7:0)
        } bits;
    }dw1;

    union _T_PIO_SETUP_dw2
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bExpLBA_low; //expanded address field LBA(31:24):LBA low register
            volatile U8    bExpLBA_mid; //expanded address field LBA(39:32):LBA mid register
            volatile U8    bEpLBA_high; //expanded address field LBA(47:40):LBA high register
            volatile U8    bReserved1;
        } bits;
    }dw2;

    union _T_PIO_SETUP_dw3
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bCount; //Count(7:0)
            volatile U8    bExpCount; //Count(15:8)
            volatile U8    bReserved1;
            volatile U8    bE_status; //E_Status(7:0)
        } bits;
    }dw3;

    union _T_PIO_SETUP_dw4
    {
        volatile U32 dAll;
        struct
        {
            volatile U16 wTrascnt; //Transfer Count
            volatile U16 wReserved2;
        } bits;
    }dw4;
}T_PIO_SETUP,*pT_PIO_SETUP;

//BIST FIS define
typedef struct _T_BIST
{
    union _T_BIST_dw0
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bType;
            volatile U8    PM_port: 4;
            volatile U8    Reserved1: 4;
            volatile U8    V: 1; //Vendor specfic
            volatile U8    Reserved2: 1;
            volatile U8    P: 1; //Primitive bit.
            volatile U8    F: 1; //Far End Analog.
            volatile U8    L: 1; //Far End Retimed Loopback* Transmitter shall insert additional ALIGNP primitives
            volatile U8    S: 1; //Bypass Scrambling (valid only in combination with T Bit)
            volatile U8    A: 1; //ALIGNP Bypass (Do not Transmit ALIGNP primitives) (valid only in combination with T Bit)
            volatile U8    T: 1; // Far end transmit only mode
            volatile U8    bReserved3;
        } bits;
    }dw0;

    union _T_BIST_dw1
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bData1_0; //Data1 (7:0)
            volatile U8    bData1_1; //Data1 (15:8)
            volatile U8    bData1_2; //Data1 (23:16)
            volatile U8    bData1_3; //Data1 (31:24)
        } bits;
    }dw1;

    union _T_BIST_dw2
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bData2_0; //Data2 (7:0)
            volatile U8    bData2_1; //Data2 (15:8)
            volatile U8    bData2_2; //Data2 (23:16)
            volatile U8    bData2_3; //Data2 (31:24)
        } bits;
    }dw2;

    volatile U32 Reserved[2]; //not defined
}T_BIST,*pT_BIST;

//Set Device Bits FIS define
typedef struct _T_SDB
{
    volatile U32 Reserved[6]; //unused
    union _T_SDB_dw0
    {
        volatile U32 dAll;
        struct
        {
            volatile U8    bType;
            volatile U8    PM_port: 4;
            volatile U8    Reserv1: 2;
            volatile U8    I: 1; //interrupt
            volatile U8    N: 1; //IntEvent Bit. This bit signals the host that the device needs attention.
            volatile U8    Status_Lo: 3; //Contains the new value of bits 2, 1, and 0 of the Status register.
            volatile U8    Reserved2: 1;
            volatile U8    Status_Hi: 3; // Contains the new value of bits 6, 5, and 4 of the Status register
            volatile U8    Reserved3: 1;
            volatile U8    bError;
        } bits;
    }dw0;
    union _T_SDB_dw1
    {
        volatile U32 dAll;
        volatile U32 dSActive;
        struct
        {
            volatile U16 wPortBitMap; //Port bit map
            volatile U16 wReserved;
        }bits;
    }dw1;
}T_SDB,*pT_SDB;


//SATA与FTL交互事件种类定义
typedef enum __attribute__ ((__packed__))
{
    E_UNDEFINE=0,
    E_TRANS_WO_DMA,
    E_TRANS_TIMEOUT,
    E_PMG_ON,	//power manage on
    E_PMG_OFF,	//power manage off
    E_UPDATE_REORDER,
    E_UPDATE_PAUSE
}E_TYPE;


typedef enum __attribute__ ((__packed__))
{
    E_HI_TASK=0,
    E_DI_TASK,
}E_QUEUE_TYPE;

//48bit/28bit mode
typedef enum __attribute__ ((__packed__))
{
    MODE_48BIT=0,
    MODE_28BIT,
}ADDR_MODE;

//SANITIZE DEVICE STATE
typedef enum __attribute__ ((__packed__))
{
    S_SD0=0,
    S_SD1,
    S_SD2,
    S_SD3,
    S_SD4,
}T_SD;

//48bit/28bit mode
typedef enum __attribute__ ((__packed__))
{
    MODE_UNLOCKED=0,
    MODE_LOCKED,
}SEC_MODE;

//任务队列种类
typedef enum __attribute__((__packed__))
{
    OP_HI_DATA=0,//Host init R/W cmd
    OP_HI_TRIM,  //Host init TRIM cmd
    OP_HI_FLUSH, //Host init FLUSH cmd
    OP_HI_IDFY,  //Host init IDENTIFY cmd
    OP_HI_GET_SMART_DATA,//read smart data 触发更新

    /*设备电源管理模式*/
    OP_HI_DPM_IDLE=10,
    OP_HI_DPM_ACTIVE,
    OP_HI_DPM_STANBY,
    OP_HI_DPM_SLEEP,
    OP_HI_DATA_FATAL_ERR,

    OP_DI_DATA=20,//Device init的数据操作
    OP_DI_WAIT4_FTL_INIT_DONE,
    OP_DI_FLUSH,
    OP_DI_SWITCHCODE,
    OP_DI_RMV_ALLTASK,
    OP_DI_GET_ROM,
    OP_DI_PIO_DATA,
    OP_DI_DMA_DATA,
    OP_DI_ERASE_UNIT,
    OP_DI_SANITIZE_DEVICE,
    OP_DI_DUAL_SCSI_INT,
    OP_DI_DUAL_SCSI_IDLE,
    OP_DI_DEVSLP_REQ,
    OP_DI_STANBY_REQ,
    OP_NONE_EVENT,
    //待增加
}CMD_OP;

typedef struct __DUMP_SECTION
{
    U32 dStartAddr;
    U32 dSize;
}T_DUMP_SECTION;

#if 0
typedef struct
{
	U32 dLba;//数据所在逻辑地址    LBA[31:0]
	U16 wLBA_exp;//数据所在逻辑地址  LBA[48:32]
	U8  bNcqCmdDir:1;       //数据大小
    U8  b4KCmdFlag:1;  //预收 4K 命令标志
	U8  bTag:6;    //NCQ 任务的时候用来记录该任务的TAG号，任务号直接跟CMD的TAG 号对应， 如：TAG 0的命令参数放置在任务0号队列中
    U8  bIsUsed:1; //表示当前任务是否有效（入队列）
    U8  bCmdPre4K:1;  //预收 4K 命令标志
    U8  bFua:6;   // Force Unit access ,表示本次数据是直接通过访问存储媒体获得，不能是缓冲区的数据
    U32 dSctSize;//数据大小
    U8  bDir;	 //用来表示数据传输的方向，DIR_IN 1:HOST 读，DIR_OUT 0: HOST 写，该方向定义与DMA1方向相反 0xff
    U8  bRFU[3];
    U32 dBuffAddr;//DI 发起的写任务的数据源地址，读任务的目标地址

#ifndef __NCQ_QUEUE_NEW__
    U64 qwTaskNum;//记录当前HOST 发起的任务的任务编号
#else
    U32 dDmaIdx;//FTL写任务排序的时候，记录该CMD数据最终对应的DMA IDX首地址
    U32 dTime;//任务入队列的时间点
#endif
}T_HI_TASK_PARAM;
#else
typedef struct
{
	U32 dLba;//数据所在逻辑地址    LBA[31:0]
	U16 wLBA_exp;//数据所在逻辑地址  LBA[48:32]
	U8  bTag:6;    //NCQ 任务的时候用来记录该任务的TAG号，任务号直接跟CMD的TAG 号对应， 如：TAG 0的命令参数放置在任务0号队列中
//	U8  bNcqCmdDir:1;       //数据大小
//  U8  bReserved0:1;
	U8  bDir:2;

    U8  b4KCmdFlag:1;  //  4K sect 命令标志
    U8  bIsUsed:1; //表示当前任务是否有效（入队列）
    U8  bCmdPre4K:1;  //预收  命令标志
    U8  bFua:5;   // Force Unit access ,表示本次数据是直接通过访问存储媒体获得，不能是缓冲区的数据
    U32 dSctSize;//数据大小
//    U8  bDir;	 //用来表示数据传输的方向，DIR_IN 1:HOST 读，DIR_OUT 0: HOST 写，该方向定义与DMA1方向相反 0xff
//    U8  bRFU[3];
    U32 dBuffAddr;//DI 发起的写任务的数据源地址，读任务的目标地址

#ifndef __NCQ_QUEUE_NEW__
    U64 qwTaskNum;//记录当前HOST 发起的任务的任务编号
#else
    U32 dDmaIdx;//FTL写任务排序的时候，记录该CMD数据最终对应的DMA IDX首地址
    U32 dTime;//任务入队列的时间点
#endif
}T_HI_TASK_PARAM;
#endif

typedef struct _TASK
{
    CMD_STATUS eStatus; //表示命令执行结果的状态，如果出错需要赋值相应的状态
    CMD_OP bOpcode;	//表示任务的具体内容
    BOOL b4KFlag;
    BOOL bIsContinue;
    T_HI_TASK_PARAM tParam;//任务参数
}T_HI_TASK,*pT_HI_TASK;

typedef void (TaskCompleteCb_t)(CMD_STATUS eStatus,void *ptParam);
typedef void (TransferCompleteCb_t)(CMD_STATUS eStatus,void *ptTaskParam);
//设备当前激活的传输层状态机
typedef void SATA_XXX_StateMachine(void);
//设备当前激活的中断处理接口
typedef void SATA_XXX_IntEventHandler(T_PISR *ptPISR);
//当前激活的ATA命令处理接口
typedef void ATACmd_xxx_handler(void);

//设备当前激活的传输层数据或者状态收发请求接口
typedef void SATA_XXX_DataTransferReq(REQ_TYPE eType,
                                      T_HI_TASK *ptTask,
                                      TransferCompleteCb_t *pfTransferCompleteCb);/*数据传输完成事件接口*/
typedef struct _EVENT
{
    CMD_OP bOpcode;
    CMD_STATUS eStatus;
    U8 bStatus;//记录出错时的status
    U8 bError;//记录出错时的error

    U8 bI;//中断标记，在同步事件中和bStatus，bError一起作为回复D2H的参数
    BOOL bNQFlg;
    U8 Tag;//记录出错时的TAG号
    U8 bRFU;

    U32 dParam[5];//同步事件具体参数 --对应不同的事件有不同的定义
    TaskCompleteCb_t *pfEventCompleteCb;//事件完成回调函数

}T_EVENT,*pT_EVENT;

typedef struct _DI_TASK
{
    CMD_STATUS eStatus;
    CMD_OP bOpcode;
    U8 bDir;
    U8 bRFU;
    U32 dLba;
    U32 dSctSize;
    U32 dBuffAddr;
}T_DI_TASK,*pT_DI_TASK;

//命令相关上下文
typedef struct _CMD_CTX
{
    T_H2D h2d;//原始的命令FIS备份
    U32 dTimeStamp;//上电以后毫秒计时 --会溢出?
}T_CMD_CTX;

typedef struct _TASK_QUEUE_CTX
{

    T_HI_TASK tHiTasks[MAX_CMD_NUM + PRE4K_CMD_NUM];/*HOST 发起的 任务，由rSF0_INT1_STS0标识
                                        若rSF0_INT1_STS0为非0 表示队列中有任务
                                     */
    volatile T_DI_TASK tDiTask;/*DEVICE发起的任务由rSF0_INT1_STS1标识
                         若rSF0_INT1_STS1为非0 表示队列中有任务

                      */
    volatile E_QUEUE_TYPE enTaskType;
    //SATA 当前运行环境的基本信息
    U8_V bIsNcqCmd;/*此标记只对HI的任务有效
                        1.若该位没有置位，产生的任务只有一个且只在Slot0
                        2.若该位有置位，则表示当前处于NCQ 任务模式，存在多个任务的可能
                   */
    U8_V  bPRE_Cmd_Flg;
    //异步启动reorder 机制
    U8_V bReOdWrIdx;//指向reorder queue下一个地址的索引
    U8_V bReOdRdIdx;//指向已经处理的reorder queue地址的索引
    U32_V dReOdCnt;//记录pending命令总数
    U32_V dStartedCnt;//记录已经启动的命令总数
    U8 bReOdCmds[32];//reorder queue

#ifdef __NCQ_QUEUE_NEW__
    //新的NCQ命令队列机制
    U32_V dCnt;//记录pending命令总数
    U32_V dPopedNum;//本次已经出栈的任务数    U8 bCmds[32];//cmd queue tag
    U32_V dPendingWrSec;
#ifdef  SC_H2DFIFO_EN
    U8 bCmds[33];//cmd queue tag
    U8 bRecordCmdPop;
#else
    U8 bCmds[32];//cmd queue tag
#endif
    U8_V bWrIdx;//下一个待写任务的索引位置
    U8_V bRdIdx;//下一个待读任务的索引位置
#endif

    U32 dPRECMD_MoveCnt;
    U32 dPRECMD_ReoderCnt;
    U32 dPRECMD_PushCnt;
    U8  bHidden_Cmd_Flg;
    U8  bHidden_Cmd_WIdx;
    U8_V  bF2S_Task_abortFlg;       //FTL 通知SATA Task_abort
    volatile E_TYPE enEventType;//FTL 端通知SATA端的事件
    U32_V dEventParam[4];//reserved for future use
}T_TASK_QUEUE_CTX;

/***********************SATA模块注册结构接口定义***********************/
typedef struct _SATA_MODULE_CTX
{
    U32_V dIsValid;//初始化完成以后被赋值成0x12345678
    U32	dDmaBuffAddr; //数据传输是用到的DMA buff的地址
    U32 dDmaBuffSize; //数据传输DMA size
    U32 dVirtualLbaStartAddr;//虚拟空间起始地址
    T_MP_CFG tMpCfg;//量产工具配置接口
    CALL_BACK void (*volatile pSysTick_Event_cb) ();//SysTick中断服务函数
    CALL_BACK void (*volatile pPHYLinkChangeEvent_cb) (ENUM_LPS enState);//PHY link 层状态变化事件的回调函数
    CALL_BACK void (*volatile pInterfaceFatalErrorEvent_cb) ();//SATA 端数据传输出错事件的回调函数--填充DMA1
    CALL_BACK void (*volatile pTaskQueuedEvent_cb) (T_TASK_QUEUE_CTX *ptQueue);//SATA 端数据传输任务入队列事件的回调函数
    CALL_BACK BOOL (*volatile pDeepSleepEntry_Event_cb) ();
    CALL_BACK void (*volatile pDeepSleepExit_Event_cb) ();
    CALL_BACK void (*volatile pRemove_AllTask_cb) (U8 bTag,U32 dFisNum);
    CALL_BACK void (*volatile pSystemExceptionEvent_cb) ();
    CALL_BACK void (*volatile pReadRetry_Process) ();
    CALL_BACK void (*volatile pFtlTask_Process) ();
    U32 dRFU2[1];//预留扩展接口
}T_SATA_MOD_ITF,pT_SATA_MOD_ITF;//TOTAL 16*4 byte

//SATA 设备上下文定义
typedef struct _SATA_DEVICE_CTX
{
    //NOTE:enum 默认占4个byte
    //SATA设备状态相关
    UN_DEV_TRANS_STATES unDevTransSts;//设备传输层状态机状态,

    BOOL bIsAP2SEn;//记录HOST 是否支持slumber 模式
    U8_V wNcqTOCnt;//NCQ命令流量控制，表示SATA端可以一把启动的最大命令数,必须大于0
    U8_V bNcqBrustTH;//NCQ命令流量控制，表示SATA端需要累积到多少个CMD才可以brust启动命令
    U8 bNcqMaxBrustCnt;///NCQ命令流量控制，表示SATA端一次可以brust的最大CMD数

    ENUM_DPS eDPMSts;//记录当前Device的应用层电源管理状态
    ENUM_LPS eLPMSts;//记录当前Device的链路层电源管理状态
    
    //ENUM_PHYS ePHYSts;//记录当前Device的phy状态 --未使用
    BOOL bSaveUartBuff;
    U8 bReorderCnt;
    U8 bPHYRESET;
    U8 bFirstH2dCmd;

    U8_V bEventLock;//双核交互锁
#if (SATA_LOG_ENABLE == 1)
    ADDR_MODE enAddrMode;
#else
    U8 Reservd;
#endif
    BOOL bIsIdyOverlay;

    U32 dLifePowerOnTime;//设备生命周期内总的通电时长,可能放到SMART表中管理
    
    U32 dPowerCycleCnt;//设备总的通电次数,可能放到SMART表中管理
    
    U32_V dPowerOnTime;//本次设备上电运行毫秒时长，1H = 0x36EE80 S，32位可以记录1193.04小时
    
    U32 dSystemFrequency;     /*!< System Clock Frequency (Core Clock) */
    U32 dMilliSecondCnt;      /*!< 一毫秒定时计数  */
    U32 dMicroSecondCnt;      /*!< 微秒定时计数  */
    //TODO:要保存起来，时间要连续？
    U32 dMilliSecAcc;		//一小时的毫秒计数器，用于累加成1小时，每到1小时清零，没一时间累加值就会写入FLASH
    
    U32 dDevCap;//当前设备可用扇区数，该值需要固定, -容量32位可以记录2048G=2T 
    U32 dMaxLbaAddr4User;//允许用户访问的最大LBA地址 -可以通过命令更改
    
    U8 bGenxSpeed;//设备当前速率
                 //0x0: Host not present or communication not established or lost
                 //0x1: SATA 1.5 Gb/s communication rate negotiated
                 //0x2: SATA 3.0 Gb/s communication rate negotiated
                 //0x3: SATA 6.0 Gb/s communication rate negotiated

    BOOL bIsDiagnosticOk;//设备诊断状态
    BOOL bIsBGActivityOn;//后台任务进行状态
    U8  bDrq;//单次PIO 传输最大扇区数
    
    U8 bMultiDrq;//read/write MULTIPLE MODE 的 DRQ大小 只对这些命令有效
    U8 bATACmd;
    BOOL bIsHIPMEn;//HIPM funtoin enabled (L_PWN_MNG_EN == 1) 四字节对齐
    BOOL bIsDIPMEn;//DIPM funtoin enabled (L_PWN_MNG_EN == 1)
    
    //PIO传输层状态机参数
    U32 dPioSctCnt;//pio传输的数据总量-用于分割PIO 任务

    //Security 相关
    BOOL bIsSecFreezeLocked;
    ENUM_ATA_SECURITY_STATE eSecState;//security 相关命令变更该状态，所有实现的命令都要参考这个状态,状态掉电要保存起来
    U8 bSecPwdAttemptsCnt;//错误密码尝试次数，上电初始化为5
    BOOL bIsSecPwdAttemptsExceeded;//密码尝试次数是否溢出

    BOOL bIsWrCacheEnabled;
    BOOL bIsLookAheadEnabled;
    //DCO 相关
    BOOL bIsDcoFreezeLocked;
    BOOL bIsRevertingEnabled;
    /* A device in the reverting to defaults enabled mode may reset parameters
        to their default power-on values during the processing of a software reset.
    */
    //FIS命令管理相关
    BOOL bIsNcqCmd;//表明当前进行的是否是NCQ命令
    volatile ENUM_SATA_TRSP_TYPE eTransType;//当前设备激活的传输类型
    //简易任务队列管理
    U8_V bCmdCnt;//队列中命令个数
    //出错处理之后进行LOG保存后归零
    BOOL IsOvLap;//Over Lap正确完成的命令个数是不是已经大于32个,

    U8 bRdIdx;//取命令索引 -如果FTL是随机执行任务的话该索引没有意义。--TODO:NCQ命令随机执行的时候出现错误如何记录LOG？
    U8 bWrIdx;//写命令索引
    U16 wNCQCmdIdx;

    //每次激活不同的状态都要重新注册处理接口
    SATA_XXX_StateMachine *DPC_Process;//DPC接口
    SATA_XXX_IntEventHandler *INT_Process;//中断服务
    SATA_XXX_DataTransferReq *DOS_TransferReq;//ATA层、FTL层数据Data Or 状态Status 传输请求统一接口
    TransferCompleteCb_t *pfTransferCompleteCb;    //SATA传输层事务完成事件回调接口
    T_SATA_MOD_ITF tSataMI;

    T_CMD_CTX tCmdList[MAX_CMD_NUM];//暂存合法的HOST TO DEVICE FIS的缓冲区

#if (SATA_LOG_ENABLE == 1)
    ADDR_MODE arrenAddrMode[MAX_CMD_NUM];//SATA LOG 需要区分28bit/48bit --新增的成员,切换代码出错以后会使LOG保存错误的内容
    U8  bCmdIdx[MAX_CMD_NUM];//每个TASK 对应的H2D存放位置
#else
    U32 dRFU1[16];
#endif
    BOOL bIsHWD2HEnabled;
    T_EVENT tSyncEvent;//64 bytes 同步事件参数
    T_EVENT tAsyncEvent;//64 bytes 异步事件参数，异步写缓冲区都是独立的

#ifndef __NCQ_QUEUE_NEW__
    U64 qwTaskCnt;

#else
    U64 qReservd2;
#endif

    TransferCompleteCb_t *pfTaskCompleteCb;//FTL 数据读写任务完成回调

    //POWER MANAGER 相关
    U32 dStanbyTimerNum; //进入STANBY的时间换成毫秒的计数
    U32 dStanbyTimerCnt;//进入STANBY的时间计数
    BOOL bIsStandyTimerOn;//进入STANBY定时器开关
    U32  dSataLogUpdateCount;//更新SATA LOG的时间毫秒计数
    U32 dSecNumPerPage;
    BOOL bIsSecErasePrepared;
    T_SD tSDState;
    U8 bSDFMPVal;//Failure Mode Policy value
    U8 bSDOCWOEVal;//Sanitize Operation Completed Without Error value
    U16 wSanitizeProCnt;
    ENUM_DCO_STATE eDCOState;//DCO configuration state
    U8  bPM;	//    U8 bRFU1;
    U32 __sda_bas;//默认的SDB bas 地址
    U32 dIsNDCTOGuardEn;//启用非数据写读命令超时标记NoneDataCmd
    U32 dNDCTOGuardTimer;//reserved for future use
    U32 dVDTFlag;//0xDEADDEAD
    U8_V bCmdPushIdx;// 命令压进FIFO 的,下一个准备接收命令的索引位置
    U8  bPre4kCmdFlg;
    U8  bReserved3[2];//reserved for future use
    U32 dRFU2[7];//reserved for future use
}T_SATA_DEV_CTX,*pT_SATA_DEV_CTX;

#ifdef    CHECK_CMD_SEQ_EXE
typedef struct
{
    U8  bPopCmds[33];//POP 出来的tag 号
    U8  CheckCmdNum;
    U8  bCheckCmdIdx;
    U8  bPopCmdIdx;
}T_SATA_Debug;

T_SATA_Debug g_tDebug;
#endif

extern T_BUFFER tSyncBuffer;
extern T_BUFFER tAsyncBuffer;
extern T_SATA_DEV_CTX tSataDC;
extern T_TASK_QUEUE_CTX  tTQC;
//Device command protocol 状态机实现接口表
extern SATA_XXX_StateMachine* const tDevCmdSMTbl_C[];

//Device command protocol Event Handle中断函数处理接口表
extern SATA_XXX_IntEventHandler* const tDevCmdEHTbl_C[];

//传输层数据收发接口函数表
extern SATA_XXX_DataTransferReq* const tDevCmdDOSTbl_C[];

// *****************************************************************************
// 函数名称: DSATA_UpdateSystemClockInfo
// 函数功能: 获取平台系统时钟信息初始化SATA时间相关变量
// 入口参数: sys_clk 系统时钟
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_UpdateSystemClockInfo (U32 sys_clk);

// *****************************************************************************
// 函数名称: DSATA_RegisteInterfaceInfo(T_SATA_MOD_ITF *ptSataMI)
// 函数功能: 获取操作锁
// 入口参数: ptSataMI :SATA 模块上下文
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_RegisteInterfaceInfo(T_SATA_MOD_ITF *ptSataMI);

// *****************************************************************************
// 函数名称: DSATA_ModuleInit
// 函数功能: SATA模块初始化函数
// 入口参数: NONE
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_ModuleInit();

// *****************************************************************************
// 函数名称: DSATA_ModuleProcess
// 函数功能: SATA 模块主循环
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_ModuleProcess ();

/*手动启动PDTR*/
void DSATA_StartPDTR(void);

// *****************************************************************************
// 函数名称: DSATA_SysTickProcess
// 函数功能: SATA 模块systick 任务
// 入口参数:
// 出口参数:
// 说    明:
// *****************************************************************************
void DSATA_SysTickProcess (void);

void DSATA_CmdReorderRestart(T_PISR* pPISR,U8 mode);

#ifdef __NCQ_QUEUE_NEW__
/*
 * NCQ 队列初始化
 * */
void NCQ_QueueInit(void);

/*
 * NCQ 查询下一条命令响应延迟时间
 * 返回值：延迟时间，单位ms
 * */
U16 NCQ_InquireTimeOfNextCmd(void);

/*
 * NCQ 队列深度查询
 * 返回值：当前队列任务数
 * */
U8   NCQ_GetQueueLen(void);
U32  NCQ_GetQueueWrSec(void);

/*
 * NCQ reorder 队列深度查询
 * 返回值：当前队列任务数
 * */
U8 NCQ_GetReorderQueueLen(void);
/*
 * NCQ新任务入队列
 * 参数bTag ：命令TAG号
 * */
void NCQ_QueuePush(U8 bTAG);

/*
 * 查询NCQ任务队列，队首命令的方向
 * 返回值：队首指针
 * */
T_HI_TASK* 	 NCQ_QueueInquire(void);

/*
 * 查询NCQ任务队列，队尾命令参数
 * 返回：队尾指针
 * */
T_HI_TASK* NCQ_QueueInquireTail(void);

/*
 * 查询NCQ任务队列，队尾命令参数
 * 返回：队尾指针
 * */
T_HI_TASK* NCQ_QueueInquireTail(void);

/*
 * 根据TAG号，获取任务参数指针
 * 返回：队首指针
 * */
T_HI_TASK* NCQ_GetParam(U8 bTag);

/*
 * NCQ命令队列，指定方向的多命令出栈
 * 参数：
 * T_QTASK* 新命令参数出口
 * bDir：指定任务方向出栈
 * 	 	0xFF:IN/OUT BOTH
 * 		0：DIR_OUT
 * 		1：DIR_IN
 * tTags:输出tag数组
 * 返回值：
 * 		返回的命令数
 * */
U8 NCQ_QueueMulPop(U8 bDir,U8* bTags,U32 *dTotalSize,U8 *bCmdType,U8 bNum);
/*
 * NCQ命令队列，命令出栈
 * 参数：
 * T_QTASK* 新命令参数出口
 * bDir：指定任务方向出栈
 * 	 0xFF:IN/OUT BOTH
 * 		0：DIR_OUT
 * 		1：DIR_IN
 * */
T_HI_TASK* NCQ_QueuePop(U8 bDir);

/*
 *函数功能：FTL 请求SATA 收发数据到指定的BUFF中
 *参数bDir：读写方向DIR_IN,DIR_OUT
    参数wSctCnt：数据扇区数
    参数dBuffAddr：数据buffer地址
 *
 * */
void Ftl2SataTask_DataTransReq(U8 bDir,U16 wSctCnt,U32 dBuffAddr);
/*
 *函数功能：FTL通知SATA DMA1超时事件
 *
 * */
void Ftl2SataTask_Dma1TimeOut(void);
/*
 *函数功能：FTL通知SATA准备进行后台任务，禁止休眠唤醒功能
 *
 * */
void Ftl2SataTask_PmgOff(void);
/*
 *函数功能：FTL通知SATA没有后台任务，可以使能休眠唤醒功能
 *
 * */
void Ftl2SataTask_PmgOn(void);

void NCQ_CmdReorderQueue(U8* pTags,U8 bCnt);

// *****************************************************************************
// 函数名称:   SATA_DataFirstPassPointEn(U8 bFirstPassEn)
// 函数功能:  开启 数据过CRC 第一级放行点（FIFO后） 或恢复原来第二级放行点（Exbuff）
// 入口参数:  bFirstPassEn  0: 开启第一级放行点并且关第二级，  1：关第一级放行点并且开启第二级放行点
// 出口参数:
// 说    明:
// *****************************************************************************
void SATA_DataFirstPassPointEn(U8 bFirstPassEn);
void FTL2Sata_TaskAbort(void);
#endif

void Save_Uart_Buff_To_Log(void);
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif/*_SATA_TYPEDEF_H_*/

