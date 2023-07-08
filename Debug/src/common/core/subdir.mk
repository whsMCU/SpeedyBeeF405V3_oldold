################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/common/core/core.c \
../src/common/core/dyn_notch_filter.c \
../src/common/core/explog_approx.c \
../src/common/core/filter.c \
../src/common/core/gps_conversion.c \
../src/common/core/gyro_filter_impl.c \
../src/common/core/maths.c \
../src/common/core/pid.c \
../src/common/core/pid_init.c \
../src/common/core/ring_buffer.c \
../src/common/core/runtime_config.c \
../src/common/core/sdft.c \
../src/common/core/sensors.c \
../src/common/core/telemetry.c 

OBJS += \
./src/common/core/core.o \
./src/common/core/dyn_notch_filter.o \
./src/common/core/explog_approx.o \
./src/common/core/filter.o \
./src/common/core/gps_conversion.o \
./src/common/core/gyro_filter_impl.o \
./src/common/core/maths.o \
./src/common/core/pid.o \
./src/common/core/pid_init.o \
./src/common/core/ring_buffer.o \
./src/common/core/runtime_config.o \
./src/common/core/sdft.o \
./src/common/core/sensors.o \
./src/common/core/telemetry.o 

C_DEPS += \
./src/common/core/core.d \
./src/common/core/dyn_notch_filter.d \
./src/common/core/explog_approx.d \
./src/common/core/filter.d \
./src/common/core/gps_conversion.d \
./src/common/core/gyro_filter_impl.d \
./src/common/core/maths.d \
./src/common/core/pid.d \
./src/common/core/pid_init.d \
./src/common/core/ring_buffer.d \
./src/common/core/runtime_config.d \
./src/common/core/sdft.d \
./src/common/core/sensors.d \
./src/common/core/telemetry.d 


# Each subdirectory must supply rules for building sources it contributes
src/common/core/%.o src/common/core/%.su src/common/core/%.cyclo: ../src/common/core/%.c src/common/core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DSTM32F40_41xxx -c -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/ap" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/bsp" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/config" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/pg" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/rx" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/scheduler" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/sensors" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/hw/include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/barometer" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/compass" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/gps" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-common-2f-core

clean-src-2f-common-2f-core:
	-$(RM) ./src/common/core/core.cyclo ./src/common/core/core.d ./src/common/core/core.o ./src/common/core/core.su ./src/common/core/dyn_notch_filter.cyclo ./src/common/core/dyn_notch_filter.d ./src/common/core/dyn_notch_filter.o ./src/common/core/dyn_notch_filter.su ./src/common/core/explog_approx.cyclo ./src/common/core/explog_approx.d ./src/common/core/explog_approx.o ./src/common/core/explog_approx.su ./src/common/core/filter.cyclo ./src/common/core/filter.d ./src/common/core/filter.o ./src/common/core/filter.su ./src/common/core/gps_conversion.cyclo ./src/common/core/gps_conversion.d ./src/common/core/gps_conversion.o ./src/common/core/gps_conversion.su ./src/common/core/gyro_filter_impl.cyclo ./src/common/core/gyro_filter_impl.d ./src/common/core/gyro_filter_impl.o ./src/common/core/gyro_filter_impl.su ./src/common/core/maths.cyclo ./src/common/core/maths.d ./src/common/core/maths.o ./src/common/core/maths.su ./src/common/core/pid.cyclo ./src/common/core/pid.d ./src/common/core/pid.o ./src/common/core/pid.su ./src/common/core/pid_init.cyclo ./src/common/core/pid_init.d ./src/common/core/pid_init.o ./src/common/core/pid_init.su ./src/common/core/ring_buffer.cyclo ./src/common/core/ring_buffer.d ./src/common/core/ring_buffer.o ./src/common/core/ring_buffer.su ./src/common/core/runtime_config.cyclo ./src/common/core/runtime_config.d ./src/common/core/runtime_config.o ./src/common/core/runtime_config.su ./src/common/core/sdft.cyclo ./src/common/core/sdft.d ./src/common/core/sdft.o ./src/common/core/sdft.su ./src/common/core/sensors.cyclo ./src/common/core/sensors.d ./src/common/core/sensors.o ./src/common/core/sensors.su ./src/common/core/telemetry.cyclo ./src/common/core/telemetry.d ./src/common/core/telemetry.o ./src/common/core/telemetry.su

.PHONY: clean-src-2f-common-2f-core

