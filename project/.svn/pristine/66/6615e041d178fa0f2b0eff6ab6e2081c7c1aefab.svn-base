// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  debug.c
//  文件标识：  
//  内容摘要：  调试接口实现 
//  其它说明：  
//  当前版本：  v0.10
//  项    目：  SG9081
//  作    者：  William
//  完成日期：  2015年07月16日 
//   
//  修改记录 1：修改历史记录，包括修改日期、修改者及修改内容 
//  修改日期： 
//  版 本 号： 
//  修 改 人： 
//  修改内容：   
// *****************************************************************************
#include "sg9081.h"
#include "debug.h"

#include "uart.h"

#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include "sata_typedef.h"
#include "dsata.h"
extern U8_C code_name[];

#ifndef __G0_CODE
__align(8) U8 aDebugInfoBuffer[DEBUG_OUT_BUFFER_SIZE];
#else
__align(8) U8 aDebugInfoBuffer[32];
#endif
#if  (TRUE == UART_PRINT_EN)
void printch(char **des,char ch)
 {
     **des=ch;
     //aDebugInfoBuffer[0]=ch;
     //UART_SendMsg(UART_NUM,(U8 *)aDebugInfoBuffer,1);//输出到终端
     (*des)++;

 }

void printstr(char **des,char* str)
{
    while(*str)
    {
        printch(des,*str++);
    }
}

 void printdec(char **des,unsigned int dec)
 {
     if(dec==0)
     {
         //printstr(des,"0");
         return;
     }
     printdec(des,dec/10);
     printch(des, (char)(dec%10 + '0'));
 }

 void printflt(char **des,double flt)
 {
     //int icnt = 0;
     int tmpint = 0;

     tmpint = (int)flt;
     printdec(des,tmpint);
     printch(des,'.');
     flt = flt - tmpint;
     tmpint = (int)(flt * 1000000);
     printdec(des,tmpint);
 }

 void printbin(char **des,int bin)
 {
     if(bin == 0)
     {
         printstr(des,"0b");
         return;
     }
     printbin(des,bin>>1);
     printch(des, (char)((bin&0x1) + '0'));
 }

 void printhex(char **des,int hex)
 {
     if(hex==0)
     {
         printstr(des,"0x");
         return;
     }
     printhex(des,((U32)hex>>4));
     if((hex&0xf) < 10)
     {
         printch(des,(char)((hex&0xf)  + '0'));
     }
     else
     {
         printch(des,(char)((hex&0xf)  - 10 + 'A' ));
     }
 }

 void PRINT_TIME()
 {
     static U32 time=0;
     if(time!=0 && time==tSataDC.dPowerOnTime)
     {
         //已经打印过时间直接退出
         return;
     }
     U32 primask=__get_PRIMASK();
     __set_PRIMASK(1);
     char *des_s=(char *)aDebugInfoBuffer;
     char **des=&des_s;
     char **des_start=&des_s;
     U8 len;
     time=tSataDC.dPowerOnTime;//记录时间
     printdec(des,tSataDC.dPowerOnTime);
     len=(unsigned int)*des_start-(unsigned int)aDebugInfoBuffer;
     UART_SendMsg(UART_NUM,aDebugInfoBuffer,len);
     UART_SendMsg(UART_NUM,(U8 *)":",strlen(":"));
     __set_PRIMASK(primask);
 }

