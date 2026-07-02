// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_security.c 
//  文件标识：    
//  内容摘要：    实现ATA8 Security Feature Set管理 
//  其它说明：     
//  项    目：    SG9081 
//  作    者：    WILLIAM 
//  完成日期：    2015年08月19日 
//   
//  修改记录 1： 
//        修改日期： 
//        版 本 号： 
//        修 改 人：
//        修改内容：   
//  修改记录 2：
// **********************************************************************
#include "ata_general.h"
#include "ata_security.h"
#include "sata_misc.h"
#include "ata_identify.h"
#include "debug.h"
#include "ata_gpl.h"
#include "uitility.h"

#if (SATA_LOG_ENABLE == 1)

// *****************************************************************************
// 函数名称: BuildIdfyLogPage06
// 函数功能: 根据Idfy 更新 IDFY LOG PAGE 06
// 入口参数: 待修改的缓冲区首地址
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void BuildIdfyLogPage06(void *pTable)
{
     T_IDFY_LOG06 *ptLog=(T_IDFY_LOG06*)pTable;
//     ptLog->Type.tSS.ENABLED = GetBits(unAtaIdentify.wArray+128,1,1);
//     ptLog->Type.tSS.LOCKED = GetBits(unAtaIdentify.wArray+128,2,1);
//     ptLog->Type.tSS.FROZEN = GetBits(unAtaIdentify.wArray+128,3,1);
//     ptLog->Type.tSS.COUNT_EX = GetBits(unAtaIdentify.wArray+128,4,1);
//     ptLog->Type.tSS.ESES = GetBits(unAtaIdentify.wArray+128,5,1);
     *(U8*)&ptLog->Type.tSS = (unAtaIdentify.wArray[128]>>1);
     ptLog->Type.tSS.SUP = (unAtaIdentify.wArray[128]&W_BIT0);     //GetBits(unAtaIdentify.wArray+128,0,1);
     ptLog->Type.tSS.MPC = ((unAtaIdentify.wArray[128]&W_BIT8)>>8);//GetBits(unAtaIdentify.wArray+128,8,1);
     ptLog->Type.tSS.S1 = 1;
}

void UpdateIDFYLogPage06Router(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    if(__pevent->dParam[0] == DIR_IN)
    {
        DEBUG_PRINT("UpdateIDFYLogPage06Router : get data\r\n");
        BuildIdfyLogPage06((void*)__pevent->dParam[3]);
        __pevent->bOpcode=OP_DI_DATA;
        __pevent->eStatus=CMD_ONGOING;
        __pevent->dParam[0]=DIR_OUT;
        __pevent->dParam[2]=1;
        __pevent->pfEventCompleteCb=UpdateIDFYLogPage06Router;
    }
    else
    {
        DEBUG_PRINT("UpdateIDFYLogPage06Router : write data over\r\n");
        __pevent->bOpcode=OP_NONE_EVENT;
        __pevent->pfEventCompleteCb=NULL;
        if(tSataDC.eTransType != POWERON_COMRESET)//如果是COMRESET 引发的更新，则不需要回复D2H,否则会出现重复D2H的BUG
        {
            ATACmd_ReleaseCommand(eStatus);
        }
        else
        {
            PRINT_TXT("IDFY P06");
            //切换到COMRESET流程回复D2H
            //SATA_ChangeState(POWERON_COMRESET,DHR0_HW_RESET_ASSERTED);
            UpdateIDFYLogPage08(CMD_IDLE,ptEvent);
        }
        //sync 任务结束
        WriterGPLLogsRouter(CMD_IDLE,NULL);//触发保存下tSataDC.eSecState的状态
    }
}

void UpdateIDFYLogPage06(CMD_STATUS eStatus,void *ptEvent)
{
    UpdateIDFYLog(eStatus,ptEvent,6,UpdateIDFYLogPage06Router);
}

void EraseUnitDone(CMD_STATUS eStatus,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    UpdateIDFYLogPage06(CMD_IDLE,__pevent);//全盘擦除完成，修改LOG数据
}

