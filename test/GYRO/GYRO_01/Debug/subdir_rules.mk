################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"C:/Senscape/CosmicResearch/cosmic/tools/msp430/bin/msp430-gcc.exe" -c -mcpu=430x -mmcu=msp430f2617 -I"C:/ti/ccsv6/ccs_base/msp430/include_gcc" -I"C:/Senscape/CosmicResearch/cosmic/tools/msp430/msp430/include" -Os -g -gdwarf-3 -gstrict-dwarf -Wall -mlarge -mhwmult=16bit -mcode-region=none -mdata-region=none -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


