#ifndef _DMA1C_H_
#define _DMA1C_H_

#ifndef DIR_IN
#define DIR_IN          (1U)
#endif

#ifndef DIR_OUT
#define DIR_OUT         (0U)
#endif

#define DMA1_WR (DIR_OUT)
#define DMA1_RD (DIR_IN)

#define DMA1_JOINT_EN  (1)
#define DMA1_JOINT_DIS (0)

#define DMA1_TOTAL_BUFF_SIZE (1024)//最大512K

void DMAC_DMA1Init(void);
void DMAC_DMA1Start(U8 bDir,U32 dDmaBuffAddr,U16 wSctCnt,U32 dDmaBuffSize,U8 wIndex);
void DMAC_DMA1WaitDone(void);
void DMAC_DMA2WaitAllDone(void);
void DMAC_DMA1JointModeEn(void);
void DMAC_DMA1JointModeDis(void);

#endif
