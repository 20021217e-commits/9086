/**************************************************************************
 *文件名称: RDTPValueScan.c
 *模块功能: P值扫描
 *
 *@SG_Kinder(2018年09月18日(周二))
************************************************************************/
#define __FDL_SET_RETRY_CFG

#include "uitility.h"
#include "flash_driver.h"
#include "fdl_task_op.h"
#include "fdl_PValueSet.h"
#include "hal.h"

#ifdef __GRAY_BOX_TEST__


U08 bBestPvaules[11][5];


#if 1
T_PAGE_MODE tPageModeAllBXXX[11] = /*适用于B16A、B17A、B0KB、B27A*/
{

	LP,0xA4,/*SLC_ZONE*/
	UP,0xA0,/*MLC_ZONE*/
	LP,0xA1,/*MLC_ZONE*/
	UP,0xA2,/*MLC_ZONE*/

	XP,0xA5,/*TLC_ZONE*/
	UP,0xA6,/*TLC_ZONE*/
	XP,0xA7,/*TLC_ZONE*/
	LP,0xA8,/*TLC_ZONE*/
	XP,0xA9,/*TLC_ZONE*/
	UP,0xAA,/*TLC_ZONE*/
	XP,0xAB,/*TLC_ZONE*/
};
#else
T_PAGE_MODE tPageModeAllBXXX[7] = /*适用于B16A、B17A、B0KB*/
{
	XP,0xA5,/*TLC_ZONE*/
	XP,0xA7,/*TLC_ZONE*/
	LP,0xA8,/*TLC_ZONE*/
	XP,0xAB,/*TLC_ZONE*/	
	UP,0xA6,/*TLC_ZONE*/
	UP,0xAA,/*TLC_ZONE*/
	XP,0xA9,/*TLC_ZONE*/
};

#endif


T_PAGE_MODE tPageModeAllM16[7] = /*M16*/
{
//	LP,0xA4,/*SLC_ZONE*/
	XP,0xD0,/*TLC_ZONE*/
	UP,0xD1,/*TLC_ZONE*/
	XP,0xD2,/*TLC_ZONE*/
	LP,0xD3,/*TLC_ZONE*/	
	XP,0xD4,/*TLC_ZONE*/
	UP,0xD5,/*TLC_ZONE*/
	XP,0xD6,/*TLC_ZONE*/
};

T_PAGE_MODE tPageModeAll8T23[7] = 
{
	LP,0x04,/*TLC_ZONE*/
	UP,0x05,/*TLC_ZONE*/
	XP,0x06,/*TLC_ZONE*/
	UP,0x07,/*TLC_ZONE*/
	LP,0x08,/*TLC_ZONE*/
	UP,0x09,/*TLC_ZONE*/
	XP,0x0A,/*TLC_ZONE*/
};

U08 *bSubInfo;					/*Feature Addr分段Table*/
U08 (*bFeatureAddrInfoAll)[3];	/*Super Feature Addr Info Table*/
U08 bBestPValueScanBuff[64];	/*P值扫描用到的参数Buff*/
T_PAGE_MODE *tPageModeAll;		/*Flash的页模型指针*/
T_BEST_PVALUE_SCAN tRDTBestPValueScan;/*扫描参数*/
void (*RDT_NFSetFeature)(U08 bChan, U08 bChip, U08 bAddr,U08 bSetMode,U08 bByteCnt, U08 *data);
void (*RDT_NFGetFeature)(U08 bChan, U08 bChip, U08 bAddr, U08 *data);