void EraseUnit(U8 bEraseMode ,void *ptEvent)
{
    T_EVENT *__pevent=(T_EVENT *)ptEvent;
    __pevent->bOpcode=OP_DI_ERASE_UNIT;
    __pevent->eStatus=CMD_ONGOING;
    __pevent->dParam[0]=bEraseMode;
    __pevent->pfEventCompleteCb=EraseUnitDone;
}

 void UpdateOrComparePwdRouter(CMD_STATUS eStatus,void *ptEvent)
 {
    T_EVENT *__pevent=(T_EVENT *)ptEvent;

    U8 I, flg=0;
    if(__pevent->dParam[0] == DIR_IN)
    {
        if(__pevent->bStatus == 0xF1)
        {
            if(__pevent->bRFU == (U8)T_USER)
            {
                /*The MASTER PASSWORD CAPABILITY bit is modified during the processing of a SECURITY SET PASSWORD
                command (see 7.41) that specifies a User password.*/
                //更新user 密码
                MemoryCopy((U8*)__pevent->dParam[3],tSataLogCtx.bPassword,32);//写入新密码
                flg=1;
            }
            else
            {
                //更新master 密码
                T_MASTER_PWD* __pwd=(T_MASTER_PWD *)__pevent->dParam[3];

                //寻找是否有覆盖的记录
                for(I=0;I<15;I++)
                {
                    if(__pevent->dParam[4] == __pwd->tPWD[I].wIDENTIFY)
                    {
                        //覆盖已有的数据
                        MemoryCopy((U8*)__pwd->tPWD[I].bPWD,tSataLogCtx.bPassword,32);//写入新密码
                        flg=1;//已经记录完成
                    }
                }

                if(flg == 0)
                {
                    //没有相同的ID，找一个空位赋值
                    for(I=0;I<15;I++)
                    {
                        if(__pwd->tPWD[I].wIDENTIFY == 0x0000)
                        {
                            //覆盖已有的数据
                            MemoryCopy((U8*)__pwd->tPWD[I].bPWD,tSataLogCtx.bPassword,32);//写入新密码
                            __pwd->tPWD[I].wIDENTIFY=__pevent->dParam[4];
                            flg=1;//已经记录完成
                        }
                    }
                }
                //新的数据
            }
            if(flg == 1)
            {

                __pevent->bOpcode=OP_DI_DATA;
                __pevent->eStatus=CMD_ONGOING;
                __pevent->dParam[0]=DIR_OUT;
                __pevent->dParam[2]=1;
                __pevent->pfEventCompleteCb=UpdateOrComparePwdRouter;
            }
            else
            {
                //MASTER 记录满了
                //命令回复ABORT
                eStatus=CMD_ABORT;
                goto event_done;
            }
        }
        else if(__pevent->bStatus == 0xF2 || __pevent->bStatus == 0xF4 || __pevent->bStatus == 0xF6)
        {
            //unlock
            if(__pevent->bRFU == (U8)T_USER)
            {
                if(FALSE == MemoryCmp((U8*)tSataLogCtx.bPassword,(U8*)__pevent->dParam[3],32))
                {
                    //密码对比成功
                    if(__pevent->bStatus == 0xF2)
                    {
                        tSataDC.eSecState=ATA_SEC5;
                        //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)1);//enable
                        unAtaIdentify.wArray[128]|=(W_BIT1);
                        //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)1);//enable
                        unAtaIdentify.wArray[85]|=(W_BIT1);
                        //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
                        //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
                        unAtaIdentify.wArray[128]&=~(W_BIT2|W_BIT3);
                        UpdateIDFYLogPage06(CMD_IDLE,__pevent);
                        //PRINT_TXT("PWD OK F2");
                    }
                    else if(__pevent->bStatus == 0xF4)
                    {
                        tSataDC.eSecState=ATA_SEC1;
                        /*
                         * If the device processes a power-on reset or a hardware reset, then the device shall:
                            a) clear the SECURITY COUNT EXPIRED bit to zero; and
                            b) set the password attempt counter to five.*/
                        tSataDC.bSecPwdAttemptsCnt=5;
                        tSataDC.bIsSecPwdAttemptsExceeded=FALSE;

                        //SetBits(unAtaIdentify.wArray+128,(U8)4,(U8)1,0);
                        //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)0);
                        //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
                        //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
                        unAtaIdentify.wArray[128]&=~(W_BIT1|W_BIT2|W_BIT3|W_BIT4);
                        //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)0);
                        unAtaIdentify.wArray[85]&=~(W_BIT1);
                        //擦除全盘数据
                        EraseUnit(__pevent->dParam[4],__pevent);
                        //PRINT_TXT("PWD OK F4");
                    }
                    else if(__pevent->bStatus == 0xF6)
                    {
                        tSataDC.eSecState=ATA_SEC1;
                        //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)0);
                        //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
                        //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
                        unAtaIdentify.wArray[128]&=~(W_BIT1|W_BIT2|W_BIT3);
                        //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)0);
                        unAtaIdentify.wArray[85]&=~(W_BIT1);
                        UpdateIDFYLogPage06(CMD_IDLE,__pevent);
                        //PRINT_TXT("PWD OK F6");
                    }
                    return;
                }
                else
                {
                    //密码对比失败
                    tSataDC.bSecPwdAttemptsCnt--;
                    if(tSataDC.bSecPwdAttemptsCnt == 0)
                    {
                        //SetBits(unAtaIdentify.wArray+128,(U8)4,(U8)1,1);//Security count expired =1
                        unAtaIdentify.wArray[128]|=(W_BIT4);
                    }
                    UpdateIDFYLogPage06(CMD_ABORT,__pevent);
                    PRINT_TXT("E:PWD");
                    return;
                }
            }
            else
            {
                T_MASTER_PWD* __pwd=(T_MASTER_PWD *)__pevent->dParam[3];
                //compare 所有存在的 master
                for(I=0;I<15;I++)
                {
                    if(__pwd->tPWD[I].wIDENTIFY != 0x0000)
                    {
                        //覆盖已有的数据
                        if(FALSE == MemoryCmp((U8*)tSataLogCtx.bPassword,(U8*)__pwd->tPWD[I].bPWD,32))
                        {
                            //密码对比成功
                            if(__pevent->bStatus == 0xF2)
                            {
                                tSataDC.eSecState=ATA_SEC5;
                                SG_DSATA->rHWD2HEN.dAll=1;//SEC4状态退出，要启动NCQ 自动回复D2H功能
                                //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)1);//enable
                                unAtaIdentify.wArray[128]|=(W_BIT1);
                                //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)1);//enable
                                unAtaIdentify.wArray[85]|=(W_BIT1);
                                //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
                                //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
                                unAtaIdentify.wArray[128]&=~(W_BIT2|W_BIT3);
                                UpdateIDFYLogPage06(CMD_IDLE,__pevent);
                                //PRINT_TXT("PWD OK F2");
                            }
                            else if(__pevent->bStatus == 0xF4)
                            {
                                tSataDC.eSecState=ATA_SEC1;
                                SG_DSATA->rHWD2HEN.dAll=1;//SEC4状态退出，要启动NCQ 自动回复D2H功能
                                //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)0);
                                //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
                                //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
                                unAtaIdentify.wArray[128]&=~(W_BIT1|W_BIT2|W_BIT3);
                                //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)0);
                                unAtaIdentify.wArray[85]&=~(W_BIT1);
                                //擦除全盘数据
                                EraseUnit(__pevent->dParam[4],__pevent);
                                //PRINT_TXT("PWD OK F4");
                            }
                            else if(__pevent->bStatus == 0xF6)
                            {
                                tSataDC.eSecState=ATA_SEC1;
                                SG_DSATA->rHWD2HEN.dAll=1;//SEC4状态退出，要启动NCQ 自动回复D2H功能
                                //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)0);
                                //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
                                //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
                                unAtaIdentify.wArray[128]&=~(W_BIT1|W_BIT2|W_BIT3);
                                //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)0);
                                unAtaIdentify.wArray[85]&=~(W_BIT1);
                                UpdateIDFYLogPage06(CMD_IDLE,__pevent);
                                //PRINT_TXT("PWD OK F6");
                            }
                            return;
                        }
                    }
                }
                //密码对比失败
                if(tSataDC.eSecState==ATA_SEC4 && tSataDC.bSecPwdAttemptsCnt>0)
                tSataDC.bSecPwdAttemptsCnt--;

                if(tSataDC.bSecPwdAttemptsCnt == 0)
                {
                    //SetBits(unAtaIdentify.wArray+128,(U8)4,(U8)1,1);//Security count expired =1
                    unAtaIdentify.wArray[128]|=(W_BIT4);
                }
                UpdateIDFYLogPage06(CMD_ABORT,__pevent);
                PRINT_TXT("E:PWD");
                return;
            }

        }
    }
    else
    {
        if(__pevent->bStatus == 0xF1 && __pevent->bRFU == (U8)T_USER)
        {
            //更新SECURITY PAGE LOG
            //SetBits(unAtaIdentify.wArray+128,(U8)1,(U8)1,(U8)1);//enable
            unAtaIdentify.wArray[128]|=(W_BIT1);
            //SetBits(unAtaIdentify.wArray+85,(U8)1,(U8)1,(U8)1);//enable
            unAtaIdentify.wArray[85]|=(W_BIT1);
            //SetBits(unAtaIdentify.wArray+128,(U8)2,(U8)1,(U8)0);
            //SetBits(unAtaIdentify.wArray+128,(U8)3,(U8)1,(U8)0);
            unAtaIdentify.wArray[128]&=~(W_BIT2|W_BIT3);
            tSataDC.eSecState=ATA_SEC5;
            UpdateIDFYLogPage06(CMD_IDLE,__pevent);
            //PRINT_TXT("PWD SET OK");
        }
        else
        {
event_done:
            __pevent->bOpcode=OP_NONE_EVENT;
            __pevent->pfEventCompleteCb=NULL;
            //TODO:可能会有异常处理
            ATACmd_ReleaseCommand(eStatus);
            //sync 任务结束
             if(tSataLogCtx.bIsGplRunning == TRUE)
             {
                 //--由于涉及到共用combuff的问题，所以要重新启动可能正在进行中的任务
                WriterGPLLogsRouter(CMD_IDLE,NULL);
             }
        }
    }
 }

