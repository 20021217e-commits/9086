################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_define.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_dev_idle.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_dma_in.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_dma_out.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_fpdma.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_misc.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_pio_in.c \
D:/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_pio_out.c 

OBJS += \
./Sata/src/sata_protocol/sata_define.o \
./Sata/src/sata_protocol/sata_dev_idle.o \
./Sata/src/sata_protocol/sata_dma_in.o \
./Sata/src/sata_protocol/sata_dma_out.o \
./Sata/src/sata_protocol/sata_fpdma.o \
./Sata/src/sata_protocol/sata_misc.o \
./Sata/src/sata_protocol/sata_pio_in.o \
./Sata/src/sata_protocol/sata_pio_out.o 

C_DEPS += \
./Sata/src/sata_protocol/sata_define.d \
./Sata/src/sata_protocol/sata_dev_idle.d \
./Sata/src/sata_protocol/sata_dma_in.d \
./Sata/src/sata_protocol/sata_dma_out.d \
./Sata/src/sata_protocol/sata_fpdma.d \
./Sata/src/sata_protocol/sata_misc.d \
./Sata/src/sata_protocol/sata_pio_in.d \
./Sata/src/sata_protocol/sata_pio_out.d 


# Each subdirectory must supply rules for building sources it contributes
Sata/src/sata_protocol/sata_define.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_define.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_dev_idle.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_dev_idle.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_dma_in.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_dma_in.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_dma_out.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_dma_out.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_fpdma.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_fpdma.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_misc.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_misc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_pio_in.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_pio_in.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/sata_protocol/sata_pio_out.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/sata_protocol/sata_pio_out.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


