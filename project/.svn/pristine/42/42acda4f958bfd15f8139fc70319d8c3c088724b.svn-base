#include "flash_driver_config.h"
#include "fdl_soft_decode.h"
#include "fdl_mcu_op.h"
#include "uitility.h"
#include "fdl_common.h"
#include "fdl_task_op.h"
#include "fdl_read_retry.h"

#if (READ_RETRY_ARC_MAX != 0)
U08 bArcStartOffset = 0;
#endif

#if (READ_RETRY_ARC_MAX == 0)
U08 bSoftDecodeBaseRRT[RRT_PARAM_CNT+1] = {0};
#endif

#if((NAND_FLASH == MIC_B0KB) ||\
	(NAND_FLASH == MIC_B16A) ||\ 
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_B27A))
U8_C bLeft_RightOffset_2SB[2] = {0xf7, 0x09};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xf8, 0x08, 0xf0, 0x10};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == MIC_M16)
U8_C bLeft_RightOffset_2SB[2] = {0xfa, 0x06};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfa, 0x06, 0xf4, 0xc};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == MIC_N18)
U8_C bLeft_RightOffset_2SB[2] = {0xf7, 0x09};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xf8, 0x08, 0xf0, 0x10};	//左边减，右边加,4SB，可修改

#elif  (NAND_FLASH == MIC_N28)
U8_C bLeft_RightOffset_2SB[2] = {0xfC, 0x04};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfC, 0x04, 0xf8, 0x08};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == TOS_BICS3)
U8_C bLeft_RightOffset_2SB[2] = {0xfa, 0x06};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfa, 0x06, 0xf4, 0xc};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == HYNIX_8A1M)
U8_C bLeft_RightOffset_2SB[2] = {0xfB, 0x05};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfB, 0x05, 0xf6, 0xA};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == SUMSUNG_3DV4)  || (NAND_FLASH == AFGM) ||  (NAND_FLASH == GCGF)
U8_C bLeft_RightOffset_2SB[2] = {0xf8, 0x08};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xf8, 0x08, 0xf0, 0x10};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == K9ADGD8U0A)
U8_C bLeft_RightOffset_2SB[2] = {0xD0, 0x2F};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xE8, 0x18, 0xD0, 0x2F};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == HYNIX_3DV4)
U8_C bLeft_RightOffset_2SB[2] = {0xfa, 0x06};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfa, 0x06, 0xf4, 0xc};	//左边减，右边加,4SB，可修改

#elif (NAND_FLASH == MIC_B27B)||(NAND_FLASH == MIC_M84C)||(NAND_FLASH == INTEL_B27B)
#if 0/*步长为6*/
U8_C bLeft_RightOffset_2SB[2] = {0xfa, 0x06};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfa, 0x06, 0xf4, 0xc};	//左边减，右边加,4SB，可修改
#else/*步长为8*/
U8_C bLeft_RightOffset_2SB[2] = {0xf8, 0x08};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xf8, 0x08, 0xf0, 0x10};	//左边减，右边加,4SB，可修改
#endif
#elif (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
U8_C bLeft_RightOffset_2SB[2] = {0xfa, 0x06};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfa, 0x06, 0xf4, 0xc};	//左边减，右边加,4SB，可修改
#else////added by flash analys group
U8_C bLeft_RightOffset_2SB[2] = {0xfa, 0x06};				//左边减，右边加,2SB，可修改
U8_C bLeft_RightOffset_4SB[4] = {0xfa, 0x06, 0xf4, 0xc};	//左边减，右边加,4SB，可修改
#endif



/*内部软解码*/
#ifdef __SOFT_INNER_LLR_3BIT__
#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A)|| (NAND_FLASH == MIC_B27B)||(NAND_FLASH == MIC_M84C)||(NAND_FLASH == INTEL_B27B))

	U8_C HAL_b3BitLUT[32] =
	{
		0x7, 0x4, 0x01, 0x1, 0x19, 0x1C, 0x1F, 0x1F
	};
#elif(NAND_FLASH == MIC_M16)
	U8_C HAL_b3BitLUT[32] =
	{
		0x7, 0x4, 0x01, 0x1, 0x19, 0x1C, 0x1F, 0x1F
	};
#elif (NAND_FLASH == MIC_N18)
	U8_C HAL_b3BitLUT[32] =
	{
		0x7, 0x4, 0x01, 0x1, 0x19, 0x1C, 0x1F, 0x1F
	};
#else ///added by flash analys group
	U8_C HAL_b3BitLUT[32] =
	{
		0x0, 0x0, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0
	};
#endif
#endif

/*外部软解码 LLR*/
#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)|| (NAND_FLASH == MIC_B27A))
#ifdef __SOFT_EX_LLR_5BIT__
	U8_C HAL_b5BitLUT[32] =
	{
			0x06, 0x03, 0x02, 0x00, 0x0f, 0x01, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00,
			0x0f, 0x02, 0x1F, 0x00, 0x0f, 0x00, 0x11, 0x1F, 0x00, 0x00, 0x1F, 0x11, 0x00, 0x1E, 0x1C, 0x19,
	};
#endif

#ifdef __SOFT_OR_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
			0x05, 0x02, 0x01, 0xFF, 0x01, 0x00, 0xFF, 0xFA,
		    0x05,0x05,0x04,0x04,0x07,0x07,0x06,0x06,
			0x09,0x09,0x08,0x08,0x0b,0x0b,0x0a,0x0a,
			0x0d,0x0d,0x0c,0x0c,0x0f,0x0f,0x0e,0x0e
	};
#endif

#elif(NAND_FLASH == MIC_N28)
//	LLR5bit={0x07,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1D,0x1D,0x19}
//	LLR3bit={0x07,0x03,0x01,0x01,0x19,0x1D,0x1F,0x1F}
#ifdef __SOFT_EX_LLR_5BIT__
	U8_C HAL_b5BitLUT[32] =
	{
			0x0A,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
			0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1D,0x1D,0x16
	};
#endif

#ifdef __SOFT_OR_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
			0x0A,0x03,0x01,0x01,0xF6,0xFD,0xFF,0xFF
	};
#endif

#ifdef __SOFT_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
//			0x08,0x01,0x01,0x01,0x18,0x1F,0x1F,0x1F
			0x08,0x01,0x01,0x01,0x1F,0x1F,0x1F,0x18
	};
