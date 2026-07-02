#include "fdl_exception.h"

void ExceptionEraseBlock(U16 wOfstInTask, U8 bEntranceType, U8 bErrType)
{
    U16 wIndex;
    U32 dBuffAddr = tFtlTask.tTaskCtrl->dEraseExBuffAddr;
	U16 wErrCntMax = (tFtlTask.tTaskCtrl->wEraseExBuffSize-sizeof(T_ERASE_EXCEPTION))/sizeof(T_ERR_INFO);

	T_ERASE_EXCEPTION *tEraseEx = (T_ERASE_EXCEPTION *)dBuffAddr;
    T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_ERASE_EXCEPTION) + sizeof(T_ERR_INFO) * tEraseEx->wLen);

	if(tEraseEx->wLen >= wErrCntMax)
	{
		FDL_Err("EErrBuffFull");
		return;
	}

	//FDL_Log("ex0=%x ex1=%x ex2=%x",wOfstInTask,bEntranceType,bErrType);

    T_ERR_INFO *tInfoTmp = (T_ERR_INFO *)(dBuffAddr + sizeof(T_READ_EXCEPTION));
    for (wIndex = 0; wIndex < tEraseEx->wLen; wIndex++,tInfoTmp++)
    {
        if (tInfoTmp->wIndex == wOfstInTask)
        {
            FDL_Err("Erase exception is exist");
            return;
        }
    }

    tInfo->wIndex = wOfstInTask;

#ifdef __SIMPLIFY_CODE__ //目前擦除只有这种fail
    tInfo->ErrType = (0x01<<ERS_FAIL);
#else
    if (ENTRANCE_CQ == bEntranceType)
    {
        tInfo->ErrType = (0x01<<ERS_FAIL);
    }


    if (ENTRANCE_VDT == bEntranceType)
    {
        tInfo->ErrType = (0x01<<ERS_ERR_VDT);
    }
#endif
    tEraseEx->bDescript |= tInfo->ErrType;

	//FDL_Log("ErrType:%x Descript:%x",tInfo->ErrType,tEraseEx->bDescript);
	
    tEraseEx->wLen++;

}

void ExceptionReadClstToBuff(U16 wOfstInTask, U8 bEntranceType, U8 bErrType)
{
	U16 wIndex;
	U32 dBuffAddr = tFtlTask.tTaskCtrl->dReadExBuffAddr;
	U16 wErrCntMax = (tFtlTask.tTaskCtrl->wReadExBuffSize-sizeof(T_READ_EXCEPTION))/sizeof(T_ERR_INFO);

	T_READ_EXCEPTION *tReadEx = (T_READ_EXCEPTION *)dBuffAddr;
	T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_READ_EXCEPTION) + sizeof(T_ERR_INFO) * tReadEx->wLen);
#ifdef __VER_TLC_PAGE__
	tReadEx->bDescript |= bErrType;
#endif		
	if(tReadEx->wLen >= wErrCntMax)
	{
		//PRINT_TXT("RErr_Full");
		return;
	}

	//FDL_Log("ex0=%x ex1=%x ex2=%x",wOfstInTask,bEntranceType,bErrType);
	
	T_ERR_INFO *tInfoTmp = (T_ERR_INFO *)(dBuffAddr + sizeof(T_READ_EXCEPTION));
	for (wIndex = 0; wIndex < tReadEx->wLen; wIndex++,tInfoTmp++)
	{
		if (tInfoTmp->wIndex == wOfstInTask)
		{
			//FDL_Err("Read exception is exist");
			#ifdef __VER_TLC_PAGE__
			tInfoTmp->ErrType |= bErrType;
			#endif	
			tInfoTmp->wIndex = wOfstInTask;
			return;
		}
	}

	tInfo->wIndex		= wOfstInTask;
	tInfo->ErrType		= bErrType; 
	tReadEx->bDescript |= bErrType;

	//FDL_Log("ErrType:%x Descript:%x",tInfo->ErrType,tReadEx->bDescript);
	
	tReadEx->wLen++;
	
#if 0//def __VER_TLC_PAGE__
	if( (tReadEx->wLen > VER_TLC_FAIL_STOP_THRES) &&
		(tFtlTask.dTaskType == TYPE_VERIFY_TLC_PAGE))
	{
		//FDL_StopTaskForce();
		//SG_CPM->rWDT0_THRESHOLD = 0x0FFFFFF;/* 2x0.6s = 1.2s*/
		//VER_TLC_FAIL_STOP_THRES = 0xFFFF;
		FDL_Err("Fail To Much To Stop Task!-%d",tReadEx->wLen);
		
		/*Fail太多,强制结束任务*/
	}
#endif

	
}





