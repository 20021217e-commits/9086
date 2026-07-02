/*********************************************************

  模块名称: DMA1

  版本号:   20191011

*********************************************************/

#ifndef __DMA1_REG_DEF__
#define __DMA1_REG_DEF__



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



#define SG_DMA1_BASE     (0x5E2000)



typedef struct _SG_DMA1_TypeDef
{
    __IO unsigned char Reserved[256];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __O unsigned char START:1 ;/*FW写1启动dma1，HW自动清零*/
            __IO unsigned char DIR:1 ;
                /*1：dma1接收host数据
                0：dma1发送数据到host*/

            __O unsigned char STOP:1 ;/*FW写1停止DMA1，HW自动清零*/
            __IO unsigned char JOINT_EN:1 ;/*DMA1联动使能,1：使能，0：disable*/
            __IO unsigned char Reserved:4 ;/*保留*/
        }bits;
    }rCTRL;/*<Offset:0x100> N/A*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char EN:1 ;/**/
            __IO unsigned char TOTAL_DONE_INT_EN:1 ;/*dma1_done_int使能，1：使能，0：disable*/
            __IO unsigned char MC_HOLD_INTEN:1 ;/*mc_hold_int使能，1：使能，0：disable*/
            __IO unsigned char DMA1_HOLD_INTEN:1 ;/*dma1数据压缩功能使能开关*/
            __IO unsigned char DMA1_CP_EN:1 ;/*dma1数据压缩功能使能开关*/
            __IO unsigned char Reserved:3 ;/*保留*/
        }bits;
    }rENABE;/*<Offset:0x101> N/A*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:13 ;/*N/A*/
            __IO unsigned short Reserved:3 ;/*保留*/
        }bits;
    }rLEN;/*<Offset:0x102> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long TRANS_DONE:1 ;/*N/A*/
            __IO unsigned long Reserved:15 ;/*保留*/
            __IO unsigned long DMA1_DONE_INT_STS:1 ;/**/
            __IO unsigned long DMA1_HOLD_INT_STS:1 ;/**/
            __IO unsigned long DMA1_CP0_FLAG:1 ;/**/
            __IO unsigned long DMA1_CP1_FLAG:1 ;
                /*表示是否有被检测数据的存在：
                0：表示没有检测到；
                1：表示检测到满足pattern1的数据被压缩了,且已经记录了被压缩数据段的首尾index，即在检查到特殊pattern1后又检测到了不含特殊pattern的dma1
                第一个dma不满足就会置1.
                HW置1，FW清零*/

            __IO unsigned long Reserved1:4 ;/*保留*/
            __IO unsigned long MC_INT_STS:1 ;/*执行mc_int指令时HW自动置1，MCU响应该中断后需要把该位清零*/
            __IO unsigned long HOLD_INT_STS:1 ;/*使能mc_hold_inten后，MC进入hold状态后HW自动置1，FW需要清0.*/
            __IO unsigned long Reserved2:6 ;/*保留*/
        }bits;
    }rSTATUS;/*<Offset:0x104> N/A*/

    __IO unsigned long rINDEX;/*<Offset:0x108>  N/A*/

    __IO unsigned long rTOTAL_SECT;/*<Offset:0x10c>  when = 0xffff; no long limit*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:28 ;/*N/A*/
            __IO unsigned long Reserved:4 ;/*保留*/
        }bits;
    }rMC_CMD;/*<Offset:0x110> N/A*/

    union
    {
        __O unsigned char bAll;
        struct
        {
            __O unsigned char MC_STOP:1 ;/*N/A*/
            __O unsigned char MC_CLEAR:1 ;/*N/A*/
            __IO unsigned char Reserved:6 ;/*保留*/
        }bits;
    }rMC_CTRL;/*<Offset:0x114> N/A*/

    __IO unsigned char Reserved1[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rMC_READY;/*<Offset:0x118> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __I unsigned long MC_IDLE_CNT:8 ;/*N/A*/
            __I unsigned long MC_FIFO_FULL:1 ;/*N/A*/
            __I unsigned long MC_DONE:1 ;/*N/A*/
            __IO unsigned long MC_FIFO_OVER_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved:21 ;/*保留*/
        }bits;
    }rMC_STS;/*<Offset:0x11c> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long DMA1_INDEX_OF_RAM:14 ;/*N/A*/
            __I unsigned long FIFO_EMPTY:1 ;/*N/A*/
            __I unsigned long FIFO_AFULL:1 ;/*N/A*/
            __I unsigned long DMA1_TRANS_CNT_BUF:13 ;/*N/A*/
            __I unsigned long FIFO_RDY:1 ;/*N/A*/
            __I unsigned long FIFO_OV_WR:1 ;/*N/A*/
            __I unsigned long DMA1_RDY:1 ;/*N/A*/
        }bits;
    }rDEBUG_STATUS0;/*<Offset:0x120> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;/*N/A*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rBSADDR;/*<Offset:0x124> N/A*/

    __IO unsigned char Reserved2[4];/*保留*/

    __IO unsigned long rCP_PAT0;/*<Offset:0x12c>  N/A*/

    __IO unsigned long rCP_PAT1;/*<Offset:0x130>  N/A*/

    __IO unsigned long rCP0_EINDEX;/*<Offset:0x134>  N/A*/

    __IO unsigned long rCP1_EINDEX;/*<Offset:0x138>  N/A*/

    __IO unsigned char Reserved3[8];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long DMA1_TRANS_CNT_SC:13 ;/*N/A*/
            __I unsigned long DMA1_BUF_DATA_DOING:1 ;/*N/A*/
            __I unsigned long DMA1_CSTATE:3 ;/*N/A*/
            __IO unsigned long Reserved:15 ;/*保留*/
        }bits;
    }rDEBUG_STATUS1;/*<Offset:0x144> N/A*/

} SG_DMA1_TypeDef, *pSG_DMA1_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