#endif

#elif (NAND_FLASH == MIC_M16)
#ifdef __SOFT_EX_LLR_5BIT__
	U8_C HAL_b5BitLUT[32] =
	{	
			0x07, 0x04, 0x04, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
			0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1c, 0x1c, 0x19
	};
#endif

#ifdef __SOFT_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =  //无效
	{
			0x05, 0x02, 0x01, 0x11, 0x01, 0x00, 0x11, 0x16
	};
#endif
#elif (NAND_FLASH == MIC_B0KB)
#ifdef __SOFT_EX_LLR_5BIT__
	U8_C HAL_b5BitLUT[32] =
	{
			0x07, 0x06, 0x03, 0x00, 0x0f, 0x03, 0x11, 0x00, 0x0f, 0x00, 0x01, 0x00, 0x00, 0x0f, 0x11, 0x11,
			0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x1E, 0x11, 0x00, 0x1D, 0x1B, 0x18,
	};
#endif
	
#ifdef __SOFT_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
			0x06, 0x03, 0x01, 0x11, 0x00, 0x00, 0x1E, 0x19
	};
#endif
	
#elif (NAND_FLASH == TOS_BICS3)
#ifdef __SOFT_OR_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
			0x7,	0x3,	0x3,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,
			0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1F,	0x1D,	0x1D,	0x18,
	};
#endif

#ifdef __SOFT_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
			0, 0, 0, 0, 0, 0, 0, 0
	};
#endif

#elif (NAND_FLASH == HYNIX_8A1M)

#ifdef __SOFT_EX_LLR_5BIT__
	U8_C HAL_b5BitLUT[32] =
	{		
			0x08, 	0x04, 	0x04, 	0x01, 	0x01, 	0x02, 	0x01, 	0x01, 	0x01, 	0x01, 	0x02, 	0x01, 	0x01, 	0x01, 	0x01, 	0x01, 
			0x1F, 	0x1F, 	0x1F, 	0x1F,	0x1F, 	0x1E, 	0x1F, 	0x1F, 	0x1F, 	0x1F, 	0x1E, 	0x1F, 	0x1F, 	0x1C, 	0x1C, 	0x17
	};
#endif

#ifdef __SOFT_EX_LLR_3BIT__
	U8_C HAL_b3BitLUT[32] =
	{
			0x06, 0x02, 0x02, 0x01, 0x1F, 0x1E, 0x1E, 0x19
	};
#endif

#elif (NAND_FLASH == SUMSUNG_3DV4)
#ifdef __SOFT_EX_LLR_5BIT__
U8_C HAL_b5BitLUT[32] =
{
		0x07,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1D,0x1D,0x17
};
#endif

#ifdef __SOFT_OR_EX_LLR_3BIT__//3CW亦或解码
U8_C HAL_b3BitLUT[32] =
{
	0x07,0x03,0x01,0x01,0xF8,0xFD,0xFF,0xFF,
    0x05,0x05,0x04,0x04,0x07,0x07,0x06,0x06,
	0x09,0x09,0x08,0x08,0x0b,0x0b,0x0a,0x0a,
	0x0d,0x0d,0x0c,0x0c,0x0f,0x0f,0x0e,0x0e
};
#endif

#ifdef __SOFT_EX_LLR_3BIT__
U8_C HAL_b3BitLUT[32] =
{
		0x06,0x01,0x01,0x01,0xFF,0xFF,0xFF,0xF9,//修改LLR表，正常
	    0x05,0x05,0x04,0x04,0x07,0x07,0x06,0x06,
		0x09,0x09,0x08,0x08,0x0b,0x0b,0x0a,0x0a,
		0x0d,0x0d,0x0c,0x0c,0x0f,0x0f,0x0e,0x0e
};
#endif
#elif (NAND_FLASH == MIC_B27B)||(NAND_FLASH == MIC_M84C)||(NAND_FLASH == INTEL_B27B)
#ifdef __SOFT_EX_LLR_5BIT__
U8_C HAL_b5BitLUT[32] =
{
		0x06,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1E,0x1E,0x1A
};
#endif

#ifdef __SOFT_OR_EX_LLR_3BIT__
U8_C HAL_b3BitLUT[32] =
{
		0x06,0x02,0x01,0x01,0xFA,0xFE,0xFF,0xFF
};
#endif

#ifdef __SOFT_EX_LLR_3BIT__
U8_C HAL_b3BitLUT[32] =
{
		0x06,0x01,0x01,0x01,0x1F,0x1F,0x1F,0x1A
};
#endif

#elif (NAND_FLASH == YMTC_B08T)||(NAND_FLASH == YMTC_A06M)
#ifdef __SOFT_EX_LLR_5BIT__
U8_C HAL_b5BitLUT[32] =
{
		0x07,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1D,0x1D,0x19
};


#endif

#ifdef __SOFT_EX_LLR_3BIT__
U8_C HAL_b3BitLUT[32] =
{
//		{6,1,1,1,-1,-1,-1,-6}
		0x06,0x01,0x01,0x01,0x1F,0x1F,0x1F,0x1A
};
#endif

#else ///added by flash analys group
	U8_C HAL_b5BitLUT[32] =
	{
		0x0, 0x0, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0
	};

#endif


