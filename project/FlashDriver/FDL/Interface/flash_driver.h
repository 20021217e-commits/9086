
// *****************************************************************************
//  版权所有  (C) 2018,  硅格半导体有限公司
//
//  文件名称：  flash_driver.h
//  文件标识：
//  内容摘要：简要说明flash_driver对外接口函数功能
//  其它说明：详细说明参考《SG9081 FDL接口说明文档.pdf》
//  当前版本：  FDLv6.0     SDKv2.02.01
//  项    目：  SG9081
//  作    者：
//  完成日期：  2018年02月09日
//
//  修改记录:    
//  修改日期：
//  版 本 号：
//  修 改 人：
//  修改内容：
// *****************************************************************************


#ifndef     __FLASH_DRIVER_H__
#define     __FLASH_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//Include Files
//------------------------------------------------------------------------------
//#include "data_type.h"
#include "flash_driver_struct.h"
#include "flash_driver_config.h"


//******************************************************************************
//                         任 务 相 关 接 口
// =============================================================================
// 函数功能: 等待相应Task完成
// 入口参数: 无
// 出口参数: 无
// 说     明: fdl接口功能的实现方式是内部创建了Task队列，所以接口参数中包含FDL_TASK_CTRL的，要通过此接口等Task Done
// =============================================================================
void FDL_WaitTaskDone(void);

// =============================================================================
// 函数功能: 等待相应Task完成和Flash真正的编程Done
// 入口参数: 无
// 出口参数: 无
// 说     明: fdl接口功能的实现方式是内部创建了Task队列，所以接口参数中包含FDL_TASK_CTRL的，要通过此接口等Task Real Done
// =============================================================================
void FDL_WaitTaskRealDone(void);

// =============================================================================
// 函数功能: 继续添加一段任务到正在执行的任务队列中
// 入口参数:
//           wCount:任务个数
//           bEndFlg:是否有后续任务段添加
// 出口参数: 添加是否成功
// 说     明: 暂未支持
// =============================================================================
U8 FDL_ContinueTask(U16 wCount);

//                         任 务 相 关 接 口 End
//******************************************************************************


//******************************************************************************
//                            常 规 接 口
// =============================================================================
// 函数功能: fdl层参数初始化
// 入口参数: 
//           ptFdlParam:fdl初始化的结构体参数
// 出口参数: 无
// 说     明:
// =============================================================================
void FDL_Init(T_FDL_PARAM *ptFdlParam);

// =============================================================================
// 函数功能: reset flash
// 入口参数:
//           bChan:通道编号
//           bChip:片选
// 出口参数: 无
// 说     明: 无
// =============================================================================
void FDL_ResetFlash(U8 bChan, U8 bChip);

// =============================================================================
// 函数功能: FDL_ClrRetryReg
// 入口参数:
//          
//           
// 出口参数: 无
// 说     明: 无
// =============================================================================
void FDL_ClrRetryReg(U08 bLogChan,U08 bLogLun);

// =============================================================================
// 函数功能: FDL_GetCurLocal
// 入口参数:
//           bDir:操作方向
//           
// 出口参数: 无
// 说     明: 无
// =============================================================================
U32 FDL_GetCurLocal(U08 bDir);

// =============================================================================
// 函数功能: FDL_OpDmaBuffIndex,获取或者清除DMA的当前Buff Index
// 入口参数:
//           bChan:逻辑通道号(为0xFF时,表示操作DMA1 Buff Index)
//			 bClear:0-获取,1-清除
//           
// 出口参数: 无
// 说     明: 无
// =============================================================================
U32 FDL_OpDmaBuffIndex(U08 bChan,U08 bClear);


// =============================================================================
// 函数功能: reset lun
// 入口参数:
//           bChan:通道编号
//           bLun:Lun编号
// 出口参数: 无
// 说     明: 无
// =============================================================================
void FDL_ResetLun(U8 bChan, U8 bLun);

