// ********************************************************************* 
//  版权所有  (C) 2015,  硅格半导体有限公司 
//   
//  文件名称：    ata_identify.c 
//  文件标识：    
//  内容摘要：    ATA规范的IDENTIFY表格以及管理接口定义 
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

#include "ata_identify.h"
#include "debug.h"
#include "sata_pio_in.h"
#include "uitility.h"
/*Word 10-19*/
#define SERIAL            0x282B, 0x3836, 0x2B38, 0x2930, 0x3735, 0x352D, 0x3838, 0x3835, 0x3232, 0x3936  /*Serial number:(+86)0755-88852296      */

/*Word 23-26*/
#define FW_REV            0x3930, 0x3836, 0x4242, 0x3031/*Firmware revision:9081CC01*/

/*Word 27-46*/
#define MODEL             0x5953, 0x3930, 0x3836, 0x4242, 0x2D31, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020,\
                          0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020  /*Model number:SG9081CC-01        */
///*Word 60-61*/
#define CAPACITY_28BIT    ((U16)(DEVICE_CAPCITY&0xFFFF)),((U16)((DEVICE_CAPCITY>>16)&0xFFFF))

/*Word 100-103*/
#define CAPACITY_48BIT    ((U16)(DEVICE_CAPCITY&0xFFFF)),((U16)((DEVICE_CAPCITY>>16)&0xFFFF)), 0x0000, 0x0000

/*Word 129-159*/
#define VENDOR_SPEC       0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
                          0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
                          0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,\
                          0x0000

/*Word 176-205 CMSM(Current media serial number) (60 ASCII characters)*/
#define CMSM              0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020,\
                          0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020,\
                          0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020, 0x2020


//#if (DEVICE_CAPCITY >= 16,514,064U)
//#define LOGIC_CYLINDERS (0x3FFFU)
//#else
//#define LOGIC_CYLINDERS (DEVICE_CAPCITY>>0x10) /*扇区数=64，磁头数=16*/
//#endif

__align(8) UN_ATA_INDETIFY_T unAtaIdentify __section(".identify");