void  FCL_SoftDecodeGetWLPageAddrFromPageNum(U32 dRowAddr, U16  *pwWordlineNum,  U08 *pbPageOffsInWL, U08 *pbWLPageMode)
{


#if (NAND_FLASH == MIC_B0KB)
	U16 wPageNum = dRowAddr & 0x7FF;
	U16 wTmpPageNum;
	if(wPageNum<16)
	{
		*pwWordlineNum  = wPageNum;
		*pbPageOffsInWL = 0;
	}
	else if(wPageNum<112)
	{
		wTmpPageNum     = wPageNum-16;
		*pwWordlineNum  = (wTmpPageNum>>1) + 16;
		*pbPageOffsInWL = wTmpPageNum&0x1;
	}
	else
	{
		U08 bFlag = 0;
		if ((wPageNum>=112)&&(wPageNum<=1501))
		{
			wTmpPageNum = wPageNum - 112;
			if (0 == (wTmpPageNum%3))
			{
				*pwWordlineNum  = (wTmpPageNum/3) + 32;//MIN_WL_NUM(2);
				*pbPageOffsInWL = 2;
				bFlag = 1;
			}
		}

		if((0==bFlag) && (wPageNum>=113)&&(wPageNum<=1502))
		{
			wTmpPageNum = wPageNum - 113;
			if (0 == (wTmpPageNum%3))
			{
				*pwWordlineNum  = (wTmpPageNum/3) + 64;//MIN_WL_NUM(3);
				*pbPageOffsInWL = 0;
				bFlag = 1;
			}
		}

		if((0==bFlag) && (wPageNum>=114)&&(wPageNum<=1503))
		{
			wTmpPageNum = wPageNum - 114;
			if (0 == (wTmpPageNum%3))
			{
				*pwWordlineNum  = (wTmpPageNum/3) + 64;//MIN_WL_NUM(3);
				*pbPageOffsInWL = 1;
				bFlag = 1;
			}
		}

		if((0==bFlag) && (wPageNum>=1504)&&(wPageNum<=1534))
		{
			wTmpPageNum = wPageNum - 1504;
			if (0 == (wTmpPageNum&0x01))
			{
				*pwWordlineNum  = (wTmpPageNum>>1) + 496;//WL_CNT(4);
				*pbPageOffsInWL = 2;
				bFlag = 1;
			}
		}

		if((0==bFlag) && (wPageNum>=1505)&&(wPageNum<=1535))
		{
			wTmpPageNum = wPageNum - 1505;
			if (0 == (wTmpPageNum&0x01))
			{
				*pwWordlineNum  = (wTmpPageNum>>1) + 528;//WL_CNT(6);
				*pbPageOffsInWL = 0;
				bFlag = 1;
			}
		}
		else 
		{
#ifdef  __DEBUG_INF__
			{LOG_ERROR("B0KB pageoffs to wl error!");}
#endif
		}
	}

	if ((*pwWordlineNum<16) || (*pwWordlineNum>=528))						//只有LP页
	{
		*pbWLPageMode = SLC_PAGE;
	}
	else if (((16<=*pwWordlineNum) && (*pwWordlineNum<32)) || ((512<=*pwWordlineNum) && (*pwWordlineNum<528)))		//有LP和HP页，没有EXTRA页										//只有LP和HP页
	{
		*pbWLPageMode = MLC_PAGE;
	}
	else if ((32<=*pwWordlineNum) && (*pwWordlineNum<512))				//3WL
	{
		*pbWLPageMode = TLC_PAGE;
	}
	LOG_PRINT("B0KB *pwWordlineNum=%d!", *pwWordlineNum);
#endif

#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A))
	U08 bWordLineOffs;	//WL内的偏移
	U16 wTmpPageNum;
	U16 wPrgWLNum = 0;	//WL的编号
	U16 wPageNum = dRowAddr & 0xFFF;

	if((wPageNum>=0)&&(wPageNum<=11))
	{
		wPrgWLNum = wPageNum;
		bWordLineOffs = 0;
	}
	else if((wPageNum>=12)&&(wPageNum<=35))
	{
		wTmpPageNum = wPageNum -12;
		HAL_PMCPDivision(wTmpPageNum,2,&wTmpPageNum,&bWordLineOffs);

		wPrgWLNum = wTmpPageNum + 12;
	}
	else if((wPageNum>=36)&&(wPageNum<=59))
	{
		wTmpPageNum = wPageNum -36;
		wPrgWLNum = wTmpPageNum + 24;
		bWordLineOffs = 0;
	}
	else if((wPageNum>=60)&&(wPageNum<=2221))
	{
		wTmpPageNum = wPageNum -60;
		HAL_PMCPDivision(wTmpPageNum,3,&wTmpPageNum,&bWordLineOffs);
		if(bWordLineOffs == 2)
		{
			wPrgWLNum = wTmpPageNum + 48;
		}
		else
		{
			wPrgWLNum = wTmpPageNum + 24;
		}

		bWordLineOffs = 2-bWordLineOffs;
	}
	else if((wPageNum>=2222)&&(wPageNum<=2267))
	{
		wTmpPageNum = (wPageNum -2222)/2;
		HAL_PMCPDivision(wTmpPageNum,2,&wTmpPageNum,&bWordLineOffs);
		if(bWordLineOffs == 1)
		{
			wPrgWLNum = wTmpPageNum + 745;
		}
		else
		{
			wPrgWLNum = wTmpPageNum + 768;
		}

		if((wPageNum-2222)%4 > 1)
		{
			bWordLineOffs = 4-bWordLineOffs;
		}
	}
	else if((wPageNum>=2268)&&(wPageNum<=2291))
	{
		wTmpPageNum = wPageNum -2268;
		HAL_PMCPDivision(wTmpPageNum,2,&wTmpPageNum,&bWordLineOffs);

		wPrgWLNum = wTmpPageNum + 756;
		bWordLineOffs = 2-bWordLineOffs;
	}
	else if((wPageNum>=2292)&&(wPageNum<=2303))
	{
		wPrgWLNum = 780+(wPageNum-2292);
		bWordLineOffs = 0;
	}

	*pwWordlineNum  = wPrgWLNum;
	*pbPageOffsInWL = bWordLineOffs;

	if ((*pwWordlineNum<12) || (*pwWordlineNum>=780))						//只有LP页
	{
		*pbWLPageMode = SLC_PAGE;
	}
	else if (((12<=*pwWordlineNum) && (*pwWordlineNum<24)) || ((768<=*pwWordlineNum) && (*pwWordlineNum<780)))		//有LP和HP页，没有EXTRA页										//只有LP和HP页
	{
		*pbWLPageMode = MLC_PAGE;
	}
	else if ((24<=*pwWordlineNum) && (*pwWordlineNum<768))				//3WL
	{
		*pbWLPageMode = TLC_PAGE;
	}
#endif
}

