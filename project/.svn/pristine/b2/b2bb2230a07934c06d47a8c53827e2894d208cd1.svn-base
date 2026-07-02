/*********************************************************

  模块名称: SC

  版本号:   0.2

*********************************************************/

#ifndef __SC_REG_DEF__
#define __SC_REG_DEF__



#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus

#ifndef __I
#define   __I     volatile             /*!< Defines 'read only' permissions                 */
#endif

#else

#ifndef __I
#define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif

#endif

#ifndef __O
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#endif

#ifndef __IO
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif



#define SG_SC_BASE     (0x5E1000)



typedef struct _SG_SC_TypeDef
{
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*HwInit  1: ATAPI  0: ATA  Firmware must write 0 this bit immediately after power-on prior to initiating any data transfer*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rCFGR;/*<Offset:0x00> This register is used to configure the DWC_dsata core*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __O unsigned long R:1 ;/*N/A*/
            __IO unsigned long IE:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rGCR;/*<Offset:0x04> N/A*/

    __IO unsigned char Reserved[152];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long PD:8 ;/*N/A*/
            __I unsigned long NCP:8 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rBISTAFR;/*<Offset:0xA0> contains the pattern definition*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long PATTERN:4 ;/*N/A*/
            __IO unsigned long PV:1 ;/*N/A*/
            __IO unsigned long FLIP:1 ;/*N/A*/
            __IO unsigned long ERREN:1 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long LLC:3 ;/*N/A*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __IO unsigned long SDFE:1 ;/*N/A*/
            __IO unsigned long ERRLOSSEN:1 ;/*N/A*/
            __IO unsigned long QPHYINIT:1 ;/*N/A*/
            __IO unsigned long LLB:1 ;/*N/A*/
            __I unsigned long NEALB:1 ;/*N/A*/
            __O unsigned long CNTCLR:1 ;/*N/A*/
            __O unsigned long TXO:1 ;/*N/A*/
            __O unsigned long STOPB:1 ;/*N/A*/
            __O unsigned long FERLB:1 ;/*N/A*/
            __IO unsigned long Reserved2:3 ;/*保留*/
            __IO unsigned long LATE_PHY_READY:1 ;/*N/A*/
            __IO unsigned long Reserved3:7 ;/*保留*/
        }bits;
    }rBISTCR;/*<Offset:0xA4> N/A*/

    __IO unsigned long rBISTFCTR;/*<Offset:0xA8>  Received BIST FIS count*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long FRAMERR:16 ;/*N/A*/
            __I unsigned long BRSTERR:8 ;/*N/A*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rBISTSR;/*<Offset:0xAC>  contains errors detected in the received BIST Data FIS in the loopback initiator far-end retimed*/

    __I unsigned long rBISTDECR;/*<Offset:0xB0>  This register contains the number of U32 errors detected  in the received BIST frame in the loopback*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long PORTSM_CS:7 ;/*N/A*/
            __I unsigned long RXFISM:6 ;/*N/A*/
            __I unsigned long TSM_CS:6 ;/*N/A*/
            __I unsigned long TCHK_CS:4 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __I unsigned long LINK_CS:5 ;/*N/A*/
            __IO unsigned long TLERR_BLPS:1 ;/*N/A*/
            __IO unsigned long TLERR_ALPS:1 ;/*N/A*/
            __I unsigned long STEL:1 ;/*N/A*/
        }bits;
    }rDIAGNR;/*<Offset:0xB4> for hardware debug*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long PORTSM_LBFS:8 ;/*出现fatal error 前，portsm最后一个状态*/
            __IO unsigned long RXFISM_LBFS:8 ;/*出现fatal error前，rxfism最后一个状态*/
            __IO unsigned long ALIGN_ADJ_NUM:4 ;/*number of align to sub/add from/to 2048*/
            __IO unsigned long ALIGN_ADJ_DIR:1 ;/*1: decrement; 0: increment*/
            __IO unsigned long Reserved:7 ;/*保留*/
            __IO unsigned long ERR_BADEND:1 ;/*link band end error, WTRM instead of EOF*/
            __IO unsigned long ERR_TBEB:1 ;/*link data 10b8b decoding error*/
            __IO unsigned long ERR_DISP:1 ;/*link data disparity error*/
            __IO unsigned long ERR_PRIM:1 ;/*link primitive error*/
        }bits;
    }rDIAGNR1;/*<Offset:0xB8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long MAX_COMINIT:8 ;/*N/A*/
            __IO unsigned long MIN_COMINIT:8 ;/*N/A*/
            __IO unsigned long MAX_COMWAKE:8 ;/*N/A*/
            __IO unsigned long MIN_COMWAKE:7 ;/*minimun space of comwake detect*/
            __IO unsigned long OOBR_WE:1 ;/*write_enable bit :1--bits[30:0] can be written*/
        }bits;
    }rOOBR;/*<Offset:0xBC> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long TX_PUSH_WCNT:12 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __I unsigned long TX_PUSH_AF:1 ;/*N/A*/
            __I unsigned long TX_PUSH_EMPTY:1 ;/*N/A*/
            __I unsigned long TX_PUSH_FULL:1 ;/*N/A*/
            __I unsigned long RX_POP_WCNT:12 ;/*N/A*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __I unsigned long RX_POP_AE:1 ;/*N/A*/
            __I unsigned long RX_POP_EMPTY:1 ;/*N/A*/
            __I unsigned long RX_POP_FUL:1 ;/*N/A*/
        }bits;
    }rDIAGNR2;/*<Offset:0xC0> N/A*/

    __IO unsigned char Reserved1[36];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long M_HDATA:3 ;/*N/A*/
            __I unsigned long S_HDATA:3 ;/*N/A*/
            __I unsigned long M_HADDR:1 ;/*N/A*/
            __I unsigned long S_HADDR:1 ;/*N/A*/
            __I unsigned long AHB_ENDIAN:2 ;/*N/A*/
            __I unsigned long RETURN_ERR:1 ;/*N/A*/
            __I unsigned long PHY_TYPE:3 ;/*N/A*/
            __I unsigned long LATCH_M:1 ;/*N/A*/
            __I unsigned long PHY_STAT:6 ;/*N/A*/
            __I unsigned long PHY_CTRL:6 ;/*N/A*/
            __I unsigned long PHY_RST:1 ;/*N/A*/
            __I unsigned long PHY_DATA:2 ;/*N/A*/
            __I unsigned long RX_BUFFER:1 ;/*N/A*/
            __I unsigned long ALIGN_M:1 ;/*N/A*/
        }bits;
    }rGPARAM1R;/*<Offset:0xE8> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long RXOOB_CLK:9 ;/*N/A*/
            __I unsigned long TX_OOB_M:1 ;/*N/A*/
            __I unsigned long RX_OOB_M:1 ;/*N/A*/
            __I unsigned long RXOOB_CLK_M:1 ;/*N/A*/
            __I unsigned long ENCODE_M:1 ;/*N/A*/
            __I unsigned long BIST_M:1 ;/*N/A*/
            __IO unsigned long Reserved:18 ;/*保留*/
        }bits;
    }rGPARAM2R;/*<Offset:0xEC> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long RXFIFO_DEPTH:3 ;/*N/A*/
            __I unsigned long TXFIFO_DEPTH:3 ;/*N/A*/
            __I unsigned long RX_MEM_S:1 ;/*N/A*/
            __I unsigned long RX_MEM_M:1 ;/*N/A*/
            __I unsigned long TX_MEM_S:1 ;/*N/A*/
            __I unsigned long TX_MEM_M:1 ;/*N/A*/
            __IO unsigned long Reserved:22 ;/*保留*/
        }bits;
    }rPPARAMR;/*<Offset:0xF0> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rTESTR;/*<Offset:0xF4> put the DWC_dsata slave  interface into a test mode*/

    __I unsigned long rVERSIONR;/*<Offset:0xF8>  Default value:DSATA_VERSION_NUM*/

    __I unsigned long rIDR;/*<Offset:0xFC>  Default value:DSATA_ID_NUM or core_id*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long Reserved:10 ;/*保留*/
            __IO unsigned long dVAL:22 ;/*Command List Base Address*/
        }bits;
    }rPCLBR;/*<Offset:0x100>  This register is used to put the DWC_dsata slave interface into a test mode*/

    __IO unsigned char Reserved2[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long Reserved:5 ;/*保留*/
            __IO unsigned long dVAL:27 ;/* FIS Base Address*/
        }bits;
    }rPFBR;/*<Offset:0x108> This register is used to put the DWC_dsata slave   interface into a test mode*/

    __IO unsigned char Reserved3[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long HDRS:1 ;/*RW1C, Host to Device Register FIS Interrupt*/
            __IO unsigned long FTXDS:1 ;/*RW1C, FIS Transmit Done Interrupt*/
            __IO unsigned long UFS:1 ;/*RW1C, Unknown FIS Interrupt*/
            __IO unsigned long DPS:1 ;/*RW1C,  Descriptor Processed*/
            __IO unsigned long DMATS:1 ;/*RW1C, DMAT Status*/
            __IO unsigned long TXOK:1 ;/*RW1C, Transmit Okay*/
            __IO unsigned long PMNAK:1 ;/*接收HOST下发的PMNAK指示中断*/
            __IO unsigned long H2DINBUF:1 ;/*H2D BUFFER里面缓存有新命令*/
            __IO unsigned long PCS:1 ;/*RW1C, Port COMRESET Status*/
            __IO unsigned long LSCS:1 ;/*RW1C, Link State Change Status*/
            __IO unsigned long PMREQ_P:1 ;/*收到host请求partial中断*/
            __IO unsigned long PMREQ_S:1 ;/*收到HOST请求slumber中断*/
            __IO unsigned long Reserved:12 ;/*保留*/
            __IO unsigned long OFS:1 ;/*RW1C, Overflow Status*/
            __IO unsigned long INFS:1 ;/*RW1C, Interface Non-fatal Error Status*/
            __IO unsigned long IFS:1 ;/*RW1C , Interface Fatal Error Status*/
            __IO unsigned long PSDEF:1 ;/*RW1C ,Phy Signal Detect Error Flag*/
            __IO unsigned long MBES:1 ;/*RW1C , Master Bus Error Status*/
            __IO unsigned long CMDONS:1 ;/*RW1C , Command Done status ,NCQ命令重新排序全部执行完后产生中断状态位*/
            __IO unsigned long COMWKS:1 ;/*comwake 中断*/
            __IO unsigned long REORDST:1 ;/*启动reorder中断*/
        }bits;
    }rPISR;/*<Offset:0x110> This register is used to put the DWC_dsata slave interface into a test mode*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long HDRE:1 ;/* Host to Device Register FIS Interrupt Enable*/
            __IO unsigned long FTXDE:1 ;/*FIS Transmit Done Interrupt Enable*/
            __IO unsigned long UFE:1 ;/*Unknown FIS Interrupt Enable*/
            __IO unsigned long DPE:1 ;/*Descriptor Processed Interrupt Enable*/
            __IO unsigned long DMATE:1 ;/*DMAT Enable*/
            __IO unsigned long TXOKE:1 ;/*Transmit Okay Enable*/
            __IO unsigned long PMNAKE:1 ;/*接收HOST下发的PMNAK中断使能*/
            __IO unsigned long H2DINBUF_EN:1 ;/**/
            __IO unsigned long PCE:1 ;/*Port COMRESET Interrupt Enable*/
            __IO unsigned long LSCE:1 ;/* Link State Change Enable*/
            __IO unsigned long PMREQ_P:1 ;/*host发起partial休眠请求中断使能*/
            __IO unsigned long PMREQ_S:1 ;/*host发起slumber休眠请求中断使能*/
            __IO unsigned long Reserved:12 ;/*保留*/
            __IO unsigned long OFE:1 ;/*Overflow Enable*/
            __IO unsigned long INFE:1 ;/* Interface Non-fatal Error Enable*/
            __IO unsigned long IFE:1 ;/*Interface Fatal Error Enable */
            __IO unsigned long PSDEE:1 ;
                /*Phy Signal Detect Error Enable
                When set, GCR.IE=1, and PISR.MBES=1, the intrq output is asserted*/

            __IO unsigned long MBEE:1 ;
                /*Master Bus Error Enable
                When set, GCR.IE=1, and PISR.MBES=1, the intrq output is asserted*/

            __IO unsigned long CMDONE:1 ;/*Command done interrupt enable(新增)NCQ命令重新排序全部执行完后产生中断使能*/
            __IO unsigned long COMWKE:1 ;/*comwake interrupt enable*/
            __IO unsigned long REORDSTE:1 ;/*N/A*/
        }bits;
    }rPIER;/*<Offset:0x114> This register enables or disables/masks the reporting of the corresponding interrupt to firmware*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long ST:1 ;/*Start*/
            __IO unsigned long STD:1 ;/*RW1C, Start Disable*/
            __IO unsigned long Reserved:2 ;/*保留*/
            __IO unsigned long FRXP:1 ;/*RW1C, FIS Receive Paused*/
            __IO unsigned long Reserved1:3 ;/*保留*/
            __I unsigned long CCS:5 ;/*Current Command Slot*/
            __IO unsigned long DPP:1 ;/*RW1C, Descriptor Processing Paused*/
            __IO unsigned long DTABORT:1 ;/*RW1S,Data Transfer Abort*/
            __IO unsigned long TXSAS:1 ;/*Transmit Status Auto-Send*/
            __IO unsigned long TXREG:1 ;/*RW1S,Transmit Register FIS*/
            __IO unsigned long TXSDB:1 ;/*RW1S, Transmit Set Device Bits FIS*/
            __IO unsigned long TXBISTA:1 ;/*RW1S Transmit BIST Activate FIS*/
            __IO unsigned long Reserved2:4 ;/*保留*/
            __IO unsigned long APSTE:1 ;/*Automatic Partial to Slumber Transitions Enabled*/
            __IO unsigned long Reserved3:4 ;/*保留*/
            __IO unsigned long ICC:3 ;/*Interface Communication Control */
            __IO unsigned long Reserved4:1 ;/*保留*/
        }bits;
    }rPCMDR;/*<Offset:0x118> enables or disables/masks the reporting of the corresponding interrupt to firmware*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:3 ;/*N/A*/
            __IO unsigned long Reserved:29 ;/*保留*/
        }bits;
    }rASICMONITOR_SEL;/*<Offset:0x11C> N/A*/

    __IO unsigned char Reserved4[8];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long DET:3 ;/*Host Detection*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __I unsigned long SPD:2 ;/*Current Interface Speed*/
            __IO unsigned long Reserved1:2 ;/*保留*/
            __I unsigned long IPM:3 ;/*Interface Power Management */
            __IO unsigned long Reserved2:21 ;/*保留*/
        }bits;
    }rPSSTSR;/*<Offset:0x128> This 32-bit register conveys the current state of the interface and host*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long DET:3 ;/*Host Detection Initialization*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long SCTL_SPD:2 ;/*选择sata 速度*/
            __IO unsigned long Reserved1:2 ;/*保留*/
            __IO unsigned long IPM:2 ;
                /*Interface Power Management Transitions Allowed
                Note: PCMDR.ICC 请求不受该位影响*/

            __IO unsigned long Reserved2:2 ;/*保留*/
            __IO unsigned long ITIMER_EN:1 ;/*This field enables inactivity timer operation when set*/
            __IO unsigned long ITIMER_PM:1 ;
                /*0: Partial
                1: Slumber*/

            __IO unsigned long Reserved3:18 ;/*保留*/
        }bits;
    }rPSCTLR;/*<Offset:0x12C> Firmware uses this 32-bit read-write register to control SATA interface capabilities*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long ERR_I:1 ;/*Recovered Data Integrity*/
            __IO unsigned long ERR_M:1 ;/*Recovered Communication Error*/
            __IO unsigned long Reserved:6 ;/*保留*/
            __IO unsigned long ERR_T:1 ;/*Non-recovered Transient Data Integrity Error*/
            __IO unsigned long ERR_C:1 ;/*PHY Not Ready Error*/
            __IO unsigned long ERR_P:1 ;/*Protocol Error*/
            __IO unsigned long ERR_E:1 ;/*Internal Bus Error*/
            __IO unsigned long Reserved1:5 ;/*保留*/
            __IO unsigned long DIAG_I:1 ;/*PHY Internal Error*/
            __IO unsigned long DIAG_W:1 ;/*Comm Wake*/
            __IO unsigned long DIAG_B:1 ;/*10B to 8B Decode Error*/
            __IO unsigned long DIAG_D:1 ;/*Disparity Error*/
            __IO unsigned long DIAG_C:1 ;/*CRC Error*/
            __IO unsigned long DIAG_H:1 ;/*Handshake Error*/
            __IO unsigned long DIAG_S:1 ;/*Link Sequence Error*/
            __IO unsigned long DIAG_T:1 ;/*Transport State Transition Error*/
            __IO unsigned long Reserved2:6 ;/*保留*/
            __IO unsigned long RXFOV:1 ;/*RXFIFO Overflow Error*/
        }bits;
    }rPSERR;/*<Offset:0x130> This field is bit significant. Each bit corresponds to a command slot in the Command List structure*/

    __IO unsigned char Reserved5[4];/*保留*/

    __IO unsigned long rPDTR;/*<Offset:0x138>  Firmware uses this 32-bit read-write register to control SATA interface capabilities*/

    __IO unsigned char Reserved6[4];/*保留*/

    __IO unsigned long rPITIMER;/*<Offset:0x140>  N/A*/

    __IO unsigned char Reserved7[44];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long TXTS:4 ;/*Transmit Transaction Size*/
            __IO unsigned long RXTS:4 ;/*Receive Transaction Size*/
            __IO unsigned long TXABL:4 ;/*This field is read-write when PCMDR.ST=0 and read-only when PCMDR.ST=1*/
            __IO unsigned long RXABL:4 ;
                /*Receive AHB Burst Limit
                This field is read-write when PCMDR.ST=0 and read-only when PCMDR.ST=1*/

            __IO unsigned long Reserved:15 ;/*保留*/
            __IO unsigned long ANI:1 ;
                /*Address Non Increment, 0 - Address is incremented
                1 - Address is not incremented
                This field is read-write when PCMDR.ST=0 and read-only when PCMDR.ST=1*/

        }bits;
    }rPDMACR;/*<Offset:0x170> N/A*/

    __IO unsigned char Reserved8[4];/*保留*/

    __IO unsigned long rPHYCR;/*<Offset:0x178>  N/A*/

    __I unsigned long rPHYSR;/*<Offset:0x17C>  N/A*/

    __IO unsigned long rPPICR;/*<Offset:0x180>  N/A*/

    __I unsigned long rPPBCR;/*<Offset:0x184>  N/A*/

    __IO unsigned char Reserved9[32];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rNCQ_CMD_ST;/*<Offset:0x1A8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rFILL_INVLDA_ST;/*<Offset:0x1AC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long HWCLR_EN:1 ;/**/
            __IO unsigned long D2HCLR_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rHWCLR_EN;/*<Offset:0x1B0> 硬件清PISR[0]使能*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rCRCNT;/*<Offset:0x1B4> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rTXERRC;/*<Offset:0x1B8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long SRSTCMD:1 ;/*NCQ命令是soft reset*/
            __IO unsigned long USEDTAG:1 ;/*新NCQ命令出现TAG重复*/
            __IO unsigned long CROSBORD:1 ;/*新NCQ命令出现地址越界*/
            __IO unsigned long Reserved:29 ;/*保留*/
        }bits;
    }rH2DERR;/*<Offset:0x1BC> 当开启硬件自动回复D2H FIS时,收到新的NCQ命令,如果该命令异常，FW可通过该寄存器判断是soft reset或者出现地址越界或者TAG重复*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*复位sata controller和dmc、ram接口模块*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rDMCRST;/*<Offset:0x1C0> 复位sc和dmc/ram接口模块*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rERRMSK_CLR;/*<Offset:0x1C4> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*剔除RX DATA中K28.5不在byte0的数据使能*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rDEL_NONALIGN_EN;/*<Offset:0x1C8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*速度协商时，device延迟发送sync计数器*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rDLY_TXSYNC_CNT;/*<Offset:0x1CC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*采用comreset/comwake复位8/10编码中running disparity的清零使能*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rRST_RD_EN;/*<Offset:0x1D0> 采用comreset/comwake复位8/10编码中running disparity的清零使能*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long DEL_ALDROP_EN:1 ;/*去除align drop这一级延迟*/
            __IO unsigned long DEL_DESCR_EN:1 ;/*去除descramble这一级延迟*/
            __IO unsigned long DEL_SCRAM_EN:1 ;/*去除scramble这一级延迟*/
            __IO unsigned long DEL_LINKTX_EN:1 ;/*去除link_tx这一级延迟*/
            __IO unsigned long DEL_DEFRM_EN:1 ;/*除去deframe这一级r_rdy_det的延时。*/
            __IO unsigned long Reserved:27 ;/*保留*/
        }bits;
    }rSHORTPATH_EN;/*<Offset:0x1D4> 减小rountrip latency使能*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __O unsigned long CLR_INRECVTAG:1 ;/*打开自动回复D2H 功能时,当出现命令异常时清除SC内部指示命令tag寄存器*/
            __O unsigned long CLR_NCQCNT:1 ;/*FW清除SC内部硬件缓存FPDMA counter*/
            __O unsigned long CLR_PDMAREQ:1 ;/*FW清除数据传输异常时,pdma的请求信号*/
            __I unsigned long DFISPATH_SEL:1 ;/*data fis传输路径选择*/
            __I unsigned long STOP_FAKE_ENG:1 ;/*中断fake engine 功能*/
            __I unsigned long STOP_CNTCRC_DATA:1 ;/*中断统计CRC 数据*/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rERR_CLR;/*<Offset:0x1D8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*计数FPDMA个数的16 bit counter发生溢出标志 */
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rOVERFLOW;/*<Offset:0x1DC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rALGNCNTEN;/*<Offset:0x1E0> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rALGNCNT;/*<Offset:0x1E4> N/A*/

    __IO unsigned char Reserved10[24];/*保留*/

    __IO unsigned long rFILL_INVLDATAL;/*<Offset:0x200>  */

    __IO unsigned long rFILL_INVLDATAH;/*<Offset:0x204>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rFIFO_HOLD;/*<Offset:0x208> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long EN_REORDER:1 ;/*N/A*/
            __IO unsigned long EN_NORDY:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rNCQ_REORDER_EN;/*<Offset:0x20C> N/A*/

    __IO unsigned long rNCQ_REORDER_REG[32];
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rNCQ_REORDER_STP;/*<Offset:0x290> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rNCQ_REORDER_TOTAL;/*<Offset:0x294> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rNCQ_REORDER_NOTREADY;/*<Offset:0x298> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rNCQ_DONE_TOTAL;/*<Offset:0x29C> N/A*/

    __IO unsigned long rNCQ_DONE_REG[32];

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSTCRCE;/*<Offset:0x320> force crc error enble*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long RXCRCE:1 ;/*RX方向制造CRC错，硬件清0*/
            __IO unsigned long TXCRCE:1 ;/*TX方向制造CRC错，硬件清0*/
            __IO unsigned long ERR_TAG:5 ;/**/
            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long ERR_DFIS_NUM:12 ;/**/
            __IO unsigned long Reserved1:5 ;/*保留*/
            __IO unsigned long SDBCRCE:1 ;/*强制SDB FIS CRC出错使能*/
            __IO unsigned long PIOSCRCE:1 ;/*强制PIO-Setup FIS CRC出错使能*/
            __IO unsigned long DMASTXCRCE:1 ;/*强制DMA-Setup FIS（D2H）CRC出错使能*/
            __IO unsigned long DMASRXCRCE:1 ;/*强制DMA-Setup FIS（H2D）CRC出错使能*/
            __IO unsigned long DMAACRCE:1 ;/*强制DMA-Active FIS CRC出错使能*/
            __IO unsigned long D2HCRCE:1 ;/*强制D2H FIS CRC出错使能*/
            __IO unsigned long H2DCRCE:1 ;/*强制H2D FIS CRC出错使能。*/
        }bits;
    }rTRXCRC;/*<Offset:0x324> TX/RX 出错控制,出错tag及出错data fis number*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPHYNRDY_DLYEN;/*<Offset:0x328> Device发起休眠前，延迟PHY not ready使能开关*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:4 ;/*N/A*/
            __IO unsigned long Reserved:28 ;/*保留*/
        }bits;
    }rPHYNRDY_DLYTM;/*<Offset:0x32C> Device发起休眠时，延迟PHY NOT READY信号时钟周期*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*enable signal for D2H FIS Tranxfer first if collision with SDB FIS*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rD2H1ST_EN;/*<Offset:0x330> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*enable for HW to transfer D2H FIS automatially */
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rHWD2HEN;/*<Offset:0x334> N/A*/

    __IO unsigned long rRECVTAG;/*<Offset:0x338>  N/A*/

    __IO unsigned long rTOTALCAPCITY;/*<Offset:0x33C>  N/A*/

    __IO unsigned long rHWD2HDONE0;/*<Offset:0x340>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rMAXTAG;/*<Offset:0x344> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long Reserved:10 ;/*保留*/
            __IO unsigned long dVAL:22 ;/*base addr for H2D FIS*/
        }bits;
    }rH2DADDR;/*<Offset:0x348> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:6 ;/*N/A*/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rACTAG;/*<Offset:0x34C> N/A*/

    __I unsigned long rDATAFIS_NUM;/*<Offset:0x350>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/**/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rREDOR_DLY_TIME;/*<Offset:0x354> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:6 ;/*H2D FIFO push index*/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rPUSH_INDEX;/*<Offset:0x358> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:6 ;/*N/A*/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rPOP_INDEX;/*<Offset:0x35C> N/A*/

    __IO unsigned long rPOP_ST;/*<Offset:0x360>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:6 ;/*N/A*/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rH2D_COUNTER;/*<Offset:0x364> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long FULL:1 ;/*N/A*/
            __IO unsigned long ALFUL:1 ;/*N/A*/
            __IO unsigned long EMPTY:1 ;/*N/A*/
            __IO unsigned long ALEMPTY:1 ;/*N/A*/
            __IO unsigned long Reserved:28 ;/*保留*/
        }bits;
    }rH2DBUF_STS;/*<Offset:0x368> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long H2DBUF_RST:1 ;/*N/A*/
            __IO unsigned long BUFH2DEN:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rH2DBUF_RST;/*<Offset:0x36C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long H2DINT_BLOCK_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long HW_PRIG_4K_EN:1 ;/*N/A*/
            __IO unsigned long BUFFER_EX_EN:1 ;/*N/A*/
            __IO unsigned long TRIG_DMASTP_EN:1 ;/*N/A*/
            __IO unsigned long WAIT_CRC_WRDMA1_EN:1 ;/*N/A*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __IO unsigned long CRCPASS_CNT_EN:1 ;/*N/A*/
            __IO unsigned long FAKE_ENGINE_EN:1 ;/*N/A*/
            __IO unsigned long Reserved2:7 ;/*保留*/
            __IO unsigned long RDEXBUF_PASCRC_EN:1 ;/*N/A*/
            __IO unsigned long TRANS_RRDY_EN:1 ;/*N/A*/
            __IO unsigned long KALIGN_PASS_EN:1 ;/*N/A*/
            __IO unsigned long PHYVLD_FILTER_EN:1 ;/*N/A*/
            __IO unsigned long FIL_CHECKEXBUF_EN:1 ;/*N/A*/
            __IO unsigned long Reserved3:11 ;/*保留*/
        }bits;
    }rFW_EN;/*<Offset:0x370> N/A*/

    __IO unsigned long rFK_ENG_LBA;/*<Offset:0x374>  N/A*/

    __IO unsigned long rFK_ENG_CNT;/*<Offset:0x378>  N/A*/

    __IO unsigned long rFK_ENG_SCTCNT;/*<Offset:0x37C>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rFK_ENG_DIR;/*<Offset:0x380> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPIER_EX;/*<Offset:0x384> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPISR_EX;/*<Offset:0x388> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSATA_FAIL_FLAG;/*<Offset:0x38C> N/A*/

    __IO unsigned long rCRC_PASS_INDEX;/*<Offset:0x390>  N/A*/

    __IO unsigned char Reserved11[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rSDB_DLY_TIME;/*<Offset:0x398> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long ACTIVE_SEND:1 ;/*N/A*/
            __IO unsigned long PRD_COMP_INT:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rACTIVE_SEND;/*<Offset:0x39C> N/A*/

    __IO unsigned long rCRC_PASS_MIN;/*<Offset:0x3A0>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPRE_TRIG_ON;/*<Offset:0x3A4> N/A*/

    __IO unsigned long rPRE_TRIG_CNT;/*<Offset:0x3A8>  N/A*/

    __IO unsigned long rPRE_TRIG_DMABO;/*<Offset:0x3AC>  N/A*/

    __IO unsigned long rSATA_CAL_TIMEL;/*<Offset:0x3B0>  N/A*/

    __IO unsigned long rSATA_CAL_TIMEH;/*<Offset:0x3B4>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSATA_CAL_ST;/*<Offset:0x3B8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSATA_CAL_STOP;/*<Offset:0x3BC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSATA_CALDONE_FLAG;/*<Offset:0x3C0> N/A*/

    __IO unsigned long rSATA_DFIS_CNTL;/*<Offset:0x3C4>  N/A*/

    __IO unsigned long rSATA_DFIS_CNTH;/*<Offset:0x3C8>  N/A*/

    __IO unsigned long rBUFF_EX_BADDR;/*<Offset:0x3CC>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long FK_ERR_LBA:1 ;/*N/A*/
            __IO unsigned long FK_ERR_FLAG:1 ;/*N/A*/
            __IO unsigned long FK_ERR_TOTLCNT:1 ;/*N/A*/
            __IO unsigned long Reserved:29 ;/*保留*/
        }bits;
    }rFK_RD_ERR;/*<Offset:0x3D0> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rFK_WR_DONE;/*<Offset:0x3D4> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rCLR_EXBUF;/*<Offset:0x3D8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long EXBUF_FULL:1 ;/*N/A*/
            __IO unsigned long EXBUF_EMPTY:1 ;/*N/A*/
            __IO unsigned long EXBUF_ALFUL:1 ;/*N/A*/
            __IO unsigned long EXBUF_ALEMPTY:1 ;/*N/A*/
            __IO unsigned long CRCPASS_EMPTY:1 ;/*N/A*/
            __IO unsigned long Reserved:27 ;/*保留*/
        }bits;
    }rEXBUF_STS;/*<Offset:0x3DC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;
                /*1: write;
                0:read*/

            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rAPB_WRITE;/*<Offset:0x3E0> N/A*/

    __IO unsigned long rAPB_ADDR;/*<Offset:0x3E4>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rAPB_PSEL;/*<Offset:0x3E8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rAPB_PENABLE;/*<Offset:0x3EC> N/A*/

    __IO unsigned long rAPB_PWDATA;/*<Offset:0x3F0>  N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rAPB_PREADY;/*<Offset:0x3F4> N/A*/

    __I unsigned long rAPB_PRDATA;/*<Offset:0x3F8>  N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rAPB_PSLVERR;/*<Offset:0x3FC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rI2C_WDATA;/*<Offset:0x400> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rI2C_RDATA;/*<Offset:0x404> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long COMRST_NUM:3 ;/*N/A*/
            __IO unsigned long COMWK_NUM:3 ;/*N/A*/
            __IO unsigned long Reserved:2 ;/*保留*/
            __IO unsigned long TX_COMINIT_MUX:1 ;/*N/A*/
            __IO unsigned long Reserved1:23 ;/*保留*/
        }bits;
    }rOOB_CFG;/*<Offset:0x408> N/A*/

    __IO unsigned char Reserved12[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:21 ;/*N/A*/
            __IO unsigned long Reserved:11 ;/*保留*/
        }bits;
    }rCOMINIT_TX_NUM1;/*<Offset:0x410> N/A*/

    __IO unsigned char Reserved13[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:21 ;/*N/A*/
            __IO unsigned long Reserved:11 ;/*保留*/
        }bits;
    }rCOMINIT_TX_NUM2;/*<Offset:0x418> N/A*/

    __IO unsigned char Reserved14[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPOP_4K;/*<Offset:0x420> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:19 ;/*N/A*/
            __IO unsigned long Reserved:13 ;/*保留*/
        }bits;
    }rOOB_WT_COMWAKE;/*<Offset:0x424> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:17 ;/*N/A*/
            __IO unsigned long Reserved:15 ;/*保留*/
        }bits;
    }rOOB_BURST;/*<Offset:0x428> N/A*/

    __IO unsigned char Reserved15[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long LINKINIT_CURRENT_STATE:4 ;/*N/A*/
            __IO unsigned long POLL_TIMEOUT:1 ;/*hardware set 1 and software clear.*/
            __IO unsigned long ALIGN_TIMEOUT:1 ;/*hardware set 1 and software clear.*/
            __IO unsigned long LINK_READY :1 ;/*hardware set 1 and software clear.*/
            __I unsigned long RESERVE:1 ;/*N/A*/
            __I unsigned long OOB_CURRENT_STATE:3 ;/*N/A*/
            __I unsigned long RESERVE1:1 ;/*N/A*/
            __IO unsigned long SPC_CNT_OVRN_OOB:1 ;/*hardware set 1 and software clear.*/
            __IO unsigned long Reserved:19 ;/*保留*/
        }bits;
    }rOOBLINK_ST;/*<Offset:0x430> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*1: enable, 0: disable*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSIGDET_GTEN;/*<Offset:0x434> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:31 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
        }bits;
    }rSIGDET_ERR;/*<Offset:0x438> N/A*/

    __IO unsigned long rSIGDET_ERR_CNT;/*<Offset:0x43C>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;
                /*1:：pre4k功能只预处理1条NCQ 4K命令；
                0：pre4k功能最大处理3条NCQ 4K命令。*/

            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPRE4K_1CMD;/*<Offset:0x440> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rGT_DMAP1_CNT;/*<Offset:0x444> N/A*/

    __IO unsigned long rH2D_ADDR1;/*<Offset:0x448>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rMULTI_PL;/*<Offset:0x44C> N/A*/
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rCRC_PASS_DLYEN;/*<Offset:0x450> N/A*/
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:5 ;/*N/A*/
            __IO unsigned long Reserved:27 ;/*保留*/
        }bits;
    }rCRC_PASS_DLYTM;/*<Offset:0x454> N/A*/
    __IO unsigned long rPRE4K_DONE_CNT;/*<Offset:0x458>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*1’b1：硬件自动回复D2H的值（DW1-DW4）跟随收到的H2D的值；1’b0：硬件自动回复D2H的值（DW1-DW4）固定为0。*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rAUTO_D2H_VALUE;/*<Offset:0x45C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:4 ;/*清除txfifo时需要等待的时钟周期数。*/
            __IO unsigned long Reserved:28 ;/*保留*/
        }bits;
    }rTXFIFO_CFG;/*<Offset:0x460> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*容量配置的高16位。与TOTALCAPCITY一起决定SC的容量大小。*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rTOTAL_CAPCITYH;/*<Offset:0x464> N/A*/

    __IO unsigned char Reserved16[408];/*保留*/

    __IO unsigned long rPDTR_BAK;/*<Offset:0x600>  N/A*/

    __IO unsigned long rPPICR_BAK;/*<Offset:0x604>  N/A*/

    __IO unsigned long rPPBCR_BAK;/*<Offset:0x608>  N/A*/

    __IO unsigned long rACTAG_BAK;/*<Offset:0x60C>  N/A*/

    __IO unsigned long rPSERR_BAK;/*<Offset:0x610>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*当COMRESET到来时，备份PRE_TRIG_ON。*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rPRE_TRIG_ON_BAK;/*<Offset:0x614> N/A*/

    __IO unsigned long rDIAGNR_BAK;/*<Offset:0x618>  N/A*/

    __IO unsigned long rDIAGNR1_BAK;/*<Offset:0x61C>  N/A*/

    __IO unsigned long rDIAGNR2_BAK;/*<Offset:0x620>  N/A*/

} SG_SC_TypeDef, *pSG_SC_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
