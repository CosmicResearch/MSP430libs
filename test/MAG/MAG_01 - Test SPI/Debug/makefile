################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/Senscape/firmware/tools/msp430

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./MAG_01.o" \
"./main.o" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-l"m" \
-l"c" \
-l"gcc" \
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
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
MAG_01.out \

EXE_OUTPUTS__QUOTED += \
"MAG_01.out" \

BIN_OUTPUTS += \
MAG_01.hex \

BIN_OUTPUTS__QUOTED += \
"MAG_01.hex" \


# All Target
all: MAG_01.out

dependents:
	-cd C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/MAG_lib/Debug && $(MAKE) all

# Tool invocations
MAG_01.out: $(OBJS) $(A_SRCS) $(GEN_CMDS) C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/MAG_lib/Debug/libMAG_lib.a
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Linker'
	"C:/Senscape/firmware/tools/msp430/bin/msp430-gcc.exe" -mmcu=msp430f2617 -DF_CPU=16000000L -DSENSCAPE=100 -Os -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -Wl,-Map,"MAG_01.map" -Wl,--gc-sections -Wl,--undefined,main -o"MAG_01.out" $(ORDERED_OBJS) $(A_SRCS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

MAG_01.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: GNU Objcopy Utility'
	"C:/Senscape/firmware/tools/msp430/bin/msp430-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "MAG_01.hex"
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "MAG_01.d" "main.d" 
	-$(RM) "MAG_01.o" "main.o" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-C:/Senscape/firmware/tools/msp430/bin/msp430-objcopy -O ihex -R .eeprom MAG_01.out MAG_01.hex
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
C:/Senscape/cosmic/hardware/msp430/libraries/msp430f2617/MAG_lib/Debug/libMAG_lib.a:

-include ../makefile.targets

