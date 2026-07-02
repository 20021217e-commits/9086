/*
 * Global.h
 *
 *  Created on: Jun 1, 2016
 *      Author: Administrator
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "common.h"

#define	 UDP_MIX_ADDR					   (0x003E0800)
#define  DMA_BUFFER_SIZE                   (64U*1024U)
#define  DMA_READ_BUFF_SIZE				   (16U*1024U)
#define  DEV_DMA_INFO_BUFF_SIZE  		   (4U*1024U)
#define  PMCP_BUFF_SIZE					   (1024)
//#define  DEBUG_OUT_BUFFER_SIZE 			   (512U)
#define	 MEM_UART_PAGE_SIZE				   (17*1024)


typedef union _DMA_BUFFER
{
    U8  bArray[DMA_BUFFER_SIZE];
    U16 wArray[DMA_BUFFER_SIZE/2];
    U32 dArray[DMA_BUFFER_SIZE/4];
}T_DMA_BUFFER,*pT_DMA_BUFFER;

extern	T_DMA_BUFFER tDmaBuffer;

#define DMA_BUFFER_BASE_ADDR     ((U32)(&tDmaBuffer.bArray[0]))

#define DMA_BUFF_ADDR		     DMA_BUFFER_BASE_ADDR

#endif 