void Fdl_MCU_SetFeature_ReadOffs(U8 bChan, U8 bChip, U8 bAddr, U8 *pData)
{
	//U8 i;
	
	HAL_MCUCheckStatus(bChan, bChip);		//需要等RB Done
	HAL_MCU_SetCe(bChan, bChip, 1);
	Delay(4);
	HAL_MCU_SendCmd(bChan, 0xEF);		//SET FEATURE
	HAL_MCU_SendOneAddr(bChan, bAddr);
	Delay(4);
#if 0
	for (i = 0; i < 4; i++)
	{
		HAL_MCUSendDataByDqs(bChan,pData[i]);
	}
#else
	HAL_MCUSendNByteDataByDqs(bChan,pData,4);
#endif
	HAL_MCUCheckStatus(bChan, bChip);
}

void Fdl_SetReadOffset(U8 bChan, U8 bChip, U32 dRowAddr, U8 bFlashMode, U8 *pData)
{
#if 0
	//LOG_PRINT("pData[0]=0x%x\n", pData[0]);

	if (bFlashMode == 0)		// SLC
	{
		Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, (U8)LP_SLC_FEATUREADDR, pData);
	}
	else						// TLC
	{
		U16 wWordLineNum;
		U08 bPageOffsInWl;
		U08 bWLPageMode =  SLC_PAGE;	//SLC,MLC,还是TLC页

		FCL_SoftDecodeGetWLPageAddrFromPageNum(dRowAddr, (U16 *)&wWordLineNum, (U8 *)&bPageOffsInWl, &bWLPageMode);
		if (SLC_PAGE == bWLPageMode)						//只有LP页
		{
			Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, (U8)LP_NOTUPEX_FEATUREADDR, pData);
			//FDL_Log("Only LP,TLC Open Page");
		}
		else if (MLC_PAGE == bWLPageMode)		//有LP和HP页，没有EXTRA页
		{
			if (0==bPageOffsInWl)
			{
				Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, LP_NOTEX_FEATUREADDR, pData);		//LP
				//FDL_Log("TWO PAGE, LP, SLC");
			}
			else
			{
				Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, UP_NOTEX_FEATUREADDR1, pData);		//HP
				Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, UP_NOTEX_FEATUREADDR2, pData);
				//FDL_Log("TWO PAGE, HP, MLC");
			}
		}
		else if (TLC_PAGE == bWLPageMode)		//有LP/HP/XP
		{
			U8 bAddrCnt = 0;
			
			if (0==bPageOffsInWl)
			{
				for (bAddrCnt=0; bAddrCnt<bTLC_VthCnt[0]; bAddrCnt++)	//LP页只有一个阈值电压
				{
					Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bTLC_Offset_FeatureAddr[bTLC_LP_Index[bAddrCnt]], pData);		//TLC Open Page
					//FDL_Log("3WL, LP,TLC Page");
				}
			}
			else if(1==bPageOffsInWl)
			{
				for (bAddrCnt=0; bAddrCnt<bTLC_VthCnt[1]; bAddrCnt++)	//XP页只有两个阈值电压
				{
					Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bTLC_Offset_FeatureAddr[bTLC_HP_Index[bAddrCnt]], pData);			//TLC Open Page
					//FDL_Log("3WL, HP,TLC Page");
				}
			}
			else
			{
				for (bAddrCnt=0; bAddrCnt<bTLC_VthCnt[2]; bAddrCnt++)	//UP页只有四个阈值电压
				{
					Fdl_MCU_SetFeature_ReadOffs(bChan, bChip, bTLC_Offset_FeatureAddr[bTLC_EX_Index[bAddrCnt]], pData);			//TLC Open Page
					//FDL_Log("3WL, EXTRA PAGE,TLC Page");
				}
			}
		}
	}
#endif
}

void Fdl_MCUReadDmaCmd(U8 bChan, U8 bChip, U32 dRowAddr, U16 wColAddr,U08 bFlashMode)
{
	HAL_MCUCheckStatus(bChan, bChip);
	Fdl_MCUPreRead(bChan,bFlashMode);//前缀(0xDA/0xDF 0x01/0x02/0x03)
	HAL_MCU_SendCmd(bChan, FL_CMD_READ1);
	HAL_MCU_SendTwoAddr(bChan, wColAddr);
	HAL_MCU_SendThreeAddr(bChan, dRowAddr);
	HAL_MCU_SendCmd(bChan, FL_CMD_READ2);
	HAL_MCUCheckStatus(bChan, bChip);
	
	HAL_MCU_SendCmd(bChan, FL_CMD_READ1);
	HAL_MCU_SendTwoAddr(bChan, wColAddr);
	HAL_MCU_SendThreeAddr(bChan, dRowAddr);
	HAL_MCU_SendCmd(bChan, FL_CMD_RANDOM_DATA_OUTPUT1);
	HAL_MCU_SendTwoAddr(bChan, wColAddr);
	HAL_MCU_SendCmd(bChan, FL_CMD_RANDOM_DATA_OUTPUT2);
	Delay(10);
}

void Fdl_MCUStartReadDma(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U08 bFlashMode)
{
	Fdl_MCUReadDmaCmd(bChan, bChip, ptDmaInfor->dRowAddr, ptDmaInfor->wColAddr, bFlashMode);
	Joint_Mode JointMode = HAL_ADMGetJointMode(bChan);
	HAL_ADMStartDMA2(bChan, ptDmaInfor->dDmaBuffAddr, ptDmaInfor->dDmaInfoAddr, DMA2_RD_TO_BUFF, 
		ptDmaInfor->dBuffIdx, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma, JointMode);
	
	while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE)
	{
		//LOG_PRINT("SoftDecode,Wait_Buf_Done,bChan=%d\n", bChan);
	}
}

