#ifndef     __HAL_SATA_H__    
#define     __HAL_SATA_H__
#ifdef __cplusplus
extern "C" {
#endif
//#include "ApCommon.h"

	//void HAL_SATA_CheckPassCRCIndex(void);

	U32 HAL_Sata_GetPassCRCIndex(void);

	void HAL_Sata_SubPassCRCIndex(U32 dNeedSubSctCnt);

	void HAL_Sata_SetZeroPassCRCIndex(void);

	U08 HAL_Sata_GetCRCFailFlag(void);

	void HAL_Sata_ClrCRCFailFlag(void);

	void HAL_Sata_WaitCRCState(U32 dNeedCheckDma1Index);

#ifdef __cplusplus
	extern }
#endif

#endif