int PRINTS(const char *pfmt,void *param1,void *param2,void *param3,void *param4,void *param5)
{
      PRINT_TIME();
      U32 primask=__get_PRIMASK();
      __set_PRIMASK(1);
      unsigned int len;
      char *des_s=(char*)aDebugInfoBuffer;
      char **des=&des_s;
      char **des_start=&des_s;
      unsigned char param_idx=0;
      U32* PLIST[5];
      PLIST[0]=param1;
      PLIST[1]=param2;
      PLIST[2]=param3;
      PLIST[3]=param4;
      PLIST[4]=param5;
      while(*pfmt)
      {
          if(*pfmt == '%')
          {
              switch(*(++pfmt))
              {
                  case 'd':
                  case 'D':
                      //B byte,W word,D double word
                  {
                      char width=*(++pfmt);
                      unsigned long val;
                      if(width == 'W' || width == 'w')
                      {
                          val=*(U16*)PLIST[param_idx];
                      }
                      else if(width == 'D' || width == 'd')
                      {
                          val=*(U32*)PLIST[param_idx];
                      }
                      else
                      {
                         //B or other
                          val=*(U8*)PLIST[param_idx];
                      }
                      if(val == 0)
                      {
                          printch(des,'0');
                      }
                      else
                          printdec(des,val);
                      param_idx++;
                      break;
                  }
                  case 's':
                  case 'S':
                      printstr(des,(char *)(PLIST[param_idx]));
                      param_idx++;
                      break;
                  case 'x':
                  case 'X':
                  {
                      //B byte,W word,D double word
                      char width=*(++pfmt);
                      unsigned long val;
                      if(width == 'W' || width == 'w')
                      {
                          val=*(U16*)PLIST[param_idx];
                      }
                      else if(width == 'D' || width == 'd')
                      {
                          val=*(U32*)PLIST[param_idx];
                      }
                      else
                      {
                         //B or other
                          val=*(U8*)PLIST[param_idx];
                      }
                      if(val == 0)
                      {
                    	  printch(des,'0');
                    	  printch(des,'x');
                    	  printch(des,'0');
                      }
                      else
                    	  printhex(des,val);
                      param_idx++;
                      break;
                  }
                  case '%':
                      printch(des,'%');
                      break;
                  default:
                      break;
              }
              pfmt++;
          }
          else
          {
              printch(des,*pfmt++);
          }
      }
      len=(unsigned int)*des_start-(unsigned int)aDebugInfoBuffer;
      UART_SendMsg(UART_NUM,aDebugInfoBuffer,len);
      __set_PRIMASK(primask);
      return len;
  }

 void PRINT_TXT(char* text)
 {
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    PRINT_TIME();
    UART_SendMsg(UART_NUM,(U8 *)text,strlen(text));
    UART_SendMsg(UART_NUM,(U8 *)"\r\n",strlen("\r\n"));
    __set_PRIMASK(primask);
 }

 void PRINT_DEC(char* text,U32 dVAL)
 {
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    char *des_s=(char *)aDebugInfoBuffer;
    char **des=&des_s;
    char **des_start=&des_s;
    U8 len;
    PRINT_TIME();
    UART_SendMsg(UART_NUM,(U8 *)text,strlen(text));
    if(dVAL == 0)
	{
		UART_SendMsg(UART_NUM,(U8 *)"0",1);
	}
	else
	{
		printdec(des,dVAL);
	}
    len=(unsigned int)*des_start-(unsigned int)aDebugInfoBuffer;
    UART_SendMsg(UART_NUM,aDebugInfoBuffer,len);
    UART_SendMsg(UART_NUM,(U8 *)"\r\n",strlen("\r\n"));
    __set_PRIMASK(primask);
 }
 void PRINT_HEX(char* text,U32 dVAL)
 {
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);
    char *des_s=(char *)aDebugInfoBuffer;
    char **des=&des_s;
    char **des_start=&des_s;
    U8 len;
    PRINT_TIME();
    UART_SendMsg(UART_NUM,(U8 *)text,strlen(text));
    if(dVAL == 0)
    {
    	UART_SendMsg(UART_NUM,(U8 *)"0x0",3);
    }
    else
    {
    	printhex(des,dVAL);
    }
    len=(unsigned int)*des_start-(unsigned int)aDebugInfoBuffer;
    UART_SendMsg(UART_NUM,aDebugInfoBuffer,len);
    UART_SendMsg(UART_NUM,(U8 *)"\r\n",strlen("\r\n"));
    __set_PRIMASK(primask);
 }
#endif

#if (GPIO_DEBUG_ON == 1)
void GPIO_DEBUGDW(U32 dwDATA)
{
    SG_PADCTRL->rPORT_DATA14.bits.dVAL   = 0xDA;
    GPIO_DEBUG(ARRAY(dwDATA,0));
    GPIO_DEBUG(ARRAY(dwDATA,1));
    GPIO_DEBUG(ARRAY(dwDATA,2));
    GPIO_DEBUG(ARRAY(dwDATA,3));
    SG_PADCTRL->rPORT_DATA14.bits.dVAL   = 0xFF;
}

void GPIO_DEBUG_INIT(void)
{
    SG_PADCTRL->rPORT_MODE14.bits.dVAL=0;//GPIO3使能输出
    SG_PADCTRL->rPORT_DATA14.bits.dVAL=0xFF;//GPIO3输出FF
}
#else

#endif

#if (UART_DEBUG_ON == 1)
/******************DEBUG_ON == 1***********************/
#include <stdio.h>
#include <stdarg.h>

const char TLB_misc[6][35]={
        "TLB read protection (data)",
        "TLB write protection (data)",
        "TLB non-executable page (Inst)",
        "TLB page modified (data)",
        "TLB Access bit (+I/D bit)",
        "Reserved PTE Attribute (+I/D bit)",
};
const char Machine_error[6][50]={
        "Cache locking error (+I/D bit)",
        "TLB locking error (+I/D bit)",
        "TLB multiple hit (+I/D bit)",
        "Parity/ECC error (+I/D bit)",
        "Unimplemented page size error",
        "Illegal parallel memory accesses (Audio extension)",
};

const char General_exception[12][55]={
        "Alignment check (+I/D bit) or branch target alignment",
        "Reserved instruction",
        "Trap",
        "Arithmetic",
        "Precise bus error (+I/D bit)",
        "Imprecise bus error (+I/D bit)",
        "Coprocessor",
        "Privileged instruction",
        "RFU",
        "Nonexistent memory address (+I/D bit)",
        "MPZIU Control (+I/D bit)",
        "Next precise stack overflow",
};

const char I_TYPE[8][20]={
    "Reset/NMI",
    "TLB fill",
    "PTE not present",//software reset
    "TLB misc",//EXECUTE DEVICE DIAGNOSTIC command protocol
    "TLB VLPT miss",
    "Machine Error",
    "Debug related",
    "General exception",
};

