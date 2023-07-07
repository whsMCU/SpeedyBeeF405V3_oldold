################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/common/core/sensors/acceleration.c \
../src/common/core/sensors/acceleration_init.c \
../src/common/core/sensors/barometer.c \
../src/common/core/sensors/compass.c \
../src/common/core/sensors/gyro.c \
../src/common/core/sensors/gyro_init.c 

OBJS += \
./src/common/core/sensors/acceleration.o \
./src/common/core/sensors/acceleration_init.o \
./src/common/core/sensors/barometer.o \
./src/common/core/sensors/compass.o \
./src/common/core/sensors/gyro.o \
./src/common/core/sensors/gyro_init.o 

C_DEPS += \
./src/common/core/sensors/acceleration.d \
./src/common/core/sensors/acceleration_init.d \
./src/common/core/sensors/barometer.d \
./src/common/core/sensors/compass.d \
./src/common/core/sensors/gyro.d \
./src/common/core/sensors/gyro_init.d 


# Each subdirectory must supply rules for building sources it contributes
src/common/core/sensors/%.o src/common/core/sensors/%.su src/common/core/sensors/%.cyclo: ../src/common/core/sensors/%.c src/common/core/sensors/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -c -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/ap" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/bsp" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/config" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/pg" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/rx" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/scheduler" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/sensors" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/hw/include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/barometer" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/compass" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/gps" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-common-2f-core-2f-sensors

clean-src-2f-common-2f-core-2f-sensors:
	-$(RM) ./src/common/core/sensors/acceleration.cyclo ./src/common/core/sensors/acceleration.d ./src/common/core/sensors/acceleration.o ./src/common/core/sensors/acceleration.su ./src/common/core/sensors/acceleration_init.cyclo ./src/common/core/sensors/acceleration_init.d ./src/common/core/sensors/acceleration_init.o ./src/common/core/sensors/acceleration_init.su ./src/common/core/sensors/barometer.cyclo ./src/common/core/sensors/barometer.d ./src/common/core/sensors/barometer.o ./src/common/core/sensors/barometer.su ./src/common/core/sensors/compass.cyclo ./src/common/core/sensors/compass.d ./src/common/core/sensors/compass.o ./src/common/core/sensors/compass.su ./src/common/core/sensors/gyro.cyclo ./src/common/core/sensors/gyro.d ./src/common/core/sensors/gyro.o ./src/common/core/sensors/gyro.su ./src/common/core/sensors/gyro_init.cyclo ./src/common/core/sensors/gyro_init.d ./src/common/core/sensors/gyro_init.o ./src/common/core/sensors/gyro_init.su

.PHONY: clean-src-2f-common-2f-core-2f-sensors

