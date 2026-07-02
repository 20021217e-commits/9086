################################################################################
# Automatically-generated file. Do not edit!
################################################################################

ifndef ANDESIGHT_ROOT
ANDESIGHT_ROOT=/cygdrive/C/app/AndeSight300RDS
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=nds32le-elf-
endif

ifndef SECONDARY_OUTPUT_PATH
SECONDARY_OUTPUT_PATH=output
endif

$(shell mkdir -p $(SECONDARY_OUTPUT_PATH))

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include FDL/src/subdir.mk
-include HAL_Lib/src/subdir.mk
-include Sata/src/ata_cmd/subdir.mk
-include Sata/src/driver/subdir.mk
-include Sata/src/sata_protocol/subdir.mk
-include Sata/src/scsi/subdir.mk
-include Sata/src/task_manage/subdir.mk
-include Sata/src/task_process/subdir.mk
-include SysCfg/src/platform/subdir.mk
-include SysCfg/src/startup/subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
BUILD_ARTIFACT_FILE_BASE_NAME = APP_SDK_G3CODE

LINKER_OUTPUTS = APP_SDK_G3CODE.adx

GCOV_OUT += \
$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)) \

GPROF_OUT += \
../gmon.sum ../gmon.out \

SYMBOL_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \

READELF_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \

OBJDUMP_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/objdump.txt \

SIZE_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/.PHONY.size \

LDSAG_OUT = $(SECONDARY_OUTPUT_PATH)/nds32.ld

# All Target
all: pre-build 
	$(MAKE) main-build

# Main-build Target
main-build: APP_SDK_G3CODE.adx secondary-outputs

# Tool invocations
APP_SDK_G3CODE.adx: $(OBJS) $(USER_OBJS) $(LDSAG_OUT)
	@echo 'Building target: $@'
	@echo 'Invoking: Andes C Linker'
	$(CROSS_COMPILE)gcc -Os -nostartfiles -static -mcmodel=large -T"$(LDSAG_OUT)" -Wl,--gc-sections -mvh -o "APP_SDK_G3CODE.adx" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)): APP_SDK_G3CODE.adx $(OBJS) $(USER_OBJS) $(LDSAG_OUT)
../gmon.sum ../gmon.out: APP_SDK_G3CODE.adx $(OBJS) $(USER_OBJS) $(LDSAG_OUT)

$(SECONDARY_OUTPUT_PATH)/symbol.txt: $(LINKER_OUTPUTS)
	@echo 'Invoking: NM (symbol listing)'
	$(CROSS_COMPILE)nm -n -l -C "APP_SDK_G3CODE.adx" > $(SECONDARY_OUTPUT_PATH)/symbol.txt
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/readelf.txt: $(LINKER_OUTPUTS)
	@echo 'Invoking: Readelf (ELF info listing)'
	$(CROSS_COMPILE)readelf -a "APP_SDK_G3CODE.adx" > $(SECONDARY_OUTPUT_PATH)/readelf.txt
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/objdump.txt: $(LINKER_OUTPUTS)
	@echo 'Invoking: Objdump (disassembly)'
	$(CROSS_COMPILE)objdump -x -d -C "APP_SDK_G3CODE.adx" > $(SECONDARY_OUTPUT_PATH)/objdump.txt
	@echo 'Finished building: $@'
	@echo ' '

: $(LINKER_OUTPUTS)
	@echo 'Invoking: Objcopy (object content copy)'
	$(CROSS_COMPILE)objcopy -S -O binary "APP_SDK_G3CODE.adx"
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/.PHONY.size: $(LINKER_OUTPUTS)
	@echo 'Invoking: Size (section size listing)'
	$(CROSS_COMPILE)size  "APP_SDK_G3CODE.adx" | tee $(SECONDARY_OUTPUT_PATH)/.PHONY.size
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/nds32.ld: $(SAG_SRCS)
	@echo 'Invoking: LdSaG Tool'
	$(ANDESIGHT_ROOT)/utils/nds_ldsag -t $(ANDESIGHT_ROOT)/utils/nds32_template.txt $(SAG_SRCS) -o $(LDSAG_OUT)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(GCOV_OUT)$(LDSAG_OUT)$(SYMBOL_OUTPUTS)$(OBJDUMP_OUTPUTS)$(READELF_OUTPUTS)$(GPROF_OUT)$(SIZE_OUTPUTS)$(EXECUTABLES)$(S_UPPER_DEPS) APP_SDK_G3CODE.adx
	-@echo ' '

pre-build:
	-sh -x ../CopyBinHex.cmd;../../A1-SourceInsight4.0-DoubleCore/CopyBinHexG3.bat
	-@echo ' '

post-build:
	-sh -x ../CopyBinHex.cmd;../../A1-SourceInsight4.0-DoubleCore/CopyBinHexG3.bat
	-@echo ' '

secondary-outputs: $(SYMBOL_OUTPUTS) $(READELF_OUTPUTS) $(OBJDUMP_OUTPUTS) $(SIZE_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build
.NOTPARALLEL: pre-build

-include ../makefile.targets
