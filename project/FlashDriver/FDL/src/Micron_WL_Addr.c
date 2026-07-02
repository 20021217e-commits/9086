/*
 * Micron_WL_Addr.c
 *
 *  Created on: Feb 18, 2019
 *      Author: Administrator
 */
#include "flash_driver_config.h"
#include "fdl_soft_decode.h"
#include "fdl_mcu_op.h"
#include "uitility.h"
#include "fdl_common.h"
#include "fdl_task_op.h"
#include "Micron_WL_Addr.h"

#if ((NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A))
U8 IFLP_PAGE(U32 dRowAddr)
{
	U32 dPageNum = GET_PAGE_NUM(dRowAddr);

	if((dPageNum>=0) && (dPageNum<=11))
	{
		return SLC_LP_PAGE;
	}
	else if((dPageNum>=12) && (dPageNum<=34))
	{
		if(dPageNum%2 == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if((dPageNum>=36) && (dPageNum<=59))
	{
		return TLC_LP_PAGE;
	}
	else if((dPageNum>=62) && (dPageNum<2222))
	{
		if((dPageNum-62)%3 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=2222) && (dPageNum<=2266))
	{
		if((dPageNum-2222)%4 == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if((dPageNum>=2292) && (dPageNum<2304))
	{
		return SLC_LP_PAGE;
	}
	return 0;
}
#elif (NAND_FLASH == MIC_B27A)
U8 IFLP_PAGE(U32 dRowAddr)
{
	U32 dPageNum = GET_PAGE_NUM(dRowAddr);

	if((dPageNum>=0) && (dPageNum<=17))
	{
		return SLC_LP_PAGE;
	}
	else if((dPageNum>=89) && (dPageNum<2466))
	{
		if((dPageNum-89)%3 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if(((dPageNum>=18) && (dPageNum<54)) 		||
			((dPageNum>=2466) && (dPageNum<=2501)) 	||
			((dPageNum>=2592) && (dPageNum<=2627)) 	||
			((dPageNum>=2646) && (dPageNum<=2681)) 	||
			((dPageNum>=5112) && (dPageNum<=5147)))
	{
		if((dPageNum)%2 == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if(((dPageNum>=54) && (dPageNum<89)) 		||
			((dPageNum>=2628) && (dPageNum<=2645)) 	||
			((dPageNum>=2574) && (dPageNum<2592)))
	{
		return TLC_LP_PAGE;
	}
	else if((dPageNum>=2682) && (dPageNum<5060))
	{
		if((dPageNum)%3 == 2)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=5060) && (dPageNum<5112))
	{
		if((dPageNum)%3 == 2)
		{
			return SLC_LP_PAGE;
		}
	else if((dPageNum>=5112) && (dPageNum<5148))
	{
		if((dPageNum-5112)%2 == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	return 0;
}
#elif (NAND_FLASH == MIC_B0KB)
U8 IFLP_PAGE(U32 dRowAddr)
{
	U32 dPageNum = GET_PAGE_NUM(dRowAddr);

	if((dPageNum>=0) && (dPageNum<=15))
	{
		return SLC_LP_PAGE;
	}
	else if((dPageNum>=16) && (dPageNum<=46))
	{
		if(dPageNum%2 == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if((dPageNum>=48) && (dPageNum<=110))
	{
		if((dPageNum-48)%2 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=113) && (dPageNum<=1406))
	{
		if((dPageNum-113)%3 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=1409) && (dPageNum<=1454))
	{
		if((dPageNum-1409)%3 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=1457) && (dPageNum<=1502))
	{
		if((dPageNum-1457)%3 == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if((dPageNum>=1505) && (dPageNum<=1535))
	{
		if((dPageNum-1505)%2 == 0)
		{
			return SLC_LP_PAGE;
		}
	}
	return 0;
}
#elif ((NAND_FLASH == MIC_N18) || (NAND_FLASH == MIC_M16))
U8 IFLP_PAGE(U32 dRowAddr)
{
	U32 dPageNum = GET_PAGE_NUM(dRowAddr);

	if((dPageNum>=0) && (dPageNum<=11))
	{
		return SLC_LP_PAGE;
	}
	else if((dPageNum>=12) && (dPageNum<=47))
	{
		if(dPageNum%3 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=48) && (dPageNum<=119))
	{
		if(dPageNum%3 == 0)
		{
			return QLC_LP_PAGE;
		}
	}
	else if((dPageNum>=120) && (dPageNum<=2999))
	{
		if((dPageNum-1)%4 == 0)
		{
			return QLC_LP_PAGE;
		}
	}
	else if((dPageNum>=3000) && (dPageNum<=3047))
	{
		if((dPageNum-1)%4 == 0)
		{
			return TLC_LP_PAGE;
		}
	}
	else if((dPageNum>=3048) && (dPageNum<=3071))
	{
		if(3 - (dPageNum%2)*3 == 0)
		{
			return SLC_LP_PAGE;
		}
	}
	return 0;
}
#elif (NAND_FLASH == MIC_N28)
U8 IFLP_PAGE(U32 dRowAddr)                    
{                                             
	U32 wPageNum = GET_PAGE_NUM(dRowAddr);      

	if((wPageNum>=0) && (wPageNum<12))          
	{                                           
		return SLC_LP_PAGE;                       
	}                                           
	if((wPageNum>=12) && (wPageNum<24))         
	{                                           
		return TLC_LP_PAGE;                       
	}                                           
	if(	(wPageNum>=24 && wPageNum<132))        
	{                                           
		if((wPageNum%3) == 0)                     
		{                                         
			return QLC_LP_PAGE;                     
		}                                         
	}                                           
	else if((wPageNum>=132 && wPageNum<2148)  ||
			(wPageNum>=2292 && wPageNum<2340) ||    
			(wPageNum>=2196 && wPageNum<2244) ||    
			(wPageNum>=2496)&&(wPageNum<2544) ||    
			(wPageNum>=2580)&&(wPageNum<4500))      
	{                                           
		if((wPageNum&3) == 0)                     
		{                                         
			return QLC_LP_PAGE;                     
		}                                         
	}                                           
	else if((wPageNum>=2148 && wPageNum<2196) ||
			(wPageNum>=2244 && wPageNum<2292) ||    
			(wPageNum>=2412)&&(wPageNum<2460) ||    
			(wPageNum>=4500)&&(wPageNum<4548))      
	{                                           
		if((wPageNum&3) == 0)                     
		{                                         
			return TLC_LP_PAGE;                     
		}                                         
	}                                           
	else if((wPageNum>=2376 && wPageNum<2412) ||
			(wPageNum>=2460)&&(wPageNum<2496) ||
			(wPageNum>=2544)&&(wPageNum<2580))      
	{                                           
		if((wPageNum%3) == 0)                     
		{                                         
			return QLC_LP_PAGE;                     
		}                                         
	}  
	else if((wPageNum>=2340 && wPageNum<2376))      
	{                                           
		if((wPageNum%3) == 0)                     
		{                                         
			return TLC_LP_PAGE;                     
		}                                         
	} 
	else                                        
	{                                           
		if ((wPageNum%5) == 3)                    
		{                                         
			return SLC_LP_PAGE;                     
		}                                         
	}                                           
	return 0;                                   
}  
#elif ((NAND_FLASH == MIC_B27B) || (NAND_FLASH == INTEL_B27B))
U8 IFLP_PAGE(U32 dRowAddr)
{
	U32 wPageNum = GET_PAGE_NUM(dRowAddr);

	if((wPageNum>=0) && (wPageNum<12))
	{
		return SLC_LP_PAGE;
	}
	if((wPageNum>=12 && wPageNum<36))
	{
		if((wPageNum%2) == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if(wPageNum>=36 && wPageNum<60)
	{
		return TLC_LP_PAGE;
	}
	else if(wPageNum>=60 && wPageNum<1610)
	{
		if((wPageNum%3) == 2)
		{
			return TLC_LP_PAGE;
		}
	}
	else if ((wPageNum>=1610)&&(wPageNum<1658))
	{
		if ((wPageNum&3) == 2)
		{
			return MLC_LP_PAGE;
		}
	}

	else if ((wPageNum>=1658)&&(wPageNum<1694))
	{
		if ((wPageNum%3) == 2)
		{
			return TLC_LP_PAGE;
		}
	}

	else if ((wPageNum>=1694)&&(wPageNum<1740))
	{
		if ((wPageNum&3) == 2)
		{
			return MLC_LP_PAGE;
		}
	}
	else if ((wPageNum>=1740)&&(wPageNum<1752))
	{
		return TLC_LP_PAGE;
	}
	else if ((wPageNum>=1752)&&(wPageNum<1776))
	{
		if ((wPageNum%2) == 0)
		{
			return MLC_LP_PAGE;
		}
	}
	else if ((wPageNum>=1776)&&(wPageNum<1787))
	{
		return TLC_LP_PAGE;
	}
	else if(wPageNum>=1787 && wPageNum<3374)
	{
		if((wPageNum%3) == 2)
		{
			return TLC_LP_PAGE;
		}
	}
	else if(wPageNum>=3374 && wPageNum<3422)
	{
		if((wPageNum&3) == 2)
		{
			return MLC_LP_PAGE;
		}
	}
	else if(wPageNum>=3422 && wPageNum<3456)
	{
		if((wPageNum%3) == 2)
		{
			return SLC_LP_PAGE;
		}
	}
	return 0;
}
#else
U8 IFLP_PAGE(U32 dRowAddr)	//其他Flash
{
	return 0;
}
#endif




//#if ((NAND_FLASH == MIC_B0KB) || (NAND_FLASH == MIC_B16A) || (NAND_FLASH == MIC_B17A)||(NAND_FLASH == MIC_B27A) || (NAND_FLASH == MIC_M16) || (NAND_FLASH == MIC_N18))
#if (READ_RETRY_OFFSET_MAX != 0)
U8 IFSLC_OR_TLCLP(U32 dRowAddr)
{
	U8 bTLCPageFlag = 0;
#ifdef __RETRY_LP_PAGE_OFFSET__/*LP Page走Read Offset流程*/
	if((dRowAddr>>24) == 0)	// SLC页
	{
		return SLC_LP_PAGE;
	}
	bTLCPageFlag = IFLP_PAGE(dRowAddr);	// 判断是否是TLC LP页
#endif
	return bTLCPageFlag;
}
#else
U8 IFSLC_OR_TLCLP(U32 dRowAddr)
{
	return 0;
}
#endif