#ifndef __SOFT_INNER_LLR_3BIT__
void Fdl_MCUSoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bFlashMode, U8 bSBNum)
{
	U08 i,j = 0;
	U08 pReadOffset[4] = {0, 0, 0, 0};
	U08 bLeftRightOffset[4];

	if (2 == bSBNum)
	{	
		U08MemCopy((U08*)bLeftRightOffset, (U08*)bLeft_RightOffset_2SB, 2);
	}
	else
	{
		U08MemCopy((U08*)bLeftRightOffset,(U08*)bLeft_RightOffset_4SB, 4);
	}
	
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM = bSBNum;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_EN = 1;
	SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;                      //????????

	HAL_McuSetMask(bChan, ptDmaInfor->bMaskInfo, MASK_BYTE_OF_CLST,0);						  
	HAL_McuSetRndSeed(bChan, ptDmaInfor->dRowAddr, ptDmaInfor->wSeedIdx);
	HAL_McuSetUd(bChan, ptDmaInfor->dUserDataAddr, ptDmaInfor->bSecOfClst, ptDmaInfor->bUdSize, 1);
	Fdl_MCUStartReadDma(bChan, bChip, ptDmaInfor, bFlashMode);
	
#if (READ_RETRY_ARC_MAX != 0)     
	for(i = 0;i<4;i++)
	{
		bLeftRightOffset[i] += bArcStartOffset;
	}

	for (i = 0; i<SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM; i++)
	{
		pReadOffset[0] = bLeftRightOffset[i];
		Retry_SetArcStartOffset(bChan,bChip,pReadOffset);
		Fdl_MCUStartReadDma(bChan, bChip, ptDmaInfor, bFlashMode);
	}
	
	while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
	{
	}
	pReadOffset[0] = bArcStartOffset;
	Retry_SetArcStartOffset(bChan,bChip,pReadOffset);
	
#else

	for (i = 0; i<SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM; i++)
	{

#if ((NAND_FLASH == TOS_BICS3) || (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == YMTC_B08T) || (NAND_FLASH == AFGM) || (NAND_FLASH == HYNIX_8A1M) ||  (NAND_FLASH == GCGF) || (NAND_FLASH == YMTC_A06M))
		U08MemCopy(bSoftDecodeBaseRRT, (U8 *)RRT[bBestOption[bChan][bChip]],RRT_PARAM_CNT);
	
		for(j = 0;j<RRT_PARAM_CNT;j++)
		{
			bSoftDecodeBaseRRT[j] += bLeftRightOffset[i];
		}
		
		Retry_SetFlashReg(bChan, bChip,READ_RETRY_SET_OPTION_MAX,ptDmaInfor->dRowAddr);
#endif

		Fdl_MCUStartReadDma(bChan, bChip, ptDmaInfor, bFlashMode);
	}
	while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
	{
	}

	Retry_SetFlashReg(bChan, bChip,bBestOption[bChan][bChip],ptDmaInfor->dRowAddr);
#endif
}
#else
void Fdl_MCUSoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bFlashMode, U8 bSBNum)
{
	U08 bData[4] = {0,0,0,0};
#if((NAND_FLASH == MIC_B16A) ||\
	(NAND_FLASH == MIC_B17A) ||\
	(NAND_FLASH == MIC_M16)  ||\
	(NAND_FLASH == MIC_N18)  ||\
	(NAND_FLASH == MIC_N28)  ||\
	(NAND_FLASH == MIC_B27A) ||\
	(NAND_FLASH == MIC_B27B) ||\
	(NAND_FLASH == INTEL_B27B) ||\
	(NAND_FLASH == MIC_M84C)||\
	(NAND_FLASH == YMTC_B08T)||\
	(NAND_FLASH == YMTC_A06M))
	bData[0] = bSBNum;
#endif

	Joint_Mode JointMode = HAL_ADMGetJointMode(bChan);
	
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM = bSBNum;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_EN = 1;
	SG_DMA2_CH(bChan)->rSTATUS.dAll = 0;

	HAL_McuSetMask(bChan, ptDmaInfor->bMaskInfo, MASK_BYTE_OF_CLST,0);						  
	HAL_McuSetRndSeed(bChan, ptDmaInfor->dRowAddr, ptDmaInfor->wSeedIdx);
	HAL_McuSetUd(bChan, ptDmaInfor->dUserDataAddr, ptDmaInfor->bSecOfClst, ptDmaInfor->bUdSize, 1);

	U08 i,j;
	for(i = 0;i<bSBNum+1;i++)
	{
		HAL_MCU_SetCe(bChan, bChip, 1); 	//拉片选，到发送命令，需要时间
		Delay(5);
		
		if(i == 0)
		{
			Delay(3);
			HAL_MCU_SendCmd(bChan,FL_CMD_SET_FEATURE);
			HAL_MCU_SendOneAddr(bChan,0x97);
			Delay(4);
			
#if 0
			for (j = 0; j < 4; j++)
			{
				HAL_MCUSendDataByDqs(bChan,bData[j]);
			}
#else
			HAL_MCUSendNByteDataByDqs(bChan,bData,4);
#endif

			HAL_MCUCheckStatus(bChan, bChip);
			
			HAL_MCU_SendCmd(bChan, FL_CMD_SOFT_READ0);
			HAL_MCU_SendTwoAddr(bChan, ptDmaInfor->wColAddr);
			HAL_MCU_SendThreeAddr(bChan, ptDmaInfor->dRowAddr);
			HAL_MCU_SendCmd(bChan, FL_CMD_READ2);
		}
		else
		{
			HAL_MCU_SendCmd(bChan, FL_CMD_SOFT_READ1);
		}
		
		HAL_MCUCheckStatus(bChan, bChip);
		HAL_MCU_SendCmd(bChan, FL_CMD_READ1);
		HAL_MCU_SendTwoAddr(bChan, ptDmaInfor->wColAddr);
		HAL_MCU_SendThreeAddr(bChan, ptDmaInfor->dRowAddr);
		HAL_MCU_SendCmd(bChan, FL_CMD_RANDOM_DATA_OUTPUT1);
		HAL_MCU_SendTwoAddr(bChan, ptDmaInfor->wColAddr);
		HAL_MCU_SendCmd(bChan, FL_CMD_RANDOM_DATA_OUTPUT2);
		Delay(10);

		HAL_ADMStartDMA2(bChan, ptDmaInfor->dDmaBuffAddr, ptDmaInfor->dDmaInfoAddr, DMA2_RD_TO_BUFF, 
			ptDmaInfor->dBuffIdx, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma, JointMode);
		
		while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE)
		{
		}
	}
	
	while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
	{
	}	
}

#endif

#ifdef __SOFT_OR_EX_LLR_3BIT__

void Fdl_MemORMem(U8* pDst,U8* pSrc1,U8* pSrc2,U32 nLen)
{
	U32 i = 0;
	for(i = 0; i < nLen; i++)
	{
		pDst[i] = pSrc1[i]^pSrc2[i];
	}
}