// =============================================================================
// 函数功能: 读Flash ID
// 入口参数:
//           dBuffAddr:存放ID数据的buffer
//           bChan:通道编号
//           bLun:Lun编号
// 出口参数: 无
// 说     明:
// =============================================================================
void FDL_ReadFlashID(U32 IDBuffAdd,U8 bChan,U8 bCe);
void FDL_DmaRdClrTimeout(U08 bChan,U32 dBuffAddr,U08 bSecCnt);
void FDL_RemoveDevAllLunDwlsvBlkList(void);
void FDL_GetDevAllLunDwlsvBlkListStat(U08 *pbDevLunStat);
U08  FDL_ShallowEraseEn(U08 bChan,U08 bChip);
// =============================================================================
// 函数功能: 获取ONFI Parameter
// 入口参数: bChan:通道号
//           bChip:片选号
// 出口参数:        
//		 TRUE:/*镁光Flash,可能存在浅擦除问题*/
    
//		 FALSE:/*非镁光Flash,不可能存在浅擦除问题*/
   
// 说     明:  判断是否存在浅擦除问题
// =============================================================================
U08 FDL_ONFIParameterGet(U08 bChan,U08 bChip);

// =============================================================================
// 函数功能: Flash接口模式初始化
// 入口参数: bChan:通道号
//           bChip:片选号
// 出口参数: 无
// 说     明:  实现了例如flash Async 到 toggle2.0模式的切换
// =============================================================================
U08 FDL_FlashIFModeInit(U8 bChan, U8 bChip);

// =============================================================================
// 函数功能: Flash写保护控制
// 入口参数: 
//           bMode: 0-使能写保护;      1-关闭写保护
// 出口参数: 无
// 说     明:  
// =============================================================================
void FDL_FlashWPCtrl(U08 bMode);


// =============================================================================
// 函数功能: Check前一次写操作过所有Lun True Ready状态,保证完全编程完毕
// 入口参数: 无
// 出口参数: 无
// 说     明:  
// =============================================================================
void FDL_CheckWriteFlashEnd(void);

// =============================================================================
// 函数功能: 遍历Check Dev的所有Lun Ready状态
// 入口参数: 无
// 出口参数: 无
// 说     明:  
// =============================================================================
void FDL_CheckMultiLunReady(void);

// =============================================================================
// 函数说明：擦除多个块
// 参数说明：
//           ptTaskCtrl： 任务控制结构体
//			 ptBlockAddr：块号起始位置
//           bBlockCnt:块数
//           bFlashMode:擦除模式,SLC = 0，TLC = 2
// 出口参数: 无    
// 说     明: 
// =============================================================================
void FDL_EraseBlocks(FDL_TASK_CTRL *ptTaskCtrl,T_FLASH_PHY_BLK_ADDR *ptBlockAddr, U16 bBlockCnt, U8 bFlashMode);

//                        常 规 接 口 End
//******************************************************************************


//******************************************************************************
//                       非 联 动 数 据 传 输 接 口
// =============================================================================
// 函数功能: Buff中的小数据写多份到flash
// 入口参数:
//           ptTaskCtrl：任务控制结构体
//           dBuffAddr:  数据所在的ram地址
//           ptPageAddr: 操作的页参数
//           wDataSize:  每一份数据的大小
//           bNumOfCopies:写入的份数
//			 pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
// 出口参数: 无
// 说     明: 小数据长度一般小于512Byte，不带UserData 
// =============================================================================
void  FDL_WriteLittleDataToFlash(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_FLASH_PHY_ADDR const *ptPageAddr, U16 wDataSize, U8 bNumOfCopies,U16 *pwSeedIndex);


// =============================================================================
// 函数功能: 从flash读取小数据到buffer
// 入口参数:
//           ptTaskCtrl：任务控制结构体
//           dBuffAddr:读取数据写入的ram地址
//           ptPageAddr:操作的页参数
//           bDataSize:每一份数据的大小
//           bIdxOfCopies:读取的数据是写入份数的序号
//			 pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
// 出口参数: 无
// 说     明: 例如写入份数10，读取份数序号最大是9   
// =============================================================================
void FDL_ReadLittleDataToBuffer(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_FLASH_PHY_ADDR const *ptPageAddr, U16 wDataSize, U8 bIdxOfCopies,U16 *pwSeedIndex);


// =============================================================================
// 函数功能: 写Buff数据到Flash
// 入口参数:
//           ptTaskCtrl：任务控制结构体
//           dBuffAddr:数据所在的ram地址
//           ptPageAddr:起始页地址
//           bPageCnt:页个数
//           dUdBuffAddr:user data存放的地址
//			 pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
// 出口参数: 无
// 说     明:
// =============================================================================
void  FDL_WriteBuffToFlash(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_FLASH_PHY_ADDR const *ptPageAddr, U16 bPageCnt,U32 dUdBuffAddr,U16 *pwSeedIndex);

