/*********************************************************

  模块名称: CPM

  版本号:   0.3

*********************************************************/

#ifndef __CPM_REG_DEF__
#define __CPM_REG_DEF__



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



#define SG_CPM_BASE     (0x5E0000)



typedef struct _SG_CPM_TypeDef
{
    __IO unsigned long rSYS_RESERVED0;/*<Offset:0x00>  开放给FW做特殊用途，只能被POR复位*/

    __IO unsigned long rSYS_RESERVED1;/*<Offset:0x04>  开放给FW做特殊用途，只能被POR复位*/

    __IO unsigned long rSYS_RESERVED2;/*<Offset:0x08>  开放给FW做特殊用途，只能被POR复位*/

    __IO unsigned long rSYS_RESERVED3;/*<Offset:0x0c>  开放给FW做特殊用途，只能被POR复位*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:6 ;/*通道0 trx2分频参数*/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rTRX2_CLK_PERIOD;
        /*<Offset:0x10> 只支持偶数分频和bypass模式，即2、4、6、8….62 
        6’h0：不支持
        6’h1：bypass模式即直通模式
        6’h2：2分频
        6’h3：不支持
        6’h4：4分频
        6’h5：不支持
        6’h6：6分频
        ……
        注意：不能设置为除了1之外的其他奇数*/


    __IO unsigned char rSYS_CLK_PERIOD;
        /*<Offset:0x14>  只支持偶数分频和bypass模式，即2、4、6、8….62 
        6’h0：不支持
        6’h1：bypass模式即直通模式
        6’h2：2分频
        6’h3：不支持
        6’h4：4分频
        6’h5：不支持
        6’h6：6分频
        ……
        注意：不能设置为除了1之外的其他奇数*/


    __IO unsigned char Reserved[3];/*保留*/

    __IO unsigned char rSPI_CLK_PERIOD;/*<Offset:0x18>  spi clock 分频系数，是对SYS_CLK进行分频，f_SPI_CLK = f_SYS_CLK / SPI_CLK_period, 有效值范围1~255之间的偶数，即只支持偶数分频*/

    __IO unsigned char Reserved1[7];/*保留*/

    __IO unsigned char rLDEC_CLK_PERIOD;
        /*<Offset:0x20>  分频参数，只支持偶数分频和bypass模式，即2、4、8…62
        6’h0：不支持
        6’h1：bypass模式即直通模式
        6’h2：2分频
        6’h3：不支持
        6’h4：4分频
        6’h5：不支持
        6’h6：6分频
        ……
         注意：
        1.不能设置为除了1之外的其他奇数
        2.不能设置成0*/


    __IO unsigned char Reserved2[3];/*保留*/

    __IO unsigned char rSPI_SPCLK_CTRL;/*<Offset:0x24>  N/A*/

    __IO unsigned char Reserved3[3];/*保留*/

    __IO unsigned long rSPI_SPCLK_DLY_CTRL0;/*<Offset:0x28>  */

    __IO unsigned long rSPI_SPCLK_DLY_CTRL1;/*<Offset:0x2C>  */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:4 ;/*通道0 trx2 clock source select register*/
            __IO unsigned long Reserved:28 ;/*保留*/
        }bits;
    }rTRX2_CLK_SOURCE_SEL;
        /*<Offset:0x30> 选择trx2_clk分频器的输入时钟来源，直接切换，不需要操作clk_switch_en
        4’b0001：选择pre_clk
        4’b0010：选择OSC_clk_333M
        4’b0100：选择OSC_clk_375M
        4’b1000：选择OSC_clk_400M*/


    __IO unsigned char rSYS_CLK_SOURCE_SEL;
        /*<Offset:0x34>  选择sys_clk分频器的输入时钟来源，直接切换，不需要操作clk_switch_en
        4’b0001：选择pre_clk
        4’b0010：选择OSC_clk_333M
        4’b0100：选择OSC_clk_375M
        4’b1000：选择OSC_clk_400M*/


    __IO unsigned char Reserved4[7];/*保留*/

    __IO unsigned char rLDEC_CLK_SOURCE_SEL;
        /*<Offset:0x3C>  选择LDPC Decode clock分频器的输入时钟来源，直接切换，不需要操作clk_switch_en
        4’b0001：选择pre_clk（见REG 0x09 PRE_CLK_source_sel）
        4’b0010：选择OSC_clk_333M
        4’b0100：选择OSC_clk_375M
        4’b1000：选择OSC_clk_400M*/


    __IO unsigned char Reserved5[3];/*保留*/

    __IO unsigned char rPRE_CLK_SOURCE_SEL;
        /*<Offset:0x40>  选择sys_clk分频器和trx2_clk分频器的输入时钟clk1的来源，即pre_clk，直接切换，不需要操作clk_switch_en
        4’b0001：选择OSC_clk_250M
        4’b0010：选择OSC_clk_25M
        4’b0100：选择OSC_clk_25M
        4’b1000：选择OSC_clk_25M*/


    __IO unsigned char Reserved6[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:6 ;/**/
            __IO unsigned long Reserved:26 ;/*保留*/
        }bits;
    }rRX_CLK_PERIOD;/*<Offset:0x44> */

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:4 ;/**/
            __IO unsigned long Reserved:28 ;/*保留*/
        }bits;
    }rRX_CLK_SOURCE_SEL;/*<Offset:0x48> */

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;/**/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rRX_CLK_SWITCH_EN;/*<Offset:0x4C> */

    __IO unsigned char Reserved7[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;/*通道0 trx2 clock switch enable register*/
            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rTRX2_CLK_SWITCH_EN;
        /*<Offset:0x50> 0：改变分频系数对clk_divider无影响
        1：clk_divider会加载新的分频系数进行频率切换
        切换频率时应该遵循如下操作流程：
        Soft_gating &= 0x1FFFFFF;
        TRX2_Clk_switch_en=0;
        Change TRX2_CLK_period;
        Delay;
        TRX2_Clk_switch_en=1;
        Delay;
        Clk_gating |= 0xFFffFfff;*/


    __IO unsigned char Reserved8[3];/*保留*/

    __IO unsigned char rSYS_CLK_SWITCH_EN;
        /*<Offset:0x54>  0：改变分频系数对clk_divider无影响
        1：clk_divider会加载新的分频系数进行频率切换
        切换频率时应该遵循如下操作流程：
        SYS_Clk_switch_en=0;
        Change SYS_CLK_period;
        SYS_Clk_switch_en=1;*/


    __IO unsigned char Reserved9[3];/*保留*/

    __IO unsigned char rSPI_CLK_SWITCH_EN;
        /*<Offset:0x58>  0：改变分频系数对clk_divider无影响
        1：clk_divider会加载新的分频系数进行频率切换
        切换频率时应该遵循如下操作流程：
        SPI_Clk_switch_en=0;
        Change SPI_CLK_period;
        SPI_Clk_switch_en=1;*/


    __IO unsigned char Reserved10[7];/*保留*/

    __IO unsigned char rLDEC_CLK_SWITCH_EN;/*<Offset:0x60>  */

    __IO unsigned char Reserved11[11];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long MIXRAM2:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM3:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM4:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM5:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM6:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM7:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM8:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM9:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM10:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM11:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long TEST_DIV_CLK_GATE:1 ;/*N/A*/
            __IO unsigned long Reserved:21 ;/*保留*/
        }bits;
    }rSOFT_GATE2;/*<Offset:0x6C> 控制各模块clk的开关，1表示有clk，0表示无clk且固定是低电平*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long G0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long G3:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long DIU:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC1:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC2:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC3:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long PADCTRL:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long PMCP0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long PMCP1:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long UART0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved:3 ;/*保留*/
            __IO unsigned long ADM:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved1:1 ;/*保留*/
            __IO unsigned long DARB:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved2:1 ;/*保留*/
            __IO unsigned long RAID:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved3:1 ;/*保留*/
            __IO unsigned long SPI:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved4:4 ;/*保留*/
            __IO unsigned long FC0_TRX2_CLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC1_TRX2_CLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC2_TRX2_CLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long FC3_TRX2_CLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved5:3 ;/*保留*/
        }bits;
    }rSOFT_GATE0;/*<Offset:0x70> 控制各模块clk的开关，1表示有clk，0表示无clk且固定是低电平*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long LENC_SYS:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long LDEC_SYS:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long SC_HCLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long ROM:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM1:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM2:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM3:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM4:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM5:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long RSROM:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long SPI_SYS:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM6:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long COMRAM7:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long MIXRAM1:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long RAIDRAM0:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long RAIDRAM1:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long RAIDRAM2:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long RAIDRAM3:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long LENC_CLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long LDEC_CLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long SC_ACLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long SC_PCLK:1 ;/*1:有clk  0:关clk*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rSOFT_GATE1;/*<Offset:0x74> 控制各模块clk的开关，1表示有clk，0表示无clk且固定是低电平*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long G0:1 ;/*写0复位*/
            __IO unsigned long G3:1 ;/*写0复位*/
            __IO unsigned long DIU:1 ;/*写0复位*/
            __IO unsigned long FC0:1 ;/*写0复位*/
            __IO unsigned long FC1:1 ;/*写0复位*/
            __IO unsigned long FC2:1 ;/*写0复位*/
            __IO unsigned long FC3:1 ;/*写0复位*/
            __IO unsigned long FC0_REG:1 ;/*写0复位*/
            __IO unsigned long FC1_REG:1 ;/*写0复位*/
            __IO unsigned long FC2_REG:1 ;/*写0复位*/
            __IO unsigned long FC3_REG:1 ;/*写0复位*/
            __IO unsigned long PADCTRL:1 ;/*写0复位*/
            __IO unsigned long PMCP0:1 ;/*写0复位*/
            __IO unsigned long PMCP1:1 ;/*写0复位*/
            __IO unsigned long UART0:1 ;/*写0复位*/
            __IO unsigned long UART1:1 ;/*写0复位*/
            __IO unsigned long VMC0:1 ;/*写0复位*/
            __IO unsigned long VMC1:1 ;/*写0复位*/
            __IO unsigned long ADM_DMA1:1 ;/*写0复位*/
            __IO unsigned long BFC:1 ;/*写0复位*/
            __IO unsigned long DARB:1 ;/*写0复位*/
            __IO unsigned long DMA:1 ;/*写0复位*/
            __IO unsigned long RAID:1 ;/*写0复位*/
            __IO unsigned long RM:1 ;/*写0复位*/
            __IO unsigned long SPI:1 ;/*写0复位*/
            __IO unsigned long Reserved:2 ;/*保留*/
            __IO unsigned long SC:1 ;/*写0复位*/
            __IO unsigned long LENC:1 ;/*写0复位*/
            __IO unsigned long LDEC:1 ;/*写0复位*/
            __IO unsigned long PHY:1 ;/*写0复位*/
            __IO unsigned long Reserved1:1 ;/*保留*/
        }bits;
    }rSOFT_RST0;/*<Offset:0x78> 写0产生低电平的reset脉冲信号，过后HW会自动置1*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short ADM_DMA20:1 ;/*写0复位*/
            __IO unsigned short ADM_DMA21:1 ;/*写0复位*/
            __IO unsigned short ADM_DMA22:1 ;/*写0复位*/
            __IO unsigned short ADM_DMA23:1 ;/*写0复位*/
            __IO unsigned short ADM2LDPC:1 ;/*写0复位*/
            __IO unsigned short Reserved:4 ;/*保留*/
            __IO unsigned short FC0_CQ:1 ;/*写0复位*/
            __IO unsigned short FC1_CQ:1 ;/*写0复位*/
            __IO unsigned short FC2_CQ:1 ;/*写0复位*/
            __IO unsigned short FC3_CQ:1 ;/*写0复位*/
            __IO unsigned short Reserved1:3 ;/*保留*/
        }bits;
    }rSOFT_RST1;/*<Offset:0x7C> 写0产生低电平的reset脉冲信号，过后HW会自动置1*/

    __IO unsigned char Reserved12[2];/*保留*/

    __IO unsigned long rDQS0_DELAY_CTRL;/*<Offset:0x80>  等待PT结果，见MAS*/

    __IO unsigned long rDQS1_DELAY_CTRL;/*<Offset:0x84>  等待PT结果*/

    __IO unsigned long rDQS2_DELAY_CTRL;/*<Offset:0x88>  等待PT结果*/

    __IO unsigned long rDQS3_DELAY_CTRL;/*<Offset:0x8c>  等待PT结果*/

    __IO unsigned short rMONITOR_CTRL;/*<Offset:0x90>  */

    __IO unsigned char Reserved13[2];/*保留*/

    __I unsigned char rPHY_PLL_STS;
        /*<Offset:0x94>  PHY PLL Clock Ready.
        1’b1: Indicates phy pll ready
        1’b0: Indicates phy pll not ready*/


    __IO unsigned char Reserved14[3];/*保留*/

    __I unsigned long rCHIP_ID;/*<Offset:0x98>  */

    __IO unsigned char Reserved15[4];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short VDT_WARN_FALL_INT_EN:1 ;/*VDT_warn下降沿中断使能，高有效*/
            __IO unsigned short VDT_WARN_RISE_INT_EN:1 ;/*VDT_warn上升沿中断使能，高有效*/
            __IO unsigned short DEVSLP_ENTER_INT_EN:1 ;/*进入DEVSLP中断使能，高有效*/
            __IO unsigned short DEVSLP_EXIT_INT_EN:1 ;/*退出DEVSLP中断使能，高有效*/
            __IO unsigned short TSENSOR_OF_INT_EN:1 ;/*温度传感器超出设定范围中断使能，高有效*/
            __IO unsigned short TSENSOR_RECOVERY_INT_EN:1 ;/*温度传感器从超出设定范围到回复正常中断使能，高有效*/
            __IO unsigned short VDT_LOW_INT_EN:1 ;/*VDT低电平持续中断使能，高有效*/
            __IO unsigned short PAW_INT_EN:1 ;/*Partial auto wakeup中断使能，高有效*/
            __IO unsigned short COMRESET_INT_EN:1 ;/*Sata comreset中断使能，高有效*/
            __IO unsigned short Reserved:7 ;/*保留*/
        }bits;
    }rCPM_INT_CTRL;/*<Offset:0xA0> */

    __IO unsigned char Reserved16[2];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short VDT_WARN_FALL_INT_STS:1 ;/*vdt_warn falling int status*/
            __IO unsigned short VDT_WARN_RISE_INT_STS:1 ;/*vdt_warn rising int status*/
            __IO unsigned short DEVSLP_ENTER_INT_STS:1 ;/*N/A*/
            __IO unsigned short DEVSLP_EXIT_INT_STS:1 ;/*N/A*/
            __IO unsigned short TSENSOR_OF_INT_STS:1 ;/*N/A*/
            __IO unsigned short TSENSOR_RECOVERY_INT_STS:1 ;/*N/A*/
            __IO unsigned short VDT_LOW_INT_STS:1 ;/*N/A*/
            __IO unsigned short PAW_INT_STS:1 ;/*N/A*/
            __IO unsigned short COMRESET_INT_STS:1 ;/*N/A*/
            __IO unsigned short Reserved:7 ;/*保留*/
        }bits;
    }rCPM_INT_STS;/*<Offset:0xA4> */

    __IO unsigned char Reserved17[2];/*保留*/

    __IO unsigned short rVDT_CNT;/*<Offset:0xA8>  用于计数VDT_warn低电平宽度，每隔1us计数1次，当计数到最大值0xFFFF后保持该值（65ms），MCU写0清零，且只可POR复位*/

    __IO unsigned char Reserved18[2];/*保留*/

    __IO unsigned short rVDT_LOW_THRESHOLD;/*<Offset:0xAC>  N/A*/

    __IO unsigned char Reserved19[2];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long WDT0_RST_EN:8 ;
                /*8’haa:表示WDT0_rst无效，不可用于复位其它模块；
                其他值表示有效，可用于复位其它模块；*/

            __IO unsigned long WDT0_RST_SYS_EN:8 ;/*8‘haa：表示WDT0_rst不可以复位除SATA协议逻辑（包括PHY）之外的系统内部逻辑，其他值表示可以复位*/
            __IO unsigned long WDT0_RST_SATA_EN:8 ;/*8‘haa：表示WDT0_rst不可以复位SATA协议逻辑（包括PHY），其他值表示可以复位*/
            __IO unsigned long WDT0_RST_G0_EN:8 ;/*8‘haa：表示WDT0_rst不可以复位G0，其他值表示可以复位G0*/
        }bits;
    }rWDT0_RST_CTRL;
        /*<Offset:0xB0> 1：无论wdt0_rst是否有效，只要产生了WDT0_rst都会有WDT0_rst_flag
        2：只有WDT0_rst有效时，该寄存器的高24bit分别才有意义
        3：WDT0固定给G0使用*/


    __IO unsigned char rWDT0_EN;/*<Offset:0xB4>  MCU写入8’haa将会关闭WDT0，写入其它值则会打开WDT0*/

    __IO unsigned char Reserved20[3];/*保留*/

    __IO unsigned long rWDT0_THRESHOLD;/*<Offset:0xB8>  设定阈值，当WDT0_cur_cnt计数到这个值时就会产生WDT0_rst*/

    __O unsigned char rWDT0_CLEAR;/*<Offset:0xBC>  MCU写0或写1都会清零WDT0_cur_cnt，HW自动清零*/

    __IO unsigned char Reserved21[3];/*保留*/

    __I unsigned long rWDT0_CUR_CNT;
        /*<Offset:0xC0>  WDT0当前的计数值
        注意：当软件读取这个值的同时，count还在继续计数，所以读到的值会有滞后*/


    __IO unsigned char rWDT0_RST_FLAG;
        /*<Offset:0xC4>  1：表示发生了WDT0复位事件
        0：未发生
        MCU写0清零*/


    __IO unsigned char Reserved22[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long WDT1_RST_EN:8 ;
                /*8’haa:表示WDT1_rst无效，不可用于复位其它模块；
                其他值表示有效，可用于复位其它模块；*/

            __IO unsigned long WDT1_RST_SYS_EN:8 ;/*8‘haa：表示WDT1_rst不可以复位除SATA协议逻辑（包括PHY）之外的系统内部逻辑，其他值表示可以复位*/
            __IO unsigned long WDT1_RST_SATA_EN:8 ;/*8‘haa：表示WDT1_rst不可以复位SATA协议逻辑（包括PHY），其他值表示可以复位*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rWDT1_RST_CTRL;/*<Offset:0xC8> 注意：只有当bit[0]=1的时候bit[2:1]才有意义*/

    __IO unsigned char rWDT1_EN;/*<Offset:0xCC>  MCU写入8’haa将会关闭WDT0，写入其它值则会打开WDT1*/

    __IO unsigned char Reserved23[3];/*保留*/

    __IO unsigned long rWDT1_THRESHOLD;/*<Offset:0xD0>  设定阈值，当WDT1_cur_cnt计数到这个值时就会产生WDT1_rst*/

    __O unsigned char rWDT1_CLEAR;/*<Offset:0xD4>  MCU写0或写1都会清零WDT1_cur_cnt，HW自动清零*/

    __IO unsigned char Reserved24[3];/*保留*/

    __I unsigned long rWDT1_CUR_CNT;
        /*<Offset:0xD8>  WDT1当前的计数值
        注意：当软件读取这个值的同时，count还在继续计数，所以读到的值会有滞后*/


    __IO unsigned char rWDT1_RST_FLAG;
        /*<Offset:0xDC>  1：表示发生了WDT1复位事件
        0：未发生
        MCU写0清零*/


    __IO unsigned char Reserved25[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;
                /*1：打开G0，即给出clock且reset为高
                0：关闭G0，即关闭clock且reset固定为低*/

            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rMCU_CTRL;
        /*<Offset:0xE0> 1：打开G0，即给出clock且reset为高
        0：关闭G0，即关闭clock且reset固定为低*/


    __IO unsigned char Reserved26[3];/*保留*/

    __I unsigned long rRTL_TAG;/*<Offset:0xE4>  FPGA使用*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long SENSOR_MIN_THRESHOLD_VALUE:5 ;/*当sensor输出的值小于max_threshold_value，输出sensor中断*/
            __IO unsigned long Reserved:3 ;/*保留*/
            __IO unsigned long SENSOR_MAX_THRESHOLD_VALUE:5 ;/*当sensor输出的值大于max_threshold_value，输出sensor中断*/
            __IO unsigned long Reserved1:3 ;/*保留*/
            __IO unsigned long SENSOR_RECOVERY_MIN_THRESHOLD_VALUE:5 ;/*当sensor输出的值从小于min_threshold_value值恢复到大于等于recovery_min_threshold_value，输出sensor recovery中断*/
            __IO unsigned long Reserved2:3 ;/*保留*/
            __IO unsigned long SENSOR_RECOVERY_MAX_THRESHOLD_VALUE:5 ;/*当sensor输出的值从大于max_threshold_value值恢复到小于等于recovery_min_threshold_value，输出sensor recovery中断*/
            __IO unsigned long Reserved3:3 ;/*保留*/
        }bits;
    }rSENSOR_THRESHOLD_VALUE;/*<Offset:0xE8> N/A*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ENTER_OFFLINE_MODE_REQ:1 ;/*Enter offline mode request, write 1, send enter offline mode request to sleep module.读取的值为0*/
            __IO unsigned char EXIT_SLEEP_MODE_REQ:1 ;/*Exit sleep mode request, write 1, send exit sleep mode request读取的值为0*/
            __IO unsigned char SLUMBER_S_ENTER_EN:1 ;/*当检测到controller发出的slumber信号之后，Hardware会等到该bit置位之后才置位slumber_s信号(前提是PHY_POWER_CTRL[17]有效)*/
            __IO unsigned char Reserved:5 ;/*保留*/
        }bits;
    }rSLEEP_CTRL;/*<Offset:0xEC> N/A*/

    __IO unsigned char Reserved27[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long PARTIAL_EN:1 ;/*Partial state enable, high active. This bit is valid only when partial control mode asserted 1;*/
            __IO unsigned long SLUMBER_EN:1 ;/*Slumber state enable, high active. This bit is valid only when slumber control mode asserted 1;*/
            __IO unsigned long SLUMBER_S_EN:1 ;/*Slumber_s state enable, high active. This bit is valid only when slumber control mode asserted 1;*/
            __IO unsigned long OFFLINE_EN:1 ;/*Offline state enable, high active. This bit is valid only when offline control mode asserted 1;*/
            __IO unsigned long LISTEN:1 ;/*listen state enable, high active*/
            __IO unsigned long Reserved:3 ;/*保留*/
            __IO unsigned long PARTIAL_CTRL_MODE:1 ;
                /*Partial control mode. 1’b1:mcu control; 1’b0:hardware control
                Default value: hardware control*/

            __IO unsigned long SLUMBER_CTRL_MODE:1 ;
                /*Slumber control mode. 1’b1:mcu control; 1’b0:hardware control
                Default value: hardware control*/

            __IO unsigned long SLUMBER_S_CTRL_MODE:1 ;
                /*Slumber_s control mode. 1’b1:mcu control; 1’b0:hardware control
                Default value: hardware control*/

            __IO unsigned long OFFLINE_CTRL_MODE:1 ;
                /*Offline control mode. 1’b1:mcu control; 1’b0:hardware control
                Default value: hardware control*/

            __IO unsigned long Reserved1:4 ;/*保留*/
            __IO unsigned long PARTIAL_SLUMBER_CLK_GATING_EN:1 ;
                /*Default value: No clock gating
                1’b1: Gating Clocks when enter partial mode
                1’b0: No Gating Clocks when enter partial mode*/

            __IO unsigned long SLUMBER_MODE:1 ;
                /*1’b1: enter slumber_s mode when hardware detect slumber request.
                1’b0: enter slumber mode when hardware detect partial request.*/

            __IO unsigned long MCU_CLK_GATING_EN:1 ;
                /*1’b1: Gating mcu clock when enter offline/partial/slumber state
                1’b0: Don’t gating mcu clock when enter offline/partial/slumber state*/

            __IO unsigned long ACLK_GATING_EN:1 ;
                /*1’b1: Gating sata controller asic clock when enter offline/partial/slumber state
                1’b0: Don’t gating sata controller asic clock when enter offline/partial/slumber state*/

            __IO unsigned long PCLK_GATING_EN:1 ;
                /*1’b1: Gating sata controller pmalive clock when enter offline/partial/slumber state
                1’b0: Don’t gating sata controller pmalive clock when enter offline/partial/slumber state*/

            __IO unsigned long HCLK_GATING_EN:1 ;
                /*1’b1: Gating sata controller ahb clock when enter offline/partial/slumber state
                1’b0: Don’t gating sata controller ahb clock when enter offline/partial/slumber state
                 Note: This bit should be asserted 0 in partial/slumber state*/

            __IO unsigned long Reserved2:10 ;/*保留*/
        }bits;
    }rPHY_POWER_CTRL;/*<Offset:0xF0> 控制phy的power mode以及设置进入sleep时device的行为*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long PAWCNT:16 ;/*Default value:1ms, reference clock:25Mhz*/
            __IO unsigned long PAWEN:1 ;
                /*1’b1: enable partial auto wakeup mode
                1’b0: disable partial auto wakeup mode
                When the mode enable, Hardware will auto wakeup after enter partial state PAWCNT cycles.*/

            __IO unsigned long Reserved:15 ;/*保留*/
        }bits;
    }rPAWCTRL;/*<Offset:0xF4> partial auto wakeup control register*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char DEVSLP_EN:1 ;
                /*1：Enable DEVSLP
                0：Disable DEVSLP(default value)
                 注意：DEVSLP功能默认无效，Firmware根据SATA协议需要使能该bit，相关的协议部分见：SATA协议Chapter 8, Devslp signal protocol and timing part(8.5).*/

            __IO unsigned char PHY_PORN_EN:1 ;
                /*1’b1:Assert phy porn when exiting from DEVSLP
                1’b0:De-assert phy porn when exiting from DEVSLP*/

            __IO unsigned char PHY_RESET_EN:1 ;
                /*1’b1:Assert phy reset when exiting from DEVSLP
                1’b0:De-assert phy reset when exiting from DEVSLP*/

            __IO unsigned char XSC_DIS_EN:1 ;
                /*1’b1: When enter DEVSLP state, disable osc
                1’b0: When enter DEVSLP state, don’t disable osc*/

            __IO unsigned char DE_ASSERTED_PAR_SLUMBER_EN:1 ;
                /*1’b1: If device enter DEVSLP state from partial and slumber state, cpm will de-assert partial and slumber signal whether partial signal assert or not
                1’b0: If device enter DEVSLP state from partial and slumber state, cpm will not de-assert partial and slumber signal until detecting partial signal release.*/

            __IO unsigned char ASSERT_COMWAKE_EN:1 ;
                /*1’b1: When exiting from DEVSLP, cpm will assert comwake signal until sata clock release 4 cycles
                1’b0: When exiting from DEVSLP, cpm will not assert comwake signal*/

            __IO unsigned char SC_PARESET_EN:1 ;
                /*1’b1: When exiting from DEVSLP, cpm will reset sc controller pclk and aclk domain logic
                1’b0: When exiting from DEVSLP, cpm will not reset sc controller pclk and aclk domain logic*/

            __IO unsigned char Reserved:1 ;/*保留*/
        }bits;
    }rDEVSLP_CTRL;/*<Offset:0xF8> device sleep control register*/

    __IO unsigned char Reserved28[3];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:12 ;/*设定DEVSLP从0变成1之后高电平维持的时间，采用25M_clk计数，若超过这个值则认为DEVSLP真正变成了1，默认为10us*/
            __IO unsigned short Reserved:4 ;/*保留*/
        }bits;
    }rDEVSLP_ENTER_MDT;/*<Offset:0xFC> device sleep enter mdt register*/

    __IO unsigned char Reserved29[2];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short wVAL:12 ;/*设定DEVSLP从1变成0之后高电平维持的时间，采用25M_clk计数，若超过这个值则认为DEVSLP真正变成了1，默认为10us*/
            __IO unsigned short Reserved:4 ;/*保留*/
        }bits;
    }rDEVSLP_EXIT_MDT;/*<Offset:0x100> device sleep exit mdt register*/

    __IO unsigned char Reserved30[2];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char DEVICE_SLEEP_STS:1 ;
                /*1’b1:Device in sleep state
                1’b0:Device in normal state*/

            __IO unsigned char DEVICE_WAKEUP_STS:1 ;
                /*1’b1:Device has been wake up
                1’b0:Device in sleep mode*/

            __IO unsigned char Reserved:6 ;/*保留*/
        }bits;
    }rPOWER_STS;/*<Offset:0x104> device power status register*/

    __IO unsigned char Reserved31[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:1 ;
                /*PHY进入offline模式时会置位DSOUT，出offline模式时会复位DSOUT，在offline模式期间会维持输出电平信号，该REG用于控制DSOUT的输出极性. DSOUT信号的作用是控制芯片外面的电源开关。
                1：DSOUT输出高有效
                0：DSOUT输出低有效（默认）*/

            __IO unsigned char Reserved:7 ;/*保留*/
        }bits;
    }rDSOUT_CTRL;/*<Offset:0x108> 1:DSOUT高有效  0:DSOUT低有效*/

    __IO unsigned char Reserved32[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long SLEEP_EXIT_CNT:16 ;/*接收到wakeup信号之后，还需要再等待一段时间才会退出sleep，When receive wakeup signal, then chip will exit sleep mode after Sleep_exit_cnt osc clock cycles, this counter value should be greater than 1*/
            __IO unsigned long PAR_SLU_ENTER_CNT:16 ;/*接收到sata controller发出的Partial或者slumber后，过Partial/Slumber enter count之后，才会置位PHY的partial或者slumber信号,this counter value should be greater than 1*/
        }bits;
    }rSLEEP_CNT;/*<Offset:0x10c> sleep相关count*/

    __IO unsigned short rXSC_UNSTABLE_TIME;/*<Offset:0x110>  XSC起振之后，HW会先计数一段时间再把osc_clk给到内部逻辑，以避开起振之后可能存在的osc_clk不稳定时间默认10us*/

    __IO unsigned char Reserved33[2];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ISO_EN:1 ;
                /*1’b1: Enable isolation cell between power down and always on
                1’b0: Disable isolation cell between power down and always on
                This bit valid only iso_en_ctrl_mode asserted 0*/

            __IO unsigned char PG:1 ;
                /*1’b1: Shut Off PD domain power
                1’b0: Open PD domain power
                This bit valid only pg_en_ctrl_mode asserted 0*/

            __IO unsigned char SHUT_OFF_POWER_REQ:1 ;
                /*写1，启动shut off power过程；读取的值无意义.
                该bit只有在pg_en_ctrl_mode为1时才有效。*/

            __IO unsigned char OPEN_POWER_REQ:1 ;
                /*写1，启动open power过程；读取的值无意义.
                该bit只有在pg_en_ctrl_mode为1时才有效。*/

            __IO unsigned char ISO_EN_CTRL_MODE:1 ;
                /*1’b1:iso_en controlled by hardware
                1’b0:iso_en controlled by firmware*/

            __IO unsigned char PG_EN_CTRL_MODE:1 ;
                /*1’b1:pg controlled by hardware
                1’b0:pg controlled by firmware*/

            __IO unsigned char Reserved:2 ;/*保留*/
        }bits;
    }rPD_POWER_CTRL;/*<Offset:0x114> N/A*/

    __IO unsigned char Reserved34[3];/*保留*/

    union
    {
        __I unsigned char bAll;
        struct
        {
            __IO unsigned char ISO_EN_STS:1 ;
                /*1’b1: Indicates isolation cell enable
                1’b0: Indicates isolation cell disable*/

            __IO unsigned char PG_STS:1 ;
                /*1’b1: Indicates PD domain power off
                1’b0: Indicates PD domain power on*/

            __IO unsigned char Reserved:6 ;/*保留*/
        }bits;
    }rPD_POWER_STS;/*<Offset:0x118> N/A*/

    __IO unsigned char Reserved35[3];/*保留*/

    union
    {
        __IO unsigned long dAll;
        struct
        {
            __IO unsigned long dVAL:24 ;/*Open pd domain power时，经过pg_exit_cnt个clock cycle(system clock)，才开始release reset及clock gating，以保证reset和clock gating在pd power确定被打开之后才被release*/
            __IO unsigned long Reserved:8 ;/*保留*/
        }bits;
    }rPG_EXIT_CNT;/*<Offset:0x11C> N/A*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:4 ;
                /*[2:0]：档位选择，共8档
                [3]：1-使能filter  0-关闭filter
                用于滤除负毛刺
                单位ns    max    typ    min    
                step    10.1    5.6    3.5    */

            __IO unsigned char Reserved:4 ;/*保留*/
        }bits;
    }rDSIN_FILTER_CTRL;/*<Offset:0x120> N/A*/

    __IO unsigned char Reserved36[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char ERST_FILTER_CTRL:4 ;
                /*2:0]：档位选择，共8档
                [3]：1-使能filter  0-关闭filter
                用于滤除负毛刺
                单位ns    max    typ    min    
                step    10.1    5.6    3.5    */

            __IO unsigned char ERST_EN:1 ;
                /*1’b1:eRst会作为POR使用
                1’b0:eRst不起作用*/

            __IO unsigned char Reserved:3 ;/*保留*/
        }bits;
    }rERST_CTRL;/*<Offset:0x124> N/A*/

    __IO unsigned char Reserved37[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char bVAL:3 ;
                /*000: do not enable filter
                001: filter 0.3ns glitch   010: filter 0.6ns glitch  011: filter 0.9ns glitch
                100: filter 1.2ns glitch   101: filter 1.5ns glitch 
                110: filter 1.8ns glitch   111: filter 2.1ns glitch*/

            __IO unsigned char Reserved:5 ;/*保留*/
        }bits;
    }rDEVDET_FILTER_CTRL;/*<Offset:0x128> N/A*/

    __IO unsigned char Reserved38[3];/*保留*/

    union
    {
        __IO unsigned char bAll;
        struct
        {
            __IO unsigned char COMRESET_EN:1 ;
                /*1’b1: Enable comreset
                1’b0: Disable comreset*/

            __IO unsigned char COMRESET_RST_PCLK_EN:1 ;
                /*1’b1: Enable comrest reset sc pclk domain
                1’b0: Enable comrest reset sc pclk domain*/

            __IO unsigned char COMRESET_RST_ACLK_EN:1 ;
                /*1’b1: Enable comrest reset sc aclk domain
                1’b0: Enable comrest reset sc aclk domain*/

            __IO unsigned char COMRESET_RST_HCLK_EN:1 ;
                /*1’b1: Enable comrest reset sc hclk domain
                1’b0: Enable comrest reset sc hclk domain*/

            __IO unsigned char Reserved:4 ;/*保留*/
        }bits;
    }rCOMRESET_CTRL;/*<Offset:0x12C> N/A*/

    __IO unsigned char Reserved39[19];/*保留*/

    union
    {
        __IO unsigned short wAll;
        struct
        {
            __IO unsigned short UPDATE_DELAY_VALUE:1 ;/*FW写1后HW自动更新ADDLL给出的delay line的设定值，HW自动清零该REG*/
            __IO unsigned short LOCK_START:1 ;/*FW写1 start，HW自动清零*/
            __IO unsigned short LOCK_STOP:1 ;/*FW写1 stop，HW自动清零*/
            __IO unsigned short Reserved:5 ;/*保留*/
            __IO unsigned short ADJUST_MODE:1 ;/*0-设定ADDLL从Coarse档位开始调节（默认），1-设定ADDLL从General档位开始调节*/
            __IO unsigned short Reserved1:7 ;/*保留*/
        }bits;
    }rADDLL0_CTRL;/*<Offset:0x140> N/A*/

    __IO unsigned char Reserved40[2];/*保留*/

    __I unsigned long rADDLL0_DLY_VALUE;/*<Offset:0x144>  ADDLL给出的delay_line的设定值，若lock ok，则该值表示单个delay line会产生90度相移    */

    union
    {
        __I unsigned char bAll;
        struct
        {
            __IO unsigned char LOCK_OK:1 ;/*HW自动清零，MCU只读*/
            __IO unsigned char LOCK_DONE:1 ;/*HW自动清零，MCU只读*/
            __IO unsigned char Reserved:6 ;/*保留*/
        }bits;
    }rADDLL0_LOCK_STATUS;/*<Offset:0x148> N/A*/

    __IO unsigned char Reserved41[3];/*保留*/

    __IO unsigned long rADDLL0_INIT_DLY_VALUE;
        /*<Offset:0x14C>  设定ADDLL的初始值
        注意：
        因为ADDLL使用了2个delay line，因此设定的初始值不能让单个delay line产生大于180度的相移*/


} SG_CPM_TypeDef, *pSG_CPM_TypeDef;



#ifdef __cplusplus
}
#endif



#endif
