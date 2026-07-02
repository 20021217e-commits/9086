#ifndef _SCSI_H_
#define _SCSI_H_

#define LBA_00005500 (0x00005500)
#define LBA_00000055 (0x00000055)
#define LBA_0000AA00 (0x0000AA00)
#define LBA_000000AA (0x000000AA)
#define LBA_000055AA (0x000055AA)

#define NORMAL_MODE  (0x00)
#define SCSI_MODE    (0x01) 

#ifndef OP_WR
#define OP_WR        (0x00)
#endif

#ifndef OP_RD
#define OP_RD        (0x01)
#endif

#define CMD_STATE    (0x00)
#define DATA_STATE   (0x01)

#define CMD_INVAILD  (0x00)
#define CMD_VAILD    (0x01)
#define CMD_RESPONSE (0x02)

#define HEAD_KEY_CNT (8)

//命令包结构体
#define CMD_TITILE_SIZE            (8)//sata控制命令头大小
#define CMD_DEFINE_DATA_SIZE    (468)//sata自定义数据大小

#define WR_SUB_CMD  (0x00)
#define RD_SUB_CMD  (0x01)

typedef struct
{
  U32 dReadFlag;		//用于量产工具识别返回的数据，是否是想要的数据
  U32 dN_CheckSum;		//Check sum的反码，用于量产工具判断是否进行CheckSum校验
  U32 dP_CheckSum;
  U32 dN_ECCFailFlag;
  U32 dP_ECCFailFlag;	//Fail时，根据主控返回的状态
  U32 dReverse1;
}T_PrivDataVerify;

typedef struct _CMD_HEAD
{
    //头标记
    U32 dTitle[CMD_TITILE_SIZE];
    //操作码
    U8 bOpCode;
    //子操作码
    U8 bSubOpCode;
    //SCT
    U16 wSCT;
    //保留
//    U32 dReserve;
    U8 	bEnhanceEnable;
	U8 	bReserve1;
	U16 wReserve2;
    //自定义数据
    U8 bDataDefine[CMD_DEFINE_DATA_SIZE - sizeof(T_PrivDataVerify)];

    T_PrivDataVerify tPrivData;
    //校验和
    U32 dCheckSum;

}T_SCSI_CMD_HEAD;

typedef struct _DATA_HEAD
{
    //头标记
    U32 dTitle[CMD_TITILE_SIZE];
    //保留
    U32 dReserve[2];
    //自定义数据
    U8 bDataDefine[CMD_DEFINE_DATA_SIZE - sizeof(T_PrivDataVerify)];

    T_PrivDataVerify tPrivData;
    //校验和
    U32 dCheckSum;
}T_SCSI_DATA_HEAD;

typedef struct
{
	U8* bFwVersion;
    U8 (*ScsiIntCmdHandleExt)(U8 bOpMode);   //私有命令扩展中断预检查接口
    U8 (*ScsiIdleCmdHandleExt)(U8 bOpMode);   //私有命令扩展处理接口
    BOOL (*ScsiGetNandFlashId)(U8 *pDesBuff);//flash id读写接口，因为没有驱动，所以用来扩展
    BOOL (*ScsiGetValidCENandFlashId)(U8 *pDesBuff);//flash id读写接口，因为没有驱动，所以用来扩展
    U8 RFU1[3];
    U32 RFU2[2];
}T_SCSI_MODULE_ITF;  //sizeof(T_SCSI_MODULE_ITF) = 0x20

typedef struct
{
    U32 dNextAddr;
    U32 dWrValidFlg;
    U8  bMode;
    U8  bState;
    U8  bVaild;
    U8  bDir;
    U8  bMode_old;
	U8  bReserve[3];
    T_SCSI_MODULE_ITF tScsiMI;
}T_SCSI_CTRL;

/****************************************************************/
#define FW_VERSION_SIZE (40)
#define FW_CH_NUM       (4)
#define FW_CE_NUM       (4)
#define IC_VERSION_SIZE (8)
typedef struct
{
    U8  bG0FwVersion[FW_VERSION_SIZE];
    U8  bG3FwVersion[FW_VERSION_SIZE];
    U8  bFlashID[FW_CH_NUM][FW_CE_NUM][8];
    U32 dResReg0Val;
    U8  bICVersion[IC_VERSION_SIZE];
}T_BASIS_INFO;

typedef struct  
{
    U32 dRamAddr;
    U16 wSctCnt;
    U8  bReserve[2];
}T_OP_RAM_INFO;

typedef struct
{
   U8  bG0RunningEn;//控制G0是否使能，0：dis，1：en
   U8  bReserve1[3];
   U32 dG0PCAddr;    //设置G0PC指针地址
   U32 dG0VectorAddr;//设置G0的向量地址
   U8  bG3RunningEn;//控制G0是否使能，0：dis，1：en
   U8  bReserve2[3];
   U32 dG3PCAddr;   //设置G3PC指针地址
   U8  bReserve3[4];
   U32 dResReg0Val;
   U8  bReserve4[480];
   U32 dCheckSum;
}T_CORE_SWITCH_INFO;

/****************************************************************/
extern U32_C dRDCmdHead[];
extern U32_C dWRCmdHead[];

extern T_SCSI_CTRL    tScsiCtl;
extern T_SCSI_CMD_HEAD tScsiCmdHead;
extern T_SCSI_DATA_HEAD tScsiDataHead;

U8   Scsi_CmdHeadVerify(void);
U8   Scsi_ScsiModeVerify(U32 dLba,U16 wSct,U8 bOpMode);
U8   Scsi_IntCmdHandle(U8 bOpMode);
U8   Scsi_IdleCmdHandle(U8 bOpMode);
void Scsi_PriCmdHandle(U8 bOpMode);

/*****SCSI 模块对外接口*****/
void Scsi_RegisteInterfaceInfo(T_SCSI_MODULE_ITF *ptScsiMI);
void Scsi_DummyReadWrite(U32 dwTag);

#endif
