/*********************************************************

  模块名称: MCU

  版本号:   20191011

*********************************************************/

#ifndef __MCU_REG_DEF__
#define __MCU_REG_DEF__



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



#define SG_MCU_BASE     (0x5E6000)



typedef struct _SG_MCU_TypeDef
{
    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ENABLE:1 ;/*N/A*/
            __IO unsigned char OV_FLAG:1 ;/*N/A*/
            __IO unsigned char MODE:1 ;/*N/A*/
            __IO unsigned char Reserved:5 ;/*保留*/
        }bits;
    }rTIMER0_CTRL;/*<Offset:0x00> N/A*/

    __IO unsigned char Reserved[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ENABLE:1 ;/*N/A*/
            __IO unsigned char OV_FLAG:1 ;/*N/A*/
            __IO unsigned char MODE:1 ;/*N/A*/
            __IO unsigned char Reserved:5 ;/*保留*/
        }bits;
    }rTIMER1_CTRL;/*<Offset:0x04> N/A*/

    __IO unsigned char Reserved1[3];/*保留*/

    __IO unsigned long rPWM_HIGH_LEVEL_REGISTER;/*<Offset:0x08>  该PWM波发生器以系统时钟作为基准时钟，其高电平维持时间由计数器计数产生，比如系统时钟为50Mhz时，则每一次计数结果为得到20ns的高电平，若要高电平持续时间为200ns，则设置该寄存器为10，即需要10个cycle的基准时钟周期。该寄存器为32bit寄存器。*/

    __IO unsigned long rPWM_LOW_LEVEL_REGISTER;/*<Offset:0x0C>  该PWM波发生器以系统时钟作为基准时钟，其低电平维持时间由计数器计数产生，比如系统时钟为50Mhz时，则每一次计数结果为得到20ns的高电平，若要低电平持续时间为200ns，则设置该寄存器为10，即需要10个cycle的基准时钟周期。该寄存器为32bit寄存器。*/

    __IO unsigned long rTIMER0;/*<Offset:0x10>  32bit internal timer register.该定时器以g0_MCU的时钟作为基准时钟。比如MCU时钟为50Mhz时，如果想要得到0.2us的定时，则设置该寄存器为0.2*50=10，即需要10个cycle的基准时钟周期。该定时器为减法定时器，内部计数器为0时，会置位timer_ov_flag，同时产生timer ov中断，并且计数器会自动load该寄存器里面的值，重新计数。*/

    __IO unsigned long rTIMER1;/*<Offset:0x14>  32bit internal timer register.该定时器以g3_MCU的时钟作为基准时钟。比如MCU时钟为50Mhz时，如果想要得到0.2us的定时，则设置该寄存器为0.2*50=10，即需要10个cycle的基准时钟周期。该定时器为减法定时器，内部计数器为0时，会置位timer_ov_flag，同时产生timer ov中断，并且计数器会自动load该寄存器里面的值，重新计数。*/

    __IO unsigned long rG0_VECTOT_TABLE_OFFSET_ADDRESS;/*<Offset:0x18>  Vector_table_offset_address: when G0 processor access the region between 0x0~0x9C, map the access address to 0x0~0x9C+vector_table_offset_address.*/

    __IO unsigned long rG3_VECTOT_TABLE_OFFSET_ADDRESS;/*<Offset:0x1C>  Vector_table_offset_address: when G0 processor access the region between 0x0~0x9C, map the access address to 0x0~0x9C+vector_table_offset_address.*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char STANDBY_REQ:1 ;/*N/A*/
            __IO unsigned char GLOBAL_STALL:1 ;/*N/A*/
            __IO unsigned char TRUSTED_DEBUG_EXIT:1 ;/*N/A*/
            __IO unsigned char EDM_RESTRICT_ACCESS:1 ;/*N/A*/
            __IO unsigned char Reserved:4 ;/*保留*/
        }bits;
    }rG0_CTRL;/*<Offset:0x20> N/A*/

    __IO unsigned char Reserved2[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char STANDBY_REQ:1 ;/*N/A*/
            __IO unsigned char GLOBAL_STALL:1 ;/*N/A*/
            __IO unsigned char TRUSTED_DEBUG_EXIT:1 ;/*N/A*/
            __IO unsigned char EDM_RESTRICT_ACCESS:1 ;/*N/A*/
            __IO unsigned char Reserved:4 ;/*保留*/
        }bits;
    }rG3_CTRL;/*<Offset:0x24> N/A*/

    __IO unsigned char Reserved3[3];/*保留*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long STANDBY_ST:1 ;/*N/A*/
            __I unsigned long DEBUG_MODE:1 ;/*N/A*/
            __I unsigned long STANDBY_MODE:1 ;/*N/A*/
            __I unsigned long HDEBUG_ACCESS:1 ;/*N/A*/
            __I unsigned long RESERVED:4 ;/*N/A*/
            __I unsigned long CORE_PC:24 ;/*N/A*/
        }bits;
    }rG0_STATUE;/*<Offset:0x28> N/A*/

    union
    {
        __I unsigned long dAll;
        struct
        {
            __I unsigned long STANDBY_ST:1 ;/*N/A*/
            __I unsigned long DEBUG_MODE:1 ;/*N/A*/
            __I unsigned long STANDBY_MODE:1 ;/*N/A*/
            __I unsigned long HDEBUG_ACCESS:1 ;/*N/A*/
            __I unsigned long RESERVED:4 ;/*N/A*/
            __I unsigned long CORE_PC:24 ;/*N/A*/
        }bits;
    }rG3_STATUE;/*<Offset:0x2C> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long LED0_CTRL_EN:1 ;
                /*LED0_CTRL_EN
                1---enable LED0, 0---disable LED0*/

            __IO unsigned long LED1_CTRL_EN :1 ;
                /*LED1_CTRL_EN 
                1---enable LED1, 0---disable LED1*/

            __IO unsigned long LED2_CTRL_EN :1 ;
                /*LED2_CTRL_EN 
                1---enable LED2, 0---disable LED2*/

            __IO unsigned long LED3_CTRL_EN :1 ;
                /*LED3_CTRL_EN 
                1---enable LED3, 0---disable LED3*/

            __IO unsigned long LED0_AO:1 ;
                /*LED0_CTRL_EN
                1---enable LED0, 0---disable LED0*/

            __IO unsigned long LED1_AO:1 ;
                /*LED0_CTRL_EN
                1---enable LED0, 0---disable LED0*/

            __IO unsigned long LED2_AO:1 ;
                /*LED0_CTRL_EN
                1---enable LED0, 0---disable LED0*/

            __IO unsigned long LED03_AO:1 ;
                /*LED0_CTRL_EN
                1---enable LED0, 0---disable LED0*/

            __IO unsigned long Reserved:24 ;/*保留*/
        }bits;
    }rREG_LED_CTRL;/*<Offset:0x30> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long LED0_CTRL_PERIOD:16 ;/*LED1_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）*/
            __IO unsigned long LED1_CTRL_PERIOD:16 ;/*LED0_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）*/
        }bits;
    }rREG_LED_CTRL0_PERIOD;
        /*<Offset:0x34> 高16bit表示LED0闪烁周期的计数值，低16bit表示LED1闪烁周期的计数值。
        LED0_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）
        LED1_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）*/


    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long LED2_CTRL_PERIOD:16 ;/*LED3_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）*/
            __IO unsigned long LED3_CTRL_PERIOD:16 ;/*LED2_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）*/
        }bits;
    }rREG_LED_CTRL1_PERIOD;
        /*<Offset:0x38> 高16bit表示LED2闪烁周期的计数值，低16bit表示LED3闪烁周期的计数值。
        LED2_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）
        LED3_CTRL_PERIOD: LED0闪烁周期时间，单位为41.9ms（假定系统时钟50Mhz）*/


    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*If (SF*_INT0_STS& SF*_EN[0])àSF*_INT0有效，可用于产生soft interrupt*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSF0_INT0_STS;/*<Offset:0x3C> If (SF0_INT0_STS& SF0_EN[0])  THEN SF0_INT0有效，可用于产生soft interrupt*/

    __O unsigned long rSF0_INT1_STS0_SET;/*<Offset:0x40>  If (SF0_INT1_STS0_SET[n]) then  SF0_INT1_STS0[n]=1，用于置某一bit为1，否则SF0_INT1_STS0[n]值不变*/

    __O unsigned long rSF0_INT1_STS1_SET;/*<Offset:0x44>  If (SF0_INT1_STS1_SET[n])then SF0_INT1_STS1[n]=1，用于置某一bit为1，否则SF0_INT1_STS1[n]值不变*/

    __O unsigned long rSF0_INT1_STS0_CLR;/*<Offset:0x48>  If (SF0_INT1_STS0_CLR[n])then SF0_INT1_STS0[n]=0，用于清某一bit为0，否则SF0_INT1_STS0[n]值不变*/

    __O unsigned long rSF0_INT1_STS1_CLR;/*<Offset:0x4C>  If (SF0_INT1_STS1_CLR[n]) then  SF0_INT1_STS1[n]=0，用于清某一bit为0，否则SF0_INT1_STS1[n]值不变*/

    __I unsigned long rSF0_INT1_STS0;/*<Offset:0x50>  N/A*/

    __I unsigned long rSF0_INT1_STS1;/*<Offset:0x54>  N/A*/

    __IO unsigned long rSF0_INT1_EN0;/*<Offset:0x58>  If(SF0_INT1_EN0[n]& SF0_INT1_STS0[n]& SF0_EN[1])  then SF0_INT1低32bit第nbit有效，可用于两MCU间传递信息*/

    __IO unsigned long rSF0_INT1_EN1;/*<Offset:0x5C>  If(SF0_INT1_EN1[n]& SF0_INT1_STS1[n]& SF0_EN[1]) Then SF0_INT1高32bit第n+31bit有效，可用于两MCU间传递信息*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long SF0_INT0_EN:1 ;/*SF0_INT0_EN SF0_INT0中断总使能，高有效*/
            __IO unsigned long SF0_INT1_EN:1 ;/*SF0_INT1_EN SF1_INT1中断总使能，高有效*/
            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rSF0_INT_EN;/*<Offset:0x60> N/A*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:1 ;/*N/A*/
            __IO unsigned long Reserved:31 ;/*保留*/
        }bits;
    }rSF1_INT0_STS;/*<Offset:0x64> If (SF1_INT0_STS& SF1_EN[0])  THEN SF1_INT0有效，可用于产生soft interrupt*/

    __O unsigned long rSF1_INT1_STS0_SET;/*<Offset:0x68>  If (SF1_INT1_STS0_SET[n]) Then SF1_INT1_STS0[n]=1，用于置某一bit为1，否则SF1_INT1_STS0[n]值不变*/

    __O unsigned long rSF1_INT1_STS1_SET;/*<Offset:0x6C>  If (SF1_INT1_STS1_SET[n]) Then SF1_INT1_STS1[n]=1，用于置某一bit为1，否则SF1_INT1_STS1[n]值不变*/

    __O unsigned long rSF1_INT1_STS0_CLR;/*<Offset:0x70>  If (SF1_INT1_STS0_CLR[n])Then SF1_INT1_STS0[n]=0，用于清某一bit为0，否则SF1_INT1_STS0[n]值不变*/

    __O unsigned long rSF1_INT1_STS1_CLR;/*<Offset:0x74>  If (SF1_INT1_STS1_CLR[n])Then SF1_INT1_STS1[n]=0，用于清某一bit为0，否则SF1_INT1_STS1[n]值不变*/

    __I unsigned long rSF1_INT1_STS0;/*<Offset:0x78>  N/A*/

    __I unsigned long rSF1_INT1_STS1;/*<Offset:0x7C>  N/A*/

    __IO unsigned long rSF1_INT1_EN0;/*<Offset:0x80>  If(SF1_INT1_EN0[n]& SF1_INT1_STS0[n]& SF1_EN[1])àSF1_INT1低32bit第nbit有效，可用于两MCU间传递信息*/

    __IO unsigned long rSF1_INT1_EN1;/*<Offset:0x84>  If(SF1_INT1_EN1[n]& SF1_INT1_STS1[n]& SF1_EN[1])àSF1_INT1高32bit第n+31bit有效，可用于两MCU间传递信息*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long SF1_INT0_EN:1 ;
                /*SF1_INT0_EN
                SF1_INT0中断总使能，高有效*/

            __IO unsigned long SF1_INT1_EN:1 ;
                /*SF1_INT1_EN
                SF1_INT1中断总使能，高有效*/

            __IO unsigned long Reserved:30 ;/*保留*/
        }bits;
    }rSF1_INT_EN;/*<Offset:0x88> N/A*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ENABLE:1 ;/*N/A*/
            __IO unsigned char OV_FLAG:1 ;/*N/A*/
            __IO unsigned char MODE:1 ;/*N/A*/
            __IO unsigned char Reserved:5 ;/*保留*/
        }bits;
    }rTIMER2_CTRL;/*<Offset:0x8C> N/A*/

    __IO unsigned char Reserved4[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ENABLE:1 ;/*N/A*/
            __IO unsigned char OV_FLAG:1 ;/*N/A*/
            __IO unsigned char MODE:1 ;/*N/A*/
            __IO unsigned char Reserved:5 ;/*保留*/
        }bits;
    }rTIMER3_CTRL;/*<Offset:0x90> N/A*/

    __IO unsigned char Reserved5[3];/*保留*/

    __IO unsigned long rTIMER2_L;/*<Offset:0x94>  32bit internal timer register.该定时器以g3_MCU的时钟作为基准时钟。比如MCU时钟为50Mhz时，如果想要得到0.2us的定时，则设置该寄存器为0.2*50=10，即需要10个cycle的基准时钟周期。该定时器为减法定时器，内部计数器为0时，会置位timer_ov_flag，同时产生timer ov中断，并且计数器会自动load该寄存器里面的值，重新计数。*/

    __IO unsigned long rTIMER2_H;/*<Offset:0x98>  32bit internal timer register.该定时器以g3_MCU的时钟作为基准时钟。比如MCU时钟为50Mhz时，如果想要得到0.2us的定时，则设置该寄存器为0.2*50=10，即需要10个cycle的基准时钟周期。该定时器为减法定时器，内部计数器为0时，会置位timer_ov_flag，同时产生timer ov中断，并且计数器会自动load该寄存器里面的值，重新计数。*/
    __IO unsigned long rTIMER3_L;/*<Offset:0x9c>  32bit internal timer register.该定时器以g3_MCU的时钟作为基准时钟。比如MCU时钟为50Mhz时，如果想要得到0.2us的定时，则设置该寄存器为0.2*50=10，即需要10个cycle的基准时钟周期。该定时器为减法定时器，内部计数器为0时，会置位timer_ov_flag，同时产生timer ov中断，并且计数器会自动load该寄存器里面的值，重新计数。*/

    __IO unsigned long rTIMER3_H;/*<Offset:0xA0>  32bit internal timer register.该定时器以g3_MCU的时钟作为基准时钟。比如MCU时钟为50Mhz时，如果想要得到0.2us的定时，则设置该寄存器为0.2*50=10，即需要10个cycle的基准时钟周期。该定时器为减法定时器，内部计数器为0时，会置位timer_ov_flag，同时产生timer ov中断，并且计数器会自动load该寄存器里面的值，重新计数。*/
    __I unsigned long rTIMER_CNT0;/*<Offset:0x9c>  用于表示time0当前的计数值*/

    __I unsigned long rTIMER_CNT1;/*<Offset:0xA0>  用于表示time0当前的计数值*/



    __IO unsigned char Reserved6[4];/*保留*/

    __IO unsigned char rMUTEREG0;/*<Offset:0xB0>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved7[3];/*保留*/

    __IO unsigned char rMUTEREG1;/*<Offset:0xB4>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved8[3];/*保留*/

    __IO unsigned char rMUTEREG2;/*<Offset:0xB8>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved9[3];/*保留*/

    __IO unsigned char rMUTEREG3;/*<Offset:0xBC>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved10[3];/*保留*/

    __IO unsigned char rMUTEREG4;/*<Offset:0xC0>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved11[3];/*保留*/

    __IO unsigned char rMUTEREG5;/*<Offset:0xC4>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved12[3];/*保留*/

    __IO unsigned char rMUTEREG6;/*<Offset:0xC8>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved13[3];/*保留*/

    __IO unsigned char rMUTEREG7;/*<Offset:0xCC>  双核交互仲裁寄存器*/

    __IO unsigned char Reserved14[3];/*保留*/
    __I unsigned long rTIMER_CNT2_L;/*<Offset:0xD0>  用于表示time0当前的计数值*/
    __I unsigned long rTIMER_CNT2_H;/*<Offset:0xD4>  用于表示time0当前的计数值*/
    __I unsigned long rTIMER_CNT3_L;/*<Offset:0xD8>  用于表示time0当前的计数值*/
    __I unsigned long rTIMER_CNT3_H;/*<Offset:0xDC>  用于表示time0当前的计数值*/
} SG_MCU_TypeDef, *pSG_MCU_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