// =============================================================================
// 函数功能: 快速检验CopyBack搬移到TLC Page的数据
// 入口参数:
//           ptTaskCtrl:	任务控制结构体
//           ptBlockInfo:	块信息所在的ram地址
//           wThres:		停止Task的阈值
//           bLunCnt:		操作的Lun个数
//           dBuffAddr:		DMA Buff起始地址
//           dUdBuffAddr:	user data存放的地址
//			 pwSeedIndex:	外部种子索引，为Null时默认使用物理Row地址
// 出口参数: 无
// 说     明:
// =============================================================================
void FDL_VerifyTLCPage(	FDL_TASK_CTRL *ptTaskCtrl,
						T_BLK_INFO_PER_LUN *ptBlockInfo,
						U16 wThres,
						U08 bLunCnt, 
						U32 dBuffAddr, 					
						U32 dUdBuffAddr,
						U16 *pwSeedIndex);


/**************************************************************************
 *函数名称: FDL_FillSLCBlocks
 *函数功能: 快速填充没写满的Block为随机数
 *			
 *入口参数:
 * @param ptTaskCtrl 	任务控制结构体
 * @param ptBlockInfo 	块信息所在的ram地址
 * @param bLunCnt 		操作的Lun个数
 * @param bDataType 	操作的数据类型
 * @param dBuffAddr 	DMA Buff起始地址
 * @param pwSeedIndex 	外部种子索引，为Null时默认使用物理Row地址
 * 
 *出口参数:Null
 * @return Null
 * @SG_Kinder(2019年04月11日(周四))
************************************************************************/
void FDL_FillSLCBlocks( FDL_TASK_CTRL *ptTaskCtrl,
						T_BLK_INFO_PER_LUN *ptBlockInfo,
						U08 bLunCnt, 
						U08 bDataType,
						U32 dBuffAddr,					
						U16 *pwSeedIndex);



// =============================================================================
// 函数功能: 从flash中读取簇到buffer
// 入口参数:
//           ptTaskCtrl：任务控制结构体
//           dBuffAddr:读取数据写入的ram地址
//           ptClstAddr:起始簇地址
//           wClstLen:簇个数
//           dUdBuffAddr:user data存放的地址
//			 pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
// 出口参数: 无
// 说     明: 操作单位为簇
// =============================================================================
void FDL_ReadClstToBuff(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_FLASH_PHY_ADDR const *ptClstAddr, U16 wClstLen, U32 dUdBuffAddr,U16 *pwSeedIndex);

// =============================================================================
// 函数功能: 从flash中读取簇到buffer中，带Mask功能
// 入口参数:
//           ptTaskCtrl：任务控制结构体
//           dBuffAddr:读取数据写入的ram地址
//           ptClstAddr:只向簇地址的指针
//           wClstLen:簇个数
//           dUdBuffAddr:user data存放的地址
//           pMskBuff:Mask buffer地址，Mask值所占Byte数由初始化指定
//			 pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
// 出口参数: 无
// 说     明: 操作单位为簇
// =============================================================================
void FDL_ReadClstToBuffWithMsk(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_FLASH_PHY_ADDR const *ptClstAddr, U16 wClstLen, U32 dUdBuffAddr, U8 *pMskBuff,U16 *pwSeedIndex);


// =============================================================================
// 函数功能: 从flash中读取表格到buffer中，带离散功能
// 入口参数:
//			dBuffAddr：数据所在的RAM地址
//			pwBuffIdx：分段传输，计算当前每段传输位置相对base地址的Index偏移
//			ptClstAddr：当前操作的Flash的簇地址信息
//			wClstLen：需要读取的簇个数
//			dUdBuffAddr: UserData地址
//			pwSeedIndex：指针指向随机种子的索引
//			bLastChanNum：上一次调用4K预读接口操作的具体通道号bCheckHold：当前是否要进行对自己的CheckHold
//			bCheckHold：当前是否要进行对自己的CheckHold
// 出口参数: 无
// 说     明: 操作单位为簇
// =============================================================================

