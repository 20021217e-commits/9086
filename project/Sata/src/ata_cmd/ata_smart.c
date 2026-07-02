// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司
//   
//  文件名称：    ata_smart.c
//  文件标识：
//  内容摘要：    实现ATA8 SMART Feature Set管理
//  其它说明：
//  项    目：    SG9081
//  作    者：    WILLIAM
//  完成日期：    2015年07月07日
//   
//  修改记录 1：
//        修改日期：
//        版 本 号：
//        修 改 人：
//        修改内容：
//  修改记录 2：
// **********************************************************************
#include "ata_typedef.h"
#include "ata_smart.h"
#include "sata_pio_in.h"
#include "sata_pio_out.h"
#include "sata_dma_in.h"
#include "sata_dma_out.h"
#include "sata_misc.h"
#include "ata_general.h"
#include "dsata.h"
#include "debug.h"
#include "task_manage.h"
#include "ata_gpl.h"
#include "uitility.h"
#include "ata_identify.h"

U8 abSmartMask[]={0,0,0,0,0,0,0,0};
#ifdef __SMART_1__
U8 abSmartMask[]={0,0,0,0,0,0,0,0};
#if(SATA_LOG_ENABLE == 1)

__align(8) UN_SMART_DATA_STRUCTURE unSmartDS_C __section(".SMART_DATA")={
#else
#ifdef __ALG_FORMAL
__align(8) const UN_SMART_DATA_STRUCTURE unSmartDS_C __section(".SMART_DATA")={
#else
__align(8) const UN_SMART_DATA_STRUCTURE unSmartDS_C ={
#endif
#endif
     0x0001,
{   
     //------------ID=01  Read Error Rate  
     {
     0x01,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=02 Throughput Performance
     {
     0x02,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     }, 
     //------------ID=03 Spin Up Time
     {
     0x03,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     }, 
     //------------ID=05 Reallocated Sector Count
     {
     0x05,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=07 Seek Error Rate
     {
     0x07,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     }, 
     //------------ID=08 Seek Time performance
     {
     0x08,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     }, 
     //------------ID=09 Power-On hours Count
     {
     0x09,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=0A Spin Retry Count
     {
     0x0A,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     }, 
     //------------ID=0C Drive Power Cycle Count
     {
     0x0C,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A0 Uncorrectable sector count when r/w
     {
     0xA0,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A1 Number of valid spare block
     {
     0xA1,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A3 Orignal Bad Block 
     {
     0xA3,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A4 TOTAL Erase Count
     {
     0xA4,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A5 Maximum erase count
     {
     0xA5,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },

     //------------ID=A6 Minimum erase count
     {
     0xA6,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },

     //------------ID=A7 Average erase count
     {
     0xA7,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },

     //------------ID=A8 SATA PHY Error Count
     {
     0xA8,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A9 Bad Block Count
     {
     0xA9,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=AA Maximum Bad Block Count
     {
     0xAA,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=AF Bad Cluster Table Count
     {
     0xAF,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=B7 Sata DownShift error count(掉速运行)
     {
     0xB7,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C0 Unexpected Power Loss Count
     {
     0xC0,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C2 Temperature
     {
     0xC2,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C3 Read retry number
     {
     0xC3,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C4  Reallocated event Count
     {
     0xC4,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C7 Sata PHY retry count(CRC error)
     {
     0xC7,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=E9 Write Sector Count to Nand
     {
     0xE9,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=EA Read Sector Count from Nand
     {
     0xEA,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=F1 Write Sector Count
     {
     0xF1,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=F2 Read Sector Count
     {
     0xF2,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
},
    0x00,
    //Off-line data collection status
    0x00,
    //Self-test execution status byte

    0x0078,
    //Total time in seconds to complete off-line data collection activity
    0x00,
    //vs

    0x5D,//Off-line data collection capability
    /*
     *  Bit Description
        7 Reserved
        6 SELECTIVE SELF-TEST IMPLEMENTED bit – If this bit is cleared to zero, the device does not implement the
            Selective self-test routine. If this bit is set to one, the device implements the Selective self-test routine.
        5 CONVEYANCE SELF-TEST IMPLEMENTED bit – If this bit is cleared to zero, the device does not implement the
            Conveyance self-test routines. If this bit is set to one, the device implements the Conveyance self-test
            routines.
        4 SELF-TEST IMPLEMENTED bit – If this bit is cleared to zero, the device does not implement the Short and
            Extended self-test routines. If this bit is set to one, the device implements the Short and Extended
            self-test routines.
        3 OFF-LINE READ SCANNING IMPLEMENTED bit – If this bit is cleared to zero, the device does not support
            off-line read scanning. If this bit is set to one, the device supports off-line read scanning.
        2 ABORT/RESTART OFF-LINE BY HOST bit – If this bit is set to one, then the device shall abort all off-line data
            collection activity initiated by a SMART EXECUTE OFF-LINE IMMEDIATE command upon receipt of a
            new command within 2 seconds of receiving the new command. If this bit is cleared to zero, the device
            shall suspend off-line data collection activity after an interrupting command and resume off-line data
            collection activity after some vendor-specified event.
        1 Vendor specific.
        0 EXECUTE OFF-LINE IMMEDIATE IMPLEMENTED bit – If this bit is set to one, then the SMART EXECUTE
            OFF-LINE IMMEDIATE command is implemented by this device. If this bit is cleared to zero, then the
            SMART EXECUTE OFF-LINE IMMEDIATE command is not implemented by this device.
     * */

    0x0002,
    //SMART capability

    0x01,
    //Error logging capability
    0x00,//vs
    0x02,
    //Short self-test routine recommended polling time (default:1)
    0x0A,
    //Extended self-test routine recommended polling time (default:1),if FF using 375,376
    0x00,
    //Conveyance self-test routine recommended polling time (default:0)
    {0x0000},
    //Extended self-test routine recommended polling time (default:1) w375,376
};
#endif

#ifdef __SMART_2__
#if(SATA_LOG_ENABLE == 1)
__align(8)  UN_SMART_DATA_STRUCTURE unSmartDS_C __section(".SMART_DATA")={
#else
#ifdef __ALG_FORMAL
__align(8) UN_SMART_DATA_STRUCTURE unSmartDS_C __section(".SMART_DATA")={
#else
__align(8) UN_SMART_DATA_STRUCTURE unSmartDS_C ={
#endif
#endif
     0x0001,
{
     //------------ID=01  Read Error Rate
     {
     0x01,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=05 Reallocated Sector Count
     {
     0x05,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=09 Power-On hours Count
     {
     0x09,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=0C Drive Power Cycle Count
     {
     0x0C,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A0 Uncorrectable sector count when r/w
     {
     0xA0,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A1 Number of valid spare block
     {
     0xA1,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A3 Orignal Bad Block
     {
     0xA3,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A4 TLC TOTAL Erase Count
     {
     0xA4,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A5 Maximum erase count
     {
     0xA5,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },

     //------------ID=A6 Minimum erase count
     {
     0xA6,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },

     //------------ID=A7 Average erase count
     {
     0xA7,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },

     //------------ID=A8 SATA PHY Error Count
     {
     0xA8,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=A9 Bad Block Count
     {
     0xA9,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=AF
     {
     0xAF,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=B0
     {
     0xB0,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=B1
     {
     0xB1,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=B2
     {
     0xB2,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=B5
     {
     0xB5,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=B6
     {
     0xB6,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C0 Unexpected Power Loss Count
     {
     0xC0,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C2 Temperature
     {
     0xC2,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C3 Read retry number
     {
     0xC3,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C4  Reallocated event Count
     {
     0xC4,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=C5 Current Pending Sector Count
      {
      0xC5,
      {0x0032},
      0x64,
      0x64,
      {0x00,0x00,0x00,0x00,0x00,0x00},
      0x00
      },
      //------------ID=C6 Smart Off-line Scan Uncorrectable Error Count
      {
      0xC6,
      {0x0032},
      0x64,
      0x64,
      {0x00,0x00,0x00,0x00,0x00,0x00},
      0x00
      },
     //------------ID=C7 Sata PHY retry count(CRC error)
     {
     0xC7,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=E8
     {
     0xE8,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=F1 Write Sector Count
     {
     0xF1,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=F2 Read Sector Count
     {
     0xF2,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
     //------------ID=F5
     {
     0xF9,
     {0x0032},
     0x64,
     0x64,
     {0x00,0x00,0x00,0x00,0x00,0x00},
     0x00
     },
},
    0x00,
    //Off-line data collection status
    0x00,
    //Self-test execution status byte

    0x0078,
    //Total time in seconds to complete off-line data collection activity
    0x00,
    //vs

    0x5D,//Off-line data collection capability
    /*
     *  Bit Description
        7 Reserved
        6 SELECTIVE SELF-TEST IMPLEMENTED bit – If this bit is cleared to zero, the device does not implement the
            Selective self-test routine. If this bit is set to one, the device implements the Selective self-test routine.
        5 CONVEYANCE SELF-TEST IMPLEMENTED bit – If this bit is cleared to zero, the device does not implement the
            Conveyance self-test routines. If this bit is set to one, the device implements the Conveyance self-test
            routines.
        4 SELF-TEST IMPLEMENTED bit – If this bit is cleared to zero, the device does not implement the Short and
            Extended self-test routines. If this bit is set to one, the device implements the Short and Extended
            self-test routines.
        3 OFF-LINE READ SCANNING IMPLEMENTED bit – If this bit is cleared to zero, the device does not support
            off-line read scanning. If this bit is set to one, the device supports off-line read scanning.
        2 ABORT/RESTART OFF-LINE BY HOST bit – If this bit is set to one, then the device shall abort all off-line data
            collection activity initiated by a SMART EXECUTE OFF-LINE IMMEDIATE command upon receipt of a
            new command within 2 seconds of receiving the new command. If this bit is cleared to zero, the device
            shall suspend off-line data collection activity after an interrupting command and resume off-line data
            collection activity after some vendor-specified event.
        1 Vendor specific.
        0 EXECUTE OFF-LINE IMMEDIATE IMPLEMENTED bit – If this bit is set to one, then the SMART EXECUTE
            OFF-LINE IMMEDIATE command is implemented by this device. If this bit is cleared to zero, then the
            SMART EXECUTE OFF-LINE IMMEDIATE command is not implemented by this device.
     * */

    0x0002,
    //SMART capability

    0x01,
    //Error logging capability
    0x00,//vs
    0x02,
    //Short self-test routine recommended polling time (default:1)
    0x0A,
    //Extended self-test routine recommended polling time (default:1),if FF using 375,376
    0x00,
    //Conveyance self-test routine recommended polling time (default:0)
    {0x0000},
    //Extended self-test routine recommended polling time (default:1) w375,376
};
#endif

const U8 bSmartIDList[]={
        0x01,0x05,0x09,0x0c,
        0xA0,0xA1,0xA3,0xA4,
        0xA5,0xA6,0xA7,0xA8,
        0xA9,0xAF,0xB0,0xB1,
        0xB2,0xB5,0xB6,0xC0,
        0xC2,0xC3,0xC4,0xC5,
        0xC6,0xC7,0xE8,0xF1,
        0xF2,0xF9
};

#if (SATA_LOG_ENABLE == 1)
const U32 arrdwShortSelfLba[]={
        0,
        0x55,
        0x2255,
        0x3366,
        0x55aa,
        0xaa55,
        0x55,
        0x2255,
};
const U32 arrdwExtSelfLba[]={
        0,
        0x55,
        0x2255,
        0x3366,
        0x55aa,
        0xaa55,
        0x55,
        0x2255,
};
const U16 CMD_NUMS[]={sizeof(arrdwShortSelfLba)/4,sizeof(arrdwShortSelfLba)/4,sizeof(arrdwExtSelfLba)/4,0,5};

T_SEL_SELFLOG __align(8)  tSmartSelSelfLog  __section(".satalogctx");

//SMART 相关的LOG的读写
//SMART SELF 同步方式执行，异步方式执行两种模式
//同步模式：直接调用FTL接口 读写数据
//异步模式：状态执行任务，通过回调函数执行
//优先级：SMART ERROR LOG > GPL LOG > SMART SEIF TEST
/*
    抢占任务的时候会出现这样的情况：
    FTL正在准备执行低优先级的任务，已经获取了相关参数，这个时候来一个优先级更高的任务
    如果被抢占了，待写入的数据缓冲区会被冲掉，最终写入的数据有问题。
    所以执行任务的时候要屏蔽SATA中断
*/

// *****************************************************************************
// 函数名称: Build_CSELTable
// 函数功能: 根据 出错的最近六个 CMD 重建ComprehensiveSmartErrorLog表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Build_CSELTable(U8* pTable)
{

    //根据相关变量，重建SAMRT表格数据，准备写入FLASH中
    T_COMP_ERROR *ptLog=(T_COMP_ERROR *)pTable;
    T_CMD_CTX* ptCmd;
    ADDR_MODE enAddrMode;
    U8 readidx=((tSataDC.bRdIdx- (U8)(tSataDC.bCmdCnt==0))&0x1F);
    U8 I;

    ptCmd=&tSataDC.tCmdList[readidx];//当前正在执行的命令
    enAddrMode=tSataDC.arrenAddrMode[readidx];
    if(enAddrMode != MODE_28BIT)
    {
        //出错的命令不是28bit命令,不作记录
        return;
    }

    ptLog->m.bVersion=0x01;
    
    if(ptLog->m.bIndex == MAX_SMART_ERROR_LOG_IDX)
    {
        ptLog->m.bIndex=0;
    }

    for(I=0;I<5;I++)
    {
        if(readidx >= 4 || TRUE == tSataDC.IsOvLap)
        {
            U8 idx=(readidx-4+I)&0x1F;
            enAddrMode=tSataDC.arrenAddrMode[idx];
            ptCmd=&tSataDC.tCmdList[idx];
            if(enAddrMode != MODE_28BIT)
            {
                //寻找下一个
                continue;
            }
            
        }
        else
        {
            //reset 后新命令不够5个
            if(I >= (4-readidx))
            {
                U8 idx=I - (4-readidx);
                enAddrMode=tSataDC.arrenAddrMode[idx];
                ptCmd=&tSataDC.tCmdList[idx];
                if(enAddrMode != MODE_28BIT)
                {
                    //寻找下一个
                    continue;
                }
                //找到合法的给表格赋值
            }
            else
            {
                //寻找下一个
                //不够的要清零
                MemorySet((U8*)&ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I],0,sizeof(T_COMP_CDS));
                continue;
            }
            
        } 
        if(enAddrMode == MODE_28BIT)
        {
            //只有该项是合法的命令才会给表格赋值
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].dTimestamp=ptCmd->dTimeStamp;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bControl=ptCmd->h2d.dw3.bits.bCtrl;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bCmd=ptCmd->h2d.dw0.bits.Cmd;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bCount=ptCmd->h2d.dw3.bits.bCount;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bDevice=ptCmd->h2d.dw1.bits.bDevice;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bFeature=ptCmd->h2d.dw0.bits.bFeatures_L;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bLba_low=ptCmd->h2d.dw1.bits.bLBA_low;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bLba_mid=ptCmd->h2d.dw1.bits.bLBA_mid;
            ptLog->m.tDS[ptLog->m.bIndex].tCmdDS[I].bLba_high=ptCmd->h2d.dw1.bits.bLBA_high;
        }        
    }
    
    ptCmd=&tSataDC.tCmdList[readidx];//当前正在执行的命令
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bReserved=0x01;
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bError=tSataDC.tSyncEvent.bError;//todo:要或上具体的错误类型
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bCount=ptCmd->h2d.dw3.bits.bCount;
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bLba_low=ptCmd->h2d.dw1.bits.bLBA_low;
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bLba_mid=ptCmd->h2d.dw1.bits.bLBA_mid;
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bLba_high=ptCmd->h2d.dw1.bits.bLBA_high;
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bDevice=ptCmd->h2d.dw1.bits.bDevice;
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bStatus=tSataDC.tSyncEvent.bStatus;//具体类型具体定义
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.wLifeTimestamp=tSataDC.dLifePowerOnTime;
    
    /*
        
        Value a State
        x0h Unknown
        x1h Sleep
        x2h Standby
        x3h Active/Idle
        x4h Executing SMART off-line or self-test
        x5h..xAh Reserved
        xBh..xFh Vendor specific
        a The value of x is vendor specific and may be different for each state.
    */
    
    //tSataDC.eDPMSts
    ptLog->m.tDS[ptLog->m.bIndex].tErrorDS.bState=0;//当前的省电模式

    //更新INDEX
    ptLog->m.bIndex++;

    if(ptLog->m.wDevErrCnt < 0xFFFF)
    {
        ptLog->m.wDevErrCnt++;//到了最大值就不再累加
    }
    //update check sum
    ptLog->m.bCheckSum =  (~GetSum(pTable,511)) + 1;
    //INDEX 循环记录

}

//#pragma  O0
// *****************************************************************************
// 函数名称: Build_ECSELTable
// 函数功能: 根据 出错的最近六个 CMD 重建ExtComprehensiveSmartErrorLog表格数据
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Build_ECSELTable(U8* pTable)
{

     //根据相关变量，重建SAMRT表格数据，准备写入FLASH中
    T_EXTCOMP_ERROR *ptLog=(T_EXTCOMP_ERROR *)pTable;
    T_CMD_CTX* ptCmd;
    //ADDR_MODE enAddrMode;
    U8 readidx=((tSataDC.bRdIdx - (U8)(tSataDC.bCmdCnt==0))&0x1F);
    U8 I;

    ptLog->m.bVersion=0x01;

    if(ptLog->m.wIndex == MAX_EXT_SMART_ERROR_LOG_IDX)
    {
        ptLog->m.wIndex=0;
    }

    for(I=0;I<5;I++)
    {
        if(readidx >= 4 || TRUE == tSataDC.IsOvLap)
        {
            U8 idx=(readidx-4+I)&0x1F;
            //enAddrMode=tSataDC.arrenAddrMode[idx];
            ptCmd=&tSataDC.tCmdList[idx];

//            if(enAddrMode != MODE_48BIT)
//            {
//                //寻找下一个
//                continue;
//            }
            
        }
        else
        {
            //reset 后新命令不够5个
            if(I >= (4-readidx))
            {
                U8 idx=I - (4-readidx);
                //enAddrMode=tSataDC.arrenAddrMode[idx];
                ptCmd=&tSataDC.tCmdList[idx];
//                if(enAddrMode != MODE_48BIT)
//                {
//                    //寻找下一个
//                    continue;
//                }
                //找到合法的给表格赋值
            }
            else
            {
                //寻找下一个
                MemorySet((U8*)&ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I],0,sizeof(T_EXTCOMP_CDS));
                continue;
            }
            
        } 
        //if(enAddrMode == MODE_48BIT)
        {
            //只有该项是合法的命令才会给表格赋值
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].dTimestamp=ptCmd->dTimeStamp;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bControl=ptCmd->h2d.dw3.bits.bCtrl;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bCmd=ptCmd->h2d.dw0.bits.Cmd;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bCount=ptCmd->h2d.dw3.bits.bCount;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bCount_H=ptCmd->h2d.dw3.bits.bExpCount;
            
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bDevice=ptCmd->h2d.dw1.bits.bDevice;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bFeature=ptCmd->h2d.dw0.bits.bFeatures_L;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bFeature_H=ptCmd->h2d.dw2.bits.bFeatures_H;
            
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bLba_low=ptCmd->h2d.dw1.bits.bLBA_low;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bLba_mid=ptCmd->h2d.dw1.bits.bLBA_mid;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bLba_high=ptCmd->h2d.dw1.bits.bLBA_high;
            
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bExpLba_low=ptCmd->h2d.dw2.bits.bExpLBA_low;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bExpLba_mid=ptCmd->h2d.dw2.bits.bExpLBA_mid;
            ptLog->m.tDS[ptLog->m.wIndex].tCmdDS[I].bExpLba_high=ptCmd->h2d.dw2.bits.bExpLBA_high;
        }
    }
    
    ptCmd=&tSataDC.tCmdList[readidx];//当前正在执行的出错的命令

    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bError=tSataDC.tSyncEvent.bError;//todo:要或上具体的错误类型
    
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bCount=ptCmd->h2d.dw3.bits.bCount;
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bCount_H=ptCmd->h2d.dw3.bits.bExpCount;
    
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bLba_low=ptCmd->h2d.dw1.bits.bLBA_low;
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bLba_mid=ptCmd->h2d.dw1.bits.bLBA_mid;
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bLba_high=ptCmd->h2d.dw1.bits.bLBA_high;

    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bExpLba_low=ptCmd->h2d.dw2.bits.bExpLBA_low;
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bExpLba_mid=ptCmd->h2d.dw2.bits.bExpLBA_mid;
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bExpLba_high=ptCmd->h2d.dw2.bits.bExpLBA_high;
    
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bDevice=ptCmd->h2d.dw1.bits.bDevice;
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bStatus=tSataDC.tSyncEvent.bStatus;//具体类型具体定义
    
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.wLifeTimestamp=tSataDC.dLifePowerOnTime;
    
    /*
        
        Value a State
        x0h Unknown
        x1h Sleep
        x2h Standby
        x3h Active/Idle
        x4h Executing SMART off-line or self-test
        x5h..xAh Reserved
        xBh..xFh Vendor specific
        a The value of x is vendor specific and may be different for each state.
    */
    
    //tSataDC.eDPMSts
    ptLog->m.tDS[ptLog->m.wIndex].tErrorDS.bState=0;//当前的省电模式
    
    //更新INDEX
    ptLog->m.wIndex++;

    if(ptLog->m.wDevErrCnt < 0xFFFF)
    {
        ptLog->m.wDevErrCnt++;//到了最大值就不再累加
    }

    //update check sum
    ptLog->m.bCheckSum =  (~GetSum(pTable,511)) + 1;
    //INDEX 循环记录
}
//#pragma  O0
// *****************************************************************************
// 函数名称: Build_NCQErrorLogTable
// 函数功能: 根据 出错的NCQ命令保存LOG
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Build_NCQErrorLogTable(U8* pTable)
{
    T_CMD_CTX* ptCmd=&tSataDC.tCmdList[tSataDC.bCmdIdx[tSataDC.tSyncEvent.Tag]];
    
    UART_PRINT("tSataDC.bCmdIdx[bTag]=%d\r\n",tSataDC.bCmdIdx[tSataDC.tSyncEvent.Tag]);
    
    UART_PRINT("unFsmBuff.FIS.h2d.dw0=%#010lx\r\n",ptCmd->h2d.dw0.dAll);
    UART_PRINT("unFsmBuff.FIS.h2d.dw1=%#010lx\r\n",ptCmd->h2d.dw1.dAll);
    UART_PRINT("unFsmBuff.FIS.h2d.dw2=%#010lx\r\n",ptCmd->h2d.dw2.dAll);
    UART_PRINT("unFsmBuff.FIS.h2d.dw3=%#010lx\r\n",ptCmd->h2d.dw3.dAll);
    UART_PRINT("unFsmBuff.FIS.h2d.dw4=%#010lx\r\n",ptCmd->h2d.dw4.dAll);
    /*
        0 Bit Name
        7 NQ bit (see A.14.3)
        6 UNL bit (see A.14.4)
        5 Reserved
        4:0 NCQ TAG field (see A.14.2)
        1 Reserved
        2 STATUS field (see A.14.5)
        3 ERROR field (see A.14.5)
        4 LBA field (7:0) (see A.14.5)
        5 LBA field (15:8) (see A.14.5)
        6 LBA field (23:16) (see A.14.5)
        7 DEVICE field (see A.14.5)
        8 LBA field (31:24) (see A.14.5)
        9 LBA field (39:32) (see A.14.5)
        10 LBA field (47:40) (see A.14.5)
        11 Reserved
        12 COUNT field (7:0) (see A.14.5)
        13 COUNT field (15:8) (see A.14.5)
        14 SENSE KEY field (see A.14.6)
        15 ADDITIONAL SENSE CODE field (see A.14.6)
        16 ADDITIONAL SENSE CODE QUALIFIER field (see A.14.6)
        17..255 Reserved
        256..510 Vendor Specific
        511 Checksum (see A.14.7)
    */
    UART_PRINT("tAsyncBuffer.dArray[0]=%#010lx\r\n",tAsyncBuffer.dArray[0]);
    UART_PRINT("tAsyncBuffer.dArray[1]=%#010lx\r\n",tAsyncBuffer.dArray[1]);
    UART_PRINT("tAsyncBuffer.dArray[2]=%#010lx\r\n",tAsyncBuffer.dArray[2]);
    UART_PRINT("tAsyncBuffer.dArray[3]=%#010lx\r\n",tAsyncBuffer.dArray[3]);
    
    pTable[0]=tSataDC.tSyncEvent.Tag;
    if(tSataDC.tSyncEvent.bNQFlg)
    {
        //none ncq cmd --SDB
        pTable[0]|=(1<<7);//NQ =1
        pTable[2]=(unFsmBuff.FIS.sdb.dw0.bits.Status_Hi<<4)+unFsmBuff.FIS.sdb.dw0.bits.Status_Lo;
        pTable[3]=unFsmBuff.FIS.sdb.dw0.bits.bError;
    }
    else
    {
        pTable[2]=unFsmBuff.FIS.d2h.dw0.bits.bStatus;
        pTable[3]=unFsmBuff.FIS.d2h.dw0.bits.bError;
        
    }
    pTable[4]=ptCmd->h2d.dw1.bits.bLBA_low;
    pTable[5]=ptCmd->h2d.dw1.bits.bLBA_mid;
    pTable[6]=ptCmd->h2d.dw1.bits.bLBA_high;
    pTable[7]=ptCmd->h2d.dw1.bits.bDevice;
    pTable[8]=ptCmd->h2d.dw2.bits.bExpLBA_low;
    pTable[9]=ptCmd->h2d.dw2.bits.bExpLBA_mid;
    pTable[10]=ptCmd->h2d.dw2.bits.bExpLBA_high;
    pTable[12]=ptCmd->h2d.dw3.bits.bCount;
    pTable[13]=ptCmd->h2d.dw3.bits.bExpCount;
    pTable[511]= (~GetSum(pTable,511)) + 1;
    UART_PRINT("tAsyncBuffer.dArray[0]=%#010lx\r\n",tAsyncBuffer.dArray[0]);
    UART_PRINT("tAsyncBuffer.dArray[1]=%#010lx\r\n",tAsyncBuffer.dArray[1]);
    UART_PRINT("tAsyncBuffer.dArray[2]=%#010lx\r\n",tAsyncBuffer.dArray[2]);
    UART_PRINT("tAsyncBuffer.dArray[3]=%#010lx\r\n",tAsyncBuffer.dArray[3]);
    UART_PRINT("tAsyncBuffer.dArray[124]=%#010lx\r\n",tAsyncBuffer.dArray[124]);
    UART_PRINT("tAsyncBuffer.dArray[125]=%#010lx\r\n",tAsyncBuffer.dArray[125]);
    UART_PRINT("tAsyncBuffer.dArray[126]=%#010lx\r\n",tAsyncBuffer.dArray[126]);
    UART_PRINT("tAsyncBuffer.dArray[127]=%#010lx\r\n",tAsyncBuffer.dArray[127]);
}
// *****************************************************************************
// 函数名称: WriterSmartErrorLogsRouter
// 函数功能: 写所有的smart error log 到FLASH 中的状态机 --同步接口
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void WriterSmartErrorLogsRouter(CMD_STATUS eStatus,void *ptTaskParam)
{
    //函数内部状态机
    //准备数据缓冲区，DMA缓冲区不一定有效，要分多次写，用SATA自己的缓冲区，分多次写入LOG
    //除非有BUFF使用的互斥机制，（加锁）
    //利用DMA buff的话会加快写的过程
    //TODO:If the command data structure represents a hardware reset,content of byte n shall be FFh
    //COMRESET 也需要加入LOG中
    static U8 bState=0;//next sate
    U32 addr=(U32)tSyncBuffer.dArray;
    U32 lba_offset =EXT_COMPH_SMART_ERROR_LOG_OFFSET;  
    U8 size=1;
    U8 bDir=DIR_OUT;
    UART_PRINT("Enter func=<%s>[file=%s,line=%d] [*********bState=%d]\r\n",__func__,__MODULE__,__LINE__,bState);
    NVIC_DisableIRQ(IRQ_SC);

    if(eStatus < CMD_IDLE)
    {
        //TODO:出现异常？
        PRINT_TXT("E:WRSMTEL");//WRITE SMART ERR LOG FAIL
        goto error_handler;
    }
    
    if(eStatus == CMD_IDLE)
    {
        //IDLE 状态下必须bState=0，有新的写任务请求，状态机重新开始写log
        bState=0;
    }

    switch(bState)
    {
        case 0://write NCQ Error Log
            if(tSataDC.bIsNcqCmd)
            {
                 //保存NCQ ERROR LOG
                 Mem32SetVal((U32*)addr,0U,128U);//清除数据
                 Build_NCQErrorLogTable((U8*)addr);
                 lba_offset =NCQ_ERROR_LOG_OFFSET;
                 bState=1;
                 break;
            }
            //非NCQ 命令出错，不保存NCQ error log
            
        case 1: //read Extended Comprehensive SMART Error log
            bDir=DIR_IN;
            lba_offset =EXT_COMPH_SMART_ERROR_LOG_OFFSET;
            bState=2;

            break;
        case 2://update Extended Comprehensive SMART Error log
            //tSataDC.tSyncEvent.dParam[3]+=(tSataDC.tSyncEvent.dParam[1]-tSataDC.tSataMI.dVirtualLbaStartAddr)<<9;//对于读操作，修正数据所在的偏移
            //Mem32Copy((U32 *)&tAsyncBuffer,(U32*)(tSataDC.tSyncEvent.dParam[3]),sizeof(tAsyncBuffer)>>2);//读到的历史数据保存到缓冲区
            Build_ECSELTable((U8*)addr);//历史数据中更新记录
            lba_offset =EXT_COMPH_SMART_ERROR_LOG_OFFSET;
            bState=3;
            break;
        
        case 3://read Comprehensive SMART Error log 
            bDir=DIR_IN;
            lba_offset =COMPH_SMART_ERROR_LOG_OFFSET;  
            bState=4;
            break;
        case 4://update Comprehensive SMART Error log
            //tSataDC.tSyncEvent.dParam[3]+=(tSataDC.tSyncEvent.dParam[1]-tSataDC.tSataMI.dVirtualLbaStartAddr)<<9;//对于读操作，修正数据所在的偏移
            //Mem32Copy((U32 *)&tAsyncBuffer,(U32*)(tSataDC.tSyncEvent.dParam[3]),sizeof(tAsyncBuffer)>>2);//读到的历史数据保存到缓冲区
            Build_CSELTable((U8*)addr);//历史数据中更新记录
            lba_offset =COMPH_SMART_ERROR_LOG_OFFSET;
            bState=5;
            break;
        default:
            bState=0xFF;
            break;
    }
    if(bState != 0xFF)
    {
        //FTL初始化完成,发起读取LOG任务事件
        tSataDC.tSyncEvent.bOpcode=OP_DI_DATA;
        tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
        tSataDC.tSyncEvent.dParam[0]=bDir;  //方向
        tSataDC.tSyncEvent.dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + (lba_offset);//起始LBA
        ERROR_PRINT("bDir=%d,LBA=%#010lx\r\n",bDir,tSataDC.tSyncEvent.dParam[1]);
        tSataDC.tSyncEvent.dParam[2]=size;
        tSataDC.tSyncEvent.dParam[3]=addr;
        tSataDC.tSyncEvent.pfEventCompleteCb=WriterSmartErrorLogsRouter;//读LOG任务完成事件回调
        //PRINT_HEX("L:",tSataDC.tSyncEvent.dParam[1]);
        //PRINT_HEX("BUFFADDR=",tSataDC.tSyncEvent.dParam[3]);
    }
    else
    {
        bState=0;

        if(tSataDC.tSyncEvent.bStatus != 0x7F)
        {
            //写LOG 过程中不回D2H，直到写完ERROR log再回D2H。
            //非IFS中断才需要额外回复D2H
            if(!tSataDC.bIsNcqCmd)
            {
                SATA_SendStatus(tSataDC.tSyncEvent.bStatus, tSataDC.tSyncEvent.bError, tSataDC.tSyncEvent.bI);//回复D2H
            }
            else
            {
                if(tSataDC.tSyncEvent.bStatus != 0)
                {
                     UART_PRINT("tSataDC.tSyncEvent.bStatus != 0!!!\r\n");
                    //none error state
                    //NCQ状态下 处理方式不一样
                    DSATA_TrigeSendFis(D2H_FIS);
                    DSATA_EnableRxFis();
                    UART_PRINT("tSataDC.unDevTransSts.eDFPDMAQ=%d\r\n",tSataDC.unDevTransSts.eDFPDMAQ);
                    UART_PRINT("tSataDC.eTransType=%d\r\n",tSataDC.eTransType);
                }
                else
                {
                     UART_PRINT("tSataDC.tSyncEvent.bStatus == 0!!!\r\n");
                    //ERROR state  --sactive 已经完成的置1，未完成的置0
                    DSATA_TrigeSendFis(SDB_FIS);
                    DSATA_EnableRxFis();
                }
            }
        }
        tSataDC.tSyncEvent.bOpcode=OP_NONE_EVENT;
        tSataDC.tSyncEvent.pfEventCompleteCb=NULL;
        
        //同步任务优先级最高
        if(tSataLogCtx.bIsGplRunning)
        {
             WriterGPLLogsRouter(CMD_IDLE,NULL);//唤醒之前被抢占的任务
        }
        else
        {
            if(tSataLogCtx.bIsSelfTestRunning)
            {
                //开启self test
                SmartSelfTestRouter(CMD_IDLE,NULL);//唤醒之前被抢占的任务
            }
        }
        NVIC_EnableIRQ(IRQ_SC);
        ERROR_PRINT("Save log success!\r\n");
        PRINT_TXT("SELD");//SATA_SaveErrorLog DONE
    } 
    return;
error_handler:
    //终止任务
    tSataDC.tSyncEvent.bOpcode=OP_NONE_EVENT;
    tSataDC.tSyncEvent.pfEventCompleteCb =NULL;
    NVIC_EnableIRQ(IRQ_SC);
    ERROR_PRINT("error_handler!\r\n");

}
// *****************************************************************************
// 函数名称: Build_SmartSelfTestLogTable
// 函数功能:
// 入口参数: pTable 待读写的表格缓冲区
// 出口参数: NONE
// 说    明:
// *****************************************************************************
 void Build_SmartSelfTestLogTable(U8* pTable)
 {
     T_SELF_TEST_LOG *ptLog=(T_SELF_TEST_LOG*)pTable;
     ptLog->wVersion=0x0001;
     if(ptLog->bIndex == MAX_SMART_SELFTEST_IDX)
     {
        ptLog->bIndex=0;
     }

     ptLog->tDesEntrys[ptLog->bIndex].bLba = (tSataLogCtx.qwLbaUDT&0xFF);

     ptLog->tDesEntrys[ptLog->bIndex].bFail=0;
     ptLog->tDesEntrys[ptLog->bIndex].bSelfES=unSmartDS_C.bSelfTestExeSts;
     if((unSmartDS_C.bSelfTestExeSts&0xF0) != 0 || (unSmartDS_C.bSelfTestExeSts&0xF0) != 0xF0)
     {
         //出错记录
         U8 *lba=(U8*)&tSataLogCtx.qwLbaUDT;
         ptLog->tDesEntrys[ptLog->bIndex].bLba28[0] = lba[0];
         ptLog->tDesEntrys[ptLog->bIndex].bLba28[1] = lba[1];
         ptLog->tDesEntrys[ptLog->bIndex].bLba28[2] = lba[2];
         ptLog->tDesEntrys[ptLog->bIndex].bLba28[3] = lba[3];
     }

     ptLog->tDesEntrys[ptLog->bIndex].wTimeStamp=tSataDC.dLifePowerOnTime;

     ptLog->bIndex++;
     //update check sum
     ptLog->bCheckSum =  (~GetSum(pTable,511)) + 1;
 }

 // *****************************************************************************
 // 函数名称: Build_SmartExtSelfTestLogTable
 // 函数功能:
 // 入口参数: pTable 待读写的表格缓冲区
 // 出口参数: NONE
 // 说    明:
 // *****************************************************************************
  void Build_SmartExtSelfTestLogTable(U8* pTable)
  {
     T_EXTSELF_TEST_LOG *ptLog=(T_EXTSELF_TEST_LOG*)pTable;

     ptLog->bVersion=0x0001;
     if(ptLog->wIndex == MAX_EXT_SMART_SELFTEST_IDX)
     {
        ptLog->wIndex=0;
     }

     ptLog->tDesEntrys[ptLog->wIndex].bLba = (tSataLogCtx.qwLbaUDT&0xFF);

     ptLog->tDesEntrys[ptLog->wIndex].bFail=0;
     ptLog->tDesEntrys[ptLog->wIndex].bSelfES=unSmartDS_C.bSelfTestExeSts;
     if((unSmartDS_C.bSelfTestExeSts&0xF0) != 0 || (unSmartDS_C.bSelfTestExeSts&0xF0) != 0xF0)
     {
         //出错记录
         U8 *lba=(U8*)&tSataLogCtx.qwLbaUDT;
         ptLog->tDesEntrys[ptLog->wIndex].bLba48[0] = lba[0];
         ptLog->tDesEntrys[ptLog->wIndex].bLba48[1] = lba[1];
         ptLog->tDesEntrys[ptLog->wIndex].bLba48[2] = lba[2];
         ptLog->tDesEntrys[ptLog->wIndex].bLba48[3] = lba[3];
         ptLog->tDesEntrys[ptLog->wIndex].bLba48[4] = lba[4];
         ptLog->tDesEntrys[ptLog->wIndex].bLba48[5] = lba[5];
     }

     ptLog->tDesEntrys[ptLog->wIndex].wTimeStamp=tSataDC.dLifePowerOnTime;
     ptLog->wIndex++;
     //update check sum
     ptLog->bCheckSum =  (~GetSum(pTable,511)) + 1;
  }

// *****************************************************************************
// 函数名称: SmartSelfTestRouter
// 函数功能: 写所有的smart self test router到FLASH 中的状态机
// 入口参数: eStatus:任务完成状态
//           ptTaskParam:前一个完成的任务相关参数
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void SmartSelfTestRouter(CMD_STATUS eStatus,void *ptTaskParam)
{
    //TODO:进行读、写操作？可不可以写数据？
    static U16 wCount=0;
    static U8 bState=0;//next sate
    static U32 dStartTime=0;
    U32 dLba=0;
    U8 bDir=DIR_OUT;
    U16 wSize=1;
    U32 dAddr=(U32)tAsyncBuffer.dArray; //TODO:具体定下来以后需要传入实际值
    UN_SMART_DATA_STRUCTURE *ptr = (UN_SMART_DATA_STRUCTURE *)&unSmartDS_C;
    T_EVENT* ptEvent=&tSataDC.tAsyncEvent;
    UART_PRINT("Enter func=<%s>[file=%s,line=%d] [*********bState=%d]\r\n ",__func__,__MODULE__,__LINE__,bState);
//    PRINT_TXT("SmartSelfTestRouter IN");
//    if(eStatus < CMD_IDLE)
//    {
//        //TODO:出现异常？
//        PRINT_TXT("E:SMARTSLFT");
//        goto error_handler;
//    }

    //TODO:异步的过程中出现同步任务怎么办？？？？？？？？ --强行终止异步过程
    if(M_CAPTIVE == tSataLogCtx.enSelfTestMode)
    {
        ptEvent=&tSataDC.tSyncEvent;
        dAddr=(U32)tSyncBuffer.dArray;
        if(tSataLogCtx.bIsSelfTestRunning == TRUE)
        {
            //--强行终止异步过程
            tSataDC.tAsyncEvent.bOpcode=OP_NONE_EVENT;
            tSataLogCtx.enGplUpdateEvent=GPL_E_NONE;
            tSataDC.tAsyncEvent.pfEventCompleteCb =NULL;
        }
    }
    else
    {
        tSataLogCtx.bIsSelfTestRunning=TRUE;
    }
    
    if(TRUE == tSataLogCtx.bIsGplRunning)
    {
        //有更高优先级的任务正在进行，本次不创建任务，仅置位任务启动请求
        return;
    }
    
    if(eStatus == CMD_IDLE)
     {
         //IDLE 状态下必须bState=0，有新的写任务请求，重新开始流程
         bState=0;
         wCount=0;
         dStartTime=tSataDC.dPowerOnTime;
     }

    if(wCount == tSataLogCtx.wMaxCmdNums)//到达最大值
    {
        //PRINT_HEX("end>",wCount);
    	wCount=0;
        bState=0;
        if(M_CAPTIVE == tSataLogCtx.enSelfTestMode)
        {
            //前台模式，self test完成以后再回复D2H
            SATA_SendStatus(0x50,0x00,1);
        }
        ptEvent->bOpcode=OP_NONE_EVENT;
        tSataLogCtx.bIsSelfTestRunning=FALSE;
        ptEvent->pfEventCompleteCb=NULL;//整个任务终结，回调清零，以免触发新的任务
        WriterGPLLogsRouter(CMD_IDLE,NULL);//更新状态
        UART_PRINT("Exit func=<%s>[file=%s,line=%d] [*********bState=%d]\r\n",__func__,__MODULE__,__LINE__,bState);
        return;
    }

    switch(bState)
    {
        case 0:
            wSize=8;
            bState=1;
            //获取下一个任务
            switch(tSataLogCtx.enSelfTestRouteMode)
            {
                case R_SHORT_SELFTEST:
                	 dLba = (U32)( wCount) * (1 << SHIFT_32M_NUM) ;
                	 wSize = 256;//4*2048;//4M
                     ERROR_PRINT("R_SHORT_SELFTEST:dLba=%#010lx,wSize=%#010lx\r\n",dLba,wSize);
                     break;
                case R_EXT_SELFTEST:
                	 dLba = (U32)( wCount) * (1 << SHIFT_32M_NUM) ;
                	 wSize = 512;//16*2048;//16M
                     ERROR_PRINT("R_EXT_SELFTEST:dLba=%#010lx,wSize=%#010lx\r\n",dLba,wSize);
                    break;
                case R_SEL_SELFTEST:
                    //根据HOST 准备的lba 进行测试
                    dLba=tSmartSelSelfLog.Spans[wCount].qwStartingLba;
                    wSize=tSmartSelSelfLog.Spans[wCount].qwEndingLba-tSmartSelSelfLog.Spans[wCount].qwStartingLba;
                    bState=5;
                    ERROR_PRINT("R_SEL_SELFTEST:dLba=%#010lx,wSize=%#010lx\r\n",dLba,wSize);
                    break;
                default:
                    break;
            }
            tSataLogCtx.qwLbaUDT=dLba;
            dAddr=tSataDC.tSataMI.dDmaBuffAddr;
            bDir=DIR_IN;
            break;
        case 1:
            ptr->bOffLineDCSts = 0x03;//Off-line activity in progress
            //单次 self test 结束，更新测试状态
            if(eStatus < CMD_IDLE)
            {
                //self test error
                //记录测试项到LOG--2个阶段
                ptr->bSelfTestExeSts =0x40;//记录异常
                ptr->wTotalTime=(tSataDC.dPowerOnTime-dStartTime)/1000;
            }
            else
            {
                //TODO:正确报告
                ptr->bSelfTestExeSts = 0xF0;//记录异常
                //PRINT_HEX("ExeSts1>",ptr->bSelfTestExeSts);
                U8 percent=((wCount+1)*10)/tSataLogCtx.wMaxCmdNums;
                //PRINT_HEX("percent>",percent);
                ptr->bSelfTestExeSts |= (10-percent);
                if (percent == 10)
                {
                	 ptr->bOffLineDCSts = 0x02;//Off-line activity in progress
                	 ptr->bSelfTestExeSts = 0x00;//结束
                	 ptr->wTotalTime=(tSataDC.dPowerOnTime-dStartTime)/1000;
                }
                //PRINT_HEX("ExeSts2>",ptr->bSelfTestExeSts);
            }

            {
                bDir=DIR_IN;
                wSize=1;
                if(ptEvent->dParam[1] <= 0xFFFFFFF)
                {
                    //更新 SMART_SELFTEST_LOG之前先读出旧的信息
                    //28BIT LBA
                    dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + SMART_SELFTEST_LOG_OFFSET;//起始LBA
                    bState=2;//下一阶段，写28 LOG
                }
                else
                {
                    //48BIT LBA
                    dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + EXT_SMART_SELFTEST_LOG_OFFSET;//起始LBA
                    bState=4;//下一阶段，写48 LOG
                }

            }
            
            break;
        case 2:
            if(eStatus < CMD_IDLE)
            {
                //TODO:出现异常？
                goto error_handler;
            }
            //写之前build SMART_SELFTEST_LOG data
            Build_SmartSelfTestLogTable((U8*)dAddr);
            bDir=DIR_OUT;//更新 EXT_SMART_SELFTEST_LOG
            dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + SMART_SELFTEST_LOG_OFFSET;//起始LBA
            wSize=1;
            bState=3;//下一阶段，读48 LOG
            break;
        case 3:
            bDir=DIR_IN;//更新 EXT_SMART_SELFTEST_LOG之前先读出旧的信息
            dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + EXT_SMART_SELFTEST_LOG_OFFSET;//起始LBA
            wSize=1;
            bState=4;//下一阶段，写48 LOG
            break;
        case 4:
            //写之前build EXT_SMART_SELFTEST_LOG data
            Build_SmartExtSelfTestLogTable((U8*)dAddr);
            bDir=DIR_OUT;//更新 SMART_SELFTEST_LOG
            dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + EXT_SMART_SELFTEST_LOG_OFFSET;//起始LBA
            wSize=1;
            bState=0;//本次test 结束，取下一个LBA
            wCount++;
            break;
        case 5:
            //更新SMART_SEL_SELFTEST_LOG 直接写入FLASH
            tSmartSelSelfLog.qwCurLbaUDT=ptEvent->dParam[1];
            tSmartSelSelfLog.wCurSpanUDT=wCount;
            dAddr=(U32)&tSmartSelSelfLog;

            bDir=DIR_OUT;//更新 SELE_SELFTEST_LOG_OFFSET
            dLba=tSataDC.tSataMI.dVirtualLbaStartAddr + SELE_SELFTEST_LOG_OFFSET;//起始LBA
            wSize=1;
            bState=0;//本次test 结束，取下一个LBA
            wCount++;
            break;//update selective self test log
        default:
            break;
    }

    //同步异步自适应
    ptEvent->bOpcode=OP_DI_DATA;
    ptEvent->eStatus=CMD_ONGOING;
    ptEvent->dParam[0]=bDir;  //方向
    ptEvent->dParam[1]=dLba;//起始LBA 读某些LBA
    ptEvent->dParam[2]=wSize;
    ptEvent->dParam[3]=dAddr;
    ptEvent->pfEventCompleteCb=SmartSelfTestRouter;//读LOG任务完成事件回调
    //PRINT_HEX("LBA>",dLba);
    //PRINT_HEX("SIZE>",wSize);
    //PRINT_TXT("SmartSelfTestRouter OUT");
    return;

error_handler:
    ptr->bOffLineDCSts = 0x06;//Off-line data collection activity was aborted by the device with a fatal error
    ptr->bSelfTestExeSts =0x30;//报告异常
    ptEvent->bOpcode=OP_NONE_EVENT;
    ptEvent->pfEventCompleteCb =NULL;

    //TODO:重传？
}

// *****************************************************************************
// 函数名称: Overlay_SmartTable
// 函数功能: 根据 量产工具配置修正SMART表格数据
// 入口参数:
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void Overlay_SmartTable()
{
    //修改上报的SMART 内容
    UN_SMART_DATA_STRUCTURE *pUnSmart =(UN_SMART_DATA_STRUCTURE*)&unSmartDS_C;
    if(abSmartMask[ID_C7] == T_ENABLE)
    {
        MemorySet((U8*)&pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT],0,sizeof(T_ATTRI_VALUE));
    }
    else if(abSmartMask[ID_C7] == T_DISABLE)
    {
        //do not report C7
        pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT].bID=0xD7;//改写成D7
    }
    else
    {
        U32 val=0;//FIX to 0
        MemoryCopy((U8*)pUnSmart->atSmtAttrData[SATA_CRC_ERR_CNT].abRawData,(U8*)&val,4);
    }

    if(tSataDC.tSataMI.tMpCfg.bits.T_FIX)
    {
        *(U8*)pUnSmart->atSmtAttrData[TEMPERATURE].abRawData=tSataDC.tSataMI.tMpCfg.bits.T_VAL;
    }
    else
    {
        if(tSataLogCtx.bCurrentTemp >= tSataDC.tSataMI.tMpCfg.bits.T_MAX)
        {
            *(U8*)pUnSmart->atSmtAttrData[TEMPERATURE].abRawData=tSataDC.tSataMI.tMpCfg.bits.T_MAX;
        }
    }
    pUnSmart->bCheckSum=(~GetSum((U8*)pUnSmart,511)) + 1;
}

#endif
#ifndef __BOOT_CODE__
// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD0_ReadData
// 函数功能: SMART READ DATA - B0h/D0h, PIO Data-In
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD0_ReadData(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
    if(!tSataLogCtx.bSmartEn)
    {
        return CMD_ABORT;
    }
#endif
    SATA_ChangeState(PIO_DATA_IN,0); 
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD2_EnDisAttrAutoSave
// 函数功能: SMART ENABLE/DISABLE ATTRIBUTE AUTOSAVE - B0h/D2h, Non-Data
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD2_EnDisAttrAutoSave(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
    if(!tSataLogCtx.bSmartEn)
    {
        return CMD_ABORT;
    }
#endif
    SATA_ChangeState(NONE_DATA,0); 
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD4_ExeOffLineImmediate
// 函数功能: SMART EXECUTE OFF-LINE IMMEDIATE - B0h/D4h, Non-Data
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD4_ExeOffLineImmediate(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
    if(!tSataLogCtx.bSmartEn)
    {
        return CMD_ABORT;
    }
#endif
    SATA_ChangeState(NONE_DATA,0); 

#if (SATA_LOG_ENABLE == 1)

    U8 subcmd=unFsmBuff.FIS.h2d.dw1.bits.bLBA_low;

    if(subcmd == 0x7F)
    {
        //终止所有self 操作
        tSataLogCtx.enSelfTestRouteMode=R_UNDIFINE;
        tSataDC.tAsyncEvent.bOpcode=OP_NONE_EVENT;
        tSataLogCtx.enGplUpdateEvent=GPL_E_NONE;
        tSataDC.tAsyncEvent.pfEventCompleteCb =NULL;
        UN_SMART_DATA_STRUCTURE *ptr = (UN_SMART_DATA_STRUCTURE *)&unSmartDS_C;
        ptr->bOffLineDCSts = 0x05;//Off-line data collection activity was aborted by an interrupting command from host.
        ptr->bSelfTestExeSts =0x01;//The self-test routine was aborted by the host
        tSataLogCtx.enSelfTestMode=M_ABORT;
    }
    else
    {
        if((subcmd & 0x80) != 0)
        {
            //captive 模式
            tSataLogCtx.enSelfTestMode=M_CAPTIVE;
        }
        else
        {
            tSataLogCtx.enSelfTestMode=M_OFF_LINE;
        }

        tSataLogCtx.enSelfTestRouteMode=(EN_ROUTE_MODE)(subcmd & 0x7F);

        if(tSataLogCtx.enSelfTestRouteMode != R_CONVE_SELFTEST)
        {
            U16 *pwArray =(U16*)&CMD_NUMS[0];
            *(pwArray + 0) = (U16)(SG_DSATA->rTOTALCAPCITY >> SHIFT_32M_NUM);
            *(pwArray + 1) = (U16)(SG_DSATA->rTOTALCAPCITY >> SHIFT_32M_NUM);
            *(pwArray + 2) = (U16)(SG_DSATA->rTOTALCAPCITY >> SHIFT_32M_NUM);
        	tSataLogCtx.wMaxCmdNums = CMD_NUMS[tSataLogCtx.enSelfTestRouteMode];
            ERROR_PRINT("enSelfTestRouteMode=%x, wMaxCmdNums=%x\r\n",tSataLogCtx.enSelfTestRouteMode,tSataLogCtx.wMaxCmdNums);
            SmartSelfTestRouter(CMD_IDLE,NULL);//启动 self test route
        }
        else
        {
            //暂不支持R_CONVE_SELFTEST
            tSataLogCtx.enSelfTestMode=M_ABORT;
            return CMD_ABORT;
        }
    }
#endif
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD5_ReadLog
// 函数功能: SMART READ LOG - B0h/D5h, PIO Data-In
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD5_ReadLog(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    
#if (SATA_LOG_ENABLE == 1)
    {
        U8 bLogAddr=unFsmBuff.FIS.h2d.dw1.bits.bLBA_low;
        if((!tSataLogCtx.bSmartEn) && bLogAddr!=0xE0 && bLogAddr!= 0xE1)
        {
            //SMART DISABLE AND NOT SCT CMD！！
            return CMD_ABORT;
        }
    }
#endif
    
    SATA_ChangeState(PIO_DATA_IN,0); 
    //根据参数换算成自定义的LBA
    eStatus=ATACmd_LogAddr2Lba(&unFsmBuff.FIS.h2d,RO,SL);
    //创建FTL任务直接回复HOST
    ATACmd_CreatePioTask(eStatus,DIR_IN); 
    UART_PRINT("ATACmd_Smart_0xD5_ReadLog!!!\r\n");
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);    
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD6_WriteLog
// 函数功能: SMART WRITE LOG - B0h/D6h, PIO Data-Out
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD6_WriteLog(void)
{
    CMD_STATUS eStatus=CMD_IDLE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
    {
        U8 bLogAddr=unFsmBuff.FIS.h2d.dw1.bits.bLBA_low;
        if((!tSataLogCtx.bSmartEn) && bLogAddr!=0xE0 && bLogAddr!= 0xE1)
        {
            //SMART DISABLE AND NOT SCT CMD！！
            return CMD_ABORT;
        }
    }
#endif
    SATA_ChangeState(PIO_DATA_OUT,0); 
    //根据参数换算成自定义的LBA
    eStatus=ATACmd_LogAddr2Lba(&unFsmBuff.FIS.h2d,WO,SL);
    //创建FTL任务直接回复HOST
    ATACmd_CreatePioTask(eStatus,DIR_OUT);    
    UART_PRINT("ATACmd_Smart_0xD6_WriteLog!!!\r\n");
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);    
    return CMD_DONE;
}

// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD8_EnableOperation
// 函数功能: SMART ENABLE OPERATIONS - B0h/D8h, Non-Data
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD8_EnableOperation(void)
{
    //C24Fh 要不要检查？
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    SATA_ChangeState(NONE_DATA,0); 
    
    /*The state of SMART, either enabled or disabled, shall be preserved 
      by the device during all power and reset events.*/
    
    if((unFsmBuff.FIS.h2d.dw1.bits.bLBA_mid == 0x4F) &&
       (unFsmBuff.FIS.h2d.dw1.bits.bLBA_high == 0xC2))
    {
        DEBUG_PRINT("Exit CMD_DONE func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
            tSataLogCtx.bSmartEn=TRUE;
#endif
        return CMD_DONE;
    }
    else
    {
        DEBUG_PRINT("Exit CMD_ABORT func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
        return CMD_ABORT;
    }
    
}
// *****************************************************************************
// 函数名称: ATACmd_Smart_0xD9_DisableOperation
// 函数功能: SMART DISABLE OPERATIONS - B0h/D9h, Non-Data
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xD9_DisableOperation(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
#if (SATA_LOG_ENABLE == 1)
    if(!tSataLogCtx.bSmartEn)
    {
        return CMD_ABORT;
    }
#endif
    SATA_ChangeState(NONE_DATA,0); 
#if (SATA_LOG_ENABLE == 1)
    tSataLogCtx.bSmartEn=FALSE;
#endif
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_DONE;
}
// *****************************************************************************
// 函数名称: ATACmd_Smart_0xDA_ReturnStatus
// 函数功能: SMART RETURN STATUS - B0h/DAh, NON-DATA
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_0xDA_ReturnStatus(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    SATA_ChangeState(NONE_DATA,0); 
    /*LBA[23:8]
            C24Fh The subcommand specified a captive self-test that has completed
                    ithout error.
            2CF4h The device has detected a threshold exceeded condition
    */
    //检查相关参数
    
//    unFsmBuff.FIS.h2d.dw1.bits.bLBA_mid = 0x4F;
//    unFsmBuff.FIS.h2d.dw1.bits.bLBA_high = 0xC2;

    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_DONE;
}
// *****************************************************************************
// 函数名称: ATACmd_Smart_CheckData
// 函数功能:
// 入口参数: NONE
// 出口参数: 命令有效性检查状态
// 说    明:
// *****************************************************************************
CMD_STATUS ATACmd_Smart_CheckData(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    return CMD_DONE;
}
// *****************************************************************************
// 函数名称: ATACmd_Smart_DmaDataReceived
// 函数功能: DMA数据接收完成事务处理
// 入口参数: NONE
// 出口参数:
// 说    明: 该接口由传输层回调
// *****************************************************************************
void ATACmd_Smart_DmaDataReceived(void)
{
    //数据接收完成
    CMD_STATUS eStatus;
    T_HI_TASK tTask;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus = ATACmd_Smart_CheckData();

    tTask.eStatus=eStatus;
    tSataDC.DOS_TransferReq(REQ_STATUS,&tTask,NULL);//发送状态
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
// *****************************************************************************
// 函数名称: ATACmd_Smart_PioDataReceived
// 函数功能: PIO数据接收完成事务处理
// 入口参数: NONE
// 出口参数:
// 说    明: 该接口由传输层回调
// *****************************************************************************
void ATACmd_Smart_PioDataReceived(void)
{
    //数据接收完成
    CMD_STATUS eStatus;
    T_HI_TASK tTask;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    eStatus = ATACmd_Smart_CheckData();
    
    tTask.eStatus=eStatus;
    tSataDC.DOS_TransferReq(REQ_STATUS,&tTask,NULL);//发送状态
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

void BuildSmartD1Data(UN_SMART_DATA_STRUCTURE* pUnSmart)
{
    U8 IDs=30;
    U8 I=0;
    Mem32SetVal((U32*)pUnSmart,0U,128U);
    pUnSmart->wVS0=0x0001;
    while(IDs--)
    {
        pUnSmart->atSmtAttrData[I].bID=bSmartIDList[I];

//        if((bSmartIDList[I] == 0x05) && abSmartMask[ID_05] == T_DISABLE)
//        {
//            //do not report ID_05
//            pUnSmart->atSmtAttrData[I].bID=0x03;//05必须上报，否则软件无法解析F1F2
//        }

        if((bSmartIDList[I] == 0xC3) && abSmartMask[ID_C3] == T_DISABLE)
        {
            //do not report ID_C3
            pUnSmart->atSmtAttrData[I].bID=0xD3;
        }

        if((bSmartIDList[I] == 0xC4) && abSmartMask[ID_C4] == T_DISABLE)
        {
            //do not report ID_C4
            pUnSmart->atSmtAttrData[I].bID=0xD4;
        }

        if((bSmartIDList[I] == 0xC5) && abSmartMask[ID_C5] == T_DISABLE)
        {
            //do not report ID_C5
            pUnSmart->atSmtAttrData[I].bID=0xD5;
        }

        if((bSmartIDList[I] == 0xC6) && abSmartMask[ID_C6] == T_DISABLE)
        {
            //do not report ID_C6
            pUnSmart->atSmtAttrData[I].bID=0xD6;
        }

        if((bSmartIDList[I] == 0xC7) && abSmartMask[ID_C7] == T_DISABLE)
        {
            //do not report ID_C7
            pUnSmart->atSmtAttrData[I].bID=0xD7;
        }


        pUnSmart->atSmtAttrData[I].unFlag.bThreshold=50;
        I++;
    }
    pUnSmart->bCheckSum=(~GetSum((U8*)pUnSmart,511)) + 1;
}

// *****************************************************************************
// 函数名称: GetSmartDataDone
// 函数功能:  GetSmart 任务结束
// 入口参数: NONE
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void GetSmartDataDone(CMD_STATUS eStatus,void *ptTaskParam)
{
    T_HI_TASK tTask;
    UN_CH_DI unDI;
    tTask.tParam.bTag=CMD_SLOT0;
    //SMART READ DATA - B0h/D0h, PIO Data-In
    eStatus=ATACmd_Smart_0xD0_ReadData();
    unDI.dAll=0xC0;
    DSATA_SetCmdHead(CMD_SLOT0,unDI,1,1,0);
#if (SATA_LOG_ENABLE == 0)
    //DMA1 不能访问ROM 区域，需要拷贝到RAM 区域
    Mem32Copy((U32*)&tAsyncBuffer.wArray[0],(U32*)&unSmartDS_C,128);
    tTask.tParam.dBuffAddr=(U32)&tAsyncBuffer.wArray[0];
#else
    if(unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L == 0xD0)
    {
        tSataLogCtx.bUpdateEn=TRUE;
        tSataLogCtx.enGplUpdateEvent=GPL_E_RESTART;//HOST 读smart信息会触发保存操作
    }

    Build_SmartTable((UN_SMART_DATA_STRUCTURE*)&unSmartDS_C);
    //Overlay_SmartTable();
    tTask.tParam.dBuffAddr=(U32)&unSmartDS_C;
#endif
    tTask.tParam.bDir=DIR_IN;
    tTask.tParam.dSctSize=1;
    tTask.eStatus=eStatus;

    tSataDC.DOS_TransferReq(REQ_TRANSFER,&tTask,NULL);//发送状态
}
#ifndef __BOOT_CODE__
// *****************************************************************************
// 函数名称: ATACmd_SmartFeatureSet
// 函数功能: 实现ATA命令 SMART
// 入口参数: NONE
// 出口参数:
// 说    明: SMART 0xB0
// *****************************************************************************
void ATACmd_SmartFeatureSet(void)
{
    CMD_STATUS eStatus=CMD_DONE;
    T_HI_TASK tTask;
    UN_CH_DI unDI;
    REQ_TYPE eType=REQ_STATUS;
    TransferCompleteCb_t *pfFunc=NULL;
    tTask.tParam.bTag=CMD_SLOT0;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    PRINT_HEX("C:B0/",unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L);

#if(SATA_DCO_ENABLE == TRUE)
    //如果被改成不支持SMART 命令返回error
    if(!DCO_SMART)
    {
        SATA_ChangeState(NONE_DATA,0);
        ATACmd_ReleaseCommand(CMD_ABORT);
        return;
    }
#endif

    switch(unFsmBuff.FIS.h2d.dw0.bits.bFeatures_L)
    {
        case 0xD0:
             TQM_CreateTask(OP_HI_GET_SMART_DATA,&tTQC.tHiTasks[0],GetSmartDataDone);
             return;
        case 0xD1:
            //WINDOS 自带AHCI 驱动，上报SMART需要D1报告SMART支持的ID
            eStatus=ATACmd_Smart_0xD0_ReadData();
            BuildSmartD1Data((UN_SMART_DATA_STRUCTURE*)tSyncBuffer.dArray);
            //send data
            //SMART READ DATA - B0h/D0h, PIO Data-In
            unDI.dAll=0xC0;
            DSATA_SetCmdHead(CMD_SLOT0,unDI,1,1,0);
            tTask.tParam.bDir=DIR_IN;
            tTask.tParam.dSctSize=1;
            tTask.tParam.dBuffAddr=(U32)tSyncBuffer.dArray;
            eType=REQ_TRANSFER;
            break;
        case 0xD2:
            //SMART ENABLE/DISABLE ATTRIBUTE AUTOSAVE - B0h/D2h, Non-Data
            eStatus=ATACmd_Smart_0xD2_EnDisAttrAutoSave();
            break;
        case 0xD4:
            //SMART EXECUTE OFF-LINE IMMEDIATE - B0h/D4h, Non-Data
            eStatus=ATACmd_Smart_0xD4_ExeOffLineImmediate();
#if (SATA_LOG_ENABLE == 1)
            if(M_CAPTIVE==tSataLogCtx.enSelfTestMode)
            {
                //前台模式，完成所有测试以后再回复状态
                return;
            }
#endif
            break;
        case 0xD5:
            //SMART READ LOG - B0h/D5h, PIO Data-In
            if(CMD_DONE == ATACmd_Smart_0xD5_ReadLog())
            return;//这个模式不需要出去发送状态
        case 0xD6:
            //SMART WRITE LOG - B0h/D6h, PIO Data-Out
            if(CMD_DONE == ATACmd_Smart_0xD6_WriteLog())
            return;//这个模式不需要出去发送状态
        case 0xD8:
            //SMART ENABLE OPERATIONS - B0h/D8h, Non-Data
            eStatus=ATACmd_Smart_0xD8_EnableOperation();
            break;
        case 0xD9:
            //SMART DISABLE OPERATIONS - B0h/D9h, Non-Data
            eStatus=ATACmd_Smart_0xD9_DisableOperation();
            break;
        case 0xDA:
            //SMART RETURN STATUS - B0h/DAh, Non-Data
            eStatus=ATACmd_Smart_0xDA_ReturnStatus();
            break;
        default:
            SATA_ChangeState(NONE_DATA,0);//未定义指令，切换到已定义传输模式报告状态
            eStatus=CMD_ABORT;
            break;
    }
    tTask.eStatus=eStatus;
    tSataDC.DOS_TransferReq(eType,&tTask,pfFunc);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#endif
#endif