//只能使用RAID MEM
void RAID_MemORMem(U8 bChan,U8* pDst,U8* pSrc1,U8* pSrc2,U32 nLen)
{
	U32 bDmaLen = SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN;
	U32 bRaidBuff = SG_DMA2_CH(bChan)->rRAID_BUF_ADDR;
	Joint_Mode JointMode = HAL_ADMGetJointMode(bChan);
	U08 bRaidEn = SG_DMA2_CH(bChan)->rRAID_CTRL.bits.RAID_EN;
	U32 bDmainfo = SG_DMA2_CH(bChan)->rDMAINFO_BSADDR.bits.dVAL;

	SG_DMA2_CH(bChan)->rCTRL.bits.DIR = 0;			//先变成写方向
	SG_DMA2_CH(bChan)->rCTRL.bits.FC2ADM_MASK = 1;	//关闭FC-ADM通路
	SG_DMA2_CH(bChan)->rCTRL.bits.FIFO_DISABLE = 1;	//关闭ADM FIFO 空满判断功能
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = 512;
	SG_DMA2_CH(bChan)->rRAID_CTRL.bits.RAID_EN = 1;	//开启RAID写
	//HAL_MCU_SetCe(bChan,0,0);
	//HAL_MCU_SetRd(bChan);
	if(pDst == pSrc1)//Dst数据不需要初始化为pSrc1
	{
		SG_DMA2_CH(bChan)->rRAID_CTRL.bits.RAID_MODE = 1;
		SG_DMA2_CH(bChan)->rRAID_BUF_ADDR = pDst - 0x470000;//开启RAID写
		HAL_ADMStartDMA2(bChan, pSrc1, bDmainfo, DMA2_WR_TO_FLASH,
				0, 0, (nLen+511)/512, JointMode);
		while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE)
		{
			//LOG_PRINT("SoftDecode,Wait_Buf_Done,bChan=%d\n", bChan);
		}
		SG_DMA2_CH(bChan)->rRAID_CTRL.bits.RAID_MODE = 0;
	}
	SG_DMA2_CH(bChan)->rRAID_BUF_ADDR = pDst - 0x470000;//开启RAID写
	HAL_ADMStartDMA2(bChan, pSrc2, bDmainfo, DMA2_WR_TO_FLASH,
		0, 0, (nLen+511)/512, JointMode);
	while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE)
	{
		//LOG_PRINT("SoftDecode,Wait_Buf_Done,bChan=%d\n", bChan);
	}
	//HAL_MCU_SetCe(bChan,0,1);
	SG_DMA2_CH(bChan)->rCTRL.bits.FIFO_DISABLE = 0;
	SG_DMA2_CH(bChan)->rCTRL.bits.FC2ADM_MASK = 0;
	SG_DMA2_CH(bChan)->rCTRL.bits.FIFO_CLR = 1;
	SG_DMA2_CH(bChan)->rCTRL.bits.DIR = 1;//恢复读方向
	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = bDmaLen;
	SG_DMA2_CH(bChan)->rRAID_BUF_ADDR = bRaidBuff;
	SG_DMA2_CH(bChan)->rRAID_CTRL.bits.RAID_EN = bRaidEn;

}

void from_dmaram_to_admbuf(U8 bChan,U32 dBuffAddr,U32 dDmaInfoAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint)
{

	U8 bCrc = SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN;
	U8 bUserData = SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN;
	U8 bPunchEn = SG_DMA2_CH(bChan)->rECC_CFG.bits.PT_EN;
	U8 bRndBypass = SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS;
	U8 bDmaInfoEn = SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN;
	U32 bDmaLen = SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN;
	U32 bCwLen = SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN;

	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;			   //解决解码buf done不了的问题
	__nds32__dsb();

	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM = 2;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_EN = 1;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rECC_CFG.bits.PT_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN = 0;		//关CRC
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 1;	//数据直通
	SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN = 0;//关DMAINFO
	SG_DMA2_CH(bChan)->rENABLE.bits.EN = 1;
	SG_DMA2_CH(bChan)->rENABLE.bits.Reserved = 0;//dma20_ldpc_fifoody_en

	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN= 2380;

	if(joint == JOINT_NONE)
	{
		SG_DMA2_CH(bChan)->rBSADDR_HANDLE = dBuffAddr;
	}
	else
	{
		SG_DMA2_CH(bChan)->rBSADDR = dBuffAddr;
	}

	SG_DMA2_CH(bChan)->rCTRL.bits.DIR = dir;
	__nds32__dsb();
    SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG = bTotalSect;
    SG_DMA2_CH(bChan)->rINDEX_CFG = dBuffIdx;

    SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE = joint;
	 __nds32__dsb();

	SG_DMA2_CH(bChan)->rCTRL.bits.FC2ADM_MASK = 1;
	 __nds32__dsb();
	SG_DMA2_CH(bChan)->rSTART.bits.START_DMA2_BUF = 1;
	__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();
	__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();
	while(!SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE);

	SG_DMA2_CH(bChan)->rLEN_CFG.bits.BASIC_DATA_LEN = bDmaLen;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN = bCrc;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = bUserData;
	SG_DMA2_CH(bChan)->rECC_CFG.bits.PT_EN = bPunchEn;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = bRndBypass;
	SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN = bDmaInfoEn;
	__nds32__dsb();
}

void from_admbuf_to_LDPC(U8 bChan,U32 dBuffAddr,U32 dDmaInfoAddr, DMA2_Dir dir, U32 dBuffIdx, U8 bFlashIdx, U8 bTotalSect, Joint_Mode joint)
{
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM = 2;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.BYPASS_LP = 0;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_EN = 1;
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 1;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rCTRL.bits.DIR = dir;
	__nds32__dsb();
	if(joint == JOINT_NONE)
	{
		SG_DMA2_CH(bChan)->rBSADDR_HANDLE = dBuffAddr;
	}
	else
	{
		SG_DMA2_CH(bChan)->rBSADDR = dBuffAddr;
	}
	SG_DMA2_CH(bChan)->rCTRL.bits.FC2ADM_MASK = 1;

    SG_DMA2_CH(bChan)->rTOTAL_SECT_CFG = bTotalSect;
    SG_DMA2_CH(bChan)->rINDEX_CFG = dBuffIdx;
    SG_DMA2_CH(bChan)->rFLASH_INDEX.bits.FLASH_TRANS_INDEX = bFlashIdx;
	 __nds32__dsb();
    SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE = joint;
	 __nds32__dsb();

	SG_DMA2_CH(bChan)->rDMAINFO_BSADDR.bits.dVAL  = dDmaInfoAddr;

	 __nds32__dsb();
	SG_DMA2_CH(bChan)->rSTART.bits.START_DMA2_BUF = 1;
	__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();
	__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();__nds32__dsb();
	while(!SG_DMA2_CH(bChan)->rSTATUS.bits.BUF_DONE);
	SG_DMA2_CH(bChan)->rCTRL.bits.FIFO_CLR = 1;
	__nds32__dsb();
}

