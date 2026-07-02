/*********************************************************

  模块名称: FC

  版本号:   0.3

*********************************************************/

#ifndef __FC_REG_DEF__
#define __FC_REG_DEF__

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



#define SG_FC_BASE     (0x5ec000)



typedef struct _SG_FC_TypeDef
{
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long DDR_WR_FIFO_CTRL_EN:1 ;/*it is use shut contrl down and kick off FIFO matgin function*/
            __IO unsigned long DDR_WR_FIFO_THROD:3 ;/*N/A*/
            __IO unsigned long FLASH_MODE:2 ;/*it is use define flash timging interface mode */
            __IO unsigned long FC_RD_DATAPH_MODE:1 ;/*FC read flash data path is 16bit width or 8 bits width */
            __IO unsigned long FC_WR_DATAPH_MODE:1 ;/**/
            __IO unsigned long ALL_0X00_THRESHOLD:8 ;/*N/A*/
            __IO unsigned long RD_SMP_CFG:2 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long FC_RD_32_MODE:1 ;/*N/A*/
            __IO unsigned long FC_WR_32_MODE:1 ;/*N/A*/
            __IO unsigned long Reserved1:3 ;/*保留*/
            __IO unsigned long ALL_0XFF_THRESHOLD:8 ;/*N/A*/
        }bits;
    }rNF_CFG;/*<Offset:0x00> it is FC contrler register */

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;/*N/A*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rALL_00FF_CTRL;
        /*<Offset:0x04> 这个是enable的功能：
        1：表示开启全00、ff的判断功能
        0：表示关闭全00/ff的判断功能，即不会产生all_00_flag/all_ff_flag*/


    __IO unsigned char Reserved[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char NF_BP:1 ;/*N/A*/
            __IO unsigned char SPC_DATA_SEL:1 ;/*N/A*/
            __IO unsigned char SPC_LDPC_BP:1 ;/*N/A*/
            __IO unsigned char SPC_RND_BP:1 ;/*N/A*/
            __IO unsigned char SPC_CRC_BP:1 ;/*N/A*/
            __IO unsigned char SPC_ALL0F_BP:1 ;/*N/A*/
            __IO unsigned char SPC_FILT_BP:1 ;/*N/A*/
            __IO unsigned char SPC_UDATA_EN:1 ;/*N/A*/
        }bits;
    }rNF_BP_CTRL;/*<Offset:0x08> */

    __IO unsigned char Reserved1[3];/*保留*/

    __IO unsigned long rSPC_PATTERN;/*<Offset:0x0C>  */

    __IO unsigned long rSPC_UDATA0;/*<Offset:0x10>  */

    __IO unsigned long rSPC_UDATA1;/*<Offset:0x14>  */

    __IO unsigned long rSPC_UDATA2;/*<Offset:0x18>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long FIO_OPERATOR:8 ;/*N/A*/
            __IO unsigned long FIO_SPOP_TYP:3 ;/*N/A*/
            __IO unsigned long Reserved:21 ;/*保留*/
        }bits;
    }rFIO_SPOP_CTRL;/*<Offset:0x1C> N/A*/

    __IO unsigned long rSPC_UDATA3;/*<Offset:0x20>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rMCU_SET_RD;/*<Offset:0x24> N/A*/

    __IO unsigned long rRX_TMOT_THROD;/*<Offset:0x28>  N/A*/
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rRX_TMOT_EN;/*<Offset:0x2C> N/A*/
    __I unsigned long rRX_TMOT_TIME;/*<Offset:0x30>  N/A*/

    __IO unsigned long rSPC_RAM_BSADDR;/*<Offset:0x34>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long WR_WARMUP_CYC:4 ;/*N/A*/
            __IO unsigned long RD_WARMUP_CYC:4 ;/*N/A*/
            __IO unsigned long WARMUP_EN:1 ;/*N/A*/
            __IO unsigned long WARMUP_EVERY_DMA:1 ;/*N/A*/
            __IO unsigned long Reserved:10 ;/*保留*/
            __IO unsigned long RPRE_CYC:4 ;/*N/A*/
            __IO unsigned long Reserved1:8 ;/*保留*/
        }bits;
    }rDDR_TIMING_CFG;/*<Offset:0x38> N/A*/

    __IO unsigned char Reserved2[4];/*±£Áô*/

    union
    {
        __O unsigned long dAll;
        struct
        {
            __O unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSEND_RD_PULSE;/*<Offset:0x40> N/A*/

    union
    {
        __O unsigned long dAll;
        struct
        {
            __O unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rSEND_WR_PULSE;/*<Offset:0x44> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rRW_PULSE_DONE;/*<Offset:0x48> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long RW_DATA:8 ;/*N/A*/
            __I unsigned long RW_DATA2:8 ;/*N/A*/
            __I unsigned long FIO_DATA:8 ;/*N/A*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rRW_PULSE_DATA;/*<Offset:0x4C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:2 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rCQ_SET_RD_CFG_DQS;/*<Offset:0x50> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long Reserved:1 ;/*±£Áô*/
            __O unsigned long CQ_CLEAR:1 ;/*写1清楚CQ FIFO RAM ，HW自动清零*/
            __IO unsigned long Reserved1:7 ;/*±£Áô*/
            __IO unsigned long CQ_FIFO_RDY_INT_EN:1 ;/*N/A*/
            __IO unsigned long Reserved2:22 ;/*±£Áô*/
        }bits;
    }rCQ_CTRL;/*<Offset:0x54> N/A*/

    __IO unsigned char Reserved3[12];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CQ_INT_INDEX:7 ;/*N/A*/
            __IO unsigned long CQ_INT_STOP_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rCQ_INT_INFO;/*<Offset:0x64> N/A*/

    __IO unsigned char Reserved4[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long REDUANT_FILL_DATA_RND_EN:1 ;/*N/A*/
            __IO unsigned long FILT_EN:1 ;/*N/A*/
            __IO unsigned long FILL_DATA_SEL:1 ;/*N/A*/
            __IO unsigned long FILL_DATA_RND_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:12 ;/*保留*/
            __IO unsigned long BADCOL_FILL_DATA:8 ;/*N/A*/
            __IO unsigned long REDUANT_FILL_DATA:8 ;/*N/A*/
        }bits;
    }rBAD_COLUMN_CTRL;/*<Offset:0x6C> N/A*/

    __IO unsigned long rDMA2_FILTER_ECC_CFG_BADDR;/*<Offset:0x70>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rBADCOL_INFO_BADDR;/*<Offset:0x74> N/A*/

    __IO unsigned char Reserved5[4];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rRND_SEED;/*<Offset:0x7C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long TOTAL_CYC:4 ;/*N/A*/
            __IO unsigned long LOW_CYC:4 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rALE_CLE_TIM_CFG;/*<Offset:0x80> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rMCU_CFG_CE;/*<Offset:0x84> N/A*/

    union
    {
        __O unsigned long dAll;
        struct
        {
            __O unsigned long CMD1:8 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
            __O unsigned long DELAY:5 ;/*N/A*/
            __IO unsigned long Reserved1:2 ;/*保留*/
            __O unsigned long AUTO_PULLRE_EN:1 ;/*N/A*/
        }bits;
    }rMCU_CFG_CLE;/*<Offset:0x88> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __O unsigned long ADDR1:8 ;/*N/A*/
            __O unsigned long ADDR2:8 ;/*N/A*/
            __O unsigned long ADDR3:8 ;/*N/A*/
            __IO unsigned long ADDR_SEL:2 ;/*N/A*/
            __IO unsigned long Reserved:6 ;/*保留*/
        }bits;
    }rMCU_CFG_ALE;/*<Offset:0x8C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long DMA2_ABNL_HOLD_CQ_FLAG:1 ;/*N/A*/
            __IO unsigned long CQ_HOLD_INT_EN:1 ;/*N/A*/
            __IO unsigned long CQ_PAUSE_EN:1 ;/*N/A*/
            __IO unsigned long DMA2_ABNL_HOLD_CQ_EN:1 ;/*N/A*/
            __IO unsigned long CQ_HOLD_INT_REAL:1 ;/*N/A*/
            __IO unsigned long CQ_HOLD_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved:26 ;/*±£Áô*/
        }bits;
    }rCQ_HOLD_CTRL;/*<Offset:0x90> N/A*/

    __IO unsigned char Reserved6[8];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rMCU_CFG_DONE;/*<Offset:0x9C> N/A*/

    __IO unsigned char Reserved7[8];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rDQS_CNT;/*<Offset:0xA8> N/A*/

    __IO unsigned char Reserved8[12];/*保留*/

    __IO unsigned long rCHK_ST_RB_THROD_MIN;/*<Offset:0xB8>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:10 ;/*N/A*/
            __IO unsigned long Reserved:22 ;/*保留*/
        }bits;
    }rCQ_FIFO_THROD;/*<Offset:0xBC> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CKST_TRPRE:4 ;/*N/A*/
            __IO unsigned long CKST_DDR_T:4 ;/*N/A*/
            __IO unsigned long CKST_SDR_T:4 ;/*N/A*/
            __IO unsigned long CKST_TSMP:4 ;/*N/A*/
            __IO unsigned long Reserved:15 ;/*保留*/
            __IO unsigned long CKST_DOUBLE_SMP_DIS:1 ;/*N/A*/
        }bits;
    }rCHK_ST_TIM_CFG;/*<Offset:0xC0> N/A*/

    __IO unsigned long rCHK_ST_RB_CNT;/*<Offset:0xC4>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CHK_ST_MAX_CTRL:1 ;/*N/A*/
            __IO unsigned long CHK_ST_MAXTIME_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved:2 ;/*保留*/
            __IO unsigned long CHK_ST_FAIL_CTRL:1 ;/*N/A*/
            __I unsigned long CQ_FIFO_ST:1 ;/*N/A*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __IO unsigned long CHK_ST_FAIL_INT_EN:1 ;/*N/A*/
            __IO unsigned long CHK_ST_MIN_CTRL:1 ;/*N/A*/
            __IO unsigned long CHK_ST_MINTIME_FLAG:1 ;/*N/A*/
            __IO unsigned long CHK_ST_MAXTIME_INT_EN:1 ;/*N/A*/
            __IO unsigned long CHK_ST_MINTIME_INT_EN:1 ;/*N/A*/
            __IO unsigned long CHK_ST_FAIL_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved2:19 ;/*保留*/
        }bits;
    }rCHK_CQ_CTRL;/*<Offset:0xC8> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long DATA_ZERO_CNT:16 ;/*N/A*/
            __I unsigned long DATA_ONE_CNT:16 ;/*N/A*/
        }bits;
    }rALL_0X00_0XFF_CNT;/*<Offset:0xCC> N/A*/

    __I unsigned long rCHK_ST_TIME;/*<Offset:0xD0>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CQ_INT_FLAG:1 ;/*写0清零*/
            __I unsigned long CQ_CHK_HOLD_INT:1 ;/**/
            __I unsigned long CHK_ST_FAIL_INT:1 ;/**/
            __IO unsigned long CHK_ST_MIN_INT:1 ;/**/
            __I unsigned long CHK_ST_MAX_INT:1 ;/**/
            __IO unsigned long Reserved:1 ;/*保留*/
            __I unsigned long CQ_FIFO_RDY_INT:1 ;/**/
            __IO unsigned long Reserved1:25 ;/*保留*/
        }bits;
    }rCHK_INT_ST;/*<Offset:0xD4> N/A*/

    __IO unsigned long rCQ_INSTRUCT_REG;/*<Offset:0xD8>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long CQ_FIFO_IDLE_SPACE:11 ;/*N/A*/
            __IO unsigned long Reserved:3 ;/*保留*/
            __I unsigned long CQ_FSM_IDLE:1 ;/*N/A*/
            __I unsigned long CQ_FIFO_RDY_FLAG:1 ;/*N/A*/
            __I unsigned long CQ_VALID_CNT:11 ;/*N/A*/
            __IO unsigned long Reserved1:4 ;/*保留*/
            __IO unsigned long CQ_FIFO_OVERFLOW_FLAG:1 ;/*N/A*/
        }bits;
    }rCQ_FIFO_ST_CTRL;/*<Offset:0xDC> N/A*/

    __IO unsigned char Reserved9[4];/*保留*/

    __IO unsigned long rLOGIC_NUM;/*<Offset:0xE4>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long SYS_RX_CS:5 ;/*N/A*/
            __IO unsigned long RXFIFO_EMPTY:1 ;/*N/A*/
            __IO unsigned long RXFIFO_FULL:1 ;/*N/A*/
            __IO unsigned long RXFIFO_DDR_FULL:1 ;/*N/A*/
            __IO unsigned long FIF_TX_CS:6 ;/*N/A*/
            __IO unsigned long TXFIFO_EMPTY:1 ;/*N/A*/
            __IO unsigned long TXFIFO_FULL:1 ;/*N/A*/
            __IO unsigned long FIF_RX_CS:10 ;/*N/A*/
            __IO unsigned long SYS_TX_CS:5 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
        }bits;
    }rDEBUG_REG0;/*<Offset:0xE8> N/A*/

    __I unsigned long rCQ_CMD_RD;/*<Offset:0xEC>  N/A*/

    __IO unsigned long rLOGIC_NUM1;/*<Offset:0xF0>  N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long CQ_CS:8 ;/*N/A*/
            __I unsigned long TXCA_CS:6 ;/*N/A*/
            __IO unsigned long Reserved:2 ;/*保留*/
            __I unsigned long CKST_CS:7 ;/*N/A*/
            __IO unsigned long Reserved1:9 ;/*保留*/
        }bits;
    }rDEBUG_REG1;/*<Offset:0xF4> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long FIF_TX_DCNT:12 ;/*N/A*/
            __IO unsigned long Reserved:4 ;/*保留*/
            __I unsigned long FIF_RXFIFO_WDCNT:9 ;/*N/A*/
            __IO unsigned long Reserved1:7 ;/*保留*/
        }bits;
    }rDEBUG_REG2;/*<Offset:0xF8> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long DQS_CNT_TMP:16 ;/*N/A*/
            __I unsigned long SYS_DCNT:16 ;/*N/A*/
        }bits;
    }rDEBUG_REG3;/*<Offset:0xFC> N/A*/

} SG_FC_TypeDef, *pSG_FC_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
