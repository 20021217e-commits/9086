################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/development/Automation/9086BB/9086/SysCfg/src/platform/debug.c \
D:/development/Automation/9086BB/9086/SysCfg/src/platform/mcu.c \
D:/development/Automation/9086BB/9086/SysCfg/src/platform/pad.c \
D:/development/Automation/9086BB/9086/SysCfg/src/platform/system_sg9081.c \
D:/development/Automation/9086BB/9086/SysCfg/src/platform/uart.c \
D:/development/Automation/9086BB/9086/SysCfg/src/platform/uitility.c 

OBJS += \
./SysCfg/src/platform/debug.o \
./SysCfg/src/platform/mcu.o \
./SysCfg/src/platform/pad.o \
./SysCfg/src/platform/system_sg9081.o \
./SysCfg/src/platform/uart.o \
./SysCfg/src/platform/uitility.o 

C_DEPS += \
./SysCfg/src/platform/debug.d \
./SysCfg/src/platform/mcu.d \
./SysCfg/src/platform/pad.d \
./SysCfg/src/platform/system_sg9081.d \
./SysCfg/src/platform/uart.d \
./SysCfg/src/platform/uitility.d 


# Each subdirectory must supply rules for building sources it contributes
SysCfg/src/platform/debug.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/platform/debug.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/platform/mcu.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/platform/mcu.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/platform/pad.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/platform/pad.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/platform/system_sg9081.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/platform/system_sg9081.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/platform/uart.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/platform/uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysCfg/src/platform/uitility.o: /cygdrive/D/development/Automation/9086BB/9086/SysCfg/src/platform/uitility.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