const char TRSP_TYPE[15][20]={
    
    "POWERON_COMRESET",
    "DEV_IDLE",
    "SRESET",//software reset
    "EXECUTE_DEVICE_DIGA",//EXECUTE DEVICE DIAGNOSTIC command protocol
    "DEV_RESET",
    "NONE_DATA",
    "PIO_DATA_IN",
    "PIO_DATA_OUT",
    "DMA_DATA_IN",
    "DMA_DATA_OUT",
    "PACKET",
    "READ_DMA_QUEUED",
    "WRITE_DMA_QUEUED",
    "FPDMA_QUEUED",
    "UNKNOWN_YET",
};
const char LPS_STATE[8][15]={

    "LPS_OFF_LINE",// Host not present or communication not established or lost
    "LPS_ACTIVE",
    "LPS_PARTIAL",
    "",
    "",
    "",
    "LPS_SLUMBER", 
    "LPS_DEV_SLEEP",
};//Device Power state
//#pragma GCC optimize("O0")

// *****************************************************************************
// 函数名称: DEBUG_PRINT
// 函数功能: 调试信息打印接口
// 入口参数: 
//			 format：输入参数格式，标准C语言格式化方法
//			 ...   ：变量列表
// 出口参数: 格式化后的字符串长度
// 说	 明: 
// *****************************************************************************
int stringformat(char *des,const char *format,...)
{
    va_list args;
    U16 n;
    va_start(args, format);
    n = vsprintf((char *)aDebugInfoBuffer, format, args);
    va_end(args);
    return n;
}

int PRINT_E(const char *format,...)
{
    va_list args;
    U16 n;
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);

    va_start(args, format);
    n = vsprintf((char *)aDebugInfoBuffer, format, args);

    UART_SendMsg(UART_NUM,(U8 *)aDebugInfoBuffer,n);//输出到终端
     __set_PRIMASK(primask);
    return n;
}
int PRINT(const char *format,...)
{
    if(SG_CPM->rCPM_INT_CTRL.bits.DEVSLP_EXIT_INT_EN == 1)
    {
        //DEVSLP 唤醒的时候，UART还没有初始化完成，没有办法打印
        return 0;
    }
    va_list args;
    U16 n;
    U32 primask=__get_PRIMASK();
    __set_PRIMASK(1);

     n=stringformat((char *)aDebugInfoBuffer,"[--%s--][%010d]:",code_name,tSataDC.dPowerOnTime);
     UART_SendMsg(UART_NUM,aDebugInfoBuffer,n);//输出到终端

    va_start(args, format);
    n = vsprintf((char *)aDebugInfoBuffer, format, args);

    UART_SendMsg(UART_NUM,(U8 *)aDebugInfoBuffer,n);//输出到终端
     __set_PRIMASK(primask);
    return n;
}
#ifdef __PFM_MONITOR__
#include "mcu.h"
void pfm_restart()
{
    PRINT("performance count start....\r\n");
    __enable_performance();
}
void pfm_print(const char *flgstr,U8 flg)
{
    U32 __pfm_count=__get_pfm_count();
    PRINT(flgstr);
    PRINT_E(">>>>\r\n");
    if(flg == 1)
    {
        U32 time =__pfm_count/tSataDC.dMicroSecondCnt;
        PRINT(">>>>performance monitor counter= %#010lx[%d us]\r\n", __pfm_count,time);
    }
    __enable_performance();
}
#endif
const char HEX_CHAR[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
int UART_PRINT_BUFF(U8 *pBuff,U32 dBuffSize)
{
    U8 i;
    U8 bCnt;
    U8 bVal;
    while(dBuffSize)
    {
        if (dBuffSize>16)
        {
            bCnt = 16;
        }
        else
        {
            bCnt = (U8)dBuffSize;
        }
        for (i = 0;i<bCnt;i++)
        {
            bVal = *pBuff;
            aDebugInfoBuffer[i*3+0] = HEX_CHAR[(U8)(bVal>>4)];
            aDebugInfoBuffer[i*3+1] = HEX_CHAR[(U8)(bVal&0x0F)];
            aDebugInfoBuffer[i*3+2] = ' ';
            pBuff++;
        }
        aDebugInfoBuffer[i*3+0] = ' ';
        aDebugInfoBuffer[i*3+1] = '\r';
        aDebugInfoBuffer[i*3+2] = '\n';
        UART_SendMsg(UART_NUM,(U8 *)aDebugInfoBuffer,(bCnt+1)*3);
        dBuffSize -= bCnt;
    }
    return dBuffSize;
}
#endif//#if (UART_DEBUG_ON == 1)


#if	(SIMULATION==1)

// *****************************************************************************
// 函数名称: fputc
// 函数功能: 软件仿真重定向输出到debug printf viewer
// 入口参数: 
//			 ch：字符
//			 *f  ：文件
// 出口参数: 格式化后的字符串长度
// 说	 明: 
// *****************************************************************************
int fputc(int ch, FILE *f) {
    if (DEMCR1 & TRCENA) {
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
    return 0;
}
#endif /*#if (SIMULATION==1)*/


