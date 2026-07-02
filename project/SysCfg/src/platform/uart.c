#include "sg9081.h"
#include "uart.h"
#include "debug.h"

#ifdef __SATA_TEST
#include "forSATAtest.h"
#include "spi.h"
#include "uitility.h"
U32 dPrintBufAddr = (U32)(&tPrintFifo);
#endif

#if(UART_TXMODE == UART_FIFO)
#define UART_FIFO_SIZE_NEW    (512U)

__align(8) U8 UART_TX_FIFO[UART_FIFO_SIZE_NEW];//变量区不够先减少512 byte串口打印
#endif
#ifdef __FW__
void __section(".CODE_INIT") UART_Init(U8 bNum,U32 dBaud,U32 dBufferAddr,U32 dSystemFrequency)
#else
void UART_Init(U8 bNum,U32 dBaud,U32 dBufferAddr,U32 dSystemFrequency)
#endif
{
    SG_UART_TypeDef *pUart;

    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTRX = 0;
    SG_PADCTRL->rPAD_FUNC_CTRL4.bits.URTTX = 0;

    SG_PADCTRL->rUART_CFG.dAll = UART_CFG;
    SG_PADCTRL->rPAD_REU_CTRL.bits.URTTX=1;//打开UART TX 内部强上拉
    pUart=SG_UART(bNum);

    pUart->rDMA_ADDR=dBufferAddr;
    
    pUart->rUART_CTRL.bits.wVAL=1;
    
    pUart->rUART_CFG.bits.UART_RX_EN=0;
    pUart->rUART_CFG.bits.UART_TX_EN=0;

    pUart->rINT_EN.bits.TX_INT_EN=0;
    pUart->rINT_EN.bits.RX_INT_EN=0;
    
    pUart->rBAUD_CTRL.bits.wVAL=dSystemFrequency/dBaud;
    pUart->rTX_GAP.bits.bVAL=0x01;
    
    //pUart->rCFG.bits.PARITY_EN=1;
    //pUart->rCFG.bits.PARITY_SEL=1;
    //pUart->rCFG.bits.RX_EN=1;
    //pUart->rCFG.bits.TX_EN=1;
    pUart->rUART_CFG.wAll=0;

#if(UART_TXMODE == UART_FIFO)
    pUart->rUART_CFG.bits.UART_TX_EN=1;
    pUart->rUART_CFG.bits.FIFO_TX_MODE=1;
    pUart->rDMA_LEN_TX.wAll=UART_FIFO_SIZE_NEW-1;
    pUart->rDMA_ADDR=(U32)UART_TX_FIFO;
#elif(UART_TXMODE == UART_DMA)
    pUart->rUART_CFG.wAll=0x011;
#elif(UART_TXMODE == UART_REG)
#endif
}


void UART_SendMsg(U8 bNum,U8 *pInfo,U16 wLen)
{
    volatile SG_UART_TypeDef *pUart;
    
    if(0 == wLen)
    {
        return;
    }
    
    pUart=SG_UART(bNum);

#if (SIMULATION==1)
    pInfo[wLen] = '\0';
    printf((char*)pInfo);
#else

#if(UART_TXMODE == UART_FIFO)
    U16 i=0;
    //wtite data to fifo

    while(i<wLen)
    {
        //FIFO not ready or write op is busying
        while((pUart->rTX_FIFO_STS.dAll&0x01) == 0);//busy
        while((pUart->rTX_FIFO_STS.dAll&0x0100)==0x0100);//full
        pUart->rTX_FIFO_PORT.dAll=pInfo[i];
        i++;
        __nds32__dsb();
    }

#elif(UART_TXMODE == UART_DMA)
    pUart->rDMA_ADDR=(U32)pInfo;
    pUart->rDMA_LEN_TX.bits.wVAL = wLen;

    while(1 != pUart->rINT_STS.bits.TX_DONE);
    pUart->rINT_STS.bits.TX_DONE=1;
#elif(UART_TXMODE == UART_REG)
#endif

#ifdef __SATA_TEST  //测试SATA稳定性时，保存打印信息至spi中
    if(bSaveFlag)
    {
        if(bClearFlag)
        {
            dPrintBufAddr = (U32)(&tPrintFifo);
            bClearFlag = 0;
        }
        tPrintList.wMsgSize[tPrintList.bTimes] += wLen;
        MemoryCopy((U8 *)dPrintBufAddr, pInfo, wLen);
        dPrintBufAddr += wLen;
    }
#endif
    
#endif 
}

U8 UART_Read(U8 bNum,U8 *pInfo,U16 wLen)
{
    return 0;
}

 U16 UART_BytesToRead(U8 bNum)
{
    return SG_UART(bNum)->rTRANS_CNT.bits.wVAL;
}