#ifdef __BOOT_CODE__
const UN_ATA_INDETIFY_T unAtaIdentify_C ={
        {0x0040,/*Word 00:General configuration bit-significant information:
                                  15 0 = ATA device                0
                                  14-8 Retired                    0000100
                                  7 1 = removable media device    0
                                  6 Obsolete                    1
                                  5-3 Retired                    001
                                  2 Response incomplete            0
                                  1 Retired                        1
                                  0 Reserved                    0*/
        0x0000,/*Word 01:Number of logical cylinders (ATA5) -柱面数，大于7.8G 容量时该字段等于3FFF*/
        0xC837,/*Word 02:Specific configurationDevice does not require SET FEATURES subcommand to spin-up after
                         power-up and IDENTIFY DEVICE data is complete*/
        0x0010,/*Word 03:Number of logical heads-磁头数*/
        0x0000,0x0000,/*Word 04-05: Retired*/
        0x003F,/*Word 06:Number of logical Scts per logical track-扇区数*/
        0x0000,0x0000,/*Word 07-08: Reserved for assignment by the CompactFlashTM Association*/
        0x0000,/*Word 09:Retired*/
        SERIAL,/*Word 10-19:Serial number (20 ASCII characters)*/
        0x0000, 0x0000, 0x0004,/*Word 20-22:Retired/Obsolete*/
        FW_REV,/*Word 23-26:Firmware revision (8 ASCII characters)*/
        MODEL, /*Word 27-46:Model number (40 ASCII characters)*/
        0x8001,/*47:Maximum number of Scts on READ/WRITE MULTIPLE
                                    15-8  80h                    80
                                    7-0   00h = Reserved         10
                                          01h-10h = Maximum number of Scts that shall be transferred  
                                                    per interrupt on READ/WRITE MULTIPLE commands
                                          11h-FFh = Reserved*/
        0x4000,/*Word 48:Trusted Computing feature set options
                                 15     Shall be cleared to zero                        0
                                 14     Shall be set to one                             1
                                 13-1   Reserved for the Trusted Computing Group        0000000000000            
                                 0      1 = Trusted Computing feature set is supported  0 = NOT SUPPORT*/
        0x2F00,/*Word 49:Capabilities
                                 15-14  Reserved for the IDENTIFY PACKET DEVICE command.                        00
                                 13     1 = Standby timer values as specified in this standard are supported    1
                                        0 = Standby timer values shall be managed by the device    
                                 12     Reserved for the IDENTIFY PACKET DEVICE command                            0
                                 11     1 = IORDY supported                                                        1
                                        0 = IORDY may be supported    
                                 10     1 = IORDY may be disabled                                                1
                                 9      1 = Shall be set to one (i.e. LBA supported)                            1
                                 8      1 = DMA supported                                                        1
                                 7-2    Reserved                                                                000000
                                 1-0    Long Physical Sct Alignment Error reporting                            00*/
        0x4000,/*Word 50:Capabilities    
                                 15        Shall be cleared to zero.                                                0
                                 14        Shall be set to one.                                                    1
                                 13-2   Reserved.                                                                000000000000                                                                
                                 1      Obsolete                                                                0
                                 0      1 = There is a minimum Standby time value and it is vendor specific.    0*/
        0x0200, 0x0000,/*Word 51-52: Obsolete*/
        0x0007,/*Word 53:          
                                 15-8   Free-fall Control Sensitivity                                            00000000
                                        00h=Vendor's recommended's setting    
                                        01h-ffh=Sensitivity level. A larger number is a more sensitive setting.    00
                                 7-3    Reserved                                                                00000
                                 2      1 = the fields reported in Word 88 are valid                            1
                                        0 = the fields reported in Word 88 are not valid    
                                 1      1 = the fields reported in Words (70:64) are valid                        1
                                        0 = the fields reported in Words (70:64) are not valid    
                                 1      Obsolete                                                                1*/
        0x3FFF,/*Word 54当前柱面数*/
        0x0010,/*Word 55当前磁头数*/
        0x003F,/*Word 56当前扇区数*/ 
        0xFC10,/*Word 57当前逻辑地址高位*/
        0x00FB,/*Word 58当前逻辑地址低位*/
               /*前逻辑地址=当前柱面数*当前磁头数*当前扇区数*/
        0x0101,/*Word 59:             
                                 15        1 = BLOCK ERASE EXT command is supported                                               0
                                 14        1 = OVERWRITE EXT command is supported                                                   0
                                 13        1 = CRYPTO SCRAMBLE EXT command is supported                                           0
                                 12        1 = Sanitize feature set is supported                                                   0
                                 11        1 = The commands allowed during a sanitize operation are as specified by this standard
                                        0 = The commands allowed during a sanitize operation are as specified by ACS-2"           0
                                 10        1 = The SANITIZE ANTIFREEZE LOCK EXT command is supported                               0
                                 9        Reserved                                                                               0
                                 8        1 = Multiple logical Sct setting is valid                                           1
                                 7-0    xxh = Current setting for number of logical Scts that shall be transferred 
                                        per DRQ data block on READ/WRITE Multiple commands                                       00000000*/
        CAPACITY_28BIT,/*Word 60-61:Total number of user addressable logical Scts for 28-bit commands*/
        0x0000,/*Word 62:Obsolete*/
        0x0007,/*Word 63:        15-11  Reserved    00000
                                 10     1 = MultiWord DMA mode 2 is selected                        0
                                        0 = MultiWord DMA mode 2 is not selected    
                                 9      1 = MultiWord DMA mode 1 is selected                        0
                                        0 = MultiWord DMA mode 1 is not selected    
                                 8      1 = MultiWord DMA mode 0 is selected                        0
                                        0 = MultiWord DMA mode 0 is not selected    
                                 7-3    Reserved                                                    00000
                                 2      1 = MultiWord DMA mode 2 and below are supported            1
                                 1      1 = MultiWord DMA mode 1 and below are supported            1
                                 0      1 = MultiWord DMA mode 0 is supported                         1*/
        0x0003,/*Word 64:        15-2   Reserved
                                 1-0    PIO modes 3 and 4 supported*/
        0x0078,/*Word 65:0078h (M) Minimum MultiWord DMA transfer cycle time per Word*/
        0x0078,/*Word 66:0078h (M) Minimum MultiWord DMA transfer cycle time per Word*/    
        0x0078,/*Word 67:0078h (M) Minimum MultiWord DMA transfer cycle time per Word*/
        0x0078,/*Word 68:0078h (M) Minimum MultiWord DMA transfer cycle time per Word
                                 15-0   Cycle time in nanoseconds*/
        0x0000,/*Word 69: Additional Supported
                                 15     1 = CFast Specification Support                                                        0
                                 14     1 = Deterministic data in trimmed LBA range(s) is supported                               0
                                 13     1 = Long Physical Sct Alignment Error Reporting Control is supported                   0
                                 12     1 = DEVICE CONFIGURATION IDENTIFY DMA and DEVICE CONFIGURATION SET DMA are supported   0
                                 11     1 = READ BUFFER DMA is supported                                                       0
                                 10     1 = WRITE BUFFER DMA is supported                                                       0
                                 9      1 = SET MAX SET PASSWord DMA and SET MAX UNLOCK DMA are supported                       0
                                 8      1 = DOWNLOAD MICROCODE DMA is supported                                                1
                                 7      Reserved for IEEE 1667                                                                   0
                                 6      0 = Optional ATA device 28-bit commands supported                                      0
                                 5      1 = Trimmed LBA range(s) returning zeroed data is supported                            0
                                 4      1 = Device Encrypts All User data                                                       0
                                 3      1 = Extended Number of User Addressable Scts is supported                                0
                                 2      1 = All write cache is non-volatile                                                    0
                                 1-0    Reserved                                                                               00*/
        0x0000,/*Word 70:Reserved*/
        0x0000, 0x0000, 0x0000, 0x0000,/*Word 71-74:Reserved for IDENTIFY PACKET DEVICE command.*/
        0x001F,/*Word 75:               
                                15-5    Reserved                          0    
                                4-0     Maximum queue depth - 1           32*/
        0x000E,/*Word 76:
                                15     Supports READ LOG DMA EXT as equivalent to READ LOG EXT                    1
                                14     Supports Device Automatic Partial to Slumber transitions                    0
                                13     Supports Host Automatic Partial to Slumber transitions                    0
                                12     Supports Native Command Queuing priority                                  0
                                11     Supports Unload while NCQ commands outstanding                            0
                                10     Supports Phy event counters                                                1
                                9      Supports receipt of host-initiated interface power management requests    1
                                8      Supports native command queuing                                            1
                                7-4    Reserved for future Serial ATA signaling speed grades                    0000
                                3      1 = Supports Serial ATA Gen-3 signaling speed (6.0Gbps)                    1
                                2      1 = Supports Serial ATA Gen-2 signaling speed (3.0Gbps)                    1
                                1      1 = Supports Serial ATA Gen-1 signaling speed (1.5Gbps)                    1
                                0      Shall be cleared to zero                                                    0*/
        0x0000,/*Word 77:Serial ATA Additional capabilities
                              15-8 Reserved
                              7 Supports DevSleep to reduced power state capability    1
                              6 Supports RECEIVE FPDMA QUEUED and SEND FPDMA QUEUED commands 
                              5 Supports NCQ Queue Management Command
                              4 Supports NCQ Streaming
                              3-1 Coded value indicating current negotiated Serial ATA signal speed
                              0 Shall be cleared to zero*/
        0x0000,/*Word 78:
                                15-12  Reserved for Serial ATA                                        0000
                                11     1 = Supports Rebuild Assist                                    0
                                10     1 = Supports Device Initiated Interface Power Management 
                                           Software Settings Preservation                            0
                                9      1 = Supports Hybrid Information                                0
                                8      1 = Device Sleep supported                                    0
                                7      1 = Supports NCQ Autosense                                    1
                                6      1 = Supports software settings preservation                    1
                                5      1 = Hardware Feature Control is supported                    0
                                4      1 = Supports in-order data delivery                            0
                                3      1 = Supports dev initiate interface power management            1
                                2      1 = Supports DMA Setup Auto-Activate optimization            1
                                1      1 = Supports non-zero buffer offsets in DMA Setup FIS        0
                                0      Shall be cleared to zero                                     0*/
        0x0000,/*Word 79:Serial ATA features enabled
                                15-12  Reserved                                                     0000
                                11     1 = Rebuild Assist enabled                                    0
                                10     Reserved                                                        0
                                9      1 = Hybrid Information feature is enabled                    0
                                8      1 = Device Sleep enabled                                        0
                                7      1 = Device Automatic Partial to Slumber transitions enabled    0
                                6      1 = software settings preservation enabled                    1
                                5      1 = Hardware Feature Control is enabled                        0
                                4      1 = In-order data delivery enabled                            0
                                3      1 = Device initiating interface power management enabled        0
                                2      1 = DMA Setup Auto-Activate optimization enabled                0
                                1      1 = Non-zero buffer offsets in DMA Setup FIS enabled            0
                                0      Shall be cleared to zero                                     0*/
        0x03FC,/*Word 80:Major version number
                                0000h or FFFFh = device does not report version    
                                15-11  Reserved                     00000
                                10     1 = Supports ACS-3         0
                                9      1 = Supports ACS-2         1
                                8      1 = Supports ATA8-ACS     1
                                7      1 = Supports ATA/ATAPI-7     1
                                6      1 = Supports ATA/ATAPI-6     1
                                5      1 = Supports ATA/ATAPI-5     1
                                4      1 = Supports ATA/ATAPI-4     1
                                3      Obsolete                     1
                                2      Obsolete                     1
                                1      Obsolete                     0
                                0      Reserved                     0*/
        0x0000,/*Word 81:Minor revision number
                                0000h or FFFFh = device does not report version*/
        0x7008,/*Word 82:Commands and feature set supported
                                15 Obsolete    0
                                14 1 = NOP command supported    1
                                13 1 = READ BUFFER command supported    1
                                12 1 = WRITE BUFFER command supported    1
                                11 Obsolete    0
                                10 1 = Host Protected Area feature set supported    1
                                9 1 = DEVICE RESET command supported    0
                                8 1 = SERVICE interrupt supported    0
                                7 1 = release interrupt supported    0
                                6 1 = look-ahead supported    1
                                5 1 = write cache supported    1
                                4 Shall be cleared to zero to indicate that the PACKET feature set is not supported    0
                                3 Shall be set to one to indicate that the mandatory Power Management feature set is supported    1
                                2 Obsolete    0
                                1 1 = Security Mode feature set supported    1
                                0 1 = SMART feature set supported    1*/
        0x4000,/*Word 83:Commands and features sets supported
                               15 Shall be cleared to zero    0
                               14 Shall be set to one    1
                               13 1 = FLUSH CACHE EXT command supported    1
                               12 Shall be set to one to indicate that the mandatory FLUSH CACHE command is supported"    1
                               11 1 = Device Configuration Overlay feature set supported    1
                               10 1 = 48-bit Address feature set supported    1
                               9 1 = Automatic Acoustic Management feature set supported    0
                               8 1 = SET MAX security extension supported    1
                               7 Reserved for the Address Offset Reserved Area Boot Method    0
                               6 1 = SET FEATURES subcommand required to spin-up after power-up    0
                               5 1 = Power-Up In Standby features set supported    0
                               4 Obsolete    0
                               3 1 = Advanced Power Management feature set supported    1
                               2 1 = CFA feature set supported    0
                               1 1 = TCQ feature set supported    0
                               0 1 = DOWNLOAD MICROCODE command supported    1*/
        0x4000,/*Word 84:Command set/feature supported extension.
                               15 Shall be cleared to zero    0
                               14 Shall be set to one    1
                               13 1 = IDLE IMMEDIATE with UNLOAD FEATURE supported    0
                               12 Reserved for technical report INCITS TR-37-2004 (TLC)    0
                               11 Reserved for technical report INCITS TR-37-2004 (TLC)    0
                               10-9 Obsolete    00
                               8 Shall be set to one (i.e. 64-bit Word wide name supported)    1
                               7 1 = WRITE DMA QUEUED FUA EXT command supported    0
                               6 1 = WRITE DMA FUA EXT and WRITE MULTI FUA EXT commands supported    1
                               5 1 = General Purpose Logging feature set supported    1
                               4 1 = Streaming feature set supported    0
                               3 1 = Media Card Pass Through Command feature set supported    0
                               2 1 = Media serial number supported    0
                               1 1 = SMART self-test supported    1
                               0 1 = SMART error logging supported    1*/
        0x7008,/*Word 85:Command set/feature enabled.
                            15 Obsolete    0
                            14 1 = NOP command supported    1
                            13 1 = READ BUFFER command supported    1
                            12 1 = WRITE BUFFER command supported    1
                            11 Obsolete    0
                            10 1 = Host Protected Area feature set enabled    1
                             9 Shall be cleared to zero to indicate that the DEVICE RESET command is not supported     0
                             8 1 = SERVICE interrupt enabled    0
                             7 1 = release interrupt enabled    0
                             6 1 = Read look-ahead enabled    0
                             5 1 = write cache enabled    1
                             4 Shall be cleared to zero to indicate that the PACKET feature set is not supported.    0
                             3 Shall be set to one to indicate Power Management feature set is supported    1
                             2 Obsolete    0
                             1 1 = Security Mode feature set enabled    0
                             0 1 = SMART feature set enabled    1*/
        0x0000,/*Word 86:Command set/feature enabled.
                            15 1 = Words 120-119 are valid    0
                            14 Reserved    1
                            13 1 = FLUSH CACHE EXT command supported    1
                            12 1 = FLUSH CACHE command supported    1
                            11 1 = Device Configuration Overlay supported    1
                            10 1 = 48-bit Address features set supported    1
                             9 1 = Automatic Acoustic Management feature set enabled    0
                             8 1 = SET MAX security enabled by SET MAX SET PASSWord    1
                             7 Reserved for address Offset Reserved Area Boot,INCITS TR27:2001    0
                             6 1 = SET FEATURES subcommand required to spin-up after power-up    0
                             5 1 = Power-Up In Standby features set enabled    0
                             4 Obsolete    0
                             3 1 = Advanced Power Management feature set enabled    0
                             2 1 = CFA feature set supported    0
                             1 1 = TCQ feature set supported    0
                             0 1 = DOWNLOAD MICROCODE command supported    1*/
        0x0000,/*Word 87:Command set/feature enabled/supported.
                            15 Shall be cleared to zero    0
                            14 Shall be set to one    1
                            13 1 = IDLE IMMEDIATE with UNLOAD FEATURE supported    0
                            12 Reserved for technical report- INCITS tr-37-2004 (TLC)    0
                            11 Reserved for technical report- INCITS TR-37-2004 (TLC)    0
                          10-9 Obsolete    00
                             8 Shall be set to one (i.e. 64 bit Word wide name supported)    1
                             7 1 = WRITE DMA QUEUED FUA EXT command supported    0
                             6 1 = WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands supported    1
                             5 1 = General Purpose Logging feature set supported    1
                             4 Obsolete    0
                             3 1 = Media Card Pass Through Command feature set supported    0
                             2 1 = Media serial number is valid    0
                             1 1 = SMART self-test supported    1
                             0 1 = SMART error logging supported    1*/
        0x203F,/*Word 88:Ultra DMA modes
                             15 Reserved    0
                             14 1 = Ultra DMA mode 6 is selected    1
                                0 = Ultra DMA mode 6 is not selected     
                             13 1 = Ultra DMA mode 5 is selected    1
                                0 = Ultra DMA mode 5 is not selected     
                             12 1 = Ultra DMA mode 4 is selected    0
                                0 = Ultra DMA mode 4 is not selected     
                             11 1 = Ultra DMA mode 3 is selected    0
                                0 = Ultra DMA mode 3 is not selected     
                             10 1 = Ultra DMA mode 2 is selected    0
                                0 = Ultra DMA mode 2 is not selected     
                              9 1 = Ultra DMA mode 1 is selected    0
                                0 = Ultra DMA mode 1 is not selected     
                              8 1 = Ultra DMA mode 0 is selected    0
                                0 = Ultra DMA mode 0 is not selected     
                              7 Reserved    0
                              6 1 = Ultra DMA mode 6 and below are supported    1
                              5 1 = Ultra DMA mode 5 and below are supported    1
                              4 1 = Ultra DMA mode 4 and below are supported    1
                              3 1 = Ultra DMA mode 3 and below are supported    1
                              2 1 = Ultra DMA mode 2 and below are supported    1
                              1 1 = Ultra DMA mode 1 and below are supported    1
                              0 1 = Ultra DMA mode 0 is supported    1*/
        0x0001,/*Word 89:
                              15 1 = Extended Time is reported in bits 14:0    0
                                 0 = Time is reported in bits 7:0 and bits 14:8 are reserved    
                                 14-8 Extended Time required for Normal Erase mode SECURITY ERASE UNIT command     0000000
                                 7-0 Extended Time required for Normal Erase mode SECURITY ERASE UNIT command and Time 
                                     required for Normal Erase mode SECURITY ERASE UNIT command    00000001*/
        0x0001,/*Word 90:
                              15 1 = Extended Time is reported in bits 14:0    0
                                 0 = Time is reported in bits 7:0 and bits 14:8 are reserved    
                              14-8 Extended Time required for Enhanced Erase mode SECURITY ERASE UNIT command    0000000
                              7-0 Extended Time required for Enhanced Erase mode SECURITY ERASE UNIT command and Time 
                                  required for Enhanced Erase mode SECURITY ERASE UNIT command       00000001*/
        0x0000,/*Word 91: Current advanced power management level value
                              15-8 Reserved       00000000                                                 
                              7-0 Current APM level value    00000000
                                         00h = Reserved
                                         01h = Minimum power consumption with Standby
                                         02h-7Fh = Intermediate power management levels with Standby
                                         80h = Minimum power consumption without Standby
                                         81h-FDh = Intermediate power management levels without Standby
                                         FEh = Maximum performance
                                         FFh = Reserved*/
        0xFFFE,/*Word 92:   Master PassWord Identifier*/
        0x0000,/*Word 93:  Hardware reset result. For SATA devices, Word 93 shall be set to the value 0000h.*/
        0x0000,/*Word 94:                                     
                              15-8 Vendor's recommended acoustic management value.
                              7-0 Current automatic acoustic management value.*/
        0x0000,/*Word 95: Stream Minimum Request Size*/
        0x0000,/*Word 96: Streaming Transfer Time - DMA*/
        0x0000,/*Word 97: Streaming Access Latency - DMA and PIO*/
        0x0000, 0x0000,/*Word 98-99:Streaming Performance Granularity(98-99)*/
        CAPACITY_48BIT,/**Word 100-103 Total Number of user LBA for 48-bit Address feature set.*/
        0x0000,/*Word 104:Streaming Transfer Time - PIO*/
        0x0008,/*Word 105:Maximum number of 512-U8 blocks per DATA SET MANAGEMENT command*/    
        0x4000,/*Word 106:Physical Sct size / Logical Sct Size 
                          15 Shall be cleared to zero 0
                          14 Shall be set to one   1
                          13 1 = Device has multiple logical Scts per physical Sct. 0
                          12 1 = Device Logical Sct Longer than 256 Words 0
                          11-4 Reserved 00000000
                          3-0 2^x logical Scts per physical Sct 0000*/ 
        0x0000,/*Word 107:Inter-seek delay for ISO-7779 acoustic testing in microseconds*/
        0x0000,/*Word 108:            15-12 NAA (3-0) 0000
                                      11-0 IEEE OUI (23-12) 000000000000*/
        0x0000,/*Word 109:            15-4 IEEE OUI (11-0) 000000000000
                                      3-0 Unique ID (35-32) 0000*/
        0x0000,/*Word 110:15-0 Unique ID (31-16)*/
        0x0000,/*Word 111:15-0 Unique ID (15-0)*/
        0x0000,0x0000,0x0000,0x0000,/*Word 112-115: Reserved for 128 bit world wide name extension to 128 bits*/
        0x0000,/*Word 116:Reserved for INCITS TR-37-2004*/
        0x0000,0x0000,/*Word 117-118:Words per Logical Sct*/
        0x4018,/*Word 119:Commands and feature sets supported (Continued from Words 84-82)
                        15 Shall be cleared to zero    0
                        14 Shall be set to one    1
                        13-10 Reserved    0000
                         9 1 = DSN feature set is supported    0
                         8 1 = Accessible Max Address Configuration feature set is supported    0
                         7 1 = Extended Power Conditions feature set is supported    0
                         6 1 = Sense Data Reporting feature set is supported    0
                         5 1 = The Free-fall Control feature set is supported    0
                         4 1 = The DOWNLOAD MICROCODE command with mode 3 is supported    1
                         3 1 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported    1
                         2 1 = The WRITE UNCORRECTABLE EXT command is supported    0
                         1 1 = The Write-Read-Verify feature set is supported    0
                         0 Reserved for Disable Data Transfer    0*/ 
        0x4000,/*Word 120:Commands and feature sets supported or enabled (Continued from Words 87-85)
                                          15 Shall be cleared to zero    0
                        14 Shall be set to one    1
                        13-10 Reserved    0000
                         9 1 = DSN feature set is enabled    0
                         8 Reserved    0
                         7 1 = Extended Power Conditions feature set is enabled    0
                         6 1 = Sense Data Reporting feature set is enabled    0
                         5 1 = The Free-fall Control feature set is enabled    0
                         4 1 = The DOWNLOAD MICROCODE command with mode 3 is supported    1
                         3 1 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported    1
                         2 1 = The WRITE UNCORRECTABLE EXT command is supported    0
                         1 1 = The Write-Read-Verify feature set is enabled    0
                         0 Reserved for Disable Data Transfer    0*/
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,/*Word 121-126: Reserved for expanded supported and enable settings*/
        0x0000,/*Word 127:Obsolete*/
        0x0000,/*Word 128:Security status
                         15-9 Reserved    0000000
                         8 Master PassWord Capability 0 = High, 1 = Maximum    0
                         7-6 Reserved    00
                         5 1 = Enhanced security erase supported   1
                         4 1 = Security count expired    0
                         3 1 = Security frozen    1
                         2 1 = Security locked    0
                         1 1 = Security enabled    0
                         0 1 = Security supported    1*/
        VENDOR_SPEC,/*Word 129-159:Vendor specific*/
        0x0000,/*Word 160: CFA power mode 1
                            15 Word 160 supported 0
                            14 Reserved 0
                            13 CFA power mode 1 is required for one or more commands implemented by the device 0
                            12 CFA power mode 1 disabled 0
                            11-0 Maximum current in MA 000000000000*/
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,/*Word 161-167:Reserved for assignment by the CompactFlashTM Association*/
        0x0003,/*Word 168:Device Nominal Form Factor
                                  15-4 Reserved
                                  3-0 Device Nominal Form Factor 产品尺寸03：2.5英寸*/
        0x0000,/*Word 169:DATA SET MANAGEMENT command is supported
                    15-1 Reserved
                    0 1 = the Trim bit in the DATA SET MANAGEMENT command is supported 1*/
        0x0000, 0x0000, 0x0000, 0x0000,/*Word 170-173:Additional Product Identifier(ATA String)*/
        0x0000, 0x0000,/*Word 174-175:  Reserved*/
        CMSM,/*Word 176:Current media serial number (60 ASCII characters)*/
        0x0000,/*Word 206:SCT Command Transport
                     15-12 Vender Specific    0000
                     11-6 Reserved    000000
                        5 SCT Command Transport Data Tables support    1
                        4 SCT Command Transport Features Control supported                        1
                        3 SCT Command Transport Error Recovery control supported    1
                        2 SCT Command Transport Write Same supported    0
                        1 SCT Command Transport Long Sct Access supported    0
                        0 SCT Command Transport supported    1*/
        0x0000, 0x0000,/*Word 207-208:  Reserved*/
        0x4000,/*Word 209:Alignment of logical blocks within a larger physical block                              
                     15 Shall be cleared to zero 0
                     14 Shall be set to one 1
                     13-0 'Logical Sct' offset within the first physical Sct where the first logical Sct is placed. */
        0x0000,0x0000,/*Word 210-211:Write-Read-Verify Sct Count Mode 3 Only*/                                 
        0x0000,0x0000,/*Word 212-213:Write-Read-Verify Sct Count Mode 2 Only*/
        0x0000,/*Word 214: NV Cache Capabilities
                           15-12 NV Cache feature set version    0000
                           11-8 NV Cache Power Mode feature set version    0000
                           7-5 Reserved    000
                           4 1 = NV Cache feature set enabled    0
                           3-2 Reserved    00
                           1 1 = NV Cache Power mode feature set enabled    0
                           0 1 = NV Cache Power Mode feature set supported    0*/
        0x0000,/*Word 215: NV Cache Size in Logical Blocks (LSW)*/
        0x0000,/*Word 216: NV Cache Size in Logical Blocks (MSW)*/
        0x0001,/*Word 217:Nominal media rotation rate (ATA8-ACS 1699-D Revision 6)
                             NV Cache Read Transfer Speed in MB/s (ATA8-ACS 1699-D Revision 3f)*/
        0x0000,/*Word 218:NV Cache Write Transfer Speed in MB/s*/
        0x0000,/*Word 219:NV Cache Options
                          15-8 Reserved
                          7-0 Device Estimated Time to Spin Up in Seconds*/
        0x0000,/*Word 220:       
                          15-8 Reserved
                          7-0 Write-Read-Verify feature set current mode*/
        0x0000,/*Word 221:  Reserved*/
        0x101F,/*Word 222: Transport Major revision number. 0000h or FFFFh = device does not report version
                          15-12 Transport Type - 0 = Parallel, 1 = Serial, 2-15 = Reserved 0001
                                Parallel (Type = 0)                Serial (Type = 1)    
                          11-8 Reserved                           Reserved    0000
                             7 Reserved                           SATA Rev 3.2    0
                             6 Reserved                           SATA Rev 3.1    1
                             5 Reserved                           SATA Rev 3.0     1
                             4 Reserved                           SATA Rev 2.6    1
                             3 Reserved                           SATA Rev 2.5    1
                             2 Reserved                           SATA II: Extensions    1
                             1 ATA/ATAPI-7                        SATA 1.0a    1
                             0 ATA8-APT                           ATA8-AST    1*/
        0x0000,/*Word 223:Transport Minor revision number not report version*/
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,/*Word 224-229:  Reserved for CE-ATA*/
        0x0000, 0x0000, 0x0000, 0x0000,/*Word 230:  Extended Number of User Addressable Scts(QWord)*/
        0x0010,/*Word 234: Minimum number of 512 U8 units per DOWNLOAD MICROCODE command mode 3*/
        0x0010,/*Word 235:  Maximum number of 512 U8 units per DOWNLOAD MICROCODE command mode 3*/

        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /*Word 236-254:  Reserved*/
        0x00A5},/*Word 255:Integrity Word
                             15-8 Checksum
                              7-0 Checksum Validity Indicator*/
};

