/*********************************************************

  模块名称: LDPC

  版本号:   0.6

*********************************************************/

#ifndef __LDPC_REG_DEF__
#define __LDPC_REG_DEF__



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



#define SG_LDPC_BASE     (0x5F2000)



typedef struct _SG_LDPC_TypeDef
{
    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CR_ID:16 ;/*选择code rate*/
            __IO unsigned long Reserved:15 ;/*保留*/
            __IO unsigned long CRN_VALID:1 ;/*code rate设置完成标记*/
        }bits;
    }rLENC_FORMAT0;/*<Offset:0x0> */

    __IO unsigned char Reserved[76];/*保留*/

    __IO unsigned long rLENC_IRQ;/*<Offset:0x50>  */

    __IO unsigned char Reserved1[32];/*保留*/

    __IO unsigned long rLENC_PMN0_CTRL;/*<Offset:0x74>  */

    __IO unsigned char Reserved2[8];/*保留*/

    __IO unsigned long rLENC_CTRL;/*<Offset:0x80>  */

    __IO unsigned char Reserved3[4];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __IO unsigned long Reserved:27 ;/*保留*/
            __I unsigned long LENC_IDLE_OUT:1 ;/**/
            __I unsigned long LENC_RECEIVED_CNT:4 ;/**/
        }bits;
    }rLENC_STATUS;/*<Offset:0x88> */

    __IO unsigned char Reserved4[116];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:23 ;/*设置写入mem的addr*/
            __IO unsigned long Reserved:9 ;/*保留*/
        }bits;
    }rMCU_WLENC_MEM_ADDR;/*<Offset:0x100> */

    __IO unsigned long rMCU_WLENC_MEM_D0;/*<Offset:0x104>  */

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long CS_TX:4 ;/*N/A*/
            __I unsigned long CS_RX:3 ;/*N/A*/
            __IO unsigned long Reserved:1 ;/*保留*/
            __I unsigned long LENC_CORE_IDLE:1 ;/*N/A*/
            __I unsigned long LENC_IDLE:1 ;/*N/A*/
            __IO unsigned long Reserved1:22 ;/*保留*/
        }bits;
    }rLENC_DEBUG_STS;/*<Offset:0x108> */

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long LENC_REG_RACK:1 ;/*N/A*/
            __I unsigned long LENC_REG_WACK:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rMCU_WLENC_REG_OP_STS;/*<Offset:0x10C> */

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long LENC_MEM_RACK:1 ;/*N/A*/
            __I unsigned long LENC_MEM_WACK:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rMCU_WLENC_MEM_OP_STS;/*<Offset:0x110> */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/**/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rMCU_WLENC_MEM_RD;/*<Offset:0x114> */

    __I unsigned long rMCU_WLENC_MEM_DOUT;/*<Offset:0x118>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long MCU_WRITE_EN:1 ;/*N/A*/
            __IO unsigned long MCU_READ_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rLENC_MEM_OP_MODE;/*<Offset:0x11C> */

    __I unsigned long rLENC_REG_ROUT;/*<Offset:0x120>  */

    __IO unsigned char Reserved5[220];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:23 ;/**/
            __IO unsigned long Reserved:9 ;/*保留*/
        }bits;
    }rMCU_WLDEC_MEM_ADDR;/*<Offset:0x200> */

    __IO unsigned long rMCU_WLDEC_MEM_DIN;/*<Offset:0x204>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/**/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rMCU_WLDEC_MEM_RD;/*<Offset:0x208> */

    __I unsigned long rMCU_WLDEC_MEM_DOUT;/*<Offset:0x20C>  */

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long LDEC_REG_RACK:1 ;/*N/A*/
            __I unsigned long LDEC_REG_WACK:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rMCU_WLDEC_REG_OP_STS;/*<Offset:0x210> */

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long LDEC_MEM_RACK:1 ;/*N/A*/
            __I unsigned long LDEC_MEM_WACK:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rMCU_WLDEC_MEM_OP_STS;/*<Offset:0x214> */

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;/*1：输出parity到DMARAM 0：不输出*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rPARITY_OUT_EN;/*<Offset:0x218> */

    __IO unsigned char Reserved6[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __I unsigned char bVAL:1 ;/*1：已经输入LDEC的来自通道0的CW全部解码完成并输出*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rLDEC_CH0_STATUS;/*<Offset:0x21C> */

    __IO unsigned char Reserved7[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __I unsigned char bVAL:1 ;/*1：已经输入LDEC的来自通道1的CW全部解码完成并输出*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rLDEC_CH1_STATUS;/*<Offset:0x220> */

    __IO unsigned char Reserved8[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __I unsigned char bVAL:1 ;/*1：已经输入LDEC的来自通道2的CW全部解码完成并输出*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rLDEC_CH2_STATUS;/*<Offset:0x224> */

    __IO unsigned char Reserved9[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __I unsigned char bVAL:1 ;/*1：已经输入LDEC的来自通道3的CW全部解码完成并输出*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rLDEC_CH3_STATUS;/*<Offset:0x228> */

    __IO unsigned char Reserved10[3];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long CS_TX:4 ;/*N/A*/
            __I unsigned long CS_RX:3 ;/*N/A*/
            __I unsigned long LDEC_CORE_IDLE:1 ;/*N/A*/
            __I unsigned long CH0_CW_CNT:6 ;/*N/A*/
            __I unsigned long CH1_CW_CNT:6 ;/*N/A*/
            __I unsigned long CH2_CW_CNT:6 ;/**/
            __I unsigned long CH3_CW_CNT:6 ;/**/
        }bits;
    }rLDEC_DEBUG_STATUS;/*<Offset:0x22C> */

    __I unsigned long rLDEC_REG_ROUT;/*<Offset:0x230>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long MCU_WRITE_EN:1 ;/*N/A*/
            __IO unsigned long MCU_READ_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rLDEC_MEM_OP_MODE;/*<Offset:0x234> */

    __IO unsigned char Reserved11[200];/*保留*/

    __IO unsigned long rLP_LDEC_CTRL;/*<Offset:0x300>  */

    __IO unsigned long rLP_LDEC_FORMAT0;/*<Offset:0x304>  */

    __IO unsigned char Reserved12[8];/*保留*/

    __IO unsigned long rLP_LDEC_OP_MEM_CTRL;/*<Offset:0x310>  */

    __IO unsigned char Reserved13[24];/*保留*/

    __IO unsigned long rLP_LDEC_STATUS;/*<Offset:0x32C>  */

    __IO unsigned long rLP_LDEC_IRQ;/*<Offset:0x330>  */

    __IO unsigned char Reserved14[12];/*保留*/

    __IO unsigned long rLP_LDEC_PMN0_CTRL;/*<Offset:0x340>  */

    __IO unsigned char Reserved15[188];/*保留*/

    __IO unsigned long rLDEC_CTRL;/*<Offset:0x400>  */

    __IO unsigned long rLDEC_FORMAT0;/*<Offset:0x404>  */

    __IO unsigned char Reserved16[8];/*保留*/

    __IO unsigned long rLDEC_H_MEM_CTRL;/*<Offset:0x410>  */

    __IO unsigned long rLDEC_SB_MAP0;/*<Offset:0x414>  */

    __IO unsigned long rLDEC_SB_MAP1;/*<Offset:0x418>  */

    __IO unsigned long rLDEC_SB_MAP2;/*<Offset:0x41C>  */

    __IO unsigned long rLDEC_SB_MAP3;/*<Offset:0x420>  */

    __IO unsigned long rLDEC_SB_MAP4;/*<Offset:0x424>  */

    __IO unsigned long rLDEC_SB_MAP5;/*<Offset:0x428>  */

    __IO unsigned long rLDEC_STATUS;/*<Offset:0x42C>  */

    union
    {
        __I unsigned long dAll;
        struct
        {
            __IO unsigned long Reserved:1 ;/*保留*/
            __I unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved1:30 ;/*保留*/
        }bits;
    }rLDEC_IRQ;/*<Offset:0x430> */

    __IO unsigned char Reserved17[4];/*保留*/

    __IO unsigned long rLDEC_SB_MAP6;/*<Offset:0x438>  */

    __IO unsigned long rLDEC_SB_MAP7;/*<Offset:0x43C>  */

    __IO unsigned long rLDEC_PMN0_CTRL;/*<Offset:0x440>  */

} SG_LDPC_TypeDef, *pSG_LDPC_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
