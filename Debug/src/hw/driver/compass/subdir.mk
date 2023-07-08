################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hw/driver/compass/compass_qmc5883l.c 

OBJS += \
./src/hw/driver/compass/compass_qmc5883l.o 

C_DEPS += \
./src/hw/driver/compass/compass_qmc5883l.d 


# Each subdirectory must supply rules for building sources it contributes
src/hw/driver/compass/%.o src/hw/driver/compass/%.su src/hw/driver/compass/%.cyclo: ../src/hw/driver/compass/%.c src/hw/driver/compass/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DSTM32F40_41xxx -c -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/ap" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/bsp" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/config" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/pg" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/rx" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/scheduler" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/sensors" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/hw/include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/barometer" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/compass" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/gps" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-hw-2f-driver-2f-compass

clean-src-2f-hw-2f-driver-2f-compass:
	-$(RM) ./src/hw/driver/compass/compass_qmc5883l.cyclo ./src/hw/driver/compass/compass_qmc5883l.d ./src/hw/driver/compass/compass_qmc5883l.o ./src/hw/driver/compass/compass_qmc5883l.su

.PHONY: clean-src-2f-hw-2f-driver-2f-compass

