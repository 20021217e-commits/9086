
#ifndef     __FDL_DEBUG_H__
#define     __FDL_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif
	#include "flash_driver.h"
    #include "debug.h"
    #include <stdio.h>
    #include "string.h"
    #include <stdarg.h>

//#define __TEST_LLR_MAX_ECC__
#ifdef __TEST_LLR_MAX_ECC__
#define MEM_LLR_ECC_SRC_DATA     (0x460000)
#define MEM_LLR_ECC_DES_DATA     (0x461000)
#define MEM_LLR_ECC_DMAINFOR     (0x47f000)
#endif

#if (FUNC_ENABLE == FDL_EXCEPTION_DETECTIVE_ON)
    #define FDL_Err(format,...)  LOG_PRINT("FDL ERR:"format"\r\n",##__VA_ARGS__)
#else
    #define FDL_Err(format,...)
#endif


#if (FDL_LOG_ON == 1)
    #define FDL_Log(format,...)  LOG_PRINT("FDL:"format"\r\n", ##__VA_ARGS__)
#else
    #define FDL_Log(format,...)

#endif

void Fdb_OperateStatusExitDetect(void);
void Fdb_PrintfMemBySector(U32 pBuffAddr,U32 dSecCnt);//Mem打印
void Fdb_PrintfPhyAddress(T_FLASH_PHY_ADDR *ptAddr, U16 wTotal);//物理地址打印
void Fdb_PrintfBuffStatus(void);//水位打印
void Fdb_CQDeepTest(U08 bChanPhy,U08 bFlow,U08 *LogString);


#ifdef __cplusplus
}
#endif

#endif






