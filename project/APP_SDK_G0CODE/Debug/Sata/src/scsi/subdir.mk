################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/development/Automation/9086BB/9086/Sata/src/scsi/scsi.c 

OBJS += \
./Sata/src/scsi/scsi.o 

C_DEPS += \
./Sata/src/scsi/scsi.d 


# Each subdirectory must supply rules for building sources it contributes
Sata/src/scsi/scsi.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/scsi/scsi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


