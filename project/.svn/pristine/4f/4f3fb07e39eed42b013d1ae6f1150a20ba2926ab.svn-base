// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  SATA_fpdma.h
//  文件标识：  
//  内容摘要：  fpdma传输相关定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月17日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef _SATA_FPDMA_H_
#define _SATA_FPDMA_H_
/******************************包含头文件******************************/
#include "sata_typedef.h"
#include "task_manage.h"
/******************************类型和宏定义******************************/
#define WRITE_FPDMA_QUEUED     0x61
#define SEND_FPDMA_QUEUED      0x64

#define READ_FPDMA_QUEUED      0x60
#define RECEIVE_FPDMA_QUEUED   0x65

/******************************变量声明******************************/

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

    // *****************************************************************************
    // 函数名称: SATA_DFPDMAQ_StateMachine
    // 函数功能: 实现SATA设备命令传输协议中的FPDMA QUEUED command protocol 状态机
    // 入口参数: 
    // 出口参数: 
    // 说    明: DFPDMAQ->Device FPDMA Queued
    // *****************************************************************************
    void SATA_DFPDMAQ_StateMachine(void);

    // *****************************************************************************
    // 函数名称: SATA_DFPDMAQ_IntEventHandler
    // 函数功能: 实现SATA设备命令传输协议中的FPDMA QUEUED command protocol 中断处理
    // 入口参数: 
    // 出口参数: 
    // 说    明: DFPDMAQ->Device FPDMA Queued，此函数工作在中断模式
    // *****************************************************************************
    void SATA_DFPDMAQ_IntEventHandler(T_PISR *ptPISR);
    
    // *****************************************************************************
    // 函数名称: SATA_DFPDMAQ_DataTransferReq
    // 函数功能: FPDMA模式下读数据事务处理
    // 入口参数: eType:请求类型
    
    // 出口参数: NONE
    // 说    明:  DFPDMAQ->Device FPDMA Queued
    // *****************************************************************************
    void SATA_DFPDMAQ_DataTransferReq(REQ_TYPE eType,T_HI_TASK *ptTask,TransferCompleteCb_t *pfTransferCompleteCb);

    // *****************************************************************************
    // 函数名称: AddCommandToQueue
    // 函数功能: 子过程
    // 入口参数: 命令层参数检查结果eStatus
    // 出口参数: 
    // 说    明: 
    // *****************************************************************************
    void AddCommandToQueue(CMD_STATUS eStatus);

#ifdef __cplusplus
}
#endif
#endif/*_SATA_FPDMA_H_*/
