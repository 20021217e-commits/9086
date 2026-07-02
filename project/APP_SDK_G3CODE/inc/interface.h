// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  debug.h
//  文件标识：  
//  内容摘要：  调试功能定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月16日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include "sata_typedef.h"

typedef enum  __attribute__ ((__packed__))
{
	E_TASK_HI = 0,
	E_TASK_DI_LOG ,
	E_TASK_DI_DATA ,
	E_TASK_NONE ,
}E_TASK_TYPE;

#ifndef SATALOG_WR
#define SATALOG_WR	0
#endif
#ifndef SATALOG_RD
#define SATALOG_RD	1
#endif

extern E_TASK_TYPE	bTaskType;

void INTF_TaskQueuedProcess (T_TASK_QUEUE_CTX *queue);
void INTF_Di_TaskProcess (T_TASK_QUEUE_CTX *queue);
void INTF_Hi_DataTaskProcess (T_TASK_QUEUE_CTX *queue);
void INTF_Hi_NoneDataTaskProcess (T_TASK_QUEUE_CTX *queue);
void INTF_Di_DataProcess(T_TASK_QUEUE_CTX *queue, U32 dBufAddr);
void Remove_AllTask(U8 bTag,U32 dFisNum);

void Exception_DummyReadWrite(U32 dwTag);
#endif/*_DEBUG_H_*/
