
#ifndef _HAL_CPM_H_
#define _HAL_CPM_H_

#include "regCPM.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REG_DQS_DELAY_ADDR(bChan)      (SG_CPM_BASE+0x80+bChan*4)
#define REG_ADDLL_CTRL_ADDR(bChan)	   (SG_CPM_BASE+0x140+bChan*0x10)
#define REG_ADDLL_DELAYVAL_ADDR(bChan) (SG_CPM_BASE+0x144+bChan*0x10)
#define REG_ADDLL_STS_ADDR(bChan)      (SG_CPM_BASE+0x148+bChan*0x10)
#define REG_ADDLL_INITVAL_ADDR(bChan)  (SG_CPM_BASE+0x14C+bChan*0x10)

//ADDLL_CTRL
#define BIT_ADJUST_MODE    (8)
#define BIT_LOCK_STAT      (1) 
#define BIT_LOCK_STOP      (2)
#define BIT_UPDATA_VAL     (0)

#ifdef __cplusplus
}
#endif
#endif