U08 FDL_PreReadTableToBuff4K( FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, U16 *pwBuffIdx, T_FLASH_PHY_ADDR const *ptClstAddr,								
							  U08 bClstLen, U32 dUdBuffAddr, U16 *pwSeedIndex,U08 bLastChanNum,U08 bCheckHold);



//                       非 联 动 数 据 传 输 接 口 End
//******************************************************************************


//******************************************************************************
//                       联 动 数 据 传 输 接 口

// =============================================================================
// 函数功能: 联动模式将离散的buffer数据按簇顺序，写入到Buff页上。
// 入口参数:
//      dBaseBuffAddr:联动传输的基地址，pwBuffAddr记录是相关此地址的偏移
//      pwBuffAddr：分段传输，计算每段传输位置相对base地址的Index偏移
//      pwSec：分段传输，记录每段传输扇区个数
//      bCnt：分段传输的段数
//      bJointMode:是否关联上水位
// 出口参数:
// 说   明:
// =============================================================================
void FDL_JointWriteHostToDiscreteBuff(U32 dBaseBuffAddr, U16 *pwBuffIdx,U16 *pwSec, U08 bCnt, U08 bJointMode);

// =============================================================================
// 函数功能: 写离散buffer数据到flash
// 入口参数:
//           ptTaskCtrl：任务控制结构体
//           dBaseBuffAddr:Buff地址
//           pwBuffIdx:只向Buff Index的指针
//           bBuffClstCnt:簇个数
//           pPageAddr：写页地址的指针
//           bPageCnt:写的页个数
//           dUdBuffAddr:user data存放的地址
//			 pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
//           bJointMode:联动，非联动模式
// 出口参数: 无
// 说     明:
// =============================================================================
void FDL_WriteDiscreteBuffToFlash (FDL_TASK_CTRL *ptTaskCtrl,U32 dBaseBuffAddr,U16 *pwBuffIdx, U08 bBuffClstCnt,
				                               T_FLASH_PHY_ADDR const *ptPageAddr, U08 bPageCnt,U32 dUdBuffAddr,U16 *pwSeedIndex,U08 bJointMode);


// =============================================================================
// 函数功能: 从flash中读取簇到离散的buffer中，带Mask功能
// 入口参数:
//			ptTaskCtrl：任务控制结构体
//			dBaseBuffAddr:buffer地址
//			pwBuffIdx:只向Buff Index的指针
//			ptClstAddr:只向簇地址的指针
//			wClstLen:簇个数
//			dUdBuffAddr:user data存放的地址
//			pMskBuff:Mask buffer地址，Mask值所占Byte数由初始化指定
//			pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
//			bJointMode:联动，非联动模式
// 出口参数: 无
// 说	  明:	
// =============================================================================
void FDL_ReadClstToDiscreteBuff (FDL_TASK_CTRL *ptTaskCtrl,U32 dBaseBuffAddr,U16 *pwBuffIdx, T_FLASH_PHY_ADDR const *ptClstAddr,
				                 U16 wClstLen, U32 dUdBuffAddr, U08 *pMaskBuff,U16 *pwSeedIndex,U08 bJointMode);

 // =============================================================================
 // 函数功能: 从flash中读取簇到buffer,增加Row地址重排序,以提升性能
 // 入口参数:
 // 		  ptTaskCtrl：任务控制结构体
 // 		  dBuffAddr:读取数据写入的ram地址
 // 		  ptClstAddr:起始簇地址
 // 		  wClstLen:簇个数
 // 		  dUdBuffAddr:user data存放的地址
 // 		  pwSeedIndex:外部种子索引，为Null时默认使用物理Row地址
 //			  pwBuffIdx:BuffIndex,不需要赋值,直接传一片Buff进来即可
 // 出口参数: 无
 // 说 	明: 操作单位为簇
 // =============================================================================
 void FDL_ReadClstWithRowReorder(FDL_TASK_CTRL *ptTaskCtrl,
								 U32 dBaseBuffAddr,
								 T_FLASH_PHY_ADDR const *ptClstAddr,
								 U16 wClstLen,
								 U32 dUdBuffAddr,								 
								 U16 *pwSeedIndex,
								 U16 *pwBuffIdx);

