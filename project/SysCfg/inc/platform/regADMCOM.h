/*********************************************************

  模块名称: ADMCOM

  版本号:   201901011

*********************************************************/

#ifndef __ADMCOM_REG_DEF__
#define __ADMCOM_REG_DEF__



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



#define SG_ADMCOM_BASE     (0x5E2000)



typedef struct _SG_ADMCOM_TypeDef
{
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rREAD_BUF_STS;/*<Offset:0x0> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rSET_BIT_STS;/*<Offset:0x4> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rCLR_BIT_STS;/*<Offset:0x8> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rBIT_INDEX;/*<Offset:0xc> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rRESET_BUF_STS;/*<Offset:0x10> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rMCU_RDATA_STS;/*<Offset:0x14> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long FRAME_SIZE:7 ;/*N/A*/
            __IO unsigned long Reserved:9 ;/*保留*/
            __IO unsigned long SEED_INDEX_SIZE:8 ;/*N/A*/
            __IO unsigned long Reserved1:8 ;/*保留*/
        }bits;
    }rTOS_SEED_OPR;/*<Offset:0x18> */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;/*设置dma1的BUF基地址，需要8字节对齐*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rTOS_SEED_BSADDR;/*<Offset:0x1c> N/A*/

    __IO unsigned short rBUF_SIZE;/*<Offset:0x20>  N/A*/

    __IO unsigned char Reserved[2];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long ALL0_STOP_DMA2_EN:1 ;/*dma20 data all0 stop dma2 trans使能，1:使能后,all0的dma不会向darb发起dma请求；0：disable*/
            __IO unsigned long ALL1_STOP_DMA2_EN:1 ;/*dma20 data all1 stop dma2 trans使能，1:使能后,all1的dma不会向darb发起dma请求；0：disable*/
            __IO unsigned long DDR_TIMEOUT_STOP_DMA2_EN:1 ;/*ddr timeout stop dma2 trans使能，1:使能后ddr_timeout的dma不会向darb发起dma请求；0：disable*/
            __IO unsigned long ECC_FAIL_STOP_DMA2_EN:1 ;/*ecc fail stop dma2使能，1:使能后，Ecc fail的dma不会向DARB发起dma请求不改变buf_sts；0：disable*/
            __IO unsigned long CRC_ERR_STOP_DMA2_EN:1 ;/*crc error stop dma2使能，1:使能后, crc error的dma不会向DARB发起dma请求且不会改变buf_sts；0：disable*/
            __IO unsigned long ECC_OVF_STOP_DMA2_EN:1 ;/*ecc overflow stop dma2使能，1:使能后, ecc ovf的dma不会向DARB发起dma请求且不会改变buf_sts；0：disable*/
            __IO unsigned long CRC_ERR_NO_W_BUF_STS:1 ;
                /*crc error不改变buf_sts使能，1:使能后不会改变buf_sts；0：disable
                不使能crc_fail_stop_dma2时，可以让之前的CQ继续走完，但是出错的dma不会改sts，不会被dma1传出去*/

            __IO unsigned long ECC_FAIL_NO_W_BUF_STS:1 ;
                /*ecc fail不改变buf_sts使能，1:使能后不会改变buf_sts；0：disable
                不使能ecc_fail_stop_dma2时，可以让之前的CQ继续走完，但是出错的dma不会改sts，不会被dma1传出去*/

            __IO unsigned long Reserved:2 ;/*保留*/
            __IO unsigned long SVALUE_OVF_HOLD_CQ_EN:1 ;/*DDR timeout hold cq使能，1:使能后FC在ddrtimeout的时候会收到异常信号，在执行hold cq指令时会产生hold_int,能hold cq的执行；0：disable*/
            __IO unsigned long ECC_FAIL_HOLD_CQ_EN:1 ;/*ecc fail hold cq使能，1:使能后FC在ecc fail的时候会收到异常信号，在执行hold cq指令时会产生hold_int,能hold cq的执行；0：disable*/
            __IO unsigned long CRC_ERR_HOLD_CQ_EN:1 ;/*crc error hold cq使能，1:使能后FC在crc error的时候会收到异常信号，在执行hold cq指令时会产生hold_int,能hold cq的执行；0：disable*/
            __IO unsigned long ECC_OVF_HOLD_CQ_EN:1 ;/*ecc overflow hold cq使能，1:使能后fc在ecc ovf的时候会收到异常信号，在执行hold cq指令时会产生hold_int,能hold cq的执行；0：disable*/
            __IO unsigned long Reserved1:18 ;/*保留*/
        }bits;
    }rDATA_ERR_STOP_DMA2_EN;/*<Offset:0x24> N/A*/

    __IO unsigned short rPRNG_TH;/*<Offset:0x28>  N/A*/

    __IO unsigned char Reserved1[22];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;/*N/A*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rTABLE_EXEC_BSADDR;/*<Offset:0x40> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rTABLE_EXEC_RD_POINT;/*<Offset:0x44> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rTABLE_EXEC_WR_POINT;/*<Offset:0x48> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rTABLE_EXEC_FIFO_SIZE;/*<Offset:0x4c> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rTABLE_EXEC_CNT;/*<Offset:0x50> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rTABLE_EXEC_RD_FIFO;/*<Offset:0x54> N/A*/

    __IO unsigned char Reserved2[2664];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __IO unsigned long DMA_CS:6 ;/*N/A*/
            __IO unsigned long DMA_TRANS_CNT_BUF_PRE:13 ;/*N/A*/
            __IO unsigned long DMA_TOTAL_LEN:13 ;/*N/A*/
        }bits;
    }rDMA_DEBUG_STATUS0;/*<Offset:0xac0> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long FIFO_RDY:1 ;/*N/A*/
            __I unsigned long DMA_BUF_DATA_DONE:1 ;/*N/A*/
            __I unsigned long USER_DATA_DOING:1 ;/*N/A*/
            __I unsigned long BASIC_DATA_DOING:1 ;/*N/A*/
            __I unsigned long FIFO_OV_WR:1 ;/*N/A*/
            __I unsigned long FIFO_EMPTY:1 ;/*N/A*/
            __I unsigned long FIFO_AFULL:1 ;/*N/A*/
            __I unsigned long RM_DMA_ENC_READY:1 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rDMA_DEBUG_STATUS1;/*<Offset:0xac4> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long DARB_CSTATE:7 ;/*N/A*/
            __I unsigned long ROLL_FLAG:2 ;/*N/A*/
            __IO unsigned long Reserved:23 ;/*保留*/
        }bits;
    }rDARB_DEBUG_STATUS;/*<Offset:0xac8> N/A*/

} SG_ADMCOM_TypeDef, *pSG_ADMCOM_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