void read_password(U8 bCmd,T_PWDTYPE tType,U16 wParam)
{
    tSataDC.tSyncEvent.bOpcode=OP_DI_DATA;
    tSataDC.tSyncEvent.eStatus=CMD_ONGOING;
    tSataDC.tSyncEvent.dParam[0]=DIR_IN;
    tSataDC.tSyncEvent.dParam[1]=tSataDC.tSataMI.dVirtualLbaStartAddr + DEVICE_PASSWORD_OFFSET + tType;
    tSataDC.tSyncEvent.dParam[2]=1;
    tSataDC.tSyncEvent.dParam[3]=(U32)tSyncBuffer.bArray;
    tSataDC.tSyncEvent.dParam[4]=wParam;
    tSataDC.tSyncEvent.bRFU=(U8)tType;
    tSataDC.tSyncEvent.bStatus = bCmd;
    tSataDC.tSyncEvent.pfEventCompleteCb=UpdateOrComparePwdRouter;
}

void security_set_password_F1(U8* ptable)
{
    T_SSP_DC *__pssp=(T_SSP_DC *)ptable;
    if(__pssp->CW.IDENTIFY == (U8)T_USER)
    {
        /*The MASTER
            PASSWORD CAPABILITY bit (see A.11.8.3.2) shall remain unchanged*/
        tSataLogCtx.bMPCFlg=__pssp->CW.NMPC;
        //SetBits(unAtaIdentify.wArray+128,(U8)8,(U8)1,tSataLogCtx.bMPCFlg);
        unAtaIdentify.wArray[128]&=(~(1<<8));
        unAtaIdentify.wArray[128]|=((tSataLogCtx.bMPCFlg)<<8);

        //设定用户密码,创建DI任务，保存密码
        //改写B0h，00page内容
        MemoryCopy((U8*)tSataLogCtx.bPassword,__pssp->arrbPWD,32);//暂存数据
        read_password(0xF1,T_USER,0);
    }
    else
    {
        if(__pssp->NMPI != 0x0000 && __pssp->NMPI != 0xFFFF)
        {
            MemoryCopy((U8*)tSataLogCtx.bPassword,__pssp->arrbPWD,32);
            read_password(0xF1,T_MASTER,__pssp->NMPI);
        }
        else
        {
            /*
             * If the NEW MASTER PASSWORD IDENTIFIER field contains 0000h or FFFFh, the device shall preserve the existing
                contents of the MASTER PASSWORD IDENTIFIER field and return successful command completion.
             * */
            ATACmd_ReleaseCommand(CMD_VALID);
        }
    }
}

