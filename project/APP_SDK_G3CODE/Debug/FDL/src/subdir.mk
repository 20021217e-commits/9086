################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/Micron_WL_Addr.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_PValueSet.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_common.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_copyback.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_debug.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_erase_block.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_exception.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_irq.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_mcu_op.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_read.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_read_retry.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_soft_decode.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_task_op.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_write.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_write_fail_handler.c \
D:/development/Automation/9086BB/9086/FlashDriver/FDL/src/flash_driver.c 

OBJS += \
./FDL/src/Micron_WL_Addr.o \
./FDL/src/fdl_PValueSet.o \
./FDL/src/fdl_common.o \
./FDL/src/fdl_copyback.o \
./FDL/src/fdl_debug.o \
./FDL/src/fdl_erase_block.o \
./FDL/src/fdl_exception.o \
./FDL/src/fdl_irq.o \
./FDL/src/fdl_mcu_op.o \
./FDL/src/fdl_read.o \
./FDL/src/fdl_read_retry.o \
./FDL/src/fdl_soft_decode.o \
./FDL/src/fdl_task_op.o \
./FDL/src/fdl_write.o \
./FDL/src/fdl_write_fail_handler.o \
./FDL/src/flash_driver.o 

C_DEPS += \
./FDL/src/Micron_WL_Addr.d \
./FDL/src/fdl_PValueSet.d \
./FDL/src/fdl_common.d \
./FDL/src/fdl_copyback.d \
./FDL/src/fdl_debug.d \
./FDL/src/fdl_erase_block.d \
./FDL/src/fdl_exception.d \
./FDL/src/fdl_irq.d \
./FDL/src/fdl_mcu_op.d \
./FDL/src/fdl_read.d \
./FDL/src/fdl_read_retry.d \
./FDL/src/fdl_soft_decode.d \
./FDL/src/fdl_task_op.d \
./FDL/src/fdl_write.d \
./FDL/src/fdl_write_fail_handler.d \
./FDL/src/flash_driver.d 


# Each subdirectory must supply rules for building sources it contributes
FDL/src/Micron_WL_Addr.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/Micron_WL_Addr.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_PValueSet.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_PValueSet.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_common.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_copyback.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_copyback.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_debug.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_debug.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_erase_block.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_erase_block.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_exception.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_exception.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_irq.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_irq.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_mcu_op.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_mcu_op.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_read.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_read.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_read_retry.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_read_retry.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_soft_decode.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_soft_decode.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_task_op.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_task_op.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_write.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_write.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/fdl_write_fail_handler.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/fdl_write_fail_handler.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FDL/src/flash_driver.o: /cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/src/flash_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DIS_ALIGN_RD__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__DUAL_CORE__ -D__FILL_PAGE_EN__11 -D__FDL_HW_VIRTUL_CLST__ -D__FDL_CQFIFO_TRIG_IRQ__ -D__FDL_CRC_FUNC__ -D__G3_FLYCODE -D__SW_PMCP__11 -D__TLC_RD_VERIFY__11 -D__FLASH_SUMSUNG_3DV4__ -D__FPGA_PLATEFORM_OFF -U__CAP_512G_4CHAN_4CE_SUPPORT__ -U__FPGA_PLATEFORM -U__APP_CACHE_READ__ -U__YS9083XT_ALGO__ -U__SATA_LOG_EN__ -U__VMC_EN__ -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G3CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/FlashDriver/FDL/Interface" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/APP_HAL_Lib/HAL_Lib/interface" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -Os -mcmodel=large -g3 -Wall -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


