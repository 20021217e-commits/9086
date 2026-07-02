/*********************************************************

  模块名称: SPI

  版本号:   075

*********************************************************/

#ifndef __SPI_REG_DEF__
#define __SPI_REG_DEF__



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



#define SG_SPI_BASE     (0x5E7000)



typedef struct _SG_SPI_TypeDef
{
    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char SPI_CS:1 ;
                /*SPI_CS:  主模式下，控制spi_cs的状态；0：表示select，1(default)：表示deselect。
                从模式下，表示当前spi_cs线的状态。*/

            __IO unsigned char Reserved:1 ;/*保留*/
            __IO unsigned char RX_DORD:1 ;/*RX_DORD:  RX data order, 0: First bit is MSB; 1: First bit is LSB;*/
            __IO unsigned char TX_DORD:1 ;
                /*TX_DORD:  Data order    0: MSB first data transmission(高位在前).     
                1: LSB first data transmission (低位在前)*/

            __IO unsigned char CPOL:1 ;/*CPOL: 时钟极性, 表示 SPI 在空闲时, 时钟信号是高电平还是低电平. 若 CPOL 被设为 1, 那么该设备在空闲时 SCK 管脚下的时钟信号为高电平. 当 CPOL 被设为 0 时则正好相反*/
            __IO unsigned char CPHA:1 ;/*CPHA: 时钟相位, 表示 SPI 设备是在 SCK 管脚上的时钟信号变为上升沿时触发数据采样, 还是在时钟信号变为下降沿时触发数据采样. 若 CPHA 被设置为 0, 则 SPI 设备在第1个时钟沿采样数据, 在第2个时钟沿时发送数据. 当 CPHA 被设为 1 时则 SPI 设备在第2个时钟沿采样数据.*/
            __IO unsigned char SPI_MODE:1 ;/*SPI_MODE:  0 (default):  SPI master mode; 1: SPI slave mode*/
            __IO unsigned char SPI_ENABLE:1 ;/*SPI_ENABLE:   0 : Disable the SPI; 1 : Enable the SPI. 全局使能，如果为0整个SPI不工作。*/
        }bits;
    }rCONFIG_SPI;/*<Offset:0x00> */

    __IO unsigned char Reserved[7];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char NOR_TRNS_WIDTH:2 ;
                /*NOR_TRNS_WIDTH: (bit1~bit0)   Normal data transfer IO width(非DMA模式): 
                0: standard SPI transmission；  
                1: dual IO transmission。*/

            __IO unsigned char HZ_CTL:2 ;
                /*HZ_CTL: (bit3~bit2) hiz control:  （Master模式下有效）
                0: normal output; 
                1: output high Z state on data lines for the whole data byte; 
                2: output high Z state on data lines for the last four bits of the data byte*/

            __IO unsigned char DMA_TRNS_WIDTH:2 ;
                /*DMA_TRNS_WIDTH: (bit5~bit4) DMA IO width after current data/command: 
                0: standard SPI transmission, 
                1: dual IO transmission  
                2:quad IO transmission*/

            __IO unsigned char DMA_TX_RD_RAM_ENABLE:1 ;
                /*DMA_TX_RD_RAM_ENABLE: (bit6)  
                主模式DMA操作时，发送数据来自RAM使能。（Master模式下有效，slave模式无效）
                0：不从RAM读取数据，DMA时发送NOR_TX_DATA中数据；
                1：发送数据从RAM读取，对应RAM地址DMA_BASE_ADDR_TX*/

            __IO unsigned char DMA_RX_WR_RAM_ENABLE:1 ;
                /*DMA_RX_WR_RAM_ENABLE: (bit7)  
                主模式DMA操作，接收数据写入RAM使能。0：接收到数据不写入RAM；
                1：接收到数据写入RAM，对应RAM地址DMA_BASE_ADDR_RX。*/

        }bits;
    }rTRANS_SET;/*<Offset:0x08> */

    __IO unsigned char Reserved1[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:7 ;/*N/A*/
            __IO unsigned char Reserved:1 ;/*保留*/
        }bits;
    }rDMA_DLY_SET;/*<Offset:0x0c> 主模式DMA, byte间delay值配置寄存器，范围0～127。设置为0，表示byte间没有停顿，数据连续发送；设置为7，1byte发送完后，要间隔7T（T为SPI clock周期）再发送下byte数据。加该寄存器的原因是，有客户处理速度跟不上我们发送速度。*/

    __IO unsigned char Reserved2[3];/*保留*/

    __IO unsigned char rNOR_TX_DATA;
        /*<Offset:0x10>   Normal data that will be transmitted. (非DMA模式)
        主模式下，非DMA发送数据寄存器。在主模式下，非DMA模式，对该寄存器写操作，将自动启动SPI传输，并将写入数据发出。查询NOR_TX_DATA_FULL判断数据发送是否完成。*/


    __IO unsigned char Reserved3[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __I unsigned char bVAL:1 ;
                /*主模式下，非DMA发送数据完成查询标记
                1: NOR_TX_DATA fifo is full; 数据未发送完，不能写入新数据
                0: NOR_TX_DATA fifo is not full, current TX data transfer completed.数据发送完成，
                可继续写入新的发送数据。*/

            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rNOR_TX_DATA_FULL;/*<Offset:0x14> */

    __IO unsigned char Reserved4[3];/*保留*/

    __I unsigned char rNOR_RX_DATA;
        /*<Offset:0x18>  NOR_RX_DATA [7:0]:  （只读）  data received
        主模式下，非DMA模式，接收到数据存放buffer。
        从模式下，非DMA模式下，接收到数据存放buffer*/


    __IO unsigned char Reserved5[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char NOR_BYTE_DONE_STS:1 ;
                /*NOR_BYTE_DONE_STS: SPI byte trans done flag in normal mode. When one byte is received,  
                the NOR_BYTE_DONE_STS bit is set and an Interrupt is generated if SLAVE_RXRCV_SPIE = 1, 
                This bit is cleared software. 硬件置1，软件清零*/

            __IO unsigned char DMA_DONE_STS:1 ;/*DMA_DONE_STS: SPI DMA data trans done. when SPI DMA trans done ,DMA_DONE_STS is setted 1’b1 and an Interrupt is generated if SLAVE_RXCS_STS = 1. This bit is cleared software. 硬件置1，软件清零*/
            __IO unsigned char SLAVE_RXCS_STS:1 ;/*SLAVE_RXCS_STS: SPI /CS pull high flag in slave mode. When at posedge /CS, the SLAVE_ RXCS_STS bit is set and an Interrupt is generated if SLAVE_RXCS_STS = 1,This bit is cleared software. 硬件置1，软件清零*/
            __IO unsigned char NOR_BYTE_DONE_INT_EN:1 ;
                /*NOR_BYTE_DONE_INT_EN: SPI NORMAL trans done interrupt enable
                     1: enable normal data trans done interrupt in master and slave mode.  0:disable normal data trans done interrupt.*/

            __IO unsigned char DMA_DONE_INT_EN:1 ;
                /*DMA_DONE_INT_EN (bit4): SPI DMA data trans done interrupt enable. 
                1: enable DMA data trans done interrupt in master and slave mode.  0:disable DMA data trans done interrupt.*/

            __IO unsigned char SLAVE_RXCS_INT_EN:1 ;
                /*SLAVE_RXCS_INT_EN (bit5) :  SLAVE SPI /CS  interrupt enable. 
                1: enable /CS interrupt in slave mode.  0: disable /CS interrupt in slave mode*/

            __IO unsigned char Reserved:2 ;/*保留*/
        }bits;
    }rINT_IF;/*<Offset:0x1c> */

    __IO unsigned char Reserved6[3];/*保留*/

    __IO unsigned long rDMA_ADDR_TX;/*<Offset:0x20>  注：DMA模式下，RAM操作以8bytes为单位；地址需8byte对齐。举例：若写入DMA_ADDR_TX[15:0] = 0x8001，数据将从0x8000读取，而非0x8001。*/

    __IO unsigned long rDMA_ADDR_RX;/*<Offset:0x24>    注：DMA模式下，RAM操作以8bytes为单位；地址需8byte对齐。举例：若写入DMA_ADDR_RX[15:0] = 0x8001，数据将写入0x8000，而非0x8001。*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:13 ;/*N/A*/
            __IO unsigned short Reserved:3 ;/*保留*/
        }bits;
    }rDMA_LEN;/*<Offset:0x28> DMA发送长度，以byte为单位。支持DMA最大长度8K。*/

    __IO unsigned char Reserved7[2];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;
                /*Firmware set 1，means DMA transfer start;
                Hardware clear, means DMA transfer completed.
                置1启动DMA；DMA完成，硬件将该标记位复位；通过查询该标记位判断DMA是否完成*/

            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rDMA_MST_START;/*<Offset:0x2c> */

} SG_SPI_TypeDef, *pSG_SPI_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
