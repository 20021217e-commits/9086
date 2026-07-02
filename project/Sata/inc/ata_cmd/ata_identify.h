// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  ata_identify.h
//  文件标识：  
//  内容摘要：  ata 命令相关定义
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
#ifndef _ATA_IDENTIFY_H_
#define _ATA_IDENTIFY_H_

/******************************包含头文件******************************/
#include "common.h"

/******************************类型和宏定义******************************/
typedef union {//IDENTIFY table

    U16   wArray[256];//字访问
    U32  dwArray[128];//双字访问
    U8    bArray[512];//字节访问

}UN_ATA_INDETIFY_T,*pUN_ATA_INDETIFY_T;

#define W_BIT0   ((U16)0x1)
#define W_BIT1   ((U16)0x2)
#define W_BIT2   ((U16)0x4)
#define W_BIT3   ((U16)0x8)
#define W_BIT4   ((U16)0x10)
#define W_BIT5   ((U16)0x20)
#define W_BIT6   ((U16)0x40)
#define W_BIT7   ((U16)0x80)
#define W_BIT8   ((U16)0x100)
#define W_BIT9   ((U16)0x200)
#define W_BIT10  ((U16)0x400)
#define W_BIT11  ((U16)0x800)
#define W_BIT12  ((U16)0x1000)
#define W_BIT13  ((U16)0x2000)
#define W_BIT14  ((U16)0x4000)
#define W_BIT15  ((U16)0x8000)

/******************************变量声明******************************/ 
    extern UN_ATA_INDETIFY_T unAtaIdentify;
    extern const UN_ATA_INDETIFY_T unAtaIdentify_C;
