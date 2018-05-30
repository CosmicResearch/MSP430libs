################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/Senscape/cosmic/tools/msp430/bin/msp430-gcc.exe" -c -mmcu=msp430f2617 -DF_CPU=16000000L -DSENSCAPE=100 -I"C:/Senscape/cosmic/hardware/msp430/cores/msp430f2617" -I"C:/Senscape/cosmic/hardware/libraries/MAG" -I"C:/Senscape/cosmic/hardware/libraries/ADXL377" -I"C:/Senscape/cosmic/hardware/libraries/ADC" -I"C:/Senscape/cosmic/hardware/libraries/SENSFUS" -I"C:/Senscape/cosmic/hardware/msp430/cores/msp430f2617/drivers" -I"C:/Senscape/cosmic/hardware/msp430/cores/msp430f2617/interfaces" -I"C:/Senscape/cosmic/hardware/msp430/variants/cosmic" -I"C:/Senscape/cosmic/tools/msp430" -Os -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -std=c99 -lm -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


