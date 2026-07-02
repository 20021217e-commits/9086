################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/development/Automation/9086BB/9086/SysCfg/src/startup/init-default.c \
D:/development/Automation/9086BB/9086/SysCfg/src/startup/init-soc.c 

S_UPPER_SRCS += \
D:/development/Automation/9086BB/9086/SysCfg/src/startup/crt0.S \
D:/development/Automation/9086BB/9086/SysCfg/src/startup/crtbegin.S \
D:/development/Automation/9086BB/9086/SysCfg/src/startup/crtend.S 

OBJS += \
./SysCfg/src/startup/crt0.o \
./SysCfg/src/startup/crtbegin.o \
./SysCfg/src/startup/crtend.o \
./SysCfg/src/startup/init-default.o \
./SysCfg/src/startup/init-soc.o 

C_DEPS += \
./SysCfg/src/startup/init-default.d \
./SysCfg/src/startup/init-soc.d 

S_UPPER_DEPS += \
./SysCfg/src/startup/crt0.d \
./SysCfg/src/startup/crtbegin.d \
./SysCfg/src/startup/crtend.d 


# Each subdirectory must supply rules for building sources it contributes
SysCfg/src/startup/crt0.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/startup/crt0.S
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/startup/crtbegin.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/startup/crtbegin.S
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/startup/crtend.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/startup/crtend.S
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/startup/init-default.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/startup/init-default.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/startup/init-soc.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/startup/init-soc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