/**************************************************************************
 *函数名称: RDT_AutoCalFeatureAddrInfo
 *函数功能: 根据Flash的P值模型自动构建扫描所需的表格
 *入口参数:
 * @param tpPageMode 			:Flash的P值模型
 * @param bSupFeatureAddrInfo 	:Super Feature Addr Info Table
 * @param pbSubInfo 			:Feature Addr分段
 * 
 *出口参数:Null
 * @return Null
 *@SG_Kinder(2018年09月18日(周二))
************************************************************************/
void RDT_AutoCalFeatureAddrInfo(T_PAGE_MODE *tpPageMode,U08 (*bSupFeatureAddrInfo)[3],U08 *pbSubInfo)
{
	U08 i,j,k;
	U08 bStatus 		= 0;/**/
	U08 bSharePage 		= 0;/**/
	U08 bCellType 		= 0;/**/
	U08 bPageModeCur 	= 0;/*当前找到符合要求的Word Line Offset在tpPageMode中的偏移*/
	U08 bPageModeCnt 	= 0;/*每个区域(SLC、MLC、TLC)FeatureAddr个数*/
	U08 bWordLineOffset = 0;/*当前需要需寻找的Word Line Offset(LP、UP、XP)*/

	U08 bPageModeMark[FEATURE_ADDR_CNT];/*每找到一个,就标记一个,直到标记完*/
	U08 bFeatureAddrInfoSLC[FEATURE_ADDR_CNT][4];
	U08 bFeatureAddrInfoMLC[FEATURE_ADDR_CNT][4];
	U08 bFeatureAddrInfoTLC[FEATURE_ADDR_CNT][4];
	U08 (*bFeatureAddrInfo)[4];
	T_PAGE_MODE *tpPageModetemp;

	/*按照一定规则将tPageModeAll里面的数据分类*/
	for(k = 0;k < CELL_TYPE_CNT;k++)
	{
		bStatus = 0;
		
		while(bStatus != 0xFF)
		{
			switch(bStatus)
			{
				case 0:/*Status0:关键变量初始化*/
					i = 0;
					j = 0;
					bStatus = 1;
					bPageModeCur 	= 0xFF;
					bWordLineOffset = 0;												
					
					switch(k)/**/
					{
						case SLC_ZONE:
							bPageModeCnt 		= SLC_FEATURE_ADDR_CNT;
							tpPageModetemp 		= &tpPageMode[0];
							bFeatureAddrInfo 	= bFeatureAddrInfoSLC;							
							MemorySet(bFeatureAddrInfoSLC,0x00,sizeof(bFeatureAddrInfoSLC));
							break;
						case MLC_ZONE:
							bPageModeCnt 		= MLC_FEATURE_ADDR_CNT;
							tpPageModetemp 		= &tpPageMode[SLC_FEATURE_ADDR_CNT];
							bFeatureAddrInfo 	= bFeatureAddrInfoMLC;
							MemorySet(bFeatureAddrInfoMLC,0x00,sizeof(bFeatureAddrInfoMLC));
							break;
						case TLC_ZONE:
							bPageModeCnt 		= TLC_FEATURE_ADDR_CNT;
							tpPageModetemp 		= &tpPageMode[SLC_FEATURE_ADDR_CNT + MLC_FEATURE_ADDR_CNT];
							bFeatureAddrInfo 	= bFeatureAddrInfoTLC;
							MemorySet(bFeatureAddrInfoTLC,0x00,sizeof(bFeatureAddrInfoTLC));
							break;
						default:/*不知道什么鬼,直接跳出循环*/
							bStatus = 0xFF;
							break;
					}					
					MemorySet(bPageModeMark,0x00,sizeof(bPageModeMark));
					break;
				
				case 1:/*Status1:在tpPageMode中寻找符合要求的Word Line Offset*/
					for(i = 0;i < bPageModeCnt;i++)
					{
						if((tpPageModetemp[i].bWordLine == bWordLineOffset) && (bPageModeMark[i] == 0x00))
						{
							bPageModeCur = i;
							bPageModeMark[i] = 0xFF;
							break;
						}					
					}
					bStatus = 2;
					break;
					
				case 2:/*Status2:找到后赋值到其他变量*/
					if(bPageModeCur != 0xFF)
					{
						bFeatureAddrInfo[j][0] 		= tpPageModetemp[bPageModeCur].bFeatureAddr;
						bFeatureAddrInfo[j][1] 		= k;
						bFeatureAddrInfo[j++][2] 	= bWordLineOffset;
						bWordLineOffset++;
					}
					else
					{
						bWordLineOffset++;
					}

					if(bWordLineOffset > XP)
					{
						bWordLineOffset = 0;
					}
					bStatus = 3;
					break;
					
				case 3:/*Status3:寻找结束判断*/
					bPageModeCur = 0xFF;
					if(j >= bPageModeCnt)
					{
						bStatus = 0xFF;
					}
					else
					{
						bStatus = 1;
					}
					break;
					
				default:/*不知道什么鬼,直接跳出循环*/
					bStatus = 0xFF;
					break;
			}
		}
	}


	/*按照一定的规则将SLC、MLC、TLC数据融合到bFeatureAddrInfoAll*/
	bStatus = 0;
	MemorySet(bSupFeatureAddrInfo,0x00,sizeof(bSupFeatureAddrInfo));

	while(bStatus != 0xFF)
	{
		switch(bStatus)
		{
			case 0:/*Status0:局部变量初始化*/
				i = 0;
				j = 0;
				k = 0;				
				bStatus 	= 1;
				bSharePage 	= LP;
				bCellType 	= SLC_ZONE;
				pbSubInfo[FEATURE_ADDR_SUB] = FEATURE_ADDR_CNT;
				break;
			case 1:/*Status1:数据融合处理*/
				switch(bCellType)

				{
					case SLC_ZONE:/*SLC*/
						pbSubInfo[k++] = j;/*初始化Feature Addr分段信息*/
						bPageModeCnt = SLC_FEATURE_ADDR_CNT;
					
						for(bSharePage = 0;bSharePage < XP + 1;bSharePage++)
						{
							for(i = 0;i < bPageModeCnt;i++)
							{								
								if( (bCellType	== bFeatureAddrInfoSLC[i][1]) &&
									(bSharePage == bFeatureAddrInfoSLC[i][2]) &&
									(0xFF		!= bFeatureAddrInfoSLC[i][3]))
								{
									bSupFeatureAddrInfo[j][0] 	= bFeatureAddrInfoSLC[i][0];
									bSupFeatureAddrInfo[j][1] 	= bFeatureAddrInfoSLC[i][1];
									bSupFeatureAddrInfo[j++][2] = bFeatureAddrInfoSLC[i][2];
									bFeatureAddrInfoSLC[i][3] 	= 0xFF;
									break;						
								}
							}
						}
						
						bSharePage 	= LP;
						bCellType 	= MLC_ZONE;						
						break;
						
					case MLC_ZONE:/*MLC*/						
						bPageModeCnt = MLC_FEATURE_ADDR_CNT;
						
						for(bSharePage = 0;bSharePage < XP + 1;bSharePage++)
						{
							for(i = 0;i < bPageModeCnt;i++)
							{								
								if( (bCellType	== bFeatureAddrInfoMLC[i][1]) &&
									(bSharePage == bFeatureAddrInfoMLC[i][2]) &&
									(0xFF		!= bFeatureAddrInfoMLC[i][3]))
								{
									bSupFeatureAddrInfo[j][0] 	= bFeatureAddrInfoMLC[i][0];
									bSupFeatureAddrInfo[j][1] 	= bFeatureAddrInfoMLC[i][1];
									bSupFeatureAddrInfo[j++][2] = bFeatureAddrInfoMLC[i][2];
									bFeatureAddrInfoMLC[i][3] 	= 0xFF;
									break;								
								}
							}		
						}
						bSharePage 	= LP;
						bCellType 	= TLC_ZONE;
						break;
						
					case TLC_ZONE:/*TLC*/
						bPageModeCnt = TLC_FEATURE_ADDR_CNT;
						
						for(bSharePage = 0;bSharePage < XP + 1;bSharePage++)
						{
							for(i = 0;i < bPageModeCnt;i++)
							{
								
								if( (bCellType	== bFeatureAddrInfoTLC[i][1]) &&
									(bSharePage == bFeatureAddrInfoTLC[i][2]) &&
									(0xFF		!= bFeatureAddrInfoTLC[i][3]))
								{
									bSupFeatureAddrInfo[j][0] 	= bFeatureAddrInfoTLC[i][0];
									bSupFeatureAddrInfo[j][1] 	= bFeatureAddrInfoTLC[i][1];
									bSupFeatureAddrInfo[j++][2] = bFeatureAddrInfoTLC[i][2];
									bFeatureAddrInfoTLC[i][3] 	= 0xFF;
									break;									
								}
							}		
						}
						bSharePage 	= LP;
						bCellType 	= SLC_ZONE;

						if(j >= FEATURE_ADDR_CNT)/*处理结束判断*/
						{
							bCellType 	= 0xFF;
							bStatus 	= 0xFF;
						}					
						break;

					default:/*不知道什么鬼,直接跳出循环*/
						bCellType 	= 0xFF;
						bStatus 	= 0xFF;
						break;
				}				
				break;

			default:/*不知道什么鬼,直接跳出循环*/
				bCellType 	= 0xFF;
				bStatus 	= 0xFF;
				break;
		}		
	}	
}




