// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  ata_dco.h
//  文件标识：  
//  内容摘要：  ata 命令相关定义
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年08月19日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 

//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#ifndef _ATA_DCO_H_
#define _ATA_DCO_H_
/******************************包含头文件******************************/
#include "common.h"
/******************************类型和宏定义******************************/

typedef union {
	U16 wData;
	struct{
		U16 bit0:1;
		U16 bit1:1;
		U16 bit2:1;
		U16 bit3:1;
		U16 bit4:1;
		U16 bit5:1;
		U16 bit6:1;
		U16 bit7:1;
		U16 bit8:1;
		U16 bit9:1;
		U16 bit10:1;
		U16 bit11:1;
		U16 bit12:1;
		U16 bit13:1;
		U16 bit14:1;
		U16 bit15:1;
	}b;
}UN_BITS;

/*Device Configuration Overlay (DCO) data structure */
typedef union _UN_DCO_DATA_STRUCTURE
{
    U16   wArray[256];//字访问
    U32  dwArray[128];//双字访问
    U8    bArray[512];//字节访问 
}UN_DCO_DATA_STRUCTURE;
/******************************变量声明******************************/

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

    // *****************************************************************************
    // 函数名称: ATACmd_DcoFeatureSet
    // 函数功能: 实现ATA命令 Device Configuration Overlay
    // 入口参数: NONE
    // 出口参数: 
    // 说    明: Device Configuration Overlay (DCO) 0xB1
    // *****************************************************************************
    void ATACmd_DcoFeatureSet(void);

    void UpdateIdfyFromDcoIdfy(void *pTable);

#ifdef __cplusplus
}
#endif
#endif/*_ATA_DCO_H_*/



