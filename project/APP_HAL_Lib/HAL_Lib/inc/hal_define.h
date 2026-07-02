// *****************************************************************************
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：  hal.h
//  文件标识：  
//  内容摘要：  hal接口定义
//  其它说明：  
//  当前版本：  v0.10
//  项   目：  SG9081
//  作   者：  gary
// *****************************************************************************
#ifndef __HAL_DEFINE_H__
#define __HAL_DEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif
    #include "hal.h"
    #include "sg9081.h"

    typedef struct
    {
        T_REG_CFG *pHwReg;       //HAL参数指针，由上层初始化
        T_HAL_PARAM tHalParam;
    }T_GLOBAL_HAL_VAR;

    extern T_GLOBAL_HAL_VAR gHalVar;
#ifdef __cplusplus
}
#endif
U16 HAL_GetParityLen(void);
#endif
