################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/common/core/BMI270-Sensor-API/bmi2.c \
../src/common/core/BMI270-Sensor-API/bmi270.c \
../src/common/core/BMI270-Sensor-API/bmi270_context.c \
../src/common/core/BMI270-Sensor-API/bmi270_hc.c \
../src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.c \
../src/common/core/BMI270-Sensor-API/bmi270_wh.c \
../src/common/core/BMI270-Sensor-API/bmi2_ois.c 

OBJS += \
./src/common/core/BMI270-Sensor-API/bmi2.o \
./src/common/core/BMI270-Sensor-API/bmi270.o \
./src/common/core/BMI270-Sensor-API/bmi270_context.o \
./src/common/core/BMI270-Sensor-API/bmi270_hc.o \
./src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.o \
./src/common/core/BMI270-Sensor-API/bmi270_wh.o \
./src/common/core/BMI270-Sensor-API/bmi2_ois.o 

C_DEPS += \
./src/common/core/BMI270-Sensor-API/bmi2.d \
./src/common/core/BMI270-Sensor-API/bmi270.d \
./src/common/core/BMI270-Sensor-API/bmi270_context.d \
./src/common/core/BMI270-Sensor-API/bmi270_hc.d \
./src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.d \
./src/common/core/BMI270-Sensor-API/bmi270_wh.d \
./src/common/core/BMI270-Sensor-API/bmi2_ois.d 


# Each subdirectory must supply rules for building sources it contributes
src/common/core/BMI270-Sensor-API/%.o src/common/core/BMI270-Sensor-API/%.su src/common/core/BMI270-Sensor-API/%.cyclo: ../src/common/core/BMI270-Sensor-API/%.c src/common/core/BMI270-Sensor-API/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -c -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/ap" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/bsp" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/config" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/rx" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/scheduler" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/sensors" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/hw/include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/barometer" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/compass" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/gps" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-common-2f-core-2f-BMI270-2d-Sensor-2d-API

clean-src-2f-common-2f-core-2f-BMI270-2d-Sensor-2d-API:
	-$(RM) ./src/common/core/BMI270-Sensor-API/bmi2.cyclo ./src/common/core/BMI270-Sensor-API/bmi2.d ./src/common/core/BMI270-Sensor-API/bmi2.o ./src/common/core/BMI270-Sensor-API/bmi2.su ./src/common/core/BMI270-Sensor-API/bmi270.cyclo ./src/common/core/BMI270-Sensor-API/bmi270.d ./src/common/core/BMI270-Sensor-API/bmi270.o ./src/common/core/BMI270-Sensor-API/bmi270.su ./src/common/core/BMI270-Sensor-API/bmi270_context.cyclo ./src/common/core/BMI270-Sensor-API/bmi270_context.d ./src/common/core/BMI270-Sensor-API/bmi270_context.o ./src/common/core/BMI270-Sensor-API/bmi270_context.su ./src/common/core/BMI270-Sensor-API/bmi270_hc.cyclo ./src/common/core/BMI270-Sensor-API/bmi270_hc.d ./src/common/core/BMI270-Sensor-API/bmi270_hc.o ./src/common/core/BMI270-Sensor-API/bmi270_hc.su ./src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.cyclo ./src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.d ./src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.o ./src/common/core/BMI270-Sensor-API/bmi270_maximum_fifo.su ./src/common/core/BMI270-Sensor-API/bmi270_wh.cyclo ./src/common/core/BMI270-Sensor-API/bmi270_wh.d ./src/common/core/BMI270-Sensor-API/bmi270_wh.o ./src/common/core/BMI270-Sensor-API/bmi270_wh.su ./src/common/core/BMI270-Sensor-API/bmi2_ois.cyclo ./src/common/core/BMI270-Sensor-API/bmi2_ois.d ./src/common/core/BMI270-Sensor-API/bmi2_ois.o ./src/common/core/BMI270-Sensor-API/bmi2_ois.su

.PHONY: clean-src-2f-common-2f-core-2f-BMI270-2d-Sensor-2d-API