void security_unlock_F2(U8* ptable)
{
    T_SUL_DC *__psul=(T_SUL_DC *)ptable;
    if(SEC_ENABLED)
    {
        if(__psul->CW.IDENTIFY == (U8)T_USER)
        {
            MemoryCopy((U8*)tSataLogCtx.bPassword,__psul->arrbPWD,32);//暂存数据
            read_password(0xF2,T_USER,0);
        }
        else
        {
            if(SEC_MPC == 0)
            {
                MemoryCopy((U8*)tSataLogCtx.bPassword,__psul->arrbPWD,32);
                read_password(0xF2,T_MASTER,0);
            }
            else
            {
                /*
                 * If the SECURITY ENABLED bit is set to one and the MASTER PASSWORD CAPABILITY bit is set to one (i.e., Maximum),
                    then:
                    a) if the IDENTIFIER bit is set to one (i.e., compare Master password), then the device shall return command
                    aborted; or
                    b) if the IDENTIFIER bit is cleared to zero (i.e., compare User password), then the contents of the PASSWORD
                    field shall be compared with the stored User password
                 * */
                ATACmd_ReleaseCommand(CMD_ABORT);
            }
        }
    }
    else
    {
        /*
         * If the SECURITY ENABLED bit (see A.11.8.3.7) is cleared to zero and the IDENTIFIER bit (see table 98) is cleared to
            zero (i.e., compare User password), then the device shall return command aborted
         * */
        if(__psul->CW.IDENTIFY == (U8)T_USER)
        ATACmd_ReleaseCommand(CMD_ABORT);
        else
        {
            //????
            MemoryCopy((U8*)tSataLogCtx.bPassword,__psul->arrbPWD,32);
            read_password(0xF2,T_MASTER,0);
        }
    }
}