/**************************************************************************
 *函数名称: RDT_PValueScanInit
 *函数功能: 根据Flash型号自动初始化P值扫描环境
 *入口参数:
 * @param bPlane		:Plane号
 * 
 *出口参数:Null
 * @return Null
 *@SG_Kinder(2018年09月20日(周四))
************************************************************************/
void RDT_PValueScanInit(void)
{

#if 1
#if (NAND_FLASH == MIC_B16A)
	tPageModeAll = tPageModeAllBXXX;
	RDT_NFSetFeature = HAL_McuSetFeature;
	RDT_NFGetFeature = HAL_McuGetFeature;

	tRDTBestPValueScan.bFeatureAddrSLC 	= 0;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC 	= 0;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC 	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB 	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue		= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/

#elif(NAND_FLASH == MIC_B0KB)
	tPageModeAll = tPageModeAllBXXX;
	RDT_NFSetFeature = HAL_McuSetFeature;
	RDT_NFGetFeature = HAL_McuGetFeature;

#if 0
	tRDTBestPValueScan.bFeatureAddrSLC	= 1;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 3;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
#endif
	tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/

#elif(NAND_FLASH == MIC_M16)
	tPageModeAll = tPageModeAllM16;
	RDT_NFSetFeature = HAL_McuSetFeature;
	RDT_NFGetFeature = HAL_McuGetFeature;
	
	tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/


#elif(NAND_FLASH == TOS_BICS3)
	tPageModeAll = tPageModeAll8T23;
	RDT_NFSetFeature = HAL_McuSetFeature;
	RDT_NFGetFeature = HAL_McuSetFeature;
	
	tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 3;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= 0x00 - 0x16;/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 3;/*区分ED3和其他Flash*/


#elif(NAND_FLASH == MIC_B17A)
	tPageModeAll = tPageModeAllBXXX;
	RDT_NFSetFeature = HAL_McuSetFeature;
	RDT_NFGetFeature = HAL_McuGetFeature;
	
	tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/


#elif(NAND_FLASH == MIC_B27A)

	tPageModeAll = tPageModeAllBXXX;
	RDT_NFSetFeature = HAL_McuSetFeature;
	RDT_NFGetFeature = HAL_McuGetFeature;

#if 1
	tRDTBestPValueScan.bFeatureAddrSLC	= 1;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 3;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
#else
	tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
	tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
	tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
	tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
	tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
	tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
	tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/

#endif



#endif
#else

	if( (tUdp.tRdtOp.wFlashTypeID > Flash_Num_B16A_MIN) && 
				(tUdp.tRdtOp.wFlashTypeID < Flash_Num_B16A_MAX))	/*Flash B16A*/
		{		
			tPageModeAll = tPageModeAllBXXX;
			RDT_NFSetFeature = HAL_McuSetFeature;
			RDT_NFGetFeature = HAL_McuGetFeature;
	
			tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
			tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
			tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
			tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
			tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
			tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
			tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
			
		}
		else if((tUdp.tRdtOp.wFlashTypeID > Flash_Num_B0KB_MIN) && 
				(tUdp.tRdtOp.wFlashTypeID < Flash_Num_B0KB_MAX))	/*Flash B0KB*/
		{
			tPageModeAll = tPageModeAllBXXX;
			RDT_NFSetFeature = HAL_McuSetFeature;
			RDT_NFGetFeature = HAL_McuGetFeature;
	
	#if 0
			tRDTBestPValueScan.bFeatureAddrSLC	= 1;/*Feature Addr SLC*/
			tRDTBestPValueScan.bFeatureAddrMLC	= 3;/*Feature Addr MLC*/
			tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
			tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
			tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
			tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
			tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
	#endif
			tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
			tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
			tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
			tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
			tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
			tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
			tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
	
		}
		else if((tUdp.tRdtOp.wFlashTypeID > Flash_Num_M16_MIN) && 
				(tUdp.tRdtOp.wFlashTypeID < Flash_Num_M16_MAX)) 	/*Flash M16*/
		{
			tPageModeAll = tPageModeAllM16;
			RDT_NFSetFeature = HAL_McuSetFeature;
			RDT_NFGetFeature = HAL_McuGetFeature;
	
			tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
			tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
			tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
			tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
			tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
			tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
			tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
		}
		else if((tUdp.tRdtOp.wFlashTypeID > Flash_Num_8T23_MIN) && 
				(tUdp.tRdtOp.wFlashTypeID < Flash_Num_8T23_MAX))	/*Flash 8T23*/
		{
			tPageModeAll = tPageModeAll8T23;
			RDT_NFSetFeature = HAL_BICS3SetParameter;
			RDT_NFGetFeature = HAL_BICS3GetParameter;
	
			tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
			tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
			tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
			tRDTBestPValueScan.bFeatureAddrSUB	= 3;/*Feature Addr 分段扫描*/
			tRDTBestPValueScan.bStartPValue 	= 0x00 - 0x16;/*扫描的起始Offset*/
			tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
			tRDTBestPValueScan.bTLCPreCmd		= 3;/*区分ED3和其他Flash*/
			
		}
		else if((tUdp.tRdtOp.wFlashTypeID > Flash_Num_B17A_MIN) && 
				(tUdp.tRdtOp.wFlashTypeID < Flash_Num_B17A_MAX))	/*Flash B17A*/
		{
			tPageModeAll = tPageModeAllBXXX;
			RDT_NFSetFeature = HAL_McuSetFeature;
			RDT_NFGetFeature = HAL_McuGetFeature;
	
			tRDTBestPValueScan.bFeatureAddrSLC	= 0;/*Feature Addr SLC*/
			tRDTBestPValueScan.bFeatureAddrMLC	= 0;/*Feature Addr MLC*/
			tRDTBestPValueScan.bFeatureAddrTLC	= 7;/*Feature Addr TLC*/
			tRDTBestPValueScan.bFeatureAddrSUB	= 4;/*Feature Addr 分段扫描*/
			tRDTBestPValueScan.bStartPValue 	= (0x00 - 16);/*扫描的起始Offset*/
			tRDTBestPValueScan.bPValueCnt		= 32;/*扫描的Offset个数*/
			tRDTBestPValueScan.bTLCPreCmd		= 1;/*区分ED3和其他Flash*/
		}


#endif

			
			
	bSubInfo = bBestPValueScanBuff;/*Feature Addr分段Table*/
	bFeatureAddrInfoAll = bSubInfo + tRDTBestPValueScan.bFeatureAddrSUB + 1;/*Super Feature Addr Info Table*/
	RDT_AutoCalFeatureAddrInfo(tPageModeAll,bFeatureAddrInfoAll,bSubInfo);/*计算*/
}