void ExceptionPreRead4K(U16 wOfstInTask, U8 bEntranceType, U8 bErrType)
{
	U16 wIndex;
    U32 dBuffAddr = tFtlTask.tTaskCtrl->dReadExBuffAddr;
	U16 wErrCntMax = (tFtlTask.tTaskCtrl->wReadExBuffSize-sizeof(T_READ_EXCEPTION))/sizeof(T_ERR_INFO);

	T_READ_EXCEPTION *tReadEx = (T_READ_EXCEPTION *)dBuffAddr;
    T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_READ_EXCEPTION) + sizeof(T_ERR_INFO) * tReadEx->wLen);

	if(tReadEx->wLen >= wErrCntMax)
	{
		FDL_Err("RErrBuffFull");
		return;
	}

	//FDL_Log("ex0=%x ex1=%x ex2=%x",wOfstInTask,bEntranceType,bErrType);
    
    T_ERR_INFO *tInfoTmp = (T_ERR_INFO *)(dBuffAddr + sizeof(T_READ_EXCEPTION));
    for (wIndex = 0; wIndex < tReadEx->wLen; wIndex++,tInfoTmp++)
    {
        if (tInfoTmp->wIndex == (wOfstInTask + tFtlTask.bArgsArr[50]))
        {
            FDL_Err("Read exception is exist");
            tInfoTmp->wIndex = (wOfstInTask + tFtlTask.bArgsArr[50]);
            return;
        }
    }
	

    tInfo->wIndex 		= (wOfstInTask + tFtlTask.bArgsArr[50]);
    tInfo->ErrType 		= bErrType; 
	tReadEx->bDescript |= bErrType;

	//FDL_Log("ErrType:%x Descript:%x",tInfo->ErrType,tReadEx->bDescript);
	
    tReadEx->wLen++;

}


void ExceptionWritePageToFlash(U16 wOfstInTask, U8 bEntranceType, U8 bErrType)
{
#ifdef __SLC_PRG_FAIL_HANDL__
        U16 wIndex;
        U32 dBuffAddr = tFtlTask.tTaskCtrl->dWriteExBuffAddr;
        U16 wErrCntMax = (tFtlTask.tTaskCtrl->wWriteExBuffSize-sizeof(T_WRITE_EXCEPTION))/sizeof(T_ERR_INFO);
    
        T_WRITE_EXCEPTION *tWriteEx = (T_WRITE_EXCEPTION *)dBuffAddr;
        T_ERR_INFO *tInfo = (T_ERR_INFO *)(dBuffAddr + sizeof(T_WRITE_EXCEPTION) + sizeof(T_ERR_INFO) * tWriteEx->wExLen);
        //FDL_Log("write ex0=%x ex1=%x ex2=%x",wOfstInTask,bEntranceType,bErrType);
    
        if(tWriteEx->wExLen >= wErrCntMax)
        {
            FDL_Err("WErrBuffFull");
            return;
        }
        
        T_ERR_INFO *tInfoTmp = (T_ERR_INFO *)(dBuffAddr + sizeof(T_WRITE_EXCEPTION));
        for (wIndex = 0; wIndex < tWriteEx->wExLen; wIndex++,tInfoTmp++)
        {
            if (tInfoTmp->wIndex == wOfstInTask)
            {
                FDL_Err("Write exception is exist");
                return;
            }
        }
    
        tInfo->wIndex = wOfstInTask;
        tInfo->ErrType = 0;
    
    
        if (ENTRANCE_CQ == bEntranceType)
        {
            tInfo->ErrType = (0x01<<WR_PROG_FAIL);
        }
        else if (ENTRANCE_VDT == bEntranceType)
        {
            tInfo->ErrType = (0x01<<WR_ERR_VDT);
        }
    
        if (WR_DATA_OVERLAP == bErrType)/*是否有数据覆盖*/
        {
            FDL_Log("WR_DATA_OVERLAP[0x%X]",tInfo->ErrType);
            tInfo->ErrType |= (0x01<<WR_DATA_OVERLAP);
            tWriteEx->bOverlapCnt++;
        }
    
        tWriteEx->bDescript |= tInfo->ErrType;  
        tWriteEx->dHostDateLen = HAL_ADMGetDMA1Index(); 
        
        //FDL_Log("ErrType:%x Descript:%x",tInfo->ErrType,tWriteEx->bDescript);
    
        tWriteEx->wExLen++;
#endif

}

void ExceptionCopyBack(U16 wOfstInTask, U8 bDir, U8 bErrType)
{
    if(bDir == DMA2_RD_TO_BUFF)//记录读异常信息
    {
        ExceptionReadClstToBuff(wOfstInTask,ENTRANCE_CQ,bErrType);
    }
    else //记录写异常信息
    {
    	//需要增加判断，根据bErrType去配置bEntranceType（待实现）
        ExceptionWritePageToFlash(wOfstInTask,ENTRANCE_CQ,bErrType);
    }
}
void ExceptionMergeBuffClstToFlash(U16 wOfstInTask, U8 bEntranceType, U8 bErrType)
{
}
#ifdef _TLC_ORDER_PROGRAM_
void ExceptionOrderPrgUnit(U16 wOfstInTask, U8 bEntranceType, U8 bErrType)
{
}
#endif
