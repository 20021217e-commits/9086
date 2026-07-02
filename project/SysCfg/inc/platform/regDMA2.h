/*********************************************************

  模块名称: DMA2

  版本号:   20191011

*********************************************************/

#ifndef __DMA2_REG_DEF__
#define __DMA2_REG_DEF__



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



#define SG_DMA2_BASE     (0x5E2200)



typedef struct _SG_DMA2_TypeDef
{
    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short Reserved:2 ;/*保留*/
            __IO unsigned short DIR:1 ;
                /*DMA2_dir:   1-DMA2读出Flash数据写到RAM；
                DMA2读出RAM数据写到Flash；*/

            __O unsigned short STOP:1 ;/*stop_DMA2:  MCU写1停止DMA2，硬件自动在下个clock清零。*/
            __IO unsigned short JOINT_MODE:2 ;/*N/A*/
            __IO unsigned short Reserved1:2 ;/*保留*/
            __IO unsigned short ADM2BUF_MASK:1 ;/*N/A*/
            __IO unsigned short FC2ADM_MASK:1 ;/*N/A*/
            __IO unsigned short FIFO_DISABLE:1 ;/*N/A*/
            __O unsigned short FIFO_CLR:1 ;/*N/A*/
            __IO unsigned short WR_BUF_STS_KEEP:1 ;/*N/A*/
            __IO unsigned short Reserved2:3 ;/*保留*/
        }bits;
    }rCTRL;/*<Offset:0x00> N/A*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short EN:1 ;/*N/A*/
            __IO unsigned short REAL_DONE_INT_EN:1 ;/*N/A*/
            __IO unsigned short UDATA_EN:1 ;/*N/A*/
            __IO unsigned short DATA_ALL0_INTEN:1 ;/*N/A*/
            __IO unsigned short DATA_ALL1_INTEN:1 ;/*N/A*/
            __IO unsigned short DDR_TIMEOUT_INTEN:1 ;/*N/A*/
            __IO unsigned short ECC_FAIL_INTEN:1 ;/*N/A*/
            __IO unsigned short ECC_OVFLOW_INTEN:1 ;/*N/A*/
            __IO unsigned short CRC_FAIL_INTEN:1 ;/*N/A*/
            __IO unsigned short SVALUE_OVF_INTEN:1 ;/*N/A*/
            __IO unsigned short SAVE_DMAINFO_EN:1 ;/*N/A*/
            __IO unsigned short FC_FIFORDY_EN:1 ;/*N/A*/
            __IO unsigned short FC_DONE_INTEN:1 ;/*N/A*/
            __IO unsigned short HOLD_EN:1 ;/*N/A*/
            __IO unsigned short Reserved:1 ;/*保留*/
            __IO unsigned short PRNG_EN:1 ;/*N/A*/
        }bits;
    }rENABLE;
        /*<Offset:0x02> DMA2控制开关，1 为打开，此时FW填写DMA2_ctrl[0]为1 才能启动DMA2进行工作。0 为关闭，此时FW填写DMA2_ctrl[0]为1 不能启动DMA2进行工作。默认为打开状态。
        DMA2_SECT_DONE_INT_EN:DMA1中断使能开关。1：使能每次dma2（一个sector） done产生一个dma2中断信号。0：关闭使能。默认值为1.
        DMA2_ TRANS _DONE_INT_EN:DMA1中断使能开关。1：使能dma2 trans done时产生一个dma2中断信号。0：关闭使能。默认值为0.可与DMA2_SECT_DONE_INT_EN同时使能。*/


    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long BASIC_DATA_LEN:12 ;/*N/A*/
            __IO unsigned long Reserved:4 ;/*保留*/
            __IO unsigned long UDATA_LEN:6 ;/*N/A*/
            __IO unsigned long Reserved1:2 ;/*保留*/
            __IO unsigned long UDATA_ATTCH_SIZE:6 ;/*N/A*/
            __IO unsigned long Reserved2:2 ;/*保留*/
        }bits;
    }rLEN_CFG;/*<Offset:0x04> */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;/*N/A*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rUDATA_BSADDR;/*<Offset:0x08> */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long PT_NUM:6 ;/*N/A*/
            __IO unsigned long PT_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long OVFLOW_THRESHOLD:6 ;/*N/A*/
            __IO unsigned long Reserved1:2 ;/*保留*/
            __IO unsigned long SVA_OVF_TH:13 ;/*N/A*/
            __IO unsigned long Reserved2:3 ;/*保留*/
        }bits;
    }rECC_CFG;/*<Offset:0x0c> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long SVALUE:13 ;/*N/A*/
            __IO unsigned long Reserved:3 ;/*保留*/
            __I unsigned long LDPC_ITR_NUM:6 ;/*N/A*/
            __I unsigned long HPM:1 ;/*N/A*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __IO unsigned long MAX_ITR_NUM:6 ;/*N/A*/
            __IO unsigned long Reserved2:2 ;/*保留*/
        }bits;
    }rECC_STATUS;/*<Offset:0x10> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long FC_DONE:1 ;/*DMA2_real_done: “1”表示当前对DMA2的操作已经结束，可以是被stop、发生ECC fail、DDR timeout（非正常结束），或者是DMA2_trans_total_sect自减到0（正常结束）。DMA2正常结束时该bit只是表示数据传输完成，pipeline模式下ecc不一定已经完成。*/
            __I unsigned long BUF_DONE:1 ;/*DMA2_real_done: “1”表示当前对DMA2的操作已经结束，可以是被stop、发生ECC fail、DDR timeout（非正常结束），或者是DMA2_trans_total_sect自减到0（正常结束）。DMA2正常结束时该bit只是表示数据传输完成，pipeline模式下ecc不一定已经完成。*/
            __I unsigned long REAL_DONE:1 ;/*DMA2_real_done: “1”表示当前对DMA2的操作已经结束，可以是被stop、发生ECC fail、DDR timeout（非正常结束），或者是DMA2_trans_total_sect自减到0（正常结束）。DMA2正常结束时该bit只是表示数据传输完成，pipeline模式下ecc不一定已经完成。*/
            __I unsigned long HANDLE_DONE:1 ;/*N/A*/
            __IO unsigned long Reserved:4 ;/*保留*/
            __IO unsigned long DATA_ALL0_FLAG:1 ;/*N/A*/
            __IO unsigned long DATA_ALL1_FLAG:1 ;/*N/A*/
            __IO unsigned long DDR_TIMEOUT_FLAG:1 ;/*N/A*/
            __IO unsigned long ECC_FAIL_FLAG:1 ;/*N/A*/
            __IO unsigned long ECC_OVFLOW_FLAG:1 ;/*N/A*/
            __IO unsigned long CRC_FAIL_FLAG:1 ;/*N/A*/
            __IO unsigned long SVA_OVF_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __IO unsigned long FC_DONE_INT_STS:1 ;/*N/A*/
            __IO unsigned long REAL_DONE_INT_STS:1 ;/*N/A*/
            __IO unsigned long SD_NALL0_FLAG:1 ;/*N/A*/
            __IO unsigned long DMA2_SD_NALL1_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved2:12 ;/*保留*/
        }bits;
    }rSTATUS;/*<Offset:0x14> */

    __IO unsigned long rINDEX_CFG;/*<Offset:0x18>  */

    __IO unsigned short rTOTAL_SECT_CFG;
        /*<Offset:0x1c>  设置当前DMA2需要传输的sector总数，
        读回来的值仍然是上次设置的值,不随DMA2传输状态的改变而改变。*/


    __I unsigned short rTOTAL_SECT_BUF;
        /*<Offset:0x1e>  当前DMA2需要传输的sector总数，每单次传输完成会自减。
        读回来的值表示当前DMA2传输还剩下的sector数。
        每一次DMA2_START的时候会load DMA2_TOTAL_SECT_CFG寄存器里的值。*/


    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long FLASH_TRANS_INDEX:6 ;/*N/A*/
            __IO unsigned long Reserved:10 ;/*保留*/
            __I unsigned long FLASH_INDEX_BUF:6 ;/*N/A*/
            __IO unsigned long Reserved1:2 ;/*保留*/
            __I unsigned long FLASH_INDEX_LDPC:6 ;/*N/A*/
            __IO unsigned long Reserved2:2 ;/*保留*/
        }bits;
    }rFLASH_INDEX;/*<Offset:0x20> N/A*/

    __IO unsigned long rMASK;
        /*<Offset:0x24>  Mask只在DMA2读flash数据时有意义，对写无意义。
        MASK[63:0]起作用的sector地址对应为start_dma2时的[dma2_trans_index+7:dma2_trans_index].
        1：DMA2不会改写buffer里的数据，也不会改变buffer status
        0：正常操作。
        NOTE: 无论联动还是非联动模式，mask的只对此寄存器写操作之后的第一次start_dma2起作用，因为dma2_done的时候会将此寄存器清零。*/


    __IO unsigned char Reserved[4];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char RND_EN:1 ;/*N/A*/
            __IO unsigned char RND_ALG_SEL:2 ;/*N/A*/
            __IO unsigned char RND_DATA_INVERT:1 ;/*N/A*/
            __IO unsigned char RND_BYPASS:1 ;/*N/A*/
            __IO unsigned char CRC_ALG_SEL:1 ;/*N/A*/
            __IO unsigned char CRC_EN:1 ;/*N/A*/
            __IO unsigned char Reserved:1 ;/*保留*/
        }bits;
    }rRND_CTRL;/*<Offset:0x2c> N/A*/

    __IO unsigned char Reserved1[3];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:15 ;/*N/A*/
            __IO unsigned short Reserved:1 ;/*保留*/
        }bits;
    }rSSRND_SEED;/*<Offset:0x30> N/A*/

    __IO unsigned char Reserved2[2];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long RAID_EN:1 ;/*N/A*/
            __IO unsigned long RAID_MODE:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*±£Áô*/
        }bits;
    }rRAID_CTRL;/*<Offset:0x34> N/A*/

    __IO unsigned long rRAID_BUF_ADDR;/*<Offset:0x38>  N/A*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short SEED_INDEX_L:8 ;/*N/A*/
            __IO unsigned short SEED_INDEX_H:8 ;/*N/A*/
        }bits;
    }rSEED_INDEX;/*<Offset:0x3c> N/A*/

    __IO unsigned char Reserved3[2];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long BCH_CNT:6 ;/*N/A*/
            __I unsigned long FIFO_READY:1 ;/*N/A*/
            __I unsigned long ABNORMAL_STOP_ADM2RAM:1 ;/*N/A*/
            __I unsigned long FC_FIFO_READY:1 ;/*N/A*/
            __IO unsigned long LDPC_IDLE:1 ;/*N/A*/
            __I unsigned long READY:1 ;/*N/A*/
            __I unsigned long TRANS_ABNORMAL_BUF:1 ;/*N/A*/
            __I unsigned long UDATA_EN_FC:1 ;/*N/A*/
            __I unsigned long UDATA_EN_BUF:1 ;/*N/A*/
            __I unsigned long CSTATE_LDPC:3 ;/*N/A*/
            __I unsigned long CSTATE_FC:3 ;/*N/A*/
            __IO unsigned long Reserved:2 ;/*保留*/
            __I unsigned long FIFO_CNT:10 ;/*N/A*/
        }bits;
    }rDEBUG_STS0;/*<Offset:0x40> */

    union
    {
        __I unsigned short wAll;
        struct
        {
            __I unsigned short wVAL:1 ;/*N/A*/
            __IO unsigned short Reserved:15 ;/*保留*/
        }bits;
    }rTOTAL_SECT_FC;/*<Offset:0x44> N/A*/

    __I unsigned short rTOTAL_SECT_LDPC;/*<Offset:0x46>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long ECC_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:7 ;/*保留*/
            __IO unsigned long LDPC_MOD:4 ;/*N/A*/
            __IO unsigned long BYPASS_LP:1 ;/*N/A*/
            __IO unsigned long Reserved1:3 ;/*保留*/
            __IO unsigned long SB_NUM:3 ;/*N/A*/
            __IO unsigned long SB_EN:1 ;/*N/A*/
            __IO unsigned long Reserved2:12 ;/*保留*/
        }bits;
    }rECC_CTRL;/*<Offset:0x48> N/A*/

    __IO unsigned long rBSADDR;/*<Offset:0x4c>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long OVF_CNT:16 ;/*N/A*/
            __IO unsigned long SVA_OVF_CNT:16 ;/*N/A*/
        }bits;
    }rOVF_CNT;/*<Offset:0x50> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;/*N/A*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rDMAINFO_BSADDR;/*<Offset:0x54> N/A*/

    union
    {
        __I unsigned short wAll;
        struct
        {
            __I unsigned short wVAL:14 ;/*N/A*/
            __IO unsigned short Reserved:2 ;/*保留*/
        }bits;
    }rINDEX_OF_RAM;/*<Offset:0x58> 取余后的index*/

    __IO unsigned char Reserved4[2];/*保留*/

    __IO unsigned long rBSADDR_HANDLE;/*<Offset:0x5c>  */

    union
    {
        __O unsigned long dAll;
        struct
        {
            __O unsigned long START_DMA2_BUF:1 ;/*N/A*/
            __O unsigned long START_DMA2_FC:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rSTART;/*<Offset:0x60> */

    __I unsigned long rBUF_INDEX_LDPC;/*<Offset:0x64>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long BUF_OV_START:1 ;/*N/A*/
            __IO unsigned long FC_OV_START:1 ;/*N/A*/
            __IO unsigned long FIFO_OV_WR:1 ;/*N/A*/
            __IO unsigned long DIR_WR_NODONE:1 ;/*N/A*/
            __I unsigned long TOTAL_LEN_FC:12 ;/*N/A*/
            __I unsigned long TOTAL_LEN_BUF:12 ;/*N/A*/
            __IO unsigned long FSM_BUF:3 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
        }bits;
    }rDEBUG_STS1;/*<Offset:0x68> */

    __I unsigned long rBUF_INDEX_FAIL;/*<Offset:0x6c>  */

    __I unsigned char rFLASH_INDEX_FAIL;/*<Offset:0x70>  */

    __IO unsigned char Reserved5[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long FAIL_STOP_CQ_STS_CE0:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE1:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE2:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE3:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE4:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE5:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE6:1 ;/*N/A*/
            __IO unsigned long FAIL_STOP_CQ_STS_CE7:1 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rFAIL_STOP_CQ_STS;/*<Offset:0x74> N/A*/

    __I unsigned long rINDEX_BUF;/*<Offset:0x78>  */

    __IO unsigned char Reserved6[4];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short USER_DATA_LEN_HANDLE:6 ;/*N/A*/
            __IO unsigned short Reserved:2 ;/*保留*/
            __IO unsigned short UDATA_ATTACH_SIZE_HANDLE:6 ;/*N/A*/
            __IO unsigned short Reserved1:2 ;/*保留*/
        }bits;
    }rUDATA_LEN_HANDLE;/*<Offset:0x80> N/A*/

    __IO unsigned char Reserved7[2];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;/*N/A*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rTABLE_EXEC_EN;/*<Offset:0x84> N/A*/

    __IO unsigned char Reserved8[3];/*保留*/

    __IO unsigned short rTABLE_EXEC_INFO;/*<Offset:0x88>  N/A*/

} SG_DMA2_TypeDef, *pSG_DMA2_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
