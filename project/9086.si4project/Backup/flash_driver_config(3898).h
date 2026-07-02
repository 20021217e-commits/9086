
#ifndef     __FLASH_DRIVER_CONFIG_H__    
#define     __FLASH_DRIVER_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


// supported device

// 0    ~ 511  Toshiba 2D
// 512  ~ 999  Toshiba 3D TLC&MLC
// 1000 ~ 1023 Toshiba 3D QLC
// 1024 ~ 1535 Micron 2D
// 1536 ~ 1999 Micron 3D TLC&MLC
// 2000 ~ 2047 Micron&Intel&Solidigm 3D QLC
// 2048 ~ 2559 Hynix 2D
// 2560 ~ 2999 Hynix 3D TLC&MLC
// 3000 ~ 3071 Hynix 3D QLC
// 3072 ~ 3583 samsung 2D
// 3584 ~ 3999 samsung 3D TLC&MLC
// 4000 ~ 4095 samsung 3D QLC
// 4096 ~ 4999 YMTC 3D TLC&MLC
// 5000 ~ 5119 YMTC 3D QLC

/*Toshiba&Sandisk (Align with jupiter)*/
#define KHMB                (518)  // 旺宏4GB MLC KH30LMBGX8AAWGB 和TOSHIBA_5DCJ相同

#define SanDisk_BiCS5_QLC 	(1000) //0F25
#define SanDisk_BiCS6_TLC 	(1001) //T0T46
#define SanDisk_BiCS6_QLC 	(1002) //T0T46
#define SanDisk_BiCS8_TLC 	(1003) //T0T48
#define SanDisk_BiCS8_QLC   (1004) //T0F48

/*Micron (Align with jupiter)*/
#define MIC_B27B 			(1541)
#define MIC_B47R 			(1543)
#define MIC_B58R 			(1544)
#define MIC_B57T 			(1545)
#define MIC_N48R 			(2004)
#define MIC_N58R			(2005)
#define MIC_N69R            (2006)

/*Hynix (Align with jupiter)*/
#define Hynix_3DV5_TLC 		(2562)
#define Hynix_3DV6_TLC 		(2563)
#define Hynix_3DV7_TLC 		(2565)
#define Hynix_3DV8_TLC 		(2566)
#define Hynix_3DV9_TLC      (2567)

#define Hynix_3DV7_QLC 		(3001)


/*YMTC (Align with jupiter)*/
#define YMTC_X2_9060 		(4099)
#define YMTC_X3_9060 		(4100)
#define YMTC_X3_9070 		(4101)
#define YMTC_X4_9060 		(4102)
#define YMTC_X4_9070        (4103)

#define YMTC_X2_6070 		(5001)
#define YMTC_X3_6070 		(5002)
#define YMTC_X4_6080        (5003)


/*INTEL[Solidigm] (Align with jupiter)*/
#define INTEL_N38A 			(2003)
#define INTEL_N38B 			(5121)
#define INTEL_N4xA 			(5122)

/*SAMSUNG (Align with jupiter)*/
#define SAMSUNG_3DV6_TLC 	(3589)
#define SAMSUNG_3DV8_TLC 	(3591)
#define SAMSUNG_3DV6P    	(3592)
#define SAMSUNG_3DV7_TLC 	(3595)
#define  SAMSUNG_3DV5_QLC	(3590)
#define  SAMSUNG_3DV4_QLC   (3593)
#define  SAMSUNG_3DV7_QLC   (3594)
#define SAMSUNG_3DV5_2K_MLC (3998)


//Flash选择
//#define NAND_FLASH  MIC_B27B
//#define NAND_FLASH  MIC_B47R
//#define NAND_FLASH  MIC_B58R
//#define NAND_FLASH  MIC_N48R
//#define NAND_FLASH  MIC_B57T
//#define NAND_FLASH  MIC_N58R
//#define NAND_FLASH  MIC_N69R
//#define NAND_FLASH  INTEL_N38A
//#define NAND_FLASH  INTEL_N38B
//#define NAND_FLASH  INTEL_N4xA


//#define NAND_FLASH  YMTC_X2_6070
//#define NAND_FLASH  YMTC_X2_9060
//#define NAND_FLASH  YMTC_X3_9060
//#define NAND_FLASH  YMTC_X3_9070
//#define NAND_FLASH  YMTC_X3_6070
#define NAND_FLASH  YMTC_X4_9060//未测试,202308月底到样片
//#define NAND_FLASH  YMTC_X4_6080//未测试

