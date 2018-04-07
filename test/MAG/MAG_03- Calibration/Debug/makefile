################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/Senscape/cosmic/tools/msp430

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./MAG_03.o" \
"./main.o" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lm \
-lc \
-lgcc \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
MAG_03.out \

EXE_OUTPUTS__QUOTED += \
"MAG_03.out" \

BIN_OUTPUTS += \
MAG_03.hex \

BIN_OUTPUTS__QUOTED += \
"MAG_03.hex" \


# All Target
all: MAG_03.out

dependents:
	-cd C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/MAG_lib/Debug && $(MAKE) all
	-cd C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/SENSFUS_lib/Debug && $(MAKE) all
	-cd C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/ADXL377_lib/Debug && $(MAKE) all

# Tool invocations
MAG_03.out: $(OBJS) $(A_SRCS) $(GEN_CMDS) C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/MAG_lib/Debug/libMAG_lib.a C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/SENSFUS_lib/Debug/libSENSFUS_lib.a C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/ADXL377_lib/Debug/libADXL377_lib.a
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"C:/Senscape/cosmic/tools/msp430/bin/msp430-gcc.exe" -DF_CPU=16000000L -DSENSCAPE=100 -Os -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -std=c99 -lm -mmcu=msp430f2617 -Wl,-Map,"MAG_03.map" -Wl,--gc-sections -Wl,--undefined,main -o"MAG_03.out" $(ORDERED_OBJS) $(A_SRCS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

MAG_03.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: GNU Objcopy Utility'
	"C:/Senscape/cosmic/tools/msp430/bin/msp430-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "MAG_03.hex"
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "MAG_03.o" "main.o" 
	-$(RM) "MAG_03.d" "main.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-C:/Senscape/cosmic/tools/msp430/bin/msp430-objcopy -O ihex -R .eeprom MAG_03.out MAG_03.hex
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/MAG_lib/Debug/libMAG_lib.a:
C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/SENSFUS_lib/Debug/libSENSFUS_lib.a:
C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/ADXL377_lib/Debug/libADXL377_lib.a:

-include ../makefile.targets

