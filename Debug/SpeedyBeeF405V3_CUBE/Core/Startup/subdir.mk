################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../SpeedyBeeF405V3_CUBE/Core/Startup/startup_stm32f405rgtx.s 

OBJS += \
./SpeedyBeeF405V3_CUBE/Core/Startup/startup_stm32f405rgtx.o 

S_DEPS += \
./SpeedyBeeF405V3_CUBE/Core/Startup/startup_stm32f405rgtx.d 


# Each subdirectory must supply rules for building sources it contributes
SpeedyBeeF405V3_CUBE/Core/Startup/%.o: ../SpeedyBeeF405V3_CUBE/Core/Startup/%.s SpeedyBeeF405V3_CUBE/Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-SpeedyBeeF405V3_CUBE-2f-Core-2f-Startup

clean-SpeedyBeeF405V3_CUBE-2f-Core-2f-Startup:
	-$(RM) ./SpeedyBeeF405V3_CUBE/Core/Startup/startup_stm32f405rgtx.d ./SpeedyBeeF405V3_CUBE/Core/Startup/startup_stm32f405rgtx.o

.PHONY: clean-SpeedyBeeF405V3_CUBE-2f-Core-2f-Startup