// =============================================================================
// 函数说明：联动多个簇连续读数据
// 参数说明：
//
// 返回值：
// 说   明:
//
// =============================================================================
void FDL_JointReadClsts(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr,T_FLASH_PHY_ADDR const *ptClstAddr,U08 bSecOfstInFstClst, U32 dTotalSec, U16 wClstLen, U32 dUdBuffAddr/*,U8 *pMskBuff*/,U16 *pwSeedIndex);

// =============================================================================
// 函数说明：联动多个簇连续读数据
// 参数说明：
//
// 返回值：
// 说   明:
//
// =============================================================================
void FDL_JointReadClstsRand4K(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr,T_FLASH_PHY_ADDR const *ptClstAddr,U08 bSecOfstInFstClst, U32 dTotalSec, U16 wClstLen, U32 dUdBuffAddr/*,U8 *pMskBuff*/,U16 *pwSeedIndex);

// =============================================================================
// 函数说明：联动4K读，每个LUN最多一个簇
// 参数说明：
//			dBuffAddr：数据所在的RAM地址
//			pwBuffIdx：分段传输，计算当前每段传输位置相对base地址的Index偏移
//			ptClstAddr：当前操作的Flash的簇地址信息
//			wClstLen：需要读取的簇个数
//			dUdBuffAddr: UserData地址
//			pwSeedIndex：指针指向随机种子的索引
//			bLastChanNum：上一次调用4K预读接口操作的具体通道号
//			bCheckHold：当前是否要进行对自己的CheckHold
// 返回值：
// 说   明:
//
// =============================================================================
U08 FDL_JointPreReadClsts4K(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr,U16 *pwBuffIdx,T_FLASH_PHY_ADDR const *ptClstAddr,U08 bClstLen, 
							U32 dUdBuffAddr,U16 *pwSeedIndex,U08 bLastChanNum,U08 bCheckHold);

void FDL_Pre4K_WaitTaskDone(FDL_TASK_CTRL *ptTaskCtrl,U08 bLastChanNum);
// =============================================================================
// 函数说明：联动多个页连续写数据
// 参数说明：
//			ptTaskCtrl:控制参数
//			dBuffAddr:memory地址
//			bSecOffs:起始数据放置位置，相对于dBuffAddr偏移扇区数
//			dTotalSec:此次联动写HOST总共的数据量，以扇区为单位
//			ptPageAddr:flash物理地址参数所在meory位置
//			bPageCnt:总共写的页数
//			dUdBuffAddr:userdata所在meory位置(有厂商称为metadata)
//			pwSeedIndex:random种子所在位置
// 返回值：
// 说   明:	当bSecOffs不为0时，表示在dBuffAddr位置已经放置了bSecOffs个有效扇区数据
//			函数内部会将此些扇区水位置起来，确保此些扇区能联动写入nandflash
// =============================================================================
void FDL_JointWritePages(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, U8 bSecOffs, U32 dTotalSec, T_FLASH_PHY_ADDR  *ptPageAddr, U16 bPageCnt,U32 dUdBuffAddr, U16 *pwSeedIndex);


//                       联 动 数 据 传 输 接 口 End
//******************************************************************************


//******************************************************************************
//                       数 据 搬 移 CopyBack 接 口							
// =============================================================================
// 函数说明：通过外部DMA2操作，将源数据搬移到目标页上
// 入口参数:
//			 ptTaskCtrl：任务控制结构体
//			 ptSrcClst：数据源簇的地址指针
//			 wClstLen:数据源簇个数
//			 ptDstPage:目标页地址指针
//			 wPageCnt:目标页（写页）个数
//			 dSrcUdBuffAddr:读源数据使用UserData Buff
//			 dDstUdBuffAddr:写目标页时UserData的Buff
//			 pwSrcSeedIndex:外部种子索引
//			 pwDstSeedIndex:外部种子索引
// 出口参数: 无
// 说	 明:
// =============================================================================
void FDL_CopyBackProg(FDL_TASK_CTRL *ptTaskCtrl,T_FLASH_PHY_ADDR *ptSrcClst, U16 wClstLen,T_FLASH_PHY_ADDR const *ptDstPage, U16 wPageCnt, U32 dCBBuff,U32 dCBBuffSize,U32 dSrcUdBuffAddr,U32 dDstUdBuffAddr,U16 *pwSrcSeedIndex, U16 *pwDstSeedIndex);


