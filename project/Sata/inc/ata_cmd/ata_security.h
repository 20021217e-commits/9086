// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  ata_security.h
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
#ifndef _ATA_SECURITY_H_
#define _ATA_SECURITY_H_
/******************************包含头文件******************************/
#include "sata_typedef.h"
/******************************类型和宏定义******************************/
typedef enum
{
	T_USER=0,
	T_MASTER
}T_PWDTYPE;

typedef enum
{
	T_NORMAL=0,
	T_ENHANCED
}T_ERASEMODE;

#pragma  pack(1)
typedef union __IDFY_LOG06
{
	U8 bPayload[512];
	struct
	{
		U64 qwHead_Info;
		struct{
			U64 IDENTIFY:16;
			U64 RFU:47;
			U64 S1:1;//set 1
		}tMPI;//Master Password Identifier

		struct{
			U64 ENABLED:1;
			U64 LOCKED:1;
			U64 FROZEN:1;
			U64 COUNT_EX:1;
			U64 ESES:1;//ENHANCED SECURITY ERASE SUPPORTED bit
			U64 MPC:1;//MASTER PASSWORD CAPABILITY bit
			U64 SUP:1;//SECURITY SUPPORTED bit
			U64 RFU:56;
			U64 S1:1;//set 1
		}tSS;//Security Status

		struct{
			U64 ESET:15;//ENHANCED SECURITY ERASE TIME field
			U64 ESETF:1;//ENHANCED SECURITY ERASE TIME field
			U64 RFU:47;
			U64 S1:1;//set 1
		}tTFEU;//Time required for an Enhanced Erase mode SECURITY ERASE UNIT command

		struct{
			U64 NSET:15;//NORMAL SECURITY ERASE TIME field
			U64 NSETF:1;//NORMAL SECURITY ERASE TIME FORMAT bit
			U64 RFU:47;
			U64 S1:1;//set 1
		}tTFNU;//Time required for a Normal Erase mode SECURITY ERASE UNIT command

		U64 qwTCFS;//Trusted Computing feature set
		U64 qwCapabilities;//Security Capabilities
		U64 qwRO[57];
	}Type;
}T_IDFY_LOG06;

typedef struct
{
	U16 wIDENTIFY;
	U8 bPWD[32];
}T_PWD;

typedef struct
{
	T_PWD tPWD[15];
	U8 bCount;//有效ID的索引
	U8 bRFU;
}T_MASTER_PWD;

typedef struct{
	struct
	{
		U16 IDENTIFY:1;/*0=set User password
						1=set Master password*/
		U16 RFU0:7;
		U16 NMPC:1;//NEW MASTER PASSWORD CAPABILITY
		U16 RFU1:7;
	}CW;//Control word

	U8 arrbPWD[32];
	U16 NMPI;//NEW MASTER PASSWORD IDENTIFIER field (see 7.41.2.2)
	U16 wRFU[238];
}T_SSP_DC;//SECURITY SET PASSWORD data content

typedef struct{
	struct
	{
		U16 IDENTIFY:1;/*0=set User password
						1=set Master password*/
		U16 RFU0:15;
	}CW;//Control word
	U8 arrbPWD[32];
	U16 wRFU[239];
}T_SUL_DC;//SECURITY UNLOCK data content

typedef struct{
	struct
	{
		U16 IDENTIFY:1;/*0=set User password
						1=set Master password*/
		U16 ERASE_MODE:1;
		U16 RFU0:14;
	}CW;//Control word
	U8 arrbPWD[32];
	U16 wRFU[239];
}T_SEU_DC;//SECURITY ERASE UNIT data content

#pragma  pack(4)
/******************************变量声明******************************/

/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif

	// *****************************************************************************
	// 函数名称: ATACmd_SecuritySetPassWord
	// 函数功能: 实现ATA命令 SECURITY SET PASSWORD
	// 入口参数: NONE
	// 出口参数:
	// 说    明: SECURITY SET PASSWORD – F1h, PIO Data-Out
	// *****************************************************************************
	void ATACmd_SecuritySetPassWord(void);

	// *****************************************************************************
	// 函数名称: ATACmd_SecurityUnLock
	// 函数功能: 实现ATA命令 SECURITY FREEZE UNLOCK
	// 入口参数: NONE
	// 出口参数:
	// 说    明: SECURITY UNLOCK – F2h, PIO Data-Out
	// *****************************************************************************
	void ATACmd_SecurityUnLock(void);
	// *****************************************************************************
	// 函数名称: ATACmd_SecurityErasePrepare
	// 函数功能: 实现ATA命令 SECURITY  ERASE PREPARE
	// 入口参数: NONE
	// 出口参数:
	// 说    明: SECURITY ERASE PREPARE – F3h, Non-Data
	// *****************************************************************************
	void ATACmd_SecurityErasePrepare(void);

	// *****************************************************************************
	// 函数名称: ATACmd_SecurityEraseUnit
	// 函数功能: 实现ATA命令 SECURITY  ERASE UNIT
	// 入口参数: NONE
	// 出口参数:
	// 说    明: SECURITY ERASE UNIT – F4h, PIO Data-Out
	// *****************************************************************************
	void ATACmd_SecurityEraseUnit(void);

	// *****************************************************************************
	// 函数名称: ATACmd_SecurityDisablePassword
	// 函数功能: 实现ATA命令 SECURITY DISABLE PASSWORD
	// 入口参数: NONE
	// 出口参数:
	// 说    明: SECURITY DISABLE PASSWORD – F6h, PIO Data-Out
	// *****************************************************************************
	void ATACmd_SecurityDisablePassword(void);
    // *****************************************************************************
    // 函数名称: ATACmd_SecurityFreezeLock
    // 函数功能: 实现ATA命令 SECURITY FREEZE LOCK
    // 入口参数: NONE
    // 出口参数: 
    // 说    明: SECURITY FREEZE LOCK - F5h, Non-Data
    // *****************************************************************************
    void ATACmd_SecurityFreezeLock(void);

    void UpdateIDFYLogPage06(CMD_STATUS eStatus,void *ptEvent);
    void security_set_password_F1(U8* ptable);
    void security_disable_password_F6(U8* ptable);
    void security_unlock_F2(U8* ptable);
    void security_erase_unit_F4(U8* ptable);

#ifdef __cplusplus
}
#endif
#endif/*_ATA_SECURITY_H_*/