/**************************************************************************
 *函数名称: RDT_BestPValueSet
 *函数功能: P值设置接口
 *入口参数:
 * @param bPlane 		:Plane号
 * 
 *出口参数:Null
 * @return Null
 *@SG_Kinder(2018年09月18日(周二))
************************************************************************/
void RDT_BestPValueSet(U08 bChan, U08 bChip, U08 bPValue)
{
	U08 i = 0;
	U08 bOffsetData[4] 	= {0,0,0,0};
	U08 bFeatureAddr 	= 0;

	RDT_PValueScanInit();

	for(i = 0;i < FEATURE_ADDR_CNT;i++)
	{
		/*指定Feature P值设定*/
		bFeatureAddr 	= bFeatureAddrInfoAll[i][0];
		bOffsetData[0] 	= bPValue;
		RDT_NFSetFeature(bChan, bChip,bFeatureAddr,1,4,bOffsetData);

		bOffsetData[0] 	= 0;
		RDT_NFGetFeature(bChan, bChip,bFeatureAddr,bOffsetData);
		LOG_PRINT("***LP:0x%02X = 0x%02X\n",bFeatureAddr,bOffsetData[0]);
		//if(bOffsetData[0] != bPValue)
		{
			PRINT_HEX("SetF>",bOffsetData[0]);
		}
	}
}


void FDL_GrayBoxRetryOption(void)
{
	U08 bChan,bChip,bPhyChan,bPhyChip;

	if(tFdlRtyOption.bRetryEn)
	{
		for(bChan=0; bChan< CHAN_MAX_CNT;bChan++)
		{
			for(bChip=0; bChip< CHIP_OF_CHAN;bChip++)
			{
				bPhyChan =  CHANS(bChan);
				bPhyChip =	CHIPES(bPhyChan,bChip);
				RDT_BestPValueSet( bPhyChan,  bPhyChip,  tFdlRtyOption.dRetryOffsetVlaue);
			}
		}
	}
}

#endif