void security_erase_unit_F4(U8* ptable)
{
    T_SEU_DC *__pseu=(T_SEU_DC *)ptable;

    if(!SEC_EHERASE && __pseu->CW.ERASE_MODE == T_ENHANCED)
    {
        /**
         * the ERASE MODE bit was set to one and the device does not support Enhanced Erase mode
         */
        ATACmd_ReleaseCommand(CMD_ABORT);
    }
    else
    {
        if(SEC_ENABLED)
        {
            if(__pseu->CW.IDENTIFY == (U8)T_USER)
            {
                MemoryCopy((U8*)tSataLogCtx.bPassword,__pseu->arrbPWD,32);//暂存数据
                read_password(0xF4,T_USER,__pseu->CW.ERASE_MODE);
            }
            else
            {
                MemoryCopy((U8*)tSataLogCtx.bPassword,__pseu->arrbPWD,32);
                read_password(0xF4,T_MASTER,__pseu->CW.ERASE_MODE);
            }
        }
        else
        {
            /*If the SECURITY ENABLED bit (see A.11.8.3.7) is cleared to zero and the IDENTIFIER bit (see table 93) is cleared to
            zero (i.e.,compare User password), then the device shall return command aborted.*/
            if(__pseu->CW.IDENTIFY == (U8)T_USER)
            ATACmd_ReleaseCommand(CMD_ABORT);
            else
            {
                //????
                MemoryCopy((U8*)tSataLogCtx.bPassword,__pseu->arrbPWD,32);
                read_password(0xF4,T_MASTER,__pseu->CW.ERASE_MODE);
            }
        }
    }

}

