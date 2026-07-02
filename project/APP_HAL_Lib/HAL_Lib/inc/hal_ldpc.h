#include "sg9081.h"
#ifndef HAL_LDPC_H_
#define HAL_LDPC_H_

//LLR数据结构






#define LDPC_MODE5   5
#define LDPC_MODE7   7
#define LDPC_MODE8   8
#define LDPC_MODE9   9
#define LDPC_MODE10  10


void Hal_LDPC_LENCSetMatrix(U32 *pMatrixDataA, U16 wMatrixASize, U32 *pMatrixDataB, U16 wMatrixBSize);
void Hal_LDPC_LDECSetMatrix(U32 *P1, U16 wMatrixP1Size,  U32 *P2, U16 wMatrixP2Size);

void HAL_LDPC_ParityChangeCfg(U08 En,U08 Len);
void HAL_LDPC_ItrMaxCfg(U08 ItrVal);
void HAL_LDPC_Init(U8 bMode, U32 dMemMtxA, U32 dMemMtxB, U32 dMemMtxP1, U32 dMemMtxP2);

#endif