//#define NAND_FLASH  Hynix_3DV6_TLC
//#define NAND_FLASH  Hynix_3DV5_TLC
//#define NAND_FLASH  Hynix_3DV7_TLC
//#define NAND_FLASH  Hynix_3DV7_QLC //H25T0QA18CX542,7T0Q
//#define NAND_FLASH  Hynix_3DV8_TLC //H25G9TD18CX576,8G9T
//#define NAND_FLASH  Hynix_3DV9_TLC//H25T3TD88CX860,9T0T

//#define NAND_FLASH  SAMSUNG_3DV6_TLC
//#define NAND_FLASH  SAMSUNG_3DV8_TLC//K9AKGD8J0B
//#define NAND_FLASH  SAMSUNG_3DV7_TLC
//#define NAND_FLASH  SAMSUNG_3DV6P
//#define NAND_FLASH  SAMSUNG_3DV4_QLC
//#define NAND_FLASH  SAMSUNG_3DV5_QLC
//#define NAND_FLASH  SAMSUNG_3DV7_QLC
//#define NAND_FLASH  SAMSUNG_3DV5_2K_MLC

//#define NAND_FLASH KHMB                    //旺宏4GB MLC KH30LMBGX8AAWGB 和TOSHIBA_5DCJ相同

//#define NAND_FLASH  SanDisk_BiCS5_QLC    //0F25
//#define NAND_FLASH  SanDisk_BiCS6_TLC    //T0T46
//#define NAND_FLASH  SanDisk_BiCS6_QLC    //0F46
//#define NAND_FLASH  SanDisk_BiCS8_TLC    //T0T48
//#define NAND_FLASH  SanDisk_BiCS8_QLC    //T0F48

#define ADDR_MODE_5 5
#define ADDR_MODE_6 6


#define WL_ADD_TYPE_TLC 1
#define WL_ADD_TYPE_QLC 2



//flash physical parameter
#if (NAND_FLASH == MIC_B27B)
#define READ_RETRY_SET_OPTION_MAX 8		//如有修改需要，请先和Webb确认
#define READ_RETRY_ARC_MAX 4			//如有修改需要，请先和Webb确认
#define READ_RETRY_OFFSET_MAX 18			//如有修改需要，请先和Webb确认
#else
#define READ_RETRY_SET_OPTION_MAX 8
#define READ_RETRY_ARC_MAX 0			//如有修改需要，请先和Webb确认
#define READ_RETRY_OFFSET_MAX 0			//如有修改需要，请先和Webb确认
#define RRT_TABLE_CNT      (READ_RETRY_SET_OPTION_MAX)
#define RRT_PARAM_CNT      (8)
#endif




/*******************************************************
**************************宏管理************************
*******************************************************/
/*《1》FDL Debug类宏开关*/
#define FDL_LOG_ON						(FUNC_ENABLE)/*FDL 是否打印Log信息*/
#define FDL_EXCEPTION_DETECTIVE_ON		(FUNC_ENABLE)/*FDL 是否打印异常信息*/
//#define __RECORD_CONTINUE_COUNT__		/*Debug:记录Continue每段个数,便于分析(最多记录20段数据)--删掉*/
//#define __SLC_0X10_PRAGRAM__	 /*SLC使用10h编程，默认关闭，只在Cache写有问题时打开*/
//#define __RETRY_TABLE_RATE__	 /*统计Retry Table的使用率(每一组Retry表格的使用次数)*/



/*《2》FDL已测试稳定的功能开关类宏开关*/
#define __PRE_4K_READ_ENABLE__			/*Pre系列4K接口开关*/
//#define __FILL_PAGE_EN__				/*补页功能打开*/
//#define __L06B_TO_B0KB__				/*L06B转B0KB(85h->P1=02h,P2=P3=P4=0)*/
//#define __FILL_SLC_BLOCKS__			/*快速填充Block没写满的Page,接口开关*/
#define __RETRY_LP_PAGE_OFFSET__		/*Retry LP Page走Offset流程开关*/
#define __RETRY_PERSISTENCE__			/*Retry保持功能*/

#define RETRY_SVALUE (0x180)			/*用于读校验Retry Pass的S-Value阈值 100bit左右*/

#ifdef __SLC_JOINT_WR_FAIL__
#define __SLC_PRG_FAIL_HANDL__			/*SLC编程Fail处理(默认打开)*/
#endif

#ifdef __SLC_PRG_FAIL_HANDL__/*SLC编程Fail处理(默认打开)*/
#define __RAID_FUNCTION_ON__/*Raid功能*/
#endif