void security_disable_password_F6(U8* ptable)
{
    T_SUL_DC *__psul=(T_SUL_DC *)ptable;
    if(SEC_ENABLED)
    {
        if(__psul->CW.IDENTIFY == (U8)T_USER)
        {
            MemoryCopy((U8*)tSataLogCtx.bPassword,__psul->arrbPWD,32);//暂存数据
            read_password(0xF6,T_USER,0);
        }
        else
        {
            if(SEC_MPC == 0)
            {
                MemoryCopy((U8*)tSataLogCtx.bPassword,__psul->arrbPWD,32);
                read_password(0xF6,T_MASTER,0);
            }
            else
            {
                /*
                 * If the SECURITY ENABLED bit is set to one and the MASTER PASSWORD CAPABILITY bit is set to one (i.e., Maximum),
                    then:
                    a) if the IDENTIFIER bit is set to one (i.e., compare Master password), then the device shall return command
                    aborted; or
                    b) if the IDENTIFIER bit is cleared to zero (i.e., compare User password), then the contents of the PASSWORD
                    field shall be compared with the stored User password
                 * */
                ATACmd_ReleaseCommand(CMD_ABORT);
            }
        }
    }
    else
    {
        if(__psul->CW.IDENTIFY == (U8)T_USER)
        {
            /*
             * if the IDENTIFIER bit is cleared to zero (i.e., compare User password), the device shall return command
             * aborted; or
             * */
            ATACmd_ReleaseCommand(CMD_ABORT);
        }
        else
        {
            MemoryCopy((U8*)tSataLogCtx.bPassword,__psul->arrbPWD,32);
            read_password(0xF6,T_MASTER,0);
        }
    }
}

