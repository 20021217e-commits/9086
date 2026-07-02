#ifndef    _MAIN_H_
#define    _MAIN_H_

#ifndef __DUAL_CORE__
#define __DUAL_CORE__
#endif
extern U8_C code_name[];
#define DMA_BUFFER_SIZE    (128*1024)

//DMA buffer define
typedef union _DMA_BUFFER
{
    U8  bArray[DMA_BUFFER_SIZE];
    U16 wArray[DMA_BUFFER_SIZE/2];
    U32 dArray[DMA_BUFFER_SIZE/4];
}T_DMA_BUFFER,*pT_DMA_BUFFER;
#ifndef __ALG_FORMAL
#else
typedef  struct __attribute__((__packed__))
{
	U32  dBankVAddr[8];//最高bit为0表示使用BANK0虚拟映射，为1表示使用BANK1虚拟映射
	U32  dBank0PAddr;
	U32  dBank1PAddr;
	U32  dVMAStartAddr;
	U32  dVMAEndAddr;
}T_G0_VMC_CFG;

typedef struct __attribute__((__packed__))
{
	U32  m_dSataLgAddr;//SSD有部分保留的逻辑空间（没有上报给host），用来存日志类的信息，如Sata协议定义的SMART信息等
	U32  m_dResLgAddr;
	U16  m_wSataLgSctCnt;
	U16  m_wResLgSctCnt;
	U8   m_bReserve[452];
	T_G0_VMC_CFG m_tG0VmcCfg;
}T_SATA_UDP;

#endif

#endif
