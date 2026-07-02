/*********************************************************

  模块名称: PMCP

  版本号:   11.8

*********************************************************/

#ifndef __PMCP_REG_DEF__
#define __PMCP_REG_DEF__



#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus

#ifndef __I
#define   __I     volatile             /*!< Defines 'read only' permissions                 */
#endif

#else

#ifndef __I
#define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif

#endif

#ifndef __O
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#endif

#ifndef __IO
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif



#define SG_PMCP_BASE     (0x5ea000)



typedef struct _SG_PMCP_TypeDef
{
    __IO unsigned long rBASE_ADDR0;
        /*<Offset:0x0>  1.    Bit图统计的基地址；
        2.    搜索算法的基地址，模糊搜索算法目标值的基地址； 
        3.    矩阵运算的第一个基地址；
        4.    矩阵替换中的index数值的起始地址。 
        5.    数据区间搜索功能的区间长度数组的起始地址
        6.    Bit查询及Bit搜索的起始地址*/


    __IO unsigned long rBASE_ADDR1;
        /*<Offset:0x4>  1. 矩阵替换中的目标元素的起始地址（同时也是结果存放的起始地址）；
        2. 最大最小值搜索时的原始数据存放的起始地址；
        3. 区间环形搜索时的搜索起始index。
        4. 数据区间搜索功能的起始数据数组起始地址。
        5. Bit查询功能时的Bit index值    
        6. Bit搜索功能时表示搜索第几个0/1*/


    __IO unsigned long rRESULT_ADDR;
        /*<Offset:0x8>  1.    Bit图统计结果存放起始地址；
        2.    矩阵运算和搜索算法结果存放起始地址；
        3.    memory set的起始地址。
        4.    数据区间搜索功能的结果存放起始地址。*/


    __IO unsigned long rRESULT_CNT;/*<Offset:0xC>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;
                /*需要操作的数据总长度。
                在bit图统计时，矩阵运算以double word为单位，最大为8K-1
                搜索算法以搜索元素个数为单位, 最大8K-1
                模糊搜索表示搜索次数，每次搜索8byte，最大为8K-1
                Memory set的长度，以double word为单位。
                矩阵替换算法中的index数组的长度，以index数组的元素步长为单位
                数据地址区间搜索功能数据区间的个数
                Bit搜索功能时的数据总长度，以double word为单位。*/

            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rOPE_CNT;
        /*<Offset:0x10> 需要操作的数据总长度。
        在bit图统计时，矩阵运算以double word为单位，最大为8K-1
        搜索算法以搜索元素个数为单位, 最大8K-1
        模糊搜索表示搜索次数，每次搜索8byte，最大为8K-1
        Memory set的长度，以double word为单位。
        矩阵替换算法中的index数组的长度，以index数组的元素步长为单位
        数据地址区间搜索功能数据区间的个数
        Bit搜索功能时的数据总长度，以double word为单位。*/


    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long STEP_LEN:4 ;
                /*1. 模糊搜索的步长不受该值影响，固定为8；
                2.步长不支持3byte。
                3.模糊搜索，bit图统计，bit搜索功能的元素步长固定为4byte。
                4.矩阵运算的元素步长也固定为4byte*/

            __IO unsigned long Reserved:1 ;/*保留*/
            __IO unsigned long RESULT_STEP_LENGTH:3 ;
                /*1.结果步长不支持3byte。
                                   2.数据区间搜索功能的结果长度固定为4byte。
                                   3.矩阵运算功能的结果长度固定为4byte。*/

            __IO unsigned long DIFF_BIT_NUM:8 ;
                /*模糊搜索算法中，固件设定允许的不相同的最大bit数，最大值为64.
                低四bit在矩阵运算，最大最小值搜索以及区间搜索时为byte_mask寄存器。该寄存器只有在byte mask功能使能的情况下才有效。*/

            __IO unsigned long Reserved1:16 ;/*保留*/
        }bits;
    }rOPE_CSR ;/*<Offset:0x14> N/A*/

    __IO unsigned long rOPE_MODE;/*<Offset:0x18>  N/A*/

    __IO unsigned long rBIT_MASK;/*<Offset:0x1C>  N/A*/

    __IO unsigned long rMIN_VAL;
        /*<Offset:0x20>  1.区间搜索算法的最小参考值， 32 bits
        2.模糊搜索算法的参考值, 低32bits
        3.最大最小值环形搜索时的搜索起始index。
        4.数据区间搜索功能的目标数据。*/


    __IO unsigned long rMAX_VAL;
        /*<Offset:0x24>  1.区间搜索算法的最大参考值，32 bit
        2.模糊搜索算法的参考值，高32bits. 例如：假设模糊搜索时的一串数据key是0001020304050607，则min_val应该设置为03020100，max_val应该设置为07060504。
        3.固定数的矩阵运算操作中的固定数。
        4.矩阵替换中的目标数，比如如果矩阵步长为bit，则取D0替换相应的元素，步长为byte则取D7~D0替换矩阵元素，依次类推。
        5.Memory set算法中要写入memory的数值。
        6.最大最小值搜索算法的阈值，32bit. 比如：最大值搜索时，设置阈值为0x10，则只搜索小于等于0x10的最大值；最小值搜索时，设置阈值为0x10，则只搜索大于等于0x10的最小值. */


    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:16 ;/*N/A*/
            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rMBD_CFG;/*<Offset:0x28> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long PX_DISABLE:1 ;/*写1屏蔽掉px硬件模块的操作，屏蔽之后px module不会启动，不会操作ram*/
            __IO unsigned long INT_EN:1 ;/*1-enable interrupt;0-disable interrupt*/
            __IO unsigned long SOFT_RESET:1 ;/*N/A*/
            __IO unsigned long DEBUG_INT_EN:1 ;/*1-使能memory带宽控制功能。*/
            __IO unsigned long MBD_CTRL:1 ;/*N/A*/
            __IO unsigned long Reserved:27 ;/*保留*/
        }bits;
    }rPMCP_CTRL;/*<Offset:0x2C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*中断标志，写1清零。*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rINT_STS;/*<Offset:0x30> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*Write 1，starting operation ; Read 1, indicates operation done. Read 0, indicates operation doing*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rOP_START;/*<Offset:0x34> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:16 ;
                /*Bit0: Bit查询时返回的Bit值
                Bit31~0：Bit搜索时返回的搜素目标index
                带索引的内存值设定时计数没有被替换的值*/

            __IO unsigned long Reserved:16 ;/*保留*/
        }bits;
    }rBIT_RESULT;/*<Offset:0x38> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long dVAL:24 ;/*搜索算法结果个数寄存器，包括区间搜索(包括环形)，最值搜索(包括环形)，模糊搜索，数据区间搜索，Bit搜索，带索引的memory copy，区间段信息搜索，bit统计。返回真正搜索到的结果个数值，当要求返回结果个数大于总的数据个数时，搜索操作会在找不到符合条件的元素时提前结束*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rGOT_RESULT_CNT;/*<Offset:0x3C> N/A*/

    __IO unsigned long rDIVIDEND;/*<Offset:0x40>  除法功能的被除数寄存器。*/

    __IO unsigned long rDIVISOR;/*<Offset:0x44>  除法功能的除数寄存器。*/

    __IO unsigned long rQUOTIENT;/*<Offset:0x48>  除法功能的商。*/

    __IO unsigned long rREMAINDER;/*<Offset:0x4C>  除法功能的余数。*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long DIV_START:1 ;/*Write 1，starting division operation ; Read 1, indicates division operation done. Read 0, indicates division operation doing*/
            __IO unsigned long DIV_WIDTH:2 ;/*Quotient的位宽. 2’b00: quotient位宽为32bit; 2’b01: quotient位宽为16bit; 2’b10: quotient位宽为8bit. 增加该寄存器的目的是为了提高除法的效率，商位宽为32bit时，除法的计算周期为32个cycle；商位宽为16bit时，除法的计算周期为16个cycle；商位宽为8bit时，除法的计算周期为8个cycle*/
            __IO unsigned long Reserved:29 ;/*保留*/
        }bits;
    }rPMCP_DIV_CTRL;/*<Offset:0x50> N/A*/

    __IO unsigned long rINVALID_VAL;/*<Offset:0x54>  N/A*/

    __IO unsigned long rRMD0_ADDR;/*<Offset:0x58>  N/A*/

    __IO unsigned long rRMD1_ADDR;/*<Offset:0x5C>  N/A*/

    __IO unsigned long rRMD2_ADDR;/*<Offset:0x60>  N/A*/

    __IO unsigned long rRMD3_ADDR;/*<Offset:0x64>  N/A*/

    __IO unsigned long rMATRIX_MOVE_OP_SEL;/*<Offset:0x68>  N/A*/

    __IO unsigned long rMOVE_RLT0_OP_ARRH;/*<Offset:0x6C>  N/A*/

    __IO unsigned long rMOVE_RLT0_OP_ARRL;/*<Offset:0x70>  N/A*/

    __IO unsigned long rMOVE_RLT1_OP_ARRH;/*<Offset:0x74>  N/A*/

    __IO unsigned long rMOVE_RLT1_OP_ARRL;/*<Offset:0x78>  N/A*/

    __IO unsigned long rMOVE_RLT2_OP_ARRH;/*<Offset:0x7C>  N/A*/

    __IO unsigned long rMOVE_RLT2_OP_ARRL;/*<Offset:0x80>  N/A*/

    __IO unsigned long rMOVE_RLT3_OP_ARRH;/*<Offset:0x84>  N/A*/

    __IO unsigned long rMOVE_RLT3_OP_ARRL;/*<Offset:0x88>  N/A*/

    __IO unsigned long rMOVE_RLT4_OP_ARRH;/*<Offset:0x8C>  N/A*/

    __IO unsigned long rMOVE_RLT4_OP_ARRL;/*<Offset:0x90>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CQ_START:1 ;/*写1启动CQ，HW自动清零*/
            __IO unsigned long CQ_STOP:1 ;/*写1停止CQ，HW自动清零*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rCQ_CTRL;/*<Offset:0x94> N/A*/

    __IO unsigned long rCQ_BADDR;/*<Offset:0x98>  N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CQ_INT_EN:16 ;/*N/A*/
            __IO unsigned long HOLD_INT_EN:1 ;/*N/A*/
            __IO unsigned long Reserved:15 ;/*保留*/
        }bits;
    }rCQ_INT_EN;/*<Offset:0x9c> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long CQ_INT_STS:16 ;/*表示由CQ发送的中断编号，用于区分发送的各个中断*/
            __IO unsigned long HOLD_INT_STS:1 ;/*表示cq中断标志*/
            __IO unsigned long Reserved:15 ;/*保留*/
        }bits;
    }rCQ_INT_STS;/*<Offset:0xa0> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*当前CQ执行到CQ_Hold指令时，HW会去查询CQ_ready，若CQ_ready=0则会继续查询，直到FW设定CQ_ready=1之后，CQ才会继续执行，同时把CQ_ready清零*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rCQ_READY;/*<Offset:0xa4> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*当前CQ执行到CQ_Hold指令时，HW会将cq_hold_flag 标志位置1*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rCQ_HOLD_FLAG;/*<Offset:0xa8> N/A*/

    __IO unsigned char Reserved[20];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long OV_FLAG:1 ;/*N/A*/
            __IO unsigned long CFG_ERR:1 ;/*N/A*/
            __IO unsigned long OP_ERR:1 ;/*N/A*/
            __IO unsigned long CQ_ERR:1 ;/*N/A*/
            __IO unsigned long ALL0_FLAG:1 ;/*N/A*/
            __IO unsigned long ALL1_FLAG:1 ;/*N/A*/
            __IO unsigned long EQ_FLAG:1 ;/*N/A*/
            __IO unsigned long Reserved:25 ;/*保留*/
        }bits;
    }rPMCP_STS;/*<Offset:0xc0> N/A*/

    __IO unsigned char Reserved1[12];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:3 ;/*N/A*/
            __IO unsigned long Reserved:29 ;/*保留*/
        }bits;
    }rSAF_PARAM;/*<Offset:0xd0> N/A*/

    __IO unsigned char Reserved2[4];/*保留*/

    __IO unsigned long rBIT_MASK2;/*<Offset:0xd8>  N/A*/

    __IO unsigned char Reserved3[4];/*保留*/

    __IO unsigned long rRESULT_CNT2;/*<Offset:0xe0>  N/A*/

    __IO unsigned long rRESULT_CNT3;/*<Offset:0xe4>  N/A*/

    __IO unsigned char Reserved4[8];/*保留*/

    __IO unsigned long rMOVE_RLT5_OP_ARRH;/*<Offset:0xf0>  N/A*/

    __IO unsigned long rMOVE_RLT5_OP_ARRL;/*<Offset:0xf4>  N/A*/

} SG_PMCP_TypeDef, *pSG_PMCP_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
