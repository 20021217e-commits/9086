################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_cfa.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_cmd_list.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_dco.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_fpdma.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_general.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_gpl.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_hpa.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_identify.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_power_mg.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_sanitize.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_security.c \
D:/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_smart.c 

OBJS += \
./Sata/src/ata_cmd/ata_cfa.o \
./Sata/src/ata_cmd/ata_cmd_list.o \
./Sata/src/ata_cmd/ata_dco.o \
./Sata/src/ata_cmd/ata_fpdma.o \
./Sata/src/ata_cmd/ata_general.o \
./Sata/src/ata_cmd/ata_gpl.o \
./Sata/src/ata_cmd/ata_hpa.o \
./Sata/src/ata_cmd/ata_identify.o \
./Sata/src/ata_cmd/ata_power_mg.o \
./Sata/src/ata_cmd/ata_sanitize.o \
./Sata/src/ata_cmd/ata_security.o \
./Sata/src/ata_cmd/ata_smart.o 

C_DEPS += \
./Sata/src/ata_cmd/ata_cfa.d \
./Sata/src/ata_cmd/ata_cmd_list.d \
./Sata/src/ata_cmd/ata_dco.d \
./Sata/src/ata_cmd/ata_fpdma.d \
./Sata/src/ata_cmd/ata_general.d \
./Sata/src/ata_cmd/ata_gpl.d \
./Sata/src/ata_cmd/ata_hpa.d \
./Sata/src/ata_cmd/ata_identify.d \
./Sata/src/ata_cmd/ata_power_mg.d \
./Sata/src/ata_cmd/ata_sanitize.d \
./Sata/src/ata_cmd/ata_security.d \
./Sata/src/ata_cmd/ata_smart.d 


# Each subdirectory must supply rules for building sources it contributes
Sata/src/ata_cmd/ata_cfa.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_cfa.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_cmd_list.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_cmd_list.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_dco.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_dco.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_fpdma.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_fpdma.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_general.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_general.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_gpl.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_gpl.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_hpa.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_hpa.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_identify.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_identify.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_power_mg.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_power_mg.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_sanitize.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_sanitize.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_security.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_security.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sata/src/ata_cmd/ata_smart.o: /cygdrive/D/development/Automation/9086BB/9086/Sata/src/ata_cmd/ata_smart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__DUAL_CORE__ -D__JUPITER_DEBUG__ -DSATA_DRIVE_NEW -D__G0_CODE -D__SATA_LOG_EN__OFF -U__FPGA_PLATEFORM -I"/cygdrive/D/development/Automation/9086BB/9086/APP_SDK_G0CODE/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/ata_cmd" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/startup" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc/platform" -I"/cygdrive/D/development/Automation/9086BB/9086/SysCfg/inc" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/driver" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/sata_protocol" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/scsi" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_manage" -I"/cygdrive/D/development/Automation/9086BB/9086/Sata/inc/task_process" -Os -mcmodel=large -g -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


