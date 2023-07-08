################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hw/driver/adc.c \
../src/hw/driver/cdc.c \
../src/hw/driver/cli.c \
../src/hw/driver/fatfs.c \
../src/hw/driver/files.c \
../src/hw/driver/flash.c \
../src/hw/driver/gpio.c \
../src/hw/driver/i2c.c \
../src/hw/driver/led.c \
../src/hw/driver/rtc.c \
../src/hw/driver/sd.c \
../src/hw/driver/spi.c \
../src/hw/driver/timer.c \
../src/hw/driver/uart.c \
../src/hw/driver/usb.c 

OBJS += \
./src/hw/driver/adc.o \
./src/hw/driver/cdc.o \
./src/hw/driver/cli.o \
./src/hw/driver/fatfs.o \
./src/hw/driver/files.o \
./src/hw/driver/flash.o \
./src/hw/driver/gpio.o \
./src/hw/driver/i2c.o \
./src/hw/driver/led.o \
./src/hw/driver/rtc.o \
./src/hw/driver/sd.o \
./src/hw/driver/spi.o \
./src/hw/driver/timer.o \
./src/hw/driver/uart.o \
./src/hw/driver/usb.o 

C_DEPS += \
./src/hw/driver/adc.d \
./src/hw/driver/cdc.d \
./src/hw/driver/cli.d \
./src/hw/driver/fatfs.d \
./src/hw/driver/files.d \
./src/hw/driver/flash.d \
./src/hw/driver/gpio.d \
./src/hw/driver/i2c.d \
./src/hw/driver/led.d \
./src/hw/driver/rtc.d \
./src/hw/driver/sd.d \
./src/hw/driver/spi.d \
./src/hw/driver/timer.d \
./src/hw/driver/uart.d \
./src/hw/driver/usb.d 


# Each subdirectory must supply rules for building sources it contributes
src/hw/driver/%.o src/hw/driver/%.su src/hw/driver/%.cyclo: ../src/hw/driver/%.c src/hw/driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DSTM32F40_41xxx -c -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/ap" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/bsp" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/config" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/pg" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/rx" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/scheduler" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/core/sensors" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/common/hw/include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/barometer" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/compass" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/gps" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"D:/STM32CubeIDE/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-hw-2f-driver

clean-src-2f-hw-2f-driver:
	-$(RM) ./src/hw/driver/adc.cyclo ./src/hw/driver/adc.d ./src/hw/driver/adc.o ./src/hw/driver/adc.su ./src/hw/driver/cdc.cyclo ./src/hw/driver/cdc.d ./src/hw/driver/cdc.o ./src/hw/driver/cdc.su ./src/hw/driver/cli.cyclo ./src/hw/driver/cli.d ./src/hw/driver/cli.o ./src/hw/driver/cli.su ./src/hw/driver/fatfs.cyclo ./src/hw/driver/fatfs.d ./src/hw/driver/fatfs.o ./src/hw/driver/fatfs.su ./src/hw/driver/files.cyclo ./src/hw/driver/files.d ./src/hw/driver/files.o ./src/hw/driver/files.su ./src/hw/driver/flash.cyclo ./src/hw/driver/flash.d ./src/hw/driver/flash.o ./src/hw/driver/flash.su ./src/hw/driver/gpio.cyclo ./src/hw/driver/gpio.d ./src/hw/driver/gpio.o ./src/hw/driver/gpio.su ./src/hw/driver/i2c.cyclo ./src/hw/driver/i2c.d ./src/hw/driver/i2c.o ./src/hw/driver/i2c.su ./src/hw/driver/led.cyclo ./src/hw/driver/led.d ./src/hw/driver/led.o ./src/hw/driver/led.su ./src/hw/driver/rtc.cyclo ./src/hw/driver/rtc.d ./src/hw/driver/rtc.o ./src/hw/driver/rtc.su ./src/hw/driver/sd.cyclo ./src/hw/driver/sd.d ./src/hw/driver/sd.o ./src/hw/driver/sd.su ./src/hw/driver/spi.cyclo ./src/hw/driver/spi.d ./src/hw/driver/spi.o ./src/hw/driver/spi.su ./src/hw/driver/timer.cyclo ./src/hw/driver/timer.d ./src/hw/driver/timer.o ./src/hw/driver/timer.su ./src/hw/driver/uart.cyclo ./src/hw/driver/uart.d ./src/hw/driver/uart.o ./src/hw/driver/uart.su ./src/hw/driver/usb.cyclo ./src/hw/driver/usb.d ./src/hw/driver/usb.o ./src/hw/driver/usb.su

.PHONY: clean-src-2f-hw-2f-driver