// =============================================================================
// 函数功能: 多个"LUN对"中，源和目的Lun不同，Lun之间数据搬移操作
// 入口参数:
//			 ptTaskCtrl：任务控制结构体
//			 ptSrcInfo: 指外部搬移的源LUN数据簇信息指针。  每个"LUN对"有一个指针信息
//			 ptDstInfo: 指搬移的目的LUN的页信息指针。  每个"LUN对"有一个指针信息
//           bMultiPlnMode:是否开启多Plane
//			 bCnt: 指总的要CopyBack操作的LUN个数。
//			 pwSrcSeedIndex:外部种子索引
//			 pwDstSeedIndex:外部种子索引
// 出口参数: 无
// 说	 明:  该功能要求目的页为SLC页
//			 数据传输中用到的Buffer,使用CopyBack Buff。
// =============================================================================
void FDL_MultiLunExdieCopyBackPrg (FDL_TASK_CTRL *ptTaskCtrl,T_SRC_INFO_OF_LUN *ptSrcInfo, T_DST_INFO_OF_LUN *ptDstInfo,U32 dCBBuff,U32 dCBBuffSize,
											  U08 bMultiPlnMode,U08 bCnt,U16 *pwSrcSeedIndex, U16 *pwDstSeedIndex);


// =============================================================================
// 函数功能: 多个"LUN对"中，源和目的Lun相同，数据搬移操作
// 入口参数:
//		     ptTaskCtrl：任务控制结构体
//		     ptSrcInfo: 指外部搬移的源LUN数据簇信息指针。	每个"LUN对"有一个指针信息
//		     ptDstInfo: 指搬移的目的LUN的页信息指针。  每个"LUN对"有一个指针信息
//		     bMultiPlnMode:是否开启多Plane
//		     bCnt: 指总的要CopyBack操作的LUN个数。
//		     pwSrcSeedIndex:外部种子索引
//		     pwDstSeedIndex:外部种子索引
// 出口参数: 无
// 说     明:  该功能要求目的页为SLC页
//		     数据传输中用到的Buffer,使用CopyBack Buff。
// =============================================================================
void FDL_MultiLunIndieCopyBackPrg (FDL_TASK_CTRL *ptTaskCtrl,T_SRC_INFO_OF_LUN *ptSrcInfo, T_DST_INFO_OF_LUN *ptDstInfo,U32 dCBBuff,U32 dCBBuffSize,
											  U08 bMultiPlnMode,U08 bCnt,U16 *pwSrcSeedIndex, U16 *pwDstSeedIndex);

//					   数 据 搬 移 CopyBack 接 口 		End				  
//******************************************************************************

//******************************************************************************
//					   Order（例TLC） 编 程 接 口 						  
// =============================================================================
// 函数功能: 将Buffer中的数据Order编程到Flash
// 入口参数:
//			 ptTaskCtrl:任务控制参数
//           dBuffAddr:数据所在Buff地址
//           tPlaneParam:Plane操作参数
//           ptOrderPrgUnit：Order编程单位指针
//           dUdBuffAddr：UserData所在地址
//           bCnt：Order编程单位的个数
//
// 出口参数:
// 说     明: 2D，3D MLC, 3D TLC直接Wordline写入模式，都可以使用该接口函数。
//		     这编程过程中有可能会出现最开始或最后的数据包长度不满足multiplane编程，
//		     因此会采用次级的multiplane编程（如3plane编程，2pane编程）或singleplane编程。
//           类似于MLC操作的FDL_WriteBuffToFlash()接口函数，对Flash的页进行编程写入。
// =============================================================================
void FDL_OrderPrgBuffToFlash(FDL_TASK_CTRL *ptTaskCtrl,U32 dBuffAddr, T_PLANE_PARAM * ptPlaneParam, T_FLASH_PROG_ORDER_UNIT *ptOrderPrgUnit, U32 dUdBuffAddr, U16 wCnt, U16 *pwSeedIndex);


// =============================================================================
// 函数功能: 联动将Host的数据Order编程到Flash
// 入口参数:
//			 ptTaskCtrl:任务控制参数
//           tPlaneParam:Plane操作控制参数
//           ptOrderPrgUnit：Order编程单位指针
//           dUdBuffAddr：UserData所在地址
//           bCnt：Order编程单位的个数
// 出口参数: 无
// 说     明: 2D，3D MLC, 3D TLC直接Wordline写入模式，都可以使用该接口函数。
//           类似于MLC操作的FDL_JointWritePages()接口函数，对Flash的页进行编程写入。
// =============================================================================
#ifdef _TLC_ORDER_PROGRAM_
void FDL_JointOrderPrgUnit(FDL_TASK_CTRL *ptTaskCtrl,U32 dDmaBuffAddr,U32 dDma1SecCnt, U32 dUnitBaseAddr, U32 dUdBuffAddr);
#endif
//					   Order（例TLC） 编 程 接 口 			End			  
//******************************************************************************

