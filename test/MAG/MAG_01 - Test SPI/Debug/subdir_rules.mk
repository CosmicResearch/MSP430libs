################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"C:/Senscape/firmware/tools/msp430/bin/msp430-gcc.exe" -c -mmcu=msp430f2617 -DF_CPU=16000000L -DSENSCAPE=100 -I"C:/Senscape/cosmic/hardware/msp430/cores/msp430f2617" -I"C:/Senscape/cosmic/hardware/libraries/MAG" -I"C:/Senscape/cosmic/hardware/msp430/cores/msp430f2617/drivers" -I"C:/Senscape/cosmic/hardware/msp430/cores/msp430f2617/interfaces" -I"C:/Senscape/cosmic/hardware/msp430/variants/cosmic" -I"C:/Senscape/firmware/tools/msp430" -Os -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


