#ifndef     __FDL_MCU_OP_H__
#define     __FDL_MCU_OP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "fdl_task_op.h"

void Fdl_MCUPreRead(U08 bChan,U08 bFlashMode);
void Fdl_PullUpCeAndRd(U08 bChan);
void Fdl_MCUMultiPlnReadCmd(U08 bChan,U08 bChip,U08 bLun,U32 dRowAddr);
void Fdl_MCU_MultiPlnCacheReadCmd(U08 bChan,U08 bChip,U08 bLun,U32 dRowAddr);
void Fdl_MCUReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode);
void Fdl_MCUReadOneDma(U8 bChan,U16 wBufIndex, U8 bFlashIdx, U16 wDmaSect, U32 dDmaBuffAddr, U32 dDmaInfoAddr);
void Fdl_MCUSendPreReadCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode);
void Fdl_MCUReadCacheCmd(U8 bChan, U8 bChip, U16 wColAddr);

#ifdef __cplusplus
}
#endif

#endif