void Fdl_ExSoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bFlashMode, U8 bSBNum)
{
	U32 bAll0_FFCtrl = SG_FC(bChan)->rALL_00FF_CTRL.bits.bVAL;//关闭FC ALL00_0XFF功能
	Joint_Mode joint =  SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE;
	U08 ldpc_fifo_en = SG_DMA2_CH(bChan)->rENABLE.bits.Reserved;
	SG_FC(bChan)->rALL_00FF_CTRL.bits.bVAL = 0;

	from_dmaram_to_admbuf(bChan, MEM_SOFT_DATA, ptDmaInfor->dDmaInfoAddr, DMA2_WR_TO_FLASH,
	0, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma+1, JOINT_NONE);

	from_admbuf_to_LDPC(bChan, ptDmaInfor->dDmaBuffAddr, ptDmaInfor->dDmaInfoAddr, DMA2_RD_TO_BUFF,
	ptDmaInfor->dBuffIdx, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma, joint);

	SG_DMA2_CH(bChan)->rENABLE.bits.Reserved = 0;//dma20_ldpc_fifoody_en
	SG_DMA2_CH(bChan)->rENABLE.bits.EN = 0;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rENABLE.bits.Reserved = 0;//dma20_ldpc_fifoody_en
	SG_DMA2_CH(bChan)->rENABLE.bits.EN = 1;
	__nds32__dsb();
	//送SB0数据
	from_dmaram_to_admbuf(bChan, MEM_SOFT_DATA+4096*1, ptDmaInfor->dDmaInfoAddr, DMA2_WR_TO_FLASH,
	0, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma+1, JOINT_NONE);

	from_admbuf_to_LDPC(bChan, ptDmaInfor->dDmaBuffAddr, ptDmaInfor->dDmaInfoAddr, DMA2_RD_TO_BUFF,
	ptDmaInfor->dBuffIdx, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma, joint);

	SG_DMA2_CH(bChan)->rENABLE.bits.Reserved = 0;//dma20_ldpc_fifoody_en
	SG_DMA2_CH(bChan)->rENABLE.bits.EN = 0;
	__nds32__dsb();
	SG_DMA2_CH(bChan)->rENABLE.bits.Reserved = 0;//dma20_ldpc_fifoody_en
	SG_DMA2_CH(bChan)->rENABLE.bits.EN = 1;
	__nds32__dsb();
	from_dmaram_to_admbuf(bChan, MEM_SOFT_DATA+4096*2, ptDmaInfor->dDmaInfoAddr, DMA2_WR_TO_FLASH,
	0, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma+1, JOINT_NONE);

	from_admbuf_to_LDPC(bChan, ptDmaInfor->dDmaBuffAddr, ptDmaInfor->dDmaInfoAddr, DMA2_RD_TO_BUFF,
	ptDmaInfor->dBuffIdx, ptDmaInfor->bFlashIdx, ptDmaInfor->bSectOfDma, joint);

	while(!SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE);//解码完成
	SG_DMA2_CH(bChan)->rCTRL.bits.FC2ADM_MASK = 0;
	SG_CPM->rSOFT_RST1.wAll = 0xffffffef;//软解复位，否则第二次会卡死
	SG_FC(bChan)->rALL_00FF_CTRL.bits.bVAL = bAll0_FFCtrl;
	SG_DMA2_CH(bChan)->rENABLE.bits.Reserved = ldpc_fifo_en;

}
void Fdl_MCUSoftORDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bFlashMode, U8 bSBNum)
{
	U08 i,j = 0;
	U08 count = 0;
	//U08 pReadOffset[4] = {0, 0, 0, 0};
	U08 bLeftRightOffset[4];
	U32 bBuffAddr = ptDmaInfor->dDmaBuffAddr;
	U32 bSectOfDma = ptDmaInfor->bSectOfDma;
	U32 dBuffIdx = ptDmaInfor->dBuffIdx;
	U08 bMaskInfo = ptDmaInfor->bMaskInfo;//

	U8 bCrc = SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN;
	U8 bUserData = SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN;
	U8 bPunchEn = SG_DMA2_CH(bChan)->rECC_CFG.bits.PT_EN;
	U8 bRndBypass = SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS;
	U8 bDmaInfoEn = SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN;
	U32 bJointMode = SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE;//保存联动模式，此处要使用非联动模式读数据，数据读取完进入软解需要改为联动模式


	SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE = 0;//如果是联动，先关掉联动模式，进入非联动处理问题
	SG_DMA2_CH(bChan)->rECC_CFG.bits.PT_EN = 0;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = 0;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN = 0;		//关CRC
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = 1;	//数据直通
	SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN = 0;//关DMAINFO

	ptDmaInfor->dBuffIdx = 0;
	ptDmaInfor->bMaskInfo = 0;	//准备MEM数据时需要屏蔽MASK信息
	//配置环境，关ECC读CW到MEM
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;
	ptDmaInfor->bSectOfDma = 8;
	ptDmaInfor->dDmaBuffAddr = MEM_SOFT_DATA;
	U08MemCopy(bLeftRightOffset,bLeft_RightOffset_4SB, 4);
	//读HB
	HAL_McuSetMask(bChan, ptDmaInfor->bMaskInfo, MASK_BYTE_OF_CLST,0);
	HAL_McuSetRndSeed(bChan, ptDmaInfor->dRowAddr, ptDmaInfor->wSeedIdx);
	HAL_McuSetUd(bChan, ptDmaInfor->dUserDataAddr, ptDmaInfor->bSecOfClst, ptDmaInfor->bUdSize, 1);
	Fdl_MCUStartReadDma(bChan, bChip, ptDmaInfor, bFlashMode);

	for (i = 0; i < 2; i++)//读取SB0和SB1到mem
	{
#if ((NAND_FLASH == TOS_BICS3) || (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == YMTC_B08T))
		U08MemCopy(bSoftDecodeBaseRRT, (U8 *)RRT[bBestOption[bChan][bChip]],RRT_PARAM_CNT);

		for(j = 0;j<RRT_PARAM_CNT;j++)
		{
			bSoftDecodeBaseRRT[j] += bLeftRightOffset[i];
		}
		Retry_SetFlashReg(bChan, bChip,READ_RETRY_SET_OPTION_MAX,ptDmaInfor->dRowAddr);
#endif
		ptDmaInfor->dDmaBuffAddr += 4096;
		Fdl_MCUStartReadDma(bChan, bChip, ptDmaInfor, bFlashMode);
		while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
		{
		}
	}
	//此处进行MEM数据亦或运算
	RAID_MemORMem(bChan,(U8*)(MEM_SOFT_DATA+4096*1),(U8*)(MEM_SOFT_DATA+4096*1),(U8*)(MEM_SOFT_DATA+4096*2),2400);

	for (i = 2; i < 4; i++)//读取SB2和SB3到mem
	{
#if ((NAND_FLASH == TOS_BICS3) || (NAND_FLASH == SUMSUNG_3DV4) || (NAND_FLASH == YMTC_B08T))
		U08MemCopy(bSoftDecodeBaseRRT, (U8 *)RRT[bBestOption[bChan][bChip]],RRT_PARAM_CNT);

		for(j = 0;j<RRT_PARAM_CNT;j++)
		{
			bSoftDecodeBaseRRT[j] += bLeftRightOffset[i];
		}
		Retry_SetFlashReg(bChan, bChip,READ_RETRY_SET_OPTION_MAX,ptDmaInfor->dRowAddr);
#endif
		Fdl_MCUStartReadDma(bChan, bChip, ptDmaInfor, bFlashMode);
		ptDmaInfor->dDmaBuffAddr += 4096;
		while(1 != SG_DMA2_CH(bChan)->rSTATUS.bits.REAL_DONE)
		{
		}
	}
	//此处进行MEM数据亦或运算
	RAID_MemORMem(bChan,(U8*)(MEM_SOFT_DATA+4096*2),(U8*)(MEM_SOFT_DATA+4096*2),(U8*)(MEM_SOFT_DATA+4096*3),2400);


	Retry_SetFlashReg(bChan, bChip,bBestOption[bChan][bChip],ptDmaInfor->dRowAddr);
	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 1;	//环境恢复
	ptDmaInfor->dDmaBuffAddr = bBuffAddr;			//恢复ptDmaInfor->dDmaBuffAddr
	ptDmaInfor->bSectOfDma = bSectOfDma;
	ptDmaInfor->dBuffIdx = dBuffIdx;
	ptDmaInfor->bMaskInfo =  bMaskInfo;

	SG_DMA2_CH(bChan)->rRND_CTRL.bits.CRC_EN = bCrc;
	SG_DMA2_CH(bChan)->rENABLE.bits.UDATA_EN = bUserData;
	SG_DMA2_CH(bChan)->rECC_CFG.bits.PT_EN = bPunchEn;
	SG_DMA2_CH(bChan)->rRND_CTRL.bits.RND_BYPASS = bRndBypass;
	SG_DMA2_CH(bChan)->rENABLE.bits.SAVE_DMAINFO_EN = bDmaInfoEn ;
	SG_DMA2_CH(bChan)->rCTRL.bits.JOINT_MODE = bJointMode;//恢复原来的模式
}

