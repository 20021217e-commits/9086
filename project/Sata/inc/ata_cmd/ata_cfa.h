// *****************************************************************************
//  版权所有  (C) 2017,  硅格半导体有限公司
//   
//  文件名称：  ata_cfa.h
//  文件标识：  
//  内容摘要：  ata cfa 命令相关定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2017年06月29日
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 

//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef _ATA_CFA_H_
#define _ATA_CFA_H_
/******************************包含头文件******************************/

/******************************类型和宏定义******************************/

/******************************变量声明******************************/

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

//TODO:ata_cfa关命令接口声明
#if(SATA_DCO_ENABLE == TRUE)
    // *****************************************************************************
    // 函数名称: ATACmd_CfaEraseSectors
    // 函数功能: 实现ATA命令  CFA ERASE SECTORS
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: CFA ERASE SECTORS - C0h, Non-Data
    // *****************************************************************************
    void ATACmd_CFAEraseSectors(void);
    
    // *****************************************************************************
    // 函数名称: ATACmd_CFARequstExtErrCode
    // 函数功能: 实现ATA命令 CFA REQUEST EXTENDED ERROR CODE
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: CFA REQUEST EXTENDED ERROR CODE - 03h, Non-Data
    // *****************************************************************************
    void ATACmd_CFARequstExtErrCode(void);
    
    // *****************************************************************************
    // 函数名称: ATACmd_CFATranslateSector
    // 函数功能: 实现ATA命令 CFA TRANSLATE SECTOR
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明:  CFA TRANSLATE SECTOR - 87h, PIO Data-In
    // *****************************************************************************
    void ATACmd_CFATranslateSector(void);
    
    // *****************************************************************************
    // 函数名称: ATACmd_CFAWriteMultiWOErase
    // 函数功能: 实现ATA命令 CFA WRITE MULTIPLE WITHOUT ERASE
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明: CFA WRITE MULTIPLE WITHOUT ERASE - CDh, PIO Data-Out
    // *****************************************************************************
    void ATACmd_CFAWriteMultiWOErase(void);
    
    // *****************************************************************************
    // 函数名称: ATACmd_CFAWriteSectorsWOErase
    // 函数功能: 实现ATA命令 CFA WRITE SECTORS WITHOUT ERASE
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明:  CFA WRITE SECTORS WITHOUT ERASE - 38h, PIO Data-Out
    // *****************************************************************************
    void ATACmd_CFAWriteSectorsWOErase(void);
    
#endif
#ifdef __cplusplus
}
#endif
#endif/*_ATA_CFA_H_*/