/******************************接口定义******************************/
#ifdef __cplusplus
extern "C" {
#endif 

    // *****************************************************************************
    // 函数名称: Update_IdfyParam
    // 函数功能: Update_IdfyParam
    // 入口参数: NONE
    // 出口参数: NONE
    // 说    明:
    // *****************************************************************************
    void Update_IdfyParam();
#if 0
    // *****************************************************************************
    // 函数名称: GetBits
    // 函数功能: 获取指定字的一位或者连续多位数据  
    // 入口参数: 
    //             @addr    :字地址
    //             @b_offset:位偏移
    //             @width   :位宽
    // 出口参数: 指定数据区域的值（BCD码）
    // 说    明: 获取指定字的一位或者连续多位数据  
    //             0000001100000100
    //                |width=3|
    //                b_offset=10
    //                val=011=3
    // *****************************************************************************
    int GetBits(const U16* wAddr,U8 bB_offset,U8 bWidth);

    // *****************************************************************************
    // 函数名称: SetBits
    // 函数功能: 设置指定字的一位或者连续多位数据
    // 入口参数: 
    //             @addr    :字地址
    //           @b_offset:位偏移
    //           @width   :位宽
    //           @value   :新值
    // 出口参数: 0        :OK
    //             OTHER  :NG  参数出错
    // 说    明:  
    // *****************************************************************************
    U8 SetBits(U16* wAddr,U8 wB_offset,U8 bWidth,U16 wValue);
    
    // *****************************************************************************
    // 函数名称: UpdateCheckSum
    // 函数功能:
    // 入口参数: 
    //             @bAddr    :缓冲区地址
    //           @wCount   :待校验的字节数，checksum结果保存在最后一个字节
    // 出口参数: NONE
    // 说    明:  
    // *****************************************************************************

    int GetBit(const U16* wAddr,U8 bB_offset);
    void SetBit(U16* wAddr,U8 bB_offset,U8 val);
#endif
    void UpdateCheckSum(U8* bAddr,U16 wCount);
//快速访问宏
    /*Feature Set NCQ support*/
#define FS_NCQ   		    ((unAtaIdentify.wArray[76]&(1<<8))>>8)  //GetBit(unAtaIdentify.wArray+76,8)

    /*Feature Set PACKET support*/
#define FS_PACKET   		((unAtaIdentify.wArray[82]&(1<<4))>>4) //GetBit(unAtaIdentify.wArray+82,4)

    /* Feature Set software settings preservation support*/ 
#define FS_SSP   			((unAtaIdentify.wArray[79]&(1<<6))>>6) //GetBit(unAtaIdentify.wArray+79,6)
    
    /* Feature Set DMA Setup FIS Auto-Activate optimization support*/ 
#define FS_AUTO   			((unAtaIdentify.wArray[79]&(1<<2))>>2) //GetBit(unAtaIdentify.wArray+79,2)
    
    /* Default DRQ data block size */ 
#define DRQ_SIZE   			unAtaIdentify.bArray[94]

#define DCO_HIPM   			((unAtaIdentify.wArray[76]&(1<<9))>>9) //GetBit(unAtaIdentify.wArray+76,9)
#define DCO_DIPM   			((unAtaIdentify.wArray[78]&(1<<3))>>3) //GetBit(unAtaIdentify.wArray+78,3)
#define DCO_AP2S   			((unAtaIdentify.wArray[76]&(1<<14))>>14) //GetBit(unAtaIdentify.wArray+76,14)
#define DCO_48BIT  			((unAtaIdentify.wArray[83]&(1<<10))>>10) //GetBit(unAtaIdentify.wArray+83,10)
#define DCO_SSP    			((unAtaIdentify.wArray[78]&(1<<6))>>6) //GetBit(unAtaIdentify.wArray+78,6)
#define DCO_SMART    		((unAtaIdentify.wArray[82]&(1))) //GetBit(unAtaIdentify.wArray+82,0)
#define DCO_HPA    			((unAtaIdentify.wArray[82]&(1<<10))>>10) //GetBit(unAtaIdentify.wArray+82,10)

#define SEC_ENABLED  		((unAtaIdentify.wArray[128]&(1<<1))>>1) //GetBit(unAtaIdentify.wArray+128,1)
#define SEC_LOCKED    		((unAtaIdentify.wArray[128]&(1<<2))>>2) //GetBit(unAtaIdentify.wArray+128,2)
#define SEC_FROZEN    		((unAtaIdentify.wArray[128]&(1<<3))>>3) //GetBit(unAtaIdentify.wArray+128,3)
#define SEC_MPC     		((unAtaIdentify.wArray[128]&(1<<8))>>8) //GetBit(unAtaIdentify.wArray+128,8)
#define SEC_EXPIRED 		((unAtaIdentify.wArray[128]&(1<<4))>>4) //GetBit(unAtaIdentify.wArray+128,4)
#define SEC_EHERASE  		((unAtaIdentify.wArray[128]&(1<<5))>>5) //GetBit(unAtaIdentify.wArray+128,5)
#define SEC_SUPPORT  		((unAtaIdentify.wArray[128]&(1))) //GetBit(unAtaIdentify.wArray+128,0)
//SANITIZE DEVICE
#define SEC_ANTIFREE  		((unAtaIdentify.wArray[59]&(1<<10))>>10) //GetBit(unAtaIdentify.wArray+59,10)
#define SD_SUPPORT 			((unAtaIdentify.wArray[59]&(1<<12))>>12) //GetBit(unAtaIdentify.wArray+59,12)
#define SD_CRYPTO 			((unAtaIdentify.wArray[59]&(1<<13))>>13) //GetBit(unAtaIdentify.wArray+59,13)
#define SEC_OVERWRITE  		((unAtaIdentify.wArray[59]&(1<<14))>>14) //GetBit(unAtaIdentify.wArray+59,14)
#define SEC_ERASE  			((unAtaIdentify.wArray[59]&(1<<15))>>15) //GetBit(unAtaIdentify.wArray+59,15)

#define GetWord(x) 			unAtaIdentify.wArray[x]

#define MAX_TRIM_ENTRY   	unAtaIdentify.wArray[105]

#ifdef __cplusplus
}
#endif

#endif /*END OF _ATA_IDENTIFY_H_*/