// *****************************************************************************
// 函数名称: ATACmd_SecuritySetPassWord
// 函数功能: 实现ATA命令 SECURITY SET PASSWORD
// 入口参数: NONE
// 出口参数:
// 说    明: SECURITY SET PASSWORD – F1h, PIO Data-Out
// *****************************************************************************
void ATACmd_SecuritySetPassWord(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(SEC_LOCKED || SEC_FROZEN)
    {
        /*
         * The device shall not modify the Security page (see A.11.8) of the IDENTIFY DEVICE data log and shall return
            command aborted if:
            a) the SECURITY LOCKED bit (see A.11.8.3.6) is set to one; or
            b) the SECURITY FROZEN bit (see A.11.8.3.5) is set to one.
         * */
        //abort
        ATACmd_ReleaseCommand(CMD_ABORT);
    }
    else
    {
        ReceiveDataFromHost(0xF1);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SecurityUnLock
// 函数功能: 实现ATA命令 SECURITY FREEZE UNLOCK
// 入口参数: NONE
// 出口参数:
// 说    明: SECURITY UNLOCK – F2h, PIO Data-Out
// *****************************************************************************
void ATACmd_SecurityUnLock(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(SEC_EXPIRED || SEC_FROZEN || tSataDC.bSecPwdAttemptsCnt == 0)
    {
        /*The device shall not modify the Security page (see A.11.8) of the IDENTIFY DEVICE data log and shall return
            command aborted if:
            a) the contents of the PASSWORD field (see table 98) are not the same as the stored password specified by
            the IDENTIFIER bit (see 7.42.2);
            b) the SECURITY FROZEN bit (see A.11.8.3.5) is set to one; or
            c) the SECURITY COUNT EXPIRED bit (see A.11.8.3.4) is set to one.
         * */
        ATACmd_ReleaseCommand(CMD_ABORT);
    }
    else
    {
        ReceiveDataFromHost(0xF2);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SecurityErasePrepare
// 函数功能: 实现ATA命令 SECURITY  ERASE PREPARE
// 入口参数: NONE
// 出口参数:
// 说    明: SECURITY ERASE PREPARE – F3h, Non-Data
// *****************************************************************************
void ATACmd_SecurityErasePrepare(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    CMD_STATUS eStatus=CMD_IDLE;
    if(SEC_FROZEN)
    {
        eStatus=CMD_ABORT;
    }
    else
    {
        tSataDC.bIsSecErasePrepared=TRUE;
    }
    ATACmd_ReleaseCommand(eStatus);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SecurityEraseUnit
// 函数功能: 实现ATA命令 SECURITY  ERASE UNIT
// 入口参数: NONE
// 出口参数:
// 说    明: SECURITY ERASE UNIT – F4h, PIO Data-Out
// *****************************************************************************
void ATACmd_SecurityEraseUnit(void)
{
   DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
   if(tSataDC.bIsSecErasePrepared==FALSE || SEC_FROZEN)
   {
       /*
        *
        * The device shall return command aborted if:
            a) this command was not immediately preceded by a SECURITY ERASE PREPARE command;
        * */
       ATACmd_ReleaseCommand(CMD_ABORT);
   }
   else
   {
       ReceiveDataFromHost(0xF4);
   }
   DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SecurityFreezeLock
// 函数功能: 实现ATA命令 SECURITY FREEZE LOCK
// 入口参数: NONE
// 出口参数: 
// 说    明: SECURITY FREEZE LOCK - F5h, Non-Data
// *****************************************************************************
void ATACmd_SecurityFreezeLock(void)
{
    CMD_STATUS eStatus=CMD_DONE;

    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    DEBUG_PRINT("tSataDC.bIsSecFreezeLocked = %d \r\n",tSataDC.bIsSecFreezeLocked );

    if(tSataDC.eSecState == ATA_SEC1)
    {
        tSataDC.eSecState=ATA_SEC2;
    }
    else if(tSataDC.eSecState == ATA_SEC5)
    {
        tSataDC.eSecState=ATA_SEC6;
    }

    if(tSataDC.eSecState == ATA_SEC2 || tSataDC.eSecState == ATA_SEC6)
    {
        tSataDC.bIsSecFreezeLocked=TRUE;
        //SetBits(unAtaIdentify.wArray+128,3,1,1);
        unAtaIdentify.wArray[128]|=(W_BIT3);
        UpdateIDFYLogPage06(CMD_IDLE,&tSataDC.tSyncEvent);
        return;
    }
    else
    {
        /*
         * The ABORT bit shall be set to one if the device is in the SEC3: Powered down/Security Enabled/Locked/Not
            Frozen state (see 4.18.11.7) or the SEC4: Security Enabled/Locked/Not Frozen state
         * */
        eStatus=CMD_ABORT;
    }

    ATACmd_ReleaseCommand(eStatus);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}

// *****************************************************************************
// 函数名称: ATACmd_SecurityDisablePassword
// 函数功能: 实现ATA命令 SECURITY DISABLE PASSWORD
// 入口参数: NONE
// 出口参数:
// 说    明: SECURITY DISABLE PASSWORD – F6h, PIO Data-Out
// *****************************************************************************
void ATACmd_SecurityDisablePassword(void)
{
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    if(!SEC_SUPPORT || SEC_FROZEN || SEC_LOCKED)
    {
        ATACmd_ReleaseCommand(CMD_ABORT);
    }
    else
    {
        ReceiveDataFromHost(0xF6);
    }
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#else
// *****************************************************************************
// 函数名称: ATACmd_SecurityFreezeLock
// 函数功能: 实现ATA命令 SECURITY FREEZE LOCK
// 入口参数: NONE
// 出口参数:
// 说    明: SECURITY FREEZE LOCK - F5h, Non-Data
// *****************************************************************************
void ATACmd_SecurityFreezeLock(void)
{
    CMD_STATUS eStatus=CMD_DONE;
    DEBUG_PRINT("Enter func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
    DEBUG_PRINT("tSataDC.bIsSecFreezeLocked = %d \r\n",tSataDC.bIsSecFreezeLocked );

    tSataDC.bIsSecFreezeLocked=TRUE;
    //SetBits(unAtaIdentify.wArray+128,3,1,1);
    unAtaIdentify.wArray[128]|=(W_BIT3);
    ATACmd_ReleaseCommand(eStatus);
    DEBUG_PRINT("Exit func=<%s>[file=%s,line=%d]\r\n",__func__,__MODULE__,__LINE__);
}
#endif
