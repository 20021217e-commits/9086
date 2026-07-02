#ifndef __RDT_PVALUE_SCAN__
#define __RDT_PVALUE_SCAN__


#define RETRY_MAX_OFFSET_VALUE		(100U)			//-128~+128，私有命令参数不一样

#ifdef __FDL_SET_RETRY_CFG
#define __FDL_SET_RETRY_EXT
#else
#define __FDL_SET_RETRY_EXT			extern
#endif



/*数据结构定义*/
enum CellType
{
	SLC_ZONE,
	MLC_ZONE,
	TLC_ZONE,
};
	
enum OffsetInWordLine
{
	LP,
	UP,
	XP,
};

typedef struct
{
	U08 bFeatureAddrs[11];	/*Feature Addr*/
	U08 bPValue[11];
	U08 bReserve[2];
}T_PVALUE;


typedef struct
{
	U08 bWordLine;
	U08 bFeatureAddr;	/*Feature Addr*/		
}T_PAGE_MODE;

//typedef struct
//{
//	U08 bFeatureAddr[11/2];	/*Feature Addr*/
//	U08 bBestPValues[11/2];	/*最优P值*/
//	U32 dErrorBitCur[11/2];	/*当前错误Bit数*/
//	U32 dErrorBitMIX[11/2];	/*最小错误Bit数*/
//}T_ECC_INFO;

typedef struct
{		
	U08 bRev;			/*保留*/
	U08 bPValueCnt;		/*扫描的Offset个数*/
	U08 bTLCPreCmd;		/*区分ED3和其他Flash*/
	U08 bStartPValue;	/*扫描的起始Offset*/
	U08 bFeatureAddrMLC;/*Feature Addr Cnt MLC*/
	U08 bFeatureAddrSLC;/*Feature Addr Cnt SLC*/
	U08 bFeatureAddrTLC;/*Feature Addr Cnt TLC*/
	U08 bFeatureAddrSUB;/*Feature Addr 分段扫描*/
}T_BEST_PVALUE_SCAN;

typedef struct
{
	U08 bRetryEn ;
	U08 bRev[3];
	U32 dRetryOffsetVlaue ;
}T_RETRY_OPTION ;


__FDL_SET_RETRY_EXT T_RETRY_OPTION  tFdlRtyOption;

/*对外接口声明*/
void RDT_PValueScanInit(void);
void RDT_BestPValueSet(U08 bChan, U08 bChip, U08 bPValue);
void RDT_BestPValueScan(U08 bChan,U08 bChip,U16 wBlock,U08 *bBestPValue);
extern U08 bBestPvaules[11][5];
extern T_BEST_PVALUE_SCAN tRDTBestPValueScan;

/*扫描用的参数宏(切记:一定要先初始化再使用!!!)*/
#define SLC_FEATURE_ADDR_CNT 	(tRDTBestPValueScan.bFeatureAddrSLC)/*Feature Addr Cnt SLC*/
#define MLC_FEATURE_ADDR_CNT 	(tRDTBestPValueScan.bFeatureAddrMLC)/*Feature Addr Cnt MLC*/
#define TLC_FEATURE_ADDR_CNT 	(tRDTBestPValueScan.bFeatureAddrTLC)/*Feature Addr Cnt TLC*/
#define FEATURE_ADDR_CNT 		(SLC_FEATURE_ADDR_CNT + MLC_FEATURE_ADDR_CNT + TLC_FEATURE_ADDR_CNT)/*Feature Addr个数*/
#define FEATURE_ADDR_SUB 		(tRDTBestPValueScan.bFeatureAddrSUB)/*Feature Addr分段*/
#define PVALUE_CNT 				(tRDTBestPValueScan.bPValueCnt)		/*需要扫描的p值个数*/
#define START_PVALUE 			(tRDTBestPValueScan.bStartPValue)	/*扫描起始值*/
#define CELL_TYPE_CNT 			(3)			/*TLC块里面Cell Type(SLC MLC TLC)个数(暂时不能更改)*/
#define SECTOR_OF_CULST 		(32)		/*一次读取Sector个数(暂时不能更改)*/
#define OUT_OF_MEMORY_ADDR		(0x5F3000)	/*DMA Buff 使用物理上不存在的Memory*/
#define TLC_PRE_CMD     		(tRDTBestPValueScan.bTLCPreCmd)/*区分ED3和其他Flash*/


#if 0
#define M16_TP_PAGE_SEG1_NUM  120  //TP page 最小页
#define M16_TP_PAGE_SEG2_NUM  3050 // TP page order 分段
#define M16_TP_PAGE_SEG3_NUM  3070 // TP page 最大页

#define M16_TP_PAGE_GAP1  4
#define M16_TP_PAGE_GAP2  2
#else
#endif


#endif