U8 FDL_LDPC_SoftDecode(U8 bChan, U8 bChip, T_DMA_INFOR *ptDmaInfor, U8 bFlashMode)
{
//	U8 bChIdx;
//	//准备开始进行软解码
//#ifndef __SOFT_INNER_LLR_3BIT__
//	Retry_SetFlashReg_Arc(bChan, bChip, 0x02);	//进入软解码，就关闭calibration功能，保持persistence。
//#endif
//
//	Fdl_MCUSoftORDecode(bChan, bChip, ptDmaInfor, bFlashMode, 2);//准备MEM数据
//	//软解码前的初始设置
//	for (bChIdx=0; bChIdx<4; bChIdx++)
//	{
//		if (bChIdx != bChan)
//		{
//			SG_DMA2_CH(bChIdx)->rENABLE.bits.HOLD_EN = 1;	    //Hold住其他通道所有CQ
//		}
//	}
//	__nds32__dsb();
//	while(SG_LDPC->rLDEC_CH0_STATUS.bits.bVAL != 1);			//等待进入的CW解码完成
//	while(SG_LDPC->rLDEC_CH1_STATUS.bits.bVAL != 1);
//	while(SG_LDPC->rLDEC_CH2_STATUS.bits.bVAL != 1);
//	while(SG_LDPC->rLDEC_CH3_STATUS.bits.bVAL != 1);
//
//	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 0;			   //解决解码buf done不了的问题
//	__nds32__dsb();
//	SG_DMA2_CH(bChan)->rECC_CTRL.bits.ECC_EN = 1;
//	//读取HB,SBR2,SBR2
//	Fdl_ExSoftDecode(bChan, bChip, ptDmaInfor, bFlashMode, 2);//读HB,SBR1,SBR2
//
//	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_NUM = 0;
//	__nds32__dsb();
//	SG_DMA2_CH(bChan)->rECC_CTRL.bits.SB_EN = 0;
//
//	for (bChIdx=0; bChIdx<4; bChIdx++)
//	{
//		if (bChIdx != bChan)
//		{
//			SG_DMA2_CH(bChIdx)->rENABLE.bits.HOLD_EN = 0;		// 恢复其他通道CQ的执行
//		}
//	}
//	PRINT_TXT("Soft");
//	//PRINT_HEX("Soft:",SG_DMA2_CH(bChan)->rSTATUS.dAll);
//#ifdef __RETRY_CNT_RECORD__
//    pFdlParam.FTL_CollectRetryInfor(bPhyToLogChan[bChan],bPhyCeToLogLun[bChip],1,0);//ftl保证实现该回调,各chan各lun的retry次数计数
//#endif
//    return 0;
}
#endif
