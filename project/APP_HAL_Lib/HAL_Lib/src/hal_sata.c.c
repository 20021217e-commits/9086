
#include "hal_config.h"
#include "hal.h"
#include "debug.h"
#include "uitility.h"
#include "hal_sata.h"

U8 HAL_GetSataErrInfo(void)
{
	return  0;
}

U8 HAL_ClrSataErrInfo(void)
{
	return 0;
}

void HAL_SataSetNCQTag(T_NCQ_NUM *ptNCQ)
{
    U8 i;

//#if (FUNC_ENABLE == FDL_EXCEPTION_DETECTIVE_ON)
    if (ptNCQ->m_dTabCnt == 0)
    {
        //FDL_Err("NCQ command count is zero");
    }
//#endif

    SG_DSATA->rNCQ_REORDER_EN.bits.EN_REORDER = FUNC_DISABLE;
    SG_DSATA->rNCQ_REORDER_TOTAL.dAll         = 0;//写操作会触发启动，所以要在使能之前清除
    SG_DSATA->rNCQ_REORDER_EN.bits.EN_NORDY   = FUNC_DISABLE;
    SG_DSATA->rNCQ_REORDER_EN.bits.EN_REORDER = FUNC_ENABLE;//初始化操作要在使能操作之前

    for (i = 0 ;i < ptNCQ->m_dTabCnt; i++)
    {
        SG_DSATA->rNCQ_REORDER_REG[i] = ptNCQ->m_bNCQTag[i];
    }

    SG_DSATA->rNCQ_REORDER_NOTREADY.bits.dVAL = FUNC_ENABLE;
    SG_DSATA->rNCQ_REORDER_TOTAL.bits.dVAL = ptNCQ->m_dTabCnt;
    SG_DSATA->rNCQ_CMD_ST.bits.dVAL = FUNC_ENABLE;
}

U32 HAL_Sata_GetPassCRCIndex(void)
{
	return SG_DSATA->rCRC_PASS_INDEX;
}
void HAL_Sata_SubPassCRCIndex(U32 dNeedSubSctCnt)
{
	SG_DSATA->rCRC_PASS_MIN = dNeedSubSctCnt;
	__nds32__dsb();  //Wait CRC_PASS_MIN Write  success
}
void HAL_Sata_SetZeroPassCRCIndex(void)
{
	U32 dTempPassCRCIdx  = 0;
	dTempPassCRCIdx = SG_DSATA->rCRC_PASS_INDEX;
	SG_DSATA->rCRC_PASS_MIN = dTempPassCRCIdx;
}
U08 HAL_Sata_GetCRCFailFlag(void)
{
	if(TRUE == SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL)
	{
		return TRUE;
	}
	return  FALSE;
}
void HAL_Sata_ClrCRCFailFlag(void)
{
	SG_DSATA->rSATA_FAIL_FLAG.bits.dVAL = 0;
}
void HAL_Sata_WaitCRCState(U32 dNeedCheckDma1Index)
{
  while (1)
  {
    if (HAL_Sata_GetPassCRCIndex() >= dNeedCheckDma1Index)
    {
      break;
    }
    else if(TRUE == HAL_Sata_GetCRCFailFlag())
    {
      break;
    }
  }
}