#ifndef __YS9083XT_ALGO__	/*由FTL定义全局宏*/
#define __4CH_X_4CE_SUPPORT__			/*4X4支持*/
#endif

#ifdef __YS9083XT_ALGO__/*由FTL定义全局宏*/
#define __YS9083_FDL__/*YS9083(主要用来控制读ID的时候CH3是否需要降频)*/
#endif



//《3》待测试使用的宏
//#define __BADBLK_BINDMAP__			/*Row地址重映射,用于坏块处理*/
//#define __BADBLK_BINDMAP_FDLTEST__	/*Row地址重映射,CopyBack PlaneMapping-测试流程,非专业人士,请勿打开*/
//#define __TASK_ERROR_FORCE_STOP__ 	/*任务异常强制停止功能*/
#ifdef __DIS_ALIGN_RD__/*由FTL定义全局宏*/
#define __LAST_CLUST_MASK_OFF__		/*联动读关掉最后一个簇的Mask*/
#endif
//#define __TASK_REORDER_TO_READ_TABLE__/*读表格Row地址重排序功能*/

//《4》FTL根据需要使用的宏
#define __RETRY_CNT_RECORD__			 /*记录Retry Cnt统计开关,默认关闭*/
#define __SIMPLIFY_CODE__                /*精简代码*/

#if((NAND_FLASH == MIC_M84C))			/*使用BCH模式的Flash请都添加在这里(‘||’或上去)*/
#define __CFG_BCH_MODE__				/*使用BCH模式*/
#endif

#if((NAND_FLASH == MIC_B0KB)	||\
	(NAND_FLASH == MIC_B16A)	||\
	(NAND_FLASH == MIC_B17A)	||\
	(NAND_FLASH == MIC_M16)		||\
	(NAND_FLASH == MIC_N18)		||\
	(NAND_FLASH == MIC_N28)		||\
	(NAND_FLASH == MIC_L06B)	||\
	(NAND_FLASH == TOS_BICS3)	||\
	(NAND_FLASH == HYNIX_8A1M)	||\
	(NAND_FLASH == HYNIX_3DV4)	||\
	(NAND_FLASH == MIC_B27A)	||\
	(NAND_FLASH == MIC_B27B)	||\
	(NAND_FLASH == INTEL_B27B)	||\
	(NAND_FLASH == AFGM)		||\
	(NAND_FLASH == SUMSUNG_3DV4)||\
	(NAND_FLASH == GCGF)||\
	(NAND_FLASH == YMTC_B08T))

#if(NAND_FLASH != TOS_BICS3) && (NAND_FLASH != HYNIX_8A1M)
#define __CACHE_READ_ONLY_31H__ 	/*Cache读,只发31h减少发地址的时间(关闭就会发Row地址)*/
#endif
#ifdef __APP_CACHE_READ__			/*由FTL定义全局宏*/
#define __CACHE_READ_ON__ 			/*Cache Read开关(理论上所有单CE配置模式都可以打开)*/
#define __TAB_CACHE_RD_OFF__		/*表格Cache RD开关(默认关闭)*/
#endif
#endif


//(5)待解决的问题使用的宏

#if((NAND_FLASH == TOS_BICS3) || (NAND_FLASH == HYNIX_8A1M))			/*ED3类型Flash请都添加在这里(‘||’或上去)*/
#define TLC_PRE_CMD (3) 				/*需要用前缀1、2、3来区分同一个WordLine中的LP、XP、HP*/
#define TLC_ED3_MODE_SUPPORT	(1) 	/*兼容ED3类型Flash的宏开关(1:打开,0:关闭)*/
#else
#define TLC_PRE_CMD (1) 				/*无需用前缀1、2、3来区分同一个WordLine中的LP、XP、HP,用Row地址直接指定*/
#define TLC_ED3_MODE_SUPPORT	(0) 	/*兼容ED3类型Flash的宏开关(1:打开,0:关闭)*/
#endif

#if (NAND_FLASH == TOS_BICS3)
#define __CMD_SAME_AS_SMI__
#endif

#ifdef __TLC_RD_VERIFY__/*由FTL定义全局宏*/
#ifndef __DEBUG_TLC_VERIFY__/*由FTL定义全局宏*/
//#define __VER_TLC_PAGE__				/*快速检验CopyBack搬移到TLC Page的数据,接口开关*/
#endif
#endif




#ifdef __cplusplus
}
#endif

#endif







