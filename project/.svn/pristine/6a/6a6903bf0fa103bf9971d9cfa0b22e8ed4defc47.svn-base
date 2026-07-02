#ifndef     __FDL_WRITE_FAIL_HANDLE_H__    
#define     __FDL_WRITE_FAIL_HANDLE_H__

#include "Hal.h"
#include "Fdl_common.h"
#include "Fdl_task_op.h"
#include "fdl_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADDR_IN_PLN0 0
#define ADDR_IN_PLN1 1
#define ADDR_IN_PLN2 2
#define ADDR_IN_PLN3 3

#define LUN_STAT_PAGE_PRO_FAIL_CUR          (1<<0)
#define LUN_STAT_PAGE_PRO_FAIL_PRE          (1<<1)

#define CACHE_CUR_PAGE                      0
#define CACHE_PRE_PAGE                      1
#define CACHE_PAGE_FAIL_CNT                 2

U08 FDL_JudgeAddrOfstInPln(U32 dAddr);
U16 Fdl_SearchOfstByAddr(U8 bFailChan,U8 bFailLogLun,U32 dAddr,U16 wOfst,U08 bMode);

void Fdl_WriteFailHandler(U8 bFailChanPhy);
void Fdl_EraseFailHandler(U8 bChanFail);
U32  Fdl_GetWriteFailInfor(U8 bChanFail);


#ifdef __cplusplus
}
#endif

#endif

