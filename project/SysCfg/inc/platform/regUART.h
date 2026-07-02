/*********************************************************

  模块名称: UART

  版本号:   075

*********************************************************/

#ifndef __UART_REG_DEF__
#define __UART_REG_DEF__



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



#define SG_UART_BASE     (0x5E3000)



typedef struct _SG_UART_TypeDef
{
    __IO unsigned char Reserved[32];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:14 ;/*N/A*/
            __IO unsigned short Reserved:2 ;/*保留*/
        }bits;
    }rBAUD_CTRL;/*<Offset:0x20> Baud rate control register. UART tx/rx frequency = Fsys_clk / BAUD_CTRL*/

    __IO unsigned char Reserved1[2];/*保留*/

    __IO unsigned long rDMA_ADDR;/*<Offset:0x24>  DMA模式下，RAM操作以8bytes为单位；地址需8byte对齐。*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:11 ;/*N/A*/
            __IO unsigned short Reserved:5 ;/*保留*/
        }bits;
    }rDMA_LEN_TX;
        /*<Offset:0x28> DMA发送长度，以byte为单位，为DMA_LEN_TX Byte。即DMA_LEN_TX = 128，表示DMA数据长度为128 Byte。支持DMA最大长度2047 Byte。
        Note1: DMA_LEN_TX值不能设置成0
        Note2: 一旦FW操作DMA_LEN_TX，HW将开始从DMA RAM读取数据并发送
        Note3: this register is the same as DMA_LEN_RX for the HW.*/


    __IO unsigned char Reserved2[150];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:11 ;/*N/A*/
            __IO unsigned short Reserved:5 ;/*保留*/
        }bits;
    }rDMA_LEN_RX;
        /*<Offset:0xc0> DMA接收长度，以byte为单位，为DMA_LEN_RX Byte。即DMA_LEN_RX = 128，表示DMA数据长度为128 Byte。支持DMA最大长度2047 Byte。
        Note1: DMA_LEN_RX值不能设置成0
        Note2: 一旦FW操作DMA_LEN_RX，HW将开始接收数据并写入到DMA RAM
        Note3: this register is the same as DMA_LEN_TX for the HW.*/


    __IO unsigned char Reserved3[62];/*保留*/

    __IO unsigned char rTX_DATA;
        /*<Offset:0x100>  Normal data that will be transmitted. (非DMA模式)
        Note: HW will transmit this data if FW writes TX_DATA.*/


    __IO unsigned char Reserved4[3];/*保留*/

    __I unsigned char rRX_DATA;/*<Offset:0x104>  RX_DATA [7:0]:  data received buffer*/

    __IO unsigned char Reserved5[3];/*保留*/

    union
    {
        __I unsigned short wAll;
        struct
        {
            __I unsigned short wVAL:11 ;/*N/A*/
            __IO unsigned short Reserved:5 ;/*保留*/
        }bits;
    }rTRANS_CNT;
        /*<Offset:0x108> UART传输长度，以byte为单位，为TRANS_CNT Byte。即TRANS_CNT = 128，在TX DMA mode,表示TX已发送的数据长度为128 Byte，在RX DMA mode, 则表示RX已接收并保存到DMA RAM的数据长度为128 Byte。
        Note: this register is cleared by HW when TX or RX starts under DMA mode.*/


    __IO unsigned char Reserved6[2];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:2 ;/*N/A*/
            __IO unsigned char Reserved:6 ;/*保留*/
        }bits;
    }rTX_GAP;
        /*<Offset:0x10c> UART transmit gap between the end of one transfer and the next start of transfer.
        Unit: UART baud rate bit time.*/


    __IO unsigned char Reserved7[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __IO unsigned char FRAME_ERROR:1 ;/*D0: frame error---1: RX stop bit error*/
            __IO unsigned char PARIT_ERROR:1 ;/*parity error--- 1: RX parity bit error*/
            __IO unsigned char Reserved:6 ;/*保留*/
        }bits;
    }rRX_STS;/*<Offset:0x110> */

    __IO unsigned char Reserved8[3];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short UART_TX_EN:1 ;/*D0: UART_TX_EN---1: enable UART TX function 0: disable default: 0*/
            __IO unsigned short PARITY_EN:1 ;/*D1: PARITY_EN--- 1: enable parity bit 0: disable default: 0*/
            __IO unsigned short FIFO_TX_MODE:1 ;/**/
            __IO unsigned short Reserved:1 ;/*保留*/
            __IO unsigned short UART_RX_EN:1 ;/*D4: UART_RX_EN---1: enable UART RX function 0: disable default: 0*/
            __IO unsigned short Reserved1:3 ;/*保留*/
            __IO unsigned short PARITY_SEL:1 ;/*D8: PARITY_SEL--- 1: odd parity 0: even parity default: 0*/
            __IO unsigned short Reserved2:7 ;/*保留*/
        }bits;
    }rUART_CFG;/*<Offset:0x114> */

    __IO unsigned char Reserved9[2];/*保留*/

    union
    {
        __O unsigned short wAll;
        struct
        {
            __O unsigned short wVAL:1 ;/*D0: UART_STOP--- write 1 to stop UART TX & RX, in other words, to reset the UART module.*/
            __IO unsigned short Reserved:15 ;/*保留*/
        }bits;
    }rUART_CTRL;/*<Offset:0x118> */

    __IO unsigned char Reserved10[2];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short TX_DONE :1 ;/*W1 to clear the corresponding bit*/
            __IO unsigned short RX_DONE :1 ;/*W1 to clear the corresponding bit*/
            __IO unsigned short FIFO_OV:1 ;/*W1 to clear the corresponding bit*/
            __IO unsigned short FIFO_WL:1 ;/*W1 to clear the corresponding bit*/
            __IO unsigned short Reserved:12 ;/*保留*/
        }bits;
    }rINT_STS;/*<Offset:0x11c> */

    __IO unsigned char Reserved11[2];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char TX_INT_EN :1 ;/*D0: TX_INT_EN --- 1: enable TX interrupt*/
            __IO unsigned char RX_INT_EN :1 ;/*D1: RX_INT_EN --- 1: enable RX interrupt*/
            __IO unsigned char FIFO_OV:1 ;/**/
            __IO unsigned char FIFO_WL:1 ;/**/
            __IO unsigned char Reserved:4 ;/*保留*/
        }bits;
    }rINT_EN;/*<Offset:0x120> */

    __IO unsigned char Reserved12[31];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:8 ;/*N/A*/
            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rTX_FIFO_PORT;/*<Offset:0x140> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rTX_FIFO_STS;/*<Offset:0x144> N/A*/

} SG_UART_TypeDef, *pSG_UART_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