//******************************************************************************
//					   Order（例TLC） 数 据 搬 移 接 口 						  
// =============================================================================
// 函数功能: 多个LUN进行Copy back编程操作。只是源和目标LUN不同，在Lun之间做对应的Copy back编程，数据经过DAM读出Flash后再写入。
// 入口参数:
//			 ptTaskCtrl:任务控制参数
//           ptSrcInfo: 指外部搬移的源LUN数据簇信息指针。
//           ptDstInfo: 指搬移的目的LUN的页信息指针。
//           bMultiPlnMode:多Plane模式是否开启
//           bCnt: 指总的要CopyBack操作的LUN对个数。
//           dUdBuffAddr：userdata放置buff位置
//           pwSrcSeedIndex:随机种子索引的地址
//           pwDstSeedIndex:目的页随机种子索引地址
// 出口参数: 无
// 说     明:  该功能只适用于普通TLC Flash的Copy back.
// =============================================================================
void FDL_MultiLunExdieCopyBackOrderPrg(FDL_TASK_CTRL *ptTaskCtrl,T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo, T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo,U32 dCBBuff,U32 dCBBuffSize,U08 bMultiPlnMode, U08 bCnt,U16 *pwSrcSeedIndex, U16 *pwDstSeedIndex);

// =============================================================================
// 函数功能: 多个LUN进行Copy back编程操作。只是每个LUN(源、目标LUN是一样的)在Lun内做对应的Copy back编程，数据经过DAM读出Flash后再写入。
// 入口参数:
//			 ptTaskCtrl:任务控制参数
//           ptSrcInfo: 指外部搬移的源LUN数据簇信息指针。
//           ptDstInfo: 指搬移的目的LUN的页信息指针。
//           bMultiPlnMode:多Plane模式是否开启
//           bCnt: 指总的要CopyBack操作的LUN对个数。
//           dUdBuffAddr：userdata放置buff位置
//           pwSrcSeedIndex:随机种子索引的地址
//           pwDstSeedIndex:目的页随机种子索引地址
// 出口参数: 无
// 说     明:  该功能只适用于普通TLC Flash的Copy back.
// =============================================================================
void FDL_MultiLunIndieCopyBackOrderPrg (FDL_TASK_CTRL *ptTaskCtrl,T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo, T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo,U32 dCBBuff,U32 dCBBuffSize,U08 bMultiPlnMode, U08 bCnt, U16 *pwSrcSeedIndex, U16 *pwDstSeedIndex);

// =============================================================================
// 函数功能: 多个LUN内部Copy back编程操作，数据不经过DMA。
// 入口参数:
//			 ptTaskCtrl:任务控制参数
//           ptSrcInfo: 指外部搬移的源LUN数据簇信息指针。
//           ptDstInfo: 指搬移的目的LUN的页信息指针。
//           bMultiPlnMode:多Plane模式是否开启
//           bCnt: 指总的要CopyBack操作的LUN对个数。
//           pwSrcSeedIndex:随机种子索引的地址
// 出口参数: 无
// 说     明:  该功能适用于SLC到TLC的内部CopyBack.
// =============================================================================
void FDL_MultiLunInnerCopyBackOrderPrg(FDL_TASK_CTRL *ptTaskCtrl,T_IN_COPY_SRC_INFO_OF_LUN *ptSrcInfo, T_FLASH_ORDER_PROG_INFO_OF_LUN *ptDstInfo, U32 dCBBuff,U32 dCBBuffSize,U08 bMultiPlnMode,  U08 bCnt, U16 *pwSrcSeedIndex,U16 *pwDstSeedIndex);
#ifdef _TLC_ORDER_PROGRAM_
U08 FDL_ContinueUnitTask( U08 bSlcOrMixMode, U16 wCount);
#endif
#ifdef __cplusplus
}
#endif

#endif