#else

#if(SATA_LOG_ENABLE == 1)
const UN_ATA_INDETIFY_T unAtaIdentify_C __section(".IDENTIFY_C")={
#else
#ifdef __ALG_FORMAL
const UN_ATA_INDETIFY_T unAtaIdentify_C __section(".IDENTIFY_C")={
#else
const UN_ATA_INDETIFY_T unAtaIdentify_C = {
#endif
#endif
        {0x0040,/*Word 00:General configuration bit-significant information:
                                  15 0 = ATA device                0
                                  14-8 Retired                    0000100
                                  7 1 = removable media device    0
                                  6 Obsolete                    1
                                  5-3 Retired                    001
                                  2 Response incomplete            0
                                  1 Retired                        1
                                  0 Reserved                    0*/
        0x3FFF,/*Word 01:Number of logical cylinders (ATA5) -柱面数，大于7.8G 容量时该字段等于3FFF
                 Word 1 shall contain the number of user-addressable logical cylinders in the default CHS translation. If
                the content of words (61:60) is less than 16,514,064 then the content of word 1 shall be greater than or
                equal to one and less than or equal to 65,535. If the content of words (61:60) is greater than or equal to
                16,514,064 then the content of word 1 shall be equal to 16,383.
                 */
        0xC837,/*Word 02:Specific configurationDevice does not require SET FEATURES subcommand to spin-up after
                         power-up and IDENTIFY DEVICE data is complete*/
        0x0010,/*Word 03:Number of logical heads-磁头数*/
        0x0000,0x0000,/*Word 04-05: Retired*/
        0x003F,/*Word 06:Number of logical Scts per logical track-扇区数*/
        0x0000,0x0000,/*Word 07-08: Reserved for assignment by the CompactFlashTM Association*/
        0x0000,/*Word 09:Retired*/
        SERIAL,/*Word 10-19:Serial number (20 ASCII characters)*/
        0x0000, 0x0000, 0x0000,/*Word 20-22:Retired/Obsolete*/
        FW_REV,/*Word 23-26:Firmware revision (8 ASCII characters)*/
        MODEL, /*Word 27-46:Model number (40 ASCII characters)*/
        0x8008,/*47:Maximum number of Scts on READ/WRITE MULTIPLE
                                    15-8  80h                    80
                                    7-0   00h = Reserved         10
                                          01h-10h = Maximum number of Scts that shall be transferred  
                                                    per interrupt on READ/WRITE MULTIPLE commands
                                          11h-FFh = Reserved*/
        0x4000,/*Word 48:Trusted Computing feature set options
                                 15     Shall be cleared to zero                        0
                                 14     Shall be set to one                             1
                                 13-1   Reserved for the Trusted Computing Group        0000000000000            
                                 0      1 = Trusted Computing feature set is supported  0 = NOT SUPPORT*/
        0x2F00,/*Word 49:Capabilities    
                                 15-14  Reserved for the IDENTIFY PACKET DEVICE command.                        00
                                 13     1 = Standby timer values as specified in this standard are supported    1
                                        0 = Standby timer values shall be managed by the device    
                                 12     Reserved for the IDENTIFY PACKET DEVICE command                            0
                                 11     1 = IORDY supported                                                        1
                                        0 = IORDY may be supported    
                                 10     1 = IORDY may be disabled                                                1
                                 9      1 = Shall be set to one (i.e. LBA supported)                            1
                                 8      1 = DMA supported                                                        1
                                 7-2    Reserved                                                                000000
                                 1-0    Long Physical Sct Alignment Error reporting                            00*/
        0x4000,/*Word 50:Capabilities    
                                 15        Shall be cleared to zero.                                                0
                                 14        Shall be set to one.                                                    1
                                 13-2   Reserved.                                                                000000000000                                                                
                                 1      Obsolete                                                                0
                                 0      1 = There is a minimum Standby time value and it is vendor specific.    0*/
        0x0000, 0x0000,/*Word 51-52: Obsolete*/
        0x0007,/*Word 53:          
                                 15-8   Free-fall Control Sensitivity                                            00000000
                                        00h=Vendor's recommended's setting    
                                        01h-ffh=Sensitivity level. A larger number is a more sensitive setting.    00
                                 7-3    Reserved                                                                00000
                                 2      1 = the fields reported in Word 88 are valid                            1
                                        0 = the fields reported in Word 88 are not valid    
                                 1      1 = the fields reported in Words (70:64) are valid                        1
                                        0 = the fields reported in Words (70:64) are not valid    
                                 1      Obsolete                                                                1*/
        0x3FFF,/*Word 54当前柱面数*/
        0x0010,/*Word 55当前磁头数*/
        0x003F,/*Word 56当前扇区数*/ 
        0xFC10,/*Word 57当前逻辑地址高位*/
        0x00FB,/*Word 58当前逻辑地址低位*/
               /*前逻辑地址=当前柱面数*当前磁头数*当前扇区数*/
        0x9101,/*Word 59:
                                 15        1 = BLOCK ERASE EXT command is supported                                               1
                                 14        1 = OVERWRITE EXT command is supported                                                   0
                                 13        1 = CRYPTO SCRAMBLE EXT command is supported                                           0
                                 12        1 = Sanitize feature set is supported                                                   1
                                 11        1 = The commands allowed during a sanitize operation are as specified by this standard
                                        0 = The commands allowed during a sanitize operation are as specified by ACS-2"           0
                                 10        1 = The SANITIZE ANTIFREEZE LOCK EXT command is supported                               0
                                 9        Reserved                                                                               0
                                 8        1 = Multiple logical Sct setting is valid                                           1
                                 7-0    xxh = Current setting for number of logical Scts that shall be transferred 
                                        per DRQ data block on READ/WRITE Multiple commands                                       00000000*/
        CAPACITY_28BIT,/*Word 60-61:Total number of user addressable logical Scts for 28-bit commands*/
        0x0000,/*Word 62:Obsolete*/
        0x0007,/*Word 63:        15-11  Reserved    00000
                                 10     1 = MultiWord DMA mode 2 is selected                        0
                                        0 = MultiWord DMA mode 2 is not selected    
                                 9      1 = MultiWord DMA mode 1 is selected                        0
                                        0 = MultiWord DMA mode 1 is not selected    
                                 8      1 = MultiWord DMA mode 0 is selected                        0
                                        0 = MultiWord DMA mode 0 is not selected    
                                 7-3    Reserved                                                    00000
                                 2      1 = MultiWord DMA mode 2 and below are supported            1
                                 1      1 = MultiWord DMA mode 1 and below are supported            1
                                 0      1 = MultiWord DMA mode 0 is supported                         1*/
        0x0003,/*Word 64:        15-2   Reserved
                                 1-0    PIO modes 3 and 4 supported*/
        0x0078,/*Word 65:0078h (M) Minimum MultiWord DMA transfer cycle time per Word*/
        0x0078,/*Word 66:0078h (M) Minimum MultiWord DMA transfer cycle time per Word*/    
        0x0078,/*Word 67:0078h (M) Minimum MultiWord DMA transfer cycle time per Word*/
        0x0078,/*Word 68:0078h (M) Minimum MultiWord DMA transfer cycle time per Word
                                 15-0   Cycle time in nanoseconds*/
        0x0C00,/*Word 69: Additional Supported
                                 15     1 = CFast Specification Support                                                        0
                                 14     1 = Deterministic data in trimmed LBA range(s) is supported                               0
                                 13     1 = Long Physical Sct Alignment Error Reporting Control is supported                   0
                                 12     1 = DEVICE CONFIGURATION IDENTIFY DMA and DEVICE CONFIGURATION SET DMA are supported   0
                                 11     1 = READ BUFFER DMA is supported                                                       0
                                 10     1 = WRITE BUFFER DMA is supported                                                       0
                                 9      1 = SET MAX SET PASSWord DMA and SET MAX UNLOCK DMA are supported                       0
                                 8      1 = DOWNLOAD MICROCODE DMA is supported                                                1
                                 7      Reserved for IEEE 1667                                                                   0
                                 6      0 = Optional ATA device 28-bit commands supported                                      0
                                 5      1 = Trimmed LBA range(s) returning zeroed data is supported                            0
                                 4      1 = Device Encrypts All User data                                                       0
                                 3      1 = Extended Number of User Addressable Scts is supported                                0
                                 2      1 = All write cache is non-volatile                                                    0
                                 1-0    Reserved                                                                               00*/
        0x0000,/*Word 70:Reserved*/
        0x0000, 0x0000, 0x0000, 0x0000,/*Word 71-74:Reserved for IDENTIFY PACKET DEVICE command.*/
        0x001F,/*Word 75:               
                                15-5    Reserved                          0    
                                4-0     Maximum queue depth - 1           32*/
#ifdef __RDT_SATA_CTRL//rdt上盘需要上报不支持NCQ
		0xE40E,
#else
		0xE50E,
#endif

		/*Word 76:
                                15     Supports READ LOG DMA EXT as equivalent to READ LOG EXT                    1
                                14     Supports Device Automatic Partial to Slumber transitions                    0
                                13     Supports Host Automatic Partial to Slumber transitions                    0
                                12     Supports Native Command Queuing priority                                  0
                                11     Supports Unload while NCQ commands outstanding                            0
                                10     Supports Phy event counters                                                1
                                9      Supports receipt of host-initiated interface power management requests    1
                                8      Supports native command queuing                                            1
                                7-4    Reserved for future Serial ATA signaling speed grades                    0000
                                3      1 = Supports Serial ATA Gen-3 signaling speed (6.0Gbps)                    1
                                2      1 = Supports Serial ATA Gen-2 signaling speed (3.0Gbps)                    1
                                1      1 = Supports Serial ATA Gen-1 signaling speed (1.5Gbps)                    1
                                0      Shall be cleared to zero                                                    0*/
        0x0006,/*Word 77:Serial ATA Additional capabilities
                              15-8 Reserved
                              7 Supports DevSleep to reduced power state capability    1
                                If the DEVSLEEP_TO_REDUCEDPWRSTATE CAPABILITY SUPPORTED bit is set to one, then:
                                        a) the device supports maintaining whether it was in Partial or Slumber after detection of
                                        assertion, and subsequent detection of negation, of DEVSLP; and
                                        b) the DEVICE SLEEP SUPPORTED bit shall be set to one.
                                        If the DEVSLEEP_TO_REDUCEDPWRSTATE CAPABILITY SUPPORTED bit is cleared to zero, then the
                                        device does not support remembering whether it was in Partial or Slumber after detection of
                                        assertion, and subsequent detection of negation, of DEVSLP.
                              6 Supports RECEIVE FPDMA QUEUED and SEND FPDMA QUEUED commands 
                              5 Supports NCQ Queue Management Command
                              4 Supports NCQ Streaming
                              3-1 Coded value indicating current negotiated Serial ATA signal speed
                              0 Shall be cleared to zero*/
        0x014C,/*Word 78:
                                15-12  Reserved for Serial ATA                                        0000
                                11     1 = Supports Rebuild Assist                                    0
                                10     1 = Supports Device Initiated Interface Power Management 
                                           Software Settings Preservation                            0
                                9      1 = Supports Hybrid Information                                0
                                8      1 = Device Sleep supported                                    0
                                7      1 = Supports NCQ Autosense                                    1
                                6      1 = Supports software settings preservation                    1
                                5      1 = Hardware Feature Control is supported                    0
                                4      1 = Supports in-order data delivery                            0
                                3      1 = Supports dev initiate interface power management            1
                                2      1 = Supports DMA Setup Auto-Activate optimization            1
                                1      1 = Supports non-zero buffer offsets in DMA Setup FIS        0
                                0      Shall be cleared to zero                                     0*/
        0x0040,/*Word 79:Serial ATA features enabled
                                15-12  Reserved                                                     0000
                                11     1 = Rebuild Assist enabled                                    0
                                10     Reserved                                                        0
                                9      1 = Hybrid Information feature is enabled                    0
                                8      1 = Device Sleep enabled                                        0
                                7      1 = Device Automatic Partial to Slumber transitions enabled    0
                                6      1 = software settings preservation enabled                    1
                                5      1 = Hardware Feature Control is enabled                        0
                                4      1 = In-order data delivery enabled                            0
                                3      1 = Device initiating interface power management enabled        0
                                2      1 = DMA Setup Auto-Activate optimization enabled                0
                                1      1 = Non-zero buffer offsets in DMA Setup FIS enabled            0
                                0      Shall be cleared to zero                                     0*/
        0x01FC,/*Word 80:Major version number
                                0000h or FFFFh = device does not report version    
                                15-11  Reserved                     00000
                                10     1 = Supports ACS-3         0
                                9      1 = Supports ACS-2         1
                                8      1 = Supports ATA8-ACS     1
                                7      1 = Supports ATA/ATAPI-7     1
                                6      1 = Supports ATA/ATAPI-6     1
                                5      1 = Supports ATA/ATAPI-5     1
                                4      1 = Supports ATA/ATAPI-4     1
                                3      Obsolete                     1
                                2      Obsolete                     1
                                1      Obsolete                     0
                                0      Reserved                     0*/
        0x0029,/*Word 81:Minor revision number
                                0000h or FFFFh = device does not report version*/
        0x706B,/*Word 82:Commands and feature set supported
                                15 Obsolete    0
                                14 1 = NOP command supported   0
                                13 1 = READ BUFFER command supported    1
                                12 1 = WRITE BUFFER command supported    1
                                11 Obsolete    0
                                10 1 = Host Protected Area feature set supported    1
                                9 1 = DEVICE RESET command supported    0
                                8 1 = SERVICE interrupt supported    0
                                7 1 = release interrupt supported    0
                                6 1 = look-ahead supported    1
                                5 1 = write cache supported    1
                                4 Shall be cleared to zero to indicate that the PACKET feature set is not supported    0
                                3 Shall be set to one to indicate that the mandatory Power Management feature set is supported    1
                                2 Obsolete    0
                                1 1 = Security Mode feature set supported    1
                                1 1 = SMART feature set supported    1*/
        0x7400,/*Word 83:Commands and features sets supported
                               15 Shall be cleared to zero    0
                               14 Shall be set to one    1
                               13 1 = FLUSH CACHE EXT command supported    1
                               12 Shall be set to one to indicate that the mandatory FLUSH CACHE command is supported"    1
                               11 1 = Device Configuration Overlay feature set supported    1
                               10 1 = 48-bit Address feature set supported    1
                               9 1 = Automatic Acoustic Management feature set supported    0
                               8 1 = SET MAX security extension supported    1
                               7 Reserved for the Address Offset Reserved Area Boot Method    0
                               6 1 = SET FEATURES subcommand required to spin-up after power-up    0
                               5 1 = Power-Up In Standby features set supported    0
                               4 Obsolete    0
                               3 1 = Advanced Power Management feature set supported    1
                               2 1 = CFA feature set supported    0
                               1 0 = TCQ feature set supported    0
                               0 0 = DOWNLOAD MICROCODE command supported    1*/
        0x4163,/*Word 84:Command set/feature supported extension.
                               15 Shall be cleared to zero    0
                               14 Shall be set to one    1
                               13 1 = IDLE IMMEDIATE with UNLOAD FEATURE supported    0
                               12 Reserved for technical report INCITS TR-37-2004 (TLC)    0
                               11 Reserved for technical report INCITS TR-37-2004 (TLC)    0
                               10-9 Obsolete    00
                               8 Shall be set to one (i.e. 64-bit Word wide name supported)    1
                               7 1 = WRITE DMA QUEUED FUA EXT command supported    0
                               6 1 = WRITE DMA FUA EXT and WRITE MULTI FUA EXT commands supported    1
                               5 1 = General Purpose Logging feature set supported    1
                               4 1 = Streaming feature set supported    0
                               3 1 = Media Card Pass Through Command feature set supported    0
                               2 1 = Media serial number supported    0
                               1 1 = SMART self-test supported    1
                               0 1 = SMART error logging supported    1*/
        0x746B,/*Word 85:Command set/feature enabled.
                            15 Obsolete    0
                            14 1 = NOP command supported    1
                            13 1 = READ BUFFER command supported    1
                            12 1 = WRITE BUFFER command supported    1
                            11 Obsolete    0
                            10 1 = Host Protected Area feature set enabled    1
                             9 Shall be cleared to zero to indicate that the DEVICE RESET command is not supported     0
                             8 1 = SERVICE interrupt enabled    0
                             7 1 = release interrupt enabled    0
                             6 1 = Read look-ahead enabled    0
                             5 1 = write cache enabled    1
                             4 Shall be cleared to zero to indicate that the PACKET feature set is not supported.    0
                             3 Shall be set to one to indicate Power Management feature set is supported    1
                             2 Obsolete    0
                             1 1 = Security Mode feature set enabled    0
                             0 1 = SMART feature set enabled    1*/
        0xB400,/*Word 86:Command set/feature enabled.
                            15 1 = Words 120-119 are valid    0
                            14 Reserved    1
                            13 1 = FLUSH CACHE EXT command supported    1
                            12 1 = FLUSH CACHE command supported    1
                            11 1 = Device Configuration Overlay supported    1
                            10 1 = 48-bit Address features set supported    1
                             9 1 = Automatic Acoustic Management feature set enabled    0
                             8 1 = SET MAX security enabled by SET MAX SET PASSWord    1
                             7 Reserved for address Offset Reserved Area Boot,INCITS TR27:2001    0
                             6 1 = SET FEATURES subcommand required to spin-up after power-up    0
                             5 1 = Power-Up In Standby features set enabled    0
                             4 Obsolete    0
                             3 1 = Advanced Power Management feature set enabled    1
                             2 1 = CFA feature set supported    0
                             1 1 = TCQ feature set supported    0
                             0 1 = DOWNLOAD MICROCODE command supported    1*/
        0x4163,/*Word 87:Command set/feature enabled/supported.                           
                            15 Shall be cleared to zero    0
                            14 Shall be set to one    1
                            13 1 = IDLE IMMEDIATE with UNLOAD FEATURE supported    0
                            12 Reserved for technical report- INCITS tr-37-2004 (TLC)    0
                            11 Reserved for technical report- INCITS TR-37-2004 (TLC)    0
                          10-9 Obsolete    00
                             8 Shall be set to one (i.e. 64 bit Word wide name supported)    1
                             7 1 = WRITE DMA QUEUED FUA EXT command supported    0
                             6 1 = WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands supported    1
                             5 1 = General Purpose Logging feature set supported    1
                             4 Obsolete    0
                             3 1 = Media Card Pass Through Command feature set supported    0
                             2 1 = Media serial number is valid    0
                             1 1 = SMART self-test supported    1
                             0 1 = SMART error logging supported    1*/
        0x207F,/*Word 88:Ultra DMA modes
                             15 Reserved    0
                             14 1 = Ultra DMA mode 6 is selected    1
                                0 = Ultra DMA mode 6 is not selected     
                             13 1 = Ultra DMA mode 5 is selected    1
                                0 = Ultra DMA mode 5 is not selected     
                             12 1 = Ultra DMA mode 4 is selected    0
                                0 = Ultra DMA mode 4 is not selected     
                             11 1 = Ultra DMA mode 3 is selected    0
                                0 = Ultra DMA mode 3 is not selected     
                             10 1 = Ultra DMA mode 2 is selected    0
                                0 = Ultra DMA mode 2 is not selected     
                              9 1 = Ultra DMA mode 1 is selected    0
                                0 = Ultra DMA mode 1 is not selected     
                              8 1 = Ultra DMA mode 0 is selected    0
                                0 = Ultra DMA mode 0 is not selected     
                              7 Reserved    0
                              6 1 = Ultra DMA mode 6 and below are supported    1
                              5 1 = Ultra DMA mode 5 and below are supported    1
                              4 1 = Ultra DMA mode 4 and below are supported    1
                              3 1 = Ultra DMA mode 3 and below are supported    1
                              2 1 = Ultra DMA mode 2 and below are supported    1
                              1 1 = Ultra DMA mode 1 and below are supported    1
                              0 1 = Ultra DMA mode 0 is supported    1*/
        0x0001,/*Word 89:
                              15 1 = Extended Time is reported in bits 14:0    0
                                 0 = Time is reported in bits 7:0 and bits 14:8 are reserved    
                                 14-8 Extended Time required for Normal Erase mode SECURITY ERASE UNIT command     0000000
                                 7-0 Extended Time required for Normal Erase mode SECURITY ERASE UNIT command and Time 
                                     required for Normal Erase mode SECURITY ERASE UNIT command    00000001*/
        0x0001,/*Word 90:
                              15 1 = Extended Time is reported in bits 14:0    0
                                 0 = Time is reported in bits 7:0 and bits 14:8 are reserved    
                              14-8 Extended Time required for Enhanced Erase mode SECURITY ERASE UNIT command    0000000
                              7-0 Extended Time required for Enhanced Erase mode SECURITY ERASE UNIT command and Time 
                                  required for Enhanced Erase mode SECURITY ERASE UNIT command       00000001*/
        0x0000,/*Word 91: Current advanced power management level value
                              15-8 Reserved       00000000                                                 
                              7-0 Current APM level value    00000000
                                         00h = Reserved
                                         01h = Minimum power consumption with Standby
                                         02h-7Fh = Intermediate power management levels with Standby
                                         80h = Minimum power consumption without Standby
                                         81h-FDh = Intermediate power management levels without Standby
                                         FEh = Maximum performance
                                         FFh = Reserved*/
        0xFFFE,/*Word 92:   Master PassWord Identifier*/
        0x0000,/*Word 93:  Hardware reset result. For SATA devices, Word 93 shall be set to the value 0000h.*/
        0x0000,/*Word 94:                                     
                              15-8 Vendor's recommended acoustic management value.
                              7-0 Current automatic acoustic management value.*/
        0x0000,/*Word 95: Stream Minimum Request Size*/
        0x0000,/*Word 96: Streaming Transfer Time - DMA*/
        0x0000,/*Word 97: Streaming Access Latency - DMA and PIO*/
        0x0000, 0x0000,/*Word 98-99:Streaming Performance Granularity(98-99)*/
        CAPACITY_48BIT,/**Word 100-103 Total Number of user LBA for 48-bit Address feature set.*/
        0x0000,/*Word 104:Streaming Transfer Time - PIO*/
        0x0008,/*Word 105:Maximum number of 512-U8 blocks per DATA SET MANAGEMENT command*/    
        0x4000,/*Word 106:Physical Sct size / Logical Sct Size 
                          15 Shall be cleared to zero 0
                          14 Shall be set to one   1
                          13 1 = Device has multiple logical Scts per physical Sct. 0
                          12 1 = Device Logical Sct Longer than 256 Words 0
                          11-4 Reserved 00000000
                          3-0 2^x logical Scts per physical Sct 0000*/ 
        0x0000,/*Word 107:Inter-seek delay for ISO-7779 acoustic testing in microseconds*/
        0x0000,/*Word 108:            15-12 NAA (3-0) 0000
                                      11-0 IEEE OUI (23-12) 000000000000*/
        0x0000,/*Word 109:            15-4 IEEE OUI (11-0) 000000000000
                                      3-0 Unique ID (35-32) 0000*/
        0x0000,/*Word 110:15-0 Unique ID (31-16)*/
        0x0000,/*Word 111:15-0 Unique ID (15-0)*/
        0x0000,0x0000,0x0000,0x0000,/*Word 112-115: Reserved for 128 bit world wide name extension to 128 bits*/
        0x0000,/*Word 116:Reserved for INCITS TR-37-2004*/
        0x0000,0x0000,/*Word 117-118:Words per Logical Sct*/
        0x4008,/*Word 119:Commands and feature sets supported (Continued from Words 84-82)
                        15 Shall be cleared to zero    0
                        14 Shall be set to one    1
                        13-10 Reserved    0000
                         9 1 = DSN feature set is supported    0
                         8 1 = Accessible Max Address Configuration feature set is supported    0
                         7 1 = Extended Power Conditions feature set is supported    0
                         6 1 = Sense Data Reporting feature set is supported    0
                         5 1 = The Free-fall Control feature set is supported    0
                         4 1 = The DOWNLOAD MICROCODE command with mode 3 is supported    1
                         3 1 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported    1
                         2 1 = The WRITE UNCORRECTABLE EXT command is supported    0
                         1 1 = The Write-Read-Verify feature set is supported    0
                         0 Reserved for Disable Data Transfer    0*/ 
        0x4008,/*Word 120:Commands and feature sets supported or enabled (Continued from Words 87-85)
                                          15 Shall be cleared to zero    0
                        14 Shall be set to one    1
                        13-10 Reserved    0000
                         9 1 = DSN feature set is enabled    0
                         8 Reserved    0
                         7 1 = Extended Power Conditions feature set is enabled    0
                         6 1 = Sense Data Reporting feature set is enabled    0
                         5 1 = The Free-fall Control feature set is enabled    0
                         4 1 = The DOWNLOAD MICROCODE command with mode 3 is supported    1
                         3 1 = The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported    1
                         2 1 = The WRITE UNCORRECTABLE EXT command is supported    0
                         1 1 = The Write-Read-Verify feature set is enabled    0
                         0 Reserved for Disable Data Transfer    0*/
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,/*Word 121-126: Reserved for expanded supported and enable settings*/
        0x0000,/*Word 127:Obsolete*/
        0x0021,/*Word 128:Security status
                         15-9 Reserved    0000000
                         8 Master PassWord Capability 0 = High, 1 = Maximum    0
                         7-6 Reserved    00
                         5 1 = Enhanced security erase supported   1
                         4 1 = Security count expired    0
                         3 1 = Security frozen    0
                         2 1 = Security locked    0
                         1 1 = Security enabled    0
                         0 1 = Security supported    1*/
        VENDOR_SPEC,/*Word 129-159:Vendor specific*/
        0x0000,/*Word 160: CFA power mode 1
                            15 Word 160 supported 0
                            14 Reserved 0
                            13 CFA power mode 1 is required for one or more commands implemented by the device 0
                            12 CFA power mode 1 disabled 0
                            11-0 Maximum current in MA 000000000000*/
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,/*Word 161-167:Reserved for assignment by the CompactFlashTM Association*/
        0x0003,/*Word 168:Device Nominal Form Factor
                                  15-4 Reserved
                                  3-0 Device Nominal Form Factor 产品尺寸03：2.5英寸*/
        0x0000,/*Word 169:DATA SET MANAGEMENT command is supported
                    15-1 Reserved
                    0 1 = the Trim bit in the DATA SET MANAGEMENT command is supported 1*/
        0x2020, 0x2020, 0x2020, 0x2020,/*Word 170-173:Additional Product Identifier(ATA String)*/
        0x0000, 0x0000,/*Word 174-175:  Reserved*/
        CMSM,/*Word 176:Current media serial number (60 ASCII characters)*/
        0x0000,/*Word 206:SCT Command Transport
                     15-12 Vender Specific    0000
                     11-6 Reserved    000000
                        5 SCT Command Transport Data Tables support    1
                        4 SCT Command Transport Features Control supported                        1
                        3 SCT Command Transport Error Recovery control supported    1
                        2 SCT Command Transport Write Same supported    0
                        1 SCT Command Transport Long Sct Access supported    0
                        0 SCT Command Transport supported    1*/
        0x0000, 0x0000,/*Word 207-208:  Reserved*/
        0x4000,/*Word 209:Alignment of logical blocks within a larger physical block                              
                     15 Shall be cleared to zero 0
                     14 Shall be set to one 1
                     13-0 'Logical Sct' offset within the first physical Sct where the first logical Sct is placed. */
        0x0000,0x0000,/*Word 210-211:Write-Read-Verify Sct Count Mode 3 Only*/                                 
        0x0000,0x0000,/*Word 212-213:Write-Read-Verify Sct Count Mode 2 Only*/
        0x0000,/*Word 214: NV Cache Capabilities
                           15-12 NV Cache feature set version    0000
                           11-8 NV Cache Power Mode feature set version    0000
                           7-5 Reserved    000
                           4 1 = NV Cache feature set enabled    0
                           3-2 Reserved    00
                           1 1 = NV Cache Power mode feature set enabled    0
                           0 1 = NV Cache Power Mode feature set supported    0*/
        0x0000,/*Word 215: NV Cache Size in Logical Blocks (LSW)*/
        0x0000,/*Word 216: NV Cache Size in Logical Blocks (MSW)*/
        0x0001,/*Word 217:Nominal media rotation rate (ATA8-ACS 1699-D Revision 6)
                             NV Cache Read Transfer Speed in MB/s (ATA8-ACS 1699-D Revision 3f)*/
        0x0000,/*Word 218:NV Cache Write Transfer Speed in MB/s*/
        0x0000,/*Word 219:NV Cache Options
                          15-8 Reserved
                          7-0 Device Estimated Time to Spin Up in Seconds*/
        0x0000,/*Word 220:       
                          15-8 Reserved
                          7-0 Write-Read-Verify feature set current mode*/
        0x0000,/*Word 221:  Reserved*/
        0x101F,/*Word 222: Transport Major revision number. 0000h or FFFFh = device does not report version
                          15-12 Transport Type - 0 = Parallel, 1 = Serial, 2-15 = Reserved 0001
                                Parallel (Type = 0)                Serial (Type = 1)    
                          11-8 Reserved                           Reserved    0000
                             7 Reserved                           SATA Rev 3.2    0
                             6 Reserved                           SATA Rev 3.1    1
                             5 Reserved                           SATA Rev 3.0     1
                             4 Reserved                           SATA Rev 2.6    1
                             3 Reserved                           SATA Rev 2.5    1
                             2 Reserved                           SATA II: Extensions    1
                             1 ATA/ATAPI-7                        SATA 1.0a    1
                             0 ATA8-APT                           ATA8-AST    1*/
        0x0000,/*Word 223:Transport Minor revision number not report version*/
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,/*Word 224-229:  Reserved for CE-ATA*/
        0x0000, 0x0000, 0x0000, 0x0000,/*Word 230:  Extended Number of User Addressable Scts(QWord)*/
        0x0010,/*Word 234: Minimum number of 512 U8 units per DOWNLOAD MICROCODE command mode 3*/
        0x0010,/*Word 235:  Maximum number of 512 U8 units per DOWNLOAD MICROCODE command mode 3*/

        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /*Word 236-254:  Reserved*/
        0x00A5},/*Word 255:Integrity Word
                             15-8 Checksum
                              7-0 Checksum Validity Indicator*/
};
#endif
// *****************************************************************************
// 函数名称: Update_IdfyParam
// 函数功能: Update_IdfyParam
// 入口参数: NONE
// 出口参数: NONE
// 说    明:
// *****************************************************************************
void Update_IdfyParam()
{
    //从IDENTIFY获取设备容量信息（48 bit容量)
    tSataDC.dDevCap=((U32)GetWord(100) + (((U32)GetWord(101))<<16));
    /*+((U64)GetWord(102)<<32) +((U64)GetWord(103)<<48)*///32位LBA地址能包含2T的数据量，所以这里只使用低32位
    //TODO:如果容量大于28bit(127G),需要把WORD60 WORD61 赋值0xFFFFFFF

#ifndef __ALG_FORMAL
    //省电模式
    //NOTE: 量产工具生成的表格有问题，由于修正配置的代码 --将来发布的时候要删除
    #ifndef __BOOT_CODE__
        #if (L_PWN_MNG_EN == 1)
                #if (L_HIPM_EN == 1)
                    tSataDC.bIsHIPMEn=TRUE;
                    //SetBits(unAtaIdentify.wArray+76,(U8)9,(U8)1,(U16)1);//support hi
                    unAtaIdentify.wArray[76]|=(W_BIT9);
                #endif //end (L_HIPM_EN == 1)

                #if (L_DIPM_EN == 1)
                    //SetBits(unAtaIdentify.wArray+78,(U8)3,(U8)1,(U16)1);//support di
                    unAtaIdentify.wArray[78]|=(W_BIT3);
                    //SetBits(unAtaIdentify.wArray+79,(U8)3,(U8)1,(U16)0);//disable di
                    unAtaIdentify.wArray[79]&=~(W_BIT3);
                #endif //end (L_DIPM_EN == 1)
                //SetBits(unAtaIdentify.wArray+76,(U8)13,(U8)2,(U16)3);//SUPPORT auto s/p
                unAtaIdentify.wArray[76]&=(~(0x7F<<12));
                unAtaIdentify.wArray[76]|=(3<<12);

         #else   //(L_PWN_MNG_EN == 0)
                unAtaIdentify.wArray[76]=0x990e;//NOTE: 量产工具生成的表格有问题，由于修正配置的代码 --将来发布的时候要删除
                unAtaIdentify.wArray[78]=0x0004;//NOTE: 量产工具生成的表格有问题，由于修正配置的代码 --将来发布的时候要删除
        #endif //(L_PWN_MNG_EN == 1)
    #endif
     if(tSataDC.dDevCap >= 16515072U)
     {
         unAtaIdentify.wArray[1] = 0x3FFF;
     }
     else
     {
         unAtaIdentify.wArray[1] = tSataDC.dDevCap>>0x10;
     }
     // unAtaIdentify.wArray[76]&=(~W_BIT8);//not support ncq
     tSataDC.dDevCap=((U32)GetWord(100) + (((U32)GetWord(101))<<16));
#endif

     tSataDC.bDrq=1;//(U8)DRQ_SIZE;//默认512
     unAtaIdentify.wArray[75]=(MAX_CMD_NUM-1);//配置NCQ深度

     #if (D2H_AUTO_SEND == TRUE)
         SG_DSATA->rTOTALCAPCITY = tSataDC.dDevCap;
         SG_DSATA->rMAXTAG.dAll=GetWord(75);
     #endif
         if(tSataDC.bIsDIPMEn==TRUE)
         {
            //SetBits(unAtaIdentify.wArray+79,(U8)3,(U8)1,(U16)1);
            unAtaIdentify.wArray[79]|=(W_BIT3);
         }

         tSataDC.dMaxLbaAddr4User=tSataDC.dDevCap;//TODO:这个配置需要掉电保存,到时候需要从支持SSP定义的表格中获取
         UART_PRINT("22222222unAtaIdentify.wArray[79]=%x\r\n",unAtaIdentify.wArray[79]);

         //版本上报
         if(unAtaIdentify.bArray[290] == 1)
         {
        	 unAtaIdentify.bArray[308] = 0xE;
         }

         if(unAtaIdentify.bArray[290] == 0)
         {
        	 unAtaIdentify.bArray[308] = 0xC;
         }

         //unAtaIdentify.wArray[76]&=(~W_BIT8);//not support ncq
}
#if 0
int GetBit(const U16* wAddr,U8 bB_offset)
{
    return (((*wAddr)&(1<<bB_offset))>>bB_offset);
}

void SetBit(U16* wAddr,U8 bB_offset,U8 val)
{
    *wAddr=((*wAddr)&(~(1<<bB_offset)))|(val<<bB_offset);
}

// *****************************************************************************
// 函数名称: GetBits
// 函数功能: 获取指定字的一位或者连续多位数据  
// 入口参数: 
//             @addr    :字地址
//             @b_offset:位偏移
//             @width   :位宽
// 出口参数: 指定数据区域的值（BCD码）
// 说    明: 获取指定字的一位或者连续多位数据  
//             0000001100000100
//                |width=3|
//                b_offset=10
//                val=011=3
// *****************************************************************************
int GetBits(const U16* wAddr,U8 bB_offset,U8 bWidth)
{
    U16 wMask;
    U16 wRet;
    if((0 == bWidth) ||
       (bB_offset+1) < (bWidth-1))
    {
        //error
        return -1;
    }
    //根据位宽算出屏蔽码
    wMask=(U16)(1<<bWidth)-1;//ffff
    
    wRet=((*wAddr)>>((bB_offset-bWidth)+1)) & wMask;
    return wRet;
}

// *****************************************************************************
// 函数名称: SetBits
// 函数功能: 设置指定字的一位或者连续多位数据
// 入口参数: 
//             @addr    :字地址
//           @b_offset:位偏移
//           @width   :位宽
//           @value   :新值
// 出口参数: 0        :OK
//             OTHER  :NG  参数出错
// 说    明:  
// *****************************************************************************
U8 SetBits(U16* wAddr,U8 bB_offset,U8 bWidth,U16 wValue)
{
    U16 wMask;
    U16 wTmpVal;

   if((0 == bWidth) || 
     ((bB_offset+1U) < (bWidth-1U)))
    {
        //error
        return 0xFF;
    }
    //根据位宽计算屏蔽码
    wMask=(U16)(~(((1U<<bWidth)-1U)<<((bB_offset+1U)-bWidth)));
    //清除原始数据
    wTmpVal=(*wAddr) & wMask;
    //赋新值
    (*wAddr) =(U16)(wTmpVal | ((U32)wValue<<((bB_offset+1U)-bWidth)));
     return 0;
}
#endif

void UpdateCheckSum(U8* bAddr,U16 wCount)
{

	/*7.12.7.91 Word 255: Integrity word
		If bits 7:0 of this word contain the Checksum Validity Indicator A5h, then bits 15:8 contain the data structure
		checksum. The data structure checksum is the two’s complement of the sum of all bytes in words 0..254 and the
		byte consisting of bits 7:0 in word 255. Each byte shall be added with unsigned arithmetic, and overflow shall be
		ignored. The sum of all 512 bytes is zero if the checksum is correct.
	*/
    //bAddr[wCount-1]=(U8)(256-(U8)(GetSum(bAddr,wCount-2)+0xA5));
	//二进制补码
    bAddr[wCount-1]=(~GetSum(bAddr,wCount-1)) + 1;
}
